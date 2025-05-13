#
# testsuite.sh - Copyright (c) 2024-2025 - Olivier Poncet
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
# some basic checks
# ----------------------------------------------------------------------------

./pico-regex.bin ''              ''                                  || exit 1
./pico-regex.bin '^$'            ''                                  || exit 1
./pico-regex.bin '^.*$'          'lorem ipsum dolor sit amet'        || exit 1
./pico-regex.bin '^lorem.*$'     'lorem ipsum dolor sit amet'        || exit 1
./pico-regex.bin '^.*ipsum.*$'   'lorem ipsum dolor sit amet'        || exit 1
./pico-regex.bin '^.*dolor.*$'   'lorem ipsum dolor sit amet'        || exit 1
./pico-regex.bin '^.*sit.*$'     'lorem ipsum dolor sit amet'        || exit 1
./pico-regex.bin '^.*amet$'      'lorem ipsum dolor sit amet'        || exit 1
./pico-regex.bin '^lorem.*amet$' 'lorem ipsum dolor sit amet'        || exit 1
./pico-regex.bin 'lorem'         'lorem ipsum dolor sit amet'        || exit 1
./pico-regex.bin 'ipsum'         'lorem ipsum dolor sit amet'        || exit 1
./pico-regex.bin 'dolor'         'lorem ipsum dolor sit amet'        || exit 1
./pico-regex.bin 'sit'           'lorem ipsum dolor sit amet'        || exit 1
./pico-regex.bin 'amet'          'lorem ipsum dolor sit amet'        || exit 1
./pico-regex.bin '^a?zerty$'     'zerty'                             || exit 1
./pico-regex.bin '^a?zerty$'     'azerty'                            || exit 1
./pico-regex.bin '^a?azerty$'    'azerty'                            || exit 1
./pico-regex.bin '^a?azerty$'    'aazerty'                           || exit 1
./pico-regex.bin '^a+zerty$'     'azerty'                            || exit 1
./pico-regex.bin '^a+zerty$'     'aazerty'                           || exit 1
./pico-regex.bin '^a+zerty$'     'aaazerty'                          || exit 1
./pico-regex.bin '^a+zerty$'     'aaaazerty'                         || exit 1

# ----------------------------------------------------------------------------
# End-Of-File
# ----------------------------------------------------------------------------
