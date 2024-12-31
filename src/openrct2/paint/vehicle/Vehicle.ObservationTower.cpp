/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../ride/Vehicle.h"

#include "../../ride/Ride.h"
#include "../Paint.h"
#include "VehiclePaint.h"

#include <cstdint>

namespace OpenRCT2
{
    static uint32_t GetObservationTowerVehicleBaseImageId(
        const Vehicle* vehicle, const CarEntry* carEntry, int32_t imageDirection)
    {
        uint32_t result = (vehicle->restraints_position / 64);
        if (vehicle->restraints_position >= 64)
        {
            auto directionOffset = imageDirection / 8;
            if ((directionOffset == 0) || (directionOffset == 3))
            {
                result = carEntry->base_image_id + 8;
            }
            else
            {
                result *= 2;
                result += carEntry->base_image_id;
                if (directionOffset == 1)
                {
                    result += 28;
                }
                else
                {
                    result += 22;
                }
            }
        }
        else
        {
            result = (vehicle->animation_frame * 2) + carEntry->base_image_id + 8;
        }
        return result;
    }

    /**
     *
     *  rct2: 0x006D6258
     */
    void VehicleVisualObservationTower(
        PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
        const CarEntry* carEntry)
    {
        auto baseImageId = GetObservationTowerVehicleBaseImageId(vehicle, carEntry, imageDirection);
        auto imageId0 = ImageId(baseImageId + 0, vehicle->colours.Body, vehicle->colours.Trim, vehicle->colours.Tertiary);
        auto imageId1 = ImageId(baseImageId + 1, vehicle->colours.Body, vehicle->colours.Trim, vehicle->colours.Tertiary);
        if (vehicle->IsGhost())
        {
            imageId0 = ImageId(baseImageId + 0).WithRemap(FilterPaletteID::PaletteGhost);
            imageId1 = ImageId(baseImageId + 1).WithRemap(FilterPaletteID::PaletteGhost);
        }

        PaintAddImageAsParent(session, imageId0, { 0, 0, z }, { { -11, -11, z + 1 }, { 2, 2, 41 } });
        PaintAddImageAsParent(session, imageId1, { 0, 0, z }, { { -5, -5, z + 1 }, { 16, 16, 41 } });
        assert(carEntry->effect_visual == 1);
    }
} // namespace OpenRCT2
