#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#pragma once

#include "../common.h"

interface ILanguagePack
{
    virtual ~ILanguagePack() = default;

    virtual uint16 GetId() const abstract;
    virtual uint32 GetCount() const abstract;

    virtual void            SetString(rct_string_id stringId, const utf8 * str) abstract;
    virtual const utf8 *    GetString(rct_string_id stringId) const abstract;
    virtual rct_string_id   GetObjectOverrideStringId(const char * objectIdentifier, uint8 index) abstract;
    virtual rct_string_id   GetScenarioOverrideStringId(const utf8 * scenarioFilename, uint8 index) abstract;
};

namespace LanguagePackFactory
{
    ILanguagePack * FromFile(uint16 id, const utf8 * path);
    ILanguagePack * FromText(uint16 id, const utf8 * text);
}
