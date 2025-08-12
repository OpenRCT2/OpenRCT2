/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../Version.h"
#include "../../localisation/Formatting.h"
#include "../../localisation/StringIds.h"
#include "../Location.hpp"
#include "BaseMap.hpp"
#include "rule/Rule.h"

#include <optional>
#include <vector>

namespace OpenRCT2::World::MapGenerator
{
    static const uint32_t DEFAULT_SEED = static_cast<uint32_t>(std::hash<std::string>{}(OPENRCT2_NAME));

    enum class Algorithm : uint8_t
    {
        blank,
        simplexNoise,
        warpedNoise,
        heightmapImage,
    };

    enum class Bias : uint8_t
    {
        none,
        island,
        valley,
        coastal,
        river,
        canyon,
        mountain,
        cliff
    };

    struct Settings
    {
        // Base
        Algorithm algorithm = Algorithm::blank;
        TileCoordsXY mapSize{ 150, 150 };
        uint32_t seed = DEFAULT_SEED;
        int32_t waterLevel = 6;
        int32_t heightmapLow = 14;
        int32_t heightmapHigh = 60;
        bool smoothTileEdges = true;

        std::vector<Rule::TextureRule> textureRules{
            Rule::TextureRule{
                .enabled = true,
                .isDefault = true,
                .name = FormatStringID(STR_MAPGEN_RULE_DEFAULT),
                .conditions = std::vector<Rule::Condition>{},
                .result{ .applyLandTexture = true, .landTexture = 0, .applyEdgeTexture = true, .edgeTexture = 0 } },
            Rule::TextureRule{
                .enabled = true,
                .isDefault = false,
                .name = FormatStringID(STR_MAPGEN_RULE_BEACHES_WATER_BODIES),
                .conditions = std::vector{ Rule::Condition{ .enabled = true,
                                                            .type = Rule::Type::HeightRelativeToWater,
                                                            .predicate = Rule::Predicate::lessThanOrEqual,
                                                            .data = Rule::HeightData{ 2 } },
                                           Rule::Condition{ .enabled = false,
                                                            .type = Rule::Type::DistanceToWater,
                                                            .predicate = Rule::Predicate::lessThanOrEqual,
                                                            .data = Rule::DistanceData{ 4 } } },
                .result = { .applyLandTexture = true, .landTexture = 11, .applyEdgeTexture = false, .edgeTexture = 0 } }
        };
        std::vector<Rule::SceneryRule> sceneryRules;

        // Features (e.g. tree, rivers, lakes etc.)
        bool trees = true;
        int32_t treeToLandRatio = 25;
        int32_t minTreeAltitude = 10;
        int32_t maxTreeAltitude = 50;

        // Simplex Noise Parameters
        int32_t simplex_base_freq = 175;
        int32_t simplex_octaves = 6;

        // Bias settings
        Bias bias = Bias::none;
        int32_t bias_strength = 75;

        // Height map settings
        bool smooth_height_map = true;
        uint32_t smooth_strength = 1;
        bool normalize_height = true;

        // Erosion settings
        bool simulate_erosion = false;
        int32_t particles_per_tile = 300;
    };

    void generate(Settings* settings);
    void resetSurfaces(Settings* settings);
    void setWaterLevel(int32_t waterLevel);
    void setMapHeight(Settings* settings, const HeightMap& heightMap);

} // namespace OpenRCT2::World::MapGenerator
