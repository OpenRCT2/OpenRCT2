/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "NewsItem.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../GameState.h"
#include "../Input.h"
#include "../OpenRCT2.h"
#include "../audio/Audio.h"
#include "../entity/EntityRegistry.h"
#include "../entity/Peep.h"
#include "../localisation/Formatter.h"
#include "../localisation/Formatting.h"
#include "../management/Research.h"
#include "../profiling/Profiling.h"
#include "../ride/Ride.h"
#include "../ride/Vehicle.h"
#include "../ui/WindowManager.h"
#include "../windows/Intent.h"
#include "../world/Location.hpp"
#include "../world/Map.h"

#include <cassert>

using namespace OpenRCT2;

News::Item& News::ItemQueues::Current()
{
    return Recent.front();
}

const News::Item& News::ItemQueues::Current() const
{
    return Recent.front();
}

bool News::IsValidIndex(int32_t index)
{
    if (index >= MaxItems)
    {
        LOG_ERROR("Tried to get news item past MAX_NEWS.");
        return false;
    }
    return true;
}

News::Item* News::GetItem(int32_t index)
{
    return getGameState().newsItems.At(index);
}

News::Item& News::ItemQueues::operator[](size_t index)
{
    return const_cast<Item&>(const_cast<const ItemQueues&>(*this)[index]);
}

const News::Item& News::ItemQueues::operator[](size_t index) const
{
    if (index < Recent.capacity())
        return Recent[index];

    return Archived[index - Recent.capacity()];
}

News::Item* News::ItemQueues::At(int32_t index)
{
    return const_cast<Item*>(const_cast<const ItemQueues&>(*this).At(index));
}

const News::Item* News::ItemQueues::At(int32_t index) const
{
    if (IsValidIndex(index))
    {
        return &(*this)[index];
    }

    return nullptr;
}

bool News::IsQueueEmpty()
{
    return getGameState().newsItems.IsEmpty();
}

bool News::ItemQueues::IsEmpty() const
{
    return Recent.empty();
}

/**
 *
 *  rct2: 0x0066DF32
 */
void News::ItemQueues::Clear()
{
    Recent.clear();
    Archived.clear();
}

void News::InitQueue(GameState_t& gameState)
{
    gameState.newsItems.Clear();
    assert(gameState.newsItems.IsEmpty());

    // Throttles for warning types (PEEP_*_WARNING)
    for (auto& warningThrottle : gameState.park.peepWarningThrottle)
    {
        warningThrottle = 0;
    }

    auto intent = Intent(INTENT_ACTION_INVALIDATE_TICKER_NEWS);
    ContextBroadcastIntent(&intent);
}

uint16_t News::ItemQueues::IncrementTicks()
{
    return ++Current().ticks;
}

static void TickCurrent()
{
    int32_t ticks = getGameState().newsItems.IncrementTicks();
    // Only play news item sound when in normal playing mode
    if (ticks == 1 && (gLegacyScene == LegacyScene::playing))
    {
        // Play sound
        OpenRCT2::Audio::Play(Audio::SoundId::newsItem, 0, ContextGetWidth() / 2);
    }
}

int32_t News::ItemQueues::RemoveTime() const
{
    if (!Recent[5].isEmpty() && !Recent[4].isEmpty() && !Recent[3].isEmpty() && !Recent[2].isEmpty())
    {
        return 256;
    }
    return 320;
}

bool News::ItemQueues::CurrentShouldBeArchived() const
{
    return Current().ticks >= RemoveTime();
}

/**
 *
 *  rct2: 0x0066E252
 */
void News::UpdateCurrentItem()
{
    PROFILED_FUNCTION();

    auto& gameState = getGameState();
    // Check if there is a current news item
    if (gameState.newsItems.IsEmpty())
        return;

    auto intent = Intent(INTENT_ACTION_INVALIDATE_TICKER_NEWS);
    ContextBroadcastIntent(&intent);

    // Update the current news item
    TickCurrent();

    // Removal of current news item
    if (gameState.newsItems.CurrentShouldBeArchived())
        gameState.newsItems.ArchiveCurrent();
}

/**
 *
 *  rct2: 0x0066E377
 */
void News::CloseCurrentItem()
{
    getGameState().newsItems.ArchiveCurrent();
}

void News::ItemQueues::ArchiveCurrent()
{
    // Check if there is a current message
    if (IsEmpty())
        return;

    Archived.push_back(Current());

    // Invalidate the news window
    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateByClass(WindowClass::recentNews);

    // Dequeue the current news item, shift news up
    Recent.pop_front();

    // Invalidate current news item bar
    auto intent = Intent(INTENT_ACTION_INVALIDATE_TICKER_NEWS);
    ContextBroadcastIntent(&intent);
}

/**
 * Get the (x,y,z) coordinates of the subject of a news item.
 * If the new item is no longer valid, return LOCATION_NULL in the x-coordinate
 *
 *  rct2: 0x0066BA74
 */
std::optional<CoordsXYZ> News::GetSubjectLocation(ItemType type, int32_t subject)
{
    std::optional<CoordsXYZ> subjectLoc{ std::nullopt };

    auto& gameState = getGameState();

    switch (type)
    {
        case ItemType::ride:
        {
            Ride* ride = GetRide(RideId::FromUnderlying(subject));
            if (ride == nullptr || ride->overallView.IsNull())
            {
                break;
            }
            auto rideViewCentre = ride->overallView.ToTileCentre();
            subjectLoc = CoordsXYZ{ rideViewCentre, TileElementHeight(rideViewCentre) };
            break;
        }
        case ItemType::peepOnRide:
        {
            auto peep = gameState.entities.TryGetEntity<Peep>(EntityId::FromUnderlying(subject));
            if (peep == nullptr)
                break;

            subjectLoc = peep->GetLocation();
            if (subjectLoc->x != kLocationNull)
                break;

            if (peep->State != PeepState::onRide && peep->State != PeepState::enteringRide)
            {
                subjectLoc = std::nullopt;
                break;
            }

            // Find which ride peep is on
            Ride* ride = GetRide(peep->CurrentRide);
            if (ride == nullptr || !(ride->lifecycleFlags & RIDE_LIFECYCLE_ON_TRACK))
            {
                subjectLoc = std::nullopt;
                break;
            }

            // Find the first car of the train peep is on
            auto sprite = gameState.entities.TryGetEntity<Vehicle>(ride->vehicles[peep->CurrentTrain]);
            // Find the actual car peep is on
            for (int32_t i = 0; i < peep->CurrentCar && sprite != nullptr; i++)
            {
                sprite = gameState.entities.TryGetEntity<Vehicle>(sprite->next_vehicle_on_train);
            }
            if (sprite != nullptr)
            {
                subjectLoc = sprite->GetLocation();
            }
            break;
        }
        case ItemType::peep:
        {
            auto peep = gameState.entities.TryGetEntity<Peep>(EntityId::FromUnderlying(subject));
            if (peep != nullptr)
            {
                subjectLoc = peep->GetLocation();
            }
            break;
        }
        case ItemType::blank:
        {
            auto subjectUnsigned = static_cast<uint32_t>(subject);
            auto subjectXY = CoordsXY{ static_cast<int16_t>(subjectUnsigned & 0xFFFF),
                                       static_cast<int16_t>(subjectUnsigned >> 16) };
            if (!subjectXY.IsNull())
            {
                subjectLoc = CoordsXYZ{ subjectXY, TileElementHeight(subjectXY) };
            }
            break;
        }
        default:
            break;
    }
    return subjectLoc;
}

News::Item* News::ItemQueues::FirstOpenOrNewSlot()
{
    for (auto emptySlots = Recent.capacity() - Recent.size(); emptySlots < 2; ++emptySlots)
    {
        ArchiveCurrent();
    }

    auto res = Recent.end();
    // The for loop above guarantees there is always an extra element to use
    assert(Recent.capacity() - Recent.size() >= 2);
    auto newsItem = res + 1;
    newsItem->type = ItemType::null;

    return &*res;
}

/**
 *
 *  rct2: 0x0066DF55
 */
News::Item* News::AddItemToQueue(ItemType type, StringId string_id, uint32_t assoc, const Formatter& formatter)
{
    utf8 buffer[256];

    // overflows possible?
    FormatStringLegacy(buffer, 256, string_id, formatter.Data());
    return AddItemToQueue(type, buffer, assoc);
}

// TODO: Use variant for assoc, requires strong type for each possible input.
News::Item* News::AddItemToQueue(ItemType type, StringId string_id, EntityId assoc, const Formatter& formatter)
{
    return AddItemToQueue(type, string_id, assoc.ToUnderlying(), formatter);
}

News::Item* News::AddItemToQueue(ItemType type, const utf8* text, uint32_t assoc)
{
    auto& date = GetDate();
    Item* newsItem = getGameState().newsItems.FirstOpenOrNewSlot();
    newsItem->type = type;
    newsItem->flags = 0;
    newsItem->assoc = assoc; // Make optional for Award, Money, Graph and Null
    newsItem->ticks = 0;
    newsItem->monthYear = static_cast<uint16_t>(date.GetMonthsElapsed());
    newsItem->day = date.GetDay() + 1;
    newsItem->text = text;

    return newsItem;
}

/**
 * Checks if News::ItemType requires an assoc
 * @return A boolean if assoc is required.
 */

bool News::CheckIfItemRequiresAssoc(ItemType type)
{
    switch (type)
    {
        case ItemType::null:
        case ItemType::award:
        case ItemType::money:
        case ItemType::graph:
            return false;
        default:
            return true; // Everything else requires assoc
    }
}

/**
 * Opens the window/tab for the subject of the news item
 *
 *  rct2: 0x0066EBE6
 *
 */
void News::OpenSubject(ItemType type, int32_t subject)
{
    switch (type)
    {
        case ItemType::ride:
        {
            auto intent = Intent(WindowClass::ride);
            intent.PutExtra(INTENT_EXTRA_RIDE_ID, subject);
            ContextOpenIntent(&intent);
            break;
        }
        case ItemType::peepOnRide:
        case ItemType::peep:
        {
            auto peep = getGameState().entities.TryGetEntity<Peep>(EntityId::FromUnderlying(subject));
            if (peep != nullptr)
            {
                auto intent = Intent(WindowClass::peep);
                intent.PutExtra(INTENT_EXTRA_PEEP, peep);
                ContextOpenIntent(&intent);
            }
            break;
        }
        case ItemType::money:
            ContextOpenWindow(WindowClass::finances);
            break;
        case ItemType::campaign:
            ContextOpenWindowView(WindowView::financeMarketing);
            break;
        case ItemType::research:
        {
            auto item = ResearchItem(subject, ResearchCategory::transport, 0);
            if (item.type == Research::EntryType::ride)
            {
                auto intent = Intent(INTENT_ACTION_NEW_RIDE_OF_TYPE);
                intent.PutExtra(INTENT_EXTRA_RIDE_TYPE, item.baseRideType);
                intent.PutExtra(INTENT_EXTRA_RIDE_ENTRY_INDEX, item.entryIndex);
                ContextOpenIntent(&intent);
                break;
            }

            auto intent = Intent(INTENT_ACTION_NEW_SCENERY);
            intent.PutExtra(INTENT_EXTRA_SCENERY_GROUP_ENTRY_INDEX, item.entryIndex);
            ContextOpenIntent(&intent);
            break;
        }
        case ItemType::peeps:
        {
            auto intent = Intent(WindowClass::guestList);
            intent.PutExtra(INTENT_EXTRA_GUEST_LIST_FILTER, static_cast<int32_t>(GuestListFilterType::guestsThinkingX));
            intent.PutExtra(INTENT_EXTRA_RIDE_ID, subject);
            ContextOpenIntent(&intent);
            break;
        }
        case ItemType::award:
            ContextOpenWindowView(WindowView::parkAwards);
            break;
        case ItemType::graph:
            ContextOpenWindowView(WindowView::parkRating);
            break;
        case ItemType::null:
        case ItemType::blank:
        case ItemType::count:
            break;
    }
}

/**
 *
 *  rct2: 0x0066E407
 */
void News::DisableNewsItems(ItemType type, uint32_t assoc)
{
    auto& gameState = getGameState();
    // TODO: write test invalidating windows
    gameState.newsItems.ForeachRecentNews([type, assoc, &gameState](auto& newsItem) {
        if (type == newsItem.type && assoc == newsItem.assoc)
        {
            newsItem.setFlags(ItemFlags::hasButton);
            if (&newsItem == &gameState.newsItems.Current())
            {
                auto intent = Intent(INTENT_ACTION_INVALIDATE_TICKER_NEWS);
                ContextBroadcastIntent(&intent);
            }
        }
    });

    gameState.newsItems.ForeachArchivedNews([type, assoc](auto& newsItem) {
        if (type == newsItem.type && assoc == newsItem.assoc)
        {
            newsItem.setFlags(ItemFlags::hasButton);
            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->InvalidateByClass(WindowClass::recentNews);
        }
    });
}

void News::AddItemToQueue(Item* newNewsItem)
{
    Item* newsItem = getGameState().newsItems.FirstOpenOrNewSlot();
    *newsItem = *newNewsItem;
}

void News::RemoveItem(int32_t index)
{
    if (index < 0 || index >= MaxItems)
        return;

    auto& gameState = getGameState();
    // News item is already null, no need to remove it
    if (gameState.newsItems[index].type == ItemType::null)
        return;

    size_t newsBoundary = index < ItemHistoryStart ? ItemHistoryStart : MaxItems;
    for (size_t i = index; i < newsBoundary - 1; i++)
    {
        gameState.newsItems[i] = gameState.newsItems[i + 1];
    }
    gameState.newsItems[newsBoundary - 1].type = ItemType::null;
}

void News::importNewsItems(GameState_t& gameState, const std::span<const Item> recent, const std::span<const Item> archived)
{
    gameState.newsItems.Clear();

    for (size_t i = 0; i < std::min<size_t>(recent.size(), ItemHistoryStart); i++)
    {
        gameState.newsItems[i] = recent[i];
    }
    for (size_t i = 0; i < std::min<size_t>(archived.size(), MaxItemsArchive); i++)
    {
        gameState.newsItems[ItemHistoryStart + i] = archived[i];
    }
}
