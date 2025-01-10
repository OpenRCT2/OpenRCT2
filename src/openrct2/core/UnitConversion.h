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

namespace OpenRCT2
{
    int32_t SquaredMetresToSquaredFeet(int32_t squaredMetres);
    int32_t MetresToFeet(int32_t metres);
    int32_t FeetToMetres(int32_t feet);
    int32_t MphToKmph(int32_t mph);
    int32_t MphToDmps(int32_t mph);
    int32_t BaseZToMetres(int16_t baseZ);
    uint8_t MetresToBaseZ(int16_t metres);
    int32_t HeightUnitsToMetres(int32_t heightUnit);
    int32_t ToHumanReadableSpeed(int32_t baseSpeed);
    uint16_t ToHumanReadableAirTime(uint16_t airTime);
    int32_t ToHumanReadableRideLength(int32_t rideLength);
} // namespace OpenRCT2
