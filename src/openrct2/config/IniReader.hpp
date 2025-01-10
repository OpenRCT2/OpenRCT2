/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <memory>
#include <string>

namespace OpenRCT2
{
    struct IStream;
}

template<typename T>
struct IConfigEnum;

struct IIniReader
{
    virtual ~IIniReader() = default;

    virtual bool ReadSection(const std::string& name) = 0;

    virtual bool GetBoolean(const std::string& name, bool defaultValue) const = 0;
    virtual int32_t GetInt32(const std::string& name, int32_t defaultValue) const = 0;
    virtual int64_t GetInt64(const std::string& name, int64_t defaultValue) const = 0;
    virtual float GetFloat(const std::string& name, float defaultValue) const = 0;
    virtual std::string GetString(const std::string& name, const std::string& defaultValue) const = 0;
    virtual bool TryGetString(const std::string& name, std::string* outValue) const = 0;

    template<typename T>
    T GetEnum(const std::string& name, T defaultValue, const IConfigEnum<T>& configEnum) const
    {
        std::string szValue;
        if (!TryGetString(name, &szValue))
        {
            return defaultValue;
        }

        return configEnum.GetValue(szValue, defaultValue);
    }
};

[[nodiscard]] std::unique_ptr<IIniReader> CreateIniReader(OpenRCT2::IStream* stream);
[[nodiscard]] std::unique_ptr<IIniReader> CreateDefaultIniReader();
