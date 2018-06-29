/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../audio/audio.h"
#include "../Context.h"
#include "../core/Util.hpp"
#include "../Input.h"
#include "../interface/Window.h"
#include "../localisation/Date.h"
#include "../localisation/Localisation.h"
#include "../management/Research.h"
#include "../OpenRCT2.h"
#include "../ride/Ride.h"
#include "../util/Util.h"
#include "../windows/Intent.h"
#include "../world/Sprite.h"
#include "NewsItem.h"

NewsItem gNewsItems[MAX_NEWS_ITEMS];

/** rct2: 0x0097BE7C */
const uint8_t news_type_properties[] =
{
    0,                                              // NEWS_ITEM_NULL
    NEWS_TYPE_HAS_LOCATION | NEWS_TYPE_HAS_SUBJECT, // NEWS_ITEM_RIDE
    NEWS_TYPE_HAS_LOCATION | NEWS_TYPE_HAS_SUBJECT, // NEWS_ITEM_PEEP_ON_RIDE
    NEWS_TYPE_HAS_LOCATION | NEWS_TYPE_HAS_SUBJECT, // NEWS_ITEM_PEEP
    NEWS_TYPE_HAS_SUBJECT,                          // NEWS_ITEM_MONEY
    NEWS_TYPE_HAS_LOCATION,                         // NEWS_ITEM_BLANK
    NEWS_TYPE_HAS_SUBJECT,                          // NEWS_ITEM_RESEARCH
    NEWS_TYPE_HAS_SUBJECT,                          // NEWS_ITEM_PEEPS
    NEWS_TYPE_HAS_SUBJECT,                          // NEWS_ITEM_AWARD
    NEWS_TYPE_HAS_SUBJECT,                          // NEWS_ITEM_GRAPH
};

static int32_t news_item_get_new_history_slot();

bool news_item_is_valid_idx(int32_t index)
{
    if (index >= MAX_NEWS_ITEMS)
    {
        log_error("Tried to get news item past MAX_NEWS.");
        return false;
    }
    return true;
}

NewsItem * news_item_get(int32_t index)
{
    if (news_item_is_valid_idx(index))
    {
        return &gNewsItems[index];
    }
    else
    {
        return nullptr;
    }
}

bool news_item_is_empty(int32_t index)
{
    NewsItem * news = news_item_get(index);
    return news != nullptr && news->Type == NEWS_ITEM_NULL;
}

bool news_item_is_queue_empty()
{
    return news_item_is_empty(0);
}

/**
 *
 *  rct2: 0x0066DF32
 */
void news_item_init_queue()
{
    news_item_get(0)->Type  = NEWS_ITEM_NULL;
    news_item_get(11)->Type = NEWS_ITEM_NULL;

    // Throttles for warning types (PEEP_*_WARNING)
    for (auto &warningThrottle : gPeepWarningThrottle)
    {
        warningThrottle = 0;
    }

    auto intent = Intent(INTENT_ACTION_INVALIDATE_TICKER_NEWS);
    context_broadcast_intent(&intent);
}

static void news_item_tick_current()
{
    int32_t ticks;
    ticks = ++news_item_get(0)->Ticks;
    // Only play news item sound when in normal playing mode
    if (ticks == 1 && (gScreenFlags == SCREEN_FLAGS_PLAYING))
    {
        // Play sound
        audio_play_sound(SOUND_NEWS_ITEM, 0, context_get_width() / 2);
    }
}

static bool news_item_is_current_old()
{
    int32_t remove_time = 320;
    if (!news_item_is_empty(5) &&
        !news_item_is_empty(4) &&
        !news_item_is_empty(3) &&
        !news_item_is_empty(2))
    {
        remove_time = 256;
    }

    return news_item_get(0)->Ticks >= remove_time;
}

/**
 *
 *  rct2: 0x0066E252
 */
void news_item_update_current()
{
    // Check if there is a current news item
    if (news_item_is_queue_empty())
        return;

    auto intent = Intent(INTENT_ACTION_INVALIDATE_TICKER_NEWS);
    context_broadcast_intent(&intent);

    // Update the current news item
    news_item_tick_current();

    // Removal of current news item
    if (news_item_is_current_old())
        news_item_close_current();
}

/**
 *
 *  rct2: 0x0066E377
 */
void news_item_close_current()
{
    int32_t i;
    NewsItem * newsItems = gNewsItems;

    // Check if there is a current message
    if (news_item_is_queue_empty())
        return;

    // Find an available history news item slot for current message
    i = news_item_get_new_history_slot();

    // Set the history news item slot to the current news item
    newsItems[i] = newsItems[0];

    // Set the end of the end of the history list
    if (i < MAX_NEWS_ITEMS - 1)
        newsItems[i + 1].Type = NEWS_ITEM_NULL;

    // Invalidate the news window
    window_invalidate_by_class(WC_RECENT_NEWS);

    // Dequeue the current news item, shift news up
    for (i = 0; i < 10; i++)
    {
        newsItems[i] = newsItems[i + 1];
    }
    newsItems[10].Type = NEWS_ITEM_NULL;

    // Invalidate current news item bar
    auto intent = Intent(INTENT_ACTION_INVALIDATE_TICKER_NEWS);
    context_broadcast_intent(&intent);
}

static void news_item_shift_history_up()
{
    const int32_t history_idx = 11;
    NewsItem * history_start = news_item_get(history_idx);
    const size_t count = sizeof(NewsItem) * (MAX_NEWS_ITEMS - 1 - history_idx);
    memmove(history_start, history_start + 1, count);
}


/**
 * Finds a spare history slot or replaces an existing one if there are no spare
 * slots available.
 */
static int32_t news_item_get_new_history_slot()
{
    // Find an available history news item slot
    for (int32_t i = 11; i < MAX_NEWS_ITEMS; i++)
    {
        if (news_item_is_empty(i))
            return i;
    }

    // Dequeue the first history news item, shift history up
    news_item_shift_history_up();
    return MAX_NEWS_ITEMS - 1;
}

/**
 * Get the (x,y,z) coordinates of the subject of a news item.
 * If the new item is no longer valid, return LOCATION_NULL in the x-coordinate
 *
 *  rct2: 0x0066BA74
 */
void news_item_get_subject_location(int32_t type, int32_t subject, int32_t * x, int32_t * y, int32_t * z)
{
    Ride        * ride;
    rct_peep    * peep;
    rct_vehicle * vehicle;

    switch (type)
    {
    case NEWS_ITEM_RIDE:
        ride = get_ride(subject);
        if (ride->overall_view.xy == RCT_XY8_UNDEFINED)
        {
            *x = LOCATION_NULL;
            break;
        }
        *x   = ride->overall_view.x * 32 + 16;
        *y   = ride->overall_view.y * 32 + 16;
        *z   = tile_element_height(*x, *y);
        break;
    case NEWS_ITEM_PEEP_ON_RIDE:
        peep = GET_PEEP(subject);
        *x = peep->x;
        *y = peep->y;
        *z = peep->z;
        if (*x != LOCATION_NULL)
            break;

        if (peep->state != 3 && peep->state != 7)
        {
            *x = LOCATION_NULL;
            break;
        }

        // Find which ride peep is on
        ride = get_ride(peep->current_ride);
        if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK))
        {
            *x = LOCATION_NULL;
            break;
        }

        // Find the first car of the train peep is on
        vehicle = &(get_sprite(ride->vehicles[peep->current_train])->vehicle);
        // Find the actual car peep is on
        for (int32_t i = 0; i < peep->current_car; i++)
        {
            vehicle = &(get_sprite(vehicle->next_vehicle_on_train)->vehicle);
        }
        *x = vehicle->x;
        *y = vehicle->y;
        *z = vehicle->z;
        break;
    case NEWS_ITEM_PEEP:
        peep = GET_PEEP(subject);
        *x = peep->x;
        *y = peep->y;
        *z = peep->z;
        break;
    case NEWS_ITEM_BLANK:
        *x = subject;
        *y = subject >> 16;
        *z = tile_element_height(*x, *y);
        break;
    default:
        *x = LOCATION_NULL;
        break;
    }
}


/**
 *
 *  rct2: 0x0066DF55
 */
void news_item_add_to_queue(uint8_t type, rct_string_id string_id, uint32_t assoc)
{
    utf8 buffer[256];
    void * args = gCommonFormatArgs;

    // overflows possible?
    format_string(buffer, 256, string_id, args);
    news_item_add_to_queue_raw(type, buffer, assoc);
}

void news_item_add_to_queue_raw(uint8_t type, const utf8 * text, uint32_t assoc)
{
    NewsItem * newsItem = gNewsItems;

    // Find first open slot
    while (newsItem->Type != NEWS_ITEM_NULL)
    {
        if (newsItem + 1 >= &gNewsItems[10])
            news_item_close_current();
        else
            newsItem++;
    }

    // Now we have found an item slot to place the new news in
    newsItem->Type      = type;
    newsItem->Flags     = 0;
    newsItem->Assoc     = assoc;
    newsItem->Ticks     = 0;
    newsItem->MonthYear = gDateMonthsElapsed;
    newsItem->Day       = ((days_in_month[date_get_month(newsItem->MonthYear)] * gDateMonthTicks) >> 16) + 1;
    safe_strcpy(newsItem->Text, text, sizeof(newsItem->Text));

    // Blatant disregard for what happens on the last element.
    // TODO: Change this when we implement the queue ourselves.
    newsItem++;
    newsItem->Type = NEWS_ITEM_NULL;
}

/**
 * Opens the window/tab for the subject of the news item
 *
 *  rct2: 0x0066EBE6
 *
 */
void news_item_open_subject(int32_t type, int32_t subject)
{
    rct_peep   * peep;
    rct_window * window;

    switch (type)
    {
    case NEWS_ITEM_RIDE:
    {
        auto intent = Intent(WC_RIDE);
        intent.putExtra(INTENT_EXTRA_RIDE_ID, subject);
        context_open_intent(&intent);
        break;
    }
    case NEWS_ITEM_PEEP_ON_RIDE:
    case NEWS_ITEM_PEEP:
    {
        peep = GET_PEEP(subject);

        auto intent = Intent(WC_PEEP);
        intent.putExtra(INTENT_EXTRA_PEEP, peep);
        context_open_intent(&intent);
        break;
    }
    case NEWS_ITEM_MONEY:
        context_open_window(WC_FINANCES);
        break;
    case NEWS_ITEM_RESEARCH:
        if (subject >= RESEARCH_ENTRY_RIDE_MASK)
        {
            auto intent = Intent(INTENT_ACTION_NEW_RIDE_OF_TYPE);
            intent.putExtra(INTENT_EXTRA_RIDE_TYPE, subject >> 8);
            intent.putExtra(INTENT_EXTRA_RIDE_ENTRY_INDEX, subject & 0xFF);
            context_open_intent(&intent);
            break;
        }

        // Check if window is already open
        window = window_bring_to_front_by_class(WC_SCENERY);
        if (window == nullptr)
        {
            window = window_find_by_class(WC_TOP_TOOLBAR);
            if (window != nullptr)
            {
                window_invalidate(window);
                if (!tool_set(window, WC_TOP_TOOLBAR__WIDX_SCENERY, TOOL_ARROW))
                {
                    input_set_flag(INPUT_FLAG_6, true);
                    context_open_window(WC_SCENERY);
                }
            }
        }

        // Switch to new scenery tab
        window = window_find_by_class(WC_SCENERY);
        if (window != nullptr)
            window_event_mouse_down_call(window, WC_SCENERY__WIDX_SCENERY_TAB_1 + subject);
        break;
    case NEWS_ITEM_PEEPS:
    {
        auto intent = Intent(WC_GUEST_LIST);
        intent.putExtra(INTENT_EXTRA_GUEST_LIST_FILTER, GLFT_GUESTS_THINKING_X);
        intent.putExtra(INTENT_EXTRA_RIDE_ID, subject);
        context_open_intent(&intent);
        break;
    }
    case NEWS_ITEM_AWARD:
        context_open_window_view(WV_PARK_AWARDS);
        break;
    case NEWS_ITEM_GRAPH:
        context_open_window_view(WV_PARK_RATING);
        break;
    }
}

/**
 *
 *  rct2: 0x0066E407
 */
void news_item_disable_news(uint8_t type, uint32_t assoc)
{
    // TODO: write test invalidating windows
    for (int32_t i = 0; i < 11; i++)
    {
        if (!news_item_is_empty(i))
        {
            NewsItem * const newsItem = news_item_get(i);
            if (type == newsItem->Type && assoc == newsItem->Assoc)
            {
                newsItem->Flags |= NEWS_FLAG_HAS_BUTTON;
                if (i == 0)
                {
                    auto intent = Intent(INTENT_ACTION_INVALIDATE_TICKER_NEWS);
                    context_broadcast_intent(&intent);
                }
            }
        }
        else
        {
            break;
        }
    }

    for (int32_t i = 11; i < MAX_NEWS_ITEMS; i++)
    {
        if (!news_item_is_empty(i))
        {
            NewsItem * const newsItem = news_item_get(i);
            if (type == newsItem->Type && assoc == newsItem->Assoc)
            {
                newsItem->Flags |= NEWS_FLAG_HAS_BUTTON;
                window_invalidate_by_class(WC_RECENT_NEWS);
            }
        }
        else
        {
            break;
        }
    }
}

void news_item_add_to_queue_custom(NewsItem * newNewsItem)
{
    NewsItem * newsItem = gNewsItems;

    // Find first open slot
    while (newsItem->Type != NEWS_ITEM_NULL)
    {
        if (newsItem + 1 >= &gNewsItems[10])
            news_item_close_current();
        else
            newsItem++;
    }

    *newsItem = *newNewsItem;
    newsItem++;
    newsItem->Type = NEWS_ITEM_NULL;
}
