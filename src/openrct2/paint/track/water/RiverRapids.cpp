/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../Game.h"
#include "../../../GameState.h"
#include "../../../config/Config.h"
#include "../../../interface/Viewport.h"
#include "../../../ride/Track.h"
#include "../../../ride/TrackPaint.h"
#include "../../../ride/Vehicle.h"
#include "../../../world/Map.h"
#include "../../Paint.h"
#include "../../support/WoodenSupports.h"
#include "../../support/WoodenSupports.hpp"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Square;

/** rct2: 0x00757650 */
static void PaintRiverRapidsTrackFlat(
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

/** rct2: 0x007576C0 */
static void PaintRiverRapidsStation(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    TrackPaintUtilDrawNarrowStationPlatform(session, ride, direction, height, 12, trackElement, StationBaseType::none, 0);
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

/** rct2: 0x00757660 */
static void PaintRiverRapidsTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<TrackElemType::Up25>(session, supportType.wooden, 0, direction, height, session.SupportColours);

    bool isStart = direction == 0 || direction == 3;
    int8_t tunnelHeightOffset = isStart ? -8 : 8;
    auto tunnelSubType = isStart ? TunnelSubType::SlopeStart : TunnelSubType::SlopeEnd;
    PaintUtilPushTunnelRotated(session, direction, height + tunnelHeightOffset, kTunnelGroup, tunnelSubType);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x00757670 */
static void PaintRiverRapidsTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<TrackElemType::FlatToUp25>(
        session, supportType.wooden, 0, direction, height, session.SupportColours);

    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x00757680 */
static void PaintRiverRapidsTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<TrackElemType::Up25ToFlat>(
        session, supportType.wooden, 0, direction, height, session.SupportColours);

    bool isStart = direction == 0 || direction == 3;
    int8_t heightOffset = isStart ? -8 : 8;
    auto tunnelSubType = isStart ? TunnelSubType::Flat : TunnelSubType::FlatTo25Deg;
    PaintUtilPushTunnelRotated(session, direction, height + heightOffset, kTunnelGroup, tunnelSubType);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

/** rct2: 0x00757690 */
static void PaintRiverRapidsTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    direction = DirectionReverse(direction);

    DrawSupportForSequenceA<TrackElemType::Up25>(session, supportType.wooden, 0, direction, height, session.SupportColours);

    bool isStart = direction == 0 || direction == 3;
    int8_t tunnelHeightOffset = isStart ? -8 : 8;
    auto tunnelSubType = isStart ? TunnelSubType::SlopeStart : TunnelSubType::SlopeEnd;
    PaintUtilPushTunnelRotated(session, direction, height + tunnelHeightOffset, kTunnelGroup, tunnelSubType);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x007576A0 */
static void PaintRiverRapidsTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    direction = DirectionReverse(direction);

    DrawSupportForSequenceA<TrackElemType::Up25ToFlat>(
        session, supportType.wooden, 0, direction, height, session.SupportColours);

    bool isStart = direction == 0 || direction == 3;
    int8_t heightOffset = isStart ? -8 : 8;
    auto tunnelSubType = isStart ? TunnelSubType::Flat : TunnelSubType::FlatTo25Deg;
    PaintUtilPushTunnelRotated(session, direction, height + heightOffset, kTunnelGroup, tunnelSubType);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

/** rct2: 0x007576B0 */
static void PaintRiverRapidsTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    direction = DirectionReverse(direction);
    DrawSupportForSequenceA<TrackElemType::FlatToUp25>(
        session, supportType.wooden, 0, direction, height, session.SupportColours);

    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x007576F0 */
static void PaintRiverRapidsTrackLeftQuarterTurn1Tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites3(session, ride, trackSequence, direction, height, trackElement, supportType);
    WoodenASupportsPaintSetupRotated(
        session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x00757700 */
static void PaintRiverRapidsTrackRightQuarterTurn1Tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites3(session, ride, trackSequence, direction, height, trackElement, supportType);
    WoodenASupportsPaintSetupRotated(
        session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x00757710 */
static void PaintRiverRapidsTrackWaterfall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintWaterfall(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<TrackElemType::Waterfall>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    if (direction & 1)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Tall);
    }
    else
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Tall);
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x00757720 */
static void PaintRiverRapidsTrackRapids(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintRapidsSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<TrackElemType::Rapids>(
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

/** rct2: 0x00757740 */
static void PaintRiverRapidsTrackOnRidePhoto(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    TrackPaintUtilOnridePhotoPaint(session, direction, height + 13, trackElement);

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
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x */
static void PaintRiverRapidsTrackWhirlpool(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintWhirlpoolSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<TrackElemType::Whirlpool>(
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

/**
 * rct2: 0x0075745C
 **/
TrackPaintFunction GetTrackPaintFunctionRiverRapids(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return PaintRiverRapidsTrackFlat;

        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return PaintRiverRapidsStation;

        case TrackElemType::Up25:
            return PaintRiverRapidsTrack25DegUp;
        case TrackElemType::FlatToUp25:
            return PaintRiverRapidsTrackFlatTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return PaintRiverRapidsTrack25DegUpToFlat;

        case TrackElemType::Down25:
            return PaintRiverRapidsTrack25DegDown;
        case TrackElemType::FlatToDown25:
            return PaintRiverRapidsTrackFlatTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return PaintRiverRapidsTrack25DegDownToFlat;

        case TrackElemType::LeftQuarterTurn1Tile:
            return PaintRiverRapidsTrackLeftQuarterTurn1Tile;
        case TrackElemType::RightQuarterTurn1Tile:
            return PaintRiverRapidsTrackRightQuarterTurn1Tile;

        case TrackElemType::Waterfall:
            return PaintRiverRapidsTrackWaterfall;

        case TrackElemType::Rapids:
            return PaintRiverRapidsTrackRapids;

        case TrackElemType::OnRidePhoto:
            return PaintRiverRapidsTrackOnRidePhoto;

        case TrackElemType::Whirlpool:
            return PaintRiverRapidsTrackWhirlpool;
        default:
            return TrackPaintFunctionDummy;
    }
}
