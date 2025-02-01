/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TrackData.h"

#include "../core/EnumUtils.hpp"
#include "../localisation/StringIds.h"
#include "Track.h"
#include "TrackPaint.h"

#include <cstdint>
#include <iterator>

using namespace OpenRCT2;

namespace OpenRCT2::TrackMetaData
{
    struct SequenceData
    {
        uint8_t numSequences;
        std::array<SequenceDescriptor, kMaxSequencesPerPiece> sequences;
    };

    // clang-format off
    static constexpr TrackCoordinates kTrackCoordinates[] = {
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
            { 0, 2, 0, -216, 32, -32 },  // TrackElemType::LeftMediumHalfLoopDown
            { 0, 2, 0, -216, 32, 32 },   // TrackElemType::RightMediumHalfLoopDown
            { 0, 0, 0, 56, -64, 0 },     // TrackElemType::LeftZeroGRollUp
            { 0, 0, 0, 56, -64, 0 },     // TrackElemType::RightZeroGRollUp
            { 0, 0, 0, -56,-64, 0 },     // TrackElemType::LeftZeroGRollDown
            { 0, 0, 0, -56,-64, 0 },     // TrackElemType::RightZeroGRollDown
            { 0, 0, 0, 152, -96, 0 },    // TrackElemType::LeftLargeZeroGRollUp
            { 0, 0, 0, 152, -96, 0 },    // TrackElemType::RightLargeZeroGRollUp
            { 0, 0, 0, -152,-96, 0 },    // TrackElemType::LeftLargeZeroGRollDown
            { 0, 0, 0, -152,-96, 0 },    // TrackElemType::RightLargeZeroGRollDown
            { 0, 2, 0, 248, -64, -32 },  // TrackElemType::LeftFlyerLargeHalfLoopUninvertedUp
            { 0, 2, 0, 248, -64, 32 },   // TrackElemType::RightFlyerLargeHalfLoopUninvertedUp
            { 0, 2, 0, -248, 64, -32 },  // TrackElemType::LeftFlyerLargeHalfLoopInvertedDown
            { 0, 2, 0, -248, 64, 32 },   // TrackElemType::RightFlyerLargeHalfLoopInvertedDown
            { 0, 2, 0, 280, -64, -32 },  // TrackElemType::LeftFlyerLargeHalfLoopInvertedUp
            { 0, 2, 0, 280, -64, 32 },   // TrackElemType::RightFlyerLargeHalfLoopInvertedUp
            { 0, 2, 0, -280, 64, -32 },  // TrackElemType::LeftFlyerLargeHalfLoopUninvertedDown
            { 0, 2, 0, -280, 64, 32 },   // TrackElemType::RightFlyerLargeHalfLoopUninvertedDown
            { 0, 2, 0, 152, -32, 0 },    // TrackElemType::FlyerHalfLoopInvertedUp
            { 0, 2, 0, -152, 32, 0 },    // TrackElemType::FlyerHalfLoopUninvertedDown
            { 0, 7, 0, 48, -64, -32 }, // TrackElemType::LeftEighthToDiagUp25          
            { 0, 4, 0, 48, -64, 32 },  // TrackElemType::RightEighthToDiagUp25        
            { 0, 7, 0,-48, -64, -32 }, // TrackElemType::LeftEighthToDiagDown25       
            { 0, 4, 0,-48, -64, 32 },  // TrackElemType::RightEighthToDiagDown25      
            { 4, 0, 0, 48, -64, 32 },  // TrackElemType::LeftEighthToOrthogonalUp25   
            { 4, 1, 0, 48, -32, 64 },  // TrackElemType::RightEighthToOrthogonalUp25  
            { 4, 0, 0,-48, -64, 32 },  // TrackElemType::LeftEighthToOrthogonalDown25 
            { 4, 1, 0,-48, -32, 64 },  // TrackElemType::RightEighthToOrthogonalDown25 
        { 4, 4, 0, 16, -32, 32 },  // TrackElemType::DiagUp25ToLeftBankedUp25
        { 4, 4, 0, 16, -32, 32 },  // TrackElemType::DiagUp25ToRightBankedUp25
        { 4, 4, 0, 16, -32, 32 },  // TrackElemType::DiagLeftBankedUp25ToUp25
        { 4, 4, 0, 16, -32, 32 },  // TrackElemType::DiagRightBankedUp25ToUp25
        { 4, 4, 16, 0, -32, 32 },  // TrackElemType::DiagDown25ToLeftBankedDown25
        { 4, 4, 16, 0, -32, 32 },  // TrackElemType::DiagDown25ToRightBankedDown25
        { 4, 4, 16, 0, -32, 32 },  // TrackElemType::DiagLeftBankedDown25ToDown25
        { 4, 4, 16, 0, -32, 32 },  // TrackElemType::DiagRightBankedDown25ToDown25
        { 4, 4, 0, 8, -32, 32 },   // TrackElemType::DiagLeftBankedFlatToLeftBankedUp25
        { 4, 4, 0, 8, -32, 32 },   // TrackElemType::DiagRightBankedFlatToRightBankedUp25
        { 4, 4, 0, 8, -32, 32 },   // TrackElemType::DiagLeftBankedUp25ToLeftBankedFlat
        { 4, 4, 0, 8, -32, 32 },   // TrackElemType::DiagRightBankedUp25ToRightBankedFlat
        { 4, 4, 8, 0, -32, 32 },   // TrackElemType::DiagLeftBankedFlatToLeftBankedDown25
        { 4, 4, 8, 0, -32, 32 },   // TrackElemType::DiagRightBankedFlatToRightBankedDown25
        { 4, 4, 8, 0, -32, 32 },   // TrackElemType::DiagLeftBankedDown25ToLeftBankedFlat
        { 4, 4, 8, 0, -32, 32 },   // TrackElemType::DiagRightBankedDown25ToRightBankedFlat
        { 4, 4, 0, 8, -32, 32 },   // TrackElemType::DiagFlatToLeftBankedUp25
        { 4, 4, 0, 8, -32, 32 },   // TrackElemType::DiagFlatToRightBankedUp25
        { 4, 4, 0, 8, -32, 32 },   // TrackElemType::DiagLeftBankedUp25ToFlat
        { 4, 4, 0, 8, -32, 32 },   // TrackElemType::DiagRightBankedUp25ToFlat
        { 4, 4, 8, 0, -32, 32 },   // TrackElemType::DiagFlatToLeftBankedDown25
        { 4, 4, 8, 0, -32, 32 },   // TrackElemType::DiagFlatToRightBankedDown25
        { 4, 4, 8, 0, -32, 32 },   // TrackElemType::DiagLeftBankedDown25ToFlat
        { 4, 4, 8, 0, -32, 32 },   // TrackElemType::DiagRightBankedDown25ToFlat
        { 4, 4, 0, 16, -32, 32 },  // TrackElemType::DiagUp25LeftBanked
        { 4, 4, 0, 16, -32, 32 },  // TrackElemType::DiagUp25RightBanked 
        { 4, 4, 16, 0, -32, 32 },  // TrackElemType::DiagDown25LeftBanked
        { 4, 4, 16, 0, -32, 32 },  // TrackElemType::DiagDown25RightBanked
        { 0, 7, 0, 48, -64, -32 }, // TrackElemType::LeftEighthBankToDiagUp25
        { 0, 4, 0, 48, -64, 32 },  // TrackElemType::RightEighthBankToDiagUp25
        { 0, 7, 0,-48, -64, -32 }, // TrackElemType::LeftEighthBankToDiagDown25
        { 0, 4, 0,-48, -64, 32 },  // TrackElemType::RightEighthBankToDiagDown25
        { 4, 0, 0, 48, -64, 32 },  // TrackElemType::LeftEighthBankToOrthogonalUp25
        { 4, 1, 0, 48, -32, 64 },  // TrackElemType::RightEighthBankToOrthogonalUp25 
        { 4, 0, 0,-48, -64, 32 },  // TrackElemType::LeftEighthBankToOrthogonalDown25
        { 4, 1, 0,-48, -32, 64 },  // TrackElemType::RightEighthBankToOrthogonalDown25     
        { 4, 4, 0, 0, -32, 32 },   // TrackElemType::DiagBrakes
        { 4, 4, 0, 0, -32, 32 },   // TrackElemType::DiagBlockBrakes
        { 0, 0, 16, 0, 0, 0 },     // TrackElemType::Down25Brakes
        { 4, 4, 0, 0, -32, 32 },   // TrackElemType::DiagBooster
        { 4, 4, 0, 88, -96, 96 },   // TrackElemType::DiagFlatToUp60LongBase,
        { 4, 4, 0, 88, -96, 96 },   // TrackElemType::DiagUp60ToFlatLongBase,
        { 4, 4, 88, 0, -96, 96 },   // TrackElemType::DiagFlatToDown60LongBase,
        { 4, 4, 88, 0, -96, 96 },   // TrackElemType::DiagDown60ToFlatLongBase,
        { 4, 0, 0,152, -96, 32 },   // TrackElemType::LeftEighthDiveLoopUpToOrthogonal
        { 4, 1, 0,152, -32, 96 },   // TrackElemType::RightEighthDiveLoopUpToOrthogonal
        { 0, 7,152, 0, -96, -32 },   // TrackElemType::LeftEighthDiveLoopDownToDiag 
        { 0, 4,152, 0, -96, 32 },   // TrackElemType::RightEighthDiveLoopDownToDiag
    };
    static_assert(std::size(kTrackCoordinates) == EnumValue(TrackElemType::Count));
    
    static constexpr uint8_t kTrackPieceLengths[] = {
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
        100,    // TrackElemType::LeftLargeHalfLoopDown
        100,    // TrackElemType::RightLargeHalfLoopDown
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
        80,     // TrackElemType::LeftMediumHalfLoopDown
        80,     // TrackElemType::RightMediumHalfLoopDown
        96,     // TrackElemType::LeftZeroGRollUp
        96,     // TrackElemType::RightZeroGRollUp
        96,     // TrackElemType::LeftZeroGRollDown
        96,     // TrackElemType::RightZeroGRollDown
        165,    // TrackElemType::LeftLargeZeroGRollUp
        165,    // TrackElemType::RightLargeZeroGRollUp
        165,    // TrackElemType::LeftLargeZeroGRollDown
        165,    // TrackElemType::RightLargeZeroGRollDown
        100,    // TrackElemType::LeftFlyerLargeHalfLoopUninvertedUp
        100,    // TrackElemType::RightFlyerLargeHalfLoopUninvertedUp
        100,    // TrackElemType::LeftFlyerLargeHalfLoopInvertedDown
        100,    // TrackElemType::RightFlyerLargeHalfLoopInvertedDown
        100,    // TrackElemType::LeftFlyerLargeHalfLoopInvertedUp
        100,    // TrackElemType::RightFlyerLargeHalfLoopInvertedUp
        100,    // TrackElemType::LeftFlyerLargeHalfLoopUninvertedDown
        100,    // TrackElemType::RightFlyerLargeHalfLoopUninvertedDown
        64,     // TrackElemType::FlyerHalfLoopInvertedUp
        64,     // TrackElemType::FlyerHalfLoopUninvertedDown
        92, // TrackElemType::LeftEighthToDiagUp25          
        92, // TrackElemType::RightEighthToDiagUp25        
        92, // TrackElemType::LeftEighthToDiagDown25       
        92, // TrackElemType::RightEighthToDiagDown25      
        92, // TrackElemType::LeftEighthToOrthogonalUp25   
        92, // TrackElemType::RightEighthToOrthogonalUp25  
        92, // TrackElemType::LeftEighthToOrthogonalDown25 
        92, // TrackElemType::RightEighthToOrthogonalDown25 
        45, // TrackElemType::DiagUp25ToLeftBankedUp25
        45, // TrackElemType::DiagUp25ToRightBankedUp25
        45, // TrackElemType::DiagLeftBankedUp25ToUp25
        45, // TrackElemType::DiagRightBankedUp25ToUp25
        45, // TrackElemType::DiagDown25ToLeftBankedDown25
        45, // TrackElemType::DiagDown25ToRightBankedDown25
        45, // TrackElemType::DiagLeftBankedDown25ToDown25
        45, // TrackElemType::DiagRightBankedDown25ToDown25
        45, // TrackElemType::DiagLeftBankedFlatToLeftBankedUp25
        45, // TrackElemType::DiagRightBankedFlatToRightBankedUp25
        45, // TrackElemType::DiagLeftBankedUp25ToLeftBankedFlat
        45, // TrackElemType::DiagRightBankedUp25ToRightBankedFlat
        45, // TrackElemType::DiagLeftBankedFlatToLeftBankedDown25
        45, // TrackElemType::DiagRightBankedFlatToRightBankedDown25
        45, // TrackElemType::DiagLeftBankedDown25ToLeftBankedFlat
        45, // TrackElemType::DiagRightBankedDown25ToRightBankedFlat
        45, // TrackElemType::DiagFlatToLeftBankedUp25
        45, // TrackElemType::DiagFlatToRightBankedUp25
        45, // TrackElemType::DiagLeftBankedUp25ToFlat
        45, // TrackElemType::DiagRightBankedUp25ToFlat
        45, // TrackElemType::DiagFlatToLeftBankedDown25
        45, // TrackElemType::DiagFlatToRightBankedDown25
        45, // TrackElemType::DiagLeftBankedDown25ToFlat
        45, // TrackElemType::DiagRightBankedDown25ToFlat
        45, // TrackElemType::DiagUp25LeftBanked
        45, // TrackElemType::DiagUp25RightBanked 
        45, // TrackElemType::DiagDown25LeftBanked
        45, // TrackElemType::DiagDown25RightBanked
        92, // TrackElemType::LeftEighthBankToDiagUp25
        92, // TrackElemType::RightEighthBankToDiagUp25
        92, // TrackElemType::LeftEighthBankToDiagDown25
        92, // TrackElemType::RightEighthBankToDiagDown25
        92, // TrackElemType::LeftEighthBankToOrthogonalUp25
        92, // TrackElemType::RightEighthBankToOrthogonalUp25 
        92, // TrackElemType::LeftEighthBankToOrthogonalDown25
        92, // TrackElemType::RightEighthBankToOrthogonalDown25
        45, // TrackElemType::DiagBrakes
        45, // TrackElemType::DiagBlockBrakes
        33, // TrackElemType::Down25Brakes
        45, // TrackElemType::DiagBooster
        165, // TrackElemType::DiagFlatToUp60LongBase,
        165, // TrackElemType::DiagUp60ToFlatLongBase,
        165, // TrackElemType::DiagFlatToDown60LongBase,
        165, // TrackElemType::DiagDown60ToFlatLongBase,
        165, // TrackElemType::LeftEighthDiveLoopUpToOrthogonal    
        165, // TrackElemType::RightEighthDiveLoopUpToOrthogonal   
        165, // TrackElemType::LeftEighthDiveLoopDownToDiag  
        165, // TrackElemType::RightEighthDiveLoopDownToDiag      
    };

    static_assert(std::size(kTrackPieceLengths) == EnumValue(TrackElemType::Count));
    
    // rct2: 0x00998C95
    static constexpr TrackCurveChain kTrackCurveChain[] = {
        { TrackCurve::None, TrackCurve::None },
        { TrackElemType::EndStation, TrackElemType::EndStation },
        { TrackElemType::EndStation, TrackElemType::EndStation },
        { TrackElemType::EndStation, TrackElemType::EndStation },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::Left, TrackCurve::Left },
        { TrackCurve::Right, TrackCurve::Right },
        { TrackCurve::Left, TrackCurve::None },
        { TrackCurve::Right, TrackCurve::None },
        { TrackCurve::None, TrackCurve::Left },
        { TrackCurve::None, TrackCurve::Right },
        { TrackCurve::Left, TrackCurve::Left },
        { TrackCurve::Right, TrackCurve::Right },
        { TrackCurve::None, TrackCurve::Left },
        { TrackCurve::None, TrackCurve::Right },
        { TrackCurve::Left, TrackCurve::None },
        { TrackCurve::Right, TrackCurve::None },
        { TrackCurve::None, TrackCurve::Left },
        { TrackCurve::None, TrackCurve::Right },
        { TrackCurve::Left, TrackCurve::None },
        { TrackCurve::Right, TrackCurve::None },
        { TrackCurve::Left, TrackCurve::Left },
        { TrackCurve::Right, TrackCurve::Right },
        { TrackCurve::Left, TrackCurve::Left },
        { TrackCurve::Right, TrackCurve::Right },
        { TrackCurve::Left, TrackCurve::Left },
        { TrackCurve::Right, TrackCurve::Right },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::LeftSmall, TrackCurve::LeftSmall },
        { TrackCurve::RightSmall, TrackCurve::RightSmall },
        { TrackCurve::LeftSmall, TrackCurve::LeftSmall },
        { TrackCurve::RightSmall, TrackCurve::RightSmall },
        { TrackCurve::LeftSmall, TrackCurve::LeftSmall },
        { TrackCurve::RightSmall, TrackCurve::RightSmall },
        { TrackCurve::LeftSmall, TrackCurve::LeftSmall },
        { TrackCurve::RightSmall, TrackCurve::RightSmall },
        { TrackCurve::LeftVerySmall, TrackCurve::LeftVerySmall },
        { TrackCurve::RightVerySmall, TrackCurve::RightVerySmall },
        { TrackElemType::LeftTwistUpToDown, TrackCurve::None },
        { TrackElemType::RightTwistUpToDown, TrackCurve::None },
        { TrackCurve::None, TrackElemType::LeftTwistDownToUp },
        { TrackCurve::None, TrackElemType::RightTwistDownToUp },
        { TrackElemType::HalfLoopDown, TrackCurve::None },
        { TrackCurve::None, TrackElemType::HalfLoopUp },
        { TrackElemType::RightCorkscrewDown, TrackCurve::None },
        { TrackElemType::LeftCorkscrewDown, TrackCurve::None },
        { TrackCurve::None, TrackElemType::RightCorkscrewUp },
        { TrackCurve::None, TrackElemType::LeftCorkscrewUp },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackElemType::TowerSection, TrackCurve::None },
        { TrackElemType::TowerSection, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::Left, TrackCurve::Left },
        { TrackCurve::Right, TrackCurve::Right },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::LeftSmall, TrackCurve::LeftSmall },
        { TrackCurve::RightSmall, TrackCurve::RightSmall },
        { TrackElemType::LeftHalfBankedHelixUpSmall, TrackElemType::LeftHalfBankedHelixUpSmall },
        { TrackElemType::RightHalfBankedHelixUpSmall, TrackElemType::RightHalfBankedHelixUpSmall },
        { TrackElemType::LeftHalfBankedHelixDownSmall, TrackElemType::LeftHalfBankedHelixDownSmall },
        { TrackElemType::RightHalfBankedHelixDownSmall, TrackElemType::RightHalfBankedHelixDownSmall },
        { TrackElemType::LeftHalfBankedHelixUpLarge, TrackElemType::LeftHalfBankedHelixUpLarge },
        { TrackElemType::RightHalfBankedHelixUpLarge, TrackElemType::RightHalfBankedHelixUpLarge },
        { TrackElemType::LeftHalfBankedHelixDownLarge, TrackElemType::LeftHalfBankedHelixDownLarge },
        { TrackElemType::RightHalfBankedHelixDownLarge, TrackElemType::RightHalfBankedHelixDownLarge },
        { TrackCurve::LeftSmall, TrackCurve::LeftSmall },
        { TrackCurve::RightSmall, TrackCurve::RightSmall },
        { TrackCurve::LeftSmall, TrackCurve::LeftSmall },
        { TrackCurve::RightSmall, TrackCurve::RightSmall },
        { TrackElemType::Brakes, TrackElemType::Brakes },
        { TrackElemType::Booster, TrackElemType::Booster },
        { TrackCurve::None, TrackCurve::None },
        { TrackElemType::LeftQuarterBankedHelixLargeUp, TrackElemType::LeftQuarterBankedHelixLargeUp },
        { TrackElemType::RightQuarterBankedHelixLargeUp, TrackElemType::RightQuarterBankedHelixLargeUp },
        { TrackElemType::LeftQuarterBankedHelixLargeDown, TrackElemType::LeftQuarterBankedHelixLargeDown },
        { TrackElemType::RightQuarterBankedHelixLargeDown, TrackElemType::RightQuarterBankedHelixLargeDown },
        { TrackElemType::LeftQuarterHelixLargeUp, TrackElemType::LeftQuarterHelixLargeUp },
        { TrackElemType::RightQuarterHelixLargeUp, TrackElemType::RightQuarterHelixLargeUp },
        { TrackElemType::LeftQuarterHelixLargeDown, TrackElemType::LeftQuarterHelixLargeDown },
        { TrackElemType::RightQuarterHelixLargeDown, TrackElemType::RightQuarterHelixLargeDown },
        { TrackCurve::Left, TrackCurve::Left },
        { TrackCurve::Right, TrackCurve::Right },
        { TrackElemType::Waterfall, TrackElemType::Waterfall },
        { TrackElemType::Rapids, TrackElemType::Rapids },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::Left, TrackCurve::Left },
        { TrackCurve::Right, TrackCurve::Right },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackElemType::ReverseFreefallVertical, TrackCurve::None },
        { TrackElemType::ReverseFreefallVertical, TrackElemType::ReverseFreefallVertical },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::LeftLarge, TrackCurve::LeftLarge },
        { TrackCurve::RightLarge, TrackCurve::RightLarge },
        { TrackCurve::LeftLarge, TrackCurve::LeftLarge },
        { TrackCurve::RightLarge, TrackCurve::RightLarge },
        { TrackCurve::LeftLarge, TrackCurve::LeftLarge },
        { TrackCurve::RightLarge, TrackCurve::RightLarge },
        { TrackCurve::LeftLarge, TrackCurve::LeftLarge },
        { TrackCurve::RightLarge, TrackCurve::RightLarge },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::LeftLarge, TrackCurve::None },
        { TrackCurve::RightLarge, TrackCurve::None },
        { TrackCurve::None, TrackCurve::LeftLarge },
        { TrackCurve::None, TrackCurve::RightLarge },
        { TrackCurve::None, TrackCurve::LeftLarge },
        { TrackCurve::None, TrackCurve::RightLarge },
        { TrackCurve::LeftLarge, TrackCurve::None },
        { TrackCurve::RightLarge, TrackCurve::None },
        { TrackCurve::None, TrackCurve::LeftLarge },
        { TrackCurve::None, TrackCurve::RightLarge },
        { TrackCurve::LeftLarge, TrackCurve::None },
        { TrackCurve::RightLarge, TrackCurve::None },
        { TrackCurve::LeftLarge, TrackCurve::LeftLarge },
        { TrackCurve::RightLarge, TrackCurve::RightLarge },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackElemType::LeftBarrelRollDownToUp, TrackCurve::None },
        { TrackElemType::RightBarrelRollDownToUp, TrackCurve::None },
        { TrackCurve::None, TrackElemType::LeftBarrelRollUpToDown },
        { TrackCurve::None, TrackElemType::RightBarrelRollUpToDown },
        { TrackCurve::LeftSmall, TrackCurve::LeftSmall },
        { TrackCurve::RightSmall, TrackCurve::RightSmall },
        { TrackCurve::LeftSmall, TrackCurve::LeftSmall },
        { TrackCurve::RightSmall, TrackCurve::RightSmall },
        { TrackElemType::PoweredLift, TrackElemType::PoweredLift },
        { TrackElemType::RightLargeHalfLoopDown, TrackCurve::None },
        { TrackElemType::LeftLargeHalfLoopDown, TrackCurve::None },
        { TrackCurve::None, TrackElemType::RightLargeHalfLoopUp },
        { TrackCurve::None, TrackElemType::LeftLargeHalfLoopUp },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackElemType::InvertedFlatToDown90QuarterLoop, TrackCurve::None },
        { TrackCurve::None, TrackElemType::Up90ToInvertedFlatQuarterLoop },
        { TrackElemType::LeftCurvedLiftHill, TrackElemType::LeftCurvedLiftHill },
        { TrackElemType::RightCurvedLiftHill, TrackElemType::RightCurvedLiftHill },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackElemType::AirThrustVerticalDown, TrackElemType::ReverseFreefallVertical },
        { TrackElemType::AirThrustVerticalDown, TrackElemType::AirThrustVerticalDown },
        { TrackCurve::None, TrackElemType::AirThrustVerticalDown },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::LeftSmall, TrackCurve::LeftSmall },
        { TrackCurve::RightSmall, TrackCurve::RightSmall },
        { TrackCurve::LeftSmall, TrackCurve::LeftSmall },
        { TrackCurve::RightSmall, TrackCurve::RightSmall },
        { TrackCurve::Left, TrackCurve::Left },
        { TrackCurve::Right, TrackCurve::Right },
        { TrackCurve::Left, TrackCurve::Left },
        { TrackCurve::Right, TrackCurve::Right },
        { TrackCurve::Left, TrackCurve::None },
        { TrackCurve::Right, TrackCurve::None },
        { TrackCurve::None, TrackCurve::Left },
        { TrackCurve::None, TrackCurve::Right },
        { TrackCurve::Left, TrackCurve::None },
        { TrackCurve::Right, TrackCurve::None },
        { TrackCurve::None, TrackCurve::Left },
        { TrackCurve::None, TrackCurve::Right },
        { TrackCurve::Left, TrackCurve::Left },
        { TrackCurve::Right, TrackCurve::Right },
        { TrackCurve::Left, TrackCurve::Left },
        { TrackCurve::Right, TrackCurve::Right },
        { TrackCurve::Left, TrackCurve::Left },
        { TrackCurve::Right, TrackCurve::Right },
        { TrackCurve::Left, TrackCurve::Left },
        { TrackCurve::Right, TrackCurve::Right },
        { TrackCurve::Left, TrackCurve::None },
        { TrackCurve::Right, TrackCurve::None },
        { TrackCurve::None, TrackCurve::Left },
        { TrackCurve::None, TrackCurve::Right },
        { TrackCurve::Left, TrackCurve::None },
        { TrackCurve::Right, TrackCurve::None },
        { TrackCurve::None, TrackCurve::Left },
        { TrackCurve::None, TrackCurve::Right },
        { TrackCurve::LeftSmall, TrackCurve::LeftSmall },
        { TrackCurve::RightSmall, TrackCurve::RightSmall },
        { TrackCurve::LeftSmall, TrackCurve::LeftSmall },
        { TrackCurve::RightSmall, TrackCurve::RightSmall },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackCurve::None, TrackCurve::None },
        { TrackElemType::None, TrackElemType::None },                       // TrackElemType::FlatTrack1x4A
        { TrackCurve::None, TrackCurve::None }, // TrackElemType::FlatTrack2x2
        { TrackCurve::None, TrackCurve::None }, // TrackElemType::FlatTrack4x4
        { TrackElemType::None, TrackElemType::None },                       // TrackElemType::FlatTrack2x4
        { TrackElemType::None, TrackElemType::None },                       // TrackElemType::FlatTrack1x5
        { TrackElemType::None, TrackElemType::None },                       // TrackElemType::FlatTrack1x1A
        { TrackElemType::None, TrackElemType::None },                       // TrackElemType::FlatTrack1x4B
        { TrackElemType::None, TrackElemType::None },                       // TrackElemType::FlatTrack1x1B
        { TrackElemType::None, TrackElemType::None },                       // TrackElemType::FlatTrack1x4C
        { TrackElemType::None, TrackElemType::None },                       // TrackElemType::FlatTrack3x3
        { TrackElemType::RightLargeCorkscrewDown, TrackCurve::None }, // TrackElemType::LeftLargeCorkscrewUp
        { TrackElemType::LeftLargeCorkscrewDown, TrackCurve::None },  // TrackElemType::RightLargeCorkscrewUp
        { TrackCurve::None, TrackElemType::RightLargeCorkscrewUp },   // TrackElemType::LeftLargeCorkscrewDown
        { TrackCurve::None, TrackElemType::LeftLargeCorkscrewUp },    // TrackElemType::RightCorkscrewDown
        { TrackElemType::RightMediumHalfLoopDown, TrackCurve::None }, // TrackElemType::LeftMediumHalfLoopUp
        { TrackElemType::LeftMediumHalfLoopDown, TrackCurve::None },  // TrackElemType::RightMediumHalfLoopUp
        { TrackCurve::None, TrackElemType::RightMediumHalfLoopUp },   // TrackElemType::LeftMediumHalfLoopDown
        { TrackCurve::None, TrackElemType::LeftMediumHalfLoopUp },    // TrackElemType::RightMediumHalfLoopDown
        { TrackElemType::LeftZeroGRollDown, TrackCurve::None },       // TrackElemType::LeftZeroGRollUp
        { TrackElemType::RightZeroGRollDown, TrackCurve::None },      // TrackElemType::RightZeroGRollUp
        { TrackCurve::None, TrackElemType::LeftZeroGRollUp },         // TrackElemType::LeftZeroGRollDown
        { TrackCurve::None, TrackElemType::RightZeroGRollUp },        // TrackElemType::RightZeroGRollDown
        { TrackElemType::LeftLargeZeroGRollDown, TrackCurve::None },  // TrackElemType::LeftLargeZeroGRollUp
        { TrackElemType::RightLargeZeroGRollDown, TrackCurve::None }, // TrackElemType::RightLargeZeroGRollUp
        { TrackCurve::None, TrackElemType::LeftLargeZeroGRollUp },    // TrackElemType::LeftLargeZeroGRollDown
        { TrackCurve::None, TrackElemType::RightLargeZeroGRollUp },   // TrackElemType::RightLargeZeroGRollDown
        { TrackElemType::RightFlyerLargeHalfLoopInvertedDown, TrackCurve::None },   // LeftFlyerLargeHalfLoopUninvertedUp
        { TrackElemType::LeftFlyerLargeHalfLoopInvertedDown, TrackCurve::None },    // RightFlyerLargeHalfLoopUninvertedUp
        { TrackCurve::None, TrackElemType::RightFlyerLargeHalfLoopUninvertedUp },   // LeftFlyerLargeHalfLoopInvertedDown
        { TrackCurve::None, TrackElemType::LeftFlyerLargeHalfLoopUninvertedUp },    // RightFlyerLargeHalfLoopInvertedDown
        { TrackElemType::RightFlyerLargeHalfLoopUninvertedDown, TrackCurve::None },  // LeftFlyerLargeHalfLoopInvertedUp
        { TrackElemType::LeftFlyerLargeHalfLoopUninvertedDown, TrackCurve::None },  // RightFlyerLargeHalfLoopInvertedUp
        { TrackCurve::None, TrackElemType::RightFlyerLargeHalfLoopInvertedUp },     // LeftFlyerLargeHalfLoopUninvertedDown
        { TrackCurve::None, TrackElemType::LeftFlyerLargeHalfLoopInvertedUp },      // RightFlyerLargeHalfLoopUninvertedDown
        { TrackCurve::None, TrackCurve::None }, // FlyerHalfLoopInvertedUp
        { TrackCurve::None, TrackCurve::None }, // FlyerHalfLoopUninvertedUp
        { TrackCurve::LeftLarge, TrackCurve::LeftLarge },   // TrackElemType::LeftEighthToDiagUp25         
        { TrackCurve::RightLarge, TrackCurve::RightLarge }, // TrackElemType::RightEighthToDiagUp25        
        { TrackCurve::LeftLarge, TrackCurve::LeftLarge },   // TrackElemType::LeftEighthToDiagDown25       
        { TrackCurve::RightLarge, TrackCurve::RightLarge }, // TrackElemType::RightEighthToDiagDown25      
        { TrackCurve::LeftLarge, TrackCurve::LeftLarge },   // TrackElemType::LeftEighthToOrthogonalUp25   
        { TrackCurve::RightLarge, TrackCurve::RightLarge }, // TrackElemType::RightEighthToOrthogonalUp25  
        { TrackCurve::LeftLarge, TrackCurve::LeftLarge },   // TrackElemType::LeftEighthToOrthogonalDown25 
        { TrackCurve::RightLarge, TrackCurve::RightLarge }, // TrackElemType::RightEighthToOrthogonalDown25
        { TrackCurve::LeftLarge, TrackCurve::LeftLarge },   // TrackElemType::DiagUp25ToLeftBankedUp25
        { TrackCurve::RightLarge, TrackCurve::RightLarge }, // TrackElemType::DiagUp25ToRightBankedUp25
        { TrackCurve::None, TrackCurve::None },               // TrackElemType::DiagLeftBankedUp25ToUp25
        { TrackCurve::None, TrackCurve::None },               // TrackElemType::DiagRightBankedUp25ToUp25
        { TrackCurve::LeftLarge, TrackCurve::LeftLarge },   // TrackElemType::DiagDown25ToLeftBankedDown25
        { TrackCurve::RightLarge, TrackCurve::RightLarge }, // TrackElemType::DiagDown25ToRightBankedDown25
        { TrackCurve::None, TrackCurve::None },               // TrackElemType::DiagLeftBankedDown25ToDown25
        { TrackCurve::None, TrackCurve::None },               // TrackElemType::DiagRightBankedDown25ToDown25
        { TrackCurve::LeftLarge, TrackCurve::LeftLarge },   // TrackElemType::DiagLeftBankedFlatToLeftBankedUp25
        { TrackCurve::RightLarge, TrackCurve::RightLarge }, // TrackElemType::DiagRightBankedFlatToRightBankedUp25
        { TrackCurve::LeftLarge, TrackCurve::LeftLarge },   // TrackElemType::DiagLeftBankedUp25ToLeftBankedFlat
        { TrackCurve::RightLarge, TrackCurve::RightLarge }, // TrackElemType::DiagRightBankedUp25ToRightBankedFlat
        { TrackCurve::LeftLarge, TrackCurve::LeftLarge },   // TrackElemType::DiagLeftBankedFlatToLeftBankedDown25
        { TrackCurve::RightLarge, TrackCurve::RightLarge }, // TrackElemType::DiagRightBankedFlatToRightBankedDown25
        { TrackCurve::LeftLarge, TrackCurve::LeftLarge },   // TrackElemType::DiagLeftBankedDown25ToLeftBankedFlat
        { TrackCurve::RightLarge, TrackCurve::RightLarge }, // TrackElemType::DiagRightBankedDown25ToRightBankedFlat
        { TrackCurve::LeftLarge, TrackCurve::LeftLarge },   // TrackElemType::DiagFlatToLeftBankedUp25
        { TrackCurve::RightLarge, TrackCurve::RightLarge }, // TrackElemType::DiagFlatToRightBankedUp25
        { TrackCurve::None, TrackCurve::None },               // TrackElemType::DiagLeftBankedUp25ToFlat
        { TrackCurve::None, TrackCurve::None },               // TrackElemType::DiagRightBankedUp25ToFlat
        { TrackCurve::LeftLarge, TrackCurve::LeftLarge },   // TrackElemType::DiagFlatToLeftBankedDown25
        { TrackCurve::RightLarge, TrackCurve::RightLarge }, // TrackElemType::DiagFlatToRightBankedDown25
        { TrackCurve::None, TrackCurve::None },               // TrackElemType::DiagLeftBankedDown25ToFlat
        { TrackCurve::None, TrackCurve::None },               // TrackElemType::DiagRightBankedDown25ToFlat
        { TrackCurve::LeftLarge, TrackCurve::LeftLarge },   // TrackElemType::DiagUp25LeftBanked
        { TrackCurve::RightLarge, TrackCurve::RightLarge }, // TrackElemType::DiagUp25RightBanked 
        { TrackCurve::LeftLarge, TrackCurve::LeftLarge },   // TrackElemType::DiagDown25LeftBanked
        { TrackCurve::RightLarge, TrackCurve::RightLarge }, // TrackElemType::DiagDown25RightBanked
        { TrackCurve::LeftLarge, TrackCurve::LeftLarge },   // TrackElemType::LeftEighthBankToDiagUp25
        { TrackCurve::RightLarge, TrackCurve::RightLarge }, // TrackElemType::RightEighthBankToDiagUp25
        { TrackCurve::LeftLarge, TrackCurve::LeftLarge },   // TrackElemType::LeftEighthBankToDiagDown25
        { TrackCurve::RightLarge, TrackCurve::RightLarge }, // TrackElemType::RightEighthBankToDiagDown25
        { TrackCurve::LeftLarge, TrackCurve::LeftLarge },   // TrackElemType::LeftEighthBankToOrthogonalUp25
        { TrackCurve::RightLarge, TrackCurve::RightLarge }, // TrackElemType::RightEighthBankToOrthogonalUp25 
        { TrackCurve::LeftLarge, TrackCurve::LeftLarge },   // TrackElemType::LeftEighthBankToOrthogonalDown25
        { TrackCurve::RightLarge, TrackCurve::RightLarge }, // TrackElemType::RightEighthBankToOrthogonalDown25     
        { TrackElemType::DiagBrakes, TrackElemType::DiagBrakes }, // TrackElemType::DiagBrakes
        { TrackCurve::None, TrackCurve::None },             // TrackElemType::DiagBlockBrakes
        { TrackElemType::Down25Brakes, TrackElemType::Down25Brakes }, // TrackElemType::Down25Brakes
        { TrackElemType::DiagBooster, TrackElemType::DiagBooster }, // TrackElemType::DiagBooster
        { TrackCurve::None, TrackCurve::None },             // TrackElemType::DiagFlatToUp60LongBase,
        { TrackCurve::None, TrackCurve::None },             // TrackElemType::DiagUp60ToFlatLongBase,
        { TrackCurve::None, TrackCurve::None },             // TrackElemType::DiagFlatToDown60LongBase,
        { TrackCurve::None, TrackCurve::None },             // TrackElemType::DiagDown60ToFlatLongBase,
        { TrackElemType::RightEighthDiveLoopDownToDiag , TrackCurve::None },          // TrackElemType::LeftEighthDiveLoopUpToOrthogonal    
        { TrackElemType::LeftEighthDiveLoopDownToDiag, TrackCurve::None },            // TrackElemType::RightEighthDiveLoopUpToOrthogonal   
        { TrackCurve::None, TrackElemType::RightEighthDiveLoopUpToOrthogonal  },             // TrackElemType::LeftEighthDiveLoopDownToDiag  
        { TrackCurve::None, TrackElemType::LeftEighthDiveLoopUpToOrthogonal },             // TrackElemType::RightEighthDiveLoopDownToDiag 
    };
    static_assert(std::size(kTrackCurveChain) == EnumValue(TrackElemType::Count));
    
    const TrackDescriptor gTrackDescriptors[186] = {
        {   true,   TrackPitch::Down60,     TrackRoll::None,    TrackCurve::None,             TrackPitch::Down60,   TrackRoll::None,    TrackElemType::DiagDown60                              },
        {   true,   TrackPitch::Down60,     TrackRoll::None,    TrackCurve::None,             TrackPitch::Down25,   TrackRoll::None,    TrackElemType::DiagDown60ToDown25                      },
        {   true,   TrackPitch::Down60,     TrackRoll::None,    TrackCurve::None,             TrackPitch::None,     TrackRoll::None,    TrackElemType::DiagDown60ToFlat                        },
        {   true,   TrackPitch::Down25,     TrackRoll::None,    TrackCurve::None,             TrackPitch::Down60,   TrackRoll::None,    TrackElemType::DiagDown25ToDown60                      },
        {   true,   TrackPitch::Down25,     TrackRoll::None,    TrackCurve::None,             TrackPitch::Down25,   TrackRoll::None,    TrackElemType::DiagDown25                              },
        {   true,   TrackPitch::Down25,     TrackRoll::None,    TrackCurve::None,             TrackPitch::None,     TrackRoll::Left,    TrackElemType::DiagDown25ToLeftBank                    },
        {   true,   TrackPitch::Down25,     TrackRoll::None,    TrackCurve::None,             TrackPitch::None,     TrackRoll::None,    TrackElemType::DiagDown25ToFlat                        },
        {   true,   TrackPitch::Down25,     TrackRoll::None,    TrackCurve::None,             TrackPitch::None,     TrackRoll::Right,   TrackElemType::DiagDown25ToRightBank                   },
        {   true,   TrackPitch::None,       TrackRoll::Left,    TrackCurve::LeftLarge,        TrackPitch::None,     TrackRoll::Left,    TrackElemType::LeftEighthBankToOrthogonal              },
        {   true,   TrackPitch::None,       TrackRoll::Left,    TrackCurve::None,             TrackPitch::Down25,   TrackRoll::None,    TrackElemType::DiagLeftBankToDown25                    },
        {   true,   TrackPitch::None,       TrackRoll::Left,    TrackCurve::None,             TrackPitch::None,     TrackRoll::Left,    TrackElemType::DiagLeftBank                            },
        {   true,   TrackPitch::None,       TrackRoll::Left,    TrackCurve::None,             TrackPitch::None,     TrackRoll::None,    TrackElemType::DiagLeftBankToFlat                      },
        {   true,   TrackPitch::None,       TrackRoll::Left,    TrackCurve::None,             TrackPitch::Up25,     TrackRoll::None,    TrackElemType::DiagLeftBankToUp25                      },
        {   true,   TrackPitch::None,       TrackRoll::None,    TrackCurve::LeftLarge,        TrackPitch::None,     TrackRoll::None,    TrackElemType::LeftEighthToOrthogonal                  },
        {   true,   TrackPitch::None,       TrackRoll::None,    TrackCurve::None,             TrackPitch::Down60,   TrackRoll::None,    TrackElemType::DiagFlatToDown60                        },
        {   true,   TrackPitch::None,       TrackRoll::None,    TrackCurve::None,             TrackPitch::Down25,   TrackRoll::None,    TrackElemType::DiagFlatToDown25                        },
        {   true,   TrackPitch::None,       TrackRoll::None,    TrackCurve::None,             TrackPitch::None,     TrackRoll::Left,    TrackElemType::DiagFlatToLeftBank                      },
        {   true,   TrackPitch::None,       TrackRoll::None,    TrackCurve::None,             TrackPitch::None,     TrackRoll::None,    TrackElemType::DiagFlat                                },
        {   true,   TrackPitch::None,       TrackRoll::None,    TrackCurve::None,             TrackPitch::None,     TrackRoll::Right,   TrackElemType::DiagFlatToRightBank                     },
        {   true,   TrackPitch::None,       TrackRoll::None,    TrackCurve::None,             TrackPitch::Up25,     TrackRoll::None,    TrackElemType::DiagFlatToUp25                          },
        {   true,   TrackPitch::None,       TrackRoll::None,    TrackCurve::None,             TrackPitch::Up60,     TrackRoll::None,    TrackElemType::DiagFlatToUp60                          },
        {   true,   TrackPitch::None,       TrackRoll::None,    TrackCurve::RightLarge,       TrackPitch::None,     TrackRoll::None,    TrackElemType::RightEighthToOrthogonal                 },
        {   true,   TrackPitch::None,       TrackRoll::Right,   TrackCurve::None,             TrackPitch::Down25,   TrackRoll::None,    TrackElemType::DiagRightBankToDown25                   },
        {   true,   TrackPitch::None,       TrackRoll::Right,   TrackCurve::None,             TrackPitch::None,     TrackRoll::None,    TrackElemType::DiagRightBankToFlat                     },
        {   true,   TrackPitch::None,       TrackRoll::Right,   TrackCurve::None,             TrackPitch::None,     TrackRoll::Right,   TrackElemType::DiagRightBank                           },
        {   true,   TrackPitch::None,       TrackRoll::Right,   TrackCurve::None,             TrackPitch::Up25,     TrackRoll::None,    TrackElemType::DiagRightBankToUp25                     },
        {   true,   TrackPitch::None,       TrackRoll::Right,   TrackCurve::RightLarge,       TrackPitch::None,     TrackRoll::Right,   TrackElemType::RightEighthBankToOrthogonal             },
        {   true,   TrackPitch::Up25,       TrackRoll::None,    TrackCurve::None,             TrackPitch::None,     TrackRoll::Left,    TrackElemType::DiagUp25ToLeftBank                      },
        {   true,   TrackPitch::Up25,       TrackRoll::None,    TrackCurve::None,             TrackPitch::None,     TrackRoll::None,    TrackElemType::DiagUp25ToFlat                          },
        {   true,   TrackPitch::Up25,       TrackRoll::None,    TrackCurve::None,             TrackPitch::None,     TrackRoll::Right,   TrackElemType::DiagUp25ToRightBank                     },
        {   true,   TrackPitch::Up25,       TrackRoll::None,    TrackCurve::None,             TrackPitch::Up25,     TrackRoll::None,    TrackElemType::DiagUp25                                },
        {   true,   TrackPitch::Up25,       TrackRoll::None,    TrackCurve::None,             TrackPitch::Up60,     TrackRoll::None,    TrackElemType::DiagUp25ToUp60                          },
        {   true,   TrackPitch::Up60,       TrackRoll::None,    TrackCurve::None,             TrackPitch::None,     TrackRoll::None,    TrackElemType::DiagUp60ToFlat                          },
        {   true,   TrackPitch::Up60,       TrackRoll::None,    TrackCurve::None,             TrackPitch::Up25,     TrackRoll::None,    TrackElemType::DiagUp60ToUp25                          },
        {   true,   TrackPitch::Up60,       TrackRoll::None,    TrackCurve::None,             TrackPitch::Up60,     TrackRoll::None,    TrackElemType::DiagUp60                                },
        {   false,  TrackPitch::Down90,     TrackRoll::None,    TrackCurve::LeftSmall,        TrackPitch::Down90,   TrackRoll::None,    TrackElemType::LeftQuarterTurn1TileDown90              },
        {   false,  TrackPitch::Down90,     TrackRoll::None,    TrackCurve::None,             TrackPitch::Down90,   TrackRoll::None,    TrackElemType::Down90                                  },
        {   false,  TrackPitch::Down90,     TrackRoll::None,    TrackCurve::None,             TrackPitch::Down60,   TrackRoll::None,    TrackElemType::Down90ToDown60                          },
        {   false,  TrackPitch::Down90,     TrackRoll::None,    TrackCurve::RightSmall,       TrackPitch::Down90,   TrackRoll::None,    TrackElemType::RightQuarterTurn1TileDown90             },
        {   false,  TrackPitch::Down60,     TrackRoll::None,    TrackCurve::LeftSmall,        TrackPitch::Down60,   TrackRoll::None,    TrackElemType::LeftQuarterTurn1TileDown60              },
        {   false,  TrackPitch::Down60,     TrackRoll::None,    TrackCurve::None,             TrackPitch::Down90,   TrackRoll::None,    TrackElemType::Down60ToDown90                          },
        {   false,  TrackPitch::Down60,     TrackRoll::None,    TrackCurve::None,             TrackPitch::Down60,   TrackRoll::None,    TrackElemType::Down60                                  },
        {   false,  TrackPitch::Down60,     TrackRoll::None,    TrackCurve::None,             TrackPitch::Down25,   TrackRoll::None,    TrackElemType::Down60ToDown25                          },
        {   false,  TrackPitch::Down60,     TrackRoll::None,    TrackCurve::None,             TrackPitch::None,     TrackRoll::None,    TrackElemType::Down60ToFlat                            },
        {   false,  TrackPitch::Down60,     TrackRoll::None,    TrackCurve::RightSmall,       TrackPitch::Down60,   TrackRoll::None,    TrackElemType::RightQuarterTurn1TileDown60             },
        {   false,  TrackPitch::Down25,     TrackRoll::Left,    TrackCurve::LeftSmall,        TrackPitch::Down25,   TrackRoll::Left,    TrackElemType::LeftBankedQuarterTurn3TileDown25        },
        {   false,  TrackPitch::Down25,     TrackRoll::Left,    TrackCurve::Left,             TrackPitch::Down25,   TrackRoll::Left,    TrackElemType::LeftBankedQuarterTurn5TileDown25        },
        {   false,  TrackPitch::Down25,     TrackRoll::Left,    TrackCurve::None,             TrackPitch::Down25,   TrackRoll::Left,    TrackElemType::Down25LeftBanked                        },
        {   false,  TrackPitch::Down25,     TrackRoll::Left,    TrackCurve::None,             TrackPitch::Down25,   TrackRoll::None,    TrackElemType::LeftBankedDown25ToDown25                },
        {   false,  TrackPitch::Down25,     TrackRoll::Left,    TrackCurve::None,             TrackPitch::None,     TrackRoll::Left,    TrackElemType::LeftBankedDown25ToLeftBankedFlat        },
        {   false,  TrackPitch::Down25,     TrackRoll::Left,    TrackCurve::None,             TrackPitch::None,     TrackRoll::None,    TrackElemType::LeftBankedDown25ToFlat                  },
        {   false,  TrackPitch::Down25,     TrackRoll::None,    TrackCurve::LeftSmall,        TrackPitch::Down25,   TrackRoll::None,    TrackElemType::LeftQuarterTurn3TilesDown25             },
        {   false,  TrackPitch::Down25,     TrackRoll::None,    TrackCurve::LeftSmall,        TrackPitch::None,     TrackRoll::Left,    TrackElemType::LeftQuarterTurn3TilesDown25ToLeftBank   },
        {   false,  TrackPitch::Down25,     TrackRoll::None,    TrackCurve::Left,             TrackPitch::Down25,   TrackRoll::None,    TrackElemType::LeftQuarterTurn5TilesDown25             },
        {   false,  TrackPitch::Down25,     TrackRoll::None,    TrackCurve::None,             TrackPitch::Down60,   TrackRoll::None,    TrackElemType::Down25ToDown60                          },
        {   false,  TrackPitch::Down25,     TrackRoll::None,    TrackCurve::None,             TrackPitch::Down25,   TrackRoll::Left,    TrackElemType::Down25ToLeftBankedDown25                },
        {   false,  TrackPitch::Down25,     TrackRoll::None,    TrackCurve::None,             TrackPitch::Down25,   TrackRoll::None,    TrackElemType::Down25                                  },
        {   false,  TrackPitch::Down25,     TrackRoll::None,    TrackCurve::None,             TrackPitch::Down25,   TrackRoll::Right,   TrackElemType::Down25ToRightBankedDown25               },
        {   false,  TrackPitch::Down25,     TrackRoll::None,    TrackCurve::None,             TrackPitch::None,     TrackRoll::Left,    TrackElemType::Down25ToLeftBank                        },
        {   false,  TrackPitch::Down25,     TrackRoll::None,    TrackCurve::None,             TrackPitch::None,     TrackRoll::None,    TrackElemType::Down25ToFlat                            },
        {   false,  TrackPitch::Down25,     TrackRoll::None,    TrackCurve::None,             TrackPitch::None,     TrackRoll::Right,   TrackElemType::Down25ToRightBank                       },
        {   false,  TrackPitch::Down25,     TrackRoll::None,    TrackCurve::Right,            TrackPitch::Down25,   TrackRoll::None,    TrackElemType::RightQuarterTurn5TilesDown25            },
        {   false,  TrackPitch::Down25,     TrackRoll::None,    TrackCurve::RightSmall,       TrackPitch::None,     TrackRoll::Right,   TrackElemType::RightQuarterTurn3TilesDown25ToRightBank },
        {   false,  TrackPitch::Down25,     TrackRoll::None,    TrackCurve::RightSmall,       TrackPitch::Down25,   TrackRoll::None,    TrackElemType::RightQuarterTurn3TilesDown25            },
        {   false,  TrackPitch::Down25,     TrackRoll::Right,   TrackCurve::None,             TrackPitch::Down25,   TrackRoll::None,    TrackElemType::RightBankedDown25ToDown25               },
        {   false,  TrackPitch::Down25,     TrackRoll::Right,   TrackCurve::None,             TrackPitch::Down25,   TrackRoll::Right,   TrackElemType::Down25RightBanked                       },
        {   false,  TrackPitch::Down25,     TrackRoll::Right,   TrackCurve::None,             TrackPitch::None,     TrackRoll::None,    TrackElemType::RightBankedDown25ToFlat                 },
        {   false,  TrackPitch::Down25,     TrackRoll::Right,   TrackCurve::None,             TrackPitch::None,     TrackRoll::Right,   TrackElemType::RightBankedDown25ToRightBankedFlat      },
        {   false,  TrackPitch::Down25,     TrackRoll::Right,   TrackCurve::Right,            TrackPitch::Down25,   TrackRoll::Right,   TrackElemType::RightBankedQuarterTurn5TileDown25       },
        {   false,  TrackPitch::Down25,     TrackRoll::Right,   TrackCurve::RightSmall,       TrackPitch::Down25,   TrackRoll::Right,   TrackElemType::RightBankedQuarterTurn3TileDown25       },
        {   false,  TrackPitch::None,       TrackRoll::Left,    TrackCurve::LeftSmall,        TrackPitch::None,     TrackRoll::Left,    TrackElemType::LeftBankedQuarterTurn3Tiles             },
        {   false,  TrackPitch::None,       TrackRoll::Left,    TrackCurve::LeftSmall,        TrackPitch::Up25,     TrackRoll::None,    TrackElemType::LeftBankToLeftQuarterTurn3TilesUp25     },
        {   false,  TrackPitch::None,       TrackRoll::Left,    TrackCurve::Left,             TrackPitch::None,     TrackRoll::Left,    TrackElemType::BankedLeftQuarterTurn5Tiles             },
        {   false,  TrackPitch::None,       TrackRoll::Left,    TrackCurve::LeftLarge,        TrackPitch::None,     TrackRoll::Left,    TrackElemType::LeftEighthBankToDiag                    },
        {   false,  TrackPitch::None,       TrackRoll::Left,    TrackCurve::None,             TrackPitch::Down25,   TrackRoll::Left,    TrackElemType::LeftBankedFlatToLeftBankedDown25        },
        {   false,  TrackPitch::None,       TrackRoll::Left,    TrackCurve::None,             TrackPitch::Down25,   TrackRoll::None,    TrackElemType::LeftBankToDown25                        },
        {   false,  TrackPitch::None,       TrackRoll::Left,    TrackCurve::None,             TrackPitch::None,     TrackRoll::Left,    TrackElemType::LeftBank                                },
        {   false,  TrackPitch::None,       TrackRoll::Left,    TrackCurve::None,             TrackPitch::None,     TrackRoll::None,    TrackElemType::LeftBankToFlat                          },
        {   false,  TrackPitch::None,       TrackRoll::Left,    TrackCurve::None,             TrackPitch::Up25,     TrackRoll::Left,    TrackElemType::LeftBankedFlatToLeftBankedUp25          },
        {   false,  TrackPitch::None,       TrackRoll::Left,    TrackCurve::None,             TrackPitch::Up25,     TrackRoll::None,    TrackElemType::LeftBankToUp25                          },
        {   false,  TrackPitch::None,       TrackRoll::None,    TrackCurve::LeftSmall,        TrackPitch::None,     TrackRoll::None,    TrackElemType::LeftQuarterTurn3Tiles                   },
        {   false,  TrackPitch::None,       TrackRoll::None,    TrackCurve::Left,             TrackPitch::None,     TrackRoll::None,    TrackElemType::LeftQuarterTurn5Tiles                   },
        {   false,  TrackPitch::None,       TrackRoll::None,    TrackCurve::LeftLarge,        TrackPitch::None,     TrackRoll::None,    TrackElemType::LeftEighthToDiag                        },
        {   false,  TrackPitch::None,       TrackRoll::None,    TrackCurve::None,             TrackPitch::Down60,   TrackRoll::None,    TrackElemType::FlatToDown60                            },
        {   false,  TrackPitch::None,       TrackRoll::None,    TrackCurve::None,             TrackPitch::Down25,   TrackRoll::Left,    TrackElemType::FlatToLeftBankedDown25                  },
        {   false,  TrackPitch::None,       TrackRoll::None,    TrackCurve::None,             TrackPitch::Down25,   TrackRoll::None,    TrackElemType::FlatToDown25                            },
        {   false,  TrackPitch::None,       TrackRoll::None,    TrackCurve::None,             TrackPitch::Down25,   TrackRoll::Right,   TrackElemType::FlatToRightBankedDown25                 },
        {   false,  TrackPitch::None,       TrackRoll::None,    TrackCurve::None,             TrackPitch::None,     TrackRoll::Left,    TrackElemType::FlatToLeftBank                          },
        {   false,  TrackPitch::None,       TrackRoll::None,    TrackCurve::None,             TrackPitch::None,     TrackRoll::None,    TrackElemType::Flat                                    },
        {   false,  TrackPitch::None,       TrackRoll::None,    TrackCurve::LeftVerySmall,    TrackPitch::None,     TrackRoll::None,    TrackElemType::LeftQuarterTurn1Tile                    },
        {   false,  TrackPitch::None,       TrackRoll::None,    TrackCurve::RightVerySmall,   TrackPitch::None,     TrackRoll::None,    TrackElemType::RightQuarterTurn1Tile                   },
        {   false,  TrackPitch::None,       TrackRoll::None,    TrackCurve::None,             TrackPitch::None,     TrackRoll::Right,   TrackElemType::FlatToRightBank                         },
        {   false,  TrackPitch::None,       TrackRoll::None,    TrackCurve::None,             TrackPitch::Up25,     TrackRoll::Left,    TrackElemType::FlatToLeftBankedUp25                    },
        {   false,  TrackPitch::None,       TrackRoll::None,    TrackCurve::None,             TrackPitch::Up25,     TrackRoll::None,    TrackElemType::FlatToUp25                              },
        {   false,  TrackPitch::None,       TrackRoll::None,    TrackCurve::None,             TrackPitch::Up25,     TrackRoll::Right,   TrackElemType::FlatToRightBankedUp25                   },
        {   false,  TrackPitch::None,       TrackRoll::None,    TrackCurve::None,             TrackPitch::Up60,     TrackRoll::None,    TrackElemType::FlatToUp60                              },
        {   false,  TrackPitch::None,       TrackRoll::None,    TrackCurve::RightLarge,       TrackPitch::None,     TrackRoll::None,    TrackElemType::RightEighthToDiag                       },
        {   false,  TrackPitch::None,       TrackRoll::None,    TrackCurve::Right,            TrackPitch::None,     TrackRoll::None,    TrackElemType::RightQuarterTurn5Tiles                  },
        {   false,  TrackPitch::None,       TrackRoll::None,    TrackCurve::RightSmall,       TrackPitch::None,     TrackRoll::None,    TrackElemType::RightQuarterTurn3Tiles                  },
        {   false,  TrackPitch::None,       TrackRoll::Right,   TrackCurve::None,             TrackPitch::Down25,   TrackRoll::None,    TrackElemType::RightBankToDown25                       },
        {   false,  TrackPitch::None,       TrackRoll::Right,   TrackCurve::None,             TrackPitch::Down25,   TrackRoll::Right,   TrackElemType::RightBankedFlatToRightBankedDown25      },
        {   false,  TrackPitch::None,       TrackRoll::Right,   TrackCurve::None,             TrackPitch::None,     TrackRoll::None,    TrackElemType::RightBankToFlat                         },
        {   false,  TrackPitch::None,       TrackRoll::Right,   TrackCurve::None,             TrackPitch::None,     TrackRoll::Right,   TrackElemType::RightBank                               },
        {   false,  TrackPitch::None,       TrackRoll::Right,   TrackCurve::None,             TrackPitch::Up25,     TrackRoll::None,    TrackElemType::RightBankToUp25                         },
        {   false,  TrackPitch::None,       TrackRoll::Right,   TrackCurve::None,             TrackPitch::Up25,     TrackRoll::Right,   TrackElemType::RightBankedFlatToRightBankedUp25        },
        {   false,  TrackPitch::None,       TrackRoll::Right,   TrackCurve::RightLarge,       TrackPitch::None,     TrackRoll::Right,   TrackElemType::RightEighthBankToDiag                   },
        {   false,  TrackPitch::None,       TrackRoll::Right,   TrackCurve::Right,            TrackPitch::None,     TrackRoll::Right,   TrackElemType::BankedRightQuarterTurn5Tiles            },
        {   false,  TrackPitch::None,       TrackRoll::Right,   TrackCurve::RightSmall,       TrackPitch::None,     TrackRoll::Right,   TrackElemType::RightBankedQuarterTurn3Tiles            },
        {   false,  TrackPitch::None,       TrackRoll::Right,   TrackCurve::RightSmall,       TrackPitch::Up25,     TrackRoll::None,    TrackElemType::RightBankToRightQuarterTurn3TilesUp25   },
        {   false,  TrackPitch::Up25,       TrackRoll::Left,    TrackCurve::LeftSmall,        TrackPitch::Up25,     TrackRoll::Left,    TrackElemType::LeftBankedQuarterTurn3TileUp25          },
        {   false,  TrackPitch::Up25,       TrackRoll::Left,    TrackCurve::Left,             TrackPitch::Up25,     TrackRoll::Left,    TrackElemType::LeftBankedQuarterTurn5TileUp25          },
        {   false,  TrackPitch::Up25,       TrackRoll::Left,    TrackCurve::None,             TrackPitch::None,     TrackRoll::Left,    TrackElemType::LeftBankedUp25ToLeftBankedFlat          },
        {   false,  TrackPitch::Up25,       TrackRoll::Left,    TrackCurve::None,             TrackPitch::None,     TrackRoll::None,    TrackElemType::LeftBankedUp25ToFlat                    },
        {   false,  TrackPitch::Up25,       TrackRoll::Left,    TrackCurve::None,             TrackPitch::Up25,     TrackRoll::Left,    TrackElemType::Up25LeftBanked                          },
        {   false,  TrackPitch::Up25,       TrackRoll::Left,    TrackCurve::None,             TrackPitch::Up25,     TrackRoll::None,    TrackElemType::LeftBankedUp25ToUp25                    },
        {   false,  TrackPitch::Up25,       TrackRoll::None,    TrackCurve::LeftSmall,        TrackPitch::Up25,     TrackRoll::None,    TrackElemType::LeftQuarterTurn3TilesUp25               },
        {   false,  TrackPitch::Up25,       TrackRoll::None,    TrackCurve::Left,             TrackPitch::Up25,     TrackRoll::None,    TrackElemType::LeftQuarterTurn5TilesUp25               },
        {   false,  TrackPitch::Up25,       TrackRoll::None,    TrackCurve::None,             TrackPitch::None,     TrackRoll::Left,    TrackElemType::Up25ToLeftBank                          },
        {   false,  TrackPitch::Up25,       TrackRoll::None,    TrackCurve::None,             TrackPitch::None,     TrackRoll::None,    TrackElemType::Up25ToFlat                              },
        {   false,  TrackPitch::Up25,       TrackRoll::None,    TrackCurve::None,             TrackPitch::None,     TrackRoll::Right,   TrackElemType::Up25ToRightBank                         },
        {   false,  TrackPitch::Up25,       TrackRoll::None,    TrackCurve::None,             TrackPitch::Up25,     TrackRoll::Left,    TrackElemType::Up25ToLeftBankedUp25                    },
        {   false,  TrackPitch::Up25,       TrackRoll::None,    TrackCurve::None,             TrackPitch::Up25,     TrackRoll::None,    TrackElemType::Up25                                    },
        {   false,  TrackPitch::Up25,       TrackRoll::None,    TrackCurve::None,             TrackPitch::Up25,     TrackRoll::Right,   TrackElemType::Up25ToRightBankedUp25                   },
        {   false,  TrackPitch::Up25,       TrackRoll::None,    TrackCurve::None,             TrackPitch::Up60,     TrackRoll::None,    TrackElemType::Up25ToUp60                              },
        {   false,  TrackPitch::Up25,       TrackRoll::None,    TrackCurve::Right,            TrackPitch::Up25,     TrackRoll::None,    TrackElemType::RightQuarterTurn5TilesUp25              },
        {   false,  TrackPitch::Up25,       TrackRoll::None,    TrackCurve::RightSmall,       TrackPitch::Up25,     TrackRoll::None,    TrackElemType::RightQuarterTurn3TilesUp25              },
        {   false,  TrackPitch::Up25,       TrackRoll::Right,   TrackCurve::None,             TrackPitch::None,     TrackRoll::None,    TrackElemType::RightBankedUp25ToFlat                   },
        {   false,  TrackPitch::Up25,       TrackRoll::Right,   TrackCurve::None,             TrackPitch::None,     TrackRoll::Right,   TrackElemType::RightBankedUp25ToRightBankedFlat        },
        {   false,  TrackPitch::Up25,       TrackRoll::Right,   TrackCurve::None,             TrackPitch::Up25,     TrackRoll::None,    TrackElemType::RightBankedUp25ToUp25                   },
        {   false,  TrackPitch::Up25,       TrackRoll::Right,   TrackCurve::None,             TrackPitch::Up25,     TrackRoll::Right,   TrackElemType::Up25RightBanked                         },
        {   false,  TrackPitch::Up25,       TrackRoll::Right,   TrackCurve::Right,            TrackPitch::Up25,     TrackRoll::Right,   TrackElemType::RightBankedQuarterTurn5TileUp25         },
        {   false,  TrackPitch::Up25,       TrackRoll::Right,   TrackCurve::RightSmall,       TrackPitch::Up25,     TrackRoll::Right,   TrackElemType::RightBankedQuarterTurn3TileUp25         },
        {   false,  TrackPitch::Up60,       TrackRoll::None,    TrackCurve::LeftSmall,        TrackPitch::Up60,     TrackRoll::None,    TrackElemType::LeftQuarterTurn1TileUp60                },
        {   false,  TrackPitch::Up60,       TrackRoll::None,    TrackCurve::None,             TrackPitch::None,     TrackRoll::None,    TrackElemType::Up60ToFlat                              },
        {   false,  TrackPitch::Up60,       TrackRoll::None,    TrackCurve::None,             TrackPitch::Up25,     TrackRoll::None,    TrackElemType::Up60ToUp25                              },
        {   false,  TrackPitch::Up60,       TrackRoll::None,    TrackCurve::None,             TrackPitch::Up60,     TrackRoll::None,    TrackElemType::Up60                                    },
        {   false,  TrackPitch::Up60,       TrackRoll::None,    TrackCurve::None,             TrackPitch::Up90,     TrackRoll::None,    TrackElemType::Up60ToUp90                              },
        {   false,  TrackPitch::Up60,       TrackRoll::None,    TrackCurve::RightSmall,       TrackPitch::Up60,     TrackRoll::None,    TrackElemType::RightQuarterTurn1TileUp60               },
        {   false,  TrackPitch::Up90,       TrackRoll::None,    TrackCurve::LeftSmall,        TrackPitch::Up90,     TrackRoll::None,    TrackElemType::LeftQuarterTurn1TileUp90                },
        {   false,  TrackPitch::Up90,       TrackRoll::None,    TrackCurve::None,             TrackPitch::Up60,     TrackRoll::None,    TrackElemType::Up90ToUp60                              },
        {   false,  TrackPitch::Up90,       TrackRoll::None,    TrackCurve::None,             TrackPitch::Up90,     TrackRoll::None,    TrackElemType::Up90                                    },
        {   false,  TrackPitch::Up90,       TrackRoll::None,    TrackCurve::RightSmall,       TrackPitch::Up90,     TrackRoll::None,    TrackElemType::RightQuarterTurn1TileUp90               },
        {   false,  TrackPitch::Up25,       TrackRoll::None,    TrackCurve::LeftLarge,        TrackPitch::Up25,     TrackRoll::None,    TrackElemType::LeftEighthToDiagUp25                    },
        {   false,  TrackPitch::Up25,       TrackRoll::None,    TrackCurve::RightLarge,       TrackPitch::Up25,     TrackRoll::None,    TrackElemType::RightEighthToDiagUp25                   },
        {   false,  TrackPitch::Down25,     TrackRoll::None,    TrackCurve::LeftLarge,        TrackPitch::Down25,   TrackRoll::None,    TrackElemType::LeftEighthToDiagDown25                  },
        {   false,  TrackPitch::Down25,     TrackRoll::None,    TrackCurve::RightLarge,       TrackPitch::Down25,   TrackRoll::None,    TrackElemType::RightEighthToDiagDown25                 },
        {   true,   TrackPitch::Up25,       TrackRoll::None,    TrackCurve::LeftLarge,        TrackPitch::Up25,     TrackRoll::None,    TrackElemType::LeftEighthToOrthogonalUp25              },
        {   true,   TrackPitch::Up25,       TrackRoll::None,    TrackCurve::RightLarge,       TrackPitch::Up25,     TrackRoll::None,    TrackElemType::RightEighthToOrthogonalUp25             },
        {   true,   TrackPitch::Down25,     TrackRoll::None,    TrackCurve::LeftLarge,        TrackPitch::Down25,   TrackRoll::None,    TrackElemType::LeftEighthToOrthogonalDown25            },
        {   true,   TrackPitch::Down25,     TrackRoll::None,    TrackCurve::RightLarge,       TrackPitch::Down25,   TrackRoll::None,    TrackElemType::RightEighthToOrthogonalDown25           },
        {   true,   TrackPitch::Up25,       TrackRoll::None,    TrackCurve::None,             TrackPitch::Up25,     TrackRoll::Left,    TrackElemType::DiagUp25ToLeftBankedUp25                },
        {   true,   TrackPitch::Up25,       TrackRoll::None,    TrackCurve::None,             TrackPitch::Up25,     TrackRoll::Right,   TrackElemType::DiagUp25ToRightBankedUp25               },
        {   true,   TrackPitch::Up25,       TrackRoll::Left,    TrackCurve::None,             TrackPitch::Up25,     TrackRoll::None,    TrackElemType::DiagLeftBankedUp25ToUp25                },
        {   true,   TrackPitch::Up25,       TrackRoll::Right,   TrackCurve::None,             TrackPitch::Up25,     TrackRoll::None,    TrackElemType::DiagRightBankedUp25ToUp25               },
        {   true,   TrackPitch::Down25,     TrackRoll::None,    TrackCurve::None,             TrackPitch::Down25,   TrackRoll::Left,    TrackElemType::DiagDown25ToLeftBankedDown25            },
        {   true,   TrackPitch::Down25,     TrackRoll::None,    TrackCurve::None,             TrackPitch::Down25,   TrackRoll::Right,   TrackElemType::DiagDown25ToRightBankedDown25           },
        {   true,   TrackPitch::Down25,     TrackRoll::Left,    TrackCurve::None,             TrackPitch::Down25,   TrackRoll::None,    TrackElemType::DiagLeftBankedDown25ToDown25            },
        {   true,   TrackPitch::Down25,     TrackRoll::Right,   TrackCurve::None,             TrackPitch::Down25,   TrackRoll::None,    TrackElemType::DiagRightBankedDown25ToDown25           },
        {   true,   TrackPitch::None,       TrackRoll::Left,    TrackCurve::None,             TrackPitch::Up25,     TrackRoll::Left,    TrackElemType::DiagLeftBankedFlatToLeftBankedUp25      },
        {   true,   TrackPitch::None,       TrackRoll::Right,   TrackCurve::None,             TrackPitch::Up25,     TrackRoll::Right,   TrackElemType::DiagRightBankedFlatToRightBankedUp25    },
        {   true,   TrackPitch::Up25,       TrackRoll::Left,    TrackCurve::None,             TrackPitch::None,     TrackRoll::Left,    TrackElemType::DiagLeftBankedUp25ToLeftBankedFlat      },
        {   true,   TrackPitch::Up25,       TrackRoll::Right,   TrackCurve::None,             TrackPitch::None,     TrackRoll::Right,   TrackElemType::DiagRightBankedUp25ToRightBankedFlat    },
        {   true,   TrackPitch::None,       TrackRoll::Left,    TrackCurve::None,             TrackPitch::Down25,   TrackRoll::Left,    TrackElemType::DiagLeftBankedFlatToLeftBankedDown25    },
        {   true,   TrackPitch::None,       TrackRoll::Right,   TrackCurve::None,             TrackPitch::Down25,   TrackRoll::Right,   TrackElemType::DiagRightBankedFlatToRightBankedDown25  },
        {   true,   TrackPitch::Down25,     TrackRoll::Left,    TrackCurve::None,             TrackPitch::None,     TrackRoll::Left,    TrackElemType::DiagLeftBankedDown25ToLeftBankedFlat    },
        {   true,   TrackPitch::Down25,     TrackRoll::Right,   TrackCurve::None,             TrackPitch::None,     TrackRoll::Right,   TrackElemType::DiagRightBankedDown25ToRightBankedFlat  },
        {   true,   TrackPitch::None,       TrackRoll::None,    TrackCurve::None,             TrackPitch::Up25,     TrackRoll::Left,    TrackElemType::DiagFlatToLeftBankedUp25                },
        {   true,   TrackPitch::None,       TrackRoll::None,    TrackCurve::None,             TrackPitch::Up25,     TrackRoll::Right,   TrackElemType::DiagFlatToRightBankedUp25               },
        {   true,   TrackPitch::Up25,       TrackRoll::Left,    TrackCurve::None,             TrackPitch::None,     TrackRoll::None,    TrackElemType::DiagLeftBankedUp25ToFlat                },
        {   true,   TrackPitch::Up25,       TrackRoll::Right,   TrackCurve::None,             TrackPitch::None,     TrackRoll::None,    TrackElemType::DiagRightBankedUp25ToFlat               },
        {   true,   TrackPitch::None,       TrackRoll::None,    TrackCurve::None,             TrackPitch::Down25,   TrackRoll::Left,    TrackElemType::DiagFlatToLeftBankedDown25              },
        {   true,   TrackPitch::None,       TrackRoll::None,    TrackCurve::None,             TrackPitch::Down25,   TrackRoll::Right,   TrackElemType::DiagFlatToRightBankedDown25             },
        {   true,   TrackPitch::Down25,     TrackRoll::Left,    TrackCurve::None,             TrackPitch::None,     TrackRoll::None,    TrackElemType::DiagLeftBankedDown25ToFlat              },
        {   true,   TrackPitch::Down25,     TrackRoll::Right,   TrackCurve::None,             TrackPitch::None,     TrackRoll::None,    TrackElemType::DiagRightBankedDown25ToFlat             },
        {   true,   TrackPitch::Up25,       TrackRoll::Left,    TrackCurve::None,             TrackPitch::Up25,     TrackRoll::Left,    TrackElemType::DiagUp25LeftBanked                      },
        {   true,   TrackPitch::Up25,       TrackRoll::Right,   TrackCurve::None,             TrackPitch::Up25,     TrackRoll::Right,   TrackElemType::DiagUp25RightBanked                     },
        {   true,   TrackPitch::Down25,     TrackRoll::Left,    TrackCurve::None,             TrackPitch::Down25,   TrackRoll::Left,    TrackElemType::DiagDown25LeftBanked                    },
        {   true,   TrackPitch::Down25,     TrackRoll::Right,   TrackCurve::None,             TrackPitch::Down25,   TrackRoll::Right,   TrackElemType::DiagDown25RightBanked                   },
        {   false,  TrackPitch::Up25,       TrackRoll::Left,    TrackCurve::LeftLarge,        TrackPitch::Up25,     TrackRoll::Left,    TrackElemType::LeftEighthBankToDiagUp25                },
        {   false,  TrackPitch::Up25,       TrackRoll::Right,   TrackCurve::RightLarge,       TrackPitch::Up25,     TrackRoll::Right,   TrackElemType::RightEighthBankToDiagUp25               },
        {   false,  TrackPitch::Down25,     TrackRoll::Left,    TrackCurve::LeftLarge,        TrackPitch::Down25,   TrackRoll::Left,    TrackElemType::LeftEighthBankToDiagDown25              },
        {   false,  TrackPitch::Down25,     TrackRoll::Right,   TrackCurve::RightLarge,       TrackPitch::Down25,   TrackRoll::Right,   TrackElemType::RightEighthBankToDiagDown25             },
        {   true,   TrackPitch::Up25,       TrackRoll::Left,    TrackCurve::LeftLarge,        TrackPitch::Up25,     TrackRoll::Left,    TrackElemType::LeftEighthBankToOrthogonalUp25          },
        {   true,   TrackPitch::Up25,       TrackRoll::Right,   TrackCurve::RightLarge,       TrackPitch::Up25,     TrackRoll::Right,   TrackElemType::RightEighthBankToOrthogonalUp25         },
        {   true,   TrackPitch::Down25,     TrackRoll::Left,    TrackCurve::LeftLarge,        TrackPitch::Down25,   TrackRoll::Left,    TrackElemType::LeftEighthBankToOrthogonalDown25        },
        {   true,   TrackPitch::Down25,     TrackRoll::Right,   TrackCurve::RightLarge,       TrackPitch::Down25,   TrackRoll::Right,   TrackElemType::RightEighthBankToOrthogonalDown25       },
    };
    
    /** rct2: 0x00993D1C */
    static constexpr OpenRCT2::TrackElemType kAlternativeTrackTypes[] = {
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
        TrackElemType::None, // LeftFlyerLargeHalfLoopInvertedDown
        TrackElemType::None, // RightFlyerLargeHalfLoopInvertedDown
        TrackElemType::None, // FlyerHalfLoopInvertedUp
        TrackElemType::None, // FlyerHalfLoopUninvertedDown
        TrackElemType::None, // TrackElemType::LeftEighthToDiagUp25          
        TrackElemType::None, // TrackElemType::RightEighthToDiagUp25        
        TrackElemType::None, // TrackElemType::LeftEighthToDiagDown25       
        TrackElemType::None, // TrackElemType::RightEighthToDiagDown25      
        TrackElemType::None, // TrackElemType::LeftEighthToOrthogonalUp25   
        TrackElemType::None, // TrackElemType::RightEighthToOrthogonalUp25  
        TrackElemType::None, // TrackElemType::LeftEighthToOrthogonalDown25 
        TrackElemType::None, // TrackElemType::RightEighthToOrthogonalDown25 
        TrackElemType::None, // TrackElemType::DiagUp25ToLeftBankedUp25
        TrackElemType::None, // TrackElemType::DiagUp25ToRightBankedUp25
        TrackElemType::None, // TrackElemType::DiagLeftBankedUp25ToUp25
        TrackElemType::None, // TrackElemType::DiagRightBankedUp25ToUp25
        TrackElemType::None, // TrackElemType::DiagDown25ToLeftBankedDown25
        TrackElemType::None, // TrackElemType::DiagDown25ToRightBankedDown25
        TrackElemType::None, // TrackElemType::DiagLeftBankedDown25ToDown25
        TrackElemType::None, // TrackElemType::DiagRightBankedDown25ToDown25
        TrackElemType::None, // TrackElemType::DiagLeftBankedFlatToLeftBankedUp25
        TrackElemType::None, // TrackElemType::DiagRightBankedFlatToRightBankedUp25
        TrackElemType::None, // TrackElemType::DiagLeftBankedUp25ToLeftBankedFlat
        TrackElemType::None, // TrackElemType::DiagRightBankedUp25ToRightBankedFlat
        TrackElemType::None, // TrackElemType::DiagLeftBankedFlatToLeftBankedDown25
        TrackElemType::None, // TrackElemType::DiagRightBankedFlatToRightBankedDown25
        TrackElemType::None, // TrackElemType::DiagLeftBankedDown25ToLeftBankedFlat
        TrackElemType::None, // TrackElemType::DiagRightBankedDown25ToRightBankedFlat
        TrackElemType::None, // TrackElemType::DiagFlatToLeftBankedUp25
        TrackElemType::None, // TrackElemType::DiagFlatToRightBankedUp25
        TrackElemType::None, // TrackElemType::DiagLeftBankedUp25ToFlat
        TrackElemType::None, // TrackElemType::DiagRightBankedUp25ToFlat
        TrackElemType::None, // TrackElemType::DiagFlatToLeftBankedDown25
        TrackElemType::None, // TrackElemType::DiagFlatToRightBankedDown25
        TrackElemType::None, // TrackElemType::DiagLeftBankedDown25ToFlat
        TrackElemType::None, // TrackElemType::DiagRightBankedDown25ToFlat
        TrackElemType::None, // TrackElemType::DiagUp25LeftBanked
        TrackElemType::None, // TrackElemType::DiagUp25RightBanked 
        TrackElemType::None, // TrackElemType::DiagDown25LeftBanked
        TrackElemType::None, // TrackElemType::DiagDown25RightBanked
        TrackElemType::None, // TrackElemType::LeftEighthBankToDiagUp25
        TrackElemType::None, // TrackElemType::RightEighthBankToDiagUp25
        TrackElemType::None, // TrackElemType::LeftEighthBankToDiagDown25
        TrackElemType::None, // TrackElemType::RightEighthBankToDiagDown25
        TrackElemType::None, // TrackElemType::LeftEighthBankToOrthogonalUp25
        TrackElemType::None, // TrackElemType::RightEighthBankToOrthogonalUp25 
        TrackElemType::None, // TrackElemType::LeftEighthBankToOrthogonalDown25
        TrackElemType::None, // TrackElemType::RightEighthBankToOrthogonalDown25
        TrackElemType::None, // TrackElemType::DiagBrakes
        TrackElemType::None, // TrackElemType::DiagBlockBrakes
        TrackElemType::None, // TrackElemType::Down25Brakes
        TrackElemType::None, // TrackElemType::DiagBooster
        TrackElemType::None, // TrackElemType::DiagFlatToUp60LongBase,
        TrackElemType::None, // TrackElemType::DiagUp60ToFlatLongBase,
        TrackElemType::None, // TrackElemType::DiagFlatToDown60LongBase,
        TrackElemType::None, // TrackElemType::DiagDown60ToFlatLongBase,
        TrackElemType::None, // TrackElemType::LeftEighthDiveLoopUpToOrthogonal   
        TrackElemType::None, // TrackElemType::RightEighthDiveLoopUpToOrthogonal  
        TrackElemType::None, // TrackElemType::LeftEighthDiveLoopDownToDiag 
        TrackElemType::None, // TrackElemType::RightEighthDiveLoopDownToDiag
    };
    static_assert(std::size(kAlternativeTrackTypes) == EnumValue(TrackElemType::Count));
    
    /** rct2: 0x0099DA34 */
    static constexpr money64 kTrackPricing[] = {
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
        884736, // TrackElemType::LeftLargeHalfLoopDown
        884736, // TrackElemType::RightLargeHalfLoopDown
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
        884736, // TrackElemType::LeftFlyerLargeHalfLoopInvertedDown
        884736, // TrackElemType::RightFlyerLargeHalfLoopInvertedDown
        884736, // TrackElemType::LeftFlyerLargeHalfLoopInvertedUp
        884736, // TrackElemType::RightFlyerLargeHalfLoopInvertedUp
        884736, // TrackElemType::LeftFlyerLargeHalfLoopUninvertedDown
        884736, // TrackElemType::RightFlyerLargeHalfLoopUninvertedDown
        294912, // TrackElemType::FlyerHalfLoopInvertedUp
        294912, // TrackElemType::FlyerHalfLoopUninvertedDown
        219559, // TrackElemType::LeftEighthToDiagUp25
        219559, // TrackElemType::RightEighthToDiagUp25
        219559, // TrackElemType::LeftEighthToDiagDown25
        219559, // TrackElemType::RightEighthToDiagDown25
        219559, // TrackElemType::LeftEighthToOrthogonalUp25
        219559, // TrackElemType::RightEighthToOrthogonalUp25
        219559, // TrackElemType::LeftEighthToOrthogonalDown25
        219559, // TrackElemType::RightEighthToOrthogonalDown25
        118809, // TrackElemType::DiagUp25ToLeftBankedUp25
        118809, // TrackElemType::DiagUp25ToRightBankedUp25
        118809, // TrackElemType::DiagLeftBankedUp25ToUp25
        118809, // TrackElemType::DiagRightBankedUp25ToUp25
        118809, // TrackElemType::DiagDown25ToLeftBankedDown25
        118809, // TrackElemType::DiagDown25ToRightBankedDown25
        118809, // TrackElemType::DiagLeftBankedDown25ToDown25
        118809, // TrackElemType::DiagRightBankedDown25ToDown25
        108800, // TrackElemType::DiagLeftBankedFlatToLeftBankedUp25
        108800, // TrackElemType::DiagRightBankedFlatToRightBankedUp25
        108800, // TrackElemType::DiagLeftBankedUp25ToLeftBankedFlat
        108800, // TrackElemType::DiagRightBankedUp25ToRightBankedFlat
        108800, // TrackElemType::DiagLeftBankedFlatToLeftBankedDown25
        108800, // TrackElemType::DiagRightBankedFlatToRightBankedDown25
        108800, // TrackElemType::DiagLeftBankedDown25ToLeftBankedFlat
        108800, // TrackElemType::DiagRightBankedDown25ToRightBankedFlat
        108800, // TrackElemType::DiagFlatToLeftBankedUp25
        108800, // TrackElemType::DiagFlatToRightBankedUp25
        108800, // TrackElemType::DiagLeftBankedUp25ToFlat
        108800, // TrackElemType::DiagRightBankedUp25ToFlat
        108800, // TrackElemType::DiagFlatToLeftBankedDown25
        108800, // TrackElemType::DiagFlatToRightBankedDown25
        108800, // TrackElemType::DiagLeftBankedDown25ToFlat
        108800, // TrackElemType::DiagRightBankedDown25ToFlat
        118809, // TrackElemType::DiagUp25LeftBanked
        118809, // TrackElemType::DiagUp25RightBanked 
        118809, // TrackElemType::DiagDown25LeftBanked
        118809, // TrackElemType::DiagDown25RightBanked
        233281, // TrackElemType::LeftEighthBankToDiagUp25
        233281, // TrackElemType::RightEighthBankToDiagUp25
        233281, // TrackElemType::LeftEighthBankToDiagDown25
        233281, // TrackElemType::RightEighthBankToDiagDown25
        233281, // TrackElemType::LeftEighthBankToOrthogonalUp25
        233281, // TrackElemType::RightEighthBankToOrthogonalUp25 
        233281, // TrackElemType::LeftEighthBankToOrthogonalDown25
        233281, // TrackElemType::RightEighthBankToOrthogonalDown25
        123456, // TrackElemType::DiagBrakes
        123456, // TrackElemType::DiagBlockBrakes
        109824, // TrackElemType::Down25Brakes
        123456, // TrackElemType::DiagBooster
        399690 , // TrackElemType::DiagFlatToUp60LongBase,
        399690, // TrackElemType::DiagUp60ToFlatLongBase,
        399690, // TrackElemType::DiagFlatToDown60LongBase,
        399690, // TrackElemType::DiagDown60ToFlatLongBase,
        458752, // TrackElemType::LeftEighthDiveLoopUpToOrthogonal   
        458752, // TrackElemType::RightEighthDiveLoopUpToOrthogonal  
        458752, // TrackElemType::LeftEighthDiveLoopDownToDiag 
        458752, // TrackElemType::RightEighthDiveLoopDownToDiag
    };
    static_assert(std::size(kTrackPricing) == EnumValue(TrackElemType::Count));
    
    /** rct2: 0x0099EA1C */
    static constexpr OpenRCT2::TrackElemType kTrackElementMirrorMap[] = {
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
        TrackElemType::RightLargeHalfLoopDown, // TrackElemType::LeftLargeHalfLoopDown
        TrackElemType::LeftLargeHalfLoopDown, // TrackElemType::RightLargeHalfLoopDown
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
        TrackElemType::RightFlyerLargeHalfLoopInvertedDown, // TrackElemType::LeftFlyerLargeHalfLoopInvertedDown
        TrackElemType::LeftFlyerLargeHalfLoopInvertedDown, // TrackElemType::RightFlyerLargeHalfLoopInvertedDown
        TrackElemType::RightFlyerLargeHalfLoopInvertedUp, // TrackElemType::LeftFlyerLargeHalfLoopInvertedUp
        TrackElemType::LeftFlyerLargeHalfLoopInvertedUp, // TrackElemType::RightFlyerLargeHalfLoopInvertedUp
        TrackElemType::RightFlyerLargeHalfLoopUninvertedDown, // TrackElemType::LeftFlyerLargeHalfLoopUninvertedDown
        TrackElemType::LeftFlyerLargeHalfLoopUninvertedDown, // TrackElemType::RightFlyerLargeHalfLoopUninvertedDown
        TrackElemType::FlyerHalfLoopInvertedUp,
        TrackElemType::FlyerHalfLoopUninvertedDown,
        TrackElemType::RightEighthToDiagUp25,          // TrackElemType::LeftEighthToDiagUp25           
        TrackElemType::LeftEighthToDiagUp25,           // TrackElemType::RightEighthToDiagUp25        
        TrackElemType::RightEighthToDiagDown25,        // TrackElemType::LeftEighthToDiagDown25       
        TrackElemType::LeftEighthToDiagDown25,         // TrackElemType::RightEighthToDiagDown25      
        TrackElemType::RightEighthToOrthogonalUp25,    // TrackElemType::LeftEighthToOrthogonalUp25   
        TrackElemType::LeftEighthToOrthogonalUp25,     // TrackElemType::RightEighthToOrthogonalUp25  
        TrackElemType::RightEighthToOrthogonalDown25,  // TrackElemType::LeftEighthToOrthogonalDown25
        TrackElemType::LeftEighthToOrthogonalDown25,   // TrackElemType::RightEighthToOrthogonalDown25
        TrackElemType::DiagUp25ToRightBankedUp25,             // TrackElemType::DiagUp25ToRightBankedUp25
        TrackElemType::DiagUp25ToLeftBankedUp25,              // TrackElemType::DiagUp25ToLeftBankedUp25
        TrackElemType::DiagRightBankedUp25ToUp25,             // TrackElemType::DiagRightBankedUp25ToUp25
        TrackElemType::DiagLeftBankedUp25ToUp25,              // TrackElemType::DiagLeftBankedUp25ToUp25
        TrackElemType::DiagDown25ToRightBankedDown25,         // TrackElemType::DiagDown25ToRightBankedDown25
        TrackElemType::DiagDown25ToLeftBankedDown25,          // TrackElemType::DiagDown25ToLeftBankedDown25
        TrackElemType::DiagRightBankedDown25ToDown25,         // TrackElemType::DiagRightBankedDown25ToDown25
        TrackElemType::DiagLeftBankedDown25ToDown25,          // TrackElemType::DiagLeftBankedDown25ToDown25
        TrackElemType::DiagRightBankedFlatToRightBankedUp25,  // TrackElemType::DiagRightBankedFlatToRightBankedUp25
        TrackElemType::DiagLeftBankedFlatToLeftBankedUp25,    // TrackElemType::DiagLeftBankedFlatToLeftBankedUp25
        TrackElemType::DiagRightBankedUp25ToRightBankedFlat,  // TrackElemType::DiagRightBankedUp25ToRightBankedFlat
        TrackElemType::DiagLeftBankedUp25ToLeftBankedFlat,    // TrackElemType::DiagLeftBankedUp25ToLeftBankedFlat
        TrackElemType::DiagRightBankedFlatToRightBankedDown25,// TrackElemType::DiagRightBankedFlatToRightBankedDown25
        TrackElemType::DiagLeftBankedFlatToLeftBankedDown25,  // TrackElemType::DiagLeftBankedFlatToLeftBankedDown25
        TrackElemType::DiagRightBankedDown25ToRightBankedFlat,// TrackElemType::DiagRightBankedDown25ToRightBankedFlat
        TrackElemType::DiagLeftBankedDown25ToLeftBankedFlat,  // TrackElemType::DiagLeftBankedDown25ToLeftBankedFlat
        TrackElemType::DiagFlatToRightBankedUp25,             // TrackElemType::DiagFlatToRightBankedUp25
        TrackElemType::DiagFlatToLeftBankedUp25,              // TrackElemType::DiagFlatToLeftBankedUp25
        TrackElemType::DiagRightBankedUp25ToFlat,             // TrackElemType::DiagRightBankedUp25ToFlat
        TrackElemType::DiagLeftBankedUp25ToFlat,              // TrackElemType::DiagLeftBankedUp25ToFlat
        TrackElemType::DiagFlatToRightBankedDown25,           // TrackElemType::DiagFlatToRightBankedDown25
        TrackElemType::DiagFlatToLeftBankedDown25,            // TrackElemType::DiagFlatToLeftBankedDown25
        TrackElemType::DiagRightBankedDown25ToFlat,           // TrackElemType::DiagRightBankedDown25ToFlat
        TrackElemType::DiagLeftBankedDown25ToFlat,            // TrackElemType::DiagLeftBankedDown25ToFlat
        TrackElemType::DiagUp25RightBanked,                   // TrackElemType::DiagUp25RightBanked
        TrackElemType::DiagUp25LeftBanked,                    // TrackElemType::DiagUp25LeftBanked 
        TrackElemType::DiagDown25RightBanked,                 // TrackElemType::DiagDown25RightBanked
        TrackElemType::DiagDown25LeftBanked,                  // TrackElemType::DiagDown25LeftBanked
        TrackElemType::RightEighthBankToDiagUp25,             // TrackElemType::RightEighthBankToDiagUp25
        TrackElemType::LeftEighthBankToDiagUp25,              // TrackElemType::LeftEighthBankToDiagUp25
        TrackElemType::RightEighthBankToDiagDown25,           // TrackElemType::RightEighthBankToDiagDown25
        TrackElemType::LeftEighthBankToDiagDown25,            // TrackElemType::LeftEighthBankToDiagDown25
        TrackElemType::RightEighthBankToOrthogonalUp25,       // TrackElemType::RightEighthBankToOrthogonalUp25
        TrackElemType::LeftEighthBankToOrthogonalUp25,        // TrackElemType::LeftEighthBankToOrthogonalUp25 
        TrackElemType::RightEighthBankToOrthogonalDown25,     // TrackElemType::RightEighthBankToOrthogonalDown25
        TrackElemType::LeftEighthBankToOrthogonalDown25,      // TrackElemType::LeftEighthBankToOrthogonalDown25
        TrackElemType::DiagBrakes,
        TrackElemType::DiagBlockBrakes,
        TrackElemType::Down25Brakes,
        TrackElemType::DiagBooster,
        TrackElemType::DiagFlatToUp60LongBase,                // TrackElemType::DiagFlatToUp60LongBase,
        TrackElemType::DiagUp60ToFlatLongBase,                // TrackElemType::DiagUp60ToFlatLongBase,
        TrackElemType::DiagFlatToDown60LongBase,              // TrackElemType::DiagFlatToDown60LongBase,
        TrackElemType::DiagDown60ToFlatLongBase,              // TrackElemType::DiagDown60ToFlatLongBase,
        TrackElemType::RightEighthDiveLoopUpToOrthogonal,     // TrackElemType::LeftEighthDiveLoopUpToOrthogonal   
        TrackElemType::LeftEighthDiveLoopUpToOrthogonal,      // TrackElemType::RightEighthDiveLoopUpToOrthogonal  
        TrackElemType::RightEighthDiveLoopDownToDiag,   // TrackElemType::LeftEighthDiveLoopDownToDiag 
        TrackElemType::LeftEighthDiveLoopDownToDiag,    // TrackElemType::RightEighthDiveLoopDownToDiag
    };
    static_assert(std::size(kTrackElementMirrorMap) == EnumValue(TrackElemType::Count));
    
    /** rct2: 0x00999694 */
    static constexpr uint32_t kTrackHeightMarkerPositions[] = {
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
        (1 << 0) | (1 << 6), // TrackElemType::LeftLargeHalfLoopDown
        (1 << 0) | (1 << 6), // TrackElemType::RightLargeHalfLoopDown
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
        (1 << 0) | (1 << 6), // TrackElemType::LeftFlyerLargeHalfLoopInvertedDown
        (1 << 0) | (1 << 6), // TrackElemType::RightFlyerLargeHalfLoopInvertedDown
        (1 << 0) | (1 << 6), // TrackElemType::LeftFlyerLargeHalfLoopInvertedUp
        (1 << 0) | (1 << 6), // TrackElemType::RightFlyerLargeHalfLoopInvertedUp
        (1 << 0) | (1 << 6), // TrackElemType::LeftFlyerLargeHalfLoopUninvertedDown
        (1 << 0) | (1 << 6), // TrackElemType::RightFlyerLargeHalfLoopUninvertedDown
        (1 << 0) | (1 << 3), // TrackElemType::FlyerHalfLoopInvertedUp
        (1 << 0) | (1 << 3), // TrackElemType::FlyerHalfLoopUninvertedDown
        (1 << 0) | (1 << 4), // TrackElemType::LeftEighthToDiagUp25         
        (1 << 0) | (1 << 4), // TrackElemType::RightEighthToDiagUp25        
        (1 << 0) | (1 << 4), // TrackElemType::LeftEighthToDiagDown25       
        (1 << 0) | (1 << 4), // TrackElemType::RightEighthToDiagDown25      
        (1 << 0) | (1 << 4), // TrackElemType::LeftEighthToOrthogonalUp25   
        (1 << 0) | (1 << 4), // TrackElemType::RightEighthToOrthogonalUp25  
        (1 << 0) | (1 << 4), // TrackElemType::LeftEighthToOrthogonalDown25 
        (1 << 0) | (1 << 4), // TrackElemType::RightEighthToOrthogonalDown25
        (1 << 0), // TrackElemType::DiagUp25ToRightBankedUp25
        (1 << 0), // TrackElemType::DiagUp25ToLeftBankedUp25
        (1 << 0), // TrackElemType::DiagRightBankedUp25ToUp25
        (1 << 0), // TrackElemType::DiagLeftBankedUp25ToUp25
        (1 << 0), // TrackElemType::DiagDown25ToRightBankedDown25
        (1 << 0), // TrackElemType::DiagDown25ToLeftBankedDown25
        (1 << 0), // TrackElemType::DiagRightBankedDown25ToDown25
        (1 << 0), // TrackElemType::DiagLeftBankedDown25ToDown25
        (1 << 0), // TrackElemType::DiagRightBankedFlatToRightBankedUp25
        (1 << 0), // TrackElemType::DiagLeftBankedFlatToLeftBankedUp25
        (1 << 0), // TrackElemType::DiagRightBankedUp25ToRightBankedFlat
        (1 << 0), // TrackElemType::DiagLeftBankedUp25ToLeftBankedFlat
        (1 << 0), // TrackElemType::DiagRightBankedFlatToRightBankedDown25
        (1 << 0), // TrackElemType::DiagLeftBankedFlatToLeftBankedDown25
        (1 << 0), // TrackElemType::DiagRightBankedDown25ToRightBankedFlat
        (1 << 0), // TrackElemType::DiagLeftBankedDown25ToLeftBankedFlat
        (1 << 0), // TrackElemType::DiagFlatToRightBankedUp25
        (1 << 0), // TrackElemType::DiagFlatToLeftBankedUp25
        (1 << 0), // TrackElemType::DiagRightBankedUp25ToFlat
        (1 << 0), // TrackElemType::DiagLeftBankedUp25ToFlat
        (1 << 0), // TrackElemType::DiagFlatToRightBankedDown25
        (1 << 0), // TrackElemType::DiagFlatToLeftBankedDown25
        (1 << 0), // TrackElemType::DiagRightBankedDown25ToFlat
        (1 << 0), // TrackElemType::DiagLeftBankedDown25ToFlat
        (1 << 0), // TrackElemType::DiagUp25RightBanked
        (1 << 0), // TrackElemType::DiagUp25LeftBanked 
        (1 << 0), // TrackElemType::DiagDown25RightBanked
        (1 << 0), // TrackElemType::DiagDown25LeftBanked
        (1 << 0) | (1 << 4), // TrackElemType::RightEighthBankToDiagUp25
        (1 << 0) | (1 << 4), // TrackElemType::LeftEighthBankToDiagUp25
        (1 << 0) | (1 << 4), // TrackElemType::RightEighthBankToDiagDown25
        (1 << 0) | (1 << 4), // TrackElemType::LeftEighthBankToDiagDown25
        (1 << 0) | (1 << 4), // TrackElemType::RightEighthBankToOrthogonalUp25
        (1 << 0) | (1 << 4), // TrackElemType::LeftEighthBankToOrthogonalUp25 
        (1 << 0) | (1 << 4), // TrackElemType::RightEighthBankToOrthogonalDown25
        (1 << 0) | (1 << 4), // TrackElemType::LeftEighthBankToOrthogonalDown25
        (1 << 0), // TrackElemType::DiagBrakes
        (1 << 0), // TrackElemType::DiagBlockBrakes
        (1 << 0), // TrackElemType::Down25Brakes
        (1 << 0), // TrackElemType::DiagBooster
        (1 << 0), // TrackElemType::DiagFlatToUp60LongBase,
        (1 << 0), // TrackElemType::DiagUp60ToFlatLongBase,
        (1 << 9), // TrackElemType::DiagFlatToDown60LongBase,
        (1 << 9), // TrackElemType::DiagDown60ToFlatLongBase,
        (1 << 0) | (1 << 5), // TrackElemType::LeftEighthDiveLoopUpToOrthogonal   
        (1 << 0) | (1 << 5), // TrackElemType::RightEighthDiveLoopUpToOrthogonal  
        (1 << 0) | (1 << 5), // TrackElemType::LeftEighthDiveLoopDownToDiag 
        (1 << 0) | (1 << 5), // TrackElemType::RightEighthDiveLoopDownToDiag
    };
    static_assert(std::size(kTrackHeightMarkerPositions) == EnumValue(TrackElemType::Count));
    
    
    
    
    /** rct2: 0x0099423C */
    static constexpr uint32_t kTrackFlags[] = {
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
        /* TrackElemType::LeftLargeHalfLoopDown                         */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
        /* TrackElemType::RightLargeHalfLoopDown                        */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
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
        /* TrackElemType::LeftHeartLineRoll                             */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL | TRACK_ELEM_FLAG_CAN_BE_PARTLY_UNDERGROUND,
        /* TrackElemType::RightHeartLineRoll                            */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL | TRACK_ELEM_FLAG_CAN_BE_PARTLY_UNDERGROUND,
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
        /* TrackElemType::LeftMediumHalfLoopDown                        */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
        /* TrackElemType::RightMediumHalfLoopDown                       */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
        /* TrackElemType::LeftZeroGRollUp                               */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
        /* TrackElemType::RightZeroGRollUp                              */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
        /* TrackElemType::LeftZeroGRollDown                             */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
        /* TrackElemType::RightZeroGRollDown                            */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
        /* TrackElemType::LeftLargeZeroGRollUp                          */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
        /* TrackElemType::RightLargeZeroGRollUp                         */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
        /* TrackElemType::LeftLargeZeroGRollDown                        */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
        /* TrackElemType::RightLargeZeroGRollDown                       */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
        /* TrackElemType::LeftFlyerLargeHalfLoopUninvertedUp            */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
        /* TrackElemType::RightFlyerLargeHalfLoopUninvertedUp           */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
        /* TrackElemType::LeftFlyerLargeHalfLoopInvertedDown            */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
        /* TrackElemType::RightFlyerLargeHalfLoopInvertedDown           */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
        /* TrackElemType::LeftFlyerLargeHalfLoopInvertedUp              */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
        /* TrackElemType::RightFlyerLargeHalfLoopInvertedUp             */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
        /* TrackElemType::LeftFlyerLargeHalfLoopUninvertedDown          */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION,
        /* TrackElemType::RightFlyerLargeHalfLoopUninvertedDown         */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION,
        /* TrackElemType::FlyerHalfLoopInvertedUp                       */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
        /* TrackElemType::FlyerHalfLoopUnivertedDown                    */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION,
        /* TrackElemType::LeftEighthToDiagUp25                          */   TRACK_ELEM_FLAG_TURN_LEFT  | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP   | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,
        /* TrackElemType::RightEighthToDiagUp25                         */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP   | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,
        /* TrackElemType::LeftEighthToDiagDown25                        */   TRACK_ELEM_FLAG_TURN_LEFT  | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,
        /* TrackElemType::RightEighthToDiagDown25                       */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,
        /* TrackElemType::LeftEighthToOrthogonalUp25                    */   TRACK_ELEM_FLAG_TURN_LEFT  | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP   | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,
        /* TrackElemType::RightEighthToOrthogonalUp25                   */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP   | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,
        /* TrackElemType::LeftEighthToOrthogonalDown25                  */   TRACK_ELEM_FLAG_TURN_LEFT  | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,
        /* TrackElemType::RightEighthToOrthogonalDown25                 */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,
        /* TrackElemType::DiagUp25ToLeftBankedUp25               */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,   
        /* TrackElemType::DiagUp25ToRightBankedUp25              */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,   
        /* TrackElemType::DiagLeftBankedUp25ToUp25               */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,   
        /* TrackElemType::DiagRightBankedUp25ToUp25              */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,   
        /* TrackElemType::DiagDown25ToLeftBankedDown25           */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,    
        /* TrackElemType::DiagDown25ToRightBankedDown25          */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,       
        /* TrackElemType::DiagLeftBankedDown25ToDown25           */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,       
        /* TrackElemType::DiagRightBankedDown25ToDown25          */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,       
        /* TrackElemType::DiagLeftBankedFlatToLeftBankedUp25     */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED,   
        /* TrackElemType::DiagRightBankedFlatToRightBankedUp25   */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED,   
        /* TrackElemType::DiagLeftBankedUp25ToLeftBankedFlat     */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,   
        /* TrackElemType::DiagRightBankedUp25ToRightBankedFlat   */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,   
        /* TrackElemType::DiagLeftBankedFlatToLeftBankedDown25   */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,       
        /* TrackElemType::DiagRightBankedFlatToRightBankedDown25 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,       
        /* TrackElemType::DiagLeftBankedDown25ToLeftBankedFlat   */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED,       
        /* TrackElemType::DiagRightBankedDown25ToRightBankedFlat */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED,       
        /* TrackElemType::DiagFlatToLeftBankedUp25               */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED,   
        /* TrackElemType::DiagFlatToRightBankedUp25              */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED,   
        /* TrackElemType::DiagLeftBankedUp25ToFlat               */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,   
        /* TrackElemType::DiagRightBankedUp25ToFlat              */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,   
        /* TrackElemType::DiagFlatToLeftBankedDown25             */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,       
        /* TrackElemType::DiagFlatToRightBankedDown25            */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,       
        /* TrackElemType::DiagLeftBankedDown25ToFlat             */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED,       
        /* TrackElemType::DiagRightBankedDown25ToFlat            */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED,       
        /* TrackElemType::Up25LeftBanked                         */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,   
        /* TrackElemType::Up25RightBanked                        */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,   
        /* TrackElemType::Down25LeftBanked                       */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,       
        /* TrackElemType::Down25RightBanked                      */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,       
        /* TrackElemType::LeftEighthBankToDiagUp25               */   TRACK_ELEM_FLAG_TURN_LEFT  | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP   | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,  
        /* TrackElemType::RightEighthBankToDiagUp25              */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP   | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,  
        /* TrackElemType::LeftEighthBankToDiagDown25             */   TRACK_ELEM_FLAG_TURN_LEFT  | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,  
        /* TrackElemType::RightEighthBankToDiagDown25            */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,  
        /* TrackElemType::LeftEighthBankToOrthogonalUp25         */   TRACK_ELEM_FLAG_TURN_LEFT  | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP   | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,  
        /* TrackElemType::RightEighthBankToOrthogonalUp25        */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP   | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,  
        /* TrackElemType::LeftEighthBankToOrthogonalDown25       */   TRACK_ELEM_FLAG_TURN_LEFT  | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,  
        /* TrackElemType::RightEighthBankToOrthogonalDown25      */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,  
        /* TrackElemType::DiagBrakes                             */   0,
        /* TrackElemType::DiagBlockBrakes                        */   0,
        /* TrackElemType::Down25Brakes                           */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
        /* TrackElemType::DiagBooster                            */   0,
        /* TrackElemType::DiagFlatToUp60LongBase                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_IS_STEEP_UP,
        /* TrackElemType::DiagUp60ToFlatLongBase                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_IS_STEEP_UP,
        /* TrackElemType::DiagFlatToDown60LongBase               */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
        /* TrackElemType::DiagDown60ToFlatLongBase               */   TRACK_ELEM_FLAG_DOWN,                                                                    
        /* TrackElemType::LeftEighthDiveLoopUpToOrthogonal       */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
        /* TrackElemType::RightEighthDiveLoopUpToOrthogonal      */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
        /* TrackElemType::LeftEighthDiveLoopDownToDiag     */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
        /* TrackElemType::RightEighthDiveLoopDownToDiag    */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    };
    static_assert(std::size(kTrackFlags) == EnumValue(TrackElemType::Count));
    // clang-format on

    /**  rct2: 0x00997C9D */
    // clang-format off
    static constexpr TrackDefinition kTrackDefinitions[] =
    {
        // TYPE                                          VANGLE END                    VANGLE START                BANK END                BANK START              PREVIEW Z OFFSET
        { TrackGroup::straight,                          TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Flat
        { TrackGroup::stationEnd,                        TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::EndStation
        { TrackGroup::stationEnd,                        TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::BeginStation
        { TrackGroup::stationEnd,                        TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::MiddleStation
        { TrackGroup::slope,                             TrackPitch::Up25,             TrackPitch::Up25,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Up25
        { TrackGroup::slopeSteepUp,                      TrackPitch::Up60,             TrackPitch::Up60,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Up60
        { TrackGroup::slope,                             TrackPitch::Up25,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::FlatToUp25
        { TrackGroup::slopeSteepUp,                      TrackPitch::Up60,             TrackPitch::Up25,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Up25ToUp60
        { TrackGroup::slopeSteepUp,                      TrackPitch::Up25,             TrackPitch::Up60,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Up60ToUp25
        { TrackGroup::slope,                             TrackPitch::None,             TrackPitch::Up25,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Up25ToFlat
        { TrackGroup::slope,                             TrackPitch::Down25,           TrackPitch::Down25,         TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Down25
        { TrackGroup::slopeSteepDown,                    TrackPitch::Down60,           TrackPitch::Down60,         TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Down60
        { TrackGroup::slope,                             TrackPitch::Down25,           TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::FlatToDown25
        { TrackGroup::slopeSteepDown,                    TrackPitch::Down60,           TrackPitch::Down25,         TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Down25ToDown60
        { TrackGroup::slopeSteepDown,                    TrackPitch::Down25,           TrackPitch::Down60,         TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Down60ToDown25
        { TrackGroup::slope,                             TrackPitch::None,             TrackPitch::Down25,         TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Down25ToFlat
        { TrackGroup::curve,                             TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::LeftQuarterTurn5Tiles
        { TrackGroup::curve,                             TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::RightQuarterTurn5Tiles
        { TrackGroup::flatRollBanking,                   TrackPitch::None,             TrackPitch::None,           TrackRoll::Left,        TrackRoll::None,        0   }, // TrackElemType::FlatToLeftBank
        { TrackGroup::flatRollBanking,                   TrackPitch::None,             TrackPitch::None,           TrackRoll::Right,       TrackRoll::None,        0   }, // TrackElemType::FlatToRightBank
        { TrackGroup::flatRollBanking,                   TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::Left,        0   }, // TrackElemType::LeftBankToFlat
        { TrackGroup::flatRollBanking,                   TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::Right,       0   }, // TrackElemType::RightBankToFlat
        { TrackGroup::flatRollBanking,                   TrackPitch::None,             TrackPitch::None,           TrackRoll::Left,        TrackRoll::Left,        0   }, // TrackElemType::BankedLeftQuarterTurn5Tiles
        { TrackGroup::flatRollBanking,                   TrackPitch::None,             TrackPitch::None,           TrackRoll::Right,       TrackRoll::Right,       0   }, // TrackElemType::BankedRightQuarterTurn5Tiles
        { TrackGroup::flatRollBanking,                   TrackPitch::Up25,             TrackPitch::None,           TrackRoll::None,        TrackRoll::Left,        0   }, // TrackElemType::LeftBankToUp25
        { TrackGroup::flatRollBanking,                   TrackPitch::Up25,             TrackPitch::None,           TrackRoll::None,        TrackRoll::Right,       0   }, // TrackElemType::RightBankToUp25
        { TrackGroup::flatRollBanking,                   TrackPitch::None,             TrackPitch::Up25,           TrackRoll::Left,        TrackRoll::None,        0   }, // TrackElemType::Up25ToLeftBank
        { TrackGroup::flatRollBanking,                   TrackPitch::None,             TrackPitch::Up25,           TrackRoll::Right,       TrackRoll::None,        0   }, // TrackElemType::Up25ToRightBank
        { TrackGroup::flatRollBanking,                   TrackPitch::Down25,           TrackPitch::None,           TrackRoll::None,        TrackRoll::Left,        0   }, // TrackElemType::LeftBankToDown25
        { TrackGroup::flatRollBanking,                   TrackPitch::Down25,           TrackPitch::None,           TrackRoll::None,        TrackRoll::Right,       0   }, // TrackElemType::RightBankToDown25
        { TrackGroup::flatRollBanking,                   TrackPitch::None,             TrackPitch::Down25,         TrackRoll::Left,        TrackRoll::None,        0   }, // TrackElemType::Down25ToLeftBank
        { TrackGroup::flatRollBanking,                   TrackPitch::None,             TrackPitch::Down25,         TrackRoll::Right,       TrackRoll::None,        0   }, // TrackElemType::Down25ToRightBank
        { TrackGroup::flatRollBanking,                   TrackPitch::None,             TrackPitch::None,           TrackRoll::Left,        TrackRoll::Left,        0   }, // TrackElemType::LeftBank
        { TrackGroup::flatRollBanking,                   TrackPitch::None,             TrackPitch::None,           TrackRoll::Right,       TrackRoll::Right,       0   }, // TrackElemType::RightBank
        { TrackGroup::slopeCurve,                        TrackPitch::Up25,             TrackPitch::Up25,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::LeftQuarterTurn5TilesUp25
        { TrackGroup::slopeCurve,                        TrackPitch::Up25,             TrackPitch::Up25,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::RightQuarterTurn5TilesUp25
        { TrackGroup::slopeCurve,                        TrackPitch::Down25,           TrackPitch::Down25,         TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::LeftQuarterTurn5TilesDown25
        { TrackGroup::slopeCurve,                        TrackPitch::Down25,           TrackPitch::Down25,         TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::RightQuarterTurn5TilesDown25
        { TrackGroup::sBend,                             TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::SBendLeft
        { TrackGroup::sBend,                             TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::SBendRight
        { TrackGroup::verticalLoop,                      TrackPitch::Down25,           TrackPitch::Up25,           TrackRoll::None,        TrackRoll::None,        -48 }, // TrackElemType::LeftVerticalLoop
        { TrackGroup::verticalLoop,                      TrackPitch::Down25,           TrackPitch::Up25,           TrackRoll::None,        TrackRoll::None,        -48 }, // TrackElemType::RightVerticalLoop
        { TrackGroup::curveSmall,                        TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::LeftQuarterTurn3Tiles
        { TrackGroup::curveSmall,                        TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::RightQuarterTurn3Tiles
        { TrackGroup::flatRollBanking,                   TrackPitch::None,             TrackPitch::None,           TrackRoll::Left,        TrackRoll::Left,        0   }, // TrackElemType::LeftBankedQuarterTurn3Tiles
        { TrackGroup::flatRollBanking,                   TrackPitch::None,             TrackPitch::None,           TrackRoll::Right,       TrackRoll::Right,       0   }, // TrackElemType::RightBankedQuarterTurn3Tiles
        { TrackGroup::slopeCurve,                        TrackPitch::Up25,             TrackPitch::Up25,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::LeftQuarterTurn3TilesUp25
        { TrackGroup::slopeCurve,                        TrackPitch::Up25,             TrackPitch::Up25,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::RightQuarterTurn3TilesUp25
        { TrackGroup::slopeCurve,                        TrackPitch::Down25,           TrackPitch::Down25,         TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::LeftQuarterTurn3TilesDown25
        { TrackGroup::slopeCurve,                        TrackPitch::Down25,           TrackPitch::Down25,         TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::RightQuarterTurn3TilesDown25
        { TrackGroup::curveVerySmall,                    TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::LeftQuarterTurn1Tile
        { TrackGroup::curveVerySmall,                    TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::RightQuarterTurn1Tile
        { TrackGroup::twist,                             TrackPitch::None,             TrackPitch::None,           TrackRoll::UpsideDown,  TrackRoll::None,        0   }, // TrackElemType::LeftTwistDownToUp
        { TrackGroup::twist,                             TrackPitch::None,             TrackPitch::None,           TrackRoll::UpsideDown,  TrackRoll::None,        0   }, // TrackElemType::RightTwistDownToUp
        { TrackGroup::twist,                             TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::UpsideDown,  0   }, // TrackElemType::LeftTwistUpToDown
        { TrackGroup::twist,                             TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::UpsideDown,  0   }, // TrackElemType::RightTwistUpToDown
        { TrackGroup::halfLoop,                          TrackPitch::None,             TrackPitch::Up25,           TrackRoll::UpsideDown,  TrackRoll::None,        64  }, // TrackElemType::HalfLoopUp
        { TrackGroup::halfLoop,                          TrackPitch::Down25,           TrackPitch::None,           TrackRoll::None,        TrackRoll::UpsideDown,  -64 }, // TrackElemType::HalfLoopDown
        { TrackGroup::corkscrew,                         TrackPitch::None,             TrackPitch::None,           TrackRoll::UpsideDown,  TrackRoll::None,        0   }, // TrackElemType::LeftCorkscrewUp
        { TrackGroup::corkscrew,                         TrackPitch::None,             TrackPitch::None,           TrackRoll::UpsideDown,  TrackRoll::None,        0   }, // TrackElemType::RightCorkscrewUp
        { TrackGroup::corkscrew,                         TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::UpsideDown,  -32 }, // TrackElemType::LeftCorkscrewDown
        { TrackGroup::corkscrew,                         TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::UpsideDown,  -32 }, // TrackElemType::RightCorkscrewDown
        { TrackGroup::flatToSteepSlope,                  TrackPitch::Up60,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::FlatToUp60
        { TrackGroup::flatToSteepSlope,                  TrackPitch::None,             TrackPitch::Up60,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Up60ToFlat
        { TrackGroup::flatToSteepSlope,                  TrackPitch::Down60,           TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::FlatToDown60
        { TrackGroup::flatToSteepSlope,                  TrackPitch::None,             TrackPitch::Down60,         TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Down60ToFlat
        { TrackGroup::tower,                             TrackPitch::Tower,            TrackPitch::Tower,          TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::TowerBase
        { TrackGroup::tower,                             TrackPitch::Tower,            TrackPitch::Tower,          TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::TowerSection
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::FlatCovered
        { TrackGroup::flat,                              TrackPitch::Up25,             TrackPitch::Up25,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Up25Covered
        { TrackGroup::flat,                              TrackPitch::Up60,             TrackPitch::Up60,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Up60Covered
        { TrackGroup::flat,                              TrackPitch::Up25,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::FlatToUp25Covered
        { TrackGroup::flat,                              TrackPitch::Up60,             TrackPitch::Up25,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Up25ToUp60Covered
        { TrackGroup::flat,                              TrackPitch::Up25,             TrackPitch::Up60,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Up60ToUp25Covered
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::Up25,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Up25ToFlatCovered
        { TrackGroup::flat,                              TrackPitch::Down25,           TrackPitch::Down25,         TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Down25Covered
        { TrackGroup::flat,                              TrackPitch::Down60,           TrackPitch::Down60,         TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Down60Covered
        { TrackGroup::flat,                              TrackPitch::Down25,           TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::FlatToDown25Covered
        { TrackGroup::flat,                              TrackPitch::Down60,           TrackPitch::Down25,         TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Down25ToDown60Covered
        { TrackGroup::flat,                              TrackPitch::Down25,           TrackPitch::Down60,         TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Down60ToDown25Covered
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::Down25,         TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Down25ToFlatCovered
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::LeftQuarterTurn5TilesCovered
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::RightQuarterTurn5TilesCovered
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::SBendLeftCovered
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::SBendRightCovered
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::LeftQuarterTurn3TilesCovered
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::RightQuarterTurn3TilesCovered
        { TrackGroup::helixUpBankedHalf,                 TrackPitch::None,             TrackPitch::None,           TrackRoll::Left,        TrackRoll::Left,        0   }, // TrackElemType::LeftHalfBankedHelixUpSmall
        { TrackGroup::helixUpBankedHalf,                 TrackPitch::None,             TrackPitch::None,           TrackRoll::Right,       TrackRoll::Right,       0   }, // TrackElemType::RightHalfBankedHelixUpSmall
        { TrackGroup::helixDownBankedHalf,               TrackPitch::None,             TrackPitch::None,           TrackRoll::Left,        TrackRoll::Left,        0   }, // TrackElemType::LeftHalfBankedHelixDownSmall
        { TrackGroup::helixDownBankedHalf,               TrackPitch::None,             TrackPitch::None,           TrackRoll::Right,       TrackRoll::Right,       0   }, // TrackElemType::RightHalfBankedHelixDownSmall
        { TrackGroup::helixUpBankedHalf,                 TrackPitch::None,             TrackPitch::None,           TrackRoll::Left,        TrackRoll::Left,        0   }, // TrackElemType::LeftHalfBankedHelixUpLarge
        { TrackGroup::helixUpBankedHalf,                 TrackPitch::None,             TrackPitch::None,           TrackRoll::Right,       TrackRoll::Right,       0   }, // TrackElemType::RightHalfBankedHelixUpLarge
        { TrackGroup::helixDownBankedHalf,               TrackPitch::None,             TrackPitch::None,           TrackRoll::Left,        TrackRoll::Left,        0   }, // TrackElemType::LeftHalfBankedHelixDownLarge
        { TrackGroup::helixDownBankedHalf,               TrackPitch::None,             TrackPitch::None,           TrackRoll::Right,       TrackRoll::Right,       0   }, // TrackElemType::RightHalfBankedHelixDownLarge
        { TrackGroup::slopeCurveSteep,                   TrackPitch::Up60,             TrackPitch::Up60,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::LeftQuarterTurn1TileUp60
        { TrackGroup::slopeCurveSteep,                   TrackPitch::Up60,             TrackPitch::Up60,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::RightQuarterTurn1TileUp60
        { TrackGroup::slopeCurveSteep,                   TrackPitch::Down60,           TrackPitch::Down60,         TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::LeftQuarterTurn1TileDown60
        { TrackGroup::slopeCurveSteep,                   TrackPitch::Down60,           TrackPitch::Down60,         TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::RightQuarterTurn1TileDown60
        { TrackGroup::brakes,                            TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Brakes
        { TrackGroup::booster,                           TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Booster
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Maze
        { TrackGroup::helixUpBankedQuarter,              TrackPitch::None,             TrackPitch::None,           TrackRoll::Left,        TrackRoll::Left,        0   }, // TrackElemType::LeftQuarterBankedHelixLargeUp
        { TrackGroup::helixUpBankedQuarter,              TrackPitch::None,             TrackPitch::None,           TrackRoll::Right,       TrackRoll::Right,       0   }, // TrackElemType::RightQuarterBankedHelixLargeUp
        { TrackGroup::helixDownBankedQuarter,            TrackPitch::None,             TrackPitch::None,           TrackRoll::Left,        TrackRoll::Left,        0   }, // TrackElemType::LeftQuarterBankedHelixLargeDown
        { TrackGroup::helixDownBankedQuarter,            TrackPitch::None,             TrackPitch::None,           TrackRoll::Right,       TrackRoll::Right,       0   }, // TrackElemType::RightQuarterBankedHelixLargeDown
        { TrackGroup::helixUpUnbankedQuarter,            TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::LeftQuarterHelixLargeUp
        { TrackGroup::helixUpUnbankedQuarter,            TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::RightQuarterHelixLargeUp
        { TrackGroup::helixDownUnbankedQuarter,          TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::LeftQuarterHelixLargeDown
        { TrackGroup::helixDownUnbankedQuarter,          TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::RightQuarterHelixLargeDown
        { TrackGroup::slopeRollBanking,                  TrackPitch::Up25,             TrackPitch::Up25,           TrackRoll::Left,        TrackRoll::Left,        0   }, // TrackElemType::Up25LeftBanked
        { TrackGroup::slopeRollBanking,                  TrackPitch::Up25,             TrackPitch::Up25,           TrackRoll::Right,       TrackRoll::Right,       0   }, // TrackElemType::Up25RightBanked
        { TrackGroup::waterfall,                         TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Waterfall
        { TrackGroup::rapids,                            TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Rapids
        { TrackGroup::onridePhoto,                       TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::OnRidePhoto
        { TrackGroup::slopeRollBanking,                  TrackPitch::Down25,           TrackPitch::Down25,         TrackRoll::Left,        TrackRoll::Left,        0   }, // TrackElemType::Down25LeftBanked
        { TrackGroup::slopeRollBanking,                  TrackPitch::Down25,           TrackPitch::Down25,         TrackRoll::Right,       TrackRoll::Right,       0   }, // TrackElemType::Down25RightBanked
        { TrackGroup::waterSplash,                       TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Watersplash
        { TrackGroup::slopeSteepLong,                    TrackPitch::Up60,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::FlatToUp60LongBase
        { TrackGroup::slopeSteepLong,                    TrackPitch::None,             TrackPitch::Up60,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Up60ToFlatLongBase
        { TrackGroup::whirlpool,                         TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Whirlpool
        { TrackGroup::slopeSteepLong,                    TrackPitch::None,             TrackPitch::Down60,         TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Down60ToFlatLongBase
        { TrackGroup::slopeSteepLong,                    TrackPitch::Down60,           TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::FlatToDown60LongBase
        { TrackGroup::liftHillCable,                     TrackPitch::Down60,           TrackPitch::Up25,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::CableLiftHill
        { TrackGroup::reverseFreefall,                   TrackPitch::ReverseFreefall,  TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::ReverseFreefallSlope
        { TrackGroup::reverseFreefall,                   TrackPitch::ReverseFreefall,  TrackPitch::ReverseFreefall,TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::ReverseFreefallVertical
        { TrackGroup::flat,                              TrackPitch::Up90,             TrackPitch::Up90,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Up90
        { TrackGroup::flat,                              TrackPitch::Down90,           TrackPitch::Down90,         TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Down90
        { TrackGroup::flat,                              TrackPitch::Up90,             TrackPitch::Up60,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Up60ToUp90
        { TrackGroup::flat,                              TrackPitch::Down60,           TrackPitch::Down90,         TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Down90ToDown60
        { TrackGroup::flat,                              TrackPitch::Up60,             TrackPitch::Up90,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Up90ToUp60
        { TrackGroup::flat,                              TrackPitch::Down90,           TrackPitch::Down60,         TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::Down60ToDown90
        { TrackGroup::brakeForDrop,                      TrackPitch::Down60,           TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::BrakeForDrop
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::LeftEighthToDiag
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::RightEighthToDiag
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::LeftEighthToOrthogonal
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::RightEighthToOrthogonal
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::None,           TrackRoll::Left,        TrackRoll::Left,        0   }, // TrackElemType::LeftEighthBankToDiag
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::None,           TrackRoll::Right,       TrackRoll::Right,       0   }, // TrackElemType::RightEighthBankToDiag
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::None,           TrackRoll::Left,        TrackRoll::Left,        0   }, // TrackElemType::LeftEighthBankToOrthogonal
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::None,           TrackRoll::Right,       TrackRoll::Right,       0   }, // TrackElemType::RightEighthBankToOrthogonal
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::DiagFlat
        { TrackGroup::flat,                              TrackPitch::Up25,             TrackPitch::Up25,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::DiagUp25
        { TrackGroup::flat,                              TrackPitch::Up60,             TrackPitch::Up60,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::DiagUp60
        { TrackGroup::flat,                              TrackPitch::Up25,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::DiagFlatToUp25
        { TrackGroup::flat,                              TrackPitch::Up60,             TrackPitch::Up25,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::DiagUp25ToUp60
        { TrackGroup::flat,                              TrackPitch::Up25,             TrackPitch::Up60,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::DiagUp60ToUp25
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::Up25,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::DiagUp25ToFlat
        { TrackGroup::flat,                              TrackPitch::Down25,           TrackPitch::Down25,         TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::DiagDown25
        { TrackGroup::flat,                              TrackPitch::Down60,           TrackPitch::Down60,         TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::DiagDown60
        { TrackGroup::flat,                              TrackPitch::Down25,           TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::DiagFlatToDown25
        { TrackGroup::flat,                              TrackPitch::Down60,           TrackPitch::Down25,         TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::DiagDown25ToDown60
        { TrackGroup::flat,                              TrackPitch::Down25,           TrackPitch::Down60,         TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::DiagDown60ToDown25
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::Down25,         TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::DiagDown25ToFlat
        { TrackGroup::flat,                              TrackPitch::Up60,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::DiagFlatToUp60
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::Up60,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::DiagUp60ToFlat
        { TrackGroup::flat,                              TrackPitch::Down60,           TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::DiagFlatToDown60
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::Down60,         TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::DiagDown60ToFlat
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::None,           TrackRoll::Left,        TrackRoll::None,        0   }, // TrackElemType::DiagFlatToLeftBank
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::None,           TrackRoll::Right,       TrackRoll::None,        0   }, // TrackElemType::DiagFlatToRightBank
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::Left,        0   }, // TrackElemType::DiagLeftBankToFlat
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::Right,       0   }, // TrackElemType::DiagRightBankToFlat
        { TrackGroup::flat,                              TrackPitch::Up25,             TrackPitch::None,           TrackRoll::None,        TrackRoll::Left,        0   }, // TrackElemType::DiagLeftBankToUp25
        { TrackGroup::flat,                              TrackPitch::Up25,             TrackPitch::None,           TrackRoll::None,        TrackRoll::Right,       0   }, // TrackElemType::DiagRightBankToUp25
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::Up25,           TrackRoll::Left,        TrackRoll::None,        0   }, // TrackElemType::DiagUp25ToLeftBank
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::Up25,           TrackRoll::Right,       TrackRoll::None,        0   }, // TrackElemType::DiagUp25ToRightBank
        { TrackGroup::flat,                              TrackPitch::Down25,           TrackPitch::None,           TrackRoll::None,        TrackRoll::Left,        0   }, // TrackElemType::DiagLeftBankToDown25
        { TrackGroup::flat,                              TrackPitch::Down25,           TrackPitch::None,           TrackRoll::None,        TrackRoll::Right,       0   }, // TrackElemType::DiagRightBankToDown25
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::Down25,         TrackRoll::Left,        TrackRoll::None,        0   }, // TrackElemType::DiagDown25ToLeftBank
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::Down25,         TrackRoll::Right,       TrackRoll::None,        0   }, // TrackElemType::DiagDown25ToRightBank
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::None,           TrackRoll::Left,        TrackRoll::Left,        0   }, // TrackElemType::DiagLeftBank
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::None,           TrackRoll::Right,       TrackRoll::Right,       0   }, // TrackElemType::DiagRightBank
        { TrackGroup::logFlumeReverser,                  TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::LogFlumeReverser
        { TrackGroup::spinningTunnel,                    TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::SpinningTunnel
        { TrackGroup::barrelRoll,                        TrackPitch::None,             TrackPitch::None,           TrackRoll::UpsideDown,  TrackRoll::None,        0   }, // TrackElemType::LeftBarrelRollUpToDown
        { TrackGroup::barrelRoll,                        TrackPitch::None,             TrackPitch::None,           TrackRoll::UpsideDown,  TrackRoll::None,        0   }, // TrackElemType::RightBarrelRollUpToDown
        { TrackGroup::barrelRoll,                        TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::UpsideDown,  0   }, // TrackElemType::LeftBarrelRollDownToUp
        { TrackGroup::barrelRoll,                        TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::UpsideDown,  0   }, // TrackElemType::RightBarrelRollDownToUp
        { TrackGroup::flat,                              TrackPitch::Up25,             TrackPitch::None,           TrackRoll::None,        TrackRoll::Left,        0   }, // TrackElemType::LeftBankToLeftQuarterTurn3TilesUp25
        { TrackGroup::flat,                              TrackPitch::Up25,             TrackPitch::None,           TrackRoll::None,        TrackRoll::Right,       0   }, // TrackElemType::RightBankToRightQuarterTurn3TilesUp25
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::Down25,         TrackRoll::Left,        TrackRoll::None,        0   }, // TrackElemType::LeftQuarterTurn3TilesDown25ToLeftBank
        { TrackGroup::flat,                              TrackPitch::None,             TrackPitch::Down25,         TrackRoll::Right,       TrackRoll::None,        0   }, // TrackElemType::RightQuarterTurn3TilesDown25ToRightBank
        { TrackGroup::poweredLift,                       TrackPitch::Up25,             TrackPitch::Up25,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::PoweredLift
        {TrackGroup::halfLoopLarge,                      TrackPitch::None,             TrackPitch::Up25,           TrackRoll::UpsideDown,  TrackRoll::None,        64  }, // TrackElemType::LeftLargeHalfLoopUp
        {TrackGroup::halfLoopLarge,                      TrackPitch::None,             TrackPitch::Up25,           TrackRoll::UpsideDown,  TrackRoll::None,        64  }, // TrackElemType::RightLargeHalfLoopUp
        {TrackGroup::halfLoopLarge,                      TrackPitch::Down25,           TrackPitch::None,           TrackRoll::None,        TrackRoll::UpsideDown,  -64 }, // TrackElemType::LeftLargeHalfLoopDown
        {TrackGroup::halfLoopLarge,                      TrackPitch::Down25,           TrackPitch::None,           TrackRoll::None,        TrackRoll::UpsideDown,  -64 }, // TrackElemType::RightLargeHalfLoopDown
        {TrackGroup::inlineTwistUninverted,              TrackPitch::None,             TrackPitch::None,           TrackRoll::UpsideDown,  TrackRoll::None,        0   }, // TrackElemType::LeftFlyerTwistUp
        {TrackGroup::inlineTwistUninverted,              TrackPitch::None,             TrackPitch::None,           TrackRoll::UpsideDown,  TrackRoll::None,        0   }, // TrackElemType::RightFlyerTwistUp
        {TrackGroup::inlineTwistInverted,                TrackPitch::None,             TrackPitch::None,           TrackRoll::UpsideDown,  TrackRoll::None,        0   }, // TrackElemType::LeftFlyerTwistDown
        {TrackGroup::inlineTwistInverted,                TrackPitch::None,             TrackPitch::None,           TrackRoll::UpsideDown,  TrackRoll::None,        0   }, // TrackElemType::RightFlyerTwistDown
        {TrackGroup::flyingHalfLoopUninvertedUp,         TrackPitch::None,             TrackPitch::Up25,           TrackRoll::UpsideDown,  TrackRoll::None,        64  }, // TrackElemType::FlyerHalfLoopUninvertedUp
        {TrackGroup::flyingHalfLoopInvertedDown,         TrackPitch::Down25,           TrackPitch::None,           TrackRoll::UpsideDown,  TrackRoll::None,        -64 }, // TrackElemType::FlyerHalfLoopInvertedDown
        {TrackGroup::corkscrewUninverted,                TrackPitch::None,             TrackPitch::None,           TrackRoll::UpsideDown,  TrackRoll::None,        0   }, // TrackElemType::LeftFlyerCorkscrewUp
        {TrackGroup::corkscrewUninverted,                TrackPitch::None,             TrackPitch::None,           TrackRoll::UpsideDown,  TrackRoll::None,        0   }, // TrackElemType::RightFlyerCorkscrewUp
        {TrackGroup::corkscrewInverted,                  TrackPitch::None,             TrackPitch::None,           TrackRoll::UpsideDown,  TrackRoll::None,        -32 }, // TrackElemType::LeftFlyerCorkscrewDown
        {TrackGroup::corkscrewInverted,                  TrackPitch::None,             TrackPitch::None,           TrackRoll::UpsideDown,  TrackRoll::None,        -32 }, // TrackElemType::RightFlyerCorkscrewDown
        {TrackGroup::heartlineTransfer,                  TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::HeartLineTransferUp
        {TrackGroup::heartlineTransfer,                  TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::HeartLineTransferDown
        {TrackGroup::heartlineRoll,                      TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::LeftHeartLineRoll
        {TrackGroup::heartlineRoll,                      TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::RightHeartLineRoll
        {TrackGroup::miniGolfHole,                       TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::MinigolfHoleA
        {TrackGroup::miniGolfHole,                       TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::MinigolfHoleB
        {TrackGroup::miniGolfHole,                       TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::MinigolfHoleC
        {TrackGroup::miniGolfHole,                       TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::MinigolfHoleD
        {TrackGroup::miniGolfHole,                       TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::MinigolfHoleE
        { TrackGroup::quarterLoopInvertedDown,           TrackPitch::Down90,           TrackPitch::None,           TrackRoll::UpsideDown,  TrackRoll::None,        0   }, // TrackElemType::MultiDimInvertedFlatToDown90QuarterLoop
        { TrackGroup::quarterLoop,                       TrackPitch::None,             TrackPitch::Up90,           TrackRoll::UpsideDown,  TrackRoll::None,        0   }, // TrackElemType::Up90ToInvertedFlatQuarterLoop
        { TrackGroup::quarterLoop,                       TrackPitch::Down90,           TrackPitch::None,           TrackRoll::None,        TrackRoll::UpsideDown,  0   }, // TrackElemType::InvertedFlatToDown90QuarterLoop
        { TrackGroup::liftHillCurved,                    TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::LeftCurvedLiftHill
        { TrackGroup::liftHillCurved,                    TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::RightCurvedLiftHill
        { TrackGroup::reverser,                          TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::LeftReverser
        { TrackGroup::reverser,                          TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::RightReverser
        { TrackGroup::slopeToFlat,                       TrackPitch::Down90,           TrackPitch::Up90,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::AirThrustTopCap
        { TrackGroup::slopeToFlat,                       TrackPitch::Down90,           TrackPitch::Down90,         TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::AirThrustVerticalDown
        { TrackGroup::slopeToFlat,                       TrackPitch::None,             TrackPitch::Down90,         TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::AirThrustVerticalDownToLevel
        { TrackGroup::blockBrakes,                       TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::BlockBrakes
        { TrackGroup::slopeCurve,                        TrackPitch::Up25,             TrackPitch::Up25,           TrackRoll::Left,        TrackRoll::Left,        0   }, // TrackElemType::LeftBankedQuarterTurn3TileUp25
        { TrackGroup::slopeCurve,                        TrackPitch::Up25,             TrackPitch::Up25,           TrackRoll::Right,       TrackRoll::Right,       0   }, // TrackElemType::RightBankedQuarterTurn3TileUp25
        { TrackGroup::slopeCurve,                        TrackPitch::Down25,           TrackPitch::Down25,         TrackRoll::Left,        TrackRoll::Left,        0   }, // TrackElemType::LeftBankedQuarterTurn3TileDown25
        { TrackGroup::slopeCurve,                        TrackPitch::Down25,           TrackPitch::Down25,         TrackRoll::Right,       TrackRoll::Right,       0   }, // TrackElemType::RightBankedQuarterTurn3TileDown25
        { TrackGroup::slopeCurve,                        TrackPitch::Up25,             TrackPitch::Up25,           TrackRoll::Left,        TrackRoll::Left,        0   }, // TrackElemType::LeftBankedQuarterTurn5TileUp25
        { TrackGroup::slopeCurve,                        TrackPitch::Up25,             TrackPitch::Up25,           TrackRoll::Right,       TrackRoll::Right,       0   }, // TrackElemType::RightBankedQuarterTurn5TileUp25
        { TrackGroup::slopeCurve,                        TrackPitch::Down25,           TrackPitch::Down25,         TrackRoll::Left,        TrackRoll::Left,        0   }, // TrackElemType::LeftBankedQuarterTurn5TileDown25
        { TrackGroup::slopeCurve,                        TrackPitch::Down25,           TrackPitch::Down25,         TrackRoll::Right,       TrackRoll::Right,       0   }, // TrackElemType::RightBankedQuarterTurn5TileDown25
        { TrackGroup::slopeRollBanking,                  TrackPitch::Up25,             TrackPitch::Up25,           TrackRoll::Left,        TrackRoll::None,        0   }, // TrackElemType::Up25ToLeftBankedUp25
        { TrackGroup::slopeRollBanking,                  TrackPitch::Up25,             TrackPitch::Up25,           TrackRoll::Right,       TrackRoll::None,        0   }, // TrackElemType::Up25ToRightBankedUp25
        { TrackGroup::slopeRollBanking,                  TrackPitch::Up25,             TrackPitch::Up25,           TrackRoll::None,        TrackRoll::Left,        0   }, // TrackElemType::LeftBankedUp25ToUp25
        { TrackGroup::slopeRollBanking,                  TrackPitch::Up25,             TrackPitch::Up25,           TrackRoll::None,        TrackRoll::Right,       0   }, // TrackElemType::RightBankedUp25ToUp25
        { TrackGroup::slopeRollBanking,                  TrackPitch::Down25,           TrackPitch::Down25,         TrackRoll::Left,        TrackRoll::None,        0   }, // TrackElemType::Down25ToLeftBankedDown25
        { TrackGroup::slopeRollBanking,                  TrackPitch::Down25,           TrackPitch::Down25,         TrackRoll::Right,       TrackRoll::None,        0   }, // TrackElemType::Down25ToRightBankedDown25
        { TrackGroup::slopeRollBanking,                  TrackPitch::Down25,           TrackPitch::Down25,         TrackRoll::None,        TrackRoll::Left,        0   }, // TrackElemType::LeftBankedDown25ToDown25
        { TrackGroup::slopeRollBanking,                  TrackPitch::Down25,           TrackPitch::Down25,         TrackRoll::None,        TrackRoll::Right,       0   }, // TrackElemType::RightBankedDown25ToDown25
        { TrackGroup::slopeRollBanking,                  TrackPitch::Up25,             TrackPitch::None,           TrackRoll::Left,        TrackRoll::Left,        0   }, // TrackElemType::LeftBankedFlatToLeftBankedUp25
        { TrackGroup::slopeRollBanking,                  TrackPitch::Up25,             TrackPitch::None,           TrackRoll::Right,       TrackRoll::Right,       0   }, // TrackElemType::RightBankedFlatToRightBankedUp25
        { TrackGroup::slopeRollBanking,                  TrackPitch::None,             TrackPitch::Up25,           TrackRoll::Left,        TrackRoll::Left,        0   }, // TrackElemType::LeftBankedUp25ToLeftBankedFlat
        { TrackGroup::slopeRollBanking,                  TrackPitch::None,             TrackPitch::Up25,           TrackRoll::Right,       TrackRoll::Right,       0   }, // TrackElemType::RightBankedUp25ToRightBankedFlat
        { TrackGroup::slopeRollBanking,                  TrackPitch::Down25,           TrackPitch::None,           TrackRoll::Left,        TrackRoll::Left,        0   }, // TrackElemType::LeftBankedFlatToLeftBankedDown25
        { TrackGroup::slopeRollBanking,                  TrackPitch::Down25,           TrackPitch::None,           TrackRoll::Right,       TrackRoll::Right,       0   }, // TrackElemType::RightBankedFlatToRightBankedDown25
        { TrackGroup::slopeRollBanking,                  TrackPitch::None,             TrackPitch::Down25,         TrackRoll::Left,        TrackRoll::Left,        0   }, // TrackElemType::LeftBankedDown25ToLeftBankedFlat
        { TrackGroup::slopeRollBanking,                  TrackPitch::None,             TrackPitch::Down25,         TrackRoll::Right,       TrackRoll::Right,       0   }, // TrackElemType::RightBankedDown25ToRightBankedFlat
        { TrackGroup::slopeRollBanking,                  TrackPitch::Up25,             TrackPitch::None,           TrackRoll::Left,        TrackRoll::None,        0   }, // TrackElemType::FlatToLeftBankedUp25
        { TrackGroup::slopeRollBanking,                  TrackPitch::Up25,             TrackPitch::None,           TrackRoll::Right,       TrackRoll::None,        0   }, // TrackElemType::FlatToRightBankedUp25
        { TrackGroup::slopeRollBanking,                  TrackPitch::None,             TrackPitch::Up25,           TrackRoll::None,        TrackRoll::Left,        0   }, // TrackElemType::LeftBankedUp25ToFlat
        { TrackGroup::slopeRollBanking,                  TrackPitch::None,             TrackPitch::Up25,           TrackRoll::None,        TrackRoll::Right,       0   }, // TrackElemType::RightBankedUp25ToFlat
        { TrackGroup::slopeRollBanking,                  TrackPitch::Down25,           TrackPitch::None,           TrackRoll::Left,        TrackRoll::None,        0   }, // TrackElemType::FlatToLeftBankedDown25
        { TrackGroup::slopeRollBanking,                  TrackPitch::Down25,           TrackPitch::None,           TrackRoll::Right,       TrackRoll::None,        0   }, // TrackElemType::FlatToRightBankedDown25
        { TrackGroup::slopeRollBanking,                  TrackPitch::None,             TrackPitch::Down25,         TrackRoll::None,        TrackRoll::Left,        0   }, // TrackElemType::LeftBankedDown25ToFlat
        { TrackGroup::slopeRollBanking,                  TrackPitch::None,             TrackPitch::Down25,         TrackRoll::None,        TrackRoll::Right,       0   }, // TrackElemType::RightBankedDown25ToFlat
        { TrackGroup::slopeCurveSteep,                   TrackPitch::Up90,             TrackPitch::Up90,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::LeftQuarterTurn1TileUp90
        { TrackGroup::slopeCurveSteep,                   TrackPitch::Up90,             TrackPitch::Up90,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::RightQuarterTurn1TileUp90
        { TrackGroup::slopeCurveSteep,                   TrackPitch::Down90,           TrackPitch::Down90,         TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::LeftQuarterTurn1TileDown90
        { TrackGroup::slopeCurveSteep,                   TrackPitch::Down90,           TrackPitch::Down90,         TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::RightQuarterTurn1TileDown90
        { TrackGroup::quarterLoopUninvertedUp,           TrackPitch::None,             TrackPitch::Up90,           TrackRoll::UpsideDown,  TrackRoll::None,        0   }, // TrackElemType::MultiDimUp90ToInvertedFlatQuarterLoop
        { TrackGroup::quarterLoopUninvertedDown,         TrackPitch::Down90,           TrackPitch::None,           TrackRoll::UpsideDown,  TrackRoll::None,        0   }, // TrackElemType::MultiDimFlatToDown90QuarterLoop
        { TrackGroup::quarterLoopInvertedUp,             TrackPitch::None,             TrackPitch::Up90,           TrackRoll::UpsideDown,  TrackRoll::None,        0   }, // TrackElemType::MultiDimInvertedUp90ToFlatQuarterLoop
        { TrackGroup::rotationControlToggle,             TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::RotationControlToggle
        { TrackGroup::flatRideBase,                      TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::FlatTrack1x4A
        { TrackGroup::flatRideBase,                      TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::FlatTrack2x2
        { TrackGroup::flatRideBase,                      TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::FlatTrack4x4
        { TrackGroup::flatRideBase,                      TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::FlatTrack2x4
        { TrackGroup::flatRideBase,                      TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::FlatTrack1x5
        { TrackGroup::flatRideBase,                      TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::FlatTrack1x1A
        { TrackGroup::flatRideBase,                      TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::FlatTrack1x4B
        { TrackGroup::flatRideBase,                      TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::FlatTrack1x1B
        { TrackGroup::flatRideBase,                      TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        -40 }, // TrackElemType::FlatTrack1x4C
        { TrackGroup::flatRideBase,                      TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        0   }, // TrackElemType::FlatTrack3x3
        { TrackGroup::corkscrewLarge,                    TrackPitch::None,             TrackPitch::None,           TrackRoll::UpsideDown,  TrackRoll::None,        32  }, // TrackElemType::LeftLargeCorkscrewUp
        { TrackGroup::corkscrewLarge,                    TrackPitch::None,             TrackPitch::None,           TrackRoll::UpsideDown,  TrackRoll::None,        32  }, // TrackElemType::RightLargeCorkscrewUp
        { TrackGroup::corkscrewLarge,                    TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::UpsideDown,  -32 }, // TrackElemType::LeftLargeCorkscrewDown
        { TrackGroup::corkscrewLarge,                    TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::UpsideDown,  -32 }, // TrackElemType::RightLargeCorkscrewDown
        { TrackGroup::halfLoopMedium,                    TrackPitch::None,             TrackPitch::Up25,           TrackRoll::UpsideDown,  TrackRoll::None,        64  }, // TrackElemType::LeftMediumHalfLoopUp
        { TrackGroup::halfLoopMedium,                    TrackPitch::None,             TrackPitch::Up25,           TrackRoll::UpsideDown,  TrackRoll::None,        64  }, // TrackElemType::RightMediumHalfLoopUp
        { TrackGroup::halfLoopMedium,                    TrackPitch::Down25,           TrackPitch::None,           TrackRoll::None,        TrackRoll::UpsideDown,  -64 }, // TrackElemType::LeftMediumHalfLoopDown
        { TrackGroup::halfLoopMedium,                    TrackPitch::Down25,           TrackPitch::None,           TrackRoll::None,        TrackRoll::UpsideDown,  -64 }, // TrackElemType::RightMediumHalfLoopDown
        { TrackGroup::zeroGRoll,                         TrackPitch::None,             TrackPitch::Up25,           TrackRoll::UpsideDown,  TrackRoll::None,        0   }, // TrackElemType::LeftZeroGRollUp
        { TrackGroup::zeroGRoll,                         TrackPitch::None,             TrackPitch::Up25,           TrackRoll::UpsideDown,  TrackRoll::None,        0   }, // TrackElemType::RightZeroGRollUp
        { TrackGroup::zeroGRoll,                         TrackPitch::Down25,           TrackPitch::None,           TrackRoll::None,        TrackRoll::UpsideDown,  0   }, // TrackElemType::LeftZeroGRollDown
        { TrackGroup::zeroGRoll,                         TrackPitch::Down25,           TrackPitch::None,           TrackRoll::None,        TrackRoll::UpsideDown,  0   }, // TrackElemType::RightZeroGRollDown
        { TrackGroup::zeroGRollLarge,                    TrackPitch::None,             TrackPitch::Up60,           TrackRoll::UpsideDown,  TrackRoll::None,        64  }, // TrackElemType::LeftLargeZeroGRollUp
        { TrackGroup::zeroGRollLarge,                    TrackPitch::None,             TrackPitch::Up60,           TrackRoll::UpsideDown,  TrackRoll::None,        64  }, // TrackElemType::RightLargeZeroGRollUp
        { TrackGroup::zeroGRollLarge,                    TrackPitch::Down60,           TrackPitch::None,           TrackRoll::None,        TrackRoll::UpsideDown,  -64 }, // TrackElemType::LeftLargeZeroGRollDown
        { TrackGroup::zeroGRollLarge,                    TrackPitch::Down60,           TrackPitch::None,           TrackRoll::None,        TrackRoll::UpsideDown,  -64 }, // TrackElemType::RightLargeZeroGRollDown
        { TrackGroup::flyingLargeHalfLoopUninvertedUp,   TrackPitch::None,             TrackPitch::Up25,           TrackRoll::UpsideDown,  TrackRoll::None,        64  }, // TrackElemType::LeftFlyerLargeHalfLoopUninvertedUp
        { TrackGroup::flyingLargeHalfLoopUninvertedUp,   TrackPitch::None,             TrackPitch::Up25,           TrackRoll::UpsideDown,  TrackRoll::None,        64  }, // TrackElemType::RightFlyerLargeHalfLoopUninvertedUp
        { TrackGroup::flyingLargeHalfLoopInvertedDown,   TrackPitch::Down25,           TrackPitch::None,           TrackRoll::UpsideDown,  TrackRoll::None,        -64 }, // TrackElemType::LeftFlyerLargeHalfLoopInvertedDown
        { TrackGroup::flyingLargeHalfLoopInvertedDown,   TrackPitch::Down25,           TrackPitch::None,           TrackRoll::UpsideDown,  TrackRoll::None,        -64 }, // TrackElemType::RightFlyerLargeHalfLoopInvertedDown
        { TrackGroup::flyingLargeHalfLoopInvertedUp,     TrackPitch::None,             TrackPitch::Up25,           TrackRoll::UpsideDown,  TrackRoll::None,        64  }, // TrackElemType::LeftFlyerLargeHalfLoopInvertedUp
        { TrackGroup::flyingLargeHalfLoopInvertedUp,     TrackPitch::None,             TrackPitch::Up25,           TrackRoll::UpsideDown,  TrackRoll::None,        64  }, // TrackElemType::RightFlyerLargeHalfLoopInvertedUp
        { TrackGroup::flyingLargeHalfLoopUninvertedDown, TrackPitch::Down25,           TrackPitch::None,           TrackRoll::UpsideDown,  TrackRoll::None,       -64  }, // TrackElemType::LeftFlyerLargeHalfLoopUninvertedDown
        { TrackGroup::flyingLargeHalfLoopUninvertedDown, TrackPitch::Down25,           TrackPitch::None,           TrackRoll::UpsideDown,  TrackRoll::None,       -64  }, // TrackElemType::RightFlyerLargeHalfLoopUninvertedDown
        { TrackGroup::flyingHalfLoopInvertedUp,          TrackPitch::None,             TrackPitch::Up25,           TrackRoll::UpsideDown,  TrackRoll::None,        64  }, // TrackElemType::FlyerHalfLoopInvertedUp
        { TrackGroup::flyingHalfLoopUninvertedDown,      TrackPitch::Down25,           TrackPitch::None,           TrackRoll::UpsideDown,  TrackRoll::None,       -64  }, // TrackElemType::FlyerHalfLoopUninvertedDown
        { TrackGroup::slopeCurveLarge,                   TrackPitch::Up25,             TrackPitch::Up25,           TrackRoll::None,        TrackRoll::None,        16  }, // TrackElemType::LeftEighthToDiagUp25
        { TrackGroup::slopeCurveLarge,                   TrackPitch::Up25,             TrackPitch::Up25,           TrackRoll::None,        TrackRoll::None,        16  }, // TrackElemType::RightEighthToDiagUp25
        { TrackGroup::slopeCurveLarge,                   TrackPitch::Down25,           TrackPitch::Down25,         TrackRoll::None,        TrackRoll::None,       -16  }, // TrackElemType::LeftEighthToDiagDown25
        { TrackGroup::slopeCurveLarge,                   TrackPitch::Down25,           TrackPitch::Down25,         TrackRoll::None,        TrackRoll::None,       -16  }, // TrackElemType::RightEighthToDiagDown25
        { TrackGroup::slopeCurveLarge,                   TrackPitch::Up25,             TrackPitch::Up25,           TrackRoll::None,        TrackRoll::None,        16  }, // TrackElemType::LeftEighthToOrthogonalUp25
        { TrackGroup::slopeCurveLarge,                   TrackPitch::Up25,             TrackPitch::Up25,           TrackRoll::None,        TrackRoll::None,        16  }, // TrackElemType::RightEighthToOrthogonalUp25
        { TrackGroup::slopeCurveLarge,                   TrackPitch::Down25,           TrackPitch::Down25,         TrackRoll::None,        TrackRoll::None,       -16  }, // TrackElemType::LeftEighthToOrthogonalDown25
        { TrackGroup::slopeCurveLarge,                   TrackPitch::Down25,           TrackPitch::Down25,         TrackRoll::None,        TrackRoll::None,       -16  }, // TrackElemType::RightEighthToOrthogonalDown25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::Up25,             TrackPitch::Up25,           TrackRoll::Left,        TrackRoll::None,       0    }, // TrackElemType::DiagUp25ToLeftBankedUp25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::Up25,             TrackPitch::Up25,           TrackRoll::Right,       TrackRoll::None,       0    }, // TrackElemType::DiagUp25ToRightBankedUp25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::Up25,             TrackPitch::Up25,           TrackRoll::None,        TrackRoll::Left,       0    }, // TrackElemType::DiagLeftBankedUp25ToUp25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::Up25,             TrackPitch::Up25,           TrackRoll::None,        TrackRoll::Right,      0    }, // TrackElemType::DiagRightBankedUp25ToUp25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::Down25,           TrackPitch::Down25,         TrackRoll::Left,        TrackRoll::None,       0    }, // TrackElemType::DiagDown25ToLeftBankedDown25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::Down25,           TrackPitch::Down25,         TrackRoll::Right,       TrackRoll::None,       0    }, // TrackElemType::DiagDown25ToRightBankedDown25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::Down25,           TrackPitch::Down25,         TrackRoll::None,        TrackRoll::Left,       0    }, // TrackElemType::DiagLeftBankedDown25ToDown25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::Down25,           TrackPitch::Down25,         TrackRoll::None,        TrackRoll::Right,      0    }, // TrackElemType::DiagRightBankedDown25ToDown25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::Up25,             TrackPitch::None,           TrackRoll::Left,        TrackRoll::Left,       0    }, // TrackElemType::DiagLeftBankedFlatToLeftBankedUp25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::Up25,             TrackPitch::None,           TrackRoll::Right,       TrackRoll::Right,      0    }, // TrackElemType::DiagRightBankedFlatToRightBankedUp25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::None,             TrackPitch::Up25,           TrackRoll::Left,        TrackRoll::Left,       0    }, // TrackElemType::DiagLeftBankedUp25ToLeftBankedFlat
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::None,             TrackPitch::Up25,           TrackRoll::Right,       TrackRoll::Right,      0    }, // TrackElemType::DiagRightBankedUp25ToRightBankedFlat
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::Down25,           TrackPitch::None,           TrackRoll::Left,        TrackRoll::Left,       0    }, // TrackElemType::DiagLeftBankedFlatToLeftBankedDown25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::Down25,           TrackPitch::None,           TrackRoll::Right,       TrackRoll::Right,      0    }, // TrackElemType::DiagRightBankedFlatToRightBankedDown25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::None,             TrackPitch::Down25,         TrackRoll::Left,        TrackRoll::Left,       0    }, // TrackElemType::DiagLeftBankedDown25ToLeftBankedFlat
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::None,             TrackPitch::Down25,         TrackRoll::Right,       TrackRoll::Right,      0    }, // TrackElemType::DiagRightBankedDown25ToRightBankedFlat
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::Up25,             TrackPitch::None,           TrackRoll::Left,        TrackRoll::None,       0    }, // TrackElemType::DiagFlatToLeftBankedUp25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::Up25,             TrackPitch::None,           TrackRoll::Right,       TrackRoll::None,       0    }, // TrackElemType::DiagFlatToRightBankedUp25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::None,             TrackPitch::Up25,           TrackRoll::None,        TrackRoll::Left,       0    }, // TrackElemType::DiagLeftBankedUp25ToFlat
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::None,             TrackPitch::Up25,           TrackRoll::None,        TrackRoll::Right,      0    }, // TrackElemType::DiagRightBankedUp25ToFlat
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::Down25,           TrackPitch::None,           TrackRoll::Left,        TrackRoll::None,       0    }, // TrackElemType::DiagFlatToLeftBankedDown25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::Down25,           TrackPitch::None,           TrackRoll::Right,       TrackRoll::None,       0    }, // TrackElemType::DiagFlatToRightBankedDown25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::None,             TrackPitch::Down25,         TrackRoll::None,        TrackRoll::Left,       0    }, // TrackElemType::DiagLeftBankedDown25ToFlat
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::None,             TrackPitch::Down25,         TrackRoll::None,        TrackRoll::Right,      0    }, // TrackElemType::DiagRightBankedDown25ToFlat
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::Up25,             TrackPitch::Up25,           TrackRoll::Left,        TrackRoll::Left,       0    }, // TrackElemType::Up25LeftBanked
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::Up25,             TrackPitch::Up25,           TrackRoll::Right,       TrackRoll::Right,      0    }, // TrackElemType::Up25RightBanked 
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::Down25,           TrackPitch::Down25,         TrackRoll::Left,        TrackRoll::Left,       0    }, // TrackElemType::Down25LeftBanked
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::Down25,           TrackPitch::Down25,         TrackRoll::Right,       TrackRoll::Right,      0    }, // TrackElemType::Down25RightBanked
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::Up25,             TrackPitch::Up25,           TrackRoll::Left,        TrackRoll::Left,      16    }, // TrackElemType::LeftEighthBankToDiagUp25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::Up25,             TrackPitch::Up25,           TrackRoll::Right,       TrackRoll::Right,     16    }, // TrackElemType::RightEighthBankToDiagUp25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::Down25,           TrackPitch::Down25,         TrackRoll::Left,        TrackRoll::Left,     -16    }, // TrackElemType::LeftEighthBankToDiagDown25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::Down25,           TrackPitch::Down25,         TrackRoll::Right,       TrackRoll::Right,    -16    }, // TrackElemType::RightEighthBankToDiagDown25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::Up25,             TrackPitch::Up25,           TrackRoll::Left,        TrackRoll::Left,      16    }, // TrackElemType::LeftEighthBankToOrthogonalUp25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::Up25,             TrackPitch::Up25,           TrackRoll::Right,       TrackRoll::Right,     16    }, // TrackElemType::RightEighthBankToOrthogonalUp25 
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::Down25,           TrackPitch::Down25,         TrackRoll::Left,        TrackRoll::Left,     -16    }, // TrackElemType::LeftEighthBankToOrthogonalDown25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::Down25,           TrackPitch::Down25,         TrackRoll::Right,       TrackRoll::Right,    -16    }, // TrackElemType::RightEighthBankToOrthogonalDown25
        { TrackGroup::diagBrakes,                        TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,       0    }, // TrackElemType::DiagBrakes
        { TrackGroup::diagBlockBrakes,                   TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,       0    }, // TrackElemType::DiagBlockBrakes
        { TrackGroup::inclinedBrakes,                    TrackPitch::Down25,           TrackPitch::Down25,         TrackRoll::None,        TrackRoll::None,       0    }, // TrackElemType::Down25Brakes
        { TrackGroup::diagBooster,                       TrackPitch::None,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,       0    }, // TrackElemType::DiagBooster
        { TrackGroup::slopeSteepLong,                    TrackPitch::Up60,             TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        16   }, // TrackElemType::DiagFlatToUp60LongBase,
        { TrackGroup::slopeSteepLong,                    TrackPitch::None,             TrackPitch::Up60,           TrackRoll::None,        TrackRoll::None,        48   }, // TrackElemType::DiagUp60ToFlatLongBase,
        { TrackGroup::slopeSteepLong,                    TrackPitch::Down60,           TrackPitch::None,           TrackRoll::None,        TrackRoll::None,        -32   }, // TrackElemType::DiagFlatToDown60LongBase,
        { TrackGroup::slopeSteepLong,                    TrackPitch::None,             TrackPitch::Down60,         TrackRoll::None,        TrackRoll::None,        -24   }, // TrackElemType::DiagDown60ToFlatLongBase,
        { TrackGroup::diveLoop,                          TrackPitch::None,             TrackPitch::Up60,           TrackRoll::UpsideDown,  TrackRoll::None,        56  }, // TrackElemType::LeftEighthDiveLoopUpToOrthogonal   
        { TrackGroup::diveLoop,                          TrackPitch::None,             TrackPitch::Up60,           TrackRoll::UpsideDown,  TrackRoll::None,        56  }, // TrackElemType::RightEighthDiveLoopUpToOrthogonal  
        { TrackGroup::diveLoop,                          TrackPitch::Down60,           TrackPitch::None,           TrackRoll::None,        TrackRoll::UpsideDown,  -56 }, // TrackElemType::LeftEighthDiveLoopDownToDiag 
        { TrackGroup::diveLoop,                          TrackPitch::Down60,           TrackPitch::None,           TrackRoll::None,        TrackRoll::UpsideDown,  -56 }, // TrackElemType::RightEighthDiveLoopDownToDiag
    };
    static_assert(std::size(kTrackDefinitions) == EnumValue(TrackElemType::Count));

    // clang-format on

    constexpr static SpinFunction kTrackTypeToSpinFunction[] = {
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::L8,   SpinFunction::R8,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::L8,   SpinFunction::R8,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::L8,   SpinFunction::R8,
        SpinFunction::L8,   SpinFunction::R8,   SpinFunction::LR,   SpinFunction::RL,   SpinFunction::None, SpinFunction::None,
        SpinFunction::L7,   SpinFunction::R7,   SpinFunction::L7,   SpinFunction::R7,   SpinFunction::L7,   SpinFunction::R7,
        SpinFunction::L7,   SpinFunction::R7,   SpinFunction::L5,   SpinFunction::R5,   SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::L8,   SpinFunction::R8,   SpinFunction::LR,
        SpinFunction::RL,   SpinFunction::L7,   SpinFunction::R7,   SpinFunction::L7,   SpinFunction::R7,   SpinFunction::L7,
        SpinFunction::R7,   SpinFunction::L8,   SpinFunction::R8,   SpinFunction::L8,   SpinFunction::R8,   SpinFunction::L5,
        SpinFunction::R5,   SpinFunction::L5,   SpinFunction::R5,   SpinFunction::None, SpinFunction::RC,   SpinFunction::None,
        SpinFunction::L8,   SpinFunction::R8,   SpinFunction::L8,   SpinFunction::R8,   SpinFunction::L8,   SpinFunction::R8,
        SpinFunction::L8,   SpinFunction::R8,   SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::SP,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::R5,   SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::L9,   SpinFunction::R9,   SpinFunction::L9,   SpinFunction::R9,   SpinFunction::L9,
        SpinFunction::R9,   SpinFunction::L9,   SpinFunction::R9,   SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::L7,   SpinFunction::R7,
        SpinFunction::L7,   SpinFunction::R7,   SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::L7,
        SpinFunction::R7,   SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::L7,   SpinFunction::R7,   SpinFunction::L7,   SpinFunction::R7,   SpinFunction::L8,
        SpinFunction::R8,   SpinFunction::L8,   SpinFunction::R8,   SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::L9,
        SpinFunction::R9,   SpinFunction::L9,   SpinFunction::R9,   SpinFunction::L9,   SpinFunction::R9,   SpinFunction::L9,
        SpinFunction::R9,   SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::L9,
        SpinFunction::R9,   SpinFunction::L9,   SpinFunction::R9,   SpinFunction::L9,   SpinFunction::R9,   SpinFunction::L9,
        SpinFunction::R9,   SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::None,
        SpinFunction::None, SpinFunction::None, SpinFunction::None, SpinFunction::L9,   SpinFunction::R9,   SpinFunction::L9,
        SpinFunction::R9
    };
    static_assert(std::size(kTrackTypeToSpinFunction) == EnumValue(TrackElemType::Count));

#pragma region GForces

    template<int32_t TConstant>
    static int32_t EvaluatorConst(const int16_t)
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

    static int32_t EvaluatorDiveLoopUp(const int16_t progress)
    {
        return 385 - 2 * progress;
    }

    static int32_t EvaluatorDiveLoopDown(const int16_t progress)
    {
        return 67 + 2 * progress;
    }

    static int32_t EvaluatorDiveLoopUpLeft(const int16_t progress)
    {
        return 380 - 2 * progress;
    }

    static int32_t EvaluatorDiveLoopUpRight(const int16_t progress)
    {
        return 2 * progress - 380;
    }

    static int32_t EvaluatorDiveLoopDownLeft(const int16_t progress)
    {
        return -(62 + 2 * progress);
    }

    static int32_t EvaluatorDiveLoopDownRight(const int16_t progress)
    {
        return 62 + 2 * progress;
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

    static constexpr TrackComputeFunction GetLateralFunction(TrackElemType type)
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
            case TrackElemType::DiagUp25ToLeftBankedUp25:
            case TrackElemType::DiagUp25ToRightBankedUp25:
            case TrackElemType::DiagLeftBankedUp25ToUp25:
            case TrackElemType::DiagRightBankedUp25ToUp25:
            case TrackElemType::DiagDown25ToLeftBankedDown25:
            case TrackElemType::DiagDown25ToRightBankedDown25:
            case TrackElemType::DiagLeftBankedDown25ToDown25:
            case TrackElemType::DiagRightBankedDown25ToDown25:
            case TrackElemType::DiagLeftBankedFlatToLeftBankedUp25:
            case TrackElemType::DiagRightBankedFlatToRightBankedUp25:
            case TrackElemType::DiagLeftBankedUp25ToLeftBankedFlat:
            case TrackElemType::DiagRightBankedUp25ToRightBankedFlat:
            case TrackElemType::DiagLeftBankedFlatToLeftBankedDown25:
            case TrackElemType::DiagRightBankedFlatToRightBankedDown25:
            case TrackElemType::DiagLeftBankedDown25ToLeftBankedFlat:
            case TrackElemType::DiagRightBankedDown25ToRightBankedFlat:
            case TrackElemType::DiagFlatToLeftBankedUp25:
            case TrackElemType::DiagFlatToRightBankedUp25:
            case TrackElemType::DiagLeftBankedUp25ToFlat:
            case TrackElemType::DiagRightBankedUp25ToFlat:
            case TrackElemType::DiagFlatToLeftBankedDown25:
            case TrackElemType::DiagFlatToRightBankedDown25:
            case TrackElemType::DiagLeftBankedDown25ToFlat:
            case TrackElemType::DiagRightBankedDown25ToFlat:
            case TrackElemType::DiagUp25LeftBanked:
            case TrackElemType::DiagUp25RightBanked:
            case TrackElemType::DiagDown25LeftBanked:
            case TrackElemType::DiagDown25RightBanked:
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
            case TrackElemType::DiagFlatToUp60LongBase:
            case TrackElemType::DiagDown60ToFlatLongBase:
                return EvaluatorConst<0>;
            case TrackElemType::Up60ToFlatLongBase:
            case TrackElemType::FlatToDown60LongBase:
            case TrackElemType::DiagUp60ToFlatLongBase:
            case TrackElemType::DiagFlatToDown60LongBase:
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
            case TrackElemType::LeftEighthToDiagUp25:
            case TrackElemType::LeftEighthToDiagDown25:
            case TrackElemType::LeftEighthToOrthogonalUp25:
            case TrackElemType::LeftEighthToOrthogonalDown25:
                return EvaluatorConst<137>;
            case TrackElemType::RightEighthToDiag:
            case TrackElemType::RightEighthToOrthogonal:
            case TrackElemType::RightEighthToDiagUp25:
            case TrackElemType::RightEighthToDiagDown25:
            case TrackElemType::RightEighthToOrthogonalUp25:
            case TrackElemType::RightEighthToOrthogonalDown25:
                return EvaluatorConst<-137>;
            case TrackElemType::LeftEighthBankToDiag:
            case TrackElemType::LeftEighthBankToOrthogonal:
            case TrackElemType::LeftEighthBankToDiagUp25:
            case TrackElemType::LeftEighthBankToDiagDown25:
            case TrackElemType::LeftEighthBankToOrthogonalUp25:
            case TrackElemType::LeftEighthBankToOrthogonalDown25:
                return EvaluatorConst<200>;
            case TrackElemType::RightEighthBankToDiag:
            case TrackElemType::RightEighthBankToOrthogonal:
            case TrackElemType::RightEighthBankToDiagUp25:
            case TrackElemType::RightEighthBankToDiagDown25:
            case TrackElemType::RightEighthBankToOrthogonalUp25:
            case TrackElemType::RightEighthBankToOrthogonalDown25:
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
            case TrackElemType::LeftEighthDiveLoopUpToOrthogonal:
                return EvaluatorDiveLoopUpLeft;
            case TrackElemType::RightEighthDiveLoopUpToOrthogonal:
                return EvaluatorDiveLoopUpRight;
            case TrackElemType::LeftEighthDiveLoopDownToDiag:
                return EvaluatorDiveLoopDownLeft;
            case TrackElemType::RightEighthDiveLoopDownToDiag:
                return EvaluatorDiveLoopDownRight;
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
            case TrackElemType::LeftLargeHalfLoopDown:
            case TrackElemType::RightLargeHalfLoopDown:
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

    static constexpr TrackComputeFunction GetVerticalFunction(TrackElemType type)
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
            case TrackElemType::LeftEighthToDiagUp25:
            case TrackElemType::RightEighthToDiagUp25:
            case TrackElemType::LeftEighthToDiagDown25:
            case TrackElemType::RightEighthToDiagDown25:
            case TrackElemType::LeftEighthToOrthogonalUp25:
            case TrackElemType::RightEighthToOrthogonalUp25:
            case TrackElemType::LeftEighthToOrthogonalDown25:
            case TrackElemType::RightEighthToOrthogonalDown25:
            case TrackElemType::DiagUp25ToLeftBankedUp25:
            case TrackElemType::DiagUp25ToRightBankedUp25:
            case TrackElemType::DiagLeftBankedUp25ToUp25:
            case TrackElemType::DiagRightBankedUp25ToUp25:
            case TrackElemType::DiagDown25ToLeftBankedDown25:
            case TrackElemType::DiagDown25ToRightBankedDown25:
            case TrackElemType::DiagLeftBankedDown25ToDown25:
            case TrackElemType::DiagRightBankedDown25ToDown25:
            case TrackElemType::DiagUp25LeftBanked:
            case TrackElemType::DiagUp25RightBanked:
            case TrackElemType::DiagDown25LeftBanked:
            case TrackElemType::DiagDown25RightBanked:
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
            case TrackElemType::DiagFlatToUp60LongBase:
            case TrackElemType::DiagDown60ToFlatLongBase:
                return EvaluatorConst<180>;
            case TrackElemType::DiagUp60ToFlatLongBase:
            case TrackElemType::DiagFlatToDown60LongBase:
                return EvaluatorConst<-180>;
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
            case TrackElemType::LeftEighthBankToDiagUp25:
            case TrackElemType::LeftEighthBankToDiagDown25:
            case TrackElemType::LeftEighthBankToOrthogonalUp25:
            case TrackElemType::LeftEighthBankToOrthogonalDown25:
                return EvaluatorConst<270>;
            case TrackElemType::RightEighthBankToDiag:
            case TrackElemType::RightEighthBankToOrthogonal:
            case TrackElemType::RightEighthBankToDiagUp25:
            case TrackElemType::RightEighthBankToDiagDown25:
            case TrackElemType::RightEighthBankToOrthogonalUp25:
            case TrackElemType::RightEighthBankToOrthogonalDown25:
                return EvaluatorConst<270>;
            case TrackElemType::DiagFlatToUp25:
            case TrackElemType::DiagDown25ToFlat:
            case TrackElemType::DiagLeftBankToUp25:
            case TrackElemType::DiagRightBankToUp25:
            case TrackElemType::DiagDown25ToLeftBank:
            case TrackElemType::DiagDown25ToRightBank:
            case TrackElemType::DiagLeftBankedFlatToLeftBankedUp25:
            case TrackElemType::DiagRightBankedFlatToRightBankedUp25:
            case TrackElemType::DiagLeftBankedDown25ToLeftBankedFlat:
            case TrackElemType::DiagRightBankedDown25ToRightBankedFlat:
            case TrackElemType::DiagFlatToLeftBankedUp25:
            case TrackElemType::DiagFlatToRightBankedUp25:
            case TrackElemType::DiagLeftBankedDown25ToFlat:
            case TrackElemType::DiagRightBankedDown25ToFlat:
                return EvaluatorConst<113>;
            case TrackElemType::DiagUp25ToFlat:
            case TrackElemType::DiagFlatToDown25:
            case TrackElemType::DiagUp25ToLeftBank:
            case TrackElemType::DiagUp25ToRightBank:
            case TrackElemType::DiagLeftBankToDown25:
            case TrackElemType::DiagRightBankToDown25:
            case TrackElemType::DiagLeftBankedUp25ToLeftBankedFlat:
            case TrackElemType::DiagRightBankedUp25ToRightBankedFlat:
            case TrackElemType::DiagLeftBankedFlatToLeftBankedDown25:
            case TrackElemType::DiagRightBankedFlatToRightBankedDown25:
            case TrackElemType::DiagLeftBankedUp25ToFlat:
            case TrackElemType::DiagRightBankedUp25ToFlat:
            case TrackElemType::DiagFlatToLeftBankedDown25:
            case TrackElemType::DiagFlatToRightBankedDown25:
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
            case TrackElemType::LeftEighthDiveLoopUpToOrthogonal:
            case TrackElemType::RightEighthDiveLoopUpToOrthogonal:
                return EvaluatorDiveLoopUp;
            case TrackElemType::LeftEighthDiveLoopDownToDiag:
            case TrackElemType::RightEighthDiveLoopDownToDiag:
                return EvaluatorDiveLoopDown;
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
            case TrackElemType::LeftFlyerLargeHalfLoopInvertedUp:
            case TrackElemType::RightFlyerLargeHalfLoopInvertedUp:
                return EvaluatorLargeHalfLoopUp;
            case TrackElemType::LeftLargeHalfLoopDown:
            case TrackElemType::RightLargeHalfLoopDown:
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

#pragma endregion

    static constexpr StringId kRideConfigurationStringIds[] = {
        kStringIdEmpty,                    // 0
        STR_STATION_PLATFORM,              // 1
        kStringIdEmpty,                    // 2
        kStringIdEmpty,                    // 3
        kStringIdEmpty,                    // 4
        kStringIdEmpty,                    // 5
        kStringIdEmpty,                    // 6
        kStringIdEmpty,                    // 7
        kStringIdEmpty,                    // 8
        kStringIdEmpty,                    // 9
        kStringIdEmpty,                    // 10
        kStringIdEmpty,                    // 11
        kStringIdEmpty,                    // 12
        kStringIdEmpty,                    // 13
        kStringIdEmpty,                    // 14
        kStringIdEmpty,                    // 15
        kStringIdEmpty,                    // 16
        kStringIdEmpty,                    // 17
        kStringIdEmpty,                    // 18
        kStringIdEmpty,                    // 19
        kStringIdEmpty,                    // 20
        kStringIdEmpty,                    // 21
        kStringIdEmpty,                    // 22
        kStringIdEmpty,                    // 23
        kStringIdEmpty,                    // 24
        kStringIdEmpty,                    // 25
        kStringIdEmpty,                    // 26
        kStringIdEmpty,                    // 27
        kStringIdEmpty,                    // 28
        kStringIdEmpty,                    // 29
        kStringIdEmpty,                    // 30
        kStringIdEmpty,                    // 31
        kStringIdEmpty,                    // 32
        kStringIdEmpty,                    // 33
        kStringIdEmpty,                    // 34
        kStringIdEmpty,                    // 35
        kStringIdEmpty,                    // 36
        kStringIdEmpty,                    // 37
        STR_S_BEND_LEFT,                   // 38
        STR_S_BEND_RIGHT,                  // 39
        STR_VERTICAL_LOOP_LEFT,            // 40
        STR_VERTICAL_LOOP_RIGHT,           // 41
        kStringIdEmpty,                    // 42
        kStringIdEmpty,                    // 43
        kStringIdEmpty,                    // 44
        kStringIdEmpty,                    // 45
        kStringIdEmpty,                    // 46
        kStringIdEmpty,                    // 47
        kStringIdEmpty,                    // 48
        kStringIdEmpty,                    // 49
        kStringIdEmpty,                    // 50
        kStringIdEmpty,                    // 51
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
        kStringIdEmpty,                    // 62
        kStringIdEmpty,                    // 63
        kStringIdEmpty,                    // 64
        kStringIdEmpty,                    // 65
        STR_ENTRY_EXIT_PLATFORM,           // 66
        STR_VERTICAL_TOWER,                // 67
        kStringIdEmpty,                    // 68
        kStringIdEmpty,                    // 69
        kStringIdEmpty,                    // 70
        kStringIdEmpty,                    // 71
        kStringIdEmpty,                    // 72
        kStringIdEmpty,                    // 73
        kStringIdEmpty,                    // 74
        kStringIdEmpty,                    // 75
        kStringIdEmpty,                    // 76
        kStringIdEmpty,                    // 77
        kStringIdEmpty,                    // 78
        kStringIdEmpty,                    // 79
        kStringIdEmpty,                    // 80
        kStringIdEmpty,                    // 81
        kStringIdEmpty,                    // 82
        STR_S_BEND_LEFT,                   // 83
        STR_S_BEND_RIGHT,                  // 84
        kStringIdEmpty,                    // 85
        kStringIdEmpty,                    // 86
        STR_HELIX_UP_SMALL,                // 87
        STR_HELIX_UP_SMALL,                // 88
        STR_HELIX_DOWN_SMALL,              // 89
        STR_HELIX_DOWN_SMALL,              // 90
        STR_HELIX_UP_LARGE,                // 91
        STR_HELIX_UP_LARGE,                // 92
        STR_HELIX_DOWN_LARGE,              // 93
        STR_HELIX_DOWN_LARGE,              // 94
        kStringIdEmpty,                    // 95
        kStringIdEmpty,                    // 96
        kStringIdEmpty,                    // 97
        kStringIdEmpty,                    // 98
        STR_BRAKES,                        // 99
        STR_BOOSTER,                       // 100
        kStringIdEmpty,                    // 101
        STR_HELIX_UP_LARGE,                // 102
        STR_HELIX_UP_LARGE,                // 103
        STR_HELIX_DOWN_LARGE,              // 104
        STR_HELIX_DOWN_LARGE,              // 105
        STR_HELIX_UP_LEFT,                 // 106
        STR_HELIX_UP_RIGHT,                // 107
        STR_HELIX_DOWN_LEFT,               // 108
        STR_HELIX_DOWN_RIGHT,              // 109
        kStringIdEmpty,                    // TrackElemType::Up25LeftBanked
        kStringIdEmpty,                    // TrackElemType::Up25RighBanked
        STR_WATERFALLS,                    // 112
        STR_RAPIDS,                        // 113
        STR_ON_RIDE_PHOTO_SECTION,         // 114
        kStringIdEmpty,                    // TrackElemType::Down25LeftBanked
        kStringIdEmpty,                    // TrackElemType::Down25RightBanked
        STR_WATER_SPLASH,                  // 117
        kStringIdEmpty,                    // 118
        kStringIdEmpty,                    // 119
        STR_WHIRLPOOL,                     // 120
        kStringIdEmpty,                    // 121
        kStringIdEmpty,                    // 122
        STR_CABLE_LIFT_HILL,               // 123
        STR_SLOPE_UP_TO_VERTICAL,          // 124
        STR_VERTICAL_TRACK,                // 125
        kStringIdEmpty,                    // 126
        kStringIdEmpty,                    // 127
        kStringIdEmpty,                    // 128
        kStringIdEmpty,                    // 129
        kStringIdEmpty,                    // 130
        kStringIdEmpty,                    // 131
        STR_HOLDING_BRAKE_FOR_DROP,        // 132
        kStringIdEmpty,                    // 133
        kStringIdEmpty,                    // 134
        kStringIdEmpty,                    // 135
        kStringIdEmpty,                    // 136
        kStringIdEmpty,                    // 137
        kStringIdEmpty,                    // 138
        kStringIdEmpty,                    // 139
        kStringIdEmpty,                    // 140
        kStringIdEmpty,                    // 141
        kStringIdEmpty,                    // 142
        kStringIdEmpty,                    // 143
        kStringIdEmpty,                    // 144
        kStringIdEmpty,                    // 145
        kStringIdEmpty,                    // 146
        kStringIdEmpty,                    // 147
        kStringIdEmpty,                    // 148
        kStringIdEmpty,                    // 149
        kStringIdEmpty,                    // 150
        kStringIdEmpty,                    // 151
        kStringIdEmpty,                    // 152
        kStringIdEmpty,                    // 153
        kStringIdEmpty,                    // 154
        kStringIdEmpty,                    // 155
        kStringIdEmpty,                    // 156
        kStringIdEmpty,                    // 157
        kStringIdEmpty,                    // 158
        kStringIdEmpty,                    // 159
        kStringIdEmpty,                    // 160
        kStringIdEmpty,                    // 161
        kStringIdEmpty,                    // 162
        kStringIdEmpty,                    // 163
        kStringIdEmpty,                    // 164
        kStringIdEmpty,                    // 165
        kStringIdEmpty,                    // 166
        kStringIdEmpty,                    // 167
        kStringIdEmpty,                    // 168
        kStringIdEmpty,                    // 169
        kStringIdEmpty,                    // 170
        kStringIdEmpty,                    // 171
        STR_REVERSER_TURNTABLE,            // 172
        STR_SPINNING_TUNNEL,               // 173
        STR_BARREL_ROLL_LEFT,              // 174
        STR_BARREL_ROLL_RIGHT,             // 175
        STR_BARREL_ROLL_LEFT,              // 176
        STR_BARREL_ROLL_RIGHT,             // 177
        kStringIdEmpty,                    // 178
        kStringIdEmpty,                    // 179
        kStringIdEmpty,                    // 180
        kStringIdEmpty,                    // 181
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
        kStringIdEmpty,                    // 217
        kStringIdEmpty,                    // 218
        kStringIdEmpty,                    // 219
        kStringIdEmpty,                    // 220
        kStringIdEmpty,                    // 221
        kStringIdEmpty,                    // 222
        kStringIdEmpty,                    // 223
        kStringIdEmpty,                    // 224
        kStringIdEmpty,                    // 225
        kStringIdEmpty,                    // 226
        kStringIdEmpty,                    // 227
        kStringIdEmpty,                    // 228
        kStringIdEmpty,                    // 229
        kStringIdEmpty,                    // 230
        kStringIdEmpty,                    // 231
        kStringIdEmpty,                    // 232
        kStringIdEmpty,                    // 233
        kStringIdEmpty,                    // 234
        kStringIdEmpty,                    // 235
        kStringIdEmpty,                    // 236
        kStringIdEmpty,                    // 237
        kStringIdEmpty,                    // 238
        kStringIdEmpty,                    // 239
        kStringIdEmpty,                    // 240
        kStringIdEmpty,                    // 241
        kStringIdEmpty,                    // 242
        kStringIdEmpty,                    // 243
        kStringIdEmpty,                    // 244
        kStringIdEmpty,                    // 245
        kStringIdEmpty,                    // 246
        kStringIdEmpty,                    // 247
        kStringIdEmpty,                    // 248
        kStringIdEmpty,                    // 249
        kStringIdEmpty,                    // 250
        kStringIdEmpty,                    // 251
        kStringIdEmpty,                    // 252
        STR_QUARTER_LOOP,                  // 253
        STR_QUARTER_LOOP,                  // 254
        STR_QUARTER_LOOP,                  // 255
        STR_SPINNING_CONTROL_TOGGLE_TRACK, // 256
        kStringIdEmpty,                    // TrackElemType::FlatTrack1x4A
        STR_BASE_SIZE_2_X_2,               // TrackElemType::FlatTrack2x2
        STR_BASE_SIZE_4_X_4,               // TrackElemType::FlatTrack4x4
        STR_BASE_SIZE_2_X_4,               // TrackElemType::FlatTrack2x4
        STR_BASE_SIZE_5_X_1,               // TrackElemType::FlatTrack1x5
        kStringIdEmpty,                    // TrackElemType::FlatTrack1x1A
        kStringIdEmpty,                    // TrackElemType::FlatTrack1x4B
        kStringIdEmpty,                    // TrackElemType::FlatTrack1x1B
        kStringIdEmpty,                    // TrackElemType::FlatTrack1x4C
        kStringIdEmpty,                    // TrackElemType::FlatTrack3x3
        STR_LARGE_HALF_CORKSCREW_LEFT,     // TrackElemType::LeftCorkscrewUp
        STR_LARGE_HALF_CORKSCREW_RIGHT,    // TrackElemType::RightCorkscrewUp
        STR_LARGE_HALF_CORKSCREW_LEFT,     // TrackElemType::LeftCorkscrewDown
        STR_LARGE_HALF_CORKSCREW_RIGHT,    // TrackElemType::RightCorkscrewDown
        STR_MEDIUM_HALF_LOOP_LEFT,         // TrackElemType::LeftMediumHalfLoopUp
        STR_MEDIUM_HALF_LOOP_RIGHT,        // TrackElemType::RightMediumHalfLoopUp
        STR_MEDIUM_HALF_LOOP_LEFT,         // TrackElemType::LeftMediumHalfLoopDown
        STR_MEDIUM_HALF_LOOP_RIGHT,        // TrackElemType::RightMediumHalfLoopDown
        STR_ZERO_G_ROLL_LEFT,              // TrackElemType::LeftZeroGRollUp
        STR_ZERO_G_ROLL_RIGHT,             // TrackElemType::RightZeroGRollUp
        STR_ZERO_G_ROLL_LEFT,              // TrackElemType::LeftZeroGRollDown
        STR_ZERO_G_ROLL_RIGHT,             // TrackElemType::RightZeroGRollDown
        STR_LARGE_ZERO_G_ROLL_LEFT,        // TrackElemType::LeftLargeZeroGRollUp
        STR_LARGE_ZERO_G_ROLL_RIGHT,       // TrackElemType::RightLargeZeroGRollUp
        STR_LARGE_ZERO_G_ROLL_LEFT,        // TrackElemType::LeftLargeZeroGRollDown
        STR_LARGE_ZERO_G_ROLL_RIGHT,       // TrackElemType::RightLargeZeroGRollDown
        STR_LARGE_HALF_LOOP_LEFT,          // TrackElemType::LeftFlyerLargeHalfLoopUninvertedUp
        STR_LARGE_HALF_LOOP_RIGHT,         // TrackElemType::RightFlyerLargeHalfLoopUninvertedUp
        STR_LARGE_HALF_LOOP_LEFT,          // TrackElemType::LeftFlyerLargeHalfLoopInvertedDown
        STR_LARGE_HALF_LOOP_RIGHT,         // TrackElemType::RightFlyerLargeHalfLoopInvertedDown
        STR_LARGE_HALF_LOOP_LEFT,          // TrackElemType::LeftFlyerLargeHalfLoopInvertedUp
        STR_LARGE_HALF_LOOP_RIGHT,         // TrackElemType::RightFlyerLargeHalfLoopInvertedUp
        STR_LARGE_HALF_LOOP_LEFT,          // TrackElemType::LeftFlyerLargeHalfLoopUninvertedDown
        STR_LARGE_HALF_LOOP_RIGHT,         // TrackElemType::RightFlyerLargeHalfLoopUninvertedDown
        STR_HALF_LOOP,                     // TrackElemType::FlyerHalfLoopInvertedUp
        STR_HALF_LOOP,                     // TrackElemType::FlyerHalfLoopUninvertedDown
        kStringIdEmpty,                    // TrackElemType::LeftEighthToDiagUp25
        kStringIdEmpty,                    // TrackElemType::RightEighthToDiagUp25
        kStringIdEmpty,                    // TrackElemType::LeftEighthToDiagDown25
        kStringIdEmpty,                    // TrackElemType::RightEighthToDiagDown25
        kStringIdEmpty,                    // TrackElemType::LeftEighthToOrthogonalUp25
        kStringIdEmpty,                    // TrackElemType::RightEighthToOrthogonalUp25
        kStringIdEmpty,                    // TrackElemType::LeftEighthToOrthogonalDown25
        kStringIdEmpty,                    // TrackElemType::RightEighthToOrthogonalDown25
        kStringIdEmpty,                    // TrackElemType::DiagUp25ToLeftBankedUp25
        kStringIdEmpty,                    // TrackElemType::DiagUp25ToRightBankedUp25
        kStringIdEmpty,                    // TrackElemType::DiagLeftBankedUp25ToUp25
        kStringIdEmpty,                    // TrackElemType::DiagRightBankedUp25ToUp25
        kStringIdEmpty,                    // TrackElemType::DiagDown25ToLeftBankedDown25
        kStringIdEmpty,                    // TrackElemType::DiagDown25ToRightBankedDown25
        kStringIdEmpty,                    // TrackElemType::DiagLeftBankedDown25ToDown25
        kStringIdEmpty,                    // TrackElemType::DiagRightBankedDown25ToDown25
        kStringIdEmpty,                    // TrackElemType::DiagLeftBankedFlatToLeftBankedUp25
        kStringIdEmpty,                    // TrackElemType::DiagRightBankedFlatToRightBankedUp25
        kStringIdEmpty,                    // TrackElemType::DiagLeftBankedUp25ToLeftBankedFlat
        kStringIdEmpty,                    // TrackElemType::DiagRightBankedUp25ToRightBankedFlat
        kStringIdEmpty,                    // TrackElemType::DiagLeftBankedFlatToLeftBankedDown25
        kStringIdEmpty,                    // TrackElemType::DiagRightBankedFlatToRightBankedDown25
        kStringIdEmpty,                    // TrackElemType::DiagLeftBankedDown25ToLeftBankedFlat
        kStringIdEmpty,                    // TrackElemType::DiagRightBankedDown25ToRightBankedFlat
        kStringIdEmpty,                    // TrackElemType::DiagFlatToLeftBankedUp25
        kStringIdEmpty,                    // TrackElemType::DiagFlatToRightBankedUp25
        kStringIdEmpty,                    // TrackElemType::DiagLeftBankedUp25ToFlat
        kStringIdEmpty,                    // TrackElemType::DiagRightBankedUp25ToFlat
        kStringIdEmpty,                    // TrackElemType::DiagFlatToLeftBankedDown25
        kStringIdEmpty,                    // TrackElemType::DiagFlatToRightBankedDown25
        kStringIdEmpty,                    // TrackElemType::DiagLeftBankedDown25ToFlat
        kStringIdEmpty,                    // TrackElemType::DiagRightBankedDown25ToFlat
        kStringIdEmpty,                    // TrackElemType::Up25LeftBanked
        kStringIdEmpty,                    // TrackElemType::Up25RightBanked
        kStringIdEmpty,                    // TrackElemType::Down25LeftBanked
        kStringIdEmpty,                    // TrackElemType::Down25RightBanked
        kStringIdEmpty,                    // TrackElemType::LeftEighthBankToDiagUp25
        kStringIdEmpty,                    // TrackElemType::RightEighthBankToDiagUp25
        kStringIdEmpty,                    // TrackElemType::LeftEighthBankToDiagDown25
        kStringIdEmpty,                    // TrackElemType::RightEighthBankToDiagDown25
        kStringIdEmpty,                    // TrackElemType::LeftEighthBankToOrthogonalUp25
        kStringIdEmpty,                    // TrackElemType::RightEighthBankToOrthogonalUp25
        kStringIdEmpty,                    // TrackElemType::LeftEighthBankToOrthogonalDown25
        kStringIdEmpty,                    // TrackElemType::RightEighthBankToOrthogonalDown25
        STR_BRAKES,                        // TrackElemType::DiagBrakes
        STR_BLOCK_BRAKES,                  // TrackElemType::DiagBlockBrakes
        STR_BRAKES,                        // TrackElemType::Down25Brakes
        STR_BOOSTER,                       // TrackElemType::DiagBooster
        kStringIdEmpty,                    // TrackElemType::DiagFlatToUp60LongBase,
        kStringIdEmpty,                    // TrackElemType::DiagUp60ToFlatLongBase,
        kStringIdEmpty,                    // TrackElemType::DiagFlatToDown60LongBase,
        kStringIdEmpty,                    // TrackElemType::DiagDown60ToFlatLongBase,
        STR_DIVE_LOOP_LEFT,                // TrackElemType::LeftEighthDiveLoopUpToOrthogonal
        STR_DIVE_LOOP_RIGHT,               // TrackElemType::RightEighthDiveLoopUpToOrthogonal
        STR_DIVE_LOOP_LEFT,                // TrackElemType::LeftEighthDiveLoopDownToDiag
        STR_DIVE_LOOP_RIGHT,               // TrackElemType::RightEighthDiveLoopDownToDiag
    };
    static_assert(std::size(kRideConfigurationStringIds) == EnumValue(TrackElemType::Count));

#pragma region trackBlocks

    static constexpr SequenceDescriptor kFlatSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kEndStationSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN
            | TRACK_SEQUENCE_FLAG_DISALLOW_DOORS,
        .woodenSupports = { WoodenSupportSubType::NeSw },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kBeginStationSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN
            | TRACK_SEQUENCE_FLAG_DISALLOW_DOORS,
        .woodenSupports = { WoodenSupportSubType::NeSw },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kMiddleStationSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN
            | TRACK_SEQUENCE_FLAG_DISALLOW_DOORS,
        .woodenSupports = { WoodenSupportSubType::NeSw },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up25Deg },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kUp60Seq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up60Deg },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kFlatToUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::FlatToUp25Deg },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kUp25ToUp60Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up25DegToUp60Deg },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kUp60ToUp25Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up60DegToUp25Deg },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kUp25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up25DegToFlat },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Down25Deg },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kDown60Seq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Down60Deg },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kFlatToDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::FlatToDown25Deg },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kDown25ToDown60Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Down25DegToDown60Deg },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kDown60ToDown25Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Down60DegToDown25Deg },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kDown25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Down25DegToFlat },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::Null },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::Corner1 },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesSeq4 = {
        .clearance = { -32, -64, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::Null },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesSeq5 = {
        .clearance = { -64, -32, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesSeq6 = {
        .clearance = { -64, -64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::NwSe },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = -1,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::NeSw },
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Null },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesSeq4 = {
        .clearance = { -32, 64, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Null },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesSeq5 = {
        .clearance = { -64, 32, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesSeq6 = {
        .clearance = { -64, 64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::NwSe },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = 1,
    };

    static constexpr SequenceDescriptor kFlatToLeftBankSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kFlatToRightBankSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kLeftBankToFlatSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kRightBankToFlatSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kBankedLeftQuarterTurn5TilesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kBankedLeftQuarterTurn5TilesSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::Null },
    };

    static constexpr SequenceDescriptor kBankedLeftQuarterTurn5TilesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kBankedLeftQuarterTurn5TilesSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::Corner1 },
    };

    static constexpr SequenceDescriptor kBankedLeftQuarterTurn5TilesSeq4 = {
        .clearance = { -32, -64, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::Null },
    };

    static constexpr SequenceDescriptor kBankedLeftQuarterTurn5TilesSeq5 = {
        .clearance = { -64, -32, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kBankedLeftQuarterTurn5TilesSeq6 = {
        .clearance = { -64, -64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::NwSe },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = -1,
    };

    static constexpr SequenceDescriptor kBankedRightQuarterTurn5TilesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::NeSw },
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kBankedRightQuarterTurn5TilesSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Null },
    };

    static constexpr SequenceDescriptor kBankedRightQuarterTurn5TilesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kBankedRightQuarterTurn5TilesSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kBankedRightQuarterTurn5TilesSeq4 = {
        .clearance = { -32, 64, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Null },
    };

    static constexpr SequenceDescriptor kBankedRightQuarterTurn5TilesSeq5 = {
        .clearance = { -64, 32, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kBankedRightQuarterTurn5TilesSeq6 = {
        .clearance = { -64, 64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::NwSe },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = 1,
    };

    static constexpr SequenceDescriptor kLeftBankToUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::FlatToUp25Deg },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kRightBankToUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::FlatToUp25Deg },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kUp25ToLeftBankSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up25DegToFlat },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kUp25ToRightBankSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up25DegToFlat },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kLeftBankToDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::FlatToDown25Deg },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kRightBankToDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::FlatToDown25Deg },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kDown25ToLeftBankSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Down25DegToFlat },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kDown25ToRightBankSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Down25DegToFlat },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kLeftBankSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kRightBankSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up25Deg },
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesUp25Seq1 = {
        .clearance = { 0, -32, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesUp25Seq2 = {
        .clearance = { -32, 0, 16, 16, { 0b0111, 0b0100 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesUp25Seq3 = {
        .clearance = { -32, -32, 24, 16, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::Corner1 },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesUp25Seq4 = {
        .clearance = { -32, -64, 48, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesUp25Seq5 = {
        .clearance = { -64, -32, 32, 16, { 0b0111, 0b0100 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesUp25Seq6 = {
        .clearance = { -64, -64, 48, 16, { 0b1111, 0b0110 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up25Deg },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = -1,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up25Deg },
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesUp25Seq1 = {
        .clearance = { 0, 32, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesUp25Seq2 = {
        .clearance = { -32, 0, 16, 16, { 0b1011, 0b1000 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesUp25Seq3 = {
        .clearance = { -32, 32, 24, 16, { 0b1110, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesUp25Seq4 = {
        .clearance = { -32, 64, 48, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesUp25Seq5 = {
        .clearance = { -64, 32, 32, 16, { 0b1011, 0b1000 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesUp25Seq6 = {
        .clearance = { -64, 64, 48, 16, { 0b1111, 0b1001 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up25Deg },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = 1,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesDown25Seq0 = {
        .clearance = { 0, 0, 48, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Down25Deg },
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesDown25Seq1 = {
        .clearance = { 0, -32, 48, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesDown25Seq2 = {
        .clearance = { -32, 0, 32, 16, { 0b0111, 0b0001 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesDown25Seq3 = {
        .clearance = { -32, -32, 24, 16, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::Corner1 },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesDown25Seq4 = {
        .clearance = { -32, -64, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesDown25Seq5 = {
        .clearance = { -64, -32, 16, 16, { 0b0111, 0b0001 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesDown25Seq6 = {
        .clearance = { -64, -64, 0, 16, { 0b1111, 0b1001 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up25Deg },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = -1,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesDown25Seq0 = {
        .clearance = { 0, 0, 48, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Down25Deg },
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesDown25Seq1 = {
        .clearance = { 0, 32, 48, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesDown25Seq2 = {
        .clearance = { -32, 0, 32, 16, { 0b1011, 0b0010 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesDown25Seq3 = {
        .clearance = { -32, 32, 24, 16, { 0b1110, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesDown25Seq4 = {
        .clearance = { -32, 64, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesDown25Seq5 = {
        .clearance = { -64, 32, 16, 16, { 0b1011, 0b0010 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesDown25Seq6 = {
        .clearance = { -64, 64, 0, 16, { 0b1111, 0b0110 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up25Deg },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = 1,
    };

    static constexpr SequenceDescriptor kSBendLeftSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kSBendLeftSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kSBendLeftSeq2 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, 0 },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::Corner1 },
    };

    static constexpr SequenceDescriptor kSBendLeftSeq3 = {
        .clearance = { -64, -32, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kSBendRightSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kSBendRightSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kSBendRightSeq2 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kSBendRightSeq3 = {
        .clearance = { -64, 32, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b0010,
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq1 = {
        .clearance = { -32, 0, 16, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b0010,
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq2 = {
        .clearance = { -64, 0, 32, 96, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq3 = {
        .clearance = { -32, 0, 120, 16, { 0b0110, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq4 = {
        .clearance = { -32, -32, 120, 0, { 0b0000, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq5 = {
        .clearance = { 0, 0, 120, 0, { 0b0000, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq6 = {
        .clearance = { 0, -32, 120, 16, { 0b1001, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq7 = {
        .clearance = { 32, -32, 32, 96, { 0b1000, 0 }, 0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq8 = {
        .clearance = { 0, -32, 16, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1000,
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq9 = {
        .clearance = { -32, -32, 0, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1000,
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1000,
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq1 = {
        .clearance = { -32, 0, 16, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1000,
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq2 = {
        .clearance = { -64, 0, 32, 96, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq3 = {
        .clearance = { -32, 0, 120, 16, { 0b1001, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq4 = {
        .clearance = { -32, 32, 120, 0, { 0b0000, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq5 = {
        .clearance = { 0, 0, 120, 0, { 0b0000, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq6 = {
        .clearance = { 0, 32, 120, 16, { 0b0110, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq7 = {
        .clearance = { 32, 32, 32, 96, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq8 = {
        .clearance = { 0, 32, 16, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b0010,
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq9 = {
        .clearance = { -32, 32, 0, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b0010,
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = -1,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = 1,
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TilesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TilesSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TilesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TilesSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = -1,
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TilesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TilesSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TilesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TilesSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = 1,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b0111, 0b0100 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesUp25Seq1 = {
        .clearance = { 0, -32, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesUp25Seq2 = {
        .clearance = { -32, 0, 16, 0, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesUp25Seq3 = {
        .clearance = { -32, -32, 16, 16, { 0b0111, 0b0110 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = -1,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1011, 0b1000 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesUp25Seq1 = {
        .clearance = { 0, 32, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesUp25Seq2 = {
        .clearance = { -32, 0, 16, 0, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesUp25Seq3 = {
        .clearance = { -32, 32, 16, 16, { 0b1011, 0b1001 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = 1,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesDown25Seq0 = {
        .clearance = { 0, 0, 16, 16, { 0b0111, 0b0011 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesDown25Seq1 = {
        .clearance = { 0, -32, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesDown25Seq2 = {
        .clearance = { -32, 0, 16, 0, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesDown25Seq3 = {
        .clearance = { -32, -32, 0, 16, { 0b0111, 0b0001 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = -1,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesDown25Seq0 = {
        .clearance = { 0, 0, 16, 16, { 0b1011, 0b0011 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesDown25Seq1 = {
        .clearance = { 0, 32, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesDown25Seq2 = {
        .clearance = { -32, 0, 16, 0, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesDown25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b1011, 0b0010 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = 1,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn1TileSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn1TileSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kLeftTwistDownToUpSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kLeftTwistDownToUpSeq1 = {
        .clearance = { -32, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kLeftTwistDownToUpSeq2 = {
        .clearance = { -64, 0, 16, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kRightTwistDownToUpSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kRightTwistDownToUpSeq1 = {
        .clearance = { -32, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kRightTwistDownToUpSeq2 = {
        .clearance = { -64, 0, 16, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kLeftTwistUpToDownSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kLeftTwistUpToDownSeq1 = {
        .clearance = { -32, 0, -16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kLeftTwistUpToDownSeq2 = {
        .clearance = { -64, 0, -16, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kRightTwistUpToDownSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kRightTwistUpToDownSeq1 = {
        .clearance = { -32, 0, -16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kRightTwistUpToDownSeq2 = {
        .clearance = { -64, 0, -16, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kHalfLoopUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kHalfLoopUpSeq1 = {
        .clearance = { -32, 0, 16, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kHalfLoopUpSeq2 = {
        .clearance = { -64, 0, 32, 96, { 0b0011, 0 }, 0 },
        .allowedWallEdges = 0b1011,
    };

    static constexpr SequenceDescriptor kHalfLoopUpSeq3 = {
        .clearance = { -32, 0, 120, 16, { 0b1111, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kHalfLoopDownSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kHalfLoopDownSeq1 = {
        .clearance = { -32, 0, -120, 96, { 0b0011, 0 }, 0 },
        .allowedWallEdges = 0b1011,
    };

    static constexpr SequenceDescriptor kHalfLoopDownSeq2 = {
        .clearance = { 0, 0, -136, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kHalfLoopDownSeq3 = {
        .clearance = { 32, 0, -152, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kLeftCorkscrewUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kLeftCorkscrewUpSeq1 = {
        .clearance = { -32, 0, 24, 32, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftCorkscrewUpSeq2 = {
        .clearance = { -32, -32, 48, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kRightCorkscrewUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kRightCorkscrewUpSeq1 = {
        .clearance = { -32, 0, 24, 32, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightCorkscrewUpSeq2 = {
        .clearance = { -32, 32, 48, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kLeftCorkscrewDownSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kLeftCorkscrewDownSeq1 = {
        .clearance = { -32, 0, -56, 32, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftCorkscrewDownSeq2 = {
        .clearance = { -32, -32, -80, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kRightCorkscrewDownSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kRightCorkscrewDownSeq1 = {
        .clearance = { -32, 0, -56, 32, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightCorkscrewDownSeq2 = {
        .clearance = { -32, 32, -80, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kFlatToUp60Seq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::FlatToUp60Deg },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kUp60ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up60DegToFlat },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kFlatToDown60Seq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::FlatToDown60Deg },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kDown60ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Down60DegToFlat },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kTowerBaseSeq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_ORIGIN,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kTowerBaseSeq1 = {
        .clearance = { -32, -32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_3,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kTowerBaseSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kTowerBaseSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kTowerBaseSeq4 = {
        .clearance = { 0, -32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_3,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kTowerBaseSeq5 = {
        .clearance = { 0, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_1,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kTowerBaseSeq6 = {
        .clearance = { 32, -32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kTowerBaseSeq7 = {
        .clearance = { 32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kTowerBaseSeq8 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_1 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_2,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kTowerSectionSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kTowerSectionSeq1 = {
        .clearance = { 32, 0, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_1 },
    };

    static constexpr SequenceDescriptor kFlatCoveredSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kUp25CoveredSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kUp60CoveredSeq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kFlatToUp25CoveredSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kUp25ToUp60CoveredSeq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kUp60ToUp25CoveredSeq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kUp25ToFlatCoveredSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kDown25CoveredSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kDown60CoveredSeq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kFlatToDown25CoveredSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kDown25ToDown60CoveredSeq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kDown60ToDown25CoveredSeq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kDown25ToFlatCoveredSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesCoveredSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesCoveredSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesCoveredSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesCoveredSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesCoveredSeq4 = {
        .clearance = { -32, -64, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesCoveredSeq5 = {
        .clearance = { -64, -32, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesCoveredSeq6 = {
        .clearance = { -64, -64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = -1,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesCoveredSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesCoveredSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesCoveredSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesCoveredSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesCoveredSeq4 = {
        .clearance = { -32, 64, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesCoveredSeq5 = {
        .clearance = { -64, 32, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesCoveredSeq6 = {
        .clearance = { -64, 64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = 1,
    };

    static constexpr SequenceDescriptor kSBendLeftCoveredSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kSBendLeftCoveredSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kSBendLeftCoveredSeq2 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, 0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kSBendLeftCoveredSeq3 = {
        .clearance = { -64, -32, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kSBendRightCoveredSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kSBendRightCoveredSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kSBendRightCoveredSeq2 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, 0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kSBendRightCoveredSeq3 = {
        .clearance = { -64, 32, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesCoveredSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesCoveredSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesCoveredSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesCoveredSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = -1,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesCoveredSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesCoveredSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesCoveredSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesCoveredSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = 1,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::Corner1 },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq2 = {
        .clearance = { -32, 0, 0, 4, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq3 = {
        .clearance = { -32, -32, 0, 4, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = -1,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq4 = {
        .clearance = { -32, -64, 8, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = -1,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq5 = {
        .clearance = { 0, -64, 8, 0, { 0b0100, 0b0000 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq6 = {
        .clearance = { -32, -96, 8, 4, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq7 = {
        .clearance = { 0, -96, 8, 4, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = -2,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpSmallSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpSmallSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0b0000 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpSmallSeq2 = {
        .clearance = { -32, 0, 0, 4, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpSmallSeq3 = {
        .clearance = { -32, 32, 0, 4, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = 1,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpSmallSeq4 = {
        .clearance = { -32, 64, 8, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = 1,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpSmallSeq5 = {
        .clearance = { 0, 64, 8, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::Corner1 },
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpSmallSeq6 = {
        .clearance = { -32, 96, 8, 4, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpSmallSeq7 = {
        .clearance = { 0, 96, 8, 4, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = 2,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownSmallSeq0 = {
        .clearance = { 0, 0, 8, 4, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownSmallSeq1 = {
        .clearance = { 0, -32, 8, 4, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownSmallSeq2 = {
        .clearance = { -32, 0, 8, 0, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownSmallSeq3 = {
        .clearance = { -32, -32, 8, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = -1,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownSmallSeq4 = {
        .clearance = { -32, -64, 0, 4, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = -1,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownSmallSeq5 = {
        .clearance = { 0, -64, 0, 4, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner1 },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownSmallSeq6 = {
        .clearance = { -32, -96, 0, 0, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownSmallSeq7 = {
        .clearance = { 0, -96, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = -2,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownSmallSeq0 = {
        .clearance = { 0, 0, 8, 4, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownSmallSeq1 = {
        .clearance = { 0, 32, 8, 4, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner1 },
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownSmallSeq2 = {
        .clearance = { -32, 0, 8, 0, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownSmallSeq3 = {
        .clearance = { -32, 32, 8, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = 1,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownSmallSeq4 = {
        .clearance = { -32, 64, 0, 4, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = 1,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownSmallSeq5 = {
        .clearance = { 0, 64, 0, 4, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownSmallSeq6 = {
        .clearance = { -32, 96, 0, 0, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownSmallSeq7 = {
        .clearance = { 0, 96, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = 2,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::Corner1 },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq4 = {
        .clearance = { -32, -64, 0, 4, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq5 = {
        .clearance = { -64, -32, 0, 4, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq6 = {
        .clearance = { -64, -64, 0, 4, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::NwSe },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = -1,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq7 = {
        .clearance = { -64, -96, 8, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::NwSe },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = -1,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq8 = {
        .clearance = { -32, -96, 8, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq9 = {
        .clearance = { -64, -128, 8, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq10 = {
        .clearance = { -32, -128, 8, 0, { 0b1110, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq11 = {
        .clearance = { 0, -128, 8, 4, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq12 = {
        .clearance = { -32, -160, 8, 4, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq13 = {
        .clearance = { 0, -160, 8, 4, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::NeSw },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = -2,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::NeSw },
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq4 = {
        .clearance = { -32, 64, 0, 4, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq5 = {
        .clearance = { -64, 32, 0, 4, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq6 = {
        .clearance = { -64, 64, 0, 4, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::NwSe },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = 1,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq7 = {
        .clearance = { -64, 96, 8, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::NwSe },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = 1,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq8 = {
        .clearance = { -32, 96, 8, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq9 = {
        .clearance = { -64, 128, 8, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq10 = {
        .clearance = { -32, 128, 8, 0, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::Corner1 },
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq11 = {
        .clearance = { 0, 128, 8, 4, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq12 = {
        .clearance = { -32, 160, 8, 4, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq13 = {
        .clearance = { 0, 160, 8, 4, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = 2,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq0 = {
        .clearance = { 0, 0, 8, 4, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq1 = {
        .clearance = { 0, -32, 8, 4, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq2 = {
        .clearance = { -32, 0, 8, 4, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq3 = {
        .clearance = { -32, -32, 8, 0, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq4 = {
        .clearance = { -32, -64, 8, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq5 = {
        .clearance = { -64, -32, 8, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq6 = {
        .clearance = { -64, -64, 8, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::NwSe },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = -1,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq7 = {
        .clearance = { -64, -96, 0, 4, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::NwSe },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = -1,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq8 = {
        .clearance = { -32, -96, 0, 4, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq9 = {
        .clearance = { -64, -128, 0, 4, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq10 = {
        .clearance = { -32, -128, 0, 0, { 0b1110, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::Corner1 },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq11 = {
        .clearance = { 0, -128, 0, 0, { 0b0100, 0b0000 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq12 = {
        .clearance = { -32, -160, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq13 = {
        .clearance = { 0, -160, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::NeSw },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = -2,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq0 = {
        .clearance = { 0, 0, 8, 4, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::NeSw },
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq1 = {
        .clearance = { 0, 32, 8, 4, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq2 = {
        .clearance = { -32, 0, 8, 4, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq3 = {
        .clearance = { -32, 32, 8, 0, { 0b1110, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::Corner1 },
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq4 = {
        .clearance = { -32, 64, 8, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq5 = {
        .clearance = { -64, 32, 8, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq6 = {
        .clearance = { -64, 64, 8, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::NwSe },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = 1,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq7 = {
        .clearance = { -64, 96, 0, 4, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::NwSe },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = 1,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq8 = {
        .clearance = { -32, 96, 0, 4, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq9 = {
        .clearance = { -64, 128, 0, 4, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq10 = {
        .clearance = { -32, 128, 0, 0, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq11 = {
        .clearance = { 0, 128, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq12 = {
        .clearance = { -32, 160, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq13 = {
        .clearance = { 0, 160, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = 2,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn1TileUp60Seq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn1TileUp60Seq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn1TileDown60Seq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn1TileDown60Seq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kBrakesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kBoosterSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kMazeSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2
            | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kMazeSeq1 = {
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2
            | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN,
    };

    static constexpr SequenceDescriptor kMazeSeq2 = {
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2
            | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN,
    };

    static constexpr SequenceDescriptor kMazeSeq3 = {
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2
            | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN,
    };

    static constexpr SequenceDescriptor kMazeSeq4 = {
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2
            | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN,
    };

    static constexpr SequenceDescriptor kMazeSeq5 = {
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2
            | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN,
    };

    static constexpr SequenceDescriptor kMazeSeq6 = {
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2
            | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN,
    };

    static constexpr SequenceDescriptor kMazeSeq7 = {
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2
            | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN,
    };

    static constexpr SequenceDescriptor kMazeSeq8 = {
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2
            | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN,
    };

    static constexpr SequenceDescriptor kMazeSeq9 = {
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2
            | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN,
    };

    static constexpr SequenceDescriptor kMazeSeq10 = {
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2
            | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN,
    };

    static constexpr SequenceDescriptor kMazeSeq11 = {
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2
            | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN,
    };

    static constexpr SequenceDescriptor kMazeSeq12 = {
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2
            | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN,
    };

    static constexpr SequenceDescriptor kMazeSeq13 = {
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2
            | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN,
    };

    static constexpr SequenceDescriptor kMazeSeq14 = {
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2
            | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN,
    };

    static constexpr SequenceDescriptor kMazeSeq15 = {
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2
            | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeUpSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeUpSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeUpSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeUpSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeUpSeq4 = {
        .clearance = { -32, -64, 0, 12, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeUpSeq5 = {
        .clearance = { -64, -32, 0, 12, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeUpSeq6 = {
        .clearance = { -64, -64, 0, 12, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = -1,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeUpSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeUpSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeUpSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeUpSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeUpSeq4 = {
        .clearance = { -32, 64, 0, 12, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeUpSeq5 = {
        .clearance = { -64, 32, 0, 12, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeUpSeq6 = {
        .clearance = { -64, 64, 0, 12, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = 1,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeDownSeq0 = {
        .clearance = { 0, 0, 0, 12, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeDownSeq1 = {
        .clearance = { 0, -32, 0, 12, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeDownSeq2 = {
        .clearance = { -32, 0, 0, 12, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeDownSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeDownSeq4 = {
        .clearance = { -32, -64, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeDownSeq5 = {
        .clearance = { -64, -32, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeDownSeq6 = {
        .clearance = { -64, -64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = -1,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeDownSeq0 = {
        .clearance = { 0, 0, 0, 12, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeDownSeq1 = {
        .clearance = { 0, 32, 0, 12, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeDownSeq2 = {
        .clearance = { -32, 0, 0, 12, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeDownSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeDownSeq4 = {
        .clearance = { -32, 64, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeDownSeq5 = {
        .clearance = { -64, 32, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeDownSeq6 = {
        .clearance = { -64, 64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = 1,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeUpSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeUpSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeUpSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeUpSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeUpSeq4 = {
        .clearance = { -32, -64, 0, 12, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeUpSeq5 = {
        .clearance = { -64, -32, 0, 12, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeUpSeq6 = {
        .clearance = { -64, -64, 0, 12, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = -1,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeUpSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeUpSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeUpSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeUpSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeUpSeq4 = {
        .clearance = { -32, 64, 0, 12, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeUpSeq5 = {
        .clearance = { -64, 32, 0, 12, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeUpSeq6 = {
        .clearance = { -64, 64, 0, 12, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = 1,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeDownSeq0 = {
        .clearance = { 0, 0, 0, 12, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeDownSeq1 = {
        .clearance = { 0, -32, 0, 12, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeDownSeq2 = {
        .clearance = { -32, 0, 0, 12, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeDownSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeDownSeq4 = {
        .clearance = { -32, -64, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeDownSeq5 = {
        .clearance = { -64, -32, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeDownSeq6 = {
        .clearance = { -64, -64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = -1,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeDownSeq0 = {
        .clearance = { 0, 0, 0, 12, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeDownSeq1 = {
        .clearance = { 0, 32, 0, 12, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeDownSeq2 = {
        .clearance = { -32, 0, 0, 12, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeDownSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeDownSeq4 = {
        .clearance = { -32, 64, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeDownSeq5 = {
        .clearance = { -64, 32, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeDownSeq6 = {
        .clearance = { -64, 64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .metalSupports = { MetalSupportPlace::Centre },
        .extraSupportRotation = 1,
    };

    static constexpr SequenceDescriptor kUp25LeftBankedSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up25Deg },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kUp25RightBankedSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up25Deg },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kWaterfallSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRapidsSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kOnRidePhotoSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kDown25LeftBankedSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Down25Deg },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kDown25RightBankedSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Down25Deg },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kWatersplashSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kWatersplashSeq1 = {
        .clearance = { -32, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kWatersplashSeq2 = {
        .clearance = { -64, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kWatersplashSeq3 = {
        .clearance = { -96, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kWatersplashSeq4 = {
        .clearance = { -128, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatToUp60LongBaseSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::FlatToUp60DegLongBaseSeq0 },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kFlatToUp60LongBaseSeq1 = {
        .clearance = { -32, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::FlatToUp60DegLongBaseSeq1 },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kFlatToUp60LongBaseSeq2 = {
        .clearance = { -64, 0, 16, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::FlatToUp60DegLongBaseSeq2 },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kFlatToUp60LongBaseSeq3 = {
        .clearance = { -96, 0, 40, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::FlatToUp60DegLongBaseSeq3 },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kUp60ToFlatLongBaseSeq0 = {
        .clearance = { 0, 0, 0, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up60DegToFlatLongBaseSeq0 },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kUp60ToFlatLongBaseSeq1 = {
        .clearance = { -32, 0, 40, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up60DegToFlatLongBaseSeq1 },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kUp60ToFlatLongBaseSeq2 = {
        .clearance = { -64, 0, 64, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up60DegToFlatLongBaseSeq2 },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kUp60ToFlatLongBaseSeq3 = {
        .clearance = { -96, 0, 80, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up60DegToFlatLongBaseSeq3 },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kWhirlpoolSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kDown60ToFlatLongBaseSeq0 = {
        .clearance = { 0, 0, 40, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Down60DegToFlatLongBaseSeq0 },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kDown60ToFlatLongBaseSeq1 = {
        .clearance = { -32, 0, 16, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Down60DegToFlatLongBaseSeq1 },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kDown60ToFlatLongBaseSeq2 = {
        .clearance = { -64, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Down60DegToFlatLongBaseSeq2 },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kDown60ToFlatLongBaseSeq3 = {
        .clearance = { -96, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Down60DegToFlatLongBaseSeq3 },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kFlatToDown60LongBaseSeq0 = {
        .clearance = { 0, 0, 80, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::FlatToDown60DegLongBaseSeq0 },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kFlatToDown60LongBaseSeq1 = {
        .clearance = { -32, 0, 64, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::FlatToDown60DegLongBaseSeq1 },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kFlatToDown60LongBaseSeq2 = {
        .clearance = { -64, 0, 40, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::FlatToDown60DegLongBaseSeq2 },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kFlatToDown60LongBaseSeq3 = {
        .clearance = { -96, 0, 0, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::FlatToDown60DegLongBaseSeq3 },
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kCableLiftHillSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kCableLiftHillSeq1 = {
        .clearance = { -32, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kCableLiftHillSeq2 = {
        .clearance = { -64, 0, -32, 32, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kCableLiftHillSeq3 = {
        .clearance = { -96, 0, -96, 64, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::Centre, true },
    };

    static constexpr SequenceDescriptor kReverseFreefallSlopeSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kReverseFreefallSlopeSeq1 = {
        .clearance = { -32, 0, 0, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kReverseFreefallSlopeSeq2 = {
        .clearance = { -64, 0, 0, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kReverseFreefallSlopeSeq3 = {
        .clearance = { -96, 0, 0, 80, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kReverseFreefallSlopeSeq4 = {
        .clearance = { -128, 0, 0, 160, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kReverseFreefallSlopeSeq5 = {
        .clearance = { -192, 0, 0, 208, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1011,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kReverseFreefallSlopeSeq6 = {
        .clearance = { -160, 0, 0, 208, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kReverseFreefallVerticalSeq0 = {
        .clearance = { 0, 0, 0, 48, { 0b1111, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kReverseFreefallVerticalSeq1 = {
        .clearance = { 32, 0, 0, 48, { 0b1111, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kUp90Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1111,
    };

    static constexpr SequenceDescriptor kUp90Seq1 = {
        .clearance = { 32, 0, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    };

    static constexpr SequenceDescriptor kDown90Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1111,
    };

    static constexpr SequenceDescriptor kDown90Seq1 = {
        .clearance = { 32, 0, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    };

    static constexpr SequenceDescriptor kUp60ToUp90Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b1100 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1011,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kUp60ToUp90Seq1 = {
        .clearance = { 32, 0, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    };

    static constexpr SequenceDescriptor kDown90ToDown60Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b0011 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1110,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kUp90ToUp60Seq0 = {
        .clearance = { 0, 0, 0, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1011,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kDown60ToDown90Seq0 = {
        .clearance = { 0, 0, 0, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1110,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kDown60ToDown90Seq1 = {
        .clearance = { 32, 0, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    };

    static constexpr SequenceDescriptor kBrakeForDropSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagSeq2 = {
        .clearance = { -32, -32, 0, 0, { 0b1000, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::Corner1 },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagSeq3 = {
        .clearance = { -64, 0, 0, 0, { 0b0010, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagSeq4 = {
        .clearance = { -64, -32, 0, 0, { 0b0001, 0 }, 0 },
        .metalSupports = { MetalSupportPlace::BottomCorner },
    };

    static constexpr SequenceDescriptor kRightEighthToDiagSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::NeSw },
        .metalSupports = { MetalSupportPlace::Centre },
    };

    static constexpr SequenceDescriptor kRightEighthToDiagSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightEighthToDiagSeq2 = {
        .clearance = { -32, 32, 0, 0, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kRightEighthToDiagSeq3 = {
        .clearance = { -64, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kRightEighthToDiagSeq4 = {
        .clearance = { -64, 32, 0, 0, { 0b0010, 0 }, 0 },
        .metalSupports = { MetalSupportPlace::LeftCorner },
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalSeq2 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalSeq4 = {
        .clearance = { -64, 32, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0100,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner1 },
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalSeq4 = {
        .clearance = { -32, 64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagSeq2 = {
        .clearance = { -32, -32, 0, 0, { 0b1000, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::Corner1 },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagSeq3 = {
        .clearance = { -64, 0, 0, 0, { 0b0010, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagSeq4 = {
        .clearance = { -64, -32, 0, 0, { 0b0001, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagSeq2 = {
        .clearance = { -32, 32, 0, 0, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagSeq3 = {
        .clearance = { -64, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagSeq4 = {
        .clearance = { -64, 32, 0, 0, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalSeq2 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalSeq4 = {
        .clearance = { -64, 32, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0100,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner1 },
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalSeq4 = {
        .clearance = { -32, 64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
    };

    static constexpr SequenceDescriptor kDiagFlatSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagFlatSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagFlatSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagFlatSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagUp25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagUp25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagUp60Seq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagUp60Seq1 = {
        .clearance = { 0, 32, 0, 64, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagUp60Seq2 = {
        .clearance = { -32, 0, 0, 64, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagUp60Seq3 = {
        .clearance = { -32, 32, 0, 64, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagFlatToUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagFlatToUp25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagFlatToUp25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagFlatToUp25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagUp25ToUp60Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagUp25ToUp60Seq1 = {
        .clearance = { 0, 32, 0, 32, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagUp25ToUp60Seq2 = {
        .clearance = { -32, 0, 0, 32, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagUp25ToUp60Seq3 = {
        .clearance = { -32, 32, 0, 32, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagUp60ToUp25Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagUp60ToUp25Seq1 = {
        .clearance = { 0, 32, 0, 32, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagUp60ToUp25Seq2 = {
        .clearance = { -32, 0, 0, 32, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagUp60ToUp25Seq3 = {
        .clearance = { -32, 32, 0, 32, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagUp25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagUp25ToFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagUp25ToFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagUp25ToFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagDown25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagDown25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagDown25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagDown60Seq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagDown60Seq1 = {
        .clearance = { 0, 32, 0, 64, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagDown60Seq2 = {
        .clearance = { -32, 0, 0, 64, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagDown60Seq3 = {
        .clearance = { -32, 32, 0, 64, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagFlatToDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagFlatToDown25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagFlatToDown25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagFlatToDown25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagDown25ToDown60Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagDown25ToDown60Seq1 = {
        .clearance = { 0, 32, 0, 32, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagDown25ToDown60Seq2 = {
        .clearance = { -32, 0, 0, 32, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagDown25ToDown60Seq3 = {
        .clearance = { -32, 32, 0, 32, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagDown60ToDown25Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagDown60ToDown25Seq1 = {
        .clearance = { 0, 32, 0, 32, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagDown60ToDown25Seq2 = {
        .clearance = { -32, 0, 0, 32, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagDown60ToDown25Seq3 = {
        .clearance = { -32, 32, 0, 32, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagDown25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagDown25ToFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagDown25ToFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagDown25ToFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagFlatToUp60Seq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagFlatToUp60Seq1 = {
        .clearance = { 0, 32, 0, 24, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagFlatToUp60Seq2 = {
        .clearance = { -32, 0, 0, 24, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagFlatToUp60Seq3 = {
        .clearance = { -32, 32, 0, 24, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagUp60ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagUp60ToFlatSeq1 = {
        .clearance = { 0, 32, 0, 24, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagUp60ToFlatSeq2 = {
        .clearance = { -32, 0, 0, 24, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagUp60ToFlatSeq3 = {
        .clearance = { -32, 32, 0, 24, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagFlatToDown60Seq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagFlatToDown60Seq1 = {
        .clearance = { 0, 32, 0, 24, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagFlatToDown60Seq2 = {
        .clearance = { -32, 0, 0, 24, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagFlatToDown60Seq3 = {
        .clearance = { -32, 32, 0, 24, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagDown60ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagDown60ToFlatSeq1 = {
        .clearance = { 0, 32, 0, 24, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagDown60ToFlatSeq2 = {
        .clearance = { -32, 0, 0, 24, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagDown60ToFlatSeq3 = {
        .clearance = { -32, 32, 0, 24, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToFlatSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToFlatSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToFlatSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToFlatSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankToFlatSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankToFlatSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankToFlatSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagRightBankToFlatSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToUp25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToUp25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToUp25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankToUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankToUp25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankToUp25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagRightBankToUp25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagUp25ToLeftBankSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagUp25ToLeftBankSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagUp25ToLeftBankSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagUp25ToLeftBankSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagUp25ToRightBankSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagUp25ToRightBankSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagUp25ToRightBankSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagUp25ToRightBankSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToDown25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToDown25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToDown25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankToDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankToDown25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankToDown25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagRightBankToDown25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagDown25ToLeftBankSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagDown25ToLeftBankSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagDown25ToLeftBankSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagDown25ToLeftBankSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagDown25ToRightBankSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagDown25ToRightBankSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagDown25ToRightBankSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagDown25ToRightBankSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagRightBankSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kLogFlumeReverserSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kSpinningTunnelSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = TRACK_SEQUENCE_FLAG_DISALLOW_DOORS,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftBarrelRollUpToDownSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftBarrelRollUpToDownSeq1 = {
        .clearance = { -32, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftBarrelRollUpToDownSeq2 = {
        .clearance = { -64, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightBarrelRollUpToDownSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightBarrelRollUpToDownSeq1 = {
        .clearance = { -32, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightBarrelRollUpToDownSeq2 = {
        .clearance = { -64, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftBarrelRollDownToUpSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftBarrelRollDownToUpSeq1 = {
        .clearance = { -32, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftBarrelRollDownToUpSeq2 = {
        .clearance = { -64, 0, -32, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightBarrelRollDownToUpSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightBarrelRollDownToUpSeq1 = {
        .clearance = { -32, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightBarrelRollDownToUpSeq2 = {
        .clearance = { -64, 0, -32, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftBankToLeftQuarterTurn3TilesUp25Seq0 = {
        .clearance = { 0, 0, 0, 0, { 0b0111, 0b0100 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kLeftBankToLeftQuarterTurn3TilesUp25Seq1 = {
        .clearance = { 0, -32, 0, 16, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftBankToLeftQuarterTurn3TilesUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftBankToLeftQuarterTurn3TilesUp25Seq3 = {
        .clearance = { -32, -32, 16, 16, { 0b0111, 0b0110 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kRightBankToRightQuarterTurn3TilesUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1011, 0b1000 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kRightBankToRightQuarterTurn3TilesUp25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightBankToRightQuarterTurn3TilesUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightBankToRightQuarterTurn3TilesUp25Seq3 = {
        .clearance = { -32, 32, 16, 16, { 0b1011, 0b1001 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesDown25ToLeftBankSeq0 = {
        .clearance = { 0, 0, 16, 16, { 0b0111, 0b0011 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesDown25ToLeftBankSeq1 = {
        .clearance = { 0, -32, 0, 16, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesDown25ToLeftBankSeq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesDown25ToLeftBankSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b0111, 0b0001 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesDown25ToRightBankSeq0 = {
        .clearance = { 0, 0, 16, 16, { 0b1011, 0b0011 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesDown25ToRightBankSeq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesDown25ToRightBankSeq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesDown25ToRightBankSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1011, 0b0010 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kPoweredLiftSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up25Deg },
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopUpSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopUpSeq1 = {
        .clearance = { -32, 0, 16, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopUpSeq2 = {
        .clearance = { -64, 0, 32, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopUpSeq3 = {
        .clearance = { -96, 0, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopUpSeq4 = {
        .clearance = { -128, -32, 120, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopUpSeq5 = {
        .clearance = { -96, -32, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopUpSeq6 = {
        .clearance = { -64, -32, 248, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopUpSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopUpSeq1 = {
        .clearance = { -32, 0, 16, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopUpSeq2 = {
        .clearance = { -64, 0, 32, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopUpSeq3 = {
        .clearance = { -96, 0, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopUpSeq4 = {
        .clearance = { -128, 32, 120, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopUpSeq5 = {
        .clearance = { -96, 32, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopUpSeq6 = {
        .clearance = { -64, 32, 248, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopDownSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopDownSeq1 = {
        .clearance = { -32, 0, -216, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopDownSeq2 = {
        .clearance = { -64, 0, -160, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopDownSeq3 = {
        .clearance = { -32, -32, -216, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopDownSeq4 = {
        .clearance = { 0, -32, -248, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopDownSeq5 = {
        .clearance = { 32, -32, -264, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopDownSeq6 = {
        .clearance = { 64, -32, -280, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopDownSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopDownSeq1 = {
        .clearance = { -32, 0, -216, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopDownSeq2 = {
        .clearance = { -64, 0, -160, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopDownSeq3 = {
        .clearance = { -32, 32, -216, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopDownSeq4 = {
        .clearance = { 0, 32, -248, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopDownSeq5 = {
        .clearance = { 32, 32, -264, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopDownSeq6 = {
        .clearance = { 64, 32, -280, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kLeftFlyerTwistUpSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kLeftFlyerTwistUpSeq1 = {
        .clearance = { -32, 0, -16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kLeftFlyerTwistUpSeq2 = {
        .clearance = { -64, 0, -16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kRightFlyerTwistUpSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kRightFlyerTwistUpSeq1 = {
        .clearance = { -32, 0, -16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kRightFlyerTwistUpSeq2 = {
        .clearance = { -64, 0, -16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kLeftFlyerTwistDownSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kLeftFlyerTwistDownSeq1 = {
        .clearance = { -32, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kLeftFlyerTwistDownSeq2 = {
        .clearance = { -64, 0, 16, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kRightFlyerTwistDownSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kRightFlyerTwistDownSeq1 = {
        .clearance = { -32, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kRightFlyerTwistDownSeq2 = {
        .clearance = { -64, 0, 16, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopUninvertedUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopUninvertedUpSeq1 = {
        .clearance = { -32, 0, 16, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopUninvertedUpSeq2 = {
        .clearance = { -64, 0, 32, 96, { 0b0011, 0 }, 0 },
        .allowedWallEdges = 0b1011,
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopUninvertedUpSeq3 = {
        .clearance = { -32, 0, 120, 16, { 0b1111, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopInvertedDownSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopInvertedDownSeq1 = {
        .clearance = { -32, 0, -88, 96, { 0b0011, 0 }, 0 },
        .allowedWallEdges = 0b1011,
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopInvertedDownSeq2 = {
        .clearance = { 0, 0, -104, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopInvertedDownSeq3 = {
        .clearance = { 32, 0, -120, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kLeftFlyerCorkscrewUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kLeftFlyerCorkscrewUpSeq1 = {
        .clearance = { -32, 0, 24, 32, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftFlyerCorkscrewUpSeq2 = {
        .clearance = { -32, -32, 48, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
    };

    static constexpr SequenceDescriptor kRightFlyerCorkscrewUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kRightFlyerCorkscrewUpSeq1 = {
        .clearance = { -32, 0, 24, 32, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightFlyerCorkscrewUpSeq2 = {
        .clearance = { -32, 32, 48, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
    };

    static constexpr SequenceDescriptor kLeftFlyerCorkscrewDownSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kLeftFlyerCorkscrewDownSeq1 = {
        .clearance = { -32, 0, -24, 32, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftFlyerCorkscrewDownSeq2 = {
        .clearance = { -32, -32, -48, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
    };

    static constexpr SequenceDescriptor kRightFlyerCorkscrewDownSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kRightFlyerCorkscrewDownSeq1 = {
        .clearance = { -32, 0, -24, 32, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightFlyerCorkscrewDownSeq2 = {
        .clearance = { -32, 32, -48, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
    };

    static constexpr SequenceDescriptor kHeartLineTransferUpSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::FlatToUp25Deg },
    };

    static constexpr SequenceDescriptor kHeartLineTransferUpSeq1 = {
        .clearance = { -32, 0, 0, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up25DegToFlat },
    };

    static constexpr SequenceDescriptor kHeartLineTransferUpSeq2 = {
        .clearance = { -64, 0, 16, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kHeartLineTransferUpSeq3 = {
        .clearance = { 0, 0, 32, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kHeartLineTransferDownSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kHeartLineTransferDownSeq1 = {
        .clearance = { -32, 0, -32, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up25DegToFlat },
    };

    static constexpr SequenceDescriptor kHeartLineTransferDownSeq2 = {
        .clearance = { -64, 0, -16, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kHeartLineTransferDownSeq3 = {
        .clearance = { 0, 0, -32, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::FlatToUp25Deg },
    };

    static constexpr SequenceDescriptor kLeftHeartLineRollSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftHeartLineRollSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftHeartLineRollSeq2 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftHeartLineRollSeq3 = {
        .clearance = { -96, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftHeartLineRollSeq4 = {
        .clearance = { -128, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftHeartLineRollSeq5 = {
        .clearance = { -160, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightHeartLineRollSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightHeartLineRollSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightHeartLineRollSeq2 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightHeartLineRollSeq3 = {
        .clearance = { -96, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightHeartLineRollSeq4 = {
        .clearance = { -128, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightHeartLineRollSeq5 = {
        .clearance = { -160, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kMinigolfHoleASeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kMinigolfHoleASeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kMinigolfHoleBSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kMinigolfHoleBSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kMinigolfHoleCSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kMinigolfHoleCSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kMinigolfHoleDSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kMinigolfHoleDSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kMinigolfHoleDSeq2 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NwSe },
    };

    static constexpr SequenceDescriptor kMinigolfHoleESeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kMinigolfHoleESeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kMinigolfHoleESeq2 = {
        .clearance = { -32, -32, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NwSe },
    };

    static constexpr SequenceDescriptor kMultiDimInvertedFlatToDown90QuarterLoopSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kMultiDimInvertedFlatToDown90QuarterLoopSeq1 = {
        .clearance = { -32, 0, -40, 32, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kMultiDimInvertedFlatToDown90QuarterLoopSeq2 = {
        .clearance = { -64, 0, -96, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kMultiDimInvertedFlatToDown90QuarterLoopSeq3 = {
        .clearance = { -96, 0, -96, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    };

    static constexpr SequenceDescriptor kUp90ToInvertedFlatQuarterLoopSeq0 = {
        .clearance = { 0, 0, 0, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kUp90ToInvertedFlatQuarterLoopSeq1 = {
        .clearance = { 32, 0, 56, 32, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kUp90ToInvertedFlatQuarterLoopSeq2 = {
        .clearance = { 64, 0, 96, 16, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kInvertedFlatToDown90QuarterLoopSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kInvertedFlatToDown90QuarterLoopSeq1 = {
        .clearance = { -32, 0, -72, 32, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kInvertedFlatToDown90QuarterLoopSeq2 = {
        .clearance = { -64, 0, -128, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kInvertedFlatToDown90QuarterLoopSeq3 = {
        .clearance = { -96, 0, -128, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    };

    static constexpr SequenceDescriptor kLeftCurvedLiftHillSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kLeftCurvedLiftHillSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftCurvedLiftHillSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftCurvedLiftHillSeq3 = {
        .clearance = { -32, -32, 0, 8, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kRightCurvedLiftHillSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kRightCurvedLiftHillSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightCurvedLiftHillSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightCurvedLiftHillSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kLeftReverserSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftReverserSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftReverserSeq2 = {
        .clearance = { -32, -32, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftReverserSeq3 = {
        .clearance = { -64, -32, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftReverserSeq4 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftReverserSeq5 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightReverserSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightReverserSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightReverserSeq2 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightReverserSeq3 = {
        .clearance = { -64, 32, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightReverserSeq4 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightReverserSeq5 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kAirThrustTopCapSeq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kAirThrustTopCapSeq1 = {
        .clearance = { 32, 0, 0, 32, { 0b1111, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kAirThrustTopCapSeq2 = {
        .clearance = { -64, 0, 0, 32, { 0b1111, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kAirThrustTopCapSeq3 = {
        .clearance = { -32, 0, 0, 32, { 0b1111, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownSeq0 = {
        .clearance = { 0, 0, 0, 48, { 0b1111, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownSeq1 = {
        .clearance = { 32, 0, 0, 48, { 0b1111, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownToLevelSeq0 = {
        .clearance = { 0, 0, 0, 208, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownToLevelSeq1 = {
        .clearance = { 32, 0, 0, 208, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownToLevelSeq2 = {
        .clearance = { -32, 0, 0, 160, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownToLevelSeq3 = {
        .clearance = { -64, 0, 0, 80, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownToLevelSeq4 = {
        .clearance = { -96, 0, 0, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownToLevelSeq5 = {
        .clearance = { -128, 0, 0, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownToLevelSeq6 = {
        .clearance = { -160, 0, 0, 16, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kBlockBrakesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TileUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b0111, 0b0100 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TileUp25Seq1 = {
        .clearance = { 0, -32, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TileUp25Seq2 = {
        .clearance = { -32, 0, 16, 0, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TileUp25Seq3 = {
        .clearance = { -32, -32, 16, 16, { 0b0111, 0b0110 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TileUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1011, 0b1000 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TileUp25Seq1 = {
        .clearance = { 0, 32, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TileUp25Seq2 = {
        .clearance = { -32, 0, 16, 0, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TileUp25Seq3 = {
        .clearance = { -32, 32, 16, 16, { 0b1011, 0b1001 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TileDown25Seq0 = {
        .clearance = { 0, 0, 16, 16, { 0b0111, 0b0011 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TileDown25Seq1 = {
        .clearance = { 0, -32, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TileDown25Seq2 = {
        .clearance = { -32, 0, 16, 0, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TileDown25Seq3 = {
        .clearance = { -32, -32, 0, 16, { 0b0111, 0b0001 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TileDown25Seq0 = {
        .clearance = { 0, 0, 16, 16, { 0b1011, 0b0011 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TileDown25Seq1 = {
        .clearance = { 0, 32, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TileDown25Seq2 = {
        .clearance = { -32, 0, 16, 0, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TileDown25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b1011, 0b0010 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileUp25Seq1 = {
        .clearance = { 0, -32, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileUp25Seq2 = {
        .clearance = { -32, 0, 16, 16, { 0b0111, 0b0100 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileUp25Seq3 = {
        .clearance = { -32, -32, 24, 16, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::Corner1 },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileUp25Seq4 = {
        .clearance = { -32, -64, 48, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileUp25Seq5 = {
        .clearance = { -64, -32, 32, 16, { 0b0111, 0b0100 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileUp25Seq6 = {
        .clearance = { -64, -64, 48, 16, { 0b1111, 0b0110 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileUp25Seq7 = {
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileUp25Seq1 = {
        .clearance = { 0, 32, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileUp25Seq2 = {
        .clearance = { -32, 0, 16, 16, { 0b1011, 0b1000 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileUp25Seq3 = {
        .clearance = { -32, 32, 24, 16, { 0b1110, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileUp25Seq4 = {
        .clearance = { -32, 64, 48, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileUp25Seq5 = {
        .clearance = { -64, 32, 32, 16, { 0b1011, 0b1000 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileUp25Seq6 = {
        .clearance = { -64, 64, 48, 16, { 0b1111, 0b1001 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileUp25Seq7 = {
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileDown25Seq0 = {
        .clearance = { 0, 0, 48, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileDown25Seq1 = {
        .clearance = { 0, -32, 48, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileDown25Seq2 = {
        .clearance = { -32, 0, 32, 16, { 0b0111, 0b0001 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileDown25Seq3 = {
        .clearance = { -32, -32, 24, 16, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileDown25Seq4 = {
        .clearance = { -32, -64, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileDown25Seq5 = {
        .clearance = { -64, -32, 16, 16, { 0b0111, 0b0001 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileDown25Seq6 = {
        .clearance = { -64, -64, 0, 16, { 0b1111, 0b1001 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileDown25Seq7 = {
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileDown25Seq0 = {
        .clearance = { 0, 0, 48, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileDown25Seq1 = {
        .clearance = { 0, 32, 48, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileDown25Seq2 = {
        .clearance = { -32, 0, 32, 16, { 0b1011, 0b0010 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileDown25Seq3 = {
        .clearance = { -32, 32, 24, 16, { 0b1110, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::Corner1 },
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileDown25Seq4 = {
        .clearance = { -32, 64, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileDown25Seq5 = {
        .clearance = { -64, 32, 16, 16, { 0b1011, 0b0010 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileDown25Seq6 = {
        .clearance = { -64, 64, 0, 16, { 0b1111, 0b0110 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileDown25Seq7 = {
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kUp25ToLeftBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up25Deg },
    };

    static constexpr SequenceDescriptor kUp25ToRightBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up25Deg },
    };

    static constexpr SequenceDescriptor kLeftBankedUp25ToUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up25Deg },
    };

    static constexpr SequenceDescriptor kRightBankedUp25ToUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up25Deg },
    };

    static constexpr SequenceDescriptor kDown25ToLeftBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Down25Deg },
    };

    static constexpr SequenceDescriptor kDown25ToRightBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Down25Deg },
    };

    static constexpr SequenceDescriptor kLeftBankedDown25ToDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Down25Deg },
    };

    static constexpr SequenceDescriptor kRightBankedDown25ToDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Down25Deg },
    };

    static constexpr SequenceDescriptor kLeftBankedFlatToLeftBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::FlatToUp25Deg },
    };

    static constexpr SequenceDescriptor kRightBankedFlatToRightBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::FlatToUp25Deg },
    };

    static constexpr SequenceDescriptor kLeftBankedUp25ToLeftBankedFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up25DegToFlat },
    };

    static constexpr SequenceDescriptor kRightBankedUp25ToRightBankedFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up25DegToFlat },
    };

    static constexpr SequenceDescriptor kLeftBankedFlatToLeftBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::FlatToDown25Deg },
    };

    static constexpr SequenceDescriptor kRightBankedFlatToRightBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::FlatToDown25Deg },
    };

    static constexpr SequenceDescriptor kLeftBankedDown25ToLeftBankedFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Down25DegToFlat },
    };

    static constexpr SequenceDescriptor kRightBankedDown25ToRightBankedFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Down25DegToFlat },
    };

    static constexpr SequenceDescriptor kFlatToLeftBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::FlatToUp25Deg },
    };

    static constexpr SequenceDescriptor kFlatToRightBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::FlatToUp25Deg },
    };

    static constexpr SequenceDescriptor kLeftBankedUp25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up25DegToFlat },
    };

    static constexpr SequenceDescriptor kRightBankedUp25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up25DegToFlat },
    };

    static constexpr SequenceDescriptor kFlatToLeftBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::FlatToDown25Deg },
    };

    static constexpr SequenceDescriptor kFlatToRightBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::FlatToDown25Deg },
    };

    static constexpr SequenceDescriptor kLeftBankedDown25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Down25DegToFlat },
    };

    static constexpr SequenceDescriptor kRightBankedDown25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Down25DegToFlat },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn1TileUp90Seq0 = {
        .clearance = { 0, 0, 0, 72, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1111,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn1TileUp90Seq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn1TileUp90Seq0 = {
        .clearance = { 0, 0, 0, 72, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1111,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn1TileUp90Seq1 = {
        .clearance = { 0, -32, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn1TileDown90Seq0 = {
        .clearance = { 0, 0, 0, 72, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1111,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn1TileDown90Seq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn1TileDown90Seq0 = {
        .clearance = { 0, 0, 0, 72, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1111,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn1TileDown90Seq1 = {
        .clearance = { 0, -32, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    };

    static constexpr SequenceDescriptor kMultiDimUp90ToInvertedFlatQuarterLoopSeq0 = {
        .clearance = { 0, 0, 0, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kMultiDimUp90ToInvertedFlatQuarterLoopSeq1 = {
        .clearance = { 32, 0, 56, 32, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kMultiDimUp90ToInvertedFlatQuarterLoopSeq2 = {
        .clearance = { 64, 0, 96, 16, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kMultiDimFlatToDown90QuarterLoopSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kMultiDimFlatToDown90QuarterLoopSeq1 = {
        .clearance = { -32, 0, -72, 32, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kMultiDimFlatToDown90QuarterLoopSeq2 = {
        .clearance = { -64, 0, -128, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kMultiDimFlatToDown90QuarterLoopSeq3 = {
        .clearance = { -96, 0, -128, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    };

    static constexpr SequenceDescriptor kMultiDimInvertedUp90ToFlatQuarterLoopSeq0 = {
        .clearance = { 0, 0, 32, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kMultiDimInvertedUp90ToFlatQuarterLoopSeq1 = {
        .clearance = { 32, 0, 88, 32, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kMultiDimInvertedUp90ToFlatQuarterLoopSeq2 = {
        .clearance = { 64, 0, 128, 16, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kRotationControlToggleSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4ASeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4ASeq1 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4ASeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4ASeq3 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_1 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x2Seq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x2Seq1 = {
        .clearance = { 0, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x2Seq2 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x2Seq3 = {
        .clearance = { 32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq1 = {
        .clearance = { 0, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq2 = {
        .clearance = { 0, 64, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq3 = {
        .clearance = { 0, 96, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq4 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_3,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq5 = {
        .clearance = { 32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq6 = {
        .clearance = { 32, 64, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq7 = {
        .clearance = { 32, 96, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_1,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq8 = {
        .clearance = { 64, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_3,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq9 = {
        .clearance = { 64, 32, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq10 = {
        .clearance = { 64, 64, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq11 = {
        .clearance = { 64, 96, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_1,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq12 = {
        .clearance = { 96, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq13 = {
        .clearance = { 96, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_2,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq14 = {
        .clearance = { 96, 64, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_2,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq15 = {
        .clearance = { 96, 96, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq1 = {
        .clearance = { 0, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq2 = {
        .clearance = { 0, 64, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq3 = {
        .clearance = { 0, 96, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq4 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq5 = {
        .clearance = { 32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_2,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq6 = {
        .clearance = { 32, 64, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_2,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq7 = {
        .clearance = { 32, 96, 0, 0, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_1 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x5Seq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x5Seq1 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x5Seq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x5Seq3 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x5Seq4 = {
        .clearance = { 64, 0, 0, 0, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_1 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x1ASeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_ORIGIN | TRACK_SEQUENCE_FLAG_CONNECTS_TO_PATH,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4BSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_ORIGIN,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4BSeq1 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4BSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_1,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4BSeq3 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_1 },
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x1BSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2
            | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN | TRACK_SEQUENCE_FLAG_CONNECTS_TO_PATH,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4CSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4CSeq1 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4CSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4CSeq3 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_1 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_ORIGIN,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq1 = {
        .clearance = { -32, -32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_3,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq4 = {
        .clearance = { 0, -32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_3,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq5 = {
        .clearance = { 0, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_1,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq6 = {
        .clearance = { 32, -32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq7 = {
        .clearance = { 32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq8 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_1 },
        .flags = TRACK_SEQUENCE_FLAG_DIRECTION_2,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewUpSeq1 = {
        .clearance = { -32, 0, 8, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewUpSeq2 = {
        .clearance = { -64, 0, 32, 24, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewUpSeq3 = {
        .clearance = { -32, -32, 32, 24, { 0b1000, 0 }, 0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewUpSeq4 = {
        .clearance = { -64, -32, 48, 40, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewUpSeq5 = {
        .clearance = { -64, -64, 72, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewUpSeq1 = {
        .clearance = { -32, 0, 8, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewUpSeq2 = {
        .clearance = { -64, 0, 32, 24, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewUpSeq3 = {
        .clearance = { -32, 32, 32, 24, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewUpSeq4 = {
        .clearance = { -64, 32, 48, 40, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewUpSeq5 = {
        .clearance = { -64, 64, 72, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewDownSeq0 = {
        .clearance = { 0, 0, -40, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewDownSeq1 = {
        .clearance = { -32, 0, -64, 40, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewDownSeq2 = {
        .clearance = { -32, -32, -80, 24, { 0b1000, 0 }, 0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewDownSeq3 = {
        .clearance = { -64, 0, -80, 24, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewDownSeq4 = {
        .clearance = { -64, -32, -104, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewDownSeq5 = {
        .clearance = { -64, -64, -112, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewDownSeq0 = {
        .clearance = { 0, 0, -40, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewDownSeq1 = {
        .clearance = { -32, 0, -64, 40, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewDownSeq2 = {
        .clearance = { -32, 32, -80, 24, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewDownSeq3 = {
        .clearance = { -64, 0, -80, 24, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewDownSeq4 = {
        .clearance = { -64, 32, -104, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewDownSeq5 = {
        .clearance = { -64, 64, -112, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopUpSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopUpSeq1 = {
        .clearance = { -32, 0, 16, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopUpSeq2 = {
        .clearance = { -64, 0, 56, 120, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopUpSeq3 = {
        .clearance = { -64, -32, 56, 120, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopUpSeq4 = {
        .clearance = { -32, -32, 168, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopUpSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopUpSeq1 = {
        .clearance = { -32, 0, 16, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopUpSeq2 = {
        .clearance = { -64, 0, 56, 120, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopUpSeq3 = {
        .clearance = { -64, 32, 56, 120, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopUpSeq4 = {
        .clearance = { -32, 32, 168, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopDownSeq0 = {
        .clearance = { 0, 0, -48, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopDownSeq1 = {
        .clearance = { -32, 0, -160, 120, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopDownSeq2 = {
        .clearance = { -32, -32, -160, 120, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopDownSeq3 = {
        .clearance = { 0, -32, -200, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopDownSeq4 = {
        .clearance = { 32, -32, -216, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopDownSeq0 = {
        .clearance = { 0, 0, -48, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopDownSeq1 = {
        .clearance = { -32, 0, -160, 120, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopDownSeq2 = {
        .clearance = { -32, 32, -160, 120, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopDownSeq3 = {
        .clearance = { 0, 32, -200, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopDownSeq4 = {
        .clearance = { 32, 32, -216, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kLeftZeroGRollUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftZeroGRollUpSeq1 = {
        .clearance = { -32, 0, 8, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftZeroGRollUpSeq2 = {
        .clearance = { -64, 0, 24, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightZeroGRollUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightZeroGRollUpSeq1 = {
        .clearance = { -32, 0, 8, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightZeroGRollUpSeq2 = {
        .clearance = { -64, 0, 24, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftZeroGRollDownSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftZeroGRollDownSeq1 = {
        .clearance = { -32, 0, -48, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftZeroGRollDownSeq2 = {
        .clearance = { -64, 0, -56, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightZeroGRollDownSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightZeroGRollDownSeq1 = {
        .clearance = { -32, 0, -48, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightZeroGRollDownSeq2 = {
        .clearance = { -64, 0, -56, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftLargeZeroGRollUpSeq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftLargeZeroGRollUpSeq1 = {
        .clearance = { -32, 0, 56, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftLargeZeroGRollUpSeq2 = {
        .clearance = { -64, 0, 96, 40, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftLargeZeroGRollUpSeq3 = {
        .clearance = { -96, 0, 120, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightLargeZeroGRollUpSeq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightLargeZeroGRollUpSeq1 = {
        .clearance = { -32, 0, 56, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightLargeZeroGRollUpSeq2 = {
        .clearance = { -64, 0, 96, 40, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightLargeZeroGRollUpSeq3 = {
        .clearance = { -96, 0, 120, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftLargeZeroGRollDownSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftLargeZeroGRollDownSeq1 = {
        .clearance = { -32, 0, -56, 40, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftLargeZeroGRollDownSeq2 = {
        .clearance = { -64, 0, -96, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftLargeZeroGRollDownSeq3 = {
        .clearance = { -96, 0, -152, 64, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightLargeZeroGRollDownSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightLargeZeroGRollDownSeq1 = {
        .clearance = { -32, 0, -56, 40, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightLargeZeroGRollDownSeq2 = {
        .clearance = { -64, 0, -96, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightLargeZeroGRollDownSeq3 = {
        .clearance = { -96, 0, -152, 64, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedUpSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedUpSeq1 = {
        .clearance = { -32, 0, 16, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedUpSeq2 = {
        .clearance = { -64, 0, 32, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedUpSeq3 = {
        .clearance = { -96, 0, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedUpSeq4 = {
        .clearance = { -128, -32, 120, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedUpSeq5 = {
        .clearance = { -96, -32, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedUpSeq6 = {
        .clearance = { -64, -32, 248, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedUpSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedUpSeq1 = {
        .clearance = { -32, 0, 16, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedUpSeq2 = {
        .clearance = { -64, 0, 32, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedUpSeq3 = {
        .clearance = { -96, 0, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedUpSeq4 = {
        .clearance = { -128, 32, 120, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedUpSeq5 = {
        .clearance = { -96, 32, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedUpSeq6 = {
        .clearance = { -64, 32, 248, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedDownSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedDownSeq1 = {
        .clearance = { -32, 0, -184, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedDownSeq2 = {
        .clearance = { -64, 0, -128, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedDownSeq3 = {
        .clearance = { -32, -32, -184, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedDownSeq4 = {
        .clearance = { 0, -32, -216, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedDownSeq5 = {
        .clearance = { 32, -32, -232, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedDownSeq6 = {
        .clearance = { 64, -32, -248, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedDownSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedDownSeq1 = {
        .clearance = { -32, 0, -184, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedDownSeq2 = {
        .clearance = { -64, 0, -128, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedDownSeq3 = {
        .clearance = { -32, 32, -184, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedDownSeq4 = {
        .clearance = { 0, 32, -216, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedDownSeq5 = {
        .clearance = { 32, 32, -232, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedDownSeq6 = {
        .clearance = { 64, 32, -248, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedUpSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedUpSeq1 = {
        .clearance = { -32, 0, 16, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedUpSeq2 = {
        .clearance = { -64, 0, 32, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedUpSeq3 = {
        .clearance = { -96, 0, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedUpSeq4 = {
        .clearance = { -128, -32, 120, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedUpSeq5 = {
        .clearance = { -96, -32, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedUpSeq6 = {
        .clearance = { -64, -32, 248, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedUpSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedUpSeq1 = {
        .clearance = { -32, 0, 16, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedUpSeq2 = {
        .clearance = { -64, 0, 32, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedUpSeq3 = {
        .clearance = { -96, 0, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedUpSeq4 = {
        .clearance = { -128, 32, 120, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedUpSeq5 = {
        .clearance = { -96, 32, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedUpSeq6 = {
        .clearance = { -64, 32, 248, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedDownSeq0 = {
        .clearance = { 0, 0, -32, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedDownSeq1 = {
        .clearance = { -32, 0, -216, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedDownSeq2 = {
        .clearance = { -64, 0, -160, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedDownSeq3 = {
        .clearance = { -32, -32, -216, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedDownSeq4 = {
        .clearance = { 0, -32, -248, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedDownSeq5 = {
        .clearance = { 32, -32, -264, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedDownSeq6 = {
        .clearance = { 64, -32, -280, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedDownSeq0 = {
        .clearance = { 0, 0, -32, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedDownSeq1 = {
        .clearance = { -32, 0, -216, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedDownSeq2 = {
        .clearance = { -64, 0, -160, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedDownSeq3 = {
        .clearance = { -32, 32, -216, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedDownSeq4 = {
        .clearance = { 0, 32, -248, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedDownSeq5 = {
        .clearance = { 32, 32, -264, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedDownSeq6 = {
        .clearance = { 64, 32, -280, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopInvertedUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopInvertedUpSeq1 = {
        .clearance = { -32, 0, 16, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopInvertedUpSeq2 = {
        .clearance = { -64, 0, 32, 96, { 0b0011, 0 }, 0 },
        .allowedWallEdges = 0b1011,
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopInvertedUpSeq3 = {
        .clearance = { -32, 0, 120, 32, { 0b1111, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopUninvertedDownSeq0 = {
        .clearance = { 0, 0, -32, 32, { 0b1111, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopUninvertedDownSeq1 = {
        .clearance = { -32, 0, -120, 96, { 0b0011, 0 }, 0 },
        .allowedWallEdges = 0b1011,
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopUninvertedDownSeq2 = {
        .clearance = { 0, 0, -136, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopUninvertedDownSeq3 = {
        .clearance = { 32, 0, -152, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up25Deg },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagUp25Seq1 = {
        .clearance = { -32, 0, 16, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagUp25Seq2 = {
        .clearance = { -32, -32, 32, 16, { 0b1000, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::Corner1 },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagUp25Seq3 = {
        .clearance = { -64, 0, 32, 16, { 0b0010, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagUp25Seq4 = {
        .clearance = { -64, -32, 32, 24, { 0b0001, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kRightEighthToDiagUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up25Deg },
    };

    static constexpr SequenceDescriptor kRightEighthToDiagUp25Seq1 = {
        .clearance = { -32, 0, 16, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightEighthToDiagUp25Seq2 = {
        .clearance = { -32, 32, 32, 16, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kRightEighthToDiagUp25Seq3 = {
        .clearance = { -64, 0, 32, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kRightEighthToDiagUp25Seq4 = {
        .clearance = { -64, 32, 32, 24, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagDown25Seq0 = {
        .clearance = { 0, 0, -16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Down25Deg },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagDown25Seq1 = {
        .clearance = { -32, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagDown25Seq2 = {
        .clearance = { -32, -32, -48, 16, { 0b1000, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::Corner1 },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagDown25Seq3 = {
        .clearance = { -64, 0, -48, 16, { 0b0010, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagDown25Seq4 = {
        .clearance = { -64, -32, -48, 16, { 0b0001, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kRightEighthToDiagDown25Seq0 = {
        .clearance = { 0, 0, -16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Down25Deg },
    };

    static constexpr SequenceDescriptor kRightEighthToDiagDown25Seq1 = {
        .clearance = { -32, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightEighthToDiagDown25Seq2 = {
        .clearance = { -32, 32, -48, 16, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kRightEighthToDiagDown25Seq3 = {
        .clearance = { -64, 0, -48, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kRightEighthToDiagDown25Seq4 = {
        .clearance = { -64, 32, -48, 16, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalUp25Seq1 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalUp25Seq2 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalUp25Seq3 = {
        .clearance = { -32, 32, 16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalUp25Seq4 = {
        .clearance = { -64, 32, 32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalUp25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0100,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalUp25Seq3 = {
        .clearance = { -32, 32, 16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::NwSe },
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalUp25Seq4 = {
        .clearance = { -32, 64, 32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::NwSe },
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalDown25Seq0 = {
        .clearance = { 0, 0, -16, 24, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Down25Deg },
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalDown25Seq1 = {
        .clearance = { -32, 0, -16, 16, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalDown25Seq2 = {
        .clearance = { 0, 32, -16, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalDown25Seq3 = {
        .clearance = { -32, 32, -32, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalDown25Seq4 = {
        .clearance = { -64, 32, -48, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalDown25Seq0 = {
        .clearance = { 0, 0, -16, 24, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Down25Deg },
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalDown25Seq1 = {
        .clearance = { 0, 32, -16, 16, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0100,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalDown25Seq2 = {
        .clearance = { -32, 0, -16, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner1 },
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalDown25Seq3 = {
        .clearance = { -32, 32, -32, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalDown25Seq4 = {
        .clearance = { -32, 64, -48, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
    };

    static constexpr SequenceDescriptor kDiagUp25ToLeftBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagUp25ToLeftBankedUp25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagUp25ToLeftBankedUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagUp25ToLeftBankedUp25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagUp25ToRightBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagUp25ToRightBankedUp25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagUp25ToRightBankedUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagUp25ToRightBankedUp25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToUp25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToUp25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToUp25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToUp25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagDown25ToLeftBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagDown25ToLeftBankedDown25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagDown25ToLeftBankedDown25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagDown25ToLeftBankedDown25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagDown25ToRightBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagDown25ToRightBankedDown25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagDown25ToRightBankedDown25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagDown25ToRightBankedDown25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToDown25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToDown25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToDown25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToDown25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToDown25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToDown25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankedFlatToLeftBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankedFlatToLeftBankedUp25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankedFlatToLeftBankedUp25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankedFlatToLeftBankedUp25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankedFlatToRightBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankedFlatToRightBankedUp25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankedFlatToRightBankedUp25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagRightBankedFlatToRightBankedUp25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToLeftBankedFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToLeftBankedFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToLeftBankedFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToLeftBankedFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToRightBankedFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToRightBankedFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToRightBankedFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToRightBankedFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankedFlatToLeftBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankedFlatToLeftBankedDown25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankedFlatToLeftBankedDown25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankedFlatToLeftBankedDown25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankedFlatToRightBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankedFlatToRightBankedDown25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankedFlatToRightBankedDown25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagRightBankedFlatToRightBankedDown25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToLeftBankedFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToLeftBankedFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToLeftBankedFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToLeftBankedFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToRightBankedFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToRightBankedFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToRightBankedFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToRightBankedFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankedUp25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankedUp25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankedUp25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankedUp25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankedUp25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankedUp25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankedDown25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankedDown25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankedDown25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankedDown25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankedDown25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankedDown25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagUp25LeftBankedSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagUp25LeftBankedSeq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagUp25LeftBankedSeq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagUp25LeftBankedSeq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagUp25RightBankedSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagUp25RightBankedSeq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagUp25RightBankedSeq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagUp25RightBankedSeq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagDown25LeftBankedSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagDown25LeftBankedSeq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagDown25LeftBankedSeq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagDown25LeftBankedSeq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagDown25RightBankedSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagDown25RightBankedSeq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagDown25RightBankedSeq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagDown25RightBankedSeq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up25Deg },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagUp25Seq1 = {
        .clearance = { -32, 0, 16, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagUp25Seq2 = {
        .clearance = { -32, -32, 32, 16, { 0b1000, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::Corner1 },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagUp25Seq3 = {
        .clearance = { -64, 0, 32, 16, { 0b0010, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagUp25Seq4 = {
        .clearance = { -64, -32, 32, 24, { 0b0001, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Up25Deg },
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagUp25Seq1 = {
        .clearance = { -32, 0, 16, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagUp25Seq2 = {
        .clearance = { -32, 32, 32, 16, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagUp25Seq3 = {
        .clearance = { -64, 0, 32, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagUp25Seq4 = {
        .clearance = { -64, 32, 32, 24, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagDown25Seq0 = {
        .clearance = { 0, 0, -16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Down25Deg },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagDown25Seq1 = {
        .clearance = { -32, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagDown25Seq2 = {
        .clearance = { -32, -32, -48, 16, { 0b1000, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::Corner1 },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagDown25Seq3 = {
        .clearance = { -64, 0, -48, 16, { 0b0010, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagDown25Seq4 = {
        .clearance = { -64, -32, -48, 16, { 0b0001, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagDown25Seq0 = {
        .clearance = { 0, 0, -16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Down25Deg },
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagDown25Seq1 = {
        .clearance = { -32, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagDown25Seq2 = {
        .clearance = { -32, 32, -48, 16, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagDown25Seq3 = {
        .clearance = { -64, 0, -48, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagDown25Seq4 = {
        .clearance = { -64, 32, -48, 16, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalUp25Seq1 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalUp25Seq2 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalUp25Seq3 = {
        .clearance = { -32, 32, 16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalUp25Seq4 = {
        .clearance = { -64, 32, 32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalUp25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0100,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalUp25Seq3 = {
        .clearance = { -32, 32, 16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::NwSe },
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalUp25Seq4 = {
        .clearance = { -32, 64, 32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::NwSe },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalDown25Seq0 = {
        .clearance = { 0, 0, -16, 24, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Down25Deg },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalDown25Seq1 = {
        .clearance = { -32, 0, -16, 16, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalDown25Seq2 = {
        .clearance = { 0, 32, -16, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalDown25Seq3 = {
        .clearance = { -32, 32, -32, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalDown25Seq4 = {
        .clearance = { -64, 32, -48, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalDown25Seq0 = {
        .clearance = { 0, 0, -16, 24, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::NeSw, WoodenSupportTransitionType::Down25Deg },
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalDown25Seq1 = {
        .clearance = { 0, 32, -16, 16, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0100,
        .woodenSupports = { WoodenSupportSubType::NeSw },
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalDown25Seq2 = {
        .clearance = { -32, 0, -16, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner1 },
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalDown25Seq3 = {
        .clearance = { -32, 32, -32, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::Corner3 },
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalDown25Seq4 = {
        .clearance = { -32, 64, -48, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
    };

    static constexpr SequenceDescriptor kDiagBrakesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagBrakesSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagBrakesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagBrakesSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagBlockBrakesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagBlockBrakesSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::Corner0 },
    };

    static constexpr SequenceDescriptor kDiagBlockBrakesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::Corner2 },
    };

    static constexpr SequenceDescriptor kDiagBlockBrakesSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, 0 },
    };

    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
        .allowedWallEdges = 0b0000,
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq3 = {
        .clearance = { -32, 32, 0, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0000,
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq4 = {
        .clearance = { -32, 64, 8, 40, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq5 = {
        .clearance = { -64, 32, 8, 40, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq6 = {
        .clearance = { -64, 64, 8, 64, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0000,
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq7 = {
        .clearance = { -64, 96, 40, 88, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq8 = {
        .clearance = { -96, 64, 40, 88, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq9 = {
        .clearance = { -96, 96, 40, 104, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0000,
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1101, 0 }, 0 },
        .allowedWallEdges = 0b0000,
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq1 = {
        .clearance = { 0, 32, 0, 72, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq2 = {
        .clearance = { -32, 0, 0, 72, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq3 = {
        .clearance = { -32, 32, 0, 80, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0000,
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq4 = {
        .clearance = { -32, 64, 56, 32, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq5 = {
        .clearance = { -64, 32, 56, 32, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq6 = {
        .clearance = { -64, 64, 56, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0000,
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq7 = {
        .clearance = { -64, 96, 80, 8, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq8 = {
        .clearance = { -96, 64, 80, 8, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq9 = {
        .clearance = { -96, 96, 80, 8, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0000,
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq0 = {
        .clearance = { 0, 0, 80, 8, { 0b1101, 0 }, 0 },
        .allowedWallEdges = 0b0000,
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq1 = {
        .clearance = { 0, 32, 80, 8, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq2 = {
        .clearance = { -32, 0, 80, 8, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq3 = {
        .clearance = { -32, 32, 56, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0000,
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq4 = {
        .clearance = { -32, 64, 56, 32, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq5 = {
        .clearance = { -64, 32, 56, 32, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq6 = {
        .clearance = { -64, 64, 0, 80, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0000,
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq7 = {
        .clearance = { -64, 96, 0, 72, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq8 = {
        .clearance = { -96, 64, 0, 72, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq9 = {
        .clearance = { -96, 96, 0, 64, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0000,
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq0 = {
        .clearance = { 0, 0, 40, 104, { 0b1101, 0 }, 0 },
        .allowedWallEdges = 0b0000,
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq1 = {
        .clearance = { 0, 32, 40, 88, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq2 = {
        .clearance = { -32, 0, 40, 88, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq3 = {
        .clearance = { -32, 32, 8, 64, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0000,
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq4 = {
        .clearance = { -32, 64, 8, 40, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq5 = {
        .clearance = { -64, 32, 8, 40, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq6 = {
        .clearance = { -64, 64, 0, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0000,
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq7 = {
        .clearance = { -64, 96, 0, 16, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq8 = {
        .clearance = { -96, 64, 0, 16, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq9 = {
        .clearance = { -96, 96, 0, 0, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0000,
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopUpToOrthogonalSeq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1101, 0 }, 0 },
        .allowedWallEdges = 0b0000,
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopUpToOrthogonalSeq1 = {
        .clearance = { 0, 32, 0, 88, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopUpToOrthogonalSeq2 = {
        .clearance = { -32, 0, 0, 88, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopUpToOrthogonalSeq3 = {
        .clearance = { -32, 32, 32, 80, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopUpToOrthogonalSeq4 = {
        .clearance = { -64, 32, 80, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopUpToOrthogonalSeq5 = {
        .clearance = { -96, 32, 112, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopUpToOrthogonalSeq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1101, 0 }, 0 },
        .allowedWallEdges = 0b0000,
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopUpToOrthogonalSeq1 = {
        .clearance = { -32, 0, 0, 88, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopUpToOrthogonalSeq2 = {
        .clearance = { 0, 32, 0, 88, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopUpToOrthogonalSeq3 = {
        .clearance = { -32, 32, 32, 80, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopUpToOrthogonalSeq4 = {
        .clearance = { -32, 64, 80, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopUpToOrthogonalSeq5 = {
        .clearance = { -32, 96, 112, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopDownToDiagSeq0 = {
        .clearance = { 0, 0, 112, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopDownToDiagSeq1 = {
        .clearance = { -32, 0, 80, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopDownToDiagSeq2 = {
        .clearance = { -64, 0, 32, 80, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopDownToDiagSeq3 = {
        .clearance = { -64, -32, 0, 88, { 0b1000, 0 }, 0 },
        .allowedWallEdges = 0b1100,
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopDownToDiagSeq4 = {
        .clearance = { -96, 0, 0, 88, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopDownToDiagSeq5 = {
        .clearance = { -96, -32, 0, 64, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b0000,
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopDownToDiagSeq0 = {
        .clearance = { 0, 0, 112, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopDownToDiagSeq1 = {
        .clearance = { -32, 0, 80, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopDownToDiagSeq2 = {
        .clearance = { -64, 0, 32, 80, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopDownToDiagSeq3 = {
        .clearance = { -64, 32, 0, 88, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopDownToDiagSeq4 = {
        .clearance = { -96, 0, 0, 88, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopDownToDiagSeq5 = {
        .clearance = { -96, 32, 0, 64, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0000,
    };

    static constexpr SequenceData kSequenceDescriptorsByElement[] = {
        /* TrackElemType::Flat */ { 1, { kFlatSeq0 } },
        /* TrackElemType::EndStation */ { 1, { kEndStationSeq0 } },
        /* TrackElemType::BeginStation */ { 1, { kBeginStationSeq0 } },
        /* TrackElemType::MiddleStation */ { 1, { kMiddleStationSeq0 } },
        /* TrackElemType::Up25 */ { 1, { kUp25Seq0 } },
        /* TrackElemType::Up60 */ { 1, { kUp60Seq0 } },
        /* TrackElemType::FlatToUp25 */ { 1, { kFlatToUp25Seq0 } },
        /* TrackElemType::Up25ToUp60 */ { 1, { kUp25ToUp60Seq0 } },
        /* TrackElemType::Up60ToUp25 */ { 1, { kUp60ToUp25Seq0 } },
        /* TrackElemType::Up25ToFlat */ { 1, { kUp25ToFlatSeq0 } },
        /* TrackElemType::Down25 */ { 1, { kDown25Seq0 } },
        /* TrackElemType::Down60 */ { 1, { kDown60Seq0 } },
        /* TrackElemType::FlatToDown25 */ { 1, { kFlatToDown25Seq0 } },
        /* TrackElemType::Down25ToDown60 */ { 1, { kDown25ToDown60Seq0 } },
        /* TrackElemType::Down60ToDown25 */ { 1, { kDown60ToDown25Seq0 } },
        /* TrackElemType::Down25ToFlat */ { 1, { kDown25ToFlatSeq0 } },
        /* TrackElemType::LeftQuarterTurn5Tiles */
        { 7,
          { kLeftQuarterTurn5TilesSeq0, kLeftQuarterTurn5TilesSeq1, kLeftQuarterTurn5TilesSeq2, kLeftQuarterTurn5TilesSeq3,
            kLeftQuarterTurn5TilesSeq4, kLeftQuarterTurn5TilesSeq5, kLeftQuarterTurn5TilesSeq6 } },
        /* TrackElemType::RightQuarterTurn5Tiles */
        { 7,
          { kRightQuarterTurn5TilesSeq0, kRightQuarterTurn5TilesSeq1, kRightQuarterTurn5TilesSeq2, kRightQuarterTurn5TilesSeq3,
            kRightQuarterTurn5TilesSeq4, kRightQuarterTurn5TilesSeq5, kRightQuarterTurn5TilesSeq6 } },
        /* TrackElemType::FlatToLeftBank */ { 1, { kFlatToLeftBankSeq0 } },
        /* TrackElemType::FlatToRightBank */ { 1, { kFlatToRightBankSeq0 } },
        /* TrackElemType::LeftBankToFlat */ { 1, { kLeftBankToFlatSeq0 } },
        /* TrackElemType::RightBankToFlat */ { 1, { kRightBankToFlatSeq0 } },
        /* TrackElemType::BankedLeftQuarterTurn5Tiles */
        { 7,
          { kBankedLeftQuarterTurn5TilesSeq0, kBankedLeftQuarterTurn5TilesSeq1, kBankedLeftQuarterTurn5TilesSeq2,
            kBankedLeftQuarterTurn5TilesSeq3, kBankedLeftQuarterTurn5TilesSeq4, kBankedLeftQuarterTurn5TilesSeq5,
            kBankedLeftQuarterTurn5TilesSeq6 } },
        /* TrackElemType::BankedRightQuarterTurn5Tiles */
        { 7,
          { kBankedRightQuarterTurn5TilesSeq0, kBankedRightQuarterTurn5TilesSeq1, kBankedRightQuarterTurn5TilesSeq2,
            kBankedRightQuarterTurn5TilesSeq3, kBankedRightQuarterTurn5TilesSeq4, kBankedRightQuarterTurn5TilesSeq5,
            kBankedRightQuarterTurn5TilesSeq6 } },
        /* TrackElemType::LeftBankToUp25 */ { 1, { kLeftBankToUp25Seq0 } },
        /* TrackElemType::RightBankToUp25 */ { 1, { kRightBankToUp25Seq0 } },
        /* TrackElemType::Up25ToLeftBank */ { 1, { kUp25ToLeftBankSeq0 } },
        /* TrackElemType::Up25ToRightBank */ { 1, { kUp25ToRightBankSeq0 } },
        /* TrackElemType::LeftBankToDown25 */ { 1, { kLeftBankToDown25Seq0 } },
        /* TrackElemType::RightBankToDown25 */ { 1, { kRightBankToDown25Seq0 } },
        /* TrackElemType::Down25ToLeftBank */ { 1, { kDown25ToLeftBankSeq0 } },
        /* TrackElemType::Down25ToRightBank */ { 1, { kDown25ToRightBankSeq0 } },
        /* TrackElemType::LeftBank */ { 1, { kLeftBankSeq0 } },
        /* TrackElemType::RightBank */ { 1, { kRightBankSeq0 } },
        /* TrackElemType::LeftQuarterTurn5TilesUp25 */
        { 7,
          { kLeftQuarterTurn5TilesUp25Seq0, kLeftQuarterTurn5TilesUp25Seq1, kLeftQuarterTurn5TilesUp25Seq2,
            kLeftQuarterTurn5TilesUp25Seq3, kLeftQuarterTurn5TilesUp25Seq4, kLeftQuarterTurn5TilesUp25Seq5,
            kLeftQuarterTurn5TilesUp25Seq6 } },
        /* TrackElemType::RightQuarterTurn5TilesUp25 */
        { 7,
          { kRightQuarterTurn5TilesUp25Seq0, kRightQuarterTurn5TilesUp25Seq1, kRightQuarterTurn5TilesUp25Seq2,
            kRightQuarterTurn5TilesUp25Seq3, kRightQuarterTurn5TilesUp25Seq4, kRightQuarterTurn5TilesUp25Seq5,
            kRightQuarterTurn5TilesUp25Seq6 } },
        /* TrackElemType::LeftQuarterTurn5TilesDown25 */
        { 7,
          { kLeftQuarterTurn5TilesDown25Seq0, kLeftQuarterTurn5TilesDown25Seq1, kLeftQuarterTurn5TilesDown25Seq2,
            kLeftQuarterTurn5TilesDown25Seq3, kLeftQuarterTurn5TilesDown25Seq4, kLeftQuarterTurn5TilesDown25Seq5,
            kLeftQuarterTurn5TilesDown25Seq6 } },
        /* TrackElemType::RightQuarterTurn5TilesDown25 */
        { 7,
          { kRightQuarterTurn5TilesDown25Seq0, kRightQuarterTurn5TilesDown25Seq1, kRightQuarterTurn5TilesDown25Seq2,
            kRightQuarterTurn5TilesDown25Seq3, kRightQuarterTurn5TilesDown25Seq4, kRightQuarterTurn5TilesDown25Seq5,
            kRightQuarterTurn5TilesDown25Seq6 } },
        /* TrackElemType::SBendLeft */ { 4, { kSBendLeftSeq0, kSBendLeftSeq1, kSBendLeftSeq2, kSBendLeftSeq3 } },
        /* TrackElemType::SBendRight */ { 4, { kSBendRightSeq0, kSBendRightSeq1, kSBendRightSeq2, kSBendRightSeq3 } },
        /* TrackElemType::LeftVerticalLoop */
        { 10,
          { kLeftVerticalLoopSeq0, kLeftVerticalLoopSeq1, kLeftVerticalLoopSeq2, kLeftVerticalLoopSeq3, kLeftVerticalLoopSeq4,
            kLeftVerticalLoopSeq5, kLeftVerticalLoopSeq6, kLeftVerticalLoopSeq7, kLeftVerticalLoopSeq8,
            kLeftVerticalLoopSeq9 } },
        /* TrackElemType::RightVerticalLoop */
        { 10,
          { kRightVerticalLoopSeq0, kRightVerticalLoopSeq1, kRightVerticalLoopSeq2, kRightVerticalLoopSeq3,
            kRightVerticalLoopSeq4, kRightVerticalLoopSeq5, kRightVerticalLoopSeq6, kRightVerticalLoopSeq7,
            kRightVerticalLoopSeq8, kRightVerticalLoopSeq9 } },
        /* TrackElemType::LeftQuarterTurn3Tiles */
        { 4,
          { kLeftQuarterTurn3TilesSeq0, kLeftQuarterTurn3TilesSeq1, kLeftQuarterTurn3TilesSeq2, kLeftQuarterTurn3TilesSeq3 } },
        /* TrackElemType::RightQuarterTurn3Tiles */
        { 4,
          { kRightQuarterTurn3TilesSeq0, kRightQuarterTurn3TilesSeq1, kRightQuarterTurn3TilesSeq2,
            kRightQuarterTurn3TilesSeq3 } },
        /* TrackElemType::LeftBankedQuarterTurn3Tiles */
        { 4,
          { kLeftBankedQuarterTurn3TilesSeq0, kLeftBankedQuarterTurn3TilesSeq1, kLeftBankedQuarterTurn3TilesSeq2,
            kLeftBankedQuarterTurn3TilesSeq3 } },
        /* TrackElemType::RightBankedQuarterTurn3Tiles */
        { 4,
          { kRightBankedQuarterTurn3TilesSeq0, kRightBankedQuarterTurn3TilesSeq1, kRightBankedQuarterTurn3TilesSeq2,
            kRightBankedQuarterTurn3TilesSeq3 } },
        /* TrackElemType::LeftQuarterTurn3TilesUp25 */
        { 4,
          { kLeftQuarterTurn3TilesUp25Seq0, kLeftQuarterTurn3TilesUp25Seq1, kLeftQuarterTurn3TilesUp25Seq2,
            kLeftQuarterTurn3TilesUp25Seq3 } },
        /* TrackElemType::RightQuarterTurn3TilesUp25 */
        { 4,
          { kRightQuarterTurn3TilesUp25Seq0, kRightQuarterTurn3TilesUp25Seq1, kRightQuarterTurn3TilesUp25Seq2,
            kRightQuarterTurn3TilesUp25Seq3 } },
        /* TrackElemType::LeftQuarterTurn3TilesDown25 */
        { 4,
          { kLeftQuarterTurn3TilesDown25Seq0, kLeftQuarterTurn3TilesDown25Seq1, kLeftQuarterTurn3TilesDown25Seq2,
            kLeftQuarterTurn3TilesDown25Seq3 } },
        /* TrackElemType::RightQuarterTurn3TilesDown25 */
        { 4,
          { kRightQuarterTurn3TilesDown25Seq0, kRightQuarterTurn3TilesDown25Seq1, kRightQuarterTurn3TilesDown25Seq2,
            kRightQuarterTurn3TilesDown25Seq3 } },
        /* TrackElemType::LeftQuarterTurn1Tile */ { 1, { kLeftQuarterTurn1TileSeq0 } },
        /* TrackElemType::RightQuarterTurn1Tile */ { 1, { kRightQuarterTurn1TileSeq0 } },
        /* TrackElemType::LeftTwistDownToUp */
        { 3, { kLeftTwistDownToUpSeq0, kLeftTwistDownToUpSeq1, kLeftTwistDownToUpSeq2 } },
        /* TrackElemType::RightTwistDownToUp */
        { 3, { kRightTwistDownToUpSeq0, kRightTwistDownToUpSeq1, kRightTwistDownToUpSeq2 } },
        /* TrackElemType::LeftTwistUpToDown */
        { 3, { kLeftTwistUpToDownSeq0, kLeftTwistUpToDownSeq1, kLeftTwistUpToDownSeq2 } },
        /* TrackElemType::RightTwistUpToDown */
        { 3, { kRightTwistUpToDownSeq0, kRightTwistUpToDownSeq1, kRightTwistUpToDownSeq2 } },
        /* TrackElemType::HalfLoopUp */ { 4, { kHalfLoopUpSeq0, kHalfLoopUpSeq1, kHalfLoopUpSeq2, kHalfLoopUpSeq3 } },
        /* TrackElemType::HalfLoopDown */ { 4, { kHalfLoopDownSeq0, kHalfLoopDownSeq1, kHalfLoopDownSeq2, kHalfLoopDownSeq3 } },
        /* TrackElemType::LeftCorkscrewUp */ { 3, { kLeftCorkscrewUpSeq0, kLeftCorkscrewUpSeq1, kLeftCorkscrewUpSeq2 } },
        /* TrackElemType::RightCorkscrewUp */ { 3, { kRightCorkscrewUpSeq0, kRightCorkscrewUpSeq1, kRightCorkscrewUpSeq2 } },
        /* TrackElemType::LeftCorkscrewDown */
        { 3, { kLeftCorkscrewDownSeq0, kLeftCorkscrewDownSeq1, kLeftCorkscrewDownSeq2 } },
        /* TrackElemType::RightCorkscrewDown */
        { 3, { kRightCorkscrewDownSeq0, kRightCorkscrewDownSeq1, kRightCorkscrewDownSeq2 } },
        /* TrackElemType::FlatToUp60 */ { 1, { kFlatToUp60Seq0 } },
        /* TrackElemType::Up60ToFlat */ { 1, { kUp60ToFlatSeq0 } },
        /* TrackElemType::FlatToDown60 */ { 1, { kFlatToDown60Seq0 } },
        /* TrackElemType::Down60ToFlat */ { 1, { kDown60ToFlatSeq0 } },
        /* TrackElemType::TowerBase */
        { 9,
          { kTowerBaseSeq0, kTowerBaseSeq1, kTowerBaseSeq2, kTowerBaseSeq3, kTowerBaseSeq4, kTowerBaseSeq5, kTowerBaseSeq6,
            kTowerBaseSeq7, kTowerBaseSeq8 } },
        /* TrackElemType::TowerSection */ { 2, { kTowerSectionSeq0, kTowerSectionSeq1 } },
        /* TrackElemType::FlatCovered */ { 1, { kFlatCoveredSeq0 } },
        /* TrackElemType::Up25Covered */ { 1, { kUp25CoveredSeq0 } },
        /* TrackElemType::Up60Covered */ { 1, { kUp60CoveredSeq0 } },
        /* TrackElemType::FlatToUp25Covered */ { 1, { kFlatToUp25CoveredSeq0 } },
        /* TrackElemType::Up25ToUp60Covered */ { 1, { kUp25ToUp60CoveredSeq0 } },
        /* TrackElemType::Up60ToUp25Covered */ { 1, { kUp60ToUp25CoveredSeq0 } },
        /* TrackElemType::Up25ToFlatCovered */ { 1, { kUp25ToFlatCoveredSeq0 } },
        /* TrackElemType::Down25Covered */ { 1, { kDown25CoveredSeq0 } },
        /* TrackElemType::Down60Covered */ { 1, { kDown60CoveredSeq0 } },
        /* TrackElemType::FlatToDown25Covered */ { 1, { kFlatToDown25CoveredSeq0 } },
        /* TrackElemType::Down25ToDown60Covered */ { 1, { kDown25ToDown60CoveredSeq0 } },
        /* TrackElemType::Down60ToDown25Covered */ { 1, { kDown60ToDown25CoveredSeq0 } },
        /* TrackElemType::Down25ToFlatCovered */ { 1, { kDown25ToFlatCoveredSeq0 } },
        /* TrackElemType::LeftQuarterTurn5TilesCovered */
        { 7,
          { kLeftQuarterTurn5TilesCoveredSeq0, kLeftQuarterTurn5TilesCoveredSeq1, kLeftQuarterTurn5TilesCoveredSeq2,
            kLeftQuarterTurn5TilesCoveredSeq3, kLeftQuarterTurn5TilesCoveredSeq4, kLeftQuarterTurn5TilesCoveredSeq5,
            kLeftQuarterTurn5TilesCoveredSeq6 } },
        /* TrackElemType::RightQuarterTurn5TilesCovered */
        { 7,
          { kRightQuarterTurn5TilesCoveredSeq0, kRightQuarterTurn5TilesCoveredSeq1, kRightQuarterTurn5TilesCoveredSeq2,
            kRightQuarterTurn5TilesCoveredSeq3, kRightQuarterTurn5TilesCoveredSeq4, kRightQuarterTurn5TilesCoveredSeq5,
            kRightQuarterTurn5TilesCoveredSeq6 } },
        /* TrackElemType::SBendLeftCovered */
        { 4, { kSBendLeftCoveredSeq0, kSBendLeftCoveredSeq1, kSBendLeftCoveredSeq2, kSBendLeftCoveredSeq3 } },
        /* TrackElemType::SBendRightCovered */
        { 4, { kSBendRightCoveredSeq0, kSBendRightCoveredSeq1, kSBendRightCoveredSeq2, kSBendRightCoveredSeq3 } },
        /* TrackElemType::LeftQuarterTurn3TilesCovered */
        { 4,
          { kLeftQuarterTurn3TilesCoveredSeq0, kLeftQuarterTurn3TilesCoveredSeq1, kLeftQuarterTurn3TilesCoveredSeq2,
            kLeftQuarterTurn3TilesCoveredSeq3 } },
        /* TrackElemType::RightQuarterTurn3TilesCovered */
        { 4,
          { kRightQuarterTurn3TilesCoveredSeq0, kRightQuarterTurn3TilesCoveredSeq1, kRightQuarterTurn3TilesCoveredSeq2,
            kRightQuarterTurn3TilesCoveredSeq3 } },
        /* TrackElemType::LeftHalfBankedHelixUpSmall */
        { 8,
          { kLeftHalfBankedHelixUpSmallSeq0, kLeftHalfBankedHelixUpSmallSeq1, kLeftHalfBankedHelixUpSmallSeq2,
            kLeftHalfBankedHelixUpSmallSeq3, kLeftHalfBankedHelixUpSmallSeq4, kLeftHalfBankedHelixUpSmallSeq5,
            kLeftHalfBankedHelixUpSmallSeq6, kLeftHalfBankedHelixUpSmallSeq7 } },
        /* TrackElemType::RightHalfBankedHelixUpSmall */
        { 8,
          { kRightHalfBankedHelixUpSmallSeq0, kRightHalfBankedHelixUpSmallSeq1, kRightHalfBankedHelixUpSmallSeq2,
            kRightHalfBankedHelixUpSmallSeq3, kRightHalfBankedHelixUpSmallSeq4, kRightHalfBankedHelixUpSmallSeq5,
            kRightHalfBankedHelixUpSmallSeq6, kRightHalfBankedHelixUpSmallSeq7 } },
        /* TrackElemType::LeftHalfBankedHelixDownSmall */
        { 8,
          { kLeftHalfBankedHelixDownSmallSeq0, kLeftHalfBankedHelixDownSmallSeq1, kLeftHalfBankedHelixDownSmallSeq2,
            kLeftHalfBankedHelixDownSmallSeq3, kLeftHalfBankedHelixDownSmallSeq4, kLeftHalfBankedHelixDownSmallSeq5,
            kLeftHalfBankedHelixDownSmallSeq6, kLeftHalfBankedHelixDownSmallSeq7 } },
        /* TrackElemType::RightHalfBankedHelixDownSmall */
        { 8,
          { kRightHalfBankedHelixDownSmallSeq0, kRightHalfBankedHelixDownSmallSeq1, kRightHalfBankedHelixDownSmallSeq2,
            kRightHalfBankedHelixDownSmallSeq3, kRightHalfBankedHelixDownSmallSeq4, kRightHalfBankedHelixDownSmallSeq5,
            kRightHalfBankedHelixDownSmallSeq6, kRightHalfBankedHelixDownSmallSeq7 } },
        /* TrackElemType::LeftHalfBankedHelixUpLarge */
        { 14,
          { kLeftHalfBankedHelixUpLargeSeq0, kLeftHalfBankedHelixUpLargeSeq1, kLeftHalfBankedHelixUpLargeSeq2,
            kLeftHalfBankedHelixUpLargeSeq3, kLeftHalfBankedHelixUpLargeSeq4, kLeftHalfBankedHelixUpLargeSeq5,
            kLeftHalfBankedHelixUpLargeSeq6, kLeftHalfBankedHelixUpLargeSeq7, kLeftHalfBankedHelixUpLargeSeq8,
            kLeftHalfBankedHelixUpLargeSeq9, kLeftHalfBankedHelixUpLargeSeq10, kLeftHalfBankedHelixUpLargeSeq11,
            kLeftHalfBankedHelixUpLargeSeq12, kLeftHalfBankedHelixUpLargeSeq13 } },
        /* TrackElemType::RightHalfBankedHelixUpLarge */
        { 14,
          { kRightHalfBankedHelixUpLargeSeq0, kRightHalfBankedHelixUpLargeSeq1, kRightHalfBankedHelixUpLargeSeq2,
            kRightHalfBankedHelixUpLargeSeq3, kRightHalfBankedHelixUpLargeSeq4, kRightHalfBankedHelixUpLargeSeq5,
            kRightHalfBankedHelixUpLargeSeq6, kRightHalfBankedHelixUpLargeSeq7, kRightHalfBankedHelixUpLargeSeq8,
            kRightHalfBankedHelixUpLargeSeq9, kRightHalfBankedHelixUpLargeSeq10, kRightHalfBankedHelixUpLargeSeq11,
            kRightHalfBankedHelixUpLargeSeq12, kRightHalfBankedHelixUpLargeSeq13 } },
        /* TrackElemType::LeftHalfBankedHelixDownLarge */
        { 14,
          { kLeftHalfBankedHelixDownLargeSeq0, kLeftHalfBankedHelixDownLargeSeq1, kLeftHalfBankedHelixDownLargeSeq2,
            kLeftHalfBankedHelixDownLargeSeq3, kLeftHalfBankedHelixDownLargeSeq4, kLeftHalfBankedHelixDownLargeSeq5,
            kLeftHalfBankedHelixDownLargeSeq6, kLeftHalfBankedHelixDownLargeSeq7, kLeftHalfBankedHelixDownLargeSeq8,
            kLeftHalfBankedHelixDownLargeSeq9, kLeftHalfBankedHelixDownLargeSeq10, kLeftHalfBankedHelixDownLargeSeq11,
            kLeftHalfBankedHelixDownLargeSeq12, kLeftHalfBankedHelixDownLargeSeq13 } },
        /* TrackElemType::RightHalfBankedHelixDownLarge */
        { 14,
          { kRightHalfBankedHelixDownLargeSeq0, kRightHalfBankedHelixDownLargeSeq1, kRightHalfBankedHelixDownLargeSeq2,
            kRightHalfBankedHelixDownLargeSeq3, kRightHalfBankedHelixDownLargeSeq4, kRightHalfBankedHelixDownLargeSeq5,
            kRightHalfBankedHelixDownLargeSeq6, kRightHalfBankedHelixDownLargeSeq7, kRightHalfBankedHelixDownLargeSeq8,
            kRightHalfBankedHelixDownLargeSeq9, kRightHalfBankedHelixDownLargeSeq10, kRightHalfBankedHelixDownLargeSeq11,
            kRightHalfBankedHelixDownLargeSeq12, kRightHalfBankedHelixDownLargeSeq13 } },
        /* TrackElemType::LeftQuarterTurn1TileUp60 */ { 1, { kLeftQuarterTurn1TileUp60Seq0 } },
        /* TrackElemType::RightQuarterTurn1TileUp60 */ { 1, { kRightQuarterTurn1TileUp60Seq0 } },
        /* TrackElemType::LeftQuarterTurn1TileDown60 */ { 1, { kLeftQuarterTurn1TileDown60Seq0 } },
        /* TrackElemType::RightQuarterTurn1TileDown60 */ { 1, { kRightQuarterTurn1TileDown60Seq0 } },
        /* TrackElemType::Brakes */ { 1, { kBrakesSeq0 } },
        /* TrackElemType::Booster */ { 1, { kBoosterSeq0 } },
        /* TrackElemType::Maze */
        { 1,
          { kMazeSeq0, kMazeSeq1, kMazeSeq2, kMazeSeq3, kMazeSeq4, kMazeSeq5, kMazeSeq6, kMazeSeq7, kMazeSeq8, kMazeSeq9,
            kMazeSeq10, kMazeSeq11, kMazeSeq12, kMazeSeq13, kMazeSeq14, kMazeSeq15 } },
        /* TrackElemType::LeftQuarterBankedHelixLargeUp */
        { 7,
          { kLeftQuarterBankedHelixLargeUpSeq0, kLeftQuarterBankedHelixLargeUpSeq1, kLeftQuarterBankedHelixLargeUpSeq2,
            kLeftQuarterBankedHelixLargeUpSeq3, kLeftQuarterBankedHelixLargeUpSeq4, kLeftQuarterBankedHelixLargeUpSeq5,
            kLeftQuarterBankedHelixLargeUpSeq6 } },
        /* TrackElemType::RightQuarterBankedHelixLargeUp */
        { 7,
          { kRightQuarterBankedHelixLargeUpSeq0, kRightQuarterBankedHelixLargeUpSeq1, kRightQuarterBankedHelixLargeUpSeq2,
            kRightQuarterBankedHelixLargeUpSeq3, kRightQuarterBankedHelixLargeUpSeq4, kRightQuarterBankedHelixLargeUpSeq5,
            kRightQuarterBankedHelixLargeUpSeq6 } },
        /* TrackElemType::LeftQuarterBankedHelixLargeDown */
        { 7,
          { kLeftQuarterBankedHelixLargeDownSeq0, kLeftQuarterBankedHelixLargeDownSeq1, kLeftQuarterBankedHelixLargeDownSeq2,
            kLeftQuarterBankedHelixLargeDownSeq3, kLeftQuarterBankedHelixLargeDownSeq4, kLeftQuarterBankedHelixLargeDownSeq5,
            kLeftQuarterBankedHelixLargeDownSeq6 } },
        /* TrackElemType::RightQuarterBankedHelixLargeDown */
        { 7,
          { kRightQuarterBankedHelixLargeDownSeq0, kRightQuarterBankedHelixLargeDownSeq1, kRightQuarterBankedHelixLargeDownSeq2,
            kRightQuarterBankedHelixLargeDownSeq3, kRightQuarterBankedHelixLargeDownSeq4, kRightQuarterBankedHelixLargeDownSeq5,
            kRightQuarterBankedHelixLargeDownSeq6 } },
        /* TrackElemType::LeftQuarterHelixLargeUp */
        { 7,
          { kLeftQuarterHelixLargeUpSeq0, kLeftQuarterHelixLargeUpSeq1, kLeftQuarterHelixLargeUpSeq2,
            kLeftQuarterHelixLargeUpSeq3, kLeftQuarterHelixLargeUpSeq4, kLeftQuarterHelixLargeUpSeq5,
            kLeftQuarterHelixLargeUpSeq6 } },
        /* TrackElemType::RightQuarterHelixLargeUp */
        { 7,
          { kRightQuarterHelixLargeUpSeq0, kRightQuarterHelixLargeUpSeq1, kRightQuarterHelixLargeUpSeq2,
            kRightQuarterHelixLargeUpSeq3, kRightQuarterHelixLargeUpSeq4, kRightQuarterHelixLargeUpSeq5,
            kRightQuarterHelixLargeUpSeq6 } },
        /* TrackElemType::LeftQuarterHelixLargeDown */
        { 7,
          { kLeftQuarterHelixLargeDownSeq0, kLeftQuarterHelixLargeDownSeq1, kLeftQuarterHelixLargeDownSeq2,
            kLeftQuarterHelixLargeDownSeq3, kLeftQuarterHelixLargeDownSeq4, kLeftQuarterHelixLargeDownSeq5,
            kLeftQuarterHelixLargeDownSeq6 } },
        /* TrackElemType::RightQuarterHelixLargeDown */
        { 7,
          { kRightQuarterHelixLargeDownSeq0, kRightQuarterHelixLargeDownSeq1, kRightQuarterHelixLargeDownSeq2,
            kRightQuarterHelixLargeDownSeq3, kRightQuarterHelixLargeDownSeq4, kRightQuarterHelixLargeDownSeq5,
            kRightQuarterHelixLargeDownSeq6 } },
        /* TrackElemType::Up25LeftBanked */ { 1, { kUp25LeftBankedSeq0 } },
        /* TrackElemType::Up25RightBanked */ { 1, { kUp25RightBankedSeq0 } },
        /* TrackElemType::Waterfall */ { 1, { kWaterfallSeq0 } },
        /* TrackElemType::Rapids */ { 1, { kRapidsSeq0 } },
        /* TrackElemType::OnRidePhoto */ { 1, { kOnRidePhotoSeq0 } },
        /* TrackElemType::Down25LeftBanked */ { 1, { kDown25LeftBankedSeq0 } },
        /* TrackElemType::Down25RightBanked */ { 1, { kDown25RightBankedSeq0 } },
        /* TrackElemType::Watersplash */
        { 5, { kWatersplashSeq0, kWatersplashSeq1, kWatersplashSeq2, kWatersplashSeq3, kWatersplashSeq4 } },
        /* TrackElemType::FlatToUp60LongBase */
        { 4, { kFlatToUp60LongBaseSeq0, kFlatToUp60LongBaseSeq1, kFlatToUp60LongBaseSeq2, kFlatToUp60LongBaseSeq3 } },
        /* TrackElemType::Up60ToFlatLongBase */
        { 4, { kUp60ToFlatLongBaseSeq0, kUp60ToFlatLongBaseSeq1, kUp60ToFlatLongBaseSeq2, kUp60ToFlatLongBaseSeq3 } },
        /* TrackElemType::Whirlpool */ { 1, { kWhirlpoolSeq0 } },
        /* TrackElemType::Down60ToFlatLongBase */
        { 4, { kDown60ToFlatLongBaseSeq0, kDown60ToFlatLongBaseSeq1, kDown60ToFlatLongBaseSeq2, kDown60ToFlatLongBaseSeq3 } },
        /* TrackElemType::FlatToDown60LongBase */
        { 4, { kFlatToDown60LongBaseSeq0, kFlatToDown60LongBaseSeq1, kFlatToDown60LongBaseSeq2, kFlatToDown60LongBaseSeq3 } },
        /* TrackElemType::CableLiftHill */
        { 4, { kCableLiftHillSeq0, kCableLiftHillSeq1, kCableLiftHillSeq2, kCableLiftHillSeq3 } },
        /* TrackElemType::ReverseFreefallSlope */
        { 7,
          { kReverseFreefallSlopeSeq0, kReverseFreefallSlopeSeq1, kReverseFreefallSlopeSeq2, kReverseFreefallSlopeSeq3,
            kReverseFreefallSlopeSeq4, kReverseFreefallSlopeSeq5, kReverseFreefallSlopeSeq6 } },
        /* TrackElemType::ReverseFreefallVertical */ { 2, { kReverseFreefallVerticalSeq0, kReverseFreefallVerticalSeq1 } },
        /* TrackElemType::Up90 */ { 2, { kUp90Seq0, kUp90Seq1 } },
        /* TrackElemType::Down90 */ { 2, { kDown90Seq0, kDown90Seq1 } },
        /* TrackElemType::Up60ToUp90 */ { 2, { kUp60ToUp90Seq0, kUp60ToUp90Seq1 } },
        /* TrackElemType::Down90ToDown60 */ { 1, { kDown90ToDown60Seq0 } },
        /* TrackElemType::Up90ToUp60 */ { 1, { kUp90ToUp60Seq0 } },
        /* TrackElemType::Down60ToDown90 */ { 2, { kDown60ToDown90Seq0, kDown60ToDown90Seq1 } },
        /* TrackElemType::BrakeForDrop */ { 1, { kBrakeForDropSeq0 } },
        /* TrackElemType::LeftEighthToDiag */
        { 5,
          { kLeftEighthToDiagSeq0, kLeftEighthToDiagSeq1, kLeftEighthToDiagSeq2, kLeftEighthToDiagSeq3,
            kLeftEighthToDiagSeq4 } },
        /* TrackElemType::RightEighthToDiag */
        { 5,
          { kRightEighthToDiagSeq0, kRightEighthToDiagSeq1, kRightEighthToDiagSeq2, kRightEighthToDiagSeq3,
            kRightEighthToDiagSeq4 } },
        /* TrackElemType::LeftEighthToOrthogonal */
        { 5,
          { kLeftEighthToOrthogonalSeq0, kLeftEighthToOrthogonalSeq1, kLeftEighthToOrthogonalSeq2, kLeftEighthToOrthogonalSeq3,
            kLeftEighthToOrthogonalSeq4 } },
        /* TrackElemType::RightEighthToOrthogonal */
        { 5,
          { kRightEighthToOrthogonalSeq0, kRightEighthToOrthogonalSeq1, kRightEighthToOrthogonalSeq2,
            kRightEighthToOrthogonalSeq3, kRightEighthToOrthogonalSeq4 } },
        /* TrackElemType::LeftEighthBankToDiag */
        { 5,
          { kLeftEighthBankToDiagSeq0, kLeftEighthBankToDiagSeq1, kLeftEighthBankToDiagSeq2, kLeftEighthBankToDiagSeq3,
            kLeftEighthBankToDiagSeq4 } },
        /* TrackElemType::RightEighthBankToDiag */
        { 5,
          { kRightEighthBankToDiagSeq0, kRightEighthBankToDiagSeq1, kRightEighthBankToDiagSeq2, kRightEighthBankToDiagSeq3,
            kRightEighthBankToDiagSeq4 } },
        /* TrackElemType::LeftEighthBankToOrthogonal */
        { 5,
          { kLeftEighthBankToOrthogonalSeq0, kLeftEighthBankToOrthogonalSeq1, kLeftEighthBankToOrthogonalSeq2,
            kLeftEighthBankToOrthogonalSeq3, kLeftEighthBankToOrthogonalSeq4 } },
        /* TrackElemType::RightEighthBankToOrthogonal */
        { 5,
          { kRightEighthBankToOrthogonalSeq0, kRightEighthBankToOrthogonalSeq1, kRightEighthBankToOrthogonalSeq2,
            kRightEighthBankToOrthogonalSeq3, kRightEighthBankToOrthogonalSeq4 } },
        /* TrackElemType::DiagFlat */ { 4, { kDiagFlatSeq0, kDiagFlatSeq1, kDiagFlatSeq2, kDiagFlatSeq3 } },
        /* TrackElemType::DiagUp25 */ { 4, { kDiagUp25Seq0, kDiagUp25Seq1, kDiagUp25Seq2, kDiagUp25Seq3 } },
        /* TrackElemType::DiagUp60 */ { 4, { kDiagUp60Seq0, kDiagUp60Seq1, kDiagUp60Seq2, kDiagUp60Seq3 } },
        /* TrackElemType::DiagFlatToUp25 */
        { 4, { kDiagFlatToUp25Seq0, kDiagFlatToUp25Seq1, kDiagFlatToUp25Seq2, kDiagFlatToUp25Seq3 } },
        /* TrackElemType::DiagUp25ToUp60 */
        { 4, { kDiagUp25ToUp60Seq0, kDiagUp25ToUp60Seq1, kDiagUp25ToUp60Seq2, kDiagUp25ToUp60Seq3 } },
        /* TrackElemType::DiagUp60ToUp25 */
        { 4, { kDiagUp60ToUp25Seq0, kDiagUp60ToUp25Seq1, kDiagUp60ToUp25Seq2, kDiagUp60ToUp25Seq3 } },
        /* TrackElemType::DiagUp25ToFlat */
        { 4, { kDiagUp25ToFlatSeq0, kDiagUp25ToFlatSeq1, kDiagUp25ToFlatSeq2, kDiagUp25ToFlatSeq3 } },
        /* TrackElemType::DiagDown25 */ { 4, { kDiagDown25Seq0, kDiagDown25Seq1, kDiagDown25Seq2, kDiagDown25Seq3 } },
        /* TrackElemType::DiagDown60 */ { 4, { kDiagDown60Seq0, kDiagDown60Seq1, kDiagDown60Seq2, kDiagDown60Seq3 } },
        /* TrackElemType::DiagFlatToDown25 */
        { 4, { kDiagFlatToDown25Seq0, kDiagFlatToDown25Seq1, kDiagFlatToDown25Seq2, kDiagFlatToDown25Seq3 } },
        /* TrackElemType::DiagDown25ToDown60 */
        { 4, { kDiagDown25ToDown60Seq0, kDiagDown25ToDown60Seq1, kDiagDown25ToDown60Seq2, kDiagDown25ToDown60Seq3 } },
        /* TrackElemType::DiagDown60ToDown25 */
        { 4, { kDiagDown60ToDown25Seq0, kDiagDown60ToDown25Seq1, kDiagDown60ToDown25Seq2, kDiagDown60ToDown25Seq3 } },
        /* TrackElemType::DiagDown25ToFlat */
        { 4, { kDiagDown25ToFlatSeq0, kDiagDown25ToFlatSeq1, kDiagDown25ToFlatSeq2, kDiagDown25ToFlatSeq3 } },
        /* TrackElemType::DiagFlatToUp60 */
        { 4, { kDiagFlatToUp60Seq0, kDiagFlatToUp60Seq1, kDiagFlatToUp60Seq2, kDiagFlatToUp60Seq3 } },
        /* TrackElemType::DiagUp60ToFlat */
        { 4, { kDiagUp60ToFlatSeq0, kDiagUp60ToFlatSeq1, kDiagUp60ToFlatSeq2, kDiagUp60ToFlatSeq3 } },
        /* TrackElemType::DiagFlatToDown60 */
        { 4, { kDiagFlatToDown60Seq0, kDiagFlatToDown60Seq1, kDiagFlatToDown60Seq2, kDiagFlatToDown60Seq3 } },
        /* TrackElemType::DiagDown60ToFlat */
        { 4, { kDiagDown60ToFlatSeq0, kDiagDown60ToFlatSeq1, kDiagDown60ToFlatSeq2, kDiagDown60ToFlatSeq3 } },
        /* TrackElemType::DiagFlatToLeftBank */
        { 4, { kDiagFlatToLeftBankSeq0, kDiagFlatToLeftBankSeq1, kDiagFlatToLeftBankSeq2, kDiagFlatToLeftBankSeq3 } },
        /* TrackElemType::DiagFlatToRightBank */
        { 4, { kDiagFlatToRightBankSeq0, kDiagFlatToRightBankSeq1, kDiagFlatToRightBankSeq2, kDiagFlatToRightBankSeq3 } },
        /* TrackElemType::DiagLeftBankToFlat */
        { 4, { kDiagLeftBankToFlatSeq0, kDiagLeftBankToFlatSeq1, kDiagLeftBankToFlatSeq2, kDiagLeftBankToFlatSeq3 } },
        /* TrackElemType::DiagRightBankToFlat */
        { 4, { kDiagRightBankToFlatSeq0, kDiagRightBankToFlatSeq1, kDiagRightBankToFlatSeq2, kDiagRightBankToFlatSeq3 } },
        /* TrackElemType::DiagLeftBankToUp25 */
        { 4, { kDiagLeftBankToUp25Seq0, kDiagLeftBankToUp25Seq1, kDiagLeftBankToUp25Seq2, kDiagLeftBankToUp25Seq3 } },
        /* TrackElemType::DiagRightBankToUp25 */
        { 4, { kDiagRightBankToUp25Seq0, kDiagRightBankToUp25Seq1, kDiagRightBankToUp25Seq2, kDiagRightBankToUp25Seq3 } },
        /* TrackElemType::DiagUp25ToLeftBank */
        { 4, { kDiagUp25ToLeftBankSeq0, kDiagUp25ToLeftBankSeq1, kDiagUp25ToLeftBankSeq2, kDiagUp25ToLeftBankSeq3 } },
        /* TrackElemType::DiagUp25ToRightBank */
        { 4, { kDiagUp25ToRightBankSeq0, kDiagUp25ToRightBankSeq1, kDiagUp25ToRightBankSeq2, kDiagUp25ToRightBankSeq3 } },
        /* TrackElemType::DiagLeftBankToDown25 */
        { 4, { kDiagLeftBankToDown25Seq0, kDiagLeftBankToDown25Seq1, kDiagLeftBankToDown25Seq2, kDiagLeftBankToDown25Seq3 } },
        /* TrackElemType::DiagRightBankToDown25 */
        { 4,
          { kDiagRightBankToDown25Seq0, kDiagRightBankToDown25Seq1, kDiagRightBankToDown25Seq2, kDiagRightBankToDown25Seq3 } },
        /* TrackElemType::DiagDown25ToLeftBank */
        { 4, { kDiagDown25ToLeftBankSeq0, kDiagDown25ToLeftBankSeq1, kDiagDown25ToLeftBankSeq2, kDiagDown25ToLeftBankSeq3 } },
        /* TrackElemType::DiagDown25ToRightBank */
        { 4,
          { kDiagDown25ToRightBankSeq0, kDiagDown25ToRightBankSeq1, kDiagDown25ToRightBankSeq2, kDiagDown25ToRightBankSeq3 } },
        /* TrackElemType::DiagLeftBank */ { 4, { kDiagLeftBankSeq0, kDiagLeftBankSeq1, kDiagLeftBankSeq2, kDiagLeftBankSeq3 } },
        /* TrackElemType::DiagRightBank */
        { 4, { kDiagRightBankSeq0, kDiagRightBankSeq1, kDiagRightBankSeq2, kDiagRightBankSeq3 } },
        /* TrackElemType::LogFlumeReverser */ { 1, { kLogFlumeReverserSeq0 } },
        /* TrackElemType::SpinningTunnel */ { 1, { kSpinningTunnelSeq0 } },
        /* TrackElemType::LeftBarrelRollUpToDown */
        { 3, { kLeftBarrelRollUpToDownSeq0, kLeftBarrelRollUpToDownSeq1, kLeftBarrelRollUpToDownSeq2 } },
        /* TrackElemType::RightBarrelRollUpToDown */
        { 3, { kRightBarrelRollUpToDownSeq0, kRightBarrelRollUpToDownSeq1, kRightBarrelRollUpToDownSeq2 } },
        /* TrackElemType::LeftBarrelRollDownToUp */
        { 3, { kLeftBarrelRollDownToUpSeq0, kLeftBarrelRollDownToUpSeq1, kLeftBarrelRollDownToUpSeq2 } },
        /* TrackElemType::RightBarrelRollDownToUp */
        { 3, { kRightBarrelRollDownToUpSeq0, kRightBarrelRollDownToUpSeq1, kRightBarrelRollDownToUpSeq2 } },
        /* TrackElemType::LeftBankToLeftQuarterTurn3TilesUp25 */
        { 4,
          { kLeftBankToLeftQuarterTurn3TilesUp25Seq0, kLeftBankToLeftQuarterTurn3TilesUp25Seq1,
            kLeftBankToLeftQuarterTurn3TilesUp25Seq2, kLeftBankToLeftQuarterTurn3TilesUp25Seq3 } },
        /* TrackElemType::RightBankToRightQuarterTurn3TilesUp25 */
        { 4,
          { kRightBankToRightQuarterTurn3TilesUp25Seq0, kRightBankToRightQuarterTurn3TilesUp25Seq1,
            kRightBankToRightQuarterTurn3TilesUp25Seq2, kRightBankToRightQuarterTurn3TilesUp25Seq3 } },
        /* TrackElemType::LeftQuarterTurn3TilesDown25ToLeftBank */
        { 4,
          { kLeftQuarterTurn3TilesDown25ToLeftBankSeq0, kLeftQuarterTurn3TilesDown25ToLeftBankSeq1,
            kLeftQuarterTurn3TilesDown25ToLeftBankSeq2, kLeftQuarterTurn3TilesDown25ToLeftBankSeq3 } },
        /* TrackElemType::RightQuarterTurn3TilesDown25ToRightBank */
        { 4,
          { kRightQuarterTurn3TilesDown25ToRightBankSeq0, kRightQuarterTurn3TilesDown25ToRightBankSeq1,
            kRightQuarterTurn3TilesDown25ToRightBankSeq2, kRightQuarterTurn3TilesDown25ToRightBankSeq3 } },
        /* TrackElemType::PoweredLift */ { 1, { kPoweredLiftSeq0 } },
        /* TrackElemType::LeftLargeHalfLoopUp */
        { 7,
          { kLeftLargeHalfLoopUpSeq0, kLeftLargeHalfLoopUpSeq1, kLeftLargeHalfLoopUpSeq2, kLeftLargeHalfLoopUpSeq3,
            kLeftLargeHalfLoopUpSeq4, kLeftLargeHalfLoopUpSeq5, kLeftLargeHalfLoopUpSeq6 } },
        /* TrackElemType::RightLargeHalfLoopUp */
        { 7,
          { kRightLargeHalfLoopUpSeq0, kRightLargeHalfLoopUpSeq1, kRightLargeHalfLoopUpSeq2, kRightLargeHalfLoopUpSeq3,
            kRightLargeHalfLoopUpSeq4, kRightLargeHalfLoopUpSeq5, kRightLargeHalfLoopUpSeq6 } },
        /* TrackElemType::LeftLargeHalfLoopDown */
        { 7,
          { kLeftLargeHalfLoopDownSeq0, kLeftLargeHalfLoopDownSeq1, kLeftLargeHalfLoopDownSeq2, kLeftLargeHalfLoopDownSeq3,
            kLeftLargeHalfLoopDownSeq4, kLeftLargeHalfLoopDownSeq5, kLeftLargeHalfLoopDownSeq6 } },
        /* TrackElemType::RightLargeHalfLoopDown */
        { 7,
          { kRightLargeHalfLoopDownSeq0, kRightLargeHalfLoopDownSeq1, kRightLargeHalfLoopDownSeq2, kRightLargeHalfLoopDownSeq3,
            kRightLargeHalfLoopDownSeq4, kRightLargeHalfLoopDownSeq5, kRightLargeHalfLoopDownSeq6 } },
        /* TrackElemType::LeftFlyerTwistUp */ { 3, { kLeftFlyerTwistUpSeq0, kLeftFlyerTwistUpSeq1, kLeftFlyerTwistUpSeq2 } },
        /* TrackElemType::RightFlyerTwistUp */
        { 3, { kRightFlyerTwistUpSeq0, kRightFlyerTwistUpSeq1, kRightFlyerTwistUpSeq2 } },
        /* TrackElemType::LeftFlyerTwistDown */
        { 3, { kLeftFlyerTwistDownSeq0, kLeftFlyerTwistDownSeq1, kLeftFlyerTwistDownSeq2 } },
        /* TrackElemType::RightFlyerTwistDown */
        { 3, { kRightFlyerTwistDownSeq0, kRightFlyerTwistDownSeq1, kRightFlyerTwistDownSeq2 } },
        /* TrackElemType::FlyerHalfLoopUninvertedUp */
        { 4,
          { kFlyerHalfLoopUninvertedUpSeq0, kFlyerHalfLoopUninvertedUpSeq1, kFlyerHalfLoopUninvertedUpSeq2,
            kFlyerHalfLoopUninvertedUpSeq3 } },
        /* TrackElemType::FlyerHalfLoopInvertedDown */
        { 4,
          { kFlyerHalfLoopInvertedDownSeq0, kFlyerHalfLoopInvertedDownSeq1, kFlyerHalfLoopInvertedDownSeq2,
            kFlyerHalfLoopInvertedDownSeq3 } },
        /* TrackElemType::LeftFlyerCorkscrewUp */
        { 3, { kLeftFlyerCorkscrewUpSeq0, kLeftFlyerCorkscrewUpSeq1, kLeftFlyerCorkscrewUpSeq2 } },
        /* TrackElemType::RightFlyerCorkscrewUp */
        { 3, { kRightFlyerCorkscrewUpSeq0, kRightFlyerCorkscrewUpSeq1, kRightFlyerCorkscrewUpSeq2 } },
        /* TrackElemType::LeftFlyerCorkscrewDown */
        { 3, { kLeftFlyerCorkscrewDownSeq0, kLeftFlyerCorkscrewDownSeq1, kLeftFlyerCorkscrewDownSeq2 } },
        /* TrackElemType::RightFlyerCorkscrewDown */
        { 3, { kRightFlyerCorkscrewDownSeq0, kRightFlyerCorkscrewDownSeq1, kRightFlyerCorkscrewDownSeq2 } },
        /* TrackElemType::HeartLineTransferUp */
        { 4, { kHeartLineTransferUpSeq0, kHeartLineTransferUpSeq1, kHeartLineTransferUpSeq2, kHeartLineTransferUpSeq3 } },
        /* TrackElemType::HeartLineTransferDown */
        { 4,
          { kHeartLineTransferDownSeq0, kHeartLineTransferDownSeq1, kHeartLineTransferDownSeq2, kHeartLineTransferDownSeq3 } },
        /* TrackElemType::LeftHeartLineRoll */
        { 6,
          { kLeftHeartLineRollSeq0, kLeftHeartLineRollSeq1, kLeftHeartLineRollSeq2, kLeftHeartLineRollSeq3,
            kLeftHeartLineRollSeq4, kLeftHeartLineRollSeq5 } },
        /* TrackElemType::RightHeartLineRoll */
        { 6,
          { kRightHeartLineRollSeq0, kRightHeartLineRollSeq1, kRightHeartLineRollSeq2, kRightHeartLineRollSeq3,
            kRightHeartLineRollSeq4, kRightHeartLineRollSeq5 } },
        /* TrackElemType::MinigolfHoleA */ { 2, { kMinigolfHoleASeq0, kMinigolfHoleASeq1 } },
        /* TrackElemType::MinigolfHoleB */ { 2, { kMinigolfHoleBSeq0, kMinigolfHoleBSeq1 } },
        /* TrackElemType::MinigolfHoleC */ { 2, { kMinigolfHoleCSeq0, kMinigolfHoleCSeq1 } },
        /* TrackElemType::MinigolfHoleD */ { 3, { kMinigolfHoleDSeq0, kMinigolfHoleDSeq1, kMinigolfHoleDSeq2 } },
        /* TrackElemType::MinigolfHoleE */ { 3, { kMinigolfHoleESeq0, kMinigolfHoleESeq1, kMinigolfHoleESeq2 } },
        /* TrackElemType::MultiDimInvertedFlatToDown90QuarterLoop */
        { 4,
          { kMultiDimInvertedFlatToDown90QuarterLoopSeq0, kMultiDimInvertedFlatToDown90QuarterLoopSeq1,
            kMultiDimInvertedFlatToDown90QuarterLoopSeq2, kMultiDimInvertedFlatToDown90QuarterLoopSeq3 } },
        /* TrackElemType::Up90ToInvertedFlatQuarterLoop */
        { 3, { kUp90ToInvertedFlatQuarterLoopSeq0, kUp90ToInvertedFlatQuarterLoopSeq1, kUp90ToInvertedFlatQuarterLoopSeq2 } },
        /* TrackElemType::InvertedFlatToDown90QuarterLoop */
        { 4,
          { kInvertedFlatToDown90QuarterLoopSeq0, kInvertedFlatToDown90QuarterLoopSeq1, kInvertedFlatToDown90QuarterLoopSeq2,
            kInvertedFlatToDown90QuarterLoopSeq3 } },
        /* TrackElemType::LeftCurvedLiftHill */
        { 4, { kLeftCurvedLiftHillSeq0, kLeftCurvedLiftHillSeq1, kLeftCurvedLiftHillSeq2, kLeftCurvedLiftHillSeq3 } },
        /* TrackElemType::RightCurvedLiftHill */
        { 4, { kRightCurvedLiftHillSeq0, kRightCurvedLiftHillSeq1, kRightCurvedLiftHillSeq2, kRightCurvedLiftHillSeq3 } },
        /* TrackElemType::LeftReverser */
        { 6,
          { kLeftReverserSeq0, kLeftReverserSeq1, kLeftReverserSeq2, kLeftReverserSeq3, kLeftReverserSeq4,
            kLeftReverserSeq5 } },
        /* TrackElemType::RightReverser */
        { 6,
          { kRightReverserSeq0, kRightReverserSeq1, kRightReverserSeq2, kRightReverserSeq3, kRightReverserSeq4,
            kRightReverserSeq5 } },
        /* TrackElemType::AirThrustTopCap */
        { 4, { kAirThrustTopCapSeq0, kAirThrustTopCapSeq1, kAirThrustTopCapSeq2, kAirThrustTopCapSeq3 } },
        /* TrackElemType::AirThrustVerticalDown */ { 2, { kAirThrustVerticalDownSeq0, kAirThrustVerticalDownSeq1 } },
        /* TrackElemType::AirThrustVerticalDownToLevel */
        { 7,
          { kAirThrustVerticalDownToLevelSeq0, kAirThrustVerticalDownToLevelSeq1, kAirThrustVerticalDownToLevelSeq2,
            kAirThrustVerticalDownToLevelSeq3, kAirThrustVerticalDownToLevelSeq4, kAirThrustVerticalDownToLevelSeq5,
            kAirThrustVerticalDownToLevelSeq6 } },
        /* TrackElemType::BlockBrakes */ { 1, { kBlockBrakesSeq0 } },
        /* TrackElemType::LeftBankedQuarterTurn3TileUp25 */
        { 4,
          { kLeftBankedQuarterTurn3TileUp25Seq0, kLeftBankedQuarterTurn3TileUp25Seq1, kLeftBankedQuarterTurn3TileUp25Seq2,
            kLeftBankedQuarterTurn3TileUp25Seq3 } },
        /* TrackElemType::RightBankedQuarterTurn3TileUp25 */
        { 4,
          { kRightBankedQuarterTurn3TileUp25Seq0, kRightBankedQuarterTurn3TileUp25Seq1, kRightBankedQuarterTurn3TileUp25Seq2,
            kRightBankedQuarterTurn3TileUp25Seq3 } },
        /* TrackElemType::LeftBankedQuarterTurn3TileDown25 */
        { 4,
          { kLeftBankedQuarterTurn3TileDown25Seq0, kLeftBankedQuarterTurn3TileDown25Seq1, kLeftBankedQuarterTurn3TileDown25Seq2,
            kLeftBankedQuarterTurn3TileDown25Seq3 } },
        /* TrackElemType::RightBankedQuarterTurn3TileDown25 */
        { 4,
          { kRightBankedQuarterTurn3TileDown25Seq0, kRightBankedQuarterTurn3TileDown25Seq1,
            kRightBankedQuarterTurn3TileDown25Seq2, kRightBankedQuarterTurn3TileDown25Seq3 } },
        /* TrackElemType::LeftBankedQuarterTurn5TileUp25 */
        { 7,
          { kLeftBankedQuarterTurn5TileUp25Seq0, kLeftBankedQuarterTurn5TileUp25Seq1, kLeftBankedQuarterTurn5TileUp25Seq2,
            kLeftBankedQuarterTurn5TileUp25Seq3, kLeftBankedQuarterTurn5TileUp25Seq4, kLeftBankedQuarterTurn5TileUp25Seq5,
            kLeftBankedQuarterTurn5TileUp25Seq6, kLeftBankedQuarterTurn5TileUp25Seq7 } },
        /* TrackElemType::RightBankedQuarterTurn5TileUp25 */
        { 7,
          { kRightBankedQuarterTurn5TileUp25Seq0, kRightBankedQuarterTurn5TileUp25Seq1, kRightBankedQuarterTurn5TileUp25Seq2,
            kRightBankedQuarterTurn5TileUp25Seq3, kRightBankedQuarterTurn5TileUp25Seq4, kRightBankedQuarterTurn5TileUp25Seq5,
            kRightBankedQuarterTurn5TileUp25Seq6, kRightBankedQuarterTurn5TileUp25Seq7 } },
        /* TrackElemType::LeftBankedQuarterTurn5TileDown25 */
        { 7,
          { kLeftBankedQuarterTurn5TileDown25Seq0, kLeftBankedQuarterTurn5TileDown25Seq1, kLeftBankedQuarterTurn5TileDown25Seq2,
            kLeftBankedQuarterTurn5TileDown25Seq3, kLeftBankedQuarterTurn5TileDown25Seq4, kLeftBankedQuarterTurn5TileDown25Seq5,
            kLeftBankedQuarterTurn5TileDown25Seq6, kLeftBankedQuarterTurn5TileDown25Seq7 } },
        /* TrackElemType::RightBankedQuarterTurn5TileDown25 */
        { 7,
          { kRightBankedQuarterTurn5TileDown25Seq0, kRightBankedQuarterTurn5TileDown25Seq1,
            kRightBankedQuarterTurn5TileDown25Seq2, kRightBankedQuarterTurn5TileDown25Seq3,
            kRightBankedQuarterTurn5TileDown25Seq4, kRightBankedQuarterTurn5TileDown25Seq5,
            kRightBankedQuarterTurn5TileDown25Seq6, kRightBankedQuarterTurn5TileDown25Seq7 } },
        /* TrackElemType::Up25ToLeftBankedUp25 */ { 1, { kUp25ToLeftBankedUp25Seq0 } },
        /* TrackElemType::Up25ToRightBankedUp25 */ { 1, { kUp25ToRightBankedUp25Seq0 } },
        /* TrackElemType::LeftBankedUp25ToUp25 */ { 1, { kLeftBankedUp25ToUp25Seq0 } },
        /* TrackElemType::RightBankedUp25ToUp25 */ { 1, { kRightBankedUp25ToUp25Seq0 } },
        /* TrackElemType::Down25ToLeftBankedDown25 */ { 1, { kDown25ToLeftBankedDown25Seq0 } },
        /* TrackElemType::Down25ToRightBankedDown25 */ { 1, { kDown25ToRightBankedDown25Seq0 } },
        /* TrackElemType::LeftBankedDown25ToDown25 */ { 1, { kLeftBankedDown25ToDown25Seq0 } },
        /* TrackElemType::RightBankedDown25ToDown25 */ { 1, { kRightBankedDown25ToDown25Seq0 } },
        /* TrackElemType::LeftBankedFlatToLeftBankedUp25 */ { 1, { kLeftBankedFlatToLeftBankedUp25Seq0 } },
        /* TrackElemType::RightBankedFlatToRightBankedUp25 */ { 1, { kRightBankedFlatToRightBankedUp25Seq0 } },
        /* TrackElemType::LeftBankedUp25ToLeftBankedFlat */ { 1, { kLeftBankedUp25ToLeftBankedFlatSeq0 } },
        /* TrackElemType::RightBankedUp25ToRightBankedFlat */ { 1, { kRightBankedUp25ToRightBankedFlatSeq0 } },
        /* TrackElemType::LeftBankedFlatToLeftBankedDown25 */ { 1, { kLeftBankedFlatToLeftBankedDown25Seq0 } },
        /* TrackElemType::RightBankedFlatToRightBankedDown25 */ { 1, { kRightBankedFlatToRightBankedDown25Seq0 } },
        /* TrackElemType::LeftBankedDown25ToLeftBankedFlat */ { 1, { kLeftBankedDown25ToLeftBankedFlatSeq0 } },
        /* TrackElemType::RightBankedDown25ToRightBankedFlat */ { 1, { kRightBankedDown25ToRightBankedFlatSeq0 } },
        /* TrackElemType::FlatToLeftBankedUp25 */ { 1, { kFlatToLeftBankedUp25Seq0 } },
        /* TrackElemType::FlatToRightBankedUp25 */ { 1, { kFlatToRightBankedUp25Seq0 } },
        /* TrackElemType::LeftBankedUp25ToFlat */ { 1, { kLeftBankedUp25ToFlatSeq0 } },
        /* TrackElemType::RightBankedUp25ToFlat */ { 1, { kRightBankedUp25ToFlatSeq0 } },
        /* TrackElemType::FlatToLeftBankedDown25 */ { 1, { kFlatToLeftBankedDown25Seq0 } },
        /* TrackElemType::FlatToRightBankedDown25 */ { 1, { kFlatToRightBankedDown25Seq0 } },
        /* TrackElemType::LeftBankedDown25ToFlat */ { 1, { kLeftBankedDown25ToFlatSeq0 } },
        /* TrackElemType::RightBankedDown25ToFlat */ { 1, { kRightBankedDown25ToFlatSeq0 } },
        /* TrackElemType::LeftQuarterTurn1TileUp90 */ { 2, { kLeftQuarterTurn1TileUp90Seq0, kLeftQuarterTurn1TileUp90Seq1 } },
        /* TrackElemType::RightQuarterTurn1TileUp90 */
        { 2, { kRightQuarterTurn1TileUp90Seq0, kRightQuarterTurn1TileUp90Seq1 } },
        /* TrackElemType::LeftQuarterTurn1TileDown90 */
        { 2, { kLeftQuarterTurn1TileDown90Seq0, kLeftQuarterTurn1TileDown90Seq1 } },
        /* TrackElemType::RightQuarterTurn1TileDown90 */
        { 2, { kRightQuarterTurn1TileDown90Seq0, kRightQuarterTurn1TileDown90Seq1 } },
        /* TrackElemType::MultiDimUp90ToInvertedFlatQuarterLoop */
        { 3,
          { kMultiDimUp90ToInvertedFlatQuarterLoopSeq0, kMultiDimUp90ToInvertedFlatQuarterLoopSeq1,
            kMultiDimUp90ToInvertedFlatQuarterLoopSeq2 } },
        /* TrackElemType::MultiDimFlatToDown90QuarterLoop */
        { 4,
          { kMultiDimFlatToDown90QuarterLoopSeq0, kMultiDimFlatToDown90QuarterLoopSeq1, kMultiDimFlatToDown90QuarterLoopSeq2,
            kMultiDimFlatToDown90QuarterLoopSeq3 } },
        /* TrackElemType::MultiDimInvertedUp90ToFlatQuarterLoop */
        { 3,
          { kMultiDimInvertedUp90ToFlatQuarterLoopSeq0, kMultiDimInvertedUp90ToFlatQuarterLoopSeq1,
            kMultiDimInvertedUp90ToFlatQuarterLoopSeq2 } },
        /* TrackElemType::RotationControlToggle */ { 1, { kRotationControlToggleSeq0 } },
        /* TrackElemType::FlatTrack1x4A */
        { 4, { kFlatTrack1x4ASeq0, kFlatTrack1x4ASeq1, kFlatTrack1x4ASeq2, kFlatTrack1x4ASeq3 } },
        /* TrackElemType::FlatTrack2x2 */ { 4, { kFlatTrack2x2Seq0, kFlatTrack2x2Seq1, kFlatTrack2x2Seq2, kFlatTrack2x2Seq3 } },
        /* TrackElemType::FlatTrack4x4 */
        { 16,
          { kFlatTrack4x4Seq0, kFlatTrack4x4Seq1, kFlatTrack4x4Seq2, kFlatTrack4x4Seq3, kFlatTrack4x4Seq4, kFlatTrack4x4Seq5,
            kFlatTrack4x4Seq6, kFlatTrack4x4Seq7, kFlatTrack4x4Seq8, kFlatTrack4x4Seq9, kFlatTrack4x4Seq10, kFlatTrack4x4Seq11,
            kFlatTrack4x4Seq12, kFlatTrack4x4Seq13, kFlatTrack4x4Seq14, kFlatTrack4x4Seq15 } },
        /* TrackElemType::FlatTrack2x4 */
        { 8,
          { kFlatTrack2x4Seq0, kFlatTrack2x4Seq1, kFlatTrack2x4Seq2, kFlatTrack2x4Seq3, kFlatTrack2x4Seq4, kFlatTrack2x4Seq5,
            kFlatTrack2x4Seq6, kFlatTrack2x4Seq7 } },
        /* TrackElemType::FlatTrack1x5 */
        { 5, { kFlatTrack1x5Seq0, kFlatTrack1x5Seq1, kFlatTrack1x5Seq2, kFlatTrack1x5Seq3, kFlatTrack1x5Seq4 } },
        /* TrackElemType::FlatTrack1x1A */ { 1, { kFlatTrack1x1ASeq0 } },
        /* TrackElemType::FlatTrack1x4B */
        { 4, { kFlatTrack1x4BSeq0, kFlatTrack1x4BSeq1, kFlatTrack1x4BSeq2, kFlatTrack1x4BSeq3 } },
        /* TrackElemType::FlatTrack1x1B */ { 1, { kFlatTrack1x1BSeq0 } },
        /* TrackElemType::FlatTrack1x4C */
        { 4, { kFlatTrack1x4CSeq0, kFlatTrack1x4CSeq1, kFlatTrack1x4CSeq2, kFlatTrack1x4CSeq3 } },
        /* TrackElemType::FlatTrack3x3 */
        { 9,
          { kFlatTrack3x3Seq0, kFlatTrack3x3Seq1, kFlatTrack3x3Seq2, kFlatTrack3x3Seq3, kFlatTrack3x3Seq4, kFlatTrack3x3Seq5,
            kFlatTrack3x3Seq6, kFlatTrack3x3Seq7, kFlatTrack3x3Seq8 } },
        /* TrackElemType::LeftLargeCorkscrewUp */
        { 6,
          { kLeftLargeCorkscrewUpSeq0, kLeftLargeCorkscrewUpSeq1, kLeftLargeCorkscrewUpSeq2, kLeftLargeCorkscrewUpSeq3,
            kLeftLargeCorkscrewUpSeq4, kLeftLargeCorkscrewUpSeq5 } },
        /* TrackElemType::RightLargeCorkscrewUp */
        { 6,
          { kRightLargeCorkscrewUpSeq0, kRightLargeCorkscrewUpSeq1, kRightLargeCorkscrewUpSeq2, kRightLargeCorkscrewUpSeq3,
            kRightLargeCorkscrewUpSeq4, kRightLargeCorkscrewUpSeq5 } },
        /* TrackElemType::LeftLargeCorkscrewDown */
        { 6,
          { kLeftLargeCorkscrewDownSeq0, kLeftLargeCorkscrewDownSeq1, kLeftLargeCorkscrewDownSeq2, kLeftLargeCorkscrewDownSeq3,
            kLeftLargeCorkscrewDownSeq4, kLeftLargeCorkscrewDownSeq5 } },
        /* TrackElemType::RightLargeCorkscrewDown */
        { 6,
          { kRightLargeCorkscrewDownSeq0, kRightLargeCorkscrewDownSeq1, kRightLargeCorkscrewDownSeq2,
            kRightLargeCorkscrewDownSeq3, kRightLargeCorkscrewDownSeq4, kRightLargeCorkscrewDownSeq5 } },
        /* TrackElemType::LeftMediumHalfLoopUp */
        { 5,
          { kLeftMediumHalfLoopUpSeq0, kLeftMediumHalfLoopUpSeq1, kLeftMediumHalfLoopUpSeq2, kLeftMediumHalfLoopUpSeq3,
            kLeftMediumHalfLoopUpSeq4 } },
        /* TrackElemType::RightMediumHalfLoopUp */
        { 5,
          { kRightMediumHalfLoopUpSeq0, kRightMediumHalfLoopUpSeq1, kRightMediumHalfLoopUpSeq2, kRightMediumHalfLoopUpSeq3,
            kRightMediumHalfLoopUpSeq4 } },
        /* TrackElemType::LeftMediumHalfLoopDown */
        { 5,
          { kLeftMediumHalfLoopDownSeq0, kLeftMediumHalfLoopDownSeq1, kLeftMediumHalfLoopDownSeq2, kLeftMediumHalfLoopDownSeq3,
            kLeftMediumHalfLoopDownSeq4 } },
        /* TrackElemType::RightMediumHalfLoopDown */
        { 5,
          { kRightMediumHalfLoopDownSeq0, kRightMediumHalfLoopDownSeq1, kRightMediumHalfLoopDownSeq2,
            kRightMediumHalfLoopDownSeq3, kRightMediumHalfLoopDownSeq4 } },
        /* TrackElemType::LeftZeroGRollUp */ { 3, { kLeftZeroGRollUpSeq0, kLeftZeroGRollUpSeq1, kLeftZeroGRollUpSeq2 } },
        /* TrackElemType::RightZeroGRollUp */ { 3, { kRightZeroGRollUpSeq0, kRightZeroGRollUpSeq1, kRightZeroGRollUpSeq2 } },
        /* TrackElemType::LeftZeroGRollDown */
        { 3, { kLeftZeroGRollDownSeq0, kLeftZeroGRollDownSeq1, kLeftZeroGRollDownSeq2 } },
        /* TrackElemType::RightZeroGRollDown */
        { 3, { kRightZeroGRollDownSeq0, kRightZeroGRollDownSeq1, kRightZeroGRollDownSeq2 } },
        /* TrackElemType::LeftLargeZeroGRollUp */
        { 4, { kLeftLargeZeroGRollUpSeq0, kLeftLargeZeroGRollUpSeq1, kLeftLargeZeroGRollUpSeq2, kLeftLargeZeroGRollUpSeq3 } },
        /* TrackElemType::RightLargeZeroGRollUp */
        { 4,
          { kRightLargeZeroGRollUpSeq0, kRightLargeZeroGRollUpSeq1, kRightLargeZeroGRollUpSeq2, kRightLargeZeroGRollUpSeq3 } },
        /* TrackElemType::LeftLargeZeroGRollDown */
        { 4,
          { kLeftLargeZeroGRollDownSeq0, kLeftLargeZeroGRollDownSeq1, kLeftLargeZeroGRollDownSeq2,
            kLeftLargeZeroGRollDownSeq3 } },
        /* TrackElemType::RightLargeZeroGRollDown */
        { 4,
          { kRightLargeZeroGRollDownSeq0, kRightLargeZeroGRollDownSeq1, kRightLargeZeroGRollDownSeq2,
            kRightLargeZeroGRollDownSeq3 } },
        /* TrackElemType::LeftFlyerLargeHalfLoopUninvertedUp */
        { 7,
          { kLeftFlyerLargeHalfLoopUninvertedUpSeq0, kLeftFlyerLargeHalfLoopUninvertedUpSeq1,
            kLeftFlyerLargeHalfLoopUninvertedUpSeq2, kLeftFlyerLargeHalfLoopUninvertedUpSeq3,
            kLeftFlyerLargeHalfLoopUninvertedUpSeq4, kLeftFlyerLargeHalfLoopUninvertedUpSeq5,
            kLeftFlyerLargeHalfLoopUninvertedUpSeq6 } },
        /* TrackElemType::RightFlyerLargeHalfLoopUninvertedUp */
        { 7,
          { kRightFlyerLargeHalfLoopUninvertedUpSeq0, kRightFlyerLargeHalfLoopUninvertedUpSeq1,
            kRightFlyerLargeHalfLoopUninvertedUpSeq2, kRightFlyerLargeHalfLoopUninvertedUpSeq3,
            kRightFlyerLargeHalfLoopUninvertedUpSeq4, kRightFlyerLargeHalfLoopUninvertedUpSeq5,
            kRightFlyerLargeHalfLoopUninvertedUpSeq6 } },
        /* TrackElemType::LeftFlyerLargeHalfLoopInvertedDown */
        { 7,
          { kLeftFlyerLargeHalfLoopInvertedDownSeq0, kLeftFlyerLargeHalfLoopInvertedDownSeq1,
            kLeftFlyerLargeHalfLoopInvertedDownSeq2, kLeftFlyerLargeHalfLoopInvertedDownSeq3,
            kLeftFlyerLargeHalfLoopInvertedDownSeq4, kLeftFlyerLargeHalfLoopInvertedDownSeq5,
            kLeftFlyerLargeHalfLoopInvertedDownSeq6 } },
        /* TrackElemType::RightFlyerLargeHalfLoopInvertedDown */
        { 7,
          { kRightFlyerLargeHalfLoopInvertedDownSeq0, kRightFlyerLargeHalfLoopInvertedDownSeq1,
            kRightFlyerLargeHalfLoopInvertedDownSeq2, kRightFlyerLargeHalfLoopInvertedDownSeq3,
            kRightFlyerLargeHalfLoopInvertedDownSeq4, kRightFlyerLargeHalfLoopInvertedDownSeq5,
            kRightFlyerLargeHalfLoopInvertedDownSeq6 } },
        /* TrackElemType::LeftFlyerLargeHalfLoopInvertedUp */
        { 7,
          { kLeftFlyerLargeHalfLoopInvertedUpSeq0, kLeftFlyerLargeHalfLoopInvertedUpSeq1, kLeftFlyerLargeHalfLoopInvertedUpSeq2,
            kLeftFlyerLargeHalfLoopInvertedUpSeq3, kLeftFlyerLargeHalfLoopInvertedUpSeq4, kLeftFlyerLargeHalfLoopInvertedUpSeq5,
            kLeftFlyerLargeHalfLoopInvertedUpSeq6 } },
        /* TrackElemType::RightFlyerLargeHalfLoopInvertedUp */
        { 7,
          { kRightFlyerLargeHalfLoopInvertedUpSeq0, kRightFlyerLargeHalfLoopInvertedUpSeq1,
            kRightFlyerLargeHalfLoopInvertedUpSeq2, kRightFlyerLargeHalfLoopInvertedUpSeq3,
            kRightFlyerLargeHalfLoopInvertedUpSeq4, kRightFlyerLargeHalfLoopInvertedUpSeq5,
            kRightFlyerLargeHalfLoopInvertedUpSeq6 } },
        /* TrackElemType::LeftFlyerLargeHalfLoopUninvertedDown */
        { 7,
          { kLeftFlyerLargeHalfLoopUninvertedDownSeq0, kLeftFlyerLargeHalfLoopUninvertedDownSeq1,
            kLeftFlyerLargeHalfLoopUninvertedDownSeq2, kLeftFlyerLargeHalfLoopUninvertedDownSeq3,
            kLeftFlyerLargeHalfLoopUninvertedDownSeq4, kLeftFlyerLargeHalfLoopUninvertedDownSeq5,
            kLeftFlyerLargeHalfLoopUninvertedDownSeq6 } },
        /* TrackElemType::RightFlyerLargeHalfLoopUninvertedDown */
        { 7,
          { kRightFlyerLargeHalfLoopUninvertedDownSeq0, kRightFlyerLargeHalfLoopUninvertedDownSeq1,
            kRightFlyerLargeHalfLoopUninvertedDownSeq2, kRightFlyerLargeHalfLoopUninvertedDownSeq3,
            kRightFlyerLargeHalfLoopUninvertedDownSeq4, kRightFlyerLargeHalfLoopUninvertedDownSeq5,
            kRightFlyerLargeHalfLoopUninvertedDownSeq6 } },
        /* TrackElemType::FlyerHalfLoopInvertedUp */
        { 4,
          { kFlyerHalfLoopInvertedUpSeq0, kFlyerHalfLoopInvertedUpSeq1, kFlyerHalfLoopInvertedUpSeq2,
            kFlyerHalfLoopInvertedUpSeq3 } },
        /* TrackElemType::FlyerHalfLoopUninvertedDown */
        { 4,
          { kFlyerHalfLoopUninvertedDownSeq0, kFlyerHalfLoopUninvertedDownSeq1, kFlyerHalfLoopUninvertedDownSeq2,
            kFlyerHalfLoopUninvertedDownSeq3 } },
        /* TrackElemType::LeftEighthToDiagUp25 */
        { 5,
          { kLeftEighthToDiagUp25Seq0, kLeftEighthToDiagUp25Seq1, kLeftEighthToDiagUp25Seq2, kLeftEighthToDiagUp25Seq3,
            kLeftEighthToDiagUp25Seq4 } },
        /* TrackElemType::RightEighthToDiagUp25 */
        { 5,
          { kRightEighthToDiagUp25Seq0, kRightEighthToDiagUp25Seq1, kRightEighthToDiagUp25Seq2, kRightEighthToDiagUp25Seq3,
            kRightEighthToDiagUp25Seq4 } },
        /* TrackElemType::LeftEighthToDiagDown25 */
        { 5,
          { kLeftEighthToDiagDown25Seq0, kLeftEighthToDiagDown25Seq1, kLeftEighthToDiagDown25Seq2, kLeftEighthToDiagDown25Seq3,
            kLeftEighthToDiagDown25Seq4 } },
        /* TrackElemType::RightEighthToDiagDown25 */
        { 5,
          { kRightEighthToDiagDown25Seq0, kRightEighthToDiagDown25Seq1, kRightEighthToDiagDown25Seq2,
            kRightEighthToDiagDown25Seq3, kRightEighthToDiagDown25Seq4 } },
        /* TrackElemType::LeftEighthToOrthogonalUp25 */
        { 5,
          { kLeftEighthToOrthogonalUp25Seq0, kLeftEighthToOrthogonalUp25Seq1, kLeftEighthToOrthogonalUp25Seq2,
            kLeftEighthToOrthogonalUp25Seq3, kLeftEighthToOrthogonalUp25Seq4 } },
        /* TrackElemType::RightEighthToOrthogonalUp25 */
        { 5,
          { kRightEighthToOrthogonalUp25Seq0, kRightEighthToOrthogonalUp25Seq1, kRightEighthToOrthogonalUp25Seq2,
            kRightEighthToOrthogonalUp25Seq3, kRightEighthToOrthogonalUp25Seq4 } },
        /* TrackElemType::LeftEighthToOrthogonalDown25 */
        { 5,
          { kLeftEighthToOrthogonalDown25Seq0, kLeftEighthToOrthogonalDown25Seq1, kLeftEighthToOrthogonalDown25Seq2,
            kLeftEighthToOrthogonalDown25Seq3, kLeftEighthToOrthogonalDown25Seq4 } },
        /* TrackElemType::RightEighthToOrthogonalDown25 */
        { 5,
          { kRightEighthToOrthogonalDown25Seq0, kRightEighthToOrthogonalDown25Seq1, kRightEighthToOrthogonalDown25Seq2,
            kRightEighthToOrthogonalDown25Seq3, kRightEighthToOrthogonalDown25Seq4 } },
        /* TrackElemType::DiagUp25ToLeftBankedUp25 */
        { 4,
          { kDiagUp25ToLeftBankedUp25Seq0, kDiagUp25ToLeftBankedUp25Seq1, kDiagUp25ToLeftBankedUp25Seq2,
            kDiagUp25ToLeftBankedUp25Seq3 } },
        /* TrackElemType::DiagUp25ToRightBankedUp25 */
        { 4,
          { kDiagUp25ToRightBankedUp25Seq0, kDiagUp25ToRightBankedUp25Seq1, kDiagUp25ToRightBankedUp25Seq2,
            kDiagUp25ToRightBankedUp25Seq3 } },
        /* TrackElemType::DiagLeftBankedUp25ToUp25 */
        { 4,
          { kDiagLeftBankedUp25ToUp25Seq0, kDiagLeftBankedUp25ToUp25Seq1, kDiagLeftBankedUp25ToUp25Seq2,
            kDiagLeftBankedUp25ToUp25Seq3 } },
        /* TrackElemType::DiagRightBankedUp25ToUp25 */
        { 4,
          { kDiagRightBankedUp25ToUp25Seq0, kDiagRightBankedUp25ToUp25Seq1, kDiagRightBankedUp25ToUp25Seq2,
            kDiagRightBankedUp25ToUp25Seq3 } },
        /* TrackElemType::DiagDown25ToLeftBankedDown25 */
        { 4,
          { kDiagDown25ToLeftBankedDown25Seq0, kDiagDown25ToLeftBankedDown25Seq1, kDiagDown25ToLeftBankedDown25Seq2,
            kDiagDown25ToLeftBankedDown25Seq3 } },
        /* TrackElemType::DiagDown25ToRightBankedDown25 */
        { 4,
          { kDiagDown25ToRightBankedDown25Seq0, kDiagDown25ToRightBankedDown25Seq1, kDiagDown25ToRightBankedDown25Seq2,
            kDiagDown25ToRightBankedDown25Seq3 } },
        /* TrackElemType::DiagLeftBankedDown25ToDown25 */
        { 4,
          { kDiagLeftBankedDown25ToDown25Seq0, kDiagLeftBankedDown25ToDown25Seq1, kDiagLeftBankedDown25ToDown25Seq2,
            kDiagLeftBankedDown25ToDown25Seq3 } },
        /* TrackElemType::DiagRightBankedDown25ToDown25 */
        { 4,
          { kDiagRightBankedDown25ToDown25Seq0, kDiagRightBankedDown25ToDown25Seq1, kDiagRightBankedDown25ToDown25Seq2,
            kDiagRightBankedDown25ToDown25Seq3 } },
        /* TrackElemType::DiagLeftBankedFlatToLeftBankedUp25 */
        { 4,
          { kDiagLeftBankedFlatToLeftBankedUp25Seq0, kDiagLeftBankedFlatToLeftBankedUp25Seq1,
            kDiagLeftBankedFlatToLeftBankedUp25Seq2, kDiagLeftBankedFlatToLeftBankedUp25Seq3 } },
        /* TrackElemType::DiagRightBankedFlatToRightBankedUp25 */
        { 4,
          { kDiagRightBankedFlatToRightBankedUp25Seq0, kDiagRightBankedFlatToRightBankedUp25Seq1,
            kDiagRightBankedFlatToRightBankedUp25Seq2, kDiagRightBankedFlatToRightBankedUp25Seq3 } },
        /* TrackElemType::DiagLeftBankedUp25ToLeftBankedFlat */
        { 4,
          { kDiagLeftBankedUp25ToLeftBankedFlatSeq0, kDiagLeftBankedUp25ToLeftBankedFlatSeq1,
            kDiagLeftBankedUp25ToLeftBankedFlatSeq2, kDiagLeftBankedUp25ToLeftBankedFlatSeq3 } },
        /* TrackElemType::DiagRightBankedUp25ToRightBankedFlat */
        { 4,
          { kDiagRightBankedUp25ToRightBankedFlatSeq0, kDiagRightBankedUp25ToRightBankedFlatSeq1,
            kDiagRightBankedUp25ToRightBankedFlatSeq2, kDiagRightBankedUp25ToRightBankedFlatSeq3 } },
        /* TrackElemType::DiagLeftBankedFlatToLeftBankedDown25 */
        { 4,
          { kDiagLeftBankedFlatToLeftBankedDown25Seq0, kDiagLeftBankedFlatToLeftBankedDown25Seq1,
            kDiagLeftBankedFlatToLeftBankedDown25Seq2, kDiagLeftBankedFlatToLeftBankedDown25Seq3 } },
        /* TrackElemType::DiagRightBankedFlatToRightBankedDown25 */
        { 4,
          { kDiagRightBankedFlatToRightBankedDown25Seq0, kDiagRightBankedFlatToRightBankedDown25Seq1,
            kDiagRightBankedFlatToRightBankedDown25Seq2, kDiagRightBankedFlatToRightBankedDown25Seq3 } },
        /* TrackElemType::DiagLeftBankedDown25ToLeftBankedFlat */
        { 4,
          { kDiagLeftBankedDown25ToLeftBankedFlatSeq0, kDiagLeftBankedDown25ToLeftBankedFlatSeq1,
            kDiagLeftBankedDown25ToLeftBankedFlatSeq2, kDiagLeftBankedDown25ToLeftBankedFlatSeq3 } },
        /* TrackElemType::DiagRightBankedDown25ToRightBankedFlat */
        { 4,
          { kDiagRightBankedDown25ToRightBankedFlatSeq0, kDiagRightBankedDown25ToRightBankedFlatSeq1,
            kDiagRightBankedDown25ToRightBankedFlatSeq2, kDiagRightBankedDown25ToRightBankedFlatSeq3 } },
        /* TrackElemType::DiagFlatToLeftBankedUp25 */
        { 4,
          { kDiagFlatToLeftBankedUp25Seq0, kDiagFlatToLeftBankedUp25Seq1, kDiagFlatToLeftBankedUp25Seq2,
            kDiagFlatToLeftBankedUp25Seq3 } },
        /* TrackElemType::DiagFlatToRightBankedUp25 */
        { 4,
          { kDiagFlatToRightBankedUp25Seq0, kDiagFlatToRightBankedUp25Seq1, kDiagFlatToRightBankedUp25Seq2,
            kDiagFlatToRightBankedUp25Seq3 } },
        /* TrackElemType::DiagLeftBankedUp25ToFlat */
        { 4,
          { kDiagLeftBankedUp25ToFlatSeq0, kDiagLeftBankedUp25ToFlatSeq1, kDiagLeftBankedUp25ToFlatSeq2,
            kDiagLeftBankedUp25ToFlatSeq3 } },
        /* TrackElemType::DiagRightBankedUp25ToFlat */
        { 4,
          { kDiagRightBankedUp25ToFlatSeq0, kDiagRightBankedUp25ToFlatSeq1, kDiagRightBankedUp25ToFlatSeq2,
            kDiagRightBankedUp25ToFlatSeq3 } },
        /* TrackElemType::DiagFlatToLeftBankedDown25 */
        { 4,
          { kDiagFlatToLeftBankedDown25Seq0, kDiagFlatToLeftBankedDown25Seq1, kDiagFlatToLeftBankedDown25Seq2,
            kDiagFlatToLeftBankedDown25Seq3 } },
        /* TrackElemType::DiagFlatToRightBankedDown25 */
        { 4,
          { kDiagFlatToRightBankedDown25Seq0, kDiagFlatToRightBankedDown25Seq1, kDiagFlatToRightBankedDown25Seq2,
            kDiagFlatToRightBankedDown25Seq3 } },
        /* TrackElemType::DiagLeftBankedDown25ToFlat */
        { 4,
          { kDiagLeftBankedDown25ToFlatSeq0, kDiagLeftBankedDown25ToFlatSeq1, kDiagLeftBankedDown25ToFlatSeq2,
            kDiagLeftBankedDown25ToFlatSeq3 } },
        /* TrackElemType::DiagRightBankedDown25ToFlat */
        { 4,
          { kDiagRightBankedDown25ToFlatSeq0, kDiagRightBankedDown25ToFlatSeq1, kDiagRightBankedDown25ToFlatSeq2,
            kDiagRightBankedDown25ToFlatSeq3 } },
        /* TrackElemType::DiagUp25LeftBanked */
        { 4, { kDiagUp25LeftBankedSeq0, kDiagUp25LeftBankedSeq1, kDiagUp25LeftBankedSeq2, kDiagUp25LeftBankedSeq3 } },
        /* TrackElemType::DiagUp25RightBanked */
        { 4, { kDiagUp25RightBankedSeq0, kDiagUp25RightBankedSeq1, kDiagUp25RightBankedSeq2, kDiagUp25RightBankedSeq3 } },
        /* TrackElemType::DiagDown25LeftBanked */
        { 4, { kDiagDown25LeftBankedSeq0, kDiagDown25LeftBankedSeq1, kDiagDown25LeftBankedSeq2, kDiagDown25LeftBankedSeq3 } },
        /* TrackElemType::DiagDown25RightBanked */
        { 4,
          { kDiagDown25RightBankedSeq0, kDiagDown25RightBankedSeq1, kDiagDown25RightBankedSeq2, kDiagDown25RightBankedSeq3 } },
        /* TrackElemType::LeftEighthBankToDiagUp25 */
        { 5,
          { kLeftEighthBankToDiagUp25Seq0, kLeftEighthBankToDiagUp25Seq1, kLeftEighthBankToDiagUp25Seq2,
            kLeftEighthBankToDiagUp25Seq3, kLeftEighthBankToDiagUp25Seq4 } },
        /* TrackElemType::RightEighthBankToDiagUp25 */
        { 5,
          { kRightEighthBankToDiagUp25Seq0, kRightEighthBankToDiagUp25Seq1, kRightEighthBankToDiagUp25Seq2,
            kRightEighthBankToDiagUp25Seq3, kRightEighthBankToDiagUp25Seq4 } },
        /* TrackElemType::LeftEighthBankToDiagDown25 */
        { 5,
          { kLeftEighthBankToDiagDown25Seq0, kLeftEighthBankToDiagDown25Seq1, kLeftEighthBankToDiagDown25Seq2,
            kLeftEighthBankToDiagDown25Seq3, kLeftEighthBankToDiagDown25Seq4 } },
        /* TrackElemType::RightEighthBankToDiagDown25 */
        { 5,
          { kRightEighthBankToDiagDown25Seq0, kRightEighthBankToDiagDown25Seq1, kRightEighthBankToDiagDown25Seq2,
            kRightEighthBankToDiagDown25Seq3, kRightEighthBankToDiagDown25Seq4 } },
        /* TrackElemType::LeftEighthBankToOrthogonalUp25 */
        { 5,
          { kLeftEighthBankToOrthogonalUp25Seq0, kLeftEighthBankToOrthogonalUp25Seq1, kLeftEighthBankToOrthogonalUp25Seq2,
            kLeftEighthBankToOrthogonalUp25Seq3, kLeftEighthBankToOrthogonalUp25Seq4 } },
        /* TrackElemType::RightEighthBankToOrthogonalUp25 */
        { 5,
          { kRightEighthBankToOrthogonalUp25Seq0, kRightEighthBankToOrthogonalUp25Seq1, kRightEighthBankToOrthogonalUp25Seq2,
            kRightEighthBankToOrthogonalUp25Seq3, kRightEighthBankToOrthogonalUp25Seq4 } },
        /* TrackElemType::LeftEighthBankToOrthogonalDown25 */
        { 5,
          { kLeftEighthBankToOrthogonalDown25Seq0, kLeftEighthBankToOrthogonalDown25Seq1, kLeftEighthBankToOrthogonalDown25Seq2,
            kLeftEighthBankToOrthogonalDown25Seq3, kLeftEighthBankToOrthogonalDown25Seq4 } },
        /* TrackElemType::RightEighthBankToOrthogonalDown25 */
        { 5,
          { kRightEighthBankToOrthogonalDown25Seq0, kRightEighthBankToOrthogonalDown25Seq1,
            kRightEighthBankToOrthogonalDown25Seq2, kRightEighthBankToOrthogonalDown25Seq3,
            kRightEighthBankToOrthogonalDown25Seq4 } },
        /* TrackElemType::DiagBrakes */ { 4, { kDiagBrakesSeq0, kDiagBrakesSeq1, kDiagBrakesSeq2, kDiagBrakesSeq3 } },
        /* TrackElemType::DiagBlockBrakes */
        { 4, { kDiagBlockBrakesSeq0, kDiagBlockBrakesSeq1, kDiagBlockBrakesSeq2, kDiagBlockBrakesSeq3 } },
        /* TrackElemType::InclinedBrakes */
        { 1, { kDown25Seq0 } },
        /* TrackElemType::DiagBooster */ { 4, { kDiagFlatSeq0, kDiagFlatSeq1, kDiagFlatSeq2, kDiagFlatSeq3 } },
        /* TrackElemType::DiagFlatToUp60LongBase */
        { 10,
          { kDiagFlatToUp60LongBaseSeq0, kDiagFlatToUp60LongBaseSeq1, kDiagFlatToUp60LongBaseSeq2, kDiagFlatToUp60LongBaseSeq3,
            kDiagFlatToUp60LongBaseSeq4, kDiagFlatToUp60LongBaseSeq5, kDiagFlatToUp60LongBaseSeq6, kDiagFlatToUp60LongBaseSeq7,
            kDiagFlatToUp60LongBaseSeq8, kDiagFlatToUp60LongBaseSeq9 } },
        /* TrackElemType::DiagUp60ToFlatLongBase */
        { 10,
          { kDiagUp60ToFlatLongBaseSeq0, kDiagUp60ToFlatLongBaseSeq1, kDiagUp60ToFlatLongBaseSeq2, kDiagUp60ToFlatLongBaseSeq3,
            kDiagUp60ToFlatLongBaseSeq4, kDiagUp60ToFlatLongBaseSeq5, kDiagUp60ToFlatLongBaseSeq6, kDiagUp60ToFlatLongBaseSeq7,
            kDiagUp60ToFlatLongBaseSeq8, kDiagUp60ToFlatLongBaseSeq9 } },
        /* TrackElemType::DiagFlatToDown60LongBase */
        { 10,
          { kDiagFlatToDown60LongBaseSeq0, kDiagFlatToDown60LongBaseSeq1, kDiagFlatToDown60LongBaseSeq2,
            kDiagFlatToDown60LongBaseSeq3, kDiagFlatToDown60LongBaseSeq4, kDiagFlatToDown60LongBaseSeq5,
            kDiagFlatToDown60LongBaseSeq6, kDiagFlatToDown60LongBaseSeq7, kDiagFlatToDown60LongBaseSeq8,
            kDiagFlatToDown60LongBaseSeq9 } },
        /* TrackElemType::DiagDown60ToFlatLongBase */
        { 10,
          { kDiagDown60ToFlatLongBaseSeq0, kDiagDown60ToFlatLongBaseSeq1, kDiagDown60ToFlatLongBaseSeq2,
            kDiagDown60ToFlatLongBaseSeq3, kDiagDown60ToFlatLongBaseSeq4, kDiagDown60ToFlatLongBaseSeq5,
            kDiagDown60ToFlatLongBaseSeq6, kDiagDown60ToFlatLongBaseSeq7, kDiagDown60ToFlatLongBaseSeq8,
            kDiagDown60ToFlatLongBaseSeq9 } },
        /* TrackElemType::LeftEighthDiveLoopUpToOrthogonal */
        { 6,
          { kLeftEighthDiveLoopUpToOrthogonalSeq0, kLeftEighthDiveLoopUpToOrthogonalSeq1, kLeftEighthDiveLoopUpToOrthogonalSeq2,
            kLeftEighthDiveLoopUpToOrthogonalSeq3, kLeftEighthDiveLoopUpToOrthogonalSeq4,
            kLeftEighthDiveLoopUpToOrthogonalSeq5 } },
        /* TrackElemType::RightEighthDiveLoopUpToOrthogonal */
        { 6,
          { kRightEighthDiveLoopUpToOrthogonalSeq0, kRightEighthDiveLoopUpToOrthogonalSeq1,
            kRightEighthDiveLoopUpToOrthogonalSeq2, kRightEighthDiveLoopUpToOrthogonalSeq3,
            kRightEighthDiveLoopUpToOrthogonalSeq4, kRightEighthDiveLoopUpToOrthogonalSeq5 } },
        /* TrackElemType::LeftEighthDiveLoopDownToDiag */
        { 6,
          { kLeftEighthDiveLoopDownToDiagSeq0, kLeftEighthDiveLoopDownToDiagSeq1, kLeftEighthDiveLoopDownToDiagSeq2,
            kLeftEighthDiveLoopDownToDiagSeq3, kLeftEighthDiveLoopDownToDiagSeq4, kLeftEighthDiveLoopDownToDiagSeq5 } },
        /* TrackElemType::RightEighthDiveLoopDownToDiag */
        { 6,
          { kRightEighthDiveLoopDownToDiagSeq0, kRightEighthDiveLoopDownToDiagSeq1, kRightEighthDiveLoopDownToDiagSeq2,
            kRightEighthDiveLoopDownToDiagSeq3, kRightEighthDiveLoopDownToDiagSeq4, kRightEighthDiveLoopDownToDiagSeq5 } },
    };

#pragma endregion

    static constexpr auto BuildDescriptorTable()
    {
        std::array<TrackElementDescriptor, EnumValue(TrackElemType::Count)> res{};

        for (int i = 0; i < EnumValue(TrackElemType::Count); i++)
        {
            TrackElementDescriptor& desc = res[i];
            desc.description = kRideConfigurationStringIds[i];
            desc.alternativeType = kAlternativeTrackTypes[i];
            desc.coordinates = kTrackCoordinates[i];
            desc.curveChain = kTrackCurveChain[i];
            desc.flags = kTrackFlags[i];
            desc.heightMarkerPositions = kTrackHeightMarkerPositions[i];
            desc.mirrorElement = kTrackElementMirrorMap[i];
            desc.pieceLength = kTrackPieceLengths[i];
            desc.priceModifier = kTrackPricing[i];
            desc.definition = kTrackDefinitions[i];
            desc.spinFunction = kTrackTypeToSpinFunction[i];
            desc.verticalFactor = GetVerticalFunction(static_cast<TrackElemType>(i));
            desc.lateralFactor = GetLateralFunction(static_cast<TrackElemType>(i));
            desc.numSequences = kSequenceDescriptorsByElement[i].numSequences;

            for (uint8_t sequenceIndex = 0; sequenceIndex < kMaxSequencesPerPiece; sequenceIndex++)
            {
                desc.sequences[sequenceIndex] = kSequenceDescriptorsByElement[i].sequences[sequenceIndex];
            }
        }

        return res;
    }

    static constexpr auto kTrackElementDescriptors = BuildDescriptorTable();

    const TrackElementDescriptor& GetTrackElementDescriptor(OpenRCT2::TrackElemType type)
    {
        if (EnumValue(type) >= kTrackElementDescriptors.size())
            return kTrackElementDescriptors[0];
        return kTrackElementDescriptors[EnumValue(type)];
    }

} // namespace OpenRCT2::TrackMetaData
