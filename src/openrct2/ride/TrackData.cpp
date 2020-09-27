/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TrackData.h"

#include "Track.h"
#include "TrackPaint.h"

#include <iterator>

// clang-format off
const rct_track_coordinates FlatTrackCoordinates[TRACK_ELEM_COUNT] = {
    {    0,    0,    0,    0,    0,    0 },
    {    0,    0,    0,    0,    0,    0 },
    {    0,    0,    0,    0,    0,    0 },
    {    0,    0,    0,    0,    0,    0 },
    {    0,    0,    0,   16,    0,    0 },
    {    0,    0,    0,   64,    0,    0 },
    {    0,    0,    0,    8,    0,    0 },
    {    0,    0,    0,   32,    0,    0 },
    {    0,    0,    0,   32,    0,    0 },
    {    0,    0,    0,    8,    0,    0 },
    {    0,    0,   16,    0,    0,    0 },
    {    0,    0,   64,    0,    0,    0 },
    {    0,    0,    8,    0,    0,    0 },
    {    0,    0,   32,    0,    0,    0 },
    {    0,    0,   32,    0,    0,    0 },
    {    0,    0,    8,    0,    0,    0 },
    {    0,    3,    0,    0,  -64,  -64 },
    {    0,    1,    0,    0,  -64,   64 },
    {    0,    0,    0,    0,    0,    0 },
    {    0,    0,    0,    0,    0,    0 },
    {    0,    0,    0,    0,    0,    0 },
    {    0,    0,    0,    0,    0,    0 },
    {    0,    3,    0,    0,  -64,  -64 },
    {    0,    1,    0,    0,  -64,   64 },
    {    0,    0,    0,    8,    0,    0 },
    {    0,    0,    0,    8,    0,    0 },
    {    0,    0,    0,    8,    0,    0 },
    {    0,    0,    0,    8,    0,    0 },
    {    0,    0,    8,    0,    0,    0 },
    {    0,    0,    8,    0,    0,    0 },
    {    0,    0,    8,    0,    0,    0 },
    {    0,    0,    8,    0,    0,    0 },
    {    0,    0,    0,    0,    0,    0 },
    {    0,    0,    0,    0,    0,    0 },
    {    0,    3,    0,   64,  -64,  -64 },
    {    0,    1,    0,   64,  -64,   64 },
    {    0,    3,   64,    0,  -64,  -64 },
    {    0,    1,   64,    0,  -64,   64 },
    {    0,    0,    0,    0,  -64,  -32 },
    {    0,    0,    0,    0,  -64,   32 },
    {    0,    0,    0,    0,  -32,  -32 },
    {    0,    0,    0,    0,  -32,   32 },
    {    0,    3,    0,    0,  -32,  -32 },
    {    0,    1,    0,    0,  -32,   32 },
    {    0,    3,    0,    0,  -32,  -32 },
    {    0,    1,    0,    0,  -32,   32 },
    {    0,    3,    0,   32,  -32,  -32 },
    {    0,    1,    0,   32,  -32,   32 },
    {    0,    3,   32,    0,  -32,  -32 },
    {    0,    1,   32,    0,  -32,   32 },
    {    0,    3,    0,    0,    0,    0 },
    {    0,    1,    0,    0,    0,    0 },
    {    0,    0,    0,   16,  -64,    0 },
    {    0,    0,    0,   16,  -64,    0 },
    {    0,    0,    0,  -16,  -64,    0 },
    {    0,    0,    0,  -16,  -64,    0 },
    {    0,    2,    0,  152,  -32,    0 },
    {    0,    2,    0, -152,   32,    0 },
    {    0,    3,    0,   80,  -32,  -32 },
    {    0,    1,    0,   80,  -32,   32 },
    {    0,    3,    0,  -80,  -32,  -32 },
    {    0,    1,    0,  -80,  -32,   32 },
    {    0,    0,    0,   24,    0,    0 },
    {    0,    0,    0,   24,    0,    0 },
    {    0,    0,   24,    0,    0,    0 },
    {    0,    0,   24,    0,    0,    0 },
    {    0,    0,    0,   96,   32,    0 },
    {    0,    0,    0,   32,   32,    0 },
    {    0,    0,    0,    0,    0,    0 },
    {    0,    0,    0,   16,    0,    0 },
    {    0,    0,    0,   64,    0,    0 },
    {    0,    0,    0,    8,    0,    0 },
    {    0,    0,    0,   32,    0,    0 },
    {    0,    0,    0,   32,    0,    0 },
    {    0,    0,    0,    8,    0,    0 },
    {    0,    0,   16,    0,    0,    0 },
    {    0,    0,   64,    0,    0,    0 },
    {    0,    0,    8,    0,    0,    0 },
    {    0,    0,   32,    0,    0,    0 },
    {    0,    0,   32,    0,    0,    0 },
    {    0,    0,    8,    0,    0,    0 },
    {    0,    3,    0,    0,  -64,  -64 },
    {    0,    1,    0,    0,  -64,   64 },
    {    0,    0,    0,    0,  -64,  -32 },
    {    0,    0,    0,    0,  -64,   32 },
    {    0,    3,    0,    0,  -32,  -32 },
    {    0,    1,    0,    0,  -32,   32 },
    {    0,    2,    0,   16,    0,  -96 },
    {    0,    2,    0,   16,    0,   96 },
    {    0,    2,   16,    0,    0,  -96 },
    {    0,    2,   16,    0,    0,   96 },
    {    0,    2,    0,   16,    0, -160 },
    {    0,    2,    0,   16,    0,  160 },
    {    0,    2,   16,    0,    0, -160 },
    {    0,    2,   16,    0,    0,  160 },
    {    0,    2,    0,    0,    0,   32 }, // FLAT_TRACK_ELEM_1_X_4_A
    {    0,    1,    0,   64,    0,    0 },
    {    0,    3,   64,    0,    0,    0 },
    {    0,    1,   64,    0,    0,    0 },
    {    0,    0,    0,    0,    0,    0 },
    {    0,    0,    0,    0,    0,    0 },
    {    0,    0,    0,    0,    0,    0 },
    {    0,    3,    0,   16,  -64,  -64 },
    {    0,    1,    0,   16,  -64,   64 },
    {    0,    3,   16,    0,  -64,  -64 },
    {    0,    1,   16,    0,  -64,   64 },
    {    0,    3,    0,   16,  -64,  -64 },
    {    0,    1,    0,   16,  -64,   64 },
    {    0,    3,   16,    0,  -64,  -64 },
    {    0,    1,   16,    0,  -64,   64 },
    {    0,    2,    0,    0,    0,   32 }, // FLAT_TRACK_ELEM_2_X_2
    {    0,    2,    0,    0,    0,   32 }, // FLAT_TRACK_ELEM_4_X_4
    {    0,    0,    0,    0,    0,    0 },
    {    0,    0,    0,    0,    0,    0 },
    {    0,    0,    0,    0,    0,    0 },
    {    0,    2,    0,    0,    0,   32 }, // ???
    {    0,    2,    0,    0,    0,   32 }, // FLAT_TRACK_ELEM_1_X_5
    {    0,    0,   16,   16, -128,    0 },
    {    0,    2,    0,    0,    0,   32 }, // FLAT_TRACK_ELEM_1_X_1_A
    {    0,    2,    0,    0,    0,   32 }, // FLAT_TRACK_ELEM_1_X_4_B
    {    0,    0,    0,    0,    0,    0 },
    {    0,    2,    0,    0,    0,   32 }, // FLAT_TRACK_ELEM_1_X_1_B
    {    0,    2,    0,    0,    0,   32 }, // FLAT_TRACK_ELEM_1_X_4_C
    {    0,    0,    0,   96,   32,    0 }, // FLAT_TRACK_ELEM_3_X_3
    {    0,    0,    0,  240, -160,    0 },
    {    0,    0,    0,   80,   32,    0 },
    {    0,    0,    0,   32,   32,    0 },
    {    0,    0,   32,    0,   32,    0 },
    {    0,    0,    0,   56,   32,    0 },
    {    0,    0,   56,    0,    0,    0 },
    {    0,    0,    0,   56,    0,    0 },
    {    0,    0,   56,    0,   32,    0 },
    {    0,    0,   24,    0,    0,    0 },
    {    0,    7,    0,    0,  -64,  -32 },
    {    0,    4,    0,    0,  -64,   32 },
    {    4,    0,    0,    0,  -64,   32 },
    {    4,    1,    0,    0,  -32,   64 },
    {    0,    7,    0,    0,  -64,  -32 },
    {    0,    4,    0,    0,  -64,   32 },
    {    4,    0,    0,    0,  -64,   32 },
    {    4,    1,    0,    0,  -32,   64 },
    {    4,    4,    0,    0,  -32,   32 },
    {    4,    4,    0,   16,  -32,   32 },
    {    4,    4,    0,   64,  -32,   32 },
    {    4,    4,    0,    8,  -32,   32 },
    {    4,    4,    0,   32,  -32,   32 },
    {    4,    4,    0,   32,  -32,   32 },
    {    4,    4,    0,    8,  -32,   32 },
    {    4,    4,   16,    0,  -32,   32 },
    {    4,    4,   64,    0,  -32,   32 },
    {    4,    4,    8,    0,  -32,   32 },
    {    4,    4,   32,    0,  -32,   32 },
    {    4,    4,   32,    0,  -32,   32 },
    {    4,    4,    8,    0,  -32,   32 },
    {    4,    4,    0,   24,  -32,   32 },
    {    4,    4,    0,   24,  -32,   32 },
    {    4,    4,   24,    0,  -32,   32 },
    {    4,    4,   24,    0,  -32,   32 },
    {    4,    4,    0,    0,  -32,   32 },
    {    4,    4,    0,    0,  -32,   32 },
    {    4,    4,    0,    0,  -32,   32 },
    {    4,    4,    0,    0,  -32,   32 },
    {    4,    4,    0,    8,  -32,   32 },
    {    4,    4,    0,    8,  -32,   32 },
    {    4,    4,    0,    8,  -32,   32 },
    {    4,    4,    0,    8,  -32,   32 },
    {    4,    4,    8,    0,  -32,   32 },
    {    4,    4,    8,    0,  -32,   32 },
    {    4,    4,    8,    0,  -32,   32 },
    {    4,    4,    8,    0,  -32,   32 },
    {    4,    4,    0,    0,  -32,   32 },
    {    4,    4,    0,    0,  -32,   32 },
    {    0,    0,    0,    0,    0,    0 },
    {    0,    0,    0,    0,    0,    0 },
    {    0,    0,    0,   32,  -64,    0 },
    {    0,    0,    0,   32,  -64,    0 },
    {    0,    0,    0,  -32,  -64,    0 },
    {    0,    0,    0,  -32,  -64,    0 },
    {    0,    3,    0,   24,  -32,  -32 },
    {    0,    1,    0,   24,  -32,   32 },
    {    0,    3,   24,    0,  -32,  -32 },
    {    0,    1,   24,    0,  -32,   32 },
    {    0,    0,    0,   16,    0,    0 },
    {    0,    2,    0,  280,  -64,  -32 },
    {    0,    2,    0,  280,  -64,   32 },
    {    0,    2,    0, -280,   64,  -32 },
    {    0,    2,    0, -280,   64,   32 },
    {    0,    0,    0,  -16,  -64,    0 },
    {    0,    0,    0,  -16,  -64,    0 },
    {    0,    0,    0,   16,  -64,    0 },
    {    0,    0,    0,   16,  -64,    0 },
    {    0,    2,    0,  120,  -32,    0 },
    {    0,    2,    0, -120,   32,    0 },
    {    0,    3,    0,   48,  -32,  -32 },
    {    0,    1,    0,   48,  -32,   32 },
    {    0,    3,    0,  -48,  -32,  -32 },
    {    0,    1,    0,  -48,  -32,   32 },
    {    0,    2,    0,   32,    0,    0 },
    {    0,    2,    0,  -32,    0,    0 },
    {    0,    0,    0,    0, -160,    0 },
    {    0,    0,    0,    0, -160,    0 },
    {    0,    0,    0,    0,  -32,    0 },
    {    0,    0,    0,    0,  -32,    0 },
    {    0,    0,    0,    0,  -32,    0 },
    {    0,    1,    0,    0,  -32,   32 },
    {    0,    3,    0,    0,  -32,  -32 },
    {    0,    0,    0,    0,  -32,    0 }, // ???
    {    0,    0,    0,    0,  -32,    0 }, // ???
    {    0,    0,    0,    0,  -32,    0 }, // ???
    {    0,    3,    0,   16,  -32,  -32 },
    {    0,    1,    0,   16,  -32,   32 },
    {    0,    0,    0,    0,  -64,    0 },
    {    0,    0,    0,    0,  -64,    0 },
    {    0,    0,    0,    0,  -32,    0 },
    {    0,    0,   80,    0,   32,    0 },
    {    0,    0,  240,    0, -160,    0 },
    {    0,    0,    0,    0,    0,    0 },
    {    0,    3,    0,   32,  -32,  -32 },
    {    0,    1,    0,   32,  -32,   32 },
    {    0,    3,   32,    0,  -32,  -32 },
    {    0,    1,   32,    0,  -32,   32 },
    {    0,    3,    0,   64,  -64,  -64 },
    {    0,    1,    0,   64,  -64,   64 },
    {    0,    3,   64,    0,  -64,  -64 },
    {    0,    1,   64,    0,  -64,   64 },
    {    0,    0,    0,   16,    0,    0 },
    {    0,    0,    0,   16,    0,    0 },
    {    0,    0,    0,   16,    0,    0 },
    {    0,    0,    0,   16,    0,    0 },
    {    0,    0,   16,    0,    0,    0 },
    {    0,    0,   16,    0,    0,    0 },
    {    0,    0,   16,    0,    0,    0 },
    {    0,    0,   16,    0,    0,    0 },
    {    0,    0,    0,    8,    0,    0 },
    {    0,    0,    0,    8,    0,    0 },
    {    0,    0,    0,    8,    0,    0 },
    {    0,    0,    0,    8,    0,    0 },
    {    0,    0,    8,    0,    0,    0 },
    {    0,    0,    8,    0,    0,    0 },
    {    0,    0,    8,    0,    0,    0 },
    {    0,    0,    8,    0,    0,    0 },
    {    0,    0,    0,    8,    0,    0 },
    {    0,    0,    0,    8,    0,    0 },
    {    0,    0,    0,    8,    0,    0 },
    {    0,    0,    0,    8,    0,    0 },
    {    0,    0,    8,    0,    0,    0 },
    {    0,    0,    8,    0,    0,    0 },
    {    0,    0,    8,    0,    0,    0 },
    {    0,    0,    8,    0,    0,    0 },
    {    0,    3,    0,   96,    0,   32 },
    {    0,    1,    0,   96,    0,  -32 },
    {    0,    3,   96,    0,    0,   32 },
    {    0,    1,   96,    0,    0,  -32 },
};

const rct_track_coordinates TrackCoordinates[TRACK_ELEM_COUNT] = {
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
        { 0, 2, 0, 128, 64, 0 }
};

/** rct2: 0x0099BA64 */
const uint8_t TrackSequenceProperties[][16] = {
    { 0 },
    /* TRACK_ELEM_END_STATION */    { TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN | TRACK_SEQUENCE_FLAG_DISALLOW_DOORS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /* TRACK_ELEM_BEGIN_STATION */  { TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN | TRACK_SEQUENCE_FLAG_DISALLOW_DOORS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /* TRACK_ELEM_MIDDLE_STATION */ { TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN | TRACK_SEQUENCE_FLAG_DISALLOW_DOORS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
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
    /* TRACK_ELEM_TOWER_BASE */ { TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_3, TRACK_SEQUENCE_FLAG_DIRECTION_0, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1, TRACK_SEQUENCE_FLAG_DIRECTION_3, TRACK_SEQUENCE_FLAG_DIRECTION_1, TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3, TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2, TRACK_SEQUENCE_FLAG_DIRECTION_2, 0, 0, 0, 0, 0, 0, 0 },
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
    /* TRACK_ELEM_MAZE */   { TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN },
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
    /* TRACK_ELEM_SPINNING_TUNNEL */    { TRACK_SEQUENCE_FLAG_DISALLOW_DOORS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
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
};

/** rct2: 0x0099CA64 */
const uint8_t FlatRideTrackSequenceProperties[][16] = {
    { 0 },
    /* 1 */ { TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /* 2 */ { TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /* 3 */ { TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
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
    /* 66 */    { TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_3, TRACK_SEQUENCE_FLAG_DIRECTION_0, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1, TRACK_SEQUENCE_FLAG_DIRECTION_3, TRACK_SEQUENCE_FLAG_DIRECTION_1, TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3, TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2, TRACK_SEQUENCE_FLAG_DIRECTION_2, 0, 0, 0, 0, 0, 0, 0 },
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
    /* FLAT_TRACK_ELEM_1_X_4_A */   { TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, 0, TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    /* 101 */   { TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    /* FLAT_TRACK_ELEM_2_X_2 */ { TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1, TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3, TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /* FLAT_TRACK_ELEM_4_X_4 */ { TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0, TRACK_SEQUENCE_FLAG_DIRECTION_0, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1, TRACK_SEQUENCE_FLAG_DIRECTION_3, 0, 0, TRACK_SEQUENCE_FLAG_DIRECTION_1, TRACK_SEQUENCE_FLAG_DIRECTION_3, 0, 0, TRACK_SEQUENCE_FLAG_DIRECTION_1, TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3, TRACK_SEQUENCE_FLAG_DIRECTION_2, TRACK_SEQUENCE_FLAG_DIRECTION_2, TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 },
    { 0 },
    { 0 },
    { 0 },
    /* 115 */   { TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0, TRACK_SEQUENCE_FLAG_DIRECTION_0, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1, TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3, TRACK_SEQUENCE_FLAG_DIRECTION_2, TRACK_SEQUENCE_FLAG_DIRECTION_2, TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2, 0, 0, 0, 0, 0, 0, 0, 0 },
    /* FLAT_TRACK_ELEM_1_X_5 */ { TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, 0, TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3, TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0 },
    /* FLAT_TRACK_ELEM_1_X_1_A */   { TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_ORIGIN | TRACK_SEQUENCE_FLAG_CONNECTS_TO_PATH, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /* FLAT_TRACK_ELEM_1_X_4_B */   { TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_ORIGIN, 0, TRACK_SEQUENCE_FLAG_DIRECTION_1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0 },
    /* FLAT_TRACK_ELEM_1_X_1_B */   { TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN | TRACK_SEQUENCE_FLAG_CONNECTS_TO_PATH, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /* FLAT_TRACK_ELEM_1_X_4_C */   { TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3 | TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3, TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_3, TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /* FLAT_TRACK_ELEM_3_X_3 */ { TRACK_SEQUENCE_FLAG_ORIGIN, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_3, TRACK_SEQUENCE_FLAG_DIRECTION_0, TRACK_SEQUENCE_FLAG_DIRECTION_0 | TRACK_SEQUENCE_FLAG_DIRECTION_1, TRACK_SEQUENCE_FLAG_DIRECTION_3, TRACK_SEQUENCE_FLAG_DIRECTION_1, TRACK_SEQUENCE_FLAG_DIRECTION_2 | TRACK_SEQUENCE_FLAG_DIRECTION_3, TRACK_SEQUENCE_FLAG_DIRECTION_1 | TRACK_SEQUENCE_FLAG_DIRECTION_2, TRACK_SEQUENCE_FLAG_DIRECTION_2, 0, 0, 0, 0, 0, 0, 0 },
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
};

#define TRACK_BLOCK_END { 255, 255, 255, 255, 255, {255, 255}, 255 }

static constexpr const rct_preview_track TrackBlocks000[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks001[] = {
    { 0, 0, 0, 0, 0,{ 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks002[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks003[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks004[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks005[] = {
    { 0, 0, 0, 0, 64, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks006[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks007[] = {
    { 0, 0, 0, 0, 32, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks008[] = {
    { 0, 0, 0, 0, 32, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks009[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks010[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks011[] = {
    { 0, 0, 0, 0, 64, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks012[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks013[] = {
    { 0, 0, 0, 0, 32, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks014[] = {
    { 0, 0, 0, 0, 32, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks015[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks016[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0111, 0 }, 0 },
    { 3, -32, -32, 0, 0, { 0b1101, 0 }, 0 },
    { 4, -32, -64, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, -32, 0, 0, { 0b0111, 0 }, 0 },
    { 6, -64, -64, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks017[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b1011, 0 }, 0 },
    { 3, -32, 32, 0, 0, { 0b1110, 0 }, 0 },
    { 4, -32, 64, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, 32, 0, 0, { 0b1011, 0 }, 0 },
    { 6, -64, 64, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks018[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks019[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks020[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks021[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks022[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0111, 0 }, 0 },
    { 3, -32, -32, 0, 0, { 0b1101, 0 }, 0 },
    { 4, -32, -64, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, -32, 0, 0, { 0b0111, 0 }, 0 },
    { 6, -64, -64, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks023[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b1011, 0 }, 0 },
    { 3, -32, 32, 0, 0, { 0b1110, 0 }, 0 },
    { 4, -32, 64, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, 32, 0, 0, { 0b1011, 0 }, 0 },
    { 6, -64, 64, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks024[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks025[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks026[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks027[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks028[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks029[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks030[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks031[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks032[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks033[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks034[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
    { 1, 0, -32, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 16, 16, { 0b0111, 0b0100 }, 0 },
    { 3, -32, -32, 24, 16, { 0b1101, 0 }, 0 },
    { 4, -32, -64, 48, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, -32, 32, 16, { 0b0111, 0b0100 }, 0 },
    { 6, -64, -64, 48, 16, { 0b1111, 0b0110 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks035[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
    { 1, 0, 32, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 16, 16, { 0b1011, 0b1000 }, 0 },
    { 3, -32, 32, 24, 16, { 0b1110, 0 }, 0 },
    { 4, -32, 64, 48, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, 32, 32, 16, { 0b1011, 0b1000 }, 0 },
    { 6, -64, 64, 48, 16, { 0b1111, 0b1001 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks036[] = {
    { 0, 0, 0, 48, 16, { 0b1111, 0b0011 }, 0 },
    { 1, 0, -32, 48, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 32, 16, { 0b0111, 0b0001 }, 0 },
    { 3, -32, -32, 24, 16, { 0b1101, 0 }, 0 },
    { 4, -32, -64, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, -32, 16, 16, { 0b0111, 0b0001 }, 0 },
    { 6, -64, -64, 0, 16, { 0b1111, 0b1001 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks037[] = {
    { 0, 0, 0, 48, 16, { 0b1111, 0b0011 }, 0 },
    { 1, 0, 32, 48, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 32, 16, { 0b1011, 0b0010 }, 0 },
    { 3, -32, 32, 24, 16, { 0b1110, 0 }, 0 },
    { 4, -32, 64, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, 32, 16, 16, { 0b1011, 0b0010 }, 0 },
    { 6, -64, 64, 0, 16, { 0b1111, 0b0110 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks038[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 0, { 0b0111, 0 }, 0 },
    { 2, -32, -32, 0, 0, { 0b1101, 0 }, 0 },
    { 3, -64, -32, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks039[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 0, { 0b1011, 0 }, 0 },
    { 2, -32, 32, 0, 0, { 0b1110, 0 }, 0 },
    { 3, -64, 32, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks040[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
    { 1, -32, 0, 16, 16, { 0b1111, 0b1100 }, 0 },
    { 2, -64, 0, 32, 96, { 0b0010, 0 }, 0 },
    { 3, -32, 0, 120, 16, { 0b0110, 0 }, 0 },
    { 4, -32, -32, 120, 0, { 0b0001, 0 }, 0 },
    { 5, 0, 0, 120, 0, { 0b0100, 0 }, 0 },
    { 6, 0, -32, 120, 16, { 0b1001, 0 }, 0 },
    { 7, 32, -32, 32, 96, { 0b1000, 0 }, 0 },
    { 8, 0, -32, 16, 16, { 0b1111, 0b0011 }, 0 },
    { 9, -32, -32, 0, 16, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks041[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
    { 1, -32, 0, 16, 16, { 0b1111, 0b1100 }, 0 },
    { 2, -64, 0, 32, 96, { 0b0001, 0 }, 0 },
    { 3, -32, 0, 120, 16, { 0b1001, 0 }, 0 },
    { 4, -32, 32, 120, 0, { 0b0010, 0 }, 0 },
    { 5, 0, 0, 120, 0, { 0b1000, 0 }, 0 },
    { 6, 0, 32, 120, 16, { 0b0110, 0 }, 0 },
    { 7, 32, 32, 32, 96, { 0b0100, 0 }, 0 },
    { 8, 0, 32, 16, 16, { 0b1111, 0b0011 }, 0 },
    { 9, -32, 32, 0, 16, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks042[] = {
    { 0, 0, 0, 0, 0, { 0b0111, 0 }, 0 },
    { 1, 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0010, 0 }, 0 },
    { 3, -32, -32, 0, 0, { 0b0111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks043[] = {
    { 0, 0, 0, 0, 0, { 0b1011, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0001, 0 }, 0 },
    { 3, -32, 32, 0, 0, { 0b1011, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks044[] = {
    { 0, 0, 0, 0, 0, { 0b0111, 0 }, 0 },
    { 1, 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0010, 0 }, 0 },
    { 3, -32, -32, 0, 0, { 0b0111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks045[] = {
    { 0, 0, 0, 0, 0, { 0b1011, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0001, 0 }, 0 },
    { 3, -32, 32, 0, 0, { 0b1011, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks046[] = {
    { 0, 0, 0, 0, 16, { 0b0111, 0b0100 }, 0 },
    { 1, 0, -32, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 16, 0, { 0b0010, 0 }, 0 },
    { 3, -32, -32, 16, 16,{ 0b0111, 0b0110 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks047[] = {
    { 0, 0, 0, 0, 16, { 0b1011, 0b1000 }, 0 },
    { 1, 0, 32, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 16, 0, { 0b0001, 0 }, 0 },
    { 3, -32, 32, 16, 16,{ 0b1011, 0b1001 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks048[] = {
    { 0, 0, 0, 16, 16,{ 0b0111, 0b0011 }, 0 },
    { 1, 0, -32, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 16, 0, { 0b0010, 0 }, 0 },
    { 3, -32, -32, 0, 16, { 0b0111, 0b0001 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks049[] = {
    { 0, 0, 0, 16, 16, { 0b1011, 0b0011 }, 0 },
    { 1, 0, 32, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 16, 0, { 0b0001, 0 }, 0 },
    { 3, -32, 32, 0, 16, { 0b1011, 0b0010 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks050[] = {
    { 0, 0, 0, 0, 0, { 0b0111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks051[] = {
    { 0, 0, 0, 0, 0, { 0b1011, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks052[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 2, -64, 0, 16, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks053[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 2, -64, 0, 16, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks054[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -16, 16, { 0b1111, 0 }, 0 },
    { 2, -64, 0, -16, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks055[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -16, 16, { 0b1111, 0 }, 0 },
    { 2, -64, 0, -16, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks056[] = {
    { 0, 0, 0, 0, 32, { 0b1111, 0b1100 }, 0 },
    { 1, -32, 0, 16, 16, { 0b1111, 0b1100 }, 0 },
    { 2, -64, 0, 32, 96,{ 0b0011, 0 }, 0 },
    { 3, -32, 0, 120, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks057[] = {
    { 0, 0, 0, -32, 32, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -120, 96,{ 0b0011, 0 }, 0 },
    { 2, 0, 0, -136, 16, { 0b1111, 0b1100 }, 0 },
    { 3, 32, 0, -152, 16, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks058[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 24, 32, { 0b0111, 0 }, 0 },
    { 2, -32, -32, 48, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks059[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 24, 32, { 0b1011, 0 }, 0 },
    { 2, -32, 32, 48, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks060[] = {
    { 0, 0, 0, -32, 16, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -56, 32, { 0b0111, 0 }, 0 },
    { 2, -32, -32, -80, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks061[] = {
    { 0, 0, 0, -32, 16, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -56, 32, { 0b1011, 0 }, 0 },
    { 2, -32, 32, -80, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks062[] = {
    { 0, 0, 0, 0, 24, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks063[] = {
    { 0, 0, 0, 0, 24, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks064[] = {
    { 0, 0, 0, 0, 24, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks065[] = {
    { 0, 0, 0, 0, 24, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks066[] = {
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

static constexpr const rct_preview_track TrackBlocks067[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, 32, 0, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_1 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks068[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks069[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks070[] = {
    { 0, 0, 0, 0, 64, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks071[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks072[] = {
    { 0, 0, 0, 0, 32, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks073[] = {
    { 0, 0, 0, 0, 32, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks074[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks075[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks076[] = {
    { 0, 0, 0, 0, 64, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks077[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks078[] = {
    { 0, 0, 0, 0, 32, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks079[] = {
    { 0, 0, 0, 0, 32, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks080[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks081[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0111, 0 }, 0 },
    { 3, -32, -32, 0, 0, { 0b1101, 0 }, 0 },
    { 4, -32, -64, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, -32, 0, 0, { 0b0111, 0 }, 0 },
    { 6, -64, -64, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks082[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b1011, 0 }, 0 },
    { 3, -32, 32, 0, 0, { 0b1110, 0 }, 0 },
    { 4, -32, 64, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, 32, 0, 0, { 0b1011, 0 }, 0 },
    { 6, -64, 64, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks083[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 0, { 0b0111, 0 }, 0 },
    { 2, -32, -32, 0, 0, { 0b1101, 0 }, 0 },
    { 3, -64, -32, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks084[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 0, { 0b1011, 0 }, 0 },
    { 2, -32, 32, 0, 0, { 0b1110, 0 }, 0 },
    { 3, -64, 32, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks085[] = {
    { 0, 0, 0, 0, 0, { 0b0111, 0 }, 0 },
    { 1, 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0010, 0 }, 0 },
    { 3, -32, -32, 0, 0, { 0b0111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks086[] = {
    { 0, 0, 0, 0, 0, { 0b1011, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0001, 0 }, 0 },
    { 3, -32, 32, 0, 0, { 0b1011, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks087[] = {
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

static constexpr const rct_preview_track TrackBlocks088[] = {
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

static constexpr const rct_preview_track TrackBlocks089[] = {
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

static constexpr const rct_preview_track TrackBlocks090[] = {
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

static constexpr const rct_preview_track TrackBlocks091[] = {
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

static constexpr const rct_preview_track TrackBlocks092[] = {
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

static constexpr const rct_preview_track TrackBlocks093[] = {
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

static constexpr const rct_preview_track TrackBlocks094[] = {
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

static constexpr const rct_preview_track TrackBlocks095[] = {
    { 0, 0, 0, 0, 64, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks096[] = {
    { 0, 0, 0, 0, 64, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks097[] = {
    { 0, 0, 0, 0, 64, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks098[] = {
    { 0, 0, 0, 0, 64, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks099[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks100[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks101[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks102[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0111, 0 }, 0 },
    { 3, -32, -32, 0, 0, { 0b1101, 0 }, 0 },
    { 4, -32, -64, 0, 12, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, -32, 0, 12, { 0b0111, 0 }, 0 },
    { 6, -64, -64, 0, 12, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks103[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b1011, 0 }, 0 },
    { 3, -32, 32, 0, 0, { 0b1110, 0 }, 0 },
    { 4, -32, 64, 0, 12, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, 32, 0, 12, { 0b1011, 0 }, 0 },
    { 6, -64, 64, 0, 12, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks104[] = {
    { 0, 0, 0, 0, 12, { 0b1111, 0 }, 0 },
    { 1, 0, -32, 0, 12, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 12, { 0b0111, 0 }, 0 },
    { 3, -32, -32, 0, 0, { 0b1101, 0 }, 0 },
    { 4, -32, -64, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, -32, 0, 0, { 0b0111, 0 }, 0 },
    { 6, -64, -64, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks105[] = {
    { 0, 0, 0, 0, 12, { 0b1111, 0 }, 0 },
    { 1, 0, 32, 0, 12, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 12, { 0b1011, 0 }, 0 },
    { 3, -32, 32, 0, 0, { 0b1110, 0 }, 0 },
    { 4, -32, 64, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, 32, 0, 0, { 0b1011, 0 }, 0 },
    { 6, -64, 64, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks106[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0111, 0 }, 0 },
    { 3, -32, -32, 0, 0, { 0b1101, 0 }, 0 },
    { 4, -32, -64, 0, 12, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, -32, 0, 12, { 0b0111, 0 }, 0 },
    { 6, -64, -64, 0, 12, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks107[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b1011, 0 }, 0 },
    { 3, -32, 32, 0, 0, { 0b1110, 0 }, 0 },
    { 4, -32, 64, 0, 12, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, 32, 0, 12, { 0b1011, 0 }, 0 },
    { 6, -64, 64, 0, 12, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks108[] = {
    { 0, 0, 0, 0, 12, { 0b1111, 0 }, 0 },
    { 1, 0, -32, 0, 12, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 12, { 0b0111, 0 }, 0 },
    { 3, -32, -32, 0, 0, { 0b1101, 0 }, 0 },
    { 4, -32, -64, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, -32, 0, 0, { 0b0111, 0 }, 0 },
    { 6, -64, -64, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks109[] = {
    { 0, 0, 0, 0, 12, { 0b1111, 0 }, 0 },
    { 1, 0, 32, 0, 12, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 12, { 0b1011, 0 }, 0 },
    { 3, -32, 32, 0, 0, { 0b1110, 0 }, 0 },
    { 4, -32, 64, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, 32, 0, 0, { 0b1011, 0 }, 0 },
    { 6, -64, 64, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks110[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks111[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks112[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks113[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks114[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks115[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks116[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks117[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 2, -64, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 3, -96, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 4, -128, 0, 0, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks118[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 2, -64, 0, 16, 24, { 0b1111, 0 }, 0 },
    { 3, -96, 0, 40, 48, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks119[] = {
    { 0, 0, 0, 0, 48, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 40, 48, { 0b1111, 0 }, 0 },
    { 2, -64, 0, 64, 24, { 0b1111, 0 }, 0 },
    { 3, -96, 0, 80, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks120[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks121[] = {
    { 0, 0, 0, 40, 48, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 16, 24, { 0b1111, 0 }, 0 },
    { 2, -64, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 3, -96, 0, 0, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks122[] = {
    { 0, 0, 0, 80, 8, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 64, 24, { 0b1111, 0 }, 0 },
    { 2, -64, 0, 40, 48, { 0b1111, 0 }, 0 },
    { 3, -96, 0, 0, 48, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks123[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
    { 1, -32, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
    { 2, -64, 0, -32, 32, { 0b1111, 0b0011 }, 0 },
    { 3, -96, 0, -96, 64, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks124[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 32, { 0b1111, 0 }, 0 },
    { 2, -64, 0, 0, 48, { 0b1111, 0 }, 0 },
    { 3, -96, 0, 0, 80, { 0b1111, 0 }, 0 },
    { 4, -128, 0, 0, 160, { 0b1111, 0 }, 0 },
    { 5, -192, 0, 0, 208, { 0b1111, 0 }, 0 },
    { 6, -160, 0, 0, 208, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks125[] = {
    { 0, 0, 0, 0, 48, { 0b1111, 0 }, 0 },
    { 1, 32, 0, 0, 48, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks126[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 1, 32, 0, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks127[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 1, 32, 0, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks128[] = {
    { 0, 0, 0, 0, 32, { 0b1111, 0b1100 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 1, 32, 0, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks129[] = {
    { 0, 0, 0, 0, 32, { 0b1111, 0b0011 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks130[] = {
    { 0, 0, 0, 0, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks131[] = {
    { 0, 0, 0, 0, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 1, 32, 0, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks132[] = {
    { 0, 0, 0, 0, 24, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks133[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 2, -32, -32, 0, 0, { 0b1000, 0 }, 0 },
    { 3, -64, 0, 0, 0, { 0b0010, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 4, -64, -32, 0, 0, { 0b0001, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks134[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 2, -32, 32, 0, 0, { 0b0100, 0 }, 0 },
    { 3, -64, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 4, -64, 32, 0, 0, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks135[] = {
    { 0, 0, 0, 0, 0, { 0b1101, 0 }, 0 },
    { 1, -32, 0, 0, 0, { 0b0001, 0 }, 0 },
    { 2, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 0, { 0b1111, 0 }, 0 },
    { 4, -64, 32, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks136[] = {
    { 0, 0, 0, 0, 0, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, 0 },
    { 2, -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 0, { 0b1111, 0 }, 0 },
    { 4, -32, 64, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks137[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 2, -32, -32, 0, 0, { 0b1000, 0 }, 0 },
    { 3, -64, 0, 0, 0, { 0b0010, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 4, -64, -32, 0, 0, { 0b0001, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks138[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 2, -32, 32, 0, 0, { 0b0100, 0 }, 0 },
    { 3, -64, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 4, -64, 32, 0, 0, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks139[] = {
    { 0, 0, 0, 0, 0, { 0b1101, 0 }, 0 },
    { 1, -32, 0, 0, 0, { 0b0001, 0 }, 0 },
    { 2, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 0, { 0b1111, 0 }, 0 },
    { 4, -64, 32, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks140[] = {
    { 0, 0, 0, 0, 0, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, 0 },
    { 2, -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 0, { 0b1111, 0 }, 0 },
    { 4, -32, 64, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks141[] = {
    { 0, 0, 0, 0, 0, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 0, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks142[] = {
    { 0, 0, 0, 0, 16, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 16, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks143[] = {
    { 0, 0, 0, 0, 64, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 64, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 64, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 64, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks144[] = {
    { 0, 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks145[] = {
    { 0, 0, 0, 0, 32, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 32, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 32, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 32, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks146[] = {
    { 0, 0, 0, 0, 32, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 32, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 32, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 32, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks147[] = {
    { 0, 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks148[] = {
    { 0, 0, 0, 0, 16, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 16, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks149[] = {
    { 0, 0, 0, 0, 64, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 64, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 64, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 64, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks150[] = {
    { 0, 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks151[] = {
    { 0, 0, 0, 0, 32, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 32, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 32, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 32, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks152[] = {
    { 0, 0, 0, 0, 32, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 32, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 32, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 32, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks153[] = {
    { 0, 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks154[] = {
    { 0, 0, 0, 0, 24, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 24, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 24, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 24, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks155[] = {
    { 0, 0, 0, 0, 24, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 24, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 24, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 24, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks156[] = {
    { 0, 0, 0, 0, 24, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 24, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 24, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 24, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks157[] = {
    { 0, 0, 0, 0, 24, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 24, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 24, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 24, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks158[] = {
    { 0, 0, 0, 0, 0, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 0, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks159[] = {
    { 0, 0, 0, 0, 0, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 0, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks160[] = {
    { 0, 0, 0, 0, 0, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 0, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks161[] = {
    { 0, 0, 0, 0, 0, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 0, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks162[] = {
    { 0, 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks163[] = {
    { 0, 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks164[] = {
    { 0, 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks165[] = {
    { 0, 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks166[] = {
    { 0, 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks167[] = {
    { 0, 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks168[] = {
    { 0, 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks169[] = {
    { 0, 0, 0, 0, 8, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 8, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks170[] = {
    { 0, 0, 0, 0, 0, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 0, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks171[] = {
    { 0, 0, 0, 0, 0, { 0b1101, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 3, -32, 32, 0, 0, { 0b0010, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks172[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks173[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks174[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 2, -64, 0, 0, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks175[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 2, -64, 0, 0, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks176[] = {
    { 0, 0, 0, -32, 16, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -32, 16, { 0b1111, 0 }, 0 },
    { 2, -64, 0, -32, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks177[] = {
    { 0, 0, 0, -32, 16, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -32, 16, { 0b1111, 0 }, 0 },
    { 2, -64, 0, -32, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks178[] = {
    { 0, 0, 0, 0, 0, { 0b0111, 0b0100 }, 0 },
    { 1, 0, -32, 0, 16, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 16, { 0b0010, 0 }, 0 },
    { 3, -32, -32, 16, 16,{ 0b0111, 0b0110 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks179[] = {
    { 0, 0, 0, 0, 16, { 0b1011, 0b1000 }, 0 },
    { 1, 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 16, { 0b0001, 0 }, 0 },
    { 3, -32, 32, 16, 16,{ 0b1011, 0b1001 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks180[] = {
    { 0, 0, 0, 16, 16,{ 0b0111, 0b0011 }, 0 },
    { 1, 0, -32, 0, 16, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 16, { 0b0010, 0 }, 0 },
    { 3, -32, -32, 0, 0, { 0b0111, 0b0001 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks181[] = {
    { 0, 0, 0, 16, 16, { 0b1011, 0b0011 }, 0 },
    { 1, 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 16, { 0b0001, 0 }, 0 },
    { 3, -32, 32, 0, 0, { 0b1011, 0b0010 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks182[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks183[] = {
    { 0, 0, 0, 0, 24, { 0b1111, 0b1100 }, 0 },
    { 1, -32, 0, 16, 40, { 0b1111, 0b1100 }, 0 },
    { 2, -64, 0, 32, 56, { 0b1111, 0 }, 0 },
    { 3, -96, 0, 64, 192, { 0b1111, 0 }, 0 },
    { 4, -128, -32, 120, 96, { 0b1111, 0 }, 0 },
    { 5, -96, -32, 64, 192, { 0b1111, 0 }, 0 },
    { 6, -64, -32, 248, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks184[] = {
    { 0, 0, 0, 0, 24, { 0b1111, 0b1100 }, 0 },
    { 1, -32, 0, 16, 40, { 0b1111, 0b1100 }, 0 },
    { 2, -64, 0, 32, 56, { 0b1111, 0 }, 0 },
    { 3, -96, 0, 64, 192, { 0b1111, 0 }, 0 },
    { 4, -128, 32, 120, 96, { 0b1111, 0 }, 0 },
    { 5, -96, 32, 64, 192, { 0b1111, 0 }, 0 },
    { 6, -64, 32, 248, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks185[] = {
    { 0, 0, 0, -32, 24, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -216, 192, { 0b1111, 0 }, 0 },
    { 2, -64, 0, -160, 96, { 0b1111, 0 }, 0 },
    { 3, -32, -32, -216, 192, { 0b1111, 0 }, 0 },
    { 4, 0, -32, -248, 56, { 0b1111, 0 }, 0 },
    { 5, 32, -32, -264, 40, { 0b1111, 0b1100 }, 0 },
    { 6, 64, -32, -280, 24, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks186[] = {
    { 0, 0, 0, -32, 24, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -216, 192, { 0b1111, 0 }, 0 },
    { 2, -64, 0, -160, 96, { 0b1111, 0 }, 0 },
    { 3, -32, 32, -216, 192, { 0b1111, 0 }, 0 },
    { 4, 0, 32, -248, 56, { 0b1111, 0 }, 0 },
    { 5, 32, 32, -264, 40, { 0b1111, 0b1100 }, 0 },
    { 6, 64, 32, -280, 24, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks187[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -16, 16, { 0b1111, 0 }, 0 },
    { 2, -64, 0, -16, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks188[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -16, 16, { 0b1111, 0 }, 0 },
    { 2, -64, 0, -16, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks189[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 2, -64, 0, 16, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks190[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 2, -64, 0, 16, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks191[] = {
    { 0, 0, 0, 0, 32, { 0b1111, 0b1100 }, 0 },
    { 1, -32, 0, 16, 16, { 0b1111, 0b1100 }, 0 },
    { 2, -64, 0, 32, 96,{ 0b0011, 0 }, 0 },
    { 3, -32, 0, 120, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks192[] = {
    { 0, 0, 0, 0, 32, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -88, 96,{ 0b0011, 0 }, 0 },
    { 2, 0, 0, -104, 16, { 0b1111, 0b1100 }, 0 },
    { 3, 32, 0, -120, 16, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks193[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 24, 32, { 0b0111, 0 }, 0 },
    { 2, -32, -32, 48, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks194[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 24, 32, { 0b1011, 0 }, 0 },
    { 2, -32, 32, 48, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks195[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -24, 32, { 0b0111, 0 }, 0 },
    { 2, -32, -32, -48, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks196[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -24, 32, { 0b1011, 0 }, 0 },
    { 2, -32, 32, -48, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks197[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 32, { 0b1111, 0 }, 0 },
    { 2, -64, 0, 16, 0, { 0b1111, 0 }, 0 },
    { 3, 0, 0, 32, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks198[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, -32, 32, { 0b1111, 0 }, 0 },
    { 2, -64, 0, -16, 0, { 0b1111, 0 }, 0 },
    { 3, 0, 0, -32, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks199[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 2, -64, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 3, -96, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 4, -128, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 5, -160, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks200[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 2, -64, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 3, -96, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 4, -128, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 5, -160, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks201[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks202[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks203[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks204[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 2, -32, 32, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks205[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 2, -32, -32, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks206[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 1, -32, 0, -40, 32, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 2, -64, 0, -96, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 3, -96, 0, -96, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks207[] = {
    { 0, 0, 0, 0, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 1, 32, 0, 56, 32, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 2, 64, 0, 96, 16, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks208[] = {
    { 0, 0, 0, -32, 16, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 1, -32, 0, -72, 32, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 2, -64, 0, -128, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 3, -96, 0, -128, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks209[] = {
    { 0, 0, 0, 0, 0, { 0b0111, 0 }, 0 },
    { 1, 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 8, { 0b0010, 0 }, 0 },
    { 3, -32, -32, 0, 8, { 0b0111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks210[] = {
    { 0, 0, 0, 0, 0, { 0b1011, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 0, 8, { 0b0001, 0 }, 0 },
    { 3, -32, 32, 0, 8, { 0b1011, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks211[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, 0, -32, 0, 0, { 0b1111, 0 }, 0 },
    { 2, -32, -32, 0, 0, { 0b1111, 0 }, 0 },
    { 3, -64, -32, 0, 0, { 0b1111, 0 }, 0 },
    { 4, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 5, -64, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks212[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b1111, 0 }, 0 },
    { 2, -32, 32, 0, 0, { 0b1111, 0 }, 0 },
    { 3, -64, 32, 0, 0, { 0b1111, 0 }, 0 },
    { 4, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 5, -64, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks213[] = {
    { 0, 0, 0, 0, 32, { 0b1111, 0 }, 0 },
    { 1, 32, 0, 0, 32, { 0b1111, 0 }, 0 },
    { 2, -64, 0, 0, 32, { 0b1111, 0 }, 0 },
    { 3, -32, 0, 0, 32, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks214[] = {
    { 0, 0, 0, 0, 48, { 0b1111, 0 }, 0 },
    { 1, 32, 0, 0, 48, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks215[] = {
    { 0, 0, 0, 0, 208, { 0b1111, 0 }, 0 },
    { 1, 32, 0, 0, 208, { 0b1111, 0 }, 0 },
    { 2, -32, 0, 0, 160, { 0b1111, 0 }, 0 },
    { 3, -64, 0, 0, 80, { 0b1111, 0 }, 0 },
    { 4, -96, 0, 0, 48, { 0b1111, 0 }, 0 },
    { 5, -128, 0, 0, 32, { 0b1111, 0 }, 0 },
    { 6, -160, 0, 0, 16, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks216[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks217[] = {
    { 0, 0, 0, 0, 16, { 0b0111, 0b0100 }, 0 },
    { 1, 0, -32, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 16, 0, { 0b0010, 0 }, 0 },
    { 3, -32, -32, 16, 16,{ 0b0111, 0b0110 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks218[] = {
    { 0, 0, 0, 0, 16, { 0b1011, 0b1000 }, 0 },
    { 1, 0, 32, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 16, 0, { 0b0001, 0 }, 0 },
    { 3, -32, 32, 16, 16,{ 0b1011, 0b1001 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks219[] = {
    { 0, 0, 0, 16, 16,{ 0b0111, 0b0011 }, 0 },
    { 1, 0, -32, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 16, 0, { 0b0010, 0 }, 0 },
    { 3, -32, -32, 0, 16, { 0b0111, 0b0001 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks220[] = {
    { 0, 0, 0, 16, 16, { 0b1011, 0b0011 }, 0 },
    { 1, 0, 32, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 16, 0, { 0b0001, 0 }, 0 },
    { 3, -32, 32, 0, 16, { 0b1011, 0b0010 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks221[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
    { 1, 0, -32, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 16, 16, { 0b0111, 0b0100 }, 0 },
    { 3, -32, -32, 24, 16, { 0b1101, 0 }, 0 },
    { 4, -32, -64, 48, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, -32, 32, 16, { 0b0111, 0b0100 }, 0 },
    { 6, -64, -64, 48, 16, { 0b1111, 0b0110 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks222[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
    { 1, 0, 32, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 16, 16, { 0b1011, 0b1000 }, 0 },
    { 3, -32, 32, 24, 16, { 0b1110, 0 }, 0 },
    { 4, -32, 64, 48, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, 32, 32, 16, { 0b1011, 0b1000 }, 0 },
    { 6, -64, 64, 48, 16, { 0b1111, 0b1001 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks223[] = {
    { 0, 0, 0, 48, 16, { 0b1111, 0b0011 }, 0 },
    { 1, 0, -32, 48, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 32, 16, { 0b0111, 0b0001 }, 0 },
    { 3, -32, -32, 24, 16, { 0b1101, 0 }, 0 },
    { 4, -32, -64, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, -32, 16, 16, { 0b0111, 0b0001 }, 0 },
    { 6, -64, -64, 0, 16, { 0b1111, 0b1001 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks224[] = {
    { 0, 0, 0, 48, 16, { 0b1111, 0b0011 }, 0 },
    { 1, 0, 32, 48, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 2, -32, 0, 32, 16, { 0b1011, 0b0010 }, 0 },
    { 3, -32, 32, 24, 16, { 0b1110, 0 }, 0 },
    { 4, -32, 64, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
    { 5, -64, 32, 16, 16, { 0b1011, 0b0010 }, 0 },
    { 6, -64, 64, 0, 16, { 0b1111, 0b0110 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks225[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks226[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks227[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks228[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks229[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks230[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks231[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks232[] = {
    { 0, 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks233[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks234[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks235[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks236[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks237[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks238[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks239[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks240[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks241[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks242[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks243[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks244[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks245[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks246[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks247[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks248[] = {
    { 0, 0, 0, 0, 8, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks249[] = {
    { 0, 0, 0, 0, 72, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 1, 0, 32, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks250[] = {
    { 0, 0, 0, 0, 72, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 1, 0, -32, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks251[] = {
    { 0, 0, 0, 0, 72, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 1, 0, 32, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks252[] = {
    { 0, 0, 0, 0, 72, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 1, 0, -32, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks253[] = {
    { 0, 0, 0, 0, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 1, 32, 0, 56, 32, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 2, 64, 0, 96, 16, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks254[] = {
    { 0, 0, 0, -32, 16, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 1, -32, 0, -72, 32, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 2, -64, 0, -128, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 3, -96, 0, -128, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track TrackBlocks255[] = {
    { 0, 0, 0, 32, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 1, 32, 0, 88, 32, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    { 2, 64, 0, 128, 16, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    TRACK_BLOCK_END
};

// rct2: 0x00994638
const rct_preview_track *TrackBlocks[TRACK_ELEM_COUNT] = {
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
    TrackBlocks255
};

static constexpr const rct_preview_track FlatRideTrackBlocks095[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -64, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 2, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 3, 32, 0, 0, 0, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_1 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track FlatRideTrackBlocks110[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, 0, 32, 0, 0, { 0b1111, 0 }, 0 },
    { 2, 32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 3, 32, 32, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track FlatRideTrackBlocks111[] = {
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

static constexpr const rct_preview_track FlatRideTrackBlocks115[] = {
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

static constexpr const rct_preview_track FlatRideTrackBlocks116[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -64, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 2, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 3, 32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 4, 64, 0, 0, 0, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_1 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track FlatRideTrackBlocks118[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track FlatRideTrackBlocks119[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -64, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 2, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 3, 32, 0, 0, 0, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_1 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track FlatRideTrackBlocks121[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track FlatRideTrackBlocks122[] = {
    { 0, 0, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 1, -64, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 2, -32, 0, 0, 0, { 0b1111, 0 }, 0 },
    { 3, 32, 0, 0, 0, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_1 },
    TRACK_BLOCK_END
};

static constexpr const rct_preview_track FlatRideTrackBlocks123[] = {
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

static constexpr const rct_preview_track FlatRideTrackBlocks255[] = {
    TRACK_BLOCK_END
};

// rct2: 0x00994A38
const rct_preview_track *FlatRideTrackBlocks[TRACK_ELEM_COUNT] = {
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
    FlatRideTrackBlocks095,
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
    FlatRideTrackBlocks110,
    FlatRideTrackBlocks111,
    TrackBlocks112,
    TrackBlocks113,
    TrackBlocks114,
    FlatRideTrackBlocks115,
    FlatRideTrackBlocks116,
    TrackBlocks117,
    FlatRideTrackBlocks118,
    FlatRideTrackBlocks119,
    TrackBlocks120,
    FlatRideTrackBlocks121,
    FlatRideTrackBlocks122,
    FlatRideTrackBlocks123,
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
    FlatRideTrackBlocks255
};

const uint8_t TrackPieceLengths[TRACK_ELEM_COUNT] = {
    32,     // TRACK_ELEM_FLAT
    32,     // TRACK_ELEM_END_STATION
    32,     // TRACK_ELEM_BEGIN_STATION
    32,     // TRACK_ELEM_MIDDLE_STATION
    33,     // TRACK_ELEM_25_DEG_UP
    40,     // TRACK_ELEM_60_DEG_UP
    32,     // TRACK_ELEM_FLAT_TO_25_DEG_UP
    34,     // TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP
    34,     // TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP
    32,     // TRACK_ELEM_25_DEG_UP_TO_FLAT
    33,     // TRACK_ELEM_25_DEG_DOWN
    40,     // TRACK_ELEM_60_DEG_DOWN
    32,     // TRACK_ELEM_FLAT_TO_25_DEG_DOWN
    34,     // TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN
    34,     // TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN
    32,     // TRACK_ELEM_25_DEG_DOWN_TO_FLAT
    124,    // TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES
    124,    // TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES
    32,     // TRACK_ELEM_FLAT_TO_LEFT_BANK
    32,     // TRACK_ELEM_FLAT_TO_RIGHT_BANK
    32,     // TRACK_ELEM_LEFT_BANK_TO_FLAT
    32,     // TRACK_ELEM_RIGHT_BANK_TO_FLAT
    124,    // TRACK_ELEM_BANKED_LEFT_QUARTER_TURN_5_TILES
    124,    // TRACK_ELEM_BANKED_RIGHT_QUARTER_TURN_5_TILES
    32,     // TRACK_ELEM_LEFT_BANK_TO_25_DEG_UP
    32,     // TRACK_ELEM_RIGHT_BANK_TO_25_DEG_UP
    32,     // TRACK_ELEM_25_DEG_UP_TO_LEFT_BANK
    32,     // TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANK
    32,     // TRACK_ELEM_LEFT_BANK_TO_25_DEG_DOWN
    32,     // TRACK_ELEM_RIGHT_BANK_TO_25_DEG_DOWN
    32,     // TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANK
    32,     // TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANK
    32,     // TRACK_ELEM_LEFT_BANK
    32,     // TRACK_ELEM_RIGHT_BANK
    130,    // TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_UP
    130,    // TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_UP
    130,    // TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_DOWN
    130,    // TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_DOWN
    96,     // TRACK_ELEM_S_BEND_LEFT
    96,     // TRACK_ELEM_S_BEND_RIGHT
    120,    // TRACK_ELEM_LEFT_VERTICAL_LOOP
    120,    // TRACK_ELEM_RIGHT_VERTICAL_LOOP
    75,     // TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES
    75,     // TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES
    75,     // TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_BANK
    75,     // TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_BANK
    77,     // TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP
    77,     // TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP
    77,     // TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN
    77,     // TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN
    24,     // TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE
    24,     // TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE
    96,     // TRACK_ELEM_LEFT_TWIST_DOWN_TO_UP
    96,     // TRACK_ELEM_RIGHT_TWIST_DOWN_TO_UP
    96,     // TRACK_ELEM_LEFT_TWIST_UP_TO_DOWN
    96,     // TRACK_ELEM_RIGHT_TWIST_UP_TO_DOWN
    60,     // TRACK_ELEM_HALF_LOOP_UP
    60,     // TRACK_ELEM_HALF_LOOP_DOWN
    55,     // TRACK_ELEM_LEFT_CORKSCREW_UP
    55,     // TRACK_ELEM_RIGHT_CORKSCREW_UP
    55,     // TRACK_ELEM_LEFT_CORKSCREW_DOWN
    55,     // TRACK_ELEM_RIGHT_CORKSCREW_DOWN
    36,     // TRACK_ELEM_FLAT_TO_60_DEG_UP
    36,     // TRACK_ELEM_60_DEG_UP_TO_FLAT
    36,     // TRACK_ELEM_FLAT_TO_60_DEG_DOWN
    36,     // TRACK_ELEM_60_DEG_DOWN_TO_FLAT
    32,     // TRACK_ELEM_TOWER_BASE
    32,     // TRACK_ELEM_TOWER_SECTION
    32,     // TRACK_ELEM_FLAT_COVERED
    33,     // TRACK_ELEM_25_DEG_UP_COVERED
    40,     // TRACK_ELEM_60_DEG_UP_COVERED
    32,     // TRACK_ELEM_FLAT_TO_25_DEG_UP_COVERED
    34,     // TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP_COVERED
    34,     // TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP_COVERED
    32,     // TRACK_ELEM_25_DEG_UP_TO_FLAT_COVERED
    33,     // TRACK_ELEM_25_DEG_DOWN_COVERED
    40,     // TRACK_ELEM_60_DEG_DOWN_COVERED
    32,     // TRACK_ELEM_FLAT_TO_25_DEG_DOWN_COVERED
    34,     // TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN_COVERED
    34,     // TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN_COVERED
    32,     // TRACK_ELEM_25_DEG_DOWN_TO_FLAT_COVERED
    124,    // TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_COVERED
    124,    // TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_COVERED
    96,     // TRACK_ELEM_S_BEND_LEFT_COVERED
    96,     // TRACK_ELEM_S_BEND_RIGHT_COVERED
    75,     // TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_COVERED
    75,     // TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_COVERED
    150,    // TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_SMALL
    150,    // TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_SMALL
    150,    // TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_SMALL
    150,    // TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_SMALL
    248,    // TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_LARGE
    248,    // TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_LARGE
    248,    // TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_LARGE
    248,    // TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_LARGE
    64,     // TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_UP
    64,     // TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_UP
    64,     // TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_DOWN
    64,     // TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_DOWN
    32,     // TRACK_ELEM_BRAKES
    32,     // TRACK_ELEM_ROTATION_CONTROL_TOGGLE
    32,     // TRACK_ELEM_INVERTED_90_DEG_UP_TO_FLAT_QUARTER_LOOP
    124,    // TRACK_ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_UP
    124,    // TRACK_ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_UP
    124,    // TRACK_ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_DOWN
    124,    // TRACK_ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_DOWN
    124,    // TRACK_ELEM_LEFT_QUARTER_HELIX_LARGE_UP
    124,    // TRACK_ELEM_RIGHT_QUARTER_HELIX_LARGE_UP
    124,    // TRACK_ELEM_LEFT_QUARTER_HELIX_LARGE_DOWN
    124,    // TRACK_ELEM_RIGHT_QUARTER_HELIX_LARGE_DOWN
    33,     // TRACK_ELEM_25_DEG_UP_LEFT_BANKED
    33,     // TRACK_ELEM_25_DEG_UP_RIGHT_BANKED
    32,     // TRACK_ELEM_WATERFALL
    32,     // TRACK_ELEM_RAPIDS
    32,     // TRACK_ELEM_ON_RIDE_PHOTO
    33,     // TRACK_ELEM_25_DEG_DOWN_LEFT_BANKED
    33,     // TRACK_ELEM_25_DEG_DOWN_RIGHT_BANKED
    128,    // TRACK_ELEM_WATER_SPLASH
    165,    // TRACK_ELEM_FLAT_TO_60_DEG_UP_LONG_BASE
    165,    // TRACK_ELEM_60_DEG_UP_TO_FLAT_LONG_BASE
    32,     // TRACK_ELEM_WHIRLPOOL
    165,    // TRACK_ELEM_FLAT_TO_60_DEG_DOWN_LONG_BASE
    165,    // TRACK_ELEM_60_DEG_UP_TO_FLAT_LONG_BASE_122
    138,    // TRACK_ELEM_CABLE_LIFT_HILL
    32,     // TRACK_ELEM_REVERSE_FREEFALL_SLOPE
    32,     // TRACK_ELEM_REVERSE_FREEFALL_VERTICAL
    32,     // TRACK_ELEM_90_DEG_UP
    32,     // TRACK_ELEM_90_DEG_DOWN
    32,     // TRACK_ELEM_60_DEG_UP_TO_90_DEG_UP
    32,     // TRACK_ELEM_90_DEG_DOWN_TO_60_DEG_DOWN
    32,     // TRACK_ELEM_90_DEG_UP_TO_60_DEG_UP
    32,     // TRACK_ELEM_60_DEG_DOWN_TO_90_DEG_DOWN
    32,     // TRACK_ELEM_BRAKE_FOR_DROP
    87,     // TRACK_ELEM_LEFT_EIGHTH_TO_DIAG
    87,     // TRACK_ELEM_RIGHT_EIGHTH_TO_DIAG
    87,     // TRACK_ELEM_LEFT_EIGHTH_TO_ORTHOGONAL
    87,     // TRACK_ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL
    87,     // TRACK_ELEM_LEFT_EIGHTH_BANK_TO_DIAG
    87,     // TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_DIAG
    87,     // TRACK_ELEM_LEFT_EIGHTH_BANK_TO_ORTHOGONAL
    87,     // TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_ORTHOGONAL
    45,     // TRACK_ELEM_DIAG_FLAT
    45,     // TRACK_ELEM_DIAG_25_DEG_UP
    45,     // TRACK_ELEM_DIAG_60_DEG_UP
    45,     // TRACK_ELEM_DIAG_FLAT_TO_25_DEG_UP
    45,     // TRACK_ELEM_DIAG_25_DEG_UP_TO_60_DEG_UP
    45,     // TRACK_ELEM_DIAG_60_DEG_UP_TO_25_DEG_UP
    45,     // TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT
    45,     // TRACK_ELEM_DIAG_25_DEG_DOWN
    45,     // TRACK_ELEM_DIAG_60_DEG_DOWN
    45,     // TRACK_ELEM_DIAG_FLAT_TO_25_DEG_DOWN
    45,     // TRACK_ELEM_DIAG_25_DEG_DOWN_TO_60_DEG_DOWN
    45,     // TRACK_ELEM_DIAG_60_DEG_DOWN_TO_25_DEG_DOWN
    45,     // TRACK_ELEM_DIAG_25_DEG_DOWN_TO_FLAT
    45,     // TRACK_ELEM_DIAG_FLAT_TO_60_DEG_UP
    45,     // TRACK_ELEM_DIAG_60_DEG_UP_TO_FLAT
    45,     // TRACK_ELEM_DIAG_FLAT_TO_60_DEG_DOWN
    45,     // TRACK_ELEM_DIAG_60_DEG_DOWN_TO_FLAT
    45,     // TRACK_ELEM_DIAG_FLAT_TO_LEFT_BANK
    45,     // TRACK_ELEM_DIAG_FLAT_TO_RIGHT_BANK
    45,     // TRACK_ELEM_DIAG_LEFT_BANK_TO_FLAT
    45,     // TRACK_ELEM_DIAG_RIGHT_BANK_TO_FLAT
    45,     // TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_UP
    45,     // TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_UP
    45,     // TRACK_ELEM_DIAG_25_DEG_UP_TO_LEFT_BANK
    45,     // TRACK_ELEM_DIAG_25_DEG_UP_TO_RIGHT_BANK
    45,     // TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_DOWN
    45,     // TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_DOWN
    45,     // TRACK_ELEM_DIAG_25_DEG_DOWN_TO_LEFT_BANK
    45,     // TRACK_ELEM_DIAG_25_DEG_DOWN_TO_RIGHT_BANK
    45,     // TRACK_ELEM_DIAG_LEFT_BANK
    45,     // TRACK_ELEM_DIAG_RIGHT_BANK
    16,     // TRACK_ELEM_LOG_FLUME_REVERSER
    32,     // TRACK_ELEM_SPINNING_TUNNEL
    96,     // TRACK_ELEM_LEFT_BARREL_ROLL_UP_TO_DOWN
    96,     // TRACK_ELEM_RIGHT_BARREL_ROLL_UP_TO_DOWN
    96,     // TRACK_ELEM_LEFT_BARREL_ROLL_DOWN_TO_UP
    96,     // TRACK_ELEM_RIGHT_BARREL_ROLL_DOWN_TO_UP
    64,     // TRACK_ELEM_LEFT_BANK_TO_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP
    64,     // TRACK_ELEM_RIGHT_BANK_TO_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP
    64,     // TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_LEFT_BANK
    64,     // TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_RIGHT_BANK
    32,     // TRACK_ELEM_POWERED_LIFT
    100,    // TRACK_ELEM_LEFT_LARGE_HALF_LOOP_UP
    100,    // TRACK_ELEM_RIGHT_LARGE_HALF_LOOP_UP
    100,    // TRACK_ELEM_RIGHT_LARGE_HALF_LOOP_DOWN
    100,    // TRACK_ELEM_LEFT_LARGE_HALF_LOOP_DOWN
    96,     //
    96,     //
    96,     //
    96,     //
    64,     //
    64,     //
    64,     //
    64,     //
    64,     //
    64,     //
    16,     //
    16,     //
    64,     //
    64,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    80,     //
    80,     //
    80,     //
    64,     //
    64,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    32,     //
    80,     //
    80,     //
    80,     //
};

// rct2: 0x00998C95
const track_curve_chain gTrackCurveChain[TRACK_ELEM_COUNT] = {
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_END_STATION, RideConstructionSpecialPieceSelected | TRACK_ELEM_END_STATION },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_END_STATION, RideConstructionSpecialPieceSelected | TRACK_ELEM_END_STATION },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_END_STATION, RideConstructionSpecialPieceSelected | TRACK_ELEM_END_STATION },
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
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_LEFT_TWIST_UP_TO_DOWN, TRACK_CURVE_NONE },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_RIGHT_TWIST_UP_TO_DOWN, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, RideConstructionSpecialPieceSelected | TRACK_ELEM_LEFT_TWIST_DOWN_TO_UP },
    { TRACK_CURVE_NONE, RideConstructionSpecialPieceSelected | TRACK_ELEM_RIGHT_TWIST_DOWN_TO_UP },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_HALF_LOOP_DOWN, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, RideConstructionSpecialPieceSelected | TRACK_ELEM_HALF_LOOP_UP },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_RIGHT_CORKSCREW_DOWN, TRACK_CURVE_NONE },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_LEFT_CORKSCREW_DOWN, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, RideConstructionSpecialPieceSelected | TRACK_ELEM_RIGHT_CORKSCREW_UP },
    { TRACK_CURVE_NONE, RideConstructionSpecialPieceSelected | TRACK_ELEM_LEFT_CORKSCREW_UP },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_TOWER_SECTION, TRACK_CURVE_NONE },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_TOWER_SECTION, TRACK_CURVE_NONE },
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
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_SMALL, RideConstructionSpecialPieceSelected | TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_SMALL },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_SMALL, RideConstructionSpecialPieceSelected | TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_SMALL },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_SMALL, RideConstructionSpecialPieceSelected | TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_SMALL },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_SMALL, RideConstructionSpecialPieceSelected | TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_SMALL },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_LARGE, RideConstructionSpecialPieceSelected | TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_LARGE },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_LARGE, RideConstructionSpecialPieceSelected | TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_LARGE },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_LARGE, RideConstructionSpecialPieceSelected | TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_LARGE },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_LARGE, RideConstructionSpecialPieceSelected | TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_LARGE },
    { TRACK_CURVE_LEFT_SMALL, TRACK_CURVE_LEFT_SMALL },
    { TRACK_CURVE_RIGHT_SMALL, TRACK_CURVE_RIGHT_SMALL },
    { TRACK_CURVE_LEFT_SMALL, TRACK_CURVE_LEFT_SMALL },
    { TRACK_CURVE_RIGHT_SMALL, TRACK_CURVE_RIGHT_SMALL },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_BRAKES, RideConstructionSpecialPieceSelected | TRACK_ELEM_BRAKES },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_UP, RideConstructionSpecialPieceSelected | TRACK_ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_UP },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_UP, RideConstructionSpecialPieceSelected | TRACK_ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_UP },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_DOWN, RideConstructionSpecialPieceSelected | TRACK_ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_DOWN },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_DOWN, RideConstructionSpecialPieceSelected | TRACK_ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_DOWN },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_LEFT_QUARTER_HELIX_LARGE_UP, RideConstructionSpecialPieceSelected | TRACK_ELEM_LEFT_QUARTER_HELIX_LARGE_UP },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_RIGHT_QUARTER_HELIX_LARGE_UP, RideConstructionSpecialPieceSelected | TRACK_ELEM_RIGHT_QUARTER_HELIX_LARGE_UP },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_LEFT_QUARTER_HELIX_LARGE_DOWN, RideConstructionSpecialPieceSelected | TRACK_ELEM_LEFT_QUARTER_HELIX_LARGE_DOWN },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_RIGHT_QUARTER_HELIX_LARGE_DOWN, RideConstructionSpecialPieceSelected | TRACK_ELEM_RIGHT_QUARTER_HELIX_LARGE_DOWN },
    { TRACK_CURVE_LEFT, TRACK_CURVE_LEFT },
    { TRACK_CURVE_RIGHT, TRACK_CURVE_RIGHT },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_WATERFALL, RideConstructionSpecialPieceSelected | TRACK_ELEM_WATERFALL },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_RAPIDS, RideConstructionSpecialPieceSelected | TRACK_ELEM_RAPIDS },
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
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_REVERSE_FREEFALL_VERTICAL, TRACK_CURVE_NONE },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_REVERSE_FREEFALL_VERTICAL, RideConstructionSpecialPieceSelected | TRACK_ELEM_REVERSE_FREEFALL_VERTICAL },
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
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_LEFT_BARREL_ROLL_DOWN_TO_UP, TRACK_CURVE_NONE },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_RIGHT_BARREL_ROLL_DOWN_TO_UP, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, RideConstructionSpecialPieceSelected | TRACK_ELEM_LEFT_BARREL_ROLL_UP_TO_DOWN },
    { TRACK_CURVE_NONE, RideConstructionSpecialPieceSelected | TRACK_ELEM_RIGHT_BARREL_ROLL_UP_TO_DOWN },
    { TRACK_CURVE_LEFT_SMALL, TRACK_CURVE_LEFT_SMALL },
    { TRACK_CURVE_RIGHT_SMALL, TRACK_CURVE_RIGHT_SMALL },
    { TRACK_CURVE_LEFT_SMALL, TRACK_CURVE_LEFT_SMALL },
    { TRACK_CURVE_RIGHT_SMALL, TRACK_CURVE_RIGHT_SMALL },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_POWERED_LIFT, RideConstructionSpecialPieceSelected | TRACK_ELEM_POWERED_LIFT },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_LEFT_LARGE_HALF_LOOP_DOWN, TRACK_CURVE_NONE },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_RIGHT_LARGE_HALF_LOOP_DOWN, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, RideConstructionSpecialPieceSelected | TRACK_ELEM_RIGHT_LARGE_HALF_LOOP_UP },
    { TRACK_CURVE_NONE, RideConstructionSpecialPieceSelected | TRACK_ELEM_LEFT_LARGE_HALF_LOOP_UP },
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
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_INVERTED_FLAT_TO_90_DEG_QUARTER_LOOP_DOWN, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, RideConstructionSpecialPieceSelected | TRACK_ELEM_90_DEG_TO_INVERTED_FLAT_QUARTER_LOOP_UP },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_LEFT_CURVED_LIFT_HILL, RideConstructionSpecialPieceSelected | TRACK_ELEM_LEFT_CURVED_LIFT_HILL },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_RIGHT_CURVED_LIFT_HILL, RideConstructionSpecialPieceSelected | TRACK_ELEM_RIGHT_CURVED_LIFT_HILL },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { TRACK_CURVE_NONE, TRACK_CURVE_NONE },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_AIR_THRUST_VERTICAL_DOWN, RideConstructionSpecialPieceSelected | TRACK_ELEM_REVERSE_FREEFALL_VERTICAL },
    { RideConstructionSpecialPieceSelected | TRACK_ELEM_AIR_THRUST_VERTICAL_DOWN, RideConstructionSpecialPieceSelected | TRACK_ELEM_AIR_THRUST_VERTICAL_DOWN },
    { TRACK_CURVE_NONE, RideConstructionSpecialPieceSelected | TRACK_ELEM_AIR_THRUST_VERTICAL_DOWN },
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
};

// rct2: 0x00999095
const track_curve_chain gFlatRideTrackCurveChain[TRACK_ELEM_COUNT] = {
    { 0, 0 },
    { 257, 257 },
    { 257, 257 },
    { 257, 257 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 1, 1 },
    { 2, 2 },
    { 1, 0 },
    { 2, 0 },
    { 0, 1 },
    { 0, 2 },
    { 1, 1 },
    { 2, 2 },
    { 0, 1 },
    { 0, 2 },
    { 1, 0 },
    { 2, 0 },
    { 0, 1 },
    { 0, 2 },
    { 1, 0 },
    { 2, 0 },
    { 1, 1 },
    { 2, 2 },
    { 1, 1 },
    { 2, 2 },
    { 1, 1 },
    { 2, 2 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 3, 3 },
    { 4, 4 },
    { 3, 3 },
    { 4, 4 },
    { 3, 3 },
    { 4, 4 },
    { 3, 3 },
    { 4, 4 },
    { 5, 5 },
    { 6, 6 },
    { 310, 0 },
    { 311, 0 },
    { 0, 308 },
    { 0, 309 },
    { 313, 0 },
    { 0, 312 },
    { 317, 0 },
    { 316, 0 },
    { 0, 315 },
    { 0, 314 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 323, 0 },
    { 323, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 1, 1 },
    { 2, 2 },
    { 0, 0 },
    { 0, 0 },
    { 3, 3 },
    { 4, 4 },
    { 343, 343 },
    { 344, 344 },
    { 345, 345 },
    { 346, 346 },
    { 347, 347 },
    { 348, 348 },
    { 349, 349 },
    { 350, 350 },
    { 65535, 65535 },
    { 4, 4 },
    { 3, 3 },
    { 4, 4 },
    { 355, 355 },
    { 0, 0 },
    { 0, 0 },
    { 358, 358 },
    { 359, 359 },
    { 360, 360 },
    { 361, 361 },
    { 362, 362 },
    { 363, 363 },
    { 364, 364 },
    { 365, 365 },
    { 65535, 65535 },
    { 65535, 65535 },
    { 368, 368 },
    { 369, 369 },
    { 0, 0 },
    { 65535, 65535 },
    { 65535, 65535 },
    { 0, 0 },
    { 65535, 65535 },
    { 65535, 65535 },
    { 0, 0 },
    { 65535, 65535 },
    { 65535, 65535 },
    { 65535, 65535 },
    { 381, 0 },
    { 381, 381 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 7, 7 },
    { 8, 8 },
    { 7, 7 },
    { 8, 8 },
    { 7, 7 },
    { 8, 8 },
    { 7, 7 },
    { 8, 8 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 7, 0 },
    { 8, 0 },
    { 0, 7 },
    { 0, 8 },
    { 0, 7 },
    { 0, 8 },
    { 7, 0 },
    { 8, 0 },
    { 0, 7 },
    { 0, 8 },
    { 7, 0 },
    { 8, 0 },
    { 7, 7 },
    { 8, 8 },
    { 0, 0 },
    { 0, 0 },
    { 432, 0 },
    { 433, 0 },
    { 0, 430 },
    { 0, 431 },
    { 3, 3 },
    { 4, 4 },
    { 3, 3 },
    { 4, 4 },
    { 438, 190 },
    { 442, 0 },
    { 441, 0 },
    { 0, 440 },
    { 0, 439 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 465, 465 },
    { 466, 466 },
    { 0, 0 },
    { 0, 0 },
    { 470, 381 },
    { 470, 470 },
    { 0, 470 },
    { 0, 0 },
    { 3, 3 },
    { 4, 4 },
    { 3, 3 },
    { 4, 4 },
    { 1, 1 },
    { 2, 2 },
    { 1, 1 },
    { 2, 2 },
    { 1, 0 },
    { 2, 0 },
    { 0, 1 },
    { 0, 1 },
    { 1, 0 },
    { 2, 0 },
    { 0, 1 },
    { 0, 2 },
    { 1, 1 },
    { 2, 2 },
    { 1, 1 },
    { 2, 2 },
    { 1, 1 },
    { 2, 2 },
    { 1, 1 },
    { 2, 2 },
    { 1, 0 },
    { 2, 0 },
    { 0, 1 },
    { 0, 2 },
    { 1, 0 },
    { 2, 0 },
    { 0, 1 },
    { 0, 2 },
    { 3, 3 },
    { 4, 4 },
    { 3, 3 },
    { 4, 4 },
    { 0, 0 },
    { 0, 0 },
    { 0, 57088 },
};

const track_descriptor gTrackDescriptors[142] = {
    {   true,   TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TRACK_ELEM_DIAG_60_DEG_DOWN                                     },
    {   true,   TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_ELEM_DIAG_60_DEG_DOWN_TO_25_DEG_DOWN                      },
    {   true,   TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_ELEM_DIAG_60_DEG_DOWN_TO_FLAT                             },
    {   true,   TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TRACK_ELEM_DIAG_25_DEG_DOWN_TO_60_DEG_DOWN                      },
    {   true,   TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_ELEM_DIAG_25_DEG_DOWN                                     },
    {   true,   TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_ELEM_DIAG_25_DEG_DOWN_TO_LEFT_BANK                        },
    {   true,   TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_ELEM_DIAG_25_DEG_DOWN_TO_FLAT                             },
    {   true,   TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_ELEM_DIAG_25_DEG_DOWN_TO_RIGHT_BANK                       },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_CURVE_LEFT_LARGE,         TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_ELEM_LEFT_EIGHTH_BANK_TO_ORTHOGONAL                       },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_DOWN                        },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_ELEM_DIAG_LEFT_BANK                                       },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_ELEM_DIAG_LEFT_BANK_TO_FLAT                               },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_UP                          },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_LEFT_LARGE,         TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_ELEM_LEFT_EIGHTH_TO_ORTHOGONAL                            },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TRACK_ELEM_DIAG_FLAT_TO_60_DEG_DOWN                             },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_ELEM_DIAG_FLAT_TO_25_DEG_DOWN                             },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_ELEM_DIAG_FLAT_TO_LEFT_BANK                               },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_ELEM_DIAG_FLAT                                            },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_ELEM_DIAG_FLAT_TO_RIGHT_BANK                              },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_ELEM_DIAG_FLAT_TO_25_DEG_UP                               },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TRACK_ELEM_DIAG_FLAT_TO_60_DEG_UP                               },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_RIGHT_LARGE,        TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL                           },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_DOWN                       },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_ELEM_DIAG_RIGHT_BANK_TO_FLAT                              },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_ELEM_DIAG_RIGHT_BANK                                      },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_UP                         },
    {   true,   TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_CURVE_RIGHT_LARGE,        TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_ORTHOGONAL                      },
    {   true,   TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_ELEM_DIAG_25_DEG_UP_TO_LEFT_BANK                          },
    {   true,   TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT                               },
    {   true,   TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_ELEM_DIAG_25_DEG_UP_TO_RIGHT_BANK                         },
    {   true,   TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_ELEM_DIAG_25_DEG_UP                                       },
    {   true,   TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TRACK_ELEM_DIAG_25_DEG_UP_TO_60_DEG_UP                          },
    {   true,   TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_ELEM_DIAG_60_DEG_UP_TO_FLAT                               },
    {   true,   TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_ELEM_DIAG_60_DEG_UP_TO_25_DEG_UP                          },
    {   true,   TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TRACK_ELEM_DIAG_60_DEG_UP                                       },
    {   false,  TRACK_SLOPE_DOWN_90,    TRACK_BANK_NONE,    TRACK_CURVE_LEFT_SMALL,         TRACK_SLOPE_DOWN_90,    TRACK_BANK_NONE,    TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_90_DEG_DOWN                 },
    {   false,  TRACK_SLOPE_DOWN_90,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_90,    TRACK_BANK_NONE,    TRACK_ELEM_90_DEG_DOWN                                          },
    {   false,  TRACK_SLOPE_DOWN_90,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TRACK_ELEM_90_DEG_DOWN_TO_60_DEG_DOWN                           },
    {   false,  TRACK_SLOPE_DOWN_90,    TRACK_BANK_NONE,    TRACK_CURVE_RIGHT_SMALL,        TRACK_SLOPE_DOWN_90,    TRACK_BANK_NONE,    TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_90_DEG_DOWN                },
    {   false,  TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TRACK_CURVE_LEFT_SMALL,         TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_DOWN                 },
    {   false,  TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_90,    TRACK_BANK_NONE,    TRACK_ELEM_60_DEG_DOWN_TO_90_DEG_DOWN                           },
    {   false,  TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TRACK_ELEM_60_DEG_DOWN                                          },
    {   false,  TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN                           },
    {   false,  TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_ELEM_60_DEG_DOWN_TO_FLAT                                  },
    {   false,  TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TRACK_CURVE_RIGHT_SMALL,        TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_DOWN                },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_LEFT,    TRACK_CURVE_LEFT_SMALL,         TRACK_SLOPE_DOWN_25,    TRACK_BANK_LEFT,    TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN          },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_LEFT,    TRACK_CURVE_LEFT,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_LEFT,    TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN          },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_LEFT,    TRACK_ELEM_25_DEG_DOWN_LEFT_BANKED                              },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN               },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_LEFT_BANKED_FLAT          },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_FLAT                      },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_LEFT_SMALL,         TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN                },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_LEFT_SMALL,         TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_LEFT_BANK   },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_LEFT,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_DOWN                },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN                           },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_LEFT,    TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANKED_25_DEG_DOWN               },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_ELEM_25_DEG_DOWN                                          },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_RIGHT,   TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANKED_25_DEG_DOWN              },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANK                             },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_ELEM_25_DEG_DOWN_TO_FLAT                                  },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANK                            },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_RIGHT,              TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_DOWN               },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_RIGHT_SMALL,        TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_RIGHT_BANK },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_CURVE_RIGHT_SMALL,        TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN               },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN              },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_RIGHT,   TRACK_ELEM_25_DEG_DOWN_RIGHT_BANKED                             },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_FLAT                     },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_RIGHT_BANKED_FLAT        },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_RIGHT,   TRACK_CURVE_RIGHT,              TRACK_SLOPE_DOWN_25,    TRACK_BANK_RIGHT,   TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN         },
    {   false,  TRACK_SLOPE_DOWN_25,    TRACK_BANK_RIGHT,   TRACK_CURVE_RIGHT_SMALL,        TRACK_SLOPE_DOWN_25,    TRACK_BANK_RIGHT,   TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN         },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_CURVE_LEFT_SMALL,         TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_BANK                       },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_CURVE_LEFT_SMALL,         TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_ELEM_LEFT_BANK_TO_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP     },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_CURVE_LEFT,               TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_ELEM_BANKED_LEFT_QUARTER_TURN_5_TILES                     },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_CURVE_LEFT_LARGE,         TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_ELEM_LEFT_EIGHTH_BANK_TO_DIAG                             },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_LEFT,    TRACK_ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_DOWN          },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_ELEM_LEFT_BANK_TO_25_DEG_DOWN                             },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_ELEM_LEFT_BANK                                            },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_ELEM_LEFT_BANK_TO_FLAT                                    },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_LEFT,    TRACK_ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_UP            },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_ELEM_LEFT_BANK_TO_25_DEG_UP                               },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_LEFT_SMALL,         TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES                            },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_LEFT,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES                            },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_LEFT_LARGE,         TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_ELEM_LEFT_EIGHTH_TO_DIAG                                  },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_60,    TRACK_BANK_NONE,    TRACK_ELEM_FLAT_TO_60_DEG_DOWN                                  },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_LEFT,    TRACK_ELEM_FLAT_TO_LEFT_BANKED_25_DEG_DOWN                      },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_ELEM_FLAT_TO_25_DEG_DOWN                                  },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_RIGHT,   TRACK_ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN                     },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_ELEM_FLAT_TO_LEFT_BANK                                    },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_ELEM_FLAT                                                 },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_LEFT_VERY_SMALL,    TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE                             },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_RIGHT_VERY_SMALL,   TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE                            },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_ELEM_FLAT_TO_RIGHT_BANK                                   },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_LEFT,    TRACK_ELEM_FLAT_TO_LEFT_BANKED_25_DEG_UP                        },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_ELEM_FLAT_TO_25_DEG_UP                                    },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_RIGHT,   TRACK_ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_UP                       },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TRACK_ELEM_FLAT_TO_60_DEG_UP                                    },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_RIGHT_LARGE,        TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_ELEM_RIGHT_EIGHTH_TO_DIAG                                 },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_RIGHT,              TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES                           },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_CURVE_RIGHT_SMALL,        TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES                           },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_NONE,    TRACK_ELEM_RIGHT_BANK_TO_25_DEG_DOWN                            },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_DOWN_25,    TRACK_BANK_RIGHT,   TRACK_ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN        },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_ELEM_RIGHT_BANK_TO_FLAT                                   },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_ELEM_RIGHT_BANK                                           },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_ELEM_RIGHT_BANK_TO_25_DEG_UP                              },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_RIGHT,   TRACK_ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_UP          },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_CURVE_RIGHT_LARGE,        TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_DIAG                            },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_CURVE_RIGHT,              TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_ELEM_BANKED_RIGHT_QUARTER_TURN_5_TILES                    },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_CURVE_RIGHT_SMALL,        TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_BANK                      },
    {   false,  TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_CURVE_RIGHT_SMALL,        TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_ELEM_RIGHT_BANK_TO_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP   },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_LEFT,    TRACK_CURVE_LEFT_SMALL,         TRACK_SLOPE_UP_25,      TRACK_BANK_LEFT,    TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP            },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_LEFT,    TRACK_CURVE_LEFT,               TRACK_SLOPE_UP_25,      TRACK_BANK_LEFT,    TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP            },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_LEFT_BANKED_FLAT            },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_FLAT                        },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_LEFT,    TRACK_ELEM_25_DEG_UP_LEFT_BANKED                                },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_LEFT,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_25_DEG_UP                   },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_CURVE_LEFT_SMALL,         TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP                  },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_CURVE_LEFT,               TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_UP                  },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_LEFT,    TRACK_ELEM_25_DEG_UP_TO_LEFT_BANK                               },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_ELEM_25_DEG_UP_TO_FLAT                                    },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANK                              },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_LEFT,    TRACK_ELEM_25_DEG_UP_TO_LEFT_BANKED_25_DEG_UP                   },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_ELEM_25_DEG_UP                                            },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_RIGHT,   TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANKED_25_DEG_UP                  },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP                               },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_CURVE_RIGHT,              TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_UP                 },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_CURVE_RIGHT_SMALL,        TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP                 },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_FLAT                       },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_RIGHT,   TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_RIGHT_BANKED_FLAT          },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_25_DEG_UP                  },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_RIGHT,   TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_RIGHT,   TRACK_ELEM_25_DEG_UP_RIGHT_BANKED                               },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_RIGHT,   TRACK_CURVE_RIGHT,              TRACK_SLOPE_UP_25,      TRACK_BANK_RIGHT,   TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP           },
    {   false,  TRACK_SLOPE_UP_25,      TRACK_BANK_RIGHT,   TRACK_CURVE_RIGHT_SMALL,        TRACK_SLOPE_UP_25,      TRACK_BANK_RIGHT,   TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP           },
    {   false,  TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TRACK_CURVE_LEFT_SMALL,         TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_UP                   },
    {   false,  TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_NONE,       TRACK_BANK_NONE,    TRACK_ELEM_60_DEG_UP_TO_FLAT                                    },
    {   false,  TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_25,      TRACK_BANK_NONE,    TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP                               },
    {   false,  TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TRACK_ELEM_60_DEG_UP                                            },
    {   false,  TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_90,      TRACK_BANK_NONE,    TRACK_ELEM_60_DEG_UP_TO_90_DEG_UP                               },
    {   false,  TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TRACK_CURVE_RIGHT_SMALL,        TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_UP                  },
    {   false,  TRACK_SLOPE_UP_90,      TRACK_BANK_NONE,    TRACK_CURVE_LEFT_SMALL,         TRACK_SLOPE_UP_90,      TRACK_BANK_NONE,    TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_90_DEG_UP                   },
    {   false,  TRACK_SLOPE_UP_90,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_60,      TRACK_BANK_NONE,    TRACK_ELEM_90_DEG_UP_TO_60_DEG_UP                               },
    {   false,  TRACK_SLOPE_UP_90,      TRACK_BANK_NONE,    TRACK_CURVE_NONE,               TRACK_SLOPE_UP_90,      TRACK_BANK_NONE,    TRACK_ELEM_90_DEG_UP                                            },
    {   false,  TRACK_SLOPE_UP_90,      TRACK_BANK_NONE,    TRACK_CURVE_RIGHT_SMALL,        TRACK_SLOPE_UP_90,      TRACK_BANK_NONE,    TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_90_DEG_UP                  },
};

/** rct2: 0x00993D1C */
const int16_t AlternativeTrackTypes[TRACK_ELEM_COUNT] = {
    TRACK_ELEM_FLAT_COVERED,                        // TRACK_ELEM_FLAT
    -1,
    -1,
    -1,
    TRACK_ELEM_25_DEG_UP_COVERED,                   // TRACK_ELEM_25_DEG_UP
    TRACK_ELEM_60_DEG_UP_COVERED,                   // TRACK_ELEM_60_DEG_UP
    TRACK_ELEM_FLAT_TO_25_DEG_UP_COVERED,           // TRACK_ELEM_FLAT_TO_25_DEG_UP
    TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP_COVERED,      // TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP
    TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP_COVERED,      // TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP
    TRACK_ELEM_25_DEG_UP_TO_FLAT_COVERED,           // TRACK_ELEM_25_DEG_UP_TO_FLAT
    TRACK_ELEM_25_DEG_DOWN_COVERED,                 // TRACK_ELEM_25_DEG_DOWN
    TRACK_ELEM_60_DEG_DOWN_COVERED,                 // TRACK_ELEM_60_DEG_DOWN
    TRACK_ELEM_FLAT_TO_25_DEG_DOWN_COVERED,         // TRACK_ELEM_FLAT_TO_25_DEG_DOWN
    TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN_COVERED,  // TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN
    TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN_COVERED,  // TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN
    TRACK_ELEM_25_DEG_DOWN_TO_FLAT_COVERED,         // TRACK_ELEM_25_DEG_DOWN_TO_FLAT
    TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_COVERED,   // TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES
    TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_COVERED,  // TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    TRACK_ELEM_S_BEND_LEFT_COVERED,                 // TRACK_ELEM_S_BEND_LEFT
    TRACK_ELEM_S_BEND_RIGHT_COVERED,                // TRACK_ELEM_S_BEND_RIGHT
    -1,
    -1,
    TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_COVERED,   // TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES
    TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_COVERED,  // TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
};

/** rct2: 0x0099DA34 */
const money32 TrackPricing[] = {
    65536,  // TRACK_ELEM_FLAT
    98304,  // TRACK_ELEM_END_STATION
    98304,  // TRACK_ELEM_BEGIN_STATION
    98304,  // TRACK_ELEM_MIDDLE_STATION
    79872,  // TRACK_ELEM_25_DEG_UP
    114688, // TRACK_ELEM_60_DEG_UP
    73728,  // TRACK_ELEM_FLAT_TO_25_DEG_UP
    96256,  // TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP
    96256,  // TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP
    73728,  // TRACK_ELEM_25_DEG_UP_TO_FLAT
    79872,  // TRACK_ELEM_25_DEG_DOWN
    114688, // TRACK_ELEM_60_DEG_DOWN
    73728,  // TRACK_ELEM_FLAT_TO_25_DEG_DOWN
    96256,  // TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN
    96256,  // TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN
    73728,  // TRACK_ELEM_25_DEG_DOWN_TO_FLAT
    257359, // TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES
    257359, // TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES
    69632,  // TRACK_ELEM_FLAT_TO_LEFT_BANK
    69632,  // TRACK_ELEM_FLAT_TO_RIGHT_BANK
    69632,  // TRACK_ELEM_LEFT_BANK_TO_FLAT
    69632,  // TRACK_ELEM_RIGHT_BANK_TO_FLAT
    273443, // TRACK_ELEM_BANKED_LEFT_QUARTER_TURN_5_TILES
    273443, // TRACK_ELEM_BANKED_RIGHT_QUARTER_TURN_5_TILES
    78336,  // TRACK_ELEM_LEFT_BANK_TO_25_DEG_UP
    78336,  // TRACK_ELEM_RIGHT_BANK_TO_25_DEG_UP
    78336,  // TRACK_ELEM_25_DEG_UP_TO_LEFT_BANK
    78336,  // TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANK
    78336,  // TRACK_ELEM_LEFT_BANK_TO_25_DEG_DOWN
    78336,  // TRACK_ELEM_RIGHT_BANK_TO_25_DEG_DOWN
    78336,  // TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANK
    78336,  // TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANK
    69632,  // TRACK_ELEM_LEFT_BANK
    69632,  // TRACK_ELEM_RIGHT_BANK
    313656, // TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_UP
    313656, // TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_UP
    313656, // TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_DOWN
    313656, // TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_DOWN
    229376, // TRACK_ELEM_S_BEND_LEFT
    229376, // TRACK_ELEM_S_BEND_RIGHT
    491520, // TRACK_ELEM_LEFT_VERTICAL_LOOP
    491520, // TRACK_ELEM_RIGHT_VERTICAL_LOOP
    154415, // TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES
    154415, // TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES
    164065, // TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_BANK
    164065, // TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_BANK
    270226, // TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP
    270226, // TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP
    270226, // TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN
    270226, // TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN
    51471,  // TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE
    51471,  // TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE
    212992, // TRACK_ELEM_LEFT_TWIST_DOWN_TO_UP
    212992, // TRACK_ELEM_RIGHT_TWIST_DOWN_TO_UP
    212992, // TRACK_ELEM_LEFT_TWIST_UP_TO_DOWN
    212992, // TRACK_ELEM_RIGHT_TWIST_UP_TO_DOWN
    294912, // TRACK_ELEM_HALF_LOOP_UP
    294912, // TRACK_ELEM_HALF_LOOP_DOWN
    229376, // TRACK_ELEM_LEFT_CORKSCREW_UP
    229376, // TRACK_ELEM_RIGHT_CORKSCREW_UP
    229376, // TRACK_ELEM_LEFT_CORKSCREW_DOWN
    229376, // TRACK_ELEM_RIGHT_CORKSCREW_DOWN
    98304,  // TRACK_ELEM_FLAT_TO_60_DEG_UP
    98304,  // TRACK_ELEM_60_DEG_UP_TO_FLAT
    98304,  // TRACK_ELEM_FLAT_TO_60_DEG_DOWN
    98304,  // TRACK_ELEM_60_DEG_DOWN_TO_FLAT
    524288, // TRACK_ELEM_TOWER_BASE
    65536,  // TRACK_ELEM_TOWER_SECTION
    69632,  // TRACK_ELEM_FLAT_COVERED
    83968,  // TRACK_ELEM_25_DEG_UP_COVERED
    118784, // TRACK_ELEM_60_DEG_UP_COVERED
    77824,  // TRACK_ELEM_FLAT_TO_25_DEG_UP_COVERED
    100352, // TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP_COVERED
    100352, // TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP_COVERED
    77824,  // TRACK_ELEM_25_DEG_UP_TO_FLAT_COVERED
    83968,  // TRACK_ELEM_25_DEG_DOWN_COVERED
    118784, // TRACK_ELEM_60_DEG_DOWN_COVERED
    77824,  // TRACK_ELEM_FLAT_TO_25_DEG_DOWN_COVERED
    100352, // TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN_COVERED
    100352, // TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN_COVERED
    77824,  // TRACK_ELEM_25_DEG_DOWN_TO_FLAT_COVERED
    261455, // TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_COVERED
    261455, // TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_COVERED
    233472, // TRACK_ELEM_S_BEND_LEFT_COVERED
    233472, // TRACK_ELEM_S_BEND_RIGHT_COVERED
    158511, // TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_COVERED
    158511, // TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_COVERED
    328192, // TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_SMALL
    328192, // TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_SMALL
    328192, // TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_SMALL
    328192, // TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_SMALL
    547072, // TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_LARGE
    547072, // TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_LARGE
    547072, // TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_LARGE
    547072, // TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_LARGE
    126976, // TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_UP
    126976, // TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_UP
    126976, // TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_DOWN
    126976, // TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_DOWN
    90112,  // TRACK_ELEM_BRAKES
    77824,  // TRACK_ELEM_ROTATION_CONTROL_TOGGLE
    65536,  // TRACK_ELEM_INVERTED_90_DEG_UP_TO_FLAT_QUARTER_LOOP / TRACK_ELEM_MAZE
    273539, // TRACK_ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_UP
    273539, // TRACK_ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_UP
    273539, // TRACK_ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_DOWN
    273539, // TRACK_ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_DOWN
    273539, // TRACK_ELEM_LEFT_QUARTER_HELIX_LARGE_UP
    273539, // TRACK_ELEM_RIGHT_QUARTER_HELIX_LARGE_UP
    273539, // TRACK_ELEM_LEFT_QUARTER_HELIX_LARGE_DOWN
    273539, // TRACK_ELEM_RIGHT_QUARTER_HELIX_LARGE_DOWN
    83968,  // TRACK_ELEM_25_DEG_UP_LEFT_BANKED
    83968,  // TRACK_ELEM_25_DEG_UP_RIGHT_BANKED
    143360, // TRACK_ELEM_WATERFALL
    118784, // TRACK_ELEM_RAPIDS
    196608, // TRACK_ELEM_ON_RIDE_PHOTO
    83968,  // TRACK_ELEM_25_DEG_DOWN_LEFT_BANKED
    83968,  // TRACK_ELEM_25_DEG_DOWN_RIGHT_BANKED
    393216, // TRACK_ELEM_WATER_SPLASH
    376832, // TRACK_ELEM_FLAT_TO_60_DEG_UP_LONG_BASE
    376832, // TRACK_ELEM_60_DEG_UP_TO_FLAT_LONG_BASE
    126976, // TRACK_ELEM_WHIRLPOOL
    376832, // TRACK_ELEM_FLAT_TO_60_DEG_DOWN_LONG_BASE
    376832, // TRACK_ELEM_60_DEG_UP_TO_FLAT_LONG_BASE_122
    643072, // TRACK_ELEM_CABLE_LIFT_HILL
    491520, // TRACK_ELEM_REVERSE_FREEFALL_SLOPE
    65536,  // TRACK_ELEM_REVERSE_FREEFALL_VERTICAL
    69632,  // TRACK_ELEM_90_DEG_UP
    69632,  // TRACK_ELEM_90_DEG_DOWN
    114688, // TRACK_ELEM_60_DEG_UP_TO_90_DEG_UP
    114688, // TRACK_ELEM_90_DEG_DOWN_TO_60_DEG_DOWN
    114688, // TRACK_ELEM_90_DEG_UP_TO_60_DEG_UP
    114688, // TRACK_ELEM_60_DEG_DOWN_TO_90_DEG_DOWN
    147456, // TRACK_ELEM_BRAKE_FOR_DROP
    180151, // TRACK_ELEM_LEFT_EIGHTH_TO_DIAG
    180151, // TRACK_ELEM_RIGHT_EIGHTH_TO_DIAG
    180151, // TRACK_ELEM_LEFT_EIGHTH_TO_ORTHOGONAL
    180151, // TRACK_ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL
    190960, // TRACK_ELEM_LEFT_EIGHTH_BANK_TO_DIAG
    190960, // TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_DIAG
    190960, // TRACK_ELEM_LEFT_EIGHTH_BANK_TO_ORTHOGONAL
    190960, // TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_ORTHOGONAL
    92681,  // TRACK_ELEM_DIAG_FLAT
    111820, // TRACK_ELEM_DIAG_25_DEG_UP
    147456, // TRACK_ELEM_DIAG_60_DEG_UP
    102400, // TRACK_ELEM_DIAG_FLAT_TO_25_DEG_UP
    129024, // TRACK_ELEM_DIAG_25_DEG_UP_TO_60_DEG_UP
    129024, // TRACK_ELEM_DIAG_60_DEG_UP_TO_25_DEG_UP
    102400, // TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT
    111820, // TRACK_ELEM_DIAG_25_DEG_DOWN
    147456, // TRACK_ELEM_DIAG_60_DEG_DOWN
    102400, // TRACK_ELEM_DIAG_FLAT_TO_25_DEG_DOWN
    129024, // TRACK_ELEM_DIAG_25_DEG_DOWN_TO_60_DEG_DOWN
    129024, // TRACK_ELEM_DIAG_60_DEG_DOWN_TO_25_DEG_DOWN
    102400, // TRACK_ELEM_DIAG_25_DEG_DOWN_TO_FLAT
    107008, // TRACK_ELEM_DIAG_FLAT_TO_60_DEG_UP
    107008, // TRACK_ELEM_DIAG_60_DEG_UP_TO_FLAT
    107008, // TRACK_ELEM_DIAG_FLAT_TO_60_DEG_DOWN
    107008, // TRACK_ELEM_DIAG_60_DEG_DOWN_TO_FLAT
    96777,  // TRACK_ELEM_DIAG_FLAT_TO_LEFT_BANK
    96777,  // TRACK_ELEM_DIAG_FLAT_TO_RIGHT_BANK
    96777,  // TRACK_ELEM_DIAG_LEFT_BANK_TO_FLAT
    96777,  // TRACK_ELEM_DIAG_RIGHT_BANK_TO_FLAT
    106496, // TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_UP
    106496, // TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_UP
    106496, // TRACK_ELEM_DIAG_25_DEG_UP_TO_LEFT_BANK
    106496, // TRACK_ELEM_DIAG_25_DEG_UP_TO_RIGHT_BANK
    106496, // TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_DOWN
    106496, // TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_DOWN
    106496, // TRACK_ELEM_DIAG_25_DEG_DOWN_TO_LEFT_BANK
    106496, // TRACK_ELEM_DIAG_25_DEG_DOWN_TO_RIGHT_BANK
    96777,  // TRACK_ELEM_DIAG_LEFT_BANK
    96777,  // TRACK_ELEM_DIAG_RIGHT_BANK
    282624, // TRACK_ELEM_LOG_FLUME_REVERSER
    217088, // TRACK_ELEM_SPINNING_TUNNEL
    221184, // TRACK_ELEM_LEFT_BARREL_ROLL_UP_TO_DOWN
    221184, // TRACK_ELEM_RIGHT_BARREL_ROLL_UP_TO_DOWN
    221184, // TRACK_ELEM_LEFT_BARREL_ROLL_DOWN_TO_UP
    221184, // TRACK_ELEM_RIGHT_BARREL_ROLL_DOWN_TO_UP
    270226, // TRACK_ELEM_LEFT_BANK_TO_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP
    270226, // TRACK_ELEM_RIGHT_BANK_TO_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP
    270226, // TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_LEFT_BANK
    270226, // TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_RIGHT_BANK
    96384,  // TRACK_ELEM_POWERED_LIFT
    884736, // TRACK_ELEM_LEFT_LARGE_HALF_LOOP_UP
    884736, // TRACK_ELEM_RIGHT_LARGE_HALF_LOOP_UP
    884736, // TRACK_ELEM_RIGHT_LARGE_HALF_LOOP_DOWN
    884736, // TRACK_ELEM_LEFT_LARGE_HALF_LOOP_DOWN
    221184, // TRACK_ELEM_LEFT_FLYER_TWIST_UP
    221184, // TRACK_ELEM_RIGHT_FLYER_TWIST_UP
    221184, // TRACK_ELEM_LEFT_FLYER_TWIST_DOWN
    221184, // TRACK_ELEM_RIGHT_FLYER_TWIST_DOWN
    294912, // TRACK_ELEM_FLYER_HALF_LOOP_UP
    294912, // TRACK_ELEM_FLYER_HALF_LOOP_DOWN
    229376, // TRACK_ELEM_LEFT_FLYER_CORKSCREW_UP
    229376, // TRACK_ELEM_RIGHT_FLYER_CORKSCREW_UP
    229376, // TRACK_ELEM_LEFT_FLYER_CORKSCREW_DOWN
    229376, // TRACK_ELEM_RIGHT_FLYER_CORKSCREW_DOWN
    294912, // TRACK_ELEM_HEARTLINE_TRANSFER_UP
    294912, // TRACK_ELEM_HEARTLINE_TRANSFER_DOWN
    417792, // TRACK_ELEM_LEFT_HEARTLINE_ROLL
    417792, // TRACK_ELEM_RIGHT_HEARTLINE_ROLL
    159744, // TRACK_ELEM_MINI_GOLF_HOLE_A
    167936, // TRACK_ELEM_MINI_GOLF_HOLE_B
    172032, // TRACK_ELEM_MINI_GOLF_HOLE_C
    184320, // TRACK_ELEM_MINI_GOLF_HOLE_D
    184320, // TRACK_ELEM_MINI_GOLF_HOLE_E
    360448, // TRACK_ELEM_MULTIDIM_INVERTED_FLAT_TO_90_DEG_QUARTER_LOOP_DOWN
    360448, // TRACK_ELEM_90_DEG_TO_INVERTED_FLAT_QUARTER_LOOP_UP
    360448, // TRACK_ELEM_INVERTED_FLAT_TO_90_DEG_QUARTER_LOOP_DOWN
    270226, // TRACK_ELEM_LEFT_CURVED_LIFT_HILL
    270226, // TRACK_ELEM_RIGHT_CURVED_LIFT_HILL
    393216, // TRACK_ELEM_LEFT_REVERSER
    393216, // TRACK_ELEM_RIGHT_REVERSER
    65536,  // TRACK_ELEM_AIR_THRUST_TOP_CAP
    65536,  // TRACK_ELEM_AIR_THRUST_VERTICAL_DOWN
    491520, // TRACK_ELEM_AIR_THRUST_VERTICAL_DOWN_TO_LEVEL
    94208,  // TRACK_ELEM_BLOCK_BRAKES
    287115, // TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP
    287115, // TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP
    287115, // TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN
    287115, // TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN
    333259, // TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP
    333259, // TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP
    333259, // TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN
    333259, // TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN
    83968,  // TRACK_ELEM_25_DEG_UP_TO_LEFT_BANKED_25_DEG_UP
    83968,  // TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANKED_25_DEG_UP
    83968,  // TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_25_DEG_UP
    83968,  // TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_25_DEG_UP
    83968,  // TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANKED_25_DEG_DOWN
    83968,  // TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANKED_25_DEG_DOWN
    83968,  // TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN
    83968,  // TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN
    77824,  // TRACK_ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_UP
    77824,  // TRACK_ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_UP
    77824,  // TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_LEFT_BANKED_FLAT
    77824,  // TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_RIGHT_BANKED_FLAT
    77824,  // TRACK_ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_DOWN
    77824,  // TRACK_ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN
    77824,  // TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_LEFT_BANKED_FLAT
    77824,  // TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_RIGHT_BANKED_FLAT
    77824,  // TRACK_ELEM_FLAT_TO_LEFT_BANKED_25_DEG_UP
    77824,  // TRACK_ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_UP
    77824,  // TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_FLAT
    77824,  // TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_FLAT
    77824,  // TRACK_ELEM_FLAT_TO_LEFT_BANKED_25_DEG_DOWN
    77824,  // TRACK_ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN
    77824,  // TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_FLAT
    77824,  // TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_FLAT
    151552, // TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_90_DEG_UP
    151552, // TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_90_DEG_UP
    151552, // TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_90_DEG_DOWN
    151552, // TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_90_DEG_DOWN
    360448, // TRACK_ELEM_MULTIDIM_90_DEG_UP_TO_INVERTED_FLAT_QUARTER_LOOP
    360448, // TRACK_ELEM_MULTIDIM_FLAT_TO_90_DEG_DOWN_QUARTER_LOOP
    360448, //
};

/** rct2: 0x0099DE34 */
const money32 FlatRideTrackPricing[] = {
    65536,
    98304,
    98304,
    98304,
    79872,
    114688,
    73728,
    96256,
    96256,
    73728,
    79872,
    114688,
    73728,
    96256,
    96256,
    73728,
    257359,
    257359,
    69632,
    69632,
    69632,
    69632,
    273443,
    273443,
    78336,
    78336,
    78336,
    78336,
    78336,
    78336,
    78336,
    78336,
    69632,
    69632,
    313656,
    313656,
    313656,
    313656,
    229376,
    229376,
    491520,
    491520,
    154415,
    154415,
    164065,
    164065,
    270226,
    270226,
    270226,
    270226,
    51471,
    51471,
    212992,
    212992,
    212992,
    212992,
    294912,
    294912,
    229376,
    229376,
    229376,
    229376,
    98304,
    98304,
    98304,
    98304,
    524288,
    65536,
    69632,
    83968,
    118784,
    77824,
    100352,
    100352,
    77824,
    83968,
    118784,
    77824,
    100352,
    100352,
    77824,
    261455,
    261455,
    233472,
    233472,
    158511,
    158511,
    328192,
    328192,
    328192,
    328192,
    547072,
    547072,
    547072,
    547072,
    262144, // FLAT_TRACK_ELEM_1_X_4_A
    126976,
    126976,
    126976,
    90112,
    77824,
    65536,
    273539,
    273539,
    273539,
    273539,
    273539,
    273539,
    273539,
    273539,
    262144, // FLAT_TRACK_ELEM_2_X_2
    1048576,    // FLAT_TRACK_ELEM_4_X_4
    143360,
    118784,
    196608,
    524288,
    327680, // FLAT_TRACK_ELEM_1_X_5
    393216,
    65536,  // FLAT_TRACK_ELEM_1_X_1_A
    262144, // FLAT_TRACK_ELEM_1_X_4_B
    126976,
    65536,  // FLAT_TRACK_ELEM_1_X_1_B
    262144, // FLAT_TRACK_ELEM_1_X_4_C
    524288, // FLAT_TRACK_ELEM_3_X_3
    491520,
    65536,
    69632,
    69632,
    114688,
    114688,
    114688,
    114688,
    147456,
    180151,
    180151,
    180151,
    180151,
    190960,
    190960,
    190960,
    190960,
    92681,
    111820,
    147456,
    102400,
    129024,
    129024,
    102400,
    111820,
    147456,
    102400,
    129024,
    129024,
    102400,
    107008,
    107008,
    107008,
    107008,
    96777,
    96777,
    96777,
    96777,
    106496,
    106496,
    106496,
    106496,
    106496,
    106496,
    106496,
    106496,
    96777,
    96777,
    282624,
    217088,
    221184,
    221184,
    221184,
    221184,
    270226,
    270226,
    270226,
    270226,
    96384,
    884736,
    884736,
    884736,
    884736,
    221184,
    221184,
    221184,
    221184,
    294912,
    294912,
    229376,
    229376,
    229376,
    229376,
    294912,
    294912,
    417792,
    417792,
    159744,
    167936,
    172032,
    184320,
    184320,
    159744,
    159744,
    159744,
    270226,
    270226,
    393216,
    393216,
    65536,
    65536,
    491520,
    94208,
    287115,
    287115,
    287115,
    287115,
    333259,
    333259,
    333259,
    333259,
    83968,
    83968,
    83968,
    83968,
    83968,
    83968,
    83968,
    83968,
    77824,
    77824,
    77824,
    77824,
    77824,
    77824,
    77824,
    77824,
    77824,
    77824,
    77824,
    77824,
    77824,
    77824,
    77824,
    77824,
    151552,
    151552,
    151552,
    151552,
};

/** rct2: 0x0099E228, 0x0099E229, 0x0099E22A, 0x0099E22B */
const dodgems_track_size DodgemsTrackSize[] = {
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 4,    4,  59,     59 },   // FLAT_TRACK_ELEM_2_X_2
    { 4,    4,  123,    123 },  // FLAT_TRACK_ELEM_4_X_4
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 4,    4,  59,     123 },  // 115
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
    { 0,    0,  0,      0 },
};

/** rct2: 0x0099EA1C */
const uint8_t TrackElementMirrorMap[] = {
    TRACK_ELEM_FLAT,
    TRACK_ELEM_END_STATION,
    TRACK_ELEM_BEGIN_STATION,
    TRACK_ELEM_MIDDLE_STATION,
    TRACK_ELEM_25_DEG_UP,
    TRACK_ELEM_60_DEG_UP,
    TRACK_ELEM_FLAT_TO_25_DEG_UP,
    TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP,
    TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP,
    TRACK_ELEM_25_DEG_UP_TO_FLAT,
    TRACK_ELEM_25_DEG_DOWN,
    TRACK_ELEM_60_DEG_DOWN,
    TRACK_ELEM_FLAT_TO_25_DEG_DOWN,
    TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN,
    TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN,
    TRACK_ELEM_25_DEG_DOWN_TO_FLAT,
    TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES, // TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES
    TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES, // TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES
    TRACK_ELEM_FLAT_TO_RIGHT_BANK, // TRACK_ELEM_FLAT_TO_LEFT_BANK
    TRACK_ELEM_FLAT_TO_LEFT_BANK, // TRACK_ELEM_FLAT_TO_RIGHT_BANK
    TRACK_ELEM_RIGHT_BANK_TO_FLAT, // TRACK_ELEM_LEFT_BANK_TO_FLAT
    TRACK_ELEM_LEFT_BANK_TO_FLAT, // TRACK_ELEM_RIGHT_BANK_TO_FLAT
    TRACK_ELEM_BANKED_RIGHT_QUARTER_TURN_5_TILES, // TRACK_ELEM_BANKED_LEFT_QUARTER_TURN_5_TILES
    TRACK_ELEM_BANKED_LEFT_QUARTER_TURN_5_TILES, // TRACK_ELEM_BANKED_RIGHT_QUARTER_TURN_5_TILES
    TRACK_ELEM_RIGHT_BANK_TO_25_DEG_UP, // TRACK_ELEM_LEFT_BANK_TO_25_DEG_UP
    TRACK_ELEM_LEFT_BANK_TO_25_DEG_UP, // TRACK_ELEM_RIGHT_BANK_TO_25_DEG_UP
    TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANK, // TRACK_ELEM_25_DEG_UP_TO_LEFT_BANK
    TRACK_ELEM_25_DEG_UP_TO_LEFT_BANK, // TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANK
    TRACK_ELEM_RIGHT_BANK_TO_25_DEG_DOWN, // TRACK_ELEM_LEFT_BANK_TO_25_DEG_DOWN
    TRACK_ELEM_LEFT_BANK_TO_25_DEG_DOWN, // TRACK_ELEM_RIGHT_BANK_TO_25_DEG_DOWN
    TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANK, // TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANK
    TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANK, // TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANK
    TRACK_ELEM_RIGHT_BANK, // TRACK_ELEM_LEFT_BANK
    TRACK_ELEM_LEFT_BANK, // TRACK_ELEM_RIGHT_BANK
    TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_UP, // TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_UP
    TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_UP, // TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_UP
    TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_DOWN, // TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_DOWN
    TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_DOWN, // TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_DOWN
    TRACK_ELEM_S_BEND_RIGHT, // TRACK_ELEM_S_BEND_LEFT
    TRACK_ELEM_S_BEND_LEFT, // TRACK_ELEM_S_BEND_RIGHT
    TRACK_ELEM_RIGHT_VERTICAL_LOOP, // TRACK_ELEM_LEFT_VERTICAL_LOOP
    TRACK_ELEM_LEFT_VERTICAL_LOOP, // TRACK_ELEM_RIGHT_VERTICAL_LOOP
    TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES, // TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES
    TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES, // TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES
    TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_BANK, // TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_BANK
    TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_BANK, // TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_BANK
    TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP, // TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP
    TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP, // TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP
    TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN, // TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN
    TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN, // TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN
    TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE, // TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE
    TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE, // TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE
    TRACK_ELEM_RIGHT_TWIST_DOWN_TO_UP, // TRACK_ELEM_LEFT_TWIST_DOWN_TO_UP
    TRACK_ELEM_LEFT_TWIST_DOWN_TO_UP, // TRACK_ELEM_RIGHT_TWIST_DOWN_TO_UP
    TRACK_ELEM_RIGHT_TWIST_UP_TO_DOWN, // TRACK_ELEM_LEFT_TWIST_UP_TO_DOWN
    TRACK_ELEM_LEFT_TWIST_UP_TO_DOWN, // TRACK_ELEM_RIGHT_TWIST_UP_TO_DOWN
    TRACK_ELEM_HALF_LOOP_UP,
    TRACK_ELEM_HALF_LOOP_DOWN,
    TRACK_ELEM_RIGHT_CORKSCREW_UP, // TRACK_ELEM_LEFT_CORKSCREW_UP
    TRACK_ELEM_LEFT_CORKSCREW_UP, // TRACK_ELEM_RIGHT_CORKSCREW_UP
    TRACK_ELEM_RIGHT_CORKSCREW_DOWN, // TRACK_ELEM_LEFT_CORKSCREW_DOWN
    TRACK_ELEM_LEFT_CORKSCREW_DOWN, // TRACK_ELEM_RIGHT_CORKSCREW_DOWN
    TRACK_ELEM_FLAT_TO_60_DEG_UP,
    TRACK_ELEM_60_DEG_UP_TO_FLAT,
    TRACK_ELEM_FLAT_TO_60_DEG_DOWN,
    TRACK_ELEM_60_DEG_DOWN_TO_FLAT,
    TRACK_ELEM_TOWER_BASE,
    TRACK_ELEM_TOWER_SECTION,
    TRACK_ELEM_FLAT_COVERED,
    TRACK_ELEM_25_DEG_UP_COVERED,
    TRACK_ELEM_60_DEG_UP_COVERED,
    TRACK_ELEM_FLAT_TO_25_DEG_UP_COVERED,
    TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP_COVERED,
    TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP_COVERED,
    TRACK_ELEM_25_DEG_UP_TO_FLAT_COVERED,
    TRACK_ELEM_25_DEG_DOWN_COVERED,
    TRACK_ELEM_60_DEG_DOWN_COVERED,
    TRACK_ELEM_FLAT_TO_25_DEG_DOWN_COVERED,
    TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN_COVERED,
    TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN_COVERED,
    TRACK_ELEM_25_DEG_DOWN_TO_FLAT_COVERED,
    TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_COVERED, // TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_COVERED
    TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_COVERED, // TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_COVERED
    TRACK_ELEM_S_BEND_RIGHT_COVERED, // TRACK_ELEM_S_BEND_LEFT_COVERED
    TRACK_ELEM_S_BEND_LEFT_COVERED, // TRACK_ELEM_S_BEND_RIGHT_COVERED
    TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_COVERED, // TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_COVERED
    TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_COVERED, // TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_COVERED
    TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_SMALL, // TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_SMALL
    TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_SMALL, // TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_SMALL
    TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_SMALL, // TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_SMALL
    TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_SMALL, // TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_SMALL
    TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_LARGE, // TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_LARGE
    TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_LARGE, // TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_LARGE
    TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_LARGE, // TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_LARGE
    TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_LARGE, // TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_LARGE
    TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_UP, // TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_UP
    TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_UP, // TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_UP
    TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_DOWN, // TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_DOWN
    TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_DOWN, // TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_DOWN
    TRACK_ELEM_BRAKES,
    TRACK_ELEM_ROTATION_CONTROL_TOGGLE,
    TRACK_ELEM_MAZE,
    TRACK_ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_UP, // TRACK_ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_UP
    TRACK_ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_UP, // TRACK_ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_UP
    TRACK_ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_DOWN, // TRACK_ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_DOWN
    TRACK_ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_DOWN, // TRACK_ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_DOWN
    TRACK_ELEM_RIGHT_QUARTER_HELIX_LARGE_UP, // TRACK_ELEM_LEFT_QUARTER_HELIX_LARGE_UP
    TRACK_ELEM_LEFT_QUARTER_HELIX_LARGE_UP, // TRACK_ELEM_RIGHT_QUARTER_HELIX_LARGE_UP
    TRACK_ELEM_RIGHT_QUARTER_HELIX_LARGE_DOWN, // TRACK_ELEM_LEFT_QUARTER_HELIX_LARGE_DOWN
    TRACK_ELEM_LEFT_QUARTER_HELIX_LARGE_DOWN, // TRACK_ELEM_RIGHT_QUARTER_HELIX_LARGE_DOWN
    TRACK_ELEM_25_DEG_UP_RIGHT_BANKED, // TRACK_ELEM_25_DEG_UP_LEFT_BANKED
    TRACK_ELEM_25_DEG_UP_LEFT_BANKED, // TRACK_ELEM_25_DEG_UP_RIGHT_BANKED
    TRACK_ELEM_WATERFALL,
    TRACK_ELEM_RAPIDS,
    TRACK_ELEM_ON_RIDE_PHOTO,
    TRACK_ELEM_25_DEG_DOWN_RIGHT_BANKED, // TRACK_ELEM_25_DEG_DOWN_LEFT_BANKED
    TRACK_ELEM_25_DEG_DOWN_LEFT_BANKED, // TRACK_ELEM_25_DEG_DOWN_RIGHT_BANKED
    TRACK_ELEM_WATER_SPLASH,
    TRACK_ELEM_FLAT_TO_60_DEG_UP_LONG_BASE,
    TRACK_ELEM_60_DEG_UP_TO_FLAT_LONG_BASE,
    TRACK_ELEM_WHIRLPOOL,
    TRACK_ELEM_FLAT_TO_60_DEG_DOWN_LONG_BASE,
    TRACK_ELEM_60_DEG_UP_TO_FLAT_LONG_BASE_122,
    TRACK_ELEM_CABLE_LIFT_HILL,
    TRACK_ELEM_REVERSE_FREEFALL_SLOPE,
    TRACK_ELEM_REVERSE_FREEFALL_VERTICAL,
    TRACK_ELEM_90_DEG_UP,
    TRACK_ELEM_90_DEG_DOWN,
    TRACK_ELEM_60_DEG_UP_TO_90_DEG_UP,
    TRACK_ELEM_90_DEG_DOWN_TO_60_DEG_DOWN,
    TRACK_ELEM_90_DEG_UP_TO_60_DEG_UP,
    TRACK_ELEM_60_DEG_DOWN_TO_90_DEG_DOWN,
    TRACK_ELEM_BRAKE_FOR_DROP,
    TRACK_ELEM_RIGHT_EIGHTH_TO_DIAG, // TRACK_ELEM_LEFT_EIGHTH_TO_DIAG
    TRACK_ELEM_LEFT_EIGHTH_TO_DIAG, // TRACK_ELEM_RIGHT_EIGHTH_TO_DIAG
    TRACK_ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL, // TRACK_ELEM_LEFT_EIGHTH_TO_ORTHOGONAL
    TRACK_ELEM_LEFT_EIGHTH_TO_ORTHOGONAL, // TRACK_ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL
    TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_DIAG, // TRACK_ELEM_LEFT_EIGHTH_BANK_TO_DIAG
    TRACK_ELEM_LEFT_EIGHTH_BANK_TO_DIAG, // TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_DIAG
    TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_ORTHOGONAL, // TRACK_ELEM_LEFT_EIGHTH_BANK_TO_ORTHOGONAL
    TRACK_ELEM_LEFT_EIGHTH_BANK_TO_ORTHOGONAL, // TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_ORTHOGONAL
    TRACK_ELEM_DIAG_FLAT,
    TRACK_ELEM_DIAG_25_DEG_UP,
    TRACK_ELEM_DIAG_60_DEG_UP,
    TRACK_ELEM_DIAG_FLAT_TO_25_DEG_UP,
    TRACK_ELEM_DIAG_25_DEG_UP_TO_60_DEG_UP,
    TRACK_ELEM_DIAG_60_DEG_UP_TO_25_DEG_UP,
    TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT,
    TRACK_ELEM_DIAG_25_DEG_DOWN,
    TRACK_ELEM_DIAG_60_DEG_DOWN,
    TRACK_ELEM_DIAG_FLAT_TO_25_DEG_DOWN,
    TRACK_ELEM_DIAG_25_DEG_DOWN_TO_60_DEG_DOWN,
    TRACK_ELEM_DIAG_60_DEG_DOWN_TO_25_DEG_DOWN,
    TRACK_ELEM_DIAG_25_DEG_DOWN_TO_FLAT,
    TRACK_ELEM_DIAG_FLAT_TO_60_DEG_UP,
    TRACK_ELEM_DIAG_60_DEG_UP_TO_FLAT,
    TRACK_ELEM_DIAG_FLAT_TO_60_DEG_DOWN,
    TRACK_ELEM_DIAG_60_DEG_DOWN_TO_FLAT,
    TRACK_ELEM_DIAG_FLAT_TO_RIGHT_BANK, // TRACK_ELEM_DIAG_FLAT_TO_LEFT_BANK
    TRACK_ELEM_DIAG_FLAT_TO_LEFT_BANK, // TRACK_ELEM_DIAG_FLAT_TO_RIGHT_BANK
    TRACK_ELEM_DIAG_RIGHT_BANK_TO_FLAT, // TRACK_ELEM_DIAG_LEFT_BANK_TO_FLAT
    TRACK_ELEM_DIAG_LEFT_BANK_TO_FLAT, // TRACK_ELEM_DIAG_RIGHT_BANK_TO_FLAT
    TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_UP, // TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_UP
    TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_UP, // TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_UP
    TRACK_ELEM_DIAG_25_DEG_UP_TO_RIGHT_BANK, // TRACK_ELEM_DIAG_25_DEG_UP_TO_LEFT_BANK
    TRACK_ELEM_DIAG_25_DEG_UP_TO_LEFT_BANK, // TRACK_ELEM_DIAG_25_DEG_UP_TO_RIGHT_BANK
    TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_DOWN, // TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_DOWN
    TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_DOWN, // TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_DOWN
    TRACK_ELEM_DIAG_25_DEG_DOWN_TO_RIGHT_BANK, // TRACK_ELEM_DIAG_25_DEG_DOWN_TO_LEFT_BANK
    TRACK_ELEM_DIAG_25_DEG_DOWN_TO_LEFT_BANK, // TRACK_ELEM_DIAG_25_DEG_DOWN_TO_RIGHT_BANK
    TRACK_ELEM_DIAG_RIGHT_BANK, // TRACK_ELEM_DIAG_LEFT_BANK
    TRACK_ELEM_DIAG_LEFT_BANK, // TRACK_ELEM_DIAG_RIGHT_BANK
    TRACK_ELEM_LOG_FLUME_REVERSER,
    TRACK_ELEM_SPINNING_TUNNEL,
    TRACK_ELEM_RIGHT_BARREL_ROLL_UP_TO_DOWN, // TRACK_ELEM_LEFT_BARREL_ROLL_UP_TO_DOWN
    TRACK_ELEM_LEFT_BARREL_ROLL_UP_TO_DOWN, // TRACK_ELEM_RIGHT_BARREL_ROLL_UP_TO_DOWN
    TRACK_ELEM_RIGHT_BARREL_ROLL_DOWN_TO_UP, // TRACK_ELEM_LEFT_BARREL_ROLL_DOWN_TO_UP
    TRACK_ELEM_LEFT_BARREL_ROLL_DOWN_TO_UP, // TRACK_ELEM_RIGHT_BARREL_ROLL_DOWN_TO_UP
    TRACK_ELEM_RIGHT_BANK_TO_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP, // TRACK_ELEM_LEFT_BANK_TO_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP
    TRACK_ELEM_LEFT_BANK_TO_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP, // TRACK_ELEM_RIGHT_BANK_TO_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP
    TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_RIGHT_BANK, // TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_LEFT_BANK
    TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_LEFT_BANK, // TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_RIGHT_BANK
    TRACK_ELEM_POWERED_LIFT,
    TRACK_ELEM_RIGHT_LARGE_HALF_LOOP_UP, // TRACK_ELEM_LEFT_LARGE_HALF_LOOP_UP
    TRACK_ELEM_LEFT_LARGE_HALF_LOOP_UP, // TRACK_ELEM_RIGHT_LARGE_HALF_LOOP_UP
    TRACK_ELEM_LEFT_LARGE_HALF_LOOP_DOWN, // TRACK_ELEM_RIGHT_LARGE_HALF_LOOP_DOWN
    TRACK_ELEM_RIGHT_LARGE_HALF_LOOP_DOWN, // TRACK_ELEM_LEFT_LARGE_HALF_LOOP_DOWN
    TRACK_ELEM_RIGHT_FLYER_TWIST_UP, // TRACK_ELEM_LEFT_FLYER_TWIST_UP
    TRACK_ELEM_LEFT_FLYER_TWIST_UP, // TRACK_ELEM_RIGHT_FLYER_TWIST_UP
    TRACK_ELEM_RIGHT_FLYER_TWIST_DOWN, // TRACK_ELEM_LEFT_FLYER_TWIST_DOWN
    TRACK_ELEM_LEFT_FLYER_TWIST_DOWN, // TRACK_ELEM_RIGHT_FLYER_TWIST_DOWN
    TRACK_ELEM_FLYER_HALF_LOOP_UP,
    TRACK_ELEM_FLYER_HALF_LOOP_DOWN,
    TRACK_ELEM_RIGHT_FLYER_CORKSCREW_UP, // TRACK_ELEM_LEFT_FLYER_CORKSCREW_UP
    TRACK_ELEM_LEFT_FLYER_CORKSCREW_UP, // TRACK_ELEM_RIGHT_FLYER_CORKSCREW_UP
    TRACK_ELEM_RIGHT_FLYER_CORKSCREW_DOWN, // TRACK_ELEM_LEFT_FLYER_CORKSCREW_DOWN
    TRACK_ELEM_LEFT_FLYER_CORKSCREW_DOWN, // TRACK_ELEM_RIGHT_FLYER_CORKSCREW_DOWN
    TRACK_ELEM_HEARTLINE_TRANSFER_UP,
    TRACK_ELEM_HEARTLINE_TRANSFER_DOWN,
    TRACK_ELEM_RIGHT_HEARTLINE_ROLL, // TRACK_ELEM_LEFT_HEARTLINE_ROLL
    TRACK_ELEM_LEFT_HEARTLINE_ROLL, // TRACK_ELEM_RIGHT_HEARTLINE_ROLL
    TRACK_ELEM_MINI_GOLF_HOLE_A,
    TRACK_ELEM_MINI_GOLF_HOLE_B,
    TRACK_ELEM_MINI_GOLF_HOLE_C,
    TRACK_ELEM_MINI_GOLF_HOLE_E, // TRACK_ELEM_MINI_GOLF_HOLE_D
    TRACK_ELEM_MINI_GOLF_HOLE_D, // TRACK_ELEM_MINI_GOLF_HOLE_E
    TRACK_ELEM_MULTIDIM_INVERTED_FLAT_TO_90_DEG_QUARTER_LOOP_DOWN,
    TRACK_ELEM_90_DEG_TO_INVERTED_FLAT_QUARTER_LOOP_UP,
    TRACK_ELEM_INVERTED_FLAT_TO_90_DEG_QUARTER_LOOP_DOWN,
    TRACK_ELEM_RIGHT_CURVED_LIFT_HILL, // TRACK_ELEM_LEFT_CURVED_LIFT_HILL
    TRACK_ELEM_LEFT_CURVED_LIFT_HILL, // TRACK_ELEM_RIGHT_CURVED_LIFT_HILL
    TRACK_ELEM_RIGHT_REVERSER, // TRACK_ELEM_LEFT_REVERSER
    TRACK_ELEM_LEFT_REVERSER, // TRACK_ELEM_RIGHT_REVERSER
    TRACK_ELEM_AIR_THRUST_TOP_CAP,
    TRACK_ELEM_AIR_THRUST_VERTICAL_DOWN,
    TRACK_ELEM_AIR_THRUST_VERTICAL_DOWN_TO_LEVEL,
    TRACK_ELEM_BLOCK_BRAKES,
    TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP, // TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP
    TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP, // TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP
    TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN, // TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN
    TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN, // TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN
    TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP, // TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP
    TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP, // TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP
    TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN, // TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN
    TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN, // TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN
    TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANKED_25_DEG_UP, // TRACK_ELEM_25_DEG_UP_TO_LEFT_BANKED_25_DEG_UP
    TRACK_ELEM_25_DEG_UP_TO_LEFT_BANKED_25_DEG_UP, // TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANKED_25_DEG_UP
    TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_25_DEG_UP, // TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_25_DEG_UP
    TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_25_DEG_UP, // TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_25_DEG_UP
    TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANKED_25_DEG_DOWN, // TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANKED_25_DEG_DOWN
    TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANKED_25_DEG_DOWN, // TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANKED_25_DEG_DOWN
    TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN, // TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN
    TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN, // TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN
    TRACK_ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_UP, // TRACK_ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_UP
    TRACK_ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_UP, // TRACK_ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_UP
    TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_RIGHT_BANKED_FLAT, // TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_LEFT_BANKED_FLAT
    TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_LEFT_BANKED_FLAT, // TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_RIGHT_BANKED_FLAT
    TRACK_ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN, // TRACK_ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_DOWN
    TRACK_ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_DOWN, // TRACK_ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN
    TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_RIGHT_BANKED_FLAT, // TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_LEFT_BANKED_FLAT
    TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_LEFT_BANKED_FLAT, // TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_RIGHT_BANKED_FLAT
    TRACK_ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_UP, // TRACK_ELEM_FLAT_TO_LEFT_BANKED_25_DEG_UP
    TRACK_ELEM_FLAT_TO_LEFT_BANKED_25_DEG_UP, // TRACK_ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_UP
    TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_FLAT, // TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_FLAT
    TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_FLAT, // TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_FLAT
    TRACK_ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN, // TRACK_ELEM_FLAT_TO_LEFT_BANKED_25_DEG_DOWN
    TRACK_ELEM_FLAT_TO_LEFT_BANKED_25_DEG_DOWN, // TRACK_ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN
    TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_FLAT, // TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_FLAT
    TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_FLAT, // TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_FLAT
    TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_90_DEG_UP, // TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_90_DEG_UP
    TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_90_DEG_UP, // TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_90_DEG_UP
    TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_90_DEG_DOWN, // TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_90_DEG_DOWN
    TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_90_DEG_DOWN, // TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_90_DEG_DOWN
    TRACK_ELEM_MULTIDIM_90_DEG_UP_TO_INVERTED_FLAT_QUARTER_LOOP,
    TRACK_ELEM_MULTIDIM_FLAT_TO_90_DEG_DOWN_QUARTER_LOOP,
    255,
};

/** rct2: 0x00999694 */
const uint32_t TrackHeightMarkerPositions[TRACK_ELEM_COUNT] = {
    (1 << 0), // TRACK_ELEM_FLAT
    (1 << 0), // TRACK_ELEM_END_STATION
    (1 << 0), // TRACK_ELEM_BEGIN_STATION
    (1 << 0), // TRACK_ELEM_MIDDLE_STATION
    (1 << 0), // TRACK_ELEM_25_DEG_UP
    (1 << 0), // TRACK_ELEM_60_DEG_UP
    (1 << 0), // TRACK_ELEM_FLAT_TO_25_DEG_UP
    (1 << 0), // TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP
    (1 << 0), // TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP
    (1 << 0), // TRACK_ELEM_25_DEG_UP_TO_FLAT
    (1 << 0), // TRACK_ELEM_25_DEG_DOWN
    (1 << 0), // TRACK_ELEM_60_DEG_DOWN
    (1 << 0), // TRACK_ELEM_FLAT_TO_25_DEG_DOWN
    (1 << 0), // TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN
    (1 << 0), // TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN
    (1 << 0), // TRACK_ELEM_25_DEG_DOWN_TO_FLAT
    (1 << 0) | (1 << 6), // TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES
    (1 << 0) | (1 << 6), // TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES
    (1 << 0), // TRACK_ELEM_FLAT_TO_LEFT_BANK
    (1 << 0), // TRACK_ELEM_FLAT_TO_RIGHT_BANK
    (1 << 0), // TRACK_ELEM_LEFT_BANK_TO_FLAT
    (1 << 0), // TRACK_ELEM_RIGHT_BANK_TO_FLAT
    (1 << 0) | (1 << 6), // TRACK_ELEM_BANKED_LEFT_QUARTER_TURN_5_TILES
    (1 << 0) | (1 << 6), // TRACK_ELEM_BANKED_RIGHT_QUARTER_TURN_5_TILES
    (1 << 0), // TRACK_ELEM_LEFT_BANK_TO_25_DEG_UP
    (1 << 0), // TRACK_ELEM_RIGHT_BANK_TO_25_DEG_UP
    (1 << 0), // TRACK_ELEM_25_DEG_UP_TO_LEFT_BANK
    (1 << 0), // TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANK
    (1 << 0), // TRACK_ELEM_LEFT_BANK_TO_25_DEG_DOWN
    (1 << 0), // TRACK_ELEM_RIGHT_BANK_TO_25_DEG_DOWN
    (1 << 0), // TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANK
    (1 << 0), // TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANK
    (1 << 0), // TRACK_ELEM_LEFT_BANK
    (1 << 0), // TRACK_ELEM_RIGHT_BANK
    (1 << 0) | (1 << 6), // TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_UP
    (1 << 0) | (1 << 6), // TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_UP
    (1 << 0) | (1 << 6), // TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_DOWN
    (1 << 0) | (1 << 6), // TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_DOWN
    (1 << 0) | (1 << 3), // TRACK_ELEM_S_BEND_LEFT
    (1 << 0) | (1 << 3), // TRACK_ELEM_S_BEND_RIGHT
    (1 << 0) | (1 << 9), // TRACK_ELEM_LEFT_VERTICAL_LOOP
    (1 << 0) | (1 << 9), // TRACK_ELEM_RIGHT_VERTICAL_LOOP
    (1 << 0) | (1 << 3), // TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES
    (1 << 0) | (1 << 3), // TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES
    (1 << 0) | (1 << 3), // TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_BANK
    (1 << 0) | (1 << 3), // TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_BANK
    (1 << 0) | (1 << 3), // TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP
    (1 << 0) | (1 << 3), // TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP
    (1 << 0) | (1 << 3), // TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN
    (1 << 0) | (1 << 3), // TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN
    (1 << 0), // TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE
    (1 << 0), // TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE
    (1 << 0) | (1 << 2), // TRACK_ELEM_LEFT_TWIST_DOWN_TO_UP
    (1 << 0) | (1 << 2), // TRACK_ELEM_RIGHT_TWIST_DOWN_TO_UP
    (1 << 0) | (1 << 2), // TRACK_ELEM_LEFT_TWIST_UP_TO_DOWN
    (1 << 0) | (1 << 2), // TRACK_ELEM_RIGHT_TWIST_UP_TO_DOWN
    (1 << 0) | (1 << 3), // TRACK_ELEM_HALF_LOOP_UP
    (1 << 0) | (1 << 3), // TRACK_ELEM_HALF_LOOP_DOWN
    (1 << 0) | (1 << 2), // TRACK_ELEM_LEFT_CORKSCREW_UP
    (1 << 0) | (1 << 2), // TRACK_ELEM_RIGHT_CORKSCREW_UP
    (1 << 0) | (1 << 2), // TRACK_ELEM_LEFT_CORKSCREW_DOWN
    (1 << 0) | (1 << 2), // TRACK_ELEM_RIGHT_CORKSCREW_DOWN
    (1 << 0), // TRACK_ELEM_FLAT_TO_60_DEG_UP
    (1 << 0), // TRACK_ELEM_60_DEG_UP_TO_FLAT
    (1 << 0), // TRACK_ELEM_FLAT_TO_60_DEG_DOWN
    (1 << 0), // TRACK_ELEM_60_DEG_DOWN_TO_FLAT
    (1 << 1) | (1 << 3) | (1 << 6) | (1 << 7), // TRACK_ELEM_TOWER_BASE
    (1 << 0), // TRACK_ELEM_TOWER_SECTION
    (1 << 0), // TRACK_ELEM_FLAT_COVERED
    (1 << 0), // TRACK_ELEM_25_DEG_UP_COVERED
    (1 << 0), // TRACK_ELEM_60_DEG_UP_COVERED
    (1 << 0), // TRACK_ELEM_FLAT_TO_25_DEG_UP_COVERED
    (1 << 0), // TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP_COVERED
    (1 << 0), // TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP_COVERED
    (1 << 0), // TRACK_ELEM_25_DEG_UP_TO_FLAT_COVERED
    (1 << 0), // TRACK_ELEM_25_DEG_DOWN_COVERED
    (1 << 0), // TRACK_ELEM_60_DEG_DOWN_COVERED
    (1 << 0), // TRACK_ELEM_FLAT_TO_25_DEG_DOWN_COVERED
    (1 << 0), // TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN_COVERED
    (1 << 0), // TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN_COVERED
    (1 << 0), // TRACK_ELEM_25_DEG_DOWN_TO_FLAT_COVERED
    (1 << 0) | (1 << 6), // TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_COVERED
    (1 << 0) | (1 << 6), // TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_COVERED
    (1 << 0) | (1 << 3), // TRACK_ELEM_S_BEND_LEFT_COVERED
    (1 << 0) | (1 << 3), // TRACK_ELEM_S_BEND_RIGHT_COVERED
    (1 << 0) | (1 << 3), // TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_COVERED
    (1 << 0) | (1 << 3), // TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_COVERED
    (1 << 0) | (1 << 7), // TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_SMALL
    (1 << 0) | (1 << 7), // TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_SMALL
    (1 << 0) | (1 << 7), // TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_SMALL
    (1 << 0) | (1 << 7), // TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_SMALL
    (1 << 0) | (1 << 13), // TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_LARGE
    (1 << 0) | (1 << 13), // TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_LARGE
    (1 << 0) | (1 << 13), // TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_LARGE
    (1 << 0) | (1 << 13), // TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_LARGE
    (1 << 0), // TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_UP
    (1 << 0), // TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_UP
    (1 << 0), // TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_DOWN
    (1 << 0), // TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_DOWN
    (1 << 0), // TRACK_ELEM_BRAKES
    (1 << 0), // TRACK_ELEM_ROTATION_CONTROL_TOGGLE
    (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11) | (1 << 12) | (1 << 13) | (1 << 14) | (1 << 15), // TRACK_ELEM_INVERTED_90_DEG_UP_TO_FLAT_QUARTER_LOOP
    (1 << 0) | (1 << 6), // TRACK_ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_UP
    (1 << 0) | (1 << 6), // TRACK_ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_UP
    (1 << 0) | (1 << 6), // TRACK_ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_DOWN
    (1 << 0) | (1 << 6), // TRACK_ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_DOWN
    (1 << 0) | (1 << 6), // TRACK_ELEM_LEFT_QUARTER_HELIX_LARGE_UP
    (1 << 0) | (1 << 6), // TRACK_ELEM_RIGHT_QUARTER_HELIX_LARGE_UP
    (1 << 0) | (1 << 6), // TRACK_ELEM_LEFT_QUARTER_HELIX_LARGE_DOWN
    (1 << 0) | (1 << 6), // TRACK_ELEM_RIGHT_QUARTER_HELIX_LARGE_DOWN
    (1 << 0), // TRACK_ELEM_25_DEG_UP_LEFT_BANKED
    (1 << 0) | (1 << 3) | (1 << 12) | (1 << 15), // TRACK_ELEM_25_DEG_UP_RIGHT_BANKED
    (1 << 0), // TRACK_ELEM_WATERFALL
    (1 << 0), // TRACK_ELEM_RAPIDS
    (1 << 0), // TRACK_ELEM_ON_RIDE_PHOTO
    (1 << 0) | (1 << 7), // TRACK_ELEM_25_DEG_DOWN_LEFT_BANKED
    (1 << 0), // TRACK_ELEM_25_DEG_DOWN_RIGHT_BANKED
    (1 << 0) | (1 << 4), // TRACK_ELEM_WATER_SPLASH
    (1 << 0) | (1 << 3), // TRACK_ELEM_FLAT_TO_60_DEG_UP_LONG_BASE
    (1 << 0) | (1 << 3), // TRACK_ELEM_60_DEG_UP_TO_FLAT_LONG_BASE
    (1 << 0), // TRACK_ELEM_WHIRLPOOL
    (1 << 0) | (1 << 3), // TRACK_ELEM_FLAT_TO_60_DEG_DOWN_LONG_BASE
    (1 << 0) | (1 << 3), // TRACK_ELEM_60_DEG_UP_TO_FLAT_LONG_BASE_122
    (1 << 0) | (1 << 3), // TRACK_ELEM_CABLE_LIFT_HILL
    (1 << 0) | (1 << 5), // TRACK_ELEM_REVERSE_FREEFALL_SLOPE
    (1 << 0), // TRACK_ELEM_REVERSE_FREEFALL_VERTICAL
    (1 << 0), // TRACK_ELEM_90_DEG_UP
    (1 << 0), // TRACK_ELEM_90_DEG_DOWN
    (1 << 0), // TRACK_ELEM_60_DEG_UP_TO_90_DEG_UP
    (1 << 0), // TRACK_ELEM_90_DEG_DOWN_TO_60_DEG_DOWN
    (1 << 0), // TRACK_ELEM_90_DEG_UP_TO_60_DEG_UP
    (1 << 0), // TRACK_ELEM_60_DEG_DOWN_TO_90_DEG_DOWN
    (1 << 0), // TRACK_ELEM_BRAKE_FOR_DROP
    (1 << 0) | (1 << 4), // TRACK_ELEM_LEFT_EIGHTH_TO_DIAG
    (1 << 0) | (1 << 4), // TRACK_ELEM_RIGHT_EIGHTH_TO_DIAG
    (1 << 0) | (1 << 4), // TRACK_ELEM_LEFT_EIGHTH_TO_ORTHOGONAL
    (1 << 0) | (1 << 4), // TRACK_ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL
    (1 << 0) | (1 << 4), // TRACK_ELEM_LEFT_EIGHTH_BANK_TO_DIAG
    (1 << 0) | (1 << 4), // TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_DIAG
    (1 << 0) | (1 << 4), // TRACK_ELEM_LEFT_EIGHTH_BANK_TO_ORTHOGONAL
    (1 << 0) | (1 << 4), // TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_ORTHOGONAL
    (1 << 0), // TRACK_ELEM_DIAG_FLAT
    (1 << 0), // TRACK_ELEM_DIAG_25_DEG_UP
    (1 << 0), // TRACK_ELEM_DIAG_60_DEG_UP
    (1 << 0), // TRACK_ELEM_DIAG_FLAT_TO_25_DEG_UP
    (1 << 0), // TRACK_ELEM_DIAG_25_DEG_UP_TO_60_DEG_UP
    (1 << 0), // TRACK_ELEM_DIAG_60_DEG_UP_TO_25_DEG_UP
    (1 << 0), // TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT
    (1 << 3), // TRACK_ELEM_DIAG_25_DEG_DOWN
    (1 << 3), // TRACK_ELEM_DIAG_60_DEG_DOWN
    (1 << 3), // TRACK_ELEM_DIAG_FLAT_TO_25_DEG_DOWN
    (1 << 3), // TRACK_ELEM_DIAG_25_DEG_DOWN_TO_60_DEG_DOWN
    (1 << 3), // TRACK_ELEM_DIAG_60_DEG_DOWN_TO_25_DEG_DOWN
    (1 << 3), // TRACK_ELEM_DIAG_25_DEG_DOWN_TO_FLAT
    (1 << 0), // TRACK_ELEM_DIAG_FLAT_TO_60_DEG_UP
    (1 << 0), // TRACK_ELEM_DIAG_60_DEG_UP_TO_FLAT
    (1 << 3), // TRACK_ELEM_DIAG_FLAT_TO_60_DEG_DOWN
    (1 << 3), // TRACK_ELEM_DIAG_60_DEG_DOWN_TO_FLAT
    (1 << 0), // TRACK_ELEM_DIAG_FLAT_TO_LEFT_BANK
    (1 << 0), // TRACK_ELEM_DIAG_FLAT_TO_RIGHT_BANK
    (1 << 0), // TRACK_ELEM_DIAG_LEFT_BANK_TO_FLAT
    (1 << 0), // TRACK_ELEM_DIAG_RIGHT_BANK_TO_FLAT
    (1 << 0), // TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_UP
    (1 << 0), // TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_UP
    (1 << 0), // TRACK_ELEM_DIAG_25_DEG_UP_TO_LEFT_BANK
    (1 << 0), // TRACK_ELEM_DIAG_25_DEG_UP_TO_RIGHT_BANK
    (1 << 3), // TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_DOWN
    (1 << 3), // TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_DOWN
    (1 << 3), // TRACK_ELEM_DIAG_25_DEG_DOWN_TO_LEFT_BANK
    (1 << 3), // TRACK_ELEM_DIAG_25_DEG_DOWN_TO_RIGHT_BANK
    (1 << 0), // TRACK_ELEM_DIAG_LEFT_BANK
    (1 << 0), // TRACK_ELEM_DIAG_RIGHT_BANK
    (1 << 0), // TRACK_ELEM_LOG_FLUME_REVERSER
    (1 << 0), // TRACK_ELEM_SPINNING_TUNNEL
    (1 << 0) | (1 << 2), // TRACK_ELEM_LEFT_BARREL_ROLL_UP_TO_DOWN
    (1 << 0) | (1 << 2), // TRACK_ELEM_RIGHT_BARREL_ROLL_UP_TO_DOWN
    (1 << 0) | (1 << 2), // TRACK_ELEM_LEFT_BARREL_ROLL_DOWN_TO_UP
    (1 << 0) | (1 << 2), // TRACK_ELEM_RIGHT_BARREL_ROLL_DOWN_TO_UP
    (1 << 0) | (1 << 3), // TRACK_ELEM_LEFT_BANK_TO_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP
    (1 << 0) | (1 << 3), // TRACK_ELEM_RIGHT_BANK_TO_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP
    (1 << 0) | (1 << 3), // TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_LEFT_BANK
    (1 << 0) | (1 << 3), // TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_RIGHT_BANK
    (1 << 0), // TRACK_ELEM_POWERED_LIFT
    (1 << 0) | (1 << 6), // TRACK_ELEM_LEFT_LARGE_HALF_LOOP_UP
    (1 << 0) | (1 << 6), // TRACK_ELEM_RIGHT_LARGE_HALF_LOOP_UP
    (1 << 0) | (1 << 6), // TRACK_ELEM_RIGHT_LARGE_HALF_LOOP_DOWN
    (1 << 0) | (1 << 6), // TRACK_ELEM_LEFT_LARGE_HALF_LOOP_DOWN
    (1 << 0) | (1 << 2), // TRACK_ELEM_LEFT_FLYER_TWIST_UP
    (1 << 0) | (1 << 2), // TRACK_ELEM_RIGHT_FLYER_TWIST_UP
    (1 << 0) | (1 << 2), // TRACK_ELEM_LEFT_FLYER_TWIST_DOWN
    (1 << 0) | (1 << 2), // TRACK_ELEM_RIGHT_FLYER_TWIST_DOWN
    (1 << 0) | (1 << 3), // TRACK_ELEM_FLYER_HALF_LOOP_UP
    (1 << 0) | (1 << 3), // TRACK_ELEM_FLYER_HALF_LOOP_DOWN
    (1 << 0) | (1 << 2), // TRACK_ELEM_LEFT_FLYER_CORKSCREW_UP
    (1 << 0) | (1 << 2), // TRACK_ELEM_RIGHT_FLYER_CORKSCREW_UP
    (1 << 0) | (1 << 2), // TRACK_ELEM_LEFT_FLYER_CORKSCREW_DOWN
    (1 << 0) | (1 << 2), // TRACK_ELEM_RIGHT_FLYER_CORKSCREW_DOWN
    (1 << 0) | (1 << 3), // TRACK_ELEM_HEARTLINE_TRANSFER_UP
    (1 << 0) | (1 << 3), // TRACK_ELEM_HEARTLINE_TRANSFER_DOWN
    (1 << 0) | (1 << 5), // TRACK_ELEM_LEFT_HEARTLINE_ROLL
    (1 << 0) | (1 << 5), // TRACK_ELEM_RIGHT_HEARTLINE_ROLL
    (1 << 0) | (1 << 1), // TRACK_ELEM_MINI_GOLF_HOLE_A
    (1 << 0) | (1 << 1), // TRACK_ELEM_MINI_GOLF_HOLE_B
    (1 << 0) | (1 << 1), // TRACK_ELEM_MINI_GOLF_HOLE_C
    (1 << 0) | (1 << 2), // TRACK_ELEM_MINI_GOLF_HOLE_D
    (1 << 0) | (1 << 2), // TRACK_ELEM_MINI_GOLF_HOLE_E
    (1 << 0) | (1 << 2), // TRACK_ELEM_MULTIDIM_INVERTED_FLAT_TO_90_DEG_QUARTER_LOOP_DOWN
    (1 << 0) | (1 << 2), // TRACK_ELEM_90_DEG_TO_INVERTED_FLAT_QUARTER_LOOP_UP
    (1 << 0) | (1 << 2), // TRACK_ELEM_INVERTED_FLAT_TO_90_DEG_QUARTER_LOOP_DOWN
    (1 << 0) | (1 << 3), // TRACK_ELEM_LEFT_CURVED_LIFT_HILL
    (1 << 0) | (1 << 3), // TRACK_ELEM_RIGHT_CURVED_LIFT_HILL
    (1 << 0) | (1 << 5), // TRACK_ELEM_LEFT_REVERSER
    (1 << 0) | (1 << 5), // TRACK_ELEM_RIGHT_REVERSER
    (1 << 0) | (1 << 3), // TRACK_ELEM_AIR_THRUST_TOP_CAP
    (1 << 0), // TRACK_ELEM_AIR_THRUST_VERTICAL_DOWN
    (1 << 0) | (1 << 5), // TRACK_ELEM_AIR_THRUST_VERTICAL_DOWN_TO_LEVEL
    (1 << 0), // TRACK_ELEM_BLOCK_BRAKES
    (1 << 0) | (1 << 3), // TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP
    (1 << 0) | (1 << 3), // TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP
    (1 << 0) | (1 << 3), // TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN
    (1 << 0) | (1 << 3), // TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN
    (1 << 0) | (1 << 6), // TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP
    (1 << 0) | (1 << 6), // TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP
    (1 << 0) | (1 << 6), // TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN
    (1 << 0) | (1 << 6), // TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN
    (1 << 0), // TRACK_ELEM_25_DEG_UP_TO_LEFT_BANKED_25_DEG_UP
    (1 << 0), // TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANKED_25_DEG_UP
    (1 << 0), // TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_25_DEG_UP
    (1 << 0), // TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_25_DEG_UP
    (1 << 0), // TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANKED_25_DEG_DOWN
    (1 << 0), // TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANKED_25_DEG_DOWN
    (1 << 0), // TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN
    (1 << 0), // TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN
    (1 << 0), // TRACK_ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_UP
    (1 << 0), // TRACK_ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_UP
    (1 << 0), // TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_LEFT_BANKED_FLAT
    (1 << 0), // TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_RIGHT_BANKED_FLAT
    (1 << 0), // TRACK_ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_DOWN
    (1 << 0), // TRACK_ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN
    (1 << 0), // TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_LEFT_BANKED_FLAT
    (1 << 0), // TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_RIGHT_BANKED_FLAT
    (1 << 0), // TRACK_ELEM_FLAT_TO_LEFT_BANKED_25_DEG_UP
    (1 << 0), // TRACK_ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_UP
    (1 << 0), // TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_FLAT
    (1 << 0), // TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_FLAT
    (1 << 0), // TRACK_ELEM_FLAT_TO_LEFT_BANKED_25_DEG_DOWN
    (1 << 0), // TRACK_ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN
    (1 << 0), // TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_FLAT
    (1 << 0), // TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_FLAT
    (1 << 0), // TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_90_DEG_UP
    (1 << 0), // TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_90_DEG_UP
    (1 << 0), // TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_90_DEG_DOWN
    (1 << 0), // TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_90_DEG_DOWN
    (1 << 0) | (1 << 2), // TRACK_ELEM_MULTIDIM_90_DEG_UP_TO_INVERTED_FLAT_QUARTER_LOOP
    (1 << 0) | (1 << 2), // TRACK_ELEM_MULTIDIM_FLAT_TO_90_DEG_DOWN_QUARTER_LOOP
    (1 << 0) | (1 << 2), // 255
};

/** rct2: 0x00999A94 */
const uint8_t TrackSequenceElementAllowedWallEdges[][16] = {
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_FLAT
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_END_STATION
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_BEGIN_STATION
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_MIDDLE_STATION
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_25_DEG_UP
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_60_DEG_UP
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_FLAT_TO_25_DEG_UP
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_25_DEG_UP_TO_FLAT
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_25_DEG_DOWN
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_60_DEG_DOWN
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_FLAT_TO_25_DEG_DOWN
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_25_DEG_DOWN_TO_FLAT
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_FLAT_TO_LEFT_BANK
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_FLAT_TO_RIGHT_BANK
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_BANK_TO_FLAT
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_BANK_TO_FLAT
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_BANKED_LEFT_QUARTER_TURN_5_TILES
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_BANKED_RIGHT_QUARTER_TURN_5_TILES
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_BANK_TO_25_DEG_UP
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_BANK_TO_25_DEG_UP
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_25_DEG_UP_TO_LEFT_BANK
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANK
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_BANK_TO_25_DEG_DOWN
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_BANK_TO_25_DEG_DOWN
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANK
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANK
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_BANK
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_BANK
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_UP
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_UP
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_DOWN
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_DOWN
    { 0b1010, 0b0011, 0b1100, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_S_BEND_LEFT
    { 0b1010, 0b1001, 0b0110, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_S_BEND_RIGHT
    { 0b0010, 0b0010, 0b0011,      0,      0,      0,      0, 0b1100, 0b1000, 0b1000,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_VERTICAL_LOOP
    { 0b1000, 0b1000, 0b1001,      0,      0,      0,      0, 0b0110, 0b0010, 0b0010,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_VERTICAL_LOOP
    { 0b0010, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES
    { 0b1000, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES
    { 0b0010, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_BANK
    { 0b1000, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_BANK
    { 0b0010, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP
    { 0b1000, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP
    { 0b0010, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN
    { 0b1000, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN
    { 0b0011,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE
    { 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_TWIST_DOWN_TO_UP
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_TWIST_DOWN_TO_UP
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_TWIST_UP_TO_DOWN
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_TWIST_UP_TO_DOWN
    { 0b1010, 0b1010, 0b1011,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_HALF_LOOP_UP
    {      0, 0b1011, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_HALF_LOOP_DOWN
    { 0b1010, 0b0011, 0b0101,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_CORKSCREW_UP
    { 0b1010, 0b1001, 0b0101,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_CORKSCREW_UP
    { 0b1010, 0b0011, 0b0101,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_CORKSCREW_DOWN
    { 0b1010, 0b1001, 0b0101,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_CORKSCREW_DOWN
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_FLAT_TO_60_DEG_UP
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_60_DEG_UP_TO_FLAT
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_FLAT_TO_60_DEG_DOWN
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_60_DEG_DOWN_TO_FLAT
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_TOWER_BASE
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_TOWER_SECTION
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_FLAT_COVERED
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_25_DEG_UP_COVERED
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_60_DEG_UP_COVERED
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_FLAT_TO_25_DEG_UP_COVERED
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP_COVERED
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP_COVERED
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_25_DEG_UP_TO_FLAT_COVERED
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_25_DEG_DOWN_COVERED
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_60_DEG_DOWN_COVERED
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_FLAT_TO_25_DEG_DOWN_COVERED
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN_COVERED
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN_COVERED
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_25_DEG_DOWN_TO_FLAT_COVERED
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_COVERED
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_COVERED
    { 0b1010, 0b0011, 0b1100, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_S_BEND_LEFT_COVERED
    { 0b1010, 0b1001, 0b0110, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_S_BEND_RIGHT_COVERED
    { 0b0010, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_COVERED
    { 0b1000, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_COVERED
    { 0b0010, 0b1100, 0b0011, 0b0001, 0b0001, 0b0110, 0b1001, 0b1000,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_SMALL
    { 0b1000, 0b0110, 0b1001, 0b0001, 0b0001, 0b1100, 0b0011, 0b0010,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_SMALL
    { 0b0010, 0b1100, 0b0011, 0b0001, 0b0001, 0b0110, 0b1001, 0b1000,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_SMALL
    { 0b1000, 0b0110, 0b1001, 0b0001, 0b0001, 0b1100, 0b0011, 0b0010,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_SMALL
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001, 0b0001, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b1000,      0,      0 }, // TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_LARGE
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001, 0b0001, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0010,      0,      0 }, // TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_LARGE
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001, 0b0001, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b1000,      0,      0 }, // TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_LARGE
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001, 0b0001, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0010,      0,      0 }, // TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_LARGE
    { 0b0011,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_UP
    { 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_UP
    { 0b0011,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_DOWN
    { 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_DOWN
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_BRAKES
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_ROTATION_CONTROL_TOGGLE
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_INVERTED_90_DEG_UP_TO_FLAT_QUARTER_LOOP
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_UP
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_UP
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_DOWN
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_DOWN
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_QUARTER_HELIX_LARGE_UP
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_QUARTER_HELIX_LARGE_UP
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_QUARTER_HELIX_LARGE_DOWN
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_QUARTER_HELIX_LARGE_DOWN
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_25_DEG_UP_LEFT_BANKED
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_25_DEG_UP_RIGHT_BANKED
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_WATERFALL
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RAPIDS
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_ON_RIDE_PHOTO
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_25_DEG_DOWN_LEFT_BANKED
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_25_DEG_DOWN_RIGHT_BANKED
    { 0b1010, 0b1010, 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_WATER_SPLASH
    { 0b1010, 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_FLAT_TO_60_DEG_UP_LONG_BASE
    { 0b1010, 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_60_DEG_UP_TO_FLAT_LONG_BASE
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_WHIRLPOOL
    { 0b1010, 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_FLAT_TO_60_DEG_DOWN_LONG_BASE
    { 0b1010, 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_60_DEG_UP_TO_FLAT_LONG_BASE_122
    { 0b1010, 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_CABLE_LIFT_HILL
    { 0b1010, 0b1010, 0b1010, 0b1010, 0b1010, 0b1011,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_REVERSE_FREEFALL_SLOPE
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_REVERSE_FREEFALL_VERTICAL
    { 0b1111,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_90_DEG_UP
    { 0b1111,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_90_DEG_DOWN
    { 0b1011,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_60_DEG_UP_TO_90_DEG_UP
    { 0b1110,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_90_DEG_DOWN_TO_60_DEG_DOWN
    { 0b1011,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_90_DEG_UP_TO_60_DEG_UP
    { 0b1110,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_60_DEG_DOWN_TO_90_DEG_DOWN
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_BRAKE_FOR_DROP
    { 0b0010, 0b0010, 0b1000, 0b0011,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_EIGHTH_TO_DIAG
    { 0b1000, 0b1000, 0b0010, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_EIGHTH_TO_DIAG
    {      0, 0b1000, 0b0110, 0b0010, 0b0010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_EIGHTH_TO_ORTHOGONAL
    {      0, 0b0100, 0b1001, 0b0001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL
    { 0b0010, 0b0010, 0b1000, 0b0011,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_EIGHTH_BANK_TO_DIAG
    { 0b1000, 0b1000, 0b0010, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_DIAG
    {      0, 0b1000, 0b0110, 0b0010, 0b0010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_EIGHTH_BANK_TO_ORTHOGONAL
    {      0, 0b0100, 0b1001, 0b0001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_ORTHOGONAL
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_DIAG_FLAT
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_DIAG_25_DEG_UP
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_DIAG_60_DEG_UP
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_DIAG_FLAT_TO_25_DEG_UP
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_DIAG_25_DEG_UP_TO_60_DEG_UP
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_DIAG_60_DEG_UP_TO_25_DEG_UP
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_DIAG_25_DEG_DOWN
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_DIAG_60_DEG_DOWN
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_DIAG_FLAT_TO_25_DEG_DOWN
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_DIAG_25_DEG_DOWN_TO_60_DEG_DOWN
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_DIAG_60_DEG_DOWN_TO_25_DEG_DOWN
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_DIAG_25_DEG_DOWN_TO_FLAT
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_DIAG_FLAT_TO_60_DEG_UP
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_DIAG_60_DEG_UP_TO_FLAT
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_DIAG_FLAT_TO_60_DEG_DOWN
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_DIAG_60_DEG_DOWN_TO_FLAT
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_DIAG_FLAT_TO_LEFT_BANK
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_DIAG_FLAT_TO_RIGHT_BANK
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_DIAG_LEFT_BANK_TO_FLAT
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_DIAG_RIGHT_BANK_TO_FLAT
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_UP
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_UP
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_DIAG_25_DEG_UP_TO_LEFT_BANK
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_DIAG_25_DEG_UP_TO_RIGHT_BANK
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_DOWN
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_DOWN
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_DIAG_25_DEG_DOWN_TO_LEFT_BANK
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_DIAG_25_DEG_DOWN_TO_RIGHT_BANK
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_DIAG_LEFT_BANK
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_DIAG_RIGHT_BANK
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LOG_FLUME_REVERSER
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_SPINNING_TUNNEL
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_BARREL_ROLL_UP_TO_DOWN
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_BARREL_ROLL_UP_TO_DOWN
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_BARREL_ROLL_DOWN_TO_UP
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_BARREL_ROLL_DOWN_TO_UP
    { 0b0010, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_BANK_TO_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP
    { 0b1000, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_BANK_TO_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP
    { 0b0010, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_LEFT_BANK
    { 0b1000, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_RIGHT_BANK
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_POWERED_LIFT
    { 0b1010, 0b1010, 0b0010, 0b0011, 0b1001, 0b1000, 0b1000,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_LARGE_HALF_LOOP_UP
    { 0b1010, 0b1010, 0b1000, 0b1001, 0b0011, 0b0010, 0b0010,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_LARGE_HALF_LOOP_UP
    { 0b0010, 0b0010, 0b0011, 0b1001, 0b1000, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_LARGE_HALF_LOOP_DOWN
    { 0b1000, 0b1000, 0b1001, 0b0011, 0b0010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_LARGE_HALF_LOOP_DOWN
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_FLYER_TWIST_UP
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_FLYER_TWIST_UP
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_FLYER_TWIST_DOWN
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_FLYER_TWIST_DOWN
    { 0b1010, 0b1010, 0b1011,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_FLYER_HALF_LOOP_UP
    {      0, 0b1011, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_FLYER_HALF_LOOP_DOWN
    { 0b1010, 0b0011, 0b0101,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_FLYER_CORKSCREW_UP
    { 0b1010, 0b1001, 0b0101,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_FLYER_CORKSCREW_UP
    { 0b1010, 0b0011, 0b0101,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_FLYER_CORKSCREW_DOWN
    { 0b1010, 0b1001, 0b0101,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_FLYER_CORKSCREW_DOWN
    { 0b1010, 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_HEARTLINE_TRANSFER_UP
    { 0b1010, 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_HEARTLINE_TRANSFER_DOWN
    { 0b1010, 0b1010, 0b1010, 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_HEARTLINE_ROLL
    { 0b1010, 0b1010, 0b1010, 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_HEARTLINE_ROLL
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_MINI_GOLF_HOLE_A
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_MINI_GOLF_HOLE_B
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_MINI_GOLF_HOLE_C
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_MINI_GOLF_HOLE_D
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_MINI_GOLF_HOLE_E
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_MULTIDIM_INVERTED_FLAT_TO_90_DEG_QUARTER_LOOP_DOWN
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_90_DEG_TO_INVERTED_FLAT_QUARTER_LOOP_UP
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_INVERTED_FLAT_TO_90_DEG_QUARTER_LOOP_DOWN
    { 0b0010, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_CURVED_LIFT_HILL
    { 0b1000, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_CURVED_LIFT_HILL
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_REVERSER
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_REVERSER
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_AIR_THRUST_TOP_CAP
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_AIR_THRUST_VERTICAL_DOWN
    { 0b1010, 0b1010, 0b1010, 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_AIR_THRUST_VERTICAL_DOWN_TO_LEVEL
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_BLOCK_BRAKES
    { 0b0010, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP
    { 0b1000, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP
    { 0b0010, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN
    { 0b1000, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_25_DEG_UP_TO_LEFT_BANKED_25_DEG_UP
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANKED_25_DEG_UP
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_25_DEG_UP
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_25_DEG_UP
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANKED_25_DEG_DOWN
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANKED_25_DEG_DOWN
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_UP
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_UP
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_LEFT_BANKED_FLAT
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_RIGHT_BANKED_FLAT
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_DOWN
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_LEFT_BANKED_FLAT
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_RIGHT_BANKED_FLAT
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_FLAT_TO_LEFT_BANKED_25_DEG_UP
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_UP
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_FLAT
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_FLAT
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_FLAT_TO_LEFT_BANKED_25_DEG_DOWN
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_FLAT
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_FLAT
    { 0b1111,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_90_DEG_UP
    { 0b1111,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_90_DEG_UP
    { 0b1111,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_90_DEG_DOWN
    { 0b1111,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_90_DEG_DOWN
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_MULTIDIM_90_DEG_UP_TO_INVERTED_FLAT_QUARTER_LOOP
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // TRACK_ELEM_MULTIDIM_FLAT_TO_90_DEG_DOWN_QUARTER_LOOP
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // 255
};

/** rct2: 0x0099AA94 */
const uint8_t FlatRideTrackSequenceElementAllowedWallEdges[][16] = {
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010, 0b0011, 0b1100, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010, 0b1001, 0b0110, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b0010, 0b0010, 0b0011,      0,      0,      0,      0, 0b1100, 0b1000, 0b1000,      0,      0,      0,      0,      0,      0 },
    { 0b1000, 0b1000, 0b1001,      0,      0,      0,      0, 0b0110, 0b0010, 0b0010,      0,      0,      0,      0,      0,      0 },
    { 0b0010, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1000, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b0010, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1000, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b0010, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1000, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b0010, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1000, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b0011,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010, 0b1010, 0b1011,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b1011, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010, 0b0011, 0b0101,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010, 0b1001, 0b0101,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010, 0b0011, 0b0101,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010, 0b1001, 0b0101,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010, 0b0011, 0b1100, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010, 0b1001, 0b0110, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b0010, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1000, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b0010, 0b1100, 0b0011, 0b0001, 0b0001, 0b0110, 0b1001, 0b1000,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1000, 0b0110, 0b1001, 0b0001, 0b0001, 0b1100, 0b0011, 0b0010,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b0010, 0b1100, 0b0011, 0b0001, 0b0001, 0b0110, 0b1001, 0b1000,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1000, 0b0110, 0b1001, 0b0001, 0b0001, 0b1100, 0b0011, 0b0010,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001, 0b0001, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b1000,      0,      0 },
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001, 0b0001, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0010,      0,      0 },
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001, 0b0001, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b1000,      0,      0 },
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001, 0b0001, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0010,      0,      0 },
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // FLAT_TRACK_ELEM_1_X_4_A
    { 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b0011,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // FLAT_TRACK_ELEM_2_X_2
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // FLAT_TRACK_ELEM_4_X_4
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // FLAT_TRACK_ELEM_1_X_5
    { 0b1010, 0b1010, 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // FLAT_TRACK_ELEM_1_X_1_A
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // FLAT_TRACK_ELEM_1_X_4_B
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // FLAT_TRACK_ELEM_1_X_1_B
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // FLAT_TRACK_ELEM_1_X_4_C
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 }, // FLAT_TRACK_ELEM_3_X_3
    { 0b1010, 0b1010, 0b1010, 0b1010, 0b1010, 0b1011,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1111,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1111,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1011,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1110,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1011,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1110,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b0010, 0b0010, 0b1000, 0b0011,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1000, 0b1000, 0b0010, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b1000, 0b0110, 0b0010, 0b0010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b0100, 0b1001, 0b0001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b0010, 0b0010, 0b1000, 0b0011,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1000, 0b1000, 0b0010, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b1000, 0b0110, 0b0010, 0b0010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b0100, 0b1001, 0b0001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b0110, 0b1001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b0010, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1000, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b0010, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1000, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010, 0b1010, 0b0010, 0b0011, 0b1001, 0b1000, 0b1000,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010, 0b1010, 0b1000, 0b1001, 0b0011, 0b0010, 0b0010,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b0010, 0b0010, 0b0011, 0b1001, 0b1000, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1000, 0b1000, 0b1001, 0b0011, 0b0010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010, 0b1010, 0b1011,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0, 0b1011, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010, 0b0011, 0b0101,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010, 0b1001, 0b0101,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010, 0b0011, 0b0101,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010, 0b1001, 0b0101,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010, 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010, 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010, 0b1010, 0b1010, 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010, 0b1010, 0b1010, 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b0010, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1000, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    {      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010, 0b1010, 0b1010, 0b1010, 0b1010, 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b0010, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1000, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b0010, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1000, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b0010, 0b1100, 0b0011,      0, 0b1100, 0b0011, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1000, 0b0110, 0b1001,      0, 0b0110, 0b1001, 0b0001,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1010,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1111,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1111,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1111,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
    { 0b1111,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 },
};

/** rct2: 0x0099443C */
const uint16_t FlatTrackFlags[] = {
    /*                                                                 */   TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /*                                                                 */   0,
    /*                                                                 */   0,
    /*                                                                 */   0,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_IS_STEEP_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_IS_STEEP_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_IS_STEEP_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_LEFT,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_RIGHT,
    /*                                                                 */   TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_BANKED | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_BANKED | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,
    /*                                                                 */   0,
    /*                                                                 */   0,
    /*                                                                 */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /*                                                                 */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_LEFT,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_RIGHT,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_BANKED | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_BANKED | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_LEFT,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_RIGHT,
    /*                                                                 */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /*                                                                 */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /*                                                                 */   TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /*                                                                 */   TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_IS_STEEP_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_IS_STEEP_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /*                                                                 */   0,
    /*                                                                 */   0,
    /*                                                                 */   0,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /*                                                                 */   TRACK_ELEM_FLAG_UP,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_LEFT,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_RIGHT,
    /*                                                                 */   0,
    /*                                                                 */   0,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_LEFT,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_RIGHT,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /* FLAT_TRACK_ELEM_1_X_4_A                                         */   0,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /*                                                                 */   0,
    /*                                                                 */   0,
    /*                                                                 */   TRACK_ELEM_FLAG_ONLY_ABOVE_GROUND,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_HELIX,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_HELIX,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_HELIX,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_HELIX,
    /* FLAT_TRACK_ELEM_2_X_2                                           */   0,
    /* FLAT_TRACK_ELEM_4_X_4                                           */   0,
    /*                                                                 */   0,
    /*                                                                 */   0,
    /*                                                                 */   TRACK_ELEM_FLAG_ONLY_ABOVE_GROUND,
    /* ???                                                             */   TRACK_ELEM_FLAG_ONLY_ABOVE_GROUND,
    /* FLAT_TRACK_ELEM_1_X_5                                           */   0,
    /*                                                                 */   0,
    /* FLAT_TRACK_ELEM_1_X_1_A                                         */   0,
    /* FLAT_TRACK_ELEM_1_X_4_B                                         */   0,
    /*                                                                 */   0,
    /* FLAT_TRACK_ELEM_1_X_1_B                                         */   0,
    /* FLAT_TRACK_ELEM_1_X_4_C                                         */   0,
    /* FLAT_TRACK_ELEM_3_X_3                                           */   0,
    /*                                                                 */   TRACK_ELEM_FLAG_UP,
    /*                                                                 */   TRACK_ELEM_FLAG_UP,
    /*                                                                 */   TRACK_ELEM_FLAG_UP,
    /* ???                                                             */   TRACK_ELEM_FLAG_DOWN,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* ???                                                             */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /*                                                                 */   TRACK_ELEM_FLAG_UP,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_LEFT,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_RIGHT,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_LEFT,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_RIGHT,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_BANKED | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_BANKED | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_BANKED | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_BANKED | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_IS_STEEP_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_IS_STEEP_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_IS_STEEP_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_IS_STEEP_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_IS_STEEP_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /*                                                                 */   TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN,
    /*                                                                 */   TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /*                                                                 */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /*                                                                 */   TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /*                                                                 */   TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /*                                                                 */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /*                                                                 */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /*                                                                 */   TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /*                                                                 */   TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /*                                                                 */   0,
    /*                                                                 */   0,
    /*                                                                 */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /*                                                                 */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /*                                                                 */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION,
    /*                                                                 */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION,
    /*                                                                 */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION,
    /*                                                                 */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION,
    /*                                                                 */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION,
    /* ???                                                             */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION,
    /* ???                                                             */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION,
    /* ???                                                             */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_UP,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_UP,
    /*                                                                 */   0,
    /*                                                                 */   0,
    /*                                                                 */   0,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN,
    /*                                                                 */   0,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN,
    /*                                                                 */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN,
    /* ???                                                             */   0,
};

/** rct2: 0x0099423C */
const uint16_t TrackFlags[] = {
    /* TRACK_ELEM_FLAT                                                 */   TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_END_STATION                                          */   0,
    /* TRACK_ELEM_BEGIN_STATION                                        */   0,
    /* TRACK_ELEM_MIDDLE_STATION                                       */   0,
    /* TRACK_ELEM_25_DEG_UP                                            */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_60_DEG_UP                                            */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_IS_STEEP_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_FLAT_TO_25_DEG_UP                                    */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP                               */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_IS_STEEP_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP                               */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_IS_STEEP_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_25_DEG_UP_TO_FLAT                                    */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_25_DEG_DOWN                                          */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_60_DEG_DOWN                                          */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_FLAT_TO_25_DEG_DOWN                                  */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN                           */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN                           */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_25_DEG_DOWN_TO_FLAT                                  */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES                            */   TRACK_ELEM_FLAG_TURN_LEFT,
    /* TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES                           */   TRACK_ELEM_FLAG_TURN_RIGHT,
    /* TRACK_ELEM_FLAT_TO_LEFT_BANK                                    */   TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_FLAT_TO_RIGHT_BANK                                   */   TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_LEFT_BANK_TO_FLAT                                    */   TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_RIGHT_BANK_TO_FLAT                                   */   TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_BANKED_LEFT_QUARTER_TURN_5_TILES                     */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_BANKED | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_BANKED_RIGHT_QUARTER_TURN_5_TILES                    */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_BANKED | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_LEFT_BANK_TO_25_DEG_UP                               */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_RIGHT_BANK_TO_25_DEG_UP                              */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_25_DEG_UP_TO_LEFT_BANK                               */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANK                              */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_LEFT_BANK_TO_25_DEG_DOWN                             */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_RIGHT_BANK_TO_25_DEG_DOWN                            */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANK                             */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANK                            */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_LEFT_BANK                                            */   TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_RIGHT_BANK                                           */   TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_UP                  */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,
    /* TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_UP                 */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,
    /* TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_DOWN                */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,
    /* TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_DOWN               */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,
    /* TRACK_ELEM_S_BEND_LEFT                                          */   0,
    /* TRACK_ELEM_S_BEND_RIGHT                                         */   0,
    /* TRACK_ELEM_LEFT_VERTICAL_LOOP                                   */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_RIGHT_VERTICAL_LOOP                                  */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES                            */   TRACK_ELEM_FLAG_TURN_LEFT,
    /* TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES                           */   TRACK_ELEM_FLAG_TURN_RIGHT,
    /* TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_BANK                       */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_BANKED | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_BANK                      */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_BANKED | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP                  */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,
    /* TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP                 */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,
    /* TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN                */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,
    /* TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN               */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL | TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT,
    /* TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE                             */   TRACK_ELEM_FLAG_TURN_LEFT,
    /* TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE                            */   TRACK_ELEM_FLAG_TURN_RIGHT,
    /* TRACK_ELEM_LEFT_TWIST_DOWN_TO_UP                                */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_RIGHT_TWIST_DOWN_TO_UP                               */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_LEFT_TWIST_UP_TO_DOWN                                */   TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_RIGHT_TWIST_UP_TO_DOWN                               */   TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_HALF_LOOP_UP                                         */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_HALF_LOOP_DOWN                                       */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_LEFT_CORKSCREW_UP                                    */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_RIGHT_CORKSCREW_UP                                   */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_LEFT_CORKSCREW_DOWN                                  */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_RIGHT_CORKSCREW_DOWN                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_FLAT_TO_60_DEG_UP                                    */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_IS_STEEP_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_60_DEG_UP_TO_FLAT                                    */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_IS_STEEP_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_FLAT_TO_60_DEG_DOWN                                  */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_60_DEG_DOWN_TO_FLAT                                  */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_TOWER_BASE                                           */   0,
    /* TRACK_ELEM_TOWER_SECTION                                        */   0,
    /* TRACK_ELEM_FLAT_COVERED                                         */   0,
    /* TRACK_ELEM_25_DEG_UP_COVERED                                    */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TRACK_ELEM_60_DEG_UP_COVERED                                    */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TRACK_ELEM_FLAT_TO_25_DEG_UP_COVERED                            */   TRACK_ELEM_FLAG_UP,
    /* TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP_COVERED                       */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP_COVERED                       */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TRACK_ELEM_25_DEG_UP_TO_FLAT_COVERED                            */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TRACK_ELEM_25_DEG_DOWN_COVERED                                  */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TRACK_ELEM_60_DEG_DOWN_COVERED                                  */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TRACK_ELEM_FLAT_TO_25_DEG_DOWN_COVERED                          */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN_COVERED                   */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN_COVERED                   */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TRACK_ELEM_25_DEG_DOWN_TO_FLAT_COVERED                          */   TRACK_ELEM_FLAG_DOWN,
    /* TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_COVERED                    */   TRACK_ELEM_FLAG_TURN_LEFT,
    /* TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_COVERED                   */   TRACK_ELEM_FLAG_TURN_RIGHT,
    /* TRACK_ELEM_S_BEND_LEFT_COVERED                                  */   0,
    /* TRACK_ELEM_S_BEND_RIGHT_COVERED                                 */   0,
    /* TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_COVERED                    */   TRACK_ELEM_FLAG_TURN_LEFT,
    /* TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_COVERED                   */   TRACK_ELEM_FLAG_TURN_RIGHT,
    /* TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_SMALL                      */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_SMALL                     */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_SMALL                    */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_SMALL                   */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_LARGE                      */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_LARGE                     */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_LARGE                    */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_LARGE                   */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_UP                   */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_UP                  */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_DOWN                 */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_DOWN                */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TRACK_ELEM_BRAKES                                               */   0,
    /* TRACK_ELEM_ROTATION_CONTROL_TOGGLE                              */   0,
    /* TRACK_ELEM_INVERTED_90_DEG_UP_TO_FLAT_QUARTER_LOOP              */   TRACK_ELEM_FLAG_ONLY_ABOVE_GROUND,
    /* TRACK_ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_UP                   */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_UP                  */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_DOWN                 */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_DOWN                */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_HELIX | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_LEFT_QUARTER_HELIX_LARGE_UP                          */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_HELIX,
    /* TRACK_ELEM_RIGHT_QUARTER_HELIX_LARGE_UP                         */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_HELIX,
    /* TRACK_ELEM_LEFT_QUARTER_HELIX_LARGE_DOWN                        */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_HELIX,
    /* TRACK_ELEM_RIGHT_QUARTER_HELIX_LARGE_DOWN                       */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_HELIX,
    /* TRACK_ELEM_25_DEG_UP_LEFT_BANKED                                */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_25_DEG_UP_RIGHT_BANKED                               */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_WATERFALL                                            */   0,
    /* TRACK_ELEM_RAPIDS                                               */   0,
    /* TRACK_ELEM_ON_RIDE_PHOTO                                        */   0,
    /* TRACK_ELEM_25_DEG_DOWN_LEFT_BANKED                              */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_25_DEG_DOWN_RIGHT_BANKED                             */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_WATER_SPLASH                                         */   0,
    /* TRACK_ELEM_FLAT_TO_60_DEG_UP_LONG_BASE                          */   TRACK_ELEM_FLAG_UP,
    /* TRACK_ELEM_60_DEG_UP_TO_FLAT_LONG_BASE                          */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TRACK_ELEM_WHIRLPOOL                                            */   0,
    /* TRACK_ELEM_FLAT_TO_60_DEG_DOWN_LONG_BASE                        */   TRACK_ELEM_FLAG_DOWN,
    /* TRACK_ELEM_60_DEG_UP_TO_FLAT_LONG_BASE_122                      */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TRACK_ELEM_CABLE_LIFT_HILL                                      */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TRACK_ELEM_REVERSE_FREEFALL_SLOPE                             */ TRACK_ELEM_FLAG_UP,
    /* TRACK_ELEM_REVERSE_FREEFALL_VERTICAL                          */ TRACK_ELEM_FLAG_UP,
    /* TRACK_ELEM_90_DEG_UP                                            */   TRACK_ELEM_FLAG_UP,
    /* TRACK_ELEM_90_DEG_DOWN                                          */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_60_DEG_UP_TO_90_DEG_UP                               */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TRACK_ELEM_90_DEG_DOWN_TO_60_DEG_DOWN                           */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_90_DEG_UP_TO_60_DEG_UP                               */   TRACK_ELEM_FLAG_UP,
    /* TRACK_ELEM_60_DEG_DOWN_TO_90_DEG_DOWN                           */   TRACK_ELEM_FLAG_DOWN,
    /* TRACK_ELEM_BRAKE_FOR_DROP                                       */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TRACK_ELEM_LEFT_EIGHTH_TO_DIAG                                  */   TRACK_ELEM_FLAG_TURN_LEFT,
    /* TRACK_ELEM_RIGHT_EIGHTH_TO_DIAG                                 */   TRACK_ELEM_FLAG_TURN_RIGHT,
    /* TRACK_ELEM_LEFT_EIGHTH_TO_ORTHOGONAL                            */   TRACK_ELEM_FLAG_TURN_LEFT,
    /* TRACK_ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL                           */   TRACK_ELEM_FLAG_TURN_RIGHT,
    /* TRACK_ELEM_LEFT_EIGHTH_BANK_TO_DIAG                             */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_BANKED | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_DIAG                            */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_BANKED | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_LEFT_EIGHTH_BANK_TO_ORTHOGONAL                       */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_BANKED | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_ORTHOGONAL                      */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_BANKED | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_DIAG_FLAT                                            */   TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_DIAG_25_DEG_UP                                       */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_DIAG_60_DEG_UP                                       */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_IS_STEEP_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_DIAG_FLAT_TO_25_DEG_UP                               */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_DIAG_25_DEG_UP_TO_60_DEG_UP                          */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_IS_STEEP_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_DIAG_60_DEG_UP_TO_25_DEG_UP                          */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_IS_STEEP_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT                               */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_DIAG_25_DEG_DOWN                                     */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_DIAG_60_DEG_DOWN                                     */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_DIAG_FLAT_TO_25_DEG_DOWN                             */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_DIAG_25_DEG_DOWN_TO_60_DEG_DOWN                      */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_DIAG_60_DEG_DOWN_TO_25_DEG_DOWN                      */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_DIAG_25_DEG_DOWN_TO_FLAT                             */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_DIAG_FLAT_TO_60_DEG_UP                               */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_IS_STEEP_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_DIAG_60_DEG_UP_TO_FLAT                               */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_IS_STEEP_UP | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_DIAG_FLAT_TO_60_DEG_DOWN                             */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_DIAG_60_DEG_DOWN_TO_FLAT                             */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_ALLOW_LIFT_HILL,
    /* TRACK_ELEM_DIAG_FLAT_TO_LEFT_BANK                               */   TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_DIAG_FLAT_TO_RIGHT_BANK                              */   TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_DIAG_LEFT_BANK_TO_FLAT                               */   TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_DIAG_RIGHT_BANK_TO_FLAT                              */   TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_UP                          */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_UP                         */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_DIAG_25_DEG_UP_TO_LEFT_BANK                          */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_DIAG_25_DEG_UP_TO_RIGHT_BANK                         */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_DOWN                        */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_DOWN                       */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_DIAG_25_DEG_DOWN_TO_LEFT_BANK                        */   TRACK_ELEM_FLAG_DOWN,
    /* TRACK_ELEM_DIAG_25_DEG_DOWN_TO_RIGHT_BANK                       */   TRACK_ELEM_FLAG_DOWN,
    /* TRACK_ELEM_DIAG_LEFT_BANK                                       */   TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_DIAG_RIGHT_BANK                                      */   TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_LOG_FLUME_REVERSER                                   */   TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_SPINNING_TUNNEL                                      */   TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_LEFT_BARREL_ROLL_UP_TO_DOWN                          */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_RIGHT_BARREL_ROLL_UP_TO_DOWN                         */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_LEFT_BARREL_ROLL_DOWN_TO_UP                          */   TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_RIGHT_BARREL_ROLL_DOWN_TO_UP                         */   TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_LEFT_BANK_TO_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP     */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_RIGHT_BANK_TO_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP   */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_LEFT_BANK   */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN_TO_RIGHT_BANK */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_POWERED_LIFT                                         */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT,
    /* TRACK_ELEM_LEFT_LARGE_HALF_LOOP_UP                              */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_RIGHT_LARGE_HALF_LOOP_UP                             */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_RIGHT_LARGE_HALF_LOOP_DOWN                           */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_LEFT_LARGE_HALF_LOOP_DOWN                            */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_LEFT_FLYER_TWIST_UP                                  */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_RIGHT_FLYER_TWIST_UP                                 */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_LEFT_FLYER_TWIST_DOWN                                */   TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_RIGHT_FLYER_TWIST_DOWN                               */   TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_FLYER_HALF_LOOP_UP                                   */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_FLYER_HALF_LOOP_DOWN                                 */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_LEFT_FLYER_CORKSCREW_UP                              */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_RIGHT_FLYER_CORKSCREW_UP                             */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_LEFT_FLYER_CORKSCREW_DOWN                            */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_RIGHT_FLYER_CORKSCREW_DOWN                           */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_HEARTLINE_TRANSFER_UP                                */   0,
    /* TRACK_ELEM_HEARTLINE_TRANSFER_DOWN                              */   0,
    /* TRACK_ELEM_LEFT_HEARTLINE_ROLL                                  */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_RIGHT_HEARTLINE_ROLL                                 */   TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_MINI_GOLF_HOLE_A                                     */   TRACK_ELEM_FLAG_IS_GOLF_HOLE,
    /* TRACK_ELEM_MINI_GOLF_HOLE_B                                     */   TRACK_ELEM_FLAG_IS_GOLF_HOLE,
    /* TRACK_ELEM_MINI_GOLF_HOLE_C                                     */   TRACK_ELEM_FLAG_IS_GOLF_HOLE,
    /* TRACK_ELEM_MINI_GOLF_HOLE_D                                     */   TRACK_ELEM_FLAG_IS_GOLF_HOLE,
    /* TRACK_ELEM_MINI_GOLF_HOLE_E                                     */   TRACK_ELEM_FLAG_IS_GOLF_HOLE,
    /* TRACK_ELEM_MULTIDIM_INVERTED_FLAT_TO_90_DEG_QUARTER_LOOP_DOWN   */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_90_DEG_TO_INVERTED_FLAT_QUARTER_LOOP_UP              */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_INVERTED_FLAT_TO_90_DEG_QUARTER_LOOP_DOWN            */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_LEFT_CURVED_LIFT_HILL                                */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_UP,
    /* TRACK_ELEM_RIGHT_CURVED_LIFT_HILL                               */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_UP,
    /* TRACK_ELEM_LEFT_REVERSER                                        */   0,
    /* TRACK_ELEM_RIGHT_REVERSER                                       */   0,
    /* TRACK_ELEM_AIR_THRUST_TOP_CAP                                   */   0,
    /* TRACK_ELEM_AIR_THRUST_VERTICAL_DOWN                             */   TRACK_ELEM_FLAG_DOWN,
    /* TRACK_ELEM_AIR_THRUST_VERTICAL_DOWN_TO_LEVEL                    */   TRACK_ELEM_FLAG_DOWN,
    /* TRACK_ELEM_BLOCK_BRAKES                                         */   0,
    /* TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP            */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP           */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN          */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN         */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP            */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP           */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN          */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN         */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_25_DEG_UP_TO_LEFT_BANKED_25_DEG_UP                   */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANKED_25_DEG_UP                  */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_25_DEG_UP                   */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_25_DEG_UP                  */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANKED_25_DEG_DOWN               */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANKED_25_DEG_DOWN              */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN               */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN              */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_UP            */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_UP          */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_LEFT_BANKED_FLAT            */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_RIGHT_BANKED_FLAT          */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_DOWN          */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN        */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_LEFT_BANKED_FLAT          */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_RIGHT_BANKED_FLAT        */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_FLAT_TO_LEFT_BANKED_25_DEG_UP                        */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_UP                       */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_FLAT                        */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_FLAT                       */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_FLAT_TO_LEFT_BANKED_25_DEG_DOWN                      */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN                     */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_FLAT                      */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_FLAT                     */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_BANKED,
    /* TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_90_DEG_UP                   */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP,
    /* TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_90_DEG_UP                  */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_UP,
    /* TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_90_DEG_DOWN                 */   TRACK_ELEM_FLAG_TURN_LEFT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN,
    /* TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_90_DEG_DOWN                */   TRACK_ELEM_FLAG_TURN_RIGHT | TRACK_ELEM_FLAG_TURN_SLOPED | TRACK_ELEM_FLAG_DOWN,
    /* TRACK_ELEM_MULTIDIM_90_DEG_UP_TO_INVERTED_FLAT_QUARTER_LOOP     */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /* TRACK_ELEM_MULTIDIM_FLAT_TO_90_DEG_DOWN_QUARTER_LOOP            */   TRACK_ELEM_FLAG_DOWN | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
    /*                                                                 */   TRACK_ELEM_FLAG_UP | TRACK_ELEM_FLAG_NORMAL_TO_INVERSION | TRACK_ELEM_FLAG_INVERSION_TO_NORMAL,
};
// clang-format on
