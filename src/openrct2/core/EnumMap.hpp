/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
#pragma once

#include <algorithm>
#include <array>
#include <string_view>
#include <vector>

/**
 * Bi-directional map for converting between strings and enums / numbers.
 */
template<typename T> class EnumMap
{
private:
    std::vector<std::pair<std::string_view, T>> _map;
    bool _continiousValueIndex{ false };

    static constexpr size_t BucketSize = 43;
    std::array<std::vector<int32_t>, BucketSize> _buckets;

    static constexpr bool ValueIndexable()
    {
        if constexpr (std::is_enum_v<T>)
            return true;
        else if constexpr (std::is_integral_v<T>)
            return true;
        else
            return false;
    }

    static constexpr auto ValueDistance(T a, T b)
    {
        if constexpr (std::is_enum_v<T>)
            return static_cast<std::underlying_type_t<T>>(b) - static_cast<std::underlying_type_t<T>>(a);
        else if constexpr (std::is_integral_v<T>)
            return (b - a);
    }

    static constexpr uint32_t MakeHash(const std::string_view str)
    {
        uint32_t res = 0x811c9dc5;
        for (auto chr : str)
        {
            res ^= chr;
            res *= 0x01000193;
        }
        return res;
    }

public:
    EnumMap(const std::initializer_list<std::pair<std::string_view, T>>&& items)
        : _map{ items }
    {
        std::sort(_map.begin(), _map.end(), [](const auto& a, const auto& b) { return a.second < b.second; });

        if constexpr (ValueIndexable())
        {
            _continiousValueIndex = true;
            T cur{};
            for (size_t i = 1; i < _map.size(); i++)
            {
                auto nextVal = _map[i].second;
                auto dist = ValueDistance(cur, _map[i].second);
                if (dist != 1)
                {
                    _continiousValueIndex = false;
                    break;
                }
                cur = nextVal;
            }
        }

        int32_t index = 0;
        for (auto& kv : _map)
        {
            auto hash = MakeHash(kv.first);
            auto bucketIndex = hash % BucketSize;
            auto& bucket = _buckets[bucketIndex];
            bucket.push_back(index);
            index++;
        }
    }

    std::string_view operator[](T k) const
    {
        auto it = find(k);
        return it->first;
    }

    T operator[](std::string_view k) const
    {
        auto it = find(k);
        return it->second;
    }

    auto find(const std::string_view k) const
    {
        const auto hash = MakeHash(k);
        const auto bucketIndex = hash % BucketSize;
        const auto& bucket = _buckets[bucketIndex];

        for (auto index : bucket)
        {
            auto& entry = _map[index];
            if (entry.first == k)
            {
                return _map.begin() + index;
            }
        }

        return end();
    }

    auto find(const T k) const
    {
        const auto binarySearchValue = [&]() {
            auto it = std::lower_bound(_map.begin(), _map.end(), k, [](const auto& a, const auto& b) { return a.second < b; });
            if (it == _map.end() || it->second != k)
                return end();

            return it;
        };

        if constexpr (ValueIndexable())
        {
            if (_continiousValueIndex)
            {
                auto index = static_cast<size_t>(k);
                return _map.begin() + index;
            }

            return binarySearchValue();
        }
        else
        {
            return binarySearchValue();
        }
    }

    auto begin() const
    {
        return _map.begin();
    }

    auto end() const
    {
        return _map.end();
    }
};
