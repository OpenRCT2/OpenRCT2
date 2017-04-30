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
#include "../interface/themes.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../rct2.h"
#include "../sprites.h"
#include "../world/map.h"
#include "../world/scenery.h"

#define MINIMUM_TOOL_SIZE 1
#define MAXIMUM_TOOL_SIZE 64

enum WINDOW_CLEAR_SCENERY_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_PREVIEW,
	WIDX_DECREMENT,
	WIDX_INCREMENT,
	WIDX_SMALL_SCENERY,
	WIDX_LARGE_SCENERY,
	WIDX_FOOTPATH
};

rct_widget window_clear_scenery_widgets[] = {
	{ WWT_FRAME,	0,	0,	97,	0,	93,	0xFFFFFFFF,									STR_NONE },							// panel / background
	{ WWT_CAPTION,	0,	1,	96,	1,	14,	STR_CLEAR_SCENERY,							STR_WINDOW_TITLE_TIP },				// title bar
	{ WWT_CLOSEBOX,	0,	85,	95,	2,	13,	STR_CLOSE_X,								STR_CLOSE_WINDOW_TIP },				// close x button
	{ WWT_IMGBTN,	0,	27,	70,	17,	48,	SPR_LAND_TOOL_SIZE_0,						STR_NONE },							// preview box
	{ WWT_TRNBTN,	1,	28,	43,	18,	33,	0x20000000 | SPR_LAND_TOOL_DECREASE,		STR_ADJUST_SMALLER_LAND_TIP },		// decrement size
	{ WWT_TRNBTN,	1,	54,	69,	32,	47,	0x20000000 | SPR_LAND_TOOL_INCREASE,		STR_ADJUST_LARGER_LAND_TIP },		// increment size
	{ WWT_FLATBTN,  1,	7,	30,	53,	76,	0x20000000 | SPR_G2_BUTTON_TREES,			STR_CLEAR_SCENERY_REMOVE_SMALL_SCENERY_TIP }, // small scenery
	{ WWT_FLATBTN,	1,	37,	60,	53,	76,	0x20000000 | SPR_G2_BUTTON_LARGE_SCENERY,	STR_CLEAR_SCENERY_REMOVE_LARGE_SCENERY_TIP }, // large scenery
	{ WWT_FLATBTN,	1,	67,	90,	53,	76,	0x20000000 | SPR_G2_BUTTON_FOOTPATH,		STR_CLEAR_SCENERY_REMOVE_FOOTPATHS_TIP }, // footpaths
	{ WIDGETS_END },
};

static void window_clear_scenery_close(rct_window *w);
static void window_clear_scenery_mouseup(rct_window *w, sint32 widgetIndex);
static void window_clear_scenery_update(rct_window *w);
static void window_clear_scenery_invalidate(rct_window *w);
static void window_clear_scenery_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_clear_scenery_textinput(rct_window *w, sint32 widgetIndex, char *text);
static void window_clear_scenery_inputsize(rct_window *w);

static rct_window_event_list window_clear_scenery_events = {
	window_clear_scenery_close,
	window_clear_scenery_mouseup,
	NULL,
	NULL,
	NULL,
	NULL,
	window_clear_scenery_update,
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
	window_clear_scenery_textinput,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_clear_scenery_invalidate,
	window_clear_scenery_paint,
	NULL
};

/**
 *
 *  rct2: 0x0068E0A7
 */
void window_clear_scenery_open()
{
	rct_window* window;

	// Check if window is already open
	if (window_find_by_class(WC_CLEAR_SCENERY) != NULL)
		return;

	window = window_create(gScreenWidth - 98, 29, 98, 94, &window_clear_scenery_events, WC_CLEAR_SCENERY, 0);
	window->widgets = window_clear_scenery_widgets;
	window->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_INCREMENT) | (1 << WIDX_DECREMENT) | (1 << WIDX_PREVIEW) |
		(1 << WIDX_SMALL_SCENERY) | (1 << WIDX_LARGE_SCENERY) | (1 << WIDX_FOOTPATH);
	window_init_scroll_widgets(window);
	window_push_others_below(window);

	gClearSceneryCost = MONEY32_UNDEFINED;

	gClearSmallScenery = true;
	gClearLargeScenery = false;
	gClearFootpath = false;
}

/**
 *
 *  rct2: 0x006E6B65
 */
static void window_clear_scenery_close(rct_window *w)
{
	// If the tool wasn't changed, turn tool off
	if (clear_scenery_tool_is_active())
		tool_cancel();
}

/**
 *
 *  rct2: 0x0068E185
 */
static void window_clear_scenery_mouseup(rct_window *w, sint32 widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_DECREMENT:
		// Decrement land tool size, if it stays within the limit
		gLandToolSize = max(MINIMUM_TOOL_SIZE, gLandToolSize - 1);

		// Invalidate the window
		window_invalidate(w);
		break;
	case WIDX_INCREMENT:
		// Increment land tool size, if it stays within the limit
		gLandToolSize = min(MAXIMUM_TOOL_SIZE, gLandToolSize + 1);

		// Invalidate the window
		window_invalidate(w);
		break;
	case WIDX_PREVIEW:
		window_clear_scenery_inputsize(w);
		break;
	case WIDX_SMALL_SCENERY:
		gClearSmallScenery ^= 1;
		window_invalidate(w);
		break;
	case WIDX_LARGE_SCENERY:
		gClearLargeScenery ^= 1;
		window_invalidate(w);
		break;
	case WIDX_FOOTPATH:
		gClearFootpath ^= 1;
		window_invalidate(w);
		break;
	}
}

static void window_clear_scenery_textinput(rct_window *w, sint32 widgetIndex, char *text)
{
	sint32 size;
	char* end;

	if (widgetIndex != WIDX_PREVIEW || text == NULL)
		return;

	size = strtol(text, &end, 10);
	if (*end == '\0') {
		size=max(MINIMUM_TOOL_SIZE,size);
		size=min(MAXIMUM_TOOL_SIZE,size);
		gLandToolSize = size;
		window_invalidate(w);
	}
}

static void window_clear_scenery_inputsize(rct_window *w)
{
	TextInputDescriptionArgs[0] = MINIMUM_TOOL_SIZE;
	TextInputDescriptionArgs[1] = MAXIMUM_TOOL_SIZE;
	window_text_input_open(w, WIDX_PREVIEW, STR_SELECTION_SIZE, STR_ENTER_SELECTION_SIZE, STR_NONE, STR_NONE, 3);
}

/**
 *
 *  rct2: 0x0068E205
 */
static void window_clear_scenery_update(rct_window *w)
{
	// Close window if another tool is open
	if (!clear_scenery_tool_is_active())
		window_close(w);
}

/**
 *
 *  rct2: 0x0068E115
 */
static void window_clear_scenery_invalidate(rct_window *w)
{
	colour_scheme_update(w);

	// Set the preview image button to be pressed down
	w->pressed_widgets = (1 << WIDX_PREVIEW) |
		(gClearSmallScenery ? (1 << WIDX_SMALL_SCENERY) : 0) |
		(gClearLargeScenery ? (1 << WIDX_LARGE_SCENERY) : 0) |
		(gClearFootpath     ? (1 << WIDX_FOOTPATH)      : 0);

	// Update the preview image (for tool sizes up to 7)
	window_clear_scenery_widgets[WIDX_PREVIEW].image = gLandToolSize <= 7 ?
		SPR_LAND_TOOL_SIZE_0 + gLandToolSize : 0xFFFFFFFF;
}

/**
 *
 *  rct2: 0x0068E130
 */
static void window_clear_scenery_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	sint32 x, y;

	window_draw_widgets(w, dpi);

	// Draw number for tool sizes bigger than 7
	x = w->x + (window_clear_scenery_widgets[WIDX_PREVIEW].left + window_clear_scenery_widgets[WIDX_PREVIEW].right) / 2;
	y = w->y + (window_clear_scenery_widgets[WIDX_PREVIEW].top + window_clear_scenery_widgets[WIDX_PREVIEW].bottom) / 2;
	if (gLandToolSize > 7) {
		gfx_draw_string_centred(dpi, STR_LAND_TOOL_SIZE_VALUE, x, y - 2, COLOUR_BLACK, &gLandToolSize);
	}

	// Draw cost amount
	x = (window_clear_scenery_widgets[WIDX_PREVIEW].left + window_clear_scenery_widgets[WIDX_PREVIEW].right) / 2 + w->x;
	y = window_clear_scenery_widgets[WIDX_PREVIEW].bottom + w->y + 5 + 27;
	if (gClearSceneryCost != MONEY32_UNDEFINED &&
		gClearSceneryCost != 0
	) {
		gfx_draw_string_centred(dpi, STR_COST_AMOUNT, x, y, COLOUR_BLACK, &gClearSceneryCost);
	}
}
