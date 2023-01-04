/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once
#include <cstdint>

namespace RCT2::ObjectLimits
{
    constexpr const uint8_t MaxCategoriesPerRide = 2;
    constexpr const uint8_t MaxRideTypesPerRideEntry = 3;
    // The max number of different types of car.
    // Examples of cars here are the locomotive, tender and carriage of the Miniature Railway.
    constexpr const uint8_t MaxCarTypesPerRideEntry = 4;
    constexpr const uint8_t MaxShopItemsPerRideEntry = 2;
} // namespace RCT2::ObjectLimits
