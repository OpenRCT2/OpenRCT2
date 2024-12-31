/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
#include "../audio/audio.h"
#include "../entity/EntityRegistry.h"
#include "../entity/Peep.h"
#include "../interface/Window.h"
#include "../interface/Window_internal.h"
#include "../localisation/Formatter.h"
#include "../localisation/Formatting.h"
#include "../localisation/Localisation.Date.h"
#include "../management/Research.h"
#include "../profiling/Profiling.h"
#include "../ride/Ride.h"
#include "../ride/Vehicle.h"
#include "../windows/Intent.h"
#include "../world/Location.hpp"

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
    if (index >= News::MaxItems)
    {
        LOG_ERROR("Tried to get news item past MAX_NEWS.");
        return false;
    }
    return true;
}

News::Item* News::GetItem(int32_t index)
{
    return GetGameState().NewsItems.At(index);
}

News::Item& News::ItemQueues::operator[](size_t index)
{
    return const_cast<News::Item&>(const_cast<const News::ItemQueues&>(*this)[index]);
}

const News::Item& News::ItemQueues::operator[](size_t index) const
{
    if (index < Recent.capacity())
        return Recent[index];

    return Archived[index - Recent.capacity()];
}

News::Item* News::ItemQueues::At(int32_t index)
{
    return const_cast<News::Item*>(const_cast<const News::ItemQueues&>(*this).At(index));
}

const News::Item* News::ItemQueues::At(int32_t index) const
{
    if (News::IsValidIndex(index))
    {
        return &(*this)[index];
    }

    return nullptr;
}

bool News::IsQueueEmpty()
{
    return GetGameState().NewsItems.IsEmpty();
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

void News::InitQueue()
{
    auto& gameState = GetGameState();
    gameState.NewsItems.Clear();
    assert(gameState.NewsItems.IsEmpty());

    // Throttles for warning types (PEEP_*_WARNING)
    for (auto& warningThrottle : gameState.PeepWarningThrottle)
    {
        warningThrottle = 0;
    }

    auto intent = Intent(INTENT_ACTION_INVALIDATE_TICKER_NEWS);
    ContextBroadcastIntent(&intent);
}

uint16_t News::ItemQueues::IncrementTicks()
{
    return ++Current().Ticks;
}

static void TickCurrent()
{
    int32_t ticks = GetGameState().NewsItems.IncrementTicks();
    // Only play news item sound when in normal playing mode
    if (ticks == 1 && (gScreenFlags == SCREEN_FLAGS_PLAYING))
    {
        // Play sound
        OpenRCT2::Audio::Play(OpenRCT2::Audio::SoundId::NewsItem, 0, ContextGetWidth() / 2);
    }
}

int32_t News::ItemQueues::RemoveTime() const
{
    if (!Recent[5].IsEmpty() && !Recent[4].IsEmpty() && !Recent[3].IsEmpty() && !Recent[2].IsEmpty())
    {
        return 256;
    }
    return 320;
}

bool News::ItemQueues::CurrentShouldBeArchived() const
{
    return Current().Ticks >= RemoveTime();
}

/**
 *
 *  rct2: 0x0066E252
 */
void News::UpdateCurrentItem()
{
    PROFILED_FUNCTION();

    auto& gameState = GetGameState();
    // Check if there is a current news item
    if (gameState.NewsItems.IsEmpty())
        return;

    auto intent = Intent(INTENT_ACTION_INVALIDATE_TICKER_NEWS);
    ContextBroadcastIntent(&intent);

    // Update the current news item
    TickCurrent();

    // Removal of current news item
    if (gameState.NewsItems.CurrentShouldBeArchived())
        gameState.NewsItems.ArchiveCurrent();
}

/**
 *
 *  rct2: 0x0066E377
 */
void News::CloseCurrentItem()
{
    GetGameState().NewsItems.ArchiveCurrent();
}

void News::ItemQueues::ArchiveCurrent()
{
    // Check if there is a current message
    if (IsEmpty())
        return;

    Archived.push_back(Current());

    // Invalidate the news window
    WindowInvalidateByClass(WindowClass::RecentNews);

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
std::optional<CoordsXYZ> News::GetSubjectLocation(News::ItemType type, int32_t subject)
{
    std::optional<CoordsXYZ> subjectLoc{ std::nullopt };

    switch (type)
    {
        case News::ItemType::Ride:
        {
            Ride* ride = GetRide(RideId::FromUnderlying(subject));
            if (ride == nullptr || ride->overall_view.IsNull())
            {
                break;
            }
            auto rideViewCentre = ride->overall_view.ToTileCentre();
            subjectLoc = CoordsXYZ{ rideViewCentre, TileElementHeight(rideViewCentre) };
            break;
        }
        case News::ItemType::PeepOnRide:
        {
            auto peep = TryGetEntity<Peep>(EntityId::FromUnderlying(subject));
            if (peep == nullptr)
                break;

            subjectLoc = peep->GetLocation();
            if (subjectLoc->x != kLocationNull)
                break;

            if (peep->State != PeepState::OnRide && peep->State != PeepState::EnteringRide)
            {
                subjectLoc = std::nullopt;
                break;
            }

            // Find which ride peep is on
            Ride* ride = GetRide(peep->CurrentRide);
            if (ride == nullptr || !(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK))
            {
                subjectLoc = std::nullopt;
                break;
            }

            // Find the first car of the train peep is on
            auto sprite = TryGetEntity<Vehicle>(ride->vehicles[peep->CurrentTrain]);
            // Find the actual car peep is on
            for (int32_t i = 0; i < peep->CurrentCar && sprite != nullptr; i++)
            {
                sprite = TryGetEntity<Vehicle>(sprite->next_vehicle_on_train);
            }
            if (sprite != nullptr)
            {
                subjectLoc = sprite->GetLocation();
            }
            break;
        }
        case News::ItemType::Peep:
        {
            auto peep = TryGetEntity<Peep>(EntityId::FromUnderlying(subject));
            if (peep != nullptr)
            {
                subjectLoc = peep->GetLocation();
            }
            break;
        }
        case News::ItemType::Blank:
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
    newsItem->Type = News::ItemType::Null;

    return &*res;
}

/**
 *
 *  rct2: 0x0066DF55
 */
News::Item* News::AddItemToQueue(News::ItemType type, StringId string_id, uint32_t assoc, const Formatter& formatter)
{
    utf8 buffer[256];

    // overflows possible?
    OpenRCT2::FormatStringLegacy(buffer, 256, string_id, formatter.Data());
    return News::AddItemToQueue(type, buffer, assoc);
}

// TODO: Use variant for assoc, requires strong type for each possible input.
News::Item* News::AddItemToQueue(ItemType type, StringId string_id, EntityId assoc, const Formatter& formatter)
{
    return AddItemToQueue(type, string_id, assoc.ToUnderlying(), formatter);
}

News::Item* News::AddItemToQueue(News::ItemType type, const utf8* text, uint32_t assoc)
{
    auto& date = GetDate();
    News::Item* newsItem = GetGameState().NewsItems.FirstOpenOrNewSlot();
    newsItem->Type = type;
    newsItem->Flags = 0;
    newsItem->Assoc = assoc; // Make optional for Award, Money, Graph and Null
    newsItem->Ticks = 0;
    newsItem->MonthYear = static_cast<uint16_t>(date.GetMonthsElapsed());
    newsItem->Day = date.GetDay() + 1;
    newsItem->Text = text;

    return newsItem;
}

/**
 * Checks if News::ItemType requires an assoc
 * @return A boolean if assoc is required.
 */

bool News::CheckIfItemRequiresAssoc(News::ItemType type)
{
    switch (type)
    {
        case News::ItemType::Null:
        case News::ItemType::Award:
        case News::ItemType::Money:
        case News::ItemType::Graph:
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
void News::OpenSubject(News::ItemType type, int32_t subject)
{
    switch (type)
    {
        case News::ItemType::Ride:
        {
            auto intent = Intent(WindowClass::Ride);
            intent.PutExtra(INTENT_EXTRA_RIDE_ID, subject);
            ContextOpenIntent(&intent);
            break;
        }
        case News::ItemType::PeepOnRide:
        case News::ItemType::Peep:
        {
            auto peep = TryGetEntity<Peep>(EntityId::FromUnderlying(subject));
            if (peep != nullptr)
            {
                auto intent = Intent(WindowClass::Peep);
                intent.PutExtra(INTENT_EXTRA_PEEP, peep);
                ContextOpenIntent(&intent);
            }
            break;
        }
        case News::ItemType::Money:
            ContextOpenWindow(WindowClass::Finances);
            break;
        case News::ItemType::Campaign:
            ContextOpenWindowView(WV_FINANCE_MARKETING);
            break;
        case News::ItemType::Research:
        {
            auto item = ResearchItem(subject, ResearchCategory::Transport, 0);
            if (item.type == Research::EntryType::Ride)
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
        case News::ItemType::Peeps:
        {
            auto intent = Intent(WindowClass::GuestList);
            intent.PutExtra(INTENT_EXTRA_GUEST_LIST_FILTER, static_cast<int32_t>(GuestListFilterType::GuestsThinkingX));
            intent.PutExtra(INTENT_EXTRA_RIDE_ID, subject);
            ContextOpenIntent(&intent);
            break;
        }
        case News::ItemType::Award:
            ContextOpenWindowView(WV_PARK_AWARDS);
            break;
        case News::ItemType::Graph:
            ContextOpenWindowView(WV_PARK_RATING);
            break;
        case News::ItemType::Null:
        case News::ItemType::Blank:
        case News::ItemType::Count:
            break;
    }
}

/**
 *
 *  rct2: 0x0066E407
 */
void News::DisableNewsItems(News::ItemType type, uint32_t assoc)
{
    auto& gameState = GetGameState();
    // TODO: write test invalidating windows
    gameState.NewsItems.ForeachRecentNews([type, assoc, &gameState](auto& newsItem) {
        if (type == newsItem.Type && assoc == newsItem.Assoc)
        {
            newsItem.SetFlags(News::ItemFlags::HasButton);
            if (&newsItem == &gameState.NewsItems.Current())
            {
                auto intent = Intent(INTENT_ACTION_INVALIDATE_TICKER_NEWS);
                ContextBroadcastIntent(&intent);
            }
        }
    });

    gameState.NewsItems.ForeachArchivedNews([type, assoc](auto& newsItem) {
        if (type == newsItem.Type && assoc == newsItem.Assoc)
        {
            newsItem.SetFlags(News::ItemFlags::HasButton);
            WindowInvalidateByClass(WindowClass::RecentNews);
        }
    });
}

void News::AddItemToQueue(News::Item* newNewsItem)
{
    News::Item* newsItem = GetGameState().NewsItems.FirstOpenOrNewSlot();
    *newsItem = *newNewsItem;
}

void News::RemoveItem(int32_t index)
{
    if (index < 0 || index >= News::MaxItems)
        return;

    auto& gameState = GetGameState();
    // News item is already null, no need to remove it
    if (gameState.NewsItems[index].Type == News::ItemType::Null)
        return;

    size_t newsBoundary = index < News::ItemHistoryStart ? News::ItemHistoryStart : News::MaxItems;
    for (size_t i = index; i < newsBoundary - 1; i++)
    {
        gameState.NewsItems[i] = gameState.NewsItems[i + 1];
    }
    gameState.NewsItems[newsBoundary - 1].Type = News::ItemType::Null;
}
