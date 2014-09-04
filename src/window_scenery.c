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

#include <string.h>
#include <stdbool.h>
#include "addresses.h"
#include "audio.h"
#include "game.h"
#include "map.h"
#include "gfx.h"
#include "peep.h"
#include "sprite.h"
#include "scenery.h"
#include "string_ids.h"
#include "viewport.h"
#include "widget.h"
#include "window.h"

#define WINDOW_SCENERY_WIDTH 0x27A
#define WINDOW_SCENERY_HEIGHT 0x8E

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
} WINDOW_SCENERY_LIST_TAB;

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


void init_scenery_entry(rct_scenery_entry *sceneryEntry, int index, uint8 sceneryTabId) {
	if (RCT2_ADDRESS(0x01357BD0, sint32)[index >> 5] & (1 << (index & 0x1F))) {
		if (sceneryTabId != 0xFF) {
			uint32 sceneryTabItems = RCT2_ADDRESS(0x00F64F2C, uint32)[sceneryTabId];

			for (int ebx = 0; ebx < 0x80; ebx++) {
				if (RCT2_ADDRESS(sceneryTabItems, uint16)[ebx] == 0xFFFF)
				{
					RCT2_ADDRESS(sceneryTabItems, uint16)[ebx] = index;
					RCT2_ADDRESS(sceneryTabItems, uint16)[ebx + 1] = 0xFFFF;
					return;
				}
			}
		}

		for (int ecx = 0; ecx < 0x13; ecx++) {
			int sceneryTabItems = RCT2_ADDRESS(0x00F64F2C, uint32)[ecx];
			int counter = 0;

			while (RCT2_ADDRESS(sceneryTabItems, uint16)[counter] != 0xFFFF)
			{
				if (RCT2_ADDRESS(sceneryTabItems, uint16)[counter] == index) {
					ecx = 0x13;
					return;
				}

				counter++;
			}
		}

		uint32 edx = RCT2_GLOBAL(0x00F64F78, uint32);
		for (int ecx = 0; ecx < 0x80; ecx++) {
			if (RCT2_ADDRESS(edx, uint16)[ecx] == 0xFFFF)
			{
				RCT2_ADDRESS(edx, uint16)[ecx] = index;
				RCT2_ADDRESS(edx, uint16)[ecx + 1] = 0xFFFF;
				break;
			}
		}
	}
}

/*
* rct2: 0x006DFA00
**/
void init_scenery() {
	uint32 edi = 0;
	uint32 baseAddress = 0x00F64F7C;

	for (int i = 0; i < 0x14; i++) {
		RCT2_ADDRESS(0x00F64F2C, uint32)[i] = baseAddress;
		baseAddress += 0x102;
	}

	uint32 dword_F663A4 = 0;

	for (int edi = 0; edi < 0x14; edi++) {
		int sceneryTabItems = RCT2_ADDRESS(0x00F64F2C, uint32)[edi];
		RCT2_GLOBAL(sceneryTabItems, uint16) = 0xFFFF;
		if (edi == 13)
			continue;

		uint32 ebp = RCT2_ADDRESS(RCT2_ADDRESS_SCENERY_SET_ENTRIES, uint32)[edi];
		if (ebp == 0xFFFFFFFF)
			continue;

		for (int ebx = 0;; ebx++) {
			if (ebx >= RCT2_GLOBAL(ebp + 0x106, uint8))
				break;

			uint16 ax = RCT2_GLOBAL(ebp + ebx * 2 + 6, uint16);

			uint32 ecx = RCT2_ADDRESS(0x01357BD0, uint32)[ax >> 5];
			uint32 edx = 1 << (ax & 0x1F);
			if (ecx & edx) {
				RCT2_GLOBAL(sceneryTabItems, uint16) = ax;
				sceneryTabItems += 2;
				RCT2_GLOBAL(sceneryTabItems, uint16) = 0xFFFF;
			} else {
				dword_F663A4 |= (1 << edi);
			}
		}
	}

	// small scenery 
	for (uint16 edi = 0; edi < 0xFC; edi++) {
		if ((uint32)g_smallSceneryEntries[edi] == 0xFFFFFFFF)
			continue;

		rct_scenery_entry* sceneryEntry = g_smallSceneryEntries[edi];
		init_scenery_entry(sceneryEntry, edi, sceneryEntry->small_scenery.scenery_tab_id);
	}

	// large scenery
	for (int edi = 0x300; edi < 0x380; edi++) {
		int largeSceneryIndex = edi - 0x300;

		if ((uint32)g_largeSceneryEntries[largeSceneryIndex] == 0xFFFFFFFF)
			continue;

		rct_scenery_entry* sceneryEntry = g_largeSceneryEntries[largeSceneryIndex];
		init_scenery_entry(sceneryEntry, edi, sceneryEntry->large_scenery.scenery_tab_id);
	}

	// walls
	for (int edi = 0x200; edi < 0x280; edi++) {
		int wallSceneryIndex = edi - 0x200;

		if ((uint32)g_wallSceneryEntries[wallSceneryIndex] == 0xFFFFFFFF)
			continue;

		rct_scenery_entry* sceneryEntry = g_wallSceneryEntries[wallSceneryIndex];
		init_scenery_entry(sceneryEntry, edi, sceneryEntry->wall.scenery_tab_id);
	}

	// banners
	for (int edi = 0x400; edi < 0x420; edi++) {
		int bannerIndex = edi - 0x400;

		if ((uint32)g_bannerSceneryEntries[bannerIndex] == 0xFFFFFFFF)
			continue;

		rct_scenery_entry* sceneryEntry = g_bannerSceneryEntries[bannerIndex];
		init_scenery_entry(sceneryEntry, edi, sceneryEntry->banner.scenery_tab_id);
	}

	// path bits
	for (int edi = 0x100; edi < 0x10F; edi++) {
		int bannerIndex = edi - 0x100;

		if ((uint32)g_pathBitSceneryEntries[bannerIndex] == 0xFFFFFFFF)
			continue;

		rct_scenery_entry* sceneryEntry = g_pathBitSceneryEntries[bannerIndex];
		init_scenery_entry(sceneryEntry, edi, sceneryEntry->path_bit.scenery_tab_id);
	}

	for (int widgetIndex = WIDX_SCENERY_TAB_1; widgetIndex < WIDX_SCENERY_LIST; widgetIndex++)
		window_scenery_widgets[widgetIndex].type = 0;

	uint8 stuff[0x50];
	int ebp = 0;

	for (int ebx = 0; ebx < 0x13; ebx++) {
		uint32 scenerySetAddress = RCT2_ADDRESS(RCT2_ADDRESS_SCENERY_SET_ENTRIES, uint32)[edi];
		if (scenerySetAddress == 0xFFFFFFFF)
			continue;

		stuff[ebp] = ebx;
		stuff[ebp + 1] = RCT2_GLOBAL(scenerySetAddress + 0x108, uint8);

		ebp += 4;
		int prev_edi = ebp - 4;

		while (true) {
			if (prev_edi <= 0)
				break;

			prev_edi -= 4;
			if (stuff[prev_edi + 5] >= stuff[prev_edi + 1])
				break;

			int eax = stuff[prev_edi + 4];
			stuff[prev_edi + 4] = stuff[prev_edi];
			stuff[prev_edi] = eax;
		}
	}

	stuff[ebp] = 0x13;
	ebp += 4;

	uint16 cx = 3;
	for (int ebx = 0; ebx < ebp; ebx += 4) {
		uint32 tabIndex = stuff[ebx];
		rct_widget* currentTab = &window_scenery_widgets[tabIndex + WIDX_SCENERY_TAB_1];
		int sceneryTabItems = RCT2_ADDRESS(0x00F64F2C, uint32)[tabIndex];

		if (cx != 3 || tabIndex == 0x13) {
			if (RCT2_GLOBAL(sceneryTabItems, uint16) == 0xFFFF)
				continue;

			if (dword_F663A4 & (1 << tabIndex))
				continue;
		}

		currentTab->type = WWT_TAB;
		currentTab->left = cx;
		currentTab->right = cx + 0x1E;
		cx += 0x1F;

		if (edi >= 0x13)
			continue;

		int sceneryEntry = RCT2_ADDRESS(RCT2_ADDRESS_SCENERY_SET_ENTRIES, uint32)[tabIndex];
		currentTab->image = RCT2_GLOBAL(sceneryEntry + 2, uint32) | 0x20000000;
	}

	window_invalidate_by_id(WC_SCENERY, 0);
}

/*
* rct2: 0x006E0FEF
**/
void window_scenery_open()
{
	rct_window* window;

	// Check if window is already open
	window = window_bring_to_front_by_id(WC_SCENERY, 0);
	if (window != NULL)
		return;

	init_scenery();
	//RCT2_CALLPROC_EBPSAFE(0x006DFA00);

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
	RCT2_GLOBAL(0x00F64F05, uint8) = 3; // rotation
	RCT2_GLOBAL(0x00F64F12, uint8) = 0;
	RCT2_GLOBAL(0x00F64F13, uint8) = 0;
	window->scenery.selected_scenery_id = -1;
	window->scenery.var_482 = 0;
	window_push_others_below(window);
	RCT2_GLOBAL(0x00F64F0D, uint8) = 0;
	RCT2_GLOBAL(0x00F64EB4, uint32) = 0x80000000;
	RCT2_GLOBAL(0x00F64EC0, uint16) = 0;
	RCT2_GLOBAL(0x00F64F19, uint8) = 0; // repaint colored scenery tool state
	RCT2_GLOBAL(0x00F64F1A, uint8) = 0;

	window->min_width = WINDOW_SCENERY_WIDTH;
	window->max_width = WINDOW_SCENERY_WIDTH;
	window->min_height = WINDOW_SCENERY_HEIGHT;
	window->max_height = WINDOW_SCENERY_HEIGHT;
	window->colours[0] = 0x18;
	window->colours[1] = 0x0C;
	window->colours[2] = 0x0C;
}

/*
 * rct2: 0x0066DB3D
*/
bool window_scenery_is_tool_active() {
	int toolWindowClassification = RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass);
	int toolWidgetIndex = RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, rct_windownumber);

	if (RCT2_GLOBAL(0x009DE518, uint32) & (1 << 3))
		if (toolWindowClassification == WC_TOP_TOOLBAR && toolWidgetIndex == 9)
			return true;

	return false;
}


/*
* rct2: 0x006E1A73
**/
void window_scenery_close() {
	rct_window *w;

	window_get_register(w);

	RCT2_CALLPROC_EBPSAFE(0x006E2712);
	hide_gridlines();
	RCT2_CALLPROC_X(0x006CB70A, 0, 0, 0, 0, 0, 0, 0);

	if (window_scenery_is_tool_active())
		tool_cancel();
}

int window_scenery_scrollgetsize_num() {
	uint32 sceneryTabItems = RCT2_ADDRESS(0x00F64F2C, uint32)[RCT2_GLOBAL(0x00F64EDC, uint8)];

	int items = 0;
	while (RCT2_ADDRESS(sceneryTabItems, sint16)[items] != -1)
		items++;

	items += 8;
	int rows = items / 9;
	if (rows == 0)
		rows++;

	return rows * 80;
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
		RCT2_GLOBAL(0x00F64F05, uint8)++;
		RCT2_GLOBAL(0x00F64F05, uint8) &= 3;
		RCT2_CALLPROC_EBPSAFE(0x006E2712);
		window_invalidate(w);
		break;
	case WIDX_SCENERY_REPAINT_SCENERY_BUTTON:
		RCT2_GLOBAL(0x00F64F19, uint8) ^= 1;
		window_invalidate(w);
		break;
	case WIDX_SCENERY_BUILD_CLUSTER_BUTTON:
		RCT2_GLOBAL(0x00F64F1A, uint8) ^= 1;
		window_invalidate(w);
		break;
	}
}

/*
*
*  rct2: 0x006E1EB4
*/
void window_scenery_update_scroll(rct_window *w) {
	int scrollsize = window_scenery_scrollgetsize_num();
	w->scrolls[0].v_bottom = scrollsize;

	int typeId = RCT2_GLOBAL(0x00F64EDC, uint8);
	uint32 sceneryTabItems = RCT2_ADDRESS(0x00F64F2C, uint32)[typeId];

	int itemIndex = 0;
	sint16 sceneryId;
	while ((sceneryId = RCT2_ADDRESS(sceneryTabItems, sint16)[itemIndex]) != -1) {
		if (sceneryId == RCT2_ADDRESS(0x00F64EDD, uint16)[typeId])
			break;
		itemIndex++;
	}

	if (sceneryId == -1) {
		itemIndex = 0;
		sint16 sceneryId = RCT2_ADDRESS(sceneryTabItems, sint16)[itemIndex];
		if (sceneryId != -1)
			RCT2_ADDRESS(0x00F64EDD, uint16)[typeId] = sceneryId;
	}

	w->scrolls[0].v_top = (itemIndex / 9) * 80;
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
	int eax;

	switch (widgetIndex) {
	case WIDX_SCENERY_PRIMARY_COLOUR_BUTTON:
		eax = (RCT2_GLOBAL(0xF64F06, uint8) << 8) + 0x80 + w->colours[1];
		RCT2_CALLPROC_X(0x006ED43D, eax, 0, 0, widgetIndex, (int)w, (int)widget, 0xFFFFFFFF);
		break;
	case WIDX_SCENERY_SECONDARY_COLOUR_BUTTON:
		eax = (RCT2_GLOBAL(0xF64F07, uint8) << 8) + 0x80 + w->colours[1];
		RCT2_CALLPROC_X(0x006ED43D, eax, 0, 0, widgetIndex, (int)w, (int)widget, 0xFFFFFFFF);
		break;
	case WIDX_SCENERY_TERTIARY_COLOUR_BUTTON:
		eax = (RCT2_GLOBAL(0xF64F08, uint8) << 8) + 0x80 + w->colours[1];
		RCT2_CALLPROC_X(0x006ED43D, eax, 0, 0, widgetIndex, (int)w, (int)widget, 0xFFFFFFFF);
		break;
	}

	if (widgetIndex >= WIDX_SCENERY_TAB_1 && widgetIndex <= WIDX_SCENERY_TAB_20) {
		RCT2_GLOBAL(0x00F64EDC, uint8) = widgetIndex - WIDX_SCENERY_TAB_1;
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
		RCT2_GLOBAL(0x00F64F06, uint8) = dropdownIndex;
	}
	else if (widgetIndex == WIDX_SCENERY_SECONDARY_COLOUR_BUTTON) {
		RCT2_GLOBAL(0x00F64F07, uint8) = dropdownIndex;
	}
	else if (widgetIndex == WIDX_SCENERY_TERTIARY_COLOUR_BUTTON) {
		RCT2_GLOBAL(0x00F64F08, uint8) = dropdownIndex;
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
	rct_window* other = window_find_from_point(RCT2_GLOBAL(0x0142406C, int), RCT2_GLOBAL(0x01424070, int));
	if (other == w) {
		int window_x = RCT2_GLOBAL(0x0142406C, int) - w->x + 0x1A;
		int window_y = RCT2_GLOBAL(0x01424070, int) - w->y;

		if (window_y < 0x2C && window_x <= w->width) {
			int widgetIndex = window_find_widget_from_point(w,
				RCT2_GLOBAL(0x0142406C, int), RCT2_GLOBAL(0x01424070, int));

			if (widgetIndex >= 3) {
				w->scenery.var_482++;
				if (w->scenery.var_482 < 8) {
					if (RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, sint8) != INPUT_STATE_SCROLL_LEFT) {
						w->min_width = WINDOW_SCENERY_WIDTH;
						w->max_width = WINDOW_SCENERY_WIDTH;
						w->min_height = WINDOW_SCENERY_HEIGHT;
						w->max_height = WINDOW_SCENERY_HEIGHT;
					}
				} else {
					int windowHeight = w->scrolls[0].v_bottom + 0x3E;
					if (windowHeight > 0x1C6)
						windowHeight = 0x1C6;
					if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16) < 0x258){
						if (windowHeight > 0x176)
							windowHeight = 0x176;
					}

					w->min_width = WINDOW_SCENERY_WIDTH;
					w->max_width = WINDOW_SCENERY_WIDTH;
					w->min_height = windowHeight;
					w->max_height = windowHeight;
				}
			}
		}
	} else {
		w->scenery.var_482 = 0;
		if (RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, sint8) != INPUT_STATE_SCROLL_LEFT) {
			w->min_width = WINDOW_SCENERY_WIDTH;
			w->max_width = WINDOW_SCENERY_WIDTH;
			w->min_height = WINDOW_SCENERY_HEIGHT;
			w->max_height = WINDOW_SCENERY_HEIGHT;
		}
	}
	
	// todo
	//if (window_scenery_is_tool_active())
		//window_close(w);
	
	if (RCT2_GLOBAL(0x00F64F19, uint8) != 1) {
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TOOL, uint8) = 0x17;
		return;
	}

	uint16 typeId = RCT2_GLOBAL(0x00F64EDC, uint8);
	uint16 sceneryIndex = RCT2_ADDRESS(0x00F64EDD, uint16)[typeId];
	if (sceneryIndex == 0xFFFF)
		return;

	if (sceneryIndex > 0x400) { // banner
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TOOL, uint8) = 0x18;
	} else if (sceneryIndex > 0x300) { // large scenery
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TOOL, uint8) =
			g_largeSceneryEntries[sceneryIndex - 0x300]->large_scenery.tool_id;
	} else if (sceneryIndex > 0x200) { // wall
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TOOL, uint8) =
			g_wallSceneryEntries[sceneryIndex - 0x200]->wall.tool_id;
	} else if (sceneryIndex > 0x100) { // path bit
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TOOL, uint8) =
			g_pathBitSceneryEntries[sceneryIndex - 0x100]->path_bit.tool_id;
	} else { // small scenery
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TOOL, uint8) =
			g_smallSceneryEntries[sceneryIndex]->small_scenery.tool_id;
	}
}

/**
*
*  rct2: 0x006E1A91
*/
void window_scenery_scrollgetsize() {
	int scrollHeight = window_scenery_scrollgetsize_num();

#ifdef _MSC_VER
	__asm mov edx, scrollHeight
#else
	__asm__("mov edx, %[scrollHeight] " : [scrollHeight] "+m" (scrollHeight));
#endif
}

short get_scenery_id_by_cursor_pos(short x, short y) {
	int tabSceneryIndex = x / 66 + (y / 80) * 9;
	uint8 tabIndex = RCT2_GLOBAL(0x00F64EDC, uint8);
	uint32 sceneryTabItems = RCT2_ADDRESS(0x00F64F2C, uint32)[tabIndex];

	int itemCounter = 0;
	sint16 sceneryId = 0;
	while (itemCounter <= tabSceneryIndex) {
		sceneryId = RCT2_ADDRESS(sceneryTabItems, sint16)[itemCounter];
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
void window_scenery_scrollmousedown() {
	short x, y;
	rct_window *w;

	window_scrollmouse_get_registers(w, x, y);

	short sceneryId = get_scenery_id_by_cursor_pos(x, y);
	if (sceneryId == -1)
		return;

	uint8 tabIndex = RCT2_GLOBAL(0x00F64EDC, uint8);
	RCT2_ADDRESS(0x00F64EDD, uint16)[tabIndex] = sceneryId;

	RCT2_GLOBAL(0x00F64F19, uint8) &= 0xFE;
	sound_play_panned(4, (w->width >> 1) + w->x);
	w->scenery.var_482 = 0xFFF0;
	RCT2_GLOBAL(0x00F64EB4, uint32) = 0x80000000;
	window_invalidate(w);
}

/**
*
*  rct2: 0x006E1BB8
*/
void window_scenery_scrollmouseover() {
	short x, y;
	rct_window *w;

	window_scrollmouse_get_registers(w, x, y);
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
void window_scenery_tooltip() {
	uint16 tooltipIndex;

	#ifdef _MSC_VER
	__asm mov tooltipIndex, ax
	#else
	__asm__("mov %[tooltipIndex], ax " : [tooltipIndex] "+m" (tooltipIndex));
	#endif

	if (tooltipIndex == 0x18)
	{
		RCT2_GLOBAL(0x013CE952, uint16) = 3159;
	}
	else if (tooltipIndex >= 4 && tooltipIndex < 0x17)
	{
		uint32* sceneryEntry = RCT2_ADDRESS(RCT2_ADDRESS_SCENERY_SET_ENTRIES, uint32*)[tooltipIndex - 4];
		RCT2_GLOBAL(0x013CE952, uint16) = RCT2_GLOBAL((int)sceneryEntry, uint16);
	}
}

/**
*
*  rct2: 0x006E118B
*/
void window_scenery_invalidate() {
	rct_window* w;

	window_get_register(w);

	uint16 typeId = RCT2_GLOBAL(0x00F64EDC, uint8);
	uint32 title_string_id = 0x715;
	if (typeId <= 0x13) {
		title_string_id = *(RCT2_ADDRESS(RCT2_ADDRESS_SCENERY_SET_ENTRIES, uint32*)[typeId]);
	}
	window_scenery_widgets[WIDX_SCENERY_TITLE].image = title_string_id;

	w->pressed_widgets = (((uint32)w->pressed_widgets & 0xFF00000F) | (1 << (typeId + 4))) & 0xBBFFFFFF;

	if (RCT2_GLOBAL(0x00F64F19, uint8) == 1) { // repaint colored scenery tool is off
		w->pressed_widgets |= 0x04000000;
	}
		
	window_scenery_widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type = WWT_EMPTY;
	window_scenery_widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].type = WWT_EMPTY;
	
	uint16 globalSceneryId = RCT2_ADDRESS(0x00F64EDD, uint16)[typeId];
	if (globalSceneryId != 0xFFFF) {
		if (globalSceneryId < 0x100) {
			if (!(RCT2_GLOBAL(0x00F64F19, uint8) & 1))
				window_scenery_widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].type = WWT_FLATBTN;

			rct_scenery_entry* sceneryEntry = g_smallSceneryEntries[globalSceneryId];
			if (sceneryEntry->small_scenery.flags & SMALL_SCENERY_FLAG4) {
				window_scenery_widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type = WWT_FLATBTN;
			}
		}
		else if (globalSceneryId > 0x300) {
			window_scenery_widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type = WWT_FLATBTN;
		}
	}

	window_scenery_widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].image =
		(RCT2_GLOBAL(0x00F64F06, uint8) << 19) + 0x600013C3;
	window_scenery_widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].image =
		(RCT2_GLOBAL(0x00F64F07, uint8) << 19) + 0x600013C3;
	window_scenery_widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].image =
		(RCT2_GLOBAL(0x00F64F08, uint8) << 19) + 0x600013C3;

	window_scenery_widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WWT_EMPTY;
	window_scenery_widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WWT_EMPTY;
	window_scenery_widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].type = WWT_EMPTY;
	
	if (RCT2_GLOBAL(0x00F64F19, uint8) & 1) { // repaint colored scenery tool is on
		window_scenery_widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WWT_COLORBTN;
		window_scenery_widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WWT_COLORBTN;
		window_scenery_widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].type = WWT_COLORBTN;
		window_scenery_widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type = WWT_EMPTY;
	}
	else if (globalSceneryId != 0xFFFF) {
		rct_scenery_entry* sceneryEntry = NULL;

		if (globalSceneryId >= 0x400) {
			sceneryEntry = g_bannerSceneryEntries[globalSceneryId - 0x400];
			
			if (sceneryEntry->banner.flags & 1)
				window_scenery_widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WWT_COLORBTN;
		}
		else if (globalSceneryId >= 0x300) {
			sceneryEntry = g_largeSceneryEntries[globalSceneryId - 0x300];

			if (sceneryEntry->large_scenery.flags & 1)
				window_scenery_widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WWT_COLORBTN;
			if (sceneryEntry->large_scenery.flags & 2)
				window_scenery_widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WWT_COLORBTN;
		}
		else if (globalSceneryId >= 0x200) {
			sceneryEntry = g_wallSceneryEntries[globalSceneryId - 0x200];
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
		}
		else if (globalSceneryId < 0x100) {
			sceneryEntry = g_smallSceneryEntries[globalSceneryId];

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
	window_scenery_widgets[WIDX_SCENERY_LIST].right = w->width - 0x1A;
	window_scenery_widgets[WIDX_SCENERY_LIST].bottom = w->height - 0x0E;

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
void window_scenery_paint() {
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);

	uint16 typeId = RCT2_GLOBAL(0x00F64EDC, uint8);
	uint16 selectedTab = typeId + 4;
	uint32 image_id = ((w->colours[1] << 19) | window_scenery_widgets[selectedTab].image) + 1ul;
	
	gfx_draw_sprite(dpi, image_id,
		w->x + window_scenery_widgets[selectedTab].left,
		w->y + window_scenery_widgets[selectedTab].top,
		selectedTab);
	
	sint16 bp = w->scenery.selected_scenery_id;
	if (bp == -1) {
		if (RCT2_GLOBAL(0x00F64F19, uint8) & 1)  // repaint colored scenery tool is on
			return;

		bp = RCT2_ADDRESS(0x00F64EDD, uint16)[typeId];

		if (bp == -1)
			return;
	}

	uint32 price = 0;

	rct_scenery_entry* sceneryEntry = NULL;
	if (bp >= 0x400) {
		sceneryEntry = g_bannerSceneryEntries[bp - 0x400];
		price = sceneryEntry->banner.price;
	} else if (bp >= 0x300) {
		sceneryEntry = g_largeSceneryEntries[bp - 0x300];
		price = sceneryEntry->large_scenery.price * 10;
	} else if (bp >= 0x200) {
		sceneryEntry = g_wallSceneryEntries[bp - 0x200];
		price = sceneryEntry->wall.price;
	} else if (bp >= 0x100) {
		sceneryEntry = g_pathBitSceneryEntries[bp - 0x100];
		price = sceneryEntry->path_bit.price;
	} else {
		sceneryEntry = g_smallSceneryEntries[bp];
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

	uint8 tabIndex = RCT2_GLOBAL(0x00F64EDC, uint8);
	uint32 sceneryTabItems = RCT2_ADDRESS(0x00F64F2C, uint32)[tabIndex];

	int sceneryTabItemIndex = 0;
	sint16 currentSceneryGlobalId = -1;
	sint16 left = 0, top = 0;
	uint8 sceneryRotation = RCT2_GLOBAL(0x00F64F05, uint8);
	while ((currentSceneryGlobalId = RCT2_ADDRESS(sceneryTabItems, sint16)[sceneryTabItemIndex]) != -1) {
		uint16 sceneryTabIndex = RCT2_ADDRESS(0x00F64EDD, uint16)[tabIndex];
				
		if (RCT2_GLOBAL(0x00F64F19, uint8) == 1)
		{
			if (w->scenery.selected_scenery_id == currentSceneryGlobalId) {
				gfx_fill_rect_inset(dpi, left, top, left + 65, top + 79, w->colours[1], 0x80);
			}
		} else {
			if (sceneryTabIndex == currentSceneryGlobalId) {
				gfx_fill_rect_inset(dpi, left, top, left + 65, top + 79, w->colours[1], 0xA0);
			}
			else if (w->scenery.selected_scenery_id == currentSceneryGlobalId) {
				gfx_fill_rect_inset(dpi, left, top, left + 65, top + 79, w->colours[1], 0x80);
			}
		}

		rct_scenery_entry* sceneryEntry;
		if (currentSceneryGlobalId >= 0x400) {
			sceneryEntry = g_bannerSceneryEntries[currentSceneryGlobalId - 0x400];
			uint32 imageId = sceneryEntry->image + sceneryRotation * 2;
			imageId |= (RCT2_GLOBAL(0x00F64F06, uint8) << 19) | 0x20000000;

			gfx_draw_sprite(dpi, imageId, left + 0x21, top + 0x28, w->colours[1]);
			gfx_draw_sprite(dpi, imageId + 1, left + 0x21, top + 0x28, w->colours[1]);
		}
		else if (currentSceneryGlobalId >= 0x300) {
			sceneryEntry = g_largeSceneryEntries[currentSceneryGlobalId - 0x300];
			uint32 imageId = sceneryEntry->image + sceneryRotation;
			imageId |= (RCT2_GLOBAL(0x00F64F06, uint8) << 19) | 0x20000000;
			imageId |= (RCT2_GLOBAL(0x00F64F07, uint8) << 24) | 0x80000000;

			gfx_draw_sprite(dpi, imageId, left + 0x21, top, w->colours[1]);
		}
		else if (currentSceneryGlobalId >= 0x200) {
			sceneryEntry = g_wallSceneryEntries[currentSceneryGlobalId - 0x200];
			rct_drawpixelinfo* clipdpi = clip_drawpixelinfo(dpi, left + 1, 64, top + 1, 78);
			if (clipdpi != NULL) {
				uint32 imageId = sceneryEntry->image;
				uint8 tertiaryColour = w->colours[1];

				if (sceneryEntry->wall.flags & WALL_SCENERY_FLAG2) {
					imageId |= (RCT2_GLOBAL(0x00F64F06, uint8) << 19) | 0x20000000;

					if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_SECONDARY_COLOUR) {
						imageId |= (RCT2_GLOBAL(0x00F64F07, uint8) << 24) | 0x80000000;
					}
				}
				else {
					imageId |= (RCT2_GLOBAL(0x00F64F06, uint8) << 19) | 0x20000000;

					if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_SECONDARY_COLOUR) {
						imageId |= (RCT2_GLOBAL(0x00F64F07, uint8) << 24) | 0x80000000;

						if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_TERNARY_COLOUR) {
							imageId &= 0xDFFFFFFF;
							tertiaryColour = RCT2_GLOBAL(0x00F64F08, uint8);
						}
							
					}
				}

				gfx_draw_sprite(clipdpi, imageId, 0x2F, (sceneryEntry->wall.height * 2) + 0x32,
					tertiaryColour);
				rct2_free(clipdpi);
			}
		}
		else if (currentSceneryGlobalId >= 0x100) {
			sceneryEntry = g_pathBitSceneryEntries[currentSceneryGlobalId - 0x100];
			uint32 imageId = sceneryEntry->image;

			gfx_draw_sprite(dpi, imageId, left + 0x0B, top + 0x10, w->colours[1]);
		} else {
			sceneryEntry = g_smallSceneryEntries[currentSceneryGlobalId];
			rct_drawpixelinfo* clipdpi = clip_drawpixelinfo(dpi, left + 1, 64, top + 1, 78);
			if (clipdpi != NULL) {

				uint32 imageId = sceneryEntry->image + sceneryRotation;

				if (sceneryEntry->small_scenery.flags & SMALL_SCENERY_HAS_PRIMARY_COLOUR) {
					imageId |= (RCT2_GLOBAL(0x00F64F06, uint8) << 19) | 0x20000000;

					if (sceneryEntry->small_scenery.flags & SMALL_SCENERY_HAS_SECONDARY_COLOUR) {
						imageId |= (RCT2_GLOBAL(0x00F64F07, uint8) << 24) | 0x80000000;
					}
				}

				uint16 spriteTop = (sceneryEntry->small_scenery.height / 4) + 0x2B;

				if (sceneryEntry->small_scenery.flags & SMALL_SCENERY_FLAG1 &&
					sceneryEntry->small_scenery.flags &  SMALL_SCENERY_FLAG2) {
					spriteTop -= 0x0C;
				}

				gfx_draw_sprite(clipdpi, imageId, 0x20, spriteTop, w->colours[1]);

				if (sceneryEntry->small_scenery.flags & 0x200) {
					imageId = ((sceneryEntry->image + sceneryRotation) + 0x40000004) +
						((RCT2_GLOBAL(0x00F64F06, uint8) + 0x70) << 19);

					gfx_draw_sprite(clipdpi, imageId, 0x20, spriteTop, w->colours[1]);
				}

				if (sceneryEntry->small_scenery.flags & SMALL_SCENERY_FLAG8) {
					imageId = (sceneryEntry->image + sceneryRotation) + 4;
					gfx_draw_sprite(clipdpi, imageId, 0x20, spriteTop, w->colours[1]);
				}

				rct2_free(clipdpi);
			}
		}
		
		left += 66;
		if (left >= 594) {
			top += 80;
			left = 0;
		}
		sceneryTabItemIndex++;
	}
}