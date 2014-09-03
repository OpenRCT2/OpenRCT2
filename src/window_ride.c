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

#include <string.h>
#include "addresses.h"
#include "game.h"
#include "ride.h"
#include "string_ids.h"
#include "sprite.h"
#include "sprites.h"
#include "widget.h"
#include "window.h"
#include "window_dropdown.h"
#include "window_ride.h"

#pragma region Widgets

static rct_widget *window_ride_page_widgets[] = {
	(rct_widget*)0x009ADC34,
	(rct_widget*)0x009ADDA8,
	(rct_widget*)0x009ADEFC,
	(rct_widget*)0x009AE190,
	(rct_widget*)0x009AE2A4,
	(rct_widget*)0x009AE4C8,
	(rct_widget*)0x009AE5DC,
	(rct_widget*)0x009AE710,
	(rct_widget*)0x009AE844,
	(rct_widget*)0x009AE9C8
};

#pragma endregion

#pragma region Events

static uint32* window_ride_page_events[] = {
	(uint32*)0x0098DFD4,
	(uint32*)0x0098E204,
	(uint32*)0x0098E0B4,
	(uint32*)0x0098E124,
	(uint32*)0x0098E044,
	(uint32*)0x0098E194,
	(uint32*)0x0098DE14,
	(uint32*)0x0098DF64,
	(uint32*)0x0098DEF4,
	(uint32*)0x0098DE84
};

#pragma endregion

/**
 * 
 * rct2: 0x006AEAB4
 */
rct_window *window_ride_open(int rideIndex)
{
	rct_window *w;

	w = window_create_auto_pos(316, 180, window_ride_page_events[0], WC_RIDE, 0x400);
	w->widgets = window_ride_page_widgets[0];
	w->enabled_widgets = 0x007DBFF4;
	w->number = rideIndex;

	w->page = 0;
	w->var_48C = 0;
	w->frame_no = 0;
	w->list_information_type = 0;
	w->var_492 = 0;
	w->var_494 = 0;
	RCT2_CALLPROC_X(0x006AEB9F, 0, 0, 0, 0, (int)w, 0, 0);
	w->min_width = 316;
	w->min_height = 180;
	w->max_width = 500;
	w->max_height = 450;
	w->flags |= WF_RESIZABLE;
	w->colours[0] = 1;
	w->colours[1] = 26;
	w->colours[2] = 11;

	rct_ride *ride = &g_ride_list[rideIndex];
	uint8 *edx = (uint8*)0x009E32F8;
	if (ride->type != RIDE_TYPE_NULL) {
		int rideType = ride->type;
		do {
			edx++;
			if (*(edx - 1) != 0xFF)
				continue;
		} while (rideType-- != 0);
	}

	int eax, ebx = 0, ecx;
	while (*edx != 0xFF) {
		eax = *edx++;
		ecx = eax >> 5;
		eax &= 0x1F;
		if (!(RCT2_ADDRESS(0x001357424, uint32)[ecx] & (1 << eax)))
			continue;
		ebx++;
	}
	RCT2_GLOBAL((int)w + 496, uint16) = ebx;
	return w;
}

/**
 * 
 * rct2: 0x006ACC28
 */
void window_ride_main_open(int rideIndex)
{
	rct_window *w;

	w = window_bring_to_front_by_id(WC_RIDE, rideIndex);
	if (w == NULL) {
		w = window_ride_open(rideIndex);
		w->ride.var_482 = -1;
	}

	if (RCT2_GLOBAL(0x009DE518, uint32) & (1 << 3)) {
		if (w->classification == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass) &&
			w->number == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWNUMBER, rct_windownumber)
		) {
			tool_cancel();
		}
	}

	w->page = 0;
	w->width = 316;
	w->height = 180;
	window_invalidate(w);
	w->widgets = window_ride_page_widgets[0];
	w->enabled_widgets = 0x007DBFF4;
	w->var_020 = 0;
	w->event_handlers = window_ride_page_events[0];
	w->pressed_widgets = 0;
	RCT2_CALLPROC_X(0x006AEB9F, 0, 0, 0, 0, (int)w, 0, 0);
	window_init_scroll_widgets(w);
	w->ride.var_480 = 0;
	RCT2_CALLPROC_X(0x006AF994, 0, 0, 0, 0, (int)w, 0, 0);
}
