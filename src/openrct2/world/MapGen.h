/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../core/String.hpp"
#include "Location.hpp"

struct mapgen_settings
{
    // Base
    TileCoordsXY mapSize;
    int32_t height;
    int32_t water_level;
    int32_t floor;
    int32_t wall;

    // Features (e.g. tree, rivers, lakes etc.)
    int32_t trees;

    // Simplex Noise Parameters
    int32_t simplex_low;
    int32_t simplex_high;
    float simplex_base_freq;
    int32_t simplex_octaves;

    // Height map settings
    bool smooth;
    bool smooth_height_map;
    uint32_t smooth_strength;
    bool normalize_height;
};

void MapGenGenerateBlank(mapgen_settings* settings);
void MapGenGenerate(mapgen_settings* settings);
bool MapGenLoadHeightmap(const utf8* path);
void MapGenUnloadHeightmap();
void MapGenGenerateFromHeightmap(mapgen_settings* settings);
