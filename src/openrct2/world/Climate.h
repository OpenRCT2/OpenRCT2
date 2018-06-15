/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
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
    CLIMATE_COLD
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

enum WEATHER_EFFECT
{
    WEATHER_EFFECT_NONE,
    WEATHER_EFFECT_RAIN,
    WEATHER_EFFECT_STORM,
};

enum RAIN_LEVEL
{
    RAIN_LEVEL_NONE,
    RAIN_LEVEL_LIGHT,
    RAIN_LEVEL_HEAVY,
};

struct WeatherState
{
    sint8   TemperatureDelta;
    sint8   EffectLevel;
    sint8   GloomLevel;
    sint8   RainLevel;
    uint32  SpriteId;
};

struct ClimateState
{
    uint8 Weather;
    sint8 Temperature;
    uint8 WeatherEffect;
    uint8 WeatherGloom;
    uint8 RainLevel;
};

extern uint8        gClimate;
extern ClimateState gClimateCurrent;
extern ClimateState gClimateNext;
extern uint16       gClimateUpdateTimer;
extern uint16       gClimateLightningFlash;

sint32 climate_celsius_to_fahrenheit(sint32 celsius);
void climate_reset(sint32 climate);
void climate_update();
void climate_update_sound();
void climate_force_weather(uint8 weather);

bool climate_is_raining();
FILTER_PALETTE_ID climate_get_weather_gloom_palette_id(const ClimateState &state);
uint32 climate_get_weather_sprite_id(const ClimateState &state);
