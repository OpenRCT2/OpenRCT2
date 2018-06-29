/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../audio/audio.h"
#include "../Cheats.h"
#include "../config/Config.h"
#include "../Game.h"
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
#include "RideGroupManager.h"
#include "Station.h"
#include "Track.h"
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
int32_t track_is_connected_by_shape(rct_tile_element * a, rct_tile_element * b)
{
    int32_t trackType, aBank, aAngle, bBank, bAngle;

    trackType = track_element_get_type(a);
    aBank     = TrackDefinitions[trackType].bank_end;
    aAngle    = TrackDefinitions[trackType].vangle_end;
    aBank     = track_get_actual_bank(a, aBank);

    trackType = track_element_get_type(b);
    bBank     = TrackDefinitions[trackType].bank_start;
    bAngle    = TrackDefinitions[trackType].vangle_start;
    bBank     = track_get_actual_bank(b, bBank);

    return aBank == bBank && aAngle == bAngle;
}

const rct_preview_track * get_track_def_from_ride(Ride * ride, int32_t trackType)
{
    return ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE) ?
           FlatRideTrackBlocks[trackType] :
           TrackBlocks[trackType];
}

const rct_track_coordinates * get_track_coord_from_ride(Ride * ride, int32_t trackType)
{
    return ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE) ?
           &FlatTrackCoordinates[trackType] :
           &TrackCoordinates[trackType];
}

const rct_preview_track * get_track_def_from_ride_index(int32_t rideIndex, int32_t trackType)
{
    return get_track_def_from_ride(get_ride(rideIndex), trackType);
}

static rct_tile_element * find_station_element(int32_t x, int32_t y, int32_t z, int32_t direction, int32_t rideIndex)
{
    rct_tile_element * tileElement = map_get_first_element_at(x >> 5, y >> 5);
    do
    {
        if (z != tileElement->base_height)
            continue;
        if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;
        if (tile_element_get_direction(tileElement) != direction)
            continue;
        if (track_element_get_ride_index(tileElement) != rideIndex)
            continue;
        if (!track_element_is_station(tileElement))
            continue;

        return tileElement;
    }
    while (!(tileElement++)->IsLastForTile());
    return nullptr;
}

static void ride_remove_station(Ride * ride, int32_t x, int32_t y, int32_t z)
{
    for (int32_t i = 0; i < MAX_STATIONS; i++)
    {
        if (ride->station_starts[i].x == (x >> 5) &&
            ride->station_starts[i].y == (y >> 5) &&
            ride->station_heights[i] == z)
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
static bool track_add_station_element(int32_t x, int32_t y, int32_t z, int32_t direction, int32_t rideIndex, int32_t flags)
{
    int32_t stationX0     = x;
    int32_t stationY0     = y;
    int32_t stationX1     = x;
    int32_t stationY1     = y;
    int32_t stationLength = 1;

    Ride * ride = get_ride(rideIndex);
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

            ride->station_starts[stationIndex].x = (x >> 5);
            ride->station_starts[stationIndex].y = (y >> 5);
            ride->station_heights[stationIndex]  = z;
            ride->station_depart[stationIndex]   = 1;
            ride->station_length[stationIndex]   = 0;
            ride->num_stations++;
        }
        return true;
    }

    rct_tile_element * stationElement;

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
            if (track_element_get_type(stationElement) == TRACK_ELEM_END_STATION)
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
    }
    while (stationElement != nullptr);

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
            if (track_element_get_type(stationElement) == TRACK_ELEM_END_STATION)
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
    }
    while (stationElement != nullptr);

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

                    ride->station_starts[stationIndex].x = (x >> 5);
                    ride->station_starts[stationIndex].y = (y >> 5);
                    ride->station_heights[stationIndex]  = z;
                    ride->station_depart[stationIndex]   = 1;
                    ride->station_length[stationIndex]   = stationLength;
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
                track_element_set_type(stationElement, targetTrackType);

                map_invalidate_element(x, y, stationElement);

                if (x != stationX0 || y != stationY0)
                {
                    x -= CoordsDirectionDelta[direction].x;
                    y -= CoordsDirectionDelta[direction].y;
                    finaliseStationDone = false;
                }
            }
        }
        while (!finaliseStationDone);
    }
    return true;
}

/**
 *
 *  rct2: 0x006C494B
 */
static bool track_remove_station_element(int32_t x, int32_t y, int32_t z, int32_t direction, int32_t rideIndex, int32_t flags)
{
    int32_t removeX       = x;
    int32_t removeY       = y;
    int32_t stationX0     = x;
    int32_t stationY0     = y;
    int32_t stationX1     = x;
    int32_t stationY1     = y;
    int32_t stationLength = 0;
    int32_t byte_F441D1   = -1;

    Ride * ride = get_ride(rideIndex);
    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_3))
    {
        rct_tile_element * tileElement = map_get_track_element_at_with_direction_from_ride(x, y, z, direction, rideIndex);
        if (tileElement != nullptr)
        {
            if (flags & GAME_COMMAND_FLAG_APPLY)
            {
                ride_remove_station(ride, x, y, z);
            }
        }
        return true;
    }

    rct_tile_element * stationElement;

    // Search backwards for more station
    x = stationX0;
    y = stationY0;
    while ((stationElement = find_station_element(x, y, z, direction, rideIndex)) != nullptr)
    {
        if (track_element_get_type(stationElement) == TRACK_ELEM_END_STATION)
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
            if (track_element_get_type(stationElement) == TRACK_ELEM_END_STATION)
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
    }
    while (stationElement != nullptr);

    if (!(flags & GAME_COMMAND_FLAG_APPLY))
    {
        if ((removeX != stationX0 || removeY != stationY0) &&
            (removeX != stationX1 || removeY != stationY1) &&
            ride->num_stations >= MAX_STATIONS
            )
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

                    ride->station_starts[stationIndex].x = (x >> 5);
                    ride->station_starts[stationIndex].y = (y >> 5);
                    ride->station_heights[stationIndex]  = z;
                    ride->station_depart[stationIndex]   = 1;
                    ride->station_length[stationIndex]   = stationLength != 0 ? stationLength : byte_F441D1;
                    ride->num_stations++;

                    stationLength   = 0;
                    targetTrackType = TRACK_ELEM_END_STATION;
                }
                else
                {
                    if (x + CoordsDirectionDelta[direction].x == removeX &&
                        y + CoordsDirectionDelta[direction].y == removeY)
                    {
                        goto loc_6C4BF5;
                    }
                    else
                    {
                        if (x - CoordsDirectionDelta[direction].x == removeX &&
                            y - CoordsDirectionDelta[direction].y == removeY)
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
                track_element_set_type(stationElement, targetTrackType);

                map_invalidate_element(x, y, stationElement);
            }
        }

        if (x != stationX0 || y != stationY0)
        {
            x -= CoordsDirectionDelta[direction].x;
            y -= CoordsDirectionDelta[direction].y;
            finaliseStationDone = false;
        }
    }
    while (!finaliseStationDone);

    return true;
}

static money32 track_place(int32_t rideIndex,
                           int32_t type,
                           int32_t originX,
                           int32_t originY,
                           int32_t originZ,
                           int32_t direction,
                           int32_t brakeSpeed,
                           int32_t colour,
                           int32_t seatRotation,
                           int32_t liftHillAndAlternativeState,
                           int32_t flags)
{
    Ride * ride = get_ride(rideIndex);
    if (ride == nullptr)
    {
        log_warning("Invalid ride for track placement, rideIndex = %d", rideIndex);
        return MONEY32_UNDEFINED;
    }
    if (ride->type == RIDE_TYPE_NULL)
    {
        log_warning("Invalid ride type, rideIndex = %d", rideIndex);
        return MONEY32_UNDEFINED;
    }
    rct_ride_entry * rideEntry = get_ride_entry(ride->subtype);
    if (rideEntry == nullptr)
    {
        log_warning("Invalid ride type for track placement, rideIndex = %d", rideIndex);
        return MONEY32_UNDEFINED;
    }
    rct_tile_element * tileElement;

    gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION;
    gCommandPosition.x = originX + 16;
    gCommandPosition.y = originY + 16;
    gCommandPosition.z = originZ;
    int16_t trackpieceZ = originZ;
    direction &= 3;
    gTrackGroundFlags  = 0;

    uint32_t rideTypeFlags      = RideProperties[ride->type].flags;

    if ((ride->lifecycle_flags & RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK) && type == TRACK_ELEM_END_STATION)
    {
        gGameCommandErrorText = STR_NOT_ALLOWED_TO_MODIFY_STATION;
        return MONEY32_UNDEFINED;
    }

    if (!(flags & GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED))
    {
        if (game_is_paused() && !gCheatsBuildInPauseMode)
        {
            gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
            return MONEY32_UNDEFINED;
        }
    }

    const uint8_t (* wallEdges)[16];
    if (rideTypeFlags & RIDE_TYPE_FLAG_FLAT_RIDE)
    {
        wallEdges = &FlatRideTrackSequenceElementAllowedWallEdges[type];
    }
    else
    {
        if (type == TRACK_ELEM_ON_RIDE_PHOTO)
        {
            if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO)
            {
                gGameCommandErrorText = STR_ONLY_ONE_ON_RIDE_PHOTO_PER_RIDE;
                return MONEY32_UNDEFINED;
            }
        }
        else if (type == TRACK_ELEM_CABLE_LIFT_HILL)
        {
            if (ride->lifecycle_flags & RIDE_LIFECYCLE_CABLE_LIFT_HILL_COMPONENT_USED)
            {
                gGameCommandErrorText = STR_ONLY_ONE_CABLE_LIFT_HILL_PER_RIDE;
                return MONEY32_UNDEFINED;
            }
        }
        // Backwards steep lift hills are allowed, even on roller coasters that do not support forwards steep lift hills.
        if ((liftHillAndAlternativeState & CONSTRUCTION_LIFT_HILL_SELECTED) &&
            !(RideTypePossibleTrackConfigurations[ride->type] & (1ULL << TRACK_LIFT_HILL_STEEP)) &&
            !gCheatsEnableChainLiftOnAllTrack)
        {
            if (TrackFlags[type] & TRACK_ELEM_FLAG_IS_STEEP_UP)
            {
                gGameCommandErrorText = STR_TOO_STEEP_FOR_LIFT_HILL;
                return MONEY32_UNDEFINED;
            }
        }

        wallEdges = &TrackSequenceElementAllowedWallEdges[type];
    }

    money32                   cost         = 0;
    const rct_preview_track * trackBlock   = get_track_def_from_ride(ride, type);
    uint32_t                    num_elements = 0;
    // First check if any of the track pieces are outside the park
    for (; trackBlock->index != 0xFF; trackBlock++)
    {
        int32_t offsetX = 0;
        int32_t offsetY = 0;

        switch (direction)
        {
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

        int32_t x = originX + offsetX;
        int32_t y = originY + offsetY;
        int32_t z = originZ + trackBlock->z;

        if (!map_is_location_owned(x, y, z) && !gCheatsSandboxMode)
        {
            gGameCommandErrorText = STR_LAND_NOT_OWNED_BY_PARK;
            return MONEY32_UNDEFINED;
        }
        num_elements++;
    }

    if (!map_check_free_elements_and_reorganise(num_elements))
    {
        return MONEY32_UNDEFINED;
    }
    const uint16_t * trackFlags = (rideTypeFlags & RIDE_TYPE_FLAG_FLAT_RIDE) ?
                                FlatTrackFlags :
                                TrackFlags;
    if (trackFlags[type] & TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT)
    {
        if ((originZ & 0x0F) != 8)
        {
            gGameCommandErrorText = STR_CONSTRUCTION_ERR_UNKNOWN;
            return MONEY32_UNDEFINED;
        }
    }
    else
    {
        if ((originZ & 0x0F) != 0)
        {
            gGameCommandErrorText = STR_CONSTRUCTION_ERR_UNKNOWN;
            return MONEY32_UNDEFINED;
        }
    }

    // If that is not the case, then perform the remaining checks
    trackBlock = get_track_def_from_ride(ride, type);

    for (int32_t blockIndex = 0; trackBlock->index != 0xFF; trackBlock++, blockIndex++)
    {
        int32_t offsetX = 0;
        int32_t offsetY = 0;
        int32_t bl      = trackBlock->var_08;
        int32_t bh;
        switch (direction)
        {
        case 0:
            offsetX = trackBlock->x;
            offsetY = trackBlock->y;
            break;
        case 1:
            offsetX = trackBlock->y;
            offsetY = -trackBlock->x;
            bl      = rol8(bl, 1);
            bh      = bl;
            bh      = ror8(bh, 4);
            bl &= 0xEE;
            bh &= 0x11;
            bl |= bh;
            break;
        case 2:
            offsetX = -trackBlock->x;
            offsetY = -trackBlock->y;
            bl      = rol8(bl, 2);
            bh      = bl;
            bh      = ror8(bh, 4);
            bl &= 0xCC;
            bh &= 0x33;
            bl |= bh;
            break;
        case 3:
            offsetX = -trackBlock->y;
            offsetY = trackBlock->x;
            bl      = rol8(bl, 3);
            bh      = bl;
            bh      = ror8(bh, 4);
            bl &= 0x88;
            bh &= 0x77;
            bl |= bh;
            break;
        }
        int32_t x = originX + offsetX;
        int32_t y = originY + offsetY;
        int32_t z = originZ + trackBlock->z;

        trackpieceZ = z;

        if (z < 16)
        {
            gGameCommandErrorText = STR_TOO_LOW;
            return MONEY32_UNDEFINED;
        }

        int32_t baseZ = (originZ + trackBlock->z) / 8;

        int32_t clearanceZ = trackBlock->var_07;
        if (trackBlock->var_09 & (1 << 2) && RideData5[ride->type].clearance_height > 24)
        {
            clearanceZ += 24;
        }
        else
        {
            clearanceZ += RideData5[ride->type].clearance_height;
        }

        clearanceZ = (clearanceZ / 8) + baseZ;

        if (clearanceZ >= 255)
        {
            gGameCommandErrorText = STR_TOO_HIGH;
            return MONEY32_UNDEFINED;
        }

        _currentTrackEndX = x;
        _currentTrackEndY = y;

        if (!gCheatsDisableClearanceChecks || flags & GAME_COMMAND_FLAG_GHOST)
        {
            uint8_t crossingMode = (ride->type == RIDE_TYPE_MINIATURE_RAILWAY && type == TRACK_ELEM_FLAT) ?
                CREATE_CROSSING_MODE_TRACK_OVER_PATH :
                CREATE_CROSSING_MODE_NONE;
            if (!map_can_construct_with_clear_at(x, y, baseZ, clearanceZ, &map_place_non_scenery_clear_func, bl, flags, &cost, crossingMode))
                return MONEY32_UNDEFINED;
        }

        //6c53dc

        if ((flags & GAME_COMMAND_FLAG_APPLY) && !(flags & GAME_COMMAND_FLAG_GHOST) && !gCheatsDisableClearanceChecks)
        {
            footpath_remove_litter(x, y, z);
            if (rideTypeFlags & RIDE_TYPE_FLAG_TRACK_NO_WALLS)
            {
                wall_remove_at(x, y, baseZ * 8, clearanceZ * 8);
            }
            else
            {
                // Remove walls in the directions this track intersects
                uint8_t intersectingDirections = (*wallEdges)[blockIndex];
                intersectingDirections ^= 0x0F;
                for (int32_t i = 0; i < 4; i++)
                {
                    if (intersectingDirections & (1 << i))
                    {
                        wall_remove_intersecting_walls(x, y, baseZ, clearanceZ, i);
                    }
                }
            }
        }

        bh = gMapGroundFlags & (ELEMENT_IS_ABOVE_GROUND | ELEMENT_IS_UNDERGROUND);
        if (gTrackGroundFlags != 0 && (gTrackGroundFlags & bh) == 0)
        {
            gGameCommandErrorText = STR_CANT_BUILD_PARTLY_ABOVE_AND_PARTLY_BELOW_GROUND;
            return MONEY32_UNDEFINED;
        }

        gTrackGroundFlags = bh;
        if (rideTypeFlags & RIDE_TYPE_FLAG_FLAT_RIDE)
        {
            if (FlatTrackFlags[type] & TRACK_ELEM_FLAG_ONLY_ABOVE_GROUND)
            {
                if (gTrackGroundFlags & TRACK_ELEMENT_LOCATION_IS_UNDERGROUND)
                {
                    gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND;
                    return MONEY32_UNDEFINED;
                }
            }
        }
        else
        {
            if (TrackFlags[type] & TRACK_ELEM_FLAG_ONLY_ABOVE_GROUND)
            {
                if (gTrackGroundFlags & TRACK_ELEMENT_LOCATION_IS_UNDERGROUND)
                {
                    gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND;
                    return MONEY32_UNDEFINED;
                }
            }
        }

        if (rideTypeFlags & RIDE_TYPE_FLAG_FLAT_RIDE)
        {
            if (FlatTrackFlags[type] & TRACK_ELEM_FLAG_ONLY_UNDERWATER)
            {
                if (!(gMapGroundFlags & ELEMENT_IS_UNDERWATER))
                {
                    gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_UNDERWATER;
                    return MONEY32_UNDEFINED;
                }
            }
        }
        else
        {
            if (TrackFlags[type] & TRACK_ELEM_FLAG_ONLY_UNDERWATER)
            { // No element has this flag
                if (gMapGroundFlags & ELEMENT_IS_UNDERWATER)
                {
                    gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_UNDERWATER;
                    return MONEY32_UNDEFINED;
                }
            }
        }

        if (gMapGroundFlags & ELEMENT_IS_UNDERWATER && !gCheatsDisableClearanceChecks)
        {
            gGameCommandErrorText = STR_RIDE_CANT_BUILD_THIS_UNDERWATER;
            return MONEY32_UNDEFINED;
        }

        if ((rideTypeFlags & RIDE_TYPE_FLAG_TRACK_MUST_BE_ON_WATER) && !byte_9D8150)
        {
            tileElement = map_get_surface_element_at({x, y});

            uint8_t water_height = surface_get_water_height(tileElement) * 2;
            if (water_height == 0)
            {
                gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_ON_WATER;
                return MONEY32_UNDEFINED;
            }

            if (water_height != baseZ)
            {
                gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_ON_WATER;
                return MONEY32_UNDEFINED;
            }
            water_height -= 2;
            if (water_height == tileElement->base_height)
            {
                bh = tileElement->properties.surface.slope & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP;
                if (bh == TILE_ELEMENT_SLOPE_W_CORNER_DN ||
                    bh == TILE_ELEMENT_SLOPE_S_CORNER_DN ||
                    bh == TILE_ELEMENT_SLOPE_E_CORNER_DN ||
                    bh == TILE_ELEMENT_SLOPE_N_CORNER_DN)
                {
                    gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_ON_WATER;
                    return MONEY32_UNDEFINED;
                }
            }
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
        if ((entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN) && trackBlock->index == 0)
        {
            if (!track_add_station_element(x, y, baseZ, direction, rideIndex, 0))
            {
                return MONEY32_UNDEFINED;
            }
        }
        //6c55be
        if (entranceDirections & TRACK_SEQUENCE_FLAG_CONNECTS_TO_PATH)
        {
            entranceDirections &= 0x0F;
            if (entranceDirections != 0)
            {
                if (!(flags & GAME_COMMAND_FLAG_APPLY) &&
                    !(flags & GAME_COMMAND_FLAG_GHOST) &&
                    !gCheatsDisableClearanceChecks)
                {
                    uint8_t _bl = entranceDirections;
                    for (int32_t dl = bitscanforward(_bl); dl != -1; dl = bitscanforward(_bl))
                    {
                        _bl &= ~(1 << dl);
                        int32_t temp_x         = x, temp_y = y;
                        int32_t temp_direction = (direction + dl) & 3;
                        temp_x += CoordsDirectionDelta[temp_direction].x;
                        temp_y += CoordsDirectionDelta[temp_direction].y;
                        temp_direction ^= (1 << 1);
                        wall_remove_intersecting_walls(temp_x, temp_y, baseZ, clearanceZ, temp_direction & 3);
                    }
                }
            }
        }
        //6c5648 12 push
        tileElement = map_get_surface_element_at({x, y});
        if (!gCheatsDisableSupportLimits)
        {
            int32_t ride_height = clearanceZ - tileElement->base_height;
            if (ride_height >= 0)
            {

                uint16_t maxHeight;

                if (RideGroupManager::RideTypeIsIndependent(ride->type) && rideEntry->max_height != 0)
                {
                    maxHeight = rideEntry->max_height;
                }
                else if (RideGroupManager::RideTypeHasRideGroups(ride->type))
                {
                    const RideGroup * rideGroup = RideGroupManager::GetRideGroup(ride->type, rideEntry);
                    maxHeight = rideGroup->MaximumHeight;
                }
                else
                {
                    maxHeight = RideData5[ride->type].max_height;
                }

                ride_height /= 2;
                if (ride_height > maxHeight && !byte_9D8150)
                {
                    gGameCommandErrorText = STR_TOO_HIGH_FOR_SUPPORTS;
                    return MONEY32_UNDEFINED;
                }
            }
        }

        int32_t _support_height = baseZ - tileElement->base_height;
        if (_support_height < 0)
        {
            _support_height = 10;
        }

        cost += ((_support_height / 2) * RideTrackCosts[ride->type].support_price) * 5;

        //6c56d3

        if (!(flags & GAME_COMMAND_FLAG_APPLY))
            continue;

        invalidate_test_results(rideIndex);
        switch (type)
        {
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

        if (trackBlock->index == 0)
        {
            switch (type)
            {
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
        if (ride->overall_view.xy != RCT_XY8_UNDEFINED)
        {
            if (!(flags & GAME_COMMAND_FLAG_5))
            {
                if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
                {
                    entranceDirections = FlatRideTrackSequenceProperties[type][0];
                }
                else
                {
                    entranceDirections = TrackSequenceProperties[type][0];
                }
            }
        }
        if (entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN || ride->overall_view.xy == RCT_XY8_UNDEFINED)
        {
            ride->overall_view.x = x / 32;
            ride->overall_view.y = y / 32;
        }

        tileElement = tile_element_insert(x / 32, y / 32, baseZ, bl & 0xF);
        assert(tileElement != nullptr);
        tileElement->clearance_height = clearanceZ;

        uint8_t map_type = direction;
        map_type |= TILE_ELEMENT_TYPE_TRACK;
        if (liftHillAndAlternativeState & CONSTRUCTION_LIFT_HILL_SELECTED)
        {
            map_type |= (1 << 7);
        }
        tileElement->type = map_type;

        tile_element_set_track_sequence(tileElement, trackBlock->index);
        track_element_set_ride_index(tileElement, rideIndex);
        track_element_set_type(tileElement, type);

        if (flags & GAME_COMMAND_FLAG_GHOST)
        {
            tileElement->flags |= TILE_ELEMENT_FLAG_GHOST;
        }

        switch (type)
        {
        case TRACK_ELEM_WATERFALL:
            map_animation_create(MAP_ANIMATION_TYPE_TRACK_WATERFALL, x, y, tileElement->base_height);
            break;
        case TRACK_ELEM_RAPIDS:
            map_animation_create(MAP_ANIMATION_TYPE_TRACK_RAPIDS, x, y, tileElement->base_height);
            break;
        case TRACK_ELEM_WHIRLPOOL:
            map_animation_create(MAP_ANIMATION_TYPE_TRACK_WHIRLPOOL, x, y, tileElement->base_height);
            break;
        case TRACK_ELEM_SPINNING_TUNNEL:
            map_animation_create(MAP_ANIMATION_TYPE_TRACK_SPINNINGTUNNEL, x, y, tileElement->base_height);
            break;
        }
        if (track_element_has_speed_setting(type))
        {
            tile_element_set_brake_booster_speed(tileElement, brakeSpeed);
        }
        else
        {
            track_element_set_seat_rotation(tileElement, seatRotation);
        }

        if (liftHillAndAlternativeState & RIDE_TYPE_ALTERNATIVE_TRACK_TYPE)
        {
            track_element_set_inverted(tileElement, true);
        }
        track_element_set_colour_scheme(tileElement, colour);

        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
        {
            entranceDirections = FlatRideTrackSequenceProperties[type][0];
        }
        else
        {
            entranceDirections = TrackSequenceProperties[type][0];
        }

        if (entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN)
        {
            if (trackBlock->index == 0)
            {
                track_add_station_element(x, y, baseZ, direction, rideIndex, GAME_COMMAND_FLAG_APPLY);
            }
            sub_6CB945(rideIndex);
            ride_update_max_vehicles(rideIndex);
        }

        if (rideTypeFlags & RIDE_TYPE_FLAG_TRACK_MUST_BE_ON_WATER)
        {
            rct_tile_element * surfaceElement = map_get_surface_element_at({x, y});
            surfaceElement->type |= (1 << 6);
            tileElement = surfaceElement;
        }

        if (!gCheatsDisableClearanceChecks || !(flags & GAME_COMMAND_FLAG_GHOST))
        {
            footpath_connect_edges(x, y, tileElement, flags);
        }
        map_invalidate_tile_full(x, y);
    }

    if (gGameCommandNestLevel == 1)
    {
        LocationXYZ16 coord;
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

    if (gParkFlags & PARK_FLAGS_NO_MONEY)
    {
        return 0;
    }
    else
    {
        return price;
    }
}

/**
 *
 *  rct2: 0x006C511D
 */
void game_command_place_track(
    int32_t *                  eax,
    int32_t *                  ebx,
    int32_t *                  ecx,
    int32_t *                  edx,
    [[maybe_unused]] int32_t * esi,
    int32_t *                  edi,
    [[maybe_unused]] int32_t * ebp)
{
    *ebx = track_place(
        *edx & 0xFF,
        (*edx >> 8) & 0xFF,
        (int16_t) (*eax & 0xFFFF),
        (int16_t) (*ecx & 0xFFFF),
        (int16_t) (*edi & 0xFFFF),
        (*ebx >> 8) & 0xFF,
        (*edi >> 16) & 0xFF,
        (*edi >> 24) & 0x0F,
        (*edi >> 28) & 0x0F,
        (*edx >> 16),
        *ebx & 0xFF
    );
}

static money32 track_remove(uint8_t type,
                            uint8_t sequence,
                            int16_t originX,
                            int16_t originY,
                            int16_t originZ,
                            uint8_t rotation,
                            uint8_t flags)
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
    rct_tile_element * tileElement = map_get_first_element_at(originX / 32, originY / 32);
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

        if ((tile_element_get_direction(tileElement)) != rotation)
            continue;

        if (tile_element_get_track_sequence(tileElement) != sequence)
            continue;

        if (tileElement->IsGhost() != isGhost)
            continue;

        uint8_t track_type = track_element_get_type(tileElement);
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
    }
    while (!(tileElement++)->IsLastForTile());

    if (!found)
    {
        return MONEY32_UNDEFINED;
    }

    if (tileElement->flags & TILE_ELEMENT_FLAG_INDESTRUCTIBLE_TRACK_PIECE)
    {
        gGameCommandErrorText = STR_YOU_ARE_NOT_ALLOWED_TO_REMOVE_THIS_SECTION;
        return MONEY32_UNDEFINED;
    }

    uint8_t rideIndex = track_element_get_ride_index(tileElement);
    type = track_element_get_type(tileElement);
    bool isLiftHill = track_element_is_lift_hill(tileElement);

    Ride                    * ride       = get_ride(rideIndex);
    const rct_preview_track * trackBlock = get_track_def_from_ride(ride, type);
    trackBlock += tile_element_get_track_sequence(tileElement);

    uint8_t originDirection = tile_element_get_direction(tileElement);
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

            if ((tile_element_get_direction(tileElement)) != rotation)
                continue;

            if (tile_element_get_track_sequence(tileElement) != trackBlock->index)
                continue;

            if (track_element_get_type(tileElement) != type)
                continue;

            if (tileElement->IsGhost() != isGhost)
                continue;

            found = true;
            break;
        }
        while (!(tileElement++)->IsLastForTile());

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

        if (entranceDirections & TRACK_SEQUENCE_FLAG_ORIGIN && (tile_element_get_track_sequence(tileElement) == 0))
        {
            if (!track_remove_station_element(x, y, z / 8, rotation, rideIndex, 0))
            {
                return MONEY32_UNDEFINED;
            }
        }

        rct_tile_element * surfaceElement = map_get_surface_element_at({x, y});
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

        if (entranceDirections & (1 << 4) && (tile_element_get_track_sequence(tileElement) == 0))
        {
            if (!track_remove_station_element(x, y, z / 8, rotation, rideIndex, GAME_COMMAND_FLAG_APPLY))
            {
                return MONEY32_UNDEFINED;
            }
        }

        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_TRACK_MUST_BE_ON_WATER))
        {
            surfaceElement->type &= ~(1 << 6);
        }

        invalidate_test_results(rideIndex);
        footpath_queue_chain_reset();
        if (!gCheatsDisableClearanceChecks || !(tileElement->flags & TILE_ELEMENT_FLAG_GHOST))
        {
            footpath_remove_edges_at(x, y, tileElement);
        }
        tile_element_remove(tileElement);
        if (!(flags & GAME_COMMAND_FLAG_GHOST))
        {
            sub_6CB945(rideIndex);
            ride_update_max_vehicles(rideIndex);
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
            // Fall through
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
    int32_t * eax, int32_t * ebx, int32_t * ecx, int32_t * edx, [[maybe_unused]] int32_t * esi, int32_t * edi, [[maybe_unused]] int32_t * ebp)
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
}

/**
 *
 *  rct2: 0x006C5AE9
 */
void game_command_set_brakes_speed(
    int32_t *                  eax,
    int32_t *                  ebx,
    int32_t *                  ecx,
    int32_t *                  edx,
    [[maybe_unused]] int32_t * esi,
    int32_t *                  edi,
    [[maybe_unused]] int32_t * ebp)
{
    int32_t x           = (*eax & 0xFFFF);
    int32_t y           = (*ecx & 0xFFFF);
    int32_t z           = (*edi & 0xFFFF);
    int32_t trackType   = (*edx & 0xFF);
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

    rct_tile_element * tileElement = map_get_first_element_at(x >> 5, y >> 5);
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
        if (track_element_get_type(tileElement) != trackType)
            continue;

        tile_element_set_brake_booster_speed(tileElement, brakesSpeed);

        break;
    }
    while (!(tileElement++)->IsLastForTile());

    *ebx = 0;
}

void track_circuit_iterator_begin(track_circuit_iterator * it, CoordsXYE first)
{
    it->last           = first;
    it->first          = nullptr;
    it->firstIteration = true;
    it->looped         = false;
}

bool track_circuit_iterator_previous(track_circuit_iterator * it)
{
    track_begin_end trackBeginEnd;

    if (it->first == nullptr)
    {
        if (!track_block_get_previous(it->last.x, it->last.y, it->last.element, &trackBeginEnd))
            return false;

        it->current.x        = trackBeginEnd.begin_x;
        it->current.y        = trackBeginEnd.begin_y;
        it->current.element  = trackBeginEnd.begin_element;
        it->currentZ         = trackBeginEnd.begin_z;
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
        it->last           = it->current;

        if (track_block_get_previous(it->last.x, it->last.y, it->last.element, &trackBeginEnd))
        {
            it->current.x        = trackBeginEnd.end_x;
            it->current.y        = trackBeginEnd.end_y;
            it->current.element  = trackBeginEnd.begin_element;
            it->currentZ         = trackBeginEnd.begin_z;
            it->currentDirection = trackBeginEnd.begin_direction;
            return true;
        }
        else
        {
            return false;
        }
    }
}

bool track_circuit_iterator_next(track_circuit_iterator * it)
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
        it->last           = it->current;
        return track_block_get_next(&it->last, &it->current, &it->currentZ, &it->currentDirection);
    }
}

bool track_circuit_iterators_match(const track_circuit_iterator * firstIt, const track_circuit_iterator * secondIt)
{
    return (firstIt->currentZ == secondIt->currentZ &&
            firstIt->currentDirection == secondIt->currentDirection &&
            firstIt->current.x == secondIt->current.x &&
            firstIt->current.y == secondIt->current.y);
}

void track_get_back(CoordsXYE * input, CoordsXYE * output)
{
    CoordsXYE  lastTrack;
    track_begin_end currentTrack;
    bool            result;

    lastTrack = *input;
    do
    {
        result = track_block_get_previous(lastTrack.x, lastTrack.y, lastTrack.element, &currentTrack);
        if (result)
        {
            lastTrack.x       = currentTrack.begin_x;
            lastTrack.y       = currentTrack.begin_y;
            lastTrack.element = currentTrack.begin_element;
        }
    }
    while (result);
    *output = lastTrack;
}

void track_get_front(CoordsXYE * input, CoordsXYE * output)
{
    CoordsXYE lastTrack, currentTrack;
    int32_t       z, direction;
    bool         result;

    lastTrack = *input;
    do
    {
        result = track_block_get_next(&lastTrack, &currentTrack, &z, &direction);
        if (result)
        {
            lastTrack = currentTrack;
        }
    }
    while (result);
    *output = lastTrack;
}

bool track_element_is_lift_hill(const rct_tile_element * trackElement)
{
    return trackElement->type & TRACK_ELEMENT_TYPE_FLAG_CHAIN_LIFT;
}

void track_element_set_lift_hill(rct_tile_element * trackElement, bool on)
{
    if (on)
    {
        trackElement->type |= TRACK_ELEMENT_TYPE_FLAG_CHAIN_LIFT;
    }
    else
    {
        trackElement->type &= ~TRACK_ELEMENT_TYPE_FLAG_CHAIN_LIFT;
    }
}

/**
 * Checks if a track element is recognised as the beginning of a block.
 * A beginning of a block can be the end of a station, the end of a lift hill,
 * or a block brake.
 */
bool track_element_is_block_start(rct_tile_element * trackElement)
{
    switch (track_element_get_type(trackElement))
    {
    case TRACK_ELEM_END_STATION:
    case TRACK_ELEM_CABLE_LIFT_HILL:
    case TRACK_ELEM_BLOCK_BRAKES:
        return true;
    case TRACK_ELEM_25_DEG_UP_TO_FLAT:
    case TRACK_ELEM_60_DEG_UP_TO_FLAT:
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
    case TRACK_ELEM_DIAG_60_DEG_UP_TO_FLAT:
        if (track_element_is_lift_hill(trackElement))
        {
            return true;
        }
        break;
    }
    return false;
}

bool track_element_is_cable_lift(const rct_tile_element * trackElement)
{
    return trackElement->properties.track.colour & TRACK_ELEMENT_COLOUR_FLAG_CABLE_LIFT;
}

void track_element_set_cable_lift(rct_tile_element * trackElement)
{
    trackElement->properties.track.colour |= TRACK_ELEMENT_COLOUR_FLAG_CABLE_LIFT;
}

void track_element_clear_cable_lift(rct_tile_element * trackElement)
{
    trackElement->properties.track.colour &= ~TRACK_ELEMENT_COLOUR_FLAG_CABLE_LIFT;
}

bool track_element_is_inverted(const rct_tile_element * tileElement)
{
    return tileElement->properties.track.colour & TRACK_ELEMENT_COLOUR_FLAG_INVERTED;
}

void track_element_set_inverted(rct_tile_element * tileElement, bool inverted)
{
    if (inverted)
    {
        tileElement->properties.track.colour |= TRACK_ELEMENT_COLOUR_FLAG_INVERTED;
    }
    else
    {
        tileElement->properties.track.colour &= ~TRACK_ELEMENT_COLOUR_FLAG_INVERTED;
    }
}

int32_t track_get_actual_bank(rct_tile_element * tileElement, int32_t bank)
{
    Ride * ride     = get_ride(track_element_get_ride_index(tileElement));
    bool isInverted = track_element_is_inverted(tileElement);
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

int32_t track_get_actual_bank_3(rct_vehicle * vehicle, rct_tile_element * tileElement)
{
    bool isInverted         = ((vehicle->update_flags & VEHICLE_UPDATE_FLAG_USE_INVERTED_SPRITES) > 0) ^ track_element_is_inverted(tileElement);
    int32_t trackType        = track_element_get_type(tileElement);
    int32_t rideType         = get_ride(track_element_get_ride_index(tileElement))->type;
    int32_t bankStart        = TrackDefinitions[trackType].bank_start;
    return track_get_actual_bank_2(rideType, isInverted, bankStart);
}

bool track_element_is_station(rct_tile_element * trackElement)
{
    switch (track_element_get_type(trackElement))
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

uint8_t track_element_get_seat_rotation(const rct_tile_element * tileElement)
{
    return tileElement->properties.track.colour >> 4;
}

void track_element_set_seat_rotation(rct_tile_element * tileElement, uint8_t seatRotation)
{
    tileElement->properties.track.colour &= 0x0F;
    tileElement->properties.track.colour |= (seatRotation << 4);
}

uint8_t track_element_get_colour_scheme(const rct_tile_element * tileElement)
{
    return tileElement->properties.track.colour & 0x3;
}

void track_element_set_colour_scheme(rct_tile_element * tileElement, uint8_t colourScheme)
{
    tileElement->properties.track.colour &= ~0x3;
    tileElement->properties.track.colour |= (colourScheme & 0x3);
}


void tile_element_set_station(rct_tile_element * tileElement, uint32_t stationIndex)
{
    tileElement->properties.track.sequence &= ~MAP_ELEM_TRACK_SEQUENCE_STATION_INDEX_MASK;
    tileElement->properties.track.sequence |= (stationIndex << 4);
}

int32_t tile_element_get_track_sequence(const rct_tile_element * tileElement)
{
    return tileElement->properties.track.sequence & MAP_ELEM_TRACK_SEQUENCE_SEQUENCE_MASK;
}

void tile_element_set_track_sequence(rct_tile_element * tileElement, int32_t trackSequence)
{
    tileElement->properties.track.sequence &= ~MAP_ELEM_TRACK_SEQUENCE_SEQUENCE_MASK;
    tileElement->properties.track.sequence |= (trackSequence & MAP_ELEM_TRACK_SEQUENCE_SEQUENCE_MASK);
}

bool tile_element_get_green_light(const rct_tile_element * tileElement)
{
    return (tileElement->properties.track.sequence & MAP_ELEM_TRACK_SEQUENCE_GREEN_LIGHT) != 0;
}

void tile_element_set_green_light(rct_tile_element * tileElement, bool greenLight)
{
    tileElement->properties.track.sequence &= ~MAP_ELEM_TRACK_SEQUENCE_GREEN_LIGHT;
    if (greenLight)
    {
        tileElement->properties.track.sequence |= MAP_ELEM_TRACK_SEQUENCE_GREEN_LIGHT;
    }
}

int32_t tile_element_get_brake_booster_speed(const rct_tile_element * tileElement)
{
    return (tileElement->properties.track.sequence >> 4) << 1;
}

void tile_element_set_brake_booster_speed(rct_tile_element * tileElement, int32_t speed)
{
    tileElement->properties.track.sequence = tile_element_get_track_sequence(tileElement) | ((speed >> 1) << 4);
}

bool tile_element_is_taking_photo(const rct_tile_element * tileElement)
{
    return (tileElement->properties.track.sequence & MAP_ELEM_TRACK_SEQUENCE_TAKING_PHOTO_MASK) != 0;
}

void tile_element_set_onride_photo_timeout(rct_tile_element * tileElement)
{
    tileElement->properties.track.sequence &= MAP_ELEM_TRACK_SEQUENCE_SEQUENCE_MASK;
    tileElement->properties.track.sequence |= (3 << 4);
}

void tile_element_decrement_onride_photo_timout(rct_tile_element * tileElement)
{
    // We should only touch the upper 4 bits, avoid underflow into the lower 4.
    if (tileElement->properties.track.sequence & MAP_ELEM_TRACK_SEQUENCE_TAKING_PHOTO_MASK)
    {
        tileElement->properties.track.sequence -= (1 << 4);
    }
}

uint16_t track_element_get_maze_entry(const rct_tile_element * tileElement)
{
    return tileElement->properties.track.maze_entry;
}

uint8_t track_element_get_ride_index(const rct_tile_element * tileElement)
{
    return tileElement->properties.track.ride_index;
}

void track_element_set_ride_index(rct_tile_element * tileElement, uint8_t rideIndex)
{
    tileElement->properties.track.ride_index = rideIndex;
}

uint8_t track_element_get_type(const rct_tile_element * tileElement)
{
    return tileElement->properties.track.type;
}

void track_element_set_type(rct_tile_element * tileElement, uint8_t type)
{
    tileElement->properties.track.type = type;
}

uint8_t track_element_get_door_a_state(const rct_tile_element * tileElement)
{
    return (tileElement->properties.track.colour & TRACK_ELEMENT_DOOR_A_MASK) >> 2;
}

uint8_t track_element_get_door_b_state(const rct_tile_element * tileElement)
{
    return (tileElement->properties.track.colour & TRACK_ELEMENT_DOOR_B_MASK) >> 5;
}
