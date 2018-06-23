/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <memory>
#include <vector>
#include "RideGroupManager.h"
#include "../config/Config.h"
#include "../core/String.hpp"

#include "../localisation/StringIds.h"
#include "../management/Research.h"
#include "Ride.h"
#include "RideData.h"
#include "Track.h"
#include "TrackData.h"

static constexpr const RideGroup ride_group_corkscrew_rc = {
    /*.RideType =*/ RIDE_TYPE_CORKSCREW_ROLLER_COASTER,
    /*.MaximumHeight =*/ 28,
    /*.AvailableTrackPieces =*/ (1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_LIFT_HILL) | (1ULL << TRACK_FLAT_ROLL_BANKING) | (1ULL << TRACK_VERTICAL_LOOP) | (1ULL << TRACK_SLOPE) | (1ULL << TRACK_SLOPE_STEEP) | (1ULL << TRACK_SLOPE_CURVE) | (1ULL << TRACK_SLOPE_CURVE_STEEP) | (1ULL << TRACK_S_BEND) | (1ULL << TRACK_CURVE_SMALL) | (1ULL << TRACK_CURVE) | (1ULL << TRACK_HALF_LOOP) | (1ULL << TRACK_CORKSCREW) | (1ULL << TRACK_HELIX_SMALL) | (1ULL << TRACK_BRAKES) | (1ULL << TRACK_ON_RIDE_PHOTO) | (1ULL << TRACK_BLOCK_BRAKES) | (1ULL << TRACK_BOOSTER),
    /*.Naming =*/ { STR_CORKSCREW_RC_GROUP, STR_CORKSCREW_RC_GROUP_DESC },
    /*.Flags =*/ 0,
};

static constexpr const RideGroup ride_group_hypercoaster = {
    /*.RideType =*/ RIDE_TYPE_CORKSCREW_ROLLER_COASTER,
    /*.MaximumHeight =*/ 45,
    /*.AvailableTrackPieces =*/ (1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_LIFT_HILL) | (1ULL << TRACK_FLAT_ROLL_BANKING) | (1ULL << TRACK_SLOPE) | (1ULL << TRACK_SLOPE_STEEP) | (1ULL << TRACK_SLOPE_CURVE) | (1ULL << TRACK_SLOPE_CURVE_STEEP) | (1ULL << TRACK_S_BEND) | (1ULL << TRACK_CURVE_SMALL) | (1ULL << TRACK_CURVE) | (1ULL << TRACK_HELIX_SMALL) | (1ULL << TRACK_BRAKES) | (1ULL << TRACK_ON_RIDE_PHOTO) | (1ULL << TRACK_BLOCK_BRAKES) | (1ULL << TRACK_SLOPE_STEEP_LONG),
    /*.Naming =*/ { STR_HYPERCOASTER_GROUP, STR_HYPERCOASTER_GROUP_DESC },
    /*.Flags =*/ 0,
};

static constexpr const RideGroup ride_group_car_ride = {
    /*.RideType =*/ RIDE_TYPE_CAR_RIDE,
    /*.MaximumHeight =*/ 6,
    /*.AvailableTrackPieces =*/ (1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_SLOPE) | (1ULL << TRACK_CURVE_VERY_SMALL) | (1ULL << TRACK_CURVE_SMALL) | (1ULL << TRACK_SPINNING_TUNNEL),
    /*.Naming =*/ { STR_CAR_RIDE_GROUP, STR_CAR_RIDE_GROUP_DESC },
    /*.Flags =*/ RIDE_GROUP_FLAG_ALLOW_DOORS_ON_TRACK,
};

static constexpr const RideGroup ride_group_monster_trucks = {
    /*.RideType =*/ RIDE_TYPE_CAR_RIDE,
    /*.MaximumHeight =*/ 18,
    /*.AvailableTrackPieces =*/ (1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_SLOPE) | (1ULL << TRACK_SLOPE_STEEP) | (1ULL << TRACK_CURVE_VERY_SMALL) | (1ULL << TRACK_CURVE_SMALL) | (1ULL << TRACK_RAPIDS),
    /*.Naming =*/ { STR_MONSTER_TRUCKS_GROUP, STR_MONSTER_TRUCKS_GROUP_DESC },
    /*.Flags =*/ RIDE_GROUP_FLAG_ALLOW_DOORS_ON_TRACK,
};

static constexpr const RideGroup ride_group_steel_twister_rc = {
    /*.RideType =*/ RIDE_TYPE_TWISTER_ROLLER_COASTER,
    /*.MaximumHeight =*/ 40,
    /*.AvailableTrackPieces =*/ (1ULL << TRACK_FLAT) | (1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_LIFT_HILL) | (1ULL << TRACK_FLAT_ROLL_BANKING) | (1ULL << TRACK_VERTICAL_LOOP) | (1ULL << TRACK_SLOPE) | (1ULL << TRACK_SLOPE_STEEP) | (1ULL << TRACK_SLOPE_CURVE) | (1ULL << TRACK_SLOPE_CURVE_STEEP) | (1ULL << TRACK_S_BEND) | (1ULL << TRACK_CURVE_SMALL) | (1ULL << TRACK_CURVE) | (1ULL << TRACK_HALF_LOOP) | (1ULL << TRACK_CORKSCREW) | (1ULL << TRACK_HELIX_SMALL) | (1ULL << TRACK_BRAKES) | (1ULL << TRACK_ON_RIDE_PHOTO) | (1ULL << TRACK_SLOPE_VERTICAL) | (1ULL << TRACK_BARREL_ROLL) | (1ULL << TRACK_POWERED_LIFT) | (1ULL << TRACK_HALF_LOOP_LARGE) | (1ULL << TRACK_SLOPE_CURVE_BANKED) | (1ULL << TRACK_BLOCK_BRAKES) | (1ULL << TRACK_SLOPE_ROLL_BANKING) | (1ULL << TRACK_SLOPE_STEEP_LONG) | (1ULL << TRACK_CURVE_VERTICAL) | (1ULL << TRACK_QUARTER_LOOP) | (1ULL << TRACK_BOOSTER),
    /*.Naming =*/ { STR_STEEL_TWISTER_GROUP, STR_STEEL_TWISTER_GROUP_DESC },
    /*.Flags =*/ 0,
};

static constexpr const RideGroup ride_group_hyper_twister = {
    /*.RideType =*/ RIDE_TYPE_TWISTER_ROLLER_COASTER,
    /*.MaximumHeight =*/ 54,
    /*.AvailableTrackPieces =*/ (1ULL << TRACK_FLAT) | (1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_LIFT_HILL) | (1ULL << TRACK_FLAT_ROLL_BANKING) | (1ULL << TRACK_SLOPE) | (1ULL << TRACK_SLOPE_STEEP) | (1ULL << TRACK_SLOPE_CURVE) | (1ULL << TRACK_SLOPE_CURVE_STEEP) | (1ULL << TRACK_S_BEND) | (1ULL << TRACK_CURVE_SMALL) | (1ULL << TRACK_CURVE) | (1ULL << TRACK_HELIX_SMALL) | (1ULL << TRACK_BRAKES) | (1ULL << TRACK_ON_RIDE_PHOTO) | (1ULL << TRACK_SLOPE_CURVE_BANKED) | (1ULL << TRACK_BLOCK_BRAKES) | (1ULL << TRACK_SLOPE_ROLL_BANKING) | (1ULL << TRACK_SLOPE_STEEP_LONG) | (1ULL << TRACK_SLOPE_VERTICAL) | (1ULL << TRACK_CURVE_VERTICAL),
    /*.Naming =*/ { STR_HYPER_TWISTER_GROUP, STR_HYPER_TWISTER_GROUP_DESC },
    /*.Flags =*/ 0,
};

static constexpr const RideGroup ride_group_junior_rc = {
    /*.RideType =*/ RIDE_TYPE_JUNIOR_ROLLER_COASTER,
    /*.MaximumHeight =*/ 12,
    /*.AvailableTrackPieces =*/ (1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_LIFT_HILL) | (1ULL << TRACK_LIFT_HILL_CURVE) | (1ULL << TRACK_FLAT_ROLL_BANKING) | (1ULL << TRACK_SLOPE) | (1ULL << TRACK_SLOPE_LONG) | (1ULL << TRACK_SLOPE_CURVE) | (1ULL << TRACK_S_BEND) | (1ULL << TRACK_CURVE_SMALL) | (1ULL << TRACK_CURVE) | (1ULL << TRACK_HELIX_SMALL) | (1ULL << TRACK_BRAKES) | (1ULL << TRACK_BLOCK_BRAKES) | (1ULL << TRACK_BOOSTER),
    /*.Naming =*/ { STR_JUNIOR_RC_GROUP, STR_JUNIOR_RC_GROUP_DESC },
    /*.Flags =*/ RIDE_GROUP_FLAG_ALLOW_DOORS_ON_TRACK,
};

static constexpr const RideGroup ride_group_classic_mini_coaster = {
    /*.RideType =*/ RIDE_TYPE_JUNIOR_ROLLER_COASTER,
    /*.MaximumHeight =*/ 15,
    /*.AvailableTrackPieces =*/ (1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_LIFT_HILL) | (1ULL << TRACK_LIFT_HILL_CURVE) | (1ULL << TRACK_FLAT_ROLL_BANKING) | (1ULL << TRACK_SLOPE) | (1ULL << TRACK_SLOPE_STEEP) | (1ULL << TRACK_SLOPE_LONG) | (1ULL << TRACK_SLOPE_CURVE) | (1ULL << TRACK_S_BEND) | (1ULL << TRACK_CURVE_SMALL) | (1ULL << TRACK_CURVE) | (1ULL << TRACK_HELIX_SMALL) | (1ULL << TRACK_BRAKES) | (1ULL << TRACK_BLOCK_BRAKES) | (1ULL << TRACK_BOOSTER),
    /*.Naming =*/ { STR_CLASSIC_MINI_COASTER_GROUP, STR_CLASSIC_MINI_COASTER_GROUP_DESC },
    /*.Flags =*/ RIDE_GROUP_FLAG_ALLOW_DOORS_ON_TRACK,
};

static constexpr const RideGroup ride_group_steel_wild_mouse = {
    /*.RideType =*/ RIDE_TYPE_STEEL_WILD_MOUSE,
    /*.MaximumHeight =*/ 16,
    /*.AvailableTrackPieces =*/ (1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_LIFT_HILL) | (1ULL << TRACK_LIFT_HILL_STEEP) | (1ULL << TRACK_SLOPE) | (1ULL << TRACK_SLOPE_STEEP) | (1ULL << TRACK_SLOPE_LONG) | (1ULL << TRACK_SLOPE_CURVE) | (1ULL << TRACK_CURVE_VERY_SMALL) | (1ULL << TRACK_CURVE_SMALL) | (1ULL << TRACK_BRAKES) | (1ULL << TRACK_BLOCK_BRAKES),
    /*.Naming =*/ { STR_RIDE_NAME_WILD_MOUSE, STR_RIDE_DESCRIPTION_WILD_MOUSE },
    /*.Flags =*/ RIDE_GROUP_FLAG_ALLOW_DOORS_ON_TRACK,
};

static constexpr const RideGroup ride_group_spinning_wild_mouse = {
    /*.RideType =*/ RIDE_TYPE_STEEL_WILD_MOUSE,
    /*.MaximumHeight =*/ 16,
    /*.AvailableTrackPieces =*/ (1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_LIFT_HILL) | (1ULL << TRACK_SLOPE) | (1ULL << TRACK_SLOPE_LONG) | (1ULL << TRACK_SLOPE_CURVE) | (1ULL << TRACK_CURVE_VERY_SMALL) | (1ULL << TRACK_CURVE_SMALL) | (1ULL << TRACK_BRAKES) | (1ULL << TRACK_BLOCK_BRAKES) | (1ULL << TRACK_ROTATION_CONTROL_TOGGLE),
    /*.Naming =*/ { STR_SPINNING_WILD_MOUSE_GROUP, STR_SPINNING_WILD_MOUSE_GROUP_DESC },
    /*.Flags =*/ 0,
};

static constexpr const RideGroup corkscrew_rc_groups[MAX_RIDE_GROUPS_PER_RIDE_TYPE] = { ride_group_corkscrew_rc, ride_group_hypercoaster };
static constexpr const RideGroup junior_rc_groups[MAX_RIDE_GROUPS_PER_RIDE_TYPE] = { ride_group_junior_rc, ride_group_classic_mini_coaster };
static constexpr const RideGroup car_ride_groups[MAX_RIDE_GROUPS_PER_RIDE_TYPE] = { ride_group_car_ride, ride_group_monster_trucks };
static constexpr const RideGroup twister_rc_groups[MAX_RIDE_GROUPS_PER_RIDE_TYPE] = { ride_group_steel_twister_rc, ride_group_hyper_twister };
static constexpr const RideGroup steel_wild_mouse_groups[MAX_RIDE_GROUPS_PER_RIDE_TYPE] = { ride_group_steel_wild_mouse, ride_group_spinning_wild_mouse };

bool RideGroup::Equals(const RideGroup* otherRideGroup) const
{
    return
        this->Naming.name == otherRideGroup->Naming.name &&
        this->Naming.description == otherRideGroup->Naming.description;
}

bool RideGroup::IsInvented() const
{
    if (!ride_type_is_invented(this->RideType))
        return false;

    uint8_t *rideEntryIndexPtr = get_ride_entry_indices_for_ride_type(this->RideType);

    while (*rideEntryIndexPtr != RIDE_ENTRY_INDEX_NULL)
    {
        uint8_t rideEntryIndex = *rideEntryIndexPtr++;

        if (!ride_entry_is_invented(rideEntryIndex))
            continue;

        rct_ride_entry *rideEntry = get_ride_entry(rideEntryIndex);
        const RideGroup * rideEntryRideGroup = RideGroupManager::GetRideGroup(this->RideType, rideEntry);

        if (!this->Equals(rideEntryRideGroup))
            continue;

        // The ride entry is invented and belongs to the same ride group. This means the ride group is invented.
        return true;
    }

    return false;
}

const RideGroup * RideGroupManager::GetRideGroup(const uint8_t rideType, const rct_ride_entry * rideEntry)
{
    switch (rideType)
    {
    case RIDE_TYPE_CORKSCREW_ROLLER_COASTER:
        if (ride_entry_get_supported_track_pieces(rideEntry) & (1ULL << TRACK_VERTICAL_LOOP))
            return &ride_group_corkscrew_rc;
        else
            return &ride_group_hypercoaster;
    case RIDE_TYPE_JUNIOR_ROLLER_COASTER:
        if (ride_entry_get_supported_track_pieces(rideEntry) & (1ULL << TRACK_SLOPE_STEEP))
            return &ride_group_classic_mini_coaster;
        else
            return &ride_group_junior_rc;
    case RIDE_TYPE_CAR_RIDE:
        if (ride_entry_get_supported_track_pieces(rideEntry) & (1ULL << TRACK_SLOPE_STEEP))
            return &ride_group_monster_trucks;
        else
            return &ride_group_car_ride;
    case RIDE_TYPE_TWISTER_ROLLER_COASTER:
        if (!(rideEntry->flags & RIDE_ENTRY_FLAG_NO_INVERSIONS))
            return &ride_group_steel_twister_rc;
        else
            return &ride_group_hyper_twister;
    case RIDE_TYPE_STEEL_WILD_MOUSE:
        if (ride_entry_get_supported_track_pieces(rideEntry) & (1ULL << TRACK_SLOPE_STEEP))
            return &ride_group_steel_wild_mouse;
        else
            return &ride_group_spinning_wild_mouse;
    default:
        return nullptr;
    }
}

bool RideGroupManager::RideTypeHasRideGroups(const uint8_t rideType)
{
    switch (rideType)
    {
        case RIDE_TYPE_CORKSCREW_ROLLER_COASTER:
        case RIDE_TYPE_JUNIOR_ROLLER_COASTER:
        case RIDE_TYPE_CAR_RIDE:
        case RIDE_TYPE_TWISTER_ROLLER_COASTER:
        case RIDE_TYPE_STEEL_WILD_MOUSE:
            return true;
        default:
            return false;
    }
}

const RideGroup * RideGroupManager::RideGroupFind(const uint8_t rideType, const uint8_t index)
{
    if (index >= MAX_RIDE_GROUPS_PER_RIDE_TYPE)
        return nullptr;

    switch(rideType)
    {
    case RIDE_TYPE_CORKSCREW_ROLLER_COASTER:
        return &corkscrew_rc_groups[index];
    case RIDE_TYPE_JUNIOR_ROLLER_COASTER:
        return &junior_rc_groups[index];
    case RIDE_TYPE_CAR_RIDE:
        return &car_ride_groups[index];
    case RIDE_TYPE_TWISTER_ROLLER_COASTER:
        return &twister_rc_groups[index];
    case RIDE_TYPE_STEEL_WILD_MOUSE:
        return &steel_wild_mouse_groups[index];
    default:
        return nullptr;
    }
}

const std::vector<const char *> RideGroupManager::GetPreferredRideEntryOrder(const uint8_t rideType)
{
    // clang-format off
    static const std::vector<const char *> preferredRideEntryOrder[] =
    {
        { "SPDRCR  " },                                                             // RIDE_TYPE_SPIRAL_ROLLER_COASTER
        { "TOGST   " },                                                             // RIDE_TYPE_STAND_UP_ROLLER_COASTER
        { "ARRSW1  ", "VEKVAMP ", "ARRSW2  " },                                     // RIDE_TYPE_SUSPENDED_SWINGING_COASTER
        { "NEMT    " },                                                             // RIDE_TYPE_INVERTED_ROLLER_COASTER
        { "ZLDB    ", "ZLOG    ", "ZPANDA  " },                                     // RIDE_TYPE_JUNIOR_ROLLER_COASTER
        { "NRL     ", "NRL2    ", "AML1    ", "TRAM1   " },                         // RIDE_TYPE_MINIATURE_RAILWAY
        { "MONO1   ", "MONO2   ", "MONO3   " },                                     // RIDE_TYPE_MONORAIL
        { "BATFL   ", "SKYTR   "},                                                  // RIDE_TYPE_MINI_SUSPENDED_COASTER
        { "RBOAT   ", "BBOAT   ", "CBOAT   ", "SWANS   ", "TRIKE   ", "JSKI    " }, // RIDE_TYPE_BOAT_HIRE
        { "WMOUSE  ", "WMMINE  "},                                                  // RIDE_TYPE_WOODEN_WILD_MOUSE
        { "STEEP1  ", "STEEP2  ", "SBOX    " },                                     // RIDE_TYPE_STEEPLECHASE
        { "SPCAR   ", "RCR     ", "TRUCK1  ", "VCR     ", "CTCAR   " },             // RIDE_TYPE_CAR_RIDE
        { "SSC1    " },                                                             // RIDE_TYPE_LAUNCHED_FREEFALL
        { "BOB1    ", "INTBOB  " },                                                 // RIDE_TYPE_BOBSLEIGH_COASTER
        { "OBS1    ", "OBS2    " },                                                 // RIDE_TYPE_OBSERVATION_TOWER
        { "SCHT1   " },                                                             // RIDE_TYPE_LOOPING_ROLLER_COASTER
        { "DING1   " },                                                             // RIDE_TYPE_DINGHY_SLIDE
        { "AMT1    " },                                                             // RIDE_TYPE_MINE_TRAIN_COASTER
        { "CLIFT1  ", "CLIFT2  " },                                                 // RIDE_TYPE_CHAIRLIFT
        { "ARRT1   ", "ARRT2   " },                                                 // RIDE_TYPE_CORKSCREW_ROLLER_COASTER
        { },                                                                        // RIDE_TYPE_MAZE
        { },                                                                        // RIDE_TYPE_SPIRAL_SLIDE
        { "KART1   " },                                                             // RIDE_TYPE_GO_KARTS
        { "LFB1    " },                                                             // RIDE_TYPE_LOG_FLUME
        { "RAPBOAT " },                                                             // RIDE_TYPE_RIVER_RAPIDS
        { },                                                                        // RIDE_TYPE_DODGEMS
        { },                                                                        // RIDE_TYPE_SWINGING_SHIP
        { },                                                                        // RIDE_TYPE_SWINGING_INVERTER_SHIP
        { },                                                                        // RIDE_TYPE_FOOD_STALL
        { },                                                                        // RIDE_TYPE_1D
        { },                                                                        // RIDE_TYPE_DRINK_STALL
        { },                                                                        // RIDE_TYPE_1F
        { },                                                                        // RIDE_TYPE_SHOP
        { },                                                                        // RIDE_TYPE_MERRY_GO_ROUND
        { },                                                                        // RIDE_TYPE_22
        { },                                                                        // RIDE_TYPE_INFORMATION_KIOSK
        { },                                                                        // RIDE_TYPE_TOILETS
        { },                                                                        // RIDE_TYPE_FERRIS_WHEEL
        { },                                                                        // RIDE_TYPE_MOTION_SIMULATOR
        { },                                                                        // RIDE_TYPE_3D_CINEMA
        { },                                                                        // RIDE_TYPE_TOP_SPIN
        { },                                                                        // RIDE_TYPE_SPACE_RINGS
        { "REVF1   " },                                                             // RIDE_TYPE_REVERSE_FREEFALL_COASTER
        { "LIFT1   " },                                                             // RIDE_TYPE_LIFT
        { "BMVD    " },                                                             // RIDE_TYPE_VERTICAL_DROP_ROLLER_COASTER
        { },                                                                        // RIDE_TYPE_CASH_MACHINE
        { },                                                                        // RIDE_TYPE_TWIST
        { },                                                                        // RIDE_TYPE_HAUNTED_HOUSE
        { },                                                                        // RIDE_TYPE_FIRST_AID
        { },                                                                        // RIDE_TYPE_CIRCUS
        { "GTC     ", "HMCAR   " },                                                 // RIDE_TYPE_GHOST_TRAIN
        { "BMSD    ", "BMSU    ", "BMFL    ", "BMRB    ", "GOLTR   " },             // RIDE_TYPE_TWISTER_ROLLER_COASTER
        { "PTCT1   ", "MFT     ", "PTCT2   " },                                     // RIDE_TYPE_WOODEN_ROLLER_COASTER
        { "SFRIC1  " },                                                             // RIDE_TYPE_SIDE_FRICTION_ROLLER_COASTER
        { "SMC1    ", "SMC2    ", "WMSPIN  " },                                     // RIDE_TYPE_STEEL_WILD_MOUSE
        { "ARRX    " },                                                             // RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER
        { },                                                                        // RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER_ALT
        { "BMAIR   " },                                                             // RIDE_TYPE_FLYING_ROLLER_COASTER
        { },                                                                        // RIDE_TYPE_FLYING_ROLLER_COASTER_ALT
        { "VREEL   " },                                                             // RIDE_TYPE_VIRGINIA_REEL
        { "SPBOAT  " },                                                             // RIDE_TYPE_SPLASH_BOATS
        { "HELICAR " },                                                             // RIDE_TYPE_MINI_HELICOPTERS
        { "VEKST   " },                                                             // RIDE_TYPE_LAY_DOWN_ROLLER_COASTER
        { "SMONO   " },                                                             // RIDE_TYPE_SUSPENDED_MONORAIL
        { },                                                                        // RIDE_TYPE_LAY_DOWN_ROLLER_COASTER_ALT
        { "REVCAR  " },                                                             // RIDE_TYPE_REVERSER_ROLLER_COASTER
        { "UTCAR   ", "UTCARR  " },                                                 // RIDE_TYPE_HEARTLINE_TWISTER_COASTER
        { },                                                                        // RIDE_TYPE_MINI_GOLF
        { "INTST   " },                                                             // RIDE_TYPE_GIGA_COASTER
        { "GDROP1  " },                                                             // RIDE_TYPE_ROTO_DROP
        { },                                                                        // RIDE_TYPE_FLYING_SAUCERS
        { },                                                                        // RIDE_TYPE_CROOKED_HOUSE
        { "MONBK   " },                                                             // RIDE_TYPE_MONORAIL_CYCLES
        { "SLCT    ", "SLCFO    ", "VEKDV   " },                                    // RIDE_TYPE_COMPACT_INVERTED_COASTER
        { "CSTBOAT " },                                                             // RIDE_TYPE_WATER_COASTER
        { "THCAR   " },                                                             // RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER
        { "IVMC1   " },                                                             // RIDE_TYPE_INVERTED_HAIRPIN_COASTER
        { },                                                                        // RIDE_TYPE_MAGIC_CARPET
        { "SUBMAR  " },                                                             // RIDE_TYPE_SUBMARINE_RIDE
        { "RFTBOAT " },                                                             // RIDE_TYPE_RIVER_RAFTS
        { },                                                                        // RIDE_TYPE_50
        { },                                                                        // RIDE_TYPE_ENTERPRISE
        { },                                                                        // RIDE_TYPE_52
        { },                                                                        // RIDE_TYPE_53
        { },                                                                        // RIDE_TYPE_54
        { },                                                                        // RIDE_TYPE_55
        { "INTINV  " },                                                             // RIDE_TYPE_INVERTED_IMPULSE_COASTER
        { "WCATC   ", "RCKC     ", "JSTAR1  " },                                    // RIDE_TYPE_MINI_ROLLER_COASTER
        { "PMT1    " },                                                             // RIDE_TYPE_MINE_RIDE
        { },                                                                        // RIDE_TYPE_59
        { "PREMT1  " },                                                             // RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER
    };
    // clang-format on
    return preferredRideEntryOrder[rideType];
}

/**
 * This function keeps a list of the preferred vehicle for every generic track
 * type, out of the available vehicle types in the current game. It determines
 * which picture is shown on the new ride tab and which train type is selected
 * by default.
 */
int32_t RideGroupManager::VehiclePreferenceCompare(const uint8_t rideType, const char * a, const char * b)
{
    std::vector<const char *> rideEntryOrder = RideGroupManager::GetPreferredRideEntryOrder(rideType);
    for (const char * object : rideEntryOrder)
    {
        if (String::Equals(object, a, true))
        {
            return -1;
        }
        if (String::Equals(object, b, true))
        {
            return  1;
        }
    }
    return 0;
}

bool RideGroupManager::RideTypeIsIndependent(const uint8_t rideType)
{
    switch (rideType)
    {
    case RIDE_TYPE_SPIRAL_ROLLER_COASTER:
    case RIDE_TYPE_STAND_UP_ROLLER_COASTER:
    case RIDE_TYPE_SUSPENDED_SWINGING_COASTER:
    case RIDE_TYPE_INVERTED_ROLLER_COASTER:
    case RIDE_TYPE_JUNIOR_ROLLER_COASTER:
    case RIDE_TYPE_MINIATURE_RAILWAY:
    case RIDE_TYPE_MONORAIL:
    case RIDE_TYPE_MINI_SUSPENDED_COASTER:
    case RIDE_TYPE_BOAT_HIRE:
    case RIDE_TYPE_WOODEN_WILD_MOUSE:
    case RIDE_TYPE_STEEPLECHASE:
    case RIDE_TYPE_CAR_RIDE:
    case RIDE_TYPE_LAUNCHED_FREEFALL:
    case RIDE_TYPE_BOBSLEIGH_COASTER:
    case RIDE_TYPE_OBSERVATION_TOWER:
    case RIDE_TYPE_LOOPING_ROLLER_COASTER:
    case RIDE_TYPE_DINGHY_SLIDE:
    case RIDE_TYPE_MINE_TRAIN_COASTER:
    case RIDE_TYPE_CHAIRLIFT:
    case RIDE_TYPE_CORKSCREW_ROLLER_COASTER:
    case RIDE_TYPE_GO_KARTS:
    case RIDE_TYPE_LOG_FLUME:
    case RIDE_TYPE_RIVER_RAPIDS:
    case RIDE_TYPE_REVERSE_FREEFALL_COASTER:
    case RIDE_TYPE_LIFT:
    case RIDE_TYPE_VERTICAL_DROP_ROLLER_COASTER:
    case RIDE_TYPE_GHOST_TRAIN:
    case RIDE_TYPE_TWISTER_ROLLER_COASTER:
    case RIDE_TYPE_WOODEN_ROLLER_COASTER:
    case RIDE_TYPE_SIDE_FRICTION_ROLLER_COASTER:
    case RIDE_TYPE_STEEL_WILD_MOUSE:
    case RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER:
    case RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER_ALT:
    case RIDE_TYPE_FLYING_ROLLER_COASTER:
    case RIDE_TYPE_FLYING_ROLLER_COASTER_ALT:
    case RIDE_TYPE_VIRGINIA_REEL:
    case RIDE_TYPE_SPLASH_BOATS:
    case RIDE_TYPE_MINI_HELICOPTERS:
    case RIDE_TYPE_LAY_DOWN_ROLLER_COASTER:
    case RIDE_TYPE_SUSPENDED_MONORAIL:
    case RIDE_TYPE_LAY_DOWN_ROLLER_COASTER_ALT:
    case RIDE_TYPE_REVERSER_ROLLER_COASTER:
    case RIDE_TYPE_HEARTLINE_TWISTER_COASTER:
    case RIDE_TYPE_GIGA_COASTER:
    case RIDE_TYPE_ROTO_DROP:
    case RIDE_TYPE_MONORAIL_CYCLES:
    case RIDE_TYPE_COMPACT_INVERTED_COASTER:
    case RIDE_TYPE_WATER_COASTER:
    case RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER:
    case RIDE_TYPE_INVERTED_HAIRPIN_COASTER:
    case RIDE_TYPE_SUBMARINE_RIDE:
    case RIDE_TYPE_RIVER_RAFTS:
    case RIDE_TYPE_INVERTED_IMPULSE_COASTER:
    case RIDE_TYPE_MINI_ROLLER_COASTER:
    case RIDE_TYPE_MINE_RIDE:
    case RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER:
        return false;
    case RIDE_TYPE_MAZE:
    case RIDE_TYPE_SPIRAL_SLIDE:
    case RIDE_TYPE_DODGEMS:
    case RIDE_TYPE_SWINGING_SHIP:
    case RIDE_TYPE_SWINGING_INVERTER_SHIP:
    case RIDE_TYPE_FOOD_STALL:
    case RIDE_TYPE_1D:
    case RIDE_TYPE_DRINK_STALL:
    case RIDE_TYPE_1F:
    case RIDE_TYPE_SHOP:
    case RIDE_TYPE_MERRY_GO_ROUND:
    case RIDE_TYPE_22:
    case RIDE_TYPE_INFORMATION_KIOSK:
    case RIDE_TYPE_TOILETS:
    case RIDE_TYPE_FERRIS_WHEEL:
    case RIDE_TYPE_MOTION_SIMULATOR:
    case RIDE_TYPE_3D_CINEMA:
    case RIDE_TYPE_TOP_SPIN:
    case RIDE_TYPE_SPACE_RINGS:
    case RIDE_TYPE_CASH_MACHINE:
    case RIDE_TYPE_TWIST:
    case RIDE_TYPE_HAUNTED_HOUSE:
    case RIDE_TYPE_FIRST_AID:
    case RIDE_TYPE_CIRCUS:
    case RIDE_TYPE_MINI_GOLF:
    case RIDE_TYPE_FLYING_SAUCERS:
    case RIDE_TYPE_CROOKED_HOUSE:
    case RIDE_TYPE_MAGIC_CARPET:
    case RIDE_TYPE_50:
    case RIDE_TYPE_ENTERPRISE:
    case RIDE_TYPE_52:
    case RIDE_TYPE_53:
    case RIDE_TYPE_54:
    case RIDE_TYPE_55:
    case RIDE_TYPE_59:
        return true;
    }

    assert(false);
    return true;
}

const uint8_t gRideCategories[] = {
    RIDE_CATEGORY_ROLLERCOASTER, // Spiral Roller coaster
    RIDE_CATEGORY_ROLLERCOASTER, // Stand Up Coaster
    RIDE_CATEGORY_ROLLERCOASTER, // Suspended Swinging
    RIDE_CATEGORY_ROLLERCOASTER, // Inverted
    RIDE_CATEGORY_ROLLERCOASTER, // Steel Mini Coaster
    RIDE_CATEGORY_TRANSPORT,     // Mini Railway
    RIDE_CATEGORY_TRANSPORT,     // Monorail
    RIDE_CATEGORY_ROLLERCOASTER, // Mini Suspended Coaster
    RIDE_CATEGORY_WATER,         // Boat Hire
    RIDE_CATEGORY_ROLLERCOASTER, // Wooden Wild Mine/Mouse
    RIDE_CATEGORY_ROLLERCOASTER, // Steeplechase/Motorbike/Soap Box Derby
    RIDE_CATEGORY_GENTLE,        // Car Ride
    RIDE_CATEGORY_THRILL,        // Launched Freefall
    RIDE_CATEGORY_ROLLERCOASTER, // Bobsleigh Coaster
    RIDE_CATEGORY_GENTLE,        // Observation Tower
    RIDE_CATEGORY_ROLLERCOASTER, // Looping Roller Coaster
    RIDE_CATEGORY_WATER,         // Dinghy Slide
    RIDE_CATEGORY_ROLLERCOASTER, // Mine Train Coaster
    RIDE_CATEGORY_TRANSPORT,     // Chairlift
    RIDE_CATEGORY_ROLLERCOASTER, // Corkscrew Roller Coaster
    RIDE_CATEGORY_GENTLE,        // Maze
    RIDE_CATEGORY_GENTLE,        // Spiral Slide
    RIDE_CATEGORY_THRILL,        // Go Karts
    RIDE_CATEGORY_WATER,         // Log Flume
    RIDE_CATEGORY_WATER,         // River Rapids
    RIDE_CATEGORY_GENTLE,        // Dodgems
    RIDE_CATEGORY_THRILL,        // Pirate Ship
    RIDE_CATEGORY_THRILL,        // Swinging Inverter Ship
    RIDE_CATEGORY_SHOP,          // Food Stall
    255,                         // (none)
    RIDE_CATEGORY_SHOP,          // Drink Stall
    255,                         // (none)
    RIDE_CATEGORY_SHOP,          // Shop (all types)
    RIDE_CATEGORY_GENTLE,        // Merry Go Round
    255,                         // Unknown
    RIDE_CATEGORY_SHOP,          // Information Kiosk
    RIDE_CATEGORY_SHOP,          // Bathroom
    RIDE_CATEGORY_GENTLE,        // Ferris Wheel
    RIDE_CATEGORY_THRILL,        // Motion Simulator
    RIDE_CATEGORY_THRILL,        // 3D Cinema
    RIDE_CATEGORY_THRILL,        // Top Spin
    RIDE_CATEGORY_GENTLE,        // Space Rings
    RIDE_CATEGORY_ROLLERCOASTER, // Reverse Freefall Coaster
    RIDE_CATEGORY_TRANSPORT,     // Lift
    RIDE_CATEGORY_ROLLERCOASTER, // Vertical Drop Roller Coaster
    RIDE_CATEGORY_SHOP,          // ATM
    RIDE_CATEGORY_THRILL,        // Twist
    RIDE_CATEGORY_GENTLE,        // Haunted House
    RIDE_CATEGORY_SHOP,          // First Aid
    RIDE_CATEGORY_GENTLE,        // Circus Show
    RIDE_CATEGORY_GENTLE,        // Ghost Train
    RIDE_CATEGORY_ROLLERCOASTER, // Twister Roller Coaster
    RIDE_CATEGORY_ROLLERCOASTER, // Wooden Roller Coaster
    RIDE_CATEGORY_ROLLERCOASTER, // Side-Friction Roller Coaster
    RIDE_CATEGORY_ROLLERCOASTER, // Wild Mouse
    RIDE_CATEGORY_ROLLERCOASTER, // Multi Dimension Coaster
    255,                         // (none)
    RIDE_CATEGORY_ROLLERCOASTER, // Flying Roller Coaster
    255,                         // (none)
    RIDE_CATEGORY_ROLLERCOASTER, // Virginia Reel
    RIDE_CATEGORY_WATER,         // Splash Boats
    RIDE_CATEGORY_GENTLE,        // Mini Helicopters
    RIDE_CATEGORY_ROLLERCOASTER, // Lay-down Roller Coaster
    RIDE_CATEGORY_TRANSPORT,     // Suspended Monorail
    255,                         // (none)
    RIDE_CATEGORY_ROLLERCOASTER, // Reverser Roller Coaster
    RIDE_CATEGORY_ROLLERCOASTER, // Heartline Twister Roller Coaster
    RIDE_CATEGORY_GENTLE,        // Mini Golf
    RIDE_CATEGORY_ROLLERCOASTER, // Giga Coaster
    RIDE_CATEGORY_THRILL,        // Roto-Drop
    RIDE_CATEGORY_GENTLE,        // Flying Saucers
    RIDE_CATEGORY_GENTLE,        // Crooked House
    RIDE_CATEGORY_GENTLE,        // Monorail Cycles
    RIDE_CATEGORY_ROLLERCOASTER, // Compact Inverted Coaster
    RIDE_CATEGORY_ROLLERCOASTER, // Water Coaster
    RIDE_CATEGORY_ROLLERCOASTER, // Air Powered Vertical Coaster
    RIDE_CATEGORY_ROLLERCOASTER, // Inverted Hairpin Coaster
    RIDE_CATEGORY_THRILL,        // Magic Carpet
    RIDE_CATEGORY_WATER,         // Submarine Ride
    RIDE_CATEGORY_WATER,         // River Rafts
    255,                         // (none)
    RIDE_CATEGORY_THRILL,        // Enterprise
    255,                         // (none)
    255,                         // (none)
    255,                         // (none)
    255,                         // (none)
    RIDE_CATEGORY_ROLLERCOASTER, // Inverted Impulse Coaster
    RIDE_CATEGORY_ROLLERCOASTER, // Mini Roller Coaster
    RIDE_CATEGORY_ROLLERCOASTER, // Mine Ride
    255,                         // 59 Unknown Ride
    RIDE_CATEGORY_ROLLERCOASTER  // LIM Launched Roller Coaster
};
