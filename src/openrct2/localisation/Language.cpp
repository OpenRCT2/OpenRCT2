/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Context.h"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../interface/FontFamilies.h"
#include "../interface/Fonts.h"
#include "../object/ObjectManager.h"
#include "../platform/platform.h"
#include "LanguagePack.h"
#include "Localisation.h"
#include "LocalisationService.h"

#include <stack>

// clang-format off
const language_descriptor LanguagesDescriptors[LANGUAGE_COUNT] =
{
    { "",      u8"",                      u8"",                      FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_UNDEFINED
    { "ar-EG", u8"Arabic (experimental)", u8"Arabic (experimental)", FAMILY(&TTFFamilySansSerif),           true  }, // LANGUAGE_ARABIC
    { "ca-ES", u8"Catalan",               u8"Català",                FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_CATALAN
    { "zh-CN", u8"Chinese (Simplified)",  u8"Chinese (Simplified)",  FAMILY(&TTFFamilyChineseSimplified),   false }, // LANGUAGE_CHINESE_SIMPLIFIED
    { "zh-TW", u8"Chinese (Traditional)", u8"Chinese (Traditional)", FAMILY(&TTFFamilyChineseTraditional),  false }, // LANGUAGE_CHINESE_TRADITIONAL
    { "cs-CZ", u8"Czech",                 u8"Čeština",               FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_CZECH
    { "da-DK", u8"Danish",                u8"Dansk",                 FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_DANISH
    { "de-DE", u8"German",                u8"Deutsch",               FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_GERMAN
    { "en-GB", u8"English (UK)",          u8"English (UK)",          FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_ENGLISH_UK
    { "en-US", u8"English (US)",          u8"English (US)",          FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_ENGLISH_US
    { "eo-OO", u8"Esperanto",             u8"Esperanto",             FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_ESPERANTO
    { "es-ES", u8"Spanish",               u8"Español",               FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_SPANISH
    { "fr-FR", u8"French",                u8"Français",              FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_FRENCH
    { "it-IT", u8"Italian",               u8"Italiano",              FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_ITALIAN
    { "ja-JP", u8"Japanese",              u8"Japanese",              FAMILY(&TTFFamilyJapanese),            false }, // LANGUAGE_JAPANESE
    { "ko-KR", u8"Korean",                u8"Korean",                FAMILY(&TTFFamilyKorean),              false }, // LANGUAGE_KOREAN
    { "hu-HU", u8"Hungarian",             u8"Magyar",                FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_HUNGARIAN
    { "nl-NL", u8"Dutch",                 u8"Nederlands",            FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_DUTCH
    { "nb-NO", u8"Norwegian",             u8"Norsk",                 FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_NORWEGIAN
    { "pl-PL", u8"Polish",                u8"Polski",                FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_POLISH
    { "pt-BR", u8"Portuguese (BR)",       u8"Português (BR)",        FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_PORTUGUESE_BR
    { "ru-RU", u8"Russian",               u8"Русский",               FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_RUSSIAN
    { "fi-FI", u8"Finnish",               u8"Suomi",                 FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_FINNISH
    { "sv-SE", u8"Swedish",               u8"Svenska",               FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_SWEDISH
    { "tr-TR", u8"Turkish",               u8"Türkçe",                FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_TURKISH
    { "vi-VN", u8"Vietnamese",            u8"Vietnamese",            FAMILY(&TTFFamilySansSerif),           false }, // LANGUAGE_VIETNAMESE
};
// clang-format on

uint8_t language_get_id_from_locale(const char* locale)
{
    uint8_t i = 0;
    for (const auto& langDesc : LanguagesDescriptors)
    {
        if (String::Equals(locale, langDesc.locale))
        {
            return i;
        }
        i++;
    }
    return LANGUAGE_UNDEFINED;
}

const char* language_get_string(rct_string_id id)
{
    const auto& localisationService = OpenRCT2::GetContext()->GetLocalisationService();
    return localisationService.GetString(id);
}

bool language_open(int32_t id)
{
    auto context = OpenRCT2::GetContext();
    auto& localisationService = context->GetLocalisationService();
    auto& objectManager = context->GetObjectManager();
    try
    {
        localisationService.OpenLanguage(id);
        // Objects and their localised strings need to be refreshed
        objectManager.ResetObjects();
        return true;
    }
    catch (const std::exception&)
    {
        return false;
    }
}

bool language_get_localised_scenario_strings(const utf8* scenarioFilename, rct_string_id* outStringIds)
{
    const auto& localisationService = OpenRCT2::GetContext()->GetLocalisationService();
    auto result = localisationService.GetLocalisedScenarioStrings(scenarioFilename);
    outStringIds[0] = std::get<0>(result);
    outStringIds[1] = std::get<1>(result);
    outStringIds[2] = std::get<2>(result);
    return outStringIds[0] != STR_NONE || outStringIds[1] != STR_NONE || outStringIds[2] != STR_NONE;
}

void language_free_object_string(rct_string_id stringId)
{
    auto& localisationService = OpenRCT2::GetContext()->GetLocalisationService();
    localisationService.FreeObjectString(stringId);
}

rct_string_id language_allocate_object_string(const std::string& target)
{
    auto& localisationService = OpenRCT2::GetContext()->GetLocalisationService();
    return localisationService.AllocateObjectString(target);
}
