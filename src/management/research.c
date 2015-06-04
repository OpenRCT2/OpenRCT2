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

#include "../addresses.h"
#include "../game.h"
#include "../interface/window.h"
#include "../localisation/date.h"
#include "../management/finance.h"
#include "../scenario.h"
#include "../world/scenery.h"
#include "news_item.h"
#include "research.h"

const int _researchRate[] = { 0, 160, 250, 400 };

// 0x01358844[500]
rct_research_item *gResearchItems = (rct_research_item*)RCT2_RESEARCH_ITEMS;

// 0x00EE787C
uint8 gResearchUncompletedCategories;

bool gSilentResearch = false;

/**
 *
 *  rct2: 0x006671AD, part of 0x00667132
 */
void research_reset_items()
{
	gResearchItems[0].entryIndex = RESEARCHED_ITEMS_SEPERATOR;
	gResearchItems[1].entryIndex = RESEARCHED_ITEMS_END;
	gResearchItems[2].entryIndex = RESEARCHED_ITEMS_END_2;
}

/**
 *
 *  rct2: 0x00684BAE
 */
void research_update_uncompleted_types()
{
	int uncompletedResearchTypes = 0;
	rct_research_item *researchItem = gResearchItems;
	while (researchItem->entryIndex != -1)
		researchItem++;
	researchItem++;
	for (; researchItem->entryIndex != -2; researchItem++)
		uncompletedResearchTypes |= (1 << researchItem->category);

	gResearchUncompletedCategories = uncompletedResearchTypes;
}

/**
 *
 *  rct2: 0x00684D2A
 */
static void research_calculate_expected_date()
{
	int progress = RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS, uint16);
	int progressStage = RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS_STAGE, uint8);
	int researchLevel = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_RESEARCH_LEVEL, uint8);
	int currentDay = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_TICKS, uint16);
	int currentMonth = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, uint16);
	int expectedDay, expectedMonth, dayQuotient, dayRemainder, progressRemaining, daysRemaining;

	if (progressStage == RESEARCH_STAGE_INITIAL_RESEARCH || researchLevel == RESEARCH_FUNDING_NONE) {
		RCT2_GLOBAL(RCT2_ADDRESS_NEXT_RESEARCH_EXPECTED_DAY, uint8) = 255;
	} else {
		progressRemaining = progressStage == RESEARCH_STAGE_COMPLETING_DESIGN ? 0x10000 : 0x20000;
		progressRemaining -= progress;
		daysRemaining = (progressRemaining / _researchRate[researchLevel]) * 128;

		expectedDay = currentDay + (daysRemaining & 0xFFFF);
		dayQuotient = expectedDay / 0x10000;
		dayRemainder = expectedDay % 0x10000;
		
		expectedMonth = date_get_month(currentMonth + dayQuotient + (daysRemaining >> 16));
		expectedDay = (dayRemainder * days_in_month[expectedMonth]) >> 16;

		RCT2_GLOBAL(RCT2_ADDRESS_NEXT_RESEARCH_EXPECTED_DAY, uint8) = expectedDay;
		RCT2_GLOBAL(RCT2_ADDRESS_NEXT_RESEARCH_EXPECTED_MONTH, uint8) = expectedMonth;
	}
}

static void research_invalidate_related_windows()
{
	window_invalidate_by_class(WC_CONSTRUCT_RIDE);
	window_invalidate_by_class(WC_RESEARCH);
}

/**
 *
 *  rct2: 0x00684BE5
 */
static void research_next_design()
{
	rct_research_item *firstUnresearchedItem, *researchItem, tmp;
	int ignoreActiveResearchTypes;
	int activeResearchTypes = RCT2_GLOBAL(RCT2_ADDRESS_ACTIVE_RESEARCH_TYPES, uint16);

	// Skip already researched items
	firstUnresearchedItem = gResearchItems;
	while (firstUnresearchedItem->entryIndex != RESEARCHED_ITEMS_SEPERATOR)
		firstUnresearchedItem++;

	ignoreActiveResearchTypes = 0;
	researchItem = firstUnresearchedItem;
	for (;;) {
		researchItem++;
		if (researchItem->entryIndex == RESEARCHED_ITEMS_END) {
			if (!ignoreActiveResearchTypes) {
				ignoreActiveResearchTypes = 1;
				researchItem = firstUnresearchedItem;
				continue;
			} else {
				RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS, uint16) = 0;
				RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS_STAGE, uint8) = RESEARCH_STAGE_FINISHED_ALL;
				research_invalidate_related_windows();
				// Reset funding to 0 if no more rides.
				research_set_funding(0);
				return;
			}
		} else if (ignoreActiveResearchTypes || (activeResearchTypes & (1 << researchItem->category))) {
			break;
		}
	}

	RCT2_GLOBAL(RCT2_ADDRESS_NEXT_RESEARCH_ITEM, uint32) = researchItem->entryIndex;
	RCT2_GLOBAL(RCT2_ADDRESS_NEXT_RESEARCH_CATEGORY, uint8) = researchItem->category;
	RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS, uint16) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS_STAGE, uint8) = RESEARCH_STAGE_DESIGNING;

	// Bubble research item up until it is above the researched items seperator
	do {
		tmp = *researchItem;
		*researchItem = *(researchItem - 1);
		*(researchItem - 1) = tmp;
		researchItem--;
	} while ((researchItem + 1)->entryIndex != RESEARCHED_ITEMS_SEPERATOR);

	research_invalidate_related_windows();
}

/**
 *
 *  rct2: 0x006848D4
 */
void research_finish_item(sint32 entryIndex)
{
	int i, ebx, ecx, rideEntryIndex, subSceneryEntryIndex;
	rct_ride_type *rideEntry, *rideEntry2;
	rct_scenery_set_entry *scenerySetEntry;

	RCT2_GLOBAL(RCT2_ADDRESS_LAST_RESEARCHED_ITEM_SUBJECT, sint32) = entryIndex;
	research_invalidate_related_windows();
	if (entryIndex >= 0x10000) {
		// Ride
		ecx = (entryIndex >> 8) & 0xFF;
		rideEntryIndex = entryIndex & 0xFF;
		rideEntry = GET_RIDE_ENTRY(rideEntryIndex);
		RCT2_ADDRESS(0x01357404, uint32)[ecx >> 5] |= (1 << (ecx & 0x1F));
		RCT2_ADDRESS(0x01357444, uint32)[ecx] = RCT2_ADDRESS(0x0097C468, uint32)[ecx];
		RCT2_ADDRESS(0x01357644, uint32)[ecx] = RCT2_ADDRESS(0x0097C5D4, uint32)[ecx];
		if (RCT2_GLOBAL(0x0097D4F2 + (ecx * 8), uint16) & 8) {
			ebx = RCT2_GLOBAL(0x0097D4F5 + (ecx * 8), uint8);
			RCT2_ADDRESS(0x01357444, uint32)[ebx] = RCT2_ADDRESS(0x0097C468, uint32)[ebx];
			RCT2_ADDRESS(0x01357644, uint32)[ebx] = RCT2_ADDRESS(0x0097C5D4, uint32)[ebx];
		}
		RCT2_ADDRESS(0x001357424, uint32)[rideEntryIndex >> 5] |= 1 << (rideEntryIndex & 0x1F);
		if (!(rideEntry->var_008 & 0x2000)) {
			for (i = 0; i < 128; i++) {
				rideEntry2 = GET_RIDE_ENTRY(i);
				if (rideEntry2 == (rct_ride_type*)-1)
					continue;
				if (rideEntry2->var_008 & 0x2000)
					continue;

				if (rideEntry2->ride_type[0] == ecx || rideEntry2->ride_type[1] == ecx || rideEntry2->ride_type[2] == ecx)
					RCT2_ADDRESS(0x001357424, uint32)[i >> 5] |= 1 << (i & 0x1F);
			}
		}

		// I don't think 0x009AC06C is ever not 0, so probably redundant
		if (RCT2_GLOBAL(0x009AC06C, uint8) == 0) {
			RCT2_GLOBAL(0x013CE952, rct_string_id) = rideEntry->var_008 & 0x1000 ?
				rideEntry->name : ecx + 2;
			if (!gSilentResearch)
				news_item_add_to_queue(NEWS_ITEM_RESEARCH, 2249, entryIndex);
		}

		research_invalidate_related_windows();
	} else {
		// Scenery
		scenerySetEntry = g_scenerySetEntries[entryIndex & 0xFFFF];
		for (i = 0; i < scenerySetEntry->entry_count; i++) {
			subSceneryEntryIndex = scenerySetEntry->scenery_entries[i];
			RCT2_ADDRESS(0x01357BD0, sint32)[subSceneryEntryIndex >> 5] |= 1 << (subSceneryEntryIndex & 0x1F);
		}

		// I don't think 0x009AC06C is ever not 0, so probably redundant
		if (RCT2_GLOBAL(0x009AC06C, uint8) == 0) {
			RCT2_GLOBAL(0x013CE952, rct_string_id) = scenerySetEntry->name;
			if (!gSilentResearch)
				news_item_add_to_queue(NEWS_ITEM_RESEARCH, 2250, entryIndex);
		}

		research_invalidate_related_windows();
		init_scenery();
	}
}

/**
 *
 *  rct2: 0x00684C7A
 */
void research_update()
{
	int editorScreenFlags, researchLevel, currentResearchProgress;

	editorScreenFlags = SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER;
	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & editorScreenFlags)
		return;

	if (RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TICKS, uint32) % 32 != 0)
		return;

	researchLevel = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_RESEARCH_LEVEL, uint8);

	currentResearchProgress = RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS, uint16);
	currentResearchProgress += _researchRate[researchLevel];
	if (currentResearchProgress <= 0xFFFF) {
		RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS, uint16) = currentResearchProgress;
	} else {
		switch (RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS_STAGE, uint8)) {
		case RESEARCH_STAGE_INITIAL_RESEARCH:
			research_next_design();
			research_calculate_expected_date();
			break;
		case RESEARCH_STAGE_DESIGNING:
			RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS, uint16) = 0;
			RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS_STAGE, uint8) = RESEARCH_STAGE_COMPLETING_DESIGN;
			research_calculate_expected_date();
			research_invalidate_related_windows();
			break;
		case RESEARCH_STAGE_COMPLETING_DESIGN:
			research_finish_item(RCT2_GLOBAL(RCT2_ADDRESS_NEXT_RESEARCH_ITEM, sint32));
			RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS, uint16) = 0;
			RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS_STAGE, uint8) = 0;
			research_calculate_expected_date();
			research_update_uncompleted_types();
			research_invalidate_related_windows();
			break;
		}
	}
}

/**
 *
 *  rct2: 0x00684AC3
 */
void sub_684AC3(){
	rct_research_item* research = gResearchItems;
	for (; research->entryIndex != RESEARCHED_ITEMS_END; research++);

	research++;
	for (; research->entryIndex != RESEARCHED_ITEMS_END_2; research += 2){
		if (scenario_rand() & 1) continue;

		rct_research_item* edx;
		rct_research_item* ebp;
		rct_research_item* inner_research = gResearchItems;
		do{
			if (research->entryIndex == inner_research->entryIndex){
				edx = inner_research;
			}
			if ((research + 1)->entryIndex == inner_research->entryIndex){
				ebp = inner_research;
			}
		} while ((inner_research++)->entryIndex != -2);
		edx->entryIndex = research->entryIndex;
		ebp->entryIndex = (research + 1)->entryIndex;

		uint8 cat = edx->category;
		edx->category = ebp->category;		
		ebp->category = cat;
	}

	for (int i = 0; i < 8; ++i){
		RCT2_ADDRESS(0x01357404, uint32)[i] = 0;
		RCT2_ADDRESS(0x01357424, uint32)[i] = 0;
	}

	for (int i = 0; i < 56; i++){
		RCT2_ADDRESS(0x01357BD0, uint32)[i] = -1;
	}

	for (int i = 0; i < 19; ++i){
		rct_scenery_set_entry* scenery_set = g_scenerySetEntries[i];
		if ((int)scenery_set == -1)continue;

		for (int j = 0; j < scenery_set->entry_count; ++j){
			uint8 value = scenery_set->scenery_entries[j] & 0x1F;
			RCT2_ADDRESS(0x01357BD0, uint32)[scenery_set->scenery_entries[j] >> 5] &= ~(1 << value);
		}
	}

	
	for (research = gResearchItems; research->entryIndex != RESEARCHED_ITEMS_SEPERATOR; research++){
		research_finish_item(research->entryIndex);
	}

	RCT2_GLOBAL(RCT2_ADDRESS_LAST_RESEARCHED_ITEM_SUBJECT, sint32) = -1;
	RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS_STAGE, uint8) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS, uint16) = 0;
}

/**
 *
 *  rct2: 0x0068585B
 */
void research_remove_non_separate_vehicle_types()
{
	rct_research_item *researchItem, *researchItem2;

	researchItem = gResearchItems;
	while ((researchItem + 1)->entryIndex != RESEARCHED_ITEMS_END) {
		researchItem++;
	}

	do {
		loopBeginning:
		if (
			researchItem != gResearchItems &&
			researchItem->entryIndex != RESEARCHED_ITEMS_SEPERATOR &&
			researchItem->entryIndex != RESEARCHED_ITEMS_END &&
			researchItem->entryIndex >= 0x10000			
		) {
			rct_ride_type *rideEntry = GET_RIDE_ENTRY(researchItem->entryIndex & 0xFF);
			if (!(rideEntry->var_008 & 0x3000)) {
				// Check if ride type already exists further up for a vehicle type that isn't displayed as a ride
				researchItem2 = researchItem - 1;
				do {
					if (
						researchItem2->entryIndex != RESEARCHED_ITEMS_SEPERATOR &&
						researchItem2->entryIndex >= 0x10000
					) {
						rideEntry = GET_RIDE_ENTRY(researchItem2->entryIndex & 0xFF);
						if (!(rideEntry->var_008 & 0x3000)) {
							if (((researchItem->entryIndex >> 8) & 0xFF) == ((researchItem2->entryIndex >> 8) & 0xFF)) {
								// Remove item
								researchItem2 = researchItem;
								do {
									*researchItem2 = *(researchItem2 + 1);
								} while ((researchItem2++)->entryIndex != RESEARCHED_ITEMS_END);
								goto loopBeginning;
							}
						}
					}
				} while ((researchItem2--) != gResearchItems);
			}
		}
	} while ((researchItem--) != gResearchItems);
}

/**
 *
 *  rct2: 0x006857FA
 */
static void research_insert_unresearched(int entryIndex, int category)
{
	rct_research_item *researchItem, *researchItem2;

	researchItem = gResearchItems;
	do {
		if (researchItem->entryIndex == RESEARCHED_ITEMS_END) {
			// Insert slot
			researchItem2 = researchItem;
			while (researchItem2->entryIndex != RESEARCHED_ITEMS_END_2) {
				researchItem2++;
			}
			memmove(researchItem + 1, researchItem, (researchItem2 - researchItem + 1) * sizeof(rct_research_item));

			// Place new item
			researchItem->entryIndex = entryIndex;
			researchItem->category = category;
			break;
		}
	} while (entryIndex != (researchItem++)->entryIndex);
}

/**
 *
 *  rct2: 0x00685826
 */
static void research_insert_researched(int entryIndex, int category)
{
	rct_research_item *researchItem, *researchItem2;

	researchItem = gResearchItems;
	do {
		if (researchItem->entryIndex == RESEARCHED_ITEMS_SEPERATOR) {
			// Insert slot
			researchItem2 = researchItem;
			while (researchItem2->entryIndex != RESEARCHED_ITEMS_END_2) {
				researchItem2++;
			}
			memmove(researchItem + 1, researchItem, (researchItem2 - researchItem + 1) * sizeof(rct_research_item));

			// Place new item
			researchItem->entryIndex = entryIndex;
			researchItem->category = category;
			break;
		}
	} while (entryIndex != (researchItem++)->entryIndex);
}

void research_insert(int researched, int entryIndex, int category)
{
	if (researched)
		research_insert_researched(entryIndex, category);
	else
		research_insert_unresearched(entryIndex, category);
}

/**
 *
 *  rct2: 0x00685675
 */
void research_populate_list_random()
{
	rct_ride_type *rideEntry;
	rct_scenery_set_entry *scenerySetEntry;
	int rideType, researched;

	// Rides
	for (int i = 0; i < 128; i++) {
		rideEntry = GET_RIDE_ENTRY(i);
		if (rideEntry == (rct_ride_type*)-1)
			continue;

		researched = (scenario_rand() & 0xFF) > 128;
		for (int j = 0; j < 3; j++) {
			rideType = rideEntry->ride_type[j];
			if (rideType != 255)
				research_insert(researched, 0x10000 | (rideType << 8) | i, rideEntry->category[0]);
		}
	}

	// Scenery
	for (int i = 0; i < 19; i++) {
		scenerySetEntry = g_scenerySetEntries[i];
		if (scenerySetEntry == (rct_scenery_set_entry*)-1)
			continue;

		researched = (scenario_rand() & 0xFF) > 85;
		research_insert(researched, i, RESEARCH_CATEGORY_SCENERYSET);
	}
}

void research_populate_list_researched()
{
	rct_ride_type *rideEntry;
	rct_scenery_set_entry *scenerySetEntry;
	int rideType;

	// Rides
	for (int i = 0; i < 128; i++) {
		rideEntry = GET_RIDE_ENTRY(i);
		if (rideEntry == (rct_ride_type*)-1)
			continue;

		for (int j = 0; j < 3; j++) {
			rideType = rideEntry->ride_type[j];
			if (rideType != 255)
				research_insert(true, 0x10000 | (rideType << 8) | i, rideEntry->category[0]);
		}
	}

	// Scenery
	for (int i = 0; i < 19; i++) {
		scenerySetEntry = g_scenerySetEntries[i];
		if (scenerySetEntry == (rct_scenery_set_entry*)-1)
			continue;

		research_insert(true, i, RESEARCH_CATEGORY_SCENERYSET);
	}
}

void research_set_funding(int amount)
{
	game_do_command(0, GAME_COMMAND_FLAG_APPLY, 0, amount, GAME_COMMAND_SET_RESEARCH_FUNDING, 0, 0);

}

void research_set_priority(int activeCategories)
{
	game_do_command(0, (1 << 8) | GAME_COMMAND_FLAG_APPLY, 0, activeCategories, GAME_COMMAND_SET_RESEARCH_FUNDING, 0, 0);
}

/**
 *
 *  rct2: 0x00684A7F
 */
void game_command_set_research_funding(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp)
{
	int setPriorities = (*ebx & (1 << 8)) != 0;
	int fundingAmount = *edx;
	int activeCategories = *edx;

	RCT2_GLOBAL(RCT2_ADDRESS_NEXT_EXPENDITURE_TYPE, uint8) = RCT_EXPENDITURE_TYPE_RESEARCH * 4;
	if (*ebx & GAME_COMMAND_FLAG_APPLY) {
		if (!setPriorities)
			RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_RESEARCH_LEVEL, uint8) = fundingAmount;
		else
			RCT2_GLOBAL(RCT2_ADDRESS_ACTIVE_RESEARCH_TYPES, uint8) = activeCategories;

		window_invalidate_by_class(WC_FINANCES);
		window_invalidate_by_class(WC_RESEARCH);
	}

	*ebx = 0;
}