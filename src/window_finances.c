/*****************************************************************************
* Copyright (c) 2014 Maciek Baron
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

#include "addresses.h"
#include "window.h"

enum {
	WINDOW_FINANCES_TAB_SUMMARY,
	WINDOW_FINANCES_TAB_FINANCIAL_GRAPH,
	WINDOW_FINANCES_TAB_VALUE_GRAPH,
	WINDOW_FINANCES_TAB_PROFIT_GRAPH,
	WINDOW_FINANCES_TAB_MARKETING,
	WINDOW_FINANCES_TAB_RESEARCH
} WINDOW_FINANCIAL_TAB;

/**
 *
 *  rct2: 0x0069DDF1
 */
void window_finances_open()
{
	RCT2_CALLPROC_EBPSAFE(0x0069DDF1);
}