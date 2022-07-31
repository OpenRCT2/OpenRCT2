/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Identifiers.h"
#include "../common.h"
#include "../core/String.hpp"

#include <algorithm>
#include <array>
#include <iterator>
#include <optional>
#include <string>

struct CoordsXYZ;
class Formatter;

namespace News
{
    enum class ItemType : uint8_t
    {
        Null,
        Ride,
        PeepOnRide,
        Peep,
        Money,
        Blank,
        Research,
        Peeps,
        Award,
        Graph,
        Count
    };

    constexpr size_t ItemTypeCount = static_cast<size_t>(News::ItemType::Count);

    enum ItemTypeProperty : uint8_t
    {
        HasLocation = 1,
        HasSubject = 2,
    };

    enum ItemFlags : uint8_t
    {
        HasButton = 1 << 0,
    };

    /**
     * A single news item / message.
     */
    struct Item
    {
        News::ItemType Type;
        uint8_t Flags;
        uint32_t Assoc;
        uint16_t Ticks;
        uint16_t MonthYear;
        uint8_t Day;
        std::string Text;

        constexpr bool IsEmpty() const noexcept
        {
            return Type == News::ItemType::Null;
        }

        constexpr uint8_t GetTypeProperties() const
        {
            switch (Type)
            {
                case News::ItemType::Blank:
                    return News::ItemTypeProperty::HasLocation;
                case News::ItemType::Money:
                case News::ItemType::Research:
                case News::ItemType::Peeps:
                case News::ItemType::Award:
                case News::ItemType::Graph:
                    return News::ItemTypeProperty::HasSubject;
                case News::ItemType::Ride:
                case News::ItemType::PeepOnRide:
                case News::ItemType::Peep:
                    return News::ItemTypeProperty::HasLocation | News::ItemTypeProperty::HasSubject;
                case News::ItemType::Null:
                case News::ItemType::Count:
                default:
                    return 0;
            }
        }

        void SetFlags(uint8_t flag)
        {
            Flags |= flag;
        }

        constexpr bool TypeHasSubject() const
        {
            return this->GetTypeProperties() & News::ItemTypeProperty::HasSubject;
        }

        constexpr bool TypeHasLocation() const
        {
            return this->GetTypeProperties() & News::ItemTypeProperty::HasLocation;
        }

        constexpr bool HasButton() const noexcept
        {
            return Flags & News::ItemFlags::HasButton;
        }
    };

    constexpr int32_t ItemHistoryStart = 11;
    constexpr int32_t MaxItemsArchive = 50;
    constexpr int32_t MaxItems = News::ItemHistoryStart + News::MaxItemsArchive;

    template<std::size_t N> class ItemQueue
    {
    public:
        static_assert(N > 0, "Cannot instantiate News::ItemQueue with size=0");

        using value_type = typename std::array<News::Item, N>::value_type;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;
        using iterator = typename std::array<News::Item, N>::iterator;
        using const_iterator = typename std::array<News::Item, N>::const_iterator;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        ItemQueue()
        {
            std::get<0>(Queue).Type = News::ItemType::Null;
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
            return std::get<0>(Queue).IsEmpty();
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
            Queue[N - 1].Type = News::ItemType::Null;
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
                    it->Type = News::ItemType::Null;
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
            front().Type = News::ItemType::Null;
        }

    private:
        std::array<News::Item, N> Queue;
    };

    struct ItemQueues
    {
        News::Item& operator[](size_t index);
        const News::Item& operator[](size_t index) const;
        News::Item* At(int32_t index);
        const News::Item* At(int32_t index) const;
        bool IsEmpty() const;
        void Clear();
        uint16_t IncrementTicks();
        News::Item& Current();
        const News::Item& Current() const;
        bool CurrentShouldBeArchived() const;
        void ArchiveCurrent();
        News::Item* FirstOpenOrNewSlot();
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

        News::ItemQueue<News::ItemHistoryStart> Recent;
        News::ItemQueue<News::MaxItemsArchive> Archived;
    };

    void InitQueue();

    void UpdateCurrentItem();
    void CloseCurrentItem();

    std::optional<CoordsXYZ> GetSubjectLocation(News::ItemType type, int32_t subject);

    News::Item* AddItemToQueue(News::ItemType type, StringId string_id, uint32_t assoc, const Formatter& formatter);
    News::Item* AddItemToQueue(News::ItemType type, StringId string_id, EntityId assoc, const Formatter& formatter);
    News::Item* AddItemToQueue(News::ItemType type, const utf8* text, uint32_t assoc);

    bool CheckIfItemRequiresAssoc(News::ItemType type);

    void OpenSubject(News::ItemType type, int32_t subject);

    void DisableNewsItems(News::ItemType type, uint32_t assoc);

    News::Item* GetItem(int32_t index);

    bool IsQueueEmpty();

    bool IsValidIndex(int32_t index);

    void AddItemToQueue(News::Item* newNewsItem);
    void RemoveItem(int32_t index);
} // namespace News

extern News::ItemQueues gNewsItems;
