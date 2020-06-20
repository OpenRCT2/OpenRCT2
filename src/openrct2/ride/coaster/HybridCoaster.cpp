#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https//:ggithub.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../../drawing/Drawing.h"
#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../../sprites.h"
#include "../../world/Map.h"
#include "../../world/Sprite.h"
#include "../RideData.h"
#include "../TrackData.h"
#include "../TrackPaint.h"

//Glitches to fix
//Gentle to steep slope over diagonal bank
//Flickering on steep slopes

//Sloped turns
//S bend clipping



static uint32_t hybrid_rc_get_track_colour(paint_session* session)
{
        if(session->TrackColours[SCHEME_TRACK] == 0x21600000)return 0x21600000; //TODO dirty hack
        else return (session->TrackColours[SCHEME_TRACK] & ~0x1F000000) | ((session->TrackColours[SCHEME_SUPPORTS] & 0xF80000)<<5);
}


static void hybrid_rc_track_flat(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
                               const TileElement * tileElement)
{
    
    if (tileElement->AsTrack()->HasChain())
    {
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_FLAT + 0), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_FLAT + 1), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_FLAT + 2), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_FLAT + 3), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        }
    }
    else
    {
        switch (direction)
        {
        case 0:
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT + 0), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        case 1:
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT + 1), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        }
    }
wooden_a_supports_paint_setup(session, direction & 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

static void hybrid_rc_track_station(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
                                  const TileElement * tileElement)
{
    static constexpr const uint32_t imageIds[4][3] = {
        { (SPR_G2_HYBRID_TRACK_BRAKE + 0), (SPR_G2_HYBRID_TRACK_BLOCK_BRAKE + 0), SPR_STATION_BASE_A_SW_NE },
        { (SPR_G2_HYBRID_TRACK_BRAKE + 1), (SPR_G2_HYBRID_TRACK_BLOCK_BRAKE + 1), SPR_STATION_BASE_A_NW_SE },
        { (SPR_G2_HYBRID_TRACK_BRAKE + 0), (SPR_G2_HYBRID_TRACK_BLOCK_BRAKE + 0), SPR_STATION_BASE_A_SW_NE },
        { (SPR_G2_HYBRID_TRACK_BRAKE + 1), (SPR_G2_HYBRID_TRACK_BLOCK_BRAKE + 1), SPR_STATION_BASE_A_NW_SE },
    };

    if (tileElement->AsTrack()->GetType() == TrackElemType::EndStation)
    {
        sub_98197C_rotated(session, direction, imageIds[direction][1] | hybrid_rc_get_track_colour(session), 0, 0, 32, 20, 1,
                           height, 0, 6, height + 3);
    }
    else
    {
        sub_98197C_rotated(session, direction, imageIds[direction][0] | hybrid_rc_get_track_colour(session), 0, 0, 32, 20, 1,
                           height, 0, 6, height + 3);
    }
    
wooden_a_supports_paint_setup(session, direction & 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
   
    auto ride = get_ride(rideIndex);
    if (ride != nullptr)
        track_paint_util_draw_station_platform(session, ride, direction, height, 9, tileElement);
    
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

static void hybrid_rc_track_25_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                    int32_t height, const TileElement * tileElement)
{
    if (tileElement->AsTrack()->HasChain())
    {
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE + 8), 0, 0, 32, 20, 2, height, 0, 6,
                               height+3);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE + 9), 0, 0, 32, 20, 2, height, 0, 6,
                               height+3);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE + 10), 0, 0, 32, 20, 2, height, 0, 6,
                               height+3);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE + 11), 0, 0, 32, 20, 2, height, 0, 6,
                               height+3);
            break;
        }
    }
    else
    {
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE + 8), 0, 0, 32, 20, 2, height, 0, 6,
                               height+3);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE + 9), 0, 0, 32, 20, 2, height, 0, 6,
                               height+3);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE + 10), 0, 0, 32, 20, 2, height, 0, 6,
                               height+3);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE + 11), 0, 0, 32, 20, 2, height, 0, 6,
                               height+3);
            break;
        }
    }
wooden_a_supports_paint_setup(session, direction & 1, 9 + direction, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_8);
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 56, 0x20);
}

static void hybrid_rc_track_60_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                    int32_t height, const TileElement * tileElement)
{
    if (tileElement->AsTrack()->HasChain())
    {
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP + 12), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        case 1:
            session->WoodenSupportsPrependTo = sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP + 13), 0, 0, 2, 24, 93, height, 28, 4,
                               height - 16);
            break;
        case 2:
            session->WoodenSupportsPrependTo = sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP + 14), 0, 0, 2, 24, 93, height, 28, 4,
                               height - 16);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP + 15), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        }
    }
    else
    {
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP + 12), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        case 1:
            session->WoodenSupportsPrependTo = sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP + 13), 0, 0, 2, 24, 93, height, 28, 4,
                               height - 16);
            break;
        case 2:
            session->WoodenSupportsPrependTo = sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP + 14), 0, 0, 2, 24, 93, height, 28, 4,
                               height - 16);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP + 15), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        }
    }
wooden_a_supports_paint_setup(session, direction & 1, 21 + direction, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 56, TUNNEL_SQUARE_8);
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 104, 0x20);
}

static void hybrid_rc_track_flat_to_25_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                            int32_t height, const TileElement * tileElement)
{
    if (tileElement->AsTrack()->HasChain())
    {
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE + 0), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE + 1), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE + 2), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE + 3), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        }
    }
    else
    {
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE + 0), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE + 1), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE + 2), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE + 3), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        }
    }
wooden_a_supports_paint_setup(session, direction & 1, 1 + direction, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_8);
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

static void hybrid_rc_track_25_deg_up_to_60_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                                 int32_t height, const TileElement * tileElement)
{
    if (tileElement->AsTrack()->HasChain())
    {
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP + 0), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        case 1:
            session->WoodenSupportsPrependTo = sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP + 1), 0, 0, 32, 20, 3, height, 0, 3,
                               height+3);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP + 2), 0, 0, 32, 2, 43, height, 0, 4,
                               height);
            break;
        case 2:
            session->WoodenSupportsPrependTo = sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP + 3), 0, 0, 32, 20, 3, height, 0, 3,
                               height+3);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP + 4), 0, 0, 32, 2, 43, height, 0, 4,
                               height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP + 5), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        }
    }
    else
    {
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP + 0), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        case 1:
            session->WoodenSupportsPrependTo = sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP + 1), 0, 0, 32, 20, 3, height, 0, 6,
                               height+3);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP + 2), 0, 0, 32, 2, 43, height, 0, 4,
                               height);
            break;
        case 2:
            session->WoodenSupportsPrependTo = sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP + 3), 0, 0, 32, 20, 3, height, 0, 6,
                               height+3);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP + 4), 0, 0, 32, 2, 43, height, 0, 4,
                               height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP + 5), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        }
    }
wooden_a_supports_paint_setup(session, direction & 1, 13 + direction, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 24, TUNNEL_SQUARE_8);
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 72, 0x20);
}

static void hybrid_rc_track_60_deg_up_to_25_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                                 int32_t height, const TileElement * tileElement)
{
    if (tileElement->AsTrack()->HasChain())
    {
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP + 6), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        case 1:
            session->WoodenSupportsPrependTo = sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP + 7), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP + 8), 0, 0, 32, 1, 66, height, 0, 27,
                               height);
            break;
        case 2:
            session->WoodenSupportsPrependTo = sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP + 9), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP + 10), 0, 0, 32, 1, 66, height, 0, 27,
                               height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP + 11), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        }
    }
    else
    {
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP + 6), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        case 1:
            session->WoodenSupportsPrependTo = sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP + 7), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP + 8), 0, 0, 32, 1, 66, height, 0, 27,
                               height);
            break;
        case 2:
            session->WoodenSupportsPrependTo = sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP + 9), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP + 10), 0, 0, 32, 1, 66, height, 0, 27,
                               height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP + 11), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        }
    }
wooden_a_supports_paint_setup(session, direction & 1, 17 + direction, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 24, TUNNEL_SQUARE_8);
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 72, 0x20);
}

static void hybrid_rc_track_25_deg_up_to_flat(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                            int32_t height, const TileElement * tileElement)
{
    if (tileElement->AsTrack()->HasChain())
    {
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE + 4), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE + 5), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE + 6), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE + 7), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        }
    }
    else
    {
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE + 4), 0, 0, 32, 20, 2, height, 0, 6,
                               height+3);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE + 5), 0, 0, 32, 20, 2, height, 0, 6,
                               height+3);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE + 6), 0, 0, 32, 20, 2, height, 0, 6,
                               height+3);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE + 7), 0, 0, 32, 20, 2, height, 0, 6,
                               height+3);
            break;
        }
    }
wooden_a_supports_paint_setup(session, direction & 1, 5 + direction, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_14);
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 40, 0x20);
}

static void hybrid_rc_track_25_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                      int32_t height, const TileElement * tileElement)
{
    hybrid_rc_track_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

static void hybrid_rc_track_60_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                      int32_t height, const TileElement * tileElement)
{
    hybrid_rc_track_60_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

static void hybrid_rc_track_flat_to_25_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                              int32_t height, const TileElement * tileElement)
{
    hybrid_rc_track_25_deg_up_to_flat(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

static void hybrid_rc_track_25_deg_down_to_60_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                     uint8_t direction, int32_t height, const TileElement * tileElement)
{
    hybrid_rc_track_60_deg_up_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

static void hybrid_rc_track_60_deg_down_to_25_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                     uint8_t direction, int32_t height, const TileElement * tileElement)
{
    hybrid_rc_track_25_deg_up_to_60_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

static void hybrid_rc_track_25_deg_down_to_flat(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                              int32_t height, const TileElement * tileElement)
{
    hybrid_rc_track_flat_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

static void hybrid_rc_track_90_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                        int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_VERTICAL+8), 0, 0, 2, 20, 31, height, 4, 6,
                               height + 8);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_VERTICAL+9), 0, 0, 2, 20, 31, height, 24, 6,
                               height + 8);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_VERTICAL+10), 0, 0, 2, 20, 31, height, 24, 6,
                               height + 8);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_VERTICAL+11), 0, 0, 2, 20, 31, height, 4, 6,
                               height + 8);
            break;
        }
        paint_util_set_vertical_tunnel(session, height + 32);
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        break;
    }
}

static void hybrid_rc_track_90_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                          int32_t height, const TileElement * tileElement)
{
    hybrid_rc_track_90_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

static void hybrid_rc_track_60_deg_up_to_90_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                     uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_VERTICAL+0), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_VERTICAL+1), 0, 0, 2, 20, 55, height, 24, 6,
                               height);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_VERTICAL+2), 0, 0, 2, 20, 55, height, 24, 6,
                               height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_VERTICAL+3), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        }
wooden_a_supports_paint_setup(session, direction & 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
        }
        paint_util_set_vertical_tunnel(session, height + 56);
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 1:
        break;
    }
}

static void hybrid_rc_track_90_deg_down_to_60_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                         uint8_t direction, int32_t height, const TileElement * tileElement)
{
    hybrid_rc_track_60_deg_up_to_90_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

static void hybrid_rc_track_90_deg_up_to_60_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                     uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (direction)
    {
    case 0:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_VERTICAL+4), 0, 0, 32, 20, 3, height, 0, 6,
                           height + 8);
        break;
    case 1:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_VERTICAL+5), 0, 0, 2, 20, 31, height, 24, 6,
                           height + 8);
        break;
    case 2:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_VERTICAL+6), 0, 0, 2, 20, 31, height, 24, 6,
                           height + 8);
        break;
    case 3:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_VERTICAL+7), 0, 0, 32, 20, 3, height, 0, 6,
                           height + 8);
        break;
    }
    switch (direction)
    {
    case 1:
        paint_util_push_tunnel_right(session, height + 48, TUNNEL_SQUARE_8);
        break;
    case 2:
        paint_util_push_tunnel_left(session, height + 48, TUNNEL_SQUARE_8);
        break;
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 80, 0x20);
}

static void hybrid_rc_track_60_deg_down_to_90_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                         uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_VERTICAL+6), 0, 0, 2, 20, 31, height, 24, 6,
                               height + 8);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_VERTICAL+7), 0, 0, 32, 20, 3, height, 0, 6,
                               height + 8);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_VERTICAL+4), 0, 0, 32, 20, 3, height, 0, 6,
                               height + 8);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_VERTICAL+5), 0, 0, 2, 20, 31, height, 24, 6,
                               height + 8);
            break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height + 48, TUNNEL_SQUARE_8);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 80, 0x20);
        break;
    case 1:
        break;
    }
}

static void hybrid_rc_track_left_quarter_turn_3(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                              int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_CURVE+0), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_CURVE+3), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_CURVE+6), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_CURVE+9), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_CURVE+1), 0, 0, 16, 16, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_CURVE+4), 0, 0, 16, 16, 3, height, 0, 0,
                               height);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_CURVE+7), 0, 0, 16, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_CURVE+10), 0, 0, 16, 16, 3, height, 16, 16,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
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
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_CURVE+2), 0, 0, 32, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_CURVE+5), 0, 0, 32, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_CURVE+8), 0, 0, 32, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_CURVE+11), 0, 0, 32, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        switch (direction)
        {
        case 2:
            paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
            break;
        case 3:
            paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

static void hybrid_rc_track_right_quarter_turn_3(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                               int32_t height, const TileElement * tileElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    hybrid_rc_track_left_quarter_turn_3(session, rideIndex, trackSequence, (direction - 1) & 3, height, tileElement);
}


static void hybrid_rc_track_left_quarter_turn_5(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                              int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE+0), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE+5), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE+10), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE+15), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
	paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
	paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, (direction+1) & 3), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE+1), 0, 0, 32, 16, 3, height, 0, 0,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE+6), 0, 0, 36, 16, 3, height, 0, 4,
                               height);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE+11), 0, 0, 32, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE+16), 0, 0, 32, 14, 3, height, 0, 18,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session,paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, (direction+1) & 3), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE+2), 0, 0, 16, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE+7), 0, 0, 20, 20, 3, height, 16, 16,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE+12), 0, 0, 16, 16, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE+17), 0, 0, 33, 33, 3, height, 0, 0,
                               height);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, (direction+1) & 3), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 4:
	paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, (direction+1) & 3), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 5:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE+3), 0, 0, 16, 32, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE+8), 0, 0, 16, 36, 3, height, 4, 0,
                               height);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE+13), 0, 0, 16, 32, 3, height, 0, 0,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE+18), 0, 0, 14, 32, 3, height, 18, 0,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session,paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, (direction+1) & 3), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 6:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE+4), 0, 0, 32, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE+9), 0, 0, 32, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE+14), 0, 0, 32, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE+19), 0, 0, 32, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        switch (direction)
        {
        case 2:
            paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
            break;
        case 3:
            paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

static void hybrid_rc_track_right_quarter_turn_5(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                               int32_t height, const TileElement * tileElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    hybrid_rc_track_left_quarter_turn_5(session, rideIndex, trackSequence, (direction - 1) & 3, height, tileElement);
}

static void hybrid_rc_track_left_eighth_to_diag(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                              int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE+0), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE+4), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE+8), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE+12), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE+1), 0, 0, 32, 16, 3, height, 0, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE+5), 0, 0, 34, 16, 3, height, 0, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE+9), 0, 0, 32, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE+13), 0, 0, 32, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENTS_ALL, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE+2), 0, 0, 16, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE+6), 0, 0, 16, 16, 3, height, 16, 16,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE+10), 0, 0, 16, 16, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE+14), 0, 0, 34, 16, 3, height, 0, 0,
                               height);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
            switch (direction)
            {
                case 0:
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 4:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE+3), 0, 0, 16, 16, 3, height, 16, 16,
                               height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE+7), 0, 0, 16, 18, 3, height, 0, 16+8,
                              height);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE+11), 0, 0, 16, 16, 3, height, 0, 0,
                               height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE+15), 0, 0, 16, 16, 3, height, 16, 0,
                               height);
            break;
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENTS_ALL, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

static void hybrid_rc_track_right_eighth_to_diag(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                               int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE+16), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE+20), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE+24), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE+28), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE+17), 0, 0, 32, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE+21), 0, 0, 32, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE+25), 0, 0, 34, 16, 3, height, 0, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE+29), 0, 0, 32, 16, 3, height, 0, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENTS_ALL, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE+18), 0, 0, 34, 16, 3, height, 0, 0,
                               height);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE+22), 0, 0, 16, 16, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE+26), 0, 0, 28, 28, 3, height, 4, 4,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE+30), 0, 0, 16, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
            switch (direction)
            {
                case 0:
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 4:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE+19), 0, 0, 16, 16, 3, height, 16, 0,
                               height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE+23), 0, 0, 16, 16, 3, height, 0, 0,
                               height);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE+27), 0, 0, 16, 18, 3, height, 0, 16,
                               height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE+31), 0, 0, 16, 16, 3, height, 16, 16,
                               height);
            break;
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENTS_ALL, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

static void hybrid_rc_track_left_eighth_to_orthogonal(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                    uint8_t direction, int32_t height, const TileElement * tileElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    hybrid_rc_track_right_eighth_to_diag(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

static void hybrid_rc_track_right_eighth_to_orthogonal(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                     uint8_t direction, int32_t height, const TileElement * tileElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    hybrid_rc_track_left_eighth_to_diag(session, rideIndex, trackSequence, (direction + 3) & 3, height, tileElement);
}

static void hybrid_rc_track_diag_flat(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                    int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        if (tileElement->AsTrack()->HasChain())
        {
            switch (direction)
            {
            case 3:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_FLAT_DIAGONAL+3), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        else
        {
            switch (direction)
            {
            case 3:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_DIAGONAL+3), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
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
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_FLAT_DIAGONAL+0), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                break;
            }
        }
        else
        {
            switch (direction)
            {
            case 0:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_DIAGONAL+0), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        switch(direction)
	{
        case 0:
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
        case 1:
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
        case 2:
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
        case 3:
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        if (tileElement->AsTrack()->HasChain())
        {
            switch (direction)
            {
            case 2:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_FLAT_DIAGONAL+2), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        else
        {
            switch (direction)
            {
            case 2:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_DIAGONAL+2), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        switch(direction)
	{
        case 0:
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
        case 1:
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
        case 2:
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
        case 3:
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        if (tileElement->AsTrack()->HasChain())
        {
            switch (direction)
            {
            case 1:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_FLAT_DIAGONAL+1), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        else
        {
            switch (direction)
            {
            case 1:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_DIAGONAL+1), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

static void hybrid_rc_track_diag_25_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                         int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        if (tileElement->AsTrack()->HasChain())
        {
            switch (direction)
            {
            case 3:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE_DIAGONAL+11), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        else
        {
            switch (direction)
            {
            case 3:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_DIAGONAL+11), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 1:
        if (tileElement->AsTrack()->HasChain())
        {
            switch (direction)
            {
            case 0:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE_DIAGONAL+8), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        else
        {
            switch (direction)
            {
            case 0:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_DIAGONAL+8), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        switch(direction)
	{
        case 0:
            wooden_b_supports_paint_setup(session, 2, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
        case 1:
            wooden_b_supports_paint_setup(session, 3, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
        case 2:
            wooden_b_supports_paint_setup(session, 4, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
        case 3:
            wooden_b_supports_paint_setup(session, 5, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 2:
        if (tileElement->AsTrack()->HasChain())
        {
            switch (direction)
            {
            case 2:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE_DIAGONAL+10), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        else
        {
            switch (direction)
            {
            case 2:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_DIAGONAL+10), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        switch(direction)
	{
        case 0:
            wooden_b_supports_paint_setup(session, 4, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
        case 1:
            wooden_b_supports_paint_setup(session, 5, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
        case 2:
            wooden_b_supports_paint_setup(session, 2, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
        case 3:
            wooden_b_supports_paint_setup(session, 3, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 3:
        if (tileElement->AsTrack()->HasChain())
        {
            switch (direction)
            {
            case 1:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE_DIAGONAL+9), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        else
        {
            switch (direction)
            {
            case 1:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_DIAGONAL+1), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    }
}

static void hybrid_rc_track_diag_25_deg_up_to_flat(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                                 int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        if (tileElement->AsTrack()->HasChain())
        {
            switch (direction)
            {
            case 3:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE_DIAGONAL+7), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        else
        {
            switch (direction)
            {
            case 3:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_DIAGONAL+7), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 1:
        if (tileElement->AsTrack()->HasChain())
        {
            switch (direction)
            {
            case 0:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE_DIAGONAL+4), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        else
        {
            switch (direction)
            {
            case 0:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_DIAGONAL+4), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 2, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 1:                                                     
wooden_b_supports_paint_setup(session, 3, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 2:                                                     
wooden_b_supports_paint_setup(session, 4, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 3:                                                     
wooden_b_supports_paint_setup(session, 5, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 2:
        if (tileElement->AsTrack()->HasChain())
        {
            switch (direction)
            {
            case 2:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE_DIAGONAL+6), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        else
        {
            switch (direction)
            {
            case 2:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_DIAGONAL+6), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 4, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 1:                                                     
wooden_b_supports_paint_setup(session, 5, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 2:                                                     
wooden_b_supports_paint_setup(session, 2, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 3:                                                     
wooden_b_supports_paint_setup(session, 3, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 3:
        if (tileElement->AsTrack()->HasChain())
        {
            switch (direction)
            {
            case 1:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE_DIAGONAL+5), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        else
        {
            switch (direction)
            {
            case 1:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_DIAGONAL+5), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    }
}

static void hybrid_rc_track_diag_flat_to_25_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                                 int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        if (tileElement->AsTrack()->HasChain())
        {
            switch (direction)
            {
            case 3:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE_DIAGONAL+3), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        else
        {
            switch (direction)
            {
            case 3:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_DIAGONAL+3), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 1:
        if (tileElement->AsTrack()->HasChain())
        {
            switch (direction)
            {
            case 0:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE_DIAGONAL+0), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        else
        {
            switch (direction)
            {
            case 0:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_DIAGONAL+0), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
        case 1:
wooden_b_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
        case 2:
wooden_b_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
        case 3:
wooden_b_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 2:
        if (tileElement->AsTrack()->HasChain())
        {
            switch (direction)
            {
            case 2:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE_DIAGONAL+2), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        else
        {
            switch (direction)
            {
            case 2:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_DIAGONAL+2), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
        case 1:
wooden_b_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
        case 2:
wooden_b_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
        case 3:
wooden_b_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 3:
        if (tileElement->AsTrack()->HasChain())
        {
            switch (direction)
            {
            case 1:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE_DIAGONAL+1), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        else
        {
            switch (direction)
            {
            case 1:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_DIAGONAL+1), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    }
}

static void hybrid_rc_track_diag_25_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                           int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        if (tileElement->AsTrack()->HasChain())
        {
            switch (direction)
            {
            case 3:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE_DIAGONAL+9), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        else
        {
            switch (direction)
            {
            case 3:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_DIAGONAL+9), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 1:
        if (tileElement->AsTrack()->HasChain())
        {
            switch (direction)
            {
            case 0:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE_DIAGONAL+10), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        else
        {
            switch (direction)
            {
            case 0:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_DIAGONAL+10), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 2, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 1:                                                     
wooden_b_supports_paint_setup(session, 3, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 2:                                                     
wooden_b_supports_paint_setup(session, 4, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 3:                                                     
wooden_b_supports_paint_setup(session, 5, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 2:
        if (tileElement->AsTrack()->HasChain())
        {
            switch (direction)
            {
            case 2:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE_DIAGONAL+8), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        else
        {
            switch (direction)
            {
            case 2:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_DIAGONAL+8), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 4, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 1:                                                     
wooden_b_supports_paint_setup(session, 5, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 2:                                                     
wooden_b_supports_paint_setup(session, 2, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 3:                                                     
wooden_b_supports_paint_setup(session, 3, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 3:
        if (tileElement->AsTrack()->HasChain())
        {
            switch (direction)
            {
            case 1:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE_DIAGONAL+11), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        else
        {
            switch (direction)
            {
            case 1:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_DIAGONAL+11), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    }
}

static void hybrid_rc_track_diag_flat_to_25_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                   uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        if (tileElement->AsTrack()->HasChain())
        {
            switch (direction)
            {
            case 3:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE_DIAGONAL+5), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        else
        {
            switch (direction)
            {
            case 3:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_DIAGONAL+5), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        break;
    case 1:
        if (tileElement->AsTrack()->HasChain())
        {
            switch (direction)
            {
            case 0:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE_DIAGONAL+6), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        else
        {
            switch (direction)
            {
            case 0:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_DIAGONAL+6), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 2, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 1:                                                     
wooden_b_supports_paint_setup(session, 3, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 2:                                                     
wooden_b_supports_paint_setup(session, 4, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 3:                                                     
wooden_b_supports_paint_setup(session, 5, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        break;
    case 2:
        if (tileElement->AsTrack()->HasChain())
        {
            switch (direction)
            {
            case 2:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE_DIAGONAL+4), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        else
        {
            switch (direction)
            {
            case 2:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_DIAGONAL+4), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 4, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 1:                                                     
wooden_b_supports_paint_setup(session, 5, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 2:                                                     
wooden_b_supports_paint_setup(session, 2, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 3:                                                     
wooden_b_supports_paint_setup(session, 3, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        break;
    case 3:
        if (tileElement->AsTrack()->HasChain())
        {
            switch (direction)
            {
            case 1:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE_DIAGONAL+7), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        else
        {
            switch (direction)
            {
            case 1:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_DIAGONAL+7), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        break;
    }

    paint_util_set_general_support_height(session, height + 56, 0x20);
}

static void hybrid_rc_track_diag_25_deg_down_to_flat(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                   uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        if (tileElement->AsTrack()->HasChain())
        {
            switch (direction)
            {
            case 3:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE_DIAGONAL+1), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        else
        {
            switch (direction)
            {
            case 3:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_DIAGONAL+1), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 1:
        if (tileElement->AsTrack()->HasChain())
        {
            switch (direction)
            {
            case 0:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE_DIAGONAL+2), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        else
        {
            switch (direction)
            {
            case 0:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_DIAGONAL+2), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
        case 1:
wooden_b_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
        case 2:
wooden_b_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
        case 3:
wooden_b_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 2:
        if (tileElement->AsTrack()->HasChain())
        {
            switch (direction)
            {
            case 2:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE_DIAGONAL+0), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        else
        {
            switch (direction)
            {
            case 2:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_DIAGONAL+0), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
        case 1:
wooden_b_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
        case 2:
wooden_b_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
        case 3:
wooden_b_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 3:
        if (tileElement->AsTrack()->HasChain())
        {
            switch (direction)
            {
            case 1:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_GENTLE_DIAGONAL+3), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        else
        {
            switch (direction)
            {
            case 1:
                sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_DIAGONAL+3), -16, -16, 32, 32, 3, height,
                                   -16, -16, height);
                break;
            }
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    }
}

static void hybrid_rc_track_diag_60_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                         int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        if (tileElement->AsTrack()->HasChain())
	{	
		switch (direction)
		{
		case 3:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP_DIAGONAL+11), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
	else
	{
		switch (direction)
		{
		case 3:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_DIAGONAL+11), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 104, 0x20);
        break;
    case 1:
        if (tileElement->AsTrack()->HasChain())
        {
		switch (direction)
		{
		case 0:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP_DIAGONAL+8), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
	else
	{
		switch (direction)
		{
		case 0:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_DIAGONAL+8), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 2, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 1:                                                     
wooden_b_supports_paint_setup(session, 3, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 2:                                                     
wooden_b_supports_paint_setup(session, 4, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 3:                                                     
wooden_b_supports_paint_setup(session, 5, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 104, 0x20);
        break;
    case 2:
        if (tileElement->AsTrack()->HasChain())
        {
		switch (direction)
		{
		case 2:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP_DIAGONAL+10), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
	else
	{
		switch (direction)
		{
		case 2:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_DIAGONAL+10), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 4, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 1:                                                     
wooden_b_supports_paint_setup(session, 5, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 2:                                                     
wooden_b_supports_paint_setup(session, 2, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 3:                                                     
wooden_b_supports_paint_setup(session, 3, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 104, 0x20);
        break;
    case 3:
        if (tileElement->AsTrack()->HasChain())
        {
		switch (direction)
		{
		case 1:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP_DIAGONAL+9), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
	else
	{
		switch (direction)
		{
		case 1:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_DIAGONAL+9), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 104, 0x20);
        break;
    }
}

static void hybrid_rc_track_diag_25_deg_up_to_60_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                      uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        if (tileElement->AsTrack()->HasChain())
        {
		switch (direction)
		{
		case 3:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP_DIAGONAL+3), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
	else
	{
		switch (direction)
		{
		case 3:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_DIAGONAL+3), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 1:
        if (tileElement->AsTrack()->HasChain())
        {
		switch (direction)
		{
		case 0:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP_DIAGONAL+0), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
	else
	{
		switch (direction)
		{
		case 0:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_DIAGONAL+0), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 2, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 1:                                                     
wooden_b_supports_paint_setup(session, 3, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 2:                                                     
wooden_b_supports_paint_setup(session, 4, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 3:                                                     
wooden_b_supports_paint_setup(session, 5, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 2:
        if (tileElement->AsTrack()->HasChain())
        {
		switch (direction)
		{
		case 2:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP_DIAGONAL+2), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
	else
	{
		switch (direction)
		{
		case 2:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_DIAGONAL+2), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 4, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 1:                                                     
wooden_b_supports_paint_setup(session, 5, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 2:                                                     
wooden_b_supports_paint_setup(session, 2, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 3:                                                     
wooden_b_supports_paint_setup(session, 3, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 3:
        if (tileElement->AsTrack()->HasChain())
        {
		switch (direction)
		{
		case 1:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP_DIAGONAL+1), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
	else
	{
		switch (direction)
		{
		case 1:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_DIAGONAL+1), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    }
}

static void hybrid_rc_track_diag_60_deg_up_to_25_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                      uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        if (tileElement->AsTrack()->HasChain())
        {
		switch (direction)
		{
		case 3:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP_DIAGONAL+7), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
	else
	{
		switch (direction)
		{
		case 3:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_DIAGONAL+7), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 1:
        if (tileElement->AsTrack()->HasChain())
        {
		switch (direction)
		{
		case 0:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP_DIAGONAL+4), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
	else
	{
		switch (direction)
		{
		case 0:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_DIAGONAL+4), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 2, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 1:                                                     
wooden_b_supports_paint_setup(session, 3, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 2:                                                     
wooden_b_supports_paint_setup(session, 4, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 3:                                                     
wooden_b_supports_paint_setup(session, 5, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 2:
        if (tileElement->AsTrack()->HasChain())
        {
		switch (direction)
		{
		case 2:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP_DIAGONAL+6), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
	else
	{
		switch (direction)
		{
		case 2:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_DIAGONAL+6), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 4, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 1:                                                     
wooden_b_supports_paint_setup(session, 5, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 2:                                                     
wooden_b_supports_paint_setup(session, 2, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 3:                                                     
wooden_b_supports_paint_setup(session, 3, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 3:
        if (tileElement->AsTrack()->HasChain())
        {
		switch (direction)
		{
		case 1:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP_DIAGONAL+5), -16, -16, 16, 16, 3, height, 0,
				       0, height);
		    break;
		}
	}
	else
	{
		switch (direction)
		{
		case 1:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_DIAGONAL+5), -16, -16, 16, 16, 3, height, 0,
				       0, height);
		    break;
		}
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    }
}

static void hybrid_rc_track_diag_60_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                           int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        if (tileElement->AsTrack()->HasChain())
        {
		switch (direction)
		{
		case 3:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP_DIAGONAL+9), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
	else
	{
		switch (direction)
		{
		case 3:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_DIAGONAL+9), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 104, 0x20);
        break;
    case 1:
        if (tileElement->AsTrack()->HasChain())
        {
		switch (direction)
		{
		case 0:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP_DIAGONAL+10), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
	else
	{
		switch (direction)
		{
		case 0:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_DIAGONAL+10), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 2, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 1:                                                     
wooden_b_supports_paint_setup(session, 3, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 2:                                                     
wooden_b_supports_paint_setup(session, 4, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 3:                                                     
wooden_b_supports_paint_setup(session, 5, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 104, 0x20);
        break;
    case 2:
        if (tileElement->AsTrack()->HasChain())
        {
		switch (direction)
		{
		case 2:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP_DIAGONAL+8), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
	else
	{
		switch (direction)
		{
		case 2:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_DIAGONAL+8), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 4, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 1:                                                     
wooden_b_supports_paint_setup(session, 5, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 2:                                                     
wooden_b_supports_paint_setup(session, 2, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 3:                                                     
wooden_b_supports_paint_setup(session, 3, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 104, 0x20);
        break;
    case 3:
        if (tileElement->AsTrack()->HasChain())
        {
		switch (direction)
		{
		case 1:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP_DIAGONAL+11), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
	else
	{
		switch (direction)
		{
		case 1:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_DIAGONAL+11), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 104, 0x20);
        break;
    }
}

static void hybrid_rc_track_diag_25_deg_down_to_60_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                          uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        if (tileElement->AsTrack()->HasChain())
        {
		switch (direction)
		{
		case 3:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP_DIAGONAL+5), -16, -16, 16, 16, 3, height, 0,
				       0, height);
		    break;
		}
	}
	else
	{
		switch (direction)
		{
		case 3:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_DIAGONAL+5), -16, -16, 16, 16, 3, height, 0,
				       0, height);
		    break;
		}
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 1:
        if (tileElement->AsTrack()->HasChain())
        {
		switch (direction)
		{
		case 0:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP_DIAGONAL+6), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
	else
	{
		switch (direction)
		{
		case 0:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_DIAGONAL+6), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 2, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 1:                                                     
wooden_b_supports_paint_setup(session, 3, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 2:                                                     
wooden_b_supports_paint_setup(session, 4, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 3:                                                     
wooden_b_supports_paint_setup(session, 5, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 2:
        if (tileElement->AsTrack()->HasChain())
        {
		switch (direction)
		{
		case 2:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP_DIAGONAL+4), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
	else
	{
		switch (direction)
		{
		case 2:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_DIAGONAL+4), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 4, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 1:                                                     
wooden_b_supports_paint_setup(session, 5, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 2:                                                     
wooden_b_supports_paint_setup(session, 2, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 3:                                                     
wooden_b_supports_paint_setup(session, 3, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 3:
        if (tileElement->AsTrack()->HasChain())
        {
		switch (direction)
		{
		case 1:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP_DIAGONAL+7), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
	else
	{
		switch (direction)
		{
		case 1:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_DIAGONAL+7), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    }
}

static void hybrid_rc_track_diag_60_deg_down_to_25_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                          uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        if (tileElement->AsTrack()->HasChain())
        {
		switch (direction)
		{
		case 3:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP_DIAGONAL+1), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
	else
	{
		switch (direction)
		{
		case 3:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_DIAGONAL+1), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 1:
        if (tileElement->AsTrack()->HasChain())
        {
		switch (direction)
		{
		case 0:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP_DIAGONAL+2), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
	else
	{
		switch (direction)
		{
		case 0:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_DIAGONAL+2), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 2, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 1:                                                     
wooden_b_supports_paint_setup(session, 3, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 2:                                                     
wooden_b_supports_paint_setup(session, 4, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 3:                                                     
wooden_b_supports_paint_setup(session, 5, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 2:
        if (tileElement->AsTrack()->HasChain())
        {
		switch (direction)
		{
		case 2:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP_DIAGONAL+0), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
	else
	{
		switch (direction)
		{
		case 2:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_DIAGONAL+0), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 4, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 1:                                                     
wooden_b_supports_paint_setup(session, 5, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 2:                                                     
wooden_b_supports_paint_setup(session, 2, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 3:                                                     
wooden_b_supports_paint_setup(session, 3, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 3:
        if (tileElement->AsTrack()->HasChain())
        {
		switch (direction)
		{
		case 1:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_LIFT_TRACK_STEEP_DIAGONAL+3), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
	else
	{
		switch (direction)
		{
		case 1:
		    sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_DIAGONAL+3), -16, -16, 32, 32, 3, height,
				       -16, -16, height);
		    break;
		}
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    }
}




static void hybrid_rc_track_flat_to_left_bank(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                            int32_t height, const TileElement * tileElement)
{
    switch (direction)
    {
    case 0:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+0), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+1), 0, 0, 32, 1, 26, height, 0, 27,
                           height);
        break;
    case 1:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+2), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+3), 0, 0, 32, 1, 26, height, 0, 27,
                           height);
        break;
    case 2:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+4), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        break;
    case 3:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+5), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        break;
    }
wooden_a_supports_paint_setup(session, direction & 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

static void hybrid_rc_track_flat_to_right_bank(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                             int32_t height, const TileElement * tileElement)
{
    switch (direction)
    {
    case 0:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+6), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        break;
    case 1:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+7), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        break;
    case 2:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+8), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+9), 0, 0, 32, 1, 26, height, 0, 27,
                           height);
        break;
    case 3:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+10), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+11), 0, 0, 32, 1, 26, height, 0, 27,
                           height);
        break;
    }
wooden_a_supports_paint_setup(session, direction & 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

static void hybrid_rc_track_left_bank_to_flat(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                            int32_t height, const TileElement * tileElement)
{
	hybrid_rc_track_flat_to_right_bank(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

static void hybrid_rc_track_right_bank_to_flat(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                             int32_t height, const TileElement * tileElement)
{
	hybrid_rc_track_flat_to_left_bank(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}


static void hybrid_rc_track_left_bank_to_25_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                                 int32_t height, const TileElement * tileElement)
{
    switch (direction)
    {
    case 0:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+12), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+13), 0, 0, 32, 1, 34, height, 0, 27,
                           height);
        break;
    case 1:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+14), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+15), 0, 0, 32, 1, 34, height, 0, 27,
                           height);
        break;
    case 2:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+16), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        break;
    case 3:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+17), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        break;
    }
wooden_a_supports_paint_setup(session, direction & 1, 1 + direction, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_8);
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

static void hybrid_rc_track_right_bank_to_25_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                  uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (direction)
    {
    case 0:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+18), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        break;
    case 1:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+19), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        break;
    case 2:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+20), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+21), 0, 0, 32, 1, 34, height, 0, 27,
                           height);
        break;
    case 3:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+22), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+23), 0, 0, 32, 1, 34, height, 0, 27,
                           height);
        break;
    }
wooden_a_supports_paint_setup(session, direction & 1, 1 + direction, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_8);
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

static void hybrid_rc_track_25_deg_up_to_left_bank(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                                 int32_t height, const TileElement * tileElement)
{
    switch (direction)
    {
    case 0:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+24), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+25), 0, 0, 32, 1, 34, height, 0, 27,
                           height);
        break;
    case 1:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+26), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+27), 0, 0, 32, 1, 34, height, 0, 27,
                           height);
        break;
    case 2:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+28), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        break;
    case 3:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+29), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        break;
    }
wooden_a_supports_paint_setup(session, direction & 1, 5 + direction, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_14);
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 40, 0x20);
}

static void hybrid_rc_track_25_deg_up_to_right_bank(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                  uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (direction)
    {
    case 0:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+30), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        break;
    case 1:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+31), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        break;
    case 2:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+32), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+33), 0, 0, 32, 1, 34, height, 0, 27,
                           height);
        break;
    case 3:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+34), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+35), 0, 0, 32, 1, 34, height, 0, 27,
                           height);
        break;
    }
wooden_a_supports_paint_setup(session, direction & 1, 5 + direction, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_14);
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 40, 0x20);
}

static void hybrid_rc_track_left_bank_to_25_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                   uint8_t direction, int32_t height, const TileElement * tileElement)
{
    hybrid_rc_track_25_deg_up_to_right_bank(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

static void hybrid_rc_track_right_bank_to_25_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                    uint8_t direction, int32_t height, const TileElement * tileElement)
{
    hybrid_rc_track_25_deg_up_to_left_bank(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

static void hybrid_rc_track_25_deg_down_to_left_bank(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                   uint8_t direction, int32_t height, const TileElement * tileElement)
{
    hybrid_rc_track_right_bank_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

static void hybrid_rc_track_25_deg_down_to_right_bank(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                    uint8_t direction, int32_t height, const TileElement * tileElement)
{
    hybrid_rc_track_left_bank_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

static void hybrid_rc_track_left_bank(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                    int32_t height, const TileElement * tileElement)
{
    switch (direction)
    {
    case 0:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+36), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+37), 0, 0, 32, 1, 26, height, 0, 27,
                           height);
        break;
    case 1:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+38), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+39), 0, 0, 32, 1, 26, height, 0, 27,
                           height);
        break;
    case 2:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+40), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        break;
    case 3:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION+41), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        break;
    }
wooden_a_supports_paint_setup(session, direction & 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

static void hybrid_rc_track_right_bank(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                     int32_t height, const TileElement * tileElement)
{
    hybrid_rc_track_left_bank(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}



static void hybrid_rc_track_diag_flat_to_left_bank(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                                 int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+4), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+0), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+1), -16, -16, 32, 32, 0, height,
                               -16, -16, height + 28);
            break;
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 1:                                                
wooden_b_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 2:                                                
wooden_b_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 3:                                                
wooden_b_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+3), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 1:                                                
wooden_b_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 2:                                                
wooden_b_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 3:                                                
wooden_b_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction)
        {
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+2), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

static void hybrid_rc_track_diag_flat_to_right_bank(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                  uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+9), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+5), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 1:                                                
wooden_b_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 2:                                                
wooden_b_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 3:                                                
wooden_b_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+7), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+8), -16, -16, 32, 32, 0, height,
                               -16, -16, height + 28);
            break;
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 1:                                                
wooden_b_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 2:                                                
wooden_b_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 3:                                                
wooden_b_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction)
        {
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+6), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

static void hybrid_rc_track_diag_left_bank_to_flat(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                                 int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+6), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+7), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+8), -16, -16, 32, 32, 0, height,
                               -16, -16, height + 28);
            break;
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 1:                                                
wooden_b_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 2:                                                
wooden_b_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 3:                                                
wooden_b_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+5), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 1:                                                
wooden_b_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 2:                                                
wooden_b_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 3:                                                
wooden_b_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction)
        {
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+9), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

static void hybrid_rc_track_diag_right_bank_to_flat(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                  uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+2), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+3), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 1:                                                
wooden_b_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 2:                                                
wooden_b_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 3:                                                
wooden_b_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+0), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+1), -16, -16, 32, 32, 0, height,
                               -16, -16, height + 28);
            break;
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 1:                                                
wooden_b_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 2:                                                
wooden_b_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 3:                                                
wooden_b_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction)
        {
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+4), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

static void hybrid_rc_track_diag_left_bank_to_25_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                      uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+14), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 1:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+10), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+11), -16, -16, 32, 32, 0, height,
                               -16, -16, height + 35);
            break;
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 1:                                                
wooden_b_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 2:                                                
wooden_b_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 3:                                                
wooden_b_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+13), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 1:                                                
wooden_b_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 2:                                                
wooden_b_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 3:                                                
wooden_b_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 3:
        switch (direction)
        {
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+12), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    }
}

static void hybrid_rc_track_diag_right_bank_to_25_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                       uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+19), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 1:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+15), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 1:                                                
wooden_b_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 2:                                                
wooden_b_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 3:                                                
wooden_b_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+17), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+18), -16, -16, 32, 32, 0, height,
                               -16, -16, height + 35);
            break;
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 1:                                                
wooden_b_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 2:                                                
wooden_b_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 3:                                                
wooden_b_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 3:
        switch (direction)
        {
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+16), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    }
}

static void hybrid_rc_track_diag_25_deg_up_to_left_bank(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                      uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+24), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 1:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+20), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+21), -16, -16, 32, 32, 0, height,
                               -16, -16, height + 38);
            break;
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 2, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 1:                                                     
wooden_b_supports_paint_setup(session, 3, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 2:                                                     
wooden_b_supports_paint_setup(session, 4, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 3:                                                     
wooden_b_supports_paint_setup(session, 5, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+23), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 4, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 1:                                                     
wooden_b_supports_paint_setup(session, 5, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 2:                                                     
wooden_b_supports_paint_setup(session, 2, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 3:                                                     
wooden_b_supports_paint_setup(session, 3, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 3:
        switch (direction)
        {
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+22), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    }
}

static void hybrid_rc_track_diag_25_deg_up_to_right_bank(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                       uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+29), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 1:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+25), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 2, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 1:                                                     
wooden_b_supports_paint_setup(session, 3, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 2:                                                     
wooden_b_supports_paint_setup(session, 4, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 3:                                                     
wooden_b_supports_paint_setup(session, 5, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+27), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+28), -16, -16, 32, 32, 0, height,
                               -16, -16, height + 38);
            break;
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 4, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 1:                                                     
wooden_b_supports_paint_setup(session, 5, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 2:                                                     
wooden_b_supports_paint_setup(session, 2, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 3:                                                     
wooden_b_supports_paint_setup(session, 3, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 3:
        switch (direction)
        {
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+26), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    }
}

static void hybrid_rc_track_diag_left_bank_to_25_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                        uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+26), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        break;
    case 1:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+27), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+28), -16, -16, 32, 32, 0, height,
                               -16, -16, height + 38);
            break;
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 2, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 1:                                                     
wooden_b_supports_paint_setup(session, 3, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 2:                                                     
wooden_b_supports_paint_setup(session, 4, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 3:                                                     
wooden_b_supports_paint_setup(session, 5, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        break;
    case 2:
        switch (direction)
        {
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+25), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 4, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 1:                                                     
wooden_b_supports_paint_setup(session, 5, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 2:                                                     
wooden_b_supports_paint_setup(session, 2, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 3:                                                     
wooden_b_supports_paint_setup(session, 3, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        break;
    case 3:
        switch (direction)
        {
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+29), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        break;
    }

    paint_util_set_general_support_height(session, height + 56, 0x20);
}

static void hybrid_rc_track_diag_right_bank_to_25_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                         uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+22), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        break;
    case 1:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+23), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 2, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 1:                                                     
wooden_b_supports_paint_setup(session, 3, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 2:                                                     
wooden_b_supports_paint_setup(session, 4, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 3:                                                     
wooden_b_supports_paint_setup(session, 5, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        break;
    case 2:
        switch (direction)
        {
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+20), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+21), -16, -16, 32, 32, 0, height,
                               -16, -16, height + 38);
            break;
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 4, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 1:                                                     
wooden_b_supports_paint_setup(session, 5, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 2:                                                     
wooden_b_supports_paint_setup(session, 2, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                                  
        case 3:                                                     
wooden_b_supports_paint_setup(session, 3, 0, height + 16, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        break;
    case 3:
        switch (direction)
        {
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+24), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        break;
    }

    paint_util_set_general_support_height(session, height + 56, 0x20);
}

static void hybrid_rc_track_diag_25_deg_down_to_left_bank(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                        uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+16), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 1:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+17), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+18), -16, -16, 32, 32, 0, height,
                               -16, -16, height + 38);
            break;
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 1:                                                
wooden_b_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 2:                                                
wooden_b_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 3:                                                
wooden_b_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+15), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 1:                                                
wooden_b_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 2:                                                
wooden_b_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 3:                                                
wooden_b_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 3:
        switch (direction)
        {
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+19), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    }
}

static void hybrid_rc_track_diag_25_deg_down_to_right_bank(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                         uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+12), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 1:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+13), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 1:                                                
wooden_b_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 2:                                                
wooden_b_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 3:                                                
wooden_b_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+10), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+11), -16, -16, 32, 32, 0, height,
                               -16, -16, height + 38);
            break;
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 1:                                                
wooden_b_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 2:                                                
wooden_b_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 3:                                                
wooden_b_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 3:
        switch (direction)
        {
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+14), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    }
}

static void hybrid_rc_track_diag_left_bank(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                         int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+34), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+30), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+31), -16, -16, 32, 32, 0, height,
                               -16, -16, height + 28);
            break;
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 1:                                                
wooden_b_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 2:                                                
wooden_b_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 3:                                                
wooden_b_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+33), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 1:                                                
wooden_b_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 2:                                                
wooden_b_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 3:                                                
wooden_b_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction)
        {
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+32), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

static void hybrid_rc_track_diag_right_bank(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                          int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+32), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+33), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 1:                                                
wooden_b_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 2:                                                
wooden_b_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 3:                                                
wooden_b_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+30), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+31), -16, -16, 32, 32, 0, height,
                               -16, -16, height + 28);
            break;
        }
        switch(direction)
	{
        case 0:
wooden_b_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 1:                                                
wooden_b_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 2:                                                
wooden_b_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;                                             
        case 3:                                                
wooden_b_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    	    break;
	}
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction)
        {
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BANK_TRANSITION_DIAGONAL+34), -16, -16, 32, 32, 3, height,
                               -16, -16, height);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}




          
static void hybrid_rc_track_left_quarter_turn_3_bank(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                   uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_CURVE_BANKED+0), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_CURVE_BANKED+1), 0, 0, 32, 6, 26, height, 0, 27,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_CURVE_BANKED+4), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_CURVE_BANKED+5), 0, 0, 32, 6, 26, height, 0, 27,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_CURVE_BANKED+9), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_CURVE_BANKED+13), 0, 0, 26, 32, 3, height, 6, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_CURVE_BANKED+2), 0, 0, 16, 16, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_CURVE_BANKED+6), 0, 0, 16, 16, 1, height, 0, 0,
                               height + 28);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_CURVE_BANKED+10), 0, 0, 16, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_CURVE_BANKED+14), 0, 0, 22, 22, 3, height, 16, 16,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
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
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_CURVE_BANKED+3), 0, 0, 32, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_CURVE_BANKED+7), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_CURVE_BANKED+8), 0, 0, 6, 32, 26, height, 27, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_CURVE_BANKED+11), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_CURVE_BANKED+12), 0, 0, 6, 32, 26, height, 27, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_CURVE_BANKED+15), 0, 0, 32, 26, 3, height, 6, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        switch (direction)
        {
        case 2:
            paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
            break;
        case 3:
            paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

static void hybrid_rc_track_right_quarter_turn_3_bank(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                    uint8_t direction, int32_t height, const TileElement * tileElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    hybrid_rc_track_left_quarter_turn_3_bank(session, rideIndex, trackSequence, (direction - 1) & 3, height, tileElement);
}
    
static void hybrid_rc_track_banked_left_quarter_turn_5(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                     uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE_BANKED+0), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE_BANKED+1), 0, 0, 32, 6, 26, height, 0, 27,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE_BANKED+6), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE_BANKED+7), 0, 0, 32, 6, 26, height, 0, 27,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE_BANKED+14), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE_BANKED+20), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, (direction+1) & 3), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE_BANKED+2), 0, 0, 32, 16, 3, height, 0, 0,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE_BANKED+8), 0, 0, 48, 16, 1, height, 0, 0,
                               height + 28);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE_BANKED+15), 0, 0, 32, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE_BANKED+21), 0, 0, 32, 14, 3, height, 0, 18,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session,paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, (direction+1) & 3), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE_BANKED+3), 0, 0, 16, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE_BANKED+9), 0, 0, 38, 38, 3, height, 0, 0,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE_BANKED+10), 0, 0, 16, 16, 1, height, 16, 16,
                               height + 28);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE_BANKED+16), 0, 0, 16, 16, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE_BANKED+22), 0, 0, 38, 38, 3, height, 0, 0,
                               height);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, (direction+1) & 3), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 4:
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, (direction+1) & 3), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 5:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE_BANKED+4), 0, 0, 16, 32, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE_BANKED+11), 0, 0, 16, 48, 1, height, 0, 0,
                               height + 28);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE_BANKED+17), 0, 0, 16, 32, 3, height, 0, 0,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE_BANKED+23), 0, 0, 14, 32, 3, height, 18, 0,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session,paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, (direction+1) & 3), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 6:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE_BANKED+5), 0, 0, 32, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE_BANKED+12), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE_BANKED+13), 0, 0, 6, 32, 26, height, 27, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE_BANKED+18), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE_BANKED+19), 0, 0, 6, 32, 26, height, 27, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_MEDIUM_CURVE_BANKED+24), 0, 0, 32, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        switch (direction)
        {
        case 2:
            paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
            break;
        case 3:
            paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

static void hybrid_rc_track_banked_right_quarter_turn_5(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                      uint8_t direction, int32_t height, const TileElement * tileElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    hybrid_rc_track_banked_left_quarter_turn_5(session, rideIndex, trackSequence, (direction - 1) & 3, height, tileElement);
}

static void hybrid_rc_track_left_eighth_bank_to_diag(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                   uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+0), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+1), 0, 0, 32, 6, 26, height, 0, 27,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+5), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+6), 0, 0, 32, 6, 26, height, 0, 27,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+12), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+16), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+2), 0, 0, 32, 16, 3, height, 0, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+7), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+8), 0, 0, 34, 16, 0, height, 0, 0,
                               height + 28);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+13), 0, 0, 32, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+17), 0, 0, 32, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENTS_ALL, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+3), 0, 0, 16, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+9), 0, 0, 16, 16, 0, height, 16, 16,
                               height + 28);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+14), 0, 0, 16, 16, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+18), 0, 0, 34, 16, 3, height, 0, 0,
                               height);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction)
        {
        case 0:
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
        break;
        case 1:
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
        break;
        case 2:
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
        break;
        case 3:
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
        break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 4:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+4), 0, 0, 16, 16, 3, height, 16, 16,
                               height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+10), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+11), 0, 0, 16, 18, 0, height, 0, 16,
                               height + 28);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+15), 0, 0, 16, 16, 3, height, 0, 0,
                               height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+19), 0, 0, 16, 16, 3, height, 16, 0,
                               height);
            break;
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENTS_ALL, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

static void hybrid_rc_track_right_eighth_bank_to_diag(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                    uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+20), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+24), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+28), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+29), 0, 0, 32, 6, 26, height, 0, 27,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+35), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+36), 0, 0, 32, 6, 26, height, 0, 27,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+21), 0, 0, 32, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+25), 0, 0, 32, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+30), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+31), 0, 0, 34, 16, 0, height, 0, 0,
                               height + 28);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+37), 0, 0, 32, 16, 3, height, 0, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENTS_ALL, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+22), 0, 0, 34, 16, 3, height, 0, 0,
                               height);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+26), 0, 0, 16, 16, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+32), 0, 0, 28, 28, 0, height, 4, 4,
                               height + 28);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+38), 0, 0, 16, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
            switch (direction)
            {
                case 0:
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 4:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+23), 0, 0, 16, 16, 3, height, 16, 0,//TODO
                               height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+27), 0, 0, 16, 16, 3, height, 0, 0,
                               height);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+33), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+34), 0, 0, 16, 18, 0, height, 0, 16,
                               height + 28);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_CURVE_BANKED+39), 0, 0, 16, 16, 3, height, 16, 16,
                               height);
            break;
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENTS_ALL, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

static void hybrid_rc_track_left_eighth_bank_to_orthogonal(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                         uint8_t direction, int32_t height, const TileElement * tileElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    hybrid_rc_track_right_eighth_bank_to_diag(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

static void hybrid_rc_track_right_eighth_bank_to_orthogonal(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                          uint8_t direction, int32_t height, const TileElement * tileElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    hybrid_rc_track_left_eighth_bank_to_diag(session, rideIndex, trackSequence, (direction + 3) & 3, height, tileElement);
}




static void hybrid_rc_track_left_quarter_turn_3_25_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                        uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE+0), 0, 6, 32, 20, 3, height);
wooden_a_supports_paint_setup(session, 0, 9, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE+2), 0, 6, 34, 20, 3, height);
wooden_a_supports_paint_setup(session, 1, 10, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE+4), 0, 6, 32, 20, 3, height);
wooden_a_supports_paint_setup(session, 0, 11, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE+6), 0, 6, 32, 20, 3, height);
wooden_a_supports_paint_setup(session, 1, 12, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session,paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 3:
        switch (direction)
        {
        case 0:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE+1), 6, 0, 20, 32, 3, height);
wooden_a_supports_paint_setup(session, 1, 12, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE+3), 6, 0, 20, 34, 3, height);
wooden_a_supports_paint_setup(session, 0, 9, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE+5), 6, 0, 20, 32, 3, height);
wooden_a_supports_paint_setup(session, 1, 10, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE+7), 6, 0, 20, 32, 3, height);
wooden_a_supports_paint_setup(session, 0, 11, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        switch (direction)
        {
        case 2:
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_8);
            break;
        case 3:
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_8);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    }
}
        
static void hybrid_rc_track_right_quarter_turn_3_25_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                         uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE+8), 0, 6, 32, 20, 3, height);
wooden_a_supports_paint_setup(session, 0, 9, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE+10), 0, 6, 32, 20, 3, height);
wooden_a_supports_paint_setup(session, 1, 10, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE+12), 0, 6, 34, 20, 3, height);
wooden_a_supports_paint_setup(session, 0, 11, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE+14), 0, 6, 32, 20, 3, height);
wooden_a_supports_paint_setup(session, 1, 12, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session,paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 3:
        switch (direction)
        {
        case 0:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE+9), 6, 0, 20, 32, 3, height);
wooden_a_supports_paint_setup(session, 1, 10, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE+11), 6, 0, 20, 32, 3, height);
wooden_a_supports_paint_setup(session, 0, 11, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE+13), 6, 0, 20, 34, 3, height);
wooden_a_supports_paint_setup(session, 1, 12, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE+15), 6, 0, 20, 32, 3, height);
wooden_a_supports_paint_setup(session, 0, 9, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        switch (direction)
        {
        case 0:
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_8);
            break;
        case 1:
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_8);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    }
}

static void hybrid_rc_track_left_quarter_turn_3_25_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                          uint8_t direction, int32_t height, const TileElement * tileElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    hybrid_rc_track_right_quarter_turn_3_25_deg_up(session, rideIndex, trackSequence, (direction + 1) & 3, height, tileElement);
}

static void hybrid_rc_track_right_quarter_turn_3_25_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                           uint8_t direction, int32_t height, const TileElement * tileElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    hybrid_rc_track_left_quarter_turn_3_25_deg_up(session, rideIndex, trackSequence, (direction - 1) & 3, height, tileElement);
}








static void hybrid_rc_track_left_quarter_turn_5_25_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                        uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+0), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 9, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+5), 0, 0, 34, 20, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 10, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+10), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 11, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+15), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 12, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 1:
        paint_util_set_segment_support_height(
            session,paint_util_rotate_segments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+1), 0, 0, 32, 16, 3, height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+6), 0, 0, 32, 16, 3, height);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+11), 0, 0, 32, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+16), 0, 0, 32, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 3:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+2), 0, 0, 16, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+7), 0, 0, 16, 16, 3, height, 16, 16,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+12), 0, 0, 16, 16, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+17), 0, 0, 16, 16, 3, height);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 64, 0x20);
        break;
    case 4:
        paint_util_set_segment_support_height(
            session,paint_util_rotate_segments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 5:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+3), 0, 0, 16, 32, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+8), 0, 0, 16, 32, 3, height);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+13), 0, 0, 16, 32, 3, height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+18), 0, 0, 16, 32, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 6:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+4), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 12, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+9), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 9, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+14), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 10, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+19), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 11, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        switch (direction)
        {
        case 2:
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_8);
            break;
        case 3:
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_8);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    }
}


static void hybrid_rc_track_right_quarter_turn_5_25_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                         uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+20), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 9, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+25), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 10, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+30), 0, 0, 34, 20, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 11, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+35), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 12, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 1:
        paint_util_set_segment_support_height(
            session,paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+21), 0, 0, 32, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+26), 0, 0, 32, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+31), 0, 0, 32, 16, 3, height);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+36), 0, 0, 32, 16, 3, height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 3:
        switch (direction)
        {
        case 0:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+22), 0, 0, 16, 16, 3, height);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+27), 0, 0, 16, 16, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+32), 0, 0, 16, 16, 3, height, 16, 16,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+37), 0, 0, 16, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 64, 0x20);
        break;
    case 4:
        paint_util_set_segment_support_height(
            session,paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 5:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+23), 0, 0, 16, 32, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+28), 0, 0, 16, 32, 3, height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+33), 0, 0, 16, 32, 3, height);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+38), 0, 0, 16, 32, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
          session,  paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 6:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+24), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 10, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+29), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 11, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+34), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 12, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE+39), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 9, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        switch (direction)
        {
        case 0:
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_8);
            break;
        case 1:
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_8);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    }
}

static void hybrid_rc_track_left_quarter_turn_5_25_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                          uint8_t direction, int32_t height, const TileElement * tileElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    hybrid_rc_track_right_quarter_turn_5_25_deg_up(session, rideIndex, trackSequence, (direction + 1) & 3, height, tileElement);
}

static void hybrid_rc_track_right_quarter_turn_5_25_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                           uint8_t direction, int32_t height, const TileElement * tileElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    hybrid_rc_track_left_quarter_turn_5_25_deg_up(session, rideIndex, trackSequence, (direction - 1) & 3, height, tileElement);
}

static void hybrid_rc_track_left_quarter_turn_1_60_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                        uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (direction)
    {
    case 0:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_SMALL_CURVE+0), 0, 0, 28, 28, 3, height, 2, 2,
                           height);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_SMALL_CURVE+1), 0, 0, 28, 28, 1, height, 2, 2,
                           height + 99);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
        break;
    case 1:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_SMALL_CURVE+2), 0, 0, 28, 28, 3, height, 2, 2,
                           height);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_SMALL_CURVE+3), 0, 0, 28, 28, 1, height, 2, 2,
                           height + 99);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
        break;
    case 2:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_SMALL_CURVE+4), 0, 0, 28, 28, 3, height, 2, 2,
                           height);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_SMALL_CURVE+5), 0, 0, 28, 28, 1, height, 2, 2,
                           height + 99);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
        break;
    case 3:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_SMALL_CURVE+6), 0, 0, 28, 28, 3, height, 2, 2,
                           height);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_SMALL_CURVE+7), 0, 0, 28, 28, 1, height, 2, 2,
                           height + 99);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
        break;
    }
    track_paint_util_left_quarter_turn_1_tile_tunnel(session, direction, height, -8, TUNNEL_SQUARE_7, +56, TUNNEL_SQUARE_8);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 104, 0x20);
}

static void hybrid_rc_track_right_quarter_turn_1_60_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                         uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (direction)
    {
    case 0:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_SMALL_CURVE+8), 0, 0, 28, 28, 3, height, 2, 2,
                           height);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_SMALL_CURVE+9), 0, 0, 28, 28, 1, height, 2, 2,
                           height + 99);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
        break;
    case 1:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_SMALL_CURVE+10), 0, 0, 28, 28, 3, height, 2, 2,
                           height);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_SMALL_CURVE+11), 0, 0, 28, 28, 1, height, 2, 2,
                           height + 99);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
        break;
    case 2:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_SMALL_CURVE+12), 0, 0, 28, 28, 3, height, 2, 2,
                           height);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_SMALL_CURVE+13), 0, 0, 28, 28, 1, height, 2, 2,
                           height + 99);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
        break;
    case 3:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_SMALL_CURVE+14), 0, 0, 28, 28, 3, height, 2, 2,
                           height);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_STEEP_SMALL_CURVE+15), 0, 0, 28, 28, 1, height, 2, 2,
                           height + 99);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
        break;
    }
    track_paint_util_right_quarter_turn_1_tile_tunnel(session, direction, height, -8, TUNNEL_SQUARE_7, +56, TUNNEL_SQUARE_8);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 104, 0x20);
}

static void hybrid_rc_track_left_quarter_turn_1_60_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                          uint8_t direction, int32_t height, const TileElement * tileElement)
{
    hybrid_rc_track_right_quarter_turn_1_60_deg_up(session, rideIndex, trackSequence, (direction + 1) & 3, height, tileElement);
}

static void hybrid_rc_track_right_quarter_turn_1_60_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                           uint8_t direction, int32_t height, const TileElement * tileElement)
{
    hybrid_rc_track_left_quarter_turn_1_60_deg_up(session, rideIndex, trackSequence, (direction - 1) & 3, height, tileElement);
}

static void hybrid_rc_track_left_quarter_turn_1_90_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                            uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_VERTICAL_TWIST+0), 0, 0, 2, 20, 63, height, 4, 6,
                               height + 8);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_VERTICAL_TWIST+1), 0, 0, 2, 20, 63, height, 4, 6,
                               height + 8);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_VERTICAL_TWIST+2), 0, 0, 2, 20, 63, height, 24, 6,
                               height + 8);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_VERTICAL_TWIST+3), 0, 0, 2, 20, 63, height, 24, 6,
                               height + 8);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_VERTICAL_TWIST+4), 0, 0, 2, 20, 63, height, 4, 6,
                               height + 8);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_VERTICAL_TWIST+5), 0, 0, 2, 20, 63, height, 24, 6,
                               height + 8);
            break;
        }
        paint_util_set_vertical_tunnel(session, height + 96);
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 96, 0x20);
        break;
    case 1:
        break;
    }
}

static void hybrid_rc_track_right_quarter_turn_1_90_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                             uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_VERTICAL_TWIST+6), 0, 0, 2, 20, 63, height, 4, 6,
                               height + 8);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_VERTICAL_TWIST+7), 0, 0, 2, 20, 63, height, 24, 6,
                               height + 8);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_VERTICAL_TWIST+8), 0, 0, 2, 20, 63, height, 24, 6,
                               height + 8);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_VERTICAL_TWIST+9), 0, 0, 2, 20, 63, height, 4, 6,
                               height + 8);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_VERTICAL_TWIST+10), 0, 0, 2, 20, 63, height, 24, 6,
                               height + 8);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_VERTICAL_TWIST+11), 0, 0, 2, 20, 63, height, 4, 6,
                               height + 8);
            break;
        }
        paint_util_set_vertical_tunnel(session, height + 96);
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 96, 0x20);
        break;
    case 1:
        break;
    }
}

static void hybrid_rc_track_left_quarter_turn_1_90_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                              uint8_t direction, int32_t height, const TileElement * tileElement)
{
    hybrid_rc_track_right_quarter_turn_1_90_deg_up(session, rideIndex, trackSequence, (direction + 1) & 3, height,tileElement);
}

static void hybrid_rc_track_right_quarter_turn_1_90_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                               uint8_t direction, int32_t height, const TileElement * tileElement)
{
    hybrid_rc_track_left_quarter_turn_1_90_deg_up(session, rideIndex, trackSequence, (direction - 1) & 3, height,tileElement);
}

static void hybrid_rc_track_25_deg_up_to_left_banked_25_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                             uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (direction)
    {
    case 0:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+0), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        break;
    case 1:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+1), 0, 0, 32, 20, 2, height, 0, 6,
                           height);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+2), 0, 0, 32, 1, 34, height, 0, 27,
                           height+3);
        break;
    case 2:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+3), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        break;
    case 3:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+4), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        break;
    }
wooden_a_supports_paint_setup(session, direction & 1, 9 + direction, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_8);
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 56, 0x20);
}

static void hybrid_rc_track_25_deg_up_to_right_banked_25_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                              uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (direction)
    {
    case 0:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+5), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        break;
    case 1:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+6), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        break;
    case 2:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+7), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+8), 0, 0, 32, 1, 34, height, 0, 27,
                           height);
        break;
    case 3:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+9), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        break;
    }
wooden_a_supports_paint_setup(session, direction & 1, 9 + direction, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_8);
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 56, 0x20);
}

static void hybrid_rc_track_left_banked_25_deg_up_to_25_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                             uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (direction)
    {
    case 0:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+10), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        break;
    case 1:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+11), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+12), 0, 0, 32, 1, 34, height, 0, 27,
                           height);
        break;
    case 2:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+13), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        break;
    case 3:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+14), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        break;
    }
wooden_a_supports_paint_setup(session, direction & 1, 9 + direction, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_8);
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 56, 0x20);
}

static void hybrid_rc_track_right_banked_25_deg_up_to_25_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                              uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (direction)
    {
    case 0:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+15), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        break;
    case 1:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+16), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        break;
    case 2:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+17), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+18), 0, 0, 32, 1, 34, height, 0, 27,
                           height);
        break;
    case 3:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+19), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        break;
    }
wooden_a_supports_paint_setup(session, direction & 1, 9 + direction, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_8);
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 56, 0x20);
}

static void hybrid_rc_track_left_banked_25_deg_down_to_25_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                                 uint8_t direction, int32_t height, const TileElement * tileElement)
{
    hybrid_rc_track_25_deg_up_to_right_banked_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height,
                                                      tileElement);
}

static void hybrid_rc_track_right_banked_25_deg_down_to_25_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                                  uint8_t direction, int32_t height, const TileElement * tileElement)
{
    hybrid_rc_track_25_deg_up_to_left_banked_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height,
                                                     tileElement);
}

static void hybrid_rc_track_25_deg_down_to_left_banked_25_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                                 uint8_t direction, int32_t height, const TileElement * tileElement)
{
    hybrid_rc_track_right_banked_25_deg_up_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height,
                                                      tileElement);
}

static void hybrid_rc_track_25_deg_down_to_right_banked_25_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                                  uint8_t direction, int32_t height, const TileElement * tileElement)
{
    hybrid_rc_track_left_banked_25_deg_up_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height,
                                                     tileElement);
}

static void hybrid_rc_track_left_banked_flat_to_left_banked_25_deg_up(paint_session * session, uint16_t rideIndex,
                                                                    uint8_t trackSequence, uint8_t direction, int32_t height,
                                                                    const TileElement * tileElement)
{
    switch (direction)
    {
    case 0:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+20), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+21), 0, 0, 32, 1, 34, height, 0, 27,
                           height);
        break;
    case 1:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+22), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+23), 0, 0, 32, 1, 34, height, 0, 27,
                           height);
        break;
    case 2:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+24), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        break;
    case 3:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+25), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        break;
    }
wooden_a_supports_paint_setup(session, direction & 1, 1 + direction, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_8);
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

static void hybrid_rc_track_right_banked_flat_to_right_banked_25_deg_up(paint_session * session, uint16_t rideIndex,
                                                                      uint8_t trackSequence, uint8_t direction, int32_t height,
                                                                      const TileElement * tileElement)
{
    switch (direction)
    {
    case 0:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+26), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        break;
    case 1:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+27), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        break;
    case 2:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+28), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+29), 0, 0, 32, 1, 34, height, 0, 27,
                           height);
        break;
    case 3:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+30), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+31), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        break;
    }
wooden_a_supports_paint_setup(session, direction & 1, 1 + direction, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_8);
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

static void hybrid_rc_track_left_banked_25_deg_up_to_left_banked_flat(paint_session * session, uint16_t rideIndex,
                                                                    uint8_t trackSequence, uint8_t direction, int32_t height,
                                                                    const TileElement * tileElement)
{
    switch (direction)
    {
    case 0:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+32), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+33), 0, 0, 32, 1, 34, height, 0, 27,
                           height);
        break;
    case 1:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+34), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+35), 0, 0, 32, 1, 34, height, 0, 27,
                           height);
        break;
    case 2:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+36), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        break;
    case 3:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+37), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        break;
    }
wooden_a_supports_paint_setup(session, direction & 1, 5 + direction, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_14);
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 40, 0x20);
}

static void hybrid_rc_track_right_banked_25_deg_up_to_right_banked_flat(paint_session * session, uint16_t rideIndex,
                                                                      uint8_t trackSequence, uint8_t direction, int32_t height,
                                                                      const TileElement * tileElement)
{
    switch (direction)
    {
    case 0:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+38), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        break;
    case 1:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+39), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        break;
    case 2:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+40), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+41), 0, 0, 32, 1, 34, height, 0, 27,
                           height);
        break;
    case 3:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+42), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+43), 0, 0, 32, 1, 34, height, 0, 27,
                           height);
        break;
    }
wooden_a_supports_paint_setup(session, direction & 1, 5 + direction, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_14);
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 40, 0x20);
}

static void hybrid_rc_track_left_banked_flat_to_left_banked_25_deg_down(paint_session * session, uint16_t rideIndex,
                                                                      uint8_t trackSequence, uint8_t direction, int32_t height,
                                                                      const TileElement * tileElement)
{
    hybrid_rc_track_right_banked_25_deg_up_to_right_banked_flat(session, rideIndex, trackSequence, (direction + 2) & 3, height,
                                                              tileElement);
}

static void hybrid_rc_track_right_banked_flat_to_right_banked_25_deg_down(paint_session * session, uint16_t rideIndex,
                                                                        uint8_t trackSequence, uint8_t direction, int32_t height,
                                                                        const TileElement * tileElement)
{
    hybrid_rc_track_left_banked_25_deg_up_to_left_banked_flat(session, rideIndex, trackSequence, (direction + 2) & 3, height,
                                                            tileElement);
}

static void hybrid_rc_track_left_banked_25_deg_down_to_left_banked_flat(paint_session * session, uint16_t rideIndex,
                                                                      uint8_t trackSequence, uint8_t direction, int32_t height,
                                                                      const TileElement * tileElement)
{
    hybrid_rc_track_right_banked_flat_to_right_banked_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height,
                                                              tileElement);
}

static void hybrid_rc_track_right_banked_25_deg_down_to_right_banked_flat(paint_session * session, uint16_t rideIndex,
                                                                        uint8_t trackSequence, uint8_t direction, int32_t height,
                                                                        const TileElement * tileElement)
{
    hybrid_rc_track_left_banked_flat_to_left_banked_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height,
                                                            tileElement);
}

static void hybrid_rc_track_25_deg_up_left_banked(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                                int32_t height, const TileElement * tileElement)
{
    switch (direction)
    {
    case 0:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+44), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        break;
    case 1:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+45), 0, 0, 32, 1, 34, height, 0, 27,
                           height);
        break;
    case 2:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+46), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        break;
    case 3:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+47), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        break;
    }
wooden_a_supports_paint_setup(session, direction & 1, 9 + direction, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_8);
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 56, 0x20);
}

static void hybrid_rc_track_25_deg_up_right_banked(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                                 int32_t height, const TileElement * tileElement)
{
    switch (direction)
    {
    case 0:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+48), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        break;
    case 1:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+49), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        break;
    case 2:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+50), 0, 0, 32, 1, 34, height, 0, 27,
                           height);
        break;
    case 3:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+51), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        break;
    }
wooden_a_supports_paint_setup(session, direction & 1, 9 + direction, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_8);
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 56, 0x20);
}

static void hybrid_rc_track_25_deg_down_left_banked(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                  uint8_t direction, int32_t height, const TileElement * tileElement)
{
    hybrid_rc_track_25_deg_up_right_banked(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

static void hybrid_rc_track_25_deg_down_right_banked(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                   uint8_t direction, int32_t height, const TileElement * tileElement)
{
    hybrid_rc_track_25_deg_up_left_banked(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

static void hybrid_rc_track_flat_to_left_banked_25_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                        uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (direction)
    {
    case 0:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+52), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        break;
    case 1:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+53), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+54), 0, 0, 32, 1, 34, height, 0, 27,
                           height);
        break;
    case 2:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+55), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        break;
    case 3:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+56), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        break;
    }
wooden_a_supports_paint_setup(session, direction & 1, 1 + direction, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_8);
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

static void hybrid_rc_track_flat_to_right_banked_25_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                         uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (direction)
    {
    case 0:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+57), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        break;
    case 1:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+58), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        break;
    case 2:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+59), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+60), 0, 0, 32, 1, 34, height, 0, 27,
                           height);
        break;
    case 3:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+61), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        break;
    }
wooden_a_supports_paint_setup(session, direction & 1, 1 + direction, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_8);
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

static void hybrid_rc_track_left_banked_25_deg_up_to_flat(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                        uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (direction)
    {
    case 0:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+62), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        break;
    case 1:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+63), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+64), 0, 0, 32, 1, 34, height, 0, 27,
                           height);
        break;
    case 2:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+65), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        break;
    case 3:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+66), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        break;
    }
wooden_a_supports_paint_setup(session, direction & 1, 5 + direction, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_14);
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 40, 0x20);
}

static void hybrid_rc_track_right_banked_25_deg_up_to_flat(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                         uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (direction)
    {
    case 0:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+67), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        break;
    case 1:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+68), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        break;
    case 2:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+69), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+70), 0, 0, 32, 1, 34, height, 0, 27,
                           height);
        break;
    case 3:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SLOPE_BANK_TRANSITION+71), 0, 0, 32, 20, 2, height, 0, 6,
                           height+3);
        break;
    }
wooden_a_supports_paint_setup(session, direction & 1, 5 + direction, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_14);
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 40, 0x20);
}

static void hybrid_rc_track_flat_to_left_banked_25_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                          uint8_t direction, int32_t height, const TileElement * tileElement)
{
    hybrid_rc_track_right_banked_25_deg_up_to_flat(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

static void hybrid_rc_track_flat_to_right_banked_25_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                           uint8_t direction, int32_t height, const TileElement * tileElement)
{
    hybrid_rc_track_left_banked_25_deg_up_to_flat(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

static void hybrid_rc_track_left_banked_25_deg_down_to_flat(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                          uint8_t direction, int32_t height, const TileElement * tileElement)
{
    hybrid_rc_track_flat_to_right_banked_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

static void hybrid_rc_track_right_banked_25_deg_down_to_flat(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                           uint8_t direction, int32_t height, const TileElement * tileElement)
{
    hybrid_rc_track_flat_to_left_banked_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, tileElement);
}

static void hybrid_rc_track_left_banked_quarter_turn_3_25_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                               uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE_BANKED+0), 0, 6, 32, 20, 3, height);
wooden_a_supports_paint_setup(session, 0, 9, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE_BANKED+2), 0, 6, 32, 20, 3, height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE_BANKED+3), 0, 6, 34, 1, 34, height, 0, 27,
                               height);
wooden_a_supports_paint_setup(session, 1, 10, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE_BANKED+6), 0, 6, 32, 20, 3, height);
wooden_a_supports_paint_setup(session, 0, 11, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE_BANKED+9), 0, 6, 32, 20, 3, height);
wooden_a_supports_paint_setup(session, 1, 12, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 3:
        switch (direction)
        {
        case 0:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE_BANKED+1), 6, 0, 20, 32, 3, height);
wooden_a_supports_paint_setup(session, 0, 12, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE_BANKED+4), 6, 0, 20, 32, 3, height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE_BANKED+5), 6, 0, 1, 34, 34, height, 27, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 9, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE_BANKED+7), 6, 0, 20, 32, 3, height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE_BANKED+8), 6, 0, 1, 32, 34, height, 27, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 10, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE_BANKED+10), 6, 0, 20, 32, 3, height);
wooden_a_supports_paint_setup(session, 1, 11, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        switch (direction)
        {
        case 2:
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_8);
            break;
        case 3:
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_8);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    }
}

static void hybrid_rc_track_right_banked_quarter_turn_3_25_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                                uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE_BANKED+11), 0, 6, 32, 20, 3, height);
wooden_a_supports_paint_setup(session, 0, 9, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE_BANKED+13), 0, 6, 32, 20, 3, height);
wooden_a_supports_paint_setup(session, 1, 10, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE_BANKED+16), 0, 6, 32, 20, 3, height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE_BANKED+17), 0, 6, 34, 1, 34, height, 0, 27,
                               height);
wooden_a_supports_paint_setup(session, 0, 11, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE_BANKED+20), 0, 6, 32, 20, 3, height);
wooden_a_supports_paint_setup(session, 1, 12, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 3:
        switch (direction)
        {
        case 0:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE_BANKED+12), 6, 0, 20, 32, 3, height);
wooden_a_supports_paint_setup(session, 1, 10, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE_BANKED+14), 6, 0, 20, 32, 3, height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE_BANKED+15), 6, 0, 1, 32, 34, height, 27, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 11, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE_BANKED+18), 6, 0, 20, 32, 3, height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE_BANKED+19), 6, 0, 1, 34, 34, height, 27, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 12, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_SMALL_CURVE_BANKED+21), 6, 0, 20, 32, 3, height);
wooden_a_supports_paint_setup(session, 0, 9, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        switch (direction)
        {
        case 0:
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_8);
            break;
        case 1:
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_8);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    }
}

static void hybrid_rc_track_left_banked_quarter_turn_3_25_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                                 uint8_t direction, int32_t height, const TileElement * tileElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    hybrid_rc_track_right_banked_quarter_turn_3_25_deg_up(session, rideIndex, trackSequence, (direction + 1) & 3, height,
                                                        tileElement);
}

static void hybrid_rc_track_right_banked_quarter_turn_3_25_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                                  uint8_t direction, int32_t height, const TileElement * tileElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    hybrid_rc_track_left_banked_quarter_turn_3_25_deg_up(session, rideIndex, trackSequence, (direction - 1) & 3, height,
                                                       tileElement);
}
            

static void hybrid_rc_track_left_banked_quarter_turn_5_25_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                               uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+0), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 9, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+5), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+6), 0, 0, 34, 1, 34, height, 0, 27,
                               height);
wooden_a_supports_paint_setup(session, 1, 10, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+15), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 11, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+21), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 12, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 1:
        paint_util_set_segment_support_height(
            session,paint_util_rotate_segments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+1), 0, 0, 32, 16, 3, height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+7), 0, 0, 32, 16, 3, height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+8), 0, 0, 1, 1, 34, height, 30, 30,
                               height);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+16), 0, 0, 32, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+22), 0, 0, 32, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 3:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+2), 0, 0, 16, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+9), 0, 0, 16, 16, 3, height, 0, -8,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+10), 0, 0, 1, 1, 34, height, 30, 30,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+17), 0, 0, 1, 1, 3, height, 64, 0,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+23), 0, 0, 16, 16, 3, height);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 64, 0x20);
        break;
    case 4:
        paint_util_set_segment_support_height(
            session,paint_util_rotate_segments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 5:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+3), 0, 0, 16, 32, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+11), 0, 0, 16, 32, 3, height, -8, 0,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+12), 0, 0, 1 , 1, 34, height, 30, 30,
                               height);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+18), 0, 0, 1, 32, 34, height, 27, 0,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+24), 0, 0, 16, 32, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 6:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+4), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 12, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+13), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+14), 0, 0, 1, 32, 34, height, 27, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 9, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+19), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+20), 0, 0, 1, 32, 34, height, 27, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 10, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+25), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 11, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        switch (direction)
        {
        case 2:
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_8);
            break;
        case 3:
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_8);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    }
}

static void hybrid_rc_track_right_banked_quarter_turn_5_25_deg_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                                uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+26), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 9, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+31), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 10, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+37), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+38), 0, 0, 34, 1, 34, height, 0, 27,
                               height);
wooden_a_supports_paint_setup(session, 0, 11, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+47), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 12, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 1:
        paint_util_set_segment_support_height(
            session,paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+27), 0, 0, 32, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+32), 0, 0, 32, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+39), 0, 0, 32, 16, 3, height, 0, 16,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+40), 0, 0, 1, 1, 34, height, 30, 30,
                               height);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+48), 0, 0, 32, 16, 3, height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 3:
        switch (direction)
        {
        case 0:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+28), 0, 0, 16, 16, 3, height);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+33), 0, 0, 1, 1, 3, height, 64, 0,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+41), 0, 0, 16, 16, 3, height, -8, 0,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+42), 0, 0, 1, 1, 34, height, 30, 30,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+49), 0, 0, 16, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 64, 0x20);
        break;
    case 4:
        paint_util_set_segment_support_height(
            session,paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 5:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+29), 0, 0, 16, 32, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+34), 0, 0, 1, 32, 34, height, 27, 0,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+43), 0, 0, 16, 32, 3, height, -8, 0,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+44), 0, 0, 1, 1, 34, height, 30, 30,
                               height);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+50), 0, 0, 16, 32, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 6:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+30), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 10, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+35), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+36), 0, 0, 1, 32, 34, height, 27, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 11, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+45), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+46), 0, 0, 1, 32, 34, height, 27, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 12, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_GENTLE_MEDIUM_CURVE_BANKED+51), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 9, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        switch (direction)
        {
        case 0:
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_8);
            break;
        case 1:
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_8);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    }
}

static void hybrid_rc_track_left_banked_quarter_turn_5_25_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                                 uint8_t direction, int32_t height, const TileElement * tileElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    hybrid_rc_track_right_banked_quarter_turn_5_25_deg_up(session, rideIndex, trackSequence, (direction + 1) & 3, height,
                                                        tileElement);
}

static void hybrid_rc_track_right_banked_quarter_turn_5_25_deg_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                                  uint8_t direction, int32_t height, const TileElement * tileElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    hybrid_rc_track_left_banked_quarter_turn_5_25_deg_up(session, rideIndex, trackSequence, (direction - 1) & 3, height,
                                                       tileElement);
}




static void hybrid_rc_track_s_bend_left(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                      int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_S_BEND+0), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_S_BEND+4), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_S_BEND+3), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_S_BEND+7), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction)
        {
        case 0:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_S_BEND+1), 0, 0, 32, 26, 3, height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_S_BEND+5), 0, 0, 34, 26, 3, height);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_S_BEND+2), 0, 0, 32, 26, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_S_BEND+6), 0, 0, 32, 26, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_S_BEND+2), 0, 0, 32, 26, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_S_BEND+6), 0, 0, 32, 26, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_S_BEND+1), 0, 0, 32, 26, 3, height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_S_BEND+5), 0, 0, 34, 26, 3, height);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_S_BEND+3), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_S_BEND+7), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_S_BEND+0), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_S_BEND+4), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        switch (direction)
        {
        case 1:
            paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

static void hybrid_rc_track_s_bend_right(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                       int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_S_BEND+8), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_S_BEND+12), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_S_BEND+11), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_S_BEND+15), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_S_BEND+9), 0, 0, 32, 26, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_S_BEND+13), 0, 0, 32, 26, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_S_BEND+10), 0, 0, 34, 26, 3, height);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_S_BEND+14), 0, 0, 32, 26, 3, height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_S_BEND+10), 0, 0, 34, 26, 3, height);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98196C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_S_BEND+14), 0, 0, 32, 26, 3, height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_S_BEND+9), 0, 0, 32, 26, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_S_BEND+13), 0, 0, 32, 26, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_S_BEND+11), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_S_BEND+15), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_S_BEND+8), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_S_BEND+12), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        switch (direction)
        {
        case 1:
            paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}



static void hybrid_rc_track_left_half_banked_helix_up_small(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                          uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+0), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+1), 0, 0, 32, 1, 26, height, 0, 27,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+4), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+5), 0, 0, 32, 1, 26, height, 0, 27,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+9), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+13), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction)
        {
        case 0:
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
	paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+2), 0, 0, 16, 16, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+6), 0, 0, 16, 16, 1, height, 0, 0,
                               height + 28);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+10), 0, 0, 16, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+14), 0, 0, 16, 16, 3, height, 16, 16,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
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
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+3), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+7), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+8), 0, 0, 1, 32, 26, height, 27, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+11), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+12), 0, 0, 1, 32, 26, height, 27, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+15), 0, 0, 32, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        switch (direction)
        {
        case 2:
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_FLAT);
            break;
        case 3:
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_FLAT);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 4:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+13), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+0), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+1), 0, 0, 1, 32, 26, height, 27, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+4), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+5), 0, 0, 1, 32, 26, height, 27, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+9), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        switch (direction)
        {
        case 0:
            paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
            break;
        case 1:
            paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 5:
        switch (direction)
        {
        case 0:
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 6:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+14), 0, 0, 16, 16, 3, height, 16, 16,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+2), 0, 0, 16, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+6), 0, 0, 16, 16, 1, height, 0, 0,
                               height + 28);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+10), 0, 0, 16, 16, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 7:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+15), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+3), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+7), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+8), 0, 0, 32, 1, 26, height, 0, 27,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+11), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+12), 0, 0, 32, 1, 26, height, 0, 27,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_FLAT);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

static void hybrid_rc_track_right_half_banked_helix_up_small(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                           uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+16), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+19), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+23), 0, 0, 34, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+24), 0, 0, 32, 1, 26, height, 0, 27,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+28), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+29), 0, 0, 32, 1, 26, height, 0, 27,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction)
        {
        case 0:
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+17), 0, 0, 16, 16, 3, height, 16, 16,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+20), 0, 0, 16, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+25), 0, 0, 16, 16, 1, height, 0, 0,
                               height + 28);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+30), 0, 0, 16, 16, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+18), 0, 0, 32, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+21), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+22), 0, 0, 1, 32, 26, height, 27, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+26), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+27), 0, 0, 1, 32, 26, height, 27, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+31), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        switch (direction)
        {
        case 0:
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_FLAT);
            break;
        case 1:
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_FLAT);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 4:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+19), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+23), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+24), 0, 0, 1, 32, 26, height, 27, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+28), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+29), 0, 0, 1, 32, 26, height, 27, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+16), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        switch (direction)
        {
        case 2:
            paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
            break;
        case 3:
            paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 5:
        switch (direction)
        {
        case 0:
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 6:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+20), 0, 0, 16, 16, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+25), 0, 0, 16, 16, 1, height, 0, 0,
                               height + 28);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+30), 0, 0, 16, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+17), 0, 0, 16, 16, 3, height, 16, 16,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 7:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+21), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+22), 0, 0, 32, 1, 26, height, 0, 27,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+26), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+27), 0, 0, 32, 1, 26, height, 0, 27,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+31), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_SMALL_HELIX+18), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_FLAT);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

static void hybrid_rc_track_left_half_banked_helix_down_small(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                            uint8_t direction, int32_t height, const TileElement * tileElement)
{
    if (trackSequence >= 4)
    {
        trackSequence -= 4;
        direction = (direction - 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    hybrid_rc_track_right_half_banked_helix_up_small(session, rideIndex, trackSequence, (direction + 1) & 3, height, tileElement);
}

static void hybrid_rc_track_right_half_banked_helix_down_small(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                             uint8_t direction, int32_t height, const TileElement * tileElement)
{
    if (trackSequence >= 4)
    {
        trackSequence -= 4;
        direction = (direction + 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    hybrid_rc_track_left_half_banked_helix_up_small(session, rideIndex, trackSequence, (direction - 1) & 3, height, tileElement);
}

static void hybrid_rc_track_left_half_banked_helix_up_large(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                          uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+0), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+1), 0, 0, 32, 1, 26, height, 0, 27,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+6), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+7), 0, 0, 32, 1, 26, height, 0, 27,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+14), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+20), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+2), 0, 0, 32, 16, 3, height, 0, 0,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+8), 0, 0, 33, 16, 1, height, 0, 0,
                               height + 28);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+15), 0, 0, 32, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+21), 0, 0, 32, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+3), 0, 0, 16, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+9), 0, 0, 16, 16, 3, height, 0, 16,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+10), 0, 0, 16, 16, 1, height, 16, 16,
                               height + 28);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+16), 0, 0, 16, 16, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+22), 0, 0, 34, 34, 3, height, 0, 0,
                               height);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4 | SEGMENT_B4 | SEGMENT_C0 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 4:
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 5:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+4), 0, 0, 16, 32, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+11), 0, 0, 16, 33, 1, height, 0, 0,
                               height + 28);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+17), 0, 0, 16, 32, 3, height, 0, 0,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+23), 0, 0, 16, 32, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 6:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+5), 0, 0, 32, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+12), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+13), 0, 0, 1, 32, 26, height, 27, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+18), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+19), 0, 0, 1, 32, 26, height, 27, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+24), 0, 0, 28, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        switch (direction)
        {
        case 2:
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_FLAT);
            break;
        case 3:
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_FLAT);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 7:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+20), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+0), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+1), 0, 0, 1, 32, 26, height, 27, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+6), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+7), 0, 0, 1, 32, 26, height, 27, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+14), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        switch (direction)
        {
        case 0:
            paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
            break;
        case 1:
            paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 8:
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 9:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+21), 0, 0, 16, 32, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+2), 0, 0, 16, 32, 3, height, 0, 0,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+8), 0, 0, 16, 32, 1, height, 0, 0,
                               height + 28);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+15), 0, 0, 16, 32, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 10:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+22), 0, 0, 16, 16, 3, height, 0, 0,
                               height);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+3), 0, 0, 16, 16, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+9), 0, 0, 16, 16, 3, height, 0, 16,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+10), 0, 0, 16, 16, 1, height, 16, 16,
                               height + 28);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+16), 0, 0, 16, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_B8 | SEGMENT_BC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 11:
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 12:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+23), 0, 0, 32, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+4), 0, 0, 32, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+11), 0, 0, 32, 16, 1, height, 0, 0,
                               height + 28);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+17), 0, 0, 32, 16, 3, height, 0, 0,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 13:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+24), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+5), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+12), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+13), 0, 0, 32, 1, 26, height, 0, 27,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+18), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+19), 0, 0, 32, 1, 26, height, 0, 27,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_FLAT);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

static void hybrid_rc_track_right_half_banked_helix_up_large(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                           uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+25), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+30), 0, 0, 32, 32, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+36), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+37), 0, 0, 32, 1, 26, height, 0, 27,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+44), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+45), 0, 0, 32, 1, 26, height, 0, 27,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+26), 0, 0, 32, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+31), 0, 0, 32, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+38), 0, 0, 33, 16, 1, height, 0, 0,
                               height + 28);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+46), 0, 0, 32, 16, 3, height, 0, 0,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+27), 0, 0, 34, 16, 3, height, 0, 0,
                               height);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+32), 0, 0, 16, 16, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+39), 0, 0, 16, 32, 3, height, 0, 16,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+40), 0, 0, 16, 16, 1, height, 16, 16,
                               height + 28);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+47), 0, 0, 16, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_B8 | SEGMENT_BC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 4:
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 5:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+28), 0, 0, 16, 32, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+33), 0, 0, 16, 32, 3, height, 0, 0,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+41), 0, 0, 16, 33, 1, height, 0, 0,
                               height + 28);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+48), 0, 0, 16, 32, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 6:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+29), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+34), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+35), 0, 0, 1, 32, 26, height, 27, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+42), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+43), 0, 0, 1, 32, 26, height, 27, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+49), 0, 0, 32, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        switch (direction)
        {
        case 0:
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_FLAT);
            break;
        case 1:
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_FLAT);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 7:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+30), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+36), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+37), 0, 0, 1, 32, 26, height, 27, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+44), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+45), 0, 0, 1, 32, 26, height, 27, 0,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+25), 0, 0, 20, 32, 3, height, 6, 0,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        switch (direction)
        {
        case 2:
            paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
            break;
        case 3:
            paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 8:
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 9:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+31), 0, 0, 16, 32, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+38), 0, 0, 16, 32, 1, height, 0, 0,
                               height + 28);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+46), 0, 0, 16, 32, 3, height, 0, 0,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+26), 0, 0, 16, 32, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 10:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+32), 0, 0, 16, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+39), 0, 0, 16, 16, 3, height, 0, 16,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+40), 0, 0, 16, 16, 1, height, 16, 16,
                               height + 28);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+47), 0, 0, 16, 16, 3, height, 16, 0,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+27), 0, 0, 16, 16, 3, height, 0, 0,
                               height);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4 | SEGMENT_B4 | SEGMENT_C0 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 11:
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 12:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+33), 0, 0, 32, 16, 3, height, 0, 0,
                               height);
wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+41), 0, 0, 32, 16, 1, height, 0, 0,
                               height + 28);
wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+48), 0, 0, 32, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+28), 0, 0, 32, 16, 3, height, 0, 16,
                               height);
wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 13:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+34), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+35), 0, 0, 32, 1, 26, height, 0, 27,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+42), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+43), 0, 0, 32, 1, 26, height, 0, 27,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+49), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_LARGE_HELIX+29), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_FLAT);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

static void hybrid_rc_track_left_half_banked_helix_down_large(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                            uint8_t direction, int32_t height, const TileElement * tileElement)
{
    if (trackSequence >= 7)
    {
        trackSequence -= 7;
        direction = (direction - 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    hybrid_rc_track_right_half_banked_helix_up_large(session, rideIndex, trackSequence, (direction + 1) & 3, height, tileElement);
}

static void hybrid_rc_track_right_half_banked_helix_down_large(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                             uint8_t direction, int32_t height, const TileElement * tileElement)
{
    if (trackSequence >= 7)
    {
        trackSequence -= 7;
        direction = (direction + 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    hybrid_rc_track_left_half_banked_helix_up_large(session, rideIndex, trackSequence, (direction - 1) & 3, height, tileElement);
}



static void hybrid_rc_track_left_barrel_roll_up_to_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                          uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+0), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+1), 0, 0, 32, 20, 0, height, 0, 27,
                               height + 28);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+6), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+7), 0, 0, 32, 20, 0, height, 0, 27,
                               height + 28);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+12), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+13), 0, 0, 32, 20, 0, height, 0, 27,
                               height + 28);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+18), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+19), 0, 0, 32, 20, 0, height, 0, 27,
                               height + 28);
            break;
        }
wooden_a_supports_paint_setup(session, direction & 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+2), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+3), 0, 0, 32, 1, 26, height, 0, 27,
                               height + 28);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+8), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+9), 0, 0, 32, 1, 26, height, 0, 27,
                               height + 28);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+14), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+15), 0, 0, 32, 1, 26, height, 0, 27,
                               height + 28);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+20), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+21), 0, 0, 32, 1, 26, height, 0, 27,
                               height + 28);
            break;
        }
wooden_a_supports_paint_setup(session, direction & 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENTS_ALL, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+4), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+5), 0, 0, 32, 1, 26, height, 0, 27,
                               height + 44);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+10), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+11), 0, 0, 32, 1, 26, height, 0, 27,
                               height + 44);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+16), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+17), 0, 0, 32, 1, 26, height, 0, 27,
                               height + 44);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+22), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+23), 0, 0, 32, 1, 26, height, 0, 27,
                               height + 44);
            break;
        }
        switch (direction)
        {
        case 1:
            paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_INVERTED_9);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_INVERTED_9);
            break;
        }
wooden_a_supports_paint_setup(session, direction & 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    }
}

static void hybrid_rc_track_right_barrel_roll_up_to_down(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                           uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+24), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+25), 0, 0, 32, 1, 26, height, 0, 27,
                               height + 28);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+30), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+31), 0, 0, 32, 1, 26, height, 0, 27,
                               height + 28);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+36), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+37), 0, 0, 32, 1, 26, height, 0, 27,
                               height + 28);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+42), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+43), 0, 0, 32, 1, 26, height, 0, 27,
                               height + 28);
            break;
        }
wooden_a_supports_paint_setup(session, direction & 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+26), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+27), 0, 0, 32, 1, 26, height, 0, 27,
                               height + 28);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+32), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+33), 0, 0, 32, 1, 26, height, 0, 27,
                               height + 28);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+38), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+39), 0, 0, 32, 1, 26, height, 0, 27,
                               height + 28);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+44), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+45), 0, 0, 32, 1, 26, height, 0, 27,
                               height + 28);
            break;
        }
wooden_a_supports_paint_setup(session, direction & 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENTS_ALL, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+28), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+29), 0, 0, 32, 1, 26, height, 0, 27,
                               height + 44);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+34), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+35), 0, 0, 32, 1, 26, height, 0, 27,
                               height + 44);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+40), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+41), 0, 0, 32, 1, 26, height, 0, 27,
                               height + 44);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+46), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BARREL_ROLL+47), 0, 0, 32, 1, 26, height, 0, 27,
                               height + 44);
            break;
        }
wooden_a_supports_paint_setup(session, direction & 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
        switch (direction)
        {
        case 1:
            paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_INVERTED_9);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_INVERTED_9);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    }
}

static void hybrid_rc_track_left_barrel_roll_down_to_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                          uint8_t direction, int32_t height, const TileElement * tileElement)
{
    hybrid_rc_track_left_barrel_roll_up_to_down(session, rideIndex, 2 - trackSequence, (direction + 2) & 3, height,
                                                         tileElement);
}

static void hybrid_rc_track_right_barrel_roll_down_to_up(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                           uint8_t direction, int32_t height, const TileElement * tileElement)
{
    hybrid_rc_track_right_barrel_roll_up_to_down(session, rideIndex, 2 - trackSequence, (direction + 2) & 3, height,
                                                          tileElement);
}

static void hybrid_rc_track_90_deg_to_inverted_flat_quarter_loop_up(
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
                       session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_QUARTER_LOOP+0), 0, 0, 2, 20, 31, height, 4, 6,
                        height + 8);
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_QUARTER_LOOP+1), 0, 0, 2, 20, 31, height, 4, 32,
                        height + 8);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_QUARTER_LOOP+6), 0, 0, 2, 20, 31, height, 24, -32,
                        height + 8);
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_QUARTER_LOOP+7), 0, 0, 2, 20, 31, height, 24, 6,
                        height + 8);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_QUARTER_LOOP+12), 0, 0, 2, 20, 31, height, 24, -32,
                        height + 8);
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_QUARTER_LOOP+13), 0, 0, 2, 20, 31, height, 24, 6,
                        height + 8);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_QUARTER_LOOP+18), 0, 0, 2, 20, 31, height, 4, 6,
                        height + 8);
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_QUARTER_LOOP+19), 0, 0, 2, 20, 31, height, 4, 32,
                        height + 8);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 88, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_QUARTER_LOOP+2), 0, 0, 2, 20, 31, height, -8, 6,
                        height);
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_QUARTER_LOOP+3), 0, 0, 2, 20, 63, height, 24, 6,
                        height + 8);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_QUARTER_LOOP+8), 0, 0, 2, 20, 31, height, -8, 6,
                        height);
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_QUARTER_LOOP+9), 0, 0, 2, 20, 63, height, 24, 6,
                        height + 8);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_QUARTER_LOOP+14), 0, 0, 2, 20, 31, height, -8, 6,
                        height);
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_QUARTER_LOOP+15), 0, 0, 2, 20, 63, height, 24, 6,
                        height + 8);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_QUARTER_LOOP+20), 0, 0, 2, 20, 31, height, -8, 6,
                        height);
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_QUARTER_LOOP+21), 0, 0, 2, 20, 63, height, 24, 6,
                        height + 8);
                    break;
            }
wooden_a_supports_paint_setup(session, direction & 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 64, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_QUARTER_LOOP+4), 0, 0, 32, 20, 3, height, 0, 6,
                        height);
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_QUARTER_LOOP+5), 0, 0, 32, 20, 3, height, 0, 6,
                        height + 24);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_QUARTER_LOOP+10), 0, 0, 2, 20, 3, height, 0, 6,
                        height);
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_QUARTER_LOOP+11), 0, 0, 2, 20, 31, height, 24, 6,
                        height + 24);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_QUARTER_LOOP+16), 0, 0, 2, 20, 3, height, 0, 6,
                        height);
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_QUARTER_LOOP+17), 0, 0, 2, 20, 31, height, 24, 6,
                        height + 24);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_QUARTER_LOOP+22), 0, 0, 32, 20, 3, height, 0, 6,
                        height);
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_QUARTER_LOOP+23), 0, 0, 32, 20, 3, height, 0, 6,
                        height + 24);
                    break;
            }
wooden_a_supports_paint_setup(session, direction & 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height + 16, TUNNEL_0);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
    }
}

static void hybrid_rc_track_inverted_flat_to_90_deg_quarter_loop_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    hybrid_rc_track_90_deg_to_inverted_flat_quarter_loop_up(
        session, rideIndex, 2 - trackSequence, direction, height, tileElement);
}



static void hybrid_rc_track_brakes(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
                                 const TileElement * tileElement)
{
    switch (direction)
    {
    case 0:
    case 2:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BRAKE+0), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        break;
    case 1:
    case 3:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BRAKE+1), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        break;
    }
wooden_a_supports_paint_setup(session, direction & 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x008ADA44 */
static void hybrid_rc_track_on_ride_photo(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                        int32_t height, const TileElement * tileElement)
{
    switch (direction)
    {
    case 0:
        sub_98196C_rotated(session, direction, IMAGE_TYPE_REMAP | SPR_STATION_BASE_D, 0, 0, 32, 32, 1, height);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT+0), 0, 0, 32, 20, 0, height, 0, 6,
                           height + 3);
        break;
    case 1:
        sub_98196C_rotated(session, direction, IMAGE_TYPE_REMAP | SPR_STATION_BASE_D, 0, 0, 32, 32, 1, height);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT+1), 0, 0, 32, 20, 0, height, 0, 6,
                           height + 3);
        break;
    case 2:
        sub_98196C_rotated(session, direction, IMAGE_TYPE_REMAP | SPR_STATION_BASE_D, 0, 0, 32, 32, 1, height);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT+0), 0, 0, 32, 20, 0, height, 0, 6,
                           height + 3);
        break;
    case 3:
        sub_98196C_rotated(session, direction, IMAGE_TYPE_REMAP | SPR_STATION_BASE_D, 0, 0, 32, 32, 1, height);
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT+1), 0, 0, 32, 20, 0, height, 0, 6,
                           height + 3);
        break;
    }
    track_paint_util_onride_photo_paint(session, direction, height + 3, tileElement);
wooden_a_supports_paint_setup(session, direction & 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/** rct2: 0x008ADED4 */
static void hybrid_rc_track_flat_to_60_deg_up_long_base(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                      uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_TO_STEEP+0), 0, 0, 32, 20, 3, height, 0,
                               6, height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_TO_STEEP+4), 0, 0, 32, 20, 3, height, 0,
                               6, height);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_TO_STEEP+8), 0, 0, 32, 20, 3, height, 0,
                               6, height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_TO_STEEP+12), 0, 0, 32, 20, 3, height, 0,
                               6, height);
            break;
        }
wooden_a_supports_paint_setup(session, direction & 1, 50 + direction, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 1:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_TO_STEEP+1), 0, 0, 32, 20, 3, height, 0,
                               6, height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_TO_STEEP+5), 0, 0, 32, 20, 3, height, 0,
                               6, height);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_TO_STEEP+9), 0, 0, 32, 20, 3, height, 0,
                               6, height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_TO_STEEP+13), 0, 0, 32, 20, 3, height, 0,
                               6, height);
            break;
        }
wooden_a_supports_paint_setup(session, direction & 1, 54 + direction, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_TO_STEEP+2), 0, 0, 32, 20, 3, height, 0,
                               6, height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_TO_STEEP+6), 0, 0, 32, 20, 3, height, 0,
                               6, height);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_TO_STEEP+10), 0, 0, 32, 20, 3, height, 0,
                               6, height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_TO_STEEP+14), 0, 0, 32, 20, 3, height, 0,
                               6, height);
            break;
        }
wooden_a_supports_paint_setup(session, direction & 1, 58 + direction, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 64, 0x20);
        break;
    case 3:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_TO_STEEP+3), 0, 0, 32, 20, 3, height, 0,
                               6, height);
            break;
        case 1:
            session->WoodenSupportsPrependTo = sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_TO_STEEP+7), 0, 0, 32, 1, 98, height, 0,
                               27, height);
            break;
        case 2:
            session->WoodenSupportsPrependTo = sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_TO_STEEP+11), 0, 0, 32, 1, 98, height, 0,
                               27, height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_TO_STEEP+15), 0, 0, 32, 20, 3, height, 0,
                               6, height);
            break;
        }
wooden_a_supports_paint_setup(session, direction & 1, 62 + direction, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
        switch (direction)
        {
        case 1:
            paint_util_push_tunnel_right(session, height + 24, TUNNEL_SQUARE_8);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height + 24, TUNNEL_SQUARE_8);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 80, 0x20);
        break;
    }
}

/** rct2: 0x008ADEE4 */
static void hybrid_rc_track_60_deg_up_to_flat_long_base(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                      uint8_t direction, int32_t height, const TileElement * tileElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_TO_STEEP+16), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        case 1:
            session->WoodenSupportsPrependTo = sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_TO_STEEP+20), 0, 0, 32, 1, 98, height, 0, 27,
                               height);
            break;
        case 2:
            session->WoodenSupportsPrependTo = sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_TO_STEEP+24), 0, 0, 32, 1, 98, height, 0, 27,
                               height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_TO_STEEP+28), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        }
wooden_a_supports_paint_setup(session, direction & 1, 66 + direction, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_7);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 80, 0x20);
        break;
    case 1:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_TO_STEEP+17), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_TO_STEEP+21), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_TO_STEEP+25), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_TO_STEEP+29), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        }
wooden_a_supports_paint_setup(session, direction & 1, 70 + direction, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 80, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_TO_STEEP+18), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_TO_STEEP+22), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_TO_STEEP+26), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_TO_STEEP+30), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        }
wooden_a_supports_paint_setup(session, direction & 1, 74 + direction, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 3:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_TO_STEEP+19), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_TO_STEEP+23), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        case 2:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_TO_STEEP+27), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_FLAT_TO_STEEP+31), 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        }
wooden_a_supports_paint_setup(session, direction & 1, 78 + direction, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
        switch (direction)
        {
        case 1:
            paint_util_push_tunnel_right(session, height + 8, TUNNEL_SQUARE_FLAT);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height + 8, TUNNEL_SQUARE_FLAT);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 40, 0x20);
        break;
    }
}

/** rct2: 0x008ADEF4 */
static void hybrid_rc_track_flat_to_60_deg_down_long_base(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                        uint8_t direction, int32_t height, const TileElement * tileElement)
{
    hybrid_rc_track_60_deg_up_to_flat_long_base(session, rideIndex, 3 - trackSequence, (direction + 2) & 3, height, tileElement);
}

/** rct2: 0x008ADF04 */
static void hybrid_rc_track_60_deg_down_to_flat_long_base(paint_session * session, uint16_t rideIndex, uint8_t trackSequence,
                                                         uint8_t direction, int32_t height, const TileElement * tileElement)
{
    hybrid_rc_track_flat_to_60_deg_up_long_base(session, rideIndex, 3 - trackSequence, (direction + 2) & 3, height, tileElement);
}

static void hybrid_rc_track_block_brakes(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction,
                                       int32_t height, const TileElement * tileElement)
{
    switch (direction)
    {
    case 0:
    case 2:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BLOCK_BRAKE+0), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        break;
    case 1:
    case 3:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BLOCK_BRAKE+1), 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        break;
    }
wooden_a_supports_paint_setup(session, direction & 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}


static void hybrid_rc_track_booster(paint_session * session, uint16_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
                                  const TileElement * tileElement)
{
    // These offsets could be moved to the g2.dat file when that supports offsets.
    int8_t ne_sw_offsetX = 7;
    int8_t ne_sw_offsetY = -15;
    int8_t nw_se_offsetX = -15;
    int8_t nw_se_offsetY = 7;

    switch (direction)
    {
    case 0:
    case 2:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BOOSTER+0),
                           ne_sw_offsetX, ne_sw_offsetY, 32, 20, 3, height, 0, 6, height);
        break;
    case 1:
    case 3:
        sub_98197C_rotated(session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_BOOSTER+1),
                           nw_se_offsetX, nw_se_offsetY, 32, 20, 3, height, 0, 6, height);
        break;
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENTS_ALL, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}


static void hybrid_rc_track_powered_lift(paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
                                const TileElement* tileElement)
{
    switch (direction)
    {
        case 0:
            sub_98197C_rotated(
                session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_POWERED_LIFT+0), 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 1:
            sub_98197C_rotated(
                session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_POWERED_LIFT+1), 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 2:
            sub_98197C_rotated(
                session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_POWERED_LIFT+2), 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
        case 3:
            sub_98197C_rotated(
                session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_POWERED_LIFT+3), 0, 0, 32, 20, 3, height, 0, 6, height);
            break;
    }
wooden_a_supports_paint_setup(session, direction & 1, 9 + direction, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_8);
    }
    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 56, 0x20);
}

static void hybrid_rc_track_left_corkscrew_up(
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
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_CORKSCREW+0), 0, 0, 32, 20, 3, height, 0, 6,
                        height + 4);
			wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_CORKSCREW+3), 0, 0, 32, 20, 3, height, 0, 6,
                        height + 4);
			wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_CORKSCREW+6), 0, 0, 32, 20, 3, height, 0, 6,
                        height + 4);
			wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_CORKSCREW+9), 0, 0, 32, 20, 3, height, 0, 6,
                        height + 4);
			wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
            }
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_CORKSCREW+1), 0, 0, 20, 20, 3, height, 6, 6,
                        height + 10);
			wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_CORKSCREW+4), 0, 0, 20, 20, 3, height, 6, 6,
                        height + 10);
			wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_CORKSCREW+7), 0, 0, 20, 20, 3, height, 6, 6,
                        height + 10);
			wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_CORKSCREW+10), 0, 0, 20, 20, 3, height, 6, 6,
                        height + 10);
			wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }
            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_CORKSCREW+2), 0, 0, 20, 32, 3, height, 6, 0,
                        height + 24);
			wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_CORKSCREW+5), 0, 0, 20, 32, 3, height, 6, 0,
                        height + 24);
			wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_CORKSCREW+8), 0, 0, 20, 32, 3, height, 6, 0,
                        height + 24);
			wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_CORKSCREW+11), 0, 0, 20, 32, 3, height, 6, 0,
                        height + 24);
			wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }

            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 2:
                    paint_util_push_tunnel_right(session, height + 8, TUNNEL_0);
                    break;
                case 3:
                    paint_util_push_tunnel_left(session, height + 8, TUNNEL_0);
                    break;
            }
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
    }
}

static void hybrid_rc_track_right_corkscrew_up(
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
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_CORKSCREW+12), 0, 0, 32, 20, 3, height, 0, 6,
                        height + 4);
			wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_CORKSCREW+15), 0, 0, 32, 20, 3, height, 0, 6,
                        height + 4);
			wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_CORKSCREW+18), 0, 0, 32, 20, 3, height, 0, 6,
                        height + 4);
			wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_CORKSCREW+21), 0, 0, 32, 20, 3, height, 0, 6,
                        height + 4);
			wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_CORKSCREW+13), 0, 0, 20, 20, 3, height, 6, 6,
                        height + 10);
			wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_CORKSCREW+16), 0, 0, 20, 20, 3, height, 6, 6,
                        height + 10);
			wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_CORKSCREW+19), 0, 0, 20, 20, 3, height, 6, 6,
                        height + 10);
			wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_CORKSCREW+22), 0, 0, 20, 20, 3, height, 6, 6,
                        height + 10);
			wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }
            paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_CORKSCREW+14), 0, 0, 20, 32, 3, height, 6, 0,
                        height + 24);
			wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_CORKSCREW+17), 0, 0, 20, 32, 3, height, 6, 0,
                        height + 24);
			wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_CORKSCREW+20), 0, 0, 20, 32, 3, height, 6, 0,
                        height + 24);
			wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_CORKSCREW+23), 0, 0, 20, 32, 3, height, 6, 0,
                        height + 24);
			wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }

            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    paint_util_push_tunnel_right(session, height + 8, TUNNEL_0);
                    break;
                case 1:
                    paint_util_push_tunnel_left(session, height + 8, TUNNEL_0);
                    break;
            }
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
    }
}

static void hybrid_rc_track_left_corkscrew_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    hybrid_rc_track_right_corkscrew_up(session, rideIndex, 2 - trackSequence, (direction + 1) & 3, height, tileElement);
}

static void hybrid_rc_track_right_corkscrew_down(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    hybrid_rc_track_left_corkscrew_up(session, rideIndex, 2 - trackSequence, (direction - 1) & 3, height, tileElement);
}

static void hybrid_rc_left_bank_to_left_quarter_turn_3_25_deg_up(
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
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+0) , 0, 6, 32, 20, 3, height);
		    wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+2), 0, 6, 32, 20, 3, height);
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+3), 0, 6, 32, 1, 26, height, 0, 27,
                        height);
		    wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+5), 0, 6, 32, 20, 3, height);
		    wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+7), 0, 6, 32, 20, 3, height);
		    wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 64, 0x20);
            break;
        case 1:
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                   wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                   break;
                case 1:
                   wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                   break;
                case 2:
                   wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                   break;
                case 3:
                   wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                   break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    sub_98196C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+1), 6, 0, 20, 32, 3, height);
		    wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+4), 6, 0, 20, 32, 3, height);
		    wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+6), 6, 0, 20, 32, 3, height);
		    wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+8), 6, 0, 20, 32, 3, height);
		    wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }
            switch (direction)
            {
                case 2:
                    paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_8);
                    break;
                case 3:
                    paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_8);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 64, 0x20);
            break;
    }
}

static void hybrid_rc_right_bank_to_right_quarter_turn_3_25_deg_up(
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
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+9), 0, 6, 32, 20, 3, height);
		    wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+11), 0, 6, 32, 20, 3, height);
		    wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+13), 0, 6, 32, 20, 3, height);
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+14), 0, 6, 32, 1, 26, height, 0, 27,
                        height);
		    wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+16), 0, 6, 32, 20, 3, height);
		    wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 64, 0x20);
            break;
        case 1:
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                   wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                   break;
                case 1:
                   wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                   break;
                case 2:
                   wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                   break;
                case 3:
                   wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                   break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    sub_98196C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+10), 6, 0, 20, 32, 3, height);
		    wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+12), 6, 0, 20, 32, 3, height);
		    wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+15), 6, 0, 20, 32, 3, height);
		    wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+17), 6, 0, 20, 32, 3, height);
		    wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }
            switch (direction)
            {
                case 0:
                    paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_8);
                    break;
                case 1:
                    paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_8);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 64, 0x20);
            break;
    }
}

static void hybrid_rc_left_quarter_turn_3_25_deg_down_to_left_bank(
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
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+12), 0, 6, 32, 20, 3, height);
		    wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+15), 0, 6, 32, 20, 3, height);
		    wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+17), 0, 6, 32, 20, 3, height);
		    wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+10), 0, 6, 32, 20, 3, height);
		    wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_8);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 64, 0x20);
            break;
        case 1:
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                   wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                   break;
                case 1:
                   wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                   break;
                case 2:
                   wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                   break;
                case 3:
                   wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                   break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    sub_98196C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+11), 6, 0, 20, 32, 3, height);
		    wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+13), 6, 0, 20, 32, 3, height);
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+14), 6, 0, 1, 32, 26, height, 27, 0,
                        height);
		    wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+16), 6, 0, 20, 32, 3, height);
		    wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+9), 6, 0, 20, 32, 3, height);
		    wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }
            switch (direction)
            {
                case 2:
                    paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
                    break;
                case 3:
                    paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 64, 0x20);
            break;
    }
}

static void hybrid_rc_right_quarter_turn_3_25_deg_down_to_right_bank(
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
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+8), 0, 6, 32, 20, 3, height);
		    wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+1), 0, 6, 32, 20, 3, height);
		    wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+4), 0, 6, 32, 20, 3, height);
		    wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+6), 0, 6, 32, 20, 3, height);
		    wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_8);
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 64, 0x20);
            break;
        case 1:
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                   wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                   break;
                case 1:
                   wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                   break;
                case 2:
                   wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                   break;
                case 3:
                   wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                   break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    sub_98196C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+7), 6, 0, 20, 32, 3, height);
		    wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 1:
                    sub_98196C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+0), 6, 0, 20, 32, 3, height);
		    wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 2:
                    sub_98196C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+2), 6, 0, 20, 32, 3, height);
                    sub_98197C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+3), 6, 0, 1, 32, 26, height, 27, 0,
                        height);
		    wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
                case 3:
                    sub_98196C_rotated(
                        session, direction, hybrid_rc_get_track_colour(session) | (SPR_G2_HYBRID_TRACK_TURN_BANK_TRANSITION+5), 6, 0, 20, 32, 3, height);
		    wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], nullptr);
                    break;
            }
            switch (direction)
            {
                case 0:
                    paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
                    break;
                case 1:
                    paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
                    break;
            }
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENTS_ALL, direction), 0xFFFF, 0);
            paint_util_set_general_support_height(session, height + 64, 0x20);
            break;
    }
}


TRACK_PAINT_FUNCTION get_track_paint_function_hybrid_rc(int32_t trackType)
{
    switch (trackType)
    {
    case TrackElemType::Flat:
        return hybrid_rc_track_flat;
    case TrackElemType::EndStation:
    case TrackElemType::BeginStation:
    case TrackElemType::MiddleStation:
        return hybrid_rc_track_station;
    case TrackElemType::Up25:
        return hybrid_rc_track_25_deg_up;
    case TrackElemType::Up60:
        return hybrid_rc_track_60_deg_up;
    case TrackElemType::FlatToUp25:
        return hybrid_rc_track_flat_to_25_deg_up;
    case TrackElemType::Up25ToUp60:
        return hybrid_rc_track_25_deg_up_to_60_deg_up;
    case TrackElemType::Up60ToUp25:
        return hybrid_rc_track_60_deg_up_to_25_deg_up;
    case TrackElemType::Up25ToFlat:
        return hybrid_rc_track_25_deg_up_to_flat;
    case TrackElemType::Down25:
        return hybrid_rc_track_25_deg_down;
    case TrackElemType::Down60:
        return hybrid_rc_track_60_deg_down;
    case TrackElemType::FlatToDown25:
        return hybrid_rc_track_flat_to_25_deg_down;
    case TrackElemType::Down25ToDown60:
        return hybrid_rc_track_25_deg_down_to_60_deg_down;
    case TrackElemType::Down60ToDown25:
        return hybrid_rc_track_60_deg_down_to_25_deg_down;
    case TrackElemType::Down25ToFlat:
        return hybrid_rc_track_25_deg_down_to_flat;
    case TrackElemType::Up90:
        return hybrid_rc_track_90_deg_up;
    case TrackElemType::Down90:
        return hybrid_rc_track_90_deg_down;
    case TrackElemType::Up60ToUp90:
        return hybrid_rc_track_60_deg_up_to_90_deg_up;
    case TrackElemType::Down90ToDown60:
        return hybrid_rc_track_90_deg_down_to_60_deg_down;
    case TrackElemType::Up90ToUp60:
        return hybrid_rc_track_90_deg_up_to_60_deg_up;
    case TrackElemType::Down60ToDown90:
        return hybrid_rc_track_60_deg_down_to_90_deg_down;
    case TrackElemType::LeftQuarterTurn5Tiles:
        return hybrid_rc_track_left_quarter_turn_5;
    case TrackElemType::RightQuarterTurn5Tiles:
        return hybrid_rc_track_right_quarter_turn_5;
    case TrackElemType::FlatToLeftBank:
        return hybrid_rc_track_flat_to_left_bank;
    case TrackElemType::LeftCorkscrewUp:
        return hybrid_rc_track_left_corkscrew_up;
    case TrackElemType::RightCorkscrewUp:
        return hybrid_rc_track_right_corkscrew_up;
    case TrackElemType::LeftCorkscrewDown:
        return hybrid_rc_track_left_corkscrew_down;
    case TrackElemType::RightCorkscrewDown:
        return hybrid_rc_track_right_corkscrew_down;
    case TrackElemType::FlatToRightBank:
        return hybrid_rc_track_flat_to_right_bank;
    case TrackElemType::LeftBankToFlat:
        return hybrid_rc_track_left_bank_to_flat;
    case TrackElemType::RightBankToFlat:
        return hybrid_rc_track_right_bank_to_flat;
    case TrackElemType::BankedLeftQuarterTurn5Tiles:
        return hybrid_rc_track_banked_left_quarter_turn_5;
    case TrackElemType::BankedRightQuarterTurn5Tiles:
        return hybrid_rc_track_banked_right_quarter_turn_5;
    case TrackElemType::LeftBankToUp25:
        return hybrid_rc_track_left_bank_to_25_deg_up;
    case TrackElemType::RightBankToUp25:
        return hybrid_rc_track_right_bank_to_25_deg_up;
    case TrackElemType::Up25ToLeftBank:
        return hybrid_rc_track_25_deg_up_to_left_bank;
    case TrackElemType::Up25ToRightBank:
        return hybrid_rc_track_25_deg_up_to_right_bank;
    case TrackElemType::LeftBankToDown25:
        return hybrid_rc_track_left_bank_to_25_deg_down;
    case TrackElemType::RightBankToDown25:
        return hybrid_rc_track_right_bank_to_25_deg_down;
    case TrackElemType::Down25ToLeftBank:
        return hybrid_rc_track_25_deg_down_to_left_bank;
    case TrackElemType::Down25ToRightBank:
        return hybrid_rc_track_25_deg_down_to_right_bank;
    case TrackElemType::LeftBank:
        return hybrid_rc_track_left_bank;
    case TrackElemType::RightBank:
        return hybrid_rc_track_right_bank;
    case TrackElemType::LeftQuarterTurn5TilesUp25:
        return hybrid_rc_track_left_quarter_turn_5_25_deg_up;
    case TrackElemType::RightQuarterTurn5TilesUp25:
        return hybrid_rc_track_right_quarter_turn_5_25_deg_up;
    case TrackElemType::LeftQuarterTurn5TilesDown25:
        return hybrid_rc_track_left_quarter_turn_5_25_deg_down;
    case TrackElemType::RightQuarterTurn5TilesDown25:
        return hybrid_rc_track_right_quarter_turn_5_25_deg_down;
    case TrackElemType::SBendLeft:
        return hybrid_rc_track_s_bend_left;
    case TrackElemType::SBendRight:
        return hybrid_rc_track_s_bend_right;
    case TrackElemType::LeftQuarterTurn3Tiles:
        return hybrid_rc_track_left_quarter_turn_3;
    case TrackElemType::RightQuarterTurn3Tiles:
        return hybrid_rc_track_right_quarter_turn_3;
    case TrackElemType::LeftBankedQuarterTurn3Tiles:
        return hybrid_rc_track_left_quarter_turn_3_bank;
    case TrackElemType::RightBankedQuarterTurn3Tiles:
        return hybrid_rc_track_right_quarter_turn_3_bank;
    case TrackElemType::LeftQuarterTurn3TilesUp25:
        return hybrid_rc_track_left_quarter_turn_3_25_deg_up;
    case TrackElemType::RightQuarterTurn3TilesUp25:
        return hybrid_rc_track_right_quarter_turn_3_25_deg_up;
    case TrackElemType::LeftQuarterTurn3TilesDown25:
        return hybrid_rc_track_left_quarter_turn_3_25_deg_down;
    case TrackElemType::RightQuarterTurn3TilesDown25:
        return hybrid_rc_track_right_quarter_turn_3_25_deg_down;
    case TrackElemType::LeftHalfBankedHelixUpSmall:
        return hybrid_rc_track_left_half_banked_helix_up_small;
    case TrackElemType::RightHalfBankedHelixUpSmall:
        return hybrid_rc_track_right_half_banked_helix_up_small;
    case TrackElemType::LeftHalfBankedHelixDownSmall:
        return hybrid_rc_track_left_half_banked_helix_down_small;
    case TrackElemType::RightHalfBankedHelixDownSmall:
        return hybrid_rc_track_right_half_banked_helix_down_small;
    case TrackElemType::LeftHalfBankedHelixUpLarge:
        return hybrid_rc_track_left_half_banked_helix_up_large;
    case TrackElemType::RightHalfBankedHelixUpLarge:
        return hybrid_rc_track_right_half_banked_helix_up_large;
    case TrackElemType::LeftHalfBankedHelixDownLarge:
        return hybrid_rc_track_left_half_banked_helix_down_large;
    case TrackElemType::RightHalfBankedHelixDownLarge:
        return hybrid_rc_track_right_half_banked_helix_down_large;
    case TrackElemType::LeftQuarterTurn1TileUp60:
        return hybrid_rc_track_left_quarter_turn_1_60_deg_up;
    case TrackElemType::RightQuarterTurn1TileUp60:
        return hybrid_rc_track_right_quarter_turn_1_60_deg_up;
    case TrackElemType::LeftQuarterTurn1TileDown60:
        return hybrid_rc_track_left_quarter_turn_1_60_deg_down;
    case TrackElemType::RightQuarterTurn1TileDown60:
        return hybrid_rc_track_right_quarter_turn_1_60_deg_down;
    case TrackElemType::Brakes:
        return hybrid_rc_track_brakes;
    case TrackElemType::Up25LeftBanked:
        return hybrid_rc_track_25_deg_up_left_banked;
    case TrackElemType::Up25RightBanked:
        return hybrid_rc_track_25_deg_up_right_banked;
    case TrackElemType::OnRidePhoto:
        return hybrid_rc_track_on_ride_photo;
    case TrackElemType::Down25LeftBanked:
        return hybrid_rc_track_25_deg_down_left_banked;
    case TrackElemType::Down25RightBanked:
        return hybrid_rc_track_25_deg_down_right_banked;
    case TrackElemType::FlatToUp60LongBase:
        return hybrid_rc_track_flat_to_60_deg_up_long_base;
    case TrackElemType::Up60ToFlatLongBase:
        return hybrid_rc_track_60_deg_up_to_flat_long_base;
    case TrackElemType::FlatToDown60LongBase:
        return hybrid_rc_track_flat_to_60_deg_down_long_base;
    case TrackElemType::Down60ToFlatLongBase:
        return hybrid_rc_track_60_deg_down_to_flat_long_base;
    case TrackElemType::LeftEighthToDiag:
        return hybrid_rc_track_left_eighth_to_diag;
    case TrackElemType::RightEighthToDiag:
        return hybrid_rc_track_right_eighth_to_diag;
    case TrackElemType::LeftEighthToOrthogonal:
        return hybrid_rc_track_left_eighth_to_orthogonal;
    case TrackElemType::RightEighthToOrthogonal:
        return hybrid_rc_track_right_eighth_to_orthogonal;
    case TrackElemType::LeftEighthBankToDiag:
        return hybrid_rc_track_left_eighth_bank_to_diag;
    case TrackElemType::RightEighthBankToDiag:
        return hybrid_rc_track_right_eighth_bank_to_diag;
    case TrackElemType::LeftEighthBankToOrthogonal:
        return hybrid_rc_track_left_eighth_bank_to_orthogonal;
    case TrackElemType::RightEighthBankToOrthogonal:
        return hybrid_rc_track_right_eighth_bank_to_orthogonal;
    case TrackElemType::DiagFlat:
        return hybrid_rc_track_diag_flat;
    case TrackElemType::DiagUp25:
        return hybrid_rc_track_diag_25_deg_up;
    case TrackElemType::DiagUp60:
        return hybrid_rc_track_diag_60_deg_up;
    case TrackElemType::DiagFlatToUp25:
        return hybrid_rc_track_diag_flat_to_25_deg_up;
    case TrackElemType::DiagUp25ToUp60:
        return hybrid_rc_track_diag_25_deg_up_to_60_deg_up;
    case TrackElemType::DiagUp60ToUp25:
        return hybrid_rc_track_diag_60_deg_up_to_25_deg_up;
    case TrackElemType::DiagUp25ToFlat:
        return hybrid_rc_track_diag_25_deg_up_to_flat;
    case TrackElemType::DiagDown25:
        return hybrid_rc_track_diag_25_deg_down;
    case TrackElemType::DiagDown60:
        return hybrid_rc_track_diag_60_deg_down;
    case TrackElemType::DiagFlatToDown25:
        return hybrid_rc_track_diag_flat_to_25_deg_down;
    case TrackElemType::DiagDown25ToDown60:
        return hybrid_rc_track_diag_25_deg_down_to_60_deg_down;
    case TrackElemType::DiagDown60ToDown25:
        return hybrid_rc_track_diag_60_deg_down_to_25_deg_down;
    case TrackElemType::DiagDown25ToFlat:
        return hybrid_rc_track_diag_25_deg_down_to_flat;
    case TrackElemType::DiagFlatToLeftBank:
        return hybrid_rc_track_diag_flat_to_left_bank;
    case TrackElemType::DiagFlatToRightBank:
        return hybrid_rc_track_diag_flat_to_right_bank;
    case TrackElemType::DiagLeftBankToFlat:
        return hybrid_rc_track_diag_left_bank_to_flat;
    case TrackElemType::DiagRightBankToFlat:
        return hybrid_rc_track_diag_right_bank_to_flat;
    case TrackElemType::DiagLeftBankToUp25:
        return hybrid_rc_track_diag_left_bank_to_25_deg_up;
    case TrackElemType::DiagRightBankToUp25:
        return hybrid_rc_track_diag_right_bank_to_25_deg_up;
    case TrackElemType::DiagUp25ToLeftBank:
        return hybrid_rc_track_diag_25_deg_up_to_left_bank;
    case TrackElemType::DiagUp25ToRightBank:
        return hybrid_rc_track_diag_25_deg_up_to_right_bank;
    case TrackElemType::DiagLeftBankToDown25:
        return hybrid_rc_track_diag_left_bank_to_25_deg_down;
    case TrackElemType::DiagRightBankToDown25:
        return hybrid_rc_track_diag_right_bank_to_25_deg_down;
    case TrackElemType::DiagDown25ToLeftBank:
        return hybrid_rc_track_diag_25_deg_down_to_left_bank;
    case TrackElemType::DiagDown25ToRightBank:
        return hybrid_rc_track_diag_25_deg_down_to_right_bank;
    case TrackElemType::DiagLeftBank:
        return hybrid_rc_track_diag_left_bank;
    case TrackElemType::DiagRightBank:
        return hybrid_rc_track_diag_right_bank;
    case TrackElemType::BlockBrakes:
        return hybrid_rc_track_block_brakes;
    case TrackElemType::LeftBankedQuarterTurn3TileUp25:
        return hybrid_rc_track_left_banked_quarter_turn_3_25_deg_up;
    case TrackElemType::RightBankedQuarterTurn3TileUp25:
        return hybrid_rc_track_right_banked_quarter_turn_3_25_deg_up;
    case TrackElemType::LeftBankedQuarterTurn3TileDown25:
        return hybrid_rc_track_left_banked_quarter_turn_3_25_deg_down;
    case TrackElemType::RightBankedQuarterTurn3TileDown25:
        return hybrid_rc_track_right_banked_quarter_turn_3_25_deg_down;
    case TrackElemType::LeftBankedQuarterTurn5TileUp25:
        return hybrid_rc_track_left_banked_quarter_turn_5_25_deg_up;
    case TrackElemType::RightBankedQuarterTurn5TileUp25:
        return hybrid_rc_track_right_banked_quarter_turn_5_25_deg_up;
    case TrackElemType::LeftBankedQuarterTurn5TileDown25:
        return hybrid_rc_track_left_banked_quarter_turn_5_25_deg_down;
    case TrackElemType::RightBankedQuarterTurn5TileDown25:
        return hybrid_rc_track_right_banked_quarter_turn_5_25_deg_down;
    case TrackElemType::Up25ToLeftBankedUp25:
        return hybrid_rc_track_25_deg_up_to_left_banked_25_deg_up;
    case TrackElemType::Up25ToRightBankedUp25:
        return hybrid_rc_track_25_deg_up_to_right_banked_25_deg_up;
    case TrackElemType::LeftBankedUp25ToUp25:
        return hybrid_rc_track_left_banked_25_deg_up_to_25_deg_up;
    case TrackElemType::RightBankedUp25ToUp25:
        return hybrid_rc_track_right_banked_25_deg_up_to_25_deg_up;
    case TrackElemType::Down25ToLeftBankedDown25:
        return hybrid_rc_track_25_deg_down_to_left_banked_25_deg_down;
    case TrackElemType::Down25ToRightBankedDown25:
        return hybrid_rc_track_25_deg_down_to_right_banked_25_deg_down;
    case TrackElemType::LeftBankedDown25ToDown25:
        return hybrid_rc_track_left_banked_25_deg_down_to_25_deg_down;
    case TrackElemType::RightBankedDown25ToDown25:
        return hybrid_rc_track_right_banked_25_deg_down_to_25_deg_down;
    case TrackElemType::LeftBankedFlatToLeftBankedUp25:
        return hybrid_rc_track_left_banked_flat_to_left_banked_25_deg_up;
    case TrackElemType::RightBankedFlatToRightBankedUp25:
        return hybrid_rc_track_right_banked_flat_to_right_banked_25_deg_up;
    case TrackElemType::LeftBankedUp25ToLeftBankedFlat:
        return hybrid_rc_track_left_banked_25_deg_up_to_left_banked_flat;
    case TrackElemType::RightBankedUp25ToRightBankedFlat:
        return hybrid_rc_track_right_banked_25_deg_up_to_right_banked_flat;
    case TrackElemType::LeftBankedFlatToLeftBankedDown25:
        return hybrid_rc_track_left_banked_flat_to_left_banked_25_deg_down;
    case TrackElemType::RightBankedFlatToRightBankedDown25:
        return hybrid_rc_track_right_banked_flat_to_right_banked_25_deg_down;
    case TrackElemType::LeftBankedDown25ToLeftBankedFlat:
        return hybrid_rc_track_left_banked_25_deg_down_to_left_banked_flat;
    case TrackElemType::RightBankedDown25ToRightBankedFlat:
        return hybrid_rc_track_right_banked_25_deg_down_to_right_banked_flat;
    case TrackElemType::FlatToLeftBankedUp25:
        return hybrid_rc_track_flat_to_left_banked_25_deg_up;
    case TrackElemType::FlatToRightBankedUp25:
        return hybrid_rc_track_flat_to_right_banked_25_deg_up;
    case TrackElemType::LeftBankedUp25ToFlat:
        return hybrid_rc_track_left_banked_25_deg_up_to_flat;
    case TrackElemType::RightBankedUp25ToFlat:
        return hybrid_rc_track_right_banked_25_deg_up_to_flat;
    case TrackElemType::FlatToLeftBankedDown25:
        return hybrid_rc_track_flat_to_left_banked_25_deg_down;
    case TrackElemType::FlatToRightBankedDown25:
        return hybrid_rc_track_flat_to_right_banked_25_deg_down;
    case TrackElemType::LeftBankedDown25ToFlat:
        return hybrid_rc_track_left_banked_25_deg_down_to_flat;
    case TrackElemType::RightBankedDown25ToFlat:
        return hybrid_rc_track_right_banked_25_deg_down_to_flat;
    case TrackElemType::LeftBankToLeftQuarterTurn3TilesUp25:
        return hybrid_rc_left_bank_to_left_quarter_turn_3_25_deg_up;
    case TrackElemType::RightBankToRightQuarterTurn3TilesUp25:
        return hybrid_rc_right_bank_to_right_quarter_turn_3_25_deg_up;
    case TrackElemType::LeftQuarterTurn3TilesDown25ToLeftBank:
        return hybrid_rc_left_quarter_turn_3_25_deg_down_to_left_bank;
    case TrackElemType::RightQuarterTurn3TilesDown25ToRightBank:
        return hybrid_rc_right_quarter_turn_3_25_deg_down_to_right_bank;
    case TrackElemType::LeftQuarterTurn1TileUp90:
        return hybrid_rc_track_left_quarter_turn_1_90_deg_up;
    case TrackElemType::RightQuarterTurn1TileUp90:
        return hybrid_rc_track_right_quarter_turn_1_90_deg_up;
    case TrackElemType::LeftQuarterTurn1TileDown90:
        return hybrid_rc_track_left_quarter_turn_1_90_deg_down;
    case TrackElemType::RightQuarterTurn1TileDown90:
        return hybrid_rc_track_right_quarter_turn_1_90_deg_down;
    case TrackElemType::LeftBarrelRollUpToDown:
        return hybrid_rc_track_left_barrel_roll_up_to_down;
    case TrackElemType::RightBarrelRollUpToDown:
        return hybrid_rc_track_right_barrel_roll_up_to_down;
    case TrackElemType::LeftBarrelRollDownToUp:
        return hybrid_rc_track_left_barrel_roll_down_to_up;
    case TrackElemType::RightBarrelRollDownToUp:
        return hybrid_rc_track_right_barrel_roll_down_to_up;
    case TrackElemType::Up90ToInvertedFlatQuarterLoop:
        return hybrid_rc_track_90_deg_to_inverted_flat_quarter_loop_up;
    case TrackElemType::InvertedFlatToDown90QuarterLoop:
        return hybrid_rc_track_inverted_flat_to_90_deg_quarter_loop_down;
    case TrackElemType::PoweredLift:
	return hybrid_rc_track_powered_lift;
    case TrackElemType::Booster:
        return hybrid_rc_track_booster;
    }
    return nullptr;
}
