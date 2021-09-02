/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once
#include <cstdint>

namespace RCT12::Limits
{
    constexpr const uint8_t RCT12_MAX_RIDE_OBJECTS = 128;

    constexpr const uint8_t RCT12_MAX_RIDES_IN_PARK = 255;
    constexpr const uint8_t RCT12_MAX_AWARDS = 4;
    constexpr const uint8_t RCT12_MAX_NEWS_ITEMS = 61;
    constexpr const uint8_t RCT12_MAX_STATIONS_PER_RIDE = 4;
    constexpr const uint8_t RCT12_MAX_PEEP_SPAWNS = 2;
    constexpr const uint8_t RCT12_MAX_PARK_ENTRANCES = 4;
    // The number of elements in the patrol_areas array per staff member. Every bit in the array represents a 4x4 square.
    // In RCT1, that's an 8-bit array. 8 * 128 = 1024 bits, which is also the number of 4x4 squares on a 128x128 map.
    // For RCT2, it's a 32-bit array. 32 * 128 = 4096 bits, which is also the number of 4x4 squares on a 256x256 map.
    constexpr const uint8_t RCT12_PATROL_AREA_SIZE = 128;
    constexpr const uint8_t RCT12_STAFF_TYPE_COUNT = 4;
    constexpr const uint8_t RCT12_NUM_COLOUR_SCHEMES = 4;
    constexpr const uint8_t RCT12_MAX_VEHICLE_COLOURS = 32;

    constexpr const uint8_t RCT12_EXPENDITURE_TABLE_MONTH_COUNT = 16;
    constexpr const uint8_t RCT12_EXPENDITURE_TYPE_COUNT = 14;
    constexpr const uint8_t RCT12_FINANCE_GRAPH_SIZE = 128;

    constexpr const uint16_t RCT12_MAX_USER_STRINGS = 1024;
    constexpr const uint8_t RCT12_USER_STRING_MAX_LENGTH = 32;

    constexpr const uint8_t RCT12_PEEP_MAX_THOUGHTS = 5;

    constexpr const uint16_t RCT12_RIDE_MEASUREMENT_MAX_ITEMS = 4800;

    constexpr uint16_t const RCT12_MAX_INVERSIONS = 31;
    constexpr uint16_t const RCT12_MAX_GOLF_HOLES = 31;
    constexpr uint16_t const RCT12_MAX_HELICES = 31;
    constexpr const uint8_t RCT12_MAX_ELEMENT_HEIGHT = 255;
}
