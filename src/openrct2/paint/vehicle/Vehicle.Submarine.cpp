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
    void VehicleVisualSubmarine(
        PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
        const CarEntry* carEntry)
    {
        auto baseImageId = SubmarineVehicleGetBaseImageId(vehicle, carEntry, imageDirection);
        auto imageId0 = ImageId(baseImageId + 0, vehicle->colours.Body, vehicle->colours.Trim, vehicle->colours.Tertiary);
        auto imageId1 = ImageId(baseImageId + 1, vehicle->colours.Body, vehicle->colours.Trim, vehicle->colours.Tertiary);
        if (vehicle->IsGhost())
        {
            imageId0 = ImageId(baseImageId + 0).WithRemap(FilterPaletteID::PaletteGhost);
            imageId1 = ImageId(baseImageId + 1).WithRemap(FilterPaletteID::PaletteGhost);
        }

        const auto& bb = VehicleBoundboxes[carEntry->draw_order][OpenRCT2::Entity::Yaw::YawTo16(imageDirection)];
        PaintAddImageAsParent(
            session, imageId0, { 0, 0, z },
            { { bb.offset_x, bb.offset_y, bb.offset_z + z }, { bb.length_x, bb.length_y, bb.length_z } });
        PaintAddImageAsParent(
            session, imageId1, { 0, 0, z },
            { { bb.offset_x, bb.offset_y, bb.offset_z + z - 10 }, { bb.length_x, bb.length_y, 2 } });
        assert(carEntry->effect_visual == 1);
    }
} // namespace OpenRCT2
