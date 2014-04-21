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
#include "map.h"
#include "strings.h"
#include "sprites.h"
#include "widget.h"
#include "window.h"

static enum WINDOW_CLEAR_SCENERY_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_PREVIEW,
	WIDX_DECREMENT,
	WIDX_INCREMENT
};

static rct_widget window_clear_scenery_widgets[] = {
	{ WWT_FRAME,	0,	0,	97,	0,	66,	-1,										STR_NONE },							// panel / background
	{ WWT_CAPTION,	0,	1,	96,	1,	14,	STR_CLEAR_SCENERY,						STR_WINDOW_TITLE_TIP },				// title bar
	{ WWT_CLOSEBOX,	0,	85,	95,	2,	13,	STR_CLOSE_X,							STR_CLOSE_WINDOW_TIP },				// close x button
	{ WWT_IMGBTN,	0,	27,	70,	17,	48,	SPR_LAND_TOOL_SIZE_0,					STR_NONE },							// preview box
	{ WWT_TRNBTN,	1,	28,	43,	18,	33,	0x20000000 | SPR_LAND_TOOL_DECREASE,	STR_ADJUST_SMALLER_LAND_TIP },		// decrement size
	{ WWT_TRNBTN,	1,	54,	69,	32,	47,	0x20000000 | SPR_LAND_TOOL_INCREASE,	STR_ADJUST_LARGER_LAND_TIP },		// increment size
	{ WIDGETS_END },
};

static int window_clear_scenery_should_close();

static void window_clear_scenery_emptysub() { }
static void window_clear_scenery_close();
static void window_clear_scenery_mouseup();
static void window_clear_scenery_update();
static void window_clear_scenery_invalidate();
static void window_clear_scenery_paint();

static uint32 window_clear_scenery_events[] = {
	window_clear_scenery_close,
	window_clear_scenery_mouseup,
	window_clear_scenery_emptysub,
	window_clear_scenery_emptysub,
	window_clear_scenery_emptysub,
	window_clear_scenery_emptysub,
	window_clear_scenery_update,
	window_clear_scenery_emptysub,
	window_clear_scenery_emptysub,
	window_clear_scenery_emptysub,
	window_clear_scenery_emptysub,
	window_clear_scenery_emptysub,
	window_clear_scenery_emptysub,
	window_clear_scenery_emptysub,
	window_clear_scenery_emptysub,
	window_clear_scenery_emptysub,
	window_clear_scenery_emptysub,
	window_clear_scenery_emptysub,
	window_clear_scenery_emptysub,
	window_clear_scenery_emptysub,
	window_clear_scenery_emptysub,
	window_clear_scenery_emptysub,
	window_clear_scenery_emptysub,
	window_clear_scenery_emptysub,
	window_clear_scenery_emptysub,
	window_clear_scenery_invalidate,
	window_clear_scenery_paint,
	window_clear_scenery_emptysub
};

/**
 * 
 *  rct2: 0x0068E0A7
 */
void window_clear_scenery_open()
{
	rct_window* window;

	// Check if window is already open
	if (window_find_by_id(WC_CLEAR_SCENERY, 0) != NULL)
		return;

	window = window_create(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16) - 98, 29, 98, 67, window_clear_scenery_events, WC_CLEAR_SCENERY, 0);
	window->widgets = window_clear_scenery_widgets;
	window->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_INCREMENT) | (1 << WIDX_DECREMENT);
	window_init_scroll_widgets(window);
	window_push_others_below(window);

	RCT2_GLOBAL(0x00F1AD62, uint32) = 0x80000000;
	window->colours[0] = 24;
	window->colours[1] = 24;
	window->colours[2] = 24;
}

/**
 *
 *  rct2: 0x006E6B65
 */
static void window_clear_scenery_close()
{
	// If the tool wasn't changed, turn tool off
	if (!window_clear_scenery_should_close())
		RCT2_CALLPROC_EBPSAFE(0x006EE281);
}

/**
 *
 *  rct2: 0x0068E185
 */
static void window_clear_scenery_mouseup()
{
	rct_window *w;
	int limit;
	short widgetIndex;

	__asm mov widgetIndex, dx
	__asm mov w, esi

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_DECREMENT:
		// Decrement land tool size
		RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16)--;
		limit = 1;

		if (RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) < limit)
			RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) = limit;

		// Invalidate the window
		window_invalidate(w);
		break;
	case WIDX_INCREMENT:
		// Increment land tool size
		RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16)++;

		// FEATURE: maximum size is always 7
		limit = 7;
		// limit = (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 2 ? 7 : 5);
		
		if (RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) > limit)
			RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) = limit;

		// Invalidate the window
		window_invalidate(w);
		break;
	}
}

/**
 *
 *  rct2: 0x0068E205
 */
static void window_clear_scenery_update()
{
	rct_window *w;

	__asm mov w, esi

	// Close window if another tool is open
	if (window_clear_scenery_should_close())
		window_close(w);
}

/**
 *
 *  rct2: 0x0068E115
 */
static void window_clear_scenery_invalidate()
{
	rct_window *w;

	__asm mov w, esi

	// Set the preview image button to be pressed down
	w->pressed_widgets |= (1 << WIDX_PREVIEW);

	// Update the preview image
	window_clear_scenery_widgets[WIDX_PREVIEW].image = SPR_LAND_TOOL_SIZE_0 + RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16);
}

/**
 *
 *  rct2: 0x0068E130
 */
static void window_clear_scenery_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;
	int x, y;

	__asm mov w, esi
	__asm mov dpi, edi

	window_draw_widgets(w, dpi);

	// Draw cost amount
	x = (window_clear_scenery_widgets[WIDX_PREVIEW].left + window_clear_scenery_widgets[WIDX_PREVIEW].right) / 2 + w->x;
	y = window_clear_scenery_widgets[WIDX_PREVIEW].bottom + w->y + 5;
	if (RCT2_GLOBAL(0x00F1AD62, uint32) != 0x80000000 && RCT2_GLOBAL(0x00F1AD62, uint32) != 0)
		gfx_draw_string_centred(dpi, 986, x, y, 0, (void*)0x00F1AD62);
}

/**
 *
 *  rct2: 0x0066D125
 */
static int window_clear_scenery_should_close()
{
	if (!(RCT2_GLOBAL(0x009DE518, uint32) & (1 << 3)))
		return 1;
	if (RCT2_GLOBAL(0x009DE544, rct_windowclass) != WC_TOP_TOOLBAR)
		return 1;
	if (RCT2_GLOBAL(0x009DE546, uint16) != 16)
		return 1;
	return 0;
}
