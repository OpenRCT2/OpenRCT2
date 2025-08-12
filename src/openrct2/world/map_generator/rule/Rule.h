/*****************************************************************************
 * Copyright (c) 2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../tile_element/SurfaceElement.h"
#include "../BaseMap.hpp"

#include <functional>
#include <optional>
#include <random>
#include <unordered_map>
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

    struct EvaluationHeights
    {
        int32_t tile;
        int32_t min;
        int32_t max;
        int32_t water;
    };

    struct EvaluationContext
    {
        TileCoordsXY coords;
        std::unordered_map<int32_t, std::unique_ptr<Noise>> noiseFns;
        std::unordered_map<int32_t, std::mt19937> prngs;
        std::uniform_real_distribution<float> dist{ 0.0f, 1.0f };
        NormalMap normalMap;
        EvaluationHeights heights;
    };

    enum class Predicate : uint8_t
    {
        equal,
        notEqual,
        lessThan,
        greaterThan,
        lessThanOrEqual,
        greaterThanOrEqual,
    };

    enum class Type : uint8_t
    {
        HeightAbsolute,
        HeightRelativeToWater,
        DistanceToWater,
        Noise,
        NormalAngle,
        Prng,
        PrngHeightBlend,
        PrngNoiseBlend,
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
    struct PrngData
    {
        uint32_t seedOffset;
        float value;
    };

    using ConditionData = std::variant<
        HeightData, DistanceData, NoiseData, NormalAngleData, PrngData, BlendHeightData, BlendNoiseData>;
    struct Condition
    {
        bool enabled;
        Type type;
        Predicate predicate;
        ConditionData data;
    };

    template<typename T>
    struct Rule
    {
        bool enabled;
        bool isDefault;
        std::string name;
        std::vector<Condition> conditions;

        T result;
    };

    struct TextureResult
    {
        bool applyLandTexture;
        ObjectEntryIndex landTexture;
        bool applyEdgeTexture;
        ObjectEntryIndex edgeTexture;
    };

    struct SceneryResult
    {
        // TODO
    };

    using TextureRule = Rule<TextureResult>;
    using SceneryRule = Rule<SceneryResult>;

    using TextureRuleList = std::vector<TextureRule>;
    using SceneryRuleList = std::vector<SceneryRule>;

    template<typename T>
    using Callback = std::function<void(TileCoordsXY&, SurfaceElement&, std::optional<T>&)>;

    template<typename T>
    void evaluateRules(Settings& settings, std::vector<Rule<T>>& rules, Callback<T> callback);

    void createNewTextureRule(Settings& settings);
    void createNewTextureRuleFromPreset(Settings& settings, int32_t presetId);
    void createNewSceneryRule(Settings& settings);
    void createNewSceneryRuleFromPreset(Settings& settings, int32_t presetId);

    Condition createNewCondition(Type& type);

} // namespace OpenRCT2::World::MapGenerator::Rule
