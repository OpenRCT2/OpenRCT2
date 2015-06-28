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

#include "track.h"
#include "track_data.h"

const rct_track_coordinates TrackCoordinates[256] = {
		{ 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 16, 0, 0 },
		{ 0, 0, 0, 64, 0, 0 },
		{ 0, 0, 0, 8, 0, 0 },
		{ 0, 0, 0, 32, 0, 0 },
		{ 0, 0, 0, 32, 0, 0 },
		{ 0, 0, 0, 8, 0, 0 },
		{ 0, 0, 16, 0, 0, 0 },
		{ 0, 0, 64, 0, 0, 0 },
		{ 0, 0, 8, 0, 0, 0 },
		{ 0, 0, 32, 0, 0, 0 },
		{ 0, 0, 32, 0, 0, 0 },
		{ 0, 0, 8, 0, 0, 0 },
		{ 0, 3, 0, 0, -64, -64 },
		{ 0, 1, 0, 0, -64, 64 },
		{ 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0 },
		{ 0, 3, 0, 0, -64, -64 },
		{ 0, 1, 0, 0, -64, 64 },
		{ 0, 0, 0, 8, 0, 0 },
		{ 0, 0, 0, 8, 0, 0 },
		{ 0, 0, 0, 8, 0, 0 },
		{ 0, 0, 0, 8, 0, 0 },
		{ 0, 0, 8, 0, 0, 0 },
		{ 0, 0, 8, 0, 0, 0 },
		{ 0, 0, 8, 0, 0, 0 },
		{ 0, 0, 8, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0 },
		{ 0, 3, 0, 64, -64, -64 },
		{ 0, 1, 0, 64, -64, 64 },
		{ 0, 3, 64, 0, -64, -64 },
		{ 0, 1, 64, 0, -64, 64 },
		{ 0, 0, 0, 0, -64, -32 },
		{ 0, 0, 0, 0, -64, 32 },
		{ 0, 0, 0, 0, -32, -32 },
		{ 0, 0, 0, 0, -32, 32 },
		{ 0, 3, 0, 0, -32, -32 },
		{ 0, 1, 0, 0, -32, 32 },
		{ 0, 3, 0, 0, -32, -32 },
		{ 0, 1, 0, 0, -32, 32 },
		{ 0, 3, 0, 32, -32, -32 },
		{ 0, 1, 0, 32, -32, 32 },
		{ 0, 3, 32, 0, -32, -32 },
		{ 0, 1, 32, 0, -32, 32 },
		{ 0, 3, 0, 0, 0, 0 },
		{ 0, 1, 0, 0, 0, 0 },
		{ 0, 0, 0, 16, -64, 0 },
		{ 0, 0, 0, 16, -64, 0 },
		{ 0, 0, 0, -16, -64, 0 },
		{ 0, 0, 0, -16, -64, 0 },
		{ 0, 2, 0, 152, -32, 0 },
		{ 0, 2, 0, -152, 32, 0 },
		{ 0, 3, 0, 80, -32, -32 },
		{ 0, 1, 0, 80, -32, 32 },
		{ 0, 3, 0, -80, -32, -32 },
		{ 0, 1, 0, -80, -32, 32 },
		{ 0, 0, 0, 24, 0, 0 },
		{ 0, 0, 0, 24, 0, 0 },
		{ 0, 0, 24, 0, 0, 0 },
		{ 0, 0, 24, 0, 0, 0 },
		{ 0, 0, 0, 96, 32, 0 },
		{ 0, 0, 0, 32, 32, 0 },
		{ 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 16, 0, 0 },
		{ 0, 0, 0, 64, 0, 0 },
		{ 0, 0, 0, 8, 0, 0 },
		{ 0, 0, 0, 32, 0, 0 },
		{ 0, 0, 0, 32, 0, 0 },
		{ 0, 0, 0, 8, 0, 0 },
		{ 0, 0, 16, 0, 0, 0 },
		{ 0, 0, 64, 0, 0, 0 },
		{ 0, 0, 8, 0, 0, 0 },
		{ 0, 0, 32, 0, 0, 0 },
		{ 0, 0, 32, 0, 0, 0 },
		{ 0, 0, 8, 0, 0, 0 },
		{ 0, 3, 0, 0, -64, -64 },
		{ 0, 1, 0, 0, -64, 64 },
		{ 0, 0, 0, 0, -64, -32 },
		{ 0, 0, 0, 0, -64, 32 },
		{ 0, 3, 0, 0, -32, -32 },
		{ 0, 1, 0, 0, -32, 32 },
		{ 0, 2, 0, 16, 0, -96 },
		{ 0, 2, 0, 16, 0, 96 },
		{ 0, 2, 16, 0, 0, -96 },
		{ 0, 2, 16, 0, 0, 96 },
		{ 0, 2, 0, 16, 0, -160 },
		{ 0, 2, 0, 16, 0, 160 },
		{ 0, 2, 16, 0, 0, -160 },
		{ 0, 2, 16, 0, 0, 160 },
		{ 0, 3, 0, 64, 0, 0 },
		{ 0, 1, 0, 64, 0, 0 },
		{ 0, 3, 64, 0, 0, 0 },
		{ 0, 1, 64, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0 },
		{ 0, 3, 0, 16, -64, -64 },
		{ 0, 1, 0, 16, -64, 64 },
		{ 0, 3, 16, 0, -64, -64 },
		{ 0, 1, 16, 0, -64, 64 },
		{ 0, 3, 0, 16, -64, -64 },
		{ 0, 1, 0, 16, -64, 64 },
		{ 0, 3, 16, 0, -64, -64 },
		{ 0, 1, 16, 0, -64, 64 },
		{ 0, 0, 0, 16, 0, 0 },
		{ 0, 0, 0, 16, 0, 0 },
		{ 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 16, 0, 0, 0 },
		{ 0, 0, 16, 0, 0, 0 },
		{ 0, 0, 16, 16, -128, 0 },
		{ 0, 0, 0, 88, -96, 0 },
		{ 0, 0, 0, 88, -96, 0 },
		{ 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 88, 0, -96, 0 },
		{ 0, 0, 88, 0, -96, 0 },
		{ 0, 0, 0, -96, -96, 0 },
		{ 0, 0, 0, 240, -160, 0 },
		{ 0, 0, 0, 80, 32, 0 },
		{ 0, 0, 0, 32, 32, 0 },
		{ 0, 0, 32, 0, 32, 0 },
		{ 0, 0, 0, 56, 32, 0 },
		{ 0, 0, 56, 0, 0, 0 },
		{ 0, 0, 0, 56, 0, 0 },
		{ 0, 0, 56, 0, 32, 0 },
		{ 0, 0, 24, 0, 0, 0 },
		{ 0, 7, 0, 0, -64, -32 },
		{ 0, 4, 0, 0, -64, 32 },
		{ 4, 0, 0, 0, -64, 32 },
		{ 4, 1, 0, 0, -32, 64 },
		{ 0, 7, 0, 0, -64, -32 },
		{ 0, 4, 0, 0, -64, 32 },
		{ 4, 0, 0, 0, -64, 32 },
		{ 4, 1, 0, 0, -32, 64 },
		{ 4, 4, 0, 0, -32, 32 },
		{ 4, 4, 0, 16, -32, 32 },
		{ 4, 4, 0, 64, -32, 32 },
		{ 4, 4, 0, 8, -32, 32 },
		{ 4, 4, 0, 32, -32, 32 },
		{ 4, 4, 0, 32, -32, 32 },
		{ 4, 4, 0, 8, -32, 32 },
		{ 4, 4, 16, 0, -32, 32 },
		{ 4, 4, 64, 0, -32, 32 },
		{ 4, 4, 8, 0, -32, 32 },
		{ 4, 4, 32, 0, -32, 32 },
		{ 4, 4, 32, 0, -32, 32 },
		{ 4, 4, 8, 0, -32, 32 },
		{ 4, 4, 0, 24, -32, 32 },
		{ 4, 4, 0, 24, -32, 32 },
		{ 4, 4, 24, 0, -32, 32 },
		{ 4, 4, 24, 0, -32, 32 },
		{ 4, 4, 0, 0, -32, 32 },
		{ 4, 4, 0, 0, -32, 32 },
		{ 4, 4, 0, 0, -32, 32 },
		{ 4, 4, 0, 0, -32, 32 },
		{ 4, 4, 0, 8, -32, 32 },
		{ 4, 4, 0, 8, -32, 32 },
		{ 4, 4, 0, 8, -32, 32 },
		{ 4, 4, 0, 8, -32, 32 },
		{ 4, 4, 8, 0, -32, 32 },
		{ 4, 4, 8, 0, -32, 32 },
		{ 4, 4, 8, 0, -32, 32 },
		{ 4, 4, 8, 0, -32, 32 },
		{ 4, 4, 0, 0, -32, 32 },
		{ 4, 4, 0, 0, -32, 32 },
		{ 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 32, -64, 0 },
		{ 0, 0, 0, 32, -64, 0 },
		{ 0, 0, 0, -32, -64, 0 },
		{ 0, 0, 0, -32, -64, 0 },
		{ 0, 3, 0, 24, -32, -32 },
		{ 0, 1, 0, 24, -32, 32 },
		{ 0, 3, 24, 0, -32, -32 },
		{ 0, 1, 24, 0, -32, 32 },
		{ 0, 0, 0, 16, 0, 0 },
		{ 0, 2, 0, 280, -64, -32 },
		{ 0, 2, 0, 280, -64, 32 },
		{ 0, 2, 0, -280, 64, -32 },
		{ 0, 2, 0, -280, 64, 32 },
		{ 0, 0, 0, -16, -64, 0 },
		{ 0, 0, 0, -16, -64, 0 },
		{ 0, 0, 0, 16, -64, 0 },
		{ 0, 0, 0, 16, -64, 0 },
		{ 0, 2, 0, 120, -32, 0 },
		{ 0, 2, 0, -120, 32, 0 },
		{ 0, 3, 0, 48, -32, -32 },
		{ 0, 1, 0, 48, -32, 32 },
		{ 0, 3, 0, -48, -32, -32 },
		{ 0, 1, 0, -48, -32, 32 },
		{ 0, 2, 0, 32, 0, 0 },
		{ 0, 2, 0, -32, 0, 0 },
		{ 0, 0, 0, 0, -160, 0 },
		{ 0, 0, 0, 0, -160, 0 },
		{ 0, 0, 0, 0, -32, 0 },
		{ 0, 0, 0, 0, -32, 0 },
		{ 0, 0, 0, 0, -32, 0 },
		{ 0, 1, 0, 0, -32, 32 },
		{ 0, 3, 0, 0, -32, -32 },
		{ 0, 2, 0, -96, -96, 0 },
		{ 0, 2, 0, 128, 64, 0 },
		{ 0, 2, 0, -128, -96, 0 },
		{ 0, 3, 0, 16, -32, -32 },
		{ 0, 1, 0, 16, -32, 32 },
		{ 0, 0, 0, 0, -64, 0 },
		{ 0, 0, 0, 0, -64, 0 },
		{ 0, 0, 0, 0, -32, 0 },
		{ 0, 0, 80, 0, 32, 0 },
		{ 0, 0, 240, 0, -160, 0 },
		{ 0, 0, 0, 0, 0, 0 },
		{ 0, 3, 0, 32, -32, -32 },
		{ 0, 1, 0, 32, -32, 32 },
		{ 0, 3, 32, 0, -32, -32 },
		{ 0, 1, 32, 0, -32, 32 },
		{ 0, 3, 0, 64, -64, -64 },
		{ 0, 1, 0, 64, -64, 64 },
		{ 0, 3, 64, 0, -64, -64 },
		{ 0, 1, 64, 0, -64, 64 },
		{ 0, 0, 0, 16, 0, 0 },
		{ 0, 0, 0, 16, 0, 0 },
		{ 0, 0, 0, 16, 0, 0 },
		{ 0, 0, 0, 16, 0, 0 },
		{ 0, 0, 16, 0, 0, 0 },
		{ 0, 0, 16, 0, 0, 0 },
		{ 0, 0, 16, 0, 0, 0 },
		{ 0, 0, 16, 0, 0, 0 },
		{ 0, 0, 0, 8, 0, 0 },
		{ 0, 0, 0, 8, 0, 0 },
		{ 0, 0, 0, 8, 0, 0 },
		{ 0, 0, 0, 8, 0, 0 },
		{ 0, 0, 8, 0, 0, 0 },
		{ 0, 0, 8, 0, 0, 0 },
		{ 0, 0, 8, 0, 0, 0 },
		{ 0, 0, 8, 0, 0, 0 },
		{ 0, 0, 0, 8, 0, 0 },
		{ 0, 0, 0, 8, 0, 0 },
		{ 0, 0, 0, 8, 0, 0 },
		{ 0, 0, 0, 8, 0, 0 },
		{ 0, 0, 8, 0, 0, 0 },
		{ 0, 0, 8, 0, 0, 0 },
		{ 0, 0, 8, 0, 0, 0 },
		{ 0, 0, 8, 0, 0, 0 },
		{ 0, 3, 0, 96, 0, 32 },
		{ 0, 1, 0, 96, 0, -32 },
		{ 0, 3, 96, 0, 0, 32 },
		{ 0, 1, 96, 0, 0, -32 },
		{ 0, 2, 0, 96, 64, 0 },
		{ 0, 2, 0, -128, -96, 0 },
		{ 0, 2, 0, 128, 64, 0 }
};

// rct2: 0x0097C468 (0 - 31) and 0x0097C5D4 (32 - 63)
const uint64 RideTypePossibleTrackConfigurations[91] = {
	/* RIDE_TYPE_SPIRAL_ROLLER_COASTER				*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_FLAT_ROLL_BANKING | TRACK_VERTICAL_LOOP | TRACK_SLOPE | TRACK_SLOPE_STEEP | TRACK_SLOPE_CURVE | TRACK_SLOPE_CURVE_STEEP | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_CURVE | TRACK_HELIX_SMALL | TRACK_BRAKES | TRACK_ON_RIDE_PHOTO,
	/* RIDE_TYPE_STAND_UP_ROLLER_COASTER			*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_LIFT_HILL | TRACK_FLAT_ROLL_BANKING | TRACK_VERTICAL_LOOP | TRACK_SLOPE | TRACK_SLOPE_STEEP | TRACK_SLOPE_CURVE | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_CURVE | TRACK_HALF_LOOP | TRACK_CORKSCREW | TRACK_HELIX_SMALL | TRACK_BRAKES | TRACK_ON_RIDE_PHOTO,
	/* RIDE_TYPE_SUSPENDED_SWINGING_COASTER			*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_LIFT_HILL | TRACK_FLAT_ROLL_BANKING | TRACK_SLOPE | TRACK_SLOPE_STEEP | TRACK_SLOPE_CURVE | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_CURVE | TRACK_HELIX_LARGE_UNBANKED | TRACK_BRAKES,
	/* RIDE_TYPE_INVERTED_ROLLER_COASTER			*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_LIFT_HILL | TRACK_FLAT_ROLL_BANKING | TRACK_VERTICAL_LOOP | TRACK_SLOPE | TRACK_SLOPE_STEEP | TRACK_SLOPE_CURVE | TRACK_SLOPE_CURVE_STEEP | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_CURVE | TRACK_TWIST | TRACK_HALF_LOOP | TRACK_CORKSCREW | TRACK_HELIX_LARGE | TRACK_BRAKES | TRACK_ON_RIDE_PHOTO | TRACK_HALF_LOOP_LARGE,
	/* RIDE_TYPE_JUNIOR_ROLLER_COASTER				*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_LIFT_HILL | TRACK_LIFT_HILL_CURVE | TRACK_FLAT_ROLL_BANKING | TRACK_SLOPE | TRACK_SLOPE_CURVE | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_CURVE | TRACK_HELIX_SMALL | TRACK_BRAKES,
	/* RIDE_TYPE_MINIATURE_RAILWAY					*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_SLOPE | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_CURVE,
	/* RIDE_TYPE_MONORAIL							*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_SLOPE | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_CURVE,
	/* RIDE_TYPE_MINI_SUSPENDED_COASTER				*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_LIFT_HILL | TRACK_SLOPE | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_CURVE,
	/* RIDE_TYPE_BOAT_RIDE							*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_CURVE_VERY_SMALL,
	/* RIDE_TYPE_WOODEN_WILD_MOUSE					*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_LIFT_HILL | TRACK_LIFT_HILL_STEEP | TRACK_SLOPE | TRACK_SLOPE_STEEP | TRACK_SLOPE_LONG | TRACK_CURVE_VERY_SMALL | TRACK_CURVE_SMALL,
	/* RIDE_TYPE_STEEPLECHASE						*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_LIFT_HILL | TRACK_FLAT_ROLL_BANKING | TRACK_SLOPE | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_CURVE | TRACK_BRAKES,
	/* RIDE_TYPE_CAR_RIDE							*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_SLOPE | TRACK_SLOPE_STEEP | TRACK_CURVE_VERY_SMALL | TRACK_CURVE_SMALL | TRACK_TOWER_BASE,
	/* RIDE_TYPE_LAUNCHED_FREEFALL					*/	TRACK_TOWER_BASE,
	/* RIDE_TYPE_BOBSLEIGH_COASTER					*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_LIFT_HILL | TRACK_FLAT_ROLL_BANKING | TRACK_SLOPE | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_CURVE | TRACK_HELIX_SMALL | TRACK_BRAKES,
	/* RIDE_TYPE_OBSERVATION_TOWER					*/	TRACK_TOWER_BASE,
	/* RIDE_TYPE_LOOPING_ROLLER_COASTER				*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_LIFT_HILL | TRACK_FLAT_ROLL_BANKING | TRACK_VERTICAL_LOOP | TRACK_SLOPE | TRACK_SLOPE_STEEP | TRACK_SLOPE_CURVE | TRACK_SLOPE_CURVE_STEEP | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_CURVE | TRACK_HELIX_SMALL | TRACK_BRAKES | TRACK_ON_RIDE_PHOTO,
	/* RIDE_TYPE_DINGHY_SLIDE						*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_LIFT_HILL | TRACK_SLOPE | TRACK_SLOPE_STEEP | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_CURVE,
	/* RIDE_TYPE_MINE_TRAIN_COASTER					*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_LIFT_HILL | TRACK_FLAT_ROLL_BANKING | TRACK_SLOPE | TRACK_SLOPE_STEEP | TRACK_SLOPE_CURVE | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_CURVE | TRACK_HELIX_SMALL | TRACK_BRAKES | TRACK_ON_RIDE_PHOTO,
	/* RIDE_TYPE_CHAIRLIFT							*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_SLOPE | TRACK_CURVE_VERY_SMALL,
	/* RIDE_TYPE_CORKSCREW_ROLLER_COASTER			*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_LIFT_HILL | TRACK_FLAT_ROLL_BANKING | TRACK_VERTICAL_LOOP | TRACK_SLOPE | TRACK_SLOPE_STEEP | TRACK_SLOPE_CURVE | TRACK_SLOPE_CURVE_STEEP | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_CURVE | TRACK_HALF_LOOP | TRACK_CORKSCREW | TRACK_HELIX_SMALL | TRACK_BRAKES | TRACK_ON_RIDE_PHOTO,
	/* RIDE_TYPE_MAZE								*/	0,
	/* RIDE_TYPE_SPIRAL_SLIDE						*/	0,
	/* RIDE_TYPE_GO_KARTS							*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_SLOPE | TRACK_CURVE_VERY_SMALL,
	/* RIDE_TYPE_LOG_FLUME							*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_SLOPE | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_ON_RIDE_PHOTO,
	/* RIDE_TYPE_RIVER_RAPIDS						*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_SLOPE | TRACK_CURVE_VERY_SMALL | TRACK_TOWER_BASE | TRACK_ON_RIDE_PHOTO,
	/* RIDE_TYPE_DODGEMS							*/	0,
	/* RIDE_TYPE_PIRATE_SHIP						*/	0,
	/* RIDE_TYPE_SWINGING_INVERTER_SHIP				*/	0,
	/* RIDE_TYPE_FOOD_STALL							*/	0,
	/* RIDE_TYPE_1D									*/	0,
	/* RIDE_TYPE_DRINK_STALL						*/	0,
	/* RIDE_TYPE_1F									*/	0,
	/* RIDE_TYPE_SHOP								*/	0,
	/* RIDE_TYPE_MERRY_GO_ROUND						*/	0,
	/* RIDE_TYPE_22									*/	0,
	/* RIDE_TYPE_INFORMATION_KIOSK					*/	0,
	/* RIDE_TYPE_TOILETS							*/	0,
	/* RIDE_TYPE_FERRIS_WHEEL						*/	0,
	/* RIDE_TYPE_MOTION_SIMULATOR					*/	0,
	/* RIDE_TYPE_3D_CINEMA							*/	0,
	/* RIDE_TYPE_TOP_SPIN							*/	0,
	/* RIDE_TYPE_SPACE_RINGS						*/	0,
	/* RIDE_TYPE_REVERSE_FREEFALL_COASTER			*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_LIFT_HILL_STEEP,
	/* RIDE_TYPE_LIFT								*/	TRACK_TOWER_BASE,
	/* RIDE_TYPE_VERTICAL_DROP_ROLLER_COASTER		*/	TRACK_FLAT | TRACK_STRAIGHT | TRACK_STATION_END | TRACK_LIFT_HILL | TRACK_LIFT_HILL_STEEP | TRACK_FLAT_ROLL_BANKING | TRACK_VERTICAL_LOOP | TRACK_SLOPE | TRACK_SLOPE_STEEP | TRACK_SLOPE_LONG | TRACK_SLOPE_CURVE | TRACK_SLOPE_CURVE_STEEP | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_CURVE | TRACK_HELIX_SMALL | TRACK_BRAKES | TRACK_ON_RIDE_PHOTO | TRACK_SLOPE_VERTICAL,
	/* RIDE_TYPE_CASH_MACHINE						*/	0,
	/* RIDE_TYPE_TWIST								*/	0,
	/* RIDE_TYPE_HAUNTED_HOUSE						*/	0,
	/* RIDE_TYPE_FIRST_AID							*/	0,
	/* RIDE_TYPE_CIRCUS_SHOW						*/	0,
	/* RIDE_TYPE_GHOST_TRAIN						*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_SLOPE | TRACK_CURVE_VERY_SMALL | TRACK_CURVE_SMALL | TRACK_BRAKES,
	/* RIDE_TYPE_TWISTER_ROLLER_COASTER				*/	TRACK_FLAT | TRACK_STRAIGHT | TRACK_STATION_END | TRACK_LIFT_HILL | TRACK_FLAT_ROLL_BANKING | TRACK_VERTICAL_LOOP | TRACK_SLOPE | TRACK_SLOPE_STEEP | TRACK_SLOPE_CURVE | TRACK_SLOPE_CURVE_STEEP | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_CURVE | TRACK_HALF_LOOP | TRACK_CORKSCREW | TRACK_HELIX_SMALL | TRACK_BRAKES | TRACK_ON_RIDE_PHOTO | TRACK_SLOPE_VERTICAL | TRACK_BARREL_ROLL | TRACK_POWERED_LIFT | TRACK_HALF_LOOP_LARGE,
	/* RIDE_TYPE_WOODEN_ROLLER_COASTER				*/	TRACK_FLAT | TRACK_STRAIGHT | TRACK_STATION_END | TRACK_LIFT_HILL | TRACK_FLAT_ROLL_BANKING | TRACK_VERTICAL_LOOP | TRACK_SLOPE | TRACK_SLOPE_STEEP | TRACK_SLOPE_CURVE | TRACK_SLOPE_CURVE_STEEP | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_CURVE | TRACK_HELIX_SMALL | TRACK_BRAKES | TRACK_ON_RIDE_PHOTO | TRACK_WATER_SPLASH,
	/* RIDE_TYPE_SIDE_FRICTION_ROLLER_COASTER		*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_LIFT_HILL | TRACK_SLOPE | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_CURVE | TRACK_BRAKES,
	/* RIDE_TYPE_WILD_MOUSE							*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_LIFT_HILL | TRACK_LIFT_HILL_STEEP | TRACK_FLAT_ROLL_BANKING | TRACK_SLOPE | TRACK_SLOPE_STEEP | TRACK_SLOPE_LONG | TRACK_SLOPE_CURVE | TRACK_CURVE_VERY_SMALL | TRACK_CURVE_SMALL | TRACK_BRAKES,
	/* RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER		*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_LIFT_HILL | TRACK_FLAT_ROLL_BANKING | TRACK_SLOPE | TRACK_SLOPE_STEEP | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_CURVE | TRACK_HALF_LOOP | TRACK_HELIX_SMALL | TRACK_BRAKES | TRACK_ON_RIDE_PHOTO | TRACK_SLOPE_VERTICAL,
	/* RIDE_TYPE_38									*/	TRACK_STRAIGHT | TRACK_FLAT_ROLL_BANKING | TRACK_SLOPE | TRACK_SLOPE_STEEP | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_CURVE | TRACK_TWIST | TRACK_CORKSCREW | TRACK_BRAKES | TRACK_ON_RIDE_PHOTO | TRACK_SLOPE_VERTICAL,
	/* RIDE_TYPE_FLYING_ROLLER_COASTER				*/	TRACK_STRAIGHT | TRACK_FLAT_ROLL_BANKING | TRACK_VERTICAL_LOOP | TRACK_SLOPE | TRACK_SLOPE_STEEP | TRACK_SLOPE_CURVE | TRACK_SLOPE_CURVE_STEEP | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_CURVE | TRACK_HELIX_SMALL | TRACK_BRAKES | TRACK_ON_RIDE_PHOTO,
	/* RIDE_TYPE_3A									*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_LIFT_HILL | TRACK_FLAT_ROLL_BANKING | TRACK_VERTICAL_LOOP | TRACK_SLOPE | TRACK_SLOPE_STEEP | TRACK_SLOPE_CURVE | TRACK_SLOPE_CURVE_STEEP | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_CURVE | TRACK_TWIST | TRACK_HELIX_LARGE | TRACK_BRAKES | TRACK_ON_RIDE_PHOTO,
	/* RIDE_TYPE_VIRGINIA_REEL						*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_LIFT_HILL | TRACK_SLOPE | TRACK_CURVE_VERY_SMALL | TRACK_CURVE_SMALL,
	/* RIDE_TYPE_SPLASH_BOATS						*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_SLOPE | TRACK_SLOPE_STEEP | TRACK_S_BEND | TRACK_CURVE,
	/* RIDE_TYPE_MINI_HELICOPTERS					*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_SLOPE | TRACK_CURVE_VERY_SMALL | TRACK_CURVE_SMALL,
	/* RIDE_TYPE_LAY_DOWN_ROLLER_COASTER			*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_LIFT_HILL | TRACK_FLAT_ROLL_BANKING | TRACK_VERTICAL_LOOP | TRACK_SLOPE | TRACK_SLOPE_STEEP | TRACK_SLOPE_CURVE | TRACK_SLOPE_CURVE_STEEP | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_CURVE | TRACK_HELIX_SMALL | TRACK_BRAKES | TRACK_ON_RIDE_PHOTO,
	/* RIDE_TYPE_SUSPENDED_MONORAIL					*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_SLOPE | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_CURVE,
	/* RIDE_TYPE_40									*/	TRACK_STRAIGHT | TRACK_FLAT_ROLL_BANKING | TRACK_SLOPE | TRACK_SLOPE_STEEP | TRACK_SLOPE_CURVE | TRACK_SLOPE_CURVE_STEEP | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_CURVE | TRACK_TWIST | TRACK_HELIX_LARGE | TRACK_BRAKES | TRACK_ON_RIDE_PHOTO,
	/* RIDE_TYPE_REVERSER_ROLLER_COASTER			*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_LIFT_HILL | TRACK_SLOPE | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_CURVE | TRACK_BRAKES,
	/* RIDE_TYPE_HEARTLINE_TWISTER_COASTER			*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_LIFT_HILL | TRACK_LIFT_HILL_STEEP | TRACK_SLOPE | TRACK_SLOPE_STEEP,
	/* RIDE_TYPE_MINI_GOLF							*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_SLOPE | TRACK_CURVE_VERY_SMALL,
	/* RIDE_TYPE_GIGA_COASTER						*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_LIFT_HILL | TRACK_FLAT_ROLL_BANKING | TRACK_VERTICAL_LOOP | TRACK_SLOPE | TRACK_SLOPE_STEEP | TRACK_SLOPE_CURVE | TRACK_SLOPE_CURVE_STEEP | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_CURVE | TRACK_HELIX_SMALL | TRACK_BRAKES | TRACK_ON_RIDE_PHOTO,
	/* RIDE_TYPE_ROTO_DROP							*/	TRACK_TOWER_BASE,
	/* RIDE_TYPE_FLYING_SAUCERS						*/	0,
	/* RIDE_TYPE_CROOKED_HOUSE						*/	0,
	/* RIDE_TYPE_MONORAIL_CYCLES					*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_CURVE,
	/* RIDE_TYPE_COMPACT_INVERTED_COASTER			*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_LIFT_HILL | TRACK_FLAT_ROLL_BANKING | TRACK_VERTICAL_LOOP | TRACK_SLOPE | TRACK_SLOPE_STEEP | TRACK_SLOPE_CURVE | TRACK_SLOPE_CURVE_STEEP | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_CURVE | TRACK_TWIST | TRACK_HALF_LOOP | TRACK_CORKSCREW | TRACK_HELIX_LARGE | TRACK_BRAKES | TRACK_ON_RIDE_PHOTO | TRACK_SLOPE_VERTICAL,
	/* RIDE_TYPE_WATER_COASTER						*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_LIFT_HILL | TRACK_FLAT_ROLL_BANKING | TRACK_SLOPE | TRACK_SLOPE_STEEP | TRACK_SLOPE_CURVE | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_CURVE | TRACK_HELIX_SMALL | TRACK_BRAKES,
	/* RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER		*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_LIFT_HILL_STEEP | TRACK_LIFT_HILL_CURVE | TRACK_FLAT_ROLL_BANKING | TRACK_CURVE | TRACK_BRAKES,
	/* RIDE_TYPE_INVERTED_HAIRPIN_COASTER			*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_LIFT_HILL | TRACK_LIFT_HILL_STEEP | TRACK_FLAT_ROLL_BANKING | TRACK_SLOPE | TRACK_SLOPE_STEEP | TRACK_SLOPE_LONG | TRACK_SLOPE_CURVE | TRACK_CURVE_VERY_SMALL | TRACK_CURVE_SMALL | TRACK_BRAKES,
	/* RIDE_TYPE_MAGIC_CARPET						*/	0,
	/* RIDE_TYPE_SUBMARINE_RIDE						*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_CURVE_VERY_SMALL | TRACK_CURVE_SMALL,
	/* RIDE_TYPE_RIVER_RAFTS						*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_S_BEND | TRACK_CURVE,
	/* RIDE_TYPE_50									*/	0,
	/* RIDE_TYPE_ENTERPRISE							*/	0,
	/* RIDE_TYPE_52									*/	0,
	/* RIDE_TYPE_53									*/	0,
	/* RIDE_TYPE_54									*/	0,
	/* RIDE_TYPE_55									*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_LIFT_HILL | TRACK_FLAT_ROLL_BANKING | TRACK_SLOPE | TRACK_SLOPE_STEEP | TRACK_SLOPE_CURVE | TRACK_SLOPE_CURVE_STEEP | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_CURVE | TRACK_HELIX_SMALL | TRACK_BRAKES | TRACK_ON_RIDE_PHOTO,
	/* RIDE_TYPE_INVERTED_IMPULSE_COASTER			*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_SLOPE | TRACK_SLOPE_STEEP | TRACK_SLOPE_VERTICAL,
	/* RIDE_TYPE_MINI_ROLLER_COASTER				*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_LIFT_HILL | TRACK_FLAT_ROLL_BANKING | TRACK_VERTICAL_LOOP | TRACK_SLOPE | TRACK_SLOPE_STEEP | TRACK_SLOPE_CURVE | TRACK_SLOPE_CURVE_STEEP | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_CURVE | TRACK_HELIX_SMALL | TRACK_BRAKES,
	/* RIDE_TYPE_MINE_RIDE							*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_FLAT_ROLL_BANKING | TRACK_SLOPE | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_CURVE | TRACK_HELIX_SMALL | TRACK_ON_RIDE_PHOTO,
	/* RIDE_TYPE_59									*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_LIFT_HILL | TRACK_FLAT_ROLL_BANKING | TRACK_SLOPE | TRACK_CURVE_VERY_SMALL | TRACK_CURVE_SMALL | TRACK_BRAKES,
	/* RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER		*/	TRACK_STRAIGHT | TRACK_STATION_END | TRACK_FLAT_ROLL_BANKING | TRACK_VERTICAL_LOOP | TRACK_SLOPE | TRACK_SLOPE_STEEP | TRACK_SLOPE_CURVE | TRACK_SLOPE_CURVE_STEEP | TRACK_S_BEND | TRACK_CURVE_SMALL | TRACK_CURVE | TRACK_TWIST | TRACK_CORKSCREW | TRACK_HELIX_SMALL | TRACK_BRAKES | TRACK_ON_RIDE_PHOTO | TRACK_SLOPE_VERTICAL
};

#define TRACK_BLOCK_END { 255, 255, 255, 255, 255, 255, 255 }

static const rct_preview_track TrackBlocks000[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks001[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks002[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks003[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks004[] = {
	{ 0, 0, 0, 0, 16, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks005[] = {
	{ 0, 0, 0, 0, 64, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks006[] = {
	{ 0, 0, 0, 0, 8, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks007[] = {
	{ 0, 0, 0, 0, 32, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks008[] = {
	{ 0, 0, 0, 0, 32, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks009[] = {
	{ 0, 0, 0, 0, 8, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks010[] = {
	{ 0, 0, 0, 0, 16, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks011[] = {
	{ 0, 0, 0, 0, 64, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks012[] = {
	{ 0, 0, 0, 0, 8, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks013[] = {
	{ 0, 0, 0, 0, 32, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks014[] = {
	{ 0, 0, 0, 0, 32, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks015[] = {
	{ 0, 0, 0, 0, 8, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks016[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, 0, -32, 0, 0, 8, 1 },
	{ 2, -32, 0, 0, 0, 7, 0 },
	{ 3, -32, -32, 0, 0, 13, 0 },
	{ 4, -32, -64, 0, 0, 8, 1 },
	{ 5, -64, -32, 0, 0, 7, 0 },
	{ 6, -64, -64, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks017[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 0, 11, 0 },
	{ 3, -32, 32, 0, 0, 14, 0 },
	{ 4, -32, 64, 0, 0, 4, 1 },
	{ 5, -64, 32, 0, 0, 11, 0 },
	{ 6, -64, 64, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks018[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks019[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks020[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks021[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks022[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, 0, -32, 0, 0, 8, 1 },
	{ 2, -32, 0, 0, 0, 7, 0 },
	{ 3, -32, -32, 0, 0, 13, 0 },
	{ 4, -32, -64, 0, 0, 8, 1 },
	{ 5, -64, -32, 0, 0, 7, 0 },
	{ 6, -64, -64, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks023[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 0, 11, 0 },
	{ 3, -32, 32, 0, 0, 14, 0 },
	{ 4, -32, 64, 0, 0, 4, 1 },
	{ 5, -64, 32, 0, 0, 11, 0 },
	{ 6, -64, 64, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks024[] = {
	{ 0, 0, 0, 0, 8, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks025[] = {
	{ 0, 0, 0, 0, 8, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks026[] = {
	{ 0, 0, 0, 0, 8, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks027[] = {
	{ 0, 0, 0, 0, 8, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks028[] = {
	{ 0, 0, 0, 0, 8, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks029[] = {
	{ 0, 0, 0, 0, 8, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks030[] = {
	{ 0, 0, 0, 0, 8, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks031[] = {
	{ 0, 0, 0, 0, 8, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks032[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks033[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks034[] = {
	{ 0, 0, 0, 0, 16, 207, 0 },
	{ 1, 0, -32, 16, 0, 8, 1 },
	{ 2, -32, 0, 16, 16, 71, 0 },
	{ 3, -32, -32, 24, 16, 13, 0 },
	{ 4, -32, -64, 48, 0, 8, 1 },
	{ 5, -64, -32, 32, 16, 71, 0 },
	{ 6, -64, -64, 48, 16, 111, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks035[] = {
	{ 0, 0, 0, 0, 16, 207, 0 },
	{ 1, 0, 32, 16, 0, 4, 1 },
	{ 2, -32, 0, 16, 16, 139, 0 },
	{ 3, -32, 32, 24, 16, 14, 0 },
	{ 4, -32, 64, 48, 0, 4, 1 },
	{ 5, -64, 32, 32, 16, 139, 0 },
	{ 6, -64, 64, 48, 16, 159, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks036[] = {
	{ 0, 0, 0, 48, 16, 63, 0 },
	{ 1, 0, -32, 48, 0, 8, 1 },
	{ 2, -32, 0, 32, 16, 23, 0 },
	{ 3, -32, -32, 24, 16, 13, 0 },
	{ 4, -32, -64, 16, 0, 8, 1 },
	{ 5, -64, -32, 16, 16, 23, 0 },
	{ 6, -64, -64, 0, 16, 159, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks037[] = {
	{ 0, 0, 0, 48, 16, 63, 0 },
	{ 1, 0, 32, 48, 0, 4, 1 },
	{ 2, -32, 0, 32, 16, 43, 0 },
	{ 3, -32, 32, 24, 16, 14, 0 },
	{ 4, -32, 64, 16, 0, 4, 1 },
	{ 5, -64, 32, 16, 16, 43, 0 },
	{ 6, -64, 64, 0, 16, 111, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks038[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 0, 7, 0 },
	{ 2, -32, -32, 0, 0, 13, 0 },
	{ 3, -64, -32, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks039[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 0, 11, 0 },
	{ 2, -32, 32, 0, 0, 14, 0 },
	{ 3, -64, 32, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks040[] = {
	{ 0, 0, 0, 0, 16, 207, 0 },
	{ 1, -32, 0, 16, 16, 207, 0 },
	{ 2, -64, 0, 32, 96, 2, 0 },
	{ 3, -32, 0, 120, 16, 6, 0 },
	{ 4, -32, -32, 120, 0, 0, 0 },
	{ 5, 0, 0, 120, 0, 0, 0 },
	{ 6, 0, -32, 120, 16, 9, 0 },
	{ 7, 32, -32, 32, 96, 8, 0 },
	{ 8, 0, -32, 16, 16, 63, 0 },
	{ 9, -32, -32, 0, 16, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks041[] = {
	{ 0, 0, 0, 0, 16, 207, 0 },
	{ 1, -32, 0, 16, 16, 207, 0 },
	{ 2, -64, 0, 32, 96, 1, 0 },
	{ 3, -32, 0, 120, 16, 9, 0 },
	{ 4, -32, 32, 120, 0, 0, 0 },
	{ 5, 0, 0, 120, 0, 0, 0 },
	{ 6, 0, 32, 120, 16, 6, 0 },
	{ 7, 32, 32, 32, 96, 4, 0 },
	{ 8, 0, 32, 16, 16, 63, 0 },
	{ 9, -32, 32, 0, 16, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks042[] = {
	{ 0, 0, 0, 0, 0, 7, 0 },
	{ 1, 0, -32, 0, 0, 8, 1 },
	{ 2, -32, 0, 0, 0, 2, 0 },
	{ 3, -32, -32, 0, 0, 7, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks043[] = {
	{ 0, 0, 0, 0, 0, 11, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 0, 1, 0 },
	{ 3, -32, 32, 0, 0, 11, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks044[] = {
	{ 0, 0, 0, 0, 0, 7, 0 },
	{ 1, 0, -32, 0, 0, 8, 1 },
	{ 2, -32, 0, 0, 0, 2, 0 },
	{ 3, -32, -32, 0, 0, 7, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks045[] = {
	{ 0, 0, 0, 0, 0, 11, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 0, 1, 0 },
	{ 3, -32, 32, 0, 0, 11, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks046[] = {
	{ 0, 0, 0, 0, 16, 71, 0 },
	{ 1, 0, -32, 16, 0, 8, 1 },
	{ 2, -32, 0, 16, 0, 2, 0 },
	{ 3, -32, -32, 16, 16, 103, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks047[] = {
	{ 0, 0, 0, 0, 16, 139, 0 },
	{ 1, 0, 32, 16, 0, 4, 1 },
	{ 2, -32, 0, 16, 0, 1, 0 },
	{ 3, -32, 32, 16, 16, 155, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks048[] = {
	{ 0, 0, 0, 16, 16, 55, 0 },
	{ 1, 0, -32, 16, 0, 8, 1 },
	{ 2, -32, 0, 16, 0, 2, 0 },
	{ 3, -32, -32, 0, 16, 23, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks049[] = {
	{ 0, 0, 0, 16, 16, 59, 0 },
	{ 1, 0, 32, 16, 0, 4, 1 },
	{ 2, -32, 0, 16, 0, 1, 0 },
	{ 3, -32, 32, 0, 16, 43, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks050[] = {
	{ 0, 0, 0, 0, 0, 7, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks051[] = {
	{ 0, 0, 0, 0, 0, 11, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks052[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 16, 15, 0 },
	{ 2, -64, 0, 16, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks053[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 16, 15, 0 },
	{ 2, -64, 0, 16, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks054[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, -16, 16, 15, 0 },
	{ 2, -64, 0, -16, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks055[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, -16, 16, 15, 0 },
	{ 2, -64, 0, -16, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks056[] = {
	{ 0, 0, 0, 0, 32, 207, 0 },
	{ 1, -32, 0, 16, 16, 207, 0 },
	{ 2, -64, 0, 32, 96, 3, 0 },
	{ 3, -32, 0, 120, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks057[] = {
	{ 0, 0, 0, -32, 32, 15, 0 },
	{ 1, -32, 0, -120, 96, 3, 0 },
	{ 2, 0, 0, -136, 16, 207, 0 },
	{ 3, 32, 0, -152, 16, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks058[] = {
	{ 0, 0, 0, 0, 16, 15, 0 },
	{ 1, -32, 0, 24, 32, 7, 0 },
	{ 2, -32, -32, 48, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks059[] = {
	{ 0, 0, 0, 0, 16, 15, 0 },
	{ 1, -32, 0, 24, 32, 11, 0 },
	{ 2, -32, 32, 48, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks060[] = {
	{ 0, 0, 0, -32, 16, 15, 0 },
	{ 1, -32, 0, -56, 32, 7, 0 },
	{ 2, -32, -32, -80, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks061[] = {
	{ 0, 0, 0, -32, 16, 15, 0 },
	{ 1, -32, 0, -56, 32, 11, 0 },
	{ 2, -32, 32, -80, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks062[] = {
	{ 0, 0, 0, 0, 24, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks063[] = {
	{ 0, 0, 0, 0, 24, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks064[] = {
	{ 0, 0, 0, 0, 24, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks065[] = {
	{ 0, 0, 0, 0, 24, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks066[] = {
	{ 0, 0, 0, 0, 64, 15, 0 },
	{ 1, -32, -32, 0, 0, 15, 0 },
	{ 2, -32, 0, 0, 0, 15, 0 },
	{ 3, -32, 32, 0, 0, 15, 0 },
	{ 4, 0, -32, 0, 0, 15, 0 },
	{ 5, 0, 32, 0, 0, 15, 0 },
	{ 6, 32, -32, 0, 0, 15, 0 },
	{ 7, 32, 32, 0, 0, 15, 0 },
	{ 8, 32, 0, 0, 0, 15, 2 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks067[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, 32, 0, 0, 0, 0, 3 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks068[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks069[] = {
	{ 0, 0, 0, 0, 16, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks070[] = {
	{ 0, 0, 0, 0, 64, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks071[] = {
	{ 0, 0, 0, 0, 8, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks072[] = {
	{ 0, 0, 0, 0, 32, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks073[] = {
	{ 0, 0, 0, 0, 32, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks074[] = {
	{ 0, 0, 0, 0, 8, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks075[] = {
	{ 0, 0, 0, 0, 16, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks076[] = {
	{ 0, 0, 0, 0, 64, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks077[] = {
	{ 0, 0, 0, 0, 8, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks078[] = {
	{ 0, 0, 0, 0, 32, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks079[] = {
	{ 0, 0, 0, 0, 32, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks080[] = {
	{ 0, 0, 0, 0, 8, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks081[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, 0, -32, 0, 0, 8, 1 },
	{ 2, -32, 0, 0, 0, 7, 0 },
	{ 3, -32, -32, 0, 0, 13, 0 },
	{ 4, -32, -64, 0, 0, 8, 1 },
	{ 5, -64, -32, 0, 0, 7, 0 },
	{ 6, -64, -64, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks082[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 0, 11, 0 },
	{ 3, -32, 32, 0, 0, 14, 0 },
	{ 4, -32, 64, 0, 0, 4, 1 },
	{ 5, -64, 32, 0, 0, 11, 0 },
	{ 6, -64, 64, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks083[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 0, 7, 0 },
	{ 2, -32, -32, 0, 0, 13, 0 },
	{ 3, -64, -32, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks084[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 0, 11, 0 },
	{ 2, -32, 32, 0, 0, 14, 0 },
	{ 3, -64, 32, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks085[] = {
	{ 0, 0, 0, 0, 0, 7, 0 },
	{ 1, 0, -32, 0, 0, 8, 1 },
	{ 2, -32, 0, 0, 0, 2, 0 },
	{ 3, -32, -32, 0, 0, 7, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks086[] = {
	{ 0, 0, 0, 0, 0, 11, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 0, 1, 0 },
	{ 3, -32, 32, 0, 0, 11, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks087[] = {
	{ 0, 0, 0, 0, 0, 7, 0 },
	{ 1, 0, -32, 0, 0, 8, 1 },
	{ 2, -32, 0, 0, 4, 2, 0 },
	{ 3, -32, -32, 0, 4, 7, 0 },
	{ 4, -32, -64, 8, 0, 11, 0 },
	{ 5, 0, -64, 8, 0, 4, 1 },
	{ 6, -32, -96, 8, 4, 1, 0 },
	{ 7, 0, -96, 8, 4, 11, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks088[] = {
	{ 0, 0, 0, 0, 0, 11, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 4, 1, 0 },
	{ 3, -32, 32, 0, 4, 11, 0 },
	{ 4, -32, 64, 8, 0, 7, 0 },
	{ 5, 0, 64, 8, 0, 8, 1 },
	{ 6, -32, 96, 8, 4, 2, 0 },
	{ 7, 0, 96, 8, 4, 7, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks089[] = {
	{ 0, 0, 0, 8, 4, 7, 0 },
	{ 1, 0, -32, 8, 4, 8, 1 },
	{ 2, -32, 0, 8, 0, 2, 0 },
	{ 3, -32, -32, 8, 0, 7, 0 },
	{ 4, -32, -64, 0, 4, 11, 0 },
	{ 5, 0, -64, 0, 4, 4, 1 },
	{ 6, -32, -96, 0, 0, 1, 0 },
	{ 7, 0, -96, 0, 0, 11, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks090[] = {
	{ 0, 0, 0, 8, 4, 11, 0 },
	{ 1, 0, 32, 8, 4, 4, 1 },
	{ 2, -32, 0, 8, 0, 1, 0 },
	{ 3, -32, 32, 8, 0, 11, 0 },
	{ 4, -32, 64, 0, 4, 7, 0 },
	{ 5, 0, 64, 0, 4, 8, 1 },
	{ 6, -32, 96, 0, 0, 2, 0 },
	{ 7, 0, 96, 0, 0, 7, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks091[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, 0, -32, 0, 0, 8, 1 },
	{ 2, -32, 0, 0, 0, 7, 0 },
	{ 3, -32, -32, 0, 0, 13, 0 },
	{ 4, -32, -64, 0, 4, 8, 1 },
	{ 5, -64, -32, 0, 4, 7, 0 },
	{ 6, -64, -64, 0, 4, 15, 0 },
	{ 7, -64, -96, 8, 0, 15, 0 },
	{ 8, -32, -96, 8, 0, 4, 1 },
	{ 9, -64, -128, 8, 0, 11, 0 },
	{ 10, -32, -128, 8, 0, 14, 0 },
	{ 11, 0, -128, 8, 4, 4, 1 },
	{ 12, -32, -160, 8, 4, 11, 0 },
	{ 13, 0, -160, 8, 4, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks092[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 0, 11, 0 },
	{ 3, -32, 32, 0, 0, 14, 0 },
	{ 4, -32, 64, 0, 4, 4, 1 },
	{ 5, -64, 32, 0, 4, 11, 0 },
	{ 6, -64, 64, 0, 4, 15, 0 },
	{ 7, -64, 96, 8, 0, 15, 0 },
	{ 8, -32, 96, 8, 0, 8, 1 },
	{ 9, -64, 128, 8, 0, 7, 0 },
	{ 10, -32, 128, 8, 0, 13, 0 },
	{ 11, 0, 128, 8, 4, 8, 1 },
	{ 12, -32, 160, 8, 4, 7, 0 },
	{ 13, 0, 160, 8, 4, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks093[] = {
	{ 0, 0, 0, 8, 4, 15, 0 },
	{ 1, 0, -32, 8, 4, 8, 1 },
	{ 2, -32, 0, 8, 4, 7, 0 },
	{ 3, -32, -32, 8, 0, 13, 0 },
	{ 4, -32, -64, 8, 0, 8, 1 },
	{ 5, -64, -32, 8, 0, 7, 0 },
	{ 6, -64, -64, 8, 0, 15, 0 },
	{ 7, -64, -96, 0, 4, 15, 0 },
	{ 8, -32, -96, 0, 4, 4, 1 },
	{ 9, -64, -128, 0, 4, 11, 0 },
	{ 10, -32, -128, 0, 0, 14, 0 },
	{ 11, 0, -128, 0, 0, 4, 1 },
	{ 12, -32, -160, 0, 0, 11, 0 },
	{ 13, 0, -160, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks094[] = {
	{ 0, 0, 0, 8, 4, 15, 0 },
	{ 1, 0, 32, 8, 4, 4, 1 },
	{ 2, -32, 0, 8, 4, 11, 0 },
	{ 3, -32, 32, 8, 0, 14, 0 },
	{ 4, -32, 64, 8, 0, 4, 1 },
	{ 5, -64, 32, 8, 0, 11, 0 },
	{ 6, -64, 64, 8, 0, 15, 0 },
	{ 7, -64, 96, 0, 4, 15, 0 },
	{ 8, -32, 96, 0, 4, 8, 1 },
	{ 9, -64, 128, 0, 4, 7, 0 },
	{ 10, -32, 128, 0, 0, 13, 0 },
	{ 11, 0, 128, 0, 0, 8, 1 },
	{ 12, -32, 160, 0, 0, 7, 0 },
	{ 13, 0, 160, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks095[] = {
	{ 0, 0, 0, 0, 64, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks096[] = {
	{ 0, 0, 0, 0, 64, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks097[] = {
	{ 0, 0, 0, 0, 64, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks098[] = {
	{ 0, 0, 0, 0, 64, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks099[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks100[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks101[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks102[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, 0, -32, 0, 0, 8, 1 },
	{ 2, -32, 0, 0, 0, 7, 0 },
	{ 3, -32, -32, 0, 0, 13, 0 },
	{ 4, -32, -64, 0, 12, 8, 1 },
	{ 5, -64, -32, 0, 12, 7, 0 },
	{ 6, -64, -64, 0, 12, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks103[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 0, 11, 0 },
	{ 3, -32, 32, 0, 0, 14, 0 },
	{ 4, -32, 64, 0, 12, 4, 1 },
	{ 5, -64, 32, 0, 12, 11, 0 },
	{ 6, -64, 64, 0, 12, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks104[] = {
	{ 0, 0, 0, 0, 12, 15, 0 },
	{ 1, 0, -32, 0, 12, 8, 1 },
	{ 2, -32, 0, 0, 12, 7, 0 },
	{ 3, -32, -32, 0, 0, 13, 0 },
	{ 4, -32, -64, 0, 0, 8, 1 },
	{ 5, -64, -32, 0, 0, 7, 0 },
	{ 6, -64, -64, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks105[] = {
	{ 0, 0, 0, 0, 12, 15, 0 },
	{ 1, 0, 32, 0, 12, 4, 1 },
	{ 2, -32, 0, 0, 12, 11, 0 },
	{ 3, -32, 32, 0, 0, 14, 0 },
	{ 4, -32, 64, 0, 0, 4, 1 },
	{ 5, -64, 32, 0, 0, 11, 0 },
	{ 6, -64, 64, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks106[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, 0, -32, 0, 0, 8, 1 },
	{ 2, -32, 0, 0, 0, 7, 0 },
	{ 3, -32, -32, 0, 0, 13, 0 },
	{ 4, -32, -64, 0, 12, 8, 1 },
	{ 5, -64, -32, 0, 12, 7, 0 },
	{ 6, -64, -64, 0, 12, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks107[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 0, 11, 0 },
	{ 3, -32, 32, 0, 0, 14, 0 },
	{ 4, -32, 64, 0, 12, 4, 1 },
	{ 5, -64, 32, 0, 12, 11, 0 },
	{ 6, -64, 64, 0, 12, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks108[] = {
	{ 0, 0, 0, 0, 12, 15, 0 },
	{ 1, 0, -32, 0, 12, 8, 1 },
	{ 2, -32, 0, 0, 12, 7, 0 },
	{ 3, -32, -32, 0, 0, 13, 0 },
	{ 4, -32, -64, 0, 0, 8, 1 },
	{ 5, -64, -32, 0, 0, 7, 0 },
	{ 6, -64, -64, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks109[] = {
	{ 0, 0, 0, 0, 12, 15, 0 },
	{ 1, 0, 32, 0, 12, 4, 1 },
	{ 2, -32, 0, 0, 12, 11, 0 },
	{ 3, -32, 32, 0, 0, 14, 0 },
	{ 4, -32, 64, 0, 0, 4, 1 },
	{ 5, -64, 32, 0, 0, 11, 0 },
	{ 6, -64, 64, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks110[] = {
	{ 0, 0, 0, 0, 16, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks111[] = {
	{ 0, 0, 0, 0, 16, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks112[] = {
	{ 0, 0, 0, 0, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks113[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks114[] = {
	{ 0, 0, 0, 0, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks115[] = {
	{ 0, 0, 0, 0, 16, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks116[] = {
	{ 0, 0, 0, 0, 16, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks117[] = {
	{ 0, 0, 0, 0, 16, 15, 0 },
	{ 1, -32, 0, 0, 16, 15, 0 },
	{ 2, -64, 0, 0, 16, 15, 0 },
	{ 3, -96, 0, 0, 16, 15, 0 },
	{ 4, -128, 0, 0, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks118[] = {
	{ 0, 0, 0, 0, 8, 15, 0 },
	{ 1, -32, 0, 0, 16, 15, 0 },
	{ 2, -64, 0, 16, 24, 15, 0 },
	{ 3, -96, 0, 40, 48, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks119[] = {
	{ 0, 0, 0, 0, 48, 15, 0 },
	{ 1, -32, 0, 40, 48, 15, 0 },
	{ 2, -64, 0, 64, 24, 15, 0 },
	{ 3, -96, 0, 80, 8, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks120[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks121[] = {
	{ 0, 0, 0, 40, 48, 15, 0 },
	{ 1, -32, 0, 16, 24, 15, 0 },
	{ 2, -64, 0, 0, 16, 15, 0 },
	{ 3, -96, 0, 0, 8, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks122[] = {
	{ 0, 0, 0, 80, 8, 15, 0 },
	{ 1, -32, 0, 64, 24, 15, 0 },
	{ 2, -64, 0, 40, 48, 15, 0 },
	{ 3, -96, 0, 0, 48, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks123[] = {
	{ 0, 0, 0, 0, 8, 207, 0 },
	{ 1, -32, 0, 0, 8, 63, 0 },
	{ 2, -64, 0, -32, 32, 63, 0 },
	{ 3, -96, 0, -96, 64, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks124[] = {
	{ 0, 0, 0, 0, 16, 15, 0 },
	{ 1, -32, 0, 0, 32, 15, 0 },
	{ 2, -64, 0, 0, 48, 15, 0 },
	{ 3, -96, 0, 0, 80, 15, 0 },
	{ 4, -128, 0, 0, 160, 15, 0 },
	{ 5, -192, 0, 0, 208, 15, 0 },
	{ 6, -160, 0, 0, 208, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks125[] = {
	{ 0, 0, 0, 0, 48, 15, 0 },
	{ 1, 32, 0, 0, 48, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks126[] = {
	{ 0, 0, 0, 0, 8, 15, 4 },
	{ 1, 32, 0, 0, 0, 0, 5 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks127[] = {
	{ 0, 0, 0, 0, 8, 15, 4 },
	{ 1, 32, 0, 0, 0, 0, 5 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks128[] = {
	{ 0, 0, 0, 0, 32, 207, 4 },
	{ 1, 32, 0, 0, 0, 0, 5 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks129[] = {
	{ 0, 0, 0, 0, 32, 63, 4 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks130[] = {
	{ 0, 0, 0, 0, 56, 15, 4 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks131[] = {
	{ 0, 0, 0, 0, 56, 15, 4 },
	{ 1, 32, 0, 0, 0, 0, 5 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks132[] = {
	{ 0, 0, 0, 0, 24, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks133[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 0, 15, 0 },
	{ 2, -32, -32, 0, 0, 8, 0 },
	{ 3, -64, 0, 0, 0, 2, 1 },
	{ 4, -64, -32, 0, 0, 1, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks134[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 0, 15, 0 },
	{ 2, -32, 32, 0, 0, 4, 0 },
	{ 3, -64, 0, 0, 0, 1, 1 },
	{ 4, -64, 32, 0, 0, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks135[] = {
	{ 0, 0, 0, 0, 0, 13, 0 },
	{ 1, -32, 0, 0, 0, 1, 0 },
	{ 2, 0, 32, 0, 0, 4, 1 },
	{ 3, -32, 32, 0, 0, 15, 0 },
	{ 4, -64, 32, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks136[] = {
	{ 0, 0, 0, 0, 0, 13, 0 },
	{ 1, 0, 32, 0, 0, 4, 0 },
	{ 2, -32, 0, 0, 0, 1, 1 },
	{ 3, -32, 32, 0, 0, 15, 0 },
	{ 4, -32, 64, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks137[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 0, 15, 0 },
	{ 2, -32, -32, 0, 0, 8, 0 },
	{ 3, -64, 0, 0, 0, 2, 1 },
	{ 4, -64, -32, 0, 0, 1, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks138[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 0, 15, 0 },
	{ 2, -32, 32, 0, 0, 4, 0 },
	{ 3, -64, 0, 0, 0, 1, 1 },
	{ 4, -64, 32, 0, 0, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks139[] = {
	{ 0, 0, 0, 0, 0, 13, 0 },
	{ 1, -32, 0, 0, 0, 1, 0 },
	{ 2, 0, 32, 0, 0, 4, 1 },
	{ 3, -32, 32, 0, 0, 15, 0 },
	{ 4, -64, 32, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks140[] = {
	{ 0, 0, 0, 0, 0, 13, 0 },
	{ 1, 0, 32, 0, 0, 4, 0 },
	{ 2, -32, 0, 0, 0, 1, 1 },
	{ 3, -32, 32, 0, 0, 15, 0 },
	{ 4, -32, 64, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks141[] = {
	{ 0, 0, 0, 0, 0, 13, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 0, 1, 1 },
	{ 3, -32, 32, 0, 0, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks142[] = {
	{ 0, 0, 0, 0, 16, 13, 0 },
	{ 1, 0, 32, 0, 16, 4, 1 },
	{ 2, -32, 0, 0, 16, 1, 1 },
	{ 3, -32, 32, 0, 16, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks143[] = {
	{ 0, 0, 0, 0, 64, 13, 0 },
	{ 1, 0, 32, 0, 64, 4, 1 },
	{ 2, -32, 0, 0, 64, 1, 1 },
	{ 3, -32, 32, 0, 64, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks144[] = {
	{ 0, 0, 0, 0, 8, 13, 0 },
	{ 1, 0, 32, 0, 8, 4, 1 },
	{ 2, -32, 0, 0, 8, 1, 1 },
	{ 3, -32, 32, 0, 8, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks145[] = {
	{ 0, 0, 0, 0, 32, 13, 0 },
	{ 1, 0, 32, 0, 32, 4, 1 },
	{ 2, -32, 0, 0, 32, 1, 1 },
	{ 3, -32, 32, 0, 32, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks146[] = {
	{ 0, 0, 0, 0, 32, 13, 0 },
	{ 1, 0, 32, 0, 32, 4, 1 },
	{ 2, -32, 0, 0, 32, 1, 1 },
	{ 3, -32, 32, 0, 32, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks147[] = {
	{ 0, 0, 0, 0, 8, 13, 0 },
	{ 1, 0, 32, 0, 8, 4, 1 },
	{ 2, -32, 0, 0, 8, 1, 1 },
	{ 3, -32, 32, 0, 8, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks148[] = {
	{ 0, 0, 0, 0, 16, 13, 0 },
	{ 1, 0, 32, 0, 16, 4, 1 },
	{ 2, -32, 0, 0, 16, 1, 1 },
	{ 3, -32, 32, 0, 16, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks149[] = {
	{ 0, 0, 0, 0, 64, 13, 0 },
	{ 1, 0, 32, 0, 64, 4, 1 },
	{ 2, -32, 0, 0, 64, 1, 1 },
	{ 3, -32, 32, 0, 64, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks150[] = {
	{ 0, 0, 0, 0, 8, 13, 0 },
	{ 1, 0, 32, 0, 8, 4, 1 },
	{ 2, -32, 0, 0, 8, 1, 1 },
	{ 3, -32, 32, 0, 8, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks151[] = {
	{ 0, 0, 0, 0, 32, 13, 0 },
	{ 1, 0, 32, 0, 32, 4, 1 },
	{ 2, -32, 0, 0, 32, 1, 1 },
	{ 3, -32, 32, 0, 32, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks152[] = {
	{ 0, 0, 0, 0, 32, 13, 0 },
	{ 1, 0, 32, 0, 32, 4, 1 },
	{ 2, -32, 0, 0, 32, 1, 1 },
	{ 3, -32, 32, 0, 32, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks153[] = {
	{ 0, 0, 0, 0, 8, 13, 0 },
	{ 1, 0, 32, 0, 8, 4, 1 },
	{ 2, -32, 0, 0, 8, 1, 1 },
	{ 3, -32, 32, 0, 8, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks154[] = {
	{ 0, 0, 0, 0, 24, 13, 0 },
	{ 1, 0, 32, 0, 24, 4, 1 },
	{ 2, -32, 0, 0, 24, 1, 1 },
	{ 3, -32, 32, 0, 24, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks155[] = {
	{ 0, 0, 0, 0, 24, 13, 0 },
	{ 1, 0, 32, 0, 24, 4, 1 },
	{ 2, -32, 0, 0, 24, 1, 1 },
	{ 3, -32, 32, 0, 24, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks156[] = {
	{ 0, 0, 0, 0, 24, 13, 0 },
	{ 1, 0, 32, 0, 24, 4, 1 },
	{ 2, -32, 0, 0, 24, 1, 1 },
	{ 3, -32, 32, 0, 24, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks157[] = {
	{ 0, 0, 0, 0, 24, 13, 0 },
	{ 1, 0, 32, 0, 24, 4, 1 },
	{ 2, -32, 0, 0, 24, 1, 1 },
	{ 3, -32, 32, 0, 24, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks158[] = {
	{ 0, 0, 0, 0, 0, 13, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 0, 1, 1 },
	{ 3, -32, 32, 0, 0, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks159[] = {
	{ 0, 0, 0, 0, 0, 13, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 0, 1, 1 },
	{ 3, -32, 32, 0, 0, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks160[] = {
	{ 0, 0, 0, 0, 0, 13, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 0, 1, 1 },
	{ 3, -32, 32, 0, 0, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks161[] = {
	{ 0, 0, 0, 0, 0, 13, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 0, 1, 1 },
	{ 3, -32, 32, 0, 0, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks162[] = {
	{ 0, 0, 0, 0, 8, 13, 0 },
	{ 1, 0, 32, 0, 8, 4, 1 },
	{ 2, -32, 0, 0, 8, 1, 1 },
	{ 3, -32, 32, 0, 8, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks163[] = {
	{ 0, 0, 0, 0, 8, 13, 0 },
	{ 1, 0, 32, 0, 8, 4, 1 },
	{ 2, -32, 0, 0, 8, 1, 1 },
	{ 3, -32, 32, 0, 8, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks164[] = {
	{ 0, 0, 0, 0, 8, 13, 0 },
	{ 1, 0, 32, 0, 8, 4, 1 },
	{ 2, -32, 0, 0, 8, 1, 1 },
	{ 3, -32, 32, 0, 8, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks165[] = {
	{ 0, 0, 0, 0, 8, 13, 0 },
	{ 1, 0, 32, 0, 8, 4, 1 },
	{ 2, -32, 0, 0, 8, 1, 1 },
	{ 3, -32, 32, 0, 8, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks166[] = {
	{ 0, 0, 0, 0, 8, 13, 0 },
	{ 1, 0, 32, 0, 8, 4, 1 },
	{ 2, -32, 0, 0, 8, 1, 1 },
	{ 3, -32, 32, 0, 8, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks167[] = {
	{ 0, 0, 0, 0, 8, 13, 0 },
	{ 1, 0, 32, 0, 8, 4, 1 },
	{ 2, -32, 0, 0, 8, 1, 1 },
	{ 3, -32, 32, 0, 8, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks168[] = {
	{ 0, 0, 0, 0, 8, 13, 0 },
	{ 1, 0, 32, 0, 8, 4, 1 },
	{ 2, -32, 0, 0, 8, 1, 1 },
	{ 3, -32, 32, 0, 8, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks169[] = {
	{ 0, 0, 0, 0, 8, 13, 0 },
	{ 1, 0, 32, 0, 8, 4, 1 },
	{ 2, -32, 0, 0, 8, 1, 1 },
	{ 3, -32, 32, 0, 8, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks170[] = {
	{ 0, 0, 0, 0, 0, 13, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 0, 1, 1 },
	{ 3, -32, 32, 0, 0, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks171[] = {
	{ 0, 0, 0, 0, 0, 13, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 0, 1, 1 },
	{ 3, -32, 32, 0, 0, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks172[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks173[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks174[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 16, 15, 0 },
	{ 2, -64, 0, 0, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks175[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 16, 15, 0 },
	{ 2, -64, 0, 0, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks176[] = {
	{ 0, 0, 0, -32, 16, 15, 0 },
	{ 1, -32, 0, -32, 16, 15, 0 },
	{ 2, -64, 0, -32, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks177[] = {
	{ 0, 0, 0, -32, 16, 15, 0 },
	{ 1, -32, 0, -32, 16, 15, 0 },
	{ 2, -64, 0, -32, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks178[] = {
	{ 0, 0, 0, 0, 0, 71, 0 },
	{ 1, 0, -32, 0, 16, 8, 1 },
	{ 2, -32, 0, 0, 16, 2, 0 },
	{ 3, -32, -32, 16, 16, 103, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks179[] = {
	{ 0, 0, 0, 0, 16, 139, 0 },
	{ 1, 0, 32, 0, 16, 4, 1 },
	{ 2, -32, 0, 0, 16, 1, 0 },
	{ 3, -32, 32, 16, 16, 155, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks180[] = {
	{ 0, 0, 0, 16, 16, 55, 0 },
	{ 1, 0, -32, 0, 16, 8, 1 },
	{ 2, -32, 0, 0, 16, 2, 0 },
	{ 3, -32, -32, 0, 0, 23, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks181[] = {
	{ 0, 0, 0, 16, 16, 59, 0 },
	{ 1, 0, 32, 0, 16, 4, 1 },
	{ 2, -32, 0, 0, 16, 1, 0 },
	{ 3, -32, 32, 0, 0, 43, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks182[] = {
	{ 0, 0, 0, 0, 16, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks183[] = {
	{ 0, 0, 0, 0, 24, 207, 0 },
	{ 1, -32, 0, 16, 40, 207, 0 },
	{ 2, -64, 0, 32, 56, 15, 0 },
	{ 3, -96, 0, 64, 192, 15, 0 },
	{ 4, -128, -32, 120, 96, 15, 0 },
	{ 5, -96, -32, 64, 192, 15, 0 },
	{ 6, -64, -32, 248, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks184[] = {
	{ 0, 0, 0, 0, 24, 207, 0 },
	{ 1, -32, 0, 16, 40, 207, 0 },
	{ 2, -64, 0, 32, 56, 15, 0 },
	{ 3, -96, 0, 64, 192, 15, 0 },
	{ 4, -128, 32, 120, 96, 15, 0 },
	{ 5, -96, 32, 64, 192, 15, 0 },
	{ 6, -64, 32, 248, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks185[] = {
	{ 0, 0, 0, -32, 24, 15, 0 },
	{ 1, -32, 0, -216, 192, 15, 0 },
	{ 2, -64, 0, -160, 96, 15, 0 },
	{ 3, -32, -32, -216, 192, 15, 0 },
	{ 4, 0, -32, -248, 56, 15, 0 },
	{ 5, 32, -32, -264, 40, 207, 0 },
	{ 6, 64, -32, -280, 24, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks186[] = {
	{ 0, 0, 0, -32, 24, 15, 0 },
	{ 1, -32, 0, -216, 192, 15, 0 },
	{ 2, -64, 0, -160, 96, 15, 0 },
	{ 3, -32, 32, -216, 192, 15, 0 },
	{ 4, 0, 32, -248, 56, 15, 0 },
	{ 5, 32, 32, -264, 40, 207, 0 },
	{ 6, 64, 32, -280, 24, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks187[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, -16, 16, 15, 0 },
	{ 2, -64, 0, -16, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks188[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, -16, 16, 15, 0 },
	{ 2, -64, 0, -16, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks189[] = {
	{ 0, 0, 0, 0, 16, 15, 0 },
	{ 1, -32, 0, 0, 16, 15, 0 },
	{ 2, -64, 0, 16, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks190[] = {
	{ 0, 0, 0, 0, 16, 15, 0 },
	{ 1, -32, 0, 0, 16, 15, 0 },
	{ 2, -64, 0, 16, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks191[] = {
	{ 0, 0, 0, 0, 32, 207, 0 },
	{ 1, -32, 0, 16, 16, 207, 0 },
	{ 2, -64, 0, 32, 96, 3, 0 },
	{ 3, -32, 0, 120, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks192[] = {
	{ 0, 0, 0, 0, 32, 15, 0 },
	{ 1, -32, 0, -88, 96, 3, 0 },
	{ 2, 0, 0, -104, 16, 207, 0 },
	{ 3, 32, 0, -120, 16, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks193[] = {
	{ 0, 0, 0, 0, 16, 15, 0 },
	{ 1, -32, 0, 24, 32, 7, 0 },
	{ 2, -32, -32, 48, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks194[] = {
	{ 0, 0, 0, 0, 16, 15, 0 },
	{ 1, -32, 0, 24, 32, 11, 0 },
	{ 2, -32, 32, 48, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks195[] = {
	{ 0, 0, 0, 0, 16, 15, 0 },
	{ 1, -32, 0, -24, 32, 7, 0 },
	{ 2, -32, -32, -48, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks196[] = {
	{ 0, 0, 0, 0, 16, 15, 0 },
	{ 1, -32, 0, -24, 32, 11, 0 },
	{ 2, -32, 32, -48, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks197[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 32, 15, 0 },
	{ 2, -64, 0, 16, 0, 15, 0 },
	{ 3, 0, 0, 32, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks198[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, -32, 32, 15, 0 },
	{ 2, -64, 0, -16, 0, 15, 0 },
	{ 3, 0, 0, -32, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks199[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 0, 15, 0 },
	{ 2, -64, 0, 0, 0, 15, 0 },
	{ 3, -96, 0, 0, 0, 15, 0 },
	{ 4, -128, 0, 0, 0, 15, 0 },
	{ 5, -160, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks200[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 0, 15, 0 },
	{ 2, -64, 0, 0, 0, 15, 0 },
	{ 3, -96, 0, 0, 0, 15, 0 },
	{ 4, -128, 0, 0, 0, 15, 0 },
	{ 5, -160, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks201[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks202[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks203[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks204[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 0, 15, 0 },
	{ 2, -32, 32, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks205[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 0, 15, 0 },
	{ 2, -32, -32, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks206[] = {
	{ 0, 0, 0, 0, 16, 15, 4 },
	{ 1, -32, 0, -40, 32, 15, 4 },
	{ 2, -64, 0, -96, 56, 15, 4 },
	{ 3, -96, 0, -96, 0, 0, 5 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks207[] = {
	{ 0, 0, 0, 0, 56, 15, 4 },
	{ 1, 32, 0, 56, 32, 15, 4 },
	{ 2, 64, 0, 96, 16, 15, 4 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks208[] = {
	{ 0, 0, 0, -32, 16, 15, 4 },
	{ 1, -32, 0, -72, 32, 15, 4 },
	{ 2, -64, 0, -128, 56, 15, 4 },
	{ 3, -96, 0, -128, 0, 0, 5 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks209[] = {
	{ 0, 0, 0, 0, 0, 7, 0 },
	{ 1, 0, -32, 0, 0, 8, 1 },
	{ 2, -32, 0, 0, 8, 2, 0 },
	{ 3, -32, -32, 0, 8, 7, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks210[] = {
	{ 0, 0, 0, 0, 0, 11, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 8, 1, 0 },
	{ 3, -32, 32, 0, 8, 11, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks211[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, 0, -32, 0, 0, 15, 0 },
	{ 2, -32, -32, 0, 0, 15, 0 },
	{ 3, -64, -32, 0, 0, 15, 0 },
	{ 4, -32, 0, 0, 0, 15, 0 },
	{ 5, -64, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks212[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, 0, 32, 0, 0, 15, 0 },
	{ 2, -32, 32, 0, 0, 15, 0 },
	{ 3, -64, 32, 0, 0, 15, 0 },
	{ 4, -32, 0, 0, 0, 15, 0 },
	{ 5, -64, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks213[] = {
	{ 0, 0, 0, 0, 32, 15, 0 },
	{ 1, 32, 0, 0, 32, 15, 0 },
	{ 2, -64, 0, 0, 32, 15, 0 },
	{ 3, -32, 0, 0, 32, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks214[] = {
	{ 0, 0, 0, 0, 48, 15, 0 },
	{ 1, 32, 0, 0, 48, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks215[] = {
	{ 0, 0, 0, 0, 208, 15, 0 },
	{ 1, 32, 0, 0, 208, 15, 0 },
	{ 2, -32, 0, 0, 160, 15, 0 },
	{ 3, -64, 0, 0, 80, 15, 0 },
	{ 4, -96, 0, 0, 48, 15, 0 },
	{ 5, -128, 0, 0, 32, 15, 0 },
	{ 6, -160, 0, 0, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks216[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks217[] = {
	{ 0, 0, 0, 0, 16, 71, 0 },
	{ 1, 0, -32, 16, 0, 8, 1 },
	{ 2, -32, 0, 16, 0, 2, 0 },
	{ 3, -32, -32, 16, 16, 103, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks218[] = {
	{ 0, 0, 0, 0, 16, 139, 0 },
	{ 1, 0, 32, 16, 0, 4, 1 },
	{ 2, -32, 0, 16, 0, 1, 0 },
	{ 3, -32, 32, 16, 16, 155, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks219[] = {
	{ 0, 0, 0, 16, 16, 55, 0 },
	{ 1, 0, -32, 16, 0, 8, 1 },
	{ 2, -32, 0, 16, 0, 2, 0 },
	{ 3, -32, -32, 0, 16, 23, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks220[] = {
	{ 0, 0, 0, 16, 16, 59, 0 },
	{ 1, 0, 32, 16, 0, 4, 1 },
	{ 2, -32, 0, 16, 0, 1, 0 },
	{ 3, -32, 32, 0, 16, 43, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks221[] = {
	{ 0, 0, 0, 0, 16, 207, 0 },
	{ 1, 0, -32, 16, 0, 8, 1 },
	{ 2, -32, 0, 16, 16, 71, 0 },
	{ 3, -32, -32, 24, 16, 13, 0 },
	{ 4, -32, -64, 48, 0, 8, 1 },
	{ 5, -64, -32, 32, 16, 71, 0 },
	{ 6, -64, -64, 48, 16, 111, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks222[] = {
	{ 0, 0, 0, 0, 16, 207, 0 },
	{ 1, 0, 32, 16, 0, 4, 1 },
	{ 2, -32, 0, 16, 16, 139, 0 },
	{ 3, -32, 32, 24, 16, 14, 0 },
	{ 4, -32, 64, 48, 0, 4, 1 },
	{ 5, -64, 32, 32, 16, 139, 0 },
	{ 6, -64, 64, 48, 16, 159, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks223[] = {
	{ 0, 0, 0, 48, 16, 63, 0 },
	{ 1, 0, -32, 48, 0, 8, 1 },
	{ 2, -32, 0, 32, 16, 23, 0 },
	{ 3, -32, -32, 24, 16, 13, 0 },
	{ 4, -32, -64, 16, 0, 8, 1 },
	{ 5, -64, -32, 16, 16, 23, 0 },
	{ 6, -64, -64, 0, 16, 159, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks224[] = {
	{ 0, 0, 0, 48, 16, 63, 0 },
	{ 1, 0, 32, 48, 0, 4, 1 },
	{ 2, -32, 0, 32, 16, 43, 0 },
	{ 3, -32, 32, 24, 16, 14, 0 },
	{ 4, -32, 64, 16, 0, 4, 1 },
	{ 5, -64, 32, 16, 16, 43, 0 },
	{ 6, -64, 64, 0, 16, 111, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks225[] = {
	{ 0, 0, 0, 0, 16, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks226[] = {
	{ 0, 0, 0, 0, 16, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks227[] = {
	{ 0, 0, 0, 0, 16, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks228[] = {
	{ 0, 0, 0, 0, 16, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks229[] = {
	{ 0, 0, 0, 0, 16, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks230[] = {
	{ 0, 0, 0, 0, 16, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks231[] = {
	{ 0, 0, 0, 0, 16, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks232[] = {
	{ 0, 0, 0, 0, 16, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks233[] = {
	{ 0, 0, 0, 0, 8, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks234[] = {
	{ 0, 0, 0, 0, 8, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks235[] = {
	{ 0, 0, 0, 0, 8, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks236[] = {
	{ 0, 0, 0, 0, 8, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks237[] = {
	{ 0, 0, 0, 0, 8, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks238[] = {
	{ 0, 0, 0, 0, 8, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks239[] = {
	{ 0, 0, 0, 0, 8, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks240[] = {
	{ 0, 0, 0, 0, 8, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks241[] = {
	{ 0, 0, 0, 0, 8, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks242[] = {
	{ 0, 0, 0, 0, 8, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks243[] = {
	{ 0, 0, 0, 0, 8, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks244[] = {
	{ 0, 0, 0, 0, 8, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks245[] = {
	{ 0, 0, 0, 0, 8, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks246[] = {
	{ 0, 0, 0, 0, 8, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks247[] = {
	{ 0, 0, 0, 0, 8, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks248[] = {
	{ 0, 0, 0, 0, 8, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks249[] = {
	{ 0, 0, 0, 0, 72, 15, 4 },
	{ 1, 0, 32, 0, 0, 0, 5 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks250[] = {
	{ 0, 0, 0, 0, 72, 15, 4 },
	{ 1, 0, -32, 0, 0, 0, 5 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks251[] = {
	{ 0, 0, 0, 0, 72, 15, 4 },
	{ 1, 0, 32, 0, 0, 0, 5 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks252[] = {
	{ 0, 0, 0, 0, 72, 15, 4 },
	{ 1, 0, -32, 0, 0, 0, 5 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks253[] = {
	{ 0, 0, 0, 0, 56, 15, 4 },
	{ 1, 32, 0, 56, 32, 15, 4 },
	{ 2, 64, 0, 96, 16, 15, 4 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks254[] = {
	{ 0, 0, 0, -32, 16, 15, 4 },
	{ 1, -32, 0, -72, 32, 15, 4 },
	{ 2, -64, 0, -128, 56, 15, 4 },
	{ 3, -96, 0, -128, 0, 0, 5 },
	TRACK_BLOCK_END
};

static const rct_preview_track TrackBlocks255[] = {
	{ 0, 0, 0, 32, 56, 15, 4 },
	{ 1, 32, 0, 88, 32, 15, 4 },
	{ 2, 64, 0, 128, 16, 15, 4 },
	TRACK_BLOCK_END
};

// rct2: 0x00994638
const rct_preview_track *TrackBlocks[256] = {
	TrackBlocks000,
	TrackBlocks001,
	TrackBlocks002,
	TrackBlocks003,
	TrackBlocks004,
	TrackBlocks005,
	TrackBlocks006,
	TrackBlocks007,
	TrackBlocks008,
	TrackBlocks009,
	TrackBlocks010,
	TrackBlocks011,
	TrackBlocks012,
	TrackBlocks013,
	TrackBlocks014,
	TrackBlocks015,
	TrackBlocks016,
	TrackBlocks017,
	TrackBlocks018,
	TrackBlocks019,
	TrackBlocks020,
	TrackBlocks021,
	TrackBlocks022,
	TrackBlocks023,
	TrackBlocks024,
	TrackBlocks025,
	TrackBlocks026,
	TrackBlocks027,
	TrackBlocks028,
	TrackBlocks029,
	TrackBlocks030,
	TrackBlocks031,
	TrackBlocks032,
	TrackBlocks033,
	TrackBlocks034,
	TrackBlocks035,
	TrackBlocks036,
	TrackBlocks037,
	TrackBlocks038,
	TrackBlocks039,
	TrackBlocks040,
	TrackBlocks041,
	TrackBlocks042,
	TrackBlocks043,
	TrackBlocks044,
	TrackBlocks045,
	TrackBlocks046,
	TrackBlocks047,
	TrackBlocks048,
	TrackBlocks049,
	TrackBlocks050,
	TrackBlocks051,
	TrackBlocks052,
	TrackBlocks053,
	TrackBlocks054,
	TrackBlocks055,
	TrackBlocks056,
	TrackBlocks057,
	TrackBlocks058,
	TrackBlocks059,
	TrackBlocks060,
	TrackBlocks061,
	TrackBlocks062,
	TrackBlocks063,
	TrackBlocks064,
	TrackBlocks065,
	TrackBlocks066,
	TrackBlocks067,
	TrackBlocks068,
	TrackBlocks069,
	TrackBlocks070,
	TrackBlocks071,
	TrackBlocks072,
	TrackBlocks073,
	TrackBlocks074,
	TrackBlocks075,
	TrackBlocks076,
	TrackBlocks077,
	TrackBlocks078,
	TrackBlocks079,
	TrackBlocks080,
	TrackBlocks081,
	TrackBlocks082,
	TrackBlocks083,
	TrackBlocks084,
	TrackBlocks085,
	TrackBlocks086,
	TrackBlocks087,
	TrackBlocks088,
	TrackBlocks089,
	TrackBlocks090,
	TrackBlocks091,
	TrackBlocks092,
	TrackBlocks093,
	TrackBlocks094,
	TrackBlocks095,
	TrackBlocks096,
	TrackBlocks097,
	TrackBlocks098,
	TrackBlocks099,
	TrackBlocks100,
	TrackBlocks101,
	TrackBlocks102,
	TrackBlocks103,
	TrackBlocks104,
	TrackBlocks105,
	TrackBlocks106,
	TrackBlocks107,
	TrackBlocks108,
	TrackBlocks109,
	TrackBlocks110,
	TrackBlocks111,
	TrackBlocks112,
	TrackBlocks113,
	TrackBlocks114,
	TrackBlocks115,
	TrackBlocks116,
	TrackBlocks117,
	TrackBlocks118,
	TrackBlocks119,
	TrackBlocks120,
	TrackBlocks121,
	TrackBlocks122,
	TrackBlocks123,
	TrackBlocks124,
	TrackBlocks125,
	TrackBlocks126,
	TrackBlocks127,
	TrackBlocks128,
	TrackBlocks129,
	TrackBlocks130,
	TrackBlocks131,
	TrackBlocks132,
	TrackBlocks133,
	TrackBlocks134,
	TrackBlocks135,
	TrackBlocks136,
	TrackBlocks137,
	TrackBlocks138,
	TrackBlocks139,
	TrackBlocks140,
	TrackBlocks141,
	TrackBlocks142,
	TrackBlocks143,
	TrackBlocks144,
	TrackBlocks145,
	TrackBlocks146,
	TrackBlocks147,
	TrackBlocks148,
	TrackBlocks149,
	TrackBlocks150,
	TrackBlocks151,
	TrackBlocks152,
	TrackBlocks153,
	TrackBlocks154,
	TrackBlocks155,
	TrackBlocks156,
	TrackBlocks157,
	TrackBlocks158,
	TrackBlocks159,
	TrackBlocks160,
	TrackBlocks161,
	TrackBlocks162,
	TrackBlocks163,
	TrackBlocks164,
	TrackBlocks165,
	TrackBlocks166,
	TrackBlocks167,
	TrackBlocks168,
	TrackBlocks169,
	TrackBlocks170,
	TrackBlocks171,
	TrackBlocks172,
	TrackBlocks173,
	TrackBlocks174,
	TrackBlocks175,
	TrackBlocks176,
	TrackBlocks177,
	TrackBlocks178,
	TrackBlocks179,
	TrackBlocks180,
	TrackBlocks181,
	TrackBlocks182,
	TrackBlocks183,
	TrackBlocks184,
	TrackBlocks185,
	TrackBlocks186,
	TrackBlocks187,
	TrackBlocks188,
	TrackBlocks189,
	TrackBlocks190,
	TrackBlocks191,
	TrackBlocks192,
	TrackBlocks193,
	TrackBlocks194,
	TrackBlocks195,
	TrackBlocks196,
	TrackBlocks197,
	TrackBlocks198,
	TrackBlocks199,
	TrackBlocks200,
	TrackBlocks201,
	TrackBlocks202,
	TrackBlocks203,
	TrackBlocks204,
	TrackBlocks205,
	TrackBlocks206,
	TrackBlocks207,
	TrackBlocks208,
	TrackBlocks209,
	TrackBlocks210,
	TrackBlocks211,
	TrackBlocks212,
	TrackBlocks213,
	TrackBlocks214,
	TrackBlocks215,
	TrackBlocks216,
	TrackBlocks217,
	TrackBlocks218,
	TrackBlocks219,
	TrackBlocks220,
	TrackBlocks221,
	TrackBlocks222,
	TrackBlocks223,
	TrackBlocks224,
	TrackBlocks225,
	TrackBlocks226,
	TrackBlocks227,
	TrackBlocks228,
	TrackBlocks229,
	TrackBlocks230,
	TrackBlocks231,
	TrackBlocks232,
	TrackBlocks233,
	TrackBlocks234,
	TrackBlocks235,
	TrackBlocks236,
	TrackBlocks237,
	TrackBlocks238,
	TrackBlocks239,
	TrackBlocks240,
	TrackBlocks241,
	TrackBlocks242,
	TrackBlocks243,
	TrackBlocks244,
	TrackBlocks245,
	TrackBlocks246,
	TrackBlocks247,
	TrackBlocks248,
	TrackBlocks249,
	TrackBlocks250,
	TrackBlocks251,
	TrackBlocks252,
	TrackBlocks253,
	TrackBlocks254,
	TrackBlocks255
};

static const rct_preview_track FlatRideTrackBlocks000[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks001[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks002[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks003[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks004[] = {
	{ 0, 0, 0, 0, 16, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks005[] = {
	{ 0, 0, 0, 0, 64, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks006[] = {
	{ 0, 0, 0, 0, 8, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks007[] = {
	{ 0, 0, 0, 0, 32, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks008[] = {
	{ 0, 0, 0, 0, 32, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks009[] = {
	{ 0, 0, 0, 0, 8, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks010[] = {
	{ 0, 0, 0, 0, 16, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks011[] = {
	{ 0, 0, 0, 0, 64, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks012[] = {
	{ 0, 0, 0, 0, 8, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks013[] = {
	{ 0, 0, 0, 0, 32, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks014[] = {
	{ 0, 0, 0, 0, 32, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks015[] = {
	{ 0, 0, 0, 0, 8, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks016[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, 0, -32, 0, 0, 8, 1 },
	{ 2, -32, 0, 0, 0, 7, 0 },
	{ 3, -32, -32, 0, 0, 13, 0 },
	{ 4, -32, -64, 0, 0, 8, 1 },
	{ 5, -64, -32, 0, 0, 7, 0 },
	{ 6, -64, -64, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks017[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 0, 11, 0 },
	{ 3, -32, 32, 0, 0, 14, 0 },
	{ 4, -32, 64, 0, 0, 4, 1 },
	{ 5, -64, 32, 0, 0, 11, 0 },
	{ 6, -64, 64, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks018[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks019[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks020[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks021[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks022[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, 0, -32, 0, 0, 8, 1 },
	{ 2, -32, 0, 0, 0, 7, 0 },
	{ 3, -32, -32, 0, 0, 13, 0 },
	{ 4, -32, -64, 0, 0, 8, 1 },
	{ 5, -64, -32, 0, 0, 7, 0 },
	{ 6, -64, -64, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks023[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 0, 11, 0 },
	{ 3, -32, 32, 0, 0, 14, 0 },
	{ 4, -32, 64, 0, 0, 4, 1 },
	{ 5, -64, 32, 0, 0, 11, 0 },
	{ 6, -64, 64, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks024[] = {
	{ 0, 0, 0, 0, 8, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks025[] = {
	{ 0, 0, 0, 0, 8, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks026[] = {
	{ 0, 0, 0, 0, 8, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks027[] = {
	{ 0, 0, 0, 0, 8, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks028[] = {
	{ 0, 0, 0, 0, 8, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks029[] = {
	{ 0, 0, 0, 0, 8, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks030[] = {
	{ 0, 0, 0, 0, 8, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks031[] = {
	{ 0, 0, 0, 0, 8, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks032[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks033[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks034[] = {
	{ 0, 0, 0, 0, 16, 207, 0 },
	{ 1, 0, -32, 16, 0, 8, 1 },
	{ 2, -32, 0, 16, 16, 71, 0 },
	{ 3, -32, -32, 24, 16, 13, 0 },
	{ 4, -32, -64, 48, 0, 8, 1 },
	{ 5, -64, -32, 32, 16, 71, 0 },
	{ 6, -64, -64, 48, 16, 111, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks035[] = {
	{ 0, 0, 0, 0, 16, 207, 0 },
	{ 1, 0, 32, 16, 0, 4, 1 },
	{ 2, -32, 0, 16, 16, 139, 0 },
	{ 3, -32, 32, 24, 16, 14, 0 },
	{ 4, -32, 64, 48, 0, 4, 1 },
	{ 5, -64, 32, 32, 16, 139, 0 },
	{ 6, -64, 64, 48, 16, 159, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks036[] = {
	{ 0, 0, 0, 48, 16, 63, 0 },
	{ 1, 0, -32, 48, 0, 8, 1 },
	{ 2, -32, 0, 32, 16, 23, 0 },
	{ 3, -32, -32, 24, 16, 13, 0 },
	{ 4, -32, -64, 16, 0, 8, 1 },
	{ 5, -64, -32, 16, 16, 23, 0 },
	{ 6, -64, -64, 0, 16, 159, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks037[] = {
	{ 0, 0, 0, 48, 16, 63, 0 },
	{ 1, 0, 32, 48, 0, 4, 1 },
	{ 2, -32, 0, 32, 16, 43, 0 },
	{ 3, -32, 32, 24, 16, 14, 0 },
	{ 4, -32, 64, 16, 0, 4, 1 },
	{ 5, -64, 32, 16, 16, 43, 0 },
	{ 6, -64, 64, 0, 16, 111, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks038[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 0, 7, 0 },
	{ 2, -32, -32, 0, 0, 13, 0 },
	{ 3, -64, -32, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks039[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 0, 11, 0 },
	{ 2, -32, 32, 0, 0, 14, 0 },
	{ 3, -64, 32, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks040[] = {
	{ 0, 0, 0, 0, 16, 207, 0 },
	{ 1, -32, 0, 16, 16, 207, 0 },
	{ 2, -64, 0, 32, 96, 2, 0 },
	{ 3, -32, 0, 120, 16, 6, 0 },
	{ 4, -32, -32, 120, 0, 0, 0 },
	{ 5, 0, 0, 120, 0, 0, 0 },
	{ 6, 0, -32, 120, 16, 9, 0 },
	{ 7, 32, -32, 32, 96, 8, 0 },
	{ 8, 0, -32, 16, 16, 63, 0 },
	{ 9, -32, -32, 0, 16, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks041[] = {
	{ 0, 0, 0, 0, 16, 207, 0 },
	{ 1, -32, 0, 16, 16, 207, 0 },
	{ 2, -64, 0, 32, 96, 1, 0 },
	{ 3, -32, 0, 120, 16, 9, 0 },
	{ 4, -32, 32, 120, 0, 0, 0 },
	{ 5, 0, 0, 120, 0, 0, 0 },
	{ 6, 0, 32, 120, 16, 6, 0 },
	{ 7, 32, 32, 32, 96, 4, 0 },
	{ 8, 0, 32, 16, 16, 63, 0 },
	{ 9, -32, 32, 0, 16, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks042[] = {
	{ 0, 0, 0, 0, 0, 7, 0 },
	{ 1, 0, -32, 0, 0, 8, 1 },
	{ 2, -32, 0, 0, 0, 2, 0 },
	{ 3, -32, -32, 0, 0, 7, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks043[] = {
	{ 0, 0, 0, 0, 0, 11, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 0, 1, 0 },
	{ 3, -32, 32, 0, 0, 11, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks044[] = {
	{ 0, 0, 0, 0, 0, 7, 0 },
	{ 1, 0, -32, 0, 0, 8, 1 },
	{ 2, -32, 0, 0, 0, 2, 0 },
	{ 3, -32, -32, 0, 0, 7, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks045[] = {
	{ 0, 0, 0, 0, 0, 11, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 0, 1, 0 },
	{ 3, -32, 32, 0, 0, 11, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks046[] = {
	{ 0, 0, 0, 0, 16, 71, 0 },
	{ 1, 0, -32, 16, 0, 8, 1 },
	{ 2, -32, 0, 16, 0, 2, 0 },
	{ 3, -32, -32, 16, 16, 103, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks047[] = {
	{ 0, 0, 0, 0, 16, 139, 0 },
	{ 1, 0, 32, 16, 0, 4, 1 },
	{ 2, -32, 0, 16, 0, 1, 0 },
	{ 3, -32, 32, 16, 16, 155, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks048[] = {
	{ 0, 0, 0, 16, 16, 55, 0 },
	{ 1, 0, -32, 16, 0, 8, 1 },
	{ 2, -32, 0, 16, 0, 2, 0 },
	{ 3, -32, -32, 0, 16, 23, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks049[] = {
	{ 0, 0, 0, 16, 16, 59, 0 },
	{ 1, 0, 32, 16, 0, 4, 1 },
	{ 2, -32, 0, 16, 0, 1, 0 },
	{ 3, -32, 32, 0, 16, 43, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks050[] = {
	{ 0, 0, 0, 0, 0, 7, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks051[] = {
	{ 0, 0, 0, 0, 0, 11, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks052[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 16, 15, 0 },
	{ 2, -64, 0, 16, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks053[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 16, 15, 0 },
	{ 2, -64, 0, 16, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks054[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, -16, 16, 15, 0 },
	{ 2, -64, 0, -16, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks055[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, -16, 16, 15, 0 },
	{ 2, -64, 0, -16, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks056[] = {
	{ 0, 0, 0, 0, 32, 207, 0 },
	{ 1, -32, 0, 16, 16, 207, 0 },
	{ 2, -64, 0, 32, 96, 3, 0 },
	{ 3, -32, 0, 120, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks057[] = {
	{ 0, 0, 0, -32, 32, 15, 0 },
	{ 1, -32, 0, -120, 96, 3, 0 },
	{ 2, 0, 0, -136, 16, 207, 0 },
	{ 3, 32, 0, -152, 16, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks058[] = {
	{ 0, 0, 0, 0, 16, 15, 0 },
	{ 1, -32, 0, 24, 32, 7, 0 },
	{ 2, -32, -32, 48, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks059[] = {
	{ 0, 0, 0, 0, 16, 15, 0 },
	{ 1, -32, 0, 24, 32, 11, 0 },
	{ 2, -32, 32, 48, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks060[] = {
	{ 0, 0, 0, -32, 16, 15, 0 },
	{ 1, -32, 0, -56, 32, 7, 0 },
	{ 2, -32, -32, -80, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks061[] = {
	{ 0, 0, 0, -32, 16, 15, 0 },
	{ 1, -32, 0, -56, 32, 11, 0 },
	{ 2, -32, 32, -80, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks062[] = {
	{ 0, 0, 0, 0, 24, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks063[] = {
	{ 0, 0, 0, 0, 24, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks064[] = {
	{ 0, 0, 0, 0, 24, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks065[] = {
	{ 0, 0, 0, 0, 24, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks066[] = {
	{ 0, 0, 0, 0, 64, 15, 0 },
	{ 1, -32, -32, 0, 0, 15, 0 },
	{ 2, -32, 0, 0, 0, 15, 0 },
	{ 3, -32, 32, 0, 0, 15, 0 },
	{ 4, 0, -32, 0, 0, 15, 0 },
	{ 5, 0, 32, 0, 0, 15, 0 },
	{ 6, 32, -32, 0, 0, 15, 0 },
	{ 7, 32, 32, 0, 0, 15, 0 },
	{ 8, 32, 0, 0, 0, 15, 2 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks067[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, 32, 0, 0, 0, 0, 3 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks068[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks069[] = {
	{ 0, 0, 0, 0, 16, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks070[] = {
	{ 0, 0, 0, 0, 64, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks071[] = {
	{ 0, 0, 0, 0, 8, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks072[] = {
	{ 0, 0, 0, 0, 32, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks073[] = {
	{ 0, 0, 0, 0, 32, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks074[] = {
	{ 0, 0, 0, 0, 8, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks075[] = {
	{ 0, 0, 0, 0, 16, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks076[] = {
	{ 0, 0, 0, 0, 64, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks077[] = {
	{ 0, 0, 0, 0, 8, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks078[] = {
	{ 0, 0, 0, 0, 32, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks079[] = {
	{ 0, 0, 0, 0, 32, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks080[] = {
	{ 0, 0, 0, 0, 8, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks081[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, 0, -32, 0, 0, 8, 1 },
	{ 2, -32, 0, 0, 0, 7, 0 },
	{ 3, -32, -32, 0, 0, 13, 0 },
	{ 4, -32, -64, 0, 0, 8, 1 },
	{ 5, -64, -32, 0, 0, 7, 0 },
	{ 6, -64, -64, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks082[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 0, 11, 0 },
	{ 3, -32, 32, 0, 0, 14, 0 },
	{ 4, -32, 64, 0, 0, 4, 1 },
	{ 5, -64, 32, 0, 0, 11, 0 },
	{ 6, -64, 64, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks083[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 0, 7, 0 },
	{ 2, -32, -32, 0, 0, 13, 0 },
	{ 3, -64, -32, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks084[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 0, 11, 0 },
	{ 2, -32, 32, 0, 0, 14, 0 },
	{ 3, -64, 32, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks085[] = {
	{ 0, 0, 0, 0, 0, 7, 0 },
	{ 1, 0, -32, 0, 0, 8, 1 },
	{ 2, -32, 0, 0, 0, 2, 0 },
	{ 3, -32, -32, 0, 0, 7, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks086[] = {
	{ 0, 0, 0, 0, 0, 11, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 0, 1, 0 },
	{ 3, -32, 32, 0, 0, 11, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks087[] = {
	{ 0, 0, 0, 0, 0, 7, 0 },
	{ 1, 0, -32, 0, 0, 8, 1 },
	{ 2, -32, 0, 0, 4, 2, 0 },
	{ 3, -32, -32, 0, 4, 7, 0 },
	{ 4, -32, -64, 8, 0, 11, 0 },
	{ 5, 0, -64, 8, 0, 4, 1 },
	{ 6, -32, -96, 8, 4, 1, 0 },
	{ 7, 0, -96, 8, 4, 11, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks088[] = {
	{ 0, 0, 0, 0, 0, 11, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 4, 1, 0 },
	{ 3, -32, 32, 0, 4, 11, 0 },
	{ 4, -32, 64, 8, 0, 7, 0 },
	{ 5, 0, 64, 8, 0, 8, 1 },
	{ 6, -32, 96, 8, 4, 2, 0 },
	{ 7, 0, 96, 8, 4, 7, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks089[] = {
	{ 0, 0, 0, 8, 4, 7, 0 },
	{ 1, 0, -32, 8, 4, 8, 1 },
	{ 2, -32, 0, 8, 0, 2, 0 },
	{ 3, -32, -32, 8, 0, 7, 0 },
	{ 4, -32, -64, 0, 4, 11, 0 },
	{ 5, 0, -64, 0, 4, 4, 1 },
	{ 6, -32, -96, 0, 0, 1, 0 },
	{ 7, 0, -96, 0, 0, 11, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks090[] = {
	{ 0, 0, 0, 8, 4, 11, 0 },
	{ 1, 0, 32, 8, 4, 4, 1 },
	{ 2, -32, 0, 8, 0, 1, 0 },
	{ 3, -32, 32, 8, 0, 11, 0 },
	{ 4, -32, 64, 0, 4, 7, 0 },
	{ 5, 0, 64, 0, 4, 8, 1 },
	{ 6, -32, 96, 0, 0, 2, 0 },
	{ 7, 0, 96, 0, 0, 7, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks091[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, 0, -32, 0, 0, 8, 1 },
	{ 2, -32, 0, 0, 0, 7, 0 },
	{ 3, -32, -32, 0, 0, 13, 0 },
	{ 4, -32, -64, 0, 4, 8, 1 },
	{ 5, -64, -32, 0, 4, 7, 0 },
	{ 6, -64, -64, 0, 4, 15, 0 },
	{ 7, -64, -96, 8, 0, 15, 0 },
	{ 8, -32, -96, 8, 0, 4, 1 },
	{ 9, -64, -128, 8, 0, 11, 0 },
	{ 10, -32, -128, 8, 0, 14, 0 },
	{ 11, 0, -128, 8, 4, 4, 1 },
	{ 12, -32, -160, 8, 4, 11, 0 },
	{ 13, 0, -160, 8, 4, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks092[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 0, 11, 0 },
	{ 3, -32, 32, 0, 0, 14, 0 },
	{ 4, -32, 64, 0, 4, 4, 1 },
	{ 5, -64, 32, 0, 4, 11, 0 },
	{ 6, -64, 64, 0, 4, 15, 0 },
	{ 7, -64, 96, 8, 0, 15, 0 },
	{ 8, -32, 96, 8, 0, 8, 1 },
	{ 9, -64, 128, 8, 0, 7, 0 },
	{ 10, -32, 128, 8, 0, 13, 0 },
	{ 11, 0, 128, 8, 4, 8, 1 },
	{ 12, -32, 160, 8, 4, 7, 0 },
	{ 13, 0, 160, 8, 4, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks093[] = {
	{ 0, 0, 0, 8, 4, 15, 0 },
	{ 1, 0, -32, 8, 4, 8, 1 },
	{ 2, -32, 0, 8, 4, 7, 0 },
	{ 3, -32, -32, 8, 0, 13, 0 },
	{ 4, -32, -64, 8, 0, 8, 1 },
	{ 5, -64, -32, 8, 0, 7, 0 },
	{ 6, -64, -64, 8, 0, 15, 0 },
	{ 7, -64, -96, 0, 4, 15, 0 },
	{ 8, -32, -96, 0, 4, 4, 1 },
	{ 9, -64, -128, 0, 4, 11, 0 },
	{ 10, -32, -128, 0, 0, 14, 0 },
	{ 11, 0, -128, 0, 0, 4, 1 },
	{ 12, -32, -160, 0, 0, 11, 0 },
	{ 13, 0, -160, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks094[] = {
	{ 0, 0, 0, 8, 4, 15, 0 },
	{ 1, 0, 32, 8, 4, 4, 1 },
	{ 2, -32, 0, 8, 4, 11, 0 },
	{ 3, -32, 32, 8, 0, 14, 0 },
	{ 4, -32, 64, 8, 0, 4, 1 },
	{ 5, -64, 32, 8, 0, 11, 0 },
	{ 6, -64, 64, 8, 0, 15, 0 },
	{ 7, -64, 96, 0, 4, 15, 0 },
	{ 8, -32, 96, 0, 4, 8, 1 },
	{ 9, -64, 128, 0, 4, 7, 0 },
	{ 10, -32, 128, 0, 0, 13, 0 },
	{ 11, 0, 128, 0, 0, 8, 1 },
	{ 12, -32, 160, 0, 0, 7, 0 },
	{ 13, 0, 160, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks095[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -64, 0, 0, 0, 15, 0 },
	{ 2, -32, 0, 0, 0, 15, 0 },
	{ 3, 32, 0, 0, 0, 15, 2 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks096[] = {
	{ 0, 0, 0, 0, 64, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks097[] = {
	{ 0, 0, 0, 0, 64, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks098[] = {
	{ 0, 0, 0, 0, 64, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks099[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks100[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks101[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks102[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, 0, -32, 0, 0, 8, 1 },
	{ 2, -32, 0, 0, 0, 7, 0 },
	{ 3, -32, -32, 0, 0, 13, 0 },
	{ 4, -32, -64, 0, 12, 8, 1 },
	{ 5, -64, -32, 0, 12, 7, 0 },
	{ 6, -64, -64, 0, 12, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks103[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 0, 11, 0 },
	{ 3, -32, 32, 0, 0, 14, 0 },
	{ 4, -32, 64, 0, 12, 4, 1 },
	{ 5, -64, 32, 0, 12, 11, 0 },
	{ 6, -64, 64, 0, 12, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks104[] = {
	{ 0, 0, 0, 0, 12, 15, 0 },
	{ 1, 0, -32, 0, 12, 8, 1 },
	{ 2, -32, 0, 0, 12, 7, 0 },
	{ 3, -32, -32, 0, 0, 13, 0 },
	{ 4, -32, -64, 0, 0, 8, 1 },
	{ 5, -64, -32, 0, 0, 7, 0 },
	{ 6, -64, -64, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks105[] = {
	{ 0, 0, 0, 0, 12, 15, 0 },
	{ 1, 0, 32, 0, 12, 4, 1 },
	{ 2, -32, 0, 0, 12, 11, 0 },
	{ 3, -32, 32, 0, 0, 14, 0 },
	{ 4, -32, 64, 0, 0, 4, 1 },
	{ 5, -64, 32, 0, 0, 11, 0 },
	{ 6, -64, 64, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks106[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, 0, -32, 0, 0, 8, 1 },
	{ 2, -32, 0, 0, 0, 7, 0 },
	{ 3, -32, -32, 0, 0, 13, 0 },
	{ 4, -32, -64, 0, 12, 8, 1 },
	{ 5, -64, -32, 0, 12, 7, 0 },
	{ 6, -64, -64, 0, 12, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks107[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 0, 11, 0 },
	{ 3, -32, 32, 0, 0, 14, 0 },
	{ 4, -32, 64, 0, 12, 4, 1 },
	{ 5, -64, 32, 0, 12, 11, 0 },
	{ 6, -64, 64, 0, 12, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks108[] = {
	{ 0, 0, 0, 0, 12, 15, 0 },
	{ 1, 0, -32, 0, 12, 8, 1 },
	{ 2, -32, 0, 0, 12, 7, 0 },
	{ 3, -32, -32, 0, 0, 13, 0 },
	{ 4, -32, -64, 0, 0, 8, 1 },
	{ 5, -64, -32, 0, 0, 7, 0 },
	{ 6, -64, -64, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks109[] = {
	{ 0, 0, 0, 0, 12, 15, 0 },
	{ 1, 0, 32, 0, 12, 4, 1 },
	{ 2, -32, 0, 0, 12, 11, 0 },
	{ 3, -32, 32, 0, 0, 14, 0 },
	{ 4, -32, 64, 0, 0, 4, 1 },
	{ 5, -64, 32, 0, 0, 11, 0 },
	{ 6, -64, 64, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks110[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, 0, 32, 0, 0, 15, 0 },
	{ 2, 32, 0, 0, 0, 15, 0 },
	{ 3, 32, 32, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks111[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, 0, 32, 0, 0, 15, 0 },
	{ 2, 0, 64, 0, 0, 15, 0 },
	{ 3, 0, 96, 0, 0, 15, 0 },
	{ 4, 32, 0, 0, 0, 15, 0 },
	{ 5, 32, 32, 0, 0, 15, 0 },
	{ 6, 32, 64, 0, 0, 15, 0 },
	{ 7, 32, 96, 0, 0, 15, 0 },
	{ 8, 64, 0, 0, 0, 15, 0 },
	{ 9, 64, 32, 0, 0, 15, 0 },
	{ 10, 64, 64, 0, 0, 15, 0 },
	{ 11, 64, 96, 0, 0, 15, 0 },
	{ 12, 96, 0, 0, 0, 15, 0 },
	{ 13, 96, 32, 0, 0, 15, 0 },
	{ 14, 96, 64, 0, 0, 15, 0 },
	{ 15, 96, 96, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks112[] = {
	{ 0, 0, 0, 0, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks113[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks114[] = {
	{ 0, 0, 0, 0, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks115[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, 0, 32, 0, 0, 15, 0 },
	{ 2, 0, 64, 0, 0, 15, 0 },
	{ 3, 0, 96, 0, 0, 15, 0 },
	{ 4, 32, 0, 0, 0, 15, 0 },
	{ 5, 32, 32, 0, 0, 15, 0 },
	{ 6, 32, 64, 0, 0, 15, 0 },
	{ 7, 32, 96, 0, 0, 15, 2 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks116[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -64, 0, 0, 0, 15, 0 },
	{ 2, -32, 0, 0, 0, 15, 0 },
	{ 3, 32, 0, 0, 0, 15, 0 },
	{ 4, 64, 0, 0, 0, 15, 2 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks117[] = {
	{ 0, 0, 0, 0, 16, 15, 0 },
	{ 1, -32, 0, 0, 16, 15, 0 },
	{ 2, -64, 0, 0, 16, 15, 0 },
	{ 3, -96, 0, 0, 16, 15, 0 },
	{ 4, -128, 0, 0, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks118[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks119[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -64, 0, 0, 0, 15, 0 },
	{ 2, -32, 0, 0, 0, 15, 0 },
	{ 3, 32, 0, 0, 0, 15, 2 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks120[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks121[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks122[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -64, 0, 0, 0, 15, 0 },
	{ 2, -32, 0, 0, 0, 15, 0 },
	{ 3, 32, 0, 0, 0, 15, 2 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks123[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, -32, 0, 0, 15, 0 },
	{ 2, -32, 0, 0, 0, 15, 0 },
	{ 3, -32, 32, 0, 0, 15, 0 },
	{ 4, 0, -32, 0, 0, 15, 0 },
	{ 5, 0, 32, 0, 0, 15, 0 },
	{ 6, 32, -32, 0, 0, 15, 0 },
	{ 7, 32, 32, 0, 0, 15, 0 },
	{ 8, 32, 0, 0, 0, 15, 2 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks124[] = {
	{ 0, 0, 0, 0, 16, 15, 0 },
	{ 1, -32, 0, 0, 32, 15, 0 },
	{ 2, -64, 0, 0, 48, 15, 0 },
	{ 3, -96, 0, 0, 80, 15, 0 },
	{ 4, -128, 0, 0, 160, 15, 0 },
	{ 5, -192, 0, 0, 208, 15, 0 },
	{ 6, -160, 0, 0, 208, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks125[] = {
	{ 0, 0, 0, 0, 48, 15, 0 },
	{ 1, 32, 0, 0, 48, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks126[] = {
	{ 0, 0, 0, 0, 8, 15, 4 },
	{ 1, 32, 0, 0, 0, 0, 5 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks127[] = {
	{ 0, 0, 0, 0, 8, 15, 4 },
	{ 1, 32, 0, 0, 0, 0, 5 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks128[] = {
	{ 0, 0, 0, 0, 32, 207, 4 },
	{ 1, 32, 0, 0, 0, 0, 5 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks129[] = {
	{ 0, 0, 0, 0, 32, 63, 4 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks130[] = {
	{ 0, 0, 0, 0, 56, 15, 4 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks131[] = {
	{ 0, 0, 0, 0, 56, 15, 4 },
	{ 1, 32, 0, 0, 0, 0, 5 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks132[] = {
	{ 0, 0, 0, 0, 24, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks133[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 0, 15, 0 },
	{ 2, -32, -32, 0, 0, 8, 0 },
	{ 3, -64, 0, 0, 0, 2, 1 },
	{ 4, -64, -32, 0, 0, 1, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks134[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 0, 15, 0 },
	{ 2, -32, 32, 0, 0, 4, 0 },
	{ 3, -64, 0, 0, 0, 1, 1 },
	{ 4, -64, 32, 0, 0, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks135[] = {
	{ 0, 0, 0, 0, 0, 13, 0 },
	{ 1, -32, 0, 0, 0, 1, 0 },
	{ 2, 0, 32, 0, 0, 4, 1 },
	{ 3, -32, 32, 0, 0, 15, 0 },
	{ 4, -64, 32, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks136[] = {
	{ 0, 0, 0, 0, 0, 13, 0 },
	{ 1, 0, 32, 0, 0, 4, 0 },
	{ 2, -32, 0, 0, 0, 1, 1 },
	{ 3, -32, 32, 0, 0, 15, 0 },
	{ 4, -32, 64, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks137[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 0, 15, 0 },
	{ 2, -32, -32, 0, 0, 8, 0 },
	{ 3, -64, 0, 0, 0, 2, 1 },
	{ 4, -64, -32, 0, 0, 1, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks138[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 0, 15, 0 },
	{ 2, -32, 32, 0, 0, 4, 0 },
	{ 3, -64, 0, 0, 0, 1, 1 },
	{ 4, -64, 32, 0, 0, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks139[] = {
	{ 0, 0, 0, 0, 0, 13, 0 },
	{ 1, -32, 0, 0, 0, 1, 0 },
	{ 2, 0, 32, 0, 0, 4, 1 },
	{ 3, -32, 32, 0, 0, 15, 0 },
	{ 4, -64, 32, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks140[] = {
	{ 0, 0, 0, 0, 0, 13, 0 },
	{ 1, 0, 32, 0, 0, 4, 0 },
	{ 2, -32, 0, 0, 0, 1, 1 },
	{ 3, -32, 32, 0, 0, 15, 0 },
	{ 4, -32, 64, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks141[] = {
	{ 0, 0, 0, 0, 0, 13, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 0, 1, 1 },
	{ 3, -32, 32, 0, 0, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks142[] = {
	{ 0, 0, 0, 0, 16, 13, 0 },
	{ 1, 0, 32, 0, 16, 4, 1 },
	{ 2, -32, 0, 0, 16, 1, 1 },
	{ 3, -32, 32, 0, 16, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks143[] = {
	{ 0, 0, 0, 0, 64, 13, 0 },
	{ 1, 0, 32, 0, 64, 4, 1 },
	{ 2, -32, 0, 0, 64, 1, 1 },
	{ 3, -32, 32, 0, 64, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks144[] = {
	{ 0, 0, 0, 0, 8, 13, 0 },
	{ 1, 0, 32, 0, 8, 4, 1 },
	{ 2, -32, 0, 0, 8, 1, 1 },
	{ 3, -32, 32, 0, 8, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks145[] = {
	{ 0, 0, 0, 0, 32, 13, 0 },
	{ 1, 0, 32, 0, 32, 4, 1 },
	{ 2, -32, 0, 0, 32, 1, 1 },
	{ 3, -32, 32, 0, 32, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks146[] = {
	{ 0, 0, 0, 0, 32, 13, 0 },
	{ 1, 0, 32, 0, 32, 4, 1 },
	{ 2, -32, 0, 0, 32, 1, 1 },
	{ 3, -32, 32, 0, 32, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks147[] = {
	{ 0, 0, 0, 0, 8, 13, 0 },
	{ 1, 0, 32, 0, 8, 4, 1 },
	{ 2, -32, 0, 0, 8, 1, 1 },
	{ 3, -32, 32, 0, 8, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks148[] = {
	{ 0, 0, 0, 0, 16, 13, 0 },
	{ 1, 0, 32, 0, 16, 4, 1 },
	{ 2, -32, 0, 0, 16, 1, 1 },
	{ 3, -32, 32, 0, 16, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks149[] = {
	{ 0, 0, 0, 0, 64, 13, 0 },
	{ 1, 0, 32, 0, 64, 4, 1 },
	{ 2, -32, 0, 0, 64, 1, 1 },
	{ 3, -32, 32, 0, 64, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks150[] = {
	{ 0, 0, 0, 0, 8, 13, 0 },
	{ 1, 0, 32, 0, 8, 4, 1 },
	{ 2, -32, 0, 0, 8, 1, 1 },
	{ 3, -32, 32, 0, 8, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks151[] = {
	{ 0, 0, 0, 0, 32, 13, 0 },
	{ 1, 0, 32, 0, 32, 4, 1 },
	{ 2, -32, 0, 0, 32, 1, 1 },
	{ 3, -32, 32, 0, 32, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks152[] = {
	{ 0, 0, 0, 0, 32, 13, 0 },
	{ 1, 0, 32, 0, 32, 4, 1 },
	{ 2, -32, 0, 0, 32, 1, 1 },
	{ 3, -32, 32, 0, 32, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks153[] = {
	{ 0, 0, 0, 0, 8, 13, 0 },
	{ 1, 0, 32, 0, 8, 4, 1 },
	{ 2, -32, 0, 0, 8, 1, 1 },
	{ 3, -32, 32, 0, 8, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks154[] = {
	{ 0, 0, 0, 0, 24, 13, 0 },
	{ 1, 0, 32, 0, 24, 4, 1 },
	{ 2, -32, 0, 0, 24, 1, 1 },
	{ 3, -32, 32, 0, 24, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks155[] = {
	{ 0, 0, 0, 0, 24, 13, 0 },
	{ 1, 0, 32, 0, 24, 4, 1 },
	{ 2, -32, 0, 0, 24, 1, 1 },
	{ 3, -32, 32, 0, 24, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks156[] = {
	{ 0, 0, 0, 0, 24, 13, 0 },
	{ 1, 0, 32, 0, 24, 4, 1 },
	{ 2, -32, 0, 0, 24, 1, 1 },
	{ 3, -32, 32, 0, 24, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks157[] = {
	{ 0, 0, 0, 0, 24, 13, 0 },
	{ 1, 0, 32, 0, 24, 4, 1 },
	{ 2, -32, 0, 0, 24, 1, 1 },
	{ 3, -32, 32, 0, 24, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks158[] = {
	{ 0, 0, 0, 0, 0, 13, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 0, 1, 1 },
	{ 3, -32, 32, 0, 0, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks159[] = {
	{ 0, 0, 0, 0, 0, 13, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 0, 1, 1 },
	{ 3, -32, 32, 0, 0, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks160[] = {
	{ 0, 0, 0, 0, 0, 13, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 0, 1, 1 },
	{ 3, -32, 32, 0, 0, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks161[] = {
	{ 0, 0, 0, 0, 0, 13, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 0, 1, 1 },
	{ 3, -32, 32, 0, 0, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks162[] = {
	{ 0, 0, 0, 0, 8, 13, 0 },
	{ 1, 0, 32, 0, 8, 4, 1 },
	{ 2, -32, 0, 0, 8, 1, 1 },
	{ 3, -32, 32, 0, 8, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks163[] = {
	{ 0, 0, 0, 0, 8, 13, 0 },
	{ 1, 0, 32, 0, 8, 4, 1 },
	{ 2, -32, 0, 0, 8, 1, 1 },
	{ 3, -32, 32, 0, 8, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks164[] = {
	{ 0, 0, 0, 0, 8, 13, 0 },
	{ 1, 0, 32, 0, 8, 4, 1 },
	{ 2, -32, 0, 0, 8, 1, 1 },
	{ 3, -32, 32, 0, 8, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks165[] = {
	{ 0, 0, 0, 0, 8, 13, 0 },
	{ 1, 0, 32, 0, 8, 4, 1 },
	{ 2, -32, 0, 0, 8, 1, 1 },
	{ 3, -32, 32, 0, 8, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks166[] = {
	{ 0, 0, 0, 0, 8, 13, 0 },
	{ 1, 0, 32, 0, 8, 4, 1 },
	{ 2, -32, 0, 0, 8, 1, 1 },
	{ 3, -32, 32, 0, 8, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks167[] = {
	{ 0, 0, 0, 0, 8, 13, 0 },
	{ 1, 0, 32, 0, 8, 4, 1 },
	{ 2, -32, 0, 0, 8, 1, 1 },
	{ 3, -32, 32, 0, 8, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks168[] = {
	{ 0, 0, 0, 0, 8, 13, 0 },
	{ 1, 0, 32, 0, 8, 4, 1 },
	{ 2, -32, 0, 0, 8, 1, 1 },
	{ 3, -32, 32, 0, 8, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks169[] = {
	{ 0, 0, 0, 0, 8, 13, 0 },
	{ 1, 0, 32, 0, 8, 4, 1 },
	{ 2, -32, 0, 0, 8, 1, 1 },
	{ 3, -32, 32, 0, 8, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks170[] = {
	{ 0, 0, 0, 0, 0, 13, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 0, 1, 1 },
	{ 3, -32, 32, 0, 0, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks171[] = {
	{ 0, 0, 0, 0, 0, 13, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 0, 1, 1 },
	{ 3, -32, 32, 0, 0, 2, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks172[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks173[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks174[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 16, 15, 0 },
	{ 2, -64, 0, 0, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks175[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 16, 15, 0 },
	{ 2, -64, 0, 0, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks176[] = {
	{ 0, 0, 0, -32, 16, 15, 0 },
	{ 1, -32, 0, -32, 16, 15, 0 },
	{ 2, -64, 0, -32, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks177[] = {
	{ 0, 0, 0, -32, 16, 15, 0 },
	{ 1, -32, 0, -32, 16, 15, 0 },
	{ 2, -64, 0, -32, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks178[] = {
	{ 0, 0, 0, 0, 0, 71, 0 },
	{ 1, 0, -32, 0, 16, 8, 1 },
	{ 2, -32, 0, 0, 16, 2, 0 },
	{ 3, -32, -32, 16, 16, 103, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks179[] = {
	{ 0, 0, 0, 0, 16, 139, 0 },
	{ 1, 0, 32, 0, 16, 4, 1 },
	{ 2, -32, 0, 0, 16, 1, 0 },
	{ 3, -32, 32, 16, 16, 155, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks180[] = {
	{ 0, 0, 0, 16, 16, 55, 0 },
	{ 1, 0, -32, 0, 16, 8, 1 },
	{ 2, -32, 0, 0, 16, 2, 0 },
	{ 3, -32, -32, 0, 0, 23, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks181[] = {
	{ 0, 0, 0, 16, 16, 59, 0 },
	{ 1, 0, 32, 0, 16, 4, 1 },
	{ 2, -32, 0, 0, 16, 1, 0 },
	{ 3, -32, 32, 0, 0, 43, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks182[] = {
	{ 0, 0, 0, 0, 16, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks183[] = {
	{ 0, 0, 0, 0, 24, 207, 0 },
	{ 1, -32, 0, 16, 40, 207, 0 },
	{ 2, -64, 0, 32, 56, 15, 0 },
	{ 3, -96, 0, 64, 192, 15, 0 },
	{ 4, -128, -32, 120, 96, 15, 0 },
	{ 5, -96, -32, 64, 192, 15, 0 },
	{ 6, -64, -32, 248, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks184[] = {
	{ 0, 0, 0, 0, 24, 207, 0 },
	{ 1, -32, 0, 16, 40, 207, 0 },
	{ 2, -64, 0, 32, 56, 15, 0 },
	{ 3, -96, 0, 64, 192, 15, 0 },
	{ 4, -128, 32, 120, 96, 15, 0 },
	{ 5, -96, 32, 64, 192, 15, 0 },
	{ 6, -64, 32, 248, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks185[] = {
	{ 0, 0, 0, -32, 24, 15, 0 },
	{ 1, -32, 0, -216, 192, 15, 0 },
	{ 2, -64, 0, -160, 96, 15, 0 },
	{ 3, -32, -32, -216, 192, 15, 0 },
	{ 4, 0, -32, -248, 56, 15, 0 },
	{ 5, 32, -32, -264, 40, 207, 0 },
	{ 6, 64, -32, -280, 24, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks186[] = {
	{ 0, 0, 0, -32, 24, 15, 0 },
	{ 1, -32, 0, -216, 192, 15, 0 },
	{ 2, -64, 0, -160, 96, 15, 0 },
	{ 3, -32, 32, -216, 192, 15, 0 },
	{ 4, 0, 32, -248, 56, 15, 0 },
	{ 5, 32, 32, -264, 40, 207, 0 },
	{ 6, 64, 32, -280, 24, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks187[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, -16, 16, 15, 0 },
	{ 2, -64, 0, -16, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks188[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, -16, 16, 15, 0 },
	{ 2, -64, 0, -16, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks189[] = {
	{ 0, 0, 0, 0, 16, 15, 0 },
	{ 1, -32, 0, 0, 16, 15, 0 },
	{ 2, -64, 0, 16, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks190[] = {
	{ 0, 0, 0, 0, 16, 15, 0 },
	{ 1, -32, 0, 0, 16, 15, 0 },
	{ 2, -64, 0, 16, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks191[] = {
	{ 0, 0, 0, 0, 32, 207, 0 },
	{ 1, -32, 0, 16, 16, 207, 0 },
	{ 2, -64, 0, 32, 96, 3, 0 },
	{ 3, -32, 0, 120, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks192[] = {
	{ 0, 0, 0, 0, 32, 15, 0 },
	{ 1, -32, 0, -88, 96, 3, 0 },
	{ 2, 0, 0, -104, 16, 207, 0 },
	{ 3, 32, 0, -120, 16, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks193[] = {
	{ 0, 0, 0, 0, 16, 15, 0 },
	{ 1, -32, 0, 24, 32, 7, 0 },
	{ 2, -32, -32, 48, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks194[] = {
	{ 0, 0, 0, 0, 16, 15, 0 },
	{ 1, -32, 0, 24, 32, 11, 0 },
	{ 2, -32, 32, 48, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks195[] = {
	{ 0, 0, 0, 0, 16, 15, 0 },
	{ 1, -32, 0, -24, 32, 7, 0 },
	{ 2, -32, -32, -48, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks196[] = {
	{ 0, 0, 0, 0, 16, 15, 0 },
	{ 1, -32, 0, -24, 32, 11, 0 },
	{ 2, -32, 32, -48, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks197[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 32, 15, 0 },
	{ 2, -64, 0, 16, 0, 15, 0 },
	{ 3, 0, 0, 32, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks198[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, -32, 32, 15, 0 },
	{ 2, -64, 0, -16, 0, 15, 0 },
	{ 3, 0, 0, -32, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks199[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 0, 15, 0 },
	{ 2, -64, 0, 0, 0, 15, 0 },
	{ 3, -96, 0, 0, 0, 15, 0 },
	{ 4, -128, 0, 0, 0, 15, 0 },
	{ 5, -160, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks200[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 0, 15, 0 },
	{ 2, -64, 0, 0, 0, 15, 0 },
	{ 3, -96, 0, 0, 0, 15, 0 },
	{ 4, -128, 0, 0, 0, 15, 0 },
	{ 5, -160, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks201[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks202[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks203[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks204[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 0, 15, 0 },
	{ 2, -32, 32, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks205[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, -32, 0, 0, 0, 15, 0 },
	{ 2, -32, -32, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks206[] = {
	{ 0, 0, 0, 0, 16, 15, 4 },
	{ 1, -32, 0, -40, 32, 15, 4 },
	{ 2, -64, 0, -96, 56, 15, 4 },
	{ 3, -96, 0, -96, 0, 0, 5 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks207[] = {
	{ 0, 0, 0, 0, 56, 15, 4 },
	{ 1, 32, 0, 56, 32, 15, 4 },
	{ 2, 64, 0, 96, 16, 15, 4 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks208[] = {
	{ 0, 0, 0, -32, 16, 15, 4 },
	{ 1, -32, 0, -72, 32, 15, 4 },
	{ 2, -64, 0, -128, 56, 15, 4 },
	{ 3, -96, 0, -128, 0, 0, 5 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks209[] = {
	{ 0, 0, 0, 0, 0, 7, 0 },
	{ 1, 0, -32, 0, 0, 8, 1 },
	{ 2, -32, 0, 0, 8, 2, 0 },
	{ 3, -32, -32, 0, 8, 7, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks210[] = {
	{ 0, 0, 0, 0, 0, 11, 0 },
	{ 1, 0, 32, 0, 0, 4, 1 },
	{ 2, -32, 0, 0, 8, 1, 0 },
	{ 3, -32, 32, 0, 8, 11, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks211[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, 0, -32, 0, 0, 15, 0 },
	{ 2, -32, -32, 0, 0, 15, 0 },
	{ 3, -64, -32, 0, 0, 15, 0 },
	{ 4, -32, 0, 0, 0, 15, 0 },
	{ 5, -64, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks212[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	{ 1, 0, 32, 0, 0, 15, 0 },
	{ 2, -32, 32, 0, 0, 15, 0 },
	{ 3, -64, 32, 0, 0, 15, 0 },
	{ 4, -32, 0, 0, 0, 15, 0 },
	{ 5, -64, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks213[] = {
	{ 0, 0, 0, 0, 32, 15, 0 },
	{ 1, 32, 0, 0, 32, 15, 0 },
	{ 2, -64, 0, 0, 32, 15, 0 },
	{ 3, -32, 0, 0, 32, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks214[] = {
	{ 0, 0, 0, 0, 48, 15, 0 },
	{ 1, 32, 0, 0, 48, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks215[] = {
	{ 0, 0, 0, 0, 208, 15, 0 },
	{ 1, 32, 0, 0, 208, 15, 0 },
	{ 2, -32, 0, 0, 160, 15, 0 },
	{ 3, -64, 0, 0, 80, 15, 0 },
	{ 4, -96, 0, 0, 48, 15, 0 },
	{ 5, -128, 0, 0, 32, 15, 0 },
	{ 6, -160, 0, 0, 16, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks216[] = {
	{ 0, 0, 0, 0, 0, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks217[] = {
	{ 0, 0, 0, 0, 16, 71, 0 },
	{ 1, 0, -32, 16, 0, 8, 1 },
	{ 2, -32, 0, 16, 0, 2, 0 },
	{ 3, -32, -32, 16, 16, 103, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks218[] = {
	{ 0, 0, 0, 0, 16, 139, 0 },
	{ 1, 0, 32, 16, 0, 4, 1 },
	{ 2, -32, 0, 16, 0, 1, 0 },
	{ 3, -32, 32, 16, 16, 155, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks219[] = {
	{ 0, 0, 0, 16, 16, 55, 0 },
	{ 1, 0, -32, 16, 0, 8, 1 },
	{ 2, -32, 0, 16, 0, 2, 0 },
	{ 3, -32, -32, 0, 16, 23, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks220[] = {
	{ 0, 0, 0, 16, 16, 59, 0 },
	{ 1, 0, 32, 16, 0, 4, 1 },
	{ 2, -32, 0, 16, 0, 1, 0 },
	{ 3, -32, 32, 0, 16, 43, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks221[] = {
	{ 0, 0, 0, 0, 16, 207, 0 },
	{ 1, 0, -32, 16, 0, 8, 1 },
	{ 2, -32, 0, 16, 16, 71, 0 },
	{ 3, -32, -32, 24, 16, 13, 0 },
	{ 4, -32, -64, 48, 0, 8, 1 },
	{ 5, -64, -32, 32, 16, 71, 0 },
	{ 6, -64, -64, 48, 16, 111, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks222[] = {
	{ 0, 0, 0, 0, 16, 207, 0 },
	{ 1, 0, 32, 16, 0, 4, 1 },
	{ 2, -32, 0, 16, 16, 139, 0 },
	{ 3, -32, 32, 24, 16, 14, 0 },
	{ 4, -32, 64, 48, 0, 4, 1 },
	{ 5, -64, 32, 32, 16, 139, 0 },
	{ 6, -64, 64, 48, 16, 159, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks223[] = {
	{ 0, 0, 0, 48, 16, 63, 0 },
	{ 1, 0, -32, 48, 0, 8, 1 },
	{ 2, -32, 0, 32, 16, 23, 0 },
	{ 3, -32, -32, 24, 16, 13, 0 },
	{ 4, -32, -64, 16, 0, 8, 1 },
	{ 5, -64, -32, 16, 16, 23, 0 },
	{ 6, -64, -64, 0, 16, 159, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks224[] = {
	{ 0, 0, 0, 48, 16, 63, 0 },
	{ 1, 0, 32, 48, 0, 4, 1 },
	{ 2, -32, 0, 32, 16, 43, 0 },
	{ 3, -32, 32, 24, 16, 14, 0 },
	{ 4, -32, 64, 16, 0, 4, 1 },
	{ 5, -64, 32, 16, 16, 43, 0 },
	{ 6, -64, 64, 0, 16, 111, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks225[] = {
	{ 0, 0, 0, 0, 16, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks226[] = {
	{ 0, 0, 0, 0, 16, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks227[] = {
	{ 0, 0, 0, 0, 16, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks228[] = {
	{ 0, 0, 0, 0, 16, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks229[] = {
	{ 0, 0, 0, 0, 16, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks230[] = {
	{ 0, 0, 0, 0, 16, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks231[] = {
	{ 0, 0, 0, 0, 16, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks232[] = {
	{ 0, 0, 0, 0, 16, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks233[] = {
	{ 0, 0, 0, 0, 8, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks234[] = {
	{ 0, 0, 0, 0, 8, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks235[] = {
	{ 0, 0, 0, 0, 8, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks236[] = {
	{ 0, 0, 0, 0, 8, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks237[] = {
	{ 0, 0, 0, 0, 8, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks238[] = {
	{ 0, 0, 0, 0, 8, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks239[] = {
	{ 0, 0, 0, 0, 8, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks240[] = {
	{ 0, 0, 0, 0, 8, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks241[] = {
	{ 0, 0, 0, 0, 8, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks242[] = {
	{ 0, 0, 0, 0, 8, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks243[] = {
	{ 0, 0, 0, 0, 8, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks244[] = {
	{ 0, 0, 0, 0, 8, 207, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks245[] = {
	{ 0, 0, 0, 0, 8, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks246[] = {
	{ 0, 0, 0, 0, 8, 63, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks247[] = {
	{ 0, 0, 0, 0, 8, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks248[] = {
	{ 0, 0, 0, 0, 8, 15, 0 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks249[] = {
	{ 0, 0, 0, 0, 72, 15, 4 },
	{ 1, 0, 32, 0, 0, 0, 5 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks250[] = {
	{ 0, 0, 0, 0, 72, 15, 4 },
	{ 1, 0, -32, 0, 0, 0, 5 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks251[] = {
	{ 0, 0, 0, 0, 72, 15, 4 },
	{ 1, 0, 32, 0, 0, 0, 5 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks252[] = {
	{ 0, 0, 0, 0, 72, 15, 4 },
	{ 1, 0, -32, 0, 0, 0, 5 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks253[] = {
	{ 0, 0, 0, 0, 56, 15, 4 },
	{ 1, 32, 0, 56, 32, 15, 4 },
	{ 2, 64, 0, 96, 16, 15, 4 },
	TRACK_BLOCK_END
};

static const rct_preview_track FlatRideTrackBlocks254[] = {
	{ 0, 0, 0, -32, 16, 15, 4 },
	{ 1, -32, 0, -72, 32, 15, 4 },
	{ 2, -64, 0, -128, 56, 15, 4 },
	{ 3, -96, 0, -128, 0, 0, 5 },
	TRACK_BLOCK_END
};

// rct2: 0x00994A38
const rct_preview_track *FlatRideTrackBlocks[255] = {
	FlatRideTrackBlocks000,
	FlatRideTrackBlocks001,
	FlatRideTrackBlocks002,
	FlatRideTrackBlocks003,
	FlatRideTrackBlocks004,
	FlatRideTrackBlocks005,
	FlatRideTrackBlocks006,
	FlatRideTrackBlocks007,
	FlatRideTrackBlocks008,
	FlatRideTrackBlocks009,
	FlatRideTrackBlocks010,
	FlatRideTrackBlocks011,
	FlatRideTrackBlocks012,
	FlatRideTrackBlocks013,
	FlatRideTrackBlocks014,
	FlatRideTrackBlocks015,
	FlatRideTrackBlocks016,
	FlatRideTrackBlocks017,
	FlatRideTrackBlocks018,
	FlatRideTrackBlocks019,
	FlatRideTrackBlocks020,
	FlatRideTrackBlocks021,
	FlatRideTrackBlocks022,
	FlatRideTrackBlocks023,
	FlatRideTrackBlocks024,
	FlatRideTrackBlocks025,
	FlatRideTrackBlocks026,
	FlatRideTrackBlocks027,
	FlatRideTrackBlocks028,
	FlatRideTrackBlocks029,
	FlatRideTrackBlocks030,
	FlatRideTrackBlocks031,
	FlatRideTrackBlocks032,
	FlatRideTrackBlocks033,
	FlatRideTrackBlocks034,
	FlatRideTrackBlocks035,
	FlatRideTrackBlocks036,
	FlatRideTrackBlocks037,
	FlatRideTrackBlocks038,
	FlatRideTrackBlocks039,
	FlatRideTrackBlocks040,
	FlatRideTrackBlocks041,
	FlatRideTrackBlocks042,
	FlatRideTrackBlocks043,
	FlatRideTrackBlocks044,
	FlatRideTrackBlocks045,
	FlatRideTrackBlocks046,
	FlatRideTrackBlocks047,
	FlatRideTrackBlocks048,
	FlatRideTrackBlocks049,
	FlatRideTrackBlocks050,
	FlatRideTrackBlocks051,
	FlatRideTrackBlocks052,
	FlatRideTrackBlocks053,
	FlatRideTrackBlocks054,
	FlatRideTrackBlocks055,
	FlatRideTrackBlocks056,
	FlatRideTrackBlocks057,
	FlatRideTrackBlocks058,
	FlatRideTrackBlocks059,
	FlatRideTrackBlocks060,
	FlatRideTrackBlocks061,
	FlatRideTrackBlocks062,
	FlatRideTrackBlocks063,
	FlatRideTrackBlocks064,
	FlatRideTrackBlocks065,
	FlatRideTrackBlocks066,
	FlatRideTrackBlocks067,
	FlatRideTrackBlocks068,
	FlatRideTrackBlocks069,
	FlatRideTrackBlocks070,
	FlatRideTrackBlocks071,
	FlatRideTrackBlocks072,
	FlatRideTrackBlocks073,
	FlatRideTrackBlocks074,
	FlatRideTrackBlocks075,
	FlatRideTrackBlocks076,
	FlatRideTrackBlocks077,
	FlatRideTrackBlocks078,
	FlatRideTrackBlocks079,
	FlatRideTrackBlocks080,
	FlatRideTrackBlocks081,
	FlatRideTrackBlocks082,
	FlatRideTrackBlocks083,
	FlatRideTrackBlocks084,
	FlatRideTrackBlocks085,
	FlatRideTrackBlocks086,
	FlatRideTrackBlocks087,
	FlatRideTrackBlocks088,
	FlatRideTrackBlocks089,
	FlatRideTrackBlocks090,
	FlatRideTrackBlocks091,
	FlatRideTrackBlocks092,
	FlatRideTrackBlocks093,
	FlatRideTrackBlocks094,
	FlatRideTrackBlocks095,
	FlatRideTrackBlocks096,
	FlatRideTrackBlocks097,
	FlatRideTrackBlocks098,
	FlatRideTrackBlocks099,
	FlatRideTrackBlocks100,
	FlatRideTrackBlocks101,
	FlatRideTrackBlocks102,
	FlatRideTrackBlocks103,
	FlatRideTrackBlocks104,
	FlatRideTrackBlocks105,
	FlatRideTrackBlocks106,
	FlatRideTrackBlocks107,
	FlatRideTrackBlocks108,
	FlatRideTrackBlocks109,
	FlatRideTrackBlocks110,
	FlatRideTrackBlocks111,
	FlatRideTrackBlocks112,
	FlatRideTrackBlocks113,
	FlatRideTrackBlocks114,
	FlatRideTrackBlocks115,
	FlatRideTrackBlocks116,
	FlatRideTrackBlocks117,
	FlatRideTrackBlocks118,
	FlatRideTrackBlocks119,
	FlatRideTrackBlocks120,
	FlatRideTrackBlocks121,
	FlatRideTrackBlocks122,
	FlatRideTrackBlocks123,
	FlatRideTrackBlocks124,
	FlatRideTrackBlocks125,
	FlatRideTrackBlocks126,
	FlatRideTrackBlocks127,
	FlatRideTrackBlocks128,
	FlatRideTrackBlocks129,
	FlatRideTrackBlocks130,
	FlatRideTrackBlocks131,
	FlatRideTrackBlocks132,
	FlatRideTrackBlocks133,
	FlatRideTrackBlocks134,
	FlatRideTrackBlocks135,
	FlatRideTrackBlocks136,
	FlatRideTrackBlocks137,
	FlatRideTrackBlocks138,
	FlatRideTrackBlocks139,
	FlatRideTrackBlocks140,
	FlatRideTrackBlocks141,
	FlatRideTrackBlocks142,
	FlatRideTrackBlocks143,
	FlatRideTrackBlocks144,
	FlatRideTrackBlocks145,
	FlatRideTrackBlocks146,
	FlatRideTrackBlocks147,
	FlatRideTrackBlocks148,
	FlatRideTrackBlocks149,
	FlatRideTrackBlocks150,
	FlatRideTrackBlocks151,
	FlatRideTrackBlocks152,
	FlatRideTrackBlocks153,
	FlatRideTrackBlocks154,
	FlatRideTrackBlocks155,
	FlatRideTrackBlocks156,
	FlatRideTrackBlocks157,
	FlatRideTrackBlocks158,
	FlatRideTrackBlocks159,
	FlatRideTrackBlocks160,
	FlatRideTrackBlocks161,
	FlatRideTrackBlocks162,
	FlatRideTrackBlocks163,
	FlatRideTrackBlocks164,
	FlatRideTrackBlocks165,
	FlatRideTrackBlocks166,
	FlatRideTrackBlocks167,
	FlatRideTrackBlocks168,
	FlatRideTrackBlocks169,
	FlatRideTrackBlocks170,
	FlatRideTrackBlocks171,
	FlatRideTrackBlocks172,
	FlatRideTrackBlocks173,
	FlatRideTrackBlocks174,
	FlatRideTrackBlocks175,
	FlatRideTrackBlocks176,
	FlatRideTrackBlocks177,
	FlatRideTrackBlocks178,
	FlatRideTrackBlocks179,
	FlatRideTrackBlocks180,
	FlatRideTrackBlocks181,
	FlatRideTrackBlocks182,
	FlatRideTrackBlocks183,
	FlatRideTrackBlocks184,
	FlatRideTrackBlocks185,
	FlatRideTrackBlocks186,
	FlatRideTrackBlocks187,
	FlatRideTrackBlocks188,
	FlatRideTrackBlocks189,
	FlatRideTrackBlocks190,
	FlatRideTrackBlocks191,
	FlatRideTrackBlocks192,
	FlatRideTrackBlocks193,
	FlatRideTrackBlocks194,
	FlatRideTrackBlocks195,
	FlatRideTrackBlocks196,
	FlatRideTrackBlocks197,
	FlatRideTrackBlocks198,
	FlatRideTrackBlocks199,
	FlatRideTrackBlocks200,
	FlatRideTrackBlocks201,
	FlatRideTrackBlocks202,
	FlatRideTrackBlocks203,
	FlatRideTrackBlocks204,
	FlatRideTrackBlocks205,
	FlatRideTrackBlocks206,
	FlatRideTrackBlocks207,
	FlatRideTrackBlocks208,
	FlatRideTrackBlocks209,
	FlatRideTrackBlocks210,
	FlatRideTrackBlocks211,
	FlatRideTrackBlocks212,
	FlatRideTrackBlocks213,
	FlatRideTrackBlocks214,
	FlatRideTrackBlocks215,
	FlatRideTrackBlocks216,
	FlatRideTrackBlocks217,
	FlatRideTrackBlocks218,
	FlatRideTrackBlocks219,
	FlatRideTrackBlocks220,
	FlatRideTrackBlocks221,
	FlatRideTrackBlocks222,
	FlatRideTrackBlocks223,
	FlatRideTrackBlocks224,
	FlatRideTrackBlocks225,
	FlatRideTrackBlocks226,
	FlatRideTrackBlocks227,
	FlatRideTrackBlocks228,
	FlatRideTrackBlocks229,
	FlatRideTrackBlocks230,
	FlatRideTrackBlocks231,
	FlatRideTrackBlocks232,
	FlatRideTrackBlocks233,
	FlatRideTrackBlocks234,
	FlatRideTrackBlocks235,
	FlatRideTrackBlocks236,
	FlatRideTrackBlocks237,
	FlatRideTrackBlocks238,
	FlatRideTrackBlocks239,
	FlatRideTrackBlocks240,
	FlatRideTrackBlocks241,
	FlatRideTrackBlocks242,
	FlatRideTrackBlocks243,
	FlatRideTrackBlocks244,
	FlatRideTrackBlocks245,
	FlatRideTrackBlocks246,
	FlatRideTrackBlocks247,
	FlatRideTrackBlocks248,
	FlatRideTrackBlocks249,
	FlatRideTrackBlocks250,
	FlatRideTrackBlocks251,
	FlatRideTrackBlocks252,
	FlatRideTrackBlocks253,
	FlatRideTrackBlocks254
};
