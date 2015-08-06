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
#include "../drawing/drawing.h"
#include "../game.h"
#include "../input.h"
#include "../interface/themes.h"
#include "../interface/viewport.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../ride/track.h"
#include "dropdown.h"

#pragma region Widgets

enum {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_MAZE_MODE_GROUPBOX,
	WIDX_MAZE_BUILD_MODE = 6,
	WIDX_MAZE_MOVE_MODE,
	WIDX_MAZE_FILL_MODE,
	WIDX_MAZE_DIRECTION_GROUPBOX = 23,
	WIDX_MAZE_DIRECTION_NW,
	WIDX_MAZE_DIRECTION_NE,
	WIDX_MAZE_DIRECTION_SW,
	WIDX_MAZE_DIRECTION_SE,
	WIDX_MAZE_ENTRANCE = 29,
	WIDX_MAZE_EXIT,
};

static rct_widget window_maze_construction_widgets[] = {
	{ WWT_FRAME,			0,	0,		165,	0,		199,	0xFFFFFFFF,							STR_NONE												},
	{ WWT_CAPTION,			0,	1,		164,	1,		14,		896,								STR_WINDOW_TITLE_TIP									},
	{ WWT_CLOSEBOX,			0,	153,	163,	2,		13,		STR_CLOSE_X,						STR_CLOSE_WINDOW_TIP									},
	{ WWT_GROUPBOX,			0,	3,		162,	17,		71,		STR_RIDE_CONSTRUCTION_MODE,			STR_NONE												},
	{ WWT_EMPTY,			0,	0,		0,		0,		0,		0xFFFFFFFF,							STR_NONE												},
	{ WWT_EMPTY,			0,	0,		0,		0,		0,		0xFFFFFFFF,							STR_NONE												},
	{ WWT_FLATBTN,			1,	35,		66,		29,		60,		0x15C8,								STR_RIDE_CONSTRUCTION_BUILD_MODE						},
	{ WWT_FLATBTN,			1,	67,		98,		29,		60,		0x15C9,								STR_RIDE_CONSTRUCTION_MOVE_MODE							},
	{ WWT_FLATBTN,			1,	99,		130,	29,		60,		0x15CA,								STR_RIDE_CONSTRUCTION_FILL_IN_MODE						},
	{ WWT_EMPTY,			0,	0,		0,		0,		0,		0xFFFFFFFF,							STR_NONE												},
	{ WWT_EMPTY,			0,	0,		0,		0,		0,		0xFFFFFFFF,							STR_NONE												},
	{ WWT_EMPTY,			0,	0,		0,		0,		0,		0xFFFFFFFF,							STR_NONE												},
	{ WWT_EMPTY,			0,	0,		0,		0,		0,		0xFFFFFFFF,							STR_NONE												},
	{ WWT_EMPTY,			0,	0,		0,		0,		0,		0xFFFFFFFF,							STR_NONE												},
	{ WWT_EMPTY,			0,	0,		0,		0,		0,		0xFFFFFFFF,							STR_NONE												},
	{ WWT_EMPTY,			0,	0,		0,		0,		0,		0xFFFFFFFF,							STR_NONE												},
	{ WWT_EMPTY,			0,	0,		0,		0,		0,		0xFFFFFFFF,							STR_NONE												},
	{ WWT_EMPTY,			0,	0,		0,		0,		0,		0xFFFFFFFF,							STR_NONE												},
	{ WWT_EMPTY,			0,	0,		0,		0,		0,		0xFFFFFFFF,							STR_NONE												},
	{ WWT_EMPTY,			0,	0,		0,		0,		0,		0xFFFFFFFF,							STR_NONE												},
	{ WWT_EMPTY,			0,	0,		0,		0,		0,		0xFFFFFFFF,							STR_NONE												},
	{ WWT_EMPTY,			0,	0,		0,		0,		0,		0xFFFFFFFF,							STR_NONE												},
	{ WWT_EMPTY,			0,	0,		0,		0,		0,		0xFFFFFFFF,							STR_NONE												},
	{ WWT_GROUPBOX,			0,	3,		162,	80,		166,	STR_RIDE_CONSTRUCTION_BUILD,		STR_NONE												},
	{ WWT_FLATBTN,			1,	83,		127,	96,		124,	0x1603,								STR_RIDE_CONSTRUCTION_BUILD_MAZE_IN_THIS_DIRECTION_TIP	},
	{ WWT_FLATBTN,			1,	83,		127,	125,	153,	0x1604,								STR_RIDE_CONSTRUCTION_BUILD_MAZE_IN_THIS_DIRECTION_TIP	},
	{ WWT_FLATBTN,			1,	38,		82,		125,	153,	0x1605,								STR_RIDE_CONSTRUCTION_BUILD_MAZE_IN_THIS_DIRECTION_TIP	},
	{ WWT_FLATBTN,			1,	38,		82,		96,		124,	0x1606,								STR_RIDE_CONSTRUCTION_BUILD_MAZE_IN_THIS_DIRECTION_TIP	},
	{ WWT_GROUPBOX,			0,	3,		162,	168,	195,	0xFFFFFFFF,							STR_NONE												},
	{ WWT_DROPDOWN_BUTTON,	1,	9,		78,		178,	189,	STR_RIDE_CONSTRUCTION_ENTRANCE,		STR_RIDE_CONSTRUCTION_ENTRANCE_TIP						},
	{ WWT_DROPDOWN_BUTTON,	1,	87,		156,	178,	189,	STR_RIDE_CONSTRUCTION_EXIT,			STR_RIDE_CONSTRUCTION_EXIT_TIP							},
	{ WWT_EMPTY,			0,	0,		0,		0,		0,		0xFFFFFFFF,							STR_NONE												},
	{ WWT_EMPTY,			0,	0,		0,		0,		0,		0xFFFFFFFF,							STR_NONE												},
	{ WIDGETS_END }
};

#pragma endregion

#pragma region Events

static void window_maze_construction_close(rct_window *w);
static void window_maze_construction_mouseup(rct_window *w, int widgetIndex);
static void window_maze_construction_resize(rct_window *w);
static void window_maze_construction_mousedown(int widgetIndex, rct_window *w, rct_widget *widget);
static void window_maze_construction_update(rct_window *w);
static void window_maze_construction_toolupdate(rct_window* w, int widgetIndex, int x, int y);
static void window_maze_construction_tooldown(rct_window* w, int widgetIndex, int x, int y);
static void window_maze_construction_invalidate(rct_window *w);
static void window_maze_construction_paint(rct_window *w, rct_drawpixelinfo *dpi);

// 0x993F6C
static rct_window_event_list window_maze_construction_events = {
	window_maze_construction_close,
	window_maze_construction_mouseup,
	window_maze_construction_resize,
	window_maze_construction_mousedown,
	NULL,
	NULL,
	window_maze_construction_update,
	NULL,
	NULL,
	window_maze_construction_toolupdate,
	window_maze_construction_tooldown,
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
	NULL,
	NULL,
	window_maze_construction_invalidate,
	window_maze_construction_paint,
	NULL
};

#pragma endregion

static void window_maze_construction_construct(int direction);

/**
 *
 * rct2: 0x006CB481
 */
rct_window *window_maze_construction_open()
{
	rct_window *w = window_create(0, 29, 166, 200, &window_maze_construction_events, WC_RIDE_CONSTRUCTION, WF_9);
	w->widgets = window_maze_construction_widgets;
	w->enabled_widgets = 0x6F0001C4;

	window_init_scroll_widgets(w);
	colour_scheme_update(w);

	w->number = _currentRideIndex;

	window_push_others_right(w);
	show_gridlines();
	return w;
}

/**
 *
 * rct2: 0x006CD811
 */
static void window_maze_construction_close(rct_window *w)
{
	sub_6C9627();
	viewport_set_visibility(0);

	map_invalidate_map_selection_tiles();
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~(1 << 1);

	// In order to cancel the yellow arrow correctly the
	// selection tool should be cancelled.
	tool_cancel();
	
	hide_gridlines();

	uint8 rideIndex = _currentRideIndex;
	rct_ride* ride = GET_RIDE(rideIndex);
	if (ride->overall_view == 0xFFFF) {
		int savedPausedState = RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint8);
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint8) = 0;
		game_do_command(0, 9, 0, rideIndex, GAME_COMMAND_DEMOLISH_RIDE, 0, 0);
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint8) = savedPausedState;
	} else {
		window_ride_main_open(rideIndex);
	}
}

static void window_maze_construction_entrance_mouseup(rct_window *w, int widgetIndex){
	if (tool_set(w, widgetIndex, 12))
		return;

	RCT2_GLOBAL(0x00F44191, uint8) = widgetIndex == WIDX_MAZE_ENTRANCE ? 0 : 1;
	RCT2_GLOBAL(0x00F44192, uint8) = (uint8)w->number;
	RCT2_GLOBAL(0x00F44193, uint8) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) |= INPUT_FLAG_6;

	sub_6C9627();

	// ???
	uint8 old_state = _rideConstructionState;
	_rideConstructionState = 5;
	if (old_state != 5)
		_rideConstructionState = old_state;
	window_maze_construction_update_pressed_widgets();
}

/**
 *
 * rct2: 0x006CD461
 */
static void window_maze_construction_mouseup(rct_window *w, int widgetIndex)
{
	switch (widgetIndex){
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_MAZE_ENTRANCE:
	case WIDX_MAZE_EXIT:
		window_maze_construction_entrance_mouseup(w, widgetIndex);
		break;
	case WIDX_MAZE_DIRECTION_NW:
	case WIDX_MAZE_DIRECTION_NE:
	case WIDX_MAZE_DIRECTION_SE:
	case WIDX_MAZE_DIRECTION_SW:
		window_maze_construction_construct(
			((widgetIndex - WIDX_MAZE_DIRECTION_NW) - RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8)) & 3
		);
		break;
	}
}

/**
 *
 * rct2: 0x006CD623
 */
static void window_maze_construction_resize(rct_window *w)
{
	uint64 disabledWidgets = 0;
	if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_PLACE) {
		disabledWidgets |= (
			(1ULL << WIDX_MAZE_BUILD_MODE) |
			(1ULL << WIDX_MAZE_MOVE_MODE) |
			(1ULL << WIDX_MAZE_FILL_MODE) |
			(1ULL << WIDX_MAZE_DIRECTION_NW) |
			(1ULL << WIDX_MAZE_DIRECTION_NE) |
			(1ULL << WIDX_MAZE_DIRECTION_SW) |
			(1ULL << WIDX_MAZE_DIRECTION_SE)
		);
	}

	// Set and invalidate the changed widgets
	uint64 currentDisabledWidgets = w->disabled_widgets;
	if (currentDisabledWidgets == disabledWidgets)
		return;

	for (int i = 0; i < 64; i++) {
		if ((disabledWidgets & (1ULL << i)) != (currentDisabledWidgets & (1ULL << i))) {
			widget_invalidate(w, i);
		}
	}
	w->disabled_widgets = disabledWidgets;
}

/**
 *
 * rct2: 0x006CD48C
 */
static void window_maze_construction_mousedown(int widgetIndex, rct_window *w, rct_widget *widget)
{
	switch (widgetIndex) {
	case WIDX_MAZE_BUILD_MODE:
		_rideConstructionState = RIDE_CONSTRUCTION_STATE_MAZE_BUILD;
		window_maze_construction_update_pressed_widgets();
		break;
	case WIDX_MAZE_MOVE_MODE:
		_rideConstructionState = RIDE_CONSTRUCTION_STATE_MAZE_MOVE;
		window_maze_construction_update_pressed_widgets();
		break;
	case WIDX_MAZE_FILL_MODE:
		_rideConstructionState = RIDE_CONSTRUCTION_STATE_MAZE_FILL;
		window_maze_construction_update_pressed_widgets();
		break;
	}
}

/**
 *
 * rct2: 0x006CD767
 */
static void window_maze_construction_update(rct_window *w)
{
	switch (_rideConstructionState) {
	case RIDE_CONSTRUCTION_STATE_PLACE:
		if (!widget_is_active_tool(w, WIDX_MAZE_DIRECTION_GROUPBOX)) {
			window_close(w);
			return;
		}
		break;
	case RIDE_CONSTRUCTION_STATE_ENTRANCE_EXIT:
		if (!widget_is_active_tool(w, WIDX_MAZE_ENTRANCE) && !widget_is_active_tool(w, WIDX_MAZE_EXIT)) {
			_rideConstructionState = RCT2_GLOBAL(0x00F440CC, uint8);
			window_maze_construction_update_pressed_widgets();
		}
		break;
	}

	switch (_rideConstructionState) {
	case RIDE_CONSTRUCTION_STATE_FRONT:
	case RIDE_CONSTRUCTION_STATE_BACK:
	case RIDE_CONSTRUCTION_STATE_SELECTED:
		if (
			(RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint8) & INPUT_FLAG_TOOL_ACTIVE) &&
			RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass) == WC_RIDE_CONSTRUCTION
		) {
			tool_cancel();
		}
		break;
	}
	sub_6C94D8();
}

/**
 *
 * rct2: 0x006CD63E
 */
static void window_maze_construction_toolupdate(rct_window* w, int widgetIndex, int x, int y)
{
	switch (widgetIndex){
	case WIDX_MAZE_DIRECTION_GROUPBOX:
		ride_construction_toolupdate_construct(x, y);
		break;
	case WIDX_MAZE_ENTRANCE:
	case WIDX_MAZE_EXIT:
		ride_construction_toolupdate_entrance_exit(x, y);
		break;
	}
}

/**
 *
 *  rct2: 0x006C825F
 */
static void window_maze_construction_entrance_tooldown(int x, int y, rct_window* w){
	sub_6C9627();

	map_invalidate_selection_rect();

	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~((1 << 0) | (1 << 2));

	int direction = 0;
	ride_get_entrance_or_exit_position_from_screen_position(x, y, &x, &y, &direction);

	if (RCT2_GLOBAL(0x00F44194, uint8) == 0xFF)
		return;

	uint8 rideIndex = RCT2_GLOBAL(0x00F44192, uint8);
	uint8 is_exit = RCT2_GLOBAL(0x00F44191, uint8);
	RCT2_GLOBAL(0x00141E9AE, rct_string_id) = is_exit ? 1144 : 1145;

	money32 cost = game_do_command(
		x, 
		GAME_COMMAND_FLAG_APPLY | ((direction ^ 2) << 8), 
		y, 
		rideIndex | (is_exit << 8),
		GAME_COMMAND_PLACE_RIDE_ENTRANCE_OR_EXIT, 
		RCT2_GLOBAL(0x00F44193, uint8), 
		0);

	if (cost == MONEY32_UNDEFINED)
		return;

	sound_play_panned(
		SOUND_PLACE_ITEM,
		0x8001,
		RCT2_GLOBAL(0x009DEA5E, sint16),
		RCT2_GLOBAL(0x009DEA60, sint16),
		RCT2_GLOBAL(0x009DEA62, uint16));

	rct_ride* ride = GET_RIDE(rideIndex);
	if (ride_are_all_possible_entrances_and_exits_built(ride)){
		tool_cancel();
		if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_15))
			window_close(w);
	}
	else{
		RCT2_GLOBAL(0x00F44191, uint8) = is_exit ^ 1;
		window_invalidate_by_class(WC_RIDE_CONSTRUCTION);
		RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, uint8) = is_exit ? WIDX_MAZE_ENTRANCE : WIDX_MAZE_EXIT;
	}
}

/**
 * 
 *  rct2: 0x006CD65D
 */
static void window_maze_construction_tooldown(rct_window* w, int widgetIndex, int x, int y)
{
	switch (widgetIndex){
	case WIDX_MAZE_DIRECTION_GROUPBOX:
		ride_construction_tooldown_construct(x, y);
		break;
	case WIDX_MAZE_ENTRANCE:
	case WIDX_MAZE_EXIT:
		window_maze_construction_entrance_tooldown(x, y, w);
		break;
	}
}

/**
 *
 * rct2: 0x006CD435
 */
static void window_maze_construction_invalidate(rct_window *w)
{
	rct_ride *ride = GET_RIDE(_currentRideIndex);

	// Set window title arguments
	RCT2_GLOBAL(0x013CE956, rct_string_id) = ride->name;
	RCT2_GLOBAL(0x013CE958, uint32) = ride->name_arguments;
}

/**
 *
 * rct2: 0x006CD45B
 */
static void window_maze_construction_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);
}

/**
 * 
 * rct2: 0x006CD887
 */
void window_maze_construction_update_pressed_widgets()
{
	rct_window *w;
	
	w = window_find_by_class(WC_RIDE_CONSTRUCTION);
	if (w == NULL)
		return;

	uint64 pressedWidgets = w->pressed_widgets;
	pressedWidgets &= ~(1ULL << WIDX_MAZE_BUILD_MODE);
	pressedWidgets &= ~(1ULL << WIDX_MAZE_MOVE_MODE);
	pressedWidgets &= ~(1ULL << WIDX_MAZE_FILL_MODE);

	switch (_rideConstructionState) {
	case RIDE_CONSTRUCTION_STATE_MAZE_BUILD:
		pressedWidgets |= (1ULL << WIDX_MAZE_BUILD_MODE);
		break;
	case RIDE_CONSTRUCTION_STATE_MAZE_MOVE:
		pressedWidgets |= (1ULL << WIDX_MAZE_MOVE_MODE);
		break;
	case RIDE_CONSTRUCTION_STATE_MAZE_FILL:
		pressedWidgets |= (1ULL << WIDX_MAZE_FILL_MODE);
		break;
	}

	w->pressed_widgets = pressedWidgets;
	window_invalidate(w);
}

/**
 *
 * rct2: 0x006CD4AB
 */
static void window_maze_construction_construct(int direction)
{
	int x, y, z, flags, mode;

	sub_6C9627();

	x = _currentTrackBeginX + (TileDirectionDelta[direction].x / 2);
	y = _currentTrackBeginY + (TileDirectionDelta[direction].y / 2);
	z = _currentTrackBeginZ;
	switch (_rideConstructionState) {
	case RIDE_CONSTRUCTION_STATE_MAZE_BUILD:
		mode = 0;
		flags = 1;
		break;
	case RIDE_CONSTRUCTION_STATE_MAZE_MOVE:
		mode = 1;
		flags = 1 | 8;
		break;
	default:
	case RIDE_CONSTRUCTION_STATE_MAZE_FILL:
		mode = 2;
		flags = 1 | 8;
		break;
	}

	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
	money32 cost = game_do_command(
		x,
		flags | (direction << 8),
		y,
		_currentRideIndex | (mode << 8),
		GAME_COMMAND_SET_MAZE_TRACK,
		z,
		0
	);
	if (cost == MONEY32_UNDEFINED) {
		return;
	}

	_currentTrackBeginX = x;
	_currentTrackBeginY = y;
	if (_rideConstructionState != 7) {
		sound_play_panned(SOUND_PLACE_ITEM, 0x8001, x, y, z);
	}
}
