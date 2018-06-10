#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#include "../common.h"
#include "../peep/Peep.h"
#include "../rct12/RCT12.h"
#include "../rct2/RCT2.h"
#include "../world/Map.h"
#include "RideRatings.h"
#include "Vehicle.h"

interface IObjectManager;

#define MAX_RIDE_TYPES_PER_RIDE_ENTRY   3
// The max number of different types of vehicle.
// Examples of vehicles here are the locomotive, tender and carriage of the Miniature Railway.
#define MAX_VEHICLES_PER_RIDE_ENTRY     4
#define MAX_VEHICLES_PER_RIDE           32
#define RIDE_ENTRY_INDEX_NULL           255
#define NUM_COLOUR_SCHEMES              4
#define MAX_CATEGORIES_PER_RIDE         2
#define DOWNTIME_HISTORY_SIZE           8
#define CUSTOMER_HISTORY_SIZE           10
#define MAX_CARS_PER_TRAIN              255
#define MAX_STATIONS                    4
#define RIDE_MEASUREMENT_MAX_ITEMS      4800
#define MAX_RIDES                       255
#define RIDE_ID_NULL                    255
#define RIDE_ADJACENCY_CHECK_DISTANCE   5

#pragma pack(push, 1)

/**
 * Couples a ride type and subtype together.
 */
struct ride_list_item {
    union {
        struct {
            uint8 type;
            uint8 entry_index;
        };
        uint16 ride_type_and_entry;
    };
};
assert_struct_size(ride_list_item, 2);

struct track_colour {
    uint8 main;
    uint8 additional;
    uint8 supports;
};
assert_struct_size(track_colour, 3);

struct vehicle_colour {
    uint8 main;
    uint8 additional_1;
    uint8 additional_2;
};
assert_struct_size(vehicle_colour, 3);

struct track_colour_preset_list {
    uint8 count;
    track_colour list[256];
};
assert_struct_size(track_colour_preset_list, (1 + 256 * 3));

struct vehicle_colour_preset_list {
    uint8 count;
    vehicle_colour list[256];
};
assert_struct_size(vehicle_colour_preset_list, (1 + 256 * 3));

struct rct_ride_name {
    rct_string_id name;
    rct_string_id description;
};
assert_struct_size(rct_ride_name, 4);

/**
 * Ride type structure.
 * size: unknown
 */
struct rct_ride_entry {
    rct_ride_name naming;
    uint32 images_offset;                               // 0x004. The first three images are previews. They correspond to the ride_type[] array.
    uint32 flags;                                       // 0x008
    uint8 ride_type[RCT2_MAX_RIDE_TYPES_PER_RIDE_ENTRY];// 0x00C
    uint8 min_cars_in_train;                            // 0x00F
    uint8 max_cars_in_train;                            // 0x010
    uint8 cars_per_flat_ride;                           // 0x011
    // Number of cars that can't hold passengers
    uint8 zero_cars;                                    // 0x012
    // The index to the vehicle type displayed in
    // the vehicle tab.
    uint8 tab_vehicle;                                  // 0x013
    uint8 default_vehicle;                              // 0x014
    // Convert from first - fourth vehicle to
    // vehicle structure
    uint8 front_vehicle;                                // 0x015
    uint8 second_vehicle;                               // 0x016
    uint8 rear_vehicle;                                 // 0x017
    uint8 third_vehicle;                                // 0x018
    uint8 pad_019;                                      // 0x019
    rct_ride_entry_vehicle vehicles[RCT2_MAX_VEHICLES_PER_RIDE_ENTRY]; // 0x01A
    vehicle_colour_preset_list *vehicle_preset_list;    // 0x1AE
    sint8 excitement_multiplier;                        // 0x1B2
    sint8 intensity_multiplier;                         // 0x1B3
    sint8 nausea_multiplier;                            // 0x1B4
    uint8 max_height;                                   // 0x1B5
    uint64 enabledTrackPieces;                          // 0x1B6
    uint8 category[RCT2_MAX_CATEGORIES_PER_RIDE];       // 0x1BE
    uint8 shop_item;                                    // 0x1C0
    uint8 shop_item_secondary;                          // 0x1C1
    rct_string_id capacity;
    void * obj;
};

#pragma pack(pop)

/**
 * Ride structure.
 *
 * This is based on RCT2's ride structure.
 * Padding and no longer used fields have been removed.
 */
struct Ride 
{
    uint8 type;
    // pointer to static info. for example, wild mouse type is 0x36, subtype is
    // 0x4c.
    uint8 subtype;
    uint8 mode;
    uint8 colour_scheme_type;
    rct_vehicle_colour vehicle_colours[MAX_CARS_PER_TRAIN];
    // 0 = closed, 1 = open, 2 = test
    uint8 status;
    rct_string_id name;
    union {
        uint32 name_arguments;
        struct {
            rct_string_id name_arguments_type_name;
            uint16 name_arguments_number;
        };
    };
    LocationXY8 overall_view;
    LocationXY8 station_starts[MAX_STATIONS];
    uint8 station_heights[MAX_STATIONS];
    uint8 station_length[MAX_STATIONS];
    uint8 station_depart[MAX_STATIONS];
    // ride->vehicle index for current train waiting for passengers
    // at station
    uint8 train_at_station[MAX_STATIONS];
    TileCoordsXYZD entrances[MAX_STATIONS];
    TileCoordsXYZD exits[MAX_STATIONS];
    uint16 last_peep_in_queue[MAX_STATIONS];
    uint16 vehicles[MAX_VEHICLES_PER_RIDE];                         // Points to the first car in the train
    uint8 depart_flags;
    uint8 num_stations;
    uint8 num_vehicles;
    uint8 num_cars_per_train;
    uint8 proposed_num_vehicles;
    uint8 proposed_num_cars_per_train;
    uint8 max_trains;
    uint8 min_max_cars_per_train;
    uint8 min_waiting_time;
    uint8 max_waiting_time;
    union {
        uint8 operation_option;
        uint8 time_limit;
        uint8 num_laps;
        uint8 launch_speed;
        uint8 speed;
        uint8 rotations;
    };

    uint8 boat_hire_return_direction;
    LocationXY8 boat_hire_return_position;
    uint8 measurement_index;
    // bits 0 through 4 are the number of helix sections
    // bit 5: spinning tunnel, water splash, or rapids
    // bit 6: log reverser, waterfall
    // bit 7: whirlpool
    uint8 special_track_elements;
    // Divide this value by 29127 to get the human-readable max speed
    // (in RCT2, display_speed = (max_speed * 9) >> 18)
    sint32 max_speed;
    sint32 average_speed;
    uint8 current_test_segment;
    uint8 average_speed_test_timeout;
    sint32 length[MAX_STATIONS];
    uint16 time[MAX_STATIONS];
    fixed16_2dp max_positive_vertical_g;
    fixed16_2dp max_negative_vertical_g;
    fixed16_2dp max_lateral_g;
    fixed16_2dp previous_vertical_g;
    fixed16_2dp previous_lateral_g;
    uint32 testing_flags;
    // x y map location of the current track piece during a test
    // this is to prevent counting special tracks multiple times
    LocationXY8 cur_test_track_location;
    // Next 3 variables are related (XXXX XYYY ZZZa aaaa)
    uint16 turn_count_default;                                      // X = current turn count
    uint16 turn_count_banked;
    uint16 turn_count_sloped;                                       // X = number turns > 3 elements
    union {
        uint8 inversions;                                           // (???X XXXX)
        uint8 holes;                                                // (???X XXXX)
        // This is a very rough approximation of how much of the ride is undercover.
        // It reaches the maximum value of 7 at about 50% undercover and doesn't increase beyond that.
        uint8 sheltered_eighths;                                    // (XXX?-????)
    };
    // Y is number of powered lifts, X is drops
    uint8 drops;                                                    // (YYXX XXXX)
    uint8 start_drop_height;
    uint8 highest_drop_height;
    sint32 sheltered_length;
    // Unused always 0? Should affect nausea
    uint16 var_11C;
    uint8 num_sheltered_sections;                                   // (?abY YYYY)
    // see cur_test_track_location
    uint8 cur_test_track_z;
    // Customer counter in the current 960 game tick (about 30 seconds) interval
    uint16 cur_num_customers;
    // Counts ticks to update customer intervals, resets each 960 game ticks.
    uint16 num_customers_timeout;
    // Customer count in the last 10 * 960 game ticks (sliding window)
    uint16 num_customers[CUSTOMER_HISTORY_SIZE];
    money16 price;
    LocationXY8 chairlift_bullwheel_location[2];
    uint8 chairlift_bullwheel_z[2];
    union {
        rating_tuple ratings;
        struct {
            ride_rating excitement;
            ride_rating intensity;
            ride_rating nausea;
        };
    };
    uint16 value;
    uint16 chairlift_bullwheel_rotation;
    uint8 satisfaction;
    uint8 satisfaction_time_out;
    uint8 satisfaction_next;
    // Various flags stating whether a window needs to be refreshed
    uint8 window_invalidate_flags;
    uint32 total_customers;
    money32 total_profit;
    uint8 popularity;
    uint8 popularity_time_out;                                      // Updated every purchase and ?possibly by time?
    uint8 popularity_next;                                          // When timeout reached this will be the next popularity
    uint16 num_riders;
    uint8 music_tune_id;
    uint8 slide_in_use;
    union {
        uint16 slide_peep;
        uint16 maze_tiles;
    };
    uint8 slide_peep_t_shirt_colour;
    uint8 spiral_slide_progress;
    sint16 build_date;
    money16 upkeep_cost;
    uint16 race_winner;
    uint32 music_position;
    uint8 breakdown_reason_pending;
    uint8 mechanic_status;
    uint16 mechanic;
    uint8 inspection_station;
    uint8 broken_vehicle;
    uint8 broken_car;
    uint8 breakdown_reason;
    money16 price_secondary;
    union
    {
        struct
        {
            uint8 reliability_subvalue;                             // 0 - 255, acts like the decimals for reliability_percentage
            uint8 reliability_percentage;                           // Starts at 100 and decreases from there.
        };
        uint16 reliability;
    };
    // Small constant used to increase the unreliability as the game continues,
    // making breakdowns more and more likely.
    uint8 unreliability_factor;
    // Range from [0, 100]
    uint8 downtime;
    uint8 inspection_interval;
    uint8 last_inspection;
    uint8 downtime_history[DOWNTIME_HISTORY_SIZE];
    uint32 no_primary_items_sold;
    uint32 no_secondary_items_sold;
    uint8 breakdown_sound_modifier;
    // Used to oscillate the sound when ride breaks down.
    // 0 = no change, 255 = max change
    uint8 not_fixed_timeout;
    uint8 last_crash_type;
    uint8 connected_message_throttle;
    money32 income_per_hour;
    money32 profit;
    uint8 queue_time[MAX_STATIONS];
    uint8 track_colour_main[NUM_COLOUR_SCHEMES];
    uint8 track_colour_additional[NUM_COLOUR_SCHEMES];
    uint8 track_colour_supports[NUM_COLOUR_SCHEMES];
    uint8 music;
    uint8 entrance_style;
    uint16 vehicle_change_timeout;
    uint8 num_block_brakes;
    uint8 lift_hill_speed;
    uint16 guests_favourite;
    uint32 lifecycle_flags;
    uint8 vehicle_colours_extended[MAX_CARS_PER_TRAIN];
    uint16 total_air_time;
    uint8 current_test_station;
    uint8 num_circuits;
    sint16 cable_lift_x;
    sint16 cable_lift_y;
    uint8 cable_lift_z;
    uint16 cable_lift;
    uint16 queue_length[MAX_STATIONS];
};

#pragma pack(push, 1)

/**
 * Ride measurement structure.
 * size: 0x04B0C
 */
struct rct_ride_measurement {
    uint8 ride_index;                           // 0x0000
    uint8 flags;                                // 0x0001
    uint32 last_use_tick;                       // 0x0002
    uint16 num_items;                           // 0x0006
    uint16 current_item;                        // 0x0008
    uint8 vehicle_index;                        // 0x000A
    uint8 current_station;                      // 0x000B
    sint8 vertical[RIDE_MEASUREMENT_MAX_ITEMS]; // 0x000C
    sint8 lateral[RIDE_MEASUREMENT_MAX_ITEMS];  // 0x12CC
    uint8 velocity[RIDE_MEASUREMENT_MAX_ITEMS]; // 0x258C
    uint8 altitude[RIDE_MEASUREMENT_MAX_ITEMS]; // 0x384C
};
assert_struct_size(rct_ride_measurement, 0x4b0c);

struct track_begin_end {
    sint32 begin_x;
    sint32 begin_y;
    sint32 begin_z;
    sint32 begin_direction;
    rct_tile_element *begin_element;
    sint32 end_x;
    sint32 end_y;
    sint32 end_direction;
    rct_tile_element *end_element;
};
#ifdef PLATFORM_32BIT
assert_struct_size(track_begin_end, 36);
#endif

struct ride_name_args {
    uint16 type_name;
    uint16 number;
};
assert_struct_size(ride_name_args, 4);

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
    RIDE_LIFECYCLE_HAS_STALLED_VEHICLE = 1 << 11,
    RIDE_LIFECYCLE_EVER_BEEN_OPENED = 1 << 12,
    RIDE_LIFECYCLE_MUSIC = 1 << 13,
    RIDE_LIFECYCLE_INDESTRUCTIBLE = 1 << 14,
    RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK = 1 << 15,
    RIDE_LIFECYCLE_CABLE_LIFT_HILL_COMPONENT_USED = 1 << 16,
    RIDE_LIFECYCLE_CABLE_LIFT = 1 << 17,
    RIDE_LIFECYCLE_NOT_CUSTOM_DESIGN = 1 << 18, // Used for the Award for Best Custom-designed Rides
    RIDE_LIFECYCLE_SIX_FLAGS_DEPRECATED = 1 << 19 // Not used anymore
};

// Constants used by the ride_type->flags property at 0x008
enum {
    RIDE_ENTRY_FLAG_VEHICLE_TAB_SCALE_HALF          = 1 << 0,
    RIDE_ENTRY_FLAG_NO_INVERSIONS                   = 1 << 1,
    RIDE_ENTRY_FLAG_NO_BANKED_TRACK                 = 1 << 2,
    RIDE_ENTRY_FLAG_PLAY_DEPART_SOUND               = 1 << 3,
    RIDE_ENTRY_FLAG_ALTERNATIVE_SWING_MODE_1        = 1 << 4,
    // Twist type rotation ride
    RIDE_ENTRY_FLAG_ALTERNATIVE_ROTATION_MODE_1     = 1 << 5,
    // Lifting arm rotation ride (enterprise)
    RIDE_ENTRY_FLAG_ALTERNATIVE_ROTATION_MODE_2     = 1 << 6,
    RIDE_ENTRY_FLAG_7                               = 1 << 7,
    RIDE_ENTRY_FLAG_PLAY_SPLASH_SOUND               = 1 << 8,
    RIDE_ENTRY_FLAG_PLAY_SPLASH_SOUND_SLIDE         = 1 << 9,
    RIDE_ENTRY_FLAG_COVERED_RIDE                    = 1 << 10,
    RIDE_ENTRY_FLAG_LIMIT_AIRTIME_BONUS             = 1 << 11,
    RIDE_ENTRY_FLAG_SEPARATE_RIDE_NAME_DEPRECATED   = 1 << 12, // Always set with SEPARATE_RIDE, and deprecated in favour of it.
    RIDE_ENTRY_FLAG_SEPARATE_RIDE_DEPRECATED        = 1 << 13, // Made redundant by ride groups
    RIDE_ENTRY_FLAG_CANNOT_BREAK_DOWN               = 1 << 14,
    RIDE_ENTRY_DISABLE_LAST_OPERATING_MODE          = 1 << 15,
    RIDE_ENTRY_FLAG_16                              = 1 << 16,
    RIDE_ENTRY_DISABLE_FIRST_TWO_OPERATING_MODES    = 1 << 17,
    RIDE_ENTRY_FLAG_18                              = 1 << 18,
    RIDE_ENTRY_FLAG_DISABLE_COLOUR_TAB              = 1 << 19,
    // Must be set with swing mode 1 as well.
    RIDE_ENTRY_FLAG_ALTERNATIVE_SWING_MODE_2        = 1 << 20,
};

enum {
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
    RIDE_TYPE_BOAT_HIRE,
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
    RIDE_TYPE_SWINGING_SHIP,
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
    RIDE_TYPE_CIRCUS,
    RIDE_TYPE_GHOST_TRAIN = 50,
    RIDE_TYPE_TWISTER_ROLLER_COASTER,
    RIDE_TYPE_WOODEN_ROLLER_COASTER,
    RIDE_TYPE_SIDE_FRICTION_ROLLER_COASTER,
    RIDE_TYPE_STEEL_WILD_MOUSE,
    RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER,
    RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER_ALT,
    RIDE_TYPE_FLYING_ROLLER_COASTER,
    RIDE_TYPE_FLYING_ROLLER_COASTER_ALT,
    RIDE_TYPE_VIRGINIA_REEL,
    RIDE_TYPE_SPLASH_BOATS = 60,
    RIDE_TYPE_MINI_HELICOPTERS,
    RIDE_TYPE_LAY_DOWN_ROLLER_COASTER,
    RIDE_TYPE_SUSPENDED_MONORAIL,
    RIDE_TYPE_LAY_DOWN_ROLLER_COASTER_ALT,
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
    RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER = 90,

    RIDE_TYPE_COUNT
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
    RIDE_MODE_POWERED_LAUNCH_PASSTROUGH,                        // RCT2 style, pass through station
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
    RIDE_MODE_POWERED_LAUNCH,                   // RCT1 style, don't pass through station
    RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED
};

enum {
    RIDE_COLOUR_SCHEME_ALL_SAME,
    RIDE_COLOUR_SCHEME_DIFFERENT_PER_TRAIN,
    RIDE_COLOUR_SCHEME_DIFFERENT_PER_CAR
};

enum {
    RIDE_CATEGORY_TRANSPORT,
    RIDE_CATEGORY_GENTLE,
    RIDE_CATEGORY_ROLLERCOASTER,
    RIDE_CATEGORY_THRILL,
    RIDE_CATEGORY_WATER,
    RIDE_CATEGORY_SHOP
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
    BREAKDOWN_CONTROL_FAILURE,

    BREAKDOWN_COUNT
};

enum {
    RIDE_MECHANIC_STATUS_UNDEFINED,
    RIDE_MECHANIC_STATUS_CALLING,
    RIDE_MECHANIC_STATUS_HEADING,
    RIDE_MECHANIC_STATUS_FIXING,
    RIDE_MECHANIC_STATUS_HAS_FIXED_STATION_BRAKES
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

enum ride_type_flags : uint32
{
    RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_MAIN = 1 << 0,
    RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_ADDITIONAL = 1 << 1,
    RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_SUPPORTS = 1 << 2,
    RIDE_TYPE_FLAG_3 = 1 << 3,
    RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION = 1 << 4,
    RIDE_TYPE_FLAG_CAN_SYNCHRONISE_ADJACENT_STATIONS = 1 << 5,
    RIDE_TYPE_FLAG_TRACK_MUST_BE_ON_WATER = 1 << 6,             // used only by boat Hire and submarine ride
    RIDE_TYPE_FLAG_HAS_G_FORCES = 1 << 7,
    RIDE_TYPE_FLAG_CANNOT_HAVE_GAPS = 1 << 8,                   // used by rides that can't have gaps, like those with a vertical tower, such as the observation tower
    RIDE_TYPE_FLAG_HAS_DATA_LOGGING = 1 << 9,
    RIDE_TYPE_FLAG_HAS_DROPS = 1 << 10,
    RIDE_TYPE_FLAG_NO_TEST_MODE = 1 << 11,
    RIDE_TYPE_FLAG_TRACK_ELEMENTS_HAVE_TWO_VARIETIES = 1 << 12, // used by rides with two varieties, like the u and o shapes of the dinghy slide and the dry and submerged track of the water coaster
    RIDE_TYPE_FLAG_NO_VEHICLES = 1 << 13,                       // used only by maze, spiral slide and shops
    RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS = 1 << 14,
    RIDE_TYPE_FLAG_HAS_NO_TRACK = 1 << 15,
    RIDE_TYPE_FLAG_16 = 1 << 16,                                // something to do with vehicle colour scheme
    RIDE_TYPE_FLAG_IS_SHOP = 1 << 17,
    RIDE_TYPE_FLAG_TRACK_NO_WALLS = 1 << 18,                    // if set, wall scenery can not share a tile with the ride's track
    RIDE_TYPE_FLAG_FLAT_RIDE = 1 << 19,
    RIDE_TYPE_FLAG_PEEP_WILL_RIDE_AGAIN = 1 << 20,              // whether or not guests will go on the ride again if they liked it (this is usually applied to everything apart from transport rides)
    RIDE_TYPE_FLAG_PEEP_SHOULD_GO_INSIDE_FACILITY = 1 << 21,    // used by toilets and first aid to mark that peep should go inside the building (rather than 'buying' at the counter)
    RIDE_TYPE_FLAG_IN_RIDE = 1 << 22,                           // peeps are "IN" (ride) rather than "ON" (ride)
    RIDE_TYPE_FLAG_SELLS_FOOD = 1 << 23,
    RIDE_TYPE_FLAG_SELLS_DRINKS = 1 << 24,
    RIDE_TYPE_FLAG_IS_BATHROOM = 1 << 25,
    RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS = 1 << 26,               // whether or not vehicle colours can be set
    RIDE_TYPE_FLAG_CHECK_FOR_STALLING = 1 << 27,
    RIDE_TYPE_FLAG_HAS_TRACK = 1 << 28,
    RIDE_TYPE_FLAG_29 = 1 << 29,                                // used only by lift
    RIDE_TYPE_FLAG_HAS_LARGE_CURVES = 1 << 30,                  // whether the ride supports large (45 degree turn) curves
    RIDE_TYPE_FLAG_SUPPORTS_MULTIPLE_TRACK_COLOUR = 1u << 31,
};
// Hack for MSVC which thinks RIDE_TYPE_FLAG_SUPPORTS_MULTIPLE_TRACK_COLOUR = 1u << 31 is signed and generates narrowing conversion warnings
constexpr inline uint32 operator | (ride_type_flags a, ride_type_flags b) { return static_cast<ride_type_flags>(static_cast<uint32>(a) | static_cast<uint32>(b)); }

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
    RIDE_SETTING_RIDE_TYPE,
};

enum {
    MAZE_WALL_TYPE_BRICK,
    MAZE_WALL_TYPE_HEDGE,
    MAZE_WALL_TYPE_ICE,
    MAZE_WALL_TYPE_WOOD,
};

enum {
    TRACK_SELECTION_FLAG_ARROW            = 1 << 0,
    TRACK_SELECTION_FLAG_TRACK            = 1 << 1,
    TRACK_SELECTION_FLAG_ENTRANCE_OR_EXIT = 1 << 2,
    TRACK_SELECTION_FLAG_RECHECK          = 1 << 3,
};

enum {
    RIDE_MODIFY_DEMOLISH,
    RIDE_MODIFY_RENEW
};

struct rct_ride_properties {
    uint32 flags;
    uint8 min_value;
    uint8 max_value;
    uint8 max_brakes_speed;
    uint8 powered_lift_acceleration;
    uint8 booster_acceleration;
    sint8 booster_speed_factor; // The factor to shift the raw booster speed with
};

#define RIDE_MODE_COUNT 37

#define MAX_RIDE_MEASUREMENTS 8
#define RIDE_VALUE_UNDEFINED 0xFFFF
#define RIDE_INITIAL_RELIABILITY ((100 << 8) | 0xFF) // Upper byte is percentage, lower byte is "decimal".

#define STATION_DEPART_FLAG (1 << 7)
#define STATION_DEPART_MASK (~STATION_DEPART_FLAG)

#define CURRENT_TURN_COUNT_MASK 0xF800
#define TURN_MASK_1_ELEMENT     0x001F
#define TURN_MASK_2_ELEMENTS    0x00E0
#define TURN_MASK_3_ELEMENTS    0x0700
#define TURN_MASK_4_PLUS_ELEMENTS 0xF800

#define CONSTRUCTION_LIFT_HILL_SELECTED 1

extern const rct_ride_properties RideProperties[RIDE_TYPE_COUNT];

/** Helper macros until rides are stored in this module. */
Ride * get_ride(sint32 index);
rct_ride_entry * get_ride_entry(sint32 index);
void get_ride_entry_name(char * name, sint32 index);
rct_ride_measurement * get_ride_measurement(sint32 index);

/**
 * Helper macro loop for enumerating through all the non null rides.
 */
#define FOR_ALL_RIDES(i, ride) \
    for (i = 0; i < MAX_RIDES; i++) \
        if ((ride = get_ride(i))->type != RIDE_TYPE_NULL)

extern money16 gTotalRideValueForMoney;

extern const uint8 gRideClassifications[MAX_RIDES];

extern Ride gRideList[MAX_RIDES];

extern rct_ride_measurement gRideMeasurements[MAX_RIDE_MEASUREMENTS];
extern uint16 gRideCount;

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
extern uint8 _currentTrackAlternative;
extern uint8 _selectedTrackType;

extern uint8 _previousTrackBankEnd;
extern uint8 _previousTrackSlopeEnd;

extern uint16 _previousTrackPieceX;
extern uint16 _previousTrackPieceY;
extern uint16 _previousTrackPieceZ;

extern uint8 _currentBrakeSpeed2;
extern uint8 _currentSeatRotationAngle;

extern LocationXYZ16 _unkF44188;

extern CoordsXYZD _unkF440C5;

extern uint8 gRideEntranceExitPlaceType;
extern uint8 gRideEntranceExitPlaceRideIndex;
extern uint8 gRideEntranceExitPlaceStationIndex;
extern uint8 gRideEntranceExitPlacePreviousRideConstructionState;
extern uint8 gRideEntranceExitPlaceDirection;

extern bool gGotoStartPlacementMode;
extern sint32 gRideRemoveTrackPieceCallbackX;
extern sint32 gRideRemoveTrackPieceCallbackY;
extern sint32 gRideRemoveTrackPieceCallbackZ;
extern sint32 gRideRemoveTrackPieceCallbackDirection;
extern sint32 gRideRemoveTrackPieceCallbackType;

extern uint8 gLastEntranceStyle;

sint32 ride_get_empty_slot();
sint32 ride_get_default_mode(Ride *ride);
sint32 ride_get_count();
sint32 ride_get_total_queue_length(Ride *ride);
sint32 ride_get_max_queue_time(Ride *ride);
rct_peep * ride_get_queue_head_guest(Ride * ride, sint32 stationIndex);
void ride_queue_insert_guest_at_front(Ride * ride, sint32 stationIndex, rct_peep * peep);
void ride_init_all();
void reset_all_ride_build_dates();
void ride_update_favourited_stat();
void ride_update_all();
void ride_check_all_reachable();
void ride_update_satisfaction(Ride* ride, uint8 happiness);
void ride_update_popularity(Ride* ride, uint8 pop_amount);
bool ride_try_get_origin_element(sint32 rideIndex, CoordsXYE *output);
sint32 ride_find_track_gap(CoordsXYE *input, CoordsXYE *output);
void ride_construct_new(ride_list_item listItem);
void ride_construct(sint32 rideIndex);
sint32 ride_modify(CoordsXYE *input);
void ride_remove_peeps(sint32 rideIndex);
void ride_get_status(sint32 rideIndex, rct_string_id *formatSecondary, sint32 *argument);
rct_peep *ride_get_assigned_mechanic(Ride *ride);
sint32 ride_get_total_length(Ride *ride);
sint32 ride_get_total_time(Ride *ride);
sint32 ride_can_have_multiple_circuits(Ride *ride);
track_colour ride_get_track_colour(Ride *ride, sint32 colourScheme);
vehicle_colour ride_get_vehicle_colour(Ride *ride, sint32 vehicleIndex);
sint32 ride_get_unused_preset_vehicle_colour(uint8 ride_sub_type);
void ride_set_vehicle_colours_to_random_preset(Ride *ride, uint8 preset_index);
rct_ride_entry *get_ride_entry_by_ride(Ride *ride);
uint8 *get_ride_entry_indices_for_ride_type(uint8 rideType);
void reset_type_to_ride_entry_index_map(IObjectManager& objectManager);
void ride_measurement_clear(Ride *ride);
void ride_measurements_update();
rct_ride_measurement *ride_get_measurement(sint32 rideIndex, rct_string_id *message);
void ride_breakdown_add_news_item(sint32 rideIndex);
rct_peep *ride_find_closest_mechanic(Ride *ride, sint32 forInspection);
sint32 ride_is_valid_for_open(sint32 rideIndex, sint32 goingToBeOpen, sint32 isApplying);
sint32 ride_is_valid_for_test(sint32 rideIndex, sint32 goingToBeOpen, sint32 isApplying);
sint32 ride_initialise_construction_window(sint32 rideIndex);
void ride_construction_invalidate_current_track();
sint32 sub_6C683D(sint32* x, sint32* y, sint32* z, sint32 direction, sint32 type, uint16 extra_params, rct_tile_element** output_element, uint16 flags);
void ride_set_map_tooltip(rct_tile_element *tileElement);
sint32 ride_music_params_update(sint16 x, sint16 y, sint16 z, uint8 rideIndex, uint16 sampleRate, uint32 position, uint8 *tuneId);
void ride_music_update_final();
void ride_prepare_breakdown(sint32 rideIndex, sint32 breakdownReason);
rct_tile_element *ride_get_station_start_track_element(Ride *ride, sint32 stationIndex);
rct_tile_element *ride_get_station_exit_element(sint32 x, sint32 y, sint32 z);
void ride_set_status(sint32 rideIndex, sint32 status);
void game_command_set_ride_status(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp);
void ride_set_name(sint32 rideIndex, const char *name);
void game_command_set_ride_name(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp);
void game_command_set_ride_setting(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp);
sint32 ride_get_refund_price(sint32 ride_id);
sint32 ride_get_random_colour_preset_index(uint8 ride_type);
void ride_set_colour_preset(Ride *ride, uint8 index);
money32 ride_get_common_price(Ride *forRide);
rct_ride_name get_ride_naming(const uint8 rideType, rct_ride_entry * rideEntry);
void game_command_create_ride(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp);
void game_command_callback_ride_construct_new(sint32 eax, sint32 ebx, sint32 ecx, sint32 edx, sint32 esi, sint32 edi, sint32 ebp);
void game_command_callback_ride_construct_placed_front(sint32 eax, sint32 ebx, sint32 ecx, sint32 edx, sint32 esi, sint32 edi, sint32 ebp);
void game_command_callback_ride_construct_placed_back(sint32 eax, sint32 ebx, sint32 ecx, sint32 edx, sint32 esi, sint32 edi, sint32 ebp);
void game_command_callback_ride_remove_track_piece(sint32 eax, sint32 ebx, sint32 ecx, sint32 edx, sint32 esi, sint32 edi, sint32 ebp);
void game_command_demolish_ride(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp);
void game_command_set_ride_appearance(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp);
void game_command_set_ride_price(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp);
money32 ride_create_command(sint32 type, sint32 subType, sint32 flags, uint8 *outRideIndex, uint8 *outRideColour);
void ride_set_name_to_default(Ride * ride, rct_ride_entry * rideEntry);

void ride_set_name_to_track_default(Ride *ride, rct_ride_entry * rideEntry);
void ride_clear_for_construction(sint32 rideIndex);
void ride_entrance_exit_place_provisional_ghost();
void ride_entrance_exit_remove_ghost();
void ride_restore_provisional_track_piece();
void ride_remove_provisional_track_piece();
void set_vehicle_type_image_max_sizes(rct_ride_entry_vehicle* vehicle_type, sint32 num_images);
void invalidate_test_results(sint32 rideIndex);

void ride_select_next_section();
void ride_select_previous_section();

void increment_turn_count_1_element(Ride* ride, uint8 type);
void increment_turn_count_2_elements(Ride* ride, uint8 type);
void increment_turn_count_3_elements(Ride* ride, uint8 type);
void increment_turn_count_4_plus_elements(Ride* ride, uint8 type);
sint32 get_turn_count_1_element(Ride* ride, uint8 type);
sint32 get_turn_count_2_elements(Ride* ride, uint8 type);
sint32 get_turn_count_3_elements(Ride* ride, uint8 type);
sint32 get_turn_count_4_plus_elements(Ride* ride, uint8 type);

uint8 ride_get_helix_sections(Ride *ride);
bool ride_has_spinning_tunnel(Ride *ride);
bool ride_has_water_splash(Ride *ride);
bool ride_has_rapids(Ride *ride);
bool ride_has_log_reverser(Ride *ride);
bool ride_has_waterfall(Ride *ride);
bool ride_has_whirlpool(Ride *ride);

bool ride_type_has_flag(sint32 rideType, sint32 flag);
bool ride_is_powered_launched(Ride *ride);
bool ride_is_block_sectioned(Ride *ride);
bool ride_has_any_track_elements(sint32 rideIndex);
void ride_all_has_any_track_elements(bool *rideIndexArray);

void ride_construction_set_default_next_piece();

bool track_block_get_next(CoordsXYE *input, CoordsXYE *output, sint32 *z, sint32 *direction);
bool track_block_get_next_from_zero(sint16 x, sint16 y, sint16 z_start, uint8 rideIndex, uint8 direction_start, CoordsXYE *output, sint32 *z, sint32 *direction, bool isGhost);

bool track_block_get_previous(sint32 x, sint32 y, rct_tile_element *tileElement, track_begin_end *outTrackBeginEnd);
bool track_block_get_previous_from_zero(sint16 x, sint16 y, sint16 z, uint8 rideIndex, uint8 direction, track_begin_end *outTrackBeginEnd);

void ride_get_start_of_track(CoordsXYE * output);

void window_ride_construction_update_active_elements();
void ride_construction_remove_ghosts();
money32 ride_entrance_exit_place_ghost(sint32 rideIndex, sint32 x, sint32 y, sint32 direction, sint32 placeType, sint32 stationNum);
void ride_get_entrance_or_exit_position_from_screen_position(sint32 x, sint32 y, sint32 *outX, sint32 *outY, sint32 *outDirection);

bool ride_select_backwards_from_front();
bool ride_select_forwards_from_back();

money32 ride_remove_track_piece(sint32 x, sint32 y, sint32 z, sint32 direction, sint32 type, uint8 flags);

bool ride_are_all_possible_entrances_and_exits_built(Ride *ride);
void ride_fix_breakdown(sint32 rideIndex, sint32 reliabilityIncreaseFactor);

void ride_entry_get_train_layout(sint32 rideEntryIndex, sint32 numCarsPerTrain, uint8 *trainLayout);
uint8 ride_entry_get_vehicle_at_position(sint32 rideEntryIndex, sint32 numCarsPerTrain, sint32 position);
void ride_update_max_vehicles(sint32 rideIndex);
uint64 ride_entry_get_supported_track_pieces(const rct_ride_entry * rideEntry);

void ride_set_ride_entry(sint32 rideIndex, sint32 rideEntry);
void ride_set_num_vehicles(sint32 rideIndex, sint32 numVehicles);
void ride_set_num_cars_per_vehicle(sint32 rideIndex, sint32 numCarsPerVehicle);
void game_command_set_ride_vehicles(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp);

void game_command_place_ride_entrance_or_exit(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp);
void game_command_remove_ride_entrance_or_exit(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp);

void ride_set_to_default_inspection_interval(sint32 rideIndex);

void sub_6CB945(sint32 rideIndex);
void ride_crash(uint8 rideIndex, uint8 vehicleIndex);

void sub_6C94D8();

void ride_reset_all_names();
const uint8* ride_seek_available_modes(Ride *ride);

void window_ride_construction_mouseup_demolish_next_piece(sint32 x, sint32 y, sint32 z, sint32 direction, sint32 type);

uint32 ride_customers_per_hour(const Ride *ride);
uint32 ride_customers_in_last_5_minutes(const Ride *ride);

rct_vehicle * ride_get_broken_vehicle(Ride *ride);

void window_ride_construction_do_station_check();
void window_ride_construction_do_entrance_exit_check();
void game_command_callback_place_ride_entrance_or_exit(sint32 eax, sint32 ebx, sint32 ecx, sint32 edx, sint32 esi, sint32 edi, sint32 ebp);

void ride_delete(uint8 rideIndex);
void ride_renew(Ride * ride);
money16 ride_get_price(Ride * ride);

rct_tile_element *get_station_platform(sint32 x, sint32 y, sint32 z, sint32 z_tolerance);
bool ride_has_adjacent_station(Ride *ride);
bool ride_has_ratings(const Ride * ride);

const char * ride_type_get_enum_name(sint32 rideType);

uint8 ride_entry_get_first_non_null_ride_type(const rct_ride_entry * rideEntry);
bool ride_type_supports_boosters(uint8 rideType);
sint32 get_booster_speed(uint8 rideType, sint32 rawSpeed);
void fix_invalid_vehicle_sprite_sizes();
bool ride_entry_has_category(const rct_ride_entry * rideEntry, uint8 category);

sint32 ride_get_entry_index(sint32 rideType, sint32 rideSubType);

void ride_action_modify(sint32 rideIndex, sint32 modifyType, sint32 flags);
void ride_stop_peeps_queuing(sint32 rideIndex);

LocationXY16 ride_get_rotated_coords(sint16 x, sint16 y, sint16 z);

void determine_ride_entrance_and_exit_locations();

#endif
