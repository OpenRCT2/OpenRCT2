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

#include "../audio/audio.h"
#include "../Context.h"
#include "../drawing/drawing.h"
#include "../game.h"
#include "../input.h"
#include "../interface/viewport.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../peep/peep.h"
#include "../rct2.h"
#include "../world/map.h"
#include "../world/scenery.h"
#include "../world/sprite.h"
#include "dropdown.h"
#include "../network/network.h"
#include "error.h"
#include "../sprites.h"

#define WINDOW_SCENERY_WIDTH	634
#define WINDOW_SCENERY_HEIGHT	180
#define SCENERY_BUTTON_WIDTH	66
#define SCENERY_BUTTON_HEIGHT	80

enum {
	WINDOW_SCENERY_TAB_1,
	WINDOW_SCENERY_TAB_2,
	WINDOW_SCENERY_TAB_3,
	WINDOW_SCENERY_TAB_4,
	WINDOW_SCENERY_TAB_5,
	WINDOW_SCENERY_TAB_6,
	WINDOW_SCENERY_TAB_7,
	WINDOW_SCENERY_TAB_8,
	WINDOW_SCENERY_TAB_9,
	WINDOW_SCENERY_TAB_10,
	WINDOW_SCENERY_TAB_11,
	WINDOW_SCENERY_TAB_12,
	WINDOW_SCENERY_TAB_13,
	WINDOW_SCENERY_TAB_14,
	WINDOW_SCENERY_TAB_15,
	WINDOW_SCENERY_TAB_16,
	WINDOW_SCENERY_TAB_17,
	WINDOW_SCENERY_TAB_18,
	WINDOW_SCENERY_TAB_19,
	WINDOW_SCENERY_TAB_20
};

static void window_scenery_close(rct_window *w);
static void window_scenery_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_scenery_resize(rct_window *w);
static void window_scenery_mousedown(rct_widgetindex widgetIndex, rct_window* w, rct_widget* widget);
static void window_scenery_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex);
static void window_scenery_update(rct_window *w);
static void window_scenery_event_07(rct_window *w);
static void window_scenery_scrollgetsize(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height);
static void window_scenery_scrollmousedown(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y);
static void window_scenery_scrollmouseover(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y);
static void window_scenery_tooltip(rct_window* w, rct_widgetindex widgetIndex, rct_string_id *stringId);
static void window_scenery_invalidate(rct_window *w);
static void window_scenery_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_scenery_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex);

static rct_window_event_list window_scenery_events = {
	window_scenery_close,
	window_scenery_mouseup,
	window_scenery_resize,
	window_scenery_mousedown,
	window_scenery_dropdown,
	NULL,
	window_scenery_update,
	window_scenery_event_07,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_scenery_scrollgetsize,
	window_scenery_scrollmousedown,
	NULL,
	window_scenery_scrollmouseover,
	NULL,
	NULL,
	NULL,
	window_scenery_tooltip,
	NULL,
	NULL,
	window_scenery_invalidate,
	window_scenery_paint,
	window_scenery_scrollpaint,
};


enum WINDOW_SCENERY_LIST_WIDGET_IDX {
	WIDX_SCENERY_BACKGROUND,				// 1
	WIDX_SCENERY_TITLE,						// 2
	WIDX_SCENERY_CLOSE,						// 4
	WIDX_SCENERY_TAB_CONTENT_PANEL,			// 8
	WIDX_SCENERY_TAB_1,						// 10
	WIDX_SCENERY_TAB_2,						// 20
	WIDX_SCENERY_TAB_3,						// 40
	WIDX_SCENERY_TAB_4,						// 80
	WIDX_SCENERY_TAB_5,						// 100
	WIDX_SCENERY_TAB_6,						// 200
	WIDX_SCENERY_TAB_7,						// 400
	WIDX_SCENERY_TAB_8,						// 800
	WIDX_SCENERY_TAB_9,						// 1000
	WIDX_SCENERY_TAB_10,					// 2000
	WIDX_SCENERY_TAB_11,					// 4000
	WIDX_SCENERY_TAB_12,					// 8000
	WIDX_SCENERY_TAB_13,					// 10000
	WIDX_SCENERY_TAB_14,					// 20000
	WIDX_SCENERY_TAB_15,					// 40000
	WIDX_SCENERY_TAB_16,					// 80000
	WIDX_SCENERY_TAB_17,					// 100000
	WIDX_SCENERY_TAB_18,					// 200000
	WIDX_SCENERY_TAB_19,					// 400000
	WIDX_SCENERY_TAB_20,					// 800000
	WIDX_SCENERY_LIST,						// 1000000
	WIDX_SCENERY_ROTATE_OBJECTS_BUTTON,		// 2000000
	WIDX_SCENERY_REPAINT_SCENERY_BUTTON,	// 4000000
	WIDX_SCENERY_PRIMARY_COLOUR_BUTTON,		// 8000000
	WIDX_SCENERY_SECONDARY_COLOUR_BUTTON,	// 10000000
	WIDX_SCENERY_TERTIARY_COLOUR_BUTTON,	// 20000000
	WIDX_SCENERY_EYEDROPPER_BUTTON,			// 40000000
	WIDX_SCENERY_BUILD_CLUSTER_BUTTON,		// 80000000
};

validate_global_widx(WC_SCENERY, WIDX_SCENERY_TAB_1);
validate_global_widx(WC_SCENERY, WIDX_SCENERY_ROTATE_OBJECTS_BUTTON);

static rct_widget window_scenery_widgets[] = {
	{ WWT_FRAME, 0, 0, 633, 0, 141, 0xFFFFFFFF, STR_NONE },								// 1				0x009DE298
	{ WWT_CAPTION, 0, 1, 632, 1, 14, 0xFFFFFFFF, STR_WINDOW_TITLE_TIP },				// 2				0x009DE2A8
	{ WWT_CLOSEBOX, 0, 621, 631, 2, 13, STR_CLOSE_X, STR_CLOSE_WINDOW_TIP },			// 4				0x009DE2B8
	{ WWT_RESIZE, 1, 0, 633, 43, 141, 0xFFFFFFFF, STR_NONE },							// 8				0x009DE2C8
	{ WWT_TAB, 1, 3, 33, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },									// 10				0x009DE2D8
	{ WWT_TAB, 1, 34, 64, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },									// 20				0x009DE2E8
	{ WWT_TAB, 1, 65, 95, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },									// 40				0x009DE2F8
	{ WWT_TAB, 1, 96, 126, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },									// 80				0x009DE308
	{ WWT_TAB, 1, 127, 157, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },									// 100				0x009DE318
	{ WWT_TAB, 1, 158, 188, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },									// 200				0x009DE328
	{ WWT_TAB, 1, 189, 219, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },									// 400				0x009DE338
	{ WWT_TAB, 1, 220, 250, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },									// 800				0x009DE348
	{ WWT_TAB, 1, 251, 281, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },									// 1000				0x009DE358
	{ WWT_TAB, 1, 282, 312, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },									// 2000				0x009DE368
	{ WWT_TAB, 1, 313, 343, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },									// 4000				0x009DE378
	{ WWT_TAB, 1, 344, 374, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },									// 8000				0x009DE388
	{ WWT_TAB, 1, 375, 405, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },									// 10000			0x009DE398
	{ WWT_TAB, 1, 406, 436, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },									// 20000			0x009DE3A8
	{ WWT_TAB, 1, 437, 467, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },									// 40000			0x009DE3B8
	{ WWT_TAB, 1, 468, 498, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },									// 80000			0x009DE3C8
	{ WWT_TAB, 1, 468, 498, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },									// 100000			0x009DE3D8
	{ WWT_TAB, 1, 468, 498, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },									// 200000			0x009DE3E8
	{ WWT_TAB, 1, 468, 498, 17, 43, 0xFFFFFFFF, STR_STRING_DEFINED_TOOLTIP },									// 400000			0x009DE3F8
	{ WWT_TAB, 1, 468, 498, 17, 43, 0x20000000 | SPR_TAB_QUESTION, STR_STRING_DEFINED_TOOLTIP },									// 800000			0x009DE408
	{ WWT_SCROLL, 1, 2, 608, 47, 126, SCROLL_VERTICAL, STR_NONE },									// 1000000			0x009DE418
	{ WWT_FLATBTN, 1, 609, 632, 44, 67, SPR_ROTATE_ARROW, STR_ROTATE_OBJECTS_90 },					// 2000000			0x009DE428
	{ WWT_FLATBTN, 1, 609, 632, 68, 91, SPR_PAINTBRUSH, STR_SCENERY_PAINTBRUSH_TIP },									// 4000000			0x009DE438
	{ WWT_COLOURBTN, 1, 615, 626, 93, 104, 0xFFFFFFFF, STR_SELECT_COLOUR },				// 8000000			0x009DE448
	{ WWT_COLOURBTN, 1, 615, 626, 105, 116, 0xFFFFFFFF, STR_SELECT_SECONDARY_COLOUR },	// 10000000			0x009DE458
	{ WWT_COLOURBTN, 1, 615, 626, 117, 128, 0xFFFFFFFF, STR_SELECT_TERNARY_COLOUR },		// 20000000			0x009DE468
	{ WWT_FLATBTN, 1, 609, 632, 130, 153, SPR_PICKUP_BTN, STR_SCENERY_EYEDROPPER_TIP },									// 40000000			0x009DE478
	{ WWT_FLATBTN, 1, 609, 632, 154, 177, SPR_SCENERY_CLUSTER, STR_SCENERY_CLUSTER_TIP },									// 40000000			0x009DE478
	{ WIDGETS_END },
};

// rct2: 0x00F64F2C
sint16 window_scenery_tab_entries[20][SCENERY_ENTRIES_BY_TAB + 1];

void window_scenery_update_scroll(rct_window *w);

/**
 * Was part of 0x006DFA00
 * The same code repeated five times for every scenery entry type
 */
static void init_scenery_entry(rct_scenery_entry *sceneryEntry, sint32 index, uint8 sceneryTabId)
{
	if (scenery_is_invented(index)) {
		if (sceneryTabId != 0xFF) {
			for (sint32 i = 0; i < SCENERY_ENTRIES_BY_TAB; i++) {
				if (window_scenery_tab_entries[sceneryTabId][i] == -1)
				{
					window_scenery_tab_entries[sceneryTabId][i] = index;
					window_scenery_tab_entries[sceneryTabId][i + 1] = -1;
					return;
				}
			}
		}

		for (sint32 i = 0; i < 19; i++) {
			sint32 counter = 0;

			while (window_scenery_tab_entries[i][counter] != -1)
			{
				if (window_scenery_tab_entries[i][counter] == index) {
					return;
				}

				counter++;
			}
		}

		for (sint32 i = 0; i < SCENERY_ENTRIES_BY_TAB; i++) {
			if (window_scenery_tab_entries[19][i] == -1)
			{
				window_scenery_tab_entries[19][i] = index;
				window_scenery_tab_entries[19][i + 1] = -1;
				break;
			}
		}
	}
}

/**
 *
 *  rct2: 0x006DFA00
 */
void init_scenery()
{
	bool enabledScenerySets[20] = { false };

	for (sint32 scenerySetIndex = 0; scenerySetIndex < 20; scenerySetIndex++) {
		window_scenery_tab_entries[scenerySetIndex][0] = -1;
		if (scenerySetIndex == 19)
			continue;

		rct_scenery_set_entry* scenerySetEntry = get_scenery_group_entry(scenerySetIndex);
		if (scenerySetEntry == (rct_scenery_set_entry *)-1)
			continue;

		sint32 sceneryTabEntryCount = 0;
		for (sint32 i = 0; i < scenerySetEntry->entry_count; i++) {
			uint16 sceneryEntryId = scenerySetEntry->scenery_entries[i];
			if (scenery_is_invented(sceneryEntryId)) {
				window_scenery_tab_entries[scenerySetIndex][sceneryTabEntryCount] = sceneryEntryId;
				window_scenery_tab_entries[scenerySetIndex][++sceneryTabEntryCount] = -1;
			} else {
				enabledScenerySets[scenerySetIndex] = true;
			}
		}
	}

	// small scenery
	for (uint16 sceneryId = 0; sceneryId < 0xFC; sceneryId++) {
		if (get_small_scenery_entry(sceneryId) == (rct_scenery_entry *)-1)
			continue;

		rct_scenery_entry* sceneryEntry = get_small_scenery_entry(sceneryId);
		init_scenery_entry(sceneryEntry, sceneryId, sceneryEntry->small_scenery.scenery_tab_id);
	}

	// large scenery
	for (sint32 sceneryId = 0x300; sceneryId < 0x380; sceneryId++) {
		sint32 largeSceneryIndex = sceneryId - 0x300;

		if (get_large_scenery_entry(largeSceneryIndex) == (rct_scenery_entry *)-1)
			continue;

		rct_scenery_entry* sceneryEntry = get_large_scenery_entry(largeSceneryIndex);
		init_scenery_entry(sceneryEntry, sceneryId, sceneryEntry->large_scenery.scenery_tab_id);
	}

	// walls
	for (sint32 sceneryId = 0x200; sceneryId < 0x280; sceneryId++) {
		sint32 wallSceneryIndex = sceneryId - 0x200;

		if (get_wall_entry(wallSceneryIndex) == (rct_scenery_entry *)-1)
			continue;

		rct_scenery_entry* sceneryEntry = get_wall_entry(wallSceneryIndex);
		init_scenery_entry(sceneryEntry, sceneryId, sceneryEntry->wall.scenery_tab_id);
	}

	// banners
	for (sint32 sceneryId = 0x400; sceneryId < 0x420; sceneryId++) {
		sint32 bannerIndex = sceneryId - 0x400;

		if (get_banner_entry(bannerIndex) == (rct_scenery_entry *)-1)
			continue;

		rct_scenery_entry* sceneryEntry = get_banner_entry(bannerIndex);
		init_scenery_entry(sceneryEntry, sceneryId, sceneryEntry->banner.scenery_tab_id);
	}

	// path bits
	for (sint32 sceneryId = 0x100; sceneryId < 0x10F; sceneryId++) {
		sint32 pathBitIndex = sceneryId - 0x100;

		if (get_footpath_item_entry(pathBitIndex) == (rct_scenery_entry *)-1)
			continue;

		rct_scenery_entry* sceneryEntry = get_footpath_item_entry(pathBitIndex);
		init_scenery_entry(sceneryEntry, sceneryId, sceneryEntry->path_bit.scenery_tab_id);
	}

	for (rct_widgetindex widgetIndex = WIDX_SCENERY_TAB_1; widgetIndex < WIDX_SCENERY_LIST; widgetIndex++)
		window_scenery_widgets[widgetIndex].type = WWT_EMPTY;

	uint8 tabIndexes[20];
	uint8 order[20];
	sint32 usedValues = 0;

	for (sint32 scenerySetId = 0; scenerySetId < 19; scenerySetId++) {
		rct_scenery_set_entry* sceneryEntry = get_scenery_group_entry(scenerySetId);
		if (sceneryEntry == (rct_scenery_set_entry *)-1)
			continue;

		tabIndexes[usedValues] = scenerySetId;
		order[usedValues] = sceneryEntry->var_108;

		usedValues++;
	}

	while (true) {
		bool finished = true;
		for (sint32 i = 1; i < usedValues; i++) {
			if (order[i - 1] > order[i]) {
				uint8 tmp = tabIndexes[i - 1];
				tabIndexes[i - 1] = tabIndexes[i];
				tabIndexes[i] = tmp;
				tmp = order[i - 1];
				order[i - 1] = order[i];
				order[i] = tmp;
				finished = false;
			}
		}

		if (finished)
			break;
	}

	tabIndexes[usedValues] = 19;
	usedValues++;

	uint16 left = 3;
	for (sint32 i = 0; i < usedValues; i ++) {
		uint32 tabIndex = tabIndexes[i];
		rct_widget* tabWidget = &window_scenery_widgets[tabIndex + WIDX_SCENERY_TAB_1];

		if (left != 3 || tabIndex != 19) {
			if (window_scenery_tab_entries[tabIndex][0] == -1)
				continue;

			if (enabledScenerySets[tabIndex])
				continue;
		}

		tabWidget->type = WWT_TAB;
		tabWidget->left = left;
		tabWidget->right = left + 0x1E;
		left += 0x1F;

		if (tabIndex >= 19)
			continue;

		tabWidget->image = get_scenery_group_entry(tabIndex)->image | 0x20000000;
	}

	window_invalidate_by_class(WC_SCENERY);
}

/**
 *
 *  rct2: 0x006DFEE4
 */
void scenery_set_default_placement_configuration()
{
	gWindowSceneryRotation = 3;
	gWindowSceneryPrimaryColour = COLOUR_BORDEAUX_RED;
	gWindowScenerySecondaryColour = COLOUR_YELLOW;
	gWindowSceneryTertiaryColour = COLOUR_DARK_BROWN;
	init_scenery();

	for (sint32 i = 0; i < 20; i++)
		gWindowSceneryTabSelections[i] = -1;

	for (sint32 i = 0; i < 20; i++) {
		if (window_scenery_tab_entries[i][0] != -1) {
			gWindowSceneryActiveTabIndex = i;
			return;
		}
	}

	for (sint32 i = 0; i < 16; i++) {
		rct_widget *tabWidget = &window_scenery_widgets[WIDX_SCENERY_TAB_1 + i];
		if (tabWidget->type != WWT_EMPTY) {
			gWindowSceneryActiveTabIndex = i;
			return;
		}
	}
}

/**
 *
 *  rct2: 0x006E0FEF
 */
void window_scenery_open()
{
	rct_window* window;

	// Check if window is already open
	window = window_bring_to_front_by_class(WC_SCENERY);
	if (window != NULL)
		return;

	init_scenery();

	window = window_create(
		gScreenWidth - WINDOW_SCENERY_WIDTH,
		0x1D,
		WINDOW_SCENERY_WIDTH,
		WINDOW_SCENERY_HEIGHT,
		&window_scenery_events,
		WC_SCENERY,
		WF_NO_SCROLLING
	);
	window->widgets = window_scenery_widgets;

	window->enabled_widgets =
		(1 << WIDX_SCENERY_CLOSE) |
		(1 << WIDX_SCENERY_ROTATE_OBJECTS_BUTTON) |
		(1 << WIDX_SCENERY_TAB_1) |
		(1 << WIDX_SCENERY_TAB_2) |
		(1 << WIDX_SCENERY_TAB_3) |
		(1 << WIDX_SCENERY_TAB_4) |
		(1 << WIDX_SCENERY_TAB_5) |
		(1 << WIDX_SCENERY_TAB_6) |
		(1 << WIDX_SCENERY_TAB_7) |
		(1 << WIDX_SCENERY_TAB_8) |
		(1 << WIDX_SCENERY_TAB_9) |
		(1 << WIDX_SCENERY_TAB_10) |
		(1 << WIDX_SCENERY_TAB_11) |
		(1 << WIDX_SCENERY_TAB_12) |
		(1 << WIDX_SCENERY_TAB_13) |
		(1 << WIDX_SCENERY_TAB_14) |
		(1 << WIDX_SCENERY_TAB_15) |
		(1 << WIDX_SCENERY_TAB_16) |
		(1 << WIDX_SCENERY_TAB_17) |
		(1 << WIDX_SCENERY_TAB_18) |
		(1 << WIDX_SCENERY_TAB_19) |
		(1 << WIDX_SCENERY_TAB_20) |
		(1 << WIDX_SCENERY_PRIMARY_COLOUR_BUTTON) |
		(1 << WIDX_SCENERY_SECONDARY_COLOUR_BUTTON) |
		(1 << WIDX_SCENERY_REPAINT_SCENERY_BUTTON) |
		(1 << WIDX_SCENERY_TERTIARY_COLOUR_BUTTON) |
		(1 << WIDX_SCENERY_EYEDROPPER_BUTTON) |
		(1 << WIDX_SCENERY_BUILD_CLUSTER_BUTTON);

	window_init_scroll_widgets(window);
	window_scenery_update_scroll(window);
	show_gridlines();
	gWindowSceneryRotation = 3;
	gSceneryCtrlPressed = false;
	gSceneryShiftPressed = false;
	window->scenery.selected_scenery_id = -1;
	window->scenery.hover_counter = 0;
	window_push_others_below(window);
	gSceneryGhostType = 0;
	gSceneryPlaceCost = MONEY32_UNDEFINED;
	gSceneryPlaceRotation = 0;
	gWindowSceneryPaintEnabled = 0; // repaint coloured scenery tool state
	gWindowSceneryEyedropperEnabled = false;
	gWindowSceneryClusterEnabled = 0; // build cluster tool state

	window->min_width = WINDOW_SCENERY_WIDTH;
	window->max_width = WINDOW_SCENERY_WIDTH;
	window->min_height = WINDOW_SCENERY_HEIGHT;
	window->max_height = WINDOW_SCENERY_HEIGHT;
}

/**
 *
 *  rct2: 0x006E1A73
 */
void window_scenery_close(rct_window *w)
{
	scenery_remove_ghost_tool_placement();
	hide_gridlines();
	viewport_set_visibility(0);

	if (scenery_tool_is_active())
		tool_cancel();
}

static sint32 count_rows(sint32 items){
	sint32 rows = items / 9;

	return rows;
}

typedef struct scenery_item {
	sint32 allRows;
	sint32 selected_item;
	sint16 sceneryId;
} scenery_item;

static scenery_item window_scenery_count_rows_with_selected_item(sint32 tabIndex)
{
	scenery_item sceneryItem = { 0, 0, -1 };
	sint32 totalItems = 0;
	sint16 id = 0;
	sint16 sceneryId = gWindowSceneryTabSelections[tabIndex];

	while ((id = window_scenery_tab_entries[tabIndex][totalItems]) != -1){
		if (id == sceneryId){
			sceneryItem.selected_item = totalItems;
			sceneryItem.sceneryId = sceneryId;
		}
		totalItems++;
	}
	sceneryItem.allRows = count_rows(totalItems + 8);
	return sceneryItem;
}

static sint32 window_scenery_count_rows()
{
	sint32 tabIndex = gWindowSceneryActiveTabIndex;
	sint32 totalItems = 0;

	while (window_scenery_tab_entries[tabIndex][totalItems] != -1){
		totalItems++;
	}

	sint32 rows = count_rows(totalItems + 8);
	return rows;
}

static sint32 window_scenery_rows_height(sint32 rows)
{
	return rows * SCENERY_BUTTON_HEIGHT;
}

static sint32 rows_on_page(sint32 height)
{
	return height / 90;
}

/**
 *
 *  rct2: 0x006BD94C
 */
static void window_scenery_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_SCENERY_CLOSE:
		window_close(w);
		break;
	case WIDX_SCENERY_ROTATE_OBJECTS_BUTTON:
		gWindowSceneryRotation++;
		gWindowSceneryRotation = gWindowSceneryRotation % 4;
		scenery_remove_ghost_tool_placement();
		window_invalidate(w);
		break;
	case WIDX_SCENERY_REPAINT_SCENERY_BUTTON:
		gWindowSceneryPaintEnabled ^= 1;
		gWindowSceneryClusterEnabled = 0;
		gWindowSceneryEyedropperEnabled = false;
		window_invalidate(w);
		break;
	case WIDX_SCENERY_EYEDROPPER_BUTTON:
		gWindowSceneryPaintEnabled = 0;
		gWindowSceneryClusterEnabled = 0;
		gWindowSceneryEyedropperEnabled = !gWindowSceneryEyedropperEnabled;
		window_invalidate(w);
		break;
	case WIDX_SCENERY_BUILD_CLUSTER_BUTTON:
		gWindowSceneryPaintEnabled = 0;
		gWindowSceneryEyedropperEnabled = false;
		if (gWindowSceneryClusterEnabled == 1) {
			gWindowSceneryClusterEnabled = 0;
		}
		else if (network_get_mode() != NETWORK_MODE_CLIENT || network_can_perform_command(network_get_current_player_group_index(), -2)) {
			gWindowSceneryClusterEnabled ^= 1;
		}
		else {
			window_error_open(STR_CANT_DO_THIS, STR_PERMISSION_DENIED);
		}
		window_invalidate(w);
		break;
	}
}

/**
 *
 *  rct2: 0x006E1EB4
 */
void window_scenery_update_scroll(rct_window *w)
{
	sint32 tabIndex = gWindowSceneryActiveTabIndex;
	sint32 listHeight = w->height - 14 - window_scenery_widgets[WIDX_SCENERY_LIST].top - 1;

	scenery_item sceneryItem = window_scenery_count_rows_with_selected_item(tabIndex);
	w->scrolls[0].v_bottom = window_scenery_rows_height(sceneryItem.allRows) + 1;

	sint32 maxTop = max(0, w->scrolls[0].v_bottom - listHeight);
	sint32 rowSelected = count_rows(sceneryItem.selected_item);
	if (sceneryItem.sceneryId == -1) {
		rowSelected = 0;
		sint16 sceneryId = window_scenery_tab_entries[tabIndex][0];
		if (sceneryId != -1)
			gWindowSceneryTabSelections[tabIndex] = sceneryId;
	}

	w->scrolls[0].v_top = window_scenery_rows_height(rowSelected);
	w->scrolls[0].v_top = min(maxTop, w->scrolls[0].v_top);

	widget_scroll_update_thumbs(w, WIDX_SCENERY_LIST);
}


/**
 *
 *  rct2: 0x006E1E48
 */
static void window_scenery_resize(rct_window *w)
{
	if (w->width < w->min_width) {
		window_invalidate(w);
		w->width = w->min_width;
		window_invalidate(w);
	}

	if (w->width > w->max_width) {
		window_invalidate(w);
		w->width = w->max_width;
		window_invalidate(w);
	}

	if (w->height < w->min_height) {
		window_invalidate(w);
		w->height = w->min_height;
		window_invalidate(w);
		// HACK: For some reason invalidate has not been called
		window_event_invalidate_call(w);
		window_scenery_update_scroll(w);
	}

	if (w->height > w->max_height) {
		window_invalidate(w);
		w->height = w->max_height;
		window_invalidate(w);
		// HACK: For some reason invalidate has not been called
		window_event_invalidate_call(w);
		window_scenery_update_scroll(w);
	}
}

/**
 *
 *  rct2: 0x006E1A25
 */
static void window_scenery_mousedown(rct_widgetindex widgetIndex, rct_window* w, rct_widget* widget) {
	switch (widgetIndex) {
	case WIDX_SCENERY_PRIMARY_COLOUR_BUTTON:
		window_dropdown_show_colour(w, widget, w->colours[1], gWindowSceneryPrimaryColour);
		break;
	case WIDX_SCENERY_SECONDARY_COLOUR_BUTTON:
		window_dropdown_show_colour(w, widget, w->colours[1], gWindowScenerySecondaryColour);
		break;
	case WIDX_SCENERY_TERTIARY_COLOUR_BUTTON:
		window_dropdown_show_colour(w, widget, w->colours[1], gWindowSceneryTertiaryColour);
		break;
	}

	if (widgetIndex >= WIDX_SCENERY_TAB_1 && widgetIndex <= WIDX_SCENERY_TAB_20) {
		gWindowSceneryActiveTabIndex = widgetIndex - WIDX_SCENERY_TAB_1;
		window_invalidate(w);
		gSceneryPlaceCost = MONEY32_UNDEFINED;

		//HACK: for 3210 Ensures that window_scenery_update_scroll gets called one time
		w->max_height = 60;
	}
}

/**
 *
 *  rct2: 0x006E1A54
 */
static void window_scenery_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex)
{
	if (dropdownIndex == -1)
		return;

	if (widgetIndex == WIDX_SCENERY_PRIMARY_COLOUR_BUTTON) {
		gWindowSceneryPrimaryColour = (uint8)dropdownIndex;
	}
	else if (widgetIndex == WIDX_SCENERY_SECONDARY_COLOUR_BUTTON) {
		gWindowScenerySecondaryColour = (uint8)dropdownIndex;
	}
	else if (widgetIndex == WIDX_SCENERY_TERTIARY_COLOUR_BUTTON) {
		gWindowSceneryTertiaryColour = (uint8)dropdownIndex;
	}

	window_invalidate(w);
}

/**
 *
 *  rct2: 0x006E1B9F
 */
static void window_scenery_event_07(rct_window *w)
{
	if (w->scenery.selected_scenery_id != -1) {
		w->scenery.selected_scenery_id = -1;
	}
}

/**
 *
 *  rct2: 0x006E1CD3
 */
static void window_scenery_update(rct_window *w)
{
	const CursorState * state = context_get_cursor_state();
	rct_window *other = window_find_from_point(state->x, state->y);
	if (other == w) {
		sint32 window_x = state->x - w->x + 26;
		sint32 window_y = state->y - w->y;

		if (window_y < 44 || window_x <= w->width) {
			rct_widgetindex widgetIndex = window_find_widget_from_point(w, state->x, state->y);
			if (widgetIndex >= WIDX_SCENERY_TAB_CONTENT_PANEL) {
				w->scenery.hover_counter++;
				if (w->scenery.hover_counter < 8) {
					if (input_get_state() != INPUT_STATE_SCROLL_LEFT) {
						w->min_width = WINDOW_SCENERY_WIDTH;
						w->max_width = WINDOW_SCENERY_WIDTH;
						w->min_height = WINDOW_SCENERY_HEIGHT;
						w->max_height = WINDOW_SCENERY_HEIGHT;
					}
				} else {
					sint32 windowHeight = min(463, w->scrolls[0].v_bottom + 62);
					if (gScreenHeight < 600)
						windowHeight = min(374, windowHeight);
					windowHeight = max(WINDOW_SCENERY_HEIGHT, windowHeight);

					w->min_width = WINDOW_SCENERY_WIDTH;
					w->max_width = WINDOW_SCENERY_WIDTH;
					w->min_height = windowHeight;
					w->max_height = windowHeight;
				}
			}
		}
	} else {
		w->scenery.hover_counter = 0;
		if (input_get_state() != INPUT_STATE_SCROLL_LEFT) {
			w->min_width = WINDOW_SCENERY_WIDTH;
			w->max_width = WINDOW_SCENERY_WIDTH;
			w->min_height = WINDOW_SCENERY_HEIGHT;
			w->max_height = WINDOW_SCENERY_HEIGHT;
		}
	}

	window_invalidate(w);

	if (!scenery_tool_is_active()){
		window_close(w);
		return;
	}

	if (gWindowSceneryEyedropperEnabled) {
		gCurrentToolId = TOOL_PICKER;
	} else if (gWindowSceneryPaintEnabled == 1) { // the repaint scenery tool is active
		gCurrentToolId = TOOL_PAINT_DOWN;
	} else {
		uint16 tabIndex = gWindowSceneryActiveTabIndex;
		sint16 tabSelectedSceneryId = gWindowSceneryTabSelections[tabIndex];

		if (tabSelectedSceneryId != -1) {
			if (tabSelectedSceneryId >= 0x400) { // banner
				gCurrentToolId = TOOL_ENTRANCE_DOWN;
			} else if (tabSelectedSceneryId >= 0x300) { // large scenery
				gCurrentToolId = get_large_scenery_entry(tabSelectedSceneryId - 0x300)->large_scenery.tool_id;
			} else if (tabSelectedSceneryId >= 0x200) { // wall
				gCurrentToolId = get_wall_entry(tabSelectedSceneryId - 0x200)->wall.tool_id;
			} else if (tabSelectedSceneryId >= 0x100) { // path bit
				gCurrentToolId = get_footpath_item_entry(tabSelectedSceneryId - 0x100)->path_bit.tool_id;
			} else { // small scenery
				gCurrentToolId = get_small_scenery_entry(tabSelectedSceneryId)->small_scenery.tool_id;
			}
		}
	}
}

/**
 *
 *  rct2: 0x006E1A91
 */
void window_scenery_scrollgetsize(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height)
{
	sint32 rows = window_scenery_count_rows();
	*height = window_scenery_rows_height(rows);
}

static sint16 get_scenery_id_by_cursor_pos(sint16 x, sint16 y)
{
	sint32 tabSceneryIndex = x / SCENERY_BUTTON_WIDTH + (y / SCENERY_BUTTON_HEIGHT) * 9;
	uint8 tabIndex = gWindowSceneryActiveTabIndex;

	sint32 itemCounter = 0;
	sint16 sceneryId = 0;
	while (itemCounter <= tabSceneryIndex) {
		sceneryId = window_scenery_tab_entries[tabIndex][itemCounter];
		if (sceneryId == -1)
			return -1;

		itemCounter++;
	}

	return sceneryId;
}

/**
 *
 *  rct2: 0x006E1C4A
 */
void window_scenery_scrollmousedown(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y)
{
	sint16 sceneryId = get_scenery_id_by_cursor_pos(x, y);
	if (sceneryId == -1)
		return;

	uint8 tabIndex = gWindowSceneryActiveTabIndex;
	gWindowSceneryTabSelections[tabIndex] = sceneryId;

	gWindowSceneryPaintEnabled &= 0xFE;
	gWindowSceneryEyedropperEnabled = false;
	audio_play_sound_panned(4, (w->width >> 1) + w->x, 0, 0, 0);
	w->scenery.hover_counter = -16;
	gSceneryPlaceCost = MONEY32_UNDEFINED;
	window_invalidate(w);
}

/**
 *
 *  rct2: 0x006E1BB8
 */
void window_scenery_scrollmouseover(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y)
{
	sint16 sceneryId = get_scenery_id_by_cursor_pos(x, y);
	if (sceneryId != -1) {
		w->scenery.selected_scenery_id = sceneryId;
		window_invalidate(w);
	}
}

/**
 *
 *  rct2: 0x006E1C05
 */
void window_scenery_tooltip(rct_window* w, rct_widgetindex widgetIndex, rct_string_id *stringId)
{
	switch (widgetIndex) {
	case WIDX_SCENERY_LIST:
		set_format_arg(0, uint16, STR_LIST);
		break;
	case WIDX_SCENERY_TAB_1:
	case WIDX_SCENERY_TAB_2:
	case WIDX_SCENERY_TAB_3:
	case WIDX_SCENERY_TAB_4:
	case WIDX_SCENERY_TAB_5:
	case WIDX_SCENERY_TAB_6:
	case WIDX_SCENERY_TAB_7:
	case WIDX_SCENERY_TAB_8:
	case WIDX_SCENERY_TAB_9:
	case WIDX_SCENERY_TAB_10:
	case WIDX_SCENERY_TAB_11:
	case WIDX_SCENERY_TAB_12:
	case WIDX_SCENERY_TAB_13:
	case WIDX_SCENERY_TAB_14:
	case WIDX_SCENERY_TAB_15:
	case WIDX_SCENERY_TAB_16:
	case WIDX_SCENERY_TAB_17:
	case WIDX_SCENERY_TAB_18:
	case WIDX_SCENERY_TAB_19:
		set_format_arg(0, rct_string_id, get_scenery_group_entry(widgetIndex - WIDX_SCENERY_TAB_1)->name);
		break;
	case WIDX_SCENERY_TAB_20:
		set_format_arg(0, rct_string_id, STR_MISCELLANEOUS);
		break;
	}
}

/**
 *
 *  rct2: 0x006E118B
 */
void window_scenery_invalidate(rct_window *w)
{
	uint16 tabIndex = gWindowSceneryActiveTabIndex;
	uint32 titleStringId = STR_MISCELLANEOUS;
	if (tabIndex < 19)
		titleStringId = get_scenery_group_entry(tabIndex)->name;

	window_scenery_widgets[WIDX_SCENERY_TITLE].text = titleStringId;

	w->pressed_widgets = 0;
	w->pressed_widgets |= 1ULL << (tabIndex + WIDX_SCENERY_TAB_1);
	if (gWindowSceneryPaintEnabled == 1)
		w->pressed_widgets |= (1 << WIDX_SCENERY_REPAINT_SCENERY_BUTTON);
	if (gWindowSceneryEyedropperEnabled)
		w->pressed_widgets |= (1 << WIDX_SCENERY_EYEDROPPER_BUTTON);
	if (gWindowSceneryClusterEnabled == 1)
		w->pressed_widgets |= (1 << WIDX_SCENERY_BUILD_CLUSTER_BUTTON);

	window_scenery_widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type = WWT_EMPTY;
	window_scenery_widgets[WIDX_SCENERY_EYEDROPPER_BUTTON].type = WWT_EMPTY;
	window_scenery_widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].type = WWT_EMPTY;

	if (!(gWindowSceneryPaintEnabled & 1)) {
		window_scenery_widgets[WIDX_SCENERY_EYEDROPPER_BUTTON].type = WWT_FLATBTN;
	}

	sint16 tabSelectedSceneryId = gWindowSceneryTabSelections[tabIndex];
	if (tabSelectedSceneryId != -1) {
		if (tabSelectedSceneryId < 0x100) {
			if (!(gWindowSceneryPaintEnabled & 1)) {
				window_scenery_widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].type = WWT_FLATBTN;
			}

			rct_scenery_entry* sceneryEntry = get_small_scenery_entry(tabSelectedSceneryId);
			if (sceneryEntry->small_scenery.flags & SMALL_SCENERY_FLAG4) {
				window_scenery_widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type = WWT_FLATBTN;
			}
		}
		else if (tabSelectedSceneryId >= 0x300) {
			window_scenery_widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type = WWT_FLATBTN;
		}
	}

	window_scenery_widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].image =
		(gWindowSceneryPrimaryColour << 19) | 0x60000000 | SPR_PALETTE_BTN;
	window_scenery_widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].image =
		(gWindowScenerySecondaryColour << 19) | 0x60000000 | SPR_PALETTE_BTN;
	window_scenery_widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].image =
		(gWindowSceneryTertiaryColour << 19) | 0x60000000 | SPR_PALETTE_BTN;

	window_scenery_widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WWT_EMPTY;
	window_scenery_widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WWT_EMPTY;
	window_scenery_widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].type = WWT_EMPTY;

	if (gWindowSceneryPaintEnabled & 1) { // repaint coloured scenery tool is on
		window_scenery_widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WWT_COLOURBTN;
		window_scenery_widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WWT_COLOURBTN;
		window_scenery_widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].type = WWT_COLOURBTN;
		window_scenery_widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type = WWT_EMPTY;
	} else if (tabSelectedSceneryId != -1) {
		rct_scenery_entry* sceneryEntry = NULL;

		if (tabSelectedSceneryId >= 0x400) {
			sceneryEntry = get_banner_entry(tabSelectedSceneryId - 0x400);

			if (sceneryEntry->banner.flags & 1)
				window_scenery_widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WWT_COLOURBTN;
		} else if (tabSelectedSceneryId >= 0x300) {
			sceneryEntry = get_large_scenery_entry(tabSelectedSceneryId - 0x300);

			if (sceneryEntry->large_scenery.flags & LARGE_SCENERY_FLAG_HAS_PRIMARY_COLOUR)
				window_scenery_widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WWT_COLOURBTN;
			if (sceneryEntry->large_scenery.flags & LARGE_SCENERY_FLAG_HAS_SECONDARY_COLOUR)
				window_scenery_widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WWT_COLOURBTN;
		} else if (tabSelectedSceneryId >= 0x200) {
			sceneryEntry = get_wall_entry(tabSelectedSceneryId - 0x200);
			if (sceneryEntry->wall.flags & (WALL_SCENERY_HAS_PRIMARY_COLOUR | WALL_SCENERY_HAS_GLASS)) {
				window_scenery_widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WWT_COLOURBTN;

				if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_SECONDARY_COLOUR) {
					window_scenery_widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WWT_COLOURBTN;

					if (sceneryEntry->wall.flags2 & WALL_SCENERY_2_NO_SELECT_PRIMARY_COLOUR)
						window_scenery_widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WWT_EMPTY;
					if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_TERNARY_COLOUR)
						window_scenery_widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].type = WWT_COLOURBTN;
				}
			}
		} else if (tabSelectedSceneryId < 0x100) {
			sceneryEntry = get_small_scenery_entry(tabSelectedSceneryId);

			if (sceneryEntry->small_scenery.flags & (SMALL_SCENERY_FLAG_HAS_PRIMARY_COLOUR | SMALL_SCENERY_FLAG_HAS_GLASS)) {
				window_scenery_widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WWT_COLOURBTN;

				if (sceneryEntry->small_scenery.flags & SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR)
					window_scenery_widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WWT_COLOURBTN;
			}
		}
	}

	window_scenery_widgets[WIDX_SCENERY_BACKGROUND].right = w->width - 1;
	window_scenery_widgets[WIDX_SCENERY_BACKGROUND].bottom = w->height - 1;
	window_scenery_widgets[WIDX_SCENERY_TAB_CONTENT_PANEL].right = w->width - 1;
	window_scenery_widgets[WIDX_SCENERY_TAB_CONTENT_PANEL].bottom = w->height - 1;
	window_scenery_widgets[WIDX_SCENERY_TITLE].right = w->width - 2;
	window_scenery_widgets[WIDX_SCENERY_CLOSE].left = w->width - 13;
	window_scenery_widgets[WIDX_SCENERY_CLOSE].right = window_scenery_widgets[WIDX_SCENERY_CLOSE].left + 10;
	window_scenery_widgets[WIDX_SCENERY_LIST].right = w->width - 26;
	window_scenery_widgets[WIDX_SCENERY_LIST].bottom = w->height - 14;

	window_scenery_widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].left = w->width - 25;
	window_scenery_widgets[WIDX_SCENERY_REPAINT_SCENERY_BUTTON].left = w->width - 25;
	window_scenery_widgets[WIDX_SCENERY_EYEDROPPER_BUTTON].left = w->width - 25;
	window_scenery_widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].left = w->width - 25;
	window_scenery_widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].right = w->width - 2;
	window_scenery_widgets[WIDX_SCENERY_REPAINT_SCENERY_BUTTON].right = w->width - 2;
	window_scenery_widgets[WIDX_SCENERY_EYEDROPPER_BUTTON].right = w->width - 2;
	window_scenery_widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].right = w->width - 2;

	window_scenery_widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].left = w->width - 19;
	window_scenery_widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].left = w->width - 19;
	window_scenery_widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].left = w->width - 19;
	window_scenery_widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].right = w->width - 8;
	window_scenery_widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].right = w->width - 8;
	window_scenery_widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].right = w->width - 8;
}

/**
 *
 *  rct2: 0x006E1462
 */
void window_scenery_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);

	uint16 tabIndex = gWindowSceneryActiveTabIndex;
	uint16 selectedWidgetId = tabIndex + 4;
	uint32 imageId = ((w->colours[1] << 19) | window_scenery_widgets[selectedWidgetId].image) + 1ul;

	gfx_draw_sprite(dpi, imageId,
		w->x + window_scenery_widgets[selectedWidgetId].left,
		w->y + window_scenery_widgets[selectedWidgetId].top,
		selectedWidgetId);

	sint16 selectedSceneryEntryId = w->scenery.selected_scenery_id;
	if (selectedSceneryEntryId == -1) {
		if (gWindowSceneryPaintEnabled & 1)  // repaint coloured scenery tool is on
			return;
		if (gWindowSceneryEyedropperEnabled)
			return;

		selectedSceneryEntryId = gWindowSceneryTabSelections[tabIndex];

		if (selectedSceneryEntryId == -1)
			return;
	}

	uint32 price = 0;

	rct_scenery_entry* sceneryEntry = NULL;
	if (selectedSceneryEntryId >= 0x400) {
		sceneryEntry = get_banner_entry(selectedSceneryEntryId - 0x400);
		price = sceneryEntry->banner.price;
	} else if (selectedSceneryEntryId >= 0x300) {
		sceneryEntry = get_large_scenery_entry(selectedSceneryEntryId - 0x300);
		price = sceneryEntry->large_scenery.price * 10;
	} else if (selectedSceneryEntryId >= 0x200) {
		sceneryEntry = get_wall_entry(selectedSceneryEntryId - 0x200);
		price = sceneryEntry->wall.price;
	} else if (selectedSceneryEntryId >= 0x100) {
		sceneryEntry = get_footpath_item_entry(selectedSceneryEntryId - 0x100);
		price = sceneryEntry->path_bit.price;
	} else {
		sceneryEntry = get_small_scenery_entry(selectedSceneryEntryId);
		price = sceneryEntry->small_scenery.price * 10;
	}

	if (w->scenery.selected_scenery_id == -1 && gSceneryPlaceCost != MONEY32_UNDEFINED) {
		price = gSceneryPlaceCost;
	}

	set_format_arg(0, uint32, price);

	if (!(gParkFlags & PARK_FLAGS_NO_MONEY)) {
		// -14
		gfx_draw_string_right(dpi, STR_COST_LABEL, gCommonFormatArgs, COLOUR_BLACK,
			w->x + w->width - 0x1A, w->y + w->height - 13);
	}

	set_format_arg(0, rct_string_id, sceneryEntry->name);
	gfx_draw_string_left_clipped(dpi, STR_BLACK_STRING, gCommonFormatArgs, COLOUR_BLACK,
		w->x + 3, w->y + w->height - 13, w->width - 19);
}

/**
*
*  rct2: 0x006E15ED
*/
void window_scenery_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex)
{
	gfx_clear(dpi, ColourMapA[w->colours[1]].mid_light);

	uint8 tabIndex = gWindowSceneryActiveTabIndex;

	sint32 sceneryTabItemIndex = 0;
	sint16 currentSceneryGlobalId = -1;
	sint16 left = 0, top = 0;

	while ((currentSceneryGlobalId = window_scenery_tab_entries[tabIndex][sceneryTabItemIndex]) != -1) {
		uint16 tabSelectedSceneryId = gWindowSceneryTabSelections[tabIndex];

		if (gWindowSceneryPaintEnabled == 1 || gWindowSceneryEyedropperEnabled) {
			if (w->scenery.selected_scenery_id == currentSceneryGlobalId) {
				gfx_fill_rect_inset(dpi, left, top, left + SCENERY_BUTTON_WIDTH - 1,
					top + SCENERY_BUTTON_HEIGHT - 1, w->colours[1], INSET_RECT_FLAG_FILL_MID_LIGHT);
			}
		}
		else {
			if (tabSelectedSceneryId == currentSceneryGlobalId) {
				gfx_fill_rect_inset(dpi, left, top, left + SCENERY_BUTTON_WIDTH - 1,
					top + SCENERY_BUTTON_HEIGHT - 1, w->colours[1], (INSET_RECT_FLAG_BORDER_INSET | INSET_RECT_FLAG_FILL_MID_LIGHT));
			}
			else if (w->scenery.selected_scenery_id == currentSceneryGlobalId) {
				gfx_fill_rect_inset(dpi, left, top, left + SCENERY_BUTTON_WIDTH - 1,
					top + SCENERY_BUTTON_HEIGHT - 1, w->colours[1], INSET_RECT_FLAG_FILL_MID_LIGHT);
			}
		}

		rct_scenery_entry* sceneryEntry;
		rct_drawpixelinfo clipdpi;
		if (clip_drawpixelinfo(&clipdpi, dpi, left + 1, top + 1, SCENERY_BUTTON_WIDTH - 2, SCENERY_BUTTON_HEIGHT - 2)) {
			if (currentSceneryGlobalId >= 0x400) {
				sceneryEntry = get_banner_entry(currentSceneryGlobalId - 0x400);
				uint32 imageId = sceneryEntry->image + gWindowSceneryRotation * 2;
				imageId |= (gWindowSceneryPrimaryColour << 19) | 0x20000000;

				gfx_draw_sprite(&clipdpi, imageId, 0x21, 0x28, w->colours[1]);
				gfx_draw_sprite(&clipdpi, imageId + 1, 0x21, 0x28, w->colours[1]);
			}
			else if (currentSceneryGlobalId >= 0x300) {
				sceneryEntry = get_large_scenery_entry(currentSceneryGlobalId - 0x300);
				uint32 imageId = sceneryEntry->image + gWindowSceneryRotation;
				imageId |= (gWindowSceneryPrimaryColour << 19) | 0x20000000;
				imageId |= (gWindowScenerySecondaryColour << 24) | 0x80000000;

				gfx_draw_sprite(&clipdpi, imageId, 0x21, 0, w->colours[1]);
			}
			else if (currentSceneryGlobalId >= 0x200) {
				sceneryEntry = get_wall_entry(currentSceneryGlobalId - 0x200);
				uint32 imageId = sceneryEntry->image;
				uint8 tertiaryColour = w->colours[1];
				uint16 spriteTop = (sceneryEntry->wall.height * 2) + 0x32;

				if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_GLASS) {
					imageId |= (gWindowSceneryPrimaryColour << 19) | 0x20000000;

					if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_SECONDARY_COLOUR) {
						imageId |= (gWindowScenerySecondaryColour << 24) | 0x80000000;
					}
					gfx_draw_sprite(&clipdpi, imageId, 0x2F, spriteTop, tertiaryColour);

					imageId = (sceneryEntry->image + 0x40000006) | (GlassPaletteIds[gWindowSceneryPrimaryColour] << 19);
					gfx_draw_sprite(&clipdpi, imageId, 0x2F, spriteTop, tertiaryColour);
				}
				else {
					imageId |= (gWindowSceneryPrimaryColour << 19) | 0x20000000;

					if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_SECONDARY_COLOUR) {
						imageId |= (gWindowScenerySecondaryColour << 24) | 0x80000000;

						if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_TERNARY_COLOUR) {
							imageId &= 0xDFFFFFFF;
							tertiaryColour = gWindowSceneryTertiaryColour;
						}
					}
					gfx_draw_sprite(&clipdpi, imageId, 0x2F, spriteTop, tertiaryColour);

					if (sceneryEntry->wall.flags & WALL_SCENERY_IS_DOOR) {
						gfx_draw_sprite(&clipdpi, imageId + 1, 0x2F, spriteTop, tertiaryColour);
					}
				}
			}
			else if (currentSceneryGlobalId >= 0x100) {
				sceneryEntry = get_footpath_item_entry(currentSceneryGlobalId - 0x100);
				uint32 imageId = sceneryEntry->image;

				gfx_draw_sprite(&clipdpi, imageId, 0x0B, 0x10, w->colours[1]);
			}
			else {
				sceneryEntry = get_small_scenery_entry(currentSceneryGlobalId);
				uint32 imageId = sceneryEntry->image + gWindowSceneryRotation;

				if (sceneryEntry->small_scenery.flags & SMALL_SCENERY_FLAG_HAS_PRIMARY_COLOUR) {
					imageId |= (gWindowSceneryPrimaryColour << 19) | 0x20000000;

					if (sceneryEntry->small_scenery.flags & SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR) {
						imageId |= (gWindowScenerySecondaryColour << 24) | 0x80000000;
					}
				}

				uint16 spriteTop = (sceneryEntry->small_scenery.height / 4) + 0x2B;

				if (sceneryEntry->small_scenery.flags & SMALL_SCENERY_FLAG_FULL_TILE &&
					sceneryEntry->small_scenery.flags & SMALL_SCENERY_FLAG_VOFFSET_CENTRE) {
					spriteTop -= 0x0C;
				}

				gfx_draw_sprite(&clipdpi, imageId, 0x20, spriteTop, w->colours[1]);

				if (sceneryEntry->small_scenery.flags & SMALL_SCENERY_FLAG_HAS_GLASS) {
					imageId = ((sceneryEntry->image + gWindowSceneryRotation) + 0x40000004) +
						(GlassPaletteIds[gWindowSceneryPrimaryColour] << 19);

					gfx_draw_sprite(&clipdpi, imageId, 0x20, spriteTop, w->colours[1]);
				}

				if (sceneryEntry->small_scenery.flags & SMALL_SCENERY_FLAG_ANIMATED_FG) {
					imageId = (sceneryEntry->image + gWindowSceneryRotation) + 4;
					gfx_draw_sprite(&clipdpi, imageId, 0x20, spriteTop, w->colours[1]);
				}
			}
		}

		left += SCENERY_BUTTON_WIDTH;
		if (left >= 594) {
			top += SCENERY_BUTTON_HEIGHT;
			left = 0;
		}
		sceneryTabItemIndex++;
	}
}

static sint32 window_scenery_find_tab_with_scenery_id(sint32 sceneryId)
{
	for (sint32 i = 0; i < 20; i++) {
		for (sint32 j = 0; j < SCENERY_ENTRIES_BY_TAB; j++) {
			sint16 entry = window_scenery_tab_entries[i][j];
			if (entry == -1) break;
			if (entry == sceneryId) return i;
		}
	}
	return -1;
}

bool window_scenery_set_selected_item(sint32 sceneryId)
{
	bool result = false;
	rct_window * w = window_bring_to_front_by_class(WC_SCENERY);
	if (w != NULL) {
		sint32 tabIndex = window_scenery_find_tab_with_scenery_id(sceneryId);
		if (tabIndex != -1) {
			gWindowSceneryActiveTabIndex = tabIndex;
			gWindowSceneryTabSelections[tabIndex] = sceneryId;

			audio_play_sound_panned(4, (w->width >> 1) + w->x, 0, 0, 0);
			w->scenery.hover_counter = -16;
			gSceneryPlaceCost = MONEY32_UNDEFINED;
			window_invalidate(w);
			result = true;
		}
	}
	return result;
}
