/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once
#include "../rct12/Limits.h"

namespace RCT2::Limits
{
    using namespace RCT12::Limits;
    constexpr const uint8_t RCT2_MAX_STAFF = 200;
    constexpr const uint8_t RCT2_MAX_BANNERS_IN_PARK = 250;
    constexpr const uint8_t RCT2_MAX_VEHICLES_PER_RIDE = 31;
    constexpr const uint8_t RCT2_MAX_CARS_PER_TRAIN = 32;
    constexpr const uint8_t RCT2_MAX_CATEGORIES_PER_RIDE = 2;
    constexpr const uint8_t RCT2_MAX_RIDE_TYPES_PER_RIDE_ENTRY = 3;
    constexpr const uint8_t RCT2_MAX_VEHICLES_PER_RIDE_ENTRY = 4;
    constexpr const uint8_t RCT2_DOWNTIME_HISTORY_SIZE = 8;
    constexpr const uint8_t RCT2_CUSTOMER_HISTORY_SIZE = 10;
    constexpr const uint16_t RCT2_MAX_SPRITES = 10000;
    constexpr const uint32_t RCT2_MAX_TILE_ELEMENTS = 0x30000;
    constexpr const uint16_t RCT2_MAX_ANIMATED_OBJECTS = 2000;
    constexpr const uint8_t RCT2_MAX_RESEARCHED_RIDE_TYPE_QUADS = 8;  // With 32 bits per uint32_t, this means there is room for
                                                                      // 256 types.
    constexpr const uint8_t RCT2_MAX_RESEARCHED_RIDE_ENTRY_QUADS = 8; // With 32 bits per uint32_t, this means there is room for
                                                                      // 256 entries.
    constexpr const uint8_t RCT2_MAX_RESEARCHED_SCENERY_ITEM_QUADS = 56;
    constexpr const uint16_t RCT2_MAX_RESEARCHED_SCENERY_ITEMS = (RCT2_MAX_RESEARCHED_SCENERY_ITEM_QUADS * 32); // There are 32
                                                                                                                // bits per
                                                                                                                // quad.
    constexpr const uint16_t RCT2_MAX_RESEARCH_ITEMS = 500;

    constexpr uint16_t TD6MaxTrackElements = 8192;

    constexpr const uint8_t RCT2_MAX_SMALL_SCENERY_OBJECTS = 252;
    constexpr const uint8_t RCT2_MAX_LARGE_SCENERY_OBJECTS = 128;
    constexpr const uint8_t RCT2_MAX_WALL_SCENERY_OBJECTS = 128;
    constexpr const uint8_t RCT2_MAX_BANNER_OBJECTS = 32;
    constexpr const uint8_t RCT2_MAX_PATH_OBJECTS = 16;
    constexpr const uint8_t RCT2_MAX_PATH_ADDITION_OBJECTS = 15;
    constexpr const uint8_t RCT2_MAX_SCENERY_GROUP_OBJECTS = 19;
    constexpr const uint8_t RCT2_MAX_PARK_ENTRANCE_OBJECTS = 1;
    constexpr const uint8_t RCT2_MAX_WATER_OBJECTS = 1;
    constexpr const uint8_t RCT2_MAX_SCENARIO_TEXT_OBJECTS = 1;
    constexpr const uint8_t RCT2_RIDE_TYPE_COUNT = 91;
    constexpr const uint16_t RCT2_MAXIMUM_MAP_SIZE_TECHNICAL = 256;
} // namespace RCT2::Limits
