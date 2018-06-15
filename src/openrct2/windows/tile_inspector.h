/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../interface/Window.h"

enum TILE_INSPECTOR_PAGE
{
    TILE_INSPECTOR_PAGE_DEFAULT,
    TILE_INSPECTOR_PAGE_SURFACE,
    TILE_INSPECTOR_PAGE_PATH,
    TILE_INSPECTOR_PAGE_TRACK,
    TILE_INSPECTOR_PAGE_SCENERY,
    TILE_INSPECTOR_PAGE_ENTRANCE,
    TILE_INSPECTOR_PAGE_WALL,
    TILE_INSPECTOR_PAGE_LARGE_SCENERY,
    TILE_INSPECTOR_PAGE_BANNER,
    TILE_INSPECTOR_PAGE_CORRUPT
};

extern uint32 windowTileInspectorTileX;
extern uint32 windowTileInspectorTileY;
extern sint32 windowTileInspectorElementCount;
extern sint32 windowTileInspectorSelectedIndex;
