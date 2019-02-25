/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
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
#include "RideGroupManager.h"
#include "RideRatings.h"
#include "Station.h"
#include "TrackData.h"
#include "TrackDesign.h"

uint8_t gTrackGroundFlags;

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
    { TRACK_HALF_LOOP_LARGE,        TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        64                 },  // ELEM_LEFT_LARGE_HALF_LOOP_UP
    { TRACK_HALF_LOOP_LARGE,        TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        64                 },  // ELEM_RIGHT_LARGE_HALF_LOOP_UP
    { TRACK_HALF_LOOP_LARGE,        TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, -64                },  // ELEM_RIGHT_LARGE_HALF_LOOP_DOWN
    { TRACK_HALF_LOOP_LARGE,        TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, -64                },  // ELEM_LEFT_LARGE_HALF_LOOP_DOWN
    { TRACK_INLINE_TWIST_UNINVERTED,TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  },  // ELEM_LEFT_FLYER_TWIST_UP
    { TRACK_INLINE_TWIST_UNINVERTED,TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  },  // ELEM_RIGHT_FLYER_TWIST_UP
    { TRACK_INLINE_TWIST_INVERTED,  TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  },  // ELEM_LEFT_FLYER_TWIST_DOWN
    { TRACK_INLINE_TWIST_INVERTED,  TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  },  // ELEM_RIGHT_FLYER_TWIST_DOWN
    { TRACK_HALF_LOOP_UNINVERTED,   TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        64                 },  // ELEM_FLYER_HALF_LOOP_UP
    { TRACK_HALF_LOOP_INVERTED,     TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        -64                },  // ELEM_FLYER_HALF_LOOP_DOWN
    { TRACK_190,                    TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  },  // ELEM_LEFT_FLYER_CORKSCREW_UP
    { TRACK_190,                    TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  },  // ELEM_RIGHT_FLYER_CORKSCREW_UP
    { TRACK_192,                    TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        -32                },  // ELEM_LEFT_FLYER_CORKSCREW_DOWN
    { TRACK_192,                    TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        -32                },  // ELEM_RIGHT_FLYER_CORKSCREW_DOWN
    { TRACK_194,                    TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_HEARTLINE_TRANSFER_UP
    { TRACK_194,                    TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_HEARTLINE_TRANSFER_DOWN
    { TRACK_HEARTLINE_ROLL,         TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_LEFT_HEARTLINE_ROLL
    { TRACK_HEARTLINE_ROLL,         TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_RIGHT_HEARTLINE_ROLL
    { TRACK_MINI_GOLF_HOLE,         TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_MINI_GOLF_HOLE_A
    { TRACK_MINI_GOLF_HOLE,         TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_MINI_GOLF_HOLE_B
    { TRACK_MINI_GOLF_HOLE,         TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_MINI_GOLF_HOLE_C
    { TRACK_MINI_GOLF_HOLE,         TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_MINI_GOLF_HOLE_D
    { TRACK_MINI_GOLF_HOLE,         TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                  },  // ELEM_MINI_GOLF_HOLE_E
    { TRACK_QUARTER_LOOP_INVERTED,  TRACK_SLOPE_DOWN_90,        TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                  },  // ELEM_MULTIDIM_INVERTED_FLAT_TO_90_DEG_QUARTER_LOOP_DOWN
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
    { TRACK_BARREL_ROLL,            TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, 0                           },  // 177
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_LEFT,        0                           },  // 178
    { TRACK_FLAT,                   TRACK_SLOPE_UP_25,          TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_RIGHT,       0                           },  // 179
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_LEFT,        TRACK_BANK_NONE,        0                           },  // 180
    { TRACK_FLAT,                   TRACK_SLOPE_NONE,           TRACK_SLOPE_DOWN_25,        TRACK_BANK_RIGHT,       TRACK_BANK_NONE,        0                           },  // 181
    { TRACK_POWERED_LIFT,           TRACK_SLOPE_UP_25,          TRACK_SLOPE_UP_25,          TRACK_BANK_NONE,        TRACK_BANK_NONE,        0                           },  // 182
    { TRACK_HALF_LOOP_LARGE,        TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_UPSIDE_DOWN,TRACK_BANK_NONE,         64                          },  // 183
    { TRACK_HALF_LOOP_LARGE,        TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_UPSIDE_DOWN,TRACK_BANK_NONE,         64                          },  // 184
    { TRACK_HALF_LOOP_LARGE,        TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, -64                         },  // 185
    { TRACK_HALF_LOOP_LARGE,        TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_NONE,        TRACK_BANK_UPSIDE_DOWN, -64                         },  // 186
    { TRACK_190,                    TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN,TRACK_BANK_NONE,         0                           },  // 187
    { TRACK_190,                    TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN,TRACK_BANK_NONE,         0                           },  // 188
    { TRACK_192,                    TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN,TRACK_BANK_NONE,         0                           },  // 189
    { TRACK_192,                    TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN,TRACK_BANK_NONE,         0                           },  // 190
    { TRACK_190,                    TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_25,          TRACK_BANK_UPSIDE_DOWN,TRACK_BANK_NONE,         64                          },  // 191
    { TRACK_192,                    TRACK_SLOPE_DOWN_25,        TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN,TRACK_BANK_NONE,         -64                         },  // 192
    { TRACK_190,                    TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN,TRACK_BANK_NONE,         0                           },  // 193
    { TRACK_190,                    TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN,TRACK_BANK_NONE,         0                           },  // 194
    { TRACK_192,                    TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN,TRACK_BANK_NONE,         -32                         },  // 195
    { TRACK_192,                    TRACK_SLOPE_NONE,           TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN,TRACK_BANK_NONE,         -32                         },  // 196
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
    { TRACK_QUARTER_LOOP_UNINVERTED,TRACK_SLOPE_NONE,           TRACK_SLOPE_UP_90,          TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                           },  // 253
    { TRACK_QUARTER_LOOP_UNINVERTED,TRACK_SLOPE_DOWN_90,        TRACK_SLOPE_NONE,           TRACK_BANK_UPSIDE_DOWN, TRACK_BANK_NONE,        0                           },  // 254
};
// clang-format on

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

static TileElement* find_station_element(int32_t x, int32_t y, int32_t z, int32_t direction, ride_id_t rideIndex)
{
    TileElement* tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do
    {
        if (z != tileElement->base_height)
            continue;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;
        if (tileElement->GetDirection() != direction)
            continue;
        if (tileElement->AsTrack()->GetRideIndex() != rideIndex)
            continue;
        if (!track_element_is_station(tileElement))
            continue;

        return tileElement;
    } while (!(tileElement++)->IsLastForTile());
    return nullptr;
}

static void ride_remove_station(Ride* ride, int32_t x, int32_t y, int32_t z)
{
    for (int32_t i = 0; i < MAX_STATIONS; i++)
    {
        if (ride->stations[i].Start.x == (x >> 5) && ride->stations[i].Start.y == (y >> 5) && ride->stations[i].Height == z)
        {
            ride->stations[i].Start.xy = RCT_XY8_UNDEFINED;
            ride->num_stations--;
            break;
        }
    }
}

/**
 *
 *  rct2: 0x006C4D89
 */
bool track_add_station_element(int32_t x, int32_t y, int32_t z, int32_t direction, ride_id_t rideIndex, int32_t flags)
{
    int32_t stationX0 = x;
    int32_t stationY0 = y;
    int32_t stationX1 = x;
    int32_t stationY1 = y;
    int32_t stationLength = 1;

    Ride* ride = get_ride(rideIndex);
    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_3))
    {
        if (ride->num_stations >= MAX_STATIONS)
        {
            gGameCommandErrorText = STR_NO_MORE_STATIONS_ALLOWED_ON_THIS_RIDE;
            return false;
        }
        if (flags & GAME_COMMAND_FLAG_APPLY)
        {
            int8_t stationIndex = ride_get_first_empty_station_start(ride);
            assert(stationIndex != -1);

            ride->stations[stationIndex].Start.x = (x >> 5);
            ride->stations[stationIndex].Start.y = (y >> 5);
            ride->stations[stationIndex].Height = z;
            ride->stations[stationIndex].Depart = 1;
            ride->stations[stationIndex].Length = 0;
            ride->num_stations++;
        }
        return true;
    }

    TileElement* stationElement;

    // Search backwards for more station
    x = stationX0;
    y = stationY0;
    do
    {
        x -= CoordsDirectionDelta[direction].x;
        y -= CoordsDirectionDelta[direction].y;

        stationElement = find_station_element(x, y, z, direction, rideIndex);
        if (stationElement != nullptr)
        {
            if (stationElement->AsTrack()->GetTrackType() == TRACK_ELEM_END_STATION)
            {
                if (flags & GAME_COMMAND_FLAG_APPLY)
                {
                    ride_remove_station(ride, x, y, z);
                }
            }

            stationX0 = x;
            stationY0 = y;
            stationLength++;
        }
    } while (stationElement != nullptr);

    // Search forwards for more station
    x = stationX1;
    y = stationY1;
    do
    {
        x += CoordsDirectionDelta[direction].x;
        y += CoordsDirectionDelta[direction].y;

        stationElement = find_station_element(x, y, z, direction, rideIndex);
        if (stationElement != nullptr)
        {
            if (stationElement->AsTrack()->GetTrackType() == TRACK_ELEM_END_STATION)
            {
                if (flags & GAME_COMMAND_FLAG_APPLY)
                {
                    ride_remove_station(ride, x, y, z);
                }
            }

            stationX1 = x;
            stationY1 = y;
            stationLength++;
        }
    } while (stationElement != nullptr);

    if (stationX0 == stationX1 && stationY0 == stationY1 && ride->num_stations >= MAX_STATIONS)
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
        x = stationX1;
        y = stationY1;

        bool finaliseStationDone;
        do
        {
            finaliseStationDone = true;

            stationElement = find_station_element(x, y, z, direction, rideIndex);
            if (stationElement != nullptr)
            {
                int32_t targetTrackType;
                if (x == stationX1 && y == stationY1)
                {
                    int8_t stationIndex = ride_get_first_empty_station_start(ride);
                    assert(stationIndex != -1);

                    ride->stations[stationIndex].Start.x = (x >> 5);
                    ride->stations[stationIndex].Start.y = (y >> 5);
                    ride->stations[stationIndex].Height = z;
                    ride->stations[stationIndex].Depart = 1;
                    ride->stations[stationIndex].Length = stationLength;
                    ride->num_stations++;

                    targetTrackType = TRACK_ELEM_END_STATION;
                }
                else if (x == stationX0 && y == stationY0)
                {
                    targetTrackType = TRACK_ELEM_BEGIN_STATION;
                }
                else
                {
                    targetTrackType = TRACK_ELEM_MIDDLE_STATION;
                }
                stationElement->AsTrack()->SetTrackType(targetTrackType);

                map_invalidate_element(x, y, stationElement);

                if (x != stationX0 || y != stationY0)
                {
                    x -= CoordsDirectionDelta[direction].x;
                    y -= CoordsDirectionDelta[direction].y;
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
bool track_remove_station_element(int32_t x, int32_t y, int32_t z, int32_t direction, ride_id_t rideIndex, int32_t flags)
{
    int32_t removeX = x;
    int32_t removeY = y;
    int32_t stationX0 = x;
    int32_t stationY0 = y;
    int32_t stationX1 = x;
    int32_t stationY1 = y;
    int32_t stationLength = 0;
    int32_t byte_F441D1 = -1;

    Ride* ride = get_ride(rideIndex);
    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_3))
    {
        TileElement* tileElement = map_get_track_element_at_with_direction_from_ride(x, y, z, direction, rideIndex);
        if (tileElement != nullptr)
        {
            if (flags & GAME_COMMAND_FLAG_APPLY)
            {
                ride_remove_station(ride, x, y, z);
            }
        }
        return true;
    }

    TileElement* stationElement;

    // Search backwards for more station
    x = stationX0;
    y = stationY0;
    while ((stationElement = find_station_element(x, y, z, direction, rideIndex)) != nullptr)
    {
        if (stationElement->AsTrack()->GetTrackType() == TRACK_ELEM_END_STATION)
        {
            if (flags & GAME_COMMAND_FLAG_APPLY)
            {
                ride_remove_station(ride, x, y, z);
            }
        }

        stationX0 = x;
        stationY0 = y;
        byte_F441D1++;

        x -= CoordsDirectionDelta[direction].x;
        y -= CoordsDirectionDelta[direction].y;
    }

    // Search forwards for more station
    x = stationX1;
    y = stationY1;
    do
    {
        x += CoordsDirectionDelta[direction].x;
        y += CoordsDirectionDelta[direction].y;

        stationElement = find_station_element(x, y, z, direction, rideIndex);
        if (stationElement != nullptr)
        {
            if (stationElement->AsTrack()->GetTrackType() == TRACK_ELEM_END_STATION)
            {
                if (flags & GAME_COMMAND_FLAG_APPLY)
                {
                    ride_remove_station(ride, x, y, z);
                }
            }

            stationX1 = x;
            stationY1 = y;
            stationLength++;
        }
    } while (stationElement != nullptr);

    if (!(flags & GAME_COMMAND_FLAG_APPLY))
    {
        if ((removeX != stationX0 || removeY != stationY0) && (removeX != stationX1 || removeY != stationY1)
            && ride->num_stations >= MAX_STATIONS)
        {
            gGameCommandErrorText = STR_NO_MORE_STATIONS_ALLOWED_ON_THIS_RIDE;
            return false;
        }
        else
        {
            return true;
        }
    }

    x = stationX1;
    y = stationY1;
    bool finaliseStationDone;
    do
    {
        finaliseStationDone = true;

        if (x != removeX || y != removeY)
        {
            stationElement = find_station_element(x, y, z, direction, rideIndex);
            if (stationElement != nullptr)
            {
                int32_t targetTrackType;
                if (x == stationX1 && y == stationY1)
                {
                loc_6C4BF5:;
                    int8_t stationIndex = ride_get_first_empty_station_start(ride);
                    assert(stationIndex != -1);

                    ride->stations[stationIndex].Start.x = (x >> 5);
                    ride->stations[stationIndex].Start.y = (y >> 5);
                    ride->stations[stationIndex].Height = z;
                    ride->stations[stationIndex].Depart = 1;
                    ride->stations[stationIndex].Length = stationLength != 0 ? stationLength : byte_F441D1;
                    ride->num_stations++;

                    stationLength = 0;
                    targetTrackType = TRACK_ELEM_END_STATION;
                }
                else
                {
                    if (x + CoordsDirectionDelta[direction].x == removeX && y + CoordsDirectionDelta[direction].y == removeY)
                    {
                        goto loc_6C4BF5;
                    }
                    else
                    {
                        if (x - CoordsDirectionDelta[direction].x == removeX
                            && y - CoordsDirectionDelta[direction].y == removeY)
                        {
                            targetTrackType = TRACK_ELEM_BEGIN_STATION;
                        }
                        else
                        {
                            if (x == stationX0 && y == stationY0)
                            {
                                targetTrackType = TRACK_ELEM_BEGIN_STATION;
                            }
                            else
                            {
                                targetTrackType = TRACK_ELEM_MIDDLE_STATION;
                            }
                        }
                    }
                }
                stationElement->AsTrack()->SetTrackType(targetTrackType);

                map_invalidate_element(x, y, stationElement);
            }
        }

        if (x != stationX0 || y != stationY0)
        {
            x -= CoordsDirectionDelta[direction].x;
            y -= CoordsDirectionDelta[direction].y;
            finaliseStationDone = false;
        }
    } while (!finaliseStationDone);

    return true;
}

static money32 track_remove(
    uint8_t type, uint8_t sequence, int16_t originX, int16_t originY, int16_t originZ, uint8_t rotation, uint8_t flags)
{
    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
    gCommandPosition.x = originX + 16;
    gCommandPosition.y = originY + 16;
    gCommandPosition.z = originZ;
    int16_t trackpieceZ = originZ;

    switch (type)
    {
        case TRACK_ELEM_BEGIN_STATION:
        case TRACK_ELEM_MIDDLE_STATION:
            type = TRACK_ELEM_END_STATION;
            break;
    }

    if (!(flags & (1 << 3)) && game_is_paused() && !gCheatsBuildInPauseMode)
    {
        gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
        return MONEY32_UNDEFINED;
    }

    bool found = false;
    bool isGhost = flags & GAME_COMMAND_FLAG_GHOST;
    TileElement* tileElement = map_get_first_element_at(originX / 32, originY / 32);
    if (tileElement == nullptr)
    {
        log_warning("Invalid coordinates for track removal. x = %d, y = %d", originX, originY);
        return MONEY32_UNDEFINED;
    }
    do
    {
        if (tileElement->base_height * 8 != originZ)
            continue;

        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;

        if ((tileElement->GetDirection()) != rotation)
            continue;

        if (tileElement->AsTrack()->GetSequenceIndex() != sequence)
            continue;

        if (tileElement->IsGhost() != isGhost)
            continue;

        uint8_t track_type = tileElement->AsTrack()->GetTrackType();
        switch (track_type)
        {
            case TRACK_ELEM_BEGIN_STATION:
            case TRACK_ELEM_MIDDLE_STATION:
                track_type = TRACK_ELEM_END_STATION;
                break;
        }

        if (track_type != type)
            continue;

        found = true;
        break;
    } while (!(tileElement++)->IsLastForTile());

    if (!found)
    {
        return MONEY32_UNDEFINED;
    }

    if (tileElement->flags & TILE_ELEMENT_FLAG_INDESTRUCTIBLE_TRACK_PIECE)
    {
        gGameCommandErrorText = STR_YOU_ARE_NOT_ALLOWED_TO_REMOVE_THIS_SECTION;
        return MONEY32_UNDEFINED;
    }

    ride_id_t rideIndex = tileElement->AsTrack()->GetRideIndex();
    type = tileElement->AsTrack()->GetTrackType();
    bool isLiftHill = tileElement->AsTrack()->HasChain();

    Ride* ride = get_ride(rideIndex);
    const rct_preview_track* trackBlock = get_track_def_from_ride(ride, type);
    trackBlock += tileElement->AsTrack()->GetSequenceIndex();

    uint8_t originDirection = tileElement->GetDirection();
    switch (originDirection)
    {
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
    for (; trackBlock->index != 255; trackBlock++)
    {
        int16_t x = originX, y = originY, z = originZ;

        switch (originDirection)
        {
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

        found = false;
        tileElement = map_get_first_element_at(x / 32, y / 32);
        do
        {
            if (tileElement == nullptr)
                break;

            if (tileElement->base_height != z / 8)
                continue;

            if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
                continue;

            if (tileElement->GetDirection() != rotation)
                continue;

            if (tileElement->AsTrack()->GetSequenceIndex() != trackBlock->index)
                continue;

            if (tileElement->AsTrack()->GetTrackType() != type)
                continue;

            if (tileElement->IsGhost() != isGhost)
                continue;

            found = true;
            break;
        } while (!(tileElement++)->IsLastForTile());

        if (!found)
        {
            log_error("Track map element part not found!");
            return MONEY32_UNDEFINED;
        }

        int32_t entranceDirections;
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
        {
            entranceDirections = FlatRideTrackSequenceProperties[type][0];
        }
        else
        {
            entranceDirections = TrackSequenceProperties[type][0];
        }

        if (entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN && (tileElement->AsTrack()->GetSequenceIndex() == 0))
        {
            if (!track_remove_station_element(x, y, z / 8, rotation, rideIndex, 0))
            {
                return MONEY32_UNDEFINED;
            }
        }

        TileElement* surfaceElement = map_get_surface_element_at({ x, y });
        if (surfaceElement == nullptr)
        {
            return MONEY32_UNDEFINED;
        }

        int8_t _support_height = tileElement->base_height - surfaceElement->base_height;
        if (_support_height < 0)
        {
            _support_height = 10;
        }

        cost += (_support_height / 2) * RideTrackCosts[ride->type].support_price;

        if (!(flags & GAME_COMMAND_FLAG_APPLY))
            continue;

        if (entranceDirections & (1 << 4) && (tileElement->AsTrack()->GetSequenceIndex() == 0))
        {
            if (!track_remove_station_element(x, y, z / 8, rotation, rideIndex, GAME_COMMAND_FLAG_APPLY))
            {
                return MONEY32_UNDEFINED;
            }
        }

        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_TRACK_MUST_BE_ON_WATER))
        {
            surfaceElement->AsSurface()->SetHasTrackThatNeedsWater(false);
        }

        invalidate_test_results(ride);
        footpath_queue_chain_reset();
        if (!gCheatsDisableClearanceChecks || !(tileElement->flags & TILE_ELEMENT_FLAG_GHOST))
        {
            footpath_remove_edges_at(x, y, tileElement);
        }
        tile_element_remove(tileElement);
        sub_6CB945(ride);
        if (!(flags & GAME_COMMAND_FLAG_GHOST))
        {
            ride_update_max_vehicles(ride);
        }
    }

    if (flags & GAME_COMMAND_FLAG_APPLY)
    {
        switch (type)
        {
            case TRACK_ELEM_ON_RIDE_PHOTO:
                ride->lifecycle_flags &= ~RIDE_LIFECYCLE_ON_RIDE_PHOTO;
                break;
            case TRACK_ELEM_CABLE_LIFT_HILL:
                ride->lifecycle_flags &= ~RIDE_LIFECYCLE_CABLE_LIFT_HILL_COMPONENT_USED;
                break;
            case TRACK_ELEM_BLOCK_BRAKES:
                ride->num_block_brakes--;
                if (ride->num_block_brakes == 0)
                {
                    ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_OPERATING;
                    ride->mode = RIDE_MODE_CONTINUOUS_CIRCUIT;
                    if (ride->type == RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER)
                    {
                        ride->mode = RIDE_MODE_POWERED_LAUNCH;
                    }
                }
                break;
        }

        switch (type)
        {
            case TRACK_ELEM_25_DEG_UP_TO_FLAT:
            case TRACK_ELEM_60_DEG_UP_TO_FLAT:
            case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
            case TRACK_ELEM_DIAG_60_DEG_UP_TO_FLAT:
                if (!isLiftHill)
                    break;
                [[fallthrough]];
            case TRACK_ELEM_CABLE_LIFT_HILL:
                ride->num_block_brakes--;
                break;
        }
    }

    money32 price = RideTrackCosts[ride->type].track_price;
    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
    {
        price *= FlatRideTrackPricing[type];
    }
    else
    {
        price *= TrackPricing[type];
    }
    price >>= 16;
    price = (price + cost) / 2;
    if (ride->lifecycle_flags & RIDE_LIFECYCLE_EVER_BEEN_OPENED)
        price *= -7;
    else
        price *= -10;

    if (gGameCommandNestLevel == 1)
    {
        LocationXYZ16 coord;
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
void game_command_remove_track(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, [[maybe_unused]] int32_t* esi, int32_t* edi,
    [[maybe_unused]] int32_t* ebp)
{
    *ebx = track_remove(
        *edx & 0xFF, (*edx >> 8) & 0xFF, *eax & 0xFFFF, *ecx & 0xFFFF, *edi & 0xFFFF, (*ebx >> 8) & 0xFF, *ebx & 0xFF);
}

/**
 *
 *  rct2: 0x006C5AE9
 */
void game_command_set_brakes_speed(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, [[maybe_unused]] int32_t* esi, int32_t* edi,
    [[maybe_unused]] int32_t* ebp)
{
    int32_t x = (*eax & 0xFFFF);
    int32_t y = (*ecx & 0xFFFF);
    int32_t z = (*edi & 0xFFFF);
    int32_t trackType = (*edx & 0xFF);
    int32_t brakesSpeed = ((*ebx >> 8) & 0xFF);

    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
    gCommandPosition.x = x + 16;
    gCommandPosition.y = y + 16;
    gCommandPosition.z = z;

    if (*ebx & GAME_COMMAND_FLAG_APPLY)
    {
        *ebx = 0;
        return;
    }

    TileElement* tileElement = map_get_first_element_at(x >> 5, y >> 5);
    if (tileElement == nullptr)
    {
        log_warning("Invalid game command for setting brakes speed. x = %d, y = %d", x, y);
        *ebx = MONEY32_UNDEFINED;
        return;
    }
    do
    {
        if (tileElement->base_height * 8 != z)
            continue;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;
        if (tileElement->AsTrack()->GetTrackType() != trackType)
            continue;

        tileElement->AsTrack()->SetBrakeBoosterSpeed(brakesSpeed);

        break;
    } while (!(tileElement++)->IsLastForTile());

    *ebx = 0;
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
        if (!track_block_get_previous(it->last.x, it->last.y, it->last.element, &trackBeginEnd))
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

        if (track_block_get_previous(it->last.x, it->last.y, it->last.element, &trackBeginEnd))
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
        result = track_block_get_previous(lastTrack.x, lastTrack.y, lastTrack.element, &currentTrack);
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
    return type & TRACK_ELEMENT_TYPE_FLAG_CHAIN_LIFT;
}

void TrackElement::SetHasChain(bool on)
{
    if (on)
    {
        type |= TRACK_ELEMENT_TYPE_FLAG_CHAIN_LIFT;
    }
    else
    {
        type &= ~TRACK_ELEMENT_TYPE_FLAG_CHAIN_LIFT;
    }
}

/**
 * Checks if a track element is recognised as the beginning of a block.
 * A beginning of a block can be the end of a station, the end of a lift hill,
 * or a block brake.
 */
bool track_element_is_block_start(TileElement* trackElement)
{
    switch (trackElement->AsTrack()->GetTrackType())
    {
        case TRACK_ELEM_END_STATION:
        case TRACK_ELEM_CABLE_LIFT_HILL:
        case TRACK_ELEM_BLOCK_BRAKES:
            return true;
        case TRACK_ELEM_25_DEG_UP_TO_FLAT:
        case TRACK_ELEM_60_DEG_UP_TO_FLAT:
        case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
        case TRACK_ELEM_DIAG_60_DEG_UP_TO_FLAT:
            if (trackElement->AsTrack()->HasChain())
            {
                return true;
            }
            break;
    }
    return false;
}

int32_t track_get_actual_bank(TileElement* tileElement, int32_t bank)
{
    Ride* ride = get_ride(tileElement->AsTrack()->GetRideIndex());
    bool isInverted = tileElement->AsTrack()->IsInverted();
    return track_get_actual_bank_2(ride->type, isInverted, bank);
}

int32_t track_get_actual_bank_2(int32_t rideType, bool isInverted, int32_t bank)
{
    if (RideData4[rideType].flags & RIDE_TYPE_FLAG4_HAS_ALTERNATIVE_TRACK_TYPE)
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

int32_t track_get_actual_bank_3(rct_vehicle* vehicle, TileElement* tileElement)
{
    bool isInverted = ((vehicle->update_flags & VEHICLE_UPDATE_FLAG_USE_INVERTED_SPRITES) > 0)
        ^ tileElement->AsTrack()->IsInverted();
    int32_t trackType = tileElement->AsTrack()->GetTrackType();
    int32_t rideType = get_ride(tileElement->AsTrack()->GetRideIndex())->type;
    int32_t bankStart = TrackDefinitions[trackType].bank_start;
    return track_get_actual_bank_2(rideType, isInverted, bankStart);
}

bool track_element_is_station(TileElement* trackElement)
{
    switch (trackElement->AsTrack()->GetTrackType())
    {
        case TRACK_ELEM_END_STATION:
        case TRACK_ELEM_BEGIN_STATION:
        case TRACK_ELEM_MIDDLE_STATION:
            return true;
        default:
            return false;
    }
}

bool track_element_is_covered(int32_t trackElementType)
{
    switch (trackElementType)
    {
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

bool track_element_is_booster(uint8_t rideType, uint8_t trackType)
{
    // Boosters share their ID with the Spinning Control track.
    return rideType != RIDE_TYPE_STEEL_WILD_MOUSE && trackType == TRACK_ELEM_BOOSTER;
}

bool track_element_has_speed_setting(uint8_t trackType)
{
    // This does not check if the element is really a Spinning Control track instead of a booster,
    // but this does not cause problems.
    return trackType == TRACK_ELEM_BRAKES || trackType == TRACK_ELEM_BOOSTER;
}

uint8_t TrackElement::GetSeatRotation() const
{
    return colour >> 4;
}

void TrackElement::SetSeatRotation(uint8_t newSeatRotation)
{
    colour &= 0x0F;
    colour |= (newSeatRotation << 4);
}

bool TrackElement::IsTakingPhoto() const
{
    return (sequence & MAP_ELEM_TRACK_SEQUENCE_TAKING_PHOTO_MASK) != 0;
}

void TrackElement::SetPhotoTimeout()
{
    sequence &= MAP_ELEM_TRACK_SEQUENCE_SEQUENCE_MASK;
    sequence |= (3 << 4);
}

void TrackElement::SetPhotoTimeout(uint8_t value)
{
    sequence &= MAP_ELEM_TRACK_SEQUENCE_SEQUENCE_MASK;
    sequence |= (value << 4);
}

void TrackElement::DecrementPhotoTimeout()
{
    // We should only touch the upper 4 bits, avoid underflow into the lower 4.
    if (sequence & MAP_ELEM_TRACK_SEQUENCE_TAKING_PHOTO_MASK)
    {
        sequence -= (1 << 4);
    }
}

uint16_t TrackElement::GetMazeEntry() const
{
    return mazeEntry;
}

void TrackElement::SetMazeEntry(uint16_t newMazeEntry)
{
    mazeEntry = newMazeEntry;
}

void TrackElement::MazeEntryAdd(uint16_t addVal)
{
    mazeEntry |= addVal;
}

void TrackElement::MazeEntrySubtract(uint16_t subVal)
{
    mazeEntry &= ~subVal;
}

uint8_t TrackElement::GetTrackType() const
{
    return trackType;
}

void TrackElement::SetTrackType(uint8_t newType)
{
    trackType = newType;
}

uint8_t TrackElement::GetSequenceIndex() const
{
    return sequence & MAP_ELEM_TRACK_SEQUENCE_SEQUENCE_MASK;
}

void TrackElement::SetSequenceIndex(uint8_t newSequenceIndex)
{
    sequence &= ~MAP_ELEM_TRACK_SEQUENCE_SEQUENCE_MASK;
    sequence |= (newSequenceIndex & MAP_ELEM_TRACK_SEQUENCE_SEQUENCE_MASK);
}

uint8_t TrackElement::GetStationIndex() const
{
    return (sequence & MAP_ELEM_TRACK_SEQUENCE_STATION_INDEX_MASK) >> 4;
}

void TrackElement::SetStationIndex(uint8_t newStationIndex)
{
    sequence &= ~MAP_ELEM_TRACK_SEQUENCE_STATION_INDEX_MASK;
    sequence |= (newStationIndex << 4);
}

uint8_t TrackElement::GetDoorAState() const
{
    return (colour & TRACK_ELEMENT_DOOR_A_MASK) >> 2;
}

uint8_t TrackElement::GetDoorBState() const
{
    return (colour & TRACK_ELEMENT_DOOR_B_MASK) >> 5;
}

ride_id_t TrackElement::GetRideIndex() const
{
    return rideIndex;
}

void TrackElement::SetRideIndex(ride_id_t newRideIndex)
{
    rideIndex = newRideIndex;
}

uint8_t TrackElement::GetColourScheme() const
{
    return colour & 0x3;
}

void TrackElement::SetColourScheme(uint8_t newColourScheme)
{
    colour &= ~0x3;
    colour |= (newColourScheme & 0x3);
}

bool TrackElement::HasCableLift() const
{
    return colour & TRACK_ELEMENT_COLOUR_FLAG_CABLE_LIFT;
}

void TrackElement::SetHasCableLift(bool on)
{
    colour &= ~TRACK_ELEMENT_COLOUR_FLAG_CABLE_LIFT;
    if (on)
        colour |= TRACK_ELEMENT_COLOUR_FLAG_CABLE_LIFT;
}

bool TrackElement::IsInverted() const
{
    return colour & TRACK_ELEMENT_COLOUR_FLAG_INVERTED;
}

void TrackElement::SetInverted(bool inverted)
{
    if (inverted)
    {
        colour |= TRACK_ELEMENT_COLOUR_FLAG_INVERTED;
    }
    else
    {
        colour &= ~TRACK_ELEMENT_COLOUR_FLAG_INVERTED;
    }
}

uint8_t TrackElement::GetBrakeBoosterSpeed() const
{
    return (sequence >> 4) << 1;
}

void TrackElement::SetBrakeBoosterSpeed(uint8_t speed)
{
    sequence &= ~0b11110000;
    sequence |= ((speed >> 1) << 4);
}

uint8_t TrackElement::HasGreenLight() const
{
    return (sequence & MAP_ELEM_TRACK_SEQUENCE_GREEN_LIGHT) != 0;
}

void TrackElement::SetHasGreenLight(uint8_t greenLight)
{
    sequence &= ~MAP_ELEM_TRACK_SEQUENCE_GREEN_LIGHT;
    if (greenLight)
    {
        sequence |= MAP_ELEM_TRACK_SEQUENCE_GREEN_LIGHT;
    }
}

bool TrackElement::IsHighlighted() const
{
    return (type & TILE_ELEMENT_TYPE_FLAG_HIGHLIGHT);
}

void TrackElement::SetHighlight(bool on)
{
    type &= ~TILE_ELEMENT_TYPE_FLAG_HIGHLIGHT;
    if (on)
        type |= TILE_ELEMENT_TYPE_FLAG_HIGHLIGHT;
}
