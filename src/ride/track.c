/*****************************************************************************
 * Copyright (c) 2014 Ted John, Kevin Burke
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
#include "../config.h"
#include "../game.h"
#include "../interface/viewport.h"
#include "../localisation/localisation.h"
#include "../platform/platform.h"
#include "../rct1.h"
#include "../util/sawyercoding.h"
#include "../util/util.h"
#include "../world/park.h"
#include "../world/scenery.h"
#include "../world/footpath.h"
#include "../windows/error.h"
#include "ride_ratings.h"
#include "ride.h"
#include "track.h"
#include "track_data.h"

/**
 *
 *  rct2: 0x00997C9D
 */
const rct_trackdefinition *gTrackDefinitions = (rct_trackdefinition*)0x00997C9D;

// TODO This table is incorrect or at least missing 69 elements. There should be 256 in total!
const rct_trackdefinition gTrackDefinitions_INCORRECT[] = {
    // TYPE							VANGLE END					VANGLE START				BANK END				BANK START				SPECIAL
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_FLAT
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_END_STATION
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_BEGIN_STATION
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_MIDDLE_STATION
    { TRACK_FLAT,					TRACK_UP_25,				TRACK_UP_25,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_25_DEG_UP
    { TRACK_FLAT,					TRACK_UP_60,				TRACK_UP_60,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_60_DEG_UP
    { TRACK_FLAT,					TRACK_UP_25,				TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_FLAT_TO_25_DEG_UP
    { TRACK_FLAT,					TRACK_UP_60,				TRACK_UP_25,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_25_DEG_UP_TO_60_DEG_UP
    { TRACK_FLAT,					TRACK_UP_25,				TRACK_UP_60,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_60_DEG_UP_TO_25_DEG_UP
    { TRACK_FLAT,					TRACK_NONE,					TRACK_UP_25,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_25_DEG_UP_TO_FLAT
    { TRACK_FLAT,					TRACK_DOWN_25,				TRACK_DOWN_25,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_25_DEG_DOWN
    { TRACK_FLAT,					TRACK_DOWN_60,				TRACK_DOWN_60,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_60_DEG_DOWN
    { TRACK_FLAT,					TRACK_DOWN_25,				TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_FLAT_TO_25_DEG_DOWN
    { TRACK_FLAT,					TRACK_DOWN_60,				TRACK_DOWN_25,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_25_DEG_DOWN_TO_60_DEG_DOWN
    { TRACK_FLAT,					TRACK_DOWN_25,				TRACK_DOWN_60,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_60_DEG_DOWN_TO_25_DEG_DOWN
    { TRACK_FLAT,					TRACK_NONE,					TRACK_DOWN_25,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_25_DEG_DOWN_TO_FLAT
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_QUARTER_TURN_5_TILES
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_QUARTER_TURN_5_TILES
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_LEFT,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_FLAT_TO_LEFT_BANK
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_RIGHT,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_FLAT_TO_RIGHT_BANK
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_LEFT_BANK_TO_FLAT
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_RIGHT_BANK_TO_FLAT
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_LEFT,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_BANKED_LEFT_QUARTER_TURN_5_TILES
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_RIGHT,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_BANKED_RIGHT_QUARTER_TURN_5_TILES
    { TRACK_FLAT,					TRACK_UP_25,				TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_LEFT_BANK_TO_25_DEG_UP
    { TRACK_FLAT,					TRACK_UP_25,				TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_RIGHT_BANK_TO_25_DEG_UP
    { TRACK_FLAT,					TRACK_NONE,					TRACK_UP_25,				TRACK_BANK_LEFT,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_25_DEG_UP_TO_LEFT_BANK
    { TRACK_FLAT,					TRACK_NONE,					TRACK_UP_25,				TRACK_BANK_RIGHT,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_25_DEG_UP_TO_RIGHT_BANK
    { TRACK_FLAT,					TRACK_DOWN_25,				TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_LEFT_BANK_TO_25_DEG_DOWN
    { TRACK_FLAT,					TRACK_DOWN_25,				TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_RIGHT_BANK_TO_25_DEG_DOWN
    { TRACK_FLAT,					TRACK_NONE,					TRACK_DOWN_25,				TRACK_BANK_LEFT,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_25_DEG_DOWN_TO_LEFT_BANK
    { TRACK_FLAT,					TRACK_NONE,					TRACK_DOWN_25,				TRACK_BANK_RIGHT,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_25_DEG_DOWN_TO_RIGHT_BANK
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_LEFT,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_LEFT_BANK
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_RIGHT,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_RIGHT_BANK
    { TRACK_FLAT,					TRACK_UP_25,				TRACK_UP_25,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_UP
    { TRACK_FLAT,					TRACK_UP_25,				TRACK_UP_25,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_UP
    { TRACK_FLAT,					TRACK_DOWN_25,				TRACK_DOWN_25,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_DOWN
    { TRACK_FLAT,					TRACK_DOWN_25,				TRACK_DOWN_25,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_DOWN
    { TRACK_S_BEND,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_S_BEND_LEFT
    { TRACK_S_BEND,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_S_BEND_RIGHT
    { TRACK_VERTICAL_LOOP,			TRACK_DOWN_25,				TRACK_UP_25,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_UNKNOWN_VERTICAL_LOOP	},	// ELEM_LEFT_VERTICAL_LOOP
    { TRACK_VERTICAL_LOOP,			TRACK_DOWN_25,				TRACK_UP_25,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_UNKNOWN_VERTICAL_LOOP	},	// ELEM_RIGHT_VERTICAL_LOOP
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_QUARTER_TURN_3_TILES
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_QUARTER_TURN_3_TILES
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_LEFT,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_LEFT_QUARTER_TURN_3_TILES_BANK
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_RIGHT,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_RIGHT_QUARTER_TURN_3_TILES_BANK
    { TRACK_FLAT,					TRACK_UP_25,				TRACK_UP_25,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP
    { TRACK_FLAT,					TRACK_UP_25,				TRACK_UP_25,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP
    { TRACK_FLAT,					TRACK_DOWN_25,				TRACK_DOWN_25,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN
    { TRACK_FLAT,					TRACK_DOWN_25,				TRACK_DOWN_25,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_QUARTER_TURN_1_TILE
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_QUARTER_TURN_1_TILE
    { TRACK_TWIST,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_UPSIDE_DOWN,	TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_TWIST_DOWN_TO_UP
    { TRACK_TWIST,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_UPSIDE_DOWN,	TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_TWIST_DOWN_TO_UP
    { TRACK_TWIST,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_UPSIDE_DOWN,	TRACK_NONE					},	// ELEM_LEFT_TWIST_UP_TO_DOWN
    { TRACK_TWIST,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_UPSIDE_DOWN,	TRACK_NONE					},	// ELEM_RIGHT_TWIST_UP_TO_DOWN
    { TRACK_HALF_LOOP,				TRACK_NONE,					TRACK_UP_25,				TRACK_BANK_UPSIDE_DOWN,	TRACK_BANK_NONE,		TRACK_HALF_LOOP_UP			},	// ELEM_HALF_LOOP_UP
    { TRACK_HALF_LOOP,				TRACK_DOWN_25,				TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_UPSIDE_DOWN,	TRACK_HALF_LOOP_DOWN		},	// ELEM_HALF_LOOP_DOWN
    { TRACK_CORKSCREW,				TRACK_NONE,					TRACK_NONE,					TRACK_BANK_UPSIDE_DOWN,	TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_CORKSCREW_UP
    { TRACK_CORKSCREW,				TRACK_NONE,					TRACK_NONE,					TRACK_BANK_UPSIDE_DOWN,	TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_CORKSCREW_UP
    { TRACK_CORKSCREW,				TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_UPSIDE_DOWN,	TRACK_CORKSCREW_DOWN		},	// ELEM_LEFT_CORKSCREW_DOWN
    { TRACK_CORKSCREW,				TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_UPSIDE_DOWN,	TRACK_CORKSCREW_DOWN		},	// ELEM_RIGHT_CORKSCREW_DOWN
    { TRACK_FLAT,					TRACK_UP_60,				TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_FLAT_TO_60_DEG_UP
    { TRACK_FLAT,					TRACK_NONE,					TRACK_UP_60,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_60_DEG_UP_TO_FLAT
    { TRACK_FLAT,					TRACK_DOWN_60,				TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_FLAT_TO_60_DEG_DOWN
    { TRACK_FLAT,					TRACK_NONE,					TRACK_DOWN_60,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_60_DEG_DOWN_TO_FLAT
    { TRACK_TOWER_BASE,				TRACK_VANGLE_TOWER,			TRACK_VANGLE_TOWER,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_TOWER_BASE
    { TRACK_TOWER_BASE,				TRACK_VANGLE_TOWER,			TRACK_VANGLE_TOWER,			TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_TOWER_SECTION
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_FLAT_COVERED
    { TRACK_FLAT,					TRACK_UP_25,				TRACK_UP_25,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_25_DEG_UP_COVERED
    { TRACK_FLAT,					TRACK_UP_60,				TRACK_UP_60,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_60_DEG_UP_COVERED
    { TRACK_FLAT,					TRACK_UP_25,				TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_FLAT_TO_25_DEG_UP_COVERED
    { TRACK_FLAT,					TRACK_UP_60,				TRACK_UP_25,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_25_DEG_UP_TO_60_DEG_UP_COVERED
    { TRACK_FLAT,					TRACK_UP_25,				TRACK_UP_60,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_60_DEG_UP_TO_25_DEG_UP_COVERED
    { TRACK_FLAT,					TRACK_NONE,					TRACK_UP_25,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_25_DEG_UP_TO_FLAT_COVERED
    { TRACK_FLAT,					TRACK_DOWN_25,				TRACK_DOWN_25,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_25_DEG_DOWN_COVERED
    { TRACK_FLAT,					TRACK_DOWN_60,				TRACK_DOWN_60,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_60_DEG_DOWN_COVERED
    { TRACK_FLAT,					TRACK_DOWN_25,				TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_FLAT_TO_25_DEG_DOWN_COVERED
    { TRACK_FLAT,					TRACK_DOWN_60,				TRACK_DOWN_25,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_25_DEG_DOWN_TO_60_DEG_DOWN_COVERED
    { TRACK_FLAT,					TRACK_DOWN_25,				TRACK_DOWN_60,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_60_DEG_DOWN_TO_25_DEG_DOWN_COVERED
    { TRACK_FLAT,					TRACK_NONE,					TRACK_DOWN_25,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_25_DEG_DOWN_TO_FLAT_COVERED
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_QUARTER_TURN_5_TILES_COVERED
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_QUARTER_TURN_5_TILES_COVERED
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_S_BEND_LEFT_COVERED
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_S_BEND_RIGHT_COVERED
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_QUARTER_TURN_3_TILES_COVERED
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_QUARTER_TURN_3_TILES_COVERED
    { TRACK_HELIX_SMALL,			TRACK_NONE,					TRACK_NONE,					TRACK_BANK_LEFT,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_LEFT_HALF_BANKED_HELIX_UP_SMALL
    { TRACK_HELIX_SMALL,			TRACK_NONE,					TRACK_NONE,					TRACK_BANK_RIGHT,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_RIGHT_HALF_BANKED_HELIX_UP_SMALL
    { TRACK_HELIX_SMALL,			TRACK_NONE,					TRACK_NONE,					TRACK_BANK_LEFT,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_LEFT_HALF_BANKED_HELIX_DOWN_SMALL
    { TRACK_HELIX_SMALL,			TRACK_NONE,					TRACK_NONE,					TRACK_BANK_RIGHT,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_SMALL
    { TRACK_HELIX_SMALL,			TRACK_NONE,					TRACK_NONE,					TRACK_BANK_LEFT,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_LEFT_HALF_BANKED_HELIX_UP_LARGE
    { TRACK_HELIX_SMALL,			TRACK_NONE,					TRACK_NONE,					TRACK_BANK_RIGHT,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_RIGHT_HALF_BANKED_HELIX_UP_LARGE
    { TRACK_HELIX_SMALL,			TRACK_NONE,					TRACK_NONE,					TRACK_BANK_LEFT,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_LEFT_HALF_BANKED_HELIX_DOWN_LARGE
    { TRACK_HELIX_SMALL,			TRACK_NONE,					TRACK_NONE,					TRACK_BANK_RIGHT,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_LARGE
    { TRACK_FLAT,					TRACK_UP_60,				TRACK_UP_60,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_UP
    { TRACK_FLAT,					TRACK_UP_60,				TRACK_UP_60,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_UP
    { TRACK_FLAT,					TRACK_DOWN_60,				TRACK_DOWN_60,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_DOWN
    { TRACK_FLAT,					TRACK_DOWN_60,				TRACK_DOWN_60,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_DOWN
    { TRACK_BRAKES,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_BRAKES
    { TRACK_ROTATION_CONTROL_TOGGLE,TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_ROTATION_CONTROL_TOGGLE
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_INVERTED_90_DEG_UP_TO_FLAT_QUARTER_LOOP
    { TRACK_HELIX_LARGE,			TRACK_NONE,					TRACK_NONE,					TRACK_BANK_LEFT,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_UP
    { TRACK_HELIX_LARGE,			TRACK_NONE,					TRACK_NONE,					TRACK_BANK_RIGHT,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_UP
    { TRACK_HELIX_LARGE,			TRACK_NONE,					TRACK_NONE,					TRACK_BANK_LEFT,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_DOWN
    { TRACK_HELIX_LARGE,			TRACK_NONE,					TRACK_NONE,					TRACK_BANK_RIGHT,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_DOWN
    { TRACK_HELIX_LARGE_UNBANKED,	TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_QUARTER_HELIX_LARGE_UP
    { TRACK_HELIX_LARGE_UNBANKED,	TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_QUARTER_HELIX_LARGE_UP
    { TRACK_HELIX_LARGE_UNBANKED,	TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_QUARTER_HELIX_LARGE_DOWN
    { TRACK_HELIX_LARGE_UNBANKED,	TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_QUARTER_HELIX_LARGE_DOWN
    { TRACK_FLAT,					TRACK_UP_25,				TRACK_UP_25,				TRACK_BANK_LEFT,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_25_DEG_UP_LEFT_BANKED
    { TRACK_FLAT,					TRACK_UP_25,				TRACK_UP_25,				TRACK_BANK_RIGHT,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_25_DEG_UP_RIGHT_BANKED
    { TRACK_WATERFALL,				TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_WATERFALL
    { TRACK_RAPIDS,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RAPIDS
    { TRACK_ON_RIDE_PHOTO,			TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_ON_RIDE_PHOTO
    { TRACK_FLAT,					TRACK_DOWN_25,				TRACK_DOWN_25,				TRACK_BANK_LEFT,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_25_DEG_DOWN_LEFT_BANKED
    { TRACK_FLAT,					TRACK_DOWN_25,				TRACK_DOWN_25,				TRACK_BANK_RIGHT,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_25_DEG_DOWN_RIGHT_BANKED
    { TRACK_WATER_SPLASH,			TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_WATER_SPLASH
    { TRACK_FLAT,					TRACK_UP_60,				TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_FLAT_TO_60_DEG_UP_LONG_BASE
    { TRACK_FLAT,					TRACK_NONE,					TRACK_UP_60,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_60_DEG_UP_TO_FLAT_LONG_BASE
    { TRACK_WHIRLPOOL,				TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_WHIRLPOOL
    { TRACK_FLAT,					TRACK_NONE,					TRACK_DOWN_60,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_FLAT_TO_60_DEG_DOWN_LONG_BASE
    { TRACK_FLAT,					TRACK_DOWN_60,				TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_60_DEG_UP_TO_FLAT_LONG_BASE
    { TRACK_LIFT_HILL,				TRACK_DOWN_60,				TRACK_UP_25,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_CABLE_LIFT_HILL
    { TRACK_WHOA_BELLY,				TRACK_VANGLE_WHOA_BELLY,	TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_REVERSE_WHOA_BELLY_SLOPE
    { TRACK_WHOA_BELLY,				TRACK_VANGLE_WHOA_BELLY,	TRACK_VANGLE_WHOA_BELLY,	TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_REVERSE_WHOA_BELLY_VERTICAL
    { TRACK_FLAT,					TRACK_UP_90,				TRACK_UP_90,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_90_DEG_UP
    { TRACK_FLAT,					TRACK_DOWN_90,				TRACK_DOWN_90,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_90_DEG_DOWN
    { TRACK_FLAT,					TRACK_UP_90,				TRACK_UP_60,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_60_DEG_UP_TO_90_DEG_UP
    { TRACK_FLAT,					TRACK_DOWN_60,				TRACK_DOWN_90,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_90_DEG_DOWN_TO_60_DEG_DOWN
    { TRACK_FLAT,					TRACK_UP_60,				TRACK_UP_90,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_90_DEG_UP_TO_60_DEG_UP
    { TRACK_FLAT,					TRACK_DOWN_90,				TRACK_DOWN_60,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_60_DEG_DOWN_TO_90_DEG_DOWN
    { TRACK_BRAKE_FOR_DROP,			TRACK_DOWN_60,				TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_BRAKE_FOR_DROP
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_EIGHTH_TO_DIAG
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_EIGHTH_TO_DIAG
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_EIGHTH_TO_ORTHOGONAL
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_LEFT,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_LEFT_EIGHTH_BANK_TO_DIAG
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_RIGHT,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_RIGHT_EIGHTH_BANK_TO_DIAG
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_LEFT,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_LEFT_EIGHTH_BANK_TO_ORTHOGONAL
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_RIGHT,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_RIGHT_EIGHTH_BANK_TO_ORTHOGONAL
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_FLAT
    { TRACK_FLAT,					TRACK_UP_25,				TRACK_UP_25,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_25_DEG_UP
    { TRACK_FLAT,					TRACK_UP_60,				TRACK_UP_60,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_60_DEG_UP
    { TRACK_FLAT,					TRACK_UP_25,				TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_FLAT_TO_25_DEG_UP
    { TRACK_FLAT,					TRACK_UP_60,				TRACK_UP_25,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_25_DEG_UP_TO_60_DEG_UP
    { TRACK_FLAT,					TRACK_UP_25,				TRACK_UP_60,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_60_DEG_UP_TO_25_DEG_UP
    { TRACK_FLAT,					TRACK_NONE,					TRACK_UP_25,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_25_DEG_UP_TO_FLAT
    { TRACK_FLAT,					TRACK_DOWN_25,				TRACK_DOWN_25,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_25_DEG_DOWN
    { TRACK_FLAT,					TRACK_DOWN_60,				TRACK_DOWN_60,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_60_DEG_DOWN
    { TRACK_FLAT,					TRACK_DOWN_25,				TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_FLAT_TO_25_DEG_DOWN
    { TRACK_FLAT,					TRACK_DOWN_60,				TRACK_DOWN_25,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_25_DEG_DOWN_TO_60_DEG_DOWN
    { TRACK_FLAT,					TRACK_DOWN_25,				TRACK_DOWN_60,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_60_DEG_DOWN_TO_25_DEG_DOWN
    { TRACK_FLAT,					TRACK_NONE,					TRACK_DOWN_25,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_25_DEG_DOWN_TO_FLAT
    { TRACK_FLAT,					TRACK_UP_60,				TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_FLAT_TO_60_DEG_UP
    { TRACK_FLAT,					TRACK_NONE,					TRACK_UP_60,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_60_DEG_UP_TO_FLAT
    { TRACK_FLAT,					TRACK_DOWN_60,				TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_FLAT_TO_60_DEG_DOWN
    { TRACK_FLAT,					TRACK_NONE,					TRACK_DOWN_60,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_60_DEG_DOWN_TO_FLAT
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_LEFT,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_FLAT_TO_LEFT_BANK
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_RIGHT,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_FLAT_TO_RIGHT_BANK
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_DIAG_LEFT_BANK_TO_FLAT
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_DIAG_RIGHT_BANK_TO_FLAT
    { TRACK_FLAT,					TRACK_UP_25,				TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_DIAG_LEFT_BANK_TO_25_DEG_UP
    { TRACK_FLAT,					TRACK_UP_25,				TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_DIAG_RIGHT_BANK_TO_25_DEG_UP
    { TRACK_FLAT,					TRACK_NONE,					TRACK_UP_25,				TRACK_BANK_LEFT,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_25_DEG_UP_TO_LEFT_BANK
    { TRACK_FLAT,					TRACK_NONE,					TRACK_UP_25,				TRACK_BANK_RIGHT,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_25_DEG_UP_TO_RIGHT_BANK
    { TRACK_FLAT,					TRACK_DOWN_25,				TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_DIAG_LEFT_BANK_TO_25_DEG_DOWN
    { TRACK_FLAT,					TRACK_DOWN_25,				TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_DIAG_RIGHT_BANK_TO_25_DEG_DOWN
    { TRACK_FLAT,					TRACK_NONE,					TRACK_DOWN_25,				TRACK_BANK_LEFT,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_25_DEG_DOWN_TO_LEFT_BANK
    { TRACK_FLAT,					TRACK_NONE,					TRACK_DOWN_25,				TRACK_BANK_RIGHT,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_DIAG_25_DEG_DOWN_TO_RIGHT_BANK
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_LEFT,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_DIAG_LEFT_BANK
    { TRACK_FLAT,					TRACK_NONE,					TRACK_NONE,					TRACK_BANK_RIGHT,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_DIAG_RIGHT_BANK
    { TRACK_LOG_FLUME_REVERSER,		TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LOG_FLUME_REVERSER
    { TRACK_SPINNING_TUNNEL,		TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_SPINNING_TUNNEL
    { TRACK_BARREL_ROLL,			TRACK_NONE,					TRACK_NONE,					TRACK_BANK_UPSIDE_DOWN,	TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_BARREL_ROLL_UP_TO_DOWN
    { TRACK_BARREL_ROLL,			TRACK_NONE,					TRACK_NONE,					TRACK_BANK_UPSIDE_DOWN,	TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_BARREL_ROLL_UP_TO_DOWN
    { TRACK_BARREL_ROLL,			TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_UPSIDE_DOWN,	TRACK_NONE					},	// ELEM_LEFT_BARREL_ROLL_DOWN_TO_UP
    { TRACK_BARREL_ROLL,			TRACK_NONE,					TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_UPSIDE_DOWN,	TRACK_NONE					},	// ELEM_RIGHT_BARREL_ROLL_DOWN_TO_UP
    { TRACK_FLAT,					TRACK_UP_25,				TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_LEFT,		TRACK_NONE					},	// ELEM_LEFT_BANK_TO_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP
    { TRACK_FLAT,					TRACK_UP_25,				TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_RIGHT,		TRACK_NONE					},	// ELEM_RIGHT_BANK_TO_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP
    { TRACK_FLAT,					TRACK_NONE,					TRACK_DOWN_25,				TRACK_BANK_LEFT,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_LEFT_BANK
    { TRACK_FLAT,					TRACK_NONE,					TRACK_DOWN_25,				TRACK_BANK_RIGHT,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_RIGHT_BANK
    { TRACK_POWERED_LIFT,			TRACK_UP_25,				TRACK_UP_25,				TRACK_BANK_NONE,		TRACK_BANK_NONE,		TRACK_NONE					},	// ELEM_POWERED_LIFT
    { TRACK_HALF_LOOP_2,			TRACK_NONE,					TRACK_UP_25,				TRACK_BANK_UPSIDE_DOWN,	TRACK_BANK_NONE,		TRACK_HALF_LOOP_UP			},	// ELEM_LEFT_LARGE_HALF_LOOP_UP
    { TRACK_HALF_LOOP_2,			TRACK_NONE,					TRACK_UP_25,				TRACK_BANK_UPSIDE_DOWN,	TRACK_BANK_NONE,		TRACK_HALF_LOOP_UP			},	// ELEM_RIGHT_LARGE_HALF_LOOP_UP
    { TRACK_HALF_LOOP_2,			TRACK_DOWN_25,				TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_UPSIDE_DOWN,	TRACK_HALF_LOOP_DOWN		},	// ELEM_RIGHT_LARGE_HALF_LOOP_DOWN
    { TRACK_HALF_LOOP_2,			TRACK_DOWN_25,				TRACK_NONE,					TRACK_BANK_NONE,		TRACK_BANK_UPSIDE_DOWN,	TRACK_HALF_LOOP_DOWN		},	// ELEM_LEFT_LARGE_HALF_LOOP_DOWN
};

static bool track_save_should_select_scenery_around(int rideIndex, rct_map_element *mapElement);
static void track_save_select_nearby_scenery_for_tile(int rideIndex, int cx, int cy);
static void track_save_add_map_element(int mapElementSelectType, int x, int y, rct_map_element *mapElement);

uint32* sub_6AB49A(rct_object_entry* entry){
	rct_object_entry* object_list_entry = object_list_find(entry);

	if (object_list_entry == NULL) return NULL;

	// Return the address of the last value of the list entry
	return (((uint32*)object_get_next(object_list_entry)) - 1);
}

static void get_track_idx_path(char *outPath)
{
	platform_get_user_directory(outPath, NULL);
	strcat(outPath, "tracks.idx");
}

static void track_list_query_directory(int *outTotalFiles){
	int enumFileHandle;
	file_info enumFileInfo;

	*outTotalFiles = 0;

	// Enumerate through each track in the directory
	enumFileHandle = platform_enumerate_files_begin(RCT2_ADDRESS(RCT2_ADDRESS_TRACKS_PATH, char));
	if (enumFileHandle == INVALID_HANDLE)
		return;

	while (platform_enumerate_files_next(enumFileHandle, &enumFileInfo)) {
		(*outTotalFiles)++;
	}
	platform_enumerate_files_end(enumFileHandle);
}

static int track_list_cache_save(int fileCount, uint8* track_list_cache, uint32 track_list_size){
	char path[MAX_PATH];
	FILE *file;

	log_verbose("saving track list cache (tracks.idx)");
	get_track_idx_path(path);
	file = fopen(path, "wb");
	if (file == NULL) {
		log_error("Failed to save %s", path);
		return 0;
	}

	fwrite(&fileCount, sizeof(int), 1, file);
	fwrite(track_list_cache, track_list_size, 1, file);
	uint8 last_entry = 0xFE;
	fwrite(&last_entry, 1, 1, file);
	fclose(file);
	return 1;
}

static uint8* track_list_cache_load(int totalFiles){
	char path[MAX_PATH];
	FILE *file;

	log_verbose("loading track list cache (tracks.idx)");
	get_track_idx_path(path);
	file = fopen(path, "rb");
	if (file == NULL) {
		log_error("Failed to load %s", path);
		return 0;
	}

	uint8* track_list_cache;
	uint32 fileCount;
	// Remove 4 for the file count variable
	long track_list_size = fsize(file) - 4;

	if (track_list_size < 0) return 0;

	fread(&fileCount, 4, 1, file);
	
	if (fileCount != totalFiles){
		log_verbose("Track file count is different.");
		return 0;
	}

	track_list_cache = malloc(track_list_size);
	fread(track_list_cache, track_list_size, 1, file);
	return track_list_cache;
}

void track_list_populate(ride_list_item item, uint8* track_list_cache){
	uint8* track_pointer = track_list_cache;

	uint8 cur_track_entry_index = 0;
	for (uint8 track_type = *track_pointer++; track_type != 0xFE;
		track_pointer += strlen(track_pointer) + 1,
		track_type = *track_pointer++){
		rct_object_entry* track_object = (rct_object_entry*)track_pointer;
		track_pointer += sizeof(rct_object_entry);

		if (track_type != item.type){
			continue;
		}

		uint8 entry_type, entry_index;
		if (item.entry_index != 0xFF){

			if (!find_object_in_entry_group(track_object, &entry_type, &entry_index))continue;

			if (item.entry_index != entry_index)continue;
		}
		else{
			if (find_object_in_entry_group(track_object, &entry_type, &entry_index)){
				if (GET_RIDE_ENTRY(entry_index)->flags & 
					(RIDE_ENTRY_FLAG_SEPERATE_RIDE_NAME | 
					RIDE_ENTRY_FLAG_SEPERATE_RIDE))
					continue;
			}
			else{
				uint32* esi = sub_6AB49A(track_object);
				if (esi == NULL) continue;
				if (*esi & 0x1000000)continue;
			}
		}

		// If cur_track_entry_index is greater than max number of tracks
		if (cur_track_entry_index >= 1000){
			RCT2_GLOBAL(0xF635ED, uint8) |= 1;
			break;
		}

		uint8 track_entry_index = 0;
		uint8 isBelow = 0;
		for (; track_entry_index != cur_track_entry_index; track_entry_index++){
			if (strcicmp(track_pointer, &RCT2_ADDRESS(RCT2_ADDRESS_TRACK_LIST, uint8)[track_entry_index * 128]) < 0){
				isBelow = 1;
				break;
			}
		}

		if (isBelow == 1){
			memmove(
				&RCT2_ADDRESS(RCT2_ADDRESS_TRACK_LIST, uint8)[track_entry_index * 128 + 128], 
				&RCT2_ADDRESS(RCT2_ADDRESS_TRACK_LIST, uint8)[track_entry_index * 128], 
				(cur_track_entry_index - track_entry_index) * 128);
		}

		strcpy(&RCT2_ADDRESS(RCT2_ADDRESS_TRACK_LIST, uint8)[track_entry_index * 128], track_pointer);
		cur_track_entry_index++;
	}

	RCT2_ADDRESS(RCT2_ADDRESS_TRACK_LIST, uint8)[cur_track_entry_index * 128] = '\0';
	free(track_list_cache);
}

/**
 *
 *  rct2: 0x006CED50
 */
void track_load_list(ride_list_item item)
{
	RCT2_GLOBAL(0xF635ED, uint8) = 0;

	if (item.type < 0x80){
		rct_ride_type* ride_type = gRideTypeList[item.entry_index];
		if (!(ride_type->flags & RIDE_ENTRY_FLAG_SEPERATE_RIDE)){
			item.entry_index = 0xFF;
		}
	}

	int totalFiles;
	
	track_list_query_directory(&totalFiles);

	uint8* track_list_cache;

	if (item.type == 0xFC || !(track_list_cache = track_list_cache_load(totalFiles))){
		uint8* new_track_file;

		new_track_file = malloc(0x40000);

		uint8* new_file_pointer = new_track_file;
		file_info enumFileInfo;

		int enumFileHandle = platform_enumerate_files_begin(RCT2_ADDRESS(RCT2_ADDRESS_TRACKS_PATH, char));
		if (enumFileHandle == INVALID_HANDLE)
			return;

		while (platform_enumerate_files_next(enumFileHandle, &enumFileInfo)) {
			if (new_file_pointer > new_track_file + 0x3FF00)break;

			char path[MAX_PATH];
			subsitute_path(path, RCT2_ADDRESS(RCT2_ADDRESS_TRACKS_PATH, char), enumFileInfo.path);

			rct_track_td6* loaded_track = load_track_design(path);
			if (loaded_track){
				*new_file_pointer++ = loaded_track->type;
			}
			else{
				*new_file_pointer++ = 0xFF;
				// Garbage object
				new_file_pointer += sizeof(rct_object_entry);
				// Empty string
				*new_file_pointer++ = '\0';
				// Unsure why it previously didn't continue on load fail??
				continue;
			}
			memcpy(new_file_pointer, &loaded_track->vehicle_object, sizeof(rct_object_entry));
			new_file_pointer += sizeof(rct_object_entry);

			int file_name_length = strlen(enumFileInfo.path);
			strcpy(new_file_pointer, enumFileInfo.path);
			new_file_pointer += file_name_length + 1;
		}
		platform_enumerate_files_end(enumFileHandle);

		if (!track_list_cache_save(totalFiles, new_track_file, new_file_pointer - new_track_file)){
			log_error("Track list failed to save.");
			return;
		}
		free(new_track_file);

		track_list_cache = track_list_cache_load(totalFiles);
		if (!track_list_cache){
			log_error("Track list failed to load after new save");
			return;
		}
	}

	track_list_populate(item, track_list_cache);
	free(track_list_cache);
}

static void read(void *dst, char **src, int length)
{
	memcpy(dst, *src, length);
	*src += length;
}

/* rct2: 0x00677530 
 * Returns 1 if it has booster track elements
 */
uint8 td4_track_has_boosters(rct_track_td6* track_design, uint8* track_elements){
	if (track_design->type == RCT1_RIDE_TYPE_HEDGE_MAZE)
		return 0;

	rct_track_element* track_element = (rct_track_element*)track_elements;

	for (; track_element->type != 0xFF; track_element++){
		if (track_element->type == RCT1_TRACK_ELEM_BOOSTER)
			return 1;
	}

	return 0;
}

/**
 *
 *  rct2: 0x0067726A
 * path: 0x0141EF68
 */
rct_track_td6* load_track_design(const char *path)
{
	FILE *fp;
	long fpLength;
	char *fpBuffer, *decoded, *src;
	int i, decodedLength;
	uint8* edi;

	RCT2_GLOBAL(0x009AAC54, uint8) = 1;
	
	fp = fopen(path, "rb");
	if (fp == NULL)
		return 0;

	char* track_name_pointer = (char*)path;
	while (*track_name_pointer++ != '\0');
	while (*--track_name_pointer != '\\');
	char* default_name = RCT2_ADDRESS(0x009E3504, char);
	// Copy the track name for use as the default name of this ride
	while (*++track_name_pointer != '.')*default_name++ = *track_name_pointer;
	*default_name++ = '\0';

	// Read whole file into a buffer
	fpLength = fsize(fp);
	fpBuffer = malloc(fpLength);
	fread(fpBuffer, fpLength, 1, fp);
	fclose(fp);

	// Validate the checksum
	// Not the same checksum algorithm as scenarios and saved games
	if (!sawyercoding_validate_track_checksum(fpBuffer, fpLength)){
		log_error("Track checksum failed.");
		return 0;
	}

	// Decode the track data
	decoded = malloc(0x10000);
	decodedLength = sawyercoding_decode_td6(fpBuffer, decoded, fpLength);
	realloc(decoded, decodedLength);
	free(fpBuffer);

	rct_track_td6* track_design = RCT2_ADDRESS(0x009D8178, rct_track_td6);
	// Read decoded data
	src = decoded;
	// Clear top of track_design as this is not loaded from the td4 files
	memset(&track_design->track_spine_colour, 0, 67);
	// Read start of track_design
	read(track_design, &src, 32);

	uint8 version = track_design->version_and_colour_scheme >> 2;

	if (version > 2){
		free(decoded);
		return NULL;
	}

	// In td6 there are 32 sets of two byte vehicle colour specifiers 
	// In td4 there are 12 sets so the remaining 20 need to be read.
	if (version == 2)
		read(&track_design->vehicle_colours[12], &src, 40);

	read(&track_design->pad_48, &src, 24);

	// In td4 (version AA/CF) and td6 both start actual track data at 0xA3
	if (version > 0)
		read(&track_design->track_spine_colour, &src, version == 1 ? 140 : 67);

	uint8* track_elements = RCT2_ADDRESS(0x9D821B, uint8);
	// Read the actual track data.
	read(track_elements, &src, 24572);
	
	uint8* final_track_element_location = track_elements + 24572;
	free(decoded);

	// td4 files require some work to be recognised as td6.
	if (version < 2) {

		// Set any element passed the tracks to 0xFF
		if (track_design->type == RIDE_TYPE_MAZE) {
			rct_maze_element* maze_element = (rct_maze_element*)track_elements;
			while (maze_element->all != 0) {
				maze_element++;
			}
			maze_element++;
			memset(maze_element, 255, final_track_element_location - (uint8*)maze_element);
		}
		else {
			rct_track_element* track_element = (rct_track_element*)track_elements;
			while (track_element->type != 255) {
				track_element++;
			}
			memset(((uint8*)track_element) + 1, 255, final_track_element_location - (uint8*)track_element);
		
		}

		// Edit the colours to use the new versions
		// Unsure why it is 67
		edi = (uint8*)&track_design->vehicle_colours;
		for (i = 0; i < 67; i++, edi++)
			*edi = RCT2_ADDRESS(0x0097F0BC, uint8)[*edi];

		// Edit the colours to use the new versions
		edi = (uint8*)&track_design->track_spine_colour;
		for (i = 0; i < 12; i++, edi++)
			*edi = RCT2_ADDRESS(0x0097F0BC, uint8)[*edi];

		// Highest drop height is 1bit = 3/4 a meter in td6
		// Highest drop height is 1bit = 1/3 a meter in td4
		// Not sure if this is correct??
		track_design->highest_drop_height >>= 1;

		// If it has boosters then sadly track has to be discarded.
		if (td4_track_has_boosters(track_design, track_elements))
			track_design->type = RIDE_TYPE_NULL;

		if (track_design->type == RCT1_RIDE_TYPE_STEEL_MINI_ROLLER_COASTER)
			track_design->type = RIDE_TYPE_NULL;

		if (track_design->type == RCT1_RIDE_TYPE_WOODEN_ROLLER_COASTER)
			track_design->type = RIDE_TYPE_WOODEN_ROLLER_COASTER;

		if (track_design->type == RIDE_TYPE_CORKSCREW_ROLLER_COASTER) {
			if (track_design->ride_mode == RCT1_RIDE_MODE_POWERED_LAUNCH)
				track_design->ride_mode = RIDE_MODE_POWERED_LAUNCH;
			if (track_design->vehicle_type == 79) {
				if (track_design->ride_mode == 2)
					track_design->ride_mode = 1;
			}
		}

		rct_object_entry* vehicle_object;
		if (track_design->type == RIDE_TYPE_MAZE) {
			vehicle_object = RCT2_ADDRESS(0x0097F66C, rct_object_entry);
		} else {
			int vehicle_type = track_design->vehicle_type;
			if (vehicle_type == 3 && track_design->type == RIDE_TYPE_INVERTED_ROLLER_COASTER)
				vehicle_type = 80;
			vehicle_object = &RCT2_ADDRESS(0x0097F0DC, rct_object_entry)[vehicle_type];
		}

		memcpy(&track_design->vehicle_object, vehicle_object, sizeof(rct_object_entry));
		for (i = 0; i < 32; i++)
			track_design->vehicle_additional_colour[i] = track_design->vehicle_colours[i].trim_colour;

		track_design->space_required_x = 255;
		track_design->space_required_y = 255;
		track_design->lift_hill_speed_num_circuits = 5;
	}

	track_design->var_50 = min(
		track_design->var_50,
		RCT2_GLOBAL(RCT2_ADDRESS_RIDE_FLAGS + 5 + (track_design->type * 8), uint8)
	);

	return track_design;
}

/* rct2: 0x006D1DCE*/
void reset_track_list_cache(){
	int* track_list_cache = RCT2_ADDRESS(RCT2_ADDRESS_TRACK_DESIGN_INDEX_CACHE, int);
	for (int i = 0; i < 4; ++i){
		track_list_cache[i] = -1;
	}
	RCT2_GLOBAL(RCT2_ADDRESS_TRACK_DESIGN_NEXT_INDEX_CACHE, uint32) = 0;
}

/* rct2: 0x006D1C68 */
int backup_map(){
	RCT2_GLOBAL(0xF440ED, uint8*) = malloc(0xED600);
	if (RCT2_GLOBAL(0xF440ED, uint32) == 0) return 0;
	
	RCT2_GLOBAL(0xF440F1, uint8*) = malloc(0x40000);
	if (RCT2_GLOBAL(0xF440F1, uint32) == 0){
		free(RCT2_GLOBAL(0xF440ED, uint8*));
		return 0;
	}

	RCT2_GLOBAL(0xF440F5, uint8*) = malloc(14);
	if (RCT2_GLOBAL(0xF440F5, uint32) == 0){
		free(RCT2_GLOBAL(0xF440ED, uint8*));
		free(RCT2_GLOBAL(0xF440F1, uint8*));
		return 0;
	}

	uint32* map_elements = RCT2_ADDRESS(RCT2_ADDRESS_MAP_ELEMENTS, uint32);
	memcpy(RCT2_GLOBAL(0xF440ED, uint32*), map_elements, 0xED600);

	uint32* tile_map_pointers = RCT2_ADDRESS(RCT2_ADDRESS_TILE_MAP_ELEMENT_POINTERS, uint32);
	memcpy(RCT2_GLOBAL(0xF440F1, uint32*), tile_map_pointers, 0x40000);

	uint8* backup_info = RCT2_GLOBAL(0xF440F5, uint8*);
	*(uint32*)backup_info = RCT2_GLOBAL(0x0140E9A4, uint32);
	*(uint16*)(backup_info + 4) = RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE_UNITS, uint16);
	*(uint16*)(backup_info + 6) = RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE_MINUS_2, uint16);
	*(uint16*)(backup_info + 8) = RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE, uint16);
	*(uint32*)(backup_info + 10) = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32);
	return 1;
}

/* rct2: 0x006D2378 */
void reload_map_backup(){
	uint32* map_elements = RCT2_ADDRESS(RCT2_ADDRESS_MAP_ELEMENTS, uint32);
	memcpy(map_elements, RCT2_GLOBAL(0xF440ED, uint32*), 0xED600);

	uint32* tile_map_pointers = RCT2_ADDRESS(RCT2_ADDRESS_TILE_MAP_ELEMENT_POINTERS, uint32);
	memcpy(tile_map_pointers, RCT2_GLOBAL(0xF440F1, uint32*), 0x40000);

	uint8* backup_info = RCT2_GLOBAL(0xF440F5, uint8*);
	RCT2_GLOBAL(0x0140E9A4, uint32) = *(uint32*)backup_info;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE_UNITS, uint16) = *(uint16*)(backup_info + 4);
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE_MINUS_2, uint16) = *(uint16*)(backup_info + 6);
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE, uint16) = *(uint16*)(backup_info + 8);
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32) = *(uint32*)(backup_info + 10);

	free(RCT2_GLOBAL(0xF440ED, uint8*));
	free(RCT2_GLOBAL(0xF440F1, uint8*));
	free(RCT2_GLOBAL(0xF440F5, uint8*));
}

/* rct2: 0x006D1D9A */
void blank_map(){

	// These values were previously allocated in backup map but
	// it seems more fitting to place in this function
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE_UNITS, uint16) = 0x1FE0;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE_MINUS_2, uint16) = 0x20FE;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE, uint16) = 0x100;

	rct_map_element* map_element;
	for (int i = 0; i < MAX_TILE_MAP_ELEMENT_POINTERS; i++) {
		map_element = GET_MAP_ELEMENT(i);
		map_element->type = MAP_ELEMENT_TYPE_SURFACE;
		map_element->flags = MAP_ELEMENT_FLAG_LAST_TILE;
		map_element->base_height = 2;
		map_element->clearance_height = 0;
		map_element->properties.surface.slope = 0;
		map_element->properties.surface.terrain = 0;
		map_element->properties.surface.grass_length = 1;
		map_element->properties.surface.ownership = OWNERSHIP_OWNED;
	}
	map_update_tile_pointers();
}

/* rct2: 0x006ABDB0 */
void load_track_scenery_objects(){
	uint8 entry_index = RCT2_GLOBAL(0xF44157, uint8);
	rct_object_entry_extended* object_entry = &object_entry_groups[0].entries[entry_index];

	rct_object_entry* copied_entry = RCT2_ADDRESS(0xF43414, rct_object_entry);
	memcpy(copied_entry, object_entry, sizeof(rct_object_entry));

	object_unload_all();
	object_load(-1, copied_entry, 0);
	uint8 entry_type;
	find_object_in_entry_group(copied_entry, &entry_type, &entry_index);
	RCT2_GLOBAL(0xF44157, uint8) = entry_index;

	rct_track_td6* track_design = RCT2_ADDRESS(0x009D8178, rct_track_td6);
	uint8* track_elements = RCT2_ADDRESS(0x9D821B, uint8);

	if (track_design->type == RIDE_TYPE_MAZE){
		// Skip all of the maze track elements
		while (*(uint32*)track_elements != 0)track_elements += sizeof(rct_maze_element);
		track_elements += sizeof(rct_maze_element);
	}
	else{
		// Skip track_elements
		while (*track_elements != 255) track_elements += sizeof(rct_track_element);
		track_elements++;

		// Skip entrance exit elements
		while (*track_elements != 255) track_elements += sizeof(rct_track_entrance);
		track_elements++;
	}

	while (*track_elements != 255){
		rct_track_scenery* scenery_entry = (rct_track_scenery*)track_elements;

		if (!find_object_in_entry_group(&scenery_entry->scenery_object, &entry_type, &entry_index)){
			object_load(-1, &scenery_entry->scenery_object, 0);
		}
		// Skip object and location/direction/colour
		track_elements += sizeof(rct_track_scenery);
	}

	reset_loaded_objects();
}

/* rct2: 0x006D247A */
void track_mirror_scenery(uint8** track_elements){
	rct_track_scenery* scenery = (rct_track_scenery*)*track_elements;
	for (; (scenery->scenery_object.flags & 0xFF) != 0xFF; scenery++){
		uint8 entry_type, entry_index;
		if (!find_object_in_entry_group(&scenery->scenery_object, &entry_type, &entry_index)){
			entry_type = scenery->scenery_object.flags & 0xF;
			if (entry_type != OBJECT_TYPE_PATHS)
				continue;
		}

		rct_scenery_entry* scenery_entry = (rct_scenery_entry*)object_entry_groups[entry_type].chunks[entry_index];

		switch (entry_type){
		case OBJECT_TYPE_LARGE_SCENERY:
		{
			sint16 x1 = 0, x2 = 0, y1 = 0, y2 = 0;
			for (rct_large_scenery_tile* tile = scenery_entry->large_scenery.tiles;
				tile->x_offset != -1;
				tile++)
			{
				if (x1 > tile->x_offset)
					x1 = tile->x_offset;
				if (x2 < tile->x_offset)
					x2 = tile->x_offset;
				if (y1 > tile->y_offset)
					y1 = tile->y_offset;
				if (y2 > tile->y_offset)
					y2 = tile->y_offset;
			}

			switch (scenery->flags & 3){
			case 0:
				scenery->y = (-(scenery->y * 32 + y1) - y2) / 32;
				break;
			case 1:
				scenery->x = (scenery->x * 32 + y2 + y1) / 32;
				scenery->y = (-(scenery->y * 32)) / 32;
				scenery->flags ^= (1 << 1);
				break;
			case 2:
				scenery->y = (-(scenery->y * 32 - y2) + y1) / 32;
				break;
			case 3:
				scenery->x = (scenery->x * 32 - y2 - y1) / 32;
				scenery->y = (-(scenery->y * 32)) / 32;
				scenery->flags ^= (1 << 1);
				break;
			}
			break;
		}
		case OBJECT_TYPE_SMALL_SCENERY:
			scenery->y = -scenery->y;

			if (scenery_entry->small_scenery.flags & SMALL_SCENERY_FLAG9){
				scenery->flags ^= (1 << 0);
				if (!(scenery_entry->small_scenery.flags & SMALL_SCENERY_FLAG_FULL_TILE)){
					scenery->flags ^= (1 << 2);
				}
				break;
			}
			if (scenery->flags & (1 << 0))
				scenery->flags ^= (1 << 1);

			scenery->flags ^= (1 << 2);
			break;

		case OBJECT_TYPE_WALLS:
			scenery->y = -scenery->y;
			if (scenery->flags & (1 << 0))
				scenery->flags ^= (1 << 1);
			break;

		case OBJECT_TYPE_PATHS:
			scenery->y = -scenery->y;

			if (scenery->flags & (1 << 5)){
				scenery->flags ^= (1 << 6);
			}

			uint8 flags = scenery->flags;
			flags = ((flags& (1 << 3)) >> 2) | ((flags & (1 << 1)) << 2);
			scenery->flags &= 0xF5;
			scenery->flags |= flags;
		}

	}
}

/* rct2: 0x006D2443 */
void track_mirror_ride(uint8** track_elements){
	rct_track_element* track = (rct_track_element*)*track_elements;
	for (; track->type != 0xFF; track++){
		track->type = RCT2_ADDRESS(0x0099EA1C, uint8)[track->type];
	}
	*track_elements = (uint8*)track + 1;

	rct_track_entrance* entrance = (rct_track_entrance*)*track_elements;
	for (; entrance->z != -1; entrance++){
		entrance->y = -entrance->y;
		if (entrance->direction & 1){
			entrance->direction ^= (1 << 1);
		}
	}
	*track_elements = (uint8*)entrance + 1;
}

/* rct2: 0x006D25FA */
void track_mirror_maze(uint8** track_elements){
	rct_maze_element* maze = (rct_maze_element*)*track_elements;
	for (; maze->all != 0; maze++){
		maze->y = -maze->y;

		if (maze->type == 0x8 || maze->type == 0x80){
			if (maze->unk_2 & 1){
				maze->unk_2 ^= (1 << 1);
			}
			continue;
		}

		uint16 maze_entry = maze->maze_entry;
		uint16 new_entry = 0;
		for (uint8 position = bitscanforward(maze_entry);
			position != 0xFF;
			position = bitscanforward(maze_entry)){
			maze_entry &= ~(1 << position);
			new_entry |= (1 << RCT2_ADDRESS(0x00993EDC, uint8)[position]);
		}
		maze->maze_entry = new_entry;
	}
	*track_elements = (uint8*)maze + 4;
}

/* rct2: 0x006D2436 */
void track_mirror(){
	uint8* track_elements = RCT2_ADDRESS(0x009D821B, uint8);

	rct_track_td6* track_design = RCT2_ADDRESS(0x009D8178, rct_track_td6);

	if (track_design->type == RIDE_TYPE_MAZE){
		track_mirror_maze(&track_elements);
	}
	else{
		track_mirror_ride(&track_elements);
	}

	track_mirror_scenery(&track_elements);
}

void track_update_max_min_coordinates(sint16 x, sint16 y, sint16 z){
	if (x < RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_X_MIN, sint16)){
		RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_X_MIN, sint16) = x;
	}

	if (x > RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_X_MAX, sint16)){
		RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_X_MAX, sint16) = x;
	}

	if (y < RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_Y_MIN, sint16)){
		RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_Y_MIN, sint16) = y;
	}

	if (y > RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_Y_MAX, sint16)){
		RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_Y_MAX, sint16) = y;
	}

	if (z < RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_Z_MIN, sint16)){
		RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_Z_MIN, sint16) = z;
	}

	if (z > RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_Z_MAX, sint16)){
		RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_Z_MAX, sint16) = z;
	}
}

/* rct2: 0x006D0964 */
int track_place_scenery(rct_track_scenery* scenery_start, uint8 rideIndex, int originX, int originY, int originZ){
	RCT2_GLOBAL(0x00F44050, rct_track_scenery*) = scenery_start;

	// mode
	RCT2_GLOBAL(0x00F44154, uint8) = 0;

	for (uint8 mode = 0; mode <= 1; mode++){
		if ((scenery_start->scenery_object.flags & 0xFF) != 0xFF)
			RCT2_GLOBAL(0x00F4414E, uint8) |= 1 << 2;

		if (RCT2_GLOBAL(0x00F4414E, uint8) & (1 << 7))
			continue;

		for (rct_track_scenery* scenery = scenery_start; (scenery->scenery_object.flags & 0xFF) != 0xFF; scenery++){

			uint8 rotation = RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_ROTATION, uint8);

			rct_xy8 tile = { .x = originX / 32, .y = originY / 32 };
			switch (rotation & 3){
			case MAP_ELEMENT_DIRECTION_WEST:
				tile.x += scenery->x;
				tile.y += scenery->y;
				break;
			case MAP_ELEMENT_DIRECTION_NORTH:
				tile.x += scenery->y;
				tile.y -= scenery->x;
				break;
			case MAP_ELEMENT_DIRECTION_EAST:
				tile.x -= scenery->x;
				tile.y -= scenery->y;
				break;
			case MAP_ELEMENT_DIRECTION_SOUTH:
				tile.x -= scenery->y;
				tile.y += scenery->x;
				break;
			}

			rct_xy16 mapCoord = { .x = tile.x * 32, .y = tile.y * 32 };
			track_update_max_min_coordinates(mapCoord.x, mapCoord.y, originZ);

			if (RCT2_GLOBAL(0x00F440D4, uint8) == 0 &&
				mode == 0){
				uint8 new_tile = 1;
				rct_xy16* selectionTile = gMapSelectionTiles;
				for (; selectionTile->x != -1; selectionTile++){
					if (selectionTile->x == tile.x && selectionTile->y == tile.y){
						new_tile = 0;
						break;
					}
					if (selectionTile + 1 >= &gMapSelectionTiles[300]){
						new_tile = 0;
						break;
					}
				}
				if (new_tile){
					selectionTile->x = tile.x;
					selectionTile->y = tile.y;
					selectionTile++;
					selectionTile->x = -1;
				}
			}

			if (RCT2_GLOBAL(0x00F440D4, uint8) == 6 &&
				mode == 0){

				uint8 entry_type, entry_index;
				if (!find_object_in_entry_group(&scenery->scenery_object, &entry_type, &entry_index)){
					entry_type = scenery->scenery_object.flags & 0xF;
					if (entry_type != OBJECT_TYPE_PATHS)
						entry_type = 0xFF;
					if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8)&SCREEN_FLAGS_TRACK_DESIGNER)
						entry_type = 0xFF;

					entry_index = 0;
					for (rct_path_type* path = g_pathTypeEntries[0]; 
						entry_index < object_entry_group_counts[OBJECT_TYPE_PATHS]; 
						path = g_pathTypeEntries[entry_index], entry_index++){

						if (path == (rct_path_type*)-1)
							continue;
						if (path->flags & (1 << 2))
							continue;
					}

					if (entry_index == object_entry_group_counts[OBJECT_TYPE_PATHS])
						entry_type = 0xFF;
				}
				int z;
				switch (entry_type){
				case OBJECT_TYPE_SMALL_SCENERY:
					//bl
					rotation += scenery->flags;
					rotation &= 3;

					//bh
					uint8 quadrant = (scenery->flags >> 2) + RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_ROTATION, uint8);
					quadrant &= 3;

					uint8 bh = rotation | (quadrant << 6) | MAP_ELEMENT_TYPE_SCENERY;

					rct_scenery_entry* small_scenery = g_smallSceneryEntries[entry_index];
					if (!(small_scenery->small_scenery.flags & SMALL_SCENERY_FLAG_FULL_TILE) &&
						(small_scenery->small_scenery.flags & SMALL_SCENERY_FLAG9)){
						bh = bh;
					}
					else if (small_scenery->small_scenery.flags & (SMALL_SCENERY_FLAG9 | SMALL_SCENERY_FLAG24 | SMALL_SCENERY_FLAG25)){
						bh &= 0x3F;
					}

					z = (scenery->z * 8 + originZ) / 8;
					game_do_command(
						mapCoord.x,
						0x69 | bh << 8,
						mapCoord.y,
						(entry_index << 8) | z,
						GAME_COMMAND_REMOVE_SCENERY,
						0,
						0);
					break;
				case OBJECT_TYPE_LARGE_SCENERY:
					z = (scenery->z * 8 + originZ) / 8;
					game_do_command(
						mapCoord.x,
						0x69 | (((rotation + scenery->flags) & 0x3) << 8),
						mapCoord.y,
						z,
						GAME_COMMAND_REMOVE_LARGE_SCENERY,
						0,
						0);
					break;
				case OBJECT_TYPE_WALLS:
					z = (scenery->z * 8 + originZ) / 8;
					game_do_command(
						mapCoord.x,
						0x69,
						mapCoord.y,
						(z << 8) | ((rotation + scenery->flags) & 0x3),
						GAME_COMMAND_REMOVE_FENCE,
						0,
						0);
					break;
				case OBJECT_TYPE_PATHS:
					z = (scenery->z * 8 + originZ) / 8;
					footpath_remove(mapCoord.x, mapCoord.y, z, 0x69);
					break;
				}
			}

			if (RCT2_GLOBAL(0x00F440D4, uint8) == 3){
				int z = scenery->z * 8 + RCT2_GLOBAL(0x00F440D5, sint16);
				if (z < RCT2_GLOBAL(0x00F44129, sint16)){
					RCT2_GLOBAL(0x00F44129, sint16) = z;
				}
			}
			
			if (RCT2_GLOBAL(0x00F440D4, uint8) == 1 ||
				RCT2_GLOBAL(0x00F440D4, uint8) == 2 ||
				RCT2_GLOBAL(0x00F440D4, uint8) == 3 ||
				RCT2_GLOBAL(0x00F440D4, uint8) == 4 ||
				RCT2_GLOBAL(0x00F440D4, uint8) == 5){

				uint8 entry_type, entry_index;
				if (!find_object_in_entry_group(&scenery->scenery_object, &entry_type, &entry_index)){
					entry_type = scenery->scenery_object.flags & 0xF;
					if (entry_type != OBJECT_TYPE_PATHS){
						RCT2_GLOBAL(0x00F4414E, uint8) |= 1 << 1;
						continue;
					}

					if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8)&SCREEN_FLAGS_TRACK_DESIGNER){
						RCT2_GLOBAL(0x00F4414E, uint8) |= 1 << 1;
						continue;
					}

					entry_index = 0;
					for (rct_path_type* path = g_pathTypeEntries[0];
						entry_index < object_entry_group_counts[OBJECT_TYPE_PATHS];
						path = g_pathTypeEntries[entry_index], entry_index++){

						if (path == (rct_path_type*)-1)
							continue;
						if (path->flags & (1 << 2))
							continue;
					}

					if (entry_index == object_entry_group_counts[OBJECT_TYPE_PATHS]){
						RCT2_GLOBAL(0x00F4414E, uint8) |= 1 << 1;
						continue;
					}
				}

				money32 cost;
				sint16 z;
				uint8 bl;

				switch (entry_type){
				case OBJECT_TYPE_SMALL_SCENERY:
					if (mode != 0)
						continue;
					if (RCT2_GLOBAL(0x00F440D4, uint8) == 3)
						continue;

					rotation += scenery->flags;
					rotation &= 3;
					z = scenery->z * 8 + originZ;
					uint8 quadrant = ((scenery->flags >> 2) + RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_ROTATION, uint8)) & 3;

					bl = 0x81;
					if (RCT2_GLOBAL(0x00F440D4, uint8) == 5)bl = 0xA9;
					if (RCT2_GLOBAL(0x00F440D4, uint8) == 4)bl = 0xE9;
					if (RCT2_GLOBAL(0x00F440D4, uint8) == 1)bl = 0x80;

					RCT2_GLOBAL(0x00141E9AE, rct_string_id) = 1161;

					cost = game_do_command(
						mapCoord.x,
						bl | (entry_index << 8),
						mapCoord.y,
						quadrant | (scenery->primary_colour << 8),
						GAME_COMMAND_PLACE_SCENERY,
						rotation | (scenery->secondary_colour << 16),
						z
						);

					if (cost == MONEY32_UNDEFINED)
						cost = 0;
					break;
				case OBJECT_TYPE_LARGE_SCENERY:
					if (mode != 0)
						continue;
					if (RCT2_GLOBAL(0x00F440D4, uint8) == 3)
						continue;

					rotation += scenery->flags;
					rotation &= 3;

					z = scenery->z * 8 + originZ;

					bl = 0x81;
					if (RCT2_GLOBAL(0x00F440D4, uint8) == 5)bl = 0xA9;
					if (RCT2_GLOBAL(0x00F440D4, uint8) == 4)bl = 0xE9;
					if (RCT2_GLOBAL(0x00F440D4, uint8) == 1)bl = 0x80;

					cost = game_do_command(
						mapCoord.x,
						bl | (rotation << 8),
						mapCoord.y,
						scenery->primary_colour | (scenery->secondary_colour << 8),
						GAME_COMMAND_PLACE_LARGE_SCENERY,
						entry_index,
						z
						);

					if (cost == MONEY32_UNDEFINED)
						cost = 0;
					break;
				case OBJECT_TYPE_WALLS:
					if (mode != 0)
						continue;
					if (RCT2_GLOBAL(0x00F440D4, uint8) == 3)
						continue;

					z = scenery->z * 8 + originZ;
					rotation += scenery->flags;
					rotation &= 3;

					bl = 1;
					if (RCT2_GLOBAL(0x00F440D4, uint8) == 5)bl = 0xA9;
					if (RCT2_GLOBAL(0x00F440D4, uint8) == 4)bl = 105;
					if (RCT2_GLOBAL(0x00F440D4, uint8) == 1)bl = 0;

					RCT2_GLOBAL(0x00141E9AE, rct_string_id) = 1811;

					cost = game_do_command(
						mapCoord.x, 
						bl | (entry_index << 8), 
						mapCoord.y, 
						rotation | (scenery->primary_colour << 8), 
						GAME_COMMAND_41, 
						z, 
						scenery->secondary_colour | ((scenery->flags & 0xFC) << 6)
						);

					if (cost == MONEY32_UNDEFINED)
						cost = 0;
					break;
				case OBJECT_TYPE_PATHS:
					if (RCT2_GLOBAL(0x00F440D4, uint8) == 3)
						continue;

					z = (scenery->z * 8 + originZ) / 8;

					if (mode == 0){
						if (scenery->flags & (1 << 7)){
							//dh
							entry_index |= (1 << 7);
						}

						uint8 bh = ((scenery->flags & 0xF) << rotation);
						bl = bh >> 4;
						bh = (bh | bl) & 0xF;
						bl = (((scenery->flags >> 5) + rotation) & 3) << 5;
						bh |= bl;

						bh |= scenery->flags & 0x90;

						bl = 1;
						if (RCT2_GLOBAL(0x00F440D4, uint8) == 5)bl = 41;
						if (RCT2_GLOBAL(0x00F440D4, uint8) == 4)bl = 105;
						if (RCT2_GLOBAL(0x00F440D4, uint8) == 1)bl = 0;

						RCT2_GLOBAL(0x00141E9AE, rct_string_id) = 927;
						cost = game_do_command(mapCoord.x, bl | (bh << 8), mapCoord.y, z | (entry_index << 8), GAME_COMMAND_18, 0, 0);
					}
					else{
						if (RCT2_GLOBAL(0x00F440D4, uint8) == 1)
							continue;

						rct_map_element* map_element = map_get_path_element_at(mapCoord.x / 32, mapCoord.y / 32, z);

						if (map_element == NULL)
							continue;

						sub_6A7594();
						sub_6A6AA7(mapCoord.x, mapCoord.y, map_element);

						bl = 1;
						if (RCT2_GLOBAL(0x00F440D4, uint8) == 5)bl = 41;
						if (RCT2_GLOBAL(0x00F440D4, uint8) == 4)bl = 105;

						sub_6A6C66(mapCoord.x, mapCoord.y, map_element, bl);
						sub_6A759F();
						continue;
					}
					break;
				default:
					RCT2_GLOBAL(0x00F4414E, uint8) |= 1 << 1;
					continue;
					break;
				}
				RCT2_GLOBAL(0x00F440D5, money32) += cost;
				if (RCT2_GLOBAL(0x00F440D4, uint8) != 2){
					if (cost == MONEY32_UNDEFINED){
						RCT2_GLOBAL(0x00F440D5, money32) = MONEY32_UNDEFINED;
					}
				}

				if (RCT2_GLOBAL(0x00F440D5, money32) != MONEY32_UNDEFINED)
					continue;

				if (RCT2_GLOBAL(0x00F440D4, uint8) == 2)
					continue;

				return 0;
			}
		}
	}
	return 1;
}

int track_place_maze(sint16 x, sint16 y, sint16 z, uint8 rideIndex, uint8** track_elements)
{
	if (RCT2_GLOBAL(0x00F440D4, uint8) == 0){
		gMapSelectionTiles->x = -1;
		RCT2_GLOBAL(0x009DEA48, sint16) = x;
		RCT2_GLOBAL(0x009DEA4A, sint16) = y;

		RCT2_GLOBAL(0x009DEA4C, sint16) = map_element_height(x, y) & 0xFFFF;
		RCT2_GLOBAL(0x009DEA4E, uint8) = RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_ROTATION, uint8);
	}

	RCT2_GLOBAL(0x00F440D5, uint32) = 0;

	rct_maze_element* maze = (rct_maze_element*)(*track_elements);
	for (; maze->all != 0; maze++){
		uint8 rotation = RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_ROTATION, uint8);
		rct_xy16 mapCoord = { .x = maze->x * 32, .y = maze->y * 32 };

		switch (rotation & 3){
		case MAP_ELEMENT_DIRECTION_WEST:
			break;
		case MAP_ELEMENT_DIRECTION_NORTH:{
			int temp_x = -mapCoord.x;
			mapCoord.x = mapCoord.y;
			mapCoord.y = temp_x;
		}
			break;
		case MAP_ELEMENT_DIRECTION_EAST:
			mapCoord.x = -mapCoord.x;
			mapCoord.y = -mapCoord.y;
			break;
		case MAP_ELEMENT_DIRECTION_SOUTH:{
			int temp_y = -mapCoord.y;
			mapCoord.y = mapCoord.x;
			mapCoord.x = temp_y;
		}
			break;
		}

		mapCoord.x += x;
		mapCoord.y += y;

		track_update_max_min_coordinates(mapCoord.x, mapCoord.y, z);

		if (RCT2_GLOBAL(0x00F440D4, uint8) == 0){
			uint8 new_tile = 1;
			rct_xy16* selectionTile = gMapSelectionTiles;
			for (; selectionTile->x != -1; selectionTile++){
				if (selectionTile->x == mapCoord.x && selectionTile->y == mapCoord.y){
					new_tile = 0;
					break;
				}
				if (selectionTile + 1 >= &gMapSelectionTiles[300]){
					new_tile = 0;
					break;
				}
			}
			if (new_tile){
				selectionTile->x = mapCoord.x;
				selectionTile->y = mapCoord.y;
				selectionTile++;
				selectionTile->x = -1;
			}
		}

		if (RCT2_GLOBAL(0x00F440D4, uint8) == 1 ||
			RCT2_GLOBAL(0x00F440D4, uint8) == 2 ||
			RCT2_GLOBAL(0x00F440D4, uint8) == 4 ||
			RCT2_GLOBAL(0x00F440D4, uint8) == 5){

			uint8 bl; 
			money32 cost = 0;
			uint16 maze_entry;

			switch (maze->type){
			case 0x08:
				// entrance
				rotation += maze->unk_2;
				rotation &= 3;

				RCT2_GLOBAL(0x00141E9AE, rct_string_id) = 927;

				bl = 1;
				if (RCT2_GLOBAL(0x00F440D4, uint8) == 4)bl = 0x69;
				if (RCT2_GLOBAL(0x00F440D4, uint8) == 1){
					cost = game_do_command(mapCoord.x, 0 | rotation << 8, mapCoord.y, (z / 16) & 0xFF, GAME_COMMAND_12, -1, 0);
				}
				else{
					cost = game_do_command(mapCoord.x, bl | rotation << 8, mapCoord.y, rideIndex, GAME_COMMAND_12, 0, 0);
				}
				if (cost != MONEY32_UNDEFINED){
					RCT2_GLOBAL(0x00F4414E, uint8) |= (1 << 0);
				}
				break;
			case 0x80:
				// exit
				rotation += maze->unk_2;
				rotation &= 3;

				RCT2_GLOBAL(0x00141E9AE, rct_string_id) = 927;

				bl = 1;
				if (RCT2_GLOBAL(0x00F440D4, uint8) == 4)bl = 0x69;
				if (RCT2_GLOBAL(0x00F440D4, uint8) == 1){
					cost = game_do_command(mapCoord.x, 0 | rotation << 8, mapCoord.y, ((z / 16) & 0xFF) | (1 << 8), GAME_COMMAND_12, -1, 0);
				}
				else{
					cost = game_do_command(mapCoord.x, bl | rotation << 8, mapCoord.y, rideIndex | (1 << 8), GAME_COMMAND_12, 0, 0);
				}
				if (cost != MONEY32_UNDEFINED){
					RCT2_GLOBAL(0x00F4414E, uint8) |= (1 << 0);
				}
				break;
			default:
				maze_entry = rol16(maze->maze_entry, rotation * 4);

				bl = 1;
				if (RCT2_GLOBAL(0x00F440D4, uint8) == 5)bl = 0x29;
				if (RCT2_GLOBAL(0x00F440D4, uint8) == 4)bl = 0x69;
				if (RCT2_GLOBAL(0x00F440D4, uint8) == 1)bl = 0;

				RCT2_GLOBAL(0x00141E9AE, rct_string_id) = 927;

				cost = game_do_command(mapCoord.x, bl | (maze_entry & 0xFF) << 8, mapCoord.y, rideIndex | (maze_entry & 0xFF00), GAME_COMMAND_49, z, 0);
				break;
			}

			RCT2_GLOBAL(0x00F440D5, money32) += cost;

			if (cost == MONEY32_UNDEFINED){
				RCT2_GLOBAL(0x00F440D5, money32) = cost;
				return 0;
			}
		}

		if (RCT2_GLOBAL(0x00F440D4, uint8) == 3){
			if (mapCoord.x > 0x1FFF)
				continue;
			if (mapCoord.y > 0x1FFF)
				continue;
			if (mapCoord.x < 0)
				continue;
			if (mapCoord.y < 0)
				continue;

			rct_map_element* map_element = map_get_surface_element_at(mapCoord.x / 32, mapCoord.y / 32);

			sint16 map_height = map_element->base_height * 8;

			if (map_element->properties.surface.slope & 0xF){
				map_height += 16;
				if (map_element->properties.surface.slope & 0x10){
					map_height += 16;
				}
			}

			if (map_element->properties.surface.terrain & MAP_ELEMENT_WATER_HEIGHT_MASK){
				sint16 water_height = map_element->properties.surface.terrain & MAP_ELEMENT_WATER_HEIGHT_MASK;
				water_height *= 16;
				if (water_height > map_height)
					map_height = water_height;
			}

			sint16 temp_z = z + RCT2_GLOBAL(0x00F440D5, sint16) - map_height;
			if (temp_z < 0)
				RCT2_GLOBAL(0x00F440D5, sint16) -= temp_z;
		}
	}

	if (RCT2_GLOBAL(0x00F440D4, uint8) == 6){
		game_do_command(0, 0x69, 0, rideIndex, GAME_COMMAND_7, 0, 0);
	}

	RCT2_GLOBAL(0x00F44142, sint16) = x;
	RCT2_GLOBAL(0x00F44144, sint16) = y;
	RCT2_GLOBAL(0x00F44146, sint16) = z;

	*track_elements = (uint8*)maze + 4;
	return 1;
}

int track_place_ride(sint16 x, sint16 y, sint16 z, uint8 rideIndex, uint8** track_elements)
{
	RCT2_GLOBAL(0x00F44142, sint16) = x;
	RCT2_GLOBAL(0x00F44144, sint16) = y;
	RCT2_GLOBAL(0x00F44146, sint16) = z;

	if (RCT2_GLOBAL(0x00F440D4, uint8) == 0){
		gMapSelectionTiles->x = -1;
		RCT2_GLOBAL(0x009DEA48, sint16) = x;
		RCT2_GLOBAL(0x009DEA4A, sint16) = y;

		RCT2_GLOBAL(0x009DEA4C, sint16) = map_element_height(x, y) & 0xFFFF;
		RCT2_GLOBAL(0x009DEA4E, uint8) = RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_ROTATION, uint8);
	}

	RCT2_GLOBAL(0x00F440D5, uint32) = 0;
	uint8 rotation = RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_ROTATION, uint8);

	rct_track_element* track = (rct_track_element*)(*track_elements);
	for (; track->type != 0xFF; track++){

		uint8 track_type = track->type;
		if (track_type == TRACK_ELEM_INVERTED_90_DEG_UP_TO_FLAT_QUARTER_LOOP)
			track_type = 0xFF;

		track_update_max_min_coordinates(x, y, z);

		if (RCT2_GLOBAL(0x00F440D4, uint8) == 0){
			for (rct_preview_track* trackBlock = RCT2_ADDRESS(0x00994638, rct_preview_track*)[track_type];
				trackBlock->var_00 != 0xFF;
				trackBlock++){
				rct_xy16 tile;
				tile.x = x;
				tile.y = y;

				switch (rotation & 3){
				case MAP_ELEMENT_DIRECTION_WEST:
					tile.x += trackBlock->x;
					tile.y += trackBlock->y;
					break;
				case MAP_ELEMENT_DIRECTION_NORTH:
					tile.x += trackBlock->y;
					tile.y -= trackBlock->x;
					break;
				case MAP_ELEMENT_DIRECTION_EAST:
					tile.x -= trackBlock->x;
					tile.y -= trackBlock->y;
					break;
				case MAP_ELEMENT_DIRECTION_SOUTH:
					tile.x -= trackBlock->y;
					tile.y += trackBlock->x;
					break;
				}

				track_update_max_min_coordinates(tile.x, tile.y, z);

				uint8 new_tile = 1;
				rct_xy16* selectionTile = gMapSelectionTiles;
				for (; selectionTile->x != -1; selectionTile++){
					if (selectionTile->x == tile.x && selectionTile->y == tile.y){
						new_tile = 0;
						break;
					}
					if (selectionTile + 1 >= &gMapSelectionTiles[300]){
						new_tile = 0;
						break;
					}
				}
				if (new_tile){
					selectionTile->x = tile.x;
					selectionTile->y = tile.y;
					selectionTile++;
					selectionTile->x = -1;
				}
			}
		}

		if (RCT2_GLOBAL(0x00F440D4, uint8) == 6){
			const rct_track_coordinates* track_coordinates = &TrackCoordinates[track_type];

			//di
			int temp_z = z;
			temp_z -= track_coordinates->z_negative;
			rct_preview_track* trackBlock = RCT2_ADDRESS(0x00994638, rct_preview_track*)[track_type];

			temp_z += trackBlock->z;
			// rotation in bh
			// track_type in dl
			game_do_command(x, 0x69 | ((rotation & 3) << 8), y, track_type, GAME_COMMAND_4, temp_z, 0);
		}

		if (RCT2_GLOBAL(0x00F440D4, uint8) == 1 ||
			RCT2_GLOBAL(0x00F440D4, uint8) == 2 ||
			RCT2_GLOBAL(0x00F440D4, uint8) == 4 ||
			RCT2_GLOBAL(0x00F440D4, uint8) == 5){
			const rct_track_coordinates* track_coordinates = &TrackCoordinates[track_type];

			//di
			int temp_z = z;
			temp_z -= track_coordinates->z_negative;
			uint32 edi = ((track->flags & 0xF) << 17) |
				((track->flags & 0xF) << 28) |
				(((track->flags >> 4) & 0x3) << 24) |
				(temp_z & 0xFFFF);

			int edx = RCT2_GLOBAL(0x00F440A7, uint8) | (track_type << 8);

			if (track->flags & 0x80)edx |= 0x10000;
			if (track->flags & 0x40)edx |= 0x20000;

			uint8 bl = 1;
			if (RCT2_GLOBAL(0x00F440D4, uint8) == 5)bl = 41;
			if (RCT2_GLOBAL(0x00F440D4, uint8) == 4)bl = 105;
			if (RCT2_GLOBAL(0x00F440D4, uint8) == 1)bl = 0;

			RCT2_GLOBAL(0x00141E9AE, rct_string_id) = 927;
			money32 cost = game_do_command(x, bl | (rotation << 8), y, edx, GAME_COMMAND_3, edi, 0);
			RCT2_GLOBAL(0x00F440D5, money32) += cost;

			if (cost == MONEY32_UNDEFINED){
				RCT2_GLOBAL(0x00F440D5, money32) = cost;
				return 0;
			}
		}

		if (RCT2_GLOBAL(0x00F440D4, uint8) == 3){
			for (rct_preview_track* trackBlock = RCT2_ADDRESS(0x00994638, rct_preview_track*)[track_type];
				trackBlock->var_00 != 0xFF;
				trackBlock++){
				rct_xy16 tile;
				tile.x = x;
				tile.y = y;

				switch (rotation & 3){
				case MAP_ELEMENT_DIRECTION_WEST:
					tile.x += trackBlock->x;
					tile.y += trackBlock->y;
					break;
				case MAP_ELEMENT_DIRECTION_NORTH:
					tile.x += trackBlock->y;
					tile.y -= trackBlock->x;
					break;
				case MAP_ELEMENT_DIRECTION_EAST:
					tile.x -= trackBlock->x;
					tile.y -= trackBlock->y;
					break;
				case MAP_ELEMENT_DIRECTION_SOUTH:
					tile.x -= trackBlock->y;
					tile.y += trackBlock->x;
					break;
				}

				if (tile.x > 0x1FFF)
					continue;

				if (tile.y > 0x1FFF)
					continue;

				if (tile.x < 0)
					continue;

				if (tile.y < 0)
					continue;

				rct_map_element* map_element = map_get_surface_element_at(tile.x / 32, tile.y / 32);

				if (map_element == NULL)
					return 0;

				int height = map_element->base_height * 8;
				if (map_element->properties.surface.slope & 0xF){
					height += 16;
					if (map_element->properties.surface.slope & 0x10){
						height += 16;
					}
				}

				uint8 water_height = 16 * map_element->properties.surface.terrain & MAP_ELEMENT_WATER_HEIGHT_MASK;
				if (water_height){
					if (water_height > height){
						height = water_height;
					}
				}
				int temp_z = z + RCT2_GLOBAL(0x00F440D5, sint16);
				temp_z -= height;

				if (temp_z < 0){
					RCT2_GLOBAL(0x00F440D5, sint16) -= temp_z;
				}
			}
		}

		const rct_track_coordinates* track_coordinates = &TrackCoordinates[track_type];
		rotation &= 3;
		switch (rotation & 3){
		case 0:
			x += track_coordinates->x;
			y += track_coordinates->y;
			break;
		case 1:
			x += track_coordinates->y;
			y -= track_coordinates->x;
			break;
		case 2:
			x -= track_coordinates->x;
			y -= track_coordinates->y;
			break;
		case 3:
			x -= track_coordinates->y;
			y += track_coordinates->x;
			break;
		}

		z -= track_coordinates->z_negative;
		z += track_coordinates->z_positive;

		rotation += track_coordinates->rotation_positive - track_coordinates->rotation_negative;
		rotation &= 3;
		if (track_coordinates->rotation_positive & (1 << 2))
			rotation |= (1 << 2);

		if (!(rotation & (1 << 2))){
			x += RCT2_ADDRESS(0x00993CCC, sint16)[rotation * 2];
			y += RCT2_ADDRESS(0x00993CCE, sint16)[rotation * 2];
		}
	}

	// Entrance elements
	//0x6D06D8
	*track_elements = (uint8*)track + 1;
	rct_track_entrance* entrance = (rct_track_entrance*)(*track_elements);
	for (; entrance->z != -1; entrance++){
		rotation = RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_ROTATION, uint8);
		x = entrance->x;
		y = entrance->y;

		switch (rotation & 3){
		case MAP_ELEMENT_DIRECTION_WEST:
			break;
		case MAP_ELEMENT_DIRECTION_NORTH:{
			int temp_x = -x;
			x = y;
			y = temp_x;
		}
			break;
		case MAP_ELEMENT_DIRECTION_EAST:
			x = -x;
			y = -y;
			break;
		case MAP_ELEMENT_DIRECTION_SOUTH:{
			int temp_y = -y;
			y = x;
			x = temp_y;
		}
			break;
		}

		x += RCT2_GLOBAL(0x00F44142, sint16);
		y += RCT2_GLOBAL(0x00F44144, sint16);


		track_update_max_min_coordinates(x, y, z);

		if (RCT2_GLOBAL(0x00F440D4, uint8) == 0){
			uint8 new_tile = 1;
			rct_xy16* selectionTile = gMapSelectionTiles;
			for (; selectionTile->x != -1; selectionTile++){
				if (selectionTile->x == x && selectionTile->y == y){
					new_tile = 0;
					break;
				}
				if (selectionTile + 1 >= &gMapSelectionTiles[300]){
					new_tile = 0;
					break;
				}
			}
			if (new_tile){
				selectionTile->x = x;
				selectionTile->y = y;
				selectionTile++;
				selectionTile->x = -1;
			}
		}

		if (RCT2_GLOBAL(0x00F440D4, uint8) == 1 ||
			RCT2_GLOBAL(0x00F440D4, uint8) == 2 ||
			RCT2_GLOBAL(0x00F440D4, uint8) == 4 ||
			RCT2_GLOBAL(0x00F440D4, uint8) == 5){

			//bh
			rotation += entrance->direction;
			rotation &= 3;

			//dh rideIndex is dl
			uint8 is_exit = 0;
			if (entrance->direction & (1 << 7)){
				is_exit = 1;
			}

			if (RCT2_GLOBAL(0x00F440D4, uint8) != 1){
				rct_xy16 tile;
				tile.x = x + RCT2_ADDRESS(0x00993CCC, sint16)[rotation * 2];
				tile.y = y + RCT2_ADDRESS(0x00993CCE, sint16)[rotation * 2];

				rct_map_element* map_element = map_get_first_element_at(tile.x / 32, tile.y / 32);
				z = RCT2_GLOBAL(0x00F44146, sint16) / 8;

				z += (entrance->z == (sint8)0x80) ? -1 : entrance->z;

				do{
					if (map_element_get_type(map_element) != MAP_ELEMENT_TYPE_TRACK)
						continue;
					if (map_element->base_height != z)
						continue;

					int di = (map_element->properties.track.sequence >> 4) & 0x7;
					uint8 bl = 1;
					if (RCT2_GLOBAL(0x00F440D4, uint8) == 5)bl = 41;
					if (RCT2_GLOBAL(0x00F440D4, uint8) == 4)bl = 105;
					if (RCT2_GLOBAL(0x00F440D4, uint8) == 1)bl = 0;

					RCT2_GLOBAL(0x00141E9AE, rct_string_id) = 927;
					money32 cost = game_do_command(x, bl | (rotation << 8), y, rideIndex | (is_exit << 8), GAME_COMMAND_12, di, 0);
					RCT2_GLOBAL(0x00F440D5, money32) += cost;

					if (cost == MONEY32_UNDEFINED){
						RCT2_GLOBAL(0x00F440D5, money32) = cost;
						return 0;
					}
					RCT2_GLOBAL(0x00F4414E, uint8) |= (1 << 0);
					break;
				} while (!map_element_is_last_for_tile(map_element++));
			}
			else{
				//dl
				z = (entrance->z == (sint8)0x80) ? -1 : entrance->z;
				z *= 8;
				z += RCT2_GLOBAL(0x00F44146, sint16);
				z /= 16;

				RCT2_GLOBAL(0x00141E9AE, rct_string_id) = 927;
				money32 cost = game_do_command(x, 0 | (rotation << 8), y, z | (is_exit << 8), GAME_COMMAND_12, -1, 0);
				RCT2_GLOBAL(0x00F440D5, money32) += cost;

				if (cost == MONEY32_UNDEFINED){
					RCT2_GLOBAL(0x00F440D5, money32) = cost;
					return 0;
				}
				RCT2_GLOBAL(0x00F4414E, uint8) |= (1 << 0);
			}
		}
	}

	if (RCT2_GLOBAL(0x00F440D4, uint8) == 6){
		RCT2_CALLPROC_X(0x006CB945, 0, 0, 0, RCT2_GLOBAL(0x00F440A7, uint8), 0, 0, 0);
		rct_ride* ride = GET_RIDE(RCT2_GLOBAL(0x00F440A7, uint8));
		user_string_free(ride->name);
		ride->type = RIDE_TYPE_NULL;
	}

	*track_elements = ((uint8*)(entrance)) + 1;

	return 1;
}

/**
* Places a virtual track. This can involve highlighting the surface tiles and showing the track layout. It is also used by 
* the track preview window to place the whole track. 
* Depending on the value of bl it modifies the function.
* bl == 0, Draw outlines on the ground
* bl == 1,
* bl == 2,
* bl == 3, Returns the z value of a succesful placement. Only lower 16 bits are the value, the rest may be garbage?
* bl == 4,
* bl == 5, Returns cost to create the track. All 32 bits are used. Places the track. (used by the preview)
* bl == 6, Clear white outlined track.
*  rct2: 0x006D01B3
*/
int sub_6D01B3(uint8 bl, uint8 rideIndex, int x, int y, int z)
{
	RCT2_GLOBAL(0x00F4414E, uint8) = bl & 0x80;
	RCT2_GLOBAL(0x00F440D4, uint8) = bl & 0x7F;
	if (RCT2_GLOBAL(RCT2_ADDRESS_TRACK_DESIGN_SCENERY_TOGGLE, uint8) != 0){
		RCT2_GLOBAL(0x00F4414E, uint8) |= 0x80;
	}
	RCT2_GLOBAL(0x00F440A7, uint8) = rideIndex;

	RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_X_MIN, sint16) = x;
	RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_X_MAX, sint16) = x;
	RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_Y_MIN, sint16) = y;
	RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_Y_MAX, sint16) = y;
	RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_Z_MIN, sint16) = z;
	RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_Z_MAX, sint16) = z;

	RCT2_GLOBAL(0x00F44129, uint16) = 0;
	uint8* track_elements = RCT2_ADDRESS(0x009D821B, uint8);

	rct_track_td6* track_design = RCT2_ADDRESS(0x009D8178, rct_track_td6);
	uint8 track_place_success = 0;

	if (track_design->type == RIDE_TYPE_MAZE){
		track_place_success = track_place_maze(x, y, z, rideIndex, &track_elements);
	}
	else{
		track_place_success = track_place_ride(x, y, z, rideIndex, &track_elements);
	}
	// Scenery elements
	rct_track_scenery* scenery = (rct_track_scenery*)track_elements;

	if (track_place_success){
		if (!track_place_scenery(
			scenery,
			rideIndex,
			RCT2_GLOBAL(0x00F44142, sint16),
			RCT2_GLOBAL(0x00F44144, sint16),
			RCT2_GLOBAL(0x00F44146, sint16))){
			return RCT2_GLOBAL(0x00F440D5, money32);
		}
	}

	//0x6D0FE6
	if (RCT2_GLOBAL(0x00F440D4, uint8) == 0){
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) |= 0x6;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~(1 << 3);
		map_invalidate_map_selection_tiles();
	}

	if (bl == 3)
		return RCT2_GLOBAL(0x00F440D5, sint16);
	return RCT2_GLOBAL(0x00F440D5, money32);

	int eax, ebx, ecx, edx, esi, edi, ebp;
	eax = x;
	ebx = bl;
	ecx = y;
	edx = z;
	esi = 0;
	edi = 0;
	ebp = 0;
	RCT2_CALLFUNC_X(0x006D01B3, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	if (bl == 3)
		ebx &= 0xFFFF;
	return ebx;
}

/* rct2: 0x006D2189 
 * ebx = ride_id
 * cost = edi
 */
int sub_6D2189(int* cost, uint8* ride_id){
	RCT2_GLOBAL(0xF44151, uint8) = 0;
	rct_track_td6* track_design = RCT2_ADDRESS(0x009D8178, rct_track_td6);
	uint8 entry_type, entry_index;
	
	if (!find_object_in_entry_group(&track_design->vehicle_object, &entry_type, &entry_index))
		entry_index = 0xFF;

	int eax = 0, ebx, ecx = 0, edx, esi, edi = 0, ebp = 0;
	ebx = 41;
	edx = track_design->type | (entry_index << 8);
	esi = GAME_COMMAND_6;

	if (MONEY32_UNDEFINED == game_do_command_p(GAME_COMMAND_6, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp)) return 1;

	// bh
	*ride_id = edi & 0xFF;

	rct_ride* ride = GET_RIDE(*ride_id);

	uint8* ride_name = RCT2_ADDRESS(0x9E3504, uint8);
	rct_string_id new_ride_name = user_string_allocate(132, ride_name);

	if (new_ride_name){
		rct_string_id old_name = ride->name;
		ride->name = new_ride_name;
		user_string_free(old_name);
	}

	uint8 version = track_design->version_and_colour_scheme >> 2;

	if (version == 2){
		ride->entrance_style = track_design->entrance_style;
	}

	if (version != 0){
		memcpy(&ride->track_colour_main, &track_design->track_spine_colour, 4);
		memcpy(&ride->track_colour_additional, &track_design->track_rail_colour, 4);
		memcpy(&ride->track_colour_supports, &track_design->track_support_colour, 4);
	}
	else{
		memset(&ride->track_colour_main, track_design->track_spine_colour_rct1, 4);
		memset(&ride->track_colour_additional, track_design->track_rail_colour_rct1, 4);
		memset(&ride->track_colour_supports, track_design->track_support_colour_rct1, 4);
	}

	RCT2_GLOBAL(0x009D8150, uint8) |= 1;
	uint8 backup_rotation = RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_ROTATION, uint8);
	uint32 backup_park_flags = RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32);
	RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) &= ~PARK_FLAGS_FORBID_HIGH_CONSTRUCTION;
	int map_size = RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE, uint16) << 4;

	RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_ROTATION, uint8) = 0;
	int z = sub_6D01B3(3, 0, map_size, map_size, 16);

	if (RCT2_GLOBAL(0xF4414E, uint8) & 4){
		RCT2_GLOBAL(0xF44151, uint8) |= 2;
	}
	//dx
	z += 16 - RCT2_GLOBAL(0xF44129, uint16);

	int bl = 5;
	if (RCT2_GLOBAL(0xF4414E, uint8) & 2){
		bl |= 0x80;
		RCT2_GLOBAL(0xF44151, uint8) |= 1;
	}
	edi = sub_6D01B3(bl, *ride_id, map_size, map_size, z);
	RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) = backup_park_flags;

	if (edi != MONEY32_UNDEFINED){

		if (!find_object_in_entry_group(&track_design->vehicle_object, &entry_type, &entry_index)){
			RCT2_GLOBAL(0xF44151, uint8) |= 4;
		}

		RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_ROTATION, uint8) = backup_rotation;
		RCT2_GLOBAL(0x009D8150, uint8) &= ~1;
		*cost = edi;
		return 1;
	}
	else{

		RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_ROTATION, uint8) = backup_rotation;
		user_string_free(ride->name);
		ride->type = RIDE_TYPE_NULL;
		RCT2_GLOBAL(0x009D8150, uint8) &= ~1;
		return 0;
	}
}

/* rct2: 0x006D235B */
void sub_6D235B(uint8 ride_id){
	rct_ride* ride = GET_RIDE(ride_id);
	user_string_free(ride->name);
	ride->type = RIDE_TYPE_NULL;
}

/* rct2: 0x006D1EF0 */
void draw_track_preview(uint8** preview){
	// Make a copy of the map
	if (!backup_map())return;

	blank_map();

	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_MANAGER){
		load_track_scenery_objects();
	}

	int cost;
	uint8 ride_id;

	if (!sub_6D2189(&cost, &ride_id)){
		memset(preview, 0, TRACK_PREVIEW_IMAGE_SIZE * 4);
		reload_map_backup();
		return;
	}

	RCT2_GLOBAL(RCT2_ADDRESS_TRACK_DESIGN_COST, money32) = cost;

	rct_viewport* view = RCT2_ADDRESS(0x9D8161, rct_viewport);
	rct_drawpixelinfo* dpi = RCT2_ADDRESS(0x9D8151, rct_drawpixelinfo);
	int left, top, right, bottom;

	int center_x = (RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_X_MAX, sint16) + RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_X_MIN, sint16)) / 2 + 16;
	int center_y = (RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_Y_MAX, sint16) + RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_Y_MIN, sint16)) / 2 + 16;
	int center_z = (RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_Z_MIN, sint16) + RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_Z_MAX, sint16)) / 2;

	int width = RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_X_MAX, sint16) - RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_X_MIN, sint16);
	int height = RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_Y_MAX, sint16) - RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_Y_MIN, sint16);

	if (width < height)
		width = height;

	int zoom_level = 1;
	
	if (width > 1120)
		zoom_level = 2;

	if (width > 2240)
		zoom_level = 3;

	width = 370 << zoom_level;
	height = 217 << zoom_level;

	int x = center_y - center_x - width / 2;
	int y = (center_y + center_x) / 2 - center_z - height / 2;

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32) = 0;

	view->width = 370;
	view->height = 217;
	view->view_width = width;
	view->view_height = height;
	view->x = 0;
	view->y = 0;
	view->view_x = x;
	view->view_y = y;
	view->zoom = zoom_level;
	view->flags = VIEWPORT_FLAG_HIDE_BASE | VIEWPORT_FLAG_INVISIBLE_SPRITES;

	dpi->zoom_level = zoom_level;
	dpi->x = 0;
	dpi->y = 0;
	dpi->width = 370;
	dpi->height = 217;
	dpi->pitch = 0;
	dpi->bits = (char*)preview;

	top = y;
	left = x;
	bottom = y + height;
	right = x + width;

	viewport_paint(view, dpi, left, top, right, bottom);

	dpi->bits += TRACK_PREVIEW_IMAGE_SIZE;

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32) = 1;
	x = -center_y - center_x - width / 2;
	y = (center_y - center_x) / 2 - center_z - height / 2;

	view->view_x = x;
	view->view_y = y;
	top = y;
	left = x;
	bottom = y + height;
	right = x + width;

	viewport_paint(view, dpi, left, top, right, bottom);

	dpi->bits += TRACK_PREVIEW_IMAGE_SIZE;

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32) = 2;
	x =  center_x - center_y - width / 2;
	y = (-center_y - center_x) / 2 - center_z - height / 2;

	view->view_x = x;
	view->view_y = y;
	top = y;
	left = x;
	bottom = y + height;
	right = x + width;

	viewport_paint(view, dpi, left, top, right, bottom);

	dpi->bits += TRACK_PREVIEW_IMAGE_SIZE;

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32) = 3;
	x = center_x + center_y - width / 2;
	y = (center_x - center_y) / 2 - center_z - height / 2;

	view->view_x = x;
	view->view_y = y;
	top = y;
	left = x;
	bottom = y + height;
	right = x + width;

	viewport_paint(view, dpi, left, top, right, bottom);

	sub_6D235B(ride_id);
	reload_map_backup();
}

/**
 * 
 * I don't think preview is a necessary output argument. It can be obtained easily using the track design structure.
 *  rct2: 0x006D1DEC
 */
rct_track_design *track_get_info(int index, uint8** preview)
{
	rct_track_design *trackDesign;
	uint8 *trackDesignList = RCT2_ADDRESS(RCT2_ADDRESS_TRACK_LIST, uint8);
	int i;

	trackDesign = NULL;

	// Check if track design has already been loaded
	for (i = 0; i < 4; i++) {
		if (index == RCT2_ADDRESS(RCT2_ADDRESS_TRACK_DESIGN_INDEX_CACHE, uint32)[i]) {
			trackDesign = &RCT2_GLOBAL(RCT2_ADDRESS_TRACK_DESIGN_CACHE, rct_track_design*)[i];
			break;
		}
	}

	if (trackDesign == NULL) {
		// Load track design
		i = RCT2_GLOBAL(RCT2_ADDRESS_TRACK_DESIGN_NEXT_INDEX_CACHE, uint32)++;
		if (RCT2_GLOBAL(RCT2_ADDRESS_TRACK_DESIGN_NEXT_INDEX_CACHE, uint32) >= 4)
			RCT2_GLOBAL(RCT2_ADDRESS_TRACK_DESIGN_NEXT_INDEX_CACHE, uint32) = 0;

		RCT2_ADDRESS(RCT2_ADDRESS_TRACK_DESIGN_INDEX_CACHE, uint32)[i] = index;

		char track_path[MAX_PATH] = { 0 };
		subsitute_path(track_path, (char*)RCT2_ADDRESS_TRACKS_PATH, trackDesignList + (index * 128));

		rct_track_td6* loaded_track = NULL;

		log_verbose("Loading track: %s", trackDesignList + (index * 128));

		if (!(loaded_track = load_track_design(track_path))) {
			if (preview != NULL) *preview = NULL;
			// Mark cache as empty.
			RCT2_ADDRESS(RCT2_ADDRESS_TRACK_DESIGN_INDEX_CACHE, uint32)[i] = 0;
			log_error("Failed to load track: %s", trackDesignList + (index * 128));
			return NULL;
		}

		trackDesign = &RCT2_GLOBAL(RCT2_ADDRESS_TRACK_DESIGN_CACHE, rct_track_design*)[i];

		// Copy the track design apart from the preview image
		memcpy(&trackDesign->track_td6, loaded_track, sizeof(rct_track_td6));
		// Load in a new preview image, calculate cost variable, calculate var_06
		draw_track_preview((uint8**)trackDesign->preview);
		//RCT2_CALLPROC_X(0x006D1EF0, 0, 0, 0, 0, 0, (int)&trackDesign->preview, 0);

		trackDesign->track_td6.cost = RCT2_GLOBAL(RCT2_ADDRESS_TRACK_DESIGN_COST, money32);
		trackDesign->track_td6.track_flags = RCT2_GLOBAL(0x00F44151, uint8) & 7;
	}

	// Set preview to correct preview image based on rotation
	if (preview != NULL)
		*preview = trackDesign->preview[RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_ROTATION, uint8)];

	return trackDesign;
}

/**
 * 
 *  rct2: 0x006D3664
 */
int track_rename(const char *text)
{
	const char* txt_chr = text;

	while (*txt_chr != '\0'){
		switch (*txt_chr){
		case '.':
		case '/':
		case '\\':
		case '*':
		case '?':
			// Invalid characters
			RCT2_GLOBAL(0x141E9AC, uint16) = 3353;
			return 0;
		}
		txt_chr++;
	}

	char new_path[MAX_PATH];
	subsitute_path(new_path, RCT2_ADDRESS(RCT2_ADDRESS_TRACKS_PATH, char), text);
	strcat(new_path, ".TD6");

	rct_window* w = window_find_by_class(WC_TRACK_DESIGN_LIST);

	char old_path[MAX_PATH];
	subsitute_path(old_path, RCT2_ADDRESS(RCT2_ADDRESS_TRACKS_PATH, char), &RCT2_ADDRESS(RCT2_ADDRESS_TRACK_LIST, char)[128 * w->track_list.var_482]);

	if (!platform_file_move(old_path, new_path)) {
		RCT2_GLOBAL(0x141E9AC, uint16) = 3354;
		return 0;
	}

	ride_list_item item = { 0xFC, 0 };
	track_load_list(item);

	item.type = RCT2_GLOBAL(0xF44158, uint8);
	item.entry_index = RCT2_GLOBAL(0xF44159, uint8);
	track_load_list(item);

	reset_track_list_cache();
	
	window_invalidate(w);
	return 1;
}

/**
 * 
 *  rct2: 0x006D3761
 */
int track_delete()
{
	rct_window* w = window_find_by_class(WC_TRACK_DESIGN_LIST);

	char path[MAX_PATH];
	subsitute_path(path, RCT2_ADDRESS(RCT2_ADDRESS_TRACKS_PATH, char), &RCT2_ADDRESS(RCT2_ADDRESS_TRACK_LIST, char)[128 * w->track_list.var_482]);

	if (!platform_file_delete(path)) {
		RCT2_GLOBAL(0x141E9AC, uint16) = 3355;
		return 0;
	}

	ride_list_item item = { 0xFC, 0 };
	track_load_list(item);

	item.type = RCT2_GLOBAL(0xF44158, uint8);
	item.entry_index = RCT2_GLOBAL(0xF44159, uint8);
	track_load_list(item);

	reset_track_list_cache();

	window_invalidate(w);
	return 1;
}

/**
 * Helper method to determine if a connects to b by its bank and angle, not location.
 */
int track_is_connected_by_shape(rct_map_element *a, rct_map_element *b)
{
	int trackType, aBank, aAngle, bBank, bAngle;
	rct_ride *ride;

	ride = GET_RIDE(a->properties.track.ride_index);
	trackType = a->properties.track.type;
	aBank = gTrackDefinitions[trackType].bank_end;
	aAngle = gTrackDefinitions[trackType].vangle_end;
	if (RCT2_GLOBAL(0x0097D4F2 + (ride->type * 8), uint16) & 8) {
		if (a->properties.track.colour & 4) {
			if (aBank == TRACK_BANK_NONE)
				aBank = TRACK_BANK_UPSIDE_DOWN;
			else if (aBank == TRACK_BANK_UPSIDE_DOWN)
				aBank = TRACK_BANK_NONE;
		}
	}

	ride = GET_RIDE(b->properties.track.ride_index);
	trackType = b->properties.track.type;
	bBank = gTrackDefinitions[trackType].bank_start;
	bAngle = gTrackDefinitions[trackType].vangle_start;
	if (RCT2_GLOBAL(0x0097D4F2 + (ride->type * 8), uint16) & 8) {
		if (b->properties.track.colour & 4) {
			if (bBank == TRACK_BANK_NONE)
				bBank = TRACK_BANK_UPSIDE_DOWN;
			else if (bBank == TRACK_BANK_UPSIDE_DOWN)
				bBank = TRACK_BANK_NONE;
		}
	}

	return aBank == bBank && aAngle == bAngle;
}

/* Based on rct2: 0x006D2897 */
int copy_scenery_to_track(uint8** track_pointer){
	rct_track_scenery* track_scenery = (rct_track_scenery*)(*track_pointer - 1);
	rct_track_scenery* scenery_source = RCT2_ADDRESS(0x009DA193, rct_track_scenery);

	while (1){
		int ebx = 0;
		memcpy(track_scenery, scenery_source, sizeof(rct_track_scenery));
		if ((track_scenery->scenery_object.flags & 0xFF) == 0xFF) break;

		//0x00F4414D is direction of track?
		if ((track_scenery->scenery_object.flags & 0xF) == OBJECT_TYPE_PATHS){

			uint8 slope = (track_scenery->flags & 0x60) >> 5;
			slope -= RCT2_GLOBAL(0x00F4414D, uint8);

			track_scenery->flags &= 0x9F;
			track_scenery->flags |= ((slope & 3) << 5);

			// Direction of connection on path
			uint8 direction = track_scenery->flags & 0xF;
			// Rotate the direction by the track direction
			direction = ((direction << 4) >> RCT2_GLOBAL(0x00F4414D, uint8));

			track_scenery->flags &= 0xF0;
			track_scenery->flags |= (direction & 0xF) | (direction >> 4);

		}
		else if ((track_scenery->scenery_object.flags & 0xF) == OBJECT_TYPE_WALLS){
			uint8 direction = track_scenery->flags & 3;

			direction -= RCT2_GLOBAL(0x00F4414D, uint8);

			track_scenery->flags &= 0xFC;
			track_scenery->flags |= (direction & 3);
		}
		else {
			uint8 direction = track_scenery->flags & 3;
			uint8 quadrant = (track_scenery->flags & 0xC) >> 2;

			direction -= RCT2_GLOBAL(0x00F4414D, uint8);
			quadrant -= RCT2_GLOBAL(0x00F4414D, uint8);

			track_scenery->flags &= 0xF0;
			track_scenery->flags |= (direction & 3) | ((quadrant & 3) << 2);
		}
		int x = ((uint8)track_scenery->x) * 32 - RCT2_GLOBAL(0x00F44142, sint16);
		int y = ((uint8)track_scenery->y) * 32 - RCT2_GLOBAL(0x00F44144, sint16);

		switch (RCT2_GLOBAL(0x00F4414D, uint8)){
		case 0:
			break;
		case 1:
		{
			int temp_y = y;
			y = x;
			x = -temp_y;
		}
			break;
		case 2:
			x = -x;
			y = -y;
			break;
		case 3:
		{
			int temp_x = x;
			x = y;
			y = -temp_x;
		}
			break;
		}

		x /= 32;
		y /= 32;

		if (x > 127 || y > 127 || x < -126 || y < -126){
			window_error_open(3346, 3347);
			return 0;
		}

		track_scenery->x = x;
		track_scenery->y = y;

		int z = track_scenery->z * 8 - RCT2_GLOBAL(0xF44146, sint16);

		z /= 8;

		if (z > 127 || z < -126){
			window_error_open(3346, 3347);
			return 0;
		}

		track_scenery->z = z;

		track_scenery++;
		scenery_source++;
	}

	*track_pointer = (uint8*)track_scenery;
	//Skip end of scenery elements byte
	(*track_pointer)++;
	return 1;
}

/* rct2: 0x006CEAAE */
int maze_ride_to_td6(uint8 rideIndex, rct_track_td6* track_design, uint8* track_elements){
	rct_map_element* map_element;
	uint8 map_found = 0;

	sint16 start_x, start_y;

	for (start_y = 0; start_y < 8192; start_y += 32){
		for (start_x = 0; start_x < 8192; start_x += 32){
			map_element = map_get_first_element_at(start_x / 32, start_y / 32);

			do{
				if (map_element_get_type(map_element) != MAP_ELEMENT_TYPE_TRACK)
					continue;
				if (map_element->properties.track.ride_index == rideIndex){
					map_found = 1;
					break;
				}
			} while (!map_element_is_last_for_tile(map_element++));

			if (map_found)
				break;
		}
		if (map_found)
			break;
	}

	if (map_found == 0){
		RCT2_GLOBAL(0x00141E9AC, uint16) = 3347;
		return 0;
	}

	RCT2_GLOBAL(0x00F44142, sint16) = start_x;
	RCT2_GLOBAL(0x00F44144, sint16) = start_y;
	RCT2_GLOBAL(0x00F44146, sint16) = map_element->base_height * 8;

	rct_maze_element* maze = (rct_maze_element*)track_elements;

	// x is defined here as we can start the search
	// on tile start_x, start_y but then the next row
	// must restart on 0
	for (sint16 y = start_y, x = start_x; y < 8192; y += 32){
		for (; x < 8192; x += 32){
			map_element = map_get_first_element_at(x / 32, y / 32);

			do{
				if (map_element_get_type(map_element) != MAP_ELEMENT_TYPE_TRACK)
					continue;
				if (map_element->properties.track.ride_index != rideIndex)
					continue;

				maze->maze_entry = map_element->properties.track.maze_entry;
				maze->x = (x - start_x) / 32;
				maze->y = (y - start_y) / 32;
				maze++;

				if (maze >= RCT2_ADDRESS(0x009DA151, rct_maze_element)){
					RCT2_GLOBAL(0x00141E9AC, uint16) = 3347;
					return 0;
				}
			} while (!map_element_is_last_for_tile(map_element++));

		}
		x = 0;
	}

	rct_ride* ride = GET_RIDE(rideIndex);
	uint16 location = ride->entrances[0];

	if (location == 0xFFFF){
		RCT2_GLOBAL(0x00141E9AC, uint16) = 3347;
		return 0;
	}

	sint16 x = (location & 0xFF) * 32;
	sint16 y = ((location & 0xFF00) >> 8) * 32;

	map_element = map_get_first_element_at(x / 32, y / 32);

	do{
		if (map_element_get_type(map_element) != MAP_ELEMENT_TYPE_ENTRANCE)
			continue;
		if (map_element->properties.entrance.type != ENTRANCE_TYPE_RIDE_ENTRANCE)
			continue;
		if (map_element->properties.entrance.ride_index == rideIndex)
			break;
	} while (!map_element_is_last_for_tile(map_element++));
	// Add something that stops this from walking off the end

	uint8 entrance_direction = map_element->type & MAP_ELEMENT_DIRECTION_MASK;
	maze->unk_2 = entrance_direction;
	maze->type = 8;

	maze->x = (sint8)((x - start_x) / 32);
	maze->y = (sint8)((y - start_y) / 32);

	maze++;

	location = ride->exits[0];

	if (location == 0xFFFF){
		RCT2_GLOBAL(0x00141E9AC, uint16) = 3347;
		return 0;
	}

	x = (location & 0xFF) * 32;
	y = ((location & 0xFF00) >> 8) * 32;

	map_element = map_get_first_element_at(x / 32, y / 32);

	do{
		if (map_element_get_type(map_element) != MAP_ELEMENT_TYPE_ENTRANCE)
			continue;
		if (map_element->properties.entrance.type != ENTRANCE_TYPE_RIDE_EXIT)
			continue;
		if (map_element->properties.entrance.ride_index == rideIndex)
			break;
	} while (!map_element_is_last_for_tile(map_element++));
	// Add something that stops this from walking off the end

	uint8 exit_direction = map_element->type & MAP_ELEMENT_DIRECTION_MASK;
	maze->unk_2 = exit_direction;
	maze->type = 0x80;

	maze->x = (sint8)((x - start_x) / 32);
	maze->y = (sint8)((y - start_y) / 32);

	maze++;

	maze->all = 0;
	maze++;

	track_elements = (uint8*)maze;
	*track_elements++ = 0xFF;

	RCT2_GLOBAL(0x00F44058, uint8*) = track_elements;

	// Previously you had to save start_x, y, z but 
	// no need since global vars not used
	sub_6D01B3(0, 0, 4096, 4096, 0);

	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, sint16) &= 0xFFF9;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, sint16) &= 0xFFF7;

	x = RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_X_MAX, sint16) -
		RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_X_MIN, sint16);

	y = RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_Y_MAX, sint16) -
		RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_Y_MIN, sint16);

	x /= 32;
	y /= 32;
	x++;
	y++;

	track_design->space_required_x = (uint8)x;
	track_design->space_required_y = (uint8)y;

	return 1;
}

/* rct2: 0x006CE68D */
int tracked_ride_to_td6(uint8 rideIndex, rct_track_td6* track_design, uint8* track_elements){
	rct_ride* ride = GET_RIDE(rideIndex);

	rct_xy_element trackElement;

	if (sub_6CAF80(rideIndex, &trackElement) == 0){
		RCT2_GLOBAL(0x00141E9AC, uint16) = 3347;
		return 0;
	}

	int z = 0;
	//6ce69e
	if (!(sub_6C6402(&trackElement.element, &trackElement.x, &trackElement.y, &z))){
		rct_map_element* initial_map = trackElement.element;
		do {
			int x = trackElement.x;
			int y = trackElement.y;
			rct_map_element* map_element = trackElement.element;
			if (sub_6C6402(&map_element, &x, &y, &z)){
				break;
			}
			trackElement.x = x;
			trackElement.y = y;
			trackElement.element = map_element;
		} while (initial_map != trackElement.element);
	}

	z = trackElement.element->base_height * 8;
	uint8 track_type = trackElement.element->properties.track.type;
	uint8 direction = trackElement.element->type & MAP_ELEMENT_DIRECTION_MASK;
	RCT2_GLOBAL(0x00F4414D, uint8) = direction;

	if (sub_6C683D(&trackElement.x, &trackElement.y, &z, direction, track_type, 0, &trackElement.element, 0)){
		RCT2_GLOBAL(0x00141E9AC, uint16) = 3347;
		return 0;
	}

	const rct_track_coordinates *trackCoordinates = &TrackCoordinates[trackElement.element->properties.track.type];
	// Used in the following loop to know when we have
	// completed all of the elements and are back at the
	// start.
	rct_map_element* initial_map = trackElement.element;

	sint16 start_x = trackElement.x;
	sint16 start_y = trackElement.y;
	sint16 start_z = z + trackCoordinates->z_negative;
	RCT2_GLOBAL(0x00F44142, sint16) = start_x;
	RCT2_GLOBAL(0x00F44144, sint16) = start_y;
	RCT2_GLOBAL(0x00F44146, sint16) = start_z;

	rct_track_element* track = (rct_track_element*)track_elements;
	do{
		track->type = trackElement.element->properties.track.type;
		if (track->type == 0xFF)
			track->type = 101;

		if (track->type == TRACK_ELEM_LEFT_VERTICAL_LOOP ||
			track->type == TRACK_ELEM_RIGHT_VERTICAL_LOOP)
			track_design->flags |= (1 << 7);

		if (track->type == TRACK_ELEM_LEFT_TWIST_DOWN_TO_UP ||
			track->type == TRACK_ELEM_RIGHT_TWIST_DOWN_TO_UP ||
			track->type == TRACK_ELEM_LEFT_TWIST_UP_TO_DOWN ||
			track->type == TRACK_ELEM_RIGHT_TWIST_UP_TO_DOWN)
			track_design->flags |= (1 << 17);

		if (track->type == TRACK_ELEM_LEFT_BARREL_ROLL_UP_TO_DOWN ||
			track->type == TRACK_ELEM_RIGHT_BARREL_ROLL_UP_TO_DOWN ||
			track->type == TRACK_ELEM_LEFT_BARREL_ROLL_DOWN_TO_UP ||
			track->type == TRACK_ELEM_RIGHT_BARREL_ROLL_DOWN_TO_UP)
			track_design->flags |= (1 << 29);

		if (track->type == TRACK_ELEM_HALF_LOOP_UP ||
			track->type == TRACK_ELEM_HALF_LOOP_DOWN)
			track_design->flags |= (1 << 18);

		if (track->type == TRACK_ELEM_LEFT_CORKSCREW_UP ||
			track->type == TRACK_ELEM_RIGHT_CORKSCREW_UP ||
			track->type == TRACK_ELEM_LEFT_CORKSCREW_DOWN ||
			track->type == TRACK_ELEM_RIGHT_CORKSCREW_DOWN)
			track_design->flags |= (1 << 19);

		if (track->type == TRACK_ELEM_WATER_SPLASH)
			track_design->flags |= (1 << 27);

		if (track->type == TRACK_ELEM_POWERED_LIFT)
			track_design->flags |= (1 << 30);

		if (track->type == TRACK_ELEM_LEFT_LARGE_HALF_LOOP_UP ||
			track->type == TRACK_ELEM_RIGHT_LARGE_HALF_LOOP_UP ||
			track->type == TRACK_ELEM_RIGHT_LARGE_HALF_LOOP_DOWN ||
			track->type == TRACK_ELEM_LEFT_LARGE_HALF_LOOP_DOWN)
			track_design->flags |= (1 << 31);

		if (track->type == TRACK_ELEM_LOG_FLUME_REVERSER)
			track_design->var_6C |= (1 << 1);

		uint8 bh;
		if (track->type == TRACK_ELEM_BRAKES){
			bh = trackElement.element->properties.track.sequence >> 4;
		}
		else{
			bh = trackElement.element->properties.track.colour >> 4;
		}

		uint8 flags = trackElement.element->type & (1 << 7) | bh;
		flags |= (trackElement.element->properties.track.colour & 3) << 4;

		if (RCT2_ADDRESS(0x0097D4F2, uint16)[ride->type * 4] & (1 << 3) &&
			trackElement.element->properties.track.colour & (1 << 2)){
			flags |= (1 << 6);
		}

		track->flags = flags;
		track++;

		if (!track_get_next(&trackElement, &trackElement))
			break;

		z = trackElement.element->base_height * 8;
		direction = trackElement.element->type & MAP_ELEMENT_DIRECTION_MASK;
		track_type = trackElement.element->properties.track.type;

		if (sub_6C683D(&trackElement.x, &trackElement.y, &z, direction, track_type, 0, &trackElement.element, 0))
			break;

	} while (trackElement.element != initial_map);

	track_elements = (uint8*)track;
	// Mark the elements as finished.
	*track_elements++ = 0xFF;

	rct_track_entrance* entrance = (rct_track_entrance*)track_elements;

	// First entrances, second exits
	for (int i = 0; i < 2; ++i){

		for (int station_index = 0; station_index < 4; ++station_index){

			z = ride->station_heights[station_index];

			uint16 location;
			if (i == 0)location = ride->entrances[station_index];
			else location = ride->exits[station_index];

			if (location == 0xFFFF)
				continue;

			int x = (location & 0xFF) * 32;
			int y = ((location & 0xFF00) >> 8) * 32;

			rct_map_element* map_element = map_get_first_element_at(x / 32, y / 32);

			do{
				if (map_element_get_type(map_element) != MAP_ELEMENT_TYPE_ENTRANCE)
					continue;
				if (map_element->base_height == z)
					break;
			} while (!map_element_is_last_for_tile(map_element++));
			// Add something that stops this from walking off the end

			uint8 entrance_direction = map_element->type & MAP_ELEMENT_DIRECTION_MASK;
			entrance_direction -= RCT2_GLOBAL(0x00F4414D, uint8);
			entrance_direction &= MAP_ELEMENT_DIRECTION_MASK;

			entrance->direction = entrance_direction;

			x -= RCT2_GLOBAL(0x00F44142, sint16);
			y -= RCT2_GLOBAL(0x00F44144, sint16);

			switch (RCT2_GLOBAL(0x00F4414D, uint8)){
			case MAP_ELEMENT_DIRECTION_WEST:
				// Nothing required
				break;
			case MAP_ELEMENT_DIRECTION_NORTH:
			{
				int temp_y = -y;
				y = x;
				x = temp_y;
			}
				break;
			case MAP_ELEMENT_DIRECTION_EAST:
				x = -x;
				y = -y;
				break;
			case MAP_ELEMENT_DIRECTION_SOUTH:
			{
				int temp_x = -x;
				x = y;
				y = temp_x;
			}
				break;
			}

			entrance->x = x;
			entrance->y = y;

			z *= 8;
			z -= RCT2_GLOBAL(0x00F44146, sint16);
			z /= 8;

			if (z > 127 || z < -126){
				RCT2_GLOBAL(0x00141E9AC, uint16) = 3347;
				return 0;
			}

			if (z == 0xFF)
				z = 0x80;

			entrance->z = z;

			// If this is the exit version
			if (i == 1){
				entrance->direction |= (1 << 7);
			}
			entrance++;
		}
	}

	track_elements = (uint8*)entrance;
	*track_elements++ = 0xFF;
	*track_elements++ = 0xFF;

	RCT2_GLOBAL(0x00F44058, uint8*) = track_elements;

	sub_6D01B3(0, 0, 4096, 4096, 0);

	// Resave global vars for scenery reasons.
	RCT2_GLOBAL(0x00F44142, sint16) = start_x;
	RCT2_GLOBAL(0x00F44144, sint16) = start_y;
	RCT2_GLOBAL(0x00F44146, sint16) = start_z;

	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, sint16) &= 0xFFF9;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, sint16) &= 0xFFF7;

	int x = RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_X_MAX, sint16) -
		RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_X_MIN, sint16);

	int y = RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_Y_MAX, sint16) -
		RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_Y_MIN, sint16);

	x /= 32;
	y /= 32;
	x++;
	y++;

	track_design->space_required_x = x;
	track_design->space_required_y = y;

	return 1;
}

/* rct2: 0x006CE44F */
int ride_to_td6(uint8 rideIndex){
	rct_ride* ride = GET_RIDE(rideIndex);
	rct_track_td6* track_design = RCT2_ADDRESS(0x009D8178, rct_track_td6);

	track_design->type = ride->type;
	rct_object_entry_extended* object = &object_entry_groups[OBJECT_TYPE_RIDE].entries[ride->subtype];

	// Note we are only copying rct_object_entry in size and 
	// not the extended as we don't need the chunk size.
	memcpy(&track_design->vehicle_object, object, sizeof(rct_object_entry));

	track_design->ride_mode = ride->mode;

	track_design->version_and_colour_scheme =
		(ride->colour_scheme_type & 3) |
		(1 << 3); // Version .TD6

	for (int i = 0; i < 32; ++i){
		track_design->vehicle_colours[i] = ride->vehicle_colours[i];
		track_design->vehicle_additional_colour[i] = ride->vehicle_colours_extended[i];
	}

	for (int i = 0; i < 4; ++i){
		track_design->track_spine_colour[i] = ride->track_colour_main[i];
		track_design->track_rail_colour[i] = ride->track_colour_additional[i];
		track_design->track_support_colour[i] = ride->track_colour_supports[i];
	}

	track_design->depart_flags = ride->depart_flags;
	track_design->number_of_trains = ride->num_vehicles;
	track_design->number_of_cars_per_train = ride->num_cars_per_train;
	track_design->min_waiting_time = ride->min_waiting_time;
	track_design->max_waiting_time = ride->max_waiting_time;
	track_design->var_50 = ride->operation_option;
	track_design->lift_hill_speed_num_circuits =
		ride->lift_hill_speed |
		(ride->num_circuits << 5);

	track_design->entrance_style = ride->entrance_style;
	track_design->max_speed = (sint8)(ride->max_speed / 65536);
	track_design->average_speed = (sint8)(ride->average_speed / 65536);
	track_design->ride_length = ride_get_total_length(ride) / 65536;
	track_design->max_positive_vertical_g = ride->max_positive_vertical_g / 32;
	track_design->max_negative_vertical_g = ride->max_negative_vertical_g / 32;
	track_design->max_lateral_g = ride->max_lateral_g / 32;
	track_design->inversions = ride->inversions;
	track_design->drops = ride->drops;
	track_design->highest_drop_height = ride->highest_drop_height;

	uint16 total_air_time = (ride->total_air_time * 123) / 1024;
	if (total_air_time > 255)
		total_air_time = 0;
	track_design->total_air_time = (uint8)total_air_time;

	track_design->excitement = ride->ratings.excitement / 10;
	track_design->intensity = ride->ratings.intensity / 10;
	track_design->nausea = ride->ratings.nausea / 10;

	track_design->upkeep_cost = ride->upkeep_cost;
	track_design->flags = 0;
	track_design->var_6C = 0;

	if (ride->lifecycle_flags & RIDE_LIFECYCLE_SIX_FLAGS)
		track_design->var_6C |= (1 << 31);

	uint8* track_elements = RCT2_ADDRESS(0x9D821B, uint8);
	memset(track_elements, 0, 8000);

	if (track_design->type == RIDE_TYPE_MAZE){
		return maze_ride_to_td6(rideIndex, track_design, track_elements);
	}

	return tracked_ride_to_td6(rideIndex, track_design, track_elements);
}

/* rct2: 0x006771DC but not really its branched from that
 * quite far.
 */
int save_track_to_file(rct_track_td6* track_design, char* path){
	window_close_construction_windows();

	uint8* track_file = malloc(0x8000);

	int length = sawyercoding_encode_td6((char*)track_design, track_file, 0x609F);

	FILE *file;

	log_verbose("saving track %s", path);
	file = fopen(path, "wb");
	if (file == NULL) {
		free(track_file);
		log_error("Failed to save %s", path);
		return 0;
	}

	fwrite(track_file, length, 1, file);
	fclose(file);
	free(track_file);

	return 1;
}

/* rct2: 0x006D2804 & 0x006D264D */
int save_track_design(uint8 rideIndex){
	rct_ride* ride = GET_RIDE(rideIndex);

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED)){
		window_error_open(3346, RCT2_GLOBAL(0x141E9AC, rct_string_id));
		return 0;
	}

	if (ride->ratings.excitement == 0xFFFF){
		window_error_open(3346, RCT2_GLOBAL(0x141E9AC, rct_string_id));
		return 0;
	}

	if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_TRACK)) {
		window_error_open(3346, RCT2_GLOBAL(0x141E9AC, rct_string_id));
		return 0;
	}

	if (!ride_to_td6(rideIndex)){
		window_error_open(3346, RCT2_GLOBAL(0x141E9AC, rct_string_id));
		return 0;
	}

	uint8* track_pointer = RCT2_GLOBAL(0x00F44058, uint8*);
	if (RCT2_GLOBAL(0x009DEA6F, uint8) & 1){
		if (!copy_scenery_to_track(&track_pointer))
			return 0;
	}

	while (track_pointer < RCT2_ADDRESS(0x009DE217, uint8))*track_pointer++ = 0;

	char track_name[MAX_PATH];
	// Get track name
	format_string(track_name, ride->name, &ride->name_arguments);

	char path[MAX_PATH];
	subsitute_path(path, RCT2_ADDRESS(RCT2_ADDRESS_TRACKS_PATH, char), track_name);

	strcat(path, ".TD6");

	// Save track design
	format_string(RCT2_ADDRESS(0x141ED68, char), 2306, NULL);

	// Track design files
	format_string(RCT2_ADDRESS(0x141EE68, char), 2305, NULL);

	pause_sounds();

	int result = platform_open_common_file_dialog(
		0, 
		RCT2_ADDRESS(0x141ED68, char), 
		path, 
		"*.TD?", 
		RCT2_ADDRESS(0x141EE68, char));

	unpause_sounds();

	if (result == 0){
		ride_list_item item = { .type = 0xFD, .entry_index = 0 };
		track_load_list(item);
		return 1;
	}

	save_track_to_file(RCT2_ADDRESS(0x009D8178, rct_track_td6), path);

	ride_list_item item = { .type = 0xFC, .entry_index = 0 };
	track_load_list(item);
	gfx_invalidate_screen();
	return 1;
}

/**
*
*  rct2: 0x006D399D
*/
rct_track_design *temp_track_get_info(char* path, uint8** preview)
{
	rct_track_design *trackDesign;
	uint8 *trackDesignList = RCT2_ADDRESS(RCT2_ADDRESS_TRACK_LIST, uint8);
	int i;

	trackDesign = NULL;

	// Check if track design has already been loaded
	for (i = 0; i < 4; i++) {
		if (RCT2_ADDRESS(RCT2_ADDRESS_TRACK_DESIGN_INDEX_CACHE, uint32)[i] == 0) {
			trackDesign = &RCT2_GLOBAL(RCT2_ADDRESS_TRACK_DESIGN_CACHE, rct_track_design*)[i];
			break;
		}
	}

	if (trackDesign == NULL) {
		// Load track design
		i = RCT2_GLOBAL(RCT2_ADDRESS_TRACK_DESIGN_NEXT_INDEX_CACHE, uint32)++;
		if (RCT2_GLOBAL(RCT2_ADDRESS_TRACK_DESIGN_NEXT_INDEX_CACHE, uint32) >= 4)
			RCT2_GLOBAL(RCT2_ADDRESS_TRACK_DESIGN_NEXT_INDEX_CACHE, uint32) = 0;

		RCT2_ADDRESS(RCT2_ADDRESS_TRACK_DESIGN_INDEX_CACHE, uint32)[i] = 0;

		rct_track_td6* loaded_track = NULL;

		log_verbose("Loading track: %s", path);

		if (!(loaded_track = load_track_design(path))) {
			if (preview != NULL) *preview = NULL;
			log_error("Failed to load track: %s", path);
			return NULL;
		}

		trackDesign = &RCT2_GLOBAL(RCT2_ADDRESS_TRACK_DESIGN_CACHE, rct_track_design*)[i];

		object_unload_all();
		if (loaded_track->type == RIDE_TYPE_NULL){
			if (preview != NULL) *preview = NULL;
			log_error("Failed to load track (ride type null): %s", path);
			return NULL;
		}

		if (!object_load(0, &loaded_track->vehicle_object, NULL)){
			if (preview != NULL) *preview = NULL;
			log_error("Failed to load track (vehicle load fail): %s", path);
			return NULL;
		}

		// Copy the track design apart from the preview image
		memcpy(&trackDesign->track_td6, loaded_track, sizeof(rct_track_td6));
		// Load in a new preview image, calculate cost variable, calculate var_06
		draw_track_preview((uint8**)trackDesign->preview);

		trackDesign->track_td6.cost = RCT2_GLOBAL(RCT2_ADDRESS_TRACK_DESIGN_COST, money32);
		trackDesign->track_td6.track_flags = RCT2_GLOBAL(0x00F44151, uint8) & 7;
	}

	// Set preview to correct preview image based on rotation
	if (preview != NULL)
		*preview = trackDesign->preview[RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_ROTATION, uint8)];

	return trackDesign;
}

void window_track_list_format_name(char *dst, const char *src, char colour, char quotes)
{
	if (colour != 0)
		*dst++ = colour;

	if (quotes != 0)
		*dst++ = FORMAT_OPENQUOTES;

	while (*src != '.' && *src != 0) {
		*dst++ = *src++;
	}

	if (quotes != 0)
		*dst++ = FORMAT_ENDQUOTES;

	*dst = 0;
}


/**
*
*  rct2: 0x006D40B2
* returns 0 for copy fail, 1 for success, 2 for file exists.
*/
int install_track(char* source_path, char* dest_name){

	// Make a copy of the track name (no extension)
	char track_name[MAX_PATH] = { 0 };
	char* dest = track_name;
	char* dest_name_pointer = dest_name;
	while (*dest_name_pointer != '.') *dest++ = *dest_name_pointer++;

	// Check if .TD4 file exists under that name
	char* temp_extension_pointer = dest;
	strcat(track_name, ".TD4");

	char dest_path[MAX_PATH];
	subsitute_path(dest_path, RCT2_ADDRESS(RCT2_ADDRESS_TRACKS_PATH, char), track_name);

	if (platform_file_exists(dest_path))
		return 2;

	// Allow a concat onto the track_name but before extension
	*temp_extension_pointer = '\0';

	// Check if .TD6 file exists under that name
	strcat(track_name, ".TD6");

	subsitute_path(dest_path, RCT2_ADDRESS(RCT2_ADDRESS_TRACKS_PATH, char), track_name);

	if (platform_file_exists(dest_path))
		return 2;

	// Set path for actual copy
	subsitute_path(dest_path, RCT2_ADDRESS(RCT2_ADDRESS_TRACKS_PATH, char), dest_name);

	return platform_file_copy(source_path, dest_path);
}

/* rct2: 0x006D13FE */
void game_command_place_track(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp){
	int x = *eax;
	int y = *ecx;
	int z = *edi;
	uint8 flags = *ebx;

	RCT2_GLOBAL(0x009DEA5E, sint16) = x + 16;
	RCT2_GLOBAL(0x009DEA60, sint16) = y + 16;
	RCT2_GLOBAL(0x009DEA62, sint16) = z;

	if (!(flags & (1 << 3))){
		if (RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint8) != 0 && !gConfigCheat.build_in_pause_mode){
			RCT2_GLOBAL(0x00141E9AC, rct_string_id) = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
			*ebx = MONEY32_UNDEFINED;
			return;
		}
	}

	rct_track_td6* track_design = RCT2_ADDRESS(0x009D8178, rct_track_td6);
	rct_object_entry* ride_object = &track_design->vehicle_object;

	uint8 entry_type, entry_index;
	if (!find_object_in_entry_group(ride_object, &entry_type, &entry_index)){
		entry_index = 0xFF;
	}

	RCT2_GLOBAL(0x00141E9AE, rct_string_id) = 988;
	int rideIndex = 0;
	{
		int _eax = 0, 
			_ebx = GAME_COMMAND_FLAG_APPLY, 
			_ecx = 0, 
			_edx = track_design->type | (entry_index << 8), 
			_esi = GAME_COMMAND_6,
			_edi = 0, 
			_ebp = 0;
		game_do_command_p(GAME_COMMAND_6, &_eax, &_ebx, &_ecx, &_edx, &_esi, &_edi, &_ebp);
		if (_ebx == MONEY32_UNDEFINED){
			*ebx = MONEY32_UNDEFINED;
			RCT2_GLOBAL(RCT2_ADDRESS_NEXT_EXPENDITURE_TYPE, uint8) = 0;
			RCT2_GLOBAL(0x00F44121, money32) = MONEY32_UNDEFINED;
			return;
		}
		rideIndex = _edi & 0xFF;
	}

	money32 cost = 0;
	if (!(flags & GAME_COMMAND_FLAG_APPLY)){
		RCT2_GLOBAL(0x00F44150, uint8) = 0;
		cost = sub_6D01B3(1, rideIndex, x, y, z);
		if (RCT2_GLOBAL(0x00F4414E, uint8) & (1 << 1)){
			RCT2_GLOBAL(0x00F44150, uint8) |= 1 << 7;
			cost = sub_6D01B3(0x81, rideIndex, x, y, z);
		}
	}
	else{
		uint8 bl = 0;
		if (flags & (1 << 6)){
			bl = 4;
		}
		else{
			bl = 2;
		}
		bl |= RCT2_GLOBAL(0x00F44150, uint8);
		cost = sub_6D01B3(bl, rideIndex, x, y, z);
	}

	if (cost == MONEY32_UNDEFINED || 
		!(flags & GAME_COMMAND_FLAG_APPLY)){
		rct_string_id error_reason = RCT2_GLOBAL(0x00141E9AC, rct_string_id);
		game_do_command(0, GAME_COMMAND_FLAG_APPLY, 0, rideIndex, GAME_COMMAND_7, 0, 0);
		*ebx = cost;
		RCT2_GLOBAL(0x00141E9AC, rct_string_id) = error_reason;
		RCT2_GLOBAL(RCT2_ADDRESS_NEXT_EXPENDITURE_TYPE, uint8) = 0;
		RCT2_GLOBAL(0x00F44121, money32) = cost;
		return;
	}

	if (entry_index != 0xFF){
		game_do_command(0, GAME_COMMAND_FLAG_APPLY | (2 << 8), 0, rideIndex | (entry_index << 8), GAME_COMMAND_9, 0, 0);
	}

	game_do_command(0, GAME_COMMAND_FLAG_APPLY | (track_design->ride_mode << 8), 0, rideIndex | (0 << 8), GAME_COMMAND_SET_RIDE_SETTING, 0, 0);

	game_do_command(0, GAME_COMMAND_FLAG_APPLY | (0 << 8), 0, rideIndex | (track_design->number_of_trains << 8), GAME_COMMAND_9, 0, 0);

	game_do_command(0, GAME_COMMAND_FLAG_APPLY | (1 << 8), 0, rideIndex | (track_design->number_of_cars_per_train << 8), GAME_COMMAND_9, 0, 0);

	game_do_command(0, GAME_COMMAND_FLAG_APPLY | (track_design->depart_flags << 8), 0, rideIndex | (1 << 8), GAME_COMMAND_SET_RIDE_SETTING, 0, 0);

	game_do_command(0, GAME_COMMAND_FLAG_APPLY | (track_design->min_waiting_time << 8), 0, rideIndex | (2 << 8), GAME_COMMAND_SET_RIDE_SETTING, 0, 0);

	game_do_command(0, GAME_COMMAND_FLAG_APPLY | (track_design->max_waiting_time << 8), 0, rideIndex | (3 << 8), GAME_COMMAND_SET_RIDE_SETTING, 0, 0);

	game_do_command(0, GAME_COMMAND_FLAG_APPLY | (track_design->var_50 << 8), 0, rideIndex | (4 << 8), GAME_COMMAND_SET_RIDE_SETTING, 0, 0);

	game_do_command(0, GAME_COMMAND_FLAG_APPLY | ((track_design->lift_hill_speed_num_circuits & 0x1F) << 8), 0, rideIndex | (8 << 8), GAME_COMMAND_SET_RIDE_SETTING, 0, 0);

	uint8 num_circuits = track_design->lift_hill_speed_num_circuits >> 5;
	if (num_circuits == 0) num_circuits = 1;
	game_do_command(0, GAME_COMMAND_FLAG_APPLY | (num_circuits << 8), 0, rideIndex | (9 << 8), GAME_COMMAND_SET_RIDE_SETTING, 0, 0);

	rct_ride* ride = GET_RIDE(rideIndex);

	ride->lifecycle_flags |= RIDE_LIFECYCLE_18;

	if (track_design->var_6C & (1 << 31)){
		ride->lifecycle_flags |= RIDE_LIFECYCLE_SIX_FLAGS;
	}

	ride->colour_scheme_type = track_design->version_and_colour_scheme & 3;

	uint8 version = track_design->version_and_colour_scheme >> 2;
	if (version >= 2){
		ride->entrance_style = track_design->entrance_style;
	}

	if (version > 1){
		for (int i = 0; i < 4; ++i){
			ride->track_colour_main[i] = track_design->track_spine_colour[i];
			ride->track_colour_additional[i] = track_design->track_rail_colour[i];
			ride->track_colour_supports[i] = track_design->track_support_colour[i];
		}
	}
	else{
		for (int i = 0; i < 4; ++i){
			ride->track_colour_main[i] = track_design->track_spine_colour_rct1;
			ride->track_colour_additional[i] = track_design->track_rail_colour_rct1;
			ride->track_colour_supports[i] = track_design->track_support_colour_rct1;
		}
	}

	for (int i = 0; i < 32; ++i){
		ride->vehicle_colours[i].body_colour = track_design->vehicle_colours[i].body_colour;
		ride->vehicle_colours[i].trim_colour = track_design->vehicle_colours[i].trim_colour;
		ride->vehicle_colours_extended[i] = track_design->vehicle_additional_colour[i];
	}

	ride_set_name(rideIndex, RCT2_ADDRESS(0x009E3504,const char));

	RCT2_GLOBAL(RCT2_ADDRESS_NEXT_EXPENDITURE_TYPE, uint8) = 0;
	*ebx = RCT2_GLOBAL(0x00F44121, money32);
	*edi = rideIndex;
}

/**
 * 
 * rct2: 0x006D3026
 */
void track_save_reset_scenery()
{
	rct_map_element **savedMapElements = (rct_map_element**)0x00F63674;

	RCT2_GLOBAL(0x009DA193, uint8) = 255;
	savedMapElements[0] = (rct_map_element*)0xFFFFFFFF;
	gfx_invalidate_screen();
}

/**
 * 
 * rct2: 0x006D303D
 */
void track_save_select_nearby_scenery(int rideIndex)
{
	rct_map_element *mapElement;

	for (int y = 0; y < 256; y++) {
		for (int x = 0; x < 256; x++) {
			mapElement = map_get_first_element_at(x, y);
			do {
				if (track_save_should_select_scenery_around(rideIndex, mapElement)) {
					track_save_select_nearby_scenery_for_tile(rideIndex, x, y);
					break;
				}
			} while (!map_element_is_last_for_tile(mapElement++));
		}
	}
	gfx_invalidate_screen();
}

static bool track_save_should_select_scenery_around(int rideIndex, rct_map_element *mapElement)
{
	switch (map_element_get_type(mapElement)) {
	case MAP_ELEMENT_TYPE_PATH:
		if ((mapElement->type & 1) && mapElement->properties.path.addition_status == rideIndex)
			return true;
		break;
	case MAP_ELEMENT_TYPE_TRACK:
		if (mapElement->properties.track.ride_index == rideIndex)
			return true;
		break;
	case MAP_ELEMENT_TYPE_ENTRANCE:
		if (mapElement->properties.entrance.type != ENTRANCE_TYPE_RIDE_ENTRANCE)
			break;
		if (mapElement->properties.entrance.type != ENTRANCE_TYPE_RIDE_EXIT)
			break;
		if (mapElement->properties.entrance.ride_index == rideIndex)
			return true;
		break;
	}
	return false;
}

static void track_save_select_nearby_scenery_for_tile(int rideIndex, int cx, int cy)
{
	rct_map_element *mapElement;

	for (int y = cy - 1; y <= cy + 1; y++) {
		for (int x = cx - 1; x <= cx + 1; x++) {
			mapElement = map_get_first_element_at(x, y);
			do {
				int mapElementSelectType = 0;
				switch (map_element_get_type(mapElement)) {
				case MAP_ELEMENT_TYPE_PATH:
					if (!(mapElement->type & 1))
						mapElementSelectType = 6;
					else if (mapElement->properties.path.addition_status == rideIndex)
						mapElementSelectType = 6;
					break;
				case MAP_ELEMENT_TYPE_SCENERY:
					mapElementSelectType = 5;
					break;
				case MAP_ELEMENT_TYPE_FENCE:
					mapElementSelectType = 9;
					break;
				case MAP_ELEMENT_TYPE_SCENERY_MULTIPLE:
					mapElementSelectType = 10;
					break;
				}

				if (mapElementSelectType != 0) {
					bool mapElementAlreadySelected = false;
					rct_map_element **savedMapElement = (rct_map_element**)0x00F63674;
					while (*savedMapElement != (rct_map_element*)0xFFFFFFFF) {
						if (*savedMapElement == mapElement) {
							mapElementAlreadySelected = true;
							break;
						}
						savedMapElement++;
					}

					if (!mapElementAlreadySelected)
						track_save_add_map_element(mapElementSelectType, x * 32, y * 32, mapElement);
				}
			} while (!map_element_is_last_for_tile(mapElement++));
		}
	}
}

/**
 * 
 * rct2: 0x006D2B3C
 */
static void track_save_add_map_element(int mapElementSelectType, int x, int y, rct_map_element *mapElement)
{
	RCT2_CALLPROC_X(0x006D2B3C, x, mapElementSelectType, y, (int)mapElement, 0, 0, 0);
}
