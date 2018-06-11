/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3
 *****************************************************************************/

#pragma once

#include "../common.h"

enum
{
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

enum
{
    NEWS_TYPE_HAS_LOCATION = 1,
    NEWS_TYPE_HAS_SUBJECT  = 2,
};

enum
{
    NEWS_FLAG_HAS_BUTTON = 1 << 0,
};

/**
 * A single news item / message.
 */
struct NewsItem
{
    uint8  Type;
    uint8  Flags;
    uint32 Assoc;
    uint16 Ticks;
    uint16 MonthYear;
    uint8  Day;
    utf8   Text[256];
};

#define MAX_NEWS_ITEMS 61

extern const uint8 news_type_properties[10];

extern NewsItem gNewsItems[MAX_NEWS_ITEMS];

void news_item_init_queue();

void news_item_update_current();
void news_item_close_current();

void news_item_get_subject_location(sint32 type, sint32 subject, sint32 * x, sint32 * y, sint32 * z);

void news_item_add_to_queue(uint8 type, rct_string_id string_id, uint32 assoc);
void news_item_add_to_queue_raw(uint8 type, const utf8 * text, uint32 assoc);

void news_item_open_subject(sint32 type, sint32 subject);

void news_item_disable_news(uint8 type, uint32 assoc);

NewsItem * news_item_get(sint32 index);

bool news_item_is_empty(sint32 index);
bool news_item_is_queue_empty();

bool news_item_is_valid_idx(sint32 index);

void news_item_add_to_queue_custom(NewsItem * newNewsItem);

