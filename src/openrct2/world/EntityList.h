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
#include "Entity.h"
#include "Location.hpp"
#include "SpriteBase.h"

#include <list>
#include <vector>

enum class EntityListId : uint8_t
{
    Free,
    TrainHead,
    Peep,
    Misc,
    Litter,
    Vehicle,
    Count,
};

const std::list<uint16_t>& GetEntityList(const EntityListId id);

uint16_t GetEntityListCount(EntityListId list);
uint16_t GetNumFreeEntities();
void RebuildEntityLists();
const std::vector<uint16_t>& GetEntityTileList(const CoordsXY& spritePos);

template<typename T, uint16_t SpriteBase::*NextList> class EntityIterator
{
private:
    T* Entity = nullptr;
    uint16_t NextEntityId = SPRITE_INDEX_NULL;

public:
    EntityIterator(const uint16_t _EntityId)
        : NextEntityId(_EntityId)
    {
        ++(*this);
    }
    EntityIterator& operator++()
    {
        Entity = nullptr;

        while (NextEntityId != SPRITE_INDEX_NULL && Entity == nullptr)
        {
            auto baseEntity = GetEntity(NextEntityId);
            if (!baseEntity)
            {
                NextEntityId = SPRITE_INDEX_NULL;
                continue;
            }
            NextEntityId = baseEntity->*NextList;
            Entity = baseEntity->template As<T>();
        }
        return *this;
    }

    EntityIterator operator++(int)
    {
        EntityIterator retval = *this;
        ++(*this);
        return retval;
    }
    bool operator==(EntityIterator other) const
    {
        return Entity == other.Entity;
    }
    bool operator!=(EntityIterator other) const
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

template<typename T = SpriteBase> class EntityTileList
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
    std::list<uint16_t>::const_iterator iter;
    std::list<uint16_t>::const_iterator end;
    T* Entity = nullptr;

public:
    EntityListIterator(std::list<uint16_t>::const_iterator _iter, std::list<uint16_t>::const_iterator _end)
        : iter(_iter)
        , end(_end)
    {
        ++(*this);
    }
    EntityListIterator& operator++()
    {
        Entity = nullptr;

        while (iter != end && Entity == nullptr)
        {
            Entity = GetEntity<T>(*iter++);
        }
        return *this;
    }

    EntityListIterator operator++(int)
    {
        EntityListIterator retval = *this;
        ++(*this);
        return *iter;
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

template<typename T = SpriteBase> class EntityList
{
private:
    using EntityListIterator_t = EntityListIterator<T>;
    const std::list<uint16_t>& vec;

public:
    EntityList(EntityListId type)
        : vec(GetEntityList(type))
    {
    }

    EntityListIterator_t begin()
    {
        return EntityListIterator_t(std::cbegin(vec), std::cend(vec));
    }
    EntityListIterator_t end()
    {
        return EntityListIterator_t(std::cend(vec), std::cend(vec));
    }
};
