/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "StringTypes.h"

uint32_t UTF8GetNext(const utf8* char_ptr, const utf8** nextchar_ptr);
bool UTF8IsCodepointStart(const utf8* text);
int32_t UTF8GetCodepointLength(char32_t codepoint);
int32_t UTF8Length(const utf8* text);

utf8* GetStringEnd(const utf8* text);
size_t GetStringSize(const utf8* text);

constexpr utf8* UTF8WriteCodepoint(utf8* dst, uint32_t codepoint)
{
    if (codepoint <= 0x7F)
    {
        dst[0] = static_cast<utf8>(codepoint);
        return dst + 1;
    }
    if (codepoint <= 0x7FF)
    {
        dst[0] = 0xC0 | ((codepoint >> 6) & 0x1F);
        dst[1] = 0x80 | (codepoint & 0x3F);
        return dst + 2;
    }
    if (codepoint <= 0xFFFF)
    {
        dst[0] = 0xE0 | ((codepoint >> 12) & 0x0F);
        dst[1] = 0x80 | ((codepoint >> 6) & 0x3F);
        dst[2] = 0x80 | (codepoint & 0x3F);
        return dst + 3;
    }

    dst[0] = 0xF0 | ((codepoint >> 18) & 0x07);
    dst[1] = 0x80 | ((codepoint >> 12) & 0x3F);
    dst[2] = 0x80 | ((codepoint >> 6) & 0x3F);
    dst[3] = 0x80 | (codepoint & 0x3F);
    return dst + 4;
}
