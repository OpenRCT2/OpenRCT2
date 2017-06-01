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

interface IIniReader
{
    virtual ~IIniReader() = default;

    virtual bool ReadSection(const std::string &name) abstract;

    virtual bool GetBoolean(const std::string &name, bool defaultValue) const abstract;
    virtual sint32 GetSint32(const std::string &name, sint32 defaultValue) const abstract;
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
