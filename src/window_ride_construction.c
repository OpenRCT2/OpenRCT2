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
	WINDOW_RIDE_CONSTRUCTION_TAB_TRANSPORT,
	WINDOW_RIDE_CONSTRUCTION_TAB_GENTLE,
	WINDOW_RIDE_CONSTRUCTION_TAB_ROLLER_COASTER,
	WINDOW_RIDE_CONSTRUCTION_TAB_THRILL,
	WINDOW_RIDE_CONSTRUCTION_TAB_WATER,
	WINDOW_RIDE_CONSTRUCTION_TAB_SHOP,
	WINDOW_RIDE_CONSTRUCTION_TAB_RESEARCH
} WINDOW_RIDE_CONSTRUCTION_TAB;

/**
*
*  rct2: 0x006ACA58
*/
void window_ride_construction_init_vars() {
	// If we are in the track designer, default to the Roller Coaster tab
	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_DESIGNER) {
		RCT2_GLOBAL(RCT2_ADDRESS_WINDOW_RIDE_LIST_SELECTED_TAB, uint8) = WINDOW_RIDE_CONSTRUCTION_TAB_ROLLER_COASTER;
	}
	else {
		RCT2_GLOBAL(RCT2_ADDRESS_WINDOW_RIDE_LIST_SELECTED_TAB, uint8) = WINDOW_RIDE_CONSTRUCTION_TAB_TRANSPORT;
	}

	for (short i = 0; i < 6; i++) {
		/*
		Reset what is highlighted in each tab.
		Each 16bit number represents the item in its respective tab.
		*/
		RCT2_ADDRESS(RCT2_ADDRESS_WINDOW_RIDE_LIST_HIGHLIGHTED_ITEM, uint16)[i] = 0xFFFF;
	}

	RCT2_GLOBAL(RCT2_ADDRESS_WINDOW_RIDE_LIST_INFORMATION_TYPE, uint8) = 0;
}