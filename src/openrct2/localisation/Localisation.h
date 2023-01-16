/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../management/Marketing.h"
#include "Currency.h"
#include "Date.h"
#include "FormatCodes.h"
#include "Language.h"
#include "StringIds.h"

#include <string>

std::string FormatStringID(StringId format, const void* args);
void format_string(char* dest, size_t size, StringId format, const void* args);
void FormatStringToUpper(char* dest, size_t size, StringId format, const void* args);
void generate_string_file();

/**
 * Formats sizeBytes into buf as a human readable text, e.x.: "1024 MB"
 */
void FormatReadableSize(char* buf, size_t bufSize, uint64_t sizeBytes);

/**
 * Formats sizeBytesPerSec into buf as a human readable text, e.x.: "1024 MB/sec"
 */
void FormatReadableSpeed(char* buf, size_t bufSize, uint64_t sizeBytesPerSec);

utf8* GetStringEnd(const utf8* text);
size_t GetStringSize(const utf8* text);

// The maximum number of characters allowed for string/money conversions (anything above will risk integer overflow issues)
#define MONEY_STRING_MAXLENGTH 14
money64 StringToMoney(const char* string_to_monetise);
void MoneyToString(money64 amount, char* buffer_to_put_value_to, size_t buffer_len, bool forceDecimals);

bool is_user_string_id(StringId stringId);

#define MAX_USER_STRINGS 1024
#define USER_STRING_MAX_LENGTH 32

#define USER_STRING_START 0x8000
#define USER_STRING_END 0x8FFF
#define REAL_NAME_START 0xA000
#define REAL_NAME_END 0xDFFF

constexpr const size_t CommonTextBufferSize = 512;

// Real name data
extern const char real_name_initials[16];
extern const char* real_names[1024];

extern thread_local char gCommonStringFormatBuffer[CommonTextBufferSize];
extern bool gDebugStringFormatting;

extern const StringId SpeedNames[5];
extern const StringId ObjectiveNames[12];
extern const StringId ResearchFundingLevelNames[4];
extern const StringId MarketingCampaignNames[ADVERTISING_CAMPAIGN_COUNT][3];
extern const StringId RideInspectionIntervalNames[];
extern const StringId PeepThoughts[174];
extern const StringId DateDayNames[31];
extern const StringId DateGameMonthNames[MONTH_COUNT];
extern const StringId DateGameShortMonthNames[MONTH_COUNT];
