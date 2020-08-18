/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../drawing/Drawing.h"
#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../../paint/tile_element/Paint.TileElement.h"
#include "../../sprites.h"
#include "../../world/Map.h"
#include "../../world/Sprite.h"
#include "../RideData.h"
#include "../TrackData.h"
#include "../TrackPaint.h"

/** rct2: 0x008A59A8 */
static void steeplechase_track_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (tileElement->AsTrack()->HasChain())
    {
        switch (direction)
        {
            case 0:
            case 2:
                sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 28635, 0, 6, 32, 20, 3, height);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
            case 3:
                sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 28636, 0, 6, 32, 20, 3, height);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK_ALT, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
            case 2:
                sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 28633, 0, 6, 32, 20, 3, height);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
            case 3:
                sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 28634, 0, 6, 32, 20, 3, height);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK_ALT, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

static void steeplechase_track_station(
    paint_session* session, ride_id_t rideIndex, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    static constexpr const uint32_t imageIds[4][3] = {
        { 28635, SPR_STATION_BASE_B_SW_NE },
        { 28636, SPR_STATION_BASE_B_NW_SE },
        { 28635, SPR_STATION_BASE_B_SW_NE },
        { 28636, SPR_STATION_BASE_B_NW_SE },
    };

    sub_98197C_rotated(
        session, direction, imageIds[direction][1] | session->TrackColours[SCHEME_MISC], 0, 0, 32, 28, 3, height - 2, 0, 2,
        height);
    sub_98199C_rotated(
        session, direction, imageIds[direction][0] | session->TrackColours[SCHEME_TRACK], 0, 6, 32, 20, 3, height, 0, 0,
        height);
    track_paint_util_draw_station_metal_supports_2(session, direction, height, session->TrackColours[SCHEME_SUPPORTS], 3);
    track_paint_util_draw_station(session, rideIndex, direction, height, tileElement);
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x008A59B8 */
static void steeplechase_track_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (tileElement->AsTrack()->HasChain())
    {
        switch (direction)
        {
            case 0:
                sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 28649, 0, 6, 32, 20, 3, height);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 28655, 0, 6, 32, 20, 3, height);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK_ALT, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 28651, 0, 6, 32, 20, 3, height);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 28656, 0, 6, 32, 20, 3, height);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK_ALT, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 28637, 0, 6, 32, 20, 3, height);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 28643, 0, 6, 32, 20, 3, height);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK_ALT, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 28639, 0, 6, 32, 20, 3, height);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 28644, 0, 6, 32, 20, 3, height);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK_ALT, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_2);
    }
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x008A59C8 */
static void steeplechase_track_flat_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (tileElement->AsTrack()->HasChain())
    {
        switch (direction)
        {
            case 0:
                sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 28650, 0, 6, 32, 20, 3, height);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK, 4, 3, height, session->TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 28657, 0, 6, 32, 20, 3, height);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK_ALT, 4, 2, height, session->TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 28652, 0, 6, 32, 20, 3, height);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK, 4, 2, height, session->TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 28658, 0, 6, 32, 20, 3, height);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK_ALT, 4, 2, height, session->TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 28638, 0, 6, 32, 20, 3, height);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK, 4, 3, height, session->TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 28645, 0, 6, 32, 20, 3, height);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK_ALT, 4, 2, height, session->TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 28640, 0, 6, 32, 20, 3, height);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK, 4, 2, height, session->TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 28646, 0, 6, 32, 20, 3, height);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK_ALT, 4, 2, height, session->TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_2);
    }
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/** rct2: 0x008A59D8 */
static void steeplechase_track_25_deg_up_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (tileElement->AsTrack()->HasChain())
    {
        switch (direction)
        {
            case 0:
                sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 28654, 0, 6, 32, 20, 3, height);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK, 4, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 28660, 0, 6, 32, 20, 3, height);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK_ALT, 4, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 28653, 0, 6, 32, 20, 3, height);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK, 4, 5, height, session->TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 28659, 0, 6, 32, 20, 3, height);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK_ALT, 4, 5, height, session->TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 28642, 0, 6, 32, 20, 3, height);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK, 4, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 28648, 0, 6, 32, 20, 3, height);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK_ALT, 4, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 28641, 0, 6, 32, 20, 3, height);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK, 4, 5, height, session->TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 28647, 0, 6, 32, 20, 3, height);
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_STICK_ALT, 4, 5, height, session->TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_0);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_12);
    }
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 40, 0x20);
}

/** rct2: 0x008A59E8 */
static void steeplechase_track_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    steeplechase_track_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008A59F8 */
static void steeplechase_track_flat_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    steeplechase_track_25_deg_up_to_flat(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008A5A08 */
static void steeplechase_track_25_deg_down_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    steeplechase_track_flat_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008A5A18 */
static void steeplechase_track_left_quarter_turn_5(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28698, 0, 6, 32, 20, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK, 4, 1, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28703, 0, 6, 32, 20, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK_ALT, 4, 1, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28708, 0, 6, 32, 20, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK, 4, 0, height - 3, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28693, 0, 6, 32, 20, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK_ALT, 4, 0, height - 1, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 1:
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28697, 0, 0, 32, 16, 3, height);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28702, 0, 0, 32, 16, 3, height);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28707, 0, 16, 32, 16, 3, height);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28692, 0, 16, 32, 16, 3, height);
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28696, 0, 16, 16, 16, 3, height);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28701, 16, 16, 16, 16, 3, height);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28706, 16, 0, 16, 16, 3, height);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28691, 0, 0, 16, 16, 3, height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 4:
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28695, 16, 0, 16, 32, 3, height);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28700, 0, 0, 16, 32, 3, height);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28705, 0, 0, 16, 32, 3, height);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28690, 16, 0, 16, 32, 3, height);
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28694, 6, 0, 20, 32, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK_ALT, 4, 0, height - 3, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28699, 6, 0, 20, 32, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28704, 6, 0, 20, 32, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK_ALT, 4, 1, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28689, 6, 0, 20, 32, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK, 4, 0, height - 2, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 2:
                    paint_util_push_tunnel_right(session, height, TUNNEL_0);
                    break;
                case 3:
                    paint_util_push_tunnel_left(session, height, TUNNEL_0);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A5A28 */
static void steeplechase_track_right_quarter_turn_5(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    steeplechase_track_left_quarter_turn_5(session, rideIndex, trackSequence, (direction - 1) & 3, height, tileElement);
}

/** rct2: 0x008A5A38 */
static void steeplechase_track_s_bend_left(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28665, 0, 6, 32, 20, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28672, 0, 6, 32, 20, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK_ALT, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28668, 0, 6, 32, 20, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28669, 0, 6, 32, 20, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK_ALT, 4, 0, height - 2, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28666, 0, 0, 32, 26, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28671, 0, 0, 32, 26, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK_ALT, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28667, 0, 6, 32, 26, 3, height);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28670, 0, 6, 32, 26, 3, height);
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28667, 0, 6, 32, 26, 3, height);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28670, 0, 6, 32, 26, 3, height);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28666, 0, 0, 32, 26, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28671, 0, 0, 32, 26, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK_ALT, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28668, 0, 6, 32, 20, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28669, 0, 6, 32, 20, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK_ALT, 4, 0, height - 2, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28665, 0, 6, 32, 20, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28672, 0, 6, 32, 20, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK_ALT, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 1:
                    paint_util_push_tunnel_right(session, height, TUNNEL_0);
                    break;
                case 2:
                    paint_util_push_tunnel_left(session, height, TUNNEL_0);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A5A48 */
static void steeplechase_track_s_bend_right(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28661, 0, 6, 32, 20, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28676, 0, 6, 32, 20, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK_ALT, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28664, 0, 6, 32, 20, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28673, 0, 6, 32, 20, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK_ALT, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28662, 0, 6, 32, 26, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK, 8, 0, height - 2, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28675, 0, 6, 32, 26, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK_ALT, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28663, 0, 0, 32, 26, 3, height);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28674, 0, 0, 32, 26, 3, height);
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28663, 0, 0, 32, 26, 3, height);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28674, 0, 0, 32, 26, 3, height);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28662, 0, 6, 32, 26, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK, 8, 0, height - 2, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28675, 0, 6, 32, 26, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK_ALT, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28664, 0, 6, 32, 20, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28673, 0, 6, 32, 20, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK_ALT, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28661, 0, 6, 32, 20, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28676, 0, 6, 32, 20, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK_ALT, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 1:
                    paint_util_push_tunnel_right(session, height, TUNNEL_0);
                    break;
                case 2:
                    paint_util_push_tunnel_left(session, height, TUNNEL_0);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A5A88 */
static void steeplechase_track_left_quarter_turn_3(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28682, 0, 6, 32, 20, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK, 4, 3, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28685, 0, 6, 32, 20, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK_ALT, 4, 1, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28688, 0, 6, 32, 20, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28679, 0, 6, 32, 20, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK_ALT, 4, 0, height - 3, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 1:
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28681, 16, 0, 16, 16, 3, height);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28684, 0, 0, 16, 16, 3, height);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28687, 0, 16, 16, 16, 3, height);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28678, 16, 16, 16, 16, 3, height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28680, 6, 0, 20, 32, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK_ALT, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28683, 6, 0, 20, 32, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK, 4, 1, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28686, 6, 0, 20, 32, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK_ALT, 4, 3, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28677, 6, 0, 20, 32, 3, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK, 4, 0, height - 3, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 2:
                    paint_util_push_tunnel_right(session, height, TUNNEL_0);
                    break;
                case 3:
                    paint_util_push_tunnel_left(session, height, TUNNEL_0);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A5A98 */
static void steeplechase_track_right_quarter_turn_3(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    steeplechase_track_left_quarter_turn_3(session, rideIndex, trackSequence, (direction - 1) & 3, height, tileElement);
}

/** rct2: 0x008A5AA8 */
static void steeplechase_track_brakes(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
        case 2:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 28635, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
        case 3:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 28636, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_STICK_ALT, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
            break;
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x008A5AD8 */
static void steeplechase_track_left_eighth_to_diag(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28725, 0, 0, 32, 20, 3, height, 0, 6, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28729, 0, 0, 32, 20, 3, height, 0, 6, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK_ALT, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28733, 0, 0, 32, 20, 3, height, 0, 6, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28737, 0, 0, 32, 20, 3, height, 0, 6, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK_ALT, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28726, 0, 0, 32, 16, 3, height, 0, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28730, 0, 0, 34, 16, 3, height, 0, 0, height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28734, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28738, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28727, 0, 0, 16, 16, 3, height, 0, 16,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28731, 0, 0, 16, 16, 3, height, 16, 16,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28735, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28739, 0, 0, 16, 16, 3, height, 0, 0, height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 3:
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28728, 0, 0, 16, 16, 3, height, 16, 16,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28732, 0, 0, 16, 18, 3, height, 0, 16,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK_ALT, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28736, 0, 0, 16, 16, 3, height, 0, 0, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28740, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK_ALT, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A5AE8 */
static void steeplechase_track_right_eighth_to_diag(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28709, 0, 0, 32, 20, 3, height, 0, 6, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28713, 0, 0, 32, 20, 3, height, 0, 6, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK_ALT, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28717, 0, 0, 32, 20, 3, height, 0, 6, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28721, 0, 0, 32, 20, 3, height, 0, 6, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK_ALT, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28710, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28714, 0, 0, 32, 16, 3, height, 0, 16,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28718, 0, 0, 34, 16, 3, height, 0, 0, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28722, 0, 0, 32, 16, 3, height, 0, 0, height);
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28711, 0, 0, 16, 16, 3, height, 0, 0, height);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28715, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28719, 0, 0, 28, 28, 3, height, 4, 4, height);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28723, 0, 0, 16, 16, 3, height, 0, 16,
                        height);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 3:
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28712, 0, 0, 16, 16, 3, height, 16, 0,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28716, 0, 0, 16, 16, 3, height, 0, 0, height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK_ALT, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28720, 0, 0, 16, 18, 3, height, 0, 16,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 28724, 0, 0, 16, 16, 3, height, 16, 16,
                        height);
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_STICK_ALT, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            paint_util_set_segment_support_height(
                session,
                paint_util_rotate_segments(
                    SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A5AF8 */
static void steeplechase_track_left_eighth_to_orthogonal(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    steeplechase_track_right_eighth_to_diag(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008A5B08 */
static void steeplechase_track_right_eighth_to_orthogonal(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    steeplechase_track_left_eighth_to_diag(session, rideIndex, trackSequence, (direction + 3) & 3, height, tileElement);
}

/** rct2: 0x008A5AC8 */
static void steeplechase_track_diag_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28760, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28744, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 1:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28757, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28741, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 2:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28759, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28743, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 3:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28758, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK_ALT, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK_ALT, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28742, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK_ALT, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_a_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK_ALT, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A5B38 */
static void steeplechase_track_diag_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28772, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28756, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
        case 1:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28769, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28753, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
        case 2:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28771, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28755, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
        case 3:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK, 1, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28770, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK_ALT, 0, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK, 2, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK_ALT, 3, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK, 1, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28754, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK_ALT, 0, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK, 2, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK_ALT, 3, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008A5B18 */
static void steeplechase_track_diag_flat_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28764, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28748, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 1:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28761, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28745, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 2:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28763, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28747, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 3:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28762, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK_ALT, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK_ALT, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28746, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK_ALT, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK_ALT, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A5B28 */
static void steeplechase_track_diag_25_deg_up_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28768, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28752, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
        case 1:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28765, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28749, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
        case 2:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28767, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28751, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
        case 3:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK, 1, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28766, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK_ALT, 0, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK, 2, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK_ALT, 3, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK, 1, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28750, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK_ALT, 0, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK, 2, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK_ALT, 3, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008A5B68 */
static void steeplechase_track_diag_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28770, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28754, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
        case 1:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28771, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28755, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
        case 2:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28769, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28753, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
        case 3:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK, 1, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28772, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK_ALT, 0, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK, 2, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK_ALT, 3, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK, 1, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28756, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK_ALT, 0, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK, 2, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK_ALT, 3, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008A5B48 */
static void steeplechase_track_diag_flat_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28766, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28750, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            break;
        case 1:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28767, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28751, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            break;
        case 2:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28765, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28749, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            break;
        case 3:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK, 1, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28768, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK_ALT, 0, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK, 2, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK_ALT, 3, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK, 1, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28752, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK_ALT, 0, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK, 2, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK_ALT, 3, 4, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            break;
    }

    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x008A5B58 */
static void steeplechase_track_diag_25_deg_down_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (trackSequence)
    {
        case 0:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28762, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28746, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 1:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28763, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28747, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 2:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28761, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28745, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 3:
            if (tileElement->AsTrack()->HasChain())
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28764, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK_ALT, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK_ALT, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        sub_98197C_rotated(
                            session, direction, session->TrackColours[SCHEME_TRACK] | 28748, -16, -16, 32, 32, 3, height, -16,
                            -16, height);
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK_ALT, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        metal_b_supports_paint_setup(
                            session, METAL_SUPPORTS_STICK_ALT, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A5AB8 */
static void steeplechase_track_block_brakes(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
        case 2:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 28635, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_STICK, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
        case 3:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 28636, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_STICK_ALT, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
            break;
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

TRACK_PAINT_FUNCTION get_track_paint_function_steeplechase(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return steeplechase_track_flat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return steeplechase_track_station;
        case TrackElemType::Up25:
            return steeplechase_track_25_deg_up;
        case TrackElemType::FlatToUp25:
            return steeplechase_track_flat_to_25_deg_up;
        case TrackElemType::Up25ToFlat:
            return steeplechase_track_25_deg_up_to_flat;
        case TrackElemType::Down25:
            return steeplechase_track_25_deg_down;
        case TrackElemType::FlatToDown25:
            return steeplechase_track_flat_to_25_deg_down;
        case TrackElemType::Down25ToFlat:
            return steeplechase_track_25_deg_down_to_flat;
        case TrackElemType::LeftQuarterTurn5Tiles:
            return steeplechase_track_left_quarter_turn_5;
        case TrackElemType::RightQuarterTurn5Tiles:
            return steeplechase_track_right_quarter_turn_5;
        case TrackElemType::SBendLeft:
            return steeplechase_track_s_bend_left;
        case TrackElemType::SBendRight:
            return steeplechase_track_s_bend_right;
        case TrackElemType::LeftQuarterTurn3Tiles:
            return steeplechase_track_left_quarter_turn_3;
        case TrackElemType::RightQuarterTurn3Tiles:
            return steeplechase_track_right_quarter_turn_3;
        case TrackElemType::Brakes:
            return steeplechase_track_brakes;
        case TrackElemType::LeftEighthToDiag:
            return steeplechase_track_left_eighth_to_diag;
        case TrackElemType::RightEighthToDiag:
            return steeplechase_track_right_eighth_to_diag;
        case TrackElemType::LeftEighthToOrthogonal:
            return steeplechase_track_left_eighth_to_orthogonal;
        case TrackElemType::RightEighthToOrthogonal:
            return steeplechase_track_right_eighth_to_orthogonal;
        case TrackElemType::DiagFlat:
            return steeplechase_track_diag_flat;
        case TrackElemType::DiagUp25:
            return steeplechase_track_diag_25_deg_up;
        case TrackElemType::DiagFlatToUp25:
            return steeplechase_track_diag_flat_to_25_deg_up;
        case TrackElemType::DiagUp25ToFlat:
            return steeplechase_track_diag_25_deg_up_to_flat;
        case TrackElemType::DiagDown25:
            return steeplechase_track_diag_25_deg_down;
        case TrackElemType::DiagFlatToDown25:
            return steeplechase_track_diag_flat_to_25_deg_down;
        case TrackElemType::DiagDown25ToFlat:
            return steeplechase_track_diag_25_deg_down_to_flat;
        case TrackElemType::BlockBrakes:
            return steeplechase_track_block_brakes;
    }
    return nullptr;
}
