/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
#pragma once
#include <cstdint>
#include <vector>

struct Vehicle;

namespace TrainManager
{
    // Iteration of heads of trains
    class View
    {
    private:
        const std::vector<uint16_t>* vec;

        class Iterator
        {
        private:
            const std::vector<uint16_t>* vec{};
            int32_t index{};
            Vehicle* Entity = nullptr;

        public:
            Iterator() = default;
            Iterator(const std::vector<uint16_t>* v, int32_t startIndex)
                : vec(v)
                , index{ startIndex }
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
            if (vec->empty())
                return end();

            return Iterator(vec, static_cast<int32_t>(vec->size()) - 1);
        }
        Iterator end()
        {
            return Iterator();
        }
    };
} // namespace TrainManager
