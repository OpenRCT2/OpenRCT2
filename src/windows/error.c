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
#include "../audio/audio.h"
#include "../localisation/localisation.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "error.h"

enum {
	WIDX_BACKGROUND
};

static rct_widget window_error_widgets[] = {
	{ WWT_IMGBTN, 0, 0, 199, 0, 41, 0xFFFFFFFF, STR_NONE },
	{ WIDGETS_END }
};

static void window_error_unknown5(rct_window *w);
static void window_error_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_error_events = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_error_unknown5,
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
	NULL,
	window_error_paint,
	NULL
};

static char _window_error_text[512];
static uint16 _window_error_num_lines;

/**
 * 
 *  rct2: 0x0066792F
 *
 * bx: title
 * dx: message
 */
void window_error_open(rct_string_id title, rct_string_id message)
{
	char *dst, *args;
	int numLines, fontHeight, x, y, width, height, maxY;
	rct_window *w;

	window_close_by_class(WC_ERROR);
	dst = _window_error_text;
	args = (char*)0x0013CE952;

	// Format the title
	*dst++ = FORMAT_BLACK;
	if (title != (rct_string_id)STR_NONE) {
		format_string(dst, title, args);
		dst += get_string_size(dst) - 1;
	}

	// Format the message
	if (message != (rct_string_id)STR_NONE) {
		*dst++ = FORMAT_NEWLINE;
		format_string(dst, message, args);
		dst += get_string_size(dst) - 1;
	}

	log_verbose("show error, %s", _window_error_text + 1);

	// Check if there is any text to display
	if (dst == _window_error_text + 1)
		return;

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = 224;
	width = gfx_get_string_width_new_lined(_window_error_text);
	width = min(196, width);

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = 224;
	gfx_wrap_string(_window_error_text, width + 1, &numLines, &fontHeight);

	_window_error_num_lines = numLines;
	width = width + 3;
	height = (numLines + 1) * 10 + 4;

	window_error_widgets[WIDX_BACKGROUND].right = width;
	window_error_widgets[WIDX_BACKGROUND].bottom = height;

	x = RCT2_GLOBAL(0x0142406C, sint32) - (width / 2);
	x = clamp(0, x, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16));

	y = RCT2_GLOBAL(0x01424070, sint32) + 26;
	y = max(22, y);
	maxY = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16) - height;
	if (y > maxY) {
		y = y - height - 40;
		y = min(y, maxY);
	}
		
	w = window_create(x, y, width, height, &window_error_events, WC_ERROR, WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_RESIZABLE);
	w->widgets = window_error_widgets;
	w->error.var_480 = 0;
	if (!(RCT2_GLOBAL(0x009A8C29, uint8) & 1))
		sound_play_panned(SOUND_ERROR, 0, w->x + (w->width / 2), 0, 0);
}

/**
 * 
 *  rct2: 0x00667BFE
 */
static void window_error_unknown5(rct_window *w)
{
	w->error.var_480++;
	if (w->error.var_480 >= 8)
		window_close(w);
}

/**
 * 
 *  rct2: 0x00667AA3
 */
static void window_error_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	int t, l, r, b;

	l = w->x;
	t = w->y;
	r = w->x + w->width - 1;
	b = w->y + w->height - 1;

	gfx_fill_rect(dpi, l + 1, t + 1, r - 1, b - 1, 0x200002D);
	gfx_fill_rect(dpi, l, t, r, b, 0x200008B);

	gfx_fill_rect(dpi, l, t + 2, l, b - 2, 0x200002F);
	gfx_fill_rect(dpi, r, t + 2, r, b - 2, 0x200002F);
	gfx_fill_rect(dpi, l + 2, b, r - 2, b, 0x200002F);
	gfx_fill_rect(dpi, l + 2, t, r - 2, t, 0x200002F);

	gfx_fill_rect(dpi, r + 1, t + 1, r + 1, t + 1, 0x200002F);
	gfx_fill_rect(dpi, r - 1, t + 1, r - 1, t + 1, 0x200002F);
	gfx_fill_rect(dpi, l + 1, b - 1, l + 1, b - 1, 0x200002F);
	gfx_fill_rect(dpi, r - 1, b - 1, r - 1, b - 1, 0x200002F);

	l = w->x + (w->width + 1) / 2 - 1;
	t = w->y + 1;
	draw_string_centred_raw(dpi, l, t, _window_error_num_lines, _window_error_text);
}