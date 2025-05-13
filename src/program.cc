/*
 * program.cc - Copyright (c) 2024-2025 - Olivier Poncet
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
#include "globals.h"
#include "program.h"
#include "regexp.h"

// ---------------------------------------------------------------------------
// Program
// ---------------------------------------------------------------------------

auto Program::init(const ArgList& args) -> bool
{
    static const std::locale new_locale("");
    static const std::locale old_locale(std::locale::global(new_locale));

    auto do_init = [&]() -> bool
    {
        int argn =  0;
        int argi = -1;
        for(auto& arg : args) {
            if(++argi == 0) {
                Globals::arg0 = arg;
                ++argn;
                continue;
            }
            else if(arg == "-h") {
                return false;
            }
            else if(arg == "--help") {
                return false;
            }
            else if((arg == "-v") || (arg == "--verbose")) {
                Globals::loglevel = LogLevel::LOG_TRACE;
            }
            else if((arg == "-q") || (arg == "--quiet")) {
                Globals::loglevel = LogLevel::LOG_QUIET;
            }
            else if((arg == "-1") || (arg == "--error")) {
                Globals::loglevel = LogLevel::LOG_ERROR;
            }
            else if((arg == "-2") || (arg == "--alert")) {
                Globals::loglevel = LogLevel::LOG_ALERT;
            }
            else if((arg == "-3") || (arg == "--print")) {
                Globals::loglevel = LogLevel::LOG_PRINT;
            }
            else if((arg == "-4") || (arg == "--debug")) {
                Globals::loglevel = LogLevel::LOG_DEBUG;
            }
            else if((arg == "-5") || (arg == "--trace")) {
                Globals::loglevel = LogLevel::LOG_TRACE;
            }
            else if(argn == 1) {
                Globals::arg1 = arg;
                ++argn;
            }
            else if(argn == 2) {
                Globals::arg2 = arg;
                ++argn;
            }
            else {
                throw std::runtime_error(std::string("invalid argument") + ' ' + '\'' + arg + '\'');
            }
        }
        return true;
    };

    return do_init();
}

auto Program::main(const ArgList& args) -> void
{
    auto do_main = [&](std::ostream& stream) -> void
    {
        RegExp regexp(stream, Globals::loglevel);

        if(regexp.compile(Globals::arg1) == false) {
            Globals::exitcode = EXIT_FAILURE;
            return;
        }
        if(regexp.execute(Globals::arg2) == false) {
            Globals::exitcode = EXIT_FAILURE;
            return;
        }
    };

    return do_main(std::cout);
}

auto Program::help(const ArgList& args) -> void
{
    auto program_name = []() -> const char*
    {
        const char* arg = Globals::arg0.c_str();
        const char* sep = ::strrchr(arg, '/');
        if(sep != nullptr) {
            arg = (sep + 1);
        }
        return arg;
    };

    auto do_help = [&](std::ostream& stream) -> void
    {
        stream << "Usage: " << program_name() << " [OPTIONS...] [PATTERN] [STRING]" << std::endl;
        stream << ""                                                                << std::endl;
        stream << "Options:"                                                        << std::endl;
        stream << ""                                                                << std::endl;
        stream << "  -h, --help                    display this help and exit"      << std::endl;
        stream << "  -v, --verbose                 verbose mode"                    << std::endl;
        stream << "  -q, --quiet                   quiet mode"                      << std::endl;
        stream << "  -1, --error                   error log level"                 << std::endl;
        stream << "  -2, --alert                   alert log level"                 << std::endl;
        stream << "  -3, --print                   print log level (default)"       << std::endl;
        stream << "  -4, --debug                   debug log level"                 << std::endl;
        stream << "  -5, --trace                   trace log level"                 << std::endl;
        stream << ""                                                                << std::endl;
    };

    return do_help(std::cout);
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    const ArgList args(argv, argv + argc);

    try {
        if(Program::init(args) != false) {
            Program::main(args);
        }
        else {
            Program::help(args);
        }
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch(...) {
        std::cerr << "error!" << std::endl;
        return EXIT_FAILURE;
    }
    return Globals::exitcode;
}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
