/*
 * regexp.cc - Copyright (c) 2024-2025 - Olivier Poncet
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cstdarg>
#include <cmath>
#include <chrono>
#include <thread>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include "regexp.h"

// ---------------------------------------------------------------------------
// RegExp
// ---------------------------------------------------------------------------

RegExp::RegExp()
    : _bytecode()
    , _pattern()
    , _string()
{
}

auto RegExp::compile(const std::string& pattern) -> bool
{
    _pattern = pattern;

    return exec_compile();
}

auto RegExp::compare(const std::string& string) -> bool
{
    _string = string;

    return exec_compare();
}

auto RegExp::exec_compile() -> bool
{
    _bytecode.clear();
    _bytecode.emit_nop();
    expect_expression(_pattern.begin(), _pattern.end());
    _bytecode.emit_ret();

    return true;
}

auto RegExp::exec_compare() -> bool
{
    auto iter = _string.begin();
    auto last = _string.end();

    if(match(_bytecode.begin(), iter) != false) {
        return true;
    }
    while(iter != last) {
        if(match(_bytecode.begin(), iter++) != false) {
            return true;
        }
    }
    return false;
}

auto RegExp::expect_expression(std::string::const_iterator begin, std::string::const_iterator end) -> size_t
{
    auto iterator = begin;

    if(iterator != end) {
        do {
            const char character = *iterator;
            switch(character) {
                case '?':
                    throw std::runtime_error("unexpected <?> quantifier");
                case '*':
                    throw std::runtime_error("unexpected <*> quantifier");
                case '+':
                    throw std::runtime_error("unexpected <+> quantifier");
                case '^':
                    iterator += expect_stx(iterator, end);
                    break;
                case '$':
                    iterator += expect_etx(iterator, end);
                    break;
                case '.':
                    iterator += expect_any(iterator, end);
                    break;
                case '\\':
                    iterator += expect_esc(iterator, end);
                    break;
                default:
                    iterator += expect_chr(iterator, end);
                    break;
            }
        } while(iterator != end);
    }
    return iterator - begin;
};

auto RegExp::accept_quantifier(std::string::const_iterator begin, std::string::const_iterator end) -> size_t
{
    auto iterator = begin;

    if(iterator != end) {
        const char character = *iterator;
        switch(character) {
            case '?': // zero or one
                ++iterator;
                _bytecode.emit_rep(0, +1);
                break;
            case '*': // zero or more
                ++iterator;
                _bytecode.emit_rep(0, -1);
                break;
            case '+': // one or more
                ++iterator;
                _bytecode.emit_rep(1, -1);
                break;
            default:
                break;
        }
    }
    return iterator - begin;
}

auto RegExp::expect_stx(std::string::const_iterator begin, std::string::const_iterator end) -> size_t
{
    auto iterator = begin;

    if(iterator != end) {
        char character = *iterator;
        if(character == '^') {
            ++iterator;
            _bytecode.emit_stx();
        }
        else {
            throw std::runtime_error("stx was expected");
        }
    }
    else {
        throw std::runtime_error("unexpected end of string when stx was expected");
    }
    return iterator - begin;
}

auto RegExp::expect_etx(std::string::const_iterator begin, std::string::const_iterator end) -> size_t
{
    auto iterator = begin;

    if(iterator != end) {
        char character = *iterator;
        if(character == '$') {
            ++iterator;
            _bytecode.emit_etx();
        }
        else {
            throw std::runtime_error("etx was expected");
        }
    }
    else {
        throw std::runtime_error("unexpected end of string when etx was expected");
    }
    return iterator - begin;
}

auto RegExp::expect_any(std::string::const_iterator begin, std::string::const_iterator end) -> size_t
{
    auto iterator = begin;

    if(iterator != end) {
        char character = *iterator;
        if(character == '.') {
            ++iterator;
            iterator += accept_quantifier(iterator, end);
            _bytecode.emit_any();
        }
        else {
            throw std::runtime_error("any was expected");
        }
    }
    else {
        throw std::runtime_error("unexpected end of string when any was expected");
    }
    return iterator - begin;
}

auto RegExp::expect_esc(std::string::const_iterator begin, std::string::const_iterator end) -> size_t
{
    auto iterator = begin;

    if(iterator != end) {
        char character = *iterator;
        if(character == '\\') {
            ++iterator;
        }
        else {
            throw std::runtime_error("esc was expected");
        }
    }
    else {
        throw std::runtime_error("unexpected end of string when esc was expected");
    }
    if(iterator != end) {
        char character = *iterator;
        switch(character) {
            case 'a': // bell
                character = '\a';
                break;
            case 'b': // backspace
                character = '\b';
                break;
            case 't': // horizontal tab
                character = '\t';
                break;
            case 'r': // carriage return
                character = '\r';
                break;
            case 'n': // newline
                character = '\n';
                break;
            case 'v': // vertical tab
                character = '\v';
                break;
            case 'f': // form feed
                character = '\f';
                break;
            default:
                break;
        }
        switch(character) {
            case '?':
            case '*':
            case '+':
            case '^':
            case '$':
            case '.':
            case '\\':
            case '\a':
            case '\b':
            case '\t':
            case '\r':
            case '\n':
            case '\v':
            case '\f':
                ++iterator;
                iterator += accept_quantifier(iterator, end);
                _bytecode.emit_chr(character);
                break;
            default:
                throw std::runtime_error("invalid escape sequence");
        }
    }
    else {
        throw std::runtime_error("unexpected end of string when esc was expected");
    }
    return iterator - begin;
}

auto RegExp::expect_chr(std::string::const_iterator begin, std::string::const_iterator end) -> size_t
{
    auto iterator = begin;

    if(iterator != end) {
        char character = *iterator;
        ++iterator;
        iterator += accept_quantifier(iterator, end);
        _bytecode.emit_chr(character);
    }
    else {
        throw std::runtime_error("unexpected end of string when chr was expected");
    }
    return iterator - begin;
}

auto RegExp::match(ByteCode::const_iterator opcode, std::string::const_iterator string) -> bool
{
    const auto bytecode_end = _bytecode.end();
    const auto string_begin = _string.begin();
    const auto string_end   = _string.end();
    auto       prev_opcode  = opcode;
    auto       prev_string  = string;
    uint32_t   repeat_cnt   = 0;
    uint32_t   repeat_max   = 0;
    uint32_t   repeat_min   = 0;

    auto exec_nop = [&]() -> bool
    {
        opcode += 1;
        return true;
    };

    auto exec_stx = [&]() -> bool
    {
        opcode += 1;
        if(string == string_begin) {
            return true;
        }
        return false;
    };

    auto exec_etx = [&]() -> bool
    {
        opcode += 1;
        if(string == string_end) {
            return true;
        }
        return false;
    };

    auto exec_any = [&]() -> bool
    {
        opcode += 1;
        while((string != string_end) && (repeat_cnt < repeat_max)) {
            ++string;
            ++repeat_cnt;
        }
        if((repeat_cnt >= repeat_min)
        && (repeat_cnt <= repeat_max)) {
            return true;
        }
        return false;
    };

    auto exec_chr = [&]() -> bool
    {
        const char expected = *(opcode + 1);
        opcode += 2;
        while((string != string_end) && (repeat_cnt < repeat_max) && (*string == expected)) {
            ++string;
            ++repeat_cnt;
        }
        if((repeat_cnt >= repeat_min)
        && (repeat_cnt <= repeat_max)) {
            return true;
        }
        return false;
    };

    auto exec_rep = [&]() -> bool
    {
        repeat_min = 0;
        repeat_min = ((repeat_min << 8) | *(opcode + 1));
        repeat_min = ((repeat_min << 8) | *(opcode + 2));
        repeat_min = ((repeat_min << 8) | *(opcode + 3));
        repeat_min = ((repeat_min << 8) | *(opcode + 4));
        repeat_max = 0;
        repeat_max = ((repeat_max << 8) | *(opcode + 5));
        repeat_max = ((repeat_max << 8) | *(opcode + 6));
        repeat_max = ((repeat_max << 8) | *(opcode + 7));
        repeat_max = ((repeat_max << 8) | *(opcode + 8));
        opcode += 9;
        bool status = false;
        switch(*opcode) {
            case Opcodes::OP_ANY:
                status = exec_any();
                break;
            case Opcodes::OP_CHR:
                status = exec_chr();
                break;
            default:
                throw std::runtime_error("unexpected non-repeatable opcode");
        }
        if(status != false) {
            while(repeat_cnt != 0) {
                string = prev_string + repeat_cnt;
                if(match(opcode, string) != false) {
                    return true;
                }
                else {
                    --repeat_cnt;
                }
            }
            if(repeat_min == 0) {
                string = prev_string;
                if(match(opcode, string) != false) {
                    return true;
                }
            }
        }
        return false;
    };

    auto exec_ret = [&]() -> bool
    {
        opcode = bytecode_end;
        return true;
    };

    auto do_match = [&]() -> bool
    {
        bool status = false;
        while(opcode != bytecode_end) {
            prev_opcode = opcode;
            prev_string = string;
            repeat_cnt  = 0;
            repeat_min  = 1;
            repeat_max  = 1;
            switch(*opcode) {
                case Opcodes::OP_NOP:
                    status = exec_nop();
                    break;
                case Opcodes::OP_STX:
                    status = exec_stx();
                    break;
                case Opcodes::OP_ETX:
                    status = exec_etx();
                    break;
                case Opcodes::OP_ANY:
                    status = exec_any();
                    break;
                case Opcodes::OP_CHR:
                    status = exec_chr();
                    break;
                case Opcodes::OP_REP:
                    status = exec_rep();
                    break;
                case Opcodes::OP_RET:
                    status = exec_ret();
                    break;
                default:
                    throw std::runtime_error("unexpected opcode");
            }
            if(status == false) {
                return false;
            }
        }
        return true;
    };

    return do_match();
}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
