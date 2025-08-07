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
#include "../tile_element/SurfaceElement.h"
#include "HeightMap.hpp"
#include "NoiseMapGen.h"
#include "PngTerrainGenerator.h"
#include "SurfaceSelection.h"
#include "TreePlacement.h"

namespace OpenRCT2::World::MapGenerator
{
    static void generateBlankMap(Settings* settings);

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

            case Algorithm::warpedNoise:
                generateWarpedMap(settings);
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

    void resetSurfaces(Settings* settings)
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

    static void addBeaches(Settings* settings)
    {
        auto beachTextureId = generateBeachTextureId(settings);
        if (beachTextureId == kObjectEntryIndexNull)
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
    void setWaterLevel(int32_t waterLevel)
    {
        auto& gameState = getGameState();
        for (int32_t y = 1; y < gameState.mapSize.y - 1; y++)
        {
            for (int32_t x = 1; x < gameState.mapSize.x - 1; x++)
            {
                auto surfaceElement = MapGetSurfaceElementAt(TileCoordsXY{ x, y });
                if (surfaceElement != nullptr && surfaceElement->BaseHeight < waterLevel)
                    surfaceElement->SetWaterHeight(waterLevel * kCoordsZStep);
            }
        }
    }

    /**
     * Sets the height of the actual game map tiles to the height map.
     */
    void setMapHeight(Settings* settings, const HeightMap& heightMap)
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
                surfaceElement->BaseHeight = static_cast<uint8_t>(adjustedHeight);

                // If base height is below water level, lower it to create more natural shorelines
                if (!settings->simulate_erosion && surfaceElement->BaseHeight >= 4
                    && surfaceElement->BaseHeight <= settings->waterLevel)
                    surfaceElement->BaseHeight -= 2;

                surfaceElement->ClearanceHeight = surfaceElement->BaseHeight;
            }
        }
    }
} // namespace OpenRCT2::World::MapGenerator
