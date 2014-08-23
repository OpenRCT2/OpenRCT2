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

typedef fixed16_2dp ride_rating;

// Convenience function for writing ride ratings. The result is a 16 bit signed
// integer. To create the ride rating 3.65 type RIDE_RATING(3,65)
#define RIDE_RATING(whole, fraction)	FIXED_2DP(whole, fraction)

// Used for return values, for functions that modify all three.
typedef struct {
	ride_rating excitement;
	ride_rating intensity;
	ride_rating nausea;
} rating_tuple;

/**
 * Ride structure.
 * size: 0x0260
 */
typedef struct {
	uint8 type;						// 0x000
	// pointer to static info. for example, wild mouse type is 0x36, subtype is
	// 0x4c.
	uint8 subtype;					// 0x001
	uint16 pad_002;
	uint8 mode;						// 0x004
	uint8 colour_scheme_type;		// 0x005
	uint16 car_colours[32];			// 0x006
	uint8 pad_046[0x03];
	// 0 = closed, 1 = open, 2 = test
	uint8 status;					// 0x049
	uint16 var_04A;
	uint32 var_04C;
	uint16 overall_view;			// 0x050
	uint16 station_starts[4];		// 0x052
	uint8 station_heights[4];		// 0x05A
	uint8 pad_05E[0xC];
	uint16 entrances[4];			// 0x06A
	uint16 exits[4];				// 0x072
	uint8 pad_07A[0x0C];
	uint16 train_car_map[1];		// 0x086 Points to the first car in the train
	uint8 pad_088[0x3F];

	// Not sure if these should be uint or sint.
	uint8 var_0C7;
	uint8 var_0C8;
	uint8 var_0C9;

	uint8 pad_0CA[0x1A];

	sint32 var_0E4;
	sint32 var_0E8;
	sint32 var_0EC;
	sint32 var_0F0;
	uint8 pad_0F4[0x20];
	uint8 var_114;
	// Track length? Number of track segments?
	uint8 var_115;
	uint8 pad_116[0x0E];
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
	money16 price;					// 0x138
	uint8 pad_13A[0x06];
	sint16 excitement;				// 0x140
	sint16 intensity;				// 0x142
	uint16 nausea;					// 0x144
	uint16 reliability;				// 0x146
	uint16 pad_148;
	uint16 var_14A;
	uint8 pad_14C;
	uint8 var_14D;
	uint8 pad_14E[0x06];
	uint32 var_154;
	uint16 var_158;
	uint8 pad_15A;
	uint8 num_riders;				// 0x15B
	uint8 pad_15C[0x24];
	uint16 build_date;				
	sint16 upkeep_cost;				// 0x182
	uint16 race_winner;				// 0x184
	uint8 pad_186[0x10];
	uint16 var_196;
	// used in computing excitement, nausea, etc
	uint8 var_198;
	uint8 var_199;
	uint8 pad_19A[0x14];
	uint8 var_1AE;
	uint8 connected_message_throttle;
	uint32 pad_1B0;
	sint32 profit;					// 0x1B4
	uint8 queue_time[4];			// 0x1B8
	uint8 var_1BC;
	uint8 pad_1BD[0x10];
	uint8 var_1CD;
	uint16 guests_favourite;		// 0x1CE
	uint32 lifecycle_flags;			// 0x1D0
	uint8 pad_1D4[0x20];
	// Example value for wild mouse ride is d5 (before it's been constructed)
	// I tried searching the IDA file for "1F4" but couldn't find places where
	// this is written to.
	uint16 var_1F4;
	uint8 pad_1F6[0x0a];
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

// Constants used by the lifecycle_flags property at 0x1D0
enum {
	RIDE_LIFECYCLE_ON_TRACK = 1,
	RIDE_LIFECYCLE_TESTED = 1 << 1,
	RIDE_LIFECYCLE_TEST_IN_PROGRESS = 1 << 2,
	RIDE_LIFECYCLE_NO_RAW_STATS = 1 << 3,
	RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING = 1 << 4,
	RIDE_LIFECYCLE_ON_RIDE_PHOTO = 1 << 5,

	RIDE_LIFECYCLE_BROKEN_DOWN = 1 << 7,

	RIDE_LIFECYCLE_CRASHED = 1 << 10,

	RIDE_LIFECYCLE_EVER_BEEN_OPENED = 1 << 12,
	RIDE_LIFECYCLE_MUSIC = 1 << 13,
	RIDE_LIFECYCLE_INDESTRUCTIBLE = 1 << 14,
	RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK = 1 << 15,

	RIDE_LIFECYCLE_CABLE_LIFT = 1 << 17
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
	RIDE_MODE_LIM_POWERED_LAUNCH,                  // 0x17
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
	RIDE_MODE_POWERED_LAUNCH2,						// 0x23. RCT2 style?
	RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED
};

enum {
	RIDE_COLOUR_SCHEME_ALL_SAME,
	RIDE_COLOUR_SCHEME_DIFFERENT_PER_TRAIN,
	RIDE_COLOUR_SCHEME_DIFFERENT_PER_CAR
};

enum {
	RIDE_GROUP_TRANSPORT,
	RIDE_GROUP_GENTLE,
	RIDE_GROUP_ROLLERCOASTER,
	RIDE_GROUP_THRILL,
	RIDE_GROUP_WATER,
	RIDE_GROUP_SHOP
};

#define MAX_RIDES 255

#define MAX_RIDE_MEASUREMENTS 8
#define RIDE_RELIABILITY_UNDEFINED 0xFFFF

// rct2: 0x013628F8
extern rct_ride* g_ride_list;

/** Helper macros until rides are stored in this module. */
#define GET_RIDE(x) (&g_ride_list[x])
#define GET_RIDE_MEASUREMENT(x) (&(RCT2_ADDRESS(RCT2_ADDRESS_RIDE_MEASUREMENTS, rct_ride_measurement)[x]))

/**
 * Helper macro loop for enumerating through all the non null rides.
 */
#define FOR_ALL_RIDES(i, ride) \
	for (i = 0; i < MAX_RIDES; i++) \
		if ((ride = &g_ride_list[i])->type != RIDE_TYPE_NULL)

extern const uint8 gRideClassifications[255];

int ride_get_count();
int ride_get_total_queue_length(rct_ride *ride);
int ride_get_max_queue_time(rct_ride *ride);
void ride_init_all();
void reset_all_ride_build_dates();
void ride_update_favourited_stat();
void ride_check_all_reachable();

#endif
