/*****************************************************************************
 * Copyright (c) 2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Rule.h"

#include "../../../Context.h"
#include "../../../GameState.h"
#include "../../../object/ObjectEntryManager.h"
#include "../../../object/SmallSceneryEntry.h"
#include "../../../util/Util.h"
#include "../../Map.h"
#include "../MapGen.h"
#include "../MapHelpers.h"
#include "../MapTraversalUtils.h"
#include "../Noise.h"
#include "../TileQueue.hpp"

#include <random>
#include <ranges>
#include <regex>

namespace OpenRCT2::World::MapGenerator::Rule
{
    constexpr float kNoiseScale = 1.0f / 256.0f;

    constexpr VecXY kQuadOffset[] = {
        VecXY{ 0.0f, 0.0f },
        VecXY{ 0.0f, 0.5f },
        VecXY{ 0.5f, 0.5f },
        VecXY{ 0.5f, 0.0f },
    };

    static void computeNormalMap(const MapGenContext& genCtx, NormalMap& normalMap)
    {
        // TODO actually compute the normal
        normalMap = NormalMap{ genCtx.dimensions };
        normalMap.fill({ 0.0f, 0.0f, 1.0f });
    }

    static void computeLandDistanceMap(const MapGenContext& genCtx, EvaluationContext& evalCtx)
    {
        evalCtx.distanceToLand = DistanceMap{ genCtx.dimensions };
        evalCtx.distanceToLand.fill(std::numeric_limits<float>::infinity());
        TrackingStableTileQueue queue{ genCtx.dimensions };

        for (int32_t y = 0; y < genCtx.dimensions.y; y++)
        {
            for (int32_t x = 0; x < genCtx.dimensions.x; x++)
            {
                const TileCoordsXY pos{ x, y };

                bool isRiver = genCtx.riverContext.has_value() && genCtx.riverContext.value().flags[pos].has(River::river);
                bool isSea = quantizeHeight(genCtx.heightMap[pos]) < genCtx.settings.waterLevel;

                if (!isSea && !isRiver)
                {
                    initZeroDistance(pos, evalCtx.distanceToLand, queue);
                }
            }
        }

        completeDistanceMap(evalCtx.distanceToLand, queue);
    }

    static void computeWaterDistanceMap(const MapGenContext& genCtx, EvaluationContext& evalCtx)
    {
        evalCtx.distanceToWater = DistanceMap{ genCtx.dimensions };
        evalCtx.distanceToWater.fill(std::numeric_limits<float>::infinity());
        TrackingStableTileQueue queue{ genCtx.dimensions };

        for (int32_t y = 0; y < genCtx.dimensions.y; y++)
        {
            for (int32_t x = 0; x < genCtx.dimensions.x; x++)
            {
                const TileCoordsXY pos{ x, y };

                bool isRiver = genCtx.riverContext.has_value() && genCtx.riverContext.value().flags[pos].has(River::river);
                bool isSea = quantizeHeight(genCtx.heightMap[pos]) < genCtx.settings.waterLevel;

                if (isSea || isRiver)
                {
                    initZeroDistance(pos, evalCtx.distanceToWater, queue);
                }
            }
        }

        completeDistanceMap(evalCtx.distanceToWater, queue);
    }

    static void computeSeaDistanceMap(const MapGenContext& genCtx, EvaluationContext& evalCtx)
    {
        evalCtx.distanceToSea = DistanceMap{ genCtx.dimensions };
        evalCtx.distanceToSea.fill(std::numeric_limits<float>::infinity());
        TrackingStableTileQueue queue{ genCtx.dimensions };

        for (int32_t y = 0; y < genCtx.dimensions.y; y++)
        {
            for (int32_t x = 0; x < genCtx.dimensions.x; x++)
            {
                const TileCoordsXY pos{ x, y };

                bool isRiver = genCtx.riverContext.has_value() && genCtx.riverContext.value().flags[pos].has(River::river);
                bool isSea = quantizeHeight(genCtx.heightMap[pos]) < genCtx.settings.waterLevel;

                if (isSea && !isRiver)
                {
                    initZeroDistance(pos, evalCtx.distanceToSea, queue);
                }
            }
        }

        completeDistanceMap(evalCtx.distanceToSea, queue);
    }

    static void computeBorderDistanceMap(const MapGenContext& genCtx, EvaluationContext& evalCtx)
    {
        evalCtx.distanceToBorder = DistanceMap{ genCtx.dimensions };
        evalCtx.distanceToBorder.fill(std::numeric_limits<float>::infinity());
        TrackingStableTileQueue queue{ genCtx.dimensions };

        for (int32_t y = 0; y < genCtx.dimensions.y; y++)
        {
            for (int32_t x = 0; x < genCtx.dimensions.x; x++)
            {
                const TileCoordsXY pos{ x, y };

                if (!isInWorldMap(genCtx, pos))
                {
                    initZeroDistance(pos, evalCtx.distanceToSea, queue);
                }
            }
        }

        completeDistanceMap(evalCtx.distanceToBorder, queue);
    }

    template<typename T>
    static std::optional<int32_t> evaluatePredicate(const T a, const Predicate& predicate, const T b)
    {
        bool result;
        switch (predicate)
        {
            case Predicate::Equal:
            {
                result = a == b;
                break;
            }
            case Predicate::NotEqual:
            {
                result = a != b;
                break;
            }
            case Predicate::LessThan:
            {
                result = a < b;
                break;
            }
            case Predicate::GreaterThan:
            {
                result = a > b;
                break;
            }
            case Predicate::LessThanOrEqual:
            {
                result = a <= b;
                break;
            }
            case Predicate::GreaterThanOrEqual:
            {
                result = a >= b;
                break;
            }
            default:
            {
                throw std::invalid_argument("unknown predicate");
            }
        }
        return result ? std::make_optional(static_cast<int32_t>(a)) : std::nullopt;
    }

    static float lookupDistanceTo(const EvaluationContext& ctx, const Feature& feature)
    {
        float distanceActual = std::numeric_limits<float>::infinity();
        switch (feature)
        {
            case Feature::MapBorder:
                distanceActual = ctx.distanceToBorder[ctx.genCoords];
                break;
            case Feature::Water:
                distanceActual = ctx.distanceToWater[ctx.genCoords];
                break;
            case Feature::River:
                distanceActual = ctx.distanceToRiver[ctx.genCoords];
                break;
            case Feature::Sea:
                distanceActual = ctx.distanceToSea[ctx.genCoords];
                break;
            case Feature::Land:
                distanceActual = ctx.distanceToLand[ctx.genCoords];
                break;
            case Feature::Fill:
                distanceActual = ctx.distanceToFill[ctx.genCoords];
                break;
            case Feature::Breach:
                distanceActual = ctx.distanceToBreach[ctx.genCoords];
                break;
        }
        return distanceActual;
    }

    static std::optional<int32_t> fetchHeight(const HeightType& type, const std::optional<TileEvaluationHeights>& heights)
    {
        switch (type)
        {
            case HeightType::Land:
                return heights.has_value() ? std::make_optional(heights.value().land) : std::nullopt;
            case HeightType::Water:
                return heights.has_value() ? heights.value().water : std::nullopt;
            default:
                throw std::runtime_error("Unknown HeightType");
        }
    }

    static std::optional<int32_t> fetchHeight(
        const HeightSource& source, const HeightType& type, const EvaluationHeights& localHeights)
    {
        switch (source) // TODO handle sea level better
        {
            case HeightSource::Self:
                return fetchHeight(type, localHeights.self);
            case HeightSource::NeighbourNW:
                return fetchHeight(type, localHeights.neighbourNW);
            case HeightSource::NeighbourNE:
                return fetchHeight(type, localHeights.neighbourNE);
            case HeightSource::NeighbourSE:
                return fetchHeight(type, localHeights.neighbourSE);
            case HeightSource::NeighbourSW:
                return fetchHeight(type, localHeights.neighbourSW);
            case HeightSource::GlobalMin:
                return localHeights.globalMin;
            case HeightSource::GlobalMax:
                return localHeights.globalMax;
            case HeightSource::GlobalWaterLevel:
                return localHeights.globalWaterLevel;
            default:
                throw std::runtime_error("Unknown HeightSource");
        }
    }

    static std::optional<int32_t> fetchHeightValue(const HeightData& heightData, const EvaluationHeights& localHeights)
    {
        if (heightData.mode == HeightMode::Absolute)
        {
            switch (heightData.typeFirst)
            {
                case HeightType::Land:
                    return localHeights.self.land;
                case HeightType::Water:
                    return localHeights.self.water;
                default:
                    throw std::runtime_error("Unknown HeightType");
            }
        }

        const auto heightFirst = fetchHeight(heightData.sourceFirst, heightData.typeFirst, localHeights);
        const auto heightSecond = fetchHeight(heightData.sourceSecond, heightData.typeSecond, localHeights);

        if (heightFirst.has_value() && heightSecond.has_value())
        {
            return heightFirst.value() - heightSecond.value();
        }

        return std::nullopt;
    }

    static std::optional<int32_t> evaluateCondition(EvaluationContext& ctx, const ConditionKey& key, const Condition& condition)
    {
        switch (condition.type)
        {
            case Type::Height:
            {
                const auto heightData = std::get<HeightData>(condition.data);
                const auto heightActual = fetchHeightValue(heightData, ctx.evaluationHeights);
                return heightActual.has_value()
                    ? evaluatePredicate(heightActual.value(), condition.predicate, heightData.height)
                    : std::nullopt;
            }
            case Type::Distance:
            {
                auto distanceData = std::get<DistanceData>(condition.data);
                auto distanceActual = lookupDistanceTo(ctx, distanceData.feature);
                const auto limit = distanceData.distance;
                return evaluatePredicate(distanceActual, condition.predicate, limit);
            }
            case Type::Noise:
            {
                auto noiseValue = (1.0f + ctx.conditionNoiseFns[key]->generate(ctx.quadCoords)) / 2.0f;
                auto noiseCondition = std::get<NoiseData>(condition.data).value;
                return evaluatePredicate(noiseValue, condition.predicate, noiseCondition);
            }
            case Type::NormalAngle:
            {
                auto normal = ctx.normalMap[ctx.genCoords];
                auto angle = VecXYZ{ 0.0f, 0.0f, 1.0f }.Angle(normal);
                return evaluatePredicate(angle, condition.predicate, std::get<NormalAngleData>(condition.data).angle);
            }
            case Type::Random:
            {
                auto prngValue = ctx.prngDist(ctx.conditionPrngs[key]);
                return evaluatePredicate(prngValue, condition.predicate, std::get<RandomData>(condition.data).value);
            }
            case Type::BlendNoise:
            {
                auto& noiseBlendData = std::get<BlendNoiseData>(condition.data);
                auto noiseValue = (1.0f + ctx.conditionNoiseFns[key]->generate(ctx.quadCoords)) / 2.0f;
                auto noiseSs = Smoothstep(noiseBlendData.edgeLow, noiseBlendData.edgeHigh, noiseValue);
                auto prngValue = ctx.prngDist(ctx.conditionPrngs[key]);
                return evaluatePredicate(prngValue, condition.predicate, noiseSs);
            }
            case Type::BlendHeight:
            {
                auto& heightBlendData = std::get<BlendHeightData>(condition.data);
                auto heightSs = Smoothstep(heightBlendData.edgeLow, heightBlendData.edgeHigh, ctx.evaluationHeights.self.land);
                auto prngValue = ctx.prngDist(ctx.conditionPrngs[key]);
                return evaluatePredicate(prngValue, condition.predicate, heightSs);
            }
            case Type::BlendDistance:
            {
                auto& distanceBlendData = std::get<BlendDistanceData>(condition.data);
                auto actualDistance = lookupDistanceTo(ctx, distanceBlendData.feature);
                auto heightSs = Smoothstep(distanceBlendData.edgeLow, distanceBlendData.edgeHigh, actualDistance);
                auto prngValue = ctx.prngDist(ctx.conditionPrngs[key]);
                return evaluatePredicate(prngValue, condition.predicate, heightSs);
            }
            case Type::LandStyle:
            {
                auto& landStyleData = std::get<LandStyleData>(condition.data);
                auto contained = landStyleData.styles.contains(ctx.landTexture);
                // using = as include, != as exclude
                if (condition.predicate == Predicate::Equal)
                {
                    return contained ? std::make_optional(1) : std::nullopt;
                }
                if (condition.predicate == Predicate::NotEqual)
                {
                    return !contained ? std::make_optional(1) : std::nullopt;
                }
                throw std::invalid_argument("unsupported predicate");
            }
            default:
            {
                throw std::invalid_argument("unknown condition type");
            }
        }
    }

    static bool evaluateConditions(EvaluationContext& ctx, const int32_t& ruleIdx, const std::vector<Condition>& conditions)
    {
        for (int32_t c = 0; c < static_cast<int32_t>(conditions.size()); c++)
        {
            auto& condition = conditions[c];
            auto key = ConditionKey{ ruleIdx, c };

            if (!condition.enabled)
            {
                continue;
            }

            std::optional<uint8_t> result = evaluateCondition(ctx, key, condition);
            if (!result.has_value())
            {
                return false;
            }

            if (condition.zRepeat)
            {
                ctx.zRepeat = result;
            }
        }
        return true;
    }

    static TextureResult textureResultFromRulesAt(const TextureRuleList& rules, EvaluationContext& ctx)
    {
        TextureResult result;

        for (int32_t r = static_cast<int32_t>(rules.size()) - 1; r >= 0; --r)
        {
            auto& rule = rules[r];

            if (!rule.enabled)
            {
                continue;
            }

            if (evaluateConditions(ctx, r, rule.conditions))
            {
                if (rule.effect.applyLandTexture && !result.landTexture.has_value())
                {
                    result.landTexture = std::make_optional<>(rule.effect.landTexture);
                }

                if (rule.effect.applyEdgeTexture && !result.edgeTexture.has_value())
                {
                    result.edgeTexture = std::make_optional<>(rule.effect.edgeTexture);
                }

                if (result.landTexture.has_value() && result.edgeTexture.has_value())
                {
                    break;
                }
            }
        }

        return result;
    }

    static std::optional<SceneryResultItem> sceneryResultFromRuleEffect(
        EvaluationContext& ctx, const int32_t ruleIdx, const SceneryRule& rule)
    {
        if (rule.effect.objects.empty())
        {
            return std::nullopt;
        }

        auto idx = ctx.ruleItemDists[ruleIdx](ctx.rulePrngs[ruleIdx]);
        auto& selectedItem = rule.effect.objects[idx];

        auto result = SceneryResultItem{
            .type = selectedItem.type,
            .index = selectedItem.index,
            .direction = selectedItem.direction.has_value() ? selectedItem.direction.value()
                                                            : static_cast<uint8_t>(ctx.directionDist(ctx.rulePrngs[ruleIdx])),
            .colours = selectedItem.colours,
            .zRepeat = rule.zRepeat ? ctx.zRepeat : std::nullopt,
        };

        return std::make_optional(result);
    }

    static MaybeSceneryResult sceneryResultFromRulesAt(const SceneryRuleList& rules, EvaluationContext& ctx)
    {
        std::array<uint8_t, 4> quadIndices = { 0, 1, 2, 3 };
        std::ranges::shuffle(quadIndices, ctx.quadPrng);

        std::optional<WallSceneryItems> wallResult = std::nullopt;
        std::optional<TileSceneryItems> tileResult = std::nullopt;

        for (int32_t r = static_cast<int32_t>(rules.size()) - 1; r >= 0; r--)
        {
            const auto& rule = rules[r];
            ctx.zRepeat = std::nullopt;

            if (!rule.enabled)
            {
                continue;
            }

            for (size_t qIdx = 0; qIdx < quadIndices.size(); qIdx++)
            {
                const auto quad = quadIndices[qIdx];

                ctx.quadCoords = VecXY{ ctx.genCoords.x, ctx.genCoords.y } + kQuadOffset[quad];
                if (!evaluateConditions(ctx, r, rule.conditions))
                {
                    // rule doesn't match for tile/quad
                    continue;
                }

                auto item = sceneryResultFromRuleEffect(ctx, r, rule);
                if (!item.has_value())
                {
                    // rule matches but has empty object pool, abort to support clearing/glade rules
                    return std::nullopt;
                }

                switch (item.value().type)
                {
                    case Small:
                    {
                        auto* entry = ObjectEntryManager::GetObjectEntry<SmallSceneryEntry>(item.value().index);
                        Guard::Assert(entry != nullptr);

                        // treating diagonal as full tile cause sanity
                        if (entry->flags.hasAny(SmallSceneryFlag::occupiesFullTile, SmallSceneryFlag::isDiagonal))
                        {
                            // avoid oversampling on full-tile scenery by only applying full tile on first quad
                            if (!tileResult.has_value() && qIdx == 0)
                            {
                                tileResult = item.value();
                            }
                        }
                        else
                        {
                            if (!tileResult.has_value())
                            {
                                tileResult = QuadSceneryItems{ std::nullopt, std::nullopt, std::nullopt, std::nullopt };
                            }
                            if (std::holds_alternative<QuadSceneryItems>(tileResult.value()))
                            {
                                auto& quadItems = std::get<QuadSceneryItems>(tileResult.value());
                                quadItems[quad] = item;
                            }
                        }
                        break;
                    }
                    case Large:
                    {
                        // TODO
                        break;
                    }
                    case Wall:
                    {
                        // TODO handle SmallSceneryFlag::prohibitWalls? The placement actions should handle it as the walls are
                        //   placed after
                        if (!wallResult.has_value())
                        {
                            wallResult = WallSceneryItems{ std::nullopt, std::nullopt, std::nullopt, std::nullopt };
                        }
                        if (!wallResult.value()[item.value().direction].has_value())
                        {
                            wallResult.value()[item.value().direction] = item.value();
                        }
                        break;
                    }
                }
            }
        }

        if (wallResult.has_value() && tileResult.has_value())
        {
            return SceneryResult{ wallResult.value(), tileResult.value() };
        }
        if (wallResult.has_value())
        {
            return SceneryResult{ wallResult.value(),
                                  QuadSceneryItems{ std::nullopt, std::nullopt, std::nullopt, std::nullopt } };
        }
        if (tileResult.has_value())
        {
            return SceneryResult{ WallSceneryItems{ std::nullopt, std::nullopt, std::nullopt, std::nullopt },
                                  tileResult.value() };
        }
        return std::nullopt;
    }

    static void initializeEvaluationContextForCondition(
        const MapGenContext& genCtx, EvaluationContext& evalCtx, const ConditionKey& key, const Condition& condition)
    {
        if (condition.type == Type::Noise)
        {
            auto& noiseData = std::get<NoiseData>(condition.data);

            BaseSettings baseSettings = { BaseType::Simplex, genCtx.settings.seed + noiseData.seedOffset,
                                          noiseData.frequency * kNoiseScale };
            FractalSettings fractalSettings = { FractalType::Fbm, noiseData.octaves, 2.0f, 0.5f, 0.0f };

            auto noise = std::make_unique<Noise>(baseSettings, fractalSettings, std::nullopt, std::nullopt);
            evalCtx.conditionNoiseFns[key] = std::move(noise);
        }
        else if (condition.type == Type::Random)
        {
            auto& prngData = std::get<RandomData>(condition.data);
            std::mt19937 prng(genCtx.settings.seed + prngData.seedOffset);
            evalCtx.conditionPrngs[key] = std::move(prng);
        }
        else if (condition.type == Type::BlendNoise)
        {
            auto& noiseBlendData = std::get<BlendNoiseData>(condition.data);

            BaseSettings baseSettings = { BaseType::Simplex, genCtx.settings.seed + noiseBlendData.seedOffset,
                                          noiseBlendData.frequency * kNoiseScale };
            FractalSettings fractalSettings = { FractalType::Fbm, noiseBlendData.octaves, 2.0f, 0.5f, 0.0f };

            auto noise = std::make_unique<Noise>(baseSettings, fractalSettings, std::nullopt, std::nullopt);
            evalCtx.conditionNoiseFns[key] = std::move(noise);
            // shouldn't cause artifacts to use the same seed for prng and noise?
            std::mt19937 prng(genCtx.settings.seed + noiseBlendData.seedOffset);
            evalCtx.conditionPrngs[key] = std::move(prng);
        }
        else if (condition.type == Type::BlendHeight)
        {
            auto& heightBlendData = std::get<BlendHeightData>(condition.data);
            std::mt19937 prng(genCtx.settings.seed + heightBlendData.seedOffset);
            evalCtx.conditionPrngs[key] = std::move(prng);
        }
    }

    static std::optional<TileEvaluationHeights> getHeightsAt(const TileCoordsXY& gameCoords)
    {
        auto* surfaceElement = MapGetSurfaceElementAt(gameCoords);
        if (surfaceElement == nullptr)
        {
            return std::nullopt;
        }

        const int32_t baseHeight = surfaceElement->baseHeight;
        const int32_t waterHeight = surfaceElement->GetWaterHeight() / kWaterHeightStep;

        return std::make_optional(
            TileEvaluationHeights{ baseHeight, waterHeight > 0 ? std::make_optional(waterHeight) : std::nullopt });
    }

    static EvaluationHeights getHeightsAt(const MapGenContext& genCtx, const TileCoordsXY& gameCoords)
    {
        return {
            .globalMin = genCtx.settings.heightmapLow,
            .globalMax = genCtx.settings.heightmapHigh,
            .globalWaterLevel = genCtx.settings.waterLevel,
            .self = getHeightsAt(gameCoords).value(),
            .neighbourNW = getHeightsAt(gameCoords + kNeighbourNorthWest.offset),
            .neighbourNE = getHeightsAt(gameCoords + kNeighbourNorthEast.offset),
            .neighbourSE = getHeightsAt(gameCoords + kNeighbourSouthEast.offset),
            .neighbourSW = getHeightsAt(gameCoords + kNeighbourSouthWest.offset),
        };
    }

    template<typename RR, typename RL>
    static void processRules(
        const MapGenContext& genCtx, const RL& rules, EvaluationContext& evalCtx,
        const std::function<RR(const RL& rules, EvaluationContext& ctx)>& evaluateAtFn,
        const std::function<void(TileCoordsXY, RR)>& callback)
    {
        auto& gameState = getGameState();
        for (int32_t y = 1; y < gameState.mapSize.y - 1; y++)
        {
            for (int32_t x = 1; x < gameState.mapSize.x - 1; x++)
            {
                evalCtx.worldCoords = { x, y };
                evalCtx.genCoords = worldCoordsToGenCoords(genCtx, evalCtx.worldCoords);
                evalCtx.quadCoords = VecXY{ evalCtx.genCoords.x, evalCtx.genCoords.y };

                auto* surfaceElement = MapGetSurfaceElementAt(evalCtx.worldCoords);
                if (surfaceElement == nullptr)
                {
                    return;
                }

                evalCtx.evaluationHeights = getHeightsAt(genCtx, evalCtx.worldCoords);
                evalCtx.landTexture = surfaceElement->GetSurfaceObjectIndex();

                auto result = evaluateAtFn(rules, evalCtx);
                callback(evalCtx.worldCoords, result);
            }
        }
    }

    void initializeEvaluationContext(const MapGenContext& genCtx, EvaluationContext& evalCtx)
    {
        evalCtx.quadPrng = std::mt19937(genCtx.settings.seed + 4);

        computeNormalMap(genCtx, evalCtx.normalMap);
        computeWaterDistanceMap(genCtx, evalCtx);
        computeSeaDistanceMap(genCtx, evalCtx);
        computeLandDistanceMap(genCtx, evalCtx);
        computeRiverFlagBasedDistanceMap(genCtx, evalCtx.distanceToRiver, River::RiverFlag::river);
        computeRiverFlagBasedDistanceMap(genCtx, evalCtx.distanceToFill, River::RiverFlag::filled);
        computeRiverFlagBasedDistanceMap(genCtx, evalCtx.distanceToBreach, River::RiverFlag::breached);
        computeBorderDistanceMap(genCtx, evalCtx);
    }

    static void resetEvaluationContextRuleAndConditionState(EvaluationContext& evalCtx)
    {
        evalCtx.ruleItemDists.clear();
        evalCtx.rulePrngs.clear();
        evalCtx.conditionNoiseFns.clear();
        evalCtx.conditionPrngs.clear();
    }

    void evaluateTextureRules(const MapGenContext& genCtx, EvaluationContext& evalCtx, const Callback<TextureResult>& callback)
    {
        resetEvaluationContextRuleAndConditionState(evalCtx);

        for (size_t r = 0; r < genCtx.settings.textureRules.size(); ++r)
        {
            auto& rule = genCtx.settings.textureRules[r];
            for (size_t c = 0; c < rule.conditions.size(); ++c)
            {
                auto& condition = rule.conditions[c];
                auto key = ConditionKey{ static_cast<int32_t>(r), static_cast<int32_t>(c) };
                initializeEvaluationContextForCondition(genCtx, evalCtx, key, condition);
            }
        }

        processRules<TextureResult, TextureRuleList>(
            genCtx, genCtx.settings.textureRules, evalCtx, textureResultFromRulesAt, callback);
    }

    void evaluateSceneryRules(
        const MapGenContext& genCtx, EvaluationContext& evalCtx, const Callback<MaybeSceneryResult>& callback)
    {
        resetEvaluationContextRuleAndConditionState(evalCtx);

        for (size_t r = 0; r < genCtx.settings.sceneryRules.size(); ++r)
        {
            auto& rule = genCtx.settings.sceneryRules[r];

            auto weights = rule.effect.objects | std::views::transform(&SceneryEffectItem::weight);
            evalCtx.ruleItemDists[r] = std::discrete_distribution(std::ranges::begin(weights), std::ranges::end(weights));
            evalCtx.rulePrngs[r] = std::mt19937(genCtx.settings.seed + rule.effect.seedOffset);

            for (size_t c = 0; c < rule.conditions.size(); ++c)
            {
                auto& condition = rule.conditions[c];
                auto key = ConditionKey{ static_cast<int32_t>(r), static_cast<int32_t>(c) };
                initializeEvaluationContextForCondition(genCtx, evalCtx, key, condition);
            }
        }

        processRules<MaybeSceneryResult, SceneryRuleList>(
            genCtx, genCtx.settings.sceneryRules, evalCtx, sceneryResultFromRulesAt, callback);
    }

} // namespace OpenRCT2::World::MapGenerator::Rule
