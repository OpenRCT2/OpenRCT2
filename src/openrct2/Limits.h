/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
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
    constexpr const uint8_t CustomerHistorySize = RCT12::Limits::CustomerHistorySize;
    constexpr const uint16_t MaxGolfHoles = RCT12::Limits::MaxGolfHoles;
    constexpr const uint16_t MaxHelices = RCT12::Limits::MaxHelices;
    constexpr const uint16_t MaxInversions = RCT12::Limits::MaxInversions;
    constexpr const uint16_t MaxTrainsPerRide = 255;
    constexpr const uint16_t MaxCarsPerTrain = 255;
    constexpr const uint16_t MaxVehicleColours = MaxTrainsPerRide; // this should really be MaxTrainsPerRide * MaxCarsPerTrain
    constexpr const uint8_t MaxCircuitsPerRide = 20;
    constexpr const uint8_t MaxAwards = RCT12::Limits::MaxAwards;
    constexpr const uint8_t NumColourSchemes = RCT12::Limits::NumColourSchemes;
    constexpr const uint8_t DowntimeHistorySize = RCT2::Limits::DowntimeHistorySize;
    constexpr const uint16_t MaxPeepSpawns = 256;
    constexpr const uint16_t MaxParkEntrances = 256;
    constexpr const uint8_t MaxWaitingTime = RCT12::Limits::MaxWaitingTime;
    constexpr const uint8_t CheatsMaxOperatingLimit = 255;
} // namespace OpenRCT2::Limits
