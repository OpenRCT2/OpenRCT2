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

#pragma once

#ifdef __cplusplus
#include <vector>

extern "C"
{
#endif
    #include "../common.h"
    #include "ride_data.h"
#ifdef __cplusplus
}
#endif

#define MAX_RIDE_GROUPS_PER_RIDE_TYPE 2

typedef struct ride_group {
    uint8 ride_type;
    uint16 maximum_height;
    uint64 available_track_pieces;
    rct_ride_name naming;
} ride_group;

#ifdef __cplusplus
interface IRideGroupManager
{
    virtual ~IRideGroupManager() { }

    virtual const ride_group * GetRideGroup(uint8 trackType, rct_ride_entry * rideEntry) const abstract;
    virtual bool RideTypeHasRideGroups(uint8 trackType) const abstract;
    virtual ride_group * RideGroupFind(uint8 rideType, uint8 index) const abstract;
    virtual bool RideGroupsAreEqual(const ride_group * a, const ride_group * b) const abstract;
    virtual bool RideGroupIsInvented(const ride_group * rideGroup) const abstract;

    virtual const std::vector<const char *> GetPreferedRideEntryOrder(uint8 rideType) const abstract;
};


IRideGroupManager * GetRideGroupManager();

extern "C"
{
#endif
    const ride_group * get_ride_group(uint8 rideType, rct_ride_entry * rideEntry);
    bool ride_type_has_ride_groups(uint8 rideType);
    ride_group * ride_group_find(uint8 rideType, uint8 index);
    bool ride_groups_are_equal(const ride_group * a, const ride_group * b);
    bool ride_group_is_invented(const ride_group * rideGroup);
    sint32 vehicle_preference_compare(uint8 rideType, const char * a, const char * b);
#ifdef __cplusplus
}
#endif
