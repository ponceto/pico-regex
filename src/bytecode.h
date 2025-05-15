/*
 * bytecode.h - Copyright (c) 2024-2025 - Olivier Poncet
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
#ifndef __ByteCode_h__
#define __ByteCode_h__

// ---------------------------------------------------------------------------
// Opcodes
// ---------------------------------------------------------------------------

struct Opcodes
{
    static constexpr uint8_t OP_NOP = 0x00; // no operation
    static constexpr uint8_t OP_STX = 0x01; // start of text
    static constexpr uint8_t OP_ETX = 0x02; // end of text
    static constexpr uint8_t OP_ANY = 0x03; // any character
    static constexpr uint8_t OP_CHR = 0x04; // specific character
    static constexpr uint8_t OP_REP = 0x05; // repeat
    static constexpr uint8_t OP_RET = 0x06; // return
};

// ---------------------------------------------------------------------------
// ByteCode
// ---------------------------------------------------------------------------

class ByteCode
{
public: // public interface
    ByteCode();

    ByteCode(ByteCode&&) = delete;

    ByteCode& operator=(ByteCode&&) = delete;

    ByteCode(const ByteCode&) = delete;

    ByteCode& operator=(const ByteCode&) = delete;

    virtual ~ByteCode() = default;

    using container_type         = std::vector<uint8_t>;
    using value_type             = container_type::value_type;
    using size_type              = container_type::size_type;
    using difference_type        = container_type::difference_type;
    using iterator               = container_type::iterator;
    using const_iterator         = container_type::const_iterator;
    using reverse_iterator       = container_type::reverse_iterator;
    using const_reverse_iterator = container_type::const_reverse_iterator;

    auto begin() -> auto
    {
        return _bytecode.begin();
    }

    auto end() -> auto
    {
        return _bytecode.end();
    }

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

protected: // protected data
    container_type _bytecode;
};

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __ByteCode_h__ */
