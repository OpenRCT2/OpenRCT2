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

#include <algorithm>
#include <random>
#include "../actions/ParkSetResearchFundingAction.hpp"
#include "../config/Config.h"
#include "../core/Guard.hpp"
#include "../core/Util.hpp"
#include "../Game.h"
#include "../interface/Window.h"
#include "../localisation/Date.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../object/ObjectList.h"
#include "../OpenRCT2.h"
#include "../scenario/Scenario.h"
#include "../rct1/RCT1.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/RideGroupManager.h"
#include "../ride/TrackData.h"
#include "../world/Scenery.h"
#include "NewsItem.h"
#include "Finance.h"
#include "Research.h"
#include "../core/Memory.hpp"
#include "../util/Util.h"

static constexpr const sint32 _researchRate[] = {0, 160, 250, 400};

uint8  gResearchFundingLevel;
uint8  gResearchPriorities;
uint16 gResearchProgress;
uint8  gResearchProgressStage;
rct_research_item gResearchLastItem;
uint8  gResearchExpectedMonth;
uint8  gResearchExpectedDay;
rct_research_item gResearchNextItem;

rct_research_item gResearchItemSeparator = { RESEARCHED_ITEMS_SEPARATOR ,0};
rct_research_item gResearchItemEnd = { RESEARCHED_ITEMS_END, 0 };
std::vector<rct_research_item> gResearchedResearchItems;
std::vector<rct_research_item> gUnResearchedResearchItems;

// 0x00EE787C
uint8 gResearchUncompletedCategories;

static bool _researchedRideTypes[RIDE_TYPE_COUNT];
static bool _researchedRideEntries[MAX_RIDE_OBJECTS];
static bool _researchedSceneryItems[MAX_RESEARCHED_SCENERY_ITEMS];

bool gSilentResearch = false;

/**
 *
 *  rct2: 0x006671AD, part of 0x00667132
 */
void research_reset_items()
{
    gResearchedResearchItems.clear();
    gUnResearchedResearchItems.clear();
}

/**
 *
 *  rct2: 0x00684BAE
 */
void research_update_uncompleted_types()
{
    sint32 uncompletedResearchTypes = 0;
    for (const rct_research_item& researchItem : gUnResearchedResearchItems)
    {
        uncompletedResearchTypes |= (1 << researchItem.category);
    }

    gResearchUncompletedCategories = uncompletedResearchTypes;
}

/**
 *
 *  rct2: 0x00684D2A
 */
static void research_calculate_expected_date()
{
    if (gResearchProgressStage == RESEARCH_STAGE_INITIAL_RESEARCH || gResearchFundingLevel == RESEARCH_FUNDING_NONE)
    {
        gResearchExpectedDay = 255;
    }
    else
    {
        sint32 progressRemaining = gResearchProgressStage == RESEARCH_STAGE_COMPLETING_DESIGN ? 0x10000 : 0x20000;
        progressRemaining -= gResearchProgress;
        sint32 daysRemaining = (progressRemaining / _researchRate[gResearchFundingLevel]) * 128;

        sint32 expectedDay  = gDateMonthTicks + (daysRemaining & 0xFFFF);
        sint32 dayQuotient  = expectedDay / 0x10000;
        sint32 dayRemainder = expectedDay % 0x10000;

        sint32 expectedMonth = date_get_month(gDateMonthsElapsed + dayQuotient + (daysRemaining >> 16));
        expectedDay = (dayRemainder * days_in_month[expectedMonth]) >> 16;

        gResearchExpectedDay   = expectedDay;
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
    gResearchProgress = 0;

    if (gUnResearchedResearchItems.empty()) // Reset funding to 0 if no more rides.
    {
        gResearchProgressStage = RESEARCH_STAGE_FINISHED_ALL;
        research_invalidate_related_windows();
        auto gameAction = ParkSetResearchFundingAction(gResearchPriorities, 0);
        GameActions::Execute(&gameAction);
        return;
    }

    auto nextResearchItem = std::find_if(
        std::begin(gUnResearchedResearchItems),
        std::end(gUnResearchedResearchItems),
        [](const rct_research_item& researchItem) { return (gResearchPriorities & (1 << researchItem.category)); });

    if (nextResearchItem == gUnResearchedResearchItems.end())
    {
        nextResearchItem = gUnResearchedResearchItems.begin();
    }

    gResearchNextItem = *nextResearchItem;
    gResearchProgressStage = RESEARCH_STAGE_DESIGNING;

    research_invalidate_related_windows();
}

/**
 *
 *  rct2: 0x006848D4
 */
void research_finish_item(rct_research_item* researchItem)
{
    gResearchLastItem = *researchItem;
    research_invalidate_related_windows();

    if (researchItem->type == RESEARCH_ENTRY_TYPE_RIDE)
    {
        // Ride
        uint32 base_ride_type = researchItem->baseRideType;
        sint32 rideEntryIndex = researchItem->entryIndex;
        rct_ride_entry* rideEntry = get_ride_entry(rideEntryIndex);

        if (rideEntry != nullptr && base_ride_type != RIDE_TYPE_NULL)
        {
            bool ride_group_was_invented_before = false;
            bool ride_type_was_invented_before  = ride_type_is_invented(base_ride_type);
            rct_string_id availabilityString;

            // Determine if the ride group this entry belongs to was invented before.
            if (RideGroupManager::RideTypeHasRideGroups(base_ride_type))
            {
                const RideGroup* rideGroup = RideGroupManager::GetRideGroup(base_ride_type, rideEntry);

                if (rideGroup->IsInvented())
                {
                    ride_group_was_invented_before = true;
                }
            }

            ride_type_set_invented(base_ride_type);
            openrct2_assert(
                base_ride_type < Util::CountOf(RideTypePossibleTrackConfigurations),
                "Invalid base_ride_type = %d",
                base_ride_type);

            ride_entry_set_invented(rideEntryIndex);

            bool seenRideEntry[MAX_RIDE_OBJECTS];

            for (rct_research_item& researchItem2 : gResearchedResearchItems)
            {
                if (researchItem2.type == RESEARCH_ENTRY_TYPE_RIDE)
                {
                    uint8 index = researchItem2.entryIndex;
                    seenRideEntry[index] = true;
                }
            }

            // RCT2 made non-separated vehicles available at once, by removing all but one from research.
            // To ensure old files keep working, look for ride entries not in research, and make them available as well.
            for (sint32 i = 0; i < MAX_RIDE_OBJECTS; i++)
            {
                if (!seenRideEntry[i])
                {
                    rct_ride_entry* rideEntry2 = get_ride_entry(i);
                    if (rideEntry2 != nullptr)
                    {
                        for (uint8 j = 0; j < MAX_RIDE_TYPES_PER_RIDE_ENTRY; j++)
                        {
                            if (rideEntry2->ride_type[j] == base_ride_type)
                            {
                                ride_entry_set_invented(i);
                                break;
                            }
                        }
                    }
                }
            }

            // If a vehicle should be listed separately (maze, mini golf, flat rides, shops)
            if (RideGroupManager::RideTypeIsIndependent(base_ride_type))
            {
                availabilityString = STR_NEWS_ITEM_RESEARCH_NEW_RIDE_AVAILABLE;
                set_format_arg(0, rct_string_id, rideEntry->naming.name);
            }
            // If a vehicle is the first to be invented for its ride group, show the ride group name.
            else if (
                !ride_type_was_invented_before
                || (RideGroupManager::RideTypeHasRideGroups(base_ride_type) && !ride_group_was_invented_before))
            {
                rct_ride_name naming = get_ride_naming(base_ride_type, rideEntry);
                availabilityString = STR_NEWS_ITEM_RESEARCH_NEW_RIDE_AVAILABLE;
                set_format_arg(0, rct_string_id, naming.name);
            }
            // If the vehicle should not be listed separately and it isn't the first to be invented for its ride group,
            // report it as a new vehicle for the existing ride group.
            else
            {
                availabilityString = STR_NEWS_ITEM_RESEARCH_NEW_VEHICLE_AVAILABLE;
                rct_ride_name baseRideNaming = get_ride_naming(base_ride_type, rideEntry);

                set_format_arg(0, rct_string_id, baseRideNaming.name);
                set_format_arg(2, rct_string_id, rideEntry->naming.name);
            }

            if (!gSilentResearch)
            {
                if (gConfigNotifications.ride_researched)
                {
                    news_item_add_to_queue(NEWS_ITEM_RESEARCH, availabilityString, researchItem->rawValue);
                }
            }

            research_invalidate_related_windows();
        }
    }
    else
    {
        // Scenery
        rct_scenery_group_entry* sceneryGroupEntry = get_scenery_group_entry(researchItem->entryIndex);
        if (sceneryGroupEntry != nullptr)
        {
            scenery_group_set_invented(researchItem->entryIndex);

            set_format_arg(0, rct_string_id, sceneryGroupEntry->name);

            if (!gSilentResearch)
            {
                if (gConfigNotifications.ride_researched)
                {
                    news_item_add_to_queue(
                        NEWS_ITEM_RESEARCH, STR_NEWS_ITEM_RESEARCH_NEW_SCENERY_SET_AVAILABLE, researchItem->rawValue);
                }
            }

            research_invalidate_related_windows();
            init_scenery();
        }
    }
}

/**
 *
 *  rct2: 0x00684C7A
 */
void research_update()
{
    sint32 editorScreenFlags, researchLevel, currentResearchProgress;

    editorScreenFlags = SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER;
    if (gScreenFlags & editorScreenFlags)
    {
        return;
    }

    if (gScenarioTicks % 32 != 0)
    {
        return;
    }

    researchLevel = gResearchFundingLevel;

    currentResearchProgress = gResearchProgress;
    currentResearchProgress += _researchRate[researchLevel];
    if (currentResearchProgress <= 0xFFFF)
    {
        gResearchProgress = currentResearchProgress;
    }
    else
    {
        switch (gResearchProgressStage)
        {
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
                research_finish_item(&gResearchNextItem);
                gResearchProgress = 0;
                gResearchProgressStage = RESEARCH_STAGE_INITIAL_RESEARCH;
                research_calculate_expected_date();
                research_update_uncompleted_types();
                research_invalidate_related_windows();
                break;
        }
    }
}

void research_process_random_items()
{
    /*
    rct_research_item * research = gResearchItems;
    for (; research->rawValue != RESEARCHED_ITEMS_END; research++) { }

    research++;
    for (; research->rawValue != RESEARCHED_ITEMS_END_2; research += 2)
    {
        if (scenario_rand() & 1)
        {
            continue;
        }

        rct_research_item * edx            = nullptr;
        rct_research_item * ebp            = nullptr;
        rct_research_item * inner_research = gResearchItems;
        do
        {
            if (research->rawValue == inner_research->rawValue)
            {
                edx = inner_research;
            }
            if ((research + 1)->rawValue == inner_research->rawValue)
            {
                ebp = inner_research;
            }
        }
        while ((inner_research++)->rawValue != RESEARCHED_ITEMS_END);
        assert(edx != nullptr);
        edx->rawValue = research->rawValue;
        assert(ebp != nullptr);
        ebp->rawValue = (research + 1)->rawValue;

        uint8 cat = edx->category;
        edx->category = ebp->category;
        ebp->category = cat;
    }
    */
}

/**
 *
 *  rct2: 0x00684AC3
 */
void research_reset_current_item()
{
    research_process_random_items();

    set_every_ride_type_not_invented();
    set_every_ride_entry_not_invented();

    // The following two instructions together make all items not tied to a scenery group available.
    set_all_scenery_items_invented();
    set_all_scenery_groups_not_invented();

    for (rct_research_item& research : gResearchedResearchItems)
    {
        research_finish_item(&research);
    }

    gResearchLastItem = gResearchItemSeparator;
    gResearchProgressStage = RESEARCH_STAGE_INITIAL_RESEARCH;
    gResearchProgress = 0;
}

/**
 *
 *  rct2: 0x006857CF
 */
void research_remove(rct_research_item* researchItemToRemove)
{
    auto CompareResearchItems = [](const rct_research_item& First, const rct_research_item& Second) -> bool {
        return First.rawValue == Second.rawValue;
    };

    auto RemoveItemIfFound = [&](std::vector<rct_research_item>& ResearchItems) {
        auto Found = std::remove_if(
            std::begin(ResearchItems), std::end(ResearchItems), [&](const rct_research_item& researchItem) -> bool {
                return CompareResearchItems(*researchItemToRemove, researchItem);
            });
        if (Found != std::end(ResearchItems))
        {
            ResearchItems.erase(Found);
        }
    };

    RemoveItemIfFound(gResearchedResearchItems);
    RemoveItemIfFound(gUnResearchedResearchItems);
}

void research_insert(sint32 researched, sint32 rawValue, uint8 category)
{
    auto CompareRawValue = [rawValue](const rct_research_item& researchItem) { return researchItem.rawValue == rawValue; };

    if (gResearchedResearchItems.end()
            != std::find_if(std::begin(gResearchedResearchItems), std::end(gResearchedResearchItems), CompareRawValue)
        || gUnResearchedResearchItems.end()
            != std::find_if(std::begin(gUnResearchedResearchItems), std::end(gUnResearchedResearchItems), CompareRawValue))
    {
        return;
    }

    rct_research_item researchItem;
    researchItem.rawValue = rawValue;
    researchItem.category = category;

    if (researched)
    {
        gResearchedResearchItems.push_back(researchItem);
    }
    else
    {
        gUnResearchedResearchItems.push_back(researchItem);
    }
}

/**
 *
 *  rct2: 0x00685675
 */
void research_populate_list_random()
{
    // Rides
    for (sint32 i = 0; i < MAX_RIDE_OBJECTS; i++)
    {
        rct_ride_entry* rideEntry = get_ride_entry(i);
        if (rideEntry == nullptr)
        {
            continue;
        }

        sint32 researched = (scenario_rand() & 0xFF) > 128;
        for (auto rideType : rideEntry->ride_type)
        {
            if (rideType != RIDE_TYPE_NULL)
            {
                research_insert(researched, RESEARCH_ENTRY_RIDE_MASK | (rideType << 8) | i, rideEntry->category[0]);
            }
        }
    }

    // Scenery
    for (sint32 i = 0; i < MAX_SCENERY_GROUP_OBJECTS; i++)
    {
        rct_scenery_group_entry* sceneryGroupEntry = get_scenery_group_entry(i);
        if (sceneryGroupEntry == nullptr)
        {
            continue;
        }

        sint32 researched = (scenario_rand() & 0xFF) > 85;
        research_insert(researched, i, RESEARCH_CATEGORY_SCENERY_GROUP);
    }
}

void research_populate_list_researched()
{
    // Rides
    for (sint32 i = 0; i < MAX_RIDE_OBJECTS; i++)
    {
        rct_ride_entry* rideEntry = get_ride_entry(i);
        if (rideEntry == nullptr)
        {
            continue;
        }

        for (auto rideType : rideEntry->ride_type)
        {
            if (rideType != RIDE_TYPE_NULL)
            {
                research_insert(true, RESEARCH_ENTRY_RIDE_MASK | (rideType << 8) | i, rideEntry->category[0]);
            }
        }
    }

    // Scenery
    for (sint32 i = 0; i < MAX_SCENERY_GROUP_OBJECTS; i++)
    {
        rct_scenery_group_entry* sceneryGroupEntry = get_scenery_group_entry(i);
        if (sceneryGroupEntry == nullptr)
        {
            continue;
        }

        research_insert(true, i, RESEARCH_CATEGORY_SCENERY_GROUP);
    }
}

void research_insert_ride_entry(uint8 entryIndex, bool researched)
{
    rct_ride_entry* rideEntry = get_ride_entry(entryIndex);
    uint8 category = rideEntry->category[0];
    for (auto rideType : rideEntry->ride_type)
    {
        if (rideType != RIDE_TYPE_NULL)
        {
            research_insert(researched, RESEARCH_ENTRY_RIDE_MASK | (rideType << 8) | entryIndex, category);
        }
    }
}

void research_insert_scenery_group_entry(uint8 entryIndex, bool researched)
{
    research_insert(researched, entryIndex, RESEARCH_CATEGORY_SCENERY_GROUP);
}

bool ride_type_is_invented(uint32 rideType)
{
    Guard::Assert(rideType < Util::CountOf(_researchedRideTypes), GUARD_LINE);
    return _researchedRideTypes[rideType];
}

bool ride_entry_is_invented(sint32 rideEntryIndex)
{
    return _researchedRideEntries[rideEntryIndex];
}

bool track_piece_is_available_for_ride_type(uint8 rideType, sint32 trackType)
{
    return RideTypePossibleTrackConfigurations[rideType] & (1ULL << trackType);
}

void ride_type_set_invented(uint32 rideType)
{
    Guard::Assert(rideType < Util::CountOf(_researchedRideTypes), GUARD_LINE);
    _researchedRideTypes[rideType] = true;
}

void ride_entry_set_invented(sint32 rideEntryIndex)
{
    _researchedRideEntries[rideEntryIndex] = true;
}

bool scenery_is_invented(uint16 sceneryItem)
{
    return _researchedSceneryItems[sceneryItem];
}

void scenery_set_invented(uint16 sceneryItem)
{
    _researchedSceneryItems[sceneryItem] = true;
}

void scenery_set_not_invented(uint16 sceneryItem)
{
    _researchedSceneryItems[sceneryItem] = false;
}

bool scenery_group_is_invented(sint32 sgIndex)
{
    auto invented = false;
    const auto sgEntry = get_scenery_group_entry(sgIndex);
    if (sgEntry != nullptr && sgEntry->entry_count > 0)
    {
        if (gCheatsIgnoreResearchStatus)
        {
            invented = true;
        }
        else
        {
            for (auto i = 0; i < sgEntry->entry_count; i++)
            {
                auto sceneryEntryIndex = sgEntry->scenery_entries[i];
                if (scenery_is_invented(sceneryEntryIndex))
                {
                    invented = true;
                    break;
                }
            }
        }
    }
    return invented;
}

void scenery_group_set_invented(sint32 sgIndex)
{
    const auto sgEntry = get_scenery_group_entry(sgIndex);
    if (sgEntry != nullptr && sgEntry->entry_count > 0)
    {
        for (auto i = 0; i < sgEntry->entry_count; i++)
        {
            auto sceneryEntryIndex = sgEntry->scenery_entries[i];
            scenery_set_invented(sceneryEntryIndex);
        }
    }
}

void set_all_scenery_groups_not_invented()
{
    for (sint32 i = 0; i < MAX_SCENERY_GROUP_OBJECTS; ++i)
    {
        rct_scenery_group_entry* scenery_set = get_scenery_group_entry(i);
        if (scenery_set == nullptr)
        {
            continue;
        }

        for (sint32 j = 0; j < scenery_set->entry_count; ++j)
        {
            scenery_set_not_invented(scenery_set->scenery_entries[j]);
        }
    }
}

void set_all_scenery_items_invented()
{
    std::fill(std::begin(_researchedSceneryItems), std::end(_researchedSceneryItems), true);
}

void set_all_scenery_items_not_invented()
{
    std::fill(std::begin(_researchedSceneryItems), std::end(_researchedSceneryItems), false);
}

void set_every_ride_type_invented()
{
    std::fill(std::begin(_researchedRideTypes), std::end(_researchedRideTypes), true);
}

void set_every_ride_type_not_invented()
{
    std::fill(std::begin(_researchedRideTypes), std::end(_researchedRideTypes), false);
}

void set_every_ride_entry_invented()
{
    std::fill(std::begin(_researchedRideEntries), std::end(_researchedRideEntries), true);
}

void set_every_ride_entry_not_invented()
{
    std::fill(std::begin(_researchedRideEntries), std::end(_researchedRideEntries), false);
}

/**
 *
 *  rct2: 0x0068563D
 */
rct_string_id research_item_get_name(const rct_research_item* researchItem)
{
    if (researchItem->type == RESEARCH_ENTRY_TYPE_RIDE)
    {
        rct_ride_entry* rideEntry = get_ride_entry(researchItem->entryIndex);
        if (rideEntry == nullptr)
        {
            return STR_EMPTY;
        }
        else
        {
            return rideEntry->naming.name;
        }
    }
    else
    {
        rct_scenery_group_entry* sceneryEntry = get_scenery_group_entry(researchItem->entryIndex);
        if (sceneryEntry == nullptr)
        {
            return STR_EMPTY;
        }
        else
        {
            return sceneryEntry->name;
        }
    }
}

/**
 * This will return the name of the base ride type or ride group, as seen in the research window.
 */
rct_string_id research_get_friendly_base_ride_type_name(uint8 trackType, rct_ride_entry* rideEntry)
{
    if (RideGroupManager::RideTypeHasRideGroups(trackType))
    {
        const RideGroup* rideGroup = RideGroupManager::GetRideGroup(trackType, rideEntry);
        return rideGroup->Naming.name;
    }
    else
    {
        return RideNaming[trackType].name;
    }
}

/**
 *
 *  rct2: 0x00685A79
 *  Do not use the research list outside of the inventions list window with the flags
 */
void research_remove_flags()
{
    auto ClearAlwaysResearchedFlag = [](rct_research_item& researchItem) { researchItem.flags = 0; };

    std::for_each(std::begin(gResearchedResearchItems), std::end(gResearchedResearchItems), ClearAlwaysResearchedFlag);
    std::for_each(std::begin(gUnResearchedResearchItems), std::end(gUnResearchedResearchItems), ClearAlwaysResearchedFlag);
}

void research_fix()
{
    std::vector<rct_research_item> researchItemsToRemove;

    // Fix invalid research items
    for (rct_research_item& researchItem : gResearchedResearchItems)
    {
        if (researchItem.type == RESEARCH_ENTRY_TYPE_RIDE)
        {
            rct_ride_entry* rideEntry = get_ride_entry(researchItem.entryIndex);
            if (rideEntry == nullptr)
            {
                researchItemsToRemove.push_back(researchItem);
            }
        }
        else
        {
            rct_scenery_group_entry* sceneryGroupEntry = get_scenery_group_entry(researchItem.rawValue);
            if (sceneryGroupEntry == nullptr)
            {
                researchItemsToRemove.push_back(researchItem);
            }
        }
    }

    for (rct_research_item& researchItemToRemove : researchItemsToRemove)
    {
        research_remove(&researchItemToRemove);
    }

    research_update_uncompleted_types();
    if (gResearchUncompletedCategories == 0)
        gResearchProgressStage = RESEARCH_STAGE_FINISHED_ALL;

    // Sometimes ride entries are not in the research table.
    // If all research is done, simply insert all of them as researched.
    // For good measure, also include scenery groups.
    if (gResearchProgressStage == RESEARCH_STAGE_FINISHED_ALL)
    {
        for (uint8 i = 0; i < MAX_RIDE_OBJECTS; i++)
        {
            const rct_ride_entry* rideEntry = get_ride_entry(i);

            if (rideEntry)
            {
                research_insert_ride_entry(i, true);
                ride_entry_set_invented(i);

                for (uint8 j = 0; j < MAX_RIDE_TYPES_PER_RIDE_ENTRY; j++)
                {
                    uint32 rideType = rideEntry->ride_type[j];
                    if (rideType != RIDE_TYPE_NULL)
                    {
                        ride_type_set_invented(rideEntry->ride_type[j]);
                    }
                }
            }
        }

        for (uint8 i = 0; i < MAX_SCENERY_GROUP_OBJECTS; i++)
        {
            const rct_scenery_group_entry* groupEntry = get_scenery_group_entry(i);

            if (groupEntry)
                research_insert_scenery_group_entry(i, true);
        }
    }
}

void research_items_make_all_unresearched()
{
    std::vector<rct_research_item> alwaysResearchedItems;
    for (const rct_research_item& researchItem : gResearchedResearchItems)
    {
        if (research_item_is_always_researched(&researchItem))
        {
            alwaysResearchedItems.push_back(researchItem);
        }
        else
        {
            gUnResearchedResearchItems.push_back(researchItem);
        }
    }

    gResearchedResearchItems = alwaysResearchedItems;
}

void research_items_make_all_researched()
{
    gResearchedResearchItems.insert(
        gResearchedResearchItems.end(), gUnResearchedResearchItems.begin(), gUnResearchedResearchItems.end());
    gUnResearchedResearchItems.clear();
}

/**
 *
 *  rct2: 0x00685A93
 */
void research_items_shuffle()
{
    std::mt19937 g(util_rand());
    std::shuffle(std::begin(gUnResearchedResearchItems), std::end(gUnResearchedResearchItems), g);
}

bool research_item_is_always_researched(const rct_research_item* researchItem)
{
    return (researchItem->flags
            & (RESEARCH_ENTRY_FLAG_RIDE_ALWAYS_RESEARCHED | RESEARCH_ENTRY_FLAG_SCENERY_SET_ALWAYS_RESEARCHED))
        != 0;
}
