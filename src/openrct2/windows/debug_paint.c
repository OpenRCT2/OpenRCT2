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

#include "../input.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../paint/map_element/map_element.h"
#include "../paint/paint.h"
#include "../rct2.h"
#include "../ride/track_paint.h"
#include "../sprites.h"
#include "../world/map.h"

enum WINDOW_DEBUG_PAINT_WIDGET_IDX
{
	WIDX_BACKGROUND,
	WIDX_TOGGLE_OLD_DRAWING,
	WIDX_TOGGLE_SHOW_SEGMENT_HEIGHTS,
	WIDX_TOGGLE_SHOW_BOUND_BOXES,
};

#define WINDOW_WIDTH	(200)
#define WINDOW_HEIGHT	(8 + 15 + 15 + 11 + 8)

static rct_widget window_debug_paint_widgets[] = {
	{ WWT_FRAME,	0,	0,	WINDOW_WIDTH - 1,	0,				WINDOW_HEIGHT - 1,	0xFFFFFFFF,								STR_NONE},
	{ WWT_CHECKBOX,	1,	8,	WINDOW_WIDTH - 8,	8,				8 + 11,				STR_DEBUG_PAINT_USE_OLD_DRAWING,		STR_NONE},
	{ WWT_CHECKBOX,	1,	8,	WINDOW_WIDTH - 8,	8 + 15,			8 + 15 + 11,		STR_DEBUG_PAINT_SHOW_SEGMENT_HEIGHTS,	STR_NONE},
	{ WWT_CHECKBOX,	1,	8,	WINDOW_WIDTH - 8,	8 + 15 + 15,	8 + 15 + 15 + 11,	STR_DEBUG_PAINT_SHOW_BOUND_BOXES,		STR_NONE},
	{ WIDGETS_END },
};

static void window_debug_paint_mouseup(rct_window * w, rct_widgetindex widgetIndex);
static void window_debug_paint_invalidate(rct_window * w);
static void window_debug_paint_paint(rct_window * w, rct_drawpixelinfo * dpi);

static rct_window_event_list window_debug_paint_events = {
	NULL,
	window_debug_paint_mouseup,
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
	NULL,
	NULL,
	NULL,
	window_debug_paint_invalidate,
	window_debug_paint_paint,
	NULL
};

void window_debug_paint_open()
{
	rct_window * window;

	// Check if window is already open
	if (window_find_by_class(WC_DEBUG_PAINT) != NULL)
		return;

	window = window_create(
		16,
		gScreenHeight - 16 - 33 - WINDOW_HEIGHT,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		&window_debug_paint_events,
		WC_DEBUG_PAINT,
		WF_STICK_TO_FRONT | WF_TRANSPARENT
	);

	window->widgets = window_debug_paint_widgets;
	window->enabled_widgets = (1 << WIDX_TOGGLE_OLD_DRAWING) | (1 << WIDX_TOGGLE_SHOW_BOUND_BOXES) | (1 << WIDX_TOGGLE_SHOW_SEGMENT_HEIGHTS);
	window_init_scroll_widgets(window);
	window_push_others_below(window);

	window->colours[0] = TRANSLUCENT(COLOUR_BLACK);
	window->colours[1] = COLOUR_GREY;
}

static void window_debug_paint_mouseup(rct_window * w, rct_widgetindex widgetIndex)
{
	switch (widgetIndex) {
		case WIDX_TOGGLE_OLD_DRAWING:
			gUseOriginalRidePaint = !gUseOriginalRidePaint;
			gfx_invalidate_screen();
			break;

		case WIDX_TOGGLE_SHOW_SEGMENT_HEIGHTS:
			gShowSupportSegmentHeights = !gShowSupportSegmentHeights;
			gfx_invalidate_screen();
			break;

		case WIDX_TOGGLE_SHOW_BOUND_BOXES:
			gPaintBoundingBoxes = !gPaintBoundingBoxes;
			gfx_invalidate_screen();
			break;
	}
}

static void window_debug_paint_invalidate(rct_window * w)
{
	widget_set_checkbox_value(w, WIDX_TOGGLE_OLD_DRAWING, gUseOriginalRidePaint);
	widget_set_checkbox_value(w, WIDX_TOGGLE_SHOW_SEGMENT_HEIGHTS, gShowSupportSegmentHeights);
	widget_set_checkbox_value(w, WIDX_TOGGLE_SHOW_BOUND_BOXES, gPaintBoundingBoxes);
}

static void window_debug_paint_paint(rct_window * w, rct_drawpixelinfo * dpi)
{
	window_draw_widgets(w, dpi);
}
