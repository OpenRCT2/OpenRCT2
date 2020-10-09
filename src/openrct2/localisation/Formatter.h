/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

#include <array>
#include <cstring>

extern thread_local uint8_t gCommonFormatArgs[80];

class Formatter
{
    std::array<uint8_t, 80> Buffer{};
    uint8_t* StartBuf{};
    uint8_t* CurrentBuf{};

public:
    explicit Formatter(uint8_t* buf)
        : StartBuf(buf)
        , CurrentBuf(buf)
    {
    }

    Formatter()
        : Buffer{}
        , StartBuf(Buffer.data())
        , CurrentBuf(StartBuf)
    {
    }

    Formatter(const Formatter& other)
    {
        *this = other;
    }

    Formatter& operator=(const Formatter& other);

    static Formatter Common();

    auto Buf()
    {
        return CurrentBuf;
    }

    auto Data() const
    {
        return StartBuf;
    }

    void Increment(size_t count)
    {
        CurrentBuf += count;
    }

    void Rewind()
    {
        CurrentBuf -= NumBytes();
    }

    std::size_t NumBytes() const
    {
        return CurrentBuf - StartBuf;
    }

    template<typename TSpecified, typename TDeduced> Formatter& Add(TDeduced value)
    {
        static_assert(sizeof(TSpecified) <= sizeof(uintptr_t), "Type too large");
        static_assert(sizeof(TDeduced) <= sizeof(uintptr_t), "Type too large");

        // clang-format off
        static_assert(
            std::is_same_v<TSpecified, char*> ||
            std::is_same_v<TSpecified, const char*> ||
            std::is_same_v<TSpecified, int16_t> ||
            std::is_same_v<TSpecified, int32_t> ||
            std::is_same_v<TSpecified, money32> ||
            std::is_same_v<TSpecified, rct_string_id> ||
            std::is_same_v<TSpecified, uint16_t> ||
            std::is_same_v<TSpecified, uint32_t> ||
            std::is_same_v<TSpecified, utf8*> ||
            std::is_same_v<TSpecified, const utf8*>
        );
        // clang-format on

        uintptr_t convertedValue;
        if constexpr (std::is_integral_v<TSpecified>)
        {
            convertedValue = static_cast<uintptr_t>(value);
        }
        else
        {
            convertedValue = reinterpret_cast<uintptr_t>(value);
        }
        std::memcpy(CurrentBuf, &convertedValue, sizeof(TSpecified));
        Increment(sizeof(TSpecified));
        return *this;
    }
};

struct OpenRCT2String
{
    rct_string_id str;
    Formatter args;
};
