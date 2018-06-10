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

#pragma once

#include "../common.h"
#include "../object/ObjectLimits.h"
#include "../ride/Ride.h"

struct rct_ride_entry;

#pragma pack(push, 1)
struct rct_research_item
{
    // Bit 16 (0: scenery entry, 1: ride entry)
    union
    {
        sint32 rawValue;
        struct
        {
            uint8 entryIndex;
            uint8 baseRideType;
            uint8 type; // 0: scenery entry, 1: ride entry
            uint8 flags;
        };
    };
    uint8 category;
};
assert_struct_size(rct_research_item, 5);
#pragma pack(pop)

enum
{
    RESEARCH_ENTRY_TYPE_SCENERY = 0,
    RESEARCH_ENTRY_TYPE_RIDE    = 1,
};

enum
{
    RESEARCH_ENTRY_FLAG_SCENERY_SET_ALWAYS_RESEARCHED = (1 << 5),
    RESEARCH_ENTRY_FLAG_RIDE_ALWAYS_RESEARCHED = (1 << 6),
};

// Everything before this point has been researched
#define RESEARCHED_ITEMS_SEPARATOR -1
// Everything before this point and after separator still requires research
#define RESEARCHED_ITEMS_END -2
// Extra end of list entry. Leftover from RCT1.
#define RESEARCHED_ITEMS_END_2 -3

#define MAX_RESEARCH_ITEMS            500
#define MAX_RESEARCHED_TRACK_TYPES    128
#define MAX_RESEARCHED_SCENERY_ITEMS 1792

#define RESEARCH_ENTRY_RIDE_MASK 0x10000

enum {
    RESEARCH_FUNDING_NONE,
    RESEARCH_FUNDING_MINIMUM,
    RESEARCH_FUNDING_NORMAL,
    RESEARCH_FUNDING_MAXIMUM,

    RESEARCH_FUNDING_COUNT
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
    RESEARCH_CATEGORY_SCENERY_GROUP
};

extern uint8 gResearchFundingLevel;
extern uint8 gResearchPriorities;
extern uint16 gResearchProgress;
extern uint8 gResearchProgressStage;
extern uint8 gResearchExpectedMonth;
extern uint8 gResearchExpectedDay;
extern rct_research_item gResearchLastItem;
extern rct_research_item gResearchNextItem;

extern rct_research_item gResearchItems[MAX_RESEARCH_ITEMS];
extern uint8 gResearchUncompletedCategories;
extern bool gSilentResearch;

void research_reset_items();
void research_update_uncompleted_types();
void research_update();
void research_reset_current_item();
void research_populate_list_random();
void research_populate_list_researched();
void research_process_random_items();

void research_finish_item(rct_research_item * researchItem);
void research_insert(sint32 researched, sint32 rawValue, uint8 category);
void research_remove(rct_research_item * researchItem);

void research_insert_ride_entry(uint8 entryIndex, bool researched);
void research_insert_scenery_group_entry(uint8 entryIndex, bool researched);

void ride_type_set_invented(uint32 rideType);
void ride_entry_set_invented(sint32 rideEntryIndex);
void scenery_set_invented(uint16 sceneryItem);
void scenery_set_not_invented(uint16 sceneryItem);
bool ride_type_is_invented(uint32 rideType);
bool ride_entry_is_invented(sint32 rideEntryIndex);
bool track_piece_is_available_for_ride_type(uint8 rideType, sint32 trackType);
bool scenery_group_is_invented(sint32 sgIndex);
void scenery_group_set_invented(sint32 sgIndex);
bool scenery_is_invented(uint16 sceneryItem);
void set_all_scenery_items_invented();
void set_all_scenery_items_not_invented();
void set_all_scenery_groups_not_invented();
void set_every_ride_type_invented();
void set_every_ride_type_not_invented();
void set_every_ride_entry_invented();
void set_every_ride_entry_not_invented();
rct_string_id research_item_get_name(const rct_research_item * researchItem);
rct_string_id research_get_friendly_base_ride_type_name(uint8 trackType, rct_ride_entry * rideEntry);
void research_remove_flags();
void research_fix();

void research_items_make_all_unresearched();
void research_items_make_all_researched();
void research_items_shuffle();
bool research_item_is_always_researched(rct_research_item * researchItem);
