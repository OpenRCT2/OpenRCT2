/*****************************************************************************
* Copyright (c) 2014 Dániel Tar
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
#include "../drawing/drawing.h"
#include "../game.h"
#include "../input.h"
#include "../interface/viewport.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../peep/peep.h"
#include "../world/map.h"
#include "../world/scenery.h"
#include "../world/sprite.h"
#include "dropdown.h"
#include "scenery.h"

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

static void window_scenery_emptysub() { }
static void window_scenery_close();
static void window_scenery_mouseup();
static void window_scenery_resize();
static void window_scenery_mousedown(int widgetIndex, rct_window* w, rct_widget* widget);
static void window_scenery_dropdown();
static void window_scenery_update(rct_window *w);
static void window_scenery_event_07();
static void window_scenery_scrollgetsize();
static void window_scenery_scrollmousedown();
static void window_scenery_scrollmouseover();
static void window_scenery_tooltip();
static void window_scenery_invalidate();
static void window_scenery_paint();
static void window_scenery_scrollpaint();

static void* window_scenery_events[] = {
	window_scenery_close,
	window_scenery_mouseup,
	window_scenery_resize,
	window_scenery_mousedown,
	window_scenery_dropdown,
	window_scenery_emptysub,
	window_scenery_update,
	window_scenery_event_07,
	window_scenery_emptysub,
	window_scenery_emptysub,
	window_scenery_emptysub,
	window_scenery_emptysub,
	window_scenery_emptysub,
	window_scenery_emptysub,
	window_scenery_emptysub,
	window_scenery_scrollgetsize,
	window_scenery_scrollmousedown,
	window_scenery_emptysub,
	window_scenery_scrollmouseover,
	window_scenery_emptysub,
	window_scenery_emptysub,
	window_scenery_emptysub,
	window_scenery_tooltip,
	window_scenery_emptysub,
	window_scenery_emptysub,
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
	WIDX_SCENERY_BUILD_CLUSTER_BUTTON,		// 40000000
};

static rct_widget window_scenery_widgets[] = {
	{ WWT_FRAME, 0, 0, 633, 0, 141, 0xFFFFFFFF, STR_NONE },								// 1				0x009DE298
	{ WWT_CAPTION, 0, 1, 632, 1, 14, 0xFFFFFFFF, STR_WINDOW_TITLE_TIP },				// 2				0x009DE2A8
	{ WWT_CLOSEBOX, 0, 621, 631, 2, 13, STR_CLOSE_X, STR_CLOSE_WINDOW_TIP },			// 4				0x009DE2B8
	{ WWT_RESIZE, 1, 0, 633, 43, 141, 0xFFFFFFFF, STR_NONE },							// 8				0x009DE2C8
	{ WWT_TAB, 1, 3, 33, 17, 43, 0xFFFFFFFF, 1812 },									// 10				0x009DE2D8
	{ WWT_TAB, 1, 34, 64, 17, 43, 0xFFFFFFFF, 1812 },									// 20				0x009DE2E8
	{ WWT_TAB, 1, 65, 95, 17, 43, 0xFFFFFFFF, 1812 },									// 40				0x009DE2F8
	{ WWT_TAB, 1, 96, 126, 17, 43, 0xFFFFFFFF, 1812 },									// 80				0x009DE308
	{ WWT_TAB, 1, 127, 157, 17, 43, 0xFFFFFFFF, 1812 },									// 100				0x009DE318
	{ WWT_TAB, 1, 158, 188, 17, 43, 0xFFFFFFFF, 1812 },									// 200				0x009DE328
	{ WWT_TAB, 1, 189, 219, 17, 43, 0xFFFFFFFF, 1812 },									// 400				0x009DE338
	{ WWT_TAB, 1, 220, 250, 17, 43, 0xFFFFFFFF, 1812 },									// 800				0x009DE348
	{ WWT_TAB, 1, 251, 281, 17, 43, 0xFFFFFFFF, 1812 },									// 1000				0x009DE358
	{ WWT_TAB, 1, 282, 312, 17, 43, 0xFFFFFFFF, 1812 },									// 2000				0x009DE368
	{ WWT_TAB, 1, 313, 343, 17, 43, 0xFFFFFFFF, 1812 },									// 4000				0x009DE378
	{ WWT_TAB, 1, 344, 374, 17, 43, 0xFFFFFFFF, 1812 },									// 8000				0x009DE388
	{ WWT_TAB, 1, 375, 405, 17, 43, 0xFFFFFFFF, 1812 },									// 10000			0x009DE398
	{ WWT_TAB, 1, 406, 436, 17, 43, 0xFFFFFFFF, 1812 },									// 20000			0x009DE3A8
	{ WWT_TAB, 1, 437, 467, 17, 43, 0xFFFFFFFF, 1812 },									// 40000			0x009DE3B8
	{ WWT_TAB, 1, 468, 498, 17, 43, 0xFFFFFFFF, 1812 },									// 80000			0x009DE3C8
	{ WWT_TAB, 1, 468, 498, 17, 43, 0xFFFFFFFF, 1812 },									// 100000			0x009DE3D8
	{ WWT_TAB, 1, 468, 498, 17, 43, 0xFFFFFFFF, 1812 },									// 200000			0x009DE3E8
	{ WWT_TAB, 1, 468, 498, 17, 43, 0xFFFFFFFF, 1812 },									// 400000			0x009DE3F8
	{ WWT_TAB, 1, 468, 498, 17, 43, 0x20001598, 1812 },									// 800000			0x009DE408
	{ WWT_SCROLL, 1, 2, 608, 47, 126, 2, STR_NONE },									// 1000000			0x009DE418
	{ WWT_FLATBTN, 1, 609, 632, 44, 67, 5169, STR_ROTATE_OBJECTS_90 },					// 2000000			0x009DE428
	{ WWT_FLATBTN, 1, 609, 632, 68, 91, 5173, 3102 },									// 4000000			0x009DE438
	{ WWT_COLORBTN, 1, 615, 626, 93, 104, 0xFFFFFFFF, STR_SELECT_COLOUR },				// 8000000			0x009DE448
	{ WWT_COLORBTN, 1, 615, 626, 105, 116, 0xFFFFFFFF, STR_SELECT_SECONDARY_COLOUR },	// 10000000			0x009DE458
	{ WWT_COLORBTN, 1, 615, 626, 117, 128, 0xFFFFFFFF, STR_SELECT_TERNARY_COLOUR },		// 20000000			0x009DE468
	{ WWT_FLATBTN, 1, 609, 632, 117, 140, 5172, 3225 },									// 40000000			0x009DE478
	{ WIDGETS_END },
};

static sint16 window_scenery_tab_entries[0x14][SCENERY_ENTRIES_BY_TAB + 1];

/**
 * Was part of 0x006DFA00
 * The same code repeated five times for every scenery entry type
 */
void init_scenery_entry(rct_scenery_entry *sceneryEntry, int index, uint8 sceneryTabId) {
	if (RCT2_ADDRESS(0x01357BD0, sint32)[index >> 5] & (1 << (index & 0x1F))) {
		if (sceneryTabId != 0xFF) {
			for (int i = 0; i < SCENERY_ENTRIES_BY_TAB; i++) {
				if (window_scenery_tab_entries[sceneryTabId][i] == -1)
				{
					window_scenery_tab_entries[sceneryTabId][i] = index;
					window_scenery_tab_entries[sceneryTabId][i + 1] = -1;
					return;
				}
			}
		}

		for (int i = 0; i < 0x13; i++) {
			int counter = 0;

			while (window_scenery_tab_entries[i][counter] != -1)
			{
				if (window_scenery_tab_entries[i][counter] == index) {
					return;
				}

				counter++;
			}
		}

		for (int i = 0; i < SCENERY_ENTRIES_BY_TAB; i++) {
			if (window_scenery_tab_entries[0x13][i] == -1)
			{
				window_scenery_tab_entries[0x13][i] = index;
				window_scenery_tab_entries[0x13][i + 1] = -1;
				break;
			}
		}
	}
}

/**
 *
 *  rct2: 0x006DFA00
 */
void init_scenery() {
	bool enabledScenerySets[0x14] = { false };

	for (int scenerySetIndex = 0; scenerySetIndex < 0x14; scenerySetIndex++) {
		window_scenery_tab_entries[scenerySetIndex][0] = -1;
		if (scenerySetIndex == 0x13)
			continue;

		rct_scenery_set_entry* scenerySetEntry = g_scenerySetEntries[scenerySetIndex];
		if ((uint32)scenerySetEntry == 0xFFFFFFFF)
			continue;

		int sceneryTabEntryCount = 0;

		for (int i = 0; i < scenerySetEntry->entry_count; i++) {
			uint16 sceneryEntryId = scenerySetEntry->scenery_entries[i];
			uint32 ecx = RCT2_ADDRESS(0x01357BD0, uint32)[sceneryEntryId >> 5];
			uint32 edx = 1 << (sceneryEntryId & 0x1F);
			if (ecx & edx) {
				window_scenery_tab_entries[scenerySetIndex][sceneryTabEntryCount] = sceneryEntryId;
				window_scenery_tab_entries[scenerySetIndex][++sceneryTabEntryCount] = -1;
			} else {
				enabledScenerySets[scenerySetIndex] = true;
			}
		}
	}

	// small scenery 
	for (uint16 sceneryId = 0; sceneryId < 0xFC; sceneryId++) {
		if ((uint32)g_smallSceneryEntries[sceneryId] == 0xFFFFFFFF)
			continue;

		rct_scenery_entry* sceneryEntry = g_smallSceneryEntries[sceneryId];
		init_scenery_entry(sceneryEntry, sceneryId, sceneryEntry->small_scenery.scenery_tab_id);
	}

	// large scenery
	for (int sceneryId = 0x300; sceneryId < 0x380; sceneryId++) {
		int largeSceneryIndex = sceneryId - 0x300;

		if ((uint32)g_largeSceneryEntries[largeSceneryIndex] == 0xFFFFFFFF)
			continue;

		rct_scenery_entry* sceneryEntry = g_largeSceneryEntries[largeSceneryIndex];
		init_scenery_entry(sceneryEntry, sceneryId, sceneryEntry->large_scenery.scenery_tab_id);
	}

	// walls
	for (int sceneryId = 0x200; sceneryId < 0x280; sceneryId++) {
		int wallSceneryIndex = sceneryId - 0x200;

		if ((uint32)g_wallSceneryEntries[wallSceneryIndex] == 0xFFFFFFFF)
			continue;

		rct_scenery_entry* sceneryEntry = g_wallSceneryEntries[wallSceneryIndex];
		init_scenery_entry(sceneryEntry, sceneryId, sceneryEntry->wall.scenery_tab_id);
	}

	// banners
	for (int sceneryId = 0x400; sceneryId < 0x420; sceneryId++) {
		int bannerIndex = sceneryId - 0x400;

		if ((uint32)g_bannerSceneryEntries[bannerIndex] == 0xFFFFFFFF)
			continue;

		rct_scenery_entry* sceneryEntry = g_bannerSceneryEntries[bannerIndex];
		init_scenery_entry(sceneryEntry, sceneryId, sceneryEntry->banner.scenery_tab_id);
	}

	// path bits
	for (int sceneryId = 0x100; sceneryId < 0x10F; sceneryId++) {
		int pathBitIndex = sceneryId - 0x100;

		if ((uint32)g_pathBitSceneryEntries[pathBitIndex] == 0xFFFFFFFF)
			continue;

		rct_scenery_entry* sceneryEntry = g_pathBitSceneryEntries[pathBitIndex];
		init_scenery_entry(sceneryEntry, sceneryId, sceneryEntry->path_bit.scenery_tab_id);
	}

	for (int widgetIndex = WIDX_SCENERY_TAB_1; widgetIndex < WIDX_SCENERY_LIST; widgetIndex++)
		window_scenery_widgets[widgetIndex].type = 0;

	uint8 tabIndexes[0x13];
	uint8 order[0x13];
	int usedValues = 0;

	for (int scenerySetId = 0; scenerySetId < 0x13; scenerySetId++) {
		rct_scenery_set_entry* sceneryEntry = g_scenerySetEntries[scenerySetId];
		if ((uint32)sceneryEntry == 0xFFFFFFFF)
			continue;

		tabIndexes[usedValues] = scenerySetId;
		order[usedValues] = sceneryEntry->var_108;

		usedValues++;
	}

	while (true) {
		bool finished = true;
		for (int i = 1; i < usedValues; i++) {
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

	tabIndexes[usedValues] = 0x13;
	usedValues++;

	uint16 left = 3;
	for (int i = 0; i < usedValues; i ++) {
		uint32 tabIndex = tabIndexes[i];
		rct_widget* tabWidget = &window_scenery_widgets[tabIndex + WIDX_SCENERY_TAB_1];

		if (left != 3 || tabIndex != 0x13) {
			if (window_scenery_tab_entries[tabIndex][0] == -1)
				continue;

			if (enabledScenerySets[tabIndex])
				continue;
		}

		tabWidget->type = WWT_TAB;
		tabWidget->left = left;
		tabWidget->right = left + 0x1E;
		left += 0x1F;

		if (tabIndex >= 0x13)
			continue;

		tabWidget->image = g_scenerySetEntries[tabIndex]->image | 0x20000000;
	}

	window_invalidate_by_class(WC_SCENERY);
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

	window = window_create(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16) - WINDOW_SCENERY_WIDTH, 0x1D, WINDOW_SCENERY_WIDTH, WINDOW_SCENERY_HEIGHT,
		(uint32*)window_scenery_events, WC_SCENERY, WF_2);
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
		(1 << WIDX_SCENERY_BUILD_CLUSTER_BUTTON);

	window_init_scroll_widgets(window);
	RCT2_CALLPROC_X(0x006E1EB4, 0, 0, 0, 0, (int)window, 0, 0);
	show_gridlines();
	window_scenery_rotation = 3;
	RCT2_GLOBAL(0x00F64F12, uint8) = 0;
	RCT2_GLOBAL(0x00F64F13, uint8) = 0;
	window->scenery.selected_scenery_id = -1;
	window->scenery.hover_counter = 0;
	window_push_others_below(window);
	RCT2_GLOBAL(0x00F64F0D, uint8) = 0;
	RCT2_GLOBAL(0x00F64EB4, uint32) = 0x80000000;
	RCT2_GLOBAL(0x00F64EC0, uint16) = 0;
	window_scenery_is_repaint_scenery_tool_on = 0; // repaint colored scenery tool state
	window_scenery_is_build_cluster_tool_on = 0; // build cluster tool state

	window->min_width = WINDOW_SCENERY_WIDTH;
	window->max_width = WINDOW_SCENERY_WIDTH;
	window->min_height = WINDOW_SCENERY_HEIGHT;
	window->max_height = WINDOW_SCENERY_HEIGHT;
	window->colours[0] = 0x18;
	window->colours[1] = 0x0C;
	window->colours[2] = 0x0C;
}

/**
 *
 *  rct2: 0x0066DB3D
 */
bool window_scenery_is_scenery_tool_active() {
	int toolWindowClassification = RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass);
	int toolWidgetIndex = RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, rct_windownumber);

	if (RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) & INPUT_FLAG_TOOL_ACTIVE)
		if (toolWindowClassification == WC_TOP_TOOLBAR && toolWidgetIndex == 9) // 9 is WIDX_SCENERY
			return true;

	return false;
}


/**
 *
 *  rct2: 0x006E1A73
 */
void window_scenery_close() {
	rct_window *w;

	window_get_register(w);

	RCT2_CALLPROC_EBPSAFE(0x006E2712);
	hide_gridlines();
	RCT2_CALLPROC_X(0x006CB70A, 0, 0, 0, 0, 0, 0, 0);

	if (window_scenery_is_scenery_tool_active())
		tool_cancel();
}

int window_scenery_scrollgetsize_num()
{
	int items = 0;
	while (window_scenery_tab_entries[window_scenery_active_tab_index][items] != -1)
		items++;

	items += 8;
	int rows = items / 9;
	if (rows == 0)
		rows++;

	return rows * SCENERY_BUTTON_HEIGHT;
}

/**
 *
 *  rct2: 0x006BD94C
 */
static void window_scenery_mouseup()
{
	short widgetIndex;
	rct_window *w;

	window_widget_get_registers(w, widgetIndex);

	switch (widgetIndex) {
	case WIDX_SCENERY_CLOSE:
		window_close(w);
		break;
	case WIDX_SCENERY_ROTATE_OBJECTS_BUTTON:
		window_scenery_rotation++;
		window_scenery_rotation = window_scenery_rotation % 4;
		RCT2_CALLPROC_EBPSAFE(0x006E2712);
		window_invalidate(w);
		break;
	case WIDX_SCENERY_REPAINT_SCENERY_BUTTON:
		window_scenery_is_repaint_scenery_tool_on ^= 1;
		window_invalidate(w);
		break;
	case WIDX_SCENERY_BUILD_CLUSTER_BUTTON:
		window_scenery_is_build_cluster_tool_on ^= 1;
		window_invalidate(w);
		break;
	}
}

/*
 *
 *  rct2: 0x006E1EB4
 */
void window_scenery_update_scroll(rct_window *w)
{
	w->scrolls[0].v_bottom = window_scenery_scrollgetsize_num() + 1;

	int tabIndex = window_scenery_active_tab_index;

	int itemIndex = 0;
	sint16 sceneryId;
	while ((sceneryId = window_scenery_tab_entries[tabIndex][itemIndex]) != -1) {
		if (sceneryId == window_scenery_selected_scenery_by_tab[tabIndex])
			break;
		itemIndex++;
	}

	if (sceneryId == -1) {
		itemIndex = 0;
		sint16 sceneryId = window_scenery_tab_entries[tabIndex][itemIndex];
		if (sceneryId != -1)
			window_scenery_selected_scenery_by_tab[tabIndex] = sceneryId;
	}

	w->scrolls[0].v_top = (itemIndex / 9) * SCENERY_BUTTON_HEIGHT;
	widget_scroll_update_thumbs(w, WIDX_SCENERY_LIST);
}

/**
 *
 *  rct2: 0x006E1E48
 */
static void window_scenery_resize()
{
	rct_window *w;

	window_get_register(w);
		
	if (w->width < w->min_width) {
		w->width = w->min_width;
		window_invalidate(w);
	}

	if (w->width > w->max_width) {
		w->width = w->max_width;
		window_invalidate(w);
	}

	if (w->height < w->min_height) {
		w->height = w->min_height;
		window_invalidate(w);
		window_scenery_update_scroll(w);
	}

	if (w->height > w->max_height) {
		w->height = w->max_height;
		window_invalidate(w);
		window_scenery_update_scroll(w);
	}
}

/**
 *
 *  rct2: 0x006E1A25
 */
static void window_scenery_mousedown(int widgetIndex, rct_window* w, rct_widget* widget) {
	switch (widgetIndex) {
	case WIDX_SCENERY_PRIMARY_COLOUR_BUTTON:
		window_dropdown_show_colour(w, widget, w->colours[1], window_scenery_primary_colour);
		break;
	case WIDX_SCENERY_SECONDARY_COLOUR_BUTTON:
		window_dropdown_show_colour(w, widget, w->colours[1], window_scenery_secondary_colour);
		break;
	case WIDX_SCENERY_TERTIARY_COLOUR_BUTTON:
		window_dropdown_show_colour(w, widget, w->colours[1], window_scenery_tertiary_colour);
		break;
	}

	if (widgetIndex >= WIDX_SCENERY_TAB_1 && widgetIndex <= WIDX_SCENERY_TAB_20) {
		window_scenery_active_tab_index = widgetIndex - WIDX_SCENERY_TAB_1;
		window_invalidate(w);
		RCT2_GLOBAL(0x00F64EB4, uint32) = 0x80000000;
		window_scenery_update_scroll(w);
	}
}

/**
 *
 *  rct2: 0x006E1A54
 */
static void window_scenery_dropdown() {
	rct_window* w;
	short widgetIndex, dropdownIndex;
	window_dropdown_get_registers(w, widgetIndex, dropdownIndex);

	if (dropdownIndex == -1)
		return;

	if (widgetIndex == WIDX_SCENERY_PRIMARY_COLOUR_BUTTON) {
		window_scenery_primary_colour = (uint8)dropdownIndex;
	}
	else if (widgetIndex == WIDX_SCENERY_SECONDARY_COLOUR_BUTTON) {
		window_scenery_secondary_colour = (uint8)dropdownIndex;
	}
	else if (widgetIndex == WIDX_SCENERY_TERTIARY_COLOUR_BUTTON) {
		window_scenery_tertiary_colour = (uint8)dropdownIndex;
	}

	window_invalidate(w);
}

/**
 *
 *  rct2: 0x006E1B9F
 */
static void window_scenery_event_07() {
	rct_window *w;

	window_get_register(w);

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
	rct_window *other = window_find_from_point(RCT2_GLOBAL(0x0142406C, int), RCT2_GLOBAL(0x01424070, int));
	if (other == w) {
		int window_x = RCT2_GLOBAL(0x0142406C, int) - w->x + 26;
		int window_y = RCT2_GLOBAL(0x01424070, int) - w->y;

		if (window_y < 44 || window_x <= w->width) {
			int widgetIndex = window_find_widget_from_point(w, RCT2_GLOBAL(0x0142406C, int), RCT2_GLOBAL(0x01424070, int));
			if (widgetIndex >= WIDX_SCENERY_TAB_CONTENT_PANEL) {
				w->scenery.hover_counter++;
				if (w->scenery.hover_counter < 8) {
					if (RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, sint8) != INPUT_STATE_SCROLL_LEFT) {
						w->min_width = WINDOW_SCENERY_WIDTH;
						w->max_width = WINDOW_SCENERY_WIDTH;
						w->min_height = WINDOW_SCENERY_HEIGHT;
						w->max_height = WINDOW_SCENERY_HEIGHT;
					}
				} else {
					int windowHeight = min(454, w->scrolls[0].v_bottom - 1 + 62);
					if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16) < 600)
						windowHeight = min(374, windowHeight);

					w->min_width = WINDOW_SCENERY_WIDTH;
					w->max_width = WINDOW_SCENERY_WIDTH;
					w->min_height = windowHeight;
					w->max_height = windowHeight;
				}
			}
		}
	} else {
		w->scenery.hover_counter = 0;
		if (RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, sint8) != INPUT_STATE_SCROLL_LEFT) {
			w->min_width = WINDOW_SCENERY_WIDTH;
			w->max_width = WINDOW_SCENERY_WIDTH;
			w->min_height = WINDOW_SCENERY_HEIGHT;
			w->max_height = WINDOW_SCENERY_HEIGHT;
		}
	}

	gfx_invalidate_screen();
	
	if (!window_scenery_is_scenery_tool_active()){
		window_close(w);
		return;
	}

	if (window_scenery_is_repaint_scenery_tool_on == 1) { // the repaint scenery tool is active
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TOOL, uint8) = 0x17;
	} else {
		uint16 tabIndex = window_scenery_active_tab_index;
		sint16 tabSelectedSceneryId = window_scenery_selected_scenery_by_tab[tabIndex];

		if (tabSelectedSceneryId != -1) {
			if (tabSelectedSceneryId >= 0x400) { // banner
				RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TOOL, uint8) = 0x18;
			} else if (tabSelectedSceneryId >= 0x300) { // large scenery
				RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TOOL, uint8) =
					g_largeSceneryEntries[tabSelectedSceneryId - 0x300]->large_scenery.tool_id;
			} else if (tabSelectedSceneryId >= 0x200) { // wall
				RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TOOL, uint8) =
					g_wallSceneryEntries[tabSelectedSceneryId - 0x200]->wall.tool_id;
			} else if (tabSelectedSceneryId >= 0x100) { // path bit
				RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TOOL, uint8) =
					g_pathBitSceneryEntries[tabSelectedSceneryId - 0x100]->path_bit.tool_id;
			} else { // small scenery
				RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TOOL, uint8) =
					g_smallSceneryEntries[tabSelectedSceneryId]->small_scenery.tool_id;
			}
		}
	}
}

/**
 *
 *  rct2: 0x006E1A91
 */
void window_scenery_scrollgetsize()
{
	int width, height;

	width = 0;
	height = window_scenery_scrollgetsize_num();

	window_scrollsize_set_registers(width, height);
}

short get_scenery_id_by_cursor_pos(short x, short y)
{
	int tabSceneryIndex = x / SCENERY_BUTTON_WIDTH + (y / SCENERY_BUTTON_HEIGHT) * 9;
	uint8 tabIndex = window_scenery_active_tab_index;

	int itemCounter = 0;
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
void window_scenery_scrollmousedown()
{
	short x, y, scrollIndex;
	rct_window *w;

	window_scrollmouse_get_registers(w, scrollIndex, x, y);

	short sceneryId = get_scenery_id_by_cursor_pos(x, y);
	if (sceneryId == -1)
		return;

	uint8 tabIndex = window_scenery_active_tab_index;
	window_scenery_selected_scenery_by_tab[tabIndex] = sceneryId;

	window_scenery_is_repaint_scenery_tool_on &= 0xFE;
	sound_play_panned(4, (w->width >> 1) + w->x, 0, 0, 0);
	w->scenery.hover_counter = -16;
	RCT2_GLOBAL(0x00F64EB4, uint32) = 0x80000000;
	window_invalidate(w);
}

/**
 *
 *  rct2: 0x006E1BB8
 */
void window_scenery_scrollmouseover()
{
	short x, y, scrollIndex;
	rct_window *w;

	window_scrollmouse_get_registers(w, scrollIndex, x, y);
	short sceneryId = get_scenery_id_by_cursor_pos(x, y);
	if (sceneryId != -1) {
		w->scenery.selected_scenery_id = sceneryId;
		window_invalidate(w);
	}
}

/**
 *
 *  rct2: 0x006E1C05
 */
void window_scenery_tooltip()
{
	rct_window *w;
	short widget;

	window_scroll_get_registers(w, widget);

	switch (widget) {
	case WIDX_SCENERY_LIST:
		RCT2_GLOBAL(0x013CE952, uint16) = 3159;
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
	case WIDX_SCENERY_TAB_20:
		RCT2_GLOBAL(0x013CE952, uint16) = g_scenerySetEntries[widget - WIDX_SCENERY_TAB_1]->name;
		break;
	}
}

/**
 *
 *  rct2: 0x006E118B
 */
void window_scenery_invalidate()
{
	rct_window* w;

	window_get_register(w);

	uint16 tabIndex = window_scenery_active_tab_index;
	uint32 titleStringId = 1813;
	if (tabIndex <= 19)
		titleStringId = g_scenerySetEntries[tabIndex]->name;
	
	window_scenery_widgets[WIDX_SCENERY_TITLE].image = titleStringId;

	w->pressed_widgets = (((uint32)w->pressed_widgets & 0xFF00000F) | (1 << (tabIndex + 4))) & 0xBBFFFFFF;

	if (window_scenery_is_repaint_scenery_tool_on == 1)
		w->pressed_widgets |= (1 << WIDX_SCENERY_REPAINT_SCENERY_BUTTON);

	if (window_scenery_is_build_cluster_tool_on == 1)
		w->pressed_widgets |= (1 << WIDX_SCENERY_BUILD_CLUSTER_BUTTON);
		
	window_scenery_widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type = WWT_EMPTY;
	window_scenery_widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].type = WWT_EMPTY;
	
	sint16 tabSelectedSceneryId = window_scenery_selected_scenery_by_tab[tabIndex];
	if (tabSelectedSceneryId != -1) {
		if (tabSelectedSceneryId < 0x100) {
			if (!(window_scenery_is_repaint_scenery_tool_on & 1))
				window_scenery_widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].type = WWT_FLATBTN;

			rct_scenery_entry* sceneryEntry = g_smallSceneryEntries[tabSelectedSceneryId];
			if (sceneryEntry->small_scenery.flags & SMALL_SCENERY_FLAG4) {
				window_scenery_widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type = WWT_FLATBTN;
			}
		}
		else if (tabSelectedSceneryId > 0x300) {
			window_scenery_widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type = WWT_FLATBTN;
		}
	}

	window_scenery_widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].image =
		(window_scenery_primary_colour << 19) + 0x600013C3;
	window_scenery_widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].image =
		(window_scenery_secondary_colour << 19) + 0x600013C3;
	window_scenery_widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].image =
		(window_scenery_tertiary_colour << 19) + 0x600013C3;

	window_scenery_widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WWT_EMPTY;
	window_scenery_widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WWT_EMPTY;
	window_scenery_widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].type = WWT_EMPTY;
	
	if (window_scenery_is_repaint_scenery_tool_on & 1) { // repaint colored scenery tool is on
		window_scenery_widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WWT_COLORBTN;
		window_scenery_widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WWT_COLORBTN;
		window_scenery_widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].type = WWT_COLORBTN;
		window_scenery_widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type = WWT_EMPTY;
	} else if (tabSelectedSceneryId != -1) {
		rct_scenery_entry* sceneryEntry = NULL;

		if (tabSelectedSceneryId >= 0x400) {
			sceneryEntry = g_bannerSceneryEntries[tabSelectedSceneryId - 0x400];
			
			if (sceneryEntry->banner.flags & 1)
				window_scenery_widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WWT_COLORBTN;
		} else if (tabSelectedSceneryId >= 0x300) {
			sceneryEntry = g_largeSceneryEntries[tabSelectedSceneryId - 0x300];

			if (sceneryEntry->large_scenery.flags & 1)
				window_scenery_widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WWT_COLORBTN;
			if (sceneryEntry->large_scenery.flags & 2)
				window_scenery_widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WWT_COLORBTN;
		} else if (tabSelectedSceneryId >= 0x200) {
			sceneryEntry = g_wallSceneryEntries[tabSelectedSceneryId - 0x200];
			if (sceneryEntry->wall.flags & (WALL_SCENERY_FLAG1 | WALL_SCENERY_FLAG2)) {
				window_scenery_widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WWT_COLORBTN;

				if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_SECONDARY_COLOUR) {
					window_scenery_widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WWT_COLORBTN;

					if (sceneryEntry->wall.flags2 & WALL_SCENERY_FLAG1)
						window_scenery_widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WWT_EMPTY;
					if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_TERNARY_COLOUR)
						window_scenery_widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].type = WWT_COLORBTN;
				}
			}
		} else if (tabSelectedSceneryId < 0x100) {
			sceneryEntry = g_smallSceneryEntries[tabSelectedSceneryId];

			if (sceneryEntry->small_scenery.flags & (SMALL_SCENERY_HAS_PRIMARY_COLOUR | SMALL_SCENERY_FLAG10)) {
				window_scenery_widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WWT_COLORBTN;

				if (sceneryEntry->small_scenery.flags & SMALL_SCENERY_HAS_SECONDARY_COLOUR)
					window_scenery_widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WWT_COLORBTN;
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
	window_scenery_widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].left = w->width - 25;
	window_scenery_widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].right = w->width - 2;
	window_scenery_widgets[WIDX_SCENERY_REPAINT_SCENERY_BUTTON].right = w->width - 2;
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
void window_scenery_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);

	uint16 tabIndex = window_scenery_active_tab_index;
	uint16 selectedWidgetId = tabIndex + 4;
	uint32 imageId = ((w->colours[1] << 19) | window_scenery_widgets[selectedWidgetId].image) + 1ul;
	
	gfx_draw_sprite(dpi, imageId,
		w->x + window_scenery_widgets[selectedWidgetId].left,
		w->y + window_scenery_widgets[selectedWidgetId].top,
		selectedWidgetId);
	
	sint16 selectedSceneryEntryId = w->scenery.selected_scenery_id;
	if (selectedSceneryEntryId == -1) {
		if (window_scenery_is_repaint_scenery_tool_on & 1)  // repaint colored scenery tool is on
			return;

		selectedSceneryEntryId = window_scenery_selected_scenery_by_tab[tabIndex];

		if (selectedSceneryEntryId == -1)
			return;
	}

	uint32 price = 0;

	rct_scenery_entry* sceneryEntry = NULL;
	if (selectedSceneryEntryId >= 0x400) {
		sceneryEntry = g_bannerSceneryEntries[selectedSceneryEntryId - 0x400];
		price = sceneryEntry->banner.price;
	} else if (selectedSceneryEntryId >= 0x300) {
		sceneryEntry = g_largeSceneryEntries[selectedSceneryEntryId - 0x300];
		price = sceneryEntry->large_scenery.price * 10;
	} else if (selectedSceneryEntryId >= 0x200) {
		sceneryEntry = g_wallSceneryEntries[selectedSceneryEntryId - 0x200];
		price = sceneryEntry->wall.price;
	} else if (selectedSceneryEntryId >= 0x100) {
		sceneryEntry = g_pathBitSceneryEntries[selectedSceneryEntryId - 0x100];
		price = sceneryEntry->path_bit.price;
	} else {
		sceneryEntry = g_smallSceneryEntries[selectedSceneryEntryId];
		price = sceneryEntry->small_scenery.price * 10;
	}

	if (w->scenery.selected_scenery_id == -1 && RCT2_GLOBAL(0x00F64EB4, uint32) != 0x80000000) {
		price = RCT2_GLOBAL(0x00F64EB4, uint32);
	}

	RCT2_GLOBAL(0x013CE952, uint32) = price;

	if (!(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY)) {
		// -14
		gfx_draw_string_right(dpi, STR_COST_LABEL, (void*)0x013CE952, 0,
			w->x + w->width - 0x1A, w->y + w->height - 13);
	}

	RCT2_GLOBAL(0x013CE952, uint16) = sceneryEntry->name;
	gfx_draw_string_left_clipped(dpi, 0x4A7, (void*)0x013CE952, 0,
		w->x + 3, w->y + w->height - 13, w->width - 19);
}

/**
*
*  rct2: 0x006E15ED
*/
void window_scenery_scrollpaint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	gfx_clear(dpi, ((char*)0x0141FC48)[w->colours[1] * 8] * 0x1010101);

	uint8 tabIndex = window_scenery_active_tab_index;

	int sceneryTabItemIndex = 0;
	sint16 currentSceneryGlobalId = -1;
	sint16 left = 0, top = 0;
	
	while ((currentSceneryGlobalId = window_scenery_tab_entries[tabIndex][sceneryTabItemIndex]) != -1) {
		uint16 tabSelectedSceneryId = window_scenery_selected_scenery_by_tab[tabIndex];
				
		if (window_scenery_is_repaint_scenery_tool_on == 1)
		{
			if (w->scenery.selected_scenery_id == currentSceneryGlobalId) {
				gfx_fill_rect_inset(dpi, left, top, left + SCENERY_BUTTON_WIDTH - 1,
					top + SCENERY_BUTTON_HEIGHT - 1, w->colours[1], 0x80);
			}
		} else {
			if (tabSelectedSceneryId == currentSceneryGlobalId) {
				gfx_fill_rect_inset(dpi, left, top, left + SCENERY_BUTTON_WIDTH - 1,
					top + SCENERY_BUTTON_HEIGHT - 1, w->colours[1], 0xA0);
			}
			else if (w->scenery.selected_scenery_id == currentSceneryGlobalId) {
				gfx_fill_rect_inset(dpi, left, top, left + SCENERY_BUTTON_WIDTH - 1,
					top + SCENERY_BUTTON_HEIGHT - 1, w->colours[1], 0x80);
			}
		}

		rct_scenery_entry* sceneryEntry;
		if (currentSceneryGlobalId >= 0x400) {
			sceneryEntry = g_bannerSceneryEntries[currentSceneryGlobalId - 0x400];
			uint32 imageId = sceneryEntry->image + window_scenery_rotation * 2;
			imageId |= (window_scenery_primary_colour << 19) | 0x20000000;

			gfx_draw_sprite(dpi, imageId, left + 0x21, top + 0x28, w->colours[1]);
			gfx_draw_sprite(dpi, imageId + 1, left + 0x21, top + 0x28, w->colours[1]);
		} else if (currentSceneryGlobalId >= 0x300) {
			sceneryEntry = g_largeSceneryEntries[currentSceneryGlobalId - 0x300];
			uint32 imageId = sceneryEntry->image + window_scenery_rotation;
			imageId |= (window_scenery_primary_colour << 19) | 0x20000000;
			imageId |= (window_scenery_secondary_colour << 24) | 0x80000000;

			gfx_draw_sprite(dpi, imageId, left + 0x21, top, w->colours[1]);
		} else if (currentSceneryGlobalId >= 0x200) {
			sceneryEntry = g_wallSceneryEntries[currentSceneryGlobalId - 0x200];
			rct_drawpixelinfo* clipdpi = clip_drawpixelinfo(dpi, left + 1, 64, top + 1, 78);
			if (clipdpi != NULL) {
				uint32 imageId = sceneryEntry->image;
				uint8 tertiaryColour = w->colours[1];

				if (sceneryEntry->wall.flags & WALL_SCENERY_FLAG2) {
					imageId |= (window_scenery_primary_colour << 19) | 0x20000000;

					if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_SECONDARY_COLOUR) {
						imageId |= (window_scenery_secondary_colour << 24) | 0x80000000;
					}
					gfx_draw_sprite(clipdpi, imageId, 0x2F, (sceneryEntry->wall.height * 2) + 0x32,
						tertiaryColour);

					imageId = (sceneryEntry->image + 0x40000006) | ((window_scenery_primary_colour + 0x70) << 19);
					gfx_draw_sprite(clipdpi, imageId, 0x2F, (sceneryEntry->wall.height * 2) + 0x32,
						tertiaryColour);
				}
				else {
					imageId |= (window_scenery_primary_colour << 19) | 0x20000000;

					if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_SECONDARY_COLOUR) {
						imageId |= (window_scenery_secondary_colour << 24) | 0x80000000;

						if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_TERNARY_COLOUR) {
							imageId &= 0xDFFFFFFF;
							tertiaryColour = window_scenery_tertiary_colour;
						}
							
					}				
					gfx_draw_sprite(clipdpi, imageId, 0x2F, (sceneryEntry->wall.height * 2) + 0x32,
					tertiaryColour);

					if (sceneryEntry->wall.flags & WALL_SCENERY_FLAG5){
						gfx_draw_sprite(clipdpi, imageId + 1, 0x2F, (sceneryEntry->wall.height * 2) + 0x32,
							tertiaryColour);
					}
				}


				rct2_free(clipdpi);
			}
		} else if (currentSceneryGlobalId >= 0x100) {
			sceneryEntry = g_pathBitSceneryEntries[currentSceneryGlobalId - 0x100];
			uint32 imageId = sceneryEntry->image;

			gfx_draw_sprite(dpi, imageId, left + 0x0B, top + 0x10, w->colours[1]);
		} else {
			sceneryEntry = g_smallSceneryEntries[currentSceneryGlobalId];
			rct_drawpixelinfo* clipdpi = clip_drawpixelinfo(dpi, left + 1, SCENERY_BUTTON_WIDTH - 2, top + 1, SCENERY_BUTTON_HEIGHT - 2);
			if (clipdpi != NULL) {

				uint32 imageId = sceneryEntry->image + window_scenery_rotation;

				if (sceneryEntry->small_scenery.flags & SMALL_SCENERY_HAS_PRIMARY_COLOUR) {
					imageId |= (window_scenery_primary_colour << 19) | 0x20000000;

					if (sceneryEntry->small_scenery.flags & SMALL_SCENERY_HAS_SECONDARY_COLOUR) {
						imageId |= (window_scenery_secondary_colour << 24) | 0x80000000;
					}
				}

				uint16 spriteTop = (sceneryEntry->small_scenery.height / 4) + 0x2B;

				if (sceneryEntry->small_scenery.flags & SMALL_SCENERY_FLAG1 &&
					sceneryEntry->small_scenery.flags &  SMALL_SCENERY_FLAG2) {
					spriteTop -= 0x0C;
				}

				gfx_draw_sprite(clipdpi, imageId, 0x20, spriteTop, w->colours[1]);

				if (sceneryEntry->small_scenery.flags & 0x200) {
					imageId = ((sceneryEntry->image + window_scenery_rotation) + 0x40000004) +
						((window_scenery_primary_colour + 0x70) << 19);

					gfx_draw_sprite(clipdpi, imageId, 0x20, spriteTop, w->colours[1]);
				}

				if (sceneryEntry->small_scenery.flags & SMALL_SCENERY_FLAG8) {
					imageId = (sceneryEntry->image + window_scenery_rotation) + 4;
					gfx_draw_sprite(clipdpi, imageId, 0x20, spriteTop, w->colours[1]);
				}

				rct2_free(clipdpi);
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