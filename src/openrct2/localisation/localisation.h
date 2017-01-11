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

#ifndef LOCALISATION_H
#define LOCALISATION_H

#include "currency.h"
#include "date.h"
#include "format_codes.h"
#include "language.h"
#include "string_ids.h"
#include "../management/marketing.h"

bool utf8_is_format_code(sint32 codepoint);
bool utf8_is_colour_code(sint32 codepoint);
bool utf8_should_use_sprite_for_codepoint(sint32 codepoint);
sint32 utf8_get_format_code_arg_length(sint32 codepoint);
void utf8_remove_formatting(utf8* string, bool allowColours);

void format_string(char *dest, size_t size, rct_string_id format, void *args);
void format_string_raw(char *dest, size_t size, char *src, void *args);
void format_string_to_upper(char *dest, size_t size, rct_string_id format, void *args);
void generate_string_file();
utf8 *get_string_end(const utf8 *text);
size_t get_string_size(const utf8 *text);
sint32 get_string_length(const utf8 *text);

// The maximum number of characters allowed for string/money conversions (anything above will risk integer overflow issues)
#define MONEY_STRING_MAXLENGTH 14
money32 string_to_money(char * string_to_monetise);
void money_to_string(money32 amount, char * buffer_to_put_value_to, size_t buffer_len);

void user_string_clear_all();
rct_string_id user_string_allocate(sint32 base, const utf8 *text);
void user_string_free(rct_string_id id);
bool is_user_string_id(rct_string_id stringId);

utf8 *win1252_to_utf8_alloc(const char *src, size_t srcMaxSize);
sint32 win1252_to_utf8(utf8string dst, const char *src, size_t srcLength, size_t maxBufferLength);

sint32 rct2_to_utf8(utf8 *dst, const char *src);
sint32 utf8_to_rct2(char *dst, const utf8 *src);
wchar_t encoding_convert_rct2_to_unicode(wchar_t rct2str);
wchar_t encoding_convert_unicode_to_rct2(wchar_t unicode);
wchar_t encoding_convert_gb2312_to_unicode(wchar_t gb2312);
wchar_t encoding_convert_big5_to_unicode(wchar_t big5);

#define MAX_USER_STRINGS 1024
#define USER_STRING_MAX_LENGTH 32

// Constants used by user_string_allocate
enum {
    USER_STRING_HIGH_ID_NUMBER = 1 << 2,
    USER_STRING_DUPLICATION_PERMITTED = 1 << 7
};

// Real name data
extern const char real_name_initials[16];
extern const char *real_names[1024];

extern utf8 gUserStrings[MAX_USER_STRINGS * USER_STRING_MAX_LENGTH];
extern char gCommonStringFormatBuffer[256];
extern uint8 gCommonFormatArgs[80];
extern uint8 gMapTooltipFormatArgs[40];
extern bool gDebugStringFormatting;

extern const rct_string_id SpeedNames[5];
extern const rct_string_id ObjectiveNames[12];
extern const rct_string_id ResearchFundingLevelNames[4];
extern const rct_string_id MarketingCampaignNames[ADVERTISING_CAMPAIGN_COUNT][3];
extern const rct_string_id RideInspectionIntervalNames[];
extern const rct_string_id PeepThoughts[174];
extern const rct_string_id DateDayNames[31];
extern const rct_string_id DateGameMonthNames[MONTH_COUNT];
extern const rct_string_id DateGameShortMonthNames[MONTH_COUNT];

static inline void set_format_arg_body(uint8 *args, size_t offset, uintptr_t value, size_t size)
{
    memcpy(args + offset, &value, size);
}

#define set_format_arg(offset, type, value) \
    do { static_assert(sizeof(type) <= sizeof(uintptr_t), "Type too large"); \
    set_format_arg_body(gCommonFormatArgs, offset, (uintptr_t)value, sizeof(type)); } while (0)

#define set_format_arg_on(args, offset, type, value)	\
	set_format_arg_body(args, offset, (uintptr_t)value, sizeof(type))

#define set_map_tooltip_format_arg(offset, type, value) \
    do { static_assert(sizeof(type) <= sizeof(uintptr_t), "Type too large"); \
    set_format_arg_body(gMapTooltipFormatArgs, offset, (uintptr_t)value, sizeof(type)); } while (0)

#endif
