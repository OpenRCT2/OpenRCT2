/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "PngTerrainGenerator.h"

#include "../../Context.h"
#include "../../Diagnostic.h"
#include "../../core/Imaging.h"
#include "../../localisation/Formatter.h"
#include "../../localisation/StringIds.h"
#include "../Map.h"
#include "../tile_element/SurfaceElement.h"
#include "MapGen.h"
#include "MapHelpers.h"
#include "SurfaceSelection.h"

#include <algorithm>

namespace OpenRCT2::World::MapGenerator
{
    static struct
    {
        uint32_t width = 0;
        uint32_t height = 0;
        std::vector<uint8_t> mono_bitmap;
    } _heightMapData;

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
        if (format == IMAGE_FORMAT::PNG)
        {
            // Promote to 32-bit
            format = IMAGE_FORMAT::PNG_32;
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
            _heightMapData.mono_bitmap.resize(width * height);
            _heightMapData.width = width;
            _heightMapData.height = height;

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
                    _heightMapData.mono_bitmap[x + y * _heightMapData.width] = (red + green + blue) / 3;
                }
            }
            return true;
        }
        catch (const std::exception& e)
        {
            switch (format)
            {
                case IMAGE_FORMAT::BITMAP:
                    ContextShowError(STR_HEIGHT_MAP_ERROR, STR_ERROR_READING_BITMAP, {});
                    break;
                case IMAGE_FORMAT::PNG_32:
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
        _heightMapData.mono_bitmap.clear();
        _heightMapData.width = 0;
        _heightMapData.height = 0;
    }

    /**
     * Applies box blur to the surface N times
     */
    static void SmoothHeightmap(std::vector<uint8_t>& src, int32_t strength)
    {
        // Create buffer to store one channel
        std::vector<uint8_t> dest(src.size());

        for (int32_t i = 0; i < strength; i++)
        {
            // Calculate box blur value to all pixels of the surface
            for (uint32_t y = 0; y < _heightMapData.height; y++)
            {
                for (uint32_t x = 0; x < _heightMapData.width; x++)
                {
                    uint32_t heightSum = 0;

                    // Loop over neighbour pixels, all of them have the same weight
                    for (int8_t offsetX = -1; offsetX <= 1; offsetX++)
                    {
                        for (int8_t offsetY = -1; offsetY <= 1; offsetY++)
                        {
                            // Clamp x and y so they stay within the image
                            // This assumes the height map is not tiled, and increases the weight of the edges
                            const int32_t readX = std::clamp<int32_t>(x + offsetX, 0, _heightMapData.width - 1);
                            const int32_t readY = std::clamp<int32_t>(y + offsetY, 0, _heightMapData.height - 1);
                            heightSum += src[readX + readY * _heightMapData.width];
                        }
                    }

                    // Take average
                    dest[x + y * _heightMapData.width] = heightSum / 9;
                }
            }

            // Now apply the blur to the source pixels
            for (uint32_t y = 0; y < _heightMapData.height; y++)
            {
                for (uint32_t x = 0; x < _heightMapData.width; x++)
                {
                    src[x + y * _heightMapData.width] = dest[x + y * _heightMapData.width];
                }
            }
        }
    }

    void GenerateFromHeightmapImage(Settings* settings)
    {
        Guard::Assert(!_heightMapData.mono_bitmap.empty(), "No height map loaded");
        Guard::Assert(settings->heightmapHigh != settings->heightmapLow, "Low and high setting cannot be the same");

        // Make a copy of the original height map that we can edit
        auto dest = _heightMapData.mono_bitmap;

        // Get technical map size, +2 for the black tiles around the map
        auto maxWidth = static_cast<int32_t>(_heightMapData.width + 2);
        auto maxHeight = static_cast<int32_t>(_heightMapData.height + 2);
        MapInit({ maxHeight, maxWidth });

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
            minValue = 0xff;
            for (uint32_t y = 0; y < _heightMapData.height; y++)
            {
                for (uint32_t x = 0; x < _heightMapData.width; x++)
                {
                    uint8_t value = dest[x + y * _heightMapData.width];
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

        for (uint32_t y = 0; y < _heightMapData.height; y++)
        {
            for (uint32_t x = 0; x < _heightMapData.width; x++)
            {
                // The x and y axis are flipped in the world, so this uses y for x and x for y.
                auto tileCoords = HeightmapCoordToTileCoordsXY(x, y);
                auto* const surfaceElement = MapGetSurfaceElementAt(tileCoords);
                if (surfaceElement == nullptr)
                    continue;

                // Read value from bitmap, and convert its range
                uint8_t value = dest[x + y * _heightMapData.width];
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
            // Keep smoothing the entire map until no tiles are changed anymore
            while (true)
            {
                uint32_t numTilesChanged = 0;
                for (uint32_t y = 0; y < _heightMapData.height; y++)
                {
                    for (uint32_t x = 0; x < _heightMapData.width; x++)
                    {
                        auto tileCoords = HeightmapCoordToTileCoordsXY(x, y);
                        numTilesChanged += TileSmooth(tileCoords);
                    }
                }

                if (numTilesChanged == 0)
                    break;
            }
        }
    }
} // namespace OpenRCT2::World::MapGenerator
