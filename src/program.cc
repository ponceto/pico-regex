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

    auto do_parse = [&]() -> bool
    {
        int argi = -1;
        for(auto& arg : args) {
            if(++argi == 0) {
                continue;
            }
            else if(arg == "-h") {
                return false;
            }
            else if(arg == "--help") {
                return false;
            }
            else if(Globals::pattern.empty()) {
                Globals::pattern = arg;
            }
            else if(Globals::string.empty()) {
                Globals::string = arg;
            }
            else {
                throw std::runtime_error(std::string("invalid argument") + ' ' + '\'' + arg + '\'');
            }
        }
        return true;
    };

    auto do_init = [&]() -> bool
    {
        Globals::init();

        return do_parse();
    };

    return do_init();
}

auto Program::main(const ArgList& args) -> void
{
    auto do_main = [&](std::ostream& stream) -> void
    {
        RegExp regexp;
        if(regexp.compile(Globals::pattern) == false) {
            throw std::runtime_error("invalid expression");
        }
        if(regexp.compare(Globals::string) != false) {
            std::cout << "the string matches the regular expression" << std::endl;
        }
        else {
            std::cout << "the string does not match the regular expression" << std::endl;
        }
    };

    return do_main(std::cout);
}

auto Program::help(const ArgList& args) -> void
{
    auto program_name = [&]() -> const char*
    {
        const char* arg = args[0].c_str();
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
    return EXIT_SUCCESS;
}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
