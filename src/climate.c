/*****************************************************************************
 * Copyright (c) 2014 Ted John
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

#include "addresses.h"
#include "climate.h"
#include "date.h"
#include "gfx.h"
#include "rct2.h"

typedef struct {
	sint8 base_temperature;
	sint8 distribution_size;
	sint8 distribution[24];
} rct_weather_transition;

int gClimateNextWeather;

static int _climateNextTemperature;
static int _climateNextWeatherEffect;
static int _climateNextWeatherGloom;
static int _climateNextRainLevel;

static const rct_weather_transition* climate_transitions[4];

static void climate_determine_future_weather();

int climate_celcius_to_fahrenheit(int celcius)
{
	return (celcius * 29) / 16 + 32;
}

/**
 *  Set climate and determine start weather.
 *  rct2: 0x006C45ED
 */
void climate_reset(int climate)
{
	RCT2_GLOBAL(RCT2_ADDRESS_CLIMATE, sint8) = climate;
	climate_determine_future_weather();
}


/**
 * Weather & climate update iteration.
 * Gradually changes the weather parameters towards their determined next values.
 * 
 * rct2: 0x006C46B1
 */
void climate_update()
{
	uint8 screen_flags = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8);
	sint8 temperature = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TEMPERATURE, sint8),
		target_temperature = _climateNextTemperature,
		cur_gloom = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_WEATHER_GLOOM, sint8),
		next_gloom = _climateNextWeatherGloom,
		cur_rain = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_RAIN_LEVEL, sint8),
		next_rain = _climateNextRainLevel;
	

	if (screen_flags & (~SCREEN_FLAGS_PLAYING)) // only normal play mode gets climate
		return;

	if (RCT2_GLOBAL(RCT2_ADDRESS_CLIMATE_UPDATE_TIMER, sint16))	{

		if (RCT2_GLOBAL(RCT2_ADDRESS_CLIMATE_UPDATE_TIMER, sint16) == 960)
			RCT2_GLOBAL(0x009A9804, uint32) |= 8; // climate dirty flag?

		RCT2_GLOBAL(RCT2_ADDRESS_CLIMATE_UPDATE_TIMER, sint16)--;

	} else if (!(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, sint32) & 0x7F)) {
		
		if (temperature == target_temperature) {
			if (cur_gloom == next_gloom) {
				RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_WEATHER_EFFECT, sint8) = _climateNextWeatherEffect;

				if (cur_rain == next_rain) {
					RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_WEATHER, sint8) = gClimateNextWeather;
					climate_determine_future_weather();
					RCT2_GLOBAL(0x009A9804, uint32) |= 8; // climate dirty flag?
				} else {
					if (next_rain == 3) {
						cur_rain = 3;
					} else {
						sint8 next_rain_step = cur_rain + 1;
						if (cur_rain > next_rain)
							next_rain_step = cur_rain - 1;
						RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_RAIN_LEVEL, sint8) = next_rain_step;
					}
				}
			} else {
				sint8 next_gloom_step = cur_gloom + 1;
				if (cur_gloom > next_gloom)
					next_gloom_step = cur_gloom - 1;

				RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_WEATHER_GLOOM, sint8) = next_gloom_step;
				gfx_invalidate_screen();
			}

		} else {
			sint8 newtemp = temperature + 1;			
			if (temperature > target_temperature)
				newtemp = temperature - 1;

			RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TEMPERATURE, sint8) = newtemp;
			RCT2_GLOBAL(0x009A9804, uint32) |= 8; // climate dirty flag?
		}
	}
}


/**
 * Calculates future weather development.
 * RCT2 implements this as discrete probability distributions dependant on month and climate 
 * for next_weather. The other weather parameters are then looked up depending only on the
 * next weather.
 *
 * rct2: 0x006C461C
 */
static void climate_determine_future_weather()
{
	sint8 climate = RCT2_GLOBAL(RCT2_ADDRESS_CLIMATE, sint8);
	const rct_weather_transition* climate_table = climate_transitions[climate];
	sint8 month = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, sint16) & 7;
	rct_weather_transition transition = climate_table[month];
	
	// Generate a random variable with values 0 upto distribution_size-1 and chose weather from the distribution table accordingly
	sint8 next_weather = transition.distribution[ ((rand() & 0xFF) * transition.distribution_size) >> 8 ];
	gClimateNextWeather = next_weather;

	_climateNextTemperature = transition.base_temperature + climate_weather_data[next_weather].temp_delta;
	_climateNextWeatherEffect = climate_weather_data[next_weather].effect_level;
	_climateNextWeatherGloom = climate_weather_data[next_weather].gloom_level;
	_climateNextRainLevel = climate_weather_data[next_weather].rain_level;
	
	RCT2_GLOBAL(RCT2_ADDRESS_CLIMATE_UPDATE_TIMER, sint16) = 1920;
}



#pragma region Climate / Weather data tables

// rct2: 0x00993C94
// There is actually a sprite at 0x5A9C for snow but only these weather types seem to be fully implemented
const rct_weather climate_weather_data[6] = {
	{ .temp_delta = 10, .effect_level = 0, .gloom_level = 0, .rain_level = 0, .sprite_id = 0x5A96 }, // Sunny
	{ .temp_delta = 5, .effect_level = 0, .gloom_level = 0, .rain_level = 0, .sprite_id = 0x5A97 }, // Partially Cloudy
	{ .temp_delta = 0, .effect_level = 0, .gloom_level = 0, .rain_level = 0, .sprite_id = 0x5A98 }, // Cloudy
	{ .temp_delta = -2, .effect_level = 1, .gloom_level = 1, .rain_level = 1, .sprite_id = 0x5A99 }, // Rain
	{ .temp_delta = -4, .effect_level = 1, .gloom_level = 2, .rain_level = 2, .sprite_id = 0x5A9A }, // Heavy Rain
	{ .temp_delta = 2, .effect_level = 2, .gloom_level = 2, .rain_level = 2, .sprite_id = 0x5A9B }, // Thunderstorm
};


// rct2: 00993998
static const rct_weather_transition climate_cool_and_wet_transitions[] = {
	{ .base_temperature = 8, .distribution_size = 18,
	.distribution = { 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 0, 0, 0, 0, 0 } },
	{ .base_temperature = 10, .distribution_size = 21,
	.distribution = { 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 0, 0 } },
	{ .base_temperature = 14, .distribution_size = 17,
	.distribution = { 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4, 0, 0, 0, 0, 0, 0 } },
	{ .base_temperature = 17, .distribution_size = 17,
	.distribution = { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 0, 0, 0, 0, 0, 0 } },
	{ .base_temperature = 19, .distribution_size = 23,
	.distribution = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 4 } },
	{ .base_temperature = 20, .distribution_size = 23,
	.distribution = { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 4, 4, 4, 5 } },
	{ .base_temperature = 16, .distribution_size = 19,
	.distribution = { 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 4, 4, 5, 0, 0, 0, 0 } },
	{ .base_temperature = 13, .distribution_size = 16,
	.distribution = { 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 4, 5, 0, 0, 0, 0, 0, 0, 0 } }
};
static const rct_weather_transition climate_warm_transitions[] = {
	{ .base_temperature = 12, .distribution_size = 21,
	.distribution = { 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 4, 0, 0 } },
	{ .base_temperature = 13, .distribution_size = 22,
	.distribution = { 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 5, 0 } },
	{ .base_temperature = 16, .distribution_size = 17,
	.distribution = { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 0, 0, 0, 0, 0, 0 } },
	{ .base_temperature = 19, .distribution_size = 18,
	.distribution = { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 0, 0, 0, 0, 0 } },
	{ .base_temperature = 21, .distribution_size = 22,
	.distribution = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 0 } },
	{ .base_temperature = 22, .distribution_size = 17,
	.distribution = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 5, 0, 0, 0, 0, 0, 0 } },
	{ .base_temperature = 19, .distribution_size = 17,
	.distribution = { 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 0, 0, 0, 0, 0, 0 } },
	{ .base_temperature = 16, .distribution_size = 17,
	.distribution = { 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 0, 0, 0, 0, 0, 0 } }
};
static const rct_weather_transition climate_hot_and_dry_transitions[] = {
	{ .base_temperature = 12, .distribution_size = 15,
	.distribution = { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0 } },
	{ .base_temperature = 14, .distribution_size = 12,
	.distribution = { 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
	{ .base_temperature = 16, .distribution_size = 11,
	.distribution = { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
	{ .base_temperature = 19, .distribution_size = 9,
	.distribution = { 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
	{ .base_temperature = 21, .distribution_size = 13,
	.distribution = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
	{ .base_temperature = 22, .distribution_size = 11,
	.distribution = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
	{ .base_temperature = 21, .distribution_size = 12,
	.distribution = { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
	{ .base_temperature = 16, .distribution_size = 13,
	.distribution = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
};
static const rct_weather_transition climate_cold_transitions[] = {
	{ .base_temperature = 4, .distribution_size = 18,
	.distribution = { 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 3, 4, 0, 0, 0, 0, 0 } },
	{ .base_temperature = 5, .distribution_size = 21,
	.distribution = { 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 4, 5, 0, 0 } },
	{ .base_temperature = 7, .distribution_size = 17,
	.distribution = { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 4, 0, 0, 0, 0, 0, 0 } },
	{ .base_temperature = 9, .distribution_size = 17,
	.distribution = { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 0, 0, 0, 0, 0, 0 } },
	{ .base_temperature = 10, .distribution_size = 23,
	.distribution = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 4 } },
	{ .base_temperature = 11, .distribution_size = 23,
	.distribution = { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 4, 5 } },
	{ .base_temperature = 9, .distribution_size = 19,
	.distribution = { 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 4, 5, 0, 0, 0, 0 } },
	{ .base_temperature = 6, .distribution_size = 16,
	.distribution = { 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 4, 5, 0, 0, 0, 0, 0, 0, 0 } }
};

static const rct_weather_transition* climate_transitions[] = {
	climate_cool_and_wet_transitions,
	climate_warm_transitions,
	climate_hot_and_dry_transitions,
	climate_cold_transitions
};

#pragma endregion