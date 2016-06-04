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

#define gClimate						RCT2_GLOBAL(RCT2_ADDRESS_CLIMATE, uint8)
#define gClimateCurrentWeather			RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_WEATHER, uint8)
#define gClimateCurrentTemperature		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TEMPERATURE, sint8)
#define gClimateCurrentWeatherEffect	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_WEATHER_EFFECT, uint8)
#define gClimateCurrentWeatherGloom		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_WEATHER_GLOOM, uint8)
#define gClimateCurrentRainLevel		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_RAIN_LEVEL, uint8)
#define gClimateNextWeather				RCT2_GLOBAL(RCT2_ADDRESS_NEXT_WEATHER, uint8)
#define gClimateNextTemperature			RCT2_GLOBAL(RCT2_ADDRESS_NEXT_TEMPERATURE, sint8)
#define gClimateNextWeatherEffect		RCT2_GLOBAL(RCT2_ADDRESS_NEXT_WEATHER_EFFECT, uint8)
#define gClimateNextWeatherGloom		RCT2_GLOBAL(RCT2_ADDRESS_NEXT_WEATHER_GLOOM, uint8)
#define gClimateNextRainLevel			RCT2_GLOBAL(RCT2_ADDRESS_NEXT_RAIN_LEVEL, uint8)
#define gClimateUpdateTimer				RCT2_GLOBAL(RCT2_ADDRESS_CLIMATE_UPDATE_TIMER, uint16)

extern const rct_weather climate_weather_data[6];

extern uint16 gClimateLightningFlash;

int climate_celsius_to_fahrenheit(int celsius);
void climate_reset(int climate);
void climate_update();
void climate_update_sound();
void climate_force_weather(uint8 weather);

#endif
