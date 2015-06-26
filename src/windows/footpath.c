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
#include "../interface/themes.h"
#include "../cheats.h"

enum {
	PATH_CONSTRUCTION_MODE_LAND,
	PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL_TOOL,
	PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL
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

static void window_footpath_close(rct_window *w);
static void window_footpath_mouseup(rct_window *w, int widgetIndex);
static void window_footpath_mousedown(int widgetIndex, rct_window *w, rct_widget *widget);
static void window_footpath_dropdown(rct_window *w, int widgetIndex, int dropdownIndex);
static void window_footpath_update(rct_window *w);
static void window_footpath_toolupdate(rct_window* w, int widgetIndex, int x, int y);
static void window_footpath_tooldown(rct_window* w, int widgetIndex, int x, int y);
static void window_footpath_tooldrag(rct_window* w, int widgetIndex, int x, int y);
static void window_footpath_toolup(rct_window* w, int widgetIndex, int x, int y);
static void window_footpath_invalidate(rct_window *w);
static void window_footpath_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_footpath_events = {
	window_footpath_close,
	window_footpath_mouseup,
	NULL,
	window_footpath_mousedown,
	window_footpath_dropdown,
	NULL,
	window_footpath_update,
	NULL,
	NULL,
	window_footpath_toolupdate,
	window_footpath_tooldown,
	window_footpath_tooldrag,
	window_footpath_toolup,
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
	window_footpath_invalidate,
	window_footpath_paint,
	NULL
};

money32 _window_footpath_cost;
sint8 _window_footpath_provisional_path_arrow_timer;

static void window_footpath_mousedown_direction(int direction);
static void window_footpath_mousedown_slope(int slope);
static void window_footpath_show_footpath_types_dialog(rct_window *w, rct_widget *widget, int showQueues);
static void window_footpath_set_provisional_path_at_point(int x, int y);
static void window_footpath_set_selection_start_bridge_at_point(int screenX, int screenY);
static void window_footpath_place_path_at_point(int x, int y);
static void window_footpath_start_bridge_at_point(int screenX, int screenY);
static void window_footpath_construct();
static void window_footpath_remove();
static void window_footpath_set_enabled_and_pressed_widgets();
static void footpath_get_next_path_info(int *type, int *x, int *y, int *z, int *slope);

/**
 * 
 *  rct2: 0x006A7C43
 */
void window_footpath_open()
{
	rct_window* window;
	sint16 pathId;
	rct_path_type *pathType;

	// Check if window is already open
	window = window_bring_to_front_by_class(WC_FOOTPATH);
	if (window != NULL)
		return;

	window = window_create(
		0,
		29,
		106,
		381,
		&window_footpath_events,
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

	// If a restricted path was selected when the game is no longer in Sandbox mode, reset it
	pathId = RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_PATH_ID, sint16);
	pathType = g_pathTypeEntries[pathId];
	if((pathType->flags & 4) && !gCheatsSandboxMode) {
		RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_PATH_ID, sint16) = 0;
	}

	tool_cancel();
	RCT2_GLOBAL(RCT2_ADDRESS_PATH_CONSTRUCTION_MODE, uint8) = PATH_CONSTRUCTION_MODE_LAND;
	tool_set(window, WIDX_CONSTRUCT_ON_LAND, 17);
	RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) |= INPUT_FLAG_6;
	RCT2_GLOBAL(RCT2_ADDRESS_PATH_ERROR_OCCURED, uint8) = 0;
	window_footpath_set_enabled_and_pressed_widgets();
}

/**
 * 
 *  rct2: 0x006A852F
 */
static void window_footpath_close(rct_window *w)
{
	footpath_provisional_update();
	viewport_set_visibility(0);
	map_invalidate_map_selection_tiles();
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~2;
	window_invalidate_by_class(WC_TOP_TOOLBAR);
	hide_gridlines();
}

/**
 * 
 *  rct2: 0x006A7E92
 */
static void window_footpath_mouseup(rct_window *w, int widgetIndex)
{
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
		footpath_provisional_update();
		map_invalidate_map_selection_tiles();
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~2;
		RCT2_GLOBAL(RCT2_ADDRESS_PATH_CONSTRUCTION_MODE, uint8) = PATH_CONSTRUCTION_MODE_LAND;
		tool_set(w, WIDX_CONSTRUCT_ON_LAND, 17);
		RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) |= INPUT_FLAG_6;
		RCT2_GLOBAL(RCT2_ADDRESS_PATH_ERROR_OCCURED, uint8) = 0;
		window_footpath_set_enabled_and_pressed_widgets();
		break;
	case WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL:
		if (RCT2_GLOBAL(RCT2_ADDRESS_PATH_CONSTRUCTION_MODE, uint8) == PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL_TOOL)
			break;

		_window_footpath_cost = MONEY32_UNDEFINED;
		tool_cancel();
		footpath_provisional_update();
		map_invalidate_map_selection_tiles();
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~2;
		RCT2_GLOBAL(RCT2_ADDRESS_PATH_CONSTRUCTION_MODE, uint8) = PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL_TOOL;
		tool_set(w, WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL, 12);
		RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) |= INPUT_FLAG_6;
		RCT2_GLOBAL(RCT2_ADDRESS_PATH_ERROR_OCCURED, uint8) = 0;
		window_footpath_set_enabled_and_pressed_widgets();
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
		window_footpath_mousedown_direction(0);
		break;
	case WIDX_DIRECTION_NE:
		window_footpath_mousedown_direction(1);
		break;
	case WIDX_DIRECTION_SW:
		window_footpath_mousedown_direction(2);
		break;
	case WIDX_DIRECTION_SE:
		window_footpath_mousedown_direction(3);
		break;
	case WIDX_SLOPEDOWN:
		window_footpath_mousedown_slope(6);
		break;
	case WIDX_LEVEL:
		window_footpath_mousedown_slope(0);
		break;
	case WIDX_SLOPEUP:
		window_footpath_mousedown_slope(2);
		break;
	}
}

/**
 * 
 *  rct2: 0x006A7F18
 */
static void window_footpath_dropdown(rct_window *w, int widgetIndex, int dropdownIndex)
{
	int i, j, pathId;
	rct_path_type *pathType;

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
		if ((RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode)
			flags = 0;

		j = 0;
		for (i = 0; i < 16; i++) {
			pathType = g_pathTypeEntries[i];
			if (pathType == (rct_path_type*)-1)
				continue;
			if (pathType->flags & flags)
				continue;
			// Skip queue lines of scenario editor-only paths (only applicable when the game is in sandbox mode)
			if(widgetIndex == WIDX_QUEUELINE_TYPE && (pathType->flags & 4))
				continue;

			if (j == pathId)
				break;
			j++;
		}
		pathId = i;
	}

	// Set selected path id
	RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_PATH_ID, sint16) = pathId;
	footpath_provisional_update();
	_window_footpath_cost = MONEY32_UNDEFINED;
	window_invalidate(w);
}

/**
 * 
 *  rct2: 0x006A8032
 */
static void window_footpath_toolupdate(rct_window* w, int widgetIndex, int x, int y)
{
	if (widgetIndex == WIDX_CONSTRUCT_ON_LAND) {
		window_footpath_set_provisional_path_at_point(x, y);
	} else if (widgetIndex == WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL) {
		window_footpath_set_selection_start_bridge_at_point(x, y);
	}
}

/**
 * 
 *  rct2: 0x006A8047
 */
static void window_footpath_tooldown(rct_window* w, int widgetIndex, int x, int y)
{
	if (widgetIndex == WIDX_CONSTRUCT_ON_LAND)
		window_footpath_place_path_at_point(x, y);
	else if (widgetIndex == WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL)
		window_footpath_start_bridge_at_point(x, y);
}

/**
 * 
 *  rct2: 0x006A8067
 */
static void window_footpath_tooldrag(rct_window* w, int widgetIndex, int x, int y)
{
	if (widgetIndex == WIDX_CONSTRUCT_ON_LAND) {
		window_footpath_place_path_at_point(x, y);
	}
}

/**
 * 
 *  rct2: 0x006A8066
 */
static void window_footpath_toolup(rct_window* w, int widgetIndex, int x, int y)
{
	if (widgetIndex == WIDX_CONSTRUCT_ON_LAND) {
		// The function at 0x006A8380 in rct2 is just the following:
		RCT2_GLOBAL(RCT2_ADDRESS_PATH_ERROR_OCCURED, uint8) = 0;
	}
}

/**
 * 
 *  rct2: 0x006A7760
 */
static void window_footpath_update_provisional_path_for_bridge_mode(rct_window *w)
{
	int type, x, y, z, slope;

	if (RCT2_GLOBAL(RCT2_ADDRESS_PATH_CONSTRUCTION_MODE, uint8) != PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL)
		return;

	// Update provisional bridge mode path
	if (!(RCT2_GLOBAL(RCT2_ADDRESS_PROVISIONAL_PATH_FLAGS, uint8) & (1 << 1))) {
		footpath_get_next_path_info(&type, &x, &y, &z, &slope);
		_window_footpath_cost = footpath_provisional_set(type, x, y, z, slope);
		widget_invalidate(w, WIDX_CONSTRUCT);
	}

	// Update little directional arrow on provisional bridge mode path
	if (--_window_footpath_provisional_path_arrow_timer < 0) {
		_window_footpath_provisional_path_arrow_timer = 5;
		RCT2_GLOBAL(RCT2_ADDRESS_PROVISIONAL_PATH_FLAGS, uint8) ^= PROVISIONAL_PATH_FLAG_SHOW_ARROW;
		footpath_get_next_path_info(&type, &x, &y, &z, &slope);
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_X, uint16) = x;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_Y, uint16) = y;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_Z, uint16) = z * 8;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_DIRECTION, uint8) = RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_DIRECTION, uint8);
		if (RCT2_GLOBAL(RCT2_ADDRESS_PROVISIONAL_PATH_FLAGS, uint8) & PROVISIONAL_PATH_FLAG_SHOW_ARROW)
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) |= (1 << 2);
		else
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~(1 << 2);
		map_invalidate_tile_full(x, y);
	}
}

/**
 * 
 *  rct2: 0x006A84BB
 */
static void window_footpath_update(rct_window *w)
{
	widget_invalidate(w, WIDX_CONSTRUCT);
	window_footpath_update_provisional_path_for_bridge_mode(w);

	// Check tool
	if (RCT2_GLOBAL(RCT2_ADDRESS_PATH_CONSTRUCTION_MODE, uint8) == PATH_CONSTRUCTION_MODE_LAND) {
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) & INPUT_FLAG_TOOL_ACTIVE))
			window_close(w);
		else if (RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass) != WC_FOOTPATH)
			window_close(w);
		else if (RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, uint16) != WIDX_CONSTRUCT_ON_LAND)
			window_close(w);
	} else if (RCT2_GLOBAL(RCT2_ADDRESS_PATH_CONSTRUCTION_MODE, uint8) == PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL_TOOL) {
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
static void window_footpath_invalidate(rct_window *w)
{
	int selectedPath;
	rct_path_type *pathType;

	colour_scheme_update(w);
	
	// Press / unpress footpath and queue type buttons
	w->pressed_widgets &= ~(1 << WIDX_FOOTPATH_TYPE);
	w->pressed_widgets &= ~(1 << WIDX_QUEUELINE_TYPE);
	w->pressed_widgets |= RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_PATH_TYPE, uint8) == SELECTED_PATH_TYPE_NORMAL ?
		(1 << WIDX_FOOTPATH_TYPE) :
		(1 << WIDX_QUEUELINE_TYPE);

	// Enable / disable construct button
	window_footpath_widgets[WIDX_CONSTRUCT].type =
		RCT2_GLOBAL(RCT2_ADDRESS_PATH_CONSTRUCTION_MODE, uint8) == PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL ?
			WWT_IMGBTN : WWT_EMPTY;

	// Set footpath and queue type button images
	selectedPath = RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_PATH_ID, uint16);
	pathType = g_pathTypeEntries[selectedPath];

	int pathImage = 71 + pathType->image;
	window_footpath_widgets[WIDX_FOOTPATH_TYPE].image = pathImage;

	// Disable queue line button when the path is scenario editor-only (and therefore usually shouldn't have one)
	if(!(pathType->flags & 4)) {
		window_footpath_widgets[WIDX_QUEUELINE_TYPE].image = pathImage + 1;
		window_footpath_widgets[WIDX_QUEUELINE_TYPE].type = WWT_FLATBTN;
	} else {
		window_footpath_widgets[WIDX_QUEUELINE_TYPE].type = WWT_EMPTY;
	}	

	// Disable queue line button if in Scenario Editor
	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR)
		window_footpath_widgets[WIDX_QUEUELINE_TYPE].type = WWT_EMPTY;
}

/**
 * 
 *  rct2: 0x006A7D8B
 */
static void window_footpath_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	int x, y, image, selectedPath;
	rct_path_type *pathType;

	window_draw_widgets(w, dpi);

	if (!(w->disabled_widgets & (1 << WIDX_CONSTRUCT))) {
		// Get construction image
		image = (RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_DIRECTION, uint8) + RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32)) % 4;
		if (RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_SLOPE, uint8) == 2)
			image += 4;
		else if (RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_SLOPE, uint8) == 6)
			image += 8;
		image = RCT2_ADDRESS(0x0098D7E0, uint8)[image];

		selectedPath = RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_PATH_ID, uint16);
		pathType = g_pathTypeEntries[selectedPath];
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
	// If the game is in sandbox mode, also show paths that are normally restricted to the scenario editor, but not their queues (since these usually shouldn't have one)
	if ((RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_SCENARIO_EDITOR) || (gCheatsSandboxMode && !showQueues))
		flags = 0;

	for (i = 0; i < 16; i++) {
		pathType = g_pathTypeEntries[i];
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
 *  rct2: 0x006A8111 0x006A8135 0x006A815C 0x006A8183
 */
static void window_footpath_mousedown_direction(int direction)
{
	footpath_provisional_update();
	RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_DIRECTION, uint8) = (direction - RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32)) & 3;
	_window_footpath_cost = MONEY32_UNDEFINED;
	window_footpath_set_enabled_and_pressed_widgets();
}

/**
*
*  rct2: 0x006A81AA 0x006A81C5 0x006A81E0
*/
static void window_footpath_mousedown_slope(int slope)
{
	footpath_provisional_update();
	RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_SLOPE, uint8) = slope;
	_window_footpath_cost = MONEY32_UNDEFINED;
	window_footpath_set_enabled_and_pressed_widgets();
}

/**
 * 
 *  rct2: 0x006A81FB
 */
static void window_footpath_set_provisional_path_at_point(int x, int y)
{
	int slope, pathType, interactionType;
	rct_map_element *mapElement;

	map_invalidate_selection_rect();
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~(1 << 2);

	rct_xy16 mapCoord = { 0 };
	get_map_coordinates_from_pos(x, y, VIEWPORT_INTERACTION_MASK_FOOTPATH & VIEWPORT_INTERACTION_MASK_TERRAIN, &mapCoord.x, &mapCoord.y, &interactionType, &mapElement, NULL);
	x = mapCoord.x;
	y = mapCoord.y;

	if (interactionType == VIEWPORT_INTERACTION_ITEM_NONE) {
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~(1 << 0);
		footpath_provisional_update();
	} else {
		// Check for change
		if ((RCT2_GLOBAL(RCT2_ADDRESS_PROVISIONAL_PATH_FLAGS, uint8) & (1 << 1)) && RCT2_GLOBAL(RCT2_ADDRESS_PROVISIONAL_PATH_X, uint16) == x && RCT2_GLOBAL(RCT2_ADDRESS_PROVISIONAL_PATH_Y, uint16) == y && RCT2_GLOBAL(RCT2_ADDRESS_PROVISIONAL_PATH_Z, uint8) == mapElement->base_height)
			return;

		// Set map selection
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) |= (1 << 0);
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_TYPE, uint16) = 4;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16) = x;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16) = y;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16) = x;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16) = y;

		footpath_provisional_update();

		// Set provisional path
		slope = RCT2_ADDRESS(0x0098D8B4, uint8)[mapElement->properties.surface.slope & 0x1F];
		if (interactionType == VIEWPORT_INTERACTION_ITEM_FOOTPATH)
			slope = mapElement->properties.surface.slope & 7;
		pathType = (RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_PATH_TYPE, uint8) << 7) + RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_PATH_ID, uint8);

		_window_footpath_cost = footpath_provisional_set(pathType, x, y, mapElement->base_height, slope);
		window_invalidate_by_class(WC_FOOTPATH);
	}
}

/**
*
*  rct2: 0x006A8388
*/
static void window_footpath_set_selection_start_bridge_at_point(int screenX, int screenY)
{
	int x, y, direction;
	rct_map_element *mapElement;

	map_invalidate_selection_rect();
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~(1 << 0) & ~(1 << 2);

	footpath_bridge_get_info_from_pos(screenX, screenY, &x, &y, &direction, &mapElement);
	if (x == (sint16)0x8000)
		return;

	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) |= (1 << 0) | (1 << 2);
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_TYPE, uint16) = 4;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, sint16) = x;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, sint16) = x;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, sint16) = y;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, sint16) = y;

	RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_DIRECTION, uint8) = direction;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_X, uint16) = x;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_Y, uint16) = y;

	int z = mapElement->base_height;

	if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_SURFACE) {
		uint8 slope = mapElement->properties.surface.slope;
		if (slope & 0xf)
			z += 2; // Add 2 for a slope
		if (slope & 0x10)
			z += 2; // Add another 2 for a steep slope
	}

	RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_Z, uint16) = z << 3;

	map_invalidate_selection_rect();
}

/**
 * 
 *  rct2: 0x006A82C5
 */
static void window_footpath_place_path_at_point(int x, int y)
{
	int interactionType, presentType, selectedType, z, cost;
	rct_map_element *mapElement;

	if (RCT2_GLOBAL(RCT2_ADDRESS_PATH_ERROR_OCCURED, uint8) != 0)
		return;

	footpath_provisional_update();

	rct_xy16 mapCoord = { 0 };
	get_map_coordinates_from_pos(x, y, VIEWPORT_INTERACTION_MASK_FOOTPATH & VIEWPORT_INTERACTION_MASK_TERRAIN, &mapCoord.x, &mapCoord.y, &interactionType, &mapElement, NULL);
	x = mapCoord.x;
	y = mapCoord.y;

	if (interactionType == VIEWPORT_INTERACTION_ITEM_NONE)
		return;

	// Set path
	presentType = RCT2_ADDRESS(0x0098D8B4, uint8)[mapElement->properties.path.type & 0x1F];
	if (interactionType == VIEWPORT_INTERACTION_ITEM_FOOTPATH)
		presentType = mapElement->properties.path.type & 7;
	z = mapElement->base_height;
	selectedType = (RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_PATH_TYPE, uint8) << 7) + RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_PATH_ID, uint8);

	// Try and place path
	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, uint16) = STR_CANT_BUILD_FOOTPATH_HERE;
	cost = footpath_place(selectedType, x, y, z, presentType, GAME_COMMAND_FLAG_APPLY);

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
 *  rct2: 0x006A840F
 */
static void window_footpath_start_bridge_at_point(int screenX, int screenY)
{
	int x, y, z, direction;
	rct_map_element *mapElement;

	footpath_bridge_get_info_from_pos(screenX, screenY, &x, &y, &direction, &mapElement);
	if (x == (sint16)0x8000)
		return;

	if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_SURFACE) {
		// If we start the path on a slope, the arrow is slightly raised, so we
		// expect the path to be slightly raised as well.
		uint8_t slope = mapElement->properties.surface.slope;
		z = mapElement->base_height;
		if (slope & 0x10) {
			// Steep diagonal slope
			z += 4;
		} else if (slope & 0x0f) {
			// Normal slope
			z += 2;
		}
	} else {
		z = mapElement->base_height;
		if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_PATH) {
			if (mapElement->properties.path.type & 4) {
				if (direction == (mapElement->properties.path.type & 3))
					z += 2;
			}
		}
	}
		
	tool_cancel();
	RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_X, uint16) = x;
	RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_Y, uint16) = y;
	RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_Z, uint16) = z * 8;
	RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_DIRECTION, uint8) = direction;
	RCT2_GLOBAL(RCT2_ADDRESS_PROVISIONAL_PATH_FLAGS, uint8) = 0;
	_window_footpath_provisional_path_arrow_timer = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_SLOPE, uint8) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_PATH_CONSTRUCTION_MODE, uint8) = PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL;
	RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_VALID_DIRECTIONS, uint8) = 255;
	window_footpath_set_enabled_and_pressed_widgets();
}

/**
 *  Construct a piece of footpath while in bridge building mode.
 *  rct2: 0x006A79B7
 */
static void window_footpath_construct()
{
	_window_footpath_cost = MONEY32_UNDEFINED;
	footpath_provisional_update();

	int type, x, y, z, slope;
	footpath_get_next_path_info(&type, &x, &y, &z, &slope);

	// Try to place the path at the desired location
	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = 0x498;
	money32 cost = footpath_place(type, x, y, z, slope, 0);

	if (cost != MONEY32_UNDEFINED) {
		// It is possible, let's remove walls between the old and new piece of path
		uint8 direction = RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_DIRECTION, uint8);
		map_remove_intersecting_walls(x, y, z, z + 4 + (slope & 0xf ? 2 : 0), direction ^ 2);
		map_remove_intersecting_walls(
			x - TileDirectionDelta[direction].x,
			y - TileDirectionDelta[direction].y,
			z, z + 4, direction
		);
	}

	// Actually place the path now
	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = 0x498;
	cost = footpath_place(type, x, y, z, slope, GAME_COMMAND_FLAG_APPLY);

	if (cost != MONEY32_UNDEFINED) {
		sound_play_panned(
			SOUND_PLACE_ITEM,
			0x8001,
			RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_X, uint16),
			RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_Y, uint16),
			RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_Z, uint16)
		);

		if (RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_SLOPE, uint8) == 0) {
			RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_VALID_DIRECTIONS, uint8) = 0xff;
		} else {
			RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_VALID_DIRECTIONS, uint8) = RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_DIRECTION, uint8);
		}

		if (RCT2_GLOBAL(0x00F3EFA4, uint8) & 2)
			viewport_set_visibility(1);

		// If we have just built an upwards slope, the next path to construct is
		// a bit higher. Note that the z returned by footpath_get_next_path_info
		// already is lowered if we are building a downwards slope.
		if (RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_SLOPE, uint8) == 2)
			z += 2;

		RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_X, uint16) = x;
		RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_Y, uint16) = y;
		RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_Z, uint16) = z << 3;
	}

	window_footpath_set_enabled_and_pressed_widgets();
}

/**
 * 
 *  rct2: 0x006A78EF
 */
static void footpath_remove_map_element(rct_map_element *mapElement)
{
	int x, y, z;

	z = mapElement->base_height;
	int pathType = mapElement->properties.path.type;
	if (pathType & 4) {
		pathType &= 3;
		pathType ^= 2;
		if (pathType == RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_DIRECTION, uint8))
			z += 2;
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
	x = RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_X, uint16) - TileDirectionDelta[edge].x;
	y = RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_Y, uint16) - TileDirectionDelta[edge].y;
	RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_X, uint16) = x;
	RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_Y, uint16) = y;
	RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_Z, uint16) = z << 3;
	RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_DIRECTION, uint8) = edge;
	RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_VALID_DIRECTIONS, uint8) = 255;
}

/**
 * 
 *  rct2: 0x006A7873
 */
static rct_map_element *footpath_get_map_element_to_remove()
{
	rct_map_element *mapElement;
	int x, y, z, zLow;

	x = RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_X, uint16) / 32;
	y = RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_Y, uint16) / 32;
	if (x >= 256 || y >= 256)
		return NULL;

	z = (RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_Z, uint16) >> 3) & 0xFF;
	zLow = z - 2;

	mapElement = map_get_first_element_at(x, y);
	do {
		if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_PATH) {
			if (mapElement->base_height == z) {
				if (mapElement->properties.path.type & 4)
					if (((mapElement->properties.path.type & 3) ^ 2) != RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_DIRECTION, uint8))
						continue;

				return mapElement;
			} else if (mapElement->base_height == zLow) {
				if (!(mapElement->properties.path.type & 4))
					if ((mapElement->properties.path.type & 3) == RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_DIRECTION, uint8))
						continue;
				
				return mapElement;
			}
		}
	} while (!map_element_is_last_for_tile(mapElement++));

	return NULL;
}

/**
 * 
 *  rct2: 0x006A7863
 */
static void window_footpath_remove()
{
	rct_map_element *mapElement;

	_window_footpath_cost = MONEY32_UNDEFINED;
	footpath_provisional_update();

	mapElement = footpath_get_map_element_to_remove();
	if (mapElement != NULL)
		footpath_remove_map_element(mapElement);

	window_footpath_set_enabled_and_pressed_widgets();
}

/**
 * 
 *  rct2: 0x006A855C
 */
static void window_footpath_set_enabled_and_pressed_widgets()
{
	rct_window *w;
	uint64 pressedWidgets, disabledWidgets;
	int currentRotation, direction, slope;

	w = window_find_by_class(WC_FOOTPATH);
	if (w == NULL)
		return;

	if (RCT2_GLOBAL(RCT2_ADDRESS_PATH_CONSTRUCTION_MODE, uint8) == PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL) {
		map_invalidate_map_selection_tiles();
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) |= (1 << 1) | (1 << 3);

		direction = RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_DIRECTION, uint8);
		gMapSelectionTiles[0].x = RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_X, uint16) + TileDirectionDelta[direction].x;
		gMapSelectionTiles[0].y = RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_Y, uint16) + TileDirectionDelta[direction].y;
		gMapSelectionTiles[1].x = -1;
		map_invalidate_map_selection_tiles();
	}

	pressedWidgets = w->pressed_widgets & 0xFFFF887F;
	disabledWidgets = 0;
	currentRotation = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8);
	if (RCT2_GLOBAL(RCT2_ADDRESS_PATH_CONSTRUCTION_MODE, uint8) >= PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL) {
		// Set pressed directional widget
		direction = (RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_DIRECTION, uint8) + currentRotation) & 3;
		pressedWidgets |= (1LL << (WIDX_DIRECTION_NW + direction));

		// Set pressed slope widget
		slope = RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_SLOPE, uint8);
		if (slope == 6)
			pressedWidgets |= (1 << WIDX_SLOPEDOWN);
		else if (slope == 0)
			pressedWidgets |= (1 << WIDX_LEVEL);
		else
			pressedWidgets |= (1 << WIDX_SLOPEUP);

		// Enable / disable directional widgets
		direction = RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_VALID_DIRECTIONS, uint8);
		if (direction != 255) {
			disabledWidgets |=
				(1 << WIDX_DIRECTION_NW) |
				(1 << WIDX_DIRECTION_NE) |
				(1 << WIDX_DIRECTION_SW) |
				(1 << WIDX_DIRECTION_SE);

			direction = (direction + currentRotation) & 3;
			disabledWidgets &= ~(1 << (WIDX_DIRECTION_NW + direction));
		}
	} else {
		// Disable all bridge mode widgets
		disabledWidgets |=
			(1 << WIDX_DIRECTION_GROUP) |
			(1 << WIDX_DIRECTION_NW) |
			(1 << WIDX_DIRECTION_NE) |
			(1 << WIDX_DIRECTION_SW) |
			(1 << WIDX_DIRECTION_SE) |
			(1 << WIDX_SLOPE_GROUP) |
			(1 << WIDX_SLOPEDOWN) |
			(1 << WIDX_LEVEL) |
			(1 << WIDX_SLOPEUP) |
			(1 << WIDX_CONSTRUCT) |
			(1 << WIDX_REMOVE);
	}

	w->pressed_widgets = pressedWidgets;
	w->disabled_widgets = disabledWidgets;
	window_invalidate(w);
}

/**
 * 
 *  rct2: 0x006A7B20
 */
static void footpath_get_next_path_info(int *type, int *x, int *y, int *z, int *slope)
{
	int direction;

	direction = RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_DIRECTION, uint8);
	*x = RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_X, uint16) + TileDirectionDelta[direction].x;
	*y = RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_Y, uint16) + TileDirectionDelta[direction].y;
	*z = RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_FROM_Z, uint16) / 8;
	*type = (RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_PATH_TYPE, uint8) << 7) + RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_PATH_ID, uint8);
	*slope = 0;
	if (RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_SLOPE, uint8) != 0) {
		*slope = RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_DIRECTION, uint8) | 4;
		if (RCT2_GLOBAL(RCT2_ADDRESS_CONSTRUCT_PATH_SLOPE, uint8) != 2) {
			*z -= 2;
			*slope ^= 2;
		}
	}
}
