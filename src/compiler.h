/*
 * compiler.h - Copyright (c) 2024-2025 - Olivier Poncet
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
#ifndef __Compiler_h__
#define __Compiler_h__

#include "loglevel.h"
#include "bytecode.h"

// ---------------------------------------------------------------------------
// Compiler
// ---------------------------------------------------------------------------

class Compiler
{
public: // public interface
    Compiler(ByteCode&, OStream&, const uint32_t loglevel);

    Compiler(Compiler&&) = delete;

    Compiler(const Compiler&) = delete;

    Compiler& operator=(Compiler&&) = delete;

    Compiler& operator=(const Compiler&) = delete;

    virtual ~Compiler() = default;

    auto compile(const std::string& string) -> bool;

protected: // protected interface
    auto begin(const std::string& string) -> void;

    auto success() -> bool;

    auto failure() -> bool;

    auto clear() -> void;

    auto emit_nop() -> void;

    auto emit_stx() -> void;

    auto emit_etx() -> void;

    auto emit_any() -> void;

    auto emit_chr(const uint8_t character) -> void;

    auto emit_rep(const uint32_t min, const uint32_t max) -> void;

    auto emit_err() -> void;

    auto emit_ret() -> void;

    auto expect_expression(StringIterator pattern) -> size_t;

    auto accept_quantifier(StringIterator pattern) -> size_t;

    auto expect_stx(StringIterator pattern) -> size_t;

    auto expect_etx(StringIterator pattern) -> size_t;

    auto expect_any(StringIterator pattern) -> size_t;

    auto expect_esc(StringIterator pattern) -> size_t;

    auto expect_chr(StringIterator pattern) -> size_t;

protected: // protected data
    OStream&       _ostream;
    const uint32_t _loglevel;
    ByteCode&      _bytecode;
};

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __Compiler_h__ */
