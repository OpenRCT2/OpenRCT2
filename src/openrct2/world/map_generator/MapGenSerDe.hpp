/*****************************************************************************
 * Copyright (c) 2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../Version.h"
#include "../../core/FileSystem.hpp"
#include "../../platform/Platform.h"
#include "BaseMap.hpp"
#include "MapGen.h"
#include "SettingsTypesSerDe.hpp"
#include "river/RiverSerDe.hpp"
#include "rule/RuleSerDe.hpp"

#include <fstream>
#include <nlohmann/json.hpp>

namespace OpenRCT2::World::MapGenerator
{
    // could be constexpr+string_view but causes issues with certain compilers
    const std::string kMapGenSettingsTypeKey = "__type__";
    const std::string kMapGenSettingsVersionKey = "__version__";
    const std::string kMapGenSettingsFormatKey = "__format__";

    // increment for each breaking change
    constexpr size_t kMapGenSettingsFormat = 1;
    const std::string kMapGenSettingsTypeValue = std::string(OPENRCT2_NAME) + "::MapGeneratorSettings";

    NLOHMANN_JSON_SERIALIZE_ENUM( // TODO replace with NLOHMANN_JSON_SERIALIZE_ENUM_STRICT added in 3.13.0
        HeightMapGenerator,
        {
            { HeightMapGenerator::flat, "flat" },
            { HeightMapGenerator::noise, "noise" },
            { HeightMapGenerator::image, "image" },
        })

    NLOHMANN_JSON_SERIALIZE_ENUM( // TODO replace with NLOHMANN_JSON_SERIALIZE_ENUM_STRICT added in 3.13.0
        NoiseAlgorithm,
        {
            { NoiseAlgorithm::simplex, "simplex" },
            { NoiseAlgorithm::warped, "warped" },
            { NoiseAlgorithm::ridged, "ridged" },
            { NoiseAlgorithm::voronoi, "voronoi" },
        })

    NLOHMANN_JSON_SERIALIZE_ENUM( // TODO replace with NLOHMANN_JSON_SERIALIZE_ENUM_STRICT added in 3.13.0
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

    NLOHMANN_JSON_SERIALIZE_ENUM( // TODO replace with NLOHMANN_JSON_SERIALIZE_ENUM_STRICT added in 3.13.0
        Filter,
        {
            { Filter::none, "none" },
            { Filter::box, "box" },
            { Filter::gaussian, "gaussian" },
            { Filter::sharpen, "sharpen" },
            { Filter::bilateral, "bilateral" },
            { Filter::erosion, "erosion" },
        })

    NLOHMANN_JSON_SERIALIZE_ENUM( // TODO replace with NLOHMANN_JSON_SERIALIZE_ENUM_STRICT added in 3.13.0
        SlopeSmooth,
        {
            { SlopeSmooth::none, "none" },
            { SlopeSmooth::weak, "weak" },
            { SlopeSmooth::strong, "strong" },
        })

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MapSize, x, y)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(FilterSettings, type, strength)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BiasSettings, type, strength, steps)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(NoiseSettings, algorithm, baseFrequency, octaves, bias)

    // ================================================================================ Settings

    inline void to_json(nlohmann::json& j, const Settings& settings)
    {
        j[kMapGenSettingsTypeKey] = kMapGenSettingsTypeValue;
        j[kMapGenSettingsVersionKey] = kOpenRCT2Version;
        j[kMapGenSettingsFormatKey] = kMapGenSettingsFormat;

        j["generator"] = settings.generator;
        j["mapSize"] = settings.mapSize;
        j["seed"] = settings.seed;
        j["waterLevel"] = settings.waterLevel;
        j["heightmapLow"] = settings.heightmapLow;
        j["heightmapHigh"] = settings.heightmapHigh;
        j["textureRules"] = settings.textureRules;
        j["sceneryRules"] = settings.sceneryRules;
        j["noise"] = settings.noise;
        j["normalizeHeight"] = settings.normalizeHeight;
        j["filter"] = settings.filter;
        j["slopeSmooth"] = settings.slopeSmooth;
        j["river"] = settings.river;
    }

    inline void from_json(const nlohmann::json& j, Settings& settings)
    {
        auto typeValue = j.at(kMapGenSettingsTypeKey).get<std::string>();
        if (typeValue != kMapGenSettingsTypeValue)
        {
            throw SettingSerdeException(
                "Invalid ", kMapGenSettingsTypeKey, ", expected ", kMapGenSettingsTypeValue, " found ", typeValue);
        }

        auto formatValue = j.at(kMapGenSettingsFormatKey).get<size_t>();
        if (formatValue != kMapGenSettingsFormat)
        {
            throw SettingSerdeException(
                "Invalid ", kMapGenSettingsFormatKey, ", expected ", kMapGenSettingsFormat, " found ", typeValue);
        }

        j.at("generator").get_to(settings.generator);
        j.at("mapSize").get_to(settings.mapSize);
        j.at("seed").get_to(settings.seed);
        j.at("waterLevel").get_to(settings.waterLevel);
        j.at("heightmapLow").get_to(settings.heightmapLow);
        j.at("heightmapHigh").get_to(settings.heightmapHigh);
        j.at("textureRules").get_to(settings.textureRules);
        j.at("sceneryRules").get_to(settings.sceneryRules);
        j.at("noise").get_to(settings.noise);
        j.at("normalizeHeight").get_to(settings.normalizeHeight);
        j.at("filter").get_to(settings.filter);
        j.at("slopeSmooth").get_to(settings.slopeSmooth);
        j.at("river").get_to(settings.river);

        bool valid = false;
        for (size_t i = 0; i < settings.textureRules.size(); ++i)
        {
            const auto& rule = settings.textureRules[i];
            if (i == 0)
            {
                if (rule.isDefault && rule.effect.applyEdgeTexture && rule.effect.applyLandTexture)
                {
                    valid = true;
                }
            }
            else
            {
                if (rule.isDefault)
                {
                    valid = false;
                }
                for (auto& condition : rule.conditions)
                {
                    if (condition.type == Rule::Type::LandStyle)
                    {
                        valid = false;
                    }
                }
            }
        }

        if (!valid)
        {
            throw SettingSerdeException("Invalid texture rule list");
        }
    }
    // ================================================================================ Settings

    inline void saveMapgenSettingsToPath(const Settings& settings, const std::string& settingsJsonPath)
    {
        try
        {
            std::ofstream file(settingsJsonPath);
            const nlohmann::json settingsJson = settings;
            file << settingsJson.dump();
        }
        catch (nlohmann::json::exception& e)
        {
            throw SettingSerdeException("Json serialization failed ", e.what());
        }
    }

    inline Settings loadMapgenSettingsFromPath(const std::string& settingsJsonPath)
    {
        try
        {
            std::ifstream file(settingsJsonPath);
            const nlohmann::json data = nlohmann::json::parse(file);
            return data.get<Settings>();
        }
        catch (nlohmann::json::exception& e)
        {
            throw SettingSerdeException("Json deserialization failed ", e.what());
        }
    }
} // namespace OpenRCT2::World::MapGenerator
