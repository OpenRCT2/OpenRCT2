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
#include "addresses.h"
#include "game.h"
#include "gfx.h"
#include "peep.h"
#include "sprite.h"
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
static void window_scenery_invalidate();
static void window_scenery_paint();
static void window_scenery_tooltip();

static void* window_scenery_events[] = {
	(void*)0x006E1A73,    // window_scenery_close
	(void*)0x006E19FC,    // window_scenery_mouseup
	(void*)0x006E1E48,    // window_scenery_resize,
	(void*)0x006E1A25,    // window_scenery_mousedown,
	(void*)0x006E1A54,    // window_scenery_dropdown,
	window_scenery_emptysub,
	(void*)0x006E1CD3,    // window_scenery_update,
	(void*)0x006E1B9F,	  // window_scenery_emptysub,
	window_scenery_emptysub,
	window_scenery_emptysub,
	window_scenery_emptysub,			   // window_scenery_tooldown
	window_scenery_emptysub,
	window_scenery_emptysub,
	window_scenery_emptysub,
	window_scenery_emptysub,
	(void*)0x006E1A91,	  // window_scenery_scrollgetsize,
	(void*)0x006E1C4A,	  // window_scenery_scrollmousedown,
	window_scenery_emptysub,
	(void*)0x006E1BB8,	  // window_scenery_scrollmouseover,
	window_scenery_emptysub,
	window_scenery_emptysub,
	window_scenery_emptysub,
	window_scenery_tooltip, //(void*)0x006E1C05,	  // window_scenery_tooltip,
	window_scenery_emptysub,
	window_scenery_emptysub,
	window_scenery_invalidate, //(void*)0x006E118B,	  // window_scenery_invalidate,
	window_scenery_paint,	  // (void*)0x006E1462,	  // window_scenery_paint, 
	(void*)0x006E15ED,	  // window_scenery_scrollpaint,
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
	WIDX_SCENERY_FLATBUTTON1,				// 2000000
	WIDX_SCENERY_FLATBUTTON2,				// 4000000
	WIDX_SCENERY_COLORBUTTON1,				// 8000000
	WIDX_SCENERY_COLORBUTTON2,				// 10000000
	WIDX_SCENERY_COLORBUTTON3,				// 20000000
	WIDX_SCENERY_FLATBUTTON3,				// 40000000
};

static rct_widget window_scenery_widgets[] = {
	{ WWT_FRAME, 0, 0, 633, 0, 141, 0xFFFFFFFF, STR_NONE },								// 1
	{ WWT_CAPTION, 0, 1, 632, 1, 14, 0xFFFFFFFF, STR_WINDOW_TITLE_TIP },				// 2
	{ WWT_CLOSEBOX, 0, 621, 631, 2, 13, STR_CLOSE_X, STR_CLOSE_WINDOW_TIP },			// 4
	{ WWT_RESIZE, 1, 0, 633, 43, 141, 0xFFFFFFFF, STR_NONE },							// 8
	{ WWT_TAB, 1, 3, 33, 17, 43, 0xFFFFFFFF, 1812 },									// 10
	{ WWT_TAB, 1, 34, 64, 17, 43, 0xFFFFFFFF, 1812 },									// 20
	{ WWT_TAB, 1, 65, 95, 17, 43, 0xFFFFFFFF, 1812 },									// 40
	{ WWT_TAB, 1, 96, 126, 17, 43, 0xFFFFFFFF, 1812 },									// 80
	{ WWT_TAB, 1, 127, 157, 17, 43, 0xFFFFFFFF, 1812 },									// 100
	{ WWT_TAB, 1, 158, 188, 17, 43, 0xFFFFFFFF, 1812 },									// 200
	{ WWT_TAB, 1, 189, 219, 17, 43, 0xFFFFFFFF, 1812 },									// 400
	{ WWT_TAB, 1, 220, 250, 17, 43, 0xFFFFFFFF, 1812 },									// 800
	{ WWT_TAB, 1, 251, 281, 17, 43, 0xFFFFFFFF, 1812 },									// 1000
	{ WWT_TAB, 1, 282, 312, 17, 43, 0xFFFFFFFF, 1812 },									// 2000
	{ WWT_TAB, 1, 313, 343, 17, 43, 0xFFFFFFFF, 1812 },									// 4000
	{ WWT_TAB, 1, 344, 374, 17, 43, 0xFFFFFFFF, 1812 },									// 8000
	{ WWT_TAB, 1, 375, 405, 17, 43, 0xFFFFFFFF, 1812 },									// 10000
	{ WWT_TAB, 1, 406, 436, 17, 43, 0xFFFFFFFF, 1812 },									// 20000
	{ WWT_TAB, 1, 437, 467, 17, 43, 0xFFFFFFFF, 1812 },									// 40000
	{ WWT_TAB, 1, 468, 498, 17, 43, 0xFFFFFFFF, 1812 },									// 80000
	{ WWT_TAB, 1, 468, 498, 17, 43, 0xFFFFFFFF, 1812 },									// 100000
	{ WWT_TAB, 1, 468, 498, 17, 43, 0xFFFFFFFF, 1812 },									// 200000
	{ WWT_TAB, 1, 468, 498, 17, 43, 0xFFFFFFFF, 1812 },									// 400000
	{ WWT_TAB, 1, 468, 498, 17, 43, 0x20001598, 1812 },									// 800000
	{ WWT_SCROLL, 1, 2, 608, 47, 126, 2, STR_NONE },									// 1000000
	{ WWT_FLATBTN, 1, 609, 632, 44, 67, 5169, STR_ROTATE_OBJECTS_90 },					// 2000000
	{ WWT_FLATBTN, 1, 609, 632, 68, 91, 5173, 3102 },									// 4000000
	{ WWT_COLORBTN, 1, 615, 626, 93, 104, 0xFFFFFFFF, STR_SELECT_COLOUR },				// 8000000
	{ WWT_COLORBTN, 1, 615, 626, 105, 116, 0xFFFFFFFF, STR_SELECT_SECONDARY_COLOUR },	// 10000000
	{ WWT_COLORBTN, 1, 615, 626, 117, 128, 0xFFFFFFFF, STR_SELECT_TERNARY_COLOUR },		// 20000000
	{ WWT_FLATBTN, 1, 609, 632, 117, 140, 5172, 3225 },									// 40000000
	{ WIDGETS_END },
};

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

	RCT2_CALLPROC_EBPSAFE(0x006DFA00);

	window = window_create(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16) - WINDOW_SCENERY_WIDTH, 0x1D, WINDOW_SCENERY_WIDTH, WINDOW_SCENERY_HEIGHT,
		window_scenery_events, WC_SCENERY, WF_2);
	window->widgets = window_scenery_widgets;

	window->enabled_widgets =
		(1 << WIDX_SCENERY_CLOSE) |
		(1 << WIDX_SCENERY_FLATBUTTON1) |
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
		(1 << WIDX_SCENERY_COLORBUTTON1) |
		(1 << WIDX_SCENERY_COLORBUTTON2) |
		(1 << WIDX_SCENERY_FLATBUTTON2) |
		(1 << WIDX_SCENERY_COLORBUTTON3) |
		(1 << WIDX_SCENERY_FLATBUTTON3);

	window_init_scroll_widgets(window);
	RCT2_CALLPROC_X(0x006E1EB4, 0, 0, 0, 0, (int)window, 0, 0);
	show_gridlines();
	RCT2_GLOBAL(0x00F64F05, uint8) = 3;
	RCT2_GLOBAL(0x00F64F12, uint8) = 0;
	RCT2_GLOBAL(0x00F64F13, uint8) = 0;
	window->var_480 = 0xFFFF;
	window->var_482 = 0;
	window_push_others_below(window);
	RCT2_GLOBAL(0x00F64F0D, uint8) = 0;
	RCT2_GLOBAL(0x00F64EB4, uint32) = 0x80000000;
	RCT2_GLOBAL(0x00F64EC0, uint16) = 0;
	RCT2_GLOBAL(0x00F64F19, uint8) = 0;
	RCT2_GLOBAL(0x00F64F1A, uint8) = 0;

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
*  rct2: 0x006E1C05
*/
void window_scenery_tooltip() {
	RCT2_GLOBAL(0x013CE952, uint16) = STR_LIST;
}

/**
*
*  rct2: 0x006E118B
*/
void window_senery_invalidate() {
}

/**
*
*  rct2: 0x006E1462
*/
void window_scenery_paint() {
	int i;
	uint8 selectedTab;
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);

	uint32 selectedTab = RCT2_GLOBAL(0x00F64EDC, uint8) + 4;
	uint32 image_id = ((w->colours[1] << 19) | window_scenery_widgets[selectedTab].image) + 1ul;
	
	gfx_draw_sprite(dpi, image_id,
		window_scenery_widgets[selectedTab].left,
		window_scenery_widgets[selectedTab].top,
		selectedTab);


}