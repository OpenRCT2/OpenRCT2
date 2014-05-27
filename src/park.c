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
#include "addresses.h"
#include "award.h"
#include "finance.h"
#include "map.h"
#include "marketing.h"
#include "news_item.h"
#include "park.h"
#include "peep.h"
#include "ride.h"
#include "scenario.h"
#include "sprite.h"
#include "string_ids.h"
#include "window.h"

/**
 * In a difficult guest generation scenario, no guests will be generated if over this value.
 */
int _suggestedGuestMaximum;

/**
 * Probability out of 65535, of gaining a new guest per game tick.
 * new guests per second = 40 * (probability / 65535)
 * With a full park rating, non-overpriced entrance fee, less guests than the suggested maximum and four positive awards,
 * approximately 1 guest per second can be generated (+60 guests in one minute).
 */
int _guestGenerationProbability;

int park_is_open()
{
	return (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_PARK_OPEN) != 0;
}

/**
 * 
 *  rct2: 0x00667132
 */
void park_init()
{
	int i;

	RCT2_GLOBAL(0x013CA740, uint8) = 0;
	RCT2_GLOBAL(0x013573D4, uint16) = 777;
	RCT2_GLOBAL(RCT2_ADDRESS_HANDYMAN_COLOUR, uint8) = 28;
	RCT2_GLOBAL(RCT2_ADDRESS_MECHANIC_COLOUR, uint8) = 28;
	RCT2_GLOBAL(RCT2_ADDRESS_SECURITY_COLOUR, uint8) = 28;
	RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_IN_PARK, uint16) = 0;
	RCT2_GLOBAL(0x01357BC8, uint16) = 0;
	RCT2_GLOBAL(0x01357846, uint16) = 0;
	RCT2_GLOBAL(0x013573FE, uint16) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PARK_RATING, uint16) = 0;
	_guestGenerationProbability = 0;
	RCT2_GLOBAL(RCT2_TOTAL_RIDE_VALUE, uint16) = 0;
	RCT2_GLOBAL(0x01357CF4, sint32) = -1;

	for (i = 0; i < 20; i++)
		RCT2_ADDRESS(0x01358102, uint8)[i] = 0;

	RCT2_GLOBAL(0x01358844, uint32) = 0xFFFFFFFF;
	RCT2_GLOBAL(0x01358849, uint32) = 0xFFFFFFFE;
	RCT2_GLOBAL(0x0135884E, uint32) = 0xFFFFFFFD;
	finance_init();

	for (i = 0; i < 2; i++)
		RCT2_ADDRESS(0x01357404, uint32)[i] = 0;

	for (i = 0; i < 56; i++)
		RCT2_ADDRESS(0x01357BD0, sint32)[i] = -1;

	RCT2_GLOBAL(RCT2_ADDRESS_PARK_ENTRANCE_FEE, money16) = MONEY(10, 00);
	RCT2_GLOBAL(0x013573F2, sint16) = -1;
	RCT2_GLOBAL(0x013573F8, sint16) = -1;
	RCT2_GLOBAL(0x01357CF2, uint16) = 127;
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_RESEARCH_LEVEL, uint8) = 2;

	RCT2_GLOBAL(0x013580F4, uint16) = 500;
	RCT2_GLOBAL(0x013580E9, uint8) = 128;
	RCT2_GLOBAL(0x013580F6, uint8) = 200;
	RCT2_GLOBAL(0x013580F7, uint8) = 200;
	RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_TYPE, uint8) = 1;
	RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_YEAR, uint8) = 4;
	RCT2_GLOBAL(RCT2_ADDRESS_OBJECTIVE_NUM_GUESTS, uint16) = 1000;
	RCT2_GLOBAL(0x01358770, uint16) = 900;
	RCT2_GLOBAL(0x01358772, uint16) = 400;
	RCT2_GLOBAL(0x01358774, uint16) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) = PARK_FLAGS_11 | PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
	park_reset_awards_and_history();

	rct_s6_info *info = (rct_s6_info*)0x0141F570;
	info->name[0] = '\0';
	format_string(info->details, STR_NO_DETAILS_YET, NULL);
}

/**
 * 
 *  rct2: 0x0066729F
 */
void park_reset_awards_and_history()
{
	int i;

	// Reset park rating and guests in park history
	for (i = 0; i < 32; i++) {
		RCT2_ADDRESS(RCT2_ADDRESS_PARK_RATING_HISTORY, uint8)[i] = 255;
		RCT2_ADDRESS(RCT2_ADDRESS_GUESTS_IN_PARK_HISTORY, uint8)[i] = 255;
	}

	// Reset finance history
	for (i = 0; i < 128; i++) {
		RCT2_ADDRESS(RCT2_ADDRESS_BALANCE_HISTORY, money32)[i] = MONEY32_UNDEFINED;
		RCT2_ADDRESS(RCT2_ADDRESS_WEEKLY_PROFIT_HISTORY, money32)[i] = MONEY32_UNDEFINED;
		RCT2_ADDRESS(RCT2_ADDRESS_PARK_VALUE_HISTORY, money32)[i] = MONEY32_UNDEFINED;
	}

	// Reset awards
	for (i = 0; i < 4; i++)
		RCT2_ADDRESS(RCT2_ADDRESS_AWARD_LIST, rct_award)[i].time = 0;
}

/**
 * 
 *  rct2: 0x0066A348
 */
int park_calculate_size()
{
	int tiles, x, y;
	rct_map_element *mapElement;

	tiles = 0;
	for (y = 0; y < 256; y++) {
		for (x = 0; x < 256; x++) {
			mapElement = RCT2_ADDRESS(RCT2_ADDRESS_TILE_MAP_ELEMENT_POINTERS, rct_map_element*)[y * 256 + x];
			while (mapElement->type & MAP_ELEMENT_TYPE_MASK) {
				mapElement++;
			}

			if (mapElement->properties.surface.ownership & 0x30)
				tiles++;
		}
	}

	if (tiles != RCT2_GLOBAL(RCT2_ADDRESS_PARK_SIZE, sint16)) {
		RCT2_GLOBAL(RCT2_ADDRESS_PARK_SIZE, sint16) = tiles;
		window_invalidate_by_id(WC_PARK_INFORMATION, 0);
	}
	
	return tiles;
}

/**
 * 
 *  rct2: 0x00669EAA
 */
int calculate_park_rating()
{
	int result;

	result = 1150;
	if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & 0x4000)
		result = 1050;
	
	// Guests
	{
		rct_peep* peep;
		uint16 sprite_idx;
		int num_happy_peeps;
		short _bp;
		
		// -150 to +3 based on a range of guests from 0 to 2000
		result -= 150 - (min(2000, RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_IN_PARK, uint16)) / 13);

		// Guests, happiness, ?
		num_happy_peeps = 0;
		_bp = 0;
		for (sprite_idx = RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_START_PEEP, uint16); sprite_idx != SPRITE_INDEX_NULL; sprite_idx = peep->next) {
			peep = &(RCT2_ADDRESS(RCT2_ADDRESS_SPRITE_LIST, rct_sprite)[sprite_idx].peep);
			if (peep->type != PEEP_TYPE_GUEST)
				continue;
			if (peep->var_2A != 0)
				continue;
			if (peep->happiness > 128)
				num_happy_peeps++;
			if (!(peep->flags & PEEP_FLAGS_LEAVING_PARK))
				continue;
			if (peep->var_C6 <= 89)
				_bp++;
		}
		
		// Peep happiness -500 to +0
		result -= 500;

		if (RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_IN_PARK, uint16) > 0)
			result += 2 * min(250, (num_happy_peeps * 300) / RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_IN_PARK, uint16));

		// ?
		_bp -= 25;
		if (_bp >= 0)
			result -= _bp * 7;
	}

	// Rides
	{
		int i;
		short _ax, total_ride_intensity = 0, total_ride_excitement = 0, average_intensity, average_excitement;
		int num_rides, num_exciting_rides = 0;
		rct_ride* ride;

		// 
		_ax = 0;
		num_rides = 0;
		for (i = 0; i < 255; i++) {
			ride = &(RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride)[i]);

			if (ride->type == RIDE_TYPE_NULL)
				continue;
			_ax += 100 - ride->var_199;

			if (ride->excitement != -1){
				total_ride_excitement += ride->excitement / 8;
				total_ride_intensity += ride->intensity / 8;
				num_exciting_rides++;
			}
			num_rides++;
		}
		result -= 200;
		if (num_rides > 0)
			result += (_ax / num_rides) * 2;

		result -= 100;

		if (num_exciting_rides>0){
			average_excitement = total_ride_excitement / num_exciting_rides;
			average_intensity = total_ride_intensity / num_exciting_rides;

			average_excitement -= 46;
			if (average_excitement < 0){
				average_excitement = -average_excitement;
			}

			average_intensity -= 65;
			if (average_intensity < 0){
				average_intensity = -average_intensity;
			}

			average_excitement = min(average_excitement / 2, 50);
			average_intensity = min(average_intensity / 2, 50);
			result += 100 - average_excitement - average_intensity;
		}

		total_ride_excitement = min(1000, total_ride_excitement);
		total_ride_intensity = min(1000, total_ride_intensity);
		result -= 200 - ((total_ride_excitement + total_ride_intensity) / 10);
	}

	// Litter
	{
		rct_litter* litter;
		uint16 sprite_idx;
		short num_litter;

		num_litter = 0;
		for (sprite_idx = RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_START_LITTER, uint16); sprite_idx != SPRITE_INDEX_NULL; sprite_idx = litter->next) {
			litter = &(RCT2_ADDRESS(RCT2_ADDRESS_SPRITE_LIST, rct_sprite)[sprite_idx].litter);

			// Guessing this eliminates recently dropped litter
			if (litter->var_24 - RCT2_GLOBAL(0x00F663AC, uint32) >= 7680)
				num_litter++;
		}
		result -= 600 - (4 * (150 - min(150, num_litter)));
	}
	
	result -= RCT2_GLOBAL(0x0135882E, sint16);
	result = clamp(0, result, 999);
	return result;
}

money32 calculate_ride_value(rct_ride *ride)
{
	if (ride->type == RIDE_TYPE_NULL)
		return 0;
	if (ride->reliability == 0xFFFF)
		return 0;

	// Reliability * (...)
	return (ride->reliability * 10) * (
		ride->var_124 + ride->var_126 + ride->var_128 + ride->var_12A +
		ride->var_12C + ride->var_12E + ride->age + ride->running_cost +
		ride->var_134 + ride->var_136 +
		*((uint8*)(0x0097D21E + (ride->type * 8))) * 4
	);
}

/**
 * 
 *  rct2: 0x0066A3F6
 */
money32 calculate_park_value()
{
	int i;
	money32 result;
	rct_ride* ride;

	// Sum ride values
	result = 0;
	for (i = 0; i < 255; i++) {
		ride = &(RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride)[i]);
		result += calculate_ride_value(ride);
	}

	// +7.00 per guest
	result += RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_IN_PARK, uint16) * MONEY(7, 00);

	return result;
}

/**
 * Calculate the company value.
 * Cash + Park Value - Loan
 *
 *  rct2: 0x0066A498
 */
money32 calculate_company_value()
{
	return
		DECRYPT_MONEY(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONEY_ENCRYPTED, sint32)) +
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PARK_VALUE, money32) -
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_LOAN, money32);
}

/**
 *
 *  rct2: 0x00667104
 */
void reset_park_entrances()
{
	RCT2_GLOBAL(0x013573D4, uint16) = 0;

	for (short i = 0; i < 4; i++) {
		RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_X, uint16)[i] = 0x8000;
	}

	RCT2_GLOBAL(0x013573F2, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x013573F8, uint16) = 0xFFFF;
}

/**
 * Calculates the probability of a new guest. Also sets total ride value and suggested guest maximum.
 * Total ride value should probably be set else where, as its not just used for guest generation.
 * Suggested guest maximum should probably be an output result, not a global.
 * @returns A probability out of 65535
 *  rct2: 0x0066730A
 */
static int park_calculate_guest_generation_probability()
{
	unsigned int probability;
	int i, suggestedMaxGuests, totalRideValue;

	// Calculate suggested guest maximum (based on ride type) and total ride value
	suggestedMaxGuests = 0;
	totalRideValue = 0;
	for (i = 0; i < MAX_RIDES; i++) {
		rct_ride *ride = &RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride)[i];
		if (ride->type == RIDE_TYPE_NULL)
			continue;
		if (ride->status != RIDE_STATUS_OPEN)
			continue;
		if (ride->lifecycle_flags & 0x80)
			continue;
		if (ride->lifecycle_flags & 0x400)
			continue;

		// Add guest score for ride type
		suggestedMaxGuests += RCT2_GLOBAL(0x0097D21E + (ride->type * 8), uint8);

		// Add ride value
		if (ride->reliability != RIDE_RELIABILITY_UNDEFINED) {
			int rideValue = ride->reliability - ride->price;
			if (rideValue > 0)
				totalRideValue += rideValue * 2;
		}
	}

	// If difficult guest generation, extra guests are available for good rides
	if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_DIFFICULT_GUEST_GENERATION) {
		suggestedMaxGuests = min(suggestedMaxGuests, 1000);
		for (i = 0; i < MAX_RIDES; i++) {
			rct_ride *ride = &RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride)[i];
			if (ride->type == RIDE_TYPE_NULL)
				continue;
			if (ride->lifecycle_flags & 0x80)
				continue;
			if (ride->lifecycle_flags & 0x400)
				continue;

			if (!(RCT2_GLOBAL(0x0097CF40 + (ride->type * 8), uint32) & 0x10000000))
				continue;
			if (!(RCT2_GLOBAL(0x0097CF40 + (ride->type * 8), uint32) & 0x200))
				continue;
			if (!(ride->lifecycle_flags & 0x02))
				continue;
			if (ride->var_0E4 < 0x2580000)
				continue;
			if (ride->excitement < RIDE_RATING(6,00))
				continue;

			// Bonus guests for good ride
			suggestedMaxGuests += RCT2_GLOBAL(0x0097D21E + (ride->type * 8), uint8) * 2;
		}
	}

	suggestedMaxGuests = min(suggestedMaxGuests, 65535);
	RCT2_GLOBAL(RCT2_TOTAL_RIDE_VALUE, uint16) = totalRideValue;
	_suggestedGuestMaximum = suggestedMaxGuests;

	// Begin with 50 + park rating
	probability = 50 + clamp(0, RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PARK_RATING, uint16) - 200, 650);

	// The more guests, the lower the chance of a new one
	int numGuests = RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_IN_PARK, uint16) + RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_HEADING_FOR_PARK, uint16);
	if (numGuests > suggestedMaxGuests) {
		probability /= 4;

		// Even lower for difficult guest generation
		if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_DIFFICULT_GUEST_GENERATION)
			probability /= 4;
	}

	// Reduces chance for any more than 7000 guests
	if (numGuests > 7000)
		probability /= 4;

	// Check if money is enabled
	if (!(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_11)) {
		// Penalty for overpriced entrance fee relative to total ride value
		money16 entranceFee = RCT2_GLOBAL(RCT2_ADDRESS_PARK_ENTRANCE_FEE, money16);
		if (entranceFee > totalRideValue) {
			probability /= 4;

			// Extra penalty for very overpriced entrance fee
			if (entranceFee / 2 > totalRideValue)
				probability /= 4;
		}
	}

	// Reward or penalties for park awards
	for (i = 0; i < MAX_AWARDS; i++) {
		rct_award *award = &RCT2_ADDRESS(RCT2_ADDRESS_AWARD_LIST, rct_award)[i];
		if (award->time == 0)
			continue;

		// +/- 0.25% of the probability
		if (award_is_positive(award->type))
			probability += probability / 4;
		else
			probability -= probability / 4;
	}

	return probability;
}

static void get_random_peep_spawn(rct2_peep_spawn *spawn)
{
	rct2_peep_spawn *peepSpawns = RCT2_ADDRESS(RCT2_ADDRESS_PEEP_SPAWNS, rct2_peep_spawn);

	*spawn = peepSpawns[0];
	if (peepSpawns[1].x != 0xFFFF)
		if (scenario_rand() & 0x80000)
			*spawn = peepSpawns[1];
}

static rct_peep *park_generate_new_guest()
{
	rct_peep *peep;
	rct2_peep_spawn spawn;
	get_random_peep_spawn(&spawn);

	if (spawn.x != 0xFFFF) {
		spawn.z *= 16;
		spawn.direction ^= 2;
		peep = peep_generate(spawn.x, spawn.y, spawn.z);
		if (peep != NULL) {
			peep->var_1E = spawn.direction << 3;
						
			// Get the centre point of the tile the peep is on
			peep->var_32 = (peep->x & 0xFFE0) + 16;
			peep->var_34 = (peep->y & 0xFFE0) + 16;

			peep->var_36 = 5;
			peep->var_76 = 0;
			peep->var_78 = spawn.direction;
			peep->var_37 = 0;
			peep->state = PEEP_STATE_ENTERING_PARK;
		}
	}

	return peep;
}

static rct_peep *park_generate_new_guest_due_to_campaign(int campaign)
{
	rct_peep *peep = park_generate_new_guest();
	if (peep != NULL)
		marketing_set_guest_campaign(peep, campaign);
	return peep;
}

static void park_generate_new_guests()
{
	// Generate a new guest for some probability
	if ((scenario_rand() & 0xFFFF) < _guestGenerationProbability) {
		int difficultGeneration = (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_DIFFICULT_GUEST_GENERATION) != 0;
		if (!difficultGeneration || _suggestedGuestMaximum + 150 >= RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_IN_PARK, uint16))
			park_generate_new_guest();
	}

	// Extra guests generated by advertising campaigns
	int campaign;
	for (campaign = 0; campaign < ADVERTISING_CAMPAIGN_COUNT; campaign++) {
		if (RCT2_ADDRESS(0x01358102, uint8)[campaign] != 0) {
			// Random chance of guest generation
			if ((scenario_rand() & 0xFFFF) < marketing_get_campaign_guest_generation_probability(campaign))
				park_generate_new_guest_due_to_campaign(campaign);
		}
	}
}

/**
 *
 *  rct2: 0x006674F7
 */
void park_update()
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 0x0E)
		return;

	// Every 5 seconds approximately
	if (RCT2_GLOBAL(0x013628F4, uint8) % 512 == 0) {
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PARK_RATING, uint16) = calculate_park_rating();
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PARK_VALUE, money32) = calculate_park_value();
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_COMPANY_VALUE, money32) = calculate_company_value();
		window_invalidate_by_id(WC_FINANCES, 0);
		_guestGenerationProbability = park_calculate_guest_generation_probability();
		RCT2_GLOBAL(0x009A9804, uint16) |= 0x10;
		window_invalidate_by_id(WC_PARK_INFORMATION, 0);
	}

	// Generate new guests
	park_generate_new_guests();
}

/**
 *
 *  rct2: 0x0066A231
 */
void park_update_histories()
{
	RCT2_CALLPROC_EBPSAFE(0x0066A231);
}