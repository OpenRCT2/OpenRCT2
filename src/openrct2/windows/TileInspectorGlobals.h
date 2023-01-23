/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../interface/Window.h"
namespace OpenRCT2
{
    enum class TileInspectorPage : int16_t
    {
        Default = 0,
        Surface,
        Path,
        Track,
        Scenery,
        Entrance,
        Wall,
        LargeScenery,
        Banner,
    };

    extern TileCoordsXY windowTileInspectorTile;
    extern int32_t windowTileInspectorElementCount;
    extern int32_t windowTileInspectorSelectedIndex;
} // namespace OpenRCT2
