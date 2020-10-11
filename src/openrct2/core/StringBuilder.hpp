/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "String.hpp"

#include <algorithm>
#include <iterator>
#include <string>

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
        _buffer.reserve(capacity);
    }

    ~StringBuilder() = default;

    void Append(int32_t codepoint)
    {
        Append(static_cast<codepoint_t>(codepoint));
    }

    /**
     * Appends the given character to the current string.
     */
    void Append(codepoint_t codepoint)
    {
        size_t codepointLength = String::GetCodepointLength(codepoint);
        std::basic_string<utf8> data(codepointLength, {});
        String::WriteCodepoint(data.data(), codepoint);
        _buffer.insert(_buffer.end(), data.begin(), data.end());
    }

    /**
     * Appends the given string to the current string.
     */
    void Append(const utf8* text)
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
    void Append(const utf8* text, size_t textLength)
    {
        _buffer.insert(_buffer.end(), text, text + textLength);
    }

    /**
     * Appends the string of a given StringBuilder to the current string.
     */
    void Append(const StringBuilder* sb)
    {
        Append(sb->GetBuffer(), sb->GetLength());
    }

    /**
     * Clears the current string, but preserves the allocated memory for another string.
     */
    void Clear()
    {
        _buffer.clear();
    }

    /**
     * Returns the current string buffer as a standard string.
     */
    std::string GetStdString() const
    {
        return std::string(GetBuffer(), GetLength());
    }

    /**
     * Gets the current state of the StringBuilder. Warning: this represents the StringBuilder's current working buffer and will
     * be deallocated when the StringBuilder is destructed.
     */
    const utf8* GetBuffer() const
    {
        // buffer may be empty, so return an immutable empty string
        if (_buffer.empty())
            return "";
        return _buffer.c_str();
    }

    /**
     * Gets the length of the current string.
     */
    size_t GetLength() const
    {
        return _buffer.size();
    }

private:
    std::basic_string<utf8> _buffer;
};
