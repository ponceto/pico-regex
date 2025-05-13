# REGEX COMPILER

A very minimal and very incomplete [RegEx](https://en.wikipedia.org/wiki/Regular_expression) engine written for education purpose.

## DESCRIPTION

This program is written in C++ and implements a very minimal and very incomplete RegEx engine to demonstrate how it can be done in a few number of lines of code.

The regex engine quite simple and supports only a little subset of the regular expressions grammar.

Metacharacters:

```
^ ... start of text
$ ... end of text
. ... any character
```

Quantifiers:

```
? ... zero or one occurences
* ... zero or more occurences
+ ... one or more occurences
```

Escaped sequences:

```
\a ... bell
\b ... backspace
\t ... tab
\r ... carriage return
\n ... newline
\v ... vertical tab
\f ... form feed
```

## HOW TO BUILD

### Install the dependencies

Under Debian and derivatives (Ubuntu, Mint, ...):

```
apt-get install build-essential
```

### Build the project

To build the project, simply type:

```
make
```

You can also use the `-j` option if you want to build in parallel with `GNU make`:

```
make -j{number-of-jobs}
```

### Clean the project

To clean the project, simply type:

```
make clean
```

## HOW TO RUN

### Usage

Command line options:

```
Usage: regex.bin [OPTIONS...] [PATTERN] [STRING]

Options:

  -h, --help                    display this help and exit

```

### Run

```
./regex.bin
```

## LICENSE

The source code is released under the terms of the GNU General Public License 2.0.

```
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
```

