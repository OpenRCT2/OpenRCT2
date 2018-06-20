/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <string>
#include "../common.h"

interface IStream;
template <typename T> struct IConfigEnum;

interface IIniReader
{
    virtual ~IIniReader() = default;

    virtual bool ReadSection(const std::string &name) abstract;

    virtual bool GetBoolean(const std::string &name, bool defaultValue) const abstract;
    virtual int32_t GetInt32(const std::string &name, int32_t defaultValue) const abstract;
    virtual float GetFloat(const std::string &name, float defaultValue) const abstract;
    virtual std::string GetString(const std::string &name, const std::string &defaultValue) const abstract;
    virtual bool TryGetString(const std::string &name, std::string * outValue) const abstract;

    template<typename T>
    T GetEnum(const std::string &name, T defaultValue, const IConfigEnum<T> &configEnum) const
    {
        std::string szValue;
        if (!TryGetString(name, &szValue))
        {
            return defaultValue;
        }

        return configEnum.GetValue(szValue, defaultValue);
    }

    utf8 * GetCString(const std::string &name, const utf8 * defaultValue) const;
};

IIniReader * CreateIniReader(IStream * stream);
IIniReader * CreateDefaultIniReader();
