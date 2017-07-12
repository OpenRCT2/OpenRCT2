#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include <memory>
#include "RideGroupManager.h"
#include "../config/Config.h"

extern "C"
{
    #include "../localisation/string_ids.h"
    #include "../management/research.h"
    #include "ride.h"
    #include "ride_data.h"
    #include "track.h"
    #include "track_data.h"
}

class RideGroupManager final : public IRideGroupManager
{
public:
    ride_group ride_group_corkscrew_rc = {
        /*.track_type =*/ RIDE_TYPE_CORKSCREW_ROLLER_COASTER,
        /*.maximum_height =*/ 28,
        /*.available_track_pieces =*/ (1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_LIFT_HILL) | (1ULL << TRACK_FLAT_ROLL_BANKING) | (1ULL << TRACK_VERTICAL_LOOP) | (1ULL << TRACK_SLOPE) | (1ULL << TRACK_SLOPE_STEEP) | (1ULL << TRACK_SLOPE_CURVE) | (1ULL << TRACK_SLOPE_CURVE_STEEP) | (1ULL << TRACK_S_BEND) | (1ULL << TRACK_CURVE_SMALL) | (1ULL << TRACK_CURVE) | (1ULL << TRACK_HALF_LOOP) | (1ULL << TRACK_CORKSCREW) | (1ULL << TRACK_HELIX_SMALL) | (1ULL << TRACK_BRAKES) | (1ULL << TRACK_ON_RIDE_PHOTO) | (1ULL << TRACK_BLOCK_BRAKES) | (1ULL << TRACK_BOOSTER),
        /*.naming =*/ { STR_CORKSCREW_RC_GROUP, STR_CORKSCREW_RC_GROUP_DESC },
    };

    ride_group ride_group_hypercoaster = {
        /*.track_type =*/ RIDE_TYPE_CORKSCREW_ROLLER_COASTER,
        /*.maximum_height =*/ 45,
        /*.available_track_pieces =*/ (1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_LIFT_HILL) | (1ULL << TRACK_FLAT_ROLL_BANKING) | (1ULL << TRACK_SLOPE) | (1ULL << TRACK_SLOPE_STEEP) | (1ULL << TRACK_SLOPE_CURVE) | (1ULL << TRACK_SLOPE_CURVE_STEEP) | (1ULL << TRACK_S_BEND) | (1ULL << TRACK_CURVE_SMALL) | (1ULL << TRACK_CURVE) | (1ULL << TRACK_HELIX_SMALL) | (1ULL << TRACK_BRAKES) | (1ULL << TRACK_ON_RIDE_PHOTO) | (1ULL << TRACK_BLOCK_BRAKES) | (1ULL << TRACK_SLOPE_STEEP_LONG),
        /*.naming =*/ { STR_HYPERCOASTER_GROUP, STR_HYPERCOASTER_GROUP_DESC },
    };

    ride_group ride_group_car_ride = {
        /*.track_type =*/ RIDE_TYPE_CAR_RIDE,
        /*.maximum_height =*/ 6,
        /*.available_track_pieces =*/ (1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_SLOPE) | (1ULL << TRACK_CURVE_VERY_SMALL) | (1ULL << TRACK_CURVE_SMALL) | (1ULL << TRACK_SPINNING_TUNNEL),
        /*.naming =*/ { STR_CAR_RIDE_GROUP, STR_CAR_RIDE_GROUP_DESC },
    };

    ride_group ride_group_monster_trucks = {
        /*.track_type =*/ RIDE_TYPE_CAR_RIDE,
        /*.maximum_height =*/ 18,
        /*.available_track_pieces =*/ (1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_SLOPE) | (1ULL << TRACK_SLOPE_STEEP) | (1ULL << TRACK_CURVE_VERY_SMALL) | (1ULL << TRACK_CURVE_SMALL) | (1ULL << TRACK_RAPIDS),
        /*.naming =*/ { STR_MONSTER_TRUCKS_GROUP, STR_MONSTER_TRUCKS_GROUP_DESC },
    };

    ride_group ride_group_steel_twister_rc = {
        /*.track_type =*/ RIDE_TYPE_TWISTER_ROLLER_COASTER,
        /*.maximum_height =*/ 34,
        /*.available_track_pieces =*/ (1ULL << TRACK_FLAT) | (1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_LIFT_HILL) | (1ULL << TRACK_FLAT_ROLL_BANKING) | (1ULL << TRACK_VERTICAL_LOOP) | (1ULL << TRACK_SLOPE) | (1ULL << TRACK_SLOPE_STEEP) | (1ULL << TRACK_SLOPE_CURVE) | (1ULL << TRACK_SLOPE_CURVE_STEEP) | (1ULL << TRACK_S_BEND) | (1ULL << TRACK_CURVE_SMALL) | (1ULL << TRACK_CURVE) | (1ULL << TRACK_HALF_LOOP) | (1ULL << TRACK_CORKSCREW) | (1ULL << TRACK_HELIX_SMALL) | (1ULL << TRACK_BRAKES) | (1ULL << TRACK_ON_RIDE_PHOTO) | (1ULL << TRACK_SLOPE_VERTICAL) | (1ULL << TRACK_BARREL_ROLL) | (1ULL << TRACK_POWERED_LIFT) | (1ULL << TRACK_HALF_LOOP_LARGE) | (1ULL << TRACK_SLOPE_CURVE_BANKED) | (1ULL << TRACK_BLOCK_BRAKES) | (1ULL << TRACK_SLOPE_ROLL_BANKING) | (1ULL << TRACK_SLOPE_STEEP_LONG) | (1ULL << TRACK_CURVE_VERTICAL) | (1ULL << TRACK_QUARTER_LOOP) | (1ULL << TRACK_BOOSTER),
        /*.naming =*/ { STR_STEEL_TWISTER_GROUP, STR_STEEL_TWISTER_GROUP_DESC },
    };

    ride_group ride_group_hyper_twister = {
        /*.track_type =*/ RIDE_TYPE_TWISTER_ROLLER_COASTER,
        /*.maximum_height =*/ 54,
        /*.available_track_pieces =*/ (1ULL << TRACK_FLAT) | (1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_LIFT_HILL) | (1ULL << TRACK_FLAT_ROLL_BANKING) | (1ULL << TRACK_SLOPE) | (1ULL << TRACK_SLOPE_STEEP) | (1ULL << TRACK_SLOPE_CURVE) | (1ULL << TRACK_SLOPE_CURVE_STEEP) | (1ULL << TRACK_S_BEND) | (1ULL << TRACK_CURVE_SMALL) | (1ULL << TRACK_CURVE) | (1ULL << TRACK_HELIX_SMALL) | (1ULL << TRACK_BRAKES) | (1ULL << TRACK_ON_RIDE_PHOTO) | (1ULL << TRACK_POWERED_LIFT) | (1ULL << TRACK_SLOPE_CURVE_BANKED) | (1ULL << TRACK_BLOCK_BRAKES) | (1ULL << TRACK_SLOPE_ROLL_BANKING) | (1ULL << TRACK_SLOPE_STEEP_LONG),
        /*.naming =*/ { STR_HYPER_TWISTER_GROUP, STR_HYPER_TWISTER_GROUP_DESC },
    };

    ride_group ride_group_junior_rc = {
        /*.track_type =*/ RIDE_TYPE_JUNIOR_ROLLER_COASTER,
        /*.maximum_height =*/ 12,
        /*.available_track_pieces =*/ (1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_LIFT_HILL) | (1ULL << TRACK_LIFT_HILL_CURVE) | (1ULL << TRACK_FLAT_ROLL_BANKING) | (1ULL << TRACK_SLOPE) | (1ULL << TRACK_SLOPE_LONG) | (1ULL << TRACK_SLOPE_CURVE) | (1ULL << TRACK_S_BEND) | (1ULL << TRACK_CURVE_SMALL) | (1ULL << TRACK_CURVE) | (1ULL << TRACK_HELIX_SMALL) | (1ULL << TRACK_BRAKES) | (1ULL << TRACK_BLOCK_BRAKES) | (1ULL << TRACK_BOOSTER),
        /*.naming =*/ { STR_JUNIOR_RC_GROUP, STR_JUNIOR_RC_GROUP_DESC },
    };

    ride_group ride_group_midi_coaster = {
        /*.track_type =*/ RIDE_TYPE_JUNIOR_ROLLER_COASTER,
        /*.maximum_height =*/ 15,
        /*.available_track_pieces =*/ (1ULL << TRACK_STRAIGHT) | (1ULL << TRACK_STATION_END) | (1ULL << TRACK_LIFT_HILL) | (1ULL << TRACK_LIFT_HILL_CURVE) | (1ULL << TRACK_FLAT_ROLL_BANKING) | (1ULL << TRACK_SLOPE) | (1ULL << TRACK_SLOPE_STEEP) | (1ULL << TRACK_SLOPE_LONG) | (1ULL << TRACK_SLOPE_CURVE) | (1ULL << TRACK_S_BEND) | (1ULL << TRACK_CURVE_SMALL) | (1ULL << TRACK_CURVE) | (1ULL << TRACK_HELIX_SMALL) | (1ULL << TRACK_BRAKES) | (1ULL << TRACK_BLOCK_BRAKES) | (1ULL << TRACK_BOOSTER),
        /*.naming =*/ { STR_MIDI_COASTER_GROUP, STR_MIDI_COASTER_GROUP_DESC },
    };

    ride_group corkscrew_rc_groups[MAX_RIDE_GROUPS_PER_RIDE_TYPE] = { ride_group_corkscrew_rc, ride_group_hypercoaster };
    ride_group junior_rc_groups[MAX_RIDE_GROUPS_PER_RIDE_TYPE] = { ride_group_junior_rc, ride_group_midi_coaster };
    ride_group car_ride_groups[MAX_RIDE_GROUPS_PER_RIDE_TYPE] = { ride_group_car_ride, ride_group_monster_trucks };
    ride_group twister_rc_groups[MAX_RIDE_GROUPS_PER_RIDE_TYPE] = { ride_group_steel_twister_rc, ride_group_hyper_twister };

    const ride_group * GetRideGroup(uint8 trackType, rct_ride_entry * rideEntry) const override
    {
        switch (trackType) {
            case RIDE_TYPE_CORKSCREW_ROLLER_COASTER:
                if (rideEntry->enabledTrackPieces & (1ULL << TRACK_VERTICAL_LOOP))
                    return (ride_group *) &ride_group_corkscrew_rc;
                else
                    return (ride_group *) &ride_group_hypercoaster;
            case RIDE_TYPE_JUNIOR_ROLLER_COASTER:
                if (ride_entry_get_supported_track_pieces(rideEntry) & (1ULL << TRACK_SLOPE_STEEP))
                    return (ride_group *) &ride_group_midi_coaster;
                else
                    return (ride_group *) &ride_group_junior_rc;
            case RIDE_TYPE_CAR_RIDE:
                if (rideEntry->enabledTrackPieces & (1ULL << TRACK_SLOPE_STEEP))
                    return (ride_group *) &ride_group_monster_trucks;
                else
                    return (ride_group *) &ride_group_car_ride;
            case RIDE_TYPE_TWISTER_ROLLER_COASTER:
                if (rideEntry->enabledTrackPieces & (1ULL << TRACK_VERTICAL_LOOP))
                    return (ride_group *) &ride_group_steel_twister_rc;
                else
                    return (ride_group *) &ride_group_hyper_twister;
            default:
                return NULL;
        }
    }

    bool TrackTypeHasRideGroups(uint8 trackType) const override
    {
        if (!gConfigInterface.select_by_track_type) {
            return false;
        }

        switch (trackType)
        {
            case RIDE_TYPE_CORKSCREW_ROLLER_COASTER:
            case RIDE_TYPE_JUNIOR_ROLLER_COASTER:
            case RIDE_TYPE_CAR_RIDE:
            case RIDE_TYPE_TWISTER_ROLLER_COASTER:
                return true;
            default:
                return false;
        }
    }

    ride_group * RideGroupFind(uint8 rideType, uint8 index) const override
    {
        ride_group * rideGroup;

        switch(rideType)
        {
            case RIDE_TYPE_CORKSCREW_ROLLER_COASTER:
                rideGroup = (ride_group *) &corkscrew_rc_groups[index];
                break;
            case RIDE_TYPE_JUNIOR_ROLLER_COASTER:
                rideGroup = (ride_group *) &junior_rc_groups[index];
                break;
            case RIDE_TYPE_CAR_RIDE:
                rideGroup = (ride_group *) &car_ride_groups[index];
                break;
            case RIDE_TYPE_TWISTER_ROLLER_COASTER:
                rideGroup = (ride_group *) &twister_rc_groups[index];
                break;
            default:
                return NULL;
        }

        return rideGroup;
    }

    bool RideGroupsAreEqual(const ride_group * a, const ride_group * b) const override
    {
        if (a != NULL && b != NULL && (a->naming.name == b->naming.name && a->naming.description == b->naming.description))
        {
            return true;
        }
        return false;
    }

    bool RideGroupIsInvented(const ride_group * rideGroup) const override
    {
        if (!ride_type_is_invented(rideGroup->track_type))
            return false;

        uint8 *rideEntryIndexPtr = get_ride_entry_indices_for_ride_type(rideGroup->track_type);

        while (*rideEntryIndexPtr != RIDE_ENTRY_INDEX_NULL)
        {
            uint8 rideEntryIndex = *rideEntryIndexPtr++;

            if (!ride_entry_is_invented(rideEntryIndex))
                continue;

            rct_ride_entry *rideEntry = get_ride_entry(rideEntryIndex);
            const ride_group * rideEntryRideGroup = GetRideGroup(rideGroup->track_type, rideEntry);

            if (!RideGroupsAreEqual(rideGroup, rideEntryRideGroup))
                continue;

            // The ride entry is invented and belongs to the same ride group. This means the ride group is invented.
            return true;
        }

        return false;
    }
};

static std::unique_ptr<RideGroupManager> _rideGroupManager = std::unique_ptr<RideGroupManager>(new RideGroupManager());
IRideGroupManager * GetRideGroupManager()
{
    return _rideGroupManager.get();
}

extern "C"
{
    const ride_group * get_ride_group(uint8 trackType, rct_ride_entry * rideEntry)
    {
        const IRideGroupManager * rideGroupManager = GetRideGroupManager();
        return rideGroupManager->GetRideGroup(trackType, rideEntry);
    }

    bool track_type_has_ride_groups(uint8 trackType)
    {
        const IRideGroupManager * rideGroupManager = GetRideGroupManager();
        return rideGroupManager->TrackTypeHasRideGroups(trackType);
    }

    ride_group * ride_group_find(uint8 rideType, uint8 index)
    {
        const IRideGroupManager * rideGroupManager = GetRideGroupManager();
        return rideGroupManager->RideGroupFind(rideType, index);
    }

    bool ride_groups_are_equal(const ride_group * a, const ride_group * b)
    {
        const IRideGroupManager * rideGroupManager = GetRideGroupManager();
        return rideGroupManager->RideGroupsAreEqual(a, b);
    }
    bool ride_group_is_invented(const ride_group * rideGroup)
    {
        const IRideGroupManager * rideGroupManager = GetRideGroupManager();
        return rideGroupManager->RideGroupIsInvented(rideGroup);
    }
}
