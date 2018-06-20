/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../common.h"
#include <cmath>
#include <cstring>
#include <vector>

#include "../Context.h"
#include "../core/Guard.hpp"
#include "../core/Math.hpp"
#include "../core/String.hpp"
#include "../core/Util.hpp"
#include "../Game.h"
#include "../core/Imaging.h"
#include "../localisation/StringIds.h"
#include "../object/Object.h"
#include "../platform/platform.h"
#include "../util/Util.h"
#include "Map.h"
#include "MapHelpers.h"
#include "MapGen.h"
#include "Scenery.h"
#include "SmallScenery.h"
#include "Surface.h"

#pragma region Height map struct

static struct
{
    uint32_t width, height;
    uint8_t * mono_bitmap;
} _heightMapData = {
    0,
    0,
    nullptr
};

#pragma endregion Height map struct

#pragma region Random objects

static constexpr const char * GrassTrees[] = {
    // Dark
    "TCF     ",     // Caucasian Fir Tree
    "TRF     ",     // Red Fir Tree
    "TRF2    ",     // Red Fir Tree
    "TSP     ",     // Scots Pine Tree
    "TMZP    ",     // Montezuma Pine Tree
    "TAP     ",     // Aleppo Pine Tree
    "TCRP    ",     // Corsican Pine Tree
    "TBP     ",     // Black Poplar Tree

    // Light
    "TCL     ",     // Cedar of Lebanon Tree
    "TEL     ",     // European Larch Tree
};

static constexpr const char * DesertTrees[] = {
    "TMP     ",     // Monkey-Puzzle Tree
    "THL     ",     // Honey Locust Tree
    "TH1     ",     // Canary Palm Tree
    "TH2     ",     // Palm Tree
    "TPM     ",     // Palm Tree
    "TROPT1  ",     // Tree
    "TBC     ",     // Cactus
    "TSC     ",     // Cactus
};

static constexpr const char * SnowTrees[] = {
    "TCFS    ",     // Snow-covered Caucasian Fir Tree
    "TNSS    ",     // Snow-covered Norway Spruce Tree
    "TRF3    ",     // Snow-covered Red Fir Tree
    "TRFS    ",     // Snow-covered Red Fir Tree
};

#pragma endregion

// Randomly chosen base terrains. We rarely want a whole map made out of chequerboard or rock.
static constexpr const uint8_t BaseTerrain[] = {TERRAIN_GRASS, TERRAIN_SAND, TERRAIN_SAND_LIGHT, TERRAIN_DIRT, TERRAIN_ICE};

#define BLOB_HEIGHT 255

static void mapgen_place_trees();
static void mapgen_set_water_level(int32_t waterLevel);
static void mapgen_smooth_height(int32_t iterations);
static void mapgen_set_height();

static void mapgen_simplex(mapgen_settings * settings);

static int32_t  _heightSize;
static uint8_t * _height;

static int32_t get_height(int32_t x, int32_t y)
{
    if (x >= 0 && y >= 0 && x < _heightSize && y < _heightSize)
        return _height[x + y * _heightSize];
    else
        return 0;
}

static void set_height(int32_t x, int32_t y, int32_t height)
{
    if (x >= 0 && y >= 0 && x < _heightSize && y < _heightSize)
        _height[x + y * _heightSize] = height;
}

void mapgen_generate_blank(mapgen_settings * settings)
{
    int32_t x, y;
    rct_tile_element * tileElement;

    map_clear_all_elements();

    map_init(settings->mapSize);
    for (y = 1; y < settings->mapSize - 1; y++)
    {
        for (x = 1; x < settings->mapSize - 1; x++)
        {
            tileElement = map_get_surface_element_at(x, y);
            surface_set_terrain(tileElement, settings->floor);
            surface_set_terrain_edge(tileElement, settings->wall);
            tileElement->base_height      = settings->height;
            tileElement->clearance_height = settings->height;
        }
    }

    mapgen_set_water_level(settings->water_level);
}

void mapgen_generate(mapgen_settings * settings)
{
    int32_t x, y, mapSize, floorTexture, wallTexture, waterLevel;
    rct_tile_element * tileElement;

    util_srand((int32_t) platform_get_ticks());

    mapSize      = settings->mapSize;
    floorTexture = settings->floor;
    wallTexture  = settings->wall;
    waterLevel   = settings->water_level;

    if (floorTexture == -1)
        floorTexture = BaseTerrain[util_rand() % Util::CountOf(BaseTerrain)];

    if (wallTexture == -1)
    {
        // Base edge type on surface type
        switch (floorTexture)
        {
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

    map_clear_all_elements();

    // Initialise the base map
    map_init(mapSize);
    for (y = 1; y < mapSize - 1; y++)
    {
        for (x = 1; x < mapSize - 1; x++)
        {
            tileElement = map_get_surface_element_at(x, y);
            surface_set_terrain(tileElement, floorTexture);
            surface_set_terrain_edge(tileElement, wallTexture);
            tileElement->base_height      = settings->height;
            tileElement->clearance_height = settings->height;
        }
    }

    // Create the temporary height map and initialise
    _heightSize = mapSize * 2;
    _height     = new uint8_t[_heightSize * _heightSize];
    memset(_height, 0, _heightSize * _heightSize * sizeof(uint8_t));

    mapgen_simplex(settings);
    mapgen_smooth_height(2 + (util_rand() % 6));

    // Set the game map to the height map
    mapgen_set_height();
    delete[] _height;

    // Set the tile slopes so that there are no cliffs
    while (map_smooth(1, 1, mapSize - 1, mapSize - 1)) {}

    // Add the water
    mapgen_set_water_level(waterLevel);

    // Add sandy beaches
    int32_t beachTexture = floorTexture;
    if (settings->floor == -1 && floorTexture == TERRAIN_GRASS)
    {
        switch (util_rand() % 4)
        {
        case 0:
            beachTexture = TERRAIN_SAND;
            break;
        case 1:
            beachTexture = TERRAIN_SAND_LIGHT;
            break;
        }
    }
    for (y = 1; y < mapSize - 1; y++)
    {
        for (x = 1; x < mapSize - 1; x++)
        {
            tileElement = map_get_surface_element_at(x, y);

            if (tileElement->base_height < waterLevel + 6)
                surface_set_terrain(tileElement, beachTexture);
        }
    }

    // Place the trees
    if (settings->trees != 0)
        mapgen_place_trees();

    map_reorganise_elements();
}

static void mapgen_place_tree(int32_t type, int32_t x, int32_t y)
{
    int32_t surfaceZ;
    rct_tile_element  * tileElement;
    rct_scenery_entry * sceneryEntry = get_small_scenery_entry(type);
    if (sceneryEntry == nullptr)
    {
        return;
    }

    surfaceZ    = tile_element_height(x * 32 + 16, y * 32 + 16) / 8;
    tileElement = tile_element_insert(x, y, surfaceZ, (1 | 2 | 4 | 8));
    assert(tileElement != nullptr);
    tileElement->clearance_height = surfaceZ + (sceneryEntry->small_scenery.height >> 3);

    tileElement->type                    = TILE_ELEMENT_TYPE_SMALL_SCENERY | (util_rand() & 3);
    tileElement->properties.scenery.type = type;
    tileElement->properties.scenery.age  = 0;
    scenery_small_set_primary_colour(tileElement, COLOUR_YELLOW);
}

/**
 * Randomly places a selection of preset trees on the map. Picks the right tree for the terrain it is placing it on.
 */
static void mapgen_place_trees()
{
    std::vector<int32_t> grassTreeIds(Util::CountOf(GrassTrees), 0);
    std::vector<int32_t> desertTreeIds(Util::CountOf(DesertTrees), 0);
    std::vector<int32_t> snowTreeIds(Util::CountOf(SnowTrees), 0);

    for (int32_t i = 0; i < object_entry_group_counts[OBJECT_TYPE_SMALL_SCENERY]; i++)
    {
        auto sceneryEntry = get_small_scenery_entry(i);
        auto entry = object_entry_get_entry(OBJECT_TYPE_SMALL_SCENERY, i);

        if (sceneryEntry == nullptr)
            continue;

        uint32_t j;
        for (j = 0; j < Util::CountOf(GrassTrees); j++)
        {
            if (strncmp(GrassTrees[j], entry->name, 8) == 0)
                break;
        }
        if (j != Util::CountOf(GrassTrees))
        {
            grassTreeIds.push_back(i);
            continue;
        }

        for (j = 0; j < Util::CountOf(DesertTrees); j++)
        {
            if (strncmp(DesertTrees[j], entry->name, 8) == 0)
                break;
        }
        if (j != Util::CountOf(DesertTrees))
        {
            desertTreeIds.push_back(i);
            continue;
        }

        for (j = 0; j < Util::CountOf(SnowTrees); j++)
        {
            if (strncmp(SnowTrees[j], entry->name, 8) == 0)
                break;
        }
        if (j != Util::CountOf(SnowTrees))
        {
            snowTreeIds.push_back(i);
            continue;
        }
    }

    CoordsXY tmp, pos;

    std::vector<CoordsXY> availablePositions;

    // Create list of available tiles
    for (int32_t y = 1; y < gMapSize - 1; y++)
    {
        for (int32_t x = 1; x < gMapSize - 1; x++)
        {
            rct_tile_element * tileElement = map_get_surface_element_at(x, y);

            // Exclude water tiles
            if (surface_get_water_height(tileElement) > 0)
                continue;

            pos.x = x;
            pos.y = y;
            availablePositions.push_back(pos);

        }
    }

    // Shuffle list
    for (uint32_t i = 0; i < availablePositions.size(); i++)
    {
        uint32_t rindex = util_rand() % availablePositions.size();
        if (rindex == i)
            continue;

        tmp = availablePositions[i];
        availablePositions[i]      = availablePositions[rindex];
        availablePositions[rindex] = tmp;
    }

    // Place trees
    float  treeToLandRatio = (10 + (util_rand() % 30)) / 100.0f;
    int32_t numTrees        = std::min(std::max(4, (int32_t) (availablePositions.size() * treeToLandRatio)), (int32_t)availablePositions.size());

    for (int32_t i = 0; i < numTrees; i++)
    {
        pos = availablePositions[i];

        int32_t type = -1;
        rct_tile_element * tileElement = map_get_surface_element_at(pos.x, pos.y);
        switch (surface_get_terrain(tileElement))
        {
        case TERRAIN_GRASS:
        case TERRAIN_DIRT:
        case TERRAIN_GRASS_CLUMPS:
            if (grassTreeIds.size() == 0)
                break;

            type = grassTreeIds[util_rand() % grassTreeIds.size()];
            break;

        case TERRAIN_SAND:
        case TERRAIN_SAND_DARK:
        case TERRAIN_SAND_LIGHT:
            if (desertTreeIds.size() == 0)
                break;

            if (util_rand() % 4 == 0)
                type = desertTreeIds[util_rand() % desertTreeIds.size()];
            break;

        case TERRAIN_ICE:
            if (snowTreeIds.size() == 0)
                break;

            type = snowTreeIds[util_rand() % snowTreeIds.size()];
            break;
        }

        if (type != -1)
            mapgen_place_tree(type, pos.x, pos.y);
    }
}

/**
 * Sets each tile's water level to the specified water level if underneath that water level.
 */
static void mapgen_set_water_level(int32_t waterLevel)
{
    int32_t x, y, mapSize;
    rct_tile_element * tileElement;

    mapSize = gMapSize;

    for (y = 1; y < mapSize - 1; y++)
    {
        for (x = 1; x < mapSize - 1; x++)
        {
            tileElement = map_get_surface_element_at(x, y);
            if (tileElement->base_height < waterLevel)
                tileElement->properties.surface.terrain |= (waterLevel / 2);
        }
    }
}

/**
 * Smooths the height map.
 */
static void mapgen_smooth_height(int32_t iterations)
{
    int32_t i, x, y, xx, yy, avg;
    int32_t arraySize = _heightSize * _heightSize * sizeof(uint8_t);
    uint8_t * copyHeight = new uint8_t[arraySize];

    for (i = 0; i < iterations; i++)
    {
        memcpy(copyHeight, _height, arraySize);
        for (y = 1; y < _heightSize - 1; y++)
        {
            for (x = 1; x < _heightSize - 1; x++)
            {
                avg     = 0;
                for (yy = -1; yy <= 1; yy++)
                {
                    for (xx = -1; xx <= 1; xx++)
                    {
                        avg += copyHeight[(y + yy) * _heightSize + (x + xx)];
                    }
                }
                avg /= 9;
                set_height(x, y, avg);
            }
        }
    }

    delete[] copyHeight;
}

/**
 * Sets the height of the actual game map tiles to the height map.
 */
static void mapgen_set_height()
{
    int32_t x, y, heightX, heightY, mapSize;
    rct_tile_element * tileElement;

    mapSize = _heightSize / 2;
    for (y  = 1; y < mapSize - 1; y++)
    {
        for (x = 1; x < mapSize - 1; x++)
        {
            heightX = x * 2;
            heightY = y * 2;

            uint8_t q00 = get_height(heightX + 0, heightY + 0);
            uint8_t q01 = get_height(heightX + 0, heightY + 1);
            uint8_t q10 = get_height(heightX + 1, heightY + 0);
            uint8_t q11 = get_height(heightX + 1, heightY + 1);

            uint8_t baseHeight = (q00 + q01 + q10 + q11) / 4;

            tileElement = map_get_surface_element_at(x, y);
            tileElement->base_height      = std::max(2, baseHeight * 2);
            tileElement->clearance_height = tileElement->base_height;

            if (q00 > baseHeight)
                tileElement->properties.surface.slope |= TILE_ELEMENT_SLOPE_S_CORNER_UP;
            if (q01 > baseHeight)
                tileElement->properties.surface.slope |= TILE_ELEMENT_SLOPE_W_CORNER_UP;
            if (q10 > baseHeight)
                tileElement->properties.surface.slope |= TILE_ELEMENT_SLOPE_E_CORNER_UP;
            if (q11 > baseHeight)
                tileElement->properties.surface.slope |= TILE_ELEMENT_SLOPE_N_CORNER_UP;
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
static int32_t fast_floor(float x);
static float grad(int32_t hash, float x, float y);

static uint8_t perm[512];

static void noise_rand()
{
    for (auto &i : perm)
    {
        i = util_rand() & 0xFF;
    }
}

static float fractal_noise(int32_t x, int32_t y, float frequency, int32_t octaves, float lacunarity, float persistence)
{
    float       total     = 0.0f;
    float       amplitude = persistence;
    for (int32_t i         = 0; i < octaves; i++)
    {
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
    float  s  = (x + y) * F2; // Hairy factor for 2D
    float  xs = x + s;
    float  ys = y + s;
    int32_t i  = fast_floor(xs);
    int32_t j  = fast_floor(ys);

    float t  = (float) (i + j) * G2;
    float X0 = i - t; // Unskew the cell origin back to (x,y) space
    float Y0 = j - t;
    float x0 = x - X0; // The x,y distances from the cell origin
    float y0 = y - Y0;

    // For the 2D case, the simplex shape is an equilateral triangle.
    // Determine which simplex we are in.
    int32_t i1, j1; // Offsets for second (middle) corner of simplex in (i,j) coords
    if (x0 > y0)
    {
        i1 = 1;
        j1 = 0;
    } // lower triangle, XY order: (0,0)->(1,0)->(1,1)
    else
    {
        i1 = 0;
        j1 = 1;
    }      // upper triangle, YX order: (0,0)->(0,1)->(1,1)

    // A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
    // a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
    // c = (3-sqrt(3))/6

    float x1 = x0 - i1 + G2; // Offsets for middle corner in (x,y) unskewed coords
    float y1 = y0 - j1 + G2;
    float x2 = x0 - 1.0f + 2.0f * G2; // Offsets for last corner in (x,y) unskewed coords
    float y2 = y0 - 1.0f + 2.0f * G2;

    // Wrap the integer indices at 256, to avoid indexing perm[] out of bounds
    int32_t ii = i % 256;
    int32_t jj = j % 256;

    // Calculate the contribution from the three corners
    float t0 = 0.5f - x0 * x0 - y0 * y0;
    if (t0 < 0.0f)
    {
        n0 = 0.0f;
    }
    else
    {
        t0 *= t0;
        n0 = t0 * t0 * grad(perm[ii + perm[jj]], x0, y0);
    }

    float t1 = 0.5f - x1 * x1 - y1 * y1;
    if (t1 < 0.0f)
    {
        n1 = 0.0f;
    }
    else
    {
        t1 *= t1;
        n1 = t1 * t1 * grad(perm[ii + i1 + perm[jj + j1]], x1, y1);
    }

    float t2 = 0.5f - x2 * x2 - y2 * y2;
    if (t2 < 0.0f)
    {
        n2 = 0.0f;
    }
    else
    {
        t2 *= t2;
        n2 = t2 * t2 * grad(perm[ii + 1 + perm[jj + 1]], x2, y2);
    }

    // Add contributions from each corner to get the final noise value.
    // The result is scaled to return values in the interval [-1,1].
    return 40.0f * (n0 + n1 + n2); // TODO: The scale factor is preliminary!
}

static int32_t fast_floor(float x)
{
    return (x > 0) ? ((int32_t) x) : (((int32_t) x) - 1);
}

static float grad(int32_t hash, float x, float y)
{
    int32_t h = hash & 7;      // Convert low 3 bits of hash code
    float  u = h < 4 ? x : y;  // into 8 simple gradient directions,
    float  v = h < 4 ? y : x;  // and compute the dot product with (x,y).
    return ((h & 1) != 0 ? -u : u) + ((h & 2) != 0 ? -2.0f * v : 2.0f * v);
}

static void mapgen_simplex(mapgen_settings * settings)
{
    int32_t x, y;

    float  freq    = settings->simplex_base_freq * (1.0f / _heightSize);
    int32_t octaves = settings->simplex_octaves;

    int32_t low  = settings->simplex_low;
    int32_t high = settings->simplex_high;

    noise_rand();
    for (y = 0; y < _heightSize; y++)
    {
        for (x = 0; x < _heightSize; x++)
        {
            float noiseValue           = Math::Clamp(-1.0f, fractal_noise(x, y, freq, octaves, 2.0f, 0.65f), 1.0f);
            float normalisedNoiseValue = (noiseValue + 1.0f) / 2.0f;

            set_height(x, y, low + (int32_t) (normalisedNoiseValue * high));
        }
    }
}

#pragma endregion

#pragma region Heightmap

bool mapgen_load_heightmap(const utf8 * path)
{
    auto format = Imaging::GetImageFormatFromPath(path);
    if (format == IMAGE_FORMAT::PNG)
    {
        // Promote to 32-bit
        format = IMAGE_FORMAT::PNG_32;
    }

    try
    {
        auto image = Imaging::ReadFromFile(path, format);
        if (image.Width != image.Height)
        {
            context_show_error(STR_HEIGHT_MAP_ERROR, STR_ERROR_WIDTH_AND_HEIGHT_DO_NOT_MATCH);
            return false;
        }

        auto size = image.Width;
        if (image.Width > MAXIMUM_MAP_SIZE_PRACTICAL)
        {
            context_show_error(STR_HEIGHT_MAP_ERROR, STR_ERROR_HEIHGT_MAP_TOO_BIG);
            size = std::min<uint32_t>(image.Height, MAXIMUM_MAP_SIZE_PRACTICAL);
        }

        // Allocate memory for the height map values, one byte pixel
        delete[] _heightMapData.mono_bitmap;
        _heightMapData.mono_bitmap = new uint8_t[size * size];
        _heightMapData.width = size;
        _heightMapData.height = size;

        // Copy average RGB value to mono bitmap
        constexpr auto numChannels = 4;
        const auto pitch = image.Stride;
        const auto pixels = image.Pixels.data();
        for (uint32_t x = 0; x < _heightMapData.width; x++)
        {
            for (uint32_t y = 0; y < _heightMapData.height; y++)
            {
                const auto red = pixels[x * numChannels + y * pitch];
                const auto green = pixels[x * numChannels + y * pitch + 1];
                const auto blue = pixels[x * numChannels + y * pitch + 2];
                _heightMapData.mono_bitmap[x + y * _heightMapData.width] = (red + green + blue) / 3;
            }
        }
        return true;
    }
    catch (const std::exception& e)
    {
        switch (format)
        {
            case IMAGE_FORMAT::BITMAP:
                context_show_error(STR_HEIGHT_MAP_ERROR, STR_ERROR_READING_BITMAP);
                break;
            case IMAGE_FORMAT::PNG_32:
                context_show_error(STR_HEIGHT_MAP_ERROR, STR_ERROR_READING_PNG);
                break;
            default:
                log_error("Unable to load height map image: %s", e.what());
                break;
        }
        return false;
    }
}

/**
 * Frees the memory used to store the selected height map
 */
void mapgen_unload_heightmap()
{
    SafeDeleteArray(_heightMapData.mono_bitmap);
    _heightMapData.width       = 0;
    _heightMapData.height      = 0;
}

/**
 * Applies box blur to the surface N times
 */
static void mapgen_smooth_heightmap(uint8_t * src, int32_t strength)
{
    // Create buffer to store one channel
    uint8_t * dest = new uint8_t[_heightMapData.width * _heightMapData.height];

    for (int32_t i = 0; i < strength; i++)
    {
        // Calculate box blur value to all pixels of the surface
        for (uint32_t y = 0; y < _heightMapData.height; y++)
        {
            for (uint32_t x = 0; x < _heightMapData.width; x++)
            {
                uint32_t heightSum = 0;

                // Loop over neighbour pixels, all of them have the same weight
                for (int8_t offsetX = -1; offsetX <= 1; offsetX++)
                {
                    for (int8_t offsetY = -1; offsetY <= 1; offsetY++)
                    {
                        // Clamp x and y so they stay within the image
                        // This assumes the height map is not tiled, and increases the weight of the edges
                        const int32_t readX = Math::Clamp((int32_t) x + offsetX, 0, (int32_t) _heightMapData.width - 1);
                        const int32_t readY = Math::Clamp((int32_t) y + offsetY, 0, (int32_t) _heightMapData.height - 1);
                        heightSum += src[readX + readY * _heightMapData.width];
                    }
                }

                // Take average
                dest[x + y * _heightMapData.width] = heightSum / 9;
            }
        }

        // Now apply the blur to the source pixels
        for (uint32_t y = 0; y < _heightMapData.height; y++)
        {
            for (uint32_t x = 0; x < _heightMapData.width; x++)
            {
                src[x + y * _heightMapData.width] = dest[x + y * _heightMapData.width];
            }
        }
    }

    delete[] dest;
}

void mapgen_generate_from_heightmap(mapgen_settings * settings)
{
    openrct2_assert(_heightMapData.width == _heightMapData.height, "Invalid height map size");
    openrct2_assert(_heightMapData.mono_bitmap != nullptr, "No height map loaded");
    openrct2_assert(settings->simplex_high != settings->simplex_low, "Low and high setting cannot be the same");

    // Make a copy of the original height map that we can edit
    uint8_t * dest = new uint8_t[_heightMapData.width * _heightMapData.height];
    memcpy(dest, _heightMapData.mono_bitmap, _heightMapData.width * _heightMapData.width);

    map_init(_heightMapData.width + 2); // + 2 for the black tiles around the map

    if (settings->smooth_height_map)
    {
        mapgen_smooth_heightmap(dest, settings->smooth_strength);
    }

    uint8_t maxValue = 255;
    uint8_t minValue = 0;

    if (settings->normalize_height)
    {
        // Get highest and lowest pixel value
        maxValue = 0;
        minValue = 0xff;
        for (uint32_t y = 0; y < _heightMapData.height; y++)
        {
            for (uint32_t x = 0; x < _heightMapData.width; x++)
            {
                uint8_t value = dest[x + y * _heightMapData.width];
                maxValue    = std::max(maxValue, value);
                minValue    = std::min(minValue, value);
            }
        }

        if (minValue == maxValue)
        {
            context_show_error(STR_HEIGHT_MAP_ERROR, STR_ERROR_CANNOT_NORMALIZE);
            delete[] dest;
            return;
        }
    }

    openrct2_assert(maxValue > minValue, "Input range is invalid");
    openrct2_assert(settings->simplex_high > settings->simplex_low, "Output range is invalid");

    const uint8_t rangeIn  = maxValue - minValue;
    const uint8_t rangeOut = settings->simplex_high - settings->simplex_low;

    for (uint32_t y = 0; y < _heightMapData.height; y++)
    {
        for (uint32_t x = 0; x < _heightMapData.width; x++)
        {
            // The x and y axis are flipped in the world, so this uses y for x and x for y.
            rct_tile_element * const surfaceElement = map_get_surface_element_at(y + 1, x + 1);

            // Read value from bitmap, and convert its range
            uint8_t value = dest[x + y * _heightMapData.width];
            value = (uint8_t) ((float) (value - minValue) / rangeIn * rangeOut) + settings->simplex_low;
            surfaceElement->base_height      = value;

            // Floor to even number
            surfaceElement->base_height /= 2;
            surfaceElement->base_height *= 2;
            surfaceElement->clearance_height = surfaceElement->base_height;

            // Set water level
            if (surfaceElement->base_height < settings->water_level)
            {
                surfaceElement->properties.surface.terrain |= settings->water_level / 2;
            }
        }
    }

    // Smooth map
    if (settings->smooth)
    {
        // Keep smoothing the entire map until no tiles are changed anymore
        while (true)
        {
            uint32_t numTilesChanged = 0;
            for (uint32_t y = 1; y <= _heightMapData.height; y++)
            {
                for (uint32_t x = 1; x <= _heightMapData.width; x++)
                {
                    numTilesChanged += tile_smooth(x, y);
                }
            }

            if (numTilesChanged == 0)
                break;
        }
    }

    // Clean up
    delete[] dest;
}

#pragma endregion
