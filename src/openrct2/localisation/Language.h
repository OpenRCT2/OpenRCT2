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

#ifndef _LANGUAGE_H_
#define _LANGUAGE_H_

#include <string>
#include "../common.h"
#include "../drawing/Font.h"

enum {
    LANGUAGE_UNDEFINED,
    LANGUAGE_ARABIC,
    LANGUAGE_CATALAN,
    LANGUAGE_CHINESE_SIMPLIFIED,
    LANGUAGE_CHINESE_TRADITIONAL,
    LANGUAGE_CZECH,
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

struct language_descriptor {
    const char *locale;
    const utf8 *english_name;
    const utf8 *native_name;
#if !defined(NO_TTF)
    TTFontFamily const * font_family;
#else
    void * font_family;
#endif
    RCT2LanguageId rct2_original_id;
};

extern const language_descriptor LanguagesDescriptors[LANGUAGE_COUNT];

extern sint32 gCurrentLanguage;
extern bool gUseTrueTypeFont;

extern const utf8 BlackUpArrowString[];
extern const utf8 BlackDownArrowString[];
extern const utf8 BlackLeftArrowString[];
extern const utf8 BlackRightArrowString[];
extern const utf8 CheckBoxMarkString[];

uint8 language_get_id_from_locale(const char * locale);
const char *language_get_string(rct_string_id id);
bool language_open(sint32 id);
void language_close_all();

uint32 utf8_get_next(const utf8 *char_ptr, const utf8 **nextchar_ptr);
utf8 *utf8_write_codepoint(utf8 *dst, uint32 codepoint);
sint32 utf8_insert_codepoint(utf8 *dst, uint32 codepoint);
bool utf8_is_codepoint_start(const utf8 *text);
void utf8_remove_format_codes(utf8 *text, bool allowcolours);
sint32 utf8_get_codepoint_length(sint32 codepoint);
sint32 utf8_length(const utf8 *text);
wchar_t *utf8_to_widechar(const utf8 *src);
utf8 *widechar_to_utf8(const wchar_t *src);

utf8 *rct2_language_string_to_utf8(const char *src, size_t srcSize, RCT2LanguageId languageId);
bool language_get_localised_scenario_strings(const utf8 *scenarioFilename, rct_string_id *outStringIds);
void language_free_object_string(rct_string_id stringId);
rct_string_id language_get_object_override_string_id(const char * identifier, uint8 index);
rct_string_id language_allocate_object_string(const std::string &target);

#endif
