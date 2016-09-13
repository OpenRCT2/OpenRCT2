#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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
#include "String.hpp"

namespace Collections
{
    template<typename TCollection, typename TItem>
    void AddRange(TCollection &collection, std::initializer_list<TItem> initializerList)
    {
        collection.insert(collection.end(), initializerList.begin(), initializerList.end());
    }

    template<typename TCollection, typename TItem, typename TComparer>
    bool Contains(TCollection &collection, TItem needle, TComparer comparer)
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
    size_t IndexOf(TCollection &collection, TItem needle, TComparer comparer)
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

    #pragma region String helpers

    template<typename TCollection>
    bool Contains(TCollection &collection, const char * item, bool ignoreCase = false)
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
    size_t IndexOf(TCollection &collection, const char * item, bool ignoreCase = false)
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

    #pragma endregion
}
