/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Vehicle.MiniGolf.h"

#include "../../entity/EntityRegistry.h"
#include "../../entity/Guest.h"
#include "../../ride/Ride.h"
#include "../../ride/TrackPaint.h"
#include "../../ride/Vehicle.h"
#include "../Paint.h"
#include "VehiclePaint.h"

#include <cstdint>

namespace OpenRCT2
{
    /** rct2: 0x00933471 */
    // clang-format off
    static constexpr uint8_t MiniGolfPeepAnimationFramesWalk[] = {
        0, 1, 2, 3, 4, 5,
    };

    /** rct2: 0x00933478 */
    static constexpr uint8_t MiniGolfPeepAnimationFramesPlaceBallDownwards[] = {
        12, 13, 14, 15,
    };

    /** rct2: 0x009334B5 */
    static constexpr uint8_t MiniGolfPeepAnimationFramesSwing[] = {
        31, 31, 31, 31, 31, 31, 31, 31, 31, 32, 33, 33, 33, 33, 34,
    };

    /** rct2: 0x0093347D */
    static constexpr uint8_t MiniGolfPeepAnimationFramesSwingLeft[] = {
        6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8, 8, 8, 8, 9,
    };

    /** rct2: 0x0093348D */
    static constexpr uint8_t MiniGolfPeepAnimationFramesPickupBall[] = {
        12, 13, 14, 15, 14, 13, 12,
    };

    /** rct2: 0x00933495 */
    static constexpr uint8_t MiniGolfPeepAnimationFramesJump[] = {
        16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
    };

    /** rct2: 0x009334A5 */
    static constexpr uint8_t MiniGolfPeepAnimationFramesPlaceBallUpwards[] = {
        15, 14, 13, 12,
    };

    /** rct2: 0x009334C5 */
    static constexpr uint8_t MiniGolfPeepAnimationFramesPutt[] = {
        35, 36, 36, 36, 36, 36, 35, 35, 35, 35,
    };

    /** rct2: 0x009334AA */
    static constexpr uint8_t MiniGolfPeepAnimationFramesPuttLeft[] = {
        10, 11, 11, 11, 11, 11, 10, 10, 10, 10,
    };

    /** rct2: 0x008B8F74 */
    static constexpr const uint8_t* MiniGolfPeepAnimationFrames[] = {
        MiniGolfPeepAnimationFramesWalk,
        MiniGolfPeepAnimationFramesPlaceBallDownwards,
        MiniGolfPeepAnimationFramesSwingLeft,
        MiniGolfPeepAnimationFramesPickupBall,
        MiniGolfPeepAnimationFramesJump,
        MiniGolfPeepAnimationFramesPlaceBallUpwards,
        MiniGolfPeepAnimationFramesPuttLeft,
        MiniGolfPeepAnimationFramesSwing,
        MiniGolfPeepAnimationFramesPutt,
    };

    const size_t kMiniGolfPeepAnimationLengths[] = {
        std::size(MiniGolfPeepAnimationFramesWalk),
        std::size(MiniGolfPeepAnimationFramesPlaceBallDownwards),
        std::size(MiniGolfPeepAnimationFramesSwingLeft),
        std::size(MiniGolfPeepAnimationFramesPickupBall),
        std::size(MiniGolfPeepAnimationFramesJump),
        std::size(MiniGolfPeepAnimationFramesPlaceBallUpwards),
        std::size(MiniGolfPeepAnimationFramesPuttLeft),
        std::size(MiniGolfPeepAnimationFramesSwing),
        std::size(MiniGolfPeepAnimationFramesPutt),
    };
    // clang-format on

    /**
     * rct2: 0x006D42F0
     */
    void VehicleVisualMiniGolfPlayer(
        PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle)
    {
        if (vehicle->num_peeps == 0)
        {
            return;
        }

        if (session.DPI.zoom_level >= ZoomLevel{ 2 })
        {
            return;
        }

        auto ride = vehicle->GetRide();
        if (ride == nullptr)
            return;

        auto rideEntry = ride->GetRideEntry();
        if (rideEntry == nullptr)
            return;

        auto* peep = GetEntity<Guest>(vehicle->peep[0]);
        if (peep == nullptr)
            return;

        uint8_t frame = MiniGolfPeepAnimationFrames[EnumValue(vehicle->mini_golf_current_animation)][vehicle->animation_frame];
        uint32_t ebx = (frame << 2) + OpenRCT2::Entity::Yaw::YawTo4(imageDirection);

        ImageIndex index = rideEntry->Cars[0].base_image_id + 1 + ebx;
        auto image = ImageId(index, peep->TshirtColour, peep->TrousersColour);
        PaintAddImageAsParent(session, image, { 0, 0, z }, { { 0, 0, z + 5 }, { 1, 1, 11 } });
    }

    /**
     * rct2: 0x006D43C6
     */
    void VehicleVisualMiniGolfBall(
        PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle)
    {
        if (vehicle->mini_golf_current_animation != MiniGolfAnimation::PlaceBallDown)
        {
            return;
        }

        if (session.DPI.zoom_level >= ZoomLevel{ 1 })
        {
            return;
        }

        auto ride = vehicle->GetRide();
        if (ride == nullptr)
            return;

        auto rideEntry = ride->GetRideEntry();
        if (rideEntry == nullptr)
            return;

        uint32_t image_id = rideEntry->Cars[0].base_image_id;
        PaintAddImageAsParent(session, ImageId(image_id), { 0, 0, z }, { { 0, 0, z + 3 }, { 1, 1, 0 } });
    }
} // namespace OpenRCT2
