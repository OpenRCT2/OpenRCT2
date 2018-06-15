/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdlib>
#include <cstring>
#include <typeinfo>
#include "../common.h"
#include "Guard.hpp"

/**
 * Utility methods for memory management. Typically helpers and wrappers around the C standard library.
 */
namespace Memory
{
    template<typename T>
    static T * Allocate()
    {
        T* result = (T*)malloc(sizeof(T));
        Guard::ArgumentNotNull(result, "Failed to allocate %u bytes for %s", sizeof(T), typeid(T).name());
        return result;
    }

    template<typename T>
    static T * Allocate(size_t size)
    {
        T* result = (T*)malloc(size);
        Guard::ArgumentNotNull(result, "Failed to allocate %u bytes for %s", size, typeid(T).name());
        return result;
    }

    template<typename T>
    static T * AllocateArray(size_t count)
    {
        T* result = (T*)malloc(count * sizeof(T));
        Guard::ArgumentNotNull(result, "Failed to allocate array of %u * %s (%u bytes)", count, typeid(T).name(), sizeof(T));
        return result;
    }

    template<typename T>
    static T * Reallocate(T * ptr, size_t size)
    {
        T* result;
        if (ptr == nullptr)
        {
            result = (T*)malloc(size);
        }
        else
        {
            result = (T*)realloc((void*)ptr, size);
        }
        Guard::ArgumentNotNull(result, "Failed to reallocate %x (%s) to have %u bytes", ptr, typeid(T).name(), size);
        return result;
    }

    template<typename T>
    static T * ReallocateArray(T * ptr, size_t count)
    {
        T* result;
        if (ptr == nullptr)
        {
            result = (T*)malloc(count * sizeof(T));
        }
        else
        {
            result = (T*)realloc((void*)ptr, count * sizeof(T));
        }
        Guard::ArgumentNotNull(result, "Failed to reallocate array at %x (%s) to have %u entries", ptr, typeid(T).name(), count);
        return result;
    }

    template<typename T>
    static void Free(T * ptr)
    {
        free((void*)ptr);
    }

    template<typename T>
    static void FreeArray(T * ptr, size_t count)
    {
        for (size_t i = 0; i < count; i++)
        {
            ptr[i].~T();
        }
        Free(ptr);
    }
} // namespace Memory
