/*****************************************************************************
 * Copyright (c) 2014 Chad Ian Anderson
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
#include "../audio/audio.h"
#include "../localisation/localisation.h"
#include "../world/sprite.h"
#include "../sprites.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../interface/themes.h"

enum WINDOW_CHAT_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_SCROLL
};

static rct_widget window_chat_widgets[] = {
	{ WWT_FRAME,			0,	0,			399,	0,		299,	0x0FFFFFFFF,			STR_NONE },				// panel / background
	{ WWT_CAPTION,			0,	1,			398,	1,		14,		STR_CHAT,	STR_WINDOW_TITLE_TIP },	// title bar
	{ WWT_CLOSEBOX,			0,	387,		397,	2,		13,		STR_CLOSE_X,			STR_CLOSE_WINDOW_TIP },	// close x button
	{ WWT_SCROLL,			0,	4,			395,	44,		295,	2,						STR_NONE },				// scroll
	{ WIDGETS_END },
};

static void window_chat_mouseup(rct_window *w, int widgetIndex);
static void window_chat_update(rct_window *w);
static void window_chat_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height);
static void window_chat_scrollmousedown(rct_window *w, int scrollIndex, int x, int y);
static void window_chat_tooltip(rct_window* w, int widgetIndex, rct_string_id *stringId);
static void window_chat_invalidate(rct_window *w);
static void window_chat_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_chat_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex);

static rct_window_event_list window_chat_events = {
	NULL,
	window_chat_mouseup,
	NULL,
	NULL,
	NULL,
	NULL,
	window_chat_update,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_chat_scrollgetsize,
	window_chat_scrollmousedown,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_chat_tooltip,
	NULL,
	NULL,
	window_chat_invalidate,
	window_chat_paint,
	window_chat_scrollpaint
};

/**
 *
 *  rct2: 0x0066E464
 */
void window_chat_open()
{
	rct_window* window;

	// Check if window is already open
	window = window_bring_to_front_by_class(WC_CHAT);
	if (window == NULL) {
		window = window_create_auto_pos(
			400,
			300,
			&window_chat_events,
			WC_CHAT,
			0
		);
		window->widgets = window_chat_widgets;
		window->enabled_widgets = (1 << WIDX_CLOSE);
		window_init_scroll_widgets(window);
	}

// sub_66E4BA:
	rct_widget *widget;

	int width = 0;
	int height = 0;
	window_get_scroll_size(window, 0, &width, &height);
	widget = &window_chat_widgets[WIDX_SCROLL];
	window->scrolls[0].v_top = max(0, height - (widget->bottom - widget->top - 1));
	widget_scroll_update_thumbs(window, WIDX_SCROLL);

}

/**
 *
 *  rct2: 0x0066D4D5
 */
static void window_chat_mouseup(rct_window *w, int widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	}
}

/**
 *
 *  rct2: 0x0066EAB8
 */
static void window_chat_update(rct_window *w)
{
	int i, j, x, y, z;

	window_invalidate(w);
	audio_play_sound_panned(SOUND_CLICK_2, w->x + (w->width / 2), 0, 0, 0);

}

/**
 *
 *  rct2: 0x0066EA3C
 */
static void window_chat_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height)
{
	int i;

	*height = 0;
}

/**
 *
 *  rct2: 0x0066EA5C
 */
static void window_chat_scrollmousedown(rct_window *w, int scrollIndex, int x, int y)
{
	int i, buttonIndex;

	buttonIndex = 0;

	if (buttonIndex != 0) {
		window_invalidate(w);
		audio_play_sound_panned(SOUND_CLICK_1, w->x + (w->width / 2), 0, 0, 0);
	}
}

/**
 *
 *  rct2: 0x0066EAAE
 */
static void window_chat_tooltip(rct_window* w, int widgetIndex, rct_string_id *stringId)
{
	RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, uint16) = 3159;
}

/**
 *
 *  rct2: 0x0066E4E8
 */
static void window_chat_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);
}

static void window_chat_invalidate(rct_window *w)
{
	colour_scheme_update(w);
}

/**
 *
 *  rct2: 0x0066E4EE
 */
static void window_chat_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex)
{
	int i, x, y, yy, press;

	y = 0;
}
