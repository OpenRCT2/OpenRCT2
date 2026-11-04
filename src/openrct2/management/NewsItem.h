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

    namespace Park
    {
        struct ParkData;
    }
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

    constexpr size_t kItemTypeCount = static_cast<size_t>(ItemType::count);

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
        ItemType type = ItemType::null;
        uint8_t flags{};
        uint32_t assoc{};
        uint16_t ticks{};
        uint16_t monthYear{};
        uint8_t day{};
        std::string text{};

        constexpr bool isEmpty() const noexcept
        {
            return type == ItemType::null;
        }

        constexpr uint8_t getTypeProperties() const
        {
            switch (type)
            {
                case ItemType::blank:
                    return hasLocation;
                case ItemType::money:
                case ItemType::research:
                case ItemType::peeps:
                case ItemType::award:
                case ItemType::graph:
                case ItemType::campaign:
                    return hasSubject;
                case ItemType::ride:
                case ItemType::peepOnRide:
                case ItemType::peep:
                    return hasLocation | hasSubject;
                case ItemType::null:
                case ItemType::count:
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
            return this->getTypeProperties() & hasSubject;
        }

        constexpr bool typeHasLocation() const
        {
            return this->getTypeProperties() & hasLocation;
        }

        constexpr bool hasButton() const noexcept
        {
            return flags & ItemFlags::hasButton;
        }
    };

    constexpr int32_t kNewsQueueSize = 11;
    constexpr int32_t kNewsArchiveSize = 50;

    template<std::size_t N>
    class ItemCollection
    {
    public:
        static_assert(N > 0, "Cannot instantiate News::ItemQueue with size=0");

        using value_type = std::array<Item, N>::value_type;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;
        using iterator = std::array<Item, N>::iterator;
        using const_iterator = std::array<Item, N>::const_iterator;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        ItemCollection()
        {
            std::get<0>(_queue).type = ItemType::null;
        }

        constexpr iterator begin() noexcept
        {
            return std::begin(_queue);
        }
        constexpr const_iterator begin() const noexcept
        {
            return cbegin();
        }
        constexpr const_iterator cbegin() const noexcept
        {
            return std::cbegin(_queue);
        }
        iterator end() noexcept
        {
            return std::find_if(std::begin(_queue), std::end(_queue), [](const_reference item) { return item.isEmpty(); });
        }
        const_iterator end() const noexcept
        {
            return cend();
        }
        const_iterator cend() const noexcept
        {
            return std::find_if(std::cbegin(_queue), std::cend(_queue), [](const_reference item) { return item.isEmpty(); });
        }

        constexpr bool empty() const noexcept
        {
            return std::get<0>(_queue).isEmpty();
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
            std::move(std::begin(_queue) + 1, std::end(_queue), std::begin(_queue));
            _queue[N - 1].type = ItemType::null;
        }

        void push_back(const_reference item)
        {
            auto it = end();
            if (!std::distance(it, std::end(_queue)))
            {
                // Reached queue max size, need to free some space
                pop_front();
                _queue[N - 1] = item;
            }
            else
            {
                *it = item;
                ++it;
                if (std::distance(it, std::end(_queue)))
                    it->type = ItemType::null;
            }
        }

        reference operator[](size_type n) noexcept
        {
            return _queue[n];
        }
        const_reference operator[](size_type n) const noexcept
        {
            return _queue[n];
        }

        constexpr size_type capacity() const noexcept
        {
            return N;
        }

        void clear() noexcept
        {
            std::fill(_queue.begin(), _queue.end(), Item{});
        }

        const_pointer at(int32_t index) const
        {
            if (_queue[index].type == ItemType::null)
                return nullptr;

            return &_queue[index];
        }

        void remove(int32_t index)
        {
            // News item is already null, no need to remove it
            if (_queue[index].type == ItemType::null)
                return;

            for (size_t i = index; i < N - 1; i++)
            {
                _queue[i] = _queue[i + 1];
            }
            _queue[N - 1].type = ItemType::null;
        }

        void importQueue(const std::span<const value_type> src)
        {
            clear();

            for (size_t i = 0; i < std::min<size_t>(src.size(), N); i++)
            {
                _queue[i] = src[i];
            }
        }

    private:
        std::array<Item, N> _queue;
    };

    using NewsArchive = ItemCollection<kNewsArchiveSize>;
    using NewsQueue = ItemCollection<kNewsQueueSize>;

    struct Items
    {
        NewsQueue queue;
        NewsArchive archive;

        void clear();

        void updateActiveItem();
        void archiveActiveItem();

        const Item* getActiveMessage() const;

        Item* addItemToQueue(ItemType type, StringId string_id, uint32_t assoc, const Formatter& formatter);
        Item* addItemToQueue(ItemType type, StringId string_id, EntityId assoc, const Formatter& formatter);
        Item* addItemToQueue(ItemType type, const utf8* text, uint32_t assoc);

    private:
        Item* firstOpenOrNewSlot();
        int32_t removeTime() const;

        bool activeItemShouldBeArchived() const;
        void tickActiveItem();
    };

    void InitQueue(Park::ParkData& park);

    std::optional<CoordsXYZ> GetSubjectLocation(ItemType type, int32_t subject);

    bool CheckIfItemRequiresAssoc(ItemType type);

    void OpenSubject(ItemType type, int32_t subject);

    void DisableNewsItems(Items& newsItems, ItemType type, uint32_t assoc);
} // namespace OpenRCT2::News
