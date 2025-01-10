/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

namespace OpenRCT2::World::MapGenerator
{
    struct Settings;

    void NoiseRand();
    float FractalNoise(int32_t x, int32_t y, float frequency, int32_t octaves, float lacunarity, float persistence);

    void generateSimplexMap(Settings* settings);
} // namespace OpenRCT2::World::MapGenerator
