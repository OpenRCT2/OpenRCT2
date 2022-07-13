/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../drawing/Drawing.h"
#include "../util/Util.h"

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

struct WeatherState
{
    int8_t TemperatureDelta;
    WeatherEffectType EffectLevel;
    int8_t GloomLevel;
    WeatherLevel Level;
    uint32_t SpriteId;
};

struct ClimateState
{
    WeatherType Weather;
    int8_t Temperature;
    WeatherEffectType WeatherEffect;
    uint8_t WeatherGloom;
    WeatherLevel Level;
};

extern ClimateType gClimate;
extern ClimateState gClimateCurrent;
extern ClimateState gClimateNext;
extern uint16_t gClimateUpdateTimer;
extern uint16_t gClimateLightningFlash;

int32_t climate_celsius_to_fahrenheit(int32_t celsius);
void climate_reset(ClimateType climate);
void climate_update();
void climate_update_sound();
void ClimateStopWeatherSound();
void climate_force_weather(WeatherType weather);

bool climate_is_raining();
bool climate_is_snowing();
bool WeatherIsDry(WeatherType);
FilterPaletteID climate_get_weather_gloom_palette_id(const ClimateState& state);
uint32_t climate_get_weather_sprite_id(const ClimateState& state);
