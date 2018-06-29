/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <utility>
#include "../core/Guard.hpp"
#include "Intent.h"

Intent::Intent(rct_windowclass windowclass)
{
    this->_Class = windowclass;
}

Intent * Intent::putExtra(uint32_t key, uint32_t value)
{
    IntentData data = {};
    data.intVal.unsignedInt = value;
    data.type = IntentData::DT_INT;

    _Data.insert(std::make_pair(key, data));

    return this;
}

Intent * Intent::putExtra(uint32_t key, void * value)
{
    IntentData data = {};
    data.pointerVal = value;
    data.type = IntentData::DT_POINTER;

    _Data.insert(std::make_pair(key, data));

    return this;
}

Intent * Intent::putExtra(uint32_t key, int32_t value)
{
    IntentData data = {};
    data.intVal.signedInt = value;
    data.type = IntentData::DT_INT;

    _Data.insert(std::make_pair(key, data));

    return this;
}

Intent * Intent::putExtra(uint32_t key, std::string value)
{
    IntentData data = {};
    data.stringVal = std::move(value);
    data.type = IntentData::DT_STRING;

    _Data.insert(std::make_pair(key, data));

    return this;
}

Intent * Intent::putExtra(uint32_t key, close_callback value)
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

void * Intent::GetPointerExtra(uint32_t key) const
{
    if (_Data.count(key) == 0)
    {
        return nullptr;
    }

    auto data = _Data.at(key);
    openrct2_assert(data.type == IntentData::DT_POINTER, "Actual type doesn't match requested type");
    return (void *) data.pointerVal;
}

uint32_t Intent::GetUIntExtra(uint32_t key) const
{
    if (_Data.count(key) == 0)
    {
        return 0;
    }

    auto data = _Data.at(key);
    openrct2_assert(data.type == IntentData::DT_INT, "Actual type doesn't match requested type");
    return data.intVal.unsignedInt;
}

int32_t Intent::GetSIntExtra(uint32_t key) const
{
    if (_Data.count(key) == 0)
    {
        return 0;
    }

    auto data = _Data.at(key);
    openrct2_assert(data.type == IntentData::DT_INT, "Actual type doesn't match requested type");
    return data.intVal.signedInt;
}

std::string Intent::GetStringExtra(uint32_t key) const
{
    if (_Data.count(key) == 0)
    {
        return std::string {};
    }

    auto data = _Data.at(key);
    openrct2_assert(data.type == IntentData::DT_STRING, "Actual type doesn't match requested type");
    return data.stringVal;
}

close_callback Intent::GetCloseCallbackExtra(uint32_t key) const
{
    if (_Data.count(key) == 0)
    {
        return nullptr;
    }

    auto data = _Data.at(key);
    openrct2_assert(data.type == IntentData::DT_CLOSE_CALLBACK, "Actual type doesn't match requested type");
    return data.closeCallbackVal;
}
