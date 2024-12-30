/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "MapGen.h"

#include "../../Context.h"
#include "../../GameState.h"
#include "../../object/ObjectManager.h"
#include "../../util/Util.h"
#include "../tile_element/Slope.h"
#include "../tile_element/SurfaceElement.h"
#include "HeightMap.hpp"
#include "MapHelpers.h"
#include "PngTerrainGenerator.h"
#include "SimplexNoise.h"
#include "SurfaceSelection.h"
#include "TreePlacement.h"

#include <vector>

namespace OpenRCT2::World::MapGenerator
{
    static void generateBlankMap(Settings* settings);
    static void generateSimplexMap(Settings* settings);

    static void addBeaches(Settings* settings);

    void generate(Settings* settings)
    {
        // First, generate the height map
        switch (settings->algorithm)
        {
            case Algorithm::blank:
                generateBlankMap(settings);
                break;

            case Algorithm::simplexNoise:
                generateSimplexMap(settings);
                break;

            case Algorithm::heightmapImage:
                GenerateFromHeightmapImage(settings);
                break;
        }

        // Add beaches?
        if (settings->beaches)
            addBeaches(settings);

        // Place trees?
        if (settings->trees)
            placeTrees(settings);
    }

    static void setWaterLevel(int32_t waterLevel);
    static void smoothHeightMap(int32_t iterations, HeightMap& heightMap);
    static void setMapHeight(Settings* settings, const HeightMap& heightMap);

    static void resetSurfaces(Settings* settings)
    {
        MapClearAllElements();
        MapInit(settings->mapSize);

        const auto surfaceTextureId = generateSurfaceTextureId(settings);
        const auto edgeTextureId = generateEdgeTextureId(settings, surfaceTextureId);

        for (auto y = 1; y < settings->mapSize.y - 1; y++)
        {
            for (auto x = 1; x < settings->mapSize.x - 1; x++)
            {
                auto surfaceElement = MapGetSurfaceElementAt(TileCoordsXY{ x, y });
                if (surfaceElement != nullptr)
                {
                    surfaceElement->SetSurfaceObjectIndex(surfaceTextureId);
                    surfaceElement->SetEdgeObjectIndex(edgeTextureId);
                    surfaceElement->BaseHeight = settings->heightmapLow;
                    surfaceElement->ClearanceHeight = settings->heightmapLow;
                }
            }
        }
    }

    static void generateBlankMap(Settings* settings)
    {
        resetSurfaces(settings);
        setWaterLevel(settings->waterLevel);
    }

    static void generateSimplexNoise(Settings* settings, HeightMap& heightMap);

    static void generateSimplexMap(Settings* settings)
    {
        resetSurfaces(settings);

        // Create the temporary height map and initialise
        auto& mapSize = settings->mapSize;
        auto heightMap = HeightMap(mapSize.x * 2, mapSize.y * 2);

        generateSimplexNoise(settings, heightMap);
        smoothHeightMap(2 + (UtilRand() % 6), heightMap);

        // Set the game map to the height map
        setMapHeight(settings, heightMap);

        if (settings->smoothTileEdges)
        {
            // Set the tile slopes so that there are no cliffs
            while (MapSmooth(1, 1, mapSize.x - 1, mapSize.y - 1))
            {
            }
        }

        // Add the water
        setWaterLevel(settings->waterLevel);
    }

    static void addBeaches(Settings* settings)
    {
        auto beachTextureId = generateBeachTextureId();
        if (beachTextureId == OBJECT_ENTRY_INDEX_NULL)
            return;

        // Add sandy beaches
        const auto& mapSize = settings->mapSize;
        for (auto y = 1; y < mapSize.y - 1; y++)
        {
            for (auto x = 1; x < mapSize.x - 1; x++)
            {
                auto surfaceElement = MapGetSurfaceElementAt(TileCoordsXY{ x, y });

                if (surfaceElement != nullptr && surfaceElement->BaseHeight < settings->waterLevel + 6)
                    surfaceElement->SetSurfaceObjectIndex(beachTextureId);
            }
        }
    }

    /**
     * Sets each tile's water level to the specified water level if underneath that water level.
     */
    static void setWaterLevel(int32_t waterLevel)
    {
        auto& gameState = GetGameState();
        for (int32_t y = 1; y < gameState.MapSize.y - 1; y++)
        {
            for (int32_t x = 1; x < gameState.MapSize.x - 1; x++)
            {
                auto surfaceElement = MapGetSurfaceElementAt(TileCoordsXY{ x, y });
                if (surfaceElement != nullptr && surfaceElement->BaseHeight < waterLevel)
                    surfaceElement->SetWaterHeight(waterLevel * kCoordsZStep);
            }
        }
    }

    /**
     * Smooths the height map.
     */
    static void smoothHeightMap(int32_t iterations, HeightMap& heightMap)
    {
        for (auto i = 0; i < iterations; i++)
        {
            auto copyHeight = heightMap;
            for (auto y = 1; y < heightMap.height - 1; y++)
            {
                for (auto x = 1; x < heightMap.width - 1; x++)
                {
                    auto avg = 0;
                    for (auto yy = -1; yy <= 1; yy++)
                    {
                        for (auto xx = -1; xx <= 1; xx++)
                        {
                            avg += copyHeight[{ y + yy, x + xx }];
                        }
                    }
                    avg /= 9;
                    heightMap[{ x, y }] = avg;
                }
            }
        }
    }

    /**
     * Sets the height of the actual game map tiles to the height map.
     */
    static void setMapHeight(Settings* settings, const HeightMap& heightMap)
    {
        for (auto y = 1; y < heightMap.height / 2 - 1; y++)
        {
            for (auto x = 1; x < heightMap.width / 2 - 1; x++)
            {
                auto heightX = x * 2;
                auto heightY = y * 2;

                uint8_t q00 = heightMap[{ heightX + 0, heightY + 0 }];
                uint8_t q01 = heightMap[{ heightX + 0, heightY + 1 }];
                uint8_t q10 = heightMap[{ heightX + 1, heightY + 0 }];
                uint8_t q11 = heightMap[{ heightX + 1, heightY + 1 }];

                uint8_t baseHeight = (q00 + q01 + q10 + q11) / 4;

                auto surfaceElement = MapGetSurfaceElementAt(TileCoordsXY{ x, y });
                if (surfaceElement == nullptr)
                    continue;
                surfaceElement->BaseHeight = std::max(2, baseHeight * 2);

                // If base height is below water level, lower it to create more natural shorelines
                if (surfaceElement->BaseHeight >= 4 && surfaceElement->BaseHeight <= settings->waterLevel)
                    surfaceElement->BaseHeight -= 2;

                surfaceElement->ClearanceHeight = surfaceElement->BaseHeight;

                uint8_t currentSlope = surfaceElement->GetSlope();

                if (q00 > baseHeight)
                    currentSlope |= kTileSlopeSCornerUp;
                if (q01 > baseHeight)
                    currentSlope |= kTileSlopeWCornerUp;
                if (q10 > baseHeight)
                    currentSlope |= kTileSlopeECornerUp;
                if (q11 > baseHeight)
                    currentSlope |= kTileSlopeNCornerUp;

                surfaceElement->SetSlope(currentSlope);
            }
        }
    }

    static void generateSimplexNoise(Settings* settings, HeightMap& heightMap)
    {
        float freq = settings->simplex_base_freq / 100.0f * (1.0f / heightMap.width);
        int32_t octaves = settings->simplex_octaves;

        int32_t low = settings->heightmapLow / 2;
        int32_t high = settings->heightmapHigh / 2 - low;

        NoiseRand();
        for (int32_t y = 0; y < heightMap.height; y++)
        {
            for (int32_t x = 0; x < heightMap.width; x++)
            {
                float noiseValue = std::clamp(FractalNoise(x, y, freq, octaves, 2.0f, 0.65f), -1.0f, 1.0f);
                float normalisedNoiseValue = (noiseValue + 1.0f) / 2.0f;

                heightMap[{ x, y }] = low + static_cast<int32_t>(normalisedNoiseValue * high);
            }
        }
    }
} // namespace OpenRCT2::World::MapGenerator
