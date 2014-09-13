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

#include <string.h>
#include "addresses.h"
#include "sprites.h"
#include "string_ids.h"
#include "widget.h"
#include "window.h"
#include "window_scenery.h"
#include "viewport.h"

enum WINDOW_MAP_WIDGET_IDX {
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
static void window_map_mousedown(int widgetIndex, rct_window*w, rct_widget* widget);
static void window_map_update(rct_window *w);
static void window_map_scrollgetsize();
static void window_map_scrollmousedown();
static void window_map_invalidate();
static void window_map_paint();
static void window_map_scrollpaint();
static void window_map_tooltip();

static void window_map_set_bounds(rct_window* w);

static void window_map_init_map();
static void sub_68C990();

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
	window_map_tooltip,
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
	uint32* map_image_data;

	// Check if window is already open
	w = window_bring_to_front_by_id(WC_MAP, 0);
	if (w != NULL) {
		w->selected_tab = 0;
		w->list_information_type = 0;
		return;
	}

	map_image_data = rct2_malloc(256 * 256 * sizeof(uint32));
	if (map_image_data == NULL)
		return;

	RCT2_GLOBAL(RCT2_ADDRESS_MAP_IMAGE_DATA, uint32*) = map_image_data;
	w = window_create_auto_pos(245, 259, (uint32*)window_map_events, WC_MAP, 0x0400);
	w->widgets = window_map_widgets;
	w->enabled_widgets =
		(1 << WIDX_CLOSE) |
		(1 << WIDX_PEOPLE_TAB) |
		(1 << WIDX_RIDES_TAB) |
		(1 << WIDX_MAP_SIZE_SPINNER_UP) |
		(1 << WIDX_MAP_SIZE_SPINNER_DOWN) |
		(1 << WIDX_LAND_TOOL_SMALLER) |
		(1 << WIDX_LAND_TOOL_LARGER) |
		(1 << WIDX_SET_LAND_RIGHTS) |
		(1 << WIDX_LAND_OWNED_CHECKBOX) |
		(1 << WIDX_CONSTRUCTION_RIGHTS_OWNED_CHECKBOX) |
		(1 << WIDX_LAND_SALE_CHECKBOX) |
		(1 << WIDX_CONSTRUCTION_RIGHTS_SALE_CHECKBOX) |
		(1 << WIDX_BUILD_PARK_ENTRANCE) |
		(1 << WIDX_ROTATE_90) |
		(1 << WIDX_PEOPLE_STARTING_POSITION);
	w->var_020 |= 0x300;
	window_init_scroll_widgets(w);

	window_map_set_bounds(w);

	w->map.rotation = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint16);

	window_map_init_map();
	RCT2_GLOBAL(0x00F64F05, uint8) = 0;
	sub_68C990();

	w->colours[0] = 12;
	w->colours[1] = 24;
}

/**
*
*  rct2: 0x0068D0F1
*/
static void window_map_close()
{
	rct_window *w;

	window_get_register(w);

	rct2_free(RCT2_GLOBAL(RCT2_ADDRESS_MAP_IMAGE_DATA, uint32*));
	if ((RCT2_GLOBAL(0x009DE518, uint32) & (1 << 3)) &&
		RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, uint8) == w->classification &&
		RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, uint16) == w->number) {
		tool_cancel();
	}
}

/**
*
*  rct2: 0x0068CFC1
*/
static void window_map_mouseup()
{
	//RCT2_CALLPROC_EBPSAFE(0x0068CFC1);
	sint16 var_idx;
	rct_window* var_w;

	window_widget_get_registers(var_w, var_idx);
	
	switch (var_idx)
	{
	case WIDX_CLOSE:
		window_close(var_w);
		break;

	case WIDX_SET_LAND_RIGHTS:
		window_invalidate(var_w);
		if (!tool_set(var_w, var_idx, 2)) // jb nullsub_52
			break;

		RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) = 1;
		RCT2_GLOBAL(0xF1AD61, sint8) = 2;
		show_gridlines();
		show_land_rights();
		show_construction_rights();
		break;

	case WIDX_LAND_OWNED_CHECKBOX:
		RCT2_GLOBAL(0xF1AD61, sint8) ^= 2;

		if (RCT2_GLOBAL(0xF1AD61, sint8) & 2)
			RCT2_GLOBAL(0xF1AD61, sint8) &= 0xF2;

		window_invalidate(var_w);
		break;

	case WIDX_LAND_SALE_CHECKBOX:
		RCT2_GLOBAL(0xF1AD61, sint8) ^= 8;

		if (RCT2_GLOBAL(0xF1AD61, sint8) & 8)
			RCT2_GLOBAL(0xF1AD61, sint8) &= 0xF8;

		window_invalidate(var_w);
		break;

	case WIDX_CONSTRUCTION_RIGHTS_OWNED_CHECKBOX:
		RCT2_GLOBAL(0xF1AD61, sint8) ^= 1;

		if (RCT2_GLOBAL(0xF1AD61, sint8) & 1)
			RCT2_GLOBAL(0xF1AD61, sint8) &= 0xF1;

		window_invalidate(var_w);
		break;

	case WIDX_CONSTRUCTION_RIGHTS_SALE_CHECKBOX:
		RCT2_GLOBAL(0xF1AD61, sint8) ^= 4;

		if (RCT2_GLOBAL(0xF1AD61, sint8) & 4)
			RCT2_GLOBAL(0xF1AD61, sint8) &= 0xF4;

		window_invalidate(var_w);
		break;

	case WIDX_LAND_TOOL_SMALLER:
		--RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16);

		if (RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) < 1)
			RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) = 1;

		window_invalidate(var_w);
		break;

	case WIDX_LAND_TOOL_LARGER:
		++RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16);

		if (RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) > 7)
			RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) = 7;

		window_invalidate(var_w);
		break;

	case WIDX_BUILD_PARK_ENTRANCE:
		window_invalidate(var_w);
		if (!tool_set(var_w, var_idx, 2)) // jb nullsub_52
			break;

		RCT2_GLOBAL(0x9E32D2, sint8) = 0;
		
		if (!(RCT2_GLOBAL(0x9DE518, sint32) & (1 << 6))) // Remove?
			RCT2_GLOBAL(0x9DE518, sint32) |= (1 << 6);

		show_gridlines();
		show_land_rights();
		show_construction_rights();
		break;

	case WIDX_ROTATE_90:
		++window_scenery_rotation;
		window_scenery_rotation &= 3;
		break;

	case WIDX_PEOPLE_STARTING_POSITION:
		if (!tool_set(var_w, var_idx, 2)) // jb nullsub_52
			break;

		RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) = 0;

		if (RCT2_GLOBAL(RCT2_ADDRESS_PEEP_SPAWNS, sint16) != -1 && RCT2_GLOBAL(0x13573F8, sint16) != -1)
			RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) = 1;

		show_gridlines();
		show_land_rights();
		show_construction_rights();
		break;

	default:
		if (var_idx >= WIDX_PEOPLE_TAB && var_idx <= WIDX_RIDES_TAB)
		{
			var_idx -= WIDX_PEOPLE_TAB;
			if (var_idx == var_w->selected_tab)
				break;

			var_w->selected_tab = var_idx;
			var_w->list_information_type = 0;
		}
		break;

	}
	
	return;
}

/**
*
*  rct2: 0x0068D040
*/
static void window_map_mousedown(int widgetIndex, rct_window*w, rct_widget* widget)
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
	window_map_invalidate();

	#ifdef _MSC_VER
	__asm mov ecx, 512
	__asm mov edx, 512
	#else
	__asm__ ( "mov ecx, 512 " );
	__asm__ ( "mov edx, 512 " );
	#endif
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

	window_get_register(w);

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

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);

	// guest tab image (animated)
	image_id = SPR_TAB_GUESTS_0;
	if (w->selected_tab == 0)
		image_id += w->list_information_type / 4;

	gfx_draw_sprite(dpi, image_id, 
		w->x + w->widgets[WIDX_PEOPLE_TAB].left, 
		w->y + w->widgets[WIDX_PEOPLE_TAB].top, 0);

	// ride/stall tab image (animated)
	image_id = SPR_TAB_RIDE_0;
	if (w->selected_tab == 1)
		image_id += w->list_information_type / 4;

	gfx_draw_sprite(dpi, image_id,
		w->x + w->widgets[WIDX_RIDES_TAB].left,
		w->y + w->widgets[WIDX_RIDES_TAB].top, 0);

	// people starting position (scenario editor only)
	if (w->widgets[WIDX_PEOPLE_STARTING_POSITION].type != 0) {
		gfx_draw_sprite(dpi, 0x0B6E0190A,
			w->x + w->widgets[WIDX_PEOPLE_STARTING_POSITION].left + 12,
			w->y + w->widgets[WIDX_PEOPLE_STARTING_POSITION].top + 18, 0);
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

/*
*
*  rct2: 0x0068D140
*/
static void window_map_tooltip()
{
	RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, short) = 0xC55;
}

/**
*
*  rct2: 0x0068CF23
*/
static void window_map_scrollpaint()
{
	//RCT2_CALLPROC_EBPSAFE(0x0068CF23);

	rct_window *w;
	rct_drawpixelinfo *dpi;
	rct_g1_element *g1_element, pushed_g1_element;

	window_paint_get_registers(w, dpi);

	gfx_clear(dpi, 0x0A0A0A0A);

	g1_element = RCT2_ADDRESS(RCT2_ADDRESS_G1_ELEMENTS, rct_g1_element);
	pushed_g1_element = *g1_element;

	g1_element->offset = RCT2_GLOBAL(RCT2_ADDRESS_MAP_IMAGE_DATA, uint8*);
	g1_element->width = 0x200;
	g1_element->height = 0x200;
	g1_element->x_offset = 0xFFF8;
	g1_element->y_offset = 0xFFF8;
	g1_element->flags = 0;

	gfx_draw_sprite(dpi, 0, 0, 0, 0);

	*g1_element = pushed_g1_element;

	if (w->selected_tab == 0)
		RCT2_CALLPROC_EBPSAFE(0x68DADA);	//draws dots representing guests
	else
		RCT2_CALLPROC_EBPSAFE(0x68DBC1);	//draws dots representing trains
	
	RCT2_CALLPROC_EBPSAFE(0x68D8CE);	//draws the HUD rectangle on the map
}

/**
*
*  rct2: 0x0068CA6C
*/
static void window_map_init_map() 
{
	memset(RCT2_GLOBAL(RCT2_ADDRESS_MAP_IMAGE_DATA, void*), 0x0A0A0A0A, 256 * 256 * sizeof(uint32));
	RCT2_GLOBAL(0x00F1AD6C, uint32) = 0;
}

/**
*
*  rct2: 0x0068C990
*/
static void sub_68C990()
{
	rct_window *w = window_get_main();
	rct_window *w_map;
	sint16 ax, bx, cx, dx;
	sint16 bp, di;

	if (w == NULL || w->viewport == NULL)
		return;

	w_map = window_find_by_id(WC_MAP, 0);
	if (w_map == NULL)
		return;

	cx = ((w->viewport->view_width >> 1) + w->viewport->view_x) >> 5;
	dx = ((w->viewport->view_height >> 1) + w->viewport->view_y) >> 4;
	cx += RCT2_GLOBAL(0x00981BBC + (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8) * 4), uint16);
	dx += RCT2_GLOBAL(0x00981BBE + (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8) * 4), uint16);
	
	ax = w_map->widgets[WIDX_MAP].right - w_map->widgets[WIDX_MAP].left - 11;
	bx = w_map->widgets[WIDX_MAP].bottom - w_map->widgets[WIDX_MAP].top - 11;
	bp = ax;
	di = bx;

	ax >>= 1;
	bx >>= 1;
	cx = (cx - ax) > 0 ? cx - ax : 0;
	dx = (dx - bx) > 0 ? dx - bx : 0;

	bp = -bp;	// asm: neg bp
	di = -di;
	bp += w_map->scrolls[0].h_right;
	di += w_map->scrolls[0].v_bottom;

	if (bp < 0 && (bp - cx) < 0)
		cx = 0;

	if (di < 0 && (di - dx) < 0)
		dx = 0;

	w_map->scrolls[0].h_left = cx;
	w_map->scrolls[0].v_top = dx;
	widget_scroll_update_thumbs(w, WIDX_MAP);
}

/**
* ref. by: window_map_scrollmousedown
*  rct2: 0x0068D7DC
*/
void window_map_set_bounds(rct_window* w)
{
	w->flags |= WF_RESIZABLE; // (1 << 8)
	w->min_width = 245;
	w->max_width = 800;
	w->min_height = 259;
	w->max_height = 560;
}