/*****************************************************************************
 * Copyright (c) 2014 Ted John, Dennis Devriendt
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

#include "addresses.h"
#include "sprites.h"
#include "strings.h"
#include "widget.h"
#include "window.h"

static enum WINDOW_MAP_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_RESIZE = 3,
	WIDX_PEOPLE_TAB = 4,
	WIDX_RIDES_TAB = 5,
	WIDX_MAP = 6,
	WIDX_MAP_SIZE_SPINNER = 7,
	WIDX_MAP_SIZE_SPINNER_UP = 8,
	WIDX_MAP_SIZE_SPINNER_DOWN = 9,
	WIDX_SET_LAND_RIGHTS = 10,
	WIDX_BUILD_PARK_ENTRANCE = 11,
	WIDX_PEOPLE_STARTING_POSITION = 12,
	WIDX_LAND_TOOL = 13,
	WIDX_LAND_TOOL_SMALLER = 14,
	WIDX_LAND_TOOL_LARGER = 15,
	WIDX_LAND_OWNED_CHECKBOX = 16,
	WIDX_CONSTRUCTION_RIGHTS_OWNED_CHECKBOX = 17,
	WIDX_LAND_SALE_CHECKBOX = 18,
	WIDX_CONSTRUCTION_RIGHTS_SALE_CHECKBOX = 19,
	WIDX_ROTATE_90 = 20
};

static rct_widget window_map_widgets[] = {
	{ WWT_FRAME,			0,	0,		244,	0,		258,	STR_NONE,		STR_NONE },
	{ WWT_CAPTION,			0,	1,		243,	1,		14,		STR_MAP,		STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,			0,	232,	242,	2,		13,		STR_CLOSE_X,	STR_CLOSE_WINDOW_TIP },
	{ WWT_RESIZE,			1,	0,		244,	43,		257,	STR_NONE,		STR_NONE },
	{ WWT_COLORBTN,			1,	3,		33,		17,		43,		0x02000144E,	STR_SHOW_PEOPLE_ON_MAP_TIP },
	{ WWT_COLORBTN,			1,	34,		64,		17,		43,		0x02000144E,	STR_SHOW_RIDES_STALLS_ON_MAP_TIP },
	{ WWT_SCROLL,			1,	3,		241,	46,		225,	0x3,			STR_NONE },
	{ WWT_SPINNER,			1,	104,	198,	229,	240,	0xC8C,			STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	230,	234,	STR_NUMERIC_UP,	STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	235,	239,	STR_NUMERIC_DOWN, STR_NONE },
	{ WWT_FLATBTN,			1,	4,		27,		1,		24,		SPR_BUY_LAND_RIGHTS, STR_SELECT_PARK_OWNED_LAND_TIP },
	{ WWT_FLATBTN,			1,	4,		27,		1,		24,		SPR_PARK_ENTRANCE, STR_BUILD_PARK_ENTRANCE_TIP },
	{ WWT_FLATBTN,			1,	28,		51,		1,		24,		STR_NONE,		STR_SET_STARTING_POSITIONS_TIP },
	{ WWT_IMGBTN,			1,	4,		47,		17,		48,		SPR_LAND_TOOL_SIZE_0, STR_NONE },
	{ WWT_TRNBTN,			1,	5,		20,		18,		33,		0x02000157B,	STR_ADJUST_SMALLER_LAND_TIP },
	{ WWT_TRNBTN,			1,	31,		46,		32,		47,		0x02000157D,	STR_ADJUST_LARGER_LAND_TIP },
	{ WWT_CHECKBOX,			1,	58,		241,	197,	208,	STR_LAND_OWNED,	STR_SET_LAND_TO_BE_OWNED_TIP },
	{ WWT_CHECKBOX,			1,	58,		241,	197,	208,	STR_CONSTRUCTION_RIGHTS_OWNED, STR_SET_CONSTRUCTION_RIGHTS_TO_BE_OWNED_TIP },
	{ WWT_CHECKBOX,			1,	58,		241,	197,	208,	STR_LAND_SALE,	STR_SET_LAND_TO_BE_AVAILABLE_TIP },
	{ WWT_CHECKBOX,			1,	58,		231,	197,	208,	STR_CONSTRUCTION_RIGHTS_SALE, STR_SET_CONSTRUCTION_RIGHTS_TO_BE_AVAILABLE_TIP },
	{ WWT_FLATBTN,			1,	218,	241,	45,		68,		SPR_ROTATE_ARROW, STR_ROTATE_OBJECTS_90 },
	{ WIDGETS_END },
};

static void window_map_emptysub() { }
static void window_map_close();
static void window_map_mouseup();
static void window_map_mousedown();
static void window_map_update(rct_window *w);
static void window_map_scrollgetsize();
static void window_map_scrollmousedown();
static void window_map_invalidate();
static void window_map_paint();
static void window_map_scrollpaint();

static void* window_map_events[] = {
	window_map_close,
	window_map_mouseup,
	window_map_emptysub,
	window_map_mousedown,
	window_map_emptysub,
	window_map_emptysub,
	window_map_update,
	window_map_emptysub,
	window_map_emptysub,
	(void*)0x0068D093,
	(void*)0x0068D074,
	(void*)0x0068D088,
	window_map_emptysub,
	(void*)0x0068D055,
	window_map_emptysub,
	window_map_scrollgetsize,
	window_map_scrollmousedown,
	window_map_scrollmousedown,
	window_map_emptysub,
	window_map_emptysub,
	window_map_emptysub,
	window_map_emptysub,
	(void*)0x0068D140,
	window_map_emptysub,
	window_map_emptysub,
	window_map_invalidate,
	window_map_paint,
	window_map_scrollpaint
};

/**
*
*  rct2: 0x0068C88A
*/
void window_map_open()
{
	rct_window* w;
	int* var;

	// Check if window is already open
	w = window_bring_to_front_by_id(WC_MAP, 0);
	if (w != NULL) {
		w->selected_tab = 0;
		w->var_490 = 0;
		return;
	}

	var = (int*)rct2_malloc(0x40000);
	if (var == NULL)
		return;

	RCT2_GLOBAL(0x00F1AD68, uint32) = (uint32)var;
	w = window_create_auto_pos(245, 259, (uint32*)window_map_events, WC_MAP, 0x0400);
	w->widgets = window_map_widgets;
	w->enabled_widgets =
		(1 << 2) |
		(1 << 4) |
		(1 << 5) |
		(1 << 8) |
		(1 << 9) |
		(1 << 14) |
		(1 << 15) |
		(1 << 10) |
		(1 << 16) |
		(1 << 17) |
		(1 << 18) |
		(1 << 19) |
		(1 << 11) |
		(1 << 20) |
		(1 << 12);
	//TODO: .text:0068C943                 or      dword ptr [esi+20h], 300h

	window_init_scroll_widgets(w);
	w->var_480 = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint16);

	RCT2_CALLPROC_EBPSAFE(0x0068CA6C);
	RCT2_GLOBAL(0x00F64F05, uint8) = 0;
	RCT2_CALLPROC_EBPSAFE(0x0068C990);

	w->colours[0] = 12;
	w->colours[1] = 24;
}

/**
*
*  rct2: 0x0068D0F1
*/
static void window_map_close()
{
	RCT2_CALLPROC_EBPSAFE(0x0068D0F1);
}

/**
*
*  rct2: 0x006BAFCA
*/
static void window_map_mouseup()
{
	RCT2_CALLPROC_EBPSAFE(0x006BAFCA);
	/*short widgetIndex;
	rct_window *w;

	__asm mov widgetIndex, dx
	__asm mov w, esi

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	}*/
}

/**
*
*  rct2: 0x0068D040
*/
static void window_map_mousedown()
{
	RCT2_CALLPROC_EBPSAFE(0x0068D040);
}

/**
*
*  rct2: 0x0068D7FB
*/
static void window_map_update(rct_window *w)
{
	RCT2_CALLPROC_EBPSAFE(0x0068D7FB);
}

/**
*
*  rct2: 0x0068D7CC
*/
static void window_map_scrollgetsize()
{
	RCT2_CALLPROC_EBPSAFE(0x0068D7CC);
}

/**
*
*  rct2: 0x0068D726
*/
static void window_map_scrollmousedown()
{
	RCT2_CALLPROC_EBPSAFE(0x0068D726);
}

/**
*
*  rct2: 0x0068CA8F
*/
static void window_map_invalidate()
{
	rct_window *w;
	uint32 pressed_widgets;
	int i, height;

	__asm mov w, esi

	// set the pressed widgets
	pressed_widgets = (uint32)w->pressed_widgets;
	pressed_widgets &= 0xFFFFFF8F;						// both tabs and the map
	pressed_widgets |= (1 << (w->selected_tab + 4));	// people tab or rides tab
	pressed_widgets |= (1 << WIDX_LAND_TOOL);			// land tool
	pressed_widgets &= 0xFFF0FFFF;						// the land option checkboxes

	if (RCT2_GLOBAL(0x0F1AD61, uint8) & 8)
		pressed_widgets |= (1 << WIDX_LAND_SALE_CHECKBOX);

	if (RCT2_GLOBAL(0x0F1AD61, uint8) & 4)
		pressed_widgets |= (1 << WIDX_CONSTRUCTION_RIGHTS_SALE_CHECKBOX);

	if (RCT2_GLOBAL(0x0F1AD61, uint8) & 2)
		pressed_widgets |= (1 << WIDX_LAND_OWNED_CHECKBOX);

	if (RCT2_GLOBAL(0x0F1AD61, uint8) & 1)
		pressed_widgets |= (1 << WIDX_CONSTRUCTION_RIGHTS_OWNED_CHECKBOX);

	w->pressed_widgets = pressed_widgets;

	// resizing changes some widget coordinates
	w->widgets[WIDX_BACKGROUND].right = w->width - 1;
	w->widgets[WIDX_BACKGROUND].bottom = w->height - 1;
	w->widgets[WIDX_RESIZE].right = w->width - 1;
	w->widgets[WIDX_RESIZE].bottom = w->height - 1;
	w->widgets[WIDX_TITLE].right = w->width - 2;
	w->widgets[WIDX_CLOSE].left = w->width - 2 - 11;
	w->widgets[WIDX_CLOSE].right = w->width - 2 - 11 + 10;
	w->widgets[WIDX_MAP].right = w->width - 4;

	if ((RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR))
		w->widgets[WIDX_MAP].bottom = w->height - 1 - 72;
	else if (w->selected_tab == 1)
		w->widgets[WIDX_MAP].bottom = w->height - 1 - 44;
	else
		w->widgets[WIDX_MAP].bottom = w->height - 1 - 14;

	w->widgets[WIDX_MAP_SIZE_SPINNER].top = w->height - 15;
	w->widgets[WIDX_MAP_SIZE_SPINNER].bottom = w->height - 4;
	w->widgets[WIDX_MAP_SIZE_SPINNER_UP].top = w->height - 14;
	w->widgets[WIDX_MAP_SIZE_SPINNER_UP].bottom = w->height - 10;
	w->widgets[WIDX_MAP_SIZE_SPINNER_DOWN].top = w->height - 9;
	w->widgets[WIDX_MAP_SIZE_SPINNER_DOWN].bottom = w->height - 5;

	w->widgets[WIDX_SET_LAND_RIGHTS].top = w->height - 70;
	w->widgets[WIDX_SET_LAND_RIGHTS].bottom = w->height - 70 + 23;
	w->widgets[WIDX_BUILD_PARK_ENTRANCE].top = w->height - 46;
	w->widgets[WIDX_BUILD_PARK_ENTRANCE].bottom = w->height - 46 + 23;
	w->widgets[WIDX_ROTATE_90].top = w->height - 46;
	w->widgets[WIDX_ROTATE_90].bottom = w->height - 46 + 23;
	w->widgets[WIDX_PEOPLE_STARTING_POSITION].top = w->height - 46;
	w->widgets[WIDX_PEOPLE_STARTING_POSITION].bottom = w->height - 46 + 23;

	w->widgets[WIDX_LAND_TOOL].top = w->height - 42;
	w->widgets[WIDX_LAND_TOOL].bottom = w->height - 42 + 30;
	w->widgets[WIDX_LAND_TOOL_SMALLER].top = w->height - 41;
	w->widgets[WIDX_LAND_TOOL_SMALLER].bottom = w->height - 41 + 15;
	w->widgets[WIDX_LAND_TOOL_LARGER].top = w->height - 27;
	w->widgets[WIDX_LAND_TOOL_LARGER].bottom = w->height - 27 + 15;

	// land tool mode (4 checkboxes)
	height = w->height - 55;
	for (i = 0; i < 4; i++) {
		w->widgets[WIDX_LAND_OWNED_CHECKBOX + i].top = height;
		height += 11;
		w->widgets[WIDX_LAND_OWNED_CHECKBOX + i].bottom = height;
		height += 2;
	}

	// disable all scenario editor related widgets
	for (i = WIDX_MAP_SIZE_SPINNER; i <= WIDX_ROTATE_90; i++) {
		w->widgets[i].type = WWT_EMPTY;
	}

	if ((RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR)) {
		// scenario editor: build park entrance selected, show rotate button
		if ((RCT2_GLOBAL(0x009DE518, uint32) & (1 << 3)) &&
			RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, uint8) == WC_MAP &&
			RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, uint8) == WIDX_BUILD_PARK_ENTRANCE) {
			w->widgets[WIDX_ROTATE_90].type = WWT_FLATBTN;
		}

		// always show set land rights button
		w->widgets[WIDX_SET_LAND_RIGHTS].type = WWT_FLATBTN;

		if ((RCT2_GLOBAL(0x009DE518, uint32) & (1 << 3)) &&
			RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, uint8) == WC_MAP) {

			// if not in set land rights mode: show the default scenario editor buttons
			if (RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, uint8) != WIDX_SET_LAND_RIGHTS) {
				w->widgets[WIDX_BUILD_PARK_ENTRANCE].type = WWT_FLATBTN;
				w->widgets[WIDX_PEOPLE_STARTING_POSITION].type = WWT_FLATBTN;
				w->widgets[WIDX_MAP_SIZE_SPINNER].type = WWT_SPINNER;
				w->widgets[WIDX_MAP_SIZE_SPINNER_UP].type = WWT_DROPDOWN_BUTTON;
				w->widgets[WIDX_MAP_SIZE_SPINNER_DOWN].type = WWT_DROPDOWN_BUTTON;
				RCT2_GLOBAL(0x013CE952 + 2, uint16) = RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE, uint16);
			} else {	// if in set land rights mode: show land tool buttons + modes
				w->widgets[WIDX_LAND_TOOL].type = WWT_IMGBTN;
				w->widgets[WIDX_LAND_TOOL_SMALLER].type = WWT_TRNBTN;
				w->widgets[WIDX_LAND_TOOL_LARGER].type = WWT_TRNBTN;
				for (i = 0; i < 4; i++)
					w->widgets[WIDX_LAND_OWNED_CHECKBOX + i].type = WWT_CHECKBOX;
				w->widgets[WIDX_LAND_TOOL].image = SPR_LAND_TOOL_SIZE_0 + 
					RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, uint16);
			}
		}
	}
}

/**
*
*  rct2: 0x0068CDA9
*/
static void window_map_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;
	int image_id;
	int i, x, y;

	__asm mov w, esi
	__asm mov dpi, edi

	window_draw_widgets(w, dpi);

	// guest tab image (animated)
	image_id = SPR_TAB_GUESTS_0;
	if (w->selected_tab == 0)
		image_id += w->var_490 / 4;

	gfx_draw_sprite(dpi, image_id, 
		w->x + w->widgets[WIDX_PEOPLE_TAB].left, 
		w->y + w->widgets[WIDX_PEOPLE_TAB].top);

	// ride/stall tab image (animated)
	image_id = SPR_TAB_RIDE_0;
	if (w->selected_tab == 1)
		image_id += w->var_490 / 4;

	gfx_draw_sprite(dpi, image_id,
		w->x + w->widgets[WIDX_RIDES_TAB].left,
		w->y + w->widgets[WIDX_RIDES_TAB].top);

	// people starting position (scenario editor only)
	if (w->widgets[WIDX_PEOPLE_STARTING_POSITION].type != 0) {
		gfx_draw_sprite(dpi, 0x0B6E0190A,
			w->x + w->widgets[WIDX_PEOPLE_STARTING_POSITION].left + 12,
			w->y + w->widgets[WIDX_PEOPLE_STARTING_POSITION].top + 18);
	}

	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR)) {
		// render the map legend
		if (w->selected_tab != 0) {
			x = w->x + 4;
			y = w->y + w->widgets[WIDX_MAP].bottom + 2;

			for (i = 0; i < 8; i++) {
				gfx_fill_rect(dpi, x, y + 2, x + 6, y + 8, RCT2_GLOBAL(0x00981BCC+ 2 * i, uint8));
				gfx_draw_string_left(dpi, STR_MAP_RIDE + i, w, 0, x + 10, y);
				y += 10;
				if (i == 3) {
					x += 118;
					y -= 40;
				}
			}
		}

		if ((RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, uint8) != WC_MAP) &&
			(RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, uint8) != WIDX_SET_LAND_RIGHTS))
			return;
	}

	// map size spinner
	gfx_draw_string_left(dpi, STR_MAP_SIZE, 0, 0, w->x + 4, w->y + w->widgets[WIDX_MAP_SIZE_SPINNER].top + 1);
}

/**
*
*  rct2: 0x0068CF23
*/
static void window_map_scrollpaint()
{
	RCT2_CALLPROC_EBPSAFE(0x0068CF23);
}