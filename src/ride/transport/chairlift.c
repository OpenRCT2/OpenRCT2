#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#include "../../world/map.h"
#include "../track_paint.h"
#include "../track.h"
#include "../../paint/paint.h"
#include "../../interface/viewport.h"
#include "../../paint/supports.h"

enum
{
    SPR_CHAIRLIFT_CABLE_FLAT_SW_NE = 20500,
    SPR_CHAIRLIFT_CABLE_FLAT_SE_NW,
    SPR_20502,
    SPR_20503,
    SPR_20504,
    SPR_20505,
    SPR_20506,
    SPR_20507,
    SPR_20508,
    SPR_20509,
    SPR_20510,
    SPR_20511,
    SPR_20512,
    SPR_20513,
    SPR_20514,
    SPR_20515,
    SPR_CHAIRLIFT_CABLE_UP_SW_NE,
    SPR_CHAIRLIFT_CABLE_UP_NW_SE,
    SPR_CHAIRLIFT_CABLE_UP_NE_SW,
    SPR_CHAIRLIFT_CABLE_UP_SE_NW,
    SPR_20520,
    SPR_20521,
    SPR_20522,
    SPR_20523,
    SPR_20524,
    SPR_20525,
    SPR_20526,
    SPR_20527,
    SPR_CHAIRLIFT_CORNER_NW_SW,
    SPR_CHAIRLIFT_CORNER_NW_NE,
    SPR_CHAIRLIFT_CORNER_SE_NE,
    SPR_CHAIRLIFT_CORNER_SW_SE,
    SPR_20532,
    SPR_20533,
    SPR_20534,
    SPR_20535,
    SPR_20536,
    SPR_20537,
    SPR_20538,
    SPR_20539,
    SPR_20540,
    SPR_20541,
    SPR_20542,
    SPR_20543,
    SPR_20544,
    SPR_20545,
    SPR_20546,
    SPR_20547,
} SPR_CHAIRLIFT;

enum
{
    SEGMENT_B4 = (1 << 0),
    SEGMENT_0 = SEGMENT_B4,
    SEGMENT_B8 = (1 << 1),
    SEGMENT_1 = SEGMENT_B8,
    SEGMENT_BC = (1 << 2),
    SEGMENT_2 = SEGMENT_BC,
    SEGMENT_C0 = (1 << 3),
    SEGMENT_3 = SEGMENT_C0,
    SEGMENT_C4 = (1 << 4),
    SEGMENT_4 = SEGMENT_C4,
    SEGMENT_C8 = (1 << 5),
    SEGMENT_5 = SEGMENT_C8,
    SEGMENT_CC = (1 << 6),
    SEGMENT_6 = SEGMENT_CC,
    SEGMENT_D0 = (1 << 7),
    SEGMENT_7 = SEGMENT_D0,
    SEGMENT_D4 = (1 << 8),
    SEGMENT_8 = SEGMENT_D4,
};

const int SEGMENTS_ALL = SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4;

static void paint_util_set_segment_support_height(int flags, uint16 height, uint8 segment_flags)
{
    for (int s = 0; s < 9; s++) {
        if (flags & (1 << s)) {
            RCT2_GLOBAL(0x0141E9B4 + s * 4, uint16) = height;
            RCT2_GLOBAL(0x0141E9B6 + s * 4, uint8) = segment_flags;
        }
    }
}

static void paint_util_set_support_height(uint16 height, uint8 flags)
{
    if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) < height) {
        RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) = height;
        RCT2_GLOBAL(0x141E9DA, uint8) = flags;
    }
}

enum
{
    TUNNEL_0,
    TUNNEL_FLAT = 6,
    TUNNEL_7 = 7,
    TUNNEL_UP = 8,
    TUNNEL_14 = 0x0E
};

static void paint_util_push_tunnel_left(uint16 height, uint8 type)
{
    uint32 eax = 0xFFFF0000 | ((height / 16) & 0xFF) | type << 8;
    RCT2_ADDRESS(0x009E3138, uint32)[RCT2_GLOBAL(0x141F56A, uint8) / 2] = eax;
    RCT2_GLOBAL(0x141F56A, uint8)++;
}

static void paint_util_push_tunnel_right(uint16 height, uint8 type)
{
    uint32 eax = 0xFFFF0000 | ((height / 16) & 0xFF) | type << 8;
    RCT2_ADDRESS(0x009E30B6, uint32)[RCT2_GLOBAL(0x141F56B, uint8) / 2] = eax;
    RCT2_GLOBAL(0x141F56B, uint8)++;
}

static void chairlift_paint_util_draw_supports(int supports, uint16 height)
{
    bool success = false;

    for (int s = 0; s < 9; s++) {
        if (!(supports & (1 << s))) {
            continue;
        }

        if (metal_a_supports_paint_setup(10, s, 0, height, RCT2_GLOBAL(0x00F4419C, uint32))) {
            success = true;
        }
    }

    if (success) {
        return;
    }

    for (int s = 0; s < 9; s++) {
        if (!(supports & (1 << s))) {
            continue;
        }
        uint16 temp = RCT2_GLOBAL(0x0141E9B4 + s * 4, uint16);
        RCT2_GLOBAL(0x0141E9B4 + s * 4, uint16) = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, uint16);
        metal_a_supports_paint_setup(10, s, 0, height, RCT2_GLOBAL(0x00F4419C, uint32));
        RCT2_GLOBAL(0x0141E9B4 + s * 4, uint16) = temp;
    }
}

static void chairlift_paint_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    uint32 imageId;
    if (direction & 1) {
        imageId = SPR_CHAIRLIFT_CABLE_FLAT_SE_NW | RCT2_GLOBAL(0x00F44198, uint32);
        sub_98197C(imageId, 0, 0, 6, 32, 2, height, 13, 0, height + 28, get_current_rotation());
        paint_util_push_tunnel_right(height, TUNNEL_FLAT);
    } else {
        imageId = SPR_CHAIRLIFT_CABLE_FLAT_SW_NE | RCT2_GLOBAL(0x00F44198, uint32);
        sub_98197C(imageId, 0, 0, 32, 6, 2, height, 0, 13, height + 28, get_current_rotation());
        paint_util_push_tunnel_left(height, TUNNEL_FLAT);
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_support_height(height + 32, 0x20);
}

/** rct2: 0x00743FD8 */
static void chairlift_paint_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    uint32 imageId;

    switch (direction) {
        case 0:
            imageId = SPR_CHAIRLIFT_CABLE_UP_SW_NE | RCT2_GLOBAL(0x00F44198, uint32);
            sub_98197C(imageId, 0, 0, 32, 6, 2, height, 0, 13, height + 28, get_current_rotation());
            paint_util_push_tunnel_left(height - 8, TUNNEL_7);
            break;

        case 1:
            imageId = SPR_CHAIRLIFT_CABLE_UP_NW_SE | RCT2_GLOBAL(0x00F44198, uint32);
            sub_98197C(imageId, 0, 0, 6, 32, 2, height, 13, 0, height + 28, get_current_rotation());
            paint_util_push_tunnel_right(height + 8, TUNNEL_UP);
            break;

        case 2:
            imageId = SPR_CHAIRLIFT_CABLE_UP_NE_SW | RCT2_GLOBAL(0x00F44198, uint32);
            sub_98197C(imageId, 0, 0, 32, 6, 2, height, 0, 13, height + 28, get_current_rotation());
            paint_util_push_tunnel_left(height + 8, TUNNEL_UP);
            break;

        case 3:
            imageId = SPR_CHAIRLIFT_CABLE_UP_SE_NW | RCT2_GLOBAL(0x00F44198, uint32);
            sub_98197C(imageId, 0, 0, 6, 32, 2, height, 13, 0, height + 28, get_current_rotation());
            paint_util_push_tunnel_right(height - 8, TUNNEL_7);
            break;
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_support_height(height + 56, 0x20);
}

/** rct2: 0x00743FD8 */
static void chairlift_paint_flat_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    uint32 imageId;

    switch (direction) {
        case 0:
            imageId = SPR_20508 | RCT2_GLOBAL(0x00F44198, uint32);
            sub_98197C(imageId, 0, 0, 32, 6, 2, height, 0, 13, height + 28, get_current_rotation());

            imageId = SPR_20520 | RCT2_GLOBAL(0x00F4419C, uint32);
            sub_98197C(imageId, 0, 0, 4, 4, 25, height, 14, 14, height + 1, get_current_rotation());

            paint_util_push_tunnel_left(height, TUNNEL_FLAT);
            break;

        case 1:
            imageId = SPR_20509 | RCT2_GLOBAL(0x00F44198, uint32);
            sub_98197C(imageId, 0, 0, 6, 32, 2, height, 13, 0, height + 28, get_current_rotation());

            imageId = SPR_20521 | RCT2_GLOBAL(0x00F4419C, uint32);
            sub_98197C(imageId, 0, 0, 4, 4, 25, height, 14, 14, height + 1, get_current_rotation());

            paint_util_push_tunnel_right(height, TUNNEL_UP);
            break;

        case 2:
            imageId = SPR_20510 | RCT2_GLOBAL(0x00F44198, uint32);
            sub_98197C(imageId, 0, 0, 32, 6, 2, height, 0, 13, height + 28, get_current_rotation());

            imageId = SPR_20522 | RCT2_GLOBAL(0x00F4419C, uint32);
            sub_98197C(imageId, 0, 0, 4, 4, 25, height, 14, 14, height + 1, get_current_rotation());

            paint_util_push_tunnel_left(height, TUNNEL_UP);
            break;

        case 3:
            imageId = SPR_20511 | RCT2_GLOBAL(0x00F44198, uint32);
            sub_98197C(imageId, 0, 0, 6, 32, 2, height, 13, 0, height + 28, get_current_rotation());

            imageId = SPR_20523 | RCT2_GLOBAL(0x00F4419C, uint32);
            sub_98197C(imageId, 0, 0, 4, 4, 25, height, 14, 14, height + 1, get_current_rotation());

            paint_util_push_tunnel_right(height, TUNNEL_FLAT);
            break;
    }

    chairlift_paint_util_draw_supports(SEGMENT_4, height);
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_support_height(height + 48, 0x20);
}

/** rct2: 0x00743FF8 */
static void chairlift_paint_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    uint32 imageId;

    switch (direction) {
        case 0:
            imageId = SPR_20512 | RCT2_GLOBAL(0x00F44198, uint32);
            sub_98197C(imageId, 0, 0, 32, 6, 2, height, 0, 13, height + 28, get_current_rotation());

            imageId = SPR_20524 | RCT2_GLOBAL(0x00F4419C, uint32);
            sub_98197C(imageId, 0, 0, 4, 4, 25, height, 14, 14, height + 1, get_current_rotation());

            paint_util_push_tunnel_left(height - 8, TUNNEL_FLAT);
            break;

        case 1:
            imageId = SPR_20513 | RCT2_GLOBAL(0x00F44198, uint32);
            sub_98197C(imageId, 0, 0, 6, 32, 2, height, 13, 0, height + 28, get_current_rotation());

            imageId = SPR_20525 | RCT2_GLOBAL(0x00F4419C, uint32);
            sub_98197C(imageId, 0, 0, 4, 4, 25, height, 14, 14, height + 1, get_current_rotation());

            paint_util_push_tunnel_right(height + 8, TUNNEL_14);
            break;

        case 2:
            imageId = SPR_20514 | RCT2_GLOBAL(0x00F44198, uint32);
            sub_98197C(imageId, 0, 0, 32, 6, 2, height, 0, 13, height + 28, get_current_rotation());

            imageId = SPR_20526 | RCT2_GLOBAL(0x00F4419C, uint32);
            sub_98197C(imageId, 0, 0, 4, 4, 25, height, 14, 14, height + 1, get_current_rotation());

            paint_util_push_tunnel_left(height + 8, TUNNEL_14);
            break;

        case 3:
            imageId = SPR_20515 | RCT2_GLOBAL(0x00F44198, uint32);
            sub_98197C(imageId, 0, 0, 6, 32, 2, height, 13, 0, height + 28, get_current_rotation());

            imageId = SPR_20527 | RCT2_GLOBAL(0x00F4419C, uint32);
            sub_98197C(imageId, 0, 0, 4, 4, 25, height, 14, 14, height + 1, get_current_rotation());

            paint_util_push_tunnel_right(height - 8, TUNNEL_FLAT);
            break;

    }

    chairlift_paint_util_draw_supports(SEGMENT_4, height);
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_support_height(height + 40, 0x20);
}

/** rct2: 0x00744008 */
static void chairlift_paint_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    chairlift_paint_25_deg_up(rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x00744018 */
static void chairlift_paint_flat_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    chairlift_paint_25_deg_up_to_flat(rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x00744028 */
static void chairlift_paint_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    chairlift_paint_flat_to_25_deg_up(rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x00744038 */
static void chairlift_paint_left_quarter_turn_1_tile(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    uint32 imageId;

    switch (direction) {
        case 0:
            imageId = SPR_CHAIRLIFT_CORNER_NW_SW | RCT2_GLOBAL(0x00F44198, uint32);
            sub_98197C(imageId, 0, 0, 16, 16, 2, height, 16, 0, height + 28, get_current_rotation());

            imageId = SPR_20532 | RCT2_GLOBAL(0x00F4419C, uint32);
            sub_98197C(imageId, 0, 0, 2, 2, 27, height, 16, 4, height, get_current_rotation());

            imageId = SPR_20536 | RCT2_GLOBAL(0x00F4419C, uint32);
            sub_98197C(imageId, 0, 0, 2, 2, 27, height, 28, 4, height, get_current_rotation());

            paint_util_push_tunnel_left(height, TUNNEL_FLAT);
            chairlift_paint_util_draw_supports(SEGMENT_5 | SEGMENT_7, height);
            break;

        case 1:
            imageId = SPR_CHAIRLIFT_CORNER_NW_NE | RCT2_GLOBAL(0x00F44198, uint32);
            sub_98197C(imageId, 0, 0, 16, 16, 2, height, 0, 0, height + 28, get_current_rotation());

            imageId = SPR_20533 | RCT2_GLOBAL(0x00F4419C, uint32);
            sub_98197C(imageId, 0, 0, 2, 2, 27, height, 16, 4, height, get_current_rotation());

            imageId = SPR_20537 | RCT2_GLOBAL(0x00F4419C, uint32);
            sub_98197C(imageId, 0, 0, 2, 2, 27, height, 4, 16, height, get_current_rotation());

            chairlift_paint_util_draw_supports(SEGMENT_5 | SEGMENT_6, height);
            break;

        case 2:
            imageId = SPR_CHAIRLIFT_CORNER_SE_NE | RCT2_GLOBAL(0x00F44198, uint32);
            sub_98197C(imageId, 0, 0, 16, 16, 2, height, 0, 16, height + 28, get_current_rotation());

            imageId = SPR_20534 | RCT2_GLOBAL(0x00F4419C, uint32);
            sub_98197C(imageId, 0, 0, 2, 2, 27, height, 4, 16, height, get_current_rotation());

            imageId = SPR_20538 | RCT2_GLOBAL(0x00F4419C, uint32);
            sub_98197C(imageId, 0, 0, 2, 2, 27, height, 16, 28, height, get_current_rotation());

            paint_util_push_tunnel_right(height, TUNNEL_FLAT);
            chairlift_paint_util_draw_supports(SEGMENT_6 | SEGMENT_8, height);
            break;

        case 3:
            imageId = SPR_CHAIRLIFT_CORNER_SW_SE | RCT2_GLOBAL(0x00F44198, uint32);
            sub_98197C(imageId, 0, 0, 16, 16, 2, height, 16, 16, height + 28, get_current_rotation());

            imageId = SPR_20535 | RCT2_GLOBAL(0x00F4419C, uint32);
            sub_98197C(imageId, 0, 0, 2, 2, 27, height, 28, 16, height, get_current_rotation());

            imageId = SPR_20539 | RCT2_GLOBAL(0x00F4419C, uint32);
            sub_98197C(imageId, 0, 0, 2, 2, 27, height, 16, 28, height, get_current_rotation());

            paint_util_push_tunnel_left(height, TUNNEL_FLAT);
            paint_util_push_tunnel_right(height, TUNNEL_FLAT);
            chairlift_paint_util_draw_supports(SEGMENT_7 | SEGMENT_8, height);
            break;
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_support_height(height + 32, 0x20);
}

/** rct2: 0x00744048 */
static void chairlift_paint_right_quarter_turn_1_tile(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    chairlift_paint_left_quarter_turn_1_tile(rideIndex, trackSequence, (direction + 3) % 4, height, mapElement);
}

/* 0x008AAA0C */
TRACK_PAINT_FUNCTION get_track_paint_function_chairlift(int trackType, int direction)
{
    switch (trackType) {
        case TRACK_ELEM_BEGIN_STATION:
        case TRACK_ELEM_MIDDLE_STATION:
        case TRACK_ELEM_END_STATION:
            return NULL;

        case TRACK_ELEM_FLAT:
            return chairlift_paint_flat;

        case TRACK_ELEM_FLAT_TO_25_DEG_UP:
            return chairlift_paint_flat_to_25_deg_up;
        case TRACK_ELEM_25_DEG_UP:
            return chairlift_paint_25_deg_up;
        case TRACK_ELEM_25_DEG_UP_TO_FLAT:
            return chairlift_paint_25_deg_up_to_flat;

        case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
            return chairlift_paint_flat_to_25_deg_down;
        case TRACK_ELEM_25_DEG_DOWN:
            return chairlift_paint_25_deg_down;
        case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
            return chairlift_paint_25_deg_down_to_flat;

        case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE:
            return chairlift_paint_left_quarter_turn_1_tile;
        case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE:
            return chairlift_paint_right_quarter_turn_1_tile;
    }

    return NULL;
}
