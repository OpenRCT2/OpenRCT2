/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Localisation.h"

#include <cstring>
#include <wchar.h>

uint32_t UTF8GetNext(const utf8* char_ptr, const utf8** nextchar_ptr)
{
    int32_t result;
    int32_t numBytes;

    if (!(char_ptr[0] & 0x80))
    {
        result = char_ptr[0];
        numBytes = 1;
    }
    else if ((char_ptr[0] & 0xE0) == 0xC0)
    {
        result = ((char_ptr[0] & 0x1F) << 6) | (char_ptr[1] & 0x3F);
        numBytes = 2;
    }
    else if ((char_ptr[0] & 0xF0) == 0xE0)
    {
        result = ((char_ptr[0] & 0x0F) << 12) | ((char_ptr[1] & 0x3F) << 6) | (char_ptr[2] & 0x3F);
        numBytes = 3;
    }
    else if ((char_ptr[0] & 0xF8) == 0xF0)
    {
        result = ((char_ptr[0] & 0x07) << 18) | ((char_ptr[1] & 0x3F) << 12) | ((char_ptr[2] & 0x3F) << 6)
            | (char_ptr[3] & 0x3F);
        numBytes = 4;
    }
    else
    {
        // TODO 4 bytes
        result = ' ';
        numBytes = 1;
    }

    if (nextchar_ptr != nullptr)
        *nextchar_ptr = char_ptr + numBytes;
    return result;
}

/**
 * Inserts the given codepoint at the given address, shifting all characters after along.
 * @returns the size of the inserted codepoint.
 */
int32_t UTF8InsertCodepoint(utf8* dst, uint32_t codepoint)
{
    int32_t shift = UTF8GetCodepointLength(codepoint);
    utf8* endPoint = GetStringEnd(dst);
    memmove(dst + shift, dst, endPoint - dst + 1);
    UTF8WriteCodepoint(dst, codepoint);
    return shift;
}

bool UTF8IsCodepointStart(const utf8* text)
{
    if ((text[0] & 0x80) == 0)
        return true;
    if ((text[0] & 0xC0) == 0xC0)
        return true;
    return false;
}

int32_t UTF8GetCodepointLength(char32_t codepoint)
{
    if (codepoint <= 0x7F)
    {
        return 1;
    }
    if (codepoint <= 0x7FF)
    {
        return 2;
    }
    if (codepoint <= 0xFFFF)
    {
        return 3;
    }
    return 4;
}

/**
 * Gets the number of characters / codepoints in a UTF-8 string (not necessarily 1:1 with bytes and not including null
 * terminator).
 */
int32_t UTF8Length(const utf8* text)
{
    const utf8* ch = text;

    int32_t count = 0;
    while (UTF8GetNext(ch, &ch) != 0)
    {
        count++;
    }
    return count;
}

/**
 * Returns a pointer to the null terminator of the given UTF-8 string.
 */
utf8* GetStringEnd(const utf8* text)
{
    return const_cast<char*>(std::strchr(text, 0));
}

/**
 * Return the number of bytes (including the null terminator) in the given UTF-8 string.
 */
size_t GetStringSize(const utf8* text)
{
    return GetStringEnd(text) - text + 1;
}
