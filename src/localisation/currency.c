/*****************************************************************************
 * Copyright (c) 2014 Ted John, Peter Hill
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

#include "currency.h"

const rct_currency_spec g_currency_specs[CURRENCY_END] = {
	{ 10	, "\xC2\xA3"		, CURRENCY_PREFIX, 2334 },	// British Pound
	{ 10	, "$"				, CURRENCY_PREFIX, 2335},	// US Dollar
	{ 10	, "F"				, CURRENCY_SUFFIX, 2336 },	// French Franc
	{ 10	, "DM"				, CURRENCY_PREFIX, 2337 },	// Deutsche Mark
	{ 1000	, "\xC2\xA5"		, CURRENCY_PREFIX, 2338 },	// Japanese Yen
	{ 10	, "Pts"				, CURRENCY_SUFFIX, 2339 },	// Spanish Peseta
	{ 1000	, "L"				, CURRENCY_PREFIX, 2340 },	// Italian Lira
	{ 10	, "fl. "			, CURRENCY_PREFIX, 2341 },	// Dutch Guilder
	{ 10	, "kr."				, CURRENCY_SUFFIX, 2342 },	// Swedish Krona
	{ 10	, "\xE2\x82\xAC"	, CURRENCY_PREFIX, 2343 },	// Euro
	{ 10000 , "W"				, CURRENCY_PREFIX, 5577 },	// South Korean Won
	{ 1000  , "R "				, CURRENCY_PREFIX, 5578 },	// Russian Rouble
};
