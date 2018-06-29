/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _PAINT_TILE_ELEMENT_H
#define _PAINT_TILE_ELEMENT_H

#include "../../common.h"
#include "../../world/Map.h"

struct paint_session;

enum edge_t
{
    EDGE_NE = (1 << 0),
    EDGE_SE = (1 << 1),
    EDGE_SW = (1 << 2),
    EDGE_NW = (1 << 3),
    EDGE_BOTTOMLEFT = EDGE_SW,
    EDGE_BOTTOMRIGHT = EDGE_SE,
    EDGE_TOPLEFT = EDGE_NW,
    EDGE_TOPRIGHT = EDGE_NE
};

enum
{
    SEGMENT_B4 = (1 << 0), // 0
    SEGMENT_CC = (1 << 1), // 6
    SEGMENT_BC = (1 << 2), // 2
    SEGMENT_D4 = (1 << 3), // 8
    SEGMENT_C0 = (1 << 4), // 3
    SEGMENT_D0 = (1 << 5), // 7
    SEGMENT_B8 = (1 << 6), // 1
    SEGMENT_C8 = (1 << 7), // 5
    SEGMENT_C4 = (1 << 8), // 4
};

enum
{
    TUNNEL_0 = 0,
    TUNNEL_1 = 1,
    TUNNEL_2 = 2,
    TUNNEL_3 = 3,
    TUNNEL_4 = 4,
    TUNNEL_5 = 5,
    TUNNEL_6 = 6,
    TUNNEL_7 = 7,
    TUNNEL_8 = 8,
    TUNNEL_9 = 9,
    TUNNEL_10 = 0x0A,
    TUNNEL_11 = 0x0B,
    TUNNEL_12 = 0x0C,
    TUNNEL_13 = 0x0D,
    TUNNEL_14 = 0x0E,
    TUNNEL_15 = 0x0F,
    // Ghost train doors
    TUNNEL_DOORS_0 = 16,
    TUNNEL_DOORS_1 = 17,
    TUNNEL_DOORS_2 = 18,
    TUNNEL_DOORS_3 = 19,
    TUNNEL_DOORS_4 = 20,
    TUNNEL_DOORS_5 = 21,
    TUNNEL_DOORS_6 = 22,

    TUNNEL_TYPE_COUNT
};

enum
{
    G141E9DB_FLAG_1 = 1,
    G141E9DB_FLAG_2 = 2,
};

#ifdef __TESTPAINT__
extern uint16_t testPaintVerticalTunnelHeight;
#endif

extern const int32_t SEGMENTS_ALL;
extern const uint16_t segment_offsets[9];

extern bool gShowSupportSegmentHeights;

extern const LocationXY16 BannerBoundBoxes[][2];

extern const uint8_t byte_98D800[4];

void paint_util_push_tunnel_left(paint_session * session, uint16_t height, uint8_t type);
void paint_util_push_tunnel_right(paint_session * session, uint16_t height, uint8_t type);
void paint_util_set_vertical_tunnel(paint_session * session, uint16_t height);

void paint_util_set_general_support_height(paint_session * session, int16_t height, uint8_t slope);
void paint_util_force_set_general_support_height(paint_session * session, int16_t height, uint8_t slope);
void paint_util_set_segment_support_height(paint_session * session, int32_t segments, uint16_t height, uint8_t slope);
uint16_t paint_util_rotate_segments(uint16_t segments, uint8_t rotation);

void tile_element_paint_setup(paint_session * session, int32_t x, int32_t y);

void entrance_paint(paint_session * session, uint8_t direction, int32_t height, const rct_tile_element * tile_element);
void banner_paint(paint_session * session, uint8_t direction, int32_t height, const rct_tile_element * tile_element);
void surface_paint(paint_session * session, uint8_t direction, uint16_t height, const rct_tile_element * tileElement);
void path_paint(paint_session * session, uint16_t height, const rct_tile_element * tileElement);
void scenery_paint(paint_session * session, uint8_t direction, int32_t height, const rct_tile_element * tileElement);
void fence_paint(paint_session * session, uint8_t direction, int32_t height, const rct_tile_element * tileElement);
void large_scenery_paint(paint_session * session, uint8_t direction, uint16_t height, const rct_tile_element * tileElement);
void track_paint(paint_session * session, uint8_t direction, int32_t height, const rct_tile_element * tileElement);

#endif
