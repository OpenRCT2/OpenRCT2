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

#include "../common.h"

#ifdef __cplusplus

template <size_t size>
struct ByteSwapT { };

template <>
struct ByteSwapT<1>
{
    static uint8 SwapBE(uint8 value)
    {
        return value;
    }
};

template <>
struct ByteSwapT<2>
{
    static uint16 SwapBE(uint16 value)
    {
        return (uint16)((value << 8) | (value >> 8));
    }
};

template <>
struct ByteSwapT<4>
{
    static uint32 SwapBE(uint32 value)
    {
        return (uint32)(((value << 24) |
            ((value << 8) & 0x00FF0000) |
                         ((value >> 8) & 0x0000FF00) |
                         (value >> 24)));
    }
};

template <typename T>
static T ByteSwapBE(const T& value)
{
    return ByteSwapT<sizeof(T)>::SwapBE(value);
}

#endif