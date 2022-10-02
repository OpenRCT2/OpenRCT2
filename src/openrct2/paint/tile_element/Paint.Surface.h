/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../common.h"
#include "../../sprites.h"
#include "../../world/Location.hpp"

#include <optional>

enum
{
    SprTerrainGrass = 1915,
    SprTerrainGrassGrid = 1934,
    SprTerrainGrassUnderground = 1953,
    SprTerrainSand = 1972,
    SprTerrainSandGrid = 1991,
    SprTerrainSandUnderground = 2010,
    SprTerrainSandRed = 2029,
    SprTerrainSandRedGrid = 2048,
    SprTerrainSandRedUnderground = 2067,
    SprTerrainSandYellow = 2086,
    SprTerrainSandYellowGrid = 2105,
    SprTerrainSandYellowUnderground = 2124,
    SprTerrainIce = 2143,
    SprTerrainIceGrid = 2162,
    SprTerrainIceUnderground = 2181,
    SprTerrainGrassClumps = 2200,
    SprTerrainGrassClumpsGrid = 2219,
    SprTerrainGrassClumpsUnderground = 2238,
    SprTerrainGrid = 2257,
    SprTerrainGridGrid = 2276,
    SprTerrainGridUnderground = 2295,
    SprTerrainMartian = 2314,
    SprTerrainMartianGrid = 2333,
    SprTerrainMartianUnderground = 2352,
    SprTerrainCheckerboard = 2371,
    SprTerrainCheckerboardGrid = 2390,
    SprTerrainCheckerboardUnderground = 2409,
    SprTerrainCheckerboardInverted = 2428,
    SprTerrainCheckerboardInvertedGrid = 2447,
    SprTerrainCheckerboardInvertedUnderground = 2466,
    SprTerrainDirt = 2485,
    SprTerrainDirtGrid = 2504,
    SprTerrainDirtUnderground = 2523,
    SprTerrainRock = 2542,
    SprTerrainRockGrid = 2561,
    SprTerrainRockUnderground = 2580,
    SprTerrainSelectionPatrolArea = 2599,
    SprTerrainTrackDesigner = 2623,
    SprTerrainGrassMowed = 2663,
    SprTerrainGrassMowedGrid = 2682,
    SprTerrainGrassMowed90 = 2701,
    SprTerrainGrassMowed90Grid = 2720,
    SprTerrainGrassLength4Variant1 = 2739,
    SprTerrainGrassLength4Variant1Grid = 2758,
    SprTerrainGrassLength4Variant2 = 2777,
    SprTerrainGrassLength4Variant2Grid = 2796,
    SprTerrainGrassLength4Variant3 = 2815,
    SprTerrainGrassLength4Variant3Grid = 2834,
    SprTerrainGrassLength4Variant4 = 2853,
    SprTerrainGrassLength4Variant4Grid = 2872,
    SprTerrainGrassLength6Variant1 = 2891,
    SprTerrainGrassLength6Variant1Grid = 2910,
    SprTerrainGrassLength6Variant2 = 2929,
    SprTerrainGrassLength6Variant2Grid = 2948,
    SprTerrainGrassLength6Variant3 = 2967,
    SprTerrainGrassLength6Variant3Grid = 2986,
    SprTerrainGrassLength6Variant4 = 3005,
    SprTerrainGrassLength6Variant4Grid = 3024,
    SprTerrainSelectionCorner = 3043,
    SprTerrainSelectionEdge = 3062,
    SprTerrainSelectionQuarter = 3081,
    SprWaterMask = 5048,
    SprWaterOverlay = 5053,

    SprTerrainBoundaryFences1 = 22872,
    SprTerrainBoundaryFences2 = 22873,
    SprTerrainBoundaryFences3 = 22874,
    SprTerrainBoundaryFences4 = 22875,
    SprTerrainBoundaryFences5 = 22876,
    SprTerrainBoundaryFences6 = 22877,

    SprTerrainEdgeMaskTopRight = 28883,
    SprTerrainEdgeMaskBottomRight = 28902,
    SprTerrainEdgeMaskBottomLeft = 28921,
    SprTerrainEdgeMaskTopLeft = 28940,
    SprTerrainPatternGrass = 28959,
    SprTerrainPatternSand = 28965,
    SprTerrainPatternSandRed = 28971,
    SprTerrainPatternDirt = 28977,
    SprTerrainPatternSandYellow = 28983,
    SprTerrainPatternRock = 28989,
    SprTerrainPatternMartian = 28995,
    SprTerrainPatternGrassClumps = 29001,
    SprTerrainPatternIce = 29007,

    SprRct1WaterMask = SPR_CSG_BEGIN + 46787,
    SprRct1WaterOverlay = SPR_CSG_BEGIN + 46792,
};

std::optional<colour_t> GetPatrolAreaTileColour(const CoordsXY& pos);
