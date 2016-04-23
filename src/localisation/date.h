/*****************************************************************************
 * Copyright (c) 2014 Ted John
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * This file is part of OpenRCT2.
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

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

extern const sint16 days_in_month[MONTH_COUNT];
extern const rct_string_id DateFormatStringIds[];
extern const rct_string_id DateFormatStringFormatIds[];

#define gDateMonthTicks			RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_TICKS, uint16)
#define gDateMonthsElapsed		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, uint16)

int date_get_month(int months);
int date_get_year(int months);
int date_get_total_months(int month, int year);
void date_reset();

#endif
