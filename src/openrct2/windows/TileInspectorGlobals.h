/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

struct TileCoordsXY;

enum class TileInspectorPage : int16_t
{
    standard = 0,
    surface,
    path,
    track,
    scenery,
    entrance,
    wall,
    largeScenery,
    banner,
};

extern TileCoordsXY windowTileInspectorTile;
extern int32_t windowTileInspectorElementCount;
extern int32_t windowTileInspectorSelectedIndex;
