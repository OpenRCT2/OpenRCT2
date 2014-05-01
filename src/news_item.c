/*****************************************************************************
 * Copyright (c) 2014 Ted John, Peter Hill
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
#include "audio.h"
#include "news_item.h"
#include "rct2.h"
#include "ride.h"
#include "sprite.h"
#include "window.h"

void window_game_bottom_toolbar_invalidate_news_item();
static int news_item_get_new_history_slot();

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

	window_game_bottom_toolbar_invalidate_news_item();
}

/**
 * 
 *  rct2: 0x0066E252
 */
void news_item_update_current()
{
	short ax, bx, remove_time;
	rct_news_item *newsItems = RCT2_ADDRESS(RCT2_ADDRESS_NEWS_ITEM_LIST, rct_news_item);

	get_system_time();

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

	window_game_bottom_toolbar_invalidate_news_item();

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
	window_game_bottom_toolbar_invalidate_news_item();
}

/**
 * Finds a spare history slot or replaces an existing one if there are no spare
 * slots available.
 */
static int news_item_get_new_history_slot()
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
	rct_car *car;

	switch (type) {
	case NEWS_ITEM_RIDE:
		ride = &(RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride)[subject]);
		if (ride->var_050 == 0xFFFF) {
			*x = SPRITE_LOCATION_NULL;
			break;
		}
		{
			uint32 eax, ebx, ecx, edx, esi, edi, ebp;
			eax = (ride->var_050 & 0xFF) * 32 + 16;
			ecx = (ride->var_050 >> 8) * 32 + 16;
			RCT2_CALLFUNC_X(0x00662783, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
			if (edx & 0xFFFF0000)
				edx >>= 16;
			*x = eax;
			*y = ecx;
			*z = edx;
		}
		break;
	case NEWS_ITEM_PEEP_ON_RIDE:
		peep = &(RCT2_ADDRESS(RCT2_ADDRESS_SPRITE_LIST, rct_sprite)[subject]);
		*x = peep->x;
		*y = peep->y;
		*z = peep->z;
		if (*((uint16*)x) != SPRITE_LOCATION_NULL)
			break;

		if (peep->state != 3 && peep->state != 7) {
			*x = SPRITE_LOCATION_NULL;
			break;
		}

		// Find which ride peep is on
		ride = &(RCT2_ADDRESS(RCT2_ADDRESS_RIDE_LIST, rct_ride)[peep->current_ride]);
		// Check if there are trains on the track (first bit of var_1D0)
		if (!(ride->var_1D0 & 1)) {
			*x = SPRITE_LOCATION_NULL;
			break;
		}

		// Find the train peep is on
		car = &(RCT2_ADDRESS(RCT2_ADDRESS_SPRITE_LIST, rct_sprite)[ride->train_car_map[peep->current_train]]);
		// Find the car peep is on
		for (i = 0; i < peep->current_car; i++)
			car = &(RCT2_ADDRESS(RCT2_ADDRESS_SPRITE_LIST, rct_sprite)[car->next_car]);
		*x = car->x;
		*y = car->y;
		*z = car->z;
		break;
	case NEWS_ITEM_PEEP:
		peep = &(RCT2_ADDRESS(RCT2_ADDRESS_SPRITE_LIST, rct_sprite)[subject]);
		*x = peep->x;
		*y = peep->y;
		*z = peep->z;
		break;
	case NEWS_ITEM_BLANK:
		{
			uint32 eax, ebx, ecx, edx, esi, edi, ebp;
			eax = subject;
			ecx = subject >> 16;
			RCT2_CALLFUNC_X(0x00662783, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
			*x = eax;
			*y = ecx;
			*z = edx;
		}
		break;
	default:
		*x = SPRITE_LOCATION_NULL;
		break;
	}
}
