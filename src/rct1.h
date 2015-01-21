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

typedef struct {
	uint16 x;
	uint16 y;
	uint16 z;
	uint8 direction;
} rct1_entrance;

/**
 * RCT1,AA,LL scenario / saved game structure.
 * size: 0x1F850C
 */
typedef struct {
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
	uint8 handman_colour;
	uint8 mechanic_colour;
	uint8 security_guard_colour;
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
	rct_research_item research_items[200];
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
	uint16 map_size_unk_a;
	uint16 map_size_unk_b;
	uint16 map_size;
	uint16 map_size_unk_c;
	uint32 same_price_flags;
	uint16 unk_199C94;
	uint8 unk_199C96[3];
	uint8 water_colour;
	uint16 unk_199C9A;
	rct_research_item research_items_LL[180];
	uint8 unk_19A020[5468];
	rct_banner banners[100];
	char string_table[32][1024];
	uint32 game_time_counter;
	rct_ride rides[255];
	uint16 unk_game_time_counter;
	uint16 view_x;
	uint16 view_y;
	uint8 view_zoom;
	uint8 view_rotation;
	uint8 animated_objects[6000];
	uint32 num_animated_objects;
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

#endif
 