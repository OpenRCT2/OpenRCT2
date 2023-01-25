/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "StringTable.h"

#include "../Context.h"
#include "../core/IStream.hpp"
#include "../core/Json.hpp"
#include "../core/String.hpp"
#include "../localisation/Language.h"
#include "../localisation/LanguagePack.h"
#include "../localisation/LocalisationService.h"
#include "Object.h"

#include <algorithm>

static constexpr const uint8_t RCT2ToOpenRCT2LanguageId[] = {
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

static bool StringIsBlank(const utf8* str)
{
    for (auto ch = str; *ch != '\0'; ch++)
    {
        if (!isblank(static_cast<uint8_t>(*ch)))
        {
            return false;
        }
    }
    return true;
}

void StringTable::Read(IReadObjectContext* context, OpenRCT2::IStream* stream, ObjectStringID id)
{
    try
    {
        RCT2LanguageId rct2LanguageId;
        while ((rct2LanguageId = static_cast<RCT2LanguageId>(stream->ReadValue<uint8_t>())) != RCT2LanguageId::End)
        {
            uint8_t languageId = (EnumValue(rct2LanguageId) <= EnumValue(RCT2LanguageId::Portuguese))
                ? RCT2ToOpenRCT2LanguageId[EnumValue(rct2LanguageId)]
                : static_cast<uint8_t>(LANGUAGE_UNDEFINED);
            std::string stringAsWin1252 = stream->ReadStdString();
            auto stringAsUtf8 = RCT2StringToUTF8(stringAsWin1252, rct2LanguageId);

            if (!StringIsBlank(stringAsUtf8.data()))
            {
                stringAsUtf8 = String::Trim(stringAsUtf8);
                StringTableEntry entry{};
                entry.Id = id;
                entry.LanguageId = languageId;
                entry.Text = std::move(stringAsUtf8);
                _strings.push_back(std::move(entry));
            }
        }
    }
    catch (const std::exception&)
    {
        context->LogError(ObjectError::BadStringTable, "Bad string table.");
        throw;
    }
    Sort();
}

ObjectStringID StringTable::ParseStringId(const std::string& s)
{
    if (s == "name")
        return ObjectStringID::NAME;
    if (s == "description")
        return ObjectStringID::DESCRIPTION;
    if (s == "capacity")
        return ObjectStringID::CAPACITY;
    if (s == "vehicleName")
        return ObjectStringID::VEHICLE_NAME;
    return ObjectStringID::UNKNOWN;
}

void StringTable::ReadJson(json_t& root)
{
    Guard::Assert(root.is_object(), "StringTable::ReadJson expects parameter root to be object");

    // We trust the JSON type of root is object
    auto jsonStrings = root["strings"];

    for (auto& [key, jsonLanguages] : jsonStrings.items())
    {
        auto stringId = ParseStringId(key);
        if (stringId != ObjectStringID::UNKNOWN)
        {
            for (auto& [locale, jsonString] : jsonLanguages.items())
            {
                auto langId = LanguageGetIDFromLocale(locale.c_str());
                if (langId != LANGUAGE_UNDEFINED)
                {
                    auto string = Json::GetString(jsonString);
                    SetString(stringId, langId, string);
                }
            }
        }
    }
    Sort();
}

std::string StringTable::GetString(ObjectStringID id) const
{
    for (auto& string : _strings)
    {
        if (string.Id == id)
        {
            return string.Text;
        }
    }
    return std::string();
}

std::string StringTable::GetString(uint8_t language, ObjectStringID id) const
{
    for (auto& string : _strings)
    {
        if (string.LanguageId == language && string.Id == id)
        {
            return string.Text;
        }
    }
    return std::string();
}

void StringTable::SetString(ObjectStringID id, uint8_t language, const std::string& text)
{
    StringTableEntry entry;
    entry.Id = id;
    entry.LanguageId = language;
    entry.Text = text;
    _strings.push_back(std::move(entry));
}

void StringTable::Sort()
{
    const auto& languageOrder = OpenRCT2::GetContext()->GetLocalisationService().GetLanguageOrder();
    std::sort(_strings.begin(), _strings.end(), [languageOrder](const StringTableEntry& a, const StringTableEntry& b) -> bool {
        if (a.Id == b.Id)
        {
            if (a.LanguageId == b.LanguageId)
            {
                return String::Compare(a.Text, b.Text, true) < 0;
            }

            for (const auto& language : languageOrder)
            {
                if (a.LanguageId == language)
                {
                    return true;
                }
                if (b.LanguageId == language)
                {
                    return false;
                }
            }

            return a.LanguageId < b.LanguageId;
        }
        return a.Id < b.Id;
    });
}
