/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Research.h"

#include "../Context.h"
#include "../Date.h"
#include "../Diagnostic.h"
#include "../Game.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../actions/ParkSetResearchFundingAction.h"
#include "../config/Config.h"
#include "../core/BitSet.hpp"
#include "../core/EnumUtils.hpp"
#include "../core/Guard.hpp"
#include "../core/Memory.hpp"
#include "../localisation/Formatter.h"
#include "../localisation/Localisation.Date.h"
#include "../localisation/StringIds.h"
#include "../object/ObjectEntryManager.h"
#include "../object/ObjectLimits.h"
#include "../object/ObjectList.h"
#include "../object/RideObject.h"
#include "../object/SceneryGroupEntry.h"
#include "../profiling/Profiling.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/RideEntry.h"
#include "../ride/TrackData.h"
#include "../scenario/Scenario.h"
#include "../ui/WindowManager.h"
#include "../windows/Intent.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "Finance.h"
#include "NewsItem.h"

#include <iterator>

using namespace OpenRCT2;

static constexpr int32_t _researchRate[] = {
    0,
    160,
    250,
    400,
};

static bool _researchedRideTypes[RIDE_TYPE_COUNT];
static bool _researchedRideEntries[kMaxRideObjects];
static bool _researchedSceneryItems[SCENERY_TYPE_COUNT][UINT16_MAX];

bool gSilentResearch = false;

// clang-format off
const StringId kResearchFundingLevelNames[] = {
    STR_RESEARCH_FUNDING_NONE,
    STR_RESEARCH_FUNDING_MINIMUM,
    STR_RESEARCH_FUNDING_NORMAL,
    STR_RESEARCH_FUNDING_MAXIMUM,
};
// clang-format on

/**
 *
 *  rct2: 0x006671AD, part of 0x00667132
 */
void ResearchResetItems(GameState_t& gameState)
{
    gameState.ResearchItemsUninvented.clear();
    gameState.ResearchItemsInvented.clear();
}

/**
 *
 *  rct2: 0x00684BAE
 */
void ResearchUpdateUncompletedTypes()
{
    auto& gameState = GetGameState();
    int32_t uncompletedResearchTypes = 0;

    for (auto const& researchItem : gameState.ResearchItemsUninvented)
    {
        uncompletedResearchTypes |= EnumToFlag(researchItem.category);
    }

    gameState.ResearchUncompletedCategories = uncompletedResearchTypes;
}

/**
 *
 *  rct2: 0x00684D2A
 */
static void ResearchCalculateExpectedDate()
{
    auto& gameState = GetGameState();
    if (gameState.ResearchProgressStage == RESEARCH_STAGE_INITIAL_RESEARCH
        || gameState.ResearchFundingLevel == RESEARCH_FUNDING_NONE)
    {
        gameState.ResearchExpectedDay = 255;
    }
    else
    {
        auto& date = GetDate();

        int32_t progressRemaining = gameState.ResearchProgressStage == RESEARCH_STAGE_COMPLETING_DESIGN ? 0x10000 : 0x20000;
        progressRemaining -= gameState.ResearchProgress;
        int32_t daysRemaining = (progressRemaining / _researchRate[gameState.ResearchFundingLevel]) * 128;

        int32_t expectedDay = date.GetMonthTicks() + (daysRemaining & 0xFFFF);
        int32_t dayQuotient = expectedDay / 0x10000;
        int32_t dayRemainder = expectedDay % 0x10000;

        int32_t expectedMonth = DateGetMonth(date.GetMonthsElapsed() + dayQuotient + (daysRemaining >> 16));
        expectedDay = (dayRemainder * Date::GetDaysInMonth(expectedMonth)) >> 16;

        gameState.ResearchExpectedDay = expectedDay;
        gameState.ResearchExpectedMonth = expectedMonth;
    }
}

static void ResearchInvalidateRelatedWindows()
{
    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateByClass(WindowClass::ConstructRide);
    windowMgr->InvalidateByClass(WindowClass::Research);
}

static void ResearchMarkAsFullyCompleted()
{
    auto& gameState = GetGameState();
    gameState.ResearchProgress = 0;
    gameState.ResearchProgressStage = RESEARCH_STAGE_FINISHED_ALL;
    ResearchInvalidateRelatedWindows();
    // Reset funding to 0 if no more rides.
    auto gameAction = ParkSetResearchFundingAction(gameState.ResearchPriorities, 0);
    GameActions::Execute(&gameAction);
}

/**
 *
 *  rct2: 0x00684BE5
 */
static void ResearchNextDesign()
{
    auto& gameState = GetGameState();
    if (gameState.ResearchItemsUninvented.empty())
    {
        ResearchMarkAsFullyCompleted();
        return;
    }

    // Try to find a research item of a matching type, if none found, use any first item
    auto it = std::find_if(
        gameState.ResearchItemsUninvented.begin(), gameState.ResearchItemsUninvented.end(),
        [&gameState](const auto& e) { return (gameState.ResearchPriorities & EnumToFlag(e.category)) != 0; });
    if (it == gameState.ResearchItemsUninvented.end())
    {
        it = gameState.ResearchItemsUninvented.begin();
    }

    gameState.ResearchNextItem = *it;
    gameState.ResearchProgress = 0;
    gameState.ResearchProgressStage = RESEARCH_STAGE_DESIGNING;

    ResearchInvalidateRelatedWindows();
}

static void MarkResearchItemInvented(const ResearchItem& researchItem)
{
    auto& gameState = GetGameState();
    gameState.ResearchItemsUninvented.erase(
        std::remove(gameState.ResearchItemsUninvented.begin(), gameState.ResearchItemsUninvented.end(), researchItem),
        gameState.ResearchItemsUninvented.end());

    if (std::find(gameState.ResearchItemsInvented.begin(), gameState.ResearchItemsInvented.end(), researchItem)
        == gameState.ResearchItemsInvented.end())
    {
        gameState.ResearchItemsInvented.push_back(researchItem);
    }
}

/**
 *
 *  rct2: 0x006848D4
 */
void ResearchFinishItem(const ResearchItem& researchItem)
{
    auto& gameState = GetGameState();
    gameState.ResearchLastItem = researchItem;
    ResearchInvalidateRelatedWindows();

    if (researchItem.type == Research::EntryType::Ride)
    {
        // Ride
        auto base_ride_type = researchItem.baseRideType;
        ObjectEntryIndex rideEntryIndex = researchItem.entryIndex;
        const auto* rideEntry = GetRideEntryByIndex(rideEntryIndex);

        if (rideEntry != nullptr && base_ride_type != kRideTypeNull)
        {
            if (!RideTypeIsValid(base_ride_type))
            {
                LOG_WARNING("Invalid ride type: %d", base_ride_type);
                base_ride_type = rideEntry->GetFirstNonNullRideType();
            }

            StringId availabilityString;
            RideTypeSetInvented(base_ride_type);
            RideEntrySetInvented(rideEntryIndex);

            bool seenRideEntry[kMaxRideObjects]{};
            for (auto const& researchItem3 : gameState.ResearchItemsUninvented)
            {
                ObjectEntryIndex index = researchItem3.entryIndex;
                seenRideEntry[index] = true;
            }

            // RCT2 made non-separated vehicles available at once, by removing all but one from research.
            // To ensure old files keep working, look for ride entries not in research, and make them available as well.
            for (int32_t i = 0; i < kMaxRideObjects; i++)
            {
                if (!seenRideEntry[i])
                {
                    const auto* rideEntry2 = GetRideEntryByIndex(i);
                    if (rideEntry2 != nullptr)
                    {
                        for (uint8_t j = 0; j < RCT2::ObjectLimits::kMaxRideTypesPerRideEntry; j++)
                        {
                            if (rideEntry2->ride_type[j] == base_ride_type)
                            {
                                RideEntrySetInvented(i);
                                ResearchInsertRideEntry(i, true);
                                break;
                            }
                        }
                    }
                }
            }

            Formatter ft;

            // If a vehicle is the first to be invented for its ride type, show the ride type/group name.
            // Independently listed vehicles (like all flat rides and shops) should always be announced as such.
            if (GetRideTypeDescriptor(base_ride_type).HasFlag(RtdFlag::listVehiclesSeparately)
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
                if (Config::Get().notifications.RideResearched)
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
                if (Config::Get().notifications.RideResearched)
                {
                    News::AddItemToQueue(
                        News::ItemType::Research, STR_NEWS_ITEM_RESEARCH_NEW_SCENERY_SET_AVAILABLE, researchItem.rawValue, ft);
                }
            }

            ResearchInvalidateRelatedWindows();

            auto intent = Intent(INTENT_ACTION_INIT_SCENERY);
            ContextBroadcastIntent(&intent);
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

    auto& gameState = GetGameState();
    if (gameState.CurrentTicks % 32 != 0)
    {
        return;
    }

    if ((gameState.Park.Flags & PARK_FLAGS_NO_MONEY) && gameState.ResearchFundingLevel == RESEARCH_FUNDING_NONE)
    {
        researchLevel = RESEARCH_FUNDING_NORMAL;
    }
    else
    {
        researchLevel = gameState.ResearchFundingLevel;
    }

    currentResearchProgress = gameState.ResearchProgress;
    currentResearchProgress += _researchRate[researchLevel];
    if (currentResearchProgress <= 0xFFFF)
    {
        gameState.ResearchProgress = currentResearchProgress;
    }
    else
    {
        switch (gameState.ResearchProgressStage)
        {
            case RESEARCH_STAGE_INITIAL_RESEARCH:
                ResearchNextDesign();
                ResearchCalculateExpectedDate();
                break;
            case RESEARCH_STAGE_DESIGNING:
                gameState.ResearchProgress = 0;
                gameState.ResearchProgressStage = RESEARCH_STAGE_COMPLETING_DESIGN;
                ResearchCalculateExpectedDate();
                ResearchInvalidateRelatedWindows();
                break;
            case RESEARCH_STAGE_COMPLETING_DESIGN:
                MarkResearchItemInvented(*gameState.ResearchNextItem);
                ResearchFinishItem(*gameState.ResearchNextItem);
                gameState.ResearchProgress = 0;
                gameState.ResearchProgressStage = RESEARCH_STAGE_INITIAL_RESEARCH;
                ResearchCalculateExpectedDate();
                ResearchUpdateUncompletedTypes();
                ResearchInvalidateRelatedWindows();
                break;
            case RESEARCH_STAGE_FINISHED_ALL:
                gameState.ResearchFundingLevel = RESEARCH_FUNDING_NONE;
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
    auto& gameState = GetGameState();
    SetEveryRideTypeNotInvented();
    SetEveryRideEntryNotInvented();

    // The following two instructions together make all items not tied to a scenery group available.
    SetAllSceneryItemsInvented();
    SetAllSceneryGroupsNotInvented();

    for (const auto& researchItem : gameState.ResearchItemsInvented)
    {
        ResearchFinishItem(researchItem);
    }

    gameState.ResearchLastItem = std::nullopt;
    gameState.ResearchProgressStage = RESEARCH_STAGE_INITIAL_RESEARCH;
    gameState.ResearchProgress = 0;
}

/**
 *
 *  rct2: 0x006857FA
 */
static void ResearchInsertUnresearched(ResearchItem&& item)
{
    auto& gameState = GetGameState();
    // First check to make sure that entry is not already accounted for
    if (item.Exists())
    {
        return;
    }

    gameState.ResearchItemsUninvented.push_back(std::move(item));
}

/**
 *
 *  rct2: 0x00685826
 */
static void ResearchInsertResearched(ResearchItem&& item)
{
    auto& gameState = GetGameState();
    // First check to make sure that entry is not already accounted for
    if (item.Exists())
    {
        return;
    }

    gameState.ResearchItemsInvented.push_back(std::move(item));
}

/**
 *
 *  rct2: 0x006857CF
 */
void ResearchRemove(const ResearchItem& researchItem)
{
    auto& gameState = GetGameState();
    gameState.ResearchItemsUninvented.erase(
        std::remove(gameState.ResearchItemsUninvented.begin(), gameState.ResearchItemsUninvented.end(), researchItem),
        gameState.ResearchItemsUninvented.end());
    gameState.ResearchItemsInvented.erase(
        std::remove(gameState.ResearchItemsInvented.begin(), gameState.ResearchItemsInvented.end(), researchItem),
        gameState.ResearchItemsInvented.end());
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
    auto& gameState = GetGameState();
    ResearchResetItems(gameState);

    // Rides
    for (int32_t i = 0; i < kMaxRideObjects; i++)
    {
        const auto* rideEntry = GetRideEntryByIndex(i);
        if (rideEntry == nullptr)
        {
            continue;
        }

        int32_t researched = (ScenarioRand() & 0xFF) > 128;
        for (auto rideType : rideEntry->ride_type)
        {
            if (rideType != kRideTypeNull)
            {
                ResearchCategory category = GetRideTypeDescriptor(rideType).GetResearchCategory();
                ResearchInsertRideEntry(rideType, i, category, researched);
            }
        }
    }

    // Scenery
    for (uint32_t i = 0; i < kMaxSceneryGroupObjects; i++)
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
    if (rideType != kRideTypeNull && entryIndex != kObjectEntryIndexNull)
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
        if (rideType != kRideTypeNull)
        {
            ResearchCategory category = GetRideTypeDescriptor(rideType).GetResearchCategory();
            ResearchInsertRideEntry(rideType, entryIndex, category, researched);
        }
    }
}

bool ResearchInsertSceneryGroupEntry(ObjectEntryIndex entryIndex, bool researched)
{
    if (entryIndex != kObjectEntryIndexNull)
    {
        auto tmpItem = ResearchItem(Research::EntryType::Scenery, entryIndex, 0, ResearchCategory::SceneryGroup, 0);
        ResearchInsert(std::move(tmpItem), researched);
        return true;
    }
    return false;
}

bool ResearchIsInvented(ObjectType objectType, ObjectEntryIndex index)
{
    switch (objectType)
    {
        case ObjectType::Ride:
            return RideEntryIsInvented(index);
        case ObjectType::SceneryGroup:
            return SceneryGroupIsInvented(index);
        case ObjectType::SmallScenery:
            return SceneryIsInvented({ SCENERY_TYPE_SMALL, index });
        case ObjectType::LargeScenery:
            return SceneryIsInvented({ SCENERY_TYPE_LARGE, index });
        case ObjectType::Walls:
            return SceneryIsInvented({ SCENERY_TYPE_WALL, index });
        case ObjectType::Banners:
            return SceneryIsInvented({ SCENERY_TYPE_BANNER, index });
        case ObjectType::PathAdditions:
            return SceneryIsInvented({ SCENERY_TYPE_PATH_ITEM, index });
        default:
            return true;
    }
}

bool RideTypeIsInvented(ride_type_t rideType)
{
    return RideTypeIsValid(rideType) ? _researchedRideTypes[rideType] : false;
}

bool RideEntryIsInvented(ObjectEntryIndex rideEntryIndex)
{
    if (rideEntryIndex >= std::size(_researchedRideEntries))
        return false;

    return _researchedRideEntries[rideEntryIndex];
}

void RideTypeSetInvented(ride_type_t rideType)
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
    auto& gameState = GetGameState();
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

    if (GetGameState().Cheats.ignoreResearchStatus)
    {
        return true;
    }

    return std::none_of(
        std::begin(gameState.ResearchItemsUninvented), std::end(gameState.ResearchItemsUninvented),
        [sgIndex](const ResearchItem& item) {
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
    for (int32_t i = 0; i < kMaxSceneryGroupObjects; ++i)
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
            return kStringIdEmpty;
        }

        return rideEntry->naming.Name;
    }

    const auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<SceneryGroupEntry>(entryIndex);
    if (sceneryEntry == nullptr)
    {
        return kStringIdEmpty;
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
    auto& gameState = GetGameState();
    for (auto& researchItem : gameState.ResearchItemsUninvented)
    {
        researchItem.flags &= ~(RESEARCH_ENTRY_FLAG_RIDE_ALWAYS_RESEARCHED | RESEARCH_ENTRY_FLAG_SCENERY_SET_ALWAYS_RESEARCHED);
    }
    for (auto& researchItem : gameState.ResearchItemsInvented)
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
                if (rideType != kRideTypeNull)
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
    auto& gameState = GetGameState();
    SetEveryRideTypeNotInvented();
    SetEveryRideEntryInvented();
    SetEveryRideEntryNotInvented();
    SetAllSceneryItemsNotInvented();
    for (const auto& item : gameState.ResearchItemsInvented)
    {
        // Ignore item, if the research of it is in progress
        if (gameState.ResearchProgressStage == RESEARCH_STAGE_DESIGNING
            || gameState.ResearchProgressStage == RESEARCH_STAGE_COMPLETING_DESIGN)
        {
            if (item == gameState.ResearchNextItem)
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
    auto& gameState = GetGameState();
    // Mark base ridetypes as seen if they exist in the invented research list.
    bool seenBaseEntry[kMaxRideObjects]{};
    for (auto const& researchItem : gameState.ResearchItemsInvented)
    {
        ObjectEntryIndex index = researchItem.baseRideType;
        seenBaseEntry[index] = true;
    }

    // Unlock and add research entries to the invented list for ride types whose base ridetype has been seen.
    for (ObjectEntryIndex i = 0; i < kMaxRideObjects; i++)
    {
        const auto* rideEntry = GetRideEntryByIndex(i);
        if (rideEntry != nullptr)
        {
            for (uint8_t j = 0; j < RCT2::ObjectLimits::kMaxRideTypesPerRideEntry; j++)
            {
                if (seenBaseEntry[rideEntry->ride_type[j]])
                {
                    RideEntrySetInvented(i);
                    ResearchInsertRideEntry(i, true);
                    break;
                }
            }
        }
    }

    // Mark base ridetypes as seen if they exist in the uninvented research list.
    for (auto const& researchItem : gameState.ResearchItemsUninvented)
    {
        ObjectEntryIndex index = researchItem.baseRideType;
        seenBaseEntry[index] = true;
    }

    // Only add Rides to uninvented research that haven't had their base ridetype seen.
    // This prevents rct2 grouped rides from only unlocking the first train.
    for (ObjectEntryIndex i = 0; i < kMaxRideObjects; i++)
    {
        const auto* rideEntry = GetRideEntryByIndex(i);
        if (rideEntry != nullptr)
        {
            bool baseSeen = false;
            for (uint8_t j = 0; j < RCT2::ObjectLimits::kMaxRideTypesPerRideEntry; j++)
            {
                if (seenBaseEntry[rideEntry->ride_type[j]])
                {
                    baseSeen = true;
                    break;
                }
            }
            if (!baseSeen)
            {
                ResearchInsertRideEntry(i, isResearched);
            }
        }
    }

    for (ObjectEntryIndex i = 0; i < kMaxSceneryGroupObjects; i++)
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
    auto& gameState = GetGameState();
    // Remove null entries from the research list
    ResearchRemoveNullItems(gameState.ResearchItemsInvented);
    ResearchRemoveNullItems(gameState.ResearchItemsUninvented);

    // Add missing entries to the research list
    // If research is complete, mark all the missing items as available
    ResearchAddAllMissingItems(gameState.ResearchProgressStage == RESEARCH_STAGE_FINISHED_ALL);

    // Now rebuild all the tables that say whether a ride or scenery item is invented
    ResearchRebuildInventedTables();
    ResearchUpdateUncompletedTypes();
}

void ResearchItemsMakeAllUnresearched()
{
    auto& gameState = GetGameState();
    gameState.ResearchItemsUninvented.insert(
        gameState.ResearchItemsUninvented.end(), std::make_move_iterator(gameState.ResearchItemsInvented.begin()),
        std::make_move_iterator(gameState.ResearchItemsInvented.end()));
    gameState.ResearchItemsInvented.clear();
}

void ResearchItemsMakeAllResearched()
{
    auto& gameState = GetGameState();
    gameState.ResearchItemsInvented.insert(
        gameState.ResearchItemsInvented.end(), std::make_move_iterator(gameState.ResearchItemsUninvented.begin()),
        std::make_move_iterator(gameState.ResearchItemsUninvented.end()));
    gameState.ResearchItemsUninvented.clear();
}

/**
 *
 *  rct2: 0x00685A93
 */
void ResearchItemsShuffle()
{
    auto& gameState = GetGameState();
    std::shuffle(
        std::begin(gameState.ResearchItemsUninvented), std::end(gameState.ResearchItemsUninvented),
        std::default_random_engine{});
}

bool ResearchItem::IsAlwaysResearched() const
{
    return (flags & (RESEARCH_ENTRY_FLAG_RIDE_ALWAYS_RESEARCHED | RESEARCH_ENTRY_FLAG_SCENERY_SET_ALWAYS_RESEARCHED)) != 0;
}

bool ResearchItem::IsNull() const
{
    return entryIndex == kObjectEntryIndexNull;
}

void ResearchItem::SetNull()
{
    entryIndex = kObjectEntryIndexNull;
}

bool ResearchItem::Exists() const
{
    auto& gameState = GetGameState();
    for (auto const& researchItem : gameState.ResearchItemsUninvented)
    {
        if (researchItem == *this)
        {
            return true;
        }
    }
    for (auto const& researchItem : gameState.ResearchItemsInvented)
    {
        if (researchItem == *this)
        {
            return true;
        }
    }
    return false;
}

// clang-format off
static constexpr StringId _editorInventionsResearchCategories[] = {
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
static constexpr StringId _researchCategoryNames[] = {
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
    if (rtd.HasFlag(RtdFlag::listVehiclesSeparately))
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
    auto& gameState = GetGameState();
    _seenRideType.reset();

    for (const auto& researchItem : gameState.ResearchItemsInvented)
    {
        if (researchItem.type != Research::EntryType::Ride)
            continue;

        auto rideType = researchItem.baseRideType;
        if (rideType >= RIDE_TYPE_COUNT)
            continue;

        const auto& rtd = GetRideTypeDescriptor(rideType);
        if (rtd.HasFlag(RtdFlag::listVehiclesSeparately))
            continue;

        // The last research item will also be present in gameState.ResearchItemsInvented.
        // Avoid marking its ride type as "invented" prematurely.
        if (gameState.ResearchLastItem.has_value() && !gameState.ResearchLastItem->IsNull()
            && researchItem == gameState.ResearchLastItem.value())
            continue;

        // The next research item is (sometimes?) also present in gameState.ResearchItemsInvented, even though it isn't invented
        // yet(!)
        if (gameState.ResearchNextItem.has_value() && !gameState.ResearchNextItem->IsNull()
            && researchItem == gameState.ResearchNextItem.value())
            continue;

        ResearchMarkRideTypeAsSeen(researchItem);
    }

    if (gameState.ResearchLastItem.has_value())
    {
        ResearchUpdateFirstOfType(&gameState.ResearchLastItem.value());
        ResearchMarkRideTypeAsSeen(gameState.ResearchLastItem.value());
    }
    if (gameState.ResearchNextItem.has_value())
    {
        ResearchUpdateFirstOfType(&gameState.ResearchNextItem.value());
        ResearchMarkRideTypeAsSeen(gameState.ResearchNextItem.value());
    }

    for (auto& researchItem : gameState.ResearchItemsUninvented)
    {
        // The next research item is (sometimes?) also present in gameState.ResearchItemsUninvented
        if (gameState.ResearchNextItem.has_value() && !gameState.ResearchNextItem->IsNull()
            && researchItem.baseRideType == gameState.ResearchNextItem.value().baseRideType)
        {
            // Copy the "first of type" flag.
            researchItem.flags = gameState.ResearchNextItem->flags;
            continue;
        }

        ResearchUpdateFirstOfType(&researchItem);
        ResearchMarkRideTypeAsSeen(researchItem);
    }
}
