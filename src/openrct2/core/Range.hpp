/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cmath>
#include <type_traits>

template<typename T>
struct Range
{
    static_assert(std::is_integral<T>(), "type must be integral");

    class Iterator
    {
        friend Range;

    private:
        T Lower{};
        T Upper{};
        T Value{};
        T Change{};

    private:
        Iterator(const Range& range, T initialValue)
            : Lower(range.Lower)
            , Upper(range.Upper)
            , Value(initialValue)
            , Change(range.Upper >= range.Lower ? 1 : -1)
        {
        }

    public:
        Iterator& operator++()
        {
            Value += Change;
            return *this;
        }

        Iterator operator++(int)
        {
            auto result = *this;
            ++(*this);
            return result;
        }

        bool operator==(Iterator other) const
        {
            return Value == other.Value;
        }

        bool operator!=(Iterator other) const
        {
            return !(*this == other);
        }

        const T& operator*()
        {
            return Value;
        }
    };

    T Lower{};
    T Upper{};

    Range() = default;

    Range(T single)
        : Lower(single)
        , Upper(single)
    {
    }

    Range(T lower, T upper)
        : Lower(lower)
        , Upper(upper)
    {
    }

    size_t size() const
    {
        return std::abs(Upper - Lower);
    }

    Iterator begin()
    {
        return Iterator(*this, Lower);
    }

    Iterator end()
    {
        return Iterator(*this, Upper + 1);
    }

    Iterator begin() const
    {
        return Iterator(*this, Lower);
    }

    Iterator end() const
    {
        return Iterator(*this, Upper + 1);
    }
};
