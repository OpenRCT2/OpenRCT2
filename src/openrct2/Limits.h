/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once
#include "rct2/Limits.h"

namespace OpenRCT2::Limits
{
    constexpr const uint16_t MaxRidesInPark = 1000;
    constexpr const uint16_t MaxStationsPerRide = 255;
    // constexpr const uint16_t RideMeasurementMaxItems = 4800; // Is part of Measurement struct
    using RCT12::Limits::CustomerHistorySize;
    using RCT12::Limits::MaxGolfHoles;
    using RCT12::Limits::MaxHelices;
    using RCT12::Limits::MaxInversions;
    constexpr const uint16_t MaxTrainsPerRide = 255;
    constexpr const uint16_t MaxCarsPerTrain = 255;
    constexpr const uint16_t MaxVehicleColours = MaxTrainsPerRide; // this should really be MaxTrainsPerRide * MaxCarsPerTrain
    constexpr const uint8_t MaxCircuitsPerRide = 20;
    using RCT12::Limits::NumColourSchemes;
    using RCT2::Limits::DowntimeHistorySize;
    using RCT12::Limits::MaxAwards;
    // constexpr const uint8_t MaxNewsItems = 61;
    // constexpr const uint8_t MaxPeepSpawns = 2;
    // constexpr const uint8_t MaxParkEntrances = 4;
    //// The number of elements in the patrol_areas array per staff member. Every bit in the array represents a 4x4 square.
    //// In RCT1, that's an 8-bit array. 8 * 128 = 1024 bits, which is also the number of 4x4 squares on a 128x128 map.
    //// For RCT2, it's a 32-bit array. 32 * 128 = 4096 bits, which is also the number of 4x4 squares on a 256x256 map.
    // constexpr const uint8_t PatrolAreaSize = 128;
    // constexpr const uint8_t StaffTypeCount = 4;

    // constexpr const uint8_t ExpenditureTableMonthCount = 16;
    // constexpr const uint8_t ExpenditureTypeCount = 14;
    // constexpr const uint8_t FinanceGraphSize = 128;

    // constexpr const uint16_t MaxUserStrings = 1024;
    // constexpr const uint8_t MaxUserStringLength = 32;

    // constexpr const uint8_t MaxPeepThoughts = 5;

    // constexpr const uint8_t MaxElementHeight = 255;

    // constexpr const uint8_t MaxBanners = 250;

    // constexpr const uint16_t MaxEntities = 10000;
    // constexpr const uint32_t MaxTileElements = 0x30000;
    // constexpr const uint16_t MaxAnimatedObjects = 2000;
    // constexpr const uint8_t MaxResearchedRideTypeQuads = 8;  // With 32 bits per uint32_t, this means there is room for
    //                                                         // 256 types.
    // constexpr const uint8_t MaxResearchedRideEntryQuads = 8; // With 32 bits per uint32_t, this means there is room for
    //                                                         // 256 entries.
    // constexpr const uint8_t MaxResearchedSceneryItemQuads = 56;
    // constexpr const uint16_t MaxResearchedSceneryItems = (MaxResearchedSceneryItemQuads * 32); // There are 32
    //                                                                                           // bits per
    //                                                                                           // quad.
    // constexpr const uint16_t MaxResearchItems = 500;

    // constexpr uint16_t TD6MaxTrackElements = 8192;
    // constexpr const uint16_t MaxMapSize = 256;
    // constexpr const uint8_t RideTypeCount = 91;

    // Defined in ObjectLimits.h
    // constexpr const uint8_t MaxRideObject = 128;
    // constexpr const uint8_t MaxSmallSceneryObjects = 252;
    // constexpr const uint8_t MaxLargeSceneryObjects = 128;
    // constexpr const uint8_t MaxWallSceneryObjects = 128;
    // constexpr const uint8_t MaxBannerObjects = 32;
    // constexpr const uint8_t MaxPathObjects = 16;
    // constexpr const uint8_t MaxPathAdditionObjects = 15;
    // constexpr const uint8_t MaxScenereyGroupObjects = 19;
    // constexpr const uint8_t MaxParkEntranceObjects = 1;
    // constexpr const uint8_t MaxWaterObjects = 1;
    // constexpr const uint8_t MaxScenarioTextObjects = 1;
} // namespace OpenRCT2::Limits
