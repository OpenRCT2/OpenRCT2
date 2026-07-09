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

    bool loadHeightMapImage(const utf8* path);
    void unloadHeightMapImage();
    TileCoordsXY queryHeightMapFromImageDimensions();
    void generateHeightMapFromImage(MapGenContext& ctx);
} // namespace OpenRCT2::World::MapGenerator
