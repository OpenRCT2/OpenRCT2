/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../object/Object.h"

#include <cstdint>
#include <limits>
#include <type_traits>

enum class RideId : uint16_t;
struct Ride;

constexpr const RideId RIDE_ID_NULL = static_cast<RideId>(std::numeric_limits<std::underlying_type_t<RideId>>::max());

using ride_type_t = uint16_t;

/**
 * Couples a ride type and subtype together.
 */
struct RideSelection
{
    uint8_t Type;
    ObjectEntryIndex EntryIndex;

    bool operator==(const RideSelection& other) const
    {
        return Type == other.Type && EntryIndex == other.EntryIndex;
    }

    bool operator!=(const RideSelection& other) const
    {
        return !(*this == other);
    }
};
