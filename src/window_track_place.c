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

#include "addresses.h"
#include "viewport.h"
#include "window.h"

/**
 *
 *  rct2: 0x006CFCA0
 */
void window_track_place_open()
{
	rct_window *w;

	window_close_construction_windows();
	RCT2_GLOBAL(0x00F44168, void*) = rct2_malloc(13104);
	RCT2_CALLPROC_EBPSAFE(0x006D182E);

	w = window_create(0, 29, 200, 124, (uint32*)0x0099405C, WC_TRACK_DESIGN_PLACE, 0);
	w->widgets = (rct_widget*)0x009D7F18;
	w->enabled_widgets = 4 | 8 | 0x10 | 0x20;
	window_init_scroll_widgets(w);
	w->colours[0] = 24;
	w->colours[1] = 24;
	w->colours[2] = 24;
	tool_set(w, 6, 12);
	RCT2_GLOBAL(0x009DE518, uint32) |= 6;
	window_push_others_right(w);
	show_gridlines();
	RCT2_GLOBAL(0x00F440D9, uint32) |= 0x80000000;
	RCT2_GLOBAL(0x00F440DD, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x00F440AE, uint8) = (-RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8)) & 3;
	RCT2_CALLPROC_EBPSAFE(0x006D1845);
}