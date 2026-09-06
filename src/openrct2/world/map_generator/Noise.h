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

#include <fastnoiselite.hpp>
#include <optional>

namespace OpenRCT2::World::MapGenerator
{
    struct Settings;

    enum class BaseType
    {
        Simplex,
        Voronoi,
        // Perlin?
        // Value?
    };

    enum class VoronoiValueType
    {
        Distance,
        Cell
    };

    struct BaseSettings
    {
        BaseType type;
        uint32_t seed;
        float frequency;
        VoronoiValueType voronoiValueType = VoronoiValueType::Distance;
    };

    enum class FractalType
    {
        Fbm,
        Ridge,
        PingPong
    };

    struct FractalSettings
    {
        FractalType type;
        int32_t octaves;
        float lacunarity;
        float persistence;
        float weight;
    };

    struct TransformSettings
    {
        VecXY offset;
        float rotateDeg;
    };

    enum class WarpType
    {
        Simplex,
        Grid
    };

    enum class WarpFractalType
    {
        // None?,
        Independent,
        Progressive
    };

    struct WarpSettings
    {
        WarpType type;
        WarpFractalType fractalType;
        float amplitude;
        uint32_t seed;
        float frequency;
        int32_t octaves;
        float lacunarity;
        float persistence;
    };

    struct Transform
    {
        VecXY offset;
        float sin;
        float cos;
    };

    class Noise
    {
    private:
        FastNoiseLite _noise;
        std::optional<FastNoiseLite> _warp;
        std::optional<Transform> _transform;

    public:
        Noise(
            const BaseSettings& baseSettings, const std::optional<FractalSettings>& fractalSettings,
            const std::optional<TransformSettings>& transformSettings, const std::optional<WarpSettings>& warpSettings);
        float generate(const VecXY& pos) const;
    };
} // namespace OpenRCT2::World::MapGenerator
