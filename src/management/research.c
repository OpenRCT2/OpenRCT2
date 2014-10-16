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
#include "../interface/window.h"
#include "../localisation/date.h"
#include "../world/scenery.h"
#include "news_item.h"
#include "research.h"

const int _researchRate[] = { 0, 160, 250, 400 };

// 0x01358844[500]
extern rct_research_item *gResearchItems = (rct_research_item*)RCT2_RESEARCH_ITEMS;

// 0x00EE787C
uint8 gResearchUncompletedCategories;

/**
 *
 *  rct2: 0x006671AD, part of 0x00667132
 */
void research_reset_items()
{
	gResearchItems[0].entryIndex = RESEARCHED_ITEMS_SEPERATOR;
	gResearchItems[1].entryIndex = RESEARCHED_ITEMS_END;
	gResearchItems[2].entryIndex = -3;
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
				RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS_STAGE, uint8) = RESEARCH_STAGE_INITIAL_RESEARCH;
				research_invalidate_related_windows();
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

				if (rideEntry2->var_00C == ecx || rideEntry2->var_00D == ecx || rideEntry2->var_00E == ecx)
					RCT2_ADDRESS(0x001357424, uint32)[i >> 5] |= 1 << (i & 0x1F);
			}
		}

		// I don't think 0x009AC06C is ever not 0, so probably redundant
		if (RCT2_GLOBAL(0x009AC06C, uint8) == 0) {
			RCT2_GLOBAL(0x013CE952, rct_string_id) = rideEntry->var_008 & 0x1000 ?
				rideEntry->name : ecx + 2;
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