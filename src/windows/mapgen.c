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
#include "../world/mapgen.h"
#include "../world/scenery.h"
#include "dropdown.h"
#include "../interface/themes.h"

enum {
	WINDOW_MAPGEN_PAGE_BASE,
	WINDOW_MAPGEN_PAGE_RANDOM,
	WINDOW_MAPGEN_PAGE_SIMPLEX,
	WINDOW_MAPGEN_PAGE_COUNT
};

enum {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_PAGE_BACKGROUND,
	WIDX_TAB_1,
	WIDX_TAB_2,
	WIDX_TAB_3,

	WIDX_GENERATE,

	WIDX_MAP_SIZE = 8,
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

	WIDX_RANDOM_TERRAIN = 8,
	WIDX_PLACE_TREES,

	WIDX_SIMPLEX_LABEL = 8,
	WIDX_SIMPLEX_LOW,
	WIDX_SIMPLEX_LOW_UP,
	WIDX_SIMPLEX_LOW_DOWN,
	WIDX_SIMPLEX_HIGH,
	WIDX_SIMPLEX_HIGH_UP,
	WIDX_SIMPLEX_HIGH_DOWN,
	WIDX_SIMPLEX_BASE_FREQ,
	WIDX_SIMPLEX_BASE_FREQ_UP,
	WIDX_SIMPLEX_BASE_FREQ_DOWN,
	WIDX_SIMPLEX_OCTAVES,
	WIDX_SIMPLEX_OCTAVES_UP,
	WIDX_SIMPLEX_OCTAVES_DOWN,
	WIDX_SIMPLEX_MAP_SIZE,
	WIDX_SIMPLEX_MAP_SIZE_UP,
	WIDX_SIMPLEX_MAP_SIZE_DOWN,
	WIDX_SIMPLEX_WATER_LEVEL,
	WIDX_SIMPLEX_WATER_LEVEL_UP,
	WIDX_SIMPLEX_WATER_LEVEL_DOWN,
	WIDX_SIMPLEX_FLOOR_TEXTURE,
	WIDX_SIMPLEX_WALL_TEXTURE,
};

#pragma region Widgets

static rct_widget window_mapgen_base_widgets[] = {
	{ WWT_FRAME,			0,	0,		299,	0,		195,	0xFFFFFFFF,				STR_NONE },
	{ WWT_CAPTION,			0,	1,		298,	1,		14,		2690,					STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,			0,	287,	297,	2,		13,		STR_CLOSE_X,			STR_CLOSE_WINDOW_TIP },
	{ WWT_RESIZE,			1,	0,		299,	43,		195,	0xFFFFFFFF,				STR_NONE },
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,				STR_NONE },
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x2000144E,				STR_NONE },
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,				STR_NONE },

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
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,				STR_NONE },

	{ WWT_DROPDOWN_BUTTON,	1,	104,	198,	52,		63,		2694,					STR_NONE },

	{ WWT_CHECKBOX,			1,	4,		198,	52,		63,		2695,					STR_NONE },
	{ WWT_CHECKBOX,			1,	4,		198,	70,		81,		2696,					STR_NONE },
	{ WIDGETS_END },
};

static rct_widget window_mapgen_simplex_widgets[] = {
	{ WWT_FRAME,			0,	0,		299,	0,		195,	0xFFFFFFFF,				STR_NONE },
	{ WWT_CAPTION,			0,	1,		298,	1,		14,		2690,					STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,			0,	287,	297,	2,		13,		STR_CLOSE_X,			STR_CLOSE_WINDOW_TIP },
	{ WWT_RESIZE,			1,	0,		299,	43,		195,	0xFFFFFFFF,				STR_NONE },
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,				STR_NONE },
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x2000144E,				STR_NONE },
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,				STR_NONE },

	{ WWT_DROPDOWN_BUTTON,	1,	104,	198,	52,		63,		2694,					STR_NONE },

	{ WWT_12,				1,	4,		198,	52,		63,		2685,					STR_NONE },

	{ WWT_SPINNER,			1,	104,	198,	70,		81,		STR_NONE,				STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	71,		75,		STR_NUMERIC_UP,			STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	76,		80,		STR_NUMERIC_DOWN,		STR_NONE },

	{ WWT_SPINNER,			1,	104,	198,	88,		99,		STR_NONE,				STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	89,		93,		STR_NUMERIC_UP,			STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	94,		98,		STR_NUMERIC_DOWN,		STR_NONE },

	{ WWT_SPINNER,			1,	104,	198,	106,	117,	STR_NONE,				STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	107,	111,	STR_NUMERIC_UP,			STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	112,	116,	STR_NUMERIC_DOWN,		STR_NONE },

	{ WWT_SPINNER,			1,	104,	198,	124,	135,	STR_NONE,				STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	125,	129,	STR_NUMERIC_UP,			STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	130,	134,	STR_NUMERIC_DOWN,		STR_NONE },

	{ WWT_SPINNER,			1,	104,	198,	148,	159,	STR_NONE,				STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	149,	153,	STR_NUMERIC_UP,			STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	154,	158,	STR_NUMERIC_DOWN,		STR_NONE },

	{ WWT_SPINNER,			1,	104,	198,	166,	177,	STR_NONE,				STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	167,	171,	STR_NUMERIC_UP,			STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	172,	176,	STR_NUMERIC_DOWN,		STR_NONE },

	{ WWT_FLATBTN,			1,	225,	271,	68,		103,	0xFFFFFFFF,				STR_CHANGE_BASE_LAND_TIP },
	{ WWT_FLATBTN,			1,	225,	271,	104,	139,	0xFFFFFFFF,				STR_CHANGE_VERTICAL_LAND_TIP },
	
	{ WIDGETS_END },
};

const int window_mapgen_tab_animation_divisor[] = { 1, 1, 1 };
const int window_mapgen_tab_animation_frames[] = { 1, 1, 1 };


static rct_widget *window_mapgen_page_widgets[] = {
	window_mapgen_base_widgets,
	window_mapgen_random_widgets,
	window_mapgen_simplex_widgets
};

#pragma endregion

#pragma region Events

static void window_mapgen_emptysub() { }

static void window_mapgen_base_mouseup();
static void window_mapgen_base_mousedown(int widgetIndex, rct_window *w, rct_widget* widget);
static void window_mapgen_base_dropdown();
static void window_mapgen_base_update(rct_window *w);
static void window_mapgen_textinput();
static void window_mapgen_base_invalidate();
static void window_mapgen_base_paint();
static void window_mapgen_random_mouseup();
static void window_mapgen_random_mousedown(int widgetIndex, rct_window*w, rct_widget* widget);
static void window_mapgen_random_update(rct_window *w);
static void window_mapgen_random_invalidate();
static void window_mapgen_random_paint();
static void window_mapgen_simplex_mouseup();
static void window_mapgen_simplex_mousedown(int widgetIndex, rct_window *w, rct_widget* widget);
static void window_mapgen_simplex_dropdown();
static void window_mapgen_simplex_update(rct_window *w);
static void window_mapgen_simplex_invalidate();
static void window_mapgen_simplex_paint();

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
	window_mapgen_textinput,
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

static void* window_mapgen_simplex_events[] = {
	window_mapgen_emptysub,
	window_mapgen_simplex_mouseup,
	window_mapgen_emptysub,
	window_mapgen_simplex_mousedown,
	window_mapgen_simplex_dropdown,
	window_mapgen_emptysub,
	window_mapgen_simplex_update,
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
	window_mapgen_simplex_invalidate,
	window_mapgen_simplex_paint,
	window_mapgen_emptysub
};

static void* window_mapgen_page_events[] = {
	window_mapgen_base_events,
	window_mapgen_random_events,
	window_mapgen_simplex_events
};

#pragma endregion

#pragma region Enabled widgets

static uint32 window_mapgen_page_enabled_widgets[] = {
	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_TAB_2) |
	(1 << WIDX_TAB_3) |
	(1 << WIDX_GENERATE) |
	(1 << WIDX_MAP_SIZE) |
	(1 << WIDX_MAP_SIZE_UP) |
	(1 << WIDX_MAP_SIZE_DOWN) |
	(1 << WIDX_BASE_HEIGHT) |
	(1 << WIDX_BASE_HEIGHT_UP) |
	(1 << WIDX_BASE_HEIGHT_DOWN) |
	(1 << WIDX_WATER_LEVEL) |
	(1 << WIDX_WATER_LEVEL_UP) |
	(1 << WIDX_WATER_LEVEL_DOWN) |
	(1 << WIDX_FLOOR_TEXTURE) |
	(1 << WIDX_WALL_TEXTURE),

	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_TAB_2) |
	(1 << WIDX_TAB_3) |
	(1 << WIDX_GENERATE) |
	(1 << WIDX_RANDOM_TERRAIN) |
	(1 << WIDX_PLACE_TREES),

	(1 << WIDX_CLOSE) |
	(1 << WIDX_TAB_1) |
	(1 << WIDX_TAB_2) |
	(1 << WIDX_TAB_3) |
	(1 << WIDX_GENERATE) |
	(1 << WIDX_SIMPLEX_LABEL) |
	(1 << WIDX_SIMPLEX_LOW) |
	(1 << WIDX_SIMPLEX_LOW_UP) |
	(1 << WIDX_SIMPLEX_LOW_DOWN) |
	(1 << WIDX_SIMPLEX_HIGH) |
	(1 << WIDX_SIMPLEX_HIGH_UP) |
	(1 << WIDX_SIMPLEX_HIGH_DOWN) |
	(1 << WIDX_SIMPLEX_BASE_FREQ) |
	(1 << WIDX_SIMPLEX_BASE_FREQ_UP) |
	(1 << WIDX_SIMPLEX_BASE_FREQ_DOWN) |
	(1 << WIDX_SIMPLEX_OCTAVES) |
	(1 << WIDX_SIMPLEX_OCTAVES_UP) |
	(1 << WIDX_SIMPLEX_OCTAVES_DOWN) |
	(1 << WIDX_SIMPLEX_MAP_SIZE) |
	(1 << WIDX_SIMPLEX_MAP_SIZE_UP) |
	(1 << WIDX_SIMPLEX_MAP_SIZE_DOWN) |
	(1 << WIDX_SIMPLEX_WATER_LEVEL) |
	(1 << WIDX_SIMPLEX_WATER_LEVEL_UP) |
	(1 << WIDX_SIMPLEX_WATER_LEVEL_DOWN) |
	(1 << WIDX_SIMPLEX_FLOOR_TEXTURE) |
	(1 << WIDX_SIMPLEX_WALL_TEXTURE)
};

static uint32 window_mapgen_page_hold_down_widgets[] = {
	(1 << WIDX_MAP_SIZE_UP) |
	(1 << WIDX_MAP_SIZE_DOWN) |
	(1 << WIDX_BASE_HEIGHT_UP) |
	(1 << WIDX_BASE_HEIGHT_DOWN) |
	(1 << WIDX_WATER_LEVEL_UP) |
	(1 << WIDX_WATER_LEVEL_DOWN),

	0,

	(1 << WIDX_SIMPLEX_LOW_UP) |
	(1 << WIDX_SIMPLEX_LOW_DOWN) |
	(1 << WIDX_SIMPLEX_HIGH_UP) |
	(1 << WIDX_SIMPLEX_HIGH_DOWN) |
	(1 << WIDX_SIMPLEX_BASE_FREQ_UP) |
	(1 << WIDX_SIMPLEX_BASE_FREQ_DOWN) |
	(1 << WIDX_SIMPLEX_OCTAVES_UP) |
	(1 << WIDX_SIMPLEX_OCTAVES_DOWN) |
	(1 << WIDX_SIMPLEX_MAP_SIZE_UP) |
	(1 << WIDX_SIMPLEX_MAP_SIZE_DOWN) |
	(1 << WIDX_SIMPLEX_WATER_LEVEL_UP) |
	(1 << WIDX_SIMPLEX_WATER_LEVEL_DOWN)
};

#pragma endregion

const int window_mapgen_tab_animation_loops[] = { 16, 16 };

#define MAPSIZE_MIN	16
#define MAPSIZE_MAX 256
#define BASESIZE_MIN 0
#define BASESIZE_MAX 60
#define WATERLEVEL_MIN 0
#define WATERLEVEL_MAX 54

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
static int _waterLevel = 16;
static int _floorTexture = TERRAIN_GRASS;
static int _wallTexture = TERRAIN_EDGE_ROCK;
static int _randomTerrrain = 1;
static int _placeTrees = 1;

static int	 _simplex_low = 6;
static int	 _simplex_high = 10;
static sint16 _simplex_base_freq = 60;
static int	 _simplex_octaves = 4;

rct_window *window_mapgen_open()
{
	rct_window *w;

	w = window_bring_to_front_by_class(WC_MAPGEN);
	if (w == NULL) {
		w = window_create_centred(
			300,
			200,
			window_mapgen_page_events[0],
			WC_MAPGEN,
			WF_10
		);
		w->number = 0;
		w->frame_no = 0;
	}

	w->page = WINDOW_MAPGEN_PAGE_BASE;
	window_invalidate(w);
	w->widgets = window_mapgen_page_widgets[WINDOW_MAPGEN_PAGE_BASE];
	w->enabled_widgets = window_mapgen_page_enabled_widgets[WINDOW_MAPGEN_PAGE_BASE];
	w->hold_down_widgets = window_mapgen_page_hold_down_widgets[WINDOW_MAPGEN_PAGE_BASE];
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
	case WIDX_TAB_3:
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
	case WIDX_MAP_SIZE:
		((uint16*)TextInputDescriptionArgs)[0] = MAPSIZE_MIN;
		((uint16*)TextInputDescriptionArgs)[1] = MAPSIZE_MAX;
		window_text_input_open(w, WIDX_MAP_SIZE, 5130, 5131, 5182, _mapSize, 4);
		break;
	case WIDX_BASE_HEIGHT:
		((uint16*)TextInputDescriptionArgs)[0] = (BASESIZE_MIN - 12) / 2;
		((uint16*)TextInputDescriptionArgs)[1] = (BASESIZE_MAX - 12) / 2;
		window_text_input_open(w, WIDX_BASE_HEIGHT, 5183, 5184, 5182, (_baseHeight - 12) / 2, 3);
		break;
	case WIDX_WATER_LEVEL:
		((uint16*)TextInputDescriptionArgs)[0] = (WATERLEVEL_MIN - 12) / 2;
		((uint16*)TextInputDescriptionArgs)[1] = (WATERLEVEL_MAX - 12) / 2;
		window_text_input_open(w, WIDX_WATER_LEVEL, 5185, 5186, 5182, (_waterLevel - 12) / 2, 3);
		break;
	}
}

static void window_mapgen_base_mousedown(int widgetIndex, rct_window *w, rct_widget* widget)
{
	int i;

	switch (widgetIndex) {
	case WIDX_MAP_SIZE_UP:
		_mapSize = min(_mapSize + 1, MAPSIZE_MAX);
		window_invalidate(w);
		break;
	case WIDX_MAP_SIZE_DOWN:
		_mapSize = max(_mapSize - 1, MAPSIZE_MIN);
		window_invalidate(w);
		break;
	case WIDX_BASE_HEIGHT_UP:
		_baseHeight = min(_baseHeight + 2, BASESIZE_MAX);
		window_invalidate(w);
		break;
	case WIDX_BASE_HEIGHT_DOWN:
		_baseHeight = max(_baseHeight - 2, BASESIZE_MIN);
		window_invalidate(w);
		break;
	case WIDX_WATER_LEVEL_UP:
		_waterLevel = min(_waterLevel + 2, WATERLEVEL_MAX);
		window_invalidate(w);
		break;
	case WIDX_WATER_LEVEL_DOWN:
		_waterLevel = max(_waterLevel - 2, WATERLEVEL_MIN);
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

static void window_mapgen_textinput()
{
	uint8 result;
	short widgetIndex;
	rct_window *w;
	char *text;
	int value;
	char* end;

	window_textinput_get_registers(w, widgetIndex, result, text);

	if (!result) {
		return;
	}

	value = strtol(text, &end, 10);

	if (*end != '\0') {
		return;
	}

	switch (widgetIndex) {
	case WIDX_MAP_SIZE:
		_mapSize = clamp(MAPSIZE_MIN, value, MAPSIZE_MAX);
		break;
	case WIDX_BASE_HEIGHT:
		_baseHeight = clamp(BASESIZE_MIN, (value * 2) + 12, BASESIZE_MAX);
		break;
	case WIDX_WATER_LEVEL:
		_waterLevel = clamp(WATERLEVEL_MIN, (value * 2) + 12, WATERLEVEL_MAX);
		break;
	}

	window_invalidate(w);

}

static void window_mapgen_base_invalidate()
{
	rct_window *w;

	window_get_register(w);
	colour_scheme_update(w);

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
	case WIDX_TAB_3:
		window_mapgen_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_GENERATE:
		mapgenSettings.mapSize = _mapSize;
		mapgenSettings.height = _baseHeight + 2;
		mapgenSettings.waterLevel = _waterLevel + 2;
		mapgenSettings.floor = _randomTerrrain ? -1 : _floorTexture;
		mapgenSettings.wall = _randomTerrrain ? -1 : _wallTexture;
		mapgenSettings.trees = _placeTrees;

		mapgenSettings.simplex_low = rand() % 4;
		mapgenSettings.simplex_high = 12 + (rand() % (32 - 12));
		mapgenSettings.simplex_base_freq = 1.75f;
		mapgenSettings.simplex_octaves = 6;

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
	colour_scheme_update(w);

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

#pragma region Simplex page

static void window_mapgen_simplex_mouseup()
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
	case WIDX_TAB_3:
		window_mapgen_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_GENERATE:
		mapgenSettings.mapSize = _mapSize;

		mapgenSettings.height = _baseHeight;
		mapgenSettings.waterLevel = _waterLevel + 2;
		mapgenSettings.floor = _floorTexture;
		mapgenSettings.wall = _wallTexture;
		mapgenSettings.trees = 0;

		mapgenSettings.simplex_low = _simplex_low;
		mapgenSettings.simplex_high = _simplex_high;
		mapgenSettings.simplex_base_freq = ((float)_simplex_base_freq) / 100.00f;
		mapgenSettings.simplex_octaves = _simplex_octaves;

		mapgen_generate(&mapgenSettings);
		gfx_invalidate_screen();
		break;
	}
}

static void window_mapgen_simplex_mousedown(int widgetIndex, rct_window *w, rct_widget* widget)
{
	int i;

	switch (widgetIndex) {
	case WIDX_SIMPLEX_LOW_UP:
		_simplex_low = min(_simplex_low + 1, 24);
		window_invalidate(w);
		break;
	case WIDX_SIMPLEX_LOW_DOWN:
		_simplex_low = max(_simplex_low - 1, 0);
		window_invalidate(w);
		break;
	case WIDX_SIMPLEX_HIGH_UP:
		_simplex_high = min(_simplex_high + 1, 36);
		window_invalidate(w);
		break;
	case WIDX_SIMPLEX_HIGH_DOWN:
		_simplex_high = max(_simplex_high - 1, 0);
		window_invalidate(w);
		break;
	case WIDX_SIMPLEX_BASE_FREQ_UP:
		_simplex_base_freq = min(_simplex_base_freq + 5, 1000);
		window_invalidate(w);
		break;
	case WIDX_SIMPLEX_BASE_FREQ_DOWN:
		_simplex_base_freq = max(_simplex_base_freq - 5, 0);
		window_invalidate(w);
		break;
	case WIDX_SIMPLEX_OCTAVES_UP:
		_simplex_octaves = min(_simplex_octaves + 1, 10);
		window_invalidate(w);
		break;
	case WIDX_SIMPLEX_OCTAVES_DOWN:
		_simplex_octaves = max(_simplex_octaves - 1, 1);
		window_invalidate(w);
		break;
	case WIDX_SIMPLEX_MAP_SIZE_UP:
		_mapSize = min(_mapSize + 1, 256);
		window_invalidate(w);
		break;
	case WIDX_SIMPLEX_MAP_SIZE_DOWN:
		_mapSize = max(_mapSize - 1, 16);
		window_invalidate(w);
		break;
	case WIDX_SIMPLEX_WATER_LEVEL_UP:
		_waterLevel = min(_waterLevel + 2, 54);
		window_invalidate(w);
		break;
	case WIDX_SIMPLEX_WATER_LEVEL_DOWN:
		_waterLevel = max(_waterLevel - 2, 0);
		window_invalidate(w);
		break;
	case WIDX_SIMPLEX_FLOOR_TEXTURE:
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
	case WIDX_SIMPLEX_WALL_TEXTURE:
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

static void window_mapgen_simplex_dropdown()
{
	int type;
	short dropdownIndex, widgetIndex;
	rct_window *w;

	window_dropdown_get_registers(w, widgetIndex, dropdownIndex);

	switch (widgetIndex) {
	case WIDX_SIMPLEX_FLOOR_TEXTURE:
		if (dropdownIndex == -1)
			dropdownIndex = RCT2_GLOBAL(0x009DEBA2, sint16);

		type = (dropdownIndex == -1) ?
		_floorTexture :
					  *((uint32*)&gDropdownItemsArgs[dropdownIndex]) - SPR_FLOOR_TEXTURE_GRASS;

		if (RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_SURFACE, uint8) == type) {
			RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_SURFACE, uint8) = 255;
		}
		else {
			RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_SURFACE, uint8) = type;
			_floorTexture = type;
		}
		window_invalidate(w);
		break;
	case WIDX_SIMPLEX_WALL_TEXTURE:
		if (dropdownIndex == -1)
			dropdownIndex = RCT2_GLOBAL(0x009DEBA2, sint16);

		type = (dropdownIndex == -1) ?
		_wallTexture :
					 *((uint32*)&gDropdownItemsArgs[dropdownIndex]) - SPR_WALL_TEXTURE_ROCK;

		if (RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_EDGE, uint8) == type) {
			RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_EDGE, uint8) = 255;
		}
		else {
			RCT2_GLOBAL(RCT2_ADDRESS_SELECTED_TERRAIN_EDGE, uint8) = type;
			_wallTexture = type;
		}
		window_invalidate(w);
		break;
	}
}

static void window_mapgen_simplex_update(rct_window *w)
{
	// Tab animation
	if (++w->frame_no >= window_mapgen_tab_animation_loops[w->page])
		w->frame_no = 0;
	widget_invalidate(w, WIDX_TAB_3);
}

static void window_mapgen_simplex_invalidate()
{
	rct_window *w;

	window_get_register(w);
	colour_scheme_update(w);

	if (w->widgets != window_mapgen_page_widgets[WINDOW_MAPGEN_PAGE_SIMPLEX]) {
		w->widgets = window_mapgen_page_widgets[WINDOW_MAPGEN_PAGE_SIMPLEX];
		window_init_scroll_widgets(w);
	}

	w->widgets[WIDX_SIMPLEX_FLOOR_TEXTURE].image = SPR_FLOOR_TEXTURE_GRASS + _floorTexture;
	w->widgets[WIDX_SIMPLEX_WALL_TEXTURE].image = SPR_WALL_TEXTURE_ROCK + _wallTexture;

	window_mapgen_set_pressed_tab(w);
	window_mapgen_anchor_border_widgets(w);
}

static void window_mapgen_simplex_paint()
{
	uint16 arg;
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);
	window_mapgen_draw_tab_images(dpi, w);

	gfx_draw_string_left(dpi, 2686, 0, 0, w->x + 5, w->y + w->widgets[WIDX_SIMPLEX_LOW].top + 1);
	gfx_draw_string_left(dpi, 2687, 0, 0, w->x + 5, w->y + w->widgets[WIDX_SIMPLEX_HIGH].top + 1);
	gfx_draw_string_left(dpi, 2688, 0, 0, w->x + 5, w->y + w->widgets[WIDX_SIMPLEX_BASE_FREQ].top + 1);
	gfx_draw_string_left(dpi, 2689, 0, 0, w->x + 5, w->y + w->widgets[WIDX_SIMPLEX_OCTAVES].top + 1);
	gfx_draw_string_left(dpi, STR_MAP_SIZE, 0, 0, w->x + 5, w->y + w->widgets[WIDX_SIMPLEX_MAP_SIZE].top + 1);
	gfx_draw_string_left(dpi, 2692, 0, 0, w->x + 5, w->y + w->widgets[WIDX_SIMPLEX_WATER_LEVEL].top + 1);

	gfx_draw_string_left(dpi, 1737, &_simplex_low, w->colours[1], w->x + w->widgets[WIDX_SIMPLEX_LOW].left + 1, w->y + w->widgets[WIDX_SIMPLEX_LOW].top + 1);
	gfx_draw_string_left(dpi, 1737, &_simplex_high, w->colours[1], w->x + w->widgets[WIDX_SIMPLEX_HIGH].left + 1, w->y + w->widgets[WIDX_SIMPLEX_HIGH].top + 1);
	gfx_draw_string_left(dpi, 3311, &_simplex_base_freq, w->colours[1], w->x + w->widgets[WIDX_SIMPLEX_BASE_FREQ].left + 1, w->y + w->widgets[WIDX_SIMPLEX_BASE_FREQ].top + 1);
	gfx_draw_string_left(dpi, 1737, &_simplex_octaves, w->colours[1], w->x + w->widgets[WIDX_SIMPLEX_OCTAVES].left + 1, w->y + w->widgets[WIDX_SIMPLEX_OCTAVES].top + 1);

	uint16 mapSizeArgs[2] = { _mapSize, _mapSize };
	gfx_draw_string_left(dpi, 839, mapSizeArgs, w->colours[1], w->x + w->widgets[WIDX_SIMPLEX_MAP_SIZE].left + 1, w->y + w->widgets[WIDX_SIMPLEX_MAP_SIZE].top + 1);
	
	arg = (_waterLevel - 12) / 2;
	gfx_draw_string_left(dpi, 1737, &arg, w->colours[1], w->x + w->widgets[WIDX_SIMPLEX_WATER_LEVEL].left + 1, w->y + w->widgets[WIDX_SIMPLEX_WATER_LEVEL].top + 1);
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
	w->hold_down_widgets = window_mapgen_page_hold_down_widgets[page];
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
			int frame = w->frame_no / window_mapgen_tab_animation_divisor[w->page];
			spriteIndex += (frame % window_mapgen_tab_animation_frames[w->page]);
		}

		gfx_draw_sprite(dpi, spriteIndex, w->x + w->widgets[widgetIndex].left, w->y + w->widgets[widgetIndex].top, 0);
	}
}

static void window_mapgen_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w)
{
	window_mapgen_draw_tab_image(dpi, w, WINDOW_MAPGEN_PAGE_BASE, SPR_G2_TAB_LAND);
	window_mapgen_draw_tab_image(dpi, w, WINDOW_MAPGEN_PAGE_RANDOM, SPR_G2_TAB_TREE);
	window_mapgen_draw_tab_image(dpi, w, WINDOW_MAPGEN_PAGE_SIMPLEX, SPR_G2_TAB_PENCIL);
}

#pragma endregion