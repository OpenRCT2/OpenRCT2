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
#include "../MapGen.h"
#include "../Noise.h"

#include <charconv>
#include <random>
#include <ranges>
#include <regex>
#include <span>

namespace OpenRCT2::World::MapGenerator::Rule
{
    constexpr float NOISE_SCALE = 1.0f / 256.0f;

    constexpr VecXY QUAD_OFFSET[] = {
        VecXY{ 0.0f, 0.0f },
        VecXY{ 0.0f, 0.5f },
        VecXY{ 0.5f, 0.5f },
        VecXY{ 0.5f, 0.0f },
    };

    constexpr std::string_view SURFACE_SOIL[] = {
        "rct2.terrain_surface.dirt",
        "rct2.terrain_surface.grass",
        "rct2.terrain_surface.grass_clumps",
    };

    constexpr std::string_view SURFACE_SNOW[] = {
        "rct2.terrain_surface.ice",
    };

    constexpr std::string_view SURFACE_SAND[] = {
        "rct2.terrain_surface.sand",
        "rct2.terrain_surface.sand_brown",
        "rct2.terrain_surface.sand_red",
    };

    struct SceneryPresetItem
    {
        std::string_view identifier;
        int8_t weight;
        std::optional<colour_t> primary = std::nullopt;
    };

    constexpr SceneryPresetItem DEFAULT_SHRUB[] = {
        SceneryPresetItem{ "rct2.scenery_small.tsh0", 1 },
        SceneryPresetItem{ "rct2.scenery_small.tsh1", 1 },
        SceneryPresetItem{ "rct2.scenery_small.tsh4", 1 },
    };

    constexpr SceneryPresetItem TREES_MIXED[] = {
        SceneryPresetItem{ "rct2.scenery_small.tap", 4 },  SceneryPresetItem{ "rct2.scenery_small.tas", 1 },
        SceneryPresetItem{ "rct2.scenery_small.tbp", 4 },  SceneryPresetItem{ "rct2.scenery_small.tcf", 1 },
        SceneryPresetItem{ "rct2.scenery_small.tco", 1 },  SceneryPresetItem{ "rct2.scenery_small.tel", 4 },
        SceneryPresetItem{ "rct2.scenery_small.tghc", 2 }, SceneryPresetItem{ "rct2.scenery_small.tghc2", 2 },
        SceneryPresetItem{ "rct2.scenery_small.tmbj", 4 }, SceneryPresetItem{ "rct2.scenery_small.tmc", 1 },
        SceneryPresetItem{ "rct2.scenery_small.tmzp", 1 }, SceneryPresetItem{ "rct2.scenery_small.tsb", 2 },
        SceneryPresetItem{ "rct2.scenery_small.twn", 4 },  SceneryPresetItem{ "rct2.scenery_small.tww", 2 },
        SceneryPresetItem{ "rct2.scenery_small.twp", 1 },
    };

    constexpr SceneryPresetItem TREES_CONIFER[] = {
        SceneryPresetItem{ "rct2.scenery_small.tcf", 8 },  SceneryPresetItem{ "rct2.scenery_small.tel", 1 },
        SceneryPresetItem{ "rct2.scenery_small.tmbj", 4 }, SceneryPresetItem{ "rct2.scenery_small.tns", 8 },
        SceneryPresetItem{ "rct2.scenery_small.trf", 8 },  SceneryPresetItem{ "rct2.scenery_small.trf2", 8 },
    };

    constexpr SceneryPresetItem TREES_CONIFER_SNOW[] = {
        SceneryPresetItem{ "rct2.scenery_small.tcfs", 1 },
        SceneryPresetItem{ "rct2.scenery_small.tnss", 1 },
        SceneryPresetItem{ "rct2.scenery_small.trf3", 1 },
        SceneryPresetItem{ "rct2.scenery_small.trfs", 1 },
    };

    constexpr SceneryPresetItem COLD_SHRUBS[] = {
        SceneryPresetItem{ "rct2.scenery_small.tsh1", 4 },
        SceneryPresetItem{ "rct2.scenery_small.tsh4", 4 },
        SceneryPresetItem{ "rct2.scenery_small.tsh0", 1 },
    };

    constexpr SceneryPresetItem ARID_CACTI[] = {
        SceneryPresetItem{ "rct2.scenery_small.tbc", 1 },
        SceneryPresetItem{ "rct2.scenery_small.tsc", 4 },
    };

    constexpr SceneryPresetItem ARID_SHRUBS[] = {
        SceneryPresetItem{ "rct2.scenery_small.tsh1", 2 },
        SceneryPresetItem{ "rct2.scenery_small.tsh4", 2 },
        SceneryPresetItem{ "rct2.scenery_small.tjb1", 1 },
    };

    constexpr SceneryPresetItem ARID_OASIS[] = {
        SceneryPresetItem{ "rct2.scenery_small.tmp", 1 },  SceneryPresetItem{ "rct2.scenery_small.thl", 1 },
        SceneryPresetItem{ "rct2.scenery_small.th1", 4 },  SceneryPresetItem{ "rct2.scenery_small.th2", 4 },
        SceneryPresetItem{ "rct2.scenery_small.tpm", 4 },  SceneryPresetItem{ "rct2.scenery_small.tropt1", 2 },
        SceneryPresetItem{ "rct2.scenery_small.tsh1", 4 }, SceneryPresetItem{ "rct2.scenery_small.tsh4", 4 },
        SceneryPresetItem{ "rct2.scenery_small.tjb1", 4 },
    };

    constexpr SceneryPresetItem RAINFOREST_FLOWER[] = {
        SceneryPresetItem{ "rct2.scenery_small.tjf", 1, std::optional{ COLOUR_SATURATED_RED } },
        SceneryPresetItem{ "rct2.scenery_small.tjf", 1, std::optional{ COLOUR_WHITE } }
    };

    constexpr SceneryPresetItem RAINFOREST_GROUND[] = {
        SceneryPresetItem{ "rct2.scenery_small.tg19", 1, std::optional{ COLOUR_GRASS_GREEN_DARK } },
        SceneryPresetItem{ "rct2.scenery_small.tg19", 1, std::optional{ COLOUR_OLIVE_DARK } },
        SceneryPresetItem{ "rct2.scenery_small.tg19", 1, std::optional{ COLOUR_DARK_OLIVE_DARK } },
        SceneryPresetItem{ "rct2.scenery_small.tg19", 1, std::optional{ COLOUR_MOSS_GREEN } },
    };

    constexpr SceneryPresetItem RAINFOREST_SHRUB[] = {
        SceneryPresetItem{ "rct2.scenery_small.tjp1", 5 },  SceneryPresetItem{ "rct2.scenery_small.tjp2", 3 },
        SceneryPresetItem{ "rct2.scenery_small.tjb1", 2 },  SceneryPresetItem{ "rct2.scenery_small.tjb2", 7 },
        SceneryPresetItem{ "rct2.scenery_small.tjb3", 1 },  SceneryPresetItem{ "rct2.scenery_small.tjb4", 1 },
        SceneryPresetItem{ "rct2.scenery_small.fern1", 9 }, SceneryPresetItem{ "rct2.scenery_small.beanst2", 1 },
    };

    constexpr SceneryPresetItem RAINFOREST_TREES[] = {
        SceneryPresetItem{ "rct2.scenery_small.tjt1", 4 }, SceneryPresetItem{ "rct2.scenery_small.tjt2", 4 },
        SceneryPresetItem{ "rct2.scenery_small.tjt3", 1 }, SceneryPresetItem{ "rct2.scenery_small.tjt4", 1 },
        SceneryPresetItem{ "rct2.scenery_small.tjt5", 1 }, SceneryPresetItem{ "rct2.scenery_small.tjt6", 1 },
    };

    constexpr SceneryPresetItem MEDITERRANEAN_TREES[] = {
        SceneryPresetItem{ "rct2.scenery_small.tap", 3 }, SceneryPresetItem{ "rct2.scenery_small.tcrp", 3 },
        SceneryPresetItem{ "rct2.scenery_small.tac", 1 }, SceneryPresetItem{ "rct2.scenery_small.tcj", 3 },
        SceneryPresetItem{ "rct2.scenery_small.tic", 1 }, SceneryPresetItem{ "rct2.scenery_small.titc", 5 },
        SceneryPresetItem{ "rct2.scenery_small.tlc", 1 }, SceneryPresetItem{ "rct2.scenery_small.tlp", 5 },
    };

    constexpr SceneryPresetItem MEDITERRANEAN_SHRUBS[] = {
        SceneryPresetItem{ "rct2.scenery_small.tsh0", 1 }, SceneryPresetItem{ "rct2.scenery_small.tsh1", 1 },
        SceneryPresetItem{ "rct2.scenery_small.tsh2", 1 }, SceneryPresetItem{ "rct2.scenery_small.tsh3", 1 },
        SceneryPresetItem{ "rct2.scenery_small.tsh4", 1 }, SceneryPresetItem{ "rct2.scenery_small.tsh5", 1 },
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
                    SceneryEffectItem{
                        .index = maybeObjectId.value(),
                        .weight = item.weight,
                        .direction = std::nullopt,
                        .colours = { item.primary.value_or(COLOUR_BORDEAUX_RED), COLOUR_YELLOW, COLOUR_DARK_BROWN } });
            }
        }
        return result;
    }

    static float distanceToWater(const VecXY& coords, const float distance)
    {
        const auto& gameState = getGameState();

        const auto minX = std::max<float>(1, coords.x - distance);
        const auto maxX = std::max<float>(coords.x + distance, gameState.mapSize.x - 2);
        const auto minY = std::max<float>(1, coords.y - distance);
        const auto maxY = std::max<float>(coords.y + distance, gameState.mapSize.y - 2);

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
    static bool evaluatePredicate(const T a, const Predicate& predicate, const T b)
    {
        switch (predicate)
        {
            case Predicate::Equal:
            {
                return a == b;
            }
            case Predicate::NotEqual:
            {
                return a != b;
            }
            case Predicate::LessThan:
            {
                return a < b;
            }
            case Predicate::GreaterThan:
            {
                return a > b;
            }
            case Predicate::LessThanOrEqual:
            {
                return a <= b;
            }
            case Predicate::GreaterThanOrEqual:
            {
                return a >= b;
            }
            default:
            {
                throw std::invalid_argument("unknown predicate");
            }
        }
    }

    static bool evaluateCondition(EvaluationContext& ctx, const ConditionKey& key, const Condition& condition)
    {
        switch (condition.type)
        {
            case Type::HeightAbsolute:
            {
                auto heightCondition = std::get<HeightData>(condition.data).height;
                auto heightActual = ctx.heights.tile;
                return evaluatePredicate(heightActual, condition.predicate, heightCondition);
            }
            case Type::HeightRelativeToWater:
            {
                auto heightCondition = std::get<HeightData>(condition.data).height;
                auto heightActual = ctx.heights.tile - ctx.heights.water;
                return evaluatePredicate(heightActual, condition.predicate, heightCondition);
            }
            case Type::DistanceToWater:
            {
                auto distanceCondition = std::get<DistanceData>(condition.data).distance;
                auto distanceActual = distanceToWater(ctx.quadCoords, distanceCondition);
                auto limit = static_cast<float>(distanceCondition);
                return evaluatePredicate(distanceActual, condition.predicate, limit);
            }
            case Type::Noise:
            {
                auto noiseValue = (1.0f + ctx.conditionNoiseFns[key]->Generate(ctx.quadCoords)) / 2.0f;
                auto noiseCondition = std::get<NoiseData>(condition.data).value;
                return evaluatePredicate(noiseValue, condition.predicate, noiseCondition);
            }
            case Type::NormalAngle:
            {
                auto normal = ctx.normalMap[ctx.coords];
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
                auto noiseValue = (1.0f + ctx.conditionNoiseFns[key]->Generate(ctx.quadCoords)) / 2.0f;
                auto noiseSs = Smoothstep(noiseBlendData.edgeLow, noiseBlendData.edgeHigh, noiseValue);
                auto prngValue = ctx.prngDist(ctx.conditionPrngs[key]);
                return evaluatePredicate(prngValue, condition.predicate, noiseSs);
            }
            case Type::BlendHeight:
            {
                auto& heightBlendData = std::get<BlendHeightData>(condition.data);
                auto heightSs = Smoothstep(heightBlendData.edgeLow, heightBlendData.edgeHigh, ctx.heights.tile);
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
                    return contained;
                }
                if (condition.predicate == Predicate::NotEqual)
                {
                    return !contained;
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

            if (!evaluateCondition(ctx, key, condition))
            {
                return false;
            }
        }
        return true;
    }

    static std::optional<TextureResult> textureResultFromRulesAt(const TextureRuleList& rules, EvaluationContext& ctx)
    {
        for (int32_t r = static_cast<int32_t>(rules.size()) - 1; r >= 0; --r)
        {
            auto& rule = rules[r];

            if (!rule.enabled)
            {
                continue;
            }

            if (evaluateConditions(ctx, r, rule.conditions))
            {
                return std::make_optional(rule.effect);
            }
        }

        return std::nullopt;
    }

    static std::optional<SceneryResultItem> sceneryResultFromRuleEffect(
        EvaluationContext& ctx, int32_t ruleIdx, const SceneryRule& rule)
    {
        if (rule.effect.objects.empty())
        {
            return std::nullopt;
        }

        auto idx = ctx.ruleItemDists[ruleIdx](ctx.rulePrngs[ruleIdx]);
        auto& selectedItem = rule.effect.objects[idx];

        auto result = SceneryResultItem{
            .index = selectedItem.index,
            .direction = selectedItem.direction.has_value() ? selectedItem.direction.value()
                                                            : static_cast<uint8_t>(ctx.directionDist(ctx.rulePrngs[ruleIdx])),
            .colours = selectedItem.colours,
        };

        return std::make_optional(result);
    }

    static std::optional<SceneryResult> sceneryResultFromRulesAt(const SceneryRuleList& rules, EvaluationContext& ctx)
    {
        std::array<uint8_t, 4> quadIndices = { 0, 1, 2, 3 };
        std::ranges::shuffle(quadIndices, ctx.quadPrng);

        std::optional<QuadSceneryItems> quadResult = std::nullopt;

        // pick a random quad order to sample
        for (auto q : quadIndices)
        {
            for (int32_t r = static_cast<int32_t>(rules.size()) - 1; r >= 0; --r)
            {
                auto& rule = rules[r];

                if (!rule.enabled)
                {
                    continue;
                }

                ctx.quadCoords = VecXY{ ctx.coords.x, ctx.coords.y } + QUAD_OFFSET[q];
                if (!evaluateConditions(ctx, r, rule.conditions))
                {
                    // rule doesn't match for tile, try next rule
                    continue;
                }

                auto item = sceneryResultFromRuleEffect(ctx, r, rule);
                if (!item.has_value())
                {
                    // rule matches but has empty object pool, abort to support clearing/glade rules
                    return item;
                }

                auto* entry = ObjectManager::GetObjectEntry<SmallSceneryEntry>(item.value().index);
                Guard::Assert(entry != nullptr);

                // treating diagonal as full tile cause sanity
                if (entry->HasFlag(SMALL_SCENERY_FLAG_FULL_TILE) || entry->HasFlag(SMALL_SCENERY_FLAG_DIAGONAL))
                {
                    if (quadResult.has_value())
                    {
                        // already in quad mode, ignore full tile items
                        continue;
                    }
                    return item;
                }

                // got a quad item, switch to quad mode if not already and update result
                if (!quadResult.has_value())
                {
                    QuadSceneryItems result = { std::nullopt, std::nullopt, std::nullopt, std::nullopt };
                    quadResult = std::make_optional(result);
                }
                auto& quadItem = quadResult.value()[q];
                quadItem = item;
            }

            // didn't return a full tile item or switched to quad mode from the first sampled quad, abort to avoid oversampling
            // full tile items.
            if (!quadResult.has_value())
            {
                return std::nullopt;
            }
        }

        // return the quad result, at least one quad must be populated to end up here
        return quadResult;
    }

    static void initializeEvaluationContextForCondition(
        const Settings& settings, EvaluationContext& ctx, const ConditionKey& key, const Condition& condition)
    {
        if (condition.type == Type::Noise)
        {
            auto& noiseData = std::get<NoiseData>(condition.data);
            auto noise = std::make_unique<SimplexFbmNoise>(
                settings.seed + noiseData.seedOffset, noiseData.frequency * NOISE_SCALE, noiseData.octaves, 2.0f, 0.5f);
            ctx.conditionNoiseFns[key] = std::move(noise);
        }
        else if (condition.type == Type::Random)
        {
            auto& prngData = std::get<RandomData>(condition.data);
            std::mt19937 prng(settings.seed + prngData.seedOffset);
            ctx.conditionPrngs[key] = std::move(prng);
        }
        else if (condition.type == Type::NormalAngle && ctx.normalMap.empty())
        {
            // TODO compute normals
        }
        else if (condition.type == Type::BlendNoise)
        {
            auto& noiseBlendData = std::get<BlendNoiseData>(condition.data);
            auto noise = std::make_unique<SimplexFbmNoise>(
                settings.seed + noiseBlendData.seedOffset, noiseBlendData.frequency * NOISE_SCALE, noiseBlendData.octaves, 2.0f,
                0.5f);
            ctx.conditionNoiseFns[key] = std::move(noise);
            // shouldn't cause artifacts to use the same seed for prng and noise?
            std::mt19937 prng(settings.seed + noiseBlendData.seedOffset);
            ctx.conditionPrngs[key] = std::move(prng);
        }
        else if (condition.type == Type::BlendHeight)
        {
            auto& heightBlendData = std::get<BlendHeightData>(condition.data);
            std::mt19937 prng(settings.seed + heightBlendData.seedOffset);
            ctx.conditionPrngs[key] = std::move(prng);
        }
    }

    template<typename RR, typename RL>
    static void processRules(
        const Settings& settings, const RL& rules, EvaluationContext& ctx,
        const std::function<std::optional<RR>(const RL& rules, EvaluationContext& ctx)>& evaluateAtFn,
        const Callback<RR>& callback)
    {
        auto& gameState = getGameState();
        for (int32_t y = 1; y < gameState.mapSize.y - 1; y++)
        {
            for (int32_t x = 1; x < gameState.mapSize.x - 1; x++)
            {
                ctx.coords = TileCoordsXY{ x, y };
                ctx.quadCoords = VecXY{ ctx.coords.x, ctx.coords.y };

                auto* surfaceElement = MapGetSurfaceElementAt(ctx.coords);
                if (surfaceElement == nullptr)
                {
                    return;
                }

                ctx.heights = { .tile = surfaceElement->BaseHeight,
                                .min = settings.heightmapLow,
                                .max = settings.heightmapHigh,
                                .water = settings.waterLevel };
                ctx.landTexture = surfaceElement->GetSurfaceObjectIndex();

                auto result = evaluateAtFn(rules, ctx);
                callback(ctx.coords, result);
            }
        }
    }

    void evaluateTextureRules(const Settings& settings, const Callback<TextureResult>& callback)
    {
        EvaluationContext ctx{};

        for (size_t r = 0; r < settings.textureRules.size(); ++r)
        {
            auto& rule = settings.textureRules[r];
            for (size_t c = 0; c < rule.conditions.size(); ++c)
            {
                auto& condition = rule.conditions[c];
                auto key = ConditionKey{ static_cast<int32_t>(r), static_cast<int32_t>(c) };
                initializeEvaluationContextForCondition(settings, ctx, key, condition);
            }
        }

        processRules<TextureResult, TextureRuleList>(settings, settings.textureRules, ctx, textureResultFromRulesAt, callback);
    }

    void evaluateSceneryRules(const Settings& settings, const Callback<SceneryResult>& callback)
    {
        EvaluationContext ctx{};

        for (size_t r = 0; r < settings.sceneryRules.size(); ++r)
        {
            auto& rule = settings.sceneryRules[r];

            auto weights = rule.effect.objects | std::views::transform(&SceneryEffectItem::weight);
            ctx.ruleItemDists[r] = std::discrete_distribution(std::ranges::begin(weights), std::ranges::end(weights));
            ctx.rulePrngs[r] = std::mt19937(settings.seed + rule.effect.seedOffset);
            ctx.quadPrng = std::mt19937(settings.seed + r);

            for (size_t c = 0; c < rule.conditions.size(); ++c)
            {
                auto& condition = rule.conditions[c];
                auto key = ConditionKey{ static_cast<int32_t>(r), static_cast<int32_t>(c) };
                initializeEvaluationContextForCondition(settings, ctx, key, condition);
            }
        }

        processRules<SceneryResult, SceneryRuleList>(settings, settings.sceneryRules, ctx, sceneryResultFromRulesAt, callback);
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
                         .name = FormatStringID(STR_MAPGEN_RULE_BEACHES_WATER_BODIES),
                         .conditions = std::vector{ Condition{ .enabled = true,
                                                               .type = Type::HeightRelativeToWater,
                                                               .predicate = Predicate::LessThanOrEqual,
                                                               .data = HeightData{ 2 } },
                                                    Condition{ .enabled = false,
                                                               .type = Type::DistanceToWater,
                                                               .predicate = Predicate::LessThanOrEqual,
                                                               .data = DistanceData{ 4 } } },
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
                          .type = Type::HeightRelativeToWater,
                          .predicate = Predicate::GreaterThanOrEqual,
                          .data = HeightData{ .height = 0 } };
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

    static Condition distanceToWater(const int32_t distance)
    {
        return Condition{ .enabled = true,
                          .type = Type::DistanceToWater,
                          .predicate = Predicate::LessThan,
                          .data = DistanceData{ .distance = distance } };
    }

    void createDefaultSceneryRules(Settings& settings)
    {
        std::random_device prng{};
        auto seedOffset = prng();

        settings.sceneryRules.push_back(
            SceneryRule{
                .enabled = true,
                .name = FormatStringID(STR_MAPGEN_RULE_SCENERY_SHRUBS),
                .conditions = std::vector{
                    aboveWater(),
                    blendHeight(prng(), 64, 72),
                                            blendNoise(prng(), 9.5f, 6, 0.60f, 0.70f),
                                           chance(prng(), 0.66f), },
                .effect = {
                    .objects = toSceneryEffectItemsIfAvailable(DEFAULT_SHRUB),
                    .seedOffset = 4,
                } });
        settings.sceneryRules.push_back(
            SceneryRule{
                .enabled = true,
                .name = FormatStringID(STR_MAPGEN_RULE_SCENERY_MIXED),
                .conditions = std::vector{
                    onSurface(SURFACE_SOIL),
                    chance(prng(), .33f),
                    blendHeight(prng(), 32, 42),
                    blendNoise(seedOffset, 4.0f, 5, 0.35f, 0.85f),
                },
                .effect = {
                    .objects = toSceneryEffectItemsIfAvailable(TREES_MIXED),
                    .seedOffset = 2,
                } });
        settings.sceneryRules.push_back(
            SceneryRule{ .enabled = true,
                         .name = FormatStringID(STR_MAPGEN_RULE_SCENERY_CONIFERS),
                         .conditions = std::vector{ onSurface(SURFACE_SOIL), chance(prng(), .33f), blendHeight(prng(), 54, 64),
                                                    blendNoise(seedOffset, 4.0f, 6, 0.55f, 0.80f) },
                         .effect = {
                             .objects = toSceneryEffectItemsIfAvailable(TREES_CONIFER),
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
                        onSurface(SURFACE_SOIL),
                        aboveWater(),
                        chance(prng(), .55f),
                        blendNoise(seedOffset, 8.0f, 8, 0.50f, 0.75f),
                    },
                    .effect = {
                        .objects = toSceneryEffectItemsIfAvailable(MEDITERRANEAN_SHRUBS),
                        .seedOffset = prng(),
                }});
                settings.sceneryRules.push_back(
                    SceneryRule{
                        .enabled = true,
                        .name = FormatStringID(STR_MAPGEN_RULE_SCENERY_CYPRESSES_AND_PINES),
                        .conditions = std::vector{
                            onSurface(SURFACE_SOIL),
                            chance(prng(), 0.40f),
                            blendNoise(seedOffset, 8.0f, 8, 0.50f, 1.00f),
                        },
                        .effect = {
                            .objects = toSceneryEffectItemsIfAvailable(MEDITERRANEAN_TREES),
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
                        onSurface(SURFACE_SOIL),
                        aboveWater(),
                        chance(prng(), .44f),
                        blendNoise(seedOffset,4.0f, 5, 0.3f, 0.6f),
                    },
                    .effect = {
                        .objects = toSceneryEffectItemsIfAvailable(COLD_SHRUBS),
                        .seedOffset = prng(),
                }});
                settings.sceneryRules.push_back(
                    SceneryRule{
                        .enabled = true,
                        .name = FormatStringID(STR_MAPGEN_RULE_SCENERY_CONIFERS),
                        .conditions = std::vector{
                            onSurface(SURFACE_SOIL), chance(prng(), 0.33f), blendNoise(seedOffset, 4.0f, 5, 0.35f, 0.65f),
                        },
                        .effect = {
                            .objects = toSceneryEffectItemsIfAvailable(TREES_CONIFER),
                            .seedOffset = prng(),
                        } });
                settings.sceneryRules.push_back(
                    SceneryRule{
                        .enabled = true,
                        .name = FormatStringID(STR_MAPGEN_RULE_SCENERY_CONIFERS_SNOW),
                        .conditions = std::vector{
                            onSurface(SURFACE_SNOW), chance(prng(), .33f), blendNoise(seedOffset, 4.0f, 5, 0.35f, 0.65f),
                        },
                        .effect = {
                            .objects = toSceneryEffectItemsIfAvailable(TREES_CONIFER_SNOW),
                            .seedOffset = prng(),
                        } });
                break;
            }
            case SceneryRulePreset::Arid:
            {
                auto seedOffset = prng();
                settings.sceneryRules.push_back(
                    SceneryRule{
                        .enabled = true,
                        .name = FormatStringID(STR_MAPGEN_RULE_SCENERY_OASIS),
                        .conditions = { onSurface(SURFACE_SAND), aboveWater(), chance(prng(), .55f), distanceToWater(4) },
                        .effect = {
                            .objects = toSceneryEffectItemsIfAvailable(ARID_OASIS),
                            .seedOffset = prng(),
                        } });
                settings.sceneryRules.push_back(SceneryRule{
                    .enabled = true,
                    .name = FormatStringID(STR_MAPGEN_RULE_SCENERY_SHRUBS),
                    .conditions = {
                        onSurface(SURFACE_SAND),
                        blendNoise(seedOffset,9.0f, 7, 0.62f, 1.0f),
                        chance(prng(), 0.50f),
                        aboveWater(),
                    },
                    .effect = {
                        .objects = toSceneryEffectItemsIfAvailable(ARID_SHRUBS),
                        .seedOffset = prng(),
                }});
                settings.sceneryRules.push_back(SceneryRule{
                    .enabled = true,
                    .name = FormatStringID(STR_MAPGEN_RULE_SCENERY_CACTI),
                    .conditions = {
                        onSurface(SURFACE_SAND),
                        blendNoise(seedOffset, 9.0f, 7, 0.66f, 1.00f),
                        chance(prng(), 0.33f),
                        aboveWater(),
                    },
                    .effect = {
                        .objects = toSceneryEffectItemsIfAvailable(ARID_CACTI),
                        .seedOffset = prng(),
                }});
                break;
            }
            case SceneryRulePreset::Rainforest:
            {
                for (auto& groudItem : toSceneryEffectItemsIfAvailable(RAINFOREST_GROUND))
                {
                    settings.sceneryRules.push_back(
                        SceneryRule{ .enabled = true,
                                     .name = FormatStringID(STR_MAPGEN_RULE_SCENERY_GROUND),
                                     .conditions = std::vector{ onSurface(SURFACE_SOIL), aboveWater(), chance(prng(), .25f) },
                                     .effect = {
                                         .objects = std::vector{ groudItem },
                                         .seedOffset = prng(),
                                     } });
                }
                auto flowerEffects = toSceneryEffectItemsIfAvailable(RAINFOREST_FLOWER);
                for (size_t f = 0; f < flowerEffects.size(); ++f)
                {
                    settings.sceneryRules.push_back(
                        SceneryRule{ .enabled = true,
                                     .name = FormatStringID(STR_MAPGEN_RULE_SCENERY_FLOWERS),
                                     .conditions = std::vector{ onSurface(SURFACE_SOIL), aboveWater(),
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
                        .conditions = std::vector{ onSurface(SURFACE_SOIL), aboveWater(), chance(prng(), .25f), },
                        .effect = {
                            .objects = toSceneryEffectItemsIfAvailable(RAINFOREST_SHRUB),
                            .seedOffset = prng(),
                        } });
                settings.sceneryRules.push_back(
                    SceneryRule{
                        .enabled = true,
                        .name = FormatStringID(STR_MAPGEN_RULE_SCENERY_CANOPY),
                        .conditions = std::vector{ onSurface(SURFACE_SOIL), chance(prng(), .40f), },
                        .effect = {
                            .objects = toSceneryEffectItemsIfAvailable(RAINFOREST_TREES),
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
            case Type::HeightAbsolute:
                return Condition{
                    .enabled = true, .type = type, .predicate = Predicate::GreaterThan, .data = HeightData{ .height = 2 }
                };
            case Type::HeightRelativeToWater:
                return Condition{
                    .enabled = true, .type = type, .predicate = Predicate::GreaterThan, .data = HeightData{ .height = 2 }
                };
            case Type::DistanceToWater:
                return Condition{
                    .enabled = true, .type = type, .predicate = Predicate::LessThan, .data = DistanceData{ .distance = 2 }
                };
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
