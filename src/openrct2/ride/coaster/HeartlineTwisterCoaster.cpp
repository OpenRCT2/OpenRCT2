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

/** rct2: 0x0087694C */
static void heartline_twister_rc_track_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (tileElement->AsTrack()->HasChain())
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21354, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21356, 0, 0, 32, 1, 26, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21355, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21357, 0, 0, 32, 1, 26, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21358, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21360, 0, 0, 32, 1, 26, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21359, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21361, 0, 0, 32, 1, 26, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21294, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21296, 0, 0, 32, 1, 26, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 1:
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21295, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21297, 0, 0, 32, 1, 26, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
        }
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

static void heartline_twister_rc_track_station(
    paint_session* session, ride_id_t rideIndex, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    static constexpr const uint32_t imageIds[4][3] = {
        { 19732, SPR_STATION_BASE_B_SW_NE },
        { 19733, SPR_STATION_BASE_B_NW_SE },
        { 19732, SPR_STATION_BASE_B_SW_NE },
        { 19733, SPR_STATION_BASE_B_NW_SE },
    };

    sub_98197C_rotated(
        session, direction, imageIds[direction][0] | session->TrackColours[SCHEME_TRACK], 0, 0, 32, 20, 1, height, 0, 6,
        height + 3);
    sub_98196C_rotated(
        session, direction, imageIds[direction][1] | session->TrackColours[SCHEME_MISC], 0, 0, 32, 32, 1, height);
    track_paint_util_draw_station_metal_supports_2(session, direction, height, session->TrackColours[SCHEME_SUPPORTS], 0);
    track_paint_util_draw_station(session, rideIndex, direction, height, tileElement);
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0087695C */
static void heartline_twister_rc_track_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (tileElement->AsTrack()->HasChain())
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21378, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21382, 0, 0, 32, 1, 50, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 6, 9, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21379, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21383, 0, 0, 32, 1, 50, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 7, 10, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21380, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21384, 0, 0, 32, 1, 50, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 6, 11, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21381, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21385, 0, 0, 32, 1, 50, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 7, 12, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21322, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21326, 0, 0, 32, 1, 50, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 6, 9, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21323, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21327, 0, 0, 32, 1, 50, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 7, 10, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21324, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21328, 0, 0, 32, 1, 50, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 6, 11, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21325, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21329, 0, 0, 32, 1, 50, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 7, 12, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
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
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x008769FC */
static void heartline_twister_rc_track_60_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (tileElement->AsTrack()->HasChain())
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21402, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21406, 0, 0, 32, 1, 98, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 6, 21, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 1:
                session->WoodenSupportsPrependTo = sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21403, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21407, 0, 0, 32, 1, 98, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 7, 22, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 2:
                session->WoodenSupportsPrependTo = sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21404, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21408, 0, 0, 32, 1, 98, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 6, 23, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21405, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21409, 0, 0, 32, 1, 98, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 7, 24, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21346, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21350, 0, 0, 32, 1, 98, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 6, 21, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 1:
                session->WoodenSupportsPrependTo = sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21347, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21351, 0, 0, 32, 1, 98, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 7, 22, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 2:
                session->WoodenSupportsPrependTo = sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21348, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21352, 0, 0, 32, 1, 98, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 6, 23, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21349, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21353, 0, 0, 32, 1, 98, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 7, 24, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
        }
    }
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 56, TUNNEL_2);
    }
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 104, 0x20);
}

/** rct2: 0x0087696C */
static void heartline_twister_rc_track_flat_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (tileElement->AsTrack()->HasChain())
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21362, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21366, 0, 0, 32, 1, 42, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 6, 1, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21363, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21367, 0, 0, 32, 1, 42, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 7, 2, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21364, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21368, 0, 0, 32, 1, 42, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 6, 3, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21365, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21369, 0, 0, 32, 1, 42, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 7, 4, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21306, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21310, 0, 0, 32, 1, 42, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 6, 1, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21307, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21311, 0, 0, 32, 1, 42, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 7, 2, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21308, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21312, 0, 0, 32, 1, 42, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 6, 3, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21309, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21313, 0, 0, 32, 1, 42, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 7, 4, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
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
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/** rct2: 0x008769BC */
static void heartline_twister_rc_track_25_deg_up_to_60_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (tileElement->AsTrack()->HasChain())
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21386, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21390, 0, 0, 32, 1, 66, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 6, 13, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 1:
                session->WoodenSupportsPrependTo = sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21387, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21391, 0, 0, 32, 1, 66, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 7, 14, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 2:
                session->WoodenSupportsPrependTo = sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21388, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21392, 0, 0, 32, 1, 66, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 6, 15, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21389, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21393, 0, 0, 32, 1, 66, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 7, 16, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21330, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21334, 0, 0, 32, 1, 66, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 6, 13, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 1:
                session->WoodenSupportsPrependTo = sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21331, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21335, 0, 0, 32, 1, 66, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 7, 14, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 2:
                session->WoodenSupportsPrependTo = sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21332, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21336, 0, 0, 32, 1, 66, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 6, 15, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21333, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21337, 0, 0, 32, 1, 66, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 7, 16, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
        }
    }
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 24, TUNNEL_2);
    }
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 72, 0x20);
}

/** rct2: 0x008769CC */
static void heartline_twister_rc_track_60_deg_up_to_25_deg_up(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (tileElement->AsTrack()->HasChain())
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21394, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21398, 0, 0, 32, 1, 66, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 6, 17, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 1:
                session->WoodenSupportsPrependTo = sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21395, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21399, 0, 0, 32, 1, 66, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 7, 18, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 2:
                session->WoodenSupportsPrependTo = sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21396, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21400, 0, 0, 32, 1, 66, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 6, 19, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21397, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21401, 0, 0, 32, 1, 66, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 7, 20, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21338, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21342, 0, 0, 32, 1, 66, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 6, 17, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 1:
                session->WoodenSupportsPrependTo = sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21339, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21343, 0, 0, 32, 1, 66, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 7, 18, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 2:
                session->WoodenSupportsPrependTo = sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21340, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21344, 0, 0, 32, 1, 66, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 6, 19, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21341, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21345, 0, 0, 32, 1, 66, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 7, 20, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
        }
    }
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 24, TUNNEL_2);
    }
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 72, 0x20);
}

/** rct2: 0x0087697C */
static void heartline_twister_rc_track_25_deg_up_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    if (tileElement->AsTrack()->HasChain())
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21370, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21374, 0, 0, 32, 1, 34, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 6, 5, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21371, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21375, 0, 0, 32, 1, 34, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 7, 6, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21372, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21376, 0, 0, 32, 1, 34, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 6, 7, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21373, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21377, 0, 0, 32, 1, 34, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 7, 8, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21314, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21318, 0, 0, 32, 1, 34, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 6, 5, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 1:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21315, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21319, 0, 0, 32, 1, 34, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 7, 6, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 2:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21316, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21320, 0, 0, 32, 1, 34, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 6, 7, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            case 3:
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21317, 0, 0, 32, 20, 2, height, 0, 6, height);
                sub_98197C_rotated(
                    session, direction, session->TrackColours[SCHEME_TRACK] | 21321, 0, 0, 32, 1, 34, height, 0, 27, height);
                wooden_a_supports_paint_setup(session, 7, 8, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
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
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 40, 0x20);
}

/** rct2: 0x0087698C */
static void heartline_twister_rc_track_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    heartline_twister_rc_track_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x00876A0C */
static void heartline_twister_rc_track_60_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    heartline_twister_rc_track_60_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x0087699C */
static void heartline_twister_rc_track_flat_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    heartline_twister_rc_track_25_deg_up_to_flat(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008769DC */
static void heartline_twister_rc_track_25_deg_down_to_60_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    heartline_twister_rc_track_60_deg_up_to_25_deg_up(
        session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008769EC */
static void heartline_twister_rc_track_60_deg_down_to_25_deg_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    heartline_twister_rc_track_25_deg_up_to_60_deg_up(
        session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008769AC */
static void heartline_twister_rc_track_25_deg_down_to_flat(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    heartline_twister_rc_track_flat_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x00876A6C */
static void heartline_twister_rc_track_heartline_transfer_up(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21306, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21310, 0, 0, 32, 20, 0, height, 0, 6,
                        height + 23);
                    wooden_a_supports_paint_setup(session, 6, 1, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21307, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21311, 0, 0, 32, 20, 0, height, 0, 6,
                        height + 23);
                    wooden_a_supports_paint_setup(session, 7, 2, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21308, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21312, 0, 0, 32, 20, 0, height, 0, 6,
                        height + 23);
                    wooden_a_supports_paint_setup(session, 6, 3, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21309, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21313, 0, 0, 32, 20, 0, height, 0, 6,
                        height + 23);
                    wooden_a_supports_paint_setup(session, 7, 4, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
            }
            else
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_2);
            }
            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21314, 0, 0, 32, 20, 2, height + 8, 0, 6,
                        height + 8);
                    sub_98199C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21308, 0, 0, 32, 20, 2, height + 16, 0, 6,
                        height + 16);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21318, 0, 0, 32, 1, 34, height + 8, 0, 27,
                        height + 8);
                    sub_98199C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21312, 0, 0, 32, 1, 34, height + 16, 0, 27,
                        height + 16);
                    wooden_a_supports_paint_setup(session, 6, 5, height + 8, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21315, 0, 0, 32, 20, 2, height + 8, 0, 6,
                        height + 8);
                    sub_98199C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21309, 0, 0, 32, 20, 2, height + 16, 0, 6,
                        height + 16);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21319, 0, 0, 32, 1, 34, height + 8, 0, 27,
                        height + 8);
                    sub_98199C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21313, 0, 0, 32, 1, 34, height + 16, 0, 27,
                        height + 16);
                    wooden_a_supports_paint_setup(session, 7, 6, height + 8, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21316, 0, 0, 32, 20, 2, height + 8, 0, 6,
                        height + 8);
                    sub_98199C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21306, 0, 0, 32, 20, 2, height + 16, 0, 6,
                        height + 16);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21320, 0, 0, 32, 1, 34, height + 8, 0, 27,
                        height + 8);
                    sub_98199C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21310, 0, 0, 32, 1, 34, height + 16, 0, 27,
                        height + 16);
                    wooden_a_supports_paint_setup(session, 6, 7, height + 8, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21317, 0, 0, 32, 20, 2, height + 8, 0, 6,
                        height + 8);
                    sub_98199C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21307, 0, 0, 32, 20, 2, height + 16, 0, 6,
                        height + 16);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21321, 0, 0, 32, 1, 34, height + 8, 0, 27,
                        height + 8);
                    sub_98199C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21311, 0, 0, 32, 1, 34, height + 16, 0, 27,
                        height + 16);
                    wooden_a_supports_paint_setup(session, 7, 8, height + 8, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }
            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21298, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21302, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21299, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21303, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21300, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21304, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21301, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21305, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }
            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21316, 0, 0, 32, 20, 2, height - 8, 0, 6,
                        height - 7);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21320, 0, 0, 32, 1, 34, height - 8, 0, 27,
                        height - 8);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21317, 0, 0, 32, 20, 2, height - 8, 0, 6,
                        height - 7);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21321, 0, 0, 32, 1, 34, height - 8, 0, 27,
                        height - 8);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21314, 0, 0, 32, 20, 2, height - 8, 0, 6,
                        height - 7);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21318, 0, 0, 32, 1, 34, height - 8, 0, 27,
                        height - 8);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21315, 0, 0, 32, 20, 2, height - 8, 0, 6,
                        height - 7);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21319, 0, 0, 32, 1, 34, height - 8, 0, 27,
                        height - 8);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
            }
            else
            {
                paint_util_push_tunnel_rotated(session, direction, height - 16, TUNNEL_0);
            }
            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x00876A7C */
static void heartline_twister_rc_track_heartline_transfer_down(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21316, 0, 0, 32, 20, 2, height - 8, 0, 6,
                        height - 7);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21320, 0, 0, 32, 1, 34, height - 8, 0, 27,
                        height - 8);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21317, 0, 0, 32, 20, 2, height - 8, 0, 6,
                        height - 7);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21321, 0, 0, 32, 1, 34, height - 8, 0, 27,
                        height - 8);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21314, 0, 0, 32, 20, 2, height - 8, 0, 6,
                        height - 7);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21318, 0, 0, 32, 1, 34, height - 8, 0, 27,
                        height - 8);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21315, 0, 0, 32, 20, 2, height - 8, 0, 6,
                        height - 7);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21319, 0, 0, 32, 1, 34, height - 8, 0, 27,
                        height - 8);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
            }
            else
            {
                paint_util_push_tunnel_rotated(session, direction, height - 16, TUNNEL_0);
            }
            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21314, 0, 0, 32, 20, 2, height + 8, 0, 6,
                        height + 8);
                    sub_98199C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21308, 0, 0, 32, 20, 2, height + 16, 0, 6,
                        height + 16);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21318, 0, 0, 32, 1, 34, height + 8, 0, 27,
                        height + 8);
                    sub_98199C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21312, 0, 0, 32, 1, 34, height + 16, 0, 27,
                        height + 16);
                    wooden_a_supports_paint_setup(session, 6, 5, height + 8, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21315, 0, 0, 32, 20, 2, height + 8, 0, 6,
                        height + 8);
                    sub_98199C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21309, 0, 0, 32, 20, 2, height + 16, 0, 6,
                        height + 16);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21319, 0, 0, 32, 1, 34, height + 8, 0, 27,
                        height + 8);
                    sub_98199C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21313, 0, 0, 32, 1, 34, height + 16, 0, 27,
                        height + 16);
                    wooden_a_supports_paint_setup(session, 7, 6, height + 8, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21316, 0, 0, 32, 20, 2, height + 8, 0, 6,
                        height + 8);
                    sub_98199C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21306, 0, 0, 32, 20, 2, height + 16, 0, 6,
                        height + 16);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21320, 0, 0, 32, 1, 34, height + 8, 0, 27,
                        height + 8);
                    sub_98199C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21310, 0, 0, 32, 1, 34, height + 16, 0, 27,
                        height + 16);
                    wooden_a_supports_paint_setup(session, 6, 7, height + 8, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21317, 0, 0, 32, 20, 2, height + 8, 0, 6,
                        height + 8);
                    sub_98199C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21307, 0, 0, 32, 20, 2, height + 16, 0, 6,
                        height + 16);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21321, 0, 0, 32, 1, 34, height + 8, 0, 27,
                        height + 8);
                    sub_98199C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21311, 0, 0, 32, 1, 34, height + 16, 0, 27,
                        height + 16);
                    wooden_a_supports_paint_setup(session, 7, 8, height + 8, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }
            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21298, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21302, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21299, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21303, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21300, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21304, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21301, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21305, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }
            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21306, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21310, 0, 0, 32, 20, 0, height, 0, 6,
                        height + 23);
                    wooden_a_supports_paint_setup(session, 6, 1, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21307, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21311, 0, 0, 32, 20, 0, height, 0, 6,
                        height + 23);
                    wooden_a_supports_paint_setup(session, 7, 2, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21308, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21312, 0, 0, 32, 20, 0, height, 0, 6,
                        height + 23);
                    wooden_a_supports_paint_setup(session, 6, 3, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21309, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21313, 0, 0, 32, 20, 0, height, 0, 6,
                        height + 23);
                    wooden_a_supports_paint_setup(session, 7, 4, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
            }
            else
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_2);
            }
            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x00876A4C */
static void heartline_twister_rc_track_left_heartline_roll(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21410, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21422, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21413, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21425, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21416, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21428, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21419, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21431, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21411, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21423, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21414, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21426, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21417, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21429, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21420, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21432, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }
            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21412, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21424, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21415, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21427, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21418, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21430, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21421, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21433, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }
            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21418, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21430, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21421, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21433, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21412, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21424, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21415, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21427, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }
            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21417, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21429, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21420, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21432, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21411, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21423, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21414, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21426, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }
            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21416, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21428, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21419, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21431, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21410, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21422, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21413, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21425, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x00876A5C */
static void heartline_twister_rc_track_right_heartline_roll(
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
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21434, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21446, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21437, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21449, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21440, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21452, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21443, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21455, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21435, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21447, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21438, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21450, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21441, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21453, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21444, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21456, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }
            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21436, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21448, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21439, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21451, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21442, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21454, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21445, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21457, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }
            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21442, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21454, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21445, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21457, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21436, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21448, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21439, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21451, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }
            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21441, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21453, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21444, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21456, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21435, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21447, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21438, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21450, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }
            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21440, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21452, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21443, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21455, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21434, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21446, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21437, 0, 0, 32, 20, 2, height, 0, 6, height);
                    sub_98197C_rotated(
                        session, direction, session->TrackColours[SCHEME_TRACK] | 21449, 0, 0, 32, 1, 26, height, 0, 27,
                        height);
                    wooden_a_supports_paint_setup(session, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 32, 0x20);
            break;
    }
}

TRACK_PAINT_FUNCTION get_track_paint_function_heartline_twister_rc(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return heartline_twister_rc_track_flat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return heartline_twister_rc_track_station;
        case TrackElemType::Up25:
            return heartline_twister_rc_track_25_deg_up;
        case TrackElemType::Up60:
            return heartline_twister_rc_track_60_deg_up;
        case TrackElemType::FlatToUp25:
            return heartline_twister_rc_track_flat_to_25_deg_up;
        case TrackElemType::Up25ToUp60:
            return heartline_twister_rc_track_25_deg_up_to_60_deg_up;
        case TrackElemType::Up60ToUp25:
            return heartline_twister_rc_track_60_deg_up_to_25_deg_up;
        case TrackElemType::Up25ToFlat:
            return heartline_twister_rc_track_25_deg_up_to_flat;
        case TrackElemType::Down25:
            return heartline_twister_rc_track_25_deg_down;
        case TrackElemType::Down60:
            return heartline_twister_rc_track_60_deg_down;
        case TrackElemType::FlatToDown25:
            return heartline_twister_rc_track_flat_to_25_deg_down;
        case TrackElemType::Down25ToDown60:
            return heartline_twister_rc_track_25_deg_down_to_60_deg_down;
        case TrackElemType::Down60ToDown25:
            return heartline_twister_rc_track_60_deg_down_to_25_deg_down;
        case TrackElemType::Down25ToFlat:
            return heartline_twister_rc_track_25_deg_down_to_flat;
        case TrackElemType::HeartLineTransferUp:
            return heartline_twister_rc_track_heartline_transfer_up;
        case TrackElemType::HeartLineTransferDown:
            return heartline_twister_rc_track_heartline_transfer_down;
        case TrackElemType::LeftHeartLineRoll:
            return heartline_twister_rc_track_left_heartline_roll;
        case TrackElemType::RightHeartLineRoll:
            return heartline_twister_rc_track_right_heartline_roll;
    }
    return nullptr;
}
