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

#include "../addresses.h"
#include "../localisation/localisation.h"
#include "../sprites.h"
#include "../interface/widget.h"
#include "../interface/window.h"

enum WINDOW_MUSIC_CREDITS_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE
};

rct_widget window_music_credits_widgets[] = {
	{ WWT_FRAME,	0,	0,		509,	0,	313,	0xFFFFFFFF,					STR_NONE },				// panel / background
	{ WWT_CAPTION,	0,	1,		508,	1,	14,		STR_MUSIC_ACKNOWLEDGEMENTS,	STR_WINDOW_TITLE_TIP },	// title bar
	{ WWT_CLOSEBOX,	0,	497,	507,	2,	13,		STR_CLOSE_X,				STR_CLOSE_WINDOW_TIP },	// close x button
	{ WWT_SCROLL,	0,	4,		505,	18,	309,	SCROLL_VERTICAL,							STR_NONE },				// scroll
	{ WIDGETS_END },
};

static void window_music_credits_mouseup(rct_window *w, int widgetIndex);
static void window_music_credits_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height);
static void window_music_credits_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_music_credits_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex);

static rct_window_event_list window_music_credits_events = {
	NULL,
	window_music_credits_mouseup,
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
	window_music_credits_scrollgetsize,
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
	window_music_credits_paint,
	window_music_credits_scrollpaint
};

/**
*
*  rct2: 0x0066D55B
*/
void window_music_credits_open()
{
	rct_window* window;

	// Check if window is already open
	window = window_bring_to_front_by_class(WC_MUSIC_CREDITS);
	if (window != NULL)
		return;

	window = window_create_centred(
		510,
		314,
		&window_music_credits_events,
		WC_MUSIC_CREDITS,
		0
	);

	window->widgets = window_music_credits_widgets;
	window->enabled_widgets = 1 << WIDX_CLOSE;

	window_init_scroll_widgets(window);
	window->colours[0] = COLOUR_LIGHT_BLUE;
	window->colours[1] = COLOUR_LIGHT_BLUE;
	window->colours[2] = COLOUR_LIGHT_BLUE;

}

/**
*
*  rct2: 0x0066DB2C
*/
static void window_music_credits_mouseup(rct_window *w, int widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	}
}

/**
 *
 *  rct2: 0x0066DB37
 */
static void window_music_credits_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height)
{
	*height = 560;
}

/**
*
*  rct2: 0x0066D7B9
*/
static void window_music_credits_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);
}

/**
 *
 *  rct2: 0x0066D7BF
 */
static void window_music_credits_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex)
{
	// TODO: Clean up string addition
	int x = 245;

	int y = 2;
	int string = STR_MUSIC_ACKNOWLEDGEMENTS_LINE_0;
	for (int i = 0; i < 12; i++) {
		gfx_draw_string_centred(dpi, string, x, y, 0, 0);
		y += 10;
		if (i == 10) { // Add 4 more space before "Original recordings ...".
			y += 4;
		}
		string += 1;
	}

	// Draw the separator
	y += 5;
	gfx_fill_rect_inset(dpi, 4, y, 484, y+1, w->colours[1], 0x20);
	y += 11;

	for (int i = 0; i < 31; i++) {
		if (i == 21) { // Move special courtesy to below Hypothermia.
			gfx_draw_string_centred(dpi, string + 4, x, y, 0, 0);
			y += 10;
			continue;
		} else if (i == 25) { // Remove special courtesy and blank line.
			string += 2;
		}
		gfx_draw_string_centred(dpi, string, x, y, 0, 0);
		y += 10;
		string += 1;
	}

}
