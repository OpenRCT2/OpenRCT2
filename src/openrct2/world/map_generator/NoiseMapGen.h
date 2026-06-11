/*****************************************************************************
 * Copyright (c) 2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "MapGen.h"

namespace OpenRCT2::World::MapGenerator
{
    void generateSimplexMap(MapGenCtx& context);
    void generateWarpedMap(MapGenCtx& context);
    void generateRidgedMap(MapGenCtx& context);
    void generateVoronoiMap(MapGenCtx& context);
} // namespace OpenRCT2::World::MapGenerator
