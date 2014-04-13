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

#include "addresses.h"
#include "map.h"
#include "strings.h"
#include "sprites.h"
#include "widget.h"
#include "window.h"

static enum WINDOW_LAND_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_PREVIEW,
	WIDX_DECREMENT,
	WIDX_INCREMENT,
	WIDX_FLOOR,
	WIDX_WALL,
};

static rct_widget window_land_widgets[] = {
	{ WWT_FRAME,	0,	0,	97,	0,	125,	-1,			STR_NONE },						// panel / background
	{ WWT_CAPTION,	0,	1,	96,	1,	14,		STR_LAND,	STR_WINDOW_TITLE_TIP },			// title bar
	{ WWT_CLOSEBOX,	0,	85,	95,	2,	13,		824,		STR_CLOSE_WINDOW_TIP },			// close x button
	{ WWT_IMGBTN,	0,	27,	70,	17,	48,		5503,		STR_NONE },						// preview box
	{ WWT_TRNBTN,	1,	28,	43,	18,	33,		0x20000000 | SPR_LAND_TOOL_DECREASE,	STR_ADJUST_SMALLER_LAND_TIP },	// decrement size
	{ WWT_TRNBTN,	1,	54,	69,	32,	47,		0x20000000 | SPR_LAND_TOOL_INCREASE,	STR_ADJUST_LARGER_LAND_TIP },	// increment size
	{ WWT_FLATBTN,	1,	2,	48,	75,	110,	0xFFFFFFFF,	STR_CHANGE_BASE_LAND_TIP },		// floor texture
	{ WWT_FLATBTN,	1,	49,	95,	75,	110,	0xFFFFFFFF,	STR_CHANGE_VERTICAL_LAND_TIP },	// wall texture
	{ WIDGETS_END },
};

static char window_land_floor_texture_order[] = {
	TERRAIN_SAND_DARK, TERRAIN_SAND_LIGHT,  TERRAIN_DIRT,      TERRAIN_GRASS_CLUMPS, TERRAIN_GRASS,
	TERRAIN_ROCK,      TERRAIN_SAND,        TERRAIN_MARTIAN,   TERRAIN_CHECKERBOARD, TERRAIN_ICE,
	TERRAIN_GRID_RED,  TERRAIN_GRID_YELLOW, TERRAIN_GRID_BLUE, TERRAIN_GRID_GREEN
};

static char window_land_wall_texture_order[] = {
	TERRAIN_EDGE_ROCK,       TERRAIN_EDGE_WOOD_RED,
	TERRAIN_EDGE_WOOD_BLACK, TERRAIN_EDGE_ICE,
	0, 0
};

int _selectedFloorTexture;
int _selectedWallTexture;

/**
 *
 *  rct2: 0x00663E7D
 */
void window_land_open()
{
	rct_window* window;

	// Check if window is already open
	if (window_find_by_id(WC_LAND, 0) != NULL)
		return;

	window = window_create(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16) - 98, 29, 98, 126, 0x0097B904, WC_LAND, 0);
	window->widgets = window_land_widgets;
	window->enabled_widgets =
		(1 << WIDX_CLOSE) |
		(1 << WIDX_DECREMENT) |
		(1 << WIDX_INCREMENT) |
		(1 << WIDX_FLOOR) |
		(1 << WIDX_WALL);
	window_init_scroll_widgets(window);
	window_push_others_below(window);

	RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_SURFACE, uint8) = 255;
	RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_EDGE, uint8) = 255;
	_selectedFloorTexture = 0;
	_selectedWallTexture = 0;
	RCT2_GLOBAL(0x009E2E1C, sint32) = 0x80000000;
	RCT2_GLOBAL(0x009E2E20, sint32) = 0x80000000;
	window->colours[0] = 24;
	window->colours[1] = 24;
	window->colours[2] = 24;
}