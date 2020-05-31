/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../world/Location.hpp"

#include <optional>
#include <vector>

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
    NEWS_ITEM_GRAPH,
    NEWS_ITEM_TYPE_COUNT
};

enum
{
    NEWS_TYPE_HAS_LOCATION = 1,
    NEWS_TYPE_HAS_SUBJECT = 2,
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
    uint8_t Type;
    uint8_t Flags;
    uint32_t Assoc;
    uint16_t Ticks;
    uint16_t MonthYear;
    uint8_t Day;
    utf8 Text[256];

    constexpr bool IsEmpty() const noexcept
    {
        return Type == NEWS_ITEM_NULL;
    }
};

constexpr int32_t NEWS_ITEM_HISTORY_START = 11;
constexpr int32_t MAX_NEWS_ITEMS_ARCHIVE = 50;
constexpr int32_t MAX_NEWS_ITEMS = NEWS_ITEM_HISTORY_START + MAX_NEWS_ITEMS_ARCHIVE;

extern const uint8_t news_type_properties[10];

struct NewsItemQueue
{
    NewsItem& operator[](size_t index);
    const NewsItem& operator[](size_t index) const;
    NewsItem* At(int32_t index);
    const NewsItem* At(int32_t index) const;
    bool IsEmpty() const;
    uint16_t IncrementTicks();
    const NewsItem& Current() const;
    void ArchiveCurrent();
    void ArchiveCurrentIfEnoughTicksPassed();
    NewsItem& NewNewsItem();

    template<typename Predicate> void ForeachRecentNews(Predicate&& p)
    {
        for (auto& newsItem : Recent)
        {
            if (newsItem.IsEmpty())
                break;
            p(newsItem);
        }
    }

    template<typename Predicate> void ForeachArchivedNews(Predicate&& p)
    {
        for (auto& newsItem : Archived)
        {
            if (newsItem.IsEmpty())
                break;
            p(newsItem);
        }
    }

private:
    int32_t RemoveTime() const;
    void ArchiveIfFull();
    void AppendToArchive(NewsItem& item);

    NewsItem& RecentAt(size_t index);
    const NewsItem& RecentAt(size_t index) const;
    const NewsItem& ArchivedAt(size_t index) const;

    mutable std::vector<NewsItem> Recent;
    mutable std::vector<NewsItem> Archived;
};

extern NewsItemQueue gNewsItems;

void news_item_init_queue();

void news_item_update_current();
void news_item_close_current();

std::optional<CoordsXYZ> news_item_get_subject_location(int32_t type, int32_t subject);

NewsItem* news_item_add_to_queue(uint8_t type, rct_string_id string_id, uint32_t assoc);
NewsItem* news_item_add_to_queue_raw(uint8_t type, const utf8* text, uint32_t assoc);

void news_item_open_subject(int32_t type, int32_t subject);

void news_item_disable_news(uint8_t type, uint32_t assoc);

NewsItem* news_item_get(int32_t index);

bool news_item_is_empty(int32_t index);
bool news_item_is_queue_empty();

bool news_item_is_valid_idx(int32_t index);

void news_item_add_to_queue_custom(NewsItem* newNewsItem);
void news_item_remove(int32_t index);
