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

#include "../../common.h"
#include "../../interface/viewport.h"
#include "../track_paint.h"
#include "../track.h"
#include "../../paint/paint.h"

enum
{
    SPR_BOAT_RIDE_FLAT_BACK_SW_NE = 28523,
    SPR_BOAT_RIDE_FLAT_FRONT_SW_NE = 28524,
    SPR_BOAT_RIDE_FLAT_BACK_NW_SE = 28525,
    SPR_BOAT_RIDE_FLAT_FRONT_NW_SE = 28526,
    SPR_BOAT_RIDE_FLAT_QUARTER_TURN_1_TILE_BACK_SW_NW = 28527,
    SPR_BOAT_RIDE_FLAT_QUARTER_TURN_1_TILE_FRONT_SW_NW = 28528,
    SPR_BOAT_RIDE_FLAT_QUARTER_TURN_1_TILE_BACK_NW_NE = 28529,
    SPR_BOAT_RIDE_FLAT_QUARTER_TURN_1_TILE_FRONT_NW_NE = 28530,
    SPR_BOAT_RIDE_FLAT_QUARTER_TURN_1_TILE_BACK_NE_SE = 28531,
    SPR_BOAT_RIDE_FLAT_QUARTER_TURN_1_TILE_FRONT_NE_SE = 28532,
    SPR_BOAT_RIDE_FLAT_QUARTER_TURN_1_TILE_FRONT_SE_SW = 28533,
    SPR_BOAT_RIDE_FLAT_QUARTER_TURN_1_TILE_BACK_SE_SW = 28534,
};

/** rct2: 0x008B0E40 */
static void paint_boat_ride_track_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId;

    if (direction & 1) {
        imageId = SPR_BOAT_RIDE_FLAT_BACK_NW_SE | gTrackColours[SCHEME_TRACK];
        sub_98197C(imageId, 0, 0, 1, 32, 3, height, 4, 0, height, get_current_rotation());

        imageId = SPR_BOAT_RIDE_FLAT_FRONT_NW_SE | gTrackColours[SCHEME_TRACK];
        sub_98197C(imageId, 0, 0, 1, 32, 3, height, 28, 0, height, get_current_rotation());
    } else {
        imageId = SPR_BOAT_RIDE_FLAT_BACK_SW_NE | gTrackColours[SCHEME_TRACK];
        sub_98197C(imageId, 0, 0, 32, 1, 3, height, 0, 4, height, get_current_rotation());

        imageId = SPR_BOAT_RIDE_FLAT_FRONT_SW_NE | gTrackColours[SCHEME_TRACK];
        sub_98197C(imageId, 0, 0, 32, 1, 3, height, 0, 28, height, get_current_rotation());
    }

    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 16, 0x20);
}

/** rct2: 0x008B0E50 */
static void paint_boat_ride_station(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    rct_xy16 position = gPaintMapPosition;
    rct_ride * ride = get_ride(rideIndex);
    const rct_ride_entrance_definition * entranceStyle = &RideEntranceDefinitions[ride->entrance_style];

    if (direction & 1) {
        paint_util_push_tunnel_right(height, TUNNEL_6);
        track_paint_util_draw_pier(ride, entranceStyle, position, direction, height, mapElement, get_current_rotation());
    } else {
        paint_util_push_tunnel_left(height, TUNNEL_6);
        track_paint_util_draw_pier(ride, entranceStyle, position, direction, height, mapElement, get_current_rotation());
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(height + 32, 0x20);
}

/** rct2: 0x008B0E80 */
static void paint_boat_ride_track_left_quarter_turn_1_tile(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    uint32 imageId;
    switch (direction) {
        case 0:
            imageId = SPR_BOAT_RIDE_FLAT_QUARTER_TURN_1_TILE_BACK_SW_NW | gTrackColours[SCHEME_TRACK];
            sub_98197C(imageId, 0, 0, 32, 32, 0, height, 0, 0, height, get_current_rotation());

            imageId = SPR_BOAT_RIDE_FLAT_QUARTER_TURN_1_TILE_FRONT_SW_NW | gTrackColours[SCHEME_TRACK];
            sub_98197C(imageId, 0, 0, 3, 3, 3, height, 28, 28, height + 2, get_current_rotation());
            break;
        case 1:
            imageId = SPR_BOAT_RIDE_FLAT_QUARTER_TURN_1_TILE_BACK_NW_NE | gTrackColours[SCHEME_TRACK];
            sub_98197C(imageId, 0, 0, 32, 32, 0, height, 0, 0, height, get_current_rotation());

            imageId = SPR_BOAT_RIDE_FLAT_QUARTER_TURN_1_TILE_FRONT_NW_NE | gTrackColours[SCHEME_TRACK];
            sub_98197C(imageId, 0, 0, 3, 3, 3, height, 28, 28, height + 2, get_current_rotation());
            break;
        case 2:
            imageId = SPR_BOAT_RIDE_FLAT_QUARTER_TURN_1_TILE_BACK_NE_SE | gTrackColours[SCHEME_TRACK];
            sub_98197C(imageId, 0, 0, 32, 32, 0, height, 0, 0, height, get_current_rotation());

            imageId = SPR_BOAT_RIDE_FLAT_QUARTER_TURN_1_TILE_FRONT_NE_SE | gTrackColours[SCHEME_TRACK];
            sub_98197C(imageId, 0, 0, 3, 3, 3, height, 28, 28, height + 2, get_current_rotation());
            break;
        case 3:
            imageId = SPR_BOAT_RIDE_FLAT_QUARTER_TURN_1_TILE_FRONT_SE_SW | gTrackColours[SCHEME_TRACK];
            sub_98197C(imageId, 0, 0, 3, 3, 3, height, 28, 28, height + 2, get_current_rotation());

            imageId = SPR_BOAT_RIDE_FLAT_QUARTER_TURN_1_TILE_BACK_SE_SW | gTrackColours[SCHEME_TRACK];
            sub_98197C(imageId, 0, 0, 32, 32, 0, height, 0, 0, height, get_current_rotation());
            break;
    }

    paint_util_set_segment_support_height(paint_util_rotate_segments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_C8, direction), 0xFFFF, 0);
    paint_util_set_general_support_height(height + 16, 0x20);
}

/** rct2: 0x008B0E90 */
static void paint_boat_ride_track_right_quarter_turn_1_tile(uint8 rideIndex, uint8 trackSequence, uint8 direction, sint32 height, rct_map_element * mapElement)
{
    paint_boat_ride_track_left_quarter_turn_1_tile(rideIndex, trackSequence, (direction + 3) % 4, height, mapElement);
}

/**
 * rct2: 0x008B0D60
 */
TRACK_PAINT_FUNCTION get_track_paint_function_boat_ride(sint32 trackType, sint32 direction)
{
    switch (trackType) {
        case TRACK_ELEM_FLAT:
            return paint_boat_ride_track_flat;

        case TRACK_ELEM_END_STATION:
        case TRACK_ELEM_BEGIN_STATION:
        case TRACK_ELEM_MIDDLE_STATION:
            return paint_boat_ride_station;

        case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE:
            return paint_boat_ride_track_left_quarter_turn_1_tile;
        case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE:
            return paint_boat_ride_track_right_quarter_turn_1_tile;
    }

    return NULL;
}
