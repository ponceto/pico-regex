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

#include "loglevel.h"
#include "bytecode.h"
#include "compiler.h"
#include "executor.h"

// ---------------------------------------------------------------------------
// RegExp
// ---------------------------------------------------------------------------

class RegExp
{
public: // public interface
    RegExp(OStream& ostream, const uint32_t loglevel);

    RegExp(RegExp&&) = delete;

    RegExp(const RegExp&) = delete;

    RegExp& operator=(RegExp&&) = delete;

    RegExp& operator=(const RegExp&) = delete;

    virtual ~RegExp() = default;

    auto compile(const std::string& string) -> bool;

    auto execute(const std::string& string) -> bool;

protected: // protected data
    OStream&       _ostream;
    const uint32_t _loglevel;
    ByteCode       _bytecode;
};

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __RegExp_h__ */
