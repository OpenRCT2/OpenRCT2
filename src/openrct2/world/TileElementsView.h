/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Location.hpp"
#include "Map.h"

#include <iterator>

namespace OpenRCT2
{
    namespace Detail
    {
        template<typename T, typename T2>
        T* NextMatchingTile(T2* element)
        {
            if (element == nullptr)
                return nullptr;

            for (;;)
            {
                auto* res = element->template as<T>();
                if (res != nullptr)
                    return res;

                if (element->IsLastForTile())
                {
                    break;
                }
                element++;
            }

            return nullptr;
        }
    } // namespace Detail

    template<typename T = TileElement>
    class TileElementsView
    {
        const TileCoordsXY _loc;

    public:
        struct Iterator
        {
            T* element = nullptr;

            Iterator& operator++()
            {
                if (element == nullptr)
                    return *this;

                if (element->IsLastForTile())
                {
                    element = nullptr;
                }
                else
                {
                    element++;
                    if constexpr (!std::is_same_v<T, TileElement>)
                    {
                        element = Detail::NextMatchingTile<T>(element);
                    }
                }

                return *this;
            }

            Iterator operator++(int)
            {
                Iterator res = *this;
                ++(*this);
                return res;
            }

            bool operator==(Iterator other) const
            {
                return element == other.element;
            }

            bool operator!=(Iterator other) const
            {
                return !(*this == other);
            }

            T* operator*()
            {
                return element;
            }

            const T* operator*() const
            {
                return element;
            }

            // iterator traits
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = const T*;
            using reference = const T&;
            using iterator_category = std::forward_iterator_tag;
        };

        TileElementsView(const TileCoordsXY& loc)
            : _loc(loc)
        {
        }

        TileElementsView(const CoordsXY& loc)
            : _loc(loc)
        {
        }

        Iterator begin() noexcept
        {
            T* element = reinterpret_cast<T*>(MapGetFirstElementAt(_loc));

            if constexpr (!std::is_same_v<T, TileElement>)
            {
                element = Detail::NextMatchingTile<T>(element);
            }

            return Iterator{ element };
        }

        Iterator end() noexcept
        {
            return Iterator{ nullptr };
        }
    };

} // namespace OpenRCT2
