/*****************************************************************************
 * Copyright (c) 2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RuleMisc.h"

#include "../../../Context.h"
#include "../../../GameState.h"
#include "../../../localisation/Formatting.h"
#include "../../../object/ObjectEntryManager.h"
#include "../../../object/ObjectManager.h"
#include "../../Map.h"
#include "../MapGen.h"

#include <random>
#include <ranges>
#include <regex>
#include <span>

namespace OpenRCT2::World::MapGenerator::Rule
{
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

    static Condition distanceToFeature(const Feature feature, const float distance, Predicate pred = Predicate::Equal)
    {
        return Condition{ .enabled = true,
                          .type = Type::Distance,
                          .predicate = pred,
                          .data = DistanceData{ .feature = feature, .distance = distance } };
    }

    static Condition heightDeltaBetween( const HeightSource neighbour, const HeightSource self, Predicate pred = Predicate::GreaterThan, bool zRepeat = false)
    {
        return Condition{
            .enabled = true,
            .type = Type::HeightRelative,
            .predicate = pred,
            .data = HeightRelativeData{ .height = 0,
                                .sourceFirst = neighbour,
                                .sourceSecond = self},
            .zRepeat = zRepeat,
        };
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
                         .name = FormatStringID(STR_MAPGEN_RULE_WATERFALL_NW),
                         .conditions = std::vector{
                             distanceToFeature(Feature::River, 0),
                             heightDeltaBetween(HeightSource::NeighbourNWWater, HeightSource::SelfWater, Predicate::LessThan),
                         },
                         .effect = { .applyLandTexture = false,
                                     .landTexture = 0,
                                     .applyEdgeTexture = true,
                                     .edgeTexture = lookupObjectEntryIdxByIdentifier("rct2.terrain_edge.ice").value_or(0) } });
        settings.textureRules.push_back(
            TextureRule{ .enabled = true,
                         .isDefault = false,
                .name = FormatStringID(STR_MAPGEN_RULE_WATERFALL_NE),
                         .conditions = std::vector{
                             distanceToFeature(Feature::River, 0),
                             heightDeltaBetween(HeightSource::NeighbourNEWater, HeightSource::SelfWater, Predicate::LessThan),
                         },
                         .effect = { .applyLandTexture = false,
                                     .landTexture = 0,
                                     .applyEdgeTexture = true,
                                     .edgeTexture = lookupObjectEntryIdxByIdentifier("rct2.terrain_edge.ice").value_or(0) } });
        settings.textureRules.push_back(
            TextureRule{ .enabled = true,
                         .isDefault = false,
                .name = FormatStringID(STR_MAPGEN_RULE_WATERFALL_SE),
                         .conditions = std::vector{
                             distanceToFeature(Feature::River, 0),
                             heightDeltaBetween(HeightSource::NeighbourSEWater, HeightSource::SelfWater, Predicate::LessThan),
                         },
                         .effect = { .applyLandTexture = false,
                                     .landTexture = 0,
                                     .applyEdgeTexture = true,
                                     .edgeTexture = lookupObjectEntryIdxByIdentifier("rct2.terrain_edge.ice").value_or(0) } });
        settings.textureRules.push_back(
            TextureRule{ .enabled = true,
                         .isDefault = false,
                .name = FormatStringID(STR_MAPGEN_RULE_WATERFALL_SW),
                         .conditions = std::vector{
                             distanceToFeature(Feature::River, 0),
                             heightDeltaBetween(HeightSource::NeighbourSWWater, HeightSource::SelfWater, Predicate::LessThan),
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
                                                               .data = DistanceData{ .feature = Feature::Sea, .distance = 2 } },
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

    void createDefaultSceneryRules(Settings& settings)
    {
        std::random_device prng{};
        const auto seedOffset = prng();

        settings.sceneryRules.push_back(
            SceneryRule{
                .enabled = true,
                .name = FormatStringID(STR_MAPGEN_RULE_WATERFALL_NW),
                .conditions = std::vector{ distanceToFeature(Feature::River, 0),
                                           heightDeltaBetween(
                                               HeightSource::NeighbourNWWater, HeightSource::SelfWater, Predicate::GreaterThan, true) },
                .zRepeat = true,
                .effect = {
                    .objects = toSceneryEffectItemsIfAvailable(kWaterfallNw),
                    .seedOffset = 3,
                } });
        settings.sceneryRules.push_back(
            SceneryRule{
                .enabled = true,
                .name = FormatStringID(STR_MAPGEN_RULE_WATERFALL_NE),
                .conditions = std::vector{ distanceToFeature(Feature::River, 0),
                                           heightDeltaBetween(
                                               HeightSource::NeighbourNEWater, HeightSource::SelfWater, Predicate::GreaterThan, true) },
                .zRepeat = true,
                .effect = {
                    .objects = toSceneryEffectItemsIfAvailable(kWaterfallNe),
                    .seedOffset = 3,
                } });
        settings.sceneryRules.push_back(
            SceneryRule{
                .enabled = true,
                .name = FormatStringID(STR_MAPGEN_RULE_WATERFALL_SE),
                .conditions = std::vector{ distanceToFeature(Feature::River, 0),
                                           heightDeltaBetween(
                                               HeightSource::NeighbourSEWater, HeightSource::SelfWater, Predicate::GreaterThan, true) },
                .zRepeat = true,
                .effect = {
                    .objects = toSceneryEffectItemsIfAvailable(kWaterfallSe),
                    .seedOffset = 3,
                } });
        settings.sceneryRules.push_back(
            SceneryRule{
                .enabled = true,
                .name = FormatStringID(STR_MAPGEN_RULE_WATERFALL_SW),
                .conditions = std::vector{ distanceToFeature(Feature::River, 0),
                                           heightDeltaBetween(
                                               HeightSource::NeighbourSWWater, HeightSource::SelfWater, Predicate::GreaterThan, true) },
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
                            blendHeight(prng(), 44, 54),
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
            case Type::HeightAbsolute:
                return Condition{ .enabled = true,
                                  .type = type,
                                  .predicate = Predicate::GreaterThan,
                                  .data = HeightAbsoluteData{ .height = 16,
                                                      .source = HeightSource::SelfLand,
                                                      } };
            case Type::HeightRelative:
                return Condition{ .enabled = true,
                                  .type = type,
                                  .predicate = Predicate::GreaterThan,
                                  .data = HeightRelativeData{ .height = 2,
                                                      .sourceFirst = HeightSource::SelfLand,
                                                      .sourceSecond = HeightSource::GlobalWaterLevel,
                                                      } };
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
