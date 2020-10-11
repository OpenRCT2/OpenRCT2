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
#include "../../paint/tile_element/Paint.Surface.h"
#include "../../sprites.h"
#include "../../world/Map.h"
#include "../Track.h"
#include "../TrackPaint.h"

enum
{
    SPR_MAZE_BASE_HEDGE = 21938,
    SPR_MAZE_BASE_BRICK = 21951,
    SPR_MAZE_BASE_ICE = 21964,
    SPR_MAZE_BASE_WOOD = 21977,
};

enum
{
    SPR_MAZE_OFFSET_WALL_CENTRE = 0,
    SPR_MAZE_OFFSET_WALL_INNER_NE_SW,
    SPR_MAZE_OFFSET_WALL_INNER_NW_SE,
    SPR_MAZE_OFFSET_WALL_TOP_LEFT,
    SPR_MAZE_OFFSET_WALL_TOP_RIGHT,
    SPR_MAZE_OFFSET_WALL_BOTTOM_RIGHT,
    SPR_MAZE_OFFSET_WALL_BOTTOM_LEFT,
    SPR_MAZE_OFFSET_COLUMN_CENTRE,
    SPR_MAZE_OFFSET_COLUMN_TOP_RIGHT,
    SPR_MAZE_OFFSET_COLUMN_TOP_LEFT,
    SPR_MAZE_OFFSET_COLUMN_BOTTOM_LEFT,
    SPR_MAZE_OFFSET_COLUMN_BOTTOM_RIGHT,
    SPR_MAZE_OFFSET_COLUMN_CORNER,
};

/**
 * rct: 0x004ACF4A
 */
static void maze_paint_setup(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    uint16_t maze_entry = tileElement->AsTrack()->GetMazeEntry();
    maze_entry = rol16(maze_entry, direction * 4);

    uint32_t rotation = session->CurrentRotation;
    // draw ground
    int32_t image_id = SPR_TERRAIN_DIRT | session->TrackColours[SCHEME_MISC];
    sub_98196C(session, image_id, 0, 0, 32, 32, 0, height);

    wooden_a_supports_paint_setup(session, (rotation & 1) ? 0 : 1, 0, height, session->TrackColours[SCHEME_3], nullptr);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL & ~SEGMENT_C4, 0xFFFF, 0);

    int32_t base_image_id = 0;
    switch (get_ride(rideIndex)->track_colour[0].supports)
    {
        case 0:
            base_image_id = SPR_MAZE_BASE_BRICK;
            break;
        case 1:
            base_image_id = SPR_MAZE_BASE_HEDGE;
            break;
        case 2:
            base_image_id = SPR_MAZE_BASE_ICE;
            break;
        case 3:
            base_image_id = SPR_MAZE_BASE_WOOD;
            break;
    }

    base_image_id |= session->TrackColours[SCHEME_MISC];

    image_id = base_image_id + SPR_MAZE_OFFSET_WALL_CENTRE;
    if (maze_entry & MAZE_ENTRY_FLAG_3)
        sub_98197C(session, image_id, 2, 2, 10, 10, 9, height, 3, 3, height + 2);

    if (maze_entry & MAZE_ENTRY_FLAG_7)
        sub_98197C(session, image_id, 2, 18, 10, 10, 9, height, 3, 19, height + 2);

    if (maze_entry & MAZE_ENTRY_FLAG_11)
        sub_98197C(session, image_id, 18, 18, 10, 10, 9, height, 19, 19, height + 2);

    if (maze_entry & MAZE_ENTRY_FLAG_15)
        sub_98197C(session, image_id, 18, 2, 10, 10, 9, height, 19, 3, height + 2);

    image_id = base_image_id + SPR_MAZE_OFFSET_WALL_TOP_LEFT;
    if (maze_entry & MAZE_ENTRY_FLAG_0)
        sub_98197C(session, image_id, 2, 0, 10, 1, 9, height, 3, 1, height + 2);

    if (maze_entry & MAZE_ENTRY_FLAG_13)
        sub_98197C(session, image_id, 18, 0, 10, 1, 9, height, 19, 1, height + 2);

    image_id = base_image_id + SPR_MAZE_OFFSET_WALL_BOTTOM_RIGHT;
    if (maze_entry & MAZE_ENTRY_FLAG_5)
        sub_98197C(session, image_id, 2, 30, 10, 1, 9, height, 3, 30, height + 2);

    if (maze_entry & MAZE_ENTRY_FLAG_8)
        sub_98197C(session, image_id, 18, 30, 10, 1, 9, height, 19, 30, height + 2);

    image_id = base_image_id + SPR_MAZE_OFFSET_WALL_TOP_RIGHT;
    if (maze_entry & MAZE_ENTRY_FLAG_1)
        sub_98197C(session, image_id, 0, 2, 1, 10, 9, height, 1, 3, height + 2);

    if (maze_entry & MAZE_ENTRY_FLAG_4)
        sub_98197C(session, image_id, 0, 18, 1, 10, 9, height, 1, 19, height + 2);

    image_id = base_image_id + SPR_MAZE_OFFSET_WALL_BOTTOM_LEFT;
    if (maze_entry & MAZE_ENTRY_FLAG_12)
        sub_98197C(session, image_id, 30, 2, 1, 10, 9, height, 30, 3, height + 2);

    if (maze_entry & MAZE_ENTRY_FLAG_9)
        sub_98197C(session, image_id, 30, 18, 1, 10, 9, height, 30, 19, height + 2);

    image_id = base_image_id + SPR_MAZE_OFFSET_WALL_INNER_NE_SW;
    if (maze_entry & MAZE_ENTRY_FLAG_2)
        sub_98197C(session, image_id, 2, 14, 10, 4, 9, height, 3, 14, height + 2);

    if (maze_entry & MAZE_ENTRY_FLAG_10)
        sub_98197C(session, image_id, 18, 14, 10, 4, 9, height, 19, 14, height + 2);

    image_id = base_image_id + SPR_MAZE_OFFSET_WALL_INNER_NW_SE;
    if (maze_entry & MAZE_ENTRY_FLAG_14)
        sub_98197C(session, image_id, 14, 2, 4, 10, 9, height, 14, 3, height + 2);

    if (maze_entry & MAZE_ENTRY_FLAG_6)
        sub_98197C(session, image_id, 14, 18, 4, 10, 9, height, 14, 19, height + 2);

    image_id = base_image_id + SPR_MAZE_OFFSET_COLUMN_CORNER;
    if (maze_entry & (MAZE_ENTRY_FLAG_0 | MAZE_ENTRY_FLAG_1))
        sub_98197C(session, image_id, 0, 0, 1, 1, 9, height, 1, 1, height + 2);

    if (maze_entry & (MAZE_ENTRY_FLAG_4 | MAZE_ENTRY_FLAG_5))
        sub_98197C(session, image_id, 0, 30, 1, 1, 9, height, 1, 30, height + 2);

    if (maze_entry & (MAZE_ENTRY_FLAG_8 | MAZE_ENTRY_FLAG_9))
        sub_98197C(session, image_id, 30, 30, 1, 1, 9, height, 30, 30, height + 2);

    if (maze_entry & (MAZE_ENTRY_FLAG_12 | MAZE_ENTRY_FLAG_13))
        sub_98197C(session, image_id, 30, 0, 1, 1, 9, height, 30, 1, height + 2);

    if (maze_entry & (MAZE_ENTRY_FLAG_0 | MAZE_ENTRY_FLAG_13 | MAZE_ENTRY_FLAG_14))
        sub_98197C(session, base_image_id + SPR_MAZE_OFFSET_COLUMN_TOP_LEFT, 14, 0, 2, 1, 9, height, 15, 1, height + 2);

    if (maze_entry & (MAZE_ENTRY_FLAG_5 | MAZE_ENTRY_FLAG_6 | MAZE_ENTRY_FLAG_8))
        sub_98197C(session, base_image_id + SPR_MAZE_OFFSET_COLUMN_BOTTOM_RIGHT, 14, 30, 2, 1, 9, height, 15, 30, height + 2);

    if (maze_entry & (MAZE_ENTRY_FLAG_1 | MAZE_ENTRY_FLAG_2 | MAZE_ENTRY_FLAG_4))
        sub_98197C(session, base_image_id + SPR_MAZE_OFFSET_COLUMN_TOP_RIGHT, 0, 14, 1, 2, 9, height, 1, 15, height + 2);

    if (maze_entry & (MAZE_ENTRY_FLAG_9 | MAZE_ENTRY_FLAG_10 | MAZE_ENTRY_FLAG_12))
        sub_98197C(session, base_image_id + SPR_MAZE_OFFSET_COLUMN_BOTTOM_LEFT, 30, 14, 1, 2, 9, height, 30, 15, height + 2);

    if (maze_entry & (MAZE_ENTRY_FLAG_2 | MAZE_ENTRY_FLAG_6 | MAZE_ENTRY_FLAG_10 | MAZE_ENTRY_FLAG_14))
    {
        sub_98197C(session, base_image_id + SPR_MAZE_OFFSET_COLUMN_CENTRE, 14, 14, 2, 2, 8, height, 15, 15, height + 2);

        paint_util_set_segment_support_height(session, SEGMENT_C4, height + 12, 0x20);
    }

    paint_util_set_general_support_height(session, height + 32, 0x20);
}

/**
 * rct2: 0x008A81E8
 */
TRACK_PAINT_FUNCTION get_track_paint_function_maze(int32_t trackType)
{
    if (trackType != TrackElemType::Maze)
    {
        return nullptr;
    }

    return maze_paint_setup;
}
