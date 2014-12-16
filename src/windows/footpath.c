/*****************************************************************************
 * Copyright (c) 2014 Ted John
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
#include "../game.h"
#include "../localisation/localisation.h"
#include "../input.h"
#include "../sprites.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../interface/viewport.h"
#include "../world/footpath.h"
#include "../world/map.h"
#include "dropdown.h"

enum {
	PATH_CONSTRUCTION_MODE_LAND,
	PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL,
	PATH_CONSTRUCTION_MODE_2
};

enum {
	SELECTED_PATH_TYPE_NORMAL,
	SELECTED_PATH_TYPE_QUEUE
};

enum WINDOW_FOOTPATH_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,

	WIDX_TYPE_GROUP,
	WIDX_FOOTPATH_TYPE,
	WIDX_QUEUELINE_TYPE,

	WIDX_DIRECTION_GROUP,
	WIDX_DIRECTION_NW,
	WIDX_DIRECTION_NE,
	WIDX_DIRECTION_SW,
	WIDX_DIRECTION_SE,

	WIDX_SLOPE_GROUP,
	WIDX_SLOPEDOWN,
	WIDX_LEVEL,
	WIDX_SLOPEUP,
	WIDX_CONSTRUCT,
	WIDX_REMOVE,

	WIDX_MODE_GROUP,
	WIDX_CONSTRUCT_ON_LAND,
	WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL,
};

static rct_widget window_footpath_widgets[] = {
	{ WWT_FRAME,	0,		0,		105,	0,		380,	0x0FFFFFFFF,	STR_NONE },
	{ WWT_CAPTION,	0,		1,		104,	1,		14,		STR_FOOTPATHS,	STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,	0,		93,		103,	2,		13,		STR_CLOSE_X,	STR_CLOSE_WINDOW_TIP },

	// Type group
	{ WWT_GROUPBOX,	0,		3,		102,	17,		71,		STR_TYPE,		STR_NONE },
	{ WWT_FLATBTN,	1,		6,		52,		30,		65,		0xFFFFFFFF,		STR_FOOTPATH_TIP },
	{ WWT_FLATBTN,	1,		53,		99,		30,		65,		0xFFFFFFFF,		STR_QUEUE_LINE_PATH_TIP },

	// Direction group
	{ WWT_GROUPBOX,	0,		3,		102,	75,		151,	STR_DIRECTION,	STR_NONE },
	{ WWT_FLATBTN,	1,		53,		97,		87,		115,	5635,			STR_DIRECTION_TIP },
	{ WWT_FLATBTN,	1,		53,		97,		116,	144,	5636,			STR_DIRECTION_TIP },
	{ WWT_FLATBTN,	1,		8,		52,		116,	144,	5637,			STR_DIRECTION_TIP },
	{ WWT_FLATBTN,	1,		8,		52,		87,		115,	5638,			STR_DIRECTION_TIP },

	// Slope group
	{ WWT_GROUPBOX,	0,		3,		102,	155,	195,	STR_SLOPE,		STR_NONE },
	{ WWT_FLATBTN,	1,		17,		40,		167,	190,	5145,			STR_SLOPE_DOWN_TIP },
	{ WWT_FLATBTN,	1,		41,		64,		167,	190,	5146,			STR_LEVEL_TIP },
	{ WWT_FLATBTN,	1,		65,		88,		167,	190,	5147,			STR_SLOPE_UP_TIP },
	{ WWT_FLATBTN,	1,		8,		97,		202,	291,	0xFFFFFFFF,		STR_CONSTRUCT_THE_SELECTED_FOOTPATH_SECTION_TIP },
	{ WWT_FLATBTN,	1,		30,		75,		295,	318,	5162,			STR_REMOVE_PREVIOUS_FOOTPATH_SECTION_TIP },

	// Mode group
	{ WWT_GROUPBOX,	0,		3,		102,	321,	374,	0xFFFFFFFF,		STR_NONE },
	{ WWT_FLATBTN,	1,		13,		48,		332,	367,	5639,			STR_CONSTRUCT_FOOTPATH_ON_LAND_TIP },
	{ WWT_FLATBTN,	1,		57,		92,		332,	367,	5640,			STR_CONSTRUCT_BRIDGE_OR_TUNNEL_FOOTPATH_TIP },
	{ WIDGETS_END },
};

static void window_footpath_emptysub() { }
static void window_footpath_close();
static void window_footpath_mouseup();
static void window_footpath_mousedown(int widgetIndex, rct_window *w, rct_widget *widget);
static void window_footpath_dropdown();
static void window_footpath_update(rct_window *w);
static void window_footpath_toolupdate();
static void window_footpath_tooldown();
static void window_footpath_tooldrag();
static void window_footpath_toolup();
static void window_footpath_invalidate();
static void window_footpath_paint();

static void* window_footpath_events[] = {
	window_footpath_close,
	window_footpath_mouseup,
	window_footpath_emptysub,
	window_footpath_mousedown,
	window_footpath_dropdown,
	window_footpath_emptysub,
	window_footpath_update,
	window_footpath_emptysub,
	window_footpath_emptysub,
	window_footpath_toolupdate,
	window_footpath_tooldown,
	window_footpath_tooldrag,
	window_footpath_toolup,
	window_footpath_emptysub,
	window_footpath_emptysub,
	window_footpath_emptysub,
	window_footpath_emptysub,
	window_footpath_emptysub,
	window_footpath_emptysub,
	window_footpath_emptysub,
	window_footpath_emptysub,
	window_footpath_emptysub,
	window_footpath_emptysub,
	window_footpath_emptysub,
	window_footpath_emptysub,
	window_footpath_invalidate,
	window_footpath_paint,
	window_footpath_emptysub
};

money32 _window_footpath_cost;

static void window_footpath_show_footpath_types_dialog(rct_window *w, rct_widget *widget, int showQueues);
static void window_footpath_set_provisional_path_at_point(int x, int y);
static void window_footpath_place_path_at_point(int x, int y);
static void window_footpath_construct();
static void window_footpath_remove();

/**
 * 
 *  rct2: 0x006A7C43
 */
void window_footpath_open()
{
	rct_window* window;

	// Check if window is already open
	window = window_bring_to_front_by_class(WC_FOOTPATH);
	if (window != NULL)
		return;

	window = window_create(
		0,
		29,
		106,
		381,
		(uint32*)window_footpath_events,
		WC_FOOTPATH,
		0
	);
	window->widgets = window_footpath_widgets;
	window->enabled_widgets =
		(1 << WIDX_CLOSE) |
		(1 << WIDX_FOOTPATH_TYPE) |
		(1 << WIDX_QUEUELINE_TYPE) |
		(1 << WIDX_DIRECTION_NW) |
		(1 << WIDX_DIRECTION_NE) |
		(1 << WIDX_DIRECTION_SW) |
		(1 << WIDX_DIRECTION_SE) |
		(1 << WIDX_SLOPEDOWN) |
		(1 << WIDX_LEVEL) |
		(1 << WIDX_SLOPEUP) |
		(1 << WIDX_CONSTRUCT) |
		(1 << WIDX_REMOVE) |
		(1 << WIDX_CONSTRUCT_ON_LAND) |
		(1 << WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL);

	window_init_scroll_widgets(window);
	window_push_others_right(window);
	show_gridlines();
	window->colours[0] = 24;
	window->colours[1] = 24;
	window->colours[2] = 24;

	tool_cancel();
	RCT2_GLOBAL(RCT2_ADDRESS_PATH_CONSTRUCTION_MODE, uint8) = PATH_CONSTRUCTION_MODE_LAND;
	tool_set(window, WIDX_CONSTRUCT_ON_LAND, 17);
	RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) |= INPUT_FLAG_6;
	RCT2_GLOBAL(RCT2_ADDRESS_PATH_ERROR_OCCURED, uint8) = 0;
	RCT2_CALLPROC_EBPSAFE(0x006A855C);
}

/**
 * 
 *  rct2: 0x006A852F
 */
static void window_footpath_close()
{
	rct_window *w;

	window_get_register(w);

	sub_6A7831();
	viewport_set_visibility(0);
	RCT2_CALLPROC_EBPSAFE(0x0068AB1B);
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~2;
	window_invalidate_by_class(WC_TOP_TOOLBAR);
	hide_gridlines();
}

/**
 * 
 *  rct2: 0x006A7E92
 */
static void window_footpath_mouseup()
{
	short widgetIndex;
	rct_window *w;

	window_widget_get_registers(w, widgetIndex);

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_CONSTRUCT:
		window_footpath_construct();
		break;
	case WIDX_REMOVE:
		window_footpath_remove();
		break;
	case WIDX_CONSTRUCT_ON_LAND:
		if (RCT2_GLOBAL(RCT2_ADDRESS_PATH_CONSTRUCTION_MODE, uint8) == PATH_CONSTRUCTION_MODE_LAND)
			break;

		_window_footpath_cost = MONEY32_UNDEFINED;
		tool_cancel();
		sub_6A7831();
		RCT2_CALLPROC_EBPSAFE(0x0068AB1B);
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~2;
		RCT2_GLOBAL(RCT2_ADDRESS_PATH_CONSTRUCTION_MODE, uint8) = PATH_CONSTRUCTION_MODE_LAND;
		tool_set(w, WIDX_CONSTRUCT_ON_LAND, 17);
		RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) |= INPUT_FLAG_6;
		RCT2_GLOBAL(RCT2_ADDRESS_PATH_ERROR_OCCURED, uint8) = 0;
		RCT2_CALLPROC_EBPSAFE(0x006A855C);
		break;
	case WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL:
		if (RCT2_GLOBAL(RCT2_ADDRESS_PATH_CONSTRUCTION_MODE, uint8) == PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL)
			break;

		_window_footpath_cost = MONEY32_UNDEFINED;
		tool_cancel();
		sub_6A7831();
		RCT2_CALLPROC_EBPSAFE(0x0068AB1B);
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~2;
		RCT2_GLOBAL(RCT2_ADDRESS_PATH_CONSTRUCTION_MODE, uint8) = PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL;
		tool_set(w, WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL, 12);
		RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) |= INPUT_FLAG_6;
		RCT2_GLOBAL(RCT2_ADDRESS_PATH_ERROR_OCCURED, uint8) = 0;
		RCT2_CALLPROC_EBPSAFE(0x006A855C);
		break;
	}
}

/**
 * 
 *  rct2: 0x006A7EC5
 */
static void window_footpath_mousedown(int widgetIndex, rct_window*w, rct_widget* widget)
{
	switch (widgetIndex) {
	case WIDX_FOOTPATH_TYPE:
		window_footpath_show_footpath_types_dialog(w, widget, 0);
		break;
	case WIDX_QUEUELINE_TYPE:
		window_footpath_show_footpath_types_dialog(w, widget, 1);
		break;
	case WIDX_DIRECTION_NW:
		RCT2_CALLPROC_X(0x006A8111, 0, 0, 0, 0, (int)w, 0, 0);
		break;
	case WIDX_DIRECTION_NE:
		RCT2_CALLPROC_X(0x006A8135, 0, 0, 0, 0, (int)w, 0, 0);
		break;
	case WIDX_DIRECTION_SW:
		RCT2_CALLPROC_X(0x006A815C, 0, 0, 0, 0, (int)w, 0, 0);
		break;
	case WIDX_DIRECTION_SE:
		RCT2_CALLPROC_X(0x006A8183, 0, 0, 0, 0, (int)w, 0, 0);
		break;
	case WIDX_SLOPEDOWN:
		RCT2_CALLPROC_X(0x006A81AA, 0, 0, 0, 0, (int)w, 0, 0);
		break;
	case WIDX_LEVEL:
		RCT2_CALLPROC_X(0x006A81C5, 0, 0, 0, 0, (int)w, 0, 0);
		break;
	case WIDX_SLOPEUP:
		RCT2_CALLPROC_X(0x006A81E0, 0, 0, 0, 0, (int)w, 0, 0);
		break;
	}
}

/**
 * 
 *  rct2: 0x006A7F18
 */
static void window_footpath_dropdown()
{
	int i, j, pathId;
	short dropdownIndex;
	short widgetIndex;
	rct_window *w;
	rct_path_type *pathType;

	window_dropdown_get_registers(w, widgetIndex, dropdownIndex);

	if (widgetIndex == WIDX_FOOTPATH_TYPE)
		RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_PATH_TYPE, uint8) = SELECTED_PATH_TYPE_NORMAL;
	else if (widgetIndex == WIDX_QUEUELINE_TYPE)
		RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_PATH_TYPE, uint8) = SELECTED_PATH_TYPE_QUEUE;
	else
		return;
	
	// Get path id
	pathId = dropdownIndex;
	if (pathId == -1) {
		pathId = RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_PATH_ID, sint16);
	} else {
		int flags = 4;
		if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 2)
			flags = 0;

		j = 0;
		for (i = 0; i < 16; i++) {
			pathType = RCT2_ADDRESS(RCT2_ADDRESS_PATH_TYPES, rct_path_type*)[i];
			if (pathType == (rct_path_type*)-1)
				continue;
			if (pathType->flags & flags)
				continue;

			if (j == pathId)
				break;
			j++;
		}
		pathId = i;
	}

	// Set selected path id
	RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_PATH_ID, sint16) = pathId;
	sub_6A7831();
	_window_footpath_cost = MONEY32_UNDEFINED;
	window_invalidate(w);
}

/**
 * 
 *  rct2: 0x006A8032
 */
static void window_footpath_toolupdate()
{
	short x, y;
	short widgetIndex;
	rct_window *w;

	window_tool_get_registers(w, widgetIndex, x, y);

	if (widgetIndex == WIDX_CONSTRUCT_ON_LAND) {
		window_footpath_set_provisional_path_at_point(x, y);
	} else if (widgetIndex == WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL) {
		RCT2_CALLPROC_X(0x006A8388, x, y, 0, 0, (int)w, 0, 0);
	}
}

/**
 * 
 *  rct2: 0x006A8047
 */
static void window_footpath_tooldown()
{
	short x, y;
	short widgetIndex;
	rct_window *w;

	window_tool_get_registers(w, widgetIndex, x, y);

	if (widgetIndex == WIDX_CONSTRUCT_ON_LAND) {
		window_footpath_place_path_at_point(x, y);
	} else if (widgetIndex == WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL) {
		RCT2_CALLPROC_X(0x006A840F, x, y, 0, 0, (int)w, 0, 0);
	}
}

/**
 * 
 *  rct2: 0x006A8067
 */
static void window_footpath_tooldrag()
{
	short x, y;
	short widgetIndex;
	rct_window *w;

	window_tool_get_registers(w, widgetIndex, x, y);

	if (widgetIndex == WIDX_CONSTRUCT_ON_LAND) {
		RCT2_CALLPROC_X(0x006A82C5, x, y, 0, 0, (int)w, 0, 0);
	}
}

/**
 * 
 *  rct2: 0x006A8066
 */
static void window_footpath_toolup()
{
	short x, y;
	short widgetIndex;
	rct_window *w;

	window_tool_get_registers(w, widgetIndex, x, y);

	if (widgetIndex == WIDX_CONSTRUCT_ON_LAND) {
		RCT2_CALLPROC_X(0x006A8380, x, y, 0, 0, (int)w, 0, 0);
	}
}

/**
 * 
 *  rct2: 0x006A84BB
 */
static void window_footpath_update(rct_window *w)
{
	// Invalidate construct button
	widget_invalidate(w, WIDX_CONSTRUCT);

	RCT2_CALLPROC_EBPSAFE(0x006A7760);

	// Check tool
	if (RCT2_GLOBAL(RCT2_ADDRESS_PATH_CONSTRUCTION_MODE, uint8) == PATH_CONSTRUCTION_MODE_LAND) {
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) & INPUT_FLAG_TOOL_ACTIVE))
			window_close(w);
		else if (RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass) != WC_FOOTPATH)
			window_close(w);
		else if (RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, uint16) != WIDX_CONSTRUCT_ON_LAND)
			window_close(w);
	} else if (RCT2_GLOBAL(RCT2_ADDRESS_PATH_CONSTRUCTION_MODE, uint8) == PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL) {
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) & INPUT_FLAG_TOOL_ACTIVE))
			window_close(w);
		else if (RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass) != WC_FOOTPATH)
			window_close(w);
		else if (RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, uint16) != WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL)
			window_close(w);
	}
}

/**
 * 
 *  rct2: 0x006A7D1C
 */
static void window_footpath_invalidate()
{
	int selectedPath;
	rct_path_type *pathType;
	rct_window *w;

	window_get_register(w);
	
	// Press / unpress footpath and queue type buttons
	w->pressed_widgets &= ~(1 << WIDX_FOOTPATH_TYPE);
	w->pressed_widgets &= ~(1 << WIDX_QUEUELINE_TYPE);
	w->pressed_widgets |= RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_PATH_TYPE, uint8) == SELECTED_PATH_TYPE_NORMAL ?
		(1 << WIDX_FOOTPATH_TYPE) :
		(1 << WIDX_QUEUELINE_TYPE);

	// Enable / disable construct button
	window_footpath_widgets[WIDX_CONSTRUCT].type = RCT2_GLOBAL(RCT2_ADDRESS_PATH_CONSTRUCTION_MODE, uint8) == PATH_CONSTRUCTION_MODE_LAND ? WWT_EMPTY : WWT_IMGBTN;

	// Set footpath and queue type button images
	selectedPath = RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_PATH_ID, uint16);
	pathType = RCT2_ADDRESS(RCT2_ADDRESS_PATH_TYPES, rct_path_type*)[selectedPath];

	int pathImage = 71 + pathType->image;
	window_footpath_widgets[WIDX_FOOTPATH_TYPE].image = pathImage;
	window_footpath_widgets[WIDX_QUEUELINE_TYPE].image = pathImage + 1;
	window_footpath_widgets[WIDX_QUEUELINE_TYPE].type = WWT_FLATBTN;

	// Disable queue in if in editor
	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 2)
		window_footpath_widgets[WIDX_QUEUELINE_TYPE].type = WWT_EMPTY;
}

/**
 * 
 *  rct2: 0x006A7D8B
 */
static void window_footpath_paint()
{
	int x, y, image, selectedPath;
	rct_path_type *pathType;
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);

	if (!(w->disabled_widgets & (1 << WIDX_CONSTRUCT))) {
		// Get construction image
		image = (RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_DIRECTION, uint8) + RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32)) % 4;
		if (RCT2_GLOBAL(0x00F3EF91, uint8) == 2)
			image += 4;
		else if (RCT2_GLOBAL(0x00F3EF91, uint8) == 6)
			image += 8;
		image = RCT2_ADDRESS(0x0098D7E0, uint8)[image];

		selectedPath = RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_PATH_ID, uint16);
		pathType = RCT2_ADDRESS(RCT2_ADDRESS_PATH_TYPES, rct_path_type*)[selectedPath];
		image += pathType->image;
		if (RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_PATH_TYPE, uint8) != SELECTED_PATH_TYPE_NORMAL)
			image += 51;

		// Draw construction image
		x = w->x + (window_footpath_widgets[WIDX_CONSTRUCT].left + window_footpath_widgets[WIDX_CONSTRUCT].right) / 2;
		y = w->y + window_footpath_widgets[WIDX_CONSTRUCT].bottom - 60;
		gfx_draw_sprite(dpi, image, x, y, 0);

		// Draw build this... label
		x = w->x + (window_footpath_widgets[WIDX_CONSTRUCT].left + window_footpath_widgets[WIDX_CONSTRUCT].right) / 2;
		y = w->y + window_footpath_widgets[WIDX_CONSTRUCT].bottom - 23;
		gfx_draw_string_centred(dpi, STR_BUILD_THIS, x, y, 0, 0);
	}

	// Draw cost
	x = w->x + (window_footpath_widgets[WIDX_CONSTRUCT].left + window_footpath_widgets[WIDX_CONSTRUCT].right) / 2;
	y = w->y + window_footpath_widgets[WIDX_CONSTRUCT].bottom - 12;
	if (_window_footpath_cost != MONEY32_UNDEFINED)
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY))
			gfx_draw_string_centred(dpi, STR_COST_LABEL, x, y, 0, &_window_footpath_cost);
}

/**
 * 
 *  rct2: 0x006A7F88
 */
static void window_footpath_show_footpath_types_dialog(rct_window *w, rct_widget *widget, int showQueues)
{
	int i, flags, numPathTypes, image;
	rct_path_type *pathType;

	numPathTypes = 0;
	flags = 4;
	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 2)
		flags = 0;

	for (i = 0; i < 16; i++) {
		pathType = RCT2_ADDRESS(RCT2_ADDRESS_PATH_TYPES, rct_path_type*)[i];
		if (pathType == (rct_path_type*)-1)
			continue;
		if (pathType->flags & flags)
			continue;

		image = pathType->image + 71;
		if (showQueues)
			image++;

		gDropdownItemsFormat[numPathTypes] = -1;
		gDropdownItemsArgs[numPathTypes] = image;
		numPathTypes++;
	}

	window_dropdown_show_image(
		w->x + widget->left, w->y + widget->top, widget->bottom - widget->top + 1,
		w->colours[1],
		0,
		numPathTypes,
		47,
		36,
		gAppropriateImageDropdownItemsPerRow[numPathTypes]
	);
}

/**
 * 
 *  rct2: 0x006A81FB
 */
static void window_footpath_set_provisional_path_at_point(int x, int y)
{
	int z, slope, pathType;
	rct_map_element *mapElement;

	RCT2_CALLPROC_EBPSAFE(0x0068AAE1);
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~4;

	get_map_coordinates_from_pos(x, y, 0xFFDE, &x, &y, &z, &mapElement);

	if (z == 0) {
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~1;
		sub_6A7831();
	} else {
		// Check for change
		if ((RCT2_GLOBAL(RCT2_ADDRESS_PROVISIONAL_PATH_FLAGS, uint8) & 2) && RCT2_GLOBAL(RCT2_ADDRESS_PROVISIONAL_PATH_X, uint16) == x && RCT2_GLOBAL(RCT2_ADDRESS_PROVISIONAL_PATH_Y, uint16) == y && RCT2_GLOBAL(RCT2_ADDRESS_PROVISIONAL_PATH_Z, uint8) == mapElement->base_height)
			return;

		// Set map selection
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) |= 1;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_TYPE, uint16) = 4;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, uint16) = x;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, uint16) = y;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, uint16) = x;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, uint16) = y;

		sub_6A7831();

		// Set provisional path
		slope = RCT2_ADDRESS(0x0098D8B4, uint8)[mapElement->properties.surface.slope & 0x1F];
		if (z == 6)
			slope = mapElement->properties.surface.slope & 7;
		pathType = (RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_PATH_TYPE, uint8) << 7) + RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_PATH_ID, uint8);

		_window_footpath_cost = footpath_provisional_set(pathType, x, y, mapElement->base_height, slope);
		window_invalidate_by_class(WC_FOOTPATH);
	}
}



/**
 * 
 *  rct2: 0x006A82C5
 */
static void window_footpath_place_path_at_point(int x, int y)
{
	int z, presentType, selectedType, cost;
	rct_map_element *mapElement;

	if (RCT2_GLOBAL(RCT2_ADDRESS_PATH_ERROR_OCCURED, uint8) != 0)
		return;

	sub_6A7831();

	get_map_coordinates_from_pos(x, y, 0xFFDE, &x, &y, &z, &mapElement);
	if (z == 0)
		return;

	// Set path
	presentType = RCT2_ADDRESS(0x0098D8B4, uint8)[mapElement->properties.path.type & 0x1F];
	if (z == 6)
		presentType = mapElement->properties.path.type & 7;
	z = mapElement->base_height;
	selectedType = (RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_PATH_TYPE, uint8) << 7) + RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_PATH_ID, uint8);

	// Try and place path
	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, uint16) = STR_CANT_BUILD_FOOTPATH_HERE;
	cost = footpath_place(selectedType, x, y, z, presentType, (1 << 0));

	if (cost == MONEY32_UNDEFINED) {
		RCT2_GLOBAL(RCT2_ADDRESS_PATH_ERROR_OCCURED, uint8) = 1;
	} else if (RCT2_GLOBAL(0x00F3EFD9, uint32) != 0) {
		// bp = 0x009DEA62
		// dx = 0x009DEA60
		// cx = 0x009DEA5E
		sound_play_panned(SOUND_PLACE_ITEM, 0x8001, RCT2_GLOBAL(0x009DEA5E, uint16), RCT2_GLOBAL(0x009DEA60, uint16), RCT2_GLOBAL(0x009DEA62, uint16));
	}
}

/**
 * 
 *  rct2: 0x006A79B7
 */
static void window_footpath_construct()
{
	RCT2_CALLPROC_EBPSAFE(0x006A79B7);
}

/**
 * 
 *  rct2: 0x006A7863
 */
static void window_footpath_remove()
{
	int x, y, lastTile;
	rct_map_element *mapElement;

	// RCT2_CALLPROC_EBPSAFE(0x006A7863);

	_window_footpath_cost = MONEY32_UNDEFINED;
	sub_6A7831();

	x = RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_X, uint16) / 32;
	y = RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_Y, uint16) / 32;
	int dl = (RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_Z, uint16) >> 3) & 0xFF;
	int dh = dl - 2;

	if (x >= 256 || y >= 256)
		goto loc_6A79B0;

	mapElement = RCT2_ADDRESS(RCT2_ADDRESS_TILE_MAP_ELEMENT_POINTERS, rct_map_element*)[y * 256 + x];
	do {
		if ((mapElement->type & MAP_ELEMENT_TYPE_MASK) == MAP_ELEMENT_TYPE_PATH) {
			if (mapElement->base_height == dl) {
				if (mapElement->properties.path.type & 4)
					if (((mapElement->properties.path.type & 3) ^ 2) != RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_DIRECTION, uint8))
						continue;
				goto loc_6A78EF;
			} else if (mapElement->base_height == dh) {
				if (!(mapElement->properties.path.type & 4))
					if ((mapElement->properties.path.type & 3) == RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_DIRECTION, uint8))
						continue;
				goto loc_6A78EF;
			}
		}
		lastTile = (mapElement->flags & MAP_ELEMENT_FLAG_LAST_TILE) != 0;
		mapElement++;
	} while (!lastTile);
	goto loc_6A79B0;

loc_6A78EF:
	dl = mapElement->base_height;
	int pathType = mapElement->properties.path.type;
	if (pathType & 4) {
		pathType &= 3;
		pathType ^= 2;
		if (pathType == RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_DIRECTION, uint8))
			dl += 2;
	}
	
	// Find a connected edge
	int edge = RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_DIRECTION, uint8) ^ 2;
	if (!(mapElement->properties.path.edges & (1 << edge))) {
		edge = (edge + 1) & 3;
		if (!(mapElement->properties.path.edges & (1 << edge))) {
			edge = (edge + 2) & 3;
			if (!(mapElement->properties.path.edges & (1 << edge))) {
				edge = (edge - 1) & 3;
				if (!(mapElement->properties.path.edges & (1 << edge)))
					edge ^= 2;
			}
		}
	}
	
	// Remove path
	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, uint16) = STR_CANT_REMOVE_FOOTPATH_FROM_HERE;
	footpath_remove(
		RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_X, uint16),
		RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_Y, uint16),
		mapElement->base_height,
		1
	);

	// Move selection
	edge ^= 2;
	x = RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_X, uint16) - RCT2_GLOBAL(0x00993CCC + (edge * 4), sint16);
	y = RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_Y, uint16) - RCT2_GLOBAL(0x00993CCE + (edge * 4), sint16);
	RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_X, uint16) = x;
	RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_Y, uint16) = y;
	RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_Z, uint16) = dl << 3;
	RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_DIRECTION, uint8) = edge;
	RCT2_GLOBAL(0x00F3EF9E, uint8) = 255;

loc_6A79B0:
	RCT2_CALLPROC_EBPSAFE(0x006A855C);
}