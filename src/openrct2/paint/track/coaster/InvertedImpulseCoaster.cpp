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
#include "../../support/MetalSupports.h"
#include "../../tile_element/Paint.TileElement.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Inverted;

/** rct2: 0x008B0460 */
static void InvertedImpulseRCTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 0, height + 44, session.SupportColours);
    }

    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008B0470, 0x008B0480, 0x008B0490 */
static void InvertedImpulseRCTrackStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteWithChildSupportColoursPlatformless(
        session, ride, trackSequence, direction, height, trackElement, supportType);
    if (TrackPaintUtilDrawStationInverted(session, ride, direction, height, trackElement, STATION_VARIANT_TALL))
    {
        DrawSupportsSideBySide(session, direction, height, session.SupportColours, supportType.metal);
    }
    TrackPaintUtilDrawStationTunnelTall(session, direction, height);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008B04A0 */
static void InvertedImpulseRCTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::TopRightSide, 0, height + 62, session.SupportColours);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::BottomRightSide, 0, height + 62, session.SupportColours);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::BottomLeftSide, 0, height + 62, session.SupportColours);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::TopLeftSide, 0, height + 62, session.SupportColours);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

/** rct2: 0x008B04B0 */
static void InvertedImpulseRCTrack60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 56, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 120);
}

/** rct2: 0x008B04C0 */
static void InvertedImpulseRCTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    switch (direction)
    {
        case 0:
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::TopRightSide, 0, height + 54, session.SupportColours);
            break;
        case 1:
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::BottomRightSide, 0, height + 54, session.SupportColours);
            break;
        case 2:
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::BottomLeftSide, 0, height + 54, session.SupportColours);
            break;
        case 3:
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::TopLeftSide, 0, height + 54, session.SupportColours);
            break;
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 64);
}

/** rct2: 0x008B04D0 */
static void InvertedImpulseRCTrack25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 88);
}

/** rct2: 0x008B04E0 */
static void InvertedImpulseRCTrack60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 88);
}

/** rct2: 0x008B04F0 */
static void InvertedImpulseRCTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    switch (direction)
    {
        case 0:
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::TopRightSide, 0, height + 52, session.SupportColours);
            break;
        case 1:
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::BottomRightSide, 0, height + 52, session.SupportColours);
            break;
        case 2:
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::BottomLeftSide, 0, height + 52, session.SupportColours);
            break;
        case 3:
            MetalASupportsPaintSetup(
                session, supportType.metal, MetalSupportPlace::TopLeftSide, 0, height + 52, session.SupportColours);
            break;
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x008B0500 */
static void InvertedImpulseRCTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedImpulseRCTrack25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008B0510 */
static void InvertedImpulseRCTrack60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedImpulseRCTrack60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008B0520 */
static void InvertedImpulseRCTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedImpulseRCTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008B0530 */
static void InvertedImpulseRCTrack25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedImpulseRCTrack60DegUpTo25DegUp(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008B0540 */
static void InvertedImpulseRCTrack60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedImpulseRCTrack25DegUpTo60DegUp(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008B0550 */
static void InvertedImpulseRCTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedImpulseRCTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008B05A0 */
static void InvertedImpulseRCTrack90DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            PaintUtilSetVerticalTunnel(session, height + 32);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            break;
    }
}

/** rct2: 0x008B05B0 */
static void InvertedImpulseRCTrack90DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedImpulseRCTrack90DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008B0560 */
static void InvertedImpulseRCTrack60DegUpTo90DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetVerticalTunnel(session, height + 56);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            break;
    }
}

/** rct2: 0x008B0570 */
static void InvertedImpulseRCTrack90DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    direction = (direction + 2) & 3;
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    PaintUtilSetVerticalTunnel(session, height + 56);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

/** rct2: 0x008B0580 */
static void InvertedImpulseRCTrack90DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (direction)
    {
        case 1:
            PaintUtilPushTunnelRight(session, height + 48, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 48, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 96);
}

/** rct2: 0x008B0590 */
static void InvertedImpulseRCTrack60DegDownTo90DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 48, kTunnelGroup, TunnelSubType::SlopeEnd);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 96);
            break;
        case 1:
            break;
    }
}

/** rct2: 0x008B05C0 */
static void InvertedImpulseRCTrackLeftQuarterTurn190DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            PaintUtilSetVerticalTunnel(session, height + 96);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 96);
            break;
        case 1:
            break;
    }
}

/** rct2: 0x008B05D0 */
static void InvertedImpulseRCTrackRightQuarterTurn190DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            PaintUtilSetVerticalTunnel(session, height + 96);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 96);
            break;
        case 1:
            break;
    }
}

/** rct2: 0x008B05E0 */
static void InvertedImpulseRCTrackLeftQuarterTurn190DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedImpulseRCTrackRightQuarterTurn190DegUp(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008B05F0 */
static void InvertedImpulseRCTrackRightQuarterTurn190DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    InvertedImpulseRCTrackLeftQuarterTurn190DegUp(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

TrackPaintFunction GetTrackPaintFunctionInvertedImpulseRC(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return InvertedImpulseRCTrackFlat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return InvertedImpulseRCTrackStation;
        case TrackElemType::Up25:
            return InvertedImpulseRCTrack25DegUp;
        case TrackElemType::Up60:
            return InvertedImpulseRCTrack60DegUp;
        case TrackElemType::FlatToUp25:
            return InvertedImpulseRCTrackFlatTo25DegUp;
        case TrackElemType::Up25ToUp60:
            return InvertedImpulseRCTrack25DegUpTo60DegUp;
        case TrackElemType::Up60ToUp25:
            return InvertedImpulseRCTrack60DegUpTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return InvertedImpulseRCTrack25DegUpToFlat;
        case TrackElemType::Down25:
            return InvertedImpulseRCTrack25DegDown;
        case TrackElemType::Down60:
            return InvertedImpulseRCTrack60DegDown;
        case TrackElemType::FlatToDown25:
            return InvertedImpulseRCTrackFlatTo25DegDown;
        case TrackElemType::Down25ToDown60:
            return InvertedImpulseRCTrack25DegDownTo60DegDown;
        case TrackElemType::Down60ToDown25:
            return InvertedImpulseRCTrack60DegDownTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return InvertedImpulseRCTrack25DegDownToFlat;
        case TrackElemType::Up90:
            return InvertedImpulseRCTrack90DegUp;
        case TrackElemType::Down90:
            return InvertedImpulseRCTrack90DegDown;
        case TrackElemType::Up60ToUp90:
            return InvertedImpulseRCTrack60DegUpTo90DegUp;
        case TrackElemType::Down90ToDown60:
            return InvertedImpulseRCTrack90DegDownTo60DegDown;
        case TrackElemType::Up90ToUp60:
            return InvertedImpulseRCTrack90DegUpTo60DegUp;
        case TrackElemType::Down60ToDown90:
            return InvertedImpulseRCTrack60DegDownTo90DegDown;
        case TrackElemType::LeftQuarterTurn1TileUp90:
            return InvertedImpulseRCTrackLeftQuarterTurn190DegUp;
        case TrackElemType::RightQuarterTurn1TileUp90:
            return InvertedImpulseRCTrackRightQuarterTurn190DegUp;
        case TrackElemType::LeftQuarterTurn1TileDown90:
            return InvertedImpulseRCTrackLeftQuarterTurn190DegDown;
        case TrackElemType::RightQuarterTurn1TileDown90:
            return InvertedImpulseRCTrackRightQuarterTurn190DegDown;
        default:
            return TrackPaintFunctionDummy;
    }
}
