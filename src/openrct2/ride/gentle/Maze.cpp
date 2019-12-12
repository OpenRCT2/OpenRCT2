/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Maze.h"

#include "../../Diagnostic.h"
#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../../paint/tile_element/Paint.Surface.h"
#include "../../sprites.h"
#include "../../util/Util.h"
#include "../../world/Map.h"
#include "../Track.h"
#include "../TrackPaint.h"
#include "MazePathfinding.h"

#include <algorithm>
#include <set>

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

/** rct2: 0x00981FE4 */
constexpr const uint8_t gMazeGetNewDirectionFromEdge[][4] = {
    { 15, 7, 15, 7 },
    { 11, 3, 11, 3 },
    { 7, 15, 7, 15 },
    { 3, 11, 3, 11 },
};

/** rct2: 0x00981FF4 */
constexpr const uint8_t gMazeCurrentDirectionToHedge[][4] = {
    { 1, 2, 14, 0 },
    { 4, 5, 6, 2 },
    { 6, 8, 9, 10 },
    { 14, 10, 12, 13 },
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
    PaintAddImageAsParent(session, image_id, 0, 0, 32, 32, 0, height);

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
        PaintAddImageAsParent(session, image_id, 2, 2, 10, 10, 9, height, 3, 3, height + 2);

    if (maze_entry & MAZE_ENTRY_FLAG_7)
        PaintAddImageAsParent(session, image_id, 2, 18, 10, 10, 9, height, 3, 19, height + 2);

    if (maze_entry & MAZE_ENTRY_FLAG_11)
        PaintAddImageAsParent(session, image_id, 18, 18, 10, 10, 9, height, 19, 19, height + 2);

    if (maze_entry & MAZE_ENTRY_FLAG_15)
        PaintAddImageAsParent(session, image_id, 18, 2, 10, 10, 9, height, 19, 3, height + 2);

    image_id = base_image_id + SPR_MAZE_OFFSET_WALL_TOP_LEFT;
    if (maze_entry & MAZE_ENTRY_FLAG_0)
        PaintAddImageAsParent(session, image_id, 2, 0, 10, 1, 9, height, 3, 1, height + 2);

    if (maze_entry & MAZE_ENTRY_FLAG_13)
        PaintAddImageAsParent(session, image_id, 18, 0, 10, 1, 9, height, 19, 1, height + 2);

    image_id = base_image_id + SPR_MAZE_OFFSET_WALL_BOTTOM_RIGHT;
    if (maze_entry & MAZE_ENTRY_FLAG_5)
        PaintAddImageAsParent(session, image_id, 2, 30, 10, 1, 9, height, 3, 30, height + 2);

    if (maze_entry & MAZE_ENTRY_FLAG_8)
        PaintAddImageAsParent(session, image_id, 18, 30, 10, 1, 9, height, 19, 30, height + 2);

    image_id = base_image_id + SPR_MAZE_OFFSET_WALL_TOP_RIGHT;
    if (maze_entry & MAZE_ENTRY_FLAG_1)
        PaintAddImageAsParent(session, image_id, 0, 2, 1, 10, 9, height, 1, 3, height + 2);

    if (maze_entry & MAZE_ENTRY_FLAG_4)
        PaintAddImageAsParent(session, image_id, 0, 18, 1, 10, 9, height, 1, 19, height + 2);

    image_id = base_image_id + SPR_MAZE_OFFSET_WALL_BOTTOM_LEFT;
    if (maze_entry & MAZE_ENTRY_FLAG_12)
        PaintAddImageAsParent(session, image_id, 30, 2, 1, 10, 9, height, 30, 3, height + 2);

    if (maze_entry & MAZE_ENTRY_FLAG_9)
        PaintAddImageAsParent(session, image_id, 30, 18, 1, 10, 9, height, 30, 19, height + 2);

    image_id = base_image_id + SPR_MAZE_OFFSET_WALL_INNER_NE_SW;
    if (maze_entry & MAZE_ENTRY_FLAG_2)
        PaintAddImageAsParent(session, image_id, 2, 14, 10, 4, 9, height, 3, 14, height + 2);

    if (maze_entry & MAZE_ENTRY_FLAG_10)
        PaintAddImageAsParent(session, image_id, 18, 14, 10, 4, 9, height, 19, 14, height + 2);

    image_id = base_image_id + SPR_MAZE_OFFSET_WALL_INNER_NW_SE;
    if (maze_entry & MAZE_ENTRY_FLAG_14)
        PaintAddImageAsParent(session, image_id, 14, 2, 4, 10, 9, height, 14, 3, height + 2);

    if (maze_entry & MAZE_ENTRY_FLAG_6)
        PaintAddImageAsParent(session, image_id, 14, 18, 4, 10, 9, height, 14, 19, height + 2);

    image_id = base_image_id + SPR_MAZE_OFFSET_COLUMN_CORNER;
    if (maze_entry & (MAZE_ENTRY_FLAG_0 | MAZE_ENTRY_FLAG_1))
        PaintAddImageAsParent(session, image_id, 0, 0, 1, 1, 9, height, 1, 1, height + 2);

    if (maze_entry & (MAZE_ENTRY_FLAG_4 | MAZE_ENTRY_FLAG_5))
        PaintAddImageAsParent(session, image_id, 0, 30, 1, 1, 9, height, 1, 30, height + 2);

    if (maze_entry & (MAZE_ENTRY_FLAG_8 | MAZE_ENTRY_FLAG_9))
        PaintAddImageAsParent(session, image_id, 30, 30, 1, 1, 9, height, 30, 30, height + 2);

    if (maze_entry & (MAZE_ENTRY_FLAG_12 | MAZE_ENTRY_FLAG_13))
        PaintAddImageAsParent(session, image_id, 30, 0, 1, 1, 9, height, 30, 1, height + 2);

    if (maze_entry & (MAZE_ENTRY_FLAG_0 | MAZE_ENTRY_FLAG_13 | MAZE_ENTRY_FLAG_14))
        PaintAddImageAsParent(
            session, base_image_id + SPR_MAZE_OFFSET_COLUMN_TOP_LEFT, 14, 0, 2, 1, 9, height, 15, 1, height + 2);

    if (maze_entry & (MAZE_ENTRY_FLAG_5 | MAZE_ENTRY_FLAG_6 | MAZE_ENTRY_FLAG_8))
        PaintAddImageAsParent(
            session, base_image_id + SPR_MAZE_OFFSET_COLUMN_BOTTOM_RIGHT, 14, 30, 2, 1, 9, height, 15, 30, height + 2);

    if (maze_entry & (MAZE_ENTRY_FLAG_1 | MAZE_ENTRY_FLAG_2 | MAZE_ENTRY_FLAG_4))
        PaintAddImageAsParent(
            session, base_image_id + SPR_MAZE_OFFSET_COLUMN_TOP_RIGHT, 0, 14, 1, 2, 9, height, 1, 15, height + 2);

    if (maze_entry & (MAZE_ENTRY_FLAG_9 | MAZE_ENTRY_FLAG_10 | MAZE_ENTRY_FLAG_12))
        PaintAddImageAsParent(
            session, base_image_id + SPR_MAZE_OFFSET_COLUMN_BOTTOM_LEFT, 30, 14, 1, 2, 9, height, 30, 15, height + 2);

    if (maze_entry & (MAZE_ENTRY_FLAG_2 | MAZE_ENTRY_FLAG_6 | MAZE_ENTRY_FLAG_10 | MAZE_ENTRY_FLAG_14))
    {
        PaintAddImageAsParent(
            session, base_image_id + SPR_MAZE_OFFSET_COLUMN_CENTRE, 14, 14, 2, 2, 8, height, 15, 15, height + 2);

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

#pragma region Maze Pathfinding

void swap(MazePathfindingEntry& a, MazePathfindingEntry& b)
{
    std::swap(a.data, b.data);
}

void MazePathfindingEntry::reset()
{
    data = 0;
}

void MazePathfindingEntry::clear()
{
    data = 0xffffffff;
}

void MazePathfindingEntry::set(const CoordsXY& coords, uint8_t subTileIndex, Direction origin)
{
    assert(coords.x > 0 && coords.y > 0);
    data = (coords.y & 0xffe0) << 16 | (coords.x & 0xffe0) | (origin & 0x3) << 2 | (subTileIndex & 0x3);
}

std::tuple<CoordsXY, uint8_t> MazePathfindingEntry::getCoords() const
{
    return std::tuple(CoordsXY(data & 0xffe0, data >> 16 & 0xffe0), (uint8_t)(data & 0x3));
}

Direction MazePathfindingEntry::getOrigin() const
{
    return data >> 2 & 0x3;
}

void MazePathfindingEntry::markVisited(Direction origin, uint8_t openEdgesCount)
{
    uint8_t visited;

    data |= 0x1 << ((origin & 0x3) + 16);
    visited = getVisited();

    if (bitcount(visited) >= openEdgesCount - 1)
        data |= 0x1 << 20;
}

uint8_t MazePathfindingEntry::getVisited() const
{
    return data >> 16 & 0xf;
}

bool MazePathfindingEntry::isCompletlyVisited() const
{
    return data >> 20 & 0x1;
}

void MazePathfindingEntry::notifyPeeking()
{
    data |= 0x1 << 4;
}

bool MazePathfindingEntry::hasPeeked()
{
    bool hasPeeked = (data & 0x1 << 4) != 0;
    data &= ~(0x1 << 4);
    return hasPeeked;
}
bool MazePathfindingEntry::getPeekedState() const
{
    return (data & 0x1 << 4) != 0;
}

bool MazePathfindingEntry::matchCoords(const CoordsXY& coords, uint8_t subTileIndex) const
{
    assert(coords.x > 0 && coords.y > 0);
    return (int32_t)(data >> 16 & 0xffe0) == (coords.y & 0xffe0) && (int32_t)(data & 0xffe0) == (coords.x & 0xffe0)
        && (data & 0x3) == subTileIndex;
}

bool MazePathfindingEntry::operator==(const MazePathfindingEntry& entry) const
{
    // FIXME: PeekedFlag prevents data == entry.data
    return (data & ~0x10) == (entry.data & ~0x10);
}

void MazePathfindingHistory::initialize()
{
    for (auto& item : stack)
        item.reset();
}

void MazePathfindingHistory::clear()
{
    for (auto& item : stack)
        item.clear();
}

std::tuple<MazePathfindingEntry&, uint8_t, bool> MazePathfindingHistory::meetIntersection(
    const CoordsXY& coords, uint8_t subTileIndex, Direction origin)
{
    uint8_t i;
    bool hasPeeked = stack[0].hasPeeked();

    for (i = 0; i < 5; ++i)
    {
        if (stack[i].matchCoords(coords, subTileIndex))
            break;
    }

    for (int j = std::min<int>(4, i); j > 0; --j)
        swap(stack[j - 1], stack[j]);

    if (i == 5)
        stack[0].set(coords, subTileIndex, origin);

    return std::tuple(std::ref(stack[0]), i, hasPeeked);
}

std::optional<std::reference_wrapper<MazePathfindingEntry>> MazePathfindingHistory::updateIntersection(
    const CoordsXY& coords, uint8_t subTileIndex)
{
    for (auto& item : stack)
    {
        if (item.matchCoords(coords, subTileIndex))
            return std::make_optional(std::ref(item));
    }

    return {};
}

const MazePathfindingEntry& MazePathfindingHistory::getLast() const
{
    return stack[0];
}

void MazePathfindingHistory::notifyPeeking()
{
    stack[0].notifyPeeking();
}

const std::array<MazePathfindingEntry, 5>& MazePathfindingHistory::readHistory() const
{
    return stack;
}

#pragma endregion
