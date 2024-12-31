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

namespace OpenRCT2::RCT12::Limits
{
    constexpr uint8_t kMaxRideObjects = 128;

    constexpr uint8_t kMaxRidesInPark = 255;
    constexpr uint8_t kMaxAwards = 4;
    constexpr uint8_t MaxNewsItems = 61;
    constexpr uint8_t kMaxStationsPerRide = 4;
    constexpr uint8_t kMaxPeepSpawns = 2;
    constexpr uint8_t kMaxParkEntrances = 4;
    // The number of elements in the patrol_areas array per staff member. Every bit in the array represents a 4x4 square.
    // In RCT1, that's an 8-bit array. 8 * 128 = 1024 bits, which is also the number of 4x4 squares on a 128x128 map.
    // For RCT2, it's a 32-bit array. 32 * 128 = 4096 bits, which is also the number of 4x4 squares on a 256x256 map.
    constexpr uint8_t kPatrolAreaSize = 128;
    constexpr uint8_t kStaffTypeCount = 4;
    constexpr uint8_t kNumColourSchemes = 4;
    constexpr uint8_t kMaxVehicleColours = 32;

    constexpr uint8_t kExpenditureTableMonthCount = 16;
    constexpr uint8_t kExpenditureTypeCount = 14;
    constexpr uint8_t kFinanceGraphSize = 128;

    constexpr uint16_t kMaxUserStrings = 1024;
    constexpr uint8_t kMaxUserStringLength = 32;

    constexpr uint8_t kMaxPeepThoughts = 5;

    constexpr uint16_t kRideMeasurementMaxItems = 4800;

    constexpr uint16_t const kMaxInversions = 31;
    constexpr uint16_t const kMaxGolfHoles = 31;
    constexpr uint16_t const kMaxHelices = 31;
    constexpr uint8_t kMaxElementHeight = 255;
    constexpr uint8_t kCustomerHistorySize = 10;

    constexpr uint8_t kMaxWaitingTime = 250;
} // namespace OpenRCT2::RCT12::Limits
