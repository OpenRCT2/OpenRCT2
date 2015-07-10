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
#include "../drawing/drawing.h"
#include "../localisation/localisation.h"
#include "../interface/widget.h"
#include "../interface/window.h"

enum {
	WIDX_BACKGROUND
};

static rct_widget window_tooltip_widgets[] = {
	{ WWT_IMGBTN, 0, 0, 199, 0, 31, 0x0FFFFFFFF, STR_NONE },
	{ WIDGETS_END },
};

static void window_tooltip_onclose(rct_window *w);
static void window_tooltip_update(rct_window *w);
static void window_tooltip_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_tooltip_events = {
	window_tooltip_onclose,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_tooltip_update,
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
	window_tooltip_paint,
	NULL
};

void window_tooltip_reset(int x, int y)
{
	RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_CURSOR_X, uint16) = x;
	RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_CURSOR_Y, uint16) = y;
	RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_TIMEOUT, uint16) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_WINDOW_CLASS, uint8) = 255;
	RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8) = 1;
	RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) &= ~(1 << 4);
}

uint8* gTooltip_text_buffer = RCT2_ADDRESS(RCT2_ADDRESS_TOOLTIP_TEXT_BUFFER, uint8);
/**
 * 
 *  rct2: 0x006EA10D
 */
void window_tooltip_open(rct_window *widgetWindow, int widgetIndex, int x, int y)
{
	rct_window *w;
	rct_widget *widget;
	int width, height;
	
	if (widgetWindow == NULL || widgetIndex == -1)
		return;

	widget = &widgetWindow->widgets[widgetIndex];
	window_event_invalidate_call(widgetWindow);
	if (widget->tooltip == 0xFFFF)
		return;

	RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_WINDOW_CLASS, rct_windowclass) = widgetWindow->classification;
	RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_WINDOW_NUMBER, rct_windownumber) = widgetWindow->number;
	RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_WIDGET_INDEX, uint16) = widgetIndex;

	if (window_event_tooltip_call(widgetWindow, widgetIndex) == (rct_string_id)STR_NONE)
		return;

	w = window_find_by_class(WC_ERROR);
	if (w != NULL)
		return;

	RCT2_GLOBAL(0x0142006C, sint32) = -1;
	char* buffer = RCT2_ADDRESS(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, char);

	format_string(buffer, widget->tooltip, (void*)0x013CE952);
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = 224;

	int tooltip_text_width = 0, tooltip_text_height = 0;

	tooltip_text_width = gfx_get_string_width_new_lined(buffer);
	buffer = RCT2_ADDRESS(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, char);
	tooltip_text_width &= 0xFFFF;
	if (tooltip_text_width > 196)
		tooltip_text_width = 196;

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = 224;

	int fontHeight;
	tooltip_text_width = gfx_wrap_string(buffer, tooltip_text_width + 1, &tooltip_text_height, &fontHeight);

	RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_TEXT_HEIGHT, sint16) = tooltip_text_height;
	width = tooltip_text_width + 3;
	height = ((tooltip_text_height + 1) * 10) + 4;
	window_tooltip_widgets[WIDX_BACKGROUND].right = width;
	window_tooltip_widgets[WIDX_BACKGROUND].bottom = height;

	memcpy(gTooltip_text_buffer, buffer, 512);
	
	x = clamp(0, x - (width / 2), RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16) - width);

	int max_y = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16) - height;
	y += 26; // Normally, we'd display the tooltip 26 lower
	if (y > max_y)
		// If y is too large, the tooltip could be forced below the cursor if we'd just clamped y,
		// so we'll subtract a bit more
		y -= height + 40;
	y = clamp(22, y, max_y);

	w = window_create(
		x,
		y,
		width,
		height,
		&window_tooltip_events,
		WC_TOOLTIP,
		WF_TRANSPARENT | WF_STICK_TO_FRONT
	);
	w->widgets = window_tooltip_widgets;

	RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_NOT_SHOWN_TICKS, uint16) = 0;
}

/**
 * 
 *  rct2: 0x006E98C6
 */
void window_tooltip_close()
{
	window_close_by_class(WC_TOOLTIP);
	RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_TIMEOUT, uint16) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_WINDOW_CLASS, rct_windowclass) = 255;
	RCT2_GLOBAL(0x0142006C, sint32) = -1;
	RCT2_GLOBAL(0x009DE51E, uint8) = 0;
}

/**
 * 
 *  rct2: 0x006EA578
 */
static void window_tooltip_onclose(rct_window *w)
{
	RCT2_GLOBAL(0x009BC3B0, uint8) = 0;
}

/**
 * 
 *  rct2: 0x006EA580
 */
static void window_tooltip_update(rct_window *w)
{
	if (RCT2_GLOBAL(0x009DE51E, uint8) == 0)
		RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_NOT_SHOWN_TICKS, uint16) = 0;
}

/**
 * 
 *  rct2: 0x006EA41D
 */
static void window_tooltip_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	int left = w->x;
	int top = w->y;
	int right = w->x + w->width - 1;
	int bottom = w->y + w->height - 1;

	// Background
	gfx_fill_rect(dpi, left + 1, top + 1, right - 1, bottom - 1, 0x0200002D);
	gfx_fill_rect(dpi, left + 1, top + 1, right - 1, bottom - 1, 0x02000084);

	// Sides
	gfx_fill_rect(dpi, left  + 0, top    + 2, left  + 0, bottom - 2, 0x0200002F);
	gfx_fill_rect(dpi, right + 0, top    + 2, right + 0, bottom - 2, 0x0200002F);
	gfx_fill_rect(dpi, left  + 2, bottom + 0, right - 2, bottom + 0, 0x0200002F);
	gfx_fill_rect(dpi, left  + 2, top    + 0, right - 2, top    + 0, 0x0200002F);

	// Corners
	gfx_draw_pixel(dpi, left  + 1, top    + 1, 0x0200002F);
	gfx_draw_pixel(dpi, right - 1, top    + 1, 0x0200002F);
	gfx_draw_pixel(dpi, left  + 1, bottom - 1, 0x0200002F);
	gfx_draw_pixel(dpi, right - 1, bottom - 1, 0x0200002F);
	
	// Text
	left = w->x + ((w->width + 1) / 2) - 1;
	top = w->y + 1;
	draw_string_centred_raw(dpi, left, top, RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_TEXT_HEIGHT, uint16), gTooltip_text_buffer);
}