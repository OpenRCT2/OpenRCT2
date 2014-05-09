/*****************************************************************************
 * Copyright (c) 2014 Ted John, Dennis Devriendt
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
#include "config.h"
#include "strings.h"
#include "viewport.h"
#include "widget.h"
#include "window.h"

static enum WINDOW_CLEAR_SCENERY_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_VIEWPORT,
	WIDX_BANNER_TEXT,
	WIDX_BANNER_NO_ENTRY,
	WIDX_BANNER_DEMOLISH,
	WIDX_MAIN_COLOR,
	WIDX_TEXT_COLOR_DROPDOWN,
	WIDX_TEXT_COLOR_DROPDOWN_BUTTON
};

static rct_widget window_banner_widgets[] = {
	{ WWT_FRAME,			0,	0,		112,	0,		95,		0x0FFFFFFFF,	65535},								// panel / background
	{ WWT_CAPTION,			0,	1,		111,	1,		14,		0xBA9,			STR_WINDOW_TITLE_TIP},				// title bar
	{ WWT_CLOSEBOX,			0,	100,	110,	2,		13,		0x338,			STR_CLOSE_WINDOW_TIP},				// close x button
	{ WWT_VIEWPORT,			1,	3,		87,		17,		76,		0x0FFFFFFFE,	65535},								// tab content panel
	{ WWT_FLATBTN,			1,	88,		111,	19,		42,		0x1430,			STR_CHANGE_BANNER_TEXT_TIP},		// change banner button
	{ WWT_FLATBTN,			1,	88,		111,	43,		66,		0x143A,			STR_SET_AS_NO_ENTRY_BANNER_TIP},	// no entry button
	{ WWT_FLATBTN,			1,	88,		111,	67,		90,		0x142D,			STR_DEMOLISH_BANNER_TIP},			// demolish button
	{ WWT_COLORBTN,			1,	5,		16,		80,		91,		0x0FFFFFFFF,	STR_SELECT_MAIN_COLOR_TIP},			// high money
	{ WWT_DROPDOWN,			1,	43,		81,		80,		91,		0x0FFFFFFFF,	65535},								// high money
	{ WWT_DROPDOWN_BUTTON,	1,	70,		80,		81,		90,		0x36C,			STR_SELECT_TEXT_COLOR_TIP},			// high money
	{ WIDGETS_END },
};

static void window_banner_emptysub() { }
static void window_banner_mouseup();
static void window_banner_mousedown();
static void window_banner_dropdown();
static void window_banner_textinput();
static void window_banner_invalidate();
static void window_banner_paint();

static uint32 window_banner_events[] = {
	window_banner_emptysub,
	window_banner_mouseup,
	window_banner_emptysub,
	window_banner_mousedown,
	window_banner_dropdown,
	window_banner_emptysub,
	window_banner_emptysub,
	window_banner_emptysub,
	window_banner_emptysub,
	window_banner_emptysub,
	window_banner_emptysub,
	window_banner_emptysub,
	window_banner_emptysub,
	window_banner_emptysub,
	window_banner_emptysub,
	window_banner_emptysub,
	window_banner_emptysub,
	window_banner_emptysub,
	window_banner_emptysub,
	window_banner_textinput,
	0x006BA7B5,
	window_banner_emptysub,
	window_banner_emptysub,
	window_banner_emptysub,
	window_banner_emptysub,
	window_banner_invalidate,
	window_banner_paint,
	window_banner_emptysub
};

/**
*
*  rct2: 0x006BA305
*/
void window_banner_open()
{
	rct_windownumber windownumber;
	rct_window* w;
	rct_viewport *viewport;
	rct_widget *viewportWidget;

	//__asm mov windownumber, ax // not quite right I think
	windownumber = 0;

	// Check if window is already open
	w = window_bring_to_front_by_id(WC_BANNER, windownumber);
	if (w != NULL)
		return;

	w = window_create_auto_pos(113, 96, window_banner_events, WC_BANNER, 0);
	w->widgets = window_banner_widgets;
	w->enabled_widgets =
		(1 << WIDX_CLOSE) |
		(1 << WIDX_BANNER_TEXT) |
		(1 << WIDX_BANNER_NO_ENTRY) |
		(1 << WIDX_BANNER_DEMOLISH) |
		(1 << WIDX_MAIN_COLOR) |
		(1 << WIDX_TEXT_COLOR_DROPDOWN) |
		(1 << WIDX_TEXT_COLOR_DROPDOWN_BUTTON);

	w->number = windownumber;
	window_init_scroll_widgets(w);
	w->colours[0] = 24;
	w->colours[1] = 24;
	w->colours[2] = 24;

	/*
		TODO: MISSING CODE 006BA377 -> 006BA3F6, need the banner map element 
	*/

	// Create viewport
	viewportWidget = &window_banner_widgets[WIDX_VIEWPORT];
	viewport_create(
		w,
		w->x + viewportWidget->left + 1,
		w->y + viewportWidget->top + 1,
		(viewportWidget->right - viewportWidget->left) - 2,
		(viewportWidget->bottom - viewportWidget->top) - 2,
		100,	// TODO: needs banner map position
		100		// TODO: needs banner map position
	);

	w->viewport->flags = (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) & CONFIG_FLAG_ALWAYS_SHOW_GRIDLINES) ? VIEWPORT_FLAG_GRIDLINES : 0;
	w->flags |= WF_2 | WF_TRANSPARENT;
	window_invalidate(w);
}

static void window_banner_mouseup()
{
	short widgetIndex;
	rct_window *w;

	__asm mov widgetIndex, dx
	__asm mov w, esi

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_BANNER_DEMOLISH:
		RCT2_CALLPROC_EBPSAFE(0x006BA739);
		break;
	case WIDX_BANNER_TEXT:
		RCT2_CALLPROC_EBPSAFE(0x006BA6BC);
		break;
	case WIDX_BANNER_NO_ENTRY:
		RCT2_CALLPROC_EBPSAFE(0x006BA64D);
		break;
	}
}

static void window_banner_mousedown()
{
	short widgetIndex;

	__asm mov widgetIndex, dx

	switch (widgetIndex) {
	case WIDX_MAIN_COLOR:
		RCT2_CALLPROC_EBPSAFE(0x006BA528);
		break;
	case WIDX_TEXT_COLOR_DROPDOWN_BUTTON:
		RCT2_CALLPROC_EBPSAFE(0x006BA563);
		break;
	}
}

static void window_banner_dropdown()
{
	short widgetIndex;

	__asm mov widgetIndex, dx;

	if (widgetIndex == WIDX_MAIN_COLOR)
		RCT2_CALLPROC_EBPSAFE(0x006BA548);
	else if (widgetIndex == WIDX_TEXT_COLOR_DROPDOWN_BUTTON)
		RCT2_CALLPROC_EBPSAFE(0x006BA5D0);
}

static void window_banner_textinput()
{
	short widgetIndex;

	__asm mov widgetIndex, dx;

	if (widgetIndex == WIDX_BANNER_TEXT) {
		RCT2_CALLPROC_EBPSAFE(0x006BA6D8);
	}
}

static void window_banner_invalidate()
{
	RCT2_CALLPROC_EBPSAFE(0x006BA44D);
}

static void window_banner_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;

	__asm mov w, esi
	__asm mov dpi, edi

	window_draw_widgets(w, dpi);

	// Draw viewport
	if (w->viewport != NULL) {
		window_draw_viewport(dpi, w);
	}
}
