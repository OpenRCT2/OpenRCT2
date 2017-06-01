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
#include "../game.h"
#include "date.h"
#include "string_ids.h"

uint16 gDateMonthTicks;
uint16 gDateMonthsElapsed;

// rct2: 0x00993988
const sint16 days_in_month[MONTH_COUNT] = { 31, 30, 31, 30, 31, 31, 30, 31 };

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

void date_update_real_time_of_day()
{
	time_t timestamp = time(0);
	struct tm *now = localtime(&timestamp);

	gRealTimeOfDay.second = now->tm_sec;
	gRealTimeOfDay.minute = now->tm_min;
	gRealTimeOfDay.hour = now->tm_hour;
}
