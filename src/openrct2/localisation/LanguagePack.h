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

#pragma once

#ifdef __cplusplus

#include "../common.h"

enum
{
    RCT2_LANGUAGE_ID_ENGLISH_UK,
    RCT2_LANGUAGE_ID_ENGLISH_US,
    RCT2_LANGUAGE_ID_FRENCH,
    RCT2_LANGUAGE_ID_GERMAN,
    RCT2_LANGUAGE_ID_SPANISH,
    RCT2_LANGUAGE_ID_ITALIAN,
    RCT2_LANGUAGE_ID_DUTCH,
    RCT2_LANGUAGE_ID_SWEDISH,
    RCT2_LANGUAGE_ID_JAPANESE,
    RCT2_LANGUAGE_ID_KOREAN,
    RCT2_LANGUAGE_ID_CHINESE_SIMPLIFIED,
    RCT2_LANGUAGE_ID_CHINESE_TRADITIONAL,
    RCT2_LANGUAGE_ID_12,
    RCT2_LANGUAGE_ID_PORTUGUESE,
    RCT2_LANGUAGE_ID_BLANK = 254,
    RCT2_LANGUAGE_ID_END = 255
};

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

#endif
