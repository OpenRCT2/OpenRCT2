/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Location.hpp"

namespace OpenRCT2::World::MapGenerator
{
    enum class Algorithm : uint8_t
    {
        blank,
        simplexNoise,
        heightmapImage,
    };

    struct Settings
    {
        // Base
        Algorithm algorithm = Algorithm::blank;
        TileCoordsXY mapSize{ 150, 150 };
        int32_t waterLevel = 6;
        int32_t landTexture = 0;
        int32_t edgeTexture = 0;
        int32_t heightmapLow = 14;
        int32_t heightmapHigh = 60;
        bool smoothTileEdges = true;

        // Features (e.g. tree, rivers, lakes etc.)
        bool trees = true;
        int32_t treeToLandRatio = 25;
        int32_t minTreeAltitude = 10;
        int32_t maxTreeAltitude = 50;
        bool beaches = true;

        // Simplex Noise Parameters
        int32_t simplex_base_freq = 175;
        int32_t simplex_octaves = 6;

        // Height map settings
        bool smooth_height_map = true;
        uint32_t smooth_strength = 1;
        bool normalize_height = true;
    };

    class HeightMap;

    void generate(Settings* settings);
    void resetSurfaces(Settings* settings);
    void setWaterLevel(int32_t waterLevel);
    void setMapHeight(Settings* settings, const HeightMap& heightMap);

} // namespace OpenRCT2::World::MapGenerator
