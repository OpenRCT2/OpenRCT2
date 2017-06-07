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
    static T * Copy(T * dst, const T * src, size_t size)
    {
        if (size == 0) return (T*)dst;
#ifdef DEBUG
        uintptr_t srcBegin = (uintptr_t)src;
        uintptr_t srcEnd = srcBegin + size;
        uintptr_t dstBegin = (uintptr_t)dst;
        uintptr_t dstEnd = dstBegin + size;
        Guard::Assert(srcEnd <= dstBegin || srcBegin >= dstEnd, "Source overlaps destination, try using Memory::Move.");
#endif
        return (T*)memcpy((void*)dst, (const void*)src, size);
    }

    template<typename T>
    static T * Move(T * dst, const T * src, size_t size)
    {
        if (size == 0) return (T*)dst;
        Guard::ArgumentNotNull(dst, "Trying to move memory to nullptr");
        Guard::ArgumentNotNull(src, "Trying to move memory from nullptr");
        T* result =(T*)memmove((void*)dst, (const void*)src, size);
        Guard::ArgumentNotNull(result, "Failed to move %u bytes of memory from %x to %x for %s", size, src, dst, typeid(T).name());
        return result;
    }

    template<typename T>
    static T * Duplicate(const T * src, size_t size)
    {
        T *result = Allocate<T>(size);
        return Copy(result, src, size);
    }

    template<typename T>
    static T * Set(T * dst, uint8 value, size_t size)
    {
        return (T*)memset((void*)dst, (sint32)value, size);
    }

    template<typename T>
    static T * CopyArray(T * dst, const T * src, size_t count)
    {
        // Use a loop so that copy constructors are called
        // compiler should optimise to memcpy if possible
        T * result = dst;
        for (; count > 0; count--)
        {
            *dst++ = *src++;
        }
        return result;
    }

    template<typename T>
    static T * DuplicateArray(const T * src, size_t count)
    {
        T * result = AllocateArray<T>(count);
        return CopyArray(result, src, count);
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
}
