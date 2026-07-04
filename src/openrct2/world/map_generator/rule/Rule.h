/*****************************************************************************
 * Copyright (c) 2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../../localisation/Formatting.h"
#include "../../../util/Hash.hpp"
#include "../../tile_element/SurfaceElement.h"
#include "../BaseMap.hpp"

#include <bitset>
#include <functional>
#include <optional>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace OpenRCT2::World::MapGenerator
{
    class Noise;
    struct Settings;
    struct MapGenContext;
} // namespace OpenRCT2::World::MapGenerator

namespace OpenRCT2::World::MapGenerator::Rule
{
    constexpr int32_t kHeightMin = 2;
    constexpr int32_t kHeightMax = 254;
    constexpr float kDistanceMin = 0.0f;
    constexpr float kDistanceMax = 1415.0f;
    constexpr float kNoiseMin = 0.0f;
    constexpr float kNoiseMax = 1.0f;
    constexpr float kRandomMin = 0.0f;
    constexpr float kRandomMax = 1.0f;
    constexpr float kAngleMin = 0.0f;
    constexpr float kAngleMax = 90.0f;
    constexpr float kFrequencyMin = 0.0f;
    constexpr float kFrequencyMax = 10.0f;

    constexpr int32_t kOctavesMin = 1;
    constexpr int32_t kOctavesMax = 10;
    constexpr int32_t kSeedOffsetMin = std::numeric_limits<int32_t>::min();
    constexpr int32_t kSeedOffsetMax = std::numeric_limits<int32_t>::max();

    // TODO just use ObjectType?
    enum RuleSceneryType : uint8_t
    {
        Small,
        Large,
        Wall,
    };

    inline ObjectType objectTypeOf(const RuleSceneryType type)
    {
        switch (type)
        {
            case Small:
                return ObjectType::smallScenery;
            case Large:
                return ObjectType::largeScenery;
            case Wall:
                return ObjectType::walls;
        }
        return ObjectType::none;
    }

    enum class TextureRulePreset : uint8_t
    {
        SmallRockPatches,
        MediumDirtPatches,
        LargeGrassClumpPatches,
        MountainPeaks
    };

    enum class SceneryRulePreset : uint8_t
    {
        Mediterranean,
        Cold,
        Arid,
        Rainforest,
    };

    struct EvaluationHeights
    {
        int32_t land;
        std::optional<int32_t> water;
    };

    struct LocalEvaluationHeights
    {
        int32_t seaLevel;
        EvaluationHeights self;
        std::optional<EvaluationHeights> neighbourNW;
        std::optional<EvaluationHeights> neighbourNE;
        std::optional<EvaluationHeights> neighbourSE;
        std::optional<EvaluationHeights> neighbourSW;
    };

    struct ConditionKey
    {
        int32_t ruleIdx;
        int32_t conditionIdx;
        bool operator==(const ConditionKey&) const = default;
    };

    struct ContextKeyHash
    {
        size_t operator()(const ConditionKey& s) const noexcept
        {
            size_t hash = 0;
            Util::Hash::update(hash, s.ruleIdx);
            Util::Hash::update(hash, s.conditionIdx);
            return hash;
        }
    };

    struct EvaluationContext
    {
        // per condition
        std::unordered_map<ConditionKey, std::unique_ptr<Noise>, ContextKeyHash> conditionNoiseFns;
        std::unordered_map<ConditionKey, std::mt19937, ContextKeyHash> conditionPrngs;

        // per rule
        std::unordered_map<int32_t, std::discrete_distribution<int32_t>> ruleItemDists;
        std::unordered_map<int32_t, std::mt19937> rulePrngs;

        // const
        std::uniform_real_distribution<float> prngDist{ 0.0f, 1.0f };
        std::uniform_int_distribution<int32_t> directionDist{ 0, 3 };
        std::uniform_int_distribution<int32_t> colourDist{ 0, Drawing::kColourNumTotal - 2 }; // no invis, void

        // global
        std::mt19937 quadPrng;
        NormalMap normalMap;
        DistanceMap distanceToWater;
        DistanceMap distanceToRiver;
        DistanceMap distanceToSea;
        DistanceMap distanceToLand;
        DistanceMap distanceToFill;
        DistanceMap distanceToBreach;
        DistanceMap distanceToBorder;

        // per tile
        ObjectEntryIndex landTexture;
        TileCoordsXY worldCoords;
        TileCoordsXY genCoords;
        VecXY quadCoords;
        LocalEvaluationHeights localHeights;

        // per rule per tile
        std::optional<int32_t> zRepeat = std::nullopt;
    };

    enum class Predicate : uint8_t
    {
        Equal,
        NotEqual,
        LessThan,
        GreaterThan,
        LessThanOrEqual,
        GreaterThanOrEqual,
    };

    enum class Feature : uint8_t
    {
        Water,
        River,
        Sea,
        MapBorder,
        Fill,
        Breach,
        Land,
        // TODO Spring?
        // TODO Waterfall (Downstream)?
        // Lake
    };

    enum class HeightMode : uint8_t
    {
        Absolute,
        Relative,
    };

    enum class HeightType : uint8_t
    {
        Land,
        Water,
        SeaLevel,
        // TODO Min
        // TODO Max
    };

    enum class HeightSource : uint8_t
    {
        Self,
        NeighbourNW,
        NeighbourNE,
        NeighbourSE,
        NeighbourSW,
    };

    enum class Type : uint16_t
    {
        Height,
        Distance,
        Noise,
        NormalAngle,
        Random,
        BlendHeight,
        BlendNoise,
        BlendDistance,
        LandStyle,
    };

    // $value $pred height{-relativeElevation}
    struct HeightData
    {
        int32_t height;
        HeightMode mode;
        HeightSource sourceFirst;
        HeightType typeFirst;
        HeightSource sourceSecond;
        HeightType typeSecond;
    };

    // smoothstep($low, $high, height) $pred $prng
    struct BlendHeightData
    {
        uint32_t seedOffset;
        int32_t edgeLow;
        int32_t edgeHigh;
    };

    // $value^2 $pred distance
    struct DistanceData
    {
        Feature feature;
        float distance;
    };

    // $value $pred noise(x,y)
    struct NoiseData
    {
        uint32_t seedOffset;
        float frequency;
        int32_t octaves;
        float value;
    };

    // smoothstep($low, $high, noise) $pred $prng
    struct BlendNoiseData
    {
        uint32_t seedOffset;
        float frequency;
        int32_t octaves;
        float edgeLow;
        float edgeHigh;
    };

    // smoothstep($low, $high, distance) $pred $prng
    struct BlendDistanceData
    {
        Feature feature;
        uint32_t seedOffset;
        float edgeLow;
        float edgeHigh;
    };

    // $value $pred angle between Vec(0,0,1) and normal(x,y)
    struct NormalAngleData
    {
        float angle;
    };

    //  $value $pred prng(0..100)
    struct RandomData
    {
        uint32_t seedOffset;
        float value;
    };

    struct LandStyleData
    {
        std::unordered_set<ObjectEntryIndex> styles;
    };

    using ConditionData = std::variant<
        HeightData, DistanceData, NoiseData, NormalAngleData, RandomData, BlendHeightData, BlendNoiseData, BlendDistanceData,
        LandStyleData>;

    struct Condition
    {
        bool enabled;
        Type type;
        Predicate predicate;
        ConditionData data;
        bool zRepeat = false;
    };

    struct TextureEffect
    {
        bool applyLandTexture;
        ObjectEntryIndex landTexture;
        bool applyEdgeTexture;
        ObjectEntryIndex edgeTexture;
    };

    struct SceneryEffectItem
    {
        RuleSceneryType type;
        ObjectEntryIndex index;
        int8_t weight = 1;
        std::optional<uint8_t> direction;
        std::array<Drawing::Colour, 3> colours;
    };

    struct SceneryEffect
    {
        std::vector<SceneryEffectItem> objects;
        std::uint32_t seedOffset;
    };

    struct TextureResult
    {
        std::optional<ObjectEntryIndex> landTexture;
        std::optional<ObjectEntryIndex> edgeTexture;
    };

    struct SceneryResultItem
    {
        RuleSceneryType type;
        ObjectEntryIndex index;
        uint8_t direction;
        std::array<Drawing::Colour, 3> colours;
        std::optional<uint8_t> zRepeat = std::nullopt;
    };

    using WallSceneryItems = std::array<std::optional<SceneryResultItem>, 4>;
    using QuadSceneryItems = std::array<std::optional<SceneryResultItem>, 4>;
    using TileSceneryItems = std::variant<SceneryResultItem, QuadSceneryItems>;

    struct SceneryResult
    {
        WallSceneryItems walls;
        TileSceneryItems items;
    };

    using MaybeSceneryResult = std::optional<SceneryResult>;

    struct TextureRule
    {
        bool enabled;
        bool isDefault;
        std::string name;
        std::vector<Condition> conditions;

        TextureEffect effect;
    };

    struct SceneryRule
    {
        bool enabled;
        std::string name;
        std::vector<Condition> conditions;

        bool zRepeat = false;
        SceneryEffect effect;
    };

    using TextureRuleList = std::vector<TextureRule>;
    using SceneryRuleList = std::vector<SceneryRule>;

    template<typename R>
    using Callback = std::function<void(const TileCoordsXY&, const R&)>;

    void evaluateTextureRules(const MapGenContext& genCtx, const Callback<TextureResult>& callback);
    void evaluateSceneryRules(const MapGenContext& genCtx, const Callback<MaybeSceneryResult>& callback);

    void createDefaultTextureRules(Settings& settings);
    void createNewTextureRule(Settings& settings);
    void createNewTextureRuleFromPreset(Settings& settings, TextureRulePreset preset);

    void createDefaultSceneryRules(Settings& settings);
    void createNewSceneryRule(Settings& settings);
    void createNewSceneryRuleFromPreset(Settings& settings, SceneryRulePreset preset);

    Condition createNewCondition(const Type& type);

} // namespace OpenRCT2::World::MapGenerator::Rule
