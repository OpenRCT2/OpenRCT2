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
  {10	, "\xA3"	, 1},	//British Pound
  {10	, "\x24"	, 1},	//US Dollar	   
  {10	, "FRF"		, 1},	//French Franc 
  {10	, "DM"		, 1},	//Deutsche Mark
  {1000	, "\xA5"	, 1},	//Japanese Yen 
  {10	, "Pts"		, 0},	//Spanish Peseta
  {1000	, "L"		, 1},	//Italian Lira 
  {10	, "Dfl."	, 1},	//Dutch Guilder 
  {10	, "kr."		, 0},	//Swedish Krona
  {10	, "\xb5"	, 1},	//Euro
};
