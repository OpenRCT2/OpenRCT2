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
#include "SurfaceSelection.h"

namespace OpenRCT2::World::MapGenerator
{
    static void generateBlankMap(const MapGenContext& ctx);
    static void applyTexturesFromRules(const MapGenContext& ctx);
    static void placeSceneryFromRules(const MapGenContext& ctx);
    static void placeDebugSigns(const MapGenContext& ctx);

    void generate(const Settings& settings)
    {
        const TileCoordsXY genSize{ settings.mapSize.x * Hydro::kRiversOverscanFactor,
                                    settings.mapSize.y * Hydro::kRiversOverscanFactor };

        MapGenContext context{ .settings = settings,
                           .dimensions = genSize,
                           .overscanOffset = getWorldCoordsOffset(settings),
                           .heightMap = HeightMap{ genSize },
                           .hydroContext = settings.generateRivers ? std::make_optional(genSize) : std::nullopt,
                           .debugSigns = {} };

        switch (settings.algorithm)
        {
            case Algorithm::blank:
                generateBlankMap(context);
                break;

            case Algorithm::simplexNoise:
                generateSimplexMap(context);
                break;

            case Algorithm::warpedNoise:
                generateWarpedMap(context);
                break;

            case Algorithm::ridgedNoise:
                generateRidgedMap(context);
                break;

            case Algorithm::voronoiNoise:
                generateVoronoiMap(context);
                break;

            case Algorithm::heightmapImage:
                GenerateFromHeightmapImage(context);
                break;
        }

        placeDebugSigns(context);

        applyTexturesFromRules(context);
        placeSceneryFromRules(context);
    }

    void resetSurfaces(const MapGenContext& ctx)
    {
        MapClearAllElements();
        MapInit(ctx.settings.mapSize);

        const auto surfaceTextureId = generateSurfaceTextureId(ctx.settings);
        const auto edgeTextureId = generateEdgeTextureId(ctx.settings, surfaceTextureId);

        for (auto y = 1; y < ctx.settings.mapSize.y - 1; y++)
        {
            for (auto x = 1; x < ctx.settings.mapSize.x - 1; x++)
            {
                auto surfaceElement = MapGetSurfaceElementAt(TileCoordsXY{ x, y });
                if (surfaceElement != nullptr)
                {
                    surfaceElement->setSurfaceObjectIndex(surfaceTextureId);
                    surfaceElement->setEdgeObjectIndex(edgeTextureId);
                    surfaceElement->baseHeight = ctx.settings.heightmapLow;
                    surfaceElement->clearanceHeight = ctx.settings.heightmapLow;
                }
            }
        }
    }

    static void generateBlankMap(const MapGenContext& ctx)
    {
        resetSurfaces(ctx);
        setWaterLevel(ctx);
    }

    static void applyTexturesFromRules(const MapGenContext& ctx)
    {
        auto& defaultRule = ctx.settings.textureRules[0];
        assert(defaultRule.isDefault);
        auto defaultTextures = defaultRule.effect;

        Rule::Callback<Rule::TextureResult> callback =
            [defaultTextures](const TileCoordsXY& coords, const Rule::TextureResult& result) {
                auto* element = MapGetSurfaceElementAt(coords);
                if (element == nullptr)
                {
                    return;
                }

                element->setSurfaceObjectIndex(result.landTexture.value_or(defaultTextures.landTexture));
                element->setEdgeObjectIndex(result.edgeTexture.value_or(defaultTextures.edgeTexture));
            };

        Rule::evaluateTextureRules(ctx, callback);
    }

    static void placeSceneryFromRules(const MapGenContext& ctx)
    {
        Rule::Callback<Rule::MaybeSceneryResult> callback = [](const TileCoordsXY& coords,
                                                               const Rule::MaybeSceneryResult& result) {
            if (result.has_value())
            {
                placeScenery(coords, result.value());
            }
        };

        Rule::evaluateSceneryRules(ctx, callback);
    }

    /**
     * Sets each tile's water level to the specified water level if underneath that water level.
     */
    void setWaterLevel(const MapGenContext& ctx)
    {
        auto& gameState = getGameState();
        for (int32_t y = 1; y < gameState.mapSize.y - 1; y++)
        {
            for (int32_t x = 1; x < gameState.mapSize.x - 1; x++)
            {
                auto surfaceElement = MapGetSurfaceElementAt(TileCoordsXY{ x, y });
                if (surfaceElement != nullptr && surfaceElement->baseHeight < ctx.settings.waterLevel)
                    surfaceElement->setWaterHeight(ctx.settings.waterLevel * kCoordsZStep);
            }
        }
    }

    void setRiverWater(const MapGenContext& ctx)
    {
        if (ctx.settings.generateRivers && ctx.hydroContext.has_value())
        {
            Hydro::HydroContext hydroMaps = ctx.hydroContext.value();

            for (auto y = 1; y < ctx.settings.mapSize.y - 1; y++)
            {
                for (auto x = 1; x < ctx.settings.mapSize.x - 1; x++)
                {
                    const TileCoordsXY posGameMap{ x, y };
                    const TileCoordsXY posGenMap = worldCoordsToGenCoords(ctx, posGameMap);

                    if (!hydroMaps.flags[posGenMap].has(Hydro::river))
                    {
                        continue;
                    }

                    auto surfaceElement = MapGetSurfaceElementAt(posGameMap);
                    if (surfaceElement != nullptr)
                    {
                        const int32_t riverHeight = quantizeHeight(hydroMaps.height[posGenMap]) * kCoordsZStep;
                        const int32_t waterTableHeight = surfaceElement->GetWaterHeight();
                        const int32_t waterHeight = std::max(riverHeight, waterTableHeight);

                        surfaceElement->SetWaterHeight(waterHeight);
                    }
                }
            }
        }
    }

    /**
     * Sets the height of the actual game map tiles to the height map.
     */
    void setMapHeight(const MapGenContext& ctx)
    {
        for (auto y = 1; y < ctx.settings.mapSize.y - 1; y++)
        {
            for (auto x = 1; x < ctx.settings.mapSize.x - 1; x++)
            {
                const TileCoordsXY posGameMap{ x, y };
                const TileCoordsXY posGenMap = worldCoordsToGenCoords(ctx, posGameMap);

                auto surfaceElement = MapGetSurfaceElementAt(posGameMap);
                if (surfaceElement == nullptr)
                    continue;

                surfaceElement->baseHeight = quantizeHeight(ctx.heightMap[posGenMap]);

                // TODO is this special case really needed?
                // If base height is below water level, lower it to create more natural shorelines
                if (ctx.settings.slopeSmooth == SlopeSmooth::strong && surfaceElement->baseHeight >= 4
                    && surfaceElement->baseHeight <= ctx.settings.waterLevel)
                    surfaceElement->baseHeight -= 2;

                surfaceElement->clearanceHeight = surfaceElement->baseHeight;
            }
        }
    }

    void placeDebugSigns(const MapGenContext& ctx)
    {
        for (const auto& sign : ctx.debugSigns)
        {
            placeDebugSign(ctx, sign);
        }
    }
} // namespace OpenRCT2::World::MapGenerator
