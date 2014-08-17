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
	WINDOW_NEW_RIDE_TAB_TRANSPORT,
	WINDOW_NEW_RIDE_TAB_GENTLE,
	WINDOW_NEW_RIDE_TAB_ROLLER_COASTER,
	WINDOW_NEW_RIDE_TAB_THRILL,
	WINDOW_NEW_RIDE_TAB_WATER,
	WINDOW_NEW_RIDE_TAB_SHOP,
	WINDOW_NEW_RIDE_TAB_RESEARCH
} WINDOW_RIDE_CONSTRUCTION_TAB;

enum {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_PAGE_BACKGROUND,
	WIDX_TAB_1,
	WIDX_TAB_2,
	WIDX_TAB_3,
	WIDX_TAB_4,
	WIDX_TAB_5,
	WIDX_TAB_6,
	WIDX_TAB_7,

	WIDX_CURRENTLY_IN_DEVELOPMENT_GROUP,
	WIDX_LAST_DEVELOPMENT_GROUP,
	WIDX_LAST_DEVELOPMENT_BUTTON
};

/**
*
*  rct2: 0x006ACA58
*/
void window_new_ride_init_vars() {
	// If we are in the track designer, default to the Roller Coaster tab
	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_DESIGNER) {
		RCT2_GLOBAL(RCT2_ADDRESS_WINDOW_RIDE_LIST_SELECTED_TAB, uint8) = WINDOW_NEW_RIDE_TAB_ROLLER_COASTER;
	}
	else {
		RCT2_GLOBAL(RCT2_ADDRESS_WINDOW_RIDE_LIST_SELECTED_TAB, uint8) = WINDOW_NEW_RIDE_TAB_TRANSPORT;
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

/**
 *
 *  rct2: 0x006B3CFF
 */
void window_new_ride_open()
{
	rct_window *w;

	w = window_bring_to_front_by_id(WC_CONSTRUCT_RIDE, 0);
	if (w != NULL)
		return;

	// Not sure what these windows are
	window_close_by_id(161, 0);
	window_close_by_id(162, 0);

	w = window_create_auto_pos(601, 370, (uint32*)0x0098E354, WC_CONSTRUCT_RIDE, 0x400);
	w->widgets = (rct_widget*)0x009AEBF4;
	w->enabled_widgets =
		(1 << WIDX_CLOSE) |
		(1 << WIDX_TAB_1) |
		(1 << WIDX_TAB_2) |
		(1 << WIDX_TAB_3) |
		(1 << WIDX_TAB_4) |
		(1 << WIDX_TAB_5) |
		(1 << WIDX_TAB_6) |
		(1 << WIDX_TAB_7) |
		(1 << 14) |
		(1 << 15);
	window_init_scroll_widgets(w);

	w->frame_no = 0;
	w->colours[0] = 24;
	w->colours[1] = 26;
	w->colours[2] = 26;
	w->var_480 = -1;
	w->var_482 = -1;
	RCT2_GLOBAL(0x00F43866, sint16) = -1;
	
	RCT2_CALLPROC_EBPSAFE(0x006B6F3E);
	
	w->var_482 = RCT2_ADDRESS(0x00F43825, sint16)[RCT2_GLOBAL(0x00F43824, uint8)];
	if (w->var_482 == -1)
		w->var_482 = RCT2_GLOBAL(0x00F43523, sint16);
	
	w->width = 1;
	RCT2_CALLPROC_EBPSAFE(0x006B3DF1);	// initialise window size and widgets
	RCT2_CALLPROC_EBPSAFE(0x006B7220);
}