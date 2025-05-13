/*
 * bytecode.cc - Copyright (c) 2024-2025 - Olivier Poncet
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
#include "bytecode.h"

// ---------------------------------------------------------------------------
// ByteCode
// ---------------------------------------------------------------------------

ByteCode::ByteCode()
    : _bytecode()
{
}

auto ByteCode::clear() -> void
{
    container_type().swap(_bytecode);
}

auto ByteCode::emit_byte(const uint8_t value) -> void
{
    _bytecode.push_back(static_cast<uint8_t>((value >>  0) & 0xff));
}

auto ByteCode::emit_word(const uint16_t value) -> void
{
    _bytecode.push_back(static_cast<uint8_t>((value >>  8) & 0xff));
    _bytecode.push_back(static_cast<uint8_t>((value >>  0) & 0xff));
}

auto ByteCode::emit_long(const uint32_t value) -> void
{
    _bytecode.push_back(static_cast<uint8_t>((value >> 24) & 0xff));
    _bytecode.push_back(static_cast<uint8_t>((value >> 16) & 0xff));
    _bytecode.push_back(static_cast<uint8_t>((value >>  8) & 0xff));
    _bytecode.push_back(static_cast<uint8_t>((value >>  0) & 0xff));
}

auto ByteCode::emit_nop() -> void
{
    emit_byte(OP_NOP);
}

auto ByteCode::emit_stx() -> void
{
    emit_byte(OP_STX);
}

auto ByteCode::emit_etx() -> void
{
    emit_byte(OP_ETX);
}

auto ByteCode::emit_any() -> void
{
    emit_byte(OP_ANY);
}

auto ByteCode::emit_chr(const uint8_t character) -> void
{
    emit_byte(OP_CHR);
    emit_byte(character);
}

auto ByteCode::emit_rep(const uint32_t min, const uint32_t max) -> void
{
    emit_byte(OP_REP);
    emit_long(min);
    emit_long(max);
}

auto ByteCode::emit_err() -> void
{
    emit_byte(OP_ERR);
}

auto ByteCode::emit_ret() -> void
{
    emit_byte(OP_RET);
}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
