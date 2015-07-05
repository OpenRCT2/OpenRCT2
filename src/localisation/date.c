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

#include "../addresses.h"
#include "date.h"

// rct2: 0x00993988
const sint16 days_in_month[MONTH_COUNT] = { 31, 30, 31, 30, 31, 31, 30, 31 };


int date_get_month(int months)
{
	return months % MONTH_COUNT;
}

int date_get_year(int months)
{
	return months / MONTH_COUNT;
}

int date_get_total_months(int month, int year)
{
	return (year - 1) * MONTH_COUNT + month;
}

/**
 *
 *  rct2: 0x006C4494
 */
void date_reset()
{
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, sint16) = MONTH_MARCH;
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_TICKS, sint16) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) = 0;
}
