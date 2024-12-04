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

#include <ranges>
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
    auto it = std::ranges::lower_bound(_Data, key, {}, &DataEntry::first);
    assert(it == _Data.end() || it->first != key);

    _Data.emplace(it, key, static_cast<int64_t>(value));

    return this;
}

Intent* Intent::PutExtra(uint32_t key, void* value)
{
    auto it = std::ranges::lower_bound(_Data, key, {}, &DataEntry::first);
    assert(it == _Data.end() || it->first != key);

    _Data.emplace(it, key, value);

    return this;
}

Intent* Intent::PutExtra(uint32_t key, int32_t value)
{
    auto it = std::ranges::lower_bound(_Data, key, {}, &DataEntry::first);
    assert(it == _Data.end() || it->first != key);

    _Data.emplace(it, key, static_cast<int64_t>(value));

    return this;
}

Intent* Intent::PutExtra(uint32_t key, std::string value)
{
    auto it = std::ranges::lower_bound(_Data, key, {}, &DataEntry::first);
    assert(it == _Data.end() || it->first != key);

    _Data.emplace(it, key, std::move(value));

    return this;
}

Intent* Intent::PutExtra(uint32_t key, close_callback value)
{
    auto it = std::ranges::lower_bound(_Data, key, {}, &DataEntry::first);
    assert(it == _Data.end() || it->first != key);

    _Data.emplace(it, key, value);

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
    auto it = std::ranges::lower_bound(_Data, key, {}, &DataEntry::first);
    if (it == _Data.end() || it->first != key)
    {
        return nullptr;
    }

    const auto& [_, data] = *it;

    assert(std::holds_alternative<void*>(data));
    return std::get<void*>(data);
}

uint32_t Intent::GetUIntExtra(uint32_t key) const
{
    auto it = std::ranges::lower_bound(_Data, key, {}, &DataEntry::first);
    if (it == _Data.end() || it->first != key)
    {
        return 0;
    }

    const auto& [_, data] = *it;

    assert(std::holds_alternative<int64_t>(data));
    return static_cast<uint32_t>(std::get<int64_t>(data));
}

int32_t Intent::GetSIntExtra(uint32_t key) const
{
    auto it = std::ranges::lower_bound(_Data, key, {}, &DataEntry::first);
    if (it == _Data.end() || it->first != key)
    {
        return 0;
    }

    const auto& [_, data] = *it;

    assert(std::holds_alternative<int64_t>(data));
    return static_cast<int32_t>(std::get<int64_t>(data));
}

std::string Intent::GetStringExtra(uint32_t key) const
{
    auto it = std::ranges::lower_bound(_Data, key, {}, &DataEntry::first);
    if (it == _Data.end() || it->first != key)
    {
        return std::string{};
    }

    const auto& [_, data] = *it;

    assert(std::holds_alternative<std::string>(data));
    return std::get<std::string>(data);
}

close_callback Intent::GetCloseCallbackExtra(uint32_t key) const
{
    auto it = std::ranges::lower_bound(_Data, key, {}, &DataEntry::first);
    if (it == _Data.end() || it->first != key)
    {
        return nullptr;
    }

    const auto& [_, data] = *it;

    assert(std::holds_alternative<close_callback>(data));
    return std::get<close_callback>(data);
}
