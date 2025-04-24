/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../interface/Viewport.h"
#include "../../../ride/Track.h"
#include "../../../ride/TrackPaint.h"
#include "../../../ride/Vehicle.h"
#include "../../Paint.h"
#include "../../support/WoodenSupports.h"
#include "../../support/WoodenSupports.hpp"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Square;

static void PaintSplashBoatsTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<TrackElemType::Up25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

static void PaintSplashBoatsTrack60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<TrackElemType::Up60>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 56, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104);
}

static void PaintSplashBoatsTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<TrackElemType::FlatToUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

static void PaintSplashBoatsTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<TrackElemType::Up25ToFlat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

static void PaintSplashBoatsTrack25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<TrackElemType::Up25ToUp60>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

static void PaintSplashBoatsTrack60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<TrackElemType::Up60ToUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

static void PaintSplashBoatsTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<TrackElemType::Down25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

static void PaintSplashBoatsTrack60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintSplashBoatsTrack60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void PaintSplashBoatsTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<TrackElemType::FlatToDown25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::Flat);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

static void PaintSplashBoatsTrack25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintSplashBoatsTrack60DegUpTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void PaintSplashBoatsTrack60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintSplashBoatsTrack25DegUpTo60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void PaintSplashBoatsTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<TrackElemType::Down25ToFlat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** Start of elements originally from River Rafts */
/** rct2: 0x0089B170 */
static void PaintSplashBoatsTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

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

/** rct2: 0x0089B1A0 */
static void PaintSplashBoatsStation(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2Platformless(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<TrackElemType::EndStation>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    TrackPaintUtilDrawNarrowStationPlatform(session, ride, direction, height, 7, trackElement, StationBaseType::b, 0);

    TrackPaintUtilDrawStationTunnel(session, direction, height);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0089B1D0 */
static void PaintSplashBoatsTrackLeftQuarterTurn5Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<TrackElemType::LeftQuarterTurn5Tiles>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight), direction),
                0xFFFF, 0);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::top,
                        PaintSegment::left, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right, PaintSegment::centre,
                        PaintSegment::top, PaintSegment::bottom, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight), direction),
                0xFFFF, 0);
            break;
        case 5:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottomRight, PaintSegment::bottomLeft,
                        PaintSegment::bottom, PaintSegment::left),
                    direction),
                0xFFFF, 0);
            break;
        case 6:
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            break;
    }

    switch (trackSequence)
    {
        case 0:
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            break;
        case 6:
            if (direction == 2 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction ^ 1, height, kTunnelGroup, TunnelSubType::Flat);
            }
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0089B1D0 */
static void PaintSplashBoatsTrackRightQuarterTurn5Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<TrackElemType::RightQuarterTurn5Tiles>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight), direction),
                0xFFFF, 0);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottomRight, PaintSegment::right,
                        PaintSegment::bottom, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::centre,
                        PaintSegment::left, PaintSegment::right, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight), direction),
                0xFFFF, 0);
            break;
        case 5:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomRight, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::left,
                        PaintSegment::bottom, PaintSegment::topLeft),
                    direction),
                0xFFFF, 0);
            break;
        case 6:
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            break;
    }

    switch (trackSequence)
    {
        case 0:
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            break;
        case 6:
            if (direction == 0 || direction == 1)
            {
                PaintUtilPushTunnelRotated(session, direction ^ 1, height, kTunnelGroup, TunnelSubType::Flat);
            }
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0089B180 */
static void PaintSplashBoatsTrackSBendLeft(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    switch (trackSequence)
    {
        case 0:
            DrawSupportForSequenceA<TrackElemType::SBendLeft>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            break;
        case 1:
            DrawSupportForSequenceA<TrackElemType::SBendLeft>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            break;
        case 2:
            DrawSupportForSequenceA<TrackElemType::SBendLeft>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            break;
        case 3:
            DrawSupportForSequenceA<TrackElemType::SBendLeft>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            break;
    }

    if (trackSequence == 0)
    {
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        }
    }
    else if (trackSequence == 3)
    {
        if (direction == 1 || direction == 2)
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        }
    }

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0089B190 */
static void PaintSplashBoatsTrackSBendRight(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    switch (trackSequence)
    {
        case 0:
            DrawSupportForSequenceA<TrackElemType::SBendRight>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            break;
        case 1:
            DrawSupportForSequenceA<TrackElemType::SBendRight>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            break;
        case 2:
            DrawSupportForSequenceA<TrackElemType::SBendRight>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            break;
        case 3:
            DrawSupportForSequenceA<TrackElemType::SBendRight>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            break;
    }

    if (trackSequence == 0)
    {
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        }
    }
    else if (trackSequence == 3)
    {
        if (direction == 1 || direction == 2)
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        }
    }

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void PaintSplashBoatsTrackOnRidePhoto(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2OnRidePhoto(session, ride, trackSequence, direction, height, trackElement, supportType);

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

TrackPaintFunction GetTrackPaintFunctionSplashBoats(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Up25:
            return PaintSplashBoatsTrack25DegUp;
        case TrackElemType::Up60:
            return PaintSplashBoatsTrack60DegUp;
        case TrackElemType::FlatToUp25:
            return PaintSplashBoatsTrackFlatTo25DegUp;
        case TrackElemType::Up25ToUp60:
            return PaintSplashBoatsTrack25DegUpTo60DegUp;
        case TrackElemType::Up60ToUp25:
            return PaintSplashBoatsTrack60DegUpTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return PaintSplashBoatsTrack25DegUpToFlat;
        case TrackElemType::Down25:
            return PaintSplashBoatsTrack25DegDown;
        case TrackElemType::Down60:
            return PaintSplashBoatsTrack60DegDown;
        case TrackElemType::FlatToDown25:
            return PaintSplashBoatsTrackFlatTo25DegDown;
        case TrackElemType::Down25ToDown60:
            return PaintSplashBoatsTrack25DegDownTo60DegDown;
        case TrackElemType::Down60ToDown25:
            return PaintSplashBoatsTrack60DegDownTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return PaintSplashBoatsTrack25DegDownToFlat;

        // Originally taken from River Rafts
        case TrackElemType::Flat:
            return PaintSplashBoatsTrackFlat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return PaintSplashBoatsStation;
        case TrackElemType::LeftQuarterTurn5Tiles:
            return PaintSplashBoatsTrackLeftQuarterTurn5Tiles;
        case TrackElemType::RightQuarterTurn5Tiles:
            return PaintSplashBoatsTrackRightQuarterTurn5Tiles;
        case TrackElemType::SBendLeft:
            return PaintSplashBoatsTrackSBendLeft;
        case TrackElemType::SBendRight:
            return PaintSplashBoatsTrackSBendRight;

        // Added by OpenRCT2
        case TrackElemType::OnRidePhoto:
            return PaintSplashBoatsTrackOnRidePhoto;
        default:
            return TrackPaintFunctionDummy;
    }
}
