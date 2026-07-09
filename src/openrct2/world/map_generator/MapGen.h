/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../Version.h"
#include "../Location.hpp"
#include "BaseMap.hpp"
#include "river/RiverTypes.hpp"
#include "rule/Rule.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <vector>

namespace OpenRCT2::World::MapGenerator
{
    static const uint32_t DEFAULT_SEED = static_cast<uint32_t>(std::hash<std::string>{}(OPENRCT2_NAME));

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
        int32_t strength = 75;
        int32_t steps = 1;
    };

    struct NoiseSettings
    {
        NoiseAlgorithm algorithm = NoiseAlgorithm::simplex;
        int32_t baseFrequency = 175;
        int32_t octaves = 6;

        BiasSettings bias;
    };

    struct FilterSettings
    {
        Filter type = Filter::none;
        int32_t strength = 1;
    };

    struct Settings
    {
        // Base
        HeightMapGenerator generator = HeightMapGenerator::noise;
        TileCoordsXY mapSize{ 256, 256 };
        // TileCoordsXY mapSize{ 512, 512 };
        uint32_t seed = DEFAULT_SEED;
        int32_t waterLevel = 6;
        int32_t heightmapLow = 14;
        int32_t heightmapHigh = 60;

        NoiseSettings noise;

        bool normalizeHeight = true;

        FilterSettings filter;

        SlopeSmooth slopeSmooth = SlopeSmooth::weak;

        River::RiverSettings river;

        Rule::TextureRuleList textureRules;
        Rule::SceneryRuleList sceneryRules;
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
        TileCoordsXY dimensions;
        int32_t overscan;
        TileCoordsXY overscanOffset;
        HeightMap heightMap;
        std::optional<River::RiverContext> riverContext; // TODO get rid of the optional? keeps 11 bytes per tile
        std::vector<DebugSign> debugSigns;
    };

    void generate(const Settings& settings);
} // namespace OpenRCT2::World::MapGenerator
