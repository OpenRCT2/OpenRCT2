/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <vector>

#include "../common.h"
#include "RideData.h"

#define MAX_RIDE_GROUPS_PER_RIDE_TYPE 2

extern const uint8_t gRideCategories[RIDE_TYPE_COUNT];

struct RideGroup
{
    uint8_t RideType;
    uint16_t MaximumHeight;
    uint64_t AvailableTrackPieces;
    rct_ride_name Naming;
    uint8_t Flags;

    bool Equals(const RideGroup* otherRideGroup) const;
    bool IsInvented() const;
};

class RideGroupManager
{
public:
    static const RideGroup * GetRideGroup(const uint8_t trackType, const rct_ride_entry * rideEntry);
    static bool RideTypeHasRideGroups(const uint8_t trackType);
    static const RideGroup * RideGroupFind(const uint8_t rideType, const uint8_t index);

    static const std::vector<const char *> GetPreferredRideEntryOrder(const uint8_t rideType);
    static int32_t VehiclePreferenceCompare(const uint8_t rideType, const char * a, const char * b);
    static bool RideTypeIsIndependent(const uint8_t rideType);
};

enum RideGroupFlags : uint8_t
{
    RIDE_GROUP_FLAG_ALLOW_DOORS_ON_TRACK = 0b00000001,
};
