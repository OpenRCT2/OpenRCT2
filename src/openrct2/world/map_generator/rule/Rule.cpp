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
#include "../../../localisation/Formatting.h"
#include "../../../object/ObjectEntryManager.h"
#include "../../../object/ObjectManager.h"
#include "../../../object/SmallSceneryEntry.h"
#include "../../../util/Util.h"
#include "../../Map.h"
#include "../DistanceMapUtils.h"
#include "../MapGen.h"
#include "../MapHelpers.h"
#include "../Noise.h"
#include "../TileQueue.hpp"

#include <charconv>
#include <random>
#include <ranges>
#include <regex>
#include <span>

namespace OpenRCT2::World::MapGenerator::Rule
{
    constexpr float kNoiseScale = 1.0f / 256.0f;

    constexpr VecXY kQuadOffset[] = {
        VecXY{ 0.0f, 0.0f },
        VecXY{ 0.0f, 0.5f },
        VecXY{ 0.5f, 0.5f },
        VecXY{ 0.5f, 0.0f },
    };

    constexpr std::string_view kSurfaceSoil[] = {
        "rct2.terrain_surface.dirt",
        "rct2.terrain_surface.grass",
        "rct2.terrain_surface.grass_clumps",
    };

    constexpr std::string_view kSurfaceSnow[] = {
        "rct2.terrain_surface.ice",
    };

    constexpr std::string_view kSurfaceSand[] = {
        "rct2.terrain_surface.sand",
        "rct2.terrain_surface.sand_brown",
        "rct2.terrain_surface.sand_red",
    };

    struct SceneryPresetItem
    {
        std::string_view identifier;
        int8_t weight;
        std::optional<Drawing::Colour> primary = std::nullopt;
        std::optional<Direction> direction = std::nullopt;
        RuleSceneryType type = Small;
    };

    constexpr SceneryPresetItem kDefaultShrub[] = {
        SceneryPresetItem{ "rct2.scenery_small.tsh0", 1 },
        SceneryPresetItem{ "rct2.scenery_small.tsh1", 1 },
        SceneryPresetItem{ "rct2.scenery_small.tsh4", 1 },
    };

    constexpr SceneryPresetItem kTreesMixed[] = {
        SceneryPresetItem{ "rct2.scenery_small.tap", 4 },  SceneryPresetItem{ "rct2.scenery_small.tas", 1 },
        SceneryPresetItem{ "rct2.scenery_small.tbp", 4 },  SceneryPresetItem{ "rct2.scenery_small.tcf", 1 },
        SceneryPresetItem{ "rct2.scenery_small.tco", 1 },  SceneryPresetItem{ "rct2.scenery_small.tel", 4 },
        SceneryPresetItem{ "rct2.scenery_small.tghc", 2 }, SceneryPresetItem{ "rct2.scenery_small.tghc2", 2 },
        SceneryPresetItem{ "rct2.scenery_small.tmbj", 4 }, SceneryPresetItem{ "rct2.scenery_small.tmc", 1 },
        SceneryPresetItem{ "rct2.scenery_small.tmzp", 1 }, SceneryPresetItem{ "rct2.scenery_small.tsb", 2 },
        SceneryPresetItem{ "rct2.scenery_small.twn", 4 },  SceneryPresetItem{ "rct2.scenery_small.tww", 2 },
        SceneryPresetItem{ "rct2.scenery_small.twp", 1 },
    };

    constexpr SceneryPresetItem kTreesConifer[] = {
        SceneryPresetItem{ "rct2.scenery_small.tcf", 8 },  SceneryPresetItem{ "rct2.scenery_small.tel", 1 },
        SceneryPresetItem{ "rct2.scenery_small.tmbj", 4 }, SceneryPresetItem{ "rct2.scenery_small.tns", 8 },
        SceneryPresetItem{ "rct2.scenery_small.trf", 8 },  SceneryPresetItem{ "rct2.scenery_small.trf2", 8 },
    };

    constexpr SceneryPresetItem kTreesConiferSnow[] = {
        SceneryPresetItem{ "rct2.scenery_small.tcfs", 1 },
        SceneryPresetItem{ "rct2.scenery_small.tnss", 1 },
        SceneryPresetItem{ "rct2.scenery_small.trf3", 1 },
        SceneryPresetItem{ "rct2.scenery_small.trfs", 1 },
    };

    constexpr SceneryPresetItem kColdShrubs[] = {
        SceneryPresetItem{ "rct2.scenery_small.tsh1", 4 },
        SceneryPresetItem{ "rct2.scenery_small.tsh4", 4 },
        SceneryPresetItem{ "rct2.scenery_small.tsh0", 1 },
    };

    constexpr SceneryPresetItem kAridCacti[] = {
        SceneryPresetItem{ "rct2.scenery_small.tbc", 1 },
        SceneryPresetItem{ "rct2.scenery_small.tsc", 4 },
    };

    constexpr SceneryPresetItem kAridShrubs[] = {
        SceneryPresetItem{ "rct2.scenery_small.tsh1", 2 },
        SceneryPresetItem{ "rct2.scenery_small.tsh4", 2 },
        SceneryPresetItem{ "rct2.scenery_small.tjb1", 1 },
    };

    constexpr SceneryPresetItem kAridOasis[] = {
        SceneryPresetItem{ "rct2.scenery_small.tmp", 1 },  SceneryPresetItem{ "rct2.scenery_small.thl", 1 },
        SceneryPresetItem{ "rct2.scenery_small.th1", 4 },  SceneryPresetItem{ "rct2.scenery_small.th2", 4 },
        SceneryPresetItem{ "rct2.scenery_small.tpm", 4 },  SceneryPresetItem{ "rct2.scenery_small.tropt1", 2 },
        SceneryPresetItem{ "rct2.scenery_small.tsh1", 4 }, SceneryPresetItem{ "rct2.scenery_small.tsh4", 4 },
        SceneryPresetItem{ "rct2.scenery_small.tjb1", 4 },
    };

    constexpr SceneryPresetItem kRainforestFlower[] = {
        SceneryPresetItem{ "rct2.scenery_small.tjf", 1, std::optional{ Drawing::Colour::saturatedRed } },
        SceneryPresetItem{ "rct2.scenery_small.tjf", 1, std::optional{ Drawing::Colour::white } }
    };

    constexpr SceneryPresetItem kRainforestGround[] = {
        SceneryPresetItem{ "rct2.scenery_small.tg19", 1, std::optional{ Drawing::Colour::forestGreen } },
        SceneryPresetItem{ "rct2.scenery_small.tg19", 1, std::optional{ Drawing::Colour::darkOliveGreen } },
        SceneryPresetItem{ "rct2.scenery_small.tg19", 1, std::optional{ Drawing::Colour::armyGreen } },
        SceneryPresetItem{ "rct2.scenery_small.tg19", 1, std::optional{ Drawing::Colour::mossGreen } },
    };

    constexpr SceneryPresetItem kRainforestShrub[] = {
        SceneryPresetItem{ "rct2.scenery_small.tjp1", 5 },  SceneryPresetItem{ "rct2.scenery_small.tjp2", 3 },
        SceneryPresetItem{ "rct2.scenery_small.tjb1", 2 },  SceneryPresetItem{ "rct2.scenery_small.tjb2", 7 },
        SceneryPresetItem{ "rct2.scenery_small.tjb3", 1 },  SceneryPresetItem{ "rct2.scenery_small.tjb4", 1 },
        SceneryPresetItem{ "rct2.scenery_small.fern1", 9 }, SceneryPresetItem{ "rct2.scenery_small.beanst2", 1 },
    };

    constexpr SceneryPresetItem kRainforestTrees[] = {
        SceneryPresetItem{ "rct2.scenery_small.tjt1", 4 }, SceneryPresetItem{ "rct2.scenery_small.tjt2", 4 },
        SceneryPresetItem{ "rct2.scenery_small.tjt3", 1 }, SceneryPresetItem{ "rct2.scenery_small.tjt4", 1 },
        SceneryPresetItem{ "rct2.scenery_small.tjt5", 1 }, SceneryPresetItem{ "rct2.scenery_small.tjt6", 1 },
    };

    constexpr SceneryPresetItem kMediterraneanTrees[] = {
        SceneryPresetItem{ "rct2.scenery_small.tap", 3 }, SceneryPresetItem{ "rct2.scenery_small.tcrp", 3 },
        SceneryPresetItem{ "rct2.scenery_small.tac", 1 }, SceneryPresetItem{ "rct2.scenery_small.tcj", 3 },
        SceneryPresetItem{ "rct2.scenery_small.tic", 1 }, SceneryPresetItem{ "rct2.scenery_small.titc", 5 },
        SceneryPresetItem{ "rct2.scenery_small.tlc", 1 }, SceneryPresetItem{ "rct2.scenery_small.tlp", 5 },
    };

    constexpr SceneryPresetItem kMediterraneanShrubs[] = {
        SceneryPresetItem{ "rct2.scenery_small.tsh0", 1 }, SceneryPresetItem{ "rct2.scenery_small.tsh1", 1 },
        SceneryPresetItem{ "rct2.scenery_small.tsh2", 1 }, SceneryPresetItem{ "rct2.scenery_small.tsh3", 1 },
        SceneryPresetItem{ "rct2.scenery_small.tsh4", 1 }, SceneryPresetItem{ "rct2.scenery_small.tsh5", 1 },
    };

    constexpr SceneryPresetItem kWaterfallNw[] = {
        SceneryPresetItem{ "rct2.scenery_wall.wallwf16", 1, std::nullopt, 3, Wall },
    };

    constexpr SceneryPresetItem kWaterfallNe[] = {
        SceneryPresetItem{ "rct2.scenery_wall.wallwf16", 1, std::nullopt, 0, Wall },
    };

    constexpr SceneryPresetItem kWaterfallSe[] = {
        SceneryPresetItem{ "rct2.scenery_wall.wallwf16", 1, std::nullopt, 1, Wall },
    };

    constexpr SceneryPresetItem kWaterfallSw[] = {
        SceneryPresetItem{ "rct2.scenery_wall.wallwf16", 1, std::nullopt, 2, Wall },
    };

    static std::optional<ObjectEntryIndex> lookupObjectEntryIdxByIdentifier(const std::string_view identifier)
    {
        auto& objectManager = GetContext()->GetObjectManager();
        ObjectEntryIndex idx = objectManager.GetLoadedObjectEntryIndex(identifier);
        return idx == kObjectEntryIndexNull ? std::nullopt : std::make_optional(idx);
    }

    static std::unordered_set<ObjectEntryIndex> landStylesOf(const std::span<const std::string_view> styles)
    {
        auto result = std::unordered_set<ObjectEntryIndex>();

        for (auto& style : styles)
        {
            auto maybeObjectId = lookupObjectEntryIdxByIdentifier(style);
            if (maybeObjectId.has_value())
            {
                result.insert(maybeObjectId.value());
            }
        }

        return result;
    }

    static std::vector<SceneryEffectItem> toSceneryEffectItemsIfAvailable(const std::span<const SceneryPresetItem> presetItems)
    {
        std::vector<SceneryEffectItem> result{};

        for (auto& item : presetItems)
        {
            auto maybeObjectId = lookupObjectEntryIdxByIdentifier(item.identifier);
            if (maybeObjectId.has_value())
            {
                result.push_back(
                    SceneryEffectItem{ .type = item.type,
                                       .index = maybeObjectId.value(),
                                       .weight = item.weight,
                                       .direction = item.direction,
                                       .colours = { item.primary.value_or(Drawing::Colour::bordeauxRed),
                                                    Drawing::Colour::yellow, Drawing::Colour::darkBrown } });
            }
        }
        return result;
    }

    static void computeNormalMap(const MapGenCtx& genCtx, NormalMap& normalMap)
    {
        // TODO actually compute the normal
        normalMap = NormalMap{ genCtx.dimensions };
        normalMap.fill({ 0.0f, 0.0f, 1.0f });
    }

    static void computeLandDistanceMap(const MapGenCtx& genCtx, EvaluationContext& evalCtx)
    {
        evalCtx.distanceToLand = DistanceMap{ genCtx.dimensions };
        evalCtx.distanceToLand.fill(std::numeric_limits<float>::infinity());

        StableTileQueue queue;
        MaskMap visited{ genCtx.dimensions };

        for (int32_t y = 0; y < genCtx.dimensions.y; y++)
        {
            for (int32_t x = 0; x < genCtx.dimensions.x; x++)
            {
                const TileCoordsXY pos{ x, y };

                bool isRiver = genCtx.hydroMaps.has_value() && genCtx.hydroMaps.value().flags[pos].has(Hydro::river);
                bool isSea = quantizeHeight(genCtx.heightMap[pos]) < genCtx.settings.waterLevel;

                if (!isSea && !isRiver)
                {
                    initZeroDistance(pos, evalCtx.distanceToLand, queue, visited);
                }
            }
        }

        completeDistanceMap(evalCtx.distanceToLand, queue, visited);
    }

    static void computeWaterDistanceMap(const MapGenCtx& genCtx, EvaluationContext& evalCtx)
    {
        evalCtx.distanceToWater = DistanceMap{ genCtx.dimensions };
        evalCtx.distanceToWater.fill(std::numeric_limits<float>::infinity());

        StableTileQueue queue;
        MaskMap visited{ genCtx.dimensions };

        for (int32_t y = 0; y < genCtx.dimensions.y; y++)
        {
            for (int32_t x = 0; x < genCtx.dimensions.x; x++)
            {
                const TileCoordsXY pos{ x, y };

                bool isRiver = genCtx.hydroMaps.has_value() && genCtx.hydroMaps.value().flags[pos].has(Hydro::river);
                bool isSea = quantizeHeight(genCtx.heightMap[pos]) < genCtx.settings.waterLevel;

                if (isSea || isRiver)
                {
                    initZeroDistance(pos, evalCtx.distanceToWater, queue, visited);
                }
            }
        }

        completeDistanceMap(evalCtx.distanceToWater, queue, visited);
    }

    static void computeSeaDistanceMap(const MapGenCtx& genCtx, EvaluationContext& evalCtx)
    {
        evalCtx.distanceToSea = DistanceMap{ genCtx.dimensions };
        evalCtx.distanceToSea.fill(std::numeric_limits<float>::infinity());

        StableTileQueue queue;
        MaskMap visited{ genCtx.dimensions };

        for (int32_t y = 0; y < genCtx.dimensions.y; y++)
        {
            for (int32_t x = 0; x < genCtx.dimensions.x; x++)
            {
                const TileCoordsXY pos{ x, y };

                bool isRiver = genCtx.hydroMaps.has_value() && genCtx.hydroMaps.value().flags[pos].has(Hydro::river);
                bool isSea = quantizeHeight(genCtx.heightMap[pos]) < genCtx.settings.waterLevel;

                if (isSea && !isRiver)
                {
                    initZeroDistance(pos, evalCtx.distanceToSea, queue, visited);
                }
            }
        }

        completeDistanceMap(evalCtx.distanceToSea, queue, visited);
    }

    static void computeBorderDistanceMap(const MapGenCtx& genCtx, EvaluationContext& evalCtx)
    {
        evalCtx.distanceToBorder = DistanceMap{ genCtx.dimensions };
        evalCtx.distanceToBorder.fill(std::numeric_limits<float>::infinity());

        StableTileQueue queue;
        MaskMap visited{ genCtx.dimensions };

        for (int32_t y = 0; y < evalCtx.distanceToBorder.height; y++)
        {
            const TileCoordsXY left{ 0, y };
            initZeroDistance(left, evalCtx.distanceToBorder, queue, visited);

            const TileCoordsXY right{ evalCtx.distanceToBorder.width - 1, y };
            initZeroDistance(right, evalCtx.distanceToBorder, queue, visited);
        }

        for (int32_t x = 1; x < evalCtx.distanceToBorder.width - 1; x++)
        {
            const TileCoordsXY top{ x, 0 };
            initZeroDistance(top, evalCtx.distanceToBorder, queue, visited);

            const TileCoordsXY bottom{ x, evalCtx.distanceToBorder.height - 1 };
            initZeroDistance(bottom, evalCtx.distanceToBorder, queue, visited);
        }

        completeDistanceMap(evalCtx.distanceToBorder, queue, visited);
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

    static std::optional<int32_t> fetchHeight(
        const HeightType& type, const std::optional<EvaluationHeights>& heights, int32_t seaLevel)
    {
        switch (type)
        {
            case HeightType::Land:
                return heights.has_value() ? std::make_optional(heights.value().land) : std::nullopt;
            case HeightType::Water:
                return heights.has_value() ? heights.value().water : std::nullopt;
            case HeightType::SeaLevel:
                return seaLevel > 0 ? std::make_optional(seaLevel) : std::nullopt;
            default:
                throw std::runtime_error("Unknown HeightType");
        }
    }

    static std::optional<int32_t> fetchHeight(
        const HeightSource& source, const HeightType& type, const LocalEvaluationHeights& localHeights)
    {
        switch (source) // TODO handle sea level better
        {
            case HeightSource::Self:
                return fetchHeight(type, localHeights.self, localHeights.seaLevel);
            case HeightSource::NeighbourNW:
                return fetchHeight(type, localHeights.neighbourNW, localHeights.seaLevel);
            case HeightSource::NeighbourNE:
                return fetchHeight(type, localHeights.neighbourNE, localHeights.seaLevel);
            case HeightSource::NeighbourSE:
                return fetchHeight(type, localHeights.neighbourSE, localHeights.seaLevel);
            case HeightSource::NeighbourSW:
                return fetchHeight(type, localHeights.neighbourSW, localHeights.seaLevel);
            default:
                throw std::runtime_error("Unknown HeightSource");
        }
    }

    static std::optional<int32_t> fetchHeightValue(const HeightData& heightData, const LocalEvaluationHeights& localHeights)
    {
        if (heightData.mode == HeightMode::Absolute)
        {
            switch (heightData.typeFirst)
            {
                case HeightType::Land:
                    return localHeights.self.land;
                case HeightType::Water:
                    return localHeights.self.water;
                case HeightType::SeaLevel:
                    return localHeights.seaLevel;
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
                const auto heightActual = fetchHeightValue(heightData, ctx.localHeights);
                return heightActual.has_value()
                    ? evaluatePredicate(heightActual.value(), condition.predicate, heightData.height)
                    : false;
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
                auto heightSs = Smoothstep(heightBlendData.edgeLow, heightBlendData.edgeHigh, ctx.localHeights.self.land);
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
        const MapGenCtx& genCtx, EvaluationContext& evalCtx, const ConditionKey& key, const Condition& condition)
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

    static std::optional<EvaluationHeights> getHeightsAt(const TileCoordsXY& gameCoords)
    {
        auto* surfaceElement = MapGetSurfaceElementAt(gameCoords);
        if (surfaceElement == nullptr)
        {
            return std::nullopt;
        }

        const int32_t baseHeight = surfaceElement->baseHeight;
        const int32_t waterHeight = surfaceElement->GetWaterHeight() / kWaterHeightStep;

        return std::make_optional(
            EvaluationHeights{ baseHeight, waterHeight > 0 ? std::make_optional(waterHeight) : std::nullopt });
    }

    static LocalEvaluationHeights getLocalHeightsAt(const MapGenCtx& genCtx, const TileCoordsXY& gameCoords)
    {
        return {
            .seaLevel = genCtx.settings.waterLevel,
            .self = getHeightsAt(gameCoords).value(),
            .neighbourNW = getHeightsAt(gameCoords + kNeighbourOffsetNW),
            .neighbourNE = getHeightsAt(gameCoords + kNeighbourOffsetNE),
            .neighbourSE = getHeightsAt(gameCoords + kNeighbourOffsetSE),
            .neighbourSW = getHeightsAt(gameCoords + kNeighbourOffsetSW),
        };
    }

    template<typename RR, typename RL>
    static void processRules(
        const MapGenCtx& genCtx, const RL& rules, EvaluationContext& evalCtx,
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

                evalCtx.localHeights = getLocalHeightsAt(genCtx, evalCtx.worldCoords);
                evalCtx.landTexture = surfaceElement->GetSurfaceObjectIndex();

                auto result = evaluateAtFn(rules, evalCtx);
                callback(evalCtx.worldCoords, result);
            }
        }
    }

    static void initEvaluationContextGlobals(const MapGenCtx& genCtx, EvaluationContext& evalCtx)
    {
        evalCtx.quadPrng = std::mt19937(genCtx.settings.seed + 4);

        computeNormalMap(genCtx, evalCtx.normalMap);
        computeWaterDistanceMap(genCtx, evalCtx);
        computeSeaDistanceMap(genCtx, evalCtx);
        computeLandDistanceMap(genCtx, evalCtx);
        computeHydroFlagBasedDistanceMap(genCtx, evalCtx.distanceToRiver, Hydro::HydroFlag::river);
        computeHydroFlagBasedDistanceMap(genCtx, evalCtx.distanceToFill, Hydro::HydroFlag::filled);
        computeHydroFlagBasedDistanceMap(genCtx, evalCtx.distanceToBreach, Hydro::HydroFlag::breached);
        computeBorderDistanceMap(genCtx, evalCtx);
    }

    void evaluateTextureRules(const MapGenCtx& genCtx, const Callback<TextureResult>& callback)
    {
        EvaluationContext evalCtx{};
        initEvaluationContextGlobals(genCtx, evalCtx);

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

    void evaluateSceneryRules(const MapGenCtx& genCtx, const Callback<MaybeSceneryResult>& callback)
    {
        EvaluationContext evalCtx{};
        initEvaluationContextGlobals(genCtx, evalCtx);

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

    void createDefaultTextureRules(Settings& settings)
    {
        settings.textureRules.clear();

        settings.textureRules.push_back(
            TextureRule{ .enabled = true,
                         .isDefault = true,
                         .name = FormatStringID(STR_MAPGEN_RULE_DEFAULT),
                         .conditions = std::vector<Condition>{},
                         .effect{ .applyLandTexture = true, .landTexture = 0, .applyEdgeTexture = true, .edgeTexture = 0 } });

        settings.textureRules.push_back(
            TextureRule{ .enabled = true,
                         .isDefault = false,
                         .name = "Waterfalls",
                         .conditions = std::vector{
                             Condition{
                                 .enabled = true,
                                 .type = Type::Distance,
                                 .predicate = Predicate::Equal,
                                 .data = DistanceData{
                                    .feature = Feature::River,
                                    .distance = 0} },
                         },
                         .effect = { .applyLandTexture = false,
                                     .landTexture = 0,
                                     .applyEdgeTexture = true,
                                     .edgeTexture = lookupObjectEntryIdxByIdentifier("rct2.terrain_edge.ice").value_or(0) } });

        settings.textureRules.push_back(
            TextureRule{ .enabled = true,
                         .isDefault = false,
                         .name = FormatStringID(STR_MAPGEN_RULE_BEACHES_WATER_BODIES),
                         .conditions = std::vector{ Condition{ .enabled = true,
                                                               .type = Type::Distance,
                                                               .predicate = Predicate::LessThanOrEqual,
                                                               .data = DistanceData{ .feature = Feature::Sea, .distance = 1 } },
                                                    Condition{ .enabled = true,
                                                               .type = Type::BlendDistance,
                                                               .predicate = Predicate::LessThan,
                                                               .data = BlendDistanceData{ .feature = Feature::River,
                                                                                          .seedOffset = 4,
                                                                                          .edgeLow = 2,
                                                                                          .edgeHigh = 16 } } },
                         .effect = { .applyLandTexture = true,
                                     .landTexture = lookupObjectEntryIdxByIdentifier("rct2.terrain_surface.sand").value_or(0),
                                     .applyEdgeTexture = false,
                                     .edgeTexture = 0 } });
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
                             .effect = {
                                 .applyLandTexture = false, .landTexture = 0, .applyEdgeTexture = false, .edgeTexture = 0 } };

        settings.textureRules.push_back(std::move(newRule));
    }

    void createNewTextureRuleFromPreset(Settings& settings, const TextureRulePreset preset)
    {
        std::random_device prng{};

        switch (preset)
        {
            case TextureRulePreset::SmallRockPatches:
                settings.textureRules.push_back(
                    TextureRule{
                        .enabled = true,
                        .isDefault = false,
                        .name = FormatStringID(STR_MAPGEN_RULE_SMALL_ROCK_PATCHES),
                        .conditions = std::vector{ Condition{
                            .enabled = true,

                            .type = Type::BlendNoise,
                            .predicate = Predicate::LessThanOrEqual,
                            .data = BlendNoiseData{ .seedOffset = prng(),
                                                    .frequency = 3.75f,
                                                    .octaves = 8,
                                                    .edgeLow = 0.7f,
                                                    .edgeHigh = 0.9f },
                        } },
                        .effect = { .applyLandTexture = true,
                                    .landTexture = lookupObjectEntryIdxByIdentifier("rct2.terrain_surface.rock").value_or(0),
                                    .applyEdgeTexture = false,
                                    .edgeTexture = 0 } });
                break;
            case TextureRulePreset::MediumDirtPatches:
                settings.textureRules.push_back(
                    TextureRule{
                        .enabled = true,
                        .isDefault = false,
                        .name = FormatStringID(STR_MAPGEN_RULE_MEDIUM_DIRT_PATCHES),
                        .conditions = std::vector{ Condition{
                            .enabled = true,
                            .type = Type::BlendNoise,
                            .predicate = Predicate::LessThanOrEqual,
                            .data = BlendNoiseData{ .seedOffset = prng(),
                                                    .frequency = 2.75f,
                                                    .octaves = 6,
                                                    .edgeLow = 0.65f,
                                                    .edgeHigh = 0.85f },
                        } },
                        .effect = { .applyLandTexture = true,
                                    .landTexture = lookupObjectEntryIdxByIdentifier("rct2.terrain_surface.dirt").value_or(0),
                                    .applyEdgeTexture = false,
                                    .edgeTexture = 0 } });
                break;
            case TextureRulePreset::LargeGrassClumpPatches:
                settings.textureRules.push_back(
                    TextureRule{
                        .enabled = true,
                        .isDefault = false,
                        .name = FormatStringID(STR_MAPGEN_RULE_LARGE_GRASS_CLUMP_PATCHES),
                        .conditions = std::vector{ Condition{ .enabled = true,
                                                              .type = Type::BlendNoise,
                                                              .predicate = Predicate::LessThanOrEqual,
                                                              .data = BlendNoiseData{ .seedOffset = prng(),
                                                                                      .frequency = 1.75f,
                                                                                      .octaves = 6,
                                                                                      .edgeLow = 0.5f,
                                                                                      .edgeHigh = 0.75f } } },
                        .effect = {
                            .applyLandTexture = true,
                            .landTexture = lookupObjectEntryIdxByIdentifier("rct2.terrain_surface.grass_clumps").value_or(0),
                            .applyEdgeTexture = false,
                            .edgeTexture = 0 } });
                break;
            case TextureRulePreset::MountainPeaks:
                settings.textureRules.push_back(
                    TextureRule{
                        .enabled = true,
                        .isDefault = false,
                        .name = FormatStringID(STR_MAPGEN_RULE_ROCKY_MOUNTAIN_PEAKS),
                        .conditions = std::vector{ Condition{
                            .enabled = true,
                            .type = Type::BlendHeight,
                            .predicate = Predicate::LessThanOrEqual,
                            .data = BlendHeightData{ .seedOffset = prng(), .edgeLow = 64, .edgeHigh = 72 } } },
                        .effect = { .applyLandTexture = true,
                                    .landTexture = lookupObjectEntryIdxByIdentifier("rct2.terrain_surface.rock").value_or(0),
                                    .applyEdgeTexture = false,
                                    .edgeTexture = 0 } });
                settings.textureRules.push_back(
                    TextureRule{
                        .enabled = true,
                        .isDefault = false,
                        .name = FormatStringID(STR_MAPGEN_RULE_SNOWY_MOUNTAIN_PEAKS),
                        .conditions = std::vector{ Condition{
                            .enabled = true,
                            .type = Type::BlendHeight,
                            .predicate = Predicate::LessThanOrEqual,
                            .data = BlendHeightData{ .seedOffset = prng(), .edgeLow = 80, .edgeHigh = 92 } } },
                        .effect = { .applyLandTexture = true,
                                    .landTexture = lookupObjectEntryIdxByIdentifier("rct2.terrain_surface.ice").value_or(0),
                                    .applyEdgeTexture = true,
                                    .edgeTexture = lookupObjectEntryIdxByIdentifier("rct2.terrain_edge.ice").value_or(0) } });
                break;
        }
    }

    static Condition aboveWater()
    {
        return Condition{ .enabled = true,
                          .type = Type::Distance,
                          .predicate = Predicate::GreaterThan,
                          .data = DistanceData{ .feature = Feature::Water, .distance = 0 } };
    }

    static Condition onSurface(const std::span<const std::string_view> styles)
    {
        return Condition{ .enabled = true,
                          .type = Type::LandStyle,
                          .predicate = Predicate::Equal,
                          .data = LandStyleData{ .styles = landStylesOf(styles) } };
    }

    static Condition chance(const uint32_t seed, const float above)
    {
        return Condition{ .enabled = true,
                          .type = Type::Random,
                          .predicate = Predicate::GreaterThan,
                          .data = RandomData{ .seedOffset = seed, .value = above } };
    }

    static Condition blendNoise(
        const uint32_t seed, const float frequency, const int32_t octaves, const float low, const float high)
    {
        return Condition{ .enabled = true,
                          .type = Type::BlendNoise,
                          .predicate = Predicate::LessThan,
                          .data = BlendNoiseData{ .seedOffset = seed,
                                                  .frequency = frequency,
                                                  .octaves = octaves,
                                                  .edgeLow = low,
                                                  .edgeHigh = high } };
    }

    static Condition blendHeight(const uint32_t seed, const int32_t low, const int32_t high)
    {
        return Condition{ .enabled = true,
                          .type = Type::BlendHeight,
                          .predicate = Predicate::GreaterThan,
                          .data = BlendHeightData{ .seedOffset = seed, .edgeLow = low, .edgeHigh = high } };
    }

    static Condition distanceToFeature(const Feature feature, const float distance)
    {
        return Condition{ .enabled = true,
                          .type = Type::Distance,
                          .predicate = Predicate::LessThan,
                          .data = DistanceData{ .feature = feature, .distance = distance } };
    }

    static Condition heightDeltaToNeighbour(const HeightSource neighbour, const HeightType type, bool zRepeat = false)
    {
        return Condition{
            .enabled = true,
            .type = Type::Height,
            .predicate = Predicate::GreaterThan,
            .data = HeightData{ .height = 0,
                                .mode = HeightMode::Relative,
                                .sourceFirst = neighbour,
                                .typeFirst = type,
                                .sourceSecond = HeightSource::Self,
                                .typeSecond = type },
            .zRepeat = zRepeat,
        };
    }

    void createDefaultSceneryRules(Settings& settings)
    {
        std::random_device prng{};
        const auto seedOffset = prng();

        settings.sceneryRules.push_back(
            SceneryRule{
                .enabled = true,
                .name = "Waterfalls NW",
                .conditions = std::vector{ distanceToFeature(Feature::River, 1),
                                           heightDeltaToNeighbour(HeightSource::NeighbourNW, HeightType::Water, true) },
                .zRepeat = true,
                .effect = {
                    .objects = toSceneryEffectItemsIfAvailable(kWaterfallNw),
                    .seedOffset = 3,
                } });
        settings.sceneryRules.push_back(
            SceneryRule{
                .enabled = true,
                .name = "Waterfalls NE",
                .conditions = std::vector{ distanceToFeature(Feature::River, 1),
                                           heightDeltaToNeighbour(HeightSource::NeighbourNE, HeightType::Water, true) },
                .zRepeat = true,
                .effect = {
                    .objects = toSceneryEffectItemsIfAvailable(kWaterfallNe),
                    .seedOffset = 3,
                } });
        settings.sceneryRules.push_back(
            SceneryRule{
                .enabled = true,
                .name = "Waterfalls SE",
                .conditions = std::vector{ distanceToFeature(Feature::River, 1),
                                           heightDeltaToNeighbour(HeightSource::NeighbourSE, HeightType::Water, true) },
                .zRepeat = true,
                .effect = {
                    .objects = toSceneryEffectItemsIfAvailable(kWaterfallSe),
                    .seedOffset = 3,
                } });
        settings.sceneryRules.push_back(
            SceneryRule{
                .enabled = true,
                .name = "Waterfalls SW",
                .conditions = std::vector{ distanceToFeature(Feature::River, 1),
                                           heightDeltaToNeighbour(HeightSource::NeighbourSW, HeightType::Water, true) },
                .zRepeat = true,
                .effect = {
                    .objects = toSceneryEffectItemsIfAvailable(kWaterfallSw),
                    .seedOffset = 3,
                } });
        settings.sceneryRules.push_back(
                    SceneryRule{
                        .enabled = true,
                        .name = FormatStringID(STR_MAPGEN_RULE_SCENERY_SHRUBS),
                        .conditions = std::vector{
                            aboveWater(),
                            blendHeight(prng(), 64, 72),
                                                    blendNoise(prng(), 9.5f, 6, 0.60f, 0.70f),
                                                   chance(prng(), 0.66f), },
                        .zRepeat = true,
                        .effect = {
                            .objects = toSceneryEffectItemsIfAvailable(kDefaultShrub),
                            .seedOffset = 4,
                        } });
        settings.sceneryRules.push_back(
                    SceneryRule{
                        .enabled = true,
                        .name = FormatStringID(STR_MAPGEN_RULE_SCENERY_MIXED),
                        .conditions = std::vector{
                            onSurface(kSurfaceSoil),
                            chance(prng(), .33f),
                            blendHeight(prng(), 32, 42),
                            blendNoise(seedOffset, 4.0f, 5, 0.35f, 0.85f),
                        },
                        .effect = {
                            .objects = toSceneryEffectItemsIfAvailable(kTreesMixed),
                            .seedOffset = 2,
                        } });
        settings.sceneryRules.push_back(
            SceneryRule{ .enabled = true,
                         .name = FormatStringID(STR_MAPGEN_RULE_SCENERY_CONIFERS),
                         .conditions = std::vector{ onSurface(kSurfaceSoil), chance(prng(), .33f), blendHeight(prng(), 54, 64),
                                                    blendNoise(seedOffset, 4.0f, 6, 0.55f, 0.80f) },
                         .effect = {
                             .objects = toSceneryEffectItemsIfAvailable(kTreesConifer),
                             .seedOffset = 2,
                         } });
    }

    void createNewSceneryRule(Settings& settings)
    {
        int32_t newRuleId = 1;

        std::regex newRuleRegex{ GetFmtStringById(STR_MAPGEN_RULE_TEMPLATE_NEW).WithoutFormatTokens() + "(\\d+)" };

        for (auto& rule : settings.sceneryRules)
        {
            std::smatch match;
            if (std::regex_match(rule.name, match, newRuleRegex))
            {
                int32_t ruleId = std::stoi(match[1].str());
                newRuleId = std::max(ruleId + 1, newRuleId);
            }
        }

        settings.sceneryRules.push_back(
            SceneryRule{ .enabled = true,
                         .name = FormatStringID(STR_MAPGEN_RULE_TEMPLATE_NEW, newRuleId),
                         .conditions = std::vector<Condition>{},
                         .effect = {
                             .objects = std::vector<SceneryEffectItem>{},
                             .seedOffset = std::random_device{}(),
                         } });
    }

    void createNewSceneryRuleFromPreset(Settings& settings, const SceneryRulePreset preset)
    {
        std::random_device prng{};
        switch (preset)
        {
            case SceneryRulePreset::Mediterranean:
            {
                auto seedOffset = prng();
                settings.sceneryRules.push_back(SceneryRule{
                    .enabled = true,
                    .name = FormatStringID(STR_MAPGEN_RULE_SCENERY_SHRUBS),
                    .conditions = {
                        onSurface(kSurfaceSoil),
                        aboveWater(),
                        chance(prng(), .55f),
                        blendNoise(seedOffset, 8.0f, 8, 0.50f, 0.75f),
                    },
                    .effect = {
                        .objects = toSceneryEffectItemsIfAvailable(kMediterraneanShrubs),
                        .seedOffset = prng(),
                }});
                settings.sceneryRules.push_back(
                    SceneryRule{
                        .enabled = true,
                        .name = FormatStringID(STR_MAPGEN_RULE_SCENERY_CYPRESSES_AND_PINES),
                        .conditions = std::vector{
                            onSurface(kSurfaceSoil),
                            chance(prng(), 0.40f),
                            blendNoise(seedOffset, 8.0f, 8, 0.50f, 1.00f),
                        },
                        .effect = {
                            .objects = toSceneryEffectItemsIfAvailable(kMediterraneanTrees),
                            .seedOffset = prng(),
                        } });
                break;
            }
            case SceneryRulePreset::Cold:
            {
                auto seedOffset = prng();
                settings.sceneryRules.push_back(SceneryRule{
                    .enabled = true,
                    .name = FormatStringID(STR_MAPGEN_RULE_SCENERY_SHRUBS),
                    .conditions = {
                        onSurface(kSurfaceSoil),
                        aboveWater(),
                        chance(prng(), .44f),
                        blendNoise(seedOffset,4.0f, 5, 0.3f, 0.6f),
                    },
                    .effect = {
                        .objects = toSceneryEffectItemsIfAvailable(kColdShrubs),
                        .seedOffset = prng(),
                }});
                settings.sceneryRules.push_back(
                    SceneryRule{
                        .enabled = true,
                        .name = FormatStringID(STR_MAPGEN_RULE_SCENERY_CONIFERS),
                        .conditions = std::vector{
                            onSurface(kSurfaceSoil), chance(prng(), 0.33f), blendNoise(seedOffset, 4.0f, 5, 0.35f, 0.65f),
                        },
                        .effect = {
                            .objects = toSceneryEffectItemsIfAvailable(kTreesConifer),
                            .seedOffset = prng(),
                        } });
                settings.sceneryRules.push_back(
                    SceneryRule{
                        .enabled = true,
                        .name = FormatStringID(STR_MAPGEN_RULE_SCENERY_CONIFERS_SNOW),
                        .conditions = std::vector{
                            onSurface(kSurfaceSnow), chance(prng(), .33f), blendNoise(seedOffset, 4.0f, 5, 0.35f, 0.65f),
                        },
                        .effect = {
                            .objects = toSceneryEffectItemsIfAvailable(kTreesConiferSnow),
                            .seedOffset = prng(),
                        } });
                break;
            }
            case SceneryRulePreset::Arid:
            {
                auto seedOffset = prng();
                settings.sceneryRules.push_back(
                    SceneryRule{ .enabled = true,
                                 .name = FormatStringID(STR_MAPGEN_RULE_SCENERY_OASIS),
                                 .conditions = { onSurface(kSurfaceSand), aboveWater(), chance(prng(), .55f),
                                                 distanceToFeature(Feature::Water, 4.0f) },
                                 .effect = {
                                     .objects = toSceneryEffectItemsIfAvailable(kAridOasis),
                                     .seedOffset = prng(),
                                 } });
                settings.sceneryRules.push_back(SceneryRule{
                    .enabled = true,
                    .name = FormatStringID(STR_MAPGEN_RULE_SCENERY_SHRUBS),
                    .conditions = {
                        onSurface(kSurfaceSand),
                        blendNoise(seedOffset,9.0f, 7, 0.62f, 1.0f),
                        chance(prng(), 0.50f),
                        aboveWater(),
                    },
                    .effect = {
                        .objects = toSceneryEffectItemsIfAvailable(kAridShrubs),
                        .seedOffset = prng(),
                }});
                settings.sceneryRules.push_back(SceneryRule{
                    .enabled = true,
                    .name = FormatStringID(STR_MAPGEN_RULE_SCENERY_CACTI),
                    .conditions = {
                        onSurface(kSurfaceSand),
                        blendNoise(seedOffset, 9.0f, 7, 0.66f, 1.00f),
                        chance(prng(), 0.33f),
                        aboveWater(),
                    },
                    .effect = {
                        .objects = toSceneryEffectItemsIfAvailable(kAridCacti),
                        .seedOffset = prng(),
                }});
                break;
            }
            case SceneryRulePreset::Rainforest:
            {
                for (auto& groudItem : toSceneryEffectItemsIfAvailable(kRainforestGround))
                {
                    settings.sceneryRules.push_back(
                        SceneryRule{ .enabled = true,
                                     .name = FormatStringID(STR_MAPGEN_RULE_SCENERY_GROUND),
                                     .conditions = std::vector{ onSurface(kSurfaceSoil), aboveWater(), chance(prng(), .25f) },
                                     .effect = {
                                         .objects = std::vector{ groudItem },
                                         .seedOffset = prng(),
                                     } });
                }
                auto flowerEffects = toSceneryEffectItemsIfAvailable(kRainforestFlower);
                for (size_t f = 0; f < flowerEffects.size(); ++f)
                {
                    settings.sceneryRules.push_back(
                        SceneryRule{ .enabled = true,
                                     .name = FormatStringID(STR_MAPGEN_RULE_SCENERY_FLOWERS),
                                     .conditions = std::vector{ onSurface(kSurfaceSoil), aboveWater(),
                                                                blendNoise(
                                                                    prng(), 7.0f + static_cast<float>(f * 2),
                                                                    7 + static_cast<int32_t>(f * 2), 0.55f, 0.77f) },
                                     .effect = {
                                         .objects = std::vector{ flowerEffects[f] },
                                         .seedOffset = prng(),
                                     } });
                }
                settings.sceneryRules.push_back(
                    SceneryRule{
                        .enabled = true,
                        .name = FormatStringID(STR_MAPGEN_RULE_SCENERY_SHRUBS),
                        .conditions = std::vector{ onSurface(kSurfaceSoil), aboveWater(), chance(prng(), .25f), },
                        .effect = {
                            .objects = toSceneryEffectItemsIfAvailable(kRainforestShrub),
                            .seedOffset = prng(),
                        } });
                settings.sceneryRules.push_back(
                    SceneryRule{
                        .enabled = true,
                        .name = FormatStringID(STR_MAPGEN_RULE_SCENERY_CANOPY),
                        .conditions = std::vector{ onSurface(kSurfaceSoil), chance(prng(), .40f), },
                        .effect = {
                            .objects = toSceneryEffectItemsIfAvailable(kRainforestTrees),
                            .seedOffset = prng(),
                        } });
                break;
            }
        }
    }

    Condition createNewCondition(const Type& type)
    {
        switch (type)
        {
            case Type::Height:
                return Condition{ .enabled = true,
                                  .type = type,
                                  .predicate = Predicate::GreaterThan,
                                  .data = HeightData{ .height = 2,
                                                      .mode = HeightMode::Absolute,
                                                      .sourceFirst = HeightSource::Self,
                                                      .typeFirst = HeightType::Land,
                                                      .sourceSecond = HeightSource::Self,
                                                      .typeSecond = HeightType::Land } };
            case Type::Distance:
                return Condition{ .enabled = true,
                                  .type = type,
                                  .predicate = Predicate::LessThan,
                                  .data = DistanceData{ .feature = Feature::Water, .distance = 2 } };
            case Type::Noise:
                return Condition{ .enabled = true,
                                  .type = type,
                                  .predicate = Predicate::GreaterThan,
                                  .data = NoiseData{
                                      .seedOffset = std::random_device{}(), .frequency = 1.75f, .octaves = 6, .value = 0.50 } };
            case Type::NormalAngle:
                return Condition{ .enabled = true,
                                  .type = type,
                                  .predicate = Predicate::GreaterThan,
                                  .data = NormalAngleData{ .angle = 15.0f } };
            case Type::Random:
                return Condition{ .enabled = true,
                                  .type = type,
                                  .predicate = Predicate::GreaterThan,
                                  .data = RandomData{ .seedOffset = std::random_device{}(), .value = .5f } };
            case Type::BlendHeight:
                return Condition{ .enabled = true,
                                  .type = type,
                                  .predicate = Predicate::LessThanOrEqual,
                                  .data = BlendHeightData{
                                      .seedOffset = std::random_device{}(), .edgeLow = 32, .edgeHigh = 64 } };
            case Type::BlendNoise:
                return Condition{ .enabled = true,
                                  .type = type,
                                  .predicate = Predicate::LessThanOrEqual,
                                  .data = BlendNoiseData{ .seedOffset = std::random_device{}(),
                                                          .frequency = 1.75f,
                                                          .octaves = 6,
                                                          .edgeLow = 0.33f,
                                                          .edgeHigh = 0.66f } };
            case Type::BlendDistance:
                return Condition{ .enabled = true,
                                  .type = type,
                                  .predicate = Predicate::GreaterThan,
                                  .data = BlendDistanceData{ .feature = Feature::Water,
                                                             .seedOffset = std::random_device{}(),
                                                             .edgeLow = 0,
                                                             .edgeHigh = 8 } };
            case Type::LandStyle:
                return Condition{ .enabled = true,
                                  .type = type,
                                  .predicate = Predicate::Equal,
                                  .data = LandStyleData{ .styles = std::unordered_set<ObjectEntryIndex>{} } };
            default:
                throw std::invalid_argument("unknown condition type");
        }
    }
} // namespace OpenRCT2::World::MapGenerator::Rule
