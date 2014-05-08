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

#include <string.h>
#include "addresses.h"
#include "game.h"
#include "window.h"

enum {
	WINDOW_STAFF_LIST_TAB_HANDYMEN,
	WINDOW_STAFF_LIST_TAB_MECHANICS,
	WINDOW_STAFF_LIST_TAB_SECURITY,
	WINDOW_STAFF_LIST_TAB_ENTERTAINERS
} WINDOW_STAFF_LIST_TAB;


/*
* rct2: 0x006BD39C
**/
void window_staff_init_vars() {
	RCT2_GLOBAL(RCT2_ADDRESS_WINDOW_STAFF_LIST_SELECTED_TAB, uint8) = WINDOW_STAFF_LIST_TAB_HANDYMEN;
}