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

build: build_regex
	@echo "=== $@ ok ==="

clean: clean_regex
	@echo "=== $@ ok ==="

# ----------------------------------------------------------------------------
# regex files
# ----------------------------------------------------------------------------

regex_PROGRAM = regex.bin

regex_SOURCES = \
	src/globals.cc \
	src/program.cc \
	src/regexp.cc \
	$(NULL)

regex_HEADERS = \
	src/globals.h \
	src/program.h \
	src/regexp.h \
	$(NULL)

regex_OBJECTS = \
	src/globals.o \
	src/program.o \
	src/regexp.o \
	$(NULL)

regex_LDFLAGS = \
	$(NULL)

regex_LDADD = \
	-lpthread \
	$(NULL)

regex_CLEANFILES = \
	$(NULL)

# ----------------------------------------------------------------------------
# build regex
# ----------------------------------------------------------------------------

build_regex: $(regex_PROGRAM)

$(regex_PROGRAM): $(regex_OBJECTS)
	$(LD) $(LDFLAGS) $(regex_LDFLAGS) -o $(regex_PROGRAM) $(regex_OBJECTS) $(regex_LDADD)

# ----------------------------------------------------------------------------
# clean regex
# ----------------------------------------------------------------------------

clean_regex:
	$(RM) $(RMFLAGS) $(regex_OBJECTS) $(regex_PROGRAM) $(regex_CLEANFILES)

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
