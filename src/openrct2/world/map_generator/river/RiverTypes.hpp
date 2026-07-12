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
#include "../SettingsTypes.hpp"

namespace OpenRCT2::World::MapGenerator::River
{
    constexpr int32_t kRiversOverscanFactor = 2;
    constexpr int32_t kRiversSeafloorMaxCarveDepth = 3;
    constexpr float kRiverGrowthExponentScaling = 0.01f;
    constexpr float kRiverFlowAggregationSlopeExponentScaling = 0.01f;
    constexpr float kRiverOffMapCatchmentScaling = 0.1f;
    constexpr int32_t kRiverGrowthExponentMin = 1;                 // * kRiverGrowthExponentScaling
    constexpr int32_t kRiverGrowthExponentMax = 800;               // * kRiverGrowthExponentScaling
    constexpr int32_t kRiverFlowAggregationSlopeExponentMin = 1;   // * kRiverFlowAggregationSlopeExponentScaling
    constexpr int32_t kRiverFlowAggregationSlopeExponentMax = 600; // * kRiverFlowAggregationSlopeExponentScaling
    constexpr int32_t kRiverCatchmentThresholdMin = 4;
    constexpr int32_t kRiverCatchmentThresholdMax = 2 << 20;
    constexpr int32_t kRiverPruneLengthThresholdMin = 0;
    constexpr int32_t kRiverPruneLengthThresholdMax = 128;
    constexpr int32_t kRiverWidthMin = 1;
    constexpr int32_t kRiverWidthMax = 128;
    constexpr int32_t kRiverBreachLengthMin = 0;
    constexpr int32_t kRiverBreachLengthMax = 128;
    constexpr int32_t kRiverBreachDepthMin = 0;
    constexpr int32_t kRiverBreachDepthMax = 32;

    enum RiverFlag : uint8_t
    {
        river,
        filled,
        breached,
        source,
    };

    using RiverFlags = FlagHolder<uint8_t, RiverFlag>;

    struct RiverSettings
    {
        bool generate = false;
        NumericSetting<int32_t, kRiverCatchmentThresholdMin, kRiverCatchmentThresholdMax, 64> catchmentThreshold = 512;
        NumericSetting<int32_t, kRiverPruneLengthThresholdMin, kRiverPruneLengthThresholdMax> pruneThreshold = 64;
        NumericSetting<int32_t, kRiverBreachLengthMin, kRiverBreachLengthMax> breachMaxLength = 8;
        NumericSetting<int32_t, kRiverBreachDepthMin, kRiverBreachDepthMax> breachMaxDepth = 2;
        NumericSetting<int32_t, 0, 40> offMapCatchment = 15; // * kRiverOffMapCatchmentScaling
        NumericSetting<int32_t, kRiverGrowthExponentMin, kRiverGrowthExponentMax>
            riverGrowthExponent = 50; // * kRiverGrowthExponentScaling
        NumericSetting<int32_t, kRiverFlowAggregationSlopeExponentMin, kRiverFlowAggregationSlopeExponentMax>
            riverFlowAggregationSlopeExponent = 110;
        bool riverFlowAggregationFractional = true;
        NumericSetting<int32_t, kRiverWidthMin, kRiverWidthMax> riverWidthMax = 11;
    };

    struct RiverStatistics
    {
        int32_t pitsFound = 0;
        int32_t pitsBreachSuccess = 0;
        int32_t pitsBreachedTiles = 0;
        int32_t pitsFilledTiles = 0;

        float flowAggMaxCatchment = -std::numeric_limits<float>::infinity();

        int32_t pruneSourcesFound = 0;
        int32_t pruneSourcesTilesRemoved = 0;
        int32_t pruneSourcesRemaining = 0;
        float pruneSourcesLongest = 0;

        int32_t widthAdjustNewTiles = 0;

        int32_t ensureOrdinalNewTiles = 0;

        int32_t bankIndentationsAdjusted = 0;
        int32_t riverIndentationsAdjusted = 0;

        int32_t consistencySegmentsIterations = 1;
        int32_t consistencySegmentsRaised = 0;
        int32_t consistencySegmentsLowered = 0;
        int32_t consistencySegmentsRemoved = 0;
        int32_t consistencySegmentsRaisedMaxSize = 0;
        int32_t consistencySegmentsLoweredMaxSize = 0;
        int32_t consistencySegmentsRemovedMaxSize = 0;
        int32_t consistencyBanksRaised = 0;
    };

    struct RiverContext
    {
        MapDirectionMaskMap flowsIn;
        MapDirectionMaskMap flowsOut;
        BaseMap<float> catchment;
        BaseMap<float> waterLevel;
        BaseMap<RiverFlags> flags;

        RiverStatistics statistics;

        RiverContext(const TileCoordsXY& size)
            : flowsIn(size)
            , flowsOut(size)
            , catchment(size)
            , waterLevel(size)
            , flags(size)
        {
        }
    };

} // namespace OpenRCT2::World::MapGenerator::River
