/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/EnumMap.hpp"
#include "../core/Json.hpp"
#include "../core/Memory.hpp"
#include "../paint/Boundbox.h"
#include "../world/Scenery.h"

enum DefaultBoundingBoxType : uint8_t
{
    QuarterTileBox,
    HalfTileBox,
    FullTileNorthQuadrantBox,
    FullTileNortheastSideBox,
    FullTileEastQuadrantBox,
    FullTileSoutheastSideBox,
    FullTileSouthQuadrantBox,
    FullTileSouthwestSideBox,
    FullTileWestQuadrantBox,
    FullTileNorthwestSideBox,
    FullTileBox,
    FullTileLargeBox,
    FullTileThinBox,
    CountBox
};

enum DefaultSpriteOffsetType : uint8_t
{
    QuarterTileOffset,
    FullTileThinOffset,
    FullTileOffset,
    FullTileLargeOffset,
    LargeSceneryOffset,
    CountOffset
};

SceneryBoundBoxes GetDefaultSceneryBoundBoxes(DefaultBoundingBoxType type);
SceneryBoundBoxes ReadBoundBoxes(json_t& jBBox, int32_t defaultHeight, bool fullTile);
CoordsXYZ GetDefaultSpriteOffset(DefaultSpriteOffsetType type);
CoordsXYZ ReadSpriteOffset(json_t& jCoords);
