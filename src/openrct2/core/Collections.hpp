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
#include "../common.h"
#include "Memory.hpp"
#include "String.hpp"

namespace Collections
{
    template<typename TCollection, typename TItem>
    static void AddRange(TCollection &collection, std::initializer_list<TItem> initializerList)
    {
        collection.insert(collection.end(), initializerList.begin(), initializerList.end());
    }

    template<typename TCollection, typename TItem, typename TComparer>
    static bool Contains(TCollection &collection, TItem needle, TComparer comparer)
    {
        for (TItem item : collection)
        {
            if (comparer(item, needle))
            {
                return true;
            }
        }
        return false;
    }

    template<typename TCollection, typename TItem, typename TComparer>
    static size_t IndexOf(TCollection &collection, TItem needle, TComparer comparer)
    {
        size_t index = 0;
        for (TItem item : collection)
        {
            if (comparer(item, needle))
            {
                return index;
            }
            index++;
        }
        return SIZE_MAX;
    }

    template<typename TCollection, typename TPred>
    static size_t IndexOf(TCollection &collection, TPred predicate)
    {
        size_t index = 0;
        for (auto item : collection)
        {
            if (predicate(item))
            {
                return index;
            }
            index++;
        }
        return SIZE_MAX;
    }

    #pragma region String helpers

    template<typename TCollection>
    static bool Contains(TCollection &collection, const char * item, bool ignoreCase = false)
    {
        if (ignoreCase)
        {
            return Contains(collection, item,
                [](const char * a, const char * b)
                {
                    return String::Equals(a, b, true);
                });
        }
        else
        {
            return Contains(collection, item,
                [](const char * a, const char * b)
                {
                    return String::Equals(a, b, false);
                });
        }
    }

    template<typename TCollection>
    static size_t IndexOf(TCollection &collection, const char * item, bool ignoreCase = false)
    {
        if (ignoreCase)
        {
            return IndexOf(collection, item,
                [](const char * a, const char * b)
                {
                    return String::Equals(a, b, true);
                });
        }
        else
        {
            return IndexOf(collection, item,
                [](const char * a, const char * b)
                {
                    return String::Equals(a, b, false);
                });
        }
    }

    template<typename TCollection>
    static typename TCollection::value_type * ToArray(const TCollection &collection)
    {
        size_t count = collection.size();
        if (count == 0)
        {
            return nullptr;
        }

        auto * items = Memory::AllocateArray<typename TCollection::value_type>(count);
        size_t i = 0;
        for (const auto &item : collection)
        {
            items[i] = item;
            i++;
        }
        return items;
    }

    #pragma endregion
}
