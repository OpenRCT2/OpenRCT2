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

#ifndef _RCT1_H_
#define _RCT1_H_

#include "management/award.h"
#include "management/news_item.h"
#include "management/research.h"
#include "rct12.h"
#include "ride/ride.h"
#include "world/banner.h"
#include "world/map.h"
#include "world/sprite.h"

#define RCT1_MAX_MAP_ELEMENTS         0xC000
#define RCT1_MAX_SPRITES              5000
#define RCT1_MAX_TRAINS_PER_RIDE      12
#define RCT1_MAX_MAP_SIZE             128
#define RCT1_MAX_RIDES_IN_PARK        128
#define RCT1_RESEARCH_FLAGS_SEPARATOR 0xFF

typedef struct ParkLoadResult ParkLoadResult;

#pragma pack(push, 1)
typedef struct rct1_entrance {
    uint16 x;
    uint16 y;
    uint16 z;
    uint8 direction;
} rct1_entrance;
assert_struct_size(rct1_entrance, 7);

/**
 * RCT1 ride structure
 * size: 0x260
 */
typedef struct rct1_ride {
    uint8 type;                                             // 0x000
    uint8 vehicle_type;                                     // 0x001
    uint16 lifecycle_flags;                                 // 0x002
    uint8 operating_mode;                                   // 0x004
    uint8 colour_scheme;                                    // 0x005
    struct {
        colour_t body;
        colour_t trim;
    } vehicle_colours[RCT1_MAX_TRAINS_PER_RIDE];            // 0x006
    colour_t track_primary_colour;                          // 0x01E
    colour_t track_secondary_colour;                        // 0x01F
    colour_t track_support_colour;                          // 0x020
    uint8 status;                                           // 0x021
    uint16 name;                                            // 0x022
    uint16 name_argument_ride;                              // 0x024
    uint16 name_argument_number;                            // 0x026
    rct_xy8 overall_view;                                   // 0x028
    rct_xy8 station_starts[RCT12_MAX_STATIONS_PER_RIDE];    // 0x02A
    uint8 station_height[RCT12_MAX_STATIONS_PER_RIDE];      // 0x032
    uint8 station_length[RCT12_MAX_STATIONS_PER_RIDE];      // 0x036
    uint8 station_light[RCT12_MAX_STATIONS_PER_RIDE];       // 0x03A
    uint8 station_depart[RCT12_MAX_STATIONS_PER_RIDE];      // 0x03E
    rct_xy8 entrance[RCT12_MAX_STATIONS_PER_RIDE];          // 0x042
    rct_xy8 exit[RCT12_MAX_STATIONS_PER_RIDE];              // 0x04A
    uint16 last_peep_in_queue[RCT12_MAX_STATIONS_PER_RIDE]; // 0x052
    uint8 num_peeps_in_queue[RCT12_MAX_STATIONS_PER_RIDE];  // 0x05A
    uint16 vehicles[RCT1_MAX_TRAINS_PER_RIDE];              // 0x05E
    uint8 depart_flags;                                     // 0x076
    uint8 num_stations;                                     // 0x077
    uint8 num_trains;                                       // 0x078
    uint8 num_cars_per_train;                               // 0x079
    uint8 proposed_num_vehicles;                            // 0x07A
    uint8 proposed_num_cars_per_train;                      // 0x07B
    uint8 max_trains;                                       // 0x07C
    uint8 min_max_cars_per_train;                           // 0x07D
    uint8 min_waiting_time;                                 // 0x07E
    uint8 max_waiting_time;                                 // 0x07F
    uint8 operation_option;                                 // 0x080
    uint8 boat_hire_return_direction;                       // 0x081
    rct_xy8 boat_hire_return_position;                      // 0x082
    uint8 data_logging_index;                               // 0x084
    uint8 special_track_elements;                           // 0x085
    uint16 unk_86;                                          // 0x086
    sint32 max_speed;                                       // 0x088
    sint32 average_speed;                                   // 0x08C
    uint8 current_test_segment;                             // 0x090
    uint8 average_speed_test_timeout;                       // 0x091
    uint8 pad_0E2[0x2];                                     // 0x092
    sint32 length[RCT12_MAX_STATIONS_PER_RIDE];             // 0x094
    uint16 time[RCT12_MAX_STATIONS_PER_RIDE];               // 0x0A4
    fixed16_2dp max_positive_vertical_g;                    // 0x0AC
    fixed16_2dp max_negative_vertical_g;                    // 0x0AE
    fixed16_2dp max_lateral_g;                              // 0x0B0
    fixed16_2dp previous_vertical_g;                        // 0x0B2
    fixed16_2dp previous_lateral_g;                         // 0x0B4
    uint8 pad_B6[0x2];                                      // 0x0B6
    uint32 testing_flags;                                   // 0x0B8
    // x y map location of the current track piece during a test
    // this is to prevent counting special tracks multiple times
    rct_xy8 cur_test_track_location;                        // 0x0BC
    // Next 3 variables are related (XXXX XYYY ZZZa aaaa)
    uint16 turn_count_default;                              // 0x0BE X = current turn count
    uint16 turn_count_banked;                               // 0x0C0
    uint16 turn_count_sloped;                               // 0x0C2 X = number turns > 3 elements
    union {
        uint8 num_inversions;                               // 0x0C4
        uint8 num_holes;
    };
    uint8 num_drops;                                        // 0x0C5
    uint8 start_drop_height;                                // 0x0C6
    uint8 highest_drop_height;                              // 0x0C7
    sint32 sheltered_length;                                // 0x0C8
    uint8 unk_CC[2];                                        // 0x0CC
    uint8 num_sheltered_sections;                           // 0x0CE
    // see cur_test_track_location
    uint8 cur_test_track_z;                                 // 0x0CF
    sint16 unk_D0;                                          // 0x0D0
    sint16 unk_D2;                                          // 0x0D2
    sint16 customers_per_hour;                              // 0x0D4
    sint16 unk_D6;                                          // 0x0D6
    sint16 unk_D8;                                          // 0x0D8
    sint16 unk_DA;                                          // 0x0DA
    sint16 unk_DC;                                          // 0x0DC
    sint16 unk_DE;                                          // 0x0DE
    uint16 age;                                             // 0x0E0
    sint16 running_cost;                                    // 0x0E2
    sint16 unk_E4;                                          // 0x0E4
    sint16 unk_E6;                                          // 0x0E6
    money16 price;                                          // 0x0E8
    rct_xy8 chairlift_bullwheel_location[2];                // 0x0EA
    uint8 chairlift_bullwheel_z[2];                         // 0x0EE
    union {
        rating_tuple ratings;
        struct {
            ride_rating excitement;                         // 0x0F0
            ride_rating intensity;                          // 0x0F2
            ride_rating nausea;                             // 0x0F4
        };
    };
    uint16 value;                                           // 0x0F6
    uint16 chairlift_bullwheel_rotation;                    // 0x0F8
    uint8 satisfaction;                                     // 0x0FA
    uint8 satisfaction_time_out;                            // 0x0FB
    uint8 satisfaction_next;                                // 0x0FC
    uint8 window_invalidate_flags;                          // 0x0FD
    uint8 unk_FE[2];                                        // 0x0FE
    uint32 total_customers;                                 // 0x100
    money32 total_profit;                                   // 0x104
    uint8 popularity;                                       // 0x108
    uint8 popularity_time_out;                              // 0x109
    uint8 popularity_next;                                  // 0x10A
    uint8 num_riders;                                       // 0x10B
    uint8 music_tune_id;                                    // 0x10C
    uint8 slide_in_use;                                     // 0x10D
    union {
        uint16 slide_peep;                                  // 0x10E
        uint16 maze_tiles;                                  // 0x10E
    };
    uint8 pad_110[0xE];                                     // 0x110
    uint8 slide_peep_t_shirt_colour;                        // 0x11E
    uint8 pad_11F[0x7];                                     // 0x11F
    uint8 spiral_slide_progress;                            // 0x126
    uint8 pad_127[0x9];                                     // 0x127
    sint16 build_date;                                      // 0x130
    money16 upkeep_cost;                                    // 0x131
    uint16 race_winner;                                     // 0x132
    uint8 unk_134[2];                                       // 0x134
    uint32 music_position;                                  // 0x138
    uint8 breakdown_reason_pending;                         // 0x13C
    uint8 mechanic_status;                                  // 0x13D
    uint16 mechanic;                                        // 0x13E
    uint8 inspection_station;                               // 0x140
    uint8 broken_vehicle;                                   // 0x141
    uint8 broken_car;                                       // 0x142
    uint8 breakdown_reason;                                 // 0x143
    uint8 unk_144[2];                                       // 0x144
    union
    {
        struct
        {
            uint8 reliability_subvalue;                     // 0x146, 0 - 255, acts like the decimals for reliability_percentage
            uint8 reliability_percentage;                   // 0x147, Starts at 100 and decreases from there.
        };
        uint16 reliability;                                 // 0x146
    };
    uint8 unreliability_factor;                             // 0x148
    uint8 downtime;                                         // 0x149
    uint8 inspection_interval;                              // 0x14A
    uint8 last_inspection;                                  // 0x14B
    uint8 unk_14C[20];                                      // 0x14C
    money32 income_per_hour;                                // 0x160
    money32 profit;                                         // 0x164
    uint8 queue_time[RCT12_MAX_STATIONS_PER_RIDE];          // 0x168
    colour_t track_colour_main[4];                          // 0x16C
    colour_t track_colour_additional[4];                    // 0x170
    colour_t track_colour_supports[4];                      // 0x174
    uint8 music;                                            // 0x178
    uint8 entrance_style;                                   // 0x179
    uint8 unk_17A[230];                                     // 0x17A
} rct1_ride;
assert_struct_size(rct1_ride, 0x260);

typedef struct rct1_unk_sprite {
    uint8 sprite_identifier;        // 0x00
    uint8 misc_identifier;          // 0x01
    uint16 next_in_quadrant;        // 0x02
    uint16 next;                    // 0x04
    uint16 previous;                // 0x06
    uint8 linked_list_type_offset;  // 0x08 Valid values are SPRITE_LINKEDLIST_OFFSET_...
    // Height from centre of sprite to bottom
    uint8 sprite_height_negative;   // 0x09
    uint16 sprite_index;            // 0x0A
    uint16 flags;           // 0x0C
    sint16 x;                       // 0x0E
    sint16 y;                       // 0x10
    sint16 z;                       // 0x12
    // Width from centre of sprite to edge
    uint8 sprite_width;             // 0x14
    // Height from centre of sprite to top
    uint8 sprite_height_positive;   // 0x15
    sint16 sprite_left;             // 0x16
    sint16 sprite_top;              // 0x18
    sint16 sprite_right;            // 0x1A
    sint16 sprite_bottom;           // 0x1C
    uint8  sprite_direction; //direction of sprite? 0x1e
    uint8 pad_1F[3]; // 0x1f
    rct_string_id name_string_idx;  // 0x22
    uint16 var_24;
    uint16 frame;                   // 0x26
    uint8 var_28[3];
    uint8 var_2B;
    uint8 pad_2C[0x45];
    uint8 var_71;
} rct1_unk_sprite;

typedef struct rct1_vehicle {
    uint8 sprite_identifier;        // 0x00
    uint8 is_child;                 // 0x01
    uint16 next_in_quadrant;        // 0x02
    uint16 next;                    // 0x04
    uint16 previous;                // 0x06
    uint8 linked_list_type_offset;  // 0x08 Valid values are SPRITE_LINKEDLIST_OFFSET_...
    // Height from centre of sprite to bottom
    uint8 sprite_height_negative;   // 0x09
    uint16 sprite_index;            // 0x0A
    uint16 flags;           // 0x0C
    sint16 x;                       // 0x0E
    sint16 y;                       // 0x10
    sint16 z;                       // 0x12
    // Width from centre of sprite to edge
    uint8 sprite_width;             // 0x14
    // Height from centre of sprite to top
    uint8 sprite_height_positive;   // 0x15
    sint16 sprite_left;             // 0x16
    sint16 sprite_top;              // 0x18
    sint16 sprite_right;            // 0x1A
    sint16 sprite_bottom;           // 0x1C
    uint8 sprite_direction;         // 0x1E
    uint8 vehicle_sprite_type;      // 0x1F
    uint8 bank_rotation;            // 0x20
    uint8 pad_21[3];
    sint32 remaining_distance;      // 0x24
    sint32 velocity;                // 0x28
    sint32 acceleration;            // 0x2C
    uint8 ride;                     // 0x30
    uint8 vehicle_type;             // 0x31
    rct_vehicle_colour colours;     // 0x32
    union {
        uint16 track_progress;          // 0x34
        struct {
            sint8 var_34;
            uint8 var_35;
        };
    };
    union {
        sint16 track_direction;     // 0x36 (0000 0000 0000 0011)
        sint16 track_type;          // 0x36 (0000 0011 1111 1100)
        rct_xy8 boat_location;      // 0x36
    };
    uint16 track_x;                 // 0x38
    uint16 track_y;                 // 0x3A
    uint16 track_z;                 // 0x3C
    uint16 next_vehicle_on_train;   // 0x3E

    // The previous vehicle on the same train or the last vehicle on the previous or only train.
    uint16 prev_vehicle_on_ride;    // 0x40

    // The next vehicle on the same train or the first vehicle on the next or only train
    uint16 next_vehicle_on_ride;    // 0x42

    uint16 var_44;
    uint16 friction;                // 0x46
    uint16 update_flags;            // 0x48
    uint8 var_4A;
    uint8 current_station;          // 0x4B
    union {
        sint16 swinging_car_var_0;  // 0x4C
        sint16 current_time;        // 0x4C
        struct {
            sint8 ferris_wheel_var_0;   // 0x4C
            sint8 ferris_wheel_var_1;   // 0x4D
        };
    };
    sint16 var_4E;
    uint8 status;                   // 0x50
    uint8 sub_state;                // 0x51
    uint16 peep[32];                // 0x52
    uint8 peep_tshirt_colours[32];  // 0x92
    uint8 num_seats;                // 0xB2
    uint8 num_peeps;                // 0xB3
    uint8 next_free_seat;           // 0xB4
    uint8 restraints_position;      // 0xB5 0 == Close, 255 == Open
    sint16 var_B6;
    uint16 var_B8;
    uint8 var_BA;
    uint8 sound1_id;                // 0xBB
    uint8 sound1_volume;            // 0xBC
    uint8 sound2_id;                // 0xBD
    uint8 sound2_volume;            // 0xBE
    sint8 var_BF;
    union {
        uint16 var_C0;
        uint16 time_waiting;        // 0xC0
        uint16 cable_lift_target;   // 0xC0
    };
    uint8 speed;                    // 0xC2
    uint8 powered_acceleration;     // 0xC3
    uint8 var_C4;
    uint8 var_C5;
    uint8 pad_C6[0x2];
    uint16 var_C8;
    uint16 var_CA;
    uint8 scream_sound_id;          // 0xCC
    uint8 var_CD;
    union {
        uint8 var_CE;
        uint8 num_laps;             // 0xCE
    };
    union {
        uint8 var_CF;
        uint8 brake_speed;          // 0xCF
    };
    uint16 lost_time_out;           // 0xD0
    sint8 vertical_drop_countdown;  // 0xD1
    uint8 var_D3;
    uint8 mini_golf_current_animation;
    uint8 mini_golf_flags;          // 0xD5
    uint8 ride_subtype;             // 0xD6
    uint8 colours_extended;         // 0xD7
} rct1_vehicle;

typedef struct rct1_peep {
    uint8 sprite_identifier;        // 0x00
    uint8 misc_identifier;          // 0x01
    uint16 next_in_quadrant;        // 0x02
    uint16 next;                    // 0x04
    uint16 previous;                // 0x06
    uint8 linked_list_type_offset;  // 0x08 Valid values are SPRITE_LINKEDLIST_OFFSET_...
    // Height from centre of sprite to bottom
    uint8 sprite_height_negative;   // 0x09
    uint16 sprite_index;            // 0x0A
    uint16 flags;           // 0x0C
    sint16 x;                       // 0x0E
    sint16 y;                       // 0x10
    sint16 z;                       // 0x12
    // Width from centre of sprite to edge
    uint8 sprite_width;             // 0x14
    // Height from centre of sprite to top
    uint8 sprite_height_positive;   // 0x15
    sint16 sprite_left;             // 0x16
    sint16 sprite_top;              // 0x18
    sint16 sprite_right;            // 0x1A
    sint16 sprite_bottom;           // 0x1C
    uint8 sprite_direction;         // 0x1E
    uint8 pad_1F[3];
    rct_string_id name_string_idx;  // 0x22
    uint16 next_x;                  // 0x24
    uint16 next_y;                  // 0x26
    uint8 next_z;                   // 0x28
    uint8 next_var_29;              // 0x29
    uint8 outside_of_park;          // 0x2A
    uint8 state;                    // 0x2B
    uint8 sub_state;                // 0x2C
    uint8 sprite_type;              // 0x2D
    uint8 type;                     // 0x2E
    union{
        uint8 staff_type;           // 0x2F
        uint8 no_of_rides;          // 0x2F
    };
    uint8 tshirt_colour;            // 0x30
    uint8 trousers_colour;          // 0x31
    uint16 destination_x;           // 0x32 Location that the peep is trying to get to
    uint16 destination_y;           // 0x34
    uint8 destination_tolerence;    // 0x36 How close to destination before next action/state 0 = exact
    uint8 var_37;
    uint8 energy;                   // 0x38
    uint8 energy_target;            // 0x39
    uint8 happiness;                // 0x3A
    uint8 happiness_target;         // 0x3B
    uint8 nausea;                   // 0x3C
    uint8 nausea_target;            // 0x3D
    uint8 hunger;                   // 0x3E
    uint8 thirst;                   // 0x3F
    uint8 bathroom;                 // 0x40
    uint8 var_41;
    uint8 var_42;
    uint8 intensity;                // 0x43 The max intensity is stored in the first 4 bits, and the min intensity in the second 4 bits
    uint8 nausea_tolerance;         // 0x44
    uint8 window_invalidate_flags;  // 0x45
    money16 paid_on_drink;          // 0x46
    uint8 ride_types_been_on[16];   // 0x48
    uint32 item_extra_flags;        // 0x58
    uint8 photo2_ride_ref;          // 0x5C
    uint8 photo3_ride_ref;          // 0x5D
    uint8 photo4_ride_ref;          // 0x5E
    uint8 pad_5F[0x09];             // 0x5F
    uint8 current_ride;             // 0x68
    uint8 current_ride_station;     // 0x69
    uint8 current_train;            // 0x6A
    union{
        struct{
            uint8 current_car;      // 0x6B
            uint8 current_seat;     // 0x6C
        };
        uint16 time_to_sitdown;     //0x6B
        struct{
            uint8 time_to_stand;    //0x6B
            uint8 standing_flags;   //0x6C
        };
    };
    // Normally 0, 1 for carrying sliding board on spiral slide ride, 2 for carrying lawn mower
    uint8 special_sprite;           // 0x6D
    uint8 action_sprite_type;       // 0x6E
    // Seems to be used like a local variable, as it's always set before calling peep_switch_to_next_action_sprite_type, which reads this again
    uint8 next_action_sprite_type;  // 0x6F
    uint8 action_sprite_image_offset; // 0x70
    uint8 action;                   // 0x71
    uint8 action_frame;             // 0x72
    uint8 var_73;
    union {
        uint16 var_74; // time getting to ride to fix
        uint16 next_in_queue;       // 0x74
    };
    uint8 var_76;
    uint8 pad_77;
    union{
        uint8 maze_last_edge;       // 0x78
        uint8 direction;    //Direction ?
    };
    uint8 interaction_ride_index;
    uint16 time_in_queue;           // 0x7A
    uint8 rides_been_on[32];        // 0x7C
    // 255 bit bitmap of every ride the peep has been on see
    // window_peep_rides_update for how to use.
    uint32 id;                      // 0x9C
    money32 cash_in_pocket;         // 0xA0
    money32 cash_spent;             // 0xA4
    sint32 time_in_park;            // 0xA8
    sint8 var_AC;                   // 0xAC
    uint8 previous_ride;            // 0xAD
    uint16 previous_ride_time_out;  // 0xAE
    rct_peep_thought thoughts[PEEP_MAX_THOUGHTS];   // 0xB0
    uint8 pad_C4;
    union {
        uint8 staff_id;                     // 0xC5
        uint8 guest_heading_to_ride_id;     // 0xC5
    };
    union {
        uint8 staff_orders;             // 0xC6
        uint8 peep_is_lost_countdown;   // 0xC6
    };
    uint8 photo1_ride_ref;          // 0xC7
    uint32 peep_flags;              // 0xC8
    rct_xyzd8 pathfind_goal;        // 0xCC
    rct_xyzd8 pathfind_history[4];  // 0xD0
    uint8 no_action_frame_no;       // 0xE0
    // 0x3F Litter Count split into lots of 3 with time, 0xC0 Time since last recalc
    uint8 litter_count;             // 0xE1
    union{
        uint8 time_on_ride;         // 0xE2
        uint8 var_E2;               // 0xE2
    };
    // 0x3F Sick Count split into lots of 3 with time, 0xC0 Time since last recalc
    uint8 disgusting_count;         // 0xE3
    money16 paid_to_enter;          // 0xE4
    money16 paid_on_rides;          // 0xE6
    money16 paid_on_food;           // 0xE8
    money16 paid_on_souvenirs;      // 0xEA
    uint8 no_of_food;               // 0xEC
    uint8 no_of_drinks;             // 0xED
    uint8 no_of_souvenirs;          // 0xEE
    uint8 var_EF;
    uint8 voucher_type;             // 0xF0
    uint8 voucher_arguments;        // 0xF1 ride_id or string_offset_id
    uint8 var_F2;
    uint8 var_F3;
    uint8 var_F4;
    uint8 days_in_queue;            // 0xF5
    uint8 balloon_colour;           // 0xF6
    uint8 umbrella_colour;          // 0xF7
    uint8 hat_colour;               // 0xF8
    uint8 favourite_ride;           // 0xF9
    uint8 favourite_ride_rating;    // 0xFA
    uint8 pad_FB;
    uint32 item_standard_flags;     // 0xFC
} rct1_peep;
assert_struct_size(rct1_peep, 0x100);


enum RCT1_PEEP_SPRITE_TYPE {
    RCT1_PEEP_SPRITE_TYPE_NORMAL = 0,
    RCT1_PEEP_SPRITE_TYPE_HANDYMAN = 1,
    RCT1_PEEP_SPRITE_TYPE_MECHANIC = 2,
    RCT1_PEEP_SPRITE_TYPE_SECURITY = 3,
    RCT1_PEEP_SPRITE_TYPE_ENTERTAINER_PANDA = 4,
    RCT1_PEEP_SPRITE_TYPE_ENTERTAINER_TIGER = 5,
    RCT1_PEEP_SPRITE_TYPE_ENTERTAINER_ELEPHANT = 6,
    RCT1_PEEP_SPRITE_TYPE_ENTERTAINER_ROMAN = 7,
    RCT1_PEEP_SPRITE_TYPE_ENTERTAINER_GORILLA = 8,
    RCT1_PEEP_SPRITE_TYPE_ENTERTAINER_SNOWMAN = 9,
    RCT1_PEEP_SPRITE_TYPE_ENTERTAINER_KNIGHT = 10,
    RCT1_PEEP_SPRITE_TYPE_ENTERTAINER_ASTRONAUT = 11,

    RCT1_PEEP_SPRITE_TYPE_BALLOON = 16,
    RCT1_PEEP_SPRITE_TYPE_CANDYFLOSS = 17,
    RCT1_PEEP_SPRITE_TYPE_UMBRELLA = 18,
    RCT1_PEEP_SPRITE_TYPE_PIZZA = 19, // Unsure
    RCT1_PEEP_SPRITE_TYPE_SECURITY_ALT = 20, // Unknown
    RCT1_PEEP_SPRITE_TYPE_POPCORN = 21,
    RCT1_PEEP_SPRITE_TYPE_ARMS_CROSSED = 22,
    RCT1_PEEP_SPRITE_TYPE_HEAD_DOWN = 23,
    RCT1_PEEP_SPRITE_TYPE_NAUSEOUS = 24,
    RCT1_PEEP_SPRITE_TYPE_VERY_NAUSEOUS = 25,
    RCT1_PEEP_SPRITE_TYPE_REQUIRE_BATHROOM = 26,
    RCT1_PEEP_SPRITE_TYPE_HAT = 27,
    RCT1_PEEP_SPRITE_TYPE_BURGER = 28,
    RCT1_PEEP_SPRITE_TYPE_TENTACLE = 29,
    RCT1_PEEP_SPRITE_TYPE_TOFFEE_APPLE = 30
};

typedef union rct1_sprite {
    uint8 pad_00[0x100];
    rct1_unk_sprite unknown;
    rct1_vehicle vehicle;
    rct1_peep peep;
    rct_litter litter;
    rct_balloon balloon;
    rct_sprite duck;
    rct_jumping_fountain jumping_fountain;
    rct_money_effect money_effect;
    rct_crashed_vehicle_particle crashed_vehicle_particle;
    rct_crash_splash crash_splash;
    rct_steam_particle steam_particle;
} rct1_sprite;
assert_struct_size(rct1_sprite, 0x100);

typedef struct rct1_research_item {
    uint8 item;
    uint8 related_ride;
    uint8 category;
    uint8 flags;
    uint8 expenditure_area;
} rct1_research_item;
assert_struct_size(rct1_research_item, 5);

/**
 * RCT1,AA,LL scenario / saved game structure.
 * size: 0x1F850C
 */
typedef struct rct1_s4 {
    uint16 month;
    uint16 day;
    uint32 ticks;
    uint32 random_a;
    uint32 random_b;
    rct_map_element map_elements[RCT1_MAX_MAP_ELEMENTS];
    uint32 unk_counter;
    rct1_sprite sprites[RCT1_MAX_SPRITES];
    uint16 next_sprite_index;
    uint16 first_vehicle_sprite_index;
    uint16 first_peep_sprite_index;
    uint16 first_duck_sprite_index;
    uint16 first_litter_sprite_index;
    uint16 first_oversized_ride_car_sprite_index;
    uint16 sprites_available;
    uint16 num_vehicle_sprites;
    uint16 num_peep_sprites;
    uint16 num_duck_sprites;
    uint16 num_litter_sprites;
    uint16 num_oversized_ride_car_sprites;
    uint32 park_name_string_index;
    uint32 unk_198830;
    money32 cash;
    money32 loan;
    uint32 park_flags;
    money16 park_entrance_fee;
    rct1_entrance park_entrance;
    uint8 unk_198849;
    rct2_peep_spawn peep_spawn[RCT12_MAX_PEEP_SPAWNS];
    uint8 unk_198856;
    uint8 research_level;
    uint32 unk_198858;
    uint8 available_rides[32];
    uint8 available_vehicles[32];
    uint32 ride_feature_1[128];
    uint32 ride_feature_2[128];
    uint16 guests_in_park;
    uint16 unk_198C9E;
    money32 expenditure[14 * 16];
    uint32 guests_in_park_2;
    uint8 unk_199024;
    colour_t handman_colour;
    colour_t mechanic_colour;
    colour_t security_guard_colour;
    uint8 available_scenery[128];
    uint16 available_banners;
    uint8 unk_1990AA[94];
    uint16 park_rating;
    uint8 park_rating_history[32];
    uint8 guests_in_park_history[32];
    uint8 research_priority;
    uint8 research_progress;
    uint8 last_research_item;
    uint8 last_research_ride;
    uint8 last_research_category;
    uint8 last_research_flag;
    rct1_research_item research_items[200];
    uint8 next_research_item;
    uint8 next_research_ride;
    uint8 next_research_category;
    uint8 next_research_flag;
    uint16 unk_19953C;
    uint32 unk_19953E;
    uint16 cheat_detection_land_owned;
    uint16 unk_199544;
    money16 median_recommended_park_entry_fee;
    money32 max_loan;
    money16 guest_initial_cash;
    uint8 guest_initial_hunger;
    uint8 guest_initial_thirst;
    uint8 scenario_objective_type;
    uint8 scenario_objective_years;
    uint16 unk_199552;
    money32 scenario_objective_currency;
    uint16 scenario_objective_num_guests;
    uint8 marketing_status[20];
    uint8 marketing_assoc[20];
    uint8 unk_199582[2];
    money32 cash_history[128];
    money32 total_expenditure;
    money32 profit;
    uint8 unk_199788[8];
    money32 weekly_profit_history[128];
    money32 park_value;
    money32 park_value_history[128];
    uint32 scenario_objective_score;
    uint32 num_admissions;
    money32 admission_total_income;
    money32 company_value;
    uint8 thought_timer[16];
    rct12_award awards[RCT12_MAX_AWARDS];
    money16 land_price;
    money16 construction_rights_price;
    uint16 unk_199BCC;
    uint16 unk_199BCE;
    uint32 unk_199BD0;
    char username[64];
    uint32 game_version;
    money32 objective_completion_company_value;
    uint32 finance_checksum;
    uint16 num_rides;
    uint16 cheat_detection_neg_num_rides;
    uint16 cheat_detection_max_owned_tiles;
    uint16 cheat_detection_neg_max_owned_tiles;
    uint32 finance_checksum_3;
    uint32 scenario_slot_index_checksum;
    char scenario_winner[32];
    uint32 finance_checksum_2;
    char copyright_notice[40];
    uint16 cheat_detection_sv6_sc4[4];
    uint16 unk_199C84;
    uint16 unk_199C86;
    uint16 map_size_units;
    uint16 map_size_unk_b;
    uint16 map_size;
    uint16 map_size_max_xy;
    uint32 same_price_flags;
    uint16 unk_199C94;
    uint8 unk_199C96[3];
    uint8 water_colour;
    uint16 unk_199C9A;
    rct1_research_item research_items_LL[180];
    uint8 unk_19A020[5468];
    rct_banner banners[100];
    char string_table[1024][32];
    uint32 game_time_counter;
    rct1_ride rides[255];
    uint16 unk_game_time_counter;
    uint16 view_x;
    uint16 view_y;
    uint8 view_zoom;
    uint8 view_rotation;
    uint8 map_animations[6000];
    uint32 num_map_animations;
    uint8 unk_1CADBC[12];
    uint16 scrolling_text_step;
    uint32 unk_1CADCA;
    uint16 unk_1CADCE;
    uint8 unk_1CADD0[116];
    rct_ride_measurement ride_measurements[8];
    uint32 next_guest_index;
    uint16 game_counter_5;
    uint8 patrol_areas[0x3C00];
    uint8 unk_1F42AA[116];
    uint8 unk_1F431E[4];
    uint8 unk_1F4322[8];
    uint8 climate;
    uint8 unk_1F432B;
    uint16 climate_timer;
    uint8 weather;
    uint8 target_weather;
    uint8 temperature;
    uint8 target_temperature;
    uint8 thunder_frequency;
    uint8 target_thunder_frequency;
    uint8 weather_gloom;
    uint8 target_weather_gloom;
    uint8 rain;
    uint8 target_rain;
    rct12_news_item messages[RCT12_MAX_NEWS_ITEMS];
    char scenario_name[62];
    uint16 scenario_slot_index;
    uint32 scenario_flags;
    uint8 unk_1F8358[432];
    uint32 expansion_pack_checksum;
} rct1_s4;
assert_struct_size(rct1_s4, 0x1F850C);

/**
 * Track design structure.
 * size: 0x2006
 */
typedef struct rct_track_td4 {
    uint8 type;                                     // 0x00
    uint8 vehicle_type;
    uint32 flags;                                   // 0x02
    uint8 mode;                                     // 0x06
    uint8 version_and_colour_scheme;                // 0x07 0b0000_VVCC
    rct_vehicle_colour vehicle_colours[RCT1_MAX_TRAINS_PER_RIDE]; // 0x08
    uint8 track_spine_colour_v0;                    // 0x20
    uint8 track_rail_colour_v0;                     // 0x21
    uint8 track_support_colour_v0;                  // 0x22
    uint8 depart_flags;                             // 0x23
    uint8 number_of_trains;                         // 0x24
    uint8 number_of_cars_per_train;                 // 0x25
    uint8 min_waiting_time;                         // 0x26
    uint8 max_waiting_time;                         // 0x27
    union {
        uint8 operation_setting;
        uint8 launch_speed;
        uint8 num_laps;
        uint8 max_people;
    };
    sint8 max_speed;                                // 0x29
    sint8 average_speed;                            // 0x2A
    uint16 ride_length;                             // 0x2B
    uint8 max_positive_vertical_g;                  // 0x2D
    sint8 max_negative_vertical_g;                  // 0x2C
    uint8 max_lateral_g;                            // 0x2F
    union {
        uint8 num_inversions;                       // 0x30
        uint8 num_holes;                            // 0x30
    };
    uint8 num_drops;                                // 0x31
    uint8 highest_drop_height;                      // 0x32
    uint8 excitement;                               // 0x33
    uint8 intensity;                                // 0x34
    uint8 nausea;                                   // 0x35
    money16 upkeep_cost;                            // 0x36

    // Added Attractions / Loopy Landscapes only
    uint8 track_spine_colour[4];                    // 0x38
    uint8 track_rail_colour[4];                     // 0x3C
    uint8 track_support_colour[4];                  // 0x40
    uint8 flags2;                                   // 0x44

    uint8 var_45[0x7F];                             // 0x45

    void *elements;                                 // 0xC4 (data starts here in file, 38 for original RCT1)
    size_t elementsSize;
} rct_track_td4;
#pragma pack(pop)

enum {
    RCT1_RIDE_TYPE_NULL = 255,
    RCT1_RIDE_TYPE_WOODEN_ROLLER_COASTER = 0,
    RCT1_RIDE_TYPE_STAND_UP_STEEL_ROLLER_COASTER,
    RCT1_RIDE_TYPE_SUSPENDED_ROLLER_COASTER,
    RCT1_RIDE_TYPE_INVERTED_ROLLER_COASTER,
    RCT1_RIDE_TYPE_STEEL_MINI_ROLLER_COASTER,
    RCT1_RIDE_TYPE_MINIATURE_RAILROAD,
    RCT1_RIDE_TYPE_MONORAIL,
    RCT1_RIDE_TYPE_SUSPENDED_SINGLE_RAIL_ROLLER_COASTER,
    RCT1_RIDE_TYPE_BOAT_HIRE,
    RCT1_RIDE_TYPE_WOODEN_CRAZY_RODENT_ROLLER_COASTER,
    RCT1_RIDE_TYPE_SINGLE_RAIL_ROLLER_COASTER,
    RCT1_RIDE_TYPE_CAR_RIDE,
    RCT1_RIDE_TYPE_LAUNCHED_FREEFALL,
    RCT1_RIDE_TYPE_BOBSLED_ROLLER_COASTER,
    RCT1_RIDE_TYPE_OBSERVATION_TOWER,
    RCT1_RIDE_TYPE_STEEL_ROLLER_COASTER,
    RCT1_RIDE_TYPE_WATER_SLIDE,
    RCT1_RIDE_TYPE_MINE_TRAIN_ROLLER_COASTER,
    RCT1_RIDE_TYPE_CHAIRLIFT,
    RCT1_RIDE_TYPE_STEEL_CORKSCREW_ROLLER_COASTER,
    RCT1_RIDE_TYPE_HEDGE_MAZE,
    RCT1_RIDE_TYPE_SPIRAL_SLIDE,
    RCT1_RIDE_TYPE_GO_KARTS,
    RCT1_RIDE_TYPE_LOG_FLUME,
    RCT1_RIDE_TYPE_RIVER_RAPIDS,
    RCT1_RIDE_TYPE_DODGEMS,
    RCT1_RIDE_TYPE_SWINGING_SHIP,
    RCT1_RIDE_TYPE_SWINGING_INVERTER_SHIP,
    RCT1_RIDE_TYPE_ICE_CREAM_STALL,
    RCT1_RIDE_TYPE_CHIPS_STALL,
    RCT1_RIDE_TYPE_DRINK_STALL,
    RCT1_RIDE_TYPE_CANDYFLOSS_STALL,
    RCT1_RIDE_TYPE_BURGER_BAR,
    RCT1_RIDE_TYPE_MERRY_GO_ROUND,
    RCT1_RIDE_TYPE_BALLOON_STALL,
    RCT1_RIDE_TYPE_INFORMATION_KIOSK,
    RCT1_RIDE_TYPE_TOILETS,
    RCT1_RIDE_TYPE_FERRIS_WHEEL,
    RCT1_RIDE_TYPE_MOTION_SIMULATOR,
    RCT1_RIDE_TYPE_3D_CINEMA,
    RCT1_RIDE_TYPE_GRAVITRON,
    RCT1_RIDE_TYPE_SPACE_RINGS,
    RCT1_RIDE_TYPE_REVERSE_FREEFALL_ROLLER_COASTER,
    RCT1_RIDE_TYPE_SOUVENIR_STALL,
    RCT1_RIDE_TYPE_VERTICAL_ROLLER_COASTER,
    RCT1_RIDE_TYPE_PIZZA_STALL,
    RCT1_RIDE_TYPE_SCRAMBLED_EGGS,
    RCT1_RIDE_TYPE_HAUNTED_HOUSE,
    RCT1_RIDE_TYPE_POPCORN_STALL,
    RCT1_RIDE_TYPE_CIRCUS_SHOW,
    RCT1_RIDE_TYPE_GHOST_TRAIN,
    RCT1_RIDE_TYPE_STEEL_TWISTER_ROLLER_COASTER,
    RCT1_RIDE_TYPE_WOODEN_TWISTER_ROLLER_COASTER,
    RCT1_RIDE_TYPE_WOODEN_SIDE_FRICTION_ROLLER_COASTER,
    RCT1_RIDE_TYPE_STEEL_WILD_MOUSE_ROLLER_COASTER,
    RCT1_RIDE_TYPE_HOT_DOG_STALL,
    RCT1_RIDE_TYPE_EXOTIC_SEA_FOOD_STALL,
    RCT1_RIDE_TYPE_HAT_STALL,
    RCT1_RIDE_TYPE_CANDY_APPLE_STAND,
    RCT1_RIDE_TYPE_VIRGINIA_REEL,
    RCT1_RIDE_TYPE_RIVER_RIDE,
    RCT1_RIDE_TYPE_CYCLE_MONORAIL,
    RCT1_RIDE_TYPE_FLYING_ROLLER_COASTER,
    RCT1_RIDE_TYPE_SUSPENDED_MONORAIL,
    RCT1_RIDE_TYPE_40,
    RCT1_RIDE_TYPE_WOODEN_REVERSER_ROLLER_COASTER,
    RCT1_RIDE_TYPE_HEARTLINE_TWISTER_ROLLER_COASTER,
    RCT1_RIDE_TYPE_MINIATURE_GOLF,
    RCT1_RIDE_TYPE_44,
    RCT1_RIDE_TYPE_ROTO_DROP,
    RCT1_RIDE_TYPE_FLYING_SAUCERS,
    RCT1_RIDE_TYPE_CROOKED_HOUSE,
    RCT1_RIDE_TYPE_CYCLE_RAILWAY,
    RCT1_RIDE_TYPE_SUSPENDED_LOOPING_ROLLER_COASTER,
    RCT1_RIDE_TYPE_WATER_COASTER,
    RCT1_RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER,
    RCT1_RIDE_TYPE_INVERTED_WILD_MOUSE_COASTER,
    RCT1_RIDE_TYPE_JET_SKIS,
    RCT1_RIDE_TYPE_T_SHIRT_STALL,
    RCT1_RIDE_TYPE_RAFT_RIDE,
    RCT1_RIDE_TYPE_DOUGHNUT_SHOP,
    RCT1_RIDE_TYPE_ENTERPRISE,
    RCT1_RIDE_TYPE_COFFEE_SHOP,
    RCT1_RIDE_TYPE_FRIED_CHICKEN_STALL,
    RCT1_RIDE_TYPE_LEMONADE_STALL
};

enum {
    RCT1_VEHICLE_TYPE_STEEL_ROLLER_COASTER_TRAIN = 0,
    RCT1_VEHICLE_TYPE_STEEL_ROLLER_COASTER_TRAIN_BACKWARDS,
    RCT1_VEHICLE_TYPE_WOODEN_ROLLER_COASTER_TRAIN,
    RCT1_VEHICLE_TYPE_INVERTED_COASTER_TRAIN, // Not in RCT2
    RCT1_VEHICLE_TYPE_SUSPENDED_SWINGING_CARS,
    RCT1_VEHICLE_TYPE_LADYBIRD_CARS,
    RCT1_VEHICLE_TYPE_STANDUP_ROLLER_COASTER_CARS,
    RCT1_VEHICLE_TYPE_SPINNING_CARS,
    RCT1_VEHICLE_TYPE_SINGLE_PERSON_SWINGING_CHAIRS,
    RCT1_VEHICLE_TYPE_SWANS_PEDAL_BOATS,
    RCT1_VEHICLE_TYPE_LARGE_MONORAIL_TRAIN,
    RCT1_VEHICLE_TYPE_CANOES,
    RCT1_VEHICLE_TYPE_ROWING_BOATS,
    RCT1_VEHICLE_TYPE_STEAM_TRAIN,
    RCT1_VEHICLE_TYPE_WOODEN_MOUSE_CARS,
    RCT1_VEHICLE_TYPE_BUMPER_BOATS,
    RCT1_VEHICLE_TYPE_WOODEN_ROLLER_COASTER_TRAIN_BACKWARDS,
    RCT1_VEHICLE_TYPE_ROCKET_CARS,
    RCT1_VEHICLE_TYPE_HORSES, // Steeplechase
    RCT1_VEHICLE_TYPE_SPORTSCARS,
    RCT1_VEHICLE_TYPE_LYING_DOWN_SWINGING_CARS, // Inverted single-rail
    RCT1_VEHICLE_TYPE_WOODEN_MINE_CARS,
    RCT1_VEHICLE_TYPE_SUSPENDED_SWINGING_AIRPLANE_CARS,
    RCT1_VEHICLE_TYPE_SMALL_MONORAIL_CARS,
    RCT1_VEHICLE_TYPE_WATER_TRICYCLES,
    RCT1_VEHICLE_TYPE_LAUNCHED_FREEFALL_CAR,
    RCT1_VEHICLE_TYPE_BOBSLEIGH_CARS,
    RCT1_VEHICLE_TYPE_DINGHIES,
    RCT1_VEHICLE_TYPE_ROTATING_CABIN,
    RCT1_VEHICLE_TYPE_MINE_TRAIN,
    RCT1_VEHICLE_TYPE_CHAIRLIFT_CARS,
    RCT1_VEHICLE_TYPE_CORKSCREW_ROLLER_COASTER_TRAIN,
    RCT1_VEHICLE_TYPE_MOTORBIKES,
    RCT1_VEHICLE_TYPE_RACING_CARS,
    RCT1_VEHICLE_TYPE_TRUCKS,
    RCT1_VEHICLE_TYPE_GO_KARTS,
    RCT1_VEHICLE_TYPE_RAPIDS_BOATS,
    RCT1_VEHICLE_TYPE_LOG_FLUME_BOATS,
    RCT1_VEHICLE_TYPE_DODGEMS,
    RCT1_VEHICLE_TYPE_SWINGING_SHIP,
    RCT1_VEHICLE_TYPE_SWINGING_INVERTER_SHIP,
    RCT1_VEHICLE_TYPE_MERRY_GO_ROUND,
    RCT1_VEHICLE_TYPE_FERRIS_WHEEL,
    RCT1_VEHICLE_TYPE_SIMULATOR_POD,
    RCT1_VEHICLE_TYPE_CINEMA_BUILDING,
    RCT1_VEHICLE_TYPE_TOPSPIN_CAR,
    RCT1_VEHICLE_TYPE_SPACE_RINGS,
    RCT1_VEHICLE_TYPE_REVERSE_FREEFALL_ROLLER_COASTER_CAR,
    RCT1_VEHICLE_TYPE_VERTICAL_ROLLER_COASTER_CARS,
    RCT1_VEHICLE_TYPE_CAT_CARS,
    RCT1_VEHICLE_TYPE_TWIST_ARMS_AND_CARS,
    RCT1_VEHICLE_TYPE_HAUNTED_HOUSE_BUILDING,
    RCT1_VEHICLE_TYPE_LOG_CARS,
    RCT1_VEHICLE_TYPE_CIRCUS_TENT,
    RCT1_VEHICLE_TYPE_GHOST_TRAIN_CARS,
    RCT1_VEHICLE_TYPE_STEEL_TWISTER_ROLLER_COASTER_TRAIN,
    RCT1_VEHICLE_TYPE_WOODEN_TWISTER_ROLLER_COASTER_TRAIN,
    RCT1_VEHICLE_TYPE_WOODEN_SIDE_FRICTION_CARS,
    RCT1_VEHICLE_TYPE_VINTAGE_CARS,
    RCT1_VEHICLE_TYPE_STEAM_TRAIN_COVERED_CARS,
    RCT1_VEHICLE_TYPE_STAND_UP_STEEL_TWISTER_ROLLER_COASTER_TRAIN,
    RCT1_VEHICLE_TYPE_FLOORLESS_STEEL_TWISTER_ROLLER_COASTER_TRAIN,
    RCT1_VEHICLE_TYPE_STEEL_MOUSE_CARS,
    RCT1_VEHICLE_TYPE_CHAIRLIFT_CARS_ALTERNATIVE,
    RCT1_VEHICLE_TYPE_SUSPENDED_MONORAIL_TRAIN,
    RCT1_VEHICLE_TYPE_HELICOPTER_CARS,
    RCT1_VEHICLE_TYPE_VIRGINIA_REEL_TUBS,
    RCT1_VEHICLE_TYPE_REVERSER_CARS,
    RCT1_VEHICLE_TYPE_GOLFERS,
    RCT1_VEHICLE_TYPE_RIVER_RIDE_BOATS,
    RCT1_VEHICLE_TYPE_FLYING_ROLLER_COASTER_TRAIN,
    RCT1_VEHICLE_TYPE_NON_LOOPING_STEEL_TWISTER_ROLLER_COASTER_TRAIN,
    RCT1_VEHICLE_TYPE_HEARTLINE_TWISTER_CARS,
    RCT1_VEHICLE_TYPE_HEARTLINE_TWISTER_CARS_REVERSED,
    RCT1_VEHICLE_TYPE_RESERVED,
    RCT1_VEHICLE_TYPE_ROTODROP_CAR,
    RCT1_VEHICLE_TYPE_FLYING_SAUCERS,
    RCT1_VEHICLE_TYPE_CROOKED_HOUSE_BUILDING,
    RCT1_VEHICLE_TYPE_BICYCLES,
    RCT1_VEHICLE_TYPE_HYPERCOASTER_TRAIN,
    RCT1_VEHICLE_TYPE_4_ACROSS_INVERTED_COASTER_TRAIN,
    RCT1_VEHICLE_TYPE_WATER_COASTER_BOATS,
    RCT1_VEHICLE_TYPE_FACEOFF_CARS,
    RCT1_VEHICLE_TYPE_JET_SKIS,
    RCT1_VEHICLE_TYPE_RAFT_BOATS,
    RCT1_VEHICLE_TYPE_AMERICAN_STYLE_STEAM_TRAIN,
    RCT1_VEHICLE_TYPE_AIR_POWERED_COASTER_TRAIN,
    RCT1_VEHICLE_TYPE_SUSPENDED_WILD_MOUSE_CARS, // Inverted Hairpin in RCT2
    RCT1_VEHICLE_TYPE_ENTERPRISE_WHEEL
};

enum {
    RCT1_TRACK_ELEM_BOOSTER = 100
};

enum {
    RCT1_RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE = 2,
    RCT1_RIDE_MODE_POWERED_LAUNCH = 3,
};

enum {
    RCT1_RIDE_DEPART_PLAY_MUSIC = 1 << 5,
};

enum {
    RCT1_SCENERY_THEME_GENERAL,
    RCT1_SCENERY_THEME_MINE,
    RCT1_SCENERY_THEME_CLASSICAL_ROMAN,
    RCT1_SCENERY_THEME_EGYPTIAN,
    RCT1_SCENERY_THEME_MARTIAN,
    RCT1_SCENERY_THEME_JUMPING_FOUNTAINS,   // Single researchable scenery item
    RCT1_SCENERY_THEME_WONDERLAND,
    RCT1_SCENERY_THEME_JURASSIC,
    RCT1_SCENERY_THEME_SPOOKY,
    RCT1_SCENERY_THEME_JUNGLE,
    RCT1_SCENERY_THEME_ABSTRACT,
    RCT1_SCENERY_THEME_GARDEN_CLOCK,        // Single researchable scenery item
    RCT1_SCENERY_THEME_SNOW_ICE,
    RCT1_SCENERY_THEME_MEDIEVAL,
    RCT1_SCENERY_THEME_SPACE,
    RCT1_SCENERY_THEME_CREEPY,
    RCT1_SCENERY_THEME_URBAN,
    RCT1_SCENERY_THEME_PAGODA,
};

enum {
    RCT1_FOOTPATH_TYPE_QUEUE_BLUE,
    RCT1_FOOTPATH_TYPE_QUEUE_RED,
    RCT1_FOOTPATH_TYPE_QUEUE_YELLOW,
    RCT1_FOOTPATH_TYPE_QUEUE_GREEN,

    RCT1_FOOTPATH_TYPE_TARMAC_GRAY,
    RCT1_FOOTPATH_TYPE_TARMAC_RED,
    RCT1_FOOTPATH_TYPE_TARMAC_BROWN,
    RCT1_FOOTPATH_TYPE_TARMAC_GREEN,

    RCT1_FOOTPATH_TYPE_DIRT_RED,
    RCT1_FOOTPATH_TYPE_DIRT_BLACK,

    RCT1_FOOTPATH_TYPE_CRAZY_PAVING = 12,

    RCT1_FOOTPATH_TYPE_ROADS = 16,

    RCT1_FOOTPATH_TYPE_TILE_PINK = 20,
    RCT1_FOOTPATH_TYPE_TILE_GRAY,
    RCT1_FOOTPATH_TYPE_TILE_RED,
    RCT1_FOOTPATH_TYPE_TILE_GREEN,
};

enum {
    FOOTPATH_SUPPORTS_WOODEN_TRUSS,
    FOOTPATH_SUPPORTS_WOOD,
    FOOTPATH_SUPPORTS_STEEL,
    FOOTPATH_SUPPORTS_BAMBOO,
};

enum {
    RCT1_PATH_ADDITION_NONE,
    RCT1_PATH_ADDITION_LAMP_1,
    RCT1_PATH_ADDITION_LAMP_2,
    RCT1_PATH_ADDITION_BIN,
    RCT1_PATH_ADDITION_BENCH,
    RCT1_PATH_ADDITION_JUMPING_FOUNTAIN,
    RCT1_PATH_ADDITION_LAMP_3,
    RCT1_PATH_ADDITION_LAMP_4,
    RCT1_PATH_ADDITION_BROKEN_LAMP_1,
    RCT1_PATH_ADDITION_BROKEN_LAMP_2,
    RCT1_PATH_ADDITION_BROKEN_BIN,
    RCT1_PATH_ADDITION_BROKEN_BENCH,
    RCT1_PATH_ADDITION_BROKEN_LAMP_3,
    RCT1_PATH_ADDITION_BROKEN_LAMP_4,
    RCT1_PATH_ADDITION_JUMPING_SNOW,
};

enum {
    RCT1_WALL_TYPE_GLASS_SMOOTH = 11,
    RCT1_WALL_TYPE_GLASS_PANELS = 22,
    RCT1_WALL_TYPE_WOODEN_PANEL_FENCE = 12,
    RCT1_WALL_TYPE_WOODEN_PANEL_FENCE_WITH_GATE = 13,
    RCT1_WALL_TYPE_WHITE_WOODEN_PANEL_FENCE = 26,
    RCT1_WALL_TYPE_RED_WOODEN_PANEL_FENCE = 27,
    RCT1_WALL_TYPE_SMALL_GREY_CASTLE = 35,
    RCT1_WALL_TYPE_LARGE_CREY_CASTLE = 42,
    RCT1_WALL_TYPE_LARGE_CREY_CASTLE_CROSS = 43,
    RCT1_WALL_TYPE_LARGE_CREY_CASTLE_GATE = 44,
    RCT1_WALL_TYPE_LARGE_CREY_CASTLE_WINDOW = 45,
    RCT1_WALL_TYPE_MEDIUM_CREY_CASTLE = 46,
    RCT1_WALL_TYPE_WOODEN_PANEL_FENCE_WITH_SNOW = 50,
};

enum {
    RCT1_RESEARCH_END_AVAILABLE = 0xFF,
    RCT1_RESEARCH_END_RESEARCHABLE = 0xFE,
    RCT1_RESEARCH_END = 0xFD,
};

enum {
    RCT1_RESEARCH_CATEGORY_THEME,
    RCT1_RESEARCH_CATEGORY_RIDE,
    RCT1_RESEARCH_CATEGORY_VEHICLE,
    RCT1_RESEARCH_CATEGORY_SPECIAL,
};

enum {
    RCT1_RESEARCH_EXPENDITURE_ROLLERCOASTERS         = 1 << 0,
    RCT1_RESEARCH_EXPENDITURE_THRILL_RIDES           = 1 << 1,
    RCT1_RESEARCH_EXPENDITURE_GENTLE_TRANSPORT_RIDES = 1 << 2,
    RCT1_RESEARCH_EXPENDITURE_SHOPS                  = 1 << 3,
    RCT1_RESEARCH_EXPENDITURE_SCENERY_THEMEING       = 1 << 4,
    RCT1_RESEARCH_EXPENDITURE_RIDE_IMPROVEMENTS      = 1 << 5,
};

// Unconfirmed special track elements for research
enum {
    RCT1_RESEARCH_SPECIAL_BANKED_CURVES = 0x06,
    RCT1_RESEARCH_SPECIAL_VERTICAL_LOOP = 0x07,
    RCT1_RESEARCH_SPECIAL_STEEP_TWIST = 0x0C,
    RCT1_RESEARCH_SPECIAL_INLINE_TWIST = 0x11,
    RCT1_RESEARCH_SPECIAL_HALF_LOOP = 0x12,
    RCT1_RESEARCH_SPECIAL_CORKSCREW = 0x13,
    RCT1_RESEARCH_SPECIAL_BANKED_HELIX_A = 0x15,
    RCT1_RESEARCH_SPECIAL_BANKED_HELIX_B = 0x16,
    RCT1_RESEARCH_SPECIAL_HELIX = 0x17,
    RCT1_RESEARCH_SPECIAL_ON_RIDE_PHOTO = 0x1A,
    RCT1_RESEARCH_SPECIAL_WATER_SPLASH = 0x1B,
    RCT1_RESEARCH_SPECIAL_VERTICAL_DROP = 0x1C,
    RCT1_RESEARCH_SPECIAL_BARREL_ROLL = 0x1D,
    RCT1_RESEARCH_SPECIAL_LAUNCHED_LIFT_HILL = 0x1E,
    RCT1_RESEARCH_SPECIAL_LARGE_LOOP_AND_HALF = 0x1F,
    RCT1_RESEARCH_SPECIAL_REVERSER_TURNTABLE = 0x21,
    RCT1_RESEARCH_SPECIAL_HEARTLINE_ROLL = 0x22,
    RCT1_RESEARCH_SPECIAL_REVERSING_SECTIONS = 0x23,
};

enum {
    RCT1_SCENARIO_FLAG_0 = 1 << 0,
    RCT1_SCENARIO_FLAG_1 = 1 << 1,
    RCT1_SCENARIO_FLAG_2 = 1 << 2,
    RCT1_SCENARIO_FLAG_3 = 1 << 3,
    RCT1_SCENARIO_FLAG_ENABLE_BANNERS = 1 << 4,
    RCT1_SCENARIO_FLAG_5 = 1 << 5,
    RCT1_SCENARIO_FLAG_6 = 1 << 6,
    RCT1_SCENARIO_FLAG_7 = 1 << 7,
    RCT1_SCENARIO_FLAG_CUSTOM_PARK_ENTRANCE_PATH = 1 << 8,
    RCT1_SCENARIO_FLAG_NO_CASH_RESET = 1 << 9,
    RCT1_SCENARIO_FLAG_10 = 1 << 10,
    RCT1_SCENARIO_FLAG_11 = 1 << 11,
    RCT1_SCENARIO_FLAG_12 = 1 << 12,
    RCT1_SCENARIO_FLAG_CUSTOM_MAP_SIZE = 1 << 13,
    RCT1_SCENARIO_FLAG_14 = 1 << 14,
    RCT1_SCENARIO_FLAG_15 = 1 << 15,
    RCT1_SCENARIO_FLAG_16 = 1 << 16,
    RCT1_SCENARIO_FLAG_17 = 1 << 17,
    RCT1_SCENARIO_FLAG_18 = 1 << 18,
    RCT1_SCENARIO_FLAG_19 = 1 << 19,
};

enum {
    RCT1_PARK_FLAGS_PARK_OPEN =                         (1 << 0),
    RCT1_PARK_FLAGS_SCENARIO_COMPLETE_NAME_INPUT =      (1 << 1),
    RCT1_PARK_FLAGS_FORBID_LANDSCAPE_CHANGES =          (1 << 2),
    RCT1_PARK_FLAGS_FORBID_TREE_REMOVAL =               (1 << 3),
    RCT1_PARK_FLAGS_SHOW_REAL_GUEST_NAMES =             (1 << 4),
    RCT1_PARK_FLAGS_FORBID_HIGH_CONSTRUCTION =          (1 << 5),   // Below tree height
    RCT1_PARK_FLAGS_PREF_LESS_INTENSE_RIDES =           (1 << 6),
    RCT1_PARK_FLAGS_FORBID_MARKETING_CAMPAIGN =         (1 << 7),
    RCT1_PARK_FLAGS_ANTI_CHEAT_DEPRECATED =             (1 << 8),   // Not used anymore, used for cheat detection
    RCT1_PARK_FLAGS_PREF_MORE_INTENSE_RIDES =           (1 << 9),
    RCT1_PARK_FLAGS_NO_MONEY =                          (1 << 11),  // Used for both scenarios and saved games, unlike RCT2
    RCT1_PARK_FLAGS_DIFFICULT_GUEST_GENERATION =        (1 << 12),
    RCT1_PARK_FLAGS_PARK_ENTRY_LOCKED_AT_FREE =         (1 << 13),  // Off: rides and park entry chargeable. On: only rides chargeable.
    RCT1_PARK_FLAGS_DIFFICULT_PARK_RATING =             (1 << 14),
    RCT1_PARK_FLAGS_LOCK_REAL_NAMES_OPTION_DEPRECATED = (1 << 15),  // Deprecated now we use a persistent 'real names' setting
};

enum {
    STEEL_RC_FRONT = 0,
    STEEL_RC_CARRIAGE = 1,
    WOODEN_RC_TRAIN = 2,
    MONORAIL_CARRIAGE = 10,
    MONORAIL_FRONT = 11,
    MONORAIL_BACK = 12,
    MINIATURE_RAILWAY_TENDER = 15,
    MINIATURE_RAILWAY_LOCOMOTIVE = 16,
    MINIATURE_RAILWAY_CARRIAGE = 17,
    MINE_TRAIN_FRONT = 35,
    MINE_TRAIN_CARRIAGE = 36,
    CORKSCREW_RC_FRONT = 38,
    CORKSCREW_RC_CARRIAGE = 39,
    GHOST_TRAIN_CAR = 63,
    TWISTER_RC_SPOILER = 64,
    TWISTER_RC_CARRIAGE = 65,
    GHOST_TRAIN_INVISIBLE = 66,
    ARTICULATED_RC_FRONT = 67,
    ARTICULATED_RC_CARRIAGE = 68,
    MINIATURE_RAILWAY_CARRIAGE_COVERED = 71,
    STANDUP_TWISTER_RC_CARRIAGE = 72,
    REVERSER_RC_CAR = 79,
    REVERSER_RC_BOGIE = 80,
    MINIGOLF_PLAYER = 81,
    MINIGOLF_BALL = 82,
    SPLASH_BOAT = 83,
    SPLASH_BOAT_INVISIBLE = 84,
    HYPERCOASTER_FRONT = 96,
    HYPERCOASTER_CARRIAGE = 97,
    INVERTED_4_ACROSS_CARRIAGE = 98,
    WATER_COASTER_BOAT = 99,
    WATER_COASTER_INVISIBLE = 101,
    RIVER_RAFT = 103,
};

enum {
    COPY_COLOUR_1 = -1,
    COPY_COLOUR_2 = -2,
};

enum {
    RCT1_WATER_CYAN,
    RCT1_WATER_ORANGE
};

extern const uint8 gRideCategories[RIDE_TYPE_COUNT];

bool rideTypeShouldLoseSeparateFlag(const rct_ride_entry *rideEntry);

ParkLoadResult * load_from_sv4(const char *path);
ParkLoadResult * load_from_sc4(const char *path);

colour_t rct1_get_colour(colour_t colour);

#endif
