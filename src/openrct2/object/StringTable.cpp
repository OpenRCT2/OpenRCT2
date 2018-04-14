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

#include <algorithm>
#include "../core/IStream.hpp"
#include "../core/String.hpp"
#include "../localisation/Language.h"
#include "../localisation/LanguagePack.h"
#include "Object.h"
#include "StringTable.h"

static constexpr const uint8 RCT2ToOpenRCT2LanguageId[] =
{
    LANGUAGE_ENGLISH_UK,
    LANGUAGE_ENGLISH_US,
    LANGUAGE_FRENCH,
    LANGUAGE_GERMAN,
    LANGUAGE_SPANISH,
    LANGUAGE_ITALIAN,
    LANGUAGE_DUTCH,
    LANGUAGE_SWEDISH,
    LANGUAGE_JAPANESE,
    LANGUAGE_KOREAN,
    LANGUAGE_CHINESE_SIMPLIFIED,
    LANGUAGE_CHINESE_TRADITIONAL,
    LANGUAGE_UNDEFINED,
    LANGUAGE_PORTUGUESE_BR,
};

static bool StringIsBlank(const utf8 * str)
{
    for (auto ch = str; *ch != '\0'; ch++)
    {
        if (!isblank((uint8)*ch))
        {
            return false;
        }
    }
    return true;
}

void StringTable::Read(IReadObjectContext * context, IStream * stream, uint8 id)
{
    try
    {
        RCT2LanguageId rct2LanguageId;
        while ((rct2LanguageId = (RCT2LanguageId)stream->ReadValue<uint8>()) != RCT2_LANGUAGE_ID_END)
        {
            uint8 languageId =
                (rct2LanguageId <= RCT2_LANGUAGE_ID_PORTUGUESE) ?
                RCT2ToOpenRCT2LanguageId[rct2LanguageId] :
                LANGUAGE_UNDEFINED;
            StringTableEntry entry;
            entry.Id = id;
            entry.LanguageId = languageId;

            std::string stringAsWin1252 = stream->ReadStdString();
            auto stringAsUtf8 = rct2_to_utf8(stringAsWin1252, rct2LanguageId);

            if (StringIsBlank(stringAsUtf8.data()))
            {
                entry.LanguageId = LANGUAGE_UNDEFINED;
            }
            stringAsUtf8 = String::Trim(stringAsUtf8);

            entry.Text = stringAsUtf8;
            _strings.push_back(entry);
        }
    }
    catch (const std::exception &)
    {
        context->LogError(OBJECT_ERROR_BAD_STRING_TABLE, "Bad string table.");
        throw;
    }
    Sort();
}

std::string StringTable::GetString(uint8 id) const
{
    for (auto &string : _strings)
    {
        if (string.Id == id)
        {
            return string.Text;
        }
    }
    return std::string();
}

void StringTable::SetString(uint8 id, uint8 language, const std::string &text)
{
    StringTableEntry entry;
    entry.Id = id;
    entry.LanguageId = language;
    entry.Text = String::Duplicate(text);
    _strings.push_back(entry);
}

void StringTable::Sort()
{
    std::sort(_strings.begin(), _strings.end(), [](const StringTableEntry &a, const StringTableEntry &b) -> bool
    {
        if (a.Id == b.Id)
        {
            if (a.LanguageId == b.LanguageId)
            {
                return String::Compare(a.Text, b.Text, true) < 0;
            }

            if (a.LanguageId == gCurrentLanguage)
            {
                return true;
            }
            if (b.LanguageId == gCurrentLanguage)
            {
                return false;
            }

            if (a.LanguageId == LANGUAGE_ENGLISH_UK)
            {
                return true;
            }
            if (b.LanguageId == LANGUAGE_ENGLISH_UK)
            {
                return false;
            }

            return a.LanguageId < b.LanguageId;
        }
        return a.Id < b.Id;
    });
}
