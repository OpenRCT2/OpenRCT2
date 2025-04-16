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
#include "../../../world/tile_element/TrackElement.h"
#include "../../Paint.h"
#include "../../support/WoodenSupports.h"
#include "../../support/WoodenSupports.hpp"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

#include <cassert>

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Square;

/** rct2: 0x00811264 */
static void PaintVirginiaReelTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);
    if (direction & 1)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    DrawSupportForSequenceA<TrackElemType::Flat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x00811274 */
static void PaintVirginiaReelTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<TrackElemType::Up25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
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

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x00811294 */
static void PaintVirginiaReelTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<TrackElemType::FlatToUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x00811294 */
static void PaintVirginiaReelTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteChain(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<TrackElemType::Up25ToFlat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
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

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

/** rct2: 0x008112A4 */
static void PaintVirginiaReelTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintVirginiaReelTrack25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x008112B4 */
static void PaintVirginiaReelTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintVirginiaReelTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x008112C4 */
static void PaintVirginiaReelTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintVirginiaReelTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x008112D4, 0x008112E4, 0x008112F4 */
static void PaintVirginiaReelStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (direction == 0 || direction == 2)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else if (direction == 1 || direction == 3)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    DrawSupportForSequenceA<TrackElemType::EndStation>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    TrackPaintUtilDrawStation(session, ride, direction, height, trackElement, StationBaseType::b, -2);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x00811304 */
static void PaintVirginiaReelTrackLeftQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    TrackPaintUtilLeftQuarterTurn3TilesTunnel(session, kTunnelGroup, TunnelSubType::Flat, height, direction, trackSequence);

    switch (trackSequence)
    {
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::left),
                    direction),
                0xFFFF, 0);
            break;
        case 0:
        case 3:
            DrawSupportForSequenceA<TrackElemType::LeftQuarterTurn3Tiles>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static constexpr uint8_t virginia_reel_right_quarter_turn_3_tiles_to_left_turn_map[] = {
    3,
    1,
    2,
    0,
};

/** rct2: 0x00811314 */
static void PaintVirginiaReelTrackRightQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = virginia_reel_right_quarter_turn_3_tiles_to_left_turn_map[trackSequence];
    PaintVirginiaReelTrackLeftQuarterTurn3Tiles(
        session, ride, trackSequence, (direction + 3) % 4, height, trackElement, supportType);
}

/** rct2: 0x00811324 */
static void PaintVirginiaReelTrackLeftQuarterTurn1Tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<TrackElemType::LeftQuarterTurn1Tile>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;
        case 2:
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x00811334 */
static void PaintVirginiaReelTrackRightQuarterTurn1Tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintVirginiaReelTrackLeftQuarterTurn1Tile(
        session, ride, trackSequence, (direction + 3) % 4, height, trackElement, supportType);
}

/**
 * rct2: 0x00811184
 */
TrackPaintFunction GetTrackPaintFunctionVirginiaReel(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return PaintVirginiaReelTrackFlat;

        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return PaintVirginiaReelStation;

        case TrackElemType::Up25:
            return PaintVirginiaReelTrack25DegUp;
        case TrackElemType::FlatToUp25:
            return PaintVirginiaReelTrackFlatTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return PaintVirginiaReelTrack25DegUpToFlat;

        case TrackElemType::Down25:
            return PaintVirginiaReelTrack25DegDown;
        case TrackElemType::FlatToDown25:
            return PaintVirginiaReelTrackFlatTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return PaintVirginiaReelTrack25DegDownToFlat;

        case TrackElemType::LeftQuarterTurn3Tiles:
            return PaintVirginiaReelTrackLeftQuarterTurn3Tiles;
        case TrackElemType::RightQuarterTurn3Tiles:
            return PaintVirginiaReelTrackRightQuarterTurn3Tiles;

        case TrackElemType::LeftQuarterTurn1Tile:
            return PaintVirginiaReelTrackLeftQuarterTurn1Tile;
        case TrackElemType::RightQuarterTurn1Tile:
            return PaintVirginiaReelTrackRightQuarterTurn1Tile;

        default:
            return TrackPaintFunctionDummy;
    }
}
