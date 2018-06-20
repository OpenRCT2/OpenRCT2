/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <stack>
#include "../Context.h"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../interface/Fonts.h"
#include "../interface/FontFamilies.h"
#include "../object/ObjectManager.h"
#include "../platform/platform.h"
#include "LanguagePack.h"
#include "Localisation.h"
#include "LocalisationService.h"

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

// clang-format off
const utf8 BlackUpArrowString[] =       { (utf8)(uint8_t)0xC2, (utf8)(uint8_t)0x8E, (utf8)(uint8_t)0xE2, (utf8)(uint8_t)0x96, (utf8)(uint8_t)0xB2, (utf8)(uint8_t)0x00 };
const utf8 BlackDownArrowString[] =     { (utf8)(uint8_t)0xC2, (utf8)(uint8_t)0x8E, (utf8)(uint8_t)0xE2, (utf8)(uint8_t)0x96, (utf8)(uint8_t)0xBC, (utf8)(uint8_t)0x00 };
const utf8 BlackLeftArrowString[] =     { (utf8)(uint8_t)0xC2, (utf8)(uint8_t)0x8E, (utf8)(uint8_t)0xE2, (utf8)(uint8_t)0x97, (utf8)(uint8_t)0x80, (utf8)(uint8_t)0x00 };
const utf8 BlackRightArrowString[] =    { (utf8)(uint8_t)0xC2, (utf8)(uint8_t)0x8E, (utf8)(uint8_t)0xE2, (utf8)(uint8_t)0x96, (utf8)(uint8_t)0xB6, (utf8)(uint8_t)0x00 };
const utf8 CheckBoxMarkString[] =       { (utf8)(uint8_t)0xE2, (utf8)(uint8_t)0x9C, (utf8)(uint8_t)0x93, (utf8)(uint8_t)0x00 };
// clang-format on

void utf8_remove_format_codes(utf8 * text, bool allowcolours)
{
    const utf8 * ch = text;
    utf8 * dstCh = text;
    int32_t codepoint;
    while ((codepoint = String::GetNextCodepoint(ch, &ch)) != 0)
    {
        if (!utf8_is_format_code(codepoint) || (allowcolours && utf8_is_colour_code(codepoint)))
        {
            dstCh = String::WriteCodepoint(dstCh, codepoint);
        }
    }
    *dstCh = 0;
}

uint8_t language_get_id_from_locale(const char * locale)
{
    uint8_t i = 0;
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
    const auto& localisationService = OpenRCT2::GetContext()->GetLocalisationService();
    return localisationService.GetString(id);
}

bool language_open(int32_t id)
{
    auto context = OpenRCT2::GetContext();
    auto& localisationService = context->GetLocalisationService();
    auto objectManager = context->GetObjectManager();
    try
    {
        localisationService.OpenLanguage(id, *objectManager);
        return true;
    }
    catch (const std::exception&)
    {
        return false;
    }
}

bool language_get_localised_scenario_strings(const utf8 *scenarioFilename, rct_string_id *outStringIds)
{
    const auto& localisationService = OpenRCT2::GetContext()->GetLocalisationService();
    auto result = localisationService.GetLocalisedScenarioStrings(scenarioFilename);
    outStringIds[0] = std::get<0>(result);
    outStringIds[1] = std::get<1>(result);
    outStringIds[2] = std::get<2>(result);
    return
        outStringIds[0] != STR_NONE ||
        outStringIds[1] != STR_NONE ||
        outStringIds[2] != STR_NONE;
}

void language_free_object_string(rct_string_id stringId)
{
    auto& localisationService = OpenRCT2::GetContext()->GetLocalisationService();
    localisationService.FreeObjectString(stringId);
}

rct_string_id language_get_object_override_string_id(const char * identifier, uint8_t index)
{
    const auto& localisationService = OpenRCT2::GetContext()->GetLocalisationService();
    return localisationService.GetObjectOverrideStringId(identifier, index);
}

rct_string_id language_allocate_object_string(const std::string &target)
{
    auto& localisationService = OpenRCT2::GetContext()->GetLocalisationService();
    return localisationService.AllocateObjectString(target);
}
