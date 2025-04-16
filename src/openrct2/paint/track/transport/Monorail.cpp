/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../interface/Viewport.h"
#include "../../../object/StationObject.h"
#include "../../../ride/RideData.h"
#include "../../../ride/Track.h"
#include "../../../ride/TrackPaint.h"
#include "../../../world/Map.h"
#include "../../Boundbox.h"
#include "../../Paint.h"
#include "../../support/MetalSupports.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Square;

/** rct2: 0x008AE1AC */
static void PaintMonorailTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (direction == 0 || direction == 2)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 0, height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AE25C, 0x008AE26C, 0x008AE27C */
static void PaintMonorailStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (direction == 0 || direction == 2)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (TrackPaintUtilDrawStation(session, ride, direction, height, trackElement, StationBaseType::b, -2))
    {
        DrawSupportsSideBySide(session, direction, height, session.SupportColours, supportType.metal);
    }
    else if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, direction, 0, height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AE1BC */
static void PaintMonorailTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            break;
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 8, height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x008AE1CC */
static void PaintMonorailTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 3, height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008AE1DC */
static void PaintMonorailTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height - 8, kTunnelGroup, TunnelSubType::Flat);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height - 8, kTunnelGroup, TunnelSubType::Flat);
            break;
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 6, height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

/** rct2: 0x008AE1EC */
static void PaintMonorailTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintMonorailTrack25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x008AE1FC */
static void PaintMonorailTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintMonorailTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x008AE20C */
static void PaintMonorailTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintMonorailTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x008AE22C */
static void PaintMonorailTrackRightQuarterTurn5Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    switch (trackSequence)
    {
        case 0:
        case 6:
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 0, height, session.SupportColours);
            break;
    }

    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (direction == 0 && trackSequence == 6)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (direction == 1 && trackSequence == 6)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::right);
            break;
        case 2:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottomRight, PaintSegment::right,
                PaintSegment::bottom, PaintSegment::topRight);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::centre);
            break;
        case 5:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomRight, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::left,
                PaintSegment::bottom, PaintSegment::topLeft);
            break;
        case 6:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomRight, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::left);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AE21C */
static void PaintMonorailTrackLeftQuarterTurn5Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    PaintMonorailTrackRightQuarterTurn5Tiles(
        session, ride, trackSequence, (direction + 1) % 4, height, trackElement, supportType);
}

/** rct2: 0x */
static void PaintMonorailTrackSBendLeft(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    uint8_t originalTrackSequence = trackSequence;
    if (direction == 2 || direction == 3)
    {
        trackSequence = 3 - trackSequence;
    }

    if (direction == 0 || direction == 2)
    {
        if (trackSequence == 0)
        {
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
        }
    }
    else
    {
        if (trackSequence == 3)
        {
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
        }
    }
    DrawSBendLeftSupports(session, supportType.metal, originalTrackSequence, direction, height, 0, 0);

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::top);
            break;
        case 1:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::left,
                PaintSegment::topLeft, PaintSegment::top);
            break;
        case 2:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom,
                PaintSegment::bottomRight, PaintSegment::right);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom);
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction & 1), 0xFFFF, 0);

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AE24C */
static void PaintMonorailTrackSBendRight(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    uint8_t originalTrackSequence = trackSequence;
    if (direction == 2 || direction == 3)
    {
        trackSequence = 3 - trackSequence;
    }

    if (direction == 0 || direction == 2)
    {
        if (trackSequence == 0)
        {
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
        }
    }
    else
    {
        if (trackSequence == 3)
        {
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
        }
    }
    DrawSBendRightSupports(session, supportType.metal, originalTrackSequence, direction, height, 0, 0);

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::right);
            break;
        case 1:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom,
                PaintSegment::bottomRight, PaintSegment::right);
            break;
        case 2:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::left,
                PaintSegment::topLeft, PaintSegment::top);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::left);
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction & 1), 0xFFFF, 0);

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AE29C */
static void PaintMonorailTrackRightQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    TrackPaintUtilRightQuarterTurn3TilesTunnel(session, kTunnelGroup, TunnelSubType::Flat, height, direction, trackSequence);

    switch (trackSequence)
    {
        case 0:
        case 3:
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 0, height, session.SupportColours);
            break;
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::right);
            break;
        case 2:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottomRight, PaintSegment::bottom);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomRight, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::left);
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AE28C */
static void PaintMonorailTrackLeftQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    PaintMonorailTrackRightQuarterTurn3Tiles(
        session, ride, trackSequence, (direction + 1) % 4, height, trackElement, supportType);
}

/** rct2: 0x008AE31C */
static void PaintMonorailTrackLeftEighthToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    switch (trackSequence)
    {
        case 0:
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 0, height, session.SupportColours);
            break;
        case 4:
            if (direction == 0)
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::BottomCorner, 0, height, session.SupportColours);
            if (direction == 1)
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::LeftCorner, 0, height, session.SupportColours);
            if (direction == 2)
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::TopCorner, 0, height, session.SupportColours);
            if (direction == 3)
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::RightCorner, 0, height, session.SupportColours);
            break;
    }

    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight);
            break;
        case 1:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::left,
                PaintSegment::topLeft, PaintSegment::top);
            break;
        case 2:
            blockedSegments = EnumsToFlags(
                PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom, PaintSegment::bottomRight,
                PaintSegment::right);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::left, PaintSegment::topLeft);
            break;
        case 4:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::topLeft,
                PaintSegment::bottom, PaintSegment::bottomRight);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AE32C */
static void PaintMonorailTrackRightEighthToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    switch (trackSequence)
    {
        case 0:
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 0, height, session.SupportColours);
            break;
        case 4:
            if (direction == 0)
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::LeftCorner, 0, height, session.SupportColours);
            if (direction == 1)
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::TopCorner, 0, height, session.SupportColours);
            if (direction == 2)
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::RightCorner, 0, height, session.SupportColours);
            if (direction == 3)
                MetalASupportsPaintSetup(
                    session, supportType.metal, MetalSupportPlace::BottomCorner, 0, height, session.SupportColours);
            break;
    }

    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight);
            break;
        case 1:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom,
                PaintSegment::bottomRight, PaintSegment::right);
            break;
        case 2:
            blockedSegments = EnumsToFlags(
                PaintSegment::centre, PaintSegment::topRight, PaintSegment::left, PaintSegment::topLeft, PaintSegment::top);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight);
            break;
        case 4:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::left,
                PaintSegment::topLeft, PaintSegment::bottomRight);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AE33C */
static void PaintMonorailTrackLeftEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    PaintMonorailTrackRightEighthToDiag(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x008AE34C */
static void PaintMonorailTrackRightEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    PaintMonorailTrackLeftEighthToDiag(session, ride, trackSequence, (direction + 3) % 4, height, trackElement, supportType);
}

/** rct2: 0x008AE2AC */
static void PaintMonorailTrackDiagFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    if (SupportedSequences::kDiagStraightFlat[trackSequence] != MetalSupportPlace::None)
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, SupportedSequences::kDiagStraightFlat[trackSequence], direction, 0, height,
            session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(BlockedSegments::kDiagStraightFlat[trackSequence], direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AE2DC */
static void PaintMonorailTrackDiag25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (trackSequence == 3)
    {
        MetalBSupportsPaintSetup(
            session, supportType.metal, kDiagSupportPlacement[direction], 8, height, session.SupportColours);
    }

    int32_t blockedSegments = BlockedSegments::kDiagStraightFlat[trackSequence];
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x008AE2BC */
static void PaintMonorailTrackDiagFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (trackSequence == 3)
    {
        MetalBSupportsPaintSetup(
            session, supportType.metal, kDiagSupportPlacement[direction], 0, height, session.SupportColours);
    }

    int32_t blockedSegments = BlockedSegments::kDiagStraightFlat[trackSequence];
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008AE2CC */
static void PaintMonorailTrackDiag25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (trackSequence == 3)
    {
        MetalBSupportsPaintSetup(
            session, supportType.metal, kDiagSupportPlacement[direction], 4, height, session.SupportColours);
    }

    int32_t blockedSegments = BlockedSegments::kDiagStraightFlat[trackSequence];
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x008AE30C */
static void PaintMonorailTrackDiag25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintMonorailTrackDiag25DegUp(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x008AE2EC */
static void PaintMonorailTrackDiagFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintMonorailTrackDiag25DegUpToFlat(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x008AE2FC */
static void PaintMonorailTrackDiag25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintMonorailTrackDiagFlatTo25DegUp(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/**
 * rct2: 0x008ADF34
 */
TrackPaintFunction GetTrackPaintFunctionMonorail(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return PaintMonorailTrackFlat;

        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return PaintMonorailStation;

        case TrackElemType::Up25:
            return PaintMonorailTrack25DegUp;
        case TrackElemType::FlatToUp25:
            return PaintMonorailTrackFlatTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return PaintMonorailTrack25DegUpToFlat;

        case TrackElemType::Down25:
            return PaintMonorailTrack25DegDown;
        case TrackElemType::FlatToDown25:
            return PaintMonorailTrackFlatTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return PaintMonorailTrack25DegDownToFlat;

        case TrackElemType::LeftQuarterTurn5Tiles:
            return PaintMonorailTrackLeftQuarterTurn5Tiles;
        case TrackElemType::RightQuarterTurn5Tiles:
            return PaintMonorailTrackRightQuarterTurn5Tiles;

        case TrackElemType::SBendLeft:
            return PaintMonorailTrackSBendLeft;
        case TrackElemType::SBendRight:
            return PaintMonorailTrackSBendRight;

        case TrackElemType::LeftQuarterTurn3Tiles:
            return PaintMonorailTrackLeftQuarterTurn3Tiles;
        case TrackElemType::RightQuarterTurn3Tiles:
            return PaintMonorailTrackRightQuarterTurn3Tiles;

        case TrackElemType::LeftEighthToDiag:
            return PaintMonorailTrackLeftEighthToDiag;
        case TrackElemType::RightEighthToDiag:
            return PaintMonorailTrackRightEighthToDiag;
        case TrackElemType::LeftEighthToOrthogonal:
            return PaintMonorailTrackLeftEighthToOrthogonal;
        case TrackElemType::RightEighthToOrthogonal:
            return PaintMonorailTrackRightEighthToOrthogonal;

        case TrackElemType::DiagFlat:
            return PaintMonorailTrackDiagFlat;

        case TrackElemType::DiagUp25:
            return PaintMonorailTrackDiag25DegUp;
        case TrackElemType::DiagFlatToUp25:
            return PaintMonorailTrackDiagFlatTo25DegUp;
        case TrackElemType::DiagUp25ToFlat:
            return PaintMonorailTrackDiag25DegUpToFlat;

        case TrackElemType::DiagDown25:
            return PaintMonorailTrackDiag25DegDown;
        case TrackElemType::DiagFlatToDown25:
            return PaintMonorailTrackDiagFlatTo25DegDown;
        case TrackElemType::DiagDown25ToFlat:
            return PaintMonorailTrackDiag25DegDownToFlat;
        default:
            return TrackPaintFunctionDummy;
    }
}
