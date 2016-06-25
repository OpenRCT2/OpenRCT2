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

#include <algorithm>
#include "../core/IStream.hpp"
#include "../core/String.hpp"
#include "../localisation/LanguagePack.h"
#include "StringTable.h"

constexpr uint8 RCT2_LANGUAGE_ID_ENGLISH_UK = 0;
constexpr uint8 RCT2_LANGUAGE_ID_BLANK = 254;
constexpr uint8 RCT2_LANGUAGE_ID_END = 255;

bool StringIsBlank(utf8 * str)
{
    for (utf8 * ch = str; *ch != '\0'; ch++)
    {
        if (!isblank(*ch))
        {
            return false;
        }
    }
    return true;
}

void StringTable::Read(IStream * stream, uint8 id)
{
    uint8 languageId;
    while ((languageId = stream->ReadValue<uint8>()) != RCT2_LANGUAGE_ID_END)
    {
        StringTableEntry entry;
        entry.Id = id;
        entry.LanguageId = languageId;
        entry.Text = stream->ReadString();

        if (StringIsBlank(entry.Text))
        {
            entry.LanguageId = RCT2_LANGUAGE_ID_BLANK;
        }

        _strings.push_back(entry);
    }
    Sort();
}

void StringTable::Sort()
{
    std::sort(_strings.begin(), _strings.end(), [](const StringTableEntry &a, const StringTableEntry &b) -> int
    {
        if (a.Id == b.Id)
        {
            if (a.LanguageId == b.LanguageId)
            {
                return _strcmpi(a.Text, b.Text);
            }
            if (a.LanguageId == LanguagesDescriptors[gCurrentLanguage].rct2_original_id)
            {
                return -1;
            }
            if (a.LanguageId == RCT2_LANGUAGE_ID_ENGLISH_UK)
            {
                return -1;
            }
            return 1;
        }
        return a.Id - b.Id;
    });
}
