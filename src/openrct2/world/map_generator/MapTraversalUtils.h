/*****************************************************************************
 * Copyright (c) 2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "BaseMap.hpp"
#include "MapGen.h"
#include "TileQueue.hpp"

namespace OpenRCT2::World::MapGenerator
{
    void completeDistanceMap(DistanceMap& distanceMap, TrackingStableTileQueue& queue);
    void completeBackrefMap(BackrefMap& backrefMap, TrackingStableTileQueue& queue);
    void initZeroDistance(const TileCoordsXY& pos, DistanceMap& distanceMap, TrackingStableTileQueue& queue);
    void computeHydroFlagBasedDistanceMap(const MapGenCtx& context, DistanceMap& distanceMap, Hydro::HydroFlag flag, bool invert = false);
    void computeHydroFlagBasedBackrefMap(const MapGenCtx& context, BackrefMap& backrefMap, Hydro::HydroFlag flag);

} // namespace OpenRCT2::World::MapGenerator
