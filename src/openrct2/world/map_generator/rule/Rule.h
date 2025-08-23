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
} // namespace OpenRCT2::World::MapGenerator

namespace OpenRCT2::World::MapGenerator::Rule
{
    constexpr int32_t kHeightMin = 2;
    constexpr int32_t kHeightMax = 254;
    constexpr int32_t kDistanceMin = 0;
    constexpr int32_t kDistanceMax = 1415; // ceil(SQRT2 * 1000)
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
        int32_t tile;
        int32_t min;
        int32_t max;
        int32_t water;
    };

    struct ConditionKey
    {
        int32_t ruleIdx;
        int32_t conditionIdx;
        bool operator==(const ConditionKey&) const = default;
    };

    struct ContextKeyHash
    {
        std::size_t operator()(const ConditionKey& s) const noexcept
        {
            // TODO bit of a hack...
            std::size_t hashRuleIdx = std::hash<std::string>{}(FormatString("r{INT32}", s.ruleIdx));
            std::size_t hashConditionIdx = std::hash<std::string>{}(FormatString("c{INT32}", s.conditionIdx));
            return hashRuleIdx ^ (hashConditionIdx << 1);
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

        // global
        std::uniform_real_distribution<float> prngDist{ 0.0f, 1.0f };
        std::uniform_int_distribution<int32_t> directionDist{ 0, 3 };
        std::uniform_int_distribution<int32_t> colourDist{ 0, COLOUR_COUNT - 2 }; // no invis, void
        NormalMap normalMap;
        std::mt19937 quadPrng;

        // per tile
        ObjectEntryIndex landTexture;
        TileCoordsXY coords;
        VecXY quadCoords;
        EvaluationHeights heights;
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

    enum class Type : uint8_t
    {
        HeightAbsolute,
        HeightRelativeToWater,
        DistanceToWater,
        Noise,
        NormalAngle,
        Random,
        BlendHeight,
        BlendNoise,
        LandStyle,
    };

    // $value $pred height{-relativeElevation}
    struct HeightData
    {
        int32_t height;
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
        int32_t distance;
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
        HeightData, DistanceData, NoiseData, NormalAngleData, RandomData, BlendHeightData, BlendNoiseData, LandStyleData>;

    struct Condition
    {
        bool enabled;
        Type type;
        Predicate predicate;
        ConditionData data;
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
        ObjectEntryIndex index;
        int8_t weight = 1;
        std::optional<uint8_t> direction;
        std::array<colour_t, 3> colours;
    };

    struct SceneryEffect
    {
        std::vector<SceneryEffectItem> objects;
        std::uint32_t seedOffset;
    };

    using TextureResult = TextureEffect;

    struct SceneryResultItem
    {
        ObjectEntryIndex index;
        uint8_t direction;
        std::array<colour_t, 3> colours;
    };

    using QuadSceneryItems = std::array<std::optional<SceneryResultItem>, 4>;
    using SceneryResult = std::variant<SceneryResultItem, QuadSceneryItems>;

    struct TextureRule
    {
        // TODO extract base rule struct
        bool enabled;
        bool isDefault;
        std::string name;
        std::vector<Condition> conditions;

        TextureEffect effect;
    };

    struct SceneryRule
    {
        // TODO extract base rule struct
        bool enabled;
        std::string name;
        std::vector<Condition> conditions;

        SceneryEffect effect;
    };

    using TextureRuleList = std::vector<TextureRule>;
    using SceneryRuleList = std::vector<SceneryRule>;

    template<typename R>
    using Callback = std::function<void(const TileCoordsXY&, const std::optional<R>&)>;

    void evaluateTextureRules(const Settings& settings, const Callback<TextureResult>& callback);
    void evaluateSceneryRules(const Settings& settings, const Callback<SceneryResult>& callback);

    void createDefaultTextureRules(Settings& settings);
    void createNewTextureRule(Settings& settings);
    void createNewTextureRuleFromPreset(Settings& settings, TextureRulePreset preset);

    void createDefaultSceneryRules(Settings& settings);
    void createNewSceneryRule(Settings& settings);
    void createNewSceneryRuleFromPreset(Settings& settings, SceneryRulePreset preset);

    Condition createNewCondition(const Type& type);

} // namespace OpenRCT2::World::MapGenerator::Rule
