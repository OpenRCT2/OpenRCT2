/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
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
#include "../../Paint.h"
#include "../../support/MetalSupports.h"
#include "../../support/WoodenSupports.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

using namespace OpenRCT2;

void TrackPaintUtilLeftQuarterTurn1TileTunnel(
    PaintSession& session, Direction direction, uint16_t baseHeight, int8_t startOffset, TunnelType startTunnel,
    int8_t endOffset, TunnelType endTunnel);
void TrackPaintUtilRightQuarterTurn3TilesTunnel(
    PaintSession& session, int16_t height, Direction direction, uint8_t trackSequence, TunnelType tunnelType);

enum
{
    SprGhostTrainTrackFlatSwNe = 28821,
    SprGhostTrainTrackFlatNwSe = 28822,
    SprGhostTrainTrackFlatTo25DegUpSwNe = 28823,
    SprGhostTrainTrackFlatTo25DegUpNwSe = 28824,
    SprGhostTrainTrackFlatTo25DegUpNeSw = 28825,
    SprGhostTrainTrackFlatTo25DegUpSeNw = 28826,
    SprGhostTrainTrack25DegUpToFlatSwNe = 28827,
    SprGhostTrainTrack25DegUpToFlatNwSe = 28828,
    SprGhostTrainTrack25DegUpToFlatNeSw = 28829,
    SprGhostTrainTrack25DegUpToFlatSeNw = 28830,
    SprGhostTrainTrack25DegUpSwNe = 28831,
    SprGhostTrainTrack25DegUpNwSe = 28832,
    SprGhostTrainTrack25DegUpNeSw = 28833,
    SprGhostTrainTrack25DegUpSeNw = 28834,
    SprGhostTrainTrackFlatTo25DegUpFrontSwNe = 28835,
    SprGhostTrainTrackFlatTo25DegUpFrontNwSe = 28836,
    SprGhostTrainTrackFlatTo25DegUpFrontNeSw = 28837,
    SprGhostTrainTrackFlatTo25DegUpFrontSeNw = 28838,
    SprGhostTrainTrack25DegUpToFlatFrontSwNe = 28839,
    SprGhostTrainTrack25DegUpToFlatFrontNwSe = 28840,
    SprGhostTrainTrack25DegUpToFlatFrontNeSw = 28841,
    SprGhostTrainTrack25DegUpToFlatFrontSeNw = 28842,
    SprGhostTrainTrack25DegUpFrontSwNe = 28843,
    SprGhostTrainTrack25DegUpFrontNwSe = 28844,
    SprGhostTrainTrack25DegUpFrontNeSw = 28845,
    SprGhostTrainTrack25DegUpFrontSeNw = 28846,
    SprGhostTrainQuarterTurn1TileSwNw = 28847,
    SprGhostTrainQuarterTurn1TileNwNe = 28848,
    SprGhostTrainQuarterTurn1TileNeSe = 28849,
    SprGhostTrainQuarterTurn1TileSeSw = 28850,
    SprGhostTrainQuarterTurn3TilesSwSePart0 = 28851,
    SprGhostTrainQuarterTurn3TilesSwSePart1 = 28852,
    SprGhostTrainQuarterTurn3TilesSwSePart2 = 28853,
    SprGhostTrainQuarterTurn3TilesNwSwPart0 = 28854,
    SprGhostTrainQuarterTurn3TilesNwSwPart1 = 28855,
    SprGhostTrainQuarterTurn3TilesNwSwPart2 = 28856,
    SprGhostTrainQuarterTurn3TilesNeNwPart0 = 28857,
    SprGhostTrainQuarterTurn3TilesNeNwPart1 = 28858,
    SprGhostTrainQuarterTurn3TilesNeNwPart2 = 28859,
    SprGhostTrainQuarterTurn3TilesSeNePart0 = 28860,
    SprGhostTrainQuarterTurn3TilesSeNePart1 = 28861,
    SprGhostTrainQuarterTurn3TilesSeNePart2 = 28862,
    SprGhostTrainSpinningTunnelTrackSwNe = 28863,
    SprGhostTrainSpinningTunnelTrackNwSe = 28864,

    SprGhostTrainTrackBrakesSwNe = 28881,
    SprGhostTrainTrackBrakesNwSe = 28882
};

static constexpr uint32_t GhostTrainTrackPiecesFlat[4] = {
    SprGhostTrainTrackFlatSwNe,
    SprGhostTrainTrackFlatNwSe,
    SprGhostTrainTrackFlatSwNe,
    SprGhostTrainTrackFlatNwSe,
};

static constexpr uint32_t GhostTrainTrackPiecesFlatTo25DegUp[4][2] = {
    { SprGhostTrainTrackFlatTo25DegUpSwNe, SprGhostTrainTrackFlatTo25DegUpFrontSwNe },
    { SprGhostTrainTrackFlatTo25DegUpNwSe, SprGhostTrainTrackFlatTo25DegUpFrontNwSe },
    { SprGhostTrainTrackFlatTo25DegUpNeSw, SprGhostTrainTrackFlatTo25DegUpFrontNeSw },
    { SprGhostTrainTrackFlatTo25DegUpSeNw, SprGhostTrainTrackFlatTo25DegUpFrontSeNw },
};

static constexpr uint32_t GhostTrainTrackPieces25DegUpToFlat[4][2] = {
    { SprGhostTrainTrack25DegUpToFlatSwNe, SprGhostTrainTrack25DegUpToFlatFrontSwNe },
    { SprGhostTrainTrack25DegUpToFlatNwSe, SprGhostTrainTrack25DegUpToFlatFrontNwSe },
    { SprGhostTrainTrack25DegUpToFlatNeSw, SprGhostTrainTrack25DegUpToFlatFrontNeSw },
    { SprGhostTrainTrack25DegUpToFlatSeNw, SprGhostTrainTrack25DegUpToFlatFrontSeNw },
};

static constexpr uint32_t GhostTrainTrackPieces25DegUp[4][2] = {
    { SprGhostTrainTrack25DegUpSwNe, SprGhostTrainTrack25DegUpFrontSwNe },
    { SprGhostTrainTrack25DegUpNwSe, SprGhostTrainTrack25DegUpFrontNwSe },
    { SprGhostTrainTrack25DegUpNeSw, SprGhostTrainTrack25DegUpFrontNeSw },
    { SprGhostTrainTrack25DegUpSeNw, SprGhostTrainTrack25DegUpFrontSeNw },
};

static constexpr uint32_t GhostTrainTrackPiecesQuarterTurn1Tile[4] = {
    SprGhostTrainQuarterTurn1TileSwNw,
    SprGhostTrainQuarterTurn1TileNwNe,
    SprGhostTrainQuarterTurn1TileNeSe,
    SprGhostTrainQuarterTurn1TileSeSw,
};

static constexpr uint32_t GhostTrainTrackPiecesQuarterTurn3Tiles[4][3] = {
    {
        SprGhostTrainQuarterTurn3TilesSwSePart0,
        SprGhostTrainQuarterTurn3TilesSwSePart1,
        SprGhostTrainQuarterTurn3TilesSwSePart2,
    },
    {
        SprGhostTrainQuarterTurn3TilesNwSwPart0,
        SprGhostTrainQuarterTurn3TilesNwSwPart1,
        SprGhostTrainQuarterTurn3TilesNwSwPart2,
    },
    {
        SprGhostTrainQuarterTurn3TilesNeNwPart0,
        SprGhostTrainQuarterTurn3TilesNeNwPart1,
        SprGhostTrainQuarterTurn3TilesNeNwPart2,
    },
    {
        SprGhostTrainQuarterTurn3TilesSeNePart0,
        SprGhostTrainQuarterTurn3TilesSeNePart1,
        SprGhostTrainQuarterTurn3TilesSeNePart2,
    },
};

static constexpr uint32_t ghost_train_track_pieces_spinning_tunnel_track[4] = {
    SprGhostTrainSpinningTunnelTrackSwNe,
    SprGhostTrainSpinningTunnelTrackNwSe,
    SprGhostTrainSpinningTunnelTrackSwNe,
    SprGhostTrainSpinningTunnelTrackNwSe,
};

static constexpr uint32_t GhostTrainTrackPiecesBrakes[4] = {
    SprGhostTrainTrackBrakesSwNe,
    SprGhostTrainTrackBrakesNwSe,
    SprGhostTrainTrackBrakesSwNe,
    SprGhostTrainTrackBrakesNwSe,
};

static constexpr TunnelType DoorOpeningOutwardsToImage[] = {
    TunnelType::Doors2, // Closed
    TunnelType::Doors2, // Unused?
    TunnelType::Doors3, // Half open
    TunnelType::Doors4, // Fully open
    TunnelType::Doors2, // Unused?
    TunnelType::Doors2, // Unused?
    TunnelType::Doors2, // Unused?
};

static constexpr TunnelType DoorOpeningInwardsToImage[] = {
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
            return DoorOpeningInwardsToImage[trackElement.GetDoorAState()];
        case 1:
            return DoorOpeningOutwardsToImage[trackElement.GetDoorBState()];
        case 2:
            return DoorOpeningOutwardsToImage[trackElement.GetDoorBState()];
        case 3:
            return DoorOpeningInwardsToImage[trackElement.GetDoorAState()];
    }
    return TunnelType::Doors2;
}

/** rct2: 0x00770BEC */
static void PaintGhostTrainTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    auto imageId = session.TrackColours.WithIndex(GhostTrainTrackPiecesFlat[direction]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });

    auto tunnelImage = GetTunnelDoorsImageStraightFlat(trackElement, direction);
    PaintUtilPushTunnelRotated(session, direction, height, tunnelImage);

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 0, height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x00770BFC */
static void PaintGhostTrainTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    auto imageId = session.TrackColours.WithIndex(GhostTrainTrackPieces25DegUp[direction][0]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });

    imageId = session.TrackColours.WithIndex(GhostTrainTrackPieces25DegUp[direction][1]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 23 } });

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 8, height, session.SupportColours);
    }

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height - 8, TunnelGroup::Standard, TunnelSubType::SlopeStart);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height + 8, TunnelGroup::Standard, TunnelSubType::SlopeEnd);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 8, TunnelGroup::Standard, TunnelSubType::SlopeEnd);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height - 8, TunnelGroup::Standard, TunnelSubType::SlopeStart);
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x00770C0C */
static void PaintGhostTrainTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    bool isBackwards = trackElement.GetTrackType() == TrackElemType::Down25ToFlat;
    TunnelType doorImage;
    if (!isBackwards)
    {
        doorImage = DoorOpeningInwardsToImage[trackElement.GetDoorAState()];
    }
    else
    {
        doorImage = DoorOpeningOutwardsToImage[trackElement.GetDoorBState()];
    }

    auto imageId = session.TrackColours.WithIndex(GhostTrainTrackPiecesFlatTo25DegUp[direction][0]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });

    imageId = session.TrackColours.WithIndex(GhostTrainTrackPiecesFlatTo25DegUp[direction][1]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 15 } });

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
            PaintUtilPushTunnelRight(session, height, TunnelGroup::Standard, TunnelSubType::SlopeEnd);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height, TunnelGroup::Standard, TunnelSubType::SlopeEnd);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height, doorImage);
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

static void PaintGhostTrainTrack25DegUpToFlatShared(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    auto imageId = session.TrackColours.WithIndex(GhostTrainTrackPieces25DegUpToFlat[direction][0]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });

    imageId = session.TrackColours.WithIndex(GhostTrainTrackPieces25DegUpToFlat[direction][1]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 15 } });

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 6, height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide), direction),
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
            PaintUtilPushTunnelLeft(session, height - 8, TunnelGroup::Standard, TunnelSubType::Flat);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height + 8, DoorOpeningOutwardsToImage[trackElement.GetDoorBState()]);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 8, DoorOpeningOutwardsToImage[trackElement.GetDoorBState()]);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height - 8, TunnelGroup::Standard, TunnelSubType::Flat);
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
            PaintUtilPushTunnelLeft(session, height - 8, TunnelGroup::Standard, TunnelSubType::Flat);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height + 8, DoorOpeningInwardsToImage[trackElement.GetDoorAState()]);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 8, DoorOpeningInwardsToImage[trackElement.GetDoorAState()]);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height - 8, TunnelGroup::Standard, TunnelSubType::Flat);
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
    ImageId imageId;

    static constexpr std::array imageIds = {
        SPR_STATION_BASE_B_SW_NE,
        SPR_STATION_BASE_B_NW_SE,
        SPR_STATION_BASE_B_SW_NE,
        SPR_STATION_BASE_B_NW_SE,
    };

    imageId = GetStationColourScheme(session, trackElement).WithIndex(imageIds[direction]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height - 2 }, { { 0, 2, height }, { 32, 28, 3 } });

    imageId = session.TrackColours.WithIndex(GhostTrainTrackPiecesFlat[direction]);
    PaintAddImageAsChildRotated(session, direction, imageId, { 0, 0, height }, { { 0, 0, height }, { 32, 20, 3 } });

    PaintUtilPushTunnelRotated(session, direction, height, TunnelGroup::Square, TunnelSubType::Flat);

    DrawSupportsSideBySide(session, direction, height, session.SupportColours, supportType.metal);

    TrackPaintUtilDrawStation(session, ride, direction, height, trackElement);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x00770C9C */
static void PaintGhostTrainTrackRightQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    TrackPaintUtilRightQuarterTurn3TilesPaint(
        session, 3, height, direction, trackSequence, session.TrackColours, GhostTrainTrackPiecesQuarterTurn3Tiles, nullptr,
        defaultRightQuarterTurn3TilesBoundLengths, defaultRightQuarterTurn3TilesBoundOffsets);
    bool isBackwards = trackElement.GetTrackType() == TrackElemType::LeftQuarterTurn3Tiles;
    bool isDoorA = (!isBackwards && trackSequence == 0) || (isBackwards && trackSequence == 3);
    auto tunnelType = isDoorA ? DoorOpeningInwardsToImage[trackElement.GetDoorAState()]
                              : DoorOpeningOutwardsToImage[trackElement.GetDoorBState()];
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
                PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide, PaintSegment::rightCorner);
            break;
        case 2:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::bottomRightSide, PaintSegment::bottomCorner);
            break;
        case 3:
            blockedSegments = EnumsToFlags(
                PaintSegment::bottomRightSide, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::leftCorner);
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
        tunnelStartImage = DoorOpeningInwardsToImage[trackElement.GetDoorAState()];
        tunnelEndImage = DoorOpeningOutwardsToImage[trackElement.GetDoorBState()];
    }
    else
    {
        tunnelStartImage = DoorOpeningOutwardsToImage[trackElement.GetDoorBState()];
        tunnelEndImage = DoorOpeningInwardsToImage[trackElement.GetDoorAState()];
    }

    TrackPaintUtilLeftQuarterTurn1TilePaint(
        session, 3, height, 0, direction, session.TrackColours, GhostTrainTrackPiecesQuarterTurn1Tile);
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
    auto imageId = session.TrackColours.WithIndex(ghost_train_track_pieces_spinning_tunnel_track[direction]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 2, 6, height }, { 28, 20, 3 } });

    TrackPaintUtilSpinningTunnelPaint(session, 3, height, direction);

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
    auto imageId = session.TrackColours.WithIndex(GhostTrainTrackPiecesBrakes[direction]);

    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });

    auto tunnelImage = GetTunnelDoorsImageStraightFlat(trackElement, direction);
    PaintUtilPushTunnelRotated(session, direction, height, tunnelImage);

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 0, height, session.SupportColours);
    }

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/**
 * rct2: 0x00770924
 */
TRACK_PAINT_FUNCTION GetTrackPaintFunctionGhostTrain(int32_t trackType)
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
    }

    return nullptr;
}
