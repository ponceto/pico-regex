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

auto RegExp::clear() -> void
{
    ByteCode().swap(_bytecode);
}

auto RegExp::emit_byte(const uint8_t value) -> void
{
    _bytecode.push_back(static_cast<uint8_t>((value >>  0) & 0xff));
}

auto RegExp::emit_word(const uint16_t value) -> void
{
    _bytecode.push_back(static_cast<uint8_t>((value >>  8) & 0xff));
    _bytecode.push_back(static_cast<uint8_t>((value >>  0) & 0xff));
}

auto RegExp::emit_long(const uint32_t value) -> void
{
    _bytecode.push_back(static_cast<uint8_t>((value >> 24) & 0xff));
    _bytecode.push_back(static_cast<uint8_t>((value >> 16) & 0xff));
    _bytecode.push_back(static_cast<uint8_t>((value >>  8) & 0xff));
    _bytecode.push_back(static_cast<uint8_t>((value >>  0) & 0xff));
}

auto RegExp::emit_nop() -> void
{
    emit_byte(OP_NOP);
}

auto RegExp::emit_stx() -> void
{
    emit_byte(OP_STX);
}

auto RegExp::emit_etx() -> void
{
    emit_byte(OP_ETX);
}

auto RegExp::emit_any() -> void
{
    emit_byte(OP_ANY);
}

auto RegExp::emit_chr(const uint8_t character) -> void
{
    emit_byte(OP_CHR);
    emit_byte(character);
}

auto RegExp::emit_rep(const uint32_t min, const uint32_t max) -> void
{
    emit_byte(OP_REP);
    emit_long(min);
    emit_long(max);
}

auto RegExp::emit_ret() -> void
{
    emit_byte(OP_RET);
}

auto RegExp::exec_compile() -> bool
{
    auto accept_quantifier = [&](std::string::const_iterator begin, std::string::const_iterator end) -> size_t
    {
        auto iterator = begin;

        if(iterator != end) {
            const char character = *iterator;
            switch(character) {
                case '?': // zero or one
                    emit_rep(0, +1);
                    ++iterator;
                    break;
                case '*': // zero or more
                    emit_rep(0, -1);
                    ++iterator;
                    break;
                case '+': // one or more
                    emit_rep(1, -1);
                    ++iterator;
                    break;
                default:
                    break;
            }
        }
        return iterator - begin;
    };

    auto accept_escaped_seq = [&](std::string::const_iterator begin, std::string::const_iterator end) -> size_t
    {
        auto iterator = begin;

        if(iterator != end) {
            char character = *iterator;
            switch(character) {
                case 'a':
                    character = '\a';
                    break;
                case 'b':
                    character = '\b';
                    break;
                case 't':
                    character = '\t';
                    break;
                case 'r':
                    character = '\r';
                    break;
                case 'n':
                    character = '\n';
                    break;
                case 'v':
                    character = '\v';
                    break;
                case 'f':
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
                    emit_chr(character);
                    break;
                default:
                    throw std::runtime_error("invalid escaped sequence");
            }
        }
        return iterator - begin;
    };

    auto expect_expression = [&](std::string::const_iterator begin, std::string::const_iterator end) -> size_t
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
                        ++iterator;
                        emit_stx();
                        break;
                    case '$':
                        ++iterator;
                        emit_etx();
                        break;
                    case '.':
                        ++iterator;
                        iterator += accept_quantifier(iterator, end);
                        emit_any();
                        break;
                    case '\\':
                        ++iterator;
                        iterator += accept_escaped_seq(iterator, end);
                        break;
                    default:
                        ++iterator;
                        iterator += accept_quantifier(iterator, end);
                        emit_chr(character);
                        break;
                }
            } while(iterator != end);
        }
        return iterator - begin;
    };

    auto do_compile = [&]() -> bool
    {
        clear();
        emit_nop();
        expect_expression(_pattern.begin(), _pattern.end());
        emit_ret();

        return true;
    };

    return do_compile();
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

auto RegExp::match(ByteCode::const_iterator opcode, std::string::const_iterator string) -> bool
{
    uint32_t repeat_cnt = 0;
    uint32_t repeat_max = 0;
    uint32_t repeat_min = 0;

    auto op_nop = [&]() -> bool
    {
        opcode += 1;
        return true;
    };

    auto op_stx = [&]() -> bool
    {
        opcode += 1;
        if(string == _string.begin()) {
            return true;
        }
        return false;
    };

    auto op_etx = [&]() -> bool
    {
        opcode += 1;
        if(string == _string.end()) {
            return true;
        }
        return false;
    };

    auto op_any = [&]() -> bool
    {
        opcode += 1;
        while((string != _string.end()) && (repeat_cnt < repeat_max)) {
            ++string;
            ++repeat_cnt;
        }
        if((repeat_cnt >= repeat_min)
        && (repeat_cnt <= repeat_max)) {
            return true;
        }
        return false;
    };

    auto op_chr = [&]() -> bool
    {
        const char expected = *(opcode + 1);
        opcode += 2;
        while((string != _string.end()) && (repeat_cnt < repeat_max) && (*string == expected)) {
            ++string;
            ++repeat_cnt;
        }
        if((repeat_cnt >= repeat_min)
        && (repeat_cnt <= repeat_max)) {
            return true;
        }
        return false;
    };

    auto op_rep = [&]() -> bool
    {
        const auto prev_opcode = opcode;
        const auto prev_string = string;
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
        opcode = prev_opcode + 9;
        string = prev_string + 0;
        bool status = false;
        switch(*opcode) {
            case OP_ANY:
                status = op_any();
                break;
            case OP_CHR:
                status = op_chr();
                break;
            default:
                throw std::runtime_error("unexpected opcode");
        }
        if(status != false) {
            while(repeat_cnt != 0) {
                string = prev_string + repeat_cnt;
                if(match(opcode, string) != false) {
                    return true;
                }
                --repeat_cnt;
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

    auto op_ret = [&]() -> bool
    {
        opcode = _bytecode.end();
        return true;
    };

    auto do_match = [&]() -> bool
    {
        bool status = false;

        while(opcode != _bytecode.end()) {
            repeat_cnt = 0;
            repeat_min = 1;
            repeat_max = 1;
            switch(*opcode) {
                case OP_NOP:
                    status = op_nop();
                    break;
                case OP_STX:
                    status = op_stx();
                    break;
                case OP_ETX:
                    status = op_etx();
                    break;
                case OP_ANY:
                    status = op_any();
                    break;
                case OP_CHR:
                    status = op_chr();
                    break;
                case OP_REP:
                    status = op_rep();
                    break;
                case OP_RET:
                    status = op_ret();
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
