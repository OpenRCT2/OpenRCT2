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
#include "../cheats.h"
#include "../game.h"
#include "../input.h"
#include "../interface/themes.h"
#include "../interface/viewport.h"
#include "../interface/widget.h"
#include "../localisation/localisation.h"
#include "../rct2.h"
#include "../sprites.h"
#include "../world/footpath.h"
#include "../world/scenery.h"
#include "dropdown.h"

#define FOOTPATH_SCENERY_GROUPBOX_TOP		199
#define FOOTPATH_SCENERY_GROUPBOX_HEIGHT	135
#define FOOTPATH_SCENERY_GROUPBOX_BOTTOM	(FOOTPATH_SCENERY_GROUPBOX_TOP + FOOTPATH_SCENERY_GROUPBOX_HEIGHT)
#define FOOTPATH_SCENERY_IMGBUTTON_SIZE		23
#define FOOTPATH_SCENERY_IMGBUTTON_PADDING	5
#define FOOTPATH_SCENERY_IMGBUTTON_LEFT		(FOOTPATH_SCENERY_LIST_RIGHT + FOOTPATH_SCENERY_IMGBUTTON_PADDING)
#define FOOTPATH_SCENERY_IMGBUTTON_RIGHT	(FOOTPATH_SCENERY_LIST_RIGHT + FOOTPATH_SCENERY_IMGBUTTON_PADDING + FOOTPATH_SCENERY_IMGBUTTON_SIZE)
#define FOOTPATH_SCENERY_IMGBUTTON_BOTTOM	(FOOTPATH_SCENERY_GROUPBOX_TOP + 29 + FOOTPATH_SCENERY_IMGBUTTON_SIZE)
#define FOOTPATH_SCENERY_LIST_RIGHT			(112 - FOOTPATH_SCENERY_IMGBUTTON_SIZE - FOOTPATH_SCENERY_IMGBUTTON_PADDING)
#define FOOTPATH_SCENERY_LIST_WIDTH			(FOOTPATH_SCENERY_LIST_RIGHT - 8)

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

	WIDX_AUTOSCENERY_GROUP,
	WIDX_AUTOSCENERY_DROPDOWN,
	WIDX_AUTOSCENERY_DROPDOWN_BUTTON,
	WIDX_AUTOSCENERY_DROPDOWN_LIST,
	WIDX_AUTOSCENERY_DROPDOWN_LIST_CLEAR,
	WIDX_AUTOSCENERY_OBJECT_DROPDOWN,
	WIDX_AUTOSCENERY_OBJECT_DROPDOWN_BUTTON,
	WIDX_AUTOSCENERY_ADD,

	WIDX_CONSTRUCT,
	WIDX_REMOVE,

	WIDX_MODE_GROUP,
	WIDX_CONSTRUCT_ON_LAND,
	WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL,
};

#define FOOTPATH_SCENERY_PRESET_COUNT				4
#define FOOTPATH_SCENERY_PRESET_MAX_LENGTH	10
#define PATH_BIT_DRAW_TYPE_END							-2
#define PATH_BIT_DRAW_TYPE_NOTHING					-1

static const rct_string_id _footpath_scenery_presets[] = {
	STR_FOOTPATH_SCENERY_NO_SCENERY,
	STR_FOOTPATH_SCENERY_STANDARD,
	STR_FOOTPATH_SCENERY_FOOD_CORNER,
	STR_FOOTPATH_SCENERY_CUSTOM,
};

static const sint8 _footpath_scenery_presets_types[FOOTPATH_SCENERY_PRESET_COUNT][FOOTPATH_SCENERY_PRESET_MAX_LENGTH] = {
	{ PATH_BIT_DRAW_TYPE_END, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ PATH_BIT_DRAW_TYPE_BINS, PATH_BIT_DRAW_TYPE_BENCHES, PATH_BIT_DRAW_TYPE_LIGHTS, PATH_BIT_DRAW_TYPE_BENCHES, PATH_BIT_DRAW_TYPE_BINS, PATH_BIT_DRAW_TYPE_NOTHING, PATH_BIT_DRAW_TYPE_NOTHING, PATH_BIT_DRAW_TYPE_NOTHING, PATH_BIT_DRAW_TYPE_END, 0 },
	{ PATH_BIT_DRAW_TYPE_BINS, PATH_BIT_DRAW_TYPE_BENCHES, PATH_BIT_DRAW_TYPE_BENCHES, PATH_BIT_DRAW_TYPE_BENCHES, PATH_BIT_DRAW_TYPE_BENCHES, PATH_BIT_DRAW_TYPE_BENCHES, PATH_BIT_DRAW_TYPE_END, 0, 0, 0 },
	{ PATH_BIT_DRAW_TYPE_END, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};

typedef struct path_rule_entry
{
	rct_scenery_entry *		 sceneEntry;
	struct path_rule_entry * next;
} path_rule_entry;

static path_rule_entry * _footpath_scenery_list[] = {
	NULL,
	NULL,
	NULL,
	NULL,
};

static rct_widget window_footpath_widgets[] = {
	{ WWT_FRAME,	0,		0,		120,	0,		402 + FOOTPATH_SCENERY_GROUPBOX_HEIGHT,	0xFFFFFFFF,							STR_NONE },
	{ WWT_CAPTION,	0,		1,		119,	1,		14,										STR_FOOTPATHS,						STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,	0,		108,	118,	2,		13,										STR_CLOSE_X,						STR_CLOSE_WINDOW_TIP },

	// Type group
	{ WWT_GROUPBOX,	0,		3,		117,	17,		71,										STR_TYPE,							STR_NONE },
	{ WWT_FLATBTN,	1,		10,		56,		30,		65,										0xFFFFFFFF,							STR_FOOTPATH_TIP },
	{ WWT_FLATBTN,	1,		65,		110,	30,		65,										0xFFFFFFFF,							STR_QUEUE_LINE_PATH_TIP },

	// Direction group
	{ WWT_GROUPBOX,	0,		3,		117,	75,		151,									STR_DIRECTION,						STR_NONE },
	{ WWT_FLATBTN,	1,		63,		107,	87,		115,									SPR_CONSTRUCTION_DIRECTION_NE,		STR_DIRECTION_TIP },
	{ WWT_FLATBTN,	1,		63,		107,	116,	144,									SPR_CONSTRUCTION_DIRECTION_SE,		STR_DIRECTION_TIP },
	{ WWT_FLATBTN,	1,		13,		57,		116,	144,									SPR_CONSTRUCTION_DIRECTION_SW,		STR_DIRECTION_TIP },
	{ WWT_FLATBTN,	1,		13,		57,		87,		115,									SPR_CONSTRUCTION_DIRECTION_NW,		STR_DIRECTION_TIP },

	// Slope group
	{ WWT_GROUPBOX,	0,		3,		117,	155,	195,									STR_SLOPE,							STR_NONE },
	{ WWT_FLATBTN,	1,		24,		47,		167,	190,									SPR_RIDE_CONSTRUCTION_SLOPE_DOWN,	STR_SLOPE_DOWN_TIP },
	{ WWT_FLATBTN,	1,		48,		71,		167,	190,									SPR_RIDE_CONSTRUCTION_SLOPE_LEVEL,	STR_LEVEL_TIP },
	{ WWT_FLATBTN,	1,		72,		95,		167,	190,									SPR_RIDE_CONSTRUCTION_SLOPE_UP,		STR_SLOPE_UP_TIP },

	// Autoscenery group
	{ WWT_GROUPBOX,			0,		3,									117,								FOOTPATH_SCENERY_GROUPBOX_TOP,			FOOTPATH_SCENERY_GROUPBOX_BOTTOM,		STR_FOOTPATH_SCENERY,		STR_NONE },
	{ WWT_DROPDOWN, 		1,		8,									112,								FOOTPATH_SCENERY_GROUPBOX_TOP + 13,		FOOTPATH_SCENERY_GROUPBOX_TOP + 23,		STR_STRINGID,				STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,		101,								111,								FOOTPATH_SCENERY_GROUPBOX_TOP + 14,		FOOTPATH_SCENERY_GROUPBOX_TOP + 22,		STR_DROPDOWN_GLYPH,			STR_NONE },
	{ WWT_SCROLL,			1,		8,									FOOTPATH_SCENERY_LIST_RIGHT,		FOOTPATH_SCENERY_GROUPBOX_TOP + 29,		FOOTPATH_SCENERY_GROUPBOX_TOP + 90,		SCROLL_VERTICAL,			STR_NONE },
	{ WWT_FLATBTN,			0,		FOOTPATH_SCENERY_IMGBUTTON_LEFT,	FOOTPATH_SCENERY_IMGBUTTON_RIGHT,	FOOTPATH_SCENERY_GROUPBOX_TOP + 29,		FOOTPATH_SCENERY_IMGBUTTON_BOTTOM,		SPR_DEMOLISH,				STR_FOOTPATH_SCENERY_DELETE_RULES },
	{ WWT_DROPDOWN, 		1,		8,									112,								FOOTPATH_SCENERY_GROUPBOX_TOP + 101,	FOOTPATH_SCENERY_GROUPBOX_TOP + 111,	STR_ARG_2_STRINGID,			STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,		101,								111,								FOOTPATH_SCENERY_GROUPBOX_TOP + 102,	FOOTPATH_SCENERY_GROUPBOX_TOP + 110,	STR_DROPDOWN_GLYPH,			STR_NONE },
	{ WWT_CLOSEBOX,			1,		8,									112,								FOOTPATH_SCENERY_GROUPBOX_TOP + 114,	FOOTPATH_SCENERY_GROUPBOX_TOP + 130,	STR_FOOTPATH_SCENERY_ADD,	STR_NONE },

	// Construct/Demolish
	{ WWT_FLATBTN,	1,		8,		112,	FOOTPATH_SCENERY_GROUPBOX_BOTTOM + 7,	FOOTPATH_SCENERY_GROUPBOX_BOTTOM + 111,	0xFFFFFFFF,							STR_CONSTRUCT_THE_SELECTED_FOOTPATH_SECTION_TIP },
	{ WWT_FLATBTN,	1,		37,		82,		FOOTPATH_SCENERY_GROUPBOX_BOTTOM + 115,	FOOTPATH_SCENERY_GROUPBOX_BOTTOM + 138,	SPR_DEMOLISH_CURRENT_SECTION,		STR_REMOVE_PREVIOUS_FOOTPATH_SECTION_TIP },

	// Mode group
	{ WWT_GROUPBOX,	0,		3,		117,	FOOTPATH_SCENERY_GROUPBOX_BOTTOM + 141,	FOOTPATH_SCENERY_GROUPBOX_BOTTOM + 194,	0xFFFFFFFF,							STR_NONE },
	{ WWT_FLATBTN,	1,		18,		53,		FOOTPATH_SCENERY_GROUPBOX_BOTTOM + 152,	FOOTPATH_SCENERY_GROUPBOX_BOTTOM + 187,	SPR_CONSTRUCTION_FOOTPATH_LAND,		STR_CONSTRUCT_FOOTPATH_ON_LAND_TIP },
	{ WWT_FLATBTN,	1,		68,		103,	FOOTPATH_SCENERY_GROUPBOX_BOTTOM + 152,	FOOTPATH_SCENERY_GROUPBOX_BOTTOM + 187,	SPR_CONSTRUCTION_FOOTPATH_BRIDGE,	STR_CONSTRUCT_BRIDGE_OR_TUNNEL_FOOTPATH_TIP },
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
static void window_footpath_list_scrollgetsize(rct_window * w, int scrollIndex, int * width, int * height);
static void window_footpath_list_scrollmousedown(rct_window *w, int scrollIndex, int x, int y);
static void window_footpath_invalidate(rct_window *w);
static void window_footpath_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_footpath_scrollpaint(rct_window * w, rct_drawpixelinfo * dpi, int scrollIndex);

static sint32 window_footpath_pathimage(rct_window *w);

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
	window_footpath_list_scrollgetsize,
	window_footpath_list_scrollmousedown,
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
	window_footpath_scrollpaint
};

static money32 _window_footpath_cost;
static sint8 _window_footpath_provisional_path_arrow_timer;
static uint8 _lastUpdatedCameraRotation = UINT8_MAX;
static bool _footpathErrorOccured;

static int _window_footpath_scenery_preset = 0;
static rct_string_id _window_footpath_scenery_custom_selected_id = 0;
static rct_scenery_entry * _window_footpath_scenery_selected_sceneryEntry = NULL;
static path_rule_entry * _footpath_selected_listEntry = NULL;

enum
{
	FOOTHPATH_IS_SLOPED = (1 << 2),
	IRREGULAR_SLOPE = (1 << 3),
};

/** rct2: 0x0098D8B4 */
const uint8 default_path_slope[] = {
	0,
	IRREGULAR_SLOPE,
	IRREGULAR_SLOPE,
	FOOTHPATH_IS_SLOPED | 2,
	IRREGULAR_SLOPE,
	IRREGULAR_SLOPE,
	FOOTHPATH_IS_SLOPED | 3,
	IRREGULAR_SLOPE,
	IRREGULAR_SLOPE,
	FOOTHPATH_IS_SLOPED | 1,
	IRREGULAR_SLOPE,
	IRREGULAR_SLOPE,
	FOOTHPATH_IS_SLOPED | 0,
	IRREGULAR_SLOPE,
	IRREGULAR_SLOPE,
	IRREGULAR_SLOPE,
	IRREGULAR_SLOPE,
	IRREGULAR_SLOPE,
	IRREGULAR_SLOPE,
	IRREGULAR_SLOPE,
	IRREGULAR_SLOPE,
	IRREGULAR_SLOPE,
	IRREGULAR_SLOPE,
	IRREGULAR_SLOPE,
	IRREGULAR_SLOPE,
	IRREGULAR_SLOPE,
	IRREGULAR_SLOPE,
	IRREGULAR_SLOPE,
	IRREGULAR_SLOPE,
	IRREGULAR_SLOPE,
	IRREGULAR_SLOPE,
	IRREGULAR_SLOPE,
};

/** rct2: 0x0098D7E0 */
unsigned char footpath_construction_preview_images[][4] = {
	{5,  10, 5,  10}, // Flat
	{16, 17, 18, 19}, // Upwards
	{18, 19, 16, 17}, // Downwards
};

static void window_footpath_mousedown_direction(int direction);
static void window_footpath_mousedown_slope(int slope);
static void window_footpath_autoscenery_mousedown_preset(rct_window *w, rct_widget* widget);
static void window_footpath_autoscenery_mousedown_object(rct_window *w, rct_widget* widget);
static void window_footpath_show_footpath_types_dialog(rct_window *w, rct_widget *widget, int showQueues);
static void window_footpath_set_provisional_path_at_point(int x, int y);
static void window_footpath_set_selection_start_bridge_at_point(int screenX, int screenY);
static void window_footpath_place_path_at_point(int x, int y);
static void window_footpath_start_bridge_at_point(int screenX, int screenY);
static void window_footpath_construct();
static void window_footpath_remove();
static void window_footpath_set_enabled_and_pressed_widgets();
static void footpath_get_next_path_info(int *type, int *x, int *y, int *z, int *slope);
static void footpath_select_default();
static void window_footpath_update_selected_object_string();

static void window_footpath_scenery_init_presets();
static struct rct_scenery_entry* window_footpath_scenery_get_scenery_entry_for_type(sint8 type);
static void window_footpath_update_listselection(int index);

/**
 *
 *  rct2: 0x006A7C43
 */
void window_footpath_open()
{
	// Check if window is already open
	rct_window *window = window_bring_to_front_by_class(WC_FOOTPATH);
	if (window != NULL)
		return;

	window = window_create(
		0,
		29,
		121,
		(403 + FOOTPATH_SCENERY_GROUPBOX_HEIGHT),
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
		(1 << WIDX_AUTOSCENERY_GROUP) |
		(1 << WIDX_AUTOSCENERY_DROPDOWN) |
		(1 << WIDX_AUTOSCENERY_DROPDOWN_BUTTON) |
		(1 << WIDX_AUTOSCENERY_DROPDOWN_LIST) |
		(1 << WIDX_AUTOSCENERY_DROPDOWN_LIST_CLEAR) |
		(1 << WIDX_AUTOSCENERY_OBJECT_DROPDOWN) |
		(1 << WIDX_AUTOSCENERY_OBJECT_DROPDOWN_BUTTON) |
		(1 << WIDX_AUTOSCENERY_ADD) |
		(1 << WIDX_CONSTRUCT) |
		(1 << WIDX_REMOVE) |
		(1 << WIDX_CONSTRUCT_ON_LAND) |
		(1 << WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL);

	window->no_list_items = 0;
	window->selected_list_item = -1;
	window_init_scroll_widgets(window);
	window_push_others_right(window);
	show_gridlines();

	window_footpath_scenery_init_presets();

	// If a restricted path was selected when the game is no longer in Sandbox mode, reset it
	rct_footpath_entry *pathEntry = get_footpath_entry(gFootpathSelectedId);
	if (pathEntry != (rct_footpath_entry*)-1 && (pathEntry->flags & 4) && !gCheatsSandboxMode) {
		pathEntry = (rct_footpath_entry*)-1;
	}

	// Select the default path if we don't have one
	if (pathEntry == (rct_footpath_entry*)-1) {
		footpath_select_default();
	}

	tool_cancel();
	gFootpathConstructionMode = PATH_CONSTRUCTION_MODE_LAND;
	tool_set(window, WIDX_CONSTRUCT_ON_LAND, 17);
	gInputFlags |= INPUT_FLAG_6;
	_footpathErrorOccured = false;
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
	gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
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
		if (gFootpathConstructionMode == PATH_CONSTRUCTION_MODE_LAND)
			break;

		_window_footpath_cost = MONEY32_UNDEFINED;
		tool_cancel();
		footpath_provisional_update();
		map_invalidate_map_selection_tiles();
		gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
		gFootpathConstructionMode = PATH_CONSTRUCTION_MODE_LAND;
		tool_set(w, WIDX_CONSTRUCT_ON_LAND, 17);
		gInputFlags |= INPUT_FLAG_6;
		_footpathErrorOccured = false;
		window_footpath_set_enabled_and_pressed_widgets();
		break;
	case WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL:
		if (gFootpathConstructionMode == PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL_TOOL)
			break;

		_window_footpath_cost = MONEY32_UNDEFINED;
		tool_cancel();
		footpath_provisional_update();
		map_invalidate_map_selection_tiles();
		gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
		gFootpathConstructionMode = PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL_TOOL;
		tool_set(w, WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL, 12);
		gInputFlags |= INPUT_FLAG_6;
		_footpathErrorOccured = false;
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
	case WIDX_AUTOSCENERY_DROPDOWN_BUTTON:
		window_footpath_autoscenery_mousedown_preset(w, widget);
		break;
	case WIDX_AUTOSCENERY_OBJECT_DROPDOWN_BUTTON:
		window_footpath_autoscenery_mousedown_object(w, widget);
		break;
	}
}

/**
 *
 *  rct2: 0x006A7F18
 */
static void window_footpath_dropdown(rct_window *w, int widgetIndex, int dropdownIndex)
{
	path_rule_entry *c;
	uint8 n = 0;

	switch (widgetIndex) {
		case WIDX_FOOTPATH_TYPE:
			gFootpathSelectedType = SELECTED_PATH_TYPE_NORMAL;
			break;
		case WIDX_QUEUELINE_TYPE:
			gFootpathSelectedType = SELECTED_PATH_TYPE_QUEUE;
			break;
		case WIDX_AUTOSCENERY_DROPDOWN_BUTTON:
			if (dropdownIndex != -1) {
				_window_footpath_scenery_preset = dropdownIndex;
				c = _footpath_scenery_list[_window_footpath_scenery_preset];
				while (c != NULL) {
					n++;
					c = c->next;
				}
				w->no_list_items = n;
				_footpath_selected_listEntry = _footpath_scenery_list[_window_footpath_scenery_preset];
				if (_footpath_selected_listEntry != NULL) {
					w->selected_list_item = 0;
				} else {
					w->selected_list_item = -1;
				}
				window_footpath_set_enabled_and_pressed_widgets();
				window_invalidate(w);
			}
			return;
		case WIDX_AUTOSCENERY_OBJECT_DROPDOWN_BUTTON:
			if (dropdownIndex != -1) {
				_window_footpath_scenery_custom_selected_id = dropdownIndex;
				window_footpath_update_selected_object_string();
				window_footpath_set_enabled_and_pressed_widgets();
				window_invalidate(w);
			}
		default:
			return;
	}

	// Get path id
	int pathId = dropdownIndex;
	if (pathId == -1) {
		pathId = gFootpathSelectedId;
	} else {
		int flags = 4;
		if ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || gCheatsSandboxMode)
			flags = 0;

		int i = 0, j = 0;
		for (; i < 16; i++) {
			rct_footpath_entry *pathType = get_footpath_entry(i);
			if (pathType == (rct_footpath_entry*)-1)
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
	gFootpathSelectedId = pathId;
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
		_footpathErrorOccured = false;
	}
}

/**
 *
 *  rct2: 0x006A7760
 */
static void window_footpath_update_provisional_path_for_bridge_mode(rct_window *w)
{
	int type, x, y, z, slope;

	if (gFootpathConstructionMode != PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL)
		return;

	// Recheck area for construction. Set by ride_construction window
	if (gFootpathProvisionalFlags & PROVISIONAL_PATH_FLAG_2) {
		footpath_provisional_remove();
		gFootpathProvisionalFlags &= ~PROVISIONAL_PATH_FLAG_2;
	}

	// Update provisional bridge mode path
	if (!(gFootpathProvisionalFlags & PROVISIONAL_PATH_FLAG_1)) {
		footpath_get_next_path_info(&type, &x, &y, &z, &slope);
		_window_footpath_cost = footpath_provisional_set(type, x, y, z, slope);
		widget_invalidate(w, WIDX_CONSTRUCT);
	}

	// Update little directional arrow on provisional bridge mode path
	if (--_window_footpath_provisional_path_arrow_timer < 0) {
		_window_footpath_provisional_path_arrow_timer = 5;
		gFootpathProvisionalFlags ^= PROVISIONAL_PATH_FLAG_SHOW_ARROW;
		footpath_get_next_path_info(&type, &x, &y, &z, &slope);
		gMapSelectArrowPosition.x = x;
		gMapSelectArrowPosition.y = y;
		gMapSelectArrowPosition.z = z * 8;
		gMapSelectArrowDirection = gFootpathConstructDirection;
		if (gFootpathProvisionalFlags & PROVISIONAL_PATH_FLAG_SHOW_ARROW)
			gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_ARROW;
		else
			gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
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

	// #2502: The camera might have changed rotation, so we need to update which directional buttons are pressed
	uint8 currentRotation = get_current_rotation();
	if (_lastUpdatedCameraRotation != currentRotation) {
		_lastUpdatedCameraRotation = currentRotation;
		window_footpath_set_enabled_and_pressed_widgets();
	}

	// Check tool
	if (gFootpathConstructionMode == PATH_CONSTRUCTION_MODE_LAND) {
		if (!(gInputFlags & INPUT_FLAG_TOOL_ACTIVE))
			window_close(w);
		else if (gCurrentToolWidget.window_classification != WC_FOOTPATH)
			window_close(w);
		else if (gCurrentToolWidget.widget_index != WIDX_CONSTRUCT_ON_LAND)
			window_close(w);
	} else if (gFootpathConstructionMode == PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL_TOOL) {
		if (!(gInputFlags & INPUT_FLAG_TOOL_ACTIVE))
			window_close(w);
		else if (gCurrentToolWidget.window_classification != WC_FOOTPATH)
			window_close(w);
		else if (gCurrentToolWidget.widget_index != WIDX_CONSTRUCT_BRIDGE_OR_TUNNEL)
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
	rct_footpath_entry *pathType;

	colour_scheme_update(w);

	// Press / unpress footpath and queue type buttons
	w->pressed_widgets &= ~(1 << WIDX_FOOTPATH_TYPE);
	w->pressed_widgets &= ~(1 << WIDX_QUEUELINE_TYPE);
	w->pressed_widgets |= gFootpathSelectedType == SELECTED_PATH_TYPE_NORMAL ?
		(1 << WIDX_FOOTPATH_TYPE) :
		(1 << WIDX_QUEUELINE_TYPE);

	// Enable / disable construct button
	window_footpath_widgets[WIDX_CONSTRUCT].type =
		gFootpathConstructionMode == PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL ?
			WWT_IMGBTN : WWT_EMPTY;

	// Set footpath and queue type button images
	selectedPath = gFootpathSelectedId;
	pathType = get_footpath_entry(selectedPath);

	// TODO: Should probably add constants for object sprites
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
	if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
		window_footpath_widgets[WIDX_QUEUELINE_TYPE].type = WWT_EMPTY;

	// Set selected dropdown items
	set_format_arg(0, rct_string_id, _footpath_scenery_presets[_window_footpath_scenery_preset]);
	if (_window_footpath_scenery_selected_sceneryEntry == NULL) {
		set_format_arg(2, rct_string_id, STR_FOOTPATH_SCENERY_NOTHING);
	} else {
		set_format_arg(2, rct_string_id, _window_footpath_scenery_selected_sceneryEntry->name);
	}
}

static sint32 window_footpath_pathimage(rct_window *w)
{
	uint8 direction = (gFootpathConstructDirection + get_current_rotation()) % 4;
	uint8 slope = 0;
	if (gFootpathConstructSlope == 2)
		slope = 1;
	else if (gFootpathConstructSlope == 6)
		slope = 2;
	int image = footpath_construction_preview_images[slope][direction];

	int selectedPath = gFootpathSelectedId;
	rct_footpath_entry *pathType = get_footpath_entry(selectedPath);
	image += pathType->image;
	if (gFootpathSelectedType != SELECTED_PATH_TYPE_NORMAL)
		image += 51;

	return image;
}

/**
 *
 *  rct2: 0x006A7D8B
 */
static void window_footpath_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);

	if (!(w->disabled_widgets & (1 << WIDX_CONSTRUCT))) {
		sint32 image = window_footpath_pathimage(w);

		// Draw construction image
		int x = w->x + (window_footpath_widgets[WIDX_CONSTRUCT].left + window_footpath_widgets[WIDX_CONSTRUCT].right) / 2;
		int y = w->y + window_footpath_widgets[WIDX_CONSTRUCT].bottom - 60;
		gfx_draw_sprite(dpi, image, x, y, 0);

		// Draw build this... label
		x = w->x + (window_footpath_widgets[WIDX_CONSTRUCT].left + window_footpath_widgets[WIDX_CONSTRUCT].right) / 2;
		y = w->y + window_footpath_widgets[WIDX_CONSTRUCT].bottom - 23;
		gfx_draw_string_centred(dpi, STR_BUILD_THIS, x, y, COLOUR_BLACK, NULL);
	}

	// Draw cost
	int x = w->x + (window_footpath_widgets[WIDX_CONSTRUCT].left + window_footpath_widgets[WIDX_CONSTRUCT].right) / 2;
	int y = w->y + window_footpath_widgets[WIDX_CONSTRUCT].bottom - 12;
	if (_window_footpath_cost != MONEY32_UNDEFINED)
		if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
			gfx_draw_string_centred(dpi, STR_COST_LABEL, x, y, COLOUR_BLACK, &_window_footpath_cost);
}

/**
 *
 *  rct2: 0x006A7F88
 */
static void window_footpath_show_footpath_types_dialog(rct_window *w, rct_widget *widget, int showQueues)
{
	int i, flags, numPathTypes, image;
	rct_footpath_entry *pathType;

	numPathTypes = 0;
	flags = FOOTPATH_ENTRY_FLAG_SHOW_ONLY_IN_SCENARIO_EDITOR;
	// If the game is in sandbox mode, also show paths that are normally restricted to the scenario editor, but not their queues (since these usually shouldn't have one)
	if ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || (gCheatsSandboxMode && !showQueues))
		flags = 0;

	for (i = 0; i < 16; i++) {
		pathType = get_footpath_entry(i);
		if (pathType == (rct_footpath_entry*)-1)
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
	gFootpathConstructDirection = (direction - get_current_rotation()) & 3;
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
	gFootpathConstructSlope = slope;
	_window_footpath_cost = MONEY32_UNDEFINED;
	window_footpath_set_enabled_and_pressed_widgets();
}

static void window_footpath_autoscenery_mousedown_preset(rct_window *w, rct_widget* widget)
{
	int i;
	rct_widget *dropdownWidget = widget - 1;
	for (i = 0; i < FOOTPATH_SCENERY_PRESET_COUNT; i++) {
		gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
		gDropdownItemsArgs[i] = _footpath_scenery_presets[i];
	}
	window_dropdown_show_text_custom_width(
		w->x + dropdownWidget->left,
		w->y + dropdownWidget->top,
		dropdownWidget->bottom - dropdownWidget->top + 1,
		w->colours[1],
		DROPDOWN_FLAG_STAY_OPEN,
		FOOTPATH_SCENERY_PRESET_COUNT,
		dropdownWidget->right - dropdownWidget->left - 3
		);
	dropdown_set_checked(_window_footpath_scenery_preset, true);
}

static void window_footpath_autoscenery_mousedown_object(rct_window *w, rct_widget* widget)
{
	uint16 sceneryId;
	rct_widget *dropdownWidget = widget - 1;

	int numSceneryTypes		= 1;
	gDropdownItemsFormat[0] = STR_FOOTPATH_SCENERY_NOTHING;
	gDropdownItemsArgs[0]	= SPR_NONE;

	for (sceneryId = SCENERY_PATH_SCENERY_ID_MIN; sceneryId < SCENERY_PATH_SCENERY_ID_MAX; sceneryId++)
	{
		int pathBitIndex = sceneryId - SCENERY_PATH_SCENERY_ID_MIN;

		if (get_footpath_item_entry(pathBitIndex) == (rct_scenery_entry *)-1)
			continue;

		if (!scenery_is_invented(sceneryId))
			continue;

		rct_scenery_entry * sceneryEntry = get_footpath_item_entry(pathBitIndex);

		gDropdownItemsFormat[numSceneryTypes] = -1;
		gDropdownItemsArgs[numSceneryTypes]	  = sceneryEntry->image;
		numSceneryTypes++;
	}

	window_dropdown_show_image(w->x + dropdownWidget->left, w->y + dropdownWidget->top, dropdownWidget->bottom - dropdownWidget->top + 1,
							   w->colours[0], DROPDOWN_FLAG_STAY_OPEN, numSceneryTypes, 50, 50, gAppropriateImageDropdownItemsPerRow[numSceneryTypes]);
}

static void window_footpath_update_selected_object_string()
{
	uint32 index = 0;
	uint16 sceneryId;

	if (_window_footpath_scenery_custom_selected_id == 0) {
		_window_footpath_scenery_selected_sceneryEntry = NULL;
	} else {
		for (sceneryId = SCENERY_PATH_SCENERY_ID_MIN; sceneryId < SCENERY_PATH_SCENERY_ID_MAX; sceneryId++)
		{
			int pathBitIndex = sceneryId - SCENERY_PATH_SCENERY_ID_MIN;

			if (get_footpath_item_entry(pathBitIndex) == (rct_scenery_entry *)-1)
				continue;

			if (!scenery_is_invented(sceneryId))
				continue;

			rct_scenery_entry * sceneryEntry = get_footpath_item_entry(pathBitIndex);
			index++;

			if (index == _window_footpath_scenery_custom_selected_id) {
				_window_footpath_scenery_selected_sceneryEntry = sceneryEntry;
				break;
			}
		}
	}
}

/**
 *
 *  rct2: 0x006A81FB
 */
static void window_footpath_set_provisional_path_at_point(int x, int y)
{
	map_invalidate_selection_rect();
	gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;

	int interactionType;
	rct_map_element *mapElement;
	rct_xy16 mapCoord = { 0 };
	get_map_coordinates_from_pos(x, y, VIEWPORT_INTERACTION_MASK_FOOTPATH & VIEWPORT_INTERACTION_MASK_TERRAIN, &mapCoord.x, &mapCoord.y, &interactionType, &mapElement, NULL);
	x = mapCoord.x;
	y = mapCoord.y;

	if (interactionType == VIEWPORT_INTERACTION_ITEM_NONE) {
		gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
		footpath_provisional_update();
	} else {
		// Check for change
		if (
			(gFootpathProvisionalFlags & PROVISIONAL_PATH_FLAG_1) &&
			gFootpathProvisionalPosition.x == x &&
			gFootpathProvisionalPosition.y == y &&
			gFootpathProvisionalPosition.z == mapElement->base_height
		) {
			return;
		}

		// Set map selection
		gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
		gMapSelectType = MAP_SELECT_TYPE_FULL;
		gMapSelectPositionA.x = x;
		gMapSelectPositionA.y = y;
		gMapSelectPositionB.x = x;
		gMapSelectPositionB.y = y;

		footpath_provisional_update();

		// Set provisional path
		int slope = default_path_slope[mapElement->properties.surface.slope & 0x1F];
		if (interactionType == VIEWPORT_INTERACTION_ITEM_FOOTPATH)
			slope = mapElement->properties.surface.slope & 7;
		int pathType = (gFootpathSelectedType << 7) + (gFootpathSelectedId & 0xFF);

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
	gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
	gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;

	footpath_bridge_get_info_from_pos(screenX, screenY, &x, &y, &direction, &mapElement);
	if (x == MAP_LOCATION_NULL)
		return;

	gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
	gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_ARROW;
	gMapSelectType = MAP_SELECT_TYPE_FULL;
	gMapSelectPositionA.x = x;
	gMapSelectPositionB.x = x;
	gMapSelectPositionA.y = y;
	gMapSelectPositionB.y = y;

	gMapSelectArrowDirection = direction;
	gMapSelectArrowPosition.x = x;
	gMapSelectArrowPosition.y = y;

	int z = mapElement->base_height;

	if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_SURFACE) {
		uint8 slope = mapElement->properties.surface.slope;
		if (slope & 0xf)
			z += 2; // Add 2 for a slope
		if (slope & 0x10)
			z += 2; // Add another 2 for a steep slope
	}

	gMapSelectArrowPosition.z = z << 3;

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

	if (_footpathErrorOccured)
		return;

	footpath_provisional_update();

	rct_xy16 mapCoord = { 0 };
	get_map_coordinates_from_pos(x, y, VIEWPORT_INTERACTION_MASK_FOOTPATH & VIEWPORT_INTERACTION_MASK_TERRAIN, &mapCoord.x, &mapCoord.y, &interactionType, &mapElement, NULL);
	x = mapCoord.x;
	y = mapCoord.y;

	if (interactionType == VIEWPORT_INTERACTION_ITEM_NONE)
		return;

	// Set path
	presentType = default_path_slope[mapElement->properties.path.type & 0x1F];
	if (interactionType == VIEWPORT_INTERACTION_ITEM_FOOTPATH)
		presentType = mapElement->properties.path.type & 7;
	z = mapElement->base_height;
	selectedType = (gFootpathSelectedType << 7) + (gFootpathSelectedId & 0xFF);

	// Try and place path
	gGameCommandErrorTitle = STR_CANT_BUILD_FOOTPATH_HERE;
	cost = footpath_place(selectedType, x, y, z, presentType, GAME_COMMAND_FLAG_APPLY, 0);
	// Do not place automatic path scenery here, as the previous command deleted it

	if (cost == MONEY32_UNDEFINED) {
		_footpathErrorOccured = true;
	} else if (gFootpathPrice != 0) {
		// bp = RCT2_ADDRESS_COMMAND_MAP_Z
		// dx = RCT2_ADDRESS_COMMAND_MAP_Y
		// cx = RCT2_ADDRESS_COMMAND_MAP_X
		audio_play_sound_at_location(SOUND_PLACE_ITEM, gCommandPosition.x, gCommandPosition.y, gCommandPosition.z);
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
	if (x == MAP_LOCATION_NULL)
		return;

	if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_SURFACE) {
		// If we start the path on a slope, the arrow is slightly raised, so we
		// expect the path to be slightly raised as well.
		uint8 slope = mapElement->properties.surface.slope;
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
	gFootpathConstructFromPosition.x = x;
	gFootpathConstructFromPosition.y = y;
	gFootpathConstructFromPosition.z = z * 8;
	gFootpathConstructDirection = direction;
	gFootpathProvisionalFlags = 0;
	_window_footpath_provisional_path_arrow_timer = 0;
	gFootpathConstructSlope = 0;
	gFootpathConstructionMode = PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL;
	gFootpathConstructValidDirections = 255;
	window_footpath_set_enabled_and_pressed_widgets();
}

/**
 * Construct a piece of footpath while in bridge building mode.
 *  rct2: 0x006A79B7
 */
static void window_footpath_construct()
{
	_window_footpath_cost = MONEY32_UNDEFINED;
	footpath_provisional_update();

	int type, x, y, z, slope;
	footpath_get_next_path_info(&type, &x, &y, &z, &slope);

	// Try to place the path at the desired location
	gGameCommandErrorTitle = STR_CANT_BUILD_FOOTPATH_HERE;
	money32 cost = footpath_place(type, x, y, z, slope, 0, 0);

	if (cost != MONEY32_UNDEFINED) {
		// It is possible, let's remove walls between the old and new piece of path
		uint8 direction = gFootpathConstructDirection;
		map_remove_intersecting_walls(x, y, z, z + 4 + ((slope & 0xf) ? 2 : 0), direction ^ 2);
		map_remove_intersecting_walls(
			x - TileDirectionDelta[direction].x,
			y - TileDirectionDelta[direction].y,
			z, z + 4, direction
		);
	}

	// Actually place the path now
	gGameCommandErrorTitle = STR_CANT_BUILD_FOOTPATH_HERE;
	cost = footpath_place(type, x, y, z, slope, GAME_COMMAND_FLAG_APPLY, 0);

	if (cost != MONEY32_UNDEFINED) {
		audio_play_sound_at_location(
			SOUND_PLACE_ITEM,
			gFootpathConstructFromPosition.x,
			gFootpathConstructFromPosition.y,
			gFootpathConstructFromPosition.z
		);

		// Get path scenery
		uint16 add_scenery = 0; // window_footpath_scenery_get_next_scenery();
		if (add_scenery != 0)
		{
			uint16 sceneryFlags = (add_scenery & 0xFF) + 1;
			gGameCommandErrorTitle = STR_CANT_DO_THIS;
			cost = footpath_place(type, x, y, z, slope, GAME_COMMAND_FLAG_APPLY, sceneryFlags);
		}
// 		window_footpath_scenery_advance();

		if (gFootpathConstructSlope == 0) {
			gFootpathConstructValidDirections = 0xFF;
		} else {
			gFootpathConstructValidDirections = gFootpathConstructDirection;
		}

		if (gFootpathGroundFlags & ELEMENT_IS_UNDERGROUND)
			viewport_set_visibility(1);

		// If we have just built an upwards slope, the next path to construct is
		// a bit higher. Note that the z returned by footpath_get_next_path_info
		// already is lowered if we are building a downwards slope.
		if (gFootpathConstructSlope == 2)
			z += 2;

		gFootpathConstructFromPosition.x = x;
		gFootpathConstructFromPosition.y = y;
		gFootpathConstructFromPosition.z = z << 3;
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
		if (pathType == gFootpathConstructDirection)
			z += 2;
	}

	// Find a connected edge
	int edge = gFootpathConstructDirection ^ 2;
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
	gGameCommandErrorTitle = STR_CANT_REMOVE_FOOTPATH_FROM_HERE;
	footpath_remove(
		gFootpathConstructFromPosition.x,
		gFootpathConstructFromPosition.y,
		mapElement->base_height,
		1
	);

	// Move selection
	edge ^= 2;
	x = gFootpathConstructFromPosition.x - TileDirectionDelta[edge].x;
	y = gFootpathConstructFromPosition.y - TileDirectionDelta[edge].y;
	gFootpathConstructFromPosition.x = x;
	gFootpathConstructFromPosition.y = y;
	gFootpathConstructFromPosition.z = z << 3;
	gFootpathConstructDirection = edge;
	gFootpathConstructValidDirections = 255;
}

/**
 *
 *  rct2: 0x006A7873
 */
static rct_map_element *footpath_get_map_element_to_remove()
{
	rct_map_element *mapElement;
	int x, y, z, zLow;

	x = gFootpathConstructFromPosition.x / 32;
	y = gFootpathConstructFromPosition.y / 32;
	if (x >= 256 || y >= 256)
		return NULL;

	z = (gFootpathConstructFromPosition.z >> 3) & 0xFF;
	zLow = z - 2;

	mapElement = map_get_first_element_at(x, y);
	do {
		if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_PATH) {
			if (mapElement->base_height == z) {
				if (mapElement->properties.path.type & 4)
					if (((mapElement->properties.path.type & 3) ^ 2) != gFootpathConstructDirection)
						continue;

				return mapElement;
			} else if (mapElement->base_height == zLow) {
				if (!(mapElement->properties.path.type & 4))
					if ((mapElement->properties.path.type & 3) == gFootpathConstructDirection)
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
	rct_window *w = window_find_by_class(WC_FOOTPATH);
	if (w == NULL)
		return;

	if (gFootpathConstructionMode == PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL) {
		map_invalidate_map_selection_tiles();
		gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
		gMapSelectFlags |= MAP_SELECT_FLAG_GREEN;

		int direction = gFootpathConstructDirection;
		gMapSelectionTiles[0].x = gFootpathConstructFromPosition.x + TileDirectionDelta[direction].x;
		gMapSelectionTiles[0].y = gFootpathConstructFromPosition.y + TileDirectionDelta[direction].y;
		gMapSelectionTiles[1].x = -1;
		map_invalidate_map_selection_tiles();
	}

	uint64 pressedWidgets = w->pressed_widgets & 0xFFFF887F;
	uint64 disabledWidgets = 0;
	int currentRotation = get_current_rotation();
	if (gFootpathConstructionMode >= PATH_CONSTRUCTION_MODE_BRIDGE_OR_TUNNEL) {
		// Set pressed directional widget
		int direction = (gFootpathConstructDirection + currentRotation) & 3;
		pressedWidgets |= (1LL << (WIDX_DIRECTION_NW + direction));

		// Set pressed slope widget
		int slope = gFootpathConstructSlope;
		if (slope == 6)
			pressedWidgets |= (1 << WIDX_SLOPEDOWN);
		else if (slope == 0)
			pressedWidgets |= (1 << WIDX_LEVEL);
		else
			pressedWidgets |= (1 << WIDX_SLOPEUP);

		// Enable / disable directional widgets
		direction = gFootpathConstructValidDirections;
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
			(1 << WIDX_AUTOSCENERY_GROUP) |
			(1 << WIDX_AUTOSCENERY_DROPDOWN) |
			(1 << WIDX_AUTOSCENERY_DROPDOWN_BUTTON) |
			(1 << WIDX_AUTOSCENERY_DROPDOWN_LIST) |
			(1 << WIDX_AUTOSCENERY_DROPDOWN_LIST_CLEAR) |
			(1 << WIDX_AUTOSCENERY_OBJECT_DROPDOWN) |
			(1 << WIDX_AUTOSCENERY_OBJECT_DROPDOWN_BUTTON) |
			(1 << WIDX_AUTOSCENERY_ADD) |
			(1 << WIDX_CONSTRUCT) |
			(1 << WIDX_REMOVE);
	}

	if (_footpath_scenery_presets[_window_footpath_scenery_preset] != STR_FOOTPATH_SCENERY_CUSTOM) {
		disabledWidgets |= (1 << WIDX_AUTOSCENERY_OBJECT_DROPDOWN) |
			(1 << WIDX_AUTOSCENERY_OBJECT_DROPDOWN_BUTTON) |
			(1 << WIDX_AUTOSCENERY_ADD) |
			(1 << WIDX_AUTOSCENERY_DROPDOWN_LIST_CLEAR);
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

	direction = gFootpathConstructDirection;
	*x = gFootpathConstructFromPosition.x + TileDirectionDelta[direction].x;
	*y = gFootpathConstructFromPosition.y + TileDirectionDelta[direction].y;
	*z = gFootpathConstructFromPosition.z / 8;
	*type = (gFootpathSelectedType << 7) + (gFootpathSelectedId & 0xFF);
	*slope = 0;
	if (gFootpathConstructSlope != 0) {
		*slope = gFootpathConstructDirection | 4;
		if (gFootpathConstructSlope != 2) {
			*z -= 2;
			*slope ^= 2;
		}
	}
}

static void footpath_select_default()
{
	// Select first available footpath
	gFootpathSelectedId = 0;
	for (int i = 0; i < object_entry_group_counts[OBJECT_TYPE_PATHS]; i++) {
		rct_footpath_entry *pathEntry = get_footpath_entry(i);
		if (pathEntry != (rct_footpath_entry*)-1) {
			gFootpathSelectedId = i;

			// Prioritise non-restricted path
			if (!(pathEntry->flags & 4)) {
				break;
			}
		}
	}
}

static void window_footpath_scenery_init_presets()
{
	uint8 i, j;

	for (i = 0; i < FOOTPATH_SCENERY_PRESET_COUNT - 1; i++)
	{
		if (_footpath_scenery_list[i] == NULL && _footpath_scenery_presets_types[i][0] != PATH_BIT_DRAW_TYPE_END) {
			path_rule_entry *current, *last = malloc(sizeof(path_rule_entry));
			_footpath_scenery_list[i] = last;
			last->sceneEntry = window_footpath_scenery_get_scenery_entry_for_type(_footpath_scenery_presets_types[i][0]);
			last->next = NULL;
			for (j = 1; _footpath_scenery_presets_types[i][j] != PATH_BIT_DRAW_TYPE_END; j++) {
				current = malloc(sizeof(path_rule_entry));
				last->next = current;
				last = current;

				current->sceneEntry = window_footpath_scenery_get_scenery_entry_for_type(_footpath_scenery_presets_types[i][j]);
				current->next = NULL;
			}
		}
	}
}

static struct rct_scenery_entry* window_footpath_scenery_get_scenery_entry_for_type(sint8 type)
{
	uint16 sceneryId;
	switch (type) {
		case PATH_BIT_DRAW_TYPE_END:
		case PATH_BIT_DRAW_TYPE_NOTHING:
			return NULL;
		default:
		for (sceneryId = SCENERY_PATH_SCENERY_ID_MIN; sceneryId < SCENERY_PATH_SCENERY_ID_MAX; sceneryId++)
		{
			int pathBitIndex = sceneryId - SCENERY_PATH_SCENERY_ID_MIN;
			if (get_footpath_item_entry(pathBitIndex) == (rct_scenery_entry *)-1)
				continue;

			if (!scenery_is_invented(sceneryId))
				continue;

			rct_scenery_entry *scenery = get_footpath_item_entry(pathBitIndex);
			if (scenery->path_bit.draw_type == type) {
				printf("Init with ID %d (String %d)\n", sceneryId, scenery->name);
				return scenery;
			}
		}
	}
	return NULL;
}

static void window_footpath_list_scrollgetsize(rct_window * w, int scrollIndex, int * width, int * height)
{
	int top;

	*height = w->no_list_items * 10;

	top = *height - window_footpath_widgets[WIDX_AUTOSCENERY_DROPDOWN_LIST].bottom +
			window_footpath_widgets[WIDX_AUTOSCENERY_DROPDOWN_LIST].top + 21;
	if (top < 0)
		top = 0;
	if (top < w->scrolls[0].v_top)
	{
		w->scrolls[0].v_top = top;
		window_invalidate(w);
	}
}

static void window_footpath_scrollpaint(rct_window * w, rct_drawpixelinfo * dpi, int scrollIndex)
{
	path_rule_entry *list;
	int i = 0;
	rct_string_id format = STR_BLACK_STRING;

	gfx_fill_rect(dpi, dpi->x, dpi->y, dpi->x + dpi->width, dpi->y + dpi->height, ColourMapA[NOT_TRANSLUCENT(w->colours[0])].mid_light);

	list = _footpath_scenery_list[_window_footpath_scenery_preset];
	while (list != NULL) {
		if (i == w->selected_list_item) {
			gfx_filter_rect(dpi, 0, (i * 10), FOOTPATH_SCENERY_LIST_WIDTH, (i * 10) + 9, PALETTE_DARKEN_1);
		}

		if (list->sceneEntry != NULL)
		{
			gfx_draw_string_left_clipped(dpi, format, &list->sceneEntry->name, COLOUR_BLACK, 0, (i * 10) - 1, FOOTPATH_SCENERY_LIST_WIDTH);
		}
		else
		{
			rct_string_id nothing = STR_FOOTPATH_SCENERY_NOTHING;
			gfx_draw_string_left_clipped(dpi, format, &nothing, COLOUR_BLACK, 0, (i * 10) - 1, FOOTPATH_SCENERY_LIST_WIDTH);
		}

		i++;
		list = list->next;
	}
}

static void window_footpath_list_scrollmousedown(rct_window *w, int scrollIndex, int x, int y)
{
	int index;

	index = y / 10;
	if (index >= w->no_list_items)
		return;

	w->selected_list_item = index;
	window_footpath_update_listselection(index);
}

static void window_footpath_update_listselection(int index)
{
	path_rule_entry *selected = _footpath_scenery_list[_window_footpath_scenery_preset];
	int i;

	for (i = 0; i < index; i++)
	{
		if (selected == NULL) {
			_footpath_selected_listEntry = _footpath_scenery_list[_window_footpath_scenery_preset];
			return;
		}
		selected = selected->next;
	}
	_footpath_selected_listEntry = selected;
}
