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

/** rct2: 0x008AFAD4 */
static void AirPoweredVerticalRCTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<TrackElemType::Flat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void AirPoweredVerticalRCTrackStation(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteStation(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<TrackElemType::EndStation>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    TrackPaintUtilDrawStationTunnel(session, direction, height);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void AirPoweredVerticalRCTrackRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<TrackElemType::RightQuarterTurn5Tiles>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    TrackPaintUtilRightQuarterTurn5TilesTunnel(session, kTunnelGroup, TunnelSubType::Flat, height, direction, trackSequence);

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

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void AirPoweredVerticalRCTrackLeftQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    AirPoweredVerticalRCTrackRightQuarterTurn5(
        session, ride, trackSequence, (direction + 1) % 4, height, trackElement, supportType);
}

/** rct2: 0x008AFB74 */
static void AirPoweredVerticalRCTrackFlatToLeftBank(
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

/** rct2: 0x008AFB84 */
static void AirPoweredVerticalRCTrackFlatToRightBank(
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

static void AirPoweredVerticalRCTrackLeftBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    AirPoweredVerticalRCTrackFlatToRightBank(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AFBA4 */
static void AirPoweredVerticalRCTrackRightBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    AirPoweredVerticalRCTrackFlatToLeftBank(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void AirPoweredVerticalRCTrackBankedRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<TrackElemType::BankedRightQuarterTurn5Tiles>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    TrackPaintUtilRightQuarterTurn5TilesTunnel(session, kTunnelGroup, TunnelSubType::Flat, height, direction, trackSequence);

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

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void AirPoweredVerticalRCTrackBankedLeftQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    AirPoweredVerticalRCTrackBankedRightQuarterTurn5(
        session, ride, trackSequence, (direction + 1) % 4, height, trackElement, supportType);
}

/** rct2: 0x008AFBD4 */
static void AirPoweredVerticalRCTrackLeftBank(
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

static void AirPoweredVerticalRCTrackRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    AirPoweredVerticalRCTrackLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void AirPoweredVerticalRCTrackBrakes(
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

static void AirPoweredVerticalRCTrackVerticalSlopeUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintReverseFreefallSlope(session, ride, trackSequence, direction, height, trackElement, supportType);
    DrawSupportForSequenceA<TrackElemType::ReverseFreefallSlope>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    if (trackSequence == 6)
    {
        PaintUtilSetVerticalTunnel(session, height + 240);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    static constexpr int32_t supportHeights[] = { 48, 64, 128, 176, 208, 240, 240 };
    PaintUtilSetGeneralSupportHeight(session, height + supportHeights[trackSequence]);
}

static void AirPoweredVerticalRCTrackVerticalUp(
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

static void AirPoweredVerticalRCTrackVerticalTop(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteSupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 80);
}

static void AirPoweredVerticalRCTrackVerticalDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    AirPoweredVerticalRCTrackVerticalUp(
        session, ride, trackSequence ^ 1, (direction + 2) & 3, height, trackElement, supportType);
}

static void AirPoweredVerticalRCTrackVerticalSlopeDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    AirPoweredVerticalRCTrackVerticalSlopeUp(
        session, ride, 6 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

static void AirPoweredVerticalRCTrackBooster(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    // TODO: create new sprite for this with correct colours
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (direction & 1)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    DrawSupportForSequenceA<TrackElemType::Booster>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void AirPoweredVerticalRCTrackOnridePhoto(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteOnRidePhoto(session, ride, trackSequence, direction, height, trackElement, supportType);

    DrawSupportForSequenceA<TrackElemType::OnRidePhoto>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
}

TrackPaintFunction GetTrackPaintFunctionAirPoweredVerticalRC(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return AirPoweredVerticalRCTrackFlat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return AirPoweredVerticalRCTrackStation;
        case TrackElemType::LeftQuarterTurn5Tiles:
            return AirPoweredVerticalRCTrackLeftQuarterTurn5;
        case TrackElemType::RightQuarterTurn5Tiles:
            return AirPoweredVerticalRCTrackRightQuarterTurn5;
        case TrackElemType::FlatToLeftBank:
            return AirPoweredVerticalRCTrackFlatToLeftBank;
        case TrackElemType::FlatToRightBank:
            return AirPoweredVerticalRCTrackFlatToRightBank;
        case TrackElemType::LeftBankToFlat:
            return AirPoweredVerticalRCTrackLeftBankToFlat;
        case TrackElemType::RightBankToFlat:
            return AirPoweredVerticalRCTrackRightBankToFlat;
        case TrackElemType::BankedLeftQuarterTurn5Tiles:
            return AirPoweredVerticalRCTrackBankedLeftQuarterTurn5;
        case TrackElemType::BankedRightQuarterTurn5Tiles:
            return AirPoweredVerticalRCTrackBankedRightQuarterTurn5;
        case TrackElemType::LeftBank:
            return AirPoweredVerticalRCTrackLeftBank;
        case TrackElemType::RightBank:
            return AirPoweredVerticalRCTrackRightBank;
        case TrackElemType::Brakes:
            return AirPoweredVerticalRCTrackBrakes;
        case TrackElemType::ReverseFreefallSlope:
            return AirPoweredVerticalRCTrackVerticalSlopeUp;
        case TrackElemType::ReverseFreefallVertical:
            return AirPoweredVerticalRCTrackVerticalUp;
        case TrackElemType::AirThrustTopCap:
            return AirPoweredVerticalRCTrackVerticalTop;
        case TrackElemType::AirThrustVerticalDown:
            return AirPoweredVerticalRCTrackVerticalDown;
        case TrackElemType::AirThrustVerticalDownToLevel:
            return AirPoweredVerticalRCTrackVerticalSlopeDown;
        case TrackElemType::Booster:
            return AirPoweredVerticalRCTrackBooster;
        case TrackElemType::OnRidePhoto:
            return AirPoweredVerticalRCTrackOnridePhoto;
        default:
            return TrackPaintFunctionDummy;
    }
}
