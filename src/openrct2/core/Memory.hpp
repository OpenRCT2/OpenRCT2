/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Guard.hpp"

#include <cstdlib>
#include <cstring>
#include <typeinfo>

/**
 * Utility methods for memory management. Typically helpers and wrappers around the C standard library.
 */
namespace OpenRCT2::Memory
{
    template<typename T>
    static T* Allocate()
    {
        T* result = static_cast<T*>(malloc(sizeof(T)));
        Guard::ArgumentNotNull(result, "Failed to allocate %zu bytes for %s", sizeof(T), typeid(T).name());
        return result;
    }

    template<typename T>
    static T* Allocate(size_t size)
    {
        T* result = static_cast<T*>(malloc(size));
        Guard::ArgumentNotNull(result, "Failed to allocate %zu bytes for %s", size, typeid(T).name());
        return result;
    }

    template<typename T>
    static T* Reallocate(T* ptr, size_t size)
    {
        T* result;
        if (ptr == nullptr)
        {
            result = static_cast<T*>(malloc(size));
        }
        else
        {
            result = static_cast<T*>(realloc(reinterpret_cast<void*>(ptr), size));
        }
        Guard::ArgumentNotNull(result, "Failed to reallocate %x (%s) to have %zu bytes", ptr, typeid(T).name(), size);
        return result;
    }

    template<typename T>
    static void Free(T* ptr)
    {
        free(const_cast<void*>(reinterpret_cast<const void*>(ptr)));
    }

} // namespace OpenRCT2::Memory
