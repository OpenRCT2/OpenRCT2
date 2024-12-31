/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../interface/Colour.h"

#include <cstdint>

struct TrackColour
{
    colour_t main;
    colour_t additional;
    colour_t supports;
};

enum class RideColourScheme : uint8_t
{
    main,
    additional1,
    additional2,
    additional3,
};
constexpr uint8_t kNumRideColourSchemes = 4;

enum class VehicleColourSettings : uint8_t
{
    same,
    perTrain,
    perCar,
};
constexpr uint8_t kNumVehicleColourSettings = 3;
