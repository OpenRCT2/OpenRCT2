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
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../core/StringBuilder.hpp"
#include "../interface/Fonts.h"
#include "../interface/FontFamilies.h"
#include "../object/ObjectManager.h"
#include "LanguagePack.h"

#include "../platform/platform.h"
#include "Localisation.h"

// clang-format off
const language_descriptor LanguagesDescriptors[LANGUAGE_COUNT] =
{
    { "",       "",                     "",                      FAMILY_OPENRCT2_SPRITE,                RCT2_LANGUAGE_ID_ENGLISH_UK },          // LANGUAGE_UNDEFINED
    { "ar-EG", "Arabic (experimental)", "Arabic (experimental)", FAMILY(&TTFFamilySansSerif),           RCT2_LANGUAGE_ID_ENGLISH_UK },          // LANGUAGE_ARABIC
    { "ca-ES", "Catalan",               u8"Català",              FAMILY_OPENRCT2_SPRITE,                RCT2_LANGUAGE_ID_SPANISH },             // LANGUAGE_CATALAN
    { "zh-CN", "Chinese (Simplified)",  "Chinese (Simplified)",  FAMILY(&TTFFamilyChineseSimplified),   RCT2_LANGUAGE_ID_CHINESE_SIMPLIFIED },  // LANGUAGE_CHINESE_SIMPLIFIED
    { "zh-TW", "Chinese (Traditional)", "Chinese (Traditional)", FAMILY(&TTFFamilyChineseTraditional),  RCT2_LANGUAGE_ID_CHINESE_TRADITIONAL }, // LANGUAGE_CHINESE_TRADITIONAL
    { "cs-CZ", "Czech",                 "Czech",                 FAMILY(&TTFFamilySansSerif),           RCT2_LANGUAGE_ID_ENGLISH_UK },          // LANGUAGE_CZECH
    { "de-DE", "German",                "Deutsch",               FAMILY_OPENRCT2_SPRITE,                RCT2_LANGUAGE_ID_GERMAN },              // LANGUAGE_GERMAN
    { "en-GB", "English (UK)",          "English (UK)",          FAMILY_OPENRCT2_SPRITE,                RCT2_LANGUAGE_ID_ENGLISH_UK },          // LANGUAGE_ENGLISH_UK
    { "en-US", "English (US)",          "English (US)",          FAMILY_OPENRCT2_SPRITE,                RCT2_LANGUAGE_ID_ENGLISH_US },          // LANGUAGE_ENGLISH_US
    { "es-ES", "Spanish",               u8"Español",             FAMILY_OPENRCT2_SPRITE,                RCT2_LANGUAGE_ID_SPANISH },             // LANGUAGE_SPANISH
    { "fr-FR", "French",                u8"Français",            FAMILY_OPENRCT2_SPRITE,                RCT2_LANGUAGE_ID_FRENCH },              // LANGUAGE_FRENCH
    { "it-IT", "Italian",               "Italiano",              FAMILY_OPENRCT2_SPRITE,                RCT2_LANGUAGE_ID_ITALIAN },             // LANGUAGE_ITALIAN
    { "ja-JP", "Japanese",              "Japanese",              FAMILY(&TTFFamilyJapanese),            RCT2_LANGUAGE_ID_ENGLISH_UK },          // LANGUAGE_JAPANESE
    { "ko-KR", "Korean",                "Korean",                FAMILY(&TTFFamilyKorean),              RCT2_LANGUAGE_ID_KOREAN },              // LANGUAGE_KOREAN
    { "hu-HU", "Hungarian",             "Magyar",                FAMILY_OPENRCT2_SPRITE,                RCT2_LANGUAGE_ID_ENGLISH_UK },          // LANGUAGE_HUNGARIAN
    { "nl-NL", "Dutch",                 "Nederlands",            FAMILY_OPENRCT2_SPRITE,                RCT2_LANGUAGE_ID_DUTCH },               // LANGUAGE_DUTCH
    { "nb-NO", "Norwegian",             "Norsk",                 FAMILY_OPENRCT2_SPRITE,                RCT2_LANGUAGE_ID_ENGLISH_UK },          // LANGUAGE_NORWEGIAN
    { "pl-PL", "Polish",                "Polski",                FAMILY_OPENRCT2_SPRITE,                RCT2_LANGUAGE_ID_ENGLISH_UK },          // LANGUAGE_POLISH
    { "pt-BR", "Portuguese (BR)",       u8"Português (BR)",      FAMILY_OPENRCT2_SPRITE,                RCT2_LANGUAGE_ID_PORTUGUESE },          // LANGUAGE_PORTUGUESE_BR
    { "ru-RU", "Russian",               "Russian",               FAMILY(&TTFFamilySansSerif),           RCT2_LANGUAGE_ID_ENGLISH_UK },          // LANGUAGE_RUSSIAN
    { "fi-FI", "Finnish",               "Suomi",                 FAMILY_OPENRCT2_SPRITE,                RCT2_LANGUAGE_ID_ENGLISH_UK },          // LANGUAGE_FINNISH
    { "sv-SE", "Swedish",               "Svenska",               FAMILY_OPENRCT2_SPRITE,                RCT2_LANGUAGE_ID_SWEDISH },             // LANGUAGE_SWEDISH
};
// clang-format on

sint32 gCurrentLanguage = LANGUAGE_UNDEFINED;
bool gUseTrueTypeFont = false;

static ILanguagePack * _languageFallback = nullptr;
static ILanguagePack * _languageCurrent = nullptr;

// clang-format off
const utf8 BlackUpArrowString[] =       { (utf8)(uint8)0xC2, (utf8)(uint8)0x8E, (utf8)(uint8)0xE2, (utf8)(uint8)0x96, (utf8)(uint8)0xB2, (utf8)(uint8)0x00 };
const utf8 BlackDownArrowString[] =     { (utf8)(uint8)0xC2, (utf8)(uint8)0x8E, (utf8)(uint8)0xE2, (utf8)(uint8)0x96, (utf8)(uint8)0xBC, (utf8)(uint8)0x00 };
const utf8 BlackLeftArrowString[] =     { (utf8)(uint8)0xC2, (utf8)(uint8)0x8E, (utf8)(uint8)0xE2, (utf8)(uint8)0x97, (utf8)(uint8)0x80, (utf8)(uint8)0x00 };
const utf8 BlackRightArrowString[] =    { (utf8)(uint8)0xC2, (utf8)(uint8)0x8E, (utf8)(uint8)0xE2, (utf8)(uint8)0x96, (utf8)(uint8)0xB6, (utf8)(uint8)0x00 };
const utf8 CheckBoxMarkString[] =       { (utf8)(uint8)0xE2, (utf8)(uint8)0x9C, (utf8)(uint8)0x93, (utf8)(uint8)0x00 };
// clang-format on

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

uint8 language_get_id_from_locale(const char * locale)
{
    uint8 i = 0;
    for (const auto &langDesc : LanguagesDescriptors)
    {
        if (String::Equals(locale, langDesc.locale))
        {
            return i;
        }
        i++;
    }
    return LANGUAGE_UNDEFINED;
}

const char * language_get_string(rct_string_id id)
{
    const char * result = nullptr;
    if (id == STR_EMPTY)
    {
        result = "";
    }
    else if (id != STR_NONE)
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

    return false;
}

void language_close_all()
{
    SafeDelete(_languageFallback);
    SafeDelete(_languageCurrent);
    gCurrentLanguage = LANGUAGE_UNDEFINED;
}

constexpr rct_string_id NONSTEX_BASE_STRING_ID = 3463;
constexpr uint16        MAX_OBJECT_CACHED_STRINGS = 2048;

static wchar_t convert_specific_language_character_to_unicode(RCT2LanguageId languageId, wchar_t codepoint)
{
    switch (languageId) {
    case RCT2_LANGUAGE_ID_CHINESE_TRADITIONAL:
        return encoding_convert_big5_to_unicode(codepoint);
    case RCT2_LANGUAGE_ID_CHINESE_SIMPLIFIED:
        return encoding_convert_gb2312_to_unicode(codepoint);
    case RCT2_LANGUAGE_ID_JAPANESE:
        return encoding_convert_cp932_to_unicode(codepoint);
    case RCT2_LANGUAGE_ID_KOREAN:
        return encoding_convert_cp949_to_unicode(codepoint);
    default:
        return codepoint;
    }
}

static utf8 * convert_multibyte_charset(const char * src, size_t srcMaxSize, RCT2LanguageId languageId)
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

static bool rct2_language_is_multibyte_charset(RCT2LanguageId languageId)
{
    switch (languageId) {
    case RCT2_LANGUAGE_ID_KOREAN:
    case RCT2_LANGUAGE_ID_CHINESE_TRADITIONAL:
    case RCT2_LANGUAGE_ID_CHINESE_SIMPLIFIED:
    case RCT2_LANGUAGE_ID_JAPANESE:
        return true;
    default:
        return false;
    }
}

utf8 * rct2_language_string_to_utf8(const char *src, size_t srcSize, RCT2LanguageId languageId)
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

void language_free_object_string(rct_string_id stringId)
{
    if (stringId != 0)
    {
        if (_languageCurrent != nullptr)
        {
            _languageCurrent->RemoveString(stringId);
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

rct_string_id language_allocate_object_string(const std::string &target)
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
