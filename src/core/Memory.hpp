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

extern "C"
{
    #include "../common.h"
}

/**
 * Utility methods for memory management. Typically helpers and wrappers around the C standard library.
 */
namespace Memory
{
    template<typename T>
    T * Allocate()
    {
        return (T*)malloc(sizeof(T));
    }

    template<typename T>
    T * Allocate(size_t size)
    {
        return (T*)malloc(size);
    }

    template<typename T>
    T * AllocateArray(size_t count)
    {
        return (T*)malloc(count * sizeof(T));
    }

    template<typename T>
    T * Reallocate(T * ptr, size_t size)
    {
        if (ptr == nullptr)
        {
            return (T*)malloc(size);
        }
        else
        {
            return (T*)realloc((void*)ptr, size);
        }
    }

    template<typename T>
    T * ReallocateArray(T * ptr, size_t count)
    {
        if (ptr == nullptr)
        {
            return (T*)malloc(count * sizeof(T));
        }
        else
        {
            return (T*)realloc((void*)ptr, count * sizeof(T));
        }
    }

    template<typename T>
    void Free(T * ptr)
    {
        free((void*)ptr);
    }

    template<typename T>
    T * Copy(T * dst, const T * src, size_t size)
    {
        if (size == 0) return (T*)dst;
        return (T*)memcpy((void*)dst, (const void*)src, size);
    }

    template<typename T>
    T * Duplicate(const T * src, size_t size)
    {
        T *result = Allocate<T>(size);
        return Copy(result, src, size);
    }

    template<typename T>
    T * Set(T * dst, uint8 value, size_t size)
    {
        return (T*)memset((void*)dst, (int)value, size);
    }

    template<typename T>
    T * CopyArray(T * dst, const T * src, size_t count)
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
    T * DuplicateArray(const T * src, size_t count)
    {
        T * result = AllocateArray<T>(count);
        return CopyArray(result, src, count);
    }

    template<typename T>
    void FreeArray(T * ptr, size_t count)
    {
        for (size_t i = 0; i < count; i++)
        {
            ptr[i].~T();
        }
        Free(ptr);
    }
}
