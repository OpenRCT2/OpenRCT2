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

    enum class SmoothFilter : uint8_t
    {
        none,
        box,
        gaussian,
        bilateral
    };

    enum class SlopeFunction : uint8_t
    {
        none,
        weak,
        strong
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

        Rule::TextureRuleList textureRules{};
        Rule::SceneryRuleList sceneryRules{};

        // Noise Parameters
        int32_t noiseBaseFreq = 175;
        int32_t noiseOctaves = 6;

        // Bias settings
        Bias bias = Bias::none;
        int32_t biasStrength = 75;

        // Height map settings
        bool normalizeHeight = true;

        // Erosion settings
        bool simulateErosion = false;
        int32_t particlesPerTile = 300;

        // Smooth settings
        SmoothFilter smoothFilter = SmoothFilter::none;
        uint32_t smoothStrength = 1;
        SlopeFunction slopeFunction = SlopeFunction::weak;
    };

    void generate(Settings* settings);
    void resetSurfaces(Settings* settings);
    void setWaterLevel(int32_t waterLevel);
    void setMapHeight(Settings* settings, const HeightMap& heightMap);

} // namespace OpenRCT2::World::MapGenerator
