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

#include "loglevel.h"

// ---------------------------------------------------------------------------
// ByteCode
// ---------------------------------------------------------------------------

class ByteCode
{
public: // public interface
    ByteCode();

    ByteCode(ByteCode&&) = delete;

    ByteCode(const ByteCode&) = delete;

    ByteCode& operator=(ByteCode&&) = delete;

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

    static constexpr uint8_t OP_NOP = 0x00; // no operation
    static constexpr uint8_t OP_STX = 0x01; // start of text
    static constexpr uint8_t OP_ETX = 0x02; // end of text
    static constexpr uint8_t OP_ANY = 0x03; // any character
    static constexpr uint8_t OP_CHR = 0x04; // specific character
    static constexpr uint8_t OP_REP = 0x05; // repeat
    static constexpr uint8_t OP_ERR = 0x06; // error
    static constexpr uint8_t OP_RET = 0x07; // return

    auto begin() const -> const_iterator
    {
        return _bytecode.begin();
    }

    auto end() const -> const_iterator
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

    auto emit_err() -> void;

    auto emit_ret() -> void;

protected: // protected data
    container_type _bytecode;
};

// ---------------------------------------------------------------------------
// ByteCodeIterator
// ---------------------------------------------------------------------------

class ByteCodeIterator
{
public: // public interface
    ByteCodeIterator(const ByteCode& bytecode)
        : _begin(bytecode.begin())
        , _curr(bytecode.begin())
        , _end(bytecode.end())
    {
    }

    auto to_begin() -> void
    {
        _curr = _begin;
    }

    auto to_end() -> void
    {
        _curr = _end;
    }

    auto next_byte() -> uint8_t
    {
        uint8_t value = 0;
        if((_end - _curr) >= 1) {
            value = ((value << 8) | (*_curr++));
        }
        return value;
    }

    auto next_word() -> uint16_t
    {
        uint16_t value = 0;
        if((_end - _curr) >= 2) {
            value = ((value << 8) | (*_curr++));
            value = ((value << 8) | (*_curr++));
        }
        return value;
    }

    auto next_long() -> uint32_t
    {
        uint32_t value = 0;
        if((_end - _curr) >= 4) {
            value = ((value << 8) | (*_curr++));
            value = ((value << 8) | (*_curr++));
            value = ((value << 8) | (*_curr++));
            value = ((value << 8) | (*_curr++));
        }
        return value;
    }

    operator bool() const
    {
        return _curr != _end;
    }

private: // private data
    ByteCode::const_iterator _begin;
    ByteCode::const_iterator _curr;
    ByteCode::const_iterator _end;
};

// ---------------------------------------------------------------------------
// StringIterator
// ---------------------------------------------------------------------------

class StringIterator
{
public: // public interface
    StringIterator(const std::string& string)
        : _begin(string.begin())
        , _curr(string.begin())
        , _end(string.end())
    {
    }

    auto begin() const -> std::string::const_iterator
    {
        return _begin;
    }

    auto curr() const -> std::string::const_iterator
    {
        return _curr;
    }

    auto end() const -> std::string::const_iterator
    {
        return _end;
    }

    auto at_begin() const -> bool
    {
        return _curr == _begin;
    }

    auto at_end() const -> bool
    {
        return _curr == _end;
    }

    auto advance(const size_t distance) -> void
    {
        if((_curr += distance) > _end) {
            _curr = _end;
        }
    }

    auto peek() -> std::string::value_type
    {
        if(_curr == _end) {
            return std::string::value_type();
        }
        return *_curr;
    }

    auto next() -> std::string::value_type
    {
        if(_curr == _end) {
            return std::string::value_type();
        }
        return *_curr++;
    }

    operator bool() const
    {
        return _curr != _end;
    }

private: // private data
    std::string::const_iterator _begin;
    std::string::const_iterator _curr;
    std::string::const_iterator _end;
};

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __ByteCode_h__ */
