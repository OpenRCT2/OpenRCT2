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
#include "BaseMap.hpp"
#include "MapGen.h"
#include "MapHelpers.h"

#include <algorithm>

namespace OpenRCT2::World::MapGenerator
{
    static HeightMap _heightMapData{};

    /**
     * Return the tile coordinate that matches the given pixel of a heightmap
     */
    static TileCoordsXY HeightmapCoordToTileCoordsXY(const MapGenContext& ctx, int32_t x, int32_t y)
    {
        return worldCoordsToGenCoords(ctx, TileCoordsXY(y, x));
    }

    bool loadHeightmapImage(const utf8* path)
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
                return false;
            }

            // Allocate memory for the height map values
            _heightMapData = HeightMap(width, height);

            // Copy average RGB value to mono bitmap
            constexpr auto kNumChannels = 4;
            const auto pitch = image.Stride;
            const auto pixels = image.Pixels.data();
            for (uint32_t x = 0; x < _heightMapData.width; x++)
            {
                for (uint32_t y = 0; y < _heightMapData.height; y++)
                {
                    const auto red = pixels[x * kNumChannels + y * pitch];
                    const auto green = pixels[x * kNumChannels + y * pitch + 1];
                    const auto blue = pixels[x * kNumChannels + y * pitch + 2];
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
    void unloadHeightmapImage()
    {
        _heightMapData.clear();
    }

    TileCoordsXY queryHeightmapImageDimensions()
    {
        Guard::Assert(!_heightMapData.empty(), "No height map loaded");

        // The x and y axis are flipped in the world, so this uses y for x and x for y.
        return { _heightMapData.height, _heightMapData.width };
    }

    void generateFromHeightmapImage(MapGenContext& ctx)
    {
        auto& settings = ctx.settings;
        Guard::Assert(!_heightMapData.empty(), "No height map loaded");

        float maxValue = 255.0f;
        float minValue = 0.0f;

        if (settings.normalizeHeight)
        {
            // Get highest and lowest pixel value
            maxValue = 0.0f;
            minValue = 255.0f;
            for (auto y = 0; y < _heightMapData.height; y++)
            {
                for (auto x = 0; x < _heightMapData.width; x++)
                {
                    float value = _heightMapData[{ x, y }];
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
        Guard::Assert(settings.heightmapHigh > settings.heightmapLow, "Output range is invalid");

        const float rangeIn = maxValue - minValue;
        const float rangeOut = (settings.heightmapHigh - settings.heightmapLow) * 2;

        for (auto y = 0; y < _heightMapData.height; y++)
        {
            for (auto x = 0; x < _heightMapData.width; x++)
            {
                // Read value from bitmap, and convert its range
                float value = _heightMapData[{ x, y }];
                value = (value - minValue) / rangeIn * rangeOut + settings.heightmapLow * 2;
                ctx.heightMap[HeightmapCoordToTileCoordsXY(ctx, x, y)] = value;
            }
        }
    }
} // namespace OpenRCT2::World::MapGenerator
