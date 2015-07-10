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

#include "../addresses.h"
#include "../localisation/localisation.h"
#include "../input.h"
#include "../interface/widget.h"
#include "../interface/window.h"

static rct_widget window_map_tooltip_widgets[] = {
	{ WWT_IMGBTN, 0, 0, 199, 0, 29, 0x0FFFFFFFF, STR_NONE },
	{ WIDGETS_END }
};

static void window_map_tooltip_update(rct_window *w);
static void window_map_tooltip_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_map_tooltip_events = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_map_tooltip_update,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_map_tooltip_paint,
	NULL
};

#define MAP_TOOLTIP_ARGS 

static int _lastCursorX;
static int _lastCursorY;
static int _cursorHoldDuration;

static void window_map_tooltip_open();

/**
 *
 *  rct2: 0x006EE77A
 */
void window_map_tooltip_update_visibility()
{
	int cursorX, cursorY, inputFlags;

	cursorX = RCT2_GLOBAL(0x0142406C, sint32);
	cursorY = RCT2_GLOBAL(0x01424070, sint32);
	inputFlags = RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint8);

	// Check for cursor movement
	_cursorHoldDuration++;
	if (abs(cursorX - _lastCursorX) > 5 || abs(cursorY - _lastCursorY) > 5 || (inputFlags & INPUT_FLAG_5))
		_cursorHoldDuration = 0;

	_lastCursorX = cursorX;
	_lastCursorY = cursorY;

	// Show or hide tooltip
	if (
		_cursorHoldDuration < 25 ||
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS, sint16) == -1 ||
		(RCT2_GLOBAL(RCT2_ADDRESS_PLACE_OBJECT_MODIFIER, uint8) & 3) ||
		window_find_by_class(WC_ERROR) != NULL
	) {
		window_close_by_class(WC_MAP_TOOLTIP);
	} else {
		window_map_tooltip_open();
	}
}

/**
 * 
 *  rct2: 0x006A7C43
 */
static void window_map_tooltip_open()
{
	rct_window* w;
	int x, y, width, height;

	width = 200;
	height = 44;
	x = RCT2_GLOBAL(0x0142406C, sint32) - (width / 2);
	y = RCT2_GLOBAL(0x01424070, sint32) + 15;

	w = window_find_by_class(WC_MAP_TOOLTIP);
	if (w == NULL) {
		w = window_create(
			x, y, width, height, &window_map_tooltip_events, WC_MAP_TOOLTIP, WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_5
		);
		w->widgets = window_map_tooltip_widgets;
	} else {
		window_invalidate(w);
		w->x = x;
		w->y = y;
		w->width = width;
		w->height = height;
	}
}

/**
 * 
 *  rct2: 0x006EE8CE
 */
static void window_map_tooltip_update(rct_window *w)
{
	window_invalidate(w);
}

/**
 * 
 *  rct2: 0x006EE894
 */
static void window_map_tooltip_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_TOOLTIP_ARGS, rct_string_id) == (rct_string_id)STR_NONE)
		return;

	gfx_draw_string_centred_wrapped(dpi, (void*)0x009A9808, w->x + (w->width / 2), w->y + (w->height / 2), w->width, 1162, 0);
}