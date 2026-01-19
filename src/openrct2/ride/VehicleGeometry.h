/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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

    Unk9A36C4Struct getFreeroamVehicleMovementData(uint8_t orientation);

    int32_t getRollHorizontalComponent(VehicleRoll roll);

    int32_t getSoundDirectionFromOrientation(uint8_t orientation);

    CoordsXY getCrashDirectionComponents(uint8_t orientation);

    /** The distance between subposition points in a movement vector.
     * Squashes vector components to 0 or !0, so vector length is ignored.
     */
    int32_t getTranslationDistance(CoordsXYZ distance, bool useReverserDistance);

    int32_t getAccelerationFromPitch(VehiclePitch pitch);

    CoordsXY getPitchVector32(VehiclePitch pitch);

} // namespace OpenRCT2::RideVehicle::Geometry
