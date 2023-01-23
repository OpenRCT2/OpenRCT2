/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Intent.h"

#include "../core/Guard.hpp"

#include <utility>
namespace OpenRCT2
{
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
        IntentData data = {};
        data.intVal.unsignedInt = value;
        data.type = IntentData::DataType::Int;

        _Data.insert(std::make_pair(key, data));

        return this;
    }

    Intent* Intent::PutExtra(uint32_t key, void* value)
    {
        IntentData data = {};
        data.pointerVal = value;
        data.type = IntentData::DataType::Pointer;

        _Data.insert(std::make_pair(key, data));

        return this;
    }

    Intent* Intent::PutExtra(uint32_t key, int32_t value)
    {
        IntentData data = {};
        data.intVal.signedInt = value;
        data.type = IntentData::DataType::Int;

        _Data.insert(std::make_pair(key, data));

        return this;
    }

    Intent* Intent::PutExtra(uint32_t key, std::string value)
    {
        IntentData data = {};
        data.stringVal = std::move(value);
        data.type = IntentData::DataType::String;

        _Data.insert(std::make_pair(key, data));

        return this;
    }

    Intent* Intent::PutExtra(uint32_t key, close_callback value)
    {
        IntentData data = {};
        data.closeCallbackVal = value;
        data.type = IntentData::DataType::CloseCallback;

        _Data.insert(std::make_pair(key, data));

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
        openrct2_assert(data.type == IntentData::DataType::Pointer, "Actual type doesn't match requested type");
        return static_cast<void*>(data.pointerVal);
    }

    uint32_t Intent::GetUIntExtra(uint32_t key) const
    {
        if (_Data.count(key) == 0)
        {
            return 0;
        }

        auto data = _Data.at(key);
        openrct2_assert(data.type == IntentData::DataType::Int, "Actual type doesn't match requested type");
        return data.intVal.unsignedInt;
    }

    int32_t Intent::GetSIntExtra(uint32_t key) const
    {
        if (_Data.count(key) == 0)
        {
            return 0;
        }

        auto data = _Data.at(key);
        openrct2_assert(data.type == IntentData::DataType::Int, "Actual type doesn't match requested type");
        return data.intVal.signedInt;
    }

    std::string Intent::GetStringExtra(uint32_t key) const
    {
        if (_Data.count(key) == 0)
        {
            return std::string{};
        }

        auto data = _Data.at(key);
        openrct2_assert(data.type == IntentData::DataType::String, "Actual type doesn't match requested type");
        return data.stringVal;
    }

    close_callback Intent::GetCloseCallbackExtra(uint32_t key) const
    {
        if (_Data.count(key) == 0)
        {
            return nullptr;
        }

        auto data = _Data.at(key);
        openrct2_assert(data.type == IntentData::DataType::CloseCallback, "Actual type doesn't match requested type");
        return data.closeCallbackVal;
    }
} // namespace OpenRCT2
