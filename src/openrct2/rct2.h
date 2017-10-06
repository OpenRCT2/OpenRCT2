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

#ifndef _RCT2_H_
#define _RCT2_H_

#include "ride/ride_ratings.h"
#include "ride/vehicle.h"
#include "common.h"
#include "rct12.h"

#define RCT2_MAX_RIDES_IN_PARK              255
#define RCT2_MAX_STAFF                      200
#define RCT2_MAX_BANNERS_IN_PARK            250
#define RCT2_MAX_VEHICLES_PER_RIDE          32
#define RCT2_MAX_CARS_PER_TRAIN             32
#define RCT2_MAX_CATEGORIES_PER_RIDE        2
#define RCT2_MAX_RIDE_TYPES_PER_RIDE_ENTRY  3
#define RCT2_MAX_VEHICLES_PER_RIDE_ENTRY    4
#define RCT2_DOWNTIME_HISTORY_SIZE          8
#define RCT2_CUSTOMER_HISTORY_SIZE          10
#define RCT2_MAX_SPRITES                    10000
#define RCT2_MAX_MAP_ELEMENTS               0x30000
#define RCT2_MAX_ANIMATED_OBJECTS           2000


typedef struct rct2_install_info {
    uint32 installLevel;
    char title[260];
    char path[260];
    uint32 var_20C;
    uint8 pad_210[256];
    char expansionPackNames[16][128];
    uint32 activeExpansionPacks;        //0xB10
} rct2_install_info;

#pragma pack(push, 1)

/**
 * Ride structure.
 * size: 0x0260
 */
typedef struct rct2_ride {
    uint8 type;                                                     // 0x000
    // pointer to static info. for example, wild mouse type is 0x36, subtype is
    // 0x4c.
    uint8 subtype;                                                  // 0x001
    uint16 pad_002;                                                 // 0x002
    uint8 mode;                                                     // 0x004
    uint8 colour_scheme_type;                                       // 0x005
    rct_vehicle_colour vehicle_colours[RCT2_MAX_CARS_PER_TRAIN];    // 0x006
    uint8 pad_046[0x03];                                            // 0x046, Used to be track colours in RCT1 without expansions
    // 0 = closed, 1 = open, 2 = test
    uint8 status;                                                   // 0x049
    rct_string_id name;                                             // 0x04A
    union {
        uint32 name_arguments;                                      // 0x04C
        struct {
            rct_string_id name_arguments_type_name;                 // 0x04C
            uint16 name_arguments_number;                           // 0x04E
        };
    };
    rct_xy8 overall_view;                                           // 0x050
    rct_xy8 station_starts[RCT12_MAX_STATIONS_PER_RIDE];            // 0x052
    uint8 station_heights[RCT12_MAX_STATIONS_PER_RIDE];             // 0x05A
    uint8 station_length[RCT12_MAX_STATIONS_PER_RIDE];              // 0x05E
    uint8 station_depart[RCT12_MAX_STATIONS_PER_RIDE];              // 0x062
    // ride->vehicle index for current train waiting for passengers
    // at station
    uint8 train_at_station[RCT12_MAX_STATIONS_PER_RIDE];            // 0x066
    rct_xy8 entrances[RCT12_MAX_STATIONS_PER_RIDE];                 // 0x06A
    rct_xy8 exits[RCT12_MAX_STATIONS_PER_RIDE];                     // 0x072
    uint16 last_peep_in_queue[RCT12_MAX_STATIONS_PER_RIDE];         // 0x07A
    uint8 pad_082[RCT12_MAX_STATIONS_PER_RIDE];                     // 0x082, Used to be number of peeps in queue in RCT1, but this has moved.
    uint16 vehicles[RCT2_MAX_VEHICLES_PER_RIDE];                    // 0x086, Points to the first car in the train
    uint8 depart_flags;                                             // 0x0C6

    // Not sure if these should be uint or sint.
    uint8 num_stations;                                             // 0x0C7
    uint8 num_vehicles;                                             // 0x0C8
    uint8 num_cars_per_train;                                       // 0x0C9
    uint8 proposed_num_vehicles;                                    // 0x0CA
    uint8 proposed_num_cars_per_train;                              // 0x0CB
    uint8 max_trains;                                               // 0x0CC
    uint8 min_max_cars_per_train;                                   // 0x0CD
    uint8 min_waiting_time;                                         // 0x0CE
    uint8 max_waiting_time;                                         // 0x0CF
    union {
        uint8 operation_option;                                     // 0x0D0
        uint8 time_limit;                                           // 0x0D0
        uint8 num_laps;                                             // 0x0D0
        uint8 launch_speed;                                         // 0x0D0
        uint8 speed;                                                // 0x0D0
        uint8 rotations;                                            // 0x0D0
    };

    uint8 boat_hire_return_direction;                               // 0x0D1
    rct_xy8 boat_hire_return_position;                              // 0x0D2
    uint8 measurement_index;                                        // 0x0D4
    // bits 0 through 4 are the number of helix sections
    // bit 5: spinning tunnel, water splash, or rapids
    // bit 6: log reverser, waterfall
    // bit 7: whirlpool
    uint8 special_track_elements;                                   // 0x0D5
    uint8 pad_0D6[2];                                               // 0x0D6
    // Divide this value by 29127 to get the human-readable max speed
    // (in RCT2, display_speed = (max_speed * 9) >> 18)
    sint32 max_speed;                                               // 0x0D8
    sint32 average_speed;                                           // 0x0DC
    uint8 current_test_segment;                                     // 0x0E0
    uint8 average_speed_test_timeout;                               // 0x0E1
    uint8 pad_0E2[0x2];                                             // 0x0E2
    sint32 length[RCT12_MAX_STATIONS_PER_RIDE];                     // 0x0E4
    uint16 time[RCT12_MAX_STATIONS_PER_RIDE];                       // 0x0F4
    fixed16_2dp max_positive_vertical_g;                            // 0x0FC
    fixed16_2dp max_negative_vertical_g;                            // 0x0FE
    fixed16_2dp max_lateral_g;                                      // 0x100
    fixed16_2dp previous_vertical_g;                                // 0x102
    fixed16_2dp previous_lateral_g;                                 // 0x104
    uint8 pad_106[0x2];                                             // 0x106
    uint32 testing_flags;                                           // 0x108
    // x y map location of the current track piece during a test
    // this is to prevent counting special tracks multiple times
    rct_xy8 cur_test_track_location;                                // 0x10C
    // Next 3 variables are related (XXXX XYYY ZZZa aaaa)
    uint16 turn_count_default;                                      // 0x10E X = current turn count
    uint16 turn_count_banked;                                       // 0x110
    uint16 turn_count_sloped;                                       // 0x112 X = number turns > 3 elements
    union {
        uint8 inversions;                                           // 0x114 (???X XXXX)
        uint8 holes;                                                // 0x114 (???X XXXX)
        // This is a very rough approximation of how much of the ride is undercover.
        // It reaches the maximum value of 7 at about 50% undercover and doesn't increase beyond that.
        uint8 sheltered_eighths;                                    // 0x114 (XXX?-????)
    };
    // Y is number of powered lifts, X is drops
    uint8 drops;                                                    // 0x115 (YYXX XXXX)
    uint8 start_drop_height;                                        // 0x116
    uint8 highest_drop_height;                                      // 0x117
    sint32 sheltered_length;                                        // 0x118
    // Unused always 0? Should affect nausea
    uint16 var_11C;                                                 // 0x11C
    uint8 num_sheltered_sections;                                   // 0x11E (?abY YYYY)
    // see cur_test_track_location
    uint8 cur_test_track_z;                                         // 0x11F
    // Customer counter in the current 960 game tick (about 30 seconds) interval
    uint16 cur_num_customers;                                       // 0x120
    // Counts ticks to update customer intervals, resets each 960 game ticks.
    uint16 num_customers_timeout;                                   // 0x122
    // Customer count in the last 10 * 960 game ticks (sliding window)
    uint16 num_customers[RCT2_CUSTOMER_HISTORY_SIZE];               // 0x124
    money16 price;                                                  // 0x138
    rct_xy8 chairlift_bullwheel_location[2];                        // 0x13A
    uint8 chairlift_bullwheel_z[2];                                 // 0x13E
    union {
        rating_tuple ratings;                                       // 0x140
        struct {
            ride_rating excitement;                                 // 0x140
            ride_rating intensity;                                  // 0x142
            ride_rating nausea;                                     // 0x144
        };
    };
    uint16 value;                                                   // 0x146
    uint16 chairlift_bullwheel_rotation;                            // 0x148
    uint8 satisfaction;                                             // 0x14A
    uint8 satisfaction_time_out;                                    // 0x14B
    uint8 satisfaction_next;                                        // 0x14C
    // Various flags stating whether a window needs to be refreshed
    uint8 window_invalidate_flags;                                  // 0x14D
    uint8 pad_14E[0x02];                                            // 0x14E
    uint32 total_customers;                                         // 0x150
    money32 total_profit;                                           // 0x154
    uint8 popularity;                                               // 0x158
    uint8 popularity_time_out;                                      // 0x159 Updated every purchase and ?possibly by time?
    uint8 popularity_next;                                          // 0x15A When timeout reached this will be the next popularity
    uint8 num_riders;                                               // 0x15B
    uint8 music_tune_id;                                            // 0x15C
    uint8 slide_in_use;                                             // 0x15D
    union {
        uint16 slide_peep;                                          // 0x15E
        uint16 maze_tiles;                                          // 0x15E
    };
    uint8 pad_160[0xE];                                             // 0x160
    uint8 slide_peep_t_shirt_colour;                                // 0x16E
    uint8 pad_16F[0x7];                                             // 0x16F
    uint8 spiral_slide_progress;                                    // 0x176
    uint8 pad_177[0x9];                                             // 0x177
    sint16 build_date;                                              // 0x180
    money16 upkeep_cost;                                            // 0x182
    uint16 race_winner;                                             // 0x184
    uint8 pad_186[0x02];                                            // 0x186
    uint32 music_position;                                          // 0x188
    uint8 breakdown_reason_pending;                                 // 0x18C
    uint8 mechanic_status;                                          // 0x18D
    uint16 mechanic;                                                // 0x18E
    uint8 inspection_station;                                       // 0x190
    uint8 broken_vehicle;                                           // 0x191
    uint8 broken_car;                                               // 0x192
    uint8 breakdown_reason;                                         // 0x193
    money16 price_secondary;                                        // 0x194
    union
    {
        struct
        {
            uint8 reliability_subvalue;                             // 0x196, 0 - 255, acts like the decimals for reliability_percentage
            uint8 reliability_percentage;                           // 0x197, Starts at 100 and decreases from there.
        };
        uint16 reliability;                                         // 0x196
    };
    // Small constant used to increase the unreliability as the game continues,
    // making breakdowns more and more likely.
    uint8 unreliability_factor;                                     // 0x198
    // Range from [0, 100]
    uint8 downtime;                                                 // 0x199
    uint8 inspection_interval;                                      // 0x19A
    uint8 last_inspection;                                          // 0x19B
    uint8 downtime_history[RCT2_DOWNTIME_HISTORY_SIZE];             // 0x19C
    uint32 no_primary_items_sold;                                   // 0x1A4
    uint32 no_secondary_items_sold;                                 // 0x1A8
    uint8 breakdown_sound_modifier;                                 // 0x1AC
    // Used to oscillate the sound when ride breaks down.
    // 0 = no change, 255 = max change
    uint8 not_fixed_timeout;                                        // 0x1AD
    uint8 last_crash_type;                                          // 0x1AE
    uint8 connected_message_throttle;                               // 0x1AF
    money32 income_per_hour;                                        // 0x1B0
    money32 profit;                                                 // 0x1B4
    uint8 queue_time[RCT12_MAX_STATIONS_PER_RIDE];                  // 0x1B8
    uint8 track_colour_main[RCT12_NUM_COLOUR_SCHEMES];              // 0x1BC
    uint8 track_colour_additional[RCT12_NUM_COLOUR_SCHEMES];        // 0x1C0
    uint8 track_colour_supports[RCT12_NUM_COLOUR_SCHEMES];          // 0x1C4
    uint8 music;                                                    // 0x1C8
    uint8 entrance_style;                                           // 0x1C9
    uint16 vehicle_change_timeout;                                  // 0x1CA
    uint8 num_block_brakes;                                         // 0x1CC
    uint8 lift_hill_speed;                                          // 0x1CD
    uint16 guests_favourite;                                        // 0x1CE
    uint32 lifecycle_flags;                                         // 0x1D0
    uint8 vehicle_colours_extended[RCT2_MAX_CARS_PER_TRAIN];        // 0x1D4
    uint16 total_air_time;                                          // 0x1F4
    uint8 current_test_station;                                     // 0x1F6
    uint8 num_circuits;                                             // 0x1F7
    sint16 cable_lift_x;                                            // 0x1F8
    sint16 cable_lift_y;                                            // 0x1FA
    uint8 cable_lift_z;                                             // 0x1FC
    uint8 pad_1FD;                                                  // 0x1FD
    uint16 cable_lift;                                              // 0x1FE
    uint16 queue_length[RCT12_MAX_STATIONS_PER_RIDE];               // 0x200
    uint8 pad_208[0x58];                                            // 0x208
} rct2_ride;
assert_struct_size(rct2_ride, 0x260);

#ifdef __cplusplus

/**
 * scores.dat file header.
 * size: 0x10
 */
struct rct_scores_header
{
    uint32 var_0;
    uint32 var_4;
    uint32 var_8;
    uint32 ScenarioCount;
};
assert_struct_size(rct_scores_header, 0x10);

/**
 * An entry of scores.dat
 * size: 0x02B0
 */
struct rct_scores_entry
{
    char    Path[256];
    uint8   Category;
    uint8   pad_0101[0x1F];
    sint8   ObjectiveType;
    sint8   ObjectiveArg1;
    sint32  objectiveArg2;
    sint16  objectiveArg3;
    char    Name[64];
    char    Details[256];
    sint32  Flags;
    money32 CompanyValue;
    char    CompletedBy[64];
};
assert_struct_size(rct_scores_entry, 0x02B0);

#endif // __cplusplus

#pragma pack(pop)

#endif
