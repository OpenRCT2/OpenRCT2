/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../common.h"
#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../../world/Map.h"
#include "../RideData.h"
#include "../Track.h"
#include "../TrackPaint.h"
#include "../VehiclePaint.h"

enum
{
    SprCarRideFlatSwNe = 28773,
    SprCarRideFlatNwSe = 28774,
    SprCarRideLogBumpsSwNe = 28775,
    SprCarRideLogBumpsNwSe = 28776,
    SprCarRide25DegUpSwNe = 28777,
    SprCarRideFlatTo25DegUpSwNe = 28778,
    SprCarRide25DegUpNeSw = 28779,
    SprCarRideFlatTo25DegUpNeSw = 28780,
    SprCarRideDegUpToFlatNeSw = 28781,
    SprCarRideDegUpToFlatSwNe = 28782,
    SprCarRide25DegUpNwSe = 28783,
    SprCarRide25DegUpSeNw = 28784,
    SprCarRideFlatTo25DegUpNwSe = 28785,
    SprCarRideFlatTo25DegUpSeNw = 28786,
    SprCarRideDegUpToFlatSeNw = 28787,
    SprCarRideDegUpToFlatNwSe = 28788,
    SprCarRide25DegUpTo60DegUpSwNe = 28789,
    SprCarRide25DegUpTo60DegUpNwSe = 28790,
    SprCarRide25DegUpTo60DegUpNeSw = 28791,
    SprCarRide25DegUpTo60DegUpSeNw = 28792,
    SprCarRide25DegUpTo60DegUpFrontNwSe = 28793,
    SprCarRide25DegUpTo60DegUpFrontNeSw = 28794,
    SprCarRide60DegUpTo25DegUpSwNe = 28795,
    SprCarRide60DegUpTo25DegUpNwSe = 28796,
    SprCarRide60DegUpTo25DegUpNeSw = 28797,
    SprCarRide60DegUpTo25DegUpSeNw = 28798,
    SprCarRide60DegUpTo25DegUpFrontNwSe = 28799,
    SprCarRide60DegUpTo25DegUpFrontNeSw = 28800,
    SprCarRide60DegUpSwNe = 28801,
    SprCarRide60DegUpNwSe = 28802,
    SprCarRide60DegUpNeSw = 28803,
    SprCarRide60DegUpSeNw = 28804,
    SprCarRideQuarterTurn1TileSwNw = 28805,
    SprCarRideQuarterTurn1TileNwNe = 28806,
    SprCarRideQuarterTurn1TileNeSe = 28807,
    SprCarRideQuarterTurn1TileSeSw = 28808,
    SprCarRideQuarterTurn3TilesSwSePart0 = 28809,
    SprCarRideQuarterTurn3TilesSwSePart1 = 28810,
    SprCarRideQuarterTurn3TilesSwSePart2 = 28811,
    SprCarRideQuarterTurn3TilesNwSwPart0 = 28812,
    SprCarRideQuarterTurn3TilesNwSwPart1 = 28813,
    SprCarRideQuarterTurn3TilesNwSwPart2 = 28814,
    SprCarRideQuarterTurn3TilesNeNwPart0 = 28815,
    SprCarRideQuarterTurn3TilesNeNwPart1 = 28816,
    SprCarRideQuarterTurn3TilesNeNwPart2 = 28817,
    SprCarRideQuarterTurn3TilesSeNePart0 = 28818,
    SprCarRideQuarterTurn3TilesSeNePart1 = 28819,
    SprCarRideQuarterTurn3TilesSeNePart2 = 28820,
};

static constexpr const uint32_t CarRideTrackPiecesFlat[4] = {
    SprCarRideFlatSwNe,
    SprCarRideFlatNwSe,
    SprCarRideFlatSwNe,
    SprCarRideFlatNwSe,
};

static constexpr const uint32_t CarRideTrackPiecesLogBumps[4] = {
    SprCarRideLogBumpsSwNe,
    SprCarRideLogBumpsNwSe,
    SprCarRideLogBumpsSwNe,
    SprCarRideLogBumpsNwSe,
};

static constexpr const uint32_t CarRideTrackPieces25DegUp[4] = {
    SprCarRide25DegUpSwNe,
    SprCarRide25DegUpNwSe,
    SprCarRide25DegUpNeSw,
    SprCarRide25DegUpSeNw,
};

static constexpr const uint32_t CarRideTrackPiecesFlatTo25DegUp[4] = {
    SprCarRideFlatTo25DegUpSwNe,
    SprCarRideFlatTo25DegUpNwSe,
    SprCarRideFlatTo25DegUpNeSw,
    SprCarRideFlatTo25DegUpSeNw,
};

static constexpr const uint32_t CarRideTrackPieces25DegUpToFlat[4] = {
    SprCarRideDegUpToFlatSwNe,
    SprCarRideDegUpToFlatNwSe,
    SprCarRideDegUpToFlatNeSw,
    SprCarRideDegUpToFlatSeNw,
};

static constexpr const uint32_t CarRideTrackPieces60DegUp[4] = {
    SprCarRide60DegUpSwNe,
    SprCarRide60DegUpNwSe,
    SprCarRide60DegUpNeSw,
    SprCarRide60DegUpSeNw,
};

static constexpr const uint32_t CarRideTrackPieces25DegUpTo60DegUp[4][2] = {
    { SprCarRide25DegUpTo60DegUpSwNe, 0 },
    { SprCarRide25DegUpTo60DegUpNwSe, SprCarRide25DegUpTo60DegUpFrontNwSe },
    { SprCarRide25DegUpTo60DegUpNeSw, SprCarRide25DegUpTo60DegUpFrontNeSw },
    { SprCarRide25DegUpTo60DegUpSeNw, 0 },
};

static constexpr const uint32_t CarRideTrackPieces60DegUpTo25DegUp[4][2] = {
    { SprCarRide60DegUpTo25DegUpSwNe, 0 },
    { SprCarRide60DegUpTo25DegUpNwSe, SprCarRide60DegUpTo25DegUpFrontNwSe },
    { SprCarRide60DegUpTo25DegUpNeSw, SprCarRide60DegUpTo25DegUpFrontNeSw },
    { SprCarRide60DegUpTo25DegUpSeNw, 0 },
};

static constexpr const uint32_t CarRideTrackPiecesLeftQuarterTurn1Tile[4] = {
    SprCarRideQuarterTurn1TileSwNw,
    SprCarRideQuarterTurn1TileNwNe,
    SprCarRideQuarterTurn1TileNeSe,
    SprCarRideQuarterTurn1TileSeSw,
};

static constexpr const uint32_t CarRideTrackPiecesQuarterTurn3Tiles[4][3] = {
    {
        SprCarRideQuarterTurn3TilesSwSePart0,
        SprCarRideQuarterTurn3TilesSwSePart1,
        SprCarRideQuarterTurn3TilesSwSePart2,
    },
    {
        SprCarRideQuarterTurn3TilesNwSwPart0,
        SprCarRideQuarterTurn3TilesNwSwPart1,
        SprCarRideQuarterTurn3TilesNwSwPart2,
    },
    {
        SprCarRideQuarterTurn3TilesNeNwPart0,
        SprCarRideQuarterTurn3TilesNeNwPart1,
        SprCarRideQuarterTurn3TilesNeNwPart2,
    },
    {
        SprCarRideQuarterTurn3TilesSeNePart0,
        SprCarRideQuarterTurn3TilesSeNePart1,
        SprCarRideQuarterTurn3TilesSeNePart2,
    },
};

/** rct2: 0x006F72C8 */
static void PaintCarRideTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(CarRideTrackPiecesFlat[direction]);

    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsParent(session, imageId, { 0, 6, height }, { 32, 20, 1 });
    }
    else
    {
        PaintAddImageAsParent(session, imageId, { 6, 0, height }, { 20, 32, 1 });
    }

    if (direction == 0 || direction == 2)
    {
        PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
    }
    else
    {
        PaintUtilPushTunnelRight(session, height, TUNNEL_0);
    }

    MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x006F72D8 */
static void PaintCarRideTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(CarRideTrackPieces25DegUp[direction]);

    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsParent(session, imageId, { 0, 2, height }, { { 0, 6, height }, { 32, 20, 1 } });
    }
    else
    {
        PaintAddImageAsParent(session, imageId, { 2, 0, height }, { { 6, 0, height }, { 20, 32, 1 } });
    }

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height - 8, TUNNEL_1);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height + 8, TUNNEL_2);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_2);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height - 8, TUNNEL_1);
            break;
    }

    MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x006F72E8 */
static void PaintCarRideTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(CarRideTrackPiecesFlatTo25DegUp[direction]);

    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsParent(session, imageId, { 0, 2, height }, { { 0, 6, height }, { 32, 20, 1 } });
    }
    else
    {
        PaintAddImageAsParent(session, imageId, { 2, 0, height }, { { 6, 0, height }, { 20, 32, 1 } });
    }

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height, TUNNEL_2);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height, TUNNEL_2);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height, TUNNEL_0);
            break;
    }

    MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x006F72F8 */
static void PaintCarRideTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(CarRideTrackPieces25DegUpToFlat[direction]);

    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsParent(session, imageId, { 0, 2, height }, { { 0, 6, height }, { 32, 20, 1 } });
    }
    else
    {
        PaintAddImageAsParent(session, imageId, { 2, 0, height }, { { 6, 0, height }, { 20, 32, 1 } });
    }

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height - 8, TUNNEL_0);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height + 8, TUNNEL_12);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_12);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height - 8, TUNNEL_0);
            break;
    }

    MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

/** rct2: 0x006F7308 */
static void PaintCarRideTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintCarRideTrack25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

/** rct2: 0x006F7318 */
static void PaintCarRideTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintCarRideTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

/** rct2: 0x006F7328 */
static void PaintCarRideTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintCarRideTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

/** rct2: 0x006F7338, 0x006F7348, 0x006F7358 */
static void PaintCarRideStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    ImageId imageId;

    if (direction == 0 || direction == 2)
    {
        imageId = session.TrackColours[SCHEME_MISC].WithIndex(SPR_STATION_BASE_B_SW_NE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height - 2 }, { { 0, 2, height }, { 32, 28, 1 } });
    }
    else if (direction == 1 || direction == 3)
    {
        imageId = session.TrackColours[SCHEME_MISC].WithIndex(SPR_STATION_BASE_B_NW_SE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height - 2 }, { { 2, 0, height }, { 28, 32, 1 } });
    }

    imageId = session.TrackColours[SCHEME_TRACK].WithIndex(CarRideTrackPiecesFlat[direction]);
    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsChild(session, imageId, { 0, 6, height }, { { 0, 0, height }, { 32, 20, 1 } });
    }
    else
    {
        PaintAddImageAsChild(session, imageId, { 6, 0, height }, { { 0, 0, height }, { 20, 32, 1 } });
    }

    if (direction == 0 || direction == 2)
    {
        PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
    }

    if (direction == 0 || direction == 2)
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    else
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    TrackPaintUtilDrawStation(session, ride, direction, height, trackElement);

    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x006F7378 */
static void PaintCarRideTrackRightQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    TrackPaintUtilRightQuarterTurn3TilesPaint(
        session, 3, height, direction, trackSequence, session.TrackColours[SCHEME_TRACK], CarRideTrackPiecesQuarterTurn3Tiles,
        defaultRightQuarterTurn3TilesOffsets, defaultRightQuarterTurn3TilesBoundLengths, nullptr);
    TrackPaintUtilRightQuarterTurn3TilesTunnel(session, height, direction, trackSequence, TUNNEL_0);

    switch (trackSequence)
    {
        case 0:
        case 3:
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
    }

    int32_t blockedSegments = 0;
    switch (trackSequence)
    {
        case 0:
            blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_BC;
            break;
        case 2:
            blockedSegments = SEGMENT_D0 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_C0;
            break;
        case 3:
            blockedSegments = SEGMENT_D4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_B8;
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments, direction), 0xFFFF, 0);

    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x006F7368 */
static void PaintCarRideTrackLeftQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    PaintCarRideTrackRightQuarterTurn3Tiles(session, ride, trackSequence, (direction + 1) % 4, height, trackElement);
}

/** rct2: 0x006F7388 */
static void PaintCarRideTrackLeftQuarterTurn1Tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(CarRideTrackPiecesLeftQuarterTurn1Tile[direction]);

    CoordsXYZ offset;
    switch (direction)
    {
        case 0:
            PaintAddImageAsParent(session, imageId, { 6, 0, height }, { { 6, 2, height }, { 26, 24, 1 } });
            break;
        case 1:
            offset = { 0, 0, height };
            PaintAddImageAsParent(session, imageId, offset, { offset, { 26, 26, 1 } });
            break;
        case 2:
            PaintAddImageAsParent(session, imageId, { 0, 6, height }, { { 2, 6, height }, { 24, 26, 1 } });
            break;
        case 3:
            offset = { 6, 6, height };
            PaintAddImageAsParent(session, imageId, offset, { offset, { 24, 24, 1 } });
            break;
    }

    MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);

    TrackPaintUtilLeftQuarterTurn1TileTunnel(session, direction, height, 0, TUNNEL_0, 0, TUNNEL_0);

    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x006F7398 */
static void PaintCarRideTrackRightQuarterTurn1Tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintCarRideTrackLeftQuarterTurn1Tile(session, ride, trackSequence, (direction + 3) % 4, height, trackElement);
}

/** rct2: 0x006F73A8 */
static void PaintCarRideTrackSpinningTunnel(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(CarRideTrackPiecesFlat[direction]);

    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsParent(session, imageId, { 0, 6, height }, { 32, 20, 1 });
    }
    else
    {
        PaintAddImageAsParent(session, imageId, { 6, 0, height }, { 20, 32, 1 });
    }

    TrackPaintUtilSpinningTunnelPaint(session, 1, height, direction);

    if (direction == 0 || direction == 2)
    {
        PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
    }
    else
    {
        PaintUtilPushTunnelRight(session, height, TUNNEL_0);
    }

    WoodenASupportsPaintSetup(session, (direction & 1), 0, height, session.TrackColours[SCHEME_MISC]);

    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x006F73B8 */
static void PaintCarRideTrack60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(CarRideTrackPieces60DegUp[direction]);

    switch (direction)
    {
        case 0:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
            break;
        case 1:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 27, 0, height }, { 1, 32, 98 } });
            break;
        case 2:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 98 } });
            break;
        case 3:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 6, 0, height }, { 20, 32, 1 } });
            break;
    }

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height - 8, TUNNEL_1);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height + 56, TUNNEL_2);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 56, TUNNEL_2);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height - 8, TUNNEL_1);
            break;
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
}

/** rct2: 0x006F73C8 */
static void PaintCarRideTrack25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(CarRideTrackPieces25DegUpTo60DegUp[direction][0]);

    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
    }
    else
    {
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 6, 0, height }, { 20, 32, 1 } });
    }

    if (CarRideTrackPieces25DegUpTo60DegUp[direction][1] != 0)
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(CarRideTrackPieces25DegUpTo60DegUp[direction][1]);

        if (direction == 0 || direction == 2)
        {
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 66 } });
        }
        else
        {
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 27, 0, height }, { 1, 32, 66 } });
        }
    }

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height - 8, TUNNEL_1);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height + 24, TUNNEL_2);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 24, TUNNEL_2);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height - 8, TUNNEL_1);
            break;
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
}

/** rct2: 0x006F73D8 */
static void PaintCarRideTrack60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(CarRideTrackPieces60DegUpTo25DegUp[direction][0]);

    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
    }
    else
    {
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 6, 0, height }, { 20, 32, 1 } });
    }

    if (CarRideTrackPieces60DegUpTo25DegUp[direction][1] != 0)
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(CarRideTrackPieces60DegUpTo25DegUp[direction][1]);

        if (direction == 0 || direction == 2)
        {
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 66 } });
        }
        else
        {
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 27, 0, height }, { 1, 32, 66 } });
        }
    }

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height - 8, TUNNEL_1);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height + 24, TUNNEL_2);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 24, TUNNEL_2);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height - 8, TUNNEL_1);
            break;
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
}

/** rct2: 0x006F73E8 */
static void PaintCarRideTrack60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintCarRideTrack60DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

/** rct2: 0x006F73F8 */
static void PaintCarRideTrack25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintCarRideTrack60DegUpTo25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

/** rct2: 0x006F7408 */
static void PaintCarRideTrack60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    PaintCarRideTrack25DegUpTo60DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

/** rct2: 0x006F7418 */
static void PaintCarRideTrackLogBumps(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(CarRideTrackPiecesLogBumps[direction]);

    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsParent(session, imageId, { 0, 6, height }, { 32, 20, 1 });
    }
    else
    {
        PaintAddImageAsParent(session, imageId, { 6, 0, height }, { 20, 32, 1 });
    }

    if (direction == 0 || direction == 2)
    {
        PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
    }
    else
    {
        PaintUtilPushTunnelRight(session, height, TUNNEL_0);
    }

    MetalASupportsPaintSetup(session, METAL_SUPPORTS_BOXED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/**
 * rct2: 0x006F7000
 */
TRACK_PAINT_FUNCTION GetTrackPaintFunctionCarRide(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return PaintCarRideTrackFlat;

        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return PaintCarRideStation;

        case TrackElemType::Up25:
            return PaintCarRideTrack25DegUp;
        case TrackElemType::Up60:
            return PaintCarRideTrack60DegUp;
        case TrackElemType::FlatToUp25:
            return PaintCarRideTrackFlatTo25DegUp;
        case TrackElemType::Up25ToUp60:
            return PaintCarRideTrack25DegUpTo60DegUp;
        case TrackElemType::Up60ToUp25:
            return PaintCarRideTrack60DegUpTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return PaintCarRideTrack25DegUpToFlat;

        case TrackElemType::Down25:
            return PaintCarRideTrack25DegDown;
        case TrackElemType::Down60:
            return PaintCarRideTrack60DegDown;
        case TrackElemType::FlatToDown25:
            return PaintCarRideTrackFlatTo25DegDown;
        case TrackElemType::Down25ToDown60:
            return PaintCarRideTrack25DegDownTo60DegDown;
        case TrackElemType::Down60ToDown25:
            return PaintCarRideTrack60DegDownTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return PaintCarRideTrack25DegDownToFlat;

        case TrackElemType::LeftQuarterTurn3Tiles:
            return PaintCarRideTrackLeftQuarterTurn3Tiles;
        case TrackElemType::RightQuarterTurn3Tiles:
            return PaintCarRideTrackRightQuarterTurn3Tiles;

        case TrackElemType::LeftQuarterTurn1Tile:
            return PaintCarRideTrackLeftQuarterTurn1Tile;
        case TrackElemType::RightQuarterTurn1Tile:
            return PaintCarRideTrackRightQuarterTurn1Tile;

        case TrackElemType::Rapids:
            return PaintCarRideTrackLogBumps;

        case TrackElemType::SpinningTunnel:
            return PaintCarRideTrackSpinningTunnel;
    }

    return nullptr;
}
