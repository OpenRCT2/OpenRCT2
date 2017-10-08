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
#include "../../paint/supports.h"
#include "../../interface/viewport.h"
#include "../../paint/map_element/map_element.h"
#include "../../paint/paint.h"
#include "../../sprites.h"
#include "../../world/map.h"
#include "../../world/sprite.h"
#include "../ride_data.h"
#include "../track_data.h"
#include "../track_paint.h"
#include "../vehicle_paint.h"

#ifndef NO_VEHICLES
/**
 *
 *  rct2: 0x006D4453
 */
void vehicle_visual_reverser(paint_session * session, sint32 x, sint32 imageDirection, sint32 y, sint32 z, rct_vehicle *vehicle, const rct_ride_entry_vehicle *vehicleEntry)
{
    rct_vehicle *v1 = GET_VEHICLE(vehicle->prev_vehicle_on_ride);
    rct_vehicle *v2 = GET_VEHICLE(vehicle->next_vehicle_on_ride);
    x = (v1->x + v2->x) / 2;
    y = (v1->y + v2->y) / 2;
    z = (v1->z + v2->z) / 2;
    session->SpritePosition.x = x;
    session->SpritePosition.y = y;
    vehicle_visual_default(session, x, imageDirection, y, z, vehicle, vehicleEntry);
}
#endif

/** rct2: 0x0086E65C */
static void reverser_rc_track_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21520, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21521, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
    } else {
        switch (direction) {
        case 0:
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21504, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21505, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

static void reverser_rc_track_station(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    static const uint32 imageIds[4][3] = {
        { 21506, SPR_STATION_BASE_A_SW_NE },
        { 21507, SPR_STATION_BASE_A_NW_SE },
        { 21506, SPR_STATION_BASE_A_SW_NE },
        { 21507, SPR_STATION_BASE_A_NW_SE },
    };

    sub_98197C_rotated(session, direction, imageIds[direction][1] | session->TrackColours[SCHEME_MISC], 0, 0, 32, 27, 2, height, 0, 2, height);
    sub_98199C_rotated(session, direction, imageIds[direction][0] | session->TrackColours[SCHEME_TRACK], 0, 0, 32, 27, 2, height, 0, 2, height);
    wooden_a_supports_paint_setup(session, direction & 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
    track_paint_util_draw_station_2(session, rideIndex, trackSequence, direction, height, mapElement, 9, 11);
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0086E66C */
static void reverser_rc_track_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21530, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 9, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21531, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 10, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21532, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 11, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21533, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 12, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21516, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 9, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21517, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 10, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21518, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 11, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21519, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 12, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_7);
    } else {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_8);
    }
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 56, 0x20);
}

/** rct2: 0x0086E67C */
static void reverser_rc_track_flat_to_25_deg_up(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21522, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 1, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21523, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 2, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21524, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 3, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21525, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 4, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21508, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 1, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21509, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 2, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21510, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 3, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21511, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 4, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
    } else {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_8);
    }
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 48, 0x20);
}

/** rct2: 0x0086E68C */
static void reverser_rc_track_25_deg_up_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    if (track_element_is_lift_hill(mapElement)) {
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21526, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 5, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21527, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 6, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21528, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 7, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21529, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 8, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
    } else {
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21512, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 5, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21513, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 6, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21514, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 7, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21515, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 8, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
    }
    if (direction == 0 || direction == 3) {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_6);
    } else {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_14);
    }
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 40, 0x20);
}

/** rct2: 0x0086E69C */
static void reverser_rc_track_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    reverser_rc_track_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x0086E6AC */
static void reverser_rc_track_flat_to_25_deg_down(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    reverser_rc_track_25_deg_up_to_flat(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x0086E6BC */
static void reverser_rc_track_25_deg_down_to_flat(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    reverser_rc_track_flat_to_25_deg_up(session, rideIndex, trackSequence, (direction + 2) & 3, height, mapElement);
}

/** rct2: 0x0086E6CC */
static void reverser_rc_track_left_quarter_turn_5(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21543, 0, 2, 32, 27, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21548, 0, 2, 32, 27, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21553, 0, 2, 32, 32, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21538, 0, 2, 32, 32, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21542, 0, 0, 32, 16, 2, height);
            wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21547, 0, 0, 32, 16, 2, height);
            wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21552, 0, 16, 32, 16, 2, height);
            wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21537, 0, 16, 32, 16, 2, height);
            wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21541, 0, 16, 16, 16, 2, height);
            wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21546, 16, 16, 16, 16, 2, height);
            wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21551, 16, 0, 16, 16, 2, height);
            wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21536, 0, 0, 16, 16, 2, height);
            wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 4:
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21540, 16, 0, 16, 34, 2, height);
            wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21545, 0, 0, 16, 32, 2, height);
            wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21550, 0, 0, 16, 32, 2, height);
            wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21535, 16, 0, 16, 32, 2, height);
            wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 6:
        switch (direction) {
        case 0:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21539, 2, 0, 32, 32, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21544, 2, 0, 27, 32, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21549, 2, 0, 27, 32, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98196C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21534, 2, 0, 32, 32, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(session, height, TUNNEL_6);
            break;
        case 3:
            paint_util_push_tunnel_left(session, height, TUNNEL_6);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x0086E6DC */
static void reverser_rc_track_right_quarter_turn_5(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    reverser_rc_track_left_quarter_turn_5(session, rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x0086E6EC */
static void reverser_rc_track_s_bend_left(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21566, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21570, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21569, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21573, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21567, 0, 0, 32, 26, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21571, 0, 0, 32, 26, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21568, 0, 0, 32, 26, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21572, 0, 0, 32, 26, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21568, 0, 0, 32, 26, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21572, 0, 0, 32, 26, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21567, 0, 0, 32, 26, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21571, 0, 0, 32, 26, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21569, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21573, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21566, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21570, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 1:
            paint_util_push_tunnel_right(session, height, TUNNEL_6);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height, TUNNEL_6);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x0086E6FC */
static void reverser_rc_track_s_bend_right(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21574, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21578, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21577, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21581, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21575, 0, 0, 32, 26, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21579, 0, 0, 32, 26, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21576, 0, 0, 32, 26, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21580, 0, 0, 32, 26, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21576, 0, 0, 32, 26, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21580, 0, 0, 32, 26, 2, height, 0, 0, height);
            wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21575, 0, 0, 32, 26, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21579, 0, 0, 32, 26, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21577, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21581, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21574, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21578, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 1:
            paint_util_push_tunnel_right(session, height, TUNNEL_6);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height, TUNNEL_6);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x0086E73C */
static void reverser_rc_track_left_quarter_turn_3(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21559, 0, 0, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21562, 0, 0, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21565, 0, 0, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21556, 0, 0, 32, 20, 2, height, 0, 6, height);
            wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21558, 0, 0, 16, 16, 2, height, 16, 0, height);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21561, 0, 0, 16, 16, 2, height, 0, 0, height);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21564, 0, 0, 16, 16, 2, height, 0, 16, height);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21555, 0, 0, 16, 16, 2, height, 16, 16, height);
            break;
        }
        paint_util_set_segment_support_height(session, paint_util_rotate_segments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21557, 0, 0, 20, 32, 2, height, 6, 0, height);
            wooden_a_supports_paint_setup(session, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21560, 0, 0, 20, 32, 2, height, 6, 0, height);
            wooden_a_supports_paint_setup(session, 2, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21563, 0, 0, 20, 32, 2, height, 6, 0, height);
            wooden_a_supports_paint_setup(session, 3, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21554, 0, 0, 20, 32, 2, height, 6, 0, height);
            wooden_a_supports_paint_setup(session, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 2:
            paint_util_push_tunnel_right(session, height, TUNNEL_6);
            break;
        case 3:
            paint_util_push_tunnel_left(session, height, TUNNEL_6);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x0086E74C */
static void reverser_rc_track_right_quarter_turn_3(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    reverser_rc_track_left_quarter_turn_3(session, rideIndex, trackSequence, (direction - 1) & 3, height, mapElement);
}

/** rct2: 0x0086E75C */
static void reverser_rc_track_brakes(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (direction) {
    case 0:
    case 2:
        sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21506, 0, 0, 32, 27, 2, height, 0, 2, height);
        wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
        break;
    case 1:
    case 3:
        sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21507, 0, 0, 32, 27, 2, height, 0, 2, height);
        wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
        break;
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/** rct2: 0x0086E76C */
static void reverser_rc_track_left_reverser(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21582, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21588, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21594, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21600, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21585, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21591, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21597, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21603, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21586, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21592, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21598, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21604, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21587, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21593, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21599, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21605, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 4:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21583, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21589, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21595, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21601, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21584, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21590, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21596, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21602, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 1:
            paint_util_push_tunnel_right(session, height, TUNNEL_6);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height, TUNNEL_6);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

/** rct2: 0x0086E77C */
static void reverser_rc_track_right_reverser(paint_session * session, uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    switch (trackSequence) {
    case 0:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21596, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21602, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21584, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21590, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        if (direction == 0 || direction == 3) {
            paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_6);
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 1:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21599, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21605, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21587, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21593, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 2:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21598, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21604, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21586, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21592, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 3:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21597, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21603, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21585, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21591, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 4:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21595, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21601, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21583, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21589, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    case 5:
        switch (direction) {
        case 0:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21594, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 1:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21600, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 2:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21582, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 0, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        case 3:
            sub_98197C_rotated(session, direction, session->TrackColours[SCHEME_TRACK] | 21588, 0, 0, 32, 27, 2, height, 0, 2, height);
            wooden_a_supports_paint_setup(session, 1, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
            break;
        }
        switch (direction) {
        case 1:
            paint_util_push_tunnel_right(session, height, TUNNEL_6);
            break;
        case 2:
            paint_util_push_tunnel_left(session, height, TUNNEL_6);
            break;
        }
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        paint_util_set_general_support_height(session, height + 32, 0x20);
        break;
    }
}

TRACK_PAINT_FUNCTION get_track_paint_function_reverser_rc(sint32 trackType, sint32 direction)
{
    switch (trackType) {
    case TRACK_ELEM_FLAT:
        return reverser_rc_track_flat;
    case TRACK_ELEM_END_STATION:
    case TRACK_ELEM_BEGIN_STATION:
    case TRACK_ELEM_MIDDLE_STATION:
        return reverser_rc_track_station;
    case TRACK_ELEM_25_DEG_UP:
        return reverser_rc_track_25_deg_up;
    case TRACK_ELEM_FLAT_TO_25_DEG_UP:
        return reverser_rc_track_flat_to_25_deg_up;
    case TRACK_ELEM_25_DEG_UP_TO_FLAT:
        return reverser_rc_track_25_deg_up_to_flat;
    case TRACK_ELEM_25_DEG_DOWN:
        return reverser_rc_track_25_deg_down;
    case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
        return reverser_rc_track_flat_to_25_deg_down;
    case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
        return reverser_rc_track_25_deg_down_to_flat;
    case TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES:
        return reverser_rc_track_left_quarter_turn_5;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES:
        return reverser_rc_track_right_quarter_turn_5;
    case TRACK_ELEM_S_BEND_LEFT:
        return reverser_rc_track_s_bend_left;
    case TRACK_ELEM_S_BEND_RIGHT:
        return reverser_rc_track_s_bend_right;
    case TRACK_ELEM_LEFT_QUARTER_TURN_3_TILES:
        return reverser_rc_track_left_quarter_turn_3;
    case TRACK_ELEM_RIGHT_QUARTER_TURN_3_TILES:
        return reverser_rc_track_right_quarter_turn_3;
    case TRACK_ELEM_BRAKES:
        return reverser_rc_track_brakes;
    case TRACK_ELEM_LEFT_REVERSER:
        return reverser_rc_track_left_reverser;
    case TRACK_ELEM_RIGHT_REVERSER:
        return reverser_rc_track_right_reverser;
    }
    return NULL;
}
