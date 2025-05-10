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

    auto compile(const std::string& pattern) -> bool;

    auto compare(const std::string& string) -> bool;

protected: // protected definitions
    using ByteCode = std::vector<uint8_t>;

    static constexpr uint8_t OP_NOP = 0x00;
    static constexpr uint8_t OP_STX = 0x01;
    static constexpr uint8_t OP_ETX = 0x02;
    static constexpr uint8_t OP_ANY = 0x03;
    static constexpr uint8_t OP_CHR = 0x04;
    static constexpr uint8_t OP_REP = 0x05;
    static constexpr uint8_t OP_RET = 0x06;

protected: // protected interface
    auto clear() -> void;

    auto emit_byte(const uint8_t value) -> void;

    auto emit_word(const uint16_t value) -> void;

    auto emit_long(const uint32_t value) -> void;

    auto emit_nop() -> void;

    auto emit_stx() -> void;

    auto emit_etx() -> void;

    auto emit_any() -> void;

    auto emit_chr(const uint8_t character) -> void;

    auto emit_rep(const uint32_t min, const uint32_t max) -> void;

    auto emit_ret() -> void;

    auto exec_compile() -> bool;

    auto exec_compare() -> bool;

    auto match(ByteCode::const_iterator bytecode, std::string::const_iterator string) -> bool;

protected: // protected data
    ByteCode    _bytecode;
    std::string _pattern;
    std::string _string;
};

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __RegExp_h__ */
