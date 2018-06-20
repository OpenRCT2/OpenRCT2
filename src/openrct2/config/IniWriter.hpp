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

interface IIniWriter
{
    virtual ~IIniWriter() = default;

    virtual void WriteSection(const std::string &name) abstract;

    virtual void WriteBoolean(const std::string &name, bool value) abstract;
    virtual void WriteInt32(const std::string &name, int32_t value) abstract;
    virtual void WriteFloat(const std::string &name, float value) abstract;
    virtual void WriteString(const std::string &name, const std::string &value) abstract;
    virtual void WriteEnum(const std::string &name, const std::string &key) abstract;

    template<typename T>
    void WriteEnum(const std::string &name, T value, const IConfigEnum<T> &configEnum)
    {
        std::string key = configEnum.GetName(value);
        if (key.empty())
        {
            WriteInt32(name, value);
        }
        else
        {
            WriteEnum(name, key);
        }
    }

    void WriteString(const std::string &name, const utf8 * value);
};

IIniWriter * CreateIniWriter(IStream * stream);
