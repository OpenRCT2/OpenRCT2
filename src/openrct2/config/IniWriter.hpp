/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/StringTypes.h"

#include <memory>
#include <string>

namespace OpenRCT2
{
    struct IStream;
}

template<typename T>
struct IConfigEnum;

struct IIniWriter
{
    virtual ~IIniWriter() = default;

    virtual void WriteSection(const std::string& name) = 0;

    virtual void WriteBoolean(const std::string& name, bool value) = 0;
    virtual void WriteInt32(const std::string& name, int32_t value) = 0;
    virtual void WriteInt64(const std::string& name, int64_t value) = 0;
    virtual void WriteFloat(const std::string& name, float value) = 0;
    virtual void WriteString(const std::string& name, const std::string& value) = 0;
    virtual void WriteEnum(const std::string& name, const std::string& key) = 0;

    template<typename T>
    void WriteEnum(const std::string& name, T value, const IConfigEnum<T>& configEnum)
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

    void WriteString(const std::string& name, const utf8* value);
};

[[nodiscard]] std::unique_ptr<IIniWriter> CreateIniWriter(OpenRCT2::IStream* stream);
