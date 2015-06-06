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

#ifndef _RESEARCH_H_
#define _RESEARCH_H_

#include "../common.h"

typedef struct {
	// Bit 16 (0: scenery entry, 1: ride entry)
	sint32 entryIndex;
	uint8 category;
} rct_research_item;

// Everything before this point has been researched
#define RESEARCHED_ITEMS_SEPERATOR -1
// Everything before this point and after seperator still requires research
#define RESEARCHED_ITEMS_END -2
// Extra end of list entry. Unsure why?
#define RESEARCHED_ITEMS_END_2 -3

enum {
	RESEARCH_FUNDING_NONE,
	RESEARCH_FUNDING_MINIMUM,
	RESEARCH_FUNDING_NORMAL,
	RESEARCH_FUNDING_MAXIMUM
};

enum {
	RESEARCH_STAGE_INITIAL_RESEARCH,
	RESEARCH_STAGE_DESIGNING,
	RESEARCH_STAGE_COMPLETING_DESIGN,
	RESEARCH_STAGE_UNKNOWN,
	RESEARCH_STAGE_FINISHED_ALL
};

enum {
	RESEARCH_CATEGORY_TRANSPORT,
	RESEARCH_CATEGORY_GENTLE,
	RESEARCH_CATEGORY_ROLLERCOASTER,
	RESEARCH_CATEGORY_THRILL,
	RESEARCH_CATEGORY_WATER,
	RESEARCH_CATEGORY_SHOP,
	RESEARCH_CATEGORY_SCENERYSET
};

extern rct_research_item *gResearchItems;
extern uint8 gResearchUncompletedCategories;
extern bool gSilentResearch;

void research_reset_items();
void research_update_uncompleted_types();
void research_update();
void sub_684AC3();
void research_remove_non_separate_vehicle_types();
void research_populate_list_random();
void research_populate_list_researched();

void research_set_funding(int amount);
void research_set_priority(int activeCategories);
void game_command_set_research_funding(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp);
void research_finish_item(sint32 entryIndex);
void research_insert(int researched, int entryIndex, int category);

#endif