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

#include "addresses.h"
#include "map.h"
#include "map_helpers.h"
#include "mapgen.h"

static void mapgen_set_water_level(int height);

void mapgen_generate()
{
	int x, y, mapSize;
	rct_map_element *mapElement;

	map_init();

	// Not sure how to change map size at the moment, default is 150
	mapSize = 150;

	for (y = 1; y < mapSize - 1; y++) {
		for (x = 1; x < mapSize - 1; x++) {
			mapElement = map_get_surface_element_at(x, y);
			map_element_set_terrain(mapElement, TERRAIN_SAND);

			double a = abs(x - (mapSize / 2)) / (double)(mapSize / 2);
			double b = abs(y - (mapSize / 2)) / (double)(mapSize / 2);

			a *= 2;
			b *= 2;
			double c = 1 - ((a*a + b*b) / 2);
			c = clamp(0, c, 1);

			int height = 2 + rand() % ((int)(c * 22) + 1);

			mapElement->base_height = height * 2;
			mapElement->clearance_height = mapElement->base_height;
		}
	}

	while (map_smooth(1, 1, mapSize - 1, mapSize - 1)) { }

	mapgen_set_water_level(8);
}

static void mapgen_set_water_level(int waterLevel)
{
	int x, y, mapSize;
	rct_map_element *mapElement;

	mapSize = RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE, sint16);

	for (y = 1; y < mapSize - 1; y++) {
		for (x = 1; x < mapSize - 1; x++) {
			mapElement = map_get_surface_element_at(x, y);
			if (mapElement->base_height <= waterLevel)
				mapElement->properties.surface.terrain |= (waterLevel - 5);
		}
	}
}