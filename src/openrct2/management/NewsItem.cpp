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

/**
 *
 *  rct2: 0x0066DF32
 */
void News::Items::clear()
{
    queue.clear();
    archive.clear();
}

void News::InitQueue(Park::ParkData& park)
{
    park.newsItems.clear();
    assert(park.newsItems.archive.empty() && park.newsItems.queue.empty());

    // Throttles for warning types (PEEP_*_WARNING)
    for (auto& warningThrottle : park.peepWarningThrottle)
    {
        warningThrottle = 0;
    }

    auto intent = Intent(INTENT_ACTION_UPDATE_NEWS_TICKER);
    ContextBroadcastIntent(&intent);
}

void News::Items::tickActiveItem()
{
    int32_t ticks = ++queue.front().ticks;
    // Only play news item sound when in normal playing mode
    if (ticks == 1 && (gLegacyScene == LegacyScene::playing))
    {
        // Play sound
        OpenRCT2::Audio::Play(Audio::SoundId::newsItem, 0, ContextGetWidth() / 2);
    }
}

int32_t News::Items::removeTime() const
{
    if (!queue[5].isEmpty() && !queue[4].isEmpty() && !queue[3].isEmpty() && !queue[2].isEmpty())
    {
        return 256;
    }
    return 320;
}

bool News::Items::activeItemShouldBeArchived() const
{
    return queue.front().ticks >= removeTime();
}

/**
 *
 *  rct2: 0x0066E252
 */
void News::Items::updateActiveItem()
{
    PROFILED_FUNCTION();

    // Check if there is a current news item
    if (queue.empty())
        return;

    auto intent = Intent(INTENT_ACTION_UPDATE_NEWS_TICKER);
    ContextBroadcastIntent(&intent);

    // Update the current news item
    tickActiveItem();

    // Removal of current news item
    if (activeItemShouldBeArchived())
        archiveActiveItem();
}

void News::Items::archiveActiveItem()
{
    // Check if there is a current message
    if (queue.empty())
        return;

    archive.push_back(queue.front());

    // Invalidate the news window
    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateByClass(WindowClass::recentNews);

    // Dequeue the current news item, shift news up
    queue.pop_front();

    // Invalidate current news item bar
    auto intent = Intent(INTENT_ACTION_UPDATE_NEWS_TICKER);
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
            if (ride == nullptr || ride->overallView.isNull())
            {
                break;
            }
            auto rideViewCentre = ride->overallView.toTileCentre();
            subjectLoc = CoordsXYZ{ rideViewCentre, TileElementHeight(rideViewCentre) };
            break;
        }
        case ItemType::peepOnRide:
        {
            auto peep = gameState.entities.tryGetEntity<Peep>(EntityId::FromUnderlying(subject));
            if (peep == nullptr)
                break;

            subjectLoc = peep->getLocation();
            if (subjectLoc->x != kLocationNull)
                break;

            if (peep->state != PeepState::onRide && peep->state != PeepState::enteringRide)
            {
                subjectLoc = std::nullopt;
                break;
            }

            // Find which ride peep is on
            Ride* ride = GetRide(peep->currentRide);
            if (ride == nullptr || !ride->flags.has(RideFlag::onTrack))
            {
                subjectLoc = std::nullopt;
                break;
            }

            // Find the first car of the train peep is on
            auto sprite = gameState.entities.tryGetEntity<Vehicle>(ride->vehicles[peep->currentTrain]);
            // Find the actual car peep is on
            for (int32_t i = 0; i < peep->currentCar && sprite != nullptr; i++)
            {
                sprite = gameState.entities.tryGetEntity<Vehicle>(sprite->next_vehicle_on_train);
            }
            if (sprite != nullptr)
            {
                subjectLoc = sprite->getLocation();
            }
            break;
        }
        case ItemType::peep:
        {
            auto peep = gameState.entities.tryGetEntity<Peep>(EntityId::FromUnderlying(subject));
            if (peep != nullptr)
            {
                subjectLoc = peep->getLocation();
            }
            break;
        }
        case ItemType::blank:
        {
            auto subjectUnsigned = static_cast<uint32_t>(subject);
            auto subjectXY = CoordsXY{ static_cast<int16_t>(subjectUnsigned & 0xFFFF),
                                       static_cast<int16_t>(subjectUnsigned >> 16) };
            if (!subjectXY.isNull())
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

News::Item* News::Items::firstOpenOrNewSlot()
{
    for (auto emptySlots = queue.capacity() - queue.size(); emptySlots < 2; ++emptySlots)
    {
        archiveActiveItem();
    }

    auto res = queue.end();
    // The for loop above guarantees there is always an extra element to use
    assert(queue.capacity() - queue.size() >= 2);
    auto newsItem = res + 1;
    newsItem->type = ItemType::null;

    return &*res;
}

const News::Item* News::Items::getActiveMessage() const
{
    return queue.at(0);
}

/**
 *
 *  rct2: 0x0066DF55
 */
News::Item* News::Items::addItemToQueue(ItemType type, StringId string_id, uint32_t assoc, const Formatter& formatter)
{
    utf8 buffer[256];

    // overflows possible?
    FormatStringLegacy(buffer, 256, string_id, formatter.Data());
    return addItemToQueue(type, buffer, assoc);
}

// TODO: Use variant for assoc, requires strong type for each possible input.
News::Item* News::Items::addItemToQueue(ItemType type, StringId string_id, EntityId assoc, const Formatter& formatter)
{
    return addItemToQueue(type, string_id, assoc.ToUnderlying(), formatter);
}

News::Item* News::Items::addItemToQueue(ItemType type, const utf8* text, uint32_t assoc)
{
    auto& date = GetDate();
    Item* newsItem = firstOpenOrNewSlot();
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
            auto peep = getGameState().entities.tryGetEntity<Peep>(EntityId::FromUnderlying(subject));
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
void News::DisableNewsItems(Items& newsItems, ItemType type, uint32_t assoc)
{
    // TODO: write test invalidating windows
    for (auto& newsItem : newsItems.queue)
    {
        if (type == newsItem.type && assoc == newsItem.assoc)
        {
            newsItem.setFlags(ItemFlags::hasButton);
            if (&newsItem == &newsItems.queue.front())
            {
                auto intent = Intent(INTENT_ACTION_UPDATE_NEWS_TICKER);
                ContextBroadcastIntent(&intent);
            }
        }
    }

    for (auto& newsItem : newsItems.archive)
    {
        if (type == newsItem.type && assoc == newsItem.assoc)
        {
            newsItem.setFlags(ItemFlags::hasButton);
            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->InvalidateByClass(WindowClass::recentNews);
        }
    }
}
