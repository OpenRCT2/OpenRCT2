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

#include "rct2.h"

#ifndef _CLIMATE_H_
#define _CLIMATE_H_

enum {
	CLIMATE_COOL_AND_WET,
	CLIMATE_WARM,
	CLIMATE_HOT_AND_DRY,
	CLIMATE_COLD
};

typedef struct {
	sint8 base_temperature;
	sint8 distribution_size;
	sint8 distribution[24];
} rct_weather_transition;

typedef struct {
	sint8 temp_delta;
	sint8 effect_level;
	sint8 gloom_level;
	sint8 rain_level;
	uint32 sprite_id;
} rct_weather;

extern const rct_weather weather_data[6];

int climate_celcius_to_fahrenheit(int celcius);
void climate_reset(int climate);
void update_climate();

#endif
