/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "MapGen.h"

#include "../../Context.h"
#include "../../GameState.h"
#include "../Map.h"
#include "../tile_element/SurfaceElement.h"
#include "BaseMap.hpp"
#include "MapHelpers.h"
#include "NoiseMapGen.h"
#include "PngTerrainGenerator.h"
#include "SceneryPlacement.h"
#include "river/River.h"

namespace OpenRCT2::World::MapGenerator
{
    static void resetSurfaces(const MapGenContext& ctx)
    {
        MapClearAllElements();
        MapInit(ctx.settings.mapSize);

        const auto& defaultRule = ctx.settings.textureRules[0];
        assert(defaultRule.isDefault);
        const auto defaultTextures = defaultRule.effect;

        for (auto y = 1; y < ctx.settings.mapSize.y - 1; y++)
        {
            for (auto x = 1; x < ctx.settings.mapSize.x - 1; x++)
            {
                auto surfaceElement = MapGetSurfaceElementAt(TileCoordsXY{ x, y });
                if (surfaceElement != nullptr)
                {
                    // setting the surface and edge textures here is somewhat redundant but won't hurt to have everything in a
                    // well known state
                    surfaceElement->setSurfaceObjectIndex(defaultTextures.landTexture);
                    surfaceElement->setEdgeObjectIndex(defaultTextures.edgeTexture);
                    surfaceElement->baseHeight = ctx.settings.heightmapLow;
                    surfaceElement->clearanceHeight = ctx.settings.heightmapLow;
                }
            }
        }
    }

    static void generateBlankMap(MapGenContext& ctx)
    {
        // todo apply a bit of noise for rivers?
        ctx.heightMap.fill(ctx.settings.heightmapLow);
    }

    static void textureRuleCallback(const TileCoordsXY& coords, const Rule::TextureResult& result)
    {
        auto* element = MapGetSurfaceElementAt(coords);

        // both land and surface textures must be set at this point due from the default rule
        if (element == nullptr || !result.landTexture.has_value() || !result.edgeTexture.has_value())
        {
            return;
        }

        element->setSurfaceObjectIndex(result.landTexture.value());
        element->setEdgeObjectIndex(result.edgeTexture.value());
    }

    static void sceneryRuleCallback(const TileCoordsXY& coords, const Rule::MaybeSceneryResult& result)
    {
        if (result.has_value())
        {
            placeScenery(coords, result.value());
        }
    }

    /**
     * Set the water level for each tile based on settings.waterLevel and riverCtx.waterLevel.
     *
     * TODO Move RiverContext.waterLevel to MapGenContext and initialize with settings.waterLevel?
     *   Would complicate the river logic but simplify this and be more flexible for plugins/lakes/...?
     *   Or have both and let generateRivers write back to MapGenContext?
     */
    static void setWaterLevel(const MapGenContext& ctx)
    {
        if(ctx.riverContext.has_value())
        {
            const River::RiverContext& riverCtx = ctx.riverContext.value();
            for (auto y = 1; y < ctx.settings.mapSize.y - 1; y++)
            {
                for (auto x = 1; x < ctx.settings.mapSize.x - 1; x++)
                {
                    const TileCoordsXY posGameMap{ x, y };
                    auto surfaceElement = MapGetSurfaceElementAt(posGameMap);
                    if (surfaceElement != nullptr)
                    {
                        int32_t waterLevel = ctx.settings.waterLevel;

                        const TileCoordsXY posGenMap = worldCoordsToGenCoords(ctx, posGameMap);
                        if (riverCtx.flags[posGenMap].has(River::river))
                        {
                            const int32_t riverHeight = quantizeHeight(riverCtx.waterLevel[posGenMap]);
                            waterLevel = std::max(waterLevel, riverHeight);
                        }

                        if (surfaceElement->baseHeight < waterLevel)
                        {
                            surfaceElement->setWaterHeight(waterLevel * kCoordsZStep);
                        }
                    }
                }
            }
        }
        else
        {
            for (auto y = 1; y < ctx.settings.mapSize.y - 1; y++)
            {
                for (auto x = 1; x < ctx.settings.mapSize.x - 1; x++)
                {
                    const TileCoordsXY posGameMap{ x, y };
                    auto surfaceElement = MapGetSurfaceElementAt(posGameMap);
                    if (surfaceElement != nullptr && surfaceElement->baseHeight < ctx.settings.waterLevel)
                    {
                        surfaceElement->setWaterHeight(ctx.settings.waterLevel * kCoordsZStep);
                    }
                }
            }
        }
    }

    /**
     * Apply the height map values to the game map tiles.
     */
    static void setMapHeight(const MapGenContext& ctx)
    {
        for (auto y = 1; y < ctx.settings.mapSize.y - 1; y++)
        {
            for (auto x = 1; x < ctx.settings.mapSize.x - 1; x++)
            {
                const TileCoordsXY posGameMap{ x, y };
                auto surfaceElement = MapGetSurfaceElementAt(posGameMap);
                if (surfaceElement != nullptr)
                {
                    const TileCoordsXY posGenMap = worldCoordsToGenCoords(ctx, posGameMap);
                    surfaceElement->baseHeight = quantizeHeight(ctx.heightMap[posGenMap]);
                    surfaceElement->clearanceHeight = surfaceElement->baseHeight;
                }
            }
        }
    }

    static void placeDebugSigns(const MapGenContext& ctx)
    {
        for (const auto& sign : ctx.debugSigns)
        {
            placeDebugSign(ctx, sign);
        }
    }

    static void generateHeightMap(MapGenContext& ctx)
    {
        switch (ctx.settings.algorithm)
        {
            case Algorithm::blank:
                generateBlankMap(ctx);
                break;

            case Algorithm::simplexNoise:
                generateSimplexMap(ctx);
                break;

            case Algorithm::warpedNoise:
                generateWarpedMap(ctx);
                break;

            case Algorithm::ridgedNoise:
                generateRidgedMap(ctx);
                break;

            case Algorithm::voronoiNoise:
                generateVoronoiMap(ctx);
                break;

            case Algorithm::heightmapImage:
                generateFromHeightmapImage(ctx);
                break;
        }
    }

    static MapGenContext createContext(const Settings& settings)
    {
        // TODO make this user configurable?
        const auto overscanFactor = (settings.algorithm == Algorithm::heightmapImage || settings.algorithm == Algorithm::blank)
            ? 1
            : River::kRiversOverscanFactor;

        const TileCoordsXY genSize{ settings.mapSize.x * overscanFactor, settings.mapSize.y * overscanFactor };

        MapGenContext ctx{ .settings = settings,
                           .dimensions = genSize,
                           .overscan = overscanFactor,
                           .overscanOffset = getWorldCoordsOffset(settings, overscanFactor),
                           .heightMap = HeightMap{ genSize },
                           .riverContext = settings.generateRivers ? std::make_optional(genSize) : std::nullopt,
                           .debugSigns = {} };

        return ctx;
    }

    void generate(const Settings& settings)
    {
        MapGenContext ctx = createContext(settings);

        // generate height map with the selected algorithm
        generateHeightMap(ctx);

         // transform the height map via filters/erosion
        applyHeightMapTransform(ctx);

        // generate rivers if enabled
        if (settings.generateRivers)
            River::generateRivers(ctx);

        // reset the game map and apply the generated height and water maps
        resetSurfaces(ctx);
        setMapHeight(ctx);
        setWaterLevel(ctx);

        // slope smooth functions operate on the game map
        applyTileSlopeSmooth(ctx);

        // place debug signs
        placeDebugSigns(ctx);

        // prepare rule evaluation context and evaluate texture and scenery rules
        Rule::EvaluationContext evalCtx{};
        Rule::initializeEvaluationContext(ctx, evalCtx);
        Rule::evaluateTextureRules(ctx, evalCtx, textureRuleCallback);
        Rule::evaluateSceneryRules(ctx, evalCtx, sceneryRuleCallback);
    }
} // namespace OpenRCT2::World::MapGenerator
