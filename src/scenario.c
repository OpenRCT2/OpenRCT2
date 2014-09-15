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

#include <windows.h>
#include <string.h>
#include "addresses.h"
#include "award.h"
#include "date.h"
#include "finance.h"
#include "game.h"
#include "map.h"
#include "marketing.h"
#include "news_item.h"
#include "object.h"
#include "park.h"
#include "rct2.h"
#include "ride.h"
#include "sawyercoding.h"
#include "scenario.h"
#include "string_ids.h"
#include "sprite.h"
#include "viewport.h"

/**
 * Loads only the basic information from a scenario.
 *  rct2: 0x006761D6
 */
int scenario_load_basic(const char *path)
{
	FILE *file;
	rct_s6_header *s6Header = (rct_s6_header*)0x009E34E4;
	rct_s6_info *s6Info = (rct_s6_info*)0x0141F570;

	file = fopen(path, "rb");
	if (file != NULL) {
		// Read first chunk
		sawyercoding_read_chunk(file, (uint8*)s6Header);
		if (s6Header->type == S6_TYPE_SCENARIO) {
			// Read second chunk
			sawyercoding_read_chunk(file, (uint8*)s6Info);
			fclose(file);
			RCT2_GLOBAL(0x009AA00C, uint8) = 0;

			// Checks for a scenario string object (possibly for localisation)
			if ((s6Info->entry.flags & 0xFF) != 255) {
				if (object_get_scenario_text(&s6Info->entry)) {
					int ebp = RCT2_GLOBAL(0x009ADAF8, uint32);
					format_string(s6Info->name, RCT2_GLOBAL(ebp, sint16), NULL);
					format_string(s6Info->details, RCT2_GLOBAL(ebp + 4, sint16), NULL);
					RCT2_GLOBAL(0x009AA00C, uint8) = RCT2_GLOBAL(ebp + 6, uint8);
					object_free_scenario_text();
				}
			}
			return 1;
		}
		fclose(file);
	}

	RCT2_GLOBAL(0x009AC31B, sint8) = -1;
	RCT2_GLOBAL(0x009AC31C, sint16) = 3011;
	return 0;
}

/**
 * 
 *  rct2: 0x00676053
 * scenario (ebx)
 */
void scenario_load(const char *path)
{
	FILE *file;
	int i, j;
	rct_s6_header *s6Header = (rct_s6_header*)0x009E34E4;
	rct_s6_info *s6Info = (rct_s6_info*)0x0141F570;

	file = fopen(path, "rb");
	if (file != NULL) {
		if (!sawyercoding_validate_checksum(file)) {
			fclose(file);
			RCT2_GLOBAL(0x009AC31B, uint8) = 255;
			RCT2_GLOBAL(0x009AC31C, uint16) = STR_FILE_CONTAINS_INVALID_DATA;
			return;
		}

		// Read first chunk
		sawyercoding_read_chunk(file, (uint8*)s6Header);
		if (s6Header->type == S6_TYPE_SCENARIO) {
			// Read second chunk
			sawyercoding_read_chunk(file, (uint8*)s6Info);

			// Read packed objects
			if (s6Header->num_packed_objects > 0) {
				j = 0;
				for (i = 0; i < s6Header->num_packed_objects; i++)
					j += object_load_packed();
				if (j > 0)
					object_list_load();
			}

			object_read_and_load_entries(file);

			// Read flags (16 bytes). Loads:
			//	RCT2_ADDRESS_CURRENT_MONTH_YEAR
			//	RCT2_ADDRESS_CURRENT_MONTH_TICKS
			//	RCT2_ADDRESS_SCENARIO_TICKS
			sawyercoding_read_chunk(file, (uint8*)RCT2_ADDRESS_CURRENT_MONTH_YEAR);

			// Read map elements
			memset((void*)RCT2_ADDRESS_MAP_ELEMENTS, 0, MAX_MAP_ELEMENTS * sizeof(rct_map_element));
			sawyercoding_read_chunk(file, (uint8*)RCT2_ADDRESS_MAP_ELEMENTS);

			// Read game data, including sprites
			sawyercoding_read_chunk(file, (uint8*)0x010E63B8);

			// Read number of guests in park and something else
			sawyercoding_read_chunk(file, (uint8*)RCT2_ADDRESS_GUESTS_IN_PARK);

			// Read ?
			sawyercoding_read_chunk(file, (uint8*)0x01357BC8);

			// Read park rating
			sawyercoding_read_chunk(file, (uint8*)RCT2_ADDRESS_CURRENT_PARK_RATING);

			// Read ?
			sawyercoding_read_chunk(file, (uint8*)RCT2_ADDRESS_ACTIVE_RESEARCH_TYPES);

			// Read ?
			sawyercoding_read_chunk(file, (uint8*)RCT2_ADDRESS_CURRENT_EXPENDITURE);

			// Read ?
			sawyercoding_read_chunk(file, (uint8*)RCT2_ADDRESS_CURRENT_PARK_VALUE);

			// Read more game data, including research items and rides
			sawyercoding_read_chunk(file, (uint8*)RCT2_ADDRESS_COMPLETED_COMPANY_VALUE);

			fclose(file);

			// Check expansion pack
			// RCT2_CALLPROC_EBPSAFE(0x006757E6);

			RCT2_CALLPROC_EBPSAFE(0x006A9FC0);
			map_update_tile_pointers();
			reset_0x69EBE4();// RCT2_CALLPROC_EBPSAFE(0x0069EBE4);
			return;
		}

		fclose(file);
	}

	RCT2_GLOBAL(0x009AC31B, uint8) = 255;
	RCT2_GLOBAL(0x009AC31C, uint16) = STR_FILE_CONTAINS_INVALID_DATA;
}

/**
 * 
 *  rct2: 0x00678282
 * scenario (ebx)
 */
void scenario_load_and_play(const rct_scenario_basic *scenario)
{
	rct_window *mainWindow;
	rct_s6_info *s6Info = (rct_s6_info*)0x0141F570;

	// Create the scenario pseduo-random seeds using the current time
	uint32 srand0, srand1;
	srand0 = RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_SRAND_0, uint32) ^ timeGetTime();
	srand1 = RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_SRAND_1, uint32) ^ timeGetTime();

	RCT2_CALLPROC_EBPSAFE(0x006CBCC3);

	subsitute_path(
		RCT2_ADDRESS(0x0141EF68, char),
		RCT2_ADDRESS(RCT2_ADDRESS_SCENARIOS_PATH, char),
		scenario->path
	);
	scenario_load((char*)0x0141EF68);
	RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) = SCREEN_FLAGS_PLAYING;
	viewport_init_all();
	game_create_windows();
	mainWindow = window_get_main();

	mainWindow->viewport_target_sprite = -1;
	mainWindow->saved_view_x = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_X, sint16);
	mainWindow->saved_view_y = RCT2_GLOBAL(RCT2_ADDRESS_SAVED_VIEW_Y, sint16);

	uint8 _cl = (RCT2_GLOBAL(0x0138869E, sint16) & 0xFF) - mainWindow->viewport->zoom;
	mainWindow->viewport->zoom = RCT2_GLOBAL(0x0138869E, sint16) & 0xFF;
	*((char*)(&RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, sint32))) = RCT2_GLOBAL(0x0138869E, sint16) >> 8;
	if (_cl != 0) {
		if (_cl < 0) {
			_cl = -_cl;
			mainWindow->viewport->view_width >>= _cl;
			mainWindow->viewport->view_height >>= _cl;
		} else {
			mainWindow->viewport->view_width <<= _cl;
			mainWindow->viewport->view_height <<= _cl;
		}
	}
	mainWindow->saved_view_x -= mainWindow->viewport->view_width >> 1;
	mainWindow->saved_view_y -= mainWindow->viewport->view_height >> 1;
	window_invalidate(mainWindow);

	sub_0x0069E9A7();// RCT2_CALLPROC_EBPSAFE(0x0069E9A7);
	window_new_ride_init_vars();

	// Set the scenario pseduo-random seeds
	RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_SRAND_0, sint32) = srand0;
	RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_SRAND_1, sint32) = srand1;

	RCT2_GLOBAL(0x009DEB7C, sint16) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, sint32) &= 0xFFFFF7FF;
	if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, sint32) & PARK_FLAGS_NO_MONEY_SCENARIO)
		RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, sint32) |= PARK_FLAGS_NO_MONEY;
	RCT2_CALLPROC_EBPSAFE(0x00684AC3);
	RCT2_CALLPROC_EBPSAFE(0x006DFEE4);
	news_item_init_queue();
	if (RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_TYPE, uint8) != OBJECTIVE_NONE)
		window_park_objective_open();

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PARK_RATING, sint16) = calculate_park_rating();
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PARK_VALUE, money32) = calculate_park_value();
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_COMPANY_VALUE, money32) = calculate_company_value();
	RCT2_GLOBAL(0x013587D0, money32) = RCT2_GLOBAL(0x013573DC, money32) - RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_LOAN, money32);
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONEY_ENCRYPTED, sint32) = ENCRYPT_MONEY(RCT2_GLOBAL(0x013573DC, sint32));
	sub_69E869(); // (loan related)

	strcpy((char*)RCT2_ADDRESS_SCENARIO_DETAILS, s6Info->details);
	strcpy((char*)RCT2_ADDRESS_SCENARIO_NAME, s6Info->name);

	if (RCT2_GLOBAL(0x009ADAE4, sint32) != -1) {
		char *ebp = RCT2_GLOBAL(0x009ADAE4, char*);

		// 
		format_string((char*)RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, RCT2_GLOBAL(ebp + 2, uint16), 0);
		
		// Set park name
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = STR_CANT_RENAME_PARK;
		game_do_command(1, 1, 0, *((int*)(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER + 0)), GAME_COMMAND_33,
			*((int*)(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER + 8)), 
			*((int*)(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER + 4)));
		game_do_command(2, 1, 0, *((int*)(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER + 12)), GAME_COMMAND_33, 
			*((int*)(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER + 20)),
			*((int*)(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER + 16)));
		game_do_command(0, 1, 0, *((int*)(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER + 24)), GAME_COMMAND_33,
			*((int*)(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER + 32)),
			*((int*)(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER + 28)));

		// 
		format_string((char*)RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, RCT2_GLOBAL(ebp + 0, uint16), 0);
		strncpy((char*)RCT2_ADDRESS_SCENARIO_NAME, (char*)RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, 31);
		((char*)RCT2_ADDRESS_SCENARIO_NAME)[31] = '\0';

		// Set scenario details
		format_string((char*)RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, RCT2_GLOBAL(ebp + 4, uint16), 0);
		strncpy((char*)RCT2_ADDRESS_SCENARIO_DETAILS, (char*)RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, 255);
		((char*)RCT2_ADDRESS_SCENARIO_DETAILS)[255] = '\0';
	}

	// Set the last saved game path
	strcpy((char*)RCT2_ADDRESS_SAVED_GAMES_PATH_2, (char*)RCT2_ADDRESS_SAVED_GAMES_PATH);
	format_string((char*)RCT2_ADDRESS_SAVED_GAMES_PATH_2 + strlen((char*)RCT2_ADDRESS_SAVED_GAMES_PATH_2), RCT2_GLOBAL(0x0013573D4, uint16), (void*)0x0013573D8);
	strcat((char*)RCT2_ADDRESS_SAVED_GAMES_PATH_2, ".SV6");

	memset((void*)0x001357848, 0, 56);
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_EXPENDITURE, uint32) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PROFIT, money32) = 0;
	RCT2_GLOBAL(0x01358334, money32) = 0;
	RCT2_GLOBAL(0x01358338, uint16) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_COMPLETED_COMPANY_VALUE, uint32) = 0x80000000;
	RCT2_GLOBAL(RCT2_ADDRESS_TOTAL_ADMISSIONS, uint32) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_INCOME_FROM_ADMISSIONS, uint32) = 0;
	RCT2_GLOBAL(0x013587D8, uint16) = 63;
	sub_69E869(); // (loan related, called above already)
	park_reset_history();
	finance_reset_history();
	award_reset();
	reset_all_ride_build_dates();
	date_reset();
	RCT2_CALLPROC_EBPSAFE(0x00674576);
	park_calculate_size();
	RCT2_CALLPROC_EBPSAFE(0x006C1955);
	RCT2_GLOBAL(0x01358840, uint8) = 0;
	memset((void*)0x001358102, 0, 20);
	RCT2_GLOBAL(0x00135882E, uint16) = 0;

	// Open park with free entry when there is no money
	if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY) {
		RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) |= PARK_FLAGS_PARK_OPEN;
		RCT2_GLOBAL(RCT2_ADDRESS_PARK_ENTRANCE_FEE, uint16) = 0;
	}

	RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) |= PARK_FLAGS_18;

	RCT2_CALLPROC_EBPSAFE(0x006837E3); // (palette related)

	gfx_invalidate_screen();
	RCT2_GLOBAL(0x009DEA66, uint16) = 0;
	RCT2_GLOBAL(0x009DEA5C, uint16) = 62000; // (doesn't appear to ever be read)
}


void scenario_end()
{
	rct_window* w;
	window_close_by_id(WC_DROPDOWN, 0);
	
	for (w = g_window_list; w < RCT2_GLOBAL(RCT2_ADDRESS_NEW_WINDOW_PTR, rct_window*); w++){
		if (!(w->flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT)))
			window_close(w);
	}
	window_park_objective_open();
}

/*
* rct2: 0x0066A752
**/
void scenario_failure()
{
	RCT2_GLOBAL(RCT2_ADDRESS_COMPLETED_COMPANY_VALUE, uint32) = 0x80000001;
	scenario_end();
}

/*
 * rct2: 0x0066A75E
 **/
void scenario_success()
{
	int i;
	rct_scenario_basic* scenario;
	uint32 current_val = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_COMPANY_VALUE, uint32);
	
	RCT2_GLOBAL(RCT2_ADDRESS_COMPLETED_COMPANY_VALUE, uint32) = current_val;
	peep_applause();

	for (i = 0; i < gScenarioListCount; i++) {
		char *cur_scenario_name = RCT2_ADDRESS(0x135936C, char);
		scenario = &gScenarioList[i];

		if (0 == strncmp(cur_scenario_name, scenario->path, 256)){
			if (scenario->flags & SCENARIO_FLAGS_COMPLETED && scenario->company_value < current_val)
				break; // not a new high score -> no glory

			// bts game_flags, 1 happens here but I don't know what for
			scenario->company_value = current_val;
			scenario->flags |= SCENARIO_FLAGS_COMPLETED;
			scenario->completed_by[0] = 0;
			RCT2_GLOBAL(0x013587C0, uint32) = current_val; // value used in window for score?
			scenario_scores_save();
			break;
		}
	}
	scenario_end();
}

/**
* Checks if there are 10 rollercoasters of different subtype with
* excitement >= 600 .
* rct2:
**/
void scenario_objective5_check()
{
	int i, rcs = 0;
	uint8 type_already_counted[256];
	rct_ride* ride;

	memset(type_already_counted, 0, 256);

	FOR_ALL_RIDES(i, ride) {
		uint8 subtype_id = ride->subtype;
		rct_ride_type *rideType = gRideTypeList[subtype_id];

		if ((rideType->category[0] == RIDE_GROUP_ROLLERCOASTER || rideType->category[1] == RIDE_GROUP_ROLLERCOASTER) &&
			ride->status == RIDE_STATUS_OPEN &&
			ride->excitement >= RIDE_RATING(6,00) && type_already_counted[subtype_id] == 0){
			type_already_counted[subtype_id]++;
			rcs++;
		}
	}

	if (rcs >= 10)
		scenario_success();
}

/**
 * Checks if there are 10 rollercoasters of different subtype with
 * excitement > 700 and a minimum length;
 * rct2: 0x0066A6B5
 **/
void scenario_objective8_check()
{
	int i, rcs = 0;
	uint8 type_already_counted[256];
	rct_ride* ride;
	sint16 objective_length = RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_NUM_GUESTS, uint16);

	memset(type_already_counted, 0, 256);

	FOR_ALL_RIDES(i, ride) {
		uint8 subtype_id = ride->subtype;
		rct_ride_type *rideType = gRideTypeList[subtype_id];
		if ((rideType->category[0] == RIDE_GROUP_ROLLERCOASTER || rideType->category[1] == RIDE_GROUP_ROLLERCOASTER) &&
			ride->status == RIDE_STATUS_OPEN &&
			ride->excitement >= RIDE_RATING(7,00) && type_already_counted[subtype_id] == 0){

			// this calculates the length, no idea why it's done so complicated though.
			uint8 limit = ride->var_0C7;
			uint32 sum = 0;
			for (int j = 0; j < limit; ++j) {
				sum += ((uint32*)&ride->var_0E4)[j];
			}
			if ((sum >> 16) > (uint32)objective_length) {
				type_already_counted[subtype_id]++;
				rcs++;
			}
		}
	}

	if (rcs >= 10)
		scenario_success();
}



/*
 * Checks the win/lose conditions of the current objective.
 * rct2: 0x0066A4B2
 **/
void scenario_objectives_check()
{
	uint8 objective_type = RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_TYPE, uint8),
		objective_year = RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_YEAR, uint8);
	sint16 park_rating = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PARK_RATING, sint16),
		guests_in_park = RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_IN_PARK, uint16),
		objective_guests = RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_NUM_GUESTS, uint16),
		cur_month_year = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, sint16);
	uint32 scenario_completed_company_value = RCT2_GLOBAL(RCT2_ADDRESS_COMPLETED_COMPANY_VALUE, uint32);
	sint32 objective_currency = RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_CURRENCY, sint32),
		park_value = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PARK_VALUE, sint32);


	if ( scenario_completed_company_value != 0x80000000)
		return;

	switch (objective_type) {
	case OBJECTIVE_GUESTS_BY://1

		if (cur_month_year == 8 * objective_year){
			if (park_rating >= 600 && guests_in_park >= objective_guests)
				scenario_success();
			else
				scenario_failure();
		}
		break;

	case OBJECTIVE_PARK_VALUE_BY://2

		if (cur_month_year == 8 * objective_year) {
			if (park_value >= objective_currency)
				scenario_success();
			else
				scenario_failure();
		}
		break;

	case OBJECTIVE_10_ROLLERCOASTERS://5

		scenario_objective5_check();
		break;

	case OBJECTIVE_GUESTS_AND_RATING://6

		if (park_rating >= 700 && guests_in_park >= objective_guests)
			scenario_success();
		break;

	case OBJECTIVE_MONTHLY_RIDE_INCOME://7
	{
		sint32 monthly_ride_income = RCT2_GLOBAL(RCT2_ADDRESS_MONTHLY_RIDE_INCOME, sint32);
		if (monthly_ride_income >= objective_currency)
			scenario_success();
		break;
	}
	case OBJECTIVE_10_ROLLERCOASTERS_LENGTH://8

		scenario_objective8_check();
		break;

	case OBJECTIVE_FINISH_5_ROLLERCOASTERS://9
	{
		rct_ride* ride;
		int rcs = 0;
		for (int i = 0; i < MAX_RIDES; i++) {
			ride = &g_ride_list[i];
			if (ride->status != RIDE_STATUS_CLOSED && ride->excitement >= objective_currency)
				rcs++;
		}
		if (rcs >= 5)
			scenario_success();
		break;
	}
	case OBJECTIVE_REPLAY_LOAN_AND_PARK_VALUE://A
	{
		sint32 current_loan = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_LOAN, sint32);
		if (current_loan <= 0 && park_value >= objective_currency)
			scenario_success();
		break;
	}
	case OBJECTIVE_MONTHLY_FOOD_INCOME://B
	{
		sint32 income_sum = RCT2_GLOBAL(0x013578A4, sint32) + RCT2_GLOBAL(0x013578A0, sint32) +
						   RCT2_GLOBAL(0x0135789C, sint32) + RCT2_GLOBAL(0x01357898, sint32);
		if (income_sum >= objective_currency)
			scenario_success();
		break;
	}
	default:
		return;
	}
}


/*
 * Send a warning when entrance price is too high.
 * rct2: 0x0066A80E
 **/
void scenario_entrance_fee_too_high_check()
{
	uint16 x, y;
	uint16 totalRideValue = RCT2_GLOBAL(RCT2_TOTAL_RIDE_VALUE, uint16);
	uint16 park_entrance_fee = RCT2_GLOBAL(RCT2_ADDRESS_PARK_ENTRANCE_FEE, uint16);
	int max_fee = totalRideValue + (totalRideValue / 2);
	uint32 game_flags = RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32), packed_xy;

	if ((game_flags & PARK_FLAGS_PARK_OPEN) && park_entrance_fee > max_fee) {
		for (int i = 0; RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_X, uint16)[i] != SPRITE_LOCATION_NULL; ++i) {
			x = RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_X, uint16)[i] + 16;
			y = RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_Y, uint16)[i] + 16;
		}

		packed_xy = (y << 16) | x;
		news_item_add_to_queue(NEWS_ITEM_BLANK, STR_ENTRANCE_FEE_TOO_HI, packed_xy);
	}
}

/*
 * Scenario and finance related update iteration.
 * rct2: 0x006C44B1
 **/
void scenario_update()
{
	uint8 screen_flags = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8);
	uint32 month_tick = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_TICKS, uint16),
		next_month_tick = month_tick + 4;
	uint8 month = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, sint16) & 7,
		current_days_in_month = (uint8)days_in_month[month],
		objective_type = RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_TYPE, uint8);

	if (screen_flags & (~SCREEN_FLAGS_PLAYING)) // only in normal play mode
		return;

	if ((current_days_in_month * next_month_tick) >> 16 != (current_days_in_month * month_tick) >> 16) {
		// daily checks
		finance_update_daily_profit(); // daily profit update
		RCT2_CALLPROC_EBPSAFE(0x0069C35E); // some kind of peeps days_visited update loop
		get_local_time();
		RCT2_CALLPROC_EBPSAFE(0x0066A13C); // objective 6 dragging 
		if (objective_type == 10 || objective_type == 9 || objective_type == 8 ||
			objective_type == 6 || objective_type == 5) {
			scenario_objectives_check();
		}

		window_invalidate_by_id(WC_BOTTOM_TOOLBAR, 0);
	}

	
	//if ( (unsigned int)((4 * current_day) & 0xFFFF) >= 0xFFEFu) {
	if ( next_month_tick % 0x4000 == 0) {
		// weekly checks
		finance_pay_wages();
		finance_pay_research();
		finance_pay_interest();
		marketing_update();
		peep_problem_warnings_update();
		ride_check_all_reachable();
		ride_update_favourited_stat();

		if (month <= 1 && RCT2_GLOBAL(0x009ADAE0, sint32) != -1 && RCT2_GLOBAL(0x009ADAE0 + 14, uint16) & 1) {
			for (int i = 0; i < 100; ++i) {
				int carry;
				RCT2_CALLPROC_EBPSAFE(0x006744A9); // clears carry flag on failure -.-
				#ifdef _MSC_VER
				__asm mov carry, 0;
				__asm adc carry, 0;
				#else
				__asm__ ( "mov %[carry], 0; " : [carry] "+m" (carry) );
				__asm__ ( "adc %[carry], 0; " : [carry] "+m" (carry) );
				#endif

				if (!carry)
					break;
			}
		}
		park_update_histories();
		park_calculate_size();
	}

	//if ( (unsigned int)((2 * current_day) & 0xFFFF) >= 0xFFF8) {
	if (next_month_tick % 0x8000 == 0) {
		// fortnightly 
		finance_pay_ride_upkeep();
	}

	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_TICKS, uint16) = (uint16)next_month_tick;
	if (next_month_tick >= 0x10000) {
		// month ends actions
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, sint16)++;
		RCT2_GLOBAL(0x009A9804, uint32) |= 2;
		RCT2_CALLPROC_EBPSAFE(0x0069DEAD);
		scenario_objectives_check();
		scenario_entrance_fee_too_high_check();
		award_update_all();
	}
	
}

/**
*
*  rct2: 0x006E37D2
*/
int scenario_rand()
{
	int eax = RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_SRAND_0, uint32);
	RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_SRAND_0, uint32) += ror32(RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_SRAND_1, uint32) ^ 0x1234567F, 7);
	return RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_SRAND_1, uint32) = ror32(eax, 3);
}
