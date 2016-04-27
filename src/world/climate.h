/*****************************************************************************
 * Copyright (c) 2014 Ted John, Matthias Lanzinger
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * This file is part of OpenRCT2.
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

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

typedef struct {
	sint8 temp_delta;
	sint8 effect_level;
	sint8 gloom_level;
	sint8 rain_level;
	uint32 sprite_id;
} rct_weather;

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
