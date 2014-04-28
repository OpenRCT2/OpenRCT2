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

const sint8 weather_table[48] = {
	10, 0, 0, 0, 0x96, 0x5a, 0, 0, // Sunny
	5, 0, 0, 0, 0x97, 0x5A, 0, 0, // Partially cloudy
	0, 0, 0, 0, 0x98, 0x5A, 0, 0, // Cloudy
	-2, 1, 1, 1, 0x99, 0x5A, 0, 0, // Rain
	-4, 1, 2, 2, 0x9A, 0x5A, 0, 0, // Heavy rain
	2, 2, 2, 2, 0x9B, 0x5A, 0, 0  // Thunderstorm
};


void determine_future_weather();

int climate_celcius_to_fahrenheit(int celcius)
{
	return (celcius * 29) / 16 + 32;
}

/**
 *
 *  rct2: 0x006C45ED
 */
void climate_reset(int climate)
{
	int eax, ebx, ecx, edx, esi, edi, ebp;

	RCT2_GLOBAL(RCT2_ADDRESS_CLIMATE, sint8) = climate;

	eax = 1;
	RCT2_CALLFUNC_X(0x006C4672, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_WEATHER, sint8) = eax & 0xFF;
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TEMPERATURE, sint8) = ebx & 0xFF;

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_WEATHER_EFFECT, sint8) = (ebx >> 8) & 0xFF;
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_WEATHER_GLOOM, sint8) = ecx & 0xFF;
	RCT2_GLOBAL(0x013CA752, sint8) = (ecx >> 8) & 0xFF;
	RCT2_CALLPROC_X(0x6C461C, 0, 0, 0, 0, 0, 0, 0);
}


/**
 * Weather & climate update iteration.
 * rct2: 0x006C46B1
 **/
void update_climate()
{
	uint8 screen_flags = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8);
	sint8 temperature = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TEMPERATURE, sint8);
	sint8 target_temperature = RCT2_GLOBAL(RCT2_ADDRESS_NEXT_TEMPERATURE, sint8);
	sint8 cur_gloom = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_WEATHER_GLOOM, sint8);
	sint8 next_gloom = RCT2_GLOBAL(RCT2_ADDRESS_NEXT_WEATHER_GLOOM, sint8);

	if (screen_flags & (~SCREEN_FLAGS_PLAYING)) // only normal play mode gets climate
		return;
	// 0x013CA752 and 0x013CA753 are possibly for rain particles. Can't be sure as rain rendering seems broken atm for me.

	if (RCT2_GLOBAL(RCT2_ADDRESS_CLIMATE_UPDATE_TIMER, sint16))	{

		if (RCT2_GLOBAL(RCT2_ADDRESS_CLIMATE_UPDATE_TIMER, sint16) == 960)
			RCT2_GLOBAL(0x009A9804, uint32) |= 8; // climate dirty flag?

		RCT2_GLOBAL(RCT2_ADDRESS_CLIMATE_UPDATE_TIMER, sint16)--;

	} else if (!(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, sint32) & 0x7F)) {
		
		if (temperature == target_temperature) {
			if (cur_gloom == next_gloom) {
				RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_WEATHER_EFFECT, sint8) = RCT2_GLOBAL(RCT2_ADDRESS_NEXT_WEATHER_EFFECT, sint8);

				if (RCT2_GLOBAL(0x013CA752, sint8) == RCT2_GLOBAL(0x013CA753, sint8)) {
					RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_WEATHER, sint8) = RCT2_GLOBAL(RCT2_ADDRESS_NEXT_WEATHER, sint8);
					//RCT2_CALLPROC(0x006C461C); // determine_future_weather()
					determine_future_weather();
					RCT2_GLOBAL(0x009A9804, uint32) |= 8; // climate dirty flag?
				}
				else {
					if (RCT2_GLOBAL(0x013CA753, sint8) == 3)
						RCT2_GLOBAL(0x013CA752, sint8) = 3;
					else {
						sint8 next = RCT2_GLOBAL(0x013CA752, sint8) + 1;

						if (RCT2_GLOBAL(0x013CA752, sint8) > RCT2_GLOBAL(0x013CA753, sint8))
							next = RCT2_GLOBAL(0x013CA752, sint8) - 1;
						RCT2_GLOBAL(0x013CA752, sint8) = next;
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
**/
void determine_future_weather()
{
	sint8 climate = RCT2_GLOBAL(RCT2_ADDRESS_CLIMATE, sint8);
	rct_weather_table** climate_data = ((rct_weather_table***)0x00993998)[climate];
	sint8 month = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, sint16) & 7;
	// don't do this at home. Temporary measure until we rewrite the tables.
	rct_weather_table* month_table = climate_data[month]; 
	
	// generate a random variable with values 0 upto distribution_size-1 and chose weather from the distribution table accordingly
	sint8 next_weather = month_table->distribution[ ((rand() & 0xFF) * month_table->distribution_size) >> 8 ];
	RCT2_GLOBAL(RCT2_ADDRESS_NEXT_WEATHER, sint8) = next_weather;

	//sint8* weather_table = (sint8*)0x00993C94;
	RCT2_GLOBAL(RCT2_ADDRESS_NEXT_TEMPERATURE, sint8) = month_table->base_temperature + weather_table[next_weather * 8];
	RCT2_GLOBAL(RCT2_ADDRESS_NEXT_WEATHER_EFFECT, sint8) = weather_table[next_weather * 8 + 1];
	RCT2_GLOBAL(RCT2_ADDRESS_NEXT_WEATHER_GLOOM, sint8) = weather_table[next_weather * 8 + 2];
	RCT2_GLOBAL(0x013CA753, sint8) = weather_table[next_weather * 8 + 3];
	
	RCT2_GLOBAL(RCT2_ADDRESS_CLIMATE_UPDATE_TIMER, sint16) = 1920;
}