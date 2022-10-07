/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
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
#include "../Track.h"
#include "../TrackPaint.h"
#include "../VehiclePaint.h"

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

static constexpr const uint32_t MonorailCyclesTrackPiecesFlat[2] = {
    SprMonorailCyclesFlatSwNe,
    SprMonorailCyclesFlatNwSe,
};

static constexpr const uint32_t MonorailCyclesTrackPiecesFlatQuarterTurn5Tiles[4][5] = {
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

static constexpr const uint32_t MonorailCyclesTrackPiecesSBendLeft[2][4] = {
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

static constexpr const uint32_t MonorailCyclesTrackPiecesSBendRight[2][4] = {
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

static constexpr const uint32_t MonorailCyclesTrackPiecesFlatQuarterTurn3Tiles[4][3] = {
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

static paint_struct* PaintMonorailCyclesUtil7C(
    paint_session& session, bool flip, ImageId imageId, int8_t xOffset, int8_t yOffset, int16_t boundBoxLengthX,
    int16_t boundBoxLengthY, int8_t boundBoxLengthZ, int16_t zOffset, int16_t boundBoxOffsetX, int16_t boundBoxOffsetY,
    int16_t boundBoxOffsetZ, uint32_t rotation)
{
    if (flip)
    {
        return PaintAddImageAsParent(
            session, imageId, { yOffset, xOffset, zOffset }, { boundBoxLengthY, boundBoxLengthX, boundBoxLengthZ },
            { boundBoxOffsetY, boundBoxOffsetX, boundBoxOffsetZ });
    }

    return PaintAddImageAsParent(
        session, imageId, { xOffset, yOffset, zOffset }, { boundBoxLengthX, boundBoxLengthY, boundBoxLengthZ },
        { boundBoxOffsetX, boundBoxOffsetY, boundBoxOffsetZ });
}

/** rct2: 0x0088AD48 */
static void PaintMonorailCyclesTrackFlat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(MonorailCyclesTrackPiecesFlat[(direction & 1)]);
    PaintMonorailCyclesUtil7C(
        session, static_cast<bool>(direction & 1), imageId, 0, 0, 32, 20, 3, height, 0, 6, height, session.CurrentRotation);

    if (direction & 1)
    {
        PaintUtilPushTunnelRight(session, height, TUNNEL_0);
    }
    else
    {
        PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
    }

    metal_a_supports_paint_setup(
        session, (direction & 1) ? METAL_SUPPORTS_STICK_ALT : METAL_SUPPORTS_STICK, 4, -1, height,
        session.TrackColours[SCHEME_SUPPORTS]);

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x0088ADD8 */
static void PaintMonorailCyclesStation(
    paint_session& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    ImageId imageId;

    if (direction == 0 || direction == 2)
    {
        imageId = session.TrackColours[SCHEME_MISC].WithIndex(SPR_STATION_BASE_B_SW_NE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height - 2 }, { 32, 28, 1 }, { 0, 2, height });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SprMonorailCyclesFlatSwNe);
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 32, 20, 1 }, { 0, 0, height });

        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
    }
    else if (direction == 1 || direction == 3)
    {
        imageId = session.TrackColours[SCHEME_MISC].WithIndex(SPR_STATION_BASE_B_NW_SE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height - 2 }, { 28, 32, 1 }, { 2, 0, height });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SprMonorailCyclesFlatNwSe);
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 20, 32, 1 }, { 0, 0, height });

        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
    }

    track_paint_util_draw_station(session, ride, direction, height, trackElement);

    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x0088AD88 */
static void PaintMonorailCyclesTrackLeftQuarterTurn3Tiles(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    track_paint_util_left_quarter_turn_3_tiles_paint(
        session, 3, height, direction, trackSequence, session.TrackColours[SCHEME_TRACK],
        MonorailCyclesTrackPiecesFlatQuarterTurn3Tiles);
    track_paint_util_left_quarter_turn_3_tiles_tunnel(session, height, TUNNEL_0, direction, trackSequence);

    switch (trackSequence)
    {
        case 0:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, -1, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_B4, direction), 0xFFFF, 0);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_B8, direction), 0xFFFF, 0);
            break;
        case 3:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, -1, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_C0, direction), 0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

static constexpr const uint8_t monorail_cycles_right_quarter_turn_3_tiles_to_left_turn_map[] = {
    3,
    1,
    2,
    0,
};

/** rct2: 0x0088AD98 */
static void PaintMonorailCyclesTrackRightQuarterTurn3Tiles(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = monorail_cycles_right_quarter_turn_3_tiles_to_left_turn_map[trackSequence];
    PaintMonorailCyclesTrackLeftQuarterTurn3Tiles(session, ride, trackSequence, (direction + 3) % 4, height, trackElement);
}

static constexpr const int8_t MonorailCyclesTrackRightQuarterTurn5TilesSupportHeightOffset[][7] = {
    { -2, 0, -2, 0, 0, -3, -1 },
    { -3, 0, 0, 0, 0, 0, 0 },
    { 0 },
    { 0, 0, 0, 0, 0, -2, -3 },
};

static constexpr const int8_t MonorailCyclesTrackRightQuarterTurn5TilesSupportSpecial[][7] = {
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 1 },
    { 0, 0, 1, 0, 0, 1, 1 },
    { 1, 0, 0, 0, 0, 0, 0 },
};

/** rct2: 0x0088ADB8 */
static void PaintMonorailCyclesTrackRightQuarterTurn5Tiles(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    track_paint_util_right_quarter_turn_5_tiles_paint(
        session, 1, height, direction, trackSequence, session.TrackColours[SCHEME_TRACK],
        MonorailCyclesTrackPiecesFlatQuarterTurn5Tiles, nullptr, defaultRightQuarterTurn5TilesBoundLengths,
        defaultRightQuarterTurn5TilesBoundOffsets);

    int32_t supportHeight = height + MonorailCyclesTrackRightQuarterTurn5TilesSupportHeightOffset[direction][trackSequence];
    int32_t supportSpecial = MonorailCyclesTrackRightQuarterTurn5TilesSupportSpecial[direction][trackSequence];
    switch (trackSequence)
    {
        case 0:
            metal_a_supports_paint_setup(
                session, (direction & 1) ? METAL_SUPPORTS_STICK_ALT : METAL_SUPPORTS_STICK, 4, supportSpecial, supportHeight,
                session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            if (direction == 0)
                metal_a_supports_paint_setup(
                    session, (direction & 1) ? METAL_SUPPORTS_STICK_ALT : METAL_SUPPORTS_STICK, 8, supportSpecial,
                    supportHeight, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 1)
                metal_a_supports_paint_setup(
                    session, (direction & 1) ? METAL_SUPPORTS_STICK_ALT : METAL_SUPPORTS_STICK, 7, supportSpecial,
                    supportHeight, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 2)
                metal_a_supports_paint_setup(
                    session, (direction & 1) ? METAL_SUPPORTS_STICK_ALT : METAL_SUPPORTS_STICK, 5, supportSpecial,
                    supportHeight, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 3)
                metal_a_supports_paint_setup(
                    session, (direction & 1) ? METAL_SUPPORTS_STICK_ALT : METAL_SUPPORTS_STICK, 6, supportSpecial,
                    supportHeight, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 5:
            if (direction == 0)
                metal_a_supports_paint_setup(
                    session, (direction & 1) ? METAL_SUPPORTS_STICK : METAL_SUPPORTS_STICK_ALT, 7, supportSpecial,
                    supportHeight, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 1)
                metal_a_supports_paint_setup(
                    session, (direction & 1) ? METAL_SUPPORTS_STICK : METAL_SUPPORTS_STICK_ALT, 5, supportSpecial,
                    supportHeight, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 2)
                metal_a_supports_paint_setup(
                    session, (direction & 1) ? METAL_SUPPORTS_STICK : METAL_SUPPORTS_STICK_ALT, 6, supportSpecial,
                    supportHeight, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 3)
                metal_a_supports_paint_setup(
                    session, (direction & 1) ? METAL_SUPPORTS_STICK : METAL_SUPPORTS_STICK_ALT, 8, supportSpecial,
                    supportHeight, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 6:
            metal_a_supports_paint_setup(
                session, (direction & 1) ? METAL_SUPPORTS_STICK : METAL_SUPPORTS_STICK_ALT, 4, supportSpecial, supportHeight,
                session.TrackColours[SCHEME_SUPPORTS]);
            break;
    }

    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
    }

    if (direction == 0 && trackSequence == 6)
    {
        PaintUtilPushTunnelRight(session, height, TUNNEL_0);
    }

    if (direction == 1 && trackSequence == 6)
    {
        PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
    }

    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height, TUNNEL_0);
    }

    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_BC, direction), 0xFFFF, 0);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_CC, direction),
                0xFFFF, 0);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_C4, direction), 0xFFFF, 0);
            break;
        case 5:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_D4 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C8, direction),
                0xFFFF, 0);
            break;
        case 6:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_D4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_B8, direction), 0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x0088ADA8 */
static void PaintMonorailCyclesTrackLeftQuarterTurn5Tiles(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    PaintMonorailCyclesTrackRightQuarterTurn5Tiles(session, ride, trackSequence, (direction + 1) % 4, height, trackElement);
}

/** rct2: 0x0088ADC8 */
static void PaintMonorailCyclesTrackSBendLeft(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (direction == 2 || direction == 3)
    {
        trackSequence = 3 - trackSequence;
    }

    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
        MonorailCyclesTrackPiecesSBendLeft[direction & 1][trackSequence]);
    switch (trackSequence)
    {
        case 0:
            PaintMonorailCyclesUtil7C(
                session, direction & 1, imageId, 0, 0, 32, 20, 1, height, 0, 6, height, session.CurrentRotation);
            break;
        case 1:
            PaintMonorailCyclesUtil7C(
                session, direction & 1, imageId, 0, 0, 32, 26, 1, height, 0, 0, height, session.CurrentRotation);
            break;
        case 2:
            PaintMonorailCyclesUtil7C(
                session, direction & 1, imageId, 0, 0, 32, 26, 1, height, 0, 6, height, session.CurrentRotation);
            break;
        case 3:
            PaintMonorailCyclesUtil7C(
                session, direction & 1, imageId, 0, 0, 32, 20, 1, height, 0, 6, height, session.CurrentRotation);
            break;
    }

    if (direction == 0 || direction == 2)
    {
        if (trackSequence == 0)
        {
            PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
        }

        switch (trackSequence)
        {
            case 0:
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }
    else
    {
        if (trackSequence == 3)
        {
            PaintUtilPushTunnelRight(session, height, TUNNEL_0);
        }

        switch (trackSequence)
        {
            case 0:
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK_ALT, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK_ALT, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK_ALT, 4, 0, height - 2, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_B4, direction & 1), 0xFFFF, 0);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_B8 | SEGMENT_C8 | SEGMENT_B4, direction & 1),
                0xFFFF, 0);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_C0 | SEGMENT_D4 | SEGMENT_BC, direction & 1),
                0xFFFF, 0);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_C0, direction & 1), 0xFFFF, 0);
            break;
    }
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x*/
static void PaintMonorailCyclesTrackSBendRight(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (direction == 2 || direction == 3)
    {
        trackSequence = 3 - trackSequence;
    }

    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
        MonorailCyclesTrackPiecesSBendRight[direction & 1][trackSequence]);
    switch (trackSequence)
    {
        case 0:
            PaintMonorailCyclesUtil7C(
                session, direction & 1, imageId, 0, 0, 32, 20, 1, height, 0, 6, height, session.CurrentRotation);
            break;
        case 1:
            PaintMonorailCyclesUtil7C(
                session, direction & 1, imageId, 0, 0, 32, 26, 1, height, 0, 6, height, session.CurrentRotation);
            break;
        case 2:
            PaintMonorailCyclesUtil7C(
                session, direction & 1, imageId, 0, 0, 32, 26, 1, height, 0, 0, height, session.CurrentRotation);
            break;
        case 3:
            PaintMonorailCyclesUtil7C(
                session, direction & 1, imageId, 0, 0, 32, 20, 1, height, 0, 6, height, session.CurrentRotation);
            break;
    }

    if (direction == 0 || direction == 2)
    {
        if (trackSequence == 0)
        {
            PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
        }

        switch (trackSequence)
        {
            case 0:
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK, 8, 0, height - 2, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }
    else
    {
        if (trackSequence == 3)
        {
            PaintUtilPushTunnelRight(session, height, TUNNEL_0);
        }

        switch (trackSequence)
        {
            case 0:
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK_ALT, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK_ALT, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK_ALT, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    switch (trackSequence)
    {
        case 0:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_BC, direction & 1), 0xFFFF, 0);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_C0 | SEGMENT_D4 | SEGMENT_BC, direction & 1),
                0xFFFF, 0);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_B8 | SEGMENT_C8 | SEGMENT_B4, direction & 1),
                0xFFFF, 0);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_B8, direction & 1), 0xFFFF, 0);
            break;
    }
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/**
 * rct2: 0x0088ac88
 */
TRACK_PAINT_FUNCTION GetTrackPaintFunctionMonorailCycles(int32_t trackType)
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
    }

    return nullptr;
}
