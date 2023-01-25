/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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

static void PaintShop(
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

    CoordsXYZ offset(0, 0, height);
    BoundBoxXYZ bb = { { 2, 2, height }, { 28, 28, 45 } };

    auto imageFlags = session.TrackColours[SCHEME_TRACK].WithoutSecondary();
    auto imageIndex = firstCarEntry->base_image_id + direction;
    if (hasSupports)
    {
        auto foundationImageTemplate = session.TrackColours[SCHEME_3];
        auto foundationImageIndex = (direction & 1) ? SPR_FLOOR_PLANKS_90_DEG : SPR_FLOOR_PLANKS;
        auto foundationImageId = foundationImageTemplate.WithIndex(foundationImageIndex);
        PaintAddImageAsParent(session, foundationImageId, offset, bb);
        PaintAddImageAsChild(session, imageFlags.WithIndex(imageIndex), offset, bb);
    }
    else
    {
        PaintAddImageAsParent(session, imageFlags.WithIndex(imageIndex), offset, bb);
    }

    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);

    if (direction == 1 || direction == 2)
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
}

TRACK_PAINT_FUNCTION GetTrackPaintFunctionShop(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::FlatTrack1x1A:
        case TrackElemType::FlatTrack1x1B:
            return PaintShop;
    }
    return nullptr;
}
