/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../interface/Viewport.h"
#include "../../paint/Boundbox.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../../sprites.h"
#include "../../world/Map.h"
#include "../Ride.h"
#include "../RideEntry.h"
#include "../Track.h"
#include "../TrackPaint.h"

static void PaintFacility(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool hasSupports = WoodenASupportsPaintSetup(session, direction & 1, 0, height, session.TrackColours[SCHEME_3]);

    auto rideEntry = ride.GetRideEntry();
    if (rideEntry == nullptr)
        return;

    auto firstCarEntry = &rideEntry->Cars[0];
    if (firstCarEntry == nullptr)
        return;

    auto lengthX = (direction & 1) == 0 ? 28 : 2;
    auto lengthY = (direction & 1) == 0 ? 2 : 28;
    CoordsXYZ offset(0, 0, height);
    BoundBoxXYZ bb = { { direction == 3 ? 28 : 2, direction == 0 ? 28 : 2, height }, { lengthX, lengthY, 29 } };

    auto imageTemplate = session.TrackColours[SCHEME_TRACK];
    auto imageIndex = firstCarEntry->base_image_id + ((direction + 2) & 3);
    auto imageId = imageTemplate.WithIndex(imageIndex);
    if (hasSupports)
    {
        auto foundationImageTemplate = session.TrackColours[SCHEME_3];
        auto foundationImageIndex = (direction & 1) ? SPR_FLOOR_PLANKS_90_DEG : SPR_FLOOR_PLANKS;
        auto foundationImageId = foundationImageTemplate.WithIndex(foundationImageIndex);
        PaintAddImageAsParent(session, foundationImageId, offset, bb);
        PaintAddImageAsChild(session, imageId, offset, bb);
    }
    else
    {
        PaintAddImageAsParent(session, imageId, offset, bb);
    }

    // Base image if door was drawn
    if (direction == 1)
    {
        PaintAddImageAsParent(session, imageId.WithIndexOffset(2), offset, { 2, 28, 29 }, { 28, 2, height });
    }
    else if (direction == 2)
    {
        PaintAddImageAsParent(session, imageId.WithIndexOffset(4), offset, { 28, 2, 29 }, { 2, 28, height });
    }

    PaintUtilSetSegmentSupportHeight(session, SegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/* 0x00762D44 */
TRACK_PAINT_FUNCTION get_track_paint_function_facility(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::FlatTrack1x1A:
            return PaintFacility;
    }
    return nullptr;
}
