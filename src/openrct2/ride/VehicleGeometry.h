/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/EnumUtils.hpp"
#include "../world/Location.hpp"
#include "Angles.h"

#include <array>

namespace OpenRCT2::RideVehicle::Geometry
{

    struct Unk9A36C4Struct
    {
        int16_t x;
        int16_t y;
        uint32_t distance;
    };

    extern const std::array<int32_t, 16> kSubpositionTranslationDistances;
    extern const std::array<Unk9A36C4Struct, 32> kFreeroamVehicleMovementData;
    extern const std::array<int32_t, EnumValue(VehiclePitch::pitchCount)> kAccelerationFromPitch;

    /** The distance between subposition points in a movement vector.
     * Squashes vector components to 0 or !0, so vector length is ignored.
     */
    constexpr int32_t getTranslationDistance(CoordsXYZ distance, bool useReverserDistance)
    {
        uint8_t index = ((distance.x != 0) << 0) | ((distance.y != 0) << 1) | ((distance.z != 0) << 2)
            | ((useReverserDistance) << 3);
        return kSubpositionTranslationDistances[index];
    }

} // namespace OpenRCT2::RideVehicle::Geometry
