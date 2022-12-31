/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
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
    void SetSelectedElement(const TileElement* elem);
    bool IsElementSelected(const TileElement* elem);

    GameActions::Result InsertCorruptElementAt(const CoordsXY& loc, int16_t elementIndex, bool isExecuting);
    GameActions::Result RemoveElementAt(const CoordsXY& loc, int16_t elementIndex, bool isExecuting);
    GameActions::Result SwapElementsAt(const CoordsXY& loc, int16_t first, int16_t second, bool isExecuting);
    GameActions::Result RotateElementAt(const CoordsXY& loc, int32_t elementIndex, bool isExecuting);
    GameActions::Result ToggleInvisibilityOfElementAt(const CoordsXY& loc, int32_t elementIndex, bool isExecuting);
    GameActions::Result PasteElementAt(const CoordsXY& loc, TileElement element, bool isExecuting);
    GameActions::Result SortElementsAt(const CoordsXY& loc, bool isExecuting);
    GameActions::Result AnyBaseHeightOffset(const CoordsXY& loc, int16_t elementIndex, int8_t heightOffset, bool isExecuting);
    GameActions::Result SurfaceShowParkFences(const CoordsXY& loc, bool showFences, bool isExecuting);
    GameActions::Result SurfaceToggleCorner(const CoordsXY& loc, int32_t cornerIndex, bool isExecuting);
    GameActions::Result SurfaceToggleDiagonal(const CoordsXY& loc, bool isExecuting);
    GameActions::Result PathSetSloped(const CoordsXY& loc, int32_t elementIndex, bool sloped, bool isExecuting);
    GameActions::Result PathSetBroken(const CoordsXY& loc, int32_t elementIndex, bool broken, bool isExecuting);
    GameActions::Result PathToggleEdge(const CoordsXY& loc, int32_t elementIndex, int32_t cornerIndex, bool isExecuting);
    GameActions::Result EntranceMakeUsable(const CoordsXY& loc, int32_t elementIndex, bool isExecuting);
    GameActions::Result WallSetSlope(const CoordsXY& loc, int32_t elementIndex, int32_t slopeValue, bool isExecuting);
    GameActions::Result WallAnimationFrameOffset(
        const CoordsXY& loc, int16_t elementIndex, int8_t animationFrameOffset, bool isExecuting);
    GameActions::Result TrackBaseHeightOffset(const CoordsXY& loc, int32_t elementIndex, int8_t offset, bool isExecuting);
    GameActions::Result TrackSetBrakeClosed(const CoordsXY& loc, int32_t elementIndex, bool isClosed, bool isExecuting);
    GameActions::Result TrackSetIndestructible(
        const CoordsXY& loc, int32_t elementIndex, bool isIndestructible, bool isExecuting);
    GameActions::Result TrackSetChain(
        const CoordsXY& loc, int32_t elementIndex, bool entireTrackBlock, bool setChain, bool isExecuting);
    GameActions::Result ScenerySetQuarterLocation(
        const CoordsXY& loc, int32_t elementIndex, int32_t quarterIndex, bool isExecuting);
    GameActions::Result ScenerySetQuarterCollision(
        const CoordsXY& loc, int32_t elementIndex, int32_t quarterIndex, bool isExecuting);
    GameActions::Result BannerToggleBlockingEdge(
        const CoordsXY& loc, int32_t elementIndex, int32_t edgeIndex, bool isExecuting);

} // namespace OpenRCT2::TileInspector
