/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../SpriteIds.h"
#include "../../../interface/Viewport.h"
#include "../../../ride/Ride.h"
#include "../../../ride/Track.h"
#include "../../../ride/TrackPaint.h"
#include "../../../world/Map.h"
#include "../../../world/tile_element/TrackElement.h"
#include "../../Paint.h"
#include "../../support/WoodenSupports.h"
#include "../../support/WoodenSupports.hpp"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Square;

/** rct2: 0x0074A748 */
static void PaintGoKartsTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (direction == 0 || direction == 2)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    DrawSupportForSequenceA<TrackElemType::Flat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0074A758 */
static void PaintGoKartsTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

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

/** rct2: 0x0074A768 */
static void PaintGoKartsTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<TrackElemType::FlatToUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

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

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x */
static void PaintGoKartsTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

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

/** rct2: 0x0074A788 */
static void PaintGoKartsTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintGoKartsTrack25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x0074A798 */
static void PaintGoKartsTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintGoKartsTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x0074A7A8 */
static void PaintGoKartsTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintGoKartsTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x */
static void PaintGoKartsStation(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.GetTrackType() == TrackElemType::EndStation)
    {
        trackPaintSprites4GreenLight(session, ride, trackSequence, direction, height, trackElement, supportType);
    }
    else
    {
        trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);
    }

    const auto* stationObj = ride.getStationObject();

    auto stationColour = GetStationColourScheme(session, trackElement);
    if (direction == 0 || direction == 2)
    {
        const bool hasFence = TrackPaintUtilHasFence(EDGE_NW, session.MapPosition, trackElement, ride, session.CurrentRotation);
        TrackPaintUtilDrawStationCovers(session, EDGE_NW, hasFence, stationObj, height, stationColour);
    }
    else
    {
        const bool hasFence = TrackPaintUtilHasFence(EDGE_NE, session.MapPosition, trackElement, ride, session.CurrentRotation);
        TrackPaintUtilDrawStationCovers(session, EDGE_NE, hasFence, stationObj, height, stationColour);
    }

    if (direction == 0 || direction == 2)
    {
        const bool hasFence = TrackPaintUtilHasFence(EDGE_SE, session.MapPosition, trackElement, ride, session.CurrentRotation);
        TrackPaintUtilDrawStationCovers(session, EDGE_SE, hasFence, stationObj, height, stationColour);
    }
    else
    {
        const bool hasFence = TrackPaintUtilHasFence(EDGE_SW, session.MapPosition, trackElement, ride, session.CurrentRotation);
        TrackPaintUtilDrawStationCovers(session, EDGE_SW, hasFence, stationObj, height, stationColour);
    }

    if (direction == 0 || direction == 2)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    DrawSupportForSequenceA<TrackElemType::EndStation>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0074A7E8 */
static void PaintGoKartsTrackLeftQuarterTurn1Tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites3(session, ride, trackSequence, direction, height, trackElement, supportType);

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;
        case 2:
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;
        case 3:
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;
    }

    WoodenASupportsPaintSetupRotated(
        session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0074A7F8 */
static void PaintGoKartsTrackRightQuarterTurn1Tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintGoKartsTrackLeftQuarterTurn1Tile(session, ride, trackSequence, (direction + 3) % 4, height, trackElement, supportType);
}

static void TrackUp60(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    WoodenASupportsPaintSetupRotated(
        session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
        WoodenSupportTransitionType::Up60Deg);
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

static void TrackUp25ToUp60(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    WoodenASupportsPaintSetupRotated(
        session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
        WoodenSupportTransitionType::Up25DegToUp60Deg);
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

static void TrackUp60ToUp25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    WoodenASupportsPaintSetupRotated(
        session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
        WoodenSupportTransitionType::Up60DegToUp25Deg);
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

static void TrackDown60(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    TrackUp60(session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void TrackDown25ToDown60(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    TrackUp60ToUp25(session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void TrackDown60ToDown25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    TrackUp25ToUp60(session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void TrackFlatToUp60LongBase(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const uint8_t direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<TrackElemType::FlatToUp60LongBase>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    if (trackSequence == 0 && (direction == 0 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else if (trackSequence == 3 && (direction == 1 || direction == 2))
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    static constexpr std::array generalSupportHeights = { 48, 48, 64, 80 };
    PaintUtilSetGeneralSupportHeight(session, height + generalSupportHeights[trackSequence]);
}

static void TrackUp60ToFlatLongBase(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const uint8_t direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<TrackElemType::Up60ToFlatLongBase>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    if (trackSequence == 0 && (direction == 0 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else if (trackSequence == 3 && (direction == 1 || direction == 2))
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::Flat);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    static constexpr std::array generalSupportHeights = { 80, 64, 48, 48 };
    PaintUtilSetGeneralSupportHeight(session, height + generalSupportHeights[trackSequence]);
}

static void TrackFlatToDown60LongBase(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    TrackUp60ToFlatLongBase(session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void TrackDown60ToFlatLongBase(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    TrackFlatToUp60LongBase(session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void TrackLeftQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    static constexpr std::array<WoodenSupportSubType, 4> woodenSupportSubTypes = {
        WoodenSupportSubType::NeSw,
        WoodenSupportSubType::Null,
        WoodenSupportSubType::Corner3,
        WoodenSupportSubType::NwSe,
    };
    const WoodenSupportSubType woodenSupportSubType = woodenSupportSubTypes[trackSequence];
    if (woodenSupportSubType != WoodenSupportSubType::Null)
    {
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, woodenSupportSubType, direction, height, session.SupportColours,
            WoodenSupportTransitionType::None);
    }
    TrackPaintUtilLeftQuarterTurn3TilesTunnel(session, kTunnelGroup, TunnelSubType::Flat, height, direction, trackSequence);
    static constexpr std::array<int32_t, 4> blockedSegments = {
        kSegmentsAll,
        kSegmentsAll,
        EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
        kSegmentsAll,
    };
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments[trackSequence], direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void TrackRightQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    TrackLeftQuarterTurn3Tiles(
        session, ride, kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence], DirectionPrev(direction), height,
        trackElement, supportType);
}

static void TrackLeftQuarterTurn5Tiles(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const uint8_t direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    trackPaintSprites3(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<OpenRCT2::TrackElemType::LeftQuarterTurn5Tiles>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    if (trackSequence == 0 && (direction == 0 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else if (trackSequence == 6 && (direction == 2 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, DirectionPrev(direction), height, kTunnelGroup, TunnelSubType::Flat);
    }
    static constexpr std::array<int32_t, 7> blockedSegments = {
        EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft),
        kSegmentsAll,
        EnumsToFlags(
            PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
            PaintSegment::bottomLeft),
        EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
        kSegmentsAll,
        EnumsToFlags(
            PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft,
            PaintSegment::bottomRight),
        EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight),
    };
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments[trackSequence], direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void TrackRightQuarterTurn5Tiles(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    TrackLeftQuarterTurn5Tiles(
        session, ride, kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence], DirectionPrev(direction), height,
        trackElement, supportType);
}

static void TrackLeftEighthToDiag(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    trackPaintSprites3(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<OpenRCT2::TrackElemType::LeftEighthToDiag>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    if (trackSequence == 0 && (direction == 0 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void TrackRightEighthToDiag(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    trackPaintSprites3(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<OpenRCT2::TrackElemType::RightEighthToDiag>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    if (trackSequence == 0 && (direction == 0 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void TrackLeftEighthToOrthogonal(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    TrackRightEighthToDiag(
        session, ride, mapLeftEighthTurnToOrthogonal[trackSequence], DirectionReverse(direction), height, trackElement,
        supportType);
}

static void TrackRightEighthToOrthogonal(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    TrackLeftEighthToDiag(
        session, ride, mapLeftEighthTurnToOrthogonal[trackSequence], DirectionPrev(direction), height, trackElement,
        supportType);
}

static void TrackDiagFlat(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<OpenRCT2::TrackElemType::DiagFlat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void TrackDiagFlatToUp25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<OpenRCT2::TrackElemType::DiagFlatToUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

static void TrackDiagUp25ToFlat(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceB<OpenRCT2::TrackElemType::DiagUp25ToFlat>(
        session, supportType.wooden, trackSequence, direction, height + 16, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

static void TrackDiagUp25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceB<OpenRCT2::TrackElemType::DiagUp25>(
        session, supportType.wooden, trackSequence, direction, height + 16, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

static void TrackDiagFlatToDown25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    return TrackDiagUp25ToFlat(
        session, ride, kMapReversedDiagonalStraight[trackSequence], DirectionReverse(direction), height, trackElement,
        supportType);
}

static void TrackDiagDown25ToFlat(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    return TrackDiagFlatToUp25(
        session, ride, kMapReversedDiagonalStraight[trackSequence], DirectionReverse(direction), height, trackElement,
        supportType);
}

static void TrackDiagDown25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    return TrackDiagUp25(
        session, ride, kMapReversedDiagonalStraight[trackSequence], DirectionReverse(direction), height, trackElement,
        supportType);
}

static void TrackDiagUp25ToUp60(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceB<OpenRCT2::TrackElemType::DiagUp25ToUp60>(
        session, supportType.wooden, trackSequence, direction, height + 16, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

static void TrackDiagUp60ToUp25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceB<OpenRCT2::TrackElemType::DiagUp60ToUp25>(
        session, supportType.wooden, trackSequence, direction, height + 16, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

static void TrackDiagUp60(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceB<OpenRCT2::TrackElemType::DiagUp60>(
        session, supportType.wooden, trackSequence, direction, height + 16, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104);
}

static void TrackDiagDown25ToDown60(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    return TrackDiagUp60ToUp25(
        session, ride, kMapReversedDiagonalStraight[trackSequence], DirectionReverse(direction), height, trackElement,
        supportType);
}

static void TrackDiagDown60ToDown25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    return TrackDiagUp25ToUp60(
        session, ride, kMapReversedDiagonalStraight[trackSequence], DirectionReverse(direction), height, trackElement,
        supportType);
}

static void TrackDiagDown60(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    return TrackDiagUp60(
        session, ride, kMapReversedDiagonalStraight[trackSequence], DirectionReverse(direction), height, trackElement,
        supportType);
}

static void TrackLeftQuarterTurn3TilesUp25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    static constexpr std::array<WoodenSupportSubType, 4> woodenSupportSubTypes = {
        WoodenSupportSubType::NeSw,
        WoodenSupportSubType::Null,
        WoodenSupportSubType::Corner3,
        WoodenSupportSubType::NeSw,
    };
    static constexpr std::array<WoodenSupportTransitionType, 4> woodenSupportTransitionTypes = {
        WoodenSupportTransitionType::Up25Deg,
        WoodenSupportTransitionType::None,
        WoodenSupportTransitionType::None,
        WoodenSupportTransitionType::Up25Deg,
    };
    static constexpr std::array woodenSupportExtraRotation = {
        0,
        0,
        0,
        -1,
    };
    const WoodenSupportSubType woodenSupportSubType = woodenSupportSubTypes[trackSequence];
    if (woodenSupportSubType != WoodenSupportSubType::Null)
    {
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, woodenSupportSubType, (direction + woodenSupportExtraRotation[trackSequence]) & 3,
            height, session.SupportColours, woodenSupportTransitionTypes[trackSequence]);
    }

    if (trackSequence == 0 && (direction == 0 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else if (trackSequence == 3 && (direction == 2 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, DirectionPrev(direction), height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }

    static constexpr std::array<int32_t, 4> blockedSegments = {
        kSegmentsAll,
        kSegmentsAll,
        EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
        kSegmentsAll,
    };
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments[trackSequence], direction), 0xFFFF, 0);

    static constexpr std::array generalSupportHeights = { 72, 56, 56, 72 };
    PaintUtilSetGeneralSupportHeight(session, height + generalSupportHeights[trackSequence]);
}

static void TrackRightQuarterTurn3TilesUp25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    static constexpr std::array<WoodenSupportSubType, 4> woodenSupportSubTypes = {
        WoodenSupportSubType::NeSw,
        WoodenSupportSubType::Null,
        WoodenSupportSubType::Corner2,
        WoodenSupportSubType::NeSw,
    };
    static constexpr std::array<WoodenSupportTransitionType, 4> woodenSupportTransitionTypes = {
        WoodenSupportTransitionType::Up25Deg,
        WoodenSupportTransitionType::None,
        WoodenSupportTransitionType::None,
        WoodenSupportTransitionType::Up25Deg,
    };
    static constexpr std::array woodenSupportExtraRotation = {
        0,
        0,
        0,
        1,
    };
    const WoodenSupportSubType woodenSupportSubType = woodenSupportSubTypes[trackSequence];
    if (woodenSupportSubType != WoodenSupportSubType::Null)
    {
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, woodenSupportSubType, (direction + woodenSupportExtraRotation[trackSequence]) & 3,
            height, session.SupportColours, woodenSupportTransitionTypes[trackSequence]);
    }

    if (trackSequence == 0 && (direction == 0 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else if (trackSequence == 3 && (direction == 0 || direction == 1))
    {
        PaintUtilPushTunnelRotated(session, DirectionNext(direction), height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }

    static constexpr std::array<int32_t, 4> blockedSegments = {
        kSegmentsAll,
        kSegmentsAll,
        EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
        kSegmentsAll,
    };
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments[trackSequence], direction), 0xFFFF, 0);

    static constexpr std::array generalSupportHeights = { 72, 56, 56, 72 };
    PaintUtilSetGeneralSupportHeight(session, height + generalSupportHeights[trackSequence]);
}

static void TrackLeftQuarterTurn3TilesDown25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    TrackRightQuarterTurn3TilesUp25(
        session, ride, kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence], DirectionNext(direction), height,
        trackElement, supportType);
}

static void TrackRightQuarterTurn3TilesDown25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    TrackLeftQuarterTurn3TilesUp25(
        session, ride, kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence], DirectionPrev(direction), height,
        trackElement, supportType);
}

static void TrackLeftQuarterTurn5TilesUp25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    static constexpr std::array<WoodenSupportSubType, 7> woodenSupportSubTypes = {
        WoodenSupportSubType::NeSw, WoodenSupportSubType::Null,    WoodenSupportSubType::Corner3, WoodenSupportSubType::Corner1,
        WoodenSupportSubType::Null, WoodenSupportSubType::Corner3, WoodenSupportSubType::NeSw,
    };
    static constexpr std::array<WoodenSupportTransitionType, 7> woodenSupportTransitionTypes = {
        WoodenSupportTransitionType::Up25Deg, WoodenSupportTransitionType::None, WoodenSupportTransitionType::None,
        WoodenSupportTransitionType::None,    WoodenSupportTransitionType::None, WoodenSupportTransitionType::None,
        WoodenSupportTransitionType::None,
    };
    static constexpr std::array woodenSupportHeight = { 0, 0, 0, -16, 0, 0, 0 };
    static constexpr std::array woodenSupportExtraRotation = { 0, 0, 0, 0, 0, 0, -1 };
    const WoodenSupportSubType woodenSupportSubType = woodenSupportSubTypes[trackSequence];
    if (woodenSupportSubType != WoodenSupportSubType::Null)
    {
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, woodenSupportSubType, (direction + woodenSupportExtraRotation[trackSequence]) & 3,
            height + woodenSupportHeight[trackSequence], session.SupportColours, woodenSupportTransitionTypes[trackSequence]);
    }

    if (trackSequence == 0 && (direction == 0 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else if (trackSequence == 6 && (direction == 2 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, DirectionPrev(direction), height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }

    static constexpr std::array<int32_t, 7> blockedSegments = {
        kSegmentsAll,
        EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight),
        EnumsToFlags(
            PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
            PaintSegment::bottomLeft),
        EnumsToFlags(
            PaintSegment::top, PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft,
            PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight),
        EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight),
        EnumsToFlags(
            PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft,
            PaintSegment::bottomRight),
        kSegmentsAll,
    };
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments[trackSequence], direction), 0xFFFF, 0);

    static constexpr std::array generalSupportHeights = { 72, 72, 72, 64, 72, 72, 72 };
    PaintUtilSetGeneralSupportHeight(session, height + generalSupportHeights[trackSequence]);
}

static void TrackRightQuarterTurn5TilesUp25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    static constexpr std::array<WoodenSupportSubType, 7> woodenSupportSubTypes = {
        WoodenSupportSubType::NeSw, WoodenSupportSubType::Null,    WoodenSupportSubType::Corner2, WoodenSupportSubType::Corner0,
        WoodenSupportSubType::Null, WoodenSupportSubType::Corner2, WoodenSupportSubType::NeSw,
    };
    static constexpr std::array<WoodenSupportTransitionType, 7> woodenSupportTransitionTypes = {
        WoodenSupportTransitionType::Up25Deg, WoodenSupportTransitionType::None, WoodenSupportTransitionType::None,
        WoodenSupportTransitionType::None,    WoodenSupportTransitionType::None, WoodenSupportTransitionType::None,
        WoodenSupportTransitionType::None,
    };
    static constexpr std::array woodenSupportHeight = { 0, 0, 0, -16, 0, 0, 0 };
    static constexpr std::array woodenSupportExtraRotation = { 0, 0, 0, 0, 0, 0, 1 };
    const WoodenSupportSubType woodenSupportSubType = woodenSupportSubTypes[trackSequence];
    if (woodenSupportSubType != WoodenSupportSubType::Null)
    {
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, woodenSupportSubType, (direction + woodenSupportExtraRotation[trackSequence]) & 3,
            height + woodenSupportHeight[trackSequence], session.SupportColours, woodenSupportTransitionTypes[trackSequence]);
    }

    if (trackSequence == 0 && (direction == 0 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else if (trackSequence == 6 && (direction == 2 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, DirectionNext(direction), height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }

    static constexpr std::array<int32_t, 7> blockedSegments = {
        kSegmentsAll,
        EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight),
        EnumsToFlags(
            PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
            PaintSegment::bottomRight),
        EnumsToFlags(
            PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft,
            PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight),
        EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight),
        EnumsToFlags(
            PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft,
            PaintSegment::bottomRight),
        kSegmentsAll,
    };
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments[trackSequence], direction), 0xFFFF, 0);

    static constexpr std::array generalSupportHeights = { 72, 72, 72, 64, 72, 72, 72 };
    PaintUtilSetGeneralSupportHeight(session, height + generalSupportHeights[trackSequence]);
}

static void TrackLeftQuarterTurn5TilesDown25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    TrackRightQuarterTurn5TilesUp25(
        session, ride, kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence], DirectionNext(direction), height,
        trackElement, supportType);
}

static void TrackRightQuarterTurn5TilesDown25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    TrackLeftQuarterTurn5TilesUp25(
        session, ride, kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence], DirectionPrev(direction), height,
        trackElement, supportType);
}

static void TrackLeftEighthToDiagUp25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    static constexpr std::array<WoodenSupportSubType, 5> woodenSupportSubTypes = {
        WoodenSupportSubType::NeSw,    WoodenSupportSubType::NeSw,    WoodenSupportSubType::Corner1,
        WoodenSupportSubType::Corner3, WoodenSupportSubType::Corner2,
    };
    static constexpr std::array<WoodenSupportTransitionType, 5> woodenSupportTransitionTypes = {
        WoodenSupportTransitionType::Up25Deg, WoodenSupportTransitionType::None, WoodenSupportTransitionType::None,
        WoodenSupportTransitionType::None,    WoodenSupportTransitionType::None,
    };
    static constexpr std::array woodenSupportHeight = { 0, 0, -16, 0, 0 };
    const WoodenSupportSubType woodenSupportSubType = woodenSupportSubTypes[trackSequence];
    if (woodenSupportSubType != WoodenSupportSubType::Null)
    {
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, woodenSupportSubType, direction, height + woodenSupportHeight[trackSequence],
            session.SupportColours, woodenSupportTransitionTypes[trackSequence]);
    }

    if (trackSequence == 0 && (direction == 0 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

static void TrackRightEighthToDiagUp25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    static constexpr std::array<WoodenSupportSubType, 5> woodenSupportSubTypes = {
        WoodenSupportSubType::NeSw,    WoodenSupportSubType::NeSw,    WoodenSupportSubType::Corner0,
        WoodenSupportSubType::Corner2, WoodenSupportSubType::Corner3,
    };
    static constexpr std::array<WoodenSupportTransitionType, 5> woodenSupportTransitionTypes = {
        WoodenSupportTransitionType::Up25Deg, WoodenSupportTransitionType::None, WoodenSupportTransitionType::None,
        WoodenSupportTransitionType::None,    WoodenSupportTransitionType::None,
    };
    static constexpr std::array woodenSupportHeight = { 0, 0, -16, 0, 0 };
    const WoodenSupportSubType woodenSupportSubType = woodenSupportSubTypes[trackSequence];
    if (woodenSupportSubType != WoodenSupportSubType::Null)
    {
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, woodenSupportSubType, direction, height + woodenSupportHeight[trackSequence],
            session.SupportColours, woodenSupportTransitionTypes[trackSequence]);
    }

    if (trackSequence == 0 && (direction == 0 || direction == 3))
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

static void TrackLeftEighthToOrthogonalUp25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    static constexpr std::array<WoodenSupportSubType, 5> woodenSupportSubTypes = {
        WoodenSupportSubType::Null, WoodenSupportSubType::Corner2, WoodenSupportSubType::Corner0,
        WoodenSupportSubType::NeSw, WoodenSupportSubType::NeSw,
    };
    const WoodenSupportSubType woodenSupportSubType = woodenSupportSubTypes[trackSequence];
    if (woodenSupportSubType != WoodenSupportSubType::Null)
    {
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, woodenSupportSubType, direction, height, session.SupportColours,
            WoodenSupportTransitionType::None);
    }

    if (trackSequence == 4 && (direction == 1 || direction == 2))
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

static void TrackRightEighthToOrthogonalUp25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    static constexpr std::array<WoodenSupportSubType, 5> woodenSupportSubTypes = {
        WoodenSupportSubType::Null, WoodenSupportSubType::Corner0, WoodenSupportSubType::Corner2,
        WoodenSupportSubType::NwSe, WoodenSupportSubType::NwSe,
    };
    const WoodenSupportSubType woodenSupportSubType = woodenSupportSubTypes[trackSequence];
    if (woodenSupportSubType != WoodenSupportSubType::Null)
    {
        WoodenASupportsPaintSetupRotated(
            session, supportType.wooden, woodenSupportSubType, direction, height, session.SupportColours,
            WoodenSupportTransitionType::None);
    }

    if (trackSequence == 4 && (direction == 0 || direction == 1))
    {
        PaintUtilPushTunnelRotated(session, DirectionNext(direction), height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

static void TrackLeftEighthToDiagDown25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    static constexpr std::array map = { 4, 3, 1, 2, 0 };
    TrackRightEighthToOrthogonalUp25(
        session, ride, map[trackSequence], DirectionNext(direction), height, trackElement, supportType);
}

static void TrackRightEighthToDiagDown25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    static constexpr std::array map = { 4, 3, 1, 2, 0 };
    TrackLeftEighthToOrthogonalUp25(
        session, ride, map[trackSequence], DirectionReverse(direction), height, trackElement, supportType);
}

static void TrackLeftEighthToOrthogonalDown25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    TrackRightEighthToDiagUp25(
        session, ride, mapLeftEighthTurnToOrthogonal[trackSequence], DirectionReverse(direction), height, trackElement,
        supportType);
}

static void TrackRightEighthToOrthogonalDown25(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    TrackLeftEighthToDiagUp25(
        session, ride, mapLeftEighthTurnToOrthogonal[trackSequence], DirectionPrev(direction), height, trackElement,
        supportType);
}

static void TrackSBendLeft(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    trackPaintSprites3(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<OpenRCT2::TrackElemType::SBendLeft>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    if ((trackSequence == 0 && (direction == 0 || direction == 3))
        || (trackSequence == 3 && (direction == 1 || direction == 2)))
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    static constexpr std::array<int32_t, 4> blockedSegments = {
        kSegmentsAll,
        EnumsToFlags(
            PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
            PaintSegment::bottomLeft),
        EnumsToFlags(
            PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
            PaintSegment::bottomRight),
        kSegmentsAll,
    };
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments[trackSequence], direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void TrackSBendRight(
    PaintSession& session, const Ride& ride, const uint8_t trackSequence, const Direction direction, const int32_t height,
    const TrackElement& trackElement, const SupportType supportType)
{
    trackPaintSprites3(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<OpenRCT2::TrackElemType::SBendRight>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    if ((trackSequence == 0 && (direction == 0 || direction == 3))
        || (trackSequence == 3 && (direction == 1 || direction == 2)))
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    static constexpr std::array<int32_t, 4> blockedSegments = {
        kSegmentsAll,
        EnumsToFlags(
            PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
            PaintSegment::bottomRight),
        EnumsToFlags(
            PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
            PaintSegment::bottomLeft),
        kSegmentsAll,
    };
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments[trackSequence], direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/**
 * rct2: 0x0074A668
 */
TrackPaintFunction GetTrackPaintFunctionGoKarts(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return PaintGoKartsTrackFlat;

        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return PaintGoKartsStation;

        case TrackElemType::Up25:
            return PaintGoKartsTrack25DegUp;
        case TrackElemType::FlatToUp25:
            return PaintGoKartsTrackFlatTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return PaintGoKartsTrack25DegUpToFlat;

        case TrackElemType::Down25:
            return PaintGoKartsTrack25DegDown;
        case TrackElemType::FlatToDown25:
            return PaintGoKartsTrackFlatTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return PaintGoKartsTrack25DegDownToFlat;

        case TrackElemType::LeftQuarterTurn1Tile:
            return PaintGoKartsTrackLeftQuarterTurn1Tile;
        case TrackElemType::RightQuarterTurn1Tile:
            return PaintGoKartsTrackRightQuarterTurn1Tile;

        // Added by OpenRCT2

        // Steep
        case TrackElemType::Up60:
            return TrackUp60;
        case TrackElemType::Up25ToUp60:
            return TrackUp25ToUp60;
        case TrackElemType::Up60ToUp25:
            return TrackUp60ToUp25;
        case TrackElemType::Down60:
            return TrackDown60;
        case TrackElemType::Down25ToDown60:
            return TrackDown25ToDown60;
        case TrackElemType::Down60ToDown25:
            return TrackDown60ToDown25;

        // Flat to steep
        case TrackElemType::FlatToUp60LongBase:
            return TrackFlatToUp60LongBase;
        case TrackElemType::Up60ToFlatLongBase:
            return TrackUp60ToFlatLongBase;
        case TrackElemType::FlatToDown60LongBase:
            return TrackFlatToDown60LongBase;
        case TrackElemType::Down60ToFlatLongBase:
            return TrackDown60ToFlatLongBase;

        // Small turns
        case TrackElemType::LeftQuarterTurn3Tiles:
            return TrackLeftQuarterTurn3Tiles;
        case TrackElemType::RightQuarterTurn3Tiles:
            return TrackRightQuarterTurn3Tiles;

        // Medium turns
        case TrackElemType::LeftQuarterTurn5Tiles:
            return TrackLeftQuarterTurn5Tiles;
        case TrackElemType::RightQuarterTurn5Tiles:
            return TrackRightQuarterTurn5Tiles;

        // Large turns
        case TrackElemType::LeftEighthToDiag:
            return TrackLeftEighthToDiag;
        case TrackElemType::RightEighthToDiag:
            return TrackRightEighthToDiag;
        case TrackElemType::LeftEighthToOrthogonal:
            return TrackLeftEighthToOrthogonal;
        case TrackElemType::RightEighthToOrthogonal:
            return TrackRightEighthToOrthogonal;

        // Diagonal
        case TrackElemType::DiagFlat:
            return TrackDiagFlat;
        case TrackElemType::DiagFlatToUp25:
            return TrackDiagFlatToUp25;
        case TrackElemType::DiagUp25ToFlat:
            return TrackDiagUp25ToFlat;
        case TrackElemType::DiagFlatToDown25:
            return TrackDiagFlatToDown25;
        case TrackElemType::DiagDown25ToFlat:
            return TrackDiagDown25ToFlat;
        case TrackElemType::DiagUp25:
            return TrackDiagUp25;
        case TrackElemType::DiagDown25:
            return TrackDiagDown25;
        case TrackElemType::DiagUp25ToUp60:
            return TrackDiagUp25ToUp60;
        case TrackElemType::DiagUp60ToUp25:
            return TrackDiagUp60ToUp25;
        case TrackElemType::DiagUp60:
            return TrackDiagUp60;
        case TrackElemType::DiagDown25ToDown60:
            return TrackDiagDown25ToDown60;
        case TrackElemType::DiagDown60ToDown25:
            return TrackDiagDown60ToDown25;
        case TrackElemType::DiagDown60:
            return TrackDiagDown60;

        // Small gentle turns
        case TrackElemType::LeftQuarterTurn3TilesUp25:
            return TrackLeftQuarterTurn3TilesUp25;
        case TrackElemType::RightQuarterTurn3TilesUp25:
            return TrackRightQuarterTurn3TilesUp25;
        case TrackElemType::LeftQuarterTurn3TilesDown25:
            return TrackLeftQuarterTurn3TilesDown25;
        case TrackElemType::RightQuarterTurn3TilesDown25:
            return TrackRightQuarterTurn3TilesDown25;

        // Medium gentle turns
        case TrackElemType::LeftQuarterTurn5TilesUp25:
            return TrackLeftQuarterTurn5TilesUp25;
        case TrackElemType::RightQuarterTurn5TilesUp25:
            return TrackRightQuarterTurn5TilesUp25;
        case TrackElemType::LeftQuarterTurn5TilesDown25:
            return TrackLeftQuarterTurn5TilesDown25;
        case TrackElemType::RightQuarterTurn5TilesDown25:
            return TrackRightQuarterTurn5TilesDown25;

        // Large gentle turns
        case TrackElemType::LeftEighthToDiagUp25:
            return TrackLeftEighthToDiagUp25;
        case TrackElemType::RightEighthToDiagUp25:
            return TrackRightEighthToDiagUp25;
        case TrackElemType::LeftEighthToDiagDown25:
            return TrackLeftEighthToDiagDown25;
        case TrackElemType::RightEighthToDiagDown25:
            return TrackRightEighthToDiagDown25;
        case TrackElemType::LeftEighthToOrthogonalUp25:
            return TrackLeftEighthToOrthogonalUp25;
        case TrackElemType::RightEighthToOrthogonalUp25:
            return TrackRightEighthToOrthogonalUp25;
        case TrackElemType::LeftEighthToOrthogonalDown25:
            return TrackLeftEighthToOrthogonalDown25;
        case TrackElemType::RightEighthToOrthogonalDown25:
            return TrackRightEighthToOrthogonalDown25;

        // S bends
        case TrackElemType::SBendLeft:
            return TrackSBendLeft;
        case TrackElemType::SBendRight:
            return TrackSBendRight;

        default:
            return TrackPaintFunctionDummy;
    }
}
