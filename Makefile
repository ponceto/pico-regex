#
# Makefile - Copyright (c) 2024-2025 - Olivier Poncet
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

# ----------------------------------------------------------------------------
# global environment
# ----------------------------------------------------------------------------

TOPDIR   = $(CURDIR)
OPTLEVEL = -O2 -g
WARNINGS = -Wall
EXTRAS   = -pthread
CC       = gcc
CFLAGS   = -std=c99 $(OPTLEVEL) $(WARNINGS) $(EXTRAS)
CXX      = g++
CXXFLAGS = -std=c++14 $(OPTLEVEL) $(WARNINGS) $(EXTRAS)
CPP      = cpp
CPPFLAGS = -I. -I$(TOPDIR)/src -D_DEFAULT_SOURCE -D_FORTIFY_SOURCE=2
LD       = g++
LDFLAGS  = -L.
CP       = cp
CPFLAGS  = -f
RM       = rm
RMFLAGS  = -f

# ----------------------------------------------------------------------------
# default rules
# ----------------------------------------------------------------------------

.c.o:
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

# ----------------------------------------------------------------------------
# global targets
# ----------------------------------------------------------------------------

all: build

build: build_pico_regex
	@echo "=== $@ ok ==="

clean: clean_pico_regex
	@echo "=== $@ ok ==="

check: check_pico_regex
	@echo "=== $@ ok ==="

# ----------------------------------------------------------------------------
# regex files
# ----------------------------------------------------------------------------

pico_regex_PROGRAM = bin/pico-regex.bin

pico_regex_SOURCES = \
	src/globals.cc \
	src/program.cc \
	src/loglevel.cc \
	src/bytecode.cc \
	src/compiler.cc \
	src/executor.cc \
	src/regexp.cc \
	$(NULL)

pico_regex_HEADERS = \
	src/globals.h \
	src/program.h \
	src/loglevel.h \
	src/bytecode.h \
	src/compiler.h \
	src/executor.h \
	src/regexp.h \
	$(NULL)

pico_regex_OBJECTS = \
	src/globals.o \
	src/program.o \
	src/loglevel.o \
	src/bytecode.o \
	src/compiler.o \
	src/executor.o \
	src/regexp.o \
	$(NULL)

pico_regex_LDFLAGS = \
	$(NULL)

pico_regex_LDADD = \
	-lpthread \
	$(NULL)

pico_regex_CLEANFILES = \
	$(NULL)

# ----------------------------------------------------------------------------
# build regex
# ----------------------------------------------------------------------------

build_pico_regex: $(pico_regex_PROGRAM)

$(pico_regex_PROGRAM): $(pico_regex_OBJECTS)
	$(LD) $(LDFLAGS) $(pico_regex_LDFLAGS) -o $(pico_regex_PROGRAM) $(pico_regex_OBJECTS) $(pico_regex_LDADD)

# ----------------------------------------------------------------------------
# clean regex
# ----------------------------------------------------------------------------

clean_pico_regex:
	$(RM) $(RMFLAGS) $(pico_regex_OBJECTS) $(pico_regex_PROGRAM) $(pico_regex_CLEANFILES)

# ----------------------------------------------------------------------------
# check regex
# ----------------------------------------------------------------------------

check_pico_regex:
	cd ./bin && ./testsuite.sh

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
