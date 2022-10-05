/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Identifiers.h"
#include "../common.h"
#include "../core/Guard.hpp"
#include "../core/String.hpp"

#include <array>
#include <cstring>

extern thread_local uint8_t gCommonFormatArgs[80];

class Formatter
{
    std::array<uint8_t, 256> Buffer{};
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
        auto finalCount = NumBytes() + count;
        Guard::Assert(finalCount < Buffer.size(), "Increment is greater than buffer size!");
        if (finalCount < Buffer.size())
        {
            CurrentBuf += count;
        }
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
        static_assert(sizeof(TSpecified) <= sizeof(uint64_t), "Type too large");
        static_assert(sizeof(TDeduced) <= sizeof(uint64_t), "Type too large");

        // clang-format off
        static_assert(
            std::is_same_v<typename std::remove_cv_t<TSpecified>, char*> ||
            std::is_same_v<typename std::remove_cv_t<TSpecified>, const char*> ||
            std::is_same_v<typename std::remove_cv_t<TSpecified>, int16_t> ||
            std::is_same_v<typename std::remove_cv_t<TSpecified>, int32_t> ||
            std::is_same_v<typename std::remove_cv_t<TSpecified>, money32> ||
            std::is_same_v<typename std::remove_cv_t<TSpecified>, money64> ||
            std::is_same_v<typename std::remove_cv_t<TSpecified>, RideId> ||
            std::is_same_v<typename std::remove_cv_t<TSpecified>, EntityId> ||
            std::is_same_v<typename std::remove_cv_t<TSpecified>, StringId> ||
            std::is_same_v<typename std::remove_cv_t<TSpecified>, uint16_t> ||
            std::is_same_v<typename std::remove_cv_t<TSpecified>, uint32_t> ||
            std::is_same_v<typename std::remove_cv_t<TSpecified>, utf8*> ||
            std::is_same_v<typename std::remove_cv_t<TSpecified>, const utf8*>
        );
        // clang-format on

        uint64_t convertedValue;
        if constexpr (std::is_same_v<std::remove_cv_t<TDeduced>, RideId>)
        {
            convertedValue = static_cast<uint64_t>(value.ToUnderlying());
        }
        else if constexpr (std::is_same_v<std::remove_cv_t<TDeduced>, EntityId>)
        {
            convertedValue = static_cast<uint64_t>(value.ToUnderlying());
        }
        else if constexpr (std::is_integral_v<TSpecified> || std::is_enum_v<TSpecified>)
        {
            convertedValue = static_cast<uint64_t>(value);
        }
        else
        {
            convertedValue = reinterpret_cast<uint64_t>(value);
        }
        std::memcpy(CurrentBuf, &convertedValue, sizeof(TSpecified));
        Increment(sizeof(TSpecified));
        return *this;
    }
};

struct OpenRCT2String
{
    StringId str;
    Formatter args;
};
