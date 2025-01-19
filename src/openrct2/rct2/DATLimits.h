/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once
#include <cstdint>

namespace OpenRCT2::RCT2::ObjectLimits
{
    constexpr uint8_t kMaxCategoriesPerRide = 2;
    constexpr uint8_t kMaxRideTypesPerRideEntry = 3;
    // The max number of different types of car.
    // Examples of cars here are the locomotive, tender and carriage of the Miniature Railway.
    constexpr uint8_t kMaxCarTypesPerRideEntry = 4;
    constexpr uint8_t kMaxShopItemsPerRideEntry = 2;
} // namespace OpenRCT2::RCT2::ObjectLimits
