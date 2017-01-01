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

#include "../config.h"
#include "../interface/themes.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../interface/viewport.h"
#include "../localisation/localisation.h"
#include "../paint/paint.h"
#include "../rct2.h"

enum WINDOW_VIEW_CLIPPING_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_CLIP_HEIGHT_CHECKBOX,
	WIDX_CLIP_HEIGHT_VALUE,
	WIDX_CLIP_HEIGHT_SLIDER
};

#pragma region Widgets

rct_widget window_view_clipping_widgets[] = {
	{  WWT_FRAME, 0, 0, 159, 0, 64, STR_NONE, STR_NONE }, // panel / background
	{ WWT_CAPTION, 0, 1, 158, 1, 14, STR_VIEW_CLIPPING, STR_WINDOW_TITLE_TIP }, // title bar
	{ WWT_CLOSEBOX,	0, 147,	157, 2,	13, STR_CLOSE_X, STR_CLOSE_WINDOW_TIP }, // close x button
	{ WWT_CHECKBOX, 1, 11, 89, 19, 29, STR_VIEW_CLIPPING_HEIGHT_ENABLE, STR_VIEW_CLIPPING_HEIGHT_ENABLE_TIP }, // clip height enable/disable check box
	{ WWT_SPINNER, 1, 81, 111, 34, 44, STR_NONE, STR_NONE }, // clip height value spinner
	{ WWT_SCROLL, 1, 11, 149, 49, 59, SCROLL_HORIZONTAL, STR_VIEW_CLIPPING_HEIGHT_SCROLL_TIP }, // clip height scrollbar
	// Future: add checkbox(es) to only clip height in front of the cursor position; behind the cursor position is rendered normally.

	{ WIDGETS_END }
};

#pragma endregion

#pragma region Events

static void window_view_clipping_mouseup(rct_window *w, int widgetIndex);
static void window_view_clipping_mousedown(int widgetIndex, rct_window*w, rct_widget* widget);
static void window_view_clipping_update(rct_window *w);
static void window_view_clipping_invalidate(rct_window *w);
static void window_view_clipping_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_view_clipping_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height);
//static void window_view_clipping_text_input(rct_window *w, int widgetIndex, char *text);

static rct_window_event_list window_view_clipping_events = {
	window_view_clipping_close,
	window_view_clipping_mouseup,
	NULL,
	window_view_clipping_mousedown,
	NULL,
	NULL,
	window_view_clipping_update,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_view_clipping_scrollgetsize,
	NULL,
	NULL,
	NULL,
	NULL, //window_view_clipping_text_input,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_view_clipping_invalidate,
	window_view_clipping_paint,
	NULL
};

#pragma endregion

rct_string_id clipHeight_Units_StringId;

void window_view_clipping_open()
{
	rct_window* window;

	// Get the main viewport to set the view clipping flag.
	rct_window *mainWindow = window_get_main();

	// Check if window is already open
	if (window_find_by_class(WC_VIEW_CLIPPING) != NULL) {
		// If window is already open, toggle the view clipping on/off
		if (mainWindow != NULL) {
			mainWindow->viewport->flags ^= VIEWPORT_FLAG_PAINT_CLIP_TO_HEIGHT;
			window_invalidate(mainWindow);
		}

		return;
	}

	// Window is not open - create it.
	window = window_create(32, 32, 160, 65, &window_view_clipping_events, WC_VIEW_CLIPPING, 0);
	window->widgets = window_view_clipping_widgets;
	window->enabled_widgets = (1 << WIDX_CLOSE) |
		(1 << WIDX_CLIP_HEIGHT_CHECKBOX) |
		(1 << WIDX_CLIP_HEIGHT_VALUE) |
		(1 << WIDX_CLIP_HEIGHT_SLIDER);
	window->frame_no = 0; // Needed for scroll widgets to function?
	window_init_scroll_widgets(window);
	window_push_others_below(window);

	//window_invalidate(window);

	//colour_scheme_update(window); // Segfaults - not set up for colour schemes?

	// Set the height units symbol
	switch (gConfigGeneral.measurement_format) {
		case MEASUREMENT_FORMAT_METRIC:
		case MEASUREMENT_FORMAT_SI:
			clipHeight_Units_StringId = STR_UNIT_SUFFIX_METRES;
			break;
		case MEASUREMENT_FORMAT_IMPERIAL:
		default:
			clipHeight_Units_StringId = STR_UNIT_SUFFIX_FEET;
			break;
	}

	//gClipHeightEnable = true; // Use the VIEWPORT_FLAG_PAINT_CLIP_TO_HEIGHT to record whether clipping is on/off.

	// Turn on view clipping when the window is opened.
	if (mainWindow != NULL) {
		mainWindow->viewport->flags |= VIEWPORT_FLAG_PAINT_CLIP_TO_HEIGHT;
		window_invalidate(mainWindow);
	}
}

void window_view_clipping_close()
{
	//gClipHeightEnable = false;

	// Turn off view clipping when the window is closed.
	rct_window *mainWindow = window_get_main();
	if (mainWindow != NULL) {
		mainWindow->viewport->flags &= ~VIEWPORT_FLAG_PAINT_CLIP_TO_HEIGHT;
		window_invalidate(mainWindow);
	}

}

static void window_view_clipping_mouseup(rct_window *w, int widgetIndex)
{
	rct_window *mainWindow;

	// mouseup appears to be used for buttons, checkboxes
	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_CLIP_HEIGHT_CHECKBOX:
		// Toggle height clipping.
		mainWindow = window_get_main();
		if (mainWindow != NULL) {
			mainWindow->viewport->flags ^= VIEWPORT_FLAG_PAINT_CLIP_TO_HEIGHT;
			window_invalidate(mainWindow);
		}
		window_invalidate(w);
		break;
	}
}

static void window_view_clipping_mousedown(int widgetIndex, rct_window*w, rct_widget* widget)
{
	// mousedown appears to be used primarily for dropdown list boxes
	switch (widgetIndex) {
		default:
			break;
	}
}

static void window_view_clipping_update(rct_window *w)
{
	fprintf(stderr, "Update start\n");
	rct_widget *widget;
	widget = &window_view_clipping_widgets[WIDX_CLIP_HEIGHT_SLIDER];
	uint8 clip_height = (uint8)(((float)w->scrolls[0].h_left / (w->scrolls[0].h_right - ((widget->right - widget->left) - 1))) * 255);
	if (clip_height != gClipHeight) {
		fprintf(stderr, "Update clip height from scrollbar\n");
		gClipHeight = clip_height;

		// Update the main window accordingly.
		rct_window *mainWindow = window_get_main();
		if (mainWindow != NULL) {
			window_invalidate(mainWindow);
		}
	}
	widget_invalidate(w, WIDX_CLIP_HEIGHT_SLIDER);
	fprintf(stderr, "Update end\n");
}

static void window_view_clipping_invalidate(rct_window *w)
{
	//rct_widget* widget;

	fprintf(stderr, "Invalidate start\n");
	//colour_scheme_update(w);

	//window_init_scroll_widgets(w); // Needed?
	w->disabled_widgets = 0;
	// Disable the height slider and height value according to the height checkbox.
	rct_window *mainWindow = window_get_main();
	//if (mainWindow != NULL) {
	//	if ((mainWindow->viewport->flags & VIEWPORT_FLAG_PAINT_CLIP_TO_HEIGHT) == 0) {
	//		w->disabled_widgets |= (1 << WIDX_CLIP_HEIGHT_SLIDER);
	//		w->disabled_widgets |= (1 << WIDX_CLIP_HEIGHT_VALUE);
	//	}
	//}

	// Set the clip height slider value.
	// TODO: Initialise the scroll position - do this once, not based on frame_no -> try moving to opening the window?
	//if (w->frame_no == 0) { // No animation in this windows so should not be needed?
	//	widget = &window_view_clipping_widgets[WIDX_CLIP_HEIGHT_SLIDER];
	//	w->scrolls[0].h_left = (sint16)ceil((gClipHeight / 255.0f) * (w->scrolls[0].h_right - ((widget->right - widget->left) - 1)));
	//}
	widget_scroll_update_thumbs(w, WIDX_CLIP_HEIGHT_SLIDER);

	if (mainWindow != NULL) {
		widget_set_checkbox_value(w, WIDX_CLIP_HEIGHT_CHECKBOX, mainWindow->viewport->flags & VIEWPORT_FLAG_PAINT_CLIP_TO_HEIGHT);
	}

	// TODO: is the following necessary if the widgets are always shown?
	// Widgets should be enabled/disabled (i.e. greyed out to make inactive) using w->disabled_widgets above).
	//window_view_clipping_widgets[WIDX_CLIP_HEIGHT_CHECKBOX].type = WWT_CHECKBOX;
	//window_view_clipping_widgets[WIDX_CLIP_HEIGHT_SLIDER].type = WWT_SCROLL;
	//window_view_clipping_widgets[WIDX_CLIP_HEIGHT_VALUE].type = WWT_SPINNER;
	fprintf(stderr, "Invalidate end\n");
}

static void window_view_clipping_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);

	// Clip height value
	// Currently as a spinner.
	// Alternately could try putting the value on the scrollbar.
	int x;
	int y;
	if (w->widgets[WIDX_CLIP_HEIGHT_VALUE].type != WWT_EMPTY) {
		x = w->x + 8;
		y = w->y + w->widgets[WIDX_CLIP_HEIGHT_VALUE].top;
		gfx_draw_string_left(dpi, STR_VIEW_CLIPPING_HEIGHT_VALUE, NULL, w->colours[1], x, y);

		x = w->x + w->widgets[WIDX_CLIP_HEIGHT_VALUE].left + 1;
		y = w->y + w->widgets[WIDX_CLIP_HEIGHT_VALUE].top;
		gfx_draw_string_left(dpi, STR_FORMAT_INTEGER, &gClipHeight, w->colours[1], x, y); //Printing the raw value.

		fixed16_1dp clipHeightValueInUnits; // The clip height in the unit type - feet or meters. For value in meters a fixed point number is needed.
		switch (clipHeight_Units_StringId) {
			case STR_UNIT_SUFFIX_FEET:
				clipHeightValueInUnits = FIXED_1DP(gClipHeight, 0) / 2 * 5 - FIXED_1DP(35,0);
				break;
			case STR_UNIT_SUFFIX_METRES:
			default:
				clipHeightValueInUnits = FIXED_1DP(gClipHeight, 0) / 2 * 1.5 - FIXED_1DP(10,5);
			fprintf(stderr, "Meters: %d\n", clipHeightValueInUnits);
				break;
		}
		// TODO: Display fixed point value correctly.
		gfx_draw_string_left(dpi, clipHeight_Units_StringId, &clipHeightValueInUnits, w->colours[1], x + 30, y);
	}
}

static void window_view_clipping_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height)
{
	*width = 1000;
}

