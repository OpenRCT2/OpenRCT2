/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../interface/Viewport.h"
#include "../../../ride/Ride.h"
#include "../../../ride/RideEntry.h"
#include "../../../ride/Track.h"
#include "../../../ride/TrackPaint.h"
#include "../../../sprites.h"
#include "../../../world/Map.h"
#include "../../../world/tile_element/TrackElement.h"
#include "../../Boundbox.h"
#include "../../Paint.h"
#include "../../support/WoodenSupports.h"
#include "../../support/WoodenSupports.hpp"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Square;

static void PaintFacility(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    bool hasSupports = DrawSupportForSequenceA<TrackElemType::FlatTrack1x1A>(
        session, supportType.wooden, trackSequence, direction, height, GetShopSupportColourScheme(session, trackElement));

    auto rideEntry = ride.GetRideEntry();
    if (rideEntry == nullptr)
        return;

    auto firstCarEntry = &rideEntry->Cars[0];
    if (firstCarEntry == nullptr)
        return;

    auto lengthX = (direction & 1) == 0 ? 28 : 2;
    auto lengthY = (direction & 1) == 0 ? 2 : 28;
    CoordsXYZ offset(0, 0, height);
    BoundBoxXYZ bb = { { direction == 3 ? 28 : 2, direction == 0 ? 28 : 2, height },
                       { lengthX, lengthY, trackElement.GetClearanceZ() - trackElement.GetBaseZ() - 3 } };

    auto imageTemplate = session.TrackColours;
    auto imageIndex = firstCarEntry->base_image_id + ((direction + 2) & 3);
    auto imageId = imageTemplate.WithIndex(imageIndex);
    if (hasSupports)
    {
        auto foundationImageTemplate = GetShopSupportColourScheme(session, trackElement);
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
        PaintAddImageAsParent(session, imageId.WithIndexOffset(2), offset, { { 28, 2, height }, { 2, 28, 29 } });
    }
    else if (direction == 2)
    {
        PaintAddImageAsParent(session, imageId.WithIndexOffset(4), offset, { { 2, 28, height }, { 28, 2, 29 } });
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);

    if (direction == 1 || direction == 2)
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
}

/* 0x00762D44 */
TrackPaintFunction GetTrackPaintFunctionFacility(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::FlatTrack1x1A:
            return PaintFacility;
        default:
            return TrackPaintFunctionDummy;
    }
}
