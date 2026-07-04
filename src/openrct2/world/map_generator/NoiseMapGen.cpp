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
#include "../Map.h"
#include "Erosion.h"
#include "MapHelpers.h"
#include "Noise.h"

#include <memory>
#include <variant>

namespace OpenRCT2::World::MapGenerator
{
    constexpr float kQ = 1.0f / Hydro::kRiversOverscanFactor;

    using BiasData = std::variant<std::unique_ptr<Noise>, VecXY>;

    static BiasData prepareBias(const Settings& settings)
    {
        BiasData ctx;
        if (settings.bias == Bias::coastal || settings.bias == Bias::river || settings.bias == Bias::valley)
        {
            std::mt19937 prng(settings.seed);
            std::uniform_real_distribution dist(-1.0f, 1.0f);
            auto direction = VecXY{ dist(prng), dist(prng) };
            ctx = direction.Normalize();
        }
        else if (settings.bias == Bias::canyon)
        {
            BaseSettings baseSettings = { BaseType::Simplex, settings.seed + 1, 1.0f / 384.0f };
            FractalSettings fractalSettings = { FractalType::Ridge, 6, 2.5f, 0.55f, 0.0f };
            ctx = std::make_unique<Noise>(baseSettings, fractalSettings, std::nullopt, std::nullopt);
        }
        else if (settings.bias == Bias::mountain)
        {
            BaseSettings baseSettings = { BaseType::Simplex, settings.seed + 1, 1.0f / 768.0f };
            FractalSettings fractalSettings = { FractalType::Ridge, 7, 2.5f, 0.5f, 0.0f };
            TransformSettings transformSettings = { { 256, 256 }, 0.0f };

            ctx = std::make_unique<Noise>(baseSettings, fractalSettings, transformSettings, std::nullopt);
        }

        return ctx;
    }

    static float cliffBias(const float height, const int32_t numCliffs, const float noise)
    {
        const float n = static_cast<float>(numCliffs);
        return (1 - n * height) * noise + height * std::floor((n + 1) * noise);
    }

    static float terraceBias(const float width, const int32_t numTerraces, const float noise)
    {
        const float n = static_cast<float>(numTerraces);
        const float u = n * noise;
        const float k = std::floor(u);
        const float t = u - k;
        const float g = (std::min((1.0f - width) / 2.0f, t) + std::max(0.0f, t - (1.0f + width) / 2.0f)) / (1.0f - width);
        return (k + g) / n;
    }

    static float applyBias(const MapGenContext& ctx, const BiasData& biasData, const VecXY& pos, const float noise)
    {
        const float biasStrength = static_cast<float>(ctx.settings.biasStrength) / 100.0f;
        const float biasSteps = ctx.settings.biasSteps;

        const float nx = 2.0f * pos.x / ctx.dimensions.x - 1.0f;
        const float ny = 2.0f * pos.y / ctx.dimensions.y - 1.0f;
        const VecXY normPos{ nx, ny };

        switch (ctx.settings.bias)
        {
            case Bias::none:
            {
                return noise;
            }
            case Bias::island:
            {
                if (-kQ <= nx && nx <= kQ && -kQ <= ny && ny <= kQ)
                {
                    float d = std::pow(1.0f - (1.0f - std::pow(nx / kQ, 2)) * (1.0f - std::pow(ny / kQ, 2)), 1.75f);
                    return Smoothstep(0.0f, 1.0f, 1.0f - d * biasStrength) * noise;
                }
                return 0.0f;
            }
            case Bias::valley:
            {
                const VecXY& r = std::get<VecXY>(biasData);
                const float xr = r.x * nx + r.y * ny;
                const float yr = -r.y * nx + r.x * ny;
                const float z = std::clamp(16.0f * std::pow(yr, 4.0f) - 8.0f * std::pow(xr, 3.0f), 0.0f, 1.0f);
                const float d = std::pow(0.5f * std::sin(M_PI * 0.5f * z) + 0.5f, 3.0f);
                return Smoothstep(0.0f, 1.0f, 1.0f - (1.0f - d) * biasStrength) * noise;
            }
            case Bias::coastal:
            {
                const VecXY& r = std::get<VecXY>(biasData);
                const float xr = r.x * nx + r.y * ny;
                const float yr = -r.y * nx + r.x * ny;
                const auto d = (pow(xr, 3.0f) + pow(yr, 3.0f)) / (4.0f * pow(kQ, 3.0f)) + (3.0f * xr + 3.0f * yr) / (4.0f * kQ)
                    + 0.5f;
                return Smoothstep(0.0f, 1.0f, 1.0f - (1.0f - d) * biasStrength) * noise;
            }
            case Bias::river:
            {
                // TODO offset center (or simplify the distance calc below)?
                VecXY c{ 0.0f, 0.0f };
                VecXY b = std::get<VecXY>(biasData);
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
                auto canyonNoise = canyonScale * std::pow(std::get<std::unique_ptr<Noise>>(biasData)->generate(pos), 3);
                return Smoothstep(0.0f, 1.0f, 1.0f - canyonNoise * biasStrength) * noise;
            }
            case Bias::mountain:
            {
                auto mountainScale = 2.0f + biasStrength * 2.0f;
                auto mountainNoise = mountainScale * std::pow(std::get<std::unique_ptr<Noise>>(biasData)->generate(pos), 2);
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

    static void generateMap(MapGenContext& ctx, const Noise& noise)
    {
        const auto& settings = ctx.settings;
        const BiasData biasData = prepareBias(settings);

        const float low = settings.heightmapLow;
        const float high = settings.heightmapHigh - low;

        for (int32_t y = 0; y < ctx.dimensions.y; y++)
        {
            for (int32_t x = 0; x < ctx.dimensions.x; x++)
            {
                const VecXY pos = { x, y };
                const float noiseValue = noise.generate(pos);
                const float normalisedNoiseValue = (noiseValue + 1.0f) / 2.0f;
                const float biasedNoiseValue = applyBias(ctx, biasData, pos, normalisedNoiseValue);

                ctx.heightMap[pos.AsTileCoordsXY()] = low + biasedNoiseValue * high;
            }
        }

        // apply smooth/erosion
        applyHeightMapTransform(ctx);

        // set the game map to the height map
        resetSurfaces(ctx);
        setMapHeight(ctx);

        // set the game map water lvl
        setWaterLevel(ctx);
        setRiverWater(ctx);

        // slope smooth functions operate on the game map
        applyTileSlopeSmooth(ctx);
    }

    void generateSimplexMap(MapGenContext& ctx)
    {
        const Settings& settings = ctx.settings;
        const float freq = settings.noiseBaseFreq / std::pow(2.0f, 15.0f);

        const BaseSettings baseSettings = { BaseType::Simplex, settings.seed, freq };
        const FractalSettings fractalSettings = { FractalType::Fbm, settings.noiseOctaves, 2.0f, 0.65f, 0.0f };

        const Noise simplexFbmNoise(baseSettings, fractalSettings, std::nullopt, std::nullopt);

        generateMap(ctx, simplexFbmNoise);
    }

    void generateWarpedMap(MapGenContext& ctx)
    {
        const Settings& settings = ctx.settings;
        const float freq = settings.noiseBaseFreq / std::pow(2.0f, 15.0f);

        const BaseSettings baseSettings = { BaseType::Simplex, settings.seed, freq };
        const FractalSettings fractalSettings = { FractalType::Fbm, settings.noiseOctaves, 2.0f, 0.5f, 0.0f };
        const WarpSettings warpSettings = {
            WarpType::Simplex, WarpFractalType::Independent, 256, settings.seed, freq / 2, 4, 2.0f, 0.5f
        };

        const Noise warpedNoise(baseSettings, fractalSettings, std::nullopt, warpSettings);

        generateMap(ctx, warpedNoise);
    }

    void generateRidgedMap(MapGenContext& ctx)
    {
        const Settings& settings = ctx.settings;
        const float freq = settings.noiseBaseFreq / std::pow(2.0f, 15.0f);

        const BaseSettings baseSettings = { BaseType::Simplex, settings.seed, freq };
        const FractalSettings fractalSettings = { FractalType::Ridge, settings.noiseOctaves, 2.0f, 0.5f, 0.0f };
        const TransformSettings transformSettings = { { 64, 64 }, 0.0f };

        const Noise ridgedNoise(baseSettings, fractalSettings, transformSettings, std::nullopt);

        generateMap(ctx, ridgedNoise);
    }

    void generateVoronoiMap(MapGenContext& ctx)
    {
        const Settings& settings = ctx.settings;
        const float freq = settings.noiseBaseFreq / std::pow(2.0f, 15.0f);

        const BaseSettings baseSettings = { BaseType::Voronoi, settings.seed, freq, VoronoiValueType::Distance };
        const FractalSettings fractalSettings = { FractalType::PingPong, settings.noiseOctaves, 2.0f, 0.75f, 0.0f };
        const WarpSettings warpSettings = {
            WarpType::Grid, WarpFractalType::Independent, 32, settings.seed, freq / 2, 4, 2.0f, 0.5f
        };

        const Noise voronoiNoise(baseSettings, fractalSettings, std::nullopt, warpSettings);

        generateMap(ctx, voronoiNoise);
    }
} // namespace OpenRCT2::World::MapGenerator
