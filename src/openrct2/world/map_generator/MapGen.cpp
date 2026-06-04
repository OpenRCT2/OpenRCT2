/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "MapGen.h"

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
    static void generateBlankMap(Settings& settings);

    static void applyTexturesFromRules(const Settings& settings);
    static void placeSceneryFromRules(const Settings& settings);

    void generate(Settings& settings)
    {
        switch (settings.algorithm)
        {
            case Algorithm::blank:
                generateBlankMap(settings);
                break;

            case Algorithm::simplexNoise:
                generateSimplexMap(settings);
                break;

            case Algorithm::warpedNoise:
                generateWarpedMap(settings);
                break;

            case Algorithm::ridgedNoise:
                generateRidgedMap(settings);
                break;

            case Algorithm::voronoiNoise:
                generateVoronoiMap(settings);
                break;

            case Algorithm::heightmapImage:
                GenerateFromHeightmapImage(settings);
                break;
        }

        applyTexturesFromRules(settings);
        placeSceneryFromRules(settings);
    }

    void resetSurfaces(Settings& settings)
    {
        MapClearAllElements();
        MapInit(settings.mapSize);

        const auto surfaceTextureId = generateSurfaceTextureId(settings);
        const auto edgeTextureId = generateEdgeTextureId(settings, surfaceTextureId);

        for (auto y = 1; y < settings.mapSize.y - 1; y++)
        {
            for (auto x = 1; x < settings.mapSize.x - 1; x++)
            {
                auto surfaceElement = MapGetSurfaceElementAt(TileCoordsXY{ x, y });
                if (surfaceElement != nullptr)
                {
                    surfaceElement->setSurfaceObjectIndex(surfaceTextureId);
                    surfaceElement->setEdgeObjectIndex(edgeTextureId);
                    surfaceElement->baseHeight = settings.heightmapLow;
                    surfaceElement->clearanceHeight = settings.heightmapLow;
                }
            }
        }
    }

    static void generateBlankMap(Settings& settings)
    {
        resetSurfaces(settings);
        setWaterLevel(settings.waterLevel);
    }

    static void applyTexturesFromRules(const Settings& settings)
    {
        auto& defaultRule = settings.textureRules[0];
        assert(defaultRule.isDefault);
        auto defaultTextures = defaultRule.effect;

        Rule::Callback<Rule::TextureResult> callback =
            [defaultTextures](const TileCoordsXY& coords, const std::optional<Rule::TextureResult>& result) {
                auto* element = MapGetSurfaceElementAt(coords);
                if (element == nullptr)
                {
                    return;
                }

                auto actual = result.value_or(defaultTextures);

                element->SetSurfaceObjectIndex(actual.applyLandTexture ? actual.landTexture : defaultTextures.landTexture);
                element->SetEdgeObjectIndex(actual.applyEdgeTexture ? actual.edgeTexture : defaultTextures.edgeTexture);
        };

        Rule::evaluateTextureRules(settings, callback);
    }

    static void placeSceneryFromRules(const Settings& settings)
    {
        Rule::Callback<Rule::SceneryResult> callback = [](const TileCoordsXY& coords,
                                                          const std::optional<Rule::SceneryResult>& result) {
            if (result.has_value())
            {
                placeScenery(coords, result.value());
            }
        };

        Rule::evaluateSceneryRules(settings, callback);
    }

    /**
     * Sets each tile's water level to the specified water level if underneath that water level.
     */
    void setWaterLevel(int32_t waterLevel)
    {
        auto& gameState = getGameState();
        for (int32_t y = 1; y < gameState.mapSize.y - 1; y++)
        {
            for (int32_t x = 1; x < gameState.mapSize.x - 1; x++)
            {
                auto surfaceElement = MapGetSurfaceElementAt(TileCoordsXY{ x, y });
                if (surfaceElement != nullptr && surfaceElement->baseHeight < waterLevel)
                    surfaceElement->setWaterHeight(waterLevel * kCoordsZStep);
            }
        }
    }

    /**
     * Sets the height of the actual game map tiles to the height map.
     */
    void setMapHeight(const Settings& settings, const HeightMap& heightMap)
    {
        for (auto y = 1; y < heightMap.height - 1; y++)
        {
            for (auto x = 1; x < heightMap.width - 1; x++)
            {
                TileCoordsXY pos{ x, y };

                auto surfaceElement = MapGetSurfaceElementAt(pos);
                if (surfaceElement == nullptr)
                    continue;

                // Ensure height is within [2, 254] and a multiple of 2
                auto adjustedHeight = std::round(std::clamp(heightMap[pos], 2.0f, 254.0f) * 0.5f) * 2.0f;
                surfaceElement->baseHeight = static_cast<uint8_t>(adjustedHeight);

                // TODO is this special case really needed?
                // If base height is below water level, lower it to create more natural shorelines
                if (settings.slopeSmooth == SlopeSmooth::strong && surfaceElement->baseHeight >= 4
                    && surfaceElement->baseHeight <= settings.waterLevel)
                    surfaceElement->baseHeight -= 2;

                surfaceElement->clearanceHeight = surfaceElement->baseHeight;

                uint8_t currentSlope = surfaceElement->getSlope();

                if (q00 > baseHeight)
                    currentSlope |= kTileSlopeSCornerUp;
                if (q01 > baseHeight)
                    currentSlope |= kTileSlopeWCornerUp;
                if (q10 > baseHeight)
                    currentSlope |= kTileSlopeECornerUp;
                if (q11 > baseHeight)
                    currentSlope |= kTileSlopeNCornerUp;

                surfaceElement->setSlope(currentSlope);
            }
        }
    }
} // namespace OpenRCT2::World::MapGenerator
