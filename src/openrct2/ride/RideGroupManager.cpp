/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideGroupManager.h"

#include "../Context.h"
#include "../config/Config.h"
#include "../core/String.hpp"
#include "../localisation/StringIds.h"
#include "../management/Research.h"
#include "../object/ObjectManager.h"
#include "Ride.h"
#include "RideData.h"
#include "Track.h"
#include "TrackData.h"

#include <memory>
#include <vector>

static constexpr const RideGroup ride_group_corkscrew_rc = {
    /*.RideType =*/RIDE_TYPE_CORKSCREW_ROLLER_COASTER,
    /*.MaximumHeight =*/28,
    /*.AvailableTrackPieces =*/(1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_LIFT_HILL)
        | (1ULL << TRACK_FLAT_ROLL_BANKING) | (1ULL << TRACK_VERTICAL_LOOP) | (1ULL << TRACK_SLOPE)
        | (1ULL << TRACK_SLOPE_STEEP) | (1ULL << TRACK_SLOPE_CURVE) | (1ULL << TRACK_SLOPE_CURVE_STEEP) | (1ULL << TRACK_S_BEND)
        | (1ULL << TRACK_CURVE_SMALL) | (1ULL << TRACK_CURVE) | (1ULL << TRACK_HALF_LOOP) | (1ULL << TRACK_CORKSCREW)
        | (1ULL << TRACK_HELIX_SMALL) | (1ULL << TRACK_BRAKES) | (1ULL << TRACK_ON_RIDE_PHOTO) | (1ULL << TRACK_BLOCK_BRAKES)
        | (1ULL << TRACK_BOOSTER),
    /*.Naming =*/{ STR_CORKSCREW_RC_GROUP, STR_CORKSCREW_RC_GROUP_DESC },
    /*.Flags =*/0,
};

static constexpr const RideGroup ride_group_hypercoaster = {
    /*.RideType =*/RIDE_TYPE_CORKSCREW_ROLLER_COASTER,
    /*.MaximumHeight =*/55,
    /*.AvailableTrackPieces =*/(1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_LIFT_HILL)
        | (1ULL << TRACK_FLAT_ROLL_BANKING) | (1ULL << TRACK_SLOPE) | (1ULL << TRACK_SLOPE_STEEP) | (1ULL << TRACK_SLOPE_CURVE)
        | (1ULL << TRACK_SLOPE_CURVE_STEEP) | (1ULL << TRACK_S_BEND) | (1ULL << TRACK_CURVE_SMALL) | (1ULL << TRACK_CURVE)
        | (1ULL << TRACK_HELIX_SMALL) | (1ULL << TRACK_BRAKES) | (1ULL << TRACK_ON_RIDE_PHOTO) | (1ULL << TRACK_BLOCK_BRAKES)
        | (1ULL << TRACK_SLOPE_STEEP_LONG) | (1ULL << TRACK_BOOSTER),
    /*.Naming =*/{ STR_HYPERCOASTER_GROUP, STR_HYPERCOASTER_GROUP_DESC },
    /*.Flags =*/0,
};

static constexpr const RideGroup ride_group_car_ride = {
    /*.RideType =*/RIDE_TYPE_CAR_RIDE,
    /*.MaximumHeight =*/6,
    /*.AvailableTrackPieces =*/(1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_SLOPE)
        | (1ULL << TRACK_CURVE_VERY_SMALL) | (1ULL << TRACK_CURVE_SMALL) | (1ULL << TRACK_SPINNING_TUNNEL),
    /*.Naming =*/{ STR_CAR_RIDE_GROUP, STR_CAR_RIDE_GROUP_DESC },
    /*.Flags =*/RIDE_GROUP_FLAG_ALLOW_DOORS_ON_TRACK,
};

static constexpr const RideGroup ride_group_monster_trucks = {
    /*.RideType =*/RIDE_TYPE_CAR_RIDE,
    /*.MaximumHeight =*/18,
    /*.AvailableTrackPieces =*/(1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_SLOPE)
        | (1ULL << TRACK_SLOPE_STEEP) | (1ULL << TRACK_CURVE_VERY_SMALL) | (1ULL << TRACK_CURVE_SMALL)
        | (1ULL << TRACK_RAPIDS) | (1ULL << TRACK_SPINNING_TUNNEL),
    /*.Naming =*/{ STR_MONSTER_TRUCKS_GROUP, STR_MONSTER_TRUCKS_GROUP_DESC },
    /*.Flags =*/RIDE_GROUP_FLAG_ALLOW_DOORS_ON_TRACK,
};

static constexpr const RideGroup ride_group_steel_twister_rc = {
    /*.RideType =*/RIDE_TYPE_TWISTER_ROLLER_COASTER,
    /*.MaximumHeight =*/42,
    /*.AvailableTrackPieces =*/(1ULL << TRACK_FLAT) | (1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END)
        | (1ULL << TRACK_LIFT_HILL) | (1ULL << TRACK_FLAT_ROLL_BANKING) | (1ULL << TRACK_VERTICAL_LOOP) | (1ULL << TRACK_SLOPE)
        | (1ULL << TRACK_SLOPE_STEEP) | (1ULL << TRACK_SLOPE_CURVE) | (1ULL << TRACK_SLOPE_CURVE_STEEP) | (1ULL << TRACK_S_BEND)
        | (1ULL << TRACK_CURVE_SMALL) | (1ULL << TRACK_CURVE) | (1ULL << TRACK_HALF_LOOP) | (1ULL << TRACK_CORKSCREW)
        | (1ULL << TRACK_HELIX_SMALL) | (1ULL << TRACK_BRAKES) | (1ULL << TRACK_ON_RIDE_PHOTO) | (1ULL << TRACK_SLOPE_VERTICAL)
        | (1ULL << TRACK_BARREL_ROLL) | (1ULL << TRACK_POWERED_LIFT) | (1ULL << TRACK_HALF_LOOP_LARGE)
        | (1ULL << TRACK_SLOPE_CURVE_BANKED) | (1ULL << TRACK_BLOCK_BRAKES) | (1ULL << TRACK_SLOPE_ROLL_BANKING)
        | (1ULL << TRACK_SLOPE_STEEP_LONG) | (1ULL << TRACK_CURVE_VERTICAL) | (1ULL << TRACK_QUARTER_LOOP)
        | (1ULL << TRACK_BOOSTER),
    /*.Naming =*/{ STR_STEEL_TWISTER_GROUP, STR_STEEL_TWISTER_GROUP_DESC },
    /*.Flags =*/0,
};

static constexpr const RideGroup ride_group_hyper_twister = {
    /*.RideType =*/RIDE_TYPE_TWISTER_ROLLER_COASTER,
    /*.MaximumHeight =*/68,
    /*.AvailableTrackPieces =*/(1ULL << TRACK_FLAT) | (1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END)
        | (1ULL << TRACK_LIFT_HILL) | (1ULL << TRACK_FLAT_ROLL_BANKING) | (1ULL << TRACK_SLOPE) | (1ULL << TRACK_SLOPE_STEEP)
        | (1ULL << TRACK_SLOPE_CURVE) | (1ULL << TRACK_SLOPE_CURVE_STEEP) | (1ULL << TRACK_S_BEND) | (1ULL << TRACK_CURVE_SMALL)
        | (1ULL << TRACK_CURVE) | (1ULL << TRACK_HELIX_SMALL) | (1ULL << TRACK_BRAKES) | (1ULL << TRACK_ON_RIDE_PHOTO)
        | (1ULL << TRACK_SLOPE_CURVE_BANKED) | (1ULL << TRACK_BLOCK_BRAKES) | (1ULL << TRACK_SLOPE_ROLL_BANKING)
        | (1ULL << TRACK_SLOPE_STEEP_LONG) | (1ULL << TRACK_SLOPE_VERTICAL) | (1ULL << TRACK_CURVE_VERTICAL)
        | (1ULL << TRACK_POWERED_LIFT) | (1ULL << TRACK_BOOSTER),
    /*.Naming =*/{ STR_HYPER_TWISTER_GROUP, STR_HYPER_TWISTER_GROUP_DESC },
    /*.Flags =*/0,
};

static constexpr const RideGroup ride_group_junior_rc = {
    /*.RideType =*/RIDE_TYPE_JUNIOR_ROLLER_COASTER,
    /*.MaximumHeight =*/12,
    /*.AvailableTrackPieces =*/(1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_LIFT_HILL)
        | (1ULL << TRACK_LIFT_HILL_CURVE) | (1ULL << TRACK_FLAT_ROLL_BANKING) | (1ULL << TRACK_SLOPE)
        | (1ULL << TRACK_SLOPE_LONG) | (1ULL << TRACK_SLOPE_CURVE) | (1ULL << TRACK_S_BEND) | (1ULL << TRACK_CURVE_SMALL)
        | (1ULL << TRACK_CURVE) | (1ULL << TRACK_HELIX_SMALL) | (1ULL << TRACK_BRAKES) | (1ULL << TRACK_BLOCK_BRAKES)
        | (1ULL << TRACK_BOOSTER),
    /*.Naming =*/{ STR_JUNIOR_RC_GROUP, STR_JUNIOR_RC_GROUP_DESC },
    /*.Flags =*/RIDE_GROUP_FLAG_ALLOW_DOORS_ON_TRACK,
};

static constexpr const RideGroup ride_group_classic_mini_coaster = {
    /*.RideType =*/RIDE_TYPE_JUNIOR_ROLLER_COASTER,
    /*.MaximumHeight =*/15,
    /*.AvailableTrackPieces =*/(1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_LIFT_HILL)
        | (1ULL << TRACK_LIFT_HILL_CURVE) | (1ULL << TRACK_FLAT_ROLL_BANKING) | (1ULL << TRACK_SLOPE)
        | (1ULL << TRACK_SLOPE_STEEP) | (1ULL << TRACK_SLOPE_LONG) | (1ULL << TRACK_SLOPE_CURVE) | (1ULL << TRACK_S_BEND)
        | (1ULL << TRACK_CURVE_SMALL) | (1ULL << TRACK_CURVE) | (1ULL << TRACK_HELIX_SMALL) | (1ULL << TRACK_BRAKES)
        | (1ULL << TRACK_BLOCK_BRAKES) | (1ULL << TRACK_BOOSTER),
    /*.Naming =*/{ STR_CLASSIC_MINI_COASTER_GROUP, STR_CLASSIC_MINI_COASTER_GROUP_DESC },
    /*.Flags =*/RIDE_GROUP_FLAG_ALLOW_DOORS_ON_TRACK,
};

static constexpr const RideGroup ride_group_steel_wild_mouse = {
    /*.RideType =*/RIDE_TYPE_STEEL_WILD_MOUSE,
    /*.MaximumHeight =*/16,
    /*.AvailableTrackPieces =*/(1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_LIFT_HILL)
        | (1ULL << TRACK_LIFT_HILL_STEEP) | (1ULL << TRACK_SLOPE) | (1ULL << TRACK_SLOPE_STEEP) | (1ULL << TRACK_SLOPE_LONG)
        | (1ULL << TRACK_SLOPE_CURVE) | (1ULL << TRACK_CURVE_VERY_SMALL) | (1ULL << TRACK_CURVE_SMALL) | (1ULL << TRACK_BRAKES)
        | (1ULL << TRACK_BLOCK_BRAKES),
    /*.Naming =*/{ STR_RIDE_NAME_WILD_MOUSE, STR_RIDE_DESCRIPTION_WILD_MOUSE },
    /*.Flags =*/RIDE_GROUP_FLAG_ALLOW_DOORS_ON_TRACK,
};

static constexpr const RideGroup ride_group_spinning_wild_mouse = {
    /*.RideType =*/RIDE_TYPE_STEEL_WILD_MOUSE,
    /*.MaximumHeight =*/16,
    /*.AvailableTrackPieces =*/(1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_LIFT_HILL)
        | (1ULL << TRACK_SLOPE) | (1ULL << TRACK_SLOPE_LONG) | (1ULL << TRACK_CURVE_VERY_SMALL) | (1ULL << TRACK_CURVE_SMALL)
        | (1ULL << TRACK_BRAKES) | (1ULL << TRACK_BLOCK_BRAKES) | (1ULL << TRACK_ROTATION_CONTROL_TOGGLE),
    /*.Naming =*/{ STR_SPINNING_WILD_MOUSE_GROUP, STR_SPINNING_WILD_MOUSE_GROUP_DESC },
    /*.Flags =*/0,
};

static constexpr const RideGroup corkscrew_rc_groups[MAX_RIDE_GROUPS_PER_RIDE_TYPE] = {
    ride_group_corkscrew_rc,
    ride_group_hypercoaster,
};
static constexpr const RideGroup junior_rc_groups[MAX_RIDE_GROUPS_PER_RIDE_TYPE] = {
    ride_group_junior_rc,
    ride_group_classic_mini_coaster,
};
static constexpr const RideGroup car_ride_groups[MAX_RIDE_GROUPS_PER_RIDE_TYPE] = {
    ride_group_car_ride,
    ride_group_monster_trucks,
};
static constexpr const RideGroup twister_rc_groups[MAX_RIDE_GROUPS_PER_RIDE_TYPE] = {
    ride_group_steel_twister_rc,
    ride_group_hyper_twister,
};
static constexpr const RideGroup steel_wild_mouse_groups[MAX_RIDE_GROUPS_PER_RIDE_TYPE] = {
    ride_group_steel_wild_mouse,
    ride_group_spinning_wild_mouse,
};

bool RideGroup::Equals(const RideGroup* otherRideGroup) const
{
    return this->Naming.name == otherRideGroup->Naming.name && this->Naming.description == otherRideGroup->Naming.description;
}

bool RideGroup::IsInvented() const
{
    if (!ride_type_is_invented(this->RideType))
        return false;

    auto& objManager = OpenRCT2::GetContext()->GetObjectManager();
    auto& rideEntries = objManager.GetAllRideEntries(this->RideType);
    for (auto rideEntryIndex : rideEntries)
    {
        if (ride_entry_is_invented(rideEntryIndex))
        {
            auto rideEntry = get_ride_entry(rideEntryIndex);
            auto rideEntryRideGroup = RideGroupManager::GetRideGroup(this->RideType, rideEntry);
            if (this->Equals(rideEntryRideGroup))
            {
                // The ride entry is invented and belongs to the same ride group. This means the ride group is invented.
                return true;
            }
        }
    }

    return false;
}

const RideGroup* RideGroupManager::GetRideGroup(const uint8_t rideType, const rct_ride_entry* rideEntry)
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

const RideGroup* RideGroupManager::RideGroupFind(const uint8_t rideType, const uint8_t index)
{
    if (index >= MAX_RIDE_GROUPS_PER_RIDE_TYPE)
        return nullptr;

    switch (rideType)
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

const std::vector<const char*> RideGroupManager::GetPreferredRideEntryOrder(const uint8_t rideType)
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
int32_t RideGroupManager::VehiclePreferenceCompare(const uint8_t rideType, const char* a, const char* b)
{
    std::vector<const char*> rideEntryOrder = RideGroupManager::GetPreferredRideEntryOrder(rideType);
    for (const char* object : rideEntryOrder)
    {
        if (String::Equals(object, a, true))
        {
            return -1;
        }
        if (String::Equals(object, b, true))
        {
            return 1;
        }
    }
    return 0;
}
