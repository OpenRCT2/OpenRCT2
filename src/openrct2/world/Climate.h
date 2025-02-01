/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

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

struct ClimateState
{
    WeatherType Weather;
    int8_t Temperature;
    WeatherEffectType WeatherEffect;
    uint8_t WeatherGloom;
    WeatherLevel Level;
};

extern uint16_t gClimateLightningFlash;

int32_t ClimateCelsiusToFahrenheit(int32_t celsius);
void ClimateReset(ClimateType climate);
void ClimateUpdate();
void ClimateUpdateSound();
void ClimateStopWeatherSound();
void ClimateForceWeather(WeatherType weather);

enum class FilterPaletteID : int32_t;

bool ClimateIsRaining();
bool ClimateIsSnowing();
bool ClimateIsSnowingHeavily();
bool WeatherIsDry(WeatherType);
FilterPaletteID ClimateGetWeatherGloomPaletteId(const ClimateState& state);
uint32_t ClimateGetWeatherSpriteId(const ClimateState& state);
