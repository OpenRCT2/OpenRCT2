/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "PngTerrainGenerator.h"

#include "../../Context.h"
#include "../../Diagnostic.h"
#include "../../core/Guard.hpp"
#include "../../core/Imaging.h"
#include "../../localisation/Formatter.h"
#include "../../localisation/StringIds.h"
#include "../Map.h"
#include "../tile_element/SurfaceElement.h"
#include "HeightMap.hpp"
#include "MapGen.h"
#include "MapHelpers.h"
#include "SurfaceSelection.h"

#include <algorithm>

namespace OpenRCT2::World::MapGenerator
{
    static HeightMap _heightMapData{};

    /**
     * Return the tile coordinate that matches the given pixel of a heightmap
     */
    static TileCoordsXY HeightmapCoordToTileCoordsXY(uint32_t x, uint32_t y)
    {
        // The height map does not include the empty tiles around the map, so we add 1.
        return TileCoordsXY(static_cast<int32_t>(y + 1), static_cast<int32_t>(x + 1));
    }

    bool LoadHeightmapImage(const utf8* path)
    {
        auto format = Imaging::GetImageFormatFromPath(path);
        if (format == ImageFormat::png)
        {
            // Promote to 32-bit
            format = ImageFormat::png32;
        }

        try
        {
            auto image = Imaging::ReadFromFile(path, format);
            auto width = std::min<uint32_t>(image.Width, kMaximumMapSizePractical);
            auto height = std::min<uint32_t>(image.Height, kMaximumMapSizePractical);
            if (width != image.Width || height != image.Height)
            {
                ContextShowError(STR_HEIGHT_MAP_ERROR, STR_ERROR_HEIGHT_MAP_TOO_BIG, {});
            }

            // Allocate memory for the height map values, one byte pixel
            _heightMapData = HeightMap(width, height);

            // Copy average RGB value to mono bitmap
            constexpr auto numChannels = 4;
            const auto pitch = image.Stride;
            const auto pixels = image.Pixels.data();
            for (uint32_t x = 0; x < _heightMapData.width; x++)
            {
                for (uint32_t y = 0; y < _heightMapData.height; y++)
                {
                    const auto red = pixels[x * numChannels + y * pitch];
                    const auto green = pixels[x * numChannels + y * pitch + 1];
                    const auto blue = pixels[x * numChannels + y * pitch + 2];
                    _heightMapData[TileCoordsXY(x, y)] = (red + green + blue) / 3;
                }
            }
            return true;
        }
        catch (const std::exception& e)
        {
            switch (format)
            {
                case ImageFormat::bitmap:
                    ContextShowError(STR_HEIGHT_MAP_ERROR, STR_ERROR_READING_BITMAP, {});
                    break;
                case ImageFormat::png32:
                    ContextShowError(STR_HEIGHT_MAP_ERROR, STR_ERROR_READING_PNG, {});
                    break;
                default:
                    LOG_ERROR("Unable to load height map image: %s", e.what());
                    break;
            }
            return false;
        }
    }

    /**
     * Frees the memory used to store the selected height map
     */
    void UnloadHeightmapImage()
    {
        _heightMapData.clear();
    }

    /**
     * Applies box blur to the surface N times
     */
    static void SmoothHeightmap(HeightMap& src, int32_t strength)
    {
        // Create buffer to store one channel
        HeightMap temp{ src.width, src.height };

        for (int32_t i = 0; i < strength; i++)
        {
            // Calculate box blur value to all pixels of the surface
            for (auto y = 0; y < temp.height; y++)
            {
                for (auto x = 0; x < temp.width; x++)
                {
                    uint32_t heightSum = 0;

                    // Loop over neighbour pixels, all of them have the same weight
                    for (int8_t offsetX = -1; offsetX <= 1; offsetX++)
                    {
                        for (int8_t offsetY = -1; offsetY <= 1; offsetY++)
                        {
                            // Clamp x and y so they stay within the image
                            // This assumes the height map is not tiled, and increases the weight of the edges
                            const auto readX = std::clamp<int32_t>(x + offsetX, 0, temp.width - 1);
                            const auto readY = std::clamp<int32_t>(y + offsetY, 0, temp.height - 1);
                            heightSum += src[{ readX, readY }];
                        }
                    }

                    // Take average
                    temp[{ x, y }] = heightSum / 9;
                }
            }

            // Now copy the blur to the source pixels
            for (auto y = 0; y < temp.height; y++)
            {
                for (auto x = 0; x < temp.width; x++)
                {
                    src[{ x, y }] = temp[{ x, y }];
                }
            }
        }
    }

    void GenerateFromHeightmapImage(Settings* settings)
    {
        Guard::Assert(!_heightMapData.empty(), "No height map loaded");
        Guard::Assert(settings->heightmapHigh != settings->heightmapLow, "Low and high setting cannot be the same");

        // Make a copy of the original height map that we can edit
        HeightMap dest = _heightMapData;

        // Get technical map size, +2 for the black tiles around the map
        auto mapWidth = static_cast<int32_t>(dest.width + 2);
        auto mapHeight = static_cast<int32_t>(dest.height + 2);

        // The x and y axis are flipped in the world, so this uses y for x and x for y.
        TileCoordsXY flippedMapSize{ mapHeight, mapWidth };
        MapInit(flippedMapSize);

        if (settings->smooth_height_map)
        {
            SmoothHeightmap(dest, settings->smooth_strength);
        }

        uint8_t maxValue = 255;
        uint8_t minValue = 0;

        if (settings->normalize_height)
        {
            // Get highest and lowest pixel value
            maxValue = 0;
            minValue = 255;
            for (auto y = 0; y < dest.height; y++)
            {
                for (auto x = 0; x < dest.width; x++)
                {
                    uint8_t value = dest[{ x, y }];
                    maxValue = std::max(maxValue, value);
                    minValue = std::min(minValue, value);
                }
            }

            if (minValue == maxValue)
            {
                ContextShowError(STR_HEIGHT_MAP_ERROR, STR_ERROR_CANNOT_NORMALIZE, {});
                return;
            }
        }

        Guard::Assert(maxValue > minValue, "Input range is invalid");
        Guard::Assert(settings->heightmapHigh > settings->heightmapLow, "Output range is invalid");

        const auto surfaceTextureId = generateSurfaceTextureId(settings);
        const auto edgeTextureId = generateEdgeTextureId(settings, surfaceTextureId);

        const uint8_t rangeIn = maxValue - minValue;
        const uint8_t rangeOut = (settings->heightmapHigh - settings->heightmapLow) * 2;

        for (auto y = 0; y < dest.height; y++)
        {
            for (auto x = 0; x < dest.width; x++)
            {
                auto tileCoords = HeightmapCoordToTileCoordsXY(x, y);
                auto* const surfaceElement = MapGetSurfaceElementAt(tileCoords);
                if (surfaceElement == nullptr)
                    continue;

                // Read value from bitmap, and convert its range
                uint8_t value = dest[{ x, y }];
                value = static_cast<uint8_t>(static_cast<float>(value - minValue) / rangeIn * rangeOut)
                    + (settings->heightmapLow * 2);
                surfaceElement->BaseHeight = value;

                // Floor to even number
                surfaceElement->BaseHeight /= 2;
                surfaceElement->BaseHeight *= 2;
                surfaceElement->ClearanceHeight = surfaceElement->BaseHeight;

                // Set textures
                surfaceElement->SetSurfaceObjectIndex(surfaceTextureId);
                surfaceElement->SetEdgeObjectIndex(edgeTextureId);

                // Set water level
                if (surfaceElement->BaseHeight < settings->waterLevel)
                {
                    surfaceElement->SetWaterHeight(settings->waterLevel * kCoordsZStep);
                }
            }
        }

        // Smooth tile edges
        if (settings->smoothTileEdges)
        {
            // Set the tile slopes so that there are no cliffs
            smoothMap(flippedMapSize, smoothTileWeak);
        }
    }
} // namespace OpenRCT2::World::MapGenerator
