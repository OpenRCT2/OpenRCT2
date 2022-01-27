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
    using RCT12::Limits::CustomerHistorySize;
    using RCT12::Limits::MaxGolfHoles;
    using RCT12::Limits::MaxHelices;
    using RCT12::Limits::MaxInversions;
    constexpr const uint16_t MaxTrainsPerRide = 255;
    constexpr const uint16_t MaxCarsPerTrain = 255;
    constexpr const uint16_t MaxVehicleColours = MaxTrainsPerRide; // this should really be MaxTrainsPerRide * MaxCarsPerTrain
    constexpr const uint8_t MaxCircuitsPerRide = 20;
    using RCT12::Limits::MaxAwards;
    using RCT12::Limits::NumColourSchemes;
    using RCT2::Limits::DowntimeHistorySize;
    constexpr const uint16_t MaxPeepSpawns = 256;
    constexpr const uint16_t MaxParkEntrances = 256;
} // namespace OpenRCT2::Limits
