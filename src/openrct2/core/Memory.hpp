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
