/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

constexpr int32_t MAX_YEAR = 8192;
constexpr int32_t TICKS_PER_MONTH = 0x10000;

enum
{
    MONTH_MARCH,
    MONTH_APRIL,
    MONTH_MAY,
    MONTH_JUNE,
    MONTH_JULY,
    MONTH_AUGUST,
    MONTH_SEPTEMBER,
    MONTH_OCTOBER,

    MONTH_COUNT
};

enum
{
    DATE_FORMAT_DAY_MONTH_YEAR,
    DATE_FORMAT_MONTH_DAY_YEAR,
    DATE_FORMAT_YEAR_MONTH_DAY,
    DATE_FORMAT_YEAR_DAY_MONTH
};

struct openrct2_timeofday
{
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
};

extern const int16_t days_in_month[MONTH_COUNT];
extern const StringId DateFormatStringIDs[];
extern const StringId DateFormatStringFormatIds[];

extern uint16_t gDateMonthTicks;
extern int32_t gDateMonthsElapsed;

extern openrct2_timeofday gRealTimeOfDay;

int32_t DateGetMonth(int32_t months);
int32_t DateGetYear(int32_t months);
int32_t DateGetTotalMonths(int32_t month, int32_t year);
void DateReset();
void DateUpdate();
void DateSet(int32_t year, int32_t month, int32_t day);
void DateUpdateRealTimeOfDay();
bool DateIsDayStart(int32_t monthTicks);
bool DateIsWeekStart(int32_t monthTicks);
bool DateIsFortnightStart(int32_t monthTicks);
bool DateIsMonthStart(int32_t monthTicks);
