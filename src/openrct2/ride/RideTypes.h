/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Identifiers.h"
#include "../object/ObjectTypes.h"

#include <cstdint>
#include <limits>
#include <type_traits>

struct Ride;

using ride_type_t = uint16_t;
static ride_type_t constexpr kRideTypeNull = 0xFF;

/**
 * Couples a ride type and subtype together.
 */
struct RideSelection
{
    ride_type_t Type = kRideTypeNull;
    ObjectEntryIndex EntryIndex = kObjectEntryIndexNull;

    bool operator==(const RideSelection& other) const
    {
        return Type == other.Type && EntryIndex == other.EntryIndex;
    }

    bool operator!=(const RideSelection& other) const
    {
        return !(*this == other);
    }
};
