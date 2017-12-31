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

extern const uint8 gRideCategories[RIDE_TYPE_COUNT];

typedef struct RideGroup
{
    uint8 RideType;
    uint16 MaximumHeight;
    uint64 AvailableTrackPieces;
    rct_ride_name Naming;
} RideGroup;

#ifdef __cplusplus
class RideGroupManager
{
    public:
    static const RideGroup * GetRideGroup(const uint8 trackType, const rct_ride_entry * rideEntry);
    static bool RideTypeHasRideGroups(const uint8 trackType);
    static const RideGroup * RideGroupFind(const uint8 rideType, const uint8 index);
    static bool RideGroupsAreEqual(const RideGroup * a, const RideGroup * b);
    static bool RideGroupIsInvented(const RideGroup * rideGroup);

    static const std::vector<const char *> GetPreferredRideEntryOrder(const uint8 rideType);
    static sint32 VehiclePreferenceCompare(const uint8 rideType, const char * a, const char * b);
    static bool RideTypeIsIndependent(const uint8 rideType);
};
#endif
