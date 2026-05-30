/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
    static uint32_t SubmarineVehicleGetBaseImageId(const Vehicle* vehicle, const CarEntry* carEntry, int32_t imageDirection)
    {
        uint32_t result = imageDirection;
        if (vehicle->restraints_position >= 64)
        {
            if ((carEntry->GroupEnabled(SpriteGroupType::RestraintAnimation)) && !(imageDirection & 3))
            {
                auto restraintFrame = ((vehicle->restraints_position - 64) / 64) * 4;
                result = (carEntry->SpriteByYaw(imageDirection, SpriteGroupType::RestraintAnimation) + restraintFrame)
                        * carEntry->base_num_frames
                    + carEntry->GroupImageId(SpriteGroupType::RestraintAnimation);
            }
        }
        else
        {
            result = (carEntry->SpriteByYaw(imageDirection, SpriteGroupType::SlopeFlat) * carEntry->base_num_frames)
                + carEntry->GroupImageId(SpriteGroupType::SlopeFlat) + vehicle->SwingSprite;
        }
        return result;
    }

    /**
     *
     *  rct2: 0x006D44D5
     */
    void vehicleVisualSubmarine(
        PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
        const CarEntry* carEntry)
    {
        auto baseImageId = SubmarineVehicleGetBaseImageId(vehicle, carEntry, imageDirection);
        auto imageId0 = ImageId(baseImageId + 0, vehicle->colours.Body, vehicle->colours.Trim, vehicle->colours.Tertiary);
        auto imageId1 = ImageId(baseImageId + 1, vehicle->colours.Body, vehicle->colours.Trim, vehicle->colours.Tertiary);
        if (vehicle->isGhost())
        {
            imageId0 = ImageId(baseImageId + 0).WithRemap(Drawing::FilterPaletteID::paletteGhost);
            imageId1 = ImageId(baseImageId + 1).WithRemap(Drawing::FilterPaletteID::paletteGhost);
        }

        const auto& bb = VehicleBoundboxes[carEntry->draw_order][Entity::Yaw::YawTo16(imageDirection)];
        paintAddImageAsParent(
            session, imageId0, { 0, 0, z },
            { { bb.offsetX, bb.offsetY, bb.offsetZ + z }, { bb.lengthX, bb.lengthY, bb.lengthZ } });
        paintAddImageAsParent(
            session, imageId1, { 0, 0, z }, { { bb.offsetX, bb.offsetY, bb.offsetZ + z - 10 }, { bb.lengthX, bb.lengthY, 2 } });
        assert(carEntry->effect_visual == 1);
    }
} // namespace OpenRCT2
