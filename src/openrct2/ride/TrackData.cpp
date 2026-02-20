/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
            {    0,    2,    0,    0,    0,   32 }, // TrackElemType::flatTrack1x4A
            {    0,    2,    0,    0,    0,   32 }, // TrackElemType::flatTrack2x2
            {    0,    2,    0,    0,    0,   32 }, // TrackElemType::flatTrack4x4
            {    0,    2,    0,    0,    0,   32 }, // TrackElemType::flatTrack2x4
            {    0,    2,    0,    0,    0,   32 }, // TrackElemType::flatTrack1x5
            {    0,    2,    0,    0,    0,   32 }, // TrackElemType::flatTrack1x1A
            {    0,    2,    0,    0,    0,   32 }, // TrackElemType::flatTrack1x4B
            {    0,    2,    0,    0,    0,   32 }, // TrackElemType::flatTrack1x1B
            {    0,    2,    0,    0,    0,   32 }, // TrackElemType::flatTrack1x4C
            {    0,    0,    0,   96,   32,    0 }, // TrackElemType::flatTrack3x3
            { 0, 3, 0, 112, -64, -64 },  // TrackElemType::leftCorkscrewUp
            { 0, 1, 0, 112, -64, 64 },   // TrackElemType::rightCorkscrewUp
            { 0, 3, 0, -112, -64, -64 }, // TrackElemType::leftCorkscrewDown
            { 0, 1, 0, -112, -64, 64 },  // TrackElemType::rightCorkscrewDown
            { 0, 2, 0, 216, -32, -32 },  // TrackElemType::leftMediumHalfLoopUp
            { 0, 2, 0, 216, -32, 32 },   // TrackElemType::rightMediumHalfLoopUp
            { 0, 2, 0, -216, 32, -32 },  // TrackElemType::leftMediumHalfLoopDown
            { 0, 2, 0, -216, 32, 32 },   // TrackElemType::rightMediumHalfLoopDown
            { 0, 0, 0, 56, -64, 0 },     // TrackElemType::leftZeroGRollUp
            { 0, 0, 0, 56, -64, 0 },     // TrackElemType::rightZeroGRollUp
            { 0, 0, 0, -56,-64, 0 },     // TrackElemType::leftZeroGRollDown
            { 0, 0, 0, -56,-64, 0 },     // TrackElemType::rightZeroGRollDown
            { 0, 0, 0, 152, -96, 0 },    // TrackElemType::leftLargeZeroGRollUp
            { 0, 0, 0, 152, -96, 0 },    // TrackElemType::rightLargeZeroGRollUp
            { 0, 0, 0, -152,-96, 0 },    // TrackElemType::leftLargeZeroGRollDown
            { 0, 0, 0, -152,-96, 0 },    // TrackElemType::rightLargeZeroGRollDown
            { 0, 2, 0, 248, -64, -32 },  // TrackElemType::leftFlyerLargeHalfLoopUninvertedUp
            { 0, 2, 0, 248, -64, 32 },   // TrackElemType::rightFlyerLargeHalfLoopUninvertedUp
            { 0, 2, 0, -248, 64, -32 },  // TrackElemType::leftFlyerLargeHalfLoopInvertedDown
            { 0, 2, 0, -248, 64, 32 },   // TrackElemType::rightFlyerLargeHalfLoopInvertedDown
            { 0, 2, 0, 280, -64, -32 },  // TrackElemType::leftFlyerLargeHalfLoopInvertedUp
            { 0, 2, 0, 280, -64, 32 },   // TrackElemType::rightFlyerLargeHalfLoopInvertedUp
            { 0, 2, 0, -280, 64, -32 },  // TrackElemType::leftFlyerLargeHalfLoopUninvertedDown
            { 0, 2, 0, -280, 64, 32 },   // TrackElemType::rightFlyerLargeHalfLoopUninvertedDown
            { 0, 2, 0, 152, -32, 0 },    // TrackElemType::flyerHalfLoopInvertedUp
            { 0, 2, 0, -152, 32, 0 },    // TrackElemType::flyerHalfLoopUninvertedDown
            { 0, 7, 0, 48, -64, -32 }, // TrackElemType::leftEighthToDiagUp25
            { 0, 4, 0, 48, -64, 32 },  // TrackElemType::rightEighthToDiagUp25
            { 0, 7, 0,-48, -64, -32 }, // TrackElemType::leftEighthToDiagDown25
            { 0, 4, 0,-48, -64, 32 },  // TrackElemType::rightEighthToDiagDown25
            { 4, 0, 0, 48, -64, 32 },  // TrackElemType::leftEighthToOrthogonalUp25
            { 4, 1, 0, 48, -32, 64 },  // TrackElemType::rightEighthToOrthogonalUp25
            { 4, 0, 0,-48, -64, 32 },  // TrackElemType::leftEighthToOrthogonalDown25
            { 4, 1, 0,-48, -32, 64 },  // TrackElemType::rightEighthToOrthogonalDown25
        { 4, 4, 0, 16, -32, 32 },  // TrackElemType::diagUp25ToLeftBankedUp25
        { 4, 4, 0, 16, -32, 32 },  // TrackElemType::diagUp25ToRightBankedUp25
        { 4, 4, 0, 16, -32, 32 },  // TrackElemType::diagLeftBankedUp25ToUp25
        { 4, 4, 0, 16, -32, 32 },  // TrackElemType::diagRightBankedUp25ToUp25
        { 4, 4, 16, 0, -32, 32 },  // TrackElemType::diagDown25ToLeftBankedDown25
        { 4, 4, 16, 0, -32, 32 },  // TrackElemType::diagDown25ToRightBankedDown25
        { 4, 4, 16, 0, -32, 32 },  // TrackElemType::diagLeftBankedDown25ToDown25
        { 4, 4, 16, 0, -32, 32 },  // TrackElemType::diagRightBankedDown25ToDown25
        { 4, 4, 0, 8, -32, 32 },   // TrackElemType::diagLeftBankedFlatToLeftBankedUp25
        { 4, 4, 0, 8, -32, 32 },   // TrackElemType::diagRightBankedFlatToRightBankedUp25
        { 4, 4, 0, 8, -32, 32 },   // TrackElemType::diagLeftBankedUp25ToLeftBankedFlat
        { 4, 4, 0, 8, -32, 32 },   // TrackElemType::diagRightBankedUp25ToRightBankedFlat
        { 4, 4, 8, 0, -32, 32 },   // TrackElemType::diagLeftBankedFlatToLeftBankedDown25
        { 4, 4, 8, 0, -32, 32 },   // TrackElemType::diagRightBankedFlatToRightBankedDown25
        { 4, 4, 8, 0, -32, 32 },   // TrackElemType::diagLeftBankedDown25ToLeftBankedFlat
        { 4, 4, 8, 0, -32, 32 },   // TrackElemType::diagRightBankedDown25ToRightBankedFlat
        { 4, 4, 0, 8, -32, 32 },   // TrackElemType::diagFlatToLeftBankedUp25
        { 4, 4, 0, 8, -32, 32 },   // TrackElemType::diagFlatToRightBankedUp25
        { 4, 4, 0, 8, -32, 32 },   // TrackElemType::diagLeftBankedUp25ToFlat
        { 4, 4, 0, 8, -32, 32 },   // TrackElemType::diagRightBankedUp25ToFlat
        { 4, 4, 8, 0, -32, 32 },   // TrackElemType::diagFlatToLeftBankedDown25
        { 4, 4, 8, 0, -32, 32 },   // TrackElemType::diagFlatToRightBankedDown25
        { 4, 4, 8, 0, -32, 32 },   // TrackElemType::diagLeftBankedDown25ToFlat
        { 4, 4, 8, 0, -32, 32 },   // TrackElemType::diagRightBankedDown25ToFlat
        { 4, 4, 0, 16, -32, 32 },  // TrackElemType::diagUp25LeftBanked
        { 4, 4, 0, 16, -32, 32 },  // TrackElemType::diagUp25RightBanked
        { 4, 4, 16, 0, -32, 32 },  // TrackElemType::diagDown25LeftBanked
        { 4, 4, 16, 0, -32, 32 },  // TrackElemType::diagDown25RightBanked
        { 0, 7, 0, 48, -64, -32 }, // TrackElemType::leftEighthBankToDiagUp25
        { 0, 4, 0, 48, -64, 32 },  // TrackElemType::rightEighthBankToDiagUp25
        { 0, 7, 0,-48, -64, -32 }, // TrackElemType::leftEighthBankToDiagDown25
        { 0, 4, 0,-48, -64, 32 },  // TrackElemType::rightEighthBankToDiagDown25
        { 4, 0, 0, 48, -64, 32 },  // TrackElemType::leftEighthBankToOrthogonalUp25
        { 4, 1, 0, 48, -32, 64 },  // TrackElemType::rightEighthBankToOrthogonalUp25
        { 4, 0, 0,-48, -64, 32 },  // TrackElemType::leftEighthBankToOrthogonalDown25
        { 4, 1, 0,-48, -32, 64 },  // TrackElemType::rightEighthBankToOrthogonalDown25
        { 4, 4, 0, 0, -32, 32 },   // TrackElemType::diagBrakes
        { 4, 4, 0, 0, -32, 32 },   // TrackElemType::diagBlockBrakes
        { 0, 0, 16, 0, 0, 0 },     // TrackElemType::down25Brakes
        { 4, 4, 0, 0, -32, 32 },   // TrackElemType::diagBooster
        { 4, 4, 0, 88, -96, 96 },   // TrackElemType::diagFlatToUp60LongBase,
        { 4, 4, 0, 88, -96, 96 },   // TrackElemType::diagUp60ToFlatLongBase,
        { 4, 4, 88, 0, -96, 96 },   // TrackElemType::diagFlatToDown60LongBase,
        { 4, 4, 88, 0, -96, 96 },   // TrackElemType::diagDown60ToFlatLongBase,
        { 4, 0, 0,152, -96, 32 },   // TrackElemType::leftEighthDiveLoopUpToOrthogonal
        { 4, 1, 0,152, -32, 96 },   // TrackElemType::rightEighthDiveLoopUpToOrthogonal
        { 0, 7,152, 0, -96, -32 },   // TrackElemType::leftEighthDiveLoopDownToDiag
        { 0, 4,152, 0, -96, 32 },   // TrackElemType::rightEighthDiveLoopDownToDiag
        { 4, 4, 16, 0, -32, 32 }   // TrackElemType::diagDown25Brakes
    };
    static_assert(std::size(kTrackCoordinates) == EnumValue(TrackElemType::count));

    static constexpr uint8_t kTrackPieceLengths[] = {
        32,     // TrackElemType::flat
        32,     // TrackElemType::endStation
        32,     // TrackElemType::beginStation
        32,     // TrackElemType::middleStation
        33,     // TrackElemType::up25
        40,     // TrackElemType::up60
        32,     // TrackElemType::flatToUp25
        34,     // TrackElemType::up25ToUp60
        34,     // TrackElemType::up60ToUp25
        32,     // TrackElemType::up25ToFlat
        33,     // TrackElemType::down25
        40,     // TrackElemType::down60
        32,     // TrackElemType::flatToDown25
        34,     // TrackElemType::down25ToDown60
        34,     // TrackElemType::down60ToDown25
        32,     // TrackElemType::down25ToFlat
        124,    // TrackElemType::leftQuarterTurn5Tiles
        124,    // TrackElemType::rightQuarterTurn5Tiles
        32,     // TrackElemType::flatToLeftBank
        32,     // TrackElemType::flatToRightBank
        32,     // TrackElemType::leftBankToFlat
        32,     // TrackElemType::rightBankToFlat
        124,    // TrackElemType::bankedLeftQuarterTurn5Tiles
        124,    // TrackElemType::bankedRightQuarterTurn5Tiles
        32,     // TrackElemType::leftBankToUp25
        32,     // TrackElemType::rightBankToUp25
        32,     // TrackElemType::up25ToLeftBank
        32,     // TrackElemType::up25ToRightBank
        32,     // TrackElemType::leftBankToDown25
        32,     // TrackElemType::rightBankToDown25
        32,     // TrackElemType::down25ToLeftBank
        32,     // TrackElemType::down25ToRightBank
        32,     // TrackElemType::leftBank
        32,     // TrackElemType::rightBank
        130,    // TrackElemType::leftQuarterTurn5TilesUp25
        130,    // TrackElemType::rightQuarterTurn5TilesUp25
        130,    // TrackElemType::leftQuarterTurn5TilesDown25
        130,    // TrackElemType::rightQuarterTurn5TilesDown25
        96,     // TrackElemType::sBendLeft
        96,     // TrackElemType::sBendRight
        120,    // TrackElemType::leftVerticalLoop
        120,    // TrackElemType::rightVerticalLoop
        75,     // TrackElemType::leftQuarterTurn3Tiles
        75,     // TrackElemType::rightQuarterTurn3Tiles
        75,     // TrackElemType::leftBankedQuarterTurn3Tiles
        75,     // TrackElemType::rightBankedQuarterTurn3Tiles
        77,     // TrackElemType::leftQuarterTurn3TilesUp25
        77,     // TrackElemType::rightQuarterTurn3TilesUp25
        77,     // TrackElemType::leftQuarterTurn3TilesDown25
        77,     // TrackElemType::rightQuarterTurn3TilesDown25
        24,     // TrackElemType::leftQuarterTurn1Tile
        24,     // TrackElemType::rightQuarterTurn1Tile
        96,     // TrackElemType::leftTwistDownToUp
        96,     // TrackElemType::rightTwistDownToUp
        96,     // TrackElemType::leftTwistUpToDown
        96,     // TrackElemType::rightTwistUpToDown
        60,     // TrackElemType::halfLoopUp
        60,     // TrackElemType::halfLoopDown
        55,     // TrackElemType::leftCorkscrewUp
        55,     // TrackElemType::rightCorkscrewUp
        55,     // TrackElemType::leftCorkscrewDown
        55,     // TrackElemType::rightCorkscrewDown
        36,     // TrackElemType::flatToUp60
        36,     // TrackElemType::up60ToFlat
        36,     // TrackElemType::flatToDown60
        36,     // TrackElemType::down60ToFlat
        32,     // TrackElemType::towerBase
        32,     // TrackElemType::towerSection
        32,     // TrackElemType::flatCovered
        33,     // TrackElemType::up25Covered
        40,     // TrackElemType::up60Covered
        32,     // TrackElemType::flatToUp25Covered
        34,     // TrackElemType::up25ToUp60Covered
        34,     // TrackElemType::up60ToUp25Covered
        32,     // TrackElemType::up25ToFlatCovered
        33,     // TrackElemType::down25Covered
        40,     // TrackElemType::down60Covered
        32,     // TrackElemType::flatToDown25Covered
        34,     // TrackElemType::down25ToDown60Covered
        34,     // TrackElemType::down60ToDown25Covered
        32,     // TrackElemType::down25ToFlatCovered
        124,    // TrackElemType::leftQuarterTurn5TilesCovered
        124,    // TrackElemType::rightQuarterTurn5TilesCovered
        96,     // TrackElemType::sBendLeftCovered
        96,     // TrackElemType::sBendRightCovered
        75,     // TrackElemType::leftQuarterTurn3TilesCovered
        75,     // TrackElemType::rightQuarterTurn3TilesCovered
        150,    // TrackElemType::leftHalfBankedHelixUpSmall
        150,    // TrackElemType::rightHalfBankedHelixUpSmall
        150,    // TrackElemType::leftHalfBankedHelixDownSmall
        150,    // TrackElemType::rightHalfBankedHelixDownSmall
        248,    // TrackElemType::leftHalfBankedHelixUpLarge
        248,    // TrackElemType::rightHalfBankedHelixUpLarge
        248,    // TrackElemType::leftHalfBankedHelixDownLarge
        248,    // TrackElemType::rightHalfBankedHelixDownLarge
        64,     // TrackElemType::leftQuarterTurn1TileUp60
        64,     // TrackElemType::rightQuarterTurn1TileUp60
        64,     // TrackElemType::leftQuarterTurn1TileDown60
        64,     // TrackElemType::rightQuarterTurn1TileDown60
        32,     // TrackElemType::brakes
        32,     // TrackElemType::booster
        32,     // TrackElemType::invertedUp90ToFlatQuarterLoopAlias / Maze
        124,    // TrackElemType::leftQuarterBankedHelixLargeUp
        124,    // TrackElemType::rightQuarterBankedHelixLargeUp
        124,    // TrackElemType::leftQuarterBankedHelixLargeDown
        124,    // TrackElemType::rightQuarterBankedHelixLargeDown
        124,    // TrackElemType::leftQuarterHelixLargeUp
        124,    // TrackElemType::rightQuarterHelixLargeUp
        124,    // TrackElemType::leftQuarterHelixLargeDown
        124,    // TrackElemType::rightQuarterHelixLargeDown
        33,     // TrackElemType::up25LeftBanked
        33,     // TrackElemType::up25RightBanked
        32,     // TrackElemType::waterfall
        32,     // TrackElemType::rapids
        32,     // TrackElemType::onRidePhoto
        33,     // TrackElemType::down25LeftBanked
        33,     // TrackElemType::down25RightBanked
        128,    // TrackElemType::watersplash
        165,    // TrackElemType::flatToUp60LongBase
        165,    // TrackElemType::up60ToFlatLongBase
        32,     // TrackElemType::whirlpool
        165,    // TrackElemType::down60ToFlatLongBase
        165,    // TrackElemType::flatToDown60LongBase
        138,    // TrackElemType::cableLiftHill
        32,     // TrackElemType::reverseFreefallSlope
        32,     // TrackElemType::reverseFreefallVertical
        32,     // TrackElemType::up90
        32,     // TrackElemType::down90
        32,     // TrackElemType::up60ToUp90
        32,     // TrackElemType::down90ToDown60
        32,     // TrackElemType::up90ToUp60
        32,     // TrackElemType::down60ToDown90
        32,     // TrackElemType::brakeForDrop
        87,     // TrackElemType::leftEighthToDiag
        87,     // TrackElemType::rightEighthToDiag
        87,     // TrackElemType::leftEighthToOrthogonal
        87,     // TrackElemType::rightEighthToOrthogonal
        87,     // TrackElemType::leftEighthBankToDiag
        87,     // TrackElemType::rightEighthBankToDiag
        87,     // TrackElemType::leftEighthBankToOrthogonal
        87,     // TrackElemType::rightEighthBankToOrthogonal
        45,     // TrackElemType::diagFlat
        45,     // TrackElemType::diagUp25
        45,     // TrackElemType::diagUp60
        45,     // TrackElemType::diagFlatToUp25
        45,     // TrackElemType::diagUp25ToUp60
        45,     // TrackElemType::diagUp60ToUp25
        45,     // TrackElemType::diagUp25ToFlat
        45,     // TrackElemType::diagDown25
        45,     // TrackElemType::diagDown60
        45,     // TrackElemType::diagFlatToDown25
        45,     // TrackElemType::diagDown25ToDown60
        45,     // TrackElemType::diagDown60ToDown25
        45,     // TrackElemType::diagDown25ToFlat
        45,     // TrackElemType::diagFlatToUp60
        45,     // TrackElemType::diagUp60ToFlat
        45,     // TrackElemType::diagFlatToDown60
        45,     // TrackElemType::diagDown60ToFlat
        45,     // TrackElemType::diagFlatToLeftBank
        45,     // TrackElemType::diagFlatToRightBank
        45,     // TrackElemType::diagLeftBankToFlat
        45,     // TrackElemType::diagRightBankToFlat
        45,     // TrackElemType::diagLeftBankToUp25
        45,     // TrackElemType::diagRightBankToUp25
        45,     // TrackElemType::diagUp25ToLeftBank
        45,     // TrackElemType::diagUp25ToRightBank
        45,     // TrackElemType::diagLeftBankToDown25
        45,     // TrackElemType::diagRightBankToDown25
        45,     // TrackElemType::diagDown25ToLeftBank
        45,     // TrackElemType::diagDown25ToRightBank
        45,     // TrackElemType::diagLeftBank
        45,     // TrackElemType::diagRightBank
        16,     // TrackElemType::logFlumeReverser
        32,     // TrackElemType::spinningTunnel
        96,     // TrackElemType::leftBarrelRollUpToDown
        96,     // TrackElemType::rightBarrelRollUpToDown
        96,     // TrackElemType::leftBarrelRollDownToUp
        96,     // TrackElemType::rightBarrelRollDownToUp
        64,     // TrackElemType::leftBankToLeftQuarterTurn3TilesUp25
        64,     // TrackElemType::rightBankToRightQuarterTurn3TilesUp25
        64,     // TrackElemType::leftQuarterTurn3TilesDown25ToLeftBank
        64,     // TrackElemType::rightQuarterTurn3TilesDown25ToRightBank
        32,     // TrackElemType::poweredLift
        100,    // TrackElemType::leftLargeHalfLoopUp
        100,    // TrackElemType::rightLargeHalfLoopUp
        100,    // TrackElemType::leftLargeHalfLoopDown
        100,    // TrackElemType::rightLargeHalfLoopDown
        96,     // TrackElemType::leftFlyerTwistUp
        96,     // TrackElemType::rightFlyerTwistUp
        96,     // TrackElemType::leftFlyerTwistDown
        96,     // TrackElemType::rightFlyerTwistDown
        64,     // TrackElemType::flyerHalfLoopUninvertedUp
        64,     // TrackElemType::flyerHalfLoopInvertedDown
        64,     // TrackElemType::leftFlyerCorkscrewUp
        64,     // TrackElemType::rightFlyerCorkscrewUp
        64,     // TrackElemType::leftFlyerCorkscrewDown
        64,     // TrackElemType::rightFlyerCorkscrewDown
        16,     // TrackElemType::heartLineTransferUp
        16,     // TrackElemType::heartLineTransferDown
        64,     // TrackElemType::leftHeartLineRoll
        64,     // TrackElemType::rightHeartLineRoll
        32,     // TrackElemType::minigolfHoleA
        32,     // TrackElemType::minigolfHoleB
        32,     // TrackElemType::minigolfHoleC
        32,     // TrackElemType::minigolfHoleD
        32,     // TrackElemType::minigolfHoleE
        80,     // TrackElemType::multiDimInvertedFlatToDown90QuarterLoop
        80,     // TrackElemType::up90ToInvertedFlatQuarterLoop
        80,     // TrackElemType::invertedFlatToDown90QuarterLoop
        64,     // TrackElemType::leftCurvedLiftHill
        64,     // TrackElemType::rightCurvedLiftHill
        32,     // TrackElemType::leftReverser
        32,     // TrackElemType::rightReverser
        32,     // TrackElemType::airThrustTopCap
        32,     // TrackElemType::airThrustVerticalDown
        32,     // TrackElemType::airThrustVerticalDownToLevel
        32,     // TrackElemType::blockBrakes
        32,     // TrackElemType::leftBankedQuarterTurn3TileUp25
        32,     // TrackElemType::rightBankedQuarterTurn3TileUp25
        32,     // TrackElemType::leftBankedQuarterTurn3TileDown25
        32,     // TrackElemType::rightBankedQuarterTurn3TileDown25
        32,     // TrackElemType::leftBankedQuarterTurn5TileUp25
        32,     // TrackElemType::rightBankedQuarterTurn5TileUp25
        32,     // TrackElemType::leftBankedQuarterTurn5TileDown25
        32,     // TrackElemType::rightBankedQuarterTurn5TileDown25
        32,     // TrackElemType::up25ToLeftBankedUp25
        32,     // TrackElemType::up25ToRightBankedUp25
        32,     // TrackElemType::leftBankedUp25ToUp25
        32,     // TrackElemType::rightBankedUp25ToUp25
        32,     // TrackElemType::down25ToLeftBankedDown25
        32,     // TrackElemType::down25ToRightBankedDown25
        32,     // TrackElemType::leftBankedDown25ToDown25
        32,     // TrackElemType::rightBankedDown25ToDown25
        32,     // TrackElemType::leftBankedFlatToLeftBankedUp25
        32,     // TrackElemType::rightBankedFlatToRightBankedUp25
        32,     // TrackElemType::leftBankedUp25ToLeftBankedFlat
        32,     // TrackElemType::rightBankedUp25ToRightBankedFlat
        32,     // TrackElemType::leftBankedFlatToLeftBankedDown25
        32,     // TrackElemType::rightBankedFlatToRightBankedDown25
        32,     // TrackElemType::leftBankedDown25ToLeftBankedFlat
        32,     // TrackElemType::rightBankedDown25ToRightBankedFlat
        32,     // TrackElemType::flatToLeftBankedUp25
        32,     // TrackElemType::flatToRightBankedUp25
        32,     // TrackElemType::leftBankedUp25ToFlat
        32,     // TrackElemType::rightBankedUp25ToFlat
        32,     // TrackElemType::flatToLeftBankedDown25
        32,     // TrackElemType::flatToRightBankedDown25
        32,     // TrackElemType::leftBankedDown25ToFlat
        32,     // TrackElemType::rightBankedDown25ToFlat
        32,     // TrackElemType::leftQuarterTurn1TileUp90
        32,     // TrackElemType::rightQuarterTurn1TileUp90
        32,     // TrackElemType::leftQuarterTurn1TileDown90
        32,     // TrackElemType::rightQuarterTurn1TileDown90
        80,     // TrackElemType::multiDimUp90ToInvertedFlatQuarterLoop
        80,     // TrackElemType::multiDimFlatToDown90QuarterLoop
        80,     // TrackElemType::multiDimInvertedUp90ToFlatQuarterLoop
        32,     // TrackElemType::rotationControlToggle
        0,      // TrackElemType::flatTrack1x4A
        0,      // TrackElemType::flatTrack2x2
        0,      // TrackElemType::flatTrack4x4
        0,      // TrackElemType::flatTrack2x4
        0,      // TrackElemType::flatTrack1x5
        0,      // TrackElemType::flatTrack1x1A
        0,      // TrackElemType::flatTrack1x4B
        0,      // TrackElemType::flatTrack1x1B
        0,      // TrackElemType::flatTrack1x4C
        0,      // TrackElemType::flatTrack3x3
        96,     // TrackElemType::leftCorkscrewUp
        96,     // TrackElemType::rightCorkscrewUp
        96,     // TrackElemType::leftCorkscrewDown
        96,     // TrackElemType::rightCorkscrewDown
        80,     // TrackElemType::leftMediumHalfLoopUp
        80,     // TrackElemType::rightMediumHalfLoopUp
        80,     // TrackElemType::leftMediumHalfLoopDown
        80,     // TrackElemType::rightMediumHalfLoopDown
        96,     // TrackElemType::leftZeroGRollUp
        96,     // TrackElemType::rightZeroGRollUp
        96,     // TrackElemType::leftZeroGRollDown
        96,     // TrackElemType::rightZeroGRollDown
        165,    // TrackElemType::leftLargeZeroGRollUp
        165,    // TrackElemType::rightLargeZeroGRollUp
        165,    // TrackElemType::leftLargeZeroGRollDown
        165,    // TrackElemType::rightLargeZeroGRollDown
        100,    // TrackElemType::leftFlyerLargeHalfLoopUninvertedUp
        100,    // TrackElemType::rightFlyerLargeHalfLoopUninvertedUp
        100,    // TrackElemType::leftFlyerLargeHalfLoopInvertedDown
        100,    // TrackElemType::rightFlyerLargeHalfLoopInvertedDown
        100,    // TrackElemType::leftFlyerLargeHalfLoopInvertedUp
        100,    // TrackElemType::rightFlyerLargeHalfLoopInvertedUp
        100,    // TrackElemType::leftFlyerLargeHalfLoopUninvertedDown
        100,    // TrackElemType::rightFlyerLargeHalfLoopUninvertedDown
        64,     // TrackElemType::flyerHalfLoopInvertedUp
        64,     // TrackElemType::flyerHalfLoopUninvertedDown
        92, // TrackElemType::leftEighthToDiagUp25
        92, // TrackElemType::rightEighthToDiagUp25
        92, // TrackElemType::leftEighthToDiagDown25
        92, // TrackElemType::rightEighthToDiagDown25
        92, // TrackElemType::leftEighthToOrthogonalUp25
        92, // TrackElemType::rightEighthToOrthogonalUp25
        92, // TrackElemType::leftEighthToOrthogonalDown25
        92, // TrackElemType::rightEighthToOrthogonalDown25
        45, // TrackElemType::diagUp25ToLeftBankedUp25
        45, // TrackElemType::diagUp25ToRightBankedUp25
        45, // TrackElemType::diagLeftBankedUp25ToUp25
        45, // TrackElemType::diagRightBankedUp25ToUp25
        45, // TrackElemType::diagDown25ToLeftBankedDown25
        45, // TrackElemType::diagDown25ToRightBankedDown25
        45, // TrackElemType::diagLeftBankedDown25ToDown25
        45, // TrackElemType::diagRightBankedDown25ToDown25
        45, // TrackElemType::diagLeftBankedFlatToLeftBankedUp25
        45, // TrackElemType::diagRightBankedFlatToRightBankedUp25
        45, // TrackElemType::diagLeftBankedUp25ToLeftBankedFlat
        45, // TrackElemType::diagRightBankedUp25ToRightBankedFlat
        45, // TrackElemType::diagLeftBankedFlatToLeftBankedDown25
        45, // TrackElemType::diagRightBankedFlatToRightBankedDown25
        45, // TrackElemType::diagLeftBankedDown25ToLeftBankedFlat
        45, // TrackElemType::diagRightBankedDown25ToRightBankedFlat
        45, // TrackElemType::diagFlatToLeftBankedUp25
        45, // TrackElemType::diagFlatToRightBankedUp25
        45, // TrackElemType::diagLeftBankedUp25ToFlat
        45, // TrackElemType::diagRightBankedUp25ToFlat
        45, // TrackElemType::diagFlatToLeftBankedDown25
        45, // TrackElemType::diagFlatToRightBankedDown25
        45, // TrackElemType::diagLeftBankedDown25ToFlat
        45, // TrackElemType::diagRightBankedDown25ToFlat
        45, // TrackElemType::diagUp25LeftBanked
        45, // TrackElemType::diagUp25RightBanked
        45, // TrackElemType::diagDown25LeftBanked
        45, // TrackElemType::diagDown25RightBanked
        92, // TrackElemType::leftEighthBankToDiagUp25
        92, // TrackElemType::rightEighthBankToDiagUp25
        92, // TrackElemType::leftEighthBankToDiagDown25
        92, // TrackElemType::rightEighthBankToDiagDown25
        92, // TrackElemType::leftEighthBankToOrthogonalUp25
        92, // TrackElemType::rightEighthBankToOrthogonalUp25
        92, // TrackElemType::leftEighthBankToOrthogonalDown25
        92, // TrackElemType::rightEighthBankToOrthogonalDown25
        45, // TrackElemType::diagBrakes
        45, // TrackElemType::diagBlockBrakes
        33, // TrackElemType::down25Brakes
        45, // TrackElemType::diagBooster
        165, // TrackElemType::diagFlatToUp60LongBase,
        165, // TrackElemType::diagUp60ToFlatLongBase,
        165, // TrackElemType::diagFlatToDown60LongBase,
        165, // TrackElemType::diagDown60ToFlatLongBase,
        165, // TrackElemType::leftEighthDiveLoopUpToOrthogonal
        165, // TrackElemType::rightEighthDiveLoopUpToOrthogonal
        165, // TrackElemType::leftEighthDiveLoopDownToDiag
        165, // TrackElemType::rightEighthDiveLoopDownToDiag
        45, // TrackElemType::diagDown25Brakes
    };

    static_assert(std::size(kTrackPieceLengths) == EnumValue(TrackElemType::count));

    // rct2: 0x00998C95
    static constexpr TrackCurveChain kTrackCurveChain[] = {
        { TrackCurve::none, TrackCurve::none },
        { TrackElemType::endStation, TrackElemType::endStation },
        { TrackElemType::endStation, TrackElemType::endStation },
        { TrackElemType::endStation, TrackElemType::endStation },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::left, TrackCurve::left },
        { TrackCurve::right, TrackCurve::right },
        { TrackCurve::left, TrackCurve::none },
        { TrackCurve::right, TrackCurve::none },
        { TrackCurve::none, TrackCurve::left },
        { TrackCurve::none, TrackCurve::right },
        { TrackCurve::left, TrackCurve::left },
        { TrackCurve::right, TrackCurve::right },
        { TrackCurve::none, TrackCurve::left },
        { TrackCurve::none, TrackCurve::right },
        { TrackCurve::left, TrackCurve::none },
        { TrackCurve::right, TrackCurve::none },
        { TrackCurve::none, TrackCurve::left },
        { TrackCurve::none, TrackCurve::right },
        { TrackCurve::left, TrackCurve::none },
        { TrackCurve::right, TrackCurve::none },
        { TrackCurve::left, TrackCurve::left },
        { TrackCurve::right, TrackCurve::right },
        { TrackCurve::left, TrackCurve::left },
        { TrackCurve::right, TrackCurve::right },
        { TrackCurve::left, TrackCurve::left },
        { TrackCurve::right, TrackCurve::right },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::leftSmall, TrackCurve::leftSmall },
        { TrackCurve::rightSmall, TrackCurve::rightSmall },
        { TrackCurve::leftSmall, TrackCurve::leftSmall },
        { TrackCurve::rightSmall, TrackCurve::rightSmall },
        { TrackCurve::leftSmall, TrackCurve::leftSmall },
        { TrackCurve::rightSmall, TrackCurve::rightSmall },
        { TrackCurve::leftSmall, TrackCurve::leftSmall },
        { TrackCurve::rightSmall, TrackCurve::rightSmall },
        { TrackCurve::leftVerySmall, TrackCurve::leftVerySmall },
        { TrackCurve::rightVerySmall, TrackCurve::rightVerySmall },
        { TrackElemType::leftTwistUpToDown, TrackCurve::none },
        { TrackElemType::rightTwistUpToDown, TrackCurve::none },
        { TrackCurve::none, TrackElemType::leftTwistDownToUp },
        { TrackCurve::none, TrackElemType::rightTwistDownToUp },
        { TrackElemType::halfLoopDown, TrackCurve::none },
        { TrackCurve::none, TrackElemType::halfLoopUp },
        { TrackElemType::rightCorkscrewDown, TrackCurve::none },
        { TrackElemType::leftCorkscrewDown, TrackCurve::none },
        { TrackCurve::none, TrackElemType::rightCorkscrewUp },
        { TrackCurve::none, TrackElemType::leftCorkscrewUp },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackElemType::towerSection, TrackCurve::none },
        { TrackElemType::towerSection, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::left, TrackCurve::left },
        { TrackCurve::right, TrackCurve::right },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::leftSmall, TrackCurve::leftSmall },
        { TrackCurve::rightSmall, TrackCurve::rightSmall },
        { TrackElemType::leftHalfBankedHelixUpSmall, TrackElemType::leftHalfBankedHelixUpSmall },
        { TrackElemType::rightHalfBankedHelixUpSmall, TrackElemType::rightHalfBankedHelixUpSmall },
        { TrackElemType::leftHalfBankedHelixDownSmall, TrackElemType::leftHalfBankedHelixDownSmall },
        { TrackElemType::rightHalfBankedHelixDownSmall, TrackElemType::rightHalfBankedHelixDownSmall },
        { TrackElemType::leftHalfBankedHelixUpLarge, TrackElemType::leftHalfBankedHelixUpLarge },
        { TrackElemType::rightHalfBankedHelixUpLarge, TrackElemType::rightHalfBankedHelixUpLarge },
        { TrackElemType::leftHalfBankedHelixDownLarge, TrackElemType::leftHalfBankedHelixDownLarge },
        { TrackElemType::rightHalfBankedHelixDownLarge, TrackElemType::rightHalfBankedHelixDownLarge },
        { TrackCurve::leftSmall, TrackCurve::leftSmall },
        { TrackCurve::rightSmall, TrackCurve::rightSmall },
        { TrackCurve::leftSmall, TrackCurve::leftSmall },
        { TrackCurve::rightSmall, TrackCurve::rightSmall },
        { TrackElemType::brakes, TrackElemType::brakes },
        { TrackElemType::booster, TrackElemType::booster },
        { TrackCurve::none, TrackCurve::none },
        { TrackElemType::leftQuarterBankedHelixLargeUp, TrackElemType::leftQuarterBankedHelixLargeUp },
        { TrackElemType::rightQuarterBankedHelixLargeUp, TrackElemType::rightQuarterBankedHelixLargeUp },
        { TrackElemType::leftQuarterBankedHelixLargeDown, TrackElemType::leftQuarterBankedHelixLargeDown },
        { TrackElemType::rightQuarterBankedHelixLargeDown, TrackElemType::rightQuarterBankedHelixLargeDown },
        { TrackElemType::leftQuarterHelixLargeUp, TrackElemType::leftQuarterHelixLargeUp },
        { TrackElemType::rightQuarterHelixLargeUp, TrackElemType::rightQuarterHelixLargeUp },
        { TrackElemType::leftQuarterHelixLargeDown, TrackElemType::leftQuarterHelixLargeDown },
        { TrackElemType::rightQuarterHelixLargeDown, TrackElemType::rightQuarterHelixLargeDown },
        { TrackCurve::left, TrackCurve::left },
        { TrackCurve::right, TrackCurve::right },
        { TrackElemType::waterfall, TrackElemType::waterfall },
        { TrackElemType::rapids, TrackElemType::rapids },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::left, TrackCurve::left },
        { TrackCurve::right, TrackCurve::right },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackElemType::reverseFreefallVertical, TrackCurve::none },
        { TrackElemType::reverseFreefallVertical, TrackElemType::reverseFreefallVertical },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::leftLarge, TrackCurve::leftLarge },
        { TrackCurve::rightLarge, TrackCurve::rightLarge },
        { TrackCurve::leftLarge, TrackCurve::leftLarge },
        { TrackCurve::rightLarge, TrackCurve::rightLarge },
        { TrackCurve::leftLarge, TrackCurve::leftLarge },
        { TrackCurve::rightLarge, TrackCurve::rightLarge },
        { TrackCurve::leftLarge, TrackCurve::leftLarge },
        { TrackCurve::rightLarge, TrackCurve::rightLarge },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::leftLarge, TrackCurve::none },
        { TrackCurve::rightLarge, TrackCurve::none },
        { TrackCurve::none, TrackCurve::leftLarge },
        { TrackCurve::none, TrackCurve::rightLarge },
        { TrackCurve::none, TrackCurve::leftLarge },
        { TrackCurve::none, TrackCurve::rightLarge },
        { TrackCurve::leftLarge, TrackCurve::none },
        { TrackCurve::rightLarge, TrackCurve::none },
        { TrackCurve::none, TrackCurve::leftLarge },
        { TrackCurve::none, TrackCurve::rightLarge },
        { TrackCurve::leftLarge, TrackCurve::none },
        { TrackCurve::rightLarge, TrackCurve::none },
        { TrackCurve::leftLarge, TrackCurve::leftLarge },
        { TrackCurve::rightLarge, TrackCurve::rightLarge },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackElemType::leftBarrelRollDownToUp, TrackCurve::none },
        { TrackElemType::rightBarrelRollDownToUp, TrackCurve::none },
        { TrackCurve::none, TrackElemType::leftBarrelRollUpToDown },
        { TrackCurve::none, TrackElemType::rightBarrelRollUpToDown },
        { TrackCurve::leftSmall, TrackCurve::leftSmall },
        { TrackCurve::rightSmall, TrackCurve::rightSmall },
        { TrackCurve::leftSmall, TrackCurve::leftSmall },
        { TrackCurve::rightSmall, TrackCurve::rightSmall },
        { TrackElemType::poweredLift, TrackElemType::poweredLift },
        { TrackElemType::rightLargeHalfLoopDown, TrackCurve::none },
        { TrackElemType::leftLargeHalfLoopDown, TrackCurve::none },
        { TrackCurve::none, TrackElemType::rightLargeHalfLoopUp },
        { TrackCurve::none, TrackElemType::leftLargeHalfLoopUp },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackElemType::invertedFlatToDown90QuarterLoop, TrackCurve::none },
        { TrackCurve::none, TrackElemType::up90ToInvertedFlatQuarterLoop },
        { TrackElemType::leftCurvedLiftHill, TrackElemType::leftCurvedLiftHill },
        { TrackElemType::rightCurvedLiftHill, TrackElemType::rightCurvedLiftHill },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackElemType::airThrustVerticalDown, TrackElemType::reverseFreefallVertical },
        { TrackElemType::airThrustVerticalDown, TrackElemType::airThrustVerticalDown },
        { TrackCurve::none, TrackElemType::airThrustVerticalDown },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::leftSmall, TrackCurve::leftSmall },
        { TrackCurve::rightSmall, TrackCurve::rightSmall },
        { TrackCurve::leftSmall, TrackCurve::leftSmall },
        { TrackCurve::rightSmall, TrackCurve::rightSmall },
        { TrackCurve::left, TrackCurve::left },
        { TrackCurve::right, TrackCurve::right },
        { TrackCurve::left, TrackCurve::left },
        { TrackCurve::right, TrackCurve::right },
        { TrackCurve::left, TrackCurve::none },
        { TrackCurve::right, TrackCurve::none },
        { TrackCurve::none, TrackCurve::left },
        { TrackCurve::none, TrackCurve::right },
        { TrackCurve::left, TrackCurve::none },
        { TrackCurve::right, TrackCurve::none },
        { TrackCurve::none, TrackCurve::left },
        { TrackCurve::none, TrackCurve::right },
        { TrackCurve::left, TrackCurve::left },
        { TrackCurve::right, TrackCurve::right },
        { TrackCurve::left, TrackCurve::left },
        { TrackCurve::right, TrackCurve::right },
        { TrackCurve::left, TrackCurve::left },
        { TrackCurve::right, TrackCurve::right },
        { TrackCurve::left, TrackCurve::left },
        { TrackCurve::right, TrackCurve::right },
        { TrackCurve::left, TrackCurve::none },
        { TrackCurve::right, TrackCurve::none },
        { TrackCurve::none, TrackCurve::left },
        { TrackCurve::none, TrackCurve::right },
        { TrackCurve::left, TrackCurve::none },
        { TrackCurve::right, TrackCurve::none },
        { TrackCurve::none, TrackCurve::left },
        { TrackCurve::none, TrackCurve::right },
        { TrackCurve::leftSmall, TrackCurve::leftSmall },
        { TrackCurve::rightSmall, TrackCurve::rightSmall },
        { TrackCurve::leftSmall, TrackCurve::leftSmall },
        { TrackCurve::rightSmall, TrackCurve::rightSmall },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackCurve::none, TrackCurve::none },
        { TrackElemType::none, TrackElemType::none },                       // TrackElemType::flatTrack1x4A
        { TrackCurve::none, TrackCurve::none }, // TrackElemType::flatTrack2x2
        { TrackCurve::none, TrackCurve::none }, // TrackElemType::flatTrack4x4
        { TrackElemType::none, TrackElemType::none },                       // TrackElemType::flatTrack2x4
        { TrackElemType::none, TrackElemType::none },                       // TrackElemType::flatTrack1x5
        { TrackElemType::none, TrackElemType::none },                       // TrackElemType::flatTrack1x1A
        { TrackElemType::none, TrackElemType::none },                       // TrackElemType::flatTrack1x4B
        { TrackElemType::none, TrackElemType::none },                       // TrackElemType::flatTrack1x1B
        { TrackElemType::none, TrackElemType::none },                       // TrackElemType::flatTrack1x4C
        { TrackElemType::none, TrackElemType::none },                       // TrackElemType::flatTrack3x3
        { TrackElemType::rightLargeCorkscrewDown, TrackCurve::none }, // TrackElemType::leftLargeCorkscrewUp
        { TrackElemType::leftLargeCorkscrewDown, TrackCurve::none },  // TrackElemType::rightLargeCorkscrewUp
        { TrackCurve::none, TrackElemType::rightLargeCorkscrewUp },   // TrackElemType::leftLargeCorkscrewDown
        { TrackCurve::none, TrackElemType::leftLargeCorkscrewUp },    // TrackElemType::rightCorkscrewDown
        { TrackElemType::rightMediumHalfLoopDown, TrackCurve::none }, // TrackElemType::leftMediumHalfLoopUp
        { TrackElemType::leftMediumHalfLoopDown, TrackCurve::none },  // TrackElemType::rightMediumHalfLoopUp
        { TrackCurve::none, TrackElemType::rightMediumHalfLoopUp },   // TrackElemType::leftMediumHalfLoopDown
        { TrackCurve::none, TrackElemType::leftMediumHalfLoopUp },    // TrackElemType::rightMediumHalfLoopDown
        { TrackElemType::leftZeroGRollDown, TrackCurve::none },       // TrackElemType::leftZeroGRollUp
        { TrackElemType::rightZeroGRollDown, TrackCurve::none },      // TrackElemType::rightZeroGRollUp
        { TrackCurve::none, TrackElemType::leftZeroGRollUp },         // TrackElemType::leftZeroGRollDown
        { TrackCurve::none, TrackElemType::rightZeroGRollUp },        // TrackElemType::rightZeroGRollDown
        { TrackElemType::leftLargeZeroGRollDown, TrackCurve::none },  // TrackElemType::leftLargeZeroGRollUp
        { TrackElemType::rightLargeZeroGRollDown, TrackCurve::none }, // TrackElemType::rightLargeZeroGRollUp
        { TrackCurve::none, TrackElemType::leftLargeZeroGRollUp },    // TrackElemType::leftLargeZeroGRollDown
        { TrackCurve::none, TrackElemType::rightLargeZeroGRollUp },   // TrackElemType::rightLargeZeroGRollDown
        { TrackElemType::rightFlyerLargeHalfLoopInvertedDown, TrackCurve::none },   // LeftFlyerLargeHalfLoopUninvertedUp
        { TrackElemType::leftFlyerLargeHalfLoopInvertedDown, TrackCurve::none },    // RightFlyerLargeHalfLoopUninvertedUp
        { TrackCurve::none, TrackElemType::rightFlyerLargeHalfLoopUninvertedUp },   // LeftFlyerLargeHalfLoopInvertedDown
        { TrackCurve::none, TrackElemType::leftFlyerLargeHalfLoopUninvertedUp },    // RightFlyerLargeHalfLoopInvertedDown
        { TrackElemType::rightFlyerLargeHalfLoopUninvertedDown, TrackCurve::none },  // LeftFlyerLargeHalfLoopInvertedUp
        { TrackElemType::leftFlyerLargeHalfLoopUninvertedDown, TrackCurve::none },  // RightFlyerLargeHalfLoopInvertedUp
        { TrackCurve::none, TrackElemType::rightFlyerLargeHalfLoopInvertedUp },     // LeftFlyerLargeHalfLoopUninvertedDown
        { TrackCurve::none, TrackElemType::leftFlyerLargeHalfLoopInvertedUp },      // RightFlyerLargeHalfLoopUninvertedDown
        { TrackCurve::none, TrackCurve::none }, // FlyerHalfLoopInvertedUp
        { TrackCurve::none, TrackCurve::none }, // FlyerHalfLoopUninvertedUp
        { TrackCurve::leftLarge, TrackCurve::leftLarge },   // TrackElemType::leftEighthToDiagUp25
        { TrackCurve::rightLarge, TrackCurve::rightLarge }, // TrackElemType::rightEighthToDiagUp25
        { TrackCurve::leftLarge, TrackCurve::leftLarge },   // TrackElemType::leftEighthToDiagDown25
        { TrackCurve::rightLarge, TrackCurve::rightLarge }, // TrackElemType::rightEighthToDiagDown25
        { TrackCurve::leftLarge, TrackCurve::leftLarge },   // TrackElemType::leftEighthToOrthogonalUp25
        { TrackCurve::rightLarge, TrackCurve::rightLarge }, // TrackElemType::rightEighthToOrthogonalUp25
        { TrackCurve::leftLarge, TrackCurve::leftLarge },   // TrackElemType::leftEighthToOrthogonalDown25
        { TrackCurve::rightLarge, TrackCurve::rightLarge }, // TrackElemType::rightEighthToOrthogonalDown25
        { TrackCurve::leftLarge, TrackCurve::leftLarge },   // TrackElemType::diagUp25ToLeftBankedUp25
        { TrackCurve::rightLarge, TrackCurve::rightLarge }, // TrackElemType::diagUp25ToRightBankedUp25
        { TrackCurve::none, TrackCurve::none },               // TrackElemType::diagLeftBankedUp25ToUp25
        { TrackCurve::none, TrackCurve::none },               // TrackElemType::diagRightBankedUp25ToUp25
        { TrackCurve::leftLarge, TrackCurve::leftLarge },   // TrackElemType::diagDown25ToLeftBankedDown25
        { TrackCurve::rightLarge, TrackCurve::rightLarge }, // TrackElemType::diagDown25ToRightBankedDown25
        { TrackCurve::none, TrackCurve::none },               // TrackElemType::diagLeftBankedDown25ToDown25
        { TrackCurve::none, TrackCurve::none },               // TrackElemType::diagRightBankedDown25ToDown25
        { TrackCurve::leftLarge, TrackCurve::leftLarge },   // TrackElemType::diagLeftBankedFlatToLeftBankedUp25
        { TrackCurve::rightLarge, TrackCurve::rightLarge }, // TrackElemType::diagRightBankedFlatToRightBankedUp25
        { TrackCurve::leftLarge, TrackCurve::leftLarge },   // TrackElemType::diagLeftBankedUp25ToLeftBankedFlat
        { TrackCurve::rightLarge, TrackCurve::rightLarge }, // TrackElemType::diagRightBankedUp25ToRightBankedFlat
        { TrackCurve::leftLarge, TrackCurve::leftLarge },   // TrackElemType::diagLeftBankedFlatToLeftBankedDown25
        { TrackCurve::rightLarge, TrackCurve::rightLarge }, // TrackElemType::diagRightBankedFlatToRightBankedDown25
        { TrackCurve::leftLarge, TrackCurve::leftLarge },   // TrackElemType::diagLeftBankedDown25ToLeftBankedFlat
        { TrackCurve::rightLarge, TrackCurve::rightLarge }, // TrackElemType::diagRightBankedDown25ToRightBankedFlat
        { TrackCurve::leftLarge, TrackCurve::leftLarge },   // TrackElemType::diagFlatToLeftBankedUp25
        { TrackCurve::rightLarge, TrackCurve::rightLarge }, // TrackElemType::diagFlatToRightBankedUp25
        { TrackCurve::none, TrackCurve::none },               // TrackElemType::diagLeftBankedUp25ToFlat
        { TrackCurve::none, TrackCurve::none },               // TrackElemType::diagRightBankedUp25ToFlat
        { TrackCurve::leftLarge, TrackCurve::leftLarge },   // TrackElemType::diagFlatToLeftBankedDown25
        { TrackCurve::rightLarge, TrackCurve::rightLarge }, // TrackElemType::diagFlatToRightBankedDown25
        { TrackCurve::none, TrackCurve::none },               // TrackElemType::diagLeftBankedDown25ToFlat
        { TrackCurve::none, TrackCurve::none },               // TrackElemType::diagRightBankedDown25ToFlat
        { TrackCurve::leftLarge, TrackCurve::leftLarge },   // TrackElemType::diagUp25LeftBanked
        { TrackCurve::rightLarge, TrackCurve::rightLarge }, // TrackElemType::diagUp25RightBanked
        { TrackCurve::leftLarge, TrackCurve::leftLarge },   // TrackElemType::diagDown25LeftBanked
        { TrackCurve::rightLarge, TrackCurve::rightLarge }, // TrackElemType::diagDown25RightBanked
        { TrackCurve::leftLarge, TrackCurve::leftLarge },   // TrackElemType::leftEighthBankToDiagUp25
        { TrackCurve::rightLarge, TrackCurve::rightLarge }, // TrackElemType::rightEighthBankToDiagUp25
        { TrackCurve::leftLarge, TrackCurve::leftLarge },   // TrackElemType::leftEighthBankToDiagDown25
        { TrackCurve::rightLarge, TrackCurve::rightLarge }, // TrackElemType::rightEighthBankToDiagDown25
        { TrackCurve::leftLarge, TrackCurve::leftLarge },   // TrackElemType::leftEighthBankToOrthogonalUp25
        { TrackCurve::rightLarge, TrackCurve::rightLarge }, // TrackElemType::rightEighthBankToOrthogonalUp25
        { TrackCurve::leftLarge, TrackCurve::leftLarge },   // TrackElemType::leftEighthBankToOrthogonalDown25
        { TrackCurve::rightLarge, TrackCurve::rightLarge }, // TrackElemType::rightEighthBankToOrthogonalDown25
        { TrackElemType::diagBrakes, TrackElemType::diagBrakes }, // TrackElemType::diagBrakes
        { TrackCurve::none, TrackCurve::none },             // TrackElemType::diagBlockBrakes
        { TrackElemType::down25Brakes, TrackElemType::down25Brakes }, // TrackElemType::down25Brakes
        { TrackElemType::diagBooster, TrackElemType::diagBooster }, // TrackElemType::diagBooster
        { TrackCurve::none, TrackCurve::none },             // TrackElemType::diagFlatToUp60LongBase,
        { TrackCurve::none, TrackCurve::none },             // TrackElemType::diagUp60ToFlatLongBase,
        { TrackCurve::none, TrackCurve::none },             // TrackElemType::diagFlatToDown60LongBase,
        { TrackCurve::none, TrackCurve::none },             // TrackElemType::diagDown60ToFlatLongBase,
        { TrackElemType::rightEighthDiveLoopDownToDiag , TrackCurve::none },          // TrackElemType::leftEighthDiveLoopUpToOrthogonal
        { TrackElemType::leftEighthDiveLoopDownToDiag, TrackCurve::none },            // TrackElemType::rightEighthDiveLoopUpToOrthogonal
        { TrackCurve::none, TrackElemType::rightEighthDiveLoopUpToOrthogonal  },             // TrackElemType::leftEighthDiveLoopDownToDiag
        { TrackCurve::none, TrackElemType::leftEighthDiveLoopUpToOrthogonal },             // TrackElemType::rightEighthDiveLoopDownToDiag
        { TrackElemType::diagDown25Brakes, TrackElemType::diagDown25Brakes }, // TrackElemType::diagDown25Brakes
    };
    static_assert(std::size(kTrackCurveChain) == EnumValue(TrackElemType::count));

    const TrackDescriptor gTrackDescriptors[186] = {
        {   true,   TrackPitch::down60,     TrackRoll::none,    TrackCurve::none,             TrackPitch::down60,   TrackRoll::none,    TrackElemType::diagDown60                              },
        {   true,   TrackPitch::down60,     TrackRoll::none,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::none,    TrackElemType::diagDown60ToDown25                      },
        {   true,   TrackPitch::down60,     TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::diagDown60ToFlat                        },
        {   true,   TrackPitch::down25,     TrackRoll::none,    TrackCurve::none,             TrackPitch::down60,   TrackRoll::none,    TrackElemType::diagDown25ToDown60                      },
        {   true,   TrackPitch::down25,     TrackRoll::none,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::none,    TrackElemType::diagDown25                              },
        {   true,   TrackPitch::down25,     TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::left,    TrackElemType::diagDown25ToLeftBank                    },
        {   true,   TrackPitch::down25,     TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::diagDown25ToFlat                        },
        {   true,   TrackPitch::down25,     TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::right,   TrackElemType::diagDown25ToRightBank                   },
        {   true,   TrackPitch::none,       TrackRoll::left,    TrackCurve::leftLarge,        TrackPitch::none,     TrackRoll::left,    TrackElemType::leftEighthBankToOrthogonal              },
        {   true,   TrackPitch::none,       TrackRoll::left,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::none,    TrackElemType::diagLeftBankToDown25                    },
        {   true,   TrackPitch::none,       TrackRoll::left,    TrackCurve::none,             TrackPitch::none,     TrackRoll::left,    TrackElemType::diagLeftBank                            },
        {   true,   TrackPitch::none,       TrackRoll::left,    TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::diagLeftBankToFlat                      },
        {   true,   TrackPitch::none,       TrackRoll::left,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::none,    TrackElemType::diagLeftBankToUp25                      },
        {   true,   TrackPitch::none,       TrackRoll::none,    TrackCurve::leftLarge,        TrackPitch::none,     TrackRoll::none,    TrackElemType::leftEighthToOrthogonal                  },
        {   true,   TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::down60,   TrackRoll::none,    TrackElemType::diagFlatToDown60                        },
        {   true,   TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::none,    TrackElemType::diagFlatToDown25                        },
        {   true,   TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::left,    TrackElemType::diagFlatToLeftBank                      },
        {   true,   TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::diagFlat                                },
        {   true,   TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::right,   TrackElemType::diagFlatToRightBank                     },
        {   true,   TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::none,    TrackElemType::diagFlatToUp25                          },
        {   true,   TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up60,     TrackRoll::none,    TrackElemType::diagFlatToUp60                          },
        {   true,   TrackPitch::none,       TrackRoll::none,    TrackCurve::rightLarge,       TrackPitch::none,     TrackRoll::none,    TrackElemType::rightEighthToOrthogonal                 },
        {   true,   TrackPitch::none,       TrackRoll::right,   TrackCurve::none,             TrackPitch::down25,   TrackRoll::none,    TrackElemType::diagRightBankToDown25                   },
        {   true,   TrackPitch::none,       TrackRoll::right,   TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::diagRightBankToFlat                     },
        {   true,   TrackPitch::none,       TrackRoll::right,   TrackCurve::none,             TrackPitch::none,     TrackRoll::right,   TrackElemType::diagRightBank                           },
        {   true,   TrackPitch::none,       TrackRoll::right,   TrackCurve::none,             TrackPitch::up25,     TrackRoll::none,    TrackElemType::diagRightBankToUp25                     },
        {   true,   TrackPitch::none,       TrackRoll::right,   TrackCurve::rightLarge,       TrackPitch::none,     TrackRoll::right,   TrackElemType::rightEighthBankToOrthogonal             },
        {   true,   TrackPitch::up25,       TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::left,    TrackElemType::diagUp25ToLeftBank                      },
        {   true,   TrackPitch::up25,       TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::diagUp25ToFlat                          },
        {   true,   TrackPitch::up25,       TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::right,   TrackElemType::diagUp25ToRightBank                     },
        {   true,   TrackPitch::up25,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::none,    TrackElemType::diagUp25                                },
        {   true,   TrackPitch::up25,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up60,     TrackRoll::none,    TrackElemType::diagUp25ToUp60                          },
        {   true,   TrackPitch::up60,       TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::diagUp60ToFlat                          },
        {   true,   TrackPitch::up60,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::none,    TrackElemType::diagUp60ToUp25                          },
        {   true,   TrackPitch::up60,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up60,     TrackRoll::none,    TrackElemType::diagUp60                                },
        {   false,  TrackPitch::down90,     TrackRoll::none,    TrackCurve::leftSmall,        TrackPitch::down90,   TrackRoll::none,    TrackElemType::leftQuarterTurn1TileDown90              },
        {   false,  TrackPitch::down90,     TrackRoll::none,    TrackCurve::none,             TrackPitch::down90,   TrackRoll::none,    TrackElemType::down90                                  },
        {   false,  TrackPitch::down90,     TrackRoll::none,    TrackCurve::none,             TrackPitch::down60,   TrackRoll::none,    TrackElemType::down90ToDown60                          },
        {   false,  TrackPitch::down90,     TrackRoll::none,    TrackCurve::rightSmall,       TrackPitch::down90,   TrackRoll::none,    TrackElemType::rightQuarterTurn1TileDown90             },
        {   false,  TrackPitch::down60,     TrackRoll::none,    TrackCurve::leftSmall,        TrackPitch::down60,   TrackRoll::none,    TrackElemType::leftQuarterTurn1TileDown60              },
        {   false,  TrackPitch::down60,     TrackRoll::none,    TrackCurve::none,             TrackPitch::down90,   TrackRoll::none,    TrackElemType::down60ToDown90                          },
        {   false,  TrackPitch::down60,     TrackRoll::none,    TrackCurve::none,             TrackPitch::down60,   TrackRoll::none,    TrackElemType::down60                                  },
        {   false,  TrackPitch::down60,     TrackRoll::none,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::none,    TrackElemType::down60ToDown25                          },
        {   false,  TrackPitch::down60,     TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::down60ToFlat                            },
        {   false,  TrackPitch::down60,     TrackRoll::none,    TrackCurve::rightSmall,       TrackPitch::down60,   TrackRoll::none,    TrackElemType::rightQuarterTurn1TileDown60             },
        {   false,  TrackPitch::down25,     TrackRoll::left,    TrackCurve::leftSmall,        TrackPitch::down25,   TrackRoll::left,    TrackElemType::leftBankedQuarterTurn3TileDown25        },
        {   false,  TrackPitch::down25,     TrackRoll::left,    TrackCurve::left,             TrackPitch::down25,   TrackRoll::left,    TrackElemType::leftBankedQuarterTurn5TileDown25        },
        {   false,  TrackPitch::down25,     TrackRoll::left,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::left,    TrackElemType::down25LeftBanked                        },
        {   false,  TrackPitch::down25,     TrackRoll::left,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::none,    TrackElemType::leftBankedDown25ToDown25                },
        {   false,  TrackPitch::down25,     TrackRoll::left,    TrackCurve::none,             TrackPitch::none,     TrackRoll::left,    TrackElemType::leftBankedDown25ToLeftBankedFlat        },
        {   false,  TrackPitch::down25,     TrackRoll::left,    TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::leftBankedDown25ToFlat                  },
        {   false,  TrackPitch::down25,     TrackRoll::none,    TrackCurve::leftSmall,        TrackPitch::down25,   TrackRoll::none,    TrackElemType::leftQuarterTurn3TilesDown25             },
        {   false,  TrackPitch::down25,     TrackRoll::none,    TrackCurve::leftSmall,        TrackPitch::none,     TrackRoll::left,    TrackElemType::leftQuarterTurn3TilesDown25ToLeftBank   },
        {   false,  TrackPitch::down25,     TrackRoll::none,    TrackCurve::left,             TrackPitch::down25,   TrackRoll::none,    TrackElemType::leftQuarterTurn5TilesDown25             },
        {   false,  TrackPitch::down25,     TrackRoll::none,    TrackCurve::none,             TrackPitch::down60,   TrackRoll::none,    TrackElemType::down25ToDown60                          },
        {   false,  TrackPitch::down25,     TrackRoll::none,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::left,    TrackElemType::down25ToLeftBankedDown25                },
        {   false,  TrackPitch::down25,     TrackRoll::none,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::none,    TrackElemType::down25                                  },
        {   false,  TrackPitch::down25,     TrackRoll::none,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::right,   TrackElemType::down25ToRightBankedDown25               },
        {   false,  TrackPitch::down25,     TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::left,    TrackElemType::down25ToLeftBank                        },
        {   false,  TrackPitch::down25,     TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::down25ToFlat                            },
        {   false,  TrackPitch::down25,     TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::right,   TrackElemType::down25ToRightBank                       },
        {   false,  TrackPitch::down25,     TrackRoll::none,    TrackCurve::right,            TrackPitch::down25,   TrackRoll::none,    TrackElemType::rightQuarterTurn5TilesDown25            },
        {   false,  TrackPitch::down25,     TrackRoll::none,    TrackCurve::rightSmall,       TrackPitch::none,     TrackRoll::right,   TrackElemType::rightQuarterTurn3TilesDown25ToRightBank },
        {   false,  TrackPitch::down25,     TrackRoll::none,    TrackCurve::rightSmall,       TrackPitch::down25,   TrackRoll::none,    TrackElemType::rightQuarterTurn3TilesDown25            },
        {   false,  TrackPitch::down25,     TrackRoll::right,   TrackCurve::none,             TrackPitch::down25,   TrackRoll::none,    TrackElemType::rightBankedDown25ToDown25               },
        {   false,  TrackPitch::down25,     TrackRoll::right,   TrackCurve::none,             TrackPitch::down25,   TrackRoll::right,   TrackElemType::down25RightBanked                       },
        {   false,  TrackPitch::down25,     TrackRoll::right,   TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::rightBankedDown25ToFlat                 },
        {   false,  TrackPitch::down25,     TrackRoll::right,   TrackCurve::none,             TrackPitch::none,     TrackRoll::right,   TrackElemType::rightBankedDown25ToRightBankedFlat      },
        {   false,  TrackPitch::down25,     TrackRoll::right,   TrackCurve::right,            TrackPitch::down25,   TrackRoll::right,   TrackElemType::rightBankedQuarterTurn5TileDown25       },
        {   false,  TrackPitch::down25,     TrackRoll::right,   TrackCurve::rightSmall,       TrackPitch::down25,   TrackRoll::right,   TrackElemType::rightBankedQuarterTurn3TileDown25       },
        {   false,  TrackPitch::none,       TrackRoll::left,    TrackCurve::leftSmall,        TrackPitch::none,     TrackRoll::left,    TrackElemType::leftBankedQuarterTurn3Tiles             },
        {   false,  TrackPitch::none,       TrackRoll::left,    TrackCurve::leftSmall,        TrackPitch::up25,     TrackRoll::none,    TrackElemType::leftBankToLeftQuarterTurn3TilesUp25     },
        {   false,  TrackPitch::none,       TrackRoll::left,    TrackCurve::left,             TrackPitch::none,     TrackRoll::left,    TrackElemType::bankedLeftQuarterTurn5Tiles             },
        {   false,  TrackPitch::none,       TrackRoll::left,    TrackCurve::leftLarge,        TrackPitch::none,     TrackRoll::left,    TrackElemType::leftEighthBankToDiag                    },
        {   false,  TrackPitch::none,       TrackRoll::left,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::left,    TrackElemType::leftBankedFlatToLeftBankedDown25        },
        {   false,  TrackPitch::none,       TrackRoll::left,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::none,    TrackElemType::leftBankToDown25                        },
        {   false,  TrackPitch::none,       TrackRoll::left,    TrackCurve::none,             TrackPitch::none,     TrackRoll::left,    TrackElemType::leftBank                                },
        {   false,  TrackPitch::none,       TrackRoll::left,    TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::leftBankToFlat                          },
        {   false,  TrackPitch::none,       TrackRoll::left,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::left,    TrackElemType::leftBankedFlatToLeftBankedUp25          },
        {   false,  TrackPitch::none,       TrackRoll::left,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::none,    TrackElemType::leftBankToUp25                          },
        {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::leftSmall,        TrackPitch::none,     TrackRoll::none,    TrackElemType::leftQuarterTurn3Tiles                   },
        {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::left,             TrackPitch::none,     TrackRoll::none,    TrackElemType::leftQuarterTurn5Tiles                   },
        {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::leftLarge,        TrackPitch::none,     TrackRoll::none,    TrackElemType::leftEighthToDiag                        },
        {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::down60,   TrackRoll::none,    TrackElemType::flatToDown60                            },
        {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::left,    TrackElemType::flatToLeftBankedDown25                  },
        {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::none,    TrackElemType::flatToDown25                            },
        {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::right,   TrackElemType::flatToRightBankedDown25                 },
        {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::left,    TrackElemType::flatToLeftBank                          },
        {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::flat                                    },
        {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::leftVerySmall,    TrackPitch::none,     TrackRoll::none,    TrackElemType::leftQuarterTurn1Tile                    },
        {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::rightVerySmall,   TrackPitch::none,     TrackRoll::none,    TrackElemType::rightQuarterTurn1Tile                   },
        {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::right,   TrackElemType::flatToRightBank                         },
        {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::left,    TrackElemType::flatToLeftBankedUp25                    },
        {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::none,    TrackElemType::flatToUp25                              },
        {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::right,   TrackElemType::flatToRightBankedUp25                   },
        {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up60,     TrackRoll::none,    TrackElemType::flatToUp60                              },
        {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::rightLarge,       TrackPitch::none,     TrackRoll::none,    TrackElemType::rightEighthToDiag                       },
        {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::right,            TrackPitch::none,     TrackRoll::none,    TrackElemType::rightQuarterTurn5Tiles                  },
        {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::rightSmall,       TrackPitch::none,     TrackRoll::none,    TrackElemType::rightQuarterTurn3Tiles                  },
        {   false,  TrackPitch::none,       TrackRoll::right,   TrackCurve::none,             TrackPitch::down25,   TrackRoll::none,    TrackElemType::rightBankToDown25                       },
        {   false,  TrackPitch::none,       TrackRoll::right,   TrackCurve::none,             TrackPitch::down25,   TrackRoll::right,   TrackElemType::rightBankedFlatToRightBankedDown25      },
        {   false,  TrackPitch::none,       TrackRoll::right,   TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::rightBankToFlat                         },
        {   false,  TrackPitch::none,       TrackRoll::right,   TrackCurve::none,             TrackPitch::none,     TrackRoll::right,   TrackElemType::rightBank                               },
        {   false,  TrackPitch::none,       TrackRoll::right,   TrackCurve::none,             TrackPitch::up25,     TrackRoll::none,    TrackElemType::rightBankToUp25                         },
        {   false,  TrackPitch::none,       TrackRoll::right,   TrackCurve::none,             TrackPitch::up25,     TrackRoll::right,   TrackElemType::rightBankedFlatToRightBankedUp25        },
        {   false,  TrackPitch::none,       TrackRoll::right,   TrackCurve::rightLarge,       TrackPitch::none,     TrackRoll::right,   TrackElemType::rightEighthBankToDiag                   },
        {   false,  TrackPitch::none,       TrackRoll::right,   TrackCurve::right,            TrackPitch::none,     TrackRoll::right,   TrackElemType::bankedRightQuarterTurn5Tiles            },
        {   false,  TrackPitch::none,       TrackRoll::right,   TrackCurve::rightSmall,       TrackPitch::none,     TrackRoll::right,   TrackElemType::rightBankedQuarterTurn3Tiles            },
        {   false,  TrackPitch::none,       TrackRoll::right,   TrackCurve::rightSmall,       TrackPitch::up25,     TrackRoll::none,    TrackElemType::rightBankToRightQuarterTurn3TilesUp25   },
        {   false,  TrackPitch::up25,       TrackRoll::left,    TrackCurve::leftSmall,        TrackPitch::up25,     TrackRoll::left,    TrackElemType::leftBankedQuarterTurn3TileUp25          },
        {   false,  TrackPitch::up25,       TrackRoll::left,    TrackCurve::left,             TrackPitch::up25,     TrackRoll::left,    TrackElemType::leftBankedQuarterTurn5TileUp25          },
        {   false,  TrackPitch::up25,       TrackRoll::left,    TrackCurve::none,             TrackPitch::none,     TrackRoll::left,    TrackElemType::leftBankedUp25ToLeftBankedFlat          },
        {   false,  TrackPitch::up25,       TrackRoll::left,    TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::leftBankedUp25ToFlat                    },
        {   false,  TrackPitch::up25,       TrackRoll::left,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::left,    TrackElemType::up25LeftBanked                          },
        {   false,  TrackPitch::up25,       TrackRoll::left,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::none,    TrackElemType::leftBankedUp25ToUp25                    },
        {   false,  TrackPitch::up25,       TrackRoll::none,    TrackCurve::leftSmall,        TrackPitch::up25,     TrackRoll::none,    TrackElemType::leftQuarterTurn3TilesUp25               },
        {   false,  TrackPitch::up25,       TrackRoll::none,    TrackCurve::left,             TrackPitch::up25,     TrackRoll::none,    TrackElemType::leftQuarterTurn5TilesUp25               },
        {   false,  TrackPitch::up25,       TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::left,    TrackElemType::up25ToLeftBank                          },
        {   false,  TrackPitch::up25,       TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::up25ToFlat                              },
        {   false,  TrackPitch::up25,       TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::right,   TrackElemType::up25ToRightBank                         },
        {   false,  TrackPitch::up25,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::left,    TrackElemType::up25ToLeftBankedUp25                    },
        {   false,  TrackPitch::up25,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::none,    TrackElemType::up25                                    },
        {   false,  TrackPitch::up25,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::right,   TrackElemType::up25ToRightBankedUp25                   },
        {   false,  TrackPitch::up25,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up60,     TrackRoll::none,    TrackElemType::up25ToUp60                              },
        {   false,  TrackPitch::up25,       TrackRoll::none,    TrackCurve::right,            TrackPitch::up25,     TrackRoll::none,    TrackElemType::rightQuarterTurn5TilesUp25              },
        {   false,  TrackPitch::up25,       TrackRoll::none,    TrackCurve::rightSmall,       TrackPitch::up25,     TrackRoll::none,    TrackElemType::rightQuarterTurn3TilesUp25              },
        {   false,  TrackPitch::up25,       TrackRoll::right,   TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::rightBankedUp25ToFlat                   },
        {   false,  TrackPitch::up25,       TrackRoll::right,   TrackCurve::none,             TrackPitch::none,     TrackRoll::right,   TrackElemType::rightBankedUp25ToRightBankedFlat        },
        {   false,  TrackPitch::up25,       TrackRoll::right,   TrackCurve::none,             TrackPitch::up25,     TrackRoll::none,    TrackElemType::rightBankedUp25ToUp25                   },
        {   false,  TrackPitch::up25,       TrackRoll::right,   TrackCurve::none,             TrackPitch::up25,     TrackRoll::right,   TrackElemType::up25RightBanked                         },
        {   false,  TrackPitch::up25,       TrackRoll::right,   TrackCurve::right,            TrackPitch::up25,     TrackRoll::right,   TrackElemType::rightBankedQuarterTurn5TileUp25         },
        {   false,  TrackPitch::up25,       TrackRoll::right,   TrackCurve::rightSmall,       TrackPitch::up25,     TrackRoll::right,   TrackElemType::rightBankedQuarterTurn3TileUp25         },
        {   false,  TrackPitch::up60,       TrackRoll::none,    TrackCurve::leftSmall,        TrackPitch::up60,     TrackRoll::none,    TrackElemType::leftQuarterTurn1TileUp60                },
        {   false,  TrackPitch::up60,       TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::up60ToFlat                              },
        {   false,  TrackPitch::up60,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::none,    TrackElemType::up60ToUp25                              },
        {   false,  TrackPitch::up60,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up60,     TrackRoll::none,    TrackElemType::up60                                    },
        {   false,  TrackPitch::up60,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up90,     TrackRoll::none,    TrackElemType::up60ToUp90                              },
        {   false,  TrackPitch::up60,       TrackRoll::none,    TrackCurve::rightSmall,       TrackPitch::up60,     TrackRoll::none,    TrackElemType::rightQuarterTurn1TileUp60               },
        {   false,  TrackPitch::up90,       TrackRoll::none,    TrackCurve::leftSmall,        TrackPitch::up90,     TrackRoll::none,    TrackElemType::leftQuarterTurn1TileUp90                },
        {   false,  TrackPitch::up90,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up60,     TrackRoll::none,    TrackElemType::up90ToUp60                              },
        {   false,  TrackPitch::up90,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up90,     TrackRoll::none,    TrackElemType::up90                                    },
        {   false,  TrackPitch::up90,       TrackRoll::none,    TrackCurve::rightSmall,       TrackPitch::up90,     TrackRoll::none,    TrackElemType::rightQuarterTurn1TileUp90               },
        {   false,  TrackPitch::up25,       TrackRoll::none,    TrackCurve::leftLarge,        TrackPitch::up25,     TrackRoll::none,    TrackElemType::leftEighthToDiagUp25                    },
        {   false,  TrackPitch::up25,       TrackRoll::none,    TrackCurve::rightLarge,       TrackPitch::up25,     TrackRoll::none,    TrackElemType::rightEighthToDiagUp25                   },
        {   false,  TrackPitch::down25,     TrackRoll::none,    TrackCurve::leftLarge,        TrackPitch::down25,   TrackRoll::none,    TrackElemType::leftEighthToDiagDown25                  },
        {   false,  TrackPitch::down25,     TrackRoll::none,    TrackCurve::rightLarge,       TrackPitch::down25,   TrackRoll::none,    TrackElemType::rightEighthToDiagDown25                 },
        {   true,   TrackPitch::up25,       TrackRoll::none,    TrackCurve::leftLarge,        TrackPitch::up25,     TrackRoll::none,    TrackElemType::leftEighthToOrthogonalUp25              },
        {   true,   TrackPitch::up25,       TrackRoll::none,    TrackCurve::rightLarge,       TrackPitch::up25,     TrackRoll::none,    TrackElemType::rightEighthToOrthogonalUp25             },
        {   true,   TrackPitch::down25,     TrackRoll::none,    TrackCurve::leftLarge,        TrackPitch::down25,   TrackRoll::none,    TrackElemType::leftEighthToOrthogonalDown25            },
        {   true,   TrackPitch::down25,     TrackRoll::none,    TrackCurve::rightLarge,       TrackPitch::down25,   TrackRoll::none,    TrackElemType::rightEighthToOrthogonalDown25           },
        {   true,   TrackPitch::up25,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::left,    TrackElemType::diagUp25ToLeftBankedUp25                },
        {   true,   TrackPitch::up25,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::right,   TrackElemType::diagUp25ToRightBankedUp25               },
        {   true,   TrackPitch::up25,       TrackRoll::left,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::none,    TrackElemType::diagLeftBankedUp25ToUp25                },
        {   true,   TrackPitch::up25,       TrackRoll::right,   TrackCurve::none,             TrackPitch::up25,     TrackRoll::none,    TrackElemType::diagRightBankedUp25ToUp25               },
        {   true,   TrackPitch::down25,     TrackRoll::none,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::left,    TrackElemType::diagDown25ToLeftBankedDown25            },
        {   true,   TrackPitch::down25,     TrackRoll::none,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::right,   TrackElemType::diagDown25ToRightBankedDown25           },
        {   true,   TrackPitch::down25,     TrackRoll::left,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::none,    TrackElemType::diagLeftBankedDown25ToDown25            },
        {   true,   TrackPitch::down25,     TrackRoll::right,   TrackCurve::none,             TrackPitch::down25,   TrackRoll::none,    TrackElemType::diagRightBankedDown25ToDown25           },
        {   true,   TrackPitch::none,       TrackRoll::left,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::left,    TrackElemType::diagLeftBankedFlatToLeftBankedUp25      },
        {   true,   TrackPitch::none,       TrackRoll::right,   TrackCurve::none,             TrackPitch::up25,     TrackRoll::right,   TrackElemType::diagRightBankedFlatToRightBankedUp25    },
        {   true,   TrackPitch::up25,       TrackRoll::left,    TrackCurve::none,             TrackPitch::none,     TrackRoll::left,    TrackElemType::diagLeftBankedUp25ToLeftBankedFlat      },
        {   true,   TrackPitch::up25,       TrackRoll::right,   TrackCurve::none,             TrackPitch::none,     TrackRoll::right,   TrackElemType::diagRightBankedUp25ToRightBankedFlat    },
        {   true,   TrackPitch::none,       TrackRoll::left,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::left,    TrackElemType::diagLeftBankedFlatToLeftBankedDown25    },
        {   true,   TrackPitch::none,       TrackRoll::right,   TrackCurve::none,             TrackPitch::down25,   TrackRoll::right,   TrackElemType::diagRightBankedFlatToRightBankedDown25  },
        {   true,   TrackPitch::down25,     TrackRoll::left,    TrackCurve::none,             TrackPitch::none,     TrackRoll::left,    TrackElemType::diagLeftBankedDown25ToLeftBankedFlat    },
        {   true,   TrackPitch::down25,     TrackRoll::right,   TrackCurve::none,             TrackPitch::none,     TrackRoll::right,   TrackElemType::diagRightBankedDown25ToRightBankedFlat  },
        {   true,   TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::left,    TrackElemType::diagFlatToLeftBankedUp25                },
        {   true,   TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::right,   TrackElemType::diagFlatToRightBankedUp25               },
        {   true,   TrackPitch::up25,       TrackRoll::left,    TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::diagLeftBankedUp25ToFlat                },
        {   true,   TrackPitch::up25,       TrackRoll::right,   TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::diagRightBankedUp25ToFlat               },
        {   true,   TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::left,    TrackElemType::diagFlatToLeftBankedDown25              },
        {   true,   TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::right,   TrackElemType::diagFlatToRightBankedDown25             },
        {   true,   TrackPitch::down25,     TrackRoll::left,    TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::diagLeftBankedDown25ToFlat              },
        {   true,   TrackPitch::down25,     TrackRoll::right,   TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::diagRightBankedDown25ToFlat             },
        {   true,   TrackPitch::up25,       TrackRoll::left,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::left,    TrackElemType::diagUp25LeftBanked                      },
        {   true,   TrackPitch::up25,       TrackRoll::right,   TrackCurve::none,             TrackPitch::up25,     TrackRoll::right,   TrackElemType::diagUp25RightBanked                     },
        {   true,   TrackPitch::down25,     TrackRoll::left,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::left,    TrackElemType::diagDown25LeftBanked                    },
        {   true,   TrackPitch::down25,     TrackRoll::right,   TrackCurve::none,             TrackPitch::down25,   TrackRoll::right,   TrackElemType::diagDown25RightBanked                   },
        {   false,  TrackPitch::up25,       TrackRoll::left,    TrackCurve::leftLarge,        TrackPitch::up25,     TrackRoll::left,    TrackElemType::leftEighthBankToDiagUp25                },
        {   false,  TrackPitch::up25,       TrackRoll::right,   TrackCurve::rightLarge,       TrackPitch::up25,     TrackRoll::right,   TrackElemType::rightEighthBankToDiagUp25               },
        {   false,  TrackPitch::down25,     TrackRoll::left,    TrackCurve::leftLarge,        TrackPitch::down25,   TrackRoll::left,    TrackElemType::leftEighthBankToDiagDown25              },
        {   false,  TrackPitch::down25,     TrackRoll::right,   TrackCurve::rightLarge,       TrackPitch::down25,   TrackRoll::right,   TrackElemType::rightEighthBankToDiagDown25             },
        {   true,   TrackPitch::up25,       TrackRoll::left,    TrackCurve::leftLarge,        TrackPitch::up25,     TrackRoll::left,    TrackElemType::leftEighthBankToOrthogonalUp25          },
        {   true,   TrackPitch::up25,       TrackRoll::right,   TrackCurve::rightLarge,       TrackPitch::up25,     TrackRoll::right,   TrackElemType::rightEighthBankToOrthogonalUp25         },
        {   true,   TrackPitch::down25,     TrackRoll::left,    TrackCurve::leftLarge,        TrackPitch::down25,   TrackRoll::left,    TrackElemType::leftEighthBankToOrthogonalDown25        },
        {   true,   TrackPitch::down25,     TrackRoll::right,   TrackCurve::rightLarge,       TrackPitch::down25,   TrackRoll::right,   TrackElemType::rightEighthBankToOrthogonalDown25       },
    };

    /** rct2: 0x00993D1C */
    static constexpr TrackElemType kAlternativeTrackTypes[] = {
        TrackElemType::flatCovered,                        // TrackElemType::flat
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::up25Covered,                   // TrackElemType::up25
        TrackElemType::up60Covered,                   // TrackElemType::up60
        TrackElemType::flatToUp25Covered,           // TrackElemType::flatToUp25
        TrackElemType::up25ToUp60Covered,      // TrackElemType::up25ToUp60
        TrackElemType::up60ToUp25Covered,      // TrackElemType::up60ToUp25
        TrackElemType::up25ToFlatCovered,           // TrackElemType::up25ToFlat
        TrackElemType::down25Covered,                 // TrackElemType::down25
        TrackElemType::down60Covered,                 // TrackElemType::down60
        TrackElemType::flatToDown25Covered,         // TrackElemType::flatToDown25
        TrackElemType::down25ToDown60Covered,  // TrackElemType::down25ToDown60
        TrackElemType::down60ToDown25Covered,  // TrackElemType::down60ToDown25
        TrackElemType::down25ToFlatCovered,         // TrackElemType::down25ToFlat
        TrackElemType::leftQuarterTurn5TilesCovered,   // TrackElemType::leftQuarterTurn5Tiles
        TrackElemType::rightQuarterTurn5TilesCovered,  // TrackElemType::rightQuarterTurn5Tiles
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::sBendLeftCovered,                 // TrackElemType::sBendLeft
        TrackElemType::sBendRightCovered,                // TrackElemType::sBendRight
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::leftQuarterTurn3TilesCovered,   // TrackElemType::leftQuarterTurn3Tiles
        TrackElemType::rightQuarterTurn3TilesCovered,  // TrackElemType::rightQuarterTurn3Tiles
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none,
        TrackElemType::none, // TrackElemType::flatTrack1x4A
        TrackElemType::none, // TrackElemType::flatTrack2x2
        TrackElemType::none, // TrackElemType::flatTrack4x4
        TrackElemType::none, // TrackElemType::flatTrack2x4
        TrackElemType::none, // TrackElemType::flatTrack1x5
        TrackElemType::none, // TrackElemType::flatTrack1x1A
        TrackElemType::none, // TrackElemType::flatTrack1x4B
        TrackElemType::none, // TrackElemType::flatTrack1x1B
        TrackElemType::none, // TrackElemType::flatTrack1x4C
        TrackElemType::none, // TrackElemType::flatTrack3x3
        TrackElemType::none, // TrackElemType::leftLargeCorkscrewUp
        TrackElemType::none, // TrackElemType::rightLargeCorkscrewUp
        TrackElemType::none, // TrackElemType::leftLargeCorkscrewDown
        TrackElemType::none, // TrackElemType::rightCorkscrewDown
        TrackElemType::none, // TrackElemType::leftMediumHalfLoopUp
        TrackElemType::none, // TrackElemType::rightMediumHalfLoopUp
        TrackElemType::none, // TrackElemType::leftMediumHalfLoopDown
        TrackElemType::none, // TrackElemType::rightMediumHalfLoopDown
        TrackElemType::none, // TrackElemType::leftZeroGRollUp
        TrackElemType::none, // TrackElemType::rightZeroGRollUp
        TrackElemType::none, // TrackElemType::leftZeroGRollDown
        TrackElemType::none, // TrackElemType::rightZeroGRollDown
        TrackElemType::none, // TrackElemType::leftLargeZeroGRollUp
        TrackElemType::none, // TrackElemType::rightLargeZeroGRollUp
        TrackElemType::none, // TrackElemType::leftLargeZeroGRollDown
        TrackElemType::none, // TrackElemType::rightLargeZeroGRollDown
        TrackElemType::none, // LeftFlyerLargeHalfLoopUninvertedUp
        TrackElemType::none, // RightFlyerLargeHalfLoopUninvertedUp
        TrackElemType::none, // LeftFlyerLargeHalfLoopInvertedDown
        TrackElemType::none, // RightFlyerLargeHalfLoopInvertedDown
        TrackElemType::none, // FlyerHalfLoopInvertedUp
        TrackElemType::none, // FlyerHalfLoopUninvertedDown
        TrackElemType::none, // TrackElemType::leftEighthToDiagUp25
        TrackElemType::none, // TrackElemType::rightEighthToDiagUp25
        TrackElemType::none, // TrackElemType::leftEighthToDiagDown25
        TrackElemType::none, // TrackElemType::rightEighthToDiagDown25
        TrackElemType::none, // TrackElemType::leftEighthToOrthogonalUp25
        TrackElemType::none, // TrackElemType::rightEighthToOrthogonalUp25
        TrackElemType::none, // TrackElemType::leftEighthToOrthogonalDown25
        TrackElemType::none, // TrackElemType::rightEighthToOrthogonalDown25
        TrackElemType::none, // TrackElemType::diagUp25ToLeftBankedUp25
        TrackElemType::none, // TrackElemType::diagUp25ToRightBankedUp25
        TrackElemType::none, // TrackElemType::diagLeftBankedUp25ToUp25
        TrackElemType::none, // TrackElemType::diagRightBankedUp25ToUp25
        TrackElemType::none, // TrackElemType::diagDown25ToLeftBankedDown25
        TrackElemType::none, // TrackElemType::diagDown25ToRightBankedDown25
        TrackElemType::none, // TrackElemType::diagLeftBankedDown25ToDown25
        TrackElemType::none, // TrackElemType::diagRightBankedDown25ToDown25
        TrackElemType::none, // TrackElemType::diagLeftBankedFlatToLeftBankedUp25
        TrackElemType::none, // TrackElemType::diagRightBankedFlatToRightBankedUp25
        TrackElemType::none, // TrackElemType::diagLeftBankedUp25ToLeftBankedFlat
        TrackElemType::none, // TrackElemType::diagRightBankedUp25ToRightBankedFlat
        TrackElemType::none, // TrackElemType::diagLeftBankedFlatToLeftBankedDown25
        TrackElemType::none, // TrackElemType::diagRightBankedFlatToRightBankedDown25
        TrackElemType::none, // TrackElemType::diagLeftBankedDown25ToLeftBankedFlat
        TrackElemType::none, // TrackElemType::diagRightBankedDown25ToRightBankedFlat
        TrackElemType::none, // TrackElemType::diagFlatToLeftBankedUp25
        TrackElemType::none, // TrackElemType::diagFlatToRightBankedUp25
        TrackElemType::none, // TrackElemType::diagLeftBankedUp25ToFlat
        TrackElemType::none, // TrackElemType::diagRightBankedUp25ToFlat
        TrackElemType::none, // TrackElemType::diagFlatToLeftBankedDown25
        TrackElemType::none, // TrackElemType::diagFlatToRightBankedDown25
        TrackElemType::none, // TrackElemType::diagLeftBankedDown25ToFlat
        TrackElemType::none, // TrackElemType::diagRightBankedDown25ToFlat
        TrackElemType::none, // TrackElemType::diagUp25LeftBanked
        TrackElemType::none, // TrackElemType::diagUp25RightBanked
        TrackElemType::none, // TrackElemType::diagDown25LeftBanked
        TrackElemType::none, // TrackElemType::diagDown25RightBanked
        TrackElemType::none, // TrackElemType::leftEighthBankToDiagUp25
        TrackElemType::none, // TrackElemType::rightEighthBankToDiagUp25
        TrackElemType::none, // TrackElemType::leftEighthBankToDiagDown25
        TrackElemType::none, // TrackElemType::rightEighthBankToDiagDown25
        TrackElemType::none, // TrackElemType::leftEighthBankToOrthogonalUp25
        TrackElemType::none, // TrackElemType::rightEighthBankToOrthogonalUp25
        TrackElemType::none, // TrackElemType::leftEighthBankToOrthogonalDown25
        TrackElemType::none, // TrackElemType::rightEighthBankToOrthogonalDown25
        TrackElemType::none, // TrackElemType::diagBrakes
        TrackElemType::none, // TrackElemType::diagBlockBrakes
        TrackElemType::none, // TrackElemType::down25Brakes
        TrackElemType::none, // TrackElemType::diagBooster
        TrackElemType::none, // TrackElemType::diagFlatToUp60LongBase,
        TrackElemType::none, // TrackElemType::diagUp60ToFlatLongBase,
        TrackElemType::none, // TrackElemType::diagFlatToDown60LongBase,
        TrackElemType::none, // TrackElemType::diagDown60ToFlatLongBase,
        TrackElemType::none, // TrackElemType::leftEighthDiveLoopUpToOrthogonal
        TrackElemType::none, // TrackElemType::rightEighthDiveLoopUpToOrthogonal
        TrackElemType::none, // TrackElemType::leftEighthDiveLoopDownToDiag
        TrackElemType::none, // TrackElemType::rightEighthDiveLoopDownToDiag
        TrackElemType::none, // TrackElemType::diagDown25Brakes
    };
    static_assert(std::size(kAlternativeTrackTypes) == EnumValue(TrackElemType::count));

    /** rct2: 0x0099DA34 */
    static constexpr money64 kTrackPricing[] = {
        65536,  // TrackElemType::flat
        98304,  // TrackElemType::endStation
        98304,  // TrackElemType::beginStation
        98304,  // TrackElemType::middleStation
        79872,  // TrackElemType::up25
        114688, // TrackElemType::up60
        73728,  // TrackElemType::flatToUp25
        96256,  // TrackElemType::up25ToUp60
        96256,  // TrackElemType::up60ToUp25
        73728,  // TrackElemType::up25ToFlat
        79872,  // TrackElemType::down25
        114688, // TrackElemType::down60
        73728,  // TrackElemType::flatToDown25
        96256,  // TrackElemType::down25ToDown60
        96256,  // TrackElemType::down60ToDown25
        73728,  // TrackElemType::down25ToFlat
        257359, // TrackElemType::leftQuarterTurn5Tiles
        257359, // TrackElemType::rightQuarterTurn5Tiles
        69632,  // TrackElemType::flatToLeftBank
        69632,  // TrackElemType::flatToRightBank
        69632,  // TrackElemType::leftBankToFlat
        69632,  // TrackElemType::rightBankToFlat
        273443, // TrackElemType::bankedLeftQuarterTurn5Tiles
        273443, // TrackElemType::bankedRightQuarterTurn5Tiles
        78336,  // TrackElemType::leftBankToUp25
        78336,  // TrackElemType::rightBankToUp25
        78336,  // TrackElemType::up25ToLeftBank
        78336,  // TrackElemType::up25ToRightBank
        78336,  // TrackElemType::leftBankToDown25
        78336,  // TrackElemType::rightBankToDown25
        78336,  // TrackElemType::down25ToLeftBank
        78336,  // TrackElemType::down25ToRightBank
        69632,  // TrackElemType::leftBank
        69632,  // TrackElemType::rightBank
        313656, // TrackElemType::leftQuarterTurn5TilesUp25
        313656, // TrackElemType::rightQuarterTurn5TilesUp25
        313656, // TrackElemType::leftQuarterTurn5TilesDown25
        313656, // TrackElemType::rightQuarterTurn5TilesDown25
        229376, // TrackElemType::sBendLeft
        229376, // TrackElemType::sBendRight
        491520, // TrackElemType::leftVerticalLoop
        491520, // TrackElemType::rightVerticalLoop
        154415, // TrackElemType::leftQuarterTurn3Tiles
        154415, // TrackElemType::rightQuarterTurn3Tiles
        164065, // TrackElemType::leftBankedQuarterTurn3Tiles
        164065, // TrackElemType::rightBankedQuarterTurn3Tiles
        270226, // TrackElemType::leftQuarterTurn3TilesUp25
        270226, // TrackElemType::rightQuarterTurn3TilesUp25
        270226, // TrackElemType::leftQuarterTurn3TilesDown25
        270226, // TrackElemType::rightQuarterTurn3TilesDown25
        51471,  // TrackElemType::leftQuarterTurn1Tile
        51471,  // TrackElemType::rightQuarterTurn1Tile
        212992, // TrackElemType::leftTwistDownToUp
        212992, // TrackElemType::rightTwistDownToUp
        212992, // TrackElemType::leftTwistUpToDown
        212992, // TrackElemType::rightTwistUpToDown
        294912, // TrackElemType::halfLoopUp
        294912, // TrackElemType::halfLoopDown
        229376, // TrackElemType::leftCorkscrewUp
        229376, // TrackElemType::rightCorkscrewUp
        229376, // TrackElemType::leftCorkscrewDown
        229376, // TrackElemType::rightCorkscrewDown
        98304,  // TrackElemType::flatToUp60
        98304,  // TrackElemType::up60ToFlat
        98304,  // TrackElemType::flatToDown60
        98304,  // TrackElemType::down60ToFlat
        524288, // TrackElemType::towerBase
        65536,  // TrackElemType::towerSection
        69632,  // TrackElemType::flatCovered
        83968,  // TrackElemType::up25Covered
        118784, // TrackElemType::up60Covered
        77824,  // TrackElemType::flatToUp25Covered
        100352, // TrackElemType::up25ToUp60Covered
        100352, // TrackElemType::up60ToUp25Covered
        77824,  // TrackElemType::up25ToFlatCovered
        83968,  // TrackElemType::down25Covered
        118784, // TrackElemType::down60Covered
        77824,  // TrackElemType::flatToDown25Covered
        100352, // TrackElemType::down25ToDown60Covered
        100352, // TrackElemType::down60ToDown25Covered
        77824,  // TrackElemType::down25ToFlatCovered
        261455, // TrackElemType::leftQuarterTurn5TilesCovered
        261455, // TrackElemType::rightQuarterTurn5TilesCovered
        233472, // TrackElemType::sBendLeftCovered
        233472, // TrackElemType::sBendRightCovered
        158511, // TrackElemType::leftQuarterTurn3TilesCovered
        158511, // TrackElemType::rightQuarterTurn3TilesCovered
        328192, // TrackElemType::leftHalfBankedHelixUpSmall
        328192, // TrackElemType::rightHalfBankedHelixUpSmall
        328192, // TrackElemType::leftHalfBankedHelixDownSmall
        328192, // TrackElemType::rightHalfBankedHelixDownSmall
        547072, // TrackElemType::leftHalfBankedHelixUpLarge
        547072, // TrackElemType::rightHalfBankedHelixUpLarge
        547072, // TrackElemType::leftHalfBankedHelixDownLarge
        547072, // TrackElemType::rightHalfBankedHelixDownLarge
        126976, // TrackElemType::leftQuarterTurn1TileUp60
        126976, // TrackElemType::rightQuarterTurn1TileUp60
        126976, // TrackElemType::leftQuarterTurn1TileDown60
        126976, // TrackElemType::rightQuarterTurn1TileDown60
        90112,  // TrackElemType::brakes
        77824,  // TrackElemType::booster
        65536,  // TrackElemType::maze
        273539, // TrackElemType::leftQuarterBankedHelixLargeUp
        273539, // TrackElemType::rightQuarterBankedHelixLargeUp
        273539, // TrackElemType::leftQuarterBankedHelixLargeDown
        273539, // TrackElemType::rightQuarterBankedHelixLargeDown
        273539, // TrackElemType::leftQuarterHelixLargeUp
        273539, // TrackElemType::rightQuarterHelixLargeUp
        273539, // TrackElemType::leftQuarterHelixLargeDown
        273539, // TrackElemType::rightQuarterHelixLargeDown
        83968,  // TrackElemType::up25LeftBanked
        83968,  // TrackElemType::up25RightBanked
        143360, // TrackElemType::waterfall
        118784, // TrackElemType::rapids
        196608, // TrackElemType::onRidePhoto
        83968,  // TrackElemType::down25LeftBanked
        83968,  // TrackElemType::down25RightBanked
        393216, // TrackElemType::watersplash
        376832, // TrackElemType::flatToUp60LongBase
        376832, // TrackElemType::up60ToFlatLongBase
        126976, // TrackElemType::whirlpool
        376832, // TrackElemType::down60ToFlatLongBase
        376832, // TrackElemType::flatToDown60LongBase
        643072, // TrackElemType::cableLiftHill
        491520, // TrackElemType::reverseFreefallSlope
        65536,  // TrackElemType::reverseFreefallVertical
        69632,  // TrackElemType::up90
        69632,  // TrackElemType::down90
        114688, // TrackElemType::up60ToUp90
        114688, // TrackElemType::down90ToDown60
        114688, // TrackElemType::up90ToUp60
        114688, // TrackElemType::down60ToDown90
        147456, // TrackElemType::brakeForDrop
        180151, // TrackElemType::leftEighthToDiag
        180151, // TrackElemType::rightEighthToDiag
        180151, // TrackElemType::leftEighthToOrthogonal
        180151, // TrackElemType::rightEighthToOrthogonal
        190960, // TrackElemType::leftEighthBankToDiag
        190960, // TrackElemType::rightEighthBankToDiag
        190960, // TrackElemType::leftEighthBankToOrthogonal
        190960, // TrackElemType::rightEighthBankToOrthogonal
        92681,  // TrackElemType::diagFlat
        111820, // TrackElemType::diagUp25
        147456, // TrackElemType::diagUp60
        102400, // TrackElemType::diagFlatToUp25
        129024, // TrackElemType::diagUp25ToUp60
        129024, // TrackElemType::diagUp60ToUp25
        102400, // TrackElemType::diagUp25ToFlat
        111820, // TrackElemType::diagDown25
        147456, // TrackElemType::diagDown60
        102400, // TrackElemType::diagFlatToDown25
        129024, // TrackElemType::diagDown25ToDown60
        129024, // TrackElemType::diagDown60ToDown25
        102400, // TrackElemType::diagDown25ToFlat
        107008, // TrackElemType::diagFlatToUp60
        107008, // TrackElemType::diagUp60ToFlat
        107008, // TrackElemType::diagFlatToDown60
        107008, // TrackElemType::diagDown60ToFlat
        96777,  // TrackElemType::diagFlatToLeftBank
        96777,  // TrackElemType::diagFlatToRightBank
        96777,  // TrackElemType::diagLeftBankToFlat
        96777,  // TrackElemType::diagRightBankToFlat
        106496, // TrackElemType::diagLeftBankToUp25
        106496, // TrackElemType::diagRightBankToUp25
        106496, // TrackElemType::diagUp25ToLeftBank
        106496, // TrackElemType::diagUp25ToRightBank
        106496, // TrackElemType::diagLeftBankToDown25
        106496, // TrackElemType::diagRightBankToDown25
        106496, // TrackElemType::diagDown25ToLeftBank
        106496, // TrackElemType::diagDown25ToRightBank
        96777,  // TrackElemType::diagLeftBank
        96777,  // TrackElemType::diagRightBank
        282624, // TrackElemType::logFlumeReverser
        217088, // TrackElemType::spinningTunnel
        221184, // TrackElemType::leftBarrelRollUpToDown
        221184, // TrackElemType::rightBarrelRollUpToDown
        221184, // TrackElemType::leftBarrelRollDownToUp
        221184, // TrackElemType::rightBarrelRollDownToUp
        270226, // TrackElemType::leftBankToLeftQuarterTurn3TilesUp25
        270226, // TrackElemType::rightBankToRightQuarterTurn3TilesUp25
        270226, // TrackElemType::leftQuarterTurn3TilesDown25ToLeftBank
        270226, // TrackElemType::rightQuarterTurn3TilesDown25ToRightBank
        96384,  // TrackElemType::poweredLift
        884736, // TrackElemType::leftLargeHalfLoopUp
        884736, // TrackElemType::rightLargeHalfLoopUp
        884736, // TrackElemType::leftLargeHalfLoopDown
        884736, // TrackElemType::rightLargeHalfLoopDown
        221184, // TrackElemType::leftFlyerTwistUp
        221184, // TrackElemType::rightFlyerTwistUp
        221184, // TrackElemType::leftFlyerTwistDown
        221184, // TrackElemType::rightFlyerTwistDown
        294912, // TrackElemType::flyerHalfLoopUninvertedUp
        294912, // TrackElemType::flyerHalfLoopInvertedDown
        229376, // TrackElemType::leftFlyerCorkscrewUp
        229376, // TrackElemType::rightFlyerCorkscrewUp
        229376, // TrackElemType::leftFlyerCorkscrewDown
        229376, // TrackElemType::rightFlyerCorkscrewDown
        294912, // TrackElemType::heartLineTransferUp
        294912, // TrackElemType::heartLineTransferDown
        417792, // TrackElemType::leftHeartLineRoll
        417792, // TrackElemType::rightHeartLineRoll
        159744, // TrackElemType::minigolfHoleA
        167936, // TrackElemType::minigolfHoleB
        172032, // TrackElemType::minigolfHoleC
        184320, // TrackElemType::minigolfHoleD
        184320, // TrackElemType::minigolfHoleE
        360448, // TrackElemType::multiDimInvertedFlatToDown90QuarterLoop
        360448, // TrackElemType::up90ToInvertedFlatQuarterLoop
        360448, // TrackElemType::invertedFlatToDown90QuarterLoop
        270226, // TrackElemType::leftCurvedLiftHill
        270226, // TrackElemType::rightCurvedLiftHill
        393216, // TrackElemType::leftReverser
        393216, // TrackElemType::rightReverser
        65536,  // TrackElemType::airThrustTopCap
        65536,  // TrackElemType::airThrustVerticalDown
        491520, // TrackElemType::airThrustVerticalDownToLevel
        94208,  // TrackElemType::blockBrakes
        287115, // TrackElemType::leftBankedQuarterTurn3TileUp25
        287115, // TrackElemType::rightBankedQuarterTurn3TileUp25
        287115, // TrackElemType::leftBankedQuarterTurn3TileDown25
        287115, // TrackElemType::rightBankedQuarterTurn3TileDown25
        333259, // TrackElemType::leftBankedQuarterTurn5TileUp25
        333259, // TrackElemType::rightBankedQuarterTurn5TileUp25
        333259, // TrackElemType::leftBankedQuarterTurn5TileDown25
        333259, // TrackElemType::rightBankedQuarterTurn5TileDown25
        83968,  // TrackElemType::up25ToLeftBankedUp25
        83968,  // TrackElemType::up25ToRightBankedUp25
        83968,  // TrackElemType::leftBankedUp25ToUp25
        83968,  // TrackElemType::rightBankedUp25ToUp25
        83968,  // TrackElemType::down25ToLeftBankedDown25
        83968,  // TrackElemType::down25ToRightBankedDown25
        83968,  // TrackElemType::leftBankedDown25ToDown25
        83968,  // TrackElemType::rightBankedDown25ToDown25
        77824,  // TrackElemType::leftBankedFlatToLeftBankedUp25
        77824,  // TrackElemType::rightBankedFlatToRightBankedUp25
        77824,  // TrackElemType::leftBankedUp25ToLeftBankedFlat
        77824,  // TrackElemType::rightBankedUp25ToRightBankedFlat
        77824,  // TrackElemType::leftBankedFlatToLeftBankedDown25
        77824,  // TrackElemType::rightBankedFlatToRightBankedDown25
        77824,  // TrackElemType::leftBankedDown25ToLeftBankedFlat
        77824,  // TrackElemType::rightBankedDown25ToRightBankedFlat
        77824,  // TrackElemType::flatToLeftBankedUp25
        77824,  // TrackElemType::flatToRightBankedUp25
        77824,  // TrackElemType::leftBankedUp25ToFlat
        77824,  // TrackElemType::rightBankedUp25ToFlat
        77824,  // TrackElemType::flatToLeftBankedDown25
        77824,  // TrackElemType::flatToRightBankedDown25
        77824,  // TrackElemType::leftBankedDown25ToFlat
        77824,  // TrackElemType::rightBankedDown25ToFlat
        151552, // TrackElemType::leftQuarterTurn1TileUp90
        151552, // TrackElemType::rightQuarterTurn1TileUp90
        151552, // TrackElemType::leftQuarterTurn1TileDown90
        151552, // TrackElemType::rightQuarterTurn1TileDown90
        360448, // TrackElemType::multiDimUp90ToInvertedFlatQuarterLoop
        360448, // TrackElemType::multiDimFlatToDown90QuarterLoop
        360448, // TrackElemType::multiDimInvertedUp90ToFlatQuarterLoop
        77824,  // TrackElemType::rotationControlToggle
        262144, // TrackElemType::flatTrack1x4A
        262144, // TrackElemType::flatTrack2x2
        1048576,// TrackElemType::flatTrack4x4
        524288, // TrackElemType::flatTrack2x4
        327680, // TrackElemType::flatTrack1x5
        65536,  // TrackElemType::flatTrack1x1A
        262144, // TrackElemType::flatTrack1x4B
        65536,  // TrackElemType::flatTrack1x1B
        262144, // TrackElemType::flatTrack1x4C
        524288, // TrackElemType::flatTrack3x3
        360448, // TrackElemType::leftLargeCorkscrewUp
        360448, // TrackElemType::rightLargeCorkscrewUp
        360448, // TrackElemType::leftLargeCorkscrewDown
        360448, // TrackElemType::rightCorkscrewDown
        589824, // TrackElemType::leftMediumHalfLoopUp
        589824, // TrackElemType::rightMediumHalfLoopUp
        589824, // TrackElemType::leftMediumHalfLoopDown
        589824, // TrackElemType::rightMediumHalfLoopDown
        229376, // TrackElemType::leftZeroGRollUp
        229376, // TrackElemType::rightZeroGRollUp
        229376, // TrackElemType::leftZeroGRollDown
        229376, // TrackElemType::rightZeroGRollDown
        425984, // TrackElemType::leftLargeZeroGRollUp
        425984, // TrackElemType::rightLargeZeroGRollUp
        425984, // TrackElemType::leftLargeZeroGRollDown
        425984, // TrackElemType::rightLargeZeroGRollDown
        884736, // TrackElemType::leftFlyerLargeHalfLoopUninvertedUp
        884736, // TrackElemType::rightFlyerLargeHalfLoopUninvertedUp
        884736, // TrackElemType::leftFlyerLargeHalfLoopInvertedDown
        884736, // TrackElemType::rightFlyerLargeHalfLoopInvertedDown
        884736, // TrackElemType::leftFlyerLargeHalfLoopInvertedUp
        884736, // TrackElemType::rightFlyerLargeHalfLoopInvertedUp
        884736, // TrackElemType::leftFlyerLargeHalfLoopUninvertedDown
        884736, // TrackElemType::rightFlyerLargeHalfLoopUninvertedDown
        294912, // TrackElemType::flyerHalfLoopInvertedUp
        294912, // TrackElemType::flyerHalfLoopUninvertedDown
        219559, // TrackElemType::leftEighthToDiagUp25
        219559, // TrackElemType::rightEighthToDiagUp25
        219559, // TrackElemType::leftEighthToDiagDown25
        219559, // TrackElemType::rightEighthToDiagDown25
        219559, // TrackElemType::leftEighthToOrthogonalUp25
        219559, // TrackElemType::rightEighthToOrthogonalUp25
        219559, // TrackElemType::leftEighthToOrthogonalDown25
        219559, // TrackElemType::rightEighthToOrthogonalDown25
        118809, // TrackElemType::diagUp25ToLeftBankedUp25
        118809, // TrackElemType::diagUp25ToRightBankedUp25
        118809, // TrackElemType::diagLeftBankedUp25ToUp25
        118809, // TrackElemType::diagRightBankedUp25ToUp25
        118809, // TrackElemType::diagDown25ToLeftBankedDown25
        118809, // TrackElemType::diagDown25ToRightBankedDown25
        118809, // TrackElemType::diagLeftBankedDown25ToDown25
        118809, // TrackElemType::diagRightBankedDown25ToDown25
        108800, // TrackElemType::diagLeftBankedFlatToLeftBankedUp25
        108800, // TrackElemType::diagRightBankedFlatToRightBankedUp25
        108800, // TrackElemType::diagLeftBankedUp25ToLeftBankedFlat
        108800, // TrackElemType::diagRightBankedUp25ToRightBankedFlat
        108800, // TrackElemType::diagLeftBankedFlatToLeftBankedDown25
        108800, // TrackElemType::diagRightBankedFlatToRightBankedDown25
        108800, // TrackElemType::diagLeftBankedDown25ToLeftBankedFlat
        108800, // TrackElemType::diagRightBankedDown25ToRightBankedFlat
        108800, // TrackElemType::diagFlatToLeftBankedUp25
        108800, // TrackElemType::diagFlatToRightBankedUp25
        108800, // TrackElemType::diagLeftBankedUp25ToFlat
        108800, // TrackElemType::diagRightBankedUp25ToFlat
        108800, // TrackElemType::diagFlatToLeftBankedDown25
        108800, // TrackElemType::diagFlatToRightBankedDown25
        108800, // TrackElemType::diagLeftBankedDown25ToFlat
        108800, // TrackElemType::diagRightBankedDown25ToFlat
        118809, // TrackElemType::diagUp25LeftBanked
        118809, // TrackElemType::diagUp25RightBanked
        118809, // TrackElemType::diagDown25LeftBanked
        118809, // TrackElemType::diagDown25RightBanked
        233281, // TrackElemType::leftEighthBankToDiagUp25
        233281, // TrackElemType::rightEighthBankToDiagUp25
        233281, // TrackElemType::leftEighthBankToDiagDown25
        233281, // TrackElemType::rightEighthBankToDiagDown25
        233281, // TrackElemType::leftEighthBankToOrthogonalUp25
        233281, // TrackElemType::rightEighthBankToOrthogonalUp25
        233281, // TrackElemType::leftEighthBankToOrthogonalDown25
        233281, // TrackElemType::rightEighthBankToOrthogonalDown25
        123456, // TrackElemType::diagBrakes
        123456, // TrackElemType::diagBlockBrakes
        109824, // TrackElemType::down25Brakes
        123456, // TrackElemType::diagBooster
        399690 , // TrackElemType::diagFlatToUp60LongBase,
        399690, // TrackElemType::diagUp60ToFlatLongBase,
        399690, // TrackElemType::diagFlatToDown60LongBase,
        399690, // TrackElemType::diagDown60ToFlatLongBase,
        458752, // TrackElemType::leftEighthDiveLoopUpToOrthogonal
        458752, // TrackElemType::rightEighthDiveLoopUpToOrthogonal
        458752, // TrackElemType::leftEighthDiveLoopDownToDiag
        458752, // TrackElemType::rightEighthDiveLoopDownToDiag
        109824, // TrackElemType::diagDown25Brakes
    };
    static_assert(std::size(kTrackPricing) == EnumValue(TrackElemType::count));

    /** rct2: 0x0099EA1C */
    static constexpr TrackElemType kTrackElementMirrorMap[] = {
        TrackElemType::flat,
        TrackElemType::endStation,
        TrackElemType::beginStation,
        TrackElemType::middleStation,
        TrackElemType::up25,
        TrackElemType::up60,
        TrackElemType::flatToUp25,
        TrackElemType::up25ToUp60,
        TrackElemType::up60ToUp25,
        TrackElemType::up25ToFlat,
        TrackElemType::down25,
        TrackElemType::down60,
        TrackElemType::flatToDown25,
        TrackElemType::down25ToDown60,
        TrackElemType::down60ToDown25,
        TrackElemType::down25ToFlat,
        TrackElemType::rightQuarterTurn5Tiles, // TrackElemType::leftQuarterTurn5Tiles
        TrackElemType::leftQuarterTurn5Tiles, // TrackElemType::rightQuarterTurn5Tiles
        TrackElemType::flatToRightBank, // TrackElemType::flatToLeftBank
        TrackElemType::flatToLeftBank, // TrackElemType::flatToRightBank
        TrackElemType::rightBankToFlat, // TrackElemType::leftBankToFlat
        TrackElemType::leftBankToFlat, // TrackElemType::rightBankToFlat
        TrackElemType::bankedRightQuarterTurn5Tiles, // TrackElemType::bankedLeftQuarterTurn5Tiles
        TrackElemType::bankedLeftQuarterTurn5Tiles, // TrackElemType::bankedRightQuarterTurn5Tiles
        TrackElemType::rightBankToUp25, // TrackElemType::leftBankToUp25
        TrackElemType::leftBankToUp25, // TrackElemType::rightBankToUp25
        TrackElemType::up25ToRightBank, // TrackElemType::up25ToLeftBank
        TrackElemType::up25ToLeftBank, // TrackElemType::up25ToRightBank
        TrackElemType::rightBankToDown25, // TrackElemType::leftBankToDown25
        TrackElemType::leftBankToDown25, // TrackElemType::rightBankToDown25
        TrackElemType::down25ToRightBank, // TrackElemType::down25ToLeftBank
        TrackElemType::down25ToLeftBank, // TrackElemType::down25ToRightBank
        TrackElemType::rightBank, // TrackElemType::leftBank
        TrackElemType::leftBank, // TrackElemType::rightBank
        TrackElemType::rightQuarterTurn5TilesUp25, // TrackElemType::leftQuarterTurn5TilesUp25
        TrackElemType::leftQuarterTurn5TilesUp25, // TrackElemType::rightQuarterTurn5TilesUp25
        TrackElemType::rightQuarterTurn5TilesDown25, // TrackElemType::leftQuarterTurn5TilesDown25
        TrackElemType::leftQuarterTurn5TilesDown25, // TrackElemType::rightQuarterTurn5TilesDown25
        TrackElemType::sBendRight, // TrackElemType::sBendLeft
        TrackElemType::sBendLeft, // TrackElemType::sBendRight
        TrackElemType::rightVerticalLoop, // TrackElemType::leftVerticalLoop
        TrackElemType::leftVerticalLoop, // TrackElemType::rightVerticalLoop
        TrackElemType::rightQuarterTurn3Tiles, // TrackElemType::leftQuarterTurn3Tiles
        TrackElemType::leftQuarterTurn3Tiles, // TrackElemType::rightQuarterTurn3Tiles
        TrackElemType::rightBankedQuarterTurn3Tiles, // TrackElemType::leftBankedQuarterTurn3Tiles
        TrackElemType::leftBankedQuarterTurn3Tiles, // TrackElemType::rightBankedQuarterTurn3Tiles
        TrackElemType::rightQuarterTurn3TilesUp25, // TrackElemType::leftQuarterTurn3TilesUp25
        TrackElemType::leftQuarterTurn3TilesUp25, // TrackElemType::rightQuarterTurn3TilesUp25
        TrackElemType::rightQuarterTurn3TilesDown25, // TrackElemType::leftQuarterTurn3TilesDown25
        TrackElemType::leftQuarterTurn3TilesDown25, // TrackElemType::rightQuarterTurn3TilesDown25
        TrackElemType::rightQuarterTurn1Tile, // TrackElemType::leftQuarterTurn1Tile
        TrackElemType::leftQuarterTurn1Tile, // TrackElemType::rightQuarterTurn1Tile
        TrackElemType::rightTwistDownToUp, // TrackElemType::leftTwistDownToUp
        TrackElemType::leftTwistDownToUp, // TrackElemType::rightTwistDownToUp
        TrackElemType::rightTwistUpToDown, // TrackElemType::leftTwistUpToDown
        TrackElemType::leftTwistUpToDown, // TrackElemType::rightTwistUpToDown
        TrackElemType::halfLoopUp,
        TrackElemType::halfLoopDown,
        TrackElemType::rightCorkscrewUp, // TrackElemType::leftCorkscrewUp
        TrackElemType::leftCorkscrewUp, // TrackElemType::rightCorkscrewUp
        TrackElemType::rightCorkscrewDown, // TrackElemType::leftCorkscrewDown
        TrackElemType::leftCorkscrewDown, // TrackElemType::rightCorkscrewDown
        TrackElemType::flatToUp60,
        TrackElemType::up60ToFlat,
        TrackElemType::flatToDown60,
        TrackElemType::down60ToFlat,
        TrackElemType::towerBase,
        TrackElemType::towerSection,
        TrackElemType::flatCovered,
        TrackElemType::up25Covered,
        TrackElemType::up60Covered,
        TrackElemType::flatToUp25Covered,
        TrackElemType::up25ToUp60Covered,
        TrackElemType::up60ToUp25Covered,
        TrackElemType::up25ToFlatCovered,
        TrackElemType::down25Covered,
        TrackElemType::down60Covered,
        TrackElemType::flatToDown25Covered,
        TrackElemType::down25ToDown60Covered,
        TrackElemType::down60ToDown25Covered,
        TrackElemType::down25ToFlatCovered,
        TrackElemType::rightQuarterTurn5TilesCovered, // TrackElemType::leftQuarterTurn5TilesCovered
        TrackElemType::leftQuarterTurn5TilesCovered, // TrackElemType::rightQuarterTurn5TilesCovered
        TrackElemType::sBendRightCovered, // TrackElemType::sBendLeftCovered
        TrackElemType::sBendLeftCovered, // TrackElemType::sBendRightCovered
        TrackElemType::rightQuarterTurn3TilesCovered, // TrackElemType::leftQuarterTurn3TilesCovered
        TrackElemType::leftQuarterTurn3TilesCovered, // TrackElemType::rightQuarterTurn3TilesCovered
        TrackElemType::rightHalfBankedHelixUpSmall, // TrackElemType::leftHalfBankedHelixUpSmall
        TrackElemType::leftHalfBankedHelixUpSmall, // TrackElemType::rightHalfBankedHelixUpSmall
        TrackElemType::rightHalfBankedHelixDownSmall, // TrackElemType::leftHalfBankedHelixDownSmall
        TrackElemType::leftHalfBankedHelixDownSmall, // TrackElemType::rightHalfBankedHelixDownSmall
        TrackElemType::rightHalfBankedHelixUpLarge, // TrackElemType::leftHalfBankedHelixUpLarge
        TrackElemType::leftHalfBankedHelixUpLarge, // TrackElemType::rightHalfBankedHelixUpLarge
        TrackElemType::rightHalfBankedHelixDownLarge, // TrackElemType::leftHalfBankedHelixDownLarge
        TrackElemType::leftHalfBankedHelixDownLarge, // TrackElemType::rightHalfBankedHelixDownLarge
        TrackElemType::rightQuarterTurn1TileUp60, // TrackElemType::leftQuarterTurn1TileUp60
        TrackElemType::leftQuarterTurn1TileUp60, // TrackElemType::rightQuarterTurn1TileUp60
        TrackElemType::rightQuarterTurn1TileDown60, // TrackElemType::leftQuarterTurn1TileDown60
        TrackElemType::leftQuarterTurn1TileDown60, // TrackElemType::rightQuarterTurn1TileDown60
        TrackElemType::brakes,
        TrackElemType::booster,
        TrackElemType::maze,
        TrackElemType::rightQuarterBankedHelixLargeUp, // TrackElemType::leftQuarterBankedHelixLargeUp
        TrackElemType::leftQuarterBankedHelixLargeUp, // TrackElemType::rightQuarterBankedHelixLargeUp
        TrackElemType::rightQuarterBankedHelixLargeDown, // TrackElemType::leftQuarterBankedHelixLargeDown
        TrackElemType::leftQuarterBankedHelixLargeDown, // TrackElemType::rightQuarterBankedHelixLargeDown
        TrackElemType::rightQuarterHelixLargeUp, // TrackElemType::leftQuarterHelixLargeUp
        TrackElemType::leftQuarterHelixLargeUp, // TrackElemType::rightQuarterHelixLargeUp
        TrackElemType::rightQuarterHelixLargeDown, // TrackElemType::leftQuarterHelixLargeDown
        TrackElemType::leftQuarterHelixLargeDown, // TrackElemType::rightQuarterHelixLargeDown
        TrackElemType::up25RightBanked, // TrackElemType::up25LeftBanked
        TrackElemType::up25LeftBanked, // TrackElemType::up25RightBanked
        TrackElemType::waterfall,
        TrackElemType::rapids,
        TrackElemType::onRidePhoto,
        TrackElemType::down25RightBanked, // TrackElemType::down25LeftBanked
        TrackElemType::down25LeftBanked, // TrackElemType::down25RightBanked
        TrackElemType::watersplash,
        TrackElemType::flatToUp60LongBase,
        TrackElemType::up60ToFlatLongBase,
        TrackElemType::whirlpool,
        TrackElemType::down60ToFlatLongBase,
        TrackElemType::flatToDown60LongBase,
        TrackElemType::cableLiftHill,
        TrackElemType::reverseFreefallSlope,
        TrackElemType::reverseFreefallVertical,
        TrackElemType::up90,
        TrackElemType::down90,
        TrackElemType::up60ToUp90,
        TrackElemType::down90ToDown60,
        TrackElemType::up90ToUp60,
        TrackElemType::down60ToDown90,
        TrackElemType::brakeForDrop,
        TrackElemType::rightEighthToDiag, // TrackElemType::leftEighthToDiag
        TrackElemType::leftEighthToDiag, // TrackElemType::rightEighthToDiag
        TrackElemType::rightEighthToOrthogonal, // TrackElemType::leftEighthToOrthogonal
        TrackElemType::leftEighthToOrthogonal, // TrackElemType::rightEighthToOrthogonal
        TrackElemType::rightEighthBankToDiag, // TrackElemType::leftEighthBankToDiag
        TrackElemType::leftEighthBankToDiag, // TrackElemType::rightEighthBankToDiag
        TrackElemType::rightEighthBankToOrthogonal, // TrackElemType::leftEighthBankToOrthogonal
        TrackElemType::leftEighthBankToOrthogonal, // TrackElemType::rightEighthBankToOrthogonal
        TrackElemType::diagFlat,
        TrackElemType::diagUp25,
        TrackElemType::diagUp60,
        TrackElemType::diagFlatToUp25,
        TrackElemType::diagUp25ToUp60,
        TrackElemType::diagUp60ToUp25,
        TrackElemType::diagUp25ToFlat,
        TrackElemType::diagDown25,
        TrackElemType::diagDown60,
        TrackElemType::diagFlatToDown25,
        TrackElemType::diagDown25ToDown60,
        TrackElemType::diagDown60ToDown25,
        TrackElemType::diagDown25ToFlat,
        TrackElemType::diagFlatToUp60,
        TrackElemType::diagUp60ToFlat,
        TrackElemType::diagFlatToDown60,
        TrackElemType::diagDown60ToFlat,
        TrackElemType::diagFlatToRightBank, // TrackElemType::diagFlatToLeftBank
        TrackElemType::diagFlatToLeftBank, // TrackElemType::diagFlatToRightBank
        TrackElemType::diagRightBankToFlat, // TrackElemType::diagLeftBankToFlat
        TrackElemType::diagLeftBankToFlat, // TrackElemType::diagRightBankToFlat
        TrackElemType::diagRightBankToUp25, // TrackElemType::diagLeftBankToUp25
        TrackElemType::diagLeftBankToUp25, // TrackElemType::diagRightBankToUp25
        TrackElemType::diagUp25ToRightBank, // TrackElemType::diagUp25ToLeftBank
        TrackElemType::diagUp25ToLeftBank, // TrackElemType::diagUp25ToRightBank
        TrackElemType::diagRightBankToDown25, // TrackElemType::diagLeftBankToDown25
        TrackElemType::diagLeftBankToDown25, // TrackElemType::diagRightBankToDown25
        TrackElemType::diagDown25ToRightBank, // TrackElemType::diagDown25ToLeftBank
        TrackElemType::diagDown25ToLeftBank, // TrackElemType::diagDown25ToRightBank
        TrackElemType::diagRightBank, // TrackElemType::diagLeftBank
        TrackElemType::diagLeftBank, // TrackElemType::diagRightBank
        TrackElemType::logFlumeReverser,
        TrackElemType::spinningTunnel,
        TrackElemType::rightBarrelRollUpToDown, // TrackElemType::leftBarrelRollUpToDown
        TrackElemType::leftBarrelRollUpToDown, // TrackElemType::rightBarrelRollUpToDown
        TrackElemType::rightBarrelRollDownToUp, // TrackElemType::leftBarrelRollDownToUp
        TrackElemType::leftBarrelRollDownToUp, // TrackElemType::rightBarrelRollDownToUp
        TrackElemType::rightBankToRightQuarterTurn3TilesUp25, // TrackElemType::leftBankToLeftQuarterTurn3TilesUp25
        TrackElemType::leftBankToLeftQuarterTurn3TilesUp25, // TrackElemType::rightBankToRightQuarterTurn3TilesUp25
        TrackElemType::rightQuarterTurn3TilesDown25ToRightBank, // TrackElemType::leftQuarterTurn3TilesDown25ToLeftBank
        TrackElemType::leftQuarterTurn3TilesDown25ToLeftBank, // TrackElemType::rightQuarterTurn3TilesDown25ToRightBank
        TrackElemType::poweredLift,
        TrackElemType::rightLargeHalfLoopUp, // TrackElemType::leftLargeHalfLoopUp
        TrackElemType::leftLargeHalfLoopUp, // TrackElemType::rightLargeHalfLoopUp
        TrackElemType::rightLargeHalfLoopDown, // TrackElemType::leftLargeHalfLoopDown
        TrackElemType::leftLargeHalfLoopDown, // TrackElemType::rightLargeHalfLoopDown
        TrackElemType::rightFlyerTwistUp, // TrackElemType::leftFlyerTwistUp
        TrackElemType::leftFlyerTwistUp, // TrackElemType::rightFlyerTwistUp
        TrackElemType::rightFlyerTwistDown, // TrackElemType::leftFlyerTwistDown
        TrackElemType::leftFlyerTwistDown, // TrackElemType::rightFlyerTwistDown
        TrackElemType::flyerHalfLoopUninvertedUp,
        TrackElemType::flyerHalfLoopInvertedDown,
        TrackElemType::rightFlyerCorkscrewUp, // TrackElemType::leftFlyerCorkscrewUp
        TrackElemType::leftFlyerCorkscrewUp, // TrackElemType::rightFlyerCorkscrewUp
        TrackElemType::rightFlyerCorkscrewDown, // TrackElemType::leftFlyerCorkscrewDown
        TrackElemType::leftFlyerCorkscrewDown, // TrackElemType::rightFlyerCorkscrewDown
        TrackElemType::heartLineTransferUp,
        TrackElemType::heartLineTransferDown,
        TrackElemType::rightHeartLineRoll, // TrackElemType::leftHeartLineRoll
        TrackElemType::leftHeartLineRoll, // TrackElemType::rightHeartLineRoll
        TrackElemType::minigolfHoleA,
        TrackElemType::minigolfHoleB,
        TrackElemType::minigolfHoleC,
        TrackElemType::minigolfHoleE, // TrackElemType::minigolfHoleD
        TrackElemType::minigolfHoleD, // TrackElemType::minigolfHoleE
        TrackElemType::multiDimInvertedFlatToDown90QuarterLoop,
        TrackElemType::up90ToInvertedFlatQuarterLoop,
        TrackElemType::invertedFlatToDown90QuarterLoop,
        TrackElemType::rightCurvedLiftHill, // TrackElemType::leftCurvedLiftHill
        TrackElemType::leftCurvedLiftHill, // TrackElemType::rightCurvedLiftHill
        TrackElemType::rightReverser, // TrackElemType::leftReverser
        TrackElemType::leftReverser, // TrackElemType::rightReverser
        TrackElemType::airThrustTopCap,
        TrackElemType::airThrustVerticalDown,
        TrackElemType::airThrustVerticalDownToLevel,
        TrackElemType::blockBrakes,
        TrackElemType::rightBankedQuarterTurn3TileUp25, // TrackElemType::leftBankedQuarterTurn3TileUp25
        TrackElemType::leftBankedQuarterTurn3TileUp25, // TrackElemType::rightBankedQuarterTurn3TileUp25
        TrackElemType::rightBankedQuarterTurn3TileDown25, // TrackElemType::leftBankedQuarterTurn3TileDown25
        TrackElemType::leftBankedQuarterTurn3TileDown25, // TrackElemType::rightBankedQuarterTurn3TileDown25
        TrackElemType::rightBankedQuarterTurn5TileUp25, // TrackElemType::leftBankedQuarterTurn5TileUp25
        TrackElemType::leftBankedQuarterTurn5TileUp25, // TrackElemType::rightBankedQuarterTurn5TileUp25
        TrackElemType::rightBankedQuarterTurn5TileDown25, // TrackElemType::leftBankedQuarterTurn5TileDown25
        TrackElemType::leftBankedQuarterTurn5TileDown25, // TrackElemType::rightBankedQuarterTurn5TileDown25
        TrackElemType::up25ToRightBankedUp25, // TrackElemType::up25ToLeftBankedUp25
        TrackElemType::up25ToLeftBankedUp25, // TrackElemType::up25ToRightBankedUp25
        TrackElemType::rightBankedUp25ToUp25, // TrackElemType::leftBankedUp25ToUp25
        TrackElemType::leftBankedUp25ToUp25, // TrackElemType::rightBankedUp25ToUp25
        TrackElemType::down25ToRightBankedDown25, // TrackElemType::down25ToLeftBankedDown25
        TrackElemType::down25ToLeftBankedDown25, // TrackElemType::down25ToRightBankedDown25
        TrackElemType::rightBankedDown25ToDown25, // TrackElemType::leftBankedDown25ToDown25
        TrackElemType::leftBankedDown25ToDown25, // TrackElemType::rightBankedDown25ToDown25
        TrackElemType::rightBankedFlatToRightBankedUp25, // TrackElemType::leftBankedFlatToLeftBankedUp25
        TrackElemType::leftBankedFlatToLeftBankedUp25, // TrackElemType::rightBankedFlatToRightBankedUp25
        TrackElemType::rightBankedUp25ToRightBankedFlat, // TrackElemType::leftBankedUp25ToLeftBankedFlat
        TrackElemType::leftBankedUp25ToLeftBankedFlat, // TrackElemType::rightBankedUp25ToRightBankedFlat
        TrackElemType::rightBankedFlatToRightBankedDown25, // TrackElemType::leftBankedFlatToLeftBankedDown25
        TrackElemType::leftBankedFlatToLeftBankedDown25, // TrackElemType::rightBankedFlatToRightBankedDown25
        TrackElemType::rightBankedDown25ToRightBankedFlat, // TrackElemType::leftBankedDown25ToLeftBankedFlat
        TrackElemType::leftBankedDown25ToLeftBankedFlat, // TrackElemType::rightBankedDown25ToRightBankedFlat
        TrackElemType::flatToRightBankedUp25, // TrackElemType::flatToLeftBankedUp25
        TrackElemType::flatToLeftBankedUp25, // TrackElemType::flatToRightBankedUp25
        TrackElemType::rightBankedUp25ToFlat, // TrackElemType::leftBankedUp25ToFlat
        TrackElemType::leftBankedUp25ToFlat, // TrackElemType::rightBankedUp25ToFlat
        TrackElemType::flatToRightBankedDown25, // TrackElemType::flatToLeftBankedDown25
        TrackElemType::flatToLeftBankedDown25, // TrackElemType::flatToRightBankedDown25
        TrackElemType::rightBankedDown25ToFlat, // TrackElemType::leftBankedDown25ToFlat
        TrackElemType::leftBankedDown25ToFlat, // TrackElemType::rightBankedDown25ToFlat
        TrackElemType::rightQuarterTurn1TileUp90, // TrackElemType::leftQuarterTurn1TileUp90
        TrackElemType::leftQuarterTurn1TileUp90, // TrackElemType::rightQuarterTurn1TileUp90
        TrackElemType::rightQuarterTurn1TileDown90, // TrackElemType::leftQuarterTurn1TileDown90
        TrackElemType::leftQuarterTurn1TileDown90, // TrackElemType::rightQuarterTurn1TileDown90
        TrackElemType::multiDimUp90ToInvertedFlatQuarterLoop,
        TrackElemType::multiDimFlatToDown90QuarterLoop,
        TrackElemType::multiDimInvertedUp90ToFlatQuarterLoop,
        TrackElemType::rotationControlToggle,
        TrackElemType::flatTrack1x4A, // TrackElemType::flatTrack1x4A
        TrackElemType::flatTrack2x2, // TrackElemType::flatTrack2x2
        TrackElemType::flatTrack4x4, // TrackElemType::flatTrack4x4
        TrackElemType::flatTrack2x4, // TrackElemType::flatTrack2x4
        TrackElemType::flatTrack1x5, // TrackElemType::flatTrack1x5
        TrackElemType::flatTrack1x1A, // TrackElemType::flatTrack1x1A
        TrackElemType::flatTrack1x4B, // TrackElemType::flatTrack1x4B
        TrackElemType::flatTrack1x1B, // TrackElemType::flatTrack1x1B
        TrackElemType::flatTrack1x4C, // TrackElemType::flatTrack1x4C
        TrackElemType::flatTrack3x3, // TrackElemType::flatTrack3x3
        TrackElemType::rightLargeCorkscrewUp, // TrackElemType::leftLargeCorkscrewUp
        TrackElemType::leftLargeCorkscrewUp, // TrackElemType::rightLargeCorkscrewUp
        TrackElemType::rightLargeCorkscrewDown, // TrackElemType::leftLargeCorkscrewDown
        TrackElemType::leftLargeCorkscrewDown, // TrackElemType::rightLargeCorkscrewDown
        TrackElemType::rightMediumHalfLoopUp, // TrackElemType::leftMediumHalfLoopUp
        TrackElemType::leftMediumHalfLoopUp, // TrackElemType::rightMediumHalfLoopUp
        TrackElemType::rightMediumHalfLoopDown, // TrackElemType::leftMediumHalfLoopDown
        TrackElemType::leftMediumHalfLoopDown, // TrackElemType::rightMediumHalfLoopDown
        TrackElemType::rightZeroGRollUp, // TrackElemType::leftZeroGRollUp
        TrackElemType::leftZeroGRollUp, // TrackElemType::rightZeroGRollUp
        TrackElemType::rightZeroGRollDown, // TrackElemType::leftZeroGRollDown
        TrackElemType::leftZeroGRollDown, // TrackElemType::rightZeroGRollDown
        TrackElemType::rightLargeZeroGRollUp, // TrackElemType::leftLargeZeroGRollUp
        TrackElemType::leftLargeZeroGRollUp, // TrackElemType::rightLargeZeroGRollUp
        TrackElemType::rightLargeZeroGRollDown, // TrackElemType::leftLargeZeroGRollDown
        TrackElemType::leftLargeZeroGRollDown, // TrackElemType::rightLargeZeroGRollDown
        TrackElemType::rightFlyerLargeHalfLoopUninvertedUp, // TrackElemType::leftFlyerLargeHalfLoopUninvertedUp
        TrackElemType::leftFlyerLargeHalfLoopUninvertedUp, // TrackElemType::rightFlyerLargeHalfLoopUninvertedUp
        TrackElemType::rightFlyerLargeHalfLoopInvertedDown, // TrackElemType::leftFlyerLargeHalfLoopInvertedDown
        TrackElemType::leftFlyerLargeHalfLoopInvertedDown, // TrackElemType::rightFlyerLargeHalfLoopInvertedDown
        TrackElemType::rightFlyerLargeHalfLoopInvertedUp, // TrackElemType::leftFlyerLargeHalfLoopInvertedUp
        TrackElemType::leftFlyerLargeHalfLoopInvertedUp, // TrackElemType::rightFlyerLargeHalfLoopInvertedUp
        TrackElemType::rightFlyerLargeHalfLoopUninvertedDown, // TrackElemType::leftFlyerLargeHalfLoopUninvertedDown
        TrackElemType::leftFlyerLargeHalfLoopUninvertedDown, // TrackElemType::rightFlyerLargeHalfLoopUninvertedDown
        TrackElemType::flyerHalfLoopInvertedUp,
        TrackElemType::flyerHalfLoopUninvertedDown,
        TrackElemType::rightEighthToDiagUp25,          // TrackElemType::leftEighthToDiagUp25
        TrackElemType::leftEighthToDiagUp25,           // TrackElemType::rightEighthToDiagUp25
        TrackElemType::rightEighthToDiagDown25,        // TrackElemType::leftEighthToDiagDown25
        TrackElemType::leftEighthToDiagDown25,         // TrackElemType::rightEighthToDiagDown25
        TrackElemType::rightEighthToOrthogonalUp25,    // TrackElemType::leftEighthToOrthogonalUp25
        TrackElemType::leftEighthToOrthogonalUp25,     // TrackElemType::rightEighthToOrthogonalUp25
        TrackElemType::rightEighthToOrthogonalDown25,  // TrackElemType::leftEighthToOrthogonalDown25
        TrackElemType::leftEighthToOrthogonalDown25,   // TrackElemType::rightEighthToOrthogonalDown25
        TrackElemType::diagUp25ToRightBankedUp25,             // TrackElemType::diagUp25ToRightBankedUp25
        TrackElemType::diagUp25ToLeftBankedUp25,              // TrackElemType::diagUp25ToLeftBankedUp25
        TrackElemType::diagRightBankedUp25ToUp25,             // TrackElemType::diagRightBankedUp25ToUp25
        TrackElemType::diagLeftBankedUp25ToUp25,              // TrackElemType::diagLeftBankedUp25ToUp25
        TrackElemType::diagDown25ToRightBankedDown25,         // TrackElemType::diagDown25ToRightBankedDown25
        TrackElemType::diagDown25ToLeftBankedDown25,          // TrackElemType::diagDown25ToLeftBankedDown25
        TrackElemType::diagRightBankedDown25ToDown25,         // TrackElemType::diagRightBankedDown25ToDown25
        TrackElemType::diagLeftBankedDown25ToDown25,          // TrackElemType::diagLeftBankedDown25ToDown25
        TrackElemType::diagRightBankedFlatToRightBankedUp25,  // TrackElemType::diagRightBankedFlatToRightBankedUp25
        TrackElemType::diagLeftBankedFlatToLeftBankedUp25,    // TrackElemType::diagLeftBankedFlatToLeftBankedUp25
        TrackElemType::diagRightBankedUp25ToRightBankedFlat,  // TrackElemType::diagRightBankedUp25ToRightBankedFlat
        TrackElemType::diagLeftBankedUp25ToLeftBankedFlat,    // TrackElemType::diagLeftBankedUp25ToLeftBankedFlat
        TrackElemType::diagRightBankedFlatToRightBankedDown25,// TrackElemType::diagRightBankedFlatToRightBankedDown25
        TrackElemType::diagLeftBankedFlatToLeftBankedDown25,  // TrackElemType::diagLeftBankedFlatToLeftBankedDown25
        TrackElemType::diagRightBankedDown25ToRightBankedFlat,// TrackElemType::diagRightBankedDown25ToRightBankedFlat
        TrackElemType::diagLeftBankedDown25ToLeftBankedFlat,  // TrackElemType::diagLeftBankedDown25ToLeftBankedFlat
        TrackElemType::diagFlatToRightBankedUp25,             // TrackElemType::diagFlatToRightBankedUp25
        TrackElemType::diagFlatToLeftBankedUp25,              // TrackElemType::diagFlatToLeftBankedUp25
        TrackElemType::diagRightBankedUp25ToFlat,             // TrackElemType::diagRightBankedUp25ToFlat
        TrackElemType::diagLeftBankedUp25ToFlat,              // TrackElemType::diagLeftBankedUp25ToFlat
        TrackElemType::diagFlatToRightBankedDown25,           // TrackElemType::diagFlatToRightBankedDown25
        TrackElemType::diagFlatToLeftBankedDown25,            // TrackElemType::diagFlatToLeftBankedDown25
        TrackElemType::diagRightBankedDown25ToFlat,           // TrackElemType::diagRightBankedDown25ToFlat
        TrackElemType::diagLeftBankedDown25ToFlat,            // TrackElemType::diagLeftBankedDown25ToFlat
        TrackElemType::diagUp25RightBanked,                   // TrackElemType::diagUp25RightBanked
        TrackElemType::diagUp25LeftBanked,                    // TrackElemType::diagUp25LeftBanked
        TrackElemType::diagDown25RightBanked,                 // TrackElemType::diagDown25RightBanked
        TrackElemType::diagDown25LeftBanked,                  // TrackElemType::diagDown25LeftBanked
        TrackElemType::rightEighthBankToDiagUp25,             // TrackElemType::rightEighthBankToDiagUp25
        TrackElemType::leftEighthBankToDiagUp25,              // TrackElemType::leftEighthBankToDiagUp25
        TrackElemType::rightEighthBankToDiagDown25,           // TrackElemType::rightEighthBankToDiagDown25
        TrackElemType::leftEighthBankToDiagDown25,            // TrackElemType::leftEighthBankToDiagDown25
        TrackElemType::rightEighthBankToOrthogonalUp25,       // TrackElemType::rightEighthBankToOrthogonalUp25
        TrackElemType::leftEighthBankToOrthogonalUp25,        // TrackElemType::leftEighthBankToOrthogonalUp25
        TrackElemType::rightEighthBankToOrthogonalDown25,     // TrackElemType::rightEighthBankToOrthogonalDown25
        TrackElemType::leftEighthBankToOrthogonalDown25,      // TrackElemType::leftEighthBankToOrthogonalDown25
        TrackElemType::diagBrakes,
        TrackElemType::diagBlockBrakes,
        TrackElemType::down25Brakes,
        TrackElemType::diagBooster,
        TrackElemType::diagFlatToUp60LongBase,                // TrackElemType::diagFlatToUp60LongBase,
        TrackElemType::diagUp60ToFlatLongBase,                // TrackElemType::diagUp60ToFlatLongBase,
        TrackElemType::diagFlatToDown60LongBase,              // TrackElemType::diagFlatToDown60LongBase,
        TrackElemType::diagDown60ToFlatLongBase,              // TrackElemType::diagDown60ToFlatLongBase,
        TrackElemType::rightEighthDiveLoopUpToOrthogonal,     // TrackElemType::leftEighthDiveLoopUpToOrthogonal
        TrackElemType::leftEighthDiveLoopUpToOrthogonal,      // TrackElemType::rightEighthDiveLoopUpToOrthogonal
        TrackElemType::rightEighthDiveLoopDownToDiag,   // TrackElemType::leftEighthDiveLoopDownToDiag
        TrackElemType::leftEighthDiveLoopDownToDiag,    // TrackElemType::rightEighthDiveLoopDownToDiag
        TrackElemType::diagDown25Brakes,
    };
    static_assert(std::size(kTrackElementMirrorMap) == EnumValue(TrackElemType::count));

    /** rct2: 0x0099423C */
    static constexpr TrackFlags kTrackFlags[] = {
        /* TrackElemType::flat                                          */ { TrackElementFlag::allowLiftHill },
        /* TrackElemType::endStation                                    */ {},
        /* TrackElemType::beginStation                                  */ {},
        /* TrackElemType::middleStation                                 */ {},
        /* TrackElemType::up25                                          */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        /* TrackElemType::up60                                          */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::isSteepUp, TrackElementFlag::allowLiftHill },
        /* TrackElemType::flatToUp25                                    */ { TrackElementFlag::up, TrackElementFlag::allowLiftHill },
        /* TrackElemType::up25ToUp60                                    */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::isSteepUp, TrackElementFlag::allowLiftHill },
        /* TrackElemType::up60ToUp25                                    */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::isSteepUp, TrackElementFlag::allowLiftHill },
        /* TrackElemType::up25ToFlat                                    */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        /* TrackElemType::down25                                        */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        /* TrackElemType::down60                                        */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        /* TrackElemType::flatToDown25                                  */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        /* TrackElemType::down25ToDown60                                */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        /* TrackElemType::down60ToDown25                                */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        /* TrackElemType::down25ToFlat                                  */ { TrackElementFlag::down, TrackElementFlag::allowLiftHill },
        /* TrackElemType::leftQuarterTurn5Tiles                         */ { TrackElementFlag::turnLeft },
        /* TrackElemType::rightQuarterTurn5Tiles                        */ { TrackElementFlag::turnRight },
        /* TrackElemType::flatToLeftBank                                */ { TrackElementFlag::banked },
        /* TrackElemType::flatToRightBank                               */ { TrackElementFlag::banked },
        /* TrackElemType::leftBankToFlat                                */ { TrackElementFlag::banked },
        /* TrackElemType::rightBankToFlat                               */ { TrackElementFlag::banked },
        /* TrackElemType::bankedLeftQuarterTurn5Tiles                   */ { TrackElementFlag::turnLeft, TrackElementFlag::turnBanked, TrackElementFlag::banked },
        /* TrackElemType::bankedRightQuarterTurn5Tiles                  */ { TrackElementFlag::turnRight, TrackElementFlag::turnBanked, TrackElementFlag::banked },
        /* TrackElemType::leftBankToUp25                                */ { TrackElementFlag::up, TrackElementFlag::banked },
        /* TrackElemType::rightBankToUp25                               */ { TrackElementFlag::up, TrackElementFlag::banked },
        /* TrackElemType::up25ToLeftBank                                */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::up25ToRightBank                               */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::leftBankToDown25                              */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::rightBankToDown25                             */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::down25ToLeftBank                              */ { TrackElementFlag::down, TrackElementFlag::banked },
        /* TrackElemType::down25ToRightBank                             */ { TrackElementFlag::down, TrackElementFlag::banked },
        /* TrackElemType::leftBank                                      */ { TrackElementFlag::banked },
        /* TrackElemType::rightBank                                     */ { TrackElementFlag::banked },
        /* TrackElemType::leftQuarterTurn5TilesUp25                     */ { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        /* TrackElemType::rightQuarterTurn5TilesUp25                    */ { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        /* TrackElemType::leftQuarterTurn5TilesDown25                   */ { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        /* TrackElemType::rightQuarterTurn5TilesDown25                  */ { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        /* TrackElemType::sBendLeft                                     */ {},
        /* TrackElemType::sBendRight                                    */ {},
        /* TrackElemType::leftVerticalLoop                              */ { TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::inversionToNormal },
        /* TrackElemType::rightVerticalLoop                             */ { TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::inversionToNormal },
        /* TrackElemType::leftQuarterTurn3Tiles                         */ { TrackElementFlag::turnLeft },
        /* TrackElemType::rightQuarterTurn3Tiles                        */ { TrackElementFlag::turnRight },
        /* TrackElemType::leftBankedQuarterTurn3Tiles                   */ { TrackElementFlag::turnLeft, TrackElementFlag::turnBanked, TrackElementFlag::banked },
        /* TrackElemType::rightBankedQuarterTurn3Tiles                  */ { TrackElementFlag::turnRight, TrackElementFlag::turnBanked, TrackElementFlag::banked },
        /* TrackElemType::leftQuarterTurn3TilesUp25                     */ { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        /* TrackElemType::rightQuarterTurn3TilesUp25                    */ { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        /* TrackElemType::leftQuarterTurn3TilesDown25                   */ { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        /* TrackElemType::rightQuarterTurn3TilesDown25                  */ { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        /* TrackElemType::leftQuarterTurn1Tile                          */ { TrackElementFlag::turnLeft },
        /* TrackElemType::rightQuarterTurn1Tile                         */ { TrackElementFlag::turnRight },
        /* TrackElemType::leftTwistDownToUp                             */ { TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        /* TrackElemType::rightTwistDownToUp                            */ { TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        /* TrackElemType::leftTwistUpToDown                             */ { TrackElementFlag::inversionToNormal },
        /* TrackElemType::rightTwistUpToDown                            */ { TrackElementFlag::inversionToNormal },
        /* TrackElemType::halfLoopUp                                    */ { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::inversionToNormal },
        /* TrackElemType::halfLoopDown                                  */ { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        /* TrackElemType::leftCorkscrewUp                               */ { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        /* TrackElemType::rightCorkscrewUp                              */ { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        /* TrackElemType::leftCorkscrewDown                             */ { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        /* TrackElemType::rightCorkscrewDown                            */ { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        /* TrackElemType::flatToUp60                                    */ { TrackElementFlag::up, TrackElementFlag::isSteepUp, TrackElementFlag::allowLiftHill },
        /* TrackElemType::up60ToFlat                                    */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::isSteepUp, TrackElementFlag::allowLiftHill },
        /* TrackElemType::flatToDown60                                  */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        /* TrackElemType::down60ToFlat                                  */ { TrackElementFlag::down, TrackElementFlag::allowLiftHill },
        /* TrackElemType::towerBase                                     */ {},
        /* TrackElemType::towerSection                                  */ {},
        /* TrackElemType::flatCovered                                   */ {},
        /* TrackElemType::up25Covered                                   */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::up60Covered                                   */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::flatToUp25Covered                             */ { TrackElementFlag::up },
        /* TrackElemType::up25ToUp60Covered                             */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::up60ToUp25Covered                             */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::up25ToFlatCovered                             */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::down25Covered                                 */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::down60Covered                                 */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::flatToDown25Covered                           */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::down25ToDown60Covered                         */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::down60ToDown25Covered                         */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::down25ToFlatCovered                           */ { TrackElementFlag::down },
        /* TrackElemType::leftQuarterTurn5TilesCovered                  */ { TrackElementFlag::turnLeft },
        /* TrackElemType::rightQuarterTurn5TilesCovered                 */ { TrackElementFlag::turnRight },
        /* TrackElemType::sBendLeftCovered                              */ {},
        /* TrackElemType::sBendRightCovered                             */ {},
        /* TrackElemType::leftQuarterTurn3TilesCovered                  */ { TrackElementFlag::turnLeft },
        /* TrackElemType::rightQuarterTurn3TilesCovered                 */ { TrackElementFlag::turnRight },
        /* TrackElemType::leftHalfBankedHelixUpSmall                    */ { TrackElementFlag::turnLeft, TrackElementFlag::helix, TrackElementFlag::banked },
        /* TrackElemType::rightHalfBankedHelixUpSmall                   */ { TrackElementFlag::turnRight, TrackElementFlag::helix, TrackElementFlag::banked },
        /* TrackElemType::leftHalfBankedHelixDownSmall                  */ { TrackElementFlag::turnLeft, TrackElementFlag::helix, TrackElementFlag::banked },
        /* TrackElemType::rightHalfBankedHelixDownSmall                 */ { TrackElementFlag::turnRight, TrackElementFlag::helix, TrackElementFlag::banked },
        /* TrackElemType::leftHalfBankedHelixUpLarge                    */ { TrackElementFlag::turnLeft, TrackElementFlag::helix, TrackElementFlag::banked },
        /* TrackElemType::rightHalfBankedHelixUpLarge                   */ { TrackElementFlag::turnRight, TrackElementFlag::helix, TrackElementFlag::banked },
        /* TrackElemType::leftHalfBankedHelixDownLarge                  */ { TrackElementFlag::turnLeft, TrackElementFlag::helix, TrackElementFlag::banked },
        /* TrackElemType::rightHalfBankedHelixDownLarge                 */ { TrackElementFlag::turnRight, TrackElementFlag::helix, TrackElementFlag::banked },
        /* TrackElemType::leftQuarterTurn1TileUp60                      */ { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::rightQuarterTurn1TileUp60                     */ { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::leftQuarterTurn1TileDown60                    */ { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::rightQuarterTurn1TileDown60                   */ { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::brakes                                        */ {},
        /* TrackElemType::booster                                       */ {},
        /* TrackElemType::invertedUp90ToFlatQuarterLoopAlias / Maze     */ { TrackElementFlag::onlyAboveGround },
        /* TrackElemType::leftQuarterBankedHelixLargeUp                 */ { TrackElementFlag::turnLeft, TrackElementFlag::helix, TrackElementFlag::banked },
        /* TrackElemType::rightQuarterBankedHelixLargeUp                */ { TrackElementFlag::turnRight, TrackElementFlag::helix, TrackElementFlag::banked },
        /* TrackElemType::leftQuarterBankedHelixLargeDown               */ { TrackElementFlag::turnLeft, TrackElementFlag::helix, TrackElementFlag::banked },
        /* TrackElemType::rightQuarterBankedHelixLargeDown              */ { TrackElementFlag::turnRight, TrackElementFlag::helix, TrackElementFlag::banked },
        /* TrackElemType::leftQuarterHelixLargeUp                       */ { TrackElementFlag::turnLeft, TrackElementFlag::helix },
        /* TrackElemType::rightQuarterHelixLargeUp                      */ { TrackElementFlag::turnRight, TrackElementFlag::helix },
        /* TrackElemType::leftQuarterHelixLargeDown                     */ { TrackElementFlag::turnLeft, TrackElementFlag::helix },
        /* TrackElemType::rightQuarterHelixLargeDown                    */ { TrackElementFlag::turnRight, TrackElementFlag::helix },
        /* TrackElemType::up25LeftBanked                                */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::up25RightBanked                               */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::waterfall                                     */ {},
        /* TrackElemType::rapids                                        */ {},
        /* TrackElemType::onRidePhoto                                   */ {},
        /* TrackElemType::down25LeftBanked                              */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::down25RightBanked                             */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::watersplash                                   */ {},
        /* TrackElemType::flatToUp60LongBase                            */ { TrackElementFlag::up },
        /* TrackElemType::up60ToFlatLongBase                            */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::whirlpool                                     */ {},
        /* TrackElemType::down60ToFlatLongBase                          */ { TrackElementFlag::down },
        /* TrackElemType::flatToDown60LongBase                          */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::cableLiftHill                                 */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::reverseFreefallSlope                          */ { TrackElementFlag::up },
        /* TrackElemType::reverseFreefallVertical                       */ { TrackElementFlag::up },
        /* TrackElemType::up90                                          */ { TrackElementFlag::up },
        /* TrackElemType::down90                                        */ { TrackElementFlag::down, TrackElementFlag::allowLiftHill },
        /* TrackElemType::up60ToUp90                                    */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::down90ToDown60                                */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        /* TrackElemType::up90ToUp60                                    */ { TrackElementFlag::up },
        /* TrackElemType::down60ToDown90                                */ { TrackElementFlag::down },
        /* TrackElemType::brakeForDrop                                  */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::leftEighthToDiag                              */ { TrackElementFlag::turnLeft },
        /* TrackElemType::rightEighthToDiag                             */ { TrackElementFlag::turnRight },
        /* TrackElemType::leftEighthToOrthogonal                        */ { TrackElementFlag::turnLeft },
        /* TrackElemType::rightEighthToOrthogonal                       */ { TrackElementFlag::turnRight },
        /* TrackElemType::leftEighthBankToDiag                          */ { TrackElementFlag::turnLeft, TrackElementFlag::turnBanked, TrackElementFlag::banked },
        /* TrackElemType::rightEighthBankToDiag                         */ { TrackElementFlag::turnRight, TrackElementFlag::turnBanked, TrackElementFlag::banked },
        /* TrackElemType::leftEighthBankToOrthogonal                    */ { TrackElementFlag::turnLeft, TrackElementFlag::turnBanked, TrackElementFlag::banked },
        /* TrackElemType::rightEighthBankToOrthogonal                   */ { TrackElementFlag::turnRight, TrackElementFlag::turnBanked, TrackElementFlag::banked },
        /* TrackElemType::diagFlat                                      */ { TrackElementFlag::allowLiftHill },
        /* TrackElemType::diagUp25                                      */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        /* TrackElemType::diagUp60                                      */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::isSteepUp, TrackElementFlag::allowLiftHill },
        /* TrackElemType::diagFlatToUp25                                */ { TrackElementFlag::up, TrackElementFlag::allowLiftHill },
        /* TrackElemType::diagUp25ToUp60                                */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::isSteepUp, TrackElementFlag::allowLiftHill },
        /* TrackElemType::diagUp60ToUp25                                */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::isSteepUp, TrackElementFlag::allowLiftHill },
        /* TrackElemType::diagUp25ToFlat                                */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        /* TrackElemType::diagDown25                                    */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        /* TrackElemType::diagDown60                                    */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        /* TrackElemType::diagFlatToDown25                              */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        /* TrackElemType::diagDown25ToDown60                            */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        /* TrackElemType::diagDown60ToDown25                            */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        /* TrackElemType::diagDown25ToFlat                              */ { TrackElementFlag::down, TrackElementFlag::allowLiftHill },
        /* TrackElemType::diagFlatToUp60                                */ { TrackElementFlag::up, TrackElementFlag::isSteepUp, TrackElementFlag::allowLiftHill },
        /* TrackElemType::diagUp60ToFlat                                */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::isSteepUp, TrackElementFlag::allowLiftHill },
        /* TrackElemType::diagFlatToDown60                              */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        /* TrackElemType::diagDown60ToFlat                              */ { TrackElementFlag::down, TrackElementFlag::allowLiftHill },
        /* TrackElemType::diagFlatToLeftBank                            */ { TrackElementFlag::banked },
        /* TrackElemType::diagFlatToRightBank                           */ { TrackElementFlag::banked },
        /* TrackElemType::diagLeftBankToFlat                            */ { TrackElementFlag::banked },
        /* TrackElemType::diagRightBankToFlat                           */ { TrackElementFlag::banked },
        /* TrackElemType::diagLeftBankToUp25                            */ { TrackElementFlag::up, TrackElementFlag::banked },
        /* TrackElemType::diagRightBankToUp25                           */ { TrackElementFlag::up, TrackElementFlag::banked },
        /* TrackElemType::diagUp25ToLeftBank                            */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::diagUp25ToRightBank                           */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::diagLeftBankToDown25                          */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::diagRightBankToDown25                         */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::diagDown25ToLeftBank                          */ { TrackElementFlag::down },
        /* TrackElemType::diagDown25ToRightBank                         */ { TrackElementFlag::down },
        /* TrackElemType::diagLeftBank                                  */ { TrackElementFlag::banked },
        /* TrackElemType::diagRightBank                                 */ { TrackElementFlag::banked },
        /* TrackElemType::logFlumeReverser                              */ { TrackElementFlag::banked },
        /* TrackElemType::spinningTunnel                                */ { TrackElementFlag::banked },
        /* TrackElemType::leftBarrelRollUpToDown                        */ { TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        /* TrackElemType::rightBarrelRollUpToDown                       */ { TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        /* TrackElemType::leftBarrelRollDownToUp                        */ { TrackElementFlag::inversionToNormal },
        /* TrackElemType::rightBarrelRollDownToUp                       */ { TrackElementFlag::inversionToNormal },
        /* TrackElemType::leftBankToLeftQuarterTurn3TilesUp25           */ { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::up, TrackElementFlag::banked },
        /* TrackElemType::rightBankToRightQuarterTurn3TilesUp25         */ { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::up, TrackElementFlag::banked },
        /* TrackElemType::leftQuarterTurn3TilesDown25ToLeftBank         */ { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::down, TrackElementFlag::banked },
        /* TrackElemType::rightQuarterTurn3TilesDown25ToRightBank       */ { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::down, TrackElementFlag::banked },
        /* TrackElemType::poweredLift                                   */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::leftLargeHalfLoopUp                           */ { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::inversionToNormal },
        /* TrackElemType::rightLargeHalfLoopUp                          */ { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::inversionToNormal },
        /* TrackElemType::leftLargeHalfLoopDown                         */ { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        /* TrackElemType::rightLargeHalfLoopDown                        */ { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        /* TrackElemType::leftFlyerTwistUp                              */ { TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        /* TrackElemType::rightFlyerTwistUp                             */ { TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        /* TrackElemType::leftFlyerTwistDown                            */ { TrackElementFlag::inversionToNormal },
        /* TrackElemType::rightFlyerTwistDown                           */ { TrackElementFlag::inversionToNormal },
        /* TrackElemType::flyerHalfLoopUninvertedUp                     */ { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::inversionToNormal },
        /* TrackElemType::flyerHalfLoopInvertedDown                     */ { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        /* TrackElemType::leftFlyerCorkscrewUp                          */ { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        /* TrackElemType::rightFlyerCorkscrewUp                         */ { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        /* TrackElemType::leftFlyerCorkscrewDown                        */ { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        /* TrackElemType::rightFlyerCorkscrewDown                       */ { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        /* TrackElemType::heartLineTransferUp                           */ {},
        /* TrackElemType::heartLineTransferDown                         */ {},
        /* TrackElemType::leftHeartLineRoll                             */ { TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal, TrackElementFlag::canBePartlyUnderground },
        /* TrackElemType::rightHeartLineRoll                            */ { TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal, TrackElementFlag::canBePartlyUnderground },
        /* TrackElemType::minigolfHoleA                                 */ { TrackElementFlag::isGolfHole },
        /* TrackElemType::minigolfHoleB                                 */ { TrackElementFlag::isGolfHole },
        /* TrackElemType::minigolfHoleC                                 */ { TrackElementFlag::isGolfHole },
        /* TrackElemType::minigolfHoleD                                 */ { TrackElementFlag::isGolfHole },
        /* TrackElemType::minigolfHoleE                                 */ { TrackElementFlag::isGolfHole },
        /* TrackElemType::multiDimInvertedFlatToDown90QuarterLoop       */ { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        /* TrackElemType::up90ToInvertedFlatQuarterLoop                 */ { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        /* TrackElemType::invertedFlatToDown90QuarterLoop               */ { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        /* TrackElemType::leftCurvedLiftHill                            */ { TrackElementFlag::turnLeft, TrackElementFlag::up },
        /* TrackElemType::rightCurvedLiftHill                           */ { TrackElementFlag::turnRight, TrackElementFlag::up },
        /* TrackElemType::leftReverser                                  */ {},
        /* TrackElemType::rightReverser                                 */ {},
        /* TrackElemType::airThrustTopCap                               */ {},
        /* TrackElemType::airThrustVerticalDown                         */ { TrackElementFlag::down },
        /* TrackElemType::airThrustVerticalDownToLevel                  */ { TrackElementFlag::down },
        /* TrackElemType::blockBrakes                                   */ {},
        /* TrackElemType::leftBankedQuarterTurn3TileUp25                */ { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::rightBankedQuarterTurn3TileUp25               */ { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::leftBankedQuarterTurn3TileDown25              */ { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::rightBankedQuarterTurn3TileDown25             */ { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::leftBankedQuarterTurn5TileUp25                */ { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::rightBankedQuarterTurn5TileUp25               */ { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::leftBankedQuarterTurn5TileDown25              */ { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::rightBankedQuarterTurn5TileDown25             */ { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::up25ToLeftBankedUp25                          */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::up25ToRightBankedUp25                         */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::leftBankedUp25ToUp25                          */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::rightBankedUp25ToUp25                         */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::down25ToLeftBankedDown25                      */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::down25ToRightBankedDown25                     */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::leftBankedDown25ToDown25                      */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::rightBankedDown25ToDown25                     */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::leftBankedFlatToLeftBankedUp25                */ { TrackElementFlag::up, TrackElementFlag::banked },
        /* TrackElemType::rightBankedFlatToRightBankedUp25              */ { TrackElementFlag::up, TrackElementFlag::banked },
        /* TrackElemType::leftBankedUp25ToLeftBankedFlat                */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::rightBankedUp25ToRightBankedFlat              */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::leftBankedFlatToLeftBankedDown25              */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::rightBankedFlatToRightBankedDown25            */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::leftBankedDown25ToLeftBankedFlat              */ { TrackElementFlag::down, TrackElementFlag::banked },
        /* TrackElemType::rightBankedDown25ToRightBankedFlat            */ { TrackElementFlag::down, TrackElementFlag::banked },
        /* TrackElemType::flatToLeftBankedUp25                          */ { TrackElementFlag::up, TrackElementFlag::banked },
        /* TrackElemType::flatToRightBankedUp25                         */ { TrackElementFlag::up, TrackElementFlag::banked },
        /* TrackElemType::leftBankedUp25ToFlat                          */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::rightBankedUp25ToFlat                         */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::flatToLeftBankedDown25                        */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::flatToRightBankedDown25                       */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        /* TrackElemType::leftBankedDown25ToFlat                        */ { TrackElementFlag::down, TrackElementFlag::banked },
        /* TrackElemType::rightBankedDown25ToFlat                       */ { TrackElementFlag::down, TrackElementFlag::banked },
        /* TrackElemType::leftQuarterTurn1TileUp90                      */ { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::up },
        /* TrackElemType::rightQuarterTurn1TileUp90                     */ { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::up },
        /* TrackElemType::leftQuarterTurn1TileDown90                    */ { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::down },
        /* TrackElemType::rightQuarterTurn1TileDown90                   */ { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::down },
        /* TrackElemType::multiDimUp90ToInvertedFlatQuarterLoop         */ { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        /* TrackElemType::multiDimFlatToDown90QuarterLoop               */ { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        /* TrackElemType::multiDimInvertedUp90ToFlatQuarterLoop         */ { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        /* TrackElemType::rotationControlToggle                         */ {},
        /* TrackElemType::flatTrack1x4A                                 */ {},
        /* TrackElemType::flatTrack2x2                                  */ {},
        /* TrackElemType::flatTrack4x4                                  */ {},
        /* TrackElemType::flatTrack2x4                                  */ { TrackElementFlag::onlyAboveGround },
        /* TrackElemType::flatTrack1x5                                  */ {},
        /* TrackElemType::flatTrack1x1A                                 */ {},
        /* TrackElemType::flatTrack1x4B                                 */ {},
        /* TrackElemType::flatTrack1x1B                                 */ {},
        /* TrackElemType::flatTrack1x4C                                 */ {},
        /* TrackElemType::flatTrack3x3                                  */ {},
        /* TrackElemType::leftLargeCorkscrewUp                          */ { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        /* TrackElemType::rightLargeCorkscrewUp                         */ { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        /* TrackElemType::leftLargeCorkscrewDown                        */ { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        /* TrackElemType::rightLargeCorkscrewDown                       */ { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        /* TrackElemType::leftMediumHalfLoopUp                          */ { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::inversionToNormal },
        /* TrackElemType::rightMediumHalfLoopUp                         */ { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::inversionToNormal },
        /* TrackElemType::leftMediumHalfLoopDown                        */ { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        /* TrackElemType::rightMediumHalfLoopDown                       */ { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        /* TrackElemType::leftZeroGRollUp                               */ { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::inversionToNormal },
        /* TrackElemType::rightZeroGRollUp                              */ { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::inversionToNormal },
        /* TrackElemType::leftZeroGRollDown                             */ { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        /* TrackElemType::rightZeroGRollDown                            */ { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        /* TrackElemType::leftLargeZeroGRollUp                          */ { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::inversionToNormal },
        /* TrackElemType::rightLargeZeroGRollUp                         */ { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::inversionToNormal },
        /* TrackElemType::leftLargeZeroGRollDown                        */ { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        /* TrackElemType::rightLargeZeroGRollDown                       */ { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        /* TrackElemType::leftFlyerLargeHalfLoopUninvertedUp            */ { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::rightFlyerLargeHalfLoopUninvertedUp           */ { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::leftFlyerLargeHalfLoopInvertedDown            */ { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        /* TrackElemType::rightFlyerLargeHalfLoopInvertedDown           */ { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        /* TrackElemType::leftFlyerLargeHalfLoopInvertedUp              */ { TrackElementFlag::up, TrackElementFlag::inversionToNormal, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::rightFlyerLargeHalfLoopInvertedUp             */ { TrackElementFlag::up, TrackElementFlag::inversionToNormal, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::leftFlyerLargeHalfLoopUninvertedDown          */ { TrackElementFlag::down, TrackElementFlag::normalToInversion },
        /* TrackElemType::rightFlyerLargeHalfLoopUninvertedDown         */ { TrackElementFlag::down, TrackElementFlag::normalToInversion },
        /* TrackElemType::flyerHalfLoopInvertedUp                       */ { TrackElementFlag::up, TrackElementFlag::inversionToNormal, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::flyerHalfLoopUnivertedDown                    */ { TrackElementFlag::down, TrackElementFlag::normalToInversion },
        /* TrackElemType::leftEighthToDiagUp25                          */ { TrackElementFlag::turnLeft , TrackElementFlag::turnSloped, TrackElementFlag::up  , TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        /* TrackElemType::rightEighthToDiagUp25                         */ { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::up  , TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        /* TrackElemType::leftEighthToDiagDown25                        */ { TrackElementFlag::turnLeft , TrackElementFlag::turnSloped, TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        /* TrackElemType::rightEighthToDiagDown25                       */ { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        /* TrackElemType::leftEighthToOrthogonalUp25                    */ { TrackElementFlag::turnLeft , TrackElementFlag::turnSloped, TrackElementFlag::up  , TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        /* TrackElemType::rightEighthToOrthogonalUp25                   */ { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::up  , TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        /* TrackElemType::leftEighthToOrthogonalDown25                  */ { TrackElementFlag::turnLeft , TrackElementFlag::turnSloped, TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        /* TrackElemType::rightEighthToOrthogonalDown25                 */ { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        /* TrackElemType::diagUp25ToLeftBankedUp25                      */ { TrackElementFlag::up, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::diagUp25ToRightBankedUp25                     */ { TrackElementFlag::up, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::diagLeftBankedUp25ToUp25                      */ { TrackElementFlag::up, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::diagRightBankedUp25ToUp25                     */ { TrackElementFlag::up, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::diagDown25ToLeftBankedDown25                  */ { TrackElementFlag::down, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::diagDown25ToRightBankedDown25                 */ { TrackElementFlag::down, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::diagLeftBankedDown25ToDown25                  */ { TrackElementFlag::down, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::diagRightBankedDown25ToDown25                 */ { TrackElementFlag::down, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::diagLeftBankedFlatToLeftBankedUp25            */ { TrackElementFlag::up, TrackElementFlag::banked },
        /* TrackElemType::diagRightBankedFlatToRightBankedUp25          */ { TrackElementFlag::up, TrackElementFlag::banked },
        /* TrackElemType::diagLeftBankedUp25ToLeftBankedFlat            */ { TrackElementFlag::up, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::diagRightBankedUp25ToRightBankedFlat          */ { TrackElementFlag::up, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::diagLeftBankedFlatToLeftBankedDown25          */ { TrackElementFlag::down, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::diagRightBankedFlatToRightBankedDown25        */ { TrackElementFlag::down, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::diagLeftBankedDown25ToLeftBankedFlat          */ { TrackElementFlag::down, TrackElementFlag::banked },
        /* TrackElemType::diagRightBankedDown25ToRightBankedFlat        */ { TrackElementFlag::down, TrackElementFlag::banked },
        /* TrackElemType::diagFlatToLeftBankedUp25                      */ { TrackElementFlag::up, TrackElementFlag::banked },
        /* TrackElemType::diagFlatToRightBankedUp25                     */ { TrackElementFlag::up, TrackElementFlag::banked },
        /* TrackElemType::diagLeftBankedUp25ToFlat                      */ { TrackElementFlag::up, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::diagRightBankedUp25ToFlat                     */ { TrackElementFlag::up, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::diagFlatToLeftBankedDown25                    */ { TrackElementFlag::down, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::diagFlatToRightBankedDown25                   */ { TrackElementFlag::down, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::diagLeftBankedDown25ToFlat                    */ { TrackElementFlag::down, TrackElementFlag::banked },
        /* TrackElemType::diagRightBankedDown25ToFlat                   */ { TrackElementFlag::down, TrackElementFlag::banked },
        /* TrackElemType::up25LeftBanked                                */ { TrackElementFlag::up, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::up25RightBanked                               */ { TrackElementFlag::up, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::down25LeftBanked                              */ { TrackElementFlag::down, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::down25RightBanked                             */ { TrackElementFlag::down, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::leftEighthBankToDiagUp25                      */ { TrackElementFlag::turnLeft , TrackElementFlag::turnSloped, TrackElementFlag::up  , TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        /* TrackElemType::rightEighthBankToDiagUp25                     */ { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::up  , TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        /* TrackElemType::leftEighthBankToDiagDown25                    */ { TrackElementFlag::turnLeft , TrackElementFlag::turnSloped, TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        /* TrackElemType::rightEighthBankToDiagDown25                   */ { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        /* TrackElemType::leftEighthBankToOrthogonalUp25                */ { TrackElementFlag::turnLeft , TrackElementFlag::turnSloped, TrackElementFlag::up  , TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        /* TrackElemType::rightEighthBankToOrthogonalUp25               */ { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::up  , TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        /* TrackElemType::leftEighthBankToOrthogonalDown25              */ { TrackElementFlag::turnLeft , TrackElementFlag::turnSloped, TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        /* TrackElemType::rightEighthBankToOrthogonalDown25             */ { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        /* TrackElemType::diagBrakes                                    */ {},
        /* TrackElemType::diagBlockBrakes                               */ {},
        /* TrackElemType::down25Brakes                                  */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::diagBooster                                   */ {},
        /* TrackElemType::diagFlatToUp60LongBase                        */ { TrackElementFlag::up, TrackElementFlag::isSteepUp },
        /* TrackElemType::diagUp60ToFlatLongBase                        */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::isSteepUp },
        /* TrackElemType::diagFlatToDown60LongBase                      */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
        /* TrackElemType::diagDown60ToFlatLongBase                      */ { TrackElementFlag::down },
        /* TrackElemType::leftEighthDiveLoopUpToOrthogonal              */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        /* TrackElemType::rightEighthDiveLoopUpToOrthogonal             */ { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        /* TrackElemType::leftEighthDiveLoopDownToDiag                  */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::inversionToNormal },
        /* TrackElemType::rightEighthDiveLoopDownToDiag                 */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::inversionToNormal },
        /* TrackElemType::diagDown25Brakes                              */ { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
    };
    static_assert(std::size(kTrackFlags) == EnumValue(TrackElemType::count));
    // clang-format on

    /**  rct2: 0x00997C9D */
    // clang-format off
    static constexpr TrackDefinition kTrackDefinitions[] =
    {
        // TYPE                                          VANGLE END                    VANGLE START                BANK END                BANK START              PREVIEW Z OFFSET
        { TrackGroup::straight,                          TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::flat
        { TrackGroup::stationEnd,                        TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::endStation
        { TrackGroup::stationEnd,                        TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::beginStation
        { TrackGroup::stationEnd,                        TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::middleStation
        { TrackGroup::slope,                             TrackPitch::up25,             TrackPitch::up25,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::up25
        { TrackGroup::slopeSteepUp,                      TrackPitch::up60,             TrackPitch::up60,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::up60
        { TrackGroup::slope,                             TrackPitch::up25,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::flatToUp25
        { TrackGroup::slopeSteepUp,                      TrackPitch::up60,             TrackPitch::up25,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::up25ToUp60
        { TrackGroup::slopeSteepUp,                      TrackPitch::up25,             TrackPitch::up60,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::up60ToUp25
        { TrackGroup::slope,                             TrackPitch::none,             TrackPitch::up25,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::up25ToFlat
        { TrackGroup::slope,                             TrackPitch::down25,           TrackPitch::down25,         TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::down25
        { TrackGroup::slopeSteepDown,                    TrackPitch::down60,           TrackPitch::down60,         TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::down60
        { TrackGroup::slope,                             TrackPitch::down25,           TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::flatToDown25
        { TrackGroup::slopeSteepDown,                    TrackPitch::down60,           TrackPitch::down25,         TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::down25ToDown60
        { TrackGroup::slopeSteepDown,                    TrackPitch::down25,           TrackPitch::down60,         TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::down60ToDown25
        { TrackGroup::slope,                             TrackPitch::none,             TrackPitch::down25,         TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::down25ToFlat
        { TrackGroup::curve,                             TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::leftQuarterTurn5Tiles
        { TrackGroup::curve,                             TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::rightQuarterTurn5Tiles
        { TrackGroup::flatRollBanking,                   TrackPitch::none,             TrackPitch::none,           TrackRoll::left,        TrackRoll::none,        0   }, // TrackElemType::flatToLeftBank
        { TrackGroup::flatRollBanking,                   TrackPitch::none,             TrackPitch::none,           TrackRoll::right,       TrackRoll::none,        0   }, // TrackElemType::flatToRightBank
        { TrackGroup::flatRollBanking,                   TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::left,        0   }, // TrackElemType::leftBankToFlat
        { TrackGroup::flatRollBanking,                   TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::right,       0   }, // TrackElemType::rightBankToFlat
        { TrackGroup::flatRollBanking,                   TrackPitch::none,             TrackPitch::none,           TrackRoll::left,        TrackRoll::left,        0   }, // TrackElemType::bankedLeftQuarterTurn5Tiles
        { TrackGroup::flatRollBanking,                   TrackPitch::none,             TrackPitch::none,           TrackRoll::right,       TrackRoll::right,       0   }, // TrackElemType::bankedRightQuarterTurn5Tiles
        { TrackGroup::flatRollBanking,                   TrackPitch::up25,             TrackPitch::none,           TrackRoll::none,        TrackRoll::left,        0   }, // TrackElemType::leftBankToUp25
        { TrackGroup::flatRollBanking,                   TrackPitch::up25,             TrackPitch::none,           TrackRoll::none,        TrackRoll::right,       0   }, // TrackElemType::rightBankToUp25
        { TrackGroup::flatRollBanking,                   TrackPitch::none,             TrackPitch::up25,           TrackRoll::left,        TrackRoll::none,        0   }, // TrackElemType::up25ToLeftBank
        { TrackGroup::flatRollBanking,                   TrackPitch::none,             TrackPitch::up25,           TrackRoll::right,       TrackRoll::none,        0   }, // TrackElemType::up25ToRightBank
        { TrackGroup::flatRollBanking,                   TrackPitch::down25,           TrackPitch::none,           TrackRoll::none,        TrackRoll::left,        0   }, // TrackElemType::leftBankToDown25
        { TrackGroup::flatRollBanking,                   TrackPitch::down25,           TrackPitch::none,           TrackRoll::none,        TrackRoll::right,       0   }, // TrackElemType::rightBankToDown25
        { TrackGroup::flatRollBanking,                   TrackPitch::none,             TrackPitch::down25,         TrackRoll::left,        TrackRoll::none,        0   }, // TrackElemType::down25ToLeftBank
        { TrackGroup::flatRollBanking,                   TrackPitch::none,             TrackPitch::down25,         TrackRoll::right,       TrackRoll::none,        0   }, // TrackElemType::down25ToRightBank
        { TrackGroup::flatRollBanking,                   TrackPitch::none,             TrackPitch::none,           TrackRoll::left,        TrackRoll::left,        0   }, // TrackElemType::leftBank
        { TrackGroup::flatRollBanking,                   TrackPitch::none,             TrackPitch::none,           TrackRoll::right,       TrackRoll::right,       0   }, // TrackElemType::rightBank
        { TrackGroup::slopeCurve,                        TrackPitch::up25,             TrackPitch::up25,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::leftQuarterTurn5TilesUp25
        { TrackGroup::slopeCurve,                        TrackPitch::up25,             TrackPitch::up25,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::rightQuarterTurn5TilesUp25
        { TrackGroup::slopeCurve,                        TrackPitch::down25,           TrackPitch::down25,         TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::leftQuarterTurn5TilesDown25
        { TrackGroup::slopeCurve,                        TrackPitch::down25,           TrackPitch::down25,         TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::rightQuarterTurn5TilesDown25
        { TrackGroup::sBend,                             TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::sBendLeft
        { TrackGroup::sBend,                             TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::sBendRight
        { TrackGroup::verticalLoop,                      TrackPitch::down25,           TrackPitch::up25,           TrackRoll::none,        TrackRoll::none,        -48 }, // TrackElemType::leftVerticalLoop
        { TrackGroup::verticalLoop,                      TrackPitch::down25,           TrackPitch::up25,           TrackRoll::none,        TrackRoll::none,        -48 }, // TrackElemType::rightVerticalLoop
        { TrackGroup::curveSmall,                        TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::leftQuarterTurn3Tiles
        { TrackGroup::curveSmall,                        TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::rightQuarterTurn3Tiles
        { TrackGroup::flatRollBanking,                   TrackPitch::none,             TrackPitch::none,           TrackRoll::left,        TrackRoll::left,        0   }, // TrackElemType::leftBankedQuarterTurn3Tiles
        { TrackGroup::flatRollBanking,                   TrackPitch::none,             TrackPitch::none,           TrackRoll::right,       TrackRoll::right,       0   }, // TrackElemType::rightBankedQuarterTurn3Tiles
        { TrackGroup::slopeCurve,                        TrackPitch::up25,             TrackPitch::up25,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::leftQuarterTurn3TilesUp25
        { TrackGroup::slopeCurve,                        TrackPitch::up25,             TrackPitch::up25,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::rightQuarterTurn3TilesUp25
        { TrackGroup::slopeCurve,                        TrackPitch::down25,           TrackPitch::down25,         TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::leftQuarterTurn3TilesDown25
        { TrackGroup::slopeCurve,                        TrackPitch::down25,           TrackPitch::down25,         TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::rightQuarterTurn3TilesDown25
        { TrackGroup::curveVerySmall,                    TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::leftQuarterTurn1Tile
        { TrackGroup::curveVerySmall,                    TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::rightQuarterTurn1Tile
        { TrackGroup::twist,                             TrackPitch::none,             TrackPitch::none,           TrackRoll::upsideDown,  TrackRoll::none,        0   }, // TrackElemType::leftTwistDownToUp
        { TrackGroup::twist,                             TrackPitch::none,             TrackPitch::none,           TrackRoll::upsideDown,  TrackRoll::none,        0   }, // TrackElemType::rightTwistDownToUp
        { TrackGroup::twist,                             TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::upsideDown,  0   }, // TrackElemType::leftTwistUpToDown
        { TrackGroup::twist,                             TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::upsideDown,  0   }, // TrackElemType::rightTwistUpToDown
        { TrackGroup::halfLoop,                          TrackPitch::none,             TrackPitch::up25,           TrackRoll::upsideDown,  TrackRoll::none,        64  }, // TrackElemType::halfLoopUp
        { TrackGroup::halfLoop,                          TrackPitch::down25,           TrackPitch::none,           TrackRoll::none,        TrackRoll::upsideDown,  -64 }, // TrackElemType::halfLoopDown
        { TrackGroup::corkscrew,                         TrackPitch::none,             TrackPitch::none,           TrackRoll::upsideDown,  TrackRoll::none,        0   }, // TrackElemType::leftCorkscrewUp
        { TrackGroup::corkscrew,                         TrackPitch::none,             TrackPitch::none,           TrackRoll::upsideDown,  TrackRoll::none,        0   }, // TrackElemType::rightCorkscrewUp
        { TrackGroup::corkscrew,                         TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::upsideDown,  -32 }, // TrackElemType::leftCorkscrewDown
        { TrackGroup::corkscrew,                         TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::upsideDown,  -32 }, // TrackElemType::rightCorkscrewDown
        { TrackGroup::flatToSteepSlope,                  TrackPitch::up60,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::flatToUp60
        { TrackGroup::flatToSteepSlope,                  TrackPitch::none,             TrackPitch::up60,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::up60ToFlat
        { TrackGroup::flatToSteepSlope,                  TrackPitch::down60,           TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::flatToDown60
        { TrackGroup::flatToSteepSlope,                  TrackPitch::none,             TrackPitch::down60,         TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::down60ToFlat
        { TrackGroup::tower,                             TrackPitch::tower,            TrackPitch::tower,          TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::towerBase
        { TrackGroup::tower,                             TrackPitch::tower,            TrackPitch::tower,          TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::towerSection
        { TrackGroup::flat,                              TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::flatCovered
        { TrackGroup::flat,                              TrackPitch::up25,             TrackPitch::up25,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::up25Covered
        { TrackGroup::flat,                              TrackPitch::up60,             TrackPitch::up60,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::up60Covered
        { TrackGroup::flat,                              TrackPitch::up25,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::flatToUp25Covered
        { TrackGroup::flat,                              TrackPitch::up60,             TrackPitch::up25,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::up25ToUp60Covered
        { TrackGroup::flat,                              TrackPitch::up25,             TrackPitch::up60,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::up60ToUp25Covered
        { TrackGroup::flat,                              TrackPitch::none,             TrackPitch::up25,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::up25ToFlatCovered
        { TrackGroup::flat,                              TrackPitch::down25,           TrackPitch::down25,         TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::down25Covered
        { TrackGroup::flat,                              TrackPitch::down60,           TrackPitch::down60,         TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::down60Covered
        { TrackGroup::flat,                              TrackPitch::down25,           TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::flatToDown25Covered
        { TrackGroup::flat,                              TrackPitch::down60,           TrackPitch::down25,         TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::down25ToDown60Covered
        { TrackGroup::flat,                              TrackPitch::down25,           TrackPitch::down60,         TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::down60ToDown25Covered
        { TrackGroup::flat,                              TrackPitch::none,             TrackPitch::down25,         TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::down25ToFlatCovered
        { TrackGroup::flat,                              TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::leftQuarterTurn5TilesCovered
        { TrackGroup::flat,                              TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::rightQuarterTurn5TilesCovered
        { TrackGroup::flat,                              TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::sBendLeftCovered
        { TrackGroup::flat,                              TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::sBendRightCovered
        { TrackGroup::flat,                              TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::leftQuarterTurn3TilesCovered
        { TrackGroup::flat,                              TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::rightQuarterTurn3TilesCovered
        { TrackGroup::helixUpBankedHalf,                 TrackPitch::none,             TrackPitch::none,           TrackRoll::left,        TrackRoll::left,        0   }, // TrackElemType::leftHalfBankedHelixUpSmall
        { TrackGroup::helixUpBankedHalf,                 TrackPitch::none,             TrackPitch::none,           TrackRoll::right,       TrackRoll::right,       0   }, // TrackElemType::rightHalfBankedHelixUpSmall
        { TrackGroup::helixDownBankedHalf,               TrackPitch::none,             TrackPitch::none,           TrackRoll::left,        TrackRoll::left,        0   }, // TrackElemType::leftHalfBankedHelixDownSmall
        { TrackGroup::helixDownBankedHalf,               TrackPitch::none,             TrackPitch::none,           TrackRoll::right,       TrackRoll::right,       0   }, // TrackElemType::rightHalfBankedHelixDownSmall
        { TrackGroup::helixUpBankedHalf,                 TrackPitch::none,             TrackPitch::none,           TrackRoll::left,        TrackRoll::left,        0   }, // TrackElemType::leftHalfBankedHelixUpLarge
        { TrackGroup::helixUpBankedHalf,                 TrackPitch::none,             TrackPitch::none,           TrackRoll::right,       TrackRoll::right,       0   }, // TrackElemType::rightHalfBankedHelixUpLarge
        { TrackGroup::helixDownBankedHalf,               TrackPitch::none,             TrackPitch::none,           TrackRoll::left,        TrackRoll::left,        0   }, // TrackElemType::leftHalfBankedHelixDownLarge
        { TrackGroup::helixDownBankedHalf,               TrackPitch::none,             TrackPitch::none,           TrackRoll::right,       TrackRoll::right,       0   }, // TrackElemType::rightHalfBankedHelixDownLarge
        { TrackGroup::slopeCurveSteep,                   TrackPitch::up60,             TrackPitch::up60,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::leftQuarterTurn1TileUp60
        { TrackGroup::slopeCurveSteep,                   TrackPitch::up60,             TrackPitch::up60,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::rightQuarterTurn1TileUp60
        { TrackGroup::slopeCurveSteep,                   TrackPitch::down60,           TrackPitch::down60,         TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::leftQuarterTurn1TileDown60
        { TrackGroup::slopeCurveSteep,                   TrackPitch::down60,           TrackPitch::down60,         TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::rightQuarterTurn1TileDown60
        { TrackGroup::brakes,                            TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::brakes
        { TrackGroup::booster,                           TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::booster
        { TrackGroup::flat,                              TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::maze
        { TrackGroup::helixUpBankedQuarter,              TrackPitch::none,             TrackPitch::none,           TrackRoll::left,        TrackRoll::left,        0   }, // TrackElemType::leftQuarterBankedHelixLargeUp
        { TrackGroup::helixUpBankedQuarter,              TrackPitch::none,             TrackPitch::none,           TrackRoll::right,       TrackRoll::right,       0   }, // TrackElemType::rightQuarterBankedHelixLargeUp
        { TrackGroup::helixDownBankedQuarter,            TrackPitch::none,             TrackPitch::none,           TrackRoll::left,        TrackRoll::left,        0   }, // TrackElemType::leftQuarterBankedHelixLargeDown
        { TrackGroup::helixDownBankedQuarter,            TrackPitch::none,             TrackPitch::none,           TrackRoll::right,       TrackRoll::right,       0   }, // TrackElemType::rightQuarterBankedHelixLargeDown
        { TrackGroup::helixUpUnbankedQuarter,            TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::leftQuarterHelixLargeUp
        { TrackGroup::helixUpUnbankedQuarter,            TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::rightQuarterHelixLargeUp
        { TrackGroup::helixDownUnbankedQuarter,          TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::leftQuarterHelixLargeDown
        { TrackGroup::helixDownUnbankedQuarter,          TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::rightQuarterHelixLargeDown
        { TrackGroup::slopeRollBanking,                  TrackPitch::up25,             TrackPitch::up25,           TrackRoll::left,        TrackRoll::left,        0   }, // TrackElemType::up25LeftBanked
        { TrackGroup::slopeRollBanking,                  TrackPitch::up25,             TrackPitch::up25,           TrackRoll::right,       TrackRoll::right,       0   }, // TrackElemType::up25RightBanked
        { TrackGroup::waterfall,                         TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::waterfall
        { TrackGroup::rapids,                            TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::rapids
        { TrackGroup::onridePhoto,                       TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::onRidePhoto
        { TrackGroup::slopeRollBanking,                  TrackPitch::down25,           TrackPitch::down25,         TrackRoll::left,        TrackRoll::left,        0   }, // TrackElemType::down25LeftBanked
        { TrackGroup::slopeRollBanking,                  TrackPitch::down25,           TrackPitch::down25,         TrackRoll::right,       TrackRoll::right,       0   }, // TrackElemType::down25RightBanked
        { TrackGroup::waterSplash,                       TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::watersplash
        { TrackGroup::slopeSteepLong,                    TrackPitch::up60,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::flatToUp60LongBase
        { TrackGroup::slopeSteepLong,                    TrackPitch::none,             TrackPitch::up60,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::up60ToFlatLongBase
        { TrackGroup::whirlpool,                         TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::whirlpool
        { TrackGroup::slopeSteepLong,                    TrackPitch::none,             TrackPitch::down60,         TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::down60ToFlatLongBase
        { TrackGroup::slopeSteepLong,                    TrackPitch::down60,           TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::flatToDown60LongBase
        { TrackGroup::liftHillCable,                     TrackPitch::down60,           TrackPitch::up25,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::cableLiftHill
        { TrackGroup::reverseFreefall,                   TrackPitch::reverseFreefall,  TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::reverseFreefallSlope
        { TrackGroup::reverseFreefall,                   TrackPitch::reverseFreefall,  TrackPitch::reverseFreefall,TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::reverseFreefallVertical
        { TrackGroup::flat,                              TrackPitch::up90,             TrackPitch::up90,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::up90
        { TrackGroup::flat,                              TrackPitch::down90,           TrackPitch::down90,         TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::down90
        { TrackGroup::flat,                              TrackPitch::up90,             TrackPitch::up60,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::up60ToUp90
        { TrackGroup::flat,                              TrackPitch::down60,           TrackPitch::down90,         TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::down90ToDown60
        { TrackGroup::flat,                              TrackPitch::up60,             TrackPitch::up90,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::up90ToUp60
        { TrackGroup::flat,                              TrackPitch::down90,           TrackPitch::down60,         TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::down60ToDown90
        { TrackGroup::brakeForDrop,                      TrackPitch::down60,           TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::brakeForDrop
        { TrackGroup::flat,                              TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::leftEighthToDiag
        { TrackGroup::flat,                              TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::rightEighthToDiag
        { TrackGroup::flat,                              TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::leftEighthToOrthogonal
        { TrackGroup::flat,                              TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::rightEighthToOrthogonal
        { TrackGroup::flat,                              TrackPitch::none,             TrackPitch::none,           TrackRoll::left,        TrackRoll::left,        0   }, // TrackElemType::leftEighthBankToDiag
        { TrackGroup::flat,                              TrackPitch::none,             TrackPitch::none,           TrackRoll::right,       TrackRoll::right,       0   }, // TrackElemType::rightEighthBankToDiag
        { TrackGroup::flat,                              TrackPitch::none,             TrackPitch::none,           TrackRoll::left,        TrackRoll::left,        0   }, // TrackElemType::leftEighthBankToOrthogonal
        { TrackGroup::flat,                              TrackPitch::none,             TrackPitch::none,           TrackRoll::right,       TrackRoll::right,       0   }, // TrackElemType::rightEighthBankToOrthogonal
        { TrackGroup::flat,                              TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::diagFlat
        { TrackGroup::diagSlope,                         TrackPitch::up25,             TrackPitch::up25,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::diagUp25
        { TrackGroup::diagSlopeSteepUp,                  TrackPitch::up60,             TrackPitch::up60,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::diagUp60
        { TrackGroup::diagSlope,                         TrackPitch::up25,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::diagFlatToUp25
        { TrackGroup::diagSlopeSteepUp,                  TrackPitch::up60,             TrackPitch::up25,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::diagUp25ToUp60
        { TrackGroup::diagSlopeSteepUp,                  TrackPitch::up25,             TrackPitch::up60,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::diagUp60ToUp25
        { TrackGroup::diagSlope,                         TrackPitch::none,             TrackPitch::up25,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::diagUp25ToFlat
        { TrackGroup::diagSlope,                         TrackPitch::down25,           TrackPitch::down25,         TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::diagDown25
        { TrackGroup::diagSlopeSteepDown,                TrackPitch::down60,           TrackPitch::down60,         TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::diagDown60
        { TrackGroup::diagSlope,                         TrackPitch::down25,           TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::diagFlatToDown25
        { TrackGroup::diagSlopeSteepDown,                TrackPitch::down60,           TrackPitch::down25,         TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::diagDown25ToDown60
        { TrackGroup::diagSlopeSteepDown,                TrackPitch::down25,           TrackPitch::down60,         TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::diagDown60ToDown25
        { TrackGroup::diagSlope,                         TrackPitch::none,             TrackPitch::down25,         TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::diagDown25ToFlat
        { TrackGroup::diagSlopeSteepUp,                  TrackPitch::up60,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::diagFlatToUp60
        { TrackGroup::diagSlopeSteepUp,                  TrackPitch::none,             TrackPitch::up60,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::diagUp60ToFlat
        { TrackGroup::diagSlopeSteepDown,                TrackPitch::down60,           TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::diagFlatToDown60
        { TrackGroup::diagSlopeSteepDown,                TrackPitch::none,             TrackPitch::down60,         TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::diagDown60ToFlat
        { TrackGroup::flat,                              TrackPitch::none,             TrackPitch::none,           TrackRoll::left,        TrackRoll::none,        0   }, // TrackElemType::diagFlatToLeftBank
        { TrackGroup::flat,                              TrackPitch::none,             TrackPitch::none,           TrackRoll::right,       TrackRoll::none,        0   }, // TrackElemType::diagFlatToRightBank
        { TrackGroup::flat,                              TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::left,        0   }, // TrackElemType::diagLeftBankToFlat
        { TrackGroup::flat,                              TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::right,       0   }, // TrackElemType::diagRightBankToFlat
        { TrackGroup::flat,                              TrackPitch::up25,             TrackPitch::none,           TrackRoll::none,        TrackRoll::left,        0   }, // TrackElemType::diagLeftBankToUp25
        { TrackGroup::flat,                              TrackPitch::up25,             TrackPitch::none,           TrackRoll::none,        TrackRoll::right,       0   }, // TrackElemType::diagRightBankToUp25
        { TrackGroup::flat,                              TrackPitch::none,             TrackPitch::up25,           TrackRoll::left,        TrackRoll::none,        0   }, // TrackElemType::diagUp25ToLeftBank
        { TrackGroup::flat,                              TrackPitch::none,             TrackPitch::up25,           TrackRoll::right,       TrackRoll::none,        0   }, // TrackElemType::diagUp25ToRightBank
        { TrackGroup::flat,                              TrackPitch::down25,           TrackPitch::none,           TrackRoll::none,        TrackRoll::left,        0   }, // TrackElemType::diagLeftBankToDown25
        { TrackGroup::flat,                              TrackPitch::down25,           TrackPitch::none,           TrackRoll::none,        TrackRoll::right,       0   }, // TrackElemType::diagRightBankToDown25
        { TrackGroup::flat,                              TrackPitch::none,             TrackPitch::down25,         TrackRoll::left,        TrackRoll::none,        0   }, // TrackElemType::diagDown25ToLeftBank
        { TrackGroup::flat,                              TrackPitch::none,             TrackPitch::down25,         TrackRoll::right,       TrackRoll::none,        0   }, // TrackElemType::diagDown25ToRightBank
        { TrackGroup::flat,                              TrackPitch::none,             TrackPitch::none,           TrackRoll::left,        TrackRoll::left,        0   }, // TrackElemType::diagLeftBank
        { TrackGroup::flat,                              TrackPitch::none,             TrackPitch::none,           TrackRoll::right,       TrackRoll::right,       0   }, // TrackElemType::diagRightBank
        { TrackGroup::logFlumeReverser,                  TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::logFlumeReverser
        { TrackGroup::spinningTunnel,                    TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::spinningTunnel
        { TrackGroup::barrelRoll,                        TrackPitch::none,             TrackPitch::none,           TrackRoll::upsideDown,  TrackRoll::none,        0   }, // TrackElemType::leftBarrelRollUpToDown
        { TrackGroup::barrelRoll,                        TrackPitch::none,             TrackPitch::none,           TrackRoll::upsideDown,  TrackRoll::none,        0   }, // TrackElemType::rightBarrelRollUpToDown
        { TrackGroup::barrelRoll,                        TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::upsideDown,  0   }, // TrackElemType::leftBarrelRollDownToUp
        { TrackGroup::barrelRoll,                        TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::upsideDown,  0   }, // TrackElemType::rightBarrelRollDownToUp
        { TrackGroup::flat,                              TrackPitch::up25,             TrackPitch::none,           TrackRoll::none,        TrackRoll::left,        0   }, // TrackElemType::leftBankToLeftQuarterTurn3TilesUp25
        { TrackGroup::flat,                              TrackPitch::up25,             TrackPitch::none,           TrackRoll::none,        TrackRoll::right,       0   }, // TrackElemType::rightBankToRightQuarterTurn3TilesUp25
        { TrackGroup::flat,                              TrackPitch::none,             TrackPitch::down25,         TrackRoll::left,        TrackRoll::none,        0   }, // TrackElemType::leftQuarterTurn3TilesDown25ToLeftBank
        { TrackGroup::flat,                              TrackPitch::none,             TrackPitch::down25,         TrackRoll::right,       TrackRoll::none,        0   }, // TrackElemType::rightQuarterTurn3TilesDown25ToRightBank
        { TrackGroup::poweredLift,                       TrackPitch::up25,             TrackPitch::up25,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::poweredLift
        {TrackGroup::halfLoopLarge,                      TrackPitch::none,             TrackPitch::up25,           TrackRoll::upsideDown,  TrackRoll::none,        64  }, // TrackElemType::leftLargeHalfLoopUp
        {TrackGroup::halfLoopLarge,                      TrackPitch::none,             TrackPitch::up25,           TrackRoll::upsideDown,  TrackRoll::none,        64  }, // TrackElemType::rightLargeHalfLoopUp
        {TrackGroup::halfLoopLarge,                      TrackPitch::down25,           TrackPitch::none,           TrackRoll::none,        TrackRoll::upsideDown,  -64 }, // TrackElemType::leftLargeHalfLoopDown
        {TrackGroup::halfLoopLarge,                      TrackPitch::down25,           TrackPitch::none,           TrackRoll::none,        TrackRoll::upsideDown,  -64 }, // TrackElemType::rightLargeHalfLoopDown
        {TrackGroup::inlineTwistUninverted,              TrackPitch::none,             TrackPitch::none,           TrackRoll::upsideDown,  TrackRoll::none,        0   }, // TrackElemType::leftFlyerTwistUp
        {TrackGroup::inlineTwistUninverted,              TrackPitch::none,             TrackPitch::none,           TrackRoll::upsideDown,  TrackRoll::none,        0   }, // TrackElemType::rightFlyerTwistUp
        {TrackGroup::inlineTwistInverted,                TrackPitch::none,             TrackPitch::none,           TrackRoll::upsideDown,  TrackRoll::none,        0   }, // TrackElemType::leftFlyerTwistDown
        {TrackGroup::inlineTwistInverted,                TrackPitch::none,             TrackPitch::none,           TrackRoll::upsideDown,  TrackRoll::none,        0   }, // TrackElemType::rightFlyerTwistDown
        {TrackGroup::flyingHalfLoopUninvertedUp,         TrackPitch::none,             TrackPitch::up25,           TrackRoll::upsideDown,  TrackRoll::none,        64  }, // TrackElemType::flyerHalfLoopUninvertedUp
        {TrackGroup::flyingHalfLoopInvertedDown,         TrackPitch::down25,           TrackPitch::none,           TrackRoll::upsideDown,  TrackRoll::none,        -64 }, // TrackElemType::flyerHalfLoopInvertedDown
        {TrackGroup::corkscrewUninverted,                TrackPitch::none,             TrackPitch::none,           TrackRoll::upsideDown,  TrackRoll::none,        0   }, // TrackElemType::leftFlyerCorkscrewUp
        {TrackGroup::corkscrewUninverted,                TrackPitch::none,             TrackPitch::none,           TrackRoll::upsideDown,  TrackRoll::none,        0   }, // TrackElemType::rightFlyerCorkscrewUp
        {TrackGroup::corkscrewInverted,                  TrackPitch::none,             TrackPitch::none,           TrackRoll::upsideDown,  TrackRoll::none,        -32 }, // TrackElemType::leftFlyerCorkscrewDown
        {TrackGroup::corkscrewInverted,                  TrackPitch::none,             TrackPitch::none,           TrackRoll::upsideDown,  TrackRoll::none,        -32 }, // TrackElemType::rightFlyerCorkscrewDown
        {TrackGroup::heartlineTransfer,                  TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::heartLineTransferUp
        {TrackGroup::heartlineTransfer,                  TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::heartLineTransferDown
        {TrackGroup::heartlineRoll,                      TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::leftHeartLineRoll
        {TrackGroup::heartlineRoll,                      TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::rightHeartLineRoll
        {TrackGroup::miniGolfHole,                       TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::minigolfHoleA
        {TrackGroup::miniGolfHole,                       TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::minigolfHoleB
        {TrackGroup::miniGolfHole,                       TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::minigolfHoleC
        {TrackGroup::miniGolfHole,                       TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::minigolfHoleD
        {TrackGroup::miniGolfHole,                       TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::minigolfHoleE
        { TrackGroup::quarterLoopInvertedDown,           TrackPitch::down90,           TrackPitch::none,           TrackRoll::upsideDown,  TrackRoll::none,        0   }, // TrackElemType::multiDimInvertedFlatToDown90QuarterLoop
        { TrackGroup::quarterLoop,                       TrackPitch::none,             TrackPitch::up90,           TrackRoll::upsideDown,  TrackRoll::none,        0   }, // TrackElemType::up90ToInvertedFlatQuarterLoop
        { TrackGroup::quarterLoop,                       TrackPitch::down90,           TrackPitch::none,           TrackRoll::none,        TrackRoll::upsideDown,  0   }, // TrackElemType::invertedFlatToDown90QuarterLoop
        { TrackGroup::liftHillCurved,                    TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::leftCurvedLiftHill
        { TrackGroup::liftHillCurved,                    TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::rightCurvedLiftHill
        { TrackGroup::reverser,                          TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::leftReverser
        { TrackGroup::reverser,                          TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::rightReverser
        { TrackGroup::slopeToFlat,                       TrackPitch::down90,           TrackPitch::up90,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::airThrustTopCap
        { TrackGroup::slopeToFlat,                       TrackPitch::down90,           TrackPitch::down90,         TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::airThrustVerticalDown
        { TrackGroup::slopeToFlat,                       TrackPitch::none,             TrackPitch::down90,         TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::airThrustVerticalDownToLevel
        { TrackGroup::blockBrakes,                       TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::blockBrakes
        { TrackGroup::slopeCurve,                        TrackPitch::up25,             TrackPitch::up25,           TrackRoll::left,        TrackRoll::left,        0   }, // TrackElemType::leftBankedQuarterTurn3TileUp25
        { TrackGroup::slopeCurve,                        TrackPitch::up25,             TrackPitch::up25,           TrackRoll::right,       TrackRoll::right,       0   }, // TrackElemType::rightBankedQuarterTurn3TileUp25
        { TrackGroup::slopeCurve,                        TrackPitch::down25,           TrackPitch::down25,         TrackRoll::left,        TrackRoll::left,        0   }, // TrackElemType::leftBankedQuarterTurn3TileDown25
        { TrackGroup::slopeCurve,                        TrackPitch::down25,           TrackPitch::down25,         TrackRoll::right,       TrackRoll::right,       0   }, // TrackElemType::rightBankedQuarterTurn3TileDown25
        { TrackGroup::slopeCurve,                        TrackPitch::up25,             TrackPitch::up25,           TrackRoll::left,        TrackRoll::left,        0   }, // TrackElemType::leftBankedQuarterTurn5TileUp25
        { TrackGroup::slopeCurve,                        TrackPitch::up25,             TrackPitch::up25,           TrackRoll::right,       TrackRoll::right,       0   }, // TrackElemType::rightBankedQuarterTurn5TileUp25
        { TrackGroup::slopeCurve,                        TrackPitch::down25,           TrackPitch::down25,         TrackRoll::left,        TrackRoll::left,        0   }, // TrackElemType::leftBankedQuarterTurn5TileDown25
        { TrackGroup::slopeCurve,                        TrackPitch::down25,           TrackPitch::down25,         TrackRoll::right,       TrackRoll::right,       0   }, // TrackElemType::rightBankedQuarterTurn5TileDown25
        { TrackGroup::slopeRollBanking,                  TrackPitch::up25,             TrackPitch::up25,           TrackRoll::left,        TrackRoll::none,        0   }, // TrackElemType::up25ToLeftBankedUp25
        { TrackGroup::slopeRollBanking,                  TrackPitch::up25,             TrackPitch::up25,           TrackRoll::right,       TrackRoll::none,        0   }, // TrackElemType::up25ToRightBankedUp25
        { TrackGroup::slopeRollBanking,                  TrackPitch::up25,             TrackPitch::up25,           TrackRoll::none,        TrackRoll::left,        0   }, // TrackElemType::leftBankedUp25ToUp25
        { TrackGroup::slopeRollBanking,                  TrackPitch::up25,             TrackPitch::up25,           TrackRoll::none,        TrackRoll::right,       0   }, // TrackElemType::rightBankedUp25ToUp25
        { TrackGroup::slopeRollBanking,                  TrackPitch::down25,           TrackPitch::down25,         TrackRoll::left,        TrackRoll::none,        0   }, // TrackElemType::down25ToLeftBankedDown25
        { TrackGroup::slopeRollBanking,                  TrackPitch::down25,           TrackPitch::down25,         TrackRoll::right,       TrackRoll::none,        0   }, // TrackElemType::down25ToRightBankedDown25
        { TrackGroup::slopeRollBanking,                  TrackPitch::down25,           TrackPitch::down25,         TrackRoll::none,        TrackRoll::left,        0   }, // TrackElemType::leftBankedDown25ToDown25
        { TrackGroup::slopeRollBanking,                  TrackPitch::down25,           TrackPitch::down25,         TrackRoll::none,        TrackRoll::right,       0   }, // TrackElemType::rightBankedDown25ToDown25
        { TrackGroup::slopeRollBanking,                  TrackPitch::up25,             TrackPitch::none,           TrackRoll::left,        TrackRoll::left,        0   }, // TrackElemType::leftBankedFlatToLeftBankedUp25
        { TrackGroup::slopeRollBanking,                  TrackPitch::up25,             TrackPitch::none,           TrackRoll::right,       TrackRoll::right,       0   }, // TrackElemType::rightBankedFlatToRightBankedUp25
        { TrackGroup::slopeRollBanking,                  TrackPitch::none,             TrackPitch::up25,           TrackRoll::left,        TrackRoll::left,        0   }, // TrackElemType::leftBankedUp25ToLeftBankedFlat
        { TrackGroup::slopeRollBanking,                  TrackPitch::none,             TrackPitch::up25,           TrackRoll::right,       TrackRoll::right,       0   }, // TrackElemType::rightBankedUp25ToRightBankedFlat
        { TrackGroup::slopeRollBanking,                  TrackPitch::down25,           TrackPitch::none,           TrackRoll::left,        TrackRoll::left,        0   }, // TrackElemType::leftBankedFlatToLeftBankedDown25
        { TrackGroup::slopeRollBanking,                  TrackPitch::down25,           TrackPitch::none,           TrackRoll::right,       TrackRoll::right,       0   }, // TrackElemType::rightBankedFlatToRightBankedDown25
        { TrackGroup::slopeRollBanking,                  TrackPitch::none,             TrackPitch::down25,         TrackRoll::left,        TrackRoll::left,        0   }, // TrackElemType::leftBankedDown25ToLeftBankedFlat
        { TrackGroup::slopeRollBanking,                  TrackPitch::none,             TrackPitch::down25,         TrackRoll::right,       TrackRoll::right,       0   }, // TrackElemType::rightBankedDown25ToRightBankedFlat
        { TrackGroup::slopeRollBanking,                  TrackPitch::up25,             TrackPitch::none,           TrackRoll::left,        TrackRoll::none,        0   }, // TrackElemType::flatToLeftBankedUp25
        { TrackGroup::slopeRollBanking,                  TrackPitch::up25,             TrackPitch::none,           TrackRoll::right,       TrackRoll::none,        0   }, // TrackElemType::flatToRightBankedUp25
        { TrackGroup::slopeRollBanking,                  TrackPitch::none,             TrackPitch::up25,           TrackRoll::none,        TrackRoll::left,        0   }, // TrackElemType::leftBankedUp25ToFlat
        { TrackGroup::slopeRollBanking,                  TrackPitch::none,             TrackPitch::up25,           TrackRoll::none,        TrackRoll::right,       0   }, // TrackElemType::rightBankedUp25ToFlat
        { TrackGroup::slopeRollBanking,                  TrackPitch::down25,           TrackPitch::none,           TrackRoll::left,        TrackRoll::none,        0   }, // TrackElemType::flatToLeftBankedDown25
        { TrackGroup::slopeRollBanking,                  TrackPitch::down25,           TrackPitch::none,           TrackRoll::right,       TrackRoll::none,        0   }, // TrackElemType::flatToRightBankedDown25
        { TrackGroup::slopeRollBanking,                  TrackPitch::none,             TrackPitch::down25,         TrackRoll::none,        TrackRoll::left,        0   }, // TrackElemType::leftBankedDown25ToFlat
        { TrackGroup::slopeRollBanking,                  TrackPitch::none,             TrackPitch::down25,         TrackRoll::none,        TrackRoll::right,       0   }, // TrackElemType::rightBankedDown25ToFlat
        { TrackGroup::slopeCurveSteep,                   TrackPitch::up90,             TrackPitch::up90,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::leftQuarterTurn1TileUp90
        { TrackGroup::slopeCurveSteep,                   TrackPitch::up90,             TrackPitch::up90,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::rightQuarterTurn1TileUp90
        { TrackGroup::slopeCurveSteep,                   TrackPitch::down90,           TrackPitch::down90,         TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::leftQuarterTurn1TileDown90
        { TrackGroup::slopeCurveSteep,                   TrackPitch::down90,           TrackPitch::down90,         TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::rightQuarterTurn1TileDown90
        { TrackGroup::quarterLoopUninvertedUp,           TrackPitch::none,             TrackPitch::up90,           TrackRoll::upsideDown,  TrackRoll::none,        0   }, // TrackElemType::multiDimUp90ToInvertedFlatQuarterLoop
        { TrackGroup::quarterLoopUninvertedDown,         TrackPitch::down90,           TrackPitch::none,           TrackRoll::upsideDown,  TrackRoll::none,        0   }, // TrackElemType::multiDimFlatToDown90QuarterLoop
        { TrackGroup::quarterLoopInvertedUp,             TrackPitch::none,             TrackPitch::up90,           TrackRoll::upsideDown,  TrackRoll::none,        0   }, // TrackElemType::multiDimInvertedUp90ToFlatQuarterLoop
        { TrackGroup::rotationControlToggle,             TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::rotationControlToggle
        { TrackGroup::flatRideBase,                      TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::flatTrack1x4A
        { TrackGroup::flatRideBase,                      TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::flatTrack2x2
        { TrackGroup::flatRideBase,                      TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::flatTrack4x4
        { TrackGroup::flatRideBase,                      TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::flatTrack2x4
        { TrackGroup::flatRideBase,                      TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::flatTrack1x5
        { TrackGroup::flatRideBase,                      TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::flatTrack1x1A
        { TrackGroup::flatRideBase,                      TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::flatTrack1x4B
        { TrackGroup::flatRideBase,                      TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::flatTrack1x1B
        { TrackGroup::flatRideBase,                      TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        -40 }, // TrackElemType::flatTrack1x4C
        { TrackGroup::flatRideBase,                      TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        0   }, // TrackElemType::flatTrack3x3
        { TrackGroup::corkscrewLarge,                    TrackPitch::none,             TrackPitch::none,           TrackRoll::upsideDown,  TrackRoll::none,        32  }, // TrackElemType::leftLargeCorkscrewUp
        { TrackGroup::corkscrewLarge,                    TrackPitch::none,             TrackPitch::none,           TrackRoll::upsideDown,  TrackRoll::none,        32  }, // TrackElemType::rightLargeCorkscrewUp
        { TrackGroup::corkscrewLarge,                    TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::upsideDown,  -32 }, // TrackElemType::leftLargeCorkscrewDown
        { TrackGroup::corkscrewLarge,                    TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::upsideDown,  -32 }, // TrackElemType::rightLargeCorkscrewDown
        { TrackGroup::halfLoopMedium,                    TrackPitch::none,             TrackPitch::up25,           TrackRoll::upsideDown,  TrackRoll::none,        64  }, // TrackElemType::leftMediumHalfLoopUp
        { TrackGroup::halfLoopMedium,                    TrackPitch::none,             TrackPitch::up25,           TrackRoll::upsideDown,  TrackRoll::none,        64  }, // TrackElemType::rightMediumHalfLoopUp
        { TrackGroup::halfLoopMedium,                    TrackPitch::down25,           TrackPitch::none,           TrackRoll::none,        TrackRoll::upsideDown,  -64 }, // TrackElemType::leftMediumHalfLoopDown
        { TrackGroup::halfLoopMedium,                    TrackPitch::down25,           TrackPitch::none,           TrackRoll::none,        TrackRoll::upsideDown,  -64 }, // TrackElemType::rightMediumHalfLoopDown
        { TrackGroup::zeroGRoll,                         TrackPitch::none,             TrackPitch::up25,           TrackRoll::upsideDown,  TrackRoll::none,        0   }, // TrackElemType::leftZeroGRollUp
        { TrackGroup::zeroGRoll,                         TrackPitch::none,             TrackPitch::up25,           TrackRoll::upsideDown,  TrackRoll::none,        0   }, // TrackElemType::rightZeroGRollUp
        { TrackGroup::zeroGRoll,                         TrackPitch::down25,           TrackPitch::none,           TrackRoll::none,        TrackRoll::upsideDown,  0   }, // TrackElemType::leftZeroGRollDown
        { TrackGroup::zeroGRoll,                         TrackPitch::down25,           TrackPitch::none,           TrackRoll::none,        TrackRoll::upsideDown,  0   }, // TrackElemType::rightZeroGRollDown
        { TrackGroup::zeroGRollLarge,                    TrackPitch::none,             TrackPitch::up60,           TrackRoll::upsideDown,  TrackRoll::none,        64  }, // TrackElemType::leftLargeZeroGRollUp
        { TrackGroup::zeroGRollLarge,                    TrackPitch::none,             TrackPitch::up60,           TrackRoll::upsideDown,  TrackRoll::none,        64  }, // TrackElemType::rightLargeZeroGRollUp
        { TrackGroup::zeroGRollLarge,                    TrackPitch::down60,           TrackPitch::none,           TrackRoll::none,        TrackRoll::upsideDown,  -64 }, // TrackElemType::leftLargeZeroGRollDown
        { TrackGroup::zeroGRollLarge,                    TrackPitch::down60,           TrackPitch::none,           TrackRoll::none,        TrackRoll::upsideDown,  -64 }, // TrackElemType::rightLargeZeroGRollDown
        { TrackGroup::flyingLargeHalfLoopUninvertedUp,   TrackPitch::none,             TrackPitch::up25,           TrackRoll::upsideDown,  TrackRoll::none,        64  }, // TrackElemType::leftFlyerLargeHalfLoopUninvertedUp
        { TrackGroup::flyingLargeHalfLoopUninvertedUp,   TrackPitch::none,             TrackPitch::up25,           TrackRoll::upsideDown,  TrackRoll::none,        64  }, // TrackElemType::rightFlyerLargeHalfLoopUninvertedUp
        { TrackGroup::flyingLargeHalfLoopInvertedDown,   TrackPitch::down25,           TrackPitch::none,           TrackRoll::upsideDown,  TrackRoll::none,        -64 }, // TrackElemType::leftFlyerLargeHalfLoopInvertedDown
        { TrackGroup::flyingLargeHalfLoopInvertedDown,   TrackPitch::down25,           TrackPitch::none,           TrackRoll::upsideDown,  TrackRoll::none,        -64 }, // TrackElemType::rightFlyerLargeHalfLoopInvertedDown
        { TrackGroup::flyingLargeHalfLoopInvertedUp,     TrackPitch::none,             TrackPitch::up25,           TrackRoll::upsideDown,  TrackRoll::none,        64  }, // TrackElemType::leftFlyerLargeHalfLoopInvertedUp
        { TrackGroup::flyingLargeHalfLoopInvertedUp,     TrackPitch::none,             TrackPitch::up25,           TrackRoll::upsideDown,  TrackRoll::none,        64  }, // TrackElemType::rightFlyerLargeHalfLoopInvertedUp
        { TrackGroup::flyingLargeHalfLoopUninvertedDown, TrackPitch::down25,           TrackPitch::none,           TrackRoll::upsideDown,  TrackRoll::none,       -64  }, // TrackElemType::leftFlyerLargeHalfLoopUninvertedDown
        { TrackGroup::flyingLargeHalfLoopUninvertedDown, TrackPitch::down25,           TrackPitch::none,           TrackRoll::upsideDown,  TrackRoll::none,       -64  }, // TrackElemType::rightFlyerLargeHalfLoopUninvertedDown
        { TrackGroup::flyingHalfLoopInvertedUp,          TrackPitch::none,             TrackPitch::up25,           TrackRoll::upsideDown,  TrackRoll::none,        64  }, // TrackElemType::flyerHalfLoopInvertedUp
        { TrackGroup::flyingHalfLoopUninvertedDown,      TrackPitch::down25,           TrackPitch::none,           TrackRoll::upsideDown,  TrackRoll::none,       -64  }, // TrackElemType::flyerHalfLoopUninvertedDown
        { TrackGroup::slopeCurveLarge,                   TrackPitch::up25,             TrackPitch::up25,           TrackRoll::none,        TrackRoll::none,        16  }, // TrackElemType::leftEighthToDiagUp25
        { TrackGroup::slopeCurveLarge,                   TrackPitch::up25,             TrackPitch::up25,           TrackRoll::none,        TrackRoll::none,        16  }, // TrackElemType::rightEighthToDiagUp25
        { TrackGroup::slopeCurveLarge,                   TrackPitch::down25,           TrackPitch::down25,         TrackRoll::none,        TrackRoll::none,       -16  }, // TrackElemType::leftEighthToDiagDown25
        { TrackGroup::slopeCurveLarge,                   TrackPitch::down25,           TrackPitch::down25,         TrackRoll::none,        TrackRoll::none,       -16  }, // TrackElemType::rightEighthToDiagDown25
        { TrackGroup::slopeCurveLarge,                   TrackPitch::up25,             TrackPitch::up25,           TrackRoll::none,        TrackRoll::none,        16  }, // TrackElemType::leftEighthToOrthogonalUp25
        { TrackGroup::slopeCurveLarge,                   TrackPitch::up25,             TrackPitch::up25,           TrackRoll::none,        TrackRoll::none,        16  }, // TrackElemType::rightEighthToOrthogonalUp25
        { TrackGroup::slopeCurveLarge,                   TrackPitch::down25,           TrackPitch::down25,         TrackRoll::none,        TrackRoll::none,       -16  }, // TrackElemType::leftEighthToOrthogonalDown25
        { TrackGroup::slopeCurveLarge,                   TrackPitch::down25,           TrackPitch::down25,         TrackRoll::none,        TrackRoll::none,       -16  }, // TrackElemType::rightEighthToOrthogonalDown25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::up25,             TrackPitch::up25,           TrackRoll::left,        TrackRoll::none,       0    }, // TrackElemType::diagUp25ToLeftBankedUp25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::up25,             TrackPitch::up25,           TrackRoll::right,       TrackRoll::none,       0    }, // TrackElemType::diagUp25ToRightBankedUp25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::up25,             TrackPitch::up25,           TrackRoll::none,        TrackRoll::left,       0    }, // TrackElemType::diagLeftBankedUp25ToUp25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::up25,             TrackPitch::up25,           TrackRoll::none,        TrackRoll::right,      0    }, // TrackElemType::diagRightBankedUp25ToUp25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::down25,           TrackPitch::down25,         TrackRoll::left,        TrackRoll::none,       0    }, // TrackElemType::diagDown25ToLeftBankedDown25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::down25,           TrackPitch::down25,         TrackRoll::right,       TrackRoll::none,       0    }, // TrackElemType::diagDown25ToRightBankedDown25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::down25,           TrackPitch::down25,         TrackRoll::none,        TrackRoll::left,       0    }, // TrackElemType::diagLeftBankedDown25ToDown25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::down25,           TrackPitch::down25,         TrackRoll::none,        TrackRoll::right,      0    }, // TrackElemType::diagRightBankedDown25ToDown25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::up25,             TrackPitch::none,           TrackRoll::left,        TrackRoll::left,       0    }, // TrackElemType::diagLeftBankedFlatToLeftBankedUp25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::up25,             TrackPitch::none,           TrackRoll::right,       TrackRoll::right,      0    }, // TrackElemType::diagRightBankedFlatToRightBankedUp25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::none,             TrackPitch::up25,           TrackRoll::left,        TrackRoll::left,       0    }, // TrackElemType::diagLeftBankedUp25ToLeftBankedFlat
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::none,             TrackPitch::up25,           TrackRoll::right,       TrackRoll::right,      0    }, // TrackElemType::diagRightBankedUp25ToRightBankedFlat
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::down25,           TrackPitch::none,           TrackRoll::left,        TrackRoll::left,       0    }, // TrackElemType::diagLeftBankedFlatToLeftBankedDown25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::down25,           TrackPitch::none,           TrackRoll::right,       TrackRoll::right,      0    }, // TrackElemType::diagRightBankedFlatToRightBankedDown25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::none,             TrackPitch::down25,         TrackRoll::left,        TrackRoll::left,       0    }, // TrackElemType::diagLeftBankedDown25ToLeftBankedFlat
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::none,             TrackPitch::down25,         TrackRoll::right,       TrackRoll::right,      0    }, // TrackElemType::diagRightBankedDown25ToRightBankedFlat
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::up25,             TrackPitch::none,           TrackRoll::left,        TrackRoll::none,       0    }, // TrackElemType::diagFlatToLeftBankedUp25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::up25,             TrackPitch::none,           TrackRoll::right,       TrackRoll::none,       0    }, // TrackElemType::diagFlatToRightBankedUp25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::none,             TrackPitch::up25,           TrackRoll::none,        TrackRoll::left,       0    }, // TrackElemType::diagLeftBankedUp25ToFlat
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::none,             TrackPitch::up25,           TrackRoll::none,        TrackRoll::right,      0    }, // TrackElemType::diagRightBankedUp25ToFlat
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::down25,           TrackPitch::none,           TrackRoll::left,        TrackRoll::none,       0    }, // TrackElemType::diagFlatToLeftBankedDown25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::down25,           TrackPitch::none,           TrackRoll::right,       TrackRoll::none,       0    }, // TrackElemType::diagFlatToRightBankedDown25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::none,             TrackPitch::down25,         TrackRoll::none,        TrackRoll::left,       0    }, // TrackElemType::diagLeftBankedDown25ToFlat
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::none,             TrackPitch::down25,         TrackRoll::none,        TrackRoll::right,      0    }, // TrackElemType::diagRightBankedDown25ToFlat
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::up25,             TrackPitch::up25,           TrackRoll::left,        TrackRoll::left,       0    }, // TrackElemType::up25LeftBanked
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::up25,             TrackPitch::up25,           TrackRoll::right,       TrackRoll::right,      0    }, // TrackElemType::up25RightBanked
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::down25,           TrackPitch::down25,         TrackRoll::left,        TrackRoll::left,       0    }, // TrackElemType::down25LeftBanked
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::down25,           TrackPitch::down25,         TrackRoll::right,       TrackRoll::right,      0    }, // TrackElemType::down25RightBanked
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::up25,             TrackPitch::up25,           TrackRoll::left,        TrackRoll::left,      16    }, // TrackElemType::leftEighthBankToDiagUp25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::up25,             TrackPitch::up25,           TrackRoll::right,       TrackRoll::right,     16    }, // TrackElemType::rightEighthBankToDiagUp25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::down25,           TrackPitch::down25,         TrackRoll::left,        TrackRoll::left,     -16    }, // TrackElemType::leftEighthBankToDiagDown25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::down25,           TrackPitch::down25,         TrackRoll::right,       TrackRoll::right,    -16    }, // TrackElemType::rightEighthBankToDiagDown25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::up25,             TrackPitch::up25,           TrackRoll::left,        TrackRoll::left,      16    }, // TrackElemType::leftEighthBankToOrthogonalUp25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::up25,             TrackPitch::up25,           TrackRoll::right,       TrackRoll::right,     16    }, // TrackElemType::rightEighthBankToOrthogonalUp25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::down25,           TrackPitch::down25,         TrackRoll::left,        TrackRoll::left,     -16    }, // TrackElemType::leftEighthBankToOrthogonalDown25
        { TrackGroup::slopeCurveLargeBanked,             TrackPitch::down25,           TrackPitch::down25,         TrackRoll::right,       TrackRoll::right,    -16    }, // TrackElemType::rightEighthBankToOrthogonalDown25
        { TrackGroup::diagBrakes,                        TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,       0    }, // TrackElemType::diagBrakes
        { TrackGroup::diagBlockBrakes,                   TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,       0    }, // TrackElemType::diagBlockBrakes
        { TrackGroup::inclinedBrakes,                    TrackPitch::down25,           TrackPitch::down25,         TrackRoll::none,        TrackRoll::none,       0    }, // TrackElemType::down25Brakes
        { TrackGroup::diagBooster,                       TrackPitch::none,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,       0    }, // TrackElemType::diagBooster
        { TrackGroup::slopeSteepLong,                    TrackPitch::up60,             TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        16   }, // TrackElemType::diagFlatToUp60LongBase,
        { TrackGroup::slopeSteepLong,                    TrackPitch::none,             TrackPitch::up60,           TrackRoll::none,        TrackRoll::none,        48   }, // TrackElemType::diagUp60ToFlatLongBase,
        { TrackGroup::slopeSteepLong,                    TrackPitch::down60,           TrackPitch::none,           TrackRoll::none,        TrackRoll::none,        -32   }, // TrackElemType::diagFlatToDown60LongBase,
        { TrackGroup::slopeSteepLong,                    TrackPitch::none,             TrackPitch::down60,         TrackRoll::none,        TrackRoll::none,        -24   }, // TrackElemType::diagDown60ToFlatLongBase,
        { TrackGroup::diveLoop,                          TrackPitch::none,             TrackPitch::up60,           TrackRoll::upsideDown,  TrackRoll::none,        56  }, // TrackElemType::leftEighthDiveLoopUpToOrthogonal
        { TrackGroup::diveLoop,                          TrackPitch::none,             TrackPitch::up60,           TrackRoll::upsideDown,  TrackRoll::none,        56  }, // TrackElemType::rightEighthDiveLoopUpToOrthogonal
        { TrackGroup::diveLoop,                          TrackPitch::down60,           TrackPitch::none,           TrackRoll::none,        TrackRoll::upsideDown,  -56 }, // TrackElemType::leftEighthDiveLoopDownToDiag
        { TrackGroup::diveLoop,                          TrackPitch::down60,           TrackPitch::none,           TrackRoll::none,        TrackRoll::upsideDown,  -56 }, // TrackElemType::rightEighthDiveLoopDownToDiag
        { TrackGroup::inclinedBrakes,                    TrackPitch::down25,           TrackPitch::down25,         TrackRoll::none,        TrackRoll::none,       0    }, // TrackElemType::diagDown25
    };
    static_assert(std::size(kTrackDefinitions) == EnumValue(TrackElemType::count));

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
        SpinFunction::R9,   SpinFunction::None
    };
    static_assert(std::size(kTrackTypeToSpinFunction) == EnumValue(TrackElemType::count));

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
            case TrackElemType::flat:
            case TrackElemType::endStation:
            case TrackElemType::beginStation:
            case TrackElemType::middleStation:
            case TrackElemType::up25:
            case TrackElemType::up60: //
            case TrackElemType::down25:
            case TrackElemType::down60: //
            case TrackElemType::flatToLeftBank:
            case TrackElemType::flatToRightBank:
            case TrackElemType::leftBankToFlat:
            case TrackElemType::rightBankToFlat: //
            case TrackElemType::leftBank:
            case TrackElemType::rightBank:
            case TrackElemType::towerBase:
            case TrackElemType::towerSection:
            case TrackElemType::flatCovered:
            case TrackElemType::up25Covered:
            case TrackElemType::up60Covered:
            case TrackElemType::down25Covered:
            case TrackElemType::down60Covered:
            case TrackElemType::brakes:
            case TrackElemType::rotationControlToggle:
            case TrackElemType::maze:
            case TrackElemType::up25LeftBanked:
            case TrackElemType::up25RightBanked:
            case TrackElemType::waterfall:
            case TrackElemType::rapids:
            case TrackElemType::onRidePhoto:
            case TrackElemType::down25LeftBanked:
            case TrackElemType::down25RightBanked:
            case TrackElemType::whirlpool:
            case TrackElemType::reverseFreefallVertical:
            case TrackElemType::up90:
            case TrackElemType::down90:
            case TrackElemType::diagFlat:
            case TrackElemType::diagUp25:
            case TrackElemType::diagUp60:
            case TrackElemType::diagDown25:
            case TrackElemType::diagDown60:
            case TrackElemType::diagFlatToLeftBank:
            case TrackElemType::diagFlatToRightBank:
            case TrackElemType::diagLeftBankToFlat:
            case TrackElemType::diagRightBankToFlat:
            case TrackElemType::diagLeftBank:
            case TrackElemType::diagRightBank:
            case TrackElemType::logFlumeReverser:
            case TrackElemType::spinningTunnel:
            case TrackElemType::poweredLift:
            case TrackElemType::minigolfHoleA:
            case TrackElemType::minigolfHoleB:
            case TrackElemType::minigolfHoleC:
            case TrackElemType::minigolfHoleD:
            case TrackElemType::minigolfHoleE:
            case TrackElemType::leftReverser:
            case TrackElemType::rightReverser:
            case TrackElemType::airThrustVerticalDown:
            case TrackElemType::blockBrakes:
            case TrackElemType::up25ToLeftBankedUp25:
            case TrackElemType::up25ToRightBankedUp25:
            case TrackElemType::leftBankedUp25ToUp25:
            case TrackElemType::rightBankedUp25ToUp25:
            case TrackElemType::down25ToLeftBankedDown25:
            case TrackElemType::down25ToRightBankedDown25:
            case TrackElemType::leftBankedDown25ToDown25:
            case TrackElemType::rightBankedDown25ToDown25:
            case TrackElemType::leftQuarterTurn1TileUp90:
            case TrackElemType::rightQuarterTurn1TileUp90:
            case TrackElemType::leftQuarterTurn1TileDown90:
            case TrackElemType::rightQuarterTurn1TileDown90:
                return EvaluatorConst<0>;
            case TrackElemType::flatToUp25:   //
            case TrackElemType::down25ToFlat: //
            case TrackElemType::leftBankToUp25:
            case TrackElemType::rightBankToUp25:
            case TrackElemType::down25ToLeftBank:
            case TrackElemType::down25ToRightBank:
            case TrackElemType::flatToUp25Covered:
            case TrackElemType::down25ToFlatCovered:
            case TrackElemType::leftBankedFlatToLeftBankedUp25:
            case TrackElemType::rightBankedFlatToRightBankedUp25:
            case TrackElemType::leftBankedDown25ToLeftBankedFlat:
            case TrackElemType::rightBankedDown25ToRightBankedFlat:
            case TrackElemType::flatToLeftBankedUp25:
            case TrackElemType::flatToRightBankedUp25:
            case TrackElemType::leftBankedDown25ToFlat:
            case TrackElemType::rightBankedDown25ToFlat:
            case TrackElemType::diagUp25ToLeftBankedUp25:
            case TrackElemType::diagUp25ToRightBankedUp25:
            case TrackElemType::diagLeftBankedUp25ToUp25:
            case TrackElemType::diagRightBankedUp25ToUp25:
            case TrackElemType::diagDown25ToLeftBankedDown25:
            case TrackElemType::diagDown25ToRightBankedDown25:
            case TrackElemType::diagLeftBankedDown25ToDown25:
            case TrackElemType::diagRightBankedDown25ToDown25:
            case TrackElemType::diagLeftBankedFlatToLeftBankedUp25:
            case TrackElemType::diagRightBankedFlatToRightBankedUp25:
            case TrackElemType::diagLeftBankedUp25ToLeftBankedFlat:
            case TrackElemType::diagRightBankedUp25ToRightBankedFlat:
            case TrackElemType::diagLeftBankedFlatToLeftBankedDown25:
            case TrackElemType::diagRightBankedFlatToRightBankedDown25:
            case TrackElemType::diagLeftBankedDown25ToLeftBankedFlat:
            case TrackElemType::diagRightBankedDown25ToRightBankedFlat:
            case TrackElemType::diagFlatToLeftBankedUp25:
            case TrackElemType::diagFlatToRightBankedUp25:
            case TrackElemType::diagLeftBankedUp25ToFlat:
            case TrackElemType::diagRightBankedUp25ToFlat:
            case TrackElemType::diagFlatToLeftBankedDown25:
            case TrackElemType::diagFlatToRightBankedDown25:
            case TrackElemType::diagLeftBankedDown25ToFlat:
            case TrackElemType::diagRightBankedDown25ToFlat:
            case TrackElemType::diagUp25LeftBanked:
            case TrackElemType::diagUp25RightBanked:
            case TrackElemType::diagDown25LeftBanked:
            case TrackElemType::diagDown25RightBanked:
                return EvaluatorConst<0>;
            case TrackElemType::up25ToFlat:   //
            case TrackElemType::flatToDown25: //
            case TrackElemType::up25ToLeftBank:
            case TrackElemType::up25ToRightBank:
            case TrackElemType::leftBankToDown25:
            case TrackElemType::rightBankToDown25:
            case TrackElemType::up25ToFlatCovered:
            case TrackElemType::flatToDown25Covered:
            case TrackElemType::cableLiftHill:
            case TrackElemType::leftBankedUp25ToLeftBankedFlat:
            case TrackElemType::rightBankedUp25ToRightBankedFlat:
            case TrackElemType::leftBankedFlatToLeftBankedDown25:
            case TrackElemType::rightBankedFlatToRightBankedDown25:
            case TrackElemType::leftBankedUp25ToFlat:
            case TrackElemType::rightBankedUp25ToFlat:
            case TrackElemType::flatToLeftBankedDown25:
            case TrackElemType::flatToRightBankedDown25:
                return EvaluatorConst<0>;
            case TrackElemType::up25ToUp60:     //
            case TrackElemType::down60ToDown25: //
            case TrackElemType::up25ToUp60Covered:
            case TrackElemType::down60ToDown25Covered:
                return EvaluatorConst<0>;
            case TrackElemType::up60ToUp25:     //
            case TrackElemType::down25ToDown60: //
            case TrackElemType::up60ToUp25Covered:
            case TrackElemType::down25ToDown60Covered:
                return EvaluatorConst<0>;
            case TrackElemType::leftQuarterTurn5Tiles: //
            case TrackElemType::leftQuarterTurn5TilesUp25:
            case TrackElemType::leftQuarterTurn5TilesDown25:
            case TrackElemType::leftTwistDownToUp:
            case TrackElemType::leftTwistUpToDown:
            case TrackElemType::leftQuarterTurn5TilesCovered:
            case TrackElemType::leftQuarterHelixLargeUp:
            case TrackElemType::leftQuarterHelixLargeDown:
            case TrackElemType::leftFlyerTwistUp:
            case TrackElemType::leftFlyerTwistDown:
            case TrackElemType::leftHeartLineRoll:
                return EvaluatorConst<98>;
            case TrackElemType::rightQuarterTurn5Tiles: //
            case TrackElemType::rightQuarterTurn5TilesUp25:
            case TrackElemType::rightQuarterTurn5TilesDown25:
            case TrackElemType::rightTwistDownToUp:
            case TrackElemType::rightTwistUpToDown:
            case TrackElemType::rightQuarterTurn5TilesCovered:
            case TrackElemType::rightQuarterHelixLargeUp:
            case TrackElemType::rightQuarterHelixLargeDown:
            case TrackElemType::rightFlyerTwistUp:
            case TrackElemType::rightFlyerTwistDown:
            case TrackElemType::rightHeartLineRoll:
                return EvaluatorConst<-98>;
            case TrackElemType::bankedLeftQuarterTurn5Tiles:
            case TrackElemType::leftHalfBankedHelixUpLarge:
            case TrackElemType::leftHalfBankedHelixDownLarge:
            case TrackElemType::leftQuarterBankedHelixLargeUp:
            case TrackElemType::leftQuarterBankedHelixLargeDown:
                return EvaluatorConst<160>;
            case TrackElemType::bankedRightQuarterTurn5Tiles:
            case TrackElemType::rightHalfBankedHelixUpLarge:
            case TrackElemType::rightHalfBankedHelixDownLarge:
            case TrackElemType::rightQuarterBankedHelixLargeUp:
            case TrackElemType::rightQuarterBankedHelixLargeDown:
                return EvaluatorConst<-160>;
            case TrackElemType::sBendLeft:
            case TrackElemType::sBendLeftCovered:
                return EvaluatorSBendLeft;
            case TrackElemType::sBendRight:
            case TrackElemType::sBendRightCovered:
                return EvaluatorSBendRight;
            case TrackElemType::leftVerticalLoop:
            case TrackElemType::rightVerticalLoop:
                return EvaluatorConst<0>;
            case TrackElemType::leftQuarterTurn3Tiles:
            case TrackElemType::leftQuarterTurn3TilesUp25:
            case TrackElemType::leftQuarterTurn3TilesDown25:
            case TrackElemType::leftQuarterTurn3TilesCovered:
            case TrackElemType::leftCurvedLiftHill:
                return EvaluatorConst<59>;
            case TrackElemType::rightQuarterTurn3Tiles:
            case TrackElemType::rightQuarterTurn3TilesUp25:
            case TrackElemType::rightQuarterTurn3TilesDown25:
            case TrackElemType::rightQuarterTurn3TilesCovered:
            case TrackElemType::rightCurvedLiftHill:
                return EvaluatorConst<-59>;
            case TrackElemType::leftBankedQuarterTurn3Tiles:
            case TrackElemType::leftHalfBankedHelixUpSmall:
            case TrackElemType::leftHalfBankedHelixDownSmall:
                return EvaluatorConst<100>;
            case TrackElemType::rightBankedQuarterTurn3Tiles:
            case TrackElemType::rightHalfBankedHelixUpSmall:
            case TrackElemType::rightHalfBankedHelixDownSmall:
                return EvaluatorConst<-100>;
            case TrackElemType::leftQuarterTurn1Tile:
                return EvaluatorConst<45>;
            case TrackElemType::rightQuarterTurn1Tile:
                return EvaluatorConst<-45>;
            case TrackElemType::halfLoopUp:
            case TrackElemType::flyerHalfLoopUninvertedUp:
            case TrackElemType::flyerHalfLoopInvertedUp:
                return EvaluatorConst<0>;
            case TrackElemType::halfLoopDown:
            case TrackElemType::flyerHalfLoopInvertedDown:
            case TrackElemType::flyerHalfLoopUninvertedDown:
                return EvaluatorConst<0>;
            case TrackElemType::leftLargeCorkscrewUp:
            case TrackElemType::rightLargeCorkscrewDown:
                return EvaluatorConst<117>;
            case TrackElemType::rightLargeCorkscrewUp:
            case TrackElemType::leftLargeCorkscrewDown:
                return EvaluatorConst<-117>;
            case TrackElemType::leftCorkscrewUp:
            case TrackElemType::rightCorkscrewDown:
            case TrackElemType::leftFlyerCorkscrewUp:
            case TrackElemType::rightFlyerCorkscrewDown:
                return EvaluatorConst<70>;
            case TrackElemType::rightCorkscrewUp:
            case TrackElemType::leftCorkscrewDown:
            case TrackElemType::rightFlyerCorkscrewUp:
            case TrackElemType::leftFlyerCorkscrewDown:
                return EvaluatorConst<-70>;
            case TrackElemType::flatToUp60:
            case TrackElemType::down60ToFlat:
                return EvaluatorConst<0>;
            case TrackElemType::up60ToFlat:
            case TrackElemType::flatToDown60:
            case TrackElemType::brakeForDrop:
                return EvaluatorConst<0>;
            case TrackElemType::leftQuarterTurn1TileUp60:
            case TrackElemType::leftQuarterTurn1TileDown60:
                return EvaluatorConst<88>;
            case TrackElemType::rightQuarterTurn1TileUp60:
            case TrackElemType::rightQuarterTurn1TileDown60:
                return EvaluatorConst<-88>;
            case TrackElemType::watersplash:
                return EvaluatorConst<0>;
            case TrackElemType::flatToUp60LongBase:
            case TrackElemType::down60ToFlatLongBase:
            case TrackElemType::diagFlatToUp60LongBase:
            case TrackElemType::diagDown60ToFlatLongBase:
                return EvaluatorConst<0>;
            case TrackElemType::up60ToFlatLongBase:
            case TrackElemType::flatToDown60LongBase:
            case TrackElemType::diagUp60ToFlatLongBase:
            case TrackElemType::diagFlatToDown60LongBase:
                return EvaluatorConst<0>;
            case TrackElemType::reverseFreefallSlope:
            case TrackElemType::airThrustVerticalDownToLevel:
                return EvaluatorConst<0>;
            case TrackElemType::up60ToUp90:
            case TrackElemType::down90ToDown60:
                return EvaluatorConst<0>;
            case TrackElemType::up90ToUp60:
            case TrackElemType::down60ToDown90:
                return EvaluatorConst<0>;
            case TrackElemType::leftEighthToDiag:
            case TrackElemType::leftEighthToOrthogonal:
            case TrackElemType::leftEighthToDiagUp25:
            case TrackElemType::leftEighthToDiagDown25:
            case TrackElemType::leftEighthToOrthogonalUp25:
            case TrackElemType::leftEighthToOrthogonalDown25:
                return EvaluatorConst<137>;
            case TrackElemType::rightEighthToDiag:
            case TrackElemType::rightEighthToOrthogonal:
            case TrackElemType::rightEighthToDiagUp25:
            case TrackElemType::rightEighthToDiagDown25:
            case TrackElemType::rightEighthToOrthogonalUp25:
            case TrackElemType::rightEighthToOrthogonalDown25:
                return EvaluatorConst<-137>;
            case TrackElemType::leftEighthBankToDiag:
            case TrackElemType::leftEighthBankToOrthogonal:
            case TrackElemType::leftEighthBankToDiagUp25:
            case TrackElemType::leftEighthBankToDiagDown25:
            case TrackElemType::leftEighthBankToOrthogonalUp25:
            case TrackElemType::leftEighthBankToOrthogonalDown25:
                return EvaluatorConst<200>;
            case TrackElemType::rightEighthBankToDiag:
            case TrackElemType::rightEighthBankToOrthogonal:
            case TrackElemType::rightEighthBankToDiagUp25:
            case TrackElemType::rightEighthBankToDiagDown25:
            case TrackElemType::rightEighthBankToOrthogonalUp25:
            case TrackElemType::rightEighthBankToOrthogonalDown25:
                return EvaluatorConst<-200>;
            case TrackElemType::diagFlatToUp25:
            case TrackElemType::diagDown25ToFlat:
            case TrackElemType::diagLeftBankToUp25:
            case TrackElemType::diagRightBankToUp25:
            case TrackElemType::diagDown25ToLeftBank:
            case TrackElemType::diagDown25ToRightBank:
                return EvaluatorConst<0>;
            case TrackElemType::diagUp25ToFlat:
            case TrackElemType::diagFlatToDown25:
            case TrackElemType::diagUp25ToLeftBank:
            case TrackElemType::diagUp25ToRightBank:
            case TrackElemType::diagLeftBankToDown25:
            case TrackElemType::diagRightBankToDown25:
                return EvaluatorConst<0>;
            case TrackElemType::diagUp25ToUp60:
            case TrackElemType::diagDown60ToDown25:
                return EvaluatorConst<0>;
            case TrackElemType::diagUp60ToUp25:
            case TrackElemType::diagDown25ToDown60:
                return EvaluatorConst<0>;
            case TrackElemType::diagFlatToUp60:
            case TrackElemType::diagDown60ToFlat:
                return EvaluatorConst<0>;
            case TrackElemType::diagUp60ToFlat:
            case TrackElemType::diagFlatToDown60:
                return EvaluatorConst<0>;
            case TrackElemType::leftBarrelRollUpToDown:
            case TrackElemType::leftBarrelRollDownToUp:
                return EvaluatorConst<115>;
            case TrackElemType::rightBarrelRollUpToDown:
            case TrackElemType::rightBarrelRollDownToUp:
                return EvaluatorConst<-115>;
            case TrackElemType::leftZeroGRollUp:
                return EvaluatorZeroGRollUpLeft;
            case TrackElemType::rightZeroGRollUp:
                return EvaluatorZeroGRollUpRight;
            case TrackElemType::leftZeroGRollDown:
                return EvaluatorZeroGRollDownLeft;
            case TrackElemType::rightZeroGRollDown:
                return EvaluatorZeroGRollDownRight;
            case TrackElemType::leftLargeZeroGRollUp:
                return EvaluatorLargeZeroGRollUpLeft;
            case TrackElemType::rightLargeZeroGRollUp:
                return EvaluatorLargeZeroGRollUpRight;
            case TrackElemType::leftLargeZeroGRollDown:
                return EvaluatorLargeZeroGRollDownLeft;
            case TrackElemType::rightLargeZeroGRollDown:
                return EvaluatorLargeZeroGRollDownRight;
            case TrackElemType::leftEighthDiveLoopUpToOrthogonal:
                return EvaluatorDiveLoopUpLeft;
            case TrackElemType::rightEighthDiveLoopUpToOrthogonal:
                return EvaluatorDiveLoopUpRight;
            case TrackElemType::leftEighthDiveLoopDownToDiag:
                return EvaluatorDiveLoopDownLeft;
            case TrackElemType::rightEighthDiveLoopDownToDiag:
                return EvaluatorDiveLoopDownRight;
            case TrackElemType::leftBankToLeftQuarterTurn3TilesUp25:
                return EvaluatorConst<90>;
            case TrackElemType::rightBankToRightQuarterTurn3TilesUp25:
                return EvaluatorConst<-90>;
            case TrackElemType::leftQuarterTurn3TilesDown25ToLeftBank:
                return EvaluatorConst<90>;
            case TrackElemType::rightQuarterTurn3TilesDown25ToRightBank:
                return EvaluatorConst<-90>;
            case TrackElemType::leftLargeHalfLoopUp:
            case TrackElemType::rightLargeHalfLoopUp:
                return EvaluatorConst<0>;
            case TrackElemType::leftLargeHalfLoopDown:
            case TrackElemType::rightLargeHalfLoopDown:
                return EvaluatorConst<0>;
            case TrackElemType::heartLineTransferUp:
                return EvaluatorConst<0>;
            case TrackElemType::heartLineTransferDown:
                return EvaluatorConst<0>;
            case TrackElemType::multiDimInvertedFlatToDown90QuarterLoop:
            case TrackElemType::invertedFlatToDown90QuarterLoop:
            case TrackElemType::multiDimFlatToDown90QuarterLoop:
                return EvaluatorConst<0>;
            case TrackElemType::up90ToInvertedFlatQuarterLoop:
            case TrackElemType::multiDimUp90ToInvertedFlatQuarterLoop:
            case TrackElemType::multiDimInvertedUp90ToFlatQuarterLoop:
                return EvaluatorConst<0>;
            case TrackElemType::airThrustTopCap:
                return EvaluatorConst<0>;
            case TrackElemType::leftBankedQuarterTurn3TileUp25:
            case TrackElemType::leftBankedQuarterTurn3TileDown25:
                return EvaluatorConst<100>;
            case TrackElemType::rightBankedQuarterTurn3TileUp25:
            case TrackElemType::rightBankedQuarterTurn3TileDown25:
                return EvaluatorConst<-100>;
            case TrackElemType::leftBankedQuarterTurn5TileUp25:
            case TrackElemType::leftBankedQuarterTurn5TileDown25:
                return EvaluatorConst<160>;
            case TrackElemType::rightBankedQuarterTurn5TileUp25:
            case TrackElemType::rightBankedQuarterTurn5TileDown25:
                return EvaluatorConst<-160>;
            default:
                return EvaluatorConst<0>;
        }
    }

    static constexpr TrackComputeFunction GetVerticalFunction(TrackElemType type)
    {
        switch (type)
        {
            case TrackElemType::flat:
            case TrackElemType::endStation:
            case TrackElemType::beginStation:
            case TrackElemType::middleStation:
            case TrackElemType::up25:
            case TrackElemType::up60: //
            case TrackElemType::down25:
            case TrackElemType::down60: //
            case TrackElemType::flatToLeftBank:
            case TrackElemType::flatToRightBank:
            case TrackElemType::leftBankToFlat:
            case TrackElemType::rightBankToFlat: //
            case TrackElemType::leftBank:
            case TrackElemType::rightBank:
            case TrackElemType::towerBase:
            case TrackElemType::towerSection:
            case TrackElemType::flatCovered:
            case TrackElemType::up25Covered:
            case TrackElemType::up60Covered:
            case TrackElemType::down25Covered:
            case TrackElemType::down60Covered:
            case TrackElemType::brakes:
            case TrackElemType::rotationControlToggle:
            case TrackElemType::maze:
            case TrackElemType::up25LeftBanked:
            case TrackElemType::up25RightBanked:
            case TrackElemType::waterfall:
            case TrackElemType::rapids:
            case TrackElemType::onRidePhoto:
            case TrackElemType::down25LeftBanked:
            case TrackElemType::down25RightBanked:
            case TrackElemType::whirlpool:
            case TrackElemType::reverseFreefallVertical:
            case TrackElemType::up90:
            case TrackElemType::down90:
            case TrackElemType::diagFlat:
            case TrackElemType::diagUp25:
            case TrackElemType::diagUp60:
            case TrackElemType::diagDown25:
            case TrackElemType::diagDown60:
            case TrackElemType::diagFlatToLeftBank:
            case TrackElemType::diagFlatToRightBank:
            case TrackElemType::diagLeftBankToFlat:
            case TrackElemType::diagRightBankToFlat:
            case TrackElemType::diagLeftBank:
            case TrackElemType::diagRightBank:
            case TrackElemType::logFlumeReverser:
            case TrackElemType::spinningTunnel:
            case TrackElemType::poweredLift:
            case TrackElemType::minigolfHoleA:
            case TrackElemType::minigolfHoleB:
            case TrackElemType::minigolfHoleC:
            case TrackElemType::minigolfHoleD:
            case TrackElemType::minigolfHoleE:
            case TrackElemType::leftReverser:
            case TrackElemType::rightReverser:
            case TrackElemType::airThrustVerticalDown:
            case TrackElemType::blockBrakes:
            case TrackElemType::up25ToLeftBankedUp25:
            case TrackElemType::up25ToRightBankedUp25:
            case TrackElemType::leftBankedUp25ToUp25:
            case TrackElemType::rightBankedUp25ToUp25:
            case TrackElemType::down25ToLeftBankedDown25:
            case TrackElemType::down25ToRightBankedDown25:
            case TrackElemType::leftBankedDown25ToDown25:
            case TrackElemType::rightBankedDown25ToDown25:
            case TrackElemType::leftQuarterTurn1TileUp90:
            case TrackElemType::rightQuarterTurn1TileUp90:
            case TrackElemType::leftQuarterTurn1TileDown90:
            case TrackElemType::rightQuarterTurn1TileDown90:
            case TrackElemType::leftEighthToDiagUp25:
            case TrackElemType::rightEighthToDiagUp25:
            case TrackElemType::leftEighthToDiagDown25:
            case TrackElemType::rightEighthToDiagDown25:
            case TrackElemType::leftEighthToOrthogonalUp25:
            case TrackElemType::rightEighthToOrthogonalUp25:
            case TrackElemType::leftEighthToOrthogonalDown25:
            case TrackElemType::rightEighthToOrthogonalDown25:
            case TrackElemType::diagUp25ToLeftBankedUp25:
            case TrackElemType::diagUp25ToRightBankedUp25:
            case TrackElemType::diagLeftBankedUp25ToUp25:
            case TrackElemType::diagRightBankedUp25ToUp25:
            case TrackElemType::diagDown25ToLeftBankedDown25:
            case TrackElemType::diagDown25ToRightBankedDown25:
            case TrackElemType::diagLeftBankedDown25ToDown25:
            case TrackElemType::diagRightBankedDown25ToDown25:
            case TrackElemType::diagUp25LeftBanked:
            case TrackElemType::diagUp25RightBanked:
            case TrackElemType::diagDown25LeftBanked:
            case TrackElemType::diagDown25RightBanked:
                return EvaluatorConst<0>;
            case TrackElemType::flatToUp25:   //
            case TrackElemType::down25ToFlat: //
            case TrackElemType::leftBankToUp25:
            case TrackElemType::rightBankToUp25:
            case TrackElemType::down25ToLeftBank:
            case TrackElemType::down25ToRightBank:
            case TrackElemType::flatToUp25Covered:
            case TrackElemType::down25ToFlatCovered:
            case TrackElemType::leftBankedFlatToLeftBankedUp25:
            case TrackElemType::rightBankedFlatToRightBankedUp25:
            case TrackElemType::leftBankedDown25ToLeftBankedFlat:
            case TrackElemType::rightBankedDown25ToRightBankedFlat:
            case TrackElemType::flatToLeftBankedUp25:
            case TrackElemType::flatToRightBankedUp25:
            case TrackElemType::leftBankedDown25ToFlat:
            case TrackElemType::rightBankedDown25ToFlat:
                return EvaluatorConst<103>;
            case TrackElemType::up25ToFlat:   //
            case TrackElemType::flatToDown25: //
            case TrackElemType::up25ToLeftBank:
            case TrackElemType::up25ToRightBank:
            case TrackElemType::leftBankToDown25:
            case TrackElemType::rightBankToDown25:
            case TrackElemType::up25ToFlatCovered:
            case TrackElemType::flatToDown25Covered:
            case TrackElemType::cableLiftHill:
            case TrackElemType::leftBankedUp25ToLeftBankedFlat:
            case TrackElemType::rightBankedUp25ToRightBankedFlat:
            case TrackElemType::leftBankedFlatToLeftBankedDown25:
            case TrackElemType::rightBankedFlatToRightBankedDown25:
            case TrackElemType::leftBankedUp25ToFlat:
            case TrackElemType::rightBankedUp25ToFlat:
            case TrackElemType::flatToLeftBankedDown25:
            case TrackElemType::flatToRightBankedDown25:
                return EvaluatorConst<-103>;
            case TrackElemType::up25ToUp60:     //
            case TrackElemType::down60ToDown25: //
            case TrackElemType::up25ToUp60Covered:
            case TrackElemType::down60ToDown25Covered:
                return EvaluatorConst<82>;
            case TrackElemType::up60ToUp25:     //
            case TrackElemType::down25ToDown60: //
            case TrackElemType::up60ToUp25Covered:
            case TrackElemType::down25ToDown60Covered:
                return EvaluatorConst<-82>;
            case TrackElemType::leftQuarterTurn5Tiles: //
            case TrackElemType::leftQuarterTurn5TilesUp25:
            case TrackElemType::leftQuarterTurn5TilesDown25:
            case TrackElemType::leftTwistDownToUp:
            case TrackElemType::leftTwistUpToDown:
            case TrackElemType::leftQuarterTurn5TilesCovered:
            case TrackElemType::leftQuarterHelixLargeUp:
            case TrackElemType::leftQuarterHelixLargeDown:
            case TrackElemType::leftFlyerTwistUp:
            case TrackElemType::leftFlyerTwistDown:
            case TrackElemType::leftHeartLineRoll:
                return EvaluatorConst<0>;
            case TrackElemType::rightQuarterTurn5Tiles: //
            case TrackElemType::rightQuarterTurn5TilesUp25:
            case TrackElemType::rightQuarterTurn5TilesDown25:
            case TrackElemType::rightTwistDownToUp:
            case TrackElemType::rightTwistUpToDown:
            case TrackElemType::rightQuarterTurn5TilesCovered:
            case TrackElemType::rightQuarterHelixLargeUp:
            case TrackElemType::rightQuarterHelixLargeDown:
            case TrackElemType::rightFlyerTwistUp:
            case TrackElemType::rightFlyerTwistDown:
            case TrackElemType::rightHeartLineRoll:
                return EvaluatorConst<0>;
            case TrackElemType::bankedLeftQuarterTurn5Tiles:
            case TrackElemType::leftHalfBankedHelixUpLarge:
            case TrackElemType::leftHalfBankedHelixDownLarge:
            case TrackElemType::leftQuarterBankedHelixLargeUp:
            case TrackElemType::leftQuarterBankedHelixLargeDown:
                return EvaluatorConst<200>;
            case TrackElemType::bankedRightQuarterTurn5Tiles:
            case TrackElemType::rightHalfBankedHelixUpLarge:
            case TrackElemType::rightHalfBankedHelixDownLarge:
            case TrackElemType::rightQuarterBankedHelixLargeUp:
            case TrackElemType::rightQuarterBankedHelixLargeDown:
                return EvaluatorConst<200>;
            case TrackElemType::sBendLeft:
            case TrackElemType::sBendLeftCovered:
                return EvaluatorConst<0>;
            case TrackElemType::sBendRight:
            case TrackElemType::sBendRightCovered:
                return EvaluatorConst<0>;
            case TrackElemType::leftVerticalLoop:
            case TrackElemType::rightVerticalLoop:
                return EvaluatorVerticalLoop;
            case TrackElemType::leftQuarterTurn3Tiles:
            case TrackElemType::leftQuarterTurn3TilesUp25:
            case TrackElemType::leftQuarterTurn3TilesDown25:
            case TrackElemType::leftQuarterTurn3TilesCovered:
            case TrackElemType::leftCurvedLiftHill:
                return EvaluatorConst<0>;
            case TrackElemType::rightQuarterTurn3Tiles:
            case TrackElemType::rightQuarterTurn3TilesUp25:
            case TrackElemType::rightQuarterTurn3TilesDown25:
            case TrackElemType::rightQuarterTurn3TilesCovered:
            case TrackElemType::rightCurvedLiftHill:
                return EvaluatorConst<0>;
            case TrackElemType::leftBankedQuarterTurn3Tiles:
            case TrackElemType::leftHalfBankedHelixUpSmall:
            case TrackElemType::leftHalfBankedHelixDownSmall:
                return EvaluatorConst<100>;
            case TrackElemType::rightBankedQuarterTurn3Tiles:
            case TrackElemType::rightHalfBankedHelixUpSmall:
            case TrackElemType::rightHalfBankedHelixDownSmall:
                return EvaluatorConst<100>;
            case TrackElemType::leftQuarterTurn1Tile:
                return EvaluatorConst<0>;
            case TrackElemType::rightQuarterTurn1Tile:
                return EvaluatorConst<0>;
            case TrackElemType::halfLoopUp:
            case TrackElemType::flyerHalfLoopUninvertedUp:
            case TrackElemType::flyerHalfLoopInvertedUp:
                return EvaluatorHalfLoopUp;
            case TrackElemType::halfLoopDown:
            case TrackElemType::flyerHalfLoopInvertedDown:
            case TrackElemType::flyerHalfLoopUninvertedDown:
                return EvaluatorHalfLoopDown;
            case TrackElemType::leftLargeCorkscrewUp:
            case TrackElemType::rightLargeCorkscrewDown:
                return EvaluatorConst<89>;
            case TrackElemType::rightLargeCorkscrewUp:
            case TrackElemType::leftLargeCorkscrewDown:
                return EvaluatorConst<89>;
            case TrackElemType::leftCorkscrewUp:
            case TrackElemType::rightCorkscrewDown:
            case TrackElemType::leftFlyerCorkscrewUp:
            case TrackElemType::rightFlyerCorkscrewDown:
                return EvaluatorConst<52>;
            case TrackElemType::rightCorkscrewUp:
            case TrackElemType::leftCorkscrewDown:
            case TrackElemType::rightFlyerCorkscrewUp:
            case TrackElemType::leftFlyerCorkscrewDown:
                return EvaluatorConst<52>;
            case TrackElemType::flatToUp60:
            case TrackElemType::down60ToFlat:
                return EvaluatorConst<56>;
            case TrackElemType::up60ToFlat:
            case TrackElemType::flatToDown60:
            case TrackElemType::brakeForDrop:
                return EvaluatorConst<-56>;
            case TrackElemType::leftQuarterTurn1TileUp60:
            case TrackElemType::leftQuarterTurn1TileDown60:
                return EvaluatorConst<0>;
            case TrackElemType::rightQuarterTurn1TileUp60:
            case TrackElemType::rightQuarterTurn1TileDown60:
                return EvaluatorConst<0>;
            case TrackElemType::watersplash:
                return EvaluatorWaterSplash;
            case TrackElemType::flatToUp60LongBase:
            case TrackElemType::down60ToFlatLongBase:
                return EvaluatorConst<160>;
            case TrackElemType::up60ToFlatLongBase:
            case TrackElemType::flatToDown60LongBase:
                return EvaluatorConst<-160>;
            case TrackElemType::diagFlatToUp60LongBase:
            case TrackElemType::diagDown60ToFlatLongBase:
                return EvaluatorConst<180>;
            case TrackElemType::diagUp60ToFlatLongBase:
            case TrackElemType::diagFlatToDown60LongBase:
                return EvaluatorConst<-180>;
            case TrackElemType::reverseFreefallSlope:
            case TrackElemType::airThrustVerticalDownToLevel:
                return EvaluatorConst<120>;
            case TrackElemType::up60ToUp90:
            case TrackElemType::down90ToDown60:
                return EvaluatorConst<110>;
            case TrackElemType::up90ToUp60:
            case TrackElemType::down60ToDown90:
                return EvaluatorConst<-110>;
            case TrackElemType::leftEighthToDiag:
            case TrackElemType::leftEighthToOrthogonal:
                return EvaluatorConst<0>;
            case TrackElemType::rightEighthToDiag:
            case TrackElemType::rightEighthToOrthogonal:
                return EvaluatorConst<0>;
            case TrackElemType::leftEighthBankToDiag:
            case TrackElemType::leftEighthBankToOrthogonal:
            case TrackElemType::leftEighthBankToDiagUp25:
            case TrackElemType::leftEighthBankToDiagDown25:
            case TrackElemType::leftEighthBankToOrthogonalUp25:
            case TrackElemType::leftEighthBankToOrthogonalDown25:
                return EvaluatorConst<270>;
            case TrackElemType::rightEighthBankToDiag:
            case TrackElemType::rightEighthBankToOrthogonal:
            case TrackElemType::rightEighthBankToDiagUp25:
            case TrackElemType::rightEighthBankToDiagDown25:
            case TrackElemType::rightEighthBankToOrthogonalUp25:
            case TrackElemType::rightEighthBankToOrthogonalDown25:
                return EvaluatorConst<270>;
            case TrackElemType::diagFlatToUp25:
            case TrackElemType::diagDown25ToFlat:
            case TrackElemType::diagLeftBankToUp25:
            case TrackElemType::diagRightBankToUp25:
            case TrackElemType::diagDown25ToLeftBank:
            case TrackElemType::diagDown25ToRightBank:
            case TrackElemType::diagLeftBankedFlatToLeftBankedUp25:
            case TrackElemType::diagRightBankedFlatToRightBankedUp25:
            case TrackElemType::diagLeftBankedDown25ToLeftBankedFlat:
            case TrackElemType::diagRightBankedDown25ToRightBankedFlat:
            case TrackElemType::diagFlatToLeftBankedUp25:
            case TrackElemType::diagFlatToRightBankedUp25:
            case TrackElemType::diagLeftBankedDown25ToFlat:
            case TrackElemType::diagRightBankedDown25ToFlat:
                return EvaluatorConst<113>;
            case TrackElemType::diagUp25ToFlat:
            case TrackElemType::diagFlatToDown25:
            case TrackElemType::diagUp25ToLeftBank:
            case TrackElemType::diagUp25ToRightBank:
            case TrackElemType::diagLeftBankToDown25:
            case TrackElemType::diagRightBankToDown25:
            case TrackElemType::diagLeftBankedUp25ToLeftBankedFlat:
            case TrackElemType::diagRightBankedUp25ToRightBankedFlat:
            case TrackElemType::diagLeftBankedFlatToLeftBankedDown25:
            case TrackElemType::diagRightBankedFlatToRightBankedDown25:
            case TrackElemType::diagLeftBankedUp25ToFlat:
            case TrackElemType::diagRightBankedUp25ToFlat:
            case TrackElemType::diagFlatToLeftBankedDown25:
            case TrackElemType::diagFlatToRightBankedDown25:
                return EvaluatorConst<-113>;
            case TrackElemType::diagUp25ToUp60:
            case TrackElemType::diagDown60ToDown25:
                return EvaluatorConst<95>;
            case TrackElemType::diagUp60ToUp25:
            case TrackElemType::diagDown25ToDown60:
                return EvaluatorConst<-95>;
            case TrackElemType::diagFlatToUp60:
            case TrackElemType::diagDown60ToFlat:
                return EvaluatorConst<60>;
            case TrackElemType::diagUp60ToFlat:
            case TrackElemType::diagFlatToDown60:
                return EvaluatorConst<-60>;
            case TrackElemType::leftBarrelRollUpToDown:
            case TrackElemType::leftBarrelRollDownToUp:
                return EvaluatorConst<170>;
            case TrackElemType::rightBarrelRollUpToDown:
            case TrackElemType::rightBarrelRollDownToUp:
                return EvaluatorConst<170>;
            case TrackElemType::leftZeroGRollUp:
            case TrackElemType::rightZeroGRollUp:
                return EvaluatorConst<250>;
            case TrackElemType::leftZeroGRollDown:
            case TrackElemType::rightZeroGRollDown:
                return EvaluatorConst<250>;
            case TrackElemType::leftLargeZeroGRollUp:
            case TrackElemType::rightLargeZeroGRollUp:
                return EvaluatorLargeZeroGRollUp;
            case TrackElemType::leftLargeZeroGRollDown:
            case TrackElemType::rightLargeZeroGRollDown:
                return EvaluatorLargeZeroGRollDown;
            case TrackElemType::leftEighthDiveLoopUpToOrthogonal:
            case TrackElemType::rightEighthDiveLoopUpToOrthogonal:
                return EvaluatorDiveLoopUp;
            case TrackElemType::leftEighthDiveLoopDownToDiag:
            case TrackElemType::rightEighthDiveLoopDownToDiag:
                return EvaluatorDiveLoopDown;
            case TrackElemType::leftBankToLeftQuarterTurn3TilesUp25:
            case TrackElemType::rightBankToRightQuarterTurn3TilesUp25:
            case TrackElemType::leftQuarterTurn3TilesDown25ToLeftBank:
            case TrackElemType::rightQuarterTurn3TilesDown25ToRightBank:
                return EvaluatorQuarterTurn3Tiles;
            case TrackElemType::leftMediumHalfLoopUp:
            case TrackElemType::rightMediumHalfLoopUp:
                return EvaluatorMediumHalfLoopUp;
            case TrackElemType::rightMediumHalfLoopDown:
            case TrackElemType::leftMediumHalfLoopDown:
                return EvaluatorMediumHalfLoopDown;
            case TrackElemType::leftLargeHalfLoopUp:
            case TrackElemType::rightLargeHalfLoopUp:
            case TrackElemType::leftFlyerLargeHalfLoopUninvertedUp:
            case TrackElemType::rightFlyerLargeHalfLoopUninvertedUp:
            case TrackElemType::leftFlyerLargeHalfLoopInvertedUp:
            case TrackElemType::rightFlyerLargeHalfLoopInvertedUp:
                return EvaluatorLargeHalfLoopUp;
            case TrackElemType::leftLargeHalfLoopDown:
            case TrackElemType::rightLargeHalfLoopDown:
            case TrackElemType::leftFlyerLargeHalfLoopInvertedDown:
            case TrackElemType::rightFlyerLargeHalfLoopInvertedDown:
            case TrackElemType::leftFlyerLargeHalfLoopUninvertedDown:
            case TrackElemType::rightFlyerLargeHalfLoopUninvertedDown:
                return EvaluatorLargeHalfLoopDown;
            case TrackElemType::heartLineTransferUp:
                return EvaluatorHeartLineTransferUp;
            case TrackElemType::heartLineTransferDown:
                return EvaluatorHeartLineTransferDown;
            case TrackElemType::multiDimInvertedFlatToDown90QuarterLoop:
            case TrackElemType::invertedFlatToDown90QuarterLoop:
            case TrackElemType::multiDimFlatToDown90QuarterLoop:
                return EvaluatorDown90QuarterLoop;
            case TrackElemType::up90ToInvertedFlatQuarterLoop:
            case TrackElemType::multiDimUp90ToInvertedFlatQuarterLoop:
            case TrackElemType::multiDimInvertedUp90ToFlatQuarterLoop:
                return EvaluatorUp90QuarterLoop;
            case TrackElemType::airThrustTopCap:
                return EvaluatorConst<-60>;
            case TrackElemType::leftBankedQuarterTurn3TileUp25:
            case TrackElemType::leftBankedQuarterTurn3TileDown25:
                return EvaluatorConst<200>;
            case TrackElemType::rightBankedQuarterTurn3TileUp25:
            case TrackElemType::rightBankedQuarterTurn3TileDown25:
                return EvaluatorConst<200>;
            case TrackElemType::leftBankedQuarterTurn5TileUp25:
            case TrackElemType::leftBankedQuarterTurn5TileDown25:
                return EvaluatorConst<200>;
            case TrackElemType::rightBankedQuarterTurn5TileUp25:
            case TrackElemType::rightBankedQuarterTurn5TileDown25:
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
        kStringIdEmpty,                    // TrackElemType::up25LeftBanked
        kStringIdEmpty,                    // TrackElemType::up25RighBanked
        STR_WATERFALLS,                    // 112
        STR_RAPIDS,                        // 113
        STR_ON_RIDE_PHOTO_SECTION,         // 114
        kStringIdEmpty,                    // TrackElemType::down25LeftBanked
        kStringIdEmpty,                    // TrackElemType::down25RightBanked
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
        kStringIdEmpty,                    // TrackElemType::flatTrack1x4A
        STR_BASE_SIZE_2_X_2,               // TrackElemType::flatTrack2x2
        STR_BASE_SIZE_4_X_4,               // TrackElemType::flatTrack4x4
        STR_BASE_SIZE_2_X_4,               // TrackElemType::flatTrack2x4
        STR_BASE_SIZE_5_X_1,               // TrackElemType::flatTrack1x5
        kStringIdEmpty,                    // TrackElemType::flatTrack1x1A
        kStringIdEmpty,                    // TrackElemType::flatTrack1x4B
        kStringIdEmpty,                    // TrackElemType::flatTrack1x1B
        kStringIdEmpty,                    // TrackElemType::flatTrack1x4C
        kStringIdEmpty,                    // TrackElemType::flatTrack3x3
        STR_LARGE_HALF_CORKSCREW_LEFT,     // TrackElemType::leftCorkscrewUp
        STR_LARGE_HALF_CORKSCREW_RIGHT,    // TrackElemType::rightCorkscrewUp
        STR_LARGE_HALF_CORKSCREW_LEFT,     // TrackElemType::leftCorkscrewDown
        STR_LARGE_HALF_CORKSCREW_RIGHT,    // TrackElemType::rightCorkscrewDown
        STR_MEDIUM_HALF_LOOP_LEFT,         // TrackElemType::leftMediumHalfLoopUp
        STR_MEDIUM_HALF_LOOP_RIGHT,        // TrackElemType::rightMediumHalfLoopUp
        STR_MEDIUM_HALF_LOOP_LEFT,         // TrackElemType::leftMediumHalfLoopDown
        STR_MEDIUM_HALF_LOOP_RIGHT,        // TrackElemType::rightMediumHalfLoopDown
        STR_ZERO_G_ROLL_LEFT,              // TrackElemType::leftZeroGRollUp
        STR_ZERO_G_ROLL_RIGHT,             // TrackElemType::rightZeroGRollUp
        STR_ZERO_G_ROLL_LEFT,              // TrackElemType::leftZeroGRollDown
        STR_ZERO_G_ROLL_RIGHT,             // TrackElemType::rightZeroGRollDown
        STR_LARGE_ZERO_G_ROLL_LEFT,        // TrackElemType::leftLargeZeroGRollUp
        STR_LARGE_ZERO_G_ROLL_RIGHT,       // TrackElemType::rightLargeZeroGRollUp
        STR_LARGE_ZERO_G_ROLL_LEFT,        // TrackElemType::leftLargeZeroGRollDown
        STR_LARGE_ZERO_G_ROLL_RIGHT,       // TrackElemType::rightLargeZeroGRollDown
        STR_LARGE_HALF_LOOP_LEFT,          // TrackElemType::leftFlyerLargeHalfLoopUninvertedUp
        STR_LARGE_HALF_LOOP_RIGHT,         // TrackElemType::rightFlyerLargeHalfLoopUninvertedUp
        STR_LARGE_HALF_LOOP_LEFT,          // TrackElemType::leftFlyerLargeHalfLoopInvertedDown
        STR_LARGE_HALF_LOOP_RIGHT,         // TrackElemType::rightFlyerLargeHalfLoopInvertedDown
        STR_LARGE_HALF_LOOP_LEFT,          // TrackElemType::leftFlyerLargeHalfLoopInvertedUp
        STR_LARGE_HALF_LOOP_RIGHT,         // TrackElemType::rightFlyerLargeHalfLoopInvertedUp
        STR_LARGE_HALF_LOOP_LEFT,          // TrackElemType::leftFlyerLargeHalfLoopUninvertedDown
        STR_LARGE_HALF_LOOP_RIGHT,         // TrackElemType::rightFlyerLargeHalfLoopUninvertedDown
        STR_HALF_LOOP,                     // TrackElemType::flyerHalfLoopInvertedUp
        STR_HALF_LOOP,                     // TrackElemType::flyerHalfLoopUninvertedDown
        kStringIdEmpty,                    // TrackElemType::leftEighthToDiagUp25
        kStringIdEmpty,                    // TrackElemType::rightEighthToDiagUp25
        kStringIdEmpty,                    // TrackElemType::leftEighthToDiagDown25
        kStringIdEmpty,                    // TrackElemType::rightEighthToDiagDown25
        kStringIdEmpty,                    // TrackElemType::leftEighthToOrthogonalUp25
        kStringIdEmpty,                    // TrackElemType::rightEighthToOrthogonalUp25
        kStringIdEmpty,                    // TrackElemType::leftEighthToOrthogonalDown25
        kStringIdEmpty,                    // TrackElemType::rightEighthToOrthogonalDown25
        kStringIdEmpty,                    // TrackElemType::diagUp25ToLeftBankedUp25
        kStringIdEmpty,                    // TrackElemType::diagUp25ToRightBankedUp25
        kStringIdEmpty,                    // TrackElemType::diagLeftBankedUp25ToUp25
        kStringIdEmpty,                    // TrackElemType::diagRightBankedUp25ToUp25
        kStringIdEmpty,                    // TrackElemType::diagDown25ToLeftBankedDown25
        kStringIdEmpty,                    // TrackElemType::diagDown25ToRightBankedDown25
        kStringIdEmpty,                    // TrackElemType::diagLeftBankedDown25ToDown25
        kStringIdEmpty,                    // TrackElemType::diagRightBankedDown25ToDown25
        kStringIdEmpty,                    // TrackElemType::diagLeftBankedFlatToLeftBankedUp25
        kStringIdEmpty,                    // TrackElemType::diagRightBankedFlatToRightBankedUp25
        kStringIdEmpty,                    // TrackElemType::diagLeftBankedUp25ToLeftBankedFlat
        kStringIdEmpty,                    // TrackElemType::diagRightBankedUp25ToRightBankedFlat
        kStringIdEmpty,                    // TrackElemType::diagLeftBankedFlatToLeftBankedDown25
        kStringIdEmpty,                    // TrackElemType::diagRightBankedFlatToRightBankedDown25
        kStringIdEmpty,                    // TrackElemType::diagLeftBankedDown25ToLeftBankedFlat
        kStringIdEmpty,                    // TrackElemType::diagRightBankedDown25ToRightBankedFlat
        kStringIdEmpty,                    // TrackElemType::diagFlatToLeftBankedUp25
        kStringIdEmpty,                    // TrackElemType::diagFlatToRightBankedUp25
        kStringIdEmpty,                    // TrackElemType::diagLeftBankedUp25ToFlat
        kStringIdEmpty,                    // TrackElemType::diagRightBankedUp25ToFlat
        kStringIdEmpty,                    // TrackElemType::diagFlatToLeftBankedDown25
        kStringIdEmpty,                    // TrackElemType::diagFlatToRightBankedDown25
        kStringIdEmpty,                    // TrackElemType::diagLeftBankedDown25ToFlat
        kStringIdEmpty,                    // TrackElemType::diagRightBankedDown25ToFlat
        kStringIdEmpty,                    // TrackElemType::up25LeftBanked
        kStringIdEmpty,                    // TrackElemType::up25RightBanked
        kStringIdEmpty,                    // TrackElemType::down25LeftBanked
        kStringIdEmpty,                    // TrackElemType::down25RightBanked
        kStringIdEmpty,                    // TrackElemType::leftEighthBankToDiagUp25
        kStringIdEmpty,                    // TrackElemType::rightEighthBankToDiagUp25
        kStringIdEmpty,                    // TrackElemType::leftEighthBankToDiagDown25
        kStringIdEmpty,                    // TrackElemType::rightEighthBankToDiagDown25
        kStringIdEmpty,                    // TrackElemType::leftEighthBankToOrthogonalUp25
        kStringIdEmpty,                    // TrackElemType::rightEighthBankToOrthogonalUp25
        kStringIdEmpty,                    // TrackElemType::leftEighthBankToOrthogonalDown25
        kStringIdEmpty,                    // TrackElemType::rightEighthBankToOrthogonalDown25
        STR_BRAKES,                        // TrackElemType::diagBrakes
        STR_BLOCK_BRAKES,                  // TrackElemType::diagBlockBrakes
        STR_BRAKES,                        // TrackElemType::down25Brakes
        STR_BOOSTER,                       // TrackElemType::diagBooster
        kStringIdEmpty,                    // TrackElemType::diagFlatToUp60LongBase,
        kStringIdEmpty,                    // TrackElemType::diagUp60ToFlatLongBase,
        kStringIdEmpty,                    // TrackElemType::diagFlatToDown60LongBase,
        kStringIdEmpty,                    // TrackElemType::diagDown60ToFlatLongBase,
        STR_DIVE_LOOP_LEFT,                // TrackElemType::leftEighthDiveLoopUpToOrthogonal
        STR_DIVE_LOOP_RIGHT,               // TrackElemType::rightEighthDiveLoopUpToOrthogonal
        STR_DIVE_LOOP_LEFT,                // TrackElemType::leftEighthDiveLoopDownToDiag
        STR_DIVE_LOOP_RIGHT,               // TrackElemType::rightEighthDiveLoopDownToDiag
        STR_BRAKES,                        // TrackElemType::diagDown25Brakes
    };
    static_assert(std::size(kRideConfigurationStringIds) == EnumValue(TrackElemType::count));

#pragma region trackBlocks

    using PS = PaintSegment;

    static constexpr SequenceDescriptor kFlatSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kEndStationSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin,
                   SequenceFlag::disallowDoors, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = blockedSegmentsAllTypes(kSegmentsAll),
    };

    static constexpr SequenceDescriptor kBeginStationSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin,
                   SequenceFlag::disallowDoors, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = blockedSegmentsAllTypes(kSegmentsAll),
    };

    static constexpr SequenceDescriptor kMiddleStationSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin,
                   SequenceFlag::disallowDoors, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = blockedSegmentsAllTypes(kSegmentsAll),
    };

    static constexpr SequenceDescriptor kUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp60Seq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp25ToUp60Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToUp60Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp60ToUp25Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToUp25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown60Seq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown25ToDown60Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToUp25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown60ToDown25Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToUp60Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft),                  // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::null },
        .blockedSegments = { {
            kSegmentsNone,                                          // narrow
            kSegmentsNone,                                          // inverted
            EnumsToFlags(PS::right, PS::topRight, PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsAllTypes(
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft)),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            EnumsToFlags(
                PS::top, PS::right, PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft,
                PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesSeq4 = {
        .clearance = { -32, -64, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::null },
        .extraSupportRotation = -1,
        .blockedSegments = { {
            kSegmentsNone,                                          // narrow
            kSegmentsNone,                                          // inverted
            EnumsToFlags(PS::right, PS::topRight, PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesSeq5 = {
        .clearance = { -64, -32, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .extraSupportRotation = -1,
        .blockedSegments = blockedSegmentsAllTypes(
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight)),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesSeq6 = {
        .clearance = { -64, -64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomRight),               // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn5TilesSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::null },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn5TilesSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn5TilesSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn5TilesSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesSeq4 = {
        .clearance = { -32, 64, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::null },
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn5TilesSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesSeq5 = {
        .clearance = { -64, 32, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn5TilesSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesSeq6 = {
        .clearance = { -64, 64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn5TilesSeq6.blockedSegments),
    };

    static constexpr SequenceDescriptor kFlatToLeftBankSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft),                                         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                                   // wide
        } },
    };

    static constexpr SequenceDescriptor kFlatToRightBankSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = blockedSegmentsFlipXAxis(kFlatToLeftBankSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankToFlatSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft),                                         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                                   // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankToFlatSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankToFlatSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kBankedLeftQuarterTurn5TilesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kLeftQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedLeftQuarterTurn5TilesSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::null },
        .blockedSegments = kLeftQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedLeftQuarterTurn5TilesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kLeftQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedLeftQuarterTurn5TilesSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kLeftQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedLeftQuarterTurn5TilesSeq4 = {
        .clearance = { -32, -64, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::null },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedLeftQuarterTurn5TilesSeq5 = {
        .clearance = { -64, -32, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedLeftQuarterTurn5TilesSeq6 = {
        .clearance = { -64, -64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedRightQuarterTurn5TilesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kRightQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedRightQuarterTurn5TilesSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::null },
        .blockedSegments = kRightQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedRightQuarterTurn5TilesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kRightQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedRightQuarterTurn5TilesSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kRightQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedRightQuarterTurn5TilesSeq4 = {
        .clearance = { -32, 64, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::null },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedRightQuarterTurn5TilesSeq5 = {
        .clearance = { -64, 32, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedRightQuarterTurn5TilesSeq6 = {
        .clearance = { -64, 64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftBankToUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft),                                         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                                   // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankToUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankToUp25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kUp25ToLeftBankSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft),                                         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                                   // wide
        } },
    };

    static constexpr SequenceDescriptor kUp25ToRightBankSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = blockedSegmentsFlipXAxis(kUp25ToLeftBankSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankToDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft),                                         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                                   // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankToDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankToDown25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kDown25ToLeftBankSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft),                                         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                                   // wide
        } },
    };

    static constexpr SequenceDescriptor kDown25ToRightBankSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = blockedSegmentsFlipXAxis(kDown25ToLeftBankSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft),                                         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                                   // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kLeftQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesUp25Seq1 = {
        .clearance = { 0, -32, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesUp25Seq2 = {
        .clearance = { -32, 0, 16, 16, { 0b0111, 0b0100 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kLeftQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesUp25Seq3 = {
        .clearance = { -32, -32, 24, 16, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kLeftQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesUp25Seq4 = {
        .clearance = { -32, -64, 48, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesUp25Seq5 = {
        .clearance = { -64, -32, 32, 16, { 0b0111, 0b0100 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesUp25Seq6 = {
        .clearance = { -64, -64, 48, 16, { 0b1111, 0b0110 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kRightQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesUp25Seq1 = {
        .clearance = { 0, 32, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesUp25Seq2 = {
        .clearance = { -32, 0, 16, 16, { 0b1011, 0b1000 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kRightQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesUp25Seq3 = {
        .clearance = { -32, 32, 24, 16, { 0b1110, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kRightQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesUp25Seq4 = {
        .clearance = { -32, 64, 48, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesUp25Seq5 = {
        .clearance = { -64, 32, 32, 16, { 0b1011, 0b1000 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesUp25Seq6 = {
        .clearance = { -64, 64, 48, 16, { 0b1111, 0b1001 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesDown25Seq0 = {
        .clearance = { 0, 0, 48, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 2,
        .blockedSegments = kLeftQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesDown25Seq1 = {
        .clearance = { 0, -32, 48, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesDown25Seq2 = {
        .clearance = { -32, 0, 32, 16, { 0b0111, 0b0001 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kLeftQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesDown25Seq3 = {
        .clearance = { -32, -32, 24, 16, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kLeftQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesDown25Seq4 = {
        .clearance = { -32, -64, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesDown25Seq5 = {
        .clearance = { -64, -32, 16, 16, { 0b0111, 0b0001 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesDown25Seq6 = {
        .clearance = { -64, -64, 0, 16, { 0b1111, 0b1001 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesDown25Seq0 = {
        .clearance = { 0, 0, 48, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 2,
        .blockedSegments = kRightQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesDown25Seq1 = {
        .clearance = { 0, 32, 48, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesDown25Seq2 = {
        .clearance = { -32, 0, 32, 16, { 0b1011, 0b0010 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kRightQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesDown25Seq3 = {
        .clearance = { -32, 32, 24, 16, { 0b1110, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kRightQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesDown25Seq4 = {
        .clearance = { -32, 64, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesDown25Seq5 = {
        .clearance = { -64, 32, 16, 16, { 0b1011, 0b0010 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesDown25Seq6 = {
        .clearance = { -64, 64, 0, 16, { 0b1111, 0b0110 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kSBendLeftSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kSBendLeftSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsAllTypes(
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft)),
    };

    static constexpr SequenceDescriptor kSBendLeftSeq2 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, 0 },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = blockedSegmentsAllTypes(
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight)),
    };

    static constexpr SequenceDescriptor kSBendLeftSeq3 = {
        .clearance = { -64, -32, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kSBendRightSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kSBendLeftSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kSBendRightSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kSBendLeftSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kSBendRightSeq2 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kSBendLeftSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kSBendRightSeq3 = {
        .clearance = { -64, 32, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kSBendLeftSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(
                PS::top, PS::left, PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft,
                PS::bottomRight),   // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq1 = {
        .clearance = { -32, 0, 16, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsAll,                                                                           // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq2 = {
        .clearance = { -64, 0, 32, 96, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq3 = {
        .clearance = { -32, 0, 120, 16, { 0b0110, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq4 = {
        .clearance = { -32, -32, 120, 0, { 0b0000, 0 }, 0 },
        .blockedSegments = { {
            kSegmentsNone,          // narrow
            kSegmentsNone,          // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq5 = {
        .clearance = { 0, 0, 120, 0, { 0b0000, 0 }, 0 },
        .blockedSegments = { {
            kSegmentsNone,          // narrow
            kSegmentsNone,          // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq6 = {
        .clearance = { 0, -32, 120, 16, { 0b1001, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq7 = {
        .clearance = { 32, -32, 32, 96, { 0b1000, 0 }, 0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                             // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq8 = {
        .clearance = { 0, -32, 16, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1000,
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsAll,                                                                                   // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq9 = {
        .clearance = { -32, -32, 0, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(
                PS::top, PS::right, PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft,
                PS::bottomRight),   // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftVerticalLoopSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq1 = {
        .clearance = { -32, 0, 16, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1000,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftVerticalLoopSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq2 = {
        .clearance = { -64, 0, 32, 96, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftVerticalLoopSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq3 = {
        .clearance = { -32, 0, 120, 16, { 0b1001, 0 }, 0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftVerticalLoopSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq4 = {
        .clearance = { -32, 32, 120, 0, { 0b0000, 0 }, 0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftVerticalLoopSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq5 = {
        .clearance = { 0, 0, 120, 0, { 0b0000, 0 }, 0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftVerticalLoopSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq6 = {
        .clearance = { 0, 32, 120, 16, { 0b0110, 0 }, 0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftVerticalLoopSeq6.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq7 = {
        .clearance = { 32, 32, 32, 96, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftVerticalLoopSeq7.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq8 = {
        .clearance = { 0, 32, 16, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftVerticalLoopSeq8.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq9 = {
        .clearance = { -32, 32, 0, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftVerticalLoopSeq9.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft),                  // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsAllTypes(EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft)),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomRight),               // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TilesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft),                  // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TilesSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TilesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsAllTypes(EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft)),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TilesSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomRight),               // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TilesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TilesSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TilesSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TilesSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TilesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TilesSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TilesSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TilesSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b0111, 0b0100 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft),                  // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesUp25Seq1 = {
        .clearance = { 0, -32, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesUp25Seq2 = {
        .clearance = { -32, 0, 16, 0, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            kSegmentsNone,                                                   // narrow
            kSegmentsNone,                                                   // inverted
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesUp25Seq3 = {
        .clearance = { -32, -32, 16, 16, { 0b0111, 0b0110 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomRight),               // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1011, 0b1000 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesUp25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesUp25Seq1 = {
        .clearance = { 0, 32, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesUp25Seq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesUp25Seq2 = {
        .clearance = { -32, 0, 16, 0, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesUp25Seq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesUp25Seq3 = {
        .clearance = { -32, 32, 16, 16, { 0b1011, 0b1001 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesUp25Seq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesDown25Seq0 = {
        .clearance = { 0, 0, 16, 16, { 0b0111, 0b0011 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kLeftQuarterTurn3TilesUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesDown25Seq1 = {
        .clearance = { 0, -32, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftQuarterTurn3TilesUp25Seq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesDown25Seq2 = {
        .clearance = { -32, 0, 16, 0, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kLeftQuarterTurn3TilesUp25Seq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesDown25Seq3 = {
        .clearance = { -32, -32, 0, 16, { 0b0111, 0b0001 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn3TilesUp25Seq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesDown25Seq0 = {
        .clearance = { 0, 0, 16, 16, { 0b1011, 0b0011 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kRightQuarterTurn3TilesUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesDown25Seq1 = {
        .clearance = { 0, 32, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightQuarterTurn3TilesUp25Seq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesDown25Seq2 = {
        .clearance = { -32, 0, 16, 0, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kRightQuarterTurn3TilesUp25Seq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesDown25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b1011, 0b0010 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn3TilesUp25Seq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn1TileSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn1TileSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn1TileSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftTwistDownToUpSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft),         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftTwistDownToUpSeq1 = {
        .clearance = { -32, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft),         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftTwistDownToUpSeq2 = {
        .clearance = { -64, 0, 16, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft),         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kRightTwistDownToUpSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftTwistDownToUpSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightTwistDownToUpSeq1 = {
        .clearance = { -32, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftTwistDownToUpSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightTwistDownToUpSeq2 = {
        .clearance = { -64, 0, 16, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftTwistDownToUpSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftTwistUpToDownSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kLeftTwistDownToUpSeq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftTwistUpToDownSeq1 = {
        .clearance = { -32, 0, -16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = blockedSegmentsRotate(kLeftTwistDownToUpSeq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftTwistUpToDownSeq2 = {
        .clearance = { -64, 0, -16, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsRotate(kLeftTwistDownToUpSeq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kRightTwistUpToDownSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftTwistUpToDownSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightTwistUpToDownSeq1 = {
        .clearance = { -32, 0, -16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftTwistUpToDownSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightTwistUpToDownSeq2 = {
        .clearance = { -64, 0, -16, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftTwistUpToDownSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kHalfLoopUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kHalfLoopUpSeq1 = {
        .clearance = { -32, 0, 16, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsAll,           // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kHalfLoopUpSeq2 = {
        .clearance = { -64, 0, 32, 96, { 0b0011, 0 }, 0 },
        .allowedWallEdges = 0b1011,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kHalfLoopUpSeq3 = {
        .clearance = { -32, 0, 120, 16, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kHalfLoopDownSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kHalfLoopDownSeq1 = {
        .clearance = { -32, 0, -120, 96, { 0b0011, 0 }, 0 },
        .allowedWallEdges = 0b1011,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kHalfLoopDownSeq2 = {
        .clearance = { 0, 0, -136, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsAll,           // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kHalfLoopDownSeq3 = {
        .clearance = { 32, 0, -152, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftCorkscrewUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft),                            // narrow
            EnumsToFlags(PS::top, PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                                  // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftCorkscrewUpSeq1 = {
        .clearance = { -32, 0, 24, 32, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsAll,           // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftCorkscrewUpSeq2 = {
        .clearance = { -32, -32, 48, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight),                     // narrow
            EnumsToFlags(PS::top, PS::right, PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                               // wide
        } },
    };

    static constexpr SequenceDescriptor kRightCorkscrewUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftCorkscrewUpSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightCorkscrewUpSeq1 = {
        .clearance = { -32, 0, 24, 32, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftCorkscrewUpSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightCorkscrewUpSeq2 = {
        .clearance = { -32, 32, 48, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight),                   // narrow
            EnumsToFlags(PS::top, PS::left, PS::right, PS::centre, PS::topLeft, PS::topRight, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                             // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftCorkscrewDownSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kRightCorkscrewUpSeq2.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftCorkscrewDownSeq1 = {
        .clearance = { -32, 0, -56, 32, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsRotate(kRightCorkscrewUpSeq1.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftCorkscrewDownSeq2 = {
        .clearance = { -32, -32, -80, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsRotate(kRightCorkscrewUpSeq0.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kRightCorkscrewDownSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kLeftCorkscrewUpSeq2.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightCorkscrewDownSeq1 = {
        .clearance = { -32, 0, -56, 32, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsRotate(kLeftCorkscrewUpSeq1.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightCorkscrewDownSeq2 = {
        .clearance = { -32, 32, -80, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsRotate(kLeftCorkscrewUpSeq0.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kFlatToUp60Seq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp60Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp60ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToFlat },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToDown60Seq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToFlat },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = -2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown60ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp60Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kTowerBaseSeq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::trackOrigin },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kTowerBaseSeq1 = {
        .clearance = { -32, -32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kTowerBaseSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kTowerBaseSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kTowerBaseSeq4 = {
        .clearance = { 0, -32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kTowerBaseSeq5 = {
        .clearance = { 0, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kTowerBaseSeq6 = {
        .clearance = { 32, -32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSW, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kTowerBaseSeq7 = {
        .clearance = { 32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kTowerBaseSeq8 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_1 },
        .flags = { SequenceFlag::entranceConnectionSW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kTowerSectionSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
    };

    static constexpr SequenceDescriptor kTowerSectionSeq1 = {
        .clearance = { 32, 0, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_1 },
    };

    static constexpr SequenceDescriptor kFlatCoveredSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp25CoveredSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp60CoveredSeq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kUp60Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToUp25CoveredSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatToUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp25ToUp60CoveredSeq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kUp25ToUp60Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp60ToUp25CoveredSeq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kUp60ToUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp25ToFlatCoveredSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kUp25ToFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown25CoveredSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kDown25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown60CoveredSeq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kDown60Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToDown25CoveredSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatToDown25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown25ToDown60CoveredSeq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kDown25ToDown60Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown60ToDown25CoveredSeq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kDown60ToDown25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown25ToFlatCoveredSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kDown25ToFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesCoveredSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kLeftQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesCoveredSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesCoveredSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kLeftQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesCoveredSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, 0 },
        .blockedSegments = kLeftQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesCoveredSeq4 = {
        .clearance = { -32, -64, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesCoveredSeq5 = {
        .clearance = { -64, -32, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kLeftQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesCoveredSeq6 = {
        .clearance = { -64, -64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesCoveredSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kRightQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesCoveredSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesCoveredSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kRightQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesCoveredSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, 0 },
        .blockedSegments = kRightQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesCoveredSeq4 = {
        .clearance = { -32, 64, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesCoveredSeq5 = {
        .clearance = { -64, 32, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kRightQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesCoveredSeq6 = {
        .clearance = { -64, 64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kSBendLeftCoveredSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kSBendLeftSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kSBendLeftCoveredSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kSBendLeftSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kSBendLeftCoveredSeq2 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, 0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kSBendLeftSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kSBendLeftCoveredSeq3 = {
        .clearance = { -64, -32, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kSBendLeftSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kSBendRightCoveredSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kSBendRightSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kSBendRightCoveredSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kSBendRightSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kSBendRightCoveredSeq2 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kSBendRightSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kSBendRightCoveredSeq3 = {
        .clearance = { -64, 32, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kSBendRightSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesCoveredSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kLeftQuarterTurn3TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesCoveredSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftQuarterTurn3TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesCoveredSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kLeftQuarterTurn3TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesCoveredSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn3TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesCoveredSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kRightQuarterTurn3TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesCoveredSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightQuarterTurn3TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesCoveredSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kRightQuarterTurn3TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesCoveredSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn3TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                       // inverted
            kSegmentsAll,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = { {
            kSegmentsNone,          // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsNone,          // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq2 = {
        .clearance = { -32, 0, 0, 4, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                          // inverted
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq3 = {
        .clearance = { -32, -32, 0, 4, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                             // inverted
            kSegmentsAll,                                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq4 = {
        .clearance = { -32, -64, 8, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                           // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq5 = {
        .clearance = { 0, -64, 8, 0, { 0b0100, 0b0000 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            kSegmentsNone,          // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsNone,          // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq6 = {
        .clearance = { -32, -96, 8, 4, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq7 = {
        .clearance = { 0, -96, 8, 4, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -2,
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                             // inverted
            kSegmentsAll,                                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpSmallSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpSmallSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpSmallSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0b0000 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpSmallSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpSmallSeq2 = {
        .clearance = { -32, 0, 0, 4, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpSmallSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpSmallSeq3 = {
        .clearance = { -32, 32, 0, 4, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpSmallSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpSmallSeq4 = {
        .clearance = { -32, 64, 8, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpSmallSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpSmallSeq5 = {
        .clearance = { 0, 64, 8, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpSmallSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpSmallSeq6 = {
        .clearance = { -32, 96, 8, 4, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpSmallSeq6.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpSmallSeq7 = {
        .clearance = { 0, 96, 8, 4, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 2,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpSmallSeq7.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownSmallSeq0 = {
        .clearance = { 0, 0, 8, 4, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kLeftHalfBankedHelixUpSmallSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownSmallSeq1 = {
        .clearance = { 0, -32, 8, 4, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kLeftHalfBankedHelixUpSmallSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownSmallSeq2 = {
        .clearance = { -32, 0, 8, 0, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kLeftHalfBankedHelixUpSmallSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownSmallSeq3 = {
        .clearance = { -32, -32, 8, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftHalfBankedHelixUpSmallSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownSmallSeq4 = {
        .clearance = { -32, -64, 0, 4, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftHalfBankedHelixUpSmallSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownSmallSeq5 = {
        .clearance = { 0, -64, 0, 4, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kLeftHalfBankedHelixUpSmallSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownSmallSeq6 = {
        .clearance = { -32, -96, 0, 0, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kLeftHalfBankedHelixUpSmallSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownSmallSeq7 = {
        .clearance = { 0, -96, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -2,
        .blockedSegments = kLeftHalfBankedHelixUpSmallSeq7.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownSmallSeq0 = {
        .clearance = { 0, 0, 8, 4, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kRightHalfBankedHelixUpSmallSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownSmallSeq1 = {
        .clearance = { 0, 32, 8, 4, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kRightHalfBankedHelixUpSmallSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownSmallSeq2 = {
        .clearance = { -32, 0, 8, 0, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kRightHalfBankedHelixUpSmallSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownSmallSeq3 = {
        .clearance = { -32, 32, 8, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightHalfBankedHelixUpSmallSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownSmallSeq4 = {
        .clearance = { -32, 64, 0, 4, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightHalfBankedHelixUpSmallSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownSmallSeq5 = {
        .clearance = { 0, 64, 0, 4, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kRightHalfBankedHelixUpSmallSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownSmallSeq6 = {
        .clearance = { -32, 96, 0, 0, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kRightHalfBankedHelixUpSmallSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownSmallSeq7 = {
        .clearance = { 0, 96, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 2,
        .blockedSegments = kRightHalfBankedHelixUpSmallSeq7.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                       // inverted
            kSegmentsAll,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = { {
            kSegmentsNone,                                          // narrow
            kSegmentsUnimplemented,                                 // inverted
            EnumsToFlags(PS::right, PS::topRight, PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                             // inverted
            EnumsToFlags(
                PS::top, PS::right, PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft,
                PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq4 = {
        .clearance = { -32, -64, 0, 4, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = { {
            kSegmentsNone,                                          // narrow
            kSegmentsUnimplemented,                                 // inverted
            EnumsToFlags(PS::right, PS::topRight, PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq5 = {
        .clearance = { -64, -32, 0, 4, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                       // inverted
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq6 = {
        .clearance = { -64, -64, 0, 4, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                             // inverted
            kSegmentsAll,                                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq7 = {
        .clearance = { -64, -96, 8, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                           // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq8 = {
        .clearance = { -32, -96, 8, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = { {
            kSegmentsNone,                                    // narrow
            kSegmentsUnimplemented,                           // inverted
            EnumsToFlags(PS::top, PS::topLeft, PS::topRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq9 = {
        .clearance = { -64, -128, 8, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                       // inverted
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq10 = {
        .clearance = { -32, -128, 8, 0, { 0b1110, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            kSegmentsUnimplemented,                                       // inverted
            EnumsToFlags(
                PS::top, PS::left, PS::right, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft,
                PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq11 = {
        .clearance = { 0, -128, 8, 4, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = { {
            kSegmentsNone,                                    // narrow
            kSegmentsUnimplemented,                           // inverted
            EnumsToFlags(PS::top, PS::topLeft, PS::topRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq12 = {
        .clearance = { -32, -160, 8, 4, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                         // inverted
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq13 = {
        .clearance = { 0, -160, 8, 4, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -2,
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                             // inverted
            kSegmentsAll,                                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq4 = {
        .clearance = { -32, 64, 0, 4, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq5 = {
        .clearance = { -64, 32, 0, 4, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq6 = {
        .clearance = { -64, 64, 0, 4, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq6.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq7 = {
        .clearance = { -64, 96, 8, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq7.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq8 = {
        .clearance = { -32, 96, 8, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq8.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq9 = {
        .clearance = { -64, 128, 8, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq9.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq10 = {
        .clearance = { -32, 128, 8, 0, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq10.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq11 = {
        .clearance = { 0, 128, 8, 4, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq11.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq12 = {
        .clearance = { -32, 160, 8, 4, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq12.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq13 = {
        .clearance = { 0, 160, 8, 4, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 2,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq13.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq0 = {
        .clearance = { 0, 0, 8, 4, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq1 = {
        .clearance = { 0, -32, 8, 4, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq2 = {
        .clearance = { -32, 0, 8, 4, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq3 = {
        .clearance = { -32, -32, 8, 0, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq4 = {
        .clearance = { -32, -64, 8, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq5 = {
        .clearance = { -64, -32, 8, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq6 = {
        .clearance = { -64, -64, 8, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq7 = {
        .clearance = { -64, -96, 0, 4, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq7.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq8 = {
        .clearance = { -32, -96, 0, 4, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq8.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq9 = {
        .clearance = { -64, -128, 0, 4, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq9.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq10 = {
        .clearance = { -32, -128, 0, 0, { 0b1110, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq10.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq11 = {
        .clearance = { 0, -128, 0, 0, { 0b0100, 0b0000 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq11.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq12 = {
        .clearance = { -32, -160, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq12.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq13 = {
        .clearance = { 0, -160, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -2,
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq13.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq0 = {
        .clearance = { 0, 0, 8, 4, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq1 = {
        .clearance = { 0, 32, 8, 4, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq2 = {
        .clearance = { -32, 0, 8, 4, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq3 = {
        .clearance = { -32, 32, 8, 0, { 0b1110, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq4 = {
        .clearance = { -32, 64, 8, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq5 = {
        .clearance = { -64, 32, 8, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq6 = {
        .clearance = { -64, 64, 8, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq7 = {
        .clearance = { -64, 96, 0, 4, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq7.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq8 = {
        .clearance = { -32, 96, 0, 4, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq8.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq9 = {
        .clearance = { -64, 128, 0, 4, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq9.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq10 = {
        .clearance = { -32, 128, 0, 0, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq10.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq11 = {
        .clearance = { 0, 128, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq11.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq12 = {
        .clearance = { -32, 160, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq12.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq13 = {
        .clearance = { 0, 160, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 2,
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq13.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn1TileUp60Seq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsAllTypes(kSegmentsAll),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn1TileUp60Seq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsAllTypes(kSegmentsAll),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn1TileDown60Seq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsAllTypes(kSegmentsAll),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn1TileDown60Seq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsAllTypes(kSegmentsAll),
    };

    static constexpr SequenceDescriptor kBrakesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kBoosterSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    // Maze sequence blocks do not have the height marker flag set because they are handled differently
    static constexpr SequenceDescriptor kMazeSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kMazeSeq1 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq2 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq3 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq4 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq5 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq6 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq7 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq8 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq9 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq10 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq11 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq12 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq13 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq14 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq15 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeUpSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kBankedLeftQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeUpSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kBankedLeftQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeUpSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kBankedLeftQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeUpSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, 0 },
        .blockedSegments = kBankedLeftQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeUpSeq4 = {
        .clearance = { -32, -64, 0, 12, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kBankedLeftQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeUpSeq5 = {
        .clearance = { -64, -32, 0, 12, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kBankedLeftQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeUpSeq6 = {
        .clearance = { -64, -64, 0, 12, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kBankedLeftQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeUpSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kBankedRightQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeUpSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kBankedRightQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeUpSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kBankedRightQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeUpSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, 0 },
        .blockedSegments = kBankedRightQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeUpSeq4 = {
        .clearance = { -32, 64, 0, 12, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kBankedRightQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeUpSeq5 = {
        .clearance = { -64, 32, 0, 12, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kBankedRightQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeUpSeq6 = {
        .clearance = { -64, 64, 0, 12, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kBankedRightQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeDownSeq0 = {
        .clearance = { 0, 0, 0, 12, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kBankedLeftQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeDownSeq1 = {
        .clearance = { 0, -32, 0, 12, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kBankedLeftQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeDownSeq2 = {
        .clearance = { -32, 0, 0, 12, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kBankedLeftQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeDownSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, 0 },
        .blockedSegments = kBankedLeftQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeDownSeq4 = {
        .clearance = { -32, -64, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kBankedLeftQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeDownSeq5 = {
        .clearance = { -64, -32, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kBankedLeftQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeDownSeq6 = {
        .clearance = { -64, -64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kBankedLeftQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeDownSeq0 = {
        .clearance = { 0, 0, 0, 12, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kBankedRightQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeDownSeq1 = {
        .clearance = { 0, 32, 0, 12, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kBankedRightQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeDownSeq2 = {
        .clearance = { -32, 0, 0, 12, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kBankedRightQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeDownSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, 0 },
        .blockedSegments = kBankedRightQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeDownSeq4 = {
        .clearance = { -32, 64, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kBankedRightQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeDownSeq5 = {
        .clearance = { -64, 32, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kBankedRightQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeDownSeq6 = {
        .clearance = { -64, 64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kBankedRightQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeUpSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kLeftQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeUpSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeUpSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kLeftQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeUpSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, 0 },
        .blockedSegments = kLeftQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeUpSeq4 = {
        .clearance = { -32, -64, 0, 12, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeUpSeq5 = {
        .clearance = { -64, -32, 0, 12, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kLeftQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeUpSeq6 = {
        .clearance = { -64, -64, 0, 12, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeUpSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kRightQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeUpSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeUpSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kRightQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeUpSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, 0 },
        .blockedSegments = kRightQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeUpSeq4 = {
        .clearance = { -32, 64, 0, 12, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeUpSeq5 = {
        .clearance = { -64, 32, 0, 12, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kRightQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeUpSeq6 = {
        .clearance = { -64, 64, 0, 12, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeDownSeq0 = {
        .clearance = { 0, 0, 0, 12, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kLeftQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeDownSeq1 = {
        .clearance = { 0, -32, 0, 12, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeDownSeq2 = {
        .clearance = { -32, 0, 0, 12, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kLeftQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeDownSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, 0 },
        .blockedSegments = kLeftQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeDownSeq4 = {
        .clearance = { -32, -64, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeDownSeq5 = {
        .clearance = { -64, -32, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kLeftQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeDownSeq6 = {
        .clearance = { -64, -64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeDownSeq0 = {
        .clearance = { 0, 0, 0, 12, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kRightQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeDownSeq1 = {
        .clearance = { 0, 32, 0, 12, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeDownSeq2 = {
        .clearance = { -32, 0, 0, 12, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kRightQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeDownSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, 0 },
        .blockedSegments = kRightQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeDownSeq4 = {
        .clearance = { -32, 64, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeDownSeq5 = {
        .clearance = { -64, 32, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kRightQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeDownSeq6 = {
        .clearance = { -64, 64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp25LeftBankedSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp25RightBankedSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kWaterfallSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kRapidsSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kOnRidePhotoSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsAllTypes(kSegmentsAll),
    };

    static constexpr SequenceDescriptor kDown25LeftBankedSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown25RightBankedSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kWatersplashSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kWatersplashSeq1 = {
        .clearance = { -32, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kWatersplashSeq2 = {
        .clearance = { -64, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kWatersplashSeq3 = {
        .clearance = { -96, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kWatersplashSeq4 = {
        .clearance = { -128, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kFlatToUp60LongBaseSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp60DegLongBaseSeq0 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToUp60LongBaseSeq1 = {
        .clearance = { -32, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp60DegLongBaseSeq1 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToUp60LongBaseSeq2 = {
        .clearance = { -64, 0, 16, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp60DegLongBaseSeq2 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToUp60LongBaseSeq3 = {
        .clearance = { -96, 0, 40, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp60DegLongBaseSeq3 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp60ToFlatLongBaseSeq0 = {
        .clearance = { 0, 0, 0, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToFlatLongBaseSeq0 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp60ToFlatLongBaseSeq1 = {
        .clearance = { -32, 0, 40, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToFlatLongBaseSeq1 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp60ToFlatLongBaseSeq2 = {
        .clearance = { -64, 0, 64, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToFlatLongBaseSeq2 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp60ToFlatLongBaseSeq3 = {
        .clearance = { -96, 0, 80, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToFlatLongBaseSeq3 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kWhirlpoolSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kDown60ToFlatLongBaseSeq0 = {
        .clearance = { 0, 0, 40, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp60DegLongBaseSeq3 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown60ToFlatLongBaseSeq1 = {
        .clearance = { -32, 0, 16, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp60DegLongBaseSeq2 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown60ToFlatLongBaseSeq2 = {
        .clearance = { -64, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp60DegLongBaseSeq1 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown60ToFlatLongBaseSeq3 = {
        .clearance = { -96, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp60DegLongBaseSeq0 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToDown60LongBaseSeq0 = {
        .clearance = { 0, 0, 80, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToFlatLongBaseSeq3 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToDown60LongBaseSeq1 = {
        .clearance = { -32, 0, 64, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToFlatLongBaseSeq2 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToDown60LongBaseSeq2 = {
        .clearance = { -64, 0, 40, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToFlatLongBaseSeq1 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToDown60LongBaseSeq3 = {
        .clearance = { -96, 0, 0, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToFlatLongBaseSeq0 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kCableLiftHillSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kCableLiftHillSeq1 = {
        .clearance = { -32, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kCableLiftHillSeq2 = {
        .clearance = { -64, 0, -32, 32, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kCableLiftHillSeq3 = {
        .clearance = { -96, 0, -96, 64, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kReverseFreefallSlopeSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kReverseFreefallSlopeSeq1 = {
        .clearance = { -32, 0, 0, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kReverseFreefallSlopeSeq2 = {
        .clearance = { -64, 0, 0, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kReverseFreefallSlopeSeq3 = {
        .clearance = { -96, 0, 0, 80, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kReverseFreefallSlopeSeq4 = {
        .clearance = { -128, 0, 0, 160, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kReverseFreefallSlopeSeq5 = {
        .clearance = { -192, 0, 0, 208, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1011,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kReverseFreefallSlopeSeq6 = {
        .clearance = { -160, 0, 0, 208, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kReverseFreefallVerticalSeq0 = {
        .clearance = { 0, 0, 0, 48, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kReverseFreefallVerticalSeq1 = {
        .clearance = { 32, 0, 0, 48, { 0b1111, 0 }, 0 },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp90Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1111,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp90Seq1 = {
        .clearance = { 32, 0, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    };

    static constexpr SequenceDescriptor kDown90Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1111,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown90Seq1 = {
        .clearance = { 32, 0, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    };

    static constexpr SequenceDescriptor kUp60ToUp90Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b1100 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1011,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp60ToUp90Seq1 = {
        .clearance = { 32, 0, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    };

    static constexpr SequenceDescriptor kDown90ToDown60Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b0011 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1110,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp90ToUp60Seq0 = {
        .clearance = { 0, 0, 0, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1011,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown60ToDown90Seq0 = {
        .clearance = { 0, 0, 0, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1110,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown60ToDown90Seq1 = {
        .clearance = { 32, 0, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    };

    static constexpr SequenceDescriptor kBrakeForDropSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagSeq2 = {
        .clearance = { -32, -32, 0, 0, { 0b1000, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                                   // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagSeq3 = {
        .clearance = { -64, 0, 0, 0, { 0b0010, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagSeq4 = {
        .clearance = { -64, -32, 0, 0, { 0b0001, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::bottomCorner },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kRightEighthToDiagSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagSeq2 = {
        .clearance = { -32, 32, 0, 0, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagSeq3 = {
        .clearance = { -64, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagSeq4 = {
        .clearance = { -64, 32, 0, 0, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::leftCorner },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kRightEighthToDiagSeq4.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kRightEighthToDiagSeq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalSeq2 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kRightEighthToDiagSeq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kRightEighthToDiagSeq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalSeq4 = {
        .clearance = { -64, 32, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kRightEighthToDiagSeq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthToDiagSeq4.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0100,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthToDiagSeq2.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthToDiagSeq3.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthToDiagSeq1.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalSeq4 = {
        .clearance = { -32, 64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthToDiagSeq0.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft),                  // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                            // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagSeq2 = {
        .clearance = { -32, -32, 0, 0, { 0b1000, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                                   // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagSeq3 = {
        .clearance = { -64, 0, 0, 0, { 0b0010, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagSeq4 = {
        .clearance = { -64, -32, 0, 0, { 0b0001, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(
                PS::right, PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft,
                PS::bottomRight), // inverted
            kSegmentsAll,         // wide
        } },
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthBankToDiagSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthBankToDiagSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagSeq2 = {
        .clearance = { -32, 32, 0, 0, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthBankToDiagSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagSeq3 = {
        .clearance = { -64, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthBankToDiagSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagSeq4 = {
        .clearance = { -64, 32, 0, 0, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthBankToDiagSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kRightEighthBankToDiagSeq4.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kRightEighthBankToDiagSeq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalSeq2 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kRightEighthBankToDiagSeq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kRightEighthBankToDiagSeq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalSeq4 = {
        .clearance = { -64, 32, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kRightEighthBankToDiagSeq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthBankToDiagSeq4.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0100,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthBankToDiagSeq2.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthBankToDiagSeq3.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthBankToDiagSeq1.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalSeq4 = {
        .clearance = { -32, 64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthBankToDiagSeq0.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kDiagFlatSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60Seq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60Seq1 = {
        .clearance = { 0, 32, 0, 64, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60Seq2 = {
        .clearance = { -32, 0, 0, 64, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60Seq3 = {
        .clearance = { -32, 32, 0, 64, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToUp25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToUp25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToUp25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToUp60Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToUp60Seq1 = {
        .clearance = { 0, 32, 0, 32, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToUp60Seq2 = {
        .clearance = { -32, 0, 0, 32, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToUp60Seq3 = {
        .clearance = { -32, 32, 0, 32, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60ToUp25Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60ToUp25Seq1 = {
        .clearance = { 0, 32, 0, 32, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60ToUp25Seq2 = {
        .clearance = { -32, 0, 0, 32, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60ToUp25Seq3 = {
        .clearance = { -32, 32, 0, 32, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25Seq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25Seq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25Seq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25Seq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60Seq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1101, 0 }, 0 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60Seq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60Seq1 = {
        .clearance = { 0, 32, 0, 64, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60Seq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60Seq2 = {
        .clearance = { -32, 0, 0, 64, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60Seq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60Seq3 = {
        .clearance = { -32, 32, 0, 64, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60Seq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagFlatToDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25ToFlatSeq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagFlatToDown25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25ToFlatSeq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagFlatToDown25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25ToFlatSeq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagFlatToDown25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25ToFlatSeq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown25ToDown60Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1101, 0 }, 0 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToUp25Seq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown25ToDown60Seq1 = {
        .clearance = { 0, 32, 0, 32, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToUp25Seq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown25ToDown60Seq2 = {
        .clearance = { -32, 0, 0, 32, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToUp25Seq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown25ToDown60Seq3 = {
        .clearance = { -32, 32, 0, 32, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToUp25Seq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60ToDown25Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1101, 0 }, 0 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25ToUp60Seq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60ToDown25Seq1 = {
        .clearance = { 0, 32, 0, 32, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25ToUp60Seq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60ToDown25Seq2 = {
        .clearance = { -32, 0, 0, 32, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25ToUp60Seq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60ToDown25Seq3 = {
        .clearance = { -32, 32, 0, 32, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25ToUp60Seq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp25Seq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown25ToFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp25Seq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown25ToFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp25Seq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown25ToFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp25Seq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagFlatToUp60Seq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                             // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToUp60Seq1 = {
        .clearance = { 0, 32, 0, 24, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            kSegmentsUnimplemented,                                       // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToUp60Seq2 = {
        .clearance = { -32, 0, 0, 24, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToUp60Seq3 = {
        .clearance = { -32, 32, 0, 24, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                          // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                             // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60ToFlatSeq1 = {
        .clearance = { 0, 32, 0, 24, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            kSegmentsUnimplemented,                                       // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60ToFlatSeq2 = {
        .clearance = { -32, 0, 0, 24, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60ToFlatSeq3 = {
        .clearance = { -32, 32, 0, 24, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                          // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToDown60Seq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1101, 0 }, 0 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatSeq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagFlatToDown60Seq1 = {
        .clearance = { 0, 32, 0, 24, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatSeq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagFlatToDown60Seq2 = {
        .clearance = { -32, 0, 0, 24, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatSeq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagFlatToDown60Seq3 = {
        .clearance = { -32, 32, 0, 24, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatSeq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1101, 0 }, 0 },
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60Seq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60ToFlatSeq1 = {
        .clearance = { 0, 32, 0, 24, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60Seq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60ToFlatSeq2 = {
        .clearance = { -32, 0, 0, 24, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60Seq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60ToFlatSeq3 = {
        .clearance = { -32, 32, 0, 24, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60Seq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft),             // narrow
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                                // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft),          // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                             // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToFlatSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToFlatSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToFlatSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToFlatSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft),             // narrow
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                                // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToFlatSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToFlatSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToFlatSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToFlatSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft),          // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                             // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToUp25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToUp25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToUp25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToUp25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToUp25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToUp25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToLeftBankSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToLeftBankSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToLeftBankSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToLeftBankSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToRightBankSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToRightBankSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToRightBankSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToRightBankSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToDown25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToDown25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToDown25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToDown25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToDown25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToDown25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagDown25ToLeftBankSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagDown25ToLeftBankSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagDown25ToLeftBankSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagDown25ToLeftBankSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagDown25ToRightBankSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagDown25ToRightBankSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagDown25ToRightBankSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagDown25ToRightBankSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft),             // narrow
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                                // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft),          // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                             // wide
        } },
    };

    static constexpr SequenceDescriptor kLogFlumeReverserSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kSpinningTunnelSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::disallowDoors, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBarrelRollUpToDownSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                             // inverted
            kSegmentsAll,                                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBarrelRollUpToDownSeq1 = {
        .clearance = { -32, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                         // inverted
            kSegmentsAll,                                                                                   // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBarrelRollUpToDownSeq2 = {
        .clearance = { -64, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                              // inverted
            kSegmentsAll,                                                                        // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBarrelRollUpToDownSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBarrelRollUpToDownSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBarrelRollUpToDownSeq1 = {
        .clearance = { -32, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBarrelRollUpToDownSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBarrelRollUpToDownSeq2 = {
        .clearance = { -64, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBarrelRollUpToDownSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBarrelRollDownToUpSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kLeftBarrelRollUpToDownSeq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftBarrelRollDownToUpSeq1 = {
        .clearance = { -32, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kLeftBarrelRollUpToDownSeq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftBarrelRollDownToUpSeq2 = {
        .clearance = { -64, 0, -32, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kLeftBarrelRollUpToDownSeq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kRightBarrelRollDownToUpSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBarrelRollDownToUpSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBarrelRollDownToUpSeq1 = {
        .clearance = { -32, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBarrelRollDownToUpSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBarrelRollDownToUpSeq2 = {
        .clearance = { -64, 0, -32, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBarrelRollDownToUpSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankToLeftQuarterTurn3TilesUp25Seq0 = {
        .clearance = { 0, 0, 0, 0, { 0b0111, 0b0100 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                          // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBankToLeftQuarterTurn3TilesUp25Seq1 = {
        .clearance = { 0, -32, 0, 16, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = { {
            kSegmentsNone,          // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsNone,          // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBankToLeftQuarterTurn3TilesUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            kSegmentsNone,                                                   // narrow
            kSegmentsUnimplemented,                                          // inverted
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBankToLeftQuarterTurn3TilesUp25Seq3 = {
        .clearance = { -32, -32, 16, 16, { 0b0111, 0b0110 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                             // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankToRightQuarterTurn3TilesUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1011, 0b1000 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankToLeftQuarterTurn3TilesUp25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankToRightQuarterTurn3TilesUp25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankToLeftQuarterTurn3TilesUp25Seq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankToRightQuarterTurn3TilesUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankToLeftQuarterTurn3TilesUp25Seq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankToRightQuarterTurn3TilesUp25Seq3 = {
        .clearance = { -32, 32, 16, 16, { 0b1011, 0b1001 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankToLeftQuarterTurn3TilesUp25Seq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesDown25ToLeftBankSeq0 = {
        .clearance = { 0, 0, 16, 16, { 0b0111, 0b0011 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsRotate(kRightBankToRightQuarterTurn3TilesUp25Seq3.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesDown25ToLeftBankSeq1 = {
        .clearance = { 0, -32, 0, 16, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = blockedSegmentsRotate(kRightBankToRightQuarterTurn3TilesUp25Seq1.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesDown25ToLeftBankSeq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsRotate(kRightBankToRightQuarterTurn3TilesUp25Seq2.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesDown25ToLeftBankSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b0111, 0b0001 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsRotate(kRightBankToRightQuarterTurn3TilesUp25Seq0.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesDown25ToRightBankSeq0 = {
        .clearance = { 0, 0, 16, 16, { 0b1011, 0b0011 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesDown25ToLeftBankSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesDown25ToRightBankSeq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesDown25ToLeftBankSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesDown25ToRightBankSeq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesDown25ToLeftBankSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesDown25ToRightBankSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1011, 0b0010 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesDown25ToLeftBankSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kPoweredLiftSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopUpSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopUpSeq1 = {
        .clearance = { -32, 0, 16, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopUpSeq2 = {
        .clearance = { -64, 0, 32, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopUpSeq3 = {
        .clearance = { -96, 0, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopUpSeq4 = {
        .clearance = { -128, -32, 120, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopUpSeq5 = {
        .clearance = { -96, -32, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopUpSeq6 = {
        .clearance = { -64, -32, 248, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopUpSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeHalfLoopUpSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopUpSeq1 = {
        .clearance = { -32, 0, 16, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeHalfLoopUpSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopUpSeq2 = {
        .clearance = { -64, 0, 32, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeHalfLoopUpSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopUpSeq3 = {
        .clearance = { -96, 0, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeHalfLoopUpSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopUpSeq4 = {
        .clearance = { -128, 32, 120, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeHalfLoopUpSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopUpSeq5 = {
        .clearance = { -96, 32, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeHalfLoopUpSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopUpSeq6 = {
        .clearance = { -64, 32, 248, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeHalfLoopUpSeq6.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopDownSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = kRightLargeHalfLoopUpSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopDownSeq1 = {
        .clearance = { -32, 0, -216, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = kRightLargeHalfLoopUpSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopDownSeq2 = {
        .clearance = { -64, 0, -160, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kRightLargeHalfLoopUpSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopDownSeq3 = {
        .clearance = { -32, -32, -216, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kRightLargeHalfLoopUpSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopDownSeq4 = {
        .clearance = { 0, -32, -248, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .blockedSegments = kRightLargeHalfLoopUpSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopDownSeq5 = {
        .clearance = { 32, -32, -264, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = kRightLargeHalfLoopUpSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopDownSeq6 = {
        .clearance = { 64, -32, -280, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kRightLargeHalfLoopUpSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopDownSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = kLeftLargeHalfLoopUpSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopDownSeq1 = {
        .clearance = { -32, 0, -216, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .blockedSegments = kLeftLargeHalfLoopUpSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopDownSeq2 = {
        .clearance = { -64, 0, -160, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kLeftLargeHalfLoopUpSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopDownSeq3 = {
        .clearance = { -32, 32, -216, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kLeftLargeHalfLoopUpSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopDownSeq4 = {
        .clearance = { 0, 32, -248, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = kLeftLargeHalfLoopUpSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopDownSeq5 = {
        .clearance = { 32, 32, -264, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = kLeftLargeHalfLoopUpSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopDownSeq6 = {
        .clearance = { 64, 32, -280, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kLeftLargeHalfLoopUpSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftFlyerTwistUpSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerTwistUpSeq1 = {
        .clearance = { -32, 0, -16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerTwistUpSeq2 = {
        .clearance = { -64, 0, -16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kRightFlyerTwistUpSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerTwistUpSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerTwistUpSeq1 = {
        .clearance = { -32, 0, -16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerTwistUpSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerTwistUpSeq2 = {
        .clearance = { -64, 0, -16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerTwistUpSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftFlyerTwistDownSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kLeftFlyerTwistUpSeq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftFlyerTwistDownSeq1 = {
        .clearance = { -32, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = blockedSegmentsRotate(kLeftFlyerTwistUpSeq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftFlyerTwistDownSeq2 = {
        .clearance = { -64, 0, 16, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kLeftFlyerTwistUpSeq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kRightFlyerTwistDownSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerTwistDownSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerTwistDownSeq1 = {
        .clearance = { -32, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerTwistDownSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerTwistDownSeq2 = {
        .clearance = { -64, 0, 16, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerTwistDownSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopUninvertedUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopUninvertedUpSeq1 = {
        .clearance = { -32, 0, 16, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsAll,           // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopUninvertedUpSeq2 = {
        .clearance = { -64, 0, 32, 96, { 0b0011, 0 }, 0 },
        .allowedWallEdges = 0b1011,
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::bottomLeft, PS::topLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::bottomLeft, PS::topLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopUninvertedUpSeq3 = {
        .clearance = { -32, 0, 120, 16, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopInvertedDownSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlyerHalfLoopUninvertedUpSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopInvertedDownSeq1 = {
        .clearance = { -32, 0, -88, 96, { 0b0011, 0 }, 0 },
        .allowedWallEdges = 0b1011,
        .blockedSegments = kFlyerHalfLoopUninvertedUpSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopInvertedDownSeq2 = {
        .clearance = { 0, 0, -104, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .invertSegmentBlocking = true,
        .blockedSegments = kFlyerHalfLoopUninvertedUpSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopInvertedDownSeq3 = {
        .clearance = { 32, 0, -120, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = kFlyerHalfLoopUninvertedUpSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftFlyerCorkscrewUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kLeftCorkscrewUpSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftFlyerCorkscrewUpSeq1 = {
        .clearance = { -32, 0, 24, 32, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kLeftCorkscrewUpSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftFlyerCorkscrewUpSeq2 = {
        .clearance = { -32, -32, 48, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = kLeftCorkscrewUpSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightFlyerCorkscrewUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kRightCorkscrewUpSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightFlyerCorkscrewUpSeq1 = {
        .clearance = { -32, 0, 24, 32, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kRightCorkscrewUpSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightFlyerCorkscrewUpSeq2 = {
        .clearance = { -32, 32, 48, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = kRightCorkscrewUpSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftFlyerCorkscrewDownSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                       // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerCorkscrewDownSeq1 = {
        .clearance = { -32, 0, -24, 32, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsAll,           // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerCorkscrewDownSeq2 = {
        .clearance = { -32, -32, -48, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                             // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                             // wide
        } },
    };

    static constexpr SequenceDescriptor kRightFlyerCorkscrewDownSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                              // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                              // wide
        } },
    };

    static constexpr SequenceDescriptor kRightFlyerCorkscrewDownSeq1 = {
        .clearance = { -32, 0, -24, 32, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerCorkscrewDownSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerCorkscrewDownSeq2 = {
        .clearance = { -32, 32, -48, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerCorkscrewDownSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kHeartLineTransferUpSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kHeartLineTransferUpSeq1 = {
        .clearance = { -32, 0, 0, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kHeartLineTransferUpSeq2 = {
        .clearance = { -64, 0, 16, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kHeartLineTransferUpSeq3 = {
        .clearance = { 0, 0, 32, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kHeartLineTransferDownSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kHeartLineTransferUpSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kHeartLineTransferDownSeq1 = {
        .clearance = { -32, 0, -32, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .blockedSegments = kHeartLineTransferUpSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kHeartLineTransferDownSeq2 = {
        .clearance = { -64, 0, -16, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kHeartLineTransferUpSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kHeartLineTransferDownSeq3 = {
        .clearance = { 0, 0, -32, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .blockedSegments = kHeartLineTransferUpSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHeartLineRollSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHeartLineRollSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHeartLineRollSeq2 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHeartLineRollSeq3 = {
        .clearance = { -96, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHeartLineRollSeq4 = {
        .clearance = { -128, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHeartLineRollSeq5 = {
        .clearance = { -160, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kRightHeartLineRollSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHeartLineRollSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHeartLineRollSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHeartLineRollSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHeartLineRollSeq2 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHeartLineRollSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHeartLineRollSeq3 = {
        .clearance = { -96, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHeartLineRollSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHeartLineRollSeq4 = {
        .clearance = { -128, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHeartLineRollSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHeartLineRollSeq5 = {
        .clearance = { -160, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHeartLineRollSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kMinigolfHoleASeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMinigolfHoleASeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMinigolfHoleBSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMinigolfHoleBSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMinigolfHoleCSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMinigolfHoleCSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMinigolfHoleDSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMinigolfHoleDSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMinigolfHoleDSeq2 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMinigolfHoleESeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMinigolfHoleESeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMinigolfHoleESeq2 = {
        .clearance = { -32, -32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMultiDimInvertedFlatToDown90QuarterLoopSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kMultiDimInvertedFlatToDown90QuarterLoopSeq1 = {
        .clearance = { -32, 0, -40, 32, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kMultiDimInvertedFlatToDown90QuarterLoopSeq2 = {
        .clearance = { -64, 0, -96, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kMultiDimInvertedFlatToDown90QuarterLoopSeq3 = {
        .clearance = { -96, 0, -96, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    };

    static constexpr SequenceDescriptor kUp90ToInvertedFlatQuarterLoopSeq0 = {
        .clearance = { 0, 0, 0, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kUp90ToInvertedFlatQuarterLoopSeq1 = {
        .clearance = { 32, 0, 56, 32, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kUp90ToInvertedFlatQuarterLoopSeq2 = {
        .clearance = { 64, 0, 96, 16, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kInvertedFlatToDown90QuarterLoopSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kInvertedFlatToDown90QuarterLoopSeq1 = {
        .clearance = { -32, 0, -72, 32, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kInvertedFlatToDown90QuarterLoopSeq2 = {
        .clearance = { -64, 0, -128, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kInvertedFlatToDown90QuarterLoopSeq3 = {
        .clearance = { -96, 0, -128, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .blockedSegments = { {
            kSegmentsNone,          // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsNone,          // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftCurvedLiftHillSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                          // inverted
            kSegmentsUnimplemented,                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftCurvedLiftHillSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = { {
            kSegmentsNone,          // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftCurvedLiftHillSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            kSegmentsNone,          // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftCurvedLiftHillSeq3 = {
        .clearance = { -32, -32, 0, 8, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                             // inverted
            kSegmentsUnimplemented,                                             // wide
        } },
    };

    static constexpr SequenceDescriptor kRightCurvedLiftHillSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftCurvedLiftHillSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightCurvedLiftHillSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftCurvedLiftHillSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightCurvedLiftHillSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftCurvedLiftHillSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightCurvedLiftHillSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftCurvedLiftHillSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftReverserSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftReverserSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftReverserSeq2 = {
        .clearance = { -32, -32, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftReverserSeq3 = {
        .clearance = { -64, -32, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftReverserSeq4 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftReverserSeq5 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kRightReverserSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftReverserSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightReverserSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftReverserSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightReverserSeq2 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftReverserSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightReverserSeq3 = {
        .clearance = { -64, 32, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftReverserSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightReverserSeq4 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftReverserSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightReverserSeq5 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftReverserSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kAirThrustTopCapSeq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustTopCapSeq1 = {
        .clearance = { 32, 0, 0, 32, { 0b1111, 0 }, 0 },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustTopCapSeq2 = {
        .clearance = { -64, 0, 0, 32, { 0b1111, 0 }, 0 },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustTopCapSeq3 = {
        .clearance = { -32, 0, 0, 32, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownSeq0 = {
        .clearance = { 0, 0, 0, 48, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownSeq1 = {
        .clearance = { 32, 0, 0, 48, { 0b1111, 0 }, 0 },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownToLevelSeq0 = {
        .clearance = { 0, 0, 0, 208, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownToLevelSeq1 = {
        .clearance = { 32, 0, 0, 208, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownToLevelSeq2 = {
        .clearance = { -32, 0, 0, 160, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownToLevelSeq3 = {
        .clearance = { -64, 0, 0, 80, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownToLevelSeq4 = {
        .clearance = { -96, 0, 0, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownToLevelSeq5 = {
        .clearance = { -128, 0, 0, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownToLevelSeq6 = {
        .clearance = { -160, 0, 0, 16, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kBlockBrakesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TileUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b0111, 0b0100 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft),                  // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TileUp25Seq1 = {
        .clearance = { 0, -32, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TileUp25Seq2 = {
        .clearance = { -32, 0, 16, 0, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            kSegmentsNone,                                                   // narrow
            kSegmentsNone,                                                   // inverted
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TileUp25Seq3 = {
        .clearance = { -32, -32, 16, 16, { 0b0111, 0b0110 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomRight),               // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TileUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1011, 0b1000 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TileUp25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TileUp25Seq1 = {
        .clearance = { 0, 32, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TileUp25Seq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TileUp25Seq2 = {
        .clearance = { -32, 0, 16, 0, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TileUp25Seq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TileUp25Seq3 = {
        .clearance = { -32, 32, 16, 16, { 0b1011, 0b1001 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TileUp25Seq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TileDown25Seq0 = {
        .clearance = { 0, 0, 16, 16, { 0b0111, 0b0011 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kRightBankedQuarterTurn3TileUp25Seq3.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TileDown25Seq1 = {
        .clearance = { 0, -32, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = blockedSegmentsRotate(kRightBankedQuarterTurn3TileUp25Seq1.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TileDown25Seq2 = {
        .clearance = { -32, 0, 16, 0, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsRotate(kRightBankedQuarterTurn3TileUp25Seq2.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TileDown25Seq3 = {
        .clearance = { -32, -32, 0, 16, { 0b0111, 0b0001 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kRightBankedQuarterTurn3TileUp25Seq0.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TileDown25Seq0 = {
        .clearance = { 0, 0, 16, 16, { 0b1011, 0b0011 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TileDown25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TileDown25Seq1 = {
        .clearance = { 0, 32, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TileDown25Seq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TileDown25Seq2 = {
        .clearance = { -32, 0, 16, 0, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TileDown25Seq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TileDown25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b1011, 0b0010 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TileDown25Seq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft),                  // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileUp25Seq1 = {
        .clearance = { 0, -32, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = { {
            kSegmentsNone,                                          // narrow
            kSegmentsNone,                                          // inverted
            EnumsToFlags(PS::right, PS::topRight, PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileUp25Seq2 = {
        .clearance = { -32, 0, 16, 16, { 0b0111, 0b0100 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsAllTypes(
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft)),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileUp25Seq3 = {
        .clearance = { -32, -32, 24, 16, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            EnumsToFlags(
                PS::top, PS::right, PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft,
                PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileUp25Seq4 = {
        .clearance = { -32, -64, 48, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = { {
            kSegmentsNone,                                          // narrow
            kSegmentsNone,                                          // inverted
            EnumsToFlags(PS::right, PS::topRight, PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileUp25Seq5 = {
        .clearance = { -64, -32, 32, 16, { 0b0111, 0b0100 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsAllTypes(
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight)),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileUp25Seq6 = {
        .clearance = { -64, -64, 48, 16, { 0b1111, 0b0110 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomRight),               // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileUp25Seq7 = {
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileUp25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileUp25Seq1 = {
        .clearance = { 0, 32, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileUp25Seq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileUp25Seq2 = {
        .clearance = { -32, 0, 16, 16, { 0b1011, 0b1000 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileUp25Seq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileUp25Seq3 = {
        .clearance = { -32, 32, 24, 16, { 0b1110, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileUp25Seq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileUp25Seq4 = {
        .clearance = { -32, 64, 48, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileUp25Seq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileUp25Seq5 = {
        .clearance = { -64, 32, 32, 16, { 0b1011, 0b1000 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileUp25Seq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileUp25Seq6 = {
        .clearance = { -64, 64, 48, 16, { 0b1111, 0b1001 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileUp25Seq6.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileUp25Seq7 = {
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileDown25Seq0 = {
        .clearance = { 0, 0, 48, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kRightBankedQuarterTurn5TileUp25Seq6.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileDown25Seq1 = {
        .clearance = { 0, -32, 48, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = blockedSegmentsRotate(kRightBankedQuarterTurn5TileUp25Seq4.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileDown25Seq2 = {
        .clearance = { -32, 0, 32, 16, { 0b0111, 0b0001 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kRightBankedQuarterTurn5TileUp25Seq5.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileDown25Seq3 = {
        .clearance = { -32, -32, 24, 16, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kRightBankedQuarterTurn5TileUp25Seq3.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileDown25Seq4 = {
        .clearance = { -32, -64, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = blockedSegmentsRotate(kRightBankedQuarterTurn5TileUp25Seq1.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileDown25Seq5 = {
        .clearance = { -64, -32, 16, 16, { 0b0111, 0b0001 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsRotate(kRightBankedQuarterTurn5TileUp25Seq2.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileDown25Seq6 = {
        .clearance = { -64, -64, 0, 16, { 0b1111, 0b1001 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kRightBankedQuarterTurn5TileUp25Seq0.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileDown25Seq7 = {
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileDown25Seq0 = {
        .clearance = { 0, 0, 48, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileDown25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileDown25Seq1 = {
        .clearance = { 0, 32, 48, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileDown25Seq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileDown25Seq2 = {
        .clearance = { -32, 0, 32, 16, { 0b1011, 0b0010 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileDown25Seq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileDown25Seq3 = {
        .clearance = { -32, 32, 24, 16, { 0b1110, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileDown25Seq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileDown25Seq4 = {
        .clearance = { -32, 64, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileDown25Seq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileDown25Seq5 = {
        .clearance = { -64, 32, 16, 16, { 0b1011, 0b0010 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileDown25Seq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileDown25Seq6 = {
        .clearance = { -64, 64, 0, 16, { 0b1111, 0b0110 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileDown25Seq6.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileDown25Seq7 = {
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kUp25ToLeftBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kUp25ToRightBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .blockedSegments = blockedSegmentsFlipXAxis(kUp25ToLeftBankedUp25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankedUp25ToUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankedUp25ToUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedUp25ToUp25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kDown25ToLeftBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kDown25ToRightBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = blockedSegmentsFlipXAxis(kDown25ToLeftBankedDown25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankedDown25ToDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankedDown25ToDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedDown25ToDown25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankedFlatToLeftBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankedFlatToRightBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedFlatToLeftBankedUp25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankedUp25ToLeftBankedFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankedUp25ToRightBankedFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedUp25ToLeftBankedFlatSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankedFlatToLeftBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .extraSupportRotation = 2,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankedFlatToRightBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .extraSupportRotation = 2,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedFlatToLeftBankedDown25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankedDown25ToLeftBankedFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankedDown25ToRightBankedFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedDown25ToLeftBankedFlatSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kFlatToLeftBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kFlatToRightBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .blockedSegments = blockedSegmentsFlipXAxis(kFlatToLeftBankedUp25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankedUp25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankedUp25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedUp25ToFlatSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kFlatToLeftBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .extraSupportRotation = 2,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kFlatToRightBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .extraSupportRotation = 2,
        .blockedSegments = blockedSegmentsFlipXAxis(kFlatToLeftBankedDown25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankedDown25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankedDown25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedDown25ToFlatSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn1TileUp90Seq0 = {
        .clearance = { 0, 0, 0, 72, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1111,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn1TileUp90Seq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn1TileUp90Seq0 = {
        .clearance = { 0, 0, 0, 72, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1111,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn1TileUp90Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn1TileUp90Seq1 = {
        .clearance = { 0, -32, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn1TileDown90Seq0 = {
        .clearance = { 0, 0, 0, 72, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1111,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::centre, PS::topLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn1TileDown90Seq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn1TileDown90Seq0 = {
        .clearance = { 0, 0, 0, 72, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1111,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn1TileDown90Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn1TileDown90Seq1 = {
        .clearance = { 0, -32, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    };

    static constexpr SequenceDescriptor kMultiDimUp90ToInvertedFlatQuarterLoopSeq0 = {
        .clearance = { 0, 0, 0, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kMultiDimUp90ToInvertedFlatQuarterLoopSeq1 = {
        .clearance = { 32, 0, 56, 32, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kMultiDimUp90ToInvertedFlatQuarterLoopSeq2 = {
        .clearance = { 64, 0, 96, 16, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kMultiDimFlatToDown90QuarterLoopSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kMultiDimFlatToDown90QuarterLoopSeq1 = {
        .clearance = { -32, 0, -72, 32, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kMultiDimFlatToDown90QuarterLoopSeq2 = {
        .clearance = { -64, 0, -128, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kMultiDimFlatToDown90QuarterLoopSeq3 = {
        .clearance = { -96, 0, -128, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    };

    static constexpr SequenceDescriptor kMultiDimInvertedUp90ToFlatQuarterLoopSeq0 = {
        .clearance = { 0, 0, 32, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kMultiDimInvertedUp90ToFlatQuarterLoopSeq1 = {
        .clearance = { 32, 0, 88, 32, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kMultiDimInvertedUp90ToFlatQuarterLoopSeq2 = {
        .clearance = { 64, 0, 128, 16, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRotationControlToggleSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlatTrack1x4ASeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin,
                   SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4ASeq1 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4ASeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4ASeq3 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_1 },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x2Seq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin,
                   SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x2Seq1 = {
        .clearance = { 0, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x2Seq2 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSW, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x2Seq3 = {
        .clearance = { 32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin,
                   SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq1 = {
        .clearance = { 0, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq2 = {
        .clearance = { 0, 64, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq3 = {
        .clearance = { 0, 96, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq4 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq5 = {
        .clearance = { 32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq6 = {
        .clearance = { 32, 64, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq7 = {
        .clearance = { 32, 96, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq8 = {
        .clearance = { 64, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq9 = {
        .clearance = { 64, 32, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq10 = {
        .clearance = { 64, 64, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq11 = {
        .clearance = { 64, 96, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq12 = {
        .clearance = { 96, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSW, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq13 = {
        .clearance = { 96, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq14 = {
        .clearance = { 96, 64, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq15 = {
        .clearance = { 96, 96, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin,
                   SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq1 = {
        .clearance = { 0, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq2 = {
        .clearance = { 0, 64, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq3 = {
        .clearance = { 0, 96, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq4 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSW, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq5 = {
        .clearance = { 32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq6 = {
        .clearance = { 32, 64, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq7 = {
        .clearance = { 32, 96, 0, 0, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_1 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x5Seq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x5Seq1 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x5Seq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x5Seq3 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x5Seq4 = {
        .clearance = { 64, 0, 0, 0, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_1 },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x1ASeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::trackOrigin, SequenceFlag::connectsToPath,
                   SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4BSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::trackOrigin, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4BSeq1 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4BSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4BSeq3 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_1 },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x1BSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin, SequenceFlag::connectsToPath,
                   SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4CSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4CSeq1 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW,
                   SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4CSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4CSeq3 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_1 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW, SequenceFlag::entranceConnectionNW,
                   SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::trackOrigin },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq1 = {
        .clearance = { -32, -32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq4 = {
        .clearance = { 0, -32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq5 = {
        .clearance = { 0, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq6 = {
        .clearance = { 32, -32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSW, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq7 = {
        .clearance = { 32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq8 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_1 },
        .flags = { SequenceFlag::entranceConnectionSW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                       // inverted
            kSegmentsUnimplemented,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewUpSeq1 = {
        .clearance = { -32, 0, 8, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewUpSeq2 = {
        .clearance = { -64, 0, 32, 24, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                          // inverted
            kSegmentsUnimplemented,                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewUpSeq3 = {
        .clearance = { -32, -32, 32, 24, { 0b1000, 0 }, 0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = { {
            kSegmentsNone,          // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewUpSeq4 = {
        .clearance = { -64, -32, 48, 40, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                       // inverted
            kSegmentsUnimplemented,                                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewUpSeq5 = {
        .clearance = { -64, -64, 72, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                       // inverted
            kSegmentsUnimplemented,                                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewUpSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewUpSeq1 = {
        .clearance = { -32, 0, 8, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewUpSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewUpSeq2 = {
        .clearance = { -64, 0, 32, 24, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewUpSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewUpSeq3 = {
        .clearance = { -32, 32, 32, 24, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewUpSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewUpSeq4 = {
        .clearance = { -64, 32, 48, 40, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewUpSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewUpSeq5 = {
        .clearance = { -64, 64, 72, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewUpSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewDownSeq0 = {
        .clearance = { 0, 0, -40, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kRightLargeCorkscrewUpSeq5.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewDownSeq1 = {
        .clearance = { -32, 0, -64, 40, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kRightLargeCorkscrewUpSeq4.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewDownSeq2 = {
        .clearance = { -32, -32, -80, 24, { 0b1000, 0 }, 0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = blockedSegmentsRotate(kRightLargeCorkscrewUpSeq3.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewDownSeq3 = {
        .clearance = { -64, 0, -80, 24, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsRotate(kRightLargeCorkscrewUpSeq2.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewDownSeq4 = {
        .clearance = { -64, -32, -104, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .blockedSegments = blockedSegmentsRotate(kRightLargeCorkscrewUpSeq1.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewDownSeq5 = {
        .clearance = { -64, -64, -112, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kRightLargeCorkscrewUpSeq0.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewDownSeq0 = {
        .clearance = { 0, 0, -40, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewDownSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewDownSeq1 = {
        .clearance = { -32, 0, -64, 40, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewDownSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewDownSeq2 = {
        .clearance = { -32, 32, -80, 24, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewDownSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewDownSeq3 = {
        .clearance = { -64, 0, -80, 24, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewDownSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewDownSeq4 = {
        .clearance = { -64, 32, -104, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewDownSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewDownSeq5 = {
        .clearance = { -64, 64, -112, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewDownSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopUpSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopUpSeq1 = {
        .clearance = { -32, 0, 16, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopUpSeq2 = {
        .clearance = { -64, 0, 56, 120, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                          // inverted
            kSegmentsUnimplemented,                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopUpSeq3 = {
        .clearance = { -64, -32, 56, 120, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            kSegmentsUnimplemented,                                                // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopUpSeq4 = {
        .clearance = { -32, -32, 168, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                         // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopUpSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftMediumHalfLoopUpSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopUpSeq1 = {
        .clearance = { -32, 0, 16, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftMediumHalfLoopUpSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopUpSeq2 = {
        .clearance = { -64, 0, 56, 120, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftMediumHalfLoopUpSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopUpSeq3 = {
        .clearance = { -64, 32, 56, 120, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftMediumHalfLoopUpSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopUpSeq4 = {
        .clearance = { -32, 32, 168, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftMediumHalfLoopUpSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopDownSeq0 = {
        .clearance = { 0, 0, -48, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kRightMediumHalfLoopUpSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopDownSeq1 = {
        .clearance = { -32, 0, -160, 120, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kRightMediumHalfLoopUpSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopDownSeq2 = {
        .clearance = { -32, -32, -160, 120, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kRightMediumHalfLoopUpSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopDownSeq3 = {
        .clearance = { 0, -32, -200, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .blockedSegments = kRightMediumHalfLoopUpSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopDownSeq4 = {
        .clearance = { 32, -32, -216, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kRightMediumHalfLoopUpSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopDownSeq0 = {
        .clearance = { 0, 0, -48, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftMediumHalfLoopDownSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopDownSeq1 = {
        .clearance = { -32, 0, -160, 120, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftMediumHalfLoopDownSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopDownSeq2 = {
        .clearance = { -32, 32, -160, 120, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftMediumHalfLoopDownSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopDownSeq3 = {
        .clearance = { 0, 32, -200, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftMediumHalfLoopDownSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopDownSeq4 = {
        .clearance = { 32, 32, -216, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftMediumHalfLoopDownSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftZeroGRollUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftZeroGRollUpSeq1 = {
        .clearance = { -32, 0, 8, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsAll,                                                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftZeroGRollUpSeq2 = {
        .clearance = { -64, 0, 24, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsAll,                                                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kRightZeroGRollUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftZeroGRollUpSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightZeroGRollUpSeq1 = {
        .clearance = { -32, 0, 8, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftZeroGRollUpSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightZeroGRollUpSeq2 = {
        .clearance = { -64, 0, 24, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftZeroGRollUpSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftZeroGRollDownSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kLeftZeroGRollUpSeq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftZeroGRollDownSeq1 = {
        .clearance = { -32, 0, -48, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kLeftZeroGRollUpSeq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftZeroGRollDownSeq2 = {
        .clearance = { -64, 0, -56, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kLeftZeroGRollUpSeq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kRightZeroGRollDownSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftZeroGRollDownSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightZeroGRollDownSeq1 = {
        .clearance = { -32, 0, -48, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftZeroGRollDownSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightZeroGRollDownSeq2 = {
        .clearance = { -64, 0, -56, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftZeroGRollDownSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftLargeZeroGRollUpSeq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeZeroGRollUpSeq1 = {
        .clearance = { -32, 0, 56, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeZeroGRollUpSeq2 = {
        .clearance = { -64, 0, 96, 40, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsAll,                                                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeZeroGRollUpSeq3 = {
        .clearance = { -96, 0, 120, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsAll,                                                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kRightLargeZeroGRollUpSeq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeZeroGRollUpSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeZeroGRollUpSeq1 = {
        .clearance = { -32, 0, 56, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeZeroGRollUpSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeZeroGRollUpSeq2 = {
        .clearance = { -64, 0, 96, 40, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeZeroGRollUpSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeZeroGRollUpSeq3 = {
        .clearance = { -96, 0, 120, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeZeroGRollUpSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftLargeZeroGRollDownSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kLeftLargeZeroGRollUpSeq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftLargeZeroGRollDownSeq1 = {
        .clearance = { -32, 0, -56, 40, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kLeftLargeZeroGRollUpSeq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftLargeZeroGRollDownSeq2 = {
        .clearance = { -64, 0, -96, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kLeftLargeZeroGRollUpSeq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftLargeZeroGRollDownSeq3 = {
        .clearance = { -96, 0, -152, 64, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kLeftLargeZeroGRollUpSeq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kRightLargeZeroGRollDownSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeZeroGRollDownSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeZeroGRollDownSeq1 = {
        .clearance = { -32, 0, -56, 40, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeZeroGRollDownSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeZeroGRollDownSeq2 = {
        .clearance = { -64, 0, -96, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeZeroGRollDownSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeZeroGRollDownSeq3 = {
        .clearance = { -96, 0, -152, 64, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeZeroGRollDownSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedUpSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedUpSeq1 = {
        .clearance = { -32, 0, 16, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedUpSeq2 = {
        .clearance = { -64, 0, 32, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedUpSeq3 = {
        .clearance = { -96, 0, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedUpSeq4 = {
        .clearance = { -128, -32, 120, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            kSegmentsUnimplemented,                                                // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedUpSeq5 = {
        .clearance = { -96, -32, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                         // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedUpSeq6 = {
        .clearance = { -64, -32, 248, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                         // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedUpSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedUpSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedUpSeq1 = {
        .clearance = { -32, 0, 16, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedUpSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedUpSeq2 = {
        .clearance = { -64, 0, 32, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedUpSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedUpSeq3 = {
        .clearance = { -96, 0, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedUpSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedUpSeq4 = {
        .clearance = { -128, 32, 120, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedUpSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedUpSeq5 = {
        .clearance = { -96, 32, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedUpSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedUpSeq6 = {
        .clearance = { -64, 32, 248, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedUpSeq6.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedDownSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                                 // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedDownSeq1 = {
        .clearance = { -32, 0, -184, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                                 // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedDownSeq2 = {
        .clearance = { -64, 0, -128, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                          // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedDownSeq3 = {
        .clearance = { -32, -32, -184, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                                         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedDownSeq4 = {
        .clearance = { 0, -32, -216, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                                         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedDownSeq5 = {
        .clearance = { 32, -32, -232, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                 // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedDownSeq6 = {
        .clearance = { 64, -32, -248, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                 // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedDownSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedDownSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedDownSeq1 = {
        .clearance = { -32, 0, -184, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedDownSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedDownSeq2 = {
        .clearance = { -64, 0, -128, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedDownSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedDownSeq3 = {
        .clearance = { -32, 32, -184, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedDownSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedDownSeq4 = {
        .clearance = { 0, 32, -216, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedDownSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedDownSeq5 = {
        .clearance = { 32, 32, -232, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedDownSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedDownSeq6 = {
        .clearance = { 64, 32, -248, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedDownSeq6.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedUpSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            kSegmentsUnimplemented,                                 // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedUpSeq1 = {
        .clearance = { -32, 0, 16, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                 // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedUpSeq2 = {
        .clearance = { -64, 0, 32, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                                 // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedUpSeq3 = {
        .clearance = { -96, 0, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                                 // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedUpSeq4 = {
        .clearance = { -128, -32, 120, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedUpSeq5 = {
        .clearance = { -96, -32, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                                         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedUpSeq6 = {
        .clearance = { -64, -32, 248, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                                         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedUpSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedUpSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedUpSeq1 = {
        .clearance = { -32, 0, 16, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedUpSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedUpSeq2 = {
        .clearance = { -64, 0, 32, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedUpSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedUpSeq3 = {
        .clearance = { -96, 0, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedUpSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedUpSeq4 = {
        .clearance = { -128, 32, 120, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedUpSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedUpSeq5 = {
        .clearance = { -96, 32, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedUpSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedUpSeq6 = {
        .clearance = { -64, 32, 248, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedUpSeq6.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedDownSeq0 = {
        .clearance = { 0, 0, -32, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedDownSeq1 = {
        .clearance = { -32, 0, -216, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedDownSeq2 = {
        .clearance = { -64, 0, -160, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                          // inverted
            kSegmentsUnimplemented,                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedDownSeq3 = {
        .clearance = { -32, -32, -216, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                         // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedDownSeq4 = {
        .clearance = { 0, -32, -248, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                         // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedDownSeq5 = {
        .clearance = { 32, -32, -264, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedDownSeq6 = {
        .clearance = { 64, -32, -280, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedDownSeq0 = {
        .clearance = { 0, 0, -32, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedDownSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedDownSeq1 = {
        .clearance = { -32, 0, -216, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedDownSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedDownSeq2 = {
        .clearance = { -64, 0, -160, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedDownSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedDownSeq3 = {
        .clearance = { -32, 32, -216, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedDownSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedDownSeq4 = {
        .clearance = { 0, 32, -248, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedDownSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedDownSeq5 = {
        .clearance = { 32, 32, -264, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedDownSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedDownSeq6 = {
        .clearance = { 64, 32, -280, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedDownSeq6.blockedSegments),
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopInvertedUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            kSegmentsUnimplemented,                                 // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopInvertedUpSeq1 = {
        .clearance = { -32, 0, 16, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsAll,           // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopInvertedUpSeq2 = {
        .clearance = { -64, 0, 32, 96, { 0b0011, 0 }, 0 },
        .allowedWallEdges = 0b1011,
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            EnumsToFlags(
                PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft,
                PS::bottomRight),   // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopInvertedUpSeq3 = {
        .clearance = { -32, 0, 120, 32, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                 // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopUninvertedDownSeq0 = {
        .clearance = { 0, 0, -32, 32, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopUninvertedDownSeq1 = {
        .clearance = { -32, 0, -120, 96, { 0b0011, 0 }, 0 },
        .allowedWallEdges = 0b1011,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                       // inverted
            kSegmentsUnimplemented,                                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopUninvertedDownSeq2 = {
        .clearance = { 0, 0, -136, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopUninvertedDownSeq3 = {
        .clearance = { 32, 0, -152, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagUp25Seq1 = {
        .clearance = { -32, 0, 16, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsAll,                                                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagUp25Seq2 = {
        .clearance = { -32, -32, 32, 16, { 0b1000, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                         // inverted
            kSegmentsAll,                                                                   // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagUp25Seq3 = {
        .clearance = { -64, 0, 32, 16, { 0b0010, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                          // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagUp25Seq4 = {
        .clearance = { -64, -32, 32, 24, { 0b0001, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                           // inverted
            kSegmentsAll,                                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kRightEighthToDiagUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagUp25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagUp25Seq1 = {
        .clearance = { -32, 0, 16, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagUp25Seq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagUp25Seq2 = {
        .clearance = { -32, 32, 32, 16, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagUp25Seq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagUp25Seq3 = {
        .clearance = { -64, 0, 32, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagUp25Seq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagUp25Seq4 = {
        .clearance = { -64, 32, 32, 24, { 0b0010, 0 }, 0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagUp25Seq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagDown25Seq0 = {
        .clearance = { 0, 0, -16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = kLeftEighthToDiagUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagDown25Seq1 = {
        .clearance = { -32, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kLeftEighthToDiagUp25Seq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagDown25Seq2 = {
        .clearance = { -32, -32, -48, 16, { 0b1000, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kLeftEighthToDiagUp25Seq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagDown25Seq3 = {
        .clearance = { -64, 0, -48, 16, { 0b0010, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kLeftEighthToDiagUp25Seq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagDown25Seq4 = {
        .clearance = { -64, -32, -48, 16, { 0b0001, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kLeftEighthToDiagUp25Seq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthToDiagDown25Seq0 = {
        .clearance = { 0, 0, -16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagDown25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagDown25Seq1 = {
        .clearance = { -32, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagDown25Seq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagDown25Seq2 = {
        .clearance = { -32, 32, -48, 16, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagDown25Seq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagDown25Seq3 = {
        .clearance = { -64, 0, -48, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagDown25Seq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagDown25Seq4 = {
        .clearance = { -64, 32, -48, 16, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagDown25Seq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kRightEighthToDiagUp25Seq4.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalUp25Seq1 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kRightEighthToDiagUp25Seq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalUp25Seq2 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kRightEighthToDiagUp25Seq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalUp25Seq3 = {
        .clearance = { -32, 32, 16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kRightEighthToDiagUp25Seq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalUp25Seq4 = {
        .clearance = { -64, 32, 32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kRightEighthToDiagUp25Seq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthToDiagUp25Seq4.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalUp25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0100,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthToDiagUp25Seq2.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthToDiagUp25Seq3.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalUp25Seq3 = {
        .clearance = { -32, 32, 16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthToDiagUp25Seq1.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalUp25Seq4 = {
        .clearance = { -32, 64, 32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthToDiagUp25Seq0.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalDown25Seq0 = {
        .clearance = { 0, 0, -16, 24, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = kLeftEighthToOrthogonalUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalDown25Seq1 = {
        .clearance = { -32, 0, -16, 16, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kLeftEighthToOrthogonalUp25Seq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalDown25Seq2 = {
        .clearance = { 0, 32, -16, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kLeftEighthToOrthogonalUp25Seq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalDown25Seq3 = {
        .clearance = { -32, 32, -32, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kLeftEighthToOrthogonalUp25Seq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalDown25Seq4 = {
        .clearance = { -64, 32, -48, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kLeftEighthToOrthogonalUp25Seq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalDown25Seq0 = {
        .clearance = { 0, 0, -16, 24, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = kRightEighthToOrthogonalUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalDown25Seq1 = {
        .clearance = { 0, 32, -16, 16, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0100,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kRightEighthToOrthogonalUp25Seq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalDown25Seq2 = {
        .clearance = { -32, 0, -16, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kRightEighthToOrthogonalUp25Seq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalDown25Seq3 = {
        .clearance = { -32, 32, -32, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kRightEighthToOrthogonalUp25Seq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalDown25Seq4 = {
        .clearance = { -32, 64, -48, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kRightEighthToOrthogonalUp25Seq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25ToLeftBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25ToLeftBankedUp25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25ToLeftBankedUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25ToLeftBankedUp25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25ToRightBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25ToRightBankedUp25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25ToRightBankedUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25ToRightBankedUp25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToUp25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToUp25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToUp25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToUp25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25ToLeftBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25ToLeftBankedDown25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25ToLeftBankedDown25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25ToLeftBankedDown25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25ToRightBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25ToRightBankedDown25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25ToRightBankedDown25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25ToRightBankedDown25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToDown25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToDown25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToDown25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToDown25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToDown25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToDown25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedFlatToLeftBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedFlatToLeftBankedUp25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedFlatToLeftBankedUp25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedFlatToLeftBankedUp25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedFlatToRightBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedFlatToRightBankedUp25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedFlatToRightBankedUp25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedFlatToRightBankedUp25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToLeftBankedFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToLeftBankedFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToLeftBankedFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToLeftBankedFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToRightBankedFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToRightBankedFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToRightBankedFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToRightBankedFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedFlatToLeftBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedFlatToLeftBankedDown25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedFlatToLeftBankedDown25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedFlatToLeftBankedDown25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedFlatToRightBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedFlatToRightBankedDown25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedFlatToRightBankedDown25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedFlatToRightBankedDown25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToLeftBankedFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToLeftBankedFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToLeftBankedFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToLeftBankedFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToRightBankedFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToRightBankedFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToRightBankedFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToRightBankedFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankedUp25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankedUp25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankedUp25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankedUp25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankedUp25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankedUp25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankedDown25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankedDown25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankedDown25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankedDown25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankedDown25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankedDown25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25LeftBankedSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25LeftBankedSeq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25LeftBankedSeq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25LeftBankedSeq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25RightBankedSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25RightBankedSeq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25RightBankedSeq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25RightBankedSeq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25LeftBankedSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25LeftBankedSeq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25LeftBankedSeq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25LeftBankedSeq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25RightBankedSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25RightBankedSeq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25RightBankedSeq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25RightBankedSeq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .blockedSegments = kLeftEighthToDiagUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagUp25Seq1 = {
        .clearance = { -32, 0, 16, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kLeftEighthToDiagUp25Seq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagUp25Seq2 = {
        .clearance = { -32, -32, 32, 16, { 0b1000, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kLeftEighthToDiagUp25Seq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagUp25Seq3 = {
        .clearance = { -64, 0, 32, 16, { 0b0010, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kLeftEighthToDiagUp25Seq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagUp25Seq4 = {
        .clearance = { -64, -32, 32, 24, { 0b0001, 0 }, 0 },
        .blockedSegments = kLeftEighthToDiagUp25Seq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthBankToDiagUp25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagUp25Seq1 = {
        .clearance = { -32, 0, 16, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthBankToDiagUp25Seq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagUp25Seq2 = {
        .clearance = { -32, 32, 32, 16, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthBankToDiagUp25Seq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagUp25Seq3 = {
        .clearance = { -64, 0, 32, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthBankToDiagUp25Seq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagUp25Seq4 = {
        .clearance = { -64, 32, 32, 24, { 0b0010, 0 }, 0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthBankToDiagUp25Seq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagDown25Seq0 = {
        .clearance = { 0, 0, -16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = kLeftEighthBankToDiagUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagDown25Seq1 = {
        .clearance = { -32, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kLeftEighthBankToDiagUp25Seq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagDown25Seq2 = {
        .clearance = { -32, -32, -48, 16, { 0b1000, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kLeftEighthBankToDiagUp25Seq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagDown25Seq3 = {
        .clearance = { -64, 0, -48, 16, { 0b0010, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kLeftEighthBankToDiagUp25Seq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagDown25Seq4 = {
        .clearance = { -64, -32, -48, 16, { 0b0001, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kLeftEighthBankToDiagUp25Seq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagDown25Seq0 = {
        .clearance = { 0, 0, -16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = kRightEighthBankToDiagUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagDown25Seq1 = {
        .clearance = { -32, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kRightEighthBankToDiagUp25Seq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagDown25Seq2 = {
        .clearance = { -32, 32, -48, 16, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kRightEighthBankToDiagUp25Seq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagDown25Seq3 = {
        .clearance = { -64, 0, -48, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kRightEighthBankToDiagUp25Seq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagDown25Seq4 = {
        .clearance = { -64, 32, -48, 16, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kRightEighthBankToDiagUp25Seq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kRightEighthBankToDiagUp25Seq4.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalUp25Seq1 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kRightEighthBankToDiagUp25Seq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalUp25Seq2 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kRightEighthBankToDiagUp25Seq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalUp25Seq3 = {
        .clearance = { -32, 32, 16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kRightEighthBankToDiagUp25Seq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalUp25Seq4 = {
        .clearance = { -64, 32, 32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kRightEighthBankToDiagUp25Seq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthBankToDiagUp25Seq4.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalUp25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0100,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthBankToDiagUp25Seq2.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthBankToDiagUp25Seq3.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalUp25Seq3 = {
        .clearance = { -32, 32, 16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthBankToDiagUp25Seq1.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalUp25Seq4 = {
        .clearance = { -32, 64, 32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthBankToDiagUp25Seq0.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalDown25Seq0 = {
        .clearance = { 0, 0, -16, 24, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = kLeftEighthBankToOrthogonalUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalDown25Seq1 = {
        .clearance = { -32, 0, -16, 16, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kLeftEighthBankToOrthogonalUp25Seq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalDown25Seq2 = {
        .clearance = { 0, 32, -16, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kLeftEighthBankToOrthogonalUp25Seq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalDown25Seq3 = {
        .clearance = { -32, 32, -32, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kLeftEighthBankToOrthogonalUp25Seq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalDown25Seq4 = {
        .clearance = { -64, 32, -48, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kLeftEighthBankToOrthogonalUp25Seq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalDown25Seq0 = {
        .clearance = { 0, 0, -16, 24, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = kRightEighthBankToOrthogonalUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalDown25Seq1 = {
        .clearance = { 0, 32, -16, 16, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0100,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kRightEighthBankToOrthogonalUp25Seq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalDown25Seq2 = {
        .clearance = { -32, 0, -16, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kRightEighthBankToOrthogonalUp25Seq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalDown25Seq3 = {
        .clearance = { -32, 32, -32, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kRightEighthBankToOrthogonalUp25Seq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalDown25Seq4 = {
        .clearance = { -32, 64, -48, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kRightEighthBankToOrthogonalUp25Seq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagBrakesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagBrakesSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagBrakesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagBrakesSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagBlockBrakesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagBlockBrakesSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagBlockBrakesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagBlockBrakesSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                             // inverted
            kSegmentsUnimplemented,                                             // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            kSegmentsUnimplemented,                                       // inverted
            kSegmentsUnimplemented,                                       // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            kSegmentsUnimplemented,                                                // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq3 = {
        .clearance = { -32, 32, 0, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::right, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsUnimplemented, // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq4 = {
        .clearance = { -32, 64, 8, 40, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            kSegmentsUnimplemented,                                       // inverted
            kSegmentsUnimplemented,                                       // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq5 = {
        .clearance = { -64, 32, 8, 40, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            kSegmentsUnimplemented,                                                // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq6 = {
        .clearance = { -64, 64, 8, 64, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::right, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsUnimplemented, // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq7 = {
        .clearance = { -64, 96, 40, 88, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            kSegmentsUnimplemented,                                       // inverted
            kSegmentsUnimplemented,                                       // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq8 = {
        .clearance = { -96, 64, 40, 88, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            kSegmentsUnimplemented,                                                // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq9 = {
        .clearance = { -96, 96, 40, 104, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                          // inverted
            kSegmentsUnimplemented,                                          // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1101, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                             // inverted
            kSegmentsUnimplemented,                                             // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq1 = {
        .clearance = { 0, 32, 0, 72, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            kSegmentsUnimplemented,                                       // inverted
            kSegmentsUnimplemented,                                       // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq2 = {
        .clearance = { -32, 0, 0, 72, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            kSegmentsUnimplemented,                                                // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq3 = {
        .clearance = { -32, 32, 0, 80, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::right, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsUnimplemented, // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq4 = {
        .clearance = { -32, 64, 56, 32, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            kSegmentsUnimplemented,                                       // inverted
            kSegmentsUnimplemented,                                       // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq5 = {
        .clearance = { -64, 32, 56, 32, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            kSegmentsUnimplemented,                                                // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq6 = {
        .clearance = { -64, 64, 56, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::right, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsUnimplemented, // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq7 = {
        .clearance = { -64, 96, 80, 8, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            kSegmentsUnimplemented,                                       // inverted
            kSegmentsUnimplemented,                                       // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq8 = {
        .clearance = { -96, 64, 80, 8, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            kSegmentsUnimplemented,                                                // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq9 = {
        .clearance = { -96, 96, 80, 8, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                          // inverted
            kSegmentsUnimplemented,                                          // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq0 = {
        .clearance = { 0, 0, 80, 8, { 0b1101, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatLongBaseSeq9.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq1 = {
        .clearance = { 0, 32, 80, 8, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatLongBaseSeq8.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq2 = {
        .clearance = { -32, 0, 80, 8, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatLongBaseSeq7.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq3 = {
        .clearance = { -32, 32, 56, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatLongBaseSeq6.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq4 = {
        .clearance = { -32, 64, 56, 32, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatLongBaseSeq5.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq5 = {
        .clearance = { -64, 32, 56, 32, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatLongBaseSeq4.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq6 = {
        .clearance = { -64, 64, 0, 80, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatLongBaseSeq3.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq7 = {
        .clearance = { -64, 96, 0, 72, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatLongBaseSeq2.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq8 = {
        .clearance = { -96, 64, 0, 72, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatLongBaseSeq1.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq9 = {
        .clearance = { -96, 96, 0, 64, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatLongBaseSeq0.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq0 = {
        .clearance = { 0, 0, 40, 104, { 0b1101, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60LongBaseSeq9.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq1 = {
        .clearance = { 0, 32, 40, 88, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60LongBaseSeq8.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq2 = {
        .clearance = { -32, 0, 40, 88, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60LongBaseSeq7.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq3 = {
        .clearance = { -32, 32, 8, 64, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60LongBaseSeq6.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq4 = {
        .clearance = { -32, 64, 8, 40, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60LongBaseSeq5.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq5 = {
        .clearance = { -64, 32, 8, 40, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60LongBaseSeq4.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq6 = {
        .clearance = { -64, 64, 0, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60LongBaseSeq3.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq7 = {
        .clearance = { -64, 96, 0, 16, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60LongBaseSeq2.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq8 = {
        .clearance = { -96, 64, 0, 16, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60LongBaseSeq1.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq9 = {
        .clearance = { -96, 96, 0, 0, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60LongBaseSeq0.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopUpToOrthogonalSeq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1101, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                             // inverted
            kSegmentsUnimplemented,                                             // wide
        } },
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopUpToOrthogonalSeq1 = {
        .clearance = { 0, 32, 0, 88, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            kSegmentsUnimplemented,                                       // inverted
            kSegmentsUnimplemented,                                       // wide
        } },
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopUpToOrthogonalSeq2 = {
        .clearance = { -32, 0, 0, 88, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            kSegmentsUnimplemented,                                                // wide
        } },
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopUpToOrthogonalSeq3 = {
        .clearance = { -32, 32, 32, 80, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopUpToOrthogonalSeq4 = {
        .clearance = { -64, 32, 80, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopUpToOrthogonalSeq5 = {
        .clearance = { -96, 32, 112, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopUpToOrthogonalSeq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1101, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(
            blockedSegmentsFlipXAxis(kLeftEighthDiveLoopUpToOrthogonalSeq0.blockedSegments), 1),
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopUpToOrthogonalSeq1 = {
        .clearance = { -32, 0, 0, 88, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsRotate(
            blockedSegmentsFlipXAxis(kLeftEighthDiveLoopUpToOrthogonalSeq1.blockedSegments), 1),
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopUpToOrthogonalSeq2 = {
        .clearance = { 0, 32, 0, 88, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsRotate(
            blockedSegmentsFlipXAxis(kLeftEighthDiveLoopUpToOrthogonalSeq2.blockedSegments), 1),
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopUpToOrthogonalSeq3 = {
        .clearance = { -32, 32, 32, 80, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .blockedSegments = blockedSegmentsRotate(
            blockedSegmentsFlipXAxis(kLeftEighthDiveLoopUpToOrthogonalSeq3.blockedSegments), 1),
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopUpToOrthogonalSeq4 = {
        .clearance = { -32, 64, 80, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(
            blockedSegmentsFlipXAxis(kLeftEighthDiveLoopUpToOrthogonalSeq4.blockedSegments), 1),
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopUpToOrthogonalSeq5 = {
        .clearance = { -32, 96, 112, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(
            blockedSegmentsFlipXAxis(kLeftEighthDiveLoopUpToOrthogonalSeq5.blockedSegments), 1),
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopDownToDiagSeq0 = {
        .clearance = { 0, 0, 112, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kRightEighthDiveLoopUpToOrthogonalSeq5.blockedSegments, 1),
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopDownToDiagSeq1 = {
        .clearance = { -32, 0, 80, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kRightEighthDiveLoopUpToOrthogonalSeq4.blockedSegments, 1),
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopDownToDiagSeq2 = {
        .clearance = { -64, 0, 32, 80, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = blockedSegmentsRotate(kRightEighthDiveLoopUpToOrthogonalSeq3.blockedSegments, 1),
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopDownToDiagSeq3 = {
        .clearance = { -64, -32, 0, 88, { 0b1000, 0 }, 0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = blockedSegmentsRotate(kRightEighthDiveLoopUpToOrthogonalSeq2.blockedSegments, 1),
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopDownToDiagSeq4 = {
        .clearance = { -96, 0, 0, 88, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsRotate(kRightEighthDiveLoopUpToOrthogonalSeq1.blockedSegments, 1),
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopDownToDiagSeq5 = {
        .clearance = { -96, -32, 0, 64, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kRightEighthDiveLoopUpToOrthogonalSeq0.blockedSegments, 1),
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopDownToDiagSeq0 = {
        .clearance = { 0, 0, 112, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kLeftEighthDiveLoopUpToOrthogonalSeq5.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopDownToDiagSeq1 = {
        .clearance = { -32, 0, 80, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kLeftEighthDiveLoopUpToOrthogonalSeq4.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopDownToDiagSeq2 = {
        .clearance = { -64, 0, 32, 80, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .blockedSegments = blockedSegmentsRotate(kLeftEighthDiveLoopUpToOrthogonalSeq3.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopDownToDiagSeq3 = {
        .clearance = { -64, 32, 0, 88, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsRotate(kLeftEighthDiveLoopUpToOrthogonalSeq2.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopDownToDiagSeq4 = {
        .clearance = { -96, 0, 0, 88, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsRotate(kLeftEighthDiveLoopUpToOrthogonalSeq1.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopDownToDiagSeq5 = {
        .clearance = { -96, 32, 0, 64, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthDiveLoopUpToOrthogonalSeq0.blockedSegments, 2),
    };

    static constexpr SequenceData kSequenceDescriptorsByElement[] = {
        /* TrackElemType::flat */ { 1, { kFlatSeq0 } },
        /* TrackElemType::endStation */ { 1, { kEndStationSeq0 } },
        /* TrackElemType::beginStation */ { 1, { kBeginStationSeq0 } },
        /* TrackElemType::middleStation */ { 1, { kMiddleStationSeq0 } },
        /* TrackElemType::up25 */ { 1, { kUp25Seq0 } },
        /* TrackElemType::up60 */ { 1, { kUp60Seq0 } },
        /* TrackElemType::flatToUp25 */ { 1, { kFlatToUp25Seq0 } },
        /* TrackElemType::up25ToUp60 */ { 1, { kUp25ToUp60Seq0 } },
        /* TrackElemType::up60ToUp25 */ { 1, { kUp60ToUp25Seq0 } },
        /* TrackElemType::up25ToFlat */ { 1, { kUp25ToFlatSeq0 } },
        /* TrackElemType::down25 */ { 1, { kDown25Seq0 } },
        /* TrackElemType::down60 */ { 1, { kDown60Seq0 } },
        /* TrackElemType::flatToDown25 */ { 1, { kFlatToDown25Seq0 } },
        /* TrackElemType::down25ToDown60 */ { 1, { kDown25ToDown60Seq0 } },
        /* TrackElemType::down60ToDown25 */ { 1, { kDown60ToDown25Seq0 } },
        /* TrackElemType::down25ToFlat */ { 1, { kDown25ToFlatSeq0 } },
        /* TrackElemType::leftQuarterTurn5Tiles */
        { 7,
          { kLeftQuarterTurn5TilesSeq0, kLeftQuarterTurn5TilesSeq1, kLeftQuarterTurn5TilesSeq2, kLeftQuarterTurn5TilesSeq3,
            kLeftQuarterTurn5TilesSeq4, kLeftQuarterTurn5TilesSeq5, kLeftQuarterTurn5TilesSeq6 } },
        /* TrackElemType::rightQuarterTurn5Tiles */
        { 7,
          { kRightQuarterTurn5TilesSeq0, kRightQuarterTurn5TilesSeq1, kRightQuarterTurn5TilesSeq2, kRightQuarterTurn5TilesSeq3,
            kRightQuarterTurn5TilesSeq4, kRightQuarterTurn5TilesSeq5, kRightQuarterTurn5TilesSeq6 } },
        /* TrackElemType::flatToLeftBank */ { 1, { kFlatToLeftBankSeq0 } },
        /* TrackElemType::flatToRightBank */ { 1, { kFlatToRightBankSeq0 } },
        /* TrackElemType::leftBankToFlat */ { 1, { kLeftBankToFlatSeq0 } },
        /* TrackElemType::rightBankToFlat */ { 1, { kRightBankToFlatSeq0 } },
        /* TrackElemType::bankedLeftQuarterTurn5Tiles */
        { 7,
          { kBankedLeftQuarterTurn5TilesSeq0, kBankedLeftQuarterTurn5TilesSeq1, kBankedLeftQuarterTurn5TilesSeq2,
            kBankedLeftQuarterTurn5TilesSeq3, kBankedLeftQuarterTurn5TilesSeq4, kBankedLeftQuarterTurn5TilesSeq5,
            kBankedLeftQuarterTurn5TilesSeq6 } },
        /* TrackElemType::bankedRightQuarterTurn5Tiles */
        { 7,
          { kBankedRightQuarterTurn5TilesSeq0, kBankedRightQuarterTurn5TilesSeq1, kBankedRightQuarterTurn5TilesSeq2,
            kBankedRightQuarterTurn5TilesSeq3, kBankedRightQuarterTurn5TilesSeq4, kBankedRightQuarterTurn5TilesSeq5,
            kBankedRightQuarterTurn5TilesSeq6 } },
        /* TrackElemType::leftBankToUp25 */ { 1, { kLeftBankToUp25Seq0 } },
        /* TrackElemType::rightBankToUp25 */ { 1, { kRightBankToUp25Seq0 } },
        /* TrackElemType::up25ToLeftBank */ { 1, { kUp25ToLeftBankSeq0 } },
        /* TrackElemType::up25ToRightBank */ { 1, { kUp25ToRightBankSeq0 } },
        /* TrackElemType::leftBankToDown25 */ { 1, { kLeftBankToDown25Seq0 } },
        /* TrackElemType::rightBankToDown25 */ { 1, { kRightBankToDown25Seq0 } },
        /* TrackElemType::down25ToLeftBank */ { 1, { kDown25ToLeftBankSeq0 } },
        /* TrackElemType::down25ToRightBank */ { 1, { kDown25ToRightBankSeq0 } },
        /* TrackElemType::leftBank */ { 1, { kLeftBankSeq0 } },
        /* TrackElemType::rightBank */ { 1, { kRightBankSeq0 } },
        /* TrackElemType::leftQuarterTurn5TilesUp25 */
        { 7,
          { kLeftQuarterTurn5TilesUp25Seq0, kLeftQuarterTurn5TilesUp25Seq1, kLeftQuarterTurn5TilesUp25Seq2,
            kLeftQuarterTurn5TilesUp25Seq3, kLeftQuarterTurn5TilesUp25Seq4, kLeftQuarterTurn5TilesUp25Seq5,
            kLeftQuarterTurn5TilesUp25Seq6 } },
        /* TrackElemType::rightQuarterTurn5TilesUp25 */
        { 7,
          { kRightQuarterTurn5TilesUp25Seq0, kRightQuarterTurn5TilesUp25Seq1, kRightQuarterTurn5TilesUp25Seq2,
            kRightQuarterTurn5TilesUp25Seq3, kRightQuarterTurn5TilesUp25Seq4, kRightQuarterTurn5TilesUp25Seq5,
            kRightQuarterTurn5TilesUp25Seq6 } },
        /* TrackElemType::leftQuarterTurn5TilesDown25 */
        { 7,
          { kLeftQuarterTurn5TilesDown25Seq0, kLeftQuarterTurn5TilesDown25Seq1, kLeftQuarterTurn5TilesDown25Seq2,
            kLeftQuarterTurn5TilesDown25Seq3, kLeftQuarterTurn5TilesDown25Seq4, kLeftQuarterTurn5TilesDown25Seq5,
            kLeftQuarterTurn5TilesDown25Seq6 } },
        /* TrackElemType::rightQuarterTurn5TilesDown25 */
        { 7,
          { kRightQuarterTurn5TilesDown25Seq0, kRightQuarterTurn5TilesDown25Seq1, kRightQuarterTurn5TilesDown25Seq2,
            kRightQuarterTurn5TilesDown25Seq3, kRightQuarterTurn5TilesDown25Seq4, kRightQuarterTurn5TilesDown25Seq5,
            kRightQuarterTurn5TilesDown25Seq6 } },
        /* TrackElemType::sBendLeft */ { 4, { kSBendLeftSeq0, kSBendLeftSeq1, kSBendLeftSeq2, kSBendLeftSeq3 } },
        /* TrackElemType::sBendRight */ { 4, { kSBendRightSeq0, kSBendRightSeq1, kSBendRightSeq2, kSBendRightSeq3 } },
        /* TrackElemType::leftVerticalLoop */
        { 10,
          { kLeftVerticalLoopSeq0, kLeftVerticalLoopSeq1, kLeftVerticalLoopSeq2, kLeftVerticalLoopSeq3, kLeftVerticalLoopSeq4,
            kLeftVerticalLoopSeq5, kLeftVerticalLoopSeq6, kLeftVerticalLoopSeq7, kLeftVerticalLoopSeq8,
            kLeftVerticalLoopSeq9 } },
        /* TrackElemType::rightVerticalLoop */
        { 10,
          { kRightVerticalLoopSeq0, kRightVerticalLoopSeq1, kRightVerticalLoopSeq2, kRightVerticalLoopSeq3,
            kRightVerticalLoopSeq4, kRightVerticalLoopSeq5, kRightVerticalLoopSeq6, kRightVerticalLoopSeq7,
            kRightVerticalLoopSeq8, kRightVerticalLoopSeq9 } },
        /* TrackElemType::leftQuarterTurn3Tiles */
        { 4,
          { kLeftQuarterTurn3TilesSeq0, kLeftQuarterTurn3TilesSeq1, kLeftQuarterTurn3TilesSeq2, kLeftQuarterTurn3TilesSeq3 } },
        /* TrackElemType::rightQuarterTurn3Tiles */
        { 4,
          { kRightQuarterTurn3TilesSeq0, kRightQuarterTurn3TilesSeq1, kRightQuarterTurn3TilesSeq2,
            kRightQuarterTurn3TilesSeq3 } },
        /* TrackElemType::leftBankedQuarterTurn3Tiles */
        { 4,
          { kLeftBankedQuarterTurn3TilesSeq0, kLeftBankedQuarterTurn3TilesSeq1, kLeftBankedQuarterTurn3TilesSeq2,
            kLeftBankedQuarterTurn3TilesSeq3 } },
        /* TrackElemType::rightBankedQuarterTurn3Tiles */
        { 4,
          { kRightBankedQuarterTurn3TilesSeq0, kRightBankedQuarterTurn3TilesSeq1, kRightBankedQuarterTurn3TilesSeq2,
            kRightBankedQuarterTurn3TilesSeq3 } },
        /* TrackElemType::leftQuarterTurn3TilesUp25 */
        { 4,
          { kLeftQuarterTurn3TilesUp25Seq0, kLeftQuarterTurn3TilesUp25Seq1, kLeftQuarterTurn3TilesUp25Seq2,
            kLeftQuarterTurn3TilesUp25Seq3 } },
        /* TrackElemType::rightQuarterTurn3TilesUp25 */
        { 4,
          { kRightQuarterTurn3TilesUp25Seq0, kRightQuarterTurn3TilesUp25Seq1, kRightQuarterTurn3TilesUp25Seq2,
            kRightQuarterTurn3TilesUp25Seq3 } },
        /* TrackElemType::leftQuarterTurn3TilesDown25 */
        { 4,
          { kLeftQuarterTurn3TilesDown25Seq0, kLeftQuarterTurn3TilesDown25Seq1, kLeftQuarterTurn3TilesDown25Seq2,
            kLeftQuarterTurn3TilesDown25Seq3 } },
        /* TrackElemType::rightQuarterTurn3TilesDown25 */
        { 4,
          { kRightQuarterTurn3TilesDown25Seq0, kRightQuarterTurn3TilesDown25Seq1, kRightQuarterTurn3TilesDown25Seq2,
            kRightQuarterTurn3TilesDown25Seq3 } },
        /* TrackElemType::leftQuarterTurn1Tile */ { 1, { kLeftQuarterTurn1TileSeq0 } },
        /* TrackElemType::rightQuarterTurn1Tile */ { 1, { kRightQuarterTurn1TileSeq0 } },
        /* TrackElemType::leftTwistDownToUp */
        { 3, { kLeftTwistDownToUpSeq0, kLeftTwistDownToUpSeq1, kLeftTwistDownToUpSeq2 } },
        /* TrackElemType::rightTwistDownToUp */
        { 3, { kRightTwistDownToUpSeq0, kRightTwistDownToUpSeq1, kRightTwistDownToUpSeq2 } },
        /* TrackElemType::leftTwistUpToDown */
        { 3, { kLeftTwistUpToDownSeq0, kLeftTwistUpToDownSeq1, kLeftTwistUpToDownSeq2 } },
        /* TrackElemType::rightTwistUpToDown */
        { 3, { kRightTwistUpToDownSeq0, kRightTwistUpToDownSeq1, kRightTwistUpToDownSeq2 } },
        /* TrackElemType::halfLoopUp */ { 4, { kHalfLoopUpSeq0, kHalfLoopUpSeq1, kHalfLoopUpSeq2, kHalfLoopUpSeq3 } },
        /* TrackElemType::halfLoopDown */ { 4, { kHalfLoopDownSeq0, kHalfLoopDownSeq1, kHalfLoopDownSeq2, kHalfLoopDownSeq3 } },
        /* TrackElemType::leftCorkscrewUp */ { 3, { kLeftCorkscrewUpSeq0, kLeftCorkscrewUpSeq1, kLeftCorkscrewUpSeq2 } },
        /* TrackElemType::rightCorkscrewUp */ { 3, { kRightCorkscrewUpSeq0, kRightCorkscrewUpSeq1, kRightCorkscrewUpSeq2 } },
        /* TrackElemType::leftCorkscrewDown */
        { 3, { kLeftCorkscrewDownSeq0, kLeftCorkscrewDownSeq1, kLeftCorkscrewDownSeq2 } },
        /* TrackElemType::rightCorkscrewDown */
        { 3, { kRightCorkscrewDownSeq0, kRightCorkscrewDownSeq1, kRightCorkscrewDownSeq2 } },
        /* TrackElemType::flatToUp60 */ { 1, { kFlatToUp60Seq0 } },
        /* TrackElemType::up60ToFlat */ { 1, { kUp60ToFlatSeq0 } },
        /* TrackElemType::flatToDown60 */ { 1, { kFlatToDown60Seq0 } },
        /* TrackElemType::down60ToFlat */ { 1, { kDown60ToFlatSeq0 } },
        /* TrackElemType::towerBase */
        { 9,
          { kTowerBaseSeq0, kTowerBaseSeq1, kTowerBaseSeq2, kTowerBaseSeq3, kTowerBaseSeq4, kTowerBaseSeq5, kTowerBaseSeq6,
            kTowerBaseSeq7, kTowerBaseSeq8 } },
        /* TrackElemType::towerSection */ { 2, { kTowerSectionSeq0, kTowerSectionSeq1 } },
        /* TrackElemType::flatCovered */ { 1, { kFlatCoveredSeq0 } },
        /* TrackElemType::up25Covered */ { 1, { kUp25CoveredSeq0 } },
        /* TrackElemType::up60Covered */ { 1, { kUp60CoveredSeq0 } },
        /* TrackElemType::flatToUp25Covered */ { 1, { kFlatToUp25CoveredSeq0 } },
        /* TrackElemType::up25ToUp60Covered */ { 1, { kUp25ToUp60CoveredSeq0 } },
        /* TrackElemType::up60ToUp25Covered */ { 1, { kUp60ToUp25CoveredSeq0 } },
        /* TrackElemType::up25ToFlatCovered */ { 1, { kUp25ToFlatCoveredSeq0 } },
        /* TrackElemType::down25Covered */ { 1, { kDown25CoveredSeq0 } },
        /* TrackElemType::down60Covered */ { 1, { kDown60CoveredSeq0 } },
        /* TrackElemType::flatToDown25Covered */ { 1, { kFlatToDown25CoveredSeq0 } },
        /* TrackElemType::down25ToDown60Covered */ { 1, { kDown25ToDown60CoveredSeq0 } },
        /* TrackElemType::down60ToDown25Covered */ { 1, { kDown60ToDown25CoveredSeq0 } },
        /* TrackElemType::down25ToFlatCovered */ { 1, { kDown25ToFlatCoveredSeq0 } },
        /* TrackElemType::leftQuarterTurn5TilesCovered */
        { 7,
          { kLeftQuarterTurn5TilesCoveredSeq0, kLeftQuarterTurn5TilesCoveredSeq1, kLeftQuarterTurn5TilesCoveredSeq2,
            kLeftQuarterTurn5TilesCoveredSeq3, kLeftQuarterTurn5TilesCoveredSeq4, kLeftQuarterTurn5TilesCoveredSeq5,
            kLeftQuarterTurn5TilesCoveredSeq6 } },
        /* TrackElemType::rightQuarterTurn5TilesCovered */
        { 7,
          { kRightQuarterTurn5TilesCoveredSeq0, kRightQuarterTurn5TilesCoveredSeq1, kRightQuarterTurn5TilesCoveredSeq2,
            kRightQuarterTurn5TilesCoveredSeq3, kRightQuarterTurn5TilesCoveredSeq4, kRightQuarterTurn5TilesCoveredSeq5,
            kRightQuarterTurn5TilesCoveredSeq6 } },
        /* TrackElemType::sBendLeftCovered */
        { 4, { kSBendLeftCoveredSeq0, kSBendLeftCoveredSeq1, kSBendLeftCoveredSeq2, kSBendLeftCoveredSeq3 } },
        /* TrackElemType::sBendRightCovered */
        { 4, { kSBendRightCoveredSeq0, kSBendRightCoveredSeq1, kSBendRightCoveredSeq2, kSBendRightCoveredSeq3 } },
        /* TrackElemType::leftQuarterTurn3TilesCovered */
        { 4,
          { kLeftQuarterTurn3TilesCoveredSeq0, kLeftQuarterTurn3TilesCoveredSeq1, kLeftQuarterTurn3TilesCoveredSeq2,
            kLeftQuarterTurn3TilesCoveredSeq3 } },
        /* TrackElemType::rightQuarterTurn3TilesCovered */
        { 4,
          { kRightQuarterTurn3TilesCoveredSeq0, kRightQuarterTurn3TilesCoveredSeq1, kRightQuarterTurn3TilesCoveredSeq2,
            kRightQuarterTurn3TilesCoveredSeq3 } },
        /* TrackElemType::leftHalfBankedHelixUpSmall */
        { 8,
          { kLeftHalfBankedHelixUpSmallSeq0, kLeftHalfBankedHelixUpSmallSeq1, kLeftHalfBankedHelixUpSmallSeq2,
            kLeftHalfBankedHelixUpSmallSeq3, kLeftHalfBankedHelixUpSmallSeq4, kLeftHalfBankedHelixUpSmallSeq5,
            kLeftHalfBankedHelixUpSmallSeq6, kLeftHalfBankedHelixUpSmallSeq7 } },
        /* TrackElemType::rightHalfBankedHelixUpSmall */
        { 8,
          { kRightHalfBankedHelixUpSmallSeq0, kRightHalfBankedHelixUpSmallSeq1, kRightHalfBankedHelixUpSmallSeq2,
            kRightHalfBankedHelixUpSmallSeq3, kRightHalfBankedHelixUpSmallSeq4, kRightHalfBankedHelixUpSmallSeq5,
            kRightHalfBankedHelixUpSmallSeq6, kRightHalfBankedHelixUpSmallSeq7 } },
        /* TrackElemType::leftHalfBankedHelixDownSmall */
        { 8,
          { kLeftHalfBankedHelixDownSmallSeq0, kLeftHalfBankedHelixDownSmallSeq1, kLeftHalfBankedHelixDownSmallSeq2,
            kLeftHalfBankedHelixDownSmallSeq3, kLeftHalfBankedHelixDownSmallSeq4, kLeftHalfBankedHelixDownSmallSeq5,
            kLeftHalfBankedHelixDownSmallSeq6, kLeftHalfBankedHelixDownSmallSeq7 } },
        /* TrackElemType::rightHalfBankedHelixDownSmall */
        { 8,
          { kRightHalfBankedHelixDownSmallSeq0, kRightHalfBankedHelixDownSmallSeq1, kRightHalfBankedHelixDownSmallSeq2,
            kRightHalfBankedHelixDownSmallSeq3, kRightHalfBankedHelixDownSmallSeq4, kRightHalfBankedHelixDownSmallSeq5,
            kRightHalfBankedHelixDownSmallSeq6, kRightHalfBankedHelixDownSmallSeq7 } },
        /* TrackElemType::leftHalfBankedHelixUpLarge */
        { 14,
          { kLeftHalfBankedHelixUpLargeSeq0, kLeftHalfBankedHelixUpLargeSeq1, kLeftHalfBankedHelixUpLargeSeq2,
            kLeftHalfBankedHelixUpLargeSeq3, kLeftHalfBankedHelixUpLargeSeq4, kLeftHalfBankedHelixUpLargeSeq5,
            kLeftHalfBankedHelixUpLargeSeq6, kLeftHalfBankedHelixUpLargeSeq7, kLeftHalfBankedHelixUpLargeSeq8,
            kLeftHalfBankedHelixUpLargeSeq9, kLeftHalfBankedHelixUpLargeSeq10, kLeftHalfBankedHelixUpLargeSeq11,
            kLeftHalfBankedHelixUpLargeSeq12, kLeftHalfBankedHelixUpLargeSeq13 } },
        /* TrackElemType::rightHalfBankedHelixUpLarge */
        { 14,
          { kRightHalfBankedHelixUpLargeSeq0, kRightHalfBankedHelixUpLargeSeq1, kRightHalfBankedHelixUpLargeSeq2,
            kRightHalfBankedHelixUpLargeSeq3, kRightHalfBankedHelixUpLargeSeq4, kRightHalfBankedHelixUpLargeSeq5,
            kRightHalfBankedHelixUpLargeSeq6, kRightHalfBankedHelixUpLargeSeq7, kRightHalfBankedHelixUpLargeSeq8,
            kRightHalfBankedHelixUpLargeSeq9, kRightHalfBankedHelixUpLargeSeq10, kRightHalfBankedHelixUpLargeSeq11,
            kRightHalfBankedHelixUpLargeSeq12, kRightHalfBankedHelixUpLargeSeq13 } },
        /* TrackElemType::leftHalfBankedHelixDownLarge */
        { 14,
          { kLeftHalfBankedHelixDownLargeSeq0, kLeftHalfBankedHelixDownLargeSeq1, kLeftHalfBankedHelixDownLargeSeq2,
            kLeftHalfBankedHelixDownLargeSeq3, kLeftHalfBankedHelixDownLargeSeq4, kLeftHalfBankedHelixDownLargeSeq5,
            kLeftHalfBankedHelixDownLargeSeq6, kLeftHalfBankedHelixDownLargeSeq7, kLeftHalfBankedHelixDownLargeSeq8,
            kLeftHalfBankedHelixDownLargeSeq9, kLeftHalfBankedHelixDownLargeSeq10, kLeftHalfBankedHelixDownLargeSeq11,
            kLeftHalfBankedHelixDownLargeSeq12, kLeftHalfBankedHelixDownLargeSeq13 } },
        /* TrackElemType::rightHalfBankedHelixDownLarge */
        { 14,
          { kRightHalfBankedHelixDownLargeSeq0, kRightHalfBankedHelixDownLargeSeq1, kRightHalfBankedHelixDownLargeSeq2,
            kRightHalfBankedHelixDownLargeSeq3, kRightHalfBankedHelixDownLargeSeq4, kRightHalfBankedHelixDownLargeSeq5,
            kRightHalfBankedHelixDownLargeSeq6, kRightHalfBankedHelixDownLargeSeq7, kRightHalfBankedHelixDownLargeSeq8,
            kRightHalfBankedHelixDownLargeSeq9, kRightHalfBankedHelixDownLargeSeq10, kRightHalfBankedHelixDownLargeSeq11,
            kRightHalfBankedHelixDownLargeSeq12, kRightHalfBankedHelixDownLargeSeq13 } },
        /* TrackElemType::leftQuarterTurn1TileUp60 */ { 1, { kLeftQuarterTurn1TileUp60Seq0 } },
        /* TrackElemType::rightQuarterTurn1TileUp60 */ { 1, { kRightQuarterTurn1TileUp60Seq0 } },
        /* TrackElemType::leftQuarterTurn1TileDown60 */ { 1, { kLeftQuarterTurn1TileDown60Seq0 } },
        /* TrackElemType::rightQuarterTurn1TileDown60 */ { 1, { kRightQuarterTurn1TileDown60Seq0 } },
        /* TrackElemType::brakes */ { 1, { kBrakesSeq0 } },
        /* TrackElemType::booster */ { 1, { kBoosterSeq0 } },
        /* TrackElemType::maze */
        { 1,
          { kMazeSeq0, kMazeSeq1, kMazeSeq2, kMazeSeq3, kMazeSeq4, kMazeSeq5, kMazeSeq6, kMazeSeq7, kMazeSeq8, kMazeSeq9,
            kMazeSeq10, kMazeSeq11, kMazeSeq12, kMazeSeq13, kMazeSeq14, kMazeSeq15 } },
        /* TrackElemType::leftQuarterBankedHelixLargeUp */
        { 7,
          { kLeftQuarterBankedHelixLargeUpSeq0, kLeftQuarterBankedHelixLargeUpSeq1, kLeftQuarterBankedHelixLargeUpSeq2,
            kLeftQuarterBankedHelixLargeUpSeq3, kLeftQuarterBankedHelixLargeUpSeq4, kLeftQuarterBankedHelixLargeUpSeq5,
            kLeftQuarterBankedHelixLargeUpSeq6 } },
        /* TrackElemType::rightQuarterBankedHelixLargeUp */
        { 7,
          { kRightQuarterBankedHelixLargeUpSeq0, kRightQuarterBankedHelixLargeUpSeq1, kRightQuarterBankedHelixLargeUpSeq2,
            kRightQuarterBankedHelixLargeUpSeq3, kRightQuarterBankedHelixLargeUpSeq4, kRightQuarterBankedHelixLargeUpSeq5,
            kRightQuarterBankedHelixLargeUpSeq6 } },
        /* TrackElemType::leftQuarterBankedHelixLargeDown */
        { 7,
          { kLeftQuarterBankedHelixLargeDownSeq0, kLeftQuarterBankedHelixLargeDownSeq1, kLeftQuarterBankedHelixLargeDownSeq2,
            kLeftQuarterBankedHelixLargeDownSeq3, kLeftQuarterBankedHelixLargeDownSeq4, kLeftQuarterBankedHelixLargeDownSeq5,
            kLeftQuarterBankedHelixLargeDownSeq6 } },
        /* TrackElemType::rightQuarterBankedHelixLargeDown */
        { 7,
          { kRightQuarterBankedHelixLargeDownSeq0, kRightQuarterBankedHelixLargeDownSeq1, kRightQuarterBankedHelixLargeDownSeq2,
            kRightQuarterBankedHelixLargeDownSeq3, kRightQuarterBankedHelixLargeDownSeq4, kRightQuarterBankedHelixLargeDownSeq5,
            kRightQuarterBankedHelixLargeDownSeq6 } },
        /* TrackElemType::leftQuarterHelixLargeUp */
        { 7,
          { kLeftQuarterHelixLargeUpSeq0, kLeftQuarterHelixLargeUpSeq1, kLeftQuarterHelixLargeUpSeq2,
            kLeftQuarterHelixLargeUpSeq3, kLeftQuarterHelixLargeUpSeq4, kLeftQuarterHelixLargeUpSeq5,
            kLeftQuarterHelixLargeUpSeq6 } },
        /* TrackElemType::rightQuarterHelixLargeUp */
        { 7,
          { kRightQuarterHelixLargeUpSeq0, kRightQuarterHelixLargeUpSeq1, kRightQuarterHelixLargeUpSeq2,
            kRightQuarterHelixLargeUpSeq3, kRightQuarterHelixLargeUpSeq4, kRightQuarterHelixLargeUpSeq5,
            kRightQuarterHelixLargeUpSeq6 } },
        /* TrackElemType::leftQuarterHelixLargeDown */
        { 7,
          { kLeftQuarterHelixLargeDownSeq0, kLeftQuarterHelixLargeDownSeq1, kLeftQuarterHelixLargeDownSeq2,
            kLeftQuarterHelixLargeDownSeq3, kLeftQuarterHelixLargeDownSeq4, kLeftQuarterHelixLargeDownSeq5,
            kLeftQuarterHelixLargeDownSeq6 } },
        /* TrackElemType::rightQuarterHelixLargeDown */
        { 7,
          { kRightQuarterHelixLargeDownSeq0, kRightQuarterHelixLargeDownSeq1, kRightQuarterHelixLargeDownSeq2,
            kRightQuarterHelixLargeDownSeq3, kRightQuarterHelixLargeDownSeq4, kRightQuarterHelixLargeDownSeq5,
            kRightQuarterHelixLargeDownSeq6 } },
        /* TrackElemType::up25LeftBanked */ { 1, { kUp25LeftBankedSeq0 } },
        /* TrackElemType::up25RightBanked */ { 1, { kUp25RightBankedSeq0 } },
        /* TrackElemType::waterfall */ { 1, { kWaterfallSeq0 } },
        /* TrackElemType::rapids */ { 1, { kRapidsSeq0 } },
        /* TrackElemType::onRidePhoto */ { 1, { kOnRidePhotoSeq0 } },
        /* TrackElemType::down25LeftBanked */ { 1, { kDown25LeftBankedSeq0 } },
        /* TrackElemType::down25RightBanked */ { 1, { kDown25RightBankedSeq0 } },
        /* TrackElemType::watersplash */
        { 5, { kWatersplashSeq0, kWatersplashSeq1, kWatersplashSeq2, kWatersplashSeq3, kWatersplashSeq4 } },
        /* TrackElemType::flatToUp60LongBase */
        { 4, { kFlatToUp60LongBaseSeq0, kFlatToUp60LongBaseSeq1, kFlatToUp60LongBaseSeq2, kFlatToUp60LongBaseSeq3 } },
        /* TrackElemType::up60ToFlatLongBase */
        { 4, { kUp60ToFlatLongBaseSeq0, kUp60ToFlatLongBaseSeq1, kUp60ToFlatLongBaseSeq2, kUp60ToFlatLongBaseSeq3 } },
        /* TrackElemType::whirlpool */ { 1, { kWhirlpoolSeq0 } },
        /* TrackElemType::down60ToFlatLongBase */
        { 4, { kDown60ToFlatLongBaseSeq0, kDown60ToFlatLongBaseSeq1, kDown60ToFlatLongBaseSeq2, kDown60ToFlatLongBaseSeq3 } },
        /* TrackElemType::flatToDown60LongBase */
        { 4, { kFlatToDown60LongBaseSeq0, kFlatToDown60LongBaseSeq1, kFlatToDown60LongBaseSeq2, kFlatToDown60LongBaseSeq3 } },
        /* TrackElemType::cableLiftHill */
        { 4, { kCableLiftHillSeq0, kCableLiftHillSeq1, kCableLiftHillSeq2, kCableLiftHillSeq3 } },
        /* TrackElemType::reverseFreefallSlope */
        { 7,
          { kReverseFreefallSlopeSeq0, kReverseFreefallSlopeSeq1, kReverseFreefallSlopeSeq2, kReverseFreefallSlopeSeq3,
            kReverseFreefallSlopeSeq4, kReverseFreefallSlopeSeq5, kReverseFreefallSlopeSeq6 } },
        /* TrackElemType::reverseFreefallVertical */ { 2, { kReverseFreefallVerticalSeq0, kReverseFreefallVerticalSeq1 } },
        /* TrackElemType::up90 */ { 2, { kUp90Seq0, kUp90Seq1 } },
        /* TrackElemType::down90 */ { 2, { kDown90Seq0, kDown90Seq1 } },
        /* TrackElemType::up60ToUp90 */ { 2, { kUp60ToUp90Seq0, kUp60ToUp90Seq1 } },
        /* TrackElemType::down90ToDown60 */ { 1, { kDown90ToDown60Seq0 } },
        /* TrackElemType::up90ToUp60 */ { 1, { kUp90ToUp60Seq0 } },
        /* TrackElemType::down60ToDown90 */ { 2, { kDown60ToDown90Seq0, kDown60ToDown90Seq1 } },
        /* TrackElemType::brakeForDrop */ { 1, { kBrakeForDropSeq0 } },
        /* TrackElemType::leftEighthToDiag */
        { 5,
          { kLeftEighthToDiagSeq0, kLeftEighthToDiagSeq1, kLeftEighthToDiagSeq2, kLeftEighthToDiagSeq3,
            kLeftEighthToDiagSeq4 } },
        /* TrackElemType::rightEighthToDiag */
        { 5,
          { kRightEighthToDiagSeq0, kRightEighthToDiagSeq1, kRightEighthToDiagSeq2, kRightEighthToDiagSeq3,
            kRightEighthToDiagSeq4 } },
        /* TrackElemType::leftEighthToOrthogonal */
        { 5,
          { kLeftEighthToOrthogonalSeq0, kLeftEighthToOrthogonalSeq1, kLeftEighthToOrthogonalSeq2, kLeftEighthToOrthogonalSeq3,
            kLeftEighthToOrthogonalSeq4 } },
        /* TrackElemType::rightEighthToOrthogonal */
        { 5,
          { kRightEighthToOrthogonalSeq0, kRightEighthToOrthogonalSeq1, kRightEighthToOrthogonalSeq2,
            kRightEighthToOrthogonalSeq3, kRightEighthToOrthogonalSeq4 } },
        /* TrackElemType::leftEighthBankToDiag */
        { 5,
          { kLeftEighthBankToDiagSeq0, kLeftEighthBankToDiagSeq1, kLeftEighthBankToDiagSeq2, kLeftEighthBankToDiagSeq3,
            kLeftEighthBankToDiagSeq4 } },
        /* TrackElemType::rightEighthBankToDiag */
        { 5,
          { kRightEighthBankToDiagSeq0, kRightEighthBankToDiagSeq1, kRightEighthBankToDiagSeq2, kRightEighthBankToDiagSeq3,
            kRightEighthBankToDiagSeq4 } },
        /* TrackElemType::leftEighthBankToOrthogonal */
        { 5,
          { kLeftEighthBankToOrthogonalSeq0, kLeftEighthBankToOrthogonalSeq1, kLeftEighthBankToOrthogonalSeq2,
            kLeftEighthBankToOrthogonalSeq3, kLeftEighthBankToOrthogonalSeq4 } },
        /* TrackElemType::rightEighthBankToOrthogonal */
        { 5,
          { kRightEighthBankToOrthogonalSeq0, kRightEighthBankToOrthogonalSeq1, kRightEighthBankToOrthogonalSeq2,
            kRightEighthBankToOrthogonalSeq3, kRightEighthBankToOrthogonalSeq4 } },
        /* TrackElemType::diagFlat */ { 4, { kDiagFlatSeq0, kDiagFlatSeq1, kDiagFlatSeq2, kDiagFlatSeq3 } },
        /* TrackElemType::diagUp25 */ { 4, { kDiagUp25Seq0, kDiagUp25Seq1, kDiagUp25Seq2, kDiagUp25Seq3 } },
        /* TrackElemType::diagUp60 */ { 4, { kDiagUp60Seq0, kDiagUp60Seq1, kDiagUp60Seq2, kDiagUp60Seq3 } },
        /* TrackElemType::diagFlatToUp25 */
        { 4, { kDiagFlatToUp25Seq0, kDiagFlatToUp25Seq1, kDiagFlatToUp25Seq2, kDiagFlatToUp25Seq3 } },
        /* TrackElemType::diagUp25ToUp60 */
        { 4, { kDiagUp25ToUp60Seq0, kDiagUp25ToUp60Seq1, kDiagUp25ToUp60Seq2, kDiagUp25ToUp60Seq3 } },
        /* TrackElemType::diagUp60ToUp25 */
        { 4, { kDiagUp60ToUp25Seq0, kDiagUp60ToUp25Seq1, kDiagUp60ToUp25Seq2, kDiagUp60ToUp25Seq3 } },
        /* TrackElemType::diagUp25ToFlat */
        { 4, { kDiagUp25ToFlatSeq0, kDiagUp25ToFlatSeq1, kDiagUp25ToFlatSeq2, kDiagUp25ToFlatSeq3 } },
        /* TrackElemType::diagDown25 */ { 4, { kDiagDown25Seq0, kDiagDown25Seq1, kDiagDown25Seq2, kDiagDown25Seq3 } },
        /* TrackElemType::diagDown60 */ { 4, { kDiagDown60Seq0, kDiagDown60Seq1, kDiagDown60Seq2, kDiagDown60Seq3 } },
        /* TrackElemType::diagFlatToDown25 */
        { 4, { kDiagFlatToDown25Seq0, kDiagFlatToDown25Seq1, kDiagFlatToDown25Seq2, kDiagFlatToDown25Seq3 } },
        /* TrackElemType::diagDown25ToDown60 */
        { 4, { kDiagDown25ToDown60Seq0, kDiagDown25ToDown60Seq1, kDiagDown25ToDown60Seq2, kDiagDown25ToDown60Seq3 } },
        /* TrackElemType::diagDown60ToDown25 */
        { 4, { kDiagDown60ToDown25Seq0, kDiagDown60ToDown25Seq1, kDiagDown60ToDown25Seq2, kDiagDown60ToDown25Seq3 } },
        /* TrackElemType::diagDown25ToFlat */
        { 4, { kDiagDown25ToFlatSeq0, kDiagDown25ToFlatSeq1, kDiagDown25ToFlatSeq2, kDiagDown25ToFlatSeq3 } },
        /* TrackElemType::diagFlatToUp60 */
        { 4, { kDiagFlatToUp60Seq0, kDiagFlatToUp60Seq1, kDiagFlatToUp60Seq2, kDiagFlatToUp60Seq3 } },
        /* TrackElemType::diagUp60ToFlat */
        { 4, { kDiagUp60ToFlatSeq0, kDiagUp60ToFlatSeq1, kDiagUp60ToFlatSeq2, kDiagUp60ToFlatSeq3 } },
        /* TrackElemType::diagFlatToDown60 */
        { 4, { kDiagFlatToDown60Seq0, kDiagFlatToDown60Seq1, kDiagFlatToDown60Seq2, kDiagFlatToDown60Seq3 } },
        /* TrackElemType::diagDown60ToFlat */
        { 4, { kDiagDown60ToFlatSeq0, kDiagDown60ToFlatSeq1, kDiagDown60ToFlatSeq2, kDiagDown60ToFlatSeq3 } },
        /* TrackElemType::diagFlatToLeftBank */
        { 4, { kDiagFlatToLeftBankSeq0, kDiagFlatToLeftBankSeq1, kDiagFlatToLeftBankSeq2, kDiagFlatToLeftBankSeq3 } },
        /* TrackElemType::diagFlatToRightBank */
        { 4, { kDiagFlatToRightBankSeq0, kDiagFlatToRightBankSeq1, kDiagFlatToRightBankSeq2, kDiagFlatToRightBankSeq3 } },
        /* TrackElemType::diagLeftBankToFlat */
        { 4, { kDiagLeftBankToFlatSeq0, kDiagLeftBankToFlatSeq1, kDiagLeftBankToFlatSeq2, kDiagLeftBankToFlatSeq3 } },
        /* TrackElemType::diagRightBankToFlat */
        { 4, { kDiagRightBankToFlatSeq0, kDiagRightBankToFlatSeq1, kDiagRightBankToFlatSeq2, kDiagRightBankToFlatSeq3 } },
        /* TrackElemType::diagLeftBankToUp25 */
        { 4, { kDiagLeftBankToUp25Seq0, kDiagLeftBankToUp25Seq1, kDiagLeftBankToUp25Seq2, kDiagLeftBankToUp25Seq3 } },
        /* TrackElemType::diagRightBankToUp25 */
        { 4, { kDiagRightBankToUp25Seq0, kDiagRightBankToUp25Seq1, kDiagRightBankToUp25Seq2, kDiagRightBankToUp25Seq3 } },
        /* TrackElemType::diagUp25ToLeftBank */
        { 4, { kDiagUp25ToLeftBankSeq0, kDiagUp25ToLeftBankSeq1, kDiagUp25ToLeftBankSeq2, kDiagUp25ToLeftBankSeq3 } },
        /* TrackElemType::diagUp25ToRightBank */
        { 4, { kDiagUp25ToRightBankSeq0, kDiagUp25ToRightBankSeq1, kDiagUp25ToRightBankSeq2, kDiagUp25ToRightBankSeq3 } },
        /* TrackElemType::diagLeftBankToDown25 */
        { 4, { kDiagLeftBankToDown25Seq0, kDiagLeftBankToDown25Seq1, kDiagLeftBankToDown25Seq2, kDiagLeftBankToDown25Seq3 } },
        /* TrackElemType::diagRightBankToDown25 */
        { 4,
          { kDiagRightBankToDown25Seq0, kDiagRightBankToDown25Seq1, kDiagRightBankToDown25Seq2, kDiagRightBankToDown25Seq3 } },
        /* TrackElemType::diagDown25ToLeftBank */
        { 4, { kDiagDown25ToLeftBankSeq0, kDiagDown25ToLeftBankSeq1, kDiagDown25ToLeftBankSeq2, kDiagDown25ToLeftBankSeq3 } },
        /* TrackElemType::diagDown25ToRightBank */
        { 4,
          { kDiagDown25ToRightBankSeq0, kDiagDown25ToRightBankSeq1, kDiagDown25ToRightBankSeq2, kDiagDown25ToRightBankSeq3 } },
        /* TrackElemType::diagLeftBank */ { 4, { kDiagLeftBankSeq0, kDiagLeftBankSeq1, kDiagLeftBankSeq2, kDiagLeftBankSeq3 } },
        /* TrackElemType::diagRightBank */
        { 4, { kDiagRightBankSeq0, kDiagRightBankSeq1, kDiagRightBankSeq2, kDiagRightBankSeq3 } },
        /* TrackElemType::logFlumeReverser */ { 1, { kLogFlumeReverserSeq0 } },
        /* TrackElemType::spinningTunnel */ { 1, { kSpinningTunnelSeq0 } },
        /* TrackElemType::leftBarrelRollUpToDown */
        { 3, { kLeftBarrelRollUpToDownSeq0, kLeftBarrelRollUpToDownSeq1, kLeftBarrelRollUpToDownSeq2 } },
        /* TrackElemType::rightBarrelRollUpToDown */
        { 3, { kRightBarrelRollUpToDownSeq0, kRightBarrelRollUpToDownSeq1, kRightBarrelRollUpToDownSeq2 } },
        /* TrackElemType::leftBarrelRollDownToUp */
        { 3, { kLeftBarrelRollDownToUpSeq0, kLeftBarrelRollDownToUpSeq1, kLeftBarrelRollDownToUpSeq2 } },
        /* TrackElemType::rightBarrelRollDownToUp */
        { 3, { kRightBarrelRollDownToUpSeq0, kRightBarrelRollDownToUpSeq1, kRightBarrelRollDownToUpSeq2 } },
        /* TrackElemType::leftBankToLeftQuarterTurn3TilesUp25 */
        { 4,
          { kLeftBankToLeftQuarterTurn3TilesUp25Seq0, kLeftBankToLeftQuarterTurn3TilesUp25Seq1,
            kLeftBankToLeftQuarterTurn3TilesUp25Seq2, kLeftBankToLeftQuarterTurn3TilesUp25Seq3 } },
        /* TrackElemType::rightBankToRightQuarterTurn3TilesUp25 */
        { 4,
          { kRightBankToRightQuarterTurn3TilesUp25Seq0, kRightBankToRightQuarterTurn3TilesUp25Seq1,
            kRightBankToRightQuarterTurn3TilesUp25Seq2, kRightBankToRightQuarterTurn3TilesUp25Seq3 } },
        /* TrackElemType::leftQuarterTurn3TilesDown25ToLeftBank */
        { 4,
          { kLeftQuarterTurn3TilesDown25ToLeftBankSeq0, kLeftQuarterTurn3TilesDown25ToLeftBankSeq1,
            kLeftQuarterTurn3TilesDown25ToLeftBankSeq2, kLeftQuarterTurn3TilesDown25ToLeftBankSeq3 } },
        /* TrackElemType::rightQuarterTurn3TilesDown25ToRightBank */
        { 4,
          { kRightQuarterTurn3TilesDown25ToRightBankSeq0, kRightQuarterTurn3TilesDown25ToRightBankSeq1,
            kRightQuarterTurn3TilesDown25ToRightBankSeq2, kRightQuarterTurn3TilesDown25ToRightBankSeq3 } },
        /* TrackElemType::poweredLift */ { 1, { kPoweredLiftSeq0 } },
        /* TrackElemType::leftLargeHalfLoopUp */
        { 7,
          { kLeftLargeHalfLoopUpSeq0, kLeftLargeHalfLoopUpSeq1, kLeftLargeHalfLoopUpSeq2, kLeftLargeHalfLoopUpSeq3,
            kLeftLargeHalfLoopUpSeq4, kLeftLargeHalfLoopUpSeq5, kLeftLargeHalfLoopUpSeq6 } },
        /* TrackElemType::rightLargeHalfLoopUp */
        { 7,
          { kRightLargeHalfLoopUpSeq0, kRightLargeHalfLoopUpSeq1, kRightLargeHalfLoopUpSeq2, kRightLargeHalfLoopUpSeq3,
            kRightLargeHalfLoopUpSeq4, kRightLargeHalfLoopUpSeq5, kRightLargeHalfLoopUpSeq6 } },
        /* TrackElemType::leftLargeHalfLoopDown */
        { 7,
          { kLeftLargeHalfLoopDownSeq0, kLeftLargeHalfLoopDownSeq1, kLeftLargeHalfLoopDownSeq2, kLeftLargeHalfLoopDownSeq3,
            kLeftLargeHalfLoopDownSeq4, kLeftLargeHalfLoopDownSeq5, kLeftLargeHalfLoopDownSeq6 } },
        /* TrackElemType::rightLargeHalfLoopDown */
        { 7,
          { kRightLargeHalfLoopDownSeq0, kRightLargeHalfLoopDownSeq1, kRightLargeHalfLoopDownSeq2, kRightLargeHalfLoopDownSeq3,
            kRightLargeHalfLoopDownSeq4, kRightLargeHalfLoopDownSeq5, kRightLargeHalfLoopDownSeq6 } },
        /* TrackElemType::leftFlyerTwistUp */ { 3, { kLeftFlyerTwistUpSeq0, kLeftFlyerTwistUpSeq1, kLeftFlyerTwistUpSeq2 } },
        /* TrackElemType::rightFlyerTwistUp */
        { 3, { kRightFlyerTwistUpSeq0, kRightFlyerTwistUpSeq1, kRightFlyerTwistUpSeq2 } },
        /* TrackElemType::leftFlyerTwistDown */
        { 3, { kLeftFlyerTwistDownSeq0, kLeftFlyerTwistDownSeq1, kLeftFlyerTwistDownSeq2 } },
        /* TrackElemType::rightFlyerTwistDown */
        { 3, { kRightFlyerTwistDownSeq0, kRightFlyerTwistDownSeq1, kRightFlyerTwistDownSeq2 } },
        /* TrackElemType::flyerHalfLoopUninvertedUp */
        { 4,
          { kFlyerHalfLoopUninvertedUpSeq0, kFlyerHalfLoopUninvertedUpSeq1, kFlyerHalfLoopUninvertedUpSeq2,
            kFlyerHalfLoopUninvertedUpSeq3 } },
        /* TrackElemType::flyerHalfLoopInvertedDown */
        { 4,
          { kFlyerHalfLoopInvertedDownSeq0, kFlyerHalfLoopInvertedDownSeq1, kFlyerHalfLoopInvertedDownSeq2,
            kFlyerHalfLoopInvertedDownSeq3 } },
        /* TrackElemType::leftFlyerCorkscrewUp */
        { 3, { kLeftFlyerCorkscrewUpSeq0, kLeftFlyerCorkscrewUpSeq1, kLeftFlyerCorkscrewUpSeq2 } },
        /* TrackElemType::rightFlyerCorkscrewUp */
        { 3, { kRightFlyerCorkscrewUpSeq0, kRightFlyerCorkscrewUpSeq1, kRightFlyerCorkscrewUpSeq2 } },
        /* TrackElemType::leftFlyerCorkscrewDown */
        { 3, { kLeftFlyerCorkscrewDownSeq0, kLeftFlyerCorkscrewDownSeq1, kLeftFlyerCorkscrewDownSeq2 } },
        /* TrackElemType::rightFlyerCorkscrewDown */
        { 3, { kRightFlyerCorkscrewDownSeq0, kRightFlyerCorkscrewDownSeq1, kRightFlyerCorkscrewDownSeq2 } },
        /* TrackElemType::heartLineTransferUp */
        { 4, { kHeartLineTransferUpSeq0, kHeartLineTransferUpSeq1, kHeartLineTransferUpSeq2, kHeartLineTransferUpSeq3 } },
        /* TrackElemType::heartLineTransferDown */
        { 4,
          { kHeartLineTransferDownSeq0, kHeartLineTransferDownSeq1, kHeartLineTransferDownSeq2, kHeartLineTransferDownSeq3 } },
        /* TrackElemType::leftHeartLineRoll */
        { 6,
          { kLeftHeartLineRollSeq0, kLeftHeartLineRollSeq1, kLeftHeartLineRollSeq2, kLeftHeartLineRollSeq3,
            kLeftHeartLineRollSeq4, kLeftHeartLineRollSeq5 } },
        /* TrackElemType::rightHeartLineRoll */
        { 6,
          { kRightHeartLineRollSeq0, kRightHeartLineRollSeq1, kRightHeartLineRollSeq2, kRightHeartLineRollSeq3,
            kRightHeartLineRollSeq4, kRightHeartLineRollSeq5 } },
        /* TrackElemType::minigolfHoleA */ { 2, { kMinigolfHoleASeq0, kMinigolfHoleASeq1 } },
        /* TrackElemType::minigolfHoleB */ { 2, { kMinigolfHoleBSeq0, kMinigolfHoleBSeq1 } },
        /* TrackElemType::minigolfHoleC */ { 2, { kMinigolfHoleCSeq0, kMinigolfHoleCSeq1 } },
        /* TrackElemType::minigolfHoleD */ { 3, { kMinigolfHoleDSeq0, kMinigolfHoleDSeq1, kMinigolfHoleDSeq2 } },
        /* TrackElemType::minigolfHoleE */ { 3, { kMinigolfHoleESeq0, kMinigolfHoleESeq1, kMinigolfHoleESeq2 } },
        /* TrackElemType::multiDimInvertedFlatToDown90QuarterLoop */
        { 4,
          { kMultiDimInvertedFlatToDown90QuarterLoopSeq0, kMultiDimInvertedFlatToDown90QuarterLoopSeq1,
            kMultiDimInvertedFlatToDown90QuarterLoopSeq2, kMultiDimInvertedFlatToDown90QuarterLoopSeq3 } },
        /* TrackElemType::up90ToInvertedFlatQuarterLoop */
        { 3, { kUp90ToInvertedFlatQuarterLoopSeq0, kUp90ToInvertedFlatQuarterLoopSeq1, kUp90ToInvertedFlatQuarterLoopSeq2 } },
        /* TrackElemType::invertedFlatToDown90QuarterLoop */
        { 4,
          { kInvertedFlatToDown90QuarterLoopSeq0, kInvertedFlatToDown90QuarterLoopSeq1, kInvertedFlatToDown90QuarterLoopSeq2,
            kInvertedFlatToDown90QuarterLoopSeq3 } },
        /* TrackElemType::leftCurvedLiftHill */
        { 4, { kLeftCurvedLiftHillSeq0, kLeftCurvedLiftHillSeq1, kLeftCurvedLiftHillSeq2, kLeftCurvedLiftHillSeq3 } },
        /* TrackElemType::rightCurvedLiftHill */
        { 4, { kRightCurvedLiftHillSeq0, kRightCurvedLiftHillSeq1, kRightCurvedLiftHillSeq2, kRightCurvedLiftHillSeq3 } },
        /* TrackElemType::leftReverser */
        { 6,
          { kLeftReverserSeq0, kLeftReverserSeq1, kLeftReverserSeq2, kLeftReverserSeq3, kLeftReverserSeq4,
            kLeftReverserSeq5 } },
        /* TrackElemType::rightReverser */
        { 6,
          { kRightReverserSeq0, kRightReverserSeq1, kRightReverserSeq2, kRightReverserSeq3, kRightReverserSeq4,
            kRightReverserSeq5 } },
        /* TrackElemType::airThrustTopCap */
        { 4, { kAirThrustTopCapSeq0, kAirThrustTopCapSeq1, kAirThrustTopCapSeq2, kAirThrustTopCapSeq3 } },
        /* TrackElemType::airThrustVerticalDown */ { 2, { kAirThrustVerticalDownSeq0, kAirThrustVerticalDownSeq1 } },
        /* TrackElemType::airThrustVerticalDownToLevel */
        { 7,
          { kAirThrustVerticalDownToLevelSeq0, kAirThrustVerticalDownToLevelSeq1, kAirThrustVerticalDownToLevelSeq2,
            kAirThrustVerticalDownToLevelSeq3, kAirThrustVerticalDownToLevelSeq4, kAirThrustVerticalDownToLevelSeq5,
            kAirThrustVerticalDownToLevelSeq6 } },
        /* TrackElemType::blockBrakes */ { 1, { kBlockBrakesSeq0 } },
        /* TrackElemType::leftBankedQuarterTurn3TileUp25 */
        { 4,
          { kLeftBankedQuarterTurn3TileUp25Seq0, kLeftBankedQuarterTurn3TileUp25Seq1, kLeftBankedQuarterTurn3TileUp25Seq2,
            kLeftBankedQuarterTurn3TileUp25Seq3 } },
        /* TrackElemType::rightBankedQuarterTurn3TileUp25 */
        { 4,
          { kRightBankedQuarterTurn3TileUp25Seq0, kRightBankedQuarterTurn3TileUp25Seq1, kRightBankedQuarterTurn3TileUp25Seq2,
            kRightBankedQuarterTurn3TileUp25Seq3 } },
        /* TrackElemType::leftBankedQuarterTurn3TileDown25 */
        { 4,
          { kLeftBankedQuarterTurn3TileDown25Seq0, kLeftBankedQuarterTurn3TileDown25Seq1, kLeftBankedQuarterTurn3TileDown25Seq2,
            kLeftBankedQuarterTurn3TileDown25Seq3 } },
        /* TrackElemType::rightBankedQuarterTurn3TileDown25 */
        { 4,
          { kRightBankedQuarterTurn3TileDown25Seq0, kRightBankedQuarterTurn3TileDown25Seq1,
            kRightBankedQuarterTurn3TileDown25Seq2, kRightBankedQuarterTurn3TileDown25Seq3 } },
        /* TrackElemType::leftBankedQuarterTurn5TileUp25 */
        { 7,
          { kLeftBankedQuarterTurn5TileUp25Seq0, kLeftBankedQuarterTurn5TileUp25Seq1, kLeftBankedQuarterTurn5TileUp25Seq2,
            kLeftBankedQuarterTurn5TileUp25Seq3, kLeftBankedQuarterTurn5TileUp25Seq4, kLeftBankedQuarterTurn5TileUp25Seq5,
            kLeftBankedQuarterTurn5TileUp25Seq6, kLeftBankedQuarterTurn5TileUp25Seq7 } },
        /* TrackElemType::rightBankedQuarterTurn5TileUp25 */
        { 7,
          { kRightBankedQuarterTurn5TileUp25Seq0, kRightBankedQuarterTurn5TileUp25Seq1, kRightBankedQuarterTurn5TileUp25Seq2,
            kRightBankedQuarterTurn5TileUp25Seq3, kRightBankedQuarterTurn5TileUp25Seq4, kRightBankedQuarterTurn5TileUp25Seq5,
            kRightBankedQuarterTurn5TileUp25Seq6, kRightBankedQuarterTurn5TileUp25Seq7 } },
        /* TrackElemType::leftBankedQuarterTurn5TileDown25 */
        { 7,
          { kLeftBankedQuarterTurn5TileDown25Seq0, kLeftBankedQuarterTurn5TileDown25Seq1, kLeftBankedQuarterTurn5TileDown25Seq2,
            kLeftBankedQuarterTurn5TileDown25Seq3, kLeftBankedQuarterTurn5TileDown25Seq4, kLeftBankedQuarterTurn5TileDown25Seq5,
            kLeftBankedQuarterTurn5TileDown25Seq6, kLeftBankedQuarterTurn5TileDown25Seq7 } },
        /* TrackElemType::rightBankedQuarterTurn5TileDown25 */
        { 7,
          { kRightBankedQuarterTurn5TileDown25Seq0, kRightBankedQuarterTurn5TileDown25Seq1,
            kRightBankedQuarterTurn5TileDown25Seq2, kRightBankedQuarterTurn5TileDown25Seq3,
            kRightBankedQuarterTurn5TileDown25Seq4, kRightBankedQuarterTurn5TileDown25Seq5,
            kRightBankedQuarterTurn5TileDown25Seq6, kRightBankedQuarterTurn5TileDown25Seq7 } },
        /* TrackElemType::up25ToLeftBankedUp25 */ { 1, { kUp25ToLeftBankedUp25Seq0 } },
        /* TrackElemType::up25ToRightBankedUp25 */ { 1, { kUp25ToRightBankedUp25Seq0 } },
        /* TrackElemType::leftBankedUp25ToUp25 */ { 1, { kLeftBankedUp25ToUp25Seq0 } },
        /* TrackElemType::rightBankedUp25ToUp25 */ { 1, { kRightBankedUp25ToUp25Seq0 } },
        /* TrackElemType::down25ToLeftBankedDown25 */ { 1, { kDown25ToLeftBankedDown25Seq0 } },
        /* TrackElemType::down25ToRightBankedDown25 */ { 1, { kDown25ToRightBankedDown25Seq0 } },
        /* TrackElemType::leftBankedDown25ToDown25 */ { 1, { kLeftBankedDown25ToDown25Seq0 } },
        /* TrackElemType::rightBankedDown25ToDown25 */ { 1, { kRightBankedDown25ToDown25Seq0 } },
        /* TrackElemType::leftBankedFlatToLeftBankedUp25 */ { 1, { kLeftBankedFlatToLeftBankedUp25Seq0 } },
        /* TrackElemType::rightBankedFlatToRightBankedUp25 */ { 1, { kRightBankedFlatToRightBankedUp25Seq0 } },
        /* TrackElemType::leftBankedUp25ToLeftBankedFlat */ { 1, { kLeftBankedUp25ToLeftBankedFlatSeq0 } },
        /* TrackElemType::rightBankedUp25ToRightBankedFlat */ { 1, { kRightBankedUp25ToRightBankedFlatSeq0 } },
        /* TrackElemType::leftBankedFlatToLeftBankedDown25 */ { 1, { kLeftBankedFlatToLeftBankedDown25Seq0 } },
        /* TrackElemType::rightBankedFlatToRightBankedDown25 */ { 1, { kRightBankedFlatToRightBankedDown25Seq0 } },
        /* TrackElemType::leftBankedDown25ToLeftBankedFlat */ { 1, { kLeftBankedDown25ToLeftBankedFlatSeq0 } },
        /* TrackElemType::rightBankedDown25ToRightBankedFlat */ { 1, { kRightBankedDown25ToRightBankedFlatSeq0 } },
        /* TrackElemType::flatToLeftBankedUp25 */ { 1, { kFlatToLeftBankedUp25Seq0 } },
        /* TrackElemType::flatToRightBankedUp25 */ { 1, { kFlatToRightBankedUp25Seq0 } },
        /* TrackElemType::leftBankedUp25ToFlat */ { 1, { kLeftBankedUp25ToFlatSeq0 } },
        /* TrackElemType::rightBankedUp25ToFlat */ { 1, { kRightBankedUp25ToFlatSeq0 } },
        /* TrackElemType::flatToLeftBankedDown25 */ { 1, { kFlatToLeftBankedDown25Seq0 } },
        /* TrackElemType::flatToRightBankedDown25 */ { 1, { kFlatToRightBankedDown25Seq0 } },
        /* TrackElemType::leftBankedDown25ToFlat */ { 1, { kLeftBankedDown25ToFlatSeq0 } },
        /* TrackElemType::rightBankedDown25ToFlat */ { 1, { kRightBankedDown25ToFlatSeq0 } },
        /* TrackElemType::leftQuarterTurn1TileUp90 */ { 2, { kLeftQuarterTurn1TileUp90Seq0, kLeftQuarterTurn1TileUp90Seq1 } },
        /* TrackElemType::rightQuarterTurn1TileUp90 */
        { 2, { kRightQuarterTurn1TileUp90Seq0, kRightQuarterTurn1TileUp90Seq1 } },
        /* TrackElemType::leftQuarterTurn1TileDown90 */
        { 2, { kLeftQuarterTurn1TileDown90Seq0, kLeftQuarterTurn1TileDown90Seq1 } },
        /* TrackElemType::rightQuarterTurn1TileDown90 */
        { 2, { kRightQuarterTurn1TileDown90Seq0, kRightQuarterTurn1TileDown90Seq1 } },
        /* TrackElemType::multiDimUp90ToInvertedFlatQuarterLoop */
        { 3,
          { kMultiDimUp90ToInvertedFlatQuarterLoopSeq0, kMultiDimUp90ToInvertedFlatQuarterLoopSeq1,
            kMultiDimUp90ToInvertedFlatQuarterLoopSeq2 } },
        /* TrackElemType::multiDimFlatToDown90QuarterLoop */
        { 4,
          { kMultiDimFlatToDown90QuarterLoopSeq0, kMultiDimFlatToDown90QuarterLoopSeq1, kMultiDimFlatToDown90QuarterLoopSeq2,
            kMultiDimFlatToDown90QuarterLoopSeq3 } },
        /* TrackElemType::multiDimInvertedUp90ToFlatQuarterLoop */
        { 3,
          { kMultiDimInvertedUp90ToFlatQuarterLoopSeq0, kMultiDimInvertedUp90ToFlatQuarterLoopSeq1,
            kMultiDimInvertedUp90ToFlatQuarterLoopSeq2 } },
        /* TrackElemType::rotationControlToggle */ { 1, { kRotationControlToggleSeq0 } },
        /* TrackElemType::flatTrack1x4A */
        { 4, { kFlatTrack1x4ASeq0, kFlatTrack1x4ASeq1, kFlatTrack1x4ASeq2, kFlatTrack1x4ASeq3 } },
        /* TrackElemType::flatTrack2x2 */ { 4, { kFlatTrack2x2Seq0, kFlatTrack2x2Seq1, kFlatTrack2x2Seq2, kFlatTrack2x2Seq3 } },
        /* TrackElemType::flatTrack4x4 */
        { 16,
          { kFlatTrack4x4Seq0, kFlatTrack4x4Seq1, kFlatTrack4x4Seq2, kFlatTrack4x4Seq3, kFlatTrack4x4Seq4, kFlatTrack4x4Seq5,
            kFlatTrack4x4Seq6, kFlatTrack4x4Seq7, kFlatTrack4x4Seq8, kFlatTrack4x4Seq9, kFlatTrack4x4Seq10, kFlatTrack4x4Seq11,
            kFlatTrack4x4Seq12, kFlatTrack4x4Seq13, kFlatTrack4x4Seq14, kFlatTrack4x4Seq15 } },
        /* TrackElemType::flatTrack2x4 */
        { 8,
          { kFlatTrack2x4Seq0, kFlatTrack2x4Seq1, kFlatTrack2x4Seq2, kFlatTrack2x4Seq3, kFlatTrack2x4Seq4, kFlatTrack2x4Seq5,
            kFlatTrack2x4Seq6, kFlatTrack2x4Seq7 } },
        /* TrackElemType::flatTrack1x5 */
        { 5, { kFlatTrack1x5Seq0, kFlatTrack1x5Seq1, kFlatTrack1x5Seq2, kFlatTrack1x5Seq3, kFlatTrack1x5Seq4 } },
        /* TrackElemType::flatTrack1x1A */ { 1, { kFlatTrack1x1ASeq0 } },
        /* TrackElemType::flatTrack1x4B */
        { 4, { kFlatTrack1x4BSeq0, kFlatTrack1x4BSeq1, kFlatTrack1x4BSeq2, kFlatTrack1x4BSeq3 } },
        /* TrackElemType::flatTrack1x1B */ { 1, { kFlatTrack1x1BSeq0 } },
        /* TrackElemType::flatTrack1x4C */
        { 4, { kFlatTrack1x4CSeq0, kFlatTrack1x4CSeq1, kFlatTrack1x4CSeq2, kFlatTrack1x4CSeq3 } },
        /* TrackElemType::flatTrack3x3 */
        { 9,
          { kFlatTrack3x3Seq0, kFlatTrack3x3Seq1, kFlatTrack3x3Seq2, kFlatTrack3x3Seq3, kFlatTrack3x3Seq4, kFlatTrack3x3Seq5,
            kFlatTrack3x3Seq6, kFlatTrack3x3Seq7, kFlatTrack3x3Seq8 } },
        /* TrackElemType::leftLargeCorkscrewUp */
        { 6,
          { kLeftLargeCorkscrewUpSeq0, kLeftLargeCorkscrewUpSeq1, kLeftLargeCorkscrewUpSeq2, kLeftLargeCorkscrewUpSeq3,
            kLeftLargeCorkscrewUpSeq4, kLeftLargeCorkscrewUpSeq5 } },
        /* TrackElemType::rightLargeCorkscrewUp */
        { 6,
          { kRightLargeCorkscrewUpSeq0, kRightLargeCorkscrewUpSeq1, kRightLargeCorkscrewUpSeq2, kRightLargeCorkscrewUpSeq3,
            kRightLargeCorkscrewUpSeq4, kRightLargeCorkscrewUpSeq5 } },
        /* TrackElemType::leftLargeCorkscrewDown */
        { 6,
          { kLeftLargeCorkscrewDownSeq0, kLeftLargeCorkscrewDownSeq1, kLeftLargeCorkscrewDownSeq2, kLeftLargeCorkscrewDownSeq3,
            kLeftLargeCorkscrewDownSeq4, kLeftLargeCorkscrewDownSeq5 } },
        /* TrackElemType::rightLargeCorkscrewDown */
        { 6,
          { kRightLargeCorkscrewDownSeq0, kRightLargeCorkscrewDownSeq1, kRightLargeCorkscrewDownSeq2,
            kRightLargeCorkscrewDownSeq3, kRightLargeCorkscrewDownSeq4, kRightLargeCorkscrewDownSeq5 } },
        /* TrackElemType::leftMediumHalfLoopUp */
        { 5,
          { kLeftMediumHalfLoopUpSeq0, kLeftMediumHalfLoopUpSeq1, kLeftMediumHalfLoopUpSeq2, kLeftMediumHalfLoopUpSeq3,
            kLeftMediumHalfLoopUpSeq4 } },
        /* TrackElemType::rightMediumHalfLoopUp */
        { 5,
          { kRightMediumHalfLoopUpSeq0, kRightMediumHalfLoopUpSeq1, kRightMediumHalfLoopUpSeq2, kRightMediumHalfLoopUpSeq3,
            kRightMediumHalfLoopUpSeq4 } },
        /* TrackElemType::leftMediumHalfLoopDown */
        { 5,
          { kLeftMediumHalfLoopDownSeq0, kLeftMediumHalfLoopDownSeq1, kLeftMediumHalfLoopDownSeq2, kLeftMediumHalfLoopDownSeq3,
            kLeftMediumHalfLoopDownSeq4 } },
        /* TrackElemType::rightMediumHalfLoopDown */
        { 5,
          { kRightMediumHalfLoopDownSeq0, kRightMediumHalfLoopDownSeq1, kRightMediumHalfLoopDownSeq2,
            kRightMediumHalfLoopDownSeq3, kRightMediumHalfLoopDownSeq4 } },
        /* TrackElemType::leftZeroGRollUp */ { 3, { kLeftZeroGRollUpSeq0, kLeftZeroGRollUpSeq1, kLeftZeroGRollUpSeq2 } },
        /* TrackElemType::rightZeroGRollUp */ { 3, { kRightZeroGRollUpSeq0, kRightZeroGRollUpSeq1, kRightZeroGRollUpSeq2 } },
        /* TrackElemType::leftZeroGRollDown */
        { 3, { kLeftZeroGRollDownSeq0, kLeftZeroGRollDownSeq1, kLeftZeroGRollDownSeq2 } },
        /* TrackElemType::rightZeroGRollDown */
        { 3, { kRightZeroGRollDownSeq0, kRightZeroGRollDownSeq1, kRightZeroGRollDownSeq2 } },
        /* TrackElemType::leftLargeZeroGRollUp */
        { 4, { kLeftLargeZeroGRollUpSeq0, kLeftLargeZeroGRollUpSeq1, kLeftLargeZeroGRollUpSeq2, kLeftLargeZeroGRollUpSeq3 } },
        /* TrackElemType::rightLargeZeroGRollUp */
        { 4,
          { kRightLargeZeroGRollUpSeq0, kRightLargeZeroGRollUpSeq1, kRightLargeZeroGRollUpSeq2, kRightLargeZeroGRollUpSeq3 } },
        /* TrackElemType::leftLargeZeroGRollDown */
        { 4,
          { kLeftLargeZeroGRollDownSeq0, kLeftLargeZeroGRollDownSeq1, kLeftLargeZeroGRollDownSeq2,
            kLeftLargeZeroGRollDownSeq3 } },
        /* TrackElemType::rightLargeZeroGRollDown */
        { 4,
          { kRightLargeZeroGRollDownSeq0, kRightLargeZeroGRollDownSeq1, kRightLargeZeroGRollDownSeq2,
            kRightLargeZeroGRollDownSeq3 } },
        /* TrackElemType::leftFlyerLargeHalfLoopUninvertedUp */
        { 7,
          { kLeftFlyerLargeHalfLoopUninvertedUpSeq0, kLeftFlyerLargeHalfLoopUninvertedUpSeq1,
            kLeftFlyerLargeHalfLoopUninvertedUpSeq2, kLeftFlyerLargeHalfLoopUninvertedUpSeq3,
            kLeftFlyerLargeHalfLoopUninvertedUpSeq4, kLeftFlyerLargeHalfLoopUninvertedUpSeq5,
            kLeftFlyerLargeHalfLoopUninvertedUpSeq6 } },
        /* TrackElemType::rightFlyerLargeHalfLoopUninvertedUp */
        { 7,
          { kRightFlyerLargeHalfLoopUninvertedUpSeq0, kRightFlyerLargeHalfLoopUninvertedUpSeq1,
            kRightFlyerLargeHalfLoopUninvertedUpSeq2, kRightFlyerLargeHalfLoopUninvertedUpSeq3,
            kRightFlyerLargeHalfLoopUninvertedUpSeq4, kRightFlyerLargeHalfLoopUninvertedUpSeq5,
            kRightFlyerLargeHalfLoopUninvertedUpSeq6 } },
        /* TrackElemType::leftFlyerLargeHalfLoopInvertedDown */
        { 7,
          { kLeftFlyerLargeHalfLoopInvertedDownSeq0, kLeftFlyerLargeHalfLoopInvertedDownSeq1,
            kLeftFlyerLargeHalfLoopInvertedDownSeq2, kLeftFlyerLargeHalfLoopInvertedDownSeq3,
            kLeftFlyerLargeHalfLoopInvertedDownSeq4, kLeftFlyerLargeHalfLoopInvertedDownSeq5,
            kLeftFlyerLargeHalfLoopInvertedDownSeq6 } },
        /* TrackElemType::rightFlyerLargeHalfLoopInvertedDown */
        { 7,
          { kRightFlyerLargeHalfLoopInvertedDownSeq0, kRightFlyerLargeHalfLoopInvertedDownSeq1,
            kRightFlyerLargeHalfLoopInvertedDownSeq2, kRightFlyerLargeHalfLoopInvertedDownSeq3,
            kRightFlyerLargeHalfLoopInvertedDownSeq4, kRightFlyerLargeHalfLoopInvertedDownSeq5,
            kRightFlyerLargeHalfLoopInvertedDownSeq6 } },
        /* TrackElemType::leftFlyerLargeHalfLoopInvertedUp */
        { 7,
          { kLeftFlyerLargeHalfLoopInvertedUpSeq0, kLeftFlyerLargeHalfLoopInvertedUpSeq1, kLeftFlyerLargeHalfLoopInvertedUpSeq2,
            kLeftFlyerLargeHalfLoopInvertedUpSeq3, kLeftFlyerLargeHalfLoopInvertedUpSeq4, kLeftFlyerLargeHalfLoopInvertedUpSeq5,
            kLeftFlyerLargeHalfLoopInvertedUpSeq6 } },
        /* TrackElemType::rightFlyerLargeHalfLoopInvertedUp */
        { 7,
          { kRightFlyerLargeHalfLoopInvertedUpSeq0, kRightFlyerLargeHalfLoopInvertedUpSeq1,
            kRightFlyerLargeHalfLoopInvertedUpSeq2, kRightFlyerLargeHalfLoopInvertedUpSeq3,
            kRightFlyerLargeHalfLoopInvertedUpSeq4, kRightFlyerLargeHalfLoopInvertedUpSeq5,
            kRightFlyerLargeHalfLoopInvertedUpSeq6 } },
        /* TrackElemType::leftFlyerLargeHalfLoopUninvertedDown */
        { 7,
          { kLeftFlyerLargeHalfLoopUninvertedDownSeq0, kLeftFlyerLargeHalfLoopUninvertedDownSeq1,
            kLeftFlyerLargeHalfLoopUninvertedDownSeq2, kLeftFlyerLargeHalfLoopUninvertedDownSeq3,
            kLeftFlyerLargeHalfLoopUninvertedDownSeq4, kLeftFlyerLargeHalfLoopUninvertedDownSeq5,
            kLeftFlyerLargeHalfLoopUninvertedDownSeq6 } },
        /* TrackElemType::rightFlyerLargeHalfLoopUninvertedDown */
        { 7,
          { kRightFlyerLargeHalfLoopUninvertedDownSeq0, kRightFlyerLargeHalfLoopUninvertedDownSeq1,
            kRightFlyerLargeHalfLoopUninvertedDownSeq2, kRightFlyerLargeHalfLoopUninvertedDownSeq3,
            kRightFlyerLargeHalfLoopUninvertedDownSeq4, kRightFlyerLargeHalfLoopUninvertedDownSeq5,
            kRightFlyerLargeHalfLoopUninvertedDownSeq6 } },
        /* TrackElemType::flyerHalfLoopInvertedUp */
        { 4,
          { kFlyerHalfLoopInvertedUpSeq0, kFlyerHalfLoopInvertedUpSeq1, kFlyerHalfLoopInvertedUpSeq2,
            kFlyerHalfLoopInvertedUpSeq3 } },
        /* TrackElemType::flyerHalfLoopUninvertedDown */
        { 4,
          { kFlyerHalfLoopUninvertedDownSeq0, kFlyerHalfLoopUninvertedDownSeq1, kFlyerHalfLoopUninvertedDownSeq2,
            kFlyerHalfLoopUninvertedDownSeq3 } },
        /* TrackElemType::leftEighthToDiagUp25 */
        { 5,
          { kLeftEighthToDiagUp25Seq0, kLeftEighthToDiagUp25Seq1, kLeftEighthToDiagUp25Seq2, kLeftEighthToDiagUp25Seq3,
            kLeftEighthToDiagUp25Seq4 } },
        /* TrackElemType::rightEighthToDiagUp25 */
        { 5,
          { kRightEighthToDiagUp25Seq0, kRightEighthToDiagUp25Seq1, kRightEighthToDiagUp25Seq2, kRightEighthToDiagUp25Seq3,
            kRightEighthToDiagUp25Seq4 } },
        /* TrackElemType::leftEighthToDiagDown25 */
        { 5,
          { kLeftEighthToDiagDown25Seq0, kLeftEighthToDiagDown25Seq1, kLeftEighthToDiagDown25Seq2, kLeftEighthToDiagDown25Seq3,
            kLeftEighthToDiagDown25Seq4 } },
        /* TrackElemType::rightEighthToDiagDown25 */
        { 5,
          { kRightEighthToDiagDown25Seq0, kRightEighthToDiagDown25Seq1, kRightEighthToDiagDown25Seq2,
            kRightEighthToDiagDown25Seq3, kRightEighthToDiagDown25Seq4 } },
        /* TrackElemType::leftEighthToOrthogonalUp25 */
        { 5,
          { kLeftEighthToOrthogonalUp25Seq0, kLeftEighthToOrthogonalUp25Seq1, kLeftEighthToOrthogonalUp25Seq2,
            kLeftEighthToOrthogonalUp25Seq3, kLeftEighthToOrthogonalUp25Seq4 } },
        /* TrackElemType::rightEighthToOrthogonalUp25 */
        { 5,
          { kRightEighthToOrthogonalUp25Seq0, kRightEighthToOrthogonalUp25Seq1, kRightEighthToOrthogonalUp25Seq2,
            kRightEighthToOrthogonalUp25Seq3, kRightEighthToOrthogonalUp25Seq4 } },
        /* TrackElemType::leftEighthToOrthogonalDown25 */
        { 5,
          { kLeftEighthToOrthogonalDown25Seq0, kLeftEighthToOrthogonalDown25Seq1, kLeftEighthToOrthogonalDown25Seq2,
            kLeftEighthToOrthogonalDown25Seq3, kLeftEighthToOrthogonalDown25Seq4 } },
        /* TrackElemType::rightEighthToOrthogonalDown25 */
        { 5,
          { kRightEighthToOrthogonalDown25Seq0, kRightEighthToOrthogonalDown25Seq1, kRightEighthToOrthogonalDown25Seq2,
            kRightEighthToOrthogonalDown25Seq3, kRightEighthToOrthogonalDown25Seq4 } },
        /* TrackElemType::diagUp25ToLeftBankedUp25 */
        { 4,
          { kDiagUp25ToLeftBankedUp25Seq0, kDiagUp25ToLeftBankedUp25Seq1, kDiagUp25ToLeftBankedUp25Seq2,
            kDiagUp25ToLeftBankedUp25Seq3 } },
        /* TrackElemType::diagUp25ToRightBankedUp25 */
        { 4,
          { kDiagUp25ToRightBankedUp25Seq0, kDiagUp25ToRightBankedUp25Seq1, kDiagUp25ToRightBankedUp25Seq2,
            kDiagUp25ToRightBankedUp25Seq3 } },
        /* TrackElemType::diagLeftBankedUp25ToUp25 */
        { 4,
          { kDiagLeftBankedUp25ToUp25Seq0, kDiagLeftBankedUp25ToUp25Seq1, kDiagLeftBankedUp25ToUp25Seq2,
            kDiagLeftBankedUp25ToUp25Seq3 } },
        /* TrackElemType::diagRightBankedUp25ToUp25 */
        { 4,
          { kDiagRightBankedUp25ToUp25Seq0, kDiagRightBankedUp25ToUp25Seq1, kDiagRightBankedUp25ToUp25Seq2,
            kDiagRightBankedUp25ToUp25Seq3 } },
        /* TrackElemType::diagDown25ToLeftBankedDown25 */
        { 4,
          { kDiagDown25ToLeftBankedDown25Seq0, kDiagDown25ToLeftBankedDown25Seq1, kDiagDown25ToLeftBankedDown25Seq2,
            kDiagDown25ToLeftBankedDown25Seq3 } },
        /* TrackElemType::diagDown25ToRightBankedDown25 */
        { 4,
          { kDiagDown25ToRightBankedDown25Seq0, kDiagDown25ToRightBankedDown25Seq1, kDiagDown25ToRightBankedDown25Seq2,
            kDiagDown25ToRightBankedDown25Seq3 } },
        /* TrackElemType::diagLeftBankedDown25ToDown25 */
        { 4,
          { kDiagLeftBankedDown25ToDown25Seq0, kDiagLeftBankedDown25ToDown25Seq1, kDiagLeftBankedDown25ToDown25Seq2,
            kDiagLeftBankedDown25ToDown25Seq3 } },
        /* TrackElemType::diagRightBankedDown25ToDown25 */
        { 4,
          { kDiagRightBankedDown25ToDown25Seq0, kDiagRightBankedDown25ToDown25Seq1, kDiagRightBankedDown25ToDown25Seq2,
            kDiagRightBankedDown25ToDown25Seq3 } },
        /* TrackElemType::diagLeftBankedFlatToLeftBankedUp25 */
        { 4,
          { kDiagLeftBankedFlatToLeftBankedUp25Seq0, kDiagLeftBankedFlatToLeftBankedUp25Seq1,
            kDiagLeftBankedFlatToLeftBankedUp25Seq2, kDiagLeftBankedFlatToLeftBankedUp25Seq3 } },
        /* TrackElemType::diagRightBankedFlatToRightBankedUp25 */
        { 4,
          { kDiagRightBankedFlatToRightBankedUp25Seq0, kDiagRightBankedFlatToRightBankedUp25Seq1,
            kDiagRightBankedFlatToRightBankedUp25Seq2, kDiagRightBankedFlatToRightBankedUp25Seq3 } },
        /* TrackElemType::diagLeftBankedUp25ToLeftBankedFlat */
        { 4,
          { kDiagLeftBankedUp25ToLeftBankedFlatSeq0, kDiagLeftBankedUp25ToLeftBankedFlatSeq1,
            kDiagLeftBankedUp25ToLeftBankedFlatSeq2, kDiagLeftBankedUp25ToLeftBankedFlatSeq3 } },
        /* TrackElemType::diagRightBankedUp25ToRightBankedFlat */
        { 4,
          { kDiagRightBankedUp25ToRightBankedFlatSeq0, kDiagRightBankedUp25ToRightBankedFlatSeq1,
            kDiagRightBankedUp25ToRightBankedFlatSeq2, kDiagRightBankedUp25ToRightBankedFlatSeq3 } },
        /* TrackElemType::diagLeftBankedFlatToLeftBankedDown25 */
        { 4,
          { kDiagLeftBankedFlatToLeftBankedDown25Seq0, kDiagLeftBankedFlatToLeftBankedDown25Seq1,
            kDiagLeftBankedFlatToLeftBankedDown25Seq2, kDiagLeftBankedFlatToLeftBankedDown25Seq3 } },
        /* TrackElemType::diagRightBankedFlatToRightBankedDown25 */
        { 4,
          { kDiagRightBankedFlatToRightBankedDown25Seq0, kDiagRightBankedFlatToRightBankedDown25Seq1,
            kDiagRightBankedFlatToRightBankedDown25Seq2, kDiagRightBankedFlatToRightBankedDown25Seq3 } },
        /* TrackElemType::diagLeftBankedDown25ToLeftBankedFlat */
        { 4,
          { kDiagLeftBankedDown25ToLeftBankedFlatSeq0, kDiagLeftBankedDown25ToLeftBankedFlatSeq1,
            kDiagLeftBankedDown25ToLeftBankedFlatSeq2, kDiagLeftBankedDown25ToLeftBankedFlatSeq3 } },
        /* TrackElemType::diagRightBankedDown25ToRightBankedFlat */
        { 4,
          { kDiagRightBankedDown25ToRightBankedFlatSeq0, kDiagRightBankedDown25ToRightBankedFlatSeq1,
            kDiagRightBankedDown25ToRightBankedFlatSeq2, kDiagRightBankedDown25ToRightBankedFlatSeq3 } },
        /* TrackElemType::diagFlatToLeftBankedUp25 */
        { 4,
          { kDiagFlatToLeftBankedUp25Seq0, kDiagFlatToLeftBankedUp25Seq1, kDiagFlatToLeftBankedUp25Seq2,
            kDiagFlatToLeftBankedUp25Seq3 } },
        /* TrackElemType::diagFlatToRightBankedUp25 */
        { 4,
          { kDiagFlatToRightBankedUp25Seq0, kDiagFlatToRightBankedUp25Seq1, kDiagFlatToRightBankedUp25Seq2,
            kDiagFlatToRightBankedUp25Seq3 } },
        /* TrackElemType::diagLeftBankedUp25ToFlat */
        { 4,
          { kDiagLeftBankedUp25ToFlatSeq0, kDiagLeftBankedUp25ToFlatSeq1, kDiagLeftBankedUp25ToFlatSeq2,
            kDiagLeftBankedUp25ToFlatSeq3 } },
        /* TrackElemType::diagRightBankedUp25ToFlat */
        { 4,
          { kDiagRightBankedUp25ToFlatSeq0, kDiagRightBankedUp25ToFlatSeq1, kDiagRightBankedUp25ToFlatSeq2,
            kDiagRightBankedUp25ToFlatSeq3 } },
        /* TrackElemType::diagFlatToLeftBankedDown25 */
        { 4,
          { kDiagFlatToLeftBankedDown25Seq0, kDiagFlatToLeftBankedDown25Seq1, kDiagFlatToLeftBankedDown25Seq2,
            kDiagFlatToLeftBankedDown25Seq3 } },
        /* TrackElemType::diagFlatToRightBankedDown25 */
        { 4,
          { kDiagFlatToRightBankedDown25Seq0, kDiagFlatToRightBankedDown25Seq1, kDiagFlatToRightBankedDown25Seq2,
            kDiagFlatToRightBankedDown25Seq3 } },
        /* TrackElemType::diagLeftBankedDown25ToFlat */
        { 4,
          { kDiagLeftBankedDown25ToFlatSeq0, kDiagLeftBankedDown25ToFlatSeq1, kDiagLeftBankedDown25ToFlatSeq2,
            kDiagLeftBankedDown25ToFlatSeq3 } },
        /* TrackElemType::diagRightBankedDown25ToFlat */
        { 4,
          { kDiagRightBankedDown25ToFlatSeq0, kDiagRightBankedDown25ToFlatSeq1, kDiagRightBankedDown25ToFlatSeq2,
            kDiagRightBankedDown25ToFlatSeq3 } },
        /* TrackElemType::diagUp25LeftBanked */
        { 4, { kDiagUp25LeftBankedSeq0, kDiagUp25LeftBankedSeq1, kDiagUp25LeftBankedSeq2, kDiagUp25LeftBankedSeq3 } },
        /* TrackElemType::diagUp25RightBanked */
        { 4, { kDiagUp25RightBankedSeq0, kDiagUp25RightBankedSeq1, kDiagUp25RightBankedSeq2, kDiagUp25RightBankedSeq3 } },
        /* TrackElemType::diagDown25LeftBanked */
        { 4, { kDiagDown25LeftBankedSeq0, kDiagDown25LeftBankedSeq1, kDiagDown25LeftBankedSeq2, kDiagDown25LeftBankedSeq3 } },
        /* TrackElemType::diagDown25RightBanked */
        { 4,
          { kDiagDown25RightBankedSeq0, kDiagDown25RightBankedSeq1, kDiagDown25RightBankedSeq2, kDiagDown25RightBankedSeq3 } },
        /* TrackElemType::leftEighthBankToDiagUp25 */
        { 5,
          { kLeftEighthBankToDiagUp25Seq0, kLeftEighthBankToDiagUp25Seq1, kLeftEighthBankToDiagUp25Seq2,
            kLeftEighthBankToDiagUp25Seq3, kLeftEighthBankToDiagUp25Seq4 } },
        /* TrackElemType::rightEighthBankToDiagUp25 */
        { 5,
          { kRightEighthBankToDiagUp25Seq0, kRightEighthBankToDiagUp25Seq1, kRightEighthBankToDiagUp25Seq2,
            kRightEighthBankToDiagUp25Seq3, kRightEighthBankToDiagUp25Seq4 } },
        /* TrackElemType::leftEighthBankToDiagDown25 */
        { 5,
          { kLeftEighthBankToDiagDown25Seq0, kLeftEighthBankToDiagDown25Seq1, kLeftEighthBankToDiagDown25Seq2,
            kLeftEighthBankToDiagDown25Seq3, kLeftEighthBankToDiagDown25Seq4 } },
        /* TrackElemType::rightEighthBankToDiagDown25 */
        { 5,
          { kRightEighthBankToDiagDown25Seq0, kRightEighthBankToDiagDown25Seq1, kRightEighthBankToDiagDown25Seq2,
            kRightEighthBankToDiagDown25Seq3, kRightEighthBankToDiagDown25Seq4 } },
        /* TrackElemType::leftEighthBankToOrthogonalUp25 */
        { 5,
          { kLeftEighthBankToOrthogonalUp25Seq0, kLeftEighthBankToOrthogonalUp25Seq1, kLeftEighthBankToOrthogonalUp25Seq2,
            kLeftEighthBankToOrthogonalUp25Seq3, kLeftEighthBankToOrthogonalUp25Seq4 } },
        /* TrackElemType::rightEighthBankToOrthogonalUp25 */
        { 5,
          { kRightEighthBankToOrthogonalUp25Seq0, kRightEighthBankToOrthogonalUp25Seq1, kRightEighthBankToOrthogonalUp25Seq2,
            kRightEighthBankToOrthogonalUp25Seq3, kRightEighthBankToOrthogonalUp25Seq4 } },
        /* TrackElemType::leftEighthBankToOrthogonalDown25 */
        { 5,
          { kLeftEighthBankToOrthogonalDown25Seq0, kLeftEighthBankToOrthogonalDown25Seq1, kLeftEighthBankToOrthogonalDown25Seq2,
            kLeftEighthBankToOrthogonalDown25Seq3, kLeftEighthBankToOrthogonalDown25Seq4 } },
        /* TrackElemType::rightEighthBankToOrthogonalDown25 */
        { 5,
          { kRightEighthBankToOrthogonalDown25Seq0, kRightEighthBankToOrthogonalDown25Seq1,
            kRightEighthBankToOrthogonalDown25Seq2, kRightEighthBankToOrthogonalDown25Seq3,
            kRightEighthBankToOrthogonalDown25Seq4 } },
        /* TrackElemType::diagBrakes */ { 4, { kDiagBrakesSeq0, kDiagBrakesSeq1, kDiagBrakesSeq2, kDiagBrakesSeq3 } },
        /* TrackElemType::diagBlockBrakes */
        { 4, { kDiagBlockBrakesSeq0, kDiagBlockBrakesSeq1, kDiagBlockBrakesSeq2, kDiagBlockBrakesSeq3 } },
        /* TrackElemType::inclinedBrakes */
        { 1, { kDown25Seq0 } },
        /* TrackElemType::diagBooster */ { 4, { kDiagFlatSeq0, kDiagFlatSeq1, kDiagFlatSeq2, kDiagFlatSeq3 } },
        /* TrackElemType::diagFlatToUp60LongBase */
        { 10,
          { kDiagFlatToUp60LongBaseSeq0, kDiagFlatToUp60LongBaseSeq1, kDiagFlatToUp60LongBaseSeq2, kDiagFlatToUp60LongBaseSeq3,
            kDiagFlatToUp60LongBaseSeq4, kDiagFlatToUp60LongBaseSeq5, kDiagFlatToUp60LongBaseSeq6, kDiagFlatToUp60LongBaseSeq7,
            kDiagFlatToUp60LongBaseSeq8, kDiagFlatToUp60LongBaseSeq9 } },
        /* TrackElemType::diagUp60ToFlatLongBase */
        { 10,
          { kDiagUp60ToFlatLongBaseSeq0, kDiagUp60ToFlatLongBaseSeq1, kDiagUp60ToFlatLongBaseSeq2, kDiagUp60ToFlatLongBaseSeq3,
            kDiagUp60ToFlatLongBaseSeq4, kDiagUp60ToFlatLongBaseSeq5, kDiagUp60ToFlatLongBaseSeq6, kDiagUp60ToFlatLongBaseSeq7,
            kDiagUp60ToFlatLongBaseSeq8, kDiagUp60ToFlatLongBaseSeq9 } },
        /* TrackElemType::diagFlatToDown60LongBase */
        { 10,
          { kDiagFlatToDown60LongBaseSeq0, kDiagFlatToDown60LongBaseSeq1, kDiagFlatToDown60LongBaseSeq2,
            kDiagFlatToDown60LongBaseSeq3, kDiagFlatToDown60LongBaseSeq4, kDiagFlatToDown60LongBaseSeq5,
            kDiagFlatToDown60LongBaseSeq6, kDiagFlatToDown60LongBaseSeq7, kDiagFlatToDown60LongBaseSeq8,
            kDiagFlatToDown60LongBaseSeq9 } },
        /* TrackElemType::diagDown60ToFlatLongBase */
        { 10,
          { kDiagDown60ToFlatLongBaseSeq0, kDiagDown60ToFlatLongBaseSeq1, kDiagDown60ToFlatLongBaseSeq2,
            kDiagDown60ToFlatLongBaseSeq3, kDiagDown60ToFlatLongBaseSeq4, kDiagDown60ToFlatLongBaseSeq5,
            kDiagDown60ToFlatLongBaseSeq6, kDiagDown60ToFlatLongBaseSeq7, kDiagDown60ToFlatLongBaseSeq8,
            kDiagDown60ToFlatLongBaseSeq9 } },
        /* TrackElemType::leftEighthDiveLoopUpToOrthogonal */
        { 6,
          { kLeftEighthDiveLoopUpToOrthogonalSeq0, kLeftEighthDiveLoopUpToOrthogonalSeq1, kLeftEighthDiveLoopUpToOrthogonalSeq2,
            kLeftEighthDiveLoopUpToOrthogonalSeq3, kLeftEighthDiveLoopUpToOrthogonalSeq4,
            kLeftEighthDiveLoopUpToOrthogonalSeq5 } },
        /* TrackElemType::rightEighthDiveLoopUpToOrthogonal */
        { 6,
          { kRightEighthDiveLoopUpToOrthogonalSeq0, kRightEighthDiveLoopUpToOrthogonalSeq1,
            kRightEighthDiveLoopUpToOrthogonalSeq2, kRightEighthDiveLoopUpToOrthogonalSeq3,
            kRightEighthDiveLoopUpToOrthogonalSeq4, kRightEighthDiveLoopUpToOrthogonalSeq5 } },
        /* TrackElemType::leftEighthDiveLoopDownToDiag */
        { 6,
          { kLeftEighthDiveLoopDownToDiagSeq0, kLeftEighthDiveLoopDownToDiagSeq1, kLeftEighthDiveLoopDownToDiagSeq2,
            kLeftEighthDiveLoopDownToDiagSeq3, kLeftEighthDiveLoopDownToDiagSeq4, kLeftEighthDiveLoopDownToDiagSeq5 } },
        /* TrackElemType::rightEighthDiveLoopDownToDiag */
        { 6,
          { kRightEighthDiveLoopDownToDiagSeq0, kRightEighthDiveLoopDownToDiagSeq1, kRightEighthDiveLoopDownToDiagSeq2,
            kRightEighthDiveLoopDownToDiagSeq3, kRightEighthDiveLoopDownToDiagSeq4, kRightEighthDiveLoopDownToDiagSeq5 } },
        /* TrackElemType::diagDown25Brakes */ { 4, { kDiagDown25Seq0, kDiagDown25Seq1, kDiagDown25Seq2, kDiagDown25Seq3 } }
    };

#pragma endregion

    static constexpr auto BuildDescriptorTable()
    {
        std::array<TrackElementDescriptor, EnumValue(TrackElemType::count)> res{};

        for (int i = 0; i < EnumValue(TrackElemType::count); i++)
        {
            TrackElementDescriptor& desc = res[i];
            desc.description = kRideConfigurationStringIds[i];
            desc.alternativeType = kAlternativeTrackTypes[i];
            desc.coordinates = kTrackCoordinates[i];
            desc.curveChain = kTrackCurveChain[i];
            desc.flags = kTrackFlags[i];
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

    const TrackElementDescriptor& GetTrackElementDescriptor(TrackElemType type)
    {
        if (EnumValue(type) >= kTrackElementDescriptors.size())
            return kTrackElementDescriptors[0];
        return kTrackElementDescriptors[EnumValue(type)];
    }

} // namespace OpenRCT2::TrackMetaData
