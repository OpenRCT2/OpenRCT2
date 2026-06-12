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
#include "NoiseMapGen.h"
#include "PngTerrainGenerator.h"
#include "SceneryPlacement.h"
#include "SurfaceSelection.h"

namespace OpenRCT2::World::MapGenerator
{
    static void generateBlankMap(const MapGenCtx& context);

    static void applyTexturesFromRules(const MapGenCtx& context);
    static void placeSceneryFromRules(const MapGenCtx& context);

    void generate(Settings& settings)
    {
        MapGenCtx context{
            .settings = settings,
            .heightMap = HeightMap{settings.mapSize},
            .riverMap = settings.generateRivers ? std::make_optional(settings.mapSize) : std::nullopt,
        };

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

        applyTexturesFromRules(context);
        placeSceneryFromRules(context);
    }

    void resetSurfaces(const MapGenCtx& context)
    {
        MapClearAllElements();
        MapInit(context.settings.mapSize);

        const auto surfaceTextureId = generateSurfaceTextureId(context.settings);
        const auto edgeTextureId = generateEdgeTextureId(context.settings, surfaceTextureId);

        for (auto y = 1; y < context.settings.mapSize.y - 1; y++)
        {
            for (auto x = 1; x < context.settings.mapSize.x - 1; x++)
            {
                auto surfaceElement = MapGetSurfaceElementAt(TileCoordsXY{ x, y });
                if (surfaceElement != nullptr)
                {
                    surfaceElement->setSurfaceObjectIndex(surfaceTextureId);
                    surfaceElement->setEdgeObjectIndex(edgeTextureId);
                    surfaceElement->baseHeight = context.settings.heightmapLow;
                    surfaceElement->clearanceHeight = context.settings.heightmapLow;
                }
            }
        }
    }

    static void generateBlankMap(const MapGenCtx& context)
    {
        resetSurfaces(context);
        setWaterLevel(context);
    }

    static void applyTexturesFromRules(const MapGenCtx& context)
    {
        auto& defaultRule = context.settings.textureRules[0];
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

        Rule::evaluateTextureRules(context, callback);
    }

    static void placeSceneryFromRules(const MapGenCtx& context)
    {
        Rule::Callback<Rule::MaybeSceneryResult> callback = [](const TileCoordsXY& coords,
                                                          const Rule::MaybeSceneryResult& result) {
            if (result.has_value())
            {
                placeScenery(coords, result.value());
            }
        };

        Rule::evaluateSceneryRules(context, callback);
    }

    /**
     * Sets each tile's water level to the specified water level if underneath that water level.
     */
    void setWaterLevel(const MapGenCtx& context)
    {
        auto& gameState = getGameState();
        for (int32_t y = 1; y < gameState.mapSize.y - 1; y++)
        {
            for (int32_t x = 1; x < gameState.mapSize.x - 1; x++)
            {
                auto surfaceElement = MapGetSurfaceElementAt(TileCoordsXY{ x, y });
                if (surfaceElement != nullptr && surfaceElement->baseHeight < context.settings.waterLevel)
                    surfaceElement->setWaterHeight(context.settings.waterLevel * kCoordsZStep);
            }
        }
    }

    void setRiverWater(const MapGenCtx& context)
    {
        if (context.settings.generateRivers && context.riverMap.has_value())
        {
            RiverMap riverMap = context.riverMap.value();
            for (auto y = 1; y < riverMap.height - 1; y++)
            {
                for (auto x = 1; x < riverMap.width - 1; x++)
                {
                    TileCoordsXY pos{ x, y };

                    if (!riverMap[pos].isRiver)
                    {
                        continue;
                    }

                    auto surfaceElement = MapGetSurfaceElementAt(pos);
                    if (surfaceElement != nullptr )
                    {
                        auto waterHeight = std::max(
                            (surfaceElement->baseHeight + 2) * kCoordsZStep, surfaceElement->GetWaterHeight());
                        surfaceElement->SetWaterHeight(waterHeight);
                    }
                }
            }
        }
    }

    /**
     * Sets the height of the actual game map tiles to the height map.
     */
    void setMapHeight(const MapGenCtx& context)
    {
        for (auto y = 1; y < context.heightMap.height - 1; y++)
        {
            for (auto x = 1; x < context.heightMap.width - 1; x++)
            {
                TileCoordsXY pos{ x, y };

                auto surfaceElement = MapGetSurfaceElementAt(pos);
                if (surfaceElement == nullptr)
                    continue;

                // Ensure height is within [2, 254] and a multiple of 2
                auto adjustedHeight = std::round(std::clamp(context.heightMap[pos], 2.0f, 254.0f) * 0.5f) * 2.0f;
                surfaceElement->baseHeight = static_cast<uint8_t>(adjustedHeight);

                // TODO is this special case really needed?
                // If base height is below water level, lower it to create more natural shorelines
                if (context.settings.slopeSmooth == SlopeSmooth::strong && surfaceElement->baseHeight >= 4
                    && surfaceElement->baseHeight <= context.settings.waterLevel)
                    surfaceElement->baseHeight -= 2;

                surfaceElement->clearanceHeight = surfaceElement->baseHeight;
            }
        }
    }
} // namespace OpenRCT2::World::MapGenerator
