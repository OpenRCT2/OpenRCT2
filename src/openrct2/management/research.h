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

#ifndef _RESEARCH_H_
#define _RESEARCH_H_

#include "../common.h"

#pragma pack(push, 1)
typedef struct rct_research_item {
    // Bit 16 (0: scenery entry, 1: ride entry)
    sint32 entryIndex;
    uint8 category;
} rct_research_item;
assert_struct_size(rct_research_item, 5);
#pragma pack(pop)

enum{
    RESEARCH_ENTRY_FLAG_SCENERY_SET_ALWAYS_RESEARCHED = (1 << 29),
    RESEARCH_ENTRY_FLAG_RIDE_ALWAYS_RESEARCHED = (1 << 30),
};

// Everything before this point has been researched
#define RESEARCHED_ITEMS_SEPARATOR -1
// Everything before this point and after separator still requires research
#define RESEARCHED_ITEMS_END -2
// Extra end of list entry. Unsure why?
#define RESEARCHED_ITEMS_END_2 -3

#define MAX_RESEARCH_ITEMS 500
#define MAX_RESEARCHED_SCENERY_ITEMS 56
#define MAX_RESEARCHED_RIDE_TYPES 8
#define MAX_RESEARCHED_RIDE_ENTRIES 8
#define MAX_RESEARCHED_TRACK_TYPES 128

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

extern uint8 gResearchFundingLevel;
extern uint8 gResearchPriorities;
extern uint16 gResearchProgress;
extern uint8 gResearchProgressStage;
extern uint32 gResearchLastItemSubject;
extern uint8 gResearchExpectedMonth;
extern uint8 gResearchExpectedDay;
extern uint8 gResearchNextCategory;
extern uint32 gResearchNextItem;

extern rct_research_item gResearchItems[MAX_RESEARCH_ITEMS];
extern uint8 gResearchUncompletedCategories;
extern uint32 gResearchedRideTypes[MAX_RESEARCHED_RIDE_TYPES];
extern uint32 gResearchedRideEntries[MAX_RESEARCHED_RIDE_ENTRIES];
extern uint32 gResearchedTrackTypesA[MAX_RESEARCHED_TRACK_TYPES];
extern uint32 gResearchedTrackTypesB[MAX_RESEARCHED_TRACK_TYPES];
extern uint32 gResearchedSceneryItems[MAX_RESEARCHED_SCENERY_ITEMS];
extern bool gSilentResearch;

void research_reset_items();
void research_update_uncompleted_types();
void research_update();
void research_reset_current_item();
void research_remove_non_separate_vehicle_types();
void research_populate_list_random();
void research_populate_list_researched();

void research_set_funding(sint32 amount);
void research_set_priority(sint32 activeCategories);
void game_command_set_research_funding(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void research_finish_item(sint32 entryIndex);
void research_insert(sint32 researched, sint32 entryIndex, sint32 category);
void research_remove(sint32 entryIndex);

void research_insert_ride_entry(uint8 entryIndex, bool researched);
void research_insert_scenery_group_entry(uint8 entryIndex, bool researched);

bool ride_type_is_invented(sint32 rideType);
bool ride_entry_is_invented(sint32 rideEntryIndex);
bool track_piece_is_available_for_ride_type(uint8 rideType, sint32 trackType);
bool scenery_is_invented(uint16 sceneryItem);
void reset_researched_scenery_items();
void reset_researched_ride_types_and_entries();

#endif
