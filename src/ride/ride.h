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

#ifndef _RIDE_H_
#define _RIDE_H_

#include "../addresses.h"
#include "../common.h"
#include "../peep/peep.h"
#include "../world/map.h"
#include "vehicle.h"

typedef fixed16_2dp ride_rating;

// Convenience function for writing ride ratings. The result is a 16 bit signed
// integer. To create the ride rating 3.65 type RIDE_RATING(3,65)
#define RIDE_RATING(whole, fraction)	FIXED_2DP(whole, fraction)

#pragma pack(push, 1)

// Used for return values, for functions that modify all three.
typedef struct rating_tuple {
	ride_rating excitement;
	ride_rating intensity;
	ride_rating nausea;
} rating_tuple;
assert_struct_size(rating_tuple, 6);

/**
 * Couples a ride type and subtype together.
 */
typedef struct ride_list_item {
	union {
		struct {
			uint8 type;
			uint8 entry_index;
		};
		uint16 ride_type_and_entry;
	};
} ride_list_item;
assert_struct_size(ride_list_item, 2);

typedef struct track_colour {
	uint8 main;
	uint8 additional;
	uint8 supports;
} track_colour;
assert_struct_size(track_colour, 3);

typedef struct vehicle_colour {
	uint8 main;
	uint8 additional_1;
	uint8 additional_2;
} vehicle_colour;
assert_struct_size(vehicle_colour, 3);

typedef struct track_colour_preset_list {
	uint8 count;
	track_colour list[256];
} track_colour_preset_list;
assert_struct_size(track_colour_preset_list, (1 + 256 * 3));

typedef struct vehicle_colour_preset_list {
	uint8 count;
	vehicle_colour list[256];
} vehicle_colour_preset_list;
assert_struct_size(vehicle_colour_preset_list, (1 + 256 * 3));

/**
 * Ride type structure.
 * size: unknown
 */
typedef struct rct_ride_entry {
	rct_string_id name;						// 0x000
	rct_string_id description;				// 0x002
	uint32 images_offset;					// 0x004
	uint32 flags;							// 0x008
	uint8 ride_type[3];						// 0x00C
	uint8 min_cars_in_train;				// 0x00F
	uint8 max_cars_in_train;				// 0x010
	uint8 cars_per_flat_ride;				// 0x011
	// Number of cars that can't hold passengers
	uint8 zero_cars;						// 0x012
	// The index to the vehicle type displayed in
	// the vehicle tab.
	uint8 tab_vehicle;						// 0x013
	uint8 default_vehicle;					// 0x014
	// Convert from first - fourth vehicle to
	// vehicle structure
	uint8 front_vehicle;					// 0x015
	uint8 second_vehicle;					// 0x016
	uint8 rear_vehicle;						// 0x017
	uint8 third_vehicle;					// 0x018
	uint8 pad_019;
	rct_ride_entry_vehicle vehicles[4];		// 0x1A
	vehicle_colour_preset_list *vehicle_preset_list;	// 0x1AE
	sint8 excitement_multipler;				// 0x1B2
	sint8 intensity_multipler;				// 0x1B3
	sint8 nausea_multipler;					// 0x1B4
	uint8 max_height;			// 0x1B5
	union {
		uint64 enabledTrackPieces;						// 0x1B6
		struct {
			uint32 enabledTrackPiecesA;					// 0x1B6
			uint32 enabledTrackPiecesB;					// 0x1BA
		};
	};
	uint8 category[2];									// 0x1BE
	uint8 shop_item;									// 0x1C0
	uint8 shop_item_secondary;							// 0x1C1
} rct_ride_entry;
#ifdef PLATFORM_32BIT
assert_struct_size(rct_ride_entry, 0x1c2);
#endif

/**
 * Ride structure.
 * size: 0x0260
 */
typedef struct rct_ride {
	uint8 type;						// 0x000
	// pointer to static info. for example, wild mouse type is 0x36, subtype is
	// 0x4c.
	uint8 subtype;					// 0x001
	uint16 pad_002;
	uint8 mode;						// 0x004
	uint8 colour_scheme_type;		// 0x005
	rct_vehicle_colour vehicle_colours[32];		// 0x006
	uint8 pad_046[0x03];
	// 0 = closed, 1 = open, 2 = test
	uint8 status;					// 0x049
	rct_string_id name;				// 0x04A
	union {
		uint32 name_arguments;		// 0x04C
		struct {
			rct_string_id name_arguments_type_name;		// 0x04C
			uint16 name_arguments_number;				// 0x04E
		};
	};
	uint16 overall_view;			// 0x050 00XX = X, XX00 = Y (* 32 + 16)
	uint16 station_starts[4];		// 0x052
	uint8 station_heights[4];		// 0x05A
	uint8 station_length[4];		// 0x05E
	uint8 station_depart[4];		// 0x062
	// ride->vehicle index for current train waiting for passengers
	// at station
	uint8 train_at_station[4];		// 0x066
	uint16 entrances[4];			// 0x06A
	uint16 exits[4];				// 0x072
	uint16 last_peep_in_queue[4];	// 0x07A
	uint8 pad_082[4];
	uint16 vehicles[32];			// 0x086 Points to the first car in the train
	uint8 depart_flags;				// 0x0C6

	// Not sure if these should be uint or sint.
	uint8 num_stations;				// 0x0C7
	uint8 num_vehicles;				// 0x0C8
	uint8 num_cars_per_train;		// 0x0C9
	uint8 proposed_num_vehicles;	// 0x0CA
	uint8 proposed_num_cars_per_train; // 0x0CB
	uint8 max_trains;				// 0x0CC
	uint8 min_max_cars_per_train;	// 0x0CD
	uint8 min_waiting_time;			// 0x0CE
	uint8 max_waiting_time;			// 0x0CF
	union {
		uint8 operation_option;		// 0x0D0
		uint8 time_limit;			// 0x0D0
		uint8 num_laps;				// 0x0D0
		uint8 launch_speed;			// 0x0D0
		uint8 speed;				// 0x0D0
		uint8 rotations;			// 0x0D0
	};
	uint8 boat_hire_return_direction;	// 0x0D1
	uint16 boat_hire_return_position;	// 0x0D2
	uint8 measurement_index;		// 0x0D4
    // bits 0 through 4 are the number of helix sections
    // bit 5: spinning tunnel, water splash, or rapids
    // bit 6: log reverser, waterfall
    // bit 7: whirlpool
	uint8 special_track_elements;   // 0x0D5
	uint8 pad_0D6[2];
	// Divide this value by 29127 to get the human-readable max speed
	// (in RCT2, display_speed = (max_speed * 9) >> 18)
	sint32 max_speed;				// 0x0D8
	sint32 average_speed;			// 0x0DC
	uint8 current_test_segment;		// 0x0E0
	uint8 average_speed_test_timeout;	// 0x0E1
	uint8 pad_0E2[0x2];
	sint32 length[4];				// 0x0E4
	uint16 time[4];					// 0x0F4
	fixed16_2dp max_positive_vertical_g;	// 0x0FC
	fixed16_2dp max_negative_vertical_g;	// 0x0FE
	fixed16_2dp max_lateral_g;		// 0x100
	fixed16_2dp previous_vertical_g;// 0x102
	fixed16_2dp previous_lateral_g;	// 0x104
	uint8 pad_106[0x2];
	uint32 testing_flags;			// 0x108
	// x y map location of the current track piece during a test
	// this is to prevent counting special tracks multiple times
	rct_xy8 cur_test_track_location;	// 0x10C
	// Next 3 variables are related (XXXX XYYY ZZZa aaaa)
	uint16 turn_count_default;		// 0x10E X = current turn count
	uint16 turn_count_banked;		// 0x110
	uint16 turn_count_sloped;		// 0x112 X = number turns > 3 elements
	union {
		uint8 inversions;			// 0x114 (???X XXXX)
		uint8 holes;				// 0x114 (???X XXXX)
		// The undercover portion is a very rough approximation of how much of the ride is undercover.
		// It reaches the maximum value of 7 at about 50% undercover and doesn't increase beyond that.
		uint8 undercover_portion;	// 0x114 (XXX?-????)
	};
	// Y is number of powered lifts, X is drops
	uint8 drops;					// 0x115 (YYXX XXXX)
	uint8 start_drop_height;		// 0x116
	uint8 highest_drop_height;		// 0x117
	sint32 sheltered_length;		// 0x118
	// Unused always 0? Should affect nausea
	uint16 var_11C;
	uint8 num_sheltered_sections;   // 0x11E (?abY YYYY)
	// see cur_test_track_location
	uint8 cur_test_track_z;			// 0x11F
	// Customer counter in the current 960 game tick (about 30 seconds) interval
	uint16 cur_num_customers;       // 0x120
	// Counts ticks to update customer intervals, resets each 960 game ticks.
	uint16 num_customers_timeout;   // 0x122
	// Customer count in the last 10 * 960 game ticks (sliding window)
	uint16 num_customers[10];       // 0x124
	money16 price;					// 0x138
	rct_xy8 chairlift_bullwheel_location[2]; // 0x13A
	uint8 chairlift_bullwheel_z[2];	// 0x13E
	union {
		rating_tuple ratings;		// 0x140
		struct {
			ride_rating excitement;	// 0x140
			ride_rating intensity;	// 0x142
			ride_rating nausea;		// 0x144
		};
	};
	uint16 value;					// 0x146
	uint16 chairlift_bullwheel_rotation;	// 0x148
	uint8 satisfaction;				// 0x14A
	uint8 satisfaction_time_out;	// 0x14B
	uint8 satisfaction_next;		// 0x14C
	// Various flags stating whether a window needs to be refreshed
	uint8 window_invalidate_flags;	// 0x14D
	uint8 pad_14E[0x02];
	uint32 total_customers;			// 0x150
	money32 total_profit;			// 0x154
	uint8 popularity;				// 0x158
	uint8 popularity_time_out;		// 0x159 Updated every purchase and ?possibly by time?
	uint8 popularity_next;			// 0x15A When timeout reached this will be the next popularity
	uint8 num_riders;				// 0x15B
	uint8 music_tune_id;			// 0x15C
	uint8 slide_in_use;				// 0x15D
	union {
		uint16 slide_peep;			// 0x15E
		uint16 maze_tiles;			// 0x15E
	};
	uint8 pad_160[0xE];
	uint8 slide_peep_t_shirt_colour;// 0x16E
	uint8 pad_16F[0x7];
	uint8 spiral_slide_progress;	// 0x176
	uint8 pad_177[0x9];
	sint16 build_date;				// 0x180
	money16 upkeep_cost;			// 0x182
	uint16 race_winner;				// 0x184
	uint8 pad_186[0x02];
	uint32 music_position;			// 0x188
	uint8 breakdown_reason_pending;	// 0x18C
	uint8 mechanic_status;			// 0x18D
	uint16 mechanic;				// 0x18E
	uint8 inspection_station;		// 0x190
	uint8 broken_vehicle;			// 0x191
	uint8 broken_car;				// 0x192
	uint8 breakdown_reason;			// 0x193
	money16 price_secondary;		// 0x194
	// Starts at RIDE_INITIAL_RELIABILITY and decreases from there. Right shift
	// this number by 8 to get a reliability percentage 0-100
	uint16 reliability;				// 0x196
	// Small constant used to increase the unreliability as the game continues,
	// making breakdowns more and more likely.
	uint8 unreliability_factor;		// 0x198
	// Range from [0, 100]
	uint8 downtime;                 // 0x199
	uint8 inspection_interval;		// 0x19A
	uint8 last_inspection;			// 0x19B
	uint8 downtime_history[8];		// 0x19C
	uint32 no_primary_items_sold;	// 0x1A4
	uint32 no_secondary_items_sold; // 0x1A8
	uint8 breakdown_sound_modifier;	// 0x1AC 
	// Used to oscilate the sound when ride breaks down.
	// 0 = no change, 255 = max change
	uint8 not_fixed_timeout;		// 0x1AD
	uint8 last_crash_type;			// 0x1AE
	uint8 connected_message_throttle;	// 0x1AF
	money32 income_per_hour;		// 0x1B0
	money32 profit;					// 0x1B4
	uint8 queue_time[4];			// 0x1B8
	uint8 track_colour_main[4];		// 0x1BC
	uint8 track_colour_additional[4];	// 0x1C0
	uint8 track_colour_supports[4];	// 0x1C4
	uint8 music;					// 0x1C8
	uint8 entrance_style;			// 0x1C9
	uint16 vehicle_change_timeout;	// 0x1CA
	uint8 num_block_brakes;			// 0x1CC
	uint8 lift_hill_speed;			// 0x1CD
	uint16 guests_favourite;		// 0x1CE
	uint32 lifecycle_flags;			// 0x1D0
	uint8 vehicle_colours_extended[32];	// 0x1D4
	uint16 total_air_time;			// 0x1F4
	uint8 current_test_station;		// 0x1F6
	uint8 num_circuits;				// 0x1F7
	sint16 cable_lift_x;			// 0x1F8
	sint16 cable_lift_y;			// 0x1FA
	uint8 cable_lift_z;				// 0x1FC
	uint8 pad_1FD;
	uint16 cable_lift;				// 0x1FE
	uint16 queue_length[4];			// 0x200
	uint8 pad_208[0x58];
} rct_ride;
assert_struct_size(rct_ride, 0x260);

#define RIDE_MEASUREMENT_MAX_ITEMS 4800

/**
 * Ride measurement structure.
 * size: 0x04B0C
 */
typedef struct rct_ride_measurement {
	uint8 ride_index;							// 0x0000
	uint8 flags;								// 0x0001
	uint32 last_use_tick;						// 0x0002
	uint16 num_items;							// 0x0006
	uint16 current_item;						// 0x0008
	uint8 vehicle_index;						// 0x000A
	uint8 current_station;						// 0x000B
	sint8 vertical[RIDE_MEASUREMENT_MAX_ITEMS];	// 0x000C
	sint8 lateral[RIDE_MEASUREMENT_MAX_ITEMS];	// 0x12CC
	uint8 velocity[RIDE_MEASUREMENT_MAX_ITEMS];	// 0x258C
	uint8 altitude[RIDE_MEASUREMENT_MAX_ITEMS];	// 0x384C
} rct_ride_measurement;
assert_struct_size(rct_ride_measurement, 0x4b0c);

typedef struct track_begin_end {
	int begin_x;
	int begin_y;
	int begin_z;
	int begin_direction;
	rct_map_element *begin_element;
	int end_x;
	int end_y;
	int end_direction;
	rct_map_element *end_element;
} track_begin_end;
#ifdef PLATFORM_32BIT
assert_struct_size(track_begin_end, 36);
#endif

#pragma pack(pop)

/*
 * This array should probably be only 91 + 128 * 3 = 475 bytes long.
 * It was originally stored at address 0x009E32F8 and continued until 0x009E34E3
 * (inclusive). 0x009E34E4 is the address of s6 header, so it's likely it had
 * some padding at the end as well.
 */
#define TYPE_TO_RIDE_ENTRY_SLOTS 492
extern uint8 gTypeToRideEntryIndexMap[TYPE_TO_RIDE_ENTRY_SLOTS];

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
	RIDE_LIFECYCLE_BREAKDOWN_PENDING = 1 << 6,
	RIDE_LIFECYCLE_BROKEN_DOWN = 1 << 7,
	RIDE_LIFECYCLE_DUE_INSPECTION = 1 << 8,
	RIDE_LIFECYCLE_QUEUE_FULL = 1 << 9,
	RIDE_LIFECYCLE_CRASHED = 1 << 10,
	RIDE_LIFECYCLE_11 = 1 << 11,
	RIDE_LIFECYCLE_EVER_BEEN_OPENED = 1 << 12,
	RIDE_LIFECYCLE_MUSIC = 1 << 13,
	RIDE_LIFECYCLE_INDESTRUCTIBLE = 1 << 14,
	RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK = 1 << 15,
	RIDE_LIFECYCLE_16 = 1 << 16,
	RIDE_LIFECYCLE_CABLE_LIFT = 1 << 17,
	RIDE_LIFECYCLE_NOT_CUSTOM_DESIGN = 1 << 18, // Used for the Award for Best Custom-designed Rides
	RIDE_LIFECYCLE_SIX_FLAGS_DEPRECATED = 1 << 19 // Not used anymore
};

// Constants used by the ride_type->flags property at 0x008
enum {
	RIDE_ENTRY_FLAG_0 = 1 << 0, // 0x1
	RIDE_ENTRY_FLAG_NO_INVERSIONS = 1 << 1, // 0x2
	RIDE_ENTRY_FLAG_NO_BANKED_TRACK = 1 << 2, // 0x4
	RIDE_ENTRY_FLAG_3 = 1 << 3, // 0x8
	RIDE_ENTRY_FLAG_ALTERNATIVE_SWING_MODE_1 = 1 << 4, // 0x10
	// Twist type rotation ride
	RIDE_ENTRY_FLAG_ALTERNATIVE_ROTATION_MODE_1 = 1 << 5, // 0x20
	// Lifting arm rotation ride (enterprise)
	RIDE_ENTRY_FLAG_ALTERNATIVE_ROTATION_MODE_2 = 1 << 6, // 0x40
	RIDE_ENTRY_FLAG_7 = 1 << 7, // 0x80
	RIDE_ENTRY_FLAG_8 = 1 << 8, // 0x100
	RIDE_ENTRY_FLAG_9 = 1 << 9, // 0x200
	RIDE_ENTRY_FLAG_COVERED_RIDE = 1 << 10, // 0x400
	RIDE_ENTRY_FLAG_11 = 1 << 11, // 0x800
	RIDE_ENTRY_FLAG_SEPARATE_RIDE_NAME = 1 << 12, // 0x1000
	RIDE_ENTRY_FLAG_SEPARATE_RIDE = 1 << 13, // 0x2000
	RIDE_ENTRY_FLAG_14 = 1 << 14, // 0x4000
	RIDE_ENTRY_DISABLE_LAST_OPERATING_MODE = 1 << 15, // 0x8000
	RIDE_ENTRY_FLAG_16 = 1 << 16, // 0x10000
	RIDE_ENTRY_DISABLE_FIRST_TWO_OPERATING_MODES = 1 << 17, // 0x20000
	RIDE_ENTRY_FLAG_18 = 1 << 18, // 0x40000
	RIDE_ENTRY_FLAG_19 = 1 << 19, // 0x80000
	// Must be set with swing mode 1 as well.
	RIDE_ENTRY_FLAG_ALTERNATIVE_SWING_MODE_2 = 1 << 20, // 0x100000
	RIDE_ENTRY_FLAG_21 = 1 << 21, // 0x200000
	RIDE_ENTRY_FLAG_22 = 1 << 22, // 0x400000
	RIDE_ENTRY_FLAG_23 = 1 << 23, // 0x800000
	RIDE_ENTRY_FLAG_24 = 1 << 24, // 0x1000000
	RIDE_ENTRY_FLAG_25 = 1 << 25, // 0x2000000
	RIDE_ENTRY_FLAG_26 = 1 << 26, // 0x4000000
	RIDE_ENTRY_FLAG_27 = 1 << 27, // 0x8000000
	RIDE_ENTRY_FLAG_28 = 1 << 28, // 0x10000000
	RIDE_ENTRY_FLAG_29 = 1 << 29, // 0x20000000
	RIDE_ENTRY_FLAG_30 = 1 << 30, // 0x40000000
	RIDE_ENTRY_FLAG_31 = 1 << 31, // 0x80000000
};

enum{
	RIDE_TESTING_SHELTERED = (1 << 0),
	RIDE_TESTING_TURN_LEFT = (1 << 1),
	RIDE_TESTING_TURN_RIGHT = (1 << 2),
	RIDE_TESTING_TURN_BANKED = (1 << 3),
	RIDE_TESTING_TURN_SLOPED = (1 << 4),
	RIDE_TESTING_DROP_DOWN = (1 << 5),
	RIDE_TESTING_POWERED_LIFT = (1 << 6),
	RIDE_TESTING_DROP_UP = (1 << 7),
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
	RIDE_TYPE_BOAT_RIDE,
	RIDE_TYPE_WOODEN_WILD_MOUSE,
	RIDE_TYPE_STEEPLECHASE = 10,
	RIDE_TYPE_CAR_RIDE,
	RIDE_TYPE_LAUNCHED_FREEFALL,
	RIDE_TYPE_BOBSLEIGH_COASTER,
	RIDE_TYPE_OBSERVATION_TOWER,
	RIDE_TYPE_LOOPING_ROLLER_COASTER,
	RIDE_TYPE_DINGHY_SLIDE,
	RIDE_TYPE_MINE_TRAIN_COASTER,
	RIDE_TYPE_CHAIRLIFT,
	RIDE_TYPE_CORKSCREW_ROLLER_COASTER,
	RIDE_TYPE_MAZE = 20,
	RIDE_TYPE_SPIRAL_SLIDE,
	RIDE_TYPE_GO_KARTS,
	RIDE_TYPE_LOG_FLUME,
	RIDE_TYPE_RIVER_RAPIDS,
	RIDE_TYPE_DODGEMS,
	RIDE_TYPE_PIRATE_SHIP,
	RIDE_TYPE_SWINGING_INVERTER_SHIP,
	RIDE_TYPE_FOOD_STALL,
	RIDE_TYPE_1D,
	RIDE_TYPE_DRINK_STALL = 30,
	RIDE_TYPE_1F,
	RIDE_TYPE_SHOP,
	RIDE_TYPE_MERRY_GO_ROUND,
	RIDE_TYPE_22,
	RIDE_TYPE_INFORMATION_KIOSK,
	RIDE_TYPE_TOILETS,
	RIDE_TYPE_FERRIS_WHEEL,
	RIDE_TYPE_MOTION_SIMULATOR,
	RIDE_TYPE_3D_CINEMA,
	RIDE_TYPE_TOP_SPIN = 40,
	RIDE_TYPE_SPACE_RINGS,
	RIDE_TYPE_REVERSE_FREEFALL_COASTER,
	RIDE_TYPE_LIFT,
	RIDE_TYPE_VERTICAL_DROP_ROLLER_COASTER,
	RIDE_TYPE_CASH_MACHINE,
	RIDE_TYPE_TWIST,
	RIDE_TYPE_HAUNTED_HOUSE,
	RIDE_TYPE_FIRST_AID,
	RIDE_TYPE_CIRCUS_SHOW,
	RIDE_TYPE_GHOST_TRAIN = 50,
	RIDE_TYPE_TWISTER_ROLLER_COASTER,
	RIDE_TYPE_WOODEN_ROLLER_COASTER,
	RIDE_TYPE_SIDE_FRICTION_ROLLER_COASTER,
	RIDE_TYPE_WILD_MOUSE,
	RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER,
	RIDE_TYPE_38,
	RIDE_TYPE_FLYING_ROLLER_COASTER,
	RIDE_TYPE_3A,
	RIDE_TYPE_VIRGINIA_REEL,
	RIDE_TYPE_SPLASH_BOATS = 60,
	RIDE_TYPE_MINI_HELICOPTERS,
	RIDE_TYPE_LAY_DOWN_ROLLER_COASTER,
	RIDE_TYPE_SUSPENDED_MONORAIL,
	RIDE_TYPE_40,
	RIDE_TYPE_REVERSER_ROLLER_COASTER,
	RIDE_TYPE_HEARTLINE_TWISTER_COASTER,
	RIDE_TYPE_MINI_GOLF,
	RIDE_TYPE_GIGA_COASTER,
	RIDE_TYPE_ROTO_DROP,
	RIDE_TYPE_FLYING_SAUCERS = 70,
	RIDE_TYPE_CROOKED_HOUSE,
	RIDE_TYPE_MONORAIL_CYCLES,
	RIDE_TYPE_COMPACT_INVERTED_COASTER,
	RIDE_TYPE_WATER_COASTER,
	RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER,
	RIDE_TYPE_INVERTED_HAIRPIN_COASTER,
	RIDE_TYPE_MAGIC_CARPET,
	RIDE_TYPE_SUBMARINE_RIDE,
	RIDE_TYPE_RIVER_RAFTS,
	RIDE_TYPE_50 = 80,
	RIDE_TYPE_ENTERPRISE,
	RIDE_TYPE_52,
	RIDE_TYPE_53,
	RIDE_TYPE_54,
	RIDE_TYPE_55,
	RIDE_TYPE_INVERTED_IMPULSE_COASTER,
	RIDE_TYPE_MINI_ROLLER_COASTER,
	RIDE_TYPE_MINE_RIDE,
	RIDE_TYPE_59,
	RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER = 90
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
	RIDE_MODE_POWERED_LAUNCH_PASSTROUGH,						// RCT2 style, pass through station
	RIDE_MODE_SHUTTLE,
	RIDE_MODE_BOAT_HIRE,
	RIDE_MODE_UPWARD_LAUNCH,
	RIDE_MODE_ROTATING_LIFT,
	RIDE_MODE_STATION_TO_STATION,
	RIDE_MODE_SINGLE_RIDE_PER_ADMISSION,
	RIDE_MODE_UNLIMITED_RIDES_PER_ADMISSION = 10,
	RIDE_MODE_MAZE,
	RIDE_MODE_RACE,
	RIDE_MODE_BUMPERCAR,
	RIDE_MODE_SWING,
	RIDE_MODE_SHOP_STALL,
	RIDE_MODE_ROTATION,
	RIDE_MODE_FORWARD_ROTATION,
	RIDE_MODE_BACKWARD_ROTATION,
	RIDE_MODE_FILM_AVENGING_AVIATORS,
	RIDE_MODE_3D_FILM_MOUSE_TAILS = 20,
	RIDE_MODE_SPACE_RINGS,
	RIDE_MODE_BEGINNERS,
	RIDE_MODE_LIM_POWERED_LAUNCH,
	RIDE_MODE_FILM_THRILL_RIDERS,
	RIDE_MODE_3D_FILM_STORM_CHASERS,
	RIDE_MODE_3D_FILM_SPACE_RAIDERS,
	RIDE_MODE_INTENSE,
	RIDE_MODE_BERSERK,
	RIDE_MODE_HAUNTED_HOUSE,
	RIDE_MODE_CIRCUS_SHOW = 30,
	RIDE_MODE_DOWNWARD_LAUNCH,
	RIDE_MODE_CROOKED_HOUSE,
	RIDE_MODE_FREEFALL_DROP,
	RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED,
	RIDE_MODE_POWERED_LAUNCH,					// RCT1 style, don't pass through station
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
	MUSIC_STYLE_CIRCUS_SHOW,
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
	MUSIC_STYLE_CANDY_STYLE,
	MUSIC_STYLE_COUNT
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
	RIDE_MECHANIC_STATUS_UNDEFINED,
	RIDE_MECHANIC_STATUS_CALLING,
	RIDE_MECHANIC_STATUS_HEADING,
	RIDE_MECHANIC_STATUS_FIXING,
	RIDE_MECHANIC_STATUS_4
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
	WAIT_FOR_LOAD_QUARTER,
	WAIT_FOR_LOAD_HALF,
	WAIT_FOR_LOAD_THREE_QUARTER,
	WAIT_FOR_LOAD_FULL,
	WAIT_FOR_LOAD_ANY,
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
	RIDE_ENTRANCE_STYLE_SPACE,
	RIDE_ENTRANCE_STYLE_NONE,

	RIDE_ENTRANCE_STYLE_COUNT
};

enum {
	RIDE_INSPECTION_EVERY_10_MINUTES,
	RIDE_INSPECTION_EVERY_20_MINUTES,
	RIDE_INSPECTION_EVERY_30_MINUTES,
	RIDE_INSPECTION_EVERY_45_MINUTES,
	RIDE_INSPECTION_EVERY_HOUR,
	RIDE_INSPECTION_EVERY_2_HOURS,
	RIDE_INSPECTION_NEVER
};

// Flags used by ride->window_invalidate_flags
enum {
	RIDE_INVALIDATE_RIDE_CUSTOMER    = 1,
	RIDE_INVALIDATE_RIDE_INCOME      = 1 << 1,
	RIDE_INVALIDATE_RIDE_MAIN        = 1 << 2,
	RIDE_INVALIDATE_RIDE_LIST        = 1 << 3,
	RIDE_INVALIDATE_RIDE_OPERATING   = 1 << 4,
	RIDE_INVALIDATE_RIDE_MAINTENANCE = 1 << 5,
};

enum {
	RIDE_MEASUREMENT_FLAG_RUNNING = 1 << 0,
	RIDE_MEASUREMENT_FLAG_UNLOADING = 1 << 1,
	RIDE_MEASUREMENT_FLAG_G_FORCES = 1 << 2
};

// Constants for ride->special_track_elements
enum {
    RIDE_ELEMENT_TUNNEL_SPLASH_OR_RAPIDS = 1 << 5,
    RIDE_ELEMENT_REVERSER_OR_WATERFALL   = 1 << 6,
    RIDE_ELEMENT_WHIRLPOOL               = 1 << 7
};

enum {
	RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_MAIN = 1 << 0,
	RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_ADDITIONAL = 1 << 1,
	RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_SUPPORTS = 1 << 2,
	RIDE_TYPE_FLAG_3 = 1 << 3,
	RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION = 1 << 4,
	RIDE_TYPE_FLAG_CAN_SYNCHRONISE_ADJACENT_STATIONS = 1 << 5,
	RIDE_TYPE_FLAG_6 = 1 << 6,									// used only by boat ride and submarine ride
	RIDE_TYPE_FLAG_HAS_G_FORCES = 1 << 7,
	RIDE_TYPE_FLAG_CANNOT_HAVE_GAPS = 1 << 8,					// used by rides that can't have gaps, like those with a vertical tower, such as the observation tower
	RIDE_TYPE_FLAG_HAS_DATA_LOGGING = 1 << 9,
	RIDE_TYPE_FLAG_HAS_DROPS = 1 << 10,
	RIDE_TYPE_FLAG_NO_TEST_MODE = 1 << 11,
	RIDE_TYPE_FLAG_TRACK_ELEMENTS_HAVE_TWO_VARIETIES = 1 << 12,	// used by rides with two variaties, like the u and o shapes of the dinghy slide and the dry and submerged track of the water coaster
	RIDE_TYPE_FLAG_13 = 1 << 13,								// used only by maze, spiral slide and shops
	RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS = 1 << 14,
	RIDE_TYPE_FLAG_HAS_NO_TRACK = 1 << 15,
	RIDE_TYPE_FLAG_16 = 1 << 16,								// something to do with vehicle colour scheme
	RIDE_TYPE_FLAG_IS_SHOP = 1 << 17,
	RIDE_TYPE_FLAG_18 = 1 << 18,
	RIDE_TYPE_FLAG_FLAT_RIDE = 1 << 19,
	RIDE_TYPE_FLAG_20 = 1 << 20,
	RIDE_TYPE_FLAG_PEEP_SHOULD_GO_INSIDE_FACILITY = 1 << 21,	// used by toilets and first aid to mark that peep should go inside the building (rather than 'buying' at the counter)
	RIDE_TYPE_FLAG_IN_RIDE = 1 << 22,							// peeps are "IN" (ride) rather than "ON" (ride)
	RIDE_TYPE_FLAG_SELLS_FOOD = 1 << 23,
	RIDE_TYPE_FLAG_SELLS_DRINKS = 1 << 24,
	RIDE_TYPE_FLAG_IS_BATHROOM = 1 << 25,
	RIDE_TYPE_FLAG_26 = 1 << 26,								// something to do with vehicle colours
	RIDE_TYPE_FLAG_27 = 1 << 27,
	RIDE_TYPE_FLAG_HAS_TRACK = 1 << 28,
	RIDE_TYPE_FLAG_29 = 1 << 29,								// used only by lift
	RIDE_TYPE_FLAG_30 = 1 << 30,
	RIDE_TYPE_FLAG_SUPPORTS_MULTIPLE_TRACK_COLOUR = 1 << 31,
};

enum {
	RIDE_CRASH_TYPE_NONE = 0,
	RIDE_CRASH_TYPE_NO_FATALITIES = 2,
	RIDE_CRASH_TYPE_FATALITIES = 8
};

enum {
	RIDE_CONSTRUCTION_STATE_0,
	RIDE_CONSTRUCTION_STATE_FRONT,
	RIDE_CONSTRUCTION_STATE_BACK,
	RIDE_CONSTRUCTION_STATE_SELECTED,
	RIDE_CONSTRUCTION_STATE_PLACE,
	RIDE_CONSTRUCTION_STATE_ENTRANCE_EXIT,
	RIDE_CONSTRUCTION_STATE_MAZE_BUILD,
	RIDE_CONSTRUCTION_STATE_MAZE_MOVE,
	RIDE_CONSTRUCTION_STATE_MAZE_FILL
};

enum {
	RIDE_SET_VEHICLES_COMMAND_TYPE_NUM_TRAINS,
	RIDE_SET_VEHICLES_COMMAND_TYPE_NUM_CARS_PER_TRAIN,
	RIDE_SET_VEHICLES_COMMAND_TYPE_RIDE_ENTRY
};

enum {
	SHOP_ITEM_BALLOON,
	SHOP_ITEM_TOY,
	SHOP_ITEM_MAP,
	SHOP_ITEM_PHOTO,
	SHOP_ITEM_UMBRELLA,
	SHOP_ITEM_DRINK,
	SHOP_ITEM_BURGER,
	SHOP_ITEM_FRIES,
	SHOP_ITEM_ICE_CREAM,
	SHOP_ITEM_COTTON_CANDY,
	SHOP_ITEM_EMPTY_CAN,
	SHOP_ITEM_RUBBISH,
	SHOP_ITEM_EMPTY_BURGER_BOX,
	SHOP_ITEM_PIZZA,
	SHOP_ITEM_VOUCHER,
	SHOP_ITEM_POPCORN,
	SHOP_ITEM_HOT_DOG,
	SHOP_ITEM_TENTACLE,
	SHOP_ITEM_HAT,
	SHOP_ITEM_CANDY_APPLE,
	SHOP_ITEM_TSHIRT,
	SHOP_ITEM_DONUT,
	SHOP_ITEM_COFFEE,
	SHOP_ITEM_EMPTY_CUP,
	SHOP_ITEM_CHICKEN,
	SHOP_ITEM_LEMONADE,
	SHOP_ITEM_EMPTY_BOX,
	SHOP_ITEM_EMPTY_BOTTLE,
	SHOP_ITEM_ADMISSION = 31,
	SHOP_ITEM_PHOTO2 = 32,
	SHOP_ITEM_PHOTO3,
	SHOP_ITEM_PHOTO4,
	SHOP_ITEM_PRETZEL,
	SHOP_ITEM_CHOCOLATE,
	SHOP_ITEM_ICED_TEA,
	SHOP_ITEM_FUNNEL_CAKE,
	SHOP_ITEM_SUNGLASSES,
	SHOP_ITEM_BEEF_NOODLES,
	SHOP_ITEM_FRIED_RICE_NOODLES,
	SHOP_ITEM_WONTON_SOUP,
	SHOP_ITEM_MEATBALL_SOUP,
	SHOP_ITEM_FRUIT_JUICE,
	SHOP_ITEM_SOYBEAN_MILK,
	SHOP_ITEM_SU_JONGKWA,
	SHOP_ITEM_SUB_SANDWICH,
	SHOP_ITEM_COOKIE,
	SHOP_ITEM_EMPTY_BOWL_RED,
	SHOP_ITEM_EMPTY_DRINK_CARTON,
	SHOP_ITEM_EMPTY_JUICE_CUP,
	SHOP_ITEM_ROAST_SAUSAGE,
	SHOP_ITEM_EMPTY_BOWL_BLUE,
	SHOP_ITEM_COUNT = 56,
	SHOP_ITEM_NONE = 255
};

enum {
	RIDE_SETTING_MODE,
	RIDE_SETTING_DEPARTURE,
	RIDE_SETTING_MIN_WAITING_TIME,
	RIDE_SETTING_MAX_WAITING_TIME,
	RIDE_SETTING_OPERATION_OPTION,
	RIDE_SETTING_INSPECTION_INTERVAL,
	RIDE_SETTING_MUSIC,
	RIDE_SETTING_MUSIC_TYPE,
	RIDE_SETTING_LIFT_HILL_SPEED,
	RIDE_SETTING_NUM_CIRCUITS,
};

#define MAX_RIDES 255

#define MAX_RIDE_MEASUREMENTS 8
#define RIDE_VALUE_UNDEFINED 0xFFFF
#define RIDE_INITIAL_RELIABILITY ((100 << 8) - 1)

#define STATION_DEPART_FLAG (1 << 7)
#define STATION_DEPART_MASK (~STATION_DEPART_FLAG)

#define CURRENT_TURN_COUNT_MASK 0xF800
#define TURN_MASK_1_ELEMENT		0x001F
#define TURN_MASK_2_ELEMENTS	0x00E0
#define TURN_MASK_3_ELEMENTS	0x0700
#define TURN_MASK_4_PLUS_ELEMENTS 0xF800

/** Helper macros until rides are stored in this module. */
rct_ride *get_ride(int index);
rct_ride_entry *get_ride_entry(int index);
void get_ride_entry_name(char *name, int index);
rct_ride_measurement *get_ride_measurement(int index);

/**
 * Helper macro loop for enumerating through all the non null rides.
 */
#define FOR_ALL_RIDES(i, ride) \
	for (i = 0; i < MAX_RIDES; i++) \
		if ((ride = get_ride(i))->type != RIDE_TYPE_NULL)

#define gTotalRideValue				RCT2_GLOBAL(RCT2_TOTAL_RIDE_VALUE, money16)
#define gSamePriceThroughoutParkA	RCT2_GLOBAL(RCT2_ADDRESS_SAME_PRICE_THROUGHOUT, uint32)
#define gSamePriceThroughoutParkB	RCT2_GLOBAL(RCT2_ADDRESS_SAME_PRICE_THROUGHOUT_EXTENDED, uint32)

extern const uint8 gRideClassifications[255];

extern rct_ride *gRideList;
extern rct_ride_measurement *gRideMeasurements;

extern money32 _currentTrackPrice;

extern uint16 _numCurrentPossibleRideConfigurations;
extern uint16 _numCurrentPossibleSpecialTrackPieces;

extern uint16 _currentTrackCurve;
extern uint16 _currentTrackEndX;
extern uint16 _currentTrackEndY;
extern uint8 _rideConstructionState;
extern uint8 _currentRideIndex;
extern uint16 _currentTrackBeginX;
extern uint16 _currentTrackBeginY;
extern uint16 _currentTrackBeginZ;
extern uint8 _currentTrackPieceDirection;
extern uint8 _currentTrackPieceType;
extern uint8 _currentTrackSelectionFlags;
extern sint8 _rideConstructionArrowPulseTime;
extern uint8 _currentTrackSlopeEnd;
extern uint8 _currentTrackBankEnd;
extern uint8 _currentTrackLiftHill;
extern uint8 _currentTrackCovered;

extern uint8 _previousTrackBankEnd;
extern uint8 _previousTrackSlopeEnd;

extern uint16 _previousTrackPieceX;
extern uint16 _previousTrackPieceY;
extern uint16 _previousTrackPieceZ;

extern uint8 _currentSeatRotationAngle;

extern uint8 gRideEntranceExitPlaceType;
extern uint8 gRideEntranceExitPlaceRideIndex;
extern uint8 gRideEntranceExitPlaceStationIndex;
extern uint8 gRideEntranceExitPlacePreviousRideConstructionState;
extern uint8 gRideEntranceExitPlaceDirection;

extern bool gGotoStartPlacementMode;
extern int gRideRemoveTrackPieceCallbackX;
extern int gRideRemoveTrackPieceCallbackY;
extern int gRideRemoveTrackPieceCallbackZ;
extern int gRideRemoveTrackPieceCallbackDirection;
extern int gRideRemoveTrackPieceCallbackType;

int ride_get_count();
int ride_get_total_queue_length(rct_ride *ride);
int ride_get_max_queue_time(rct_ride *ride);
void ride_init_all();
void reset_all_ride_build_dates();
void ride_update_favourited_stat();
void ride_update_all();
void ride_check_all_reachable();
void ride_update_satisfaction(rct_ride* ride, uint8 happiness);
void ride_update_popularity(rct_ride* ride, uint8 pop_amount);
money32 get_shop_item_cost(int shopItem);
money16 get_shop_base_value(int shopItem);
money16 get_shop_hot_value(int shopItem);
money16 get_shop_cold_value(int shopItem);
bool ride_try_get_origin_element(int rideIndex, rct_xy_element *output);
int ride_find_track_gap(rct_xy_element *input, rct_xy_element *output);
void ride_construct_new(ride_list_item listItem);
void ride_construct(int rideIndex);
int ride_modify(rct_xy_element *input);
void ride_get_status(int rideIndex, int *formatSecondary, int *argument);
rct_peep *ride_get_assigned_mechanic(rct_ride *ride);
int ride_get_total_length(rct_ride *ride);
int ride_get_total_time(rct_ride *ride);
int ride_can_have_multiple_circuits(rct_ride *ride);
track_colour ride_get_track_colour(rct_ride *ride, int colourScheme);
vehicle_colour ride_get_vehicle_colour(rct_ride *ride, int vehicleIndex);
rct_ride_entry *get_ride_entry_by_ride(rct_ride *ride);
uint8 *get_ride_entry_indices_for_ride_type(uint8 rideType);
void reset_type_to_ride_entry_index_map();
void ride_measurement_clear(rct_ride *ride);
void ride_measurements_update();
rct_ride_measurement *ride_get_measurement(int rideIndex, rct_string_id *message);
void ride_breakdown_add_news_item(int rideIndex);
rct_peep *ride_find_closest_mechanic(rct_ride *ride, int forInspection);
int sub_6CC3FB(int rideIndex);
void sub_6C9627();
int sub_6C683D(int* x, int* y, int* z, int direction, int type, uint16 extra_params, rct_map_element** output_element, uint16 flags);
void ride_set_map_tooltip(rct_map_element *mapElement);
int ride_music_params_update(sint16 x, sint16 y, sint16 z, uint8 rideIndex, uint16 sampleRate, uint32 position, uint8 *tuneId);
void ride_music_update_final();
void ride_prepare_breakdown(int rideIndex, int breakdownReason);
rct_map_element *ride_get_station_start_track_element(rct_ride *ride, int stationIndex);
rct_map_element *ride_get_station_exit_element(rct_ride *ride, int x, int y, int z);
void ride_set_status(int rideIndex, int status);
void game_command_set_ride_status(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);
void ride_set_name(int rideIndex, const char *name);
void game_command_set_ride_name(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);
void game_command_set_ride_setting(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);
int ride_get_refund_price(int ride_id);
bool shop_item_has_common_price(int shopItem);
void game_command_create_ride(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);
void game_command_callback_ride_construct_new(int eax, int ebx, int ecx, int edx, int esi, int edi, int ebp);
void game_command_callback_ride_construct_placed_front(int eax, int ebx, int ecx, int edx, int esi, int edi, int ebp);
void game_command_callback_ride_construct_placed_back(int eax, int ebx, int ecx, int edx, int esi, int edi, int ebp);
void game_command_callback_ride_remove_track_piece(int eax, int ebx, int ecx, int edx, int esi, int edi, int ebp);
void game_command_demolish_ride(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);
void game_command_set_ride_appearance(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);
void game_command_set_ride_price(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);
money32 ride_create_command(int type, int subType, int flags, uint8 *outRideIndex, uint8 *outRideColour);

void ride_clear_for_construction(int rideIndex);
void set_vehicle_type_image_max_sizes(rct_ride_entry_vehicle* vehicle_type, int num_images);
void invalidate_test_results(int rideIndex);

void ride_select_next_section();
void ride_select_previous_section();

void increment_turn_count_1_element(rct_ride* ride, uint8 type);
void increment_turn_count_2_elements(rct_ride* ride, uint8 type);
void increment_turn_count_3_elements(rct_ride* ride, uint8 type);
void increment_turn_count_4_plus_elements(rct_ride* ride, uint8 type);
int get_turn_count_1_element(rct_ride* ride, uint8 type);
int get_turn_count_2_elements(rct_ride* ride, uint8 type);
int get_turn_count_3_elements(rct_ride* ride, uint8 type);
int get_turn_count_4_plus_elements(rct_ride* ride, uint8 type);

uint8 ride_get_helix_sections(rct_ride *ride);
bool ride_has_spinning_tunnel(rct_ride *ride);
bool ride_has_water_splash(rct_ride *ride);
bool ride_has_rapids(rct_ride *ride);
bool ride_has_log_reverser(rct_ride *ride);
bool ride_has_waterfall(rct_ride *ride);
bool ride_has_whirlpool(rct_ride *ride);

bool ride_type_has_flag(int rideType, int flag);
bool ride_is_powered_launched(rct_ride *ride);
bool ride_is_block_sectioned(rct_ride *ride);
bool ride_has_any_track_elements(int rideIndex);
void ride_all_has_any_track_elements(bool *rideIndexArray);

void ride_construction_set_default_next_piece();

bool track_block_get_next(rct_xy_element *input, rct_xy_element *output, int *z, int *direction);
bool track_block_get_next_from_zero(sint16 x, sint16 y, sint16 z_start, uint8 rideIndex, uint8 direction_start, rct_xy_element *output, int *z, int *direction);

bool track_block_get_previous(int x, int y, rct_map_element *mapElement, track_begin_end *outTrackBeginEnd);
bool track_block_get_previous_from_zero(sint16 x, sint16 y, sint16 z, uint8 rideIndex, uint8 direction, track_begin_end *outTrackBeginEnd);

void sub_6C84CE();
void sub_6C96C0();
money32 ride_get_entrance_or_exit_price(int rideIndex, int x, int y, int direction, int dh, int di);
void ride_get_entrance_or_exit_position_from_screen_position(int x, int y, int *outX, int *outY, int *outDirection);

bool ride_select_backwards_from_front();
bool ride_select_forwards_from_back();

money32 ride_remove_track_piece(int x, int y, int z, int direction, int type, uint8 flags);

bool ride_are_all_possible_entrances_and_exits_built(rct_ride *ride);
void ride_fix_breakdown(int rideIndex, int reliabilityIncreaseFactor);

void ride_entry_get_train_layout(int rideEntryIndex, int numCarsPerTrain, uint8 *trainLayout);
uint8 ride_entry_get_vehicle_at_position(int rideEntryIndex, int numCarsPerTrain, int position);
void ride_update_max_vehicles(int rideIndex);

void ride_set_ride_entry(int rideIndex, int rideEntry);
void ride_set_num_vehicles(int rideIndex, int numVehicles);
void ride_set_num_cars_per_vehicle(int rideIndex, int numCarsPerVehicle);
void game_command_set_ride_vehicles(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);

void game_command_place_ride_entrance_or_exit(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);
void game_command_remove_ride_entrance_or_exit(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);

void ride_set_to_default_inspection_interval(int rideIndex);

void sub_6CB945(int rideIndex);
void ride_crash(uint8 rideIndex, uint8 vehicleIndex);

void sub_6C94D8();

bool shop_item_is_food_or_drink(int shopItem);
bool shop_item_is_food(int shopItem);
bool shop_item_is_drink(int shopItem);
bool shop_item_is_souvenir(int shopItem);
void ride_reset_all_names();
const uint8* ride_seek_available_modes(rct_ride *ride);

void window_ride_construction_mouseup_demolish_next_piece(int x, int y, int z, int direction, int type);

const uint32 ride_customers_per_hour(const rct_ride *ride);
const uint32 ride_customers_in_last_5_minutes(const rct_ride *ride);

rct_vehicle * ride_get_broken_vehicle(rct_ride *ride);

void window_ride_construction_do_station_check();
void window_ride_construction_do_entrance_exit_check();
void game_command_callback_place_ride_entrance_or_exit(int eax, int ebx, int ecx, int edx, int esi, int edi, int ebp);

void ride_delete(uint8 rideIndex);

#endif
