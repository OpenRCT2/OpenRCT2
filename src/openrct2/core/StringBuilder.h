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
#include "../core/String.hpp"

#include <string>

/**
 * Class for constructing strings efficiently. A buffer is automatically allocated and reallocated when characters or strings
 * are appended. Use GetString to copy the current state of the string builder to a new fire-and-forget string.
 */
class StringBuilder final
{
public:
    StringBuilder() = default;
    explicit StringBuilder(size_t capacity);
    ~StringBuilder() = default;

    void Append(int32_t codepoint);

    /**
     * Appends the given character to the current string.
     */
    void Append(codepoint_t codepoint);

    /**
     * Appends the given string to the current string.
     */
    void Append(std::string_view text);

    /**
     * Appends the given string of the given length to the current string. Essentially used to ignore null terminators or copy
     * the data faster as the length is already known.
     * @param text Pointer to the UTF-8 text to append.
     * @param textLength Number of bytes to copy. (Can be used to append single bytes rather than codepoints)
     */
    void Append(const utf8* text, size_t textLength);

    /**
     * Appends the string of a given StringBuilder to the current string.
     */
    void Append(const StringBuilder* sb);

    /**
     * Clears the current string, but preserves the allocated memory for another string.
     */
    void Clear();

    /**
     * Returns the current string buffer as a standard string.
     */
    std::string GetStdString() const;

    /**
     * Gets the current state of the StringBuilder. Warning: this represents the StringBuilder's current working buffer and will
     * be deallocated when the StringBuilder is destructed.
     */
    const utf8* GetBuffer() const;

    /**
     * Gets the length of the current string.
     */
    size_t GetLength() const;

private:
    std::basic_string<utf8> _buffer;
};
