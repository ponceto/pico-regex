/*
 * compiler.cc - Copyright (c) 2024-2025 - Olivier Poncet
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
#include "compiler.h"

// ---------------------------------------------------------------------------
// Compiler
// ---------------------------------------------------------------------------

Compiler::Compiler(ByteCode& bytecode, OStream& ostream, const uint32_t loglevel)
    : _ostream(std::cout)
    , _loglevel(loglevel)
    , _bytecode(bytecode)
{
}

auto Compiler::compile(const std::string& string) -> bool
{
    StringIterator pattern(string);

    try {
        begin(string);
        expect_expression(pattern);
        return success();
    }
    catch(const std::exception& e) {
        if(_loglevel >= LogLevel::LOG_ERROR) {
            _ostream << "🔴" << ' ' << e.what() << std::endl;
        }
    }
    return failure();
}

auto Compiler::begin(const std::string& string) -> void
{
    if(_loglevel >= LogLevel::LOG_PRINT) {
        _ostream << "🔵" << ' ' << "compiling" << '<' << string << '>' << std::endl;
    }
    clear();
    emit_nop();
}

auto Compiler::success() -> bool
{
    emit_ret();
    if(_loglevel >= LogLevel::LOG_ALERT) {
        _ostream << "🟢" << ' ' << "the regular expression has been compiled" << std::endl;
    }
    return true;
}

auto Compiler::failure() -> bool
{
    emit_err();
    if(_loglevel >= LogLevel::LOG_ERROR) {
        _ostream << "🔴" << ' ' << "the regular expression could not be compiled" << std::endl;
    }
    return false;
}

auto Compiler::clear() -> void
{
    if(_loglevel >= LogLevel::LOG_DEBUG) {
        _ostream << "🟣" << ' ' << "code:clear" << std::endl;
    }
    _bytecode.clear();
}

auto Compiler::emit_nop() -> void
{
    if(_loglevel >= LogLevel::LOG_DEBUG) {
        _ostream << "🟣" << ' ' << "emit:nop" << std::endl;
    }
    _bytecode.emit_nop();
}

auto Compiler::emit_stx() -> void
{
    if(_loglevel >= LogLevel::LOG_DEBUG) {
        _ostream << "🟣" << ' ' << "emit:stx" << std::endl;
    }
    _bytecode.emit_stx();
}

auto Compiler::emit_etx() -> void
{
    if(_loglevel >= LogLevel::LOG_DEBUG) {
        _ostream << "🟣" << ' ' << "emit:etx" << std::endl;
    }
    _bytecode.emit_etx();
}

auto Compiler::emit_any() -> void
{
    if(_loglevel >= LogLevel::LOG_DEBUG) {
        _ostream << "🟣" << ' ' << "emit:any" << std::endl;
    }
    _bytecode.emit_any();
}

auto Compiler::emit_chr(const uint8_t character) -> void
{
    if(_loglevel >= LogLevel::LOG_DEBUG) {
        _ostream << "🟣" << ' ' << "emit:chr" << ' ' << '<' << character << '>' << std::endl;
    }
    _bytecode.emit_chr(character);
}

auto Compiler::emit_rep(const uint32_t min, const uint32_t max) -> void
{
    if(_loglevel >= LogLevel::LOG_DEBUG) {
        _ostream << "🟣" << ' ' << "emit:rep" << ' ' << '<' << min << ',' << max << '>' << std::endl;
    }
    _bytecode.emit_rep(min, max);
}

auto Compiler::emit_err() -> void
{
    if(_loglevel >= LogLevel::LOG_DEBUG) {
        _ostream << "🟣" << ' ' << "emit:err" << std::endl;
    }
    _bytecode.emit_err();
}

auto Compiler::emit_ret() -> void
{
    if(_loglevel >= LogLevel::LOG_DEBUG) {
        _ostream << "🟣" << ' ' << "emit:ret" << std::endl;
    }
    _bytecode.emit_ret();
}

auto Compiler::expect_expression(StringIterator pattern) -> size_t
{
    const auto prev = pattern.curr();

    if(pattern) {
        do {
            const char character = pattern.peek();
            switch(character) {
                case '?':
                    throw std::runtime_error("unexpected <?> quantifier");
                case '*':
                    throw std::runtime_error("unexpected <*> quantifier");
                case '+':
                    throw std::runtime_error("unexpected <+> quantifier");
                case '^':
                    pattern.advance(expect_stx(pattern));
                    break;
                case '$':
                    pattern.advance(expect_etx(pattern));
                    break;
                case '.':
                    pattern.advance(expect_any(pattern));
                    break;
                case '\\':
                    pattern.advance(expect_esc(pattern));
                    break;
                default:
                    pattern.advance(expect_chr(pattern));
                    break;
            }
        } while(pattern);
    }
    return pattern.curr() - prev;
};

auto Compiler::accept_quantifier(StringIterator pattern) -> size_t
{
    const auto prev = pattern.curr();

    if(pattern) {
        const char character = pattern.peek();
        switch(character) {
            case '?': // zero or one
                pattern.advance(1);
                emit_rep(0, +1);
                break;
            case '*': // zero or more
                pattern.advance(1);
                emit_rep(0, -1);
                break;
            case '+': // one or more
                pattern.advance(1);
                emit_rep(1, -1);
                break;
            default:
                break;
        }
    }
    return pattern.curr() - prev;
}

auto Compiler::expect_stx(StringIterator pattern) -> size_t
{
    const auto prev = pattern.curr();

    if(pattern) {
        char character = pattern.peek();
        if(character == '^') {
            pattern.advance(1);
            emit_stx();
        }
        else {
            throw std::runtime_error("unexpected character when stx was expected");
        }
    }
    else {
        throw std::runtime_error("unexpected end of string when stx was expected");
    }
    return pattern.curr() - prev;
}

auto Compiler::expect_etx(StringIterator pattern) -> size_t
{
    const auto prev = pattern.curr();

    if(pattern) {
        char character = pattern.peek();
        if(character == '$') {
            pattern.advance(1);
            emit_etx();
        }
        else {
            throw std::runtime_error("unexpected character when etx was expected");
        }
    }
    else {
        throw std::runtime_error("unexpected end of string when etx was expected");
    }
    return pattern.curr() - prev;
}

auto Compiler::expect_any(StringIterator pattern) -> size_t
{
    const auto prev = pattern.curr();

    if(pattern) {
        char character = pattern.peek();
        if(character == '.') {
            pattern.advance(1);
            pattern.advance(accept_quantifier(pattern));
            emit_any();
        }
        else {
            throw std::runtime_error("unexpected character when any was expected");
        }
    }
    else {
        throw std::runtime_error("unexpected end of string when any was expected");
    }
    return pattern.curr() - prev;
}

auto Compiler::expect_esc(StringIterator pattern) -> size_t
{
    auto prev = pattern.curr();

    if(pattern) {
        char character = pattern.peek();
        if(character == '\\') {
            pattern.advance(1);
        }
        else {
            throw std::runtime_error("unexpected character when esc was expected");
        }
    }
    else {
        throw std::runtime_error("unexpected end of string when esc was expected");
    }
    if(pattern) {
        char character = pattern.peek();
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
            case 'n': // new line
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
                pattern.advance(1);
                pattern.advance(accept_quantifier(pattern));
                emit_chr(character);
                break;
            default:
                throw std::runtime_error("invalid escape sequence");
        }
    }
    else {
        throw std::runtime_error("unexpected end of string when esc was expected");
    }
    return pattern.curr() - prev;
}

auto Compiler::expect_chr(StringIterator pattern) -> size_t
{
    const auto prev = pattern.curr();

    if(pattern) {
        char character = pattern.peek();
        if(character != '\0') {
            pattern.advance(1);
            pattern.advance(accept_quantifier(pattern));
            emit_chr(character);
        }
        else {
            throw std::runtime_error("unexpected character when chr was expected");
        }
    }
    else {
        throw std::runtime_error("unexpected end of string when chr was expected");
    }
    return pattern.curr() - prev;
}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
