/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <algorithm>
#include "../common.h"

#include "Math.hpp"
#include "Memory.hpp"
#include "String.hpp"

/**
 * Class for constructing strings efficiently. A buffer is automatically allocated and reallocated when characters or strings
 * are appended. Use GetString to copy the current state of the string builder to a new fire-and-forget string.
 */
class StringBuilder final
{
public:
    StringBuilder() = default;
    explicit StringBuilder(size_t capacity)
    {
        EnsureCapacity(capacity);
    }

    ~StringBuilder()
    {
        Memory::Free(_buffer);
    }

    void Append(int32_t codepoint)
    {
        Append((codepoint_t)codepoint);
    }

    /**
     * Appends the given character to the current string.
     */
    void Append(codepoint_t codepoint)
    {
        size_t codepointLength = String::GetCodepointLength(codepoint);
        EnsureCapacity(_length + codepointLength + 1);
        String::WriteCodepoint(_buffer + _length, codepoint);
        _length += codepointLength;
        _buffer[_length] = 0;
    }

    /**
     * Appends the given string to the current string.
     */
    void Append(const utf8 * text)
    {
        size_t textLength = String::SizeOf(text);
        Append(text, textLength);
    }

    /**
     * Appends the given string of the given length to the current string. Essentially used to ignore null terminators or copy
     * the data faster as the length is already known.
     * @param text Pointer to the UTF-8 text to append.
     * @param textLength Number of bytes to copy. (Can be used to append single bytes rather than codepoints)
     */
    void Append(const utf8 * text, size_t textLength)
    {
        EnsureCapacity(_length + textLength + 1);
        std::copy_n(text, textLength, _buffer + _length);
        _length += textLength;
        _buffer[_length] = 0;
    }

    /**
     * Appends the string of a given StringBuilder to the current string.
     */
    void Append(const StringBuilder * sb)
    {
        Append(sb->GetBuffer(), sb->GetLength());
    }

    /**
     * Clears the current string, but preserves the allocated memory for another string.
     */
    void Clear()
    {
        _length = 0;
        if (_capacity >= 1)
        {
            _buffer[_length] = 0;
        }
    }

    /**
     * Like Clear, only will guarantee freeing of the underlying buffer.
     */
    void Reset()
    {
        _length = 0;
        _capacity = 0;
        SafeFree(_buffer);
    }

    /**
     * Resets the StringBuilder and returns the working buffer (resized to the string size).
     */
    utf8 * StealString()
    {
        utf8 * result = _buffer;
        result = Memory::ReallocateArray<utf8>(result, _length + 1);
        result[_length] = 0;

        _length = 0;
        _capacity = 0;
        _buffer = nullptr;

        return result;
    }

    /**
     * Returns the current string buffer as a new fire-and-forget string.
     */
    utf8 * GetString() const
    {
        // If buffer is null, length should be 0 which will create a new one byte memory block containing a null terminator
        utf8 * result = Memory::AllocateArray<utf8>(_length + 1);
        std::copy_n(_buffer, _length, result);
        result[_length] = 0;
        return result;
    }

    /**
     * Gets the current state of the StringBuilder. Warning: this represents the StringBuilder's current working buffer and will
     * be deallocated when the StringBuilder is destructed.
     */
    const utf8 * GetBuffer() const
    {
        // buffer may be null, so return an immutable empty string
        if (_buffer == nullptr) return "";
        return _buffer;
    }

    /**
     * Gets the amount of allocated memory for the string buffer.
     */
    size_t GetCapacity() const { return _capacity; }

    /**
     * Gets the length of the current string.
     */
    size_t GetLength() const { return _length; }

private:
    utf8 * _buffer = nullptr;
    size_t _capacity = 0;
    size_t _length = 0;

    void EnsureCapacity(size_t capacity)
    {
        if (_capacity > capacity) return;

        _capacity = std::max((size_t)8, _capacity);
        while (_capacity < capacity)
        {
            _capacity *= 2;
        }

        _buffer = Memory::ReallocateArray<utf8>(_buffer, _capacity);
    }
};
