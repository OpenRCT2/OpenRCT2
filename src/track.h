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

#ifndef _TRACK_H_
#define _TRACK_H_

#include "rct2.h"

typedef struct {
	uint8 type;
	uint8 vangle_end;
	uint8 vangle_start;
	uint8 bank_end;
	uint8 bank_start;
	uint8 special;
	uint8 pad[2];
} rct_trackdefinition;

enum {
	TRACK_NONE = 0,

	TRACK_FLAT = 0,
	TRACK_STATION_END = 2,
	TRACK_VERTICAL_LOOP = 7,
	TRACK_S_BEND = 13,
	TRACK_TWIST = 17,
	TRACK_HALF_LOOP = 18,
	TRACK_CORKSCREW = 19,
	TRACK_TOWER_BASE = 20,
	TRACK_HELIX_SMALL= 21,
	TRACK_HELIX_LARGE= 22,
	TRACK_HELIX_LARGE_UNBANKED = 23, 
	TRACK_BRAKES = 24,
	TRACK_ON_RIDE_PHOTO = 26,
	TRACK_WATER_SPLASH = 27,
	TRACK_BARREL_ROLL = 29,
	TRACK_POWERED_LIFT = 30,
	TRACK_HALF_LOOP_2 = 31, // ?
	TRACK_LOG_FLUME_REVERSER = 33,
	TRACK_WHOA_BELLY = 36,
	TRACK_LIFT_HILL = 43,
	TRACK_SPINNING_TUNNEL = 46,
	TRACK_ROTATION_CONTROL_TOGGLE = 47,
	TRACK_RAPIDS = 52,
	TRACK_WATERFALL = 152,
	TRACK_WHIRLPOOL = 152,
	TRACK_BRAKE_FOR_DROP = 172
};

enum {
	TRACK_UP_25 = 2,
	TRACK_UP_60 = 4,
	TRACK_DOWN_25 = 6,
	TRACK_DOWN_60 = 8,
	TRACK_UP_90 = 10,
	TRACK_DOWN_90 = 18,

	TRACK_VANGLE_TOWER = 10,
	TRACK_VANGLE_WHOA_BELLY = 10
};

enum {
	TRACK_BANK_NONE = 0,
	TRACK_BANK_LEFT = 2,
	TRACK_BANK_RIGHT = 4,
	TRACK_BANK_UPSIDE_DOWN = 15,
};

enum {
	TRACK_HALF_LOOP_UP = 64,
	TRACK_HALF_LOOP_DOWN = 192,
	TRACK_UNKNOWN_VERTICAL_LOOP = 208,
	TRACK_CORKSCREW_DOWN = 224
};

void track_load_list(ride_list_item item);

#endif