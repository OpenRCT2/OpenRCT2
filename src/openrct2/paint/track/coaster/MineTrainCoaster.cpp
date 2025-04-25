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

/** rct2: 0x0071BFA4 */
static void MineTrainRCTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<TrackElemType::Flat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0071C154, 0x0071C164, 0x0071C174 */
static void MineTrainRCTrackStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.GetTrackType() == TrackElemType::EndStation)
    {
        trackPaintSpriteBrakeStation(session, ride, trackSequence, direction, height, trackElement, supportType);
    }
    else
    {
        trackPaintSpriteStation(session, ride, trackSequence, direction, height, trackElement, supportType);
    }
    DrawSupportsSideBySide(session, direction, height, session.SupportColours, MetalSupportType::Boxed);
    TrackPaintUtilDrawStationTunnel(session, direction, height);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0071BFB4 */
static void MineTrainRCTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);

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

/** rct2: 0x0071BFC4 */
static void MineTrainRCTrack60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

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

/** rct2: 0x0071BFD4 */
static void MineTrainRCTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);

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

/** rct2: 0x0071BFE4 */
static void MineTrainRCTrack25DegUpTo60DegUp(
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

/** rct2: 0x0071BFF4 */
static void MineTrainRCTrack60DegUpTo25DegUp(
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

/** rct2: 0x0071C004 */
static void MineTrainRCTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);

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

/** rct2: 0x0071C014 */
static void MineTrainRCTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrack25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C024 */
static void MineTrainRCTrack60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrack60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C034 */
static void MineTrainRCTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C044 */
static void MineTrainRCTrack25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrack60DegUpTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C054 */
static void MineTrainRCTrack60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrack25DegUpTo60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C064 */
static void MineTrainRCTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C0B4 */
static void MineTrainRCTrackLeftQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            DrawSupportForSequenceA<TrackElemType::LeftQuarterTurn5Tiles>(
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
            DrawSupportForSequenceA<TrackElemType::LeftQuarterTurn5Tiles>(
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
            DrawSupportForSequenceA<TrackElemType::LeftQuarterTurn5Tiles>(
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
            DrawSupportForSequenceA<TrackElemType::LeftQuarterTurn5Tiles>(
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
            DrawSupportForSequenceA<TrackElemType::LeftQuarterTurn5Tiles>(
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

/** rct2: 0x0071C0C4 */
static void MineTrainRCTrackRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    MineTrainRCTrackLeftQuarterTurn5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C074 */
static void MineTrainRCTrackFlatToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<TrackElemType::FlatToLeftBank>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0071C084 */
static void MineTrainRCTrackFlatToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<TrackElemType::FlatToRightBank>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0071C094 */
static void MineTrainRCTrackLeftBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrackFlatToRightBank(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x0071C0A4 */
static void MineTrainRCTrackRightBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrackFlatToLeftBank(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x0071C0D4 */
static void MineTrainRCTrackBankedLeftQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            DrawSupportForSequenceA<TrackElemType::BankedLeftQuarterTurn5Tiles>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
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
            DrawSupportForSequenceA<TrackElemType::BankedLeftQuarterTurn5Tiles>(
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
            DrawSupportForSequenceA<TrackElemType::BankedLeftQuarterTurn5Tiles>(
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
            break;
        case 5:
            DrawSupportForSequenceA<TrackElemType::BankedLeftQuarterTurn5Tiles>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            break;
        case 6:
            DrawSupportForSequenceA<TrackElemType::BankedLeftQuarterTurn5Tiles>(
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
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0071C0E4 */
static void MineTrainRCTrackBankedRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    MineTrainRCTrackBankedLeftQuarterTurn5(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C204 */
static void MineTrainRCTrackLeftBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<TrackElemType::LeftBankToUp25>(
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

/** rct2: 0x0071C214 */
static void MineTrainRCTrackRightBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<TrackElemType::RightBankToUp25>(
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

/** rct2: 0x0071C224 */
static void MineTrainRCTrack25DegUpToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<TrackElemType::Up25ToLeftBank>(
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

/** rct2: 0x0071C234 */
static void MineTrainRCTrack25DegUpToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<TrackElemType::Up25ToRightBank>(
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

/** rct2: 0x0071C244 */
static void MineTrainRCTrackLeftBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrack25DegUpToRightBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C254 */
static void MineTrainRCTrackRightBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrack25DegUpToLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C264 */
static void MineTrainRCTrack25DegDownToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrackRightBankTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C274 */
static void MineTrainRCTrack25DegDownToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrackLeftBankTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C304 */
static void MineTrainRCTrackLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<TrackElemType::LeftBank>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0071C314 */
static void MineTrainRCTrackRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrackLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C0F4 */
static void MineTrainRCTrackLeftQuarterTurn525DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            DrawSupportForSequenceA<TrackElemType::LeftQuarterTurn5TilesUp25>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight), direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            DrawSupportForSequenceA<TrackElemType::LeftQuarterTurn5TilesUp25>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            DrawSupportForSequenceA<TrackElemType::LeftQuarterTurn5TilesUp25>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::right, PaintSegment::bottom, PaintSegment::centre,
                        PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight), direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 5:
            DrawSupportForSequenceA<TrackElemType::LeftQuarterTurn5TilesUp25>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours,
                        WoodenSupportTransitionType::Up25Deg, 3);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours,
                        WoodenSupportTransitionType::Up25Deg, 0);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours,
                        WoodenSupportTransitionType::Up25Deg, 1);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours,
                        WoodenSupportTransitionType::Up25Deg, 2);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x0071C104 */
static void MineTrainRCTrackRightQuarterTurn525DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::Up25Deg);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight), direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight), direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours,
                        WoodenSupportTransitionType::Up25Deg, 1);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours,
                        WoodenSupportTransitionType::Up25Deg, 2);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours,
                        WoodenSupportTransitionType::Up25Deg, 3);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours,
                        WoodenSupportTransitionType::Up25Deg, 0);
                    break;
            }
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x0071C114 */
static void MineTrainRCTrackLeftQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    MineTrainRCTrackRightQuarterTurn525DegUp(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C124 */
static void MineTrainRCTrackRightQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    MineTrainRCTrackLeftQuarterTurn525DegUp(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C134 */
static void MineTrainRCTrackSBendLeft(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
            }
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
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
            }
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
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
            }
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
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
            }
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

/** rct2: 0x0071C144 */
static void MineTrainRCTrackSBendRight(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
            }
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
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
            }
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
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
            }
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
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
            }
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

/** rct2: 0x0071C184 */
static void MineTrainRCTrackLeftQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner1, direction, height, session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
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
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
            }
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

/** rct2: 0x0071C194 */
static void MineTrainRCTrackRightQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    MineTrainRCTrackLeftQuarterTurn3(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C1A4 */
static void MineTrainRCTrackLeftQuarterTurn3Bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner1, direction, height, session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
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
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
            }
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

/** rct2: 0x0071C1B4 */
static void MineTrainRCTrackRightQuarterTurn3Bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    MineTrainRCTrackLeftQuarterTurn3Bank(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C1C4 */
static void MineTrainRCTrackLeftQuarterTurn325DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner1, direction, height, session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x0071C1D4 */
static void MineTrainRCTrackRightQuarterTurn325DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height, session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
            }
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x0071C1E4 */
static void MineTrainRCTrackLeftQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    MineTrainRCTrackRightQuarterTurn325DegUp(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C1F4 */
static void MineTrainRCTrackRightQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    MineTrainRCTrackLeftQuarterTurn325DegUp(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C284 */
static void MineTrainRCTrackLeftHalfBankedHelixUpSmall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::right, PaintSegment::bottom, PaintSegment::bottomRight),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                        PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner1, direction, height, session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
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
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::topRight),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
            }
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::right, PaintSegment::bottom, PaintSegment::topRight),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 5:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height, session.SupportColours);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 6:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::bottom, PaintSegment::topLeft),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x0071C294 */
static void MineTrainRCTrackRightHalfBankedHelixUpSmall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::bottom, PaintSegment::topLeft),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height, session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
            }
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::right, PaintSegment::bottom, PaintSegment::topRight),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::topRight),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 5:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner1, direction, height, session.SupportColours);

            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 6:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::right, PaintSegment::bottom, PaintSegment::bottomRight),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                        PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x0071C2A4 */
static void MineTrainRCTrackLeftHalfBankedHelixDownSmall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackSequence >= 4)
    {
        trackSequence -= 4;
        direction = (direction - 1) & 3;
    }
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    MineTrainRCTrackRightHalfBankedHelixUpSmall(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C2B4 */
static void MineTrainRCTrackRightHalfBankedHelixDownSmall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackSequence >= 4)
    {
        trackSequence -= 4;
        direction = (direction + 1) & 3;
    }
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    MineTrainRCTrackLeftHalfBankedHelixUpSmall(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C2C4 */
static void MineTrainRCTrackLeftHalfBankedHelixUpLarge(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::right, PaintSegment::bottom, PaintSegment::bottomRight),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                        PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight), direction),
                48, 0x20);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
            }
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
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::bottom, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight), direction),
                48, 0x20);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
            }
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
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::topRight),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
            }
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::right, PaintSegment::bottom, PaintSegment::topRight),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 8:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight), direction),
                48, 0x20);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
            }
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
        case 10:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::right, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 11:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight), direction),
                48, 0x20);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 12:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
            }
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
        case 13:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::bottom, PaintSegment::topLeft),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x0071C2D4 */
static void MineTrainRCTrackRightHalfBankedHelixUpLarge(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::bottom, PaintSegment::topLeft),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight), direction),
                48, 0x20);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
            }
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
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::right, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight), direction),
                48, 0x20);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
            }
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
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
            }
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::right, PaintSegment::bottom, PaintSegment::topRight),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::topRight),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 8:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight), direction),
                48, 0x20);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
            }
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
        case 10:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::bottom, PaintSegment::topLeft, PaintSegment::bottomLeft),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 11:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight), direction),
                48, 0x20);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 12:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
            }
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
        case 13:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::right, PaintSegment::bottom, PaintSegment::bottomRight),
                    direction),
                48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                        PaintSegment::bottomLeft),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x0071C2E4 */
static void MineTrainRCTrackLeftHalfBankedHelixDownLarge(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackSequence >= 7)
    {
        trackSequence -= 7;
        direction = (direction - 1) & 3;
    }
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    MineTrainRCTrackRightHalfBankedHelixUpLarge(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C2F4 */
static void MineTrainRCTrackRightHalfBankedHelixDownLarge(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackSequence >= 7)
    {
        trackSequence -= 7;
        direction = (direction + 1) & 3;
    }
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    MineTrainRCTrackLeftHalfBankedHelixUpLarge(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C324 */
static void MineTrainRCTrackBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<TrackElemType::Brakes>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0071C334 */
static void MineTrainRCTrackOnRidePhoto(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteOnRidePhoto(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<TrackElemType::Flat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
}

/** rct2: 0x0071C354 */
static void MineTrainRCTrackLeftEighthToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
            }
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
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner3, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x0071C364 */
static void MineTrainRCTrackRightEighthToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
            }
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
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x0071C374 */
static void MineTrainRCTrackLeftEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    MineTrainRCTrackRightEighthToDiag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C384 */
static void MineTrainRCTrackRightEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    MineTrainRCTrackLeftEighthToDiag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C394 */
static void MineTrainRCTrackLeftEighthBankToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
            }
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
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner3, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x0071C3A4 */
static void MineTrainRCTrackRightEighthBankToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
            }
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
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::NwSe, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x0071C3B4 */
static void MineTrainRCTrackLeftEighthBankToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    MineTrainRCTrackRightEighthBankToDiag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C3C4 */
static void MineTrainRCTrackRightEighthBankToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    MineTrainRCTrackLeftEighthBankToDiag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
}

/** rct2: 0x0071C344 */
static void MineTrainRCTrackDiagFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<TrackElemType::DiagFlat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void MineTrainRCTrackDiagBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<TrackElemType::DiagBrakes>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void MineTrainRCTrackDiagBlockBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteBrake(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<TrackElemType::DiagBlockBrakes>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0071C414 */
static void MineTrainRCTrackDiag25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<TrackElemType::DiagUp25>(
        session, supportType.wooden, trackSequence, direction, height + 16, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x0071C474 */
static void MineTrainRCTrackDiag60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<TrackElemType::DiagUp60>(
        session, supportType.wooden, trackSequence, direction, height + 16, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104);
}

/** rct2: 0x0071C3F4 */
static void MineTrainRCTrackDiagFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<TrackElemType::DiagFlatToUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x0071C454 */
static void MineTrainRCTrackDiag25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<TrackElemType::DiagUp25ToUp60>(
        session, supportType.wooden, trackSequence, direction, height + 16, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

/** rct2: 0x0071C464 */
static void MineTrainRCTrackDiag60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<TrackElemType::DiagUp60ToUp25>(
        session, supportType.wooden, trackSequence, direction, height + 16, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

/** rct2: 0x0071C404 */
static void MineTrainRCTrackDiag25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<TrackElemType::DiagUp25ToFlat>(
        session, supportType.wooden, trackSequence, direction, height + 16, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x0071C444 */
static void MineTrainRCTrackDiag25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrackDiag25DegUp(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x0071C4A4 */
static void MineTrainRCTrackDiag60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrackDiag60DegUp(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x0071C424 */
static void MineTrainRCTrackDiagFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrackDiag25DegUpToFlat(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x0071C484 */
static void MineTrainRCTrackDiag25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrackDiag60DegUpTo25DegUp(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x0071C494 */
static void MineTrainRCTrackDiag60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrackDiag25DegUpTo60DegUp(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x0071C434 */
static void MineTrainRCTrackDiag25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrackDiagFlatTo25DegUp(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x0071C4D4 */
static void MineTrainRCTrackDiagFlatToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<TrackElemType::DiagFlatToLeftBank>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0071C4B4 */
static void MineTrainRCTrackDiagFlatToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<TrackElemType::DiagFlatToRightBank>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0071C4C4 */
static void MineTrainRCTrackDiagLeftBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrackDiagFlatToRightBank(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x0071C4E4 */
static void MineTrainRCTrackDiagRightBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrackDiagFlatToLeftBank(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x0071C514 */
static void MineTrainRCTrackDiagLeftBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<TrackElemType::DiagLeftBankToUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x0071C524 */
static void MineTrainRCTrackDiagRightBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<TrackElemType::DiagRightBankToUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x0071C4F4 */
static void MineTrainRCTrackDiag25DegUpToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceB<TrackElemType::DiagUp25ToLeftBank>(
        session, supportType.wooden, trackSequence, direction, height + 16, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x0071C504 */
static void MineTrainRCTrackDiag25DegUpToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceB<TrackElemType::DiagUp25ToRightBank>(
        session, supportType.wooden, trackSequence, direction, height + 16, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x0071C534 */
static void MineTrainRCTrackDiagLeftBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrackDiag25DegUpToRightBank(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x0071C544 */
static void MineTrainRCTrackDiagRightBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrackDiag25DegUpToLeftBank(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x0071C554 */
static void MineTrainRCTrackDiag25DegDownToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrackDiagRightBankTo25DegUp(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x0071C564 */
static void MineTrainRCTrackDiag25DegDownToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrackDiagLeftBankTo25DegUp(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x0071C3D4 */
static void MineTrainRCTrackDiagLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<TrackElemType::DiagLeftBank>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0071C3E4 */
static void MineTrainRCTrackDiagRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrackDiagLeftBank(
        session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

/** rct2: 0x0071C574 */
static void MineTrainRCTrackBlockBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteBrake(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<TrackElemType::BlockBrakes>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void MineTrainRCTrackFlatTo60DegUpLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::FlatToUp60DegLongBaseSeq0);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::FlatToUp60DegLongBaseSeq1);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::FlatToUp60DegLongBaseSeq2);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 3:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::FlatToUp60DegLongBaseSeq3);
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
    }
}
static void MineTrainRCTrack60DegUpToFlatLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    switch (trackSequence)
    {
        case 0:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::Up60DegToFlatLongBaseSeq0);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 1:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::Up60DegToFlatLongBaseSeq1);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 2:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::Up60DegToFlatLongBaseSeq2);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::Up60DegToFlatLongBaseSeq3);
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 40);
            break;
    }
}

static void MineTrainRCTrackFlatTo60DegDownLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrack60DegUpToFlatLongBase(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void MineTrainRCTrack60DegDownToFlatLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MineTrainRCTrackFlatTo60DegUpLongBase(
        session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

TrackPaintFunction GetTrackPaintFunctionMineTrainRC(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return MineTrainRCTrackFlat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return MineTrainRCTrackStation;
        case TrackElemType::Up25:
            return MineTrainRCTrack25DegUp;
        case TrackElemType::Up60:
            return MineTrainRCTrack60DegUp;
        case TrackElemType::FlatToUp25:
            return MineTrainRCTrackFlatTo25DegUp;
        case TrackElemType::Up25ToUp60:
            return MineTrainRCTrack25DegUpTo60DegUp;
        case TrackElemType::Up60ToUp25:
            return MineTrainRCTrack60DegUpTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return MineTrainRCTrack25DegUpToFlat;
        case TrackElemType::Down25:
            return MineTrainRCTrack25DegDown;
        case TrackElemType::Down60:
            return MineTrainRCTrack60DegDown;
        case TrackElemType::FlatToDown25:
            return MineTrainRCTrackFlatTo25DegDown;
        case TrackElemType::Down25ToDown60:
            return MineTrainRCTrack25DegDownTo60DegDown;
        case TrackElemType::Down60ToDown25:
            return MineTrainRCTrack60DegDownTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return MineTrainRCTrack25DegDownToFlat;
        case TrackElemType::LeftQuarterTurn5Tiles:
            return MineTrainRCTrackLeftQuarterTurn5;
        case TrackElemType::RightQuarterTurn5Tiles:
            return MineTrainRCTrackRightQuarterTurn5;
        case TrackElemType::FlatToLeftBank:
            return MineTrainRCTrackFlatToLeftBank;
        case TrackElemType::FlatToRightBank:
            return MineTrainRCTrackFlatToRightBank;
        case TrackElemType::LeftBankToFlat:
            return MineTrainRCTrackLeftBankToFlat;
        case TrackElemType::RightBankToFlat:
            return MineTrainRCTrackRightBankToFlat;
        case TrackElemType::BankedLeftQuarterTurn5Tiles:
            return MineTrainRCTrackBankedLeftQuarterTurn5;
        case TrackElemType::BankedRightQuarterTurn5Tiles:
            return MineTrainRCTrackBankedRightQuarterTurn5;
        case TrackElemType::LeftBankToUp25:
            return MineTrainRCTrackLeftBankTo25DegUp;
        case TrackElemType::RightBankToUp25:
            return MineTrainRCTrackRightBankTo25DegUp;
        case TrackElemType::Up25ToLeftBank:
            return MineTrainRCTrack25DegUpToLeftBank;
        case TrackElemType::Up25ToRightBank:
            return MineTrainRCTrack25DegUpToRightBank;
        case TrackElemType::LeftBankToDown25:
            return MineTrainRCTrackLeftBankTo25DegDown;
        case TrackElemType::RightBankToDown25:
            return MineTrainRCTrackRightBankTo25DegDown;
        case TrackElemType::Down25ToLeftBank:
            return MineTrainRCTrack25DegDownToLeftBank;
        case TrackElemType::Down25ToRightBank:
            return MineTrainRCTrack25DegDownToRightBank;
        case TrackElemType::LeftBank:
            return MineTrainRCTrackLeftBank;
        case TrackElemType::RightBank:
            return MineTrainRCTrackRightBank;
        case TrackElemType::LeftQuarterTurn5TilesUp25:
            return MineTrainRCTrackLeftQuarterTurn525DegUp;
        case TrackElemType::RightQuarterTurn5TilesUp25:
            return MineTrainRCTrackRightQuarterTurn525DegUp;
        case TrackElemType::LeftQuarterTurn5TilesDown25:
            return MineTrainRCTrackLeftQuarterTurn525DegDown;
        case TrackElemType::RightQuarterTurn5TilesDown25:
            return MineTrainRCTrackRightQuarterTurn525DegDown;
        case TrackElemType::SBendLeft:
            return MineTrainRCTrackSBendLeft;
        case TrackElemType::SBendRight:
            return MineTrainRCTrackSBendRight;
        case TrackElemType::LeftQuarterTurn3Tiles:
            return MineTrainRCTrackLeftQuarterTurn3;
        case TrackElemType::RightQuarterTurn3Tiles:
            return MineTrainRCTrackRightQuarterTurn3;
        case TrackElemType::LeftBankedQuarterTurn3Tiles:
            return MineTrainRCTrackLeftQuarterTurn3Bank;
        case TrackElemType::RightBankedQuarterTurn3Tiles:
            return MineTrainRCTrackRightQuarterTurn3Bank;
        case TrackElemType::LeftQuarterTurn3TilesUp25:
            return MineTrainRCTrackLeftQuarterTurn325DegUp;
        case TrackElemType::RightQuarterTurn3TilesUp25:
            return MineTrainRCTrackRightQuarterTurn325DegUp;
        case TrackElemType::LeftQuarterTurn3TilesDown25:
            return MineTrainRCTrackLeftQuarterTurn325DegDown;
        case TrackElemType::RightQuarterTurn3TilesDown25:
            return MineTrainRCTrackRightQuarterTurn325DegDown;
        case TrackElemType::LeftHalfBankedHelixUpSmall:
            return MineTrainRCTrackLeftHalfBankedHelixUpSmall;
        case TrackElemType::RightHalfBankedHelixUpSmall:
            return MineTrainRCTrackRightHalfBankedHelixUpSmall;
        case TrackElemType::LeftHalfBankedHelixDownSmall:
            return MineTrainRCTrackLeftHalfBankedHelixDownSmall;
        case TrackElemType::RightHalfBankedHelixDownSmall:
            return MineTrainRCTrackRightHalfBankedHelixDownSmall;
        case TrackElemType::LeftHalfBankedHelixUpLarge:
            return MineTrainRCTrackLeftHalfBankedHelixUpLarge;
        case TrackElemType::RightHalfBankedHelixUpLarge:
            return MineTrainRCTrackRightHalfBankedHelixUpLarge;
        case TrackElemType::LeftHalfBankedHelixDownLarge:
            return MineTrainRCTrackLeftHalfBankedHelixDownLarge;
        case TrackElemType::RightHalfBankedHelixDownLarge:
            return MineTrainRCTrackRightHalfBankedHelixDownLarge;
        case TrackElemType::Brakes:
            return MineTrainRCTrackBrakes;
        case TrackElemType::OnRidePhoto:
            return MineTrainRCTrackOnRidePhoto;
        case TrackElemType::LeftEighthToDiag:
            return MineTrainRCTrackLeftEighthToDiag;
        case TrackElemType::RightEighthToDiag:
            return MineTrainRCTrackRightEighthToDiag;
        case TrackElemType::LeftEighthToOrthogonal:
            return MineTrainRCTrackLeftEighthToOrthogonal;
        case TrackElemType::RightEighthToOrthogonal:
            return MineTrainRCTrackRightEighthToOrthogonal;
        case TrackElemType::LeftEighthBankToDiag:
            return MineTrainRCTrackLeftEighthBankToDiag;
        case TrackElemType::RightEighthBankToDiag:
            return MineTrainRCTrackRightEighthBankToDiag;
        case TrackElemType::LeftEighthBankToOrthogonal:
            return MineTrainRCTrackLeftEighthBankToOrthogonal;
        case TrackElemType::RightEighthBankToOrthogonal:
            return MineTrainRCTrackRightEighthBankToOrthogonal;
        case TrackElemType::DiagFlat:
            return MineTrainRCTrackDiagFlat;
        case TrackElemType::DiagUp25:
            return MineTrainRCTrackDiag25DegUp;
        case TrackElemType::DiagUp60:
            return MineTrainRCTrackDiag60DegUp;
        case TrackElemType::DiagFlatToUp25:
            return MineTrainRCTrackDiagFlatTo25DegUp;
        case TrackElemType::DiagUp25ToUp60:
            return MineTrainRCTrackDiag25DegUpTo60DegUp;
        case TrackElemType::DiagUp60ToUp25:
            return MineTrainRCTrackDiag60DegUpTo25DegUp;
        case TrackElemType::DiagUp25ToFlat:
            return MineTrainRCTrackDiag25DegUpToFlat;
        case TrackElemType::DiagDown25:
            return MineTrainRCTrackDiag25DegDown;
        case TrackElemType::DiagDown60:
            return MineTrainRCTrackDiag60DegDown;
        case TrackElemType::DiagFlatToDown25:
            return MineTrainRCTrackDiagFlatTo25DegDown;
        case TrackElemType::DiagDown25ToDown60:
            return MineTrainRCTrackDiag25DegDownTo60DegDown;
        case TrackElemType::DiagDown60ToDown25:
            return MineTrainRCTrackDiag60DegDownTo25DegDown;
        case TrackElemType::DiagDown25ToFlat:
            return MineTrainRCTrackDiag25DegDownToFlat;
        case TrackElemType::DiagFlatToLeftBank:
            return MineTrainRCTrackDiagFlatToLeftBank;
        case TrackElemType::DiagFlatToRightBank:
            return MineTrainRCTrackDiagFlatToRightBank;
        case TrackElemType::DiagLeftBankToFlat:
            return MineTrainRCTrackDiagLeftBankToFlat;
        case TrackElemType::DiagRightBankToFlat:
            return MineTrainRCTrackDiagRightBankToFlat;
        case TrackElemType::DiagLeftBankToUp25:
            return MineTrainRCTrackDiagLeftBankTo25DegUp;
        case TrackElemType::DiagRightBankToUp25:
            return MineTrainRCTrackDiagRightBankTo25DegUp;
        case TrackElemType::DiagUp25ToLeftBank:
            return MineTrainRCTrackDiag25DegUpToLeftBank;
        case TrackElemType::DiagUp25ToRightBank:
            return MineTrainRCTrackDiag25DegUpToRightBank;
        case TrackElemType::DiagLeftBankToDown25:
            return MineTrainRCTrackDiagLeftBankTo25DegDown;
        case TrackElemType::DiagRightBankToDown25:
            return MineTrainRCTrackDiagRightBankTo25DegDown;
        case TrackElemType::DiagDown25ToLeftBank:
            return MineTrainRCTrackDiag25DegDownToLeftBank;
        case TrackElemType::DiagDown25ToRightBank:
            return MineTrainRCTrackDiag25DegDownToRightBank;
        case TrackElemType::DiagLeftBank:
            return MineTrainRCTrackDiagLeftBank;
        case TrackElemType::DiagRightBank:
            return MineTrainRCTrackDiagRightBank;
        case TrackElemType::BlockBrakes:
            return MineTrainRCTrackBlockBrakes;
        case TrackElemType::DiagBrakes:
            return MineTrainRCTrackDiagBrakes;
        case TrackElemType::DiagBlockBrakes:
            return MineTrainRCTrackDiagBlockBrakes;
        case TrackElemType::FlatToUp60LongBase:
            return MineTrainRCTrackFlatTo60DegUpLongBase;
        case TrackElemType::Up60ToFlatLongBase:
            return MineTrainRCTrack60DegUpToFlatLongBase;
        case TrackElemType::FlatToDown60LongBase:
            return MineTrainRCTrackFlatTo60DegDownLongBase;
        case TrackElemType::Down60ToFlatLongBase:
            return MineTrainRCTrack60DegDownToFlatLongBase;
        default:
            return TrackPaintFunctionDummy;
    }
}
