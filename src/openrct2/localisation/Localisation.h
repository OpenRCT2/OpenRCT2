/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
#include <string>

bool utf8_is_format_code(char32_t codepoint);
bool utf8_is_colour_code(char32_t codepoint);
bool utf8_should_use_sprite_for_codepoint(char32_t codepoint);
int32_t utf8_get_format_code_arg_length(char32_t codepoint);
void utf8_remove_formatting(utf8* string, bool allowColours);

std::string format_string(rct_string_id format, const void* args);
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

bool is_user_string_id(rct_string_id stringId);

#define MAX_USER_STRINGS 1024
#define USER_STRING_MAX_LENGTH 32

#define USER_STRING_START 0x8000
#define USER_STRING_END 0x8FFF
#define REAL_NAME_START 0xA000
#define REAL_NAME_END 0xDFFF

// Real name data
extern const char real_name_initials[16];
extern const char* real_names[1024];

extern thread_local char gCommonStringFormatBuffer[512];
extern thread_local uint8_t gCommonFormatArgs[80];
extern thread_local uint8_t gMapTooltipFormatArgs[40];
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

class Formatter
{
    const uint8_t* StartBuf;
    uint8_t* CurrentBuf;

public:
    explicit Formatter(uint8_t* buf)
        : StartBuf(buf)
        , CurrentBuf(buf)
    {
    }

    static Formatter Common()
    {
        return Formatter(gCommonFormatArgs);
    }

    static Formatter MapTooltip()
    {
        return Formatter(gMapTooltipFormatArgs);
    }

    auto Buf()
    {
        return CurrentBuf;
    }

    auto GetStartBuf() const
    {
        return StartBuf;
    }

    void Increment(size_t count)
    {
        CurrentBuf += count;
    }

    void Rewind()
    {
        CurrentBuf -= NumBytes();
    }

    std::size_t NumBytes() const
    {
        return CurrentBuf - StartBuf;
    }

    template<typename TSpecified, typename TDeduced> Formatter& Add(TDeduced value)
    {
        static_assert(sizeof(TSpecified) <= sizeof(uintptr_t), "Type too large");
        static_assert(sizeof(TDeduced) <= sizeof(uintptr_t), "Type too large");

        // clang-format off
        static_assert(
            std::is_same_v<TSpecified, char*> ||
            std::is_same_v<TSpecified, const char*> ||
            std::is_same_v<TSpecified, int16_t> ||
            std::is_same_v<TSpecified, int32_t> ||
            std::is_same_v<TSpecified, money32> ||
            std::is_same_v<TSpecified, rct_string_id> ||
            std::is_same_v<TSpecified, uint16_t> ||
            std::is_same_v<TSpecified, uint32_t> ||
            std::is_same_v<TSpecified, utf8*> ||
            std::is_same_v<TSpecified, const utf8*>
        );
        // clang-format on

        uintptr_t convertedValue;
        if constexpr (std::is_integral_v<TSpecified>)
        {
            convertedValue = static_cast<uintptr_t>(value);
        }
        else
        {
            convertedValue = reinterpret_cast<uintptr_t>(value);
        }
        std::memcpy(CurrentBuf, &convertedValue, sizeof(TSpecified));
        Increment(sizeof(TSpecified));
        return *this;
    }
};

#endif
