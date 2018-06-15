/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include <cstddef>

template<typename T>
struct Nullable
{
public:
    Nullable()
    {
        _value = T();
        _hasValue = false;
    }

    Nullable(std::nullptr_t)
    {
        _value = T();
        _hasValue = false;
    }

    Nullable(const T &value)
    {
        _value = value;
        _hasValue = true;
    }

    bool HasValue() const
    {
        return _hasValue;
    }

    T GetValue() const
    {
        return _value;
    }

    T GetValueOrDefault(T defaultValue) const
    {
        return _hasValue ? _value : defaultValue;
    }

private:
    T       _value;
    bool    _hasValue;
};
