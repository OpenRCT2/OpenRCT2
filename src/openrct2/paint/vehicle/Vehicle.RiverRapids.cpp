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
    // 0x0099279E:
    static constexpr VehicleBoundBox _riverRapidsBoundbox[] = {
        { -13, -13, 1, 26, 26, 13 }, { -13, -13, 1, 26, 26, 13 }, { -13, -13, 1, 26, 26, 13 },
        { -13, -13, 1, 26, 26, 13 }, { -13, -13, 1, 26, 26, 13 }, { -13, -13, 1, 26, 26, 13 },
        { -13, -13, 1, 26, 26, 13 }, { -13, -13, 1, 26, 26, 13 }, { -13, -13, 1, 26, 26, 13 },
    };

    /**
     *
     *  rct2: 0x006D5889
     */
    void VehicleVisualRiverRapids(
        PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
        const CarEntry* carEntry)
    {
        imageDirection = OpenRCT2::Entity::Yaw::YawTo32(imageDirection);

        ImageId image_id;
        int32_t baseImage_id = imageDirection;
        uint32_t rotation = session.CurrentRotation;
        int32_t ecx = ((vehicle->spin_sprite / 8) + (rotation * 8)) & 31;
        int32_t j = 0;
        if (vehicle->Pitch == 0)
        {
            baseImage_id = ecx & 7;
        }
        else
        {
            if (vehicle->Pitch == 1 || vehicle->Pitch == 5)
            {
                if (vehicle->Pitch == 5)
                {
                    baseImage_id = imageDirection ^ 16;
                }
                baseImage_id &= 24;
                j = (baseImage_id / 8) + 1;
                baseImage_id += (ecx & 7);
                baseImage_id += 8;
            }
            else if (vehicle->Pitch == 2 || vehicle->Pitch == 6)
            {
                if (vehicle->Pitch == 6)
                {
                    baseImage_id = imageDirection ^ 16;
                }
                baseImage_id &= 24;
                j = (baseImage_id / 8) + 5;
                baseImage_id += (ecx & 7);
                baseImage_id += 40;
            }
            else
            {
                baseImage_id = ecx & 7;
            }
        }
        baseImage_id += carEntry->base_image_id;

        const auto& riverRapidsBb = _riverRapidsBoundbox[j];
        auto bb = BoundBoxXYZ{ { riverRapidsBb.offset_x, riverRapidsBb.offset_y, riverRapidsBb.offset_z + z },
                               { riverRapidsBb.length_x, riverRapidsBb.length_y, riverRapidsBb.length_z } };
        image_id = ImageId(baseImage_id, vehicle->colours.Body, vehicle->colours.Trim);
        if (vehicle->IsGhost())
        {
            image_id = ConstructionMarker.WithIndex(image_id.GetIndex());
        }
        PaintAddImageAsParent(session, image_id, { 0, 0, z }, bb);

        if (session.DPI.zoom_level < ZoomLevel{ 2 } && vehicle->num_peeps > 0 && !vehicle->IsGhost())
        {
            // Draw peeps: (this particular vehicle doesn't sort them back to front like others so the back ones sometimes clip,
            // but that's how the original does it...)
            int32_t peeps = ((ecx / 8) + 0) & 3;
            image_id = ImageId(
                baseImage_id + ((peeps + 1) * 72), vehicle->peep_tshirt_colours[0], vehicle->peep_tshirt_colours[1]);
            PaintAddImageAsChild(session, image_id, { 0, 0, z }, bb);
            if (vehicle->num_peeps > 2)
            {
                peeps = ((ecx / 8) + 2) & 3;
                image_id = ImageId(
                    baseImage_id + ((peeps + 1) * 72), vehicle->peep_tshirt_colours[2], vehicle->peep_tshirt_colours[3]);
                PaintAddImageAsChild(session, image_id, { 0, 0, z }, bb);
            }
            if (vehicle->num_peeps > 4)
            {
                peeps = ((ecx / 8) + 1) & 3;
                image_id = ImageId(
                    baseImage_id + ((peeps + 1) * 72), vehicle->peep_tshirt_colours[4], vehicle->peep_tshirt_colours[5]);
                PaintAddImageAsChild(session, image_id, { 0, 0, z }, bb);
            }
            if (vehicle->num_peeps > 6)
            {
                peeps = ((ecx / 8) + 3) & 3;
                image_id = ImageId(
                    baseImage_id + ((peeps + 1) * 72), vehicle->peep_tshirt_colours[6], vehicle->peep_tshirt_colours[7]);
                PaintAddImageAsChild(session, image_id, { 0, 0, z }, bb);
            }
        }

        VehicleVisualSplashEffect(session, z, vehicle, carEntry);
    }
} // namespace OpenRCT2
