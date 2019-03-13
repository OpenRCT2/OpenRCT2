/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef LOCALISATION_H
#define LOCALISATION_H

#include "../management/Marketing.h"
#include "Currency.h"
#include "Date.h"
#include "FormatCodes.h"
#include "Language.h"
#include "StringIds.h"

#include <cstring>

bool utf8_is_format_code(int32_t codepoint);
bool utf8_is_colour_code(int32_t codepoint);
bool utf8_should_use_sprite_for_codepoint(int32_t codepoint);
int32_t utf8_get_format_code_arg_length(int32_t codepoint);
void utf8_remove_formatting(utf8* string, bool allowColours);

void format_string(char* dest, size_t size, rct_string_id format, const void* args);
void format_string_raw(char* dest, size_t size, const char* src, const void* args);
void format_string_to_upper(char* dest, size_t size, rct_string_id format, const void* args);
void generate_string_file();

/**
 * Formats sizeBytes into buf as a human readable text, e.x.: "1024 MB"
 */
void format_readable_size(char* buf, size_t bufSize, uint64_t sizeBytes);

/**
 * Formats sizeBytesPerSec into buf as a human readable text, e.x.: "1024 MB/sec"
 */
void format_readable_speed(char* buf, size_t bufSize, uint64_t sizeBytesPerSec);

utf8* get_string_end(const utf8* text);
size_t get_string_size(const utf8* text);
int32_t get_string_length(const utf8* text);

// The maximum number of characters allowed for string/money conversions (anything above will risk integer overflow issues)
#define MONEY_STRING_MAXLENGTH 14
money32 string_to_money(const char* string_to_monetise);
void money_to_string(money32 amount, char* buffer_to_put_value_to, size_t buffer_len, bool forceDecimals);

void user_string_clear_all();
rct_string_id user_string_allocate(int32_t base, const utf8* text);
void user_string_free(rct_string_id id);
bool is_user_string_id(rct_string_id stringId);

#define MAX_USER_STRINGS 1024
#define USER_STRING_MAX_LENGTH 32

#define USER_STRING_START 0x8000
#define USER_STRING_END 0x8FFF
#define REAL_NAME_START 0xA000
#define REAL_NAME_END 0xDFFF

// Constants used by user_string_allocate
enum
{
    USER_STRING_HIGH_ID_NUMBER = 1 << 2,
    USER_STRING_DUPLICATION_PERMITTED = 1 << 7
};

// Real name data
extern const char real_name_initials[16];
extern const char* real_names[1024];

extern utf8 gUserStrings[MAX_USER_STRINGS][USER_STRING_MAX_LENGTH];
extern char gCommonStringFormatBuffer[512];
extern uint8_t gCommonFormatArgs[80];
extern uint8_t gMapTooltipFormatArgs[40];
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

[[maybe_unused]] static inline void set_format_arg_body(uint8_t* args, size_t offset, uintptr_t value, size_t size)
{
    std::memcpy(args + offset, &value, size);
}

#define set_format_arg(offset, type, value)                                                                                    \
    do                                                                                                                         \
    {                                                                                                                          \
        static_assert(sizeof(type) <= sizeof(uintptr_t), "Type too large");                                                    \
        set_format_arg_body(gCommonFormatArgs, offset, (uintptr_t)(value), sizeof(type));                                      \
    } while (false)

#define set_format_arg_on(args, offset, type, value) set_format_arg_body(args, offset, (uintptr_t)(value), sizeof(type))

#define set_map_tooltip_format_arg(offset, type, value)                                                                        \
    do                                                                                                                         \
    {                                                                                                                          \
        static_assert(sizeof(type) <= sizeof(uintptr_t), "Type too large");                                                    \
        set_format_arg_body(gMapTooltipFormatArgs, offset, (uintptr_t)(value), sizeof(type));                                  \
    } while (false)

#endif
