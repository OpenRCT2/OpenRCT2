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
#include "park.h"
#include "strings.h"
#include "sprites.h"
#include "widget.h"
#include "window.h"

enum {
	PAGE_RIDES,
	PAGE_SHOPS_AND_STALLS,
	PAGE_KIOSKS_AND_FACILITIES
};

static enum WINDOW_RIDE_LIST_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_PAGE_BACKGROUND,
	WIDX_OPEN_CLOSE_ALL,
	WIDX_CURRENT_INFORMATION_TYPE,
	WIDX_INFORMATION_TYPE_DROPDOWN,
	WIDX_SORT,
	WIDX_TAB_1,
	WIDX_TAB_2,
	WIDX_TAB_3,
	WIDX_LIST
};

static rct_widget window_ride_list_widgets[] = {
	{ WWT_FRAME,			0,	0,		339,	0,		239,	0x0FFFFFFFF,				65535 },									// panel / background
	{ WWT_CAPTION,			0,	1,		338,	1,		14,		0x0FFFFFFFF,				STR_WINDOW_TITLE_TIP },						// title bar
	{ WWT_CLOSEBOX,			0,	327,	337,	2,		13,		STR_CLOSE_X,				STR_CLOSE_WINDOW_TIP },						// close x button
	{ WWT_RESIZE,			1,	0,		339,	43,		239,	0x0FFFFFFFF,				65535 },									// tab page background
	{ WWT_FLATBTN,			1,	315,	338,	60,		83,		SPR_TOGGLE_OPEN_CLOSE,		STR_OPEN_OR_CLOSE_ALL_RIDES },				// open / close all toggle
	{ WWT_DROPDOWN,			1,	164,	273,	46,		57,		0x0FFFFFFFF,				65535 },									// current information type
	{ WWT_DROPDOWN_BUTTON,	1,	262,	272,	47,		56,		876,						STR_RIDE_LIST_INFORMATION_TYPE_TIP },		// information type dropdown button
	{ WWT_DROPDOWN_BUTTON,	1,	280,	333,	46,		57,		STR_SORT,					STR_RIDE_LIST_SORT_TIP },					// sort button
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,					STR_LIST_RIDES_TIP },						// tab 1
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x2000144E,					STR_LIST_SHOPS_AND_STALLS_TIP },			// tab 2
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,					STR_LIST_KIOSKS_AND_FACILITIES_TIP },		// tab 3
	{ WWT_SCROLL,			1,	3,		336,	60,		236,	2,							65535 },									// list
	{ WIDGETS_END },
};

static void window_ride_list_emptysub() { }
static void window_ride_list_mouseup();
static void window_ride_list_update();
static void window_ride_list_tooltip();
static void window_ride_list_invalidate();
static void window_ride_list_paint();

static uint32 window_ride_list_events[] = {
	window_ride_list_emptysub,
	window_ride_list_mouseup,
	window_ride_list_emptysub,
	window_ride_list_mousedown,
	window_ride_list_dropdown,
	window_ride_list_emptysub,
	window_ride_list_update,
	window_ride_list_emptysub,
	window_ride_list_emptysub,
	window_ride_list_emptysub,
	window_ride_list_emptysub,
	window_ride_list_emptysub,
	window_ride_list_emptysub,
	window_ride_list_emptysub,
	window_ride_list_emptysub,
	window_ride_list_emptysub,
	window_ride_list_emptysub,
	window_ride_list_emptysub,
	window_ride_list_emptysub,
	window_ride_list_emptysub,
	window_ride_list_emptysub,
	window_ride_list_emptysub,
	window_ride_list_tooltip,
	window_ride_list_emptysub,
	window_ride_list_emptysub,
	window_ride_list_invalidate,
	window_ride_list_paint,
	window_ride_list_emptysub
};

enum {
	INFORMATION_TYPE_STATUS,
	INFORMATION_TYPE_POPULARITY,
	INFORMATION_TYPE_SATISFACTION,
	INFORMATION_TYPE_PROFIT,
	INFORMATION_TYPE_QUEUE_LENGTH,
	INFORMATION_TYPE_QUEUE_TIME,
	INFORMATION_TYPE_RELIABILITY,
	INFORMATION_TYPE_DOWN_TIME,
	INFORMATION_TYPE_GUESTS_FAVOURITE
};

static void window_ride_list_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w);

static int window_ride_list_information_type;

/**
 * 
 *  rct2: 0x006B30BC
 */
void window_ride_list_open()
{
	rct_window* window;

	// Check if window is already open
	window = window_bring_to_front_by_id(WC_RIDE_LIST, 0);
	if (window == NULL) {
		window = window_create_auto_pos(340, 240, window_ride_list_events, WC_PARK_INFORMATION, 0x0400);
		window->widgets = window_ride_list_widgets;
		window->enabled_widgets =
			(1 << WIDX_CLOSE) |
			(1 << WIDX_OPEN_CLOSE_ALL) |
			(1 << WIDX_CURRENT_INFORMATION_TYPE) |
			(1 << WIDX_INFORMATION_TYPE_DROPDOWN) |
			(1 << WIDX_SORT) |
			(1 << WIDX_TAB_1) |
			(1 << WIDX_TAB_2) |
			(1 << WIDX_TAB_3);
		window_init_scroll_widgets(window);
		window->page = PAGE_RIDES;
		window->var_476 = 0;
		window->var_47A = -1;
		window->var_48E = 0;
		window->min_width = 340;
		window->min_height = 240;
		window->max_width = 400;
		window->max_height = 450;
		window->flags = WF_RESIZABLE;
		window->colours[0] = 1;
		window->colours[1] = 26;
		window->colours[2] = 26;
	}
	window_ride_list_information_type = INFORMATION_TYPE_STATUS;
	window->var_490 = 0;
}

static void window_ride_list_mouseup()
{
	int i;
	short widgetIndex;
	rct_window *w;

	__asm mov widgetIndex, dx
	__asm mov w, esi

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_SORT:
		w->var_490 = window_ride_list_information_type;
		w->var_476 = 0;
		w->var_47A = -1;
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
	case WIDX_TAB_3:
		if (w->page != widgetIndex - WIDX_TAB_1) {
			w->page = widgetIndex - WIDX_TAB_1;
			w->var_476 = 0;
			w->var_47A = -1;
			if (w->page != PAGE_RIDES && window_ride_list_information_type > INFORMATION_TYPE_PROFIT)
				window_ride_list_information_type = INFORMATION_TYPE_PROFIT;
			window_invalidate(w);
		}
		break;
	}
}

static void window_ride_list_mousedown()
{
	short widgetIndex;
	rct_window *w;

	__asm mov widgetIndex, dx
	__asm mov w, esi
}

static void window_ride_list_dropdown()
{
	short widgetIndex;
	rct_window *w;

	__asm mov widgetIndex, dx
	__asm mov w, esi
}

static void window_ride_list_update()
{
	rct_window *w;

	__asm mov w, esi

	w->var_48E = (w->var_48E + 1) % 64;
	widget_invalidate(w->classification, w->number, WIDX_TAB_1 + w->page);
	if (window_ride_list_information_type != INFORMATION_TYPE_STATUS)
		window_invalidate(w);
}

static void window_ride_list_tooltip()
{
	RCT2_GLOBAL(0x013CE952, uint16) = STR_LIST;
}

static void window_ride_list_invalidate()
{
	int i, x, y;
	rct_window *w;

	__asm mov w, esi

	window_ride_list_widgets[WIDX_CURRENT_INFORMATION_TYPE].image = STR_STATUS + window_ride_list_information_type;

	// Set correct active tab
	for (i = 0; i < 3; i++)
		w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
	w->pressed_widgets |= 1 << (WIDX_TAB_1 + w->page);

	window_ride_list_widgets[WIDX_TITLE].image = STR_RIDES + w->page;

	w->widgets[WIDX_BACKGROUND].right = w->width - 1;
	w->widgets[WIDX_BACKGROUND].bottom = w->height - 1;
	w->widgets[WIDX_PAGE_BACKGROUND].right = w->width - 1;
	w->widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;
	w->widgets[WIDX_TITLE].right = w->width - 2;
	w->widgets[WIDX_CLOSE].left = w->width - 13;
	w->widgets[WIDX_CLOSE].right = w->width - 3;
	w->widgets[WIDX_LIST].right = w->width - 26;
	w->widgets[WIDX_LIST].bottom = w->height - 4;
	w->widgets[WIDX_OPEN_CLOSE_ALL].right = w->width - 2;
	w->widgets[WIDX_OPEN_CLOSE_ALL].left = w->width - 25;
}

static void window_ride_list_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;

	__asm mov w, esi
	__asm mov dpi, edi

	window_draw_widgets(w, dpi);
	window_ride_list_draw_tab_images(dpi, w);
}

static void window_ride_list_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w)
{
	int sprite_idx;

	// Rides tab
	sprite_idx = SPR_TAB_RIDE_0;
	if (w->page == PAGE_RIDES)
		sprite_idx += w->var_48E / 4;
	gfx_draw_sprite(dpi, sprite_idx, w->x + w->widgets[WIDX_TAB_1].left, w->y + w->widgets[WIDX_TAB_1].top);

	// Shops and stalls tab
	sprite_idx = SPR_TAB_SHOPS_AND_STALLS_0;
	if (w->page == PAGE_SHOPS_AND_STALLS)
		sprite_idx += w->var_48E / 4;
	gfx_draw_sprite(dpi, sprite_idx, w->x + w->widgets[WIDX_TAB_2].left, w->y + w->widgets[WIDX_TAB_2].top);

	// Information kiosks and facilities tab
	sprite_idx = SPR_TAB_KIOSKS_AND_FACILITIES_0;
	if (w->page == PAGE_KIOSKS_AND_FACILITIES)
		sprite_idx += (w->var_48E / 4) % 8;
	gfx_draw_sprite(dpi, sprite_idx, w->x + w->widgets[WIDX_TAB_3].left, w->y + w->widgets[WIDX_TAB_3].top);
}