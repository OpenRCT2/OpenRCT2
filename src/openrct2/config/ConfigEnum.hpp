/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/String.hpp"

#include <initializer_list>
#include <utility>
#include <vector>

template<typename T>
struct ConfigEnumEntry
{
    std::string Key;
    T Value;

    ConfigEnumEntry(std::string key, T value)
        : Key(std::move(key))
        , Value(value)
    {
    }
};

template<typename T>
struct IConfigEnum
{
    virtual ~IConfigEnum() = default;
    virtual std::string GetName(T value) const = 0;
    virtual T GetValue(const std::string& key, T defaultValue) const = 0;
};

template<typename T>
class ConfigEnum final : public IConfigEnum<T>
{
private:
    const std::vector<ConfigEnumEntry<T>> _entries;

public:
    ConfigEnum(const std::initializer_list<ConfigEnumEntry<T>>& entries)
        : _entries(entries)
    {
    }

    std::string GetName(T value) const override
    {
        for (const auto& entry : _entries)
        {
            if (entry.Value == value)
            {
                return entry.Key;
            }
        }
        return std::string();
    }

    T GetValue(const std::string& key, T defaultValue) const override
    {
        for (const auto& entry : _entries)
        {
            if (OpenRCT2::String::iequals(entry.Key, key))
            {
                return entry.Value;
            }
        }
        return defaultValue;
    }
};
