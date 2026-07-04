/*****************************************************************************
 * Copyright (c) 2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../MapGen.h"
#include "../TileQueue.hpp"

namespace OpenRCT2::World::MapGenerator::Hydro
{

    enum class QueueMode
    {
        height,
        distance,
    };

    struct QueueCfg
    {
        HydroFlag flag = HydroFlag::river;
        QueueMode mode = QueueMode::height;
    };

    void primeHydroFlagQueue(MapGenCtx& context, TrackingStableTileQueue& queue, const QueueCfg& cfg);
    void prepareSourcesAndSinksForPruning(
        MapGenCtx& context, TileCoordsXYSet& sources, TileCoordsXYSet& sinks);
    void prepareConfluencesAndBifurcationsForPruning(
        MapGenCtx& context, BackrefMap& mainUpstreamMap, BackrefMap& mainDownstreamMap, BackrefsMap& auxUpstreamsMap,
        BackrefsMap& auxDownstreamsMap, BooleanMap& confluenceMap, BooleanMap& bifurcationMap);

    void traceFromSources(
        const MapGenCtx& context, const TileCoordsXYSet& sources, BackrefsMap& sourceHits,
        const BackrefsMap& auxDownstreamsMap, BooleanMap& confluenceMap, BackrefMap& confluenceMaxSourceMap);
    void traceFromSinks(
        const MapGenCtx& context, const TileCoordsXYSet& sinks, BackrefsMap& sinkHits,
        const BackrefsMap& auxUpstreamsMap, BooleanMap& bifurcationMap);

    size_t pruneSources(
        MapGenCtx& context, TileCoordsXYSet& sources, const BackrefMap& mainDownstreamMap,
        const BackrefsMap& auxDownstreamsMap, const BooleanMap& confluenceMap, const BackrefsMap& sourceHits,
        const BackrefMap& confluenceMaxSourceMap);
    size_t pruneSinks(
        MapGenCtx& context, TileCoordsXYSet& sinks, const BackrefMap& mainUpstreamMap,
        const BackrefsMap& auxUpstreamsMap, const BooleanMap& bifurcationMap, const BackrefsMap& sinkHits);

} // namespace OpenRCT2::World::MapGenerator::Hydro
