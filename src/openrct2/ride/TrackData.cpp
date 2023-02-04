/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TrackData.h"

#include "../localisation/StringIds.h"
#include "Track.h"
#include "TrackPaint.h"

#include <cstdint>
#include <iterator>

// clang-format off
static constexpr TrackCoordinates _trackCoordinates[TrackElemType::Count] = {
        { 0, 0, 0, 0, 0, 0 },       // ELEM_FLAT
        { 0, 0, 0, 0, 0, 0 },       // ELEM_END_STATION
        { 0, 0, 0, 0, 0, 0 },       // ELEM_BEGIN_STATION
        { 0, 0, 0, 0, 0, 0 },       // ELEM_MIDDLE_STATION
        { 0, 0, 0, 16, 0, 0 },      // ELEM_25_DEG_UP
        { 0, 0, 0, 64, 0, 0 },      // ELEM_60_DEG_UP
        { 0, 0, 0, 8, 0, 0 },       // ELEM_FLAT_TO_25_DEG_UP
        { 0, 0, 0, 32, 0, 0 },      // ELEM_25_DEG_UP_TO_60_DEG_UP
        { 0, 0, 0, 32, 0, 0 },      // ELEM_60_DEG_UP_TO_25_DEG_UP
        { 0, 0, 0, 8, 0, 0 },       // ELEM_25_DEG_UP_TO_FLAT
        { 0, 0, 16, 0, 0, 0 },      // ELEM_25_DEG_DOWN
        { 0, 0, 64, 0, 0, 0 },      // ELEM_60_DEG_DOWN
        { 0, 0, 8, 0, 0, 0 },       // ELEM_FLAT_TO_25_DEG_DOWN
        { 0, 0, 32, 0, 0, 0 },      // ELEM_25_DEG_DOWN_TO_60_DEG_DOWN
        { 0, 0, 32, 0, 0, 0 },      // ELEM_60_DEG_DOWN_TO_25_DEG_DOWN
        { 0, 0, 8, 0, 0, 0 },       // ELEM_25_DEG_DOWN_TO_FLAT
        { 0, 3, 0, 0, -64, -64 },   // ELEM_LEFT_QUARTER_TURN_5_TILES
        { 0, 1, 0, 0, -64, 64 },    // ELEM_RIGHT_QUARTER_TURN_5_TILES
        { 0, 0, 0, 0, 0, 0 },       // ELEM_FLAT_TO_LEFT_BANK
        { 0, 0, 0, 0, 0, 0 },       // ELEM_FLAT_TO_RIGHT_BANK
        { 0, 0, 0, 0, 0, 0 },       // ELEM_LEFT_BANK_TO_FLAT
        { 0, 0, 0, 0, 0, 0 },       // ELEM_RIGHT_BANK_TO_FLAT
        { 0, 3, 0, 0, -64, -64 },   // ELEM_BANKED_LEFT_QUARTER_TURN_5_TILES
        { 0, 1, 0, 0, -64, 64 },    // ELEM_BANKED_RIGHT_QUARTER_TURN_5_TILES
        { 0, 0, 0, 8, 0, 0 },       // ELEM_LEFT_BANK_TO_25_DEG_UP
        { 0, 0, 0, 8, 0, 0 },       // ELEM_RIGHT_BANK_TO_25_DEG_UP
        { 0, 0, 0, 8, 0, 0 },       // ELEM_25_DEG_UP_TO_LEFT_BANK
        { 0, 0, 0, 8, 0, 0 },       // ELEM_25_DEG_UP_TO_RIGHT_BANK
        { 0, 0, 8, 0, 0, 0 },       // ELEM_LEFT_BANK_TO_25_DEG_DOWN
        { 0, 0, 8, 0, 0, 0 },       // ELEM_RIGHT_BANK_TO_25_DEG_DOWN
        { 0, 0, 8, 0, 0, 0 },       // ELEM_25_DEG_DOWN_TO_LEFT_BANK
        { 0, 0, 8, 0, 0, 0 },       // ELEM_25_DEG_DOWN_TO_RIGHT_BANK
        { 0, 0, 0, 0, 0, 0 },       // ELEM_LEFT_BANK
        { 0, 0, 0, 0, 0, 0 },       // ELEM_RIGHT_BANK
        { 0, 3, 0, 64, -64, -64 },  // ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_UP
        { 0, 1, 0, 64, -64, 64 },   // ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_UP
        { 0, 3, 64, 0, -64, -64 },  // ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_DOWN
        { 0, 1, 64, 0, -64, 64 },   // ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_DOWN
        { 0, 0, 0, 0, -64, -32 },   // ELEM_S_BEND_LEFT
        { 0, 0, 0, 0, -64, 32 },    // ELEM_S_BEND_RIGHT
        { 0, 0, 0, 0, -32, -32 },   // ELEM_LEFT_VERTICAL_LOOP
        { 0, 0, 0, 0, -32, 32 },    // ELEM_RIGHT_VERTICAL_LOOP
        { 0, 3, 0, 0, -32, -32 },   // ELEM_LEFT_QUARTER_TURN_3_TILES
        { 0, 1, 0, 0, -32, 32 },    // ELEM_RIGHT_QUARTER_TURN_3_TILES
        { 0, 3, 0, 0, -32, -32 },   // ELEM_LEFT_QUARTER_TURN_3_TILES_BANK
        { 0, 1, 0, 0, -32, 32 },    // ELEM_RIGHT_QUARTER_TURN_3_TILES_BANK
        { 0, 3, 0, 32, -32, -32 },  // ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP
        { 0, 1, 0, 32, -32, 32 },   // ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP
        { 0, 3, 32, 0, -32, -32 },  // ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN
        { 0, 1, 32, 0, -32, 32 },   // ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN
        { 0, 3, 0, 0, 0, 0 },       // ELEM_LEFT_QUARTER_TURN_1_TILE
        { 0, 1, 0, 0, 0, 0 },       // ELEM_RIGHT_QUARTER_TURN_1_TILE
        { 0, 0, 0, 16, -64, 0 },    // ELEM_LEFT_TWIST_DOWN_TO_UP
        { 0, 0, 0, 16, -64, 0 },    // ELEM_RIGHT_TWIST_DOWN_TO_UP
        { 0, 0, 0, -16, -64, 0 },   // ELEM_LEFT_TWIST_UP_TO_DOWN
        { 0, 0, 0, -16, -64, 0 },   // ELEM_RIGHT_TWIST_UP_TO_DOWN
        { 0, 2, 0, 152, -32, 0 },   // ELEM_HALF_LOOP_UP
        { 0, 2, 0, -152, 32, 0 },   // ELEM_HALF_LOOP_DOWN
        { 0, 3, 0, 80, -32, -32 },  // ELEM_LEFT_CORKSCREW_UP
        { 0, 1, 0, 80, -32, 32 },   // ELEM_RIGHT_CORKSCREW_UP
        { 0, 3, 0, -80, -32, -32 }, // ELEM_LEFT_CORKSCREW_DOWN
        { 0, 1, 0, -80, -32, 32 },  // ELEM_RIGHT_CORKSCREW_DOWN
        { 0, 0, 0, 24, 0, 0 },      // ELEM_FLAT_TO_60_DEG_UP
        { 0, 0, 0, 24, 0, 0 },      // ELEM_60_DEG_UP_TO_FLAT
        { 0, 0, 24, 0, 0, 0 },      // ELEM_FLAT_TO_60_DEG_DOWN
        { 0, 0, 24, 0, 0, 0 },      // ELEM_60_DEG_DOWN_TO_FLAT
        { 0, 0, 0, 96, 32, 0 },     // ELEM_TOWER_BASE
        { 0, 0, 0, 32, 32, 0 },     // ELEM_TOWER_SECTION
        { 0, 0, 0, 0, 0, 0 },       // ELEM_FLAT_COVERED
        { 0, 0, 0, 16, 0, 0 },      // ELEM_25_DEG_UP_COVERED
        { 0, 0, 0, 64, 0, 0 },      // ELEM_60_DEG_UP_COVERED
        { 0, 0, 0, 8, 0, 0 },       // ELEM_FLAT_TO_25_DEG_UP_COVERED
        { 0, 0, 0, 32, 0, 0 },      // ELEM_25_DEG_UP_TO_60_DEG_UP_COVERED
        { 0, 0, 0, 32, 0, 0 },      // ELEM_60_DEG_UP_TO_25_DEG_UP_COVERED
        { 0, 0, 0, 8, 0, 0 },       // ELEM_25_DEG_UP_TO_FLAT_COVERED
        { 0, 0, 16, 0, 0, 0 },      // ELEM_25_DEG_DOWN_COVERED
        { 0, 0, 64, 0, 0, 0 },      // ELEM_60_DEG_DOWN_COVERED
        { 0, 0, 8, 0, 0, 0 },       // ELEM_FLAT_TO_25_DEG_DOWN_COVERED
        { 0, 0, 32, 0, 0, 0 },      // ELEM_25_DEG_DOWN_TO_60_DEG_DOWN_COVERED
        { 0, 0, 32, 0, 0, 0 },      // ELEM_60_DEG_DOWN_TO_25_DEG_DOWN_COVERED
        { 0, 0, 8, 0, 0, 0 },       // ELEM_25_DEG_DOWN_TO_FLAT_COVERED
        { 0, 3, 0, 0, -64, -64 },   // ELEM_LEFT_QUARTER_TURN_5_TILES_COVERED
        { 0, 1, 0, 0, -64, 64 },    // ELEM_RIGHT_QUARTER_TURN_5_TILES_COVERED
        { 0, 0, 0, 0, -64, -32 },   // ELEM_S_BEND_LEFT_COVERED
        { 0, 0, 0, 0, -64, 32 },    // ELEM_S_BEND_RIGHT_COVERED
        { 0, 3, 0, 0, -32, -32 },   // ELEM_LEFT_QUARTER_TURN_3_TILES_COVERED
        { 0, 1, 0, 0, -32, 32 },    // ELEM_RIGHT_QUARTER_TURN_3_TILES_COVERED
        { 0, 2, 0, 16, 0, -96 },    // ELEM_LEFT_HALF_BANKED_HELIX_UP_SMALL
        { 0, 2, 0, 16, 0, 96 },     // ELEM_RIGHT_HALF_BANKED_HELIX_UP_SMALL
        { 0, 2, 16, 0, 0, -96 },    // ELEM_LEFT_HALF_BANKED_HELIX_DOWN_SMALL
        { 0, 2, 16, 0, 0, 96 },     // ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_SMALL
        { 0, 2, 0, 16, 0, -160 },   // ELEM_LEFT_HALF_BANKED_HELIX_UP_LARGE
        { 0, 2, 0, 16, 0, 160 },    // ELEM_RIGHT_HALF_BANKED_HELIX_UP_LARGE
        { 0, 2, 16, 0, 0, -160 },   // ELEM_LEFT_HALF_BANKED_HELIX_DOWN_LARGE
        { 0, 2, 16, 0, 0, 160 },    // ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_LARGE
        { 0, 3, 0, 64, 0, 0 },      // ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_UP
        { 0, 1, 0, 64, 0, 0 },      // ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_UP
        { 0, 3, 64, 0, 0, 0 },      // ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_DOWN
        { 0, 1, 64, 0, 0, 0 },      // ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_DOWN
        { 0, 0, 0, 0, 0, 0 },       // ELEM_BRAKES
        { 0, 0, 0, 0, 0, 0 },       // ELEM_ROTATION_CONTROL_TOGGLE
        { 0, 0, 0, 0, 0, 0 },       // ELEM_INVERTED_90_DEG_UP_TO_FLAT_QUARTER_LOOP
        { 0, 3, 0, 16, -64, -64 },  // ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_UP
        { 0, 1, 0, 16, -64, 64 },   // ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_UP
        { 0, 3, 16, 0, -64, -64 },  // ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_DOWN
        { 0, 1, 16, 0, -64, 64 },   // ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_DOWN
        { 0, 3, 0, 16, -64, -64 },  // ELEM_LEFT_QUARTER_HELIX_LARGE_UP
        { 0, 1, 0, 16, -64, 64 },   // ELEM_RIGHT_QUARTER_HELIX_LARGE_UP
        { 0, 3, 16, 0, -64, -64 },  // ELEM_LEFT_QUARTER_HELIX_LARGE_DOWN
        { 0, 1, 16, 0, -64, 64 },   // ELEM_RIGHT_QUARTER_HELIX_LARGE_DOWN
        { 0, 0, 0, 16, 0, 0 },      // ELEM_25_DEG_UP_LEFT_BANKED
        { 0, 0, 0, 16, 0, 0 },      // ELEM_25_DEG_UP_RIGHT_BANKED
        { 0, 0, 0, 0, 0, 0 },       // ELEM_WATERFALL
        { 0, 0, 0, 0, 0, 0 },       // ELEM_RAPIDS
        { 0, 0, 0, 0, 0, 0 },       // ELEM_ON_RIDE_PHOTO
        { 0, 0, 16, 0, 0, 0 },      // ELEM_25_DEG_DOWN_LEFT_BANKED
        { 0, 0, 16, 0, 0, 0 },      // ELEM_25_DEG_DOWN_RIGHT_BANKED
        { 0, 0, 16, 16, -128, 0 },  // ELEM_WATER_SPLASH
        { 0, 0, 0, 88, -96, 0 },    // ELEM_FLAT_TO_60_DEG_UP_LONG_BASE
        { 0, 0, 0, 88, -96, 0 },    // ELEM_60_DEG_UP_TO_FLAT_LONG_BASE
        { 0, 0, 0, 0, 0, 0 },       // ELEM_WHIRLPOOL
        { 0, 0, 88, 0, -96, 0 },    // ELEM_60_DEG_DOWN_TO_FLAT_LONG_BASE
        { 0, 0, 88, 0, -96, 0 },    // ELEM_FLAT_TO_60_DEG_DOWN_LONG_BASE
        { 0, 0, 0, -96, -96, 0 },   // ELEM_CABLE_LIFT_HILL
        { 0, 0, 0, 240, -160, 0 },  // ELEM_REVERSE_FREEFALL_SLOPE
        { 0, 0, 0, 80, 32, 0 },     // ELEM_REVERSE_FREEFALL_VERTICAL
        { 0, 0, 0, 32, 32, 0 },     // ELEM_90_DEG_UP
        { 0, 0, 32, 0, 32, 0 },     // ELEM_90_DEG_DOWN
        { 0, 0, 0, 56, 32, 0 },     // ELEM_60_DEG_UP_TO_90_DEG_UP
        { 0, 0, 56, 0, 0, 0 },      // ELEM_90_DEG_DOWN_TO_60_DEG_DOWN
        { 0, 0, 0, 56, 0, 0 },      // ELEM_90_DEG_UP_TO_60_DEG_UP
        { 0, 0, 56, 0, 32, 0 },     // ELEM_60_DEG_DOWN_TO_90_DEG_DOWN
        { 0, 0, 24, 0, 0, 0 },      // ELEM_BRAKE_FOR_DROP
        { 0, 7, 0, 0, -64, -32 },   // ELEM_LEFT_EIGHTH_TO_DIAG
        { 0, 4, 0, 0, -64, 32 },    // ELEM_RIGHT_EIGHTH_TO_DIAG
        { 4, 0, 0, 0, -64, 32 },    // ELEM_LEFT_EIGHTH_TO_ORTHOGONAL
        { 4, 1, 0, 0, -32, 64 },    // ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL
        { 0, 7, 0, 0, -64, -32 },   // ELEM_LEFT_EIGHTH_BANK_TO_DIAG
        { 0, 4, 0, 0, -64, 32 },    // ELEM_RIGHT_EIGHTH_BANK_TO_DIAG
        { 4, 0, 0, 0, -64, 32 },    // ELEM_LEFT_EIGHTH_BANK_TO_ORTHOGONAL
        { 4, 1, 0, 0, -32, 64 },    // ELEM_RIGHT_EIGHTH_BANK_TO_ORTHOGONAL
        { 4, 4, 0, 0, -32, 32 },    // ELEM_DIAG_FLAT
        { 4, 4, 0, 16, -32, 32 },   // ELEM_DIAG_25_DEG_UP
        { 4, 4, 0, 64, -32, 32 },   // ELEM_DIAG_60_DEG_UP
        { 4, 4, 0, 8, -32, 32 },    // ELEM_DIAG_FLAT_TO_25_DEG_UP
        { 4, 4, 0, 32, -32, 32 },   // ELEM_DIAG_25_DEG_UP_TO_60_DEG_UP
        { 4, 4, 0, 32, -32, 32 },   // ELEM_DIAG_60_DEG_UP_TO_25_DEG_UP
        { 4, 4, 0, 8, -32, 32 },    // ELEM_DIAG_25_DEG_UP_TO_FLAT
        { 4, 4, 16, 0, -32, 32 },   // ELEM_DIAG_25_DEG_DOWN
        { 4, 4, 64, 0, -32, 32 },   // ELEM_DIAG_60_DEG_DOWN
        { 4, 4, 8, 0, -32, 32 },    // ELEM_DIAG_FLAT_TO_25_DEG_DOWN
        { 4, 4, 32, 0, -32, 32 },   // ELEM_DIAG_25_DEG_DOWN_TO_60_DEG_DOWN
        { 4, 4, 32, 0, -32, 32 },   // ELEM_DIAG_60_DEG_DOWN_TO_25_DEG_DOWN
        { 4, 4, 8, 0, -32, 32 },    // ELEM_DIAG_25_DEG_DOWN_TO_FLAT
        { 4, 4, 0, 24, -32, 32 },   // ELEM_DIAG_FLAT_TO_60_DEG_UP
        { 4, 4, 0, 24, -32, 32 },   // ELEM_DIAG_60_DEG_UP_TO_FLAT
        { 4, 4, 24, 0, -32, 32 },   // ELEM_DIAG_FLAT_TO_60_DEG_DOWN
        { 4, 4, 24, 0, -32, 32 },   // ELEM_DIAG_60_DEG_DOWN_TO_FLAT
        { 4, 4, 0, 0, -32, 32 },    // ELEM_DIAG_FLAT_TO_LEFT_BANK
        { 4, 4, 0, 0, -32, 32 },    // ELEM_DIAG_FLAT_TO_RIGHT_BANK
        { 4, 4, 0, 0, -32, 32 },    // ELEM_DIAG_LEFT_BANK_TO_FLAT
        { 4, 4, 0, 0, -32, 32 },    // ELEM_DIAG_RIGHT_BANK_TO_FLAT
        { 4, 4, 0, 8, -32, 32 },    // ELEM_DIAG_LEFT_BANK_TO_25_DEG_UP
        { 4, 4, 0, 8, -32, 32 },    // ELEM_DIAG_RIGHT_BANK_TO_25_DEG_UP
        { 4, 4, 0, 8, -32, 32 },    // ELEM_DIAG_25_DEG_UP_TO_LEFT_BANK
        { 4, 4, 0, 8, -32, 32 },    // ELEM_DIAG_25_DEG_UP_TO_RIGHT_BANK
        { 4, 4, 8, 0, -32, 32 },    // ELEM_DIAG_LEFT_BANK_TO_25_DEG_DOWN
        { 4, 4, 8, 0, -32, 32 },    // ELEM_DIAG_RIGHT_BANK_TO_25_DEG_DOWN
        { 4, 4, 8, 0, -32, 32 },    // ELEM_DIAG_25_DEG_DOWN_TO_LEFT_BANK
        { 4, 4, 8, 0, -32, 32 },    // ELEM_DIAG_25_DEG_DOWN_TO_RIGHT_BANK
        { 4, 4, 0, 0, -32, 32 },    // ELEM_DIAG_LEFT_BANK
        { 4, 4, 0, 0, -32, 32 },    // ELEM_DIAG_RIGHT_BANK
        { 0, 0, 0, 0, 0, 0 },       // ELEM_LOG_FLUME_REVERSER
        { 0, 0, 0, 0, 0, 0 },       // ELEM_SPINNING_TUNNEL
        { 0, 0, 0, 32, -64, 0 },    // ELEM_LEFT_BARREL_ROLL_UP_TO_DOWN
        { 0, 0, 0, 32, -64, 0 },    // ELEM_RIGHT_BARREL_ROLL_UP_TO_DOWN
        { 0, 0, 0, -32, -64, 0 },   // ELEM_LEFT_BARREL_ROLL_DOWN_TO_UP
        { 0, 0, 0, -32, -64, 0 },   // ELEM_RIGHT_BARREL_ROLL_DOWN_TO_UP
        { 0, 3, 0, 24, -32, -32 },  // ELEM_LEFT_BANK_TO_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP
        { 0, 1, 0, 24, -32, 32 },   // ELEM_RIGHT_BANK_TO_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP
        { 0, 3, 24, 0, -32, -32 },  // ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_LEFT_BANK
        { 0, 1, 24, 0, -32, 32 },   // ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_RIGHT_BANK
        { 0, 0, 0, 16, 0, 0 },      // ELEM_POWERED_LIFT
        { 0, 2, 0, 280, -64, -32 }, // ELEM_LEFT_LARGE_HALF_LOOP_UP
        { 0, 2, 0, 280, -64, 32 },  // ELEM_RIGHT_LARGE_HALF_LOOP_UP
        { 0, 2, 0, -280, 64, -32 }, // ELEM_RIGHT_LARGE_HALF_LOOP_DOWN
        { 0, 2, 0, -280, 64, 32 },  // ELEM_LEFT_LARGE_HALF_LOOP_DOWN
        { 0, 0, 0, -16, -64, 0 },   // ELEM_LEFT_FLYER_TWIST_UP_TO_DOWN
        { 0, 0, 0, -16, -64, 0 },   // ELEM_RIGHT_FLYER_TWIST_UP_TO_DOWN
        { 0, 0, 0, 16, -64, 0 },    // ELEM_LEFT_FLYER_TWIST_DOWN_TO_UP
        { 0, 0, 0, 16, -64, 0 },    // ELEM_RIGHT_FLYER_TWIST_DOWN_TO_UP
        { 0, 2, 0, 120, -32, 0 },   // ELEM_FLYER_HALF_LOOP_UP
        { 0, 2, 0, -120, 32, 0 },   // ELEM_FLYER_HALF_LOOP_DOWN
        { 0, 3, 0, 48, -32, -32 },  // ELEM_LEFT_FLY_CORKSCREW_UP_TO_DOWN
        { 0, 1, 0, 48, -32, 32 },   // ELEM_RIGHT_FLY_CORKSCREW_UP_TO_DOWN
        { 0, 3, 0, -48, -32, -32 }, // ELEM_LEFT_FLY_CORKSCREW_DOWN_TO_UP
        { 0, 1, 0, -48, -32, 32 },  // ELEM_RIGHT_FLY_CORKSCREW_DOWN_TO_UP
        { 0, 2, 0, 32, 0, 0 },      // ELEM_HEARTLINE_TRANSFER_UP
        { 0, 2, 0, -32, 0, 0 },     // ELEM_HEARTLINE_TRANSFER_DOWN
        { 0, 0, 0, 0, -160, 0 },    // ELEM_LEFT_HEARTLINE_ROLL
        { 0, 0, 0, 0, -160, 0 },    // ELEM_RIGHT_HEARTLINE_ROLL
        { 0, 0, 0, 0, -32, 0 },     // ELEM_MINI_GOLF_HOLE_A
        { 0, 0, 0, 0, -32, 0 },     // ELEM_MINI_GOLF_HOLE_B
        { 0, 0, 0, 0, -32, 0 },     // ELEM_MINI_GOLF_HOLE_C
        { 0, 1, 0, 0, -32, 32 },    // ELEM_MINI_GOLF_HOLE_D
        { 0, 3, 0, 0, -32, -32 },   // ELEM_MINI_GOLF_HOLE_E
        { 0, 2, 0, -96, -96, 0 },   // ELEM_INVERTED_FLAT_TO_90_DEG_DOWN_QUARTER_LOOP
        { 0, 2, 0, 128, 64, 0 },    // ELEM_90_DEG_UP_QUARTER_LOOP_TO_INVERTED
        { 0, 2, 0, -128, -96, 0 },  // ELEM_QUARTER_LOOP_INVERT_TO_90_DEG_DOWN
        { 0, 3, 0, 16, -32, -32 },  // ELEM_LEFT_CURVED_LIFT_HILL
        { 0, 1, 0, 16, -32, 32 },   // ELEM_RIGHT_CURVED_LIFT_HILL
        { 0, 0, 0, 0, -64, 0 },     // ELEM_LEFT_REVERSER
        { 0, 0, 0, 0, -64, 0 },     // ELEM_RIGHT_REVERSER
        { 0, 0, 0, 0, -32, 0 },     // ELEM_AIR_THRUST_TOP_CAP
        { 0, 0, 80, 0, 32, 0 },     // ELEM_AIR_THRUST_VERTICAL_DOWN
        { 0, 0, 240, 0, -160, 0 },  // ELEM_AIR_THRUST_VERTICAL_DOWN_TO_LEVEL
        { 0, 0, 0, 0, 0, 0 },       // ELEM_BLOCK_BRAKES
        { 0, 3, 0, 32, -32, -32 },  // ELEM_BANKED_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP
        { 0, 1, 0, 32, -32, 32 },   // ELEM_BANKED_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP
        { 0, 3, 32, 0, -32, -32 },
        { 0, 1, 32, 0, -32, 32 },
        { 0, 3, 0, 64, -64, -64 },
        { 0, 1, 0, 64, -64, 64 },
        { 0, 3, 64, 0, -64, -64 },
        { 0, 1, 64, 0, -64, 64 },
        { 0, 0, 0, 16, 0, 0 },
        { 0, 0, 0, 16, 0, 0 },
        { 0, 0, 0, 16, 0, 0 },
        { 0, 0, 0, 16, 0, 0 },
        { 0, 0, 16, 0, 0, 0 },
        { 0, 0, 16, 0, 0, 0 },
        { 0, 0, 16, 0, 0, 0 },
        { 0, 0, 16, 0, 0, 0 },
        { 0, 0, 0, 8, 0, 0 },
        { 0, 0, 0, 8, 0, 0 },
        { 0, 0, 0, 8, 0, 0 },
        { 0, 0, 0, 8, 0, 0 },
        { 0, 0, 8, 0, 0, 0 },
        { 0, 0, 8, 0, 0, 0 },
        { 0, 0, 8, 0, 0, 0 },
        { 0, 0, 8, 0, 0, 0 },
        { 0, 0, 0, 8, 0, 0 },
        { 0, 0, 0, 8, 0, 0 },
        { 0, 0, 0, 8, 0, 0 },
        { 0, 0, 0, 8, 0, 0 },
        { 0, 0, 8, 0, 0, 0 },
        { 0, 0, 8, 0, 0, 0 },
        { 0, 0, 8, 0, 0, 0 },
        { 0, 0, 8, 0, 0, 0 },
        { 0, 3, 0, 96, 0, 32 },
        { 0, 1, 0, 96, 0, -32 },
        { 0, 3, 96, 0, 0, 32 },
        { 0, 1, 96, 0, 0, -32 },
        { 0, 2, 0, 96, 64, 0 },
        { 0, 2, 0, -128, -96, 0 },
        { 0, 2, 0, 128, 64, 0 },
        { 0, 0, 0, 0, 0, 0 },
        {    0,    2,    0,    0,    0,   32 }, // TrackElemType::FlatTrack1x4A
        {    0,    2,    0,    0,    0,   32 }, // TrackElemType::FlatTrack2x2
        {    0,    2,    0,    0,    0,   32 }, // TrackElemType::FlatTrack4x4
        {    0,    2,    0,    0,    0,   32 }, // TrackElemType::FlatTrack2x4
        {    0,    2,    0,    0,    0,   32 }, // TrackElemType::FlatTrack1x5
        {    0,    2,    0,    0,    0,   32 }, // TrackElemType::FlatTrack1x1A
        {    0,    2,    0,    0,    0,   32 }, // TrackElemType::FlatTrack1x4B
        {    0,    2,    0,    0,    0,   32 }, // TrackElemType::FlatTrack1x1B
        {    0,    2,    0,    0,    0,   32 }, // TrackElemType::FlatTrack1x4C
        {    0,    0,    0,   96,   32,    0 }, // TrackElemType::FlatTrack3x3
        { 0, 3, 0, 112, -64, -64 },  // TrackElemType::LeftCorkscrewUp
        { 0, 1, 0, 112, -64, 64 },   // TrackElemType::RightCorkscrewUp
        { 0, 3, 0, -112, -64, -64 }, // TrackElemType::LeftCorkscrewDown
        { 0, 1, 0, -112, -64, 64 },  // TrackElemType::RightCorkscrewDown
        { 0, 2, 0, 216, -32, -32 },  // TrackElemType::LeftMediumHalfLoopUp
        { 0, 2, 0, 216, -32, 32 },   // TrackElemType::RightMediumHalfLoopUp
        { 0, 2, 0, -216, 32, -32 },  // TrackElemType::RightMediumHalfLoopDown
        { 0, 2, 0, -216, 32, 32 },   // TrackElemType::LeftMediumHalfLoopDown
        { 0, 0, 0, 56, -64, 0 },     // TrackElemType::LeftZeroGRollUp
        { 0, 0, 0, 56, -64, 0 },     // TrackElemType::RightZeroGRollUp
        { 0, 0, 0, -56,-64, 0 },     // TrackElemType::RightZeroGRollDown
        { 0, 0, 0, -56,-64, 0 },     // TrackElemType::LeftZeroGRollDown
        { 0, 0, 0, 152, -96, 0 },    // TrackElemType::LeftLargeZeroGRollUp
        { 0, 0, 0, 152, -96, 0 },    // TrackElemType::RightLargeZeroGRollUp
        { 0, 0, 0, -152,-96, 0 },    // TrackElemType::RightLargeZeroGRollDown
        { 0, 0, 0, -152,-96, 0 },    // TrackElemType::LeftLargeZeroGRollDown
        { 0, 2, 0, 248, -64, -32 },  // TrackElemType::LeftFlyerLargeHalfLoopUninvertedUp
        { 0, 2, 0, 248, -64, 32 },   // TrackElemType::RightFlyerLargeHalfLoopUninvertedUp
        { 0, 2, 0, -248, 64, -32 },  // TrackElemType::RightFlyerLargeHalfLoopInvertedDown
        { 0, 2, 0, -248, 64, 32 },   // TrackElemType::LeftFlyerLargeHalfLoopInvertedDown
        { 0, 2, 0, 280, -64, -32 },  // TrackElemType::LeftFlyerLargeHalfLoopInvertedUp
        { 0, 2, 0, 280, -64, 32 },   // TrackElemType::RightFlyerLargeHalfLoopInvertedUp
        { 0, 2, 0, -280, 64, -32 },  // TrackElemType::RightFlyerLargeHalfLoopUninvertedDown
        { 0, 2, 0, -280, 64, 32 },   // TrackElemType::LeftFlyerLargeHalfLoopUninvertedDown
        { 0, 2, 0, 152, -32, 0 },    // TrackElemType::FlyerHalfLoopInvertedUp
        { 0, 2, 0, -152, 32, 0 },    // TrackElemType::FlyerHalfLoopUninvertedDown
};

/** rct2: 0x0099BA64 */
static constexpr uint8_t TrackSequenceProperties[][MaxSequencesPerPiece] = {
    { 0 },
    /* TrackElemType::EndStation */    { TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN | TRACK_SEQUENCE_FLAG_DISALLOW_DOORS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /* TrackElemType::BeginStation */  { TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN | TRACK_SEQUENCE_FLAG_DISALLOW_DOORS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /* TrackElemType::MiddleStation */ { TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN | TRACK_SEQUENCE_FLAG_DISALLOW_DOORS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    /* TrackElemType::TowerBase */ { TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_3, TRACK_SEQUENCE_FLAG_DIRECTION_0, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1, TRACK_SEQUENCE_FLAG_DIRECTION_3, TRACK_SEQUENCE_FLAG_DIRECTION_1, TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3, TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2, TRACK_SEQUENCE_FLAG_DIRECTION_2, 0, 0, 0, 0, 0, 0, 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    /* TrackElemType::Maze */   { TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    /* TrackElemType::SpinningTunnel */    { TRACK_SEQUENCE_FLAG_DISALLOW_DOORS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    /* TrackElemType::FlatTrack1x4A    */ { TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, 0, TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /* TrackElemType::FlatTrack2x2     */ { TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1, TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3, TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /* TrackElemType::FlatTrack4x4     */ { TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0, TRACK_SEQUENCE_FLAG_DIRECTION_0, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1, TRACK_SEQUENCE_FLAG_DIRECTION_3, 0, 0, TRACK_SEQUENCE_FLAG_DIRECTION_1, TRACK_SEQUENCE_FLAG_DIRECTION_3, 0, 0, TRACK_SEQUENCE_FLAG_DIRECTION_1, TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3, TRACK_SEQUENCE_FLAG_DIRECTION_2, TRACK_SEQUENCE_FLAG_DIRECTION_2, TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 },
    /* TrackElemType::FlatTrack2x4     */ { TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0, TRACK_SEQUENCE_FLAG_DIRECTION_0, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1, TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3, TRACK_SEQUENCE_FLAG_DIRECTION_2, TRACK_SEQUENCE_FLAG_DIRECTION_2, TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2, 0, 0, 0, 0, 0, 0, 0, 0 },
    /* TrackElemType::FlatTrack1x5     */ { TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, 0, TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3, TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /* TrackElemType::FlatTrack1x1A    */ { TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_ORIGIN | TRACK_SEQUENCE_FLAG_CONNECTS_TO_PATH, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /* TrackElemType::FlatTrack1x4B    */ { TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_ORIGIN, 0, TRACK_SEQUENCE_FLAG_DIRECTION_1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /* TrackElemType::FlatTrack1x1B    */ { TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN | TRACK_SEQUENCE_FLAG_CONNECTS_TO_PATH, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /* TrackElemType::FlatTrack1x4C    */ { TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3, TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3, TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /* TrackElemType::FlatTrack3x3     */ { TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_3, TRACK_SEQUENCE_FLAG_DIRECTION_0, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1, TRACK_SEQUENCE_FLAG_DIRECTION_3, TRACK_SEQUENCE_FLAG_DIRECTION_1, TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3, TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2, TRACK_SEQUENCE_FLAG_DIRECTION_2, 0, 0, 0, 0, 0, 0, 0 },
    { 0 }, // TrackElemType::LeftCorkscrewUp
    { 0 }, // TrackElemType::RightCorkscrewUp
    { 0 }, // TrackElemType::LeftCorkscrewDown
    { 0 }, // TrackElemType::RightCorkscrewDown
    { 0 }, // TrackElemType::LeftMediumHalfLoopUp
    { 0 }, // TrackElemType::RightMediumHalfLoopUp
    { 0 }, // TrackElemType::RightMediumHalfLoopDown
    { 0 }, // TrackElemType::LeftMediumHalfLoopDown
    { 0 }, // TrackElemType::LeftZeroGRollUp
    { 0 }, // TrackElemType::RightZeroGRollUp
    { 0 }, // TrackElemType::RightZeroGRollDown
    { 0 }, // TrackElemType::LeftZeroGRollDown
    { 0 }, // TrackElemType::LeftLargeZeroGRollUp
    { 0 }, // TrackElemType::RightLargeZeroGRollUp
    { 0 }, // TrackElemType::RightLargeZeroGRollDown
    { 0 }, // TrackElemType::LeftLargeZeroGRollDown

    /* LeftFlyerLargeHalfLoopUninvertedUp    */ { 0 },
    /* RightFlyerLargeHalfLoopUninvertedUp   */ { 0 },
    /* RightFlyerLargeHalfLoopInvertedDown   */ { 0 },
    /* LeftFlyerLargeHalfLoopInvertedDown    */ { 0 },
    /* LeftFlyerLargeHalfLoopInvertedUp      */ { 0 },
    /* RightFlyerLargeHalfLoopInvertedUp     */ { 0 },
    /* RightFlyerLargeHalfLoopUninvertedDown */ { 0 },
    /* LeftFlyerLargeHalfLoopUninvertedDown  */ { 0 },
    /* FlyerHalfLoopInvertedUp               */ { 0 },
    /* FlyerHalfLoopUninvertedUp             */ { 0 },
};

static constexpr PreviewTrack TrackBlocks000[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks001[] = {
    { 0, 0, 0, 0, 0,{ 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks002[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks003[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks004[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks005[] = {
    { 0, 0, 0, 0, 64, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks006[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks007[] = {
    { 0, 0, 0, 0, 32, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks008[] = {
    { 0, 0, 0, 0, 32, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks009[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks010[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks011[] = {
    { 0, 0, 0, 0, 64, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks012[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks013[] = {
    { 0, 0, 0, 0, 32, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks014[] = {
    { 0, 0, 0, 0, 32, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks015[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks016[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0111, 0 }, 0 },
    { 3, -32, -32, 0, 0, { 0b1101, 0 }, 0 },
    { 4, -32, -64, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, -32, 0, 0, { 0b0111, 0 }, 0 },
    { 6, -64, -64, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks017[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b1011, 0 }, 0 },
    { 3, -32, 32, 0, 0, { 0b1110, 0 }, 0 },
    { 4, -32, 64, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, 32, 0, 0, { 0b1011, 0 }, 0 },
    { 6, -64, 64, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks018[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks019[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks020[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks021[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks022[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0111, 0 }, 0 },
    { 3, -32, -32, 0, 0, { 0b1101, 0 }, 0 },
    { 4, -32, -64, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, -32, 0, 0, { 0b0111, 0 }, 0 },
    { 6, -64, -64, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks023[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b1011, 0 }, 0 },
    { 3, -32, 32, 0, 0, { 0b1110, 0 }, 0 },
    { 4, -32, 64, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, 32, 0, 0, { 0b1011, 0 }, 0 },
    { 6, -64, 64, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks024[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks025[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks026[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks027[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks028[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks029[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks030[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks031[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks032[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks033[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks034[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
    { 1, 0, -32, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 16, 16, { 0b0111, 0b0100 }, 0 },
    { 3, -32, -32, 24, 16, { 0b1101, 0 }, 0 },
    { 4, -32, -64, 48, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, -32, 32, 16, { 0b0111, 0b0100 }, 0 },
    { 6, -64, -64, 48, 16, { 0b1111, 0b0110 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks035[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
    { 1, 0, 32, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 16, 16, { 0b1011, 0b1000 }, 0 },
    { 3, -32, 32, 24, 16, { 0b1110, 0 }, 0 },
    { 4, -32, 64, 48, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, 32, 32, 16, { 0b1011, 0b1000 }, 0 },
    { 6, -64, 64, 48, 16, { 0b1111, 0b1001 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks036[] = {
    { 0, 0, 0, 48, 16, { 0b1111, 0b0011 }, 0 },
    { 1, 0, -32, 48, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 32, 16, { 0b0111, 0b0001 }, 0 },
    { 3, -32, -32, 24, 16, { 0b1101, 0 }, 0 },
    { 4, -32, -64, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, -32, 16, 16, { 0b0111, 0b0001 }, 0 },
    { 6, -64, -64, 0, 16, { 0b1111, 0b1001 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks037[] = {
    { 0, 0, 0, 48, 16, { 0b1111, 0b0011 }, 0 },
    { 1, 0, 32, 48, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 32, 16, { 0b1011, 0b0010 }, 0 },
    { 3, -32, 32, 24, 16, { 0b1110, 0 }, 0 },
    { 4, -32, 64, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, 32, 16, 16, { 0b1011, 0b0010 }, 0 },
    { 6, -64, 64, 0, 16, { 0b1111, 0b0110 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks038[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 0, { 0b0111, 0 }, 0 },
    { 2, -32, -32, 0, 0, { 0b1101, 0 }, 0 },
    { 3, -64, -32, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks039[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 0, { 0b1011, 0 }, 0 },
    { 2, -32, 32, 0, 0, { 0b1110, 0 }, 0 },
    { 3, -64, 32, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks040[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
    { 1, -32, 0, 16, 16, { 0b1111, 0b1100 }, 0 },
    { 2, -64, 0, 32, 96, { 0b0010, 0 }, 0 },
    { 3, -32, 0, 120, 16, { 0b0110, 0 }, 0 },
    { 4, -32, -32, 120, 0, { 0b0000, 0 }, 0 },
    { 5, 0, 0, 120, 0, { 0b0000, 0 }, 0 },
    { 6, 0, -32, 120, 16, { 0b1001, 0 }, 0 },
    { 7, 32, -32, 32, 96, { 0b1000, 0 }, 0 },
    { 8, 0, -32, 16, 16, { 0b1111, 0b0011 }, 0 },
    { 9, -32, -32, 0, 16, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks041[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
    { 1, -32, 0, 16, 16, { 0b1111, 0b1100 }, 0 },
    { 2, -64, 0, 32, 96, { 0b0001, 0 }, 0 },
    { 3, -32, 0, 120, 16, { 0b1001, 0 }, 0 },
    { 4, -32, 32, 120, 0, { 0b0000, 0 }, 0 },
    { 5, 0, 0, 120, 0, { 0b0000, 0 }, 0 },
    { 6, 0, 32, 120, 16, { 0b0110, 0 }, 0 },
    { 7, 32, 32, 32, 96, { 0b0100, 0 }, 0 },
    { 8, 0, 32, 16, 16, { 0b1111, 0b0011 }, 0 },
    { 9, -32, 32, 0, 16, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks042[] = {
    { 0, 0, 0, 0, 0, { 0b0111, 0 }, 0 },
    { 1, 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0010, 0 }, 0 },
    { 3, -32, -32, 0, 0, { 0b0111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks043[] = {
    { 0, 0, 0, 0, 0, { 0b1011, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0001, 0 }, 0 },
    { 3, -32, 32, 0, 0, { 0b1011, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks044[] = {
    { 0, 0, 0, 0, 0, { 0b0111, 0 }, 0 },
    { 1, 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0010, 0 }, 0 },
    { 3, -32, -32, 0, 0, { 0b0111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks045[] = {
    { 0, 0, 0, 0, 0, { 0b1011, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0001, 0 }, 0 },
    { 3, -32, 32, 0, 0, { 0b1011, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks046[] = {
    { 0, 0, 0, 0, 16, { 0b0111, 0b0100 }, 0 },
    { 1, 0, -32, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 16, 0, { 0b0010, 0 }, 0 },
    { 3, -32, -32, 16, 16,{ 0b0111, 0b0110 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks047[] = {
    { 0, 0, 0, 0, 16, { 0b1011, 0b1000 }, 0 },
    { 1, 0, 32, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 16, 0, { 0b0001, 0 }, 0 },
    { 3, -32, 32, 16, 16,{ 0b1011, 0b1001 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks048[] = {
    { 0, 0, 0, 16, 16,{ 0b0111, 0b0011 }, 0 },
    { 1, 0, -32, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 16, 0, { 0b0010, 0 }, 0 },
    { 3, -32, -32, 0, 16, { 0b0111, 0b0001 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks049[] = {
    { 0, 0, 0, 16, 16, { 0b1011, 0b0011 }, 0 },
    { 1, 0, 32, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 16, 0, { 0b0001, 0 }, 0 },
    { 3, -32, 32, 0, 16, { 0b1011, 0b0010 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks050[] = {
    { 0, 0, 0, 0, 0, { 0b0111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks051[] = {
    { 0, 0, 0, 0, 0, { 0b1011, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks052[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 2, -64, 0, 16, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks053[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 2, -64, 0, 16, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks054[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -16, 16, { 0b1111, 0 }, 0 },
    { 2, -64, 0, -16, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks055[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -16, 16, { 0b1111, 0 }, 0 },
    { 2, -64, 0, -16, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks056[] = {
    { 0, 0, 0, 0, 32, { 0b1111, 0b1100 }, 0 },
    { 1, -32, 0, 16, 16, { 0b1111, 0b1100 }, 0 },
    { 2, -64, 0, 32, 96,{ 0b0011, 0 }, 0 },
    { 3, -32, 0, 120, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks057[] = {
    { 0, 0, 0, -32, 32, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -120, 96,{ 0b0011, 0 }, 0 },
    { 2, 0, 0, -136, 16, { 0b1111, 0b1100 }, 0 },
    { 3, 32, 0, -152, 16, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks058[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 24, 32, { 0b0111, 0 }, 0 },
    { 2, -32, -32, 48, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks059[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 24, 32, { 0b1011, 0 }, 0 },
    { 2, -32, 32, 48, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks060[] = {
    { 0, 0, 0, -32, 16, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -56, 32, { 0b0111, 0 }, 0 },
    { 2, -32, -32, -80, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks061[] = {
    { 0, 0, 0, -32, 16, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -56, 32, { 0b1011, 0 }, 0 },
    { 2, -32, 32, -80, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks062[] = {
    { 0, 0, 0, 0, 24, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks063[] = {
    { 0, 0, 0, 0, 24, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks064[] = {
    { 0, 0, 0, 0, 24, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks065[] = {
    { 0, 0, 0, 0, 24, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks066[] = {
    { 0, 0, 0, 0, 64, { 0b1111, 0 }, 0 },
    { 1, -32, -32, 0, 0, { 0b1111, 0 }, 0 },
    { 2, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 3, -32, 32, 0, 0, { 0b1111, 0 }, 0 },
    { 4, 0, -32, 0, 0, { 0b1111, 0 }, 0 },
    { 5, 0, 32, 0, 0, { 0b1111, 0 }, 0 },
    { 6, 32, -32, 0, 0, { 0b1111, 0 }, 0 },
    { 7, 32, 32, 0, 0, { 0b1111, 0 }, 0 },
    { 8, 32, 0, 0, 0, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_1 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks067[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, 32, 0, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_1 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks068[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks069[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks070[] = {
    { 0, 0, 0, 0, 64, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks071[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks072[] = {
    { 0, 0, 0, 0, 32, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks073[] = {
    { 0, 0, 0, 0, 32, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks074[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks075[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks076[] = {
    { 0, 0, 0, 0, 64, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks077[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks078[] = {
    { 0, 0, 0, 0, 32, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks079[] = {
    { 0, 0, 0, 0, 32, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks080[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks081[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0111, 0 }, 0 },
    { 3, -32, -32, 0, 0, { 0b1101, 0 }, 0 },
    { 4, -32, -64, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, -32, 0, 0, { 0b0111, 0 }, 0 },
    { 6, -64, -64, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks082[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b1011, 0 }, 0 },
    { 3, -32, 32, 0, 0, { 0b1110, 0 }, 0 },
    { 4, -32, 64, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, 32, 0, 0, { 0b1011, 0 }, 0 },
    { 6, -64, 64, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks083[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 0, { 0b0111, 0 }, 0 },
    { 2, -32, -32, 0, 0, { 0b1101, 0 }, 0 },
    { 3, -64, -32, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks084[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 0, { 0b1011, 0 }, 0 },
    { 2, -32, 32, 0, 0, { 0b1110, 0 }, 0 },
    { 3, -64, 32, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks085[] = {
    { 0, 0, 0, 0, 0, { 0b0111, 0 }, 0 },
    { 1, 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0010, 0 }, 0 },
    { 3, -32, -32, 0, 0, { 0b0111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks086[] = {
    { 0, 0, 0, 0, 0, { 0b1011, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0001, 0 }, 0 },
    { 3, -32, 32, 0, 0, { 0b1011, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks087[] = {
    { 0, 0, 0, 0, 0, { 0b0111, 0 }, 0 },
    { 1, 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 4, { 0b0010, 0 }, 0 },
    { 3, -32, -32, 0, 4, { 0b0111, 0 }, 0 },
    { 4, -32, -64, 8, 0, { 0b1011, 0 }, 0 },
    { 5, 0, -64, 8, 0, { 0b0100, 0b0000 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 6, -32, -96, 8, 4, { 0b0001, 0 }, 0 },
    { 7, 0, -96, 8, 4, { 0b1011, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks088[] = {
    { 0, 0, 0, 0, 0, { 0b1011, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0b0000 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 4, { 0b0001, 0 }, 0 },
    { 3, -32, 32, 0, 4, { 0b1011, 0 }, 0 },
    { 4, -32, 64, 8, 0, { 0b0111, 0 }, 0 },
    { 5, 0, 64, 8, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 6, -32, 96, 8, 4, { 0b0010, 0 }, 0 },
    { 7, 0, 96, 8, 4, { 0b0111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks089[] = {
    { 0, 0, 0, 8, 4, { 0b0111, 0 }, 0 },
    { 1, 0, -32, 8, 4, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 8, 0, { 0b0010, 0 }, 0 },
    { 3, -32, -32, 8, 0, { 0b0111, 0 }, 0 },
    { 4, -32, -64, 0, 4, { 0b1011, 0 }, 0 },
    { 5, 0, -64, 0, 4, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 6, -32, -96, 0, 0, { 0b0001, 0 }, 0 },
    { 7, 0, -96, 0, 0, { 0b1011, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks090[] = {
    { 0, 0, 0, 8, 4, { 0b1011, 0 }, 0 },
    { 1, 0, 32, 8, 4, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 8, 0, { 0b0001, 0 }, 0 },
    { 3, -32, 32, 8, 0, { 0b1011, 0 }, 0 },
    { 4, -32, 64, 0, 4, { 0b0111, 0 }, 0 },
    { 5, 0, 64, 0, 4, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 6, -32, 96, 0, 0, { 0b0010, 0 }, 0 },
    { 7, 0, 96, 0, 0, { 0b0111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks091[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0111, 0 }, 0 },
    { 3, -32, -32, 0, 0, { 0b1101, 0 }, 0 },
    { 4, -32, -64, 0, 4, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, -32, 0, 4, { 0b0111, 0 }, 0 },
    { 6, -64, -64, 0, 4, { 0b1111, 0 }, 0 },
    { 7, -64, -96, 8, 0, { 0b1111, 0 }, 0 },
    { 8, -32, -96, 8, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 9, -64, -128, 8, 0, { 0b1011, 0 }, 0 },
    { 10, -32, -128, 8, 0, { 0b1110, 0 }, 0 },
    { 11, 0, -128, 8, 4, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 12, -32, -160, 8, 4, { 0b1011, 0 }, 0 },
    { 13, 0, -160, 8, 4, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks092[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b1011, 0 }, 0 },
    { 3, -32, 32, 0, 0, { 0b1110, 0 }, 0 },
    { 4, -32, 64, 0, 4, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, 32, 0, 4, { 0b1011, 0 }, 0 },
    { 6, -64, 64, 0, 4, { 0b1111, 0 }, 0 },
    { 7, -64, 96, 8, 0, { 0b1111, 0 }, 0 },
    { 8, -32, 96, 8, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 9, -64, 128, 8, 0, { 0b0111, 0 }, 0 },
    { 10, -32, 128, 8, 0, { 0b1101, 0 }, 0 },
    { 11, 0, 128, 8, 4, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 12, -32, 160, 8, 4, { 0b0111, 0 }, 0 },
    { 13, 0, 160, 8, 4, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks093[] = {
    { 0, 0, 0, 8, 4, { 0b1111, 0 }, 0 },
    { 1, 0, -32, 8, 4, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 8, 4, { 0b0111, 0 }, 0 },
    { 3, -32, -32, 8, 0, { 0b1101, 0 }, 0 },
    { 4, -32, -64, 8, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, -32, 8, 0, { 0b0111, 0 }, 0 },
    { 6, -64, -64, 8, 0, { 0b1111, 0 }, 0 },
    { 7, -64, -96, 0, 4, { 0b1111, 0 }, 0 },
    { 8, -32, -96, 0, 4, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 9, -64, -128, 0, 4, { 0b1011, 0 }, 0 },
    { 10, -32, -128, 0, 0, { 0b1110, 0 }, 0 },
    { 11, 0, -128, 0, 0, { 0b0100, 0b0000 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 12, -32, -160, 0, 0, { 0b1011, 0 }, 0 },
    { 13, 0, -160, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks094[] = {
    { 0, 0, 0, 8, 4, { 0b1111, 0 }, 0 },
    { 1, 0, 32, 8, 4, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 8, 4, { 0b1011, 0 }, 0 },
    { 3, -32, 32, 8, 0, { 0b1110, 0 }, 0 },
    { 4, -32, 64, 8, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, 32, 8, 0, { 0b1011, 0 }, 0 },
    { 6, -64, 64, 8, 0, { 0b1111, 0 }, 0 },
    { 7, -64, 96, 0, 4, { 0b1111, 0 }, 0 },
    { 8, -32, 96, 0, 4, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 9, -64, 128, 0, 4, { 0b0111, 0 }, 0 },
    { 10, -32, 128, 0, 0, { 0b1101, 0 }, 0 },
    { 11, 0, 128, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 12, -32, 160, 0, 0, { 0b0111, 0 }, 0 },
    { 13, 0, 160, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks095[] = {
    { 0, 0, 0, 0, 64, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks096[] = {
    { 0, 0, 0, 0, 64, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks097[] = {
    { 0, 0, 0, 0, 64, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks098[] = {
    { 0, 0, 0, 0, 64, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks099[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks100[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks101[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks102[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0111, 0 }, 0 },
    { 3, -32, -32, 0, 0, { 0b1101, 0 }, 0 },
    { 4, -32, -64, 0, 12, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, -32, 0, 12, { 0b0111, 0 }, 0 },
    { 6, -64, -64, 0, 12, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks103[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b1011, 0 }, 0 },
    { 3, -32, 32, 0, 0, { 0b1110, 0 }, 0 },
    { 4, -32, 64, 0, 12, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, 32, 0, 12, { 0b1011, 0 }, 0 },
    { 6, -64, 64, 0, 12, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks104[] = {
    { 0, 0, 0, 0, 12, { 0b1111, 0 }, 0 },
    { 1, 0, -32, 0, 12, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 12, { 0b0111, 0 }, 0 },
    { 3, -32, -32, 0, 0, { 0b1101, 0 }, 0 },
    { 4, -32, -64, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, -32, 0, 0, { 0b0111, 0 }, 0 },
    { 6, -64, -64, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks105[] = {
    { 0, 0, 0, 0, 12, { 0b1111, 0 }, 0 },
    { 1, 0, 32, 0, 12, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 12, { 0b1011, 0 }, 0 },
    { 3, -32, 32, 0, 0, { 0b1110, 0 }, 0 },
    { 4, -32, 64, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, 32, 0, 0, { 0b1011, 0 }, 0 },
    { 6, -64, 64, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks106[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0111, 0 }, 0 },
    { 3, -32, -32, 0, 0, { 0b1101, 0 }, 0 },
    { 4, -32, -64, 0, 12, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, -32, 0, 12, { 0b0111, 0 }, 0 },
    { 6, -64, -64, 0, 12, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks107[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b1011, 0 }, 0 },
    { 3, -32, 32, 0, 0, { 0b1110, 0 }, 0 },
    { 4, -32, 64, 0, 12, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, 32, 0, 12, { 0b1011, 0 }, 0 },
    { 6, -64, 64, 0, 12, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks108[] = {
    { 0, 0, 0, 0, 12, { 0b1111, 0 }, 0 },
    { 1, 0, -32, 0, 12, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 12, { 0b0111, 0 }, 0 },
    { 3, -32, -32, 0, 0, { 0b1101, 0 }, 0 },
    { 4, -32, -64, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, -32, 0, 0, { 0b0111, 0 }, 0 },
    { 6, -64, -64, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks109[] = {
    { 0, 0, 0, 0, 12, { 0b1111, 0 }, 0 },
    { 1, 0, 32, 0, 12, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 12, { 0b1011, 0 }, 0 },
    { 3, -32, 32, 0, 0, { 0b1110, 0 }, 0 },
    { 4, -32, 64, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, 32, 0, 0, { 0b1011, 0 }, 0 },
    { 6, -64, 64, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks110[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks111[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks112[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks113[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks114[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks115[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks116[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks117[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 2, -64, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 3, -96, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 4, -128, 0, 0, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks118[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 2, -64, 0, 16, 24, { 0b1111, 0 }, 0 },
    { 3, -96, 0, 40, 48, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks119[] = {
    { 0, 0, 0, 0, 48, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 40, 48, { 0b1111, 0 }, 0 },
    { 2, -64, 0, 64, 24, { 0b1111, 0 }, 0 },
    { 3, -96, 0, 80, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks120[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks121[] = {
    { 0, 0, 0, 40, 48, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 16, 24, { 0b1111, 0 }, 0 },
    { 2, -64, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 3, -96, 0, 0, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks122[] = {
    { 0, 0, 0, 80, 8, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 64, 24, { 0b1111, 0 }, 0 },
    { 2, -64, 0, 40, 48, { 0b1111, 0 }, 0 },
    { 3, -96, 0, 0, 48, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks123[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
    { 1, -32, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
    { 2, -64, 0, -32, 32, { 0b1111, 0b0011 }, 0 },
    { 3, -96, 0, -96, 64, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks124[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 32, { 0b1111, 0 }, 0 },
    { 2, -64, 0, 0, 48, { 0b1111, 0 }, 0 },
    { 3, -96, 0, 0, 80, { 0b1111, 0 }, 0 },
    { 4, -128, 0, 0, 160, { 0b1111, 0 }, 0 },
    { 5, -192, 0, 0, 208, { 0b1111, 0 }, 0 },
    { 6, -160, 0, 0, 208, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks125[] = {
    { 0, 0, 0, 0, 48, { 0b1111, 0 }, 0 },
    { 1, 32, 0, 0, 48, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks126[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 1, 32, 0, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks127[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 1, 32, 0, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks128[] = {
    { 0, 0, 0, 0, 32, { 0b1111, 0b1100 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 1, 32, 0, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks129[] = {
    { 0, 0, 0, 0, 32, { 0b1111, 0b0011 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks130[] = {
    { 0, 0, 0, 0, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks131[] = {
    { 0, 0, 0, 0, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 1, 32, 0, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks132[] = {
    { 0, 0, 0, 0, 24, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks133[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 2, -32, -32, 0, 0, { 0b1000, 0 }, 0 },
    { 3, -64, 0, 0, 0, { 0b0010, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 4, -64, -32, 0, 0, { 0b0001, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks134[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 2, -32, 32, 0, 0, { 0b0100, 0 }, 0 },
    { 3, -64, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 4, -64, 32, 0, 0, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks135[] = {
    { 0, 0, 0, 0, 0, { 0b1101, 0 }, 0 },
    { 1, -32, 0, 0, 0, { 0b0001, 0 }, 0 },
    { 2, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 0, { 0b1111, 0 }, 0 },
    { 4, -64, 32, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks136[] = {
    { 0, 0, 0, 0, 0, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, 0 },
    { 2, -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 0, { 0b1111, 0 }, 0 },
    { 4, -32, 64, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks137[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 2, -32, -32, 0, 0, { 0b1000, 0 }, 0 },
    { 3, -64, 0, 0, 0, { 0b0010, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 4, -64, -32, 0, 0, { 0b0001, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks138[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 2, -32, 32, 0, 0, { 0b0100, 0 }, 0 },
    { 3, -64, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 4, -64, 32, 0, 0, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks139[] = {
    { 0, 0, 0, 0, 0, { 0b1101, 0 }, 0 },
    { 1, -32, 0, 0, 0, { 0b0001, 0 }, 0 },
    { 2, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 0, { 0b1111, 0 }, 0 },
    { 4, -64, 32, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks140[] = {
    { 0, 0, 0, 0, 0, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, 0 },
    { 2, -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 0, { 0b1111, 0 }, 0 },
    { 4, -32, 64, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks141[] = {
    { 0, 0, 0, 0, 0, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 0, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks142[] = {
    { 0, 0, 0, 0, 16, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 16, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks143[] = {
    { 0, 0, 0, 0, 64, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 64, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 64, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 64, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks144[] = {
    { 0, 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks145[] = {
    { 0, 0, 0, 0, 32, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 32, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 32, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 32, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks146[] = {
    { 0, 0, 0, 0, 32, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 32, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 32, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 32, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks147[] = {
    { 0, 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks148[] = {
    { 0, 0, 0, 0, 16, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 16, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks149[] = {
    { 0, 0, 0, 0, 64, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 64, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 64, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 64, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks150[] = {
    { 0, 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks151[] = {
    { 0, 0, 0, 0, 32, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 32, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 32, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 32, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks152[] = {
    { 0, 0, 0, 0, 32, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 32, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 32, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 32, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks153[] = {
    { 0, 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks154[] = {
    { 0, 0, 0, 0, 24, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 24, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 24, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 24, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks155[] = {
    { 0, 0, 0, 0, 24, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 24, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 24, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 24, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks156[] = {
    { 0, 0, 0, 0, 24, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 24, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 24, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 24, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks157[] = {
    { 0, 0, 0, 0, 24, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 24, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 24, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 24, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks158[] = {
    { 0, 0, 0, 0, 0, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 0, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks159[] = {
    { 0, 0, 0, 0, 0, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 0, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks160[] = {
    { 0, 0, 0, 0, 0, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 0, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks161[] = {
    { 0, 0, 0, 0, 0, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 0, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks162[] = {
    { 0, 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks163[] = {
    { 0, 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks164[] = {
    { 0, 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks165[] = {
    { 0, 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks166[] = {
    { 0, 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks167[] = {
    { 0, 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks168[] = {
    { 0, 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks169[] = {
    { 0, 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks170[] = {
    { 0, 0, 0, 0, 0, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 0, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks171[] = {
    { 0, 0, 0, 0, 0, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 0, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks172[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks173[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks174[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 2, -64, 0, 0, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks175[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 2, -64, 0, 0, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks176[] = {
    { 0, 0, 0, -32, 16, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -32, 16, { 0b1111, 0 }, 0 },
    { 2, -64, 0, -32, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks177[] = {
    { 0, 0, 0, -32, 16, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -32, 16, { 0b1111, 0 }, 0 },
    { 2, -64, 0, -32, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks178[] = {
    { 0, 0, 0, 0, 0, { 0b0111, 0b0100 }, 0 },
    { 1, 0, -32, 0, 16, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 16, { 0b0010, 0 }, 0 },
    { 3, -32, -32, 16, 16,{ 0b0111, 0b0110 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks179[] = {
    { 0, 0, 0, 0, 16, { 0b1011, 0b1000 }, 0 },
    { 1, 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 16, { 0b0001, 0 }, 0 },
    { 3, -32, 32, 16, 16,{ 0b1011, 0b1001 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks180[] = {
    { 0, 0, 0, 16, 16,{ 0b0111, 0b0011 }, 0 },
    { 1, 0, -32, 0, 16, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 16, { 0b0010, 0 }, 0 },
    { 3, -32, -32, 0, 0, { 0b0111, 0b0001 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks181[] = {
    { 0, 0, 0, 16, 16, { 0b1011, 0b0011 }, 0 },
    { 1, 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 16, { 0b0001, 0 }, 0 },
    { 3, -32, 32, 0, 0, { 0b1011, 0b0010 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks182[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks183[] = {
    { 0, 0, 0, 0, 24, { 0b1111, 0b1100 }, 0 },
    { 1, -32, 0, 16, 40, { 0b1111, 0b1100 }, 0 },
    { 2, -64, 0, 32, 56, { 0b1111, 0 }, 0 },
    { 3, -96, 0, 64, 192, { 0b1111, 0 }, 0 },
    { 4, -128, -32, 120, 96, { 0b1111, 0 }, 0 },
    { 5, -96, -32, 64, 192, { 0b1111, 0 }, 0 },
    { 6, -64, -32, 248, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks184[] = {
    { 0, 0, 0, 0, 24, { 0b1111, 0b1100 }, 0 },
    { 1, -32, 0, 16, 40, { 0b1111, 0b1100 }, 0 },
    { 2, -64, 0, 32, 56, { 0b1111, 0 }, 0 },
    { 3, -96, 0, 64, 192, { 0b1111, 0 }, 0 },
    { 4, -128, 32, 120, 96, { 0b1111, 0 }, 0 },
    { 5, -96, 32, 64, 192, { 0b1111, 0 }, 0 },
    { 6, -64, 32, 248, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks185[] = {
    { 0, 0, 0, -32, 24, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -216, 192, { 0b1111, 0 }, 0 },
    { 2, -64, 0, -160, 96, { 0b1111, 0 }, 0 },
    { 3, -32, -32, -216, 192, { 0b1111, 0 }, 0 },
    { 4, 0, -32, -248, 56, { 0b1111, 0 }, 0 },
    { 5, 32, -32, -264, 40, { 0b1111, 0b1100 }, 0 },
    { 6, 64, -32, -280, 24, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks186[] = {
    { 0, 0, 0, -32, 24, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -216, 192, { 0b1111, 0 }, 0 },
    { 2, -64, 0, -160, 96, { 0b1111, 0 }, 0 },
    { 3, -32, 32, -216, 192, { 0b1111, 0 }, 0 },
    { 4, 0, 32, -248, 56, { 0b1111, 0 }, 0 },
    { 5, 32, 32, -264, 40, { 0b1111, 0b1100 }, 0 },
    { 6, 64, 32, -280, 24, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks187[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -16, 16, { 0b1111, 0 }, 0 },
    { 2, -64, 0, -16, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks188[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -16, 16, { 0b1111, 0 }, 0 },
    { 2, -64, 0, -16, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks189[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 2, -64, 0, 16, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks190[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 2, -64, 0, 16, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks191[] = {
    { 0, 0, 0, 0, 32, { 0b1111, 0b1100 }, 0 },
    { 1, -32, 0, 16, 16, { 0b1111, 0b1100 }, 0 },
    { 2, -64, 0, 32, 96,{ 0b0011, 0 }, 0 },
    { 3, -32, 0, 120, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks192[] = {
    { 0, 0, 0, 0, 32, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -88, 96,{ 0b0011, 0 }, 0 },
    { 2, 0, 0, -104, 16, { 0b1111, 0b1100 }, 0 },
    { 3, 32, 0, -120, 16, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks193[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 24, 32, { 0b0111, 0 }, 0 },
    { 2, -32, -32, 48, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks194[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 24, 32, { 0b1011, 0 }, 0 },
    { 2, -32, 32, 48, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks195[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -24, 32, { 0b0111, 0 }, 0 },
    { 2, -32, -32, -48, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks196[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -24, 32, { 0b1011, 0 }, 0 },
    { 2, -32, 32, -48, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks197[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 32, { 0b1111, 0 }, 0 },
    { 2, -64, 0, 16, 0, { 0b1111, 0 }, 0 },
    { 3, 0, 0, 32, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks198[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -32, 32, { 0b1111, 0 }, 0 },
    { 2, -64, 0, -16, 0, { 0b1111, 0 }, 0 },
    { 3, 0, 0, -32, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks199[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 2, -64, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 3, -96, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 4, -128, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 5, -160, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks200[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 2, -64, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 3, -96, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 4, -128, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 5, -160, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks201[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks202[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks203[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks204[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 2, -32, 32, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks205[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 2, -32, -32, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks206[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 1, -32, 0, -40, 32, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 2, -64, 0, -96, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 3, -96, 0, -96, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks207[] = {
    { 0, 0, 0, 0, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 1, 32, 0, 56, 32, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 2, 64, 0, 96, 16, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks208[] = {
    { 0, 0, 0, -32, 16, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 1, -32, 0, -72, 32, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 2, -64, 0, -128, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 3, -96, 0, -128, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks209[] = {
    { 0, 0, 0, 0, 0, { 0b0111, 0 }, 0 },
    { 1, 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 8, { 0b0010, 0 }, 0 },
    { 3, -32, -32, 0, 8, { 0b0111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks210[] = {
    { 0, 0, 0, 0, 0, { 0b1011, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 8, { 0b0001, 0 }, 0 },
    { 3, -32, 32, 0, 8, { 0b1011, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks211[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, 0, -32, 0, 0, { 0b1111, 0 }, 0 },
    { 2, -32, -32, 0, 0, { 0b1111, 0 }, 0 },
    { 3, -64, -32, 0, 0, { 0b1111, 0 }, 0 },
    { 4, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 5, -64, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks212[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b1111, 0 }, 0 },
    { 2, -32, 32, 0, 0, { 0b1111, 0 }, 0 },
    { 3, -64, 32, 0, 0, { 0b1111, 0 }, 0 },
    { 4, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 5, -64, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks213[] = {
    { 0, 0, 0, 0, 32, { 0b1111, 0 }, 0 },
    { 1, 32, 0, 0, 32, { 0b1111, 0 }, 0 },
    { 2, -64, 0, 0, 32, { 0b1111, 0 }, 0 },
    { 3, -32, 0, 0, 32, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks214[] = {
    { 0, 0, 0, 0, 48, { 0b1111, 0 }, 0 },
    { 1, 32, 0, 0, 48, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks215[] = {
    { 0, 0, 0, 0, 208, { 0b1111, 0 }, 0 },
    { 1, 32, 0, 0, 208, { 0b1111, 0 }, 0 },
    { 2, -32, 0, 0, 160, { 0b1111, 0 }, 0 },
    { 3, -64, 0, 0, 80, { 0b1111, 0 }, 0 },
    { 4, -96, 0, 0, 48, { 0b1111, 0 }, 0 },
    { 5, -128, 0, 0, 32, { 0b1111, 0 }, 0 },
    { 6, -160, 0, 0, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks216[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks217[] = {
    { 0, 0, 0, 0, 16, { 0b0111, 0b0100 }, 0 },
    { 1, 0, -32, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 16, 0, { 0b0010, 0 }, 0 },
    { 3, -32, -32, 16, 16,{ 0b0111, 0b0110 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks218[] = {
    { 0, 0, 0, 0, 16, { 0b1011, 0b1000 }, 0 },
    { 1, 0, 32, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 16, 0, { 0b0001, 0 }, 0 },
    { 3, -32, 32, 16, 16,{ 0b1011, 0b1001 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks219[] = {
    { 0, 0, 0, 16, 16,{ 0b0111, 0b0011 }, 0 },
    { 1, 0, -32, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 16, 0, { 0b0010, 0 }, 0 },
    { 3, -32, -32, 0, 16, { 0b0111, 0b0001 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks220[] = {
    { 0, 0, 0, 16, 16, { 0b1011, 0b0011 }, 0 },
    { 1, 0, 32, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 16, 0, { 0b0001, 0 }, 0 },
    { 3, -32, 32, 0, 16, { 0b1011, 0b0010 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks221[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
    { 1, 0, -32, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 16, 16, { 0b0111, 0b0100 }, 0 },
    { 3, -32, -32, 24, 16, { 0b1101, 0 }, 0 },
    { 4, -32, -64, 48, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, -32, 32, 16, { 0b0111, 0b0100 }, 0 },
    { 6, -64, -64, 48, 16, { 0b1111, 0b0110 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks222[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
    { 1, 0, 32, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 16, 16, { 0b1011, 0b1000 }, 0 },
    { 3, -32, 32, 24, 16, { 0b1110, 0 }, 0 },
    { 4, -32, 64, 48, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, 32, 32, 16, { 0b1011, 0b1000 }, 0 },
    { 6, -64, 64, 48, 16, { 0b1111, 0b1001 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks223[] = {
    { 0, 0, 0, 48, 16, { 0b1111, 0b0011 }, 0 },
    { 1, 0, -32, 48, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 32, 16, { 0b0111, 0b0001 }, 0 },
    { 3, -32, -32, 24, 16, { 0b1101, 0 }, 0 },
    { 4, -32, -64, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, -32, 16, 16, { 0b0111, 0b0001 }, 0 },
    { 6, -64, -64, 0, 16, { 0b1111, 0b1001 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks224[] = {
    { 0, 0, 0, 48, 16, { 0b1111, 0b0011 }, 0 },
    { 1, 0, 32, 48, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 32, 16, { 0b1011, 0b0010 }, 0 },
    { 3, -32, 32, 24, 16, { 0b1110, 0 }, 0 },
    { 4, -32, 64, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, 32, 16, 16, { 0b1011, 0b0010 }, 0 },
    { 6, -64, 64, 0, 16, { 0b1111, 0b0110 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks225[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks226[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks227[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks228[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks229[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks230[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks231[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks232[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks233[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks234[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks235[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks236[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks237[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks238[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks239[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks240[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks241[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks242[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks243[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks244[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks245[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks246[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks247[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks248[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks249[] = {
    { 0, 0, 0, 0, 72, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 1, 0, 32, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks250[] = {
    { 0, 0, 0, 0, 72, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 1, 0, -32, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks251[] = {
    { 0, 0, 0, 0, 72, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 1, 0, 32, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks252[] = {
    { 0, 0, 0, 0, 72, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 1, 0, -32, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks253[] = {
    { 0, 0, 0, 0, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 1, 32, 0, 56, 32, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 2, 64, 0, 96, 16, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks254[] = {
    { 0, 0, 0, -32, 16, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 1, -32, 0, -72, 32, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 2, -64, 0, -128, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 3, -96, 0, -128, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocks255[] = {
    { 0, 0, 0, 32, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 1, 32, 0, 88, 32, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 2, 64, 0, 128, 16, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocksRotationControlToggle[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocksFlatTrack1x4A[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -64, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 2, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 3, 32, 0, 0, 0, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_1 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocksFlatTrack2x2[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b1111, 0 }, 0 },
    { 2, 32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 3, 32, 32, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocksFlatTrack4x4[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b1111, 0 }, 0 },
    { 2, 0, 64, 0, 0, { 0b1111, 0 }, 0 },
    { 3, 0, 96, 0, 0, { 0b1111, 0 }, 0 },
    { 4, 32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 5, 32, 32, 0, 0, { 0b1111, 0 }, 0 },
    { 6, 32, 64, 0, 0, { 0b1111, 0 }, 0 },
    { 7, 32, 96, 0, 0, { 0b1111, 0 }, 0 },
    { 8, 64, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 9, 64, 32, 0, 0, { 0b1111, 0 }, 0 },
    { 10, 64, 64, 0, 0, { 0b1111, 0 }, 0 },
    { 11, 64, 96, 0, 0, { 0b1111, 0 }, 0 },
    { 12, 96, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 13, 96, 32, 0, 0, { 0b1111, 0 }, 0 },
    { 14, 96, 64, 0, 0, { 0b1111, 0 }, 0 },
    { 15, 96, 96, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocksFlatTrack2x4[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b1111, 0 }, 0 },
    { 2, 0, 64, 0, 0, { 0b1111, 0 }, 0 },
    { 3, 0, 96, 0, 0, { 0b1111, 0 }, 0 },
    { 4, 32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 5, 32, 32, 0, 0, { 0b1111, 0 }, 0 },
    { 6, 32, 64, 0, 0, { 0b1111, 0 }, 0 },
    { 7, 32, 96, 0, 0, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_1 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocksFlatTrack1x5[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -64, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 2, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 3, 32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 4, 64, 0, 0, 0, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_1 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocksFlatTrack1x1A[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocksFlatTrack1x4B[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -64, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 2, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 3, 32, 0, 0, 0, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_1 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocksFlatTrack1x1B[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocksFlatTrack1x4C[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -64, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 2, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 3, 32, 0, 0, 0, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_1 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocksFlatTrack3x3[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, -32, 0, 0, { 0b1111, 0 }, 0 },
    { 2, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 3, -32, 32, 0, 0, { 0b1111, 0 }, 0 },
    { 4, 0, -32, 0, 0, { 0b1111, 0 }, 0 },
    { 5, 0, 32, 0, 0, { 0b1111, 0 }, 0 },
    { 6, 32, -32, 0, 0, { 0b1111, 0 }, 0 },
    { 7, 32, 32, 0, 0, { 0b1111, 0 }, 0 },
    { 8, 32, 0, 0, 0, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_1 },
    TRACK_BLOCK_END
};


static constexpr const PreviewTrack TrackBlocksLeftLargeCorkscrewUp[] = {
    { 0,   0,   0,  0, 16, { 0b1111, 0 }, 0 },
    { 1, -32,   0,  8, 32, { 0b1111, 0 }, 0 },
    { 2, -64,   0, 32, 24, { 0b0111, 0 }, 0 },
    { 3, -32, -32, 32, 24, { 0b1000, 0 }, 0 },
    { 4, -64, -32, 48, 40, { 0b1111, 0 }, 0 },
    { 5, -64, -64, 72, 24, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocksRightLargeCorkscrewUp[] = {
    { 0,   0,   0 ,  0, 16, { 0b1111, 0 }, 0 },
    { 1, -32,   0,   8, 32, { 0b1111, 0 }, 0 },
    { 2, -64,   0,  32, 24, { 0b1011, 0 }, 0 },
    { 3, -32,  32,  32, 24, { 0b0100, 0 }, 0 },
    { 4, -64,  32,  48, 40, { 0b1111, 0 }, 0 },
    { 5, -64,  64,  72, 24, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocksLeftLargeCorkscrewDown[] = {
    { 0,   0,   0 ,  -40, 24, { 0b1111, 0 }, 0 },
    { 1, -32,   0,   -64, 40, { 0b1111, 0 }, 0 },
    { 2, -32, -32,   -80, 24, { 0b1000, 0 }, 0 },
    { 3, -64,   0,   -80, 24, { 0b0111, 0 }, 0 },
    { 4, -64, -32,  -104, 32, { 0b1111, 0 }, 0 },
    { 5, -64, -64,  -112, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocksRightLargeCorkscrewDown[] = {
    { 0,   0,   0 ,  -40, 24, { 0b1111, 0 }, 0 },
    { 1, -32,   0,   -64, 40, { 0b1111, 0 }, 0 },
    { 2, -32,  32,   -80, 24, { 0b0100, 0 }, 0 },
    { 3, -64,   0,   -80, 24, { 0b1011, 0 }, 0 },
    { 4, -64,  32,  -104, 32, { 0b1111, 0 }, 0 },
    { 5, -64,  64,  -112, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocksLeftMediumHalfLoopUp[] = {
    { 0,   0,   0,  0, 24, { 0b1111, 0 }, 0 },
    { 1, -32,   0, 16, 48, { 0b1111, 0 }, 0 },
    { 2, -64,   0, 56,120, { 0b1111, 0 }, 0 },
    { 3, -64, -32, 56,120, { 0b1111, 0 }, 0 },
    { 4, -32, -32,168, 24, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocksRightMediumHalfLoopUp[] = {
    { 0,   0,   0,  0, 24, { 0b1111, 0 }, 0 },
    { 1, -32,   0, 16, 48, { 0b1111, 0 }, 0 },
    { 2, -64,   0, 56,120, { 0b1111, 0 }, 0 },
    { 3, -64,  32, 56,120, { 0b1111, 0 }, 0 },
    { 4, -32,  32,168, 24, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocksLeftMediumHalfLoopDown[] = {
    { 0,   0,   0,  -48, 24, { 0b1111, 0 }, 0 },
    { 1, -32,   0, -160,120, { 0b1111, 0 }, 0 },
    { 2, -32, -32, -160,120, { 0b1111, 0 }, 0 },
    { 3,   0, -32, -200, 48, { 0b1111, 0 }, 0 },
    { 4,  32, -32, -216, 24, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocksRightMediumHalfLoopDown[] = {
    { 0,   0,   0 ,  -48,  24, { 0b1111, 0 }, 0 },
    { 1, -32,   0,  -160, 120, { 0b1111, 0 }, 0 },
    { 2, -32,  32,  -160, 120, { 0b1111, 0 }, 0 },
    { 3,   0,  32,  -200,  48, { 0b1111, 0 }, 0 },
    { 4,  32,  32,  -216,  24, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocksLeftZeroGRollUp[] = {
    { 0,   0,   0,  0, 16, { 0b1111, 0 }, 0 },
    { 1, -32,   0,  8, 24, { 0b1111, 0 }, 0 },
    { 2, -64,   0,  24, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocksRightZeroGRollUp[] = {
    { 0,   0,   0,  0, 16, { 0b1111, 0 }, 0 },
    { 1, -32,   0,  8, 24, { 0b1111, 0 }, 0 },
    { 2, -64,   0,  24, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocksLeftZeroGRollDown[] = {
    { 0,   0,   0, -32, 16, { 0b1111, 0 }, 0 },
    { 1, -32,   0, -48, 24, { 0b1111, 0 }, 0 },
    { 2, -64,   0, -56, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocksRightZeroGRollDown[] = {
    { 0,   0,   0 ,-32, 16, { 0b1111, 0 }, 0 },
    { 1, -32,   0, -48, 24, { 0b1111, 0 }, 0 },
    { 2, -64,   0, -56, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};


static constexpr const PreviewTrack TrackBlocksLeftLargeZeroGRollUp[] = {
    { 0,   0,   0,  0, 64, { 0b1111, 0 }, 0 },
    { 1, -32,   0, 56, 48, { 0b1111, 0 }, 0 },
    { 2, -64,   0, 96, 40, { 0b1111, 0 }, 0 },
    { 3, -96,   0,120, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocksRightLargeZeroGRollUp[] = {
    { 0,   0,   0,   0, 64, { 0b1111, 0 }, 0 },
    { 1, -32,   0,  56, 48, { 0b1111, 0 }, 0 },
    { 2, -64,   0,  96, 40, { 0b1111, 0 }, 0 },
    { 3, -96,   0, 120, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocksLeftLargeZeroGRollDown[] = {
    { 0,   0,   0,  -32, 16, { 0b1111, 0 }, 0 },
    { 1, -32,   0,  -56, 40, { 0b1111, 0 }, 0 },
    { 2, -64,   0,  -96, 48, { 0b1111, 0 }, 0 },
    { 3, -96,   0,  -152, 64, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocksRightLargeZeroGRollDown[] = {
    { 0,   0,   0 , -32, 16, { 0b1111, 0 }, 0 },
    { 1, -32,   0,  -56, 40, { 0b1111, 0 }, 0 },
    { 2, -64,   0,  -96, 48, { 0b1111, 0 }, 0 },
    { 3, -96,   0, -152, 64, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocksLeftFlyerLargeHalfLoopUninvertedUp[] = {
    { 0, 0, 0, 0, 24, { 0b1111, 0b1100 }, 0 },
    { 1, -32, 0, 16, 40, { 0b1111, 0b1100 }, 0 },
    { 2, -64, 0, 32, 56, { 0b1111, 0 }, 0 },
    { 3, -96, 0, 64, 192, { 0b1111, 0 }, 0 },
    { 4, -128, -32, 120, 96, { 0b1111, 0 }, 0 },
    { 5, -96, -32, 64, 192, { 0b1111, 0 }, 0 },
    { 6, -64, -32, 248, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocksRightFlyerLargeHalfLoopUninvertedUp[] = {
    { 0, 0, 0, 0, 24, { 0b1111, 0b1100 }, 0 },
    { 1, -32, 0, 16, 40, { 0b1111, 0b1100 }, 0 },
    { 2, -64, 0, 32, 56, { 0b1111, 0 }, 0 },
    { 3, -96, 0, 64, 192, { 0b1111, 0 }, 0 },
    { 4, -128, 32, 120, 96, { 0b1111, 0 }, 0 },
    { 5, -96, 32, 64, 192, { 0b1111, 0 }, 0 },
    { 6, -64, 32, 248, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};


static constexpr const PreviewTrack TrackBlocksLeftFlyerLargeHalfLoopInvertedUp[] = {
    { 0, 0, 0, 0, 24, { 0b1111, 0b1100 }, 0 },
    { 1, -32, 0, 16, 40, { 0b1111, 0b1100 }, 0 },
    { 2, -64, 0, 32, 56, { 0b1111, 0 }, 0 },
    { 3, -96, 0, 64, 192, { 0b1111, 0 }, 0 },
    { 4, -128, -32, 120, 96, { 0b1111, 0 }, 0 },
    { 5, -96, -32, 64, 192, { 0b1111, 0 }, 0 },
    { 6, -64, -32, 248, 32, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocksRightFlyerLargeHalfLoopInvertedUp[] = {
    { 0, 0, 0, 0, 24, { 0b1111, 0b1100 }, 0 },
    { 1, -32, 0, 16, 40, { 0b1111, 0b1100 }, 0 },
    { 2, -64, 0, 32, 56, { 0b1111, 0 }, 0 },
    { 3, -96, 0, 64, 192, { 0b1111, 0 }, 0 },
    { 4, -128, 32, 120, 96, { 0b1111, 0 }, 0 },
    { 5, -96, 32, 64, 192, { 0b1111, 0 }, 0 },
    { 6, -64, 32, 248, 32, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocksRightFlyerLargeHalfLoopInvertedDown[] = {
    { 0, 0, 0, 0, 24, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -184, 192, { 0b1111, 0 }, 0 },
    { 2, -64, 0, -128, 96, { 0b1111, 0 }, 0 },
    { 3, -32, -32, -184, 192, { 0b1111, 0 }, 0 },
    { 4, 0, -32, -216, 56, { 0b1111, 0 }, 0 },
    { 5, 32, -32, -232, 40, { 0b1111, 0b1100 }, 0 },
    { 6, 64, -32, -248, 24, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocksLeftFlyerLargeHalfLoopInvertedDown[] = {
    { 0, 0, 0, 0, 24, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -184, 192, { 0b1111, 0 }, 0 },
    { 2, -64, 0, -128, 96, { 0b1111, 0 }, 0 },
    { 3, -32, 32, -184, 192, { 0b1111, 0 }, 0 },
    { 4, 0, 32, -216, 56, { 0b1111, 0 }, 0 },
    { 5, 32, 32, -232, 40, { 0b1111, 0b1100 }, 0 },
    { 6, 64, 32, -248, 24, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocksRightFlyerLargeHalfLoopUninvertedDown[] = {
    { 0, 0, 0, -32, 40, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -216, 192, { 0b1111, 0 }, 0 },
    { 2, -64, 0, -160, 96, { 0b1111, 0 }, 0 },
    { 3, -32, -32, -216, 192, { 0b1111, 0 }, 0 },
    { 4, 0, -32, -248, 56, { 0b1111, 0 }, 0 },
    { 5, 32, -32, -264, 40, { 0b1111, 0b1100 }, 0 },
    { 6, 64, -32, -280, 24, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocksLeftFlyerLargeHalfLoopUninvertedDown[] = {
    { 0, 0, 0, -32, 40, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -216, 192, { 0b1111, 0 }, 0 },
    { 2, -64, 0, -160, 96, { 0b1111, 0 }, 0 },
    { 3, -32, 32, -216, 192, { 0b1111, 0 }, 0 },
    { 4, 0, 32, -248, 56, { 0b1111, 0 }, 0 },
    { 5, 32, 32, -264, 40, { 0b1111, 0b1100 }, 0 },
    { 6, 64, 32, -280, 24, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocksFlyerHalfLoopUpInverted[] = {
    { 0, 0, 0, 0, 32, { 0b1111, 0b1100 }, 0 },
    { 1, -32, 0, 16, 16, { 0b1111, 0b1100 }, 0 },
    { 2, -64, 0, 32, 96,{ 0b0011, 0 }, 0 },
    { 3, -32, 0, 120, 32, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const PreviewTrack TrackBlocksFlyerHalfLoopDownUpright[] = {
    { 0, 0, 0, -32, 32, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -120, 96,{ 0b0011, 0 }, 0 },
    { 2, 0, 0, -136, 16, { 0b1111, 0b1100 }, 0 },
    { 3, 32, 0, -152, 16, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

// rct2: 0x00994638
static constexpr std::array<const PreviewTrack*, TrackElemType::Count> TrackBlocks = {
    TrackBlocks000,
    TrackBlocks001,
    TrackBlocks002,
    TrackBlocks003,
    TrackBlocks004,
    TrackBlocks005,
    TrackBlocks006,
    TrackBlocks007,
    TrackBlocks008,
    TrackBlocks009,
    TrackBlocks010,
    TrackBlocks011,
    TrackBlocks012,
    TrackBlocks013,
    TrackBlocks014,
    TrackBlocks015,
    TrackBlocks016,
    TrackBlocks017,
    TrackBlocks018,
    TrackBlocks019,
    TrackBlocks020,
    TrackBlocks021,
    TrackBlocks022,
    TrackBlocks023,
    TrackBlocks024,
    TrackBlocks025,
    TrackBlocks026,
    TrackBlocks027,
    TrackBlocks028,
    TrackBlocks029,
    TrackBlocks030,
    TrackBlocks031,
    TrackBlocks032,
    TrackBlocks033,
    TrackBlocks034,
    TrackBlocks035,
    TrackBlocks036,
    TrackBlocks037,
    TrackBlocks038,
    TrackBlocks039,
    TrackBlocks040,
    TrackBlocks041,
    TrackBlocks042,
    TrackBlocks043,
    TrackBlocks044,
    TrackBlocks045,
    TrackBlocks046,
    TrackBlocks047,
    TrackBlocks048,
    TrackBlocks049,
    TrackBlocks050,
    TrackBlocks051,
    TrackBlocks052,
    TrackBlocks053,
    TrackBlocks054,
    TrackBlocks055,
    TrackBlocks056,
    TrackBlocks057,
    TrackBlocks058,
    TrackBlocks059,
    TrackBlocks060,
    TrackBlocks061,
    TrackBlocks062,
    TrackBlocks063,
    TrackBlocks064,
    TrackBlocks065,
    TrackBlocks066,
    TrackBlocks067,
    TrackBlocks068,
    TrackBlocks069,
    TrackBlocks070,
    TrackBlocks071,
    TrackBlocks072,
    TrackBlocks073,
    TrackBlocks074,
    TrackBlocks075,
    TrackBlocks076,
    TrackBlocks077,
    TrackBlocks078,
    TrackBlocks079,
    TrackBlocks080,
    TrackBlocks081,
    TrackBlocks082,
    TrackBlocks083,
    TrackBlocks084,
    TrackBlocks085,
    TrackBlocks086,
    TrackBlocks087,
    TrackBlocks088,
    TrackBlocks089,
    TrackBlocks090,
    TrackBlocks091,
    TrackBlocks092,
    TrackBlocks093,
    TrackBlocks094,
    TrackBlocks095,
    TrackBlocks096,
    TrackBlocks097,
    TrackBlocks098,
    TrackBlocks099,
    TrackBlocks100,
    TrackBlocks101,
    TrackBlocks102,
    TrackBlocks103,
    TrackBlocks104,
    TrackBlocks105,
    TrackBlocks106,
    TrackBlocks107,
    TrackBlocks108,
    TrackBlocks109,
    TrackBlocks110,
    TrackBlocks111,
    TrackBlocks112,
    TrackBlocks113,
    TrackBlocks114,
    TrackBlocks115,
    TrackBlocks116,
    TrackBlocks117,
    TrackBlocks118,
    TrackBlocks119,
    TrackBlocks120,
    TrackBlocks121,
    TrackBlocks122,
    TrackBlocks123,
    TrackBlocks124,
    TrackBlocks125,
    TrackBlocks126,
    TrackBlocks127,
    TrackBlocks128,
    TrackBlocks129,
    TrackBlocks130,
    TrackBlocks131,
    TrackBlocks132,
    TrackBlocks133,
    TrackBlocks134,
    TrackBlocks135,
    TrackBlocks136,
    TrackBlocks137,
    TrackBlocks138,
    TrackBlocks139,
    TrackBlocks140,
    TrackBlocks141,
    TrackBlocks142,
    TrackBlocks143,
    TrackBlocks144,
    TrackBlocks145,
    TrackBlocks146,
    TrackBlocks147,
    TrackBlocks148,
    TrackBlocks149,
    TrackBlocks150,
    TrackBlocks151,
    TrackBlocks152,
    TrackBlocks153,
    TrackBlocks154,
    TrackBlocks155,
    TrackBlocks156,
    TrackBlocks157,
    TrackBlocks158,
    TrackBlocks159,
    TrackBlocks160,
    TrackBlocks161,
    TrackBlocks162,
    TrackBlocks163,
    TrackBlocks164,
    TrackBlocks165,
    TrackBlocks166,
    TrackBlocks167,
    TrackBlocks168,
    TrackBlocks169,
    TrackBlocks170,
    TrackBlocks171,
    TrackBlocks172,
    TrackBlocks173,
    TrackBlocks174,
    TrackBlocks175,
    TrackBlocks176,
    TrackBlocks177,
    TrackBlocks178,
    TrackBlocks179,
    TrackBlocks180,
    TrackBlocks181,
    TrackBlocks182,
    TrackBlocks183,
    TrackBlocks184,
    TrackBlocks185,
    TrackBlocks186,
    TrackBlocks187,
    TrackBlocks188,
    TrackBlocks189,
    TrackBlocks190,
    TrackBlocks191,
    TrackBlocks192,
    TrackBlocks193,
    TrackBlocks194,
    TrackBlocks195,
    TrackBlocks196,
    TrackBlocks197,
    TrackBlocks198,
    TrackBlocks199,
    TrackBlocks200,
    TrackBlocks201,
    TrackBlocks202,
    TrackBlocks203,
    TrackBlocks204,
    TrackBlocks205,
    TrackBlocks206,
    TrackBlocks207,
    TrackBlocks208,
    TrackBlocks209,
    TrackBlocks210,
    TrackBlocks211,
    TrackBlocks212,
    TrackBlocks213,
    TrackBlocks214,
    TrackBlocks215,
    TrackBlocks216,
    TrackBlocks217,
    TrackBlocks218,
    TrackBlocks219,
    TrackBlocks220,
    TrackBlocks221,
    TrackBlocks222,
    TrackBlocks223,
    TrackBlocks224,
    TrackBlocks225,
    TrackBlocks226,
    TrackBlocks227,
    TrackBlocks228,
    TrackBlocks229,
    TrackBlocks230,
    TrackBlocks231,
    TrackBlocks232,
    TrackBlocks233,
    TrackBlocks234,
    TrackBlocks235,
    TrackBlocks236,
    TrackBlocks237,
    TrackBlocks238,
    TrackBlocks239,
    TrackBlocks240,
    TrackBlocks241,
    TrackBlocks242,
    TrackBlocks243,
    TrackBlocks244,
    TrackBlocks245,
    TrackBlocks246,
    TrackBlocks247,
    TrackBlocks248,
    TrackBlocks249,
    TrackBlocks250,
    TrackBlocks251,
    TrackBlocks252,
    TrackBlocks253,
    TrackBlocks254,
    TrackBlocks255,
    TrackBlocksRotationControlToggle,

    TrackBlocksFlatTrack1x4A,
    TrackBlocksFlatTrack2x2,
    TrackBlocksFlatTrack4x4,
    TrackBlocksFlatTrack2x4,
    TrackBlocksFlatTrack1x5,
    TrackBlocksFlatTrack1x1A,
    TrackBlocksFlatTrack1x4B,
    TrackBlocksFlatTrack1x1B,
    TrackBlocksFlatTrack1x4C,
    TrackBlocksFlatTrack3x3,
    TrackBlocksLeftLargeCorkscrewUp,
    TrackBlocksRightLargeCorkscrewUp,
    TrackBlocksLeftLargeCorkscrewDown,
    TrackBlocksRightLargeCorkscrewDown,
    TrackBlocksLeftMediumHalfLoopUp,
    TrackBlocksRightMediumHalfLoopUp,
    TrackBlocksLeftMediumHalfLoopDown,
    TrackBlocksRightMediumHalfLoopDown,
    TrackBlocksLeftZeroGRollUp,
    TrackBlocksRightZeroGRollUp,
    TrackBlocksLeftZeroGRollDown,
    TrackBlocksRightZeroGRollDown,
    TrackBlocksLeftLargeZeroGRollUp,
    TrackBlocksRightLargeZeroGRollUp,
    TrackBlocksLeftLargeZeroGRollDown,
    TrackBlocksRightLargeZeroGRollDown,

    TrackBlocksLeftFlyerLargeHalfLoopUninvertedUp,    // LeftFlyerLargeHalfLoopUninvertedUp
    TrackBlocksRightFlyerLargeHalfLoopUninvertedUp,   // RightFlyerLargeHalfLoopUninvertedUp
    TrackBlocksRightFlyerLargeHalfLoopInvertedDown,   // RightFlyerLargeHalfLoopInvertedDown
    TrackBlocksLeftFlyerLargeHalfLoopInvertedDown,    // LeftFlyerLargeHalfLoopInvertedDown
    TrackBlocksLeftFlyerLargeHalfLoopInvertedUp,      // TrackElemType::LeftFlyerLargeHalfLoopInvertedUp
    TrackBlocksRightFlyerLargeHalfLoopInvertedUp,     // TrackElemType::RightFlyerLargeHalfLoopInvertedUp
    TrackBlocksRightFlyerLargeHalfLoopUninvertedDown, // TrackElemType::RightFlyerLargeHalfLoopUninvertedDown
    TrackBlocksLeftFlyerLargeHalfLoopUninvertedDown,  // TrackElemType::LeftFlyerLargeHalfLoopUninvertedDown
    TrackBlocksFlyerHalfLoopUpInverted,    // TrackElemType::FlyerHalfLoopInvertedUp
    TrackBlocksFlyerHalfLoopDownUpright,   // TrackElemType::FlyerHalfLoopUninvertedDown
};

static constexpr uint8_t TrackPieceLengths[TrackElemType::Count] = {
    32,     // TrackElemType::Flat
    32,     // TrackElemType::EndStation
    32,     // TrackElemType::BeginStation
    32,     // TrackElemType::MiddleStation
    33,     // TrackElemType::Up25
    40,     // TrackElemType::Up60
    32,     // TrackElemType::FlatToUp25
    34,     // TrackElemType::Up25ToUp60
    34,     // TrackElemType::Up60ToUp25
    32,     // TrackElemType::Up25ToFlat
    33,     // TrackElemType::Down25
    40,     // TrackElemType::Down60
    32,     // TrackElemType::FlatToDown25
    34,     // TrackElemType::Down25ToDown60
    34,     // TrackElemType::Down60ToDown25
    32,     // TrackElemType::Down25ToFlat
    124,    // TrackElemType::LeftQuarterTurn5Tiles
    124,    // TrackElemType::RightQuarterTurn5Tiles
    32,     // TrackElemType::FlatToLeftBank
    32,     // TrackElemType::FlatToRightBank
    32,     // TrackElemType::LeftBankToFlat
    32,     // TrackElemType::RightBankToFlat
    124,    // TrackElemType::BankedLeftQuarterTurn5Tiles
    124,    // TrackElemType::BankedRightQuarterTurn5Tiles
    32,     // TrackElemType::LeftBankToUp25
    32,     // TrackElemType::RightBankToUp25
    32,     // TrackElemType::Up25ToLeftBank
    32,     // TrackElemType::Up25ToRightBank
    32,     // TrackElemType::LeftBankToDown25
    32,     // TrackElemType::RightBankToDown25
    32,     // TrackElemType::Down25ToLeftBank
    32,     // TrackElemType::Down25ToRightBank
    32,     // TrackElemType::LeftBank
    32,     // TrackElemType::RightBank
    130,    // TrackElemType::LeftQuarterTurn5TilesUp25
    130,    // TrackElemType::RightQuarterTurn5TilesUp25
    130,    // TrackElemType::LeftQuarterTurn5TilesDown25
    130,    // TrackElemType::RightQuarterTurn5TilesDown25
    96,     // TrackElemType::SBendLeft
    96,     // TrackElemType::SBendRight
    120,    // TrackElemType::LeftVerticalLoop
    120,    // TrackElemType::RightVerticalLoop
    75,     // TrackElemType::LeftQuarterTurn3Tiles
    75,     // TrackElemType::RightQuarterTurn3Tiles
    75,     // TrackElemType::LeftBankedQuarterTurn3Tiles
    75,     // TrackElemType::RightBankedQuarterTurn3Tiles
    77,     // TrackElemType::LeftQuarterTurn3TilesUp25
    77,     // TrackElemType::RightQuarterTurn3TilesUp25
    77,     // TrackElemType::LeftQuarterTurn3TilesDown25
    77,     // TrackElemType::RightQuarterTurn3TilesDown25
    24,     // TrackElemType::LeftQuarterTurn1Tile
    24,     // TrackElemType::RightQuarterTurn1Tile
    96,     // TrackElemType::LeftTwistDownToUp
    96,     // TrackElemType::RightTwistDownToUp
    96,     // TrackElemType::LeftTwistUpToDown
    96,     // TrackElemType::RightTwistUpToDown
    60,     // TrackElemType::HalfLoopUp
    60,     // TrackElemType::HalfLoopDown
    55,     // TrackElemType::LeftCorkscrewUp
    55,     // TrackElemType::RightCorkscrewUp
    55,     // TrackElemType::LeftCorkscrewDown
    55,     // TrackElemType::RightCorkscrewDown
    36,     // TrackElemType::FlatToUp60
    36,     // TrackElemType::Up60ToFlat
    36,     // TrackElemType::FlatToDown60
    36,     // TrackElemType::Down60ToFlat
    32,     // TrackElemType::TowerBase
    32,     // TrackElemType::TowerSection
    32,     // TrackElemType::FlatCovered
    33,     // TrackElemType::Up25Covered
    40,     // TrackElemType::Up60Covered
    32,     // TrackElemType::FlatToUp25Covered
    34,     // TrackElemType::Up25ToUp60Covered
    34,     // TrackElemType::Up60ToUp25Covered
    32,     // TrackElemType::Up25ToFlatCovered
    33,     // TrackElemType::Down25Covered
    40,     // TrackElemType::Down60Covered
    32,     // TrackElemType::FlatToDown25Covered
    34,     // TrackElemType::Down25ToDown60Covered
    34,     // TrackElemType::Down60ToDown25Covered
    32,     // TrackElemType::Down25ToFlatCovered
    124,    // TrackElemType::LeftQuarterTurn5TilesCovered
    124,    // TrackElemType::RightQuarterTurn5TilesCovered
    96,     // TrackElemType::SBendLeftCovered
    96,     // TrackElemType::SBendRightCovered
    75,     // TrackElemType::LeftQuarterTurn3TilesCovered
    75,     // TrackElemType::RightQuarterTurn3TilesCovered
    150,    // TrackElemType::LeftHalfBankedHelixUpSmall
    150,    // TrackElemType::RightHalfBankedHelixUpSmall
    150,    // TrackElemType::LeftHalfBankedHelixDownSmall
    150,    // TrackElemType::RightHalfBankedHelixDownSmall
    248,    // TrackElemType::LeftHalfBankedHelixUpLarge
    248,    // TrackElemType::RightHalfBankedHelixUpLarge
    248,    // TrackElemType::LeftHalfBankedHelixDownLarge
    248,    // TrackElemType::RightHalfBankedHelixDownLarge
    64,     // TrackElemType::LeftQuarterTurn1TileUp60
    64,     // TrackElemType::RightQuarterTurn1TileUp60
    64,     // TrackElemType::LeftQuarterTurn1TileDown60
    64,     // TrackElemType::RightQuarterTurn1TileDown60
    32,     // TrackElemType::Brakes
    32,     // TrackElemType::Booster
    32,     // TrackElemType::InvertedUp90ToFlatQuarterLoopAlias / Maze
    124,    // TrackElemType::LeftQuarterBankedHelixLargeUp
    124,    // TrackElemType::RightQuarterBankedHelixLargeUp
    124,    // TrackElemType::LeftQuarterBankedHelixLargeDown
    124,    // TrackElemType::RightQuarterBankedHelixLargeDown
    124,    // TrackElemType::LeftQuarterHelixLargeUp
    124,    // TrackElemType::RightQuarterHelixLargeUp
    124,    // TrackElemType::LeftQuarterHelixLargeDown
    124,    // TrackElemType::RightQuarterHelixLargeDown
    33,     // TrackElemType::Up25LeftBanked
    33,     // TrackElemType::Up25RightBanked
    32,     // TrackElemType::Waterfall
    32,     // TrackElemType::Rapids
    32,     // TrackElemType::OnRidePhoto
    33,     // TrackElemType::Down25LeftBanked
    33,     // TrackElemType::Down25RightBanked
    128,    // TrackElemType::Watersplash
    165,    // TrackElemType::FlatToUp60LongBase
    165,    // TrackElemType::Up60ToFlatLongBase
    32,     // TrackElemType::Whirlpool
    165,    // TrackElemType::Down60ToFlatLongBase
    165,    // TrackElemType::FlatToDown60LongBase
    138,    // TrackElemType::CableLiftHill
    32,     // TrackElemType::ReverseFreefallSlope
    32,     // TrackElemType::ReverseFreefallVertical
    32,     // TrackElemType::Up90
    32,     // TrackElemType::Down90
    32,     // TrackElemType::Up60ToUp90
    32,     // TrackElemType::Down90ToDown60
    32,     // TrackElemType::Up90ToUp60
    32,     // TrackElemType::Down60ToDown90
    32,     // TrackElemType::BrakeForDrop
    87,     // TrackElemType::LeftEighthToDiag
    87,     // TrackElemType::RightEighthToDiag
    87,     // TrackElemType::LeftEighthToOrthogonal
    87,     // TrackElemType::RightEighthToOrthogonal
    87,     // TrackElemType::LeftEighthBankToDiag
    87,     // TrackElemType::RightEighthBankToDiag
    87,     // TrackElemType::LeftEighthBankToOrthogonal
    87,     // TrackElemType::RightEighthBankToOrthogonal
    45,     // TrackElemType::DiagFlat
    45,     // TrackElemType::DiagUp25
    45,     // TrackElemType::DiagUp60
    45,     // TrackElemType::DiagFlatToUp25
    45,     // TrackElemType::DiagUp25ToUp60
    45,     // TrackElemType::DiagUp60ToUp25
    45,     // TrackElemType::DiagUp25ToFlat
    45,     // TrackElemType::DiagDown25
    45,     // TrackElemType::DiagDown60
    45,     // TrackElemType::DiagFlatToDown25
    45,     // TrackElemType::DiagDown25ToDown60
    45,     // TrackElemType::DiagDown60ToDown25
    45,     // TrackElemType::DiagDown25ToFlat
    45,     // TrackElemType::DiagFlatToUp60
    45,     // TrackElemType::DiagUp60ToFlat
    45,     // TrackElemType::DiagFlatToDown60
    45,     // TrackElemType::DiagDown60ToFlat
    45,     // TrackElemType::DiagFlatToLeftBank
    45,     // TrackElemType::DiagFlatToRightBank
    45,     // TrackElemType::DiagLeftBankToFlat
    45,     // TrackElemType::DiagRightBankToFlat
    45,     // TrackElemType::DiagLeftBankToUp25
    45,     // TrackElemType::DiagRightBankToUp25
    45,     // TrackElemType::DiagUp25ToLeftBank
    45,     // TrackElemType::DiagUp25ToRightBank
    45,     // TrackElemType::DiagLeftBankToDown25
    45,     // TrackElemType::DiagRightBankToDown25
    45,     // TrackElemType::DiagDown25ToLeftBank
    45,     // TrackElemType::DiagDown25ToRightBank
    45,     // TrackElemType::DiagLeftBank
    45,     // TrackElemType::DiagRightBank
    16,     // TrackElemType::LogFlumeReverser
    32,     // TrackElemType::SpinningTunnel
    96,     // TrackElemType::LeftBarrelRollUpToDown
    96,     // TrackElemType::RightBarrelRollUpToDown
    96,     // TrackElemType::LeftBarrelRollDownToUp
    96,     // TrackElemType::RightBarrelRollDownToUp
    64,     // TrackElemType::LeftBankToLeftQuarterTurn3TilesUp25
    64,     // TrackElemType::RightBankToRightQuarterTurn3TilesUp25
    64,     // TrackElemType::LeftQuarterTurn3TilesDown25ToLeftBank
    64,     // TrackElemType::RightQuarterTurn3TilesDown25ToRightBank
    32,     // TrackElemType::PoweredLift
    100,    // TrackElemType::LeftLargeHalfLoopUp
    100,    // TrackElemType::RightLargeHalfLoopUp
    100,    // TrackElemType::RightLargeHalfLoopDown
    100,    // TrackElemType::LeftLargeHalfLoopDown
    96,     // TrackElemType::LeftFlyerTwistUp
    96,     // TrackElemType::RightFlyerTwistUp
    96,     // TrackElemType::LeftFlyerTwistDown
    96,     // TrackElemType::RightFlyerTwistDown
    64,     // TrackElemType::FlyerHalfLoopUninvertedUp
    64,     // TrackElemType::FlyerHalfLoopInvertedDown
    64,     // TrackElemType::LeftFlyerCorkscrewUp
    64,     // TrackElemType::RightFlyerCorkscrewUp
    64,     // TrackElemType::LeftFlyerCorkscrewDown
    64,     // TrackElemType::RightFlyerCorkscrewDown
    16,     // TrackElemType::HeartLineTransferUp
    16,     // TrackElemType::HeartLineTransferDown
    64,     // TrackElemType::LeftHeartLineRoll
    64,     // TrackElemType::RightHeartLineRoll
    32,     // TrackElemType::MinigolfHoleA
    32,     // TrackElemType::MinigolfHoleB
    32,     // TrackElemType::MinigolfHoleC
    32,     // TrackElemType::MinigolfHoleD
    32,     // TrackElemType::MinigolfHoleE
    80,     // TrackElemType::MultiDimInvertedFlatToDown90QuarterLoop
    80,     // TrackElemType::Up90ToInvertedFlatQuarterLoop
    80,     // TrackElemType::InvertedFlatToDown90QuarterLoop
    64,     // TrackElemType::LeftCurvedLiftHill
    64,     // TrackElemType::RightCurvedLiftHill
    32,     // TrackElemType::LeftReverser
    32,     // TrackElemType::RightReverser
    32,     // TrackElemType::AirThrustTopCap
    32,     // TrackElemType::AirThrustVerticalDown
    32,     // TrackElemType::AirThrustVerticalDownToLevel
    32,     // TrackElemType::BlockBrakes
    32,     // TrackElemType::LeftBankedQuarterTurn3TileUp25
    32,     // TrackElemType::RightBankedQuarterTurn3TileUp25
    32,     // TrackElemType::LeftBankedQuarterTurn3TileDown25
    32,     // TrackElemType::RightBankedQuarterTurn3TileDown25
    32,     // TrackElemType::LeftBankedQuarterTurn5TileUp25
    32,     // TrackElemType::RightBankedQuarterTurn5TileUp25
    32,     // TrackElemType::LeftBankedQuarterTurn5TileDown25
    32,     // TrackElemType::RightBankedQuarterTurn5TileDown25
    32,     // TrackElemType::Up25ToLeftBankedUp25
    32,     // TrackElemType::Up25ToRightBankedUp25
    32,     // TrackElemType::LeftBankedUp25ToUp25
    32,     // TrackElemType::RightBankedUp25ToUp25
    32,     // TrackElemType::Down25ToLeftBankedDown25
    32,     // TrackElemType::Down25ToRightBankedDown25
    32,     // TrackElemType::LeftBankedDown25ToDown25
    32,     // TrackElemType::RightBankedDown25ToDown25
    32,     // TrackElemType::LeftBankedFlatToLeftBankedUp25
    32,     // TrackElemType::RightBankedFlatToRightBankedUp25
    32,     // TrackElemType::LeftBankedUp25ToLeftBankedFlat
    32,     // TrackElemType::RightBankedUp25ToRightBankedFlat
    32,     // TrackElemType::LeftBankedFlatToLeftBankedDown25
    32,     // TrackElemType::RightBankedFlatToRightBankedDown25
    32,     // TrackElemType::LeftBankedDown25ToLeftBankedFlat
    32,     // TrackElemType::RightBankedDown25ToRightBankedFlat
    32,     // TrackElemType::FlatToLeftBankedUp25
    32,     // TrackElemType::FlatToRightBankedUp25
    32,     // TrackElemType::LeftBankedUp25ToFlat
    32,     // TrackElemType::RightBankedUp25ToFlat
    32,     // TrackElemType::FlatToLeftBankedDown25
    32,     // TrackElemType::FlatToRightBankedDown25
    32,     // TrackElemType::LeftBankedDown25ToFlat
    32,     // TrackElemType::RightBankedDown25ToFlat
    32,     // TrackElemType::LeftQuarterTurn1TileUp90
    32,     // TrackElemType::RightQuarterTurn1TileUp90
    32,     // TrackElemType::LeftQuarterTurn1TileDown90
    32,     // TrackElemType::RightQuarterTurn1TileDown90
    80,     // TrackElemType::MultiDimUp90ToInvertedFlatQuarterLoop
    80,     // TrackElemType::MultiDimFlatToDown90QuarterLoop
    80,     // TrackElemType::MultiDimInvertedUp90ToFlatQuarterLoop
    32,     // TrackElemType::RotationControlToggle
    0,      // TrackElemType::FlatTrack1x4A
    0,      // TrackElemType::FlatTrack2x2
    0,      // TrackElemType::FlatTrack4x4
    0,      // TrackElemType::FlatTrack2x4
    0,      // TrackElemType::FlatTrack1x5
    0,      // TrackElemType::FlatTrack1x1A
    0,      // TrackElemType::FlatTrack1x4B
    0,      // TrackElemType::FlatTrack1x1B
    0,      // TrackElemType::FlatTrack1x4C
    0,      // TrackElemType::FlatTrack3x3
    96,     // TrackElemType::LeftCorkscrewUp
    96,     // TrackElemType::RightCorkscrewUp
    96,     // TrackElemType::LeftCorkscrewDown
    96,     // TrackElemType::RightCorkscrewDown
    80,     // TrackElemType::LeftMediumHalfLoopUp
    80,     // TrackElemType::RightMediumHalfLoopUp
    80,     // TrackElemType::RightMediumHalfLoopDown
    80,     // TrackElemType::LeftMediumHalfLoopDown
    96,     // TrackElemType::LeftZeroGRollUp
    96,     // TrackElemType::RightZeroGRollUp
    96,     // TrackElemType::RightZeroGRollDown
    96,     // TrackElemType::LeftZeroGRollDown
    165,    // TrackElemType::LeftLargeZeroGRollUp
    165,    // TrackElemType::RightLargeZeroGRollUp
    165,    // TrackElemType::RightLargeZeroGRollDown
    165,    // TrackElemType::LeftLargeZeroGRollDown
    100,    // TrackElemType::LeftFlyerLargeHalfLoopUninvertedUp
    100,    // TrackElemType::RightFlyerLargeHalfLoopUninvertedUp
    100,    // TrackElemType::RightFlyerLargeHalfLoopInvertedDown
    100,    // TrackElemType::LeftFlyerLargeHalfLoopInvertedDown
    100,    // TrackElemType::LeftFlyerLargeHalfLoopInvertedUp
    100,    // TrackElemType::RightFlyerLargeHalfLoopInvertedUp
    100,    // TrackElemType::RightFlyerLargeHalfLoopUninvertedDown
    100,    // TrackElemType::LeftFlyerLargeHalfLoopUninvertedDown
    64,     // TrackElemType::FlyerHalfLoopInvertedUp
    64,     // TrackElemType::FlyerHalfLoopUninvertedDown
};

// rct2: 0x00998C95
static constexpr TrackCurveChain gTrackCurveChain[TrackElemType::Count] = {
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { RideConstructionSpecialPieceSelected | TrackElemType::EndStation, RideConstructionSpecialPieceSelected | TrackElemType::EndStation },
    { RideConstructionSpecialPieceSelected | TrackElemType::EndStation, RideConstructionSpecialPieceSelected | TrackElemType::EndStation },
    { RideConstructionSpecialPieceSelected | TrackElemType::EndStation, RideConstructionSpecialPieceSelected | TrackElemType::EndStation },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_LEFT, TRACK_CURVE_LEFT },
    { TRACK_CURVE_RIGHT, TRACK_CURVE_RIGHT },
    { TRACK_CURVE_LEFT, TRACK_CURVE_NONE },
    { TRACK_CURVE_RIGHT, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_LEFT },
    { TRACK_CURVE_NONE, TRACK_CURVE_RIGHT },
    { TRACK_CURVE_LEFT, TRACK_CURVE_LEFT },
    { TRACK_CURVE_RIGHT, TRACK_CURVE_RIGHT },
    { TRACK_CURVE_NONE, TRACK_CURVE_LEFT },
    { TRACK_CURVE_NONE, TRACK_CURVE_RIGHT },
    { TRACK_CURVE_LEFT, TRACK_CURVE_NONE },
    { TRACK_CURVE_RIGHT, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_LEFT },
    { TRACK_CURVE_NONE, TRACK_CURVE_RIGHT },
    { TRACK_CURVE_LEFT, TRACK_CURVE_NONE },
    { TRACK_CURVE_RIGHT, TRACK_CURVE_NONE },
    { TRACK_CURVE_LEFT, TRACK_CURVE_LEFT },
    { TRACK_CURVE_RIGHT, TRACK_CURVE_RIGHT },
    { TRACK_CURVE_LEFT, TRACK_CURVE_LEFT },
    { TRACK_CURVE_RIGHT, TRACK_CURVE_RIGHT },
    { TRACK_CURVE_LEFT, TRACK_CURVE_LEFT },
    { TRACK_CURVE_RIGHT, TRACK_CURVE_RIGHT },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_LEFT_SMALL, TRACK_CURVE_LEFT_SMALL },
    { TRACK_CURVE_RIGHT_SMALL, TRACK_CURVE_RIGHT_SMALL },
    { TRACK_CURVE_LEFT_SMALL, TRACK_CURVE_LEFT_SMALL },
    { TRACK_CURVE_RIGHT_SMALL, TRACK_CURVE_RIGHT_SMALL },
    { TRACK_CURVE_LEFT_SMALL, TRACK_CURVE_LEFT_SMALL },
    { TRACK_CURVE_RIGHT_SMALL, TRACK_CURVE_RIGHT_SMALL },
    { TRACK_CURVE_LEFT_SMALL, TRACK_CURVE_LEFT_SMALL },
    { TRACK_CURVE_RIGHT_SMALL, TRACK_CURVE_RIGHT_SMALL },
    { TRACK_CURVE_LEFT_VERY_SMALL, TRACK_CURVE_LEFT_VERY_SMALL },
    { TRACK_CURVE_RIGHT_VERY_SMALL, TRACK_CURVE_RIGHT_VERY_SMALL },
    { RideConstructionSpecialPieceSelected | TrackElemType::LeftTwistUpToDown, TRACK_CURVE_NONE },
    { RideConstructionSpecialPieceSelected | TrackElemType::RightTwistUpToDown, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, RideConstructionSpecialPieceSelected | TrackElemType::LeftTwistDownToUp },
    { TRACK_CURVE_NONE, RideConstructionSpecialPieceSelected | TrackElemType::RightTwistDownToUp },
    { RideConstructionSpecialPieceSelected | TrackElemType::HalfLoopDown, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, RideConstructionSpecialPieceSelected | TrackElemType::HalfLoopUp },
    { RideConstructionSpecialPieceSelected | TrackElemType::RightCorkscrewDown, TRACK_CURVE_NONE },
    { RideConstructionSpecialPieceSelected | TrackElemType::LeftCorkscrewDown, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, RideConstructionSpecialPieceSelected | TrackElemType::RightCorkscrewUp },
    { TRACK_CURVE_NONE, RideConstructionSpecialPieceSelected | TrackElemType::LeftCorkscrewUp },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { RideConstructionSpecialPieceSelected | TrackElemType::TowerSection, TRACK_CURVE_NONE },
    { RideConstructionSpecialPieceSelected | TrackElemType::TowerSection, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_LEFT, TRACK_CURVE_LEFT },
    { TRACK_CURVE_RIGHT, TRACK_CURVE_RIGHT },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_LEFT_SMALL, TRACK_CURVE_LEFT_SMALL },
    { TRACK_CURVE_RIGHT_SMALL, TRACK_CURVE_RIGHT_SMALL },
    { RideConstructionSpecialPieceSelected | TrackElemType::LeftHalfBankedHelixUpSmall, RideConstructionSpecialPieceSelected | TrackElemType::LeftHalfBankedHelixUpSmall },
    { RideConstructionSpecialPieceSelected | TrackElemType::RightHalfBankedHelixUpSmall, RideConstructionSpecialPieceSelected | TrackElemType::RightHalfBankedHelixUpSmall },
    { RideConstructionSpecialPieceSelected | TrackElemType::LeftHalfBankedHelixDownSmall, RideConstructionSpecialPieceSelected | TrackElemType::LeftHalfBankedHelixDownSmall },
    { RideConstructionSpecialPieceSelected | TrackElemType::RightHalfBankedHelixDownSmall, RideConstructionSpecialPieceSelected | TrackElemType::RightHalfBankedHelixDownSmall },
    { RideConstructionSpecialPieceSelected | TrackElemType::LeftHalfBankedHelixUpLarge, RideConstructionSpecialPieceSelected | TrackElemType::LeftHalfBankedHelixUpLarge },
    { RideConstructionSpecialPieceSelected | TrackElemType::RightHalfBankedHelixUpLarge, RideConstructionSpecialPieceSelected | TrackElemType::RightHalfBankedHelixUpLarge },
    { RideConstructionSpecialPieceSelected | TrackElemType::LeftHalfBankedHelixDownLarge, RideConstructionSpecialPieceSelected | TrackElemType::LeftHalfBankedHelixDownLarge },
    { RideConstructionSpecialPieceSelected | TrackElemType::RightHalfBankedHelixDownLarge, RideConstructionSpecialPieceSelected | TrackElemType::RightHalfBankedHelixDownLarge },
    { TRACK_CURVE_LEFT_SMALL, TRACK_CURVE_LEFT_SMALL },
    { TRACK_CURVE_RIGHT_SMALL, TRACK_CURVE_RIGHT_SMALL },
    { TRACK_CURVE_LEFT_SMALL, TRACK_CURVE_LEFT_SMALL },
    { TRACK_CURVE_RIGHT_SMALL, TRACK_CURVE_RIGHT_SMALL },
    { RideConstructionSpecialPieceSelected | TrackElemType::Brakes, RideConstructionSpecialPieceSelected | TrackElemType::Brakes },
    { RideConstructionSpecialPieceSelected | TrackElemType::Booster, RideConstructionSpecialPieceSelected | TrackElemType::Booster },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { RideConstructionSpecialPieceSelected | TrackElemType::LeftQuarterBankedHelixLargeUp, RideConstructionSpecialPieceSelected | TrackElemType::LeftQuarterBankedHelixLargeUp },
    { RideConstructionSpecialPieceSelected | TrackElemType::RightQuarterBankedHelixLargeUp, RideConstructionSpecialPieceSelected | TrackElemType::RightQuarterBankedHelixLargeUp },
    { RideConstructionSpecialPieceSelected | TrackElemType::LeftQuarterBankedHelixLargeDown, RideConstructionSpecialPieceSelected | TrackElemType::LeftQuarterBankedHelixLargeDown },
    { RideConstructionSpecialPieceSelected | TrackElemType::RightQuarterBankedHelixLargeDown, RideConstructionSpecialPieceSelected | TrackElemType::RightQuarterBankedHelixLargeDown },
    { RideConstructionSpecialPieceSelected | TrackElemType::LeftQuarterHelixLargeUp, RideConstructionSpecialPieceSelected | TrackElemType::LeftQuarterHelixLargeUp },
    { RideConstructionSpecialPieceSelected | TrackElemType::RightQuarterHelixLargeUp, RideConstructionSpecialPieceSelected | TrackElemType::RightQuarterHelixLargeUp },
    { RideConstructionSpecialPieceSelected | TrackElemType::LeftQuarterHelixLargeDown, RideConstructionSpecialPieceSelected | TrackElemType::LeftQuarterHelixLargeDown },
    { RideConstructionSpecialPieceSelected | TrackElemType::RightQuarterHelixLargeDown, RideConstructionSpecialPieceSelected | TrackElemType::RightQuarterHelixLargeDown },
    { TRACK_CURVE_LEFT, TRACK_CURVE_LEFT },
    { TRACK_CURVE_RIGHT, TRACK_CURVE_RIGHT },
    { RideConstructionSpecialPieceSelected | TrackElemType::Waterfall, RideConstructionSpecialPieceSelected | TrackElemType::Waterfall },
    { RideConstructionSpecialPieceSelected | TrackElemType::Rapids, RideConstructionSpecialPieceSelected | TrackElemType::Rapids },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_LEFT, TRACK_CURVE_LEFT },
    { TRACK_CURVE_RIGHT, TRACK_CURVE_RIGHT },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { RideConstructionSpecialPieceSelected | TrackElemType::ReverseFreefallVertical, TRACK_CURVE_NONE },
    { RideConstructionSpecialPieceSelected | TrackElemType::ReverseFreefallVertical, RideConstructionSpecialPieceSelected | TrackElemType::ReverseFreefallVertical },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_LEFT_LARGE, TRACK_CURVE_LEFT_LARGE },
    { TRACK_CURVE_RIGHT_LARGE, TRACK_CURVE_RIGHT_LARGE },
    { TRACK_CURVE_LEFT_LARGE, TRACK_CURVE_LEFT_LARGE },
    { TRACK_CURVE_RIGHT_LARGE, TRACK_CURVE_RIGHT_LARGE },
    { TRACK_CURVE_LEFT_LARGE, TRACK_CURVE_LEFT_LARGE },
    { TRACK_CURVE_RIGHT_LARGE, TRACK_CURVE_RIGHT_LARGE },
    { TRACK_CURVE_LEFT_LARGE, TRACK_CURVE_LEFT_LARGE },
    { TRACK_CURVE_RIGHT_LARGE, TRACK_CURVE_RIGHT_LARGE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_LEFT_LARGE, TRACK_CURVE_NONE },
    { TRACK_CURVE_RIGHT_LARGE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_LEFT_LARGE },
    { TRACK_CURVE_NONE, TRACK_CURVE_RIGHT_LARGE },
    { TRACK_CURVE_NONE, TRACK_CURVE_LEFT_LARGE },
    { TRACK_CURVE_NONE, TRACK_CURVE_RIGHT_LARGE },
    { TRACK_CURVE_LEFT_LARGE, TRACK_CURVE_NONE },
    { TRACK_CURVE_RIGHT_LARGE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_LEFT_LARGE },
    { TRACK_CURVE_NONE, TRACK_CURVE_RIGHT_LARGE },
    { TRACK_CURVE_LEFT_LARGE, TRACK_CURVE_NONE },
    { TRACK_CURVE_RIGHT_LARGE, TRACK_CURVE_NONE },
    { TRACK_CURVE_LEFT_LARGE, TRACK_CURVE_LEFT_LARGE },
    { TRACK_CURVE_RIGHT_LARGE, TRACK_CURVE_RIGHT_LARGE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { RideConstructionSpecialPieceSelected | TrackElemType::LeftBarrelRollDownToUp, TRACK_CURVE_NONE },
    { RideConstructionSpecialPieceSelected | TrackElemType::RightBarrelRollDownToUp, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, RideConstructionSpecialPieceSelected | TrackElemType::LeftBarrelRollUpToDown },
    { TRACK_CURVE_NONE, RideConstructionSpecialPieceSelected | TrackElemType::RightBarrelRollUpToDown },
    { TRACK_CURVE_LEFT_SMALL, TRACK_CURVE_LEFT_SMALL },
    { TRACK_CURVE_RIGHT_SMALL, TRACK_CURVE_RIGHT_SMALL },
    { TRACK_CURVE_LEFT_SMALL, TRACK_CURVE_LEFT_SMALL },
    { TRACK_CURVE_RIGHT_SMALL, TRACK_CURVE_RIGHT_SMALL },
    { RideConstructionSpecialPieceSelected | TrackElemType::PoweredLift, RideConstructionSpecialPieceSelected | TrackElemType::PoweredLift },
    { RideConstructionSpecialPieceSelected | TrackElemType::LeftLargeHalfLoopDown, TRACK_CURVE_NONE },
    { RideConstructionSpecialPieceSelected | TrackElemType::RightLargeHalfLoopDown, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, RideConstructionSpecialPieceSelected | TrackElemType::RightLargeHalfLoopUp },
    { TRACK_CURVE_NONE, RideConstructionSpecialPieceSelected | TrackElemType::LeftLargeHalfLoopUp },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { RideConstructionSpecialPieceSelected | TrackElemType::InvertedFlatToDown90QuarterLoop, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, RideConstructionSpecialPieceSelected | TrackElemType::Up90ToInvertedFlatQuarterLoop },
    { RideConstructionSpecialPieceSelected | TrackElemType::LeftCurvedLiftHill, RideConstructionSpecialPieceSelected | TrackElemType::LeftCurvedLiftHill },
    { RideConstructionSpecialPieceSelected | TrackElemType::RightCurvedLiftHill, RideConstructionSpecialPieceSelected | TrackElemType::RightCurvedLiftHill },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { RideConstructionSpecialPieceSelected | TrackElemType::AirThrustVerticalDown, RideConstructionSpecialPieceSelected | TrackElemType::ReverseFreefallVertical },
    { RideConstructionSpecialPieceSelected | TrackElemType::AirThrustVerticalDown, RideConstructionSpecialPieceSelected | TrackElemType::AirThrustVerticalDown },
    { TRACK_CURVE_NONE, RideConstructionSpecialPieceSelected | TrackElemType::AirThrustVerticalDown },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_LEFT_SMALL, TRACK_CURVE_LEFT_SMALL },
    { TRACK_CURVE_RIGHT_SMALL, TRACK_CURVE_RIGHT_SMALL },
    { TRACK_CURVE_LEFT_SMALL, TRACK_CURVE_LEFT_SMALL },
    { TRACK_CURVE_RIGHT_SMALL, TRACK_CURVE_RIGHT_SMALL },
    { TRACK_CURVE_LEFT, TRACK_CURVE_LEFT },
    { TRACK_CURVE_RIGHT, TRACK_CURVE_RIGHT },
    { TRACK_CURVE_LEFT, TRACK_CURVE_LEFT },
    { TRACK_CURVE_RIGHT, TRACK_CURVE_RIGHT },
    { TRACK_CURVE_LEFT, TRACK_CURVE_NONE },
    { TRACK_CURVE_RIGHT, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_LEFT },
    { TRACK_CURVE_NONE, TRACK_CURVE_RIGHT },
    { TRACK_CURVE_LEFT, TRACK_CURVE_NONE },
    { TRACK_CURVE_RIGHT, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_LEFT },
    { TRACK_CURVE_NONE, TRACK_CURVE_RIGHT },
    { TRACK_CURVE_LEFT, TRACK_CURVE_LEFT },
    { TRACK_CURVE_RIGHT, TRACK_CURVE_RIGHT },
    { TRACK_CURVE_LEFT, TRACK_CURVE_LEFT },
    { TRACK_CURVE_RIGHT, TRACK_CURVE_RIGHT },
    { TRACK_CURVE_LEFT, TRACK_CURVE_LEFT },
    { TRACK_CURVE_RIGHT, TRACK_CURVE_RIGHT },
    { TRACK_CURVE_LEFT, TRACK_CURVE_LEFT },
    { TRACK_CURVE_RIGHT, TRACK_CURVE_RIGHT },
    { TRACK_CURVE_LEFT, TRACK_CURVE_NONE },
    { TRACK_CURVE_RIGHT, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_LEFT },
    { TRACK_CURVE_NONE, TRACK_CURVE_RIGHT },
    { TRACK_CURVE_LEFT, TRACK_CURVE_NONE },
    { TRACK_CURVE_RIGHT, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_LEFT },
    { TRACK_CURVE_NONE, TRACK_CURVE_RIGHT },
    { TRACK_CURVE_LEFT_SMALL, TRACK_CURVE_LEFT_SMALL },
    { TRACK_CURVE_RIGHT_SMALL, TRACK_CURVE_RIGHT_SMALL },
    { TRACK_CURVE_LEFT_SMALL, TRACK_CURVE_LEFT_SMALL },
    { TRACK_CURVE_RIGHT_SMALL, TRACK_CURVE_RIGHT_SMALL },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { 65535, 65535 },                       // TrackElemType::FlatTrack1x4A
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE }, // TrackElemType::FlatTrack2x2
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE }, // TrackElemType::FlatTrack4x4
    { 65535, 65535 },                       // TrackElemType::FlatTrack2x4
    { 65535, 65535 },                       // TrackElemType::FlatTrack1x5
    { 65535, 65535 },                       // TrackElemType::FlatTrack1x1A
    { 65535, 65535 },                       // TrackElemType::FlatTrack1x4B
    { 65535, 65535 },                       // TrackElemType::FlatTrack1x1B
    { 65535, 65535 },                       // TrackElemType::FlatTrack1x4C
    { 65535, 65535 },                       // TrackElemType::FlatTrack3x3
    { RideConstructionSpecialPieceSelected | TrackElemType::RightLargeCorkscrewDown, TRACK_CURVE_NONE }, // TrackElemType::LeftLargeCorkscrewUp
    { RideConstructionSpecialPieceSelected | TrackElemType::LeftLargeCorkscrewDown, TRACK_CURVE_NONE },  // TrackElemType::RightLargeCorkscrewUp
    { TRACK_CURVE_NONE, RideConstructionSpecialPieceSelected | TrackElemType::RightLargeCorkscrewUp },   // TrackElemType::LeftLargeCorkscrewDown
    { TRACK_CURVE_NONE, RideConstructionSpecialPieceSelected | TrackElemType::LeftLargeCorkscrewUp },    // TrackElemType::RightCorkscrewDown
    { RideConstructionSpecialPieceSelected | TrackElemType::RightMediumHalfLoopDown, TRACK_CURVE_NONE }, // TrackElemType::LeftMediumHalfLoopUp
    { RideConstructionSpecialPieceSelected | TrackElemType::LeftMediumHalfLoopDown, TRACK_CURVE_NONE },  // TrackElemType::RightMediumHalfLoopUp
    { TRACK_CURVE_NONE, RideConstructionSpecialPieceSelected | TrackElemType::LeftMediumHalfLoopUp },    // TrackElemType::LeftMediumHalfLoopDown
    { TRACK_CURVE_NONE, RideConstructionSpecialPieceSelected | TrackElemType::RightMediumHalfLoopUp },   // TrackElemType::RightMediumHalfLoopDown
    { RideConstructionSpecialPieceSelected | TrackElemType::LeftZeroGRollDown, TRACK_CURVE_NONE },       // TrackElemType::LeftZeroGRollUp
    { RideConstructionSpecialPieceSelected | TrackElemType::RightZeroGRollDown, TRACK_CURVE_NONE },      // TrackElemType::RightZeroGRollUp
    { TRACK_CURVE_NONE, RideConstructionSpecialPieceSelected | TrackElemType::LeftZeroGRollUp },         // TrackElemType::LeftZeroGRollDown
    { TRACK_CURVE_NONE, RideConstructionSpecialPieceSelected | TrackElemType::RightZeroGRollUp },        // TrackElemType::RightZeroGRollDown
    { RideConstructionSpecialPieceSelected | TrackElemType::LeftLargeZeroGRollDown, TRACK_CURVE_NONE },  // TrackElemType::LeftLargeZeroGRollUp
    { RideConstructionSpecialPieceSelected | TrackElemType::RightLargeZeroGRollDown, TRACK_CURVE_NONE }, // TrackElemType::RightLargeZeroGRollUp
    { TRACK_CURVE_NONE, RideConstructionSpecialPieceSelected | TrackElemType::LeftLargeZeroGRollUp },    // TrackElemType::LeftLargeZeroGRollDown
    { TRACK_CURVE_NONE, RideConstructionSpecialPieceSelected | TrackElemType::RightLargeZeroGRollUp },   // TrackElemType::RightLargeZeroGRollDown
    { RideConstructionSpecialPieceSelected | TrackElemType::LeftFlyerLargeHalfLoopInvertedDown, TRACK_CURVE_NONE },    // LeftFlyerLargeHalfLoopUninvertedUp
    { RideConstructionSpecialPieceSelected | TrackElemType::RightFlyerLargeHalfLoopInvertedDown, TRACK_CURVE_NONE },   // RightFlyerLargeHalfLoopUninvertedUp
    { TRACK_CURVE_NONE, RideConstructionSpecialPieceSelected | TrackElemType::RightFlyerLargeHalfLoopUninvertedUp },   // RightFlyerLargeHalfLoopInvertedDown
    { TRACK_CURVE_NONE, RideConstructionSpecialPieceSelected | TrackElemType::LeftFlyerLargeHalfLoopUninvertedUp },    // LeftFlyerLargeHalfLoopInvertedDown
    { RideConstructionSpecialPieceSelected | TrackElemType::LeftFlyerLargeHalfLoopUninvertedDown, TRACK_CURVE_NONE},  // LeftFlyerLargeHalfLoopInvertedUp
    { RideConstructionSpecialPieceSelected | TrackElemType::RightFlyerLargeHalfLoopUninvertedDown, TRACK_CURVE_NONE }, // RightFlyerLargeHalfLoopInvertedUp
    { TRACK_CURVE_NONE, RideConstructionSpecialPieceSelected | TrackElemType::RightFlyerLargeHalfLoopInvertedUp }, // RightFlyerLargeHalfLoopUninvertedDown
    { TRACK_CURVE_NONE, RideConstructionSpecialPieceSelected | TrackElemType::LeftFlyerLargeHalfLoopInvertedUp }, // LeftFlyerLargeHalfLoopUninvertedDown
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE }, // FlyerHalfLoopInvertedUp
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE }, // FlyerHalfLoopUninvertedUp
};

const TrackDescriptor gTrackDescriptors[142] = {
    {   true,   TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TrackElemType::DiagDown60                                     },
    {   true,   TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TrackElemType::DiagDown60ToDown25                      },
    {   true,   TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TrackElemType::DiagDown60ToFlat                             },
    {   true,   TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TrackElemType::DiagDown25ToDown60                      },
    {   true,   TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TrackElemType::DiagDown25                                     },
    {   true,   TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TrackElemType::DiagDown25ToLeftBank                        },
    {   true,   TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TrackElemType::DiagDown25ToFlat                             },
    {   true,   TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TrackElemType::DiagDown25ToRightBank                       },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_CURVE_LEFT_LARGE,         TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TrackElemType::LeftEighthBankToOrthogonal                       },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TrackElemType::DiagLeftBankToDown25                        },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TrackElemType::DiagLeftBank                                       },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TrackElemType::DiagLeftBankToFlat                               },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TrackElemType::DiagLeftBankToUp25                          },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_LEFT_LARGE,         TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TrackElemType::LeftEighthToOrthogonal                            },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TrackElemType::DiagFlatToDown60                             },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TrackElemType::DiagFlatToDown25                             },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TrackElemType::DiagFlatToLeftBank                               },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TrackElemType::DiagFlat                                            },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TrackElemType::DiagFlatToRightBank                              },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TrackElemType::DiagFlatToUp25                               },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TrackElemType::DiagFlatToUp60                               },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_RIGHT_LARGE,        TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TrackElemType::RightEighthToOrthogonal                           },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TrackElemType::DiagRightBankToDown25                       },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TrackElemType::DiagRightBankToFlat                              },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TrackElemType::DiagRightBank                                      },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TrackElemType::DiagRightBankToUp25                         },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_CURVE_RIGHT_LARGE,        TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TrackElemType::RightEighthBankToOrthogonal                      },
    {   true,   TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TrackElemType::DiagUp25ToLeftBank                          },
    {   true,   TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TrackElemType::DiagUp25ToFlat                               },
    {   true,   TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TrackElemType::DiagUp25ToRightBank                         },
    {   true,   TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TrackElemType::DiagUp25                                       },
    {   true,   TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TrackElemType::DiagUp25ToUp60                          },
    {   true,   TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TrackElemType::DiagUp60ToFlat                               },
    {   true,   TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TrackElemType::DiagUp60ToUp25                          },
    {   true,   TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TrackElemType::DiagUp60                                       },
    {   false,  TRACK_SLOPE_DOWN_90,    TRACK_BANK_NONE,    TRACK_CURVE_LEFT_SMALL,         TRACK_SLOPE_DOWN_90,    TRACK_BANK_NONE,    TrackElemType::LeftQuarterTurn1TileDown90                 },
    {   false,  TRACK_SLOPE_DOWN_90,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_90,    TRACK_BANK_NONE,    TrackElemType::Down90                                          },
    {   false,  TRACK_SLOPE_DOWN_90,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TrackElemType::Down90ToDown60                           },
    {   false,  TRACK_SLOPE_DOWN_90,    TRACK_BANK_NONE,    TRACK_CURVE_RIGHT_SMALL,        TRACK_SLOPE_DOWN_90,    TRACK_BANK_NONE,    TrackElemType::RightQuarterTurn1TileDown90                },
    {   false,  TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TRACK_CURVE_LEFT_SMALL,         TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TrackElemType::LeftQuarterTurn1TileDown60                 },
    {   false,  TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_90,    TRACK_BANK_NONE,    TrackElemType::Down60ToDown90                           },
    {   false,  TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TrackElemType::Down60                                          },
    {   false,  TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TrackElemType::Down60ToDown25                           },
    {   false,  TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TrackElemType::Down60ToFlat                                  },
    {   false,  TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TRACK_CURVE_RIGHT_SMALL,        TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TrackElemType::RightQuarterTurn1TileDown60                },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_LEFT,    TRACK_CURVE_LEFT_SMALL,         TRACK_SLOPE_DOWN_25,    TRACK_BANK_LEFT,    TrackElemType::LeftBankedQuarterTurn3TileDown25          },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_LEFT,    TRACK_CURVE_LEFT,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_LEFT,    TrackElemType::LeftBankedQuarterTurn5TileDown25          },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_LEFT,    TrackElemType::Down25LeftBanked                              },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TrackElemType::LeftBankedDown25ToDown25               },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TrackElemType::LeftBankedDown25ToLeftBankedFlat          },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TrackElemType::LeftBankedDown25ToFlat                      },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_LEFT_SMALL,         TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TrackElemType::LeftQuarterTurn3TilesDown25                },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_LEFT_SMALL,         TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TrackElemType::LeftQuarterTurn3TilesDown25ToLeftBank   },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_LEFT,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TrackElemType::LeftQuarterTurn5TilesDown25                },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TrackElemType::Down25ToDown60                           },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_LEFT,    TrackElemType::Down25ToLeftBankedDown25               },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TrackElemType::Down25                                          },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_RIGHT,   TrackElemType::Down25ToRightBankedDown25              },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TrackElemType::Down25ToLeftBank                             },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TrackElemType::Down25ToFlat                                  },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TrackElemType::Down25ToRightBank                            },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_RIGHT,              TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TrackElemType::RightQuarterTurn5TilesDown25               },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_RIGHT_SMALL,        TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TrackElemType::RightQuarterTurn3TilesDown25ToRightBank },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_RIGHT_SMALL,        TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TrackElemType::RightQuarterTurn3TilesDown25               },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TrackElemType::RightBankedDown25ToDown25              },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_RIGHT,   TrackElemType::Down25RightBanked                             },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TrackElemType::RightBankedDown25ToFlat                     },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TrackElemType::RightBankedDown25ToRightBankedFlat        },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_RIGHT,   TRACK_CURVE_RIGHT,              TRACK_SLOPE_DOWN_25,    TRACK_BANK_RIGHT,   TrackElemType::RightBankedQuarterTurn5TileDown25         },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_RIGHT,   TRACK_CURVE_RIGHT_SMALL,        TRACK_SLOPE_DOWN_25,    TRACK_BANK_RIGHT,   TrackElemType::RightBankedQuarterTurn3TileDown25         },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_CURVE_LEFT_SMALL,         TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TrackElemType::LeftBankedQuarterTurn3Tiles                       },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_CURVE_LEFT_SMALL,         TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TrackElemType::LeftBankToLeftQuarterTurn3TilesUp25     },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_CURVE_LEFT,               TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TrackElemType::BankedLeftQuarterTurn5Tiles                     },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_CURVE_LEFT_LARGE,         TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TrackElemType::LeftEighthBankToDiag                             },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_LEFT,    TrackElemType::LeftBankedFlatToLeftBankedDown25          },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TrackElemType::LeftBankToDown25                             },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TrackElemType::LeftBank                                            },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TrackElemType::LeftBankToFlat                                    },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_LEFT,    TrackElemType::LeftBankedFlatToLeftBankedUp25            },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TrackElemType::LeftBankToUp25                               },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_LEFT_SMALL,         TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TrackElemType::LeftQuarterTurn3Tiles                            },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_LEFT,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TrackElemType::LeftQuarterTurn5Tiles                            },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_LEFT_LARGE,         TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TrackElemType::LeftEighthToDiag                                  },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TrackElemType::FlatToDown60                                  },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_LEFT,    TrackElemType::FlatToLeftBankedDown25                      },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TrackElemType::FlatToDown25                                  },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_RIGHT,   TrackElemType::FlatToRightBankedDown25                     },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TrackElemType::FlatToLeftBank                                    },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TrackElemType::Flat                                                 },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_LEFT_VERY_SMALL,    TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TrackElemType::LeftQuarterTurn1Tile                             },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_RIGHT_VERY_SMALL,   TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TrackElemType::RightQuarterTurn1Tile                            },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TrackElemType::FlatToRightBank                                   },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_LEFT,    TrackElemType::FlatToLeftBankedUp25                        },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TrackElemType::FlatToUp25                                    },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_RIGHT,   TrackElemType::FlatToRightBankedUp25                       },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TrackElemType::FlatToUp60                                    },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_RIGHT_LARGE,        TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TrackElemType::RightEighthToDiag                                 },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_RIGHT,              TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TrackElemType::RightQuarterTurn5Tiles                           },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_RIGHT_SMALL,        TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TrackElemType::RightQuarterTurn3Tiles                           },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TrackElemType::RightBankToDown25                            },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_RIGHT,   TrackElemType::RightBankedFlatToRightBankedDown25        },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TrackElemType::RightBankToFlat                                   },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TrackElemType::RightBank                                           },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TrackElemType::RightBankToUp25                              },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_RIGHT,   TrackElemType::RightBankedFlatToRightBankedUp25          },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_CURVE_RIGHT_LARGE,        TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TrackElemType::RightEighthBankToDiag                            },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_CURVE_RIGHT,              TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TrackElemType::BankedRightQuarterTurn5Tiles                    },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_CURVE_RIGHT_SMALL,        TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TrackElemType::RightBankedQuarterTurn3Tiles                      },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_CURVE_RIGHT_SMALL,        TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TrackElemType::RightBankToRightQuarterTurn3TilesUp25   },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_LEFT,    TRACK_CURVE_LEFT_SMALL,         TRACK_SLOPE_UP_25,      TRACK_BANK_LEFT,    TrackElemType::LeftBankedQuarterTurn3TileUp25            },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_LEFT,    TRACK_CURVE_LEFT,               TRACK_SLOPE_UP_25,      TRACK_BANK_LEFT,    TrackElemType::LeftBankedQuarterTurn5TileUp25            },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TrackElemType::LeftBankedUp25ToLeftBankedFlat            },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TrackElemType::LeftBankedUp25ToFlat                        },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_LEFT,    TrackElemType::Up25LeftBanked                                },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TrackElemType::LeftBankedUp25ToUp25                   },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_CURVE_LEFT_SMALL,         TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TrackElemType::LeftQuarterTurn3TilesUp25                  },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_CURVE_LEFT,               TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TrackElemType::LeftQuarterTurn5TilesUp25                  },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TrackElemType::Up25ToLeftBank                               },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TrackElemType::Up25ToFlat                                    },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TrackElemType::Up25ToRightBank                              },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_LEFT,    TrackElemType::Up25ToLeftBankedUp25                   },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TrackElemType::Up25                                            },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_RIGHT,   TrackElemType::Up25ToRightBankedUp25                  },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TrackElemType::Up25ToUp60                               },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_CURVE_RIGHT,              TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TrackElemType::RightQuarterTurn5TilesUp25                 },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_CURVE_RIGHT_SMALL,        TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TrackElemType::RightQuarterTurn3TilesUp25                 },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TrackElemType::RightBankedUp25ToFlat                       },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TrackElemType::RightBankedUp25ToRightBankedFlat          },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TrackElemType::RightBankedUp25ToUp25                  },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_RIGHT,   TrackElemType::Up25RightBanked                               },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_RIGHT,   TRACK_CURVE_RIGHT,              TRACK_SLOPE_UP_25,      TRACK_BANK_RIGHT,   TrackElemType::RightBankedQuarterTurn5TileUp25           },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_RIGHT,   TRACK_CURVE_RIGHT_SMALL,        TRACK_SLOPE_UP_25,      TRACK_BANK_RIGHT,   TrackElemType::RightBankedQuarterTurn3TileUp25           },
    {   false,  TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TRACK_CURVE_LEFT_SMALL,         TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TrackElemType::LeftQuarterTurn1TileUp60                   },
    {   false,  TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TrackElemType::Up60ToFlat                                    },
    {   false,  TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TrackElemType::Up60ToUp25                               },
    {   false,  TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TrackElemType::Up60                                            },
    {   false,  TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_90,      TRACK_BANK_NONE,    TrackElemType::Up60ToUp90                               },
    {   false,  TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TRACK_CURVE_RIGHT_SMALL,        TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TrackElemType::RightQuarterTurn1TileUp60                  },
    {   false,  TRACK_SLOPE_UP_90,      TRACK_BANK_NONE,    TRACK_CURVE_LEFT_SMALL,         TRACK_SLOPE_UP_90,      TRACK_BANK_NONE,    TrackElemType::LeftQuarterTurn1TileUp90                   },
    {   false,  TRACK_SLOPE_UP_90,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TrackElemType::Up90ToUp60                               },
    {   false,  TRACK_SLOPE_UP_90,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_90,      TRACK_BANK_NONE,    TrackElemType::Up90                                            },
    {   false,  TRACK_SLOPE_UP_90,      TRACK_BANK_NONE,    TRACK_CURVE_RIGHT_SMALL,        TRACK_SLOPE_UP_90,      TRACK_BANK_NONE,    TrackElemType::RightQuarterTurn1TileUp90                  },
};

/** rct2: 0x00993D1C */
static constexpr track_type_t AlternativeTrackTypes[TrackElemType::Count] = {
    TrackElemType::FlatCovered,                        // TrackElemType::Flat
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::Up25Covered,                   // TrackElemType::Up25
    TrackElemType::Up60Covered,                   // TrackElemType::Up60
    TrackElemType::FlatToUp25Covered,           // TrackElemType::FlatToUp25
    TrackElemType::Up25ToUp60Covered,      // TrackElemType::Up25ToUp60
    TrackElemType::Up60ToUp25Covered,      // TrackElemType::Up60ToUp25
    TrackElemType::Up25ToFlatCovered,           // TrackElemType::Up25ToFlat
    TrackElemType::Down25Covered,                 // TrackElemType::Down25
    TrackElemType::Down60Covered,                 // TrackElemType::Down60
    TrackElemType::FlatToDown25Covered,         // TrackElemType::FlatToDown25
    TrackElemType::Down25ToDown60Covered,  // TrackElemType::Down25ToDown60
    TrackElemType::Down60ToDown25Covered,  // TrackElemType::Down60ToDown25
    TrackElemType::Down25ToFlatCovered,         // TrackElemType::Down25ToFlat
    TrackElemType::LeftQuarterTurn5TilesCovered,   // TrackElemType::LeftQuarterTurn5Tiles
    TrackElemType::RightQuarterTurn5TilesCovered,  // TrackElemType::RightQuarterTurn5Tiles
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::SBendLeftCovered,                 // TrackElemType::SBendLeft
    TrackElemType::SBendRightCovered,                // TrackElemType::SBendRight
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::LeftQuarterTurn3TilesCovered,   // TrackElemType::LeftQuarterTurn3Tiles
    TrackElemType::RightQuarterTurn3TilesCovered,  // TrackElemType::RightQuarterTurn3Tiles
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None,
    TrackElemType::None, // TrackElemType::FlatTrack1x4A
    TrackElemType::None, // TrackElemType::FlatTrack2x2
    TrackElemType::None, // TrackElemType::FlatTrack4x4
    TrackElemType::None, // TrackElemType::FlatTrack2x4
    TrackElemType::None, // TrackElemType::FlatTrack1x5
    TrackElemType::None, // TrackElemType::FlatTrack1x1A
    TrackElemType::None, // TrackElemType::FlatTrack1x4B
    TrackElemType::None, // TrackElemType::FlatTrack1x1B
    TrackElemType::None, // TrackElemType::FlatTrack1x4C
    TrackElemType::None, // TrackElemType::FlatTrack3x3
    TrackElemType::None, // TrackElemType::LeftLargeCorkscrewUp
    TrackElemType::None, // TrackElemType::RightLargeCorkscrewUp
    TrackElemType::None, // TrackElemType::LeftLargeCorkscrewDown
    TrackElemType::None, // TrackElemType::RightCorkscrewDown
    TrackElemType::None, // TrackElemType::LeftMediumHalfLoopUp
    TrackElemType::None, // TrackElemType::RightMediumHalfLoopUp
    TrackElemType::None, // TrackElemType::LeftMediumHalfLoopDown
    TrackElemType::None, // TrackElemType::RightMediumHalfLoopDown
    TrackElemType::None, // TrackElemType::LeftZeroGRollUp
    TrackElemType::None, // TrackElemType::RightZeroGRollUp
    TrackElemType::None, // TrackElemType::LeftZeroGRollDown
    TrackElemType::None, // TrackElemType::RightZeroGRollDown
    TrackElemType::None, // TrackElemType::LeftLargeZeroGRollUp
    TrackElemType::None, // TrackElemType::RightLargeZeroGRollUp
    TrackElemType::None, // TrackElemType::LeftLargeZeroGRollDown
    TrackElemType::None, // TrackElemType::RightLargeZeroGRollDown
    TrackElemType::None, // LeftFlyerLargeHalfLoopUninvertedUp
    TrackElemType::None, // RightFlyerLargeHalfLoopUninvertedUp
    TrackElemType::None, // RightFlyerLargeHalfLoopInvertedDown
    TrackElemType::None, // LeftFlyerLargeHalfLoopInvertedDown
    TrackElemType::None, // FlyerHalfLoopInvertedUp
    TrackElemType::None, // FlyerHalfLoopUninvertedDown
};

/** rct2: 0x0099DA34 */
static constexpr money32 TrackPricing[TrackElemType::Count] = {
    65536,  // TrackElemType::Flat
    98304,  // TrackElemType::EndStation
    98304,  // TrackElemType::BeginStation
    98304,  // TrackElemType::MiddleStation
    79872,  // TrackElemType::Up25
    114688, // TrackElemType::Up60
    73728,  // TrackElemType::FlatToUp25
    96256,  // TrackElemType::Up25ToUp60
    96256,  // TrackElemType::Up60ToUp25
    73728,  // TrackElemType::Up25ToFlat
    79872,  // TrackElemType::Down25
    114688, // TrackElemType::Down60
    73728,  // TrackElemType::FlatToDown25
    96256,  // TrackElemType::Down25ToDown60
    96256,  // TrackElemType::Down60ToDown25
    73728,  // TrackElemType::Down25ToFlat
    257359, // TrackElemType::LeftQuarterTurn5Tiles
    257359, // TrackElemType::RightQuarterTurn5Tiles
    69632,  // TrackElemType::FlatToLeftBank
    69632,  // TrackElemType::FlatToRightBank
    69632,  // TrackElemType::LeftBankToFlat
    69632,  // TrackElemType::RightBankToFlat
    273443, // TrackElemType::BankedLeftQuarterTurn5Tiles
    273443, // TrackElemType::BankedRightQuarterTurn5Tiles
    78336,  // TrackElemType::LeftBankToUp25
    78336,  // TrackElemType::RightBankToUp25
    78336,  // TrackElemType::Up25ToLeftBank
    78336,  // TrackElemType::Up25ToRightBank
    78336,  // TrackElemType::LeftBankToDown25
    78336,  // TrackElemType::RightBankToDown25
    78336,  // TrackElemType::Down25ToLeftBank
    78336,  // TrackElemType::Down25ToRightBank
    69632,  // TrackElemType::LeftBank
    69632,  // TrackElemType::RightBank
    313656, // TrackElemType::LeftQuarterTurn5TilesUp25
    313656, // TrackElemType::RightQuarterTurn5TilesUp25
    313656, // TrackElemType::LeftQuarterTurn5TilesDown25
    313656, // TrackElemType::RightQuarterTurn5TilesDown25
    229376, // TrackElemType::SBendLeft
    229376, // TrackElemType::SBendRight
    491520, // TrackElemType::LeftVerticalLoop
    491520, // TrackElemType::RightVerticalLoop
    154415, // TrackElemType::LeftQuarterTurn3Tiles
    154415, // TrackElemType::RightQuarterTurn3Tiles
    164065, // TrackElemType::LeftBankedQuarterTurn3Tiles
    164065, // TrackElemType::RightBankedQuarterTurn3Tiles
    270226, // TrackElemType::LeftQuarterTurn3TilesUp25
    270226, // TrackElemType::RightQuarterTurn3TilesUp25
    270226, // TrackElemType::LeftQuarterTurn3TilesDown25
    270226, // TrackElemType::RightQuarterTurn3TilesDown25
    51471,  // TrackElemType::LeftQuarterTurn1Tile
    51471,  // TrackElemType::RightQuarterTurn1Tile
    212992, // TrackElemType::LeftTwistDownToUp
    212992, // TrackElemType::RightTwistDownToUp
    212992, // TrackElemType::LeftTwistUpToDown
    212992, // TrackElemType::RightTwistUpToDown
    294912, // TrackElemType::HalfLoopUp
    294912, // TrackElemType::HalfLoopDown
    229376, // TrackElemType::LeftCorkscrewUp
    229376, // TrackElemType::RightCorkscrewUp
    229376, // TrackElemType::LeftCorkscrewDown
    229376, // TrackElemType::RightCorkscrewDown
    98304,  // TrackElemType::FlatToUp60
    98304,  // TrackElemType::Up60ToFlat
    98304,  // TrackElemType::FlatToDown60
    98304,  // TrackElemType::Down60ToFlat
    524288, // TrackElemType::TowerBase
    65536,  // TrackElemType::TowerSection
    69632,  // TrackElemType::FlatCovered
    83968,  // TrackElemType::Up25Covered
    118784, // TrackElemType::Up60Covered
    77824,  // TrackElemType::FlatToUp25Covered
    100352, // TrackElemType::Up25ToUp60Covered
    100352, // TrackElemType::Up60ToUp25Covered
    77824,  // TrackElemType::Up25ToFlatCovered
    83968,  // TrackElemType::Down25Covered
    118784, // TrackElemType::Down60Covered
    77824,  // TrackElemType::FlatToDown25Covered
    100352, // TrackElemType::Down25ToDown60Covered
    100352, // TrackElemType::Down60ToDown25Covered
    77824,  // TrackElemType::Down25ToFlatCovered
    261455, // TrackElemType::LeftQuarterTurn5TilesCovered
    261455, // TrackElemType::RightQuarterTurn5TilesCovered
    233472, // TrackElemType::SBendLeftCovered
    233472, // TrackElemType::SBendRightCovered
    158511, // TrackElemType::LeftQuarterTurn3TilesCovered
    158511, // TrackElemType::RightQuarterTurn3TilesCovered
    328192, // TrackElemType::LeftHalfBankedHelixUpSmall
    328192, // TrackElemType::RightHalfBankedHelixUpSmall
    328192, // TrackElemType::LeftHalfBankedHelixDownSmall
    328192, // TrackElemType::RightHalfBankedHelixDownSmall
    547072, // TrackElemType::LeftHalfBankedHelixUpLarge
    547072, // TrackElemType::RightHalfBankedHelixUpLarge
    547072, // TrackElemType::LeftHalfBankedHelixDownLarge
    547072, // TrackElemType::RightHalfBankedHelixDownLarge
    126976, // TrackElemType::LeftQuarterTurn1TileUp60
    126976, // TrackElemType::RightQuarterTurn1TileUp60
    126976, // TrackElemType::LeftQuarterTurn1TileDown60
    126976, // TrackElemType::RightQuarterTurn1TileDown60
    90112,  // TrackElemType::Brakes
    77824,  // TrackElemType::Booster
    65536,  // TrackElemType::Maze
    273539, // TrackElemType::LeftQuarterBankedHelixLargeUp
    273539, // TrackElemType::RightQuarterBankedHelixLargeUp
    273539, // TrackElemType::LeftQuarterBankedHelixLargeDown
    273539, // TrackElemType::RightQuarterBankedHelixLargeDown
    273539, // TrackElemType::LeftQuarterHelixLargeUp
    273539, // TrackElemType::RightQuarterHelixLargeUp
    273539, // TrackElemType::LeftQuarterHelixLargeDown
    273539, // TrackElemType::RightQuarterHelixLargeDown
    83968,  // TrackElemType::Up25LeftBanked
    83968,  // TrackElemType::Up25RightBanked
    143360, // TrackElemType::Waterfall
    118784, // TrackElemType::Rapids
    196608, // TrackElemType::OnRidePhoto
    83968,  // TrackElemType::Down25LeftBanked
    83968,  // TrackElemType::Down25RightBanked
    393216, // TrackElemType::Watersplash
    376832, // TrackElemType::FlatToUp60LongBase
    376832, // TrackElemType::Up60ToFlatLongBase
    126976, // TrackElemType::Whirlpool
    376832, // TrackElemType::Down60ToFlatLongBase
    376832, // TrackElemType::FlatToDown60LongBase
    643072, // TrackElemType::CableLiftHill
    491520, // TrackElemType::ReverseFreefallSlope
    65536,  // TrackElemType::ReverseFreefallVertical
    69632,  // TrackElemType::Up90
    69632,  // TrackElemType::Down90
    114688, // TrackElemType::Up60ToUp90
    114688, // TrackElemType::Down90ToDown60
    114688, // TrackElemType::Up90ToUp60
    114688, // TrackElemType::Down60ToDown90
    147456, // TrackElemType::BrakeForDrop
    180151, // TrackElemType::LeftEighthToDiag
    180151, // TrackElemType::RightEighthToDiag
    180151, // TrackElemType::LeftEighthToOrthogonal
    180151, // TrackElemType::RightEighthToOrthogonal
    190960, // TrackElemType::LeftEighthBankToDiag
    190960, // TrackElemType::RightEighthBankToDiag
    190960, // TrackElemType::LeftEighthBankToOrthogonal
    190960, // TrackElemType::RightEighthBankToOrthogonal
    92681,  // TrackElemType::DiagFlat
    111820, // TrackElemType::DiagUp25
    147456, // TrackElemType::DiagUp60
    102400, // TrackElemType::DiagFlatToUp25
    129024, // TrackElemType::DiagUp25ToUp60
    129024, // TrackElemType::DiagUp60ToUp25
    102400, // TrackElemType::DiagUp25ToFlat
    111820, // TrackElemType::DiagDown25
    147456, // TrackElemType::DiagDown60
    102400, // TrackElemType::DiagFlatToDown25
    129024, // TrackElemType::DiagDown25ToDown60
    129024, // TrackElemType::DiagDown60ToDown25
    102400, // TrackElemType::DiagDown25ToFlat
    107008, // TrackElemType::DiagFlatToUp60
    107008, // TrackElemType::DiagUp60ToFlat
    107008, // TrackElemType::DiagFlatToDown60
    107008, // TrackElemType::DiagDown60ToFlat
    96777,  // TrackElemType::DiagFlatToLeftBank
    96777,  // TrackElemType::DiagFlatToRightBank
    96777,  // TrackElemType::DiagLeftBankToFlat
    96777,  // TrackElemType::DiagRightBankToFlat
    106496, // TrackElemType::DiagLeftBankToUp25
    106496, // TrackElemType::DiagRightBankToUp25
    106496, // TrackElemType::DiagUp25ToLeftBank
    106496, // TrackElemType::DiagUp25ToRightBank
    106496, // TrackElemType::DiagLeftBankToDown25
    106496, // TrackElemType::DiagRightBankToDown25
    106496, // TrackElemType::DiagDown25ToLeftBank
    106496, // TrackElemType::DiagDown25ToRightBank
    96777,  // TrackElemType::DiagLeftBank
    96777,  // TrackElemType::DiagRightBank
    282624, // TrackElemType::LogFlumeReverser
    217088, // TrackElemType::SpinningTunnel
    221184, // TrackElemType::LeftBarrelRollUpToDown
    221184, // TrackElemType::RightBarrelRollUpToDown
    221184, // TrackElemType::LeftBarrelRollDownToUp
    221184, // TrackElemType::RightBarrelRollDownToUp
    270226, // TrackElemType::LeftBankToLeftQuarterTurn3TilesUp25
    270226, // TrackElemType::RightBankToRightQuarterTurn3TilesUp25
    270226, // TrackElemType::LeftQuarterTurn3TilesDown25ToLeftBank
    270226, // TrackElemType::RightQuarterTurn3TilesDown25ToRightBank
    96384,  // TrackElemType::PoweredLift
    884736, // TrackElemType::LeftLargeHalfLoopUp
    884736, // TrackElemType::RightLargeHalfLoopUp
    884736, // TrackElemType::RightLargeHalfLoopDown
    884736, // TrackElemType::LeftLargeHalfLoopDown
    221184, // TrackElemType::LeftFlyerTwistUp
    221184, // TrackElemType::RightFlyerTwistUp
    221184, // TrackElemType::LeftFlyerTwistDown
    221184, // TrackElemType::RightFlyerTwistDown
    294912, // TrackElemType::FlyerHalfLoopUninvertedUp
    294912, // TrackElemType::FlyerHalfLoopInvertedDown
    229376, // TrackElemType::LeftFlyerCorkscrewUp
    229376, // TrackElemType::RightFlyerCorkscrewUp
    229376, // TrackElemType::LeftFlyerCorkscrewDown
    229376, // TrackElemType::RightFlyerCorkscrewDown
    294912, // TrackElemType::HeartLineTransferUp
    294912, // TrackElemType::HeartLineTransferDown
    417792, // TrackElemType::LeftHeartLineRoll
    417792, // TrackElemType::RightHeartLineRoll
    159744, // TrackElemType::MinigolfHoleA
    167936, // TrackElemType::MinigolfHoleB
    172032, // TrackElemType::MinigolfHoleC
    184320, // TrackElemType::MinigolfHoleD
    184320, // TrackElemType::MinigolfHoleE
    360448, // TrackElemType::MultiDimInvertedFlatToDown90QuarterLoop
    360448, // TrackElemType::Up90ToInvertedFlatQuarterLoop
    360448, // TrackElemType::InvertedFlatToDown90QuarterLoop
    270226, // TrackElemType::LeftCurvedLiftHill
    270226, // TrackElemType::RightCurvedLiftHill
    393216, // TrackElemType::LeftReverser
    393216, // TrackElemType::RightReverser
    65536,  // TrackElemType::AirThrustTopCap
    65536,  // TrackElemType::AirThrustVerticalDown
    491520, // TrackElemType::AirThrustVerticalDownToLevel
    94208,  // TrackElemType::BlockBrakes
    287115, // TrackElemType::LeftBankedQuarterTurn3TileUp25
    287115, // TrackElemType::RightBankedQuarterTurn3TileUp25
    287115, // TrackElemType::LeftBankedQuarterTurn3TileDown25
    287115, // TrackElemType::RightBankedQuarterTurn3TileDown25
    333259, // TrackElemType::LeftBankedQuarterTurn5TileUp25
    333259, // TrackElemType::RightBankedQuarterTurn5TileUp25
    333259, // TrackElemType::LeftBankedQuarterTurn5TileDown25
    333259, // TrackElemType::RightBankedQuarterTurn5TileDown25
    83968,  // TrackElemType::Up25ToLeftBankedUp25
    83968,  // TrackElemType::Up25ToRightBankedUp25
    83968,  // TrackElemType::LeftBankedUp25ToUp25
    83968,  // TrackElemType::RightBankedUp25ToUp25
    83968,  // TrackElemType::Down25ToLeftBankedDown25
    83968,  // TrackElemType::Down25ToRightBankedDown25
    83968,  // TrackElemType::LeftBankedDown25ToDown25
    83968,  // TrackElemType::RightBankedDown25ToDown25
    77824,  // TrackElemType::LeftBankedFlatToLeftBankedUp25
    77824,  // TrackElemType::RightBankedFlatToRightBankedUp25
    77824,  // TrackElemType::LeftBankedUp25ToLeftBankedFlat
    77824,  // TrackElemType::RightBankedUp25ToRightBankedFlat
    77824,  // TrackElemType::LeftBankedFlatToLeftBankedDown25
    77824,  // TrackElemType::RightBankedFlatToRightBankedDown25
    77824,  // TrackElemType::LeftBankedDown25ToLeftBankedFlat
    77824,  // TrackElemType::RightBankedDown25ToRightBankedFlat
    77824,  // TrackElemType::FlatToLeftBankedUp25
    77824,  // TrackElemType::FlatToRightBankedUp25
    77824,  // TrackElemType::LeftBankedUp25ToFlat
    77824,  // TrackElemType::RightBankedUp25ToFlat
    77824,  // TrackElemType::FlatToLeftBankedDown25
    77824,  // TrackElemType::FlatToRightBankedDown25
    77824,  // TrackElemType::LeftBankedDown25ToFlat
    77824,  // TrackElemType::RightBankedDown25ToFlat
    151552, // TrackElemType::LeftQuarterTurn1TileUp90
    151552, // TrackElemType::RightQuarterTurn1TileUp90
    151552, // TrackElemType::LeftQuarterTurn1TileDown90
    151552, // TrackElemType::RightQuarterTurn1TileDown90
    360448, // TrackElemType::MultiDimUp90ToInvertedFlatQuarterLoop
    360448, // TrackElemType::MultiDimFlatToDown90QuarterLoop
    360448, // TrackElemType::MultiDimInvertedUp90ToFlatQuarterLoop
    77824,  // TrackElemType::RotationControlToggle
    262144, // TrackElemType::FlatTrack1x4A
    262144, // TrackElemType::FlatTrack2x2
    1048576,// TrackElemType::FlatTrack4x4
    524288, // TrackElemType::FlatTrack2x4
    327680, // TrackElemType::FlatTrack1x5
    65536,  // TrackElemType::FlatTrack1x1A
    262144, // TrackElemType::FlatTrack1x4B
    65536,  // TrackElemType::FlatTrack1x1B
    262144, // TrackElemType::FlatTrack1x4C
    524288, // TrackElemType::FlatTrack3x3
    360448, // TrackElemType::LeftLargeCorkscrewUp
    360448, // TrackElemType::RightLargeCorkscrewUp
    360448, // TrackElemType::LeftLargeCorkscrewDown
    360448, // TrackElemType::RightCorkscrewDown
    589824, // TrackElemType::LeftMediumHalfLoopUp
    589824, // TrackElemType::RightMediumHalfLoopUp
    589824, // TrackElemType::LeftMediumHalfLoopDown
    589824, // TrackElemType::RightMediumHalfLoopDown
    229376, // TrackElemType::LeftZeroGRollUp
    229376, // TrackElemType::RightZeroGRollUp
    229376, // TrackElemType::LeftZeroGRollDown
    229376, // TrackElemType::RightZeroGRollDown
    425984, // TrackElemType::LeftLargeZeroGRollUp
    425984, // TrackElemType::RightLargeZeroGRollUp
    425984, // TrackElemType::LeftLargeZeroGRollDown
    425984, // TrackElemType::RightLargeZeroGRollDown
    884736, // TrackElemType::LeftFlyerLargeHalfLoopUninvertedUp
    884736, // TrackElemType::RightFlyerLargeHalfLoopUninvertedUp
    884736, // TrackElemType::RightFlyerLargeHalfLoopInvertedDown
    884736, // TrackElemType::LeftFlyerLargeHalfLoopInvertedDown
    884736, // TrackElemType::LeftFlyerLargeHalfLoopInvertedUp
    884736, // TrackElemType::RightFlyerLargeHalfLoopInvertedUp
    884736, // TrackElemType::RightFlyerLargeHalfLoopUnivertedDown
    884736, // TrackElemType::LeftFlyerLargeHalfLoopUninvertedDown
    294912, // TrackElemType::FlyerHalfLoopInvertedUp
    294912, // TrackElemType::FlyerHalfLoopUninvertedDown
};

/** rct2: 0x0099EA1C */
static constexpr track_type_t TrackElementMirrorMap[TrackElemType::Count] = {
    TrackElemType::Flat,
    TrackElemType::EndStation,
    TrackElemType::BeginStation,
    TrackElemType::MiddleStation,
    TrackElemType::Up25,
    TrackElemType::Up60,
    TrackElemType::FlatToUp25,
    TrackElemType::Up25ToUp60,
    TrackElemType::Up60ToUp25,
    TrackElemType::Up25ToFlat,
    TrackElemType::Down25,
    TrackElemType::Down60,
    TrackElemType::FlatToDown25,
    TrackElemType::Down25ToDown60,
    TrackElemType::Down60ToDown25,
    TrackElemType::Down25ToFlat,
    TrackElemType::RightQuarterTurn5Tiles, // TrackElemType::LeftQuarterTurn5Tiles
    TrackElemType::LeftQuarterTurn5Tiles, // TrackElemType::RightQuarterTurn5Tiles
    TrackElemType::FlatToRightBank, // TrackElemType::FlatToLeftBank
    TrackElemType::FlatToLeftBank, // TrackElemType::FlatToRightBank
    TrackElemType::RightBankToFlat, // TrackElemType::LeftBankToFlat
    TrackElemType::LeftBankToFlat, // TrackElemType::RightBankToFlat
    TrackElemType::BankedRightQuarterTurn5Tiles, // TrackElemType::BankedLeftQuarterTurn5Tiles
    TrackElemType::BankedLeftQuarterTurn5Tiles, // TrackElemType::BankedRightQuarterTurn5Tiles
    TrackElemType::RightBankToUp25, // TrackElemType::LeftBankToUp25
    TrackElemType::LeftBankToUp25, // TrackElemType::RightBankToUp25
    TrackElemType::Up25ToRightBank, // TrackElemType::Up25ToLeftBank
    TrackElemType::Up25ToLeftBank, // TrackElemType::Up25ToRightBank
    TrackElemType::RightBankToDown25, // TrackElemType::LeftBankToDown25
    TrackElemType::LeftBankToDown25, // TrackElemType::RightBankToDown25
    TrackElemType::Down25ToRightBank, // TrackElemType::Down25ToLeftBank
    TrackElemType::Down25ToLeftBank, // TrackElemType::Down25ToRightBank
    TrackElemType::RightBank, // TrackElemType::LeftBank
    TrackElemType::LeftBank, // TrackElemType::RightBank
    TrackElemType::RightQuarterTurn5TilesUp25, // TrackElemType::LeftQuarterTurn5TilesUp25
    TrackElemType::LeftQuarterTurn5TilesUp25, // TrackElemType::RightQuarterTurn5TilesUp25
    TrackElemType::RightQuarterTurn5TilesDown25, // TrackElemType::LeftQuarterTurn5TilesDown25
    TrackElemType::LeftQuarterTurn5TilesDown25, // TrackElemType::RightQuarterTurn5TilesDown25
    TrackElemType::SBendRight, // TrackElemType::SBendLeft
    TrackElemType::SBendLeft, // TrackElemType::SBendRight
    TrackElemType::RightVerticalLoop, // TrackElemType::LeftVerticalLoop
    TrackElemType::LeftVerticalLoop, // TrackElemType::RightVerticalLoop
    TrackElemType::RightQuarterTurn3Tiles, // TrackElemType::LeftQuarterTurn3Tiles
    TrackElemType::LeftQuarterTurn3Tiles, // TrackElemType::RightQuarterTurn3Tiles
    TrackElemType::RightBankedQuarterTurn3Tiles, // TrackElemType::LeftBankedQuarterTurn3Tiles
    TrackElemType::LeftBankedQuarterTurn3Tiles, // TrackElemType::RightBankedQuarterTurn3Tiles
    TrackElemType::RightQuarterTurn3TilesUp25, // TrackElemType::LeftQuarterTurn3TilesUp25
    TrackElemType::LeftQuarterTurn3TilesUp25, // TrackElemType::RightQuarterTurn3TilesUp25
    TrackElemType::RightQuarterTurn3TilesDown25, // TrackElemType::LeftQuarterTurn3TilesDown25
    TrackElemType::LeftQuarterTurn3TilesDown25, // TrackElemType::RightQuarterTurn3TilesDown25
    TrackElemType::RightQuarterTurn1Tile, // TrackElemType::LeftQuarterTurn1Tile
    TrackElemType::LeftQuarterTurn1Tile, // TrackElemType::RightQuarterTurn1Tile
    TrackElemType::RightTwistDownToUp, // TrackElemType::LeftTwistDownToUp
    TrackElemType::LeftTwistDownToUp, // TrackElemType::RightTwistDownToUp
    TrackElemType::RightTwistUpToDown, // TrackElemType::LeftTwistUpToDown
    TrackElemType::LeftTwistUpToDown, // TrackElemType::RightTwistUpToDown
    TrackElemType::HalfLoopUp,
    TrackElemType::HalfLoopDown,
    TrackElemType::RightCorkscrewUp, // TrackElemType::LeftCorkscrewUp
    TrackElemType::LeftCorkscrewUp, // TrackElemType::RightCorkscrewUp
    TrackElemType::RightCorkscrewDown, // TrackElemType::LeftCorkscrewDown
    TrackElemType::LeftCorkscrewDown, // TrackElemType::RightCorkscrewDown
    TrackElemType::FlatToUp60,
    TrackElemType::Up60ToFlat,
    TrackElemType::FlatToDown60,
    TrackElemType::Down60ToFlat,
    TrackElemType::TowerBase,
    TrackElemType::TowerSection,
    TrackElemType::FlatCovered,
    TrackElemType::Up25Covered,
    TrackElemType::Up60Covered,
    TrackElemType::FlatToUp25Covered,
    TrackElemType::Up25ToUp60Covered,
    TrackElemType::Up60ToUp25Covered,
    TrackElemType::Up25ToFlatCovered,
    TrackElemType::Down25Covered,
    TrackElemType::Down60Covered,
    TrackElemType::FlatToDown25Covered,
    TrackElemType::Down25ToDown60Covered,
    TrackElemType::Down60ToDown25Covered,
    TrackElemType::Down25ToFlatCovered,
    TrackElemType::RightQuarterTurn5TilesCovered, // TrackElemType::LeftQuarterTurn5TilesCovered
    TrackElemType::LeftQuarterTurn5TilesCovered, // TrackElemType::RightQuarterTurn5TilesCovered
    TrackElemType::SBendRightCovered, // TrackElemType::SBendLeftCovered
    TrackElemType::SBendLeftCovered, // TrackElemType::SBendRightCovered
    TrackElemType::RightQuarterTurn3TilesCovered, // TrackElemType::LeftQuarterTurn3TilesCovered
    TrackElemType::LeftQuarterTurn3TilesCovered, // TrackElemType::RightQuarterTurn3TilesCovered
    TrackElemType::RightHalfBankedHelixUpSmall, // TrackElemType::LeftHalfBankedHelixUpSmall
    TrackElemType::LeftHalfBankedHelixUpSmall, // TrackElemType::RightHalfBankedHelixUpSmall
    TrackElemType::RightHalfBankedHelixDownSmall, // TrackElemType::LeftHalfBankedHelixDownSmall
    TrackElemType::LeftHalfBankedHelixDownSmall, // TrackElemType::RightHalfBankedHelixDownSmall
    TrackElemType::RightHalfBankedHelixUpLarge, // TrackElemType::LeftHalfBankedHelixUpLarge
    TrackElemType::LeftHalfBankedHelixUpLarge, // TrackElemType::RightHalfBankedHelixUpLarge
    TrackElemType::RightHalfBankedHelixDownLarge, // TrackElemType::LeftHalfBankedHelixDownLarge
    TrackElemType::LeftHalfBankedHelixDownLarge, // TrackElemType::RightHalfBankedHelixDownLarge
    TrackElemType::RightQuarterTurn1TileUp60, // TrackElemType::LeftQuarterTurn1TileUp60
    TrackElemType::LeftQuarterTurn1TileUp60, // TrackElemType::RightQuarterTurn1TileUp60
    TrackElemType::RightQuarterTurn1TileDown60, // TrackElemType::LeftQuarterTurn1TileDown60
    TrackElemType::LeftQuarterTurn1TileDown60, // TrackElemType::RightQuarterTurn1TileDown60
    TrackElemType::Brakes,
    TrackElemType::Booster,
    TrackElemType::Maze,
    TrackElemType::RightQuarterBankedHelixLargeUp, // TrackElemType::LeftQuarterBankedHelixLargeUp
    TrackElemType::LeftQuarterBankedHelixLargeUp, // TrackElemType::RightQuarterBankedHelixLargeUp
    TrackElemType::RightQuarterBankedHelixLargeDown, // TrackElemType::LeftQuarterBankedHelixLargeDown
    TrackElemType::LeftQuarterBankedHelixLargeDown, // TrackElemType::RightQuarterBankedHelixLargeDown
    TrackElemType::RightQuarterHelixLargeUp, // TrackElemType::LeftQuarterHelixLargeUp
    TrackElemType::LeftQuarterHelixLargeUp, // TrackElemType::RightQuarterHelixLargeUp
    TrackElemType::RightQuarterHelixLargeDown, // TrackElemType::LeftQuarterHelixLargeDown
    TrackElemType::LeftQuarterHelixLargeDown, // TrackElemType::RightQuarterHelixLargeDown
    TrackElemType::Up25RightBanked, // TrackElemType::Up25LeftBanked
    TrackElemType::Up25LeftBanked, // TrackElemType::Up25RightBanked
    TrackElemType::Waterfall,
    TrackElemType::Rapids,
    TrackElemType::OnRidePhoto,
    TrackElemType::Down25RightBanked, // TrackElemType::Down25LeftBanked
    TrackElemType::Down25LeftBanked, // TrackElemType::Down25RightBanked
    TrackElemType::Watersplash,
    TrackElemType::FlatToUp60LongBase,
    TrackElemType::Up60ToFlatLongBase,
    TrackElemType::Whirlpool,
    TrackElemType::Down60ToFlatLongBase,
    TrackElemType::FlatToDown60LongBase,
    TrackElemType::CableLiftHill,
    TrackElemType::ReverseFreefallSlope,
    TrackElemType::ReverseFreefallVertical,
    TrackElemType::Up90,
    TrackElemType::Down90,
    TrackElemType::Up60ToUp90,
    TrackElemType::Down90ToDown60,
    TrackElemType::Up90ToUp60,
    TrackElemType::Down60ToDown90,
    TrackElemType::BrakeForDrop,
    TrackElemType::RightEighthToDiag, // TrackElemType::LeftEighthToDiag
    TrackElemType::LeftEighthToDiag, // TrackElemType::RightEighthToDiag
    TrackElemType::RightEighthToOrthogonal, // TrackElemType::LeftEighthToOrthogonal
    TrackElemType::LeftEighthToOrthogonal, // TrackElemType::RightEighthToOrthogonal
    TrackElemType::RightEighthBankToDiag, // TrackElemType::LeftEighthBankToDiag
    TrackElemType::LeftEighthBankToDiag, // TrackElemType::RightEighthBankToDiag
    TrackElemType::RightEighthBankToOrthogonal, // TrackElemType::LeftEighthBankToOrthogonal
    TrackElemType::LeftEighthBankToOrthogonal, // TrackElemType::RightEighthBankToOrthogonal
    TrackElemType::DiagFlat,
    TrackElemType::DiagUp25,
    TrackElemType::DiagUp60,
    TrackElemType::DiagFlatToUp25,
    TrackElemType::DiagUp25ToUp60,
    TrackElemType::DiagUp60ToUp25,
    TrackElemType::DiagUp25ToFlat,
    TrackElemType::DiagDown25,
    TrackElemType::DiagDown60,
    TrackElemType::DiagFlatToDown25,
    TrackElemType::DiagDown25ToDown60,
    TrackElemType::DiagDown60ToDown25,
    TrackElemType::DiagDown25ToFlat,
    TrackElemType::DiagFlatToUp60,
    TrackElemType::DiagUp60ToFlat,
    TrackElemType::DiagFlatToDown60,
    TrackElemType::DiagDown60ToFlat,
    TrackElemType::DiagFlatToRightBank, // TrackElemType::DiagFlatToLeftBank
    TrackElemType::DiagFlatToLeftBank, // TrackElemType::DiagFlatToRightBank
    TrackElemType::DiagRightBankToFlat, // TrackElemType::DiagLeftBankToFlat
    TrackElemType::DiagLeftBankToFlat, // TrackElemType::DiagRightBankToFlat
    TrackElemType::DiagRightBankToUp25, // TrackElemType::DiagLeftBankToUp25
    TrackElemType::DiagLeftBankToUp25, // TrackElemType::DiagRightBankToUp25
    TrackElemType::DiagUp25ToRightBank, // TrackElemType::DiagUp25ToLeftBank
    TrackElemType::DiagUp25ToLeftBank, // TrackElemType::DiagUp25ToRightBank
    TrackElemType::DiagRightBankToDown25, // TrackElemType::DiagLeftBankToDown25
    TrackElemType::DiagLeftBankToDown25, // TrackElemType::DiagRightBankToDown25
    TrackElemType::DiagDown25ToRightBank, // TrackElemType::DiagDown25ToLeftBank
    TrackElemType::DiagDown25ToLeftBank, // TrackElemType::DiagDown25ToRightBank
    TrackElemType::DiagRightBank, // TrackElemType::DiagLeftBank
    TrackElemType::DiagLeftBank, // TrackElemType::DiagRightBank
    TrackElemType::LogFlumeReverser,
    TrackElemType::SpinningTunnel,
    TrackElemType::RightBarrelRollUpToDown, // TrackElemType::LeftBarrelRollUpToDown
    TrackElemType::LeftBarrelRollUpToDown, // TrackElemType::RightBarrelRollUpToDown
    TrackElemType::RightBarrelRollDownToUp, // TrackElemType::LeftBarrelRollDownToUp
    TrackElemType::LeftBarrelRollDownToUp, // TrackElemType::RightBarrelRollDownToUp
    TrackElemType::RightBankToRightQuarterTurn3TilesUp25, // TrackElemType::LeftBankToLeftQuarterTurn3TilesUp25
    TrackElemType::LeftBankToLeftQuarterTurn3TilesUp25, // TrackElemType::RightBankToRightQuarterTurn3TilesUp25
    TrackElemType::RightQuarterTurn3TilesDown25ToRightBank, // TrackElemType::LeftQuarterTurn3TilesDown25ToLeftBank
    TrackElemType::LeftQuarterTurn3TilesDown25ToLeftBank, // TrackElemType::RightQuarterTurn3TilesDown25ToRightBank
    TrackElemType::PoweredLift,
    TrackElemType::RightLargeHalfLoopUp, // TrackElemType::LeftLargeHalfLoopUp
    TrackElemType::LeftLargeHalfLoopUp, // TrackElemType::RightLargeHalfLoopUp
    TrackElemType::LeftLargeHalfLoopDown, // TrackElemType::RightLargeHalfLoopDown
    TrackElemType::RightLargeHalfLoopDown, // TrackElemType::LeftLargeHalfLoopDown
    TrackElemType::RightFlyerTwistUp, // TrackElemType::LeftFlyerTwistUp
    TrackElemType::LeftFlyerTwistUp, // TrackElemType::RightFlyerTwistUp
    TrackElemType::RightFlyerTwistDown, // TrackElemType::LeftFlyerTwistDown
    TrackElemType::LeftFlyerTwistDown, // TrackElemType::RightFlyerTwistDown
    TrackElemType::FlyerHalfLoopUninvertedUp,
    TrackElemType::FlyerHalfLoopInvertedDown,
    TrackElemType::RightFlyerCorkscrewUp, // TrackElemType::LeftFlyerCorkscrewUp
    TrackElemType::LeftFlyerCorkscrewUp, // TrackElemType::RightFlyerCorkscrewUp
    TrackElemType::RightFlyerCorkscrewDown, // TrackElemType::LeftFlyerCorkscrewDown
    TrackElemType::LeftFlyerCorkscrewDown, // TrackElemType::RightFlyerCorkscrewDown
    TrackElemType::HeartLineTransferUp,
    TrackElemType::HeartLineTransferDown,
    TrackElemType::RightHeartLineRoll, // TrackElemType::LeftHeartLineRoll
    TrackElemType::LeftHeartLineRoll, // TrackElemType::RightHeartLineRoll
    TrackElemType::MinigolfHoleA,
    TrackElemType::MinigolfHoleB,
    TrackElemType::MinigolfHoleC,
    TrackElemType::MinigolfHoleE, // TrackElemType::MinigolfHoleD
    TrackElemType::MinigolfHoleD, // TrackElemType::MinigolfHoleE
    TrackElemType::MultiDimInvertedFlatToDown90QuarterLoop,
    TrackElemType::Up90ToInvertedFlatQuarterLoop,
    TrackElemType::InvertedFlatToDown90QuarterLoop,
    TrackElemType::RightCurvedLiftHill, // TrackElemType::LeftCurvedLiftHill
    TrackElemType::LeftCurvedLiftHill, // TrackElemType::RightCurvedLiftHill
    TrackElemType::RightReverser, // TrackElemType::LeftReverser
    TrackElemType::LeftReverser, // TrackElemType::RightReverser
    TrackElemType::AirThrustTopCap,
    TrackElemType::AirThrustVerticalDown,
    TrackElemType::AirThrustVerticalDownToLevel,
    TrackElemType::BlockBrakes,
    TrackElemType::RightBankedQuarterTurn3TileUp25, // TrackElemType::LeftBankedQuarterTurn3TileUp25
    TrackElemType::LeftBankedQuarterTurn3TileUp25, // TrackElemType::RightBankedQuarterTurn3TileUp25
    TrackElemType::RightBankedQuarterTurn3TileDown25, // TrackElemType::LeftBankedQuarterTurn3TileDown25
    TrackElemType::LeftBankedQuarterTurn3TileDown25, // TrackElemType::RightBankedQuarterTurn3TileDown25
    TrackElemType::RightBankedQuarterTurn5TileUp25, // TrackElemType::LeftBankedQuarterTurn5TileUp25
    TrackElemType::LeftBankedQuarterTurn5TileUp25, // TrackElemType::RightBankedQuarterTurn5TileUp25
    TrackElemType::RightBankedQuarterTurn5TileDown25, // TrackElemType::LeftBankedQuarterTurn5TileDown25
    TrackElemType::LeftBankedQuarterTurn5TileDown25, // TrackElemType::RightBankedQuarterTurn5TileDown25
    TrackElemType::Up25ToRightBankedUp25, // TrackElemType::Up25ToLeftBankedUp25
    TrackElemType::Up25ToLeftBankedUp25, // TrackElemType::Up25ToRightBankedUp25
    TrackElemType::RightBankedUp25ToUp25, // TrackElemType::LeftBankedUp25ToUp25
    TrackElemType::LeftBankedUp25ToUp25, // TrackElemType::RightBankedUp25ToUp25
    TrackElemType::Down25ToRightBankedDown25, // TrackElemType::Down25ToLeftBankedDown25
    TrackElemType::Down25ToLeftBankedDown25, // TrackElemType::Down25ToRightBankedDown25
    TrackElemType::RightBankedDown25ToDown25, // TrackElemType::LeftBankedDown25ToDown25
    TrackElemType::LeftBankedDown25ToDown25, // TrackElemType::RightBankedDown25ToDown25
    TrackElemType::RightBankedFlatToRightBankedUp25, // TrackElemType::LeftBankedFlatToLeftBankedUp25
    TrackElemType::LeftBankedFlatToLeftBankedUp25, // TrackElemType::RightBankedFlatToRightBankedUp25
    TrackElemType::RightBankedUp25ToRightBankedFlat, // TrackElemType::LeftBankedUp25ToLeftBankedFlat
    TrackElemType::LeftBankedUp25ToLeftBankedFlat, // TrackElemType::RightBankedUp25ToRightBankedFlat
    TrackElemType::RightBankedFlatToRightBankedDown25, // TrackElemType::LeftBankedFlatToLeftBankedDown25
    TrackElemType::LeftBankedFlatToLeftBankedDown25, // TrackElemType::RightBankedFlatToRightBankedDown25
    TrackElemType::RightBankedDown25ToRightBankedFlat, // TrackElemType::LeftBankedDown25ToLeftBankedFlat
    TrackElemType::LeftBankedDown25ToLeftBankedFlat, // TrackElemType::RightBankedDown25ToRightBankedFlat
    TrackElemType::FlatToRightBankedUp25, // TrackElemType::FlatToLeftBankedUp25
    TrackElemType::FlatToLeftBankedUp25, // TrackElemType::FlatToRightBankedUp25
    TrackElemType::RightBankedUp25ToFlat, // TrackElemType::LeftBankedUp25ToFlat
    TrackElemType::LeftBankedUp25ToFlat, // TrackElemType::RightBankedUp25ToFlat
    TrackElemType::FlatToRightBankedDown25, // TrackElemType::FlatToLeftBankedDown25
    TrackElemType::FlatToLeftBankedDown25, // TrackElemType::FlatToRightBankedDown25
    TrackElemType::RightBankedDown25ToFlat, // TrackElemType::LeftBankedDown25ToFlat
    TrackElemType::LeftBankedDown25ToFlat, // TrackElemType::RightBankedDown25ToFlat
    TrackElemType::RightQuarterTurn1TileUp90, // TrackElemType::LeftQuarterTurn1TileUp90
    TrackElemType::LeftQuarterTurn1TileUp90, // TrackElemType::RightQuarterTurn1TileUp90
    TrackElemType::RightQuarterTurn1TileDown90, // TrackElemType::LeftQuarterTurn1TileDown90
    TrackElemType::LeftQuarterTurn1TileDown90, // TrackElemType::RightQuarterTurn1TileDown90
    TrackElemType::MultiDimUp90ToInvertedFlatQuarterLoop,
    TrackElemType::MultiDimFlatToDown90QuarterLoop,
    TrackElemType::MultiDimInvertedUp90ToFlatQuarterLoop,
    TrackElemType::RotationControlToggle,
    TrackElemType::FlatTrack1x4A, // TrackElemType::FlatTrack1x4A
    TrackElemType::FlatTrack2x2, // TrackElemType::FlatTrack2x2
    TrackElemType::FlatTrack4x4, // TrackElemType::FlatTrack4x4
    TrackElemType::FlatTrack2x4, // TrackElemType::FlatTrack2x4
    TrackElemType::FlatTrack1x5, // TrackElemType::FlatTrack1x5
    TrackElemType::FlatTrack1x1A, // TrackElemType::FlatTrack1x1A
    TrackElemType::FlatTrack1x4B, // TrackElemType::FlatTrack1x4B
    TrackElemType::FlatTrack1x1B, // TrackElemType::FlatTrack1x1B
    TrackElemType::FlatTrack1x4C, // TrackElemType::FlatTrack1x4C
    TrackElemType::FlatTrack3x3, // TrackElemType::FlatTrack3x3
    TrackElemType::RightLargeCorkscrewUp, // TrackElemType::LeftLargeCorkscrewUp
    TrackElemType::LeftLargeCorkscrewUp, // TrackElemType::RightLargeCorkscrewUp
    TrackElemType::RightLargeCorkscrewDown, // TrackElemType::LeftLargeCorkscrewDown
    TrackElemType::LeftLargeCorkscrewDown, // TrackElemType::RightLargeCorkscrewDown
    TrackElemType::RightMediumHalfLoopUp, // TrackElemType::LeftMediumHalfLoopUp
    TrackElemType::LeftMediumHalfLoopUp, // TrackElemType::RightMediumHalfLoopUp
    TrackElemType::RightMediumHalfLoopDown, // TrackElemType::LeftMediumHalfLoopDown
    TrackElemType::LeftMediumHalfLoopDown, // TrackElemType::RightMediumHalfLoopDown
    TrackElemType::RightZeroGRollUp, // TrackElemType::LeftZeroGRollUp
    TrackElemType::LeftZeroGRollUp, // TrackElemType::RightZeroGRollUp
    TrackElemType::RightZeroGRollDown, // TrackElemType::LeftZeroGRollDown
    TrackElemType::LeftZeroGRollDown, // TrackElemType::RightZeroGRollDown
    TrackElemType::RightLargeZeroGRollUp, // TrackElemType::LeftLargeZeroGRollUp
    TrackElemType::LeftLargeZeroGRollUp, // TrackElemType::RightLargeZeroGRollUp
    TrackElemType::RightLargeZeroGRollDown, // TrackElemType::LeftLargeZeroGRollDown
    TrackElemType::LeftLargeZeroGRollDown, // TrackElemType::RightLargeZeroGRollDown
    TrackElemType::RightFlyerLargeHalfLoopUninvertedUp, // TrackElemType::LeftFlyerLargeHalfLoopUninvertedUp
    TrackElemType::LeftFlyerLargeHalfLoopUninvertedUp, // TrackElemType::RightFlyerLargeHalfLoopUninvertedUp
    TrackElemType::LeftFlyerLargeHalfLoopInvertedDown, // TrackElemType::RightFlyerLargeHalfLoopInvertedDown
    TrackElemType::RightFlyerLargeHalfLoopInvertedDown, // TrackElemType::LeftFlyerLargeHalfLoopInvertedDown
    TrackElemType::RightFlyerLargeHalfLoopInvertedUp, // TrackElemType::LeftFlyerLargeHalfLoopInvertedUp
    TrackElemType::LeftFlyerLargeHalfLoopInvertedUp, // TrackElemType::RightFlyerLargeHalfLoopInvertedUp
    TrackElemType::LeftFlyerLargeHalfLoopUninvertedDown, // TrackElemType::RightFlyerLargeHalfLoopUninvertedDown
    TrackElemType::RightFlyerLargeHalfLoopUninvertedDown, // TrackElemType::LeftFlyerLargeHalfLoopUninvertedDown
    TrackElemType::FlyerHalfLoopInvertedUp,
    TrackElemType::FlyerHalfLoopUninvertedDown,
};

/** rct2: 0x00999694 */
static constexpr uint32_t TrackHeightMarkerPositions[TrackElemType::Count] = {
    (1 << 0), // TrackElemType::Flat
    (1 << 0), // TrackElemType::EndStation
    (1 << 0), // TrackElemType::BeginStation
    (1 << 0), // TrackElemType::MiddleStation
    (1 << 0), // TrackElemType::Up25
    (1 << 0), // TrackElemType::Up60
    (1 << 0), // TrackElemType::FlatToUp25
    (1 << 0), // TrackElemType::Up25ToUp60
    (1 << 0), // TrackElemType::Up60ToUp25
    (1 << 0), // TrackElemType::Up25ToFlat
    (1 << 0), // TrackElemType::Down25
    (1 << 0), // TrackElemType::Down60
    (1 << 0), // TrackElemType::FlatToDown25
    (1 << 0), // TrackElemType::Down25ToDown60
    (1 << 0), // TrackElemType::Down60ToDown25
    (1 << 0), // TrackElemType::Down25ToFlat
    (1 << 0) | (1 << 6), // TrackElemType::LeftQuarterTurn5Tiles
    (1 << 0) | (1 << 6), // TrackElemType::RightQuarterTurn5Tiles
    (1 << 0), // TrackElemType::FlatToLeftBank
    (1 << 0), // TrackElemType::FlatToRightBank
    (1 << 0), // TrackElemType::LeftBankToFlat
    (1 << 0), // TrackElemType::RightBankToFlat
    (1 << 0) | (1 << 6), // TrackElemType::BankedLeftQuarterTurn5Tiles
    (1 << 0) | (1 << 6), // TrackElemType::BankedRightQuarterTurn5Tiles
    (1 << 0), // TrackElemType::LeftBankToUp25
    (1 << 0), // TrackElemType::RightBankToUp25
    (1 << 0), // TrackElemType::Up25ToLeftBank
    (1 << 0), // TrackElemType::Up25ToRightBank
    (1 << 0), // TrackElemType::LeftBankToDown25
    (1 << 0), // TrackElemType::RightBankToDown25
    (1 << 0), // TrackElemType::Down25ToLeftBank
    (1 << 0), // TrackElemType::Down25ToRightBank
    (1 << 0), // TrackElemType::LeftBank
    (1 << 0), // TrackElemType::RightBank
    (1 << 0) | (1 << 6), // TrackElemType::LeftQuarterTurn5TilesUp25
    (1 << 0) | (1 << 6), // TrackElemType::RightQuarterTurn5TilesUp25
    (1 << 0) | (1 << 6), // TrackElemType::LeftQuarterTurn5TilesDown25
    (1 << 0) | (1 << 6), // TrackElemType::RightQuarterTurn5TilesDown25
    (1 << 0) | (1 << 3), // TrackElemType::SBendLeft
    (1 << 0) | (1 << 3), // TrackElemType::SBendRight
    (1 << 0) | (1 << 9), // TrackElemType::LeftVerticalLoop
    (1 << 0) | (1 << 9), // TrackElemType::RightVerticalLoop
    (1 << 0) | (1 << 3), // TrackElemType::LeftQuarterTurn3Tiles
    (1 << 0) | (1 << 3), // TrackElemType::RightQuarterTurn3Tiles
    (1 << 0) | (1 << 3), // TrackElemType::LeftBankedQuarterTurn3Tiles
    (1 << 0) | (1 << 3), // TrackElemType::RightBankedQuarterTurn3Tiles
    (1 << 0) | (1 << 3), // TrackElemType::LeftQuarterTurn3TilesUp25
    (1 << 0) | (1 << 3), // TrackElemType::RightQuarterTurn3TilesUp25
    (1 << 0) | (1 << 3), // TrackElemType::LeftQuarterTurn3TilesDown25
    (1 << 0) | (1 << 3), // TrackElemType::RightQuarterTurn3TilesDown25
    (1 << 0), // TrackElemType::LeftQuarterTurn1Tile
    (1 << 0), // TrackElemType::RightQuarterTurn1Tile
    (1 << 0) | (1 << 2), // TrackElemType::LeftTwistDownToUp
    (1 << 0) | (1 << 2), // TrackElemType::RightTwistDownToUp
    (1 << 0) | (1 << 2), // TrackElemType::LeftTwistUpToDown
    (1 << 0) | (1 << 2), // TrackElemType::RightTwistUpToDown
    (1 << 0) | (1 << 3), // TrackElemType::HalfLoopUp
    (1 << 0) | (1 << 3), // TrackElemType::HalfLoopDown
    (1 << 0) | (1 << 2), // TrackElemType::LeftCorkscrewUp
    (1 << 0) | (1 << 2), // TrackElemType::RightCorkscrewUp
    (1 << 0) | (1 << 2), // TrackElemType::LeftCorkscrewDown
    (1 << 0) | (1 << 2), // TrackElemType::RightCorkscrewDown
    (1 << 0), // TrackElemType::FlatToUp60
    (1 << 0), // TrackElemType::Up60ToFlat
    (1 << 0), // TrackElemType::FlatToDown60
    (1 << 0), // TrackElemType::Down60ToFlat
    (1 << 1) | (1 << 3) | (1 << 6) | (1 << 7), // TrackElemType::TowerBase
    (1 << 0), // TrackElemType::TowerSection
    (1 << 0), // TrackElemType::FlatCovered
    (1 << 0), // TrackElemType::Up25Covered
    (1 << 0), // TrackElemType::Up60Covered
    (1 << 0), // TrackElemType::FlatToUp25Covered
    (1 << 0), // TrackElemType::Up25ToUp60Covered
    (1 << 0), // TrackElemType::Up60ToUp25Covered
    (1 << 0), // TrackElemType::Up25ToFlatCovered
    (1 << 0), // TrackElemType::Down25Covered
    (1 << 0), // TrackElemType::Down60Covered
    (1 << 0), // TrackElemType::FlatToDown25Covered
    (1 << 0), // TrackElemType::Down25ToDown60Covered
    (1 << 0), // TrackElemType::Down60ToDown25Covered
    (1 << 0), // TrackElemType::Down25ToFlatCovered
    (1 << 0) | (1 << 6), // TrackElemType::LeftQuarterTurn5TilesCovered
    (1 << 0) | (1 << 6), // TrackElemType::RightQuarterTurn5TilesCovered
    (1 << 0) | (1 << 3), // TrackElemType::SBendLeftCovered
    (1 << 0) | (1 << 3), // TrackElemType::SBendRightCovered
    (1 << 0) | (1 << 3), // TrackElemType::LeftQuarterTurn3TilesCovered
    (1 << 0) | (1 << 3), // TrackElemType::RightQuarterTurn3TilesCovered
    (1 << 0) | (1 << 7), // TrackElemType::LeftHalfBankedHelixUpSmall
    (1 << 0) | (1 << 7), // TrackElemType::RightHalfBankedHelixUpSmall
    (1 << 0) | (1 << 7), // TrackElemType::LeftHalfBankedHelixDownSmall
    (1 << 0) | (1 << 7), // TrackElemType::RightHalfBankedHelixDownSmall
    (1 << 0) | (1 << 13), // TrackElemType::LeftHalfBankedHelixUpLarge
    (1 << 0) | (1 << 13), // TrackElemType::RightHalfBankedHelixUpLarge
    (1 << 0) | (1 << 13), // TrackElemType::LeftHalfBankedHelixDownLarge
    (1 << 0) | (1 << 13), // TrackElemType::RightHalfBankedHelixDownLarge
    (1 << 0), // TrackElemType::LeftQuarterTurn1TileUp60
    (1 << 0), // TrackElemType::RightQuarterTurn1TileUp60
    (1 << 0), // TrackElemType::LeftQuarterTurn1TileDown60
    (1 << 0), // TrackElemType::RightQuarterTurn1TileDown60
    (1 << 0), // TrackElemType::Brakes
    (1 << 0), // TrackElemType::Booster
    (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11) | (1 << 12) | (1 << 13) | (1 << 14) | (1 << 15), // Maze
    (1 << 0) | (1 << 6), // TrackElemType::LeftQuarterBankedHelixLargeUp
    (1 << 0) | (1 << 6), // TrackElemType::RightQuarterBankedHelixLargeUp
    (1 << 0) | (1 << 6), // TrackElemType::LeftQuarterBankedHelixLargeDown
    (1 << 0) | (1 << 6), // TrackElemType::RightQuarterBankedHelixLargeDown
    (1 << 0) | (1 << 6), // TrackElemType::LeftQuarterHelixLargeUp
    (1 << 0) | (1 << 6), // TrackElemType::RightQuarterHelixLargeUp
    (1 << 0) | (1 << 6), // TrackElemType::LeftQuarterHelixLargeDown
    (1 << 0) | (1 << 6), // TrackElemType::RightQuarterHelixLargeDown
    (1 << 0), // TrackElemType::Up25LeftBanked
    (1 << 0) | (1 << 3) | (1 << 12) | (1 << 15), // TrackElemType::Up25RightBanked
    (1 << 0), // TrackElemType::Waterfall
    (1 << 0), // TrackElemType::Rapids
    (1 << 0), // TrackElemType::OnRidePhoto
    (1 << 0) | (1 << 7), // TrackElemType::Down25LeftBanked
    (1 << 0), // TrackElemType::Down25RightBanked
    (1 << 0) | (1 << 4), // TrackElemType::Watersplash
    (1 << 0) | (1 << 3), // TrackElemType::FlatToUp60LongBase
    (1 << 0) | (1 << 3), // TrackElemType::Up60ToFlatLongBase
    (1 << 0), // TrackElemType::Whirlpool
    (1 << 0) | (1 << 3), // TrackElemType::Down60ToFlatLongBase
    (1 << 0) | (1 << 3), // TrackElemType::FlatToUp60LongBase
    (1 << 0) | (1 << 3), // TrackElemType::CableLiftHill
    (1 << 0) | (1 << 5), // TrackElemType::ReverseFreefallSlope
    (1 << 0), // TrackElemType::ReverseFreefallVertical
    (1 << 0), // TrackElemType::Up90
    (1 << 0), // TrackElemType::Down90
    (1 << 0), // TrackElemType::Up60ToUp90
    (1 << 0), // TrackElemType::Down90ToDown60
    (1 << 0), // TrackElemType::Up90ToUp60
    (1 << 0), // TrackElemType::Down60ToDown90
    (1 << 0), // TrackElemType::BrakeForDrop
    (1 << 0) | (1 << 4), // TrackElemType::LeftEighthToDiag
    (1 << 0) | (1 << 4), // TrackElemType::RightEighthToDiag
    (1 << 0) | (1 << 4), // TrackElemType::LeftEighthToOrthogonal
    (1 << 0) | (1 << 4), // TrackElemType::RightEighthToOrthogonal
    (1 << 0) | (1 << 4), // TrackElemType::LeftEighthBankToDiag
    (1 << 0) | (1 << 4), // TrackElemType::RightEighthBankToDiag
    (1 << 0) | (1 << 4), // TrackElemType::LeftEighthBankToOrthogonal
    (1 << 0) | (1 << 4), // TrackElemType::RightEighthBankToOrthogonal
    (1 << 0), // TrackElemType::DiagFlat
    (1 << 0), // TrackElemType::DiagUp25
    (1 << 0), // TrackElemType::DiagUp60
    (1 << 0), // TrackElemType::DiagFlatToUp25
    (1 << 0), // TrackElemType::DiagUp25ToUp60
    (1 << 0), // TrackElemType::DiagUp60ToUp25
    (1 << 0), // TrackElemType::DiagUp25ToFlat
    (1 << 3), // TrackElemType::DiagDown25
    (1 << 3), // TrackElemType::DiagDown60
    (1 << 3), // TrackElemType::DiagFlatToDown25
    (1 << 3), // TrackElemType::DiagDown25ToDown60
    (1 << 3), // TrackElemType::DiagDown60ToDown25
    (1 << 3), // TrackElemType::DiagDown25ToFlat
    (1 << 0), // TrackElemType::DiagFlatToUp60
    (1 << 0), // TrackElemType::DiagUp60ToFlat
    (1 << 3), // TrackElemType::DiagFlatToDown60
    (1 << 3), // TrackElemType::DiagDown60ToFlat
    (1 << 0), // TrackElemType::DiagFlatToLeftBank
    (1 << 0), // TrackElemType::DiagFlatToRightBank
    (1 << 0), // TrackElemType::DiagLeftBankToFlat
    (1 << 0), // TrackElemType::DiagRightBankToFlat
    (1 << 0), // TrackElemType::DiagLeftBankToUp25
    (1 << 0), // TrackElemType::DiagRightBankToUp25
    (1 << 0), // TrackElemType::DiagUp25ToLeftBank
    (1 << 0), // TrackElemType::DiagUp25ToRightBank
    (1 << 3), // TrackElemType::DiagLeftBankToDown25
    (1 << 3), // TrackElemType::DiagRightBankToDown25
    (1 << 3), // TrackElemType::DiagDown25ToLeftBank
    (1 << 3), // TrackElemType::DiagDown25ToRightBank
    (1 << 0), // TrackElemType::DiagLeftBank
    (1 << 0), // TrackElemType::DiagRightBank
    (1 << 0), // TrackElemType::LogFlumeReverser
    (1 << 0), // TrackElemType::SpinningTunnel
    (1 << 0) | (1 << 2), // TrackElemType::LeftBarrelRollUpToDown
    (1 << 0) | (1 << 2), // TrackElemType::RightBarrelRollUpToDown
    (1 << 0) | (1 << 2), // TrackElemType::LeftBarrelRollDownToUp
    (1 << 0) | (1 << 2), // TrackElemType::RightBarrelRollDownToUp
    (1 << 0) | (1 << 3), // TrackElemType::LeftBankToLeftQuarterTurn3TilesUp25
    (1 << 0) | (1 << 3), // TrackElemType::RightBankToRightQuarterTurn3TilesUp25
    (1 << 0) | (1 << 3), // TrackElemType::LeftQuarterTurn3TilesDown25ToLeftBank
    (1 << 0) | (1 << 3), // TrackElemType::RightQuarterTurn3TilesDown25ToRightBank
    (1 << 0), // TrackElemType::PoweredLift
    (1 << 0) | (1 << 6), // TrackElemType::LeftLargeHalfLoopUp
    (1 << 0) | (1 << 6), // TrackElemType::RightLargeHalfLoopUp
    (1 << 0) | (1 << 6), // TrackElemType::RightLargeHalfLoopDown
    (1 << 0) | (1 << 6), // TrackElemType::LeftLargeHalfLoopDown
    (1 << 0) | (1 << 2), // TrackElemType::LeftFlyerTwistUp
    (1 << 0) | (1 << 2), // TrackElemType::RightFlyerTwistUp
    (1 << 0) | (1 << 2), // TrackElemType::LeftFlyerTwistDown
    (1 << 0) | (1 << 2), // TrackElemType::RightFlyerTwistDown
    (1 << 0) | (1 << 3), // TrackElemType::FlyerHalfLoopUninvertedUp
    (1 << 0) | (1 << 3), // TrackElemType::FlyerHalfLoopInvertedDown
    (1 << 0) | (1 << 2), // TrackElemType::LeftFlyerCorkscrewUp
    (1 << 0) | (1 << 2), // TrackElemType::RightFlyerCorkscrewUp
    (1 << 0) | (1 << 2), // TrackElemType::LeftFlyerCorkscrewDown
    (1 << 0) | (1 << 2), // TrackElemType::RightFlyerCorkscrewDown
    (1 << 0) | (1 << 3), // TrackElemType::HeartLineTransferUp
    (1 << 0) | (1 << 3), // TrackElemType::HeartLineTransferDown
    (1 << 0) | (1 << 5), // TrackElemType::LeftHeartLineRoll
    (1 << 0) | (1 << 5), // TrackElemType::RightHeartLineRoll
    (1 << 0) | (1 << 1), // TrackElemType::MinigolfHoleA
    (1 << 0) | (1 << 1), // TrackElemType::MinigolfHoleB
    (1 << 0) | (1 << 1), // TrackElemType::MinigolfHoleC
    (1 << 0) | (1 << 2), // TrackElemType::MinigolfHoleD
    (1 << 0) | (1 << 2), // TrackElemType::MinigolfHoleE
    (1 << 0) | (1 << 2), // TrackElemType::MultiDimInvertedFlatToDown90QuarterLoop
    (1 << 0) | (1 << 2), // TrackElemType::Up90ToInvertedFlatQuarterLoop
    (1 << 0) | (1 << 2), // TrackElemType::InvertedFlatToDown90QuarterLoop
    (1 << 0) | (1 << 3), // TrackElemType::LeftCurvedLiftHill
    (1 << 0) | (1 << 3), // TrackElemType::RightCurvedLiftHill
    (1 << 0) | (1 << 5), // TrackElemType::LeftReverser
    (1 << 0) | (1 << 5), // TrackElemType::RightReverser
    (1 << 0) | (1 << 3), // TrackElemType::AirThrustTopCap
    (1 << 0), // TrackElemType::AirThrustVerticalDown
    (1 << 0) | (1 << 5), // TrackElemType::AirThrustVerticalDownToLevel
    (1 << 0), // TrackElemType::BlockBrakes
    (1 << 0) | (1 << 3), // TrackElemType::LeftBankedQuarterTurn3TileUp25
    (1 << 0) | (1 << 3), // TrackElemType::RightBankedQuarterTurn3TileUp25
    (1 << 0) | (1 << 3), // TrackElemType::LeftBankedQuarterTurn3TileDown25
    (1 << 0) | (1 << 3), // TrackElemType::RightBankedQuarterTurn3TileDown25
    (1 << 0) | (1 << 6), // TrackElemType::LeftBankedQuarterTurn5TileUp25
    (1 << 0) | (1 << 6), // TrackElemType::RightBankedQuarterTurn5TileUp25
    (1 << 0) | (1 << 6), // TrackElemType::LeftBankedQuarterTurn5TileDown25
    (1 << 0) | (1 << 6), // TrackElemType::RightBankedQuarterTurn5TileDown25
    (1 << 0), // TrackElemType::Up25ToLeftBankedUp25
    (1 << 0), // TrackElemType::Up25ToRightBankedUp25
    (1 << 0), // TrackElemType::LeftBankedUp25ToUp25
    (1 << 0), // TrackElemType::RightBankedUp25ToUp25
    (1 << 0), // TrackElemType::Down25ToLeftBankedDown25
    (1 << 0), // TrackElemType::Down25ToRightBankedDown25
    (1 << 0), // TrackElemType::LeftBankedDown25ToDown25
    (1 << 0), // TrackElemType::RightBankedDown25ToDown25
    (1 << 0), // TrackElemType::LeftBankedFlatToLeftBankedUp25
    (1 << 0), // TrackElemType::RightBankedFlatToRightBankedUp25
    (1 << 0), // TrackElemType::LeftBankedUp25ToLeftBankedFlat
    (1 << 0), // TrackElemType::RightBankedUp25ToRightBankedFlat
    (1 << 0), // TrackElemType::LeftBankedFlatToLeftBankedDown25
    (1 << 0), // TrackElemType::RightBankedFlatToRightBankedDown25
    (1 << 0), // TrackElemType::LeftBankedDown25ToLeftBankedFlat
    (1 << 0), // TrackElemType::RightBankedDown25ToRightBankedFlat
    (1 << 0), // TrackElemType::FlatToLeftBankedUp25
    (1 << 0), // TrackElemType::FlatToRightBankedUp25
    (1 << 0), // TrackElemType::LeftBankedUp25ToFlat
    (1 << 0), // TrackElemType::RightBankedUp25ToFlat
    (1 << 0), // TrackElemType::FlatToLeftBankedDown25
    (1 << 0), // TrackElemType::FlatToRightBankedDown25
    (1 << 0), // TrackElemType::LeftBankedDown25ToFlat
    (1 << 0), // TrackElemType::RightBankedDown25ToFlat
    (1 << 0), // TrackElemType::LeftQuarterTurn1TileUp90
    (1 << 0), // TrackElemType::RightQuarterTurn1TileUp90
    (1 << 0), // TrackElemType::LeftQuarterTurn1TileDown90
    (1 << 0), // TrackElemType::RightQuarterTurn1TileDown90
    (1 << 0) | (1 << 2), // TrackElemType::MultiDimUp90ToInvertedFlatQuarterLoop
    (1 << 0) | (1 << 2), // TrackElemType::MultiDimFlatToDown90QuarterLoop
    (1 << 0) | (1 << 2), // TrackElemType::MultiDimInvertedUp90ToFlatQuarterLoop
    (1 << 0), // TrackElemType::RotationControlToggle
    (1 << 0), // TrackElemType::FlatTrack1x4A
    (1 << 0), // TrackElemType::FlatTrack2x2
    (1 << 0), // TrackElemType::FlatTrack4x4
    (1 << 0), // TrackElemType::FlatTrack2x4
    (1 << 0), // TrackElemType::FlatTrack1x5
    (1 << 0), // TrackElemType::FlatTrack1x1A
    (1 << 0), // TrackElemType::FlatTrack1x4B
    (1 << 0), // TrackElemType::FlatTrack1x1B
    (1 << 0), // TrackElemType::FlatTrack1x4C
    (1 << 0), // TrackElemType::FlatTrack3x3
    (1 << 0) | (1 << 5), // TrackElemType::LeftLargeCorkscrewUp
    (1 << 0) | (1 << 5), // TrackElemType::RightLargeCorkscrewUp
    (1 << 0) | (1 << 5), // TrackElemType::LeftLargeCorkscrewDown
    (1 << 0) | (1 << 5), // TrackElemType::RightLargeCorkscrewDown
    (1 << 0) | (1 << 4), // TrackElemType::LeftMediumHalfLoopUp
    (1 << 0) | (1 << 4), // TrackElemType::RightMediumHalfLoopUp
    (1 << 0) | (1 << 4), // TrackElemType::LeftMediumHalfLoopDown
    (1 << 0) | (1 << 4), // TrackElemType::RightMediumHalfLoopDown
    (1 << 0) | (1 << 2), // TrackElemType::LeftZeroGRollUp
    (1 << 0) | (1 << 2), // TrackElemType::RightZeroGRollUp
    (1 << 0) | (1 << 2), // TrackElemType::LeftZeroGRollDown
    (1 << 0) | (1 << 2), // TrackElemType::RightZeroGRollDown
    (1 << 0) | (1 << 3), // TrackElemType::LeftLargeZeroGRollUp
    (1 << 0) | (1 << 3), // TrackElemType::RightLargeZeroGRollUp
    (1 << 0) | (1 << 3), // TrackElemType::LeftLargeZeroGRollDown
    (1 << 0) | (1 << 3), // TrackElemType::RightLargeZeroGRollDown
    (1 << 0) | (1 << 6), // TrackElemType::LeftFlyerLargeHalfLoopUninvertedUp
    (1 << 0) | (1 << 6), // TrackElemType::RightFlyerLargeHalfLoopUninvertedUp
    (1 << 0) | (1 << 6), // TrackElemType::RightFlyerLargeHalfLoopInvertedDown
    (1 << 0) | (1 << 6), // TrackElemType::LeftFlyerLargeHalfLoopInvertedDown
    (1 << 0) | (1 << 6), // TrackElemType::LeftFlyerLargeHalfLoopInvertedUp
    (1 << 0) | (1 << 6), // TrackElemType::RightFlyerLargeHalfLoopInvertedUp
    (1 << 0) | (1 << 6), // TrackElemType::RightFlyerLargeHalfLoopUninvertedDown
    (1 << 0) | (1 << 6), // TrackElemType::LeftFlyerLargeHalfLoopUninvertedDown
    (1 << 0) | (1 << 3), // TrackElemType::FlyerHalfLoopInvertedUp
    (1 << 0) | (1 << 3), // TrackElemType::FlyerHalfLoopUninvertedDown
};

/** rct2: 0x00999A94 */
static constexpr uint8_t TrackSequenceElementAllowedWallEdges[TrackElemType::Count][MaxSequencesPerPiece] = {
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Flat
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::EndStation
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::BeginStation
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::MiddleStation
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Up25
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Up60
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::FlatToUp25
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Up25ToUp60
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Up60ToUp25
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Up25ToFlat
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Down25
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Down60
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::FlatToDown25
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Down25ToDown60
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Down60ToDown25
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Down25ToFlat
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftQuarterTurn5Tiles
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightQuarterTurn5Tiles
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::FlatToLeftBank
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::FlatToRightBank
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftBankToFlat
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightBankToFlat
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::BankedLeftQuarterTurn5Tiles
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::BankedRightQuarterTurn5Tiles
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftBankToUp25
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightBankToUp25
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Up25ToLeftBank
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Up25ToRightBank
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftBankToDown25
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightBankToDown25
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Down25ToLeftBank
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Down25ToRightBank
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftBank
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightBank
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftQuarterTurn5TilesUp25
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightQuarterTurn5TilesUp25
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftQuarterTurn5TilesDown25
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightQuarterTurn5TilesDown25
    { 0b1010, 0b0011, 0b1100, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::SBendLeft
    { 0b1010, 0b1001, 0b0110, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::SBendRight
    { 0b0010, 0b0010, 0b0011,      0,      0,      0,      0, 0b1100, 0b1000, 0b1000,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftVerticalLoop
    { 0b1000, 0b1000, 0b1001,      0,      0,      0,      0, 0b0110, 0b0010, 0b0010,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightVerticalLoop
    { 0b0010, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftQuarterTurn3Tiles
    { 0b1000, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightQuarterTurn3Tiles
    { 0b0010, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftBankedQuarterTurn3Tiles
    { 0b1000, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightBankedQuarterTurn3Tiles
    { 0b0010, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftQuarterTurn3TilesUp25
    { 0b1000, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightQuarterTurn3TilesUp25
    { 0b0010, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftQuarterTurn3TilesDown25
    { 0b1000, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightQuarterTurn3TilesDown25
    { 0b0011,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftQuarterTurn1Tile
    { 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightQuarterTurn1Tile
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftTwistDownToUp
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightTwistDownToUp
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftTwistUpToDown
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightTwistUpToDown
    { 0b1010, 0b1010, 0b1011,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::HalfLoopUp
    {      0, 0b1011, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::HalfLoopDown
    { 0b1010, 0b0011, 0b0101,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftCorkscrewUp
    { 0b1010, 0b1001, 0b0101,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightCorkscrewUp
    { 0b1010, 0b0011, 0b0101,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftCorkscrewDown
    { 0b1010, 0b1001, 0b0101,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightCorkscrewDown
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::FlatToUp60
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Up60ToFlat
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::FlatToDown60
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Down60ToFlat
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::TowerBase
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::TowerSection
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::FlatCovered
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Up25Covered
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Up60Covered
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::FlatToUp25Covered
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Up25ToUp60Covered
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Up60ToUp25Covered
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Up25ToFlatCovered
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Down25Covered
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Down60Covered
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::FlatToDown25Covered
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Down25ToDown60Covered
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Down60ToDown25Covered
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Down25ToFlatCovered
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftQuarterTurn5TilesCovered
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightQuarterTurn5TilesCovered
    { 0b1010, 0b0011, 0b1100, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::SBendLeftCovered
    { 0b1010, 0b1001, 0b0110, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::SBendRightCovered
    { 0b0010, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftQuarterTurn3TilesCovered
    { 0b1000, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightQuarterTurn3TilesCovered
    { 0b0010, 0b1100, 0b0011, 0b0001, 0b0001, 0b0110, 0b1001, 0b1000,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftHalfBankedHelixUpSmall
    { 0b1000, 0b0110, 0b1001, 0b0001, 0b0001, 0b1100, 0b0011, 0b0010,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightHalfBankedHelixUpSmall
    { 0b0010, 0b1100, 0b0011, 0b0001, 0b0001, 0b0110, 0b1001, 0b1000,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftHalfBankedHelixDownSmall
    { 0b1000, 0b0110, 0b1001, 0b0001, 0b0001, 0b1100, 0b0011, 0b0010,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightHalfBankedHelixDownSmall
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001, 0b0001, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b1000,      0,      0 }, // TrackElemType::LeftHalfBankedHelixUpLarge
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001, 0b0001, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0010,      0,      0 }, // TrackElemType::RightHalfBankedHelixUpLarge
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001, 0b0001, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b1000,      0,      0 }, // TrackElemType::LeftHalfBankedHelixDownLarge
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001, 0b0001, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0010,      0,      0 }, // TrackElemType::RightHalfBankedHelixDownLarge
    { 0b0011,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftQuarterTurn1TileUp60
    { 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightQuarterTurn1TileUp60
    { 0b0011,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftQuarterTurn1TileDown60
    { 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightQuarterTurn1TileDown60
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Brakes
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Booster
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::InvertedUp90ToFlatQuarterLoopAlias / Maze
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftQuarterBankedHelixLargeUp
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightQuarterBankedHelixLargeUp
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftQuarterBankedHelixLargeDown
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightQuarterBankedHelixLargeDown
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftQuarterHelixLargeUp
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightQuarterHelixLargeUp
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftQuarterHelixLargeDown
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightQuarterHelixLargeDown
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Up25LeftBanked
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Up25RightBanked
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Waterfall
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Rapids
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::OnRidePhoto
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Down25LeftBanked
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Down25RightBanked
    { 0b1010, 0b1010, 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Watersplash
    { 0b1010, 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::FlatToUp60LongBase
    { 0b1010, 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Up60ToFlatLongBase
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Whirlpool
    { 0b1010, 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Down60ToFlatLongBase
    { 0b1010, 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::FlatToDown60LongBase
    { 0b1010, 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::CableLiftHill
    { 0b1010, 0b1010, 0b1010, 0b1010, 0b1010, 0b1011,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::ReverseFreefallSlope
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::ReverseFreefallVertical
    { 0b1111,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Up90
    { 0b1111,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Down90
    { 0b1011,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Up60ToUp90
    { 0b1110,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Down90ToDown60
    { 0b1011,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Up90ToUp60
    { 0b1110,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Down60ToDown90
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::BrakeForDrop
    { 0b0010, 0b0010, 0b1000, 0b0011,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftEighthToDiag
    { 0b1000, 0b1000, 0b0010, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightEighthToDiag
    {      0, 0b1000, 0b0110, 0b0010, 0b0010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftEighthToOrthogonal
    {      0, 0b0100, 0b1001, 0b0001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightEighthToOrthogonal
    { 0b0010, 0b0010, 0b1000, 0b0011,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftEighthBankToDiag
    { 0b1000, 0b1000, 0b0010, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightEighthBankToDiag
    {      0, 0b1000, 0b0110, 0b0010, 0b0010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftEighthBankToOrthogonal
    {      0, 0b0100, 0b1001, 0b0001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightEighthBankToOrthogonal
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::DiagFlat
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::DiagUp25
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::DiagUp60
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::DiagFlatToUp25
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::DiagUp25ToUp60
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::DiagUp60ToUp25
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::DiagUp25ToFlat
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::DiagDown25
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::DiagDown60
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::DiagFlatToDown25
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::DiagDown25ToDown60
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::DiagDown60ToDown25
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::DiagDown25ToFlat
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::DiagFlatToUp60
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::DiagUp60ToFlat
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::DiagFlatToDown60
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::DiagDown60ToFlat
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::DiagFlatToLeftBank
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::DiagFlatToRightBank
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::DiagLeftBankToFlat
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::DiagRightBankToFlat
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::DiagLeftBankToUp25
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::DiagRightBankToUp25
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::DiagUp25ToLeftBank
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::DiagUp25ToRightBank
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::DiagLeftBankToDown25
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::DiagRightBankToDown25
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::DiagDown25ToLeftBank
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::DiagDown25ToRightBank
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::DiagLeftBank
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::DiagRightBank
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LogFlumeReverser
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::SpinningTunnel
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftBarrelRollUpToDown
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightBarrelRollUpToDown
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftBarrelRollDownToUp
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightBarrelRollDownToUp
    { 0b0010, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftBankToLeftQuarterTurn3TilesUp25
    { 0b1000, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightBankToRightQuarterTurn3TilesUp25
    { 0b0010, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftQuarterTurn3TilesDown25ToLeftBank
    { 0b1000, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightQuarterTurn3TilesDown25ToRightBank
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::PoweredLift
    { 0b1010, 0b1010, 0b0010, 0b0011, 0b1001, 0b1000, 0b1000,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftLargeHalfLoopUp
    { 0b1010, 0b1010, 0b1000, 0b1001, 0b0011, 0b0010, 0b0010,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightLargeHalfLoopUp
    { 0b0010, 0b0010, 0b0011, 0b1001, 0b1000, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightLargeHalfLoopDown
    { 0b1000, 0b1000, 0b1001, 0b0011, 0b0010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftLargeHalfLoopDown
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftFlyerTwistUp
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightFlyerTwistUp
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftFlyerTwistDown
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightFlyerTwistDown
    { 0b1010, 0b1010, 0b1011,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::FlyerHalfLoopUninvertedUp
    {      0, 0b1011, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::FlyerHalfLoopInvertedDown
    { 0b1010, 0b0011, 0b0101,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftFlyerCorkscrewUp
    { 0b1010, 0b1001, 0b0101,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightFlyerCorkscrewUp
    { 0b1010, 0b0011, 0b0101,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftFlyerCorkscrewDown
    { 0b1010, 0b1001, 0b0101,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightFlyerCorkscrewDown
    { 0b1010, 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::HeartLineTransferUp
    { 0b1010, 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::HeartLineTransferDown
    { 0b1010, 0b1010, 0b1010, 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftHeartLineRoll
    { 0b1010, 0b1010, 0b1010, 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightHeartLineRoll
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::MinigolfHoleA
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::MinigolfHoleB
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::MinigolfHoleC
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::MinigolfHoleD
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::MinigolfHoleE
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::MultiDimInvertedFlatToDown90QuarterLoop
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Up90ToInvertedFlatQuarterLoop
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::InvertedFlatToDown90QuarterLoop
    { 0b0010, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftCurvedLiftHill
    { 0b1000, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightCurvedLiftHill
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftReverser
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightReverser
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::AirThrustTopCap
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::AirThrustVerticalDown
    { 0b1010, 0b1010, 0b1010, 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::AirThrustVerticalDownToLevel
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::BlockBrakes
    { 0b0010, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftBankedQuarterTurn3TileUp25
    { 0b1000, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightBankedQuarterTurn3TileUp25
    { 0b0010, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftBankedQuarterTurn3TileDown25
    { 0b1000, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightBankedQuarterTurn3TileDown25
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftBankedQuarterTurn5TileUp25
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightBankedQuarterTurn5TileUp25
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftBankedQuarterTurn5TileDown25
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightBankedQuarterTurn5TileDown25
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Up25ToLeftBankedUp25
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Up25ToRightBankedUp25
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftBankedUp25ToUp25
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightBankedUp25ToUp25
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Down25ToLeftBankedDown25
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::Down25ToRightBankedDown25
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftBankedDown25ToDown25
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightBankedDown25ToDown25
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftBankedFlatToLeftBankedUp25
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightBankedFlatToRightBankedUp25
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftBankedUp25ToLeftBankedFlat
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightBankedUp25ToRightBankedFlat
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftBankedFlatToLeftBankedDown25
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightBankedFlatToRightBankedDown25
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftBankedDown25ToLeftBankedFlat
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightBankedDown25ToRightBankedFlat
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::FlatToLeftBankedUp25
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::FlatToRightBankedUp25
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftBankedUp25ToFlat
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightBankedUp25ToFlat
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::FlatToLeftBankedDown25
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::FlatToRightBankedDown25
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftBankedDown25ToFlat
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightBankedDown25ToFlat
    { 0b1111,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftQuarterTurn1TileUp90
    { 0b1111,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightQuarterTurn1TileUp90
    { 0b1111,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftQuarterTurn1TileDown90
    { 0b1111,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightQuarterTurn1TileDown90
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::MultiDimUp90ToInvertedFlatQuarterLoop
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::MultiDimFlatToDown90QuarterLoop
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::MultiDimInvertedUp90ToFlatQuarterLoop
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RotationControlToggle
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::FlatTrack1x4A
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::FlatTrack2x2
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::FlatTrack4x4
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::FlatTrack2x4
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::FlatTrack1x5
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::FlatTrack1x1A
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::FlatTrack1x4B
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::FlatTrack1x1B
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::FlatTrack1x4C
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::FlatTrack3x3
    { 0b1010, 0b0010, 0b0011, 0b1100, 0b0001, 0b0101,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftLargeCorkscrewUp
    { 0b1010, 0b1000, 0b1001, 0b0110, 0b0001, 0b0101,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightLargeCorkscrewUp
    { 0b1010, 0b0010, 0b1100, 0b0011, 0b0001, 0b0101,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftLargeCorkscrewDown
    { 0b1010, 0b1000, 0b0110, 0b1001, 0b0001, 0b0101,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightLargeCorkscrewDown
    { 0b1010, 0b0010, 0b0011, 0b1001, 0b1000,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftMediumHalfLoopUp
    { 0b1010, 0b1000, 0b1001, 0b0011, 0b0010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightMediumHalfLoopUp
    { 0b0010, 0b0011, 0b1001, 0b1000, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftMediumHalfLoopDown
    { 0b1000, 0b1001, 0b0011, 0b0010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightMediumHalfLoopDown
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftZeroGRollUp
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightZeroGRollUp
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftZeroGRollDown
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightZeroGRollDown
    { 0b1010, 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftLargeZeroGRollUp
    { 0b1010, 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightLargeZeroGRollUp
    { 0b1010, 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftLargeZeroGRollDown
    { 0b1010, 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightLargeZeroGRollDown

    { 0b1010, 0b1010, 0b0010, 0b0011, 0b1001, 0b1000, 0b1000,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftFlyerLargeHalfLoopUninvertedUp
    { 0b1010, 0b1010, 0b1000, 0b1001, 0b0011, 0b0010, 0b0010,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightFlyerLargeHalfLoopUninvertedUp
    { 0b0010, 0b0010, 0b0011, 0b1001, 0b1000, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightFlyerLargeHalfLoopInvertedDown
    { 0b1000, 0b1000, 0b1001, 0b0011, 0b0010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftFlyerLargeHalfLoopInvertedDown
    { 0b1010, 0b1010, 0b0010, 0b0011, 0b1001, 0b1000, 0b1000,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftFlyerLargeHalfLoopInvertedUp
    { 0b1010, 0b1010, 0b1000, 0b1001, 0b0011, 0b0010, 0b0010,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightFlyerLargeHalfLoopInvertedUp
    { 0b0010, 0b0010, 0b0011, 0b1001, 0b1000, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::RightFlyerLargeHalfLoopUninvertedDown
    { 0b1000, 0b1000, 0b1001, 0b0011, 0b0010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::LeftFlyerLargeHalfLoopUninvertedDown
    { 0b1010, 0b1010, 0b1011,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::FlyerHalfLoopInvertedUp
    {      0, 0b1011, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TrackElemType::FlyerHalfLoopUninvertedDown
};

/** rct2: 0x0099423C */
static constexpr uint16_t TrackFlags[TrackElemType::Count] = {
    /* TrackElemType::Flat                                          */   TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::EndStation                                    */   0,
    /* TrackElemType::BeginStation                                  */   0,
    /* TrackElemType::MiddleStation                                 */   0,
    /* TrackElemType::Up25                                          */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::Up60                                          */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_IS_STEEP_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::FlatToUp25                                    */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::Up25ToUp60                                    */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_IS_STEEP_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::Up60ToUp25                                    */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_IS_STEEP_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::Up25ToFlat                                    */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::Down25                                        */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::Down60                                        */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::FlatToDown25                                  */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::Down25ToDown60                                */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::Down60ToDown25                                */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::Down25ToFlat                                  */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::LeftQuarterTurn5Tiles                         */   TRACK_ELEM_FLAG_TURN_LEFT,
    /* TrackElemType::RightQuarterTurn5Tiles                        */   TRACK_ELEM_FLAG_TURN_RIGHT,
    /* TrackElemType::FlatToLeftBank                                */   TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::FlatToRightBank                               */   TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::LeftBankToFlat                                */   TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::RightBankToFlat                               */   TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::BankedLeftQuarterTurn5Tiles                   */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_BANKED | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::BankedRightQuarterTurn5Tiles                  */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_BANKED | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::LeftBankToUp25                                */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::RightBankToUp25                               */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::Up25ToLeftBank                                */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::Up25ToRightBank                               */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::LeftBankToDown25                              */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::RightBankToDown25                             */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::Down25ToLeftBank                              */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::Down25ToRightBank                             */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::LeftBank                                      */   TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::RightBank                                     */   TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::LeftQuarterTurn5TilesUp25                     */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,
    /* TrackElemType::RightQuarterTurn5TilesUp25                    */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,
    /* TrackElemType::LeftQuarterTurn5TilesDown25                   */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,
    /* TrackElemType::RightQuarterTurn5TilesDown25                  */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,
    /* TrackElemType::SBendLeft                                     */   0,
    /* TrackElemType::SBendRight                                    */   0,
    /* TrackElemType::LeftVerticalLoop                              */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::RightVerticalLoop                             */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::LeftQuarterTurn3Tiles                         */   TRACK_ELEM_FLAG_TURN_LEFT,
    /* TrackElemType::RightQuarterTurn3Tiles                        */   TRACK_ELEM_FLAG_TURN_RIGHT,
    /* TrackElemType::LeftBankedQuarterTurn3Tiles                   */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_BANKED | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::RightBankedQuarterTurn3Tiles                  */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_BANKED | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::LeftQuarterTurn3TilesUp25                     */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,
    /* TrackElemType::RightQuarterTurn3TilesUp25                    */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,
    /* TrackElemType::LeftQuarterTurn3TilesDown25                   */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,
    /* TrackElemType::RightQuarterTurn3TilesDown25                  */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,
    /* TrackElemType::LeftQuarterTurn1Tile                          */   TRACK_ELEM_FLAG_TURN_LEFT,
    /* TrackElemType::RightQuarterTurn1Tile                         */   TRACK_ELEM_FLAG_TURN_RIGHT,
    /* TrackElemType::LeftTwistDownToUp                             */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::RightTwistDownToUp                            */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::LeftTwistUpToDown                             */   TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::RightTwistUpToDown                            */   TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::HalfLoopUp                                    */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::HalfLoopDown                                  */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::LeftCorkscrewUp                               */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::RightCorkscrewUp                              */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::LeftCorkscrewDown                             */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::RightCorkscrewDown                            */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::FlatToUp60                                    */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_IS_STEEP_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::Up60ToFlat                                    */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_IS_STEEP_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::FlatToDown60                                  */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::Down60ToFlat                                  */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::TowerBase                                     */   0,
    /* TrackElemType::TowerSection                                  */   0,
    /* TrackElemType::FlatCovered                                   */   0,
    /* TrackElemType::Up25Covered                                   */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TrackElemType::Up60Covered                                   */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TrackElemType::FlatToUp25Covered                             */   TRACK_ELEM_FLAG_UP,
    /* TrackElemType::Up25ToUp60Covered                             */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TrackElemType::Up60ToUp25Covered                             */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TrackElemType::Up25ToFlatCovered                             */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TrackElemType::Down25Covered                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TrackElemType::Down60Covered                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TrackElemType::FlatToDown25Covered                           */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TrackElemType::Down25ToDown60Covered                         */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TrackElemType::Down60ToDown25Covered                         */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TrackElemType::Down25ToFlatCovered                           */   TRACK_ELEM_FLAG_DOWN,
    /* TrackElemType::LeftQuarterTurn5TilesCovered                  */   TRACK_ELEM_FLAG_TURN_LEFT,
    /* TrackElemType::RightQuarterTurn5TilesCovered                 */   TRACK_ELEM_FLAG_TURN_RIGHT,
    /* TrackElemType::SBendLeftCovered                              */   0,
    /* TrackElemType::SBendRightCovered                             */   0,
    /* TrackElemType::LeftQuarterTurn3TilesCovered                  */   TRACK_ELEM_FLAG_TURN_LEFT,
    /* TrackElemType::RightQuarterTurn3TilesCovered                 */   TRACK_ELEM_FLAG_TURN_RIGHT,
    /* TrackElemType::LeftHalfBankedHelixUpSmall                    */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::RightHalfBankedHelixUpSmall                   */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::LeftHalfBankedHelixDownSmall                  */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::RightHalfBankedHelixDownSmall                 */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::LeftHalfBankedHelixUpLarge                    */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::RightHalfBankedHelixUpLarge                   */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::LeftHalfBankedHelixDownLarge                  */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::RightHalfBankedHelixDownLarge                 */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::LeftQuarterTurn1TileUp60                      */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TrackElemType::RightQuarterTurn1TileUp60                     */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TrackElemType::LeftQuarterTurn1TileDown60                    */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TrackElemType::RightQuarterTurn1TileDown60                   */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TrackElemType::Brakes                                        */   0,
    /* TrackElemType::Booster                                       */   0,
    /* TrackElemType::InvertedUp90ToFlatQuarterLoopAlias / Maze     */   TRACK_ELEM_FLAG_ONLY_ABOVE_GROUND,
    /* TrackElemType::LeftQuarterBankedHelixLargeUp                 */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::RightQuarterBankedHelixLargeUp                */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::LeftQuarterBankedHelixLargeDown               */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::RightQuarterBankedHelixLargeDown              */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::LeftQuarterHelixLargeUp                       */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_HELIX,
    /* TrackElemType::RightQuarterHelixLargeUp                      */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_HELIX,
    /* TrackElemType::LeftQuarterHelixLargeDown                     */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_HELIX,
    /* TrackElemType::RightQuarterHelixLargeDown                    */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_HELIX,
    /* TrackElemType::Up25LeftBanked                                */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::Up25RightBanked                               */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::Waterfall                                     */   0,
    /* TrackElemType::Rapids                                        */   0,
    /* TrackElemType::OnRidePhoto                                   */   0,
    /* TrackElemType::Down25LeftBanked                              */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::Down25RightBanked                             */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::Watersplash                                   */   0,
    /* TrackElemType::FlatToUp60LongBase                            */   TRACK_ELEM_FLAG_UP,
    /* TrackElemType::Up60ToFlatLongBase                            */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TrackElemType::Whirlpool                                     */   0,
    /* TrackElemType::Down60ToFlatLongBase                          */   TRACK_ELEM_FLAG_DOWN,
    /* TrackElemType::FlatToDown60LongBase                          */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TrackElemType::CableLiftHill                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TrackElemType::ReverseFreefallSlope                          */ TRACK_ELEM_FLAG_UP,
    /* TrackElemType::ReverseFreefallVertical                       */ TRACK_ELEM_FLAG_UP,
    /* TrackElemType::Up90                                          */   TRACK_ELEM_FLAG_UP,
    /* TrackElemType::Down90                                        */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::Up60ToUp90                                    */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TrackElemType::Down90ToDown60                                */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::Up90ToUp60                                    */   TRACK_ELEM_FLAG_UP,
    /* TrackElemType::Down60ToDown90                                */   TRACK_ELEM_FLAG_DOWN,
    /* TrackElemType::BrakeForDrop                                  */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TrackElemType::LeftEighthToDiag                              */   TRACK_ELEM_FLAG_TURN_LEFT,
    /* TrackElemType::RightEighthToDiag                             */   TRACK_ELEM_FLAG_TURN_RIGHT,
    /* TrackElemType::LeftEighthToOrthogonal                        */   TRACK_ELEM_FLAG_TURN_LEFT,
    /* TrackElemType::RightEighthToOrthogonal                       */   TRACK_ELEM_FLAG_TURN_RIGHT,
    /* TrackElemType::LeftEighthBankToDiag                          */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_BANKED | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::RightEighthBankToDiag                         */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_BANKED | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::LeftEighthBankToOrthogonal                    */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_BANKED | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::RightEighthBankToOrthogonal                   */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_BANKED | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::DiagFlat                                      */   TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::DiagUp25                                      */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::DiagUp60                                      */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_IS_STEEP_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::DiagFlatToUp25                                */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::DiagUp25ToUp60                                */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_IS_STEEP_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::DiagUp60ToUp25                                */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_IS_STEEP_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::DiagUp25ToFlat                                */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::DiagDown25                                    */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::DiagDown60                                    */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::DiagFlatToDown25                              */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::DiagDown25ToDown60                            */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::DiagDown60ToDown25                            */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::DiagDown25ToFlat                              */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::DiagFlatToUp60                                */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_IS_STEEP_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::DiagUp60ToFlat                                */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_IS_STEEP_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::DiagFlatToDown60                              */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::DiagDown60ToFlat                              */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TrackElemType::DiagFlatToLeftBank                            */   TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::DiagFlatToRightBank                           */   TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::DiagLeftBankToFlat                            */   TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::DiagRightBankToFlat                           */   TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::DiagLeftBankToUp25                            */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::DiagRightBankToUp25                           */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::DiagUp25ToLeftBank                            */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::DiagUp25ToRightBank                           */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::DiagLeftBankToDown25                          */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::DiagRightBankToDown25                         */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::DiagDown25ToLeftBank                          */   TRACK_ELEM_FLAG_DOWN,
    /* TrackElemType::DiagDown25ToRightBank                         */   TRACK_ELEM_FLAG_DOWN,
    /* TrackElemType::DiagLeftBank                                  */   TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::DiagRightBank                                 */   TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::LogFlumeReverser                              */   TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::SpinningTunnel                                */   TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::LeftBarrelRollUpToDown                        */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::RightBarrelRollUpToDown                       */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::LeftBarrelRollDownToUp                        */   TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::RightBarrelRollDownToUp                       */   TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::LeftBankToLeftQuarterTurn3TilesUp25           */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::RightBankToRightQuarterTurn3TilesUp25         */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::LeftQuarterTurn3TilesDown25ToLeftBank         */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::RightQuarterTurn3TilesDown25ToRightBank       */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::PoweredLift                                   */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TrackElemType::LeftLargeHalfLoopUp                           */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::RightLargeHalfLoopUp                          */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::RightLargeHalfLoopDown                        */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::LeftLargeHalfLoopDown                         */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::LeftFlyerTwistUp                              */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::RightFlyerTwistUp                             */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::LeftFlyerTwistDown                            */   TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::RightFlyerTwistDown                           */   TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::FlyerHalfLoopUninvertedUp                     */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::FlyerHalfLoopInvertedDown                     */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::LeftFlyerCorkscrewUp                          */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::RightFlyerCorkscrewUp                         */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::LeftFlyerCorkscrewDown                        */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::RightFlyerCorkscrewDown                       */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::HeartLineTransferUp                           */   0,
    /* TrackElemType::HeartLineTransferDown                         */   0,
    /* TrackElemType::LeftHeartLineRoll                             */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::RightHeartLineRoll                            */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::MinigolfHoleA                                 */   TRACK_ELEM_FLAG_IS_GOLF_HOLE,
    /* TrackElemType::MinigolfHoleB                                 */   TRACK_ELEM_FLAG_IS_GOLF_HOLE,
    /* TrackElemType::MinigolfHoleC                                 */   TRACK_ELEM_FLAG_IS_GOLF_HOLE,
    /* TrackElemType::MinigolfHoleD                                 */   TRACK_ELEM_FLAG_IS_GOLF_HOLE,
    /* TrackElemType::MinigolfHoleE                                 */   TRACK_ELEM_FLAG_IS_GOLF_HOLE,
    /* TrackElemType::MultiDimInvertedFlatToDown90QuarterLoop       */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::Up90ToInvertedFlatQuarterLoop                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::InvertedFlatToDown90QuarterLoop               */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::LeftCurvedLiftHill                            */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_UP,
    /* TrackElemType::RightCurvedLiftHill                           */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_UP,
    /* TrackElemType::LeftReverser                                  */   0,
    /* TrackElemType::RightReverser                                 */   0,
    /* TrackElemType::AirThrustTopCap                               */   0,
    /* TrackElemType::AirThrustVerticalDown                         */   TRACK_ELEM_FLAG_DOWN,
    /* TrackElemType::AirThrustVerticalDownToLevel                  */   TRACK_ELEM_FLAG_DOWN,
    /* TrackElemType::BlockBrakes                                   */   0,
    /* TrackElemType::LeftBankedQuarterTurn3TileUp25                */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::RightBankedQuarterTurn3TileUp25               */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::LeftBankedQuarterTurn3TileDown25              */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::RightBankedQuarterTurn3TileDown25             */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::LeftBankedQuarterTurn5TileUp25                */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::RightBankedQuarterTurn5TileUp25               */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::LeftBankedQuarterTurn5TileDown25              */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::RightBankedQuarterTurn5TileDown25             */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::Up25ToLeftBankedUp25                          */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::Up25ToRightBankedUp25                         */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::LeftBankedUp25ToUp25                          */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::RightBankedUp25ToUp25                         */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::Down25ToLeftBankedDown25                      */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::Down25ToRightBankedDown25                     */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::LeftBankedDown25ToDown25                      */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::RightBankedDown25ToDown25                     */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::LeftBankedFlatToLeftBankedUp25                */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::RightBankedFlatToRightBankedUp25              */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::LeftBankedUp25ToLeftBankedFlat                */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::RightBankedUp25ToRightBankedFlat              */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::LeftBankedFlatToLeftBankedDown25              */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::RightBankedFlatToRightBankedDown25            */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::LeftBankedDown25ToLeftBankedFlat              */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::RightBankedDown25ToRightBankedFlat            */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::FlatToLeftBankedUp25                          */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::FlatToRightBankedUp25                         */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::LeftBankedUp25ToFlat                          */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::RightBankedUp25ToFlat                         */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::FlatToLeftBankedDown25                        */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::FlatToRightBankedDown25                       */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::LeftBankedDown25ToFlat                        */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::RightBankedDown25ToFlat                       */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED,
    /* TrackElemType::LeftQuarterTurn1TileUp90                      */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP,
    /* TrackElemType::RightQuarterTurn1TileUp90                     */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP,
    /* TrackElemType::LeftQuarterTurn1TileDown90                    */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN,
    /* TrackElemType::RightQuarterTurn1TileDown90                   */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN,
    /* TrackElemType::MultiDimUp90ToInvertedFlatQuarterLoop         */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::MultiDimFlatToDown90QuarterLoop               */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::MultiDimInvertedUp90ToFlatQuarterLoop         */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::RotationControlToggle                         */   0,
    /* TrackElemType::FlatTrack1x4A                                 */   0,
    /* TrackElemType::FlatTrack2x2                                  */   0,
    /* TrackElemType::FlatTrack4x4                                  */   0,
    /* TrackElemType::FlatTrack2x4                                  */   TRACK_ELEM_FLAG_ONLY_ABOVE_GROUND,
    /* TrackElemType::FlatTrack1x5                                  */   0,
    /* TrackElemType::FlatTrack1x1A                                 */   0,
    /* TrackElemType::FlatTrack1x4B                                 */   0,
    /* TrackElemType::FlatTrack1x1B                                 */   0,
    /* TrackElemType::FlatTrack1x4C                                 */   0,
    /* TrackElemType::FlatTrack3x3                                  */   0,
    /* TrackElemType::LeftLargeCorkscrewUp                          */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::RightLargeCorkscrewUp                         */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::LeftLargeCorkscrewDown                        */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::RightLargeCorkscrewDown                       */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::LeftMediumHalfLoopUp                          */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::RightMediumHalfLoopUp                         */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::RightMediumHalfLoopDown                       */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::LeftMediumHalfLoopDown                        */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::LeftZeroGRollUp                               */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::RightZeroGRollUp                              */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::RightZeroGRollDown                            */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::LeftZeroGRollDown                             */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::LeftLargeZeroGRollUp                          */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::RightLargeZeroGRollUp                         */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::RightLargeZeroGRollDown                       */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::LeftLargeZeroGRollDown                        */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::LeftFlyerLargeHalfLoopUninvertedUp            */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::RightFlyerLargeHalfLoopUninvertedUp           */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::RightFlyerLargeHalfLoopInvertedDown           */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::LeftFlyerLargeHalfLoopInvertedDown            */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::RightFlyerLargeHalfLoopUninvertedUp           */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::LeftFlyerLargeHalfLoopUninvertedUp            */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TrackElemType::RightFlyerLargeHalfLoopInvertedDown           */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION,
    /* TrackElemType::LeftFlyerLargeHalfLoopInvertedDown            */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION,
    /* TrackElemType::FlyerHalfLoopInvertedUp                       */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TrackElemType::FlyerHalfLoopUnivertedDown                    */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION,
};
// clang-format on

/**  rct2: 0x00997C9D */
// clang-format off
static constexpr TrackDefinition TrackDefinitions[TrackElemType::Count] =
{
    // TYPE                         VANGLE END                  VANGLE START                BANK END                BANK START              PREVIEW Z OFFSET
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_FLAT
    { TRACK_STATION_END,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_END_STATION
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_BEGIN_STATION
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_MIDDLE_STATION
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_60,          TRACK_SLOPE_UP_60,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_60_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_FLAT_TO_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_60,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_25_DEG_UP_TO_60_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_60,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_60_DEG_UP_TO_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_25_DEG_UP_TO_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_60_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_FLAT_TO_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_25_DEG_DOWN_TO_60_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_60_DEG_DOWN_TO_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_25_DEG_DOWN_TO_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_LEFT_QUARTER_TURN_5_TILES
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_RIGHT_QUARTER_TURN_5_TILES
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_NONE,        0                  },  // ELEM_FLAT_TO_LEFT_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        0                  },  // ELEM_FLAT_TO_RIGHT_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_LEFT,        0                  },  // ELEM_LEFT_BANK_TO_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       0                  },  // ELEM_RIGHT_BANK_TO_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                  },  // ELEM_BANKED_LEFT_QUARTER_TURN_5_TILES
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                  },  // ELEM_BANKED_RIGHT_QUARTER_TURN_5_TILES
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_LEFT,        0                  },  // ELEM_LEFT_BANK_TO_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       0                  },  // ELEM_RIGHT_BANK_TO_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_LEFT,        TRACK_BANK_NONE,        0                  },  // ELEM_25_DEG_UP_TO_LEFT_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        0                  },  // ELEM_25_DEG_UP_TO_RIGHT_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_LEFT,        0                  },  // ELEM_LEFT_BANK_TO_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       0                  },  // ELEM_RIGHT_BANK_TO_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_LEFT,        TRACK_BANK_NONE,        0                  },  // ELEM_25_DEG_DOWN_TO_LEFT_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        0                  },  // ELEM_25_DEG_DOWN_TO_RIGHT_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                  },  // ELEM_LEFT_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                  },  // ELEM_RIGHT_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_DOWN
    { TRACK_S_BEND,                 TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_S_BEND_LEFT
    { TRACK_S_BEND,                 TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_S_BEND_RIGHT
    { TRACK_VERTICAL_LOOP,          TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        -48                },  // ELEM_LEFT_VERTICAL_LOOP
    { TRACK_VERTICAL_LOOP,          TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        -48                },  // ELEM_RIGHT_VERTICAL_LOOP
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_LEFT_QUARTER_TURN_3_TILES
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_RIGHT_QUARTER_TURN_3_TILES
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                  },  // ELEM_LEFT_QUARTER_TURN_3_TILES_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                  },  // ELEM_RIGHT_QUARTER_TURN_3_TILES_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_LEFT_QUARTER_TURN_1_TILE
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_RIGHT_QUARTER_TURN_1_TILE
    { TRACK_TWIST,                  TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  },  // ELEM_LEFT_TWIST_DOWN_TO_UP
    { TRACK_TWIST,                  TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  },  // ELEM_RIGHT_TWIST_DOWN_TO_UP
    { TRACK_TWIST,                  TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, 0                  },  // ELEM_LEFT_TWIST_UP_TO_DOWN
    { TRACK_TWIST,                  TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, 0                  },  // ELEM_RIGHT_TWIST_UP_TO_DOWN
    { TRACK_HALF_LOOP,              TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        64                 },  // ELEM_HALF_LOOP_UP
    { TRACK_HALF_LOOP,              TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, -64                },  // ELEM_HALF_LOOP_DOWN
    { TRACK_CORKSCREW,              TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  },  // ELEM_LEFT_CORKSCREW_UP
    { TRACK_CORKSCREW,              TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  },  // ELEM_RIGHT_CORKSCREW_UP
    { TRACK_CORKSCREW,              TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, -32                },  // ELEM_LEFT_CORKSCREW_DOWN
    { TRACK_CORKSCREW,              TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, -32                },  // ELEM_RIGHT_CORKSCREW_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_UP_60,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_FLAT_TO_60_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_60,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_60_DEG_UP_TO_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_FLAT_TO_60_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_60_DEG_DOWN_TO_FLAT
    { TRACK_TOWER_BASE,             TRACK_VANGLE_TOWER,         TRACK_VANGLE_TOWER,         TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_TOWER_BASE
    { TRACK_TOWER_BASE,             TRACK_VANGLE_TOWER,         TRACK_VANGLE_TOWER,         TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_TOWER_SECTION
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_FLAT_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_25_DEG_UP_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_UP_60,          TRACK_SLOPE_UP_60,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_60_DEG_UP_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_FLAT_TO_25_DEG_UP_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_UP_60,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_25_DEG_UP_TO_60_DEG_UP_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_60,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_60_DEG_UP_TO_25_DEG_UP_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_25_DEG_UP_TO_FLAT_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_25_DEG_DOWN_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_60_DEG_DOWN_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_FLAT_TO_25_DEG_DOWN_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_25_DEG_DOWN_TO_60_DEG_DOWN_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_60_DEG_DOWN_TO_25_DEG_DOWN_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_25_DEG_DOWN_TO_FLAT_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_LEFT_QUARTER_TURN_5_TILES_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_RIGHT_QUARTER_TURN_5_TILES_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_S_BEND_LEFT_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_S_BEND_RIGHT_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_LEFT_QUARTER_TURN_3_TILES_COVERED
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_RIGHT_QUARTER_TURN_3_TILES_COVERED
    { TRACK_HELIX_SMALL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                  },  // ELEM_LEFT_HALF_BANKED_HELIX_UP_SMALL
    { TRACK_HELIX_SMALL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                  },  // ELEM_RIGHT_HALF_BANKED_HELIX_UP_SMALL
    { TRACK_HELIX_SMALL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                  },  // ELEM_LEFT_HALF_BANKED_HELIX_DOWN_SMALL
    { TRACK_HELIX_SMALL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                  },  // ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_SMALL
    { TRACK_HELIX_SMALL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                  },  // ELEM_LEFT_HALF_BANKED_HELIX_UP_LARGE
    { TRACK_HELIX_SMALL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                  },  // ELEM_RIGHT_HALF_BANKED_HELIX_UP_LARGE
    { TRACK_HELIX_SMALL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                  },  // ELEM_LEFT_HALF_BANKED_HELIX_DOWN_LARGE
    { TRACK_HELIX_SMALL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                  },  // ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_LARGE
    { TRACK_FLAT,                   TRACK_SLOPE_UP_60,          TRACK_SLOPE_UP_60,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_60,          TRACK_SLOPE_UP_60,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_DOWN
    { TRACK_BRAKES,                 TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_BRAKES
    { TRACK_BOOSTER,                TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_BOOSTER
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // TrackElemType::Maze
    { TRACK_HELIX_LARGE,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                  },  // ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_UP
    { TRACK_HELIX_LARGE,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                  },  // ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_UP
    { TRACK_HELIX_LARGE,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                  },  // ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_DOWN
    { TRACK_HELIX_LARGE,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                  },  // ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_DOWN
    { TRACK_HELIX_LARGE_UNBANKED,   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_LEFT_QUARTER_HELIX_LARGE_UP
    { TRACK_HELIX_LARGE_UNBANKED,   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_RIGHT_QUARTER_HELIX_LARGE_UP
    { TRACK_HELIX_LARGE_UNBANKED,   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_LEFT_QUARTER_HELIX_LARGE_DOWN
    { TRACK_HELIX_LARGE_UNBANKED,   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_RIGHT_QUARTER_HELIX_LARGE_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                  },  // ELEM_25_DEG_UP_LEFT_BANKED
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                  },  // ELEM_25_DEG_UP_RIGHT_BANKED
    { TRACK_WATERFALL,              TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_WATERFALL
    { TRACK_RAPIDS,                 TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_RAPIDS
    { TRACK_ON_RIDE_PHOTO,          TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_ON_RIDE_PHOTO
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                  },  // ELEM_25_DEG_DOWN_LEFT_BANKED
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                  },  // ELEM_25_DEG_DOWN_RIGHT_BANKED
    { TRACK_WATER_SPLASH,           TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_WATER_SPLASH
    { TRACK_FLAT,                   TRACK_SLOPE_UP_60,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_FLAT_TO_60_DEG_UP_LONG_BASE
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_60,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_60_DEG_UP_TO_FLAT_LONG_BASE
    { TRACK_WHIRLPOOL,              TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_WHIRLPOOL
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_FLAT_TO_60_DEG_DOWN_LONG_BASE
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_60_DEG_UP_TO_FLAT_LONG_BASE_122
    { TRACK_LIFT_HILL_CABLE,        TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_CABLE_LIFT_HILL
    { TRACK_REVERSE_FREEFALL,       TRACK_VANGLE_REVERSE_FREEFALL,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_REVERSE_FREEFALL_SLOPE
    { TRACK_REVERSE_FREEFALL,       TRACK_VANGLE_REVERSE_FREEFALL,  TRACK_VANGLE_REVERSE_FREEFALL,  TRACK_BANK_NONE,TRACK_BANK_NONE,        0                  },  // ELEM_REVERSE_FREEFALL_VERTICAL
    { TRACK_FLAT,                   TRACK_SLOPE_UP_90,          TRACK_SLOPE_UP_90,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_90_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_90,        TRACK_SLOPE_DOWN_90,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_90_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_UP_90,          TRACK_SLOPE_UP_60,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_60_DEG_UP_TO_90_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_DOWN_90,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_90_DEG_DOWN_TO_60_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_UP_60,          TRACK_SLOPE_UP_90,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_90_DEG_UP_TO_60_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_90,        TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_60_DEG_DOWN_TO_90_DEG_DOWN
    { TRACK_BRAKE_FOR_DROP,         TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_BRAKE_FOR_DROP
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_LEFT_EIGHTH_TO_DIAG
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_RIGHT_EIGHTH_TO_DIAG
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_LEFT_EIGHTH_TO_ORTHOGONAL
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                  },  // ELEM_LEFT_EIGHTH_BANK_TO_DIAG
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                  },  // ELEM_RIGHT_EIGHTH_BANK_TO_DIAG
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                  },  // ELEM_LEFT_EIGHTH_BANK_TO_ORTHOGONAL
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                  },  // ELEM_RIGHT_EIGHTH_BANK_TO_ORTHOGONAL
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_DIAG_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_DIAG_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_60,          TRACK_SLOPE_UP_60,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_DIAG_60_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_DIAG_FLAT_TO_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_60,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_DIAG_25_DEG_UP_TO_60_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_60,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_DIAG_60_DEG_UP_TO_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_DIAG_25_DEG_UP_TO_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_DIAG_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_DIAG_60_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_DIAG_FLAT_TO_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_DIAG_25_DEG_DOWN_TO_60_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_DIAG_60_DEG_DOWN_TO_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_DIAG_25_DEG_DOWN_TO_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_UP_60,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_DIAG_FLAT_TO_60_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_60,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_DIAG_60_DEG_UP_TO_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_DIAG_FLAT_TO_60_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_60,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_DIAG_60_DEG_DOWN_TO_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_NONE,        0                  },  // ELEM_DIAG_FLAT_TO_LEFT_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        0                  },  // ELEM_DIAG_FLAT_TO_RIGHT_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_LEFT,        0                  },  // ELEM_DIAG_LEFT_BANK_TO_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       0                  },  // ELEM_DIAG_RIGHT_BANK_TO_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_LEFT,        0                  },  // ELEM_DIAG_LEFT_BANK_TO_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       0                  },  // ELEM_DIAG_RIGHT_BANK_TO_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_LEFT,        TRACK_BANK_NONE,        0                  },  // ELEM_DIAG_25_DEG_UP_TO_LEFT_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        0                  },  // ELEM_DIAG_25_DEG_UP_TO_RIGHT_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_LEFT,        0                  },  // ELEM_DIAG_LEFT_BANK_TO_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       0                  },  // ELEM_DIAG_RIGHT_BANK_TO_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_LEFT,        TRACK_BANK_NONE,        0                  },  // ELEM_DIAG_25_DEG_DOWN_TO_LEFT_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        0                  },  // ELEM_DIAG_25_DEG_DOWN_TO_RIGHT_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                  },  // ELEM_DIAG_LEFT_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                  },  // ELEM_DIAG_RIGHT_BANK
    { TRACK_LOG_FLUME_REVERSER,     TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_LOG_FLUME_REVERSER
    { TRACK_SPINNING_TUNNEL,        TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_SPINNING_TUNNEL
    { TRACK_BARREL_ROLL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  },  // ELEM_LEFT_BARREL_ROLL_UP_TO_DOWN
    { TRACK_BARREL_ROLL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  },  // ELEM_RIGHT_BARREL_ROLL_UP_TO_DOWN
    { TRACK_BARREL_ROLL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, 0                  },  // ELEM_LEFT_BARREL_ROLL_DOWN_TO_UP
    { TRACK_BARREL_ROLL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, 0                  },  // ELEM_RIGHT_BARREL_ROLL_DOWN_TO_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_LEFT,        0                  },  // ELEM_LEFT_BANK_TO_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       0                  },  // ELEM_RIGHT_BANK_TO_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_LEFT,        TRACK_BANK_NONE,        0                  },  // ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_LEFT_BANK
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        0                  },  // ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_RIGHT_BANK
    { TRACK_POWERED_LIFT,           TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_POWERED_LIFT
    {TRACK_HALF_LOOP_LARGE,         TRACK_SLOPE_NONE,    TRACK_SLOPE_UP_25, TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        64                 },  // ELEM_LEFT_LARGE_HALF_LOOP_UP
    {TRACK_HALF_LOOP_LARGE,         TRACK_SLOPE_NONE,    TRACK_SLOPE_UP_25, TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        64                 },  // ELEM_RIGHT_LARGE_HALF_LOOP_UP
    {TRACK_HALF_LOOP_LARGE,         TRACK_SLOPE_DOWN_25, TRACK_SLOPE_NONE,  TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, -64                },  // ELEM_RIGHT_LARGE_HALF_LOOP_DOWN
    {TRACK_HALF_LOOP_LARGE,         TRACK_SLOPE_DOWN_25, TRACK_SLOPE_NONE,  TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, -64                },  // ELEM_LEFT_LARGE_HALF_LOOP_DOWN
    {TRACK_INLINE_TWIST_UNINVERTED, TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  },  // ELEM_LEFT_FLYER_TWIST_UP
    {TRACK_INLINE_TWIST_UNINVERTED, TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  },  // ELEM_RIGHT_FLYER_TWIST_UP
    {TRACK_INLINE_TWIST_INVERTED,   TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  },  // ELEM_LEFT_FLYER_TWIST_DOWN
    {TRACK_INLINE_TWIST_INVERTED,   TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  },  // ELEM_RIGHT_FLYER_TWIST_DOWN
    {TRACK_FLYING_HALF_LOOP_UNINVERTED_UP, TRACK_SLOPE_NONE,    TRACK_SLOPE_UP_25, TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE, 64                 },  // ELEM_FLYER_HALF_LOOP_UP
    {TRACK_FLYING_HALF_LOOP_INVERTED_DOWN, TRACK_SLOPE_DOWN_25, TRACK_SLOPE_NONE,  TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE, -64                },  // ELEM_FLYER_HALF_LOOP_DOWN
    {TRACK_CORKSCREW_UNINVERTED,    TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  },  // ELEM_LEFT_FLYER_CORKSCREW_UP
    {TRACK_CORKSCREW_UNINVERTED,    TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  },  // ELEM_RIGHT_FLYER_CORKSCREW_UP
    {TRACK_CORKSCREW_INVERTED,      TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        -32                },  // ELEM_LEFT_FLYER_CORKSCREW_DOWN
    {TRACK_CORKSCREW_INVERTED,      TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        -32                },  // ELEM_RIGHT_FLYER_CORKSCREW_DOWN
    {TRACK_HEARTLINE_TRANSFER,      TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_HEARTLINE_TRANSFER_UP
    {TRACK_HEARTLINE_TRANSFER,      TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_HEARTLINE_TRANSFER_DOWN
    {TRACK_HEARTLINE_ROLL,          TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_LEFT_HEARTLINE_ROLL
    {TRACK_HEARTLINE_ROLL,          TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_RIGHT_HEARTLINE_ROLL
    {TRACK_MINI_GOLF_HOLE,          TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_MINI_GOLF_HOLE_A
    {TRACK_MINI_GOLF_HOLE,          TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_MINI_GOLF_HOLE_B
    {TRACK_MINI_GOLF_HOLE,          TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_MINI_GOLF_HOLE_C
    {TRACK_MINI_GOLF_HOLE,          TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_MINI_GOLF_HOLE_D
    {TRACK_MINI_GOLF_HOLE,          TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_MINI_GOLF_HOLE_E
    { TRACK_QUARTER_LOOP_INVERTED_DOWN, TRACK_SLOPE_DOWN_90,    TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  },  // ELEM_MULTIDIM_INVERTED_FLAT_TO_90_DEG_QUARTER_LOOP_DOWN
    { TRACK_QUARTER_LOOP,           TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_90,          TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  },  // ELEM_90_DEG_TO_INVERTED_FLAT_QUARTER_LOOP_UP
    { TRACK_QUARTER_LOOP,           TRACK_SLOPE_DOWN_90,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, 0                  },  // ELEM_INVERTED_FLAT_TO_90_DEG_QUARTER_LOOP_DOWN
    { TRACK_LIFT_HILL_CURVED,       TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_LEFT_CURVED_LIFT_HILL
    { TRACK_LIFT_HILL_CURVED,       TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_RIGHT_CURVED_LIFT_HILL
    { TRACK_REVERSER,               TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_LEFT_REVERSER
    { TRACK_REVERSER,               TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_RIGHT_REVERSER
    { TRACK_SLOPE_TO_FLAT,          TRACK_SLOPE_DOWN_90,        TRACK_SLOPE_UP_90,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_AIR_THRUST_TOP_CAP
    { TRACK_SLOPE_TO_FLAT,          TRACK_SLOPE_DOWN_90,        TRACK_SLOPE_DOWN_90,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_AIR_THRUST_VERTICAL_DOWN
    { TRACK_SLOPE_TO_FLAT,          TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_90,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_AIR_THRUST_VERTICAL_DOWN_TO_LEVEL
    { TRACK_BLOCK_BRAKES,           TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_BLOCK_BRAKES
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                  },  // ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                  },  // ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                  },  // ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                  },  // ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                  },  // ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                  },  // ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                  },  // ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                  },  // ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_LEFT,        TRACK_BANK_NONE,        0                  },  // ELEM_25_DEG_UP_TO_LEFT_BANKED_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        0                  },  // ELEM_25_DEG_UP_TO_RIGHT_BANKED_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_LEFT,        0                  },  // ELEM_LEFT_BANKED_25_DEG_UP_TO_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       0                  },  // ELEM_RIGHT_BANKED_25_DEG_UP_TO_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_LEFT,        TRACK_BANK_NONE,        0                  },  // ELEM_25_DEG_DOWN_TO_LEFT_BANKED_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        0                  },  // ELEM_25_DEG_DOWN_TO_RIGHT_BANKED_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_LEFT,        0                  },  // ELEM_LEFT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       0                  },  // ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                  },  // ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                  },  // ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                  },  // ELEM_LEFT_BANKED_25_DEG_UP_TO_LEFT_BANKED_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                  },  // ELEM_RIGHT_BANKED_25_DEG_UP_TO_RIGHT_BANKED_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                  },  // ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                  },  // ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_LEFT,        TRACK_BANK_LEFT,        0                  },  // ELEM_LEFT_BANKED_25_DEG_DOWN_TO_LEFT_BANKED_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_RIGHT,       TRACK_BANK_RIGHT,       0                  },  // ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_RIGHT_BANKED_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_NONE,        0                  },  // ELEM_FLAT_TO_LEFT_BANKED_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        0                  },  // ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_LEFT,        0                  },  // ELEM_LEFT_BANKED_25_DEG_UP_TO_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       0                  },  // ELEM_RIGHT_BANKED_25_DEG_UP_TO_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_LEFT,        TRACK_BANK_NONE,        0                  },  // ELEM_FLAT_TO_LEFT_BANKED_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        0                  },  // ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_LEFT,        0                  },  // ELEM_LEFT_BANKED_25_DEG_DOWN_TO_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       0                  },  // ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_FLAT
    { TRACK_FLAT,                   TRACK_SLOPE_UP_90,          TRACK_SLOPE_UP_90,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_LEFT_QUARTER_TURN_1_TILE_90_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_UP_90,          TRACK_SLOPE_UP_90,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_RIGHT_QUARTER_TURN_1_TILE_90_DEG_UP
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_90,        TRACK_SLOPE_DOWN_90,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_LEFT_QUARTER_TURN_1_TILE_90_DEG_DOWN
    { TRACK_FLAT,                   TRACK_SLOPE_DOWN_90,        TRACK_SLOPE_DOWN_90,        TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_RIGHT_QUARTER_TURN_1_TILE_90_DEG_DOWN
    { TRACK_QUARTER_LOOP_UNINVERTED_UP, TRACK_SLOPE_NONE,       TRACK_SLOPE_UP_90,          TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  },  // ELEM_MULTIDIM_90_DEG_UP_TO_INVERTED_FLAT_QUARTER_LOOP
    { TRACK_QUARTER_LOOP_UNINVERTED_DOWN, TRACK_SLOPE_DOWN_90,  TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  },  // ELEM_MULTIDIM_FLAT_TO_90_DEG_DOWN_QUARTER_LOOP
    { TRACK_QUARTER_LOOP_INVERTED_UP,     TRACK_SLOPE_NONE,     TRACK_SLOPE_UP_90,          TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  }, // ELEM_MULTIDIM_INVERTED_FLAT_TO_90_DEG_UP_QUARTER_LOOP
    { TRACK_ROTATION_CONTROL_TOGGLE,TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_ROTATION_CONTROL_TOGGLE
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // TrackElemType::FlatTrack1x4A
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // TrackElemType::FlatTrack2x2
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // TrackElemType::FlatTrack4x4
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // TrackElemType::FlatTrack2x4
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // TrackElemType::FlatTrack1x5
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // TrackElemType::FlatTrack1x1A
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // TrackElemType::FlatTrack1x4B
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // TrackElemType::FlatTrack1x1B
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        -40                },  // TrackElemType::FlatTrack1x4C
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // TrackElemType::FlatTrack3x3
    { TRACK_CORKSCREW_LARGE,        TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  },  // TrackElemType::LeftLargeCorkscrewUp
    { TRACK_CORKSCREW_LARGE,        TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  },  // TrackElemType::RightLargeCorkscrewUp
    { TRACK_CORKSCREW_LARGE,        TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, -32                },  // TrackElemType::LeftLargeCorkscrewDown
    { TRACK_CORKSCREW_LARGE,        TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, -32                },  // TrackElemType::RightLargeCorkscrewDown
    { TRACK_HALF_LOOP_MEDIUM,       TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        64                 },  // TrackElemType::LeftMediumHalfLoopUp
    { TRACK_HALF_LOOP_MEDIUM,       TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        64                 },  // TrackElemType::RightMediumHalfLoopUp
    { TRACK_HALF_LOOP_MEDIUM,       TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, -64                },  // TrackElemType::LeftMediumHalfLoopDown
    { TRACK_HALF_LOOP_MEDIUM,       TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, -64                },  // TrackElemType::RightMediumHalfLoopDown
    { TRACK_ZERO_G_ROLL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  },  // TrackElemType::LeftZeroGRollUp
    { TRACK_ZERO_G_ROLL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  },  // TrackElemType::RightZeroGRollUp
    { TRACK_ZERO_G_ROLL,            TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, 0                  },  // TrackElemType::LeftZeroGRollDown
    { TRACK_ZERO_G_ROLL,            TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, 0                  },  // TrackElemType::RightZeroGRollDown
    { TRACK_ZERO_G_ROLL_LARGE,      TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_60,          TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  },  // TrackElemType::LeftLargeZeroGRollUp
    { TRACK_ZERO_G_ROLL_LARGE,      TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_60,          TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  },  // TrackElemType::RightLargeZeroGRollUp
    { TRACK_ZERO_G_ROLL_LARGE,      TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, 0                  },  // TrackElemType::LeftLargeZeroGRollDown
    { TRACK_ZERO_G_ROLL_LARGE,      TRACK_SLOPE_DOWN_60,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, 0                  },  // TrackElemType::RightLargeZeroGRollDown
    { TRACK_FLYING_LARGE_HALF_LOOP_UNINVERTED_UP,   TRACK_SLOPE_NONE,    TRACK_SLOPE_UP_25, TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        64                 },  // TrackElemType::LeftFlyerLargeHalfLoopUninvertedUp
    { TRACK_FLYING_LARGE_HALF_LOOP_UNINVERTED_UP,   TRACK_SLOPE_NONE,    TRACK_SLOPE_UP_25, TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        64                 },  // TrackElemType::RightFlyerLargeHalfLoopUninvertedUp                                                                                                       
    { TRACK_FLYING_LARGE_HALF_LOOP_INVERTED_DOWN,   TRACK_SLOPE_DOWN_25, TRACK_SLOPE_NONE,  TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,       -64                 },  // TrackElemType::RightFlyerLargeHalfLoopInvertedDown                                                                                                       
    { TRACK_FLYING_LARGE_HALF_LOOP_INVERTED_DOWN,   TRACK_SLOPE_DOWN_25, TRACK_SLOPE_NONE,  TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,       -64                 },  // TrackElemType::LeftFlyerLargeHalfLoopInvertedDown
    { TRACK_FLYING_LARGE_HALF_LOOP_INVERTED_UP,     TRACK_SLOPE_NONE,    TRACK_SLOPE_UP_25, TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        64                 }, // TrackElemType::LeftFlyerLargeHalfLoopInvertedUp
    { TRACK_FLYING_LARGE_HALF_LOOP_INVERTED_UP,     TRACK_SLOPE_NONE,    TRACK_SLOPE_UP_25, TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        64                 }, // TrackElemType::RightFlyerLargeHalfLoopInvertedUp
    { TRACK_FLYING_LARGE_HALF_LOOP_UNINVERTED_DOWN, TRACK_SLOPE_DOWN_25, TRACK_SLOPE_NONE,  TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,       -64                 },  // TrackElemType::RightFlyerLargeHalfLoopUninvertedDown                                                                                                     
    { TRACK_FLYING_LARGE_HALF_LOOP_UNINVERTED_DOWN, TRACK_SLOPE_DOWN_25, TRACK_SLOPE_NONE,  TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,       -64                 },  // TrackElemType::LeftFlyerLargeHalfLoopUninvertedDown
    {TRACK_FLYING_HALF_LOOP_INVERTED_UP,            TRACK_SLOPE_NONE,    TRACK_SLOPE_UP_25, TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        64                 },  // TrackElemType::FlyerHalfLoopInvertedUp
    {TRACK_FLYING_HALF_LOOP_UNINVERTED_DOWN,        TRACK_SLOPE_DOWN_25, TRACK_SLOPE_NONE,  TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        -64                },  // TrackElemType::FlyerHalfLoopUninvertedDown
};

// clang-format on

constexpr static uint8_t TrackTypeToSpinFunction[TrackElemType::Count] = {
    NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN,
    NO_SPIN, NO_SPIN, NO_SPIN, L8_SPIN, R8_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, L8_SPIN, R8_SPIN, NO_SPIN, NO_SPIN,
    NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, L8_SPIN, R8_SPIN, L8_SPIN, R8_SPIN, LR_SPIN,
    RL_SPIN, NO_SPIN, NO_SPIN, L7_SPIN, R7_SPIN, L7_SPIN, R7_SPIN, L7_SPIN, R7_SPIN, L7_SPIN, R7_SPIN, L5_SPIN, R5_SPIN,
    NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN,
    NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN,
    NO_SPIN, NO_SPIN, NO_SPIN, L8_SPIN, R8_SPIN, LR_SPIN, RL_SPIN, L7_SPIN, R7_SPIN, L7_SPIN, R7_SPIN, L7_SPIN, R7_SPIN,
    L8_SPIN, R8_SPIN, L8_SPIN, R8_SPIN, L5_SPIN, R5_SPIN, L5_SPIN, R5_SPIN, NO_SPIN, RC_SPIN, NO_SPIN, L8_SPIN, R8_SPIN,
    L8_SPIN, R8_SPIN, L8_SPIN, R8_SPIN, L8_SPIN, R8_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, SP_SPIN, NO_SPIN, NO_SPIN, NO_SPIN,
    NO_SPIN, NO_SPIN, NO_SPIN, R5_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN,
    NO_SPIN, NO_SPIN, NO_SPIN, L9_SPIN, R9_SPIN, L9_SPIN, R9_SPIN, L9_SPIN, R9_SPIN, L9_SPIN, R9_SPIN, NO_SPIN, NO_SPIN,
    NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN,
    NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN,
    NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, L7_SPIN, R7_SPIN, L7_SPIN, R7_SPIN,
    NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN,
    NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN,
    NO_SPIN, L7_SPIN, R7_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, L7_SPIN, R7_SPIN, L7_SPIN, R7_SPIN,
    L8_SPIN, R8_SPIN, L8_SPIN, R8_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN,
    NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN,
    NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN,
    NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN,
    NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN,
    NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN, NO_SPIN,
};

template<int32_t TConstant> static int32_t EvaluatorConst(const int16_t)
{
    return TConstant;
}

static int32_t EvaluatorVerticalLoop(const int16_t progress)
{
    return (abs(progress - 155) / 2) + 28;
}

static int32_t EvaluatorHalfLoopUp(const int16_t progress)
{
    return ((static_cast<uint16_t>(-(progress - 155))) / 2) + 28;
}

static int32_t EvaluatorHalfLoopDown(const int16_t progress)
{
    return (progress / 2) + 28;
}

static int32_t EvaluatorLargeZeroGRollUp(const int16_t progress)
{
    if (progress > 114)
        return (371 - 2 * progress);
    return 0;
}

static int32_t EvaluatorLargeZeroGRollDown(const int16_t progress)
{
    if (progress < 38)
        return 67 + 2 * progress;
    return 0;
}

static int32_t EvaluatorLargeZeroGRollUpLeft(const int16_t progress)
{
    return 387 - 2 * progress;
}

static int32_t EvaluatorLargeZeroGRollUpRight(const int16_t progress)
{
    return 2 * progress - 387;
}

static int32_t EvaluatorLargeZeroGRollDownLeft(const int16_t progress)
{
    return 83 + 2 * progress;
}

static int32_t EvaluatorLargeZeroGRollDownRight(const int16_t progress)
{
    return -(83 + 2 * progress);
}

static int32_t EvaluatorZeroGRollUpLeft(const int16_t progress)
{
    return 174 - progress;
}

static int32_t EvaluatorZeroGRollUpRight(const int16_t progress)
{
    return progress - 174;
}

static int32_t EvaluatorZeroGRollDownLeft(const int16_t progress)
{
    return 73 + progress;
}

static int32_t EvaluatorZeroGRollDownRight(const int16_t progress)
{
    return -(73 + progress);
}

static int32_t EvaluatorWaterSplash(const int16_t progress)
{
    if (progress < 32)
        return -150;
    if (progress < 64)
        return 150;
    if (progress < 96)
        return 0;
    if (progress < 128)
        return 150;
    return -150;
}

static int32_t EvaluatorQuarterTurn3Tiles(const int16_t progress)
{
    return -(progress / 2) + 134;
}

static int32_t EvaluatorMediumHalfLoopUp(const int16_t progress)
{
    return (244 - progress) / 4 + 51;
}

static int32_t EvaluatorMediumHalfLoopDown(const int16_t progress)
{
    return (progress / 4) + 51;
}

static int32_t EvaluatorLargeHalfLoopUp(const int16_t progress)
{
    return ((static_cast<uint16_t>(-(progress - 311))) / 4) + 46;
}

static int32_t EvaluatorLargeHalfLoopDown(const int16_t progress)
{
    return (progress / 4) + 46;
}

static int32_t EvaluatorHeartLineTransferUp(const int16_t progress)
{
    if (progress < 32)
        return 103;
    if (progress < 64)
        return -103;
    if (progress < 96)
        return 0;
    if (progress < 128)
        return 103;
    return -103;
}

static int32_t EvaluatorHeartLineTransferDown(const int16_t progress)
{
    if (progress < 32)
        return -103;
    if (progress < 64)
        return 103;
    if (progress < 96)
        return 0;
    if (progress < 128)
        return -103;
    return 103;
}

static int32_t EvaluatorDown90QuarterLoop(const int16_t progress)
{
    return (progress / 4) + 55;
}

static int32_t EvaluatorUp90QuarterLoop(const int16_t progress)
{
    return ((static_cast<uint16_t>(-(progress - 137))) / 4) + 55;
}

static int32_t EvaluatorSBendLeft(const int16_t progress)
{
    return (progress < 48) ? 98 : -98;
}

static int32_t EvaluatorSBendRight(const int16_t progress)
{
    return (progress < 48) ? -98 : 98;
}

static constexpr TrackComputeFunction GetLateralFunction(const uint16_t type)
{
    switch (type)
    {
        case TrackElemType::Flat:
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
        case TrackElemType::Up25:
        case TrackElemType::Up60: //
        case TrackElemType::Down25:
        case TrackElemType::Down60: //
        case TrackElemType::FlatToLeftBank:
        case TrackElemType::FlatToRightBank:
        case TrackElemType::LeftBankToFlat:
        case TrackElemType::RightBankToFlat: //
        case TrackElemType::LeftBank:
        case TrackElemType::RightBank:
        case TrackElemType::TowerBase:
        case TrackElemType::TowerSection:
        case TrackElemType::FlatCovered:
        case TrackElemType::Up25Covered:
        case TrackElemType::Up60Covered:
        case TrackElemType::Down25Covered:
        case TrackElemType::Down60Covered:
        case TrackElemType::Brakes:
        case TrackElemType::RotationControlToggle:
        case TrackElemType::Maze:
        case TrackElemType::Up25LeftBanked:
        case TrackElemType::Up25RightBanked:
        case TrackElemType::Waterfall:
        case TrackElemType::Rapids:
        case TrackElemType::OnRidePhoto:
        case TrackElemType::Down25LeftBanked:
        case TrackElemType::Down25RightBanked:
        case TrackElemType::Whirlpool:
        case TrackElemType::ReverseFreefallVertical:
        case TrackElemType::Up90:
        case TrackElemType::Down90:
        case TrackElemType::DiagFlat:
        case TrackElemType::DiagUp25:
        case TrackElemType::DiagUp60:
        case TrackElemType::DiagDown25:
        case TrackElemType::DiagDown60:
        case TrackElemType::DiagFlatToLeftBank:
        case TrackElemType::DiagFlatToRightBank:
        case TrackElemType::DiagLeftBankToFlat:
        case TrackElemType::DiagRightBankToFlat:
        case TrackElemType::DiagLeftBank:
        case TrackElemType::DiagRightBank:
        case TrackElemType::LogFlumeReverser:
        case TrackElemType::SpinningTunnel:
        case TrackElemType::PoweredLift:
        case TrackElemType::MinigolfHoleA:
        case TrackElemType::MinigolfHoleB:
        case TrackElemType::MinigolfHoleC:
        case TrackElemType::MinigolfHoleD:
        case TrackElemType::MinigolfHoleE:
        case TrackElemType::LeftReverser:
        case TrackElemType::RightReverser:
        case TrackElemType::AirThrustVerticalDown:
        case TrackElemType::BlockBrakes:
        case TrackElemType::Up25ToLeftBankedUp25:
        case TrackElemType::Up25ToRightBankedUp25:
        case TrackElemType::LeftBankedUp25ToUp25:
        case TrackElemType::RightBankedUp25ToUp25:
        case TrackElemType::Down25ToLeftBankedDown25:
        case TrackElemType::Down25ToRightBankedDown25:
        case TrackElemType::LeftBankedDown25ToDown25:
        case TrackElemType::RightBankedDown25ToDown25:
        case TrackElemType::LeftQuarterTurn1TileUp90:
        case TrackElemType::RightQuarterTurn1TileUp90:
        case TrackElemType::LeftQuarterTurn1TileDown90:
        case TrackElemType::RightQuarterTurn1TileDown90:
            return EvaluatorConst<0>;
        case TrackElemType::FlatToUp25:   //
        case TrackElemType::Down25ToFlat: //
        case TrackElemType::LeftBankToUp25:
        case TrackElemType::RightBankToUp25:
        case TrackElemType::Down25ToLeftBank:
        case TrackElemType::Down25ToRightBank:
        case TrackElemType::FlatToUp25Covered:
        case TrackElemType::Down25ToFlatCovered:
        case TrackElemType::LeftBankedFlatToLeftBankedUp25:
        case TrackElemType::RightBankedFlatToRightBankedUp25:
        case TrackElemType::LeftBankedDown25ToLeftBankedFlat:
        case TrackElemType::RightBankedDown25ToRightBankedFlat:
        case TrackElemType::FlatToLeftBankedUp25:
        case TrackElemType::FlatToRightBankedUp25:
        case TrackElemType::LeftBankedDown25ToFlat:
        case TrackElemType::RightBankedDown25ToFlat:
            return EvaluatorConst<0>;
        case TrackElemType::Up25ToFlat:   //
        case TrackElemType::FlatToDown25: //
        case TrackElemType::Up25ToLeftBank:
        case TrackElemType::Up25ToRightBank:
        case TrackElemType::LeftBankToDown25:
        case TrackElemType::RightBankToDown25:
        case TrackElemType::Up25ToFlatCovered:
        case TrackElemType::FlatToDown25Covered:
        case TrackElemType::CableLiftHill:
        case TrackElemType::LeftBankedUp25ToLeftBankedFlat:
        case TrackElemType::RightBankedUp25ToRightBankedFlat:
        case TrackElemType::LeftBankedFlatToLeftBankedDown25:
        case TrackElemType::RightBankedFlatToRightBankedDown25:
        case TrackElemType::LeftBankedUp25ToFlat:
        case TrackElemType::RightBankedUp25ToFlat:
        case TrackElemType::FlatToLeftBankedDown25:
        case TrackElemType::FlatToRightBankedDown25:
            return EvaluatorConst<0>;
        case TrackElemType::Up25ToUp60:     //
        case TrackElemType::Down60ToDown25: //
        case TrackElemType::Up25ToUp60Covered:
        case TrackElemType::Down60ToDown25Covered:
            return EvaluatorConst<0>;
        case TrackElemType::Up60ToUp25:     //
        case TrackElemType::Down25ToDown60: //
        case TrackElemType::Up60ToUp25Covered:
        case TrackElemType::Down25ToDown60Covered:
            return EvaluatorConst<0>;
        case TrackElemType::LeftQuarterTurn5Tiles: //
        case TrackElemType::LeftQuarterTurn5TilesUp25:
        case TrackElemType::LeftQuarterTurn5TilesDown25:
        case TrackElemType::LeftTwistDownToUp:
        case TrackElemType::LeftTwistUpToDown:
        case TrackElemType::LeftQuarterTurn5TilesCovered:
        case TrackElemType::LeftQuarterHelixLargeUp:
        case TrackElemType::LeftQuarterHelixLargeDown:
        case TrackElemType::LeftFlyerTwistUp:
        case TrackElemType::LeftFlyerTwistDown:
        case TrackElemType::LeftHeartLineRoll:
            return EvaluatorConst<98>;
        case TrackElemType::RightQuarterTurn5Tiles: //
        case TrackElemType::RightQuarterTurn5TilesUp25:
        case TrackElemType::RightQuarterTurn5TilesDown25:
        case TrackElemType::RightTwistDownToUp:
        case TrackElemType::RightTwistUpToDown:
        case TrackElemType::RightQuarterTurn5TilesCovered:
        case TrackElemType::RightQuarterHelixLargeUp:
        case TrackElemType::RightQuarterHelixLargeDown:
        case TrackElemType::RightFlyerTwistUp:
        case TrackElemType::RightFlyerTwistDown:
        case TrackElemType::RightHeartLineRoll:
            return EvaluatorConst<-98>;
        case TrackElemType::BankedLeftQuarterTurn5Tiles:
        case TrackElemType::LeftHalfBankedHelixUpLarge:
        case TrackElemType::LeftHalfBankedHelixDownLarge:
        case TrackElemType::LeftQuarterBankedHelixLargeUp:
        case TrackElemType::LeftQuarterBankedHelixLargeDown:
            return EvaluatorConst<160>;
        case TrackElemType::BankedRightQuarterTurn5Tiles:
        case TrackElemType::RightHalfBankedHelixUpLarge:
        case TrackElemType::RightHalfBankedHelixDownLarge:
        case TrackElemType::RightQuarterBankedHelixLargeUp:
        case TrackElemType::RightQuarterBankedHelixLargeDown:
            return EvaluatorConst<-160>;
        case TrackElemType::SBendLeft:
        case TrackElemType::SBendLeftCovered:
            return EvaluatorSBendLeft;
        case TrackElemType::SBendRight:
        case TrackElemType::SBendRightCovered:
            return EvaluatorSBendRight;
        case TrackElemType::LeftVerticalLoop:
        case TrackElemType::RightVerticalLoop:
            return EvaluatorConst<0>;
        case TrackElemType::LeftQuarterTurn3Tiles:
        case TrackElemType::LeftQuarterTurn3TilesUp25:
        case TrackElemType::LeftQuarterTurn3TilesDown25:
        case TrackElemType::LeftQuarterTurn3TilesCovered:
        case TrackElemType::LeftCurvedLiftHill:
            return EvaluatorConst<59>;
        case TrackElemType::RightQuarterTurn3Tiles:
        case TrackElemType::RightQuarterTurn3TilesUp25:
        case TrackElemType::RightQuarterTurn3TilesDown25:
        case TrackElemType::RightQuarterTurn3TilesCovered:
        case TrackElemType::RightCurvedLiftHill:
            return EvaluatorConst<-59>;
        case TrackElemType::LeftBankedQuarterTurn3Tiles:
        case TrackElemType::LeftHalfBankedHelixUpSmall:
        case TrackElemType::LeftHalfBankedHelixDownSmall:
            return EvaluatorConst<100>;
        case TrackElemType::RightBankedQuarterTurn3Tiles:
        case TrackElemType::RightHalfBankedHelixUpSmall:
        case TrackElemType::RightHalfBankedHelixDownSmall:
            return EvaluatorConst<-100>;
        case TrackElemType::LeftQuarterTurn1Tile:
            return EvaluatorConst<45>;
        case TrackElemType::RightQuarterTurn1Tile:
            return EvaluatorConst<-45>;
        case TrackElemType::HalfLoopUp:
        case TrackElemType::FlyerHalfLoopUninvertedUp:
        case TrackElemType::FlyerHalfLoopInvertedUp:
            return EvaluatorConst<0>;
        case TrackElemType::HalfLoopDown:
        case TrackElemType::FlyerHalfLoopInvertedDown:
        case TrackElemType::FlyerHalfLoopUninvertedDown:
            return EvaluatorConst<0>;
        case TrackElemType::LeftLargeCorkscrewUp:
        case TrackElemType::RightLargeCorkscrewDown:
            return EvaluatorConst<117>;
        case TrackElemType::RightLargeCorkscrewUp:
        case TrackElemType::LeftLargeCorkscrewDown:
            return EvaluatorConst<-117>;
        case TrackElemType::LeftCorkscrewUp:
        case TrackElemType::RightCorkscrewDown:
        case TrackElemType::LeftFlyerCorkscrewUp:
        case TrackElemType::RightFlyerCorkscrewDown:
            return EvaluatorConst<70>;
        case TrackElemType::RightCorkscrewUp:
        case TrackElemType::LeftCorkscrewDown:
        case TrackElemType::RightFlyerCorkscrewUp:
        case TrackElemType::LeftFlyerCorkscrewDown:
            return EvaluatorConst<-70>;
        case TrackElemType::FlatToUp60:
        case TrackElemType::Down60ToFlat:
            return EvaluatorConst<0>;
        case TrackElemType::Up60ToFlat:
        case TrackElemType::FlatToDown60:
        case TrackElemType::BrakeForDrop:
            return EvaluatorConst<0>;
        case TrackElemType::LeftQuarterTurn1TileUp60:
        case TrackElemType::LeftQuarterTurn1TileDown60:
            return EvaluatorConst<88>;
        case TrackElemType::RightQuarterTurn1TileUp60:
        case TrackElemType::RightQuarterTurn1TileDown60:
            return EvaluatorConst<-88>;
        case TrackElemType::Watersplash:
            return EvaluatorConst<0>;
        case TrackElemType::FlatToUp60LongBase:
        case TrackElemType::Down60ToFlatLongBase:
            return EvaluatorConst<0>;
        case TrackElemType::Up60ToFlatLongBase:
        case TrackElemType::FlatToDown60LongBase:
            return EvaluatorConst<0>;
        case TrackElemType::ReverseFreefallSlope:
        case TrackElemType::AirThrustVerticalDownToLevel:
            return EvaluatorConst<0>;
        case TrackElemType::Up60ToUp90:
        case TrackElemType::Down90ToDown60:
            return EvaluatorConst<0>;
        case TrackElemType::Up90ToUp60:
        case TrackElemType::Down60ToDown90:
            return EvaluatorConst<0>;
        case TrackElemType::LeftEighthToDiag:
        case TrackElemType::LeftEighthToOrthogonal:
            return EvaluatorConst<137>;
        case TrackElemType::RightEighthToDiag:
        case TrackElemType::RightEighthToOrthogonal:
            return EvaluatorConst<-137>;
        case TrackElemType::LeftEighthBankToDiag:
        case TrackElemType::LeftEighthBankToOrthogonal:
            return EvaluatorConst<200>;
        case TrackElemType::RightEighthBankToDiag:
        case TrackElemType::RightEighthBankToOrthogonal:
            return EvaluatorConst<-200>;
        case TrackElemType::DiagFlatToUp25:
        case TrackElemType::DiagDown25ToFlat:
        case TrackElemType::DiagLeftBankToUp25:
        case TrackElemType::DiagRightBankToUp25:
        case TrackElemType::DiagDown25ToLeftBank:
        case TrackElemType::DiagDown25ToRightBank:
            return EvaluatorConst<0>;
        case TrackElemType::DiagUp25ToFlat:
        case TrackElemType::DiagFlatToDown25:
        case TrackElemType::DiagUp25ToLeftBank:
        case TrackElemType::DiagUp25ToRightBank:
        case TrackElemType::DiagLeftBankToDown25:
        case TrackElemType::DiagRightBankToDown25:
            return EvaluatorConst<0>;
        case TrackElemType::DiagUp25ToUp60:
        case TrackElemType::DiagDown60ToDown25:
            return EvaluatorConst<0>;
        case TrackElemType::DiagUp60ToUp25:
        case TrackElemType::DiagDown25ToDown60:
            return EvaluatorConst<0>;
        case TrackElemType::DiagFlatToUp60:
        case TrackElemType::DiagDown60ToFlat:
            return EvaluatorConst<0>;
        case TrackElemType::DiagUp60ToFlat:
        case TrackElemType::DiagFlatToDown60:
            return EvaluatorConst<0>;
        case TrackElemType::LeftBarrelRollUpToDown:
        case TrackElemType::LeftBarrelRollDownToUp:
            return EvaluatorConst<115>;
        case TrackElemType::RightBarrelRollUpToDown:
        case TrackElemType::RightBarrelRollDownToUp:
            return EvaluatorConst<-115>;
        case TrackElemType::LeftZeroGRollUp:
            return EvaluatorZeroGRollUpLeft;
        case TrackElemType::RightZeroGRollUp:
            return EvaluatorZeroGRollUpRight;
        case TrackElemType::LeftZeroGRollDown:
            return EvaluatorZeroGRollDownLeft;
        case TrackElemType::RightZeroGRollDown:
            return EvaluatorZeroGRollDownRight;
        case TrackElemType::LeftLargeZeroGRollUp:
            return EvaluatorLargeZeroGRollUpLeft;
        case TrackElemType::RightLargeZeroGRollUp:
            return EvaluatorLargeZeroGRollUpRight;
        case TrackElemType::LeftLargeZeroGRollDown:
            return EvaluatorLargeZeroGRollDownLeft;
        case TrackElemType::RightLargeZeroGRollDown:
            return EvaluatorLargeZeroGRollDownRight;
        case TrackElemType::LeftBankToLeftQuarterTurn3TilesUp25:
            return EvaluatorConst<90>;
        case TrackElemType::RightBankToRightQuarterTurn3TilesUp25:
            return EvaluatorConst<-90>;
        case TrackElemType::LeftQuarterTurn3TilesDown25ToLeftBank:
            return EvaluatorConst<90>;
        case TrackElemType::RightQuarterTurn3TilesDown25ToRightBank:
            return EvaluatorConst<-90>;
        case TrackElemType::LeftLargeHalfLoopUp:
        case TrackElemType::RightLargeHalfLoopUp:
            return EvaluatorConst<0>;
        case TrackElemType::RightLargeHalfLoopDown:
        case TrackElemType::LeftLargeHalfLoopDown:
            return EvaluatorConst<0>;
        case TrackElemType::HeartLineTransferUp:
            return EvaluatorConst<0>;
        case TrackElemType::HeartLineTransferDown:
            return EvaluatorConst<0>;
        case TrackElemType::MultiDimInvertedFlatToDown90QuarterLoop:
        case TrackElemType::InvertedFlatToDown90QuarterLoop:
        case TrackElemType::MultiDimFlatToDown90QuarterLoop:
            return EvaluatorConst<0>;
        case TrackElemType::Up90ToInvertedFlatQuarterLoop:
        case TrackElemType::MultiDimUp90ToInvertedFlatQuarterLoop:
        case TrackElemType::MultiDimInvertedUp90ToFlatQuarterLoop:
            return EvaluatorConst<0>;
        case TrackElemType::AirThrustTopCap:
            return EvaluatorConst<0>;
        case TrackElemType::LeftBankedQuarterTurn3TileUp25:
        case TrackElemType::LeftBankedQuarterTurn3TileDown25:
            return EvaluatorConst<100>;
        case TrackElemType::RightBankedQuarterTurn3TileUp25:
        case TrackElemType::RightBankedQuarterTurn3TileDown25:
            return EvaluatorConst<-100>;
        case TrackElemType::LeftBankedQuarterTurn5TileUp25:
        case TrackElemType::LeftBankedQuarterTurn5TileDown25:
            return EvaluatorConst<160>;
        case TrackElemType::RightBankedQuarterTurn5TileUp25:
        case TrackElemType::RightBankedQuarterTurn5TileDown25:
            return EvaluatorConst<-160>;
        default:
            return EvaluatorConst<0>;
    }
}

static constexpr TrackComputeFunction GetVerticalFunction(const uint16_t type)
{
    switch (type)
    {
        case TrackElemType::Flat:
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
        case TrackElemType::Up25:
        case TrackElemType::Up60: //
        case TrackElemType::Down25:
        case TrackElemType::Down60: //
        case TrackElemType::FlatToLeftBank:
        case TrackElemType::FlatToRightBank:
        case TrackElemType::LeftBankToFlat:
        case TrackElemType::RightBankToFlat: //
        case TrackElemType::LeftBank:
        case TrackElemType::RightBank:
        case TrackElemType::TowerBase:
        case TrackElemType::TowerSection:
        case TrackElemType::FlatCovered:
        case TrackElemType::Up25Covered:
        case TrackElemType::Up60Covered:
        case TrackElemType::Down25Covered:
        case TrackElemType::Down60Covered:
        case TrackElemType::Brakes:
        case TrackElemType::RotationControlToggle:
        case TrackElemType::Maze:
        case TrackElemType::Up25LeftBanked:
        case TrackElemType::Up25RightBanked:
        case TrackElemType::Waterfall:
        case TrackElemType::Rapids:
        case TrackElemType::OnRidePhoto:
        case TrackElemType::Down25LeftBanked:
        case TrackElemType::Down25RightBanked:
        case TrackElemType::Whirlpool:
        case TrackElemType::ReverseFreefallVertical:
        case TrackElemType::Up90:
        case TrackElemType::Down90:
        case TrackElemType::DiagFlat:
        case TrackElemType::DiagUp25:
        case TrackElemType::DiagUp60:
        case TrackElemType::DiagDown25:
        case TrackElemType::DiagDown60:
        case TrackElemType::DiagFlatToLeftBank:
        case TrackElemType::DiagFlatToRightBank:
        case TrackElemType::DiagLeftBankToFlat:
        case TrackElemType::DiagRightBankToFlat:
        case TrackElemType::DiagLeftBank:
        case TrackElemType::DiagRightBank:
        case TrackElemType::LogFlumeReverser:
        case TrackElemType::SpinningTunnel:
        case TrackElemType::PoweredLift:
        case TrackElemType::MinigolfHoleA:
        case TrackElemType::MinigolfHoleB:
        case TrackElemType::MinigolfHoleC:
        case TrackElemType::MinigolfHoleD:
        case TrackElemType::MinigolfHoleE:
        case TrackElemType::LeftReverser:
        case TrackElemType::RightReverser:
        case TrackElemType::AirThrustVerticalDown:
        case TrackElemType::BlockBrakes:
        case TrackElemType::Up25ToLeftBankedUp25:
        case TrackElemType::Up25ToRightBankedUp25:
        case TrackElemType::LeftBankedUp25ToUp25:
        case TrackElemType::RightBankedUp25ToUp25:
        case TrackElemType::Down25ToLeftBankedDown25:
        case TrackElemType::Down25ToRightBankedDown25:
        case TrackElemType::LeftBankedDown25ToDown25:
        case TrackElemType::RightBankedDown25ToDown25:
        case TrackElemType::LeftQuarterTurn1TileUp90:
        case TrackElemType::RightQuarterTurn1TileUp90:
        case TrackElemType::LeftQuarterTurn1TileDown90:
        case TrackElemType::RightQuarterTurn1TileDown90:
            return EvaluatorConst<0>;
        case TrackElemType::FlatToUp25:   //
        case TrackElemType::Down25ToFlat: //
        case TrackElemType::LeftBankToUp25:
        case TrackElemType::RightBankToUp25:
        case TrackElemType::Down25ToLeftBank:
        case TrackElemType::Down25ToRightBank:
        case TrackElemType::FlatToUp25Covered:
        case TrackElemType::Down25ToFlatCovered:
        case TrackElemType::LeftBankedFlatToLeftBankedUp25:
        case TrackElemType::RightBankedFlatToRightBankedUp25:
        case TrackElemType::LeftBankedDown25ToLeftBankedFlat:
        case TrackElemType::RightBankedDown25ToRightBankedFlat:
        case TrackElemType::FlatToLeftBankedUp25:
        case TrackElemType::FlatToRightBankedUp25:
        case TrackElemType::LeftBankedDown25ToFlat:
        case TrackElemType::RightBankedDown25ToFlat:
            return EvaluatorConst<103>;
        case TrackElemType::Up25ToFlat:   //
        case TrackElemType::FlatToDown25: //
        case TrackElemType::Up25ToLeftBank:
        case TrackElemType::Up25ToRightBank:
        case TrackElemType::LeftBankToDown25:
        case TrackElemType::RightBankToDown25:
        case TrackElemType::Up25ToFlatCovered:
        case TrackElemType::FlatToDown25Covered:
        case TrackElemType::CableLiftHill:
        case TrackElemType::LeftBankedUp25ToLeftBankedFlat:
        case TrackElemType::RightBankedUp25ToRightBankedFlat:
        case TrackElemType::LeftBankedFlatToLeftBankedDown25:
        case TrackElemType::RightBankedFlatToRightBankedDown25:
        case TrackElemType::LeftBankedUp25ToFlat:
        case TrackElemType::RightBankedUp25ToFlat:
        case TrackElemType::FlatToLeftBankedDown25:
        case TrackElemType::FlatToRightBankedDown25:
            return EvaluatorConst<-103>;
        case TrackElemType::Up25ToUp60:     //
        case TrackElemType::Down60ToDown25: //
        case TrackElemType::Up25ToUp60Covered:
        case TrackElemType::Down60ToDown25Covered:
            return EvaluatorConst<82>;
        case TrackElemType::Up60ToUp25:     //
        case TrackElemType::Down25ToDown60: //
        case TrackElemType::Up60ToUp25Covered:
        case TrackElemType::Down25ToDown60Covered:
            return EvaluatorConst<-82>;
        case TrackElemType::LeftQuarterTurn5Tiles: //
        case TrackElemType::LeftQuarterTurn5TilesUp25:
        case TrackElemType::LeftQuarterTurn5TilesDown25:
        case TrackElemType::LeftTwistDownToUp:
        case TrackElemType::LeftTwistUpToDown:
        case TrackElemType::LeftQuarterTurn5TilesCovered:
        case TrackElemType::LeftQuarterHelixLargeUp:
        case TrackElemType::LeftQuarterHelixLargeDown:
        case TrackElemType::LeftFlyerTwistUp:
        case TrackElemType::LeftFlyerTwistDown:
        case TrackElemType::LeftHeartLineRoll:
            return EvaluatorConst<0>;
        case TrackElemType::RightQuarterTurn5Tiles: //
        case TrackElemType::RightQuarterTurn5TilesUp25:
        case TrackElemType::RightQuarterTurn5TilesDown25:
        case TrackElemType::RightTwistDownToUp:
        case TrackElemType::RightTwistUpToDown:
        case TrackElemType::RightQuarterTurn5TilesCovered:
        case TrackElemType::RightQuarterHelixLargeUp:
        case TrackElemType::RightQuarterHelixLargeDown:
        case TrackElemType::RightFlyerTwistUp:
        case TrackElemType::RightFlyerTwistDown:
        case TrackElemType::RightHeartLineRoll:
            return EvaluatorConst<0>;
        case TrackElemType::BankedLeftQuarterTurn5Tiles:
        case TrackElemType::LeftHalfBankedHelixUpLarge:
        case TrackElemType::LeftHalfBankedHelixDownLarge:
        case TrackElemType::LeftQuarterBankedHelixLargeUp:
        case TrackElemType::LeftQuarterBankedHelixLargeDown:
            return EvaluatorConst<200>;
        case TrackElemType::BankedRightQuarterTurn5Tiles:
        case TrackElemType::RightHalfBankedHelixUpLarge:
        case TrackElemType::RightHalfBankedHelixDownLarge:
        case TrackElemType::RightQuarterBankedHelixLargeUp:
        case TrackElemType::RightQuarterBankedHelixLargeDown:
            return EvaluatorConst<200>;
        case TrackElemType::SBendLeft:
        case TrackElemType::SBendLeftCovered:
            return EvaluatorConst<0>;
        case TrackElemType::SBendRight:
        case TrackElemType::SBendRightCovered:
            return EvaluatorConst<0>;
        case TrackElemType::LeftVerticalLoop:
        case TrackElemType::RightVerticalLoop:
            return EvaluatorVerticalLoop;
        case TrackElemType::LeftQuarterTurn3Tiles:
        case TrackElemType::LeftQuarterTurn3TilesUp25:
        case TrackElemType::LeftQuarterTurn3TilesDown25:
        case TrackElemType::LeftQuarterTurn3TilesCovered:
        case TrackElemType::LeftCurvedLiftHill:
            return EvaluatorConst<0>;
        case TrackElemType::RightQuarterTurn3Tiles:
        case TrackElemType::RightQuarterTurn3TilesUp25:
        case TrackElemType::RightQuarterTurn3TilesDown25:
        case TrackElemType::RightQuarterTurn3TilesCovered:
        case TrackElemType::RightCurvedLiftHill:
            return EvaluatorConst<0>;
        case TrackElemType::LeftBankedQuarterTurn3Tiles:
        case TrackElemType::LeftHalfBankedHelixUpSmall:
        case TrackElemType::LeftHalfBankedHelixDownSmall:
            return EvaluatorConst<100>;
        case TrackElemType::RightBankedQuarterTurn3Tiles:
        case TrackElemType::RightHalfBankedHelixUpSmall:
        case TrackElemType::RightHalfBankedHelixDownSmall:
            return EvaluatorConst<100>;
        case TrackElemType::LeftQuarterTurn1Tile:
            return EvaluatorConst<0>;
        case TrackElemType::RightQuarterTurn1Tile:
            return EvaluatorConst<0>;
        case TrackElemType::HalfLoopUp:
        case TrackElemType::FlyerHalfLoopUninvertedUp:
        case TrackElemType::FlyerHalfLoopInvertedUp:
            return EvaluatorHalfLoopUp;
        case TrackElemType::HalfLoopDown:
        case TrackElemType::FlyerHalfLoopInvertedDown:
        case TrackElemType::FlyerHalfLoopUninvertedDown:
            return EvaluatorHalfLoopDown;
        case TrackElemType::LeftLargeCorkscrewUp:
        case TrackElemType::RightLargeCorkscrewDown:
            return EvaluatorConst<89>;
        case TrackElemType::RightLargeCorkscrewUp:
        case TrackElemType::LeftLargeCorkscrewDown:
            return EvaluatorConst<89>;
        case TrackElemType::LeftCorkscrewUp:
        case TrackElemType::RightCorkscrewDown:
        case TrackElemType::LeftFlyerCorkscrewUp:
        case TrackElemType::RightFlyerCorkscrewDown:
            return EvaluatorConst<52>;
        case TrackElemType::RightCorkscrewUp:
        case TrackElemType::LeftCorkscrewDown:
        case TrackElemType::RightFlyerCorkscrewUp:
        case TrackElemType::LeftFlyerCorkscrewDown:
            return EvaluatorConst<52>;
        case TrackElemType::FlatToUp60:
        case TrackElemType::Down60ToFlat:
            return EvaluatorConst<56>;
        case TrackElemType::Up60ToFlat:
        case TrackElemType::FlatToDown60:
        case TrackElemType::BrakeForDrop:
            return EvaluatorConst<-56>;
        case TrackElemType::LeftQuarterTurn1TileUp60:
        case TrackElemType::LeftQuarterTurn1TileDown60:
            return EvaluatorConst<0>;
        case TrackElemType::RightQuarterTurn1TileUp60:
        case TrackElemType::RightQuarterTurn1TileDown60:
            return EvaluatorConst<0>;
        case TrackElemType::Watersplash:
            return EvaluatorWaterSplash;
        case TrackElemType::FlatToUp60LongBase:
        case TrackElemType::Down60ToFlatLongBase:
            return EvaluatorConst<160>;
        case TrackElemType::Up60ToFlatLongBase:
        case TrackElemType::FlatToDown60LongBase:
            return EvaluatorConst<-160>;
        case TrackElemType::ReverseFreefallSlope:
        case TrackElemType::AirThrustVerticalDownToLevel:
            return EvaluatorConst<120>;
        case TrackElemType::Up60ToUp90:
        case TrackElemType::Down90ToDown60:
            return EvaluatorConst<110>;
        case TrackElemType::Up90ToUp60:
        case TrackElemType::Down60ToDown90:
            return EvaluatorConst<-110>;
        case TrackElemType::LeftEighthToDiag:
        case TrackElemType::LeftEighthToOrthogonal:
            return EvaluatorConst<0>;
        case TrackElemType::RightEighthToDiag:
        case TrackElemType::RightEighthToOrthogonal:
            return EvaluatorConst<0>;
        case TrackElemType::LeftEighthBankToDiag:
        case TrackElemType::LeftEighthBankToOrthogonal:
            return EvaluatorConst<270>;
        case TrackElemType::RightEighthBankToDiag:
        case TrackElemType::RightEighthBankToOrthogonal:
            return EvaluatorConst<270>;
        case TrackElemType::DiagFlatToUp25:
        case TrackElemType::DiagDown25ToFlat:
        case TrackElemType::DiagLeftBankToUp25:
        case TrackElemType::DiagRightBankToUp25:
        case TrackElemType::DiagDown25ToLeftBank:
        case TrackElemType::DiagDown25ToRightBank:
            return EvaluatorConst<113>;
        case TrackElemType::DiagUp25ToFlat:
        case TrackElemType::DiagFlatToDown25:
        case TrackElemType::DiagUp25ToLeftBank:
        case TrackElemType::DiagUp25ToRightBank:
        case TrackElemType::DiagLeftBankToDown25:
        case TrackElemType::DiagRightBankToDown25:
            return EvaluatorConst<-113>;
        case TrackElemType::DiagUp25ToUp60:
        case TrackElemType::DiagDown60ToDown25:
            return EvaluatorConst<95>;
        case TrackElemType::DiagUp60ToUp25:
        case TrackElemType::DiagDown25ToDown60:
            return EvaluatorConst<-95>;
        case TrackElemType::DiagFlatToUp60:
        case TrackElemType::DiagDown60ToFlat:
            return EvaluatorConst<60>;
        case TrackElemType::DiagUp60ToFlat:
        case TrackElemType::DiagFlatToDown60:
            return EvaluatorConst<-60>;
        case TrackElemType::LeftBarrelRollUpToDown:
        case TrackElemType::LeftBarrelRollDownToUp:
            return EvaluatorConst<170>;
        case TrackElemType::RightBarrelRollUpToDown:
        case TrackElemType::RightBarrelRollDownToUp:
            return EvaluatorConst<170>;
        case TrackElemType::LeftZeroGRollUp:
        case TrackElemType::RightZeroGRollUp:
            return EvaluatorConst<250>;
        case TrackElemType::LeftZeroGRollDown:
        case TrackElemType::RightZeroGRollDown:
            return EvaluatorConst<250>;
        case TrackElemType::LeftLargeZeroGRollUp:
        case TrackElemType::RightLargeZeroGRollUp:
            return EvaluatorLargeZeroGRollUp;
        case TrackElemType::LeftLargeZeroGRollDown:
        case TrackElemType::RightLargeZeroGRollDown:
            return EvaluatorLargeZeroGRollDown;
        case TrackElemType::LeftBankToLeftQuarterTurn3TilesUp25:
        case TrackElemType::RightBankToRightQuarterTurn3TilesUp25:
        case TrackElemType::LeftQuarterTurn3TilesDown25ToLeftBank:
        case TrackElemType::RightQuarterTurn3TilesDown25ToRightBank:
            return EvaluatorQuarterTurn3Tiles;
        case TrackElemType::LeftMediumHalfLoopUp:
        case TrackElemType::RightMediumHalfLoopUp:
            return EvaluatorMediumHalfLoopUp;
        case TrackElemType::RightMediumHalfLoopDown:
        case TrackElemType::LeftMediumHalfLoopDown:
            return EvaluatorMediumHalfLoopDown;
        case TrackElemType::LeftLargeHalfLoopUp:
        case TrackElemType::RightLargeHalfLoopUp:
        case TrackElemType::LeftFlyerLargeHalfLoopUninvertedUp:
        case TrackElemType::RightFlyerLargeHalfLoopUninvertedUp:
        case TrackElemType::RightFlyerLargeHalfLoopInvertedUp:
        case TrackElemType::LeftFlyerLargeHalfLoopInvertedUp:
            return EvaluatorLargeHalfLoopUp;
        case TrackElemType::RightLargeHalfLoopDown:
        case TrackElemType::LeftLargeHalfLoopDown:
        case TrackElemType::LeftFlyerLargeHalfLoopInvertedDown:
        case TrackElemType::RightFlyerLargeHalfLoopInvertedDown:
        case TrackElemType::LeftFlyerLargeHalfLoopUninvertedDown:
        case TrackElemType::RightFlyerLargeHalfLoopUninvertedDown:
            return EvaluatorLargeHalfLoopDown;
        case TrackElemType::HeartLineTransferUp:
            return EvaluatorHeartLineTransferUp;
        case TrackElemType::HeartLineTransferDown:
            return EvaluatorHeartLineTransferDown;
        case TrackElemType::MultiDimInvertedFlatToDown90QuarterLoop:
        case TrackElemType::InvertedFlatToDown90QuarterLoop:
        case TrackElemType::MultiDimFlatToDown90QuarterLoop:
            return EvaluatorDown90QuarterLoop;
        case TrackElemType::Up90ToInvertedFlatQuarterLoop:
        case TrackElemType::MultiDimUp90ToInvertedFlatQuarterLoop:
        case TrackElemType::MultiDimInvertedUp90ToFlatQuarterLoop:
            return EvaluatorUp90QuarterLoop;
        case TrackElemType::AirThrustTopCap:
            return EvaluatorConst<-60>;
        case TrackElemType::LeftBankedQuarterTurn3TileUp25:
        case TrackElemType::LeftBankedQuarterTurn3TileDown25:
            return EvaluatorConst<200>;
        case TrackElemType::RightBankedQuarterTurn3TileUp25:
        case TrackElemType::RightBankedQuarterTurn3TileDown25:
            return EvaluatorConst<200>;
        case TrackElemType::LeftBankedQuarterTurn5TileUp25:
        case TrackElemType::LeftBankedQuarterTurn5TileDown25:
            return EvaluatorConst<200>;
        case TrackElemType::RightBankedQuarterTurn5TileUp25:
        case TrackElemType::RightBankedQuarterTurn5TileDown25:
            return EvaluatorConst<200>;
        default:
            return EvaluatorConst<0>;
    }
}

static constexpr const StringId RideConfigurationStringIds[] = {
    STR_EMPTY,                         // 0
    STR_STATION_PLATFORM,              // 1
    STR_EMPTY,                         // 2
    STR_EMPTY,                         // 3
    STR_EMPTY,                         // 4
    STR_EMPTY,                         // 5
    STR_EMPTY,                         // 6
    STR_EMPTY,                         // 7
    STR_EMPTY,                         // 8
    STR_EMPTY,                         // 9
    STR_EMPTY,                         // 10
    STR_EMPTY,                         // 11
    STR_EMPTY,                         // 12
    STR_EMPTY,                         // 13
    STR_EMPTY,                         // 14
    STR_EMPTY,                         // 15
    STR_EMPTY,                         // 16
    STR_EMPTY,                         // 17
    STR_EMPTY,                         // 18
    STR_EMPTY,                         // 19
    STR_EMPTY,                         // 20
    STR_EMPTY,                         // 21
    STR_EMPTY,                         // 22
    STR_EMPTY,                         // 23
    STR_EMPTY,                         // 24
    STR_EMPTY,                         // 25
    STR_EMPTY,                         // 26
    STR_EMPTY,                         // 27
    STR_EMPTY,                         // 28
    STR_EMPTY,                         // 29
    STR_EMPTY,                         // 30
    STR_EMPTY,                         // 31
    STR_EMPTY,                         // 32
    STR_EMPTY,                         // 33
    STR_EMPTY,                         // 34
    STR_EMPTY,                         // 35
    STR_EMPTY,                         // 36
    STR_EMPTY,                         // 37
    STR_S_BEND_LEFT,                   // 38
    STR_S_BEND_RIGHT,                  // 39
    STR_VERTICAL_LOOP_LEFT,            // 40
    STR_VERTICAL_LOOP_RIGHT,           // 41
    STR_EMPTY,                         // 42
    STR_EMPTY,                         // 43
    STR_EMPTY,                         // 44
    STR_EMPTY,                         // 45
    STR_EMPTY,                         // 46
    STR_EMPTY,                         // 47
    STR_EMPTY,                         // 48
    STR_EMPTY,                         // 49
    STR_EMPTY,                         // 50
    STR_EMPTY,                         // 51
    STR_IN_LINE_TWIST_LEFT,            // 52
    STR_IN_LINE_TWIST_RIGHT,           // 53
    STR_IN_LINE_TWIST_LEFT,            // 54
    STR_IN_LINE_TWIST_RIGHT,           // 55
    STR_HALF_LOOP,                     // 56
    STR_HALF_LOOP,                     // 57
    STR_HALF_CORKSCREW_LEFT,           // 58
    STR_HALF_CORKSCREW_RIGHT,          // 59
    STR_HALF_CORKSCREW_LEFT,           // 60
    STR_HALF_CORKSCREW_RIGHT,          // 61
    STR_EMPTY,                         // 62
    STR_EMPTY,                         // 63
    STR_EMPTY,                         // 64
    STR_EMPTY,                         // 65
    STR_ENTRY_EXIT_PLATFORM,           // 66
    STR_VERTICAL_TOWER,                // 67
    STR_EMPTY,                         // 68
    STR_EMPTY,                         // 69
    STR_EMPTY,                         // 70
    STR_EMPTY,                         // 71
    STR_EMPTY,                         // 72
    STR_EMPTY,                         // 73
    STR_EMPTY,                         // 74
    STR_EMPTY,                         // 75
    STR_EMPTY,                         // 76
    STR_EMPTY,                         // 77
    STR_EMPTY,                         // 78
    STR_EMPTY,                         // 79
    STR_EMPTY,                         // 80
    STR_EMPTY,                         // 81
    STR_EMPTY,                         // 82
    STR_S_BEND_LEFT,                   // 83
    STR_S_BEND_RIGHT,                  // 84
    STR_EMPTY,                         // 85
    STR_EMPTY,                         // 86
    STR_HELIX_UP_SMALL,                // 87
    STR_HELIX_UP_SMALL,                // 88
    STR_HELIX_DOWN_SMALL,              // 89
    STR_HELIX_DOWN_SMALL,              // 90
    STR_HELIX_UP_LARGE,                // 91
    STR_HELIX_UP_LARGE,                // 92
    STR_HELIX_DOWN_LARGE,              // 93
    STR_HELIX_DOWN_LARGE,              // 94
    STR_EMPTY,                         // 95
    STR_EMPTY,                         // 96
    STR_EMPTY,                         // 97
    STR_EMPTY,                         // 98
    STR_BRAKES,                        // 99
    STR_BOOSTER,                       // 100
    STR_EMPTY,                         // 101
    STR_HELIX_UP_LARGE,                // 102
    STR_HELIX_UP_LARGE,                // 103
    STR_HELIX_DOWN_LARGE,              // 104
    STR_HELIX_DOWN_LARGE,              // 105
    STR_HELIX_UP_LEFT,                 // 106
    STR_HELIX_UP_RIGHT,                // 107
    STR_HELIX_DOWN_LEFT,               // 108
    STR_HELIX_DOWN_RIGHT,              // 109
    STR_BASE_SIZE_2_X_2,               // 110
    STR_BASE_SIZE_4_X_4,               // 111
    STR_WATERFALLS,                    // 112
    STR_RAPIDS,                        // 113
    STR_ON_RIDE_PHOTO_SECTION,         // 114
    STR_BASE_SIZE_2_X_4,               // 115
    STR_BASE_SIZE_5_X_1,               // 116
    STR_WATER_SPLASH,                  // 117
    STR_EMPTY,                         // 118
    STR_EMPTY,                         // 119
    STR_WHIRLPOOL,                     // 120
    STR_EMPTY,                         // 121
    STR_EMPTY,                         // 122
    STR_CABLE_LIFT_HILL,               // 123
    STR_SLOPE_UP_TO_VERTICAL,          // 124
    STR_VERTICAL_TRACK,                // 125
    STR_EMPTY,                         // 126
    STR_EMPTY,                         // 127
    STR_EMPTY,                         // 128
    STR_EMPTY,                         // 129
    STR_EMPTY,                         // 130
    STR_EMPTY,                         // 131
    STR_HOLDING_BRAKE_FOR_DROP,        // 132
    STR_EMPTY,                         // 133
    STR_EMPTY,                         // 134
    STR_EMPTY,                         // 135
    STR_EMPTY,                         // 136
    STR_EMPTY,                         // 137
    STR_EMPTY,                         // 138
    STR_EMPTY,                         // 139
    STR_EMPTY,                         // 140
    STR_EMPTY,                         // 141
    STR_EMPTY,                         // 142
    STR_EMPTY,                         // 143
    STR_EMPTY,                         // 144
    STR_EMPTY,                         // 145
    STR_EMPTY,                         // 146
    STR_EMPTY,                         // 147
    STR_EMPTY,                         // 148
    STR_EMPTY,                         // 149
    STR_EMPTY,                         // 150
    STR_EMPTY,                         // 151
    STR_EMPTY,                         // 152
    STR_EMPTY,                         // 153
    STR_EMPTY,                         // 154
    STR_EMPTY,                         // 155
    STR_EMPTY,                         // 156
    STR_EMPTY,                         // 157
    STR_EMPTY,                         // 158
    STR_EMPTY,                         // 159
    STR_EMPTY,                         // 160
    STR_EMPTY,                         // 161
    STR_EMPTY,                         // 162
    STR_EMPTY,                         // 163
    STR_EMPTY,                         // 164
    STR_EMPTY,                         // 165
    STR_EMPTY,                         // 166
    STR_EMPTY,                         // 167
    STR_EMPTY,                         // 168
    STR_EMPTY,                         // 169
    STR_EMPTY,                         // 170
    STR_EMPTY,                         // 171
    STR_REVERSER_TURNTABLE,            // 172
    STR_SPINNING_TUNNEL,               // 173
    STR_BARREL_ROLL_LEFT,              // 174
    STR_BARREL_ROLL_RIGHT,             // 175
    STR_BARREL_ROLL_LEFT,              // 176
    STR_BARREL_ROLL_RIGHT,             // 177
    STR_EMPTY,                         // 178
    STR_EMPTY,                         // 179
    STR_EMPTY,                         // 180
    STR_EMPTY,                         // 181
    STR_LAUNCHED_LIFT_HILL,            // 182
    STR_LARGE_HALF_LOOP_LEFT,          // 183
    STR_LARGE_HALF_LOOP_RIGHT,         // 184
    STR_LARGE_HALF_LOOP_LEFT,          // 185
    STR_LARGE_HALF_LOOP_RIGHT,         // 186
    STR_IN_LINE_TWIST_LEFT,            // 187
    STR_IN_LINE_TWIST_RIGHT,           // 188
    STR_IN_LINE_TWIST_LEFT,            // 189
    STR_IN_LINE_TWIST_RIGHT,           // 190
    STR_HALF_LOOP,                     // 191
    STR_HALF_LOOP,                     // 192
    STR_HALF_CORKSCREW_LEFT,           // 193
    STR_HALF_CORKSCREW_RIGHT,          // 194
    STR_HALF_CORKSCREW_LEFT,           // 195
    STR_HALF_CORKSCREW_RIGHT,          // 196
    STR_UPPER_TRANSFER,                // 197
    STR_LOWER_TRANSFER,                // 198
    STR_HEARTLINE_ROLL_LEFT,           // 199
    STR_HEARTLINE_ROLL_RIGHT,          // 200
    STR_GOLF_HOLE_A,                   // 201
    STR_GOLF_HOLE_B,                   // 202
    STR_GOLF_HOLE_C,                   // 203
    STR_GOLF_HOLE_D,                   // 204
    STR_GOLF_HOLE_E,                   // 205
    STR_QUARTER_LOOP,                  // 206
    STR_QUARTER_LOOP,                  // 207
    STR_QUARTER_LOOP,                  // 208
    STR_CURVED_LIFT_HILL_LEFT,         // 209
    STR_CURVED_LIFT_HILL_RIGHT,        // 210
    STR_REVERSER_LEFT,                 // 211
    STR_REVERSER_RIGHT,                // 212
    STR_TOP_SECTION,                   // 213
    STR_VERTICAL_TRACK,                // 214
    STR_SLOPE_TO_LEVEL,                // 215
    STR_BLOCK_BRAKES,                  // 216
    STR_EMPTY,                         // 217
    STR_EMPTY,                         // 218
    STR_EMPTY,                         // 219
    STR_EMPTY,                         // 220
    STR_EMPTY,                         // 221
    STR_EMPTY,                         // 222
    STR_EMPTY,                         // 223
    STR_EMPTY,                         // 224
    STR_EMPTY,                         // 225
    STR_EMPTY,                         // 226
    STR_EMPTY,                         // 227
    STR_EMPTY,                         // 228
    STR_EMPTY,                         // 229
    STR_EMPTY,                         // 230
    STR_EMPTY,                         // 231
    STR_EMPTY,                         // 232
    STR_EMPTY,                         // 233
    STR_EMPTY,                         // 234
    STR_EMPTY,                         // 235
    STR_EMPTY,                         // 236
    STR_EMPTY,                         // 237
    STR_EMPTY,                         // 238
    STR_EMPTY,                         // 239
    STR_EMPTY,                         // 240
    STR_EMPTY,                         // 241
    STR_EMPTY,                         // 242
    STR_EMPTY,                         // 243
    STR_EMPTY,                         // 244
    STR_EMPTY,                         // 245
    STR_EMPTY,                         // 246
    STR_EMPTY,                         // 247
    STR_EMPTY,                         // 248
    STR_EMPTY,                         // 249
    STR_EMPTY,                         // 250
    STR_EMPTY,                         // 251
    STR_EMPTY,                         // 252
    STR_QUARTER_LOOP,                  // 253
    STR_QUARTER_LOOP,                  // 254
    STR_QUARTER_LOOP,                  // 255
    STR_SPINNING_CONTROL_TOGGLE_TRACK, // 256
    STR_EMPTY,                         // 257
    STR_EMPTY,                         // 258
    STR_EMPTY,                         // 259
    STR_EMPTY,                         // 260
    STR_EMPTY,                         // 261
    STR_EMPTY,                         // 262
    STR_EMPTY,                         // 263
    STR_EMPTY,                         // 264
    STR_EMPTY,                         // 265
    STR_EMPTY,                         // 266
    STR_LARGE_HALF_CORKSCREW_LEFT,     // TrackElemType::LeftCorkscrewUp
    STR_LARGE_HALF_CORKSCREW_RIGHT,    // TrackElemType::RightCorkscrewUp
    STR_LARGE_HALF_CORKSCREW_LEFT,     // TrackElemType::LeftCorkscrewDown
    STR_LARGE_HALF_CORKSCREW_RIGHT,    // TrackElemType::RightCorkscrewDown
    STR_MEDIUM_HALF_LOOP_LEFT,         // TrackElemType::LeftMediumHalfLoopUp
    STR_MEDIUM_HALF_LOOP_RIGHT,        // TrackElemType::RightMediumHalfLoopUp
    STR_MEDIUM_HALF_LOOP_LEFT,         // TrackElemType::RightMediumHalfLoopDown
    STR_MEDIUM_HALF_LOOP_RIGHT,        // TrackElemType::LeftMediumHalfLoopDown
    STR_ZERO_G_ROLL_LEFT,              // TrackElemType::LeftZeroGRollUp
    STR_ZERO_G_ROLL_RIGHT,             // TrackElemType::RightZeroGRollUp
    STR_ZERO_G_ROLL_LEFT,              // TrackElemType::RightZeroGRollDown
    STR_ZERO_G_ROLL_RIGHT,             // TrackElemType::LeftZeroGRollDown
    STR_LARGE_ZERO_G_ROLL_LEFT,        // TrackElemType::LeftLargeZeroGRollUp
    STR_LARGE_ZERO_G_ROLL_RIGHT,       // TrackElemType::RightLargeZeroGRollUp
    STR_LARGE_ZERO_G_ROLL_LEFT,        // TrackElemType::RightLargeZeroGRollDown
    STR_LARGE_ZERO_G_ROLL_RIGHT,       // TrackElemType::LeftLargeZeroGRollDown
    STR_LARGE_HALF_LOOP_LEFT,          // TrackElemType::LeftFlyerLargeHalfLoopUninvertedUp
    STR_LARGE_HALF_LOOP_RIGHT,         // TrackElemType::RightFlyerLargeHalfLoopUninvertedUp
    STR_LARGE_HALF_LOOP_RIGHT,         // TrackElemType::RightFlyerLargeHalfLoopInvertedDown
    STR_LARGE_HALF_LOOP_LEFT,          // TrackElemType::LeftFlyerLargeHalfLoopInvertedDown
    STR_LARGE_HALF_LOOP_LEFT,          // TrackElemType::LeftFlyerLargeHalfLoopInvertedUp
    STR_LARGE_HALF_LOOP_RIGHT,         // TrackElemType::RightFlyerLargeHalfLoopInvertedUp
    STR_LARGE_HALF_LOOP_RIGHT,         // TrackElemType::RightFlyerLargeHalfLoopUninvertedDown
    STR_LARGE_HALF_LOOP_LEFT,          // TrackElemType::LeftFlyerLargeHalfLoopUninvertedDown
    STR_HALF_LOOP,                     // TrackElemType::FlyerHalfLoopInvertedUp
    STR_HALF_LOOP,                     // TrackElemType::FlyerHalfLoopUninvertedDown
};

namespace OpenRCT2
{
    namespace TrackMetaData
    {
        static constexpr auto BuildDescriptorTable()
        {
            std::array<TrackElementDescriptor, TrackElemType::Count> res{};

            for (int i = 0; i < TrackElemType::Count; i++)
            {
                TrackElementDescriptor& desc = res[i];
                desc.Description = RideConfigurationStringIds[i];
                desc.AlternativeType = AlternativeTrackTypes[i];
                desc.Block = const_cast<PreviewTrack*>(TrackBlocks[i]);
                desc.Coordinates = _trackCoordinates[i];
                desc.CurveChain = gTrackCurveChain[i];
                desc.Flags = TrackFlags[i];
                desc.HeightMarkerPositions = TrackHeightMarkerPositions[i];
                desc.MirrorElement = TrackElementMirrorMap[i];
                desc.PieceLength = TrackPieceLengths[i];
                desc.PriceModifier = TrackPricing[i];
                desc.Definition = TrackDefinitions[i];
                desc.SpinFunction = TrackTypeToSpinFunction[i];
                desc.VerticalFactor = GetVerticalFunction(i);
                desc.LateralFactor = GetLateralFunction(i);

                for (uint32_t j = 0; j < MaxSequencesPerPiece; j++)
                {
                    desc.SequenceElementAllowedWallEdges[j] = TrackSequenceElementAllowedWallEdges[i][j];
                    desc.SequenceProperties[j] = TrackSequenceProperties[i][j];
                }
            }

            return res;
        }

        static auto _trackElementDescriptors = BuildDescriptorTable();

        const TrackElementDescriptor& GetTrackElementDescriptor(const uint32_t type)
        {
            if (type >= _trackElementDescriptors.size())
                return _trackElementDescriptors[0];
            return _trackElementDescriptors[type];
        }

    } // namespace TrackMetaData
} // namespace OpenRCT2

const PreviewTrack* TrackElementDescriptor::GetBlockForSequence(uint8_t sequenceIndex) const
{
    const auto* trackBlock = Block;
    if (trackBlock == nullptr)
        return nullptr;

    // The sequence index may be higher than the amount of sequences actually present.
    // We don’t know the amount of sequences present in the block upfront, but there is an end marker consisting of all 255s.
    for (auto i = 0; i < sequenceIndex; i++)
    {
        trackBlock++;

        if (trackBlock == nullptr || trackBlock->index == 255)
            return nullptr;
    }

    return trackBlock;
}
