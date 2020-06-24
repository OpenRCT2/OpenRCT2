/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "RideData.h"

#include <vector>

#define MAX_RIDE_GROUPS_PER_RIDE_TYPE 2

struct RideGroup
{
    uint8_t RideType;
    uint16_t MaximumHeight;
    uint64_t AvailableTrackPieces;
    RideNaming Naming;
    uint8_t Flags;

    bool Equals(const RideGroup* otherRideGroup) const;
    bool IsInvented() const;
};

class RideGroupManager
{
public:
    static const RideGroup* GetRideGroup(const uint8_t trackType, const rct_ride_entry* rideEntry);
    /** Will fall back to 0 if there is none found. */
    static uint8_t GetRideGroupIndex(const uint8_t trackType, const rct_ride_entry* rideEntry);
    static const RideGroup* RideGroupFind(const uint8_t rideType, const uint8_t index);
};

enum RideGroupFlags : uint8_t
{
    RIDE_GROUP_FLAG_ALLOW_DOORS_ON_TRACK = 0b00000001,
};
