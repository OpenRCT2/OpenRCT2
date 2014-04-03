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