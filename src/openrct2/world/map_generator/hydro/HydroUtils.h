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
        std::optional<std::function<void(const TileCoordsXY&)>> posCallback = std::nullopt;
    };

    void primeHydroFlagQueue(MapGenCtx& context, TrackingStableTileQueue& queue, const QueueCfg& cfg);
    void findSourcesAndSinks(MapGenCtx& context, TileCoordsXYSet& sources, TileCoordsXYSet& sinks);

} // namespace OpenRCT2::World::MapGenerator::Hydro
