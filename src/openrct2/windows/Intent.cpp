/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Intent.h"

#include "../core/Guard.hpp"

#include <utility>

using namespace OpenRCT2;

Intent::Intent(WindowClass windowClass)
    : _Class(windowClass)
{
}

Intent::Intent(WindowDetail windowDetail)
    : _WindowDetail(windowDetail)
{
}

Intent::Intent(IntentAction intentAction)
    : _Action(intentAction)
{
}

Intent* Intent::PutExtra(uint32_t key, uint32_t value)
{
    _Data.emplace(key, static_cast<int64_t>(value));

    return this;
}

Intent* Intent::PutExtra(uint32_t key, void* value)
{
    _Data.emplace(key, value);

    return this;
}

Intent* Intent::PutExtra(uint32_t key, int32_t value)
{
    _Data.emplace(key, static_cast<int64_t>(value));

    return this;
}

Intent* Intent::PutExtra(uint32_t key, std::string value)
{
    _Data.emplace(key, value);

    return this;
}

Intent* Intent::PutExtra(uint32_t key, close_callback value)
{
    _Data.emplace(key, value);

    return this;
}

WindowClass Intent::GetWindowClass() const
{
    return this->_Class;
}

WindowDetail Intent::GetWindowDetail() const
{
    return this->_WindowDetail;
}

IntentAction Intent::GetAction() const
{
    return this->_Action;
}

void* Intent::GetPointerExtra(uint32_t key) const
{
    if (_Data.count(key) == 0)
    {
        return nullptr;
    }

    auto data = _Data.at(key);

    assert(std::holds_alternative<void*>(data));
    return std::get<void*>(data);
}

uint32_t Intent::GetUIntExtra(uint32_t key) const
{
    if (_Data.count(key) == 0)
    {
        return 0;
    }

    auto data = _Data.at(key);

    assert(std::holds_alternative<int64_t>(data));
    return static_cast<uint32_t>(std::get<int64_t>(data));
}

int32_t Intent::GetSIntExtra(uint32_t key) const
{
    if (_Data.count(key) == 0)
    {
        return 0;
    }

    auto data = _Data.at(key);

    assert(std::holds_alternative<int64_t>(data));
    return static_cast<int32_t>(std::get<int64_t>(data));
}

std::string Intent::GetStringExtra(uint32_t key) const
{
    if (_Data.count(key) == 0)
    {
        return std::string{};
    }

    auto data = _Data.at(key);

    assert(std::holds_alternative<std::string>(data));
    return std::get<std::string>(data);
}

close_callback Intent::GetCloseCallbackExtra(uint32_t key) const
{
    if (_Data.count(key) == 0)
    {
        return nullptr;
    }

    auto data = _Data.at(key);

    assert(std::holds_alternative<close_callback>(data));
    return std::get<close_callback>(data);
}
