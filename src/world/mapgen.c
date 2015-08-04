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

#include <time.h>
#include "../addresses.h"
#include "../object.h"
#include "map.h"
#include "map_helpers.h"
#include "mapgen.h"
#include "scenery.h"

#pragma region Random objects

const char *GrassTrees[] = {
	// Dark
	"TCF     ",		// Caucasian Fir Tree
	"TRF     ",		// Red Fir Tree
	"TRF2    ",		// Red Fir Tree
	"TSP     ",		// Scots Pine Tree
	"TMZP    ",		// Montezuma Pine Tree
	"TAP     ",		// Aleppo Pine Tree
	"TCRP    ",		// Corsican Pine Tree
	"TBP     ",		// Black Poplar Tree

	// Light
	"TCL     ",		// Cedar of Lebanon Tree
	"TEL     ",		// European Larch Tree
};

// Trees to be placed in proximity to water
const char *GrassWaterTrees[] = {
	"TWW     "		// Weeping Willow Tree
};

const char *DesertTrees[] = {
	"TMP     ",		// Monkey-Puzzle Tree
	"THL     ",		// Honey Locust Tree
	"TH1     ",		// Canary Palm Tree
	"TH2     ",		// Palm Tree
	"TPM     ",		// Palm Tree
	"TROPT1  ",		// Tree
	"TBC     ",		// Cactus
	"TSC     ",		// Cactus
};

const char *SnowTrees[] = {
	"TCFS    ",		// Snow-covered Caucasian Fir Tree
	"TNSS    ",		// Snow-covered Norway Spruce Tree
	"TRF3    ",		// Snow-covered Red Fir Tree
	"TRFS    ",		// Snow-covered Red Fir Tree
};

#pragma endregion

// Randomly chosen base terrains. We rarely want a whole map made out of chequerboard or rock.
const uint8 BaseTerrain[] = { TERRAIN_GRASS, TERRAIN_SAND, TERRAIN_SAND_LIGHT, TERRAIN_DIRT, TERRAIN_ICE };

#define BLOB_HEIGHT 255

static void mapgen_place_trees();
static void mapgen_set_water_level(int height);
static void mapgen_blobs(int count, int lowSize, int highSize, int lowHeight, int highHeight);
static void mapgen_blob(int cx, int cy, int size, int height);
static void mapgen_smooth_height(int iterations);
static void mapgen_set_height();

static void mapgen_simplex();

static int _heightSize;
static uint8 *_height;

static int get_height(int x, int y)
{
	if (x >= 0 && y >= 0 && x < _heightSize && y < _heightSize)
		return _height[x + y * _heightSize];
	else
		return 0;
}

static void set_height(int x, int y, int height)
{
	if (x >= 0 && y >= 0 && x < _heightSize && y < _heightSize)
		_height[x + y * _heightSize] = height;
}

void mapgen_generate_blank(mapgen_settings *settings)
{
	int x, y;
	rct_map_element *mapElement;

	map_init(settings->mapSize);
	for (y = 1; y < settings->mapSize - 1; y++) {
		for (x = 1; x < settings->mapSize - 1; x++) {
			mapElement = map_get_surface_element_at(x, y);
			map_element_set_terrain(mapElement, settings->floor);
			map_element_set_terrain_edge(mapElement, settings->wall);
			mapElement->base_height = settings->height;
			mapElement->clearance_height = settings->height;
		}
	}

	mapgen_set_water_level(settings->waterLevel);
}

void mapgen_generate(mapgen_settings *settings)
{
	int x, y, mapSize, floorTexture, wallTexture, waterLevel;
	rct_map_element *mapElement;

	srand((unsigned int)time(NULL));

	mapSize = settings->mapSize;
	floorTexture = settings->floor;
	wallTexture = settings->wall;
	waterLevel = settings->waterLevel;

	if (floorTexture == -1)
		floorTexture = BaseTerrain[rand() % countof(BaseTerrain)];

	if (wallTexture == -1) {
		// Base edge type on surface type
		switch (floorTexture) {
		case TERRAIN_DIRT:
			wallTexture = TERRAIN_EDGE_WOOD_RED;
			break;
		case TERRAIN_ICE:
			wallTexture = TERRAIN_EDGE_ICE;
			break;
		default:
			wallTexture = TERRAIN_EDGE_ROCK;
			break;
		}
	}

	// Initialise the base map
	map_init(mapSize);
	for (y = 1; y < mapSize - 1; y++) {
		for (x = 1; x < mapSize - 1; x++) {
			mapElement = map_get_surface_element_at(x, y);
			map_element_set_terrain(mapElement, floorTexture);
			map_element_set_terrain_edge(mapElement, wallTexture);
			mapElement->base_height = settings->height;
			mapElement->clearance_height = settings->height;
		}
	}

	// Create the temporary height map and initialise
	_heightSize = mapSize * 2;
	_height = (uint8*)malloc(_heightSize * _heightSize * sizeof(uint8));
	memset(_height, 0, _heightSize * _heightSize * sizeof(uint8));

	if (1) {
		mapgen_simplex(settings);
		mapgen_smooth_height(2 + (rand() % 6));
	} else {
		// Keep overwriting the map with rough cicular blobs of different sizes and heights.
		// This procedural method can produce intersecting contour like land and lakes.
		// Large blobs, general shape of map
		mapgen_blobs(6, _heightSize / 2, _heightSize * 4, 4, 16);
		// Medium blobs
		mapgen_blobs(12, _heightSize / 16, _heightSize / 8, 4, 18);
		// Small blobs, small hills and lakes
		mapgen_blobs(32, _heightSize / 32, _heightSize / 16, 4, 18);

		// Smooth the land so that their aren't cliffs round every blob.
		mapgen_smooth_height(2);
	}

	// Set the game map to the height map
	mapgen_set_height();
	free(_height);

	// Set the tile slopes so that there are no cliffs
	while (map_smooth(1, 1, mapSize - 1, mapSize - 1)) { }

	// Add the water
	mapgen_set_water_level(waterLevel);

	// Add sandy beaches
	int beachTexture = floorTexture;
	if (settings->floor == -1 && floorTexture == TERRAIN_GRASS) {
		switch (rand() % 4) {
		case 0:
			beachTexture = TERRAIN_SAND;
			break;
		case 1:
			beachTexture = TERRAIN_SAND_LIGHT;
			break;
		}
	}
	for (y = 1; y < mapSize - 1; y++) {
		for (x = 1; x < mapSize - 1; x++) {
			mapElement = map_get_surface_element_at(x, y);

			if (mapElement->base_height < waterLevel + 6)
				map_element_set_terrain(mapElement, beachTexture);
		}
	}

	// Place the trees
	if (settings->trees != 0)
		mapgen_place_trees();

	map_reorganise_elements();
}

static void mapgen_place_tree(int type, int x, int y)
{
	int surfaceZ;
	rct_map_element *mapElement;
	rct_scenery_entry *sceneryEntry = g_smallSceneryEntries[type];

	surfaceZ = map_element_height(x * 32 + 16, y * 32 + 16) / 8;
	mapElement = map_element_insert(x, y, surfaceZ, (1 | 2 | 4 | 8));
	mapElement->clearance_height = surfaceZ + (sceneryEntry->small_scenery.height >> 3);

	mapElement->type = MAP_ELEMENT_TYPE_SCENERY | (rand() % 3);
	mapElement->properties.scenery.type = type;
	mapElement->properties.scenery.age = 0;
	mapElement->properties.scenery.colour_1 = 26;
	mapElement->properties.scenery.colour_1 = 18;
}

/**
 * Randomly places a selection of preset trees on the map. Picks the right tree for the terrain it is placing it on.
 */
static void mapgen_place_trees()
{
	int x, y, mapSize, i, j, rindex, type;
	rct_map_element *mapElement;

	int numGrassTreeIds = 0, numDesertTreeIds = 0, numSnowTreeIds = 0;
	int *grassTreeIds = (int*)malloc(countof(GrassTrees) * sizeof(int));
	int *desertTreeIds = (int*)malloc(countof(DesertTrees) * sizeof(int));
	int *snowTreeIds = (int*)malloc(countof(SnowTrees) * sizeof(int));

	for (i = 0; i < object_entry_group_counts[OBJECT_TYPE_SMALL_SCENERY]; i++) {
		rct_scenery_entry *sceneryEntry = g_smallSceneryEntries[i];
		rct_object_entry_extended *entry = &object_entry_groups[OBJECT_TYPE_SMALL_SCENERY].entries[i];

		if (sceneryEntry == (rct_scenery_entry*)0xFFFFFFFF || sceneryEntry == NULL)
			continue;

		for (j = 0; j < countof(GrassTrees); j++)
			if (strncmp(GrassTrees[j], entry->name, 8) == 0)
				break;
		if (j != countof(GrassTrees)) {
			grassTreeIds[numGrassTreeIds++] = i;
			continue;
		}

		for (j = 0; j < countof(DesertTrees); j++)
			if (strncmp(DesertTrees[j], entry->name, 8) == 0)
				break;
		if (j != countof(DesertTrees)) {
			desertTreeIds[numDesertTreeIds++] = i;
			continue;
		}

		for (j = 0; j < countof(SnowTrees); j++)
			if (strncmp(SnowTrees[j], entry->name, 8) == 0)
				break;
		if (j != countof(SnowTrees)) {
			snowTreeIds[numSnowTreeIds++] = i;
			continue;
		}
	}

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
	float treeToLandRatio = (10 + (rand() % 30)) / 100.0f;
	int numTrees = max(4, (int)(availablePositionsCount * treeToLandRatio));

	mapSize = RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE, sint16);
	for (i = 0; i < numTrees; i++) {
		pos = &availablePositions[i];

		type = -1;
		mapElement = map_get_surface_element_at(pos->x, pos->y);
		switch (map_element_get_terrain(mapElement)) {
		case TERRAIN_GRASS:
		case TERRAIN_DIRT:
		case TERRAIN_GRASS_CLUMPS:
			if (numGrassTreeIds == 0)
				break;

			type = grassTreeIds[rand() % numGrassTreeIds];
			break;

		case TERRAIN_SAND:
		case TERRAIN_SAND_DARK:
		case TERRAIN_SAND_LIGHT:
			if (numDesertTreeIds == 0)
				break;

			if (rand() % 4 == 0)
				type = desertTreeIds[rand() % numDesertTreeIds];
			break;

		case TERRAIN_ICE:
			if (numSnowTreeIds == 0)
				break;
			
			type = snowTreeIds[rand() % numSnowTreeIds];
			break;
		}

		if (type != -1)
			mapgen_place_tree(type, pos->x, pos->y);
	}

	free(availablePositions);
	free(grassTreeIds);
	free(desertTreeIds);
	free(snowTreeIds);
}

/**
 * Sets each tile's water level to the specified water level if underneath that water level.
 */
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

static void mapgen_blobs(int count, int lowSize, int highSize, int lowHeight, int highHeight)
{
	int i;
	int sizeRange = highSize - lowSize;
	int heightRange = highHeight - lowHeight;

	int border = 2 + (rand() % 24);
	int borderRange = _heightSize - (border * 2);
	for (i = 0; i < count; i++) {
		int radius = lowSize + (rand() % sizeRange);
		mapgen_blob(
			border + (rand() % borderRange),
			border + (rand() % borderRange),
			(int)(M_PI * radius * radius),
			lowHeight + (rand() % heightRange)
		);
	}
}

/**
 * Sets any holes within a new created blob to the specified height.
 */
static void mapgen_blob_fill(int height)
{
	// For each square find out whether it is landlocked by BLOB_HEIGHT and then fill it if it is
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
			if (get_height(xx, y) == BLOB_HEIGHT) {
				firstLand = xx;
				break;
			}
		}

		lastLand = -1;
		if (firstLand >= 0) {
			// Calculate last land
			for (int xx = right; xx >= left; xx--) {
				if (get_height(xx, y) == BLOB_HEIGHT) {
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
				landX[x * _heightSize + y] = 1;
	}

	// Do the same for Y
	for (int x = left; x <= right; x++) {
		// Calculate first land
		firstLand = -1;
		for (int yy = top; yy <= bottom; yy++) {
			if (get_height(x, yy) == BLOB_HEIGHT) {
				firstLand = yy;
				break;
			}
		}

		lastLand = -1;
		if (firstLand >= 0) {
			// Calculate last land
			for (int yy = bottom; yy >= top; yy--) {
				if (get_height(x, yy) == BLOB_HEIGHT) {
					lastLand = yy;
					break;
				}
			}
		} else {
			// No land on this row
			continue;
		}

		for (int y = top; y <= bottom; y++) {
			if (y >= firstLand && y <= lastLand && landX[x * _heightSize + y]) {
				// Not only do we know its landlocked to both x and y
				// we can change the land too
				set_height(x, y, BLOB_HEIGHT);
			}
		}
	}

	// Replace all the BLOB_HEIGHT with the actual land height
	for (int x = left; x <= right; x++)
		for (int y = top; y <= bottom; y++)
			if (get_height(x, y) == BLOB_HEIGHT)
				set_height(x, y, height);

	free(landX);
}

/**
 * Sets a rough circular blob of tiles of the specified size to the specified height.
 */
static void mapgen_blob(int cx, int cy, int size, int height)
{
	int x, y, currentSize, direction;

	x = cx;
	y = cy;
	currentSize = 1;
	direction = 0;
	set_height(x, y, BLOB_HEIGHT);

	while (currentSize < size) {
		if (rand() % 2 == 0) {
			set_height(x, y, BLOB_HEIGHT);
			currentSize++;
		}

		switch (direction) {
		case 0:
			if (y == 0) {
				currentSize = size;
				break;
			}

			y--;
			if (get_height(x + 1, y) != BLOB_HEIGHT)
				direction = 1;
			else if (get_height(x, y - 1) != BLOB_HEIGHT)
				direction = 0;
			else if (get_height(x - 1, y) != BLOB_HEIGHT)
				direction = 3;
			break;
		case 1:
			if (x == _heightSize - 1) {
				currentSize = size;
				break;
			}

			x++;
			if (get_height(x, y + 1) != BLOB_HEIGHT)
				direction = 2;
			else if (get_height(x + 1, y) != BLOB_HEIGHT)
				direction = 1;
			else if (get_height(x, y - 1) != BLOB_HEIGHT)
				direction = 0;
			break;
		case 2:
			if (y == _heightSize - 1) {
				currentSize = size;
				break;
			}

			y++;
			if (get_height(x - 1, y) != BLOB_HEIGHT)
				direction = 3;
			else if (get_height(x, y + 1) != BLOB_HEIGHT)
				direction = 2;
			else if (get_height(x + 1, y) != BLOB_HEIGHT)
				direction = 1;
			break;
		case 3:
			if (x == 0) {
				currentSize = size;
				break;
			}

			x--;
			if (get_height(x, y - 1) != BLOB_HEIGHT)
				direction = 0;
			else if (get_height(x - 1, y) != BLOB_HEIGHT)
				direction = 3;
			else if (get_height(x, y + 1) != BLOB_HEIGHT)
				direction = 2;
			break;
		}
	}

	mapgen_blob_fill(height);
}

/**
 * Smooths the height map.
 */
static void mapgen_smooth_height(int iterations)
{
	int i, x, y, xx, yy, avg;
	int arraySize = _heightSize * _heightSize * sizeof(uint8);
	uint8 *copyHeight = malloc(arraySize);

	for (i = 0; i < iterations; i++) {
		memcpy(copyHeight, _height, arraySize);
		for (y = 1; y < _heightSize - 1; y++) {
			for (x = 1; x < _heightSize - 1; x++) {
				avg = 0;
				for (yy = -1; yy <= 1; yy++)
					for (xx = -1; xx <= 1; xx++)
						avg += copyHeight[(y + yy) * _heightSize + (x + xx)];
				avg /= 9;
				set_height(x, y, avg);
			}
		}
	}

	free(copyHeight);
}

/**
 * Sets the height of the actual game map tiles to the height map.
 */
static void mapgen_set_height()
{
	int x, y, heightX, heightY, mapSize;
	rct_map_element *mapElement;

	mapSize = _heightSize / 2;
	for (y = 1; y < mapSize - 1; y++) {
		for (x = 1; x < mapSize - 1; x++) {
			heightX = x * 2;
			heightY = y * 2;

			uint8 q00 = get_height(heightX + 0, heightY + 0);
			uint8 q01 = get_height(heightX + 0, heightY + 1);
			uint8 q10 = get_height(heightX + 1, heightY + 0);
			uint8 q11 = get_height(heightX + 1, heightY + 1);

			uint8 baseHeight = (q00 + q01 + q10 + q11) / 4;
			
			mapElement = map_get_surface_element_at(x, y);
			mapElement->base_height = max(2, baseHeight * 2);
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

#pragma region Noise

/**
 * Simplex Noise Algorithm with Fractional Brownian Motion
 * Based on:
 *   - https://code.google.com/p/simplexnoise/
 *   - https://code.google.com/p/fractalterraingeneration/wiki/Fractional_Brownian_Motion
 */

static float generate(float x, float y);
static int fast_floor(float x);
static float grad(int hash, float x, float y);

static uint8 perm[512];

static void noise_rand()
{
	for (int i = 0; i < countof(perm); i++)
		perm[i] = rand() & 0xFF;
}

static float fractal_noise(int x, int y, float frequency, int octaves, float lacunarity, float persistence)
{
	float total = 0.0f;
	float amplitude = persistence;
	for (int i = 0; i < octaves; i++) {
		total += generate(x * frequency, y * frequency) * amplitude;
		frequency *= lacunarity;
		amplitude *= persistence;
	}
	return total;
}

static float generate(float x, float y)
{
	const float F2 = 0.366025403f; // F2 = 0.5*(sqrt(3.0)-1.0)
	const float G2 = 0.211324865f; // G2 = (3.0-Math.sqrt(3.0))/6.0

	float n0, n1, n2; // Noise contributions from the three corners

	// Skew the input space to determine which simplex cell we're in
	float s = (x + y) * F2; // Hairy factor for 2D
	float xs = x + s;
	float ys = y + s;
	int i = fast_floor(xs);
	int j = fast_floor(ys);

	float t = (float)(i + j) * G2;
	float X0 = i - t; // Unskew the cell origin back to (x,y) space
	float Y0 = j - t;
	float x0 = x - X0; // The x,y distances from the cell origin
	float y0 = y - Y0;

	// For the 2D case, the simplex shape is an equilateral triangle.
	// Determine which simplex we are in.
	int i1, j1; // Offsets for second (middle) corner of simplex in (i,j) coords
	if (x0 > y0) { i1 = 1; j1 = 0; } // lower triangle, XY order: (0,0)->(1,0)->(1,1)
	else { i1 = 0; j1 = 1; }      // upper triangle, YX order: (0,0)->(0,1)->(1,1)

	// A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
	// a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
	// c = (3-sqrt(3))/6

	float x1 = x0 - i1 + G2; // Offsets for middle corner in (x,y) unskewed coords
	float y1 = y0 - j1 + G2;
	float x2 = x0 - 1.0f + 2.0f * G2; // Offsets for last corner in (x,y) unskewed coords
	float y2 = y0 - 1.0f + 2.0f * G2;

	// Wrap the integer indices at 256, to avoid indexing perm[] out of bounds
	int ii = i % 256;
	int jj = j % 256;

	// Calculate the contribution from the three corners
	float t0 = 0.5f - x0 * x0 - y0 * y0;
	if (t0 < 0.0f) n0 = 0.0f;
	else {
		t0 *= t0;
		n0 = t0 * t0 * grad(perm[ii + perm[jj]], x0, y0);
	}

	float t1 = 0.5f - x1 * x1 - y1 * y1;
	if (t1 < 0.0f) n1 = 0.0f;
	else {
		t1 *= t1;
		n1 = t1 * t1 * grad(perm[ii + i1 + perm[jj + j1]], x1, y1);
	}

	float t2 = 0.5f - x2 * x2 - y2 * y2;
	if (t2 < 0.0f) n2 = 0.0f;
	else {
		t2 *= t2;
		n2 = t2 * t2 * grad(perm[ii + 1 + perm[jj + 1]], x2, y2);
	}

	// Add contributions from each corner to get the final noise value.
	// The result is scaled to return values in the interval [-1,1].
	return 40.0f * (n0 + n1 + n2); // TODO: The scale factor is preliminary!
}

static int fast_floor(float x)
{
	return (x > 0) ? ((int)x) : (((int)x) - 1);
}

static float grad(int hash, float x, float y)
{
	int h = hash & 7;      // Convert low 3 bits of hash code
	float u = h < 4 ? x : y;  // into 8 simple gradient directions,
	float v = h < 4 ? y : x;  // and compute the dot product with (x,y).
	return ((h & 1) != 0 ? -u : u) + ((h & 2) != 0 ? -2.0f * v : 2.0f * v);
}

static void mapgen_simplex(mapgen_settings *settings)
{
	int x, y;

	float freq = settings->simplex_base_freq * (1.0f / _heightSize);
	int octaves = settings->simplex_octaves;

	int low = settings->simplex_low;
	int high = settings->simplex_high;

	noise_rand();
	for (y = 0; y < _heightSize; y++) {
		for (x = 0; x < _heightSize; x++) {
			float noiseValue = clamp(-1.0f, fractal_noise(x, y, freq, octaves, 2.0f, 0.65f), 1.0f);
			float normalisedNoiseValue = (noiseValue + 1.0f) / 2.0f;

			set_height(x, y, low + (int)(normalisedNoiseValue * high));
		}
	}
}

#pragma endregion
