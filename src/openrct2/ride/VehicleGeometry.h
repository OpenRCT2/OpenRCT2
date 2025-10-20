/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/Location.hpp"
#include "Angles.h"

#include <array>

namespace OpenRCT2::RideVehicle::Geometry
{

    /** The distance between subposition points in a movement vector.
     * Squashes vector components to 0 or !0, so vector length is ignored.
     */
    constexpr int32_t getTranslationDistance(CoordsXYZ distance, bool useReverserDistance);

} // namespace OpenRCT2::RideVehicle::Geometry
