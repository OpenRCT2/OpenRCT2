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
    using Backref = std::optional<TileCoordsXY>;
    using BackrefMap = BaseMap<Backref>;

    void completeDistanceMap(DistanceMap& distanceMap, StableTileQueue& queue, MaskMap& visited);
    void initZeroDistance(const TileCoordsXY& pos, DistanceMap& distanceMap, StableTileQueue& queue, MaskMap& maskMap);
    void computeHydroFlagBasedDistanceMap(const MapGenCtx& genCtx, DistanceMap& distanceMap, Hydro::HydroFlag flag);
    void computeNearestMapFromDistanceMap(const DistanceMap& distanceMap, BackrefMap& backrefMap);
} // namespace OpenRCT2::World::MapGenerator
