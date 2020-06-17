/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
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
        | (1ULL << TRACK_SLOPE_STEEP_LONG),
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
        | (1ULL << TRACK_SLOPE_STEEP) | (1ULL << TRACK_CURVE_VERY_SMALL) | (1ULL << TRACK_CURVE_SMALL) | (1ULL << TRACK_RAPIDS),
    /*.Naming =*/{ STR_MONSTER_TRUCKS_GROUP, STR_MONSTER_TRUCKS_GROUP_DESC },
    /*.Flags =*/RIDE_GROUP_FLAG_ALLOW_DOORS_ON_TRACK,
};

static constexpr const RideGroup ride_group_steel_twister_rc = {
    /*.RideType =*/RIDE_TYPE_TWISTER_ROLLER_COASTER,
    /*.MaximumHeight =*/40,
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
    /*.MaximumHeight =*/54,
    /*.AvailableTrackPieces =*/(1ULL << TRACK_FLAT) | (1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END)
        | (1ULL << TRACK_LIFT_HILL) | (1ULL << TRACK_FLAT_ROLL_BANKING) | (1ULL << TRACK_SLOPE) | (1ULL << TRACK_SLOPE_STEEP)
        | (1ULL << TRACK_SLOPE_CURVE) | (1ULL << TRACK_SLOPE_CURVE_STEEP) | (1ULL << TRACK_S_BEND) | (1ULL << TRACK_CURVE_SMALL)
        | (1ULL << TRACK_CURVE) | (1ULL << TRACK_HELIX_SMALL) | (1ULL << TRACK_BRAKES) | (1ULL << TRACK_ON_RIDE_PHOTO)
        | (1ULL << TRACK_SLOPE_CURVE_BANKED) | (1ULL << TRACK_BLOCK_BRAKES) | (1ULL << TRACK_SLOPE_ROLL_BANKING)
        | (1ULL << TRACK_SLOPE_STEEP_LONG) | (1ULL << TRACK_SLOPE_VERTICAL) | (1ULL << TRACK_CURVE_VERTICAL),
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
    return this->Naming.Name == otherRideGroup->Naming.Name && this->Naming.Description == otherRideGroup->Naming.Description;
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
