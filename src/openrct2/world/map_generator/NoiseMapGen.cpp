/*****************************************************************************
 * Copyright (c) 2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "NoiseMapGen.h"

#include "../../Diagnostic.h"
#include "../../util/Util.h"
#include "Erosion.h"
#include "MapHelpers.h"
#include "Noise.h"

#include <memory>
#include <variant>

namespace OpenRCT2::World::MapGenerator
{
    using BiasData = std::variant<std::unique_ptr<Noise>, VecXY>;

    /**
     * Smooths the height map.
     * TODO deduplicate smoothing functions
     */
    static void smoothHeightMap(int32_t iterations, HeightMap& heightMap)
    {
        for (auto i = 0; i < iterations; i++)
        {
            auto copyHeight = heightMap;
            for (auto y = 0; y < heightMap.height; y++)
            {
                for (auto x = 0; x < heightMap.width; x++)
                {
                    auto avg = 0;
                    for (auto yy = -1; yy <= 1; yy++)
                    {
                        for (auto xx = -1; xx <= 1; xx++)
                        {
                            auto dx = x + xx;
                            auto dy = y + yy;

                            // Use tile height if OOB
                            auto oob = dy < 0 || dx < 0 || dy >= heightMap.width || dx >= heightMap.height;
                            avg += oob ? copyHeight[{ x, y }] : copyHeight[{ dx, dy }];
                        }
                    }
                    avg /= 9.0f;
                    heightMap[{ x, y }] = avg;
                }
            }
        }
    }

    static BiasData prepareBias(Settings* settings)
    {
        BiasData ctx;
        if (settings->bias == Bias::coastal || settings->bias == Bias::river)
        {
            std::mt19937 prng(settings->seed);
            std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
            auto direction = VecXY{ dist(prng), dist(prng) };
            ctx = direction.Normalize();
        }
        else if (settings->bias == Bias::canyon)
        {
            ctx = std::make_unique<RidgeFbmNoise>(settings->seed + 1, 1.0f / 384.0f, 6, 2.5f, .55f);
        }
        else if (settings->bias == Bias::mountain)
        {
            ctx = std::make_unique<RidgeFbmNoise>(settings->seed + 1, 1.0f / 768.0f, 7, 2.5f, .5f);
        }
        else if (settings->bias == Bias::cliff)
        {
            ctx = std::make_unique<SimplexFbmNoise>(settings->seed + 1, 1.0f / 512.0f, 6, 2.25f, .5f);
        }

        return ctx;
    }

    static float applyBias(Settings* settings, BiasData& ctx, VecXY pos, float noise)
    {
        float biasStrength = static_cast<float>(settings->bias_strength) / 100.0f;

        float nx = 2.0f * pos.x / settings->mapSize.x - 1.0f;
        float ny = 2.0f * pos.y / settings->mapSize.y - 1.0f;
        auto normPos = VecXY{ nx, ny };

        switch (settings->bias)
        {
            case Bias::none:
            {
                return noise;
            }
            case Bias::island:
            {
                float d = std::pow(1.0f - (1.0f - std::pow(nx, 2)) * (1.0f - std::pow(ny, 2)), 1.75f);
                return Smoothstep(0.0f, 1.0f, 1.0f - d * biasStrength) * noise;
            }
            case Bias::valley:
            {
                float d = std::pow(1.0f - (1.0f - std::pow(nx, 2)) * (1.0f - std::pow(ny, 2)), 1.25f);
                return Smoothstep(0.0f, 1.0f, 1.0f - (1.0f - d) * biasStrength) * noise;
            }
            case Bias::coastal:
            {
                // TODO offset center (or simplify the distance calc below)?
                VecXY c{ 0.0f, 0.0f };
                VecXY b = std::get<VecXY>(ctx);
                VecXY p = normPos;

                float d = ((b.y - c.y) * p.x - (b.x - c.x) * p.y + b.x * c.y - b.y * c.x)
                    / sqrt(pow(b.x - c.x, 2) + pow(b.y - c.y, 2));

                d = (std::pow(d, 3) + 1.0f) / 2.0f;

                return Smoothstep(0.0f, 1.0f, 1.0f - (1.0f - d) * biasStrength) * noise;
            }
            case Bias::river:
            {
                // TODO offset center (or simplify the distance calc below)?
                VecXY c{ 0.0f, 0.0f };
                VecXY b = std::get<VecXY>(ctx);
                VecXY p = normPos;

                float d = std::abs((b.y - c.y) * p.x - (b.x - c.x) * p.y + b.x * c.y - b.y * c.x)
                    / sqrt(pow(b.x - c.x, 2) + pow(b.y - c.y, 2));

                auto riverScale = 1.0f + biasStrength * 1.5f;
                auto riverBias = riverScale * std::pow(d, 1.5f);
                return Smoothstep(0.0f, 1.0f, 1.0f - (1.0f - riverBias) * biasStrength) * noise;
            }
            case Bias::canyon:
            {
                auto canyonScale = 3.0f + biasStrength * 3.0f;
                auto canyonNoise = canyonScale * std::pow(std::get<std::unique_ptr<Noise>>(ctx)->Generate(pos), 3);
                return Smoothstep(0.0f, 1.0f, 1.0f - canyonNoise * biasStrength) * noise;
            }
            case Bias::mountain:
            {
                auto mountainScale = 2.0f + biasStrength * 2.0f;
                auto mountainNoise = mountainScale * std::pow(std::get<std::unique_ptr<Noise>>(ctx)->Generate(pos), 2);
                return Smoothstep(0.0f, 1.0f, 1.0f - (1.0f - mountainNoise) * biasStrength) * noise;
            }
            case Bias::cliff:
            {
                auto cliffBias = std::get<std::unique_ptr<Noise>>(ctx)->Generate(pos) > 0.2f ? 1.0f : 0.0f;
                return noise * (1 - biasStrength) + cliffBias * biasStrength;
            }
            default:
            {
                LOG_ERROR("Unknown noise bias");
                return noise;
            }
        }
    }

    static void generateMap(Settings* settings, HeightMap& heightMap, Noise& noise)
    {
        BiasData ctx = prepareBias(settings);

        float low = settings->heightmapLow;
        float high = settings->heightmapHigh - low;

        for (int32_t y = 0; y < heightMap.height; y++)
        {
            for (int32_t x = 0; x < heightMap.width; x++)
            {
                VecXY pos = { x, y };
                float noiseValue = noise.Generate(pos);
                float normalisedNoiseValue = (noiseValue + 1.0f) / 2.0f;
                float biasedNoiseValue = applyBias(settings, ctx, pos, normalisedNoiseValue);

                heightMap[{ x, y }] = low + biasedNoiseValue * high;
            }
        }
    }

    static void generateCommon(Settings* settings, HeightMap& heightMap)
    {
        if (settings->simulate_erosion)
        {
            auto erosionSettings = ErosionSettings(*settings);
            simulateErosion(erosionSettings, heightMap);
        }
        else
        {
            std::mt19937 prng(settings->seed);
            std::uniform_int_distribution<int32_t> dist(0, 5);
            smoothHeightMap(2 + dist(prng), heightMap);
        }

        // Set the game map to the height map
        resetSurfaces(settings);
        setMapHeight(settings, heightMap);

        if (settings->smoothTileEdges)
        {
            // Set the tile slopes so that there are no cliffs, use the weak version with erosion.
            smoothMap(settings->mapSize, settings->simulate_erosion ? smoothTileWeak : smoothTileStrong);
        }

        // Add the water
        setWaterLevel(settings->waterLevel);
    }

    void generateSimplexMap(Settings* settings)
    {
        auto heightMap = HeightMap(settings->mapSize);

        // TODO should freq really be influenced by map width?
        float freq = settings->simplex_base_freq / 100.0f * (1.0f / heightMap.width);

        auto simplexFbmNoise = SimplexFbmNoise{ settings->seed, freq, settings->simplex_octaves, 2.0f, 0.65f };

        generateMap(settings, heightMap, simplexFbmNoise);
        generateCommon(settings, heightMap);
    }

    void generateWarpedMap(Settings* settings)
    {
        auto heightMap = HeightMap(settings->mapSize);
        VecXY domainSize{ settings->mapSize.x, settings->mapSize.y };

        auto freq = (settings->simplex_base_freq / 100.0f) * 0.095f;
        auto warpedNoise = SimplexWarpedNoise{ settings->seed, freq, settings->simplex_octaves, 2.0f, 0.5f, domainSize };

        generateMap(settings, heightMap, warpedNoise);
        generateCommon(settings, heightMap);
    }
} // namespace OpenRCT2::World::MapGenerator
