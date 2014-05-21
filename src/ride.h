/*****************************************************************************
 * Copyright (c) 2014 Ted John, Peter Hill
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

#ifndef _RIDE_H_
#define _RIDE_H_

#include "rct2.h"

/**
 * Ride structure.
 * size: 0x0260
 */
typedef struct {
	uint8 type;						// 0x000
	uint8 subtype;					// 0x001
	uint16 pad_002;
	uint8 mode;						// 0x004
	uint8 colour_scheme_type;		// 0x005
	uint16 car_colours[32];			// 0x006
	uint8 pad_046[0x03];
	uint8 status;					// 0x049
	uint16 var_04A;
	uint32 var_04C;
	uint16 overall_view;			// 0x050
	uint16 station_starts[4];		// 0x052
	uint8 pad_05A[0x10];
	uint16 entrances[4];			// 0x06A
	uint16 exits[4];				// 0x072
	uint8 pad_07A[0x0C];
	uint16 train_car_map[1];		// 0x086 Points to the first car in the train
	uint8 pad_088[0x68];
	sint16 var_0F0;
	sint16 var_0F2;
	sint16 var_0F4;
	uint8 pad_0F6[0x2E];
	sint16 var_124;
	sint16 var_126;
	sint16 var_128;
	sint16 var_12A;
	sint16 var_12C;
	sint16 var_12E;
	uint16 age;						// 0x130
	sint16 running_cost;			// 0x132
	sint16 var_134;
	sint16 var_136;
	sint16 price;					// 0x138
	uint8 pad_13A[0x06];
	sint16 excitement;				// 0x140
	sint16 intensity;				// 0x142
	uint16 nausea;					// 0x144
	uint16 reliability;				// 0x146
	uint16 pad_148;
	uint16 var_14A;
	uint8 pad_14C;
	uint8 var_14D;
	uint8 pad_14E[0x0A];
	uint16 var_158;
	uint8 pad_15A[0x26];
	uint16 build_date;				
	sint16 upkeep_cost;				// 0x182
	uint8 pad_184[0x12];
	uint16 var_196;
	uint8 pad_198;
	uint8 var_199;
	uint8 pad_19A[0x15];
	uint8 var_1AF;
	uint32 pad_1B0;
	sint32 profit;					// 0x1B4
	uint8 queue_time[4];			// 0x1B8
	uint8 pad_1BC[0x12];
	uint16 guests_favourite;		// 0x1CE
	uint32 var_1D0;
	uint8 pad_1D4[0x2C];
	uint16 queue_length[4];			// 0x200
	uint8 pad_208[0x58];
} rct_ride;

/**
 * Ride measurement structure.
 * size: 0x04B0C
 */
typedef struct {
	uint8 var_00;
	uint8 pad_01[0x4B0B];
} rct_ride_measurement;

enum {
	RIDE_CLASS_RIDE,
	RIDE_CLASS_SHOP_OR_STALL,
	RIDE_CLASS_KIOSK_OR_FACILITY
};

enum {
	RIDE_TYPE_NULL = 255,
	RIDE_TYPE_SPIRAL_ROLLER_COASTER = 0,
	RIDE_TYPE_STAND_UP_ROLLER_COASTER,
	RIDE_TYPE_SUSPENDED_SWINGING_COASTER,
	RIDE_TYPE_INVERTED_ROLLER_COASTER,
	RIDE_TYPE_JUNIOR_ROLLER_COASTER,
	RIDE_TYPE_MINIATURE_RAILWAY,
	RIDE_TYPE_MONORAIL,
	RIDE_TYPE_MINI_SUSPENDED_COASTER,
	RIDE_TYPE_BUMPER_BOATS,
	RIDE_TYPE_WOODEN_WILD_MOUSE,
	RIDE_TYPE_STEEPLECHASE,
	RIDE_TYPE_CAR_RIDE,
	RIDE_TYPE_LAUNCHED_FREEFALL,
	RIDE_TYPE_BOBSLEIGH_COASTER,
	RIDE_TYPE_OBSERVATION_TOWER,
	RIDE_TYPE_LOOPING_ROLLER_COASTER,
	RIDE_TYPE_DINGHY_SLIDE,
	RIDE_TYPE_MINE_TRAIN_COASTER,
	RIDE_TYPE_CHAIRLIFT,
	RIDE_TYPE_CORKSCREW_ROLLER_COASTER,
	RIDE_TYPE_MAZE,
	RIDE_TYPE_SPIRAL_SLIDE,
	RIDE_TYPE_GO_KARTS,
	RIDE_TYPE_LOG_FLUME,
	RIDE_TYPE_RIVER_RAPIDS,
	RIDE_TYPE_BUMPER_CARS,
	RIDE_TYPE_PIRATE_SHIP,
	RIDE_TYPE_SWINGING_INVERTER_SHIP,
	RIDE_TYPE_FOOD_STALL,
	RIDE_TYPE_1D,
	RIDE_TYPE_DRINK_STALL,
	RIDE_TYPE_1F,
	RIDE_TYPE_SHOP,
	RIDE_TYPE_MERRY_GO_ROUND,
	RIDE_TYPE_22,
	RIDE_TYPE_INFORMATION_KIOSK,
	RIDE_TYPE_BATHROOM,
	RIDE_TYPE_FERRIS_WHEEL,
	RIDE_TYPE_MOTION_SIMULATOR,
	RIDE_TYPE_3D_CINEMA,
	RIDE_TYPE_TOP_SPIN,
	RIDE_TYPE_SPACE_RINGS,
	RIDE_TYPE_REVERSE_FREEFALL_COASTER,
	RIDE_TYPE_ELEVATOR,
	RIDE_TYPE_VERTICAL_DROP_ROLLER_COASTER,
	RIDE_TYPE_ATM,
	RIDE_TYPE_TWIST,
	RIDE_TYPE_HAUNTED_HOUSE,
	RIDE_TYPE_FIRST_AID,
	RIDE_TYPE_CIRCUS_SHOW,
	RIDE_TYPE_GHOST_TRAIN,
	RIDE_TYPE_TWISTER_ROLLER_COASTER,
	RIDE_TYPE_WOODEN_ROLLER_COASTER,
	RIDE_TYPE_SIDE_FRICTION_ROLLER_COASTER,
	RIDE_TYPE_WILD_MOUSE,
	RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER,
	RIDE_TYPE_38,
	RIDE_TYPE_FLYING_ROLLER_COASTER,
	RIDE_TYPE_3A,
	RIDE_TYPE_VIRGINIA_REEL,
	RIDE_TYPE_SPLASH_BOATS,
	RIDE_TYPE_MINI_HELICOPTERS,
	RIDE_TYPE_LAY_DOWN_ROLLER_COASTER,
	RIDE_TYPE_SUSPENDED_MONORAIL,
	RIDE_TYPE_40,
	RIDE_TYPE_REVERSER_ROLLER_COASTER,
	RIDE_TYPE_HEARTLINE_TWISTER_COASTER,
	RIDE_TYPE_MINI_GOLF,
	RIDE_TYPE_GIGA_COASTER,
	RIDE_TYPE_ROTO_DROP,
	RIDE_TYPE_FLYING_SAUCERS,
	RIDE_TYPE_CROOKED_HOUSE,
	RIDE_TYPE_MONORAIL_CYCLES,
	RIDE_TYPE_COMPACT_INVERTED_COASTER,
	RIDE_TYPE_WATER_COASTER,
	RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER,
	RIDE_TYPE_INVERTED_HAIRPIN_COASTER,
	RIDE_TYPE_MAGIC_CARPET,
	RIDE_TYPE_SUBMARINE_RIDE,
	RIDE_TYPE_RIVER_RAFTS,
	RIDE_TYPE_50,
	RIDE_TYPE_ENTERPRISE,
	RIDE_TYPE_52,
	RIDE_TYPE_53,
	RIDE_TYPE_54,
	RIDE_TYPE_55,
	RIDE_TYPE_INVERTED_IMPULSE_COASTER,
	RIDE_TYPE_MINI_ROLLER_COASTER,
	RIDE_TYPE_MINE_RIDE,
	RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER
};

enum {
	RIDE_STATUS_CLOSED,
	RIDE_STATUS_OPEN,
	RIDE_STATUS_TESTING
};

enum {
	RIDE_MODE_NORMAL = 0,
	RIDE_MODE_CONTINUOUS_CIRCUIT,
	RIDE_MODE_REVERSE_INCLINED_SHUTTLE,
	RIDE_MODE_POWERED_LAUNCH,						// RCT1 style?
	RIDE_MODE_SHUTTLE,
	RIDE_MODE_BOAT_HIRE,
	RIDE_MODE_UPWARD_LAUNCH,
	RIDE_MODE_ROTATING_LIFT,
	RIDE_MODE_STATION_TO_STATION,
	RIDE_MODE_SINGLE_RIDE_PER_ADMISSION,
	RIDE_MODE_UNLIMITED_RIDES_PER_ADMISSION,
	RIDE_MODE_MAZE,
	RIDE_MODE_RACE,
	RIDE_MODE_BUMPERCAR,
	RIDE_MODE_SWING,
	RIDE_MODE_SHOP_STALL,
	RIDE_MODE_ROTATION,
	RIDE_MODE_FORWARD_ROTATION,
	RIDE_MODE_BACKWARD_ROTATION,
	RIDE_MODE_FILM_AVENGING_AVIATORS,
	RIDE_MODE_3D_FILM_MOUSE_TAILS,
	RIDE_MODE_SPACE_RINGS,
	RIDE_MODE_BEGINNERS,
	RIDE_MODE_LIM_POWERED_LAUNCH,
	RIDE_MODE_FILM_THRILL_RIDERS,
	RIDE_MODE_3D_FILM_STORM_CHASERS,
	RIDE_MODE_3D_FILM_SPACE_RAIDERS,
	RIDE_MODE_INTENSE,
	RIDE_MODE_BERSERK,
	RIDE_MODE_HAUNTED_HOUSE,
	RIDE_MODE_CIRCUS_SHOW,
	RIDE_MODE_DOWNWARD_LAUNCH,
	RIDE_MODE_CROOKED_HOUSE,
	RIDE_MODE_FREEFALL_DROP,
	RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED,
	RIDE_MODE_POWERED_LAUNCH2,						// RCT2 style?
	RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED
};

enum {
	RIDE_COLOUR_SCHEME_ALL_SAME,
	RIDE_COLOUR_SCHEME_DIFFERENT_PER_TRAIN,
	RIDE_COLOUR_SCHEME_DIFFERENT_PER_CAR
};

#define MAX_RIDES 255
#define MAX_RIDE_MEASUREMENTS 8

extern const uint8 gRideClassifications[255];

int ride_get_count();
int ride_get_total_queue_length(rct_ride *ride);
int ride_get_max_queue_time(rct_ride *ride);
void ride_init_all();
void reset_all_ride_build_dates();
void ride_update_favourited_stat();
void ride_check_all_reachable();

#endif
