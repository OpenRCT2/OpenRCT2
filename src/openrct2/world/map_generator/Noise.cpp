/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Noise.h"

namespace OpenRCT2::World::MapGenerator
{
    Noise::Noise(
        const BaseSettings& baseSettings,
        const std::optional<FractalSettings>& fractalSettings,
        const std::optional<TransformSettings>& transformSettings,
        const std::optional<WarpSettings>& warpSettings)
    {
        // base settings
        _noise = FastNoiseLite();
        _noise.SetSeed(baseSettings.seed);
        _noise.SetFrequency(baseSettings.frequency);
        switch (baseSettings.type)
        {
            case BaseType::Simplex:
                _noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
                break;
            case BaseType::Voronoi:
                _noise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
                break;
        }

        // fractal settings
        if (fractalSettings.has_value())
        {
            const FractalSettings& fractalSettingsValue = fractalSettings.value();
            _noise.SetFractalOctaves(fractalSettingsValue.octaves);
            _noise.SetFractalLacunarity(fractalSettingsValue.lacunarity);
            _noise.SetFractalGain(fractalSettingsValue.persistence);
            _noise.SetFractalWeightedStrength(fractalSettingsValue.weight);

            switch (fractalSettingsValue.type)
            {
                case FractalType::Fbm:
                    _noise.SetFractalType(FastNoiseLite::FractalType_FBm);
                    break;
                case FractalType::Ridge:
                    _noise.SetFractalType(FastNoiseLite::FractalType_Ridged);
                    break;
                case FractalType::PingPong:
                    _noise.SetFractalType(FastNoiseLite::FractalType_PingPong);
                    break;
            }
        }

        // warp settings
        if (warpSettings.has_value())
        {
            const WarpSettings& warpSettingsValue = warpSettings.value();

            FastNoiseLite warp = FastNoiseLite();
            warp.SetSeed(warpSettingsValue.seed);
            warp.SetFrequency(warpSettingsValue.frequency);
            warp.SetDomainWarpAmp(warpSettingsValue.amplitude);
            warp.SetDomainWarpType(FastNoiseLite::DomainWarpType_OpenSimplex2);
            warp.SetFractalOctaves(warpSettingsValue.octaves);
            warp.SetFractalLacunarity(warpSettingsValue.lacunarity);
            warp.SetFractalGain(warpSettingsValue.persistence);

            switch (warpSettingsValue.type)
            {
                case WarpFractalType::Independent:
                    warp.SetFractalType(FastNoiseLite::FractalType_DomainWarpIndependent);
                    break;
                case WarpFractalType::Progressive:
                    warp.SetFractalType(FastNoiseLite::FractalType_DomainWarpProgressive);
                    break;
            }

            _warp = std::make_optional<>(warp);
        }
        else
        {
            _warp = std::nullopt;
        }

        // transform settings
        if (transformSettings.has_value())
        {
            const TransformSettings& transformSettingsValue = transformSettings.value();

            _transform = std::make_optional<Transform>({
                .offset = transformSettingsValue.offset,
                .sin = std::sin(transformSettingsValue.rotateDeg),
                .cos = std::cos(transformSettingsValue.rotateDeg)});

        }
        else
        {
            _transform = std::nullopt;
        }
    }


    float Noise::generate(const VecXY& pos) const
    {
        VecXY coords = pos;

        if (_transform.has_value())
        {
            const Transform& transform = _transform.value();
            coords = {
                coords.x * transform.cos - coords.y * transform.sin,
                coords.x * transform.sin + coords.y * transform.cos
            };
            coords = coords + transform.offset;
        }

        if (_warp.has_value())
        {
            _warp.value().DomainWarp(coords.x, coords.y);
        }

        return _noise.GetNoise(coords.x, coords.y);
    }
} // namespace OpenRCT2::World::MapGenerator
