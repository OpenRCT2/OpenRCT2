#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

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
