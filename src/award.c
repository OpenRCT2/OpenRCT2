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

#include "addresses.h"
#include "award.h"
#include "news_item.h"
#include "ride.h"
#include "scenario.h"
#include "window.h"

#define NEGATIVE 0
#define POSITIVE 1

int _awardPositiveMap[] = {
	NEGATIVE, // PARK_AWARD_MOST_UNTIDY
	POSITIVE, // PARK_AWARD_MOST_TIDY
	POSITIVE, // PARK_AWARD_BEST_ROLLERCOASTERS
	POSITIVE, // PARK_AWARD_BEST_VALUE
	POSITIVE, // PARK_AWARD_MOST_BEAUTIFUL
	NEGATIVE, // PARK_AWARD_WORST_VALUE
	POSITIVE, // PARK_AWARD_SAFEST
	POSITIVE, // PARK_AWARD_BEST_STAFF
	POSITIVE, // PARK_AWARD_BEST_FOOD
	NEGATIVE, // PARK_AWARD_WORST_FOOD
	POSITIVE, // PARK_AWARD_BEST_RESTROOMS
	NEGATIVE, // PARK_AWARD_MOST_DISAPPOINTING
	POSITIVE, // PARK_AWARD_BEST_WATER_RIDES
	POSITIVE, // PARK_AWARD_BEST_CUSTOM_DESIGNED_RIDES
	POSITIVE, // PARK_AWARD_MOST_DAZZLING_RIDE_COLOURS
	NEGATIVE, // PARK_AWARD_MOST_CONFUSING_LAYOUT
	POSITIVE, // PARK_AWARD_BEST_GENTLE_RIDES
};

int award_is_positive(int type)
{
	return _awardPositiveMap[type];
}

#pragma region Award checks

static int award_is_deserved_most_untidy(int awardType, int activeAwardTypes)
{
	// TODO
	return 0;
}

static int award_is_deserved_most_tidy(int awardType, int activeAwardTypes)
{
	// TODO
	return 0;
}

static int award_is_deserved_best_rollercoasters(int awardType, int activeAwardTypes)
{
	int i, rollerCoasters;
	rct_ride *ride;
	char *object;

	rollerCoasters = 0;
	FOR_ALL_RIDES(i, ride) {
		object = RCT2_ADDRESS(0x009ACFA4, char*)[ride->subtype];
		if (RCT2_GLOBAL(object + 0x1BE, uint8) != RIDE_GROUP_ROLLERCOASTER && RCT2_GLOBAL(object + 0x1BF, uint8) != RIDE_GROUP_ROLLERCOASTER)
			continue;

		if (ride->status != RIDE_STATUS_OPEN || ride->lifecycle_flags & 0x400)
			continue;

		rollerCoasters++;
	}

	return (rollerCoasters >= 6);
}

/** Entrance fee is 0.10 less than half of the total ride value. */
static int award_is_deserved_best_value(int awardType, int activeAwardTypes)
{
	if (activeAwardTypes & (1 << PARK_AWARD_WORST_VALUE))
		return 0;
	if (activeAwardTypes & (1 << PARK_AWARD_MOST_DISAPPOINTING))
		return 0;
	if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & (PARK_FLAGS_11 | PARK_FLAGS_PARK_FREE_ENTRY))
		return 0;
	if (RCT2_GLOBAL(RCT2_TOTAL_RIDE_VALUE, money16) < MONEY(10, 00))
		return 0;
	if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_ENTRANCE_FEE, money16) + MONEY(0, 10) >= RCT2_GLOBAL(RCT2_TOTAL_RIDE_VALUE, money16) / 2)
		return 0;
	return 1;
}

static int award_is_deserved_most_beautiful(int awardType, int activeAwardTypes)
{
	// TODO
	return 0;
}

/** Entrance fee is more than total ride value. */
static int award_is_deserved_worse_value(int awardType, int activeAwardTypes)
{
	if (activeAwardTypes & (1 << PARK_AWARD_BEST_VALUE))
		return 0;
	if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_11)
		return 0;
	if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_ENTRANCE_FEE, money16) == MONEY(0, 00))
		return 0;
	if (RCT2_GLOBAL(RCT2_TOTAL_RIDE_VALUE, money16) >= RCT2_GLOBAL(RCT2_ADDRESS_PARK_ENTRANCE_FEE, money16))
		return 0;
	return 1;
}
static int award_is_deserved_safest(int awardType, int activeAwardTypes)
{
	// TODO
	return 0;
}

static int award_is_deserved_best_staff(int awardType, int activeAwardTypes)
{
	// TODO
	return 0;
}

static int award_is_deserved_best_food(int awardType, int activeAwardTypes)
{
	// TODO
	return 0;
}

static int award_is_deserved_worst_food(int awardType, int activeAwardTypes)
{
	// TODO
	return 0;
}

static int award_is_deserved_best_restrooms(int awardType, int activeAwardTypes)
{
	// TODO
	return 0;
}

/** More than half of the rides have satisfication <= 6 and park rating <= 650. */
static int award_is_deserved_most_disappointing(int awardType, int activeAwardTypes)
{
	unsigned int i, countedRides, disappointingRides;
	rct_ride *ride;

	if (activeAwardTypes & (1 << PARK_AWARD_BEST_VALUE))
		return 0;
	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PARK_RATING, uint16) > 650)
		return 0;

	// Count the number of disappointing rides
	countedRides = 0;
	disappointingRides = 0;

	FOR_ALL_RIDES(i, ride) {
		if (ride->excitement == 0xFFFF || ride->var_158 == 0xFF)
			continue;
		
		countedRides++;

		// Satification maybe?
		if (ride->var_158 <= 6)
			disappointingRides++;
	}

	// Half of the rides are disappointing
	return (disappointingRides >= countedRides / 2);
}

/** At least 6 open water rides. */
static int award_is_deserved_best_water_rides(int awardType, int activeAwardTypes)
{
	int i, waterRides;
	rct_ride *ride;
	char *object;

	waterRides = 0;
	FOR_ALL_RIDES(i, ride) {
		object = RCT2_ADDRESS(0x009ACFA4, char*)[ride->subtype];
		if (RCT2_GLOBAL(object + 0x1BE, uint8) != RIDE_GROUP_WATER && RCT2_GLOBAL(object + 0x1BF, uint8) != RIDE_GROUP_WATER)
			continue;

		if (ride->status != RIDE_STATUS_OPEN || ride->lifecycle_flags & 0x400)
			continue;

		waterRides++;
	}

	return (waterRides >= 6);
}

static int award_is_deserved_best_custom_designed_rides(int awardType, int activeAwardTypes)
{
	// TODO
	return 0;
}

static int award_is_deserved_most_dazzling_ride_colours(int awardType, int activeAwardTypes)
{
	// TODO
	return 0;
}

static int award_is_deserved_most_confusing_layout(int awardType, int activeAwardTypes)
{
	// TODO
	return 0;
}

/** At least 10 open gentle rides. */
static int award_is_deserved_best_gentle_rides(int awardType, int activeAwardTypes)
{
	int i, gentleRides;
	rct_ride *ride;
	char *object;

	gentleRides = 0;
	FOR_ALL_RIDES(i, ride) {
		object = RCT2_ADDRESS(0x009ACFA4, char*)[ride->subtype];
		if (RCT2_GLOBAL(object + 0x1BE, uint8) != RIDE_GROUP_GENTLE && RCT2_GLOBAL(object + 0x1BF, uint8) != RIDE_GROUP_GENTLE)
			continue;

		if (ride->status != RIDE_STATUS_OPEN || ride->lifecycle_flags & 0x400)
			continue;

		gentleRides++;
	}

	return (gentleRides >= 10);
}

typedef int (*award_deserved_check)(int, int);

award_deserved_check _awardChecks[] = {
	award_is_deserved_most_untidy,
	award_is_deserved_most_tidy,
	award_is_deserved_best_rollercoasters,
	award_is_deserved_best_value,
	award_is_deserved_most_beautiful,
	award_is_deserved_worse_value,
	award_is_deserved_safest,
	award_is_deserved_best_staff,
	award_is_deserved_best_food,
	award_is_deserved_worst_food,
	award_is_deserved_best_restrooms,
	award_is_deserved_most_disappointing,
	award_is_deserved_best_water_rides,
	award_is_deserved_best_custom_designed_rides,
	award_is_deserved_most_dazzling_ride_colours,
	award_is_deserved_most_confusing_layout,
	award_is_deserved_best_gentle_rides
};

static int award_is_deserved(int awardType, int activeAwardTypes)
{
	return _awardChecks[awardType](awardType, activeAwardTypes);
}

#pragma endregion

/**
 *
 *  rct2: 0x0066A86C
 */
void award_update_all()
{
	int i, activeAwardTypes, freeAwardEntryIndex;
	rct_award *awards;

	awards = RCT2_ADDRESS(RCT2_ADDRESS_AWARD_LIST, rct_award);

	// Only add new awards if park is open
	if (RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_PARK_OPEN) {
		// Set active award types as flags
		activeAwardTypes = 0;
		freeAwardEntryIndex = -1;
		for (i = 0; i < MAX_AWARDS; i++) {
			if (awards[i].time != 0)
				activeAwardTypes |= (1 << awards[i].type);
			else if (freeAwardEntryIndex != -1)
				freeAwardEntryIndex = i;
		}

		// Check if there was a free award entry
		if (freeAwardEntryIndex != -1) {
			// Get a random award type not already active
			int awardType;
			do {
				awardType = (((scenario_rand() & 0xFF) * 17) >> 8) & 0xFF;
			} while (activeAwardTypes & (1 << awardType));

			// Check if award is deserved
			if (award_is_deserved(awardType, activeAwardTypes)) {
				// Add award
				awards[freeAwardEntryIndex].type = awardType;
				awards[freeAwardEntryIndex].time = 5;
				news_item_add_to_queue(NEWS_ITEM_AWARD, STR_NEWS_ITEM_AWARD_MOST_UNTIDY + awardType, 0);
				window_invalidate_by_id(WC_PARK_INFORMATION, 0);
			}
		}
	}

	// Decrease award times
	for (i = 0; i < MAX_AWARDS; i++)
		if (awards[i].time != 0)
			if (--awards[i].time == 0)
				window_invalidate_by_id(WC_PARK_INFORMATION, 0);
}