/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
#pragma once

#include "../Identifiers.h"

#include <cstdint>
#include <list>

struct Vehicle;

namespace TrainManager
{
    // Iteration of heads of trains
    class View
    {
    private:
        const std::list<EntityId>* vec;

        class Iterator
        {
        private:
            std::list<EntityId>::const_iterator iter;
            std::list<EntityId>::const_iterator end;
            Vehicle* Entity = nullptr;

        public:
            Iterator(std::list<EntityId>::const_iterator _iter, std::list<EntityId>::const_iterator _end)
                : iter(_iter)
                , end(_end)
            {
                ++(*this);
            }
            Iterator& operator++();

            Iterator operator++(int)
            {
                Iterator retval = *this;
                ++(*this);
                return retval;
            }
            bool operator==(Iterator other) const
            {
                return Entity == other.Entity;
            }
            bool operator!=(Iterator other) const
            {
                return !(*this == other);
            }
            Vehicle* operator*()
            {
                return Entity;
            }
            // iterator traits
            using difference_type = std::ptrdiff_t;
            using value_type = Vehicle;
            using pointer = const Vehicle*;
            using reference = const Vehicle&;
            using iterator_category = std::forward_iterator_tag;
        };

    public:
        View();

        Iterator begin()
        {
            return Iterator(std::cbegin(*vec), std::cend(*vec));
        }
        Iterator end()
        {
            return Iterator(std::cend(*vec), std::cend(*vec));
        }
    };
} // namespace TrainManager
