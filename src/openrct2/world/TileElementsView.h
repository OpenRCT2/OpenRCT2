/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Location.hpp"
#include "Map.h"
#include "TileElement.h"

#include <xutility>

template<typename T = TileElement> class TileElementsView
{
    const CoordsXY _loc;

public:
    struct Iterator
    {
        TileElement* element = nullptr;

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
                    for (;;)
                    {
                        if (static_cast<TileElementType>(element->GetType()) == T::ElementType)
                            break;
                        if (element->IsLastForTile())
                        {
                            element = nullptr;
                            break;
                        }
                        element++;
                    }
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
            if constexpr (std::is_same_v<T, TileElement>)
                return element;
            else
                return element->as<T>();
        }

        T* operator->()
        {
            if constexpr (std::is_same_v<T, TileElement>)
                return element;
            else
                return element->as<T>();
        }

        // iterator traits
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = const T*;
        using reference = const T&;
        using iterator_category = std::forward_iterator_tag;
    };

    TileElementsView(const CoordsXY& loc)
        : _loc(loc)
    {
    }

    Iterator begin()
    {
        auto* element = map_get_first_element_at(_loc);
        if (element == nullptr)
            return end();

        if constexpr (!std::is_same_v<T, TileElement>)
        {
            for (;;)
            {
                if (static_cast<TileElementType>(element->GetType()) == T::ElementType)
                    break;
                if (element->IsLastForTile())
                {
                    element = nullptr;
                    break;
                }
                element++;
            }
        }

        return Iterator{ element };
    }

    Iterator end()
    {
        return Iterator{};
    }
};
