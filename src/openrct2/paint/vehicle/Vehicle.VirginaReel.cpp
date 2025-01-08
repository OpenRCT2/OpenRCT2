/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../ride/Vehicle.h"

#include "../Boundbox.h"
#include "../Paint.h"
#include "VehiclePaint.h"

#include <cstdint>

namespace OpenRCT2
{
    // 0x009927E6:
    static constexpr VehicleBoundBox _virginiaReelBoundbox[] = {
        { -11, -11, 1, 22, 22, 13 }, { -11, -11, 1, 22, 22, 13 }, { -11, -11, 1, 22, 22, 13 },
        { -11, -11, 1, 22, 22, 13 }, { -11, -11, 1, 22, 22, 13 }, { -11, -11, 1, 22, 22, 13 },
        { -11, -11, 1, 22, 22, 13 }, { -11, -11, 1, 22, 22, 13 }, { -11, -11, 1, 22, 22, 13 },
    };

    /**
     *
     *  rct2: 0x006D5B48
     */
    void VehicleVisualVirginiaReel(
        PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
        const CarEntry* carEntry)
    {
        imageDirection = OpenRCT2::Entity::Yaw::YawTo32(imageDirection);
        const uint8_t rotation = session.CurrentRotation;
        int32_t ecx = ((vehicle->spin_sprite / 8) + (rotation * 8)) & 31;
        int32_t baseImage_id = [&] {
            switch (vehicle->Pitch)
            {
                case 1:
                    return (imageDirection & 24) + 8;
                case 2:
                    return (imageDirection & 24) + 40;
                case 5:
                    return ((imageDirection ^ 16) & 24) + 8;
                case 6:
                    return ((imageDirection ^ 16) & 24) + 40;
                default:
                    return 0;
            }
        }();
        baseImage_id += ecx & 7;
        const auto& vehicleBb = _virginiaReelBoundbox[baseImage_id >> 3];
        auto bb = BoundBoxXYZ{ { vehicleBb.offset_x, vehicleBb.offset_y, vehicleBb.offset_z + z },
                               { vehicleBb.length_x, vehicleBb.length_y, vehicleBb.length_z } };

        baseImage_id += carEntry->base_image_id;
        auto image_id = ImageId(baseImage_id, vehicle->colours.Body, vehicle->colours.Trim);
        if (vehicle->IsGhost())
        {
            image_id = ConstructionMarker.WithIndex(image_id.GetIndex());
        }
        PaintAddImageAsParent(session, image_id, { 0, 0, z }, bb);

        if (session.DPI.zoom_level < ZoomLevel{ 2 } && vehicle->num_peeps > 0 && !vehicle->IsGhost())
        {
            uint8_t riding_peep_sprites[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
            for (int32_t i = 0; i < vehicle->num_peeps; i++)
            {
                riding_peep_sprites[((ecx / 8) + i) & 3] = vehicle->peep_tshirt_colours[i];
            }
            int32_t draw_order[4] = { 0, 1, 3, 2 };
            for (auto i : draw_order)
            {
                if (riding_peep_sprites[i] != 0xFF)
                {
                    image_id = ImageId(baseImage_id + ((i + 1) * 72), riding_peep_sprites[i]);
                    PaintAddImageAsChild(session, image_id, { 0, 0, z }, bb);
                }
            }
        }

        assert(carEntry->effect_visual == 1);
    }
} // namespace OpenRCT2
