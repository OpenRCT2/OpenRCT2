/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "Map.h"

namespace GameActions
{
    class Result;
}

namespace OpenRCT2::TileInspector
{
    using GameActionResultPtr = std::unique_ptr<GameActions::Result>;

    GameActionResultPtr RemoveElementAt(const CoordsXY& loc, int16_t elementIndex, bool isExecuting);
    GameActionResultPtr SwapElementsAt(const CoordsXY& loc, int16_t first, int16_t second, bool isExecuting);
    GameActionResultPtr RotateElementAt(const CoordsXY& loc, int32_t elementIndex, bool isExecuting);
    GameActionResultPtr ToggleInvisibilityOfElementAt(const CoordsXY& loc, int32_t elementIndex, bool isExecuting);
    GameActionResultPtr PasteElementAt(const CoordsXY& loc, TileElement element, bool isExecuting);
    GameActionResultPtr SortElementsAt(const CoordsXY& loc, bool isExecuting);
    GameActionResultPtr AnyBaseHeightOffset(const CoordsXY& loc, int16_t elementIndex, int8_t heightOffset, bool isExecuting);
    GameActionResultPtr SurfaceShowParkFences(const CoordsXY& loc, bool enabled, bool isExecuting);
    GameActionResultPtr SurfaceToggleCorner(const CoordsXY& loc, int32_t cornerIndex, bool isExecuting);
    GameActionResultPtr SurfaceToggleDiagonal(const CoordsXY& loc, bool isExecuting);
    GameActionResultPtr PathSetSloped(const CoordsXY& loc, int32_t elementIndex, bool sloped, bool isExecuting);
    GameActionResultPtr PathSetBroken(const CoordsXY& loc, int32_t elementIndex, bool broken, bool isExecuting);
    GameActionResultPtr PathToggleEdge(const CoordsXY& loc, int32_t elementIndex, int32_t cornerIndex, bool isExecuting);
    GameActionResultPtr EntranceMakeUsable(const CoordsXY& loc, int32_t elementIndex, bool isExecuting);
    GameActionResultPtr WallSetSlope(const CoordsXY& loc, int32_t elementIndex, int32_t slopeValue, bool isExecuting);
    GameActionResultPtr WallAnimationFrameOffset(
        const CoordsXY& loc, int16_t elementIndex, int8_t animationFrameOffset, bool isExecuting);
    GameActionResultPtr TrackBaseHeightOffset(const CoordsXY& loc, int32_t elementIndex, int8_t offset, bool isExecuting);
    GameActionResultPtr TrackSetBlockBrake(const CoordsXY& loc, int32_t elementIndex, bool blockBrake, bool isExecuting);
    GameActionResultPtr TrackSetIndestructible(
        const CoordsXY& loc, int32_t elementIndex, bool isIndestructible, bool isExecuting);
    GameActionResultPtr TrackSetChain(
        const CoordsXY& loc, int32_t elementIndex, bool entireTrackBlock, bool setChain, bool isExecuting);
    GameActionResultPtr ScenerySetQuarterLocation(
        const CoordsXY& loc, int32_t elementIndex, int32_t quarterIndex, bool isExecuting);
    GameActionResultPtr ScenerySetQuarterCollision(
        const CoordsXY& loc, int32_t elementIndex, int32_t quarterIndex, bool isExecuting);
    GameActionResultPtr BannerToggleBlockingEdge(
        const CoordsXY& loc, int32_t elementIndex, int32_t edgeIndex, bool isExecuting);

} // namespace OpenRCT2::TileInspector
