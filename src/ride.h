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

#include "map.h"
#include "peep.h"
#include "rct2.h"
#include "string_ids.h"

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
 * Ride type structure.
 * size: unknown
 */
typedef struct {
	rct_string_id name;				// 0x000
	rct_string_id description;		// 0x002
	uint32 var_004;
	uint32 var_008;
	uint8 var_00C;
	uint8 var_00D;
	uint8 pad_00E;
	uint8 var_00F;
	uint8 var_010;
	uint8 var_011;
	uint8 var_012;
	uint8 var_013;
	uint8 pad_014[0x19E];
	sint8 excitement_multipler;		// 0x1B2
	sint8 intensity_multipler;		// 0x1B3
	sint8 nausea_multipler;			// 0x1B4
	uint8 pad_1B5;
	uint32 var_1B6;
	uint8 pad_1BA[0x04];
	uint8 category[2];				// 0x1BE
	uint8 shop_item;				// 0x1C0
	uint8 shop_item_secondary;		// 0x1C1
} rct_ride_type;

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
	uint16 vehicle_colours[32];		// 0x006
	uint8 pad_046[0x03];
	// 0 = closed, 1 = open, 2 = test
	uint8 status;					// 0x049
	rct_string_id name;				// 0x04A
	uint32 name_arguments;			// 0x04C probably just for when a ride hasn't been named (e.g. Crooked House 1)
	uint16 overall_view;			// 0x050 00XX = X, XX00 = Y (* 32 + 16)
	uint16 station_starts[4];		// 0x052
	uint8 station_heights[4];		// 0x05A
	uint8 pad_05E[0xC];
	uint16 entrances[4];			// 0x06A
	uint16 exits[4];				// 0x072
	uint8 pad_07A[0x0C];
	uint16 vehicles[32];			// 0x086 Points to the first car in the train
	uint8 depart_flags;				// 0x0C6

	// Not sure if these should be uint or sint.
	uint8 num_stations;				// 0x0C7
	uint8 num_vehicles;				// 0x0C8
	uint8 num_cars_per_train;		// 0x0C9
	uint8 pad_0CA[0x2];
	uint8 var_0CC;
	uint8 var_0CD;
	uint8 min_waiting_time;			// 0x0CE
	uint8 max_waiting_time;			// 0x0CF
	uint8 var_0D0;
	uint8 pad_0D1[0x7];
	sint32 max_speed;				// 0x0D8
	sint32 average_speed;			// 0x0DC
	uint8 pad_0E0[0x4];
	sint32 length[4];				// 0x0E4
	uint16 time[4];					// 0x0F4
	fixed16_2dp max_positive_vertical_g;	// 0x0FC
	fixed16_2dp max_negative_vertical_g;	// 0x0FE
	fixed16_2dp max_lateral_g;		// 0x100
	uint8 pad_102[0x12];
	uint8 inversions;				// 0x114 (???X XXXX) holes for mini golf
	uint8 drops;					// 0x115 (??XX XXXX)
	uint8 pad_116;
	uint8 highest_drop_height;		// 0x117
	uint8 pad_118[0x0C];
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
	ride_rating excitement;			// 0x140
	ride_rating intensity;			// 0x142
	ride_rating nausea;				// 0x144
	uint16 reliability;				// 0x146
	uint16 pad_148;
	uint16 var_14A;
	uint8 pad_14C;
	uint8 var_14D;
	uint8 pad_14E[0x02];
	uint32 total_customers;			// 0x150
	money32 total_profit;			// 0x154
	uint16 var_158;
	uint8 pad_15A;
	uint8 num_riders;				// 0x15B
	uint8 pad_15C[0x24];
	sint16 build_date;				// 0x180
	money16 upkeep_cost;			// 0x182
	uint16 race_winner;				// 0x184
	uint8 pad_186[0x06];
	uint8 var_18C;
	uint8 mechanic_status;			// 0x18D
	uint16 mechanic;				// 0x18E
	uint8 pad_190[0x03];
	uint8 breakdown_reason;			// 0x193
	money16 price_secondary;		// 0x194
	uint16 var_196;
	// used in computing excitement, nausea, etc
	uint8 var_198;
	uint8 var_199;
	uint8 inspection_interval;		// 0x19A
	uint8 last_inspection;			// 0x19B
	uint8 pad_19C[0x8];
	uint32 var_1A4;
	uint8 pad_1A8[6];
	uint8 var_1AE;
	uint8 connected_message_throttle;	// 0x1AF
	money32 income_per_hour;		// 0x1B0
	money32 profit;					// 0x1B4
	uint8 queue_time[4];			// 0x1B8
	uint8 track_colour_main[4];		// 0x1BC
	uint8 track_colour_additional[4];	// 0x1C0
	uint8 track_colour_supports[4];	// 0x1C4
	uint8 music;					// 0x1C8
	uint8 entrance_style;			// 0x1C9
	uint8 pad_1CA[0x02];
	uint8 num_block_brakes;			// 0x1CC
	uint8 lift_hill_speed;			// 0x1CD
	uint16 guests_favourite;		// 0x1CE
	uint32 lifecycle_flags;			// 0x1D0
	uint8 vehicle_colours_extended[32];	// 0x1D4
	uint16 total_air_time;			// 0x1F4
	uint8 pad_1F6;
	uint8 num_circuits;				// 0x1F7
	uint8 pad_1F8[0x8];
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
	RIDE_LIFECYCLE_ON_TRACK = 1 << 0,
	RIDE_LIFECYCLE_TESTED = 1 << 1,
	RIDE_LIFECYCLE_TEST_IN_PROGRESS = 1 << 2,
	RIDE_LIFECYCLE_NO_RAW_STATS = 1 << 3,
	RIDE_LIFECYCLE_PASS_STATION_NO_STOPPING = 1 << 4,
	RIDE_LIFECYCLE_ON_RIDE_PHOTO = 1 << 5,

	RIDE_LIFECYCLE_BROKEN_DOWN = 1 << 7,

	RIDE_LIFECYCLE_CRASHED = 1 << 10,
	RIDE_LIFECYCLE_11 = 1 << 11,
	RIDE_LIFECYCLE_EVER_BEEN_OPENED = 1 << 12,
	RIDE_LIFECYCLE_MUSIC = 1 << 13,
	RIDE_LIFECYCLE_INDESTRUCTIBLE = 1 << 14,
	RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK = 1 << 15,

	RIDE_LIFECYCLE_CABLE_LIFT = 1 << 17,
	RIDE_LIFECYCLE_19 = 1 << 19
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
	RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER,
	RIDE_TYPE_90
};

enum {
	RIDE_STATUS_CLOSED,
	RIDE_STATUS_OPEN,
	RIDE_STATUS_TESTING
};

enum {
	RIDE_MODE_NORMAL,
	RIDE_MODE_CONTINUOUS_CIRCUIT,
	RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE,
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
	RIDE_MODE_POWERED_LAUNCH_35,					// RCT2 style?
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

enum {
	MUSIC_STYLE_DODGEMS_BEAT,
	MUSIC_STYLE_FAIRGROUND_ORGAN,
	MUSIC_STYLE_ROMAN_FANFARE,
	MUSIC_STYLE_ORIENTAL,
	MUSIC_STYLE_MARTIAN,
	MUSIC_STYLE_JUNGLE_DRUMS,
	MUSIC_STYLE_EGYPTIAN,
	MUSIC_STYLE_TOYLAND,
	MUSIC_STYLE_8,
	MUSIC_STYLE_SPACE,
	MUSIC_STYLE_HORROR,
	MUSIC_STYLE_TECHNO,
	MUSIC_STYLE_GENTLE,
	MUSIC_STYLE_SUMMER,
	MUSIC_STYLE_WATER,
	MUSIC_STYLE_WILD_WEST,
	MUSIC_STYLE_JURASSIC,
	MUSIC_STYLE_ROCK,
	MUSIC_STYLE_RAGTIME,
	MUSIC_STYLE_FANTASY,
	MUSIC_STYLE_ROCK_STYLE_2,
	MUSIC_STYLE_ICE,
	MUSIC_STYLE_SNOW,
	MUSIC_STYLE_CUSTOM_MUSIC_1,
	MUSIC_STYLE_CUSTOM_MUSIC_2,
	MUSIC_STYLE_MEDIEVAL,
	MUSIC_STYLE_URBAN,
	MUSIC_STYLE_ORGAN,
	MUSIC_STYLE_MECHANICAL,
	MUSIC_STYLE_MODERN,
	MUSIC_STYLE_PIRATES,
	MUSIC_STYLE_ROCK_STYLE_3,
	MUSIC_STYLE_CANDY_STYLE
};

enum {
	BREAKDOWN_NONE = 255,
	BREAKDOWN_SAFETY_CUT_OUT = 0,
	BREAKDOWN_RESTRAINTS_STUCK_CLOSED,
	BREAKDOWN_RESTRAINTS_STUCK_OPEN,
	BREAKDOWN_DOORS_STUCK_CLOSED,
	BREAKDOWN_DOORS_STUCK_OPEN,
	BREAKDOWN_VEHICLE_MALFUNCTION,
	BREAKDOWN_BRAKES_FAILURE,
	BREAKDOWN_CONTROL_FAILURE
};

enum {
	RIDE_MECHANIC_STATUS_CALLING = 1,
	RIDE_MECHANIC_STATUS_HEADING = 2,
	RIDE_MECHANIC_STATUS_FIXING = 3,
};

enum {
	RIDE_DEPART_WAIT_FOR_LOAD_MASK = 7,
	RIDE_DEPART_WAIT_FOR_LOAD = 1 << 3,
	RIDE_DEPART_LEAVE_WHEN_ANOTHER_ARRIVES = 1 << 4,
	RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS = 1 << 5,
	RIDE_DEPART_WAIT_FOR_MINIMUM_LENGTH = 1 << 6,
	RIDE_DEPART_WAIT_FOR_MAXIMUM_LENGTH = 1 << 7
};

enum {
	RIDE_COLOUR_SCHEME_MAIN,
	RIDE_COLOUR_SCHEME_ADDITIONAL_1,
	RIDE_COLOUR_SCHEME_ADDITIONAL_2,
	RIDE_COLOUR_SCHEME_ADDITIONAL_3
};

enum {
	VEHICLE_COLOUR_SCHEME_SAME,
	VEHICLE_COLOUR_SCHEME_PER_TRAIN,
	VEHICLE_COLOUR_SCHEME_PER_VEHICLE
};

enum {
	RIDE_ENTRANCE_STYLE_PLAIN,
	RIDE_ENTRANCE_STYLE_WOODEN,
	RIDE_ENTRANCE_STYLE_CANVAS_TENT,
	RIDE_ENTRANCE_STYLE_CASTLE_GREY,
	RIDE_ENTRANCE_STYLE_CASTLE_BROWN,
	RIDE_ENTRANCE_STYLE_JUNGLE,
	RIDE_ENTRANCE_STYLE_LOG_CABIN,
	RIDE_ENTRANCE_STYLE_CLASSICAL_ROMAN,
	RIDE_ENTRANCE_STYLE_ABSTRACT,
	RIDE_ENTRANCE_STYLE_SNOW_ICE,
	RIDE_ENTRANCE_STYLE_PAGODA,
	RIDE_ENTRANCE_STYLE_SPACE
};

typedef struct {
	uint8 main;
	uint8 additional;
	uint8 supports;
} track_colour;

typedef struct {
	uint8 main;
	uint8 additional_1;
	uint8 additional_2;
} vehicle_colour;

#define MAX_RIDES 255

#define MAX_RIDE_MEASUREMENTS 8
#define RIDE_RELIABILITY_UNDEFINED 0xFFFF

// rct2: 0x009ACFA4
rct_ride_type **gRideTypeList;

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
rct_map_element *sub_6CAF80(int rideIndex, int *outX, int *outY);
rct_map_element *ride_find_track_gap(rct_map_element *startTrackElement, int *outX, int *outY);
void ride_construct_new(int list_item);
int ride_try_construct(rct_map_element *trackMapElement);
void ride_get_status(int rideIndex, int *formatSecondary, int *argument);
rct_peep *ride_get_assigned_mechanic(rct_ride *ride);
int ride_get_total_length(rct_ride *ride);
int ride_can_have_multiple_circuits(rct_ride *ride);
track_colour ride_get_track_colour(rct_ride *ride, int colourScheme);
vehicle_colour ride_get_vehicle_colour(rct_ride *ride, int vehicleIndex);

#endif
