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

#ifndef CURRENCY_H
#define CURRENCY_H

#include "../common.h"

// List of currencies
typedef enum {
	CURRENCY_POUNDS,			// British Pound
	CURRENCY_DOLLARS,			// US Dollar
	CURRENCY_FRANC,				// French Franc
	CURRENCY_DEUTSCHMARK,		// Deutsche Mark
	CURRENCY_YEN,				// Japanese Yen
	CURRENCY_PESETA,			// Spanish Peseta
	CURRENCY_LIRA,				// Italian Lira
	CURRENCY_GUILDERS,			// Dutch Gilder
	CURRENCY_KRONA,				// Swedish Krona
	CURRENCY_EUROS,				// Euro
	
	CURRENCY_END				// Last item
} CURRENCY_TYPE;

typedef enum {
	CURRENCY_PREFIX,
	CURRENCY_SUFFIX
} CURRENCY_AFFIX;

// Currency format specification - inspired by OpenTTD
typedef struct {
	// Rate is relative to 0.1 GBP
	int rate;
	utf8 symbol[8];
	int affix;
} rct_currency_spec;

// List of currency formats
extern const rct_currency_spec g_currency_specs[CURRENCY_END];

#endif
