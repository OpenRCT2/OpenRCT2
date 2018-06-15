/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

struct mapgen_settings
{
    // Base
    sint32 mapSize;
    sint32 height;
    sint32 water_level;
    sint32 floor;
    sint32 wall;

    // Features (e.g. tree, rivers, lakes etc.)
    sint32 trees;

    // Simplex Noise Parameters
    sint32 simplex_low;
    sint32 simplex_high;
    float  simplex_base_freq;
    sint32 simplex_octaves;

    // Height map settings
    bool   smooth;
    bool   smooth_height_map;
    uint32 smooth_strength;
    bool   normalize_height;
};

void mapgen_generate_blank(mapgen_settings * settings);
void mapgen_generate(mapgen_settings * settings);
void mapgen_generate_custom_simplex(mapgen_settings * settings);
bool mapgen_load_heightmap(const utf8 * path);
void mapgen_unload_heightmap();
void mapgen_generate_from_heightmap(mapgen_settings * settings);
