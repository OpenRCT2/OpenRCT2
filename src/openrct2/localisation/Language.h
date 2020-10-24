/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _LANGUAGE_H_
#define _LANGUAGE_H_

#include "../common.h"
#include "../drawing/Font.h"

#include <string>
#include <string_view>

enum
{
    LANGUAGE_UNDEFINED,
    LANGUAGE_ARABIC,
    LANGUAGE_CATALAN,
    LANGUAGE_CHINESE_SIMPLIFIED,
    LANGUAGE_CHINESE_TRADITIONAL,
    LANGUAGE_CZECH,
    LANGUAGE_DANISH,
    LANGUAGE_GERMAN,
    LANGUAGE_ENGLISH_UK,
    LANGUAGE_ENGLISH_US,
    LANGUAGE_SPANISH,
    LANGUAGE_FRENCH,
    LANGUAGE_ITALIAN,
    LANGUAGE_JAPANESE,
    LANGUAGE_KOREAN,
    LANGUAGE_HUNGARIAN,
    LANGUAGE_DUTCH,
    LANGUAGE_NORWEGIAN,
    LANGUAGE_POLISH,
    LANGUAGE_PORTUGUESE_BR,
    LANGUAGE_RUSSIAN,
    LANGUAGE_FINNISH,
    LANGUAGE_SWEDISH,
    LANGUAGE_TURKISH,
    LANGUAGE_COUNT
};

enum RCT2LanguageId
{
    RCT2_LANGUAGE_ID_ENGLISH_UK,
    RCT2_LANGUAGE_ID_ENGLISH_US,
    RCT2_LANGUAGE_ID_FRENCH,
    RCT2_LANGUAGE_ID_GERMAN,
    RCT2_LANGUAGE_ID_SPANISH,
    RCT2_LANGUAGE_ID_ITALIAN,
    RCT2_LANGUAGE_ID_DUTCH,
    RCT2_LANGUAGE_ID_SWEDISH,
    RCT2_LANGUAGE_ID_JAPANESE,
    RCT2_LANGUAGE_ID_KOREAN,
    RCT2_LANGUAGE_ID_CHINESE_SIMPLIFIED,
    RCT2_LANGUAGE_ID_CHINESE_TRADITIONAL,
    RCT2_LANGUAGE_ID_12,
    RCT2_LANGUAGE_ID_PORTUGUESE,
    RCT2_LANGUAGE_ID_BLANK = 254,
    RCT2_LANGUAGE_ID_END = 255
};

#define FONT_OPENRCT2_SPRITE NULL

#include "../interface/FontFamilies.h"

struct language_descriptor
{
    const char* locale;
    const utf8* english_name;
    const utf8* native_name;
#if !defined(NO_TTF)
    TTFontFamily const* font_family;
#else
    void* font_family;
#endif
    bool isRtl;
};

extern const language_descriptor LanguagesDescriptors[LANGUAGE_COUNT];

extern const utf8 BlackUpArrowString[];
extern const utf8 BlackDownArrowString[];
extern const utf8 BlackLeftArrowString[];
extern const utf8 BlackRightArrowString[];
extern const utf8 CheckBoxMarkString[];

uint8_t language_get_id_from_locale(const char* locale);
const char* language_get_string(rct_string_id id);
bool language_open(int32_t id);

uint32_t utf8_get_next(const utf8* char_ptr, const utf8** nextchar_ptr);
int32_t utf8_insert_codepoint(utf8* dst, uint32_t codepoint);
bool utf8_is_codepoint_start(const utf8* text);
void utf8_remove_format_codes(utf8* text, bool allowcolours);
int32_t utf8_get_codepoint_length(char32_t codepoint);
int32_t utf8_length(const utf8* text);

std::string rct2_to_utf8(const std::string_view& src, RCT2LanguageId languageId);
std::string utf8_to_rct2(const std::string_view& src);
bool language_get_localised_scenario_strings(const utf8* scenarioFilename, rct_string_id* outStringIds);
void language_free_object_string(rct_string_id stringId);
rct_string_id language_allocate_object_string(const std::string& target);
std::string language_convert_string_to_tokens(const std::string_view& s);
std::string language_convert_string(const std::string_view& s);

constexpr utf8* utf8_write_codepoint(utf8* dst, uint32_t codepoint)
{
    if (codepoint <= 0x7F)
    {
        dst[0] = static_cast<utf8>(codepoint);
        return dst + 1;
    }
    else if (codepoint <= 0x7FF)
    {
        dst[0] = 0xC0 | ((codepoint >> 6) & 0x1F);
        dst[1] = 0x80 | (codepoint & 0x3F);
        return dst + 2;
    }
    else if (codepoint <= 0xFFFF)
    {
        dst[0] = 0xE0 | ((codepoint >> 12) & 0x0F);
        dst[1] = 0x80 | ((codepoint >> 6) & 0x3F);
        dst[2] = 0x80 | (codepoint & 0x3F);
        return dst + 3;
    }
    else
    {
        dst[0] = 0xF0 | ((codepoint >> 18) & 0x07);
        dst[1] = 0x80 | ((codepoint >> 12) & 0x3F);
        dst[2] = 0x80 | ((codepoint >> 6) & 0x3F);
        dst[3] = 0x80 | (codepoint & 0x3F);
        return dst + 4;
    }
}

#endif
