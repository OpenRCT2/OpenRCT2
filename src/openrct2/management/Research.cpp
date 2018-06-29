/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
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
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "NewsItem.h"
#include "Finance.h"
#include "Research.h"
#include "../core/Memory.hpp"
#include "../util/Util.h"

static constexpr const int32_t _researchRate[] = {0, 160, 250, 400};

uint8_t  gResearchFundingLevel;
uint8_t  gResearchPriorities;
uint16_t gResearchProgress;
uint8_t  gResearchProgressStage;
rct_research_item gResearchLastItem;
uint8_t  gResearchExpectedMonth;
uint8_t  gResearchExpectedDay;
rct_research_item gResearchNextItem;

// 0x01358844[500]
rct_research_item gResearchItems[MAX_RESEARCH_ITEMS];

// 0x00EE787C
uint8_t gResearchUncompletedCategories;

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
    gResearchItems[0].rawValue = RESEARCHED_ITEMS_SEPARATOR;
    gResearchItems[1].rawValue = RESEARCHED_ITEMS_END;
    gResearchItems[2].rawValue = RESEARCHED_ITEMS_END_2;
}

/**
 *
 *  rct2: 0x00684BAE
 */
void research_update_uncompleted_types()
{
    int32_t uncompletedResearchTypes = 0;
    rct_research_item * researchItem = gResearchItems;
    while (researchItem++->rawValue != RESEARCHED_ITEMS_SEPARATOR);

    for (; researchItem->rawValue != RESEARCHED_ITEMS_END; researchItem++)
    {
        uncompletedResearchTypes |= (1 << researchItem->category);
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
        int32_t progressRemaining = gResearchProgressStage == RESEARCH_STAGE_COMPLETING_DESIGN ? 0x10000 : 0x20000;
        progressRemaining -= gResearchProgress;
        int32_t daysRemaining = (progressRemaining / _researchRate[gResearchFundingLevel]) * 128;

        int32_t expectedDay  = gDateMonthTicks + (daysRemaining & 0xFFFF);
        int32_t dayQuotient  = expectedDay / 0x10000;
        int32_t dayRemainder = expectedDay % 0x10000;

        int32_t expectedMonth = date_get_month(gDateMonthsElapsed + dayQuotient + (daysRemaining >> 16));
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
    rct_research_item * firstUnresearchedItem, * researchItem, tmp;
    int32_t ignoreActiveResearchTypes;

    // Skip already researched items
    firstUnresearchedItem = gResearchItems;
    while (firstUnresearchedItem->rawValue != RESEARCHED_ITEMS_SEPARATOR)
    {
        firstUnresearchedItem++;
    }

    ignoreActiveResearchTypes = 0;
    researchItem              = firstUnresearchedItem;
    for (;;)
    {
        researchItem++;
        if (researchItem->rawValue == RESEARCHED_ITEMS_END)
        {
            if (!ignoreActiveResearchTypes)
            {
                ignoreActiveResearchTypes = 1;
                researchItem              = firstUnresearchedItem;
                continue;
            }
            else
            {
                gResearchProgress      = 0;
                gResearchProgressStage = RESEARCH_STAGE_FINISHED_ALL;
                research_invalidate_related_windows();
                // Reset funding to 0 if no more rides.
                auto gameAction = ParkSetResearchFundingAction(gResearchPriorities, 0);
                GameActions::Execute(&gameAction);
                return;
            }
        }
        else if (ignoreActiveResearchTypes || (gResearchPriorities & (1 << researchItem->category)))
        {
            break;
        }
    }

    gResearchNextItem      = *researchItem;
    gResearchProgress      = 0;
    gResearchProgressStage = RESEARCH_STAGE_DESIGNING;

    // Bubble research item up until it is above the researched items separator
    do
    {
        tmp = *researchItem;
        *researchItem       = *(researchItem - 1);
        *(researchItem - 1) = tmp;
        researchItem--;
    }
    while ((researchItem + 1)->rawValue != RESEARCHED_ITEMS_SEPARATOR);

    research_invalidate_related_windows();
}

/**
 *
 *  rct2: 0x006848D4
 */
void research_finish_item(rct_research_item * researchItem)
{
    gResearchLastItem = *researchItem;
    research_invalidate_related_windows();

    if (researchItem->type == RESEARCH_ENTRY_TYPE_RIDE)
    {
        // Ride
        uint32_t base_ride_type = researchItem->baseRideType;
        int32_t rideEntryIndex = researchItem->entryIndex;
        rct_ride_entry * rideEntry = get_ride_entry(rideEntryIndex);

        if (rideEntry != nullptr && base_ride_type != RIDE_TYPE_NULL)
        {
            bool ride_group_was_invented_before = false;
            bool ride_type_was_invented_before  = ride_type_is_invented(base_ride_type);
            rct_string_id availabilityString;

            // Determine if the ride group this entry belongs to was invented before.
            if (RideGroupManager::RideTypeHasRideGroups(base_ride_type))
            {
                const RideGroup * rideGroup = RideGroupManager::GetRideGroup(base_ride_type, rideEntry);

                if (rideGroup->IsInvented())
                {
                    ride_group_was_invented_before = true;
                }
            }

            ride_type_set_invented(base_ride_type);
            openrct2_assert(base_ride_type < Util::CountOf(RideTypePossibleTrackConfigurations),
                            "Invalid base_ride_type = %d", base_ride_type);

            ride_entry_set_invented(rideEntryIndex);

            bool seenRideEntry[MAX_RIDE_OBJECTS];

            rct_research_item * researchItem2 = gResearchItems;
            for (; researchItem2->rawValue != RESEARCHED_ITEMS_END; researchItem2++)
            {
                if (researchItem2->rawValue != RESEARCHED_ITEMS_SEPARATOR &&
                    researchItem2->type == RESEARCH_ENTRY_TYPE_RIDE)
                {
                    uint8_t index = researchItem2->entryIndex;
                    seenRideEntry[index] = true;
                }
            }

            // RCT2 made non-separated vehicles available at once, by removing all but one from research.
            // To ensure old files keep working, look for ride entries not in research, and make them available as well.
            for (int32_t i = 0; i < MAX_RIDE_OBJECTS; i++)
            {
                if (!seenRideEntry[i])
                {
                    rct_ride_entry * rideEntry2 = get_ride_entry(i);
                    if (rideEntry2 != nullptr)
                    {
                        for (uint8_t j = 0; j < MAX_RIDE_TYPES_PER_RIDE_ENTRY; j++)
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
            else if (!ride_type_was_invented_before ||
                     (RideGroupManager::RideTypeHasRideGroups(base_ride_type) && !ride_group_was_invented_before))
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
        rct_scenery_group_entry * sceneryGroupEntry = get_scenery_group_entry(researchItem->entryIndex);
        if (sceneryGroupEntry != nullptr)
        {
            scenery_group_set_invented(researchItem->entryIndex);

            set_format_arg(0, rct_string_id, sceneryGroupEntry->name);

            if (!gSilentResearch)
            {
                if (gConfigNotifications.ride_researched)
                {
                    news_item_add_to_queue(NEWS_ITEM_RESEARCH, STR_NEWS_ITEM_RESEARCH_NEW_SCENERY_SET_AVAILABLE, researchItem->rawValue);
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
    int32_t editorScreenFlags, researchLevel, currentResearchProgress;

    editorScreenFlags = SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER;
    if (gScreenFlags & editorScreenFlags)
    {
        return;
    }

    if (gScenarioTicks % 32 != 0)
    {
        return;
    }

    if ((gParkFlags & PARK_FLAGS_NO_MONEY) && gResearchFundingLevel == RESEARCH_FUNDING_NONE) 
    {
        researchLevel = RESEARCH_FUNDING_NORMAL;
    } 
    else 
    {
        researchLevel = gResearchFundingLevel;
    }

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
            gResearchProgress      = 0;
            gResearchProgressStage = RESEARCH_STAGE_COMPLETING_DESIGN;
            research_calculate_expected_date();
            research_invalidate_related_windows();
            break;
        case RESEARCH_STAGE_COMPLETING_DESIGN:
            research_finish_item(&gResearchNextItem);
            gResearchProgress      = 0;
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

        uint8_t cat = edx->category;
        edx->category = ebp->category;
        ebp->category = cat;
    }
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


    for (rct_research_item * research = gResearchItems; research->rawValue != RESEARCHED_ITEMS_SEPARATOR; research++)
    {
        research_finish_item(research);
    }

    gResearchLastItem.rawValue = RESEARCHED_ITEMS_SEPARATOR;
    gResearchProgressStage     = RESEARCH_STAGE_INITIAL_RESEARCH;
    gResearchProgress          = 0;
}

/**
 *
 *  rct2: 0x006857FA
 */
static void research_insert_unresearched(int32_t rawValue, int32_t category)
{
    rct_research_item * researchItem, * researchItem2;

    researchItem = gResearchItems;
    do
    {
        if (researchItem->rawValue == RESEARCHED_ITEMS_END)
        {
            // Insert slot
            researchItem2 = researchItem;
            while (researchItem2->rawValue != RESEARCHED_ITEMS_END_2)
            {
                researchItem2++;
            }
            memmove(researchItem + 1, researchItem, (researchItem2 - researchItem + 1) * sizeof(rct_research_item));

            // Place new item
            researchItem->rawValue = rawValue;
            researchItem->category = category;
            break;
        }
    }
    while (rawValue != (researchItem++)->rawValue);
}

/**
 *
 *  rct2: 0x00685826
 */
static void research_insert_researched(int32_t rawValue, uint8_t category)
{
    rct_research_item * researchItem, * researchItem2;

    researchItem = gResearchItems;
    // First check to make sure that entry is not already accounted for
    for (; researchItem->rawValue != RESEARCHED_ITEMS_END; researchItem++)
    {
        if ((researchItem->rawValue & 0xFFFFFF) == (rawValue & 0xFFFFFF))
        {
            return;
        }
    }
    researchItem = gResearchItems;
    do
    {
        if (researchItem->rawValue == RESEARCHED_ITEMS_SEPARATOR)
        {
            // Insert slot
            researchItem2 = researchItem;
            while (researchItem2->rawValue != RESEARCHED_ITEMS_END_2)
            {
                researchItem2++;
            }
            memmove(researchItem + 1, researchItem, (researchItem2 - researchItem + 1) * sizeof(rct_research_item));

            // Place new item
            researchItem->rawValue = rawValue;
            researchItem->category = category;
            break;
        }
    }
    while (rawValue != (researchItem++)->rawValue);
}

/**
 *
 *  rct2: 0x006857CF
 */
void research_remove(rct_research_item * researchItem)
{
    for (rct_research_item * researchItem2 = gResearchItems;
         researchItem2->rawValue != RESEARCHED_ITEMS_END; researchItem2++)
    {
        if (researchItem2->rawValue == researchItem->rawValue)
        {
            do
            {
                *researchItem2 = *(researchItem2 + 1);
            }
            while (researchItem2++->rawValue != RESEARCHED_ITEMS_END_2);
            return;
        }
    }
}

void research_insert(int32_t researched, int32_t rawValue, uint8_t category)
{
    if (researched)
    {
        research_insert_researched(rawValue, category);
    }
    else
    {
        research_insert_unresearched(rawValue, category);
    }
}

/**
 *
 *  rct2: 0x00685675
 */
void research_populate_list_random()
{
    research_reset_items();

    // Rides
    for (int32_t i = 0; i < MAX_RIDE_OBJECTS; i++)
    {
        rct_ride_entry * rideEntry = get_ride_entry(i);
        if (rideEntry == nullptr)
        {
            continue;
        }

        int32_t researched = (scenario_rand() & 0xFF) > 128;
        for (auto rideType : rideEntry->ride_type)
        {
            if (rideType != RIDE_TYPE_NULL)
            {
                research_insert(researched, RESEARCH_ENTRY_RIDE_MASK | (rideType << 8) | i, rideEntry->category[0]);
            }
        }
    }

    // Scenery
    for (int32_t i = 0; i < MAX_SCENERY_GROUP_OBJECTS; i++)
    {
        rct_scenery_group_entry * sceneryGroupEntry = get_scenery_group_entry(i);
        if (sceneryGroupEntry == nullptr)
        {
            continue;
        }

        int32_t researched = (scenario_rand() & 0xFF) > 85;
        research_insert(researched, i, RESEARCH_CATEGORY_SCENERY_GROUP);
    }
}

void research_populate_list_researched()
{
    // Rides
    for (int32_t i = 0; i < MAX_RIDE_OBJECTS; i++)
    {
        rct_ride_entry * rideEntry = get_ride_entry(i);
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
    for (int32_t i = 0; i < MAX_SCENERY_GROUP_OBJECTS; i++)
    {
        rct_scenery_group_entry * sceneryGroupEntry = get_scenery_group_entry(i);
        if (sceneryGroupEntry == nullptr)
        {
            continue;
        }

        research_insert(true, i, RESEARCH_CATEGORY_SCENERY_GROUP);
    }
}

void research_insert_ride_entry(uint8_t entryIndex, bool researched)
{
    rct_ride_entry * rideEntry = get_ride_entry(entryIndex);
    uint8_t category = rideEntry->category[0];
    for (auto rideType : rideEntry->ride_type)
    {
        if (rideType != RIDE_TYPE_NULL)
        {
            research_insert(researched, RESEARCH_ENTRY_RIDE_MASK | (rideType << 8) | entryIndex, category);
        }
    }
}

void research_insert_scenery_group_entry(uint8_t entryIndex, bool researched)
{
    research_insert(researched, entryIndex, RESEARCH_CATEGORY_SCENERY_GROUP);
}

bool ride_type_is_invented(uint32_t rideType)
{
    Guard::Assert(rideType < Util::CountOf(_researchedRideTypes), GUARD_LINE);
    return _researchedRideTypes[rideType];
}

bool ride_entry_is_invented(int32_t rideEntryIndex)
{
    return _researchedRideEntries[rideEntryIndex];
}

bool track_piece_is_available_for_ride_type(uint8_t rideType, int32_t trackType)
{
    return RideTypePossibleTrackConfigurations[rideType] & (1ULL << trackType);
}

void ride_type_set_invented(uint32_t rideType)
{
    Guard::Assert(rideType < Util::CountOf(_researchedRideTypes), GUARD_LINE);
    _researchedRideTypes[rideType] = true;
}

void ride_entry_set_invented(int32_t rideEntryIndex)
{
    _researchedRideEntries[rideEntryIndex] = true;
}

bool scenery_is_invented(uint16_t sceneryItem)
{
    return _researchedSceneryItems[sceneryItem];
}

void scenery_set_invented(uint16_t sceneryItem)
{
    _researchedSceneryItems[sceneryItem] = true;
}

void scenery_set_not_invented(uint16_t sceneryItem)
{
    _researchedSceneryItems[sceneryItem] = false;
}

bool scenery_group_is_invented(int32_t sgIndex)
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

void scenery_group_set_invented(int32_t sgIndex)
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
    for (int32_t i = 0; i < MAX_SCENERY_GROUP_OBJECTS; ++i)
    {
        rct_scenery_group_entry * scenery_set = get_scenery_group_entry(i);
        if (scenery_set == nullptr)
        {
            continue;
        }

        for (int32_t j = 0; j < scenery_set->entry_count; ++j)
        {
            scenery_set_not_invented(scenery_set->scenery_entries[j]);
        }
    }
}

void set_all_scenery_items_invented()
{
    std::fill(
        std::begin(_researchedSceneryItems),
        std::end(_researchedSceneryItems),
        true);
}

void set_all_scenery_items_not_invented()
{
    std::fill(
        std::begin(_researchedSceneryItems),
        std::end(_researchedSceneryItems),
        false);
}

void set_every_ride_type_invented()
{
    std::fill(
        std::begin(_researchedRideTypes),
        std::end(_researchedRideTypes),
        true);
}

void set_every_ride_type_not_invented()
{
    std::fill(
        std::begin(_researchedRideTypes),
        std::end(_researchedRideTypes),
        false);
}

void set_every_ride_entry_invented()
{
    std::fill(
        std::begin(_researchedRideEntries),
        std::end(_researchedRideEntries),
        true);
}

void set_every_ride_entry_not_invented()
{
    std::fill(
        std::begin(_researchedRideEntries),
        std::end(_researchedRideEntries),
        false);
}

/**
 *
 *  rct2: 0x0068563D
 */
rct_string_id research_item_get_name(const rct_research_item * researchItem)
{

    if (researchItem->type == RESEARCH_ENTRY_TYPE_RIDE)
    {
        rct_ride_entry * rideEntry = get_ride_entry(researchItem->entryIndex);
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
        rct_scenery_group_entry * sceneryEntry = get_scenery_group_entry(researchItem->entryIndex);
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
rct_string_id research_get_friendly_base_ride_type_name(uint8_t trackType, rct_ride_entry * rideEntry)
{
    if (RideGroupManager::RideTypeHasRideGroups(trackType))
    {
        const RideGroup * rideGroup = RideGroupManager::GetRideGroup(trackType, rideEntry);
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
    for (rct_research_item * research = gResearchItems; research->rawValue != RESEARCHED_ITEMS_END_2; research++)
    {
        // Clear the always researched flags.
        if (research->rawValue > RESEARCHED_ITEMS_SEPARATOR)
        {
            research->flags = 0;
        }
    }
}

void research_fix()
{
    // Fix invalid research items
    for (int32_t i = 0; i < MAX_RESEARCH_ITEMS; i++)
    {
        rct_research_item * researchItem = &gResearchItems[i];
        if (researchItem->rawValue == RESEARCHED_ITEMS_SEPARATOR)
            continue;
        if (researchItem->rawValue == RESEARCHED_ITEMS_END)
        {
            if (i == MAX_RESEARCH_ITEMS - 1)
            {
                (--researchItem)->rawValue = RESEARCHED_ITEMS_END;
            }
            (++researchItem)->rawValue = RESEARCHED_ITEMS_END_2;
            break;
        }
        if (researchItem->rawValue == RESEARCHED_ITEMS_END_2)
            break;
        if (researchItem->type == RESEARCH_ENTRY_TYPE_RIDE)
        {
            rct_ride_entry * rideEntry = get_ride_entry(researchItem->entryIndex);
            if (rideEntry == nullptr)
            {
                research_remove(researchItem);
                i--;
            }
        }
        else
        {
            rct_scenery_group_entry * sceneryGroupEntry = get_scenery_group_entry(researchItem->rawValue);
            if (sceneryGroupEntry == nullptr)
            {
                research_remove(researchItem);
                i--;
            }
        }
    }

    research_update_uncompleted_types();
    if (gResearchUncompletedCategories == 0)
        gResearchProgressStage = RESEARCH_STAGE_FINISHED_ALL;

    // Sometimes ride entries are not in the research table.
    // If all research is done, simply insert all of them as researched.
    // For good measure, also include scenery groups.
    if (gResearchProgressStage == RESEARCH_STAGE_FINISHED_ALL)
    {
        for (uint8_t i = 0; i < MAX_RIDE_OBJECTS; i++)
        {
            const rct_ride_entry * rideEntry = get_ride_entry(i);

            if (rideEntry != nullptr)
            {
                research_insert_ride_entry(i, true);
                ride_entry_set_invented(i);

                for (uint8_t j = 0; j < MAX_RIDE_TYPES_PER_RIDE_ENTRY; j++)
                {
                    uint32_t rideType = rideEntry->ride_type[j];
                    if (rideType != RIDE_TYPE_NULL)
                    {
                        ride_type_set_invented(rideEntry->ride_type[j]);
                    }
                }
            }
        }

        for (uint8_t i = 0; i < MAX_SCENERY_GROUP_OBJECTS; i++)
        {
            const rct_scenery_group_entry * groupEntry = get_scenery_group_entry(i);

            if (groupEntry != nullptr)
                research_insert_scenery_group_entry(i, true);
        }
    }
}

void research_items_make_all_unresearched()
{
    rct_research_item * researchItem, * nextResearchItem, researchItemTemp;

    int32_t sorted;
    do {
        sorted = 1;
        for (researchItem = gResearchItems; researchItem->rawValue != RESEARCHED_ITEMS_SEPARATOR; researchItem++)
        {
            if (research_item_is_always_researched(researchItem))
                continue;

            nextResearchItem = researchItem + 1;
            if (nextResearchItem->rawValue == RESEARCHED_ITEMS_SEPARATOR || research_item_is_always_researched(nextResearchItem))
            {
                // Bubble up always researched item or separator
                researchItemTemp = *researchItem;
                *researchItem = *nextResearchItem;
                *nextResearchItem = researchItemTemp;
                sorted = 0;

                if (researchItem->rawValue == RESEARCHED_ITEMS_SEPARATOR)
                    break;
            }
        }
    }
    while (!sorted);
}

void research_items_make_all_researched()
{
    rct_research_item * researchItem, researchItemTemp;

    // Find separator
    for (researchItem = gResearchItems; researchItem->rawValue != RESEARCHED_ITEMS_SEPARATOR; researchItem++) { }

    // Move separator below all items
    for (; (researchItem + 1)->rawValue != RESEARCHED_ITEMS_END; researchItem++)
    {
        // Swap separator with research item
        researchItemTemp = *researchItem;
        *researchItem = *(researchItem + 1);
        *(researchItem + 1) = researchItemTemp;
    }
}

/**
 *
 *  rct2: 0x00685A93
 */
void research_items_shuffle()
{
    rct_research_item * researchItem, * researchOrderBase, researchItemTemp;
    int32_t i, numNonResearchedItems;

    // Skip pre-researched items
    for (researchItem = gResearchItems; researchItem->rawValue != RESEARCHED_ITEMS_SEPARATOR; researchItem++) {}
    researchItem++;
    researchOrderBase = researchItem;

    // Count non pre-researched items
    numNonResearchedItems = 0;
    for (; researchItem->rawValue != RESEARCHED_ITEMS_END; researchItem++)
        numNonResearchedItems++;

    // Shuffle list
    for (i = 0; i < numNonResearchedItems; i++)
    {
        int32_t ri = util_rand() % numNonResearchedItems;
        if (ri == i)
            continue;

        researchItemTemp = researchOrderBase[i];
        researchOrderBase[i] = researchOrderBase[ri];
        researchOrderBase[ri] = researchItemTemp;
    }
}

bool research_item_is_always_researched(rct_research_item * researchItem)
{
    return (researchItem->flags & (RESEARCH_ENTRY_FLAG_RIDE_ALWAYS_RESEARCHED | RESEARCH_ENTRY_FLAG_SCENERY_SET_ALWAYS_RESEARCHED)) != 0;
}

bool rct_research_item::IsInventedEndMarker() const
{
    return rawValue == RESEARCHED_ITEMS_SEPARATOR;
}

bool rct_research_item::IsUninventedEndMarker() const
{
    return rawValue == RESEARCHED_ITEMS_END;
}

bool rct_research_item::IsRandomEndMarker() const
{
    return rawValue == RESEARCHED_ITEMS_END_2;
}
