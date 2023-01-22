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
#include <cstdint>
#include <cstdio>
#include <vector>
namespace OpenRCT2
{
    template<typename Handle, typename V> class GroupVector
    {
        std::vector<std::vector<V>> _data;

    public:
        bool Contains(Handle handle, V value)
        {
            const auto index = static_cast<size_t>(handle);
            if (index >= _data.size())
                return false;

            const auto& values = _data[index];
            return std::find(values.begin(), values.end(), value) != values.end();
        }

        void Add(Handle handle, V value)
        {
            const auto index = static_cast<size_t>(handle);
            if (index >= _data.size())
            {
                _data.resize(index + 1);
            }
            auto& values = _data[index];

            auto it = std::find(values.begin(), values.end(), value);
            if (it != values.end())
                return;

            values.push_back(value);
        }

        void Set(Handle handle, std::vector<V>&& values)
        {
            const auto index = static_cast<size_t>(handle);
            if (index >= _data.size())
            {
                _data.resize(index + 1);
            }
            _data[index] = values;
        }

        std::vector<V>* GetAll(Handle handle)
        {
            const auto index = static_cast<size_t>(handle);
            if (index < _data.size())
            {
                return &_data[index];
            }
            return nullptr;
        }

        void Clear()
        {
            _data.clear();
        }

        void RemoveHandle(Handle handle)
        {
            const auto index = static_cast<size_t>(handle);
            if (index < _data.size())
            {
                _data[index].clear();
            }
        }

        void RemoveValue(V value)
        {
            for (auto& values : _data)
            {
                values.erase(
                    std::remove_if(values.begin(), values.end(), [value](auto v) { return v == value; }), values.end());
            }
        }
    };
} // namespace OpenRCT2
