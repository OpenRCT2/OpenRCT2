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

#include "../localisation/localisation.h"
#include "../input.h"
#include "../interface/widget.h"
#include "../interface/viewport.h"
#include "../interface/window.h"
#include "../sprites.h"
#include "../util/util.h"
#include "../world/mapgen.h"
#include "../world/scenery.h"
#include "dropdown.h"
#include "../interface/themes.h"
#include "../localisation/string_ids.h"

enum {
	WINDOW_MAPGEN_PAGE_BASE,
	WINDOW_MAPGEN_PAGE_RANDOM,
	WINDOW_MAPGEN_PAGE_SIMPLEX,
	WINDOW_MAPGEN_PAGE_HEIGHTMAP,
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
	WIDX_TAB_4,

	WIDX_GENERATE,

	TAB_BEGIN,

	WIDX_MAP_SIZE = TAB_BEGIN,
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

	WIDX_RANDOM_TERRAIN = TAB_BEGIN,
	WIDX_PLACE_TREES,

	WIDX_SIMPLEX_LABEL = TAB_BEGIN,
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

	WIDX_HEIGHTMAP_SMOOTH = TAB_BEGIN,
	WIDX_HEIGHTMAP_STRONG,
	WIDX_HEIGHTMAP_WATER_LEVEL,
	WIDX_HEIGHTMAP_WATER_LEVEL_UP,
	WIDX_HEIGHTMAP_WATER_LEVEL_DOWN,
};

#pragma region Widgets

static rct_widget BaseWidgets[] = {
	{ WWT_FRAME,			0,	0,		299,	0,		195,	0xFFFFFFFF,					STR_NONE },
	{ WWT_CAPTION,			0,	1,		298,	1,		14,		STR_MAPGEN_WINDOW_TITLE,	STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,			0,	287,	297,	2,		13,		STR_CLOSE_X,				STR_CLOSE_WINDOW_TIP },
	{ WWT_RESIZE,			1,	0,		299,	43,		195,	0xFFFFFFFF,					STR_NONE },
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x20000000 | SPR_TAB,		STR_NONE },
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x20000000 | SPR_TAB,		STR_NONE },
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x20000000 | SPR_TAB,		STR_NONE },
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x20000000 | SPR_TAB,		STR_NONE },

	{ WWT_DROPDOWN_BUTTON,	1,	104,	198,	52,		63,		STR_MAPGEN_ACTION_GENERATE,	STR_NONE },

	{ WWT_SPINNER,			1,	104,	198,	52,		63,		STR_NONE,					STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	53,		57,		STR_NUMERIC_UP,				STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	58,		62,		STR_NUMERIC_DOWN,			STR_NONE },
	{ WWT_SPINNER,			1,	104,	198,	70,		81,		STR_NONE,					STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	71,		75,		STR_NUMERIC_UP,				STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	76,		80,		STR_NUMERIC_DOWN,			STR_NONE },
	{ WWT_SPINNER,			1,	104,	198,	88,		99,		STR_NONE,					STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	89,		93,		STR_NUMERIC_UP,				STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	94,		98,		STR_NUMERIC_DOWN,			STR_NONE },
	{ WWT_FLATBTN,			1,	104,	150,	106,	141,	0xFFFFFFFF,					STR_CHANGE_BASE_LAND_TIP },
	{ WWT_FLATBTN,			1,	151,	197,	106,	141,	0xFFFFFFFF,					STR_CHANGE_VERTICAL_LAND_TIP },
	{ WIDGETS_END },
};

static rct_widget RandomWidgets[] = {
	{ WWT_FRAME,			0,	0,		299,	0,		195,	0xFFFFFFFF,							STR_NONE },
	{ WWT_CAPTION,			0,	1,		298,	1,		14,		STR_MAPGEN_WINDOW_TITLE,			STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,			0,	287,	297,	2,		13,		STR_CLOSE_X,						STR_CLOSE_WINDOW_TIP },
	{ WWT_RESIZE,			1,	0,		299,	43,		195,	0xFFFFFFFF,							STR_NONE },
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x20000000 | SPR_TAB,				STR_NONE },
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x20000000 | SPR_TAB,				STR_NONE },
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x20000000 | SPR_TAB,				STR_NONE },
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x20000000 | SPR_TAB,		STR_NONE },

	{ WWT_DROPDOWN_BUTTON,	1,	104,	198,	52,		63,		STR_MAPGEN_ACTION_GENERATE,			STR_NONE },

	{ WWT_CHECKBOX,			1,	4,		198,	52,		63,		STR_MAPGEN_OPTION_RANDOM_TERRAIN,	STR_NONE },
	{ WWT_CHECKBOX,			1,	4,		198,	70,		81,		STR_MAPGEN_OPTION_PLACE_TREES,		STR_NONE },
	{ WIDGETS_END },
};

static rct_widget SimplexWidgets[] = {
	{ WWT_FRAME,			0,	0,		299,	0,		195,	0xFFFFFFFF,					STR_NONE },
	{ WWT_CAPTION,			0,	1,		298,	1,		14,		STR_MAPGEN_WINDOW_TITLE,	STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,			0,	287,	297,	2,		13,		STR_CLOSE_X,				STR_CLOSE_WINDOW_TIP },
	{ WWT_RESIZE,			1,	0,		299,	43,		195,	0xFFFFFFFF,					STR_NONE },
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x20000000 | SPR_TAB,		STR_NONE },
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x20000000 | SPR_TAB,		STR_NONE },
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x20000000 | SPR_TAB,		STR_NONE },
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x20000000 | SPR_TAB,		STR_NONE },

	{ WWT_DROPDOWN_BUTTON,	1,	104,	198,	52,		63,		STR_MAPGEN_ACTION_GENERATE,	STR_NONE },

	{ WWT_12,				1,	4,		198,	52,		63,		STR_MAPGEN_SIMPLEX_NOISE,	STR_NONE },

	{ WWT_SPINNER,			1,	104,	198,	70,		81,		STR_NONE,					STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	71,		75,		STR_NUMERIC_UP,				STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	76,		80,		STR_NUMERIC_DOWN,			STR_NONE },

	{ WWT_SPINNER,			1,	104,	198,	88,		99,		STR_NONE,					STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	89,		93,		STR_NUMERIC_UP,				STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	94,		98,		STR_NUMERIC_DOWN,			STR_NONE },

	{ WWT_SPINNER,			1,	104,	198,	106,	117,	STR_NONE,					STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	107,	111,	STR_NUMERIC_UP,				STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	112,	116,	STR_NUMERIC_DOWN,			STR_NONE },

	{ WWT_SPINNER,			1,	104,	198,	124,	135,	STR_NONE,					STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	125,	129,	STR_NUMERIC_UP,				STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	130,	134,	STR_NUMERIC_DOWN,			STR_NONE },

	{ WWT_SPINNER,			1,	104,	198,	148,	159,	STR_NONE,					STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	149,	153,	STR_NUMERIC_UP,				STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	154,	158,	STR_NUMERIC_DOWN,			STR_NONE },

	{ WWT_SPINNER,			1,	104,	198,	166,	177,	STR_NONE,					STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	167,	171,	STR_NUMERIC_UP,				STR_NONE },
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	172,	176,	STR_NUMERIC_DOWN,			STR_NONE },

	{ WWT_FLATBTN,			1,	225,	271,	68,		103,	0xFFFFFFFF,					STR_CHANGE_BASE_LAND_TIP },
	{ WWT_FLATBTN,			1,	225,	271,	104,	139,	0xFFFFFFFF,					STR_CHANGE_VERTICAL_LAND_TIP },

	{ WIDGETS_END },
};

static rct_widget HeightmapWidgets[] = {
	{ WWT_FRAME,			0,	0,		299,	0,		195,	0xFFFFFFFF,							STR_NONE },
	{ WWT_CAPTION,			0,	1,		298,	1,		14,		STR_MAPGEN_WINDOW_TITLE,			STR_WINDOW_TITLE_TIP },
	{ WWT_CLOSEBOX,			0,	287,	297,	2,		13,		STR_CLOSE_X,						STR_CLOSE_WINDOW_TIP },
	{ WWT_RESIZE,			1,	0,		299,	43,		195,	0xFFFFFFFF,							STR_NONE },
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x20000000 | SPR_TAB,				STR_NONE },
	{ WWT_TAB,				1,	34,		64,		17,		43,		0x20000000 | SPR_TAB,				STR_NONE },
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x20000000 | SPR_TAB,				STR_NONE },
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x20000000 | SPR_TAB,		STR_NONE },

	{ WWT_DROPDOWN_BUTTON,	1,	104,	198,	52,		63,		STR_MAPGEN_ACTION_GENERATE,	STR_NONE }, // WIDX_GENERATE

	{ WWT_CHECKBOX,			1,	4,		103,	52,		63,		STR_MAPGEN_SMOOTH_TILE,		STR_NONE }, // WIDX_HEIGHTMAP_SMOOTH
	{ WWT_CHECKBOX,			1,	4,		103,	70,		81,		STR_MAPGEN_SMOOTH_STRONG,	STR_NONE }, // WIDX_HEIGHTMAP_STRONG

	{ WWT_SPINNER,			1,	104,	198,	124,	135,	STR_NONE,					STR_NONE }, // WIDX_HEIGHTMAP_WATER_LEVEL
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	125,	129,	STR_NUMERIC_UP,				STR_NONE }, // WIDX_HEIGHTMAP_WATER_LEVEL_UP
	{ WWT_DROPDOWN_BUTTON,	1,	187,	197,	130,	134,	STR_NUMERIC_DOWN,			STR_NONE }, // WIDX_HEIGHTMAP_WATER_LEVEL_DOWN

	{ WIDGETS_END },
};

static const sint32 TabAnimationDivisor[] = { 1, 1, 1, 1 };
static const sint32 TabAnimationFrames[] = { 1, 1, 1, 1 };

static rct_widget *PageWidgets[] = {
	BaseWidgets,
	RandomWidgets,
	SimplexWidgets,
	HeightmapWidgets
};

#pragma endregion

#pragma region Events

static void window_mapgen_base_mouseup(rct_window *w, sint32 widgetIndex);
static void window_mapgen_base_mousedown(sint32 widgetIndex, rct_window *w, rct_widget* widget);
static void window_mapgen_base_dropdown(rct_window *w, sint32 widgetIndex, sint32 dropdownIndex);
static void window_mapgen_base_update(rct_window *w);
static void window_mapgen_textinput(rct_window *w, sint32 widgetIndex, char *text);
static void window_mapgen_base_invalidate(rct_window *w);
static void window_mapgen_base_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_mapgen_random_mouseup(rct_window *w, sint32 widgetIndex);
static void window_mapgen_random_mousedown(sint32 widgetIndex, rct_window*w, rct_widget* widget);
static void window_mapgen_random_update(rct_window *w);
static void window_mapgen_random_invalidate(rct_window *w);
static void window_mapgen_random_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_mapgen_simplex_mouseup(rct_window *w, sint32 widgetIndex);
static void window_mapgen_simplex_mousedown(sint32 widgetIndex, rct_window *w, rct_widget* widget);
static void window_mapgen_simplex_dropdown(rct_window *w, sint32 widgetIndex, sint32 dropdownIndex);
static void window_mapgen_simplex_update(rct_window *w);
static void window_mapgen_simplex_invalidate(rct_window *w);
static void window_mapgen_simplex_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_mapgen_heightmap_mouseup(rct_window *w, sint32 widgetIndex);
static void window_mapgen_heightmap_invalidate(rct_window *w);
static void window_mapgen_heightmap_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list BaseEvents = {
	NULL,
	window_mapgen_base_mouseup,
	NULL,
	window_mapgen_base_mousedown,
	window_mapgen_base_dropdown,
	NULL,
	window_mapgen_base_update,
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
	window_mapgen_textinput,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_mapgen_base_invalidate,
	window_mapgen_base_paint,
	NULL
};

static rct_window_event_list RandomEvents = {
	NULL,
	window_mapgen_random_mouseup,
	NULL,
	window_mapgen_random_mousedown,
	NULL,
	NULL,
	window_mapgen_random_update,
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
	NULL,
	NULL,
	NULL,
	NULL,
	window_mapgen_random_invalidate,
	window_mapgen_random_paint,
	NULL
};

static rct_window_event_list SimplexEvents = {
	NULL,
	window_mapgen_simplex_mouseup,
	NULL,
	window_mapgen_simplex_mousedown,
	window_mapgen_simplex_dropdown,
	NULL,
	window_mapgen_simplex_update,
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
	window_mapgen_textinput,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_mapgen_simplex_invalidate,
	window_mapgen_simplex_paint,
	NULL
};

static rct_window_event_list HeightmapEvents = {
	NULL,
	window_mapgen_heightmap_mouseup,
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
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_mapgen_heightmap_invalidate,
	window_mapgen_heightmap_paint,
	NULL
};

static rct_window_event_list *PageEvents[] = {
	&BaseEvents,
	&RandomEvents,
	&SimplexEvents,
	&HeightmapEvents
};

#pragma endregion

#pragma region Enabled widgets

static uint32 PageEnabledWidgets[] = {
	(1ULL << WIDX_CLOSE) |
	(1ULL << WIDX_TAB_1) |
	(1ULL << WIDX_TAB_2) |
	(1ULL << WIDX_TAB_3) |
	(1ULL << WIDX_TAB_4) |
	(1ULL << WIDX_GENERATE) |
	(1ULL << WIDX_MAP_SIZE) |
	(1ULL << WIDX_MAP_SIZE_UP) |
	(1ULL << WIDX_MAP_SIZE_DOWN) |
	(1ULL << WIDX_BASE_HEIGHT) |
	(1ULL << WIDX_BASE_HEIGHT_UP) |
	(1ULL << WIDX_BASE_HEIGHT_DOWN) |
	(1ULL << WIDX_WATER_LEVEL) |
	(1ULL << WIDX_WATER_LEVEL_UP) |
	(1ULL << WIDX_WATER_LEVEL_DOWN) |
	(1ULL << WIDX_FLOOR_TEXTURE) |
	(1ULL << WIDX_WALL_TEXTURE),

	(1ULL << WIDX_CLOSE) |
	(1ULL << WIDX_TAB_1) |
	(1ULL << WIDX_TAB_2) |
	(1ULL << WIDX_TAB_3) |
	(1ULL << WIDX_TAB_4) |
	(1ULL << WIDX_GENERATE) |
	(1ULL << WIDX_RANDOM_TERRAIN) |
	(1ULL << WIDX_PLACE_TREES),

	(1ULL << WIDX_CLOSE) |
	(1ULL << WIDX_TAB_1) |
	(1ULL << WIDX_TAB_2) |
	(1ULL << WIDX_TAB_3) |
	(1ULL << WIDX_TAB_4) |
	(1ULL << WIDX_GENERATE) |
	(1ULL << WIDX_SIMPLEX_LABEL) |
	(1ULL << WIDX_SIMPLEX_LOW) |
	(1ULL << WIDX_SIMPLEX_LOW_UP) |
	(1ULL << WIDX_SIMPLEX_LOW_DOWN) |
	(1ULL << WIDX_SIMPLEX_HIGH) |
	(1ULL << WIDX_SIMPLEX_HIGH_UP) |
	(1ULL << WIDX_SIMPLEX_HIGH_DOWN) |
	(1ULL << WIDX_SIMPLEX_BASE_FREQ) |
	(1ULL << WIDX_SIMPLEX_BASE_FREQ_UP) |
	(1ULL << WIDX_SIMPLEX_BASE_FREQ_DOWN) |
	(1ULL << WIDX_SIMPLEX_OCTAVES) |
	(1ULL << WIDX_SIMPLEX_OCTAVES_UP) |
	(1ULL << WIDX_SIMPLEX_OCTAVES_DOWN) |
	(1ULL << WIDX_SIMPLEX_MAP_SIZE) |
	(1ULL << WIDX_SIMPLEX_MAP_SIZE_UP) |
	(1ULL << WIDX_SIMPLEX_MAP_SIZE_DOWN) |
	(1ULL << WIDX_SIMPLEX_WATER_LEVEL) |
	(1ULL << WIDX_SIMPLEX_WATER_LEVEL_UP) |
	(1ULL << WIDX_SIMPLEX_WATER_LEVEL_DOWN) |
	(1ULL << WIDX_SIMPLEX_FLOOR_TEXTURE) |
	(1ULL << WIDX_SIMPLEX_WALL_TEXTURE),

	(1ULL << WIDX_CLOSE) |
	(1ULL << WIDX_TAB_1) |
	(1ULL << WIDX_TAB_2) |
	(1ULL << WIDX_TAB_3) |
	(1ULL << WIDX_TAB_4) |
	(1ULL << WIDX_GENERATE) |
	(1ULL << WIDX_HEIGHTMAP_SMOOTH) |
	(1ULL << WIDX_HEIGHTMAP_STRONG) |
	(1ULL << WIDX_HEIGHTMAP_WATER_LEVEL) |
	(1ULL << WIDX_HEIGHTMAP_WATER_LEVEL_UP) |
	(1ULL << WIDX_HEIGHTMAP_WATER_LEVEL_DOWN)
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
	(1 << WIDX_SIMPLEX_WATER_LEVEL_DOWN),

	0
};

#pragma endregion

static const sint32 TabAnimationLoops[] = {
	16, 16, 16, 16
};

#define MINIMUM_MAP_SIZE_TECHNICAL 15
#define MAXIMUM_MAP_SIZE_TECHNICAL 256
#define MINIMUM_MAP_SIZE_PRACTICAL MINIMUM_MAP_SIZE_TECHNICAL-2
#define MAXIMUM_MAP_SIZE_PRACTICAL MAXIMUM_MAP_SIZE_TECHNICAL-2
#define BASESIZE_MIN 0
#define BASESIZE_MAX 60
#define WATERLEVEL_MIN 0
#define WATERLEVEL_MAX 54

static void window_mapgen_set_page(rct_window *w, sint32 page);
static void window_mapgen_set_pressed_tab(rct_window *w);
static void window_mapgen_anchor_border_widgets(rct_window *w);
static void window_mapgen_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w);

static char FloorTextureOrder[] = {
	TERRAIN_SAND_DARK, TERRAIN_SAND_LIGHT,  TERRAIN_DIRT,      TERRAIN_GRASS_CLUMPS, TERRAIN_GRASS,
	TERRAIN_ROCK,      TERRAIN_SAND,        TERRAIN_MARTIAN,   TERRAIN_CHECKERBOARD, TERRAIN_ICE,
	TERRAIN_GRID_RED,  TERRAIN_GRID_YELLOW, TERRAIN_GRID_BLUE, TERRAIN_GRID_GREEN
};

static char WallTextureOrder[] = {
	TERRAIN_EDGE_ROCK,       TERRAIN_EDGE_WOOD_RED,
	TERRAIN_EDGE_WOOD_BLACK, TERRAIN_EDGE_ICE,
	0, 0
};

static sint32 _mapSize = 150;
static sint32 _baseHeight = 12;
static sint32 _waterLevel = 16;
static sint32 _floorTexture = TERRAIN_GRASS;
static sint32 _wallTexture = TERRAIN_EDGE_ROCK;
static sint32 _randomTerrrain = 1;
static sint32 _placeTrees = 1;

static sint32 _simplex_low = 6;
static sint32 _simplex_high = 10;
static sint32 _simplex_base_freq = 60;
static sint32 _simplex_octaves = 4;

rct_window *window_mapgen_open()
{
	rct_window *w;

	w = window_bring_to_front_by_class(WC_MAPGEN);
	if (w == NULL) {
		w = window_create_centred(
			300,
			200,
			PageEvents[0],
			WC_MAPGEN,
			WF_10
		);
		w->number = 0;
		w->frame_no = 0;
	}

	w->page = WINDOW_MAPGEN_PAGE_BASE;
	window_invalidate(w);
	w->widgets = PageWidgets[WINDOW_MAPGEN_PAGE_BASE];
	w->enabled_widgets = PageEnabledWidgets[WINDOW_MAPGEN_PAGE_BASE];
	w->hold_down_widgets = window_mapgen_page_hold_down_widgets[WINDOW_MAPGEN_PAGE_BASE];
	w->event_handlers = PageEvents[WINDOW_MAPGEN_PAGE_BASE];
	w->pressed_widgets = 0;
	w->disabled_widgets = 0;
	window_init_scroll_widgets(w);

	return w;
}

#pragma region Base page

static void window_mapgen_base_mouseup(rct_window *w, sint32 widgetIndex)
{
	mapgen_settings mapgenSettings;

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
	case WIDX_TAB_3:
	case WIDX_TAB_4:
		window_mapgen_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_GENERATE:
		mapgenSettings.mapSize = _mapSize;
		mapgenSettings.height = _baseHeight + 2;
		mapgenSettings.water_level = _waterLevel + 2;
		mapgenSettings.floor = _floorTexture;
		mapgenSettings.wall = _wallTexture;

		mapgen_generate_blank(&mapgenSettings);
		gfx_invalidate_screen();
		break;
	case WIDX_MAP_SIZE:
		TextInputDescriptionArgs[0] = MINIMUM_MAP_SIZE_PRACTICAL;
		TextInputDescriptionArgs[1] = MAXIMUM_MAP_SIZE_PRACTICAL;
		// Practical map size is 2 lower than the technical map size
		window_text_input_open(w, WIDX_MAP_SIZE, STR_MAP_SIZE_2, STR_ENTER_MAP_SIZE, STR_FORMAT_INTEGER, _mapSize - 2, 4);
		break;
	case WIDX_BASE_HEIGHT:
		TextInputDescriptionArgs[0] = (BASESIZE_MIN - 12) / 2;
		TextInputDescriptionArgs[1] = (BASESIZE_MAX - 12) / 2;
		window_text_input_open(w, WIDX_BASE_HEIGHT, STR_BASE_HEIGHT, STR_ENTER_BASE_HEIGHT, STR_FORMAT_INTEGER, (_baseHeight - 12) / 2, 3);
		break;
	case WIDX_WATER_LEVEL:
		TextInputDescriptionArgs[0] = (WATERLEVEL_MIN - 12) / 2;
		TextInputDescriptionArgs[1] = (WATERLEVEL_MAX - 12) / 2;
		window_text_input_open(w, WIDX_WATER_LEVEL, STR_WATER_LEVEL, STR_ENTER_WATER_LEVEL, STR_FORMAT_INTEGER, (_waterLevel - 12) / 2, 3);
		break;
	}
}

static void window_mapgen_base_mousedown(sint32 widgetIndex, rct_window *w, rct_widget* widget)
{
	sint32 i;
	sint32 defaultIndex = -1;
	switch (widgetIndex) {
	case WIDX_MAP_SIZE_UP:
		_mapSize = min(_mapSize + 1, MAXIMUM_MAP_SIZE_TECHNICAL);
		window_invalidate(w);
		break;
	case WIDX_MAP_SIZE_DOWN:
		_mapSize = max(_mapSize - 1, MINIMUM_MAP_SIZE_TECHNICAL);
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
			gDropdownItemsArgs[i] = SPR_FLOOR_TEXTURE_GRASS + FloorTextureOrder[i];
			if (FloorTextureOrder[i] == _floorTexture)
				defaultIndex = i;
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
		gDropdownDefaultIndex = defaultIndex;
		break;
	case WIDX_WALL_TEXTURE:
		for (i = 0; i < 4; i++) {
			gDropdownItemsFormat[i] = -1;
			gDropdownItemsArgs[i] = SPR_WALL_TEXTURE_ROCK + WallTextureOrder[i];
			if (WallTextureOrder[i] == _wallTexture)
				defaultIndex = i;
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
		gDropdownDefaultIndex = defaultIndex;
		break;
	}
}

static void window_mapgen_base_dropdown(rct_window *w, sint32 widgetIndex, sint32 dropdownIndex)
{
	sint32 type;

	switch (widgetIndex) {
	case WIDX_FLOOR_TEXTURE:
		if (dropdownIndex == -1)
			dropdownIndex = gDropdownHighlightedIndex;

		type = (dropdownIndex == -1) ?
			_floorTexture :
			(uint32)gDropdownItemsArgs[dropdownIndex] - SPR_FLOOR_TEXTURE_GRASS;

		if (gLandToolTerrainSurface == type) {
			gLandToolTerrainSurface = 255;
		} else {
			gLandToolTerrainSurface = type;
			_floorTexture = type;
		}
		window_invalidate(w);
		break;
	case WIDX_WALL_TEXTURE:
		if (dropdownIndex == -1)
			dropdownIndex = gDropdownHighlightedIndex;

		type = (dropdownIndex == -1) ?
			_wallTexture :
			(uint32)gDropdownItemsArgs[dropdownIndex] - SPR_WALL_TEXTURE_ROCK;

		if (gLandToolTerrainEdge == type) {
			gLandToolTerrainEdge = 255;
		} else {
			gLandToolTerrainEdge = type;
			_wallTexture = type;
		}
		window_invalidate(w);
		break;
	}
}

static void window_mapgen_base_update(rct_window *w)
{
	// Tab animation
	if (++w->frame_no >= TabAnimationLoops[w->page])
		w->frame_no = 0;
	widget_invalidate(w, WIDX_TAB_1);
}

static void window_mapgen_textinput(rct_window *w, sint32 widgetIndex, char *text)
{
	sint32 value;
	char* end;

	if (text == NULL)
		return;

	value = strtol(text, &end, 10);

	if (*end != '\0') {
		return;
	}

	switch (widgetIndex) {
	case WIDX_MAP_SIZE:
	case WIDX_SIMPLEX_MAP_SIZE:
		// The practical size is 2 lower than the technical size
		value += 2;
		_mapSize = clamp(MINIMUM_MAP_SIZE_TECHNICAL, value, MAXIMUM_MAP_SIZE_TECHNICAL);
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

static void window_mapgen_base_invalidate(rct_window *w)
{
	colour_scheme_update(w);

	if (w->widgets != PageWidgets[WINDOW_MAPGEN_PAGE_BASE]) {
		w->widgets = PageWidgets[WINDOW_MAPGEN_PAGE_BASE];
		window_init_scroll_widgets(w);
	}

	w->widgets[WIDX_FLOOR_TEXTURE].image = SPR_FLOOR_TEXTURE_GRASS + _floorTexture;
	w->widgets[WIDX_WALL_TEXTURE].image = SPR_WALL_TEXTURE_ROCK + _wallTexture;

	window_mapgen_set_pressed_tab(w);
	window_mapgen_anchor_border_widgets(w);
}

static void window_mapgen_base_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	uint16 arg;

	window_draw_widgets(w, dpi);
	window_mapgen_draw_tab_images(dpi, w);

	gfx_draw_string_left(dpi, STR_MAP_SIZE, NULL, COLOUR_BLACK, w->x + 4, w->y + w->widgets[WIDX_MAP_SIZE].top + 1);
	gfx_draw_string_left(dpi, STR_BASE_HEIGHT_LABEL, NULL, COLOUR_BLACK, w->x + 4, w->y + w->widgets[WIDX_BASE_HEIGHT].top + 1);
	gfx_draw_string_left(dpi, STR_WATER_LEVEL_LABEL, NULL, COLOUR_BLACK, w->x + 4, w->y + w->widgets[WIDX_WATER_LEVEL].top + 1);
	gfx_draw_string_left(dpi, STR_TERRAIN_LABEL, NULL, COLOUR_BLACK, w->x + 4, w->y + w->widgets[WIDX_FLOOR_TEXTURE].top + 1);

	// The practical map size is 2 lower than the technical map size
	uint16 mapSizeArgs[2] = { _mapSize - 2, _mapSize -2 };
	gfx_draw_string_left(dpi, STR_RESOLUTION_X_BY_Y, mapSizeArgs, w->colours[1], w->x + w->widgets[WIDX_MAP_SIZE].left + 1, w->y + w->widgets[WIDX_MAP_SIZE].top + 1);

	arg = (_baseHeight - 12) / 2;
	gfx_draw_string_left(dpi, STR_COMMA16, &arg, w->colours[1], w->x + w->widgets[WIDX_BASE_HEIGHT].left + 1, w->y + w->widgets[WIDX_BASE_HEIGHT].top + 1);

	arg = (_waterLevel - 12) / 2;
	gfx_draw_string_left(dpi, STR_COMMA16, &arg, w->colours[1], w->x + w->widgets[WIDX_WATER_LEVEL].left + 1, w->y + w->widgets[WIDX_WATER_LEVEL].top + 1);
}

#pragma endregion

#pragma region Random page

static void window_mapgen_random_mouseup(rct_window *w, sint32 widgetIndex)
{
	mapgen_settings mapgenSettings;

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
	case WIDX_TAB_3:
	case WIDX_TAB_4:
		window_mapgen_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_GENERATE:
		mapgenSettings.mapSize = _mapSize;
		mapgenSettings.height = _baseHeight + 2;
		mapgenSettings.water_level = _waterLevel + 2;
		mapgenSettings.floor = _randomTerrrain ? -1 : _floorTexture;
		mapgenSettings.wall = _randomTerrrain ? -1 : _wallTexture;
		mapgenSettings.trees = _placeTrees;

		mapgenSettings.simplex_low = util_rand() % 4;
		mapgenSettings.simplex_high = 12 + (util_rand() % (32 - 12));
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

static void window_mapgen_random_mousedown(sint32 widgetIndex, rct_window *w, rct_widget* widget)
{

}

static void window_mapgen_random_update(rct_window *w)
{
	// Tab animation
	if (++w->frame_no >= TabAnimationLoops[w->page])
		w->frame_no = 0;
	widget_invalidate(w, WIDX_TAB_2);
}

static void window_mapgen_random_invalidate(rct_window *w)
{
	colour_scheme_update(w);

	if (w->widgets != PageWidgets[WINDOW_MAPGEN_PAGE_RANDOM]) {
		w->widgets = PageWidgets[WINDOW_MAPGEN_PAGE_RANDOM];
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

static void window_mapgen_random_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);
	window_mapgen_draw_tab_images(dpi, w);
}

#pragma endregion

#pragma region Simplex page

static void window_mapgen_simplex_mouseup(rct_window *w, sint32 widgetIndex)
{
	mapgen_settings mapgenSettings;

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
	case WIDX_TAB_3:
	case WIDX_TAB_4:
		window_mapgen_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_SIMPLEX_MAP_SIZE:
		TextInputDescriptionArgs[0] = MINIMUM_MAP_SIZE_PRACTICAL;
		TextInputDescriptionArgs[1] = MAXIMUM_MAP_SIZE_PRACTICAL;
		// Practical map size is 2 lower than the technical map size
		window_text_input_open(w, WIDX_SIMPLEX_MAP_SIZE, STR_MAP_SIZE_2, STR_ENTER_MAP_SIZE, STR_FORMAT_INTEGER, _mapSize - 2, 4);
		break;
	case WIDX_GENERATE:
		mapgenSettings.mapSize = _mapSize;

		mapgenSettings.height = _baseHeight;
		mapgenSettings.water_level = _waterLevel + 2;
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

static void window_mapgen_simplex_mousedown(sint32 widgetIndex, rct_window *w, rct_widget* widget)
{
	sint32 i;
	sint32 defaultIndex = -1;
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
		_mapSize = min(_mapSize + 1, MAXIMUM_MAP_SIZE_TECHNICAL);
		window_invalidate(w);
		break;
	case WIDX_SIMPLEX_MAP_SIZE_DOWN:
		_mapSize = max(_mapSize - 1, MINIMUM_MAP_SIZE_TECHNICAL);
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
			gDropdownItemsArgs[i] = SPR_FLOOR_TEXTURE_GRASS + FloorTextureOrder[i];
			if (FloorTextureOrder[i] == _floorTexture)
				defaultIndex = i;
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
		gDropdownDefaultIndex = defaultIndex;
		break;
	case WIDX_SIMPLEX_WALL_TEXTURE:
		for (i = 0; i < 4; i++) {
			gDropdownItemsFormat[i] = -1;
			gDropdownItemsArgs[i] = SPR_WALL_TEXTURE_ROCK + WallTextureOrder[i];
			if (WallTextureOrder[i] == _wallTexture)
				defaultIndex = i;
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
		gDropdownDefaultIndex = defaultIndex;
		break;
	}
}

static void window_mapgen_simplex_dropdown(rct_window *w, sint32 widgetIndex, sint32 dropdownIndex)
{
	sint32 type;

	switch (widgetIndex) {
	case WIDX_SIMPLEX_FLOOR_TEXTURE:
		if (dropdownIndex == -1)
			dropdownIndex = gDropdownHighlightedIndex;

		type = (dropdownIndex == -1) ?
		_floorTexture :
					  (uint32)gDropdownItemsArgs[dropdownIndex] - SPR_FLOOR_TEXTURE_GRASS;

		if (gLandToolTerrainSurface == type) {
			gLandToolTerrainSurface = 255;
		}
		else {
			gLandToolTerrainSurface = type;
			_floorTexture = type;
		}
		window_invalidate(w);
		break;
	case WIDX_SIMPLEX_WALL_TEXTURE:
		if (dropdownIndex == -1)
			dropdownIndex = gDropdownHighlightedIndex;

		type = (dropdownIndex == -1) ?
		_wallTexture :
					 (uint32)gDropdownItemsArgs[dropdownIndex] - SPR_WALL_TEXTURE_ROCK;

		if (gLandToolTerrainEdge == type) {
			gLandToolTerrainEdge = 255;
		}
		else {
			gLandToolTerrainEdge = type;
			_wallTexture = type;
		}
		window_invalidate(w);
		break;
	}
}

static void window_mapgen_simplex_update(rct_window *w)
{
	// Tab animation
	if (++w->frame_no >= TabAnimationLoops[w->page])
		w->frame_no = 0;
	widget_invalidate(w, WIDX_TAB_3);
}

static void window_mapgen_simplex_invalidate(rct_window *w)
{
	colour_scheme_update(w);

	if (w->widgets != PageWidgets[WINDOW_MAPGEN_PAGE_SIMPLEX]) {
		w->widgets = PageWidgets[WINDOW_MAPGEN_PAGE_SIMPLEX];
		window_init_scroll_widgets(w);
	}

	w->widgets[WIDX_SIMPLEX_FLOOR_TEXTURE].image = SPR_FLOOR_TEXTURE_GRASS + _floorTexture;
	w->widgets[WIDX_SIMPLEX_WALL_TEXTURE].image = SPR_WALL_TEXTURE_ROCK + _wallTexture;

	window_mapgen_set_pressed_tab(w);
	window_mapgen_anchor_border_widgets(w);
}

static void window_mapgen_simplex_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	uint16 arg;

	window_draw_widgets(w, dpi);
	window_mapgen_draw_tab_images(dpi, w);

	gfx_draw_string_left(dpi, STR_MAPGEN_SIMPLEX_NOISE_LOW_, NULL, COLOUR_BLACK, w->x + 5, w->y + w->widgets[WIDX_SIMPLEX_LOW].top + 1);
	gfx_draw_string_left(dpi, STR_MAPGEN_SIMPLEX_NOISE_HIGH, NULL, COLOUR_BLACK, w->x + 5, w->y + w->widgets[WIDX_SIMPLEX_HIGH].top + 1);
	gfx_draw_string_left(dpi, STR_MAPGEN_SIMPLEX_NOISE_BASE_FREQUENCY, NULL, COLOUR_BLACK, w->x + 5, w->y + w->widgets[WIDX_SIMPLEX_BASE_FREQ].top + 1);
	gfx_draw_string_left(dpi, STR_MAPGEN_SIMPLEX_NOISE_OCTAVES, NULL, COLOUR_BLACK, w->x + 5, w->y + w->widgets[WIDX_SIMPLEX_OCTAVES].top + 1);
	gfx_draw_string_left(dpi, STR_MAP_SIZE, NULL, COLOUR_BLACK, w->x + 5, w->y + w->widgets[WIDX_SIMPLEX_MAP_SIZE].top + 1);
	gfx_draw_string_left(dpi, STR_WATER_LEVEL_LABEL, NULL, COLOUR_BLACK, w->x + 5, w->y + w->widgets[WIDX_SIMPLEX_WATER_LEVEL].top + 1);

	gfx_draw_string_left(dpi, STR_COMMA16, &_simplex_low, w->colours[1], w->x + w->widgets[WIDX_SIMPLEX_LOW].left + 1, w->y + w->widgets[WIDX_SIMPLEX_LOW].top + 1);
	gfx_draw_string_left(dpi, STR_COMMA16, &_simplex_high, w->colours[1], w->x + w->widgets[WIDX_SIMPLEX_HIGH].left + 1, w->y + w->widgets[WIDX_SIMPLEX_HIGH].top + 1);
	gfx_draw_string_left(dpi, STR_WINDOW_OBJECTIVE_VALUE_RATING, &_simplex_base_freq, w->colours[1], w->x + w->widgets[WIDX_SIMPLEX_BASE_FREQ].left + 1, w->y + w->widgets[WIDX_SIMPLEX_BASE_FREQ].top + 1);
	gfx_draw_string_left(dpi, STR_COMMA16, &_simplex_octaves, w->colours[1], w->x + w->widgets[WIDX_SIMPLEX_OCTAVES].left + 1, w->y + w->widgets[WIDX_SIMPLEX_OCTAVES].top + 1);

	// The practical map size is 2 lower than the technical map size
	uint16 mapSizeArgs[2] = { _mapSize - 2, _mapSize - 2 };
	gfx_draw_string_left(dpi, STR_RESOLUTION_X_BY_Y, mapSizeArgs, w->colours[1], w->x + w->widgets[WIDX_SIMPLEX_MAP_SIZE].left + 1, w->y + w->widgets[WIDX_SIMPLEX_MAP_SIZE].top + 1);

	arg = (_waterLevel - 12) / 2;
	gfx_draw_string_left(dpi, STR_COMMA16, &arg, w->colours[1], w->x + w->widgets[WIDX_SIMPLEX_WATER_LEVEL].left + 1, w->y + w->widgets[WIDX_SIMPLEX_WATER_LEVEL].top + 1);
}

#pragma endregion

#pragma region Heightmap page

static void window_mapgen_heightmap_mouseup(rct_window *w, sint32 widgetIndex)
{
	mapgen_settings mapgenSettings;

	switch (widgetIndex)
	{
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
	case WIDX_TAB_3:
	case WIDX_TAB_4:
		window_mapgen_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_GENERATE:
		mapgenSettings.water_level = _waterLevel;
		mapgenSettings.smooth = widget_is_pressed(w, WIDX_HEIGHTMAP_SMOOTH);
		mapgenSettings.strong_smooth = widget_is_pressed(w, WIDX_HEIGHTMAP_STRONG);
		mapgen_generate_from_heightmap(&mapgenSettings);
		gfx_invalidate_screen();
		break;
	case WIDX_HEIGHTMAP_SMOOTH:
	case WIDX_HEIGHTMAP_STRONG:
		widget_set_checkbox_value(w, widgetIndex, !widget_is_pressed(w, widgetIndex));
		break;
	case WIDX_HEIGHTMAP_WATER_LEVEL_UP:
		_waterLevel = min(_waterLevel + 2, 54);
		window_invalidate(w);
		break;
	case WIDX_HEIGHTMAP_WATER_LEVEL_DOWN:
		_waterLevel = max(_waterLevel - 2, 0);
		window_invalidate(w);
		break;
	}
}

static void window_mapgen_heightmap_invalidate(rct_window *w)
{
	colour_scheme_update(w);

	if (w->widgets != PageWidgets[WINDOW_MAPGEN_PAGE_HEIGHTMAP])
	{
		w->widgets = PageWidgets[WINDOW_MAPGEN_PAGE_HEIGHTMAP];
		window_init_scroll_widgets(w);
	}

	window_mapgen_set_pressed_tab(w);
	window_mapgen_anchor_border_widgets(w);
}

static void window_mapgen_heightmap_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);
	window_mapgen_draw_tab_images(dpi, w);

	// water level label and value
	gfx_draw_string_left(dpi, STR_WATER_LEVEL_LABEL, NULL, COLOUR_BLACK, w->x + 5, w->y + w->widgets[WIDX_HEIGHTMAP_WATER_LEVEL].top + 1);
	gfx_draw_string_left(dpi, STR_COMMA16, &_waterLevel, w->colours[1], w->x + w->widgets[WIDX_HEIGHTMAP_WATER_LEVEL].left + 1, w->y + w->widgets[WIDX_HEIGHTMAP_WATER_LEVEL].top + 1);
}

#pragma endregion

#pragma region Common

static void window_mapgen_set_page(rct_window *w, sint32 page)
{
	w->page = page;
	w->frame_no = 0;
	if (w->viewport != NULL) {
		w->viewport->width = 0;
		w->viewport = NULL;
	}

	w->enabled_widgets = PageEnabledWidgets[page];
	w->hold_down_widgets = window_mapgen_page_hold_down_widgets[page];
	w->event_handlers = PageEvents[page];
	w->widgets = PageWidgets[page];
	w->disabled_widgets = 0;
	w->pressed_widgets = 0;

	window_init_scroll_widgets(w);
	window_invalidate(w);
}

static void window_mapgen_set_pressed_tab(rct_window *w)
{
	sint32 i;
	for (i = 0; i < WINDOW_MAPGEN_PAGE_COUNT; i++)
		w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
	w->pressed_widgets |= 1LL << (WIDX_TAB_1 + w->page);
}

static void window_mapgen_anchor_border_widgets(rct_window *w)
{
	sint32 width = w->widgets[WIDX_GENERATE].right - w->widgets[WIDX_GENERATE].left;
	sint32 height = w->widgets[WIDX_GENERATE].bottom - w->widgets[WIDX_GENERATE].top;

	w->widgets[WIDX_GENERATE].left = w->width - 3 - width;
	w->widgets[WIDX_GENERATE].right = w->width - 3;
	w->widgets[WIDX_GENERATE].bottom = w->height - 3;
	w->widgets[WIDX_GENERATE].top = w->height - 3 - height;

	w->widgets[WIDX_BACKGROUND].right = w->width - 1;
	w->widgets[WIDX_BACKGROUND].bottom = w->height - 1;
	w->widgets[WIDX_PAGE_BACKGROUND].right = w->width - 1;
	w->widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;
	w->widgets[WIDX_TITLE].right = w->width - 2;
	w->widgets[WIDX_CLOSE].left = w->width - 13;
	w->widgets[WIDX_CLOSE].right = w->width - 3;
}

static void window_mapgen_draw_tab_image(rct_drawpixelinfo *dpi, rct_window *w, sint32 page, sint32 spriteIndex)
{
	sint32 widgetIndex = WIDX_TAB_1 + page;

	if (!(w->disabled_widgets & (1LL << widgetIndex))) {
		if (w->page == page) {
			sint32 frame = w->frame_no / TabAnimationDivisor[w->page];
			spriteIndex += (frame % TabAnimationFrames[w->page]);
		}

		gfx_draw_sprite(dpi, spriteIndex, w->x + w->widgets[widgetIndex].left, w->y + w->widgets[widgetIndex].top, 0);
	}
}

static void window_mapgen_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w)
{
	window_mapgen_draw_tab_image(dpi, w, WINDOW_MAPGEN_PAGE_BASE, SPR_G2_TAB_LAND);
	window_mapgen_draw_tab_image(dpi, w, WINDOW_MAPGEN_PAGE_RANDOM, SPR_G2_TAB_TREE);
	window_mapgen_draw_tab_image(dpi, w, WINDOW_MAPGEN_PAGE_SIMPLEX, SPR_G2_TAB_PENCIL);
	window_mapgen_draw_tab_image(dpi, w, WINDOW_MAPGEN_PAGE_SIMPLEX, SPR_G2_TAB_NEWS);
}

#pragma endregion
