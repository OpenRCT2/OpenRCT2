/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
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
    uint8_t  Type;
    uint8_t  Flags;
    uint32_t Assoc;
    uint16_t Ticks;
    uint16_t MonthYear;
    uint8_t  Day;
    utf8   Text[256];
};

#define MAX_NEWS_ITEMS 61

extern const uint8_t news_type_properties[10];

extern NewsItem gNewsItems[MAX_NEWS_ITEMS];

void news_item_init_queue();

void news_item_update_current();
void news_item_close_current();

void news_item_get_subject_location(int32_t type, int32_t subject, int32_t * x, int32_t * y, int32_t * z);

void news_item_add_to_queue(uint8_t type, rct_string_id string_id, uint32_t assoc);
void news_item_add_to_queue_raw(uint8_t type, const utf8 * text, uint32_t assoc);

void news_item_open_subject(int32_t type, int32_t subject);

void news_item_disable_news(uint8_t type, uint32_t assoc);

NewsItem * news_item_get(int32_t index);

bool news_item_is_empty(int32_t index);
bool news_item_is_queue_empty();

bool news_item_is_valid_idx(int32_t index);

void news_item_add_to_queue_custom(NewsItem * newNewsItem);

