/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Game.h"
#include "../profiling/Profiling.h"
#include "../util/Math.hpp"
#include "Date.h"
#include "StringIds.h"

#include <algorithm>
#include <time.h>

uint16_t gDateMonthTicks;
int32_t gDateMonthsElapsed;

// rct2: 0x00993988
const int16_t days_in_month[MONTH_COUNT] = {
    31, 30, 31, 30, 31, 31, 30, 31,
};

const StringId DateFormatStringIDs[] = {
    STR_DATE_FORMAT_DAY_MONTH_YEAR,
    STR_DATE_FORMAT_MONTH_DAY_YEAR,
    STR_DATE_FORMAT_YEAR_MONTH_DAY,
    STR_DATE_FORMAT_YEAR_DAY_MONTH,
};

const StringId DateFormatStringFormatIds[] = {
    STR_DATE_FORMAT_DMY,
    STR_DATE_FORMAT_MDY,
    STR_DATE_FORMAT_YMD,
    STR_DATE_FORMAT_YDM,
};

RealWorldTime gRealTimeOfDay;

int32_t DateGetMonth(int32_t months)
{
    return months % MONTH_COUNT;
}

int32_t DateGetYear(int32_t months)
{
    return months / MONTH_COUNT;
}

int32_t DateGetTotalMonths(int32_t month, int32_t year)
{
    return (year - 1) * MONTH_COUNT + month;
}

/**
 *
 *  rct2: 0x006C4494
 */
void DateReset()
{
    gDateMonthsElapsed = 0;
    gDateMonthTicks = 0;
    gCurrentRealTimeTicks = 0;
}

void DateSet(int32_t year, int32_t month, int32_t day)
{
    year = std::clamp(year, 1, MAX_YEAR);
    month = std::clamp(month, 1, static_cast<int>(MONTH_COUNT));
    day = std::clamp(day, 1, static_cast<int>(days_in_month[month - 1]));
    gDateMonthsElapsed = (year - 1) * MONTH_COUNT + month - 1;
    gDateMonthTicks = TICKS_PER_MONTH / days_in_month[month - 1] * (day - 1) + 4;
}

void DateUpdate()
{
    PROFILED_FUNCTION();

    int32_t monthTicks = gDateMonthTicks + 4;
    if (monthTicks >= TICKS_PER_MONTH)
    {
        gDateMonthTicks = 0;
        gDateMonthsElapsed++;
    }
    else
    {
        gDateMonthTicks = Floor2(static_cast<uint16_t>(monthTicks), 4);
    }
}

void DateUpdateRealTimeOfDay()
{
    time_t timestamp = time(nullptr);
    struct tm* now = localtime(&timestamp);

    gRealTimeOfDay.second = now->tm_sec;
    gRealTimeOfDay.minute = now->tm_min;
    gRealTimeOfDay.hour = now->tm_hour;
}

bool DateIsDayStart(int32_t monthTicks)
{
    if (monthTicks < 4)
    {
        return false;
    }
    int32_t prevMonthTick = monthTicks - 4;
    int32_t currentMonth = DateGetMonth(gDateMonthsElapsed);
    int32_t currentDaysInMonth = days_in_month[currentMonth];
    return ((currentDaysInMonth * monthTicks) >> 16 != (currentDaysInMonth * prevMonthTick) >> 16);
}

bool DateIsWeekStart(int32_t monthTicks)
{
    return (monthTicks & 0x3FFF) == 0;
}

bool DateIsFortnightStart(int32_t monthTicks)
{
    return (monthTicks & 0x7FFF) == 0;
}

bool DateIsMonthStart(int32_t monthTicks)
{
    return (monthTicks == 0);
}
