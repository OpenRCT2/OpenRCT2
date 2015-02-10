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
#include "../localisation/localisation.h"
#include "../input.h"
#include "../interface/widget.h"
#include "../interface/viewport.h"
#include "../interface/window.h"
#include "../sprites.h"
#include "../windows/scenery.h"
#include "../world/mapgen.h"
#include "dropdown.h"

enum {
	WINDOW_MAPGEN_PAGE_BASE,
	WINDOW_MAPGEN_PAGE_RANDOM,
	WINDOW_MAPGEN_PAGE_COUNT
};

enum {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_PAGE_BACKGROUND,
	WIDX_TAB_1,
	WIDX_TAB_2,

	WIDX_GENERATE,

	WIDX_MAP_SIZE = 7,
	WIDX_MAP_SIZE_UP,
	WIDX_MAP_SIZE_DOWN,
	WIDX_BASE_HEIGHT,
	WIDX_BASE_HEIGHT_UP,
	WIDX_BASE_HEIGHT_DOWN,
	WIDX_WATER_LEVEL,
	WIDX_WATER_LEVEL_UP,
	WIDX_WATER_LEVEL_DOWN,
	WIDX_FLOOR_TEXTURE,
	WIDX_WALL_TEXTURE,

	WIDX_RANDOM_TERRAIN = 7,
	WIDX_PLACE_TREES,
};

#pragma region Widgets

static rct_widget window_mapgen_base_widgets[] = {
	{ WWT_FRAME,			0,	0,		299,	0,		195,	0xFFFFFFFF,				STR_NONE },
	{ WWT_CAPTION,			0,	1,		298,	1,		14,		2690,					STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,			0,	287,	297,	2,		13,		STR_CLOSE_X,			STR_CLOSE_WINDOW_TIP },
	{ WWT_RESIZE,			1,	0,		299,	43,		195,	0xFFFFFFFF,				STR_NONE },
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,				STR_NONE },
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x2000144E,				STR_NONE },

	{ WWT_DROPDOWN_BUTTON,	1,	104,	198,	52,		63,		2694,					STR_NONE },

	{ WWT_SPINNER,			1,	104,	198,	52,		63,		STR_NONE,				STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	53,		57,		STR_NUMERIC_UP,			STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	58,		62,		STR_NUMERIC_DOWN,		STR_NONE },
	{ WWT_SPINNER,			1,	104,	198,	70,		81,		STR_NONE,				STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	71,		75,		STR_NUMERIC_UP,			STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	76,		80,		STR_NUMERIC_DOWN,		STR_NONE },
	{ WWT_SPINNER,			1,	104,	198,	88,		99,		STR_NONE,				STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	89,		93,		STR_NUMERIC_UP,			STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	94,		98,		STR_NUMERIC_DOWN,		STR_NONE },
	{ WWT_FLATBTN,			1,	104,	150,	106,	141,	0xFFFFFFFF,				STR_CHANGE_BASE_LAND_TIP },
	{ WWT_FLATBTN,			1,	151,	197,	106,	141,	0xFFFFFFFF,				STR_CHANGE_VERTICAL_LAND_TIP },
	{ WIDGETS_END },
};

static rct_widget window_mapgen_random_widgets[] = {
	{ WWT_FRAME,			0,	0,		299,	0,		195,	0xFFFFFFFF,				STR_NONE },
	{ WWT_CAPTION,			0,	1,		298,	1,		14,		2690,					STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,			0,	287,	297,	2,		13,		STR_CLOSE_X,			STR_CLOSE_WINDOW_TIP },
	{ WWT_RESIZE,			1,	0,		299,	43,		195,	0xFFFFFFFF,				STR_NONE },
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,				STR_NONE },
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x2000144E,				STR_NONE },

	{ WWT_DROPDOWN_BUTTON,	1,	104,	198,	52,		63,		2694,					STR_NONE },

	{ WWT_CHECKBOX,			1,	4,		198,	52,		63,		2695,					STR_NONE },
	{ WWT_CHECKBOX,			1,	4,		198,	70,		81,		2696,					STR_NONE },
	{ WIDGETS_END },
};

static rct_widget *window_mapgen_page_widgets[] = {
	window_mapgen_base_widgets,
	window_mapgen_random_widgets
};

#pragma endregion

#pragma region Events

static void window_mapgen_emptysub() { }

static void window_mapgen_base_mouseup();
static void window_mapgen_base_mousedown(int widgetIndex, rct_window *w, rct_widget* widget);
static void window_mapgen_base_dropdown();
static void window_mapgen_base_update(rct_window *w);
static void window_mapgen_base_invalidate();
static void window_mapgen_base_paint();
static void window_mapgen_random_mouseup();
static void window_mapgen_random_mousedown(int widgetIndex, rct_window*w, rct_widget* widget);
static void window_mapgen_random_update(rct_window *w);
static void window_mapgen_random_invalidate();
static void window_mapgen_random_paint();

static void* window_mapgen_base_events[] = {
	window_mapgen_emptysub,
	window_mapgen_base_mouseup,
	window_mapgen_emptysub,
	window_mapgen_base_mousedown,
	window_mapgen_base_dropdown,
	window_mapgen_emptysub,
	window_mapgen_base_update,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_base_invalidate,
	window_mapgen_base_paint,
	window_mapgen_emptysub
};

static void* window_mapgen_random_events[] = {
	window_mapgen_emptysub,
	window_mapgen_random_mouseup,
	window_mapgen_emptysub,
	window_mapgen_random_mousedown,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_random_update,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_emptysub,
	window_mapgen_random_invalidate,
	window_mapgen_random_paint,
	window_mapgen_emptysub
};

static void* window_mapgen_page_events[] = {
	window_mapgen_base_events,
	window_mapgen_random_events
};

#pragma endregion

#pragma region Enabled widgets

static uint32 window_mapgen_page_enabled_widgets[] = {
	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_TAB_2) |
	(1 << WIDX_GENERATE) |
	(1 << WIDX_MAP_SIZE_UP) |
	(1 << WIDX_MAP_SIZE_DOWN) |
	(1 << WIDX_BASE_HEIGHT_UP) |
	(1 << WIDX_BASE_HEIGHT_DOWN) |
	(1 << WIDX_WATER_LEVEL_UP) |
	(1 << WIDX_WATER_LEVEL_DOWN) |
	(1 << WIDX_FLOOR_TEXTURE) |
	(1 << WIDX_WALL_TEXTURE),

	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_TAB_2) |
	(1 << WIDX_GENERATE) |
	(1 << WIDX_RANDOM_TERRAIN) |
	(1 << WIDX_PLACE_TREES)
};

static uint32 window_mapgen_page_hold_down_widgets[] = {
	(1 << WIDX_MAP_SIZE_UP) |
	(1 << WIDX_MAP_SIZE_DOWN) |
	(1 << WIDX_BASE_HEIGHT_UP) |
	(1 << WIDX_BASE_HEIGHT_DOWN) |
	(1 << WIDX_WATER_LEVEL_UP) |
	(1 << WIDX_WATER_LEVEL_DOWN),

	0
};

#pragma endregion

const int window_mapgen_tab_animation_loops[] = { 16, 16 };

static void window_mapgen_set_page(rct_window *w, int page);
static void window_mapgen_set_pressed_tab(rct_window *w);
static void window_mapgen_anchor_border_widgets(rct_window *w);
static void window_mapgen_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w);

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

static int _mapSize = 150;
static int _baseHeight = 12;
static int _waterLevel = 6;
static int _floorTexture = TERRAIN_GRASS;
static int _wallTexture = TERRAIN_EDGE_ROCK;
static int _randomTerrrain = 1;
static int _placeTrees = 1;

rct_window *window_mapgen_open()
{
	rct_window *w;

	w = window_bring_to_front_by_class(WC_MAPGEN);
	if (w == NULL) {
		w = window_create(
			(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16) - 300) / 2, 
			(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16) - 200) / 2,
			300,
			200,
			window_mapgen_page_events[0],
			WC_MAPGEN,
			WF_10
		);
		w->number = 0;
		w->frame_no = 0;
		w->colours[0] = 12;
		w->colours[1] = 24;
		w->colours[2] = 24;
	}

	w->page = WINDOW_MAPGEN_PAGE_BASE;
	window_invalidate(w);
	w->widgets = window_mapgen_page_widgets[WINDOW_MAPGEN_PAGE_BASE];
	w->enabled_widgets = window_mapgen_page_enabled_widgets[WINDOW_MAPGEN_PAGE_BASE];
	w->var_020 = window_mapgen_page_hold_down_widgets[WINDOW_MAPGEN_PAGE_BASE];
	w->event_handlers = window_mapgen_page_events[WINDOW_MAPGEN_PAGE_BASE];
	w->pressed_widgets = 0;
	w->disabled_widgets = 0;
	window_init_scroll_widgets(w);

	return w;
}

#pragma region Base page

static void window_mapgen_base_mouseup()
{
	short widgetIndex;
	rct_window *w;
	mapgen_settings mapgenSettings;

	window_widget_get_registers(w, widgetIndex);

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
		window_mapgen_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_GENERATE:
		mapgenSettings.mapSize = _mapSize;
		mapgenSettings.height = _baseHeight + 2;
		mapgenSettings.waterLevel = _waterLevel + 2;
		mapgenSettings.floor = _floorTexture;
		mapgenSettings.wall = _wallTexture;

		mapgen_generate_blank(&mapgenSettings);
		gfx_invalidate_screen();
		break;
	}
}

static void window_mapgen_base_mousedown(int widgetIndex, rct_window *w, rct_widget* widget)
{
	int i;

	switch (widgetIndex) {
	case WIDX_MAP_SIZE_UP:
		_mapSize = min(_mapSize + 1, 256);
		window_invalidate(w);
		break;
	case WIDX_MAP_SIZE_DOWN:
		_mapSize = max(_mapSize - 1, 16);
		window_invalidate(w);
		break;
	case WIDX_BASE_HEIGHT_UP:
		_baseHeight = min(_baseHeight + 2, 60);
		window_invalidate(w);
		break;
	case WIDX_BASE_HEIGHT_DOWN:
		_baseHeight = max(_baseHeight - 2, 0);
		window_invalidate(w);
		break;
	case WIDX_WATER_LEVEL_UP:
		_waterLevel = min(_waterLevel + 2, 54);
		window_invalidate(w);
		break;
	case WIDX_WATER_LEVEL_DOWN:
		_waterLevel = max(_waterLevel - 2, 0);
		window_invalidate(w);
		break;
	case WIDX_FLOOR_TEXTURE:
		for (i = 0; i < 14; i++) {
			gDropdownItemsFormat[i] = -1;
			gDropdownItemsArgs[i] = SPR_FLOOR_TEXTURE_GRASS + window_land_floor_texture_order[i];
			if (window_land_floor_texture_order[i] == _floorTexture)
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
	case WIDX_WALL_TEXTURE:
		for (i = 0; i < 4; i++) {
			gDropdownItemsFormat[i] = -1;
			gDropdownItemsArgs[i] = SPR_WALL_TEXTURE_ROCK + window_land_wall_texture_order[i];
			if (window_land_wall_texture_order[i] == _wallTexture)
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
	}
}

static void window_mapgen_base_dropdown()
{
	int type;
	short dropdownIndex, widgetIndex;
	rct_window *w;

	window_dropdown_get_registers(w, widgetIndex, dropdownIndex);

	switch (widgetIndex) {
	case WIDX_FLOOR_TEXTURE:
		if (dropdownIndex == -1)
			dropdownIndex = RCT2_GLOBAL(0x009DEBA2, sint16);

		type = (dropdownIndex == -1) ?
			_floorTexture :
			*((uint32*)&gDropdownItemsArgs[dropdownIndex]) - SPR_FLOOR_TEXTURE_GRASS;

		if (RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_SURFACE, uint8) == type) {
			RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_SURFACE, uint8) = 255;
		} else {
			RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_SURFACE, uint8) = type;
			_floorTexture = type;
		}
		window_invalidate(w);
		break;
	case WIDX_WALL_TEXTURE:
		if (dropdownIndex == -1)
			dropdownIndex = RCT2_GLOBAL(0x009DEBA2, sint16);

		type = (dropdownIndex == -1) ?
			_wallTexture :
			*((uint32*)&gDropdownItemsArgs[dropdownIndex]) - SPR_WALL_TEXTURE_ROCK;

		if (RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_EDGE, uint8) == type) {
			RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_EDGE, uint8) = 255;
		} else {
			RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_EDGE, uint8) = type;
			_wallTexture = type;
		}
		window_invalidate(w);
		break;
	}
}

static void window_mapgen_base_update(rct_window *w)
{
	// Tab animation
	if (++w->frame_no >= window_mapgen_tab_animation_loops[w->page])
		w->frame_no = 0;
	widget_invalidate(w, WIDX_TAB_1);
}

static void window_mapgen_base_invalidate()
{
	rct_window *w;

	window_get_register(w);

	if (w->widgets != window_mapgen_page_widgets[WINDOW_MAPGEN_PAGE_BASE]) {
		w->widgets = window_mapgen_page_widgets[WINDOW_MAPGEN_PAGE_BASE];
		window_init_scroll_widgets(w);
	}

	w->widgets[WIDX_FLOOR_TEXTURE].image = SPR_FLOOR_TEXTURE_GRASS + _floorTexture;
	w->widgets[WIDX_WALL_TEXTURE].image = SPR_WALL_TEXTURE_ROCK + _wallTexture;

	window_mapgen_set_pressed_tab(w);
	window_mapgen_anchor_border_widgets(w);
}

static void window_mapgen_base_paint()
{
	uint16 arg;
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);
	window_mapgen_draw_tab_images(dpi, w);

	gfx_draw_string_left(dpi, STR_MAP_SIZE, 0, 0, w->x + 4, w->y + w->widgets[WIDX_MAP_SIZE].top + 1);
	gfx_draw_string_left(dpi, 2691, 0, 0, w->x + 4, w->y + w->widgets[WIDX_BASE_HEIGHT].top + 1);
	gfx_draw_string_left(dpi, 2692, 0, 0, w->x + 4, w->y + w->widgets[WIDX_WATER_LEVEL].top + 1);
	gfx_draw_string_left(dpi, 2693, 0, 0, w->x + 4, w->y + w->widgets[WIDX_FLOOR_TEXTURE].top + 1);

	uint16 mapSizeArgs[2] = { _mapSize, _mapSize };
	gfx_draw_string_left(dpi, 839, mapSizeArgs, w->colours[1], w->x + w->widgets[WIDX_MAP_SIZE].left + 1, w->y + w->widgets[WIDX_MAP_SIZE].top + 1);

	arg = (_baseHeight - 12) / 2;
	gfx_draw_string_left(dpi, 1737, &arg, w->colours[1], w->x + w->widgets[WIDX_BASE_HEIGHT].left + 1, w->y + w->widgets[WIDX_BASE_HEIGHT].top + 1);

	arg = (_waterLevel - 12) / 2;
	gfx_draw_string_left(dpi, 1737, &arg, w->colours[1], w->x + w->widgets[WIDX_WATER_LEVEL].left + 1, w->y + w->widgets[WIDX_WATER_LEVEL].top + 1);
}

#pragma endregion

#pragma region Random page

static void window_mapgen_random_mouseup()
{
	rct_window * w;
	short widgetIndex;
	mapgen_settings mapgenSettings;

	window_widget_get_registers(w, widgetIndex);

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
		window_mapgen_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_GENERATE:
		mapgenSettings.mapSize = _mapSize;
		mapgenSettings.height = _baseHeight + 2;
		mapgenSettings.waterLevel = _waterLevel + 2;
		mapgenSettings.floor = _randomTerrrain ? -1 : _floorTexture;
		mapgenSettings.wall = _randomTerrrain ? -1 : _wallTexture;
		mapgenSettings.trees = _placeTrees;

		mapgen_generate(&mapgenSettings);
		gfx_invalidate_screen();
		break;
	case WIDX_RANDOM_TERRAIN:
		_randomTerrrain ^= 1;
		break;
	case WIDX_PLACE_TREES:
		_placeTrees ^= 1;
		break;
	}
}

static void window_mapgen_random_mousedown(int widgetIndex, rct_window *w, rct_widget* widget)
{

}

static void window_mapgen_random_update(rct_window *w)
{
	// Tab animation
	if (++w->frame_no >= window_mapgen_tab_animation_loops[w->page])
		w->frame_no = 0;
	widget_invalidate(w, WIDX_TAB_2);
}

static void window_mapgen_random_invalidate()
{
	rct_window *w;

	window_get_register(w);

	if (w->widgets != window_mapgen_page_widgets[WINDOW_MAPGEN_PAGE_RANDOM]) {
		w->widgets = window_mapgen_page_widgets[WINDOW_MAPGEN_PAGE_RANDOM];
		window_init_scroll_widgets(w);
	}

	w->pressed_widgets = 0;
	if (_randomTerrrain)
		w->pressed_widgets |= 1 << WIDX_RANDOM_TERRAIN;
	if (_placeTrees)
		w->pressed_widgets |= 1 << WIDX_PLACE_TREES;

	window_mapgen_set_pressed_tab(w);
	window_mapgen_anchor_border_widgets(w);
}

static void window_mapgen_random_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);
	window_mapgen_draw_tab_images(dpi, w);
}

#pragma endregion

#pragma region Common

static void window_mapgen_set_page(rct_window *w, int page)
{
	w->page = page;
	w->frame_no = 0;
	if (w->viewport != NULL) {
		w->viewport->width = 0;
		w->viewport = NULL;
	}

	w->enabled_widgets = window_mapgen_page_enabled_widgets[page];
	w->var_020 = window_mapgen_page_hold_down_widgets[page];
	w->event_handlers = window_mapgen_page_events[page];
	w->widgets = window_mapgen_page_widgets[page];
	w->disabled_widgets = 0;
	w->pressed_widgets = 0;

	window_init_scroll_widgets(w);
	window_invalidate(w);
}

static void window_mapgen_set_pressed_tab(rct_window *w)
{
	int i;
	for (i = 0; i < WINDOW_MAPGEN_PAGE_COUNT; i++)
		w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
	w->pressed_widgets |= 1LL << (WIDX_TAB_1 + w->page);
}

static void window_mapgen_anchor_border_widgets(rct_window *w)
{
	int width = w->widgets[WIDX_GENERATE].right - w->widgets[WIDX_GENERATE].left;
	int height = w->widgets[WIDX_GENERATE].bottom - w->widgets[WIDX_GENERATE].top;

	w->widgets[WIDX_GENERATE].left = w->width - 3 - width;
	w->widgets[WIDX_GENERATE].right = w->width - 3;
	w->widgets[WIDX_GENERATE].bottom = w->height - 3;
	w->widgets[WIDX_GENERATE].top = w->height - 3 - height;

	w->widgets[WIDX_BACKGROUND].right = w->width - 1;
	w->widgets[WIDX_BACKGROUND].bottom = w->height - 1;
	w->widgets[WIDX_PAGE_BACKGROUND].right = w->width - 1;
	w->widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 3 - height - 3;
	w->widgets[WIDX_TITLE].right = w->width - 2;
	w->widgets[WIDX_CLOSE].left = w->width - 13;
	w->widgets[WIDX_CLOSE].right = w->width - 3;
}

static void window_mapgen_draw_tab_image(rct_drawpixelinfo *dpi, rct_window *w, int page, int spriteIndex)
{
	int widgetIndex = WIDX_TAB_1 + page;

	if (!(w->disabled_widgets & (1LL << widgetIndex))) {
		if (w->page == page) {
			int frame = w->frame_no / 2;
			if (page == WINDOW_MAPGEN_PAGE_BASE)
				frame %= 8;
			spriteIndex += frame;
		}

		gfx_draw_sprite(dpi, spriteIndex, w->x + w->widgets[widgetIndex].left, w->y + w->widgets[widgetIndex].top, 0);
	}
}

static void window_mapgen_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w)
{

}

#pragma endregion