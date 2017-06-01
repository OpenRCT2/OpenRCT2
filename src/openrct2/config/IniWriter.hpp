#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include <string>
#include "../common.h"
#include "ConfigEnum.hpp"

interface IStream;

interface IIniWriter
{
    virtual ~IIniWriter() = default;

    virtual void WriteSection(const std::string &name) abstract;

    virtual void WriteBoolean(const std::string &name, bool value) abstract;
    virtual void WriteSint32(const std::string &name, sint32 value) abstract;
    virtual void WriteFloat(const std::string &name, float value) abstract;
    virtual void WriteString(const std::string &name, const std::string &value) abstract;
    virtual void WriteEnum(const std::string &name, const std::string &key) abstract;

    template<typename T>
    void WriteEnum(const std::string &name, T value, const IConfigEnum<T> &configEnum)
    {
        std::string key = configEnum.GetName(value);
        if (key.empty())
        {
            WriteSint32(name, value);
        }
        else
        {
            WriteEnum(name, key);
        }
    }

    void WriteString(const std::string &name, const utf8 * value);
};

IIniWriter * CreateIniWriter(IStream * stream);
