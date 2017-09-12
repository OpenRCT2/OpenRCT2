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

#include "../audio/audio.h"
#include "../Context.h"
#include "../input.h"
#include "../interface/window.h"
#include "../localisation/date.h"
#include "../localisation/localisation.h"
#include "../OpenRCT2.h"
#include "../ride/ride.h"
#include "../util/util.h"
#include "../world/sprite.h"
#include "news_item.h"

NewsItem gNewsItems[MAX_NEWS_ITEMS];

/** rct2: 0x0097BE7C */
const uint8 news_type_properties[] =    {
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

static sint32 news_item_get_new_history_slot();

bool news_item_is_valid_idx(sint32 index)
{
    if (index >= MAX_NEWS_ITEMS) {
        log_error("Tried to get news item past MAX_NEWS.");
        return false;
    }
    return true;
}

NewsItem *news_item_get(sint32 index)
{
    if (news_item_is_valid_idx(index)) {
        return &gNewsItems[index];
    } else {
        return NULL;
    }
}

bool news_item_is_empty(sint32 index)
{
    return news_item_get(index)->Type == NEWS_ITEM_NULL;
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
    sint32 i;

    news_item_get(0)->Type = NEWS_ITEM_NULL;
    news_item_get(11)->Type = NEWS_ITEM_NULL;

    // Throttles for warning types (PEEP_*_WARNING)
    for (i = 0; i < 16; i++) {
        gPeepWarningThrottle[i] = 0;
    }

    window_game_bottom_toolbar_invalidate_news_item();
}

static void news_item_tick_current()
{
    sint32 ticks;
    ticks = ++news_item_get(0)->Ticks;
    // Only play news item sound when in normal playing mode
    if (ticks == 1 && (gScreenFlags == SCREEN_FLAGS_PLAYING)) {
        // Play sound
        audio_play_sound(SOUND_NEWS_ITEM, 0, context_get_width() / 2);
    }
}

static bool news_item_is_current_old()
{
    sint32 remove_time = 320;
    if (!news_item_is_empty(5) &&
        !news_item_is_empty(4) &&
        !news_item_is_empty(3) &&
        !news_item_is_empty(2))
        remove_time = 256;

    if (news_item_get(0)->Ticks >= remove_time)
        return true;

    return false;
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

    window_game_bottom_toolbar_invalidate_news_item();

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
    sint32 i;
    NewsItem *newsItems = gNewsItems;

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
        newsItems[i] = newsItems[i + 1];
    newsItems[10].Type = NEWS_ITEM_NULL;

    // Invalidate current news item bar
    window_game_bottom_toolbar_invalidate_news_item();
}

static void news_item_shift_history_up()
{
    const sint32 history_idx = 11;
    NewsItem *history_start = news_item_get(history_idx);
    const size_t count = sizeof(NewsItem) * (MAX_NEWS_ITEMS - 1 - history_idx);
    memmove(history_start, history_start + 1, count);
}


/**
 * Finds a spare history slot or replaces an existing one if there are no spare
 * slots available.
 */
static sint32 news_item_get_new_history_slot()
{
    sint32 i;

    // Find an available history news item slot
    for (i = 11; i < MAX_NEWS_ITEMS; i++)
        if (news_item_is_empty(i))
            return i;

    // Dequeue the first history news item, shift history up
    news_item_shift_history_up();
    return MAX_NEWS_ITEMS - 1;
}

/**
 * Get the (x,y,z) coordinates of the subject of a news item.
 * If the new item is no longer valid, return SPRITE_LOCATION_NULL in the x-coordinate
 *
 *  rct2: 0x0066BA74
 */
void news_item_get_subject_location(sint32 type, sint32 subject, sint32 *x, sint32 *y, sint32 *z)
{
    sint32 i;
    Ride *ride;
    rct_peep *peep;
    rct_vehicle *vehicle;

    switch (type) {
    case NEWS_ITEM_RIDE:
        ride = get_ride(subject);
        if (ride->overall_view.xy == RCT_XY8_UNDEFINED) {
            *x = SPRITE_LOCATION_NULL;
            break;
        }
        *x = ride->overall_view.x * 32 + 16;
        *y = ride->overall_view.y * 32 + 16;
        *z = map_element_height(*x, *y);
        break;
    case NEWS_ITEM_PEEP_ON_RIDE:
        peep = GET_PEEP(subject);
        *x = peep->x;
        *y = peep->y;
        *z = peep->z;
        if (*x != SPRITE_LOCATION_NULL)
            break;

        if (peep->state != 3 && peep->state != 7) {
            *x = SPRITE_LOCATION_NULL;
            break;
        }

        // Find which ride peep is on
        ride = get_ride(peep->current_ride);
        if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK)) {
            *x = SPRITE_LOCATION_NULL;
            break;
        }

        // Find the first car of the train peep is on
        vehicle = &(get_sprite(ride->vehicles[peep->current_train])->vehicle);
        // Find the actual car peep is on
        for (i = 0; i < peep->current_car; i++)
            vehicle = &(get_sprite(vehicle->next_vehicle_on_train)->vehicle);
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
        *z = map_element_height(*x, *y);
         break;
    default:
        *x = SPRITE_LOCATION_NULL;
        break;
    }
}


/**
 *
 *  rct2: 0x0066DF55
 *
 * @param  a (al)
 * @param string_id (ebx)
 * @param c (ecx)
 */
void news_item_add_to_queue(uint8 type, rct_string_id string_id, uint32 assoc)
{
    utf8 buffer[256];
    void *args = gCommonFormatArgs;

    format_string(buffer, 256, string_id, args); // overflows possible?
    news_item_add_to_queue_raw(type, buffer, assoc);
}

void news_item_add_to_queue_raw(uint8 type, const utf8 *text, uint32 assoc)
{
    NewsItem *newsItem = gNewsItems;

    // find first open slot
    while (newsItem->Type != NEWS_ITEM_NULL) {
        if (newsItem + 1 >= &gNewsItems[10]) // &news_list[10]
            news_item_close_current();
        else
            newsItem++;
    }

    //now we have found an item slot to place the new news in
    newsItem->Type = type;
    newsItem->Flags = 0;
    newsItem->Assoc = assoc;
    newsItem->Ticks = 0;
    newsItem->MonthYear = gDateMonthsElapsed;
    newsItem->Day = ((days_in_month[(newsItem->MonthYear & 7)] * gDateMonthTicks) >> 16) + 1;
    safe_strcpy(newsItem->Text, text, sizeof(newsItem->Text));

    // blatant disregard for what happens on the last element.
    // Change this when we implement the queue ourselves.
    newsItem++;
    newsItem->Type = NEWS_ITEM_NULL;
}

/**
 * Opens the window/tab for the subject of the news item
 *
 *  rct2: 0x0066EBE6
 *
 */
void news_item_open_subject(sint32 type, sint32 subject)
{
    rct_peep* peep;
    rct_window* window;

    switch (type) {
    case NEWS_ITEM_RIDE:
        window_ride_main_open(subject);
        break;
    case NEWS_ITEM_PEEP_ON_RIDE:
    case NEWS_ITEM_PEEP:
        peep = GET_PEEP(subject);
        window_guest_open(peep);
        break;
    case NEWS_ITEM_MONEY:
        window_finances_open();
        break;
    case NEWS_ITEM_RESEARCH:
        if (subject >= 0x10000) {
            // Open ride list window
            window_new_ride_open();

            // Switch to right tab and scroll to ride location
            ride_list_item rideItem;
            rideItem.type = subject >> 8;
            rideItem.entry_index = subject & 0xFF;
            window_new_ride_focus(rideItem);
            break;
        }

        // Check if window is already open
        window = window_bring_to_front_by_class(WC_SCENERY);
        if (window == NULL) {
            window = window_find_by_class(WC_TOP_TOOLBAR);
            if (window != NULL) {
                window_invalidate(window);
                if (!tool_set(window, WC_TOP_TOOLBAR__WIDX_SCENERY, TOOL_ARROW)) {
                    input_set_flag(INPUT_FLAG_6, true);
                    window_scenery_open();
                }
            }
        }

        // Switch to new scenery tab
        window = window_find_by_class(WC_SCENERY);
        if (window != NULL)
            window_event_mouse_down_call(window, WC_SCENERY__WIDX_SCENERY_TAB_1 + subject);
        break;
    case NEWS_ITEM_PEEPS:
        window_guest_list_open_with_filter(GLFT_GUESTS_THINKING_X, subject);;
        break;
    case NEWS_ITEM_AWARD:
        window_park_awards_open();
        break;
    case NEWS_ITEM_GRAPH:
        window_park_rating_open();
        break;
    }
}

/**
 *
 *  rct2: 0x0066E407
 */
void news_item_disable_news(uint8 type, uint32 assoc)
{
    // TODO: write test invalidating windows
    for (sint32 i = 0; i < 11; i++) {
        if (!news_item_is_empty(i)) {
            NewsItem * const newsItem = news_item_get(i);
            if (type == newsItem->Type && assoc == newsItem->Assoc) {
                newsItem->Flags |= NEWS_FLAG_HAS_BUTTON;
                if (i == 0) {
                    window_game_bottom_toolbar_invalidate_news_item();
                }
            }
        } else {
            break;
        }
    }

    for (sint32 i = 11; i < MAX_NEWS_ITEMS; i++) {
        if (!news_item_is_empty(i)) {
            NewsItem * const newsItem = news_item_get(i);
            if (type == newsItem->Type && assoc == newsItem->Assoc) {
                newsItem->Flags |= NEWS_FLAG_HAS_BUTTON;
                window_invalidate_by_class(WC_RECENT_NEWS);
            }
        } else {
            break;
        }
    }
}

void news_item_add_to_queue_custom(NewsItem *newNewsItem)
{
    NewsItem *newsItem = gNewsItems;

    // Find first open slot
    while (newsItem->Type != NEWS_ITEM_NULL) {
        if (newsItem + 1 >= &gNewsItems[10])
            news_item_close_current();
        else
            newsItem++;
    }

    *newsItem = *newNewsItem;
    newsItem++;
    newsItem->Type = NEWS_ITEM_NULL;
}
