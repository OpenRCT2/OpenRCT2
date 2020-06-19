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

#include <array>
#include <iterator>
#include <optional>

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

template<std::size_t N> class NewsItemQueue
{
public:
    static_assert(N > 0, "Cannot instantiate NewsItemQueue with size=0");

    using value_type = typename std::array<NewsItem, N>::value_type;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator = typename std::array<NewsItem, N>::iterator;
    using const_iterator = typename std::array<NewsItem, N>::const_iterator;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    NewsItemQueue()
    {
        Queue[0].Type = NEWS_ITEM_NULL;
    }

    constexpr iterator begin() noexcept
    {
        return std::begin(Queue);
    }
    constexpr const_iterator begin() const noexcept
    {
        return cbegin();
    }
    constexpr const_iterator cbegin() const noexcept
    {
        return std::cbegin(Queue);
    }
    iterator end() noexcept
    {
        return std::find_if(std::begin(Queue), std::end(Queue), [](const_reference item) { return item.IsEmpty(); });
    }
    const_iterator end() const noexcept
    {
        return cend();
    }
    const_iterator cend() const noexcept
    {
        return std::find_if(std::cbegin(Queue), std::cend(Queue), [](const_reference item) { return item.IsEmpty(); });
    }

    constexpr bool empty() const noexcept
    {
        return Queue[0].IsEmpty();
    }

    size_type size() const noexcept
    {
        return std::distance(cbegin(), cend());
    }

    reference front() noexcept
    {
        return *begin();
    }
    const_reference front() const noexcept
    {
        return *cbegin();
    }
    reference back() noexcept
    {
        return *end();
    }
    const_reference back() const noexcept
    {
        return *cend();
    }

    void pop_front()
    {
        std::move(std::begin(Queue) + 1, std::end(Queue), std::begin(Queue));
        Queue[N - 1].Type = NEWS_ITEM_NULL;
    }

    void push_back(const_reference item)
    {
        auto it = end();
        if (!std::distance(it, std::end(Queue)))
        {
            // Reached queue max size, need to free some space
            pop_front();
            Queue[N - 1] = item;
        }
        else
        {
            *it = item;
            ++it;
            if (std::distance(it, std::end(Queue)))
                it->Type = NEWS_ITEM_NULL;
        }
    }

    reference operator[](size_type n) noexcept
    {
        return Queue[n];
    }
    const_reference operator[](size_type n) const noexcept
    {
        return Queue[n];
    }

    constexpr size_type capacity() const noexcept
    {
        return N;
    }

    void clear() noexcept
    {
        front().Type = NEWS_ITEM_NULL;
    }

private:
    std::array<NewsItem, N> Queue;
};

struct NewsItemQueues
{
    NewsItem& operator[](size_t index);
    const NewsItem& operator[](size_t index) const;
    NewsItem* At(int32_t index);
    const NewsItem* At(int32_t index) const;
    bool IsEmpty() const;
    void Clear();
    uint16_t IncrementTicks();
    NewsItem& Current();
    const NewsItem& Current() const;
    NewsItem& Oldest();
    const NewsItem& Oldest() const;
    bool CurrentShouldBeArchived() const;
    void ArchiveCurrent();
    NewsItem* FirstOpenOrNewSlot();
    const auto& GetRecent() const
    {
        return Recent;
    }
    const auto& GetArchived() const
    {
        return Archived;
    }

    template<typename Predicate> void ForeachRecentNews(Predicate&& p)
    {
        for (auto& newsItem : Recent)
        {
            p(newsItem);
        }
    }

    template<typename Predicate> void ForeachArchivedNews(Predicate&& p)
    {
        for (auto& newsItem : Archived)
        {
            p(newsItem);
        }
    }

private:
    int32_t RemoveTime() const;
    void AppendToArchive(NewsItem& item);

    NewsItemQueue<NEWS_ITEM_HISTORY_START> Recent;
    NewsItemQueue<MAX_NEWS_ITEMS_ARCHIVE> Archived;
};

extern NewsItemQueues gNewsItems;

void news_item_init_queue();

void news_item_update_current();
void news_item_close_current();

std::optional<CoordsXYZ> news_item_get_subject_location(int32_t type, int32_t subject);

NewsItem* news_item_add_to_queue(uint8_t type, rct_string_id string_id, uint32_t assoc);
NewsItem* news_item_add_to_queue_raw(uint8_t type, const utf8* text, uint32_t assoc);

void news_item_open_subject(int32_t type, int32_t subject);

void news_item_disable_news(uint8_t type, uint32_t assoc);

NewsItem* news_item_get(int32_t index);

bool news_item_is_queue_empty();

bool news_item_is_valid_idx(int32_t index);

void news_item_add_to_queue_custom(NewsItem* newNewsItem);
void news_item_remove(int32_t index);
