/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include "../../../ride/Vehicle.h"
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

/** rct2: 0x0086E65C */
static void ReverserRCTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21520), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21521), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21504), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21505), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                break;
        }
    }
    DrawSupportForSequenceA<TrackElemType::flat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void ReverserRCTrackStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr ImageIndex imageIds[4] = {
        21506,
        21507,
        21506,
        21507,
    };
    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours.WithIndex(imageIds[direction]), { 0, 0, height },
        { { 0, 2, height + 1 }, { 32, 27, 2 } });
    DrawSupportForSequenceA<TrackElemType::endStation>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    TrackPaintUtilDrawStation2(session, ride, direction, height, trackElement, StationBaseType::a, 0, 9, 11);
    TrackPaintUtilDrawStationTunnel(session, direction, height);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0086E66C */
static void ReverserRCTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21530), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21531), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21532), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21533), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21516), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21517), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21518), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21519), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                break;
        }
    }
    DrawSupportForSequenceA<TrackElemType::up25>(
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

/** rct2: 0x0086E67C */
static void ReverserRCTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21522), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21523), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21524), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21525), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21508), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21509), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21510), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21511), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                break;
        }
    }
    DrawSupportForSequenceA<TrackElemType::flatToUp25>(
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

/** rct2: 0x0086E68C */
static void ReverserRCTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21526), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21527), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21528), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21529), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21512), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21513), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21514), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(21515), { 0, 0, height },
                    { { 0, 2, height }, { 32, 27, 2 } });
                break;
        }
    }
    DrawSupportForSequenceA<TrackElemType::up25ToFlat>(
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

/** rct2: 0x0086E69C */
static void ReverserRCTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ReverserRCTrack25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0086E6AC */
static void ReverserRCTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ReverserRCTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0086E6BC */
static void ReverserRCTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ReverserRCTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0086E6CC */
static void ReverserRCTrackLeftQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21543), { 0, 2, height }, { 32, 27, 2 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21548), { 0, 2, height }, { 32, 27, 2 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21553), { 0, 2, height }, { 32, 32, 2 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21538), { 0, 2, height }, { 32, 32, 2 });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::leftQuarterTurn5Tiles>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight), direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21542), { 0, 0, height }, { 32, 16, 2 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21547), { 0, 0, height }, { 32, 16, 2 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21552), { 0, 16, height }, { 32, 16, 2 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21537), { 0, 16, height }, { 32, 16, 2 });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::leftQuarterTurn5Tiles>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21541), { 0, 16, height }, { 16, 16, 2 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21546), { 16, 16, height }, { 16, 16, 2 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21551), { 16, 0, height }, { 16, 16, 2 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21536), { 0, 0, height }, { 16, 16, 2 });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::leftQuarterTurn5Tiles>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::right, PaintSegment::bottom, PaintSegment::centre,
                        PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight), direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21540), { 16, 0, height }, { 16, 34, 2 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21545), { 0, 0, height }, { 16, 32, 2 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21550), { 0, 0, height }, { 16, 32, 2 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21535), { 16, 0, height }, { 16, 32, 2 });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::leftQuarterTurn5Tiles>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21539), { 2, 0, height }, { 32, 32, 2 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21544), { 2, 0, height }, { 27, 32, 2 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21549), { 2, 0, height }, { 27, 32, 2 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21534), { 2, 0, height }, { 32, 32, 2 });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::leftQuarterTurn5Tiles>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x0086E6DC */
static void ReverserRCTrackRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    ReverserRCTrackLeftQuarterTurn5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x0086E6EC */
static void ReverserRCTrackSBendLeft(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21566), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21570), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21569), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21573), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::sBendLeft>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21567), { 0, 0, height },
                        { { 0, 0, height }, { 32, 26, 2 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21571), { 0, 0, height },
                        { { 0, 0, height }, { 32, 26, 2 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21568), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 2 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21572), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 2 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::sBendLeft>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21568), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 2 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21572), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 2 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21567), { 0, 0, height },
                        { { 0, 0, height }, { 32, 26, 2 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21571), { 0, 0, height },
                        { { 0, 0, height }, { 32, 26, 2 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::sBendLeft>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21569), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21573), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21566), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21570), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::sBendLeft>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x0086E6FC */
static void ReverserRCTrackSBendRight(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21574), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21578), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21577), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21581), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::sBendRight>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21575), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 2 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21579), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 2 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21576), { 0, 0, height },
                        { { 0, 0, height }, { 32, 26, 2 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21580), { 0, 0, height },
                        { { 0, 0, height }, { 32, 26, 2 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::sBendRight>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21576), { 0, 0, height },
                        { { 0, 0, height }, { 32, 26, 2 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21580), { 0, 0, height },
                        { { 0, 0, height }, { 32, 26, 2 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21575), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 2 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21579), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 2 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::sBendRight>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21577), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21581), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21574), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21578), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::sBendRight>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x0086E73C */
static void ReverserRCTrackLeftQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21559), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21562), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21565), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21556), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 2 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::leftQuarterTurn3Tiles>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21558), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 2 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21561), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 2 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21564), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 2 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21555), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 2 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21557), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21560), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21563), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21554), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 2 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::leftQuarterTurn3Tiles>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x0086E74C */
static void ReverserRCTrackRightQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    ReverserRCTrackLeftQuarterTurn3(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x0086E75C */
static void ReverserRCTrackBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(21506), { 0, 0, height },
                { { 0, 2, height }, { 32, 27, 2 } });
            break;
        case 1:
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(21507), { 0, 0, height },
                { { 0, 2, height }, { 32, 27, 2 } });
            break;
    }
    DrawSupportForSequenceA<TrackElemType::brakes>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0086E76C */
static void ReverserRCTrackLeftReverser(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21582), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21588), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21594), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21600), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::leftReverser>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21585), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21591), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21597), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21603), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::leftReverser>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21586), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21592), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21598), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21604), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::leftReverser>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21587), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21593), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21599), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21605), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::leftReverser>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21583), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21589), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21595), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21601), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::leftReverser>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21584), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21590), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21596), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21602), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::leftReverser>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0086E77C */
static void ReverserRCTrackRightReverser(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21596), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21602), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21584), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21590), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::rightReverser>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21599), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21605), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21587), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21593), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::rightReverser>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21598), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21604), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21586), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21592), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::rightReverser>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21597), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21603), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21585), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21591), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::rightReverser>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21595), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21601), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21583), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21589), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::rightReverser>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21594), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21600), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21582), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(21588), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 2 } });
                    break;
            }
            DrawSupportForSequenceA<TrackElemType::rightReverser>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

TrackPaintFunction GetTrackPaintFunctionReverserRC(TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::flat:
            return ReverserRCTrackFlat;
        case TrackElemType::endStation:
        case TrackElemType::beginStation:
        case TrackElemType::middleStation:
            return ReverserRCTrackStation;
        case TrackElemType::up25:
            return ReverserRCTrack25DegUp;
        case TrackElemType::flatToUp25:
            return ReverserRCTrackFlatTo25DegUp;
        case TrackElemType::up25ToFlat:
            return ReverserRCTrack25DegUpToFlat;
        case TrackElemType::down25:
            return ReverserRCTrack25DegDown;
        case TrackElemType::flatToDown25:
            return ReverserRCTrackFlatTo25DegDown;
        case TrackElemType::down25ToFlat:
            return ReverserRCTrack25DegDownToFlat;
        case TrackElemType::leftQuarterTurn5Tiles:
            return ReverserRCTrackLeftQuarterTurn5;
        case TrackElemType::rightQuarterTurn5Tiles:
            return ReverserRCTrackRightQuarterTurn5;
        case TrackElemType::sBendLeft:
            return ReverserRCTrackSBendLeft;
        case TrackElemType::sBendRight:
            return ReverserRCTrackSBendRight;
        case TrackElemType::leftQuarterTurn3Tiles:
            return ReverserRCTrackLeftQuarterTurn3;
        case TrackElemType::rightQuarterTurn3Tiles:
            return ReverserRCTrackRightQuarterTurn3;
        case TrackElemType::brakes:
            return ReverserRCTrackBrakes;
        case TrackElemType::leftReverser:
            return ReverserRCTrackLeftReverser;
        case TrackElemType::rightReverser:
            return ReverserRCTrackRightReverser;
        default:
            return TrackPaintFunctionDummy;
    }
}
