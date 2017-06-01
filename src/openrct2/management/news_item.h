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

enum {
	NEWS_TYPE_HAS_LOCATION = 1,
	NEWS_TYPE_HAS_SUBJECT = 2,
};

enum {
	NEWS_FLAG_HAS_BUTTON = 1 << 0,
};

extern const uint8 news_type_properties[10];

/**
 * A single news item / message.
 */
typedef struct NewsItem {
	uint8   Type;
	uint8   Flags;
	uint32  Assoc;
	uint16  Ticks;
	uint16  MonthYear;
	uint8   Day;
	utf8    Text[256];
} NewsItem;

#define MAX_NEWS_ITEMS 61

extern NewsItem gNewsItems[MAX_NEWS_ITEMS];

void news_item_init_queue();
void news_item_update_current();
void news_item_close_current();
void news_item_get_subject_location(sint32 type, sint32 subject, sint32 *x, sint32 *y, sint32 *z);
void news_item_add_to_queue(uint8 type, rct_string_id string_id, uint32 assoc);
void news_item_add_to_queue_raw(uint8 type, const utf8 *text, uint32 assoc);
void news_item_open_subject(sint32 type, sint32 subject);
void news_item_disable_news(uint8 type, uint32 assoc);
NewsItem *news_item_get(sint32 index);
bool news_item_is_empty(sint32 index);
bool news_item_is_queue_empty();
bool news_item_is_valid_idx(sint32 index);

void news_item_add_to_queue_custom(NewsItem *newNewsItem);

#endif
