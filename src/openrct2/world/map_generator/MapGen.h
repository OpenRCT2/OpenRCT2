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
#include "hydro/HydroTypes.hpp"
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
        ridgedNoise,
        voronoiNoise,
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
        cliff,
        terrace
    };

    enum class HeightMapTransform : uint8_t
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

    struct Settings
    {
        // Base
        Algorithm algorithm = Algorithm::blank;
        TileCoordsXY mapSize{ 150, 150 };
        uint32_t seed = DEFAULT_SEED;
        int32_t waterLevel = 6;
        int32_t heightmapLow = 14;
        int32_t heightmapHigh = 60;

        Rule::TextureRuleList textureRules;
        Rule::SceneryRuleList sceneryRules;

        // Noise Parameters
        int32_t noiseBaseFreq = 175;
        int32_t noiseOctaves = 6;

        // Bias settings
        Bias bias = Bias::none;
        int32_t biasStrength = 75;
        int32_t biasSteps = 1;

        // Height map settings
        bool normalizeHeight = true;

        // Transform settings
        HeightMapTransform heightmapTransform = HeightMapTransform::none;
        int32_t transformStrength = 1;
        SlopeSmooth slopeSmooth = SlopeSmooth::weak;

        // River settings
        bool generateRivers = true;
        int32_t catchmentThreshold = 384;
        int32_t pruneThreshold = 24;
        int32_t breachMaxLength = 8;
        int32_t breachMaxDepth = 2;
        int32_t offMapCatchmentMultiplier = 2;
        int32_t riverGrowthExponent = 50; // * kRiverGrowthExponentScaling
        int32_t riverWidthMax = 16;
    };

    struct DebugSign
    {
        TileCoordsXY position;
        std::string text;
        Drawing::Colour textColour = Drawing::Colour::white;
        Drawing::Colour backgroundColour = Drawing::Colour::brightRed;
    };

    struct MapGenCtx
    {
        const Settings& settings;
        HeightMap heightMap;
        std::optional<Hydro::HydroMaps> hydroMaps;
        std::vector<DebugSign> debugSigns;
    };

    void generate(Settings& settings);
    void resetSurfaces(const MapGenCtx& context);
    void setWaterLevel(const MapGenCtx& context);
    void setRiverWater(const MapGenCtx& context);
    void setMapHeight(const MapGenCtx& context);

} // namespace OpenRCT2::World::MapGenerator
