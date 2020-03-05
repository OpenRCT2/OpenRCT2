/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../object/ObjectLimits.h"
#include "../ride/Ride.h"

#include <optional>

struct rct_ride_entry;

struct ResearchItem
{
    // Bit 16 (0: scenery entry, 1: ride entry)
    union
    {
        uint32_t rawValue;
        struct
        {
            uint8_t entryIndex;
            uint8_t baseRideType;
            uint8_t type; // 0: scenery entry, 1: ride entry
            uint8_t flags;
        };
    };
    uint8_t category;

    bool IsNull() const;
    bool Equals(const ResearchItem* otherItem) const;
    bool Exists() const;
    bool IsAlwaysResearched() const;
    rct_string_id GetName() const;

    ResearchItem() = default;
    constexpr ResearchItem(uint32_t _rawValue, int32_t _category)
        : rawValue(_rawValue)
        , category(_category)
    {
    }
};

enum
{
    RESEARCH_ENTRY_TYPE_SCENERY = 0,
    RESEARCH_ENTRY_TYPE_RIDE = 1,
};

enum
{
    RESEARCH_ENTRY_FLAG_SCENERY_SET_ALWAYS_RESEARCHED = (1 << 5),
    RESEARCH_ENTRY_FLAG_RIDE_ALWAYS_RESEARCHED = (1 << 6),
};

// Only used to mark as null nowadays. Deprecated. TODO: remove.
#define RESEARCH_ITEM_NULL 0xFFFFFFFF

#define MAX_RESEARCH_ITEMS 500

#define RESEARCH_ENTRY_RIDE_MASK 0x10000

enum
{
    RESEARCH_FUNDING_NONE,
    RESEARCH_FUNDING_MINIMUM,
    RESEARCH_FUNDING_NORMAL,
    RESEARCH_FUNDING_MAXIMUM,

    RESEARCH_FUNDING_COUNT
};

enum
{
    RESEARCH_STAGE_INITIAL_RESEARCH,
    RESEARCH_STAGE_DESIGNING,
    RESEARCH_STAGE_COMPLETING_DESIGN,
    RESEARCH_STAGE_UNKNOWN,
    RESEARCH_STAGE_FINISHED_ALL
};

enum
{
    RESEARCH_CATEGORY_TRANSPORT,
    RESEARCH_CATEGORY_GENTLE,
    RESEARCH_CATEGORY_ROLLERCOASTER,
    RESEARCH_CATEGORY_THRILL,
    RESEARCH_CATEGORY_WATER,
    RESEARCH_CATEGORY_SHOP,
    RESEARCH_CATEGORY_SCENERY_GROUP
};

extern uint8_t gResearchFundingLevel;
extern uint8_t gResearchPriorities;
extern uint16_t gResearchProgress;
extern uint8_t gResearchProgressStage;
extern uint8_t gResearchExpectedMonth;
extern uint8_t gResearchExpectedDay;
extern std::optional<ResearchItem> gResearchLastItem;
extern std::optional<ResearchItem> gResearchNextItem;

extern std::vector<ResearchItem> gResearchItemsUninvented;
extern std::vector<ResearchItem> gResearchItemsInvented;
extern uint8_t gResearchUncompletedCategories;
extern bool gSilentResearch;

void research_reset_items();
void research_update_uncompleted_types();
void research_update();
void research_reset_current_item();
void research_populate_list_random();
void research_populate_list_researched();

void research_finish_item(ResearchItem* researchItem);
void research_insert(ResearchItem item, bool researched);
void research_remove(ResearchItem* researchItem);

bool research_insert_ride_entry(uint8_t rideType, ObjectEntryIndex entryIndex, uint8_t category, bool researched);
void research_insert_ride_entry(ObjectEntryIndex entryIndex, bool researched);
void research_insert_scenery_group_entry(ObjectEntryIndex entryIndex, bool researched);

void ride_type_set_invented(uint32_t rideType);
void ride_entry_set_invented(int32_t rideEntryIndex);
void scenery_set_invented(const ScenerySelection& sceneryItem);
void scenery_set_not_invented(const ScenerySelection& sceneryItem);
bool ride_type_is_invented(uint32_t rideType);
bool ride_entry_is_invented(int32_t rideEntryIndex);
uint64_t get_available_track_pieces_for_ride_type(uint8_t rideType);
bool track_piece_is_available_for_ride_type(uint8_t rideType, int32_t trackType);
bool scenery_group_is_invented(int32_t sgIndex);
void scenery_group_set_invented(int32_t sgIndex);
bool scenery_is_invented(const ScenerySelection& sceneryItem);
void set_all_scenery_items_invented();
void set_all_scenery_items_not_invented();
void set_all_scenery_groups_not_invented();
void set_every_ride_type_invented();
void set_every_ride_type_not_invented();
void set_every_ride_entry_invented();
void set_every_ride_entry_not_invented();
void research_remove_flags();
void research_fix();

void research_items_make_all_unresearched();
void research_items_make_all_researched();
void research_items_shuffle();
