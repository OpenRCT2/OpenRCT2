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

#ifndef _NEWS_ITEM_H_
#define _NEWS_ITEM_H_

#include "../common.h"

enum {
	NEWS_ITEM_NULL,
	NEWS_ITEM_RIDE,
	NEWS_ITEM_PEEP_ON_RIDE,
	NEWS_ITEM_PEEP,
	NEWS_ITEM_MONEY,
	NEWS_ITEM_BLANK,
	NEWS_ITEM_RESEARCH,
	NEWS_ITEM_PEEPS,
	NEWS_ITEM_AWARD,
	NEWS_ITEM_GRAPH
};

/**
 * News item structure.
 * size: 0x10C
 */
typedef struct {
	uint8 type;					// 0x00
	uint8 flags;				// 0x01
	uint32 assoc;				// 0x02
	uint16 ticks;				// 0x06
	uint16 month_year;			// 0x08
	uint8 day;					// 0x0A
	uint8 pad_0B;				// 0x0B
	utf8 text[256];				// 0x0C
} rct_news_item;

void news_item_init_queue();
void news_item_update_current();
void news_item_close_current();
void news_item_get_subject_location(int type, int subject, int *x, int *y, int *z);
void news_item_add_to_queue(uint8 type, rct_string_id string_id, uint32 assoc);
void news_item_add_to_queue_raw(uint8 type, const char *text, uint32 assoc);
void news_item_open_subject(int type, int subject);
void news_item_disable_news(uint8 type, uint32 assoc);
rct_news_item *news_item_get(const uint8 idx);
bool news_item_is_empty(const uint8 idx);
bool news_item_is_queue_empty();
bool news_item_is_valid_idx(const uint8 idx);

#endif
