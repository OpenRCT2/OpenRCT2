/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Location.hpp"
#include "BaseMap.hpp"
#include "SettingsTypes.hpp"
#include "river/RiverTypes.hpp"
#include "rule/Rule.h"

#include <optional>
#include <vector>

namespace OpenRCT2::World::MapGenerator
{
    static inline std::string randomSeed()
    {
        return std::to_string(std::random_device{}());
    }

    enum class HeightMapGenerator : uint8_t
    {
        flat,
        noise,
        image
    };

    enum class NoiseAlgorithm : uint8_t
    {
        simplex,
        warped,
        ridged,
        voronoi,
    };

    enum class Bias : uint8_t
    {
        none,
        island,
        valley,
        coastal,
        river, // TODO should probably rename/remove it
        canyon,
        mountain,
        cliff,
        terrace
    };

    enum class Filter : uint8_t
    {
        none,
        box,
        gaussian,
        sharpen,
        bilateral,
        erosion
    };

    enum class SlopeSmooth : uint8_t
    {
        none,
        weak,
        strong
    };

    struct BiasSettings
    {
        Bias type = Bias::none;
        NumericSetting<int32_t, 0, 100, 5> strength = 75;
        NumericSetting<int32_t, 0, 10> steps = 1;
    };

    struct NoiseSettings
    {
        NoiseAlgorithm algorithm = NoiseAlgorithm::simplex;
        NumericSetting<int32_t, 0, 1000, 5> baseFrequency = 175;
        NumericSetting<int32_t, 1, 10> octaves = 6;

        BiasSettings bias{};
    };

    struct FilterSettings
    {
        Filter type = Filter::none;
        NumericSetting<int32_t, 0, 10> strength = 1;
    };

    struct MapSize
    {
        NumericSetting<int32_t, kMinimumMapSizeTechnical, kMaximumMapSizeTechnical> x = 150;
        NumericSetting<int32_t, kMinimumMapSizeTechnical, kMaximumMapSizeTechnical> y = 150;
    };

    struct Settings
    {
        HeightMapGenerator generator = HeightMapGenerator::flat;
        MapSize mapSize{};

        std::string seed = randomSeed();

        NumericSetting<int32_t, kMinimumWaterHeight, kMaximumWaterHeight - 1, 2> waterLevel = 6;
        NumericSetting<int32_t, kMinimumLandHeight, kMaximumLandHeight - 1, 2> heightmapLow = 14;
        NumericSetting<int32_t, kMinimumLandHeight, kMaximumLandHeight - 1, 2> heightmapHigh = 60;

        NoiseSettings noise{};

        bool normalizeHeight = true;

        FilterSettings filter{};

        SlopeSmooth slopeSmooth = SlopeSmooth::weak;

        River::RiverSettings river{};

        Rule::TextureRuleList textureRules{};
        Rule::SceneryRuleList sceneryRules{};
    };

    struct DebugSign
    {
        TileCoordsXY position;
        std::string text;
        Drawing::Colour textColour = Drawing::Colour::white;
        Drawing::Colour backgroundColour = Drawing::Colour::brightRed;
    };

    struct MapGenContext
    {
        const Settings& settings;
        uint32_t seed; // the technical seed used duration map generation, derived from settings.seed
        TileCoordsXY dimensions;
        int32_t overscan;
        TileCoordsXY overscanOffset;
        HeightMap heightMap;
        std::optional<River::RiverContext> riverContext; // TODO get rid of the optional? keeps 11 bytes per tile
    };

    void resetMapGenSettings();
    Settings& getMapGenSettings();

    void setRandomSeed(Settings& settings);
    void generate(const Settings& settings);
} // namespace OpenRCT2::World::MapGenerator
