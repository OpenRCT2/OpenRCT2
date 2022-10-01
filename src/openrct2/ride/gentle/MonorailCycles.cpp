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
    SPR_MONORAIL_CYCLES_FLAT_SW_NE = 16820,
    SPR_MONORAIL_CYCLES_FLAT_NW_SE = 16821,
    SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_SW_SE_PART_0 = 16822,
    SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_SW_SE_PART_1 = 16823,
    SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_SW_SE_PART_2 = 16824,
    SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_SW_SE_PART_3 = 16825,
    SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_SW_SE_PART_4 = 16826,
    SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_NW_SW_PART_0 = 16827,
    SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_NW_SW_PART_1 = 16828,
    SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_NW_SW_PART_2 = 16829,
    SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_NW_SW_PART_3 = 16830,
    SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_NW_SW_PART_4 = 16831,
    SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_NE_NW_PART_0 = 16832,
    SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_NE_NW_PART_1 = 16833,
    SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_NE_NW_PART_2 = 16834,
    SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_NE_NW_PART_3 = 16835,
    SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_NE_NW_PART_4 = 16836,
    SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_SE_NE_PART_0 = 16837,
    SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_SE_NE_PART_1 = 16838,
    SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_SE_NE_PART_2 = 16839,
    SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_SE_NE_PART_3 = 16840,
    SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_SE_NE_PART_4 = 16841,
    SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_0 = 16842,
    SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_1 = 16843,
    SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_2 = 16844,
    SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_0 = 16845,
    SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_1 = 16846,
    SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_2 = 16847,
    SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_0 = 16848,
    SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_1 = 16849,
    SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_2 = 16850,
    SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_0 = 16851,
    SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_1 = 16852,
    SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_2 = 16853,
    SPR_MONORAIL_CYCLES_S_BEND_LEFT_SW_NE_PART_0 = 16854,
    SPR_MONORAIL_CYCLES_S_BEND_LEFT_SW_NE_PART_1 = 16855,
    SPR_MONORAIL_CYCLES_S_BEND_LEFT_SW_NE_PART_2 = 16856,
    SPR_MONORAIL_CYCLES_S_BEND_LEFT_SW_NE_PART_3 = 16857,
    SPR_MONORAIL_CYCLES_S_BEND_LEFT_NW_SE_PART_0 = 16858,
    SPR_MONORAIL_CYCLES_S_BEND_LEFT_NW_SE_PART_1 = 16859,
    SPR_MONORAIL_CYCLES_S_BEND_LEFT_NW_SE_PART_2 = 16860,
    SPR_MONORAIL_CYCLES_S_BEND_LEFT_NW_SE_PART_3 = 16861,
    SPR_MONORAIL_CYCLES_S_BEND_RIGHT_SW_NE_PART_0 = 16862,
    SPR_MONORAIL_CYCLES_S_BEND_RIGHT_SW_NE_PART_1 = 16863,
    SPR_MONORAIL_CYCLES_S_BEND_RIGHT_SW_NE_PART_2 = 16864,
    SPR_MONORAIL_CYCLES_S_BEND_RIGHT_SW_NE_PART_3 = 16865,
    SPR_MONORAIL_CYCLES_S_BEND_RIGHT_NW_SE_PART_0 = 16866,
    SPR_MONORAIL_CYCLES_S_BEND_RIGHT_NW_SE_PART_1 = 16867,
    SPR_MONORAIL_CYCLES_S_BEND_RIGHT_NW_SE_PART_2 = 16868,
    SPR_MONORAIL_CYCLES_S_BEND_RIGHT_NW_SE_PART_3 = 16869,
};

static constexpr const uint32_t monorail_cycles_track_pieces_flat[2] = {
    SPR_MONORAIL_CYCLES_FLAT_SW_NE,
    SPR_MONORAIL_CYCLES_FLAT_NW_SE,
};

static constexpr const uint32_t monorail_cycles_track_pieces_flat_quarter_turn_5_tiles[4][5] = {
    {
        SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_SW_SE_PART_0,
        SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_SW_SE_PART_1,
        SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_SW_SE_PART_2,
        SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_SW_SE_PART_3,
        SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_SW_SE_PART_4,
    },
    {
        SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_NW_SW_PART_0,
        SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_NW_SW_PART_1,
        SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_NW_SW_PART_2,
        SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_NW_SW_PART_3,
        SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_NW_SW_PART_4,
    },
    {
        SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_NE_NW_PART_0,
        SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_NE_NW_PART_1,
        SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_NE_NW_PART_2,
        SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_NE_NW_PART_3,
        SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_NE_NW_PART_4,
    },
    {
        SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_SE_NE_PART_0,
        SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_SE_NE_PART_1,
        SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_SE_NE_PART_2,
        SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_SE_NE_PART_3,
        SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_5_TILES_SE_NE_PART_4,
    },
};

static constexpr const uint32_t monorail_cycles_track_pieces_s_bend_left[2][4] = {
    {
        SPR_MONORAIL_CYCLES_S_BEND_LEFT_SW_NE_PART_0,
        SPR_MONORAIL_CYCLES_S_BEND_LEFT_SW_NE_PART_1,
        SPR_MONORAIL_CYCLES_S_BEND_LEFT_SW_NE_PART_2,
        SPR_MONORAIL_CYCLES_S_BEND_LEFT_SW_NE_PART_3,
    },
    {
        SPR_MONORAIL_CYCLES_S_BEND_LEFT_NW_SE_PART_0,
        SPR_MONORAIL_CYCLES_S_BEND_LEFT_NW_SE_PART_1,
        SPR_MONORAIL_CYCLES_S_BEND_LEFT_NW_SE_PART_2,
        SPR_MONORAIL_CYCLES_S_BEND_LEFT_NW_SE_PART_3,
    },
};

static constexpr const uint32_t monorail_cycles_track_pieces_s_bend_right[2][4] = {
    {
        SPR_MONORAIL_CYCLES_S_BEND_RIGHT_SW_NE_PART_0,
        SPR_MONORAIL_CYCLES_S_BEND_RIGHT_SW_NE_PART_1,
        SPR_MONORAIL_CYCLES_S_BEND_RIGHT_SW_NE_PART_2,
        SPR_MONORAIL_CYCLES_S_BEND_RIGHT_SW_NE_PART_3,
    },
    {
        SPR_MONORAIL_CYCLES_S_BEND_RIGHT_NW_SE_PART_0,
        SPR_MONORAIL_CYCLES_S_BEND_RIGHT_NW_SE_PART_1,
        SPR_MONORAIL_CYCLES_S_BEND_RIGHT_NW_SE_PART_2,
        SPR_MONORAIL_CYCLES_S_BEND_RIGHT_NW_SE_PART_3,
    },
};

static constexpr const uint32_t monorail_cycles_track_pieces_flat_quarter_turn_3_tiles[4][3] = {
    {
        SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_0,
        SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_1,
        SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_2,
    },
    {
        SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_0,
        SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_1,
        SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_2,
    },
    {
        SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_0,
        SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_1,
        SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_2,
    },
    {
        SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_0,
        SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_1,
        SPR_MONORAIL_CYCLES_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_2,
    },
};

static paint_struct* paint_monorail_cycles_util_7c(
    paint_session& session, bool flip, ImageId image_id, int8_t x_offset, int8_t y_offset, int16_t bound_box_length_x,
    int16_t bound_box_length_y, int8_t bound_box_length_z, int16_t z_offset, int16_t bound_box_offset_x,
    int16_t bound_box_offset_y, int16_t bound_box_offset_z, uint32_t rotation)
{
    if (flip)
    {
        return PaintAddImageAsParent(
            session, image_id, { y_offset, x_offset, z_offset }, { bound_box_length_y, bound_box_length_x, bound_box_length_z },
            { bound_box_offset_y, bound_box_offset_x, bound_box_offset_z });
    }

    return PaintAddImageAsParent(
        session, image_id, { x_offset, y_offset, z_offset }, { bound_box_length_x, bound_box_length_y, bound_box_length_z },
        { bound_box_offset_x, bound_box_offset_y, bound_box_offset_z });
}

/** rct2: 0x0088AD48 */
static void paint_monorail_cycles_track_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(monorail_cycles_track_pieces_flat[(direction & 1)]);
    paint_monorail_cycles_util_7c(
        session, static_cast<bool>(direction & 1), imageId, 0, 0, 32, 20, 3, height, 0, 6, height, session.CurrentRotation);

    if (direction & 1)
    {
        paint_util_push_tunnel_right(session, height, TUNNEL_0);
    }
    else
    {
        paint_util_push_tunnel_left(session, height, TUNNEL_0);
    }

    metal_a_supports_paint_setup(
        session, (direction & 1) ? METAL_SUPPORTS_STICK_ALT : METAL_SUPPORTS_STICK, 4, -1, height,
        session.TrackColours[SCHEME_SUPPORTS]);

    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0088ADD8 */
static void paint_monorail_cycles_station(
    paint_session& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    ImageId imageId;

    if (direction == 0 || direction == 2)
    {
        imageId = session.TrackColours[SCHEME_MISC].WithIndex(SPR_STATION_BASE_B_SW_NE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height - 2 }, { 32, 28, 1 }, { 0, 2, height });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_MONORAIL_CYCLES_FLAT_SW_NE);
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 32, 20, 1 }, { 0, 0, height });

        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
    }
    else if (direction == 1 || direction == 3)
    {
        imageId = session.TrackColours[SCHEME_MISC].WithIndex(SPR_STATION_BASE_B_NW_SE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height - 2 }, { 28, 32, 1 }, { 2, 0, height });

        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_MONORAIL_CYCLES_FLAT_NW_SE);
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 20, 32, 1 }, { 0, 0, height });

        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
    }

    track_paint_util_draw_station(session, ride, direction, height, trackElement);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0088AD88 */
static void paint_monorail_cycles_track_left_quarter_turn_3_tiles(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    track_paint_util_left_quarter_turn_3_tiles_paint(
        session, 3, height, direction, trackSequence, session.TrackColours[SCHEME_TRACK],
        monorail_cycles_track_pieces_flat_quarter_turn_3_tiles);
    track_paint_util_left_quarter_turn_3_tiles_tunnel(session, height, TUNNEL_0, direction, trackSequence);

    switch (trackSequence)
    {
        case 0:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, -1, height, session.TrackColours[SCHEME_SUPPORTS]);
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_B4, direction), 0xFFFF, 0);
            break;
        case 2:
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_B8, direction), 0xFFFF, 0);
            break;
        case 3:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, -1, height, session.TrackColours[SCHEME_SUPPORTS]);
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_C0, direction), 0xFFFF, 0);
            break;
    }

    paint_util_set_general_support_height(session, height + 32, 0x20);
}

static constexpr const uint8_t monorail_cycles_right_quarter_turn_3_tiles_to_left_turn_map[] = {
    3,
    1,
    2,
    0,
};

/** rct2: 0x0088AD98 */
static void paint_monorail_cycles_track_right_quarter_turn_3_tiles(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = monorail_cycles_right_quarter_turn_3_tiles_to_left_turn_map[trackSequence];
    paint_monorail_cycles_track_left_quarter_turn_3_tiles(
        session, ride, trackSequence, (direction + 3) % 4, height, trackElement);
}

static constexpr const int8_t monorail_cycles_track_right_quarter_turn_5_tiles_support_height_offset[][7] = {
    { -2, 0, -2, 0, 0, -3, -1 },
    { -3, 0, 0, 0, 0, 0, 0 },
    { 0 },
    { 0, 0, 0, 0, 0, -2, -3 },
};

static constexpr const int8_t monorail_cycles_track_right_quarter_turn_5_tiles_support_special[][7] = {
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 1 },
    { 0, 0, 1, 0, 0, 1, 1 },
    { 1, 0, 0, 0, 0, 0, 0 },
};

/** rct2: 0x0088ADB8 */
static void paint_monorail_cycles_track_right_quarter_turn_5_tiles(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    track_paint_util_right_quarter_turn_5_tiles_paint(
        session, 1, height, direction, trackSequence, session.TrackColours[SCHEME_TRACK],
        monorail_cycles_track_pieces_flat_quarter_turn_5_tiles, nullptr, defaultRightQuarterTurn5TilesBoundLengths,
        defaultRightQuarterTurn5TilesBoundOffsets);

    int32_t supportHeight = height
        + monorail_cycles_track_right_quarter_turn_5_tiles_support_height_offset[direction][trackSequence];
    int32_t supportSpecial = monorail_cycles_track_right_quarter_turn_5_tiles_support_special[direction][trackSequence];
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
        paint_util_push_tunnel_left(session, height, TUNNEL_0);
    }

    if (direction == 0 && trackSequence == 6)
    {
        paint_util_push_tunnel_right(session, height, TUNNEL_0);
    }

    if (direction == 1 && trackSequence == 6)
    {
        paint_util_push_tunnel_left(session, height, TUNNEL_0);
    }

    if (direction == 3 && trackSequence == 0)
    {
        paint_util_push_tunnel_right(session, height, TUNNEL_0);
    }

    switch (trackSequence)
    {
        case 0:
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_BC, direction), 0xFFFF, 0);
            break;
        case 2:
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_D0 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_CC, direction),
                0xFFFF, 0);
            break;
        case 3:
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_C4, direction), 0xFFFF, 0);
            break;
        case 5:
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_D4 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C8, direction),
                0xFFFF, 0);
            break;
        case 6:
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_D4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_B8, direction), 0xFFFF, 0);
            break;
    }

    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0088ADA8 */
static void paint_monorail_cycles_track_left_quarter_turn_5_tiles(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    paint_monorail_cycles_track_right_quarter_turn_5_tiles(
        session, ride, trackSequence, (direction + 1) % 4, height, trackElement);
}

/** rct2: 0x0088ADC8 */
static void paint_monorail_cycles_track_s_bend_left(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (direction == 2 || direction == 3)
    {
        trackSequence = 3 - trackSequence;
    }

    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
        monorail_cycles_track_pieces_s_bend_left[direction & 1][trackSequence]);
    switch (trackSequence)
    {
        case 0:
            paint_monorail_cycles_util_7c(
                session, direction & 1, imageId, 0, 0, 32, 20, 1, height, 0, 6, height, session.CurrentRotation);
            break;
        case 1:
            paint_monorail_cycles_util_7c(
                session, direction & 1, imageId, 0, 0, 32, 26, 1, height, 0, 0, height, session.CurrentRotation);
            break;
        case 2:
            paint_monorail_cycles_util_7c(
                session, direction & 1, imageId, 0, 0, 32, 26, 1, height, 0, 6, height, session.CurrentRotation);
            break;
        case 3:
            paint_monorail_cycles_util_7c(
                session, direction & 1, imageId, 0, 0, 32, 20, 1, height, 0, 6, height, session.CurrentRotation);
            break;
    }

    if (direction == 0 || direction == 2)
    {
        if (trackSequence == 0)
        {
            paint_util_push_tunnel_left(session, height, TUNNEL_0);
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
            paint_util_push_tunnel_right(session, height, TUNNEL_0);
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
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_B4, direction & 1), 0xFFFF,
                0);
            break;
        case 1:
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_B8 | SEGMENT_C8 | SEGMENT_B4, direction & 1),
                0xFFFF, 0);
            break;
        case 2:
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_C0 | SEGMENT_D4 | SEGMENT_BC, direction & 1),
                0xFFFF, 0);
            break;
        case 3:
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_C0, direction & 1), 0xFFFF,
                0);
            break;
    }
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x*/
static void paint_monorail_cycles_track_s_bend_right(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (direction == 2 || direction == 3)
    {
        trackSequence = 3 - trackSequence;
    }

    auto imageId = session.TrackColours[SCHEME_TRACK].WithIndex(
        monorail_cycles_track_pieces_s_bend_right[direction & 1][trackSequence]);
    switch (trackSequence)
    {
        case 0:
            paint_monorail_cycles_util_7c(
                session, direction & 1, imageId, 0, 0, 32, 20, 1, height, 0, 6, height, session.CurrentRotation);
            break;
        case 1:
            paint_monorail_cycles_util_7c(
                session, direction & 1, imageId, 0, 0, 32, 26, 1, height, 0, 6, height, session.CurrentRotation);
            break;
        case 2:
            paint_monorail_cycles_util_7c(
                session, direction & 1, imageId, 0, 0, 32, 26, 1, height, 0, 0, height, session.CurrentRotation);
            break;
        case 3:
            paint_monorail_cycles_util_7c(
                session, direction & 1, imageId, 0, 0, 32, 20, 1, height, 0, 6, height, session.CurrentRotation);
            break;
    }

    if (direction == 0 || direction == 2)
    {
        if (trackSequence == 0)
        {
            paint_util_push_tunnel_left(session, height, TUNNEL_0);
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
            paint_util_push_tunnel_right(session, height, TUNNEL_0);
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
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_BC, direction & 1), 0xFFFF,
                0);
            break;
        case 1:
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_C0 | SEGMENT_D4 | SEGMENT_BC, direction & 1),
                0xFFFF, 0);
            break;
        case 2:
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_B8 | SEGMENT_C8 | SEGMENT_B4, direction & 1),
                0xFFFF, 0);
            break;
        case 3:
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_B8, direction & 1), 0xFFFF,
                0);
            break;
    }
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/**
 * rct2: 0x0088ac88
 */
TRACK_PAINT_FUNCTION get_track_paint_function_monorail_cycles(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return paint_monorail_cycles_track_flat;

        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return paint_monorail_cycles_station;

        case TrackElemType::LeftQuarterTurn5Tiles:
            return paint_monorail_cycles_track_left_quarter_turn_5_tiles;
        case TrackElemType::RightQuarterTurn5Tiles:
            return paint_monorail_cycles_track_right_quarter_turn_5_tiles;

        case TrackElemType::SBendLeft:
            return paint_monorail_cycles_track_s_bend_left;
        case TrackElemType::SBendRight:
            return paint_monorail_cycles_track_s_bend_right;

        case TrackElemType::LeftQuarterTurn3Tiles:
            return paint_monorail_cycles_track_left_quarter_turn_3_tiles;
        case TrackElemType::RightQuarterTurn3Tiles:
            return paint_monorail_cycles_track_right_quarter_turn_3_tiles;
    }

    return nullptr;
}
