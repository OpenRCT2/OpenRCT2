/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

namespace OpenRCT2::Drawing
{
    enum class Colour : uint8_t;
}

struct TrackColour
{
    OpenRCT2::Drawing::Colour main;
    OpenRCT2::Drawing::Colour additional;
    OpenRCT2::Drawing::Colour supports;
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
