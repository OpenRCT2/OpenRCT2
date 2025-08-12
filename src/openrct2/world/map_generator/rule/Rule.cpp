/*****************************************************************************
 * Copyright (c) 2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Rule.h"

#include "../../../Diagnostic.h"
#include "../../../GameState.h"
#include "../../../localisation/Formatting.h"
#include "../../../util/Util.h"
#include "../MapGen.h"
#include "../Noise.h"

#include <charconv>
#include <random>
#include <regex>

namespace OpenRCT2::World::MapGenerator::Rule
{
    constexpr float NOISE_SCALE = 1.0f / 256.0f;

    static float distanceToWater(const TileCoordsXY& coords, const float distance)
    {
        const auto& gameState = getGameState();

        const auto minX = std::max<int32_t>(1, coords.x - distance);
        const auto maxX = std::max<int32_t>(coords.x + distance, gameState.mapSize.x - 2);
        const auto minY = std::max<int32_t>(1, coords.y - distance);
        const auto maxY = std::max<int32_t>(coords.y + distance, gameState.mapSize.y - 2);

        auto minDistance = distance + 1;
        const auto distanceSquared = std::pow(distance, 2);

        for (int y = minY; y <= maxY; ++y)
        {
            for (int x = minX; x <= maxX; ++x)
            {
                const auto candidateDistanceSquared = std::pow(coords.x - x, 2) + std::pow(coords.y - y, 2);
                if (candidateDistanceSquared > distanceSquared)
                {
                    continue;
                }

                const auto* surfaceElement = MapGetSurfaceElementAt(TileCoordsXY{ x, y });
                if (surfaceElement == nullptr)
                {
                    continue;
                }

                if (surfaceElement->GetWaterHeight() > surfaceElement->GetBaseZ())
                {
                    minDistance = std::min<int32_t>(minDistance, std::sqrt(candidateDistanceSquared));
                }
            }
        }

        return minDistance;
    }

    template<typename T>
    static bool evaluatePredicate(T a, Predicate& predicate, T b)
    {
        switch (predicate)
        {
            case Predicate::equal:
            {
                return a == b;
            }
            case Predicate::notEqual:
            {
                return a != b;
            }
            case Predicate::lessThan:
            {
                return a < b;
            }
            case Predicate::greaterThan:
            {
                return a > b;
            }
            case Predicate::lessThanOrEqual:
            {
                return a <= b;
            }
            case Predicate::greaterThanOrEqual:
            {
                return a >= b;
            }
            default:
            {
                throw std::runtime_error("unknown predicate");
            }
        }
    }

    template<typename T>
    static std::optional<T> resultFromRulesAt(std::vector<Rule<T>>& rules, EvaluationContext& ctx)
    {
        int32_t conditionIdx = 0;
        for (int32_t r = static_cast<int32_t>(rules.size()) - 1; r >= 0; --r)
        {
            auto& rule = rules[r];
            bool applicable = rule.enabled;
            for (int32_t c = 0; c < static_cast<int32_t>(rule.conditions.size()); c++)
            {
                auto& condition = rule.conditions[c];

                if (applicable && condition.enabled)
                {
                    switch (condition.type)
                    {
                        case Type::HeightAbsolute:
                        {
                            auto heightCondition = std::get<HeightData>(condition.data).height;
                            auto heightActual = ctx.heights.tile;
                            applicable = evaluatePredicate(heightActual, condition.predicate, heightCondition);
                            break;
                        }
                        case Type::HeightRelativeToWater:
                        {
                            auto heightCondition = std::get<HeightData>(condition.data).height;
                            auto heightActual = ctx.heights.tile - ctx.heights.water;
                            applicable = evaluatePredicate(heightActual, condition.predicate, heightCondition);
                            break;
                        }
                        case Type::DistanceToWater:
                        {
                            auto distanceCondition = std::get<DistanceData>(condition.data).distance;
                            auto distanceActual = distanceToWater(ctx.coords, distanceCondition);
                            auto limit = static_cast<float>(distanceCondition);
                            applicable = evaluatePredicate(distanceActual, condition.predicate, limit);
                            break;
                        }
                        case Type::Noise:
                        {
                            auto noiseValue = (1.0f + ctx.noiseFns[conditionIdx]->Generate(VecXY{ ctx.coords.x, ctx.coords.y }))
                                / 2.0f;
                            auto noiseCondition = std::get<NoiseData>(condition.data).value;
                            applicable = evaluatePredicate(noiseValue, condition.predicate, noiseCondition);
                            break;
                        }
                        case Type::NormalAngle:
                        {
                            auto normal = ctx.normalMap[ctx.coords];
                            auto angle = VecXYZ{ 0.0f, 0.0f, 1.0f }.Angle(normal);
                            applicable = evaluatePredicate(
                                angle, condition.predicate, std::get<NormalAngleData>(condition.data).angle);
                            break;
                        }
                        case Type::Prng:
                        {
                            auto prngValue = ctx.dist(ctx.prngs[conditionIdx]);
                            applicable = evaluatePredicate(
                                prngValue, condition.predicate, std::get<PrngData>(condition.data).value);
                            break;
                        }
                        case Type::PrngNoiseBlend:
                        {
                            auto& noiseBlendData = std::get<BlendNoiseData>(condition.data);
                            auto noiseValue = (1.0f + ctx.noiseFns[conditionIdx]->Generate(VecXY{ ctx.coords.x, ctx.coords.y }))
                                / 2.0f;

                            auto noiseSs = Smoothstep(noiseBlendData.edgeLow, noiseBlendData.edgeHigh, noiseValue);

                            auto prngValue = ctx.dist(ctx.prngs[conditionIdx]);
                            applicable = evaluatePredicate(prngValue, condition.predicate, noiseSs);
                            break;
                        }
                        case Type::PrngHeightBlend:
                        {
                            auto& heightBlendData = std::get<BlendHeightData>(condition.data);

                            auto heightSs = Smoothstep(heightBlendData.edgeLow, heightBlendData.edgeHigh, ctx.heights.tile);

                            auto prngValue = ctx.dist(ctx.prngs[conditionIdx]);
                            applicable = evaluatePredicate(prngValue, condition.predicate, heightSs);
                            break;
                        }
                    }
                }

                conditionIdx++;
            }

            if (applicable)
            {
                return std::make_optional(rule.result);
            }
        }

        return std::nullopt;
    }

    template<typename T>
    void evaluateRules(Settings& settings, std::vector<Rule<T>>& rules, Callback<T> callback)
    {
        EvaluationContext ctx{};

        int32_t conditionIdx = 0;
        bool normalsNeeded = false;
        for (int32_t r = static_cast<int32_t>(rules.size()) - 1; r >= 0; --r)
        {
            auto& rule = rules[r];
            for (int32_t c = 0; c < static_cast<int32_t>(rule.conditions.size()); c++)
            {
                auto& condition = rule.conditions[c];

                if (condition.type == Type::Noise)
                {
                    auto& noiseData = std::get<NoiseData>(condition.data);
                    auto noise = std::make_unique<SimplexFbmNoise>(
                        settings.seed + noiseData.seedOffset, noiseData.frequency * NOISE_SCALE, noiseData.octaves, 2.0f, 0.5f);
                    ctx.noiseFns[conditionIdx] = std::move(noise);
                }
                else if (condition.type == Type::Prng)
                {
                    auto& prngData = std::get<PrngData>(condition.data);
                    std::mt19937 prng(settings.seed + prngData.seedOffset);
                    ctx.prngs[conditionIdx] = std::move(prng);
                }
                else if (condition.type == Type::NormalAngle)
                {
                    normalsNeeded = true;
                }
                else if (condition.type == Type::PrngNoiseBlend)
                {
                    auto& noiseBlendData = std::get<BlendNoiseData>(condition.data);
                    auto noise = std::make_unique<SimplexFbmNoise>(
                        settings.seed + noiseBlendData.seedOffset, noiseBlendData.frequency * NOISE_SCALE,
                        noiseBlendData.octaves, 2.0f, 0.5f);
                    ctx.noiseFns[conditionIdx] = std::move(noise);
                    // shouldn't cause artifacts to use the same seed for prng and noise?
                    std::mt19937 prng(settings.seed + noiseBlendData.seedOffset);
                    ctx.prngs[conditionIdx] = std::move(prng);
                }
                else if (condition.type == Type::PrngHeightBlend)
                {
                    auto& heightBlendData = std::get<BlendHeightData>(condition.data);
                    std::mt19937 prng(settings.seed + heightBlendData.seedOffset);
                    ctx.prngs[conditionIdx] = std::move(prng);
                }

                conditionIdx++;
            }
        }

        if (normalsNeeded)
        {
            // TODO compute normals
        }

        auto& gameState = getGameState();
        for (int32_t y = 1; y < gameState.mapSize.y - 1; y++)
        {
            for (int32_t x = 1; x < gameState.mapSize.x - 1; x++)
            {
                TileCoordsXY coords{ x, y };
                auto* surfaceElement = MapGetSurfaceElementAt(coords);
                if (surfaceElement == nullptr)
                {
                    continue;
                }

                ctx.heights = { .tile = surfaceElement->BaseHeight,
                                .min = settings.heightmapLow,
                                .max = settings.heightmapHigh,
                                .water = settings.waterLevel };
                ctx.coords = coords;

                auto effect = resultFromRulesAt<T>(rules, ctx);
                callback(coords, *surfaceElement, effect);
            }
        }
    }

    void createNewTextureRule(Settings& settings)
    {
        int32_t newRuleId = 1;

        std::regex newRuleRegex{ GetFmtStringById(STR_MAPGEN_RULE_TEMPLATE_NEW).WithoutFormatTokens() + "(\\d+)" };

        for (auto& rule : settings.textureRules)
        {
            std::smatch match;
            if (std::regex_match(rule.name, match, newRuleRegex))
            {
                int32_t ruleId = std::stoi(match[1].str());
                newRuleId = std::max(ruleId + 1, newRuleId);
            }
        }

        TextureRule newRule{ .enabled = true,
                             .isDefault = false,
                             .name = FormatStringID(STR_MAPGEN_RULE_TEMPLATE_NEW, newRuleId),
                             .conditions = std::vector<Condition>{},
                             .result = {
                                 .applyLandTexture = false, .landTexture = 0, .applyEdgeTexture = false, .edgeTexture = 0 } };

        settings.textureRules.push_back(std::move(newRule));
    }

    void createNewTextureRuleFromPreset(Settings& settings, int32_t presetId)
    {
        std::random_device prng{};

        switch (presetId)
        {
            case 0: // small rock patches
                settings.textureRules.push_back(
                    TextureRule{
                        .enabled = true,
                        .isDefault = false,
                        .name = FormatStringID(STR_MAPGEN_RULE_SMALL_ROCK_PATCHES),
                        .conditions = std::vector{ Condition{
                            .enabled = true,

                            .type = Type::PrngNoiseBlend,
                            .predicate = Predicate::lessThanOrEqual,
                            .data = BlendNoiseData{ .seedOffset = prng(),
                                                    .frequency = 3.75f,
                                                    .octaves = 8,
                                                    .edgeLow = 0.7f,
                                                    .edgeHigh = 0.9f },
                        } },
                        .result = {
                            .applyLandTexture = true, .landTexture = 10, .applyEdgeTexture = false, .edgeTexture = 0 } });
                break;
            case 1: // medium dirt patches
                settings.textureRules.push_back(
                    TextureRule{
                        .enabled = true,
                        .isDefault = false,
                        .name = FormatStringID(STR_MAPGEN_RULE_MEDIUM_DIRT_PATCHES),
                        .conditions = std::vector{ Condition{
                            .enabled = true,
                            .type = Type::PrngNoiseBlend,
                            .predicate = Predicate::lessThanOrEqual,
                            .data = BlendNoiseData{ .seedOffset = prng(),
                                                    .frequency = 2.75f,
                                                    .octaves = 6,
                                                    .edgeLow = 0.65f,
                                                    .edgeHigh = 0.85f },
                        } },
                        .result = {
                            .applyLandTexture = true, .landTexture = 2, .applyEdgeTexture = false, .edgeTexture = 0 } });
                break;
            case 2: // large grass clumps patches
                settings.textureRules.push_back(
                    TextureRule{
                        .enabled = true,
                        .isDefault = false,
                        .name = FormatStringID(STR_MAPGEN_RULE_LARGE_GRASS_CLUMP_PATCHES),
                        .conditions = std::vector{ Condition{ .enabled = true,
                                                              .type = Type::PrngNoiseBlend,
                                                              .predicate = Predicate::lessThanOrEqual,
                                                              .data = BlendNoiseData{ .seedOffset = prng(),
                                                                                      .frequency = 1.75f,
                                                                                      .octaves = 6,
                                                                                      .edgeLow = 0.5f,
                                                                                      .edgeHigh = 0.75f } } },
                        .result = {
                            .applyLandTexture = true, .landTexture = 3, .applyEdgeTexture = false, .edgeTexture = 0 } });
                break;
            case 3: // mountain peaks
                settings.textureRules.push_back(
                    TextureRule{
                        .enabled = true,
                        .isDefault = false,
                        .name = FormatStringID(STR_MAPGEN_RULE_ROCKY_MOUNTAIN_PEAKS),
                        .conditions = std::vector{ Condition{
                            .enabled = true,
                            .type = Type::PrngHeightBlend,
                            .predicate = Predicate::lessThanOrEqual,
                            .data = BlendHeightData{ .seedOffset = prng(), .edgeLow = 64, .edgeHigh = 72 } } },
                        .result = {
                            .applyLandTexture = true, .landTexture = 10, .applyEdgeTexture = false, .edgeTexture = 0 } });
                settings.textureRules.push_back(
                    TextureRule{
                        .enabled = true,
                        .isDefault = false,
                        .name = FormatStringID(STR_MAPGEN_RULE_SNOWY_MOUNTAIN_PEAKS),
                        .conditions = std::vector{ Condition{
                            .enabled = true,
                            .type = Type::PrngHeightBlend,
                            .predicate = Predicate::lessThanOrEqual,
                            .data = BlendHeightData{ .seedOffset = prng(), .edgeLow = 80, .edgeHigh = 86 } } },
                        .result = { .applyLandTexture = true, .landTexture = 8, .applyEdgeTexture = true, .edgeTexture = 1 } });
                break;
            default:
                LOG_WARNING("unknown presetId %i", presetId);
        }
    }

    void createNewSceneryRule(Settings& settings)
    {
        // TODO
    }

    void createNewSceneryRuleFromPreset(Settings& settings, int32_t presetId)
    {
        // TODO
    }

    Condition createNewCondition(Type& type)
    {
        switch (type)
        {
            case Type::HeightAbsolute:
                return Condition{
                    .enabled = true, .type = type, .predicate = Predicate::greaterThan, .data = HeightData{ .height = 2 }
                };
            case Type::HeightRelativeToWater:
                return Condition{
                    .enabled = true, .type = type, .predicate = Predicate::greaterThan, .data = HeightData{ .height = 2 }
                };
            case Type::DistanceToWater:
                return Condition{
                    .enabled = true, .type = type, .predicate = Predicate::lessThan, .data = DistanceData{ .distance = 2 }
                };
            case Type::Noise:
                return Condition{ .enabled = true,
                                  .type = type,
                                  .predicate = Predicate::greaterThan,
                                  .data = NoiseData{
                                      .seedOffset = std::random_device{}(), .frequency = 1.75f, .octaves = 6, .value = 0.50 } };
            case Type::NormalAngle:
                return Condition{ .enabled = true,
                                  .type = type,
                                  .predicate = Predicate::greaterThan,
                                  .data = NormalAngleData{ .angle = 15.0f } };
            case Type::Prng:
                return Condition{ .enabled = true,
                                  .type = type,
                                  .predicate = Predicate::greaterThan,
                                  .data = PrngData{ .seedOffset = std::random_device{}(), .value = .5f } };
            case Type::PrngHeightBlend:
                return Condition{ .enabled = true,
                                  .type = type,
                                  .predicate = Predicate::lessThanOrEqual,
                                  .data = BlendHeightData{
                                      .seedOffset = std::random_device{}(), .edgeLow = 32, .edgeHigh = 64 } };
            case Type::PrngNoiseBlend:
                return Condition{ .enabled = true,
                                  .type = type,
                                  .predicate = Predicate::lessThanOrEqual,
                                  .data = BlendNoiseData{ .seedOffset = std::random_device{}(),
                                                          .frequency = 1.75f,
                                                          .octaves = 6,
                                                          .edgeLow = 0.33f,
                                                          .edgeHigh = 0.66f } };
            default:
                throw std::invalid_argument("unknown condition type");
        }
    }

    template void evaluateRules<TextureResult>(Settings& settings, TextureRuleList& rules, Callback<TextureResult> callback);
    template void evaluateRules<SceneryResult>(Settings& settings, SceneryRuleList& rules, Callback<SceneryResult> callback);
} // namespace OpenRCT2::World::MapGenerator::Rule
