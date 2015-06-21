/*****************************************************************************
 * Copyright (c) 2014 Ted John
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * This file is part of OpenRCT2.
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#ifndef _MAPGEN_H_
#define _MAPGEN_H_

typedef struct {
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