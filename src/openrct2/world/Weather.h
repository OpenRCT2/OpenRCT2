/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/EnumUtils.hpp"

#include <array>
#include <cstdint>

namespace OpenRCT2::Drawing
{
    enum class FilterPaletteID : int32_t;
}

namespace OpenRCT2::Weather
{
    enum class Type : uint8_t
    {
        sunny,
        partiallyCloudy,
        cloudy,
        rain,
        heavyRain,
        thunder,
        snow,
        heavySnow,
        blizzard,
        count
    };

    static constexpr auto kNumWeatherTypes = EnumValue(Type::count);
    static constexpr auto kNumClimateMonths = 8;
    static constexpr auto kWeatherDistSize = 23;

    struct Pattern
    {
        int8_t baseTemperature;
        int8_t randomBias;
        Type distribution[kWeatherDistSize];
    };

    struct TemperatureThresholds
    {
        int8_t cold;
        int8_t warm;
    };

    struct Climate
    {
        std::array<Pattern, kNumClimateMonths> patterns;
        TemperatureThresholds itemThresholds;
    };

    enum class EffectType : uint8_t
    {
        none,
        rain,
        storm,
        snow,
        blizzard,
    };

    enum class Level
    {
        none,
        light,
        heavy,
    };

    struct State
    {
        Type weatherType;
        int8_t temperature;
        EffectType weatherEffect;
        uint8_t weatherGloom;
        Level level;
    };

    extern uint16_t gLightningFlash;

    int32_t celsiusToFahrenheit(int32_t celsius);
    void reset();
    void update();
    void updateSound();
    void stopWeatherSound();
    void forceWeather(Type weather);

    bool isDry();
    bool isRaining();
    bool isTransitioningToSnow();
    bool isSnowing();
    bool isSnowingHeavily();
    bool isPrecipitating();
    bool hasWeatherEffect();
    Drawing::FilterPaletteID getWeatherGloomPaletteId(const State& state);
    uint32_t getWeatherSpriteId(Type weatherType);
} // namespace OpenRCT2::Weather
