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
#include "river/RiverSerDe.hpp"
#include "rule/RuleSerDe.hpp"

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TileCoordsXY, x, y)

namespace OpenRCT2::World::MapGenerator
{
    NLOHMANN_JSON_SERIALIZE_ENUM(
        HeightMapGenerator,
        {
            { HeightMapGenerator::flat, "flat" },
            { HeightMapGenerator::noise, "noise" },
            { HeightMapGenerator::image, "image" },
        })

    NLOHMANN_JSON_SERIALIZE_ENUM(
        NoiseAlgorithm,
        {
            { NoiseAlgorithm::simplex, "simplex" },
            { NoiseAlgorithm::warped, "warped" },
            { NoiseAlgorithm::ridged, "ridged" },
            { NoiseAlgorithm::voronoi, "voronoi" },
        })

    NLOHMANN_JSON_SERIALIZE_ENUM(
        Bias,
        {
            { Bias::none, "none" },
            { Bias::island, "island" },
            { Bias::valley, "valley" },
            { Bias::coastal, "coastal" },
            { Bias::river, "river" },
            { Bias::canyon, "canyon" },
            { Bias::mountain, "mountain" },
            { Bias::cliff, "cliff" },
            { Bias::terrace, "terrace" },
        })

    NLOHMANN_JSON_SERIALIZE_ENUM(
        Filter,
        {
            { Filter::none, "none" },
            { Filter::box, "box" },
            { Filter::gaussian, "gaussian" },
            { Filter::sharpen, "sharpen" },
            { Filter::bilateral, "bilateral" },
            { Filter::erosion, "erosion" },
        })

    NLOHMANN_JSON_SERIALIZE_ENUM(
        SlopeSmooth,
        {
            { SlopeSmooth::none, "none" },
            { SlopeSmooth::weak, "weak" },
            { SlopeSmooth::strong, "strong" },
        })

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(FilterSettings, type, strength)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BiasSettings, type, strength, steps)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(NoiseSettings, algorithm, baseFrequency, octaves, bias)

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        Settings, generator, mapSize, seed, waterLevel, heightmapLow, heightmapHigh, textureRules, sceneryRules,
        noise, normalizeHeight, filter, slopeSmooth, river)

    inline void saveMapgenSettingsToPath(const Settings& settings, const std::string& settingsJsonPath)
    {
        std::ofstream file(settingsJsonPath);
        const nlohmann::json settingsJson = settings;
        file << settingsJson.dump();
    }

    inline Settings loadMapgenSettingsFromPath(const std::string& settingsJsonPath)
    {
        std::ifstream file(settingsJsonPath);
        const nlohmann::json data = nlohmann::json::parse(file);
        return data.get<Settings>();
    }
} // namespace OpenRCT2::World::MapGenerator
