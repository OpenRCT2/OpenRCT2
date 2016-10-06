#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../localisation/localisation.h"
#include "../input.h"
#include "../interface/widget.h"
#include "../interface/window.h"

static rct_widget window_map_tooltip_widgets[] = {
	{ WWT_IMGBTN, 0, 0, 199, 0, 29, 0xFFFFFFFF, STR_NONE },
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

	cursorX = gCursorState.x;
	cursorY = gCursorState.y;
	inputFlags = gInputFlags;

	// Check for cursor movement
	_cursorHoldDuration++;
	if (abs(cursorX - _lastCursorX) > 5 || abs(cursorY - _lastCursorY) > 5 || (inputFlags & INPUT_FLAG_5))
		_cursorHoldDuration = 0;

	_lastCursorX = cursorX;
	_lastCursorY = cursorY;

	// Show or hide tooltip
	rct_string_id stringId;
	memcpy(&stringId, gMapTooltipFormatArgs, sizeof(rct_string_id));

	if (_cursorHoldDuration < 25 ||
		stringId == STR_NONE ||
		(gInputPlaceObjectModifier & 3) ||
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
	x = gCursorState.x - (width / 2);
	y = gCursorState.y + 15;

	w = window_find_by_class(WC_MAP_TOOLTIP);
	if (w == NULL) {
		w = window_create(
			x, y, width, height, &window_map_tooltip_events, WC_MAP_TOOLTIP, WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_NO_BACKGROUND
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
	rct_string_id stringId;
	memcpy(&stringId, gMapTooltipFormatArgs, sizeof(rct_string_id));
	if (stringId == STR_NONE) {
		return;
	}

	gfx_draw_string_centred_wrapped(dpi, gMapTooltipFormatArgs, w->x + (w->width / 2), w->y + (w->height / 2), w->width, STR_MAP_TOOLTIP_STRINGID, 0);
}
