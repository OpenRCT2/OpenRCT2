#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

typedef struct mapgen_settings {
	// Base
	int mapSize;
	int height;
	int waterLevel;
	int floor;
	int wall;

	// Features (e.g. tree, rivers, lakes etc.)
	int trees;

	// Simplex Noise Parameters
	int simplex_low;
	int simplex_high;
	float simplex_base_freq;
	int simplex_octaves;
} mapgen_settings;

void mapgen_generate_blank(mapgen_settings *settings);
void mapgen_generate(mapgen_settings *settings);
void mapgen_generate_custom_simplex(mapgen_settings *settings);

#endif
