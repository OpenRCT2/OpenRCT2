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
#include "../sprites.h"
#include "../interface/widget.h"
#include "../interface/window.h"

enum WINDOW_PUBLISHER_CREDITS_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE
};

rct_widget window_publisher_credits_widgets[] = {
	{ WWT_FRAME,	0,	0,		419,	0,	383,	0x0FFFFFFFF,				STR_NONE },				// panel / background
	{ WWT_CAPTION,	0,	1,		418,	1,	14,		STR_ROLLERCOASTER_TYCOON_2,	STR_WINDOW_TITLE_TIP },	// title bar
	{ WWT_CLOSEBOX,	0,	407,	417,	2,	13,		STR_CLOSE_X,				STR_CLOSE_WINDOW_TIP },	// close x button
	{ WWT_SCROLL,	0,	4,		415,	18,	379,	2,							STR_NONE },				// scroll
	{ WIDGETS_END },
};

static void window_publisher_credits_emptysub() { }
static void window_publisher_credits_mouseup();
static void window_publisher_credits_scrollgetsize();
static void window_publisher_credits_paint();
static void window_publisher_credits_scrollpaint();

static void* window_publisher_credits_events[] = {
	window_publisher_credits_emptysub,
	window_publisher_credits_mouseup,
	window_publisher_credits_emptysub,
	window_publisher_credits_emptysub,
	window_publisher_credits_emptysub,
	window_publisher_credits_emptysub,
	window_publisher_credits_emptysub,
	window_publisher_credits_emptysub,
	window_publisher_credits_emptysub,
	window_publisher_credits_emptysub,
	window_publisher_credits_emptysub,
	window_publisher_credits_emptysub,
	window_publisher_credits_emptysub,
	window_publisher_credits_emptysub,
	window_publisher_credits_emptysub,
	window_publisher_credits_scrollgetsize,
	window_publisher_credits_emptysub,
	window_publisher_credits_emptysub,
	window_publisher_credits_emptysub,
	window_publisher_credits_emptysub,
	window_publisher_credits_emptysub,
	window_publisher_credits_emptysub,
	window_publisher_credits_emptysub,
	window_publisher_credits_emptysub,
	window_publisher_credits_emptysub,
	window_publisher_credits_emptysub,
	window_publisher_credits_paint,
	window_publisher_credits_scrollpaint
};

/**
*
*  rct2: 0x0066D4EC
*/
void window_publisher_credits_open()
{
	rct_window* window;

	// Check if window is already open
	window = window_bring_to_front_by_class(WC_PUBLISHER_CREDITS);
	if (window != NULL)
		return;

	window = window_create_centred(
		420,
		384,
		(uint32*)window_publisher_credits_events,
		WC_PUBLISHER_CREDITS,
		0
	);

	window->widgets = window_publisher_credits_widgets;
	window->enabled_widgets = 1 << WIDX_CLOSE;

	window_init_scroll_widgets(window);
	window->colours[0] = 7;
	window->colours[1] = 7;
	window->colours[2] = 7;

}

/**
*
*  rct2: 0x0066D7A8
*/
static void window_publisher_credits_mouseup()
{
	short widgetIndex;
	rct_window *w;

	window_widget_get_registers(w, widgetIndex);

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	}
}

/**
*
*  rct2: 0x0066D7B3
*/
static void window_publisher_credits_scrollgetsize()
{
	int width, height;

	width = 0;
	height = 820;
	window_scrollsize_set_registers(width, height);
}

/**
*
*  rct2: 0x0066D5CB
*/
static void window_publisher_credits_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);
}

int credits_order[] = {
	0xB5D,
	0xB5E,
	0xB5F,
	0xB60,
	0xB61,
	0xB62,
	0xB63,
	0xB64,
	0xB72,
	0xB71,
	0xB65,
	0xB66,
	0xB6F,
	0xB70,
	0xB67,
	0xB68,
	0xB69,
	0xB6A,
	0xB6B,
	0xB6D,
	0xB6E,
	0xB6C,
};

/**
*
*  rct2: 0x0066D5D1
*/
static void window_publisher_credits_scrollpaint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;
	window_paint_get_registers(w, dpi);

	int x = 200;
	int y = 2;

	gfx_draw_sprite(dpi, SPR_CREDITS_INFOGRAMES, x - 49, y, 0);

	y += 86;

	draw_string_centred_underline(dpi, 0xB5C, 0, 0, x, y);

	y += 14;

	for (int i = 0; i < sizeof(credits_order)/sizeof(int); i++) {
		gfx_draw_string_centred(dpi, credits_order[i], x, y, 0, 0);
		y += 11;
	}


}