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

#ifndef _USE_MATH_DEFINES
  #define _USE_MATH_DEFINES
#endif
#include <math.h>
#include "../addresses.h"
#include "map.h"
#include "map_helpers.h"
#include "mapgen.h"

static void mapgen_place_trees();
static void mapgen_set_water_level(int height);
static void mapgen_blob(int cx, int cy, int size, int height);
static void mapgen_smooth_height(int iterations);
static void mapgen_set_height();

static int _heightSize;
static uint8 *_height;

#define HEIGHT(x, y) (_height[(y) * _heightSize + (x)])

const uint8 BaseTerrain[] = { TERRAIN_GRASS, TERRAIN_SAND, TERRAIN_SAND_LIGHT, TERRAIN_ICE };

void mapgen_generate_blank(int mapSize, int height, int waterLevel, int floor, int wall)
{
	int x, y;
	rct_map_element *mapElement;

	map_init(mapSize);
	for (y = 1; y < mapSize - 1; y++) {
		for (x = 1; x < mapSize - 1; x++) {
			mapElement = map_get_surface_element_at(x, y);
			map_element_set_terrain(mapElement, floor);
			map_element_set_terrain_edge(mapElement, wall);
			mapElement->base_height = height;
			mapElement->clearance_height = height;
		}
	}

	mapgen_set_water_level(waterLevel);
}

void mapgen_generate(int mapSize)
{
	int i, x, y, baseTerrain;
	rct_map_element *mapElement;

	map_init(mapSize);

	srand((unsigned int)time(NULL));
	baseTerrain = BaseTerrain[rand() % countof(BaseTerrain)];
	for (y = 0; y < mapSize; y++) {
		for (x = 0; x < mapSize; x++) {
			mapElement = map_get_surface_element_at(x, y);
			map_element_set_terrain(mapElement, baseTerrain);
			if (baseTerrain == TERRAIN_ICE)
				map_element_set_terrain_edge(mapElement, TERRAIN_EDGE_ICE);
		}
	}

	_heightSize = mapSize * 2;
	_height = (uint8*)malloc((mapSize * 2) * (mapSize * 2) * sizeof(uint8));
	for (y = 0; y < mapSize * 2; y++) {
		for (x = 0; x < mapSize * 2; x++) {
			double a = abs(x - (mapSize)) / (double)(mapSize);
			double b = abs(y - (mapSize)) / (double)(mapSize);

			a *= 2;
			b *= 2;
			double c = 1 - ((a*a + b*b) / 2);
			c = clamp(0, c, 1);

			int h = 1 + rand() % ((int)(c * 22) + 1);
			h = 1;

			HEIGHT(x, y) = h;
		}
	}

	int border = 2 + (rand() % 24);
	for (i = 0; i < 128; i++) {
		int radius = 4 + (rand() % 64);
		mapgen_blob(
			border + (rand() % (_heightSize - (border * 2))),
			border + (rand() % (_heightSize - (border * 2))),
			(int)(M_PI * radius * radius),
			1 + (rand() % 12)
		);
	}
	mapgen_smooth_height(2);
	mapgen_set_height();
	free(_height);

	while (map_smooth(1, 1, mapSize - 1, mapSize - 1)) { }
	mapgen_set_water_level(6 + (rand() % 4) * 2);
	// mapgen_place_trees();
}

const uint8 GrassTrees[] = { 21, 22, 27, 31, 42, 69, 71, 82, 84, 94, 95, 109 };
const uint8 DesertTrees[] = { 15, 16, 17, 19, 72, 81, 90, 91 };

static void mapgen_place_tree(int type, int x, int y)
{
	uint8 baseHeight;
	rct_map_element *mapElement, *surfaceElement, *nextFreeElement;

	nextFreeElement = RCT2_GLOBAL(0x0140E9A4, rct_map_element*);

	// Get first element of the tile
	mapElement = TILE_MAP_ELEMENT_POINTER(y * 256 + x);

	// Find the last element
	while (!(mapElement->flags & MAP_ELEMENT_FLAG_LAST_TILE)) {
		if ((mapElement->type & MAP_ELEMENT_TYPE_MASK) == MAP_ELEMENT_TYPE_SURFACE)
			surfaceElement = mapElement;
		mapElement++;
	}

	// Shift elements for a free space
	memmove(mapElement + 2, mapElement + 1, (int)nextFreeElement - (int)mapElement);

	baseHeight = map_element_height(x * 32 + 16, y * 32 + 16) / 8;

	mapElement->flags &= ~MAP_ELEMENT_FLAG_LAST_TILE;
	mapElement++;

	mapElement->type = MAP_ELEMENT_TYPE_SCENERY | (rand() % 3);
	mapElement->flags = MAP_ELEMENT_FLAG_LAST_TILE | (1 | 2 | 4 | 8);
	mapElement->base_height = baseHeight;
	mapElement->clearance_height = baseHeight + 15;
	mapElement->properties.scenery.type = type;
	mapElement->properties.scenery.age = 0;
	mapElement->properties.scenery.colour_1 = 26;
	mapElement->properties.scenery.colour_2 = 18;

	map_update_tile_pointers();
}

static void mapgen_place_trees()
{
	int x, y, mapSize, i, rindex, type;
	rct_map_element *mapElement;

	mapSize = RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE, sint16);

	int availablePositionsCount = 0;
	struct { int x; int y; } tmp, *pos, *availablePositions;
	availablePositions = malloc(256 * 256 * sizeof(tmp));

	// Create list of available tiles
	for (y = 1; y < mapSize - 1; y++) {
		for (x = 1; x < mapSize - 1; x++) {
			mapElement = map_get_surface_element_at(x, y);

			// Exclude water tiles
			if ((mapElement->properties.surface.terrain & 0x1F) != 0)
				continue;

			pos = &availablePositions[availablePositionsCount++];
			pos->x = x;
			pos->y = y;
		}
	}

	// Shuffle list
	for (i = 0; i < availablePositionsCount; i++) {
		rindex = rand() % availablePositionsCount;
		if (rindex == i)
			continue;

		tmp = availablePositions[i];
		availablePositions[i] = availablePositions[rindex];
		availablePositions[rindex] = tmp;
	}

	// Place trees
	mapSize = RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE, sint16);
	for (i = 0; i < min(availablePositionsCount, 2048 + (rand() % 10000)); i++) {
		pos = &availablePositions[i];

		mapElement = map_get_surface_element_at(x, y);
		if (map_element_get_terrain(mapElement) == TERRAIN_SAND)
			type = DesertTrees[rand() % countof(DesertTrees)];
		else
			type = GrassTrees[rand() % countof(GrassTrees)];

		mapgen_place_tree(type, pos->x, pos->y);
	}

	free(availablePositions);
}

static void mapgen_set_water_level(int waterLevel)
{
	int x, y, mapSize;
	rct_map_element *mapElement;

	mapSize = RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE, sint16);

	for (y = 1; y < mapSize - 1; y++) {
		for (x = 1; x < mapSize - 1; x++) {
			mapElement = map_get_surface_element_at(x, y);
			if (mapElement->base_height < waterLevel)
				mapElement->properties.surface.terrain |= (waterLevel  / 2);
		}
	}
}

static void mapgen_blob_fill(int height)
{
	// For each square find out whether it is landlocked by 255 and then fill it if it is
	int left = 0,
		top = 0,
		right = _heightSize - 1,
		bottom = _heightSize - 1;

	uint8 *landX = (uint8*)malloc(_heightSize * _heightSize * sizeof(uint8));
	int firstLand, lastLand;

	// Check each row and see if each tile is between first land x and last land x
	for (int y = top; y <= bottom; y++) {
		// Calculate first land
		firstLand = -1;
		for (int xx = left; xx <= right; xx++) {
			if (HEIGHT(xx, y) == 255) {
				firstLand = xx;
				break;
			}
		}

		lastLand = -1;
		if (firstLand >= 0) {
			// Calculate last land
			for (int xx = right; xx >= left; xx--) {
				if (HEIGHT(xx, y) == 255) {
					lastLand = xx;
					break;
				}
			}
		} else {
			// No land on this row
			continue;
		}

		for (int x = left; x <= right; x++)
			if (x >= firstLand && x <= lastLand)
				landX[x, y] = 1;
	}

	// Do the same for Y
	for (int x = left; x <= right; x++) {
		// Calculate first land
		firstLand = -1;
		for (int yy = top; yy <= bottom; yy++) {
			if (HEIGHT(x, yy) == 255) {
				firstLand = yy;
				break;
			}
		}

		lastLand = -1;
		if (firstLand >= 0) {
			// Calculate last land
			for (int yy = bottom; yy >= top; yy--) {
				if (HEIGHT(x, yy) == 255) {
					lastLand = yy;
					break;
				}
			}
		} else {
			// No land on this row
			continue;
		}

		for (int y = top; y <= bottom; y++) {
			if (y >= firstLand && y <= lastLand && landX[x, y]) {
				// Not only do we know its landlocked to both x and y
				// we can change the land too
				HEIGHT(x, y) = 255;
			}
		}
	}

	// Replace all the 255 with the actual land height
	for (int x = left; x <= right; x++)
		for (int y = top; y <= bottom; y++)
			if (HEIGHT(x, y) == 255)
				HEIGHT(x, y) = height;

	free(landX);
}

static void mapgen_blob(int cx, int cy, int size, int height)
{
	int x, y, currentSize, direction;

	x = cx;
	y = cy;
	currentSize = 1;
	direction = 0;
	HEIGHT(y, x) = 255;

	while (currentSize < size) {
		if (rand() % 2 == 0) {
			HEIGHT(x, y) = 255;
			currentSize++;
		}

		switch (direction) {
		case 0:
			if (y == 0) {
				currentSize = size;
				break;
			}

			y--;
			if (HEIGHT(x + 1, y) != 255)
				direction = 1;
			else if (HEIGHT(x, y - 1) != 255)
				direction = 0;
			else if (HEIGHT(x - 1, y) != 255)
				direction = 3;
			break;
		case 1:
			if (x == _heightSize - 1) {
				currentSize = size;
				break;
			}

			x++;
			if (HEIGHT(x, y + 1) != 255)
				direction = 2;
			else if (HEIGHT(x + 1, y) != 255)
				direction = 1;
			else if (HEIGHT(x, y - 1) != 255)
				direction = 0;
			break;
		case 2:
			if (y == _heightSize - 1) {
				currentSize = size;
				break;
			}

			y++;
			if (HEIGHT(x - 1, y) != 255)
				direction = 3;
			else if (HEIGHT(x, y + 1) != 255)
				direction = 2;
			else if (HEIGHT(x + 1, y) != 255)
				direction = 1;
			break;
		case 3:
			if (x == 0) {
				currentSize = size;
				break;
			}

			x--;
			if (HEIGHT(x, y - 1) != 255)
				direction = 0;
			else if (HEIGHT(x - 1, y) != 255)
				direction = 3;
			else if (HEIGHT(x, y + 1) != 255)
				direction = 2;
			break;
		}
	}

	mapgen_blob_fill(height);
}

static void mapgen_smooth_height(int iterations)
{
	int i, x, y, xx, yy, avg;
	int arraySize = _heightSize * _heightSize * sizeof(uint8);
	uint8 *copyHeight = malloc(arraySize);
	memcpy(copyHeight, _height, arraySize);

	for (i = 0; i < iterations; i++) {
		for (y = 1; y < _heightSize - 1; y++) {
			for (x = 1; x < _heightSize - 1; x++) {
				avg = 0;
				for (yy = -1; yy <= 1; yy++)
					for (xx = -1; xx <= 1; xx++)
						avg += copyHeight[(y + yy) * _heightSize + (x + xx)];
				avg /= 9;
				HEIGHT(x, y) = avg;
			}
		}
	}
}

static void mapgen_set_height()
{
	int x, y, heightX, heightY, mapSize;
	rct_map_element *mapElement;

	mapSize = _heightSize / 2;
	for (y = 1; y < mapSize - 1; y++) {
		for (x = 1; x < mapSize - 1; x++) {
			heightX = x * 2;
			heightY = y * 2;

			uint8 q00 = HEIGHT(heightX + 0, heightY + 0);
			uint8 q01 = HEIGHT(heightX + 0, heightY + 1);
			uint8 q10 = HEIGHT(heightX + 1, heightY + 0);
			uint8 q11 = HEIGHT(heightX + 1, heightY + 1);

			uint8 baseHeight = (q00 + q01 + q10 + q11) / 4;
			
			mapElement = map_get_surface_element_at(x, y);
			mapElement->base_height = baseHeight * 2;
			mapElement->clearance_height = mapElement->base_height;

			if (q00 > baseHeight)
				mapElement->properties.surface.slope |= 4;
			if (q01 > baseHeight)
				mapElement->properties.surface.slope |= 8;
			if (q10 > baseHeight)
				mapElement->properties.surface.slope |= 2;
			if (q11 > baseHeight)
				mapElement->properties.surface.slope |= 1;
		}
	}
}