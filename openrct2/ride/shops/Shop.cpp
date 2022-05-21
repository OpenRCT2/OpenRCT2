/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../../sprites.h"
#include "../../world/Map.h"
#include "../Ride.h"
#include "../RideEntry.h"
#include "../Track.h"
#include "../TrackPaint.h"

static void PaintShop(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool hasSupports = wooden_a_supports_paint_setup(session, direction & 1, 0, height, session.TrackColours[SCHEME_3]);

    auto rideEntry = ride.GetRideEntry();
    if (rideEntry == nullptr)
        return;

    auto firstVehicleEntry = &rideEntry->vehicles[0];
    if (firstVehicleEntry == nullptr)
        return;

    CoordsXYZ offset(0, 0, height);
    CoordsXYZ bbLength(28, 28, 45);
    CoordsXYZ bbOffset(2, 2, height);

    auto imageFlags = session.TrackColours[SCHEME_TRACK];
    if (imageFlags & IMAGE_TYPE_REMAP_2_PLUS)
    {
        imageFlags &= ~IMAGE_TYPE_REMAP_2_PLUS;
        imageFlags &= ~(31 << 24);
    }
    auto imageTemplate = ImageId::FromUInt32(imageFlags);
    auto imageIndex = firstVehicleEntry->base_image_id + direction;
    if (hasSupports)
    {
        auto foundationImageTemplate = ImageId::FromUInt32(session.TrackColours[SCHEME_3]);
        auto foundationImageIndex = (direction & 1) ? SPR_FLOOR_PLANKS_90_DEG : SPR_FLOOR_PLANKS;
        auto foundationImageId = foundationImageTemplate.WithIndex(foundationImageIndex);
        PaintAddImageAsParent(session, foundationImageId, offset, bbLength, bbOffset);
        PaintAddImageAsChild(session, imageTemplate.WithIndex(imageIndex), offset, bbLength, bbOffset);
    }
    else
    {
        PaintAddImageAsParent(session, imageTemplate.WithIndex(imageIndex), offset, bbLength, bbOffset);
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

TRACK_PAINT_FUNCTION get_track_paint_function_shop(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::FlatTrack1x1A:
        case TrackElemType::FlatTrack1x1B:
            return PaintShop;
    }
    return nullptr;
}
