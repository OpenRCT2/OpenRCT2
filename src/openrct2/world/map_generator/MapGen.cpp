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
    static void smoothHeight(int32_t iterations);
    static void setHeight(Settings* settings);

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

    static TileCoordsXY _heightSize;
    static uint8_t* _height;

    static int32_t getHeight(int32_t x, int32_t y)
    {
        if (x >= 0 && y >= 0 && x < _heightSize.x && y < _heightSize.y)
            return _height[x + y * _heightSize.x];

        return 0;
    }

    static void setHeight(int32_t x, int32_t y, int32_t height)
    {
        if (x >= 0 && y >= 0 && x < _heightSize.x && y < _heightSize.y)
            _height[x + y * _heightSize.x] = height;
    }

    static void generateSimplexNoise(Settings* settings);

    static void generateSimplexMap(Settings* settings)
    {
        resetSurfaces(settings);

        // Create the temporary height map and initialise
        auto& mapSize = settings->mapSize;
        _heightSize = { mapSize.x * 2, mapSize.y * 2 };
        _height = new uint8_t[_heightSize.y * _heightSize.x];
        std::fill_n(_height, _heightSize.y * _heightSize.x, 0x00);

        generateSimplexNoise(settings);
        smoothHeight(2 + (UtilRand() % 6));

        // Set the game map to the height map
        setHeight(settings);
        delete[] _height;

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
    static void smoothHeight(int32_t iterations)
    {
        int32_t i, x, y, xx, yy, avg;
        int32_t arraySize = _heightSize.y * _heightSize.x * sizeof(uint8_t);
        uint8_t* copyHeight = new uint8_t[arraySize];

        for (i = 0; i < iterations; i++)
        {
            std::memcpy(copyHeight, _height, arraySize);
            for (y = 1; y < _heightSize.y - 1; y++)
            {
                for (x = 1; x < _heightSize.x - 1; x++)
                {
                    avg = 0;
                    for (yy = -1; yy <= 1; yy++)
                    {
                        for (xx = -1; xx <= 1; xx++)
                        {
                            avg += copyHeight[(y + yy) * _heightSize.x + (x + xx)];
                        }
                    }
                    avg /= 9;
                    setHeight(x, y, avg);
                }
            }
        }

        delete[] copyHeight;
    }

    /**
     * Sets the height of the actual game map tiles to the height map.
     */
    static void setHeight(Settings* settings)
    {
        int32_t x, y, heightX, heightY;

        for (y = 1; y < _heightSize.y / 2 - 1; y++)
        {
            for (x = 1; x < _heightSize.x / 2 - 1; x++)
            {
                heightX = x * 2;
                heightY = y * 2;

                uint8_t q00 = getHeight(heightX + 0, heightY + 0);
                uint8_t q01 = getHeight(heightX + 0, heightY + 1);
                uint8_t q10 = getHeight(heightX + 1, heightY + 0);
                uint8_t q11 = getHeight(heightX + 1, heightY + 1);

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

    static void generateSimplexNoise(Settings* settings)
    {
        float freq = settings->simplex_base_freq / 100.0f * (1.0f / _heightSize.x);
        int32_t octaves = settings->simplex_octaves;

        int32_t low = settings->heightmapLow / 2;
        int32_t high = settings->heightmapHigh / 2 - low;

        NoiseRand();
        for (int32_t y = 0; y < _heightSize.y; y++)
        {
            for (int32_t x = 0; x < _heightSize.x; x++)
            {
                float noiseValue = std::clamp(FractalNoise(x, y, freq, octaves, 2.0f, 0.65f), -1.0f, 1.0f);
                float normalisedNoiseValue = (noiseValue + 1.0f) / 2.0f;

                setHeight(x, y, low + static_cast<int32_t>(normalisedNoiseValue * high));
            }
        }
    }
} // namespace OpenRCT2::World::MapGenerator
