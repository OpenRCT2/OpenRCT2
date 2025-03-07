/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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

// TODO: refactor usage and remove
enum class ClimateType : uint8_t
{
    CoolAndWet,
    Warm,
    HotAndDry,
    Cold,
    Count
};

enum class WeatherType : uint8_t
{
    Sunny,
    PartiallyCloudy,
    Cloudy,
    Rain,
    HeavyRain,
    Thunder,
    Snow,
    HeavySnow,
    Blizzard,
    Count
};

static constexpr auto kNumWeatherTypes = EnumValue(WeatherType::Count);
static constexpr auto kNumClimateMonths = 8;
static constexpr auto kWeatherDistSize = 23;

struct WeatherPattern
{
    int8_t baseTemperature;
    int8_t randomBias;
    WeatherType distribution[kWeatherDistSize];
};

struct TemperatureThresholds
{
    int8_t cold;
    int8_t warm;
};

struct Climate
{
    std::array<WeatherPattern, kNumClimateMonths> patterns;
    TemperatureThresholds itemThresholds;
};

enum class WeatherEffectType : uint8_t
{
    None,
    Rain,
    Storm,
    Snow,
    Blizzard,
};

enum class WeatherLevel
{
    None,
    Light,
    Heavy,
};

struct WeatherState
{
    WeatherType weatherType;
    int8_t temperature;
    WeatherEffectType weatherEffect;
    uint8_t weatherGloom;
    WeatherLevel level;
};

extern uint16_t gClimateLightningFlash;

int32_t ClimateCelsiusToFahrenheit(int32_t celsius);
void ClimateReset();
void ClimateUpdate();
void ClimateUpdateSound();
void ClimateStopWeatherSound();
void ClimateForceWeather(WeatherType weather);

enum class FilterPaletteID : int32_t;

bool ClimateIsRaining();
bool ClimateTransitioningToSnow();
bool ClimateIsSnowing();
bool ClimateIsSnowingHeavily();
bool WeatherIsDry(WeatherType);
FilterPaletteID ClimateGetWeatherGloomPaletteId(const WeatherState& state);
uint32_t ClimateGetWeatherSpriteId(const WeatherType weatherType);
