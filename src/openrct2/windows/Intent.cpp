/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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

template<typename T>
static void putExtraImpl(IntentDataStorage& storage, uint32_t key, T&& value)
{
    auto it = std::ranges::lower_bound(storage, key, {}, &IntentDataEntry::first);

    // Key should not already exist.
    assert(it == storage.end() || it->first != key);

    storage.emplace(it, key, std::forward<T>(value));
}

template<typename T>
static auto getExtraImpl(const IntentDataStorage& storage, uint32_t key)
{
    auto it = std::ranges::lower_bound(storage, key, {}, &IntentDataEntry::first);
    if (it == storage.end() || it->first != key)
    {
        // TODO: The code currently assumes that some things are optional, we need to handle this better.
        return T{};
    }

    const auto& [_, data] = *it;
    assert(std::holds_alternative<T>(data));

    return std::get<T>(data);
}

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
    putExtraImpl(_Data, key, static_cast<int64_t>(value));
    return this;
}

Intent* Intent::PutExtra(uint32_t key, void* value)
{
    putExtraImpl(_Data, key, value);
    return this;
}

Intent* Intent::PutExtra(uint32_t key, int32_t value)
{
    putExtraImpl(_Data, key, static_cast<int64_t>(value));
    return this;
}

Intent* Intent::PutExtra(uint32_t key, std::string value)
{
    putExtraImpl(_Data, key, std::move(value));
    return this;
}

Intent* Intent::PutExtra(uint32_t key, CloseCallback value)
{
    putExtraImpl(_Data, key, value);
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
    return getExtraImpl<void*>(_Data, key);
}

uint32_t Intent::GetUIntExtra(uint32_t key) const
{
    return static_cast<uint32_t>(getExtraImpl<int64_t>(_Data, key));
}

int32_t Intent::GetSIntExtra(uint32_t key) const
{
    return static_cast<uint32_t>(getExtraImpl<int64_t>(_Data, key));
}

std::string Intent::GetStringExtra(uint32_t key) const
{
    return getExtraImpl<std::string>(_Data, key);
}

CloseCallback Intent::GetCloseCallbackExtra(uint32_t key) const
{
    return getExtraImpl<CloseCallback>(_Data, key);
}
