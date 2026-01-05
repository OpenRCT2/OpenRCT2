/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Identifiers.h"
#include "../core/StringTypes.h"
#include "../localisation/StringIdType.h"

#include <algorithm>
#include <array>
#include <iterator>
#include <optional>
#include <span>
#include <string>

struct CoordsXYZ;

namespace OpenRCT2
{
    class Formatter;
    struct GameState_t;
} // namespace OpenRCT2

namespace OpenRCT2::News
{
    enum class ItemType : uint8_t
    {
        null,
        ride,
        peepOnRide,
        peep,
        money,
        blank,
        research,
        peeps,
        award,
        graph,
        campaign,
        count
    };

    constexpr size_t ItemTypeCount = static_cast<size_t>(News::ItemType::count);

    enum ItemTypeProperty : uint8_t
    {
        hasLocation = 1,
        hasSubject = 2,
    };

    enum ItemFlags : uint8_t
    {
        hasButton = 1 << 0,
    };

    /**
     * A single news item / message.
     */
    struct Item
    {
        News::ItemType type = News::ItemType::null;
        uint8_t flags{};
        uint32_t assoc{};
        uint16_t ticks{};
        uint16_t monthYear{};
        uint8_t day{};
        std::string text{};

        constexpr bool isEmpty() const noexcept
        {
            return type == News::ItemType::null;
        }

        constexpr uint8_t getTypeProperties() const
        {
            switch (type)
            {
                case News::ItemType::blank:
                    return News::ItemTypeProperty::hasLocation;
                case News::ItemType::money:
                case News::ItemType::research:
                case News::ItemType::peeps:
                case News::ItemType::award:
                case News::ItemType::graph:
                case News::ItemType::campaign:
                    return News::ItemTypeProperty::hasSubject;
                case News::ItemType::ride:
                case News::ItemType::peepOnRide:
                case News::ItemType::peep:
                    return News::ItemTypeProperty::hasLocation | News::ItemTypeProperty::hasSubject;
                case News::ItemType::null:
                case News::ItemType::count:
                default:
                    return 0;
            }
        }

        void setFlags(uint8_t flag)
        {
            flags |= flag;
        }

        constexpr bool typeHasSubject() const
        {
            return this->getTypeProperties() & News::ItemTypeProperty::hasSubject;
        }

        constexpr bool typeHasLocation() const
        {
            return this->getTypeProperties() & News::ItemTypeProperty::hasLocation;
        }

        constexpr bool hasButton() const noexcept
        {
            return flags & News::ItemFlags::hasButton;
        }
    };

    constexpr int32_t ItemHistoryStart = 11;
    constexpr int32_t MaxItemsArchive = 50;
    constexpr int32_t MaxItems = News::ItemHistoryStart + News::MaxItemsArchive;

    template<std::size_t N>
    class ItemQueue
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
            std::get<0>(Queue).type = News::ItemType::null;
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
            return std::find_if(std::begin(Queue), std::end(Queue), [](const_reference item) { return item.isEmpty(); });
        }
        const_iterator end() const noexcept
        {
            return cend();
        }
        const_iterator cend() const noexcept
        {
            return std::find_if(std::cbegin(Queue), std::cend(Queue), [](const_reference item) { return item.isEmpty(); });
        }

        constexpr bool empty() const noexcept
        {
            return std::get<0>(Queue).isEmpty();
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
            Queue[N - 1].type = News::ItemType::null;
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
                    it->type = News::ItemType::null;
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
            std::fill(Queue.begin(), Queue.end(), News::Item{});
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

        template<typename Predicate>
        void ForeachRecentNews(Predicate&& p)
        {
            for (auto& newsItem : Recent)
            {
                p(newsItem);
            }
        }

        template<typename Predicate>
        void ForeachArchivedNews(Predicate&& p)
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

    void InitQueue(GameState_t& gameState);

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

    void importNewsItems(
        GameState_t& gameState, const std::span<const News::Item> recent, const std::span<const News::Item> archived);
} // namespace OpenRCT2::News
