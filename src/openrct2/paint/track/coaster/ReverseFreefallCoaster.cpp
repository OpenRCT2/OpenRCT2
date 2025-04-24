/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../SpriteIds.h"
#include "../../../drawing/Drawing.h"
#include "../../../interface/Viewport.h"
#include "../../../ride/RideData.h"
#include "../../../ride/TrackData.h"
#include "../../../ride/TrackPaint.h"
#include "../../../world/Map.h"
#include "../../../world/tile_element/TrackElement.h"
#include "../../Paint.h"
#include "../../support/WoodenSupports.h"
#include "../../support/WoodenSupports.hpp"
#include "../../tile_element/Paint.TileElement.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Square;

static void PaintReverseFreefallRCFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<TrackElemType::Flat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    if (direction & 1)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void PaintReverseFreefallRCStation(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    TrackPaintUtilDrawNarrowStationPlatform(session, ride, direction, height, 5, trackElement, StationBaseType::b, -2);
    DrawSupportForSequenceA<TrackElemType::EndStation>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    TrackPaintUtilDrawStationTunnel(session, direction, height);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void PaintReverseFreefallRCSlope(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintReverseFreefallSlope(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<TrackElemType::ReverseFreefallSlope>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    if (trackSequence == 0)
    {
        if (direction & 1)
        {
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
        }
        else
        {
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
        }
    }
    if (trackSequence == 6)
    {
        PaintUtilSetVerticalTunnel(session, height + 240);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    static constexpr int32_t supportHeights[] = { 48, 64, 128, 176, 208, 240, 240 };
    PaintUtilSetGeneralSupportHeight(session, height + supportHeights[trackSequence]);
}

static void PaintReverseFreefallRCVertical(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintReverseFreefallVertical(session, ride, trackSequence, direction, height, trackElement, supportType);
    if (trackSequence == 1)
    {
        PaintUtilSetVerticalTunnel(session, height + 80);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 80);
}

static void PaintReverseFreefallRCOnridePhoto(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteOnRidePhoto(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<TrackElemType::OnRidePhoto>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
}

TrackPaintFunction GetTrackPaintFunctionReverseFreefallRC(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return PaintReverseFreefallRCFlat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return PaintReverseFreefallRCStation;
        case TrackElemType::ReverseFreefallSlope:
            return PaintReverseFreefallRCSlope;
        case TrackElemType::ReverseFreefallVertical:
            return PaintReverseFreefallRCVertical;
        case TrackElemType::OnRidePhoto:
            return PaintReverseFreefallRCOnridePhoto;
        default:
            return TrackPaintFunctionDummy;
    }
}
