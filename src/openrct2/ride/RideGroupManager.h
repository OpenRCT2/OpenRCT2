/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3
 *****************************************************************************/

#pragma once

#include <vector>

#include "../common.h"
#include "RideData.h"

#define MAX_RIDE_GROUPS_PER_RIDE_TYPE 2

extern const uint8 gRideCategories[RIDE_TYPE_COUNT];

struct RideGroup
{
    uint8 RideType;
    uint16 MaximumHeight;
    uint64 AvailableTrackPieces;
    rct_ride_name Naming;

    bool Equals(const RideGroup* otherRideGroup) const;
    bool IsInvented() const;
};

class RideGroupManager
{
public:
    static const RideGroup * GetRideGroup(const uint8 trackType, const rct_ride_entry * rideEntry);
    static bool RideTypeHasRideGroups(const uint8 trackType);
    static const RideGroup * RideGroupFind(const uint8 rideType, const uint8 index);

    static const std::vector<const char *> GetPreferredRideEntryOrder(const uint8 rideType);
    static sint32 VehiclePreferenceCompare(const uint8 rideType, const char * a, const char * b);
    static bool RideTypeIsIndependent(const uint8 rideType);
};
