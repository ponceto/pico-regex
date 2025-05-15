/*
 * regexp.h - Copyright (c) 2024-2025 - Olivier Poncet
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
#ifndef __RegExp_h__
#define __RegExp_h__

#include "bytecode.h"

// ---------------------------------------------------------------------------
// RegExp
// ---------------------------------------------------------------------------

class RegExp
{
public: // public interface
    RegExp();

    RegExp(RegExp&&) = delete;

    RegExp& operator=(RegExp&&) = delete;

    RegExp(const RegExp&) = delete;

    RegExp& operator=(const RegExp&) = delete;

    virtual ~RegExp() = default;

    auto compile(const std::string& pattern) -> bool;

    auto compare(const std::string& string) -> bool;

protected: // protected interface
    auto exec_compile() -> bool;

    auto exec_compare() -> bool;

    auto match(ByteCode::const_iterator opcode, std::string::const_iterator string) -> bool;

    auto expect_expression(std::string::const_iterator begin, std::string::const_iterator end) -> size_t;

    auto accept_quantifier(std::string::const_iterator begin, std::string::const_iterator end) -> size_t;

    auto expect_stx(std::string::const_iterator begin, std::string::const_iterator end) -> size_t;

    auto expect_etx(std::string::const_iterator begin, std::string::const_iterator end) -> size_t;

    auto expect_any(std::string::const_iterator begin, std::string::const_iterator end) -> size_t;

    auto expect_esc(std::string::const_iterator begin, std::string::const_iterator end) -> size_t;

    auto expect_chr(std::string::const_iterator begin, std::string::const_iterator end) -> size_t;

protected: // protected data
    ByteCode    _bytecode;
    std::string _pattern;
    std::string _string;
};

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __RegExp_h__ */
