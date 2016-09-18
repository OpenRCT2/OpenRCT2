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

#include "../input.h"
#include "../interface/themes.h"
#include "../interface/viewport.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../ride/track.h"
#include "../sprites.h"
#include "../world/footpath.h"
#include "../world/map.h"
#include "../world/scenery.h"

static const rct_string_id TerrainTypes[] = {
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

static const rct_string_id TerrainEdgeTypes[] = {
	STR_TILE_INSPECTOR_TERRAIN_EDGE_ROCK,
	STR_TILE_INSPECTOR_TERRAIN_EDGE_WOOD_RED,
	STR_TILE_INSPECTOR_TERRAIN_EDGE_WOOD_BLACK,
	STR_TILE_INSPECTOR_TERRAIN_EDGE_ICE,

};

static const rct_string_id EntranceTypes[] = {
	STR_TILE_INSPECTOR_ENTRANCE_TYPE_RIDE_ENTRANCE,
	STR_TILE_INSPECTOR_ENTRANCE_TYPE_RIDE_EXIT,
	STR_TILE_INSPECTOR_ENTRANCE_TYPE_PARK_ENTRANC,
};

enum WINDOW_TILE_INSPECTOR_PAGES {
	PAGE_DEFAULT,
	PAGE_SURFACE,
	PAGE_PATH,
	PAGE_TRACK,
	PAGE_SCENERY,
	PAGE_ENTRANCE,
	PAGE_FENCE,
	PAGE_LARGE_SCENERY,
	PAGE_BANNER,
	PAGE_CORRUPT
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

	// Fence
	WIDX_FENCE_SPINNER_HEIGHT = PAGE_WIDGETS,
	WIDX_FENCE_SPINNER_HEIGHT_INCREASE,
	WIDX_FENCE_SPINNER_HEIGHT_DECREASE,

	// Large

	// Banner

	// Corrupt
};

#define WW 400
#define WH 280
#define MIN_WW WW
#define MAX_WW WW
#define MIN_WH 240
#define MAX_WH 800

#define BW (WW - 5)		// Button's right side
#define BX (BW - 23)	// Button's left side
#define BY 17			// Button's Top
#define BH (BY + 23)	// Button's Bottom
#define BS 24

#define PADDING_BOTTOM 15
#define GROUPBOX_PADDING 6
#define LIST_ITEM_HEIGHT 11

// Column offsets for the table headers
#define COL_X_TYPE 3					// Type
#define COL_X_BH   (COL_X_TYPE + 300)	// Base height
#define COL_X_CH   (COL_X_BH + 20)		// Clearance height
#define COL_X_GF   (COL_X_CH + 20)		// Ghost flag
#define COL_X_BF   (COL_X_GF + 12)		// Broken flag
#define COL_X_LF   (COL_X_BF + 12)		// Last for tile flag

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
	/* Column headers */																																						\
	{ WWT_13,			1, COL_X_TYPE,	COL_X_BH - 1, 	42,		42 + 13,	STR_NONE,	STR_NONE },													/* Type */					\
	{ WWT_13,			1, COL_X_BH,	COL_X_CH - 1, 	42,		42 + 13,	STR_NONE,	STR_TILE_INSPECTOR_BASE_HEIGHT },							/* Base height */			\
	{ WWT_13,			1, COL_X_CH,	COL_X_GF - 1, 	42,		42 + 13,	STR_NONE,	STR_TILE_INSPECTOR_CLEARANCE_HEIGHT },						/* Clearance height */		\
	{ WWT_13,			1, COL_X_GF,	COL_X_BF - 1, 	42,		42 + 13,	STR_NONE,	STR_TILE_INSPECTOR_FLAG_GHOST },							/* Ghost flag */			\
	{ WWT_13,			1, COL_X_BF,	COL_X_LF - 1, 	42,		42 + 13,	STR_NONE,	STR_TILE_INSPECTOR_FLAG_BROKEN },							/* Broken flag */			\
	{ WWT_13,			1, COL_X_LF,	WW - 3,			42,		42 + 13,	STR_NONE,	STR_TILE_INSPECTOR_FLAG_LAST },								/* Last of tile flag */		\
	{ WWT_GROUPBOX,		1, 6,			WW - 6,			-1,		-1,			STR_NONE,									STR_NONE },					/* Details group box */		\
	{ WWT_GROUPBOX,		1, 6,			WW - 6,			-1,		-1,			STR_TILE_INSPECTOR_GROUPBOX_PROPERTIES,		STR_NONE }					/* Properties group box */

static rct_widget window_tile_inspector_widgets[] = {
	MAIN_TILE_INSPECTOR_WIDGETS,
	{ WIDGETS_END },
};

// Calculates the .left, .right, .top and .bottom for buttons in a group box
#define GBBL(col)				(12 + col * 115)
#define GBBR(col)				(GBBL(col) + 110)
#define GBBT(GROUPTOP, row)		((GROUPTOP) + 14 + row * 21)
#define GBBB(GROUPTOP, row)		(GBBT((GROUPTOP), row) + 17)
#define GBB(GROUPTOP, col, row)	GBBL(col), GBBR(col), GBBT((GROUPTOP), row), GBBB((GROUPTOP), row)
#define GBBF(GROUPTOP, col, row)GBBL(col), WW - 10, GBBT((GROUPTOP), row), GBBB((GROUPTOP), row) // Full width

// Same, but for spinners and their increase/decrease buttons
#define GBS(GBT, col, row)	GBBL(col), GBBR(col), GBBT(GBT, row) + 3, GBBB(GBT, row) - 3
#define GBSI(GBT, col, row)	GBBR(col) - 11, GBBR(col) - 1, GBBT(GBT, row) + 4, GBBT(GBT, row) + 8
#define GBSD(GBT, col, row)	GBBR(col) - 11, GBBR(col) - 1, GBBB(GBT, row) - 8, GBBB(GBT, row) - 4

// Checkbox - given topleft corner
#define CHK(x, y) x, x + 13, y + 2, y + 15

// Group boxes .top and .bottom for a given window height offsets from the bottom
#define SUR_GBPB PADDING_BOTTOM					// Surface group box properties bottom
#define SUR_GBPT (SUR_GBPB + 16 + 4 * 21)		// Surface group box properties top
#define SUR_GBDB (SUR_GBPT + GROUPBOX_PADDING)	// Surface group box details bottom
#define SUR_GBDT (SUR_GBDB + 20 + 4 * 11)		// Surface group box details top
static rct_widget window_tile_inspector_widgets_surface[] = {
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
#define PAT_GBPT (PAT_GBPB + 16 + 3 * 21)		// Path group box properties top
#define PAT_GBDB (PAT_GBPT + GROUPBOX_PADDING)	// Path group box info bottom
#define PAT_GBDT (PAT_GBDB + 20 + 2 * 11)		// Path group box info top
static rct_widget window_tile_inspector_widgets_path[] = {
	MAIN_TILE_INSPECTOR_WIDGETS,
	{ WWT_SPINNER,			1,	GBS(WH - PAT_GBPT, 1, 0),				STR_NONE,					STR_NONE }, // WIDX_PATH_SPINNER_HEIGHT
	{ WWT_DROPDOWN_BUTTON,  1,	GBSI(WH - PAT_GBPT, 1, 0),				STR_NUMERIC_UP,				STR_NONE }, // WIDX_PATH_SPINNER_HEIGHT_INCREASE
	{ WWT_DROPDOWN_BUTTON,  1,	GBSD(WH - PAT_GBPT, 1, 0),				STR_NUMERIC_DOWN,			STR_NONE }, // WIDX_PATH_SPINNER_HEIGHT_DECREASE
	{ WWT_CHECKBOX,			1,	CHK(GBBL(1) + 14 * 3, GBBT(WH - PAT_GBPT, 1) + 7 * 1),	STR_NONE,	STR_NONE }, // WIDX_PATH_CHECK_EDGE_NE
	{ WWT_CHECKBOX,			1,	CHK(GBBL(1) + 14 * 4, GBBT(WH - PAT_GBPT, 1) + 7 * 2),	STR_NONE,	STR_NONE }, // WIDX_PATH_CHECK_EDGE_E
	{ WWT_CHECKBOX,			1,	CHK(GBBL(1) + 14 * 3, GBBT(WH - PAT_GBPT, 1) + 7 * 3),	STR_NONE,	STR_NONE }, // WIDX_PATH_CHECK_EDGE_SE
	{ WWT_CHECKBOX,			1,	CHK(GBBL(1) + 14 * 2, GBBT(WH - PAT_GBPT, 1) + 7 * 4),	STR_NONE,	STR_NONE }, // WIDX_PATH_CHECK_EDGE_S
	{ WWT_CHECKBOX,			1,	CHK(GBBL(1) + 14 * 1, GBBT(WH - PAT_GBPT, 1) + 7 * 3),	STR_NONE,	STR_NONE }, // WIDX_PATH_CHECK_EDGE_SW
	{ WWT_CHECKBOX,			1,	CHK(GBBL(1) + 14 * 0, GBBT(WH - PAT_GBPT, 1) + 7 * 2),	STR_NONE,	STR_NONE }, // WIDX_PATH_CHECK_EDGE_W
	{ WWT_CHECKBOX,			1,	CHK(GBBL(1) + 14 * 1, GBBT(WH - PAT_GBPT, 1) + 7 * 1),	STR_NONE,	STR_NONE }, // WIDX_PATH_CHECK_EDGE_NW
	{ WWT_CHECKBOX,			1,	CHK(GBBL(1) + 14 * 2, GBBT(WH - PAT_GBPT, 1) + 7 * 0),	STR_NONE,	STR_NONE }, // WIDX_PATH_CHECK_EDGE_N
	{ WIDGETS_END },
};

#define TRA_GBPB PADDING_BOTTOM					// Track group box properties bottom
#define TRA_GBPT (TRA_GBPB + 16 + 3 * 21)		// Track group box properties top
#define TRA_GBDB (TRA_GBPT + GROUPBOX_PADDING)	// Track group box info bottom
#define TRA_GBDT (TRA_GBDB + 20 + 5 * 11)		// Track group box info top
static rct_widget window_tile_inspector_widgets_track[] = {
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
static rct_widget window_tile_inspector_widgets_scenery[] = {
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
#define ENT_GBPT (ENT_GBPB + 16 + 0 * 21)		// Entrance group box properties top
#define ENT_GBDB (ENT_GBPT + GROUPBOX_PADDING)	// Entrance group box info bottom
#define ENT_GBDT (ENT_GBDB + 20 + 0 * 11)		// Entrance group box info top
static rct_widget window_tile_inspector_widgets_entrance[] = {
	MAIN_TILE_INSPECTOR_WIDGETS,
	{ WIDGETS_END },
};

#define FEN_GBPB PADDING_BOTTOM					// Fence group box properties bottom
#define FEN_GBPT (FEN_GBPB + 16 + 1 * 21)		// Fence group box properties top
#define FEN_GBDB (FEN_GBPT + GROUPBOX_PADDING)	// Fence group box info bottom
#define FEN_GBDT (FEN_GBDB + 20 + 0 * 11)		// Fence group box info top
static rct_widget window_tile_inspector_widgets_fence[] = {
	MAIN_TILE_INSPECTOR_WIDGETS,
	{ WWT_SPINNER,			1,	GBS(WH - FEN_GBPT, 1, 0),	STR_NONE,										STR_NONE }, // WIDX_FENCE_SPINNER_HEIGHT
	{ WWT_DROPDOWN_BUTTON,  1,	GBSI(WH - FEN_GBPT, 1, 0),	STR_NUMERIC_UP,									STR_NONE }, // WIDX_FENCE_SPINNER_HEIGHT_INCREASE
	{ WWT_DROPDOWN_BUTTON,  1,	GBSD(WH - FEN_GBPT, 1, 0),	STR_NUMERIC_DOWN,								STR_NONE }, // WIDX_FENCE_SPINNER_HEIGHT_DECREASE
	{ WIDGETS_END },
};

#define LAR_GBPB PADDING_BOTTOM					// Large scenery group box properties bottom
#define LAR_GBPT (LAR_GBPB + 16 + 0 * 21)		// Large scenery group box properties top
#define LAR_GBDB (LAR_GBPT + GROUPBOX_PADDING)	// Large scenery group box info bottom
#define LAR_GBDT (LAR_GBDB + 20 + 0 * 11)		// Large scenery group box info top
static rct_widget window_tile_inspector_widgets_large_scenery[] = {
	MAIN_TILE_INSPECTOR_WIDGETS,
	{ WIDGETS_END },
};

#define BAN_GBPB PADDING_BOTTOM					// Banner group box properties bottom
#define BAN_GBPT (BAN_GBPB + 16 + 0 * 21)		// Banner group box properties top
#define BAN_GBDB (BAN_GBPT + GROUPBOX_PADDING)	// Banner group box info bottom
#define BAN_GBDT (BAN_GBDB + 20 + 0 * 11)		// Banner group box info top
static rct_widget window_tile_inspector_widgets_banner[] = {
	MAIN_TILE_INSPECTOR_WIDGETS,
	{ WIDGETS_END },
};

#define COR_GBPB PADDING_BOTTOM					// Corrupt element group box properties bottom
#define COR_GBPT (COR_GBPB + 16 + 0 * 21)		// Corrupt element group box properties top
#define COR_GBDB (COR_GBPT + GROUPBOX_PADDING)	// Corrupt element group box info bottom
#define COR_GBDT (COR_GBDB + 20 + 0 * 11)		// Corrupt element group box info top
static rct_widget window_tile_inspector_widgets_corrupt[] = {
	MAIN_TILE_INSPECTOR_WIDGETS,
	{ WIDGETS_END },
};

static rct_widget *tile_inspector_widgets[] = {
	window_tile_inspector_widgets,
	window_tile_inspector_widgets_surface,
	window_tile_inspector_widgets_path,
	window_tile_inspector_widgets_track,
	window_tile_inspector_widgets_scenery,
	window_tile_inspector_widgets_entrance,
	window_tile_inspector_widgets_fence,
	window_tile_inspector_widgets_large_scenery,
	window_tile_inspector_widgets_banner,
	window_tile_inspector_widgets_corrupt,
};

static struct {
	// Offsets from the bottom of the window
	sint16 details_top_offset, details_bottom_offset;
	sint16 properties_top_offset, properties_bottom_offset;
	// String to be displayed in the details groupbox
	rct_string_id string_idx;
} page_group_box_settings[] = {
	{ SUR_GBDT, SUR_GBDB, SUR_GBPT, SUR_GBPB, STR_TILE_INSPECTOR_GROUPBOX_SURFACE_INFO },
	{ PAT_GBDT, PAT_GBDB, PAT_GBPT, PAT_GBPB, STR_TILE_INSPECTOR_GROUPBOX_PATH_INFO },
	{ TRA_GBDT, TRA_GBDB, TRA_GBPT, TRA_GBPB, STR_TILE_INSPECTOR_GROUPBOX_TRACK_INFO },
	{ SCE_GBDT, SCE_GBDB, SCE_GBPT, SCE_GBPB, STR_TILE_INSPECTOR_GROUPBOX_SCENERY_INFO },
	{ ENT_GBDT, ENT_GBDB, ENT_GBPT, ENT_GBPB, STR_TILE_INSPECTOR_GROUPBOX_ENTRANCE_INFO },
	{ FEN_GBDT, FEN_GBDB, FEN_GBPT, FEN_GBPB, STR_TILE_INSPECTOR_GROUPBOX_FENCE_INFO },
	{ LAR_GBDT, LAR_GBDB, LAR_GBPT, LAR_GBPB, STR_TILE_INSPECTOR_GROUPBOX_LARGE_SCENERY_INFO },
	{ BAN_GBDT, BAN_GBDB, BAN_GBPT, BAN_GBPB, STR_TILE_INSPECTOR_GROUPBOX_BANNER_INFO },
	{ COR_GBDT, COR_GBDB, COR_GBPT, COR_GBPB, STR_TILE_INSPECTOR_GROUPBOX_CORRUPT_INFO }
};

static sint16 window_tile_inspector_highlighted_index = -1;

static int window_tile_inspector_tile_x = -1;
static int window_tile_inspector_tile_y = -1;
static int window_tile_inspector_item_count = 0;
static bool window_tile_inspector_apply_to_all = false;

static void window_tile_inspector_mouseup(rct_window *w, int widgetIndex);
static void window_tile_inspector_resize(rct_window *w);
static void window_tile_inspector_update(rct_window *w);
static void window_tile_inspector_tool_update(rct_window* w, int widgetIndex, int x, int y);
static void window_tile_inspector_tool_down(rct_window* w, int widgetIndex, int x, int y);
static void window_tile_inspector_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height);
static void window_tile_inspector_scrollmousedown(rct_window *w, int scrollIndex, int x, int y);
static void window_tile_inspector_scrollmouseover(rct_window *w, int scrollIndex, int x, int y);
static void window_tile_inspector_invalidate(rct_window *w);
static void window_tile_inspector_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_tile_inspector_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex);
static void window_tile_inspector_set_page(rct_window *w, const int page);
static void window_tile_inspector_auto_set_buttons(rct_window *w);

static rct_window_event_list window_tile_inspector_events = {
	NULL,
	window_tile_inspector_mouseup,
	window_tile_inspector_resize,
	NULL,
	NULL,
	NULL,
	window_tile_inspector_update,
	NULL,
	NULL,
	window_tile_inspector_tool_update,
	window_tile_inspector_tool_down,
	NULL,
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

static uint64 window_tile_inspector_page_enabled_widgets[] = {
	(1ULL << WIDX_CLOSE) | (1ULL << WIDX_BUTTON_CORRUPT),
	(1ULL << WIDX_CLOSE) | (1ULL << WIDX_BUTTON_CORRUPT) | (1ULL << WIDX_BUTTON_REMOVE) | (1ULL << WIDX_BUTTON_ROTATE) | (1ULL << WIDX_SURFACE_SPINNER_HEIGHT_INCREASE) | (1ULL << WIDX_SURFACE_SPINNER_HEIGHT_DECREASE) | (1ULL << WIDX_SURFACE_BUTTON_REMOVE_FENCES) | (1ULL << WIDX_SURFACE_BUTTON_RESTORE_FENCES) | (1ULL << WIDX_SURFACE_CHECK_CORNER_N) | (1ULL << WIDX_SURFACE_CHECK_CORNER_E) | (1ULL << WIDX_SURFACE_CHECK_CORNER_S) | (1ULL << WIDX_SURFACE_CHECK_CORNER_W) | (1ULL << WIDX_SURFACE_CHECK_DIAGONAL),
	(1ULL << WIDX_CLOSE) | (1ULL << WIDX_BUTTON_CORRUPT) | (1ULL << WIDX_BUTTON_REMOVE) | (1ULL << WIDX_BUTTON_ROTATE) | (1ULL << WIDX_PATH_SPINNER_HEIGHT_INCREASE) | (1ULL << WIDX_PATH_SPINNER_HEIGHT_DECREASE) | (1ULL << WIDX_PATH_CHECK_EDGE_N) | (1ULL << WIDX_PATH_CHECK_EDGE_NE) | (1ULL << WIDX_PATH_CHECK_EDGE_E) | (1ULL << WIDX_PATH_CHECK_EDGE_SE) | (1ULL << WIDX_PATH_CHECK_EDGE_S) | (1ULL << WIDX_PATH_CHECK_EDGE_SW) | (1ULL << WIDX_PATH_CHECK_EDGE_W) | (1ULL << WIDX_PATH_CHECK_EDGE_NW),
	(1ULL << WIDX_CLOSE) | (1ULL << WIDX_BUTTON_CORRUPT) | (1ULL << WIDX_BUTTON_REMOVE) | (1ULL << WIDX_BUTTON_ROTATE) | (1ULL << WIDX_TRACK_CHECK_APPLY_TO_ALL) | (1ULL << WIDX_TRACK_SPINNER_HEIGHT_INCREASE) | (1ULL << WIDX_TRACK_SPINNER_HEIGHT_DECREASE) | (1ULL << WIDX_TRACK_CHECK_CHAIN_LIFT),
	(1ULL << WIDX_CLOSE) | (1ULL << WIDX_BUTTON_CORRUPT) | (1ULL << WIDX_BUTTON_REMOVE) | (1ULL << WIDX_BUTTON_ROTATE) | (1ULL << WIDX_SCENERY_SPINNER_HEIGHT_INCREASE) | (1ULL << WIDX_SCENERY_SPINNER_HEIGHT_DECREASE) | (1ULL << WIDX_SCENERY_CHECK_QUARTER_N) | (1ULL << WIDX_SCENERY_CHECK_QUARTER_E) | (1ULL << WIDX_SCENERY_CHECK_QUARTER_S) | (1ULL << WIDX_SCENERY_CHECK_QUARTER_W) | (1ULL << WIDX_SCENERY_CHECK_COLLISION_N) | (1ULL << WIDX_SCENERY_CHECK_COLLISION_E) | (1ULL << WIDX_SCENERY_CHECK_COLLISION_S) | (1ULL << WIDX_SCENERY_CHECK_COLLISION_W),
	(1ULL << WIDX_CLOSE) | (1ULL << WIDX_BUTTON_CORRUPT) | (1ULL << WIDX_BUTTON_REMOVE) | (1ULL << WIDX_BUTTON_ROTATE),
	(1ULL << WIDX_CLOSE) | (1ULL << WIDX_BUTTON_CORRUPT) | (1ULL << WIDX_BUTTON_REMOVE) | (1ULL << WIDX_BUTTON_ROTATE) | (1ULL << WIDX_FENCE_SPINNER_HEIGHT_INCREASE) | (1ULL << WIDX_FENCE_SPINNER_HEIGHT_DECREASE),
	(1ULL << WIDX_CLOSE) | (1ULL << WIDX_BUTTON_CORRUPT) | (1ULL << WIDX_BUTTON_REMOVE),
	(1ULL << WIDX_CLOSE) | (1ULL << WIDX_BUTTON_CORRUPT) | (1ULL << WIDX_BUTTON_REMOVE) | (1ULL << WIDX_BUTTON_ROTATE),
	(1ULL << WIDX_CLOSE) | (1ULL << WIDX_BUTTON_CORRUPT) | (1ULL << WIDX_BUTTON_REMOVE),
};

static uint64 window_tile_inspector_page_disabled_widgets[] = {
	(1ULL << WIDX_BUTTON_CORRUPT) | (1ULL << WIDX_BUTTON_MOVE_UP) | (1ULL << WIDX_BUTTON_MOVE_DOWN) | (1ULL << WIDX_BUTTON_REMOVE) | (1ULL << WIDX_BUTTON_ROTATE),
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

void window_tile_inspector_open() {
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
		&window_tile_inspector_events,
		WC_TILE_INSPECTOR,
		WF_RESIZABLE
	);
	window_tile_inspector_set_page(window, PAGE_DEFAULT);

	window_init_scroll_widgets(window);
	window->min_width = MIN_WW;
	window->min_height = MIN_WH;
	window->max_width = MAX_WW;
	window->max_height = MAX_WH;
	window->selected_list_item = -1;

	window_tile_inspector_tile_x = -1;
	window_tile_inspector_tile_y = -1;

	tool_set(window, WIDX_BACKGROUND, 12);
	window_tile_inspector_auto_set_buttons(window);
}

static void window_tile_inspector_load_tile(rct_window* w) {
	rct_map_element *element = map_get_first_element_at(window_tile_inspector_tile_x, window_tile_inspector_tile_y);
	int numItems = 0;
	do {
		numItems++;
	} while (!map_element_is_last_for_tile(element++));

	window_tile_inspector_item_count = numItems;

	// Set default page
	window_tile_inspector_set_page(w, PAGE_DEFAULT);

	// undo selection and buttons affecting it
	w->selected_list_item = -1;
	window_tile_inspector_auto_set_buttons(w);

	w->scrolls[0].v_top = 0;
	window_invalidate(w);
}

static void corrupt_element() {
	rct_map_element* mapElement;
	mapElement = map_get_first_element_at(window_tile_inspector_tile_x, window_tile_inspector_tile_y);

	while (!map_element_is_last_for_tile(mapElement++));
	mapElement--;

	mapElement = map_element_insert(window_tile_inspector_tile_x, window_tile_inspector_tile_y, mapElement->base_height, 0);
	assert(mapElement != NULL);
	mapElement->type = (8 << 2);
	map_invalidate_tile_full(window_tile_inspector_tile_x << 5, window_tile_inspector_tile_y << 5);
}

static void remove_element(int index) {
	assert(index < window_tile_inspector_item_count);
	rct_map_element *mapElement = map_get_first_element_at(window_tile_inspector_tile_x, window_tile_inspector_tile_y);
	mapElement += index;
	map_element_remove(mapElement);
	window_tile_inspector_item_count--;
	map_invalidate_tile_full(window_tile_inspector_tile_x << 5, window_tile_inspector_tile_y << 5);
}

static void rotate_element(int index) {
	uint8 new_rotation, path_edges, path_corners;

	assert(index < window_tile_inspector_item_count);
	rct_map_element *mapElement = map_get_first_element_at(window_tile_inspector_tile_x, window_tile_inspector_tile_y);
	mapElement += index;
	switch (map_element_get_type(mapElement)) {
	case MAP_ELEMENT_TYPE_PATH:
		if (footpath_element_is_sloped(mapElement)) {
			new_rotation = (footpath_element_get_slope_direction(mapElement) + 1) & MAP_ELEMENT_DIRECTION_MASK;
			mapElement->properties.path.type &= ~MAP_ELEMENT_DIRECTION_MASK;
			mapElement->properties.path.type |= new_rotation;
		}
		path_edges = mapElement->properties.path.edges & 0xF;
		path_corners = mapElement->properties.path.edges & 0xF0;
		mapElement->properties.path.edges = 0;
		mapElement->properties.path.edges |= ((path_edges << 1) | (path_edges >> 3)) & 0xF;
		mapElement->properties.path.edges |= ((path_corners << 1) | (path_corners >> 3)) & 0xF0;
		break;
	case MAP_ELEMENT_TYPE_TRACK:
	case MAP_ELEMENT_TYPE_SCENERY:
	case MAP_ELEMENT_TYPE_ENTRANCE:
	case MAP_ELEMENT_TYPE_FENCE:
	case MAP_ELEMENT_TYPE_BANNER:
		new_rotation = (mapElement->type + 1) & MAP_ELEMENT_DIRECTION_MASK;
		mapElement->type &= ~MAP_ELEMENT_DIRECTION_MASK;
		mapElement->type |= new_rotation;
		break;
	}

	map_invalidate_tile_full(window_tile_inspector_tile_x << 5, window_tile_inspector_tile_y << 5);
}

// Swap element with its parent
static void swap_elements(sint16 first, sint16 second) {
	rct_map_element *mapElement;
	rct_map_element *firstElement = NULL;
	rct_map_element *secondElement = NULL;
	mapElement = map_get_first_element_at(window_tile_inspector_tile_x, window_tile_inspector_tile_y);

	// swap_elements shouldn't be called when there is only one element on the tile
	assert(!map_element_is_last_for_tile(mapElement));

	// Search for the elements
	sint16 i = 0;
	do {
		if (i == first) firstElement = mapElement;
		if (i == second) secondElement = mapElement;
		i++;

		// Check if both elements have been found
		if (firstElement != NULL && secondElement != NULL)
			break;
	} while (!map_element_is_last_for_tile(mapElement++));

	// Swap their memory
	rct_map_element temp = *firstElement;
	*firstElement = *secondElement;
	*secondElement = temp;

	// Swap the 'last map element for tile' flag if either one of them was last
	if (map_element_is_last_for_tile(firstElement) || map_element_is_last_for_tile(secondElement)) {
		firstElement->flags ^= MAP_ELEMENT_FLAG_LAST_TILE;
		secondElement->flags ^= MAP_ELEMENT_FLAG_LAST_TILE;
	}
	map_invalidate_tile_full(window_tile_inspector_tile_x << 5, window_tile_inspector_tile_y << 5);
}

static void sort_elements(rct_window *w) {
	const rct_map_element *const firstElement = map_get_first_element_at(window_tile_inspector_tile_x, window_tile_inspector_tile_y);

	// Bubble sort
	for (int loop_start = 1; loop_start < window_tile_inspector_item_count; loop_start++) {
		int current_id = loop_start;
		const rct_map_element *currentElement = firstElement + current_id;
		const rct_map_element *otherElement = currentElement - 1;

		// While current element's base height is lower, or (when their baseheight is the same) the other map element's clearance height is lower...
		while (current_id > 0 && (otherElement->base_height > currentElement->base_height || (otherElement->base_height == currentElement->base_height && otherElement->clearance_height > currentElement->clearance_height))) {
			swap_elements(current_id - 1, current_id);
			current_id--;
			currentElement--;
			otherElement--;
		}
	}
}

// Copied from track.c (track_remove), and modified for raising/lowering
// Not sure if this should be in this file, track.c, or maybe another one
static void track_block_height_offset(rct_map_element *mapElement, uint8 offset) {
	uint8 type = mapElement->properties.track.type;
	uint8 sequence = mapElement->properties.track.sequence;
	sint16 originX = window_tile_inspector_tile_x << 5;
	sint16 originY = window_tile_inspector_tile_y << 5;
	sint16 originZ = mapElement->base_height * 8;
	uint8 rotation = map_element_get_direction(mapElement);

	sint16 trackpieceZ = originZ;

	switch (type) {
	case TRACK_ELEM_BEGIN_STATION:
	case TRACK_ELEM_MIDDLE_STATION:
		type = TRACK_ELEM_END_STATION;
		break;
	}

	uint8 rideIndex = mapElement->properties.track.ride_index;
	type = mapElement->properties.track.type;

	rct_ride* ride = get_ride(rideIndex);
	const rct_preview_track* trackBlock = get_track_def_from_ride(ride, type);
	trackBlock += mapElement->properties.track.sequence & 0xF;

	uint8 originDirection = map_element_get_direction(mapElement);
	switch (originDirection) {
	case 0:
		originX -= trackBlock->x;
		originY -= trackBlock->y;
		break;
	case 1:
		originX -= trackBlock->y;
		originY += trackBlock->x;
		break;
	case 2:
		originX += trackBlock->x;
		originY += trackBlock->y;
		break;
	case 3:
		originX += trackBlock->y;
		originY -= trackBlock->x;
		break;
	}

	originZ -= trackBlock->z;

	trackBlock = get_track_def_from_ride(ride, type);
	for (; trackBlock->index != 255; trackBlock++) {
		sint16 x = originX, y = originY, z = originZ;

		switch (originDirection) {
		case 0:
			x += trackBlock->x;
			y += trackBlock->y;
			break;
		case 1:
			x += trackBlock->y;
			y -= trackBlock->x;
			break;
		case 2:
			x -= trackBlock->x;
			y -= trackBlock->y;
			break;
		case 3:
			x -= trackBlock->y;
			y += trackBlock->x;
			break;
		}

		z += trackBlock->z;

		map_invalidate_tile_full(x, y);

		trackpieceZ = z;

		bool found = false;
		mapElement = map_get_first_element_at(x / 32, y / 32);
		do {
			if (mapElement->base_height != z / 8)
				continue;

			if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
				continue;

			if ((mapElement->type & MAP_ELEMENT_DIRECTION_MASK) != rotation)
				continue;

			if ((mapElement->properties.track.sequence & 0xF) != trackBlock->index)
				continue;

			if (mapElement->properties.track.type != type)
				continue;

			found = true;
			break;
		} while (!map_element_is_last_for_tile(mapElement++));

		if (!found) {
			log_error("Track map element part not found!");
			return;
		}

		// track_remove returns here on failure, not sure when this would ever be hit. Only thing I can think of is for when you decrease the map size.
		assert(map_get_surface_element_at(x / 32, y / 32) != NULL);

		// Keep?
		//invalidate_test_results(rideIndex);

		mapElement->base_height += offset;
		mapElement->clearance_height += offset;
	}
}

// Sets chainlift for entire block
// Basically a copy of the above function, with just two different lines... should probably be combined somehow
static void track_block_set_lift(rct_map_element *mapElement, bool chain) {
	uint8 type = mapElement->properties.track.type;
	uint8 sequence = mapElement->properties.track.sequence;
	sint16 originX = window_tile_inspector_tile_x << 5;
	sint16 originY = window_tile_inspector_tile_y << 5;
	sint16 originZ = mapElement->base_height * 8;
	uint8 rotation = map_element_get_direction(mapElement);

	sint16 trackpieceZ = originZ;

	switch (type) {
	case TRACK_ELEM_BEGIN_STATION:
	case TRACK_ELEM_MIDDLE_STATION:
		type = TRACK_ELEM_END_STATION;
		break;
	}

	uint8 rideIndex = mapElement->properties.track.ride_index;
	type = mapElement->properties.track.type;

	rct_ride* ride = get_ride(rideIndex);
	const rct_preview_track* trackBlock = get_track_def_from_ride(ride, type);
	trackBlock += mapElement->properties.track.sequence & 0xF;

	uint8 originDirection = map_element_get_direction(mapElement);
	switch (originDirection) {
	case 0:
		originX -= trackBlock->x;
		originY -= trackBlock->y;
		break;
	case 1:
		originX -= trackBlock->y;
		originY += trackBlock->x;
		break;
	case 2:
		originX += trackBlock->x;
		originY += trackBlock->y;
		break;
	case 3:
		originX += trackBlock->y;
		originY -= trackBlock->x;
		break;
	}

	originZ -= trackBlock->z;

	trackBlock = get_track_def_from_ride(ride, type);
	for (; trackBlock->index != 255; trackBlock++) {
		sint16 x = originX, y = originY, z = originZ;

		switch (originDirection) {
		case 0:
			x += trackBlock->x;
			y += trackBlock->y;
			break;
		case 1:
			x += trackBlock->y;
			y -= trackBlock->x;
			break;
		case 2:
			x -= trackBlock->x;
			y -= trackBlock->y;
			break;
		case 3:
			x -= trackBlock->y;
			y += trackBlock->x;
			break;
		}

		z += trackBlock->z;

		map_invalidate_tile_full(x, y);

		trackpieceZ = z;

		bool found = false;
		mapElement = map_get_first_element_at(x / 32, y / 32);
		do {
			if (mapElement->base_height != z / 8)
				continue;

			if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
				continue;

			if ((mapElement->type & MAP_ELEMENT_DIRECTION_MASK) != rotation)
				continue;

			if ((mapElement->properties.track.sequence & 0xF) != trackBlock->index)
				continue;

			if (mapElement->properties.track.type != type)
				continue;

			found = true;
			break;
		} while (!map_element_is_last_for_tile(mapElement++));

		if (!found) {
			log_error("Track map element part not found!");
			return;
		}

		// track_remove returns here on failure, not sure when this would ever be hit. Only thing I can think of is for when you decrease the map size.
		assert(map_get_surface_element_at(x / 32, y / 32) != NULL);

		// Keep?
		//invalidate_test_results(rideIndex);

		if (track_element_is_lift_hill(mapElement) != chain) {
			mapElement->type ^= TRACK_ELEMENT_FLAG_CHAIN_LIFT;
		}
	}
}

static void quarter_tile_set(rct_map_element *const mapElement, const int index) {
	// index is widget index relative to WIDX_SCENERY_CHECK_QUARTER_N, so a value from 0-3
	assert(index >= 0 && index < 4);

	const int clicked_direction = (4 + index - get_current_rotation()) % 4;

	// Set quadrant index
	mapElement->type &= ~MAP_ELEMENT_QUADRANT_MASK;
	mapElement->type |= clicked_direction << 6;

	// Update collision
	mapElement->flags &= 0xF0;
	mapElement->flags |= 1 << ((index + 6 - get_current_rotation()) % 4);

	map_invalidate_tile_full(window_tile_inspector_tile_x << 5, window_tile_inspector_tile_y << 5);
}

static void toggle_quarter_collision(rct_map_element *const mapElement, const int index) {
	// index is widget index relative to WIDX_SCENERY_CHECK_COLLISION_N, so a value from 0-3
	assert(index >= 0 && index < 4);

	mapElement->flags ^= 1 << ((index + 6 - get_current_rotation()) % 4);
}

static void window_tile_inspector_mouseup(rct_window *w, int widgetIndex) {
	switch (widgetIndex) {
	case WIDX_CLOSE:
		tool_cancel();
		window_close(w);
		break;
	case WIDX_SPINNER_X_INCREASE:
		window_tile_inspector_tile_x = min(window_tile_inspector_tile_x + 1, 255);
		window_tile_inspector_load_tile(w);
		window_tile_inspector_auto_set_buttons(w);
		break;
	case WIDX_SPINNER_X_DECREASE:
		window_tile_inspector_tile_x = max(window_tile_inspector_tile_x - 1, 0);
		window_tile_inspector_load_tile(w);
		window_tile_inspector_auto_set_buttons(w);
		break;
	case WIDX_SPINNER_Y_INCREASE:
		window_tile_inspector_tile_y = min(window_tile_inspector_tile_y + 1, 255);
		window_tile_inspector_load_tile(w);
		window_tile_inspector_auto_set_buttons(w);
		break;
	case WIDX_SPINNER_Y_DECREASE:
		window_tile_inspector_tile_y = max(window_tile_inspector_tile_y - 1, 0);
		window_tile_inspector_load_tile(w);
		window_tile_inspector_auto_set_buttons(w);
		break;
	case WIDX_BUTTON_CORRUPT:
		corrupt_element();
		window_tile_inspector_set_page(w, PAGE_CORRUPT);
		w->scrolls[0].v_top = 0;
		w->selected_list_item = window_tile_inspector_item_count++;
		window_tile_inspector_auto_set_buttons(w);
		widget_invalidate(w, WIDX_LIST);
		break;
	case WIDX_BUTTON_REMOVE:
		remove_element(w->selected_list_item);
		window_tile_inspector_set_page(w, PAGE_DEFAULT);
		w->selected_list_item = -1;
		window_tile_inspector_set_page(w, PAGE_DEFAULT);
		window_tile_inspector_auto_set_buttons(w);
		window_invalidate(w);
		break;
	case WIDX_BUTTON_ROTATE:
		rotate_element(w->selected_list_item);
		window_invalidate(w);
		break;
	case WIDX_BUTTON_SORT:
		w->selected_list_item = -1;
		sort_elements(w);
		window_invalidate(w);
		break;
	case WIDX_BUTTON_MOVE_DOWN:
		swap_elements(w->selected_list_item, w->selected_list_item + 1);
		w->selected_list_item++;
		window_tile_inspector_auto_set_buttons(w);
		widget_invalidate(w, WIDX_LIST);
		break;
	case WIDX_BUTTON_MOVE_UP:
		swap_elements(w->selected_list_item - 1, w->selected_list_item);
		w->selected_list_item--;
		window_tile_inspector_auto_set_buttons(w);
		widget_invalidate(w, WIDX_LIST);
		break;
	}

	// Get the selected map element
	rct_map_element *const mapElement = map_get_first_element_at(window_tile_inspector_tile_x, window_tile_inspector_tile_y) + w->selected_list_item;

	// Page widgets
	switch (w->page) {
	case PAGE_SURFACE:
		switch (widgetIndex) {
		case WIDX_SURFACE_SPINNER_HEIGHT_INCREASE:
			mapElement->base_height++;
			mapElement->clearance_height++;
			map_invalidate_tile_full(window_tile_inspector_tile_x << 5, window_tile_inspector_tile_y << 5);
			widget_invalidate(w, WIDX_PATH_SPINNER_HEIGHT);
			break;
		case WIDX_SURFACE_SPINNER_HEIGHT_DECREASE:
			mapElement->base_height--;
			mapElement->clearance_height--;
			map_invalidate_tile_full(window_tile_inspector_tile_x << 5, window_tile_inspector_tile_y << 5);
			widget_invalidate(w, WIDX_PATH_SPINNER_HEIGHT);
			break;
		case WIDX_SURFACE_BUTTON_REMOVE_FENCES:
			mapElement->properties.surface.ownership &= ~0xF;
			map_invalidate_tile_full(window_tile_inspector_tile_x << 5, window_tile_inspector_tile_y << 5);
			break;
		case WIDX_SURFACE_BUTTON_RESTORE_FENCES:
			update_park_fences(window_tile_inspector_tile_x << 5, window_tile_inspector_tile_y << 5);
			map_invalidate_tile_full(window_tile_inspector_tile_x << 5, window_tile_inspector_tile_y << 5);
			break;
		case WIDX_SURFACE_CHECK_CORNER_N:
		case WIDX_SURFACE_CHECK_CORNER_E:
		case WIDX_SURFACE_CHECK_CORNER_S:
		case WIDX_SURFACE_CHECK_CORNER_W:
			mapElement->properties.surface.slope ^= 1 << (((widgetIndex - WIDX_SURFACE_CHECK_CORNER_N) + 6 - get_current_rotation()) % 4);
			if (mapElement->properties.surface.slope & 0x0F) {
				mapElement->clearance_height = mapElement->base_height + 2;
			}
			else {
				mapElement->clearance_height = mapElement->base_height;
			}
			map_invalidate_tile_full(window_tile_inspector_tile_x << 5, window_tile_inspector_tile_y << 5);
			widget_invalidate(w, widgetIndex);
			break;
		case WIDX_SURFACE_CHECK_DIAGONAL:
			mapElement->properties.surface.slope ^= 0x10;
			if (mapElement->properties.surface.slope & 0x10) {
				mapElement->clearance_height = mapElement->base_height + 4;
			}
			map_invalidate_tile_full(window_tile_inspector_tile_x << 5, window_tile_inspector_tile_y << 5);
			widget_invalidate(w, widgetIndex);
			break;
		} // switch widgetindex
		break;

	case PAGE_PATH:
		switch (widgetIndex) {
		case WIDX_PATH_SPINNER_HEIGHT_INCREASE:
			mapElement->base_height++;
			mapElement->clearance_height++;
			map_invalidate_tile_full(window_tile_inspector_tile_x << 5, window_tile_inspector_tile_y << 5);
			widget_invalidate(w, WIDX_PATH_SPINNER_HEIGHT);
			break;
		case WIDX_PATH_SPINNER_HEIGHT_DECREASE:
			mapElement->base_height--;
			mapElement->clearance_height--;
			map_invalidate_tile_full(window_tile_inspector_tile_x << 5, window_tile_inspector_tile_y << 5);
			widget_invalidate(w, WIDX_PATH_SPINNER_HEIGHT);
			break;
		case WIDX_PATH_CHECK_EDGE_E:
		case WIDX_PATH_CHECK_EDGE_S:
		case WIDX_PATH_CHECK_EDGE_W:
		case WIDX_PATH_CHECK_EDGE_N:
			widget_set_checkbox_value(w, widgetIndex, !widget_is_pressed(w, widgetIndex));
			mapElement->properties.path.edges ^= (1 << (4 + (((widgetIndex - WIDX_PATH_CHECK_EDGE_E) / 2 - get_current_rotation() + 4) % 4))) & 0xF0;
			map_invalidate_tile_full(window_tile_inspector_tile_x << 5, window_tile_inspector_tile_y << 5);
			break;
		case WIDX_PATH_CHECK_EDGE_NE:
		case WIDX_PATH_CHECK_EDGE_SE:
		case WIDX_PATH_CHECK_EDGE_SW:
		case WIDX_PATH_CHECK_EDGE_NW:
			widget_set_checkbox_value(w, widgetIndex, !widget_is_pressed(w, widgetIndex));
			mapElement->properties.path.edges ^= (1 << (((widgetIndex - WIDX_PATH_CHECK_EDGE_NE) / 2 - get_current_rotation() + 4) % 4)) & 0x0F;
			map_invalidate_tile_full(window_tile_inspector_tile_x << 5, window_tile_inspector_tile_y << 5);
			break;
		} // switch widget index
		break;

	case PAGE_TRACK:
		switch (widgetIndex) {
		case WIDX_TRACK_CHECK_APPLY_TO_ALL:
			window_tile_inspector_apply_to_all ^= 1;
			widget_invalidate(w, widgetIndex);
			break;
		case WIDX_TRACK_SPINNER_HEIGHT_INCREASE:
			if (widget_is_pressed(w, WIDX_TRACK_CHECK_APPLY_TO_ALL)) {
				track_block_height_offset(mapElement, 1);
			}
			else {
				mapElement->base_height++;
				mapElement->clearance_height++;
				map_invalidate_tile_full(window_tile_inspector_tile_x << 5, window_tile_inspector_tile_y << 5);
			}
			widget_invalidate(w, WIDX_TRACK_SPINNER_HEIGHT);
			break;
		case WIDX_TRACK_SPINNER_HEIGHT_DECREASE:
			if (widget_is_pressed(w, WIDX_TRACK_CHECK_APPLY_TO_ALL)) {
				track_block_height_offset(mapElement, -1);
			}
			else {
				mapElement->base_height--;
				mapElement->clearance_height--;
				map_invalidate_tile_full(window_tile_inspector_tile_x << 5, window_tile_inspector_tile_y << 5);
			}
			widget_invalidate(w, WIDX_TRACK_SPINNER_HEIGHT);
			break;
		case WIDX_TRACK_CHECK_CHAIN_LIFT:
			if (widget_is_pressed(w, WIDX_TRACK_CHECK_APPLY_TO_ALL)) {
				bool new_lift = !track_element_is_lift_hill(mapElement);
				track_block_set_lift(mapElement, new_lift);
			}
			else {
				mapElement->type ^= TRACK_ELEMENT_FLAG_CHAIN_LIFT;
				map_invalidate_tile_full(window_tile_inspector_tile_x << 5, window_tile_inspector_tile_y << 5);
			}
			widget_invalidate(w, widgetIndex);
			break;
		} // switch widget index
		break;

	case PAGE_SCENERY:
		switch (widgetIndex) {
		case WIDX_SCENERY_SPINNER_HEIGHT_INCREASE:
			mapElement->base_height++;
			mapElement->clearance_height++;
			map_invalidate_tile_full(window_tile_inspector_tile_x << 5, window_tile_inspector_tile_y << 5);
			widget_invalidate(w, WIDX_SCENERY_SPINNER_HEIGHT);
			break;
		case WIDX_SCENERY_SPINNER_HEIGHT_DECREASE:
			mapElement->base_height--;
			mapElement->clearance_height--;
			map_invalidate_tile_full(window_tile_inspector_tile_x << 5, window_tile_inspector_tile_y << 5);
			widget_invalidate(w, WIDX_SCENERY_SPINNER_HEIGHT);
			break;
		case WIDX_SCENERY_CHECK_QUARTER_N:
		case WIDX_SCENERY_CHECK_QUARTER_E:
		case WIDX_SCENERY_CHECK_QUARTER_S:
		case WIDX_SCENERY_CHECK_QUARTER_W:
			quarter_tile_set(mapElement, widgetIndex - WIDX_SCENERY_CHECK_QUARTER_N);
			window_invalidate(w);
			break;
		case WIDX_SCENERY_CHECK_COLLISION_N:
		case WIDX_SCENERY_CHECK_COLLISION_E:
		case WIDX_SCENERY_CHECK_COLLISION_S:
		case WIDX_SCENERY_CHECK_COLLISION_W:
			toggle_quarter_collision(mapElement, widgetIndex - WIDX_SCENERY_CHECK_COLLISION_N);
			window_invalidate(w);
			break;
		}
		break;

	case PAGE_ENTRANCE:
		break;

	case PAGE_FENCE:
		switch (widgetIndex) {
		case WIDX_FENCE_SPINNER_HEIGHT_INCREASE:
			mapElement->base_height++;
			mapElement->clearance_height++;
			map_invalidate_tile_full(window_tile_inspector_tile_x << 5, window_tile_inspector_tile_y << 5);
			widget_invalidate(w, WIDX_FENCE_SPINNER_HEIGHT);
			break;
		case WIDX_FENCE_SPINNER_HEIGHT_DECREASE:
			mapElement->base_height--;
			mapElement->clearance_height--;
			map_invalidate_tile_full(window_tile_inspector_tile_x << 5, window_tile_inspector_tile_y << 5);
			widget_invalidate(w, WIDX_FENCE_SPINNER_HEIGHT);
			break;
		} // switch widget index
		break;
	case PAGE_LARGE_SCENERY:
	case PAGE_BANNER:
	case PAGE_CORRUPT:
		break;
	} // switch page
}

static void window_tile_inspector_resize(rct_window *w) {
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

static void window_tile_inspector_update(rct_window *w) {
	// Check if the mouse is hovering over the list
	if (!widget_is_highlighted(w, WIDX_LIST)) {
		window_tile_inspector_highlighted_index = -1;
		widget_invalidate(w, WIDX_LIST);
	}

	if (gCurrentToolWidget.window_classification != WC_TILE_INSPECTOR)
		window_close(w);
}

static void window_tile_inspector_tool_update(rct_window* w, int widgetIndex, int x, int y) {
	int direction;
	short mapX, mapY;

	map_invalidate_selection_rect();
	gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;

	mapX = x;
	mapY = y;
	screen_pos_to_map_pos(&mapX, &mapY, &direction);
	if (mapX == (short)0x8000) {
		gMapSelectPositionA.x = gMapSelectPositionB.x = window_tile_inspector_tile_x << 5;
		gMapSelectPositionA.y = gMapSelectPositionB.y = window_tile_inspector_tile_y << 5;
	}
	else {
		gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
		gMapSelectPositionA.x = gMapSelectPositionB.x = mapX;
		gMapSelectPositionA.y = gMapSelectPositionB.y = mapY;
	}

	gMapSelectType = MAP_SELECT_TYPE_FULL;
	map_invalidate_selection_rect();
}

static void window_tile_inspector_tool_down(rct_window* w, int widgetIndex, int x, int y) {
	int direction;
	short mapX, mapY;

	mapX = x;
	mapY = y;
	screen_pos_to_map_pos(&mapX, &mapY, &direction);
	if (mapX == (short)0x8000) {
		return;
	}

	window_tile_inspector_tile_x = mapX >> 5;
	window_tile_inspector_tile_y = mapY >> 5;

	window_tile_inspector_load_tile(w);
	window_tile_inspector_auto_set_buttons(w);
}

static void window_tile_inspector_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height) {
	*width = WW - 30;
	*height = window_tile_inspector_item_count * LIST_ITEM_HEIGHT;
}

static void window_tile_inspector_set_page(rct_window *w, const int page) {
	w->page = page;
	w->widgets = tile_inspector_widgets[page];
	w->enabled_widgets = window_tile_inspector_page_enabled_widgets[page];
	w->disabled_widgets = window_tile_inspector_page_disabled_widgets[page];
}

static void window_tile_inspector_auto_set_buttons(rct_window *w) {
	// X and Y spinners
	bool tile_is_selected = (window_tile_inspector_tile_x != -1) && (window_tile_inspector_tile_y != -1);
	widget_set_enabled(w, WIDX_SPINNER_X_INCREASE, (tile_is_selected && (window_tile_inspector_tile_x < 255)));
	widget_set_enabled(w, WIDX_SPINNER_X_DECREASE, (tile_is_selected && (window_tile_inspector_tile_x > 0)));
	widget_set_enabled(w, WIDX_SPINNER_Y_INCREASE, (tile_is_selected && (window_tile_inspector_tile_y < 255)));
	widget_set_enabled(w, WIDX_SPINNER_Y_DECREASE, (tile_is_selected && (window_tile_inspector_tile_y > 0)));

	// Sort buttons
	widget_set_enabled(w, WIDX_BUTTON_SORT, (tile_is_selected && window_tile_inspector_item_count > 1));

	// Move Up button
	widget_set_enabled(w, WIDX_BUTTON_MOVE_UP, (w->selected_list_item > 0));
	widget_invalidate(w, WIDX_BUTTON_MOVE_UP);

	// Move Down button
	widget_set_enabled(w, WIDX_BUTTON_MOVE_DOWN, (w->selected_list_item != -1 && w->selected_list_item < window_tile_inspector_item_count - 1));
	widget_invalidate(w, WIDX_BUTTON_MOVE_DOWN);
}

static void window_tile_inspector_scrollmousedown(rct_window *w, int scrollIndex, int x, int y) {
	// Because the list items are displayed in reverse order, subtract the calculated index from the amount of elements
	const sint16 index = window_tile_inspector_item_count - (y - 1) / LIST_ITEM_HEIGHT - 1;
	int page;
	if (index < 0 || index >= window_tile_inspector_item_count) {
		w->selected_list_item = -1;
		page = 0;
	}
	else {
		w->selected_list_item = index;

		// Get type of selected map element to select the correct page
		rct_map_element *mapElement = map_get_first_element_at(window_tile_inspector_tile_x, window_tile_inspector_tile_y);
		mapElement += index;
		page = (min(map_element_get_type(mapElement), MAP_ELEMENT_TYPE_CORRUPT) >> 2) + 1;
	}

	window_tile_inspector_set_page(w, page);

	// Enable/disable buttons
	window_tile_inspector_auto_set_buttons(w);
	window_invalidate(w);
}

static void window_tile_inspector_scrollmouseover(rct_window *w, int scrollIndex, int x, int y) {
	sint16 index = window_tile_inspector_item_count - (y - 1) / LIST_ITEM_HEIGHT - 1;
	if (index < 0 || index >= window_tile_inspector_item_count)
		window_tile_inspector_highlighted_index = -1;
	else
		window_tile_inspector_highlighted_index = index;

	widget_invalidate(w, WIDX_LIST);
}

static void window_tile_inspector_invalidate(rct_window *w) {
	colour_scheme_update(w);

	w->widgets[WIDX_BACKGROUND].bottom = w->height - 1;

	if (w->page == PAGE_DEFAULT) {
		w->widgets[WIDX_GROUPBOX_DETAILS].type = WWT_EMPTY;
		w->widgets[WIDX_GROUPBOX_PROPERTIES].type = WWT_EMPTY;
		w->widgets[WIDX_LIST].bottom = w->height - PADDING_BOTTOM;
	}
	else {
		w->widgets[WIDX_GROUPBOX_DETAILS].type = WWT_GROUPBOX;
		w->widgets[WIDX_GROUPBOX_PROPERTIES].type = WWT_GROUPBOX;
		w->widgets[WIDX_GROUPBOX_DETAILS].image = page_group_box_settings[w->page - 1].string_idx;
		w->widgets[WIDX_GROUPBOX_DETAILS].top = w->height - page_group_box_settings[w->page - 1].details_top_offset;
		w->widgets[WIDX_GROUPBOX_DETAILS].bottom = w->height - page_group_box_settings[w->page - 1].details_bottom_offset;
		w->widgets[WIDX_GROUPBOX_PROPERTIES].top = w->height - page_group_box_settings[w->page - 1].properties_top_offset;
		w->widgets[WIDX_GROUPBOX_PROPERTIES].bottom = w->height - page_group_box_settings[w->page - 1].properties_bottom_offset;
		w->widgets[WIDX_LIST].bottom = w->widgets[WIDX_GROUPBOX_DETAILS].top - GROUPBOX_PADDING;
	}

	// Only page-specific widgets related to the map element will be
	if (w->page == PAGE_DEFAULT) {
		return;
	}

	// Using a switch, because I don't think giving each page their own callbacks is
	// needed here, as only the mouseup and invalidate functions would be different.
	const int details_anchor = w->widgets[WIDX_GROUPBOX_DETAILS].top;
	const int properties_anchor = w->widgets[WIDX_GROUPBOX_PROPERTIES].top;
	rct_map_element *const mapElement = map_get_first_element_at(window_tile_inspector_tile_x, window_tile_inspector_tile_y) + w->selected_list_item;

	switch (w->page) {
	case PAGE_SURFACE:
		w->widgets[WIDX_SURFACE_SPINNER_HEIGHT].top = GBBT(properties_anchor, 0) + 3;
		w->widgets[WIDX_SURFACE_SPINNER_HEIGHT].bottom = GBBB(properties_anchor, 0) - 3;
		w->widgets[WIDX_SURFACE_SPINNER_HEIGHT_INCREASE].top = GBBT(properties_anchor, 0) + 4;
		w->widgets[WIDX_SURFACE_SPINNER_HEIGHT_INCREASE].bottom = GBBT(properties_anchor, 0) + 8;
		w->widgets[WIDX_SURFACE_SPINNER_HEIGHT_DECREASE].top = GBBB(properties_anchor, 0) - 8;
		w->widgets[WIDX_SURFACE_SPINNER_HEIGHT_DECREASE].bottom = GBBB(properties_anchor, 0) - 4;
		w->widgets[WIDX_SURFACE_BUTTON_REMOVE_FENCES].top = GBBT(properties_anchor, 1);
		w->widgets[WIDX_SURFACE_BUTTON_REMOVE_FENCES].bottom = GBBB(properties_anchor, 1);
		w->widgets[WIDX_SURFACE_BUTTON_RESTORE_FENCES].top = GBBT(properties_anchor, 1);
		w->widgets[WIDX_SURFACE_BUTTON_RESTORE_FENCES].bottom = GBBB(properties_anchor, 1);
		w->widgets[WIDX_SURFACE_CHECK_CORNER_N].top = GBBT(properties_anchor, 2) + 7 * 0;
		w->widgets[WIDX_SURFACE_CHECK_CORNER_N].bottom = w->widgets[WIDX_SURFACE_CHECK_CORNER_N].top + 13;
		w->widgets[WIDX_SURFACE_CHECK_CORNER_E].top = GBBT(properties_anchor, 2) + 7 * 1;
		w->widgets[WIDX_SURFACE_CHECK_CORNER_E].bottom = w->widgets[WIDX_SURFACE_CHECK_CORNER_E].top + 13;
		w->widgets[WIDX_SURFACE_CHECK_CORNER_S].top = GBBT(properties_anchor, 2) + 7 * 2;
		w->widgets[WIDX_SURFACE_CHECK_CORNER_S].bottom = w->widgets[WIDX_SURFACE_CHECK_CORNER_S].top + 13;
		w->widgets[WIDX_SURFACE_CHECK_CORNER_W].top = GBBT(properties_anchor, 2) + 7 * 1;
		w->widgets[WIDX_SURFACE_CHECK_CORNER_W].bottom = w->widgets[WIDX_SURFACE_CHECK_CORNER_W].top + 13;
		w->widgets[WIDX_SURFACE_CHECK_DIAGONAL].top = GBBT(properties_anchor, 3) + 7 * 1;
		w->widgets[WIDX_SURFACE_CHECK_DIAGONAL].bottom = w->widgets[WIDX_SURFACE_CHECK_DIAGONAL].top + 13;
		widget_set_checkbox_value(w, WIDX_SURFACE_CHECK_CORNER_N, mapElement->properties.surface.slope & (1 << ((4 + 2 - get_current_rotation()) % 4)));
		widget_set_checkbox_value(w, WIDX_SURFACE_CHECK_CORNER_E, mapElement->properties.surface.slope & (1 << ((4 + 3 - get_current_rotation()) % 4)));
		widget_set_checkbox_value(w, WIDX_SURFACE_CHECK_CORNER_S, mapElement->properties.surface.slope & (1 << ((4 + 0 - get_current_rotation()) % 4)));
		widget_set_checkbox_value(w, WIDX_SURFACE_CHECK_CORNER_W, mapElement->properties.surface.slope & (1 << ((4 + 1 - get_current_rotation()) % 4)));
		widget_set_checkbox_value(w, WIDX_SURFACE_CHECK_DIAGONAL, mapElement->properties.surface.slope & 0x10);
		break;
	case PAGE_PATH:
		w->widgets[WIDX_PATH_SPINNER_HEIGHT].top = GBBT(properties_anchor, 0) + 3;
		w->widgets[WIDX_PATH_SPINNER_HEIGHT].bottom = GBBB(properties_anchor, 0) - 3;
		w->widgets[WIDX_PATH_SPINNER_HEIGHT_INCREASE].top = GBBT(properties_anchor, 0) + 4;
		w->widgets[WIDX_PATH_SPINNER_HEIGHT_INCREASE].bottom = GBBT(properties_anchor, 0) + 8;
		w->widgets[WIDX_PATH_SPINNER_HEIGHT_DECREASE].top = GBBB(properties_anchor, 0) - 8;
		w->widgets[WIDX_PATH_SPINNER_HEIGHT_DECREASE].bottom = GBBB(properties_anchor, 0) - 4;
		w->widgets[WIDX_PATH_CHECK_EDGE_N].top = GBBT(properties_anchor, 1) + 7 * 0;
		w->widgets[WIDX_PATH_CHECK_EDGE_N].bottom = w->widgets[WIDX_PATH_CHECK_EDGE_N].top + 13;
		w->widgets[WIDX_PATH_CHECK_EDGE_NE].top = GBBT(properties_anchor, 1) + 7 * 1;
		w->widgets[WIDX_PATH_CHECK_EDGE_NE].bottom = w->widgets[WIDX_PATH_CHECK_EDGE_NE].top + 13;
		w->widgets[WIDX_PATH_CHECK_EDGE_E].top = GBBT(properties_anchor, 1) + 7 * 2;
		w->widgets[WIDX_PATH_CHECK_EDGE_E].bottom = w->widgets[WIDX_PATH_CHECK_EDGE_E].top + 13;
		w->widgets[WIDX_PATH_CHECK_EDGE_SE].top = GBBT(properties_anchor, 1) + 7 * 3;
		w->widgets[WIDX_PATH_CHECK_EDGE_SE].bottom = w->widgets[WIDX_PATH_CHECK_EDGE_SE].top + 13;
		w->widgets[WIDX_PATH_CHECK_EDGE_S].top = GBBT(properties_anchor, 1) + 7 * 4;
		w->widgets[WIDX_PATH_CHECK_EDGE_S].bottom = w->widgets[WIDX_PATH_CHECK_EDGE_S].top + 13;
		w->widgets[WIDX_PATH_CHECK_EDGE_SW].top = GBBT(properties_anchor, 1) + 7 * 3;
		w->widgets[WIDX_PATH_CHECK_EDGE_SW].bottom = w->widgets[WIDX_PATH_CHECK_EDGE_SW].top + 13;
		w->widgets[WIDX_PATH_CHECK_EDGE_W].top = GBBT(properties_anchor, 1) + 7 * 2;
		w->widgets[WIDX_PATH_CHECK_EDGE_W].bottom = w->widgets[WIDX_PATH_CHECK_EDGE_W].top + 13;
		w->widgets[WIDX_PATH_CHECK_EDGE_NW].top = GBBT(properties_anchor, 1) + 7 * 1;
		w->widgets[WIDX_PATH_CHECK_EDGE_NW].bottom = w->widgets[WIDX_PATH_CHECK_EDGE_NW].top + 13;
		widget_set_checkbox_value(w, WIDX_PATH_CHECK_EDGE_NE, mapElement->properties.path.edges & (1 << ((0 + 4 - get_current_rotation()) % 4)));
		widget_set_checkbox_value(w, WIDX_PATH_CHECK_EDGE_SE, mapElement->properties.path.edges & (1 << ((1 + 4 - get_current_rotation()) % 4)));
		widget_set_checkbox_value(w, WIDX_PATH_CHECK_EDGE_SW, mapElement->properties.path.edges & (1 << ((2 + 4 - get_current_rotation()) % 4)));
		widget_set_checkbox_value(w, WIDX_PATH_CHECK_EDGE_NW, mapElement->properties.path.edges & (1 << ((3 + 4 - get_current_rotation()) % 4)));
		widget_set_checkbox_value(w, WIDX_PATH_CHECK_EDGE_E, mapElement->properties.path.edges & (1 << ((0 + 4 - get_current_rotation()) % 4 + 4)));
		widget_set_checkbox_value(w, WIDX_PATH_CHECK_EDGE_S, mapElement->properties.path.edges & (1 << ((1 + 4 - get_current_rotation()) % 4 + 4)));
		widget_set_checkbox_value(w, WIDX_PATH_CHECK_EDGE_W, mapElement->properties.path.edges & (1 << ((2 + 4 - get_current_rotation()) % 4 + 4)));
		widget_set_checkbox_value(w, WIDX_PATH_CHECK_EDGE_N, mapElement->properties.path.edges & (1 << ((3 + 4 - get_current_rotation()) % 4 + 4)));
		break;
	case PAGE_TRACK:
		w->widgets[WIDX_TRACK_CHECK_APPLY_TO_ALL].top = GBBT(properties_anchor, 0);
		w->widgets[WIDX_TRACK_CHECK_APPLY_TO_ALL].bottom = GBBB(properties_anchor, 0);
		w->widgets[WIDX_TRACK_SPINNER_HEIGHT].top = GBBT(properties_anchor, 1) + 3;
		w->widgets[WIDX_TRACK_SPINNER_HEIGHT].bottom = GBBB(properties_anchor, 1) - 3;
		w->widgets[WIDX_TRACK_SPINNER_HEIGHT_INCREASE].top = GBBT(properties_anchor, 1) + 4;
		w->widgets[WIDX_TRACK_SPINNER_HEIGHT_INCREASE].bottom = GBBT(properties_anchor, 1) + 8;
		w->widgets[WIDX_TRACK_SPINNER_HEIGHT_DECREASE].top = GBBB(properties_anchor, 1) - 8;
		w->widgets[WIDX_TRACK_SPINNER_HEIGHT_DECREASE].bottom = GBBB(properties_anchor, 1) - 4;
		w->widgets[WIDX_TRACK_CHECK_CHAIN_LIFT].top = GBBT(properties_anchor, 2);
		w->widgets[WIDX_TRACK_CHECK_CHAIN_LIFT].bottom = GBBB(properties_anchor, 2);
		widget_set_checkbox_value(w, WIDX_TRACK_CHECK_APPLY_TO_ALL, window_tile_inspector_apply_to_all);
		widget_set_checkbox_value(w, WIDX_TRACK_CHECK_CHAIN_LIFT, track_element_is_lift_hill(mapElement));
		break;
	case PAGE_SCENERY: {
		// Raise / Lower
		w->widgets[WIDX_SCENERY_SPINNER_HEIGHT].top = GBBT(properties_anchor, 0) + 3;
		w->widgets[WIDX_SCENERY_SPINNER_HEIGHT].bottom = GBBB(properties_anchor, 0) - 3;
		w->widgets[WIDX_SCENERY_SPINNER_HEIGHT_INCREASE].top = GBBT(properties_anchor, 0) + 4;
		w->widgets[WIDX_SCENERY_SPINNER_HEIGHT_INCREASE].bottom = GBBT(properties_anchor, 0) + 8;
		w->widgets[WIDX_SCENERY_SPINNER_HEIGHT_DECREASE].top = GBBB(properties_anchor, 0) - 8;
		w->widgets[WIDX_SCENERY_SPINNER_HEIGHT_DECREASE].bottom = GBBB(properties_anchor, 0) - 4;

		// Quadrant checkboxes
		w->widgets[WIDX_SCENERY_CHECK_QUARTER_N].top = GBBT(properties_anchor, 1) - 5 + 7 * 0;
		w->widgets[WIDX_SCENERY_CHECK_QUARTER_N].bottom = w->widgets[WIDX_SCENERY_CHECK_QUARTER_N].top + 13;
		w->widgets[WIDX_SCENERY_CHECK_QUARTER_E].top = GBBT(properties_anchor, 1) - 5 + 7 * 1;
		w->widgets[WIDX_SCENERY_CHECK_QUARTER_E].bottom = w->widgets[WIDX_SCENERY_CHECK_QUARTER_E].top + 13;
		w->widgets[WIDX_SCENERY_CHECK_QUARTER_S].top = GBBT(properties_anchor, 1) - 5 + 7 * 2;
		w->widgets[WIDX_SCENERY_CHECK_QUARTER_S].bottom = w->widgets[WIDX_SCENERY_CHECK_QUARTER_S].top + 13;
		w->widgets[WIDX_SCENERY_CHECK_QUARTER_W].top = GBBT(properties_anchor, 1) - 5 + 7 * 1;
		w->widgets[WIDX_SCENERY_CHECK_QUARTER_W].bottom = w->widgets[WIDX_SCENERY_CHECK_QUARTER_W].top + 13;
		// This gets the relative rotation, by subtracting the camera's rotation, and wrapping it between 0-3 inclusive
		bool N = (mapElement->type & MAP_ELEMENT_QUADRANT_MASK) == ((0 + 4 - get_current_rotation()) % 4) << 6;
		bool E = (mapElement->type & MAP_ELEMENT_QUADRANT_MASK) == ((1 + 4 - get_current_rotation()) % 4) << 6;
		bool S = (mapElement->type & MAP_ELEMENT_QUADRANT_MASK) == ((2 + 4 - get_current_rotation()) % 4) << 6;
		bool W = (mapElement->type & MAP_ELEMENT_QUADRANT_MASK) == ((3 + 4 - get_current_rotation()) % 4) << 6;
		widget_set_checkbox_value(w, WIDX_SCENERY_CHECK_QUARTER_N, N);
		widget_set_checkbox_value(w, WIDX_SCENERY_CHECK_QUARTER_E, E);
		widget_set_checkbox_value(w, WIDX_SCENERY_CHECK_QUARTER_S, S);
		widget_set_checkbox_value(w, WIDX_SCENERY_CHECK_QUARTER_W, W);

		// Collision checkboxes
		w->widgets[WIDX_SCENERY_CHECK_COLLISION_N].top = GBBT(properties_anchor, 2) + 5 + 7 * 0;
		w->widgets[WIDX_SCENERY_CHECK_COLLISION_N].bottom = w->widgets[WIDX_SCENERY_CHECK_COLLISION_N].top + 13;
		w->widgets[WIDX_SCENERY_CHECK_COLLISION_E].top = GBBT(properties_anchor, 2) + 5 + 7 * 1;
		w->widgets[WIDX_SCENERY_CHECK_COLLISION_E].bottom = w->widgets[WIDX_SCENERY_CHECK_COLLISION_E].top + 13;
		w->widgets[WIDX_SCENERY_CHECK_COLLISION_S].top = GBBT(properties_anchor, 2) + 5 + 7 * 2;
		w->widgets[WIDX_SCENERY_CHECK_COLLISION_S].bottom = w->widgets[WIDX_SCENERY_CHECK_COLLISION_S].top + 13;
		w->widgets[WIDX_SCENERY_CHECK_COLLISION_W].top = GBBT(properties_anchor, 2) + 5 + 7 * 1;
		w->widgets[WIDX_SCENERY_CHECK_COLLISION_W].bottom = w->widgets[WIDX_SCENERY_CHECK_COLLISION_W].top + 13;
		N = mapElement->flags & (1 << ((2 + 4 - get_current_rotation()) % 4));
		E = mapElement->flags & (1 << ((3 + 4 - get_current_rotation()) % 4));
		S = mapElement->flags & (1 << ((0 + 4 - get_current_rotation()) % 4));
		W = mapElement->flags & (1 << ((1 + 4 - get_current_rotation()) % 4));
		widget_set_checkbox_value(w, WIDX_SCENERY_CHECK_COLLISION_N, N);
		widget_set_checkbox_value(w, WIDX_SCENERY_CHECK_COLLISION_E, E);
		widget_set_checkbox_value(w, WIDX_SCENERY_CHECK_COLLISION_S, S);
		widget_set_checkbox_value(w, WIDX_SCENERY_CHECK_COLLISION_W, W);
		break;
	}
	case PAGE_ENTRANCE:

		break;
	case PAGE_FENCE:
		w->widgets[WIDX_FENCE_SPINNER_HEIGHT].top = GBBT(properties_anchor, 0) + 3;
		w->widgets[WIDX_FENCE_SPINNER_HEIGHT].bottom = GBBB(properties_anchor, 0) - 3;
		w->widgets[WIDX_FENCE_SPINNER_HEIGHT_INCREASE].top = GBBT(properties_anchor, 0) + 4;
		w->widgets[WIDX_FENCE_SPINNER_HEIGHT_INCREASE].bottom = GBBT(properties_anchor, 0) + 8;
		w->widgets[WIDX_FENCE_SPINNER_HEIGHT_DECREASE].top = GBBB(properties_anchor, 0) - 8;
		w->widgets[WIDX_FENCE_SPINNER_HEIGHT_DECREASE].bottom = GBBB(properties_anchor, 0) - 4;
		break;
	case PAGE_LARGE_SCENERY:

		break;
	case PAGE_BANNER:

		break;
	case PAGE_CORRUPT:

		break;
	}
}

static void window_tile_inspector_paint(rct_window *w, rct_drawpixelinfo *dpi) {
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
	gfx_draw_string(dpi, "X:", 12, w->x + 6, w->y + 24);
	gfx_draw_string(dpi, "Y:", 12, w->x + 64, w->y + 24);
	if (window_tile_inspector_tile_x != -1 && window_tile_inspector_tile_y != -1) {
		gfx_draw_string_right(dpi, STR_FORMAT_INTEGER, &window_tile_inspector_tile_x, 12, w->x + 48, w->y + 24);
		gfx_draw_string_right(dpi, STR_FORMAT_INTEGER, &window_tile_inspector_tile_y, 12, w->x + 105, w->y + 24);
	}

	if (w->selected_list_item != -1) {
		// X and Y of first element in detail box
		int x = w->x + w->widgets[WIDX_GROUPBOX_DETAILS].left + 7;
		int y = w->y + w->widgets[WIDX_GROUPBOX_DETAILS].top + 14;

		// Get map element
		rct_map_element *mapElement = map_get_first_element_at(window_tile_inspector_tile_x, window_tile_inspector_tile_y);
		mapElement += w->selected_list_item;

		switch (w->page) {
		case PAGE_SURFACE: {
			// Details
			// Terrain texture name
			rct_string_id terrain_name_id = TerrainTypes[map_element_get_terrain(mapElement)];
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_SURFACE_TERAIN, &terrain_name_id, 12, x, y);

			// Edge texture name
			rct_string_id terrain_edge_name_id = TerrainEdgeTypes[map_element_get_terrain_edge(mapElement)];
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_SURFACE_EDGE, &terrain_edge_name_id, 12, x, y + 11);

			// Land ownership
			rct_string_id land_ownership;
			if (mapElement->properties.surface.ownership & OWNERSHIP_OWNED) land_ownership = STR_LAND_OWNED;
			else if (mapElement->properties.surface.ownership & OWNERSHIP_AVAILABLE) land_ownership = STR_LAND_SALE;
			else if (mapElement->properties.surface.ownership & OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED) land_ownership = STR_CONSTRUCTION_RIGHTS_OWNED;
			else if (mapElement->properties.surface.ownership & OWNERSHIP_CONSTRUCTION_RIGHTS_AVAILABLE) land_ownership = STR_CONSTRUCTION_RIGHTS_SALE;
			else land_ownership = STR_TILE_INSPECTOR_LAND_NOT_OWNED_AND_NOT_AVAILABLE;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_SURFACE_OWNERSHIP, &land_ownership, 12, x, y + 22);

			// Water level
			int water_level = mapElement->properties.surface.terrain & MAP_ELEMENT_WATER_HEIGHT_MASK;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_SURFACE_WATER_LEVEL, &water_level, 12, x, y + 33);

			// Properties
			// Raise / lower label
			x = w->x + w->widgets[WIDX_GROUPBOX_DETAILS].left + 7;
			y = w->y + w->widgets[WIDX_SURFACE_SPINNER_HEIGHT].top;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_RAISE_LOWER, NULL, 12, x, y);

			// Current base height
			x = w->x + w->widgets[WIDX_SURFACE_SPINNER_HEIGHT].left + 3;
			int base_height = mapElement->base_height;
			gfx_draw_string_left(dpi, STR_FORMAT_INTEGER, &base_height, 12, x, y);

			// Raised corners
			x = w->x + w->widgets[WIDX_GROUPBOX_DETAILS].left + 7;
			y = w->y + w->widgets[WIDX_SURFACE_CHECK_CORNER_E].top;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_SURFACE_CORNERS, NULL, 12, x, y);
			break;
		}

		case PAGE_PATH: {
			// Details
			// Path name
			rct_string_id path_name_id = get_footpath_entry(footpath_element_get_type(mapElement))->string_idx;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_PATH_NAME, &path_name_id, 12, x, y);

			// Path addition
			if (footpath_element_has_path_scenery(mapElement)) {
				const uint8 pathAdditionType = footpath_element_get_path_scenery_index(mapElement);
				rct_string_id addition_name_id = get_footpath_item_entry(pathAdditionType)->name;
				gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_PATH_ADDITIONS, &addition_name_id, 12, x, y + 11);
			}
			else
				gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_PATH_ADDITIONS_NONE, NULL, 12, x, y + 11);

			// Properties
			// Raise / lower label
			x = w->x + w->widgets[WIDX_GROUPBOX_DETAILS].left + 7;
			y = w->y + w->widgets[WIDX_PATH_SPINNER_HEIGHT].top;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_RAISE_LOWER, NULL, 12, x, y);

			// Current base height
			x = w->x + w->widgets[WIDX_PATH_SPINNER_HEIGHT].left + 3;
			int base_height = mapElement->base_height;
			gfx_draw_string_left(dpi, STR_FORMAT_INTEGER, &base_height, 12, x, y);

			// Path connections
			x = w->x + w->widgets[WIDX_GROUPBOX_DETAILS].left + 7;
			y = w->y + w->widgets[WIDX_PATH_CHECK_EDGE_W].top;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_PATH_CONECTED_EDGES, NULL, 12, x, y);
			break;
		}

		case PAGE_TRACK: {
			// Details
			// Ride
			sint16 ride_id = mapElement->properties.track.ride_index;
			rct_ride *ride = get_ride(ride_id);
			rct_string_id ride_type = STR_RIDE_NAME_SPIRAL_ROLLER_COASTER + ride->type;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_TRACK_RIDE_TYPE, &ride_type, 12, x, y);
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_TRACK_RIDE_ID, &ride_id, 12, x, y + 11);
			set_format_arg(0, rct_string_id, ride->name);
			set_format_arg(0 + sizeof(rct_string_id), uint32, ride->name_arguments);
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_TRACK_RIDE_NAME, gCommonFormatArgs, 12, x, y + 22);
			// Track
			sint16 track_type = mapElement->properties.track.type;
			sint16 sequence_number = mapElement->properties.track.sequence & 0xF;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_TRACK_PIECE_ID, &track_type, 12, x, y + 33);
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_TRACK_SEQUENCE, &sequence_number, 12, x, y + 44);

			// Properties
			// Raise / lower label
			y = w->y + w->widgets[WIDX_TRACK_SPINNER_HEIGHT].top;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_RAISE_LOWER, NULL, 12, x, y);

			// Current base height
			x = w->x + w->widgets[WIDX_TRACK_SPINNER_HEIGHT].left + 3;
			int base_height = mapElement->base_height;
			gfx_draw_string_left(dpi, STR_FORMAT_INTEGER, &base_height, 12, x, y);
			break;
		}

		case PAGE_SCENERY: {
			// Details
			// Age
			sint16 age = mapElement->properties.scenery.age;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_SCENERY_AGE, &age, 12, x, y);

			// Quadrant value
			if (!(get_small_scenery_entry(mapElement->properties.scenery.type)->small_scenery.flags & SMALL_SCENERY_FLAG_FULL_TILE)) {
				sint16 quadrant = (mapElement->type & MAP_ELEMENT_QUADRANT_MASK) >> 6;
				static rct_string_id quadrant_string_idx[] = {
					STR_TILE_INSPECTOR_SCENERY_QUADRANT_SW,
					STR_TILE_INSPECTOR_SCENERY_QUADRANT_NW,
					STR_TILE_INSPECTOR_SCENERY_QUADRANT_NE,
					STR_TILE_INSPECTOR_SCENERY_QUADRANT_SE
				};
				gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_SCENERY_QUADRANT, &quadrant_string_idx[quadrant], 12, x, y + 11);
			}

			// Scenery ID
			sint16 idx = mapElement->properties.scenery.type;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_SCENERY_ENTRY_IDX, &idx, 12, x, y + 22);

			// Properties
			// Raise / Lower
			y = w->y + w->widgets[WIDX_SCENERY_SPINNER_HEIGHT].top;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_RAISE_LOWER, NULL, 12, x, y);

			// Current base height
			x = w->x + w->widgets[WIDX_SCENERY_SPINNER_HEIGHT].left + 3;
			int base_height = mapElement->base_height;
			gfx_draw_string_left(dpi, STR_FORMAT_INTEGER, &base_height, 12, x, y);

			// Quarter tile
			x = w->x + w->widgets[WIDX_GROUPBOX_DETAILS].left + 7;
			y = w->y + w->widgets[WIDX_SCENERY_CHECK_QUARTER_E].top;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_SCENERY_QUADRANT_LABEL, NULL, 12, x, y);

			// Collision
			y = w->y + w->widgets[WIDX_SCENERY_CHECK_COLLISION_E].top;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_COLLISSION, NULL, 12, x, y);
			break;
		}

		case PAGE_FENCE: {
			// Properties
			// Raise / lower label
			y = w->y + w->widgets[WIDX_FENCE_SPINNER_HEIGHT].top;
			gfx_draw_string_left(dpi, STR_TILE_INSPECTOR_RAISE_LOWER, NULL, 12, x, y);

			// Current base height
			x = w->x + w->widgets[WIDX_FENCE_SPINNER_HEIGHT].left + 3;
			int base_height = mapElement->base_height;
			gfx_draw_string_left(dpi, STR_FORMAT_INTEGER, &base_height, 12, x, y);
			break;
		}
		} // switch page
	}
}

static void window_tile_inspector_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex) {
	int x = 3;
	int y = LIST_ITEM_HEIGHT * (window_tile_inspector_item_count - 1);
	int i = 0;
	char buffer[256];

	if (window_tile_inspector_tile_x == -1)
		return;

	rct_map_element *mapElement = map_get_first_element_at(window_tile_inspector_tile_x, window_tile_inspector_tile_y);

	gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
	do {
		int type = map_element_get_type(mapElement);
		char *type_name;
		int base_height = mapElement->base_height;
		int clearance_height = mapElement->clearance_height;

		// Fill colour for current list element
		const int list_width = w->widgets[WIDX_LIST].right - w->widgets[WIDX_LIST].left;
		if (i == w->selected_list_item) // Currently selected element
			gfx_fill_rect(dpi, 0, y, list_width, y + LIST_ITEM_HEIGHT - 1, ColourMapA[w->colours[1]].darker | 0x1000000);
		else if (i == window_tile_inspector_highlighted_index) // Hovering
			gfx_fill_rect(dpi, 0, y, list_width, y + LIST_ITEM_HEIGHT - 1, ColourMapA[w->colours[1]].mid_dark | 0x1000000);
		else if ((i & 1) != 0) // odd / even check
			gfx_fill_rect(dpi, 0, y, list_width, y + LIST_ITEM_HEIGHT - 1, ColourMapA[w->colours[1]].lighter | 0x1000000);

		switch (type) {
		case MAP_ELEMENT_TYPE_SURFACE:
			type_name = "Surface";
			break;
		case MAP_ELEMENT_TYPE_PATH:
			type_name = footpath_element_is_queue(mapElement) ? "Queue" : "Footpath";
			break;
		case MAP_ELEMENT_TYPE_TRACK:
			type_name = "Track";
			break;
		case MAP_ELEMENT_TYPE_SCENERY:
			snprintf(
				buffer, sizeof(buffer),
				"Scenery (%s)",
				language_get_string(get_small_scenery_entry(mapElement->properties.scenery.type)->name)
			);
			type_name = buffer;
			break;
		case MAP_ELEMENT_TYPE_ENTRANCE:
			snprintf(
				buffer, sizeof(buffer),
				"Entrance (%s)",
				language_get_string(EntranceTypes[mapElement->properties.entrance.type])
			);
			type_name = buffer;
			break;
		case MAP_ELEMENT_TYPE_FENCE:
			snprintf(
				buffer, sizeof(buffer),
				"Fence (%s)",
				language_get_string(get_wall_entry(mapElement->properties.scenery.type)->name)
			);
			type_name = buffer;
			break;
		case MAP_ELEMENT_TYPE_SCENERY_MULTIPLE:
			type_name = "Scenery multiple";
			break;
		case MAP_ELEMENT_TYPE_BANNER:
			snprintf(
				buffer, sizeof(buffer),
				"Banner (%d)",
				mapElement->properties.banner.index
			);
			type_name = buffer;
			break;
		case MAP_ELEMENT_TYPE_CORRUPT:
			// fall-through
		default:
			snprintf(buffer, sizeof(buffer), "Unknown (type %d)", type);
			type_name = buffer;
		}

		// Undo relative scroll offset, but keep the 3 pixel padding
		x = -w->widgets[WIDX_LIST].left;
		const bool ghost = (mapElement->flags & MAP_ELEMENT_FLAG_GHOST) != 0;
		const bool broken = (mapElement->flags & MAP_ELEMENT_FLAG_BROKEN) != 0;
		const bool last = (mapElement->flags & MAP_ELEMENT_FLAG_LAST_TILE) != 0;
		gfx_draw_string(dpi, type_name, 12, x + COL_X_TYPE + 3, y); // 3px padding
		gfx_draw_string_left(dpi, STR_FORMAT_INTEGER, &base_height, 12, x + COL_X_BH, y);
		gfx_draw_string_left(dpi, STR_FORMAT_INTEGER, &clearance_height, 12, x + COL_X_CH, y);
		if (ghost) gfx_draw_string(dpi, (char*)CheckBoxMarkString, w->colours[1], x + COL_X_GF, y);
		if (broken) gfx_draw_string(dpi, (char*)CheckBoxMarkString, w->colours[1], x + COL_X_BF, y);
		if (last) gfx_draw_string(dpi, (char*)CheckBoxMarkString, w->colours[1], x + COL_X_LF, y);

		y -= LIST_ITEM_HEIGHT;
		i++;
	} while (!map_element_is_last_for_tile(mapElement++));
}
