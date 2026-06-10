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
#include "../Map.h"

#include <memory>
#include <variant>

namespace OpenRCT2::World::MapGenerator
{
    using BiasData = std::variant<std::unique_ptr<Noise>, VecXY>;

    static BiasData prepareBias(const Settings& settings)
    {
        BiasData ctx;
        if (settings.bias == Bias::coastal || settings.bias == Bias::river)
        {
            std::mt19937 prng(settings.seed);
            std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
            auto direction = VecXY{ dist(prng), dist(prng) };
            ctx = direction.Normalize();
        }
        else if (settings.bias == Bias::canyon)
        {
            BaseSettings baseSettings = { BaseType::Simplex, settings.seed + 1, 1.0f / 384.0f };
            FractalSettings fractalSettings = { FractalType::Ridge, 6, 2.5f, 0.55f, 0.0f };
            ctx = std::make_unique<Noise>(baseSettings, fractalSettings, std::nullopt, std::nullopt );
        }
        else if (settings.bias == Bias::mountain)
        {
            BaseSettings baseSettings = { BaseType::Simplex, settings.seed + 1, 1.0f / 768.0f };
            FractalSettings fractalSettings = { FractalType::Ridge, 7, 2.5f, 0.5f, 0.0f };
            TransformSettings transformSettings = { { 256, 256 }, 0.0f };

            ctx = std::make_unique<Noise>(baseSettings, fractalSettings, transformSettings, std::nullopt );
        }

        return ctx;
    }

    static float cliffBias(const float height, const int32_t cliffs, const float noise)
    {
        const float n = static_cast<float>(cliffs);
        return (1 - n * height) * noise + height * std::floor((n + 1) * noise);
    }

    static float terraceBias(const float width, const int32_t terraces, const float noise)
    {
        const float n = static_cast<float>(terraces);
        const float u = n * noise;
        const float k = std::floor(u);
        const float t = u - k;
        const float g = (std::min((1.0f - width) / 2.0f, t) + std::max(0.0f, t - (1.0f + width) / 2.0f)) / (1.0f - width);
        return (k + g) / n;
    }

    static float applyBias(const Settings& settings, BiasData& ctx, VecXY pos, float noise)
    {
        float biasStrength = static_cast<float>(settings.biasStrength) / 100.0f;
        float biasSteps = settings.biasSteps;

        float nx = 2.0f * pos.x / settings.mapSize.x - 1.0f;
        float ny = 2.0f * pos.y / settings.mapSize.y - 1.0f;
        auto normPos = VecXY{ nx, ny };

        switch (settings.bias)
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
                auto canyonNoise = canyonScale * std::pow(std::get<std::unique_ptr<Noise>>(ctx)->generate(pos), 3);
                return Smoothstep(0.0f, 1.0f, 1.0f - canyonNoise * biasStrength) * noise;
            }
            case Bias::mountain:
            {
                auto mountainScale = 2.0f + biasStrength * 2.0f;
                auto mountainNoise = mountainScale * std::pow(std::get<std::unique_ptr<Noise>>(ctx)->generate(pos), 2);
                return Smoothstep(0.0f, 1.0f, 1.0f - (1.0f - mountainNoise) * biasStrength) * noise;
            }
            case Bias::cliff:
            {
                // TODO improve height and strength scaling
                return noise * (1 - biasStrength) + cliffBias(biasStrength * 0.48f, biasSteps, noise) * biasStrength;
            }
            case Bias::terrace:
            {
                // TODO improve width and strength scaling
                return noise * (1 - biasStrength) + terraceBias(biasStrength * 0.48f, biasSteps, noise) * biasStrength;
            }
            default:
            {
                LOG_ERROR("Unknown noise bias");
                return noise;
            }
        }
    }

    static void generateMap(Settings& settings, HeightMap& heightMap, Noise& noise)
    {
        BiasData ctx = prepareBias(settings);

        float low = settings.heightmapLow;
        float high = settings.heightmapHigh - low;

        for (int32_t y = 0; y < heightMap.height; y++)
        {
            for (int32_t x = 0; x < heightMap.width; x++)
            {
                VecXY pos = { x, y };
                float noiseValue = noise.generate(pos);
                float normalisedNoiseValue = (noiseValue + 1.0f) / 2.0f;
                float biasedNoiseValue = applyBias(settings, ctx, pos, normalisedNoiseValue);

                heightMap[pos.AsTileCoordsXY()] = low + biasedNoiseValue * high;
            }
        }

        // apply smooth/erosion
        std::optional<HeightMap> maybeWaterHeights = applyHeightMapTransform(heightMap, settings);

        // set the game map to the height map
        resetSurfaces(settings);
        setMapHeight(settings, heightMap);

        // slope smooth functions operate on the game map
        applyTileSlopeSmooth(settings);

        // set the game map water lvl
        setWaterLevel(settings.waterLevel);

        if (settings.generateRivers && maybeWaterHeights.has_value())
        {
            HeightMap waterHeights = maybeWaterHeights.value();
            for (auto y = 1; y < heightMap.height - 1; y++)
            {
                for (auto x = 1; x < heightMap.width - 1; x++)
                {
                    TileCoordsXY pos{ x, y };

                    if (waterHeights[pos] <= 0)
                    {
                        continue;
                    }

                    auto surfaceElement = MapGetSurfaceElementAt(pos);
                    if (surfaceElement != nullptr )
                    {
                        const auto adjustedHeight =
                            static_cast<uint8_t>(std::round(std::clamp(waterHeights[pos], 2.0f, 254.0f) * 0.5f) * 2.0f);

                        surfaceElement->SetWaterHeight(adjustedHeight * kCoordsZStep);
                    }
                }
            }
        }
    }

    void generateSimplexMap(Settings& settings)
    {
        auto heightMap = HeightMap(settings.mapSize);

        // TODO should freq really be influenced by map width?
        float freq = settings.noiseBaseFreq / 100.0f * (1.0f / heightMap.width);

        BaseSettings baseSettings = { BaseType::Simplex, settings.seed, freq };
        FractalSettings fractalSettings = { FractalType::Fbm, settings.noiseOctaves, 2.0f, 0.65f, 0.0f };

        auto simplexFbmNoise = Noise(baseSettings, fractalSettings, std::nullopt, std::nullopt);

        generateMap(settings, heightMap, simplexFbmNoise);
    }

    void generateWarpedMap(Settings& settings)
    {
        auto heightMap = HeightMap(settings.mapSize);

        auto freq = settings.noiseBaseFreq / std::pow(2.0f, 15.0f);

        BaseSettings baseSettings = { BaseType::Simplex, settings.seed, freq };
        FractalSettings fractalSettings = { FractalType::Fbm, settings.noiseOctaves, 2.0f, 0.5f, 0.0f };
        WarpSettings warpSettings = {WarpType::Simplex, WarpFractalType::Independent, 256, settings.seed, freq / 2, 4, 2.0f, 0.5f };

        auto warpedNoise = Noise(baseSettings, fractalSettings, std::nullopt, warpSettings);

        generateMap(settings, heightMap, warpedNoise);
    }

    void generateRidgedMap(Settings& settings)
    {
        auto heightMap = HeightMap(settings.mapSize);

        auto freq = settings.noiseBaseFreq / std::pow(2.0f, 15.0f);

        BaseSettings baseSettings = { BaseType::Simplex, settings.seed, freq };
        FractalSettings fractalSettings = { FractalType::Ridge, settings.noiseOctaves, 2.0f, 0.5f, 0.0f };
        TransformSettings transformSettings = { { 64, 64 }, 0.0f };

        auto ridgedNoise = Noise(baseSettings, fractalSettings, transformSettings, std::nullopt);

        generateMap(settings, heightMap, ridgedNoise);
    }

    void generateVoronoiMap(Settings& settings)
    {
        auto heightMap = HeightMap(settings.mapSize);

        auto freq = settings.noiseBaseFreq / std::pow(2.0f, 15.0f);

        BaseSettings baseSettings = { BaseType::Voronoi, settings.seed, freq };
        FractalSettings fractalSettings = { FractalType::PingPong, settings.noiseOctaves, 2.0f, 0.75f, 0.0f };
        WarpSettings warpSettings = {WarpType::Grid, WarpFractalType::Independent, 32, settings.seed, freq / 2, 4, 2.0f, 0.5f };

        auto voronoiNoise = Noise(baseSettings, fractalSettings, std::nullopt, warpSettings);

        generateMap(settings, heightMap, voronoiNoise);
    }
} // namespace OpenRCT2::World::MapGenerator
