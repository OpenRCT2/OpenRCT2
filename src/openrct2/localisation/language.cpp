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

#include <stack>
#include "../core/Console.hpp"
#include "../core/Memory.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../core/StringBuilder.hpp"
#include "../interface/Fonts.h"
#include "../object/ObjectManager.h"
#include "LanguagePack.h"

extern "C" {

#include "../platform/platform.h"
#include "localisation.h"

sint32 gCurrentLanguage = LANGUAGE_UNDEFINED;
bool gUseTrueTypeFont = false;

static ILanguagePack * _languageFallback = nullptr;
static ILanguagePack * _languageCurrent = nullptr;

const utf8 BlackUpArrowString[] =       { (utf8)(uint8)0xC2, (utf8)(uint8)0x8E, (utf8)(uint8)0xE2, (utf8)(uint8)0x96, (utf8)(uint8)0xB2, (utf8)(uint8)0x00 };
const utf8 BlackDownArrowString[] =     { (utf8)(uint8)0xC2, (utf8)(uint8)0x8E, (utf8)(uint8)0xE2, (utf8)(uint8)0x96, (utf8)(uint8)0xBC, (utf8)(uint8)0x00 };
const utf8 BlackLeftArrowString[] =     { (utf8)(uint8)0xC2, (utf8)(uint8)0x8E, (utf8)(uint8)0xE2, (utf8)(uint8)0x97, (utf8)(uint8)0x80, (utf8)(uint8)0x00 };
const utf8 BlackRightArrowString[] =    { (utf8)(uint8)0xC2, (utf8)(uint8)0x8E, (utf8)(uint8)0xE2, (utf8)(uint8)0x96, (utf8)(uint8)0xB6, (utf8)(uint8)0x00 };
const utf8 CheckBoxMarkString[] =       { (utf8)(uint8)0xE2, (utf8)(uint8)0x9C, (utf8)(uint8)0x93, (utf8)(uint8)0x00 };

void utf8_remove_format_codes(utf8 * text, bool allowcolours)
{
    const utf8 * ch = text;
    utf8 * dstCh = text;
    sint32 codepoint;
    while ((codepoint = String::GetNextCodepoint(ch, &ch)) != 0)
    {
        if (!utf8_is_format_code(codepoint) || (allowcolours && utf8_is_colour_code(codepoint)))
        {
            dstCh = String::WriteCodepoint(dstCh, codepoint);
        }
    }
    *dstCh = 0;
}

const char * language_get_string(rct_string_id id)
{
    const char * result = nullptr;
    if (id != STR_NONE)
    {
        if (_languageCurrent != nullptr)
        {
            result = _languageCurrent->GetString(id);
        }
        if (result == nullptr && _languageFallback != nullptr)
        {
            result = _languageFallback->GetString(id);
        }
        if (result == nullptr)
        {
            result = "(undefined string)";
        }
    }
    return result;
}

static utf8 * GetLanguagePath(utf8 * buffer, size_t bufferSize, uint32 languageId)
{
    const char * locale = LanguagesDescriptors[languageId].locale;

    platform_get_openrct_data_path(buffer, bufferSize);
    Path::Append(buffer, bufferSize, "language");
    Path::Append(buffer, bufferSize, locale);
    String::Append(buffer, bufferSize, ".txt");
    return buffer;
}

bool language_open(sint32 id)
{
    char filename[MAX_PATH];

    language_close_all();
    if (id == LANGUAGE_UNDEFINED)
    {
        return false;
    }

    if (id != LANGUAGE_ENGLISH_UK)
    {
        GetLanguagePath(filename, sizeof(filename), LANGUAGE_ENGLISH_UK);
        _languageFallback = LanguagePackFactory::FromFile(LANGUAGE_ENGLISH_UK, filename);
    }

    GetLanguagePath(filename, sizeof(filename), id);
    _languageCurrent = LanguagePackFactory::FromFile(id, filename);
    if (_languageCurrent != nullptr)
    {
        gCurrentLanguage = id;
        TryLoadFonts();

        // Objects and their localised strings need to be refreshed
        GetObjectManager()->ResetObjects();
        return true;
    }

    return 0;
}

void language_close_all()
{
    SafeDelete(_languageFallback);
    SafeDelete(_languageCurrent);
    gCurrentLanguage = LANGUAGE_UNDEFINED;
}

constexpr rct_string_id NONSTEX_BASE_STRING_ID = 3463;
constexpr uint16        MAX_OBJECT_CACHED_STRINGS = 2048;

static wchar_t convert_specific_language_character_to_unicode(sint32 languageId, wchar_t codepoint)
{
    switch (languageId) {
    case RCT2_LANGUAGE_ID_KOREAN:
        return codepoint;
    case RCT2_LANGUAGE_ID_CHINESE_TRADITIONAL:
        return encoding_convert_big5_to_unicode(codepoint);
    case RCT2_LANGUAGE_ID_CHINESE_SIMPLIFIED:
        return encoding_convert_gb2312_to_unicode(codepoint);
    default:
        return codepoint;
    }
}

static utf8 * convert_multibyte_charset(const char * src, size_t srcMaxSize, sint32 languageId)
{
    constexpr char CODEPOINT_DOUBLEBYTE = (char)(uint8)0xFF;

    auto sb = StringBuilder(64);
    for (const char * ch = src; (ch < src + srcMaxSize) && (*ch != '\0');)
    {
        if (*ch == CODEPOINT_DOUBLEBYTE)
        {
            ch++;
            if (ch < src + srcMaxSize)
            {
                uint8 a = *ch++;
                if (a != '\0')
                {
                    uint8 b = *ch++;
                    wchar_t codepoint16 = (wchar_t)((a << 8) | b);

                    codepoint16 = convert_specific_language_character_to_unicode(languageId, codepoint16);
                    sb.Append(codepoint16);
                }
            }
        }
        else
        {
            codepoint_t codepoint = (uint8)*ch++;
            sb.Append(codepoint);
        }
    }
    return sb.StealString();
}

static bool rct2_language_is_multibyte_charset(sint32 languageId)
{
    switch (languageId) {
    case RCT2_LANGUAGE_ID_KOREAN:
    case RCT2_LANGUAGE_ID_CHINESE_TRADITIONAL:
    case RCT2_LANGUAGE_ID_CHINESE_SIMPLIFIED:
    case RCT2_LANGUAGE_ID_8:
        return true;
    default:
        return false;
    }
}

utf8 *rct2_language_string_to_utf8(const char *src, size_t srcSize, sint32 languageId)
{
    if (rct2_language_is_multibyte_charset(languageId))
    {
        return convert_multibyte_charset(src, srcSize, languageId);
    }
    else
    {
        return win1252_to_utf8_alloc(src, srcSize);
    }
}

bool language_get_localised_scenario_strings(const utf8 *scenarioFilename, rct_string_id *outStringIds)
{
    outStringIds[0] = _languageCurrent->GetScenarioOverrideStringId(scenarioFilename, 0);
    outStringIds[1] = _languageCurrent->GetScenarioOverrideStringId(scenarioFilename, 1);
    outStringIds[2] = _languageCurrent->GetScenarioOverrideStringId(scenarioFilename, 2);
    return
        outStringIds[0] != STR_NONE ||
        outStringIds[1] != STR_NONE ||
        outStringIds[2] != STR_NONE;
}

static bool                         _availableObjectStringIdsInitialised = false;
static std::stack<rct_string_id>    _availableObjectStringIds;

rct_string_id language_allocate_object_string(const utf8 * target)
{
    if (!_availableObjectStringIdsInitialised)
    {
        _availableObjectStringIdsInitialised = true;
        for (rct_string_id stringId = NONSTEX_BASE_STRING_ID + MAX_OBJECT_CACHED_STRINGS; stringId >= NONSTEX_BASE_STRING_ID; stringId--)
        {
            _availableObjectStringIds.push(stringId);
        }
    }

    rct_string_id stringId = _availableObjectStringIds.top();
    _availableObjectStringIds.pop();
    _languageCurrent->SetString(stringId, target);
    return stringId;
}

void language_free_object_string(rct_string_id stringId)
{
    if (stringId != 0)
    {
        if (_languageCurrent != nullptr)
        {
            _languageCurrent->SetString(stringId, nullptr);
        }
        _availableObjectStringIds.push(stringId);
    }
}

rct_string_id language_get_object_override_string_id(const char * identifier, uint8 index)
{
    if (_languageCurrent == nullptr)
    {
        return STR_NONE;
    }
    return _languageCurrent->GetObjectOverrideStringId(identifier, index);
}

}
