/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3
 *****************************************************************************/

#pragma once

#include "../common.h"

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
