/*****************************************************************************
 * Copyright (c) 2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Noise.h"

#include "../../util/Util.h"

#include <random>

namespace OpenRCT2::World::MapGenerator
{
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

    float Fbm::Generate(VecXY pos, const NoiseFn& noise)
    {
        float total = 0.0f;
        float amplitude = _persistence;
        float frequency = _frequency;
        for (int32_t i = 0; i < _octaves; i++)
        {
            total += noise(pos * frequency) * amplitude;
            frequency *= _lacunarity;
            amplitude *= _persistence;
        }
        return total;
    }

    /**
     * Based on:
     *   - https://iquilezles.org/articles/warp/
     */
    float DomainWarp::Generate(VecXY pos, const NoiseFn& noise)
    {
        // need to normalize input coords to [-1, 1]
        auto p = VecXY{ 2.0f * pos.x / _domainSize.x - 1.0f, 2.0f * pos.y / _domainSize.y - 1.0f };

        auto q = VecXY{ _fbm.Generate(p + _offsetA, noise), _fbm.Generate(p + _offsetB, noise) };
        auto r = VecXY{ _fbm.Generate(p + q * 4.0f + _offsetC, noise), _fbm.Generate(p + q * 4.0f + _offsetD, noise) };

        return _fbm.Generate(p + r * 4.0f, noise);
    }

    /**
     * Simplex Noise Algorithm with Fractional Brownian Motion
     * Based on:
     *   - https://code.google.com/p/simplexnoise/
     *   - https://code.google.com/p/fractalterraingeneration/wiki/Fractional_Brownian_Motion
     */
    SimplexNoise::SimplexNoise()
    {
        for (auto& i : _perm)
        {
            i = UtilRandUniformDistributedByte();
        }
    }

    SimplexNoise::SimplexNoise(uint32_t seed)
    {
        std::mt19937 prng(seed);
        // char types are not permitted as type params of uniform_int_distribution...
        std::uniform_int_distribution<uint16_t> uniDist(0, 255);
        for (auto& i : _perm)
        {
            i = static_cast<uint8_t>(uniDist(prng));
        }
    }

    float RidgeNoise::Generate(VecXY pos)
    {
        auto absoluteNoise = std::abs(SimplexNoise::Generate(pos + 1024.0f));
        // renormalize to [-1, 1]
        return absoluteNoise * -2.0f + 1.0f;
    }

    float RidgeFbmNoise::Generate(VecXY pos)
    {
        return _fbm.Generate(pos, [this](VecXY p) { return RidgeNoise::Generate(p); });
    }

    float SimplexFbmNoise::Generate(VecXY pos)
    {
        return _fbm.Generate(pos, [this](VecXY p) { return SimplexNoise::Generate(p); });
    }

    float SimplexWarpedNoise::Generate(VecXY pos)
    {
        return _domainWarp.Generate(pos, [this](VecXY p) { return SimplexNoise::Generate(p); });
    }

    float SimplexNoise::Generate(VecXY pos)
    {
        // TODO is this impl biased around [0,0]?
        // pos = pos + 1024.0f;

        const float F2 = 0.366025403f; // F2 = 0.5*(sqrt(3.0)-1.0)
        const float G2 = 0.211324865f; // G2 = (3.0-sqrt(3.0))/6.0

        float n0, n1, n2; // Noise contributions from the three corners

        // Skew the input space to determine which simplex cell we're in
        float s = (pos.x + pos.y) * F2; // Hairy factor for 2D
        float xs = pos.x + s;
        float ys = pos.y + s;
        int32_t i = FastFloor(xs);
        int32_t j = FastFloor(ys);

        float t = static_cast<float>(i + j) * G2;
        float X0 = i - t; // Unskew the cell origin back to (x,y) space
        float Y0 = j - t;
        float x0 = pos.x - X0; // The x,y distances from the cell origin
        float y0 = pos.y - Y0;

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
            n0 = t0 * t0 * Grad(_perm[ii + _perm[jj]], x0, y0);
        }

        float t1 = 0.5f - x1 * x1 - y1 * y1;
        if (t1 < 0.0f)
        {
            n1 = 0.0f;
        }
        else
        {
            t1 *= t1;
            n1 = t1 * t1 * Grad(_perm[ii + i1 + _perm[jj + j1]], x1, y1);
        }

        float t2 = 0.5f - x2 * x2 - y2 * y2;
        if (t2 < 0.0f)
        {
            n2 = 0.0f;
        }
        else
        {
            t2 *= t2;
            n2 = t2 * t2 * Grad(_perm[ii + 1 + _perm[jj + 1]], x2, y2);
        }

        // Add contributions from each corner to get the final noise value.
        // The result is scaled to return values in the interval [-1,1].
        return 40.0f * (n0 + n1 + n2); // TODO: The scale factor is preliminary!
    }
} // namespace OpenRCT2::World::MapGenerator
