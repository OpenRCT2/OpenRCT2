#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#ifndef _CLIMATE_H_
#define _CLIMATE_H_

#include "../common.h"
#include "../drawing/drawing.h"

enum {
	CLIMATE_COOL_AND_WET,
	CLIMATE_WARM,
	CLIMATE_HOT_AND_DRY,
	CLIMATE_COLD
};

enum{
	WEATHER_SUNNY,
	WEATHER_PARTIALLY_CLOUDY,
	WEATHER_CLOUDY,
	WEATHER_RAIN,
	WEATHER_HEAVY_RAIN,
	WEATHER_THUNDER
};

#pragma pack(push, 1)
typedef struct rct_weather {
	sint8 temp_delta;
	sint8 effect_level;
	sint8 gloom_level;
	sint8 rain_level;
	uint32 sprite_id;
} rct_weather;
assert_struct_size(rct_weather, 8);
#pragma pack(pop)

extern uint8 gClimate;
extern uint8 gClimateCurrentWeather;
extern sint8 gClimateCurrentTemperature;
extern uint8 gClimateCurrentWeatherEffect;
extern uint8 gClimateCurrentWeatherGloom;
extern uint8 gClimateCurrentRainLevel;
extern uint8 gClimateNextWeather;
extern sint8 gClimateNextTemperature;
extern uint8 gClimateNextWeatherEffect;
extern uint8 gClimateNextWeatherGloom;
extern uint8 gClimateNextRainLevel;
extern uint16 gClimateUpdateTimer;

extern uint16 gClimateLightningFlash;

extern const rct_weather climate_weather_data[6];
extern const FILTER_PALETTE_ID ClimateWeatherGloomColours[4];

int climate_celsius_to_fahrenheit(int celsius);
void climate_reset(int climate);
void climate_update();
void climate_update_sound();
void climate_force_weather(uint8 weather);

#endif
