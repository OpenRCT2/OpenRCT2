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

#include <time.h>
#include "../Game.h"
#include "../core/Math.hpp"
#include "Date.h"
#include "StringIds.h"

uint16 gDateMonthTicks;
uint16 gDateMonthsElapsed;

// rct2: 0x00993988
const sint16 days_in_month[MONTH_COUNT] = { 31, 30, 31, 30, 31, 31, 30, 31 };

// clang-format off
const rct_string_id DateFormatStringIds[] = {
    STR_DATE_FORMAT_DAY_MONTH_YEAR,
    STR_DATE_FORMAT_MONTH_DAY_YEAR,
    STR_DATE_FORMAT_YEAR_MONTH_DAY,
    STR_DATE_FORMAT_YEAR_DAY_MONTH
};

const rct_string_id DateFormatStringFormatIds[] = {
    STR_DATE_FORMAT_DMY,
    STR_DATE_FORMAT_MDY,
    STR_DATE_FORMAT_YMD,
    STR_DATE_FORMAT_YDM
};
// clang-format on

openrct_timeofday gRealTimeOfDay;

sint32 date_get_month(sint32 months)
{
    return months % MONTH_COUNT;
}

sint32 date_get_year(sint32 months)
{
    return months / MONTH_COUNT;
}

sint32 date_get_total_months(sint32 month, sint32 year)
{
    return (year - 1) * MONTH_COUNT + month;
}

/**
 *
 *  rct2: 0x006C4494
 */
void date_reset()
{
    gDateMonthsElapsed = 0;
    gDateMonthTicks = 0;
    gCurrentTicks = 0;
}

void date_set(sint32 year, sint32 month, sint32 day)
{
    year = Math::Clamp(1, year, 8192);
    month = Math::Clamp(1, month, (int)MONTH_COUNT);
    day = Math::Clamp(1, day, (int)days_in_month[month - 1]);
    gDateMonthsElapsed = (year - 1) * MONTH_COUNT + month - 1;
    gDateMonthTicks = 0x10000 / days_in_month[month - 1] * (day - 1) + 4;
}

void date_update()
{
    sint32 monthTicks = gDateMonthTicks + 4;
    if (monthTicks >= 0x10000)
    {
        gDateMonthTicks = 0;
        gDateMonthsElapsed++;
    }
    else
    {
        gDateMonthTicks = floor2((uint16)monthTicks, 4);
    }
}

void date_update_real_time_of_day()
{
    time_t timestamp = time(nullptr);
    struct tm *now = localtime(&timestamp);

    gRealTimeOfDay.second = now->tm_sec;
    gRealTimeOfDay.minute = now->tm_min;
    gRealTimeOfDay.hour = now->tm_hour;
}

bool date_is_day_start(sint32 monthTicks)
{
    if (monthTicks < 4)
    {
        return false;
    }
    sint32 prevMonthTick = monthTicks - 4;
    sint32 currentMonth = date_get_month(gDateMonthsElapsed);
    sint32 currentDaysInMonth = days_in_month[currentMonth];
    return ((currentDaysInMonth * monthTicks) >> 16 != (currentDaysInMonth * prevMonthTick) >> 16);
}

bool date_is_week_start(sint32 monthTicks)
{
    return (monthTicks & 0x3FFF) == 0;
}

bool date_is_fortnight_start(sint32 monthTicks)
{
    return (monthTicks & 0x7FFF) == 0;
}

bool date_is_month_start(sint32 monthTicks)
{
    return (monthTicks == 0);
}
