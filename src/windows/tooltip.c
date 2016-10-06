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

#include "../drawing/drawing.h"
#include "../localisation/localisation.h"
#include "../input.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "tooltip.h"

enum {
	WIDX_BACKGROUND
};

static rct_widget window_tooltip_widgets[] = {
	{ WWT_IMGBTN, 0, 0, 199, 0, 31, 0xFFFFFFFF, STR_NONE },
	{ WIDGETS_END },
};

static void window_tooltip_update(rct_window *w);
static void window_tooltip_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_tooltip_events = {
	NULL,
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

static utf8 _tooltipText[512];
static sint16 _tooltipNumLines;

void window_tooltip_reset(int x, int y)
{
	gTooltipCursorX = x;
	gTooltipCursorY = y;
	gTooltipTimeout = 0;
	gTooltipWidget.window_classification = 255;
	gInputState = INPUT_STATE_NORMAL;
	gInputFlags &= ~INPUT_FLAG_4;
}

void window_tooltip_show(rct_string_id id, int x, int y)
{
	rct_window *w;
	int width, height;

	w = window_find_by_class(WC_ERROR);
	if (w != NULL)
		return;

	char* buffer = gCommonStringFormatBuffer;

	format_string(buffer, id, gCommonFormatArgs);
	gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;

	int tooltip_text_width;
	tooltip_text_width = gfx_get_string_width_new_lined(buffer);
	buffer = gCommonStringFormatBuffer;
	tooltip_text_width = min(tooltip_text_width, 196);

	gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;

	int numLines, fontSpriteBase;
	tooltip_text_width = gfx_wrap_string(buffer, tooltip_text_width + 1, &numLines, &fontSpriteBase);

	_tooltipNumLines = numLines;
	width = tooltip_text_width + 3;
	height = ((numLines + 1) * font_get_line_height(gCurrentFontSpriteBase)) + 4;
	window_tooltip_widgets[WIDX_BACKGROUND].right = width;
	window_tooltip_widgets[WIDX_BACKGROUND].bottom = height;

	memcpy(_tooltipText, buffer, sizeof(_tooltipText));

	x = clamp(0, x - (width / 2), gScreenWidth - width);

	// TODO The cursor size will be relative to the window DPI.
	//      The amount to offset the y should be adjusted.

	int max_y = gScreenHeight - height;
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

	gTooltipNotShownTicks = 0;
}

/**
 *
 *  rct2: 0x006EA10D
 */
void window_tooltip_open(rct_window *widgetWindow, int widgetIndex, int x, int y)
{
	rct_widget *widget;

	if (widgetWindow == NULL || widgetIndex == -1)
		return;

	widget = &widgetWindow->widgets[widgetIndex];
	window_event_invalidate_call(widgetWindow);
	if (widget->tooltip == 0xFFFF)
		return;

	gTooltipWidget.window_classification = widgetWindow->classification;
	gTooltipWidget.window_number = widgetWindow->number;
	gTooltipWidget.widget_index = widgetIndex;

	if (window_event_tooltip_call(widgetWindow, widgetIndex) == STR_NONE)
		return;

	window_tooltip_show(widget->tooltip, x, y);
}

/**
 *
 *  rct2: 0x006E98C6
 */
void window_tooltip_close()
{
	window_close_by_class(WC_TOOLTIP);
	gTooltipTimeout = 0;
	gTooltipWidget.window_classification = 255;
}

/**
 *
 *  rct2: 0x006EA580
 */
static void window_tooltip_update(rct_window *w)
{
	gTooltipNotShownTicks = 0;
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
	draw_string_centred_raw(dpi, left, top, _tooltipNumLines, _tooltipText);
}
