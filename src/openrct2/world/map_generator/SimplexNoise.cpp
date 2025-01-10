/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SimplexNoise.h"

#include "../../util/Util.h"
#include "HeightMap.hpp"
#include "MapGen.h"
#include "MapHelpers.h"

#include <algorithm>

namespace OpenRCT2::World::MapGenerator
{
    /**
     * Simplex Noise Algorithm with Fractional Brownian Motion
     * Based on:
     *   - https://code.google.com/p/simplexnoise/
     *   - https://code.google.com/p/fractalterraingeneration/wiki/Fractional_Brownian_Motion
     */

    static float Generate(float x, float y);
    static int32_t FastFloor(float x);
    static float Grad(int32_t hash, float x, float y);

    static uint8_t perm[512];

    void NoiseRand()
    {
        for (auto& i : perm)
        {
            i = UtilRand() & 0xFF;
        }
    }

    float FractalNoise(int32_t x, int32_t y, float frequency, int32_t octaves, float lacunarity, float persistence)
    {
        float total = 0.0f;
        float amplitude = persistence;
        for (int32_t i = 0; i < octaves; i++)
        {
            total += Generate(x * frequency, y * frequency) * amplitude;
            frequency *= lacunarity;
            amplitude *= persistence;
        }
        return total;
    }

    static float Generate(float x, float y)
    {
        const float F2 = 0.366025403f; // F2 = 0.5*(sqrt(3.0)-1.0)
        const float G2 = 0.211324865f; // G2 = (3.0-sqrt(3.0))/6.0

        float n0, n1, n2; // Noise contributions from the three corners

        // Skew the input space to determine which simplex cell we're in
        float s = (x + y) * F2; // Hairy factor for 2D
        float xs = x + s;
        float ys = y + s;
        int32_t i = FastFloor(xs);
        int32_t j = FastFloor(ys);

        float t = static_cast<float>(i + j) * G2;
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
        } // upper triangle, YX order: (0,0)->(0,1)->(1,1)

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
            n0 = t0 * t0 * Grad(perm[ii + perm[jj]], x0, y0);
        }

        float t1 = 0.5f - x1 * x1 - y1 * y1;
        if (t1 < 0.0f)
        {
            n1 = 0.0f;
        }
        else
        {
            t1 *= t1;
            n1 = t1 * t1 * Grad(perm[ii + i1 + perm[jj + j1]], x1, y1);
        }

        float t2 = 0.5f - x2 * x2 - y2 * y2;
        if (t2 < 0.0f)
        {
            n2 = 0.0f;
        }
        else
        {
            t2 *= t2;
            n2 = t2 * t2 * Grad(perm[ii + 1 + perm[jj + 1]], x2, y2);
        }

        // Add contributions from each corner to get the final noise value.
        // The result is scaled to return values in the interval [-1,1].
        return 40.0f * (n0 + n1 + n2); // TODO: The scale factor is preliminary!
    }

    static int32_t FastFloor(float x)
    {
        return (x > 0) ? (static_cast<int32_t>(x)) : ((static_cast<int32_t>(x)) - 1);
    }

    static float Grad(int32_t hash, float x, float y)
    {
        int32_t h = hash & 7;    // Convert low 3 bits of hash code
        float u = h < 4 ? x : y; // into 8 simple gradient directions,
        float v = h < 4 ? y : x; // and compute the dot product with (x,y).
        return ((h & 1) != 0 ? -u : u) + ((h & 2) != 0 ? -2.0f * v : 2.0f * v);
    }

    /**
     * Smooths the height map.
     */
    static void smoothHeightMap(int32_t iterations, HeightMap& heightMap)
    {
        for (auto i = 0; i < iterations; i++)
        {
            auto copyHeight = heightMap;
            for (auto y = 1; y < heightMap.height - 1; y++)
            {
                for (auto x = 1; x < heightMap.width - 1; x++)
                {
                    auto avg = 0;
                    for (auto yy = -1; yy <= 1; yy++)
                    {
                        for (auto xx = -1; xx <= 1; xx++)
                        {
                            avg += copyHeight[{ y + yy, x + xx }];
                        }
                    }
                    avg /= 9;
                    heightMap[{ x, y }] = avg;
                }
            }
        }
    }

    static void generateSimplexNoise(Settings* settings, HeightMap& heightMap)
    {
        float freq = settings->simplex_base_freq / 100.0f * (1.0f / heightMap.width);
        int32_t octaves = settings->simplex_octaves;

        int32_t low = settings->heightmapLow / 2;
        int32_t high = settings->heightmapHigh / 2 - low;

        NoiseRand();
        for (int32_t y = 0; y < heightMap.height; y++)
        {
            for (int32_t x = 0; x < heightMap.width; x++)
            {
                float noiseValue = std::clamp(FractalNoise(x, y, freq, octaves, 2.0f, 0.65f), -1.0f, 1.0f);
                float normalisedNoiseValue = (noiseValue + 1.0f) / 2.0f;

                heightMap[{ x, y }] = low + static_cast<int32_t>(normalisedNoiseValue * high);
            }
        }
    }

    void generateSimplexMap(Settings* settings)
    {
        resetSurfaces(settings);

        // Create the temporary height map and initialise
        auto& mapSize = settings->mapSize;
        auto heightMap = HeightMap(mapSize.x * 2, mapSize.y * 2);

        generateSimplexNoise(settings, heightMap);
        smoothHeightMap(2 + (UtilRand() % 6), heightMap);

        // Set the game map to the height map
        setMapHeight(settings, heightMap);

        if (settings->smoothTileEdges)
        {
            // Set the tile slopes so that there are no cliffs
            while (MapSmooth(1, 1, mapSize.x - 1, mapSize.y - 1))
            {
            }
        }

        // Add the water
        setWaterLevel(settings->waterLevel);
    }
} // namespace OpenRCT2::World::MapGenerator
