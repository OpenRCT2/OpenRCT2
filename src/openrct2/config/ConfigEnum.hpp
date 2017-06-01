#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#pragma once

#include <initializer_list>
#include <vector>
#include "../core/String.hpp"

template<typename T>
struct ConfigEnumEntry
{
    std::string Key;
    T           Value;

    ConfigEnumEntry(const std::string &key, T value)
        : Key(key),
          Value(value)
    {
    }
};

template<typename T>
interface IConfigEnum
{
    virtual ~IConfigEnum() = default;
    virtual std::string GetName(T value) const abstract;
    virtual T GetValue(const std::string &key, T defaultValue) const abstract;
};

template<typename T>
class ConfigEnum final : public IConfigEnum<T>
{
private:
    std::vector<ConfigEnumEntry<T>> _entries;

public:
    ConfigEnum(std::initializer_list<ConfigEnumEntry<T>> entries)
    {
        _entries = entries;
    }

    std::string GetName(T value) const override
    {
        for (const auto &entry : _entries)
        {
            if (entry.Value == value)
            {
                return entry.Key;
            }
        }
        return std::string();
    }

    T GetValue(const std::string &key, T defaultValue) const override
    {
        for (const auto &entry : _entries)
        {
            if (String::Equals(entry.Key, key, true))
            {
                return entry.Value;
            }
        }
        return defaultValue;
    }
};
