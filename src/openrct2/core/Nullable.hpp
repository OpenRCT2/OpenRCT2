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

#ifdef __cplusplus

#include "../common.h"

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

#endif
