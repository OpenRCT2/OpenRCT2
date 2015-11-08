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
#include "string_ids.h"

const rct_currency_spec g_currency_specs[CURRENCY_END] = {
	{ 10	, "\xC2\xA3"		, CURRENCY_PREFIX, STR_POUNDS },		// British Pound
	{ 10	, "$"				, CURRENCY_PREFIX, STR_DOLLARS},		// US Dollar
	{ 10	, "F"				, CURRENCY_SUFFIX, STR_FRANC },			// French Franc
	{ 10	, "DM"				, CURRENCY_PREFIX, STR_DEUTSCHMARK },	// Deutschmark
	{ 1000	, "\xC2\xA5"		, CURRENCY_PREFIX, STR_YEN },			// Japanese Yen
	{ 10	, "Pts"				, CURRENCY_SUFFIX, STR_PESETA },		// Spanish Peseta
	{ 1000	, "L"				, CURRENCY_PREFIX, STR_LIRA },			// Italian Lira
	{ 10	, "fl. "			, CURRENCY_PREFIX, STR_GUILDERS },		// Dutch Guilder
	{ 10	, "kr."				, CURRENCY_SUFFIX, STR_KRONA },			// Swedish Krona
	{ 10	, "\xE2\x82\xAC"	, CURRENCY_PREFIX, STR_EUROS },			// Euro
	{ 10000 , "W"				, CURRENCY_PREFIX, STR_WON },			// South Korean Won
	{ 1000  , "R "				, CURRENCY_PREFIX, STR_ROUBLE },		// Russian Rouble
};
