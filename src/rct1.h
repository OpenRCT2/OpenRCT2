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

#ifndef _RCT1_H_
#define _RCT1_H_

#include "management/award.h"
#include "management/news_item.h"
#include "management/research.h"
#include "rct2.h"
#include "ride/ride.h"
#include "world/banner.h"
#include "world/map.h"
#include "world/sprite.h"

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
	uint8 type;
	uint8 vehicle_type;
	uint16 lifecycle_flags;
	uint8 operating_mode;
	uint8 colour_scheme;
	struct {
		colour_t body;
		colour_t trim;
	} vehicle_colours[12];
	colour_t track_primary_colour;
	colour_t track_secondary_colour;
	colour_t track_support_colour;
	uint8 status;
	uint16 name;
	uint16 name_argument_ride;
	uint16 name_argument_number;
	uint16 overall_view;
	uint16 station_starts[4];
	uint8 station_height[4];
	uint8 station_length[4];
	uint8 station_light[4];
	uint8 station_depart[4];
	uint16 entrance[4];
	uint16 exit[4];
	uint16 last_peep_in_queue[4];
	uint8 num_peeps_in_queue[4];
	uint16 vehicles[12];
	uint8 depart_flags;
	uint8 num_stations;
	uint8 num_trains;
	uint8 num_cars_per_train;
	uint8 unk_7A;
	uint8 unk_7B;
	uint8 max_trains;
	uint8 unk_7D;
	uint8 min_waiting_time;
	uint8 max_waiting_time;
	uint8 operation_option;
	uint8 unk_081[0x3];
	uint8 data_logging_index;
	uint8 special_track_elements;
	uint16 unk_86;
	sint32 max_speed;
	sint32 average_speed;
	uint8 pad_090[4];
	sint32 length[4];
	uint16 time[4];
	fixed16_2dp max_positive_vertical_g;
	fixed16_2dp max_negative_vertical_g;
	fixed16_2dp max_lateral_g;
	uint8 unk_B2[18];
	union {
		uint8 num_inversions;
		uint8 num_holes;
	};
	uint8 num_drops;
	uint8 unk_C6;
	uint8 highest_drop_height;
	sint32 sheltered_length;
	uint8 unk_CC[2];
	uint8 num_sheltered_sections;
	uint8 unk_CF;
	sint16 unk_D0;
	sint16 unk_D2;
	sint16 customers_per_hour;
	sint16 unk_D6;
	sint16 unk_D8;
	sint16 unk_DA;
	sint16 unk_DC;
	sint16 unk_DE;
	uint16 age;
	sint16 running_cost;
	sint16 unk_E4;
	sint16 unk_E6;
	money16 price;
	sint16 var_EA;
	sint16 var_EC;
	uint8 var_EE;
	uint8 var_EF;
	union {
		rating_tuple ratings;
		struct {
			ride_rating excitement;
			ride_rating intensity;
			ride_rating nausea;
		};
	};
	uint16 value;
	uint16 var_F8;
	uint8 satisfaction;
	uint8 satisfaction_time_out;
	uint8 satisfaction_next;
	uint8 window_invalidate_flags;
	uint8 unk_FE[2];
	uint32 total_customers;
	money32 total_profit;
	uint8 popularity;
	uint8 popularity_time_out;
	uint8 popularity_next;
	uint8 num_riders;
	uint8 unk_10C[36];
	sint16 build_date;
	money16 upkeep_cost;
	uint8 unk_134[15];
	uint8 breakdown_reason;
	uint8 unk_144[2];
	uint16 reliability;
	uint8 unreliability_factor;
	uint8 unk_148;
	uint8 inspection_interval;
	uint8 last_inspection;
	uint8 unk_14C[20];
	money32 income_per_hour;
	money32 profit;
	uint8 queue_time[4];
	colour_t track_colour_main[4];
	colour_t track_colour_additional[4];
	colour_t track_colour_supports[4];
	uint8 music;
	uint8 entrance_style;
	uint8 unk_17A[230];
} rct1_ride;
assert_struct_size(rct1_ride, 0x260);

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
	rct_map_element map_elements[0xC000];
	uint32 unk_counter;
	rct_sprite sprites[5000];
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
	rct2_peep_spawn peep_spawn[2];
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
	rct_award awards[4];
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
	rct_news_item messages[61];
	char scenario_name[62];
	uint16 scenario_slot_index;
	uint32 scenario_flags;
	uint8 unk_1F8358[432];
	uint32 expansion_pack_checksum;
} rct1_s4;
assert_struct_size(rct1_s4, 0x1F850C);

typedef struct rct_track_td4 {
	uint8 type;										// 0x00
	uint8 vehicle_type;								// 0x01
	uint32 special_track_flags;						// 0x02
	uint8 operating_mode;							// 0x06
	uint8 vehicle_colour_version;					// 0x07 Vehicle colour type in first two bits, Version in bits 3,4
	colour_t body_trim_colour[24];					// 0x08
	colour_t track_spine_colour_rct1;				// 0x20
	colour_t track_rail_colour_rct1;				// 0x21
	colour_t track_support_colour_rct1;				// 0x22
	uint8 departure_control_flags;					// 0x23
	uint8 number_of_trains;							// 0x24
	uint8 cars_per_train;							// 0x25
	uint8 min_wait_time;							// 0x26
	uint8 max_wait_time;							// 0x27
	uint8 speed;									// 0x28
	uint8 max_speed;								// 0x29
	uint8 average_speed;							// 0x2A
	uint16 ride_length;								// 0x2B
	uint8 max_positive_vertical_g;					// 0x2D
	uint8 max_negitive_vertical_g;					// 0x2E
	uint8 max_lateral_g;							// 0x2F
	union {
		uint8 inversions;							// 0x30
		uint8 holes;								// 0x30
	};
	uint8 drops;									// 0x31
	uint8 highest_drop_height;						// 0x32
	uint8 excitement;								// 0x33
	uint8 intensity;								// 0x34
	uint8 nausea;									// 0x35
	uint8 pad_36[2];
	union{
		uint16 start_track_data_original;			// 0x38
		colour_t track_spine_colour[4];				// 0x38
	};
	colour_t track_rail_colour[4];					// 0x3C
	union{
		colour_t track_support_colour[4];			// 0x40
		uint8 wall_type[4];							// 0x40
	};
	uint8 pad_41[0x83];
	uint16 start_track_data_AA_CF;					// 0xC4
} rct_track_td4; // Information based off RCTTechDepot
assert_struct_size(rct_track_td4, 0xC9);
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
	RCT1_RIDE_TYPE_FRIES_STALL,
	RCT1_RIDE_TYPE_DRINK_STALL,
	RCT1_RIDE_TYPE_COTTON_CANDY_STALL,
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
	RCT1_RIDE_TYPE_REVERSE_WHOA_BELLY_ROLLER_COASTER,
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
	RCT1_SCENERY_THEME_GENERAL,
	RCT1_SCENERY_THEME_MINE,
	RCT1_SCENERY_THEME_CLASSICAL_ROMAN,
	RCT1_SCENERY_THEME_EGYPTIAN,
	RCT1_SCENERY_THEME_MARTIAN,
	RCT1_SCENERY_THEME_JUMPING_FOUNTAINS,	// Single researchable scenery item
	RCT1_SCENERY_THEME_WONDERLAND,
	RCT1_SCENERY_THEME_JURASSIC,
	RCT1_SCENERY_THEME_SPOOKY,
	RCT1_SCENERY_THEME_JUNGLE,
	RCT1_SCENERY_THEME_ABSTRACT,
	RCT1_SCENERY_THEME_GARDEN_CLOCK,		// Single researchable scenery item
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
	RCT1_PARK_FLAGS_PARK_OPEN = 					(1 << 0),
	RCT1_PARK_FLAGS_SCENARIO_COMPLETE_NAME_INPUT = 	(1 << 1),
	RCT1_PARK_FLAGS_FORBID_LANDSCAPE_CHANGES = 		(1 << 2),
	RCT1_PARK_FLAGS_FORBID_TREE_REMOVAL = 			(1 << 3),
	RCT1_PARK_FLAGS_SHOW_REAL_GUEST_NAMES = 		(1 << 4),
	RCT1_PARK_FLAGS_FORBID_HIGH_CONSTRUCTION = 		(1 << 5), 	// Below tree height
	RCT1_PARK_FLAGS_PREF_LESS_INTENSE_RIDES = 		(1 << 6),
	RCT1_PARK_FLAGS_FORBID_MARKETING_CAMPAIGN = 	(1 << 7),
	RCT1_PARK_FLAGS_ANTI_CHEAT_DEPRECATED = 		(1 << 8), 	// Not used anymore, used for cheat detection
	RCT1_PARK_FLAGS_PREF_MORE_INTENSE_RIDES = 		(1 << 9),
	RCT1_PARK_FLAGS_NO_MONEY = 						(1 << 11),	// Used for both scenarios and saved games, unlike RCT2
	RCT1_PARK_FLAGS_DIFFICULT_GUEST_GENERATION = 	(1 << 12),
	RCT1_PARK_FLAGS_PARK_ENTRY_LOCKED_AT_FREE = 	(1 << 13),	// Off: rides and park entry chargeable. On: only rides chargeable.
	RCT1_PARK_FLAGS_DIFFICULT_PARK_RATING = 		(1 << 14),
	RCT1_PARK_FLAGS_LOCK_REAL_NAMES_OPTION = 		(1 << 15),
};

extern const uint8 gRideCategories[0x60];

bool rct1_read_sc4(const char *path, rct1_s4 *s4);
bool rct1_read_sv4(const char *path, rct1_s4 *s4);
void rct1_import_s4(rct1_s4 *s4);
void rct1_fix_landscape();
int vehicle_preference_compare(uint8 rideType, const char * a, const char * b);
bool rideTypeShouldLoseSeparateFlag(rct_ride_entry *rideEntry);

bool rct1_load_saved_game(const char *path);
bool rct1_load_scenario(const char *path);

colour_t rct1_get_colour(colour_t colour);

#endif
