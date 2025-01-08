/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once
#include <openrct2/world/Location.hpp>

namespace OpenRCT2::Ui
{
    bool ViewportInteractionLeftOver(const ScreenCoordsXY& screenCoords);
    bool ViewportInteractionLeftClick(const ScreenCoordsXY& screenCoords);
    bool ViewportInteractionRightOver(const ScreenCoordsXY& screenCoords);
    bool ViewportInteractionRightClick(const ScreenCoordsXY& screenCoords);

    CoordsXY ViewportInteractionGetTileStartAtCursor(const ScreenCoordsXY& screenCoords);
} // namespace OpenRCT2::Ui
