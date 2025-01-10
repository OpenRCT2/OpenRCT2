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
     *  rct2: 0x006D5FAB
     */
    void VehicleVisualLaunchedFreefall(
        PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
        const CarEntry* carEntry)
    {
        auto imageFlags = ImageId(0, vehicle->colours.Body, vehicle->colours.Trim);
        if (vehicle->IsGhost())
        {
            imageFlags = ConstructionMarker;
        }

        // Draw back:
        int32_t baseImage_id = carEntry->base_image_id + ((vehicle->restraints_position / 64) * 2);
        auto image_id = imageFlags.WithIndex(baseImage_id + 2);
        PaintAddImageAsParent(session, image_id, { 0, 0, z }, { { -11, -11, z + 1 }, { 2, 2, 41 } });

        // Draw front:
        image_id = imageFlags.WithIndex(baseImage_id + 1);
        PaintAddImageAsParent(session, image_id, { 0, 0, z }, { { -5, -5, z + 1 }, { 16, 16, 41 } });

        // Draw peeps:
        if (session.DPI.zoom_level < ZoomLevel{ 2 } && vehicle->num_peeps > 0 && !vehicle->IsGhost())
        {
            baseImage_id = carEntry->base_image_id + 9;
            if ((vehicle->restraints_position / 64) == 3)
            {
                baseImage_id += 2; // Draw peeps sitting without transparent area between them for restraints
            }
            auto directionOffset = OpenRCT2::Entity::Yaw::YawTo4(imageDirection);
            image_id = ImageId(
                baseImage_id + (((directionOffset + 0) & 3) * 3), vehicle->peep_tshirt_colours[0],
                vehicle->peep_tshirt_colours[1]);
            PaintAddImageAsChild(session, image_id, { 0, 0, z }, { { -5, -5, z + 1 }, { 16, 16, 41 } });
            if (vehicle->num_peeps > 2)
            {
                image_id = ImageId(
                    baseImage_id + (((directionOffset + 1) & 3) * 3), vehicle->peep_tshirt_colours[2],
                    vehicle->peep_tshirt_colours[3]);
                PaintAddImageAsChild(session, image_id, { 0, 0, z }, { { -5, -5, z + 1 }, { 16, 16, 41 } });
            }
            if (vehicle->num_peeps > 4)
            {
                image_id = ImageId(
                    baseImage_id + (((directionOffset + 2) & 3) * 3), vehicle->peep_tshirt_colours[4],
                    vehicle->peep_tshirt_colours[5]);
                PaintAddImageAsChild(session, image_id, { 0, 0, z }, { { -5, -5, z + 1 }, { 16, 16, 41 } });
            }
            if (vehicle->num_peeps > 6)
            {
                image_id = ImageId(
                    baseImage_id + (((directionOffset + 3) & 3) * 3), vehicle->peep_tshirt_colours[6],
                    vehicle->peep_tshirt_colours[7]);
                PaintAddImageAsChild(session, image_id, { 0, 0, z }, { { -5, -5, z + 1 }, { 16, 16, 41 } });
            }
        }

        assert(carEntry->effect_visual == 1);
    }
} // namespace OpenRCT2
