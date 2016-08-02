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

#include "../addresses.h"
#include "../audio/audio.h"
#include "../cheats.h"
#include "../config.h"
#include "../game.h"
#include "../interface/viewport.h"
#include "../localisation/localisation.h"
#include "../management/finance.h"
#include "../network/network.h"
#include "../platform/platform.h"
#include "../rct1.h"
#include "../util/sawyercoding.h"
#include "../util/util.h"
#include "../world/map_animation.h"
#include "../world/park.h"
#include "../world/scenery.h"
#include "../world/footpath.h"
#include "../windows/error.h"
#include "ride.h"
#include "ride_data.h"
#include "ride_ratings.h"
#include "track.h"
#include "track_data.h"

uint8 gTrackGroundFlags;

/**
 *
 *  rct2: 0x00997C9D
 */
const rct_trackdefinition *gTrackDefinitions = (rct_trackdefinition*)0x00997C9D;
const rct_trackdefinition *gFlatRideTrackDefinitions = (rct_trackdefinition*)0x0099849D;

// TODO This table is incorrect or at least missing 69 elements. There should be 256 in total!
const rct_trackdefinition gTrackDefinitions_INCORRECT[] = {
	// TYPE							VANGLE END					VANGLE START				BANK END				BANK START				SPECIAL
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_FLAT
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_END_STATION
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_BEGIN_STATION
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_MIDDLE_STATION
	{ TRACK_FLAT,					TRACK_SLOPE_UP_25,			TRACK_SLOPE_UP_25,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_25_DEG_UP
	{ TRACK_FLAT,					TRACK_SLOPE_UP_60,			TRACK_SLOPE_UP_60,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_60_DEG_UP
	{ TRACK_FLAT,					TRACK_SLOPE_UP_25,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_FLAT_TO_25_DEG_UP
	{ TRACK_FLAT,					TRACK_SLOPE_UP_60,			TRACK_SLOPE_UP_25,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_25_DEG_UP_TO_60_DEG_UP
	{ TRACK_FLAT,					TRACK_SLOPE_UP_25,			TRACK_SLOPE_UP_60,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_60_DEG_UP_TO_25_DEG_UP
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_UP_25,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_25_DEG_UP_TO_FLAT
	{ TRACK_FLAT,					TRACK_SLOPE_DOWN_25,		TRACK_SLOPE_DOWN_25,		TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_25_DEG_DOWN
	{ TRACK_FLAT,					TRACK_SLOPE_DOWN_60,		TRACK_SLOPE_DOWN_60,		TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_60_DEG_DOWN
	{ TRACK_FLAT,					TRACK_SLOPE_DOWN_25,		TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_FLAT_TO_25_DEG_DOWN
	{ TRACK_FLAT,					TRACK_SLOPE_DOWN_60,		TRACK_SLOPE_DOWN_25,		TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_25_DEG_DOWN_TO_60_DEG_DOWN
	{ TRACK_FLAT,					TRACK_SLOPE_DOWN_25,		TRACK_SLOPE_DOWN_60,		TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_60_DEG_DOWN_TO_25_DEG_DOWN
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_DOWN_25,		TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_25_DEG_DOWN_TO_FLAT
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_QUARTER_TURN_5_TILES
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_QUARTER_TURN_5_TILES
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_LEFT,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_FLAT_TO_LEFT_BANK
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_RIGHT,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_FLAT_TO_RIGHT_BANK
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_LEFT_BANK_TO_FLAT
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_RIGHT_BANK_TO_FLAT
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_LEFT,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_BANKED_LEFT_QUARTER_TURN_5_TILES
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_RIGHT,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_BANKED_RIGHT_QUARTER_TURN_5_TILES
	{ TRACK_FLAT,					TRACK_SLOPE_UP_25,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_LEFT_BANK_TO_25_DEG_UP
	{ TRACK_FLAT,					TRACK_SLOPE_UP_25,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_RIGHT_BANK_TO_25_DEG_UP
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_UP_25,			TRACK_BANK_LEFT,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_25_DEG_UP_TO_LEFT_BANK
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_UP_25,			TRACK_BANK_RIGHT,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_25_DEG_UP_TO_RIGHT_BANK
	{ TRACK_FLAT,					TRACK_SLOPE_DOWN_25,		TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_LEFT_BANK_TO_25_DEG_DOWN
	{ TRACK_FLAT,					TRACK_SLOPE_DOWN_25,		TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_RIGHT_BANK_TO_25_DEG_DOWN
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_DOWN_25,		TRACK_BANK_LEFT,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_25_DEG_DOWN_TO_LEFT_BANK
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_DOWN_25,		TRACK_BANK_RIGHT,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_25_DEG_DOWN_TO_RIGHT_BANK
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_LEFT,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_LEFT_BANK
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_RIGHT,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_RIGHT_BANK
	{ TRACK_FLAT,					TRACK_SLOPE_UP_25,			TRACK_SLOPE_UP_25,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_UP
	{ TRACK_FLAT,					TRACK_SLOPE_UP_25,			TRACK_SLOPE_UP_25,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_UP
	{ TRACK_FLAT,					TRACK_SLOPE_DOWN_25,		TRACK_SLOPE_DOWN_25,		TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_DOWN
	{ TRACK_FLAT,					TRACK_SLOPE_DOWN_25,		TRACK_SLOPE_DOWN_25,		TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_DOWN
	{ TRACK_S_BEND,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_S_BEND_LEFT
	{ TRACK_S_BEND,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_S_BEND_RIGHT
	{ TRACK_VERTICAL_LOOP,			TRACK_SLOPE_DOWN_25,		TRACK_SLOPE_UP_25,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_UNKNOWN_VERTICAL_LOOP	},	// ELEM_LEFT_VERTICAL_LOOP
	{ TRACK_VERTICAL_LOOP,			TRACK_SLOPE_DOWN_25,		TRACK_SLOPE_UP_25,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_UNKNOWN_VERTICAL_LOOP	},	// ELEM_RIGHT_VERTICAL_LOOP
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_QUARTER_TURN_3_TILES
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_QUARTER_TURN_3_TILES
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_LEFT,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_LEFT_QUARTER_TURN_3_TILES_BANK
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_RIGHT,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_RIGHT_QUARTER_TURN_3_TILES_BANK
	{ TRACK_FLAT,					TRACK_SLOPE_UP_25,			TRACK_SLOPE_UP_25,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP
	{ TRACK_FLAT,					TRACK_SLOPE_UP_25,			TRACK_SLOPE_UP_25,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP
	{ TRACK_FLAT,					TRACK_SLOPE_DOWN_25,		TRACK_SLOPE_DOWN_25,		TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN
	{ TRACK_FLAT,					TRACK_SLOPE_DOWN_25,		TRACK_SLOPE_DOWN_25,		TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_QUARTER_TURN_1_TILE
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_QUARTER_TURN_1_TILE
	{ TRACK_TWIST,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_UPSIDE_DOWN,	TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_TWIST_DOWN_TO_UP
	{ TRACK_TWIST,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_UPSIDE_DOWN,	TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_TWIST_DOWN_TO_UP
	{ TRACK_TWIST,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_UPSIDE_DOWN,	TRACK_NONE					},	// ELEM_LEFT_TWIST_UP_TO_DOWN
	{ TRACK_TWIST,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_UPSIDE_DOWN,	TRACK_NONE					},	// ELEM_RIGHT_TWIST_UP_TO_DOWN
	{ TRACK_HALF_LOOP,				TRACK_SLOPE_NONE,			TRACK_SLOPE_UP_25,			TRACK_BANK_UPSIDE_DOWN,	TRACK_BANK_NONE,		TRACK_HALF_LOOP_UP			},	// ELEM_HALF_LOOP_UP
	{ TRACK_HALF_LOOP,				TRACK_SLOPE_DOWN_25,		TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_UPSIDE_DOWN,	TRACK_HALF_LOOP_DOWN		},	// ELEM_HALF_LOOP_DOWN
	{ TRACK_CORKSCREW,				TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_UPSIDE_DOWN,	TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_CORKSCREW_UP
	{ TRACK_CORKSCREW,				TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_UPSIDE_DOWN,	TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_CORKSCREW_UP
	{ TRACK_CORKSCREW,				TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_UPSIDE_DOWN,	TRACK_CORKSCREW_DOWN		},	// ELEM_LEFT_CORKSCREW_DOWN
	{ TRACK_CORKSCREW,				TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_UPSIDE_DOWN,	TRACK_CORKSCREW_DOWN		},	// ELEM_RIGHT_CORKSCREW_DOWN
	{ TRACK_FLAT,					TRACK_SLOPE_UP_60,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_FLAT_TO_60_DEG_UP
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_UP_60,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_60_DEG_UP_TO_FLAT
	{ TRACK_FLAT,					TRACK_SLOPE_DOWN_60,		TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_FLAT_TO_60_DEG_DOWN
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_DOWN_60,		TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_60_DEG_DOWN_TO_FLAT
	{ TRACK_TOWER_BASE,				TRACK_VANGLE_TOWER,			TRACK_VANGLE_TOWER,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_TOWER_BASE
	{ TRACK_TOWER_BASE,				TRACK_VANGLE_TOWER,			TRACK_VANGLE_TOWER,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_TOWER_SECTION
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_FLAT_COVERED
	{ TRACK_FLAT,					TRACK_SLOPE_UP_25,			TRACK_SLOPE_UP_25,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_25_DEG_UP_COVERED
	{ TRACK_FLAT,					TRACK_SLOPE_UP_60,			TRACK_SLOPE_UP_60,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_60_DEG_UP_COVERED
	{ TRACK_FLAT,					TRACK_SLOPE_UP_25,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_FLAT_TO_25_DEG_UP_COVERED
	{ TRACK_FLAT,					TRACK_SLOPE_UP_60,			TRACK_SLOPE_UP_25,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_25_DEG_UP_TO_60_DEG_UP_COVERED
	{ TRACK_FLAT,					TRACK_SLOPE_UP_25,			TRACK_SLOPE_UP_60,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_60_DEG_UP_TO_25_DEG_UP_COVERED
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_UP_25,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_25_DEG_UP_TO_FLAT_COVERED
	{ TRACK_FLAT,					TRACK_SLOPE_DOWN_25,		TRACK_SLOPE_DOWN_25,		TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_25_DEG_DOWN_COVERED
	{ TRACK_FLAT,					TRACK_SLOPE_DOWN_60,		TRACK_SLOPE_DOWN_60,		TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_60_DEG_DOWN_COVERED
	{ TRACK_FLAT,					TRACK_SLOPE_DOWN_25,		TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_FLAT_TO_25_DEG_DOWN_COVERED
	{ TRACK_FLAT,					TRACK_SLOPE_DOWN_60,		TRACK_SLOPE_DOWN_25,		TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_25_DEG_DOWN_TO_60_DEG_DOWN_COVERED
	{ TRACK_FLAT,					TRACK_SLOPE_DOWN_25,		TRACK_SLOPE_DOWN_60,		TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_60_DEG_DOWN_TO_25_DEG_DOWN_COVERED
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_DOWN_25,		TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_25_DEG_DOWN_TO_FLAT_COVERED
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_QUARTER_TURN_5_TILES_COVERED
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_QUARTER_TURN_5_TILES_COVERED
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_S_BEND_LEFT_COVERED
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_S_BEND_RIGHT_COVERED
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_QUARTER_TURN_3_TILES_COVERED
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_QUARTER_TURN_3_TILES_COVERED
	{ TRACK_HELIX_SMALL,			TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_LEFT,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_LEFT_HALF_BANKED_HELIX_UP_SMALL
	{ TRACK_HELIX_SMALL,			TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_RIGHT,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_RIGHT_HALF_BANKED_HELIX_UP_SMALL
	{ TRACK_HELIX_SMALL,			TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_LEFT,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_LEFT_HALF_BANKED_HELIX_DOWN_SMALL
	{ TRACK_HELIX_SMALL,			TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_RIGHT,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_SMALL
	{ TRACK_HELIX_SMALL,			TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_LEFT,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_LEFT_HALF_BANKED_HELIX_UP_LARGE
	{ TRACK_HELIX_SMALL,			TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_RIGHT,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_RIGHT_HALF_BANKED_HELIX_UP_LARGE
	{ TRACK_HELIX_SMALL,			TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_LEFT,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_LEFT_HALF_BANKED_HELIX_DOWN_LARGE
	{ TRACK_HELIX_SMALL,			TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_RIGHT,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_LARGE
	{ TRACK_FLAT,					TRACK_SLOPE_UP_60,			TRACK_SLOPE_UP_60,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_UP
	{ TRACK_FLAT,					TRACK_SLOPE_UP_60,			TRACK_SLOPE_UP_60,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_UP
	{ TRACK_FLAT,					TRACK_SLOPE_DOWN_60,		TRACK_SLOPE_DOWN_60,		TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_DOWN
	{ TRACK_FLAT,					TRACK_SLOPE_DOWN_60,		TRACK_SLOPE_DOWN_60,		TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_DOWN
	{ TRACK_BRAKES,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_BRAKES
	{ TRACK_ROTATION_CONTROL_TOGGLE,TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_ROTATION_CONTROL_TOGGLE
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_INVERTED_90_DEG_UP_TO_FLAT_QUARTER_LOOP
	{ TRACK_HELIX_LARGE,			TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_LEFT,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_UP
	{ TRACK_HELIX_LARGE,			TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_RIGHT,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_UP
	{ TRACK_HELIX_LARGE,			TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_LEFT,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_DOWN
	{ TRACK_HELIX_LARGE,			TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_RIGHT,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_DOWN
	{ TRACK_HELIX_LARGE_UNBANKED,	TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_QUARTER_HELIX_LARGE_UP
	{ TRACK_HELIX_LARGE_UNBANKED,	TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_QUARTER_HELIX_LARGE_UP
	{ TRACK_HELIX_LARGE_UNBANKED,	TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_QUARTER_HELIX_LARGE_DOWN
	{ TRACK_HELIX_LARGE_UNBANKED,	TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_QUARTER_HELIX_LARGE_DOWN
	{ TRACK_FLAT,					TRACK_SLOPE_UP_25,			TRACK_SLOPE_UP_25,			TRACK_BANK_LEFT,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_25_DEG_UP_LEFT_BANKED
	{ TRACK_FLAT,					TRACK_SLOPE_UP_25,			TRACK_SLOPE_UP_25,			TRACK_BANK_RIGHT,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_25_DEG_UP_RIGHT_BANKED
	{ TRACK_WATERFALL,				TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_WATERFALL
	{ TRACK_RAPIDS,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RAPIDS
	{ TRACK_ON_RIDE_PHOTO,			TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_ON_RIDE_PHOTO
	{ TRACK_FLAT,					TRACK_SLOPE_DOWN_25,		TRACK_SLOPE_DOWN_25,		TRACK_BANK_LEFT,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_25_DEG_DOWN_LEFT_BANKED
	{ TRACK_FLAT,					TRACK_SLOPE_DOWN_25,		TRACK_SLOPE_DOWN_25,		TRACK_BANK_RIGHT,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_25_DEG_DOWN_RIGHT_BANKED
	{ TRACK_WATER_SPLASH,			TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_WATER_SPLASH
	{ TRACK_FLAT,					TRACK_SLOPE_UP_60,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_FLAT_TO_60_DEG_UP_LONG_BASE
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_UP_60,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_60_DEG_UP_TO_FLAT_LONG_BASE
	{ TRACK_WHIRLPOOL,				TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_WHIRLPOOL
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_DOWN_60,		TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_FLAT_TO_60_DEG_DOWN_LONG_BASE
	{ TRACK_FLAT,					TRACK_SLOPE_DOWN_60,		TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_60_DEG_UP_TO_FLAT_LONG_BASE
	{ TRACK_LIFT_HILL,				TRACK_SLOPE_DOWN_60,		TRACK_SLOPE_UP_25,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_CABLE_LIFT_HILL
	{ TRACK_WHOA_BELLY,				TRACK_VANGLE_WHOA_BELLY,	TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_REVERSE_WHOA_BELLY_SLOPE
	{ TRACK_WHOA_BELLY,				TRACK_VANGLE_WHOA_BELLY,	TRACK_VANGLE_WHOA_BELLY,	TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_REVERSE_WHOA_BELLY_VERTICAL
	{ TRACK_FLAT,					TRACK_SLOPE_UP_90,			TRACK_SLOPE_UP_90,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_90_DEG_UP
	{ TRACK_FLAT,					TRACK_SLOPE_DOWN_90,		TRACK_SLOPE_DOWN_90,		TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_90_DEG_DOWN
	{ TRACK_FLAT,					TRACK_SLOPE_UP_90,			TRACK_SLOPE_UP_60,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_60_DEG_UP_TO_90_DEG_UP
	{ TRACK_FLAT,					TRACK_SLOPE_DOWN_60,		TRACK_SLOPE_DOWN_90,		TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_90_DEG_DOWN_TO_60_DEG_DOWN
	{ TRACK_FLAT,					TRACK_SLOPE_UP_60,			TRACK_SLOPE_UP_90,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_90_DEG_UP_TO_60_DEG_UP
	{ TRACK_FLAT,					TRACK_SLOPE_DOWN_90,		TRACK_SLOPE_DOWN_60,		TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_60_DEG_DOWN_TO_90_DEG_DOWN
	{ TRACK_BRAKE_FOR_DROP,			TRACK_SLOPE_DOWN_60,		TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_BRAKE_FOR_DROP
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_EIGHTH_TO_DIAG
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_EIGHTH_TO_DIAG
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_EIGHTH_TO_ORTHOGONAL
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_LEFT,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_LEFT_EIGHTH_BANK_TO_DIAG
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_RIGHT,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_RIGHT_EIGHTH_BANK_TO_DIAG
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_LEFT,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_LEFT_EIGHTH_BANK_TO_ORTHOGONAL
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_RIGHT,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_RIGHT_EIGHTH_BANK_TO_ORTHOGONAL
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_FLAT
	{ TRACK_FLAT,					TRACK_SLOPE_UP_25,			TRACK_SLOPE_UP_25,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_25_DEG_UP
	{ TRACK_FLAT,					TRACK_SLOPE_UP_60,			TRACK_SLOPE_UP_60,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_60_DEG_UP
	{ TRACK_FLAT,					TRACK_SLOPE_UP_25,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_FLAT_TO_25_DEG_UP
	{ TRACK_FLAT,					TRACK_SLOPE_UP_60,			TRACK_SLOPE_UP_25,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_25_DEG_UP_TO_60_DEG_UP
	{ TRACK_FLAT,					TRACK_SLOPE_UP_25,			TRACK_SLOPE_UP_60,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_60_DEG_UP_TO_25_DEG_UP
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_UP_25,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_25_DEG_UP_TO_FLAT
	{ TRACK_FLAT,					TRACK_SLOPE_DOWN_25,		TRACK_SLOPE_DOWN_25,		TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_25_DEG_DOWN
	{ TRACK_FLAT,					TRACK_SLOPE_DOWN_60,		TRACK_SLOPE_DOWN_60,		TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_60_DEG_DOWN
	{ TRACK_FLAT,					TRACK_SLOPE_DOWN_25,		TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_FLAT_TO_25_DEG_DOWN
	{ TRACK_FLAT,					TRACK_SLOPE_DOWN_60,		TRACK_SLOPE_DOWN_25,		TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_25_DEG_DOWN_TO_60_DEG_DOWN
	{ TRACK_FLAT,					TRACK_SLOPE_DOWN_25,		TRACK_SLOPE_DOWN_60,		TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_60_DEG_DOWN_TO_25_DEG_DOWN
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_DOWN_25,		TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_25_DEG_DOWN_TO_FLAT
	{ TRACK_FLAT,					TRACK_SLOPE_UP_60,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_FLAT_TO_60_DEG_UP
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_UP_60,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_60_DEG_UP_TO_FLAT
	{ TRACK_FLAT,					TRACK_SLOPE_DOWN_60,		TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_FLAT_TO_60_DEG_DOWN
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_DOWN_60,		TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_60_DEG_DOWN_TO_FLAT
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_LEFT,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_FLAT_TO_LEFT_BANK
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_RIGHT,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_FLAT_TO_RIGHT_BANK
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_DIAG_LEFT_BANK_TO_FLAT
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_DIAG_RIGHT_BANK_TO_FLAT
	{ TRACK_FLAT,					TRACK_SLOPE_UP_25,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_DIAG_LEFT_BANK_TO_25_DEG_UP
	{ TRACK_FLAT,					TRACK_SLOPE_UP_25,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_DIAG_RIGHT_BANK_TO_25_DEG_UP
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_UP_25,			TRACK_BANK_LEFT,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_25_DEG_UP_TO_LEFT_BANK
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_UP_25,			TRACK_BANK_RIGHT,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_25_DEG_UP_TO_RIGHT_BANK
	{ TRACK_FLAT,					TRACK_SLOPE_DOWN_25,		TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_DIAG_LEFT_BANK_TO_25_DEG_DOWN
	{ TRACK_FLAT,					TRACK_SLOPE_DOWN_25,		TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_DIAG_RIGHT_BANK_TO_25_DEG_DOWN
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_DOWN_25,		TRACK_BANK_LEFT,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_25_DEG_DOWN_TO_LEFT_BANK
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_DOWN_25,		TRACK_BANK_RIGHT,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_25_DEG_DOWN_TO_RIGHT_BANK
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_LEFT,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_DIAG_LEFT_BANK
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_RIGHT,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_DIAG_RIGHT_BANK
	{ TRACK_LOG_FLUME_REVERSER,		TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LOG_FLUME_REVERSER
	{ TRACK_SPINNING_TUNNEL,		TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_SPINNING_TUNNEL
	{ TRACK_BARREL_ROLL,			TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_UPSIDE_DOWN,	TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_BARREL_ROLL_UP_TO_DOWN
	{ TRACK_BARREL_ROLL,			TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_UPSIDE_DOWN,	TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_BARREL_ROLL_UP_TO_DOWN
	{ TRACK_BARREL_ROLL,			TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_UPSIDE_DOWN,	TRACK_NONE					},	// ELEM_LEFT_BARREL_ROLL_DOWN_TO_UP
	{ TRACK_BARREL_ROLL,			TRACK_SLOPE_NONE,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_UPSIDE_DOWN,	TRACK_NONE					},	// ELEM_RIGHT_BARREL_ROLL_DOWN_TO_UP
	{ TRACK_FLAT,					TRACK_SLOPE_UP_25,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_LEFT_BANK_TO_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP
	{ TRACK_FLAT,					TRACK_SLOPE_UP_25,			TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_RIGHT_BANK_TO_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_DOWN_25,		TRACK_BANK_LEFT,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_LEFT_BANK
	{ TRACK_FLAT,					TRACK_SLOPE_NONE,			TRACK_SLOPE_DOWN_25,		TRACK_BANK_RIGHT,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_RIGHT_BANK
	{ TRACK_POWERED_LIFT,			TRACK_SLOPE_UP_25,			TRACK_SLOPE_UP_25,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_POWERED_LIFT
	{ TRACK_HALF_LOOP_LARGE,		TRACK_SLOPE_NONE,			TRACK_SLOPE_UP_25,			TRACK_BANK_UPSIDE_DOWN,	TRACK_BANK_NONE,		TRACK_HALF_LOOP_UP			},	// ELEM_LEFT_LARGE_HALF_LOOP_UP
	{ TRACK_HALF_LOOP_LARGE,		TRACK_SLOPE_NONE,			TRACK_SLOPE_UP_25,			TRACK_BANK_UPSIDE_DOWN,	TRACK_BANK_NONE,		TRACK_HALF_LOOP_UP			},	// ELEM_RIGHT_LARGE_HALF_LOOP_UP
	{ TRACK_HALF_LOOP_LARGE,		TRACK_SLOPE_DOWN_25,		TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_UPSIDE_DOWN,	TRACK_HALF_LOOP_DOWN		},	// ELEM_RIGHT_LARGE_HALF_LOOP_DOWN
	{ TRACK_HALF_LOOP_LARGE,		TRACK_SLOPE_DOWN_25,		TRACK_SLOPE_NONE,			TRACK_BANK_NONE,		TRACK_BANK_UPSIDE_DOWN,	TRACK_HALF_LOOP_DOWN		},	// ELEM_LEFT_LARGE_HALF_LOOP_DOWN
};

/**
 * Helper method to determine if a connects to b by its bank and angle, not location.
 */
int track_is_connected_by_shape(rct_map_element *a, rct_map_element *b)
{
	int trackType, aBank, aAngle, bBank, bAngle;

	trackType = a->properties.track.type;
	aBank = gTrackDefinitions[trackType].bank_end;
	aAngle = gTrackDefinitions[trackType].vangle_end;
	aBank = track_get_actual_bank(a, aBank);

	trackType = b->properties.track.type;
	bBank = gTrackDefinitions[trackType].bank_start;
	bAngle = gTrackDefinitions[trackType].vangle_start;
	bBank = track_get_actual_bank(b, bBank);

	return aBank == bBank && aAngle == bAngle;
}

const rct_preview_track *get_track_def_from_ride(rct_ride *ride, int trackType)
{
	return ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE) ?
		FlatRideTrackBlocks[trackType] :
		TrackBlocks[trackType];
}

const rct_track_coordinates *get_track_coord_from_ride(rct_ride *ride, int trackType){
	return ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE) ?
		&FlatTrackCoordinates[trackType] :
		&TrackCoordinates[trackType];
}

const rct_preview_track *get_track_def_from_ride_index(int rideIndex, int trackType)
{
	return get_track_def_from_ride(get_ride(rideIndex), trackType);
}

static rct_map_element *find_station_element(int x, int y, int z, int direction, int rideIndex)
{
	rct_map_element *mapElement = map_get_first_element_at(x >> 5, y >> 5);
	do {
		if (z != mapElement->base_height) continue;
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK) continue;
		if (map_element_get_direction(mapElement) != direction) continue;
		if (mapElement->properties.track.ride_index != rideIndex) continue;
		if (!track_element_is_station(mapElement)) continue;

		return mapElement;
	} while (!map_element_is_last_for_tile(mapElement++));
	return NULL;
}

static void ride_remove_station(rct_ride *ride, int x, int y, int z)
{
	uint16 xy = (x >> 5) | ((y >> 5) << 8);
	for (int i = 0; i < 4; i++) {
		if (ride->station_starts[i] == xy && ride->station_heights[i] == z) {
			ride->station_starts[i] = 0xFFFF;
			ride->num_stations--;
			break;
		}
	}
}

/**
 *
 *  rct2: 0x006C4D89
 */
static bool track_add_station_element(int x, int y, int z, int direction, int rideIndex, int flags)
{
	int stationX0 = x;
	int stationY0 = y;
	int stationX1 = x;
	int stationY1 = y;
	int stationLength = 1;

	rct_ride *ride = get_ride(rideIndex);
	if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_3)) {
		if (ride->num_stations >= 4) {
			gGameCommandErrorText = STR_NO_MORE_STATIONS_ALLOWED_ON_THIS_RIDE;
			return false;
		}
		if (flags & GAME_COMMAND_FLAG_APPLY) {
			int stationIndex = -1;
			for (int i = 0; i < 4; i++) {
				if (ride->station_starts[i] == 0xFFFF) {
					stationIndex = i;
					break;
				}
			}

			assert(stationIndex != -1);

			ride->station_starts[stationIndex] = (x >> 5) | ((y >> 5) << 8);
			ride->station_heights[stationIndex] = z;
			ride->station_depart[stationIndex] = 1;
			ride->station_length[stationIndex] = 0;
			ride->num_stations++;
		}
		return true;
	}

	rct_map_element *stationElement;

	// Search backwards for more station
	x = stationX0;
	y = stationY0;
	do {
		x -= TileDirectionDelta[direction].x;
		y -= TileDirectionDelta[direction].y;

		stationElement = find_station_element(x, y, z, direction, rideIndex);
		if (stationElement != NULL) {
			if (stationElement->properties.track.type == TRACK_ELEM_END_STATION) {
				if (flags & GAME_COMMAND_FLAG_APPLY) {
					ride_remove_station(ride, x, y, z);
				}
			}

			stationX0 = x;
			stationY0 = y;
			stationLength++;
		}
	} while (stationElement != NULL);

	// Search forwards for more station
	x = stationX1;
	y = stationY1;
	do {
		x += TileDirectionDelta[direction].x;
		y += TileDirectionDelta[direction].y;

		stationElement = find_station_element(x, y, z, direction, rideIndex);
		if (stationElement != NULL) {
			if (stationElement->properties.track.type == TRACK_ELEM_END_STATION) {
				if (flags & GAME_COMMAND_FLAG_APPLY) {
					ride_remove_station(ride, x, y, z);
				}
			}

			stationX1 = x;
			stationY1 = y;
			stationLength++;
		}
	} while (stationElement != NULL);

	if (stationX0 == stationX1 && stationY0 == stationY1 && ride->num_stations >= 4) {
		gGameCommandErrorText = STR_NO_MORE_STATIONS_ALLOWED_ON_THIS_RIDE;
		return false;
	}

	if (stationLength > MAX_STATION_PLATFORM_LENGTH) {
		gGameCommandErrorText = STR_STATION_PLATFORM_TOO_LONG;
		return false;
	}

	if (flags & GAME_COMMAND_FLAG_APPLY) {
		x = stationX1;
		y = stationY1;

		bool finaliseStationDone;
		do {
			finaliseStationDone = true;

			stationElement = find_station_element(x, y, z, direction, rideIndex);
			if (stationElement != NULL) {
				int targetTrackType;
				if (x == stationX1 && y == stationY1) {
					int stationIndex = -1;
					for (int i = 0; i < 4; i++) {
						if (ride->station_starts[i] == 0xFFFF) {
							stationIndex = i;
							break;
						}
					}

					assert(stationIndex != -1);

					uint16 xy = (x >> 5) | ((y >> 5) << 8);
					ride->station_starts[stationIndex] = xy;
					ride->station_heights[stationIndex] = z;
					ride->station_depart[stationIndex] = 1;
					ride->station_length[stationIndex] = stationLength;
					ride->num_stations++;

					targetTrackType = TRACK_ELEM_END_STATION;
				} else if (x == stationX0 && y == stationY0) {
					targetTrackType = TRACK_ELEM_BEGIN_STATION;
				} else {
					targetTrackType = TRACK_ELEM_MIDDLE_STATION;
				}
				stationElement->properties.track.type = targetTrackType;

				map_invalidate_element(x, y, stationElement);

				if (x != stationX0 || y != stationY0) {
					x -= TileDirectionDelta[direction].x;
					y -= TileDirectionDelta[direction].y;
					finaliseStationDone = false;
				}
			}
		} while (!finaliseStationDone);
	}
	return true;
}

/**
 *
 *  rct2: 0x006C494B
 */
static bool track_remove_station_element(int x, int y, int z, int direction, int rideIndex, int flags)
{
	int removeX = x;
	int removeY = y;
	int stationX0 = x;
	int stationY0 = y;
	int stationX1 = x;
	int stationY1 = y;
	int stationLength = 0;
	int byte_F441D1 = -1;

	rct_ride *ride = get_ride(rideIndex);
	if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_3)) {
		rct_map_element *mapElement = map_get_track_element_at_with_direction_from_ride(x, y, z, direction, rideIndex);
		if (mapElement != NULL) {
			if (flags & GAME_COMMAND_FLAG_APPLY) {
				ride_remove_station(ride, x, y, z);
			}
		}
		return true;
	}

	rct_map_element *stationElement;

	// Search backwards for more station
	x = stationX0;
	y = stationY0;
	while ((stationElement = find_station_element(x, y, z, direction, rideIndex)) != NULL) {
		if (stationElement->properties.track.type == TRACK_ELEM_END_STATION) {
			if (flags & GAME_COMMAND_FLAG_APPLY) {
				ride_remove_station(ride, x, y, z);
			}
		}

		stationX0 = x;
		stationY0 = y;
		byte_F441D1++;

		x -= TileDirectionDelta[direction].x;
		y -= TileDirectionDelta[direction].y;
	}

	// Search forwards for more station
	x = stationX1;
	y = stationY1;
	do {
		x += TileDirectionDelta[direction].x;
		y += TileDirectionDelta[direction].y;

		stationElement = find_station_element(x, y, z, direction, rideIndex);
		if (stationElement != NULL) {
			if (stationElement->properties.track.type == TRACK_ELEM_END_STATION) {
				if (flags & GAME_COMMAND_FLAG_APPLY) {
					ride_remove_station(ride, x, y, z);
				}
			}

			stationX1 = x;
			stationY1 = y;
			stationLength++;
		}
	} while (stationElement != NULL);

	if (!(flags & GAME_COMMAND_FLAG_APPLY)) {
		if ((removeX != stationX0 || removeY != stationY0) &&
			(removeX != stationX1 || removeY != stationY1) &&
			ride->num_stations >= 4
		) {
			gGameCommandErrorText = STR_NO_MORE_STATIONS_ALLOWED_ON_THIS_RIDE;
			return false;
		} else {
			return true;
		}
	}

	x = stationX1;
	y = stationY1;
	bool finaliseStationDone;
	do {
		finaliseStationDone = true;

		if (x != removeX || y != removeY) {
			stationElement = find_station_element(x, y, z, direction, rideIndex);
			if (stationElement != NULL) {
				int targetTrackType;
				if (x == stationX1 && y == stationY1) {
				loc_6C4BF5:;
					int stationIndex = -1;
					for (int i = 0; i < 4; i++) {
						if (ride->station_starts[i] == 0xFFFF) {
							stationIndex = i;
							break;
						}
					}

					assert(stationIndex != -1);

					uint16 xy = (x >> 5) | ((y >> 5) << 8);
					ride->station_starts[stationIndex] = xy;
					ride->station_heights[stationIndex] = z;
					ride->station_depart[stationIndex] = 1;
					ride->station_length[stationIndex] = stationLength != 0 ? stationLength : byte_F441D1;
					ride->num_stations++;

					stationLength = 0;
					targetTrackType = TRACK_ELEM_END_STATION;
				} else {
					if (x + TileDirectionDelta[direction].x == removeX &&
						y + TileDirectionDelta[direction].y == removeY
					) {
						goto loc_6C4BF5;
					} else {
						if (x - TileDirectionDelta[direction].x == removeX &&
							y - TileDirectionDelta[direction].y == removeY
						) {
							targetTrackType = TRACK_ELEM_BEGIN_STATION;
						} else {
							if (x == stationX0 && y == stationY0) {
								targetTrackType = TRACK_ELEM_BEGIN_STATION;
							} else {
								targetTrackType = TRACK_ELEM_MIDDLE_STATION;
							}
						}
					}
				}
				stationElement->properties.track.type = targetTrackType;

				map_invalidate_element(x, y, stationElement);
			}
		}

		if (x != stationX0 || y != stationY0) {
			x -= TileDirectionDelta[direction].x;
			y -= TileDirectionDelta[direction].y;
			finaliseStationDone = false;
		}
	} while (!finaliseStationDone);
	
	return true;
}

static money32 track_place(int rideIndex, int type, int originX, int originY, int originZ, int direction, int properties_1, int properties_2, int properties_3, int edx_flags, int flags)
{
	rct_ride *ride = get_ride(rideIndex);
	if (ride == NULL)
	{
		log_warning("Invalid ride for track placement, rideIndex = %d", rideIndex);
		return MONEY32_UNDEFINED;
	}
	rct_ride_entry *rideEntry = get_ride_entry(ride->subtype);
	if (rideEntry == (rct_ride_entry *)0xFFFFFFFF || rideEntry == NULL)
	{
		log_warning("Invalid ride type for track placement, rideIndex = %d", rideIndex);
		return MONEY32_UNDEFINED;
	}
	rct_map_element *mapElement;

	gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
	gCommandPosition.x = originX + 16;
	gCommandPosition.y = originY + 16;
	gCommandPosition.z = originZ;
	sint16 trackpieceZ = originZ;
	direction &= 3;
	RCT2_GLOBAL(0x00F441D5, uint32) = properties_1;
	RCT2_GLOBAL(0x00F441D9, uint32) = properties_2;
	RCT2_GLOBAL(0x00F441DD, uint32) = properties_3;
	gTrackGroundFlags = 0;

	uint64 enabledTrackPieces = 0;
	enabledTrackPieces |= rideEntry->enabledTrackPiecesB & gResearchedTrackTypesB[ride->type];
	enabledTrackPieces <<= 32;
	enabledTrackPieces |= rideEntry->enabledTrackPiecesA & gResearchedTrackTypesA[ride->type];
	uint32 rideTypeFlags = RideProperties[ride->type].flags;
	RCT2_GLOBAL(0x00F44068, uint32) = rideTypeFlags;

	if ((ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK) && type == 1) {
		gGameCommandErrorText = STR_NOT_ALLOWED_TO_MODIFY_STATION;
		return MONEY32_UNDEFINED;
	}
	if (!sub_68B044()) {
		return MONEY32_UNDEFINED;
	}
	if (!(flags & GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED)) {
		if (game_is_paused() && !gCheatsBuildInPauseMode) {
			gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
			return MONEY32_UNDEFINED;
		}
	}

	if (rideTypeFlags & RIDE_TYPE_FLAG_FLAT_RIDE) {
		RCT2_GLOBAL(0x00F44054, uint8*) = &RCT2_ADDRESS(0x0099AA94, uint8)[type * 16];
	} else {
		if (type == TRACK_ELEM_ON_RIDE_PHOTO) {
			if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO) {
				gGameCommandErrorText = STR_ONLY_ONE_ON_RIDE_PHOTO_PER_RIDE;
				return MONEY32_UNDEFINED;
			}
		} else if (type == TRACK_ELEM_CABLE_LIFT_HILL) {
			if (ride->lifecycle_flags & RIDE_LIFECYCLE_16) {
				gGameCommandErrorText = STR_ONLY_ONE_CABLE_LIFT_HILL_PER_RIDE;
				return MONEY32_UNDEFINED;
			}
		}
		if ((edx_flags & (1 << 0)) && !(enabledTrackPieces & (1ULL << TRACK_LIFT_HILL_STEEP)) && !gCheatsEnableChainLiftOnAllTrack) {
			if (RCT2_ADDRESS(0x0099423C, uint16)[type] & 0x400) {
				gGameCommandErrorText = STR_TOO_STEEP_FOR_LIFT_HILL;
				return MONEY32_UNDEFINED;
			}
		}
		RCT2_GLOBAL(0x00F44054, uint8*) = &RCT2_ADDRESS(0x00999A94, uint8)[type * 16];
	}

	money32 cost = 0;
	const rct_preview_track *trackBlock = get_track_def_from_ride(ride, type);

	// First check if any of the track pieces are outside the park
	for (; trackBlock->index != 0xFF; trackBlock++) {
		int x, y, z, offsetX, offsetY;

		switch (direction) {
		case 0:
			offsetX = trackBlock->x;
			offsetY = trackBlock->y;
			break;
		case 1:
			offsetX = trackBlock->y;
			offsetY = -trackBlock->x;
			break;
		case 2:
			offsetX = -trackBlock->x;
			offsetY = -trackBlock->y;
			break;
		case 3:
			offsetX = -trackBlock->y;
			offsetY = trackBlock->x;
			break;
		}

		x = originX + offsetX;
		y = originY + offsetY;
		z = originZ + trackBlock->z;

		if (!map_is_location_owned(x, y, z) && !gCheatsSandboxMode) {
			gGameCommandErrorText = STR_LAND_NOT_OWNED_BY_PARK;
			return MONEY32_UNDEFINED;
		}
	}

	uint16 *trackFlags = (rideTypeFlags & RIDE_TYPE_FLAG_FLAT_RIDE) ?
		RCT2_ADDRESS(0x0099443C, uint16) :
		RCT2_ADDRESS(0x0099423C, uint16);
	if (trackFlags[type] & 0x100) {
		if ((originZ & 0x0F) != 8) {
			gGameCommandErrorText = 954;
			return MONEY32_UNDEFINED;
		}
	} else {
		if ((originZ & 0x0F) != 0) {
			gGameCommandErrorText = 954;
			return MONEY32_UNDEFINED;
		}
	}

	// If that is not the case, then perform the remaining checks
	trackBlock = get_track_def_from_ride(ride, type);

	for (; trackBlock->index != 0xFF; trackBlock++, RCT2_GLOBAL(0x00F44054, uint8*)++) {
		int x, y, z, offsetX, offsetY;
		int bl = trackBlock->var_08;
		int bh;
		switch (direction) {
		case 0:
			offsetX = trackBlock->x;
			offsetY = trackBlock->y;
			break;
		case 1:
			offsetX = trackBlock->y;
			offsetY = -trackBlock->x;
			bl = rol8(bl, 1);
			bh = bl;
			bh = ror8(bh, 4);
			bl &= 0xEE;
			bh &= 0x11;
			bl |= bh;
			break;
		case 2:
			offsetX = -trackBlock->x;
			offsetY = -trackBlock->y;
			bl = rol8(bl, 2);
			bh = bl;
			bh = ror8(bh, 4);
			bl &= 0xCC;
			bh &= 0x33;
			bl |= bh;
			break;
		case 3:
			offsetX = -trackBlock->y;
			offsetY = trackBlock->x;
			bl = rol8(bl, 3);
			bh = bl;
			bh = ror8(bh, 4);
			bl &= 0x88;
			bh &= 0x77;
			bl |= bh;
			break;
		}
		x = originX + offsetX;
		y = originY + offsetY;
		z = originZ + trackBlock->z;

		trackpieceZ = z;

		if (z < 16) {
			gGameCommandErrorText = STR_TOO_LOW;
			return MONEY32_UNDEFINED;
		}

		int baseZ = (originZ + trackBlock->z) / 8;

		int clearanceZ = trackBlock->var_07;
		if (trackBlock->var_09 & (1 << 2) && RideData5[ride->type].clearance_height > 24) {
			clearanceZ += 24;
		}
		else{
			clearanceZ += RideData5[ride->type].clearance_height;
		}

		clearanceZ = (clearanceZ / 8) + baseZ;

		if (clearanceZ >= 255) {
			gGameCommandErrorText = STR_TOO_HIGH;
			return MONEY32_UNDEFINED;
		}

		_currentTrackEndX = x;
		_currentTrackEndY = y;

		if (!gCheatsDisableClearanceChecks || flags & GAME_COMMAND_FLAG_GHOST){
			if (!map_can_construct_with_clear_at(x, y, baseZ, clearanceZ, &map_place_non_scenery_clear_func, bl, flags, &cost))
				return MONEY32_UNDEFINED;
		}

		//6c53dc

		if ((flags & GAME_COMMAND_FLAG_APPLY) && !(flags & GAME_COMMAND_FLAG_GHOST)) {
			footpath_remove_litter(x, y, z);
			if (rideTypeFlags & RIDE_TYPE_FLAG_18) {
				map_remove_walls_at(x, y, baseZ * 8, clearanceZ * 8);
			} else {
				// Remove walls in the directions this track intersects
				uint8 intersectingDirections = *RCT2_GLOBAL(0x00F44054, uint8*);
				intersectingDirections ^= 0x0F;
				for (int i = 0; i < 4; i++) {
					if (intersectingDirections & (1 << i)) {
						map_remove_intersecting_walls(x, y, baseZ, clearanceZ, i);
					}
				}
			}
		}

		bh = gMapGroundFlags & (ELEMENT_IS_1 | ELEMENT_IS_UNDERGROUND);
		if (gTrackGroundFlags != 0 && (gTrackGroundFlags & bh) == 0) {
			gGameCommandErrorText = STR_CANT_BUILD_PARTLY_ABOVE_AND_PARTLY_BELOW_GROUND;
			return MONEY32_UNDEFINED;
		}

		gTrackGroundFlags = bh;
		if (rideTypeFlags & RIDE_TYPE_FLAG_FLAT_RIDE) {
			if (RCT2_ADDRESS(0x0099443C, uint16)[type] & 0x200) {
				if (gTrackGroundFlags & TRACK_ELEMENT_LOCATION_IS_UNDERGROUND) {
					gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND;
					return MONEY32_UNDEFINED;
				}
			}
		}
		else {
			if (RCT2_ADDRESS(0x0099423C, uint16)[type] & 0x200) {
				if (gTrackGroundFlags & TRACK_ELEMENT_LOCATION_IS_UNDERGROUND) {
					gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND;
					return MONEY32_UNDEFINED;
				}
			}
		}

		if (rideTypeFlags & RIDE_TYPE_FLAG_FLAT_RIDE) {
			if (RCT2_ADDRESS(0x0099443C, uint16)[type] & 1) {
				if (!(gMapGroundFlags & ELEMENT_IS_UNDERWATER)) {
					gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_UNDERWATER;
					return MONEY32_UNDEFINED;
				}
			}
		}
		else {
			if (RCT2_ADDRESS(0x0099423C, uint16)[type] & 1) {
				if (gMapGroundFlags & ELEMENT_IS_UNDERWATER) {
					gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_UNDERWATER;
					return MONEY32_UNDEFINED;
				}
			}
		}

		if (gMapGroundFlags & ELEMENT_IS_UNDERWATER) {
			gGameCommandErrorText = STR_RIDE_CANT_BUILD_THIS_UNDERWATER;
			return MONEY32_UNDEFINED;
		}

		if ((rideTypeFlags & RIDE_TYPE_FLAG_6) && !(byte_9D8150 & 1)) {
			mapElement = map_get_surface_element_at(x / 32, y / 32);

			uint8 water_height = 2 * (mapElement->properties.surface.terrain & MAP_ELEMENT_WATER_HEIGHT_MASK);
			if (water_height == 0) {
				gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_ON_WATER;
				return MONEY32_UNDEFINED;
			}

			if (water_height != baseZ) {
				gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_ON_WATER;
				return MONEY32_UNDEFINED;
			}
			water_height -= 2;
			if (water_height == mapElement->base_height) {
				bh = mapElement->properties.surface.slope & 0x0F;
				if (bh == 7 || bh == 11 || bh == 13 || bh == 14) {
					gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_ON_WATER;
					return MONEY32_UNDEFINED;
				}
			}
		}

		int entranceDirections;
		if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE)) {
			entranceDirections = RCT2_ADDRESS(0x0099CA64, uint8)[type * 16];
		}
		else {
			entranceDirections = RCT2_ADDRESS(0x0099BA64, uint8)[type * 16];
		}
		if ((entranceDirections & 0x10) && trackBlock->index == 0) {
			if (!track_add_station_element(x, y, baseZ, direction, rideIndex, 0)) {
				return MONEY32_UNDEFINED;
			}
		}
		//6c55be
		if (entranceDirections & 0x20) {
			entranceDirections &= 0x0F;
			if (entranceDirections != 0) {
				if (!(flags & GAME_COMMAND_FLAG_APPLY) && !(flags & GAME_COMMAND_FLAG_GHOST)) {
					uint8 _bl = entranceDirections;
					for (int dl = bitscanforward(_bl); dl != -1; dl = bitscanforward(_bl)){
						_bl &= ~(1 << dl);
						int temp_x = x, temp_y = y;
						int temp_direction = (direction + dl) & 3;
						temp_x += TileDirectionDelta[temp_direction].x;
						temp_y += TileDirectionDelta[temp_direction].y;
						temp_direction ^= (1 << 1);
						map_remove_intersecting_walls(temp_x, temp_y, baseZ, clearanceZ, temp_direction & 3);
					}
				}
			}
		}
		//6c5648 12 push
		mapElement = map_get_surface_element_at(x / 32, y / 32);
		if (!gCheatsDisableSupportLimits){
			int ride_height = clearanceZ - mapElement->base_height;
			if (ride_height >= 0) {
				int maxHeight = rideEntry->max_height;
				if (maxHeight == 0) {
					maxHeight = RideData5[ride->type].max_height;
				}
				ride_height /= 2;
				if (ride_height > maxHeight && !(byte_9D8150 & 1)) {
					gGameCommandErrorText = STR_TOO_HIGH_FOR_SUPPORTS;
					return MONEY32_UNDEFINED;
				}
			}
		}

		int support_height = baseZ - mapElement->base_height;
		if (support_height < 0) {
			support_height = 10;
		}

		cost += ((support_height / 2) * RideTrackCosts[ride->type].support_price) * 5;

		//6c56d3

		if (!(flags & GAME_COMMAND_FLAG_APPLY))
			continue;

		invalidate_test_results(rideIndex);
		switch (type){
		case TRACK_ELEM_ON_RIDE_PHOTO:
			ride->lifecycle_flags |= RIDE_LIFECYCLE_ON_RIDE_PHOTO;
			break;
		case TRACK_ELEM_CABLE_LIFT_HILL:
			if (trackBlock->index != 0)
				break;
			ride->lifecycle_flags |= RIDE_LIFECYCLE_16;
			ride->cable_lift_x = x;
			ride->cable_lift_y = y;
			ride->cable_lift_z = baseZ;
			break;
		case 0xD8:
			ride->num_block_brakes++;
			ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_OPERATING;
			ride->mode = RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED;
			if (ride->type == RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER)
				ride->mode = RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED;
			break;
		}

		if (trackBlock->index == 0){
			switch (type){
			case TRACK_ELEM_25_DEG_UP_TO_FLAT:
			case TRACK_ELEM_60_DEG_UP_TO_FLAT:
			case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
			case TRACK_ELEM_DIAG_60_DEG_UP_TO_FLAT:
				if (!(edx_flags & 1))
					break;
				//Fall Through
			case TRACK_ELEM_CABLE_LIFT_HILL:
				ride->num_block_brakes++;
				break;
			}
		}

		entranceDirections = 0;
		if (ride->overall_view != 0xFFFF){
			if (!(flags & GAME_COMMAND_FLAG_5)){
				if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE)) {
					entranceDirections = RCT2_ADDRESS(0x0099CA64, uint8)[type * 16];
				}
				else {
					entranceDirections = RCT2_ADDRESS(0x0099BA64, uint8)[type * 16];
				}
			}
		}
		if (entranceDirections & (1 << 4) || ride->overall_view == 0xFFFF){
			ride->overall_view = (x >> 5) | (y << 3);
		}

		mapElement = map_element_insert(x / 32, y / 32, baseZ, bl & 0xF);
		assert(mapElement != NULL);
		mapElement->clearance_height = clearanceZ;

		uint8 map_type = direction;
		map_type |= MAP_ELEMENT_TYPE_TRACK;
		if (edx_flags & 1){
			map_type |= (1 << 7);
		}
		mapElement->type = map_type;

		mapElement->properties.track.sequence = trackBlock->index;
		mapElement->properties.track.ride_index = rideIndex;
		mapElement->properties.track.type = type;
		mapElement->properties.track.colour = 0;
		if (flags & GAME_COMMAND_FLAG_GHOST){
			mapElement->flags |= MAP_ELEMENT_FLAG_GHOST;
		}

		switch (type) {
		case TRACK_ELEM_WATERFALL:
			map_animation_create(MAP_ANIMATION_TYPE_TRACK_WATERFALL, x, y, mapElement->base_height);
			break;
		case TRACK_ELEM_RAPIDS:
			map_animation_create(MAP_ANIMATION_TYPE_TRACK_RAPIDS, x, y, mapElement->base_height);
			break;
		case TRACK_ELEM_WHIRLPOOL:
			map_animation_create(MAP_ANIMATION_TYPE_TRACK_WHIRLPOOL, x, y, mapElement->base_height);
			break;
		case TRACK_ELEM_SPINNING_TUNNEL:
			map_animation_create(MAP_ANIMATION_TYPE_TRACK_SPINNINGTUNNEL, x, y, mapElement->base_height);
			break;
		}
		if (type == TRACK_ELEM_BRAKES) {
			mapElement->properties.track.sequence = (properties_1 >> 1) << 4;
		}
		else {
			mapElement->properties.track.colour = properties_3 << 4;
		}

		uint8 colour = properties_2;
		if (edx_flags & (1 << 1)){
			colour |= (1 << 2);
		}
		mapElement->properties.track.colour |= colour;

		if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE)) {
			entranceDirections = RCT2_ADDRESS(0x0099CA64, uint8)[type * 16];
		}
		else {
			entranceDirections = RCT2_ADDRESS(0x0099BA64, uint8)[type * 16];
		}

		if (entranceDirections & (1 << 4)) {
			if (trackBlock->index == 0) {
				track_add_station_element(x, y, baseZ, direction, rideIndex, GAME_COMMAND_FLAG_APPLY);
			}
			sub_6CB945(rideIndex);
			ride_update_max_vehicles(rideIndex);
		}

		if (rideTypeFlags & RIDE_TYPE_FLAG_6){
			rct_map_element* surfaceElement = map_get_surface_element_at(x / 32, y / 32);
			surfaceElement->type |= (1 << 6);
			mapElement = surfaceElement;
		}

		if (!gCheatsDisableClearanceChecks || !(flags & (1 << 6))) {
			footpath_connect_edges(x, y, mapElement, flags);
		}
		map_invalidate_tile_full(x, y);
	}

	if (gGameCommandNestLevel == 1) {
		rct_xyz16 coord;
		coord.x = originX + 16;
		coord.y = originY + 16;
		coord.z = trackpieceZ;
		network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
	}

	money32 price = RideTrackCosts[ride->type].track_price;
	price *= (rideTypeFlags & RIDE_TYPE_FLAG_FLAT_RIDE) ?
		RCT2_ADDRESS(0x0099DE34, money32)[type] :
		RCT2_ADDRESS(0x0099DA34, money32)[type];

	price >>= 16;
	price = cost + ((price / 2) * 10);

	if (gParkFlags & PARK_FLAGS_NO_MONEY) {
		return 0;
	}
	else {
		return price;
	}
}

/**
 *
 *  rct2: 0x006C511D
 */
void game_command_place_track(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp)
{
	*ebx = track_place(
		*edx & 0xFF,
		(*edx >> 8) & 0xFF,
		(sint16)(*eax & 0xFFFF),
		(sint16)(*ecx & 0xFFFF),
		(sint16)(*edi & 0xFFFF),
		(*ebx >> 8) & 0xFF,
		(*edi >> 16) & 0xFF,
		(*edi >> 24) & 0x0F,
		(*edi >> 28) & 0x0F,
		(*edx >> 16),
		*ebx & 0xFF
		);
}

static money32 track_remove(uint8 type, uint8 sequence, sint16 originX, sint16 originY, sint16 originZ, uint8 rotation, uint8 flags){
	gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
	gCommandPosition.x = originX + 16;
	gCommandPosition.y = originY + 16;
	gCommandPosition.z = originZ;
	sint16 trackpieceZ = originZ;
	RCT2_GLOBAL(0x00F440E1, uint8) = sequence;

	switch (type){
	case TRACK_ELEM_BEGIN_STATION:
	case TRACK_ELEM_MIDDLE_STATION:
		type = TRACK_ELEM_END_STATION;
		break;
	}

	if (!(flags & (1 << 3)) && game_is_paused() && !gCheatsBuildInPauseMode){
		gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
		return MONEY32_UNDEFINED;
	}

	uint8 found = 0;
	rct_map_element* mapElement = map_get_first_element_at(originX / 32, originY / 32);
	if (mapElement == NULL)
	{
		log_warning("Invalid coordinates for track removal. x = %d, y = %d", originX, originY);
		return MONEY32_UNDEFINED;
	}
	do{
		if (mapElement->base_height * 8 != originZ)
			continue;

		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
			continue;

		if ((mapElement->type & MAP_ELEMENT_DIRECTION_MASK) != rotation)
			continue;

		if ((mapElement->properties.track.sequence & 0xF) != sequence)
			continue;

		// Probably should add a check for ghost here as well!

		uint8 track_type = mapElement->properties.track.type;
		switch (track_type){
		case TRACK_ELEM_BEGIN_STATION:
		case TRACK_ELEM_MIDDLE_STATION:
			track_type = TRACK_ELEM_END_STATION;
			break;
		}

		if (track_type != type)
			continue;

		found = 1;
		break;
	} while (!map_element_is_last_for_tile(mapElement++));

	if (!found){
		return MONEY32_UNDEFINED;
	}

	if (mapElement->flags & (1 << 6)){
		gGameCommandErrorText = STR_YOU_ARE_NOT_ALLOWED_TO_REMOVE_THIS_SECTION;
		return MONEY32_UNDEFINED;
	}

	uint8 rideIndex = mapElement->properties.track.ride_index;
	type = mapElement->properties.track.type;
	RCT2_GLOBAL(0x00F44139, uint8) = type;
	RCT2_GLOBAL(0x00F44138, uint8) = rideIndex;
	RCT2_GLOBAL(0x00F4414C, uint8) = mapElement->type;

	rct_ride* ride = get_ride(rideIndex);
	const rct_preview_track* trackBlock = get_track_def_from_ride(ride, type);
	trackBlock += mapElement->properties.track.sequence & 0xF;

	uint8 originDirection = mapElement->type & MAP_ELEMENT_DIRECTION_MASK;
	switch (originDirection){
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

	money32 cost = 0;

	trackBlock = get_track_def_from_ride(ride, type);
	for (; trackBlock->index != 255; trackBlock++){
		sint16 x = originX, y = originY, z = originZ;

		switch (originDirection){
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
		RCT2_GLOBAL(0x00F441C4, sint16) = x;
		RCT2_GLOBAL(0x00F441C6, sint16) = y;

		trackpieceZ = z;

		found = 0;
		mapElement = map_get_first_element_at(x / 32, y / 32);
		do{
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

			found = 1;
			break;
		} while (!map_element_is_last_for_tile(mapElement++));

		if (!found){
			log_error("Track map element part not found!");
			return MONEY32_UNDEFINED;
		}

		int entranceDirections;
		if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE)) {
			entranceDirections = RCT2_ADDRESS(0x0099CA64, uint8)[type * 16];
		}
		else {
			entranceDirections = RCT2_ADDRESS(0x0099BA64, uint8)[type * 16];
		}

		if (entranceDirections & (1 << 4) && ((mapElement->properties.track.sequence & 0xF) == 0)){
			if (!track_remove_station_element(x, y, z / 8, rotation, rideIndex, 0)) {
				return MONEY32_UNDEFINED;
			}
		}

		rct_map_element* surfaceElement = map_get_surface_element_at(x / 32, y / 32);
		if (surfaceElement == NULL){
			return MONEY32_UNDEFINED;
		}

		sint8 support_height = mapElement->base_height - surfaceElement->base_height;
		if (support_height < 0){
			support_height = 10;
		}

		cost += (support_height / 2) * RideTrackCosts[ride->type].support_price;

		if (!(flags & GAME_COMMAND_FLAG_APPLY))
			continue;

		if (entranceDirections & (1 << 4) && ((mapElement->properties.track.sequence & 0xF) == 0)){
			if (!track_remove_station_element(x, y, z / 8, rotation, rideIndex, GAME_COMMAND_FLAG_APPLY)) {
				return MONEY32_UNDEFINED;
			}
		}

		if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_6)){
			surfaceElement->type &= ~(1 << 6);
		}

		invalidate_test_results(rideIndex);
		footpath_queue_chain_reset();
		if (!gCheatsDisableClearanceChecks || !(mapElement->flags & MAP_ELEMENT_FLAG_GHOST)) {
			footpath_remove_edges_at(x, y, mapElement);
		}
		map_element_remove(mapElement);
		sub_6CB945(rideIndex);
		if (!(flags & (1 << 6))){
			ride_update_max_vehicles(rideIndex);
		}
	}

	if (flags & GAME_COMMAND_FLAG_APPLY){
		switch (type){
		case TRACK_ELEM_ON_RIDE_PHOTO:
			ride->lifecycle_flags &= ~RIDE_LIFECYCLE_ON_RIDE_PHOTO;
			break;
		case TRACK_ELEM_CABLE_LIFT_HILL:
			ride->lifecycle_flags &= ~RIDE_LIFECYCLE_16;
			break;
		case 216:
			ride->num_block_brakes--;
			if (ride->num_block_brakes == 0){
				ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_OPERATING;
				ride->mode = RIDE_MODE_CONTINUOUS_CIRCUIT;
				if (ride->type == RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER){
					ride->mode = RIDE_MODE_POWERED_LAUNCH;
				}
			}
			break;
		}

		switch (type){
		case TRACK_ELEM_25_DEG_UP_TO_FLAT:
		case TRACK_ELEM_60_DEG_UP_TO_FLAT:
		case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
		case TRACK_ELEM_DIAG_60_DEG_UP_TO_FLAT:
			if (!(RCT2_GLOBAL(0x00F4414C, uint8) & (1 << 7)))
				break;
			// Fall through
		case TRACK_ELEM_CABLE_LIFT_HILL:
			ride->num_block_brakes--;
			break;
		}
	}

	money32 price = RideTrackCosts[ride->type].track_price;
	if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE)) {
		price *= RCT2_ADDRESS(0x0099DE34, money32)[type];
	}
	else {
		price *= RCT2_ADDRESS(0x0099DA34, money32)[type];
	}
	price >>= 16;
	price = (price + cost) / 2;
	if (ride->lifecycle_flags & RIDE_LIFECYCLE_EVER_BEEN_OPENED)
		price *= -7;
	else
		price *= -10;

	if (gGameCommandNestLevel == 1) {
		rct_xyz16 coord;
		coord.x = originX + 16;
		coord.y = originY + 16;
		coord.z = trackpieceZ;
		network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
	}

	if (gParkFlags & PARK_FLAGS_NO_MONEY)
		return 0;
	else
		return price;
}

/**
 *
 *  rct2: 0x006C5B69
 */
void game_command_remove_track(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp)
{
	*ebx = track_remove(
		*edx & 0xFF,
		(*edx >> 8) & 0xFF,
		*eax & 0xFFFF,
		*ecx & 0xFFFF,
		*edi & 0xFFFF,
		(*ebx >> 8) & 0xFF,
		*ebx & 0xFF
		);
	return;
}

static uint8 maze_element_get_segment_bit(uint16 x, uint16 y) {
	uint8 minorX = x & 0x1F; // 0 or 16
	uint8 minorY = y & 0x1F; // 0 or 16

	if (minorX == 0 && minorY == 0) {
		return 3;
	}

	if (minorY == 16 && minorX == 16) {
		return 11;
	}

	if (minorY == 0) {
		return 15;
	}

	return 7;
}

static money32 set_maze_track(uint16 x, uint8 flags, uint8 direction, uint16 y, uint8 rideIndex, uint8 mode, uint16 z) {
	gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
	gCommandPosition.x = x + 8;
	gCommandPosition.y = y + 8;
	gCommandPosition.z = z;

	RCT2_GLOBAL(0xF4413E, money32) = 0;

	if (!sub_68B044()) {
		return MONEY32_UNDEFINED;
	}

	if (!(flags & GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED) && !gCheatsBuildInPauseMode && game_is_paused()) {
		gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
		return MONEY32_UNDEFINED;
	}

	if ((z & 0xF) != 0) {
		// ‘Can't construct this here…’
		// TODO: This string is empty
		gGameCommandErrorText = 954;
		return MONEY32_UNDEFINED;
	}

	if ((flags & GAME_COMMAND_FLAG_APPLY) != 0) {
		if (!(flags & GAME_COMMAND_FLAG_GHOST) && !(flags & GAME_COMMAND_FLAG_2)) {
			footpath_remove_litter(x, y, z);
			map_remove_walls_at(floor2(x, 32), floor2(y, 32), z, z + 32);
		}
	}

	if (!map_is_location_owned(floor2(x, 32), floor2(y, 32), z) && !gCheatsSandboxMode) {
		return MONEY32_UNDEFINED;
	}

	rct_map_element *mapElement = map_get_surface_element_at(x / 32, y / 32);
	if (mapElement == NULL) {
		return MONEY32_UNDEFINED;
	}

	uint8 baseHeight = z >> 3;
	uint8 clearanceHeight = (z + 32) >> 3;

	sint8 heightDifference = baseHeight - mapElement->base_height;
	if (heightDifference >= 0 && !gCheatsDisableSupportLimits) {
		heightDifference = heightDifference >> 1;

		if (heightDifference > RideData5[RIDE_TYPE_MAZE].max_height) {
			gGameCommandErrorText = STR_TOO_HIGH_FOR_SUPPORTS;
			return MONEY32_UNDEFINED;
		}
	}

	mapElement = map_get_track_element_at_of_type_from_ride(x, y, baseHeight, 0x65, rideIndex);
	if (mapElement == NULL) {
		if (mode != 0) {
			gGameCommandErrorText = 0;
			return MONEY32_UNDEFINED;
		}

		if (!map_can_construct_at(floor2(x, 32), floor2(y, 32), baseHeight, clearanceHeight, 0x0F)) {
			return MONEY32_UNDEFINED;
		}

		if (gMapGroundFlags & ELEMENT_IS_UNDERWATER) {
			gGameCommandErrorText = STR_RIDE_CANT_BUILD_THIS_UNDERWATER;
			return MONEY32_UNDEFINED;
		}

		if (gMapGroundFlags & ELEMENT_IS_UNDERGROUND) {
			gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND;
			return MONEY32_UNDEFINED;
		}

		rct_ride *ride = get_ride(rideIndex);

		money32 price = (((RideTrackCosts[ride->type].track_price * RCT2_GLOBAL(0x0099DBC8, money32)) >> 16));
		RCT2_GLOBAL(0x00F4413E, money32) = price / 2 * 10;

		if (!(flags & GAME_COMMAND_FLAG_APPLY)) {
			if (gParkFlags & PARK_FLAGS_NO_MONEY) {
				return 0;
			}

			return RCT2_GLOBAL(0xF4413E, money32);
		}

		uint16 flooredX = floor2(x, 32);
		uint16 flooredY = floor2(y, 32);

		mapElement = map_element_insert(x / 32, y / 32, baseHeight, 0xF);
		assert(mapElement != NULL);
		mapElement->clearance_height = clearanceHeight;
		mapElement->type = MAP_ELEMENT_TYPE_TRACK;
		mapElement->properties.track.type = 0x65;
		mapElement->properties.track.ride_index = rideIndex;
		mapElement->properties.track.maze_entry = 0xFFFF;

		if (flags & GAME_COMMAND_FLAG_GHOST) {
			mapElement->flags |= MAP_ELEMENT_FLAG_GHOST;
		}

		map_invalidate_tile_full(flooredX, flooredY);

		ride->maze_tiles++;
		ride->station_heights[0] = mapElement->base_height;
		ride->station_starts[0] = 0;

		if (direction == 4) {
			if (!(flags & GAME_COMMAND_FLAG_GHOST)) {
				ride->overall_view = (flooredX >> 5) | (flooredY << 3);
			}
		}
	}

	if (!(flags & GAME_COMMAND_FLAG_APPLY)) {
		if (gParkFlags & PARK_FLAGS_NO_MONEY) {
			return 0;
		}

		return RCT2_GLOBAL(0xF4413E, money32);
	}


	if (mode == 0) {
		// Build mode
		uint8 segmentOffset = maze_element_get_segment_bit(x, y);

		mapElement->properties.track.maze_entry &= ~(1 << segmentOffset);

		if (direction != 4) {
			segmentOffset = RCT2_GLOBAL(0x993ce9 + (direction + segmentOffset), uint8);
			mapElement->properties.track.maze_entry &= ~(1 << segmentOffset);

			uint8 temp_edx = RCT2_GLOBAL(0x993cfc + segmentOffset, uint8);
			if (temp_edx != 0xFF) {
				uint16 previousElementX = floor2(x, 32) - TileDirectionDelta[direction].x;
				uint16 previousElementY = floor2(y, 32) - TileDirectionDelta[direction].y;

				rct_map_element *previousMapElement = map_get_track_element_at_of_type_from_ride(previousElementX, previousElementY, baseHeight, 0x65, rideIndex);
				if (previousMapElement != NULL) {
					previousMapElement->properties.track.maze_entry &= ~(1 << temp_edx);
				} else {
					mapElement->properties.track.maze_entry |= (1 << segmentOffset);
				}
			}
		}
	} else if (mode == 1) {
		// Move mode
	} else {
		// Fill-in mode
		if (direction != 4) {
			uint16 previousSegmentX = x - TileDirectionDelta[direction].x / 2;
			uint16 previousSegmentY = y - TileDirectionDelta[direction].y / 2;

			mapElement = map_get_track_element_at_of_type_from_ride(previousSegmentX, previousSegmentY, baseHeight, 0x65, rideIndex);
			map_invalidate_tile_full(floor2(previousSegmentX, 32), floor2(previousSegmentY, 32));
			if (mapElement == NULL) {
				log_error("No surface found");
				return MONEY32_UNDEFINED;
			}

			uint32 segmentBit = maze_element_get_segment_bit(previousSegmentX, previousSegmentY);

			mapElement->properties.track.maze_entry |= (1 << segmentBit);
			segmentBit--;
			mapElement->properties.track.maze_entry |= (1 << segmentBit);
			segmentBit = (segmentBit - 4) & 0x0F;
			mapElement->properties.track.maze_entry |= (1 << segmentBit);
			segmentBit = (segmentBit + 3) & 0x0F;

			do {
				mapElement->properties.track.maze_entry |= (1 << segmentBit);

				uint32 direction1 = RCT2_GLOBAL(0x00993D0C + segmentBit, uint8_t);
				uint16 nextElementX = floor2(previousSegmentX, 32) + TileDirectionDelta[direction1].x;
				uint16 nextElementY = floor2(previousSegmentY, 32) + TileDirectionDelta[direction1].y;

				rct_map_element *tmp_mapElement = map_get_track_element_at_of_type_from_ride(nextElementX, nextElementY, baseHeight, 0x65, rideIndex);
				if (tmp_mapElement != NULL) {
					uint8 edx11 = RCT2_GLOBAL(0x993CFC + segmentBit, uint8);
					tmp_mapElement->properties.track.maze_entry |= 1 << (edx11);
				}

				segmentBit--;
			} while ((segmentBit & 0x3) != 0x3);
		}
	}

	map_invalidate_tile(floor2(x, 32), floor2(y, 32), mapElement->base_height * 8, mapElement->clearance_height * 8);

	if ((mapElement->properties.track.maze_entry & 0x8888) == 0x8888) {
		map_element_remove(mapElement);
		sub_6CB945(rideIndex);
		get_ride(rideIndex)->maze_tiles--;
	}

	if (gParkFlags & PARK_FLAGS_NO_MONEY) {
		return 0;
	}

	return RCT2_GLOBAL(0xF4413E, money32);
}

/**
 *
 *  rct2: 0x006CD8CE
 */
void game_command_set_maze_track(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp)
{
	uint16 x = (*eax & 0xFFFF); // AX
	uint8 flags = (*ebx & 0xFF); // BL
	uint8 direction = ((*ebx & 0xFF00) >> 8); // BH
	uint16 y = (*ecx & 0xFFFF); // CX
	uint8 rideIndex = (*edx & 0xFF); // DL
	uint8 mode = ((*edx & 0xFF00) >> 8); // DH
	uint16 z = (*edi & 0xFFFF); // DI

	*ebx = set_maze_track(x, flags, direction, y, rideIndex, mode, z);
}

/**
 *
 *  rct2: 0x006C5AE9
 */
void game_command_set_brakes_speed(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp)
{
	rct_map_element *mapElement;
	int x, y, z, trackType, brakesSpeed;

	x = (*eax & 0xFFFF);
	y = (*ecx & 0xFFFF);
	z = (*edi & 0xFFFF);
	trackType = (*edx & 0xFF);
	brakesSpeed = ((*ebx >> 8) & 0xFF);

	gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
	gCommandPosition.x = x + 16;
	gCommandPosition.y = y + 16;
	gCommandPosition.z = z;

	if (*ebx & GAME_COMMAND_FLAG_APPLY) {
		*ebx = 0;
		return;
	}

	mapElement = map_get_first_element_at(x >> 5, y >> 5);
	if (mapElement == NULL)
	{
		log_warning("Invalid game command for setting brakes speed. x = %d, y = %d", x, y);
		*ebx = MONEY32_UNDEFINED;
		return;
	}
	do {
		if (mapElement->base_height * 8 != z)
			continue;
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
			continue;
		if (mapElement->properties.track.type != trackType)
			continue;

		mapElement->properties.track.sequence =
			(mapElement->properties.track.sequence & 0x0F) |
			((brakesSpeed >> 1) << 4);

		break;
	} while (!map_element_is_last_for_tile(mapElement++));

	*ebx = 0;
}

void track_circuit_iterator_begin(track_circuit_iterator *it, rct_xy_element first)
{
	it->last = first;
	it->first = NULL;
	it->firstIteration = true;
	it->looped = false;
}

bool track_circuit_iterator_previous(track_circuit_iterator *it)
{
	track_begin_end trackBeginEnd;

	if (it->first == NULL) {
		if (!track_block_get_previous(it->last.x, it->last.y, it->last.element, &trackBeginEnd))
			return false;

		it->current.x = trackBeginEnd.begin_x;
		it->current.y = trackBeginEnd.begin_y;
		it->current.element = trackBeginEnd.begin_element;
		it->currentZ = trackBeginEnd.begin_z;
		it->currentDirection = trackBeginEnd.begin_direction;

		it->first = it->current.element;
		return true;
	} else {
		if (!it->firstIteration && it->first == it->current.element) {
			it->looped = true;
			return false;
		}

		it->firstIteration = false;
		it->last = it->current;

		if (track_block_get_previous(it->last.x, it->last.y, it->last.element, &trackBeginEnd)) {
			it->current.x = trackBeginEnd.end_x;
			it->current.y = trackBeginEnd.end_y;
			it->current.element = trackBeginEnd.begin_element;
			it->currentZ = trackBeginEnd.begin_z;
			it->currentDirection = trackBeginEnd.begin_direction;
			return true;
		} else {
			return false;
		}
	}
}

bool track_circuit_iterator_next(track_circuit_iterator *it)
{
	if (it->first == NULL) {
		if (!track_block_get_next(&it->last, &it->current, &it->currentZ, &it->currentDirection))
			return false;

		it->first = it->current.element;
		return true;
	} else {
		if (!it->firstIteration && it->first == it->current.element) {
			it->looped = true;
			return false;
		}

		it->firstIteration = false;
		it->last = it->current;
		return track_block_get_next(&it->last, &it->current, &it->currentZ, &it->currentDirection);
	}
}

void track_get_back(rct_xy_element *input, rct_xy_element *output)
{
	rct_xy_element lastTrack;
	track_begin_end currentTrack;
	bool result;

	lastTrack = *input;
	do {
		result = track_block_get_previous(lastTrack.x, lastTrack.y, lastTrack.element, &currentTrack);
		if (result) {
			lastTrack.x = currentTrack.begin_x;
			lastTrack.y = currentTrack.begin_y;
			lastTrack.element = currentTrack.begin_element;
		}
	} while (result);
	*output = lastTrack;
}

void track_get_front(rct_xy_element *input, rct_xy_element *output)
{
	rct_xy_element lastTrack, currentTrack;
	int z, direction;
	bool result;

	lastTrack = *input;
	do {
		result = track_block_get_next(&lastTrack, &currentTrack, &z, &direction);
		if (result) {
			lastTrack = currentTrack;
		}
	} while (result);
	*output = lastTrack;
}

bool track_element_is_lift_hill(rct_map_element *trackElement)
{
	return trackElement->type & 0x80;
}

/**
 * Checks if a track element is recognised as the beginning of a block.
 * A beginning of a block can be the end of a station, the end of a lift hill,
 * or a block brake.
 */
bool track_element_is_block_start(rct_map_element *trackElement)
{
	switch (trackElement->properties.track.type) {
	case TRACK_ELEM_END_STATION:
	case TRACK_ELEM_CABLE_LIFT_HILL:
	case 216:
		return true;
	case TRACK_ELEM_25_DEG_UP_TO_FLAT:
	case TRACK_ELEM_60_DEG_UP_TO_FLAT:
	case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
	case TRACK_ELEM_DIAG_60_DEG_UP_TO_FLAT:
		if (track_element_is_lift_hill(trackElement)) {
			return true;
		}
		break;
	}
	return false;
}

bool track_element_is_cable_lift(rct_map_element *trackElement) {
	return trackElement->properties.track.colour & TRACK_ELEMENT_COLOUR_FLAG_CABLE_LIFT;
}

void track_element_set_cable_lift(rct_map_element *trackElement) {
	trackElement->properties.track.colour |= TRACK_ELEMENT_COLOUR_FLAG_CABLE_LIFT;
}

void track_element_clear_cable_lift(rct_map_element *trackElement) {
	trackElement->properties.track.colour &= ~TRACK_ELEMENT_COLOUR_FLAG_CABLE_LIFT;
}

int track_get_actual_bank(rct_map_element *mapElement, int bank)
{
	rct_ride *ride = get_ride(mapElement->properties.track.ride_index);
	int trackColour = mapElement->properties.track.colour;
	return track_get_actual_bank_2(ride->type, trackColour, bank);
}

int track_get_actual_bank_2(int rideType, int trackColour, int bank)
{
	if (RideData4[rideType].flags & RIDE_TYPE_FLAG4_3) {
		if (trackColour & 4) {
			if (bank == TRACK_BANK_NONE) {
				bank = TRACK_BANK_UPSIDE_DOWN;
			} else if (bank == TRACK_BANK_UPSIDE_DOWN) {
				bank = TRACK_BANK_NONE;
			}
		}
	}
	return bank;
}

int track_get_actual_bank_3(rct_vehicle *vehicle, rct_map_element *mapElement)
{
	uint8 colourThingToXor = (vehicle->update_flags >> 9) & 0xFF;
	int trackType = mapElement->properties.track.type;
	int rideType = get_ride(mapElement->properties.track.ride_index)->type;
	int trackColour = mapElement->properties.track.colour ^ colourThingToXor;
	int bankStart = gTrackDefinitions[trackType].bank_start;
	return track_get_actual_bank_2(rideType, trackColour, bankStart);
}

bool track_element_is_station(rct_map_element *trackElement)
{
	switch (trackElement->properties.track.type) {
	case TRACK_ELEM_END_STATION:
	case TRACK_ELEM_BEGIN_STATION:
	case TRACK_ELEM_MIDDLE_STATION:
		return true;
	default:
		return false;
	}
}

bool track_element_is_covered(int trackElementType)
{
	switch (trackElementType) {
	case TRACK_ELEM_FLAT_COVERED:
	case TRACK_ELEM_25_DEG_UP_COVERED:
	case TRACK_ELEM_60_DEG_UP_COVERED:
	case TRACK_ELEM_FLAT_TO_25_DEG_UP_COVERED:
	case TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP_COVERED:
	case TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP_COVERED:
	case TRACK_ELEM_25_DEG_UP_TO_FLAT_COVERED:
	case TRACK_ELEM_25_DEG_DOWN_COVERED:
	case TRACK_ELEM_60_DEG_DOWN_COVERED:
	case TRACK_ELEM_FLAT_TO_25_DEG_DOWN_COVERED:
	case TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN_COVERED:
	case TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN_COVERED:
	case TRACK_ELEM_25_DEG_DOWN_TO_FLAT_COVERED:
	case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_COVERED:
	case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_COVERED:
	case TRACK_ELEM_S_BEND_LEFT_COVERED:
	case TRACK_ELEM_S_BEND_RIGHT_COVERED:
	case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_COVERED:
	case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_COVERED:
		return true;
	default:
		return false;
	}
}
