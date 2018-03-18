#pragma region Copyright (c) 2014-2018 OpenRCT2 Developers
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

#include <utility>
#include "../core/Guard.hpp"
#include "Intent.h"

Intent::Intent(rct_windowclass windowclass)
{
    this->_Class = windowclass;
}

Intent * Intent::putExtra(uint32 key, uint32 value)
{
    IntentData data = {};
    data.intVal.unsignedInt = value;
    data.type = IntentData::DT_INT;

    _Data.insert(std::make_pair(key, data));

    return this;
}

Intent * Intent::putExtra(uint32 key, void * value)
{
    IntentData data = {};
    data.pointerVal = value;
    data.type = IntentData::DT_POINTER;

    _Data.insert(std::make_pair(key, data));

    return this;
}

Intent * Intent::putExtra(uint32 key, sint32 value)
{
    IntentData data = {};
    data.intVal.signedInt = value;
    data.type = IntentData::DT_INT;

    _Data.insert(std::make_pair(key, data));

    return this;
}

Intent * Intent::putExtra(uint32 key, std::string value)
{
    IntentData data = {};
    data.stringVal = std::move(value);
    data.type = IntentData::DT_STRING;

    _Data.insert(std::make_pair(key, data));

    return this;
}

Intent * Intent::putExtra(uint32 key, close_callback value)
{
    IntentData data = {};
    data.closeCallbackVal = value;
    data.type = IntentData::DT_CLOSE_CALLBACK;

    _Data.insert(std::make_pair(key, data));

    return this;
}

rct_windowclass Intent::GetWindowClass() const
{
    return this->_Class;
}

void * Intent::GetPointerExtra(uint32 key) const
{
    if (_Data.count(key) == 0)
    {
        return nullptr;
    }

    auto data = _Data.at(key);
    openrct2_assert(data.type == IntentData::DT_POINTER, "Actual type doesn't match requested type");
    return (void *) data.pointerVal;
}

uint32 Intent::GetUIntExtra(uint32 key) const
{
    if (_Data.count(key) == 0)
    {
        return 0;
    }

    auto data = _Data.at(key);
    openrct2_assert(data.type == IntentData::DT_INT, "Actual type doesn't match requested type");
    return data.intVal.unsignedInt;
}

sint32 Intent::GetSIntExtra(uint32 key) const
{
    if (_Data.count(key) == 0)
    {
        return 0;
    }

    auto data = _Data.at(key);
    openrct2_assert(data.type == IntentData::DT_INT, "Actual type doesn't match requested type");
    return data.intVal.signedInt;
}

std::string Intent::GetStringExtra(uint32 key) const
{
    if (_Data.count(key) == 0)
    {
        return std::string {};
    }

    auto data = _Data.at(key);
    openrct2_assert(data.type == IntentData::DT_STRING, "Actual type doesn't match requested type");
    return data.stringVal;
}

close_callback Intent::GetCloseCallbackExtra(uint32 key) const
{
    if (_Data.count(key) == 0)
    {
        return nullptr;
    }

    auto data = _Data.at(key);
    openrct2_assert(data.type == IntentData::DT_CLOSE_CALLBACK, "Actual type doesn't match requested type");
    return data.closeCallbackVal;
}
