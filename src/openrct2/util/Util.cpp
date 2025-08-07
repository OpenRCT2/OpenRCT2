/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Util.h"

#include <algorithm>
#include <random>

uint32_t UtilRand()
{
    thread_local std::mt19937 _prng(std::random_device{}());
    return _prng();
}

uint8_t UtilRandUniformDistributedByte()
{
    thread_local std::mt19937 _prng{ std::random_device{}() };
    // char types are not permitted as type params of uniform_int_distribution...
    thread_local std::uniform_int_distribution<uint16_t> _distributor{ 0, 255 };
    return static_cast<uint8_t>(_distributor(_prng));
}

uint8_t Lerp(uint8_t a, uint8_t b, float t)
{
    if (t <= 0)
        return a;
    if (t >= 1)
        return b;

    int32_t range = b - a;
    int32_t amount = static_cast<int32_t>(range * t);
    return static_cast<uint8_t>(a + amount);
}

float FLerp(float a, float b, float f)
{
    return (a * (1.0f - f)) + (b * f);
}

float Smoothstep(float edge0, float edge1, float x)
{
    x = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return x * x * (3.0f - 2.0f * x);
}

uint8_t SoftLight(uint8_t a, uint8_t b)
{
    float fa = a / 255.0f;
    float fb = b / 255.0f;
    float fr;
    if (fb < 0.5f)
    {
        fr = (2 * fa * fb) + ((fa * fa) * (1 - (2 * fb)));
    }
    else
    {
        fr = (2 * fa * (1 - fb)) + (std::sqrt(fa) * ((2 * fb) - 1));
    }
    return static_cast<uint8_t>(std::clamp(fr, 0.0f, 1.0f) * 255.0f);
}
