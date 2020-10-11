/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../../world/Map.h"
#include "../RideData.h"
#include "../Track.h"
#include "../TrackPaint.h"
#include "../VehiclePaint.h"

/** rct2: 0x */
static void paint_mini_helicopters_track_station(
    paint_session* session, ride_id_t rideIndex, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    uint32_t imageId;

    if (direction == 0 || direction == 2)
    {
        imageId = SPR_STATION_BASE_B_SW_NE | session->TrackColours[SCHEME_MISC];
        sub_98197C(session, imageId, 0, 0, 32, 28, 1, height - 2, 0, 2, height);

        imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_NE_SW | session->TrackColours[SCHEME_TRACK];
        sub_98199C(session, imageId, 0, 0, 32, 20, 1, height, 0, 0, height);

        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 8, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
        paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
    }
    else if (direction == 1 || direction == 3)
    {
        imageId = SPR_STATION_BASE_B_NW_SE | session->TrackColours[SCHEME_MISC];
        sub_98197C(session, imageId, 0, 0, 28, 32, 1, height - 2, 2, 0, height);

        imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_SE_NW | session->TrackColours[SCHEME_TRACK];
        sub_98199C(session, imageId, 0, 0, 20, 32, 1, height, 0, 0, height);

        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_BOXED, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
        paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
    }

    track_paint_util_draw_station(session, rideIndex, direction, height, tileElement);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0081F348 */
static void paint_mini_helicopters_track_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    uint32_t imageId;

    if (direction & 1)
    {
        imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_SE_NW | session->TrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 20, 32, 3, height, 6, 0, height);
        paint_util_push_tunnel_right(session, height, TUNNEL_0);
    }
    else
    {
        imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_NE_SW | session->TrackColours[SCHEME_TRACK];
        sub_98197C(session, imageId, 0, 0, 32, 20, 3, height, 0, 6, height);
        paint_util_push_tunnel_left(session, height, TUNNEL_0);
    }

    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(
            session, (direction & 1) ? METAL_SUPPORTS_STICK_ALT : METAL_SUPPORTS_STICK, 4, -1, height,
            session->TrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0081F368 */
static void paint_mini_helicopters_track_flat_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    uint32_t imageId;

    switch (direction)
    {
        case 0:
            imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_TO_25_DEG_UP_SW_NE | session->TrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 32, 20, 3, height, 0, 6, height);
            paint_util_push_tunnel_left(session, height, TUNNEL_0);
            break;
        case 1:
            imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_TO_25_DEG_UP_NW_SE | session->TrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 20, 32, 3, height, 6, 0, height);
            paint_util_push_tunnel_right(session, height, TUNNEL_2);
            break;
        case 2:
            imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_TO_25_DEG_UP_NE_SW | session->TrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 32, 20, 3, height, 0, 6, height);
            paint_util_push_tunnel_left(session, height, TUNNEL_2);
            break;
        case 3:
            imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_TO_25_DEG_UP_SE_NW | session->TrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 20, 32, 3, height, 6, 0, height);
            paint_util_push_tunnel_right(session, height, TUNNEL_0);
            break;
    }

    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, -4, height, session->TrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/** rct2: 0x0081F358 */
static void paint_mini_helicopters_track_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    uint32_t imageId;

    switch (direction)
    {
        case 0:
            imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_25_DEG_UP_SW_NE | session->TrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 32, 20, 3, height, 0, 6, height);
            paint_util_push_tunnel_left(session, height - 8, TUNNEL_1);
            break;
        case 1:
            imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_25_DEG_UP_NW_SE | session->TrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 20, 32, 3, height, 6, 0, height);
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_2);
            break;
        case 2:
            imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_25_DEG_UP_NE_SW | session->TrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 32, 20, 3, height, 0, 6, height);
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_2);
            break;
        case 3:
            imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_25_DEG_UP_SE_NW | session->TrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 20, 32, 3, height, 6, 0, height);
            paint_util_push_tunnel_right(session, height - 8, TUNNEL_1);
            break;
    }

    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, -9, height, session->TrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x0081F378 */
static void paint_mini_helicopters_track_25_deg_up_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    uint32_t imageId;

    switch (direction)
    {
        case 0:
            imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_25_DEG_UP_TO_FLAT_SW_NE | session->TrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 32, 20, 3, height, 0, 6, height);
            paint_util_push_tunnel_left(session, height - 8, TUNNEL_0);
            break;
        case 1:
            imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_25_DEG_UP_TO_FLAT_NW_SE | session->TrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 20, 32, 3, height, 6, 0, height);
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_12);
            break;
        case 2:
            imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_25_DEG_UP_TO_FLAT_NE_SW | session->TrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 32, 20, 3, height, 0, 6, height);
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_12);
            break;
        case 3:
            imageId = SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_25_DEG_UP_TO_FLAT_SE_NW | session->TrackColours[SCHEME_TRACK];
            sub_98197C(session, imageId, 0, 0, 20, 32, 3, height, 6, 0, height);
            paint_util_push_tunnel_right(session, height - 8, TUNNEL_0);
            break;
    }

    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, -7, height, session->TrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 40, 0x20);
}

/** rct2: 0x */
static void paint_mini_helicopters_track_flat_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    paint_mini_helicopters_track_25_deg_up_to_flat(session, rideIndex, trackSequence, (direction + 2) % 4, height, tileElement);
}

/** rct2: 0x0081F388 */
static void paint_mini_helicopters_track_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    paint_mini_helicopters_track_25_deg_up(session, rideIndex, trackSequence, (direction + 2) % 4, height, tileElement);
}

/** rct2: 0x0081F3A8 */
static void paint_mini_helicopters_track_25_deg_down_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    paint_mini_helicopters_track_flat_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) % 4, height, tileElement);
}

/** rct2: 0x0081F3E8 */
static void paint_mini_helicopters_track_left_quarter_turn_3_tiles(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    track_paint_util_left_quarter_turn_3_tiles_paint(
        session, 3, height, direction, trackSequence, session->TrackColours[SCHEME_TRACK],
        trackSpritesSubmarineRideMiniHelicoptersQuarterTurn3Tiles);
    track_paint_util_left_quarter_turn_3_tiles_tunnel(session, height, TUNNEL_0, direction, trackSequence);

    switch (trackSequence)
    {
        case 0:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, -1, height, session->TrackColours[SCHEME_SUPPORTS]);
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_B4, direction), 0xFFFF, 0);
            break;
        case 2:
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_B8, direction), 0xFFFF, 0);
            break;
        case 3:
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, -1, height, session->TrackColours[SCHEME_SUPPORTS]);
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_C0, direction), 0xFFFF, 0);
            break;
    }

    paint_util_set_general_support_height(session, height + 32, 0x20);
}

static constexpr const uint8_t mini_helicopters_right_quarter_turn_3_tiles_to_left_turn_map[] = { 3, 1, 2, 0 };

/** rct2: 0x0081F3F8 */
static void paint_mini_helicopters_track_right_quarter_turn_3_tiles(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = mini_helicopters_right_quarter_turn_3_tiles_to_left_turn_map[trackSequence];
    paint_mini_helicopters_track_left_quarter_turn_3_tiles(
        session, rideIndex, trackSequence, (direction + 3) % 4, height, tileElement);
}

/** rct2: 0x0081F408 */
static void paint_mini_helicopters_track_left_quarter_turn_1_tile(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    track_paint_util_left_quarter_turn_1_tile_paint(
        session, 1, height, 0, direction, session->TrackColours[SCHEME_TRACK],
        trackSpritesSubmarineRideMiniHelicoptersQuarterTurn1Tile);
    track_paint_util_left_quarter_turn_1_tile_tunnel(session, direction, height, 0, TUNNEL_0, 0, TUNNEL_0);

    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0081F418 */
static void paint_mini_helicopters_track_right_quarter_turn_1_tile(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    paint_mini_helicopters_track_left_quarter_turn_1_tile(
        session, rideIndex, trackSequence, (direction + 3) % 4, height, tileElement);
}

/**
 * rct2: 0x0081F268
 */
TRACK_PAINT_FUNCTION get_track_paint_function_mini_helicopters(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return paint_mini_helicopters_track_flat;

        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return paint_mini_helicopters_track_station;

        case TrackElemType::Up25:
            return paint_mini_helicopters_track_25_deg_up;
        case TrackElemType::FlatToUp25:
            return paint_mini_helicopters_track_flat_to_25_deg_up;
        case TrackElemType::Up25ToFlat:
            return paint_mini_helicopters_track_25_deg_up_to_flat;

        case TrackElemType::Down25:
            return paint_mini_helicopters_track_25_deg_down;
        case TrackElemType::FlatToDown25:
            return paint_mini_helicopters_track_flat_to_25_deg_down;
        case TrackElemType::Down25ToFlat:
            return paint_mini_helicopters_track_25_deg_down_to_flat;

        case TrackElemType::LeftQuarterTurn3Tiles:
            return paint_mini_helicopters_track_left_quarter_turn_3_tiles;
        case TrackElemType::RightQuarterTurn3Tiles:
            return paint_mini_helicopters_track_right_quarter_turn_3_tiles;

        case TrackElemType::LeftQuarterTurn1Tile:
            return paint_mini_helicopters_track_left_quarter_turn_1_tile;
        case TrackElemType::RightQuarterTurn1Tile:
            return paint_mini_helicopters_track_right_quarter_turn_1_tile;
    }

    return nullptr;
}
