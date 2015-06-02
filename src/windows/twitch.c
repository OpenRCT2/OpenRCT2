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
#include "../config.h"

enum window_twitch_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_TWITCH_AUTH, 
	WINDOW_TWITCH_WIDGETS_SIZE
};

rct_widget window_twitch_widgets[] = {
	{ WWT_FRAME, 0, 0, 399, 0, 329, 0x0FFFFFFFF, STR_NONE },				// panel / background
	{ WWT_CAPTION, 0, 1, 398, 1, 14, STR_TWITCH_WINDOW_TITLE, STR_WINDOW_TITLE_TIP },	// title bar
	{ WWT_CLOSEBOX, 0, 387, 397, 2, 13, STR_CLOSE_X, STR_CLOSE_WINDOW_TIP },	// close x button
	{ WWT_DROPDOWN_BUTTON, 1, 100, 299, 53, 64, STR_TWITCH_AUTH, STR_TWITCH_AUTH_TIP },				// twitch auth
	{ WIDGETS_END },
};

static void window_twitch_emptysub() { }
static void window_twitch_mouseup();
static void window_twitch_paint();
static void window_twitch_invalidate();
static void window_twitch_text_input();

static void* window_twitch_events[] = {
	window_twitch_emptysub,
	window_twitch_mouseup,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_text_input,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_invalidate,
	window_twitch_paint,
	window_twitch_emptysub
};

/**
*
*  rct2: 0x0066D2AC
*/
void window_twitch_open()
{
	rct_window* window;

	// Check if window is already open
	window = window_bring_to_front_by_class(WC_TWITCH);
	if (window != NULL)
		return;

	window = window_create_centred(
		400,
		330,
		(uint32*)window_twitch_events,
		WC_TWITCH,
		0
		);
	window->widgets = window_twitch_widgets;
	window->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_TWITCH_AUTH);

	window_init_scroll_widgets(window);
	window->colours[0] = 7;
	window->colours[1] = 7;
	window->colours[2] = 7;
}

/**
*
*  rct2: 0x0066D4D5
*/
static void window_twitch_mouseup()
{
	short widgetIndex;
	rct_window *w;

	window_widget_get_registers(w, widgetIndex);

	utf8string testString = "test";

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TWITCH_AUTH:
		window_multi_text_input_raw_open(w, widgetIndex, STR_TWITCH_AUTH, STR_TWITCH_NAME_DESC, gConfigTwitch.channel, 32, false, testString, 32, true);
		break;
	}
}

/**
*
*  rct2: 0x0066D321
*/
static void window_twitch_paint()
{
	int x, y;
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);

	x = w->x + 180;
	y = w->y + 17;

	gfx_draw_sprite(dpi, SPR_G2_TAB_TWITCH, x, y, 0);

	x = w->x + 300;
	y = w->y + 230;

	gfx_draw_sprite(dpi, SPR_G2_PROMO_UL, x, y, 0);

}

static void window_twitch_text_input(){
	short widgetIndex;
	rct_window *w;
	uint8 result;
	char* text;

	window_textinput_get_registers(w, widgetIndex, result, text);

	char *userName = multi_text_input;
	char *password = multi_text_input2;
}

static void window_twitch_invalidate()
{
	rct_window *w;

	window_get_register(w);

	/*for (i = WIDX_TWITCH_AUTH; i < WINDOW_TWITCH_WIDGETS_SIZE; i++) {
		window_twitch_widgets[i].type = WWT_EMPTY;
	}*/
}
