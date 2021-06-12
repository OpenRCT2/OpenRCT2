/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once
#include <unordered_map>
#include <vector>

template<typename K, typename V> class GroupVector
{
    std::unordered_map<K, std::vector<V>> _history;

public:
    bool Contains(K key, V value)
    {
        auto it = _history.find(key);
        if (it == _history.end())
            return false;

        const auto& rides = it->second;
        return std::find(rides.begin(), rides.end(), value) != rides.end();
    }

    void Add(K key, V value)
    {
        auto& values = _history[key];

        auto it = std::find(values.begin(), values.end(), value);
        if (it != values.end())
            return;

        values.push_back(value);
    }

    void Set(K key, std::vector<V>&& values)
    {
        _history[key] = values;
    }

    void Clear()
    {
        _history.clear();
    }

    void RemoveKey(K key)
    {
        _history.erase(key);
    }

    void RemoveValue(V value)
    {
        for (auto& [_, values] : _history)
        {
            values.erase(std::remove_if(values.begin(), values.end(), [value](auto v) { return v == value; }), values.end());
        }
    }
};
