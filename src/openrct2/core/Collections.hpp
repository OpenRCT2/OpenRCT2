/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "Memory.hpp"
#include "String.hpp"

#include <initializer_list>

namespace OpenRCT2::Collections
{
    template<typename TCollection, typename TItem>
    static void AddRange(TCollection& collection, std::initializer_list<TItem> initializerList)
    {
        collection.insert(collection.end(), initializerList.begin(), initializerList.end());
    }

    template<typename TCollection, typename TItem, typename TComparer>
    static bool Contains(TCollection& collection, TItem needle, TComparer comparer)
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
    static size_t IndexOf(const TCollection& collection, TItem needle, TComparer comparer)
    {
        size_t index = 0;
        for (const auto& item : collection)
        {
            if (comparer(item, needle))
            {
                return index;
            }
            index++;
        }
        return SIZE_MAX;
    }

    template<typename TCollection, typename TPred> static size_t IndexOf(const TCollection& collection, TPred predicate)
    {
        size_t index = 0;
        for (const auto& item : collection)
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

    template<typename TCollection> static bool Contains(TCollection& collection, const char* item, bool ignoreCase = false)
    {
        return Contains(
            collection, item, [ignoreCase](const char* a, const char* b) { return String::Equals(a, b, ignoreCase); });
    }

    template<typename TCollection> static size_t IndexOf(TCollection& collection, const char* item, bool ignoreCase = false)
    {
        return IndexOf(
            collection, item, [ignoreCase](const char* a, const char* b) { return String::Equals(a, b, ignoreCase); });
    }

    template<typename TCollection> static typename TCollection::value_type* ToArray(const TCollection& collection)
    {
        size_t count = collection.size();
        if (count == 0)
        {
            return nullptr;
        }

        auto* items = Memory::AllocateArray<typename TCollection::value_type>(count);
        size_t i = 0;
        for (const auto& item : collection)
        {
            items[i] = item;
            i++;
        }
        return items;
    }

#pragma endregion
} // namespace OpenRCT2::Collections
