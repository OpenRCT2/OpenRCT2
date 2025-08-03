/*****************************************************************************
 * Copyright (c) 2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "HeightMap.hpp"
#include "MapGen.h"

namespace OpenRCT2::World::MapGenerator
{
    constexpr uint32_t kMinimumParticleCount = 5000;
    constexpr uint32_t kMaximumParticleCount = 500000;

    struct ErosionSettings
    {
        ErosionSettings() = default;
        ErosionSettings(Settings* settings)
        {
            particles = static_cast<uint32_t>(settings->particles_per_tile * settings->mapSize.x * settings->mapSize.y) / 100;
        }

        int32_t particles = 200000;

        float density = 2.0f;
        float evaporationRate = 0.001f;
        float depositionRate = 0.1f;
        float minVolume = 0.01f;
        float friction = 0.05f;
        float dt = 1.2f;
    };

    void simulateErosion(ErosionSettings& settings, HeightMap& heightMap);

} // namespace OpenRCT2::World::MapGenerator
