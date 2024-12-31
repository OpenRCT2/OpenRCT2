/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../core/Numerics.hpp"
#include "../../../interface/Viewport.h"
#include "../../../ride/Ride.h"
#include "../../../ride/RideData.h"
#include "../../../ride/Track.h"
#include "../../../ride/TrackPaint.h"
#include "../../../sprites.h"
#include "../../../world/Map.h"
#include "../../../world/tile_element/TrackElement.h"
#include "../../Paint.h"
#include "../../support/WoodenSupports.h"
#include "../../tile_element/Paint.Surface.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

using namespace OpenRCT2;

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
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    uint16_t mazeEntry = trackElement.GetMazeEntry();
    mazeEntry = Numerics::rol16(mazeEntry, direction * 4);

    uint32_t rotation = session.CurrentRotation;
    // draw ground
    auto imageId = GetStationColourScheme(session, trackElement).WithIndex(SPR_TERRAIN_DIRT);
    PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 32, 0 });

    WoodenASupportsPaintSetupRotated(
        session, WoodenSupportType::Truss, WoodenSupportSubType::NwSe, rotation, height,
        GetShopSupportColourScheme(session, trackElement));

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll & ~EnumToFlag(PaintSegment::centre), 0xFFFF, 0);

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

    auto baseImage = GetStationColourScheme(session, trackElement).WithIndex(baseImageId);

    imageId = baseImage.WithIndexOffset(SprMazeOffsetWallCentre);
    if (mazeEntry & MAZE_ENTRY_FLAG_3)
        PaintAddImageAsParent(session, imageId, { 2, 2, height }, { { 3, 3, height + 2 }, { 10, 10, 9 } });

    if (mazeEntry & MAZE_ENTRY_FLAG_7)
        PaintAddImageAsParent(session, imageId, { 2, 18, height }, { { 3, 19, height + 2 }, { 10, 10, 9 } });

    if (mazeEntry & MAZE_ENTRY_FLAG_11)
        PaintAddImageAsParent(session, imageId, { 18, 18, height }, { { 19, 19, height + 2 }, { 10, 10, 9 } });

    if (mazeEntry & MAZE_ENTRY_FLAG_15)
        PaintAddImageAsParent(session, imageId, { 18, 2, height }, { { 19, 3, height + 2 }, { 10, 10, 9 } });

    imageId = baseImage.WithIndexOffset(SprMazeOffsetWallTopLeft);
    if (mazeEntry & MAZE_ENTRY_FLAG_0)
        PaintAddImageAsParent(session, imageId, { 2, 0, height }, { { 3, 1, height + 2 }, { 10, 1, 9 } });

    if (mazeEntry & MAZE_ENTRY_FLAG_13)
        PaintAddImageAsParent(session, imageId, { 18, 0, height }, { { 19, 1, height + 2 }, { 10, 1, 9 } });

    imageId = baseImage.WithIndexOffset(SprMazeOffsetWallBottomRight);
    if (mazeEntry & MAZE_ENTRY_FLAG_5)
        PaintAddImageAsParent(session, imageId, { 2, 30, height }, { { 3, 30, height + 2 }, { 10, 1, 9 } });

    if (mazeEntry & MAZE_ENTRY_FLAG_8)
        PaintAddImageAsParent(session, imageId, { 18, 30, height }, { { 19, 30, height + 2 }, { 10, 1, 9 } });

    imageId = baseImage.WithIndexOffset(SprMazeOffsetWallTopRight);
    if (mazeEntry & MAZE_ENTRY_FLAG_1)
        PaintAddImageAsParent(session, imageId, { 0, 2, height }, { { 1, 3, height + 2 }, { 1, 10, 9 } });

    if (mazeEntry & MAZE_ENTRY_FLAG_4)
        PaintAddImageAsParent(session, imageId, { 0, 18, height }, { { 1, 19, height + 2 }, { 1, 10, 9 } });

    imageId = baseImage.WithIndexOffset(SprMazeOffsetWallBottomLeft);
    if (mazeEntry & MAZE_ENTRY_FLAG_12)
        PaintAddImageAsParent(session, imageId, { 30, 2, height }, { { 30, 3, height + 2 }, { 1, 10, 9 } });

    if (mazeEntry & MAZE_ENTRY_FLAG_9)
        PaintAddImageAsParent(session, imageId, { 30, 18, height }, { { 30, 19, height + 2 }, { 1, 10, 9 } });

    imageId = baseImage.WithIndexOffset(SprMazeOffsetWallInnerNeSw);
    if (mazeEntry & MAZE_ENTRY_FLAG_2)
        PaintAddImageAsParent(session, imageId, { 2, 14, height }, { { 3, 14, height + 2 }, { 10, 4, 9 } });

    if (mazeEntry & MAZE_ENTRY_FLAG_10)
        PaintAddImageAsParent(session, imageId, { 18, 14, height }, { { 19, 14, height + 2 }, { 10, 4, 9 } });

    imageId = baseImage.WithIndexOffset(SprMazeOffsetWallInnerNwSe);
    if (mazeEntry & MAZE_ENTRY_FLAG_14)
        PaintAddImageAsParent(session, imageId, { 14, 2, height }, { { 14, 3, height + 2 }, { 4, 10, 9 } });

    if (mazeEntry & MAZE_ENTRY_FLAG_6)
        PaintAddImageAsParent(session, imageId, { 14, 18, height }, { { 14, 19, height + 2 }, { 4, 10, 9 } });

    imageId = baseImage.WithIndexOffset(SprMazeOffsetColumnCorner);
    if (mazeEntry & (MAZE_ENTRY_FLAG_0 | MAZE_ENTRY_FLAG_1))
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 1, 1, height + 2 }, { 1, 1, 9 } });

    if (mazeEntry & (MAZE_ENTRY_FLAG_4 | MAZE_ENTRY_FLAG_5))
        PaintAddImageAsParent(session, imageId, { 0, 30, height }, { { 1, 30, height + 2 }, { 1, 1, 9 } });

    if (mazeEntry & (MAZE_ENTRY_FLAG_8 | MAZE_ENTRY_FLAG_9))
        PaintAddImageAsParent(session, imageId, { 30, 30, height }, { { 30, 30, height + 2 }, { 1, 1, 9 } });

    if (mazeEntry & (MAZE_ENTRY_FLAG_12 | MAZE_ENTRY_FLAG_13))
        PaintAddImageAsParent(session, imageId, { 30, 0, height }, { { 30, 1, height + 2 }, { 1, 1, 9 } });

    imageId = baseImage.WithIndexOffset(SprMazeOffsetColumnTopLeft);
    if (mazeEntry & (MAZE_ENTRY_FLAG_0 | MAZE_ENTRY_FLAG_13 | MAZE_ENTRY_FLAG_14))
        PaintAddImageAsParent(session, imageId, { 14, 0, height }, { { 15, 1, height + 2 }, { 2, 1, 9 } });

    imageId = baseImage.WithIndexOffset(SprMazeOffsetColumnBottomRight);
    if (mazeEntry & (MAZE_ENTRY_FLAG_5 | MAZE_ENTRY_FLAG_6 | MAZE_ENTRY_FLAG_8))
        PaintAddImageAsParent(session, imageId, { 14, 30, height }, { { 15, 30, height + 2 }, { 2, 1, 9 } });

    imageId = baseImage.WithIndexOffset(SprMazeOffsetColumnTopRight);
    if (mazeEntry & (MAZE_ENTRY_FLAG_1 | MAZE_ENTRY_FLAG_2 | MAZE_ENTRY_FLAG_4))
        PaintAddImageAsParent(session, imageId, { 0, 14, height }, { { 1, 15, height + 2 }, { 1, 2, 9 } });

    imageId = baseImage.WithIndexOffset(SprMazeOffsetColumnBottomLeft);
    if (mazeEntry & (MAZE_ENTRY_FLAG_9 | MAZE_ENTRY_FLAG_10 | MAZE_ENTRY_FLAG_12))
        PaintAddImageAsParent(session, imageId, { 30, 14, height }, { { 30, 15, height + 2 }, { 1, 2, 9 } });

    if (mazeEntry & (MAZE_ENTRY_FLAG_2 | MAZE_ENTRY_FLAG_6 | MAZE_ENTRY_FLAG_10 | MAZE_ENTRY_FLAG_14))
    {
        imageId = baseImage.WithIndexOffset(SprMazeOffsetColumnCentre);
        PaintAddImageAsParent(session, imageId, { 14, 14, height }, { { 15, 15, height + 2 }, { 2, 2, 8 } });

        PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::centre), height + 12, 0x20);
    }

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/**
 * rct2: 0x008A81E8
 */
TrackPaintFunction GetTrackPaintFunctionMaze(OpenRCT2::TrackElemType trackType)
{
    if (trackType != TrackElemType::Maze)
    {
        return TrackPaintFunctionDummy;
    }

    return MazePaintSetup;
}
