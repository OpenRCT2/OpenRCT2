/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../rct12/RCT12.h"
#include "../world/Location.hpp"
#include "EntityBase.h"
#include "EntityRegistry.h"

#include <list>
#include <vector>

enum class EntityListId : uint8_t
{
    Count = 6,
};

const std::vector<uint16_t>& GetEntityList(const EntityType id);

uint16_t GetEntityListCount(EntityType list);
uint16_t GetMiscEntityCount();
uint16_t GetNumFreeEntities();
const std::vector<uint16_t>& GetEntityTileList(const CoordsXY& spritePos);

template<typename T> class EntityTileIterator
{
private:
    std::vector<uint16_t>::const_iterator iter;
    std::vector<uint16_t>::const_iterator end;
    T* Entity = nullptr;

public:
    EntityTileIterator(std::vector<uint16_t>::const_iterator _iter, std::vector<uint16_t>::const_iterator _end)
        : iter(_iter)
        , end(_end)
    {
        ++(*this);
    }
    EntityTileIterator& operator++()
    {
        Entity = nullptr;

        while (iter != end && Entity == nullptr)
        {
            Entity = GetEntity<T>(*iter++);
        }
        return *this;
    }

    EntityTileIterator operator++(int)
    {
        EntityTileIterator retval = *this;
        ++(*this);
        return *iter;
    }
    bool operator==(EntityTileIterator other) const
    {
        return Entity == other.Entity;
    }
    bool operator!=(EntityTileIterator other) const
    {
        return !(*this == other);
    }
    T* operator*()
    {
        return Entity;
    }
    // iterator traits
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = const T*;
    using reference = const T&;
    using iterator_category = std::forward_iterator_tag;
};

template<typename T = EntityBase> class EntityTileList
{
private:
    const std::vector<uint16_t>& vec;

public:
    EntityTileList(const CoordsXY& loc)
        : vec(GetEntityTileList(loc))
    {
    }

    EntityTileIterator<T> begin()
    {
        return EntityTileIterator<T>(std::begin(vec), std::end(vec));
    }
    EntityTileIterator<T> end()
    {
        return EntityTileIterator<T>(std::end(vec), std::end(vec));
    }
};

template<typename T> class EntityListIterator
{
private:
    const std::vector<uint16_t>* vec{};
    int32_t index{};
    T* Entity = nullptr;

public:
    EntityListIterator() = default;
    EntityListIterator(const std::vector<uint16_t>& v, int32_t startIndex)
        : vec(&v)
        , index{ startIndex }
    {
        ++(*this);
    }
    EntityListIterator& operator++()
    {
        Entity = nullptr;
        while (index >= 0 && Entity == nullptr)
        {
            if (index >= vec->size())
            {
                // This can happen if multiple entities were removed at once.
                continue;
            }
            Entity = GetEntity<T>((*vec)[index--]);
        }
        return *this;
    }

    EntityListIterator operator++(int)
    {
        EntityListIterator retval = *this;
        ++(*this);
        return retval;
    }
    bool operator==(EntityListIterator other) const
    {
        return Entity == other.Entity;
    }
    bool operator!=(EntityListIterator other) const
    {
        return !(*this == other);
    }
    T* operator*()
    {
        return Entity;
    }
    // iterator traits
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = const T*;
    using reference = const T&;
    using iterator_category = std::forward_iterator_tag;
};

template<typename T = EntityBase> class EntityList
{
private:
    using EntityListIterator_t = EntityListIterator<T>;
    const std::vector<uint16_t>& vec;

public:
    EntityList()
        : vec(GetEntityList(T::cEntityType))
    {
    }

    EntityListIterator_t begin() const
    {
        if (vec.empty())
            return end();

        return EntityListIterator_t(vec, static_cast<int32_t>(vec.size()) - 1);
    }
    EntityListIterator_t end() const
    {
        return EntityListIterator_t();
    }
};
