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

#include "../config.h"
#include "../game.h"
#include "../interface/window.h"
#include "../localisation/date.h"
#include "../localisation/localisation.h"
#include "../localisation/string_ids.h"
#include "../management/finance.h"
#include "../scenario.h"
#include "../rct1.h"
#include "../ride/ride.h"
#include "../ride/ride_data.h"
#include "../ride/track_data.h"
#include "../world/scenery.h"
#include "news_item.h"
#include "research.h"

const int _researchRate[] = { 0, 160, 250, 400 };

uint8 gResearchFundingLevel;
uint8 gResearchPriorities;
uint16 gResearchProgress;
uint8 gResearchProgressStage;
uint32 gResearchLastItemSubject;
uint8 gResearchExpectedMonth;
uint8 gResearchExpectedDay;
uint8 gResearchNextCategory;
uint32 gResearchNextItem;

// 0x01358844[500]
rct_research_item gResearchItems[500];

// 0x00EE787C
uint8 gResearchUncompletedCategories;

uint32 gResearchedRideTypes[8];
uint32 gResearchedRideEntries[8];
uint32 gResearchedTrackTypesA[128];
uint32 gResearchedTrackTypesB[128];
uint32 gResearchedSceneryItems[56];

bool gSilentResearch = false;

static void ride_type_set_invented(int rideType);
static void ride_entry_set_invented(int rideEntryIndex);

/**
 *
 *  rct2: 0x006671AD, part of 0x00667132
 */
void research_reset_items()
{
	gResearchItems[0].entryIndex = RESEARCHED_ITEMS_SEPARATOR;
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
	while (researchItem++->entryIndex != RESEARCHED_ITEMS_SEPARATOR);

	for (; researchItem->entryIndex != RESEARCHED_ITEMS_END; researchItem++)
		uncompletedResearchTypes |= (1 << researchItem->category);

	gResearchUncompletedCategories = uncompletedResearchTypes;
}

/**
 *
 *  rct2: 0x00684D2A
 */
static void research_calculate_expected_date()
{
	int progress = gResearchProgress;
	int progressStage = gResearchProgressStage;
	int researchLevel = gResearchFundingLevel;
	int expectedDay, expectedMonth, dayQuotient, dayRemainder, progressRemaining, daysRemaining;

	if (progressStage == RESEARCH_STAGE_INITIAL_RESEARCH || researchLevel == RESEARCH_FUNDING_NONE) {
		gResearchExpectedDay = 255;
	} else {
		progressRemaining = progressStage == RESEARCH_STAGE_COMPLETING_DESIGN ? 0x10000 : 0x20000;
		progressRemaining -= progress;
		daysRemaining = (progressRemaining / _researchRate[researchLevel]) * 128;

		expectedDay = gDateMonthTicks + (daysRemaining & 0xFFFF);
		dayQuotient = expectedDay / 0x10000;
		dayRemainder = expectedDay % 0x10000;

		expectedMonth = date_get_month(gDateMonthsElapsed + dayQuotient + (daysRemaining >> 16));
		expectedDay = (dayRemainder * days_in_month[expectedMonth]) >> 16;

		gResearchExpectedDay = expectedDay;
		gResearchExpectedMonth = expectedMonth;
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

	// Skip already researched items
	firstUnresearchedItem = gResearchItems;
	while (firstUnresearchedItem->entryIndex != RESEARCHED_ITEMS_SEPARATOR)
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
				gResearchProgress = 0;
				gResearchProgressStage = RESEARCH_STAGE_FINISHED_ALL;
				research_invalidate_related_windows();
				// Reset funding to 0 if no more rides.
				research_set_funding(0);
				return;
			}
		} else if (ignoreActiveResearchTypes || (gResearchPriorities & (1 << researchItem->category))) {
			break;
		}
	}

	gResearchNextItem = researchItem->entryIndex;
	gResearchNextCategory = researchItem->category;
	gResearchProgress = 0;
	gResearchProgressStage = RESEARCH_STAGE_DESIGNING;

	// Bubble research item up until it is above the researched items separator
	do {
		tmp = *researchItem;
		*researchItem = *(researchItem - 1);
		*(researchItem - 1) = tmp;
		researchItem--;
	} while ((researchItem + 1)->entryIndex != RESEARCHED_ITEMS_SEPARATOR);

	research_invalidate_related_windows();
}

/**
 *
 *  rct2: 0x006848D4
 */
void research_finish_item(sint32 entryIndex)
{
	int i, ebx, base_ride_type, rideEntryIndex, subSceneryEntryIndex;
	rct_ride_entry *rideEntry, *rideEntry2;
	rct_scenery_set_entry *scenerySetEntry;

	gResearchLastItemSubject = (uint32)entryIndex;
	research_invalidate_related_windows();
	if (entryIndex >= 0x10000) {
		// Ride
		base_ride_type = (entryIndex >> 8) & 0xFF;
		rideEntryIndex = entryIndex & 0xFF;
		rideEntry = get_ride_entry(rideEntryIndex);
		ride_type_set_invented(base_ride_type);
		gResearchedTrackTypesA[base_ride_type] = (RideTypePossibleTrackConfigurations[base_ride_type]         ) & 0xFFFFFFFFULL;
		gResearchedTrackTypesB[base_ride_type] = (RideTypePossibleTrackConfigurations[base_ride_type] >> 32ULL) & 0xFFFFFFFFULL;
		if (RideData4[base_ride_type].flags & RIDE_TYPE_FLAG4_3) {
			ebx = RideData4[base_ride_type].alternate_type;
			gResearchedTrackTypesA[ebx] = (RideTypePossibleTrackConfigurations[ebx]         ) & 0xFFFFFFFFULL;
			gResearchedTrackTypesB[ebx] = (RideTypePossibleTrackConfigurations[ebx] >> 32ULL) & 0xFFFFFFFFULL;
		}
		ride_entry_set_invented(rideEntryIndex);
		if (!(rideEntry->flags & RIDE_ENTRY_FLAG_SEPARATE_RIDE)) {
			for (i = 0; i < 128; i++) {
				rideEntry2 = get_ride_entry(i);
				if (rideEntry2 == (rct_ride_entry*)-1)
					continue;
				if ((rideEntry2->flags & RIDE_ENTRY_FLAG_SEPARATE_RIDE))
					continue;

				if (rideEntry2->ride_type[0] == base_ride_type ||
					rideEntry2->ride_type[1] == base_ride_type ||
					rideEntry2->ride_type[2] == base_ride_type
				) {
					ride_entry_set_invented(i);
				}
			}
		}

		set_format_arg(0, rct_string_id, ((rideEntry->flags & RIDE_ENTRY_FLAG_SEPARATE_RIDE_NAME)) ?
			rideEntry->name : RideNaming[base_ride_type].name);
		if (!gSilentResearch) {
			if (gConfigNotifications.ride_researched) {
				news_item_add_to_queue(NEWS_ITEM_RESEARCH, STR_NEWS_ITEM_RESEARCH_NEW_RIDE_AVAILABLE, entryIndex);
			}
		}

		research_invalidate_related_windows();
	} else {
		// Scenery
		scenerySetEntry = get_scenery_group_entry(entryIndex & 0xFFFF);
		for (i = 0; i < scenerySetEntry->entry_count; i++) {
			subSceneryEntryIndex = scenerySetEntry->scenery_entries[i];
			gResearchedSceneryItems[subSceneryEntryIndex >> 5] |= 1UL << (subSceneryEntryIndex & 0x1F);
		}

		set_format_arg(0, rct_string_id, scenerySetEntry->name);
		if (!gSilentResearch) {
			if (gConfigNotifications.ride_researched) {
				news_item_add_to_queue(NEWS_ITEM_RESEARCH, STR_NEWS_ITEM_RESEARCH_NEW_SCENERY_SET_AVAILABLE, entryIndex);
			}
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
	if (gScreenFlags & editorScreenFlags)
		return;

	if (gScenarioTicks % 32 != 0)
		return;

	researchLevel = gResearchFundingLevel;

	currentResearchProgress = gResearchProgress;
	currentResearchProgress += _researchRate[researchLevel];
	if (currentResearchProgress <= 0xFFFF) {
		gResearchProgress = currentResearchProgress;
	} else {
		switch (gResearchProgressStage) {
		case RESEARCH_STAGE_INITIAL_RESEARCH:
			research_next_design();
			research_calculate_expected_date();
			break;
		case RESEARCH_STAGE_DESIGNING:
			gResearchProgress = 0;
			gResearchProgressStage = RESEARCH_STAGE_COMPLETING_DESIGN;
			research_calculate_expected_date();
			research_invalidate_related_windows();
			break;
		case RESEARCH_STAGE_COMPLETING_DESIGN:
			research_finish_item((sint32)gResearchNextItem);
			gResearchProgress = 0;
			gResearchProgressStage = 0;
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

		rct_research_item* edx = NULL;
		rct_research_item* ebp = NULL;
		rct_research_item* inner_research = gResearchItems;
		do {
			if (research->entryIndex == inner_research->entryIndex){
				edx = inner_research;
			}
			if ((research + 1)->entryIndex == inner_research->entryIndex){
				ebp = inner_research;
			}
		} while ((inner_research++)->entryIndex != RESEARCHED_ITEMS_END);
		assert(edx != NULL);
		edx->entryIndex = research->entryIndex;
		assert(ebp != NULL);
		ebp->entryIndex = (research + 1)->entryIndex;

		uint8 cat = edx->category;
		edx->category = ebp->category;
		ebp->category = cat;
	}

	for (int i = 0; i < 8; ++i){
		gResearchedRideTypes[i] = 0;
		gResearchedRideEntries[i] = 0;
	}

	for (int i = 0; i < 56; i++) {
		gResearchedSceneryItems[i] = 0xFFFFFFFF;
	}

	for (int i = 0; i < 19; ++i){
		rct_scenery_set_entry* scenery_set = get_scenery_group_entry(i);
		if ((intptr_t)scenery_set == -1)continue;

		for (int j = 0; j < scenery_set->entry_count; ++j){
			uint8 value = scenery_set->scenery_entries[j] & 0x1F;
			gResearchedSceneryItems[scenery_set->scenery_entries[j] >> 5] &= ~(1UL << value);
		}
	}


	for (research = gResearchItems; research->entryIndex != RESEARCHED_ITEMS_SEPARATOR; research++){
		research_finish_item(research->entryIndex);
	}

	gResearchLastItemSubject = (uint32)-1;
	gResearchProgressStage = 0;
	gResearchProgress = 0;
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
			researchItem->entryIndex != RESEARCHED_ITEMS_SEPARATOR &&
			researchItem->entryIndex != RESEARCHED_ITEMS_END &&
			researchItem->entryIndex >= 0x10000
		) {
			rct_ride_entry *rideEntry = get_ride_entry(researchItem->entryIndex & 0xFF);
			if (!(rideEntry->flags & (RIDE_ENTRY_FLAG_SEPARATE_RIDE | RIDE_ENTRY_FLAG_SEPARATE_RIDE_NAME))) {
				// Check if ride type already exists further up for a vehicle type that isn't displayed as a ride
				researchItem2 = researchItem - 1;
				do {
					if (
						researchItem2->entryIndex != RESEARCHED_ITEMS_SEPARATOR &&
						researchItem2->entryIndex >= 0x10000
					) {
						rideEntry = get_ride_entry(researchItem2->entryIndex & 0xFF);
						if (!(rideEntry->flags & (RIDE_ENTRY_FLAG_SEPARATE_RIDE | RIDE_ENTRY_FLAG_SEPARATE_RIDE_NAME))) {

							if (((researchItem->entryIndex >> 8) & 0xFF) == ((researchItem2->entryIndex >> 8) & 0xFF)) {
								// Remove item
								researchItem2 = researchItem;
								do {
									*researchItem2 = *(researchItem2 + 1);
								} while ((researchItem2++)->entryIndex != RESEARCHED_ITEMS_END_2);
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
	// First check to make sure that entry is not already accounted for
	for (; researchItem->entryIndex != RESEARCHED_ITEMS_END; researchItem++) {
		if (researchItem->entryIndex == entryIndex)
			return;
	}
	researchItem = gResearchItems;
	do {
		if (researchItem->entryIndex == RESEARCHED_ITEMS_SEPARATOR) {
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
 *  rct2: 0x006857CF
 */
void research_remove(sint32 entryIndex){
	for (rct_research_item *researchItem = gResearchItems; researchItem->entryIndex != RESEARCHED_ITEMS_END; researchItem++){
		if (researchItem->entryIndex == entryIndex){
			do{
				*researchItem = *(researchItem + 1);
			} while (researchItem++->entryIndex != RESEARCHED_ITEMS_END_2);
			return;
		}
	}
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
	rct_ride_entry *rideEntry;
	rct_scenery_set_entry *scenerySetEntry;
	int rideType, researched;

	// Rides
	for (int i = 0; i < 128; i++) {
		rideEntry = get_ride_entry(i);
		if (rideEntry == (rct_ride_entry*)-1)
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
		scenerySetEntry = get_scenery_group_entry(i);
		if (scenerySetEntry == (rct_scenery_set_entry*)-1)
			continue;

		researched = (scenario_rand() & 0xFF) > 85;
		research_insert(researched, i, RESEARCH_CATEGORY_SCENERYSET);
	}
}

void research_populate_list_researched()
{
	rct_ride_entry *rideEntry;
	rct_scenery_set_entry *scenerySetEntry;
	int rideType;

	// Rides
	for (int i = 0; i < 128; i++) {
		rideEntry = get_ride_entry(i);
		if (rideEntry == (rct_ride_entry*)-1)
			continue;

		for (int j = 0; j < 3; j++) {
			rideType = rideEntry->ride_type[j];
			if (rideType != 255)
				research_insert(true, 0x10000 | (rideType << 8) | i, rideEntry->category[0]);
		}
	}

	// Scenery
	for (int i = 0; i < 19; i++) {
		scenerySetEntry = get_scenery_group_entry(i);
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

	gCommandExpenditureType = RCT_EXPENDITURE_TYPE_RESEARCH;
	if (*ebx & GAME_COMMAND_FLAG_APPLY) {
		if (!setPriorities) {
			if (fundingAmount < 0 || fundingAmount >= countof(_researchRate)) {
				*ebx = MONEY32_UNDEFINED;
				log_warning("Invalid research rate %d", fundingAmount);
				return;
			}
			gResearchFundingLevel = fundingAmount;
		}
		else
			gResearchPriorities = activeCategories;

		window_invalidate_by_class(WC_FINANCES);
		window_invalidate_by_class(WC_RESEARCH);
	}

	*ebx = 0;
}

void research_insert_ride_entry(uint8 entryIndex, bool researched)
{
	rct_ride_entry *rideEntry = get_ride_entry(entryIndex);
	uint8 category = rideEntry->category[0];
	for (int i = 0; i < 3; i++) {
		uint8 rideType = rideEntry->ride_type[i];
		if (rideType != 255) {
			research_insert(researched, 0x10000 | (rideType << 8) | entryIndex, category);
		}
	}
}

void research_insert_scenery_group_entry(uint8 entryIndex, bool researched)
{
	research_insert(researched, entryIndex, RESEARCH_CATEGORY_SCENERYSET);
}

bool ride_type_is_invented(int rideType)
{
	int quadIndex = rideType >> 5;
	int bitIndex = rideType & 0x1F;
	bool invented = (gResearchedRideTypes[quadIndex] & ((uint32)1 << bitIndex));
	return invented;
}

bool ride_entry_is_invented(int rideEntryIndex)
{
	int quadIndex = rideEntryIndex >> 5;
	int bitIndex = rideEntryIndex & 0x1F;
	bool invented = (gResearchedRideEntries[quadIndex] & ((uint32)1 << bitIndex));
	return invented;
}

bool track_type_is_invented(uint8 rideType, int trackType)
{
	if (trackType < 32) {
		bool invented = (gResearchedTrackTypesA[rideType] & (1UL << trackType));
		return invented;
	} else {
		trackType -= 32;
		bool invented = (gResearchedTrackTypesB[rideType] & (1UL << trackType));
		return invented;
	}
}

static void ride_type_set_invented(int rideType)
{
	int quadIndex = rideType >> 5;
	int bitIndex = rideType & 0x1F;
	gResearchedRideTypes[quadIndex] |= 1 << bitIndex;
}

static void ride_entry_set_invented(int rideEntryIndex)
{
	int quadIndex = rideEntryIndex >> 5;
	int bitIndex = rideEntryIndex & 0x1F;
	gResearchedRideEntries[quadIndex] |= 1 << bitIndex;
}

bool scenery_is_invented(uint16 sceneryItem)
{
	int quadIndex = sceneryItem >> 5;
	int bitIndex = sceneryItem & 0x1F;
	bool invented = (gResearchedSceneryItems[quadIndex] & ((uint32)1 << bitIndex));
	return invented;
}
