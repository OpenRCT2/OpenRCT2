/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "VehicleGeometry.h"

#include <cstddef>

namespace OpenRCT2::RideVehicle::Geometry
{

    /** rct2: 0x009A2930 */
    static constexpr auto kSubpositionTranslationDistances = std::to_array({
        // For a base length of 8716 (0x220C) on the horizontal and 6554 (0x199A) on the vertical,
        // use the Pythagoras theorem and round up.
        0,     // no movement
        8716,  // X translation
        8716,  // Y translation
        12327, // XY translation
        6554,  // Z translation
        10905, // XZ translation
        10905, // YZ translation
        13961, // XYZ translation
        // For the reverser car, multiply the horizontal distance by 2.5 and the vertical distance by 4.072.
        0,     // no movement
        21790, // X translation
        21790, // Y translation
        30817, // Z translation
        16385, // XY translation
        27262, // XZ translation
        27262, // YZ translation
        34902, // XYZ translation
    });
    static_assert(std::size(kSubpositionTranslationDistances) == 16);

    constexpr int32_t getTranslationDistance(CoordsXYZ distance, bool useReverserDistance)
    {
        uint8_t index = ((distance.x != 0) << 0) | ((distance.y != 0) << 1) | ((distance.z != 0) << 2)
            | ((useReverserDistance) << 3);
        return kSubpositionTranslationDistances[index];
    }

} // namespace OpenRCT2::RideVehicle::Geometry
