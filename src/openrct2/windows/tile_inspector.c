#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#include "../common.h"
#include "../core/Guard.hpp"
#include "../game.h"
#include "../input.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../ride/ride_data.h"
#include "../ride/track.h"
#include "../sprites.h"
#include "../world/footpath.h"
#include "../world/map.h"
#include "../world/scenery.h"
#include "../world/tile_inspector.h"
#include "dropdown.h"
#include "tile_inspector.h"

static const rct_string_id TerrainTypeStringIds[] = {
	STR_TILE_INSPECTOR_TERRAIN_GRASS,
	STR_TILE_INSPECTOR_TERRAIN_SAND,
	STR_TILE_INSPECTOR_TERRAIN_DIRT,
	STR_TILE_INSPECTOR_TERRAIN_ROCK,
	STR_TILE_INSPECTOR_TERRAIN_MARTIAN,
	STR_TILE_INSPECTOR_TERRAIN_CHECKERBOARD,
	STR_TILE_INSPECTOR_TERRAIN_GRASS_CLUMPS,
	STR_TILE_INSPECTOR_TERRAIN_ICE,
	STR_TILE_INSPECTOR_TERRAIN_GRID_RED,
	STR_TILE_INSPECTOR_TERRAIN_GRID_YELLOW,
	STR_TILE_INSPECTOR_TERRAIN_GRID_BLUE,
	STR_TILE_INSPECTOR_TERRAIN_GRID_GREEN,
	STR_TILE_INSPECTOR_TERRAIN_SAND_DARK,
	STR_TILE_INSPECTOR_TERRAIN_SAND_LIGHT,
	STR_TILE_INSPECTOR_TERRAIN_CHECKERBOARD_INVERTED,
	STR_TILE_INSPECTOR_TERRAIN_UNDERGROUND_VIEW,
};

static const rct_string_id TerrainEdgeTypeStringIds[] = {
	STR_TILE_INSPECTOR_TERRAIN_EDGE_ROCK,
	STR_TILE_INSPECTOR_TERRAIN_EDGE_WOOD_RED,
	STR_TILE_INSPECTOR_TERRAIN_EDGE_WOOD_BLACK,
	STR_TILE_INSPECTOR_TERRAIN_EDGE_ICE,

};

static const rct_string_id EntranceTypeStringIds[] = {
	STR_TILE_INSPECTOR_ENTRANCE_TYPE_RIDE_ENTRANCE,
	STR_TILE_INSPECTOR_ENTRANCE_TYPE_RIDE_EXIT,
	STR_TILE_INSPECTOR_ENTRANCE_TYPE_PARK_ENTRANC,
};

static const rct_string_id ParkEntrancePartStringIds[] = {
	STR_TILE_INSPECTOR_ENTRANCE_MIDDLE,
	STR_TILE_INSPECTOR_ENTRANCE_LEFT,
	STR_TILE_INSPECTOR_ENTRANCE_RIGHT
};

static const rct_string_id WallSlopeStringIds[] = {
	STR_TILE_INSPECTOR_WALL_FLAT,
	STR_TILE_INSPECTOR_WALL_SLOPED_LEFT,
	STR_TILE_INSPECTOR_WALL_SLOPED_RIGHT
};

enum WINDOW_TILE_INSPECTOR_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_LIST,
	WIDX_SPINNER_X,
	WIDX_SPINNER_X_INCREASE,
	WIDX_SPINNER_X_DECREASE,
	WIDX_SPINNER_Y,
	WIDX_SPINNER_Y_INCREASE,
	WIDX_SPINNER_Y_DECREASE,
	WIDX_BUTTON_CORRUPT,
	WIDX_BUTTON_REMOVE,
	WIDX_BUTTON_MOVE_DOWN,
	WIDX_BUTTON_MOVE_UP,
	WIDX_BUTTON_ROTATE,
	WIDX_BUTTON_SORT,
	WIDX_BUTTON_COPY,
	WIDX_BUTTON_PASTE,
	WIDX_COLUMN_TYPE,
	WIDX_COLUMN_BASEHEIGHT,
	WIDX_COLUMN_CLEARANCEHEIGHT,
	WIDX_COLUMN_GHOSTFLAG,
	WIDX_COLUMN_BROKENFLAG,
	WIDX_COLUMN_LASTFLAG,
	WIDX_GROUPBOX_DETAILS,
	WIDX_GROUPBOX_PROPERTIES,

	PAGE_WIDGETS,

	// Surface
	WIDX_SURFACE_SPINNER_HEIGHT = PAGE_WIDGETS,
	WIDX_SURFACE_SPINNER_HEIGHT_INCREASE,
	WIDX_SURFACE_SPINNER_HEIGHT_DECREASE,
	WIDX_SURFACE_BUTTON_REMOVE_FENCES,
	WIDX_SURFACE_BUTTON_RESTORE_FENCES,
	WIDX_SURFACE_CHECK_CORNER_N,
	WIDX_SURFACE_CHECK_CORNER_E,
	WIDX_SURFACE_CHECK_CORNER_S,
	WIDX_SURFACE_CHECK_CORNER_W,
	WIDX_SURFACE_CHECK_DIAGONAL,

	// Path
	WIDX_PATH_SPINNER_HEIGHT = PAGE_WIDGETS,
	WIDX_PATH_SPINNER_HEIGHT_INCREASE,
	WIDX_PATH_SPINNER_HEIGHT_DECREASE,
	WIDX_PATH_CHECK_SLOPED,
	WIDX_PATH_CHECK_EDGE_NE, // Note: This is NOT named after the world orientation, but after the way
	WIDX_PATH_CHECK_EDGE_E,  // it looks in the window (top corner is north). Their order is important,
	WIDX_PATH_CHECK_EDGE_SE, // as this is the same order paths use for their corners / edges.
	WIDX_PATH_CHECK_EDGE_S,  //	          N
	WIDX_PATH_CHECK_EDGE_SW, //	     NW-------NE
	WIDX_PATH_CHECK_EDGE_W,	 //	  W ------------- E
	WIDX_PATH_CHECK_EDGE_NW, //	     SW-------SE
	WIDX_PATH_CHECK_EDGE_N,	 //	          S

	// Track
	WIDX_TRACK_CHECK_APPLY_TO_ALL = PAGE_WIDGETS,
	WIDX_TRACK_SPINNER_HEIGHT,
	WIDX_TRACK_SPINNER_HEIGHT_INCREASE,
	WIDX_TRACK_SPINNER_HEIGHT_DECREASE,
	WIDX_TRACK_CHECK_CHAIN_LIFT,

	// Scenery
	WIDX_SCENERY_SPINNER_HEIGHT = PAGE_WIDGETS,
	WIDX_SCENERY_SPINNER_HEIGHT_INCREASE,
	WIDX_SCENERY_SPINNER_HEIGHT_DECREASE,
	WIDX_SCENERY_CHECK_QUARTER_N,
	WIDX_SCENERY_CHECK_QUARTER_E,
	WIDX_SCENERY_CHECK_QUARTER_S,
	WIDX_SCENERY_CHECK_QUARTER_W,
	WIDX_SCENERY_CHECK_COLLISION_N,
	WIDX_SCENERY_CHECK_COLLISION_E,
	WIDX_SCENERY_CHECK_COLLISION_S,
	WIDX_SCENERY_CHECK_COLLISION_W,

	// Entrance
	WIDX_ENTRANCE_SPINNER_HEIGHT = PAGE_WIDGETS,
	WIDX_ENTRANCE_SPINNER_HEIGHT_INCREASE,
	WIDX_ENTRANCE_SPINNER_HEIGHT_DECREASE,

	// Wall
	WIDX_WALL_SPINNER_HEIGHT = PAGE_WIDGETS,
	WIDX_WALL_SPINNER_HEIGHT_INCREASE,
	WIDX_WALL_SPINNER_HEIGHT_DECREASE,
	WIDX_WALL_DROPDOWN_SLOPE,
	WIDX_WALL_DROPDOWN_SLOPE_BUTTON,

	// Large
	WIDX_LARGE_SCENERY_SPINNER_HEIGHT = PAGE_WIDGETS,
	WIDX_LARGE_SCENERY_SPINNER_HEIGHT_INCREASE,
	WIDX_LARGE_SCENERY_SPINNER_HEIGHT_DECREASE,

	// Banner
	WIDX_BANNER_SPINNER_HEIGHT = PAGE_WIDGETS,
	WIDX_BANNER_SPINNER_HEIGHT_INCREASE,
	WIDX_BANNER_SPINNER_HEIGHT_DECREASE,
	WIDX_BANNER_CHECK_BLOCK_NE,
	WIDX_BANNER_CHECK_BLOCK_SE,
	WIDX_BANNER_CHECK_BLOCK_SW,
	WIDX_BANNER_CHECK_BLOCK_NW,

	// Corrupt
	WIDX_CORRUPT_SPINNER_HEIGHT = PAGE_WIDGETS,
	WIDX_CORRUPT_SPINNER_HEIGHT_INCREASE,
	WIDX_CORRUPT_SPINNER_HEIGHT_DECREASE,
	WIDX_CORRUPT_BUTTON_CLAMP,
};

// Window sizes
#define WW 400
#define WH 280
#define MIN_WW WW
#define MAX_WW WW
#define MIN_WH 240
#define MAX_WH 800

// Button space for top buttons
#define BW (WW - 5)		// Button's right side
#define BX (BW - 23)	// Button's left side
#define BY 17			// Button's Top
#define BH (BY + 23)	// Button's Bottom
#define BS 24

// Column offsets for the table headers
#define COL_X_TYPE 3					// Type
#define COL_X_BH   (COL_X_TYPE + 300)	// Base height
#define COL_X_CH   (COL_X_BH + 20)		// Clearance height
#define COL_X_GF   (COL_X_CH + 20)		// Ghost flag
#define COL_X_BF   (COL_X_GF + 12)		// Broken flag
#define COL_X_LF   (COL_X_BF + 12)		// Last for tile flag

#define PADDING_BOTTOM 15
#define GROUPBOX_PADDING 6
#define HORIZONTAL_GROUPBOX_PADDING 5
#define VERTICAL_GROUPBOX_PADDING 4
#define LIST_ITEM_HEIGHT 11
#define BUTTONW 130
#define BUTTONH 17

// Calculates the .left, .right, .top and .bottom for buttons in a group box.
// Buttons are used as reference points for all other widgets in the group boxes.
#define GBBL(col)				(12 + col * (BUTTONW + HORIZONTAL_GROUPBOX_PADDING))
#define GBBR(col)				(GBBL(col) + BUTTONW)
#define GBBT(GROUPTOP, row)		((GROUPTOP) + 14 + row * (BUTTONH + VERTICAL_GROUPBOX_PADDING))
#define GBBB(GROUPTOP, row)		(GBBT((GROUPTOP), row) + BUTTONH)
#define GBB(GROUPTOP, col, row)	GBBL(col), GBBR(col), GBBT((GROUPTOP), row), GBBB((GROUPTOP), row)
#define GBBF(GROUPTOP, col, row)GBBL(col), WW - 10, GBBT((GROUPTOP), row), GBBB((GROUPTOP), row) // Full width

// Spinners and dropdowns use the buttons as a reference
#define GBS(GBT, col, row)	GBBL(col), GBBR(col), GBBT(GBT, row) + 3, GBBB(GBT, row) - 3 // Group box spinner / dropdown field
#define GBSI(GBT, col, row)	GBBR(col) - 11, GBBR(col) - 1, GBBT(GBT, row) + 4, GBBT(GBT, row) + 8 // Group box spinner increase
#define GBSD(GBT, col, row)	GBBR(col) - 11, GBBR(col) - 1, GBBB(GBT, row) - 8, GBBB(GBT, row) - 4 // Group box spinner decrease
#define GBDB(GBT, col, row)	GBBR(col) - 11, GBBR(col) - 1, GBBT(GBT, row) + 4, GBBB(GBT, row) - 4 // Group box dropdown button

// Checkbox - given topleft corner
#define CHK(x, y) x, x + 13, y + 2, y + 15

#define MAIN_TILE_INSPECTOR_WIDGETS \
	{ WWT_FRAME,		0,	0,				WW - 1,				0,				WH - 1,		0xFFFFFFFF,					STR_NONE },					/* panel / background */	\
	{ WWT_CAPTION,		0,	1,				WW - 2,				1,				14,			STR_TILE_INSPECTOR_TITLE,	STR_WINDOW_TITLE_TIP },		/* title bar */				\
	{ WWT_CLOSEBOX,		0,	WW - 13,		WW - 3,				2,				13,			STR_CLOSE_X,				STR_CLOSE_WINDOW_TIP },		/* close x button */		\
	{ WWT_SCROLL,		1,	3,				WW - 4,				57,				WH - PADDING_BOTTOM,	2,				STR_NONE },					/* Element list */			\
	{ WWT_SPINNER,			1,	20,			61,					23,				34,			STR_NONE,					STR_NONE },					/* Spinner for X */			\
	{ WWT_DROPDOWN_BUTTON,	1,	51,			60,					24,				28,			STR_NUMERIC_UP,				STR_NONE },					/* increase X */			\
	{ WWT_DROPDOWN_BUTTON,	1,	51,			60,					29,				33,			STR_NUMERIC_DOWN,			STR_NONE },					/* decrease X */			\
	{ WWT_SPINNER,			1,	77,			118,				23,				34,			STR_NONE,					STR_NONE },					/* Spinner for Y */			\
	{ WWT_DROPDOWN_BUTTON,	1,	108,		117,				24,				28,			STR_NUMERIC_UP,				STR_NONE },					/* increase Y */			\
	{ WWT_DROPDOWN_BUTTON,	1,	108,		117,				29,				33,			STR_NUMERIC_DOWN,			STR_NONE },					/* decrease Y */			\
	/* Buttons */																																								\
	{ WWT_FLATBTN,		1,	BX,				BW,					BY,				BH,			SPR_MAP,		  STR_INSERT_CORRUPT_TIP },				/* Insert corrupt button */	\
	{ WWT_FLATBTN,		1,  BX - BS * 1,	BW - BS * 1,		BY,				BH,			SPR_DEMOLISH,	  STR_REMOVE_SELECTED_ELEMENT_TIP },	/* Remove button */			\
	{ WWT_CLOSEBOX, 	1,	BX - BS * 2,	BW - BS * 2,		BY,				BY + 11, 	STR_UP,			  STR_MOVE_SELECTED_ELEMENT_UP_TIP },	/* Move down */				\
	{ WWT_CLOSEBOX, 	1,	BX - BS * 2, 	BW - BS * 2,		BH - 11,		BH, 		STR_DOWN,		  STR_MOVE_SELECTED_ELEMENT_DOWN_TIP },	/* Move up */				\
	{ WWT_FLATBTN,		1,  BX - BS * 3,	BW - BS * 3,		BY,				BH,			SPR_ROTATE_ARROW, STR_ROTATE_SELECTED_ELEMENT_TIP },	/* Rotate button */			\
	{ WWT_FLATBTN,		1,  BX - BS * 4,	BW - BS * 4,		BY,				BH,			SPR_G2_SORT,      STR_TILE_INSPECTOR_SORT_TIP },	    /* Sort button */			\
	{ WWT_FLATBTN,		1,  BX - BS * 5,	BW - BS * 5,		BY,				BH,			SPR_G2_COPY,      STR_TILE_INSPECTOR_COPY_TIP },	    /* Copy button */			\
	{ WWT_FLATBTN,		1,  BX - BS * 6,	BW - BS * 6,		BY,				BH,			SPR_G2_PASTE,     STR_TILE_INSPECTOR_PASTE_TIP },	    /* Paste button */			\
	/* Column headers */																																						\
	{ WWT_13,			1, COL_X_TYPE,	COL_X_BH - 1, 	42,		42 + 13,	STR_NONE,	STR_NONE },													/* Type */					\
	{ WWT_13,			1, COL_X_BH,	COL_X_CH - 1, 	42,		42 + 13,	STR_NONE,	STR_TILE_INSPECTOR_BASE_HEIGHT },							/* Base height */			\
	{ WWT_13,			1, COL_X_CH,	COL_X_GF - 1, 	42,		42 + 13,	STR_NONE,	STR_TILE_INSPECTOR_CLEARANCE_HEIGHT },						/* Clearance height */		\
	{ WWT_13,			1, COL_X_GF,	COL_X_BF - 1, 	42,		42 + 13,	STR_NONE,	STR_TILE_INSPECTOR_FLAG_GHOST },							/* Ghost flag */			\
	{ WWT_13,			1, COL_X_BF,	COL_X_LF - 1, 	42,		42 + 13,	STR_NONE,	STR_TILE_INSPECTOR_FLAG_BROKEN },							/* Broken flag */			\
	{ WWT_13,			1, COL_X_LF,	WW - 3,			42,		42 + 13,	STR_NONE,	STR_TILE_INSPECTOR_FLAG_LAST },								/* Last of tile flag */		\
	{ WWT_GROUPBOX,		1, 6,			WW - 6,			-1,		-1,			STR_NONE,									STR_NONE },					/* Details group box */		\
	{ WWT_GROUPBOX,		1, 6,			WW - 6,			-1,		-1,			STR_TILE_INSPECTOR_GROUPBOX_PROPERTIES,		STR_NONE }					/* Properties group box */

static rct_widget DefaultWidgets[] = {
	MAIN_TILE_INSPECTOR_WIDGETS,
	{ WIDGETS_END },
};

// Group boxes .top and .bottom for a given window height offsets from the bottom
#define SUR_GBPB PADDING_BOTTOM					// Surface group box properties bottom
#define SUR_GBPT (SUR_GBPB + 16 + 4 * 21)		// Surface group box properties top
#define SUR_GBDB (SUR_GBPT + GROUPBOX_PADDING)	// Surface group box details bottom
#define SUR_GBDT (SUR_GBDB + 20 + 4 * 11)		// Surface group box details top
static rct_widget SurfaceWidgets[] = {
	MAIN_TILE_INSPECTOR_WIDGETS,
	{ WWT_SPINNER,			1,	GBS(WH - SUR_GBPT, 1, 0),				STR_NONE,					STR_NONE }, // WIDX_SURFACE_SPINNER_HEIGHT
	{ WWT_DROPDOWN_BUTTON,  1,	GBSI(WH - SUR_GBPT, 1, 0),				STR_NUMERIC_UP,				STR_NONE }, // WIDX_SURFACE_SPINNER_HEIGHT_INCREASE
	{ WWT_DROPDOWN_BUTTON,  1,	GBSD(WH - SUR_GBPT, 1, 0),				STR_NUMERIC_DOWN,			STR_NONE }, // WIDX_SURFACE_SPINNER_HEIGHT_DECREASE
	{ WWT_CLOSEBOX,		1,	GBB(WH - SUR_GBPT, 0, 1),	STR_TILE_INSPECTOR_SURFACE_REMOVE_FENCES,	STR_NONE }, // WIDX_SURFACE_BUTTON_REMOVE_FENCES
	{ WWT_CLOSEBOX,		1,	GBB(WH - SUR_GBPT, 1, 1),	STR_TILE_INSPECTOR_SURFACE_RESTORE_FENCES,	STR_NONE }, // WIDX_SURFACE_BUTTON_RESTORE_FENCES
	{ WWT_CHECKBOX,			1,	CHK(GBBL(1) + 14 * 1, GBBT(WH - SUR_GBPT, 2) + 7 * 0),	STR_NONE,	STR_NONE }, // WIDX_SURFACE_CHECK_CORNER_N
	{ WWT_CHECKBOX,			1,	CHK(GBBL(1) + 14 * 2, GBBT(WH - SUR_GBPT, 2) + 7 * 1),	STR_NONE,	STR_NONE }, // WIDX_SURFACE_CHECK_CORNER_E
	{ WWT_CHECKBOX,			1,	CHK(GBBL(1) + 14 * 1, GBBT(WH - SUR_GBPT, 2) + 7 * 2),	STR_NONE,	STR_NONE }, // WIDX_SURFACE_CHECK_CORNER_S
	{ WWT_CHECKBOX,			1,	CHK(GBBL(1) + 14 * 0, GBBT(WH - SUR_GBPT, 2) + 7 * 1),	STR_NONE,	STR_NONE }, // WIDX_SURFACE_CHECK_CORNER_W
	{ WWT_CHECKBOX,			1,	GBBF(WH - SUR_GBPT, 0, 4),	STR_TILE_INSPECTOR_SURFACE_DIAGONAL,	STR_NONE }, // WIDX_SURFACE_CHECK_DIAGONAL
	{ WIDGETS_END },
};

#define PAT_GBPB PADDING_BOTTOM					// Path group box properties bottom
#define PAT_GBPT (PAT_GBPB + 16 + 4 * 21)		// Path group box properties top
#define PAT_GBDB (PAT_GBPT + GROUPBOX_PADDING)	// Path group box info bottom
#define PAT_GBDT (PAT_GBDB + 20 + 2 * 11)		// Path group box info top
static rct_widget PathWidgets[] = {
	MAIN_TILE_INSPECTOR_WIDGETS,
	{ WWT_SPINNER,			1,	GBS(WH - PAT_GBPT, 1, 0),				STR_NONE,					STR_NONE }, // WIDX_PATH_SPINNER_HEIGHT
	{ WWT_DROPDOWN_BUTTON,  1,	GBSI(WH - PAT_GBPT, 1, 0),				STR_NUMERIC_UP,				STR_NONE }, // WIDX_PATH_SPINNER_HEIGHT_INCREASE
	{ WWT_DROPDOWN_BUTTON,  1,	GBSD(WH - PAT_GBPT, 1, 0),				STR_NUMERIC_DOWN,			STR_NONE }, // WIDX_PATH_SPINNER_HEIGHT_DECREASE
	{ WWT_CHECKBOX,			1,	GBBF(WH - PAT_GBPT, 0, 1),				STR_TILE_INSPECTOR_PATH_SLOPED, STR_NONE }, // WIDX_PATH_CHECK_SLOPED
	{ WWT_CHECKBOX,			1,	CHK(GBBL(1) + 14 * 3, GBBT(WH - PAT_GBPT, 2) + 7 * 1),	STR_NONE,	STR_NONE }, // WIDX_PATH_CHECK_EDGE_NE
	{ WWT_CHECKBOX,			1,	CHK(GBBL(1) + 14 * 4, GBBT(WH - PAT_GBPT, 2) + 7 * 2),	STR_NONE,	STR_NONE }, // WIDX_PATH_CHECK_EDGE_E
	{ WWT_CHECKBOX,			1,	CHK(GBBL(1) + 14 * 3, GBBT(WH - PAT_GBPT, 2) + 7 * 3),	STR_NONE,	STR_NONE }, // WIDX_PATH_CHECK_EDGE_SE
	{ WWT_CHECKBOX,			1,	CHK(GBBL(1) + 14 * 2, GBBT(WH - PAT_GBPT, 2) + 7 * 4),	STR_NONE,	STR_NONE }, // WIDX_PATH_CHECK_EDGE_S
	{ WWT_CHECKBOX,			1,	CHK(GBBL(1) + 14 * 1, GBBT(WH - PAT_GBPT, 2) + 7 * 3),	STR_NONE,	STR_NONE }, // WIDX_PATH_CHECK_EDGE_SW
	{ WWT_CHECKBOX,			1,	CHK(GBBL(1) + 14 * 0, GBBT(WH - PAT_GBPT, 2) + 7 * 2),	STR_NONE,	STR_NONE }, // WIDX_PATH_CHECK_EDGE_W
	{ WWT_CHECKBOX,			1,	CHK(GBBL(1) + 14 * 1, GBBT(WH - PAT_GBPT, 2) + 7 * 1),	STR_NONE,	STR_NONE }, // WIDX_PATH_CHECK_EDGE_NW
	{ WWT_CHECKBOX,			1,	CHK(GBBL(1) + 14 * 2, GBBT(WH - PAT_GBPT, 2) + 7 * 0),	STR_NONE,	STR_NONE }, // WIDX_PATH_CHECK_EDGE_N
	{ WIDGETS_END },
};

#define TRA_GBPB PADDING_BOTTOM					// Track group box properties bottom
#define TRA_GBPT (TRA_GBPB + 16 + 3 * 21)		// Track group box properties top
#define TRA_GBDB (TRA_GBPT + GROUPBOX_PADDING)	// Track group box info bottom
#define TRA_GBDT (TRA_GBDB + 20 + 5 * 11)		// Track group box info top
static rct_widget TrackWidgets[] = {
	MAIN_TILE_INSPECTOR_WIDGETS,
	{ WWT_CHECKBOX,			1,	GBBF(WH - TRA_GBPT, 0, 0),	STR_TILE_INSPECTOR_TRACK_ENTIRE_TRACK_PIECE,	STR_NONE }, // WIDX_TRACK_CHECK_APPLY_TO_ALL
	{ WWT_SPINNER,			1,	GBS(WH - TRA_GBPT, 1, 1),	STR_NONE,										STR_NONE }, // WIDX_TRACK_SPINNER_HEIGHT
	{ WWT_DROPDOWN_BUTTON,  1,	GBSI(WH - TRA_GBPT, 1, 1),	STR_NUMERIC_UP,									STR_NONE }, // WIDX_TRACK_SPINNER_HEIGHT_INCREASE
	{ WWT_DROPDOWN_BUTTON,  1,	GBSD(WH - TRA_GBPT, 1, 1),	STR_NUMERIC_DOWN,								STR_NONE }, // WIDX_TRACK_SPINNER_HEIGHT_DECREASE
	{ WWT_CHECKBOX,			1,	GBBF(WH - TRA_GBPT, 0, 2),	STR_TILE_INSPECTOR_TRACK_CHAIN_LIFT,			STR_NONE }, // WIDX_TRACK_CHECK_CHAIN_LIFT
	{ WIDGETS_END },
};

#define SCE_GBPB PADDING_BOTTOM					// Scenery group box properties bottom
#define SCE_GBPT (SCE_GBPB + 16 + 4 * 21)		// Scenery group box properties top
#define SCE_GBDB (SCE_GBPT + GROUPBOX_PADDING)	// Scenery group box info bottom
#define SCE_GBDT (SCE_GBDB + 20 + 3 * 11)		// Scenery group box info top
static rct_widget SceneryWidgets[] = {
	MAIN_TILE_INSPECTOR_WIDGETS,
	{ WWT_SPINNER,			1,	GBS(WH - SCE_GBPT, 1, 0),				STR_NONE,					STR_NONE }, // WIDX_SCENERY_SPINNER_HEIGHT
	{ WWT_DROPDOWN_BUTTON,  1,	GBSI(WH - SCE_GBPT, 1, 0),				STR_NUMERIC_UP,				STR_NONE }, // WIDX_SCENERY_SPINNER_HEIGHT_INCREASE
	{ WWT_DROPDOWN_BUTTON,  1,	GBSD(WH - SCE_GBPT, 1, 0),				STR_NUMERIC_DOWN,			STR_NONE }, // WIDX_SCENERY_SPINNER_HEIGHT_DECREASE
	{ WWT_CHECKBOX,			1,	CHK(GBBL(1) + 14 * 1, GBBT(WH - SCE_GBPT, 1) + 7 * 0),	STR_NONE,	STR_NONE }, // WIDX_SCENERY_CHECK_QUARTER_N
	{ WWT_CHECKBOX,			1,	CHK(GBBL(1) + 14 * 2, GBBT(WH - SCE_GBPT, 1) + 7 * 1),	STR_NONE,	STR_NONE }, // WIDX_SCENERY_CHECK_QUARTER_E
	{ WWT_CHECKBOX,			1,	CHK(GBBL(1) + 14 * 1, GBBT(WH - SCE_GBPT, 1) + 7 * 2),	STR_NONE,	STR_NONE }, // WIDX_SCENERY_CHECK_QUARTER_S
	{ WWT_CHECKBOX,			1,	CHK(GBBL(1) + 14 * 0, GBBT(WH - SCE_GBPT, 1) + 7 * 1),	STR_NONE,	STR_NONE }, // WIDX_SCENERY_CHECK_QUARTER_W
	{ WWT_CHECKBOX,			1,	CHK(GBBL(1) + 14 * 1, GBBT(WH - SCE_GBPT, 2) + 7 * 0),	STR_NONE,	STR_NONE }, // WIDX_SCENERY_CHECK_COLLISION_N
	{ WWT_CHECKBOX,			1,	CHK(GBBL(1) + 14 * 2, GBBT(WH - SCE_GBPT, 2) + 7 * 1),	STR_NONE,	STR_NONE }, // WIDX_SCENERY_CHECK_COLLISION_E
	{ WWT_CHECKBOX,			1,	CHK(GBBL(1) + 14 * 1, GBBT(WH - SCE_GBPT, 2) + 7 * 2),	STR_NONE,	STR_NONE }, // WIDX_SCENERY_CHECK_COLLISION_S
	{ WWT_CHECKBOX,			1,	CHK(GBBL(1) + 14 * 0, GBBT(WH - SCE_GBPT, 2) + 7 * 1),	STR_NONE,	STR_NONE }, // WIDX_SCENERY_CHECK_COLLISION_W
	{ WIDGETS_END },
};

#define ENT_GBPB PADDING_BOTTOM					// Entrance group box properties bottom
#define ENT_GBPT (ENT_GBPB + 16 + 1 * 21)		// Entrance group box properties top
#define ENT_GBDB (ENT_GBPT + GROUPBOX_PADDING)	// Entrance group box info bottom
#define ENT_GBDT (ENT_GBDB + 20 + 3 * 11)		// Entrance group box info top
static rct_widget EntranceWidgets[] = {
	MAIN_TILE_INSPECTOR_WIDGETS,
	{ WWT_SPINNER,			1,	GBS(WH - ENT_GBPT, 1, 0),	STR_NONE,										STR_NONE }, // WIDX_ENTRANCE_SPINNER_HEIGHT
	{ WWT_DROPDOWN_BUTTON,  1,	GBSI(WH - ENT_GBPT, 1, 0),	STR_NUMERIC_UP,									STR_NONE }, // WIDX_ENTRANCE_SPINNER_HEIGHT_INCREASE
	{ WWT_DROPDOWN_BUTTON,  1,	GBSD(WH - ENT_GBPT, 1, 0),	STR_NUMERIC_DOWN,								STR_NONE }, // WIDX_ENTRANCE_SPINNER_HEIGHT_DECREASE
	{ WIDGETS_END },
};

#define WALL_GBPB PADDING_BOTTOM					// Wall group box properties bottom
#define WALL_GBPT (WALL_GBPB + 16 + 2 * 21)		// Wall group box properties top
#define WALL_GBDB (WALL_GBPT + GROUPBOX_PADDING)	// Wall group box info bottom
#define WALL_GBDT (WALL_GBDB + 20 + 2 * 11)		// Wall group box info top
static rct_widget WallWidgets[] = {
	MAIN_TILE_INSPECTOR_WIDGETS,
	{ WWT_SPINNER,			1,	GBS(WH - WALL_GBPT, 1, 0),	STR_NONE,										STR_NONE }, // WIDX_WALL_SPINNER_HEIGHT
	{ WWT_DROPDOWN_BUTTON,	1,	GBSI(WH - WALL_GBPT, 1, 0),	STR_NUMERIC_UP,									STR_NONE }, // WIDX_WALL_SPINNER_HEIGHT_INCREASE
	{ WWT_DROPDOWN_BUTTON,	1,	GBSD(WH - WALL_GBPT, 1, 0),	STR_NUMERIC_DOWN,								STR_NONE }, // WIDX_WALL_SPINNER_HEIGHT_DECREASE
	{ WWT_DROPDOWN,			1,  GBS(WH - WALL_GBPT, 1, 1),	STR_NONE,										STR_NONE }, // WIDX_WALL_DROPDOWN_SLOPE
	{ WWT_DROPDOWN_BUTTON,	1,	GBDB(WH - WALL_GBPT, 1, 1),	STR_DROPDOWN_GLYPH,								STR_NONE }, // WIDX_WALL_DROPDOWN_SLOPE_BUTTON
	{ WIDGETS_END },
};

#define LAR_GBPB PADDING_BOTTOM					// Large scenery group box properties bottom
#define LAR_GBPT (LAR_GBPB + 16 + 1 * 21)		// Large scenery group box properties top
#define LAR_GBDB (LAR_GBPT + GROUPBOX_PADDING)	// Large scenery group box info bottom
#define LAR_GBDT (LAR_GBDB + 20 + 3 * 11)		// Large scenery group box info top
static rct_widget LargeSceneryWidgets[] = {
	MAIN_TILE_INSPECTOR_WIDGETS,
	{ WWT_SPINNER,			1,	GBS(WH - LAR_GBPT, 1, 0),	STR_NONE,										STR_NONE }, // WIDX_LARGE_SCENERY_SPINNER_HEIGHT
	{ WWT_DROPDOWN_BUTTON,  1,	GBSI(WH - LAR_GBPT, 1, 0),	STR_NUMERIC_UP,									STR_NONE }, // WIDX_LARGE_SCENERY_SPINNER_HEIGHT_INCREASE
	{ WWT_DROPDOWN_BUTTON,  1,	GBSD(WH - LAR_GBPT, 1, 0),	STR_NUMERIC_DOWN,								STR_NONE }, // WIDX_LARGE_SCENERY_SPINNER_HEIGHT_DECREASE
	{ WIDGETS_END },
};

#define BAN_GBPB PADDING_BOTTOM					// Banner group box properties bottom
#define BAN_GBPT (BAN_GBPB + 16 + 3 * 21)		// Banner group box properties top
#define BAN_GBDB (BAN_GBPT + GROUPBOX_PADDING)	// Banner group box info bottom
#define BAN_GBDT (BAN_GBDB + 20 + 1 * 11)		// Banner group box info top
static rct_widget BannerWidgets[] = {
	MAIN_TILE_INSPECTOR_WIDGETS,
	{ WWT_SPINNER,			1,	GBS(WH - BAN_GBPT, 1, 0),	STR_NONE,										STR_NONE }, // WIDX_BANNER_SPINNER_HEIGHT
	{ WWT_DROPDOWN_BUTTON,  1,	GBSI(WH - BAN_GBPT, 1, 0),	STR_NUMERIC_UP,									STR_NONE }, // WIDX_BANNER_SPINNER_HEIGHT_INCREASE
	{ WWT_DROPDOWN_BUTTON,  1,	GBSD(WH - BAN_GBPT, 1, 0),	STR_NUMERIC_DOWN,								STR_NONE }, // WIDX_BANNER_SPINNER_HEIGHT_DECREASE
	{ WWT_CHECKBOX,			1,	CHK(GBBL(1) + 14 * 3,	GBBT(WH - BAN_GBPT, 1) + 7 * 1),	STR_NONE,		STR_NONE }, // WIDX_BANNER_CHECK_BLOCK_NE
	{ WWT_CHECKBOX,			1,	CHK(GBBL(1) + 14 * 3,	GBBT(WH - BAN_GBPT, 1) + 7 * 3),	STR_NONE,		STR_NONE }, // WIDX_BANNER_CHECK_BLOCK_SE
	{ WWT_CHECKBOX,			1,	CHK(GBBL(1) + 14 * 1,	GBBT(WH - BAN_GBPT, 1) + 7 * 3),	STR_NONE,		STR_NONE }, // WIDX_BANNER_CHECK_BLOCK_SW
	{ WWT_CHECKBOX,			1,	CHK(GBBL(1) + 14 * 1,	GBBT(WH - BAN_GBPT, 1) + 7 * 1),	STR_NONE,		STR_NONE }, // WIDX_BANNER_CHECK_BLOCK_NW

	{ WIDGETS_END },
};

#define COR_GBPB PADDING_BOTTOM					// Corrupt element group box properties bottom
#define COR_GBPT (COR_GBPB + 16 + 2 * 21)		// Corrupt element group box properties top
#define COR_GBDB (COR_GBPT + GROUPBOX_PADDING)	// Corrupt element group box info bottom
#define COR_GBDT (COR_GBDB + 20 + 0 * 11)		// Corrupt element group box info top
static rct_widget CorruptWidgets[] = {
	MAIN_TILE_INSPECTOR_WIDGETS,
	{ WWT_SPINNER,			1,	GBS(WH - COR_GBPT, 1, 0),	STR_NONE,										STR_NONE }, // WIDX_CORRUPT_SPINNER_HEIGHT
	{ WWT_DROPDOWN_BUTTON,  1,	GBSI(WH - COR_GBPT, 1, 0),	STR_NUMERIC_UP,									STR_NONE }, // WIDX_CORRUPT_SPINNER_HEIGHT_INCREASE
	{ WWT_DROPDOWN_BUTTON,  1,	GBSD(WH - COR_GBPT, 1, 0),	STR_NUMERIC_DOWN,								STR_NONE }, // WIDX_CORRUPT_SPINNER_HEIGHT_DECREASE
	{ WWT_CLOSEBOX,			1,	GBB(WH - SUR_GBPT, 0, 1),	STR_TILE_INSPECTOR_CLAMP_TO_NEXT, STR_TILE_INSPECTOR_CLAMP_TO_NEXT_TIP }, // WIDX_CORRUPT_BUTTON_CLAMP
	{ WIDGETS_END },
};

static rct_widget *PageWidgets[] = {
	DefaultWidgets,
	SurfaceWidgets,
	PathWidgets,
	TrackWidgets,
	SceneryWidgets,
	EntranceWidgets,
	WallWidgets,
	LargeSceneryWidgets,
	BannerWidgets,
	CorruptWidgets,
};

static struct {
	// Offsets from the bottom of the window
	sint16 details_top_offset, details_bottom_offset;
	sint16 properties_top_offset, properties_bottom_offset;
	// String to be displayed in the details groupbox
	rct_string_id string_id;
} PageGroupBoxSettings[] = {
	{ SUR_GBDT, SUR_GBDB, SUR_GBPT, SUR_GBPB, STR_TILE_INSPECTOR_GROUPBOX_SURFACE_INFO },
	{ PAT_GBDT, PAT_GBDB, PAT_GBPT, PAT_GBPB, STR_TILE_INSPECTOR_GROUPBOX_PATH_INFO },
	{ TRA_GBDT, TRA_GBDB, TRA_GBPT, TRA_GBPB, STR_TILE_INSPECTOR_GROUPBOX_TRACK_INFO },
	{ SCE_GBDT, SCE_GBDB, SCE_GBPT, SCE_GBPB, STR_TILE_INSPECTOR_GROUPBOX_SCENERY_INFO },
	{ ENT_GBDT, ENT_GBDB, ENT_GBPT, ENT_GBPB, STR_TILE_INSPECTOR_GROUPBOX_ENTRANCE_INFO },
	{ WALL_GBDT, WALL_GBDB, WALL_GBPT, WALL_GBPB, STR_TILE_INSPECTOR_GROUPBOX_WALL_INFO },
	{ LAR_GBDT, LAR_GBDB, LAR_GBPT, LAR_GBPB, STR_TILE_INSPECTOR_GROUPBOX_LARGE_SCENERY_INFO },
	{ BAN_GBDT, BAN_GBDB, BAN_GBPT, BAN_GBPB, STR_TILE_INSPECTOR_GROUPBOX_BANNER_INFO },
	{ COR_GBDT, COR_GBDB, COR_GBPT, COR_GBPB, STR_TILE_INSPECTOR_GROUPBOX_CORRUPT_INFO }
};

uint32 windowTileInspectorTileX;
uint32 windowTileInspectorTileY;
sint32 windowTileInspectorElementCount = 0;
static sint16 windowTileInspectorHighlightedIndex = -1;
static bool windowTileInspectorTileSelected = false;
static sint32 windowTileInspectorToolMouseX = 0;
static sint32 windowTileInspectorToolMouseY = 0;
static sint32 windowTileInspectorToolMapX = 0;
static sint32 windowTileInspectorToolMapY = 0;
static bool windowTileInspectorApplyToAll = false;
static bool windowTileInspectorElementCopied = false;
static rct_map_element tileInspectorCopiedElement;

static void window_tile_inspector_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_tile_inspector_resize(rct_window *w);
static void window_tile_inspector_mousedown(rct_widgetindex widgetIndex, rct_window *w, rct_widget* widget);
static void window_tile_inspector_update(rct_window *w);
static void window_tile_inspector_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex);
static void window_tile_inspector_tool_update(rct_window* w, rct_widgetindex widgetIndex, sint32 x, sint32 y);
static void window_tile_inspector_update_selected_tile(rct_window *w, sint32 x, sint32 y);
static void window_tile_inspector_tool_down(rct_window* w, rct_widgetindex widgetIndex, sint32 x, sint32 y);
static void window_tile_inspector_tool_drag(rct_window* w, rct_widgetindex widgetIndex, sint32 x, sint32 y);
static void window_tile_inspector_scrollgetsize(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height);
static void window_tile_inspector_scrollmousedown(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y);
static void window_tile_inspector_scrollmouseover(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y);
static void window_tile_inspector_invalidate(rct_window *w);
static void window_tile_inspector_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_tile_inspector_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex);

static rct_window_event_list TileInspectorWindowEvents = {
	NULL,
	window_tile_inspector_mouseup,
	window_tile_inspector_resize,
	window_tile_inspector_mousedown,
	window_tile_inspector_dropdown,
	NULL,
	window_tile_inspector_update,
	NULL,
	NULL,
	window_tile_inspector_tool_update,
	window_tile_inspector_tool_down,
	window_tile_inspector_tool_drag,
	NULL,
	NULL,
	NULL,
	window_tile_inspector_scrollgetsize,
	window_tile_inspector_scrollmousedown,
	NULL,
	window_tile_inspector_scrollmouseover,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_tile_inspector_invalidate,
	window_tile_inspector_paint,
	window_tile_inspector_scrollpaint
};

static uint64 PageEnabledWidgets[] = {
	(1ULL << WIDX_CLOSE) | (1ULL << WIDX_BUTTON_CORRUPT),
	(1ULL << WIDX_CLOSE) | (1ULL << WIDX_BUTTON_CORRUPT) | (1ULL << WIDX_BUTTON_REMOVE) | (1ULL << WIDX_BUTTON_ROTATE) | (1ULL << WIDX_BUTTON_COPY) | (1ULL << WIDX_SURFACE_SPINNER_HEIGHT_INCREASE) | (1ULL << WIDX_SURFACE_SPINNER_HEIGHT_DECREASE) | (1ULL << WIDX_SURFACE_BUTTON_REMOVE_FENCES) | (1ULL << WIDX_SURFACE_BUTTON_RESTORE_FENCES) | (1ULL << WIDX_SURFACE_CHECK_CORNER_N) | (1ULL << WIDX_SURFACE_CHECK_CORNER_E) | (1ULL << WIDX_SURFACE_CHECK_CORNER_S) | (1ULL << WIDX_SURFACE_CHECK_CORNER_W) | (1ULL << WIDX_SURFACE_CHECK_DIAGONAL),
	(1ULL << WIDX_CLOSE) | (1ULL << WIDX_BUTTON_CORRUPT) | (1ULL << WIDX_BUTTON_REMOVE) | (1ULL << WIDX_BUTTON_ROTATE) | (1ULL << WIDX_BUTTON_COPY) | (1ULL << WIDX_PATH_SPINNER_HEIGHT_INCREASE) | (1ULL << WIDX_PATH_SPINNER_HEIGHT_DECREASE) | (1ULL << WIDX_PATH_CHECK_SLOPED) | (1ULL << WIDX_PATH_CHECK_EDGE_N) | (1ULL << WIDX_PATH_CHECK_EDGE_NE) | (1ULL << WIDX_PATH_CHECK_EDGE_E) | (1ULL << WIDX_PATH_CHECK_EDGE_SE) | (1ULL << WIDX_PATH_CHECK_EDGE_S) | (1ULL << WIDX_PATH_CHECK_EDGE_SW) | (1ULL << WIDX_PATH_CHECK_EDGE_W) | (1ULL << WIDX_PATH_CHECK_EDGE_NW),
	(1ULL << WIDX_CLOSE) | (1ULL << WIDX_BUTTON_CORRUPT) | (1ULL << WIDX_BUTTON_REMOVE) | (1ULL << WIDX_BUTTON_ROTATE) | (1ULL << WIDX_BUTTON_COPY) | (1ULL << WIDX_TRACK_CHECK_APPLY_TO_ALL) | (1ULL << WIDX_TRACK_SPINNER_HEIGHT_INCREASE) | (1ULL << WIDX_TRACK_SPINNER_HEIGHT_DECREASE) | (1ULL << WIDX_TRACK_CHECK_CHAIN_LIFT),
	(1ULL << WIDX_CLOSE) | (1ULL << WIDX_BUTTON_CORRUPT) | (1ULL << WIDX_BUTTON_REMOVE) | (1ULL << WIDX_BUTTON_ROTATE) | (1ULL << WIDX_BUTTON_COPY) | (1ULL << WIDX_SCENERY_SPINNER_HEIGHT_INCREASE) | (1ULL << WIDX_SCENERY_SPINNER_HEIGHT_DECREASE) | (1ULL << WIDX_SCENERY_CHECK_QUARTER_N) | (1ULL << WIDX_SCENERY_CHECK_QUARTER_E) | (1ULL << WIDX_SCENERY_CHECK_QUARTER_S) | (1ULL << WIDX_SCENERY_CHECK_QUARTER_W) | (1ULL << WIDX_SCENERY_CHECK_COLLISION_N) | (1ULL << WIDX_SCENERY_CHECK_COLLISION_E) | (1ULL << WIDX_SCENERY_CHECK_COLLISION_S) | (1ULL << WIDX_SCENERY_CHECK_COLLISION_W),
	(1ULL << WIDX_CLOSE) | (1ULL << WIDX_BUTTON_CORRUPT) | (1ULL << WIDX_BUTTON_REMOVE) | (1ULL << WIDX_BUTTON_ROTATE) | (1ULL << WIDX_BUTTON_COPY) | (1ULL << WIDX_ENTRANCE_SPINNER_HEIGHT_INCREASE) | (1ULL << WIDX_ENTRANCE_SPINNER_HEIGHT_DECREASE),
	(1ULL << WIDX_CLOSE) | (1ULL << WIDX_BUTTON_CORRUPT) | (1ULL << WIDX_BUTTON_REMOVE) | (1ULL << WIDX_BUTTON_ROTATE) | (1ULL << WIDX_BUTTON_COPY) | (1ULL << WIDX_WALL_SPINNER_HEIGHT_INCREASE) | (1ULL << WIDX_WALL_SPINNER_HEIGHT_DECREASE) | (1ULL << WIDX_WALL_DROPDOWN_SLOPE) | (1ULL << WIDX_WALL_DROPDOWN_SLOPE_BUTTON),
	(1ULL << WIDX_CLOSE) | (1ULL << WIDX_BUTTON_CORRUPT) | (1ULL << WIDX_BUTTON_REMOVE) | (1ULL << WIDX_BUTTON_COPY) | (1ULL << WIDX_LARGE_SCENERY_SPINNER_HEIGHT) | (1ULL << WIDX_LARGE_SCENERY_SPINNER_HEIGHT_INCREASE) | (1ULL << WIDX_LARGE_SCENERY_SPINNER_HEIGHT_DECREASE),
	(1ULL << WIDX_CLOSE) | (1ULL << WIDX_BUTTON_CORRUPT) | (1ULL << WIDX_BUTTON_REMOVE) | (1ULL << WIDX_BUTTON_ROTATE) | (1ULL << WIDX_BUTTON_COPY) | (1ULL << WIDX_BANNER_SPINNER_HEIGHT) | (1ULL << WIDX_BANNER_SPINNER_HEIGHT_INCREASE) | (1ULL << WIDX_BANNER_SPINNER_HEIGHT_DECREASE) | (1ULL << WIDX_BANNER_CHECK_BLOCK_NE) | (1ULL << WIDX_BANNER_CHECK_BLOCK_SE) | (1ULL << WIDX_BANNER_CHECK_BLOCK_SW) | (1ULL << WIDX_BANNER_CHECK_BLOCK_NW),
	(1ULL << WIDX_CLOSE) | (1ULL << WIDX_BUTTON_CORRUPT) | (1ULL << WIDX_BUTTON_REMOVE) | (1ULL << WIDX_BUTTON_COPY) | (1ULL << WIDX_CORRUPT_SPINNER_HEIGHT) | (1ULL << WIDX_CORRUPT_SPINNER_HEIGHT_INCREASE) | (1ULL << WIDX_CORRUPT_SPINNER_HEIGHT_DECREASE) | (1ULL << WIDX_CORRUPT_BUTTON_CLAMP),
};

static uint64 PageDisabledWidgets[] = {
	(1ULL << WIDX_BUTTON_CORRUPT) | (1ULL << WIDX_BUTTON_MOVE_UP) | (1ULL << WIDX_BUTTON_MOVE_DOWN) | (1ULL << WIDX_BUTTON_REMOVE) | (1ULL << WIDX_BUTTON_ROTATE) | (1ULL << WIDX_BUTTON_COPY),
	0,
	0,
	0,
	0,
	0,
	0,
	(1ULL << WIDX_BUTTON_ROTATE),
	0,
	(1ULL << WIDX_BUTTON_ROTATE),
};

void window_tile_inspector_open()
{
	rct_window* window;

	// Check if window is already open
	window = window_bring_to_front_by_class(WC_TILE_INSPECTOR);
	if (window != NULL)
		return;

	window = window_create(
		0,
		29,
		WW,
		WH,
		&TileInspectorWindowEvents,
		WC_TILE_INSPECTOR,
		WF_RESIZABLE
	);
	window_tile_inspector_set_page(window, TILE_INSPECTOR_PAGE_DEFAULT);

	window_init_scroll_widgets(window);
	window->min_width = MIN_WW;
	window->min_height = MIN_WH;
	window->max_width = MAX_WW;
	window->max_height = MAX_WH;
	window->selected_list_item = -1;

	windowTileInspectorTileSelected = false;

	tool_set(window, WIDX_BACKGROUND, TOOL_CROSSHAIR);
	window_tile_inspector_auto_set_buttons(window);
}

void window_tile_inspector_clear_clipboard()
{
	windowTileInspectorElementCopied = false;
}

static rct_map_element* window_tile_inspector_get_selected_element(rct_window *w)
{
	openrct2_assert(w->selected_list_item >= 0 && w->selected_list_item < windowTileInspectorElementCount,
					"Selected list item out of range");
	return map_get_first_element_at(windowTileInspectorTileX, windowTileInspectorTileY) + w->selected_list_item;
}

static void window_tile_inspector_load_tile(rct_window* w)
{
	rct_map_element *element = map_get_first_element_at(windowTileInspectorTileX, windowTileInspectorTileY);
	sint32 numItems = 0;
	do {
		numItems++;
	} while (!map_element_is_last_for_tile(element++));

	windowTileInspectorElementCount = numItems;

	// Set default page
	window_tile_inspector_set_page(w, TILE_INSPECTOR_PAGE_DEFAULT);

	// undo selection and buttons affecting it
	w->selected_list_item = -1;
	window_tile_inspector_auto_set_buttons(w);

	w->scrolls[0].v_top = 0;
	window_invalidate(w);
}

static void window_tile_inspector_insert_corrupt_element(sint32 elementIndex)
{
	openrct2_assert(elementIndex >= 0 && elementIndex < windowTileInspectorElementCount,
					"elementIndex out of range");
	game_do_command(
		TILE_INSPECTOR_ANY_INSERT_CORRUPT,
		GAME_COMMAND_FLAG_APPLY,
		windowTileInspectorTileX | (windowTileInspectorTileY << 8),
		elementIndex,
		GAME_COMMAND_MODIFY_TILE,
		0,
		0
	);
}

static void window_tile_inspector_remove_element(sint32 elementIndex)
{
	openrct2_assert(elementIndex >= 0 && elementIndex < windowTileInspectorElementCount,
		"elementIndex out of range");
	game_do_command(
		TILE_INSPECTOR_ANY_REMOVE,
		GAME_COMMAND_FLAG_APPLY,
		windowTileInspectorTileX | (windowTileInspectorTileY << 8),
		elementIndex,
		GAME_COMMAND_MODIFY_TILE,
		0,
		0
	);
}

static void window_tile_inspector_rotate_element(sint32 elementIndex)
{
	openrct2_assert(elementIndex >= 0 && elementIndex < windowTileInspectorElementCount,
		"elementIndex out of range");
	game_do_command(
		TILE_INSPECTOR_ANY_ROTATE,
		GAME_COMMAND_FLAG_APPLY,
		windowTileInspectorTileX | (windowTileInspectorTileY << 8),
		elementIndex,
		GAME_COMMAND_MODIFY_TILE,
		0,
		0
	);
}

// Swap element with its parent
static void window_tile_inspector_swap_elements(sint16 first, sint16 second)
{
	openrct2_assert(first >= 0 && first < windowTileInspectorElementCount,
					"first out of range");
	openrct2_assert(second >= 0 && second < windowTileInspectorElementCount,
					"second out of range");
	game_do_command(
		TILE_INSPECTOR_ANY_SWAP,
		GAME_COMMAND_FLAG_APPLY,
		windowTileInspectorTileX | (windowTileInspectorTileY << 8),
		first,
		GAME_COMMAND_MODIFY_TILE,
		second,
		0
	);
}

static void window_tile_inspector_sort_elements()
{
	openrct2_assert(windowTileInspectorTileSelected, "No tile selected");
	game_do_command(
		TILE_INSPECTOR_ANY_SORT,
		GAME_COMMAND_FLAG_APPLY,
		windowTileInspectorTileX | (windowTileInspectorTileY << 8),
		0,
		GAME_COMMAND_MODIFY_TILE,
		0,
		0
	);
}

static void window_tile_inspector_copy_element(rct_window *w)
{
	// Copy value, in case the element gets moved
	tileInspectorCopiedElement = *window_tile_inspector_get_selected_element(w);
	windowTileInspectorElementCopied = true;
}

static void window_tile_inspector_paste_element(rct_window *w)
{
	// Construct the data to send using the surface's properties
	sint32 data[2];
	memcpy(&data[0], &tileInspectorCopiedElement, 8);
	assert_struct_size(data, sizeof(tileInspectorCopiedElement));

	game_do_command(
		TILE_INSPECTOR_ANY_PASTE,
		GAME_COMMAND_FLAG_APPLY,
		windowTileInspectorTileX | (windowTileInspectorTileY << 8),
		data[0],
		GAME_COMMAND_MODIFY_TILE,
		data[1],
		0
	);
}

static void window_tile_inspector_base_height_offset(sint16 elementIndex, sint8 heightOffset)
{
	game_do_command(
		TILE_INSPECTOR_ANY_BASE_HEIGHT_OFFSET,
		GAME_COMMAND_FLAG_APPLY,
		windowTileInspectorTileX | (windowTileInspectorTileY << 8),
		elementIndex,
		GAME_COMMAND_MODIFY_TILE,
		heightOffset,
		0
	);
}

static void window_tile_inspector_surface_show_park_fences(bool showFences)
{
	game_do_command(
		TILE_INSPECTOR_SURFACE_SHOW_PARK_FENCES,
		GAME_COMMAND_FLAG_APPLY,
		windowTileInspectorTileX | (windowTileInspectorTileY << 8),
		showFences,
		GAME_COMMAND_MODIFY_TILE,
		0,
		0
	);
}

static void window_tile_inspector_surface_toggle_corner(sint32 cornerIndex)
{
	game_do_command(
		TILE_INSPECTOR_SURFACE_TOGGLE_CORNER,
		GAME_COMMAND_FLAG_APPLY,
		windowTileInspectorTileX | (windowTileInspectorTileY << 8),
		cornerIndex,
		GAME_COMMAND_MODIFY_TILE,
		0,
		0
	);
}

static void window_tile_inspector_surface_toggle_diagonal()
{
	game_do_command(
		TILE_INSPECTOR_SURFACE_TOGGLE_DIAGONAL,
		GAME_COMMAND_FLAG_APPLY,
		windowTileInspectorTileX | (windowTileInspectorTileY << 8),
		0,
		GAME_COMMAND_MODIFY_TILE,
		0,
		0
	);
}

static void window_tile_inspector_path_set_sloped(sint32 elementIndex, bool sloped)
{
	game_do_command(
		TILE_INSPECTOR_PATH_SET_SLOPE,
		GAME_COMMAND_FLAG_APPLY,
		windowTileInspectorTileX | (windowTileInspectorTileY << 8),
		elementIndex,
		GAME_COMMAND_MODIFY_TILE,
		sloped,
		0
	);
}

static void window_tile_inspector_path_toggle_edge(sint32 elementIndex, sint32 cornerIndex)
{
	openrct2_assert(elementIndex >= 0 && elementIndex < windowTileInspectorElementCount,
		"elementIndex out of range");
	openrct2_assert(cornerIndex >= 0 && cornerIndex < 8, "cornerIndex out of range");
	game_do_command(
		TILE_INSPECTOR_PATH_TOGGLE_EDGE,
		GAME_COMMAND_FLAG_APPLY,
		windowTileInspectorTileX | (windowTileInspectorTileY << 8),
		elementIndex,
		GAME_COMMAND_MODIFY_TILE,
		cornerIndex,
		0
	);
}

static void window_tile_inspector_wall_set_slope(sint32 elementIndex, sint32 slopeValue)
{
	// Make sure only the correct bits are set
	openrct2_assert((slopeValue & 0xC0) == slopeValue, "slopeValue doesn't match its mask");

	game_do_command(
		TILE_INSPECTOR_WALL_SET_SLOPE,
		GAME_COMMAND_FLAG_APPLY,
		windowTileInspectorTileX | (windowTileInspectorTileY << 8),
		elementIndex,
		GAME_COMMAND_MODIFY_TILE,
		slopeValue,
		0
	);
}

static void window_tile_inspector_track_block_height_offset(sint32 elementIndex, sint8 heightOffset)
{
	game_do_command(
		TILE_INSPECTOR_TRACK_BASE_HEIGHT_OFFSET,
		GAME_COMMAND_FLAG_APPLY,
		windowTileInspectorTileX | (windowTileInspectorTileY << 8),
		elementIndex,
		GAME_COMMAND_MODIFY_TILE,
		heightOffset,
		0
	);
}

static void window_tile_inspector_track_block_set_lift(sint32 elementIndex, bool entireTrackBlock, bool chain)
{
	game_do_command(
		TILE_INSPECTOR_TRACK_SET_CHAIN,
		GAME_COMMAND_FLAG_APPLY,
		windowTileInspectorTileX | (windowTileInspectorTileY << 8),
		elementIndex,
		GAME_COMMAND_MODIFY_TILE,
		entireTrackBlock,
		chain
	);
}

static void window_tile_inspector_quarter_tile_set(sint32 elementIndex, const sint32 quarterIndex)
{
	// quarterIndex is widget index relative to WIDX_SCENERY_CHECK_QUARTER_N, so a value from 0-3
	openrct2_assert(quarterIndex >= 0 && quarterIndex < 4, "quarterIndex out of range");

	game_do_command(
		TILE_INSPECTOR_SCENERY_SET_QUARTER_LOCATION,
		GAME_COMMAND_FLAG_APPLY,
		windowTileInspectorTileX | (windowTileInspectorTileY << 8),
		elementIndex,
		GAME_COMMAND_MODIFY_TILE,
		(quarterIndex - get_current_rotation()) & 3,
		0
	);
}

static void window_tile_inspector_toggle_quadrant_collosion(sint32 elementIndex, const sint32 quadrantIndex)
{
	game_do_command(
		TILE_INSPECTOR_SCENERY_SET_QUARTER_COLLISION,
		GAME_COMMAND_FLAG_APPLY,
		windowTileInspectorTileX | (windowTileInspectorTileY << 8),
		elementIndex,
		GAME_COMMAND_MODIFY_TILE,
		(quadrantIndex + 2 - get_current_rotation()) & 3,
		0
	);
}

static void window_tile_inspector_banner_toggle_block(sint32 elementIndex, sint32 edgeIndex)
{
	openrct2_assert(edgeIndex >= 0 && edgeIndex < 4, "edgeIndex out of range");

	// Make edgeIndex abstract
	edgeIndex = (edgeIndex - get_current_rotation()) & 3;

	game_do_command(
		TILE_INSPECTOR_BANNER_TOGGLE_BLOCKING_EDGE,
		GAME_COMMAND_FLAG_APPLY,
		windowTileInspectorTileX | (windowTileInspectorTileY << 8),
		elementIndex,
		GAME_COMMAND_MODIFY_TILE,
		edgeIndex,
		0
	);
}

static void window_tile_inspector_clamp_corrupt(sint32 elementIndex)
{
	game_do_command(
		TILE_INSPECTOR_CORRUPT_CLAMP,
		GAME_COMMAND_FLAG_APPLY,
		windowTileInspectorTileX | (windowTileInspectorTileY << 8),
		elementIndex,
		GAME_COMMAND_MODIFY_TILE,
		0,
		0
	);
}

static void window_tile_inspector_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_CLOSE:
		tool_cancel();
		window_close(w);
		break;
	case WIDX_SPINNER_X_INCREASE:
		windowTileInspectorTileX = min(windowTileInspectorTileX + 1, 255);
		window_tile_inspector_load_tile(w);
		window_tile_inspector_auto_set_buttons(w);
		break;
	case WIDX_SPINNER_X_DECREASE:
		windowTileInspectorTileX = max(windowTileInspectorTileX - 1, 0);
		window_tile_inspector_load_tile(w);
		window_tile_inspector_auto_set_buttons(w);
		break;
	case WIDX_SPINNER_Y_INCREASE:
		windowTileInspectorTileY = min(windowTileInspectorTileY + 1, 255);
		window_tile_inspector_load_tile(w);
		window_tile_inspector_auto_set_buttons(w);
		break;
	case WIDX_SPINNER_Y_DECREASE:
		windowTileInspectorTileY = max(windowTileInspectorTileY - 1, 0);
		window_tile_inspector_load_tile(w);
		window_tile_inspector_auto_set_buttons(w);
		break;
	case WIDX_BUTTON_CORRUPT:
		window_tile_inspector_insert_corrupt_element(w->selected_list_item);
		break;
	case WIDX_BUTTON_REMOVE:
		window_tile_inspector_remove_element(w->selected_list_item);
		break;
	case WIDX_BUTTON_ROTATE:
		window_tile_inspector_rotate_element(w->selected_list_item);
		break;
	case WIDX_BUTTON_SORT:
		window_tile_inspector_sort_elements();
		break;
	case WIDX_BUTTON_COPY:
		window_tile_inspector_copy_element(w);
		window_tile_inspector_auto_set_buttons(w);
		window_invalidate(w);
		break;
	case WIDX_BUTTON_PASTE:
		window_tile_inspector_paste_element(w);
		break;
	case WIDX_BUTTON_MOVE_DOWN:
		window_tile_inspector_swap_elements(w->selected_list_item, w->selected_list_item + 1);
		break;
	case WIDX_BUTTON_MOVE_UP:
		window_tile_inspector_swap_elements(w->selected_list_item - 1, w->selected_list_item);
		break;
	}

	// Only element-specific widgets from now on
	if (w->page == TILE_INSPECTOR_PAGE_DEFAULT) {
		return;
	}

	// Get the selected map element
	rct_map_element *const mapElement = window_tile_inspector_get_selected_element(w);

	// Page widgets
	switch (w->page) {
	case TILE_INSPECTOR_PAGE_SURFACE:
		switch (widgetIndex) {
		case WIDX_SURFACE_SPINNER_HEIGHT_INCREASE:
			window_tile_inspector_base_height_offset(w->selected_list_item, 1);
			break;
		case WIDX_SURFACE_SPINNER_HEIGHT_DECREASE:
			window_tile_inspector_base_height_offset(w->selected_list_item, -1);
			break;
		case WIDX_SURFACE_BUTTON_REMOVE_FENCES:
			window_tile_inspector_surface_show_park_fences(false);
			break;
		case WIDX_SURFACE_BUTTON_RESTORE_FENCES:
			window_tile_inspector_surface_show_park_fences(true);
			break;
		case WIDX_SURFACE_CHECK_CORNER_N:
		case WIDX_SURFACE_CHECK_CORNER_E:
		case WIDX_SURFACE_CHECK_CORNER_S:
		case WIDX_SURFACE_CHECK_CORNER_W:
			window_tile_inspector_surface_toggle_corner(((widgetIndex - WIDX_SURFACE_CHECK_CORNER_N) + 2 - get_current_rotation()) & 3);
			break;
		case WIDX_SURFACE_CHECK_DIAGONAL:
			window_tile_inspector_surface_toggle_diagonal();
			break;
		} // switch widgetindex
		break;

	case TILE_INSPECTOR_PAGE_PATH:
		switch (widgetIndex) {
		case WIDX_PATH_SPINNER_HEIGHT_INCREASE:
			window_tile_inspector_base_height_offset(w->selected_list_item, 1);
			break;
		case WIDX_PATH_SPINNER_HEIGHT_DECREASE:
			window_tile_inspector_base_height_offset(w->selected_list_item, -1);
			break;
		case WIDX_PATH_CHECK_SLOPED:
			window_tile_inspector_path_set_sloped(w->selected_list_item, !footpath_element_is_sloped(mapElement));
			break;
		case WIDX_PATH_CHECK_EDGE_E:
		case WIDX_PATH_CHECK_EDGE_S:
		case WIDX_PATH_CHECK_EDGE_W:
		case WIDX_PATH_CHECK_EDGE_N:
		{
			// 0 = east/right, 1 = south/bottom, 2 = west/left, 3 = north/top
			const sint32 eswn = (widgetIndex - WIDX_PATH_CHECK_EDGE_E) / 2;
			// Transform to world orientation
			const sint32 index = (4 + eswn - get_current_rotation()) & 3;
			window_tile_inspector_path_toggle_edge(w->selected_list_item, index + 4);  // The corners are stored in the 4 most significant bits, hence the + 4
			break;
		}
		case WIDX_PATH_CHECK_EDGE_NE:
		case WIDX_PATH_CHECK_EDGE_SE:
		case WIDX_PATH_CHECK_EDGE_SW:
		case WIDX_PATH_CHECK_EDGE_NW:
		{
			// 0 = NE, 1 = SE, 2 = SW, 3 = NW
			const sint32 neseswnw = (widgetIndex - WIDX_PATH_CHECK_EDGE_NE) / 2;
			// Transform to world orientation
			const sint32 index = (4 + neseswnw - get_current_rotation()) & 3;
			window_tile_inspector_path_toggle_edge(w->selected_list_item, index);
			break;
		}
		} // switch widget index
		break;

	case TILE_INSPECTOR_PAGE_TRACK:
		switch (widgetIndex) {
		case WIDX_TRACK_CHECK_APPLY_TO_ALL:
			windowTileInspectorApplyToAll ^= 1;
			widget_invalidate(w, widgetIndex);
			break;
		case WIDX_TRACK_SPINNER_HEIGHT_INCREASE:
			if (widget_is_pressed(w, WIDX_TRACK_CHECK_APPLY_TO_ALL)) {
				window_tile_inspector_track_block_height_offset(w->selected_list_item, 1);
			}
			else {
				window_tile_inspector_base_height_offset(w->selected_list_item, 1);
			}
			break;
		case WIDX_TRACK_SPINNER_HEIGHT_DECREASE:
			if (widget_is_pressed(w, WIDX_TRACK_CHECK_APPLY_TO_ALL)) {
				window_tile_inspector_track_block_height_offset(w->selected_list_item, -1);
			}
			else {
				window_tile_inspector_base_height_offset(w->selected_list_item, -1);
			}
			break;
		case WIDX_TRACK_CHECK_CHAIN_LIFT:
		{
			bool entireTrackBlock = widget_is_pressed(w, WIDX_TRACK_CHECK_APPLY_TO_ALL);
			bool newLift = !track_element_is_lift_hill(mapElement);
			window_tile_inspector_track_block_set_lift(w->selected_list_item, entireTrackBlock, newLift);
			break;
		}
		} // switch widget index
		break;

	case TILE_INSPECTOR_PAGE_SCENERY:
		switch (widgetIndex) {
		case WIDX_SCENERY_SPINNER_HEIGHT_INCREASE:
			window_tile_inspector_base_height_offset(w->selected_list_item, 1);
			break;
		case WIDX_SCENERY_SPINNER_HEIGHT_DECREASE:
			window_tile_inspector_base_height_offset(w->selected_list_item, -1);
			break;
		case WIDX_SCENERY_CHECK_QUARTER_N:
		case WIDX_SCENERY_CHECK_QUARTER_E:
		case WIDX_SCENERY_CHECK_QUARTER_S:
		case WIDX_SCENERY_CHECK_QUARTER_W:
			window_tile_inspector_quarter_tile_set(w->selected_list_item, widgetIndex - WIDX_SCENERY_CHECK_QUARTER_N);
			break;
		case WIDX_SCENERY_CHECK_COLLISION_N:
		case WIDX_SCENERY_CHECK_COLLISION_E:
		case WIDX_SCENERY_CHECK_COLLISION_S:
		case WIDX_SCENERY_CHECK_COLLISION_W:
			window_tile_inspector_toggle_quadrant_collosion(w->selected_list_item, widgetIndex - WIDX_SCENERY_CHECK_COLLISION_N);
			break;
		} // switch widget index
		break;

	case TILE_INSPECTOR_PAGE_ENTRANCE:
		switch (widgetIndex) {
		case WIDX_ENTRANCE_SPINNER_HEIGHT_INCREASE:
			window_tile_inspector_base_height_offset(w->selected_list_item, 1);
			break;
		case WIDX_ENTRANCE_SPINNER_HEIGHT_DECREASE:
			window_tile_inspector_base_height_offset(w->selected_list_item, -1);
			break;
		} // switch widget index
		break;

	case TILE_INSPECTOR_PAGE_WALL:
		switch (widgetIndex) {
		case WIDX_WALL_SPINNER_HEIGHT_INCREASE:
			window_tile_inspector_base_height_offset(w->selected_list_item, 1);
			break;
		case WIDX_WALL_SPINNER_HEIGHT_DECREASE:
			window_tile_inspector_base_height_offset(w->selected_list_item, -1);
			break;
		} // switch widget index
		break;

	case TILE_INSPECTOR_PAGE_LARGE_SCENERY:
		switch (widgetIndex) {
		case WIDX_LARGE_SCENERY_SPINNER_HEIGHT_INCREASE:
			window_tile_inspector_base_height_offset(w->selected_list_item, 1);
			break;
		case WIDX_LARGE_SCENERY_SPINNER_HEIGHT_DECREASE:
			window_tile_inspector_base_height_offset(w->selected_list_item, -1);
			break;
		} // switch widget index
		break;

	case TILE_INSPECTOR_PAGE_BANNER:
		switch (widgetIndex) {
		case WIDX_BANNER_SPINNER_HEIGHT_INCREASE:
			window_tile_inspector_base_height_offset(w->selected_list_item, 1);
			break;
		case WIDX_BANNER_SPINNER_HEIGHT_DECREASE:
			window_tile_inspector_base_height_offset(w->selected_list_item, -1);
			break;
		case WIDX_BANNER_CHECK_BLOCK_NE:
		case WIDX_BANNER_CHECK_BLOCK_SE:
		case WIDX_BANNER_CHECK_BLOCK_SW:
		case WIDX_BANNER_CHECK_BLOCK_NW:
			window_tile_inspector_banner_toggle_block(w->selected_list_item, widgetIndex - WIDX_BANNER_CHECK_BLOCK_NE);
			break;
		} // switch widget index
		break;

	case TILE_INSPECTOR_PAGE_CORRUPT:
		switch (widgetIndex) {
		case WIDX_CORRUPT_SPINNER_HEIGHT_INCREASE:
			window_tile_inspector_base_height_offset(w->selected_list_item, 1);
			break;
		case WIDX_CORRUPT_SPINNER_HEIGHT_DECREASE:
			window_tile_inspector_base_height_offset(w->selected_list_item, -1);
			break;
		case WIDX_CORRUPT_BUTTON_CLAMP:
			window_tile_inspector_clamp_corrupt(w->selected_list_item);
			break;
		} // switch widget index
		break;
	} // switch page
}

static void window_tile_inspector_resize(rct_window *w)
{
	w->min_width = WW;
	w->min_height = MIN_WH;
	if (w->width < w->min_width) {
		window_invalidate(w);
		w->width = w->min_width;
	}
	if (w->height < w->min_height) {
		window_invalidate(w);
		w->height = w->min_height;
	}
}

static void window_tile_inspector_mousedown(rct_widgetindex widgetIndex, rct_window *w, rct_widget* widget)
{
	switch (w->page) {
	case TILE_INSPECTOR_PAGE_WALL:
		switch (widgetIndex) {
		case WIDX_WALL_DROPDOWN_SLOPE_BUTTON:
			// Use dropdown instead of dropdown button
			widget--;

			// Fill dropdown list
			gDropdownItemsFormat[0] = STR_DROPDOWN_MENU_LABEL;
			gDropdownItemsFormat[1] = STR_DROPDOWN_MENU_LABEL;
			gDropdownItemsFormat[2] = STR_DROPDOWN_MENU_LABEL;
			gDropdownItemsArgs[0] = STR_TILE_INSPECTOR_WALL_FLAT;
			gDropdownItemsArgs[1] = STR_TILE_INSPECTOR_WALL_SLOPED_LEFT;
			gDropdownItemsArgs[2] = STR_TILE_INSPECTOR_WALL_SLOPED_RIGHT;
			window_dropdown_show_text_custom_width(
				w->x + widget->left,
				w->y + widget->top,
				widget->bottom - widget->top + 1,
				w->colours[1],
				0,
				DROPDOWN_FLAG_STAY_OPEN,
				3,
				widget->right - widget->left - 3
			);

			// Set current value as checked
			rct_map_element *const mapElement = window_tile_inspector_get_selected_element(w);
			dropdown_set_checked((mapElement->type & 0xC0) >> 6, true);
			break;
		}
		break;
	}
}

static void window_tile_inspector_update(rct_window *w)
{
	// Check if the mouse is hovering over the list
	if (!widget_is_highlighted(w, WIDX_LIST)) {
		windowTileInspectorHighlightedIndex = -1;
		widget_invalidate(w, WIDX_LIST);
	}

	if (gCurrentToolWidget.window_classification != WC_TILE_INSPECTOR)
		window_close(w);
}

static void window_tile_inspector_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex)
{
	if (dropdownIndex == -1) {
		return;
	}

	// Get selected element
	rct_map_element *const mapElement = window_tile_inspector_get_selected_element(w);

	switch (w->page) {
	case TILE_INSPECTOR_PAGE_WALL:
		openrct2_assert(map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_WALL, "Element is not a wall");

		switch (widgetIndex) {
		case WIDX_WALL_DROPDOWN_SLOPE_BUTTON:
			window_tile_inspector_wall_set_slope(w->selected_list_item, dropdownIndex << 6);
			break;
		}
		break;
	}
}

static void window_tile_inspector_tool_update(rct_window* w, rct_widgetindex widgetIndex, sint32 x, sint32 y)
{
	map_invalidate_selection_rect();

	gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;

	sint16 mapX = x;
	sint16 mapY = y;
	sint32 direction;
	screen_pos_to_map_pos(&mapX, &mapY, &direction);
	if (mapX != MAP_LOCATION_NULL) {
		gMapSelectPositionA.x = gMapSelectPositionB.x = mapX;
		gMapSelectPositionA.y = gMapSelectPositionB.y = mapY;
	}
	else if (windowTileInspectorTileSelected){
		gMapSelectPositionA.x = gMapSelectPositionB.x = windowTileInspectorTileX << 5;
		gMapSelectPositionA.y = gMapSelectPositionB.y = windowTileInspectorTileY << 5;
	}
	else {
		gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
	}

	gMapSelectType = MAP_SELECT_TYPE_FULL;
	map_invalidate_selection_rect();
}

static void window_tile_inspector_update_selected_tile(rct_window *w, sint32 x, sint32 y)
{
	// Mouse hasn't moved
	if (x == windowTileInspectorToolMouseX && y == windowTileInspectorToolMouseY) {
		return;
	}
	windowTileInspectorToolMouseX = x;
	windowTileInspectorToolMouseY = y;

	sint16 mapX = x;
	sint16 mapY = y;
	sint32 direction;
	screen_pos_to_map_pos(&mapX, &mapY, &direction);
	if (mapX == MAP_LOCATION_NULL) {
		return;
	}

	// Tile is already selected
	if (windowTileInspectorTileSelected && mapX == windowTileInspectorToolMapX && mapY == windowTileInspectorToolMapY) {
		return;
	}

	windowTileInspectorTileSelected = true;
	windowTileInspectorToolMapX = mapX;
	windowTileInspectorToolMapY = mapY;
	windowTileInspectorTileX = mapX >> 5;
	windowTileInspectorTileY = mapY >> 5;

	window_tile_inspector_load_tile(w);
	window_tile_inspector_auto_set_buttons(w);
}

static void window_tile_inspector_tool_down(rct_window* w, rct_widgetindex widgetIndex, sint32 x, sint32 y)
{
	window_tile_inspector_update_selected_tile(w, x, y);
}

static void window_tile_inspector_tool_drag(rct_window* w, rct_widgetindex widgetIndex, sint32 x, sint32 y)
{
	window_tile_inspector_update_selected_tile(w, x, y);
}

static void window_tile_inspector_scrollgetsize(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height)
{
	*width = WW - 30;
	*height = windowTileInspectorElementCount * LIST_ITEM_HEIGHT;
}

void window_tile_inspector_set_page(rct_window *w, const tile_inspector_page page)
{
	w->page = page;
	w->widgets = PageWidgets[page];
	w->enabled_widgets = PageEnabledWidgets[page];
	w->disabled_widgets = PageDisabledWidgets[page];
}

void window_tile_inspector_auto_set_buttons(rct_window *w)
{
	// X and Y spinners
	widget_set_enabled(w, WIDX_SPINNER_X_INCREASE, (windowTileInspectorTileSelected && (windowTileInspectorTileX < 255)));
	widget_set_enabled(w, WIDX_SPINNER_X_DECREASE, (windowTileInspectorTileSelected && (windowTileInspectorTileX > 0)));
	widget_set_enabled(w, WIDX_SPINNER_Y_INCREASE, (windowTileInspectorTileSelected && (windowTileInspectorTileY < 255)));
	widget_set_enabled(w, WIDX_SPINNER_Y_DECREASE, (windowTileInspectorTileSelected && (windowTileInspectorTileY > 0)));

	// Sort buttons
	widget_set_enabled(w, WIDX_BUTTON_SORT, (windowTileInspectorTileSelected && windowTileInspectorElementCount > 1));

	// Move Up button
	widget_set_enabled(w, WIDX_BUTTON_MOVE_UP, (w->selected_list_item > 0));
	widget_invalidate(w, WIDX_BUTTON_MOVE_UP);

	// Move Down button
	widget_set_enabled(w, WIDX_BUTTON_MOVE_DOWN, (w->selected_list_item != -1 && w->selected_list_item < windowTileInspectorElementCount - 1));
	widget_invalidate(w, WIDX_BUTTON_MOVE_DOWN);

	// Copy button
	widget_set_enabled(w, WIDX_BUTTON_COPY, w->selected_list_item >= 0);
	widget_invalidate(w, WIDX_BUTTON_COPY);

	// Paste button
	widget_set_enabled(w, WIDX_BUTTON_PASTE, windowTileInspectorTileSelected && windowTileInspectorElementCopied);
	widget_invalidate(w, WIDX_BUTTON_PASTE);

	// Page widgets
	switch (w->page) {
	case TILE_INSPECTOR_PAGE_WALL: {
		const rct_map_element *const mapElement = window_tile_inspector_get_selected_element(w);
		const uint8 wallType = mapElement->properties.wall.type;
		const rct_wall_scenery_entry wallEntry = get_wall_entry(wallType)->wall;
		const bool canBeSloped = !(wallEntry.flags & WALL_SCENERY_CANT_BUILD_ON_SLOPE);
		// Wall slope dropdown
		widget_set_enabled(w, WIDX_WALL_DROPDOWN_SLOPE, canBeSloped);
		widget_invalidate(w, WIDX_WALL_DROPDOWN_SLOPE);
		widget_set_enabled(w, WIDX_WALL_DROPDOWN_SLOPE_BUTTON, canBeSloped);
		widget_invalidate(w, WIDX_WALL_DROPDOWN_SLOPE_BUTTON);
		break;
	}
	} // switch page
}

static void window_tile_inspector_scrollmousedown(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y)
{
	// Because the list items are displayed in reverse order, subtract the calculated index from the amount of elements
	const sint16 index = windowTileInspectorElementCount - (y - 1) / LIST_ITEM_HEIGHT - 1;
	sint32 page;
	if (index < 0 || index >= windowTileInspectorElementCount) {
		w->selected_list_item = -1;
		page = 0;
	}
	else {
		w->selected_list_item = index;

		// Get type of selected map element to select the correct page
		rct_map_element *const mapElement = window_tile_inspector_get_selected_element(w);
		page = (min(map_element_get_type(mapElement), MAP_ELEMENT_TYPE_CORRUPT) >> 2) + 1;
	}

	window_tile_inspector_set_page(w, page);

	// Enable/disable buttons
	window_tile_inspector_auto_set_buttons(w);
	window_invalidate(w);
}

static void window_tile_inspector_scrollmouseover(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y)
{
	sint16 index = windowTileInspectorElementCount - (y - 1) / LIST_ITEM_HEIGHT - 1;
	if (index < 0 || index >= windowTileInspectorElementCount)
		windowTileInspectorHighlightedIndex = -1;
	else
		windowTileInspectorHighlightedIndex = index;

	widget_invalidate(w, WIDX_LIST);
}

static void window_tile_inspector_invalidate(rct_window *w)
{
	w->widgets[WIDX_BACKGROUND].bottom = w->height - 1;

	if (w->page == TILE_INSPECTOR_PAGE_DEFAULT) {
		w->widgets[WIDX_GROUPBOX_DETAILS].type = WWT_EMPTY;
		w->widgets[WIDX_GROUPBOX_PROPERTIES].type = WWT_EMPTY;
		w->widgets[WIDX_LIST].bottom = w->height - PADDING_BOTTOM;
	}
	else {
		w->widgets[WIDX_GROUPBOX_DETAILS].type = WWT_GROUPBOX;
		w->widgets[WIDX_GROUPBOX_PROPERTIES].type = WWT_GROUPBOX;
		w->widgets[WIDX_GROUPBOX_DETAILS].image = PageGroupBoxSettings[w->page - 1].string_id;
		w->widgets[WIDX_GROUPBOX_DETAILS].top = w->height - PageGroupBoxSettings[w->page - 1].details_top_offset;
		w->widgets[WIDX_GROUPBOX_DETAILS].bottom = w->height - PageGroupBoxSettings[w->page - 1].details_bottom_offset;
		w->widgets[WIDX_GROUPBOX_PROPERTIES].top = w->height - PageGroupBoxSettings[w->page - 1].properties_top_offset;
		w->widgets[WIDX_GROUPBOX_PROPERTIES].bottom = w->height - PageGroupBoxSettings[w->page - 1].properties_bottom_offset;
		w->widgets[WIDX_LIST].bottom = w->widgets[WIDX_GROUPBOX_DETAILS].top - GROUPBOX_PADDING;
	}

	// Only page-specific widgets related to the map element will be
	if (w->page == TILE_INSPECTOR_PAGE_DEFAULT) {
		return;
	}

	// Using a switch, because I don't think giving each page their own callbacks is
	// needed here, as only the mouseup and invalidate functions are different.
	const sint32 propertiesAnchor = w->widgets[WIDX_GROUPBOX_PROPERTIES].top;
	rct_map_element *const mapElement = window_tile_inspector_get_selected_element(w);

	switch (w->page) {
	case TILE_INSPECTOR_PAGE_SURFACE:
		w->widgets[WIDX_SURFACE_SPINNER_HEIGHT].top = GBBT(propertiesAnchor, 0) + 3;
		w->widgets[WIDX_SURFACE_SPINNER_HEIGHT].bottom = GBBB(propertiesAnchor, 0) - 3;
		w->widgets[WIDX_SURFACE_SPINNER_HEIGHT_INCREASE].top = GBBT(propertiesAnchor, 0) + 4;
		w->widgets[WIDX_SURFACE_SPINNER_HEIGHT_INCREASE].bottom = GBBT(propertiesAnchor, 0) + 8;
		w->widgets[WIDX_SURFACE_SPINNER_HEIGHT_DECREASE].top = GBBB(propertiesAnchor, 0) - 8;
		w->widgets[WIDX_SURFACE_SPINNER_HEIGHT_DECREASE].bottom = GBBB(propertiesAnchor, 0) - 4;
		w->widgets[WIDX_SURFACE_BUTTON_REMOVE_FENCES].top = GBBT(propertiesAnchor, 1);
		w->widgets[WIDX_SURFACE_BUTTON_REMOVE_FENCES].bottom = GBBB(propertiesAnchor, 1);
		w->widgets[WIDX_SURFACE_BUTTON_RESTORE_FENCES].top = GBBT(propertiesAnchor, 1);
		w->widgets[WIDX_SURFACE_BUTTON_RESTORE_FENCES].bottom = GBBB(propertiesAnchor, 1);
		w->widgets[WIDX_SURFACE_CHECK_CORNER_N].top = GBBT(propertiesAnchor, 2) + 7 * 0;
		w->widgets[WIDX_SURFACE_CHECK_CORNER_N].bottom = w->widgets[WIDX_SURFACE_CHECK_CORNER_N].top + 13;
		w->widgets[WIDX_SURFACE_CHECK_CORNER_E].top = GBBT(propertiesAnchor, 2) + 7 * 1;
		w->widgets[WIDX_SURFACE_CHECK_CORNER_E].bottom = w->widgets[WIDX_SURFACE_CHECK_CORNER_E].top + 13;
		w->widgets[WIDX_SURFACE_CHECK_CORNER_S].top = GBBT(propertiesAnchor, 2) + 7 * 2;
		w->widgets[WIDX_SURFACE_CHECK_CORNER_S].bottom = w->widgets[WIDX_SURFACE_CHECK_CORNER_S].top + 13;
		w->widgets[WIDX_SURFACE_CHECK_CORNER_W].top = GBBT(propertiesAnchor, 2) + 7 * 1;
		w->widgets[WIDX_SURFACE_CHECK_CORNER_W].bottom = w->widgets[WIDX_SURFACE_CHECK_CORNER_W].top + 13;
		w->widgets[WIDX_SURFACE_CHECK_DIAGONAL].top = GBBT(propertiesAnchor, 3) + 7 * 1;
		w->widgets[WIDX_SURFACE_CHECK_DIAGONAL].bottom = w->widgets[WIDX_SURFACE_CHECK_DIAGONAL].top + 13;
		widget_set_checkbox_value(w, WIDX_SURFACE_CHECK_CORNER_N, mapElement->properties.surface.slope & (1 << ((2 - get_current_rotation()) & 3)));
		widget_set_checkbox_value(w, WIDX_SURFACE_CHECK_CORNER_E, mapElement->properties.surface.slope & (1 << ((3 - get_current_rotation()) & 3)));
		widget_set_checkbox_value(w, WIDX_SURFACE_CHECK_CORNER_S, mapElement->properties.surface.slope & (1 << ((0 - get_current_rotation()) & 3)));
		widget_set_checkbox_value(w, WIDX_SURFACE_CHECK_CORNER_W, mapElement->properties.surface.slope & (1 << ((1 - get_current_rotation()) & 3)));
		widget_set_checkbox_value(w, WIDX_SURFACE_CHECK_DIAGONAL, mapElement->properties.surface.slope & 0x10);
		break;
	case TILE_INSPECTOR_PAGE_PATH:
		w->widgets[WIDX_PATH_SPINNER_HEIGHT].top = GBBT(propertiesAnchor, 0) + 3;
		w->widgets[WIDX_PATH_SPINNER_HEIGHT].bottom = GBBB(propertiesAnchor, 0) - 3;
		w->widgets[WIDX_PATH_SPINNER_HEIGHT_INCREASE].top = GBBT(propertiesAnchor, 0) + 4;
		w->widgets[WIDX_PATH_SPINNER_HEIGHT_INCREASE].bottom = GBBT(propertiesAnchor, 0) + 8;
		w->widgets[WIDX_PATH_SPINNER_HEIGHT_DECREASE].top = GBBB(propertiesAnchor, 0) - 8;
		w->widgets[WIDX_PATH_SPINNER_HEIGHT_DECREASE].bottom = GBBB(propertiesAnchor, 0) - 4;
		w->widgets[WIDX_PATH_CHECK_SLOPED].top = GBBT(propertiesAnchor, 1) + 2;
		w->widgets[WIDX_PATH_CHECK_SLOPED].bottom = GBBT(propertiesAnchor, 1) + 15;
		w->widgets[WIDX_PATH_CHECK_EDGE_N].top = GBBT(propertiesAnchor, 2) + 7 * 0;
		w->widgets[WIDX_PATH_CHECK_EDGE_N].bottom = w->widgets[WIDX_PATH_CHECK_EDGE_N].top + 13;
		w->widgets[WIDX_PATH_CHECK_EDGE_NE].top = GBBT(propertiesAnchor, 2) + 7 * 1;
		w->widgets[WIDX_PATH_CHECK_EDGE_NE].bottom = w->widgets[WIDX_PATH_CHECK_EDGE_NE].top + 13;
		w->widgets[WIDX_PATH_CHECK_EDGE_E].top = GBBT(propertiesAnchor, 2) + 7 * 2;
		w->widgets[WIDX_PATH_CHECK_EDGE_E].bottom = w->widgets[WIDX_PATH_CHECK_EDGE_E].top + 13;
		w->widgets[WIDX_PATH_CHECK_EDGE_SE].top = GBBT(propertiesAnchor, 2) + 7 * 3;
		w->widgets[WIDX_PATH_CHECK_EDGE_SE].bottom = w->widgets[WIDX_PATH_CHECK_EDGE_SE].top + 13;
		w->widgets[WIDX_PATH_CHECK_EDGE_S].top = GBBT(propertiesAnchor, 2) + 7 * 4;
		w->widgets[WIDX_PATH_CHECK_EDGE_S].bottom = w->widgets[WIDX_PATH_CHECK_EDGE_S].top + 13;
		w->widgets[WIDX_PATH_CHECK_EDGE_SW].top = GBBT(propertiesAnchor, 2) + 7 * 3;
		w->widgets[WIDX_PATH_CHECK_EDGE_SW].bottom = w->widgets[WIDX_PATH_CHECK_EDGE_SW].top + 13;
		w->widgets[WIDX_PATH_CHECK_EDGE_W].top = GBBT(propertiesAnchor, 2) + 7 * 2;
		w->widgets[WIDX_PATH_CHECK_EDGE_W].bottom = w->widgets[WIDX_PATH_CHECK_EDGE_W].top + 13;
		w->widgets[WIDX_PATH_CHECK_EDGE_NW].top = GBBT(propertiesAnchor, 2) + 7 * 1;
		w->widgets[WIDX_PATH_CHECK_EDGE_NW].bottom = w->widgets[WIDX_PATH_CHECK_EDGE_NW].top + 13;
		widget_set_checkbox_value(w, WIDX_PATH_CHECK_SLOPED, footpath_element_is_sloped(mapElement));
		widget_set_checkbox_value(w, WIDX_PATH_CHECK_EDGE_NE, mapElement->properties.path.edges & (1 << ((0 - get_current_rotation()) & 3)));
		widget_set_checkbox_value(w, WIDX_PATH_CHECK_EDGE_SE, mapElement->properties.path.edges & (1 << ((1 - get_current_rotation()) & 3)));
		widget_set_checkbox_value(w, WIDX_PATH_CHECK_EDGE_SW, mapElement->properties.path.edges & (1 << ((2 - get_current_rotation()) & 3)));
		widget_set_checkbox_value(w, WIDX_PATH_CHECK_EDGE_NW, mapElement->properties.path.edges & (1 << ((3 - get_current_rotation()) & 3)));
		widget_set_checkbox_value(w, WIDX_PATH_CHECK_EDGE_E, mapElement->properties.path.edges & (1 << (4 + ((0 - get_current_rotation()) & 3))));
		widget_set_checkbox_value(w, WIDX_PATH_CHECK_EDGE_S, mapElement->properties.path.edges & (1 << (4 + ((1 - get_current_rotation()) & 3))));
		widget_set_checkbox_value(w, WIDX_PATH_CHECK_EDGE_W, mapElement->properties.path.edges & (1 << (4 + ((2 - get_current_rotation()) & 3))));
		widget_set_checkbox_value(w, WIDX_PATH_CHECK_EDGE_N, mapElement->properties.path.edges & (1 << (4 + ((3 - get_current_rotation()) & 3))));
		break;
	case TILE_INSPECTOR_PAGE_TRACK:
		w->widgets[WIDX_TRACK_CHECK_APPLY_TO_ALL].top = GBBT(propertiesAnchor, 0);
		w->widgets[WIDX_TRACK_CHECK_APPLY_TO_ALL].bottom = GBBB(propertiesAnchor, 0);
		w->widgets[WIDX_TRACK_SPINNER_HEIGHT].top = GBBT(propertiesAnchor, 1) + 3;
		w->widgets[WIDX_TRACK_SPINNER_HEIGHT].bottom = GBBB(propertiesAnchor, 1) - 3;
		w->widgets[WIDX_TRACK_SPINNER_HEIGHT_INCREASE].top = GBBT(propertiesAnchor, 1) + 4;
		w->widgets[WIDX_TRACK_SPINNER_HEIGHT_INCREASE].bottom = GBBT(propertiesAnchor, 1) + 8;
		w->widgets[WIDX_TRACK_SPINNER_HEIGHT_DECREASE].top = GBBB(propertiesAnchor, 1) - 8;
		w->widgets[WIDX_TRACK_SPINNER_HEIGHT_DECREASE].bottom = GBBB(propertiesAnchor, 1) - 4;
		w->widgets[WIDX_TRACK_CHECK_CHAIN_LIFT].top = GBBT(propertiesAnchor, 2);
		w->widgets[WIDX_TRACK_CHECK_CHAIN_LIFT].bottom = GBBB(propertiesAnchor, 2);
		widget_set_checkbox_value(w, WIDX_TRACK_CHECK_APPLY_TO_ALL, windowTileInspectorApplyToAll);
		widget_set_checkbox_value(w, WIDX_TRACK_CHECK_CHAIN_LIFT, track_element_is_lift_hill(mapElement));
		break;
	case TILE_INSPECTOR_PAGE_SCENERY: {
		// Raise / Lower
		w->widgets[WIDX_SCENERY_SPINNER_HEIGHT].top = GBBT(propertiesAnchor, 0) + 3;
		w->widgets[WIDX_SCENERY_SPINNER_HEIGHT].bottom = GBBB(propertiesAnchor, 0) - 3;
		w->widgets[WIDX_SCENERY_SPINNER_HEIGHT_INCREASE].top = GBBT(propertiesAnchor, 0) + 4;
		w->widgets[WIDX_SCENERY_SPINNER_HEIGHT_INCREASE].bottom = GBBT(propertiesAnchor, 0) + 8;
		w->widgets[WIDX_SCENERY_SPINNER_HEIGHT_DECREASE].top = GBBB(propertiesAnchor, 0) - 8;
		w->widgets[WIDX_SCENERY_SPINNER_HEIGHT_DECREASE].bottom = GBBB(propertiesAnchor, 0) - 4;

		// Quadrant checkboxes
		w->widgets[WIDX_SCENERY_CHECK_QUARTER_N].top = GBBT(propertiesAnchor, 1) - 5 + 7 * 0;
		w->widgets[WIDX_SCENERY_CHECK_QUARTER_N].bottom = w->widgets[WIDX_SCENERY_CHECK_QUARTER_N].top + 13;
		w->widgets[WIDX_SCENERY_CHECK_QUARTER_E].top = GBBT(propertiesAnchor, 1) - 5 + 7 * 1;
		w->widgets[WIDX_SCENERY_CHECK_QUARTER_E].bottom = w->widgets[WIDX_SCENERY_CHECK_QUARTER_E].top + 13;
		w->widgets[WIDX_SCENERY_CHECK_QUARTER_S].top = GBBT(propertiesAnchor, 1) - 5 + 7 * 2;
		w->widgets[WIDX_SCENERY_CHECK_QUARTER_S].bottom = w->widgets[WIDX_SCENERY_CHECK_QUARTER_S].top + 13;
		w->widgets[WIDX_SCENERY_CHECK_QUARTER_W].top = GBBT(propertiesAnchor, 1) - 5 + 7 * 1;
		w->widgets[WIDX_SCENERY_CHECK_QUARTER_W].bottom = w->widgets[WIDX_SCENERY_CHECK_QUARTER_W].top + 13;
		// This gets the relative rotation, by subtracting the camera's rotation, and wrapping it between 0-3 inclusive
		bool N = (mapElement->type & MAP_ELEMENT_QUADRANT_MASK) == ((0 - get_current_rotation()) & 3) << 6;
		bool E = (mapElement->type & MAP_ELEMENT_QUADRANT_MASK) == ((1 - get_current_rotation()) & 3) << 6;
		bool S = (mapElement->type & MAP_ELEMENT_QUADRANT_MASK) == ((2 - get_current_rotation()) & 3) << 6;
		bool W = (mapElement->type & MAP_ELEMENT_QUADRANT_MASK) == ((3 - get_current_rotation()) & 3) << 6;
		widget_set_checkbox_value(w, WIDX_SCENERY_CHECK_QUARTER_N, N);
		widget_set_checkbox_value(w, WIDX_SCENERY_CHECK_QUARTER_E, E);
		widget_set_checkbox_value(w, WIDX_SCENERY_CHECK_QUARTER_S, S);
		widget_set_checkbox_value(w, WIDX_SCENERY_CHECK_QUARTER_W, W);

		// Collision checkboxes
		w->widgets[WIDX_SCENERY_CHECK_COLLISION_N].top = GBBT(propertiesAnchor, 2) + 5 + 7 * 0;
		w->widgets[WIDX_SCENERY_CHECK_COLLISION_N].bottom = w->widgets[WIDX_SCENERY_CHECK_COLLISION_N].top + 13;
		w->widgets[WIDX_SCENERY_CHECK_COLLISION_E].top = GBBT(propertiesAnchor, 2) + 5 + 7 * 1;
		w->widgets[WIDX_SCENERY_CHECK_COLLISION_E].bottom = w->widgets[WIDX_SCENERY_CHECK_COLLISION_E].top + 13;
		w->widgets[WIDX_SCENERY_CHECK_COLLISION_S].top = GBBT(propertiesAnchor, 2) + 5 + 7 * 2;
		w->widgets[WIDX_SCENERY_CHECK_COLLISION_S].bottom = w->widgets[WIDX_SCENERY_CHECK_COLLISION_S].top + 13;
		w->widgets[WIDX_SCENERY_CHECK_COLLISION_W].top = GBBT(propertiesAnchor, 2) + 5 + 7 * 1;
		w->widgets[WIDX_SCENERY_CHECK_COLLISION_W].bottom = w->widgets[WIDX_SCENERY_CHECK_COLLISION_W].top + 13;
		N = mapElement->flags & (1 << ((2 - get_current_rotation()) & 3));
		E = mapElement->flags & (1 << ((3 - get_current_rotation()) & 3));
		S = mapElement->flags & (1 << ((0 - get_current_rotation()) & 3));
		W = mapElement->flags & (1 << ((1 - get_current_rotation()) & 3));
		widget_set_checkbox_value(w, WIDX_SCENERY_CHECK_COLLISION_N, N);
		widget_set_checkbox_value(w, WIDX_SCENERY_CHECK_COLLISION_E, E);
		widget_set_checkbox_value(w, WIDX_SCENERY_CHECK_COLLISION_S, S);
		widget_set_checkbox_value(w, WIDX_SCENERY_CHECK_COLLISION_W, W);
		break;
	}
	case TILE_INSPECTOR_PAGE_ENTRANCE:
		w->widgets[WIDX_ENTRANCE_SPINNER_HEIGHT].top = GBBT(propertiesAnchor, 0) + 3;
		w->widgets[WIDX_ENTRANCE_SPINNER_HEIGHT].bottom = GBBB(propertiesAnchor, 0) - 3;
		w->widgets[WIDX_ENTRANCE_SPINNER_HEIGHT_INCREASE].top = GBBT(propertiesAnchor, 0) + 4;
		w->widgets[WIDX_ENTRANCE_SPINNER_HEIGHT_INCREASE].bottom = GBBT(propertiesAnchor, 0) + 8;
		w->widgets[WIDX_ENTRANCE_SPINNER_HEIGHT_DECREASE].top = GBBB(propertiesAnchor, 0) - 8;
		w->widgets[WIDX_ENTRANCE_SPINNER_HEIGHT_DECREASE].bottom = GBBB(propertiesAnchor, 0) - 4;
		break;
	case TILE_INSPECTOR_PAGE_WALL:
		w->widgets[WIDX_WALL_SPINNER_HEIGHT].top = GBBT(propertiesAnchor, 0) + 3;
		w->widgets[WIDX_WALL_SPINNER_HEIGHT].bottom = GBBB(propertiesAnchor, 0) - 3;
		w->widgets[WIDX_WALL_SPINNER_HEIGHT_INCREASE].top = GBBT(propertiesAnchor, 0) + 4;
		w->widgets[WIDX_WALL_SPINNER_HEIGHT_INCREASE].bottom = GBBT(propertiesAnchor, 0) + 8;
		w->widgets[WIDX_WALL_SPINNER_HEIGHT_DECREASE].top = GBBB(propertiesAnchor, 0) - 8;
		w->widgets[WIDX_WALL_SPINNER_HEIGHT_DECREASE].bottom = GBBB(propertiesAnchor, 0) - 4;
		w->widgets[WIDX_WALL_DROPDOWN_SLOPE].top = GBBT(propertiesAnchor, 1) + 3;
		w->widgets[WIDX_WALL_DROPDOWN_SLOPE].bottom = GBBB(propertiesAnchor, 1) - 3;
		w->widgets[WIDX_WALL_DROPDOWN_SLOPE].text = WallSlopeStringIds[(mapElement->type & 0xC0) >> 6];
		w->widgets[WIDX_WALL_DROPDOWN_SLOPE_BUTTON].top = GBBT(propertiesAnchor, 1) + 4;
		w->widgets[WIDX_WALL_DROPDOWN_SLOPE_BUTTON].bottom = GBBB(propertiesAnchor, 1) - 4;
		break;
	case TILE_INSPECTOR_PAGE_LARGE_SCENERY:
		w->widgets[WIDX_LARGE_SCENERY_SPINNER_HEIGHT].top = GBBT(propertiesAnchor, 0) + 3;
		w->widgets[WIDX_LARGE_SCENERY_SPINNER_HEIGHT].bottom = GBBB(propertiesAnchor, 0) - 3;
		w->widgets[WIDX_LARGE_SCENERY_SPINNER_HEIGHT_INCREASE].top = GBBT(propertiesAnchor, 0) + 4;
		w->widgets[WIDX_LARGE_SCENERY_SPINNER_HEIGHT_INCREASE].bottom = GBBT(propertiesAnchor, 0) + 8;
		w->widgets[WIDX_LARGE_SCENERY_SPINNER_HEIGHT_DECREASE].top = GBBB(propertiesAnchor, 0) - 8;
		w->widgets[WIDX_LARGE_SCENERY_SPINNER_HEIGHT_DECREASE].bottom = GBBB(propertiesAnchor, 0) - 4;
		break;
	case TILE_INSPECTOR_PAGE_BANNER:
		w->widgets[WIDX_BANNER_SPINNER_HEIGHT].top = GBBT(propertiesAnchor, 0) + 3;
		w->widgets[WIDX_BANNER_SPINNER_HEIGHT].bottom = GBBB(propertiesAnchor, 0) - 3;
		w->widgets[WIDX_BANNER_SPINNER_HEIGHT_INCREASE].top = GBBT(propertiesAnchor, 0) + 4;
		w->widgets[WIDX_BANNER_SPINNER_HEIGHT_INCREASE].bottom = GBBT(propertiesAnchor, 0) + 8;
		w->widgets[WIDX_BANNER_SPINNER_HEIGHT_DECREASE].top = GBBB(propertiesAnchor, 0) - 8;
		w->widgets[WIDX_BANNER_SPINNER_HEIGHT_DECREASE].bottom = GBBB(propertiesAnchor, 0) - 4;
		widget_set_checkbox_value(w, WIDX_BANNER_CHECK_BLOCK_NE, !(mapElement->properties.banner.flags & (1 << ((0 - get_current_rotation()) & 3))));
		widget_set_checkbox_value(w, WIDX_BANNER_CHECK_BLOCK_SE, !(mapElement->properties.banner.flags & (1 << ((1 - get_current_rotation()) & 3))));
		widget_set_checkbox_value(w, WIDX_BANNER_CHECK_BLOCK_SW, !(mapElement->properties.banner.flags & (1 << ((2 - get_current_rotation()) & 3))));
		widget_set_checkbox_value(w, WIDX_BANNER_CHECK_BLOCK_NW, !(mapElement->properties.banner.flags & (1 << ((3 - get_current_rotation()) & 3))));
		break;
	case TILE_INSPECTOR_PAGE_CORRUPT:
		w->widgets[WIDX_CORRUPT_SPINNER_HEIGHT].top = GBBT(propertiesAnchor, 0) + 3;
		w->widgets[WIDX_CORRUPT_SPINNER_HEIGHT].bottom = GBBB(propertiesAnchor, 0) - 3;
		w->widgets[WIDX_CORRUPT_SPINNER_HEIGHT_INCREASE].top = GBBT(propertiesAnchor, 0) + 4;
		w->widgets[WIDX_CORRUPT_SPINNER_HEIGHT_INCREASE].bottom = GBBT(propertiesAnchor, 0) + 8;
		w->widgets[WIDX_CORRUPT_SPINNER_HEIGHT_DECREASE].top = GBBB(propertiesAnchor, 0) - 8;
		w->widgets[WIDX_CORRUPT_SPINNER_HEIGHT_DECREASE].bottom = GBBB(propertiesAnchor, 0) - 4;
		w->widgets[WIDX_CORRUPT_BUTTON_CLAMP].top = GBBT(propertiesAnchor, 1);
		w->widgets[WIDX_CORRUPT_BUTTON_CLAMP].bottom = GBBB(propertiesAnchor, 1);
		break;
	}
}

static void window_tile_inspector_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);

	// Set medium font size
	gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;

	// Draw column headers
	rct_widget *widget;
	if ((widget = &w->widgets[WIDX_COLUMN_TYPE])->type != WWT_EMPTY) {
		gfx_draw_string_left_clipped(dpi, STR_TILE_INSPECTOR_ELEMENT_TYPE, gCommonFormatArgs, w->colours[1], w->x + widget->left + 1, w->y + widget->top + 1, widget->right - widget->left);
	}
	if ((widget = &w->widgets[WIDX_COLUMN_BASEHEIGHT])->type != WWT_EMPTY) {
		gfx_draw_string_left_clipped(dpi, STR_TILE_INSPECTOR_BASE_HEIGHT_SHORT, gCommonFormatArgs, w->colours[1], w->x + widget->left + 1, w->y + widget->top + 1, widget->right - widget->left);
	}
	if ((widget = &w->widgets[WIDX_COLUMN_CLEARANCEHEIGHT])->type != WWT_EMPTY) {
		gfx_draw_string_left_clipped(dpi, STR_TILE_INSPECTOR_CLEARANGE_HEIGHT_SHORT, gCommonFormatArgs, w->colours[1], w->x + widget->left + 1, w->y + widget->top + 1, widget->right - widget->left);
	}
	if ((widget = &w->widgets[WIDX_COLUMN_GHOSTFLAG])->type != WWT_EMPTY) {
		gfx_draw_string_left_clipped(dpi, STR_TILE_INSPECTOR_FLAG_GHOST_SHORT, gCommonFormatArgs, w->colours[1], w->x + widget->left + 1, w->y + widget->top + 1, widget->right - widget->left);
	}
	if ((widget = &w->widgets[WIDX_COLUMN_BROKENFLAG])->type != WWT_EMPTY) {
		gfx_draw_string_left_clipped(dpi, STR_TILE_INSPECTOR_FLAG_BROKEN_SHORT, gCommonFormatArgs, w->colours[1], w->x + widget->left + 1, w->y + widget->top + 1, widget->right - widget->left);
	}
	if ((widget = &w->widgets[WIDX_COLUMN_LASTFLAG])->type != WWT_EMPTY) {
		gfx_draw_string_left_clipped(dpi, STR_TILE_INSPECTOR_FLAG_LAST_SHORT, gCommonFormatArgs, w->colours[1], w->x + widget->left + 1, w->y + widget->top + 1, widget->right - widget->left);
	}

	// Draw coordinates
	gfx_draw_string(dpi, "X:", COLOUR_DARK_GREEN, w->x + 6, w->y + 24);
	gfx_draw_string(dpi, "Y:", COLOUR_DARK_GREEN, w->x + 64, w->y + 24);
	if (windowTileInspectorTileSelected) {
		gfx_draw_string_right(dpi, STR_FORMAT_INTEGER, &windowTileInspectorTileX, COLOUR_DARK_GREEN, w->x + 48, w->y + 24);
		gfx_draw_string_right(dpi, STR_FORMAT_INTEGER, &windowTileInspectorTileY, COLOUR_DARK_GREEN, w->x + 105, w->y + 24);
	}
	else {
		gfx_draw_string(dpi, "-", COLOUR_DARK_GREEN, w->x + 48 - 7, w->y + 24);
		gfx_draw_string(dpi, "-", COLOUR_DARK_GREEN, w->x + 105 - 7, w->y + 24);
	}

	if (w->selected_list_item != -1) {
		// X and Y of first element in detail box
		sint32 x = w->x + w->widgets[WIDX_GROUPBOX_DETAILS].left + 7;
		sint32 y = w->y + w->widgets[WIDX_GROUPBOX_DETAILS].top + 14;

		// Get map element
		rct_map_element *const mapElement = window_tile_inspector_get_selected_element(w);

		switch (w->page) {
		case TILE_INSPECTOR_PAGE_SURFACE: {
			// Details
			// Terrain texture name
			rct_string_id terrainNameId = TerrainTypeStringIds[map_element_get_terrain(mapElement)];
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_SURFACE_TERAIN, &terrainNameId, COLOUR_DARK_GREEN, x, y);

			// Edge texture name
			sint32 idx = map_element_get_terrain_edge(mapElement);
			openrct2_assert(idx < countof(TerrainEdgeTypeStringIds), "Tried accessing invalid entry %d in terrainEdgeTypeStringIds", idx);
			rct_string_id terrainEdgeNameId = TerrainEdgeTypeStringIds[map_element_get_terrain_edge(mapElement)];
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_SURFACE_EDGE, &terrainEdgeNameId, COLOUR_DARK_GREEN, x, y + 11);

			// Land ownership
			rct_string_id landOwnership;
			if (mapElement->properties.surface.ownership & OWNERSHIP_OWNED) landOwnership = STR_LAND_OWNED;
			else if (mapElement->properties.surface.ownership & OWNERSHIP_AVAILABLE) landOwnership = STR_LAND_SALE;
			else if (mapElement->properties.surface.ownership & OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED) landOwnership = STR_CONSTRUCTION_RIGHTS_OWNED;
			else if (mapElement->properties.surface.ownership & OWNERSHIP_CONSTRUCTION_RIGHTS_AVAILABLE) landOwnership = STR_CONSTRUCTION_RIGHTS_SALE;
			else landOwnership = STR_TILE_INSPECTOR_LAND_NOT_OWNED_AND_NOT_AVAILABLE;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_SURFACE_OWNERSHIP, &landOwnership, COLOUR_DARK_GREEN, x, y + 22);

			// Water level
			sint32 waterLevel = mapElement->properties.surface.terrain & MAP_ELEMENT_WATER_HEIGHT_MASK;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_SURFACE_WATER_LEVEL, &waterLevel, COLOUR_DARK_GREEN, x, y + 33);

			// Properties
			// Raise / lower label
			x = w->x + w->widgets[WIDX_GROUPBOX_DETAILS].left + 7;
			y = w->y + w->widgets[WIDX_SURFACE_SPINNER_HEIGHT].top;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_BASE_HEIGHT_FULL, NULL, COLOUR_DARK_GREEN, x, y);

			// Current base height
			x = w->x + w->widgets[WIDX_SURFACE_SPINNER_HEIGHT].left + 3;
			sint32 baseHeight = mapElement->base_height;
			gfx_draw_string_left(dpi, STR_FORMAT_INTEGER, &baseHeight, COLOUR_DARK_GREEN, x, y);

			// Raised corners
			x = w->x + w->widgets[WIDX_GROUPBOX_DETAILS].left + 7;
			y = w->y + w->widgets[WIDX_SURFACE_CHECK_CORNER_E].top;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_SURFACE_CORNERS, NULL, COLOUR_DARK_GREEN, x, y);
			break;
		}

		case TILE_INSPECTOR_PAGE_PATH: {
			// Details
			// Path name
			rct_string_id pathNameId = get_footpath_entry(footpath_element_get_type(mapElement))->string_idx;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_PATH_NAME, &pathNameId, COLOUR_DARK_GREEN, x, y);

			// Path addition
			if (footpath_element_has_path_scenery(mapElement)) {
				const uint8 pathAdditionType = footpath_element_get_path_scenery_index(mapElement);
				rct_string_id additionNameId = get_footpath_item_entry(pathAdditionType)->name;
				gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_PATH_ADDITIONS, &additionNameId, COLOUR_DARK_GREEN, x, y + 11);
			}
			else
				gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_PATH_ADDITIONS_NONE, NULL, COLOUR_DARK_GREEN, x, y + 11);

			// Properties
			// Raise / lower label
			x = w->x + w->widgets[WIDX_GROUPBOX_DETAILS].left + 7;
			y = w->y + w->widgets[WIDX_PATH_SPINNER_HEIGHT].top;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_BASE_HEIGHT_FULL, NULL, COLOUR_DARK_GREEN, x, y);

			// Current base height
			x = w->x + w->widgets[WIDX_PATH_SPINNER_HEIGHT].left + 3;
			sint32 baseHeight = mapElement->base_height;
			gfx_draw_string_left(dpi, STR_FORMAT_INTEGER, &baseHeight, COLOUR_DARK_GREEN, x, y);

			// Path connections
			x = w->x + w->widgets[WIDX_GROUPBOX_DETAILS].left + 7;
			y = w->y + w->widgets[WIDX_PATH_CHECK_EDGE_W].top;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_PATH_CONECTED_EDGES, NULL, COLOUR_DARK_GREEN, x, y);
			break;
		}

		case TILE_INSPECTOR_PAGE_TRACK: {
			// Details
			// Ride
			sint16 rideId = mapElement->properties.track.ride_index;
			rct_ride *ride = get_ride(rideId);
			rct_string_id rideType = RideNaming[ride->type].name;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_TRACK_RIDE_TYPE, &rideType, COLOUR_DARK_GREEN, x, y);
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_TRACK_RIDE_ID, &rideId, COLOUR_DARK_GREEN, x, y + 11);
			set_format_arg(0, rct_string_id, ride->name);
			set_format_arg(0 + sizeof(rct_string_id), uint32, ride->name_arguments);
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_TRACK_RIDE_NAME, gCommonFormatArgs, COLOUR_DARK_GREEN, x, y + 22);
			// Track
			sint16 trackType = mapElement->properties.track.type;
			sint16 sequenceNumber = mapElement->properties.track.sequence & 0x0F;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_TRACK_PIECE_ID, &trackType, COLOUR_DARK_GREEN, x, y + 33);
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_TRACK_SEQUENCE, &sequenceNumber, COLOUR_DARK_GREEN, x, y + 44);

			// Properties
			// Raise / lower label
			y = w->y + w->widgets[WIDX_TRACK_SPINNER_HEIGHT].top;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_BASE_HEIGHT_FULL, NULL, COLOUR_DARK_GREEN, x, y);

			// Current base height
			x = w->x + w->widgets[WIDX_TRACK_SPINNER_HEIGHT].left + 3;
			sint32 baseHeight = mapElement->base_height;
			gfx_draw_string_left(dpi, STR_FORMAT_INTEGER, &baseHeight, COLOUR_DARK_GREEN, x, y);
			break;
		}

		case TILE_INSPECTOR_PAGE_SCENERY: {
			// Details
			// Age
			sint16 age = mapElement->properties.scenery.age;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_SCENERY_AGE, &age, COLOUR_DARK_GREEN, x, y);

			// Quadrant value
			if (!(get_small_scenery_entry(mapElement->properties.scenery.type)->small_scenery.flags & SMALL_SCENERY_FLAG_FULL_TILE)) {
				sint16 quadrant = (mapElement->type & MAP_ELEMENT_QUADRANT_MASK) >> 6;
				static rct_string_id quadrant_string_idx[] = {
					STR_TILE_INSPECTOR_SCENERY_QUADRANT_SW,
					STR_TILE_INSPECTOR_SCENERY_QUADRANT_NW,
					STR_TILE_INSPECTOR_SCENERY_QUADRANT_NE,
					STR_TILE_INSPECTOR_SCENERY_QUADRANT_SE
				};
				gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_SCENERY_QUADRANT, &quadrant_string_idx[quadrant], COLOUR_DARK_GREEN, x, y + 11);
			}

			// Scenery ID
			sint16 idx = mapElement->properties.scenery.type;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_SCENERY_ENTRY_IDX, &idx, COLOUR_DARK_GREEN, x, y + 22);

			// Properties
			// Raise / Lower
			y = w->y + w->widgets[WIDX_SCENERY_SPINNER_HEIGHT].top;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_BASE_HEIGHT_FULL, NULL, COLOUR_DARK_GREEN, x, y);

			// Current base height
			x = w->x + w->widgets[WIDX_SCENERY_SPINNER_HEIGHT].left + 3;
			sint32 baseHeight = mapElement->base_height;
			gfx_draw_string_left(dpi, STR_FORMAT_INTEGER, &baseHeight, COLOUR_DARK_GREEN, x, y);

			// Quarter tile
			x = w->x + w->widgets[WIDX_GROUPBOX_DETAILS].left + 7;
			y = w->y + w->widgets[WIDX_SCENERY_CHECK_QUARTER_E].top;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_SCENERY_QUADRANT_LABEL, NULL, COLOUR_DARK_GREEN, x, y);

			// Collision
			y = w->y + w->widgets[WIDX_SCENERY_CHECK_COLLISION_E].top;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_COLLISSION, NULL, COLOUR_DARK_GREEN, x, y);
			break;
		}

		case TILE_INSPECTOR_PAGE_ENTRANCE: {
			// Details
			// Entrance type
			rct_string_id entranceType = EntranceTypeStringIds[mapElement->properties.entrance.type];
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_ENTRANCE_TYPE, &entranceType, COLOUR_DARK_GREEN, x, y);

			if (mapElement->properties.entrance.type == ENTRANCE_TYPE_PARK_ENTRANCE) {
				// Park entrance ID
				sint32 middleX = windowTileInspectorTileX << 5;
				sint32 middleY = windowTileInspectorTileY << 5;
				// TODO: Make this work with Left/Right park entrance parts
				sint16 parkEntranceIndex = park_entrance_get_index(middleX, middleY, mapElement->base_height * 8);
				gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_ENTRANCE_ENTRANCE_ID, &parkEntranceIndex, COLOUR_DARK_GREEN, x, y + 11);
			}
			else {
				sint16 rideEntranceIndex = (mapElement->properties.entrance.index & 0x30) >> 4;
				if (mapElement->properties.entrance.type == ENTRANCE_TYPE_RIDE_ENTRANCE) {
					// Ride entrance ID
					gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_ENTRANCE_ENTRANCE_ID, &rideEntranceIndex, COLOUR_DARK_GREEN, x, y + 11);
				}
				else {
					// Ride exit ID
					gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_ENTRANCE_EXIT_ID, &rideEntranceIndex, COLOUR_DARK_GREEN, x, y + 11);
				}
			}

			if (mapElement->properties.entrance.type == ENTRANCE_TYPE_PARK_ENTRANCE) {
				// Entrance part
				rct_string_id entrancePart = ParkEntrancePartStringIds[mapElement->properties.entrance.index & 0x0F];
				gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_ENTRANCE_PART, &entrancePart, COLOUR_DARK_GREEN, x, y + 22);
			}
			else {
				// Ride ID
				sint16 rideId = mapElement->properties.entrance.ride_index;
				gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_ENTRANCE_RIDE_ID, &rideId, COLOUR_DARK_GREEN, x, y + 22);
			}

			// Properties
			// Raise / Lower
			y = w->y + w->widgets[WIDX_ENTRANCE_SPINNER_HEIGHT].top;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_BASE_HEIGHT_FULL, NULL, COLOUR_DARK_GREEN, x, y);

			// Current base height
			x = w->x + w->widgets[WIDX_ENTRANCE_SPINNER_HEIGHT].left + 3;
			sint32 baseHeight = mapElement->base_height;
			gfx_draw_string_left(dpi, STR_FORMAT_INTEGER, &baseHeight, COLOUR_DARK_GREEN, x, y);
			break;
		}

		case TILE_INSPECTOR_PAGE_WALL: {
			// Details
			// Type
			sint16 wallType = mapElement->properties.wall.type;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_WALL_TYPE, &wallType, COLOUR_DARK_GREEN, x, y);

			// Banner info
			rct_wall_scenery_entry wallEntry = get_wall_entry(wallType)->wall;
			if (wallEntry.flags & WALL_SCENERY_IS_BANNER) {
				gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_ENTRY_BANNER_TEXT, &gBanners[mapElement->properties.wall.banner_index].string_idx, COLOUR_DARK_GREEN, x, y + 11);
			}
			else {
				gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_ENTRY_BANNER_NONE, NULL, COLOUR_DARK_GREEN, x, y + 11);
			}

			// Properties
			// Raise / lower label
			y = w->y + w->widgets[WIDX_WALL_SPINNER_HEIGHT].top;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_BASE_HEIGHT_FULL, NULL, COLOUR_DARK_GREEN, x, y);

			// Current base height
			x = w->x + w->widgets[WIDX_WALL_SPINNER_HEIGHT].left + 3;
			sint32 baseHeight = mapElement->base_height;
			gfx_draw_string_left(dpi, STR_FORMAT_INTEGER, &baseHeight, COLOUR_DARK_GREEN, x, y);

			// Slope label
			x = w->x + w->widgets[WIDX_GROUPBOX_DETAILS].left + 7;
			y = w->y + w->widgets[WIDX_WALL_DROPDOWN_SLOPE].top;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_WALL_SLOPE, NULL, COLOUR_DARK_GREEN, x, y);
			break;
		}

		case TILE_INSPECTOR_PAGE_LARGE_SCENERY:
		{
			// Details
			// Type
			sint16 largeSceneryType = mapElement->properties.scenerymultiple.type & 0x03FF;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_LARGE_SCENERY_TYPE, &largeSceneryType, COLOUR_DARK_GREEN, x, y);

			// Part ID
			sint16 pieceID = (mapElement->properties.scenerymultiple.type & 0xFC00) >> 10;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_LARGE_SCENERY_PIECE_ID, &pieceID, COLOUR_DARK_GREEN, x, y + 11);

			// Banner info
			rct_scenery_entry *largeSceneryEntry = get_large_scenery_entry(mapElement->properties.scenerymultiple.type & MAP_ELEMENT_LARGE_TYPE_MASK);
			if (largeSceneryEntry->large_scenery.var_11 != 0xFF) {
				const sint32 bannerIndex = (mapElement->type & 0xC0) |
					((mapElement->properties.scenerymultiple.colour[0] & 0xE0) >> 2) |
					((mapElement->properties.scenerymultiple.colour[1] & 0xE0) >> 5);
				//window_sign_open(bannerIndex);
				gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_ENTRY_BANNER_TEXT, &gBanners[bannerIndex].string_idx, COLOUR_DARK_GREEN, x, y + 22);
			}
			else {
				gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_ENTRY_BANNER_NONE, NULL, COLOUR_DARK_GREEN, x, y + 22);
			}

			// Properties
			// Raise / lower label
			y = w->y + w->widgets[WIDX_LARGE_SCENERY_SPINNER_HEIGHT].top;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_BASE_HEIGHT_FULL, NULL, COLOUR_DARK_GREEN, x, y);

			// Current base height
			x = w->x + w->widgets[WIDX_LARGE_SCENERY_SPINNER_HEIGHT].left + 3;
			sint32 baseHeight = mapElement->base_height;
			gfx_draw_string_left(dpi, STR_FORMAT_INTEGER, &baseHeight, COLOUR_DARK_GREEN, x, y);
			break;
		}

		case TILE_INSPECTOR_PAGE_BANNER:
		{
			// Details
			// Banner info
			const uint8 bannerIndex = mapElement->properties.banner.index;
			if (gBanners[bannerIndex].flags & BANNER_FLAG_NO_ENTRY) {
				rct_string_id noEntryStringIdx = STR_NO_ENTRY;
				gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_ENTRY_BANNER_TEXT, &noEntryStringIdx, COLOUR_DARK_GREEN, x, y);
			}
			else {
				gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_ENTRY_BANNER_TEXT, &gBanners[bannerIndex].string_idx, COLOUR_DARK_GREEN, x, y);
			}

			// Properties
			// Raise / lower label
			y = w->y + w->widgets[WIDX_BANNER_SPINNER_HEIGHT].top;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_BASE_HEIGHT_FULL, NULL, COLOUR_DARK_GREEN, x, y);

			// Current base height
			x = w->x + w->widgets[WIDX_BANNER_SPINNER_HEIGHT].left + 3;
			sint32 baseHeight = mapElement->base_height;
			gfx_draw_string_left(dpi, STR_FORMAT_INTEGER, &baseHeight, COLOUR_DARK_GREEN, x, y);

			// Blocked paths
			y += 28;
			x = w->x + w->widgets[WIDX_GROUPBOX_DETAILS].left + 7;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_BANNER_BLOCKED_PATHS, NULL, COLOUR_DARK_GREEN, x, y);
			break;
		}

		case TILE_INSPECTOR_PAGE_CORRUPT:
		{
			// Properties
			// Raise / lower label
			y = w->y + w->widgets[WIDX_CORRUPT_SPINNER_HEIGHT].top;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_BASE_HEIGHT_FULL, NULL, COLOUR_DARK_GREEN, x, y);

			// Current base height
			x = w->x + w->widgets[WIDX_CORRUPT_SPINNER_HEIGHT].left + 3;
			sint32 baseHeight = mapElement->base_height;
			gfx_draw_string_left(dpi, STR_FORMAT_INTEGER, &baseHeight, COLOUR_DARK_GREEN, x, y);
			break;
		}
		} // switch page
	}
}

static void window_tile_inspector_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex)
{
	sint32 y = LIST_ITEM_HEIGHT * (windowTileInspectorElementCount - 1);
	sint32 i = 0;
	char buffer[256];

	if (!windowTileInspectorTileSelected)
		return;

	rct_map_element *mapElement = map_get_first_element_at(windowTileInspectorTileX, windowTileInspectorTileY);

	gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
	do {
		sint32 type = map_element_get_type(mapElement);
		char *typeName;
		sint32 baseHeight = mapElement->base_height;
		sint32 clearanceHeight = mapElement->clearance_height;

		// Fill colour for current list element
		const sint32 listWidth = w->widgets[WIDX_LIST].right - w->widgets[WIDX_LIST].left;
		if (i == w->selected_list_item) // Currently selected element
			gfx_fill_rect(dpi, 0, y, listWidth, y + LIST_ITEM_HEIGHT - 1, ColourMapA[w->colours[1]].darker | 0x1000000);
		else if (i == windowTileInspectorHighlightedIndex) // Hovering
			gfx_fill_rect(dpi, 0, y, listWidth, y + LIST_ITEM_HEIGHT - 1, ColourMapA[w->colours[1]].mid_dark | 0x1000000);
		else if ((i & 1) != 0) // odd / even check
			gfx_fill_rect(dpi, 0, y, listWidth, y + LIST_ITEM_HEIGHT - 1, ColourMapA[w->colours[1]].lighter | 0x1000000);

		switch (type) {
		case MAP_ELEMENT_TYPE_SURFACE:
			typeName = "Surface";
			break;
		case MAP_ELEMENT_TYPE_PATH:
			typeName = footpath_element_is_queue(mapElement) ? "Queue" : "Footpath";
			break;
		case MAP_ELEMENT_TYPE_TRACK:
			typeName = "Track";
			break;
		case MAP_ELEMENT_TYPE_SCENERY:
			snprintf(
				buffer, sizeof(buffer),
				"Scenery (%s)",
				language_get_string(get_small_scenery_entry(mapElement->properties.scenery.type)->name)
			);
			typeName = buffer;
			break;
		case MAP_ELEMENT_TYPE_ENTRANCE:
			typeName = "Entrance";
			break;
		case MAP_ELEMENT_TYPE_WALL:
			snprintf(
				buffer, sizeof(buffer),
				"Wall (%s)",
				language_get_string(get_wall_entry(mapElement->properties.scenery.type)->name)
			);
			typeName = buffer;
			break;
		case MAP_ELEMENT_TYPE_SCENERY_MULTIPLE:
			typeName = "Scenery multiple";
			break;
		case MAP_ELEMENT_TYPE_BANNER:
			snprintf(
				buffer, sizeof(buffer),
				"Banner (%d)",
				mapElement->properties.banner.index
			);
			typeName = buffer;
			break;
		case MAP_ELEMENT_TYPE_CORRUPT:
			// fall-through
		default:
			snprintf(buffer, sizeof(buffer), "Unknown (type %d)", type);
			typeName = buffer;
		}

		// Undo relative scroll offset, but keep the 3 pixel padding
		sint32 x = -w->widgets[WIDX_LIST].left;
		const bool ghost = (mapElement->flags & MAP_ELEMENT_FLAG_GHOST) != 0;
		const bool broken = (mapElement->flags & MAP_ELEMENT_FLAG_BROKEN) != 0;
		const bool last = (mapElement->flags & MAP_ELEMENT_FLAG_LAST_TILE) != 0;
		gfx_clip_string(buffer, w->widgets[WIDX_COLUMN_TYPE].right - w->widgets[WIDX_COLUMN_TYPE].left - COL_X_TYPE);
		gfx_draw_string(dpi, typeName, COLOUR_DARK_GREEN, x + COL_X_TYPE + 3, y); // 3px padding
		gfx_draw_string_left(dpi, STR_FORMAT_INTEGER, &baseHeight, COLOUR_DARK_GREEN, x + COL_X_BH, y);
		gfx_draw_string_left(dpi, STR_FORMAT_INTEGER, &clearanceHeight, COLOUR_DARK_GREEN, x + COL_X_CH, y);
		if (ghost) gfx_draw_string(dpi, (char*)CheckBoxMarkString, w->colours[1], x + COL_X_GF, y);
		if (broken) gfx_draw_string(dpi, (char*)CheckBoxMarkString, w->colours[1], x + COL_X_BF, y);
		if (last) gfx_draw_string(dpi, (char*)CheckBoxMarkString, w->colours[1], x + COL_X_LF, y);

		y -= LIST_ITEM_HEIGHT;
		i++;
	} while (!map_element_is_last_for_tile(mapElement++));
}
