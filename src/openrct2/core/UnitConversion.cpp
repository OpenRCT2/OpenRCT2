/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "UnitConversion.h"

namespace OpenRCT2
{
    int32_t SquaredMetresToSquaredFeet(int32_t squaredMetres)
    {
        // 1 metre squared = 10.7639104 feet squared
        // RCT2 approximates as 11
        return squaredMetres * 11;
    }

    int32_t MetresToFeet(int32_t metres)
    {
        // 1 metre = 3.2808399 feet
        // RCT2 approximates as 3.28125
        return (metres * 840) / 256;
    }

    int32_t FeetToMetres(int32_t feet)
    {
        return feet * 256 / 840;
    }

    int32_t MphToKmph(int32_t mph)
    {
        // 1 mph = 1.60934 kmph
        // RCT2 approximates as 1.609375
        return (mph * 1648) >> 10;
    }

    int32_t MphToDmps(int32_t mph)
    {
        // 1 mph = 4.4704 decimeters/s
        return (mph * 73243) >> 14;
    }

    int32_t BaseZToMetres(int16_t baseZ)
    {
        return (baseZ / 2 - 7) * 1.5;
    }

    uint8_t MetresToBaseZ(int16_t metres)
    {
        return ((metres / 1.5) + 7) * 2;
    }

    int32_t HeightUnitsToMetres(int32_t heightUnit)
    {
        // 1 unit = 0.75 metres
        return (heightUnit * 3) >> 2;
    }

    int32_t ToHumanReadableSpeed(int32_t baseSpeed)
    {
        // Divide this value by 29127 to get the human-readable max speed
        // (in RCT2, display_speed = (max_speed * 9) >> 18)
        return (baseSpeed * 9) >> 18;
    }

    uint16_t ToHumanReadableAirTime(uint16_t airTime)
    {
        return airTime * 3;
    }

    int32_t ToHumanReadableRideLength(int32_t rideLength)
    {
        return rideLength >> 16;
    }
} // namespace OpenRCT2
