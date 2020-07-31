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

enum class ClimateType : uint8_t
{
    CoolAndWet,
    Warm,
    HotAndDry,
    Cold,
    Count
};

enum WEATHER
{
    WEATHER_SUNNY,
    WEATHER_PARTIALLY_CLOUDY,
    WEATHER_CLOUDY,
    WEATHER_RAIN,
    WEATHER_HEAVY_RAIN,
    WEATHER_THUNDER,
};

enum class WeatherEffectType : uint8_t
{
    None,
    Rain,
    Storm,
};

enum class RainLevel
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
    RainLevel Level;
    uint32_t SpriteId;
};

struct ClimateState
{
    uint8_t Weather;
    int8_t Temperature;
    WeatherEffectType WeatherEffect;
    uint8_t WeatherGloom;
    RainLevel Level;
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
void climate_force_weather(uint8_t weather);

bool climate_is_raining();
FILTER_PALETTE_ID climate_get_weather_gloom_palette_id(const ClimateState& state);
uint32_t climate_get_weather_sprite_id(const ClimateState& state);
