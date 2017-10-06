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

#include "../audio/audio.h"
#include "../cheats.h"
#include "../config/Config.h"
#include "../game.h"
#include "../interface/viewport.h"
#include "../localisation/localisation.h"
#include "../management/Finance.h"
#include "../network/network.h"
#include "../platform/platform.h"
#include "../rct1.h"
#include "../util/sawyercoding.h"
#include "../util/util.h"
#include "../world/map_animation.h"
#include "../world/park.h"
#include "../world/scenery.h"
#include "../world/footpath.h"
#include "ride.h"
#include "ride_data.h"
#include "ride_ratings.h"
#include "RideGroupManager.h"
#include "station.h"
#include "track.h"
#include "track_data.h"
#include "../world/map.h"

uint8 gTrackGroundFlags;

/**　rct2: 0x00997C9D */
const rct_trackdefinition TrackDefinitions[256] = {
    // TYPE                         VANGLE END                  VANGLE START                BANK END                BANK START              SPECIAL
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_FLAT
    { TRACK_STATION_END,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_END_STATION
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_BEGIN_STATION
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_MIDDLE_STATION
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_60,          TRACK_SLOPE_UP_60,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_60_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_FLAT_TO_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_60,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_25_DEG_UP_TO_60_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_60,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_60_DEG_UP_TO_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_25_DEG_UP_TO_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_60_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_FLAT_TO_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_25_DEG_DOWN_TO_60_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_60_DEG_DOWN_TO_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_25_DEG_DOWN_TO_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_LEFT_QUARTER_TURN_5_TILES
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_RIGHT_QUARTER_TURN_5_TILES
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_FLAT_TO_LEFT_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_FLAT_TO_RIGHT_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_LEFT,        TRACK_NONE                  },  // ELEM_LEFT_BANK_TO_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       TRACK_NONE                  },  // ELEM_RIGHT_BANK_TO_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        TRACK_NONE                  },  // ELEM_BANKED_LEFT_QUARTER_TURN_5_TILES
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       TRACK_NONE                  },  // ELEM_BANKED_RIGHT_QUARTER_TURN_5_TILES
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_LEFT,        TRACK_NONE                  },  // ELEM_LEFT_BANK_TO_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       TRACK_NONE                  },  // ELEM_RIGHT_BANK_TO_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_LEFT,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_25_DEG_UP_TO_LEFT_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_25_DEG_UP_TO_RIGHT_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_LEFT,        TRACK_NONE                  },  // ELEM_LEFT_BANK_TO_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       TRACK_NONE                  },  // ELEM_RIGHT_BANK_TO_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_LEFT,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_25_DEG_DOWN_TO_LEFT_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_25_DEG_DOWN_TO_RIGHT_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        TRACK_NONE                  },  // ELEM_LEFT_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       TRACK_NONE                  },  // ELEM_RIGHT_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_DOWN
    { TRACK_S_BEND,                 TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_S_BEND_LEFT
    { TRACK_S_BEND,                 TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_S_BEND_RIGHT
    { TRACK_VERTICAL_LOOP,          TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_UNKNOWN_VERTICAL_LOOP },  // ELEM_LEFT_VERTICAL_LOOP
    { TRACK_VERTICAL_LOOP,          TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_UNKNOWN_VERTICAL_LOOP },  // ELEM_RIGHT_VERTICAL_LOOP
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_LEFT_QUARTER_TURN_3_TILES
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_RIGHT_QUARTER_TURN_3_TILES
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        TRACK_NONE                  },  // ELEM_LEFT_QUARTER_TURN_3_TILES_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       TRACK_NONE                  },  // ELEM_RIGHT_QUARTER_TURN_3_TILES_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_LEFT_QUARTER_TURN_1_TILE
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_RIGHT_QUARTER_TURN_1_TILE
    { TRACK_TWIST,                  TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_LEFT_TWIST_DOWN_TO_UP
    { TRACK_TWIST,                  TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_RIGHT_TWIST_DOWN_TO_UP
    { TRACK_TWIST,                  TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, TRACK_NONE                  },  // ELEM_LEFT_TWIST_UP_TO_DOWN
    { TRACK_TWIST,                  TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, TRACK_NONE                  },  // ELEM_RIGHT_TWIST_UP_TO_DOWN
    { TRACK_HALF_LOOP,              TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        TRACK_HALF_LOOP_UP          },  // ELEM_HALF_LOOP_UP
    { TRACK_HALF_LOOP,              TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, TRACK_HALF_LOOP_DOWN        },  // ELEM_HALF_LOOP_DOWN
    { TRACK_CORKSCREW,              TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_LEFT_CORKSCREW_UP
    { TRACK_CORKSCREW,              TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_RIGHT_CORKSCREW_UP
    { TRACK_CORKSCREW,              TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, TRACK_CORKSCREW_DOWN        },  // ELEM_LEFT_CORKSCREW_DOWN
    { TRACK_CORKSCREW,              TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, TRACK_CORKSCREW_DOWN        },  // ELEM_RIGHT_CORKSCREW_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_UP_60,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_FLAT_TO_60_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_60,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_60_DEG_UP_TO_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_FLAT_TO_60_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_60_DEG_DOWN_TO_FLAT
    { TRACK_TOWER_BASE,             TRACK_VANGLE_TOWER,         TRACK_VANGLE_TOWER,         TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_TOWER_BASE
    { TRACK_TOWER_BASE,             TRACK_VANGLE_TOWER,         TRACK_VANGLE_TOWER,         TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_TOWER_SECTION
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_FLAT_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_25_DEG_UP_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_UP_60,          TRACK_SLOPE_UP_60,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_60_DEG_UP_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_FLAT_TO_25_DEG_UP_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_UP_60,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_25_DEG_UP_TO_60_DEG_UP_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_60,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_60_DEG_UP_TO_25_DEG_UP_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_25_DEG_UP_TO_FLAT_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_25_DEG_DOWN_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_60_DEG_DOWN_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_FLAT_TO_25_DEG_DOWN_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_25_DEG_DOWN_TO_60_DEG_DOWN_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_60_DEG_DOWN_TO_25_DEG_DOWN_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_25_DEG_DOWN_TO_FLAT_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_LEFT_QUARTER_TURN_5_TILES_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_RIGHT_QUARTER_TURN_5_TILES_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_S_BEND_LEFT_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_S_BEND_RIGHT_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_LEFT_QUARTER_TURN_3_TILES_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_RIGHT_QUARTER_TURN_3_TILES_COVERED
    { TRACK_HELIX_SMALL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        TRACK_NONE                  },  // ELEM_LEFT_HALF_BANKED_HELIX_UP_SMALL
    { TRACK_HELIX_SMALL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       TRACK_NONE                  },  // ELEM_RIGHT_HALF_BANKED_HELIX_UP_SMALL
    { TRACK_HELIX_SMALL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        TRACK_NONE                  },  // ELEM_LEFT_HALF_BANKED_HELIX_DOWN_SMALL
    { TRACK_HELIX_SMALL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       TRACK_NONE                  },  // ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_SMALL
    { TRACK_HELIX_SMALL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        TRACK_NONE                  },  // ELEM_LEFT_HALF_BANKED_HELIX_UP_LARGE
    { TRACK_HELIX_SMALL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       TRACK_NONE                  },  // ELEM_RIGHT_HALF_BANKED_HELIX_UP_LARGE
    { TRACK_HELIX_SMALL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        TRACK_NONE                  },  // ELEM_LEFT_HALF_BANKED_HELIX_DOWN_LARGE
    { TRACK_HELIX_SMALL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       TRACK_NONE                  },  // ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_LARGE
    { TRACK_FLAT,                   TRACK_SLOPE_UP_60,          TRACK_SLOPE_UP_60,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_60,          TRACK_SLOPE_UP_60,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_DOWN
    { TRACK_BRAKES,                 TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_BRAKES
    { TRACK_ROTATION_CONTROL_TOGGLE,TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_ROTATION_CONTROL_TOGGLE
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_INVERTED_90_DEG_UP_TO_FLAT_QUARTER_LOOP
    { TRACK_HELIX_LARGE,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        TRACK_NONE                  },  // ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_UP
    { TRACK_HELIX_LARGE,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       TRACK_NONE                  },  // ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_UP
    { TRACK_HELIX_LARGE,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        TRACK_NONE                  },  // ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_DOWN
    { TRACK_HELIX_LARGE,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       TRACK_NONE                  },  // ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_DOWN
    { TRACK_HELIX_LARGE_UNBANKED,   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_LEFT_QUARTER_HELIX_LARGE_UP
    { TRACK_HELIX_LARGE_UNBANKED,   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_RIGHT_QUARTER_HELIX_LARGE_UP
    { TRACK_HELIX_LARGE_UNBANKED,   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_LEFT_QUARTER_HELIX_LARGE_DOWN
    { TRACK_HELIX_LARGE_UNBANKED,   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_RIGHT_QUARTER_HELIX_LARGE_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        TRACK_NONE                  },  // ELEM_25_DEG_UP_LEFT_BANKED
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       TRACK_NONE                  },  // ELEM_25_DEG_UP_RIGHT_BANKED
    { TRACK_WATERFALL,              TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_WATERFALL
    { TRACK_RAPIDS,                 TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_RAPIDS
    { TRACK_ON_RIDE_PHOTO,          TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_ON_RIDE_PHOTO
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        TRACK_NONE                  },  // ELEM_25_DEG_DOWN_LEFT_BANKED
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       TRACK_NONE                  },  // ELEM_25_DEG_DOWN_RIGHT_BANKED
    { TRACK_WATER_SPLASH,           TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_WATER_SPLASH
    { TRACK_FLAT,                   TRACK_SLOPE_UP_60,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_FLAT_TO_60_DEG_UP_LONG_BASE
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_60,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_60_DEG_UP_TO_FLAT_LONG_BASE
    { TRACK_WHIRLPOOL,              TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_WHIRLPOOL
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_FLAT_TO_60_DEG_DOWN_LONG_BASE
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_60_DEG_UP_TO_FLAT_LONG_BASE_122
    { TRACK_LIFT_HILL_CABLE,        TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_CABLE_LIFT_HILL
    { TRACK_REVERSE_FREEFALL,       TRACK_VANGLE_REVERSE_FREEFALL,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_REVERSE_FREEFALL_SLOPE
    { TRACK_REVERSE_FREEFALL,       TRACK_VANGLE_REVERSE_FREEFALL,  TRACK_VANGLE_REVERSE_FREEFALL,  TRACK_BANK_NONE,TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_REVERSE_FREEFALL_VERTICAL
    { TRACK_FLAT,                   TRACK_SLOPE_UP_90,          TRACK_SLOPE_UP_90,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_90_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_90,        TRACK_SLOPE_DOWN_90,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_90_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_UP_90,          TRACK_SLOPE_UP_60,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_60_DEG_UP_TO_90_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_DOWN_90,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_90_DEG_DOWN_TO_60_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_UP_60,          TRACK_SLOPE_UP_90,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_90_DEG_UP_TO_60_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_90,        TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_60_DEG_DOWN_TO_90_DEG_DOWN
    { TRACK_BRAKE_FOR_DROP,         TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_BRAKE_FOR_DROP
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_LEFT_EIGHTH_TO_DIAG
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_RIGHT_EIGHTH_TO_DIAG
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_LEFT_EIGHTH_TO_ORTHOGONAL
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        TRACK_NONE                  },  // ELEM_LEFT_EIGHTH_BANK_TO_DIAG
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       TRACK_NONE                  },  // ELEM_RIGHT_EIGHTH_BANK_TO_DIAG
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        TRACK_NONE                  },  // ELEM_LEFT_EIGHTH_BANK_TO_ORTHOGONAL
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       TRACK_NONE                  },  // ELEM_RIGHT_EIGHTH_BANK_TO_ORTHOGONAL
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_DIAG_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_DIAG_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_60,          TRACK_SLOPE_UP_60,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_DIAG_60_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_DIAG_FLAT_TO_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_60,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_DIAG_25_DEG_UP_TO_60_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_60,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_DIAG_60_DEG_UP_TO_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_DIAG_25_DEG_UP_TO_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_DIAG_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_DIAG_60_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_DIAG_FLAT_TO_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_DIAG_25_DEG_DOWN_TO_60_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_DIAG_60_DEG_DOWN_TO_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_DIAG_25_DEG_DOWN_TO_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_UP_60,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_DIAG_FLAT_TO_60_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_60,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_DIAG_60_DEG_UP_TO_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_DIAG_FLAT_TO_60_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_DIAG_60_DEG_DOWN_TO_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_DIAG_FLAT_TO_LEFT_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_DIAG_FLAT_TO_RIGHT_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_LEFT,        TRACK_NONE                  },  // ELEM_DIAG_LEFT_BANK_TO_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       TRACK_NONE                  },  // ELEM_DIAG_RIGHT_BANK_TO_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_LEFT,        TRACK_NONE                  },  // ELEM_DIAG_LEFT_BANK_TO_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       TRACK_NONE                  },  // ELEM_DIAG_RIGHT_BANK_TO_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_LEFT,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_DIAG_25_DEG_UP_TO_LEFT_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_DIAG_25_DEG_UP_TO_RIGHT_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_LEFT,        TRACK_NONE                  },  // ELEM_DIAG_LEFT_BANK_TO_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       TRACK_NONE                  },  // ELEM_DIAG_RIGHT_BANK_TO_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_LEFT,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_DIAG_25_DEG_DOWN_TO_LEFT_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_DIAG_25_DEG_DOWN_TO_RIGHT_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        TRACK_NONE                  },  // ELEM_DIAG_LEFT_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       TRACK_NONE                  },  // ELEM_DIAG_RIGHT_BANK
    { TRACK_LOG_FLUME_REVERSER,     TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_LOG_FLUME_REVERSER
    { TRACK_SPINNING_TUNNEL,        TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_SPINNING_TUNNEL
    { TRACK_BARREL_ROLL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_LEFT_BARREL_ROLL_UP_TO_DOWN
    { TRACK_BARREL_ROLL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_RIGHT_BARREL_ROLL_UP_TO_DOWN
    { TRACK_BARREL_ROLL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, TRACK_NONE                  },  // ELEM_LEFT_BARREL_ROLL_DOWN_TO_UP
    { TRACK_BARREL_ROLL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, TRACK_NONE                  },  // ELEM_RIGHT_BARREL_ROLL_DOWN_TO_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_LEFT,        TRACK_NONE                  },  // ELEM_LEFT_BANK_TO_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       TRACK_NONE                  },  // ELEM_RIGHT_BANK_TO_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_LEFT,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_LEFT_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_RIGHT_BANK
    { TRACK_POWERED_LIFT,           TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_POWERED_LIFT
    { TRACK_HALF_LOOP_LARGE,        TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        TRACK_HALF_LOOP_UP          },  // ELEM_LEFT_LARGE_HALF_LOOP_UP
    { TRACK_HALF_LOOP_LARGE,        TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        TRACK_HALF_LOOP_UP          },  // ELEM_RIGHT_LARGE_HALF_LOOP_UP
    { TRACK_HALF_LOOP_LARGE,        TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, TRACK_HALF_LOOP_DOWN        },  // ELEM_RIGHT_LARGE_HALF_LOOP_DOWN
    { TRACK_HALF_LOOP_LARGE,        TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, TRACK_HALF_LOOP_DOWN        },  // ELEM_LEFT_LARGE_HALF_LOOP_DOWN
    { TRACK_INLINE_TWIST_UNINVERTED,TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_LEFT_FLYER_TWIST_UP
    { TRACK_INLINE_TWIST_UNINVERTED,TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_RIGHT_FLYER_TWIST_UP
    { TRACK_INLINE_TWIST_INVERTED,  TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_LEFT_FLYER_TWIST_DOWN
    { TRACK_INLINE_TWIST_INVERTED,  TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_RIGHT_FLYER_TWIST_DOWN
    { TRACK_HALF_LOOP_UNINVERTED,   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        TRACK_HALF_LOOP_UP          },  // ELEM_FLYER_HALF_LOOP_UP
    { TRACK_HALF_LOOP_INVERTED,     TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        TRACK_HALF_LOOP_DOWN        },  // ELEM_FLYER_HALF_LOOP_DOWN
    { TRACK_190,                    TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_LEFT_FLYER_CORKSCREW_UP
    { TRACK_190,                    TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_RIGHT_FLYER_CORKSCREW_UP
    { TRACK_192,                    TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        TRACK_CORKSCREW_DOWN        },  // ELEM_LEFT_FLYER_CORKSCREW_DOWN
    { TRACK_192,                    TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        TRACK_CORKSCREW_DOWN        },  // ELEM_RIGHT_FLYER_CORKSCREW_DOWN
    { TRACK_194,                    TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_HEARTLINE_TRANSFER_UP
    { TRACK_194,                    TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_HEARTLINE_TRANSFER_DOWN
    { TRACK_HEARTLINE_ROLL,         TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_LEFT_HEARTLINE_ROLL
    { TRACK_HEARTLINE_ROLL,         TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_RIGHT_HEARTLINE_ROLL
    { TRACK_MINI_GOLF_HOLE,         TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_MINI_GOLF_HOLE_A
    { TRACK_MINI_GOLF_HOLE,         TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_MINI_GOLF_HOLE_B
    { TRACK_MINI_GOLF_HOLE,         TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_MINI_GOLF_HOLE_C
    { TRACK_MINI_GOLF_HOLE,         TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_MINI_GOLF_HOLE_D
    { TRACK_MINI_GOLF_HOLE,         TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_MINI_GOLF_HOLE_E
    { TRACK_QUARTER_LOOP_INVERTED,  TRACK_SLOPE_DOWN_90,        TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_MULTIDIM_INVERTED_FLAT_TO_90_DEG_QUARTER_LOOP_DOWN
    { TRACK_QUARTER_LOOP,           TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_90,          TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_90_DEG_TO_INVERTED_FLAT_QUARTER_LOOP_UP
    { TRACK_QUARTER_LOOP,           TRACK_SLOPE_DOWN_90,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, TRACK_NONE                  },  // ELEM_INVERTED_FLAT_TO_90_DEG_QUARTER_LOOP_DOWN
    { TRACK_LIFT_HILL_CURVED,       TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_LEFT_CURVED_LIFT_HILL
    { TRACK_LIFT_HILL_CURVED,       TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_RIGHT_CURVED_LIFT_HILL
    { TRACK_REVERSER,               TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_LEFT_REVERSER
    { TRACK_REVERSER,               TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_RIGHT_REVERSER
    { TRACK_SLOPE_TO_FLAT,          TRACK_SLOPE_DOWN_90,        TRACK_SLOPE_UP_90,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_AIR_THRUST_TOP_CAP
    { TRACK_SLOPE_TO_FLAT,          TRACK_SLOPE_DOWN_90,        TRACK_SLOPE_DOWN_90,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_AIR_THRUST_VERTICAL_DOWN
    { TRACK_SLOPE_TO_FLAT,          TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_90,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_AIR_THRUST_VERTICAL_DOWN_TO_LEVEL
    { TRACK_BLOCK_BRAKES,           TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_BLOCK_BRAKES
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        TRACK_NONE                  },  // ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       TRACK_NONE                  },  // ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        TRACK_NONE                  },  // ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       TRACK_NONE                  },  // ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        TRACK_NONE                  },  // ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       TRACK_NONE                  },  // ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        TRACK_NONE                  },  // ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       TRACK_NONE                  },  // ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_LEFT,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_25_DEG_UP_TO_LEFT_BANKED_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_25_DEG_UP_TO_RIGHT_BANKED_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_LEFT,        TRACK_NONE                  },  // ELEM_LEFT_BANKED_25_DEG_UP_TO_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       TRACK_NONE                  },  // ELEM_RIGHT_BANKED_25_DEG_UP_TO_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_LEFT,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_25_DEG_DOWN_TO_LEFT_BANKED_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_25_DEG_DOWN_TO_RIGHT_BANKED_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_LEFT,        TRACK_NONE                  },  // ELEM_LEFT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       TRACK_NONE                  },  // ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        TRACK_NONE                  },  // ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       TRACK_NONE                  },  // ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        TRACK_NONE                  },  // ELEM_LEFT_BANKED_25_DEG_UP_TO_LEFT_BANKED_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       TRACK_NONE                  },  // ELEM_RIGHT_BANKED_25_DEG_UP_TO_RIGHT_BANKED_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        TRACK_NONE                  },  // ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       TRACK_NONE                  },  // ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        TRACK_NONE                  },  // ELEM_LEFT_BANKED_25_DEG_DOWN_TO_LEFT_BANKED_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       TRACK_NONE                  },  // ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_RIGHT_BANKED_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_FLAT_TO_LEFT_BANKED_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_LEFT,        TRACK_NONE                  },  // ELEM_LEFT_BANKED_25_DEG_UP_TO_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       TRACK_NONE                  },  // ELEM_RIGHT_BANKED_25_DEG_UP_TO_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_FLAT_TO_LEFT_BANKED_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_LEFT,        TRACK_NONE                  },  // ELEM_LEFT_BANKED_25_DEG_DOWN_TO_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       TRACK_NONE                  },  // ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_UP_90,          TRACK_SLOPE_UP_90,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_LEFT_QUARTER_TURN_1_TILE_90_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_90,          TRACK_SLOPE_UP_90,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_RIGHT_QUARTER_TURN_1_TILE_90_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_90,        TRACK_SLOPE_DOWN_90,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_LEFT_QUARTER_TURN_1_TILE_90_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_90,        TRACK_SLOPE_DOWN_90,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_RIGHT_QUARTER_TURN_1_TILE_90_DEG_DOWN
    { TRACK_QUARTER_LOOP_UNINVERTED,TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_90,          TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_MULTIDIM_90_DEG_UP_TO_INVERTED_FLAT_QUARTER_LOOP
    { TRACK_QUARTER_LOOP_UNINVERTED,TRACK_SLOPE_DOWN_90,        TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        TRACK_NONE                  },  // ELEM_MULTIDIM_FLAT_TO_90_DEG_DOWN_QUARTER_LOOP
    { TRACK_QUARTER_LOOP_INVERTED,  TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_90,          TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        TRACK_NONE                  }, // 255
};

/**　rct2: 0x0099849D */
const rct_trackdefinition FlatRideTrackDefinitions[256] = {
    // TYPE                         VANGLE END                  VANGLE START                BANK END                BANK START              SPECIAL
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 0
    { TRACK_STATION_END,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 1
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 2
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 3
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 4
    { TRACK_FLAT,                   TRACK_SLOPE_UP_60,          TRACK_SLOPE_UP_60,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 5
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 6
    { TRACK_FLAT,                   TRACK_SLOPE_UP_60,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 7
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_60,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 8
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 9
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 10
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 11
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 12
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 13
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 14
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 15
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 16
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 17
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_NONE,        0                           },  // 18
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        0                           },  // 19
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_LEFT,        0                           },  // 20
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       0                           },  // 21
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                           },  // 22
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                           },  // 23
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_LEFT,        0                           },  // 24
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       0                           },  // 25
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_LEFT,        TRACK_BANK_NONE,        0                           },  // 26
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        0                           },  // 27
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_LEFT,        0                           },  // 28
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       0                           },  // 29
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_LEFT,        TRACK_BANK_NONE,        0                           },  // 30
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        0                           },  // 31
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                           },  // 32
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                           },  // 33
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 34
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 35
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 36
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 37
    { TRACK_S_BEND,                 TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 38
    { TRACK_S_BEND,                 TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 39
    { TRACK_VERTICAL_LOOP,          TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_UNKNOWN_VERTICAL_LOOP },  // 40
    { TRACK_VERTICAL_LOOP,          TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_UNKNOWN_VERTICAL_LOOP },  // 41
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 42
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 43
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                           },  // 44
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                           },  // 45
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 46
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 47
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 48
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 49
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 50
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 51
    { TRACK_TWIST,                  TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                           },  // 52
    { TRACK_TWIST,                  TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                           },  // 53
    { TRACK_TWIST,                  TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, 0                           },  // 54
    { TRACK_TWIST,                  TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, 0                           },  // 55
    { TRACK_HALF_LOOP,              TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        TRACK_HALF_LOOP_UP          },  // 56
    { TRACK_HALF_LOOP,              TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, TRACK_HALF_LOOP_DOWN        },  // 57
    { TRACK_CORKSCREW,              TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                           },  // 58
    { TRACK_CORKSCREW,              TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                           },  // 59
    { TRACK_CORKSCREW,              TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, TRACK_CORKSCREW_DOWN        },  // 60
    { TRACK_CORKSCREW,              TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, TRACK_CORKSCREW_DOWN        },  // 61
    { TRACK_FLAT,                   TRACK_SLOPE_UP_60,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 62
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_60,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 63
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 64
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 65
    { TRACK_TOWER_BASE,             TRACK_SLOPE_UP_90,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 66
    { TRACK_TOWER_BASE,             TRACK_SLOPE_UP_90,          TRACK_SLOPE_UP_90,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 67
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 68
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 69
    { TRACK_FLAT,                   TRACK_SLOPE_UP_60,          TRACK_SLOPE_UP_60,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 70
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 71
    { TRACK_FLAT,                   TRACK_SLOPE_UP_60,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 72
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_60,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 73
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 74
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 75
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 76
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 77
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 78
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 79
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 80
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 81
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 82
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 83
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 84
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 85
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 86
    { TRACK_HELIX_SMALL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                           },  // 87
    { TRACK_HELIX_SMALL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                           },  // 88
    { TRACK_HELIX_SMALL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                           },  // 89
    { TRACK_HELIX_SMALL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                           },  // 90
    { TRACK_HELIX_SMALL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                           },  // 91
    { TRACK_HELIX_SMALL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                           },  // 92
    { TRACK_HELIX_SMALL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                           },  // 93
    { TRACK_HELIX_SMALL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                           },  // 94
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // FLAT_TRACK_ELEM_1_X_4_A
    { TRACK_FLAT,                   TRACK_SLOPE_UP_60,          TRACK_SLOPE_UP_60,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 96
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 97
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 98
    { TRACK_BRAKES,                 TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 99
    { TRACK_ROTATION_CONTROL_TOGGLE,TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 100
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 101
    { TRACK_HELIX_LARGE,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                           },  // 102
    { TRACK_HELIX_LARGE,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                           },  // 103
    { TRACK_HELIX_LARGE,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                           },  // 104
    { TRACK_HELIX_LARGE,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                           },  // 105
    { TRACK_HELIX_LARGE_UNBANKED,   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 106
    { TRACK_HELIX_LARGE_UNBANKED,   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 107
    { TRACK_HELIX_LARGE_UNBANKED,   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 108
    { TRACK_HELIX_LARGE_UNBANKED,   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 109
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // FLAT_TRACK_ELEM_2_X_2
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // FLAT_TRACK_ELEM_4_X_4
    { TRACK_WHIRLPOOL,              TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 112
    { TRACK_WHIRLPOOL,              TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 113
    { TRACK_ON_RIDE_PHOTO,          TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 114
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 115
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // FLAT_TRACK_ELEM_1_X_5
    { TRACK_WATER_SPLASH,           TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 117
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // FLAT_TRACK_ELEM_1_X_1_A
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // FLAT_TRACK_ELEM_1_X_4_B
    { TRACK_WHIRLPOOL,              TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 120
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // FLAT_TRACK_ELEM_1_X_1_B
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        TRACK_216                   },  // FLAT_TRACK_ELEM_1_X_4_C
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // FLAT_TRACK_ELEM_3_X_3
    { TRACK_REVERSE_FREEFALL,               TRACK_SLOPE_UP_90,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 124
    { TRACK_REVERSE_FREEFALL,               TRACK_SLOPE_UP_90,          TRACK_SLOPE_UP_90,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 125
    { TRACK_FLAT,                   TRACK_SLOPE_UP_90,          TRACK_SLOPE_UP_90,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 126
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_90,        TRACK_SLOPE_DOWN_90,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 127
    { TRACK_FLAT,                   TRACK_SLOPE_UP_90,          TRACK_SLOPE_UP_60,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 128
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_DOWN_90,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 129
    { TRACK_FLAT,                   TRACK_SLOPE_UP_60,          TRACK_SLOPE_UP_90,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 130
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_90,        TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 131
    { TRACK_BRAKE_FOR_DROP,         TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 132
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 133
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 134
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 135
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 136
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                           },  // 137
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                           },  // 138
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                           },  // 139
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                           },  // 140
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 141
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 142
    { TRACK_FLAT,                   TRACK_SLOPE_UP_60,          TRACK_SLOPE_UP_60,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 143
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 144
    { TRACK_FLAT,                   TRACK_SLOPE_UP_60,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 145
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_60,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 146
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 147
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 148
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 149
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 150
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 151
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 152
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 153
    { TRACK_FLAT,                   TRACK_SLOPE_UP_60,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 154
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_60,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 155
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 156
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 157
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_NONE,        0                           },  // 158
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        0                           },  // 159
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_LEFT,        0                           },  // 160
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       0                           },  // 161
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_LEFT,        0                           },  // 162
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       0                           },  // 163
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_LEFT,        TRACK_BANK_NONE,        0                           },  // 164
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        0                           },  // 165
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_LEFT,        0                           },  // 166
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       0                           },  // 167
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_LEFT,        TRACK_BANK_NONE,        0                           },  // 168
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        0                           },  // 169
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                           },  // 170
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                           },  // 171
    { TRACK_LOG_FLUME_REVERSER,     TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 172
    { TRACK_SPINNING_TUNNEL,        TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 173
    { TRACK_BARREL_ROLL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN,TRACK_BANK_NONE,         0                           },  // 174
    { TRACK_BARREL_ROLL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN,TRACK_BANK_NONE,         0                           },  // 175
    { TRACK_BARREL_ROLL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, 0                           },  // 176
    { TRACK_BARREL_ROLL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, 0                           },  // 177
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_LEFT,        0                           },  // 178
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       0                           },  // 179
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_LEFT,        TRACK_BANK_NONE,        0                           },  // 180
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        0                           },  // 181
    { TRACK_POWERED_LIFT,           TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 182
    { TRACK_HALF_LOOP_LARGE,        TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_UPSIDE_DOWN,TRACK_BANK_NONE,         TRACK_HALF_LOOP_UP          },  // 183
    { TRACK_HALF_LOOP_LARGE,        TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_UPSIDE_DOWN,TRACK_BANK_NONE,         TRACK_HALF_LOOP_UP          },  // 184
    { TRACK_HALF_LOOP_LARGE,        TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, TRACK_HALF_LOOP_DOWN        },  // 185
    { TRACK_HALF_LOOP_LARGE,        TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, TRACK_HALF_LOOP_DOWN        },  // 186
    { TRACK_190,                    TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN,TRACK_BANK_NONE,         0                           },  // 187
    { TRACK_190,                    TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN,TRACK_BANK_NONE,         0                           },  // 188
    { TRACK_192,                    TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN,TRACK_BANK_NONE,         0                           },  // 189
    { TRACK_192,                    TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN,TRACK_BANK_NONE,         0                           },  // 190
    { TRACK_190,                    TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_UPSIDE_DOWN,TRACK_BANK_NONE,         TRACK_HALF_LOOP_UP          },  // 191
    { TRACK_192,                    TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN,TRACK_BANK_NONE,         TRACK_HALF_LOOP_DOWN        },  // 192
    { TRACK_190,                    TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN,TRACK_BANK_NONE,         0                           },  // 193
    { TRACK_190,                    TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN,TRACK_BANK_NONE,         0                           },  // 194
    { TRACK_192,                    TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN,TRACK_BANK_NONE,         TRACK_CORKSCREW_DOWN        },  // 195
    { TRACK_192,                    TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN,TRACK_BANK_NONE,         TRACK_CORKSCREW_DOWN        },  // 196
    { TRACK_194,                    TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 197
    { TRACK_194,                    TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 198
    { TRACK_HEARTLINE_ROLL,         TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 199
    { TRACK_HEARTLINE_ROLL,         TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 200
    { TRACK_MINI_GOLF_HOLE,         TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 201
    { TRACK_MINI_GOLF_HOLE,         TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 202
    { TRACK_MINI_GOLF_HOLE,         TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 203
    { TRACK_MINI_GOLF_HOLE,         TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 204
    { TRACK_MINI_GOLF_HOLE,         TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 205
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 206
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 207
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 208
    { TRACK_LIFT_HILL_CURVED,       TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 209
    { TRACK_LIFT_HILL_CURVED,       TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 210
    { TRACK_REVERSER,               TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 211
    { TRACK_REVERSER,               TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 212
    { TRACK_SLOPE_TO_FLAT,          TRACK_SLOPE_DOWN_90,        TRACK_SLOPE_UP_90,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 213
    { TRACK_SLOPE_TO_FLAT,          TRACK_SLOPE_DOWN_90,        TRACK_SLOPE_DOWN_90,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 214
    { TRACK_SLOPE_TO_FLAT,          TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_90,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 215
    { TRACK_BLOCK_BRAKES,           TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 216
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                           },  // 217
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                           },  // 218
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                           },  // 219
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                           },  // 220
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                           },  // 221
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                           },  // 222
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                           },  // 223
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                           },  // 224
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_LEFT,        TRACK_BANK_NONE,        0                           },  // 225
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        0                           },  // 226
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_LEFT,        0                           },  // 227
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       0                           },  // 228
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_LEFT,        TRACK_BANK_NONE,        0                           },  // 229
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        0                           },  // 230
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_LEFT,        0                           },  // 231
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       0                           },  // 232
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                           },  // 233
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                           },  // 234
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                           },  // 235
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                           },  // 236
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                           },  // 237
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                           },  // 238
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                           },  // 239
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                           },  // 240
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_NONE,        0                           },  // 241
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        0                           },  // 242
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_LEFT,        0                           },  // 243
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       0                           },  // 244
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_NONE,        0                           },  // 245
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        0                           },  // 246
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_LEFT,        0                           },  // 247
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       0                           },  // 248
    { TRACK_FLAT,                   TRACK_SLOPE_UP_90,          TRACK_SLOPE_UP_90,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 249
    { TRACK_FLAT,                   TRACK_SLOPE_UP_90,          TRACK_SLOPE_UP_90,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 250
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_90,        TRACK_SLOPE_DOWN_90,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 251
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_90,        TRACK_SLOPE_DOWN_90,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 252
    { TRACK_QUARTER_LOOP_UNINVERTED,TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_90,          TRACK_BANK_UPSIDE_DOWN,TRACK_BANK_NONE,         0                           },  // 253
    { TRACK_QUARTER_LOOP_UNINVERTED,TRACK_SLOPE_DOWN_90,        TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN,TRACK_BANK_NONE,         0                           },  // 254
};

/**
 * Helper method to determine if a connects to b by its bank and angle, not location.
 */
sint32 track_is_connected_by_shape(rct_map_element *a, rct_map_element *b)
{
    sint32 trackType, aBank, aAngle, bBank, bAngle;

    trackType = a->properties.track.type;
    aBank = TrackDefinitions[trackType].bank_end;
    aAngle = TrackDefinitions[trackType].vangle_end;
    aBank = track_get_actual_bank(a, aBank);

    trackType = b->properties.track.type;
    bBank = TrackDefinitions[trackType].bank_start;
    bAngle = TrackDefinitions[trackType].vangle_start;
    bBank = track_get_actual_bank(b, bBank);

    return aBank == bBank && aAngle == bAngle;
}

const rct_preview_track *get_track_def_from_ride(Ride *ride, sint32 trackType)
{
    return ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE) ?
        FlatRideTrackBlocks[trackType] :
        TrackBlocks[trackType];
}

const rct_track_coordinates *get_track_coord_from_ride(Ride *ride, sint32 trackType){
    return ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE) ?
        &FlatTrackCoordinates[trackType] :
        &TrackCoordinates[trackType];
}

const rct_preview_track *get_track_def_from_ride_index(sint32 rideIndex, sint32 trackType)
{
    return get_track_def_from_ride(get_ride(rideIndex), trackType);
}

static rct_map_element *find_station_element(sint32 x, sint32 y, sint32 z, sint32 direction, sint32 rideIndex)
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

static void ride_remove_station(Ride *ride, sint32 x, sint32 y, sint32 z)
{
    for (sint32 i = 0; i < MAX_STATIONS; i++)
    {
        if (ride->station_starts[i].x == (x >> 5) && ride->station_starts[i].y == (y >> 5) && ride->station_heights[i] == z)
        {
            ride->station_starts[i].xy = RCT_XY8_UNDEFINED;
            ride->num_stations--;
            break;
        }
    }
}

/**
 *
 *  rct2: 0x006C4D89
 */
static bool track_add_station_element(sint32 x, sint32 y, sint32 z, sint32 direction, sint32 rideIndex, sint32 flags)
{
    sint32 stationX0 = x;
    sint32 stationY0 = y;
    sint32 stationX1 = x;
    sint32 stationY1 = y;
    sint32 stationLength = 1;

    Ride *ride = get_ride(rideIndex);
    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_3)) {
        if (ride->num_stations >= MAX_STATIONS) {
            gGameCommandErrorText = STR_NO_MORE_STATIONS_ALLOWED_ON_THIS_RIDE;
            return false;
        }
        if (flags & GAME_COMMAND_FLAG_APPLY)
        {
            sint8 stationIndex = ride_get_first_empty_station_start(ride);
            assert(stationIndex != -1);

            ride->station_starts[stationIndex].x = (x >> 5);
            ride->station_starts[stationIndex].y = (y >> 5);
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

    if (stationX0 == stationX1 && stationY0 == stationY1 && ride->num_stations >= MAX_STATIONS) {
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
                sint32 targetTrackType;
                if (x == stationX1 && y == stationY1)
                {
                    sint8 stationIndex = ride_get_first_empty_station_start(ride);
                    assert(stationIndex != -1);

                    ride->station_starts[stationIndex].x = (x >> 5);
                    ride->station_starts[stationIndex].y = (y >> 5);
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
static bool track_remove_station_element(sint32 x, sint32 y, sint32 z, sint32 direction, sint32 rideIndex, sint32 flags)
{
    sint32 removeX = x;
    sint32 removeY = y;
    sint32 stationX0 = x;
    sint32 stationY0 = y;
    sint32 stationX1 = x;
    sint32 stationY1 = y;
    sint32 stationLength = 0;
    sint32 byte_F441D1 = -1;

    Ride *ride = get_ride(rideIndex);
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
            ride->num_stations >= MAX_STATIONS
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
                sint32 targetTrackType;
                if (x == stationX1 && y == stationY1)
                {
                loc_6C4BF5:;
                    sint8 stationIndex = ride_get_first_empty_station_start(ride);
                    assert(stationIndex != -1);

                    ride->station_starts[stationIndex].x = (x >> 5);
                    ride->station_starts[stationIndex].y = (y >> 5);
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

static money32 track_place(sint32 rideIndex, sint32 type, sint32 originX, sint32 originY, sint32 originZ, sint32 direction, sint32 brakeSpeed, sint32 colour, sint32 seatRotation, sint32 liftHillAndAlternativeState, sint32 flags)
{
    Ride *ride = get_ride(rideIndex);
    if (ride == NULL)
    {
        log_warning("Invalid ride for track placement, rideIndex = %d", rideIndex);
        return MONEY32_UNDEFINED;
    }
    if (ride->type == RIDE_TYPE_NULL) {
        log_warning("Invalid ride type, rideIndex = %d", rideIndex);
        return MONEY32_UNDEFINED;
    }
    rct_ride_entry *rideEntry = get_ride_entry(ride->subtype);
    if (rideEntry == (rct_ride_entry *)-1 || rideEntry == NULL)
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
    gTrackGroundFlags = 0;

    uint64 enabledTrackPieces = rideEntry->enabledTrackPieces & RideTypePossibleTrackConfigurations[ride->type];
    uint32 rideTypeFlags = RideProperties[ride->type].flags;

    if ((ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK) && type == TRACK_ELEM_END_STATION) {
        gGameCommandErrorText = STR_NOT_ALLOWED_TO_MODIFY_STATION;
        return MONEY32_UNDEFINED;
    }

    if (!(flags & GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED)) {
        if (game_is_paused() && !gCheatsBuildInPauseMode) {
            gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
            return MONEY32_UNDEFINED;
        }
    }

    const uint8 (*wallEdges)[16];
    if (rideTypeFlags & RIDE_TYPE_FLAG_FLAT_RIDE) {
        wallEdges = &FlatRideTrackSequenceElementAllowedWallEdges[type];
    } else {
        if (type == TRACK_ELEM_ON_RIDE_PHOTO) {
            if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO) {
                gGameCommandErrorText = STR_ONLY_ONE_ON_RIDE_PHOTO_PER_RIDE;
                return MONEY32_UNDEFINED;
            }
        } else if (type == TRACK_ELEM_CABLE_LIFT_HILL) {
            if (ride->lifecycle_flags & RIDE_LIFECYCLE_CABLE_LIFT_HILL_COMPONENT_USED) {
                gGameCommandErrorText = STR_ONLY_ONE_CABLE_LIFT_HILL_PER_RIDE;
                return MONEY32_UNDEFINED;
            }
        }
        // Backwards steep lift hills are allowed, even on roller coasters that do not support forwards steep lift hills.
        if ((liftHillAndAlternativeState & CONSTRUCTION_LIFT_HILL_SELECTED) && !(enabledTrackPieces & (1ULL << TRACK_LIFT_HILL_STEEP)) && !gCheatsEnableChainLiftOnAllTrack) {
            if (TrackFlags[type] & TRACK_ELEM_FLAG_IS_STEEP_UP) {
                gGameCommandErrorText = STR_TOO_STEEP_FOR_LIFT_HILL;
                return MONEY32_UNDEFINED;
            }
        }

        wallEdges = &TrackSequenceElementAllowedWallEdges[type];
    }

    money32 cost = 0;
    const rct_preview_track *trackBlock = get_track_def_from_ride(ride, type);
    uint32 num_elements = 0;
    // First check if any of the track pieces are outside the park
    for (; trackBlock->index != 0xFF; trackBlock++) {
        sint32 offsetX = 0;
        sint32 offsetY = 0;

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

        sint32 x = originX + offsetX;
        sint32 y = originY + offsetY;
        sint32 z = originZ + trackBlock->z;

        if (!map_is_location_owned(x, y, z) && !gCheatsSandboxMode) {
            gGameCommandErrorText = STR_LAND_NOT_OWNED_BY_PARK;
            return MONEY32_UNDEFINED;
        }
        num_elements++;
    }

    if (!map_check_free_elements_and_reorganise(num_elements)) {
        return MONEY32_UNDEFINED;
    }
    const uint16 *trackFlags = (rideTypeFlags & RIDE_TYPE_FLAG_FLAT_RIDE) ?
        FlatTrackFlags :
        TrackFlags;
    if (trackFlags[type] & TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT) {
        if ((originZ & 0x0F) != 8) {
            gGameCommandErrorText = STR_CONSTRUCTION_ERR_UNKNOWN;
            return MONEY32_UNDEFINED;
        }
    } else {
        if ((originZ & 0x0F) != 0) {
            gGameCommandErrorText = STR_CONSTRUCTION_ERR_UNKNOWN;
            return MONEY32_UNDEFINED;
        }
    }

    // If that is not the case, then perform the remaining checks
    trackBlock = get_track_def_from_ride(ride, type);

    for (sint32 blockIndex = 0; trackBlock->index != 0xFF; trackBlock++, blockIndex++) {
        sint32 offsetX = 0;
        sint32 offsetY = 0;
        sint32 bl = trackBlock->var_08;
        sint32 bh;
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
        sint32 x = originX + offsetX;
        sint32 y = originY + offsetY;
        sint32 z = originZ + trackBlock->z;

        trackpieceZ = z;

        if (z < 16) {
            gGameCommandErrorText = STR_TOO_LOW;
            return MONEY32_UNDEFINED;
        }

        sint32 baseZ = (originZ + trackBlock->z) / 8;

        sint32 clearanceZ = trackBlock->var_07;
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

        if ((flags & GAME_COMMAND_FLAG_APPLY) && !(flags & GAME_COMMAND_FLAG_GHOST) && !gCheatsDisableClearanceChecks) {
            footpath_remove_litter(x, y, z);
            if (rideTypeFlags & RIDE_TYPE_FLAG_TRACK_NO_WALLS) {
                wall_remove_at(x, y, baseZ * 8, clearanceZ * 8);
            } else {
                // Remove walls in the directions this track intersects
                uint8 intersectingDirections = (*wallEdges)[blockIndex];
                intersectingDirections ^= 0x0F;
                for (sint32 i = 0; i < 4; i++) {
                    if (intersectingDirections & (1 << i)) {
                        wall_remove_intersecting_walls(x, y, baseZ, clearanceZ, i);
                    }
                }
            }
        }

        bh = gMapGroundFlags & (ELEMENT_IS_ABOVE_GROUND | ELEMENT_IS_UNDERGROUND);
        if (gTrackGroundFlags != 0 && (gTrackGroundFlags & bh) == 0) {
            gGameCommandErrorText = STR_CANT_BUILD_PARTLY_ABOVE_AND_PARTLY_BELOW_GROUND;
            return MONEY32_UNDEFINED;
        }

        gTrackGroundFlags = bh;
        if (rideTypeFlags & RIDE_TYPE_FLAG_FLAT_RIDE) {
            if (FlatTrackFlags[type] & TRACK_ELEM_FLAG_ONLY_ABOVE_GROUND) {
                if (gTrackGroundFlags & TRACK_ELEMENT_LOCATION_IS_UNDERGROUND) {
                    gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND;
                    return MONEY32_UNDEFINED;
                }
            }
        }
        else {
            if (TrackFlags[type] & TRACK_ELEM_FLAG_ONLY_ABOVE_GROUND) {
                if (gTrackGroundFlags & TRACK_ELEMENT_LOCATION_IS_UNDERGROUND) {
                    gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND;
                    return MONEY32_UNDEFINED;
                }
            }
        }

        if (rideTypeFlags & RIDE_TYPE_FLAG_FLAT_RIDE) {
            if (FlatTrackFlags[type] & TRACK_ELEM_FLAG_ONLY_UNDERWATER) {
                if (!(gMapGroundFlags & ELEMENT_IS_UNDERWATER)) {
                    gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_UNDERWATER;
                    return MONEY32_UNDEFINED;
                }
            }
        }
        else {
            if (TrackFlags[type] & TRACK_ELEM_FLAG_ONLY_UNDERWATER) { // No element has this flag
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

        if ((rideTypeFlags & RIDE_TYPE_FLAG_TRACK_MUST_BE_ON_WATER) && !byte_9D8150) {
            mapElement = map_get_surface_element_at(x / 32, y / 32);

            uint8 water_height = map_get_water_height(mapElement) * 2;
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

        sint32 entranceDirections;
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE)) {
            entranceDirections = FlatRideTrackSequenceProperties[type][0];
        }
        else {
            entranceDirections = TrackSequenceProperties[type][0];
        }
        if ((entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN) && trackBlock->index == 0) {
            if (!track_add_station_element(x, y, baseZ, direction, rideIndex, 0)) {
                return MONEY32_UNDEFINED;
            }
        }
        //6c55be
        if (entranceDirections & TRACK_SEQUENCE_FLAG_CONNECTS_TO_PATH) {
            entranceDirections &= 0x0F;
            if (entranceDirections != 0) {
                if (!(flags & GAME_COMMAND_FLAG_APPLY) && !(flags & GAME_COMMAND_FLAG_GHOST) && !gCheatsDisableClearanceChecks) {
                    uint8 _bl = entranceDirections;
                    for (sint32 dl = bitscanforward(_bl); dl != -1; dl = bitscanforward(_bl)){
                        _bl &= ~(1 << dl);
                        sint32 temp_x = x, temp_y = y;
                        sint32 temp_direction = (direction + dl) & 3;
                        temp_x += TileDirectionDelta[temp_direction].x;
                        temp_y += TileDirectionDelta[temp_direction].y;
                        temp_direction ^= (1 << 1);
                        wall_remove_intersecting_walls(temp_x, temp_y, baseZ, clearanceZ, temp_direction & 3);
                    }
                }
            }
        }
        //6c5648 12 push
        mapElement = map_get_surface_element_at(x / 32, y / 32);
        if (!gCheatsDisableSupportLimits){
            sint32 ride_height = clearanceZ - mapElement->base_height;
            if (ride_height >= 0) {

                uint16 maxHeight;
                if (gConfigInterface.select_by_track_type) {
                    if (ride_type_has_ride_groups(ride->type)) {
                        const RideGroup * rideGroup = get_ride_group(ride->type, rideEntry);
                        maxHeight = rideGroup->MaximumHeight;
                    } else {
                        maxHeight = RideData5[ride->type].max_height;
                    }
                } else {
                    maxHeight = rideEntry->max_height;
                    // If a rideEntry specifies 0 as max height, it means OpenRCT2
                    // should fall back on the default for the track type.
                    if (maxHeight == 0) {
                        maxHeight = RideData5[ride->type].max_height;
                    }
                }

                ride_height /= 2;
                if (ride_height > maxHeight && !byte_9D8150) {
                    gGameCommandErrorText = STR_TOO_HIGH_FOR_SUPPORTS;
                    return MONEY32_UNDEFINED;
                }
            }
        }

        sint32 _support_height = baseZ - mapElement->base_height;
        if (_support_height < 0) {
            _support_height = 10;
        }

        cost += ((_support_height / 2) * RideTrackCosts[ride->type].support_price) * 5;

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
            ride->lifecycle_flags |= RIDE_LIFECYCLE_CABLE_LIFT_HILL_COMPONENT_USED;
            ride->cable_lift_x = x;
            ride->cable_lift_y = y;
            ride->cable_lift_z = baseZ;
            break;
        case TRACK_ELEM_BLOCK_BRAKES:
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
                if (!(liftHillAndAlternativeState & CONSTRUCTION_LIFT_HILL_SELECTED))
                    break;
                //Fall Through
            case TRACK_ELEM_CABLE_LIFT_HILL:
                ride->num_block_brakes++;
                break;
            }
        }

        entranceDirections = 0;
        if (ride->overall_view.xy != RCT_XY8_UNDEFINED){
            if (!(flags & GAME_COMMAND_FLAG_5)){
                if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE)) {
                    entranceDirections = FlatRideTrackSequenceProperties[type][0];
                }
                else {
                    entranceDirections = TrackSequenceProperties[type][0];
                }
            }
        }
        if (entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN || ride->overall_view.xy == RCT_XY8_UNDEFINED){
            ride->overall_view.x = x / 32;
            ride->overall_view.y = y / 32;
        }

        mapElement = map_element_insert(x / 32, y / 32, baseZ, bl & 0xF);
        assert(mapElement != NULL);
        mapElement->clearance_height = clearanceZ;

        uint8 map_type = direction;
        map_type |= MAP_ELEMENT_TYPE_TRACK;
        if (liftHillAndAlternativeState & CONSTRUCTION_LIFT_HILL_SELECTED){
            map_type |= (1 << 7);
        }
        mapElement->type = map_type;

        map_element_set_track_sequence(mapElement, trackBlock->index);
        mapElement->properties.track.ride_index = rideIndex;
        mapElement->properties.track.type = type;
        mapElement->properties.track.colour = 0;
        if (flags & GAME_COMMAND_FLAG_GHOST)
        {
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
        if (track_element_has_speed_setting(type))
        {
            map_element_set_brake_booster_speed(mapElement, brakeSpeed);
        }
        else
        {
            track_element_set_seat_rotation(mapElement, seatRotation);
        }

        if (liftHillAndAlternativeState & RIDE_TYPE_ALTERNATIVE_TRACK_TYPE)
        {
            track_element_set_inverted(mapElement, true);
        }
        track_element_set_colour_scheme(mapElement, colour);

        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE)) {
            entranceDirections = FlatRideTrackSequenceProperties[type][0];
        }
        else {
            entranceDirections = TrackSequenceProperties[type][0];
        }

        if (entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN) {
            if (trackBlock->index == 0) {
                track_add_station_element(x, y, baseZ, direction, rideIndex, GAME_COMMAND_FLAG_APPLY);
            }
            sub_6CB945(rideIndex);
            ride_update_max_vehicles(rideIndex);
        }

        if (rideTypeFlags & RIDE_TYPE_FLAG_TRACK_MUST_BE_ON_WATER){
            rct_map_element* surfaceElement = map_get_surface_element_at(x / 32, y / 32);
            surfaceElement->type |= (1 << 6);
            mapElement = surfaceElement;
        }

        if (!gCheatsDisableClearanceChecks || !(flags & GAME_COMMAND_FLAG_GHOST)) {
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
             FlatRideTrackPricing[type] :
             TrackPricing[type];

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
void game_command_place_track(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp)
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

        if ((map_element_get_direction(mapElement)) != rotation)
            continue;

        if (map_element_get_track_sequence(mapElement) != sequence)
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

    if (mapElement->flags & MAP_ELEMENT_FLAG_INDESTRUCTIBLE_TRACK_PIECE){
        gGameCommandErrorText = STR_YOU_ARE_NOT_ALLOWED_TO_REMOVE_THIS_SECTION;
        return MONEY32_UNDEFINED;
    }

    uint8 rideIndex = mapElement->properties.track.ride_index;
    type = mapElement->properties.track.type;
    bool isLiftHill = track_element_is_lift_hill(mapElement);

    Ride* ride = get_ride(rideIndex);
    const rct_preview_track* trackBlock = get_track_def_from_ride(ride, type);
    trackBlock += map_element_get_track_sequence(mapElement);

    uint8 originDirection = map_element_get_direction(mapElement);
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

        trackpieceZ = z;

        found = 0;
        mapElement = map_get_first_element_at(x / 32, y / 32);
        do{
            if (mapElement == NULL)
                break;

            if (mapElement->base_height != z / 8)
                continue;

            if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
                continue;

            if ((map_element_get_direction(mapElement)) != rotation)
                continue;

            if (map_element_get_track_sequence(mapElement) != trackBlock->index)
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

        sint32 entranceDirections;
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE)) {
            entranceDirections = FlatRideTrackSequenceProperties[type][0];
        }
        else {
            entranceDirections = TrackSequenceProperties[type][0];
        }

        if (entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN && (map_element_get_track_sequence(mapElement) == 0)){
            if (!track_remove_station_element(x, y, z / 8, rotation, rideIndex, 0)) {
                return MONEY32_UNDEFINED;
            }
        }

        rct_map_element* surfaceElement = map_get_surface_element_at(x / 32, y / 32);
        if (surfaceElement == NULL){
            return MONEY32_UNDEFINED;
        }

        sint8 _support_height = mapElement->base_height - surfaceElement->base_height;
        if (_support_height < 0){
            _support_height = 10;
        }

        cost += (_support_height / 2) * RideTrackCosts[ride->type].support_price;

        if (!(flags & GAME_COMMAND_FLAG_APPLY))
            continue;

        if (entranceDirections & (1 << 4) && (map_element_get_track_sequence(mapElement) == 0)){
            if (!track_remove_station_element(x, y, z / 8, rotation, rideIndex, GAME_COMMAND_FLAG_APPLY)) {
                return MONEY32_UNDEFINED;
            }
        }

        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_TRACK_MUST_BE_ON_WATER)){
            surfaceElement->type &= ~(1 << 6);
        }

        invalidate_test_results(rideIndex);
        footpath_queue_chain_reset();
        if (!gCheatsDisableClearanceChecks || !(mapElement->flags & MAP_ELEMENT_FLAG_GHOST)) {
            footpath_remove_edges_at(x, y, mapElement);
        }
        map_element_remove(mapElement);
        if (!(flags & GAME_COMMAND_FLAG_GHOST)){
            sub_6CB945(rideIndex);
            ride_update_max_vehicles(rideIndex);
        }
    }

    if (flags & GAME_COMMAND_FLAG_APPLY){
        switch (type){
        case TRACK_ELEM_ON_RIDE_PHOTO:
            ride->lifecycle_flags &= ~RIDE_LIFECYCLE_ON_RIDE_PHOTO;
            break;
        case TRACK_ELEM_CABLE_LIFT_HILL:
            ride->lifecycle_flags &= ~RIDE_LIFECYCLE_CABLE_LIFT_HILL_COMPONENT_USED;
            break;
        case TRACK_ELEM_BLOCK_BRAKES:
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
            if (!isLiftHill)
                break;
            // Fall through
        case TRACK_ELEM_CABLE_LIFT_HILL:
            ride->num_block_brakes--;
            break;
        }
    }

    money32 price = RideTrackCosts[ride->type].track_price;
    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE)) {
        price *= FlatRideTrackPricing[type];
    }
    else {
        price *= TrackPricing[type];
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
void game_command_remove_track(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp)
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

/** rct2: 0x00993CE9 */
static const uint8 byte_993CE9[] = {
    0xFF, 0xE0, 0xFF,
    14,  0,  1,  2,
     6,  2,  4,  5,
     9, 10,  6,  8,
    12, 13, 14, 10,
};

/** rct2: 0x00993CFC */
static const uint8 byte_993CFC[] = {
    5, 12, 0xFF, 0xFF, 9, 0, 0xFF, 0xFF, 13, 4, 0xFF, 0xFF, 1, 8, 0xFF, 0xFF
};

/** rct2: 0x00993D0C */
static const uint8 byte_993D0C[] = {
    3, 0, 0xFF, 0xFF, 0, 1, 0xFF, 0xFF, 1, 2, 0xFF, 0xFF, 2, 3, 0xFF, 0xFF
};

static money32 set_maze_track(uint16 x, uint8 flags, uint8 direction, uint16 y, uint8 rideIndex, uint8 mode, uint16 z) {
    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
    gCommandPosition.x = x + 8;
    gCommandPosition.y = y + 8;
    gCommandPosition.z = z;

    money32 cost = 0;

    if (!map_check_free_elements_and_reorganise(1)) {
        return MONEY32_UNDEFINED;
    }

    if (!(flags & GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED) && !gCheatsBuildInPauseMode && game_is_paused()) {
        gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
        return MONEY32_UNDEFINED;
    }

    if ((z & 0xF) != 0) {
        gGameCommandErrorText = STR_CONSTRUCTION_ERR_UNKNOWN;
        return MONEY32_UNDEFINED;
    }

    if ((flags & GAME_COMMAND_FLAG_APPLY) != 0) {
        if (!(flags & GAME_COMMAND_FLAG_GHOST) && !(flags & GAME_COMMAND_FLAG_2)) {
            footpath_remove_litter(x, y, z);
            wall_remove_at(floor2(x, 32), floor2(y, 32), z, z + 32);
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

    mapElement = map_get_track_element_at_of_type_from_ride(x, y, baseHeight, TRACK_ELEM_MAZE, rideIndex);
    if (mapElement == NULL) {
        if (mode != GC_SET_MAZE_TRACK_BUILD) {
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

        Ride *ride = get_ride(rideIndex);

        money32 price = (((RideTrackCosts[ride->type].track_price * TrackPricing[TRACK_ELEM_MAZE]) >> 16));
        cost = price / 2 * 10;

        if (!(flags & GAME_COMMAND_FLAG_APPLY)) {
            if (gParkFlags & PARK_FLAGS_NO_MONEY) {
                return 0;
            }

            return cost;
        }

        uint16 flooredX = floor2(x, 32);
        uint16 flooredY = floor2(y, 32);

        mapElement = map_element_insert(x / 32, y / 32, baseHeight, 0xF);
        assert(mapElement != NULL);
        mapElement->clearance_height = clearanceHeight;
        mapElement->type = MAP_ELEMENT_TYPE_TRACK;
        mapElement->properties.track.type = TRACK_ELEM_MAZE;
        mapElement->properties.track.ride_index = rideIndex;
        mapElement->properties.track.maze_entry = 0xFFFF;

        if (flags & GAME_COMMAND_FLAG_GHOST) {
            mapElement->flags |= MAP_ELEMENT_FLAG_GHOST;
        }

        map_invalidate_tile_full(flooredX, flooredY);

        ride->maze_tiles++;
        ride->station_heights[0] = mapElement->base_height;
        ride->station_starts[0].xy = 0;

        if (direction == 4) {
            if (!(flags & GAME_COMMAND_FLAG_GHOST)) {
                ride->overall_view.x = flooredX / 32;
                ride->overall_view.y = flooredY / 32;
            }
        }
    }

    if (!(flags & GAME_COMMAND_FLAG_APPLY)) {
        if (gParkFlags & PARK_FLAGS_NO_MONEY) {
            return 0;
        }

        return cost;
    }


    switch (mode) {
    case GC_SET_MAZE_TRACK_BUILD:
    {
        uint8 segmentOffset = maze_element_get_segment_bit(x, y);

        mapElement->properties.track.maze_entry &= ~(1 << segmentOffset);

        if (direction != 4) {
            segmentOffset = byte_993CE9[(direction + segmentOffset)];
            mapElement->properties.track.maze_entry &= ~(1 << segmentOffset);

            uint8 temp_edx = byte_993CFC[segmentOffset];
            if (temp_edx != 0xFF) {
                uint16 previousElementX = floor2(x, 32) - TileDirectionDelta[direction].x;
                uint16 previousElementY = floor2(y, 32) - TileDirectionDelta[direction].y;

                rct_map_element *previousMapElement = map_get_track_element_at_of_type_from_ride(previousElementX, previousElementY, baseHeight, TRACK_ELEM_MAZE, rideIndex);
                if (previousMapElement != NULL) {
                    previousMapElement->properties.track.maze_entry &= ~(1 << temp_edx);
                } else {
                    mapElement->properties.track.maze_entry |= (1 << segmentOffset);
                }
            }
        }

        break;
    }

    case GC_SET_MAZE_TRACK_MOVE:
        break;

    case GC_SET_MAZE_TRACK_FILL:
        if (direction != 4) {
            uint16 previousSegmentX = x - TileDirectionDelta[direction].x / 2;
            uint16 previousSegmentY = y - TileDirectionDelta[direction].y / 2;

            mapElement = map_get_track_element_at_of_type_from_ride(previousSegmentX, previousSegmentY, baseHeight, TRACK_ELEM_MAZE, rideIndex);
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

                uint32 direction1 = byte_993D0C[segmentBit];
                uint16 nextElementX = floor2(previousSegmentX, 32) + TileDirectionDelta[direction1].x;
                uint16 nextElementY = floor2(previousSegmentY, 32) + TileDirectionDelta[direction1].y;

                rct_map_element *tmp_mapElement = map_get_track_element_at_of_type_from_ride(nextElementX, nextElementY, baseHeight, TRACK_ELEM_MAZE, rideIndex);
                if (tmp_mapElement != NULL) {
                    uint8 edx11 = byte_993CFC[segmentBit];
                    tmp_mapElement->properties.track.maze_entry |= 1 << (edx11);
                }

                segmentBit--;
            } while ((segmentBit & 0x3) != 0x3);
        }
        break;
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

    return cost;
}

/**
 *
 *  rct2: 0x006CD8CE
 */
void game_command_set_maze_track(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp)
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
void game_command_set_brakes_speed(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp)
{
    rct_map_element *mapElement;
    sint32 x, y, z, trackType, brakesSpeed;

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

        map_element_set_brake_booster_speed(mapElement, brakesSpeed);

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
    sint32 z, direction;
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
    return trackElement->type & TRACK_ELEMENT_FLAG_CHAIN_LIFT;
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
    case TRACK_ELEM_BLOCK_BRAKES:
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

bool track_element_is_inverted(rct_map_element * mapElement)
{
    return mapElement->properties.track.colour & TRACK_ELEMENT_COLOUR_FLAG_INVERTED;
}

void track_element_set_inverted(rct_map_element * mapElement, bool inverted)
{
    if (inverted)
    {
        mapElement->properties.track.colour |= TRACK_ELEMENT_COLOUR_FLAG_INVERTED;
    }
    else
    {
        mapElement->properties.track.colour &= ~TRACK_ELEMENT_COLOUR_FLAG_INVERTED;
    }
}

sint32 track_get_actual_bank(rct_map_element *mapElement, sint32 bank)
{
    Ride *ride = get_ride(mapElement->properties.track.ride_index);
    sint32 trackColour = mapElement->properties.track.colour;
    return track_get_actual_bank_2(ride->type, trackColour, bank);
}

sint32 track_get_actual_bank_2(sint32 rideType, sint32 trackColour, sint32 bank)
{
    if (RideData4[rideType].flags & RIDE_TYPE_FLAG4_HAS_ALTERNATIVE_TRACK_TYPE) {
        if (trackColour & TRACK_ELEMENT_COLOUR_FLAG_INVERTED) {
            if (bank == TRACK_BANK_NONE) {
                bank = TRACK_BANK_UPSIDE_DOWN;
            } else if (bank == TRACK_BANK_UPSIDE_DOWN) {
                bank = TRACK_BANK_NONE;
            }
        }
    }
    return bank;
}

sint32 track_get_actual_bank_3(rct_vehicle *vehicle, rct_map_element *mapElement)
{
    uint8 colourThingToXor = (vehicle->update_flags >> 9) & 0xFF;
    sint32 trackType = mapElement->properties.track.type;
    sint32 rideType = get_ride(mapElement->properties.track.ride_index)->type;
    sint32 trackColour = mapElement->properties.track.colour ^ colourThingToXor;
    sint32 bankStart = TrackDefinitions[trackType].bank_start;
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

bool track_element_is_covered(sint32 trackElementType)
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

bool track_element_is_booster(uint8 rideType, uint8 trackType)
{
    // Boosters share their ID with the Spinning Control track.
    if (rideType != RIDE_TYPE_WILD_MOUSE && trackType == TRACK_ELEM_BOOSTER)
    {
        return true;
    }
    return false;
}

bool track_element_has_speed_setting(uint8 trackType)
{
    // This does not check if the element is really a Spinning Control track instead of a booster,
    // but this does not cause problems.
    if (trackType == TRACK_ELEM_BRAKES || trackType == TRACK_ELEM_BOOSTER)
    {
        return true;
    }
    return false;
}

uint8 track_element_get_seat_rotation(const rct_map_element * mapElement)
{
    return mapElement->properties.track.colour >> 4;
}

void track_element_set_seat_rotation(rct_map_element * mapElement, uint8 seatRotation)
{
    mapElement->properties.track.colour &= 0x0F;
    mapElement->properties.track.colour |= (seatRotation << 4);
}

uint8 track_element_get_colour_scheme(const rct_map_element * mapElement)
{
    return mapElement->properties.track.colour & 0x3;
}

void track_element_set_colour_scheme(rct_map_element * mapElement, uint8 colourScheme)
{
    mapElement->properties.track.colour &= ~0x3;
    mapElement->properties.track.colour |= (colourScheme & 0x3);
}
