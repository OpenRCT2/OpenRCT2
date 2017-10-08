#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../../drawing/drawing.h"
#include "../../interface/viewport.h"
#include "../../paint/map_element/map_element.h"
#include "../../paint/paint.h"
#include "../../paint/supports.h"
#include "../../sprites.h"
#include "../../world/map.h"
#include "../../world/sprite.h"
#include "../ride_data.h"
#include "../track_data.h"
#include "../track_paint.h"

/** rct2: 0x008A6D50, 0x008A6D60, 0x008A6D70 */
static void lim_launched_rc_track_station(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction,
                                          sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][3] = {
        { 15018, 15020, SPR_STATION_BASE_B_SW_NE },
        { 15019, 15021, SPR_STATION_BASE_B_NW_SE },
        { 15018, 15020, SPR_STATION_BASE_B_SW_NE },
        { 15019, 15021, SPR_STATION_BASE_B_NW_SE },
    };

    if (mapElement->properties.track.type == TRACK_ELEM_END_STATION)
    {
        sub_98197C_rotated(session, direction, imageIds[direction][1] | session->TrackColours[SCHEME_TRACK], 0, 0, 32, 20, 1,
                           height, 0, 6, height + 3);
    }
    else
    {
        sub_98197C_rotated(session, direction, imageIds[direction][0] | session->TrackColours[SCHEME_TRACK], 0, 0, 32, 20, 1,
                           height, 0, 6, height + 3);
    }
    sub_98196C_rotated(session, direction, imageIds[direction][2] | session->TrackColours[SCHEME_MISC], 0, 0, 32, 32, 1,
                       height);
    track_paint_util_draw_station_metal_supports_2(session, direction, height, session->TrackColours[SCHEME_SUPPORTS], 0);
    track_paint_util_draw_station(session, rideIndex, trackSequence, direction, height, mapElement);
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x008A65E0 */
static void lim_launched_rc_track_left_vertical_loop(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                     uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15388, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15396, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15395, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15403, 0, 6, 32, 20, 7, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
            break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 1:
        switch (direction)
        {
        case 0:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15389, 0, 0, 32, 26, 3, height);
            break;
        case 1:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15397, 0, 14, 32, 2, 63, height);
            break;
        case 2:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15394, 0, 6, 32, 26, 3, height);
            break;
        case 3:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15402, 0, 6, 32, 26, 3, height);
            break;
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15390, 16, 0, 3, 16, 119, height, 16,
                               0, height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15398, 12, 0, 3, 16, 119, height, 12,
                               0, height);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15393, 10, 16, 4, 16, 119, height, 10,
                               16, height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15401, 16, 16, 2, 16, 119, height, 16,
                               16, height);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 168, 0x20);
        break;
    case 3:
        switch (direction)
        {
        case 0:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15391, 0, 0, 32, 16, 3, height + 32);
            break;
        case 1:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15399, 0, 0, 32, 16, 3, height + 32);
            break;
        case 2:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15392, 0, 16, 32, 16, 3, height + 32);
            break;
        case 3:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15400, 0, 16, 32, 16, 3, height + 32);
            break;
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 4:
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 5:
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 6:
        switch (direction)
        {
        case 0:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15392, 0, 16, 32, 16, 3, height + 32);
            break;
        case 1:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15400, 0, 16, 32, 16, 3, height + 32);
            break;
        case 2:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15391, 0, 0, 32, 16, 3, height + 32);
            break;
        case 3:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15399, 0, 0, 32, 16, 3, height + 32);
            break;
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 7:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15393, 10, 16, 4, 16, 119, height, 10,
                               16, height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15401, 16, 16, 2, 16, 119, height, 16,
                               16, height);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15390, 16, 0, 3, 16, 119, height, 16,
                               0, height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15398, 12, 0, 3, 16, 119, height, 12,
                               0, height);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 168, 0x20);
        break;
    case 8:
        switch (direction)
        {
        case 0:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15394, 0, 6, 32, 26, 3, height);
            break;
        case 1:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15402, 0, 6, 32, 26, 3, height);
            break;
        case 2:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15389, 0, 0, 32, 26, 3, height);
            break;
        case 3:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15397, 0, 14, 32, 2, 63, height);
            break;
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 9:
        switch (direction)
        {
        case 0:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15395, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15403, 0, 6, 32, 20, 7, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session->TrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15388, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15396, 0, 6, 32, 20, 3, height);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
            break;
        }
        switch (direction)
        {
        case 1:
            paint_util_push_tunnel_right(session, height - 8, TUNNEL_1);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height - 8, TUNNEL_1);
            break;
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    }
}

/** rct2: 0x008A65F0 */
static void lim_launched_rc_track_right_vertical_loop(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                      uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15419, 0, 6, 32, 20, 7, height);
            break;
        case 1:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15411, 0, 6, 32, 20, 3, height);
            break;
        case 2:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15412, 0, 6, 32, 20, 3, height);
            break;
        case 3:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15404, 0, 6, 32, 20, 3, height);
            break;
        }
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
        }
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 1:
        switch (direction)
        {
        case 0:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15418, 0, 6, 32, 26, 3, height);
            break;
        case 1:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15410, 0, 6, 32, 26, 3, height);
            break;
        case 2:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15413, 0, 14, 32, 2, 63, height);
            break;
        case 3:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15405, 0, 0, 32, 26, 3, height);
            break;
        }
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15417, 16, 16, 2, 16, 119, height, 16,
                               16, height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15409, 10, 16, 4, 16, 119, height, 10,
                               16, height);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15414, 12, 0, 3, 16, 119, height, 12,
                               0, height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15406, 16, 0, 2, 16, 119, height, 16,
                               0, height);
            break;
        }
        paint_util_set_general_support_height(session, height + 168, 0x20);
        break;
    case 3:
        switch (direction)
        {
        case 0:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15416, 0, 16, 32, 16, 3, height + 32);
            break;
        case 1:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15408, 0, 16, 32, 16, 3, height + 32);
            break;
        case 2:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15415, 0, 0, 32, 16, 3, height + 32);
            break;
        case 3:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15407, 0, 0, 32, 16, 3, height + 32);
            break;
        }
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 4:
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 5:
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 6:
        switch (direction)
        {
        case 0:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15415, 0, 0, 32, 16, 3, height + 32);
            break;
        case 1:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15407, 0, 0, 32, 16, 3, height + 32);
            break;
        case 2:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15416, 0, 16, 32, 16, 3, height + 32);
            break;
        case 3:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15408, 0, 16, 32, 16, 3, height + 32);
            break;
        }
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 7:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15414, 12, 0, 3, 16, 119, height, 12,
                               0, height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15406, 16, 0, 2, 16, 119, height, 16,
                               0, height);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15417, 16, 16, 2, 16, 119, height, 16,
                               16, height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15409, 10, 16, 4, 16, 119, height, 10,
                               16, height);
            break;
        }
        paint_util_set_general_support_height(session, height + 168, 0x20);
        break;
    case 8:
        switch (direction)
        {
        case 0:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15413, 0, 14, 32, 2, 63, height);
            break;
        case 1:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15405, 0, 0, 32, 26, 3, height);
            break;
        case 2:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15418, 0, 6, 32, 26, 3, height);
            break;
        case 3:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15410, 0, 6, 32, 26, 3, height);
            break;
        }
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 9:
        switch (direction)
        {
        case 0:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15412, 0, 6, 32, 20, 3, height);
            break;
        case 1:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15404, 0, 6, 32, 20, 3, height);
            break;
        case 2:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15419, 0, 6, 32, 20, 7, height);
            break;
        case 3:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15411, 0, 6, 32, 20, 3, height);
            break;
        }
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session->TrackColours[SCHEME_SUPPORTS]);
        switch (direction)
        {
        case 1:
            paint_util_push_tunnel_right(session, height - 8, TUNNEL_1);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height - 8, TUNNEL_1);
            break;
        }
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    }

    track_paint_util_right_vertical_loop_segments(session, direction, trackSequence);
}

/** rct2: 0x008A6D10 */
static void lim_launched_rc_track_left_twist_down_to_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                        uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15758, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15782, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 28);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15761, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15785, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 28);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15764, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15788, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 28);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15767, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15791, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 28);
            break;
        }
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15759, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15783, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 28);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15762, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15786, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 28);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15765, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15789, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 28);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15768, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15792, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 28);
            break;
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15760, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15784, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 44);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15763, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15787, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 44);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15766, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15790, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 44);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15769, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15793, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 44);
            break;
        }
        switch (direction)
        {
        case 1:
            paint_util_push_tunnel_right(session, height - 32, TUNNEL_3);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height - 32, TUNNEL_3);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height, 0x20);
        break;
    }
}

/** rct2: 0x008A6D20 */
static void lim_launched_rc_track_right_twist_down_to_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                         uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15770, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15794, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 28);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15773, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15797, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 28);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15776, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15800, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 28);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15779, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15803, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 28);
            break;
        }
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15771, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15795, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 28);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15774, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15798, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 28);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15777, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15801, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 28);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15780, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15804, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 28);
            break;
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15772, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15796, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 44);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15775, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15799, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 44);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15778, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15802, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 44);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15781, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15805, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 44);
            break;
        }
        switch (direction)
        {
        case 1:
            paint_util_push_tunnel_right(session, height + 32, TUNNEL_3);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height + 32, TUNNEL_3);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height, 0x20);
        break;
    }
}

/** rct2: 0x008A6D30 */
static void lim_launched_rc_track_left_twist_up_to_down(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                        uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15766, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15790, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 44);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15769, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15793, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 44);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15760, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15784, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 44);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15763, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15787, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 44);
            break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 32, TUNNEL_3);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height, 0x20);
        break;
    case 1:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15765, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15789, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 28);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15768, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15792, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 28);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15759, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15783, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 28);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15762, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15786, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 28);
            break;
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15764, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15788, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 28);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15767, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15791, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 28);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15758, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15782, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 28);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15761, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15785, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 28);
            break;
        }
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
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
            session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008A6D40 */
static void lim_launched_rc_track_right_twist_up_to_down(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                         uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15778, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15802, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 44);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15781, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15805, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 44);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15772, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15796, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 44);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15775, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15799, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 44);
            break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height + 32, TUNNEL_3);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 64, 0x20);
        break;
    case 1:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15777, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15801, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 28);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15780, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15804, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 28);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15771, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15795, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 28);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15774, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15798, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 28);
            break;
        }
        paint_util_set_segment_support_height(
            session,
            paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15776, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15800, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 28);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15779, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15803, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 28);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15770, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15794, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 28);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15773, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15797, 0, 0, 32, 20, 0, height, 0, 6,
                               height + 28);
            break;
        }
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
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
            session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x008A6CD0 */
static void lim_launched_rc_track_left_corkscrew_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                    uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15734, 0, 0, 32, 20, 3, height, 0, 6,
                               height + 4);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15737, 0, 0, 32, 20, 3, height, 0, 6,
                               height + 4);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15740, 0, 0, 32, 20, 3, height, 0, 6,
                               height + 4);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15743, 0, 0, 32, 20, 3, height, 0, 6,
                               height + 4);
            break;
        }

        track_paint_util_left_corkscrew_up_supports(session, direction, height);

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
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15735, 0, 0, 20, 20, 3, height, 6, 6,
                               height + 10);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15738, 0, 0, 20, 20, 3, height, 6, 6,
                               height + 10);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15741, 0, 0, 20, 20, 3, height, 6, 6,
                               height + 10);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15744, 0, 0, 20, 20, 3, height, 6, 6,
                               height + 10);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15736, 0, 0, 20, 32, 3, height, 6, 0,
                               height + 24);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15739, 0, 0, 20, 32, 3, height, 6, 0,
                               height + 24);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15742, 0, 0, 20, 32, 3, height, 6, 0,
                               height + 24);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15745, 0, 0, 20, 32, 3, height, 6, 0,
                               height + 24);
            break;
        }

        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height + 35, session->TrackColours[SCHEME_SUPPORTS]);

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

/** rct2: 0x008A6CE0 */
static void lim_launched_rc_track_right_corkscrew_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                     uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15746, 0, 0, 32, 20, 3, height, 0, 6,
                               height + 4);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15749, 0, 0, 32, 20, 3, height, 0, 6,
                               height + 4);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15752, 0, 0, 32, 20, 3, height, 0, 6,
                               height + 4);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15755, 0, 0, 32, 20, 3, height, 0, 6,
                               height + 4);
            break;
        }
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
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
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15747, 0, 0, 20, 20, 3, height, 6, 6,
                               height + 10);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15750, 0, 0, 20, 20, 3, height, 6, 6,
                               height + 10);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15753, 0, 0, 20, 20, 3, height, 6, 6,
                               height + 10);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15756, 0, 0, 20, 20, 3, height, 6, 6,
                               height + 10);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 72, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15748, 0, 0, 20, 32, 3, height, 6, 0,
                               height + 24);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15751, 0, 0, 20, 32, 3, height, 6, 0,
                               height + 24);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15754, 0, 0, 20, 32, 3, height, 6, 0,
                               height + 24);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15757, 0, 0, 20, 32, 3, height, 6, 0,
                               height + 24);
            break;
        }

        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
            0xFFFF, 0);
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height + 35, session->TrackColours[SCHEME_SUPPORTS]);

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

/** rct2: 0x008A6CF0 */
static void lim_launched_rc_track_left_corkscrew_down(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                      uint8 direction, sint32 height, rct_map_element * mapElement)
{
    lim_launched_rc_track_right_corkscrew_up(session, rideIndex, 2 - trackSequence, (direction + 1) & 3, height, mapElement);
}

/** rct2: 0x008A6D00 */
static void lim_launched_rc_track_right_corkscrew_down(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                       uint8 direction, sint32 height, rct_map_element * mapElement)
{
    lim_launched_rc_track_left_corkscrew_up(session, rideIndex, 2 - trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x008A6D80 */
static void lim_launched_rc_track_brakes(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction,
                                         sint32 height, rct_map_element * mapElement)
{
    switch (direction)
    {
    case 0:
    case 2:
        sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15018, 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        break;
    case 1:
    case 3:
        sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15019, 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x008A6C10 */
static void lim_launched_rc_track_90_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction,
                                            sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15702, 0, 0, 2, 20, 31, height, 4, 6,
                               height + 8);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15703, 0, 0, 2, 20, 31, height, 24, 6,
                               height + 8);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15704, 0, 0, 2, 20, 31, height, 24, 6,
                               height + 8);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15705, 0, 0, 2, 20, 31, height, 4, 6,
                               height + 8);
            break;
        }
        paint_util_set_vertical_tunnel(session, height + 32);
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        break;
    }
}

/** rct2: 0x008A6C20 */
static void lim_launched_rc_track_90_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction,
                                              sint32 height, rct_map_element * mapElement)
{
    lim_launched_rc_track_90_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A6C30 */
static void lim_launched_rc_track_60_deg_up_to_90_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                         uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15694, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15695, 0, 0, 2, 20, 55, height, 24, 6,
                               height);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15696, 0, 0, 2, 20, 55, height, 24, 6,
                               height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15697, 0, 0, 32, 20, 3, height, 0, 6,
                               height);
            break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_1);
        }
        paint_util_set_vertical_tunnel(session, height + 56);
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 56, 0x20);
        break;
    case 1:
        break;
    }
}

/** rct2: 0x008A6C40 */
static void lim_launched_rc_track_90_deg_down_to_60_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                             uint8 direction, sint32 height, rct_map_element * mapElement)
{
    lim_launched_rc_track_60_deg_up_to_90_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x008A6C50 */
static void lim_launched_rc_track_90_deg_up_to_60_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                         uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction)
    {
    case 0:
        sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15698, 0, 0, 6, 20, 3, height, 0, 6,
                           height + 2);
        break;
    case 1:
        sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15699, 0, 0, 2, 20, 31, height, 39, 6,
                           height + 8);
        break;
    case 2:
        sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15700, 0, 0, 2, 20, 31, height, 39, 6,
                           height + 8);
        break;
    case 3:
        sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15701, 0, 0, 6, 20, 3, height, 0, 6,
                           height + 2);
        break;
    }
    switch (direction)
    {
    case 1:
        paint_util_push_tunnel_right(session, height + 48, TUNNEL_2);
        break;
    case 2:
        paint_util_push_tunnel_left(session, height + 48, TUNNEL_2);
        break;
    }
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 80, 0x20);
}

/** rct2: 0x008A6C60 */
static void lim_launched_rc_track_60_deg_down_to_90_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                             uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15700, 0, 0, 2, 20, 31, height, 39, 6,
                               height + 8);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15701, 0, 0, 6, 20, 3, height, 0, 6,
                               height + 2);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15698, 0, 0, 6, 20, 3, height, 0, 6,
                               height + 2);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15699, 0, 0, 2, 20, 31, height, 39, 6,
                               height + 8);
            break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height + 48, TUNNEL_2);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 80, 0x20);
        break;
    case 1:
        break;
    }
}

/** rct2: 0x008A6C70 */
static void lim_launched_rc_track_90_deg_to_inverted_flat_quarter_loop_up(paint_session * session, uint8 rideIndex,
                                                                          uint8 trackSequence, uint8 direction, sint32 height,
                                                                          rct_map_element * mapElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15722, 0, 0, 2, 20, 31, height, 4, 6,
                               height + 8);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15725, 0, 0, 2, 20, 31, height, 24, 6,
                               height + 8);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15728, 0, 0, 2, 20, 31, height, 24, 6,
                               height + 8);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15731, 0, 0, 2, 20, 31, height, 4, 6,
                               height + 8);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 88, 0x20);
        break;
    case 1:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15723, 0, 0, 2, 20, 31, height, -8, 6,
                               height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15726, 0, 0, 2, 20, 63, height, 24, 6,
                               height + 8);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15729, 0, 0, 2, 20, 63, height, 24, 6,
                               height + 8);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15732, 0, 0, 2, 20, 31, height, -8, 6,
                               height);
            break;
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 64, 0x20);
        break;
    case 2:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15724, 0, 0, 32, 20, 3, height, 0, 6,
                               height + 24);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15727, 0, 0, 2, 20, 31, height, 24, 6,
                               height + 8);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15730, 0, 0, 2, 20, 31, height, 24, 6,
                               height + 8);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15733, 0, 0, 32, 20, 3, height, 0, 6,
                               height + 24);
            break;
        }
        if (direction == 0 || direction == 3)
        {
            paint_util_push_tunnel_rotated(session, direction, height + 16, TUNNEL_0);
        }
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 48, 0x20);
        break;
    }
}

/** rct2: 0x008A6C80 */
static void lim_launched_rc_track_inverted_flat_to_90_deg_quarter_loop_down(paint_session * session, uint8 rideIndex,
                                                                            uint8 trackSequence, uint8 direction, sint32 height,
                                                                            rct_map_element * mapElement)
{
    lim_launched_rc_track_90_deg_to_inverted_flat_quarter_loop_up(session, rideIndex, 2 - trackSequence, direction, height,
                                                                  mapElement);
}

/** rct2: 0x008A6D90 */
static void lim_launched_rc_track_block_brakes(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction,
                                               sint32 height, rct_map_element * mapElement)
{
    switch (direction)
    {
    case 0:
    case 2:
        sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15020, 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        break;
    case 1:
    case 3:
        sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15021, 0, 0, 32, 20, 3, height, 0, 6,
                           height);
        break;
    }
    if (track_paint_util_should_paint_supports(session->MapPosition))
    {
        metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_0);
    paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction),
                                          0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

static void lim_launched_rc_track_left_quarter_turn_1_90_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                                uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15706, 0, 0, 2, 20, 63, height, 4, 6,
                               height + 8);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15707, 0, 0, 2, 20, 63, height, 4, 6,
                               height + 8);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15715, 0, 0, 2, 20, 63, height, 24, 6,
                               height + 8);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15716, 0, 0, 2, 2, 63, height, 24, 24,
                               height + 8);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15709, 0, 0, 2, 20, 63, height, 4, 6,
                               height + 8);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15717, 0, 0, 2, 20, 63, height, 24, 6,
                               height + 8);
            break;
        }
        paint_util_set_vertical_tunnel(session, height + 96);
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 96, 0x20);
        break;
    case 1:
        break;
    }
}

/** rct2: 0x008A6CA0 */
static void lim_launched_rc_track_right_quarter_turn_1_90_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                                 uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence)
    {
    case 0:
        switch (direction)
        {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15710, 0, 0, 2, 20, 63, height, 4, 6,
                               height + 8);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15718, 0, 0, 2, 20, 63, height, 24, 6,
                               height + 8);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15719, 0, 0, 2, 2, 63, height, 24, 24,
                               height + 8);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15712, 0, 0, 2, 20, 63, height, 4, 6,
                               height + 8);
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15720, 0, 0, 2, 20, 63, height, 24, 6,
                               height + 8);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 15713, 0, 0, 2, 20, 63, height, 4, 6,
                               height + 8);
            break;
        }
        paint_util_set_vertical_tunnel(session, height + 96);
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 96, 0x20);
        break;
    case 1:
        break;
    }
}

/** rct2: 0x008A6CB0 */
static void lim_launched_rc_track_left_quarter_turn_1_90_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence,
                                                                  uint8 direction, sint32 height, rct_map_element * mapElement)
{
    lim_launched_rc_track_right_quarter_turn_1_90_deg_up(session, rideIndex, trackSequence, (direction + 1) & 3, height,
                                                         mapElement);
}

/** rct2: 0x008A6CC0 */
static void lim_launched_rc_track_right_quarter_turn_1_90_deg_down(paint_session * session, uint8 rideIndex,
                                                                   uint8 trackSequence, uint8 direction, sint32 height,
                                                                   rct_map_element * mapElement)
{
    lim_launched_rc_track_left_quarter_turn_1_90_deg_up(session, rideIndex, trackSequence, (direction - 1) & 3, height,
                                                        mapElement);
}

TRACK_PAINT_FUNCTION get_track_paint_function_lim_launched_rc(sint32 trackType, sint32 direction)
{
    switch (trackType)
    {

    // Use Looping Roller Coaster
    case TRACK_ELEM_FLAT:
    case TRACK_ELEM_25_DEG_UP:
    case TRACK_ELEM_60_DEG_UP:
    case TRACK_ELEM_FLAT_TO_25_DEG_UP:
    case TRACK_ELEM_25_DEG_UP_TO_60_DEG_UP:
    case TRACK_ELEM_60_DEG_UP_TO_25_DEG_UP:
    case TRACK_ELEM_25_DEG_UP_TO_FLAT:
    case TRACK_ELEM_25_DEG_DOWN:
    case TRACK_ELEM_60_DEG_DOWN:
    case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
    case TRACK_ELEM_25_DEG_DOWN_TO_60_DEG_DOWN:
    case TRACK_ELEM_60_DEG_DOWN_TO_25_DEG_DOWN:
    case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES:
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES:
    case TRACK_ELEM_FLAT_TO_LEFT_BANK:
    case TRACK_ELEM_FLAT_TO_RIGHT_BANK:
    case TRACK_ELEM_LEFT_BANK_TO_FLAT:
    case TRACK_ELEM_RIGHT_BANK_TO_FLAT:
    case TRACK_ELEM_BANKED_LEFT_QUARTER_TURN_5_TILES:
    case TRACK_ELEM_BANKED_RIGHT_QUARTER_TURN_5_TILES:
    case TRACK_ELEM_LEFT_BANK_TO_25_DEG_UP:
    case TRACK_ELEM_RIGHT_BANK_TO_25_DEG_UP:
    case TRACK_ELEM_25_DEG_UP_TO_LEFT_BANK:
    case TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANK:
    case TRACK_ELEM_LEFT_BANK_TO_25_DEG_DOWN:
    case TRACK_ELEM_RIGHT_BANK_TO_25_DEG_DOWN:
    case TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANK:
    case TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANK:
    case TRACK_ELEM_LEFT_BANK:
    case TRACK_ELEM_RIGHT_BANK:
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_UP:
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_UP:
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES_25_DEG_DOWN:
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES_25_DEG_DOWN:
    case TRACK_ELEM_S_BEND_LEFT:
    case TRACK_ELEM_S_BEND_RIGHT:
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_BANK:
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_BANK:
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_UP:
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_UP:
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES_25_DEG_DOWN:
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES_25_DEG_DOWN:
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_SMALL:
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_SMALL:
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_SMALL:
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_SMALL:
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_LARGE:
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_LARGE:
    case TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_LARGE:
    case TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_LARGE:
    case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_UP:
    case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_UP:
    case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_60_DEG_DOWN:
    case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_60_DEG_DOWN:
    case TRACK_ELEM_25_DEG_UP_LEFT_BANKED:
    case TRACK_ELEM_25_DEG_UP_RIGHT_BANKED:
    case TRACK_ELEM_ON_RIDE_PHOTO:
    case TRACK_ELEM_25_DEG_DOWN_LEFT_BANKED:
    case TRACK_ELEM_25_DEG_DOWN_RIGHT_BANKED:
    case TRACK_ELEM_LEFT_EIGHTH_TO_DIAG:
    case TRACK_ELEM_RIGHT_EIGHTH_TO_DIAG:
    case TRACK_ELEM_LEFT_EIGHTH_TO_ORTHOGONAL:
    case TRACK_ELEM_RIGHT_EIGHTH_TO_ORTHOGONAL:
    case TRACK_ELEM_LEFT_EIGHTH_BANK_TO_DIAG:
    case TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_DIAG:
    case TRACK_ELEM_LEFT_EIGHTH_BANK_TO_ORTHOGONAL:
    case TRACK_ELEM_RIGHT_EIGHTH_BANK_TO_ORTHOGONAL:
    case TRACK_ELEM_DIAG_FLAT:
    case TRACK_ELEM_DIAG_25_DEG_UP:
    case TRACK_ELEM_DIAG_60_DEG_UP:
    case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_UP:
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_60_DEG_UP:
    case TRACK_ELEM_DIAG_60_DEG_UP_TO_25_DEG_UP:
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT:
    case TRACK_ELEM_DIAG_25_DEG_DOWN:
    case TRACK_ELEM_DIAG_60_DEG_DOWN:
    case TRACK_ELEM_DIAG_FLAT_TO_25_DEG_DOWN:
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_60_DEG_DOWN:
    case TRACK_ELEM_DIAG_60_DEG_DOWN_TO_25_DEG_DOWN:
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_FLAT:
    case TRACK_ELEM_DIAG_FLAT_TO_LEFT_BANK:
    case TRACK_ELEM_DIAG_FLAT_TO_RIGHT_BANK:
    case TRACK_ELEM_DIAG_LEFT_BANK_TO_FLAT:
    case TRACK_ELEM_DIAG_RIGHT_BANK_TO_FLAT:
    case TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_UP:
    case TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_UP:
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_LEFT_BANK:
    case TRACK_ELEM_DIAG_25_DEG_UP_TO_RIGHT_BANK:
    case TRACK_ELEM_DIAG_LEFT_BANK_TO_25_DEG_DOWN:
    case TRACK_ELEM_DIAG_RIGHT_BANK_TO_25_DEG_DOWN:
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_LEFT_BANK:
    case TRACK_ELEM_DIAG_25_DEG_DOWN_TO_RIGHT_BANK:
    case TRACK_ELEM_DIAG_LEFT_BANK:
    case TRACK_ELEM_DIAG_RIGHT_BANK:
    case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP:
    case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_UP:
    case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN:
    case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_3_TILE_25_DEG_DOWN:
    case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP:
    case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_UP:
    case TRACK_ELEM_LEFT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN:
    case TRACK_ELEM_RIGHT_BANKED_QUARTER_TURN_5_TILE_25_DEG_DOWN:
    case TRACK_ELEM_25_DEG_UP_TO_LEFT_BANKED_25_DEG_UP:
    case TRACK_ELEM_25_DEG_UP_TO_RIGHT_BANKED_25_DEG_UP:
    case TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_25_DEG_UP:
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_25_DEG_UP:
    case TRACK_ELEM_25_DEG_DOWN_TO_LEFT_BANKED_25_DEG_DOWN:
    case TRACK_ELEM_25_DEG_DOWN_TO_RIGHT_BANKED_25_DEG_DOWN:
    case TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN:
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_25_DEG_DOWN:
    case TRACK_ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_UP:
    case TRACK_ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_UP:
    case TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_LEFT_BANKED_FLAT:
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_RIGHT_BANKED_FLAT:
    case TRACK_ELEM_LEFT_BANKED_FLAT_TO_LEFT_BANKED_25_DEG_DOWN:
    case TRACK_ELEM_RIGHT_BANKED_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN:
    case TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_LEFT_BANKED_FLAT:
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_RIGHT_BANKED_FLAT:
    case TRACK_ELEM_FLAT_TO_LEFT_BANKED_25_DEG_UP:
    case TRACK_ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_UP:
    case TRACK_ELEM_LEFT_BANKED_25_DEG_UP_TO_FLAT:
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_UP_TO_FLAT:
    case TRACK_ELEM_FLAT_TO_LEFT_BANKED_25_DEG_DOWN:
    case TRACK_ELEM_FLAT_TO_RIGHT_BANKED_25_DEG_DOWN:
    case TRACK_ELEM_LEFT_BANKED_25_DEG_DOWN_TO_FLAT:
    case TRACK_ELEM_RIGHT_BANKED_25_DEG_DOWN_TO_FLAT:
        return get_track_paint_function_looping_rc(trackType, direction);

    case TRACK_ELEM_END_STATION:
    case TRACK_ELEM_BEGIN_STATION:
    case TRACK_ELEM_MIDDLE_STATION:
        return lim_launched_rc_track_station;
    case TRACK_ELEM_LEFT_VERTICAL_LOOP:
        return lim_launched_rc_track_left_vertical_loop;
    case TRACK_ELEM_RIGHT_VERTICAL_LOOP:
        return lim_launched_rc_track_right_vertical_loop;
    case TRACK_ELEM_LEFT_TWIST_DOWN_TO_UP:
        return lim_launched_rc_track_left_twist_down_to_up;
    case TRACK_ELEM_RIGHT_TWIST_DOWN_TO_UP:
        return lim_launched_rc_track_right_twist_down_to_up;
    case TRACK_ELEM_LEFT_TWIST_UP_TO_DOWN:
        return lim_launched_rc_track_left_twist_up_to_down;
    case TRACK_ELEM_RIGHT_TWIST_UP_TO_DOWN:
        return lim_launched_rc_track_right_twist_up_to_down;
    case TRACK_ELEM_LEFT_CORKSCREW_UP:
        return lim_launched_rc_track_left_corkscrew_up;
    case TRACK_ELEM_RIGHT_CORKSCREW_UP:
        return lim_launched_rc_track_right_corkscrew_up;
    case TRACK_ELEM_LEFT_CORKSCREW_DOWN:
        return lim_launched_rc_track_left_corkscrew_down;
    case TRACK_ELEM_RIGHT_CORKSCREW_DOWN:
        return lim_launched_rc_track_right_corkscrew_down;
    case TRACK_ELEM_BRAKES:
        return lim_launched_rc_track_brakes;
    case TRACK_ELEM_90_DEG_UP:
        return lim_launched_rc_track_90_deg_up;
    case TRACK_ELEM_90_DEG_DOWN:
        return lim_launched_rc_track_90_deg_down;
    case TRACK_ELEM_60_DEG_UP_TO_90_DEG_UP:
        return lim_launched_rc_track_60_deg_up_to_90_deg_up;
    case TRACK_ELEM_90_DEG_DOWN_TO_60_DEG_DOWN:
        return lim_launched_rc_track_90_deg_down_to_60_deg_down;
    case TRACK_ELEM_90_DEG_UP_TO_60_DEG_UP:
        return lim_launched_rc_track_90_deg_up_to_60_deg_up;
    case TRACK_ELEM_60_DEG_DOWN_TO_90_DEG_DOWN:
        return lim_launched_rc_track_60_deg_down_to_90_deg_down;
    case TRACK_ELEM_90_DEG_TO_INVERTED_FLAT_QUARTER_LOOP_UP:
        return lim_launched_rc_track_90_deg_to_inverted_flat_quarter_loop_up;
    case TRACK_ELEM_INVERTED_FLAT_TO_90_DEG_QUARTER_LOOP_DOWN:
        return lim_launched_rc_track_inverted_flat_to_90_deg_quarter_loop_down;
    case TRACK_ELEM_BLOCK_BRAKES:
        return lim_launched_rc_track_block_brakes;
    case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_90_DEG_UP:
        return lim_launched_rc_track_left_quarter_turn_1_90_deg_up;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_90_DEG_UP:
        return lim_launched_rc_track_right_quarter_turn_1_90_deg_up;
    case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE_90_DEG_DOWN:
        return lim_launched_rc_track_left_quarter_turn_1_90_deg_down;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE_90_DEG_DOWN:
        return lim_launched_rc_track_right_quarter_turn_1_90_deg_down;
    }
    return NULL;
}
