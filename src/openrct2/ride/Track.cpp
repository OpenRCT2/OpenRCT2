/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Track.h"

#include "../Cheats.h"
#include "../Game.h"
#include "../audio/audio.h"
#include "../config/Config.h"
#include "../interface/Viewport.h"
#include "../localisation/Localisation.h"
#include "../management/Finance.h"
#include "../network/network.h"
#include "../platform/platform.h"
#include "../rct1/RCT1.h"
#include "../util/SawyerCoding.h"
#include "../util/Util.h"
#include "../world/Footpath.h"
#include "../world/Map.h"
#include "../world/MapAnimation.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/Surface.h"
#include "Ride.h"
#include "RideData.h"
#include "RideRatings.h"
#include "Station.h"
#include "TrackData.h"
#include "TrackDesign.h"

/**  rct2: 0x00997C9D */
// clang-format off
const rct_trackdefinition TrackDefinitions[256] =
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
    { TRACK_ROTATION_CONTROL_TOGGLE,TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_ROTATION_CONTROL_TOGGLE
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_INVERTED_90_DEG_UP_TO_FLAT_QUARTER_LOOP
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
    {TRACK_HALF_LOOP_UNINVERTED,    TRACK_SLOPE_NONE,    TRACK_SLOPE_UP_25, TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        64                 },  // ELEM_FLYER_HALF_LOOP_UP
    {TRACK_HALF_LOOP_INVERTED,      TRACK_SLOPE_DOWN_25, TRACK_SLOPE_NONE,  TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        -64                },  // ELEM_FLYER_HALF_LOOP_DOWN
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
    {TRACK_QUARTER_LOOP_INVERTED,   TRACK_SLOPE_DOWN_90, TRACK_SLOPE_NONE,  TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  },  // ELEM_MULTIDIM_INVERTED_FLAT_TO_90_DEG_QUARTER_LOOP_DOWN
    {TRACK_QUARTER_LOOP,            TRACK_SLOPE_NONE,    TRACK_SLOPE_UP_90, TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  },  // ELEM_90_DEG_TO_INVERTED_FLAT_QUARTER_LOOP_UP
    {TRACK_QUARTER_LOOP,            TRACK_SLOPE_DOWN_90, TRACK_SLOPE_NONE,  TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, 0                  },  // ELEM_INVERTED_FLAT_TO_90_DEG_QUARTER_LOOP_DOWN
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
    { TRACK_QUARTER_LOOP_UNINVERTED,TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_90,          TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  },  // ELEM_MULTIDIM_90_DEG_UP_TO_INVERTED_FLAT_QUARTER_LOOP
    { TRACK_QUARTER_LOOP_UNINVERTED,TRACK_SLOPE_DOWN_90,        TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  },  // ELEM_MULTIDIM_FLAT_TO_90_DEG_DOWN_QUARTER_LOOP
    { TRACK_QUARTER_LOOP_INVERTED,  TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_90,          TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  }, // 255
};

/**　rct2: 0x0099849D */
const rct_trackdefinition FlatRideTrackDefinitions[256] =
{
    // TYPE                         VANGLE END                  VANGLE START                BANK END                BANK START              PREVIEW Z OFFSET
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
    { TRACK_VERTICAL_LOOP,          TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        -48                         },  // 40
    { TRACK_VERTICAL_LOOP,          TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        -48                         },  // 41
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
    { TRACK_HALF_LOOP,              TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        64                          },  // 56
    { TRACK_HALF_LOOP,              TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, -64                         },  // 57
    { TRACK_CORKSCREW,              TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                           },  // 58
    { TRACK_CORKSCREW,              TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                           },  // 59
    { TRACK_CORKSCREW,              TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, -32                         },  // 60
    { TRACK_CORKSCREW,              TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, -32                         },  // 61
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
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        -40                         },  // FLAT_TRACK_ELEM_1_X_4_C
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // FLAT_TRACK_ELEM_3_X_3
    { TRACK_REVERSE_FREEFALL,       TRACK_SLOPE_UP_90,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 124
    { TRACK_REVERSE_FREEFALL,       TRACK_SLOPE_UP_90,          TRACK_SLOPE_UP_90,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 125
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
    {TRACK_BARREL_ROLL,          TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,    TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, 0                           },  // 177
    {TRACK_FLAT,                 TRACK_SLOPE_UP_25,   TRACK_SLOPE_NONE,    TRACK_BANK_NONE,        TRACK_BANK_LEFT,        0                           },  // 178
    {TRACK_FLAT,                 TRACK_SLOPE_UP_25,   TRACK_SLOPE_NONE,    TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       0                           },  // 179
    {TRACK_FLAT,                 TRACK_SLOPE_NONE,    TRACK_SLOPE_DOWN_25, TRACK_BANK_LEFT,        TRACK_BANK_NONE,        0                           },  // 180
    {TRACK_FLAT,                 TRACK_SLOPE_NONE,    TRACK_SLOPE_DOWN_25, TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        0                           },  // 181
    {TRACK_POWERED_LIFT,         TRACK_SLOPE_UP_25,   TRACK_SLOPE_UP_25,   TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 182
    {TRACK_HALF_LOOP_LARGE,      TRACK_SLOPE_NONE,    TRACK_SLOPE_UP_25,   TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        64                          },  // 183
    {TRACK_HALF_LOOP_LARGE,      TRACK_SLOPE_NONE,    TRACK_SLOPE_UP_25,   TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        64                          },  // 184
    {TRACK_HALF_LOOP_LARGE,      TRACK_SLOPE_DOWN_25, TRACK_SLOPE_NONE,    TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, -64                         },  // 185
    {TRACK_HALF_LOOP_LARGE,      TRACK_SLOPE_DOWN_25, TRACK_SLOPE_NONE,    TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, -64                         },  // 186
    {TRACK_CORKSCREW_UNINVERTED, TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE, 0                           },  // 187
    {TRACK_CORKSCREW_UNINVERTED, TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE, 0                           },  // 188
    {TRACK_CORKSCREW_INVERTED,   TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE, 0                           },  // 189
    {TRACK_CORKSCREW_INVERTED,   TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE, 0                           },  // 190
    {TRACK_CORKSCREW_UNINVERTED, TRACK_SLOPE_NONE,    TRACK_SLOPE_UP_25, TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE, 64                          },  // 191
    {TRACK_CORKSCREW_INVERTED,   TRACK_SLOPE_DOWN_25, TRACK_SLOPE_NONE,  TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE, -64                         },  // 192
    {TRACK_CORKSCREW_UNINVERTED, TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE, 0                           },  // 193
    {TRACK_CORKSCREW_UNINVERTED, TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE, 0                           },  // 194
    {TRACK_CORKSCREW_INVERTED,   TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE, -32                         },  // 195
    {TRACK_CORKSCREW_INVERTED,   TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE, -32                         },  // 196
    {TRACK_HEARTLINE_TRANSFER,   TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_NONE,        TRACK_BANK_NONE, 0                           },  // 197
    {TRACK_HEARTLINE_TRANSFER,   TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_NONE,        TRACK_BANK_NONE, 0                           },  // 198
    {TRACK_HEARTLINE_ROLL,       TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_NONE,        TRACK_BANK_NONE, 0                           },  // 199
    {TRACK_HEARTLINE_ROLL,       TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_NONE,        TRACK_BANK_NONE, 0                           },  // 200
    {TRACK_MINI_GOLF_HOLE,       TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_NONE,        TRACK_BANK_NONE, 0                           },  // 201
    {TRACK_MINI_GOLF_HOLE,       TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_NONE,        TRACK_BANK_NONE, 0                           },  // 202
    {TRACK_MINI_GOLF_HOLE,       TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_NONE,        TRACK_BANK_NONE, 0                           },  // 203
    {TRACK_MINI_GOLF_HOLE,       TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_NONE,        TRACK_BANK_NONE, 0                           },  // 204
    {TRACK_MINI_GOLF_HOLE,       TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_NONE,        TRACK_BANK_NONE, 0                           },  // 205
    {TRACK_FLAT,                 TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_NONE,        TRACK_BANK_NONE, 0                           },  // 206
    {TRACK_FLAT,                 TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_NONE,        TRACK_BANK_NONE, 0                           },  // 207
    {TRACK_FLAT,                 TRACK_SLOPE_NONE,    TRACK_SLOPE_NONE,  TRACK_BANK_NONE,        TRACK_BANK_NONE, 0                           },  // 208
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
    { TRACK_QUARTER_LOOP_UNINVERTED,TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_90,          TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                           },  // 253
    { TRACK_QUARTER_LOOP_UNINVERTED,TRACK_SLOPE_DOWN_90,        TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                           },  // 254
};
// clang-format on

PitchAndRoll TrackPitchAndRollStart(track_type_t trackType)
{
    return { TrackDefinitions[trackType].vangle_start, TrackDefinitions[trackType].bank_start };
}

PitchAndRoll TrackPitchAndRollEnd(track_type_t trackType)
{
    return { TrackDefinitions[trackType].vangle_end, TrackDefinitions[trackType].bank_end };
}

/**
 * Helper method to determine if a connects to b by its bank and angle, not location.
 */
int32_t track_is_connected_by_shape(TileElement* a, TileElement* b)
{
    int32_t trackType, aBank, aAngle, bBank, bAngle;

    trackType = a->AsTrack()->GetTrackType();
    aBank = TrackDefinitions[trackType].bank_end;
    aAngle = TrackDefinitions[trackType].vangle_end;
    aBank = track_get_actual_bank(a, aBank);

    trackType = b->AsTrack()->GetTrackType();
    bBank = TrackDefinitions[trackType].bank_start;
    bAngle = TrackDefinitions[trackType].vangle_start;
    bBank = track_get_actual_bank(b, bBank);

    return aBank == bBank && aAngle == bAngle;
}

const rct_preview_track* get_track_def_from_ride(Ride* ride, int32_t trackType)
{
    return ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE) ? FlatRideTrackBlocks[trackType] : TrackBlocks[trackType];
}

const rct_track_coordinates* get_track_coord_from_ride(Ride* ride, int32_t trackType)
{
    return ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE) ? &FlatTrackCoordinates[trackType]
                                                                    : &TrackCoordinates[trackType];
}

const rct_preview_track* get_track_def_from_ride_index(ride_id_t rideIndex, int32_t trackType)
{
    return get_track_def_from_ride(get_ride(rideIndex), trackType);
}

static TileElement* find_station_element(const CoordsXYZD& loc, ride_id_t rideIndex)
{
    TileElement* tileElement = map_get_first_element_at(loc);
    if (tileElement == nullptr)
        return nullptr;
    do
    {
        if (loc.z != tileElement->GetBaseZ())
            continue;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;
        if (tileElement->GetDirection() != loc.direction)
            continue;
        if (tileElement->AsTrack()->GetRideIndex() != rideIndex)
            continue;
        if (!tileElement->AsTrack()->IsStation())
            continue;

        return tileElement;
    } while (!(tileElement++)->IsLastForTile());
    return nullptr;
}

static void ride_remove_station(Ride* ride, const CoordsXYZ& location)
{
    for (int32_t i = 0; i < MAX_STATIONS; i++)
    {
        auto stationStart = ride->stations[i].GetStart();
        if (stationStart == location)
        {
            ride->stations[i].Start.setNull();
            ride->num_stations--;
            break;
        }
    }
}

/**
 *
 *  rct2: 0x006C4D89
 */
bool track_add_station_element(CoordsXYZD loc, ride_id_t rideIndex, int32_t flags, bool fromTrackDesign)
{
    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
        return false;

    CoordsXY stationBackLoc = loc;
    CoordsXY stationFrontLoc = loc;
    int32_t stationLength = 1;

    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION))
    {
        if (ride->num_stations >= MAX_STATIONS)
        {
            gGameCommandErrorText = STR_NO_MORE_STATIONS_ALLOWED_ON_THIS_RIDE;
            return false;
        }
        if (flags & GAME_COMMAND_FLAG_APPLY)
        {
            auto stationIndex = ride_get_first_empty_station_start(ride);
            assert(stationIndex != STATION_INDEX_NULL);

            ride->stations[stationIndex].Start.x = loc.x;
            ride->stations[stationIndex].Start.y = loc.y;
            ride->stations[stationIndex].Height = loc.z / COORDS_Z_STEP;
            ride->stations[stationIndex].Depart = 1;
            ride->stations[stationIndex].Length = 0;
            ride->num_stations++;
        }
        return true;
    }

    TileElement* stationElement;

    // Search backwards for more station
    loc = { stationBackLoc, loc.z, loc.direction };
    do
    {
        loc -= CoordsDirectionDelta[loc.direction];

        stationElement = find_station_element(loc, rideIndex);
        if (stationElement != nullptr)
        {
            if (stationElement->AsTrack()->GetTrackType() == TrackElemType::EndStation)
            {
                if (flags & GAME_COMMAND_FLAG_APPLY)
                {
                    ride_remove_station(ride, loc);
                }
            }

            stationBackLoc = loc;
            stationLength++;
        }
    } while (stationElement != nullptr);

    // Search forwards for more station
    loc = { stationFrontLoc, loc.z, loc.direction };
    do
    {
        loc += CoordsDirectionDelta[loc.direction];

        stationElement = find_station_element(loc, rideIndex);
        if (stationElement != nullptr)
        {
            if (stationElement->AsTrack()->GetTrackType() == TrackElemType::EndStation)
            {
                if (flags & GAME_COMMAND_FLAG_APPLY)
                {
                    ride_remove_station(ride, loc);
                }
            }

            stationFrontLoc = loc;
            stationLength++;
        }
    } while (stationElement != nullptr);

    // When attempting to place a track design, it sometimes happens that the front and back of station 0 are built,
    // but the middle is not. Allow this, so the track place function can actually finish building all 4 stations.
    // This _might_ cause issues if the track designs is bugged and actually has 5.
    if (stationBackLoc == stationFrontLoc && ride->num_stations >= MAX_STATIONS && !fromTrackDesign)
    {
        gGameCommandErrorText = STR_NO_MORE_STATIONS_ALLOWED_ON_THIS_RIDE;
        return false;
    }

    if (stationLength > MAX_STATION_PLATFORM_LENGTH)
    {
        gGameCommandErrorText = STR_STATION_PLATFORM_TOO_LONG;
        return false;
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        loc = { stationFrontLoc, loc.z, loc.direction };

        bool finaliseStationDone;
        do
        {
            finaliseStationDone = true;

            stationElement = find_station_element(loc, rideIndex);
            if (stationElement != nullptr)
            {
                int32_t targetTrackType;
                if (stationFrontLoc == loc)
                {
                    auto stationIndex = ride_get_first_empty_station_start(ride);
                    if (stationIndex == STATION_INDEX_NULL)
                    {
                        log_verbose("No empty station starts, not updating metadata! This can happen with hacked rides.");
                    }
                    else
                    {
                        ride->stations[stationIndex].Start = loc;
                        ride->stations[stationIndex].Height = loc.z / COORDS_Z_STEP;
                        ride->stations[stationIndex].Depart = 1;
                        ride->stations[stationIndex].Length = stationLength;
                        ride->num_stations++;
                    }

                    targetTrackType = TrackElemType::EndStation;
                }
                else if (stationBackLoc == loc)
                {
                    targetTrackType = TrackElemType::BeginStation;
                }
                else
                {
                    targetTrackType = TrackElemType::MiddleStation;
                }
                stationElement->AsTrack()->SetTrackType(targetTrackType);

                map_invalidate_element(loc, stationElement);

                if (stationBackLoc != loc)
                {
                    loc -= CoordsDirectionDelta[loc.direction];
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
bool track_remove_station_element(const CoordsXYZD& loc, ride_id_t rideIndex, int32_t flags)
{
    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
        return false;

    CoordsXYZD removeLoc = loc;
    CoordsXYZD stationBackLoc = loc;
    CoordsXYZD stationFrontLoc = loc;
    int32_t stationLength = 0;
    int32_t byte_F441D1 = -1;

    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION))
    {
        TileElement* tileElement = map_get_track_element_at_with_direction_from_ride(loc, rideIndex);
        if (tileElement != nullptr)
        {
            if (flags & GAME_COMMAND_FLAG_APPLY)
            {
                ride_remove_station(ride, loc);
            }
        }
        return true;
    }

    TileElement* stationElement;

    // Search backwards for more station
    CoordsXYZD currentLoc = stationBackLoc;
    while ((stationElement = find_station_element(currentLoc, rideIndex)) != nullptr)
    {
        if (stationElement->AsTrack()->GetTrackType() == TrackElemType::EndStation)
        {
            if (flags & GAME_COMMAND_FLAG_APPLY)
            {
                ride_remove_station(ride, currentLoc);
            }
        }

        stationBackLoc = currentLoc;
        byte_F441D1++;

        currentLoc -= CoordsDirectionDelta[currentLoc.direction];
    }

    // Search forwards for more station
    currentLoc = stationFrontLoc;
    do
    {
        currentLoc += CoordsDirectionDelta[currentLoc.direction];

        stationElement = find_station_element(currentLoc, rideIndex);
        if (stationElement != nullptr)
        {
            if (stationElement->AsTrack()->GetTrackType() == TrackElemType::EndStation)
            {
                if (flags & GAME_COMMAND_FLAG_APPLY)
                {
                    ride_remove_station(ride, currentLoc);
                }
            }
            stationFrontLoc = currentLoc;
            stationLength++;
        }
    } while (stationElement != nullptr);

    if (!(flags & GAME_COMMAND_FLAG_APPLY))
    {
        if ((removeLoc != stationBackLoc) && (removeLoc != stationFrontLoc) && ride->num_stations >= MAX_STATIONS)
        {
            gGameCommandErrorText = STR_NO_MORE_STATIONS_ALLOWED_ON_THIS_RIDE;
            return false;
        }
        else
        {
            return true;
        }
    }

    currentLoc = stationFrontLoc;
    bool finaliseStationDone;
    do
    {
        finaliseStationDone = true;

        if (currentLoc != removeLoc)
        {
            stationElement = find_station_element(currentLoc, rideIndex);
            if (stationElement != nullptr)
            {
                int32_t targetTrackType;
                if ((currentLoc == stationFrontLoc) || (currentLoc + CoordsDirectionDelta[currentLoc.direction] == removeLoc))
                {
                    auto stationIndex = ride_get_first_empty_station_start(ride);
                    if (stationIndex == STATION_INDEX_NULL)
                    {
                        log_verbose("No empty station starts, not updating metadata! This can happen with hacked rides.");
                    }
                    else
                    {
                        ride->stations[stationIndex].Start = currentLoc;
                        ride->stations[stationIndex].Height = currentLoc.z / COORDS_Z_STEP;
                        ride->stations[stationIndex].Depart = 1;
                        ride->stations[stationIndex].Length = stationLength != 0 ? stationLength : byte_F441D1;
                        ride->num_stations++;
                    }

                    stationLength = 0;
                    targetTrackType = TrackElemType::EndStation;
                }
                else
                {
                    if (currentLoc - CoordsDirectionDelta[currentLoc.direction] == removeLoc)
                    {
                        targetTrackType = TrackElemType::BeginStation;
                    }
                    else
                    {
                        if (currentLoc == stationBackLoc)
                        {
                            targetTrackType = TrackElemType::BeginStation;
                        }
                        else
                        {
                            targetTrackType = TrackElemType::MiddleStation;
                        }
                    }
                }
                stationElement->AsTrack()->SetTrackType(targetTrackType);

                map_invalidate_element(currentLoc, stationElement);
            }
        }

        if (currentLoc != stationBackLoc)
        {
            currentLoc -= CoordsDirectionDelta[currentLoc.direction];
            finaliseStationDone = false;
        }
    } while (!finaliseStationDone);

    return true;
}

void track_circuit_iterator_begin(track_circuit_iterator* it, CoordsXYE first)
{
    it->last = first;
    it->first = nullptr;
    it->firstIteration = true;
    it->looped = false;
}

bool track_circuit_iterator_previous(track_circuit_iterator* it)
{
    track_begin_end trackBeginEnd;

    if (it->first == nullptr)
    {
        if (!track_block_get_previous({ it->last.x, it->last.y, it->last.element }, &trackBeginEnd))
            return false;

        it->current.x = trackBeginEnd.begin_x;
        it->current.y = trackBeginEnd.begin_y;
        it->current.element = trackBeginEnd.begin_element;
        it->currentZ = trackBeginEnd.begin_z;
        it->currentDirection = trackBeginEnd.begin_direction;

        it->first = it->current.element;
        return true;
    }
    else
    {
        if (!it->firstIteration && it->first == it->current.element)
        {
            it->looped = true;
            return false;
        }

        it->firstIteration = false;
        it->last = it->current;

        if (track_block_get_previous({ it->last.x, it->last.y, it->last.element }, &trackBeginEnd))
        {
            it->current.x = trackBeginEnd.end_x;
            it->current.y = trackBeginEnd.end_y;
            it->current.element = trackBeginEnd.begin_element;
            it->currentZ = trackBeginEnd.begin_z;
            it->currentDirection = trackBeginEnd.begin_direction;
            return true;
        }
        else
        {
            return false;
        }
    }
}

bool track_circuit_iterator_next(track_circuit_iterator* it)
{
    if (it->first == nullptr)
    {
        if (!track_block_get_next(&it->last, &it->current, &it->currentZ, &it->currentDirection))
            return false;

        it->first = it->current.element;
        return true;
    }
    else
    {
        if (!it->firstIteration && it->first == it->current.element)
        {
            it->looped = true;
            return false;
        }

        it->firstIteration = false;
        it->last = it->current;
        return track_block_get_next(&it->last, &it->current, &it->currentZ, &it->currentDirection);
    }
}

bool track_circuit_iterators_match(const track_circuit_iterator* firstIt, const track_circuit_iterator* secondIt)
{
    return (
        firstIt->currentZ == secondIt->currentZ && firstIt->currentDirection == secondIt->currentDirection
        && firstIt->current.x == secondIt->current.x && firstIt->current.y == secondIt->current.y);
}

void track_get_back(CoordsXYE* input, CoordsXYE* output)
{
    CoordsXYE lastTrack;
    track_begin_end currentTrack;
    bool result;

    lastTrack = *input;
    do
    {
        result = track_block_get_previous(lastTrack, &currentTrack);
        if (result)
        {
            lastTrack.x = currentTrack.begin_x;
            lastTrack.y = currentTrack.begin_y;
            lastTrack.element = currentTrack.begin_element;
        }
    } while (result);
    *output = lastTrack;
}

void track_get_front(CoordsXYE* input, CoordsXYE* output)
{
    CoordsXYE lastTrack, currentTrack;
    int32_t z, direction;
    bool result;

    lastTrack = *input;
    do
    {
        result = track_block_get_next(&lastTrack, &currentTrack, &z, &direction);
        if (result)
        {
            lastTrack = currentTrack;
        }
    } while (result);
    *output = lastTrack;
}

bool TrackElement::HasChain() const
{
    return Flags2 & TRACK_ELEMENT_FLAGS2_CHAIN_LIFT;
}

void TrackElement::SetHasChain(bool on)
{
    if (on)
    {
        Flags2 |= TRACK_ELEMENT_FLAGS2_CHAIN_LIFT;
    }
    else
    {
        Flags2 &= ~TRACK_ELEMENT_FLAGS2_CHAIN_LIFT;
    }
}

/**
 * Checks if a track element is recognised as the beginning of a block.
 * A beginning of a block can be the end of a station, the end of a lift hill,
 * or a block brake.
 */
bool TrackElement::IsBlockStart() const
{
    switch (GetTrackType())
    {
        case TrackElemType::EndStation:
        case TrackElemType::CableLiftHill:
        case TrackElemType::BlockBrakes:
            return true;
        case TrackElemType::Up25ToFlat:
        case TrackElemType::Up60ToFlat:
        case TrackElemType::DiagUp25ToFlat:
        case TrackElemType::DiagUp60ToFlat:
            if (HasChain())
            {
                return true;
            }
            break;
    }
    return false;
}

roll_type_t track_get_actual_bank(TileElement* tileElement, roll_type_t bank)
{
    auto ride = get_ride(tileElement->AsTrack()->GetRideIndex());
    if (ride != nullptr)
    {
        bool isInverted = tileElement->AsTrack()->IsInverted();
        return track_get_actual_bank_2(ride->type, isInverted, bank);
    }
    return bank;
}

roll_type_t track_get_actual_bank_2(int32_t rideType, bool isInverted, roll_type_t bank)
{
    if (RideTypeDescriptors[rideType].Flags & RIDE_TYPE_FLAG_HAS_ALTERNATIVE_TRACK_TYPE)
    {
        if (isInverted)
        {
            if (bank == TRACK_BANK_NONE)
            {
                bank = TRACK_BANK_UPSIDE_DOWN;
            }
            else if (bank == TRACK_BANK_UPSIDE_DOWN)
            {
                bank = TRACK_BANK_NONE;
            }
        }
    }
    return bank;
}

roll_type_t track_get_actual_bank_3(bool useInvertedSprites, TileElement* tileElement)
{
    auto trackType = tileElement->AsTrack()->GetTrackType();
    auto bankStart = TrackDefinitions[trackType].bank_start;
    auto ride = get_ride(tileElement->AsTrack()->GetRideIndex());
    if (ride == nullptr)
        return bankStart;

    bool isInverted = useInvertedSprites ^ tileElement->AsTrack()->IsInverted();
    return track_get_actual_bank_2(ride->type, isInverted, bankStart);
}

bool TrackElement::IsStation() const
{
    return track_type_is_station(GetTrackType());
}

bool track_type_is_station(track_type_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return true;
        default:
            return false;
    }
}

bool track_element_is_covered(track_type_t trackElementType)
{
    switch (trackElementType)
    {
        case TrackElemType::FlatCovered:
        case TrackElemType::Up25Covered:
        case TrackElemType::Up60Covered:
        case TrackElemType::FlatToUp25Covered:
        case TrackElemType::Up25ToUp60Covered:
        case TrackElemType::Up60ToUp25Covered:
        case TrackElemType::Up25ToFlatCovered:
        case TrackElemType::Down25Covered:
        case TrackElemType::Down60Covered:
        case TrackElemType::FlatToDown25Covered:
        case TrackElemType::Down25ToDown60Covered:
        case TrackElemType::Down60ToDown25Covered:
        case TrackElemType::Down25ToFlatCovered:
        case TrackElemType::LeftQuarterTurn5TilesCovered:
        case TrackElemType::RightQuarterTurn5TilesCovered:
        case TrackElemType::SBendLeftCovered:
        case TrackElemType::SBendRightCovered:
        case TrackElemType::LeftQuarterTurn3TilesCovered:
        case TrackElemType::RightQuarterTurn3TilesCovered:
            return true;
        default:
            return false;
    }
}

bool TrackTypeIsBooster(uint8_t rideType, track_type_t trackType)
{
    // Boosters share their ID with the Spinning Control track.
    return rideType != RIDE_TYPE_SPINNING_WILD_MOUSE && trackType == TrackElemType::Booster;
}

bool TrackTypeHasSpeedSetting(track_type_t trackType)
{
    // This does not check if the element is really a Spinning Control track instead of a booster,
    // but this does not cause problems.
    return trackType == TrackElemType::Brakes || trackType == TrackElemType::Booster;
}

uint8_t TrackElement::GetSeatRotation() const
{
    return ColourScheme >> 4;
}

void TrackElement::SetSeatRotation(uint8_t newSeatRotation)
{
    ColourScheme &= ~TRACK_ELEMENT_COLOUR_SEAT_ROTATION_MASK;
    ColourScheme |= (newSeatRotation << 4);
}

bool TrackElement::IsTakingPhoto() const
{
    return OnridePhotoBits != 0;
}

void TrackElement::SetPhotoTimeout()
{
    OnridePhotoBits = 3;
}

void TrackElement::SetPhotoTimeout(uint8_t value)
{
    OnridePhotoBits = value;
}

uint8_t TrackElement::GetPhotoTimeout() const
{
    return OnridePhotoBits;
}

void TrackElement::DecrementPhotoTimeout()
{
    OnridePhotoBits = std::max(0, OnridePhotoBits - 1);
}

uint16_t TrackElement::GetMazeEntry() const
{
    return MazeEntry;
}

void TrackElement::SetMazeEntry(uint16_t newMazeEntry)
{
    MazeEntry = newMazeEntry;
}

void TrackElement::MazeEntryAdd(uint16_t addVal)
{
    MazeEntry |= addVal;
}

void TrackElement::MazeEntrySubtract(uint16_t subVal)
{
    MazeEntry &= ~subVal;
}

uint16_t TrackElement::GetTrackType() const
{
    return TrackType;
}

void TrackElement::SetTrackType(uint16_t newType)
{
    TrackType = newType;
}

uint8_t TrackElement::GetSequenceIndex() const
{
    return Sequence;
}

void TrackElement::SetSequenceIndex(uint8_t newSequenceIndex)
{
    Sequence = newSequenceIndex;
}

uint8_t TrackElement::GetStationIndex() const
{
    return StationIndex;
}

void TrackElement::SetStationIndex(uint8_t newStationIndex)
{
    StationIndex = newStationIndex;
}

uint8_t TrackElement::GetDoorAState() const
{
    return (ColourScheme & TRACK_ELEMENT_COLOUR_DOOR_A_MASK) >> 2;
}

uint8_t TrackElement::GetDoorBState() const
{
    return (ColourScheme & TRACK_ELEMENT_COLOUR_DOOR_B_MASK) >> 5;
}

void TrackElement::SetDoorAState(uint8_t newState)
{
    ColourScheme &= ~TRACK_ELEMENT_COLOUR_DOOR_A_MASK;
    ColourScheme |= ((newState << 2) & TRACK_ELEMENT_COLOUR_DOOR_A_MASK);
}

void TrackElement::SetDoorBState(uint8_t newState)
{
    ColourScheme &= ~TRACK_ELEMENT_COLOUR_DOOR_B_MASK;
    ColourScheme |= ((newState << 5) & TRACK_ELEMENT_COLOUR_DOOR_B_MASK);
}

ride_id_t TrackElement::GetRideIndex() const
{
    return RideIndex;
}

void TrackElement::SetRideIndex(ride_id_t newRideIndex)
{
    RideIndex = newRideIndex;
}

uint8_t TrackElement::GetColourScheme() const
{
    return ColourScheme & TRACK_ELEMENT_COLOUR_SCHEME_MASK;
}

void TrackElement::SetColourScheme(uint8_t newColourScheme)
{
    ColourScheme &= ~TRACK_ELEMENT_COLOUR_SCHEME_MASK;
    ColourScheme |= (newColourScheme & TRACK_ELEMENT_COLOUR_SCHEME_MASK);
}

bool TrackElement::HasCableLift() const
{
    return Flags2 & TRACK_ELEMENT_FLAGS2_CABLE_LIFT;
}

void TrackElement::SetHasCableLift(bool on)
{
    Flags2 &= ~TRACK_ELEMENT_FLAGS2_CABLE_LIFT;
    if (on)
        Flags2 |= TRACK_ELEMENT_FLAGS2_CABLE_LIFT;
}

bool TrackElement::IsInverted() const
{
    return Flags2 & TRACK_ELEMENT_FLAGS2_INVERTED;
}

void TrackElement::SetInverted(bool inverted)
{
    if (inverted)
    {
        Flags2 |= TRACK_ELEMENT_FLAGS2_INVERTED;
    }
    else
    {
        Flags2 &= ~TRACK_ELEMENT_FLAGS2_INVERTED;
    }
}

bool TrackElement::BlockBrakeClosed() const
{
    return (Flags2 & TRACK_ELEMENT_FLAGS2_BLOCK_BRAKE_CLOSED) != 0;
}

void TrackElement::SetBlockBrakeClosed(bool isClosed)
{
    if (isClosed)
    {
        Flags2 |= TRACK_ELEMENT_FLAGS2_BLOCK_BRAKE_CLOSED;
    }
    else
    {
        Flags2 &= ~TRACK_ELEMENT_FLAGS2_BLOCK_BRAKE_CLOSED;
    }
}

bool TrackElement::IsIndestructible() const
{
    return (Flags2 & TRACK_ELEMENT_FLAGS2_INDESTRUCTIBLE_TRACK_PIECE) != 0;
}

void TrackElement::SetIsIndestructible(bool isIndestructible)
{
    if (isIndestructible)
    {
        Flags2 |= TRACK_ELEMENT_FLAGS2_INDESTRUCTIBLE_TRACK_PIECE;
    }
    else
    {
        Flags2 &= ~TRACK_ELEMENT_FLAGS2_INDESTRUCTIBLE_TRACK_PIECE;
    }
}

uint8_t TrackElement::GetBrakeBoosterSpeed() const
{
    return BrakeBoosterSpeed << 1;
}

void TrackElement::SetBrakeBoosterSpeed(uint8_t speed)
{
    BrakeBoosterSpeed = (speed >> 1);
}

bool TrackElement::HasGreenLight() const
{
    return (Flags2 & TRACK_ELEMENT_FLAGS2_HAS_GREEN_LIGHT) != 0;
}

void TrackElement::SetHasGreenLight(bool on)
{
    Flags2 &= ~TRACK_ELEMENT_FLAGS2_HAS_GREEN_LIGHT;
    if (on)
    {
        Flags2 |= TRACK_ELEMENT_FLAGS2_HAS_GREEN_LIGHT;
    }
}

bool TrackElement::IsHighlighted() const
{
    return (Flags2 & TRACK_ELEMENT_FLAGS2_HIGHLIGHT);
}

void TrackElement::SetHighlight(bool on)
{
    Flags2 &= ~TRACK_ELEMENT_FLAGS2_HIGHLIGHT;
    if (on)
        Flags2 |= TRACK_ELEMENT_FLAGS2_HIGHLIGHT;
}
