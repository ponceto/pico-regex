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

    auto compile(const std::string& string) -> bool;

    auto compare(const std::string& string) -> bool;

protected: // protected data
    ByteCode _bytecode;
};

// ---------------------------------------------------------------------------
// Compiler
// ---------------------------------------------------------------------------

class Compiler
{
public: // public interface
    Compiler(ByteCode&, const std::string&);

    Compiler(Compiler&&) = delete;

    Compiler& operator=(Compiler&&) = delete;

    Compiler(const Compiler&) = delete;

    Compiler& operator=(const Compiler&) = delete;

    virtual ~Compiler() = default;

    auto compile() -> bool;

protected: // protected interface
    using OpcodeIterator = ByteCode::const_iterator;
    using StringIterator = std::string::const_iterator;

    auto expect_expression(StringIterator begin, StringIterator end) -> size_t;

    auto accept_quantifier(StringIterator begin, StringIterator end) -> size_t;

    auto expect_stx(StringIterator begin, StringIterator end) -> size_t;

    auto expect_etx(StringIterator begin, StringIterator end) -> size_t;

    auto expect_any(StringIterator begin, StringIterator end) -> size_t;

    auto expect_esc(StringIterator begin, StringIterator end) -> size_t;

    auto expect_chr(StringIterator begin, StringIterator end) -> size_t;

protected: // protected data
    ByteCode&         _bytecode;
    const std::string _string;
};

// ---------------------------------------------------------------------------
// Executor
// ---------------------------------------------------------------------------

class Executor
{
public: // public interface
    Executor(ByteCode&, const std::string&);

    Executor(Executor&&) = delete;

    Executor& operator=(Executor&&) = delete;

    Executor(const Executor&) = delete;

    Executor& operator=(const Executor&) = delete;

    virtual ~Executor() = default;

    auto execute() -> bool;

protected: // protected interface
    using OpcodeIterator = ByteCode::const_iterator;
    using StringIterator = std::string::const_iterator;

    auto match(OpcodeIterator opcode, StringIterator string) -> bool;

protected: // protected data
    ByteCode&         _bytecode;
    const std::string _string;
};

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __RegExp_h__ */
