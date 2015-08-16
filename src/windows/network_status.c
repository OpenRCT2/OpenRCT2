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

#include "../interface/themes.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../network/network.h"

enum WINDOW_NETWORK_STATUS_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
};

static rct_widget window_network_status_widgets[] = {
	{ WWT_FRAME,			0,	0,		340,	0,		90,		0x0FFFFFFFF,				STR_NONE },									// panel / background
	{ WWT_CAPTION,			0,	1,		338,	1,		14,		STR_NONE,					STR_WINDOW_TITLE_TIP },						// title bar
	{ WWT_CLOSEBOX,			0,	327,	337,	2,		13,		STR_CLOSE_X,				STR_CLOSE_WINDOW_TIP },						// close x button
	{ WIDGETS_END },
};

static char window_network_status_text[1024];

static void window_network_status_mouseup(rct_window *w, int widgetIndex);
static void window_network_status_update(rct_window *w);
static void window_network_status_invalidate(rct_window *w);
static void window_network_status_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_network_status_events = {
	NULL,
	window_network_status_mouseup,
	NULL,
	NULL,
	NULL,
	NULL,
	window_network_status_update,
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
	window_network_status_invalidate,
	window_network_status_paint,
	NULL
};

void window_network_status_open(const char* text)
{
	rct_window* window;

	strncpy(window_network_status_text, text, sizeof(window_network_status_text));

	// Check if window is already open
	window = window_bring_to_front_by_class(WC_NETWORK_STATUS);
	if (window != NULL)
		return;

	window = window_create_centred(320, 90, &window_network_status_events, WC_NETWORK_STATUS, WF_10 | WF_TRANSPARENT);

	window->widgets = window_network_status_widgets;
	window->enabled_widgets = 1 << WIDX_CLOSE;
	window_init_scroll_widgets(window);
	window->no_list_items = 0;
	window->selected_list_item = -1;
	window->frame_no = 0;
	window->min_width = 320;
	window->min_height = 90;
	window->max_width = window->min_width;
	window->max_height = window->min_height;

	window->page = 0;
	window->list_information_type = 0;
	window->colours[0] = 7;
	window->colours[1] = 7;
	window->colours[2] = 7;
}

void window_network_status_close()
{
	window_close_by_class(WC_NETWORK_STATUS);
}

static void window_network_status_mouseup(rct_window *w, int widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	}
}

static void window_network_status_update(rct_window *w)
{
	widget_invalidate(w, WIDX_BACKGROUND);
}

static void window_network_status_invalidate(rct_window *w)
{
	window_network_status_widgets[WIDX_BACKGROUND].right = w->width - 1;
	window_network_status_widgets[WIDX_BACKGROUND].bottom = w->height - 1;
	window_network_status_widgets[WIDX_TITLE].right = w->width - 2;
	window_network_status_widgets[WIDX_CLOSE].left = w->width - 2 - 0x0B;
	window_network_status_widgets[WIDX_CLOSE].right = w->width - 2 - 0x0B + 0x0A;
}

static void window_network_status_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = 224;
	char buffer[sizeof(window_network_status_text) + 10];
	char* lineCh = buffer;
	lineCh = utf8_write_codepoint(lineCh, FORMAT_BLACK);
	strcpy(lineCh, window_network_status_text);
	gfx_clip_string(buffer, 230);
	int x = w->x + (w->width / 2);
	int y = w->y + (w->height / 2);
	x -= gfx_get_string_width(buffer) / 2;
	gfx_draw_string(dpi, buffer, 0, x, y);
}
