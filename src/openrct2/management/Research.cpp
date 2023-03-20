/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Research.h"

#include "../Date.h"
#include "../Game.h"
#include "../OpenRCT2.h"
#include "../actions/ParkSetResearchFundingAction.h"
#include "../config/Config.h"
#include "../core/BitSet.hpp"
#include "../core/Guard.hpp"
#include "../core/Memory.hpp"
#include "../interface/Window.h"
#include "../localisation/Date.h"
#include "../localisation/Formatter.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../object/ObjectEntryManager.h"
#include "../object/ObjectList.h"
#include "../object/RideObject.h"
#include "../object/SceneryGroupEntry.h"
#include "../profiling/Profiling.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/RideEntry.h"
#include "../ride/TrackData.h"
#include "../scenario/Scenario.h"
#include "../util/Util.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "Finance.h"
#include "NewsItem.h"

#include <algorithm>
#include <iterator>

using namespace OpenRCT2;

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
void ResearchResetItems()
{
    gResearchItemsUninvented.clear();
    gResearchItemsInvented.clear();
}

/**
 *
 *  rct2: 0x00684BAE
 */
void ResearchUpdateUncompletedTypes()
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
static void ResearchCalculateExpectedDate()
{
    if (gResearchProgressStage == RESEARCH_STAGE_INITIAL_RESEARCH || gResearchFundingLevel == RESEARCH_FUNDING_NONE)
    {
        gResearchExpectedDay = 255;
    }
    else
    {
        auto& date = GetDate();

        int32_t progressRemaining = gResearchProgressStage == RESEARCH_STAGE_COMPLETING_DESIGN ? 0x10000 : 0x20000;
        progressRemaining -= gResearchProgress;
        int32_t daysRemaining = (progressRemaining / _researchRate[gResearchFundingLevel]) * 128;

        int32_t expectedDay = date.GetMonthTicks() + (daysRemaining & 0xFFFF);
        int32_t dayQuotient = expectedDay / 0x10000;
        int32_t dayRemainder = expectedDay % 0x10000;

        int32_t expectedMonth = DateGetMonth(date.GetMonthsElapsed() + dayQuotient + (daysRemaining >> 16));
        expectedDay = (dayRemainder * Date::GetDaysInMonth(expectedMonth)) >> 16;

        gResearchExpectedDay = expectedDay;
        gResearchExpectedMonth = expectedMonth;
    }
}

static void ResearchInvalidateRelatedWindows()
{
    WindowInvalidateByClass(WindowClass::ConstructRide);
    WindowInvalidateByClass(WindowClass::Research);
}

static void ResearchMarkAsFullyCompleted()
{
    gResearchProgress = 0;
    gResearchProgressStage = RESEARCH_STAGE_FINISHED_ALL;
    ResearchInvalidateRelatedWindows();
    // Reset funding to 0 if no more rides.
    auto gameAction = ParkSetResearchFundingAction(gResearchPriorities, 0);
    GameActions::Execute(&gameAction);
}

/**
 *
 *  rct2: 0x00684BE5
 */
static void ResearchNextDesign()
{
    if (gResearchItemsUninvented.empty())
    {
        ResearchMarkAsFullyCompleted();
        return;
    }

    // Try to find a research item of a matching type, if none found, use any first item
    auto it = std::find_if(gResearchItemsUninvented.begin(), gResearchItemsUninvented.end(), [](const auto& e) {
        return (gResearchPriorities & EnumToFlag(e.category)) != 0;
    });
    if (it == gResearchItemsUninvented.end())
    {
        it = gResearchItemsUninvented.begin();
    }

    const ResearchItem researchItem = *it;
    gResearchNextItem = researchItem;
    gResearchProgress = 0;
    gResearchProgressStage = RESEARCH_STAGE_DESIGNING;

    gResearchItemsUninvented.erase(it);
    gResearchItemsInvented.push_back(researchItem);

    ResearchInvalidateRelatedWindows();
}

/**
 *
 *  rct2: 0x006848D4
 */
void ResearchFinishItem(const ResearchItem& researchItem)
{
    gResearchLastItem = researchItem;
    ResearchInvalidateRelatedWindows();

    if (researchItem.type == Research::EntryType::Ride)
    {
        // Ride
        auto base_ride_type = researchItem.baseRideType;
        ObjectEntryIndex rideEntryIndex = researchItem.entryIndex;
        const auto* rideEntry = GetRideEntryByIndex(rideEntryIndex);

        if (rideEntry != nullptr && base_ride_type != RIDE_TYPE_NULL)
        {
            if (!RideTypeIsValid(base_ride_type))
            {
                LOG_WARNING("Invalid ride type: %d", base_ride_type);
                base_ride_type = rideEntry->GetFirstNonNullRideType();
            }

            StringId availabilityString;
            RideTypeSetInvented(base_ride_type);
            RideEntrySetInvented(rideEntryIndex);

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
                    const auto* rideEntry2 = GetRideEntryByIndex(i);
                    if (rideEntry2 != nullptr)
                    {
                        for (uint8_t j = 0; j < RCT2::ObjectLimits::MaxRideTypesPerRideEntry; j++)
                        {
                            if (rideEntry2->ride_type[j] == base_ride_type)
                            {
                                RideEntrySetInvented(i);
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
                || researchItem.flags & RESEARCH_ENTRY_FLAG_FIRST_OF_TYPE)
            {
                RideNaming naming = GetRideNaming(base_ride_type, *rideEntry);
                availabilityString = STR_NEWS_ITEM_RESEARCH_NEW_RIDE_AVAILABLE;
                ft.Add<StringId>(naming.Name);
            }
            // If the vehicle should not be listed separately and it isn't the first to be invented for its ride group,
            // report it as a new vehicle for the existing ride group.
            else
            {
                availabilityString = STR_NEWS_ITEM_RESEARCH_NEW_VEHICLE_AVAILABLE;
                RideNaming baseRideNaming = GetRideNaming(base_ride_type, *rideEntry);

                ft.Add<StringId>(baseRideNaming.Name);
                ft.Add<StringId>(rideEntry->naming.Name);
            }

            if (!gSilentResearch)
            {
                if (gConfigNotifications.RideResearched)
                {
                    News::AddItemToQueue(News::ItemType::Research, availabilityString, researchItem.rawValue, ft);
                }
            }

            ResearchInvalidateRelatedWindows();
        }
    }
    else
    {
        // Scenery
        const auto* sceneryGroupEntry = OpenRCT2::ObjectManager::GetObjectEntry<SceneryGroupEntry>(researchItem.entryIndex);
        if (sceneryGroupEntry != nullptr)
        {
            SceneryGroupSetInvented(researchItem.entryIndex);

            Formatter ft;
            ft.Add<StringId>(sceneryGroupEntry->name);

            if (!gSilentResearch)
            {
                if (gConfigNotifications.RideResearched)
                {
                    News::AddItemToQueue(
                        News::ItemType::Research, STR_NEWS_ITEM_RESEARCH_NEW_SCENERY_SET_AVAILABLE, researchItem.rawValue, ft);
                }
            }

            ResearchInvalidateRelatedWindows();
            SceneryInit();
        }
    }
}

/**
 *
 *  rct2: 0x00684C7A
 */
void ResearchUpdate()
{
    PROFILED_FUNCTION();

    int32_t editorScreenFlags, researchLevel, currentResearchProgress;

    editorScreenFlags = SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER;
    if (gScreenFlags & editorScreenFlags)
    {
        return;
    }

    if (gCurrentTicks % 32 != 0)
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
                ResearchNextDesign();
                ResearchCalculateExpectedDate();
                break;
            case RESEARCH_STAGE_DESIGNING:
                gResearchProgress = 0;
                gResearchProgressStage = RESEARCH_STAGE_COMPLETING_DESIGN;
                ResearchCalculateExpectedDate();
                ResearchInvalidateRelatedWindows();
                break;
            case RESEARCH_STAGE_COMPLETING_DESIGN:
                ResearchFinishItem(*gResearchNextItem);
                gResearchProgress = 0;
                gResearchProgressStage = RESEARCH_STAGE_INITIAL_RESEARCH;
                ResearchCalculateExpectedDate();
                ResearchUpdateUncompletedTypes();
                ResearchInvalidateRelatedWindows();
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
void ResearchResetCurrentItem()
{
    SetEveryRideTypeNotInvented();
    SetEveryRideEntryNotInvented();

    // The following two instructions together make all items not tied to a scenery group available.
    SetAllSceneryItemsInvented();
    SetAllSceneryGroupsNotInvented();

    for (const auto& researchItem : gResearchItemsInvented)
    {
        ResearchFinishItem(researchItem);
    }

    gResearchLastItem = std::nullopt;
    gResearchProgressStage = RESEARCH_STAGE_INITIAL_RESEARCH;
    gResearchProgress = 0;
}

/**
 *
 *  rct2: 0x006857FA
 */
static void ResearchInsertUnresearched(ResearchItem&& item)
{
    // First check to make sure that entry is not already accounted for
    if (item.Exists())
    {
        return;
    }

    gResearchItemsUninvented.push_back(std::move(item));
}

/**
 *
 *  rct2: 0x00685826
 */
static void ResearchInsertResearched(ResearchItem&& item)
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
void ResearchRemove(const ResearchItem& researchItem)
{
    gResearchItemsUninvented.erase(
        std::remove(gResearchItemsUninvented.begin(), gResearchItemsUninvented.end(), researchItem),
        gResearchItemsUninvented.end());
    gResearchItemsInvented.erase(
        std::remove(gResearchItemsInvented.begin(), gResearchItemsInvented.end(), researchItem), gResearchItemsInvented.end());
}

void ResearchInsert(ResearchItem&& item, bool researched)
{
    if (researched)
    {
        ResearchInsertResearched(std::move(item));
    }
    else
    {
        ResearchInsertUnresearched(std::move(item));
    }
}

/**
 *
 *  rct2: 0x00685675
 */
void ResearchPopulateListRandom()
{
    ResearchResetItems();

    // Rides
    for (int32_t i = 0; i < MAX_RIDE_OBJECTS; i++)
    {
        const auto* rideEntry = GetRideEntryByIndex(i);
        if (rideEntry == nullptr)
        {
            continue;
        }

        int32_t researched = (ScenarioRand() & 0xFF) > 128;
        for (auto rideType : rideEntry->ride_type)
        {
            if (rideType != RIDE_TYPE_NULL)
            {
                ResearchCategory category = GetRideTypeDescriptor(rideType).GetResearchCategory();
                ResearchInsertRideEntry(rideType, i, category, researched);
            }
        }
    }

    // Scenery
    for (uint32_t i = 0; i < MAX_SCENERY_GROUP_OBJECTS; i++)
    {
        const auto* sceneryGroupEntry = OpenRCT2::ObjectManager::GetObjectEntry<SceneryGroupEntry>(i);
        if (sceneryGroupEntry == nullptr)
        {
            continue;
        }

        int32_t researched = (ScenarioRand() & 0xFF) > 85;
        ResearchInsertSceneryGroupEntry(i, researched);
    }
}

bool ResearchInsertRideEntry(ride_type_t rideType, ObjectEntryIndex entryIndex, ResearchCategory category, bool researched)
{
    if (rideType != RIDE_TYPE_NULL && entryIndex != OBJECT_ENTRY_INDEX_NULL)
    {
        auto tmpItem = ResearchItem(Research::EntryType::Ride, entryIndex, rideType, category, 0);
        ResearchInsert(std::move(tmpItem), researched);
        return true;
    }

    return false;
}

void ResearchInsertRideEntry(ObjectEntryIndex entryIndex, bool researched)
{
    const auto* rideEntry = GetRideEntryByIndex(entryIndex);
    if (rideEntry == nullptr)
        return;

    for (auto rideType : rideEntry->ride_type)
    {
        if (rideType != RIDE_TYPE_NULL)
        {
            ResearchCategory category = GetRideTypeDescriptor(rideType).GetResearchCategory();
            ResearchInsertRideEntry(rideType, entryIndex, category, researched);
        }
    }
}

bool ResearchInsertSceneryGroupEntry(ObjectEntryIndex entryIndex, bool researched)
{
    if (entryIndex != OBJECT_ENTRY_INDEX_NULL)
    {
        auto tmpItem = ResearchItem(Research::EntryType::Scenery, entryIndex, 0, ResearchCategory::SceneryGroup, 0);
        ResearchInsert(std::move(tmpItem), researched);
        return true;
    }
    return false;
}

bool RideTypeIsInvented(uint32_t rideType)
{
    return RideTypeIsValid(rideType) ? _researchedRideTypes[rideType] : false;
}

bool RideEntryIsInvented(ObjectEntryIndex rideEntryIndex)
{
    if (rideEntryIndex >= std::size(_researchedRideEntries))
        return false;

    return _researchedRideEntries[rideEntryIndex];
}

void RideTypeSetInvented(uint32_t rideType)
{
    if (RideTypeIsValid(rideType))
    {
        _researchedRideTypes[rideType] = true;
    }
}

void RideEntrySetInvented(ObjectEntryIndex rideEntryIndex)
{
    if (rideEntryIndex >= std::size(_researchedRideEntries))
        LOG_ERROR("Tried setting ride entry %u as invented", rideEntryIndex);
    else
        _researchedRideEntries[rideEntryIndex] = true;
}

bool SceneryIsInvented(const ScenerySelection& sceneryItem)
{
    if (sceneryItem.SceneryType < SCENERY_TYPE_COUNT)
    {
        return _researchedSceneryItems[sceneryItem.SceneryType][sceneryItem.EntryIndex];
    }

    LOG_WARNING("Invalid Scenery Type");
    return false;
}

void ScenerySetInvented(const ScenerySelection& sceneryItem)
{
    if (sceneryItem.SceneryType < SCENERY_TYPE_COUNT)
    {
        _researchedSceneryItems[sceneryItem.SceneryType][sceneryItem.EntryIndex] = true;
    }
    else
    {
        LOG_WARNING("Invalid Scenery Type");
    }
}

void ScenerySetNotInvented(const ScenerySelection& sceneryItem)
{
    if (sceneryItem.SceneryType < SCENERY_TYPE_COUNT)
    {
        _researchedSceneryItems[sceneryItem.SceneryType][sceneryItem.EntryIndex] = false;
    }
    else
    {
        LOG_WARNING("Invalid Scenery Type");
    }
}

bool SceneryGroupIsInvented(int32_t sgIndex)
{
    const auto sgEntry = OpenRCT2::ObjectManager::GetObjectEntry<SceneryGroupEntry>(sgIndex);
    if (sgEntry == nullptr || sgEntry->SceneryEntries.empty())
    {
        return false;
    }

    // All scenery is temporarily invented when in the scenario editor
    if (gScreenFlags & SCREEN_FLAGS_EDITOR)
    {
        return true;
    }

    if (gCheatsIgnoreResearchStatus)
    {
        return true;
    }

    return std::none_of(
        std::begin(gResearchItemsUninvented), std::end(gResearchItemsUninvented), [sgIndex](const ResearchItem& item) {
            return item.type == Research::EntryType::Scenery && item.entryIndex == sgIndex;
        });
}

void SceneryGroupSetInvented(int32_t sgIndex)
{
    const auto sgEntry = OpenRCT2::ObjectManager::GetObjectEntry<SceneryGroupEntry>(sgIndex);
    if (sgEntry != nullptr)
    {
        for (const auto& entry : sgEntry->SceneryEntries)
        {
            ScenerySetInvented(entry);
        }
    }
}

void SetAllSceneryGroupsNotInvented()
{
    for (int32_t i = 0; i < MAX_SCENERY_GROUP_OBJECTS; ++i)
    {
        const auto* scenery_set = OpenRCT2::ObjectManager::GetObjectEntry<SceneryGroupEntry>(i);
        if (scenery_set == nullptr)
        {
            continue;
        }

        for (const auto& sceneryEntry : scenery_set->SceneryEntries)
        {
            ScenerySetNotInvented(sceneryEntry);
        }
    }
}

void SetAllSceneryItemsInvented()
{
    for (auto sceneryType = 0; sceneryType < SCENERY_TYPE_COUNT; sceneryType++)
    {
        std::fill(std::begin(_researchedSceneryItems[sceneryType]), std::end(_researchedSceneryItems[sceneryType]), true);
    }
}

void SetAllSceneryItemsNotInvented()
{
    for (auto sceneryType = 0; sceneryType < SCENERY_TYPE_COUNT; sceneryType++)
    {
        std::fill(std::begin(_researchedSceneryItems[sceneryType]), std::end(_researchedSceneryItems[sceneryType]), false);
    }
}

void SetEveryRideTypeInvented()
{
    std::fill(std::begin(_researchedRideTypes), std::end(_researchedRideTypes), true);
}

void SetEveryRideTypeNotInvented()
{
    std::fill(std::begin(_researchedRideTypes), std::end(_researchedRideTypes), false);
}

void SetEveryRideEntryInvented()
{
    std::fill(std::begin(_researchedRideEntries), std::end(_researchedRideEntries), true);
}

void SetEveryRideEntryNotInvented()
{
    std::fill(std::begin(_researchedRideEntries), std::end(_researchedRideEntries), false);
}

/**
 *
 *  rct2: 0x0068563D
 */
StringId ResearchItem::GetName() const
{
    if (type == Research::EntryType::Ride)
    {
        const auto* rideEntry = GetRideEntryByIndex(entryIndex);
        if (rideEntry == nullptr)
        {
            return STR_EMPTY;
        }

        return rideEntry->naming.Name;
    }

    const auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<SceneryGroupEntry>(entryIndex);
    if (sceneryEntry == nullptr)
    {
        return STR_EMPTY;
    }

    return sceneryEntry->name;
}

/**
 *
 *  rct2: 0x00685A79
 *  Do not use the research list outside of the inventions list window with the flags
 *  Clears flags like "always researched".
 */
void ResearchRemoveFlags()
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

static void ResearchRemoveNullItems(std::vector<ResearchItem>& items)
{
    const auto it = std::remove_if(std::begin(items), std::end(items), [](const ResearchItem& researchItem) {
        if (researchItem.type == Research::EntryType::Ride)
        {
            return GetRideEntryByIndex(researchItem.entryIndex) == nullptr;
        }
        else
        {
            return OpenRCT2::ObjectManager::GetObjectEntry<SceneryGroupEntry>(researchItem.entryIndex) == nullptr;
        }
    });
    items.erase(it, std::end(items));
}

static void ResearchMarkItemAsResearched(const ResearchItem& item)
{
    if (item.type == Research::EntryType::Ride)
    {
        const auto* rideEntry = GetRideEntryByIndex(item.entryIndex);
        if (rideEntry != nullptr)
        {
            RideEntrySetInvented(item.entryIndex);
            for (auto rideType : rideEntry->ride_type)
            {
                if (rideType != RIDE_TYPE_NULL)
                {
                    RideTypeSetInvented(rideType);
                }
            }
        }
    }
    else if (item.type == Research::EntryType::Scenery)
    {
        const auto sgEntry = OpenRCT2::ObjectManager::GetObjectEntry<SceneryGroupEntry>(item.entryIndex);
        if (sgEntry != nullptr)
        {
            for (const auto& sceneryEntry : sgEntry->SceneryEntries)
            {
                ScenerySetInvented(sceneryEntry);
            }
        }
    }
}

static void ResearchRebuildInventedTables()
{
    SetEveryRideTypeNotInvented();
    SetEveryRideEntryInvented();
    SetEveryRideEntryNotInvented();
    SetAllSceneryItemsNotInvented();
    for (const auto& item : gResearchItemsInvented)
    {
        // Ignore item, if the research of it is in progress
        if (gResearchProgressStage == RESEARCH_STAGE_DESIGNING || gResearchProgressStage == RESEARCH_STAGE_COMPLETING_DESIGN)
        {
            if (item == gResearchNextItem)
            {
                continue;
            }
        }

        ResearchMarkItemAsResearched(item);
    }
    MarkAllUnrestrictedSceneryAsInvented();
}

static void ResearchAddAllMissingItems(bool isResearched)
{
    for (ObjectEntryIndex i = 0; i < MAX_RIDE_OBJECTS; i++)
    {
        const auto* rideEntry = GetRideEntryByIndex(i);
        if (rideEntry != nullptr)
        {
            ResearchInsertRideEntry(i, isResearched);
        }
    }

    for (ObjectEntryIndex i = 0; i < MAX_SCENERY_GROUP_OBJECTS; i++)
    {
        const auto* groupEntry = OpenRCT2::ObjectManager::GetObjectEntry<SceneryGroupEntry>(i);
        if (groupEntry != nullptr)
        {
            ResearchInsertSceneryGroupEntry(i, isResearched);
        }
    }
}

void ResearchFix()
{
    // Remove null entries from the research list
    ResearchRemoveNullItems(gResearchItemsInvented);
    ResearchRemoveNullItems(gResearchItemsUninvented);

    // Add missing entries to the research list
    // If research is complete, mark all the missing items as available
    ResearchAddAllMissingItems(gResearchProgressStage == RESEARCH_STAGE_FINISHED_ALL);

    // Now rebuild all the tables that say whether a ride or scenery item is invented
    ResearchRebuildInventedTables();
    ResearchUpdateUncompletedTypes();
}

void ResearchItemsMakeAllUnresearched()
{
    gResearchItemsUninvented.insert(
        gResearchItemsUninvented.end(), std::make_move_iterator(gResearchItemsInvented.begin()),
        std::make_move_iterator(gResearchItemsInvented.end()));
    gResearchItemsInvented.clear();
}

void ResearchItemsMakeAllResearched()
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
void ResearchItemsShuffle()
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

bool ResearchItem::Exists() const
{
    for (auto const& researchItem : gResearchItemsUninvented)
    {
        if (researchItem == *this)
        {
            return true;
        }
    }
    for (auto const& researchItem : gResearchItemsInvented)
    {
        if (researchItem == *this)
        {
            return true;
        }
    }
    return false;
}

// clang-format off
static constexpr const StringId _editorInventionsResearchCategories[] = {
    STR_RESEARCH_NEW_TRANSPORT_RIDES,
    STR_RESEARCH_NEW_GENTLE_RIDES,
    STR_RESEARCH_NEW_ROLLER_COASTERS,
    STR_RESEARCH_NEW_THRILL_RIDES,
    STR_RESEARCH_NEW_WATER_RIDES,
    STR_RESEARCH_NEW_SHOPS_AND_STALLS,
    STR_RESEARCH_NEW_SCENERY_AND_THEMING,
};
// clang-format on

StringId ResearchItem::GetCategoryInventionString() const
{
    const auto categoryValue = EnumValue(category);
    Guard::Assert(categoryValue <= 6, "Unsupported category invention string");
    return _editorInventionsResearchCategories[categoryValue];
}

// clang-format off
static constexpr const StringId _researchCategoryNames[] = {
    STR_RESEARCH_CATEGORY_TRANSPORT,
    STR_RESEARCH_CATEGORY_GENTLE,
    STR_RESEARCH_CATEGORY_ROLLERCOASTER,
    STR_RESEARCH_CATEGORY_THRILL,
    STR_RESEARCH_CATEGORY_WATER,
    STR_RESEARCH_CATEGORY_SHOP,
    STR_RESEARCH_CATEGORY_SCENERY_GROUP,
};
// clang-format on

StringId ResearchItem::GetCategoryName() const
{
    const auto categoryValue = EnumValue(category);
    Guard::Assert(categoryValue <= 6, "Unsupported category name");
    return _researchCategoryNames[categoryValue];
}

bool ResearchItem::operator==(const ResearchItem& rhs) const
{
    return (entryIndex == rhs.entryIndex && baseRideType == rhs.baseRideType && type == rhs.type);
}

static BitSet<RIDE_TYPE_COUNT> _seenRideType = {};

static void ResearchUpdateFirstOfType(ResearchItem* researchItem)
{
    if (researchItem->IsNull())
        return;

    if (researchItem->type != Research::EntryType::Ride)
        return;

    auto rideType = researchItem->baseRideType;
    if (rideType >= RIDE_TYPE_COUNT)
    {
        LOG_ERROR("Research item has non-existent ride type index %d", rideType);
        return;
    }

    researchItem->flags &= ~RESEARCH_ENTRY_FLAG_FIRST_OF_TYPE;
    const auto& rtd = GetRideTypeDescriptor(rideType);
    if (rtd.HasFlag(RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY))
    {
        researchItem->flags |= RESEARCH_ENTRY_FLAG_FIRST_OF_TYPE;
        return;
    }

    if (!_seenRideType[rideType])
        researchItem->flags |= RESEARCH_ENTRY_FLAG_FIRST_OF_TYPE;
}

static void ResearchMarkRideTypeAsSeen(const ResearchItem& researchItem)
{
    auto rideType = researchItem.baseRideType;
    if (rideType >= RIDE_TYPE_COUNT)
        return;

    _seenRideType[rideType] = true;
}

void ResearchDetermineFirstOfType()
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
        if (gResearchLastItem.has_value() && !gResearchLastItem->IsNull() && researchItem == gResearchLastItem.value())
            continue;

        // The next research item is (sometimes?) also present in gResearchItemsInvented, even though it isn't invented yet(!)
        if (gResearchNextItem.has_value() && !gResearchNextItem->IsNull() && researchItem == gResearchNextItem.value())
            continue;

        ResearchMarkRideTypeAsSeen(researchItem);
    }

    if (gResearchLastItem.has_value())
    {
        ResearchUpdateFirstOfType(&gResearchLastItem.value());
        ResearchMarkRideTypeAsSeen(gResearchLastItem.value());
    }
    if (gResearchNextItem.has_value())
    {
        ResearchUpdateFirstOfType(&gResearchNextItem.value());
        ResearchMarkRideTypeAsSeen(gResearchNextItem.value());
    }

    for (auto& researchItem : gResearchItemsUninvented)
    {
        // The next research item is (sometimes?) also present in gResearchItemsUninvented
        if (gResearchNextItem.has_value() && !gResearchNextItem->IsNull()
            && researchItem.baseRideType == gResearchNextItem.value().baseRideType)
        {
            // Copy the "first of type" flag.
            researchItem.flags = gResearchNextItem->flags;
            continue;
        }

        ResearchUpdateFirstOfType(&researchItem);
        ResearchMarkRideTypeAsSeen(researchItem);
    }
}
