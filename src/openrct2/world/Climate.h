/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../drawing/Drawing.h"

enum CLIMATE
{
    CLIMATE_COOL_AND_WET,
    CLIMATE_WARM,
    CLIMATE_HOT_AND_DRY,
    CLIMATE_COLD,
    CLIMATE_COUNT,
};

enum WEATHER
{
    WEATHER_SUNNY,
    WEATHER_PARTIALLY_CLOUDY,
    WEATHER_CLOUDY,
    WEATHER_RAIN,
    WEATHER_HEAVY_RAIN,
    WEATHER_THUNDER,
    WEATHER_SNOW,
    WEATHER_HEAVY_SNOW,
    WEATHER_BLIZZARD,
    WEATHER_COUNT,
};

enum WEATHER_EFFECT
{
    WEATHER_EFFECT_NONE,
    WEATHER_EFFECT_RAIN,
    WEATHER_EFFECT_STORM,
    WEATHER_EFFECT_SNOW,
    WEATHER_EFFECT_BLIZZARD,
};

enum WEATHER_LEVEL
{
    WEATHER_LEVEL_NONE,
    WEATHER_LEVEL_LIGHT,
    WEATHER_LEVEL_HEAVY,
};

struct WeatherState
{
    int8_t TemperatureDelta;
    int8_t EffectLevel;
    int8_t GloomLevel;
    int8_t WeatherLevel;
    uint32_t SpriteId;
};

struct ClimateState
{
    uint8_t Weather;
    int8_t Temperature;
    uint8_t WeatherEffect;
    uint8_t WeatherGloom;
    uint8_t WeatherLevel;
};

extern uint8_t gClimate;
extern ClimateState gClimateCurrent;
extern ClimateState gClimateNext;
extern uint16_t gClimateUpdateTimer;
extern uint16_t gClimateLightningFlash;

int32_t climate_celsius_to_fahrenheit(int32_t celsius);
void climate_reset(int32_t climate);
void climate_update();
void climate_update_sound();
void climate_force_weather(uint8_t weather);

bool climate_is_raining();
bool climate_is_snowing();
FILTER_PALETTE_ID climate_get_weather_gloom_palette_id(const ClimateState& state);
uint32_t climate_get_weather_sprite_id(const ClimateState& state);
