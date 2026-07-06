/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../core/StringTypes.h"
#include "../Location.hpp"

namespace OpenRCT2::World::MapGenerator
{
    struct MapGenContext;

    bool loadHeightmapImage(const utf8* path);
    void unloadHeightmapImage();
    TileCoordsXY queryHeightmapImageDimensions();
    void generateFromHeightmapImage(MapGenContext& ctx);
} // namespace OpenRCT2::World::MapGenerator
