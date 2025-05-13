/*
 * executor.cc - Copyright (c) 2024-2025 - Olivier Poncet
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
#include "executor.h"

// ---------------------------------------------------------------------------
// Executor
// ---------------------------------------------------------------------------

Executor::Executor(ByteCode& bytecode, OStream& ostream, const uint32_t loglevel)
    : _ostream(std::cout)
    , _loglevel(loglevel)
    , _bytecode(bytecode)
{
}

auto Executor::execute(const std::string& string) -> bool
{
    ByteCodeIterator bytecode(_bytecode);
    StringIterator   iterator(string);

    try {
        begin(string);
        do {
            if(match(bytecode, iterator) != false) {
                return success();
            }
            else {
                iterator.advance(1);
            }
        } while(iterator);
    }
    catch(const std::exception& e) {
        if(_loglevel >= LogLevel::LOG_ERROR) {
            _ostream << "🔴" << ' ' << e.what() << std::endl;
        }
    }
    return failure();
}

auto Executor::begin(const std::string& string) -> void
{
    if(_loglevel >= LogLevel::LOG_PRINT) {
        _ostream << "🔵" << ' ' << "comparing" << '<' << string << '>' << std::endl;
    }
}

auto Executor::success() -> bool
{
    if(_loglevel >= LogLevel::LOG_ALERT) {
        _ostream << "🟢" << ' ' << "the string matches the regular expression" << std::endl;
    }
    return true;
}

auto Executor::failure() -> bool
{
    if(_loglevel >= LogLevel::LOG_ERROR) {
        _ostream << "🔴" << ' ' << "the string does not match the regular expression" << std::endl;
    }
    return false;
}

auto Executor::match(ByteCodeIterator bytecode, StringIterator iterator) -> bool
{
    auto     prev_iter  = iterator;
    uint32_t repeat_cnt = 0;
    uint32_t repeat_max = 0;
    uint32_t repeat_min = 0;

    auto exec_nop = [&]() -> bool
    {
        if(_loglevel >= LogLevel::LOG_TRACE) {
            _ostream << "🟣" << ' ' << "exec:nop" << std::endl;
        }
        return true;
    };

    auto exec_stx = [&]() -> bool
    {
        if(_loglevel >= LogLevel::LOG_TRACE) {
            _ostream << "🟣" << ' ' << "exec:stx" << std::endl;
        }
        return iterator.at_begin();
    };

    auto exec_etx = [&]() -> bool
    {
        if(_loglevel >= LogLevel::LOG_TRACE) {
            _ostream << "🟣" << ' ' << "exec:etx" << std::endl;
        }
        return iterator.at_end();
    };

    auto exec_any = [&]() -> bool
    {
        if(_loglevel >= LogLevel::LOG_TRACE) {
            _ostream << "🟣" << ' ' << "exec:any" << std::endl;
        }
        while((iterator) && (repeat_cnt < repeat_max)) {
            iterator.advance(1);
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
        const char expected = bytecode.next_byte();
        if(_loglevel >= LogLevel::LOG_TRACE) {
            _ostream << "🟣" << ' ' << "exec:chr" << ' ' << '<' << expected << '>' << std::endl;
        }
        while((iterator) && (repeat_cnt < repeat_max) && (iterator.peek() == expected)) {
            iterator.advance(1);
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
        repeat_min = bytecode.next_long();
        repeat_max = bytecode.next_long();
        if(_loglevel >= LogLevel::LOG_TRACE) {
            _ostream << "🟣" << ' ' << "exec:rep" << ' ' << '<' << repeat_min << ',' << repeat_max << '>' << std::endl;
        }
        bool status = false;
        switch(bytecode.next_byte()) {
            case ByteCode::OP_ANY:
                status = exec_any();
                break;
            case ByteCode::OP_CHR:
                status = exec_chr();
                break;
            default:
                throw std::runtime_error("unexpected non-repeatable opcode");
        }
        if(status != false) {
            while(repeat_cnt != 0) {
                iterator = prev_iter;
                iterator.advance(repeat_cnt);
                if(match(bytecode, iterator) != false) {
                    return true;
                }
                else {
                    --repeat_cnt;
                }
            }
            if(repeat_min == 0) {
                iterator = prev_iter;
                if(match(bytecode, iterator) != false) {
                    return true;
                }
            }
        }
        return false;
    };

    auto exec_err = [&]() -> bool
    {
        bytecode.to_end();
        if(_loglevel >= LogLevel::LOG_TRACE) {
            _ostream << "🟣" << ' ' << "exec:err" << std::endl;
        }
        return false;
    };

    auto exec_ret = [&]() -> bool
    {
        bytecode.to_end();
        if(_loglevel >= LogLevel::LOG_TRACE) {
            _ostream << "🟣" << ' ' << "exec:ret" << std::endl;
        }
        return true;
    };

    auto do_match = [&]() -> bool
    {
        bool status = false;
        while(bytecode) {
            prev_iter  = iterator;
            repeat_cnt = 0;
            repeat_min = 1;
            repeat_max = 1;
            switch(bytecode.next_byte()) {
                case ByteCode::OP_NOP:
                    status = exec_nop();
                    break;
                case ByteCode::OP_STX:
                    status = exec_stx();
                    break;
                case ByteCode::OP_ETX:
                    status = exec_etx();
                    break;
                case ByteCode::OP_ANY:
                    status = exec_any();
                    break;
                case ByteCode::OP_CHR:
                    status = exec_chr();
                    break;
                case ByteCode::OP_REP:
                    status = exec_rep();
                    break;
                case ByteCode::OP_ERR:
                    status = exec_err();
                    break;
                case ByteCode::OP_RET:
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
