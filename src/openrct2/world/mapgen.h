#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#ifndef _MAPGEN_H_
#define _MAPGEN_H_

#include "../common.h"

typedef struct mapgen_settings {
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
	float simplex_base_freq;
	sint32 simplex_octaves;

	// Height map settings
	bool smooth;
	bool smooth_height_map;
	uint32 smooth_strength;
	bool normalize_height;
} mapgen_settings;

void mapgen_generate_blank(mapgen_settings *settings);
void mapgen_generate(mapgen_settings *settings);
void mapgen_generate_custom_simplex(mapgen_settings *settings);
bool mapgen_load_heightmap(const utf8 *path);
void mapgen_unload_heightmap();
void mapgen_generate_from_heightmap(mapgen_settings *settings);

#endif
