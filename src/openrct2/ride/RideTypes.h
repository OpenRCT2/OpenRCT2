/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../object/Object.h"

#include <cstdint>
#include <limits>

using ride_id_t = uint16_t;
struct Ride;

constexpr const ride_id_t RIDE_ID_NULL = std::numeric_limits<ride_id_t>::max();

enum class RideType : uint8_t
{
    SPIRAL_ROLLER_COASTER = 0,
    STAND_UP_ROLLER_COASTER,
    SUSPENDED_SWINGING_COASTER,
    INVERTED_ROLLER_COASTER,
    JUNIOR_ROLLER_COASTER,
    MINIATURE_RAILWAY,
    MONORAIL,
    MINI_SUSPENDED_COASTER,
    BOAT_HIRE,
    WOODEN_WILD_MOUSE,
    STEEPLECHASE = 10,
    CAR_RIDE,
    LAUNCHED_FREEFALL,
    BOBSLEIGH_COASTER,
    OBSERVATION_TOWER,
    LOOPING_ROLLER_COASTER,
    DINGHY_SLIDE,
    MINE_TRAIN_COASTER,
    CHAIRLIFT,
    CORKSCREW_ROLLER_COASTER,
    MAZE = 20,
    SPIRAL_SLIDE,
    GO_KARTS,
    LOG_FLUME,
    RIVER_RAPIDS,
    DODGEMS,
    SWINGING_SHIP,
    SWINGING_INVERTER_SHIP,
    FOOD_STALL,
    _1D,
    DRINK_STALL = 30,
    _1F,
    SHOP,
    MERRY_GO_ROUND,
    _22,
    INFORMATION_KIOSK,
    TOILETS,
    FERRIS_WHEEL,
    MOTION_SIMULATOR,
    _3D_CINEMA,
    TOP_SPIN = 40,
    SPACE_RINGS,
    REVERSE_FREEFALL_COASTER,
    LIFT,
    VERTICAL_DROP_ROLLER_COASTER,
    CASH_MACHINE,
    TWIST,
    HAUNTED_HOUSE,
    FIRST_AID,
    CIRCUS,
    GHOST_TRAIN = 50,
    TWISTER_ROLLER_COASTER,
    WOODEN_ROLLER_COASTER,
    SIDE_FRICTION_ROLLER_COASTER,
    STEEL_WILD_MOUSE,
    MULTI_DIMENSION_ROLLER_COASTER,
    MULTI_DIMENSION_ROLLER_COASTER_ALT,
    FLYING_ROLLER_COASTER,
    FLYING_ROLLER_COASTER_ALT,
    VIRGINIA_REEL,
    SPLASH_BOATS = 60,
    MINI_HELICOPTERS,
    LAY_DOWN_ROLLER_COASTER,
    SUSPENDED_MONORAIL,
    LAY_DOWN_ROLLER_COASTER_ALT,
    REVERSER_ROLLER_COASTER,
    HEARTLINE_TWISTER_COASTER,
    MINI_GOLF,
    GIGA_COASTER,
    ROTO_DROP,
    FLYING_SAUCERS = 70,
    CROOKED_HOUSE,
    MONORAIL_CYCLES,
    COMPACT_INVERTED_COASTER,
    WATER_COASTER,
    AIR_POWERED_VERTICAL_COASTER,
    INVERTED_HAIRPIN_COASTER,
    MAGIC_CARPET,
    SUBMARINE_RIDE,
    RIVER_RAFTS,
    _50 = 80,
    ENTERPRISE,
    _52,
    _53,
    _54,
    _55,
    INVERTED_IMPULSE_COASTER,
    MINI_ROLLER_COASTER,
    MINE_RIDE,
    _59,
    LIM_LAUNCHED_ROLLER_COASTER = 90,
    HYPERCOASTER,
    HYPER_TWISTER,
    MONSTER_TRUCKS,
    SPINNING_WILD_MOUSE,
    CLASSIC_MINI_ROLLER_COASTER,
    HYBRID_COASTER,
    SINGLE_RAIL_ROLLER_COASTER,

    COUNT,
    RIDE_TYPE_NULL = 0xFF,
};

/**
 * Couples a ride type and subtype together.
 */
struct RideSelection
{
    RideType Type;
    ObjectEntryIndex EntryIndex;

    bool operator==(const RideSelection& other) const
    {
        return Type == other.Type && EntryIndex == other.EntryIndex;
    }

    bool operator!=(const RideSelection& other) const
    {
        return !(*this == other);
    }
};
