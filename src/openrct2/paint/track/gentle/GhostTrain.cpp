/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../interface/Viewport.h"
#include "../../../ride/RideData.h"
#include "../../../ride/Track.h"
#include "../../../ride/TrackPaint.h"
#include "../../../world/Map.h"
#include "../../../world/tile_element/TrackElement.h"
#include "../../Paint.h"
#include "../../support/MetalSupports.h"
#include "../../support/WoodenSupports.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroupIncline = TunnelGroup::Standard;

void TrackPaintUtilLeftQuarterTurn1TileTunnel(
    PaintSession& session, Direction direction, uint16_t baseHeight, int8_t startOffset, TunnelType startTunnel,
    int8_t endOffset, TunnelType endTunnel);
void TrackPaintUtilRightQuarterTurn3TilesTunnel(
    PaintSession& session, int16_t height, Direction direction, uint8_t trackSequence, TunnelType tunnelType);

static constexpr TunnelType kDoorOpeningOutwardsToImage[] = {
    TunnelType::Doors2, // Closed
    TunnelType::Doors2, // Unused?
    TunnelType::Doors3, // Half open
    TunnelType::Doors4, // Fully open
    TunnelType::Doors2, // Unused?
    TunnelType::Doors2, // Unused?
    TunnelType::Doors2, // Unused?
};

static constexpr TunnelType kDoorOpeningInwardsToImage[] = {
    TunnelType::Doors2, // Closed
    TunnelType::Doors2, // Unused?
    TunnelType::Doors5, // Half open
    TunnelType::Doors6, // Fully open
    TunnelType::Doors2, // Unused?
    TunnelType::Doors2, // Unused?
    TunnelType::Doors2, // Unused?
};

static TunnelType GetTunnelDoorsImageStraightFlat(const TrackElement& trackElement, uint8_t direction)
{
    switch (direction)
    {
        case 0:
            return kDoorOpeningInwardsToImage[trackElement.GetDoorAState()];
        case 1:
            return kDoorOpeningOutwardsToImage[trackElement.GetDoorBState()];
        case 2:
            return kDoorOpeningOutwardsToImage[trackElement.GetDoorBState()];
        case 3:
            return kDoorOpeningInwardsToImage[trackElement.GetDoorAState()];
    }
    return TunnelType::Doors2;
}

static constexpr TunnelType kDoorFlatTo25DegOpeningOutwardsToImage[] = {
    TunnelType::DoorsFlatTo25Deg2, // Closed
    TunnelType::DoorsFlatTo25Deg2, // Unused?
    TunnelType::DoorsFlatTo25Deg3, // Half open
    TunnelType::DoorsFlatTo25Deg4, // Fully open
    TunnelType::DoorsFlatTo25Deg2, // Unused?
    TunnelType::DoorsFlatTo25Deg2, // Unused?
    TunnelType::DoorsFlatTo25Deg2, // Unused?
};

static constexpr TunnelType kDoorFlatTo25DegOpeningInwardsToImage[] = {
    TunnelType::DoorsFlatTo25Deg2, // Closed
    TunnelType::DoorsFlatTo25Deg2, // Unused?
    TunnelType::DoorsFlatTo25Deg5, // Half open
    TunnelType::DoorsFlatTo25Deg6, // Fully open
    TunnelType::DoorsFlatTo25Deg2, // Unused?
    TunnelType::DoorsFlatTo25Deg2, // Unused?
    TunnelType::DoorsFlatTo25Deg2, // Unused?
};

/** rct2: 0x00770BEC */
static void PaintGhostTrainTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    auto tunnelImage = GetTunnelDoorsImageStraightFlat(trackElement, direction);
    PaintUtilPushTunnelRotated(session, direction, height, tunnelImage);

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

/** rct2: 0x00770BFC */
static void PaintGhostTrainTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 8, height, session.SupportColours);
    }

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height - 8, kTunnelGroupIncline, TunnelSubType::SlopeStart);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height + 8, kTunnelGroupIncline, TunnelSubType::SlopeEnd);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroupIncline, TunnelSubType::SlopeEnd);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height - 8, kTunnelGroupIncline, TunnelSubType::SlopeStart);
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x00770C0C */
static void PaintGhostTrainTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

    bool isBackwards = trackElement.GetTrackType() == TrackElemType::Down25ToFlat;
    TunnelType doorImage;
    if (!isBackwards)
    {
        doorImage = kDoorOpeningInwardsToImage[trackElement.GetDoorAState()];
    }
    else
    {
        doorImage = kDoorOpeningOutwardsToImage[trackElement.GetDoorBState()];
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 3, height, session.SupportColours);
    }

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height, doorImage);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height, kTunnelGroupIncline, TunnelSubType::SlopeEnd);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height, kTunnelGroupIncline, TunnelSubType::SlopeEnd);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height, doorImage);
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

static void PaintGhostTrainTrack25DegUpToFlatShared(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprites2(session, ride, trackSequence, direction, height, trackElement, supportType);

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

/** rct2: 0x00770C1C */
static void PaintGhostTrainTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintGhostTrainTrack25DegUpToFlatShared(session, ride, trackSequence, direction, height, trackElement, supportType);

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height - 8, kTunnelGroupIncline, TunnelSubType::Flat);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height + 8, kDoorFlatTo25DegOpeningOutwardsToImage[trackElement.GetDoorBState()]);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 8, kDoorFlatTo25DegOpeningOutwardsToImage[trackElement.GetDoorBState()]);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height - 8, kTunnelGroupIncline, TunnelSubType::Flat);
            break;
    }
}

/** rct2: 0x00770C2C */
static void PaintGhostTrainTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintGhostTrainTrack25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x00770C3C */
static void PaintGhostTrainTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintGhostTrainTrack25DegUpToFlatShared(
        session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);

    switch ((direction + 2) % 4)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height - 8, kTunnelGroupIncline, TunnelSubType::Flat);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height + 8, kDoorFlatTo25DegOpeningInwardsToImage[trackElement.GetDoorAState()]);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 8, kDoorFlatTo25DegOpeningInwardsToImage[trackElement.GetDoorAState()]);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height - 8, kTunnelGroupIncline, TunnelSubType::Flat);
            break;
    }
}

/** rct2: 0x00770C4C */
static void PaintGhostTrainTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintGhostTrainTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x00770C5C, 0x00770C6C, 0x00770C7C */
static void PaintGhostTrainStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    TrackPaintUtilDrawStationTunnel(session, direction, height);

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

/** rct2: 0x00770C9C */
static void PaintGhostTrainTrackRightQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    bool isBackwards = trackElement.GetTrackType() == TrackElemType::LeftQuarterTurn3Tiles;
    bool isDoorA = (!isBackwards && trackSequence == 0) || (isBackwards && trackSequence == 3);
    auto tunnelType = isDoorA ? kDoorOpeningInwardsToImage[trackElement.GetDoorAState()]
                              : kDoorOpeningOutwardsToImage[trackElement.GetDoorBState()];
    TrackPaintUtilRightQuarterTurn3TilesTunnel(session, height, direction, trackSequence, tunnelType);

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

/** rct2: 0x00770CAC */
static void PaintGhostTrainTrackLeftQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    PaintGhostTrainTrackRightQuarterTurn3Tiles(
        session, ride, trackSequence, (direction + 1) % 4, height, trackElement, supportType);
}

/** rct2: 0x00770CAC */
static void PaintGhostTrainTrackLeftQuarterTurn1Tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    bool isBackwards = trackElement.GetTrackType() == TrackElemType::RightQuarterTurn1Tile;
    TunnelType tunnelStartImage, tunnelEndImage;
    if (!isBackwards)
    {
        tunnelStartImage = kDoorOpeningInwardsToImage[trackElement.GetDoorAState()];
        tunnelEndImage = kDoorOpeningOutwardsToImage[trackElement.GetDoorBState()];
    }
    else
    {
        tunnelStartImage = kDoorOpeningOutwardsToImage[trackElement.GetDoorBState()];
        tunnelEndImage = kDoorOpeningInwardsToImage[trackElement.GetDoorAState()];
    }

    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);
    TrackPaintUtilLeftQuarterTurn1TileTunnel(session, direction, height, 0, tunnelStartImage, 0, tunnelEndImage);

    MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 0, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x00770CBC */
static void PaintGhostTrainTrackRightQuarterTurn1Tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintGhostTrainTrackLeftQuarterTurn1Tile(
        session, ride, trackSequence, (direction + 3) % 4, height, trackElement, supportType);
}

/** rct2: 0x00770CCC */
static void PaintGhostTrainTrackSpinningTunnel(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteSpinningTunnel(session, ride, trackSequence, direction, height, trackElement, supportType);

    auto tunnelImage = GetTunnelDoorsImageStraightFlat(trackElement, direction);
    PaintUtilPushTunnelRotated(session, direction, height, tunnelImage);

    WoodenASupportsPaintSetupRotated(
        session, WoodenSupportType::Truss, WoodenSupportSubType::NeSw, direction, height,
        GetStationColourScheme(session, trackElement));

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x00770CDC */
static void PaintGhostTrainTrackBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSprite(session, ride, trackSequence, direction, height, trackElement, supportType);

    auto tunnelImage = GetTunnelDoorsImageStraightFlat(trackElement, direction);
    PaintUtilPushTunnelRotated(session, direction, height, tunnelImage);

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

/**
 * rct2: 0x00770924
 */
TrackPaintFunction GetTrackPaintFunctionGhostTrain(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return PaintGhostTrainTrackFlat;

        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return PaintGhostTrainStation;

        case TrackElemType::Up25:
            return PaintGhostTrainTrack25DegUp;
        case TrackElemType::FlatToUp25:
            return PaintGhostTrainTrackFlatTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return PaintGhostTrainTrack25DegUpToFlat;

        case TrackElemType::Down25:
            return PaintGhostTrainTrack25DegDown;
        case TrackElemType::FlatToDown25:
            return PaintGhostTrainTrackFlatTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return PaintGhostTrainTrack25DegDownToFlat;

        case TrackElemType::LeftQuarterTurn3Tiles:
            return PaintGhostTrainTrackLeftQuarterTurn3Tiles;
        case TrackElemType::RightQuarterTurn3Tiles:
            return PaintGhostTrainTrackRightQuarterTurn3Tiles;

        case TrackElemType::LeftQuarterTurn1Tile:
            return PaintGhostTrainTrackLeftQuarterTurn1Tile;
        case TrackElemType::RightQuarterTurn1Tile:
            return PaintGhostTrainTrackRightQuarterTurn1Tile;

        case TrackElemType::Brakes:
            return PaintGhostTrainTrackBrakes;

        case TrackElemType::SpinningTunnel:
            return PaintGhostTrainTrackSpinningTunnel;
        default:
            return TrackPaintFunctionDummy;
    }
}
