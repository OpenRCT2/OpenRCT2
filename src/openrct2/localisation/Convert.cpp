#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../core/Util.hpp"
#include "ConversionTables.h"
#include "Localisation.h"

sint32 rct2_to_utf8(utf8 *dst, const char *src)
{
    wchar_t codepoint;

    utf8 *start = dst;
    const char *ch = src;
    while (*ch != 0) {
        if (*ch == (char)(uint8)0xFF) {
            ch++;

            // Read wide char
            uint8 a = *ch++;
            uint8 b = *ch++;
            codepoint = (a << 8) | b;
        } else {
            codepoint = (uint8)(*ch++);
            codepoint = encoding_convert_rct2_to_unicode(codepoint);
        }

        dst = utf8_write_codepoint(dst, codepoint);
    }
    dst = utf8_write_codepoint(dst, 0);
    return (sint32)(dst - start);
}

sint32 utf8_to_rct2(char *dst, const utf8 *src)
{
    char *start = dst;
    const utf8 *ch = src;
    sint32 codepoint;
    while ((codepoint = utf8_get_next(ch, &ch)) != 0) {
        codepoint = encoding_convert_unicode_to_rct2(codepoint);
        if (codepoint < 256) {
            *dst++ = (char)codepoint;
        } else if (codepoint <= 0xFFFF) {
            *dst++ = (char)(uint8)0xFF;
            *dst++ = (codepoint >> 8) & 0xFF;
            *dst++ = codepoint & 0xFF;
        }
    }
    *dst++ = 0;
    return (sint32)(dst - start);
}

static sint32 encoding_search_compare(const void *pKey, const void *pEntry)
{
    uint16 key = *((uint16*)pKey);
    encoding_convert_entry *entry = (encoding_convert_entry*)pEntry;
    if (key < entry->code) return -1;
    if (key > entry->code) return 1;
    return 0;
}

static wchar_t encoding_convert_x_to_unicode(wchar_t code, const encoding_convert_entry *table, size_t count)
{
    encoding_convert_entry * entry = (encoding_convert_entry *)bsearch(&code, table, count, sizeof(encoding_convert_entry), encoding_search_compare);
    if (entry == nullptr) return code;
    else return entry->unicode;
}

uint32 encoding_convert_unicode_to_rct2(uint32 unicode)
{
    // Can't do a binary search as it's sorted by RCT2 code, not unicode
    for (uint32 i = 0; i < Util::CountOf(RCT2ToUnicodeTable); i++) {
        if (RCT2ToUnicodeTable[i].unicode == unicode) return RCT2ToUnicodeTable[i].code;
    }
    return unicode;
}

wchar_t encoding_convert_rct2_to_unicode(wchar_t rct2str)
{
    return encoding_convert_x_to_unicode(rct2str, RCT2ToUnicodeTable, Util::CountOf(RCT2ToUnicodeTable));
}

wchar_t encoding_convert_gb2312_to_unicode(wchar_t gb2312)
{
    return encoding_convert_x_to_unicode(gb2312 - 0x8080, GB2312ToUnicodeTable, Util::CountOf(GB2312ToUnicodeTable));
}

wchar_t encoding_convert_big5_to_unicode(wchar_t big5)
{
    return encoding_convert_x_to_unicode(big5, Big5ToUnicodeTable, Util::CountOf(Big5ToUnicodeTable));
}

wchar_t encoding_convert_cp932_to_unicode(wchar_t cp932)
{
    return encoding_convert_x_to_unicode(cp932, CP932ToUnicodeTable, Util::CountOf(CP932ToUnicodeTable));
}

wchar_t encoding_convert_cp949_to_unicode(wchar_t cp949)
{
    return encoding_convert_x_to_unicode(cp949, CP949ToUnicodeTable, Util::CountOf(CP949ToUnicodeTable));
}

