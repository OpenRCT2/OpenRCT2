/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>
#include <cstdio>
#include <limits>

template<typename T, T TNullValue, typename TTag> class TIdentifier
{
    enum class ValueType : T
    {
        Null = TNullValue,
    } _handle;

private:
    explicit constexpr TIdentifier(const T index)
        : _handle{ static_cast<ValueType>(index) }
    {
    }

    explicit constexpr TIdentifier(const ValueType other)
        : _handle{ other }
    {
    }

public:
    using UnderlyingType = T;

    constexpr TIdentifier() = default;

    static constexpr TIdentifier GetNull() noexcept
    {
        return TIdentifier{ ValueType::Null };
    }

    static constexpr TIdentifier FromUnderlying(const T val) noexcept
    {
        return TIdentifier{ val };
    }

    constexpr T ToUnderlying() const noexcept
    {
        return static_cast<T>(_handle);
    }

    constexpr bool IsNull() const noexcept
    {
        return _handle == ValueType::Null;
    }

    constexpr bool operator==(const ValueType other) const noexcept
    {
        return _handle == other;
    }

    constexpr bool operator!=(const ValueType other) const noexcept
    {
        return _handle != other;
    }

    constexpr bool operator==(const TIdentifier& other) const noexcept
    {
        return _handle == other._handle;
    }

    constexpr bool operator!=(const TIdentifier& other) const noexcept
    {
        return _handle != other._handle;
    }
};
