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
    /**
     *
     *  rct2: 0x006D5DA9
     */
    void VehicleVisualRotoDrop(
        PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
        const CarEntry* carEntry)
    {
        imageDirection = OpenRCT2::Entity::Yaw::YawTo32(imageDirection);

        auto imageFlags = ImageId(0, vehicle->colours.Body, vehicle->colours.Trim);
        if (vehicle->IsGhost())
        {
            imageFlags = ConstructionMarker;
        }

        ImageId image_id;
        int32_t baseImage_id = (carEntry->base_image_id + 4) + ((vehicle->animation_frame / 4) & 0x3);
        if (vehicle->restraints_position >= 64)
        {
            baseImage_id += 7;
            baseImage_id += (vehicle->restraints_position / 64);
        }

        // Draw back:
        image_id = imageFlags.WithIndex(baseImage_id);
        PaintAddImageAsParent(session, image_id, { 0, 0, z }, { { -11, -11, z + 1 }, { 2, 2, 41 } });

        // Draw front:
        image_id = imageFlags.WithIndex(baseImage_id + 4);
        PaintAddImageAsParent(session, image_id, { 0, 0, z }, { { -5, -5, z + 1 }, { 16, 16, 41 } });

        if (vehicle->num_peeps > 0 && !vehicle->IsGhost())
        {
            uint8_t riding_peep_sprites[64];
            std::fill_n(riding_peep_sprites, sizeof(riding_peep_sprites), 0xFF);
            for (int32_t i = 0; i < vehicle->num_peeps; i++)
            {
                uint8_t cl = (i & 3) * 16;
                cl += (i & 0xFC);
                cl += vehicle->animation_frame / 4;
                cl += (imageDirection / 8) * 16;
                cl &= 0x3F;
                riding_peep_sprites[cl] = vehicle->peep_tshirt_colours[i];
            }

            // Draw riding peep sprites in back to front order:
            for (int32_t j = 0; j <= 48; j++)
            {
                int32_t i = (j % 2) ? (48 - (j / 2)) : (j / 2);
                if (riding_peep_sprites[i] != 0xFF)
                {
                    baseImage_id = carEntry->base_image_id + 20 + i;
                    if (vehicle->restraints_position >= 64)
                    {
                        baseImage_id += 64;
                        baseImage_id += vehicle->restraints_position / 64;
                    }
                    image_id = ImageId(baseImage_id, riding_peep_sprites[i]);
                    PaintAddImageAsChild(session, image_id, { 0, 0, z }, { { -5, -5, z + 1 }, { 16, 16, 41 } });
                }
            }
        }

        assert(carEntry->effect_visual == 1);
        // Although called in original code, effect_visual (splash effects) are not used for many rides and does not make sense
        // so it was taken out
    }
} // namespace OpenRCT2
