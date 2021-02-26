/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Research.h"

#include "../Game.h"
#include "../OpenRCT2.h"
#include "../actions/ParkSetResearchFundingAction.h"
#include "../config/Config.h"
#include "../core/Guard.hpp"
#include "../core/Memory.hpp"
#include "../interface/Window.h"
#include "../localisation/Date.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../object/ObjectList.h"
#include "../rct1/RCT1.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/TrackData.h"
#include "../scenario/Scenario.h"
#include "../util/Util.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "Finance.h"
#include "NewsItem.h"

#include <algorithm>
#include <iterator>

static constexpr const int32_t _researchRate[] = {
    0,
    160,
    250,
    400,
};

uint8_t gResearchFundingLevel;
uint8_t gResearchPriorities;
uint16_t gResearchProgress;
uint8_t gResearchProgressStage;
std::optional<ResearchItem> gResearchLastItem;
uint8_t gResearchExpectedMonth;
uint8_t gResearchExpectedDay;
std::optional<ResearchItem> gResearchNextItem;

std::vector<ResearchItem> gResearchItemsUninvented;
std::vector<ResearchItem> gResearchItemsInvented;

// 0x00EE787C
uint8_t gResearchUncompletedCategories;

static bool _researchedRideTypes[RIDE_TYPE_COUNT];
static bool _researchedRideEntries[MAX_RIDE_OBJECTS];
static bool _researchedSceneryItems[SCENERY_TYPE_COUNT][UINT16_MAX];

bool gSilentResearch = false;

/**
 *
 *  rct2: 0x006671AD, part of 0x00667132
 */
void research_reset_items()
{
    gResearchItemsUninvented.clear();
    gResearchItemsInvented.clear();
}

/**
 *
 *  rct2: 0x00684BAE
 */
void research_update_uncompleted_types()
{
    int32_t uncompletedResearchTypes = 0;

    for (auto const& researchItem : gResearchItemsUninvented)
    {
        uncompletedResearchTypes |= EnumToFlag(researchItem.category);
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

        int32_t expectedDay = gDateMonthTicks + (daysRemaining & 0xFFFF);
        int32_t dayQuotient = expectedDay / 0x10000;
        int32_t dayRemainder = expectedDay % 0x10000;

        int32_t expectedMonth = date_get_month(gDateMonthsElapsed + dayQuotient + (daysRemaining >> 16));
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

static void research_mark_as_fully_completed()
{
    gResearchProgress = 0;
    gResearchProgressStage = RESEARCH_STAGE_FINISHED_ALL;
    research_invalidate_related_windows();
    // Reset funding to 0 if no more rides.
    auto gameAction = ParkSetResearchFundingAction(gResearchPriorities, 0);
    GameActions::Execute(&gameAction);
}

/**
 *
 *  rct2: 0x00684BE5
 */
static void research_next_design()
{
    if (gResearchItemsUninvented.empty())
    {
        research_mark_as_fully_completed();
        return;
    }

    ResearchItem researchItem;

    bool ignoreActiveResearchTypes = false;
    auto it = gResearchItemsUninvented.begin();
    for (;;)
    {
        researchItem = *it;
        if (it == gResearchItemsUninvented.end())
        {
            if (!ignoreActiveResearchTypes)
            {
                ignoreActiveResearchTypes = true;
                it = gResearchItemsUninvented.begin();
                continue;
            }
            else
            {
                research_mark_as_fully_completed();
                return;
            }
        }
        else if (ignoreActiveResearchTypes || (gResearchPriorities & EnumToFlag(researchItem.category)))
        {
            break;
        }
        it++;
    }

    gResearchNextItem = researchItem;
    gResearchProgress = 0;
    gResearchProgressStage = RESEARCH_STAGE_DESIGNING;

    gResearchItemsUninvented.erase(it);
    gResearchItemsInvented.push_back(std::move(researchItem));

    research_invalidate_related_windows();
}

/**
 *
 *  rct2: 0x006848D4
 */
void research_finish_item(ResearchItem* researchItem)
{
    gResearchLastItem = *researchItem;
    research_invalidate_related_windows();

    if (researchItem->type == Research::EntryType::Ride)
    {
        // Ride
        uint32_t base_ride_type = researchItem->baseRideType;
        ObjectEntryIndex rideEntryIndex = researchItem->entryIndex;
        rct_ride_entry* rideEntry = get_ride_entry(rideEntryIndex);

        if (rideEntry != nullptr && base_ride_type != RIDE_TYPE_NULL)
        {
            rct_string_id availabilityString;

            ride_type_set_invented(base_ride_type);
            openrct2_assert(base_ride_type < RIDE_TYPE_COUNT, "Invalid base_ride_type = %d", base_ride_type);

            ride_entry_set_invented(rideEntryIndex);

            bool seenRideEntry[MAX_RIDE_OBJECTS]{};
            for (auto const& researchItem3 : gResearchItemsUninvented)
            {
                ObjectEntryIndex index = researchItem3.entryIndex;
                seenRideEntry[index] = true;
            }
            for (auto const& researchItem3 : gResearchItemsInvented)
            {
                ObjectEntryIndex index = researchItem3.entryIndex;
                seenRideEntry[index] = true;
            }

            // RCT2 made non-separated vehicles available at once, by removing all but one from research.
            // To ensure old files keep working, look for ride entries not in research, and make them available as well.
            for (int32_t i = 0; i < MAX_RIDE_OBJECTS; i++)
            {
                if (!seenRideEntry[i])
                {
                    rct_ride_entry* rideEntry2 = get_ride_entry(i);
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

            Formatter ft;

            // If a vehicle is the first to be invented for its ride type, show the ride type/group name.
            // Independently listed vehicles (like all flat rides and shops) should always be announced as such.
            if (GetRideTypeDescriptor(base_ride_type).HasFlag(RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY)
                || researchItem->flags & RESEARCH_ENTRY_FLAG_FIRST_OF_TYPE)
            {
                RideNaming naming = get_ride_naming(base_ride_type, rideEntry);
                availabilityString = STR_NEWS_ITEM_RESEARCH_NEW_RIDE_AVAILABLE;
                ft.Add<rct_string_id>(naming.Name);
            }
            // If the vehicle should not be listed separately and it isn't the first to be invented for its ride group,
            // report it as a new vehicle for the existing ride group.
            else
            {
                availabilityString = STR_NEWS_ITEM_RESEARCH_NEW_VEHICLE_AVAILABLE;
                RideNaming baseRideNaming = get_ride_naming(base_ride_type, rideEntry);

                ft.Add<rct_string_id>(baseRideNaming.Name);
                ft.Add<rct_string_id>(rideEntry->naming.Name);
            }

            if (!gSilentResearch)
            {
                if (gConfigNotifications.ride_researched)
                {
                    News::AddItemToQueue(News::ItemType::Research, availabilityString, researchItem->rawValue, ft);
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

            Formatter ft;
            ft.Add<rct_string_id>(sceneryGroupEntry->name);

            if (!gSilentResearch)
            {
                if (gConfigNotifications.ride_researched)
                {
                    News::AddItemToQueue(
                        News::ItemType::Research, STR_NEWS_ITEM_RESEARCH_NEW_SCENERY_SET_AVAILABLE, researchItem->rawValue, ft);
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
                gResearchProgress = 0;
                gResearchProgressStage = RESEARCH_STAGE_COMPLETING_DESIGN;
                research_calculate_expected_date();
                research_invalidate_related_windows();
                break;
            case RESEARCH_STAGE_COMPLETING_DESIGN:
                research_finish_item(&*gResearchNextItem);
                gResearchProgress = 0;
                gResearchProgressStage = RESEARCH_STAGE_INITIAL_RESEARCH;
                research_calculate_expected_date();
                research_update_uncompleted_types();
                research_invalidate_related_windows();
                break;
            case RESEARCH_STAGE_FINISHED_ALL:
                gResearchFundingLevel = RESEARCH_FUNDING_NONE;
                break;
        }
    }
}

/**
 *
 *  rct2: 0x00684AC3
 */
void research_reset_current_item()
{
    set_every_ride_type_not_invented();
    set_every_ride_entry_not_invented();

    // The following two instructions together make all items not tied to a scenery group available.
    set_all_scenery_items_invented();
    set_all_scenery_groups_not_invented();

    for (auto& researchItem : gResearchItemsInvented)
    {
        research_finish_item(&researchItem);
    }

    gResearchLastItem = std::nullopt;
    gResearchProgressStage = RESEARCH_STAGE_INITIAL_RESEARCH;
    gResearchProgress = 0;
}

/**
 *
 *  rct2: 0x006857FA
 */
static void research_insert_unresearched(ResearchItem&& item)
{
    gResearchItemsUninvented.push_back(std::move(item));
}

/**
 *
 *  rct2: 0x00685826
 */
static void research_insert_researched(ResearchItem&& item)
{
    // First check to make sure that entry is not already accounted for
    if (item.Exists())
    {
        return;
    }

    gResearchItemsInvented.push_back(std::move(item));
}

/**
 *
 *  rct2: 0x006857CF
 */
void research_remove(ResearchItem* researchItem)
{
    for (auto it = gResearchItemsUninvented.begin(); it != gResearchItemsUninvented.end(); it++)
    {
        auto& researchItem2 = *it;
        if (researchItem2.Equals(researchItem))
        {
            gResearchItemsUninvented.erase(it);
            return;
        }
    }
    for (auto it = gResearchItemsInvented.begin(); it != gResearchItemsInvented.end(); it++)
    {
        auto& researchItem2 = *it;
        if (researchItem2.Equals(researchItem))
        {
            gResearchItemsInvented.erase(it);
            return;
        }
    }
}

void research_insert(ResearchItem&& item, bool researched)
{
    if (researched)
    {
        research_insert_researched(std::move(item));
    }
    else
    {
        research_insert_unresearched(std::move(item));
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
        rct_ride_entry* rideEntry = get_ride_entry(i);
        if (rideEntry == nullptr)
        {
            continue;
        }

        int32_t researched = (scenario_rand() & 0xFF) > 128;
        for (auto rideType : rideEntry->ride_type)
        {
            if (rideType != RIDE_TYPE_NULL)
            {
                ResearchCategory category = GetRideTypeDescriptor(rideType).GetResearchCategory();
                research_insert_ride_entry(rideType, i, category, researched);
            }
        }
    }

    // Scenery
    for (uint32_t i = 0; i < MAX_SCENERY_GROUP_OBJECTS; i++)
    {
        rct_scenery_group_entry* sceneryGroupEntry = get_scenery_group_entry(i);
        if (sceneryGroupEntry == nullptr)
        {
            continue;
        }

        int32_t researched = (scenario_rand() & 0xFF) > 85;
        research_insert_scenery_group_entry(i, researched);
    }
}

bool research_insert_ride_entry(uint8_t rideType, ObjectEntryIndex entryIndex, ResearchCategory category, bool researched)
{
    if (rideType != RIDE_TYPE_NULL && entryIndex != OBJECT_ENTRY_INDEX_NULL)
    {
        auto tmpItem = ResearchItem(Research::EntryType::Ride, entryIndex, rideType, category, 0);
        research_insert(std::move(tmpItem), researched);
        return true;
    }

    return false;
}

void research_insert_ride_entry(ObjectEntryIndex entryIndex, bool researched)
{
    rct_ride_entry* rideEntry = get_ride_entry(entryIndex);
    for (auto rideType : rideEntry->ride_type)
    {
        if (rideType != RIDE_TYPE_NULL)
        {
            ResearchCategory category = GetRideTypeDescriptor(rideType).GetResearchCategory();
            research_insert_ride_entry(rideType, entryIndex, category, researched);
        }
    }
}

bool research_insert_scenery_group_entry(ObjectEntryIndex entryIndex, bool researched)
{
    if (entryIndex != OBJECT_ENTRY_INDEX_NULL)
    {
        auto tmpItem = ResearchItem(
            Research::EntryType::Scenery, entryIndex, RIDE_TYPE_NULL, ResearchCategory::SceneryGroup, 0);
        research_insert(std::move(tmpItem), researched);
        return true;
    }
    return false;
}

bool ride_type_is_invented(uint32_t rideType)
{
    Guard::Assert(rideType < std::size(_researchedRideTypes), GUARD_LINE);
    return _researchedRideTypes[rideType];
}

bool ride_entry_is_invented(int32_t rideEntryIndex)
{
    return _researchedRideEntries[rideEntryIndex];
}

void ride_type_set_invented(uint32_t rideType)
{
    Guard::Assert(rideType < std::size(_researchedRideTypes), GUARD_LINE);
    _researchedRideTypes[rideType] = true;
}

void ride_entry_set_invented(int32_t rideEntryIndex)
{
    _researchedRideEntries[rideEntryIndex] = true;
}

bool scenery_is_invented(const ScenerySelection& sceneryItem)
{
    if (sceneryItem.SceneryType < SCENERY_TYPE_COUNT)
    {
        return _researchedSceneryItems[sceneryItem.SceneryType][sceneryItem.EntryIndex];
    }
    else
    {
        log_warning("Invalid Scenery Type");
        return false;
    }
}

void scenery_set_invented(const ScenerySelection& sceneryItem)
{
    if (sceneryItem.SceneryType < SCENERY_TYPE_COUNT)
    {
        _researchedSceneryItems[sceneryItem.SceneryType][sceneryItem.EntryIndex] = true;
    }
    else
    {
        log_warning("Invalid Scenery Type");
    }
}

void scenery_set_not_invented(const ScenerySelection& sceneryItem)
{
    if (sceneryItem.SceneryType < SCENERY_TYPE_COUNT)
    {
        _researchedSceneryItems[sceneryItem.SceneryType][sceneryItem.EntryIndex] = false;
    }
    else
    {
        log_warning("Invalid Scenery Type");
    }
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
                auto sceneryEntry = sgEntry->scenery_entries[i];
                if (scenery_is_invented(sceneryEntry))
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
        rct_scenery_group_entry* scenery_set = get_scenery_group_entry(i);
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
    for (auto sceneryType = 0; sceneryType < SCENERY_TYPE_COUNT; sceneryType++)
    {
        std::fill(std::begin(_researchedSceneryItems[sceneryType]), std::end(_researchedSceneryItems[sceneryType]), true);
    }
}

void set_all_scenery_items_not_invented()
{
    for (auto sceneryType = 0; sceneryType < SCENERY_TYPE_COUNT; sceneryType++)
    {
        std::fill(std::begin(_researchedSceneryItems[sceneryType]), std::end(_researchedSceneryItems[sceneryType]), true);
    }
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
rct_string_id ResearchItem::GetName() const
{
    if (type == Research::EntryType::Ride)
    {
        rct_ride_entry* rideEntry = get_ride_entry(entryIndex);
        if (rideEntry == nullptr)
        {
            return STR_EMPTY;
        }
        else
        {
            return rideEntry->naming.Name;
        }
    }
    else
    {
        rct_scenery_group_entry* sceneryEntry = get_scenery_group_entry(entryIndex);
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
 *
 *  rct2: 0x00685A79
 *  Do not use the research list outside of the inventions list window with the flags
 *  Clears flags like "always researched".
 */
void research_remove_flags()
{
    for (auto& researchItem : gResearchItemsUninvented)
    {
        researchItem.flags &= ~(RESEARCH_ENTRY_FLAG_RIDE_ALWAYS_RESEARCHED | RESEARCH_ENTRY_FLAG_SCENERY_SET_ALWAYS_RESEARCHED);
    }
    for (auto& researchItem : gResearchItemsInvented)
    {
        researchItem.flags &= ~(RESEARCH_ENTRY_FLAG_RIDE_ALWAYS_RESEARCHED | RESEARCH_ENTRY_FLAG_SCENERY_SET_ALWAYS_RESEARCHED);
    }
}

void research_fix()
{
    // Fix invalid research items
    for (auto it = gResearchItemsInvented.begin(); it != gResearchItemsInvented.end();)
    {
        auto& researchItem = *it;
        if (researchItem.type == Research::EntryType::Ride)
        {
            rct_ride_entry* rideEntry = get_ride_entry(researchItem.entryIndex);
            if (rideEntry == nullptr)
            {
                it = gResearchItemsInvented.erase(it);
            }
            else
            {
                it++;
            }
        }
        else
        {
            rct_scenery_group_entry* sceneryGroupEntry = get_scenery_group_entry(researchItem.entryIndex);
            if (sceneryGroupEntry == nullptr)
            {
                it = gResearchItemsInvented.erase(it);
            }
            else
            {
                it++;
            }
        }
    }
    for (auto it = gResearchItemsUninvented.begin(); it != gResearchItemsUninvented.end();)
    {
        auto& researchItem = *it;
        if (researchItem.type == Research::EntryType::Ride)
        {
            rct_ride_entry* rideEntry = get_ride_entry(researchItem.entryIndex);
            if (rideEntry == nullptr)
            {
                it = gResearchItemsUninvented.erase(it);
            }
            else
            {
                it++;
            }
        }
        else
        {
            rct_scenery_group_entry* sceneryGroupEntry = get_scenery_group_entry(researchItem.entryIndex);
            if (sceneryGroupEntry == nullptr)
            {
                it = gResearchItemsUninvented.erase(it);
            }
            else
            {
                it++;
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
        for (ObjectEntryIndex i = 0; i < MAX_RIDE_OBJECTS; i++)
        {
            const rct_ride_entry* rideEntry = get_ride_entry(i);

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
            const rct_scenery_group_entry* groupEntry = get_scenery_group_entry(i);

            if (groupEntry != nullptr)
                research_insert_scenery_group_entry(i, true);
        }
    }
}

void research_items_make_all_unresearched()
{
    gResearchItemsUninvented.insert(
        gResearchItemsUninvented.end(), std::make_move_iterator(gResearchItemsInvented.begin()),
        std::make_move_iterator(gResearchItemsInvented.end()));
    gResearchItemsInvented.clear();
}

void research_items_make_all_researched()
{
    gResearchItemsInvented.insert(
        gResearchItemsInvented.end(), std::make_move_iterator(gResearchItemsUninvented.begin()),
        std::make_move_iterator(gResearchItemsUninvented.end()));
    gResearchItemsUninvented.clear();
}

/**
 *
 *  rct2: 0x00685A93
 */
void research_items_shuffle()
{
    std::shuffle(std::begin(gResearchItemsUninvented), std::end(gResearchItemsUninvented), std::default_random_engine{});
}

bool ResearchItem::IsAlwaysResearched() const
{
    return (flags & (RESEARCH_ENTRY_FLAG_RIDE_ALWAYS_RESEARCHED | RESEARCH_ENTRY_FLAG_SCENERY_SET_ALWAYS_RESEARCHED)) != 0;
}

bool ResearchItem::IsNull() const
{
    return entryIndex == OBJECT_ENTRY_INDEX_NULL;
}

void ResearchItem::SetNull()
{
    entryIndex = OBJECT_ENTRY_INDEX_NULL;
}

bool ResearchItem::Equals(const ResearchItem* otherItem) const
{
    return (entryIndex == otherItem->entryIndex && baseRideType == otherItem->baseRideType && type == otherItem->type);
}

bool ResearchItem::Exists() const
{
    for (auto const& researchItem : gResearchItemsUninvented)
    {
        if (researchItem.Equals(this))
        {
            return true;
        }
    }
    for (auto const& researchItem : gResearchItemsInvented)
    {
        if (researchItem.Equals(this))
        {
            return true;
        }
    }
    return false;
}

// clang-format off
static constexpr const rct_string_id _editorInventionsResearchCategories[] = {
    STR_RESEARCH_NEW_TRANSPORT_RIDES,
    STR_RESEARCH_NEW_GENTLE_RIDES,
    STR_RESEARCH_NEW_ROLLER_COASTERS,
    STR_RESEARCH_NEW_THRILL_RIDES,
    STR_RESEARCH_NEW_WATER_RIDES,
    STR_RESEARCH_NEW_SHOPS_AND_STALLS,
    STR_RESEARCH_NEW_SCENERY_AND_THEMING,
};
// clang-format on

rct_string_id ResearchItem::GetCategoryInventionString() const
{
    const auto categoryValue = EnumValue(category);
    Guard::Assert(categoryValue <= 6, "Unsupported category invention string");
    return _editorInventionsResearchCategories[categoryValue];
}

// clang-format off
static constexpr const rct_string_id _researchCategoryNames[] = {
    STR_RESEARCH_CATEGORY_TRANSPORT,
    STR_RESEARCH_CATEGORY_GENTLE,
    STR_RESEARCH_CATEGORY_ROLLERCOASTER,
    STR_RESEARCH_CATEGORY_THRILL,
    STR_RESEARCH_CATEGORY_WATER,
    STR_RESEARCH_CATEGORY_SHOP,
    STR_RESEARCH_CATEGORY_SCENERY_GROUP,
};
// clang-format on

rct_string_id ResearchItem::GetCategoryName() const
{
    const auto categoryValue = EnumValue(category);
    Guard::Assert(categoryValue <= 6, "Unsupported category name");
    return _researchCategoryNames[categoryValue];
}

static std::bitset<RIDE_TYPE_COUNT> _seenRideType = {};

static void research_update_first_of_type(ResearchItem* researchItem)
{
    if (researchItem->IsNull())
        return;

    if (researchItem->type != Research::EntryType::Ride)
        return;

    auto rideType = researchItem->baseRideType;
    if (rideType >= RIDE_TYPE_COUNT)
    {
        log_error("Research item has non-existent ride type index %d", rideType);
        return;
    }

    const auto& rtd = GetRideTypeDescriptor(rideType);
    if (rtd.HasFlag(RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY))
    {
        researchItem->flags |= RESEARCH_ENTRY_FLAG_FIRST_OF_TYPE;
        return;
    }

    if (!_seenRideType[rideType])
        researchItem->flags |= RESEARCH_ENTRY_FLAG_FIRST_OF_TYPE;
}

static void research_mark_ride_type_as_seen(const ResearchItem& researchItem)
{
    auto rideType = researchItem.baseRideType;
    if (rideType >= RIDE_TYPE_COUNT)
        return;

    _seenRideType[rideType] = true;
}

void research_determine_first_of_type()
{
    _seenRideType.reset();

    for (const auto& researchItem : gResearchItemsInvented)
    {
        if (researchItem.type != Research::EntryType::Ride)
            continue;

        auto rideType = researchItem.baseRideType;
        if (rideType >= RIDE_TYPE_COUNT)
            continue;

        const auto& rtd = GetRideTypeDescriptor(rideType);
        if (rtd.HasFlag(RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY))
            continue;

        // The last research item will also be present in gResearchItemsInvented.
        // Avoid marking its ride type as "invented" prematurely.
        if (gResearchLastItem.has_value() && !gResearchLastItem->IsNull() && researchItem.Equals(&gResearchLastItem.value()))
            continue;

        // The next research item is (sometimes?) also present in gResearchItemsInvented, even though it isn't invented yet(!)
        if (gResearchNextItem.has_value() && !gResearchNextItem->IsNull() && researchItem.Equals(&gResearchNextItem.value()))
            continue;

        research_mark_ride_type_as_seen(researchItem);
    }

    if (gResearchLastItem.has_value())
    {
        research_update_first_of_type(&gResearchLastItem.value());
        research_mark_ride_type_as_seen(gResearchLastItem.value());
    }
    if (gResearchNextItem.has_value())
    {
        research_update_first_of_type(&gResearchNextItem.value());
        research_mark_ride_type_as_seen(gResearchNextItem.value());
    }

    for (auto& researchItem : gResearchItemsUninvented)
    {
        // The next research item is (sometimes?) also present in gResearchItemsUninvented
        if (gResearchNextItem.has_value() && !gResearchNextItem->IsNull() && researchItem.Equals(&gResearchNextItem.value()))
        {
            // Copy the "first of type" flag.
            researchItem.flags = gResearchNextItem->flags;
            continue;
        }

        research_update_first_of_type(&researchItem);
        research_mark_ride_type_as_seen(researchItem);
    }
}
