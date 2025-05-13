/*
 * loglevel.h - Copyright (c) 2024-2025 - Olivier Poncet
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
#ifndef __LogLevel_h__
#define __LogLevel_h__

// ---------------------------------------------------------------------------
// some type aliases
// ---------------------------------------------------------------------------

using IStream = std::istream;
using OStream = std::ostream;

// ---------------------------------------------------------------------------
// LogLevel
// ---------------------------------------------------------------------------

struct LogLevel
{
    static constexpr uint32_t LOG_QUIET = 0;
    static constexpr uint32_t LOG_ERROR = 1;
    static constexpr uint32_t LOG_ALERT = 2;
    static constexpr uint32_t LOG_PRINT = 3;
    static constexpr uint32_t LOG_DEBUG = 4;
    static constexpr uint32_t LOG_TRACE = 5;
};

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __LogLevel_h__ */
