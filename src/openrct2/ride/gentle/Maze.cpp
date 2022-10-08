/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
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
    SprMazeBaseHedge = 21938,
    SprMazeBaseBrick = 21951,
    SprMazeBaseIce = 21964,
    SprMazeBaseWood = 21977,
};

enum
{
    SprMazeOffsetWallCentre = 0,
    SprMazeOffsetWallInnerNeSw,
    SprMazeOffsetWallInnerNwSe,
    SprMazeOffsetWallTopLeft,
    SprMazeOffsetWallTopRight,
    SprMazeOffsetWallBottomRight,
    SprMazeOffsetWallBottomLeft,
    SprMazeOffsetColumnCentre,
    SprMazeOffsetColumnTopRight,
    SprMazeOffsetColumnTopLeft,
    SprMazeOffsetColumnBottomLeft,
    SprMazeOffsetColumnBottomRight,
    SprMazeOffsetColumnCorner,
};

/**
 * rct: 0x004ACF4A
 */
static void MazePaintSetup(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    uint16_t mazeEntry = trackElement.GetMazeEntry();
    mazeEntry = Numerics::rol16(mazeEntry, direction * 4);

    uint32_t rotation = session.CurrentRotation;
    // draw ground
    auto imageId = session.TrackColours[SCHEME_MISC].WithIndex(SPR_TERRAIN_DIRT);
    PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 32, 0 });

    WoodenASupportsPaintSetup(session, (rotation & 1) ? 0 : 1, 0, height, session.TrackColours[SCHEME_3]);

    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL & ~SEGMENT_C4, 0xFFFF, 0);

    int32_t baseImageId = 0;
    switch (ride.track_colour[0].supports)
    {
        case 0:
            baseImageId = SprMazeBaseBrick;
            break;
        case 1:
            baseImageId = SprMazeBaseHedge;
            break;
        case 2:
            baseImageId = SprMazeBaseIce;
            break;
        case 3:
            baseImageId = SprMazeBaseWood;
            break;
    }

    auto baseImage = session.TrackColours[SCHEME_MISC].WithIndex(baseImageId);

    imageId = baseImage.WithIndexOffset(SprMazeOffsetWallCentre);
    if (mazeEntry & MAZE_ENTRY_FLAG_3)
        PaintAddImageAsParent(session, imageId, { 2, 2, height }, { 10, 10, 9 }, { 3, 3, height + 2 });

    if (mazeEntry & MAZE_ENTRY_FLAG_7)
        PaintAddImageAsParent(session, imageId, { 2, 18, height }, { 10, 10, 9 }, { 3, 19, height + 2 });

    if (mazeEntry & MAZE_ENTRY_FLAG_11)
        PaintAddImageAsParent(session, imageId, { 18, 18, height }, { 10, 10, 9 }, { 19, 19, height + 2 });

    if (mazeEntry & MAZE_ENTRY_FLAG_15)
        PaintAddImageAsParent(session, imageId, { 18, 2, height }, { 10, 10, 9 }, { 19, 3, height + 2 });

    imageId = baseImage.WithIndexOffset(SprMazeOffsetWallTopLeft);
    if (mazeEntry & MAZE_ENTRY_FLAG_0)
        PaintAddImageAsParent(session, imageId, { 2, 0, height }, { 10, 1, 9 }, { 3, 1, height + 2 });

    if (mazeEntry & MAZE_ENTRY_FLAG_13)
        PaintAddImageAsParent(session, imageId, { 18, 0, height }, { 10, 1, 9 }, { 19, 1, height + 2 });

    imageId = baseImage.WithIndexOffset(SprMazeOffsetWallBottomRight);
    if (mazeEntry & MAZE_ENTRY_FLAG_5)
        PaintAddImageAsParent(session, imageId, { 2, 30, height }, { 10, 1, 9 }, { 3, 30, height + 2 });

    if (mazeEntry & MAZE_ENTRY_FLAG_8)
        PaintAddImageAsParent(session, imageId, { 18, 30, height }, { 10, 1, 9 }, { 19, 30, height + 2 });

    imageId = baseImage.WithIndexOffset(SprMazeOffsetWallTopRight);
    if (mazeEntry & MAZE_ENTRY_FLAG_1)
        PaintAddImageAsParent(session, imageId, { 0, 2, height }, { 1, 10, 9 }, { 1, 3, height + 2 });

    if (mazeEntry & MAZE_ENTRY_FLAG_4)
        PaintAddImageAsParent(session, imageId, { 0, 18, height }, { 1, 10, 9 }, { 1, 19, height + 2 });

    imageId = baseImage.WithIndexOffset(SprMazeOffsetWallBottomLeft);
    if (mazeEntry & MAZE_ENTRY_FLAG_12)
        PaintAddImageAsParent(session, imageId, { 30, 2, height }, { 1, 10, 9 }, { 30, 3, height + 2 });

    if (mazeEntry & MAZE_ENTRY_FLAG_9)
        PaintAddImageAsParent(session, imageId, { 30, 18, height }, { 1, 10, 9 }, { 30, 19, height + 2 });

    imageId = baseImage.WithIndexOffset(SprMazeOffsetWallInnerNeSw);
    if (mazeEntry & MAZE_ENTRY_FLAG_2)
        PaintAddImageAsParent(session, imageId, { 2, 14, height }, { 10, 4, 9 }, { 3, 14, height + 2 });

    if (mazeEntry & MAZE_ENTRY_FLAG_10)
        PaintAddImageAsParent(session, imageId, { 18, 14, height }, { 10, 4, 9 }, { 19, 14, height + 2 });

    imageId = baseImage.WithIndexOffset(SprMazeOffsetWallInnerNwSe);
    if (mazeEntry & MAZE_ENTRY_FLAG_14)
        PaintAddImageAsParent(session, imageId, { 14, 2, height }, { 4, 10, 9 }, { 14, 3, height + 2 });

    if (mazeEntry & MAZE_ENTRY_FLAG_6)
        PaintAddImageAsParent(session, imageId, { 14, 18, height }, { 4, 10, 9 }, { 14, 19, height + 2 });

    imageId = baseImage.WithIndexOffset(SprMazeOffsetColumnCorner);
    if (mazeEntry & (MAZE_ENTRY_FLAG_0 | MAZE_ENTRY_FLAG_1))
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 1, 1, 9 }, { 1, 1, height + 2 });

    if (mazeEntry & (MAZE_ENTRY_FLAG_4 | MAZE_ENTRY_FLAG_5))
        PaintAddImageAsParent(session, imageId, { 0, 30, height }, { 1, 1, 9 }, { 1, 30, height + 2 });

    if (mazeEntry & (MAZE_ENTRY_FLAG_8 | MAZE_ENTRY_FLAG_9))
        PaintAddImageAsParent(session, imageId, { 30, 30, height }, { 1, 1, 9 }, { 30, 30, height + 2 });

    if (mazeEntry & (MAZE_ENTRY_FLAG_12 | MAZE_ENTRY_FLAG_13))
        PaintAddImageAsParent(session, imageId, { 30, 0, height }, { 1, 1, 9 }, { 30, 1, height + 2 });

    if (mazeEntry & (MAZE_ENTRY_FLAG_0 | MAZE_ENTRY_FLAG_13 | MAZE_ENTRY_FLAG_14))
        PaintAddImageAsParent(
            session, ImageId(baseImageId + SprMazeOffsetColumnTopLeft), { 14, 0, height }, { 2, 1, 9 }, { 15, 1, height + 2 });

    if (mazeEntry & (MAZE_ENTRY_FLAG_5 | MAZE_ENTRY_FLAG_6 | MAZE_ENTRY_FLAG_8))
        PaintAddImageAsParent(
            session, ImageId(baseImageId + SprMazeOffsetColumnBottomRight), { 14, 30, height }, { 2, 1, 9 },
            { 15, 30, height + 2 });

    if (mazeEntry & (MAZE_ENTRY_FLAG_1 | MAZE_ENTRY_FLAG_2 | MAZE_ENTRY_FLAG_4))
        PaintAddImageAsParent(
            session, ImageId(baseImageId + SprMazeOffsetColumnTopRight), { 0, 14, height }, { 1, 2, 9 }, { 1, 15, height + 2 });

    if (mazeEntry & (MAZE_ENTRY_FLAG_9 | MAZE_ENTRY_FLAG_10 | MAZE_ENTRY_FLAG_12))
        PaintAddImageAsParent(
            session, ImageId(baseImageId + SprMazeOffsetColumnBottomLeft), { 30, 14, height }, { 1, 2, 9 },
            { 30, 15, height + 2 });

    if (mazeEntry & (MAZE_ENTRY_FLAG_2 | MAZE_ENTRY_FLAG_6 | MAZE_ENTRY_FLAG_10 | MAZE_ENTRY_FLAG_14))
    {
        PaintAddImageAsParent(
            session, ImageId(baseImageId + SprMazeOffsetColumnCentre), { 14, 14, height }, { 2, 2, 8 }, { 15, 15, height + 2 });

        PaintUtilSetSegmentSupportHeight(session, SEGMENT_C4, height + 12, 0x20);
    }

    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/**
 * rct2: 0x008A81E8
 */
TRACK_PAINT_FUNCTION GetTrackPaintFunctionMaze(int32_t trackType)
{
    if (trackType != TrackElemType::Maze)
    {
        return nullptr;
    }

    return MazePaintSetup;
}

money64 MazeCalculateCost(money32 constructionCost, const Ride& ride, const CoordsXYZ& loc)
{
    const auto& ted = GetTrackElementDescriptor(TrackElemType::Maze);
    money64 price = (ride.GetRideTypeDescriptor().BuildCosts.TrackPrice * ted.PriceModifier) >> 16;

    auto surfaceElement = map_get_surface_element_at(loc);
    auto heightDifference = (loc.z - surfaceElement->GetBaseZ()) / COORDS_Z_PER_TINY_Z;
    money64 supportCost = heightDifference * ride.GetRideTypeDescriptor().BuildCosts.SupportPrice;

    return constructionCost + price + supportCost;
}
