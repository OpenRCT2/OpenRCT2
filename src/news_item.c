/*****************************************************************************
 * Copyright (c) 2014 Ted John
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

#include "addresses.h"
#include "news_item.h"
#include "rct2.h"
#include "window.h"

/**
 *
 *  rct2: 0x0066DF32
 */
void news_item_init_queue()
{
	int i;
	rct_news_item *newsItems = RCT2_ADDRESS(RCT2_ADDRESS_NEWS_ITEM_LIST, rct_news_item);

	newsItems[0].type = NEWS_ITEM_NULL;
	newsItems[11].type = NEWS_ITEM_NULL;
	for (i = 0; i < 16; i++)
		RCT2_ADDRESS(0x01358750, uint8)[i] = 0;

	RCT2_CALLPROC_EBPSAFE(0x0066BB79); // window_game_bottom_toolbar_invalidate_news_item();
}

/**
 * 
 *  rct2: 0x0066E252
 */
void news_item_update_current()
{
	int _ebp;
	short ax, bx, remove_time;
	rct_news_item *newsItems = RCT2_ADDRESS(RCT2_ADDRESS_NEWS_ITEM_LIST, rct_news_item);

	RCT2_CALLPROC_EBPSAFE(0x00407671); // get_system_time()

	ax = RCT2_GLOBAL(RCT2_ADDRESS_OS_TIME_DAY, sint16);
	bx = RCT2_GLOBAL(RCT2_ADDRESS_OS_TIME_MONTH, sint16);
	if (bx != RCT2_GLOBAL(0x009DEA6B, sint16)) {
		bx--;
		if (bx == 0)
			bx = 12;
		if (bx != RCT2_GLOBAL(0x009DEA6B, sint16) || ax == 1) {
			// loc_66E2AE
			RCT2_GLOBAL(0x013573DC, sint32) = 10000;
			if (RCT2_GLOBAL(0x013573DC, sint32) >= 0)
				RCT2_GLOBAL(0x013573DC, sint32) = -RCT2_GLOBAL(0x013573DC, sint32);
		}
	} else {
		if (ax != RCT2_GLOBAL(0x009DEA69, sint16)) {
			ax--;
			if (ax != RCT2_GLOBAL(0x009DEA69, sint16)) {
				// loc_66E2AE
				RCT2_GLOBAL(0x013573DC, sint32) = 10000;
				if (RCT2_GLOBAL(0x013573DC, sint32) >= 0)
					RCT2_GLOBAL(0x013573DC, sint32) = -RCT2_GLOBAL(0x013573DC, sint32);
			}
		}
	}

	RCT2_GLOBAL(0x009DEA69, sint16) = RCT2_GLOBAL(RCT2_ADDRESS_OS_TIME_DAY, sint16);
	RCT2_GLOBAL(0x009DEA6B, sint16) = RCT2_GLOBAL(RCT2_ADDRESS_OS_TIME_MONTH, sint16);

	// Check if there is a current news item
	if (newsItems[0].type == 0)
		return;

	RCT2_CALLPROC_EBPSAFE(0x0066BB79); // window_game_bottom_toolbar_invalidate_news_item();

	// Update the current news item
	newsItems[0].ticks++;
	if (newsItems[0].ticks == 1 && !(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & 1)) {
		// Play sound
		sound_play_panned(39, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16) / 2);
	}

	// Removal of current news item
	remove_time = 320;
	if (newsItems[2].type != 0 &&
		newsItems[3].type != 0 &&
		newsItems[4].type != 0 &&
		newsItems[5].type != 0)
		remove_time = 256;

	if (newsItems[0].ticks >= remove_time)
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
	if (newsItems[0].type == NEWS_ITEM_NULL)
		return;

	// Find an available history news item slot for current message
	i = news_item_get_new_history_slot();

	// Set the history news item slot to the current news item
	newsItems[i] = newsItems[0];

	// Set the end of the end of the history list
	if (i < 60)
		newsItems[i + 1].type = NEWS_ITEM_NULL;

	// Invalidate the news window
	window_invalidate_by_id(WC_RECENT_NEWS, 0);

	// Dequeue the current news item, shift news up
	for (i = 0; i < 10; i++)
		newsItems[i] = newsItems[i + 1];
	newsItems[10].type = NEWS_ITEM_NULL;

	// Invalidate current news item bar
	RCT2_CALLPROC_EBPSAFE(0x0066BB79); // window_game_bottom_toolbar_invalidate_news_item();
}

/**
 * Finds a spare history slot or replaces an existing one if there are no spare
 * slots available.
 */
int news_item_get_new_history_slot()
{
	int i;
	rct_news_item *newsItems = RCT2_ADDRESS(RCT2_ADDRESS_NEWS_ITEM_LIST, rct_news_item);

	// Find an available history news item slot
	for (i = 11; i < 61; i++)
		if (newsItems[i].type == NEWS_ITEM_NULL)
			return i;

	// Dequeue the first history news item, shift history up
	for (i = 11; i < 60; i++)
		newsItems[i] = newsItems[i + 1];
	return 60;
}