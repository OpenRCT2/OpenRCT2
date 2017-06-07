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

#ifndef _DATE_H_
#define _DATE_H_

#include "../common.h"

enum {
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

enum {
    DATE_FORMAT_DAY_MONTH_YEAR,
    DATE_FORMAT_MONTH_DAY_YEAR,
    DATE_FORMAT_YEAR_MONTH_DAY,
    DATE_FORMAT_YEAR_DAY_MONTH
};

typedef struct openrct_timeofday {
    uint8 second;
    uint8 minute;
    uint8 hour;
} openrct_timeofday;

extern const sint16 days_in_month[MONTH_COUNT];
extern const rct_string_id DateFormatStringIds[];
extern const rct_string_id DateFormatStringFormatIds[];

extern uint16 gDateMonthTicks;
extern uint16 gDateMonthsElapsed;

extern openrct_timeofday gRealTimeOfDay;

sint32 date_get_month(sint32 months);
sint32 date_get_year(sint32 months);
sint32 date_get_total_months(sint32 month, sint32 year);
void date_reset();
void date_update_real_time_of_day();

#endif
