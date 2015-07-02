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
#include "../input.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../sprites.h"
#include "../world/map.h"
#include "dropdown.h"
#include "../interface/themes.h"

enum WINDOW_LAND_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_PREVIEW,
	WIDX_DECREMENT,
	WIDX_INCREMENT,
	WIDX_FLOOR,
	WIDX_WALL,
	WIDX_PAINTMODE,
};

static rct_widget window_land_widgets[] = {
	{ WWT_FRAME,	0,	0,	97,	0,	125,	-1,			STR_NONE },						// panel / background
	{ WWT_CAPTION,	0,	1,	96,	1,	14,		STR_LAND,	STR_WINDOW_TITLE_TIP },			// title bar
	{ WWT_CLOSEBOX,	0,	85,	95,	2,	13,		824,		STR_CLOSE_WINDOW_TIP },			// close x button
	{ WWT_IMGBTN,	0,	10,	53,	17,	48,		5503,		STR_NONE },						// preview box
	{ WWT_TRNBTN,	1,	11,	26,	18,	33,		0x20000000 | SPR_LAND_TOOL_DECREASE,	STR_ADJUST_SMALLER_LAND_TIP },	// decrement size
	{ WWT_TRNBTN,	1,	37,	52,	32,	47,		0x20000000 | SPR_LAND_TOOL_INCREASE,	STR_ADJUST_LARGER_LAND_TIP },	// increment size
	{ WWT_FLATBTN,	1,	2,	48,	75,	110,	0xFFFFFFFF,	STR_CHANGE_BASE_LAND_TIP },		// floor texture
	{ WWT_FLATBTN,	1,	49,	95,	75,	110,	0xFFFFFFFF,	STR_CHANGE_VERTICAL_LAND_TIP },	// wall texture
	{ WWT_FLATBTN,  1,	64,	87,	21,	44,		5173,		5127 }, // paint mode
	{ WIDGETS_END },
};

static void window_land_emptysub() { }
static void window_land_close();
static void window_land_mouseup();
static void window_land_mousedown(int widgetIndex, rct_window*w, rct_widget* widget);
static void window_land_dropdown();
static void window_land_update(rct_window *w);
static void window_land_invalidate();
static void window_land_paint();
static void window_land_textinput();
static void window_land_inputsize(rct_window *w);


static void* window_land_events[] = {
	window_land_close,
	window_land_mouseup,
	window_land_emptysub,
	window_land_mousedown,
	window_land_dropdown,
	window_land_emptysub,
	window_land_update,
	window_land_emptysub,
	window_land_emptysub,
	window_land_emptysub,
	window_land_emptysub,
	window_land_emptysub,
	window_land_emptysub,
	window_land_emptysub,
	window_land_emptysub,
	window_land_emptysub,
	window_land_emptysub,
	window_land_emptysub,
	window_land_emptysub,
	window_land_textinput,
	window_land_emptysub,
	window_land_emptysub,
	window_land_emptysub,
	window_land_emptysub,
	window_land_emptysub,
	window_land_invalidate,
	window_land_paint,
	window_land_emptysub
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

static int land_pricing[] = {
	300, 100, 80, 120, 100, 100, 110, 130,  110, 110, 110, 110, 110, 110
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
	if (window_find_by_class(WC_LAND) != NULL)
		return;

	window = window_create(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16) - 98, 29, 98, 126, (uint32*)window_land_events, WC_LAND, 0);
	window->widgets = window_land_widgets;
	window->enabled_widgets =
		(1 << WIDX_CLOSE) |
		(1 << WIDX_DECREMENT) |
		(1 << WIDX_INCREMENT) |
		(1 << WIDX_FLOOR) |
		(1 << WIDX_WALL) |
		(1 << WIDX_PAINTMODE) |
		(1 << WIDX_PREVIEW);
	window_init_scroll_widgets(window);
	window_push_others_below(window);

	RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_SURFACE, uint8) = 255;
	RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_EDGE, uint8) = 255;
	LandPaintMode = false;
	_selectedFloorTexture = 0;
	_selectedWallTexture = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_LAND_RAISE_COST, money32) = MONEY32_UNDEFINED;
	RCT2_GLOBAL(RCT2_ADDRESS_LAND_LOWER_COST, money32) = MONEY32_UNDEFINED;
}

/**
 *
 *  rct2: 0x006640A5
 */
static void window_land_close()
{
	// If the tool wasn't changed, turn tool off
	if (land_tool_is_active())
		tool_cancel();
}

/**
 *
 *  rct2: 0x00664064
 */
static void window_land_mouseup()
{
	int limit;
	short widgetIndex;
	rct_window *w;

	window_widget_get_registers(w, widgetIndex);

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_DECREMENT:
		// Decrement land tool size
		RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16)--;

		// FEATURE: minimum size is always 0
		limit = 0;
		// limit = (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 2 ? 0 : 1);

		if (RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) < limit)
			RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) = limit;

		// Invalidate the window
		window_invalidate(w);
		break;
	case WIDX_INCREMENT:
		// Increment land tool size
		RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16)++;

		// FEATURE: maximum size is 64
		limit = 64;
		// limit = (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 2 ? 7 : 5);
		
		if (RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) > limit)
			RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) = limit;

		// Invalidate the window
		window_invalidate(w);
		break;
	case WIDX_PAINTMODE:
		LandPaintMode ^= 1;
		window_invalidate(w);
		break;
	case WIDX_PREVIEW:
		window_land_inputsize(w);
		break;
	}
}

/**
 *
 *  rct2: 0x0066407B
 */
static void window_land_mousedown(int widgetIndex, rct_window*w, rct_widget* widget)
{
	int i;

	switch (widgetIndex) {
	case WIDX_FLOOR:
		for (i = 0; i < 14; i++) {
			gDropdownItemsFormat[i] = -1;
			gDropdownItemsArgs[i] = SPR_FLOOR_TEXTURE_GRASS + window_land_floor_texture_order[i];
			if (window_land_floor_texture_order[i] == _selectedFloorTexture)
				RCT2_GLOBAL(0x009DEBA2, sint16) = i;
		}
		window_dropdown_show_image(
			w->x + widget->left, w->y + widget->top,
			widget->bottom - widget->top,
			w->colours[2],
			0,
			14,
			47, 36,
			gAppropriateImageDropdownItemsPerRow[14]
		);
		break;
	case WIDX_WALL:
		for (i = 0; i < 4; i++) {
			gDropdownItemsFormat[i] = -1;
			gDropdownItemsArgs[i] = SPR_WALL_TEXTURE_ROCK + window_land_wall_texture_order[i];
			if (window_land_wall_texture_order[i] == _selectedWallTexture)
				RCT2_GLOBAL(0x009DEBA2, sint16) = i;
		}
		window_dropdown_show_image(
			w->x + widget->left, w->y + widget->top,
			widget->bottom - widget->top,
			w->colours[2],
			0,
			4,
			47, 36,
			gAppropriateImageDropdownItemsPerRow[4]
		);
		break;
	case WIDX_PREVIEW:
		window_land_inputsize(w);
		break;
	}
}

/**
 *
 *  rct2: 0x00664090
 */
static void window_land_dropdown()
{
	int type;
	short dropdownIndex, widgetIndex;
	rct_window *w;

	window_dropdown_get_registers(w, widgetIndex, dropdownIndex);

	switch (widgetIndex) {
	case WIDX_FLOOR:
		if (dropdownIndex == -1)
			dropdownIndex = RCT2_GLOBAL(0x009DEBA2, sint16);

		type = (dropdownIndex == -1) ?
			_selectedFloorTexture :
			*((uint32*)&gDropdownItemsArgs[dropdownIndex]) - SPR_FLOOR_TEXTURE_GRASS;

		if (RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_SURFACE, uint8) == type) {
			RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_SURFACE, uint8) = 255;
		} else {
			RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_SURFACE, uint8) = type;
			_selectedFloorTexture = type;
		}
		window_invalidate(w);
		break;
	case WIDX_WALL:
		if (dropdownIndex == -1)
			dropdownIndex = RCT2_GLOBAL(0x009DEBA2, sint16);

		type = (dropdownIndex == -1) ?
			_selectedWallTexture :
			*((uint32*)&gDropdownItemsArgs[dropdownIndex]) - SPR_WALL_TEXTURE_ROCK;

		if (RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_EDGE, uint8) == type) {
			RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_EDGE, uint8) = 255;
		} else {
			RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_EDGE, uint8) = type;
			_selectedWallTexture = type;
		}
		window_invalidate(w);
		break;
	}
}

static void window_land_textinput()
{
	uint8 result;
	short widgetIndex;
	rct_window *w;
	char *text;
	int size;
	char* end;

	window_textinput_get_registers(w, widgetIndex, result, text);

	if (widgetIndex != WIDX_PREVIEW || !result)
		return;

	size = strtol(text, &end, 10);
	if (*end == '\0') {
		if (size < 0) size = 0;
		if (size > 64) size = 64;
		RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) = size;
		window_invalidate(w);
	}
}

static void window_land_inputsize(rct_window *w)
{
	((uint16*)TextInputDescriptionArgs)[0] = 0;
	((uint16*)TextInputDescriptionArgs)[1] = 64;
	window_text_input_open(w, WIDX_PREVIEW, 5128, 5129, STR_NONE, STR_NONE, 3);
}

/**
 *
 *  rct2: 0x00664272
 */
static void window_land_update(rct_window *w)
{
	if (!land_tool_is_active())
		window_close(w);
}

/**
 *
 *  rct2: 0x00663F20
 */
static void window_land_invalidate()
{
	rct_window *w;

	window_get_register(w);
	colour_scheme_update(w);

	w->pressed_widgets = (1 << WIDX_PREVIEW);
	if (RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_SURFACE, uint8) != 255)
		w->pressed_widgets |= (1 << WIDX_FLOOR);
	if (RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_EDGE, uint8) != 255)
		w->pressed_widgets |= (1 << WIDX_WALL);
	if (LandPaintMode != 0)
		w->pressed_widgets |= (1 << WIDX_PAINTMODE);

	window_land_widgets[WIDX_FLOOR].image = SPR_FLOOR_TEXTURE_GRASS + _selectedFloorTexture;
	window_land_widgets[WIDX_WALL].image = SPR_WALL_TEXTURE_ROCK + _selectedWallTexture;
	window_land_widgets[WIDX_PREVIEW].image = RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) <= 7 ?
		SPR_LAND_TOOL_SIZE_0 + RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) :
		0xFFFFFFFF;
}

/**
 *
 *  rct2: 0x00663F7C
 */
static void window_land_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;
	int x, y, numTiles;
	money32 price;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);

	x = w->x + (window_land_widgets[WIDX_PREVIEW].left + window_land_widgets[WIDX_PREVIEW].right) / 2;
	y = w->y + (window_land_widgets[WIDX_PREVIEW].top + window_land_widgets[WIDX_PREVIEW].bottom) / 2;

	// FEATURE larger land tool size support
	if (RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) > 7) {
		RCT2_GLOBAL(0x009BC677, char) = FORMAT_BLACK;
		RCT2_GLOBAL(0x009BC678, char) = FORMAT_COMMA16;
		RCT2_GLOBAL(0x009BC679, char) = 0;
		RCT2_GLOBAL(0x013CE952, sint16) = RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16);
		gfx_draw_string_centred(dpi, 3165, x, y - 2, 0, (void*)0x013CE952);
	}

	x = w->x + (window_land_widgets[WIDX_PREVIEW].left + window_land_widgets[WIDX_PREVIEW].right) / 2 + 17;
	y = w->y + window_land_widgets[WIDX_PREVIEW].bottom + 5;

	// Draw raise cost amount
	if (RCT2_GLOBAL(RCT2_ADDRESS_LAND_RAISE_COST, uint32) != MONEY32_UNDEFINED && RCT2_GLOBAL(RCT2_ADDRESS_LAND_RAISE_COST, uint32) != 0)
		gfx_draw_string_centred(dpi, 984, x, y, 0, (void*)RCT2_ADDRESS_LAND_RAISE_COST);
	y += 10;

	// Draw lower cost amount
	if (RCT2_GLOBAL(RCT2_ADDRESS_LAND_LOWER_COST, uint32) != MONEY32_UNDEFINED && RCT2_GLOBAL(RCT2_ADDRESS_LAND_LOWER_COST, uint32) != 0)
		gfx_draw_string_centred(dpi, 985, x, y, 0, (void*)RCT2_ADDRESS_LAND_LOWER_COST);
	y += 50;

	// Draw paint price
	numTiles = RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16) * RCT2_GLOBAL(RCT2_ADDRESS_LAND_TOOL_SIZE, sint16);
	price = 0;
	if (RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_SURFACE, uint8) != 255)
		price += numTiles * land_pricing[RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_SURFACE, uint8)];
	if (RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_EDGE, uint8) != 255)
		price += numTiles * 100;

	if (price != 0 && !(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY)) {
		RCT2_GLOBAL(0x013CE952, sint32) = price;
		gfx_draw_string_centred(dpi, 986, x, y, 0, (void*)0x013CE952);
	}
}
