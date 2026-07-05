/*****************************************************************************
 * Copyright (c) 2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../../core/FlagHolder.hpp"
#include "../BaseMap.hpp"

namespace OpenRCT2::World::MapGenerator::Hydro
{
    constexpr int32_t kRiversOverscanFactor = 2;
    constexpr int32_t kRiversSeafloorMaxCarveDepth = 3;
    constexpr float kRiverGrowthExponentScaling = 0.01f;
    constexpr int32_t kRiverGrowthExponentMin = 1;   // * kRiverGrowthExponentScaling
    constexpr int32_t kRiverGrowthExponentMax = 800; // * kRiverGrowthExponentScaling
    constexpr int32_t kRiverCatchmentThresholdMin = 2;
    constexpr int32_t kRiverCatchmentThresholdMax = 2 << 19;
    constexpr int32_t kRiverPruneLengthThresholdMin = 0;
    constexpr int32_t kRiverPruneLengthThresholdMax = 128;
    constexpr int32_t kRiverWidthMin = 1;
    constexpr int32_t kRiverWidthMax = 128;
    constexpr int32_t kRiverBreachLengthMin = 0;
    constexpr int32_t kRiverBreachLengthMax = 128;
    constexpr int32_t kRiverBreachDepthMin = 0;
    constexpr int32_t kRiverBreachDepthMax = 32;

    enum HydroFlag : uint8_t
    {
        river,
        filled,
        breached,
        source,
        // TODO lake,
    };

    using HydroFlags = FlagHolder<uint8_t, HydroFlag>;

    struct HydroStatistics
    {
        int32_t pitsFound = 0;
        int32_t pitsBreachSuccess = 0;
        int32_t pitsBreachedTiles = 0;
        int32_t pitsFilledTiles = 0;

        float flowAggMax = -std::numeric_limits<float>::infinity();

        int32_t pruneSinksFound = 0;
        int32_t pruneSinksTilesRemoved = 0;
        int32_t pruneSourcesFound = 0;
        int32_t pruneSourcesTilesRemoved = 0;
        int32_t pruneSourcesRemaining = 0;
        float pruneSourcesLongest = 0;

        int32_t widthAdjustNewTiles = 0;

        int32_t ensureCardinalNewTiles = 0;

        int32_t bankIndentationsRemoved = 0;

        int32_t consistencySegmentsIterations = 1;
        int32_t consistencySegmentsRaised = 0;
        int32_t consistencySegmentsLowered = 0;
        int32_t consistencySegmentsRemoved = 0;
        int32_t consistencySegmentsRaisedMaxSize = 0;
        int32_t consistencySegmentsLoweredMaxSize = 0;
        int32_t consistencySegmentsRemovedMaxSize = 0;
        int32_t consistencyBanksRaised = 0;
    };

    struct HydroContext
    {
        MapDirectionMaskMap flowsIn;
        MapDirectionMaskMap flowsOut;
        MapDirectionMaskMap flowsLateral;
        BaseMap<float> catchment;
        BaseMap<float> height;
        BaseMap<HydroFlags> flags;

        HydroStatistics stats;

        HydroContext(const TileCoordsXY& size)
            : flowsIn(size)
            , flowsOut(size)
            , flowsLateral(size)
            , catchment(size)
            , height(size)
            , flags(size)
        {
        }
    };

} // namespace OpenRCT2::World::MapGenerator::Hydro
