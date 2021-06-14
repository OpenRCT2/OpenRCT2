/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once
#include <algorithm>
#include <vector>

template<typename Handle, typename V> class GroupVector
{
    std::vector<std::vector<V>> _data;

public:
    bool Contains(Handle handle, V value)
    {
        if (handle >= _data.size())
            return false;

        const auto& values = _data[handle];
        return std::find(values.begin(), values.end(), value) != values.end();
    }

    void Add(Handle handle, V value)
    {
        if (handle >= _data.size())
        {
            _data.resize(handle + 1);
        }
        auto& values = _data[handle];

        auto it = std::find(values.begin(), values.end(), value);
        if (it != values.end())
            return;

        values.push_back(value);
    }

    void Set(Handle handle, std::vector<V>&& values)
    {
        if (handle >= _data.size())
        {
            _data.resize(handle + 1);
        }
        _data[handle] = values;
    }

    std::vector<V>* GetAll(Handle handle)
    {
        if (handle < _data.size())
        {
            return &_data[handle];
        }
        return nullptr;
    }

    void Clear()
    {
        _data.clear();
    }

    void RemoveHandle(Handle handle)
    {
        if (handle < _data.size())
        {
            _data[handle].clear();
        }
    }

    void RemoveValue(V value)
    {
        for (auto& values : _data)
        {
            values.erase(std::remove_if(values.begin(), values.end(), [value](auto v) { return v == value; }), values.end());
        }
    }
};
