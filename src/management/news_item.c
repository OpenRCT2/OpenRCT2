/*****************************************************************************
 * Copyright (c) 2014 Ted John, Peter Hill, Matthias Lanzinger
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * This file is part of OpenRCT2.
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#include "../addresses.h"
#include "../audio/audio.h"
#include "../input.h"
#include "../interface/window.h"
#include "../localisation/date.h"
#include "../localisation/localisation.h"
#include "../ride/ride.h"
#include "../world/sprite.h"
#include "news_item.h"

rct_news_item *gNewsItems = RCT2_ADDRESS(RCT2_ADDRESS_NEWS_ITEM_LIST, rct_news_item);
void window_game_bottom_toolbar_invalidate_news_item();
static int news_item_get_new_history_slot();

#define MAX_NEWS 60

bool news_item_is_valid_idx(const uint8 idx)
{
	if (idx > MAX_NEWS)
	{
		log_error("Tried to get news item past MAX_NEWS.");
		return false;
	}
	return true;
}

rct_news_item *news_item_get(const uint8 idx)
{
	if (news_item_is_valid_idx(idx))
	{
		return &gNewsItems[idx];
	} else {
		return NULL;
	}
}

bool news_item_is_empty(const uint8 idx)
{
	return news_item_get(idx)->type == NEWS_ITEM_NULL;
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
	int i;

	news_item_get(0)->type = NEWS_ITEM_NULL;
	news_item_get(11)->type = NEWS_ITEM_NULL;
	// Throttles for warning types (PEEP_*_WARNING)
	for (i = 0; i < 16; i++)
		RCT2_ADDRESS(0x01358750, uint8)[i] = 0;

	window_game_bottom_toolbar_invalidate_news_item();
}

static void news_item_tick_current()
{
	int ticks;
	ticks = news_item_get(0)->ticks++;
	if (ticks == 1 && !(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 1)) {
		// Play sound
		sound_play_panned(SOUND_NEWS_ITEM, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16) / 2, 0, 0, 0);
	}
}

static bool news_item_is_current_old()
{
	int remove_time = 320;
	if (!news_item_is_empty(5) &&
		!news_item_is_empty(4) &&
		!news_item_is_empty(3) &&
		!news_item_is_empty(2))
		remove_time = 256;

	if (news_item_get(0)->ticks >= remove_time)
		return true;

	return false;
}

/**
 * 
 *  rct2: 0x0066E252
 */
void news_item_update_current()
{
	short ax, bx;

	get_system_time();

	ax = RCT2_GLOBAL(RCT2_ADDRESS_OS_TIME_DAY, sint16);
	bx = RCT2_GLOBAL(RCT2_ADDRESS_OS_TIME_MONTH, sint16);

	// Cheat detection
	if (bx != RCT2_GLOBAL(0x009DEA6B, sint16)) {
		bx--;
		if (bx == 0)
			bx = 12;
		if (bx != RCT2_GLOBAL(0x009DEA6B, sint16) || ax != 1) {
			// loc_66E2AE
			RCT2_GLOBAL(0x013573DC, sint32) -= 10000;
			if (RCT2_GLOBAL(0x013573DC, sint32) >= 0)
				RCT2_GLOBAL(0x013573DC, sint32) = -RCT2_GLOBAL(0x013573DC, sint32);
		}
	} else {
		if (ax != RCT2_GLOBAL(0x009DEA69, sint16)) {
			ax--;
			if (ax != RCT2_GLOBAL(0x009DEA69, sint16)) {
				// loc_66E2AE
				RCT2_GLOBAL(0x013573DC, sint32) -= 10000;
				if (RCT2_GLOBAL(0x013573DC, sint32) >= 0)
					RCT2_GLOBAL(0x013573DC, sint32) = -RCT2_GLOBAL(0x013573DC, sint32);
			}
		}
	}

	RCT2_GLOBAL(0x009DEA69, sint16) = RCT2_GLOBAL(RCT2_ADDRESS_OS_TIME_DAY, sint16);
	RCT2_GLOBAL(0x009DEA6B, sint16) = RCT2_GLOBAL(RCT2_ADDRESS_OS_TIME_MONTH, sint16);

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
	int i;
	rct_news_item *newsItems = RCT2_ADDRESS(RCT2_ADDRESS_NEWS_ITEM_LIST, rct_news_item);

	// Check if there is a current message
	if (news_item_is_queue_empty())
		return;

	// Find an available history news item slot for current message
	i = news_item_get_new_history_slot();

	// Set the history news item slot to the current news item
	newsItems[i] = newsItems[0];

	// Set the end of the end of the history list
	if (i < MAX_NEWS)
		newsItems[i + 1].type = NEWS_ITEM_NULL;

	// Invalidate the news window
	window_invalidate_by_class(WC_RECENT_NEWS);

	// Dequeue the current news item, shift news up
	for (i = 0; i < 10; i++)
		newsItems[i] = newsItems[i + 1];
	newsItems[10].type = NEWS_ITEM_NULL;

	// Invalidate current news item bar
	window_game_bottom_toolbar_invalidate_news_item();
}

static void news_item_shift_history_up()
{
	const int history_idx = 11;
	rct_news_item *history_start = news_item_get(history_idx);
	const size_t count = sizeof(rct_news_item) * (MAX_NEWS - 1 - history_idx);
	memmove(history_start, history_start + 1, count);
}


/**
 * Finds a spare history slot or replaces an existing one if there are no spare
 * slots available.
 */
static int news_item_get_new_history_slot()
{
	int i;

	// Find an available history news item slot
	for (i = 11; i <= MAX_NEWS; i++)
		if (news_item_is_empty(i))
			return i;

	// Dequeue the first history news item, shift history up
	news_item_shift_history_up();
	return MAX_NEWS;
}

/**
 * Get the (x,y,z) coordinates of the subject of a news item.
 * If the new item is no longer valid, return SPRITE_LOCATION_NULL in the x-coordinate
 *
 *  rct2: 0x0066BA74
 */
void news_item_get_subject_location(int type, int subject, int *x, int *y, int *z)
{
	int i;
	rct_ride *ride;
	rct_peep *peep;
	rct_vehicle *vehicle;

	switch (type) {
	case NEWS_ITEM_RIDE:
		ride = &g_ride_list[subject];
		if (ride->overall_view == 0xFFFF) {
			*x = SPRITE_LOCATION_NULL;
			break;
		}
		*x = (ride->overall_view & 0xFF) * 32 + 16;
		*y = (ride->overall_view >> 8) * 32 + 16;
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
		ride = &g_ride_list[peep->current_ride];
		if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK)) {
			*x = SPRITE_LOCATION_NULL;
			break;
		}

		// Find the first car of the train peep is on
		vehicle = &(g_sprite_list[ride->vehicles[peep->current_train]]).vehicle;
		// Find the actual car peep is on
		for (i = 0; i < peep->current_car; i++)
			vehicle = &(g_sprite_list[vehicle->next_vehicle_on_train]).vehicle;
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
 * rct2: 0x0066DF55
 *
 * @param  a (al)
 * @param string_id (ebx)
 * @param c (ecx)
 **/
void news_item_add_to_queue(uint8 type, rct_string_id string_id, uint32 assoc)
{
	char *buffer = (char*)0x0141EF68;
	void *args = (void*)0x013CE952;

	format_string(buffer, string_id, args); // overflows possible?
	news_item_add_to_queue_raw(type, buffer, assoc);
}

void news_item_add_to_queue_raw(uint8 type, const char *text, uint32 assoc)
{
	int i = 0;
	rct_news_item *newsItem = RCT2_ADDRESS(RCT2_ADDRESS_NEWS_ITEM_LIST, rct_news_item);

	// find first open slot
	while (newsItem->type != NEWS_ITEM_NULL) {
		if (newsItem + 1 >= (rct_news_item*)0x13CB1CC) // &news_list[10]
			news_item_close_current();
		else
			newsItem++;
	}

	//now we have found an item slot to place the new news in
	newsItem->type = type;
	newsItem->flags = 0;
	newsItem->assoc = assoc;
	newsItem->ticks = 0;
	newsItem->month_year = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, uint16);
	newsItem->day = ((days_in_month[(newsItem->month_year & 7)] * RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_TICKS, uint16)) >> 16) + 1;
	strncpy(newsItem->text, text, 255);
	newsItem->text[254] = 0;

	// blatant disregard for what happens on the last element.
	// Change this when we implement the queue ourselves.
	newsItem++;
	newsItem->type = 0;
}

/**
 * Opens the window/tab for the subject of the news item
 *
 * rct2: 0x0066EBE6
 *
 **/
void news_item_open_subject(int type, int subject)
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
				if (!tool_set(window, 9, 0)) {
					RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) |= INPUT_FLAG_6;
					window_scenery_open();
				}
			}
		}
		
		// Switch to new scenery tab
		window = window_find_by_class(WC_SCENERY);
		if (window != NULL)
			window_event_mouse_down_call(window, 4 + subject);
		break;
	case NEWS_ITEM_PEEPS:
		// Open guest list to right tab
		window_guest_list_open_with_filter(3, subject);;
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
 * rct2: 0x0066E407
 */
void news_item_disable_news(uint8 type, uint32 assoc) {
		// TODO: write test invalidating windows
		int i;
		for (i = 0; i < 11; i++)
		{
			if (!news_item_is_empty(i))
			{
				rct_news_item * const newsItem = news_item_get(i);
                if (type == newsItem->type && assoc == newsItem->assoc) {
                        newsItem->flags |= 0x1;
                        if (i == 0) {
                                window_game_bottom_toolbar_invalidate_news_item();
                        }
                }
			} else {
				break;
			}
        }

		for (i = 11; i <= MAX_NEWS; i++)
		{
			if (!news_item_is_empty(i))
			{
				rct_news_item * const newsItem = news_item_get(i);
                if (type == newsItem->type && assoc == newsItem->assoc) {
                        newsItem->flags |= 0x1;
                        window_invalidate_by_class(WC_RECENT_NEWS);
                }
			} else {
				break;
			}
        }
}

