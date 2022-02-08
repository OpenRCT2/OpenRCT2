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
    constexpr const uint8_t MaxRideObjects = 128;

    constexpr const uint8_t MaxRidesInPark = 255;
    constexpr const uint8_t MaxAwards = 4;
    constexpr const uint8_t MaxNewsItems = 61;
    constexpr const uint8_t MaxStationsPerRide = 4;
    constexpr const uint8_t MaxPeepSpawns = 2;
    constexpr const uint8_t MaxParkEntrances = 4;
    // The number of elements in the patrol_areas array per staff member. Every bit in the array represents a 4x4 square.
    // In RCT1, that's an 8-bit array. 8 * 128 = 1024 bits, which is also the number of 4x4 squares on a 128x128 map.
    // For RCT2, it's a 32-bit array. 32 * 128 = 4096 bits, which is also the number of 4x4 squares on a 256x256 map.
    constexpr const uint8_t PatrolAreaSize = 128;
    constexpr const uint8_t StaffTypeCount = 4;
    constexpr const uint8_t NumColourSchemes = 4;
    constexpr const uint8_t MaxVehicleColours = 32;

    constexpr const uint8_t ExpenditureTableMonthCount = 16;
    constexpr const uint8_t ExpenditureTypeCount = 14;
    constexpr const uint8_t FinanceGraphSize = 128;

    constexpr const uint16_t MaxUserStrings = 1024;
    constexpr const uint8_t MaxUserStringLength = 32;

    constexpr const uint8_t MaxPeepThoughts = 5;

    constexpr const uint16_t RideMeasurementMaxItems = 4800;

    constexpr uint16_t const MaxInversions = 31;
    constexpr uint16_t const MaxGolfHoles = 31;
    constexpr uint16_t const MaxHelices = 31;
    constexpr const uint8_t MaxElementHeight = 255;
    constexpr const uint8_t CustomerHistorySize = 10;
} // namespace RCT12::Limits
