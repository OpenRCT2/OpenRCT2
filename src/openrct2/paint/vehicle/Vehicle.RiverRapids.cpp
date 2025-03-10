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
    constexpr BoundBoxXYZ kRiverRapidsBoundBoxStraight = { { -13, -13, 1 }, { 26, 26, 13 } };
    constexpr BoundBoxXYZ kRiverRapidsBoundBoxCorners = { { -8, -8, 1 }, { 20, 20, 13 } };
    constexpr BoundBoxXYZ kRiverRapidsBoundBoxPitch1 = kRiverRapidsBoundBoxCorners;
    constexpr BoundBoxXYZ kRiverRapidsBoundBoxPitch2 = { { -8, -8, 1 }, { 16, 16, 13 } };

    static constexpr std::array<std::array<BoundBoxXYZ, 16>, kNumOrthogonalDirections> kRiverRapidsBoundBoxesFlat = { {
        { {
            kRiverRapidsBoundBoxStraight,
            kRiverRapidsBoundBoxStraight,
            kRiverRapidsBoundBoxStraight,
            kRiverRapidsBoundBoxStraight,
            kRiverRapidsBoundBoxStraight,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxStraight,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxStraight,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxCorners,
        } },
        { {
            kRiverRapidsBoundBoxStraight,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxStraight,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxStraight,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxStraight,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxCorners,
        } },
        { {
            kRiverRapidsBoundBoxStraight,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxStraight,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxStraight,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxStraight,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxCorners,
        } },
        { {
            kRiverRapidsBoundBoxStraight,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxStraight,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxStraight,
            kRiverRapidsBoundBoxStraight,
            kRiverRapidsBoundBoxStraight,
            kRiverRapidsBoundBoxStraight,
            kRiverRapidsBoundBoxStraight,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxCorners,
            kRiverRapidsBoundBoxCorners,
        } },
    } };

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
        BoundBoxXYZ boundBox;
        if (vehicle->Pitch == 0)
        {
            baseImage_id = ecx & 7;
            boundBox = kRiverRapidsBoundBoxesFlat[(vehicle->GetTrackDirection() + session.CurrentRotation) & 3]
                                                 [imageDirection >> 1];
        }
        else
        {
            if (vehicle->Pitch == 1 || vehicle->Pitch == 5)
            {
                boundBox = kRiverRapidsBoundBoxPitch1;
                if (vehicle->Pitch == 5)
                {
                    baseImage_id = imageDirection ^ 16;
                }
                baseImage_id &= 24;
                baseImage_id += (ecx & 7);
                baseImage_id += 8;
            }
            else if (vehicle->Pitch == 2 || vehicle->Pitch == 6)
            {
                boundBox = kRiverRapidsBoundBoxPitch2;
                if (vehicle->Pitch == 6)
                {
                    baseImage_id = imageDirection ^ 16;
                }
                baseImage_id &= 24;
                baseImage_id += (ecx & 7);
                baseImage_id += 40;
            }
            else
            {
                boundBox = kRiverRapidsBoundBoxStraight;
                baseImage_id = ecx & 7;
            }
        }
        baseImage_id += carEntry->base_image_id;

        boundBox.offset.z += z;
        image_id = ImageId(baseImage_id, vehicle->colours.Body, vehicle->colours.Trim);
        if (vehicle->IsGhost())
        {
            image_id = ConstructionMarker.WithIndex(image_id.GetIndex());
        }
        PaintAddImageAsParent(session, image_id, { 0, 0, z }, boundBox);

        if (session.DPI.zoom_level < ZoomLevel{ 2 } && vehicle->num_peeps > 0 && !vehicle->IsGhost())
        {
            // Draw peeps: (this particular vehicle doesn't sort them back to front like others so the back ones sometimes clip,
            // but that's how the original does it...)
            int32_t peeps = ((ecx / 8) + 0) & 3;
            image_id = ImageId(
                baseImage_id + ((peeps + 1) * 72), vehicle->peep_tshirt_colours[0], vehicle->peep_tshirt_colours[1]);
            PaintAddImageAsChild(session, image_id, { 0, 0, z }, boundBox);
            if (vehicle->num_peeps > 2)
            {
                peeps = ((ecx / 8) + 2) & 3;
                image_id = ImageId(
                    baseImage_id + ((peeps + 1) * 72), vehicle->peep_tshirt_colours[2], vehicle->peep_tshirt_colours[3]);
                PaintAddImageAsChild(session, image_id, { 0, 0, z }, boundBox);
            }
            if (vehicle->num_peeps > 4)
            {
                peeps = ((ecx / 8) + 1) & 3;
                image_id = ImageId(
                    baseImage_id + ((peeps + 1) * 72), vehicle->peep_tshirt_colours[4], vehicle->peep_tshirt_colours[5]);
                PaintAddImageAsChild(session, image_id, { 0, 0, z }, boundBox);
            }
            if (vehicle->num_peeps > 6)
            {
                peeps = ((ecx / 8) + 3) & 3;
                image_id = ImageId(
                    baseImage_id + ((peeps + 1) * 72), vehicle->peep_tshirt_colours[6], vehicle->peep_tshirt_colours[7]);
                PaintAddImageAsChild(session, image_id, { 0, 0, z }, boundBox);
            }
        }

        VehicleVisualSplashEffect(session, z, vehicle, carEntry);
    }
} // namespace OpenRCT2
