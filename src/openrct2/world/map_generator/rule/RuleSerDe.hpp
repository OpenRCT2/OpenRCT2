/*****************************************************************************
 * Copyright (c) 2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../../Context.h"
#include "../../../object/ObjectManager.h"
#include "../SettingsTypesSerDe.hpp"
#include "Rule.h"

#include <nlohmann/json.hpp>

namespace OpenRCT2::World::MapGenerator::Rule
{
    using json = nlohmann::json;
    NLOHMANN_JSON_SERIALIZE_ENUM( // TODO replace with NLOHMANN_JSON_SERIALIZE_ENUM_STRICT added in 3.13.0
        Type,
        {
            { Type::HeightAbsolute, "HeightAbsolute"},
            { Type::HeightRelative, "HeightRelative"},
            { Type::Distance, "Distance"},
            { Type::Noise, "Noise"},
            { Type::NormalAngle, "NormalAngle"},
            { Type::Random, "Random"},
            { Type::BlendHeight, "BlendHeight"},
            { Type::BlendNoise, "BlendNoise"},
            { Type::BlendDistance, "BlendDistance"},
            { Type::LandStyle, "LandStyle"},
        })

    NLOHMANN_JSON_SERIALIZE_ENUM( // TODO replace with NLOHMANN_JSON_SERIALIZE_ENUM_STRICT added in 3.13.0
        RuleSceneryType,
        {
            { RuleSceneryType::Small, "Small" },
            { RuleSceneryType::Large, "Large" },
            { RuleSceneryType::Wall, "Wall" },
        })

    NLOHMANN_JSON_SERIALIZE_ENUM( // TODO replace with NLOHMANN_JSON_SERIALIZE_ENUM_STRICT added in 3.13.0
        Predicate,
        {
            { Predicate::Equal, "Equal" },
            { Predicate::NotEqual, "NotEqual" },
            { Predicate::LessThan, "LessThan" },
            { Predicate::GreaterThan, "GreaterThan" },
            { Predicate::LessThanOrEqual, "LessThanOrEqual" },
            { Predicate::GreaterThanOrEqual, "GreaterThanOrEqual" },
        })

    NLOHMANN_JSON_SERIALIZE_ENUM( // TODO replace with NLOHMANN_JSON_SERIALIZE_ENUM_STRICT added in 3.13.0
        HeightSource,
        {
            {HeightSource::SelfLand, "SelfLand"},
            {HeightSource::SelfWater, "SelfWater"},
            {HeightSource::NeighbourNWLand, "NeighbourNWLand"},
            {HeightSource::NeighbourNWWater, "NeighbourNWWater"},
            {HeightSource::NeighbourNELand, "NeighbourNELand"},
            {HeightSource::NeighbourNEWater, "NeighbourNEWater"},
            {HeightSource::NeighbourSELand, "NeighbourSELand"},
            {HeightSource::NeighbourSEWater, "NeighbourSEWater"},
            {HeightSource::NeighbourSWLand, "NeighbourSWLand"},
            {HeightSource::NeighbourSWWater, "NeighbourSWWater"},
            {HeightSource::GlobalMin, "GlobalMin"},
            {HeightSource::GlobalMax, "GlobalMax"},
            {HeightSource::GlobalWaterLevel, "GlobalWaterLevel"},
        })

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(HeightAbsoluteData, height, source)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(HeightRelativeData, height, sourceFirst, sourceSecond)

    NLOHMANN_JSON_SERIALIZE_ENUM( // TODO replace with NLOHMANN_JSON_SERIALIZE_ENUM_STRICT added in 3.13.0
        Feature,
        {
            { Feature::Water, "Water" },
            { Feature::River, "River" },
            { Feature::Sea, "Sea" },
            { Feature::MapBorder, "MapBorder" },
            { Feature::Fill, "Fill" },
            { Feature::Breach, "Breach" },
            { Feature::Land, "Land" },
        })

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DistanceData, feature, distance)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(NoiseData, seedOffset, frequency, octaves, value)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(NormalAngleData, angle)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RandomData, seedOffset, value)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BlendHeightData, seedOffset, edgeLow, edgeHigh)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BlendNoiseData, seedOffset, frequency, octaves, edgeLow, edgeHigh)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BlendDistanceData, seedOffset, feature, edgeLow, edgeHigh)

    // ================================================================================ LandStyleData
    inline void to_json(nlohmann::json& j, const LandStyleData& landStyleData)
    {
        auto& objectManager = GetContext()->GetObjectManager();

        j["styles"] = nlohmann::json::array({});

        for (const ObjectEntryIndex idx : landStyleData.styles)
        {
            const auto obj = objectManager.GetLoadedObject<TerrainSurfaceObject>(idx);
            if (obj == nullptr)
            {
                throw SettingSerdeException("failed to serialize TerrainSurfaceObject {}", idx);
            }
            j["styles"].push_back(obj->GetIdentifier());
        }
    }

    inline void from_json(const nlohmann::json& j, LandStyleData& landStyleData)
    {
        auto& objectManager = GetContext()->GetObjectManager();

        auto& stylesJson = j.at("styles");
        std::unordered_set<ObjectEntryIndex> styles(stylesJson.size());

        for (auto& style : stylesJson)
        {
            auto identifier = style.get<std::string>();
            ObjectEntryIndex idx = objectManager.GetLoadedObjectEntryIndex(identifier);
            if (idx == kObjectEntryIndexNull)
            {
                throw SettingSerdeException("failed to deserialize TerrainSurfaceObject {}, not loaded?", identifier);
            }
            styles.insert(idx);
        }

        landStyleData.styles = styles;
    }
    // ================================================================================ LandStyleData

    // ================================================================================ Condition
    inline void to_json(json& j, const Condition& condition)
    {
        j = json{
            { "enabled", condition.enabled },
            { "type", condition.type },
            { "predicate", condition.predicate },
            { "zRepeat", condition.zRepeat },
        };

        switch (condition.type)
        {
            case Type::HeightAbsolute:
                j["data"] = std::get<HeightAbsoluteData>(condition.data);
                break;
            case Type::HeightRelative:
                j["data"] = std::get<HeightRelativeData>(condition.data);
                break;
            case Type::Distance:
                j["data"] = std::get<DistanceData>(condition.data);
                break;
            case Type::Noise:
                j["data"] = std::get<NoiseData>(condition.data);
                break;
            case Type::NormalAngle:
                j["data"] = std::get<NormalAngleData>(condition.data);
                break;
            case Type::Random:
                j["data"] = std::get<RandomData>(condition.data);
                break;
            case Type::BlendHeight:
                j["data"] = std::get<BlendHeightData>(condition.data);
                break;
            case Type::BlendNoise:
                j["data"] = std::get<BlendNoiseData>(condition.data);
                break;
            case Type::BlendDistance:
                j["data"] = std::get<BlendDistanceData>(condition.data);
                break;
            case Type::LandStyle:
                j["data"] = std::get<LandStyleData>(condition.data);
                break;
        }
    }

    inline void from_json(const json& j, Condition& condition)
    {
        j.at("enabled").get_to(condition.enabled);
        j.at("type").get_to(condition.type);
        j.at("predicate").get_to(condition.predicate);
        j.at("zRepeat").get_to(condition.zRepeat);

        switch (condition.type)
        {
            case Type::HeightAbsolute:
            {
                condition.data = j.at("data").get<HeightAbsoluteData>();
                break;
            }
            case Type::HeightRelative:
            {
                condition.data = j.at("data").get<HeightRelativeData>();
                break;
            }
            case Type::Distance:
            {
                condition.data = j.at("data").get<DistanceData>();
                break;
            }
            case Type::Noise:
            {
                condition.data = j.at("data").get<NoiseData>();
                break;
            }
            case Type::NormalAngle:
            {
                condition.data = j.at("data").get<NormalAngleData>();
                break;
            }
            case Type::Random:
            {
                condition.data = j.at("data").get<RandomData>();
                break;
            }
            case Type::BlendHeight:
            {
                condition.data = j.at("data").get<BlendHeightData>();
                break;
            }
            case Type::BlendNoise:
            {
                condition.data = j.at("data").get<BlendNoiseData>();
                break;
            }
            case Type::BlendDistance:
            {
                condition.data = j.at("data").get<BlendDistanceData>();
                break;
            }
            case Type::LandStyle:
            {
                condition.data = j.at("data").get<LandStyleData>();
                break;
            }
        }
    }
    // ================================================================================ Condition

    // ================================================================================ TextureEffect
    inline void to_json(nlohmann::json& j, const TextureEffect& textureEffect)
    {
        auto& objectManager = GetContext()->GetObjectManager();

        j["applyLandTexture"] = textureEffect.applyLandTexture;
        j["applyEdgeTexture"] = textureEffect.applyEdgeTexture;

        const auto* objLand = objectManager.GetLoadedObject<TerrainSurfaceObject>(textureEffect.landTexture);
        if (objLand == nullptr)
        {
            throw SettingSerdeException("failed to serialize TerrainSurfaceObject {}", textureEffect.landTexture);
        }
        j["landTexture"] = objLand->GetIdentifier();
        const auto* objEdge = objectManager.GetLoadedObject<TerrainEdgeObject>(textureEffect.edgeTexture);
        if (objEdge == nullptr)
        {
            throw SettingSerdeException("failed to serialize TerrainEdgeObject {}", textureEffect.edgeTexture);
        }
        j["edgeTexture"] = objEdge->GetIdentifier();
    }

    inline void from_json(const nlohmann::json& j, TextureEffect& textureEffect)
    {
        auto& objectManager = GetContext()->GetObjectManager();

        j.at("applyLandTexture").get_to(textureEffect.applyLandTexture);
        j.at("applyEdgeTexture").get_to(textureEffect.applyEdgeTexture);

        const auto landId = objectManager.GetLoadedObjectEntryIndex(j.at("landTexture").get<std::string>());
        const auto edgeId = objectManager.GetLoadedObjectEntryIndex(j.at("edgeTexture").get<std::string>());

        if (landId == kObjectEntryIndexNull)
        {
            throw SettingSerdeException("failed to deserialize TerrainSurfaceObject {}, not loaded?", landId);
        }
        textureEffect.landTexture = landId;

        if (edgeId == kObjectEntryIndexNull)
        {
            throw SettingSerdeException("failed to deserialize TerrainEdgeObject {}, not loaded?", edgeId);
        }
        textureEffect.edgeTexture = edgeId;
    }
    // ================================================================================ TextureEffect

    // ================================================================================ SceneryEffectItem

    inline void to_json(nlohmann::json& j, const SceneryEffectItem& sceneryEffectItem)
    {
        auto& objectManager = GetContext()->GetObjectManager();
        const auto* obj = objectManager.GetLoadedObject(objectTypeOf(sceneryEffectItem.type), sceneryEffectItem.index);

        j["type"] = sceneryEffectItem.type;
        if (obj == nullptr)
        {
            throw SettingSerdeException(
                "failed to serialize sceneryEffectItem {} {}", static_cast<int32_t>(sceneryEffectItem.type),
                sceneryEffectItem.index);
        }
        j["identifier"] = obj->GetIdentifier();
        j["weight"] = sceneryEffectItem.weight;
        if (sceneryEffectItem.direction.has_value())
        {
            j["direction"] = sceneryEffectItem.direction.value();
        }
        else
        {
            j["direction"] = nullptr;
        }
        j["colours"] = sceneryEffectItem.colours;
    }

    inline void from_json(const nlohmann::json& j, SceneryEffectItem& sceneryEffectItem)
    {
        auto& objectManager = GetContext()->GetObjectManager();

        auto identifier = j.at("identifier").get<std::string>();
        j.at("type").get_to(sceneryEffectItem.type);

        const auto index = objectManager.GetLoadedObjectEntryIndex(identifier);

        if (index == kObjectEntryIndexNull)
        {
            throw SettingSerdeException(
                "failed to deserialize sceneryEffectItem {} {}, not loaded?", static_cast<int32_t>(sceneryEffectItem.type),
                identifier);
        }
        sceneryEffectItem.index = index;

        j.at("weight").get_to(sceneryEffectItem.weight);
        if (j["direction"].is_null())
        {
            sceneryEffectItem.direction = std::nullopt;
        }
        else
        {
            sceneryEffectItem.direction = j.at("direction");
        }
        j.at("colours").get_to(sceneryEffectItem.colours);
    }
    // ================================================================================ SceneryEffectItem

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SceneryEffect, objects, seedOffset)

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TextureRule, enabled, isDefault, name, conditions, effect)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SceneryRule, enabled, name, conditions, effect)

} // namespace OpenRCT2::World::MapGenerator::Rule
