/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Maze.h"

#include "../../core/Numerics.hpp"
#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../../paint/tile_element/Paint.Surface.h"
#include "../../sprites.h"
#include "../../world/Map.h"
#include "../Ride.h"
#include "../RideData.h"
#include "../Track.h"
#include "../TrackData.h"
#include "../TrackPaint.h"

using namespace OpenRCT2::TrackMetaData;

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
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    uint16_t maze_entry = trackElement.GetMazeEntry();
    maze_entry = Numerics::rol16(maze_entry, direction * 4);

    uint32_t rotation = session.CurrentRotation;
    // draw ground
    auto image_id = session.TrackColours[SCHEME_MISC].WithIndex(SPR_TERRAIN_DIRT);
    PaintAddImageAsParent(session, image_id, { 0, 0, height }, { 32, 32, 0 });

    wooden_a_supports_paint_setup(session, (rotation & 1) ? 0 : 1, 0, height, session.TrackColours[SCHEME_3]);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL & ~SEGMENT_C4, 0xFFFF, 0);

    int32_t base_image_id = 0;
    switch (ride.track_colour[0].supports)
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

    auto baseImage = session.TrackColours[SCHEME_MISC].WithIndex(base_image_id);

    image_id = baseImage.WithIndexOffset(SPR_MAZE_OFFSET_WALL_CENTRE);
    if (maze_entry & MAZE_ENTRY_FLAG_3)
        PaintAddImageAsParent(session, image_id, { 2, 2, height }, { 10, 10, 9 }, { 3, 3, height + 2 });

    if (maze_entry & MAZE_ENTRY_FLAG_7)
        PaintAddImageAsParent(session, image_id, { 2, 18, height }, { 10, 10, 9 }, { 3, 19, height + 2 });

    if (maze_entry & MAZE_ENTRY_FLAG_11)
        PaintAddImageAsParent(session, image_id, { 18, 18, height }, { 10, 10, 9 }, { 19, 19, height + 2 });

    if (maze_entry & MAZE_ENTRY_FLAG_15)
        PaintAddImageAsParent(session, image_id, { 18, 2, height }, { 10, 10, 9 }, { 19, 3, height + 2 });

    image_id = baseImage.WithIndexOffset(SPR_MAZE_OFFSET_WALL_TOP_LEFT);
    if (maze_entry & MAZE_ENTRY_FLAG_0)
        PaintAddImageAsParent(session, image_id, { 2, 0, height }, { 10, 1, 9 }, { 3, 1, height + 2 });

    if (maze_entry & MAZE_ENTRY_FLAG_13)
        PaintAddImageAsParent(session, image_id, { 18, 0, height }, { 10, 1, 9 }, { 19, 1, height + 2 });

    image_id = baseImage.WithIndexOffset(SPR_MAZE_OFFSET_WALL_BOTTOM_RIGHT);
    if (maze_entry & MAZE_ENTRY_FLAG_5)
        PaintAddImageAsParent(session, image_id, { 2, 30, height }, { 10, 1, 9 }, { 3, 30, height + 2 });

    if (maze_entry & MAZE_ENTRY_FLAG_8)
        PaintAddImageAsParent(session, image_id, { 18, 30, height }, { 10, 1, 9 }, { 19, 30, height + 2 });

    image_id = baseImage.WithIndexOffset(SPR_MAZE_OFFSET_WALL_TOP_RIGHT);
    if (maze_entry & MAZE_ENTRY_FLAG_1)
        PaintAddImageAsParent(session, image_id, { 0, 2, height }, { 1, 10, 9 }, { 1, 3, height + 2 });

    if (maze_entry & MAZE_ENTRY_FLAG_4)
        PaintAddImageAsParent(session, image_id, { 0, 18, height }, { 1, 10, 9 }, { 1, 19, height + 2 });

    image_id = baseImage.WithIndexOffset(SPR_MAZE_OFFSET_WALL_BOTTOM_LEFT);
    if (maze_entry & MAZE_ENTRY_FLAG_12)
        PaintAddImageAsParent(session, image_id, { 30, 2, height }, { 1, 10, 9 }, { 30, 3, height + 2 });

    if (maze_entry & MAZE_ENTRY_FLAG_9)
        PaintAddImageAsParent(session, image_id, { 30, 18, height }, { 1, 10, 9 }, { 30, 19, height + 2 });

    image_id = baseImage.WithIndexOffset(SPR_MAZE_OFFSET_WALL_INNER_NE_SW);
    if (maze_entry & MAZE_ENTRY_FLAG_2)
        PaintAddImageAsParent(session, image_id, { 2, 14, height }, { 10, 4, 9 }, { 3, 14, height + 2 });

    if (maze_entry & MAZE_ENTRY_FLAG_10)
        PaintAddImageAsParent(session, image_id, { 18, 14, height }, { 10, 4, 9 }, { 19, 14, height + 2 });

    image_id = baseImage.WithIndexOffset(SPR_MAZE_OFFSET_WALL_INNER_NW_SE);
    if (maze_entry & MAZE_ENTRY_FLAG_14)
        PaintAddImageAsParent(session, image_id, { 14, 2, height }, { 4, 10, 9 }, { 14, 3, height + 2 });

    if (maze_entry & MAZE_ENTRY_FLAG_6)
        PaintAddImageAsParent(session, image_id, { 14, 18, height }, { 4, 10, 9 }, { 14, 19, height + 2 });

    image_id = baseImage.WithIndexOffset(SPR_MAZE_OFFSET_COLUMN_CORNER);
    if (maze_entry & (MAZE_ENTRY_FLAG_0 | MAZE_ENTRY_FLAG_1))
        PaintAddImageAsParent(session, image_id, { 0, 0, height }, { 1, 1, 9 }, { 1, 1, height + 2 });

    if (maze_entry & (MAZE_ENTRY_FLAG_4 | MAZE_ENTRY_FLAG_5))
        PaintAddImageAsParent(session, image_id, { 0, 30, height }, { 1, 1, 9 }, { 1, 30, height + 2 });

    if (maze_entry & (MAZE_ENTRY_FLAG_8 | MAZE_ENTRY_FLAG_9))
        PaintAddImageAsParent(session, image_id, { 30, 30, height }, { 1, 1, 9 }, { 30, 30, height + 2 });

    if (maze_entry & (MAZE_ENTRY_FLAG_12 | MAZE_ENTRY_FLAG_13))
        PaintAddImageAsParent(session, image_id, { 30, 0, height }, { 1, 1, 9 }, { 30, 1, height + 2 });

    if (maze_entry & (MAZE_ENTRY_FLAG_0 | MAZE_ENTRY_FLAG_13 | MAZE_ENTRY_FLAG_14))
        PaintAddImageAsParent(
            session, base_image_id + SPR_MAZE_OFFSET_COLUMN_TOP_LEFT, { 14, 0, height }, { 2, 1, 9 }, { 15, 1, height + 2 });

    if (maze_entry & (MAZE_ENTRY_FLAG_5 | MAZE_ENTRY_FLAG_6 | MAZE_ENTRY_FLAG_8))
        PaintAddImageAsParent(
            session, base_image_id + SPR_MAZE_OFFSET_COLUMN_BOTTOM_RIGHT, { 14, 30, height }, { 2, 1, 9 },
            { 15, 30, height + 2 });

    if (maze_entry & (MAZE_ENTRY_FLAG_1 | MAZE_ENTRY_FLAG_2 | MAZE_ENTRY_FLAG_4))
        PaintAddImageAsParent(
            session, base_image_id + SPR_MAZE_OFFSET_COLUMN_TOP_RIGHT, { 0, 14, height }, { 1, 2, 9 }, { 1, 15, height + 2 });

    if (maze_entry & (MAZE_ENTRY_FLAG_9 | MAZE_ENTRY_FLAG_10 | MAZE_ENTRY_FLAG_12))
        PaintAddImageAsParent(
            session, base_image_id + SPR_MAZE_OFFSET_COLUMN_BOTTOM_LEFT, { 30, 14, height }, { 1, 2, 9 },
            { 30, 15, height + 2 });

    if (maze_entry & (MAZE_ENTRY_FLAG_2 | MAZE_ENTRY_FLAG_6 | MAZE_ENTRY_FLAG_10 | MAZE_ENTRY_FLAG_14))
    {
        PaintAddImageAsParent(
            session, base_image_id + SPR_MAZE_OFFSET_COLUMN_CENTRE, { 14, 14, height }, { 2, 2, 8 }, { 15, 15, height + 2 });

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

money64 MazeCalculateCost(money32 constructionCost, const Ride& ride, const CoordsXYZ& _loc)
{
    const auto& ted = GetTrackElementDescriptor(TrackElemType::Maze);
    money64 price = (ride.GetRideTypeDescriptor().BuildCosts.TrackPrice * ted.PriceModifier) >> 16;

    auto surfaceElement = map_get_surface_element_at(_loc);
    auto heightDifference = (_loc.z - surfaceElement->GetBaseZ()) / COORDS_Z_PER_TINY_Z;
    money64 supportCost = heightDifference * ride.GetRideTypeDescriptor().BuildCosts.SupportPrice;

    return constructionCost + price + supportCost;
}
