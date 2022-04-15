/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "meta/CarRide.h"

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
    SPR_CAR_RIDE_FLAT_SW_NE = 28773,
    SPR_CAR_RIDE_FLAT_NW_SE = 28774,
    SPR_CAR_RIDE_LOG_BUMPS_SW_NE = 28775,
    SPR_CAR_RIDE_LOG_BUMPS_NW_SE = 28776,
    SPR_CAR_RIDE_25_DEG_UP_SW_NE = 28777,
    SPR_CAR_RIDE_FLAT_TO_25_DEG_UP_SW_NE = 28778,
    SPR_CAR_RIDE_25_DEG_UP_NE_SW = 28779,
    SPR_CAR_RIDE_FLAT_TO_25_DEG_UP_NE_SW = 28780,
    SPR_CAR_RIDE_DEG_UP_TO_FLAT_NE_SW = 28781,
    SPR_CAR_RIDE_DEG_UP_TO_FLAT_SW_NE = 28782,
    SPR_CAR_RIDE_25_DEG_UP_NW_SE = 28783,
    SPR_CAR_RIDE_25_DEG_UP_SE_NW = 28784,
    SPR_CAR_RIDE_FLAT_TO_25_DEG_UP_NW_SE = 28785,
    SPR_CAR_RIDE_FLAT_TO_25_DEG_UP_SE_NW = 28786,
    SPR_CAR_RIDE_DEG_UP_TO_FLAT_SE_NW = 28787,
    SPR_CAR_RIDE_DEG_UP_TO_FLAT_NW_SE = 28788,
    SPR_CAR_RIDE_25_DEG_UP_TO_60_DEG_UP_SW_NE = 28789,
    SPR_CAR_RIDE_25_DEG_UP_TO_60_DEG_UP_NW_SE = 28790,
    SPR_CAR_RIDE_25_DEG_UP_TO_60_DEG_UP_NE_SW = 28791,
    SPR_CAR_RIDE_25_DEG_UP_TO_60_DEG_UP_SE_NW = 28792,
    SPR_CAR_RIDE_25_DEG_UP_TO_60_DEG_UP_FRONT_NW_SE = 28793,
    SPR_CAR_RIDE_25_DEG_UP_TO_60_DEG_UP_FRONT_NE_SW = 28794,
    SPR_CAR_RIDE_60_DEG_UP_TO_25_DEG_UP_SW_NE = 28795,
    SPR_CAR_RIDE_60_DEG_UP_TO_25_DEG_UP_NW_SE = 28796,
    SPR_CAR_RIDE_60_DEG_UP_TO_25_DEG_UP_NE_SW = 28797,
    SPR_CAR_RIDE_60_DEG_UP_TO_25_DEG_UP_SE_NW = 28798,
    SPR_CAR_RIDE_60_DEG_UP_TO_25_DEG_UP_FRONT_NW_SE = 28799,
    SPR_CAR_RIDE_60_DEG_UP_TO_25_DEG_UP_FRONT_NE_SW = 28800,
    SPR_CAR_RIDE_60_DEG_UP_SW_NE = 28801,
    SPR_CAR_RIDE_60_DEG_UP_NW_SE = 28802,
    SPR_CAR_RIDE_60_DEG_UP_NE_SW = 28803,
    SPR_CAR_RIDE_60_DEG_UP_SE_NW = 28804,
    SPR_CAR_RIDE_QUARTER_TURN_1_TILE_SW_NW = 28805,
    SPR_CAR_RIDE_QUARTER_TURN_1_TILE_NW_NE = 28806,
    SPR_CAR_RIDE_QUARTER_TURN_1_TILE_NE_SE = 28807,
    SPR_CAR_RIDE_QUARTER_TURN_1_TILE_SE_SW = 28808,
    SPR_CAR_RIDE_QUARTER_TURN_3_TILES_SW_SE_PART_0 = 28809,
    SPR_CAR_RIDE_QUARTER_TURN_3_TILES_SW_SE_PART_1 = 28810,
    SPR_CAR_RIDE_QUARTER_TURN_3_TILES_SW_SE_PART_2 = 28811,
    SPR_CAR_RIDE_QUARTER_TURN_3_TILES_NW_SW_PART_0 = 28812,
    SPR_CAR_RIDE_QUARTER_TURN_3_TILES_NW_SW_PART_1 = 28813,
    SPR_CAR_RIDE_QUARTER_TURN_3_TILES_NW_SW_PART_2 = 28814,
    SPR_CAR_RIDE_QUARTER_TURN_3_TILES_NE_NW_PART_0 = 28815,
    SPR_CAR_RIDE_QUARTER_TURN_3_TILES_NE_NW_PART_1 = 28816,
    SPR_CAR_RIDE_QUARTER_TURN_3_TILES_NE_NW_PART_2 = 28817,
    SPR_CAR_RIDE_QUARTER_TURN_3_TILES_SE_NE_PART_0 = 28818,
    SPR_CAR_RIDE_QUARTER_TURN_3_TILES_SE_NE_PART_1 = 28819,
    SPR_CAR_RIDE_QUARTER_TURN_3_TILES_SE_NE_PART_2 = 28820,
};

static constexpr const uint32_t car_ride_track_pieces_flat[4] = {
    SPR_CAR_RIDE_FLAT_SW_NE,
    SPR_CAR_RIDE_FLAT_NW_SE,
    SPR_CAR_RIDE_FLAT_SW_NE,
    SPR_CAR_RIDE_FLAT_NW_SE,
};

static constexpr const uint32_t car_ride_track_pieces_log_bumps[4] = {
    SPR_CAR_RIDE_LOG_BUMPS_SW_NE,
    SPR_CAR_RIDE_LOG_BUMPS_NW_SE,
    SPR_CAR_RIDE_LOG_BUMPS_SW_NE,
    SPR_CAR_RIDE_LOG_BUMPS_NW_SE,
};

static constexpr const uint32_t car_ride_track_pieces_25_deg_up[4] = {
    SPR_CAR_RIDE_25_DEG_UP_SW_NE,
    SPR_CAR_RIDE_25_DEG_UP_NW_SE,
    SPR_CAR_RIDE_25_DEG_UP_NE_SW,
    SPR_CAR_RIDE_25_DEG_UP_SE_NW,
};

static constexpr const uint32_t car_ride_track_pieces_flat_to_25_deg_up[4] = {
    SPR_CAR_RIDE_FLAT_TO_25_DEG_UP_SW_NE,
    SPR_CAR_RIDE_FLAT_TO_25_DEG_UP_NW_SE,
    SPR_CAR_RIDE_FLAT_TO_25_DEG_UP_NE_SW,
    SPR_CAR_RIDE_FLAT_TO_25_DEG_UP_SE_NW,
};

static constexpr const uint32_t car_ride_track_pieces_25_deg_up_to_flat[4] = {
    SPR_CAR_RIDE_DEG_UP_TO_FLAT_SW_NE,
    SPR_CAR_RIDE_DEG_UP_TO_FLAT_NW_SE,
    SPR_CAR_RIDE_DEG_UP_TO_FLAT_NE_SW,
    SPR_CAR_RIDE_DEG_UP_TO_FLAT_SE_NW,
};

static constexpr const uint32_t car_ride_track_pieces_60_deg_up[4] = {
    SPR_CAR_RIDE_60_DEG_UP_SW_NE,
    SPR_CAR_RIDE_60_DEG_UP_NW_SE,
    SPR_CAR_RIDE_60_DEG_UP_NE_SW,
    SPR_CAR_RIDE_60_DEG_UP_SE_NW,
};

static constexpr const uint32_t car_ride_track_pieces_25_deg_up_to_60_deg_up[4][2] = {
    { SPR_CAR_RIDE_25_DEG_UP_TO_60_DEG_UP_SW_NE, 0 },
    { SPR_CAR_RIDE_25_DEG_UP_TO_60_DEG_UP_NW_SE, SPR_CAR_RIDE_25_DEG_UP_TO_60_DEG_UP_FRONT_NW_SE },
    { SPR_CAR_RIDE_25_DEG_UP_TO_60_DEG_UP_NE_SW, SPR_CAR_RIDE_25_DEG_UP_TO_60_DEG_UP_FRONT_NE_SW },
    { SPR_CAR_RIDE_25_DEG_UP_TO_60_DEG_UP_SE_NW, 0 },
};

static constexpr const uint32_t car_ride_track_pieces_60_deg_up_to_25_deg_up[4][2] = {
    { SPR_CAR_RIDE_60_DEG_UP_TO_25_DEG_UP_SW_NE, 0 },
    { SPR_CAR_RIDE_60_DEG_UP_TO_25_DEG_UP_NW_SE, SPR_CAR_RIDE_60_DEG_UP_TO_25_DEG_UP_FRONT_NW_SE },
    { SPR_CAR_RIDE_60_DEG_UP_TO_25_DEG_UP_NE_SW, SPR_CAR_RIDE_60_DEG_UP_TO_25_DEG_UP_FRONT_NE_SW },
    { SPR_CAR_RIDE_60_DEG_UP_TO_25_DEG_UP_SE_NW, 0 },
};

static constexpr const uint32_t car_ride_track_pieces_left_quarter_turn_1_tile[4] = {
    SPR_CAR_RIDE_QUARTER_TURN_1_TILE_SW_NW,
    SPR_CAR_RIDE_QUARTER_TURN_1_TILE_NW_NE,
    SPR_CAR_RIDE_QUARTER_TURN_1_TILE_NE_SE,
    SPR_CAR_RIDE_QUARTER_TURN_1_TILE_SE_SW,
};

static constexpr const uint32_t car_ride_track_pieces_quarter_turn_3_tiles[4][3] = {
    {
        SPR_CAR_RIDE_QUARTER_TURN_3_TILES_SW_SE_PART_0,
        SPR_CAR_RIDE_QUARTER_TURN_3_TILES_SW_SE_PART_1,
        SPR_CAR_RIDE_QUARTER_TURN_3_TILES_SW_SE_PART_2,
    },
    {
        SPR_CAR_RIDE_QUARTER_TURN_3_TILES_NW_SW_PART_0,
        SPR_CAR_RIDE_QUARTER_TURN_3_TILES_NW_SW_PART_1,
        SPR_CAR_RIDE_QUARTER_TURN_3_TILES_NW_SW_PART_2,
    },
    {
        SPR_CAR_RIDE_QUARTER_TURN_3_TILES_NE_NW_PART_0,
        SPR_CAR_RIDE_QUARTER_TURN_3_TILES_NE_NW_PART_1,
        SPR_CAR_RIDE_QUARTER_TURN_3_TILES_NE_NW_PART_2,
    },
    {
        SPR_CAR_RIDE_QUARTER_TURN_3_TILES_SE_NE_PART_0,
        SPR_CAR_RIDE_QUARTER_TURN_3_TILES_SE_NE_PART_1,
        SPR_CAR_RIDE_QUARTER_TURN_3_TILES_SE_NE_PART_2,
    },
};

/** rct2: 0x006F72C8 */
static void paint_car_ride_track_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    uint32_t imageId = car_ride_track_pieces_flat[direction] | session.TrackColours[SCHEME_TRACK];

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
        paint_util_push_tunnel_left(session, height, TUNNEL_0);
    }
    else
    {
        paint_util_push_tunnel_right(session, height, TUNNEL_0);
    }

    metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x006F72D8 */
static void paint_car_ride_track_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    uint32_t imageId = car_ride_track_pieces_25_deg_up[direction] | session.TrackColours[SCHEME_TRACK];

    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsParent(session, imageId, { 0, 2, height }, { 32, 20, 1 }, { 0, 6, height });
    }
    else
    {
        PaintAddImageAsParent(session, imageId, { 2, 0, height }, { 20, 32, 1 }, { 6, 0, height });
    }

    switch (direction)
    {
        case 0:
            paint_util_push_tunnel_left(session, height - 8, TUNNEL_1);
            break;
        case 1:
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_2);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_2);
            break;
        case 3:
            paint_util_push_tunnel_right(session, height - 8, TUNNEL_1);
            break;
    }

    metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x006F72E8 */
static void paint_car_ride_track_flat_to_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    uint32_t imageId = car_ride_track_pieces_flat_to_25_deg_up[direction] | session.TrackColours[SCHEME_TRACK];

    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsParent(session, imageId, { 0, 2, height }, { 32, 20, 1 }, { 0, 6, height });
    }
    else
    {
        PaintAddImageAsParent(session, imageId, { 2, 0, height }, { 20, 32, 1 }, { 6, 0, height });
    }

    switch (direction)
    {
        case 0:
            paint_util_push_tunnel_left(session, height, TUNNEL_0);
            break;
        case 1:
            paint_util_push_tunnel_right(session, height, TUNNEL_2);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height, TUNNEL_2);
            break;
        case 3:
            paint_util_push_tunnel_right(session, height, TUNNEL_0);
            break;
    }

    metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/** rct2: 0x006F72F8 */
static void paint_car_ride_track_25_deg_up_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    uint32_t imageId = car_ride_track_pieces_25_deg_up_to_flat[direction] | session.TrackColours[SCHEME_TRACK];

    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsParent(session, imageId, { 0, 2, height }, { 32, 20, 1 }, { 0, 6, height });
    }
    else
    {
        PaintAddImageAsParent(session, imageId, { 2, 0, height }, { 20, 32, 1 }, { 6, 0, height });
    }

    switch (direction)
    {
        case 0:
            paint_util_push_tunnel_left(session, height - 8, TUNNEL_0);
            break;
        case 1:
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_12);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_12);
            break;
        case 3:
            paint_util_push_tunnel_right(session, height - 8, TUNNEL_0);
            break;
    }

    metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 40, 0x20);
}

/** rct2: 0x006F7308 */
static void paint_car_ride_track_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    paint_car_ride_track_25_deg_up(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

/** rct2: 0x006F7318 */
static void paint_car_ride_track_flat_to_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    paint_car_ride_track_25_deg_up_to_flat(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

/** rct2: 0x006F7328 */
static void paint_car_ride_track_25_deg_down_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    paint_car_ride_track_flat_to_25_deg_up(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

/** rct2: 0x006F7338, 0x006F7348, 0x006F7358 */
static void paint_car_ride_station(
    paint_session& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    uint32_t imageId;

    if (direction == 0 || direction == 2)
    {
        imageId = SPR_STATION_BASE_B_SW_NE | session.TrackColours[SCHEME_MISC];
        PaintAddImageAsParent(session, imageId, { 0, 0, height - 2 }, { 32, 28, 1 }, { 0, 2, height });
    }
    else if (direction == 1 || direction == 3)
    {
        imageId = SPR_STATION_BASE_B_NW_SE | session.TrackColours[SCHEME_MISC];
        PaintAddImageAsParent(session, imageId, { 0, 0, height - 2 }, { 28, 32, 1 }, { 2, 0, height });
    }

    imageId = car_ride_track_pieces_flat[direction] | session.TrackColours[SCHEME_TRACK];
    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsChild(session, imageId, { 0, 6, height }, { 32, 20, 1 }, { 0, 0, height });
    }
    else
    {
        PaintAddImageAsChild(session, imageId, { 6, 0, height }, { 20, 32, 1 }, { 0, 0, height });
    }

    if (direction == 0 || direction == 2)
    {
        paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
    }

    if (direction == 0 || direction == 2)
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    else
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    track_paint_util_draw_station(session, ride, direction, height, trackElement);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x006F7378 */
static void paint_car_ride_track_right_quarter_turn_3_tiles(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    track_paint_util_right_quarter_turn_3_tiles_paint(
        session, 3, height, direction, trackSequence, session.TrackColours[SCHEME_TRACK],
        car_ride_track_pieces_quarter_turn_3_tiles, defaultRightQuarterTurn3TilesOffsets,
        defaultRightQuarterTurn3TilesBoundLengths, nullptr);
    track_paint_util_right_quarter_turn_3_tiles_tunnel(session, height, direction, trackSequence, TUNNEL_0);

    switch (trackSequence)
    {
        case 0:
        case 3:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(blockedSegments, direction), 0xFFFF, 0);

    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x006F7368 */
static void paint_car_ride_track_left_quarter_turn_3_tiles(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    paint_car_ride_track_right_quarter_turn_3_tiles(session, ride, trackSequence, (direction + 1) % 4, height, trackElement);
}

/** rct2: 0x006F7388 */
static void paint_car_ride_track_left_quarter_turn_1_tile(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    uint32_t imageId = car_ride_track_pieces_left_quarter_turn_1_tile[direction] | session.TrackColours[SCHEME_TRACK];

    switch (direction)
    {
        case 0:
            PaintAddImageAsParent(session, imageId, { 6, 0, height }, { 26, 24, 1 }, { 6, 2, height });
            break;
        case 1:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 26, 26, 1 });
            break;
        case 2:
            PaintAddImageAsParent(session, imageId, { 0, 6, height }, { 24, 26, 1 }, { 2, 6, height });
            break;
        case 3:
            PaintAddImageAsParent(session, imageId, { 6, 6, height }, { 24, 24, 1 });
            break;
    }

    metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);

    track_paint_util_left_quarter_turn_1_tile_tunnel(session, direction, height, 0, TUNNEL_0, 0, TUNNEL_0);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x006F7398 */
static void paint_car_ride_track_right_quarter_turn_1_tile(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    paint_car_ride_track_left_quarter_turn_1_tile(session, ride, trackSequence, (direction + 3) % 4, height, trackElement);
}

/** rct2: 0x006F73A8 */
static void paint_car_ride_track_spinning_tunnel(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    uint32_t imageId = car_ride_track_pieces_flat[direction] | session.TrackColours[SCHEME_TRACK];

    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsParent(session, imageId, { 0, 6, height }, { 32, 20, 1 });
    }
    else
    {
        PaintAddImageAsParent(session, imageId, { 6, 0, height }, { 20, 32, 1 });
    }

    track_paint_util_spinning_tunnel_paint(session, 1, height, direction);

    if (direction == 0 || direction == 2)
    {
        paint_util_push_tunnel_left(session, height, TUNNEL_0);
    }
    else
    {
        paint_util_push_tunnel_right(session, height, TUNNEL_0);
    }

    wooden_a_supports_paint_setup(session, (direction & 1), 0, height, session.TrackColours[SCHEME_MISC]);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x006F73B8 */
static void paint_car_ride_track_60_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    uint32_t imageId = car_ride_track_pieces_60_deg_up[direction] | session.TrackColours[SCHEME_TRACK];

    switch (direction)
    {
        case 0:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 20, 1 }, { 0, 6, height });
            break;
        case 1:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 1, 32, 98 }, { 27, 0, height });
            break;
        case 2:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 1, 98 }, { 0, 27, height });
            break;
        case 3:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 20, 32, 1 }, { 6, 0, height });
            break;
    }

    switch (direction)
    {
        case 0:
            paint_util_push_tunnel_left(session, height - 8, TUNNEL_1);
            break;
        case 1:
            paint_util_push_tunnel_right(session, height + 56, TUNNEL_2);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height + 56, TUNNEL_2);
            break;
        case 3:
            paint_util_push_tunnel_right(session, height - 8, TUNNEL_1);
            break;
    }

    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 104, 0x20);
}

/** rct2: 0x006F73C8 */
static void paint_car_ride_track_25_deg_up_to_60_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    uint32_t imageId = car_ride_track_pieces_25_deg_up_to_60_deg_up[direction][0] | session.TrackColours[SCHEME_TRACK];

    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 20, 1 }, { 0, 6, height });
    }
    else
    {
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 20, 32, 1 }, { 6, 0, height });
    }

    if (car_ride_track_pieces_25_deg_up_to_60_deg_up[direction][1] != 0)
    {
        imageId = car_ride_track_pieces_25_deg_up_to_60_deg_up[direction][1] | session.TrackColours[SCHEME_TRACK];

        if (direction == 0 || direction == 2)
        {
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 1, 66 }, { 0, 27, height });
        }
        else
        {
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 1, 32, 66 }, { 27, 0, height });
        }
    }

    switch (direction)
    {
        case 0:
            paint_util_push_tunnel_left(session, height - 8, TUNNEL_1);
            break;
        case 1:
            paint_util_push_tunnel_right(session, height + 24, TUNNEL_2);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height + 24, TUNNEL_2);
            break;
        case 3:
            paint_util_push_tunnel_right(session, height - 8, TUNNEL_1);
            break;
    }

    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 72, 0x20);
}

/** rct2: 0x006F73D8 */
static void paint_car_ride_track_60_deg_up_to_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    uint32_t imageId = car_ride_track_pieces_60_deg_up_to_25_deg_up[direction][0] | session.TrackColours[SCHEME_TRACK];

    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 20, 1 }, { 0, 6, height });
    }
    else
    {
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 20, 32, 1 }, { 6, 0, height });
    }

    if (car_ride_track_pieces_60_deg_up_to_25_deg_up[direction][1] != 0)
    {
        imageId = car_ride_track_pieces_60_deg_up_to_25_deg_up[direction][1] | session.TrackColours[SCHEME_TRACK];

        if (direction == 0 || direction == 2)
        {
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 1, 66 }, { 0, 27, height });
        }
        else
        {
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 1, 32, 66 }, { 27, 0, height });
        }
    }

    switch (direction)
    {
        case 0:
            paint_util_push_tunnel_left(session, height - 8, TUNNEL_1);
            break;
        case 1:
            paint_util_push_tunnel_right(session, height + 24, TUNNEL_2);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height + 24, TUNNEL_2);
            break;
        case 3:
            paint_util_push_tunnel_right(session, height - 8, TUNNEL_1);
            break;
    }

    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 72, 0x20);
}

/** rct2: 0x006F73E8 */
static void paint_car_ride_track_60_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    paint_car_ride_track_60_deg_up(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

/** rct2: 0x006F73F8 */
static void paint_car_ride_track_25_deg_down_to_60_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    paint_car_ride_track_60_deg_up_to_25_deg_up(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

/** rct2: 0x006F7408 */
static void paint_car_ride_track_60_deg_down_to_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    paint_car_ride_track_25_deg_up_to_60_deg_up(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

/** rct2: 0x006F7418 */
static void paint_car_ride_track_log_bumps(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    uint32_t imageId = car_ride_track_pieces_log_bumps[direction] | session.TrackColours[SCHEME_TRACK];

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
        paint_util_push_tunnel_left(session, height, TUNNEL_0);
    }
    else
    {
        paint_util_push_tunnel_right(session, height, TUNNEL_0);
    }

    metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/**
 * rct2: 0x006F7000
 */
TRACK_PAINT_FUNCTION get_track_paint_function_car_ride(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return paint_car_ride_track_flat;

        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return paint_car_ride_station;

        case TrackElemType::Up25:
            return paint_car_ride_track_25_deg_up;
        case TrackElemType::Up60:
            return paint_car_ride_track_60_deg_up;
        case TrackElemType::FlatToUp25:
            return paint_car_ride_track_flat_to_25_deg_up;
        case TrackElemType::Up25ToUp60:
            return paint_car_ride_track_25_deg_up_to_60_deg_up;
        case TrackElemType::Up60ToUp25:
            return paint_car_ride_track_60_deg_up_to_25_deg_up;
        case TrackElemType::Up25ToFlat:
            return paint_car_ride_track_25_deg_up_to_flat;

        case TrackElemType::Down25:
            return paint_car_ride_track_25_deg_down;
        case TrackElemType::Down60:
            return paint_car_ride_track_60_deg_down;
        case TrackElemType::FlatToDown25:
            return paint_car_ride_track_flat_to_25_deg_down;
        case TrackElemType::Down25ToDown60:
            return paint_car_ride_track_25_deg_down_to_60_deg_down;
        case TrackElemType::Down60ToDown25:
            return paint_car_ride_track_60_deg_down_to_25_deg_down;
        case TrackElemType::Down25ToFlat:
            return paint_car_ride_track_25_deg_down_to_flat;

        case TrackElemType::LeftQuarterTurn3Tiles:
            return paint_car_ride_track_left_quarter_turn_3_tiles;
        case TrackElemType::RightQuarterTurn3Tiles:
            return paint_car_ride_track_right_quarter_turn_3_tiles;

        case TrackElemType::LeftQuarterTurn1Tile:
            return paint_car_ride_track_left_quarter_turn_1_tile;
        case TrackElemType::RightQuarterTurn1Tile:
            return paint_car_ride_track_right_quarter_turn_1_tile;

        case TrackElemType::Rapids:
            return paint_car_ride_track_log_bumps;

        case TrackElemType::SpinningTunnel:
            return paint_car_ride_track_spinning_tunnel;
    }

    return nullptr;
}

ObjectEntryIndex OpenRCT2::RideType::RCT2ToOpenRCT2::CarRide(uint8_t rct2RideType, const rct_ride_entry* rideEntry)
{
    if (rideEntry != nullptr && ride_entry_get_supported_track_pieces(rideEntry) & (1ULL << TRACK_SLOPE_STEEP_DOWN))
        return RIDE_TYPE_MONSTER_TRUCKS;
    return RIDE_TYPE_CAR_RIDE;
}
