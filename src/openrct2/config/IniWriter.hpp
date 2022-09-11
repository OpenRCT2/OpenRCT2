/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../core/String.hpp"

#include <memory>
#include <string>

namespace OpenRCT2
{
    struct IStream;
}

template<typename T> struct IConfigEnum;

struct IIniWriter
{
    virtual ~IIniWriter() = default;

    virtual void WriteSection(std::string_view name) abstract;

    virtual void WriteBoolean(std::string_view name, bool value) abstract;
    virtual void WriteInt32(std::string_view name, int32_t value) abstract;
    virtual void WriteInt64(std::string_view name, int64_t value) abstract;
    virtual void WriteFloat(std::string_view name, float value) abstract;
    virtual void WriteString(std::string_view name, std::string_view value) abstract;
    virtual void WriteEnum(std::string_view name, std::string_view key) abstract;

    template<typename T> void WriteEnum(std::string_view name, T value, const IConfigEnum<T>& configEnum)
    {
        static_assert(sizeof(T) <= sizeof(int32_t), "Type too large");

        std::string key = configEnum.GetName(value);
        if (key.empty())
        {
            WriteInt32(name, static_cast<int32_t>(value));
        }
        else
        {
            WriteEnum(name, key);
        }
    }
};

[[nodiscard]] std::unique_ptr<IIniWriter> CreateIniWriter(OpenRCT2::IStream* stream);
