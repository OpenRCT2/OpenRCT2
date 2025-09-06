/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Noise.h"

#include <cstdint>
#include <random>

namespace OpenRCT2::World::MapGenerator
{
    struct Settings;

    class SimplexNoise : public Noise
    {
    private:
        uint8_t _perm[512];

    public:
        SimplexNoise(uint32_t seed);
        SimplexNoise();
        float Generate(float x, float y) override;
    };

    class SimplexFbmNoise final : public SimplexNoise
    {
    private:
        float _frequency;
        int32_t _octaves;
        float _lacunarity;
        float _persistence;

    public:
        SimplexFbmNoise(float frequency, int32_t octaves, float lacunarity, float persistence)
            : SimplexFbmNoise(std::random_device{}(), frequency, octaves, lacunarity, persistence)
        {
        }
        SimplexFbmNoise(uint32_t seed, float frequency, int32_t octaves, float lacunarity, float persistence)
            : SimplexNoise(seed)
            , _frequency(frequency)
            , _octaves(octaves)
            , _lacunarity(lacunarity)
            , _persistence(persistence)
        {
        }
        float Generate(float x, float y) override;
    };

    void generateSimplexMap(Settings* settings);
} // namespace OpenRCT2::World::MapGenerator
