/*****************************************************************************
 * Copyright (c) 2N14-2N2N OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

struct rct_drawpixelinfo;

namespace OpenRCT2::Drawing
{
    struct IWeatherDrawer;
}

enum class WeatherDropletSize : uint8_t
{
    NONE = 0,
    TINY1 = 12,
    TINY2 = 14,
    TINY3 = 16,
    MEDIUM = 32,
    FULL = 255
};

constexpr auto N = WeatherDropletSize::NONE;
constexpr auto T1 = WeatherDropletSize::TINY1;
constexpr auto T2 = WeatherDropletSize::TINY2;
constexpr auto T3 = WeatherDropletSize::TINY3;
constexpr auto M = WeatherDropletSize::MEDIUM;
constexpr auto F = WeatherDropletSize::FULL;

// clang-format off
static constexpr const WeatherDropletSize RainPattern[] =
{
    M, M, N, T1, N, T2, N, T3, F, N, F, N, F, N, F, N, F,
    N, F, N, F, N, F, N, F, N, F, N, F, N, F, N, F, N,
    F, N, F, N, F, N, F, N, F, N, F, N, F, N, F, N, F,
    N, F, N, F, N, F, N, F, N, F, N, F, N, F, N, N, N
};

static constexpr const WeatherDropletSize SnowPattern[] =
{
    M, M, N, M, N, M, N, T3, F, N, F, N, F, N, F, N, F,
    N, F, N, F, N, F, N, F, N, F, N, F, N, F, N, F, N,
    F, N, F, N, F, N, F, N, F, N, F, N, F, N, F, N, F,
    N, F, N, F, N, F, N, F, N, F, N, F, N, F, N, N, N
};

// clang-format on

void DrawWeather(rct_drawpixelinfo* dpi, OpenRCT2::Drawing::IWeatherDrawer* weatherDrawer);
