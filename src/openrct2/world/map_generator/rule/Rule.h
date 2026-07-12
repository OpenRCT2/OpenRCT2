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
#include "../Noise.h"
#include "../SettingsTypes.hpp"

#include <functional>
#include <optional>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace OpenRCT2::World::MapGenerator
{
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

    struct TileEvaluationHeights
    {
        int32_t land;
        std::optional<int32_t> water;
    };

    struct EvaluationHeights
    {
        int32_t globalMin;
        int32_t globalMax;
        int32_t globalWaterLevel;
        TileEvaluationHeights self;
        std::optional<TileEvaluationHeights> neighbourNW;
        std::optional<TileEvaluationHeights> neighbourNE;
        std::optional<TileEvaluationHeights> neighbourSE;
        std::optional<TileEvaluationHeights> neighbourSW;
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
        EvaluationHeights evaluationHeights;

        // per rule per tile
        std::optional<int32_t> zRepeat = std::nullopt;

        // make sure to update resetEvaluationContextRuleAndConditionState() when adding new state
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
    };

    enum class HeightSource : uint8_t
    {
        SelfLand,
        SelfWater,
        NeighbourNWLand,
        NeighbourNWWater,
        NeighbourNELand,
        NeighbourNEWater,
        NeighbourSELand,
        NeighbourSEWater,
        NeighbourSWLand,
        NeighbourSWWater,
        GlobalMin,
        GlobalMax,
        GlobalWaterLevel,
        // TODO Clearance?
    };

    enum class Type : uint16_t
    {
        HeightAbsolute,
        HeightRelative,
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
    struct HeightRelativeData
    {
        NumericSetting<int32_t, -kMaximumLandHeight, kMaximumLandHeight, 2> height;
        HeightSource sourceFirst;
        HeightSource sourceSecond;
    };

    struct HeightAbsoluteData
    {
        NumericSetting<int32_t, 0, kMaximumLandHeight, 2> height;
        HeightSource source;
    };

    // smoothstep($low, $high, height) $pred $prng
    struct BlendHeightData
    {
        NumericSetting<uint32_t, 0, std::numeric_limits<uint32_t>::max()> seedOffset;
        NumericSetting<int32_t, 0, kMaximumLandHeight, 2> edgeLow;
        NumericSetting<int32_t, 0, kMaximumLandHeight, 2> edgeHigh;
    };

    // $value^2 $pred distance
    struct DistanceData
    {
        Feature feature;
        NumericSetting<float, kDistanceMin, kDistanceMax, 1.0f> distance;
    };

    // $value $pred noise(x,y)
    struct NoiseData
    {
        NumericSetting<uint32_t, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max()> seedOffset;
        NumericSetting<float, kFrequencyMin, kFrequencyMax, 0.05f> frequency;
        NumericSetting<int32_t, kOctavesMin, kOctavesMax> octaves;
        NumericSetting<float, kNoiseMin, kNoiseMax, 0.01f> value;
    };

    // smoothstep($low, $high, noise) $pred $prng
    struct BlendNoiseData
    {
        NumericSetting<uint32_t, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max()> seedOffset;
        NumericSetting<float, kFrequencyMin, kFrequencyMax, 0.05f> frequency;
        NumericSetting<int32_t, kOctavesMin, kOctavesMax> octaves;
        NumericSetting<float, kNoiseMin, kNoiseMax, 0.01f> edgeLow;
        NumericSetting<float, kNoiseMin, kNoiseMax, 0.01f> edgeHigh;
    };

    // smoothstep($low, $high, distance) $pred $prng
    struct BlendDistanceData
    {
        Feature feature;
        NumericSetting<uint32_t, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max()> seedOffset;
        NumericSetting<float, kDistanceMin, kDistanceMax, 1.0f> edgeLow;
        NumericSetting<float, kDistanceMin, kDistanceMax, 1.0f> edgeHigh;
    };

    // $value $pred angle between Vec(0,0,1) and normal(x,y)
    struct NormalAngleData
    {
        NumericSetting<float, kAngleMin, kAngleMax, 1.0f> angle;
    };

    //  $value $pred prng(0..100)
    struct RandomData
    {
        NumericSetting<uint32_t, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max()> seedOffset;
        NumericSetting<float, kRandomMin, kRandomMax, 0.01f> value;
    };

    struct LandStyleData
    {
        std::unordered_set<ObjectEntryIndex> styles;
    };

    using ConditionData = std::variant<
        HeightAbsoluteData, HeightRelativeData, DistanceData, NoiseData, NormalAngleData, RandomData, BlendHeightData,
        BlendNoiseData, BlendDistanceData, LandStyleData>;

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
        std::optional<Direction> direction;
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

        SceneryEffect effect;
    };

    using TextureRuleList = std::vector<TextureRule>;
    using SceneryRuleList = std::vector<SceneryRule>;

    template<typename R>
    using Callback = std::function<void(const TileCoordsXY&, const R&)>;

    void initializeEvaluationContext(const MapGenContext& genCtx, EvaluationContext& evalCtx);
    void evaluateTextureRules(const MapGenContext& genCtx, EvaluationContext& evalCtx, const Callback<TextureResult>& callback);
    void evaluateSceneryRules(
        const MapGenContext& genCtx, EvaluationContext& evalCtx, const Callback<MaybeSceneryResult>& callback);
} // namespace OpenRCT2::World::MapGenerator::Rule
