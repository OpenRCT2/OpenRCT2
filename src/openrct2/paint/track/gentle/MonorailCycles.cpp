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
#include "../../../world/Map.h"
#include "../../Paint.h"
#include "../../support/MetalSupports.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Standard;

enum
{
    SprMonorailCyclesFlatSwNe = 16820,
    SprMonorailCyclesFlatNwSe = 16821,
    SprMonorailCyclesFlatQuarterTurn5TilesSwSePart0 = 16822,
    SprMonorailCyclesFlatQuarterTurn5TilesSwSePart1 = 16823,
    SprMonorailCyclesFlatQuarterTurn5TilesSwSePart2 = 16824,
    SprMonorailCyclesFlatQuarterTurn5TilesSwSePart3 = 16825,
    SprMonorailCyclesFlatQuarterTurn5TilesSwSePart4 = 16826,
    SprMonorailCyclesFlatQuarterTurn5TilesNwSwPart0 = 16827,
    SprMonorailCyclesFlatQuarterTurn5TilesNwSwPart1 = 16828,
    SprMonorailCyclesFlatQuarterTurn5TilesNwSwPart2 = 16829,
    SprMonorailCyclesFlatQuarterTurn5TilesNwSwPart3 = 16830,
    SprMonorailCyclesFlatQuarterTurn5TilesNwSwPart4 = 16831,
    SprMonorailCyclesFlatQuarterTurn5TilesNeNwPart0 = 16832,
    SprMonorailCyclesFlatQuarterTurn5TilesNeNwPart1 = 16833,
    SprMonorailCyclesFlatQuarterTurn5TilesNeNwPart2 = 16834,
    SprMonorailCyclesFlatQuarterTurn5TilesNeNwPart3 = 16835,
    SprMonorailCyclesFlatQuarterTurn5TilesNeNwPart4 = 16836,
    SprMonorailCyclesFlatQuarterTurn5TilesSeNePart0 = 16837,
    SprMonorailCyclesFlatQuarterTurn5TilesSeNePart1 = 16838,
    SprMonorailCyclesFlatQuarterTurn5TilesSeNePart2 = 16839,
    SprMonorailCyclesFlatQuarterTurn5TilesSeNePart3 = 16840,
    SprMonorailCyclesFlatQuarterTurn5TilesSeNePart4 = 16841,
    SprMonorailCyclesFlatQuarterTurn3TilesSwSePart0 = 16842,
    SprMonorailCyclesFlatQuarterTurn3TilesSwSePart1 = 16843,
    SprMonorailCyclesFlatQuarterTurn3TilesSwSePart2 = 16844,
    SprMonorailCyclesFlatQuarterTurn3TilesNwSwPart0 = 16845,
    SprMonorailCyclesFlatQuarterTurn3TilesNwSwPart1 = 16846,
    SprMonorailCyclesFlatQuarterTurn3TilesNwSwPart2 = 16847,
    SprMonorailCyclesFlatQuarterTurn3TilesNeNwPart0 = 16848,
    SprMonorailCyclesFlatQuarterTurn3TilesNeNwPart1 = 16849,
    SprMonorailCyclesFlatQuarterTurn3TilesNeNwPart2 = 16850,
    SprMonorailCyclesFlatQuarterTurn3TilesSeNePart0 = 16851,
    SprMonorailCyclesFlatQuarterTurn3TilesSeNePart1 = 16852,
    SprMonorailCyclesFlatQuarterTurn3TilesSeNePart2 = 16853,
    SprMonorailCyclesSBendLeftSwNePart0 = 16854,
    SprMonorailCyclesSBendLeftSwNePart1 = 16855,
    SprMonorailCyclesSBendLeftSwNePart2 = 16856,
    SprMonorailCyclesSBendLeftSwNePart3 = 16857,
    SprMonorailCyclesSBendLeftNwSePart0 = 16858,
    SprMonorailCyclesSBendLeftNwSePart1 = 16859,
    SprMonorailCyclesSBendLeftNwSePart2 = 16860,
    SprMonorailCyclesSBendLeftNwSePart3 = 16861,
    SprMonorailCyclesSBendRightSwNePart0 = 16862,
    SprMonorailCyclesSBendRightSwNePart1 = 16863,
    SprMonorailCyclesSBendRightSwNePart2 = 16864,
    SprMonorailCyclesSBendRightSwNePart3 = 16865,
    SprMonorailCyclesSBendRightNwSePart0 = 16866,
    SprMonorailCyclesSBendRightNwSePart1 = 16867,
    SprMonorailCyclesSBendRightNwSePart2 = 16868,
    SprMonorailCyclesSBendRightNwSePart3 = 16869,
};

static constexpr uint32_t kMonorailCyclesTrackPiecesFlat[2] = {
    SprMonorailCyclesFlatSwNe,
    SprMonorailCyclesFlatNwSe,
};

static constexpr uint32_t kMonorailCyclesTrackPiecesFlatQuarterTurn5Tiles[4][5] = {
    {
        SprMonorailCyclesFlatQuarterTurn5TilesSwSePart0,
        SprMonorailCyclesFlatQuarterTurn5TilesSwSePart1,
        SprMonorailCyclesFlatQuarterTurn5TilesSwSePart2,
        SprMonorailCyclesFlatQuarterTurn5TilesSwSePart3,
        SprMonorailCyclesFlatQuarterTurn5TilesSwSePart4,
    },
    {
        SprMonorailCyclesFlatQuarterTurn5TilesNwSwPart0,
        SprMonorailCyclesFlatQuarterTurn5TilesNwSwPart1,
        SprMonorailCyclesFlatQuarterTurn5TilesNwSwPart2,
        SprMonorailCyclesFlatQuarterTurn5TilesNwSwPart3,
        SprMonorailCyclesFlatQuarterTurn5TilesNwSwPart4,
    },
    {
        SprMonorailCyclesFlatQuarterTurn5TilesNeNwPart0,
        SprMonorailCyclesFlatQuarterTurn5TilesNeNwPart1,
        SprMonorailCyclesFlatQuarterTurn5TilesNeNwPart2,
        SprMonorailCyclesFlatQuarterTurn5TilesNeNwPart3,
        SprMonorailCyclesFlatQuarterTurn5TilesNeNwPart4,
    },
    {
        SprMonorailCyclesFlatQuarterTurn5TilesSeNePart0,
        SprMonorailCyclesFlatQuarterTurn5TilesSeNePart1,
        SprMonorailCyclesFlatQuarterTurn5TilesSeNePart2,
        SprMonorailCyclesFlatQuarterTurn5TilesSeNePart3,
        SprMonorailCyclesFlatQuarterTurn5TilesSeNePart4,
    },
};

static constexpr uint32_t kMonorailCyclesTrackPiecesSBendLeft[2][4] = {
    {
        SprMonorailCyclesSBendLeftSwNePart0,
        SprMonorailCyclesSBendLeftSwNePart1,
        SprMonorailCyclesSBendLeftSwNePart2,
        SprMonorailCyclesSBendLeftSwNePart3,
    },
    {
        SprMonorailCyclesSBendLeftNwSePart0,
        SprMonorailCyclesSBendLeftNwSePart1,
        SprMonorailCyclesSBendLeftNwSePart2,
        SprMonorailCyclesSBendLeftNwSePart3,
    },
};

static constexpr uint32_t kMonorailCyclesTrackPiecesSBendRight[2][4] = {
    {
        SprMonorailCyclesSBendRightSwNePart0,
        SprMonorailCyclesSBendRightSwNePart1,
        SprMonorailCyclesSBendRightSwNePart2,
        SprMonorailCyclesSBendRightSwNePart3,
    },
    {
        SprMonorailCyclesSBendRightNwSePart0,
        SprMonorailCyclesSBendRightNwSePart1,
        SprMonorailCyclesSBendRightNwSePart2,
        SprMonorailCyclesSBendRightNwSePart3,
    },
};

static constexpr uint32_t kMonorailCyclesTrackPiecesFlatQuarterTurn3Tiles[4][3] = {
    {
        SprMonorailCyclesFlatQuarterTurn3TilesSwSePart0,
        SprMonorailCyclesFlatQuarterTurn3TilesSwSePart1,
        SprMonorailCyclesFlatQuarterTurn3TilesSwSePart2,
    },
    {
        SprMonorailCyclesFlatQuarterTurn3TilesNwSwPart0,
        SprMonorailCyclesFlatQuarterTurn3TilesNwSwPart1,
        SprMonorailCyclesFlatQuarterTurn3TilesNwSwPart2,
    },
    {
        SprMonorailCyclesFlatQuarterTurn3TilesNeNwPart0,
        SprMonorailCyclesFlatQuarterTurn3TilesNeNwPart1,
        SprMonorailCyclesFlatQuarterTurn3TilesNeNwPart2,
    },
    {
        SprMonorailCyclesFlatQuarterTurn3TilesSeNePart0,
        SprMonorailCyclesFlatQuarterTurn3TilesSeNePart1,
        SprMonorailCyclesFlatQuarterTurn3TilesSeNePart2,
    },
};

/** rct2: 0x0088AD48 */
static void PaintMonorailCyclesTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    auto imageId = session.TrackColours.WithIndex(kMonorailCyclesTrackPiecesFlat[(direction & 1)]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });

    if (direction & 1)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    MetalASupportsPaintSetupRotated(
        session, supportType.metal, MetalSupportPlace::Centre, direction, -1, height, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide), direction),
        0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0088ADD8 */
static void PaintMonorailCyclesStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;

    if (direction == 0 || direction == 2)
    {
        imageId = GetStationColourScheme(session, trackElement).WithIndex(SPR_STATION_BASE_B_SW_NE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height - 2 }, { { 0, 2, height }, { 32, 28, 1 } });

        imageId = session.TrackColours.WithIndex(SprMonorailCyclesFlatSwNe);
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { { 0, 0, height }, { 32, 20, 1 } });
    }
    else if (direction == 1 || direction == 3)
    {
        imageId = GetStationColourScheme(session, trackElement).WithIndex(SPR_STATION_BASE_B_NW_SE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height - 2 }, { { 2, 0, height }, { 28, 32, 1 } });

        imageId = session.TrackColours.WithIndex(SprMonorailCyclesFlatNwSe);
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { { 0, 0, height }, { 20, 32, 1 } });
    }

    DrawSupportsSideBySide(session, direction, height, session.SupportColours, MetalSupportType::Boxed);
    TrackPaintUtilDrawStationTunnel(session, direction, height);

    TrackPaintUtilDrawStation(session, ride, direction, height, trackElement);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0088AD88 */
static void PaintMonorailCyclesTrackLeftQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    TrackPaintUtilLeftQuarterTurn3TilesPaint(
        session, 3, height, direction, trackSequence, session.TrackColours, kMonorailCyclesTrackPiecesFlatQuarterTurn3Tiles);
    TrackPaintUtilLeftQuarterTurn3TilesTunnel(session, kTunnelGroup, TunnelSubType::Flat, height, direction, trackSequence);

    switch (trackSequence)
    {
        case 0:
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, -1, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::topCorner),
                    direction),
                0xFFFF, 0);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topLeftSide, PaintSegment::centre, PaintSegment::bottomLeftSide,
                        PaintSegment::leftCorner),
                    direction),
                0xFFFF, 0);
            break;
        case 3:
            MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, -1, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topLeftSide, PaintSegment::centre, PaintSegment::bottomRightSide,
                        PaintSegment::bottomCorner),
                    direction),
                0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static constexpr uint8_t monorail_cycles_right_quarter_turn_3_tiles_to_left_turn_map[] = {
    3,
    1,
    2,
    0,
};

/** rct2: 0x0088AD98 */
static void PaintMonorailCyclesTrackRightQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = monorail_cycles_right_quarter_turn_3_tiles_to_left_turn_map[trackSequence];
    PaintMonorailCyclesTrackLeftQuarterTurn3Tiles(
        session, ride, trackSequence, (direction + 3) % 4, height, trackElement, supportType);
}

static constexpr int8_t kMonorailCyclesTrackRightQuarterTurn5TilesSupportHeightOffset[][7] = {
    { -2, 0, -2, 0, 0, -3, -1 },
    { -3, 0, 0, 0, 0, 0, 0 },
    { 0 },
    { 0, 0, 0, 0, 0, -2, -3 },
};

static constexpr int8_t kMonorailCyclesTrackRightQuarterTurn5TilesSupportSpecial[][7] = {
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 1 },
    { 0, 0, 1, 0, 0, 1, 1 },
    { 1, 0, 0, 0, 0, 0, 0 },
};

/** rct2: 0x0088ADB8 */
static void PaintMonorailCyclesTrackRightQuarterTurn5Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    TrackPaintUtilRightQuarterTurn5TilesPaint(
        session, 1, height, direction, trackSequence, session.TrackColours, kMonorailCyclesTrackPiecesFlatQuarterTurn5Tiles,
        nullptr, kDefaultRightQuarterTurn5TilesBoundLengths, kDefaultRightQuarterTurn5TilesBoundOffsets);

    int32_t supportHeight = height + kMonorailCyclesTrackRightQuarterTurn5TilesSupportHeightOffset[direction][trackSequence];
    int32_t supportSpecial = kMonorailCyclesTrackRightQuarterTurn5TilesSupportSpecial[direction][trackSequence];
    switch (trackSequence)
    {
        case 0:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::Centre, direction, supportSpecial, supportHeight,
                session.SupportColours);
            break;
        case 2:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::BottomRightSide, direction, supportSpecial, supportHeight,
                session.SupportColours);
            break;
        case 5:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::BottomRightSide, DirectionNext(direction), supportSpecial,
                supportHeight, session.SupportColours);
            break;
        case 6:
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::Centre, DirectionNext(direction), supportSpecial, supportHeight,
                session.SupportColours);
            break;
    }

    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (direction == 0 && trackSequence == 6)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (direction == 1 && trackSequence == 6)
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::rightCorner),
                    direction),
                0xFFFF, 0);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::bottomRightSide,
                        PaintSegment::rightCorner, PaintSegment::bottomCorner, PaintSegment::topRightSide),
                    direction),
                0xFFFF, 0);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::topLeftSide, PaintSegment::topRightSide, PaintSegment::centre),
                    direction),
                0xFFFF, 0);
            break;
        case 5:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomRightSide, PaintSegment::centre, PaintSegment::bottomLeftSide,
                        PaintSegment::leftCorner, PaintSegment::bottomCorner, PaintSegment::topLeftSide),
                    direction),
                0xFFFF, 0);
            break;
        case 6:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomRightSide, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::leftCorner),
                    direction),
                0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0088ADA8 */
static void PaintMonorailCyclesTrackLeftQuarterTurn5Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    PaintMonorailCyclesTrackRightQuarterTurn5Tiles(
        session, ride, trackSequence, (direction + 1) % 4, height, trackElement, supportType);
}

/** rct2: 0x0088ADC8 */
static void PaintMonorailCyclesTrackSBendLeft(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    uint8_t originalTrackSequence = trackSequence;
    if (direction == 2 || direction == 3)
    {
        trackSequence = 3 - trackSequence;
    }

    auto imageId = session.TrackColours.WithIndex(kMonorailCyclesTrackPiecesSBendLeft[direction & 1][trackSequence]);
    switch (trackSequence)
    {
        case 0:
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 0, height }, { 32, 26, 1 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 26, 1 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
            break;
    }

    if (direction == 0 || direction == 2)
    {
        if (trackSequence == 0)
        {
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
        }
    }
    else
    {
        if (trackSequence == 3)
        {
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
        }
    }

    DrawSBendLeftSupports(session, supportType.metal, originalTrackSequence, direction, height, 0, 0);

    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::topCorner),
                    direction & 1),
                0xFFFF, 0);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::leftCorner, PaintSegment::topLeftSide, PaintSegment::topCorner),
                    direction & 1),
                0xFFFF, 0);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomCorner, PaintSegment::bottomRightSide, PaintSegment::rightCorner),
                    direction & 1),
                0xFFFF, 0);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomCorner),
                    direction & 1),
                0xFFFF, 0);
            break;
    }
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x*/
static void PaintMonorailCyclesTrackSBendRight(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    uint8_t originalTrackSequence = trackSequence;
    if (direction == 2 || direction == 3)
    {
        trackSequence = 3 - trackSequence;
    }

    auto imageId = session.TrackColours.WithIndex(kMonorailCyclesTrackPiecesSBendRight[direction & 1][trackSequence]);
    switch (trackSequence)
    {
        case 0:
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 26, 1 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 0, height }, { 32, 26, 1 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
            break;
    }

    if (direction == 0 || direction == 2)
    {
        if (trackSequence == 0)
        {
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
        }
    }
    else
    {
        if (trackSequence == 3)
        {
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
        }
    }
    DrawSBendRightSupports(
        session, supportType.metal, originalTrackSequence, direction, trackSequence == 1 ? height - 2 : height, 0, 0);

    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::rightCorner),
                    direction & 1),
                0xFFFF, 0);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomCorner, PaintSegment::bottomRightSide, PaintSegment::rightCorner),
                    direction & 1),
                0xFFFF, 0);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::leftCorner, PaintSegment::topLeftSide, PaintSegment::topCorner),
                    direction & 1),
                0xFFFF, 0);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::leftCorner),
                    direction & 1),
                0xFFFF, 0);
            break;
    }
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/**
 * rct2: 0x0088ac88
 */
TrackPaintFunction GetTrackPaintFunctionMonorailCycles(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return PaintMonorailCyclesTrackFlat;

        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return PaintMonorailCyclesStation;

        case TrackElemType::LeftQuarterTurn5Tiles:
            return PaintMonorailCyclesTrackLeftQuarterTurn5Tiles;
        case TrackElemType::RightQuarterTurn5Tiles:
            return PaintMonorailCyclesTrackRightQuarterTurn5Tiles;

        case TrackElemType::SBendLeft:
            return PaintMonorailCyclesTrackSBendLeft;
        case TrackElemType::SBendRight:
            return PaintMonorailCyclesTrackSBendRight;

        case TrackElemType::LeftQuarterTurn3Tiles:
            return PaintMonorailCyclesTrackLeftQuarterTurn3Tiles;
        case TrackElemType::RightQuarterTurn3Tiles:
            return PaintMonorailCyclesTrackRightQuarterTurn3Tiles;
        default:
            return TrackPaintFunctionDummy;
    }
}
