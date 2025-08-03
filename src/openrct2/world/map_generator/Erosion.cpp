/*****************************************************************************
 * Copyright (c) 2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Erosion.h"

#include "../../Context.h"
#include "../../Diagnostic.h"
#include "../../GameState.h"
#include "../Vec.hpp"

namespace OpenRCT2::World::MapGenerator
{
    constexpr uint32_t UPDATE_EVERY_N_PARTICLES = 5000;

    constexpr float WEIGHT_CARDINAL = 0.18f;
    constexpr float WEIGHT_ORDINAL = 0.05f;

    struct Particle
    {
        Particle(VecXY _position)
        {
            position = _position;
        }
        VecXY position;
        VecXY speed = VecXY(0.0f, 0.0f);
        float volume = 1.0f;
        float sediment = 0.0f;
    };

    /**
     * Calculate the surface normal at the given position.
     */
    static VecXYZ surfaceNormal(HeightMap& heightMap, TileCoordsXY pos)
    {
        auto normal = VecXYZ(0.0f, 0.0f, 0.0f);

        auto deltaN = heightMap[pos] - heightMap[pos.Offset(1, 0)];
        auto deltaS = heightMap[pos] - heightMap[pos.Offset(-1, 0)];
        auto deltaE = heightMap[pos] - heightMap[pos.Offset(0, 1)];
        auto deltaW = heightMap[pos] - heightMap[pos.Offset(0, -1)];

        normal += VecXYZ(deltaN, 0.0f, 1.0f).Normalize() * WEIGHT_CARDINAL;
        normal += VecXYZ(-deltaS, 0.0f, 1.0f).Normalize() * WEIGHT_CARDINAL;
        normal += VecXYZ(0.0f, deltaE, 1.0f).Normalize() * WEIGHT_CARDINAL;
        normal += VecXYZ(0.0f, -deltaW, 1.0f).Normalize() * WEIGHT_CARDINAL;

        auto deltaNE = heightMap[pos] - heightMap[pos.Offset(+1, +1)];
        auto deltaNW = heightMap[pos] - heightMap[pos.Offset(+1, -1)];
        auto deltaSE = heightMap[pos] - heightMap[pos.Offset(-1, +1)];
        auto deltaSW = heightMap[pos] - heightMap[pos.Offset(-1, -1)];

        normal += VecXYZ(deltaNE, deltaNE, 1.0f).Normalize() * WEIGHT_ORDINAL;
        normal += VecXYZ(deltaNW, -deltaNW, 1.0f).Normalize() * WEIGHT_ORDINAL;
        normal += VecXYZ(-deltaSE, deltaSE, 1.0f).Normalize() * WEIGHT_ORDINAL;
        normal += VecXYZ(-deltaSW, -deltaSW, 1.0f).Normalize() * WEIGHT_ORDINAL;

        return normal.Normalize();
    }

    /**
     * Simulate hydraulic terrain erosion.
     *
     * Based on https://nickmcd.me/2020/04/10/simple-particle-based-hydraulic-erosion (https://github.com/weigert/SimpleErosion)
     */
    void simulateErosion(ErosionSettings& settings, HeightMap& heightMap)
    {
        GetContext()->OpenProgress(STR_EROSION_PROGRESS);

        for (auto i = 0; i < settings.particles; ++i)
        {
            if (i % UPDATE_EVERY_N_PARTICLES == 0)
            {
                LOG_VERBOSE("Erosion particle %i/%i", i, settings.particles);
                GetContext()->SetProgress(i, settings.particles);
            }

            // spawn new particle at a random location, border of 1 for normal calculation
            // TODO use a dedicated prng initialized by the map seed (once made configurable)
            auto particle = Particle(
                VecXY(1 + ScenarioRandMax(heightMap.width - 2), 1 + ScenarioRandMax(heightMap.height - 2)));

            // abort if the particle has evaporated
            while (particle.volume > settings.minVolume)
            {
                auto initialPosition = particle.position.AsTileCoordsXY();
                auto normal = surfaceNormal(heightMap, initialPosition);

                // accelerate particle and update position
                particle.speed += VecXY{ normal.x, normal.y } / (particle.volume * settings.density);
                particle.position += particle.speed * settings.dt;
                particle.speed *= 1.0f - settings.dt * settings.friction * particle.volume;

                auto updatedPosition = particle.position.AsTileCoordsXY();

                // check if particle is still within map/border
                if (updatedPosition.x < 1 || updatedPosition.y < 1 || updatedPosition.x >= heightMap.width - 1
                    || updatedPosition.y >= heightMap.height - 1)
                {
                    break;
                }

                // figure out how much sediment to transfer between particle and map; Compare the equilibrium (~ carrying
                // capacity, scales positively with speed and downward slope) with how much sediment is currently dissolved in
                // the particle. If the delta is positive dissolve more sediment, if it is negative deposit sediment.
                auto heightDelta = heightMap[initialPosition] - heightMap[updatedPosition];
                auto sedimentEquilibrium = std::max(0.0f, particle.volume * particle.speed.Length() * heightDelta);
                auto sedimentDelta = sedimentEquilibrium - particle.sediment;
                auto sedimentTransfer = settings.dt * settings.depositionRate * sedimentDelta;

                // transfer sediment between particle and map
                particle.sediment += sedimentTransfer;
                heightMap[initialPosition] -= particle.volume * sedimentTransfer;

                // reduce volume by evaporation
                particle.volume *= 1.0f - settings.dt * settings.evaporationRate;
            }
        }

        GetContext()->CloseProgress();
    }
} // namespace OpenRCT2::World::MapGenerator
