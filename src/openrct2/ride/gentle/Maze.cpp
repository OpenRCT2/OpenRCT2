/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
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

template<typename _T> static constexpr _T MaskField(uint8_t size, uint8_t offset = 0)
{
    return ((1 << size) - 1) << offset;
}

template<typename _T> static inline _T GetField(_T opaqueData, uint8_t size, uint8_t offset = 0)
{
    return (opaqueData & MaskField<_T>(size, offset)) >> offset;
}

template<typename _T> static inline void SetField(_T& opaqueData, _T value, uint8_t size, uint8_t offset)
{
    opaqueData &= ~MaskField<_T>(size, offset);
    opaqueData |= GetField(value, size) << offset;
}

void PeepMazeStateRegister::SetLastDirection(Direction lastEdge)
{
    SetField(Data, lastEdge, 2, 0);
}

Direction PeepMazeStateRegister::GetLastDirection() const
{
    return GetField(Data, 2, 0);
}

void PeepMazeStateRegister::ReverseLastDirection()
{
    SetField(Data, direction_reverse(GetField(Data, 2, 0)), 2, 0);
}

void PeepMazeStateRegister::SetDirectionAtLastIntersection(Direction lastEdge)
{
    SetField(Data, lastEdge, 2, 2);
}

Direction PeepMazeStateRegister::GetDirectionAtLastIntersection() const
{
    return GetField(Data, 2, 2);
}

bool PeepMazeStateRegister::GetPeekFlag() const
{
    return GetField(Data, 1, 4);
}

bool PeepMazeStateRegister::CheckAndResetPeekFlag()
{
    bool state = GetPeekFlag();

    SetField<uint8_t>(Data, false, 1, 4);

    return state;
}

void PeepMazeStateRegister::SetPeekFlag()
{
    SetField<uint8_t>(Data, true, 1, 4);
}

void MazePathfindingEntry::SetSubTileIndex(uint8_t subTileIndex)
{
    SetField<uint32_t>(Data, subTileIndex, 2, 0);
}

uint8_t MazePathfindingEntry::GetSubTileIndex() const
{
    return GetField(Data, 2, 0);
}

void MazePathfindingEntry::SetOrigin(Direction origin)
{
    SetField<uint32_t>(Data, origin, 2, 2);
}

void MazePathfindingEntry::SetXTile(uint16_t xTile)
{
    SetField<uint32_t>(Data, xTile, 11, 5);
}

uint16_t MazePathfindingEntry::GetXTile() const
{
    return GetField(Data, 11, 5);
}

void MazePathfindingEntry::SetVisitedFlags(uint8_t visitedFlags)
{
    SetField<uint32_t>(Data, visitedFlags, 4, 16);
}

void MazePathfindingEntry::AddVisitedFlags(Direction visitedDirection)
{
    SetVisitedFlags(GetVisited() | (1 << GetField(visitedDirection, 2)));
}

void MazePathfindingEntry::DelVisitedFlags(Direction visitedDirection)
{
    SetVisitedFlags(GetVisited() & ~(1 << GetField(visitedDirection, 2)));
}

void MazePathfindingEntry::SetCompletlyVisited(bool completlyVisited)
{
    SetField<uint32_t>(Data, completlyVisited, 1, 20);
}

void MazePathfindingEntry::SetYTile(uint16_t yTile)
{
    SetField<uint32_t>(Data, yTile, 11, 21);
}

uint16_t MazePathfindingEntry::GetYTile() const
{
    return GetField(Data, 11, 21);
}

void MazePathfindingEntry::Reset()
{
    Data = 0x0;
}

void MazePathfindingEntry::Free()
{
    Data = ~0x0u;
}

void MazePathfindingEntry::Set(const CoordsXY& coords, uint8_t subTileIndex, Direction origin)
{
    assert(coords.x > 0 && coords.y > 0);
    TileCoordsXY tileCoords(coords);

    Reset();
    SetSubTileIndex(subTileIndex);
    SetOrigin(origin);
    SetXTile(tileCoords.x);
    SetYTile(tileCoords.y);
}

std::pair<CoordsXY, uint8_t> MazePathfindingEntry::GetCoords() const
{
    TileCoordsXY tileCoords(GetXTile(), GetYTile());

    return std::pair(tileCoords.ToCoordsXY(), GetSubTileIndex());
}

Direction MazePathfindingEntry::GetOrigin() const
{
    return GetField(Data, 2, 2);
}

void MazePathfindingEntry::MarkVisited(Direction source, uint8_t openEdgesCount)
{
    AddVisitedFlags(source);

    if (bitcount(GetVisited()) >= openEdgesCount - 1)
        SetCompletlyVisited(true);
}

void MazePathfindingEntry::UnmarkVisited(Direction direction)
{
    DelVisitedFlags(direction);
    if (direction != GetOrigin())
        SetCompletlyVisited(false);
}

uint8_t MazePathfindingEntry::GetVisited() const
{
    return GetField(Data, 4, 16);
}

bool MazePathfindingEntry::IsCompletlyVisited() const
{
    return GetField(Data, 1, 20);
}

bool MazePathfindingEntry::MatchCoords(const CoordsXY& coords, uint8_t subTileIndex) const
{
    TileCoordsXY tileCoords(coords);

    return (GetXTile() == tileCoords.x) && (GetYTile() == tileCoords.y) && (GetSubTileIndex() == subTileIndex);
}

bool MazePathfindingEntry::MatchCoords(const MazePathfindingEntry& e) const
{
    return (GetXTile() == e.GetXTile()) && (GetYTile() == e.GetYTile()) && (GetSubTileIndex() == e.GetSubTileIndex());
}

void MazePathfindingHistory::Initialize()
{
    for (auto& item : Stack)
        item.Reset();
}

void MazePathfindingHistory::Free()
{
    for (auto& item : Stack)
        item.Free();
}

std::pair<MazePathfindingEntry&, uint8_t> MazePathfindingHistory::MeetIntersection(
    const CoordsXY& coords, uint8_t subTileIndex, Direction source)
{
    uint8_t i;

    for (i = 0; i < Stack.size(); ++i)
    {
        if (Stack[i].MatchCoords(coords, subTileIndex))
            break;
    }

    for (uint8_t j = std::min(static_cast<uint8_t>(Stack.size() - 1), i); j > 0; --j)
        std::swap(Stack[j - 1], Stack[j]);

    if (i == Stack.size())
        Stack[0].Set(coords, subTileIndex, source);

    return std::pair(std::ref(Stack[0]), i);
}

MazePathfindingEntry& MazePathfindingHistory::GetLast()
{
    return Stack[0];
}

const std::array<MazePathfindingEntry, 5>& MazePathfindingHistory::GetHistory() const
{
    return Stack;
}

#pragma endregion
