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

RegExp::RegExp(OStream& ostream, const uint32_t loglevel)
    : _ostream(ostream)
    , _loglevel(loglevel)
    , _bytecode()
{
}

auto RegExp::compile(const std::string& string) -> bool
{
    Compiler compiler(_bytecode, _ostream, _loglevel);

    return compiler.compile(string);
}

auto RegExp::execute(const std::string& string) -> bool
{
    Executor executor(_bytecode, _ostream, _loglevel);

    return executor.execute(string);
}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
