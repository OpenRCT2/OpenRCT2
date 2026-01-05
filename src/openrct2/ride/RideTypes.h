/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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

struct RideFilters
{
    uint8_t category[2];
    ride_type_t ride_type;
};

/**
 * Couples a ride type and subtype together.
 */
struct RideSelection
{
    ride_type_t Type = kRideTypeNull;
    OpenRCT2::ObjectEntryIndex EntryIndex = OpenRCT2::kObjectEntryIndexNull;

    bool operator==(const RideSelection& other) const
    {
        return Type == other.Type && EntryIndex == other.EntryIndex;
    }

    bool operator!=(const RideSelection& other) const
    {
        return !(*this == other);
    }
};
