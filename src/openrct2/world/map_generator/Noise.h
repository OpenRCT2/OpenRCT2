/*****************************************************************************
 * Copyright (c) 2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Vec.hpp"

#include <functional>
#include <random>

namespace OpenRCT2::World::MapGenerator
{
    struct Settings;

    using NoiseFn = std::function<float(VecXY)>;

    class Noise
    {
    public:
        virtual ~Noise() = default;
        virtual float Generate(VecXY pos) = 0;
    };

    class Fbm
    {
    private:
        float _frequency;
        int32_t _octaves;
        float _lacunarity;
        float _persistence;

    public:
        explicit Fbm(float frequency, const int32_t& octaves, float lacunarity, float persistence)
            : _frequency(frequency)
            , _octaves(octaves)
            , _lacunarity(lacunarity)
            , _persistence(persistence)
        {
        }
        float Generate(VecXY pos, const NoiseFn& noise);
    };

    class DomainWarp
    {
    private:
        Fbm _fbm;
        VecXY _domainSize;
        VecXY _offsetA;
        VecXY _offsetB;
        VecXY _offsetC;
        VecXY _offsetD;

    public:
        explicit DomainWarp(uint32_t seed, const Fbm& fbm, const VecXY& domainSize)
            : _fbm(fbm)
            , _domainSize(domainSize)
        {
            std::mt19937 prng(seed);
            std::uniform_real_distribution<float> uniDist(-10.0f, 10.0f);
            _offsetA = { uniDist(prng), uniDist(prng) };
            _offsetB = { uniDist(prng), uniDist(prng) };
            _offsetC = { uniDist(prng), uniDist(prng) };
            _offsetD = { uniDist(prng), uniDist(prng) };
        }

        float Generate(VecXY pos, const NoiseFn& noise);
    };

    class SimplexNoise : public Noise
    {
    private:
        uint8_t _perm[512];

    public:
        explicit SimplexNoise(uint32_t seed);
        SimplexNoise();
        float Generate(VecXY pos) override;
    };

    class RidgeNoise : public SimplexNoise
    {
    public:
        explicit RidgeNoise(uint32_t seed)
            : SimplexNoise(seed)
        {
        }
        RidgeNoise()
        {
        }
        float Generate(VecXY pos) override;
    };

    class SimplexFbmNoise final : public SimplexNoise
    {
    private:
        Fbm _fbm;

    public:
        explicit SimplexFbmNoise(uint32_t seed, float frequency, int32_t octaves, float lacunarity, float persistence)
            : SimplexNoise(seed)
            , _fbm(frequency, octaves, lacunarity, persistence)
        {
        }
        float Generate(VecXY pos) override;
    };

    class RidgeFbmNoise final : public RidgeNoise
    {
    private:
        Fbm _fbm;

    public:
        explicit RidgeFbmNoise(uint32_t seed, float frequency, int32_t octaves, float lacunarity, float persistence)
            : RidgeNoise(seed)
            , _fbm(frequency, octaves, lacunarity, persistence)
        {
        }
        float Generate(VecXY pos) override;
    };

    class SimplexWarpedNoise final : public SimplexNoise
    {
    private:
        DomainWarp _domainWarp;

    public:
        explicit SimplexWarpedNoise(
            uint32_t seed, float frequency, int32_t octaves, float lacunarity, float persistence, VecXY domainSize)
            : SimplexNoise(seed)
            , _domainWarp(seed, Fbm(frequency, octaves, lacunarity, persistence), domainSize)
        {
        }
        float Generate(VecXY pos) override;
    };
} // namespace OpenRCT2::World::MapGenerator
