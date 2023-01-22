/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
#include "../platform/Platform.h"
#include "LanguagePack.h"
#include "Localisation.h"
#include "LocalisationService.h"

#include <stack>
namespace OpenRCT2
{
    // clang-format off
const LanguageDescriptor LanguagesDescriptors[LANGUAGE_COUNT] =
{
    { "",       "",                     "",                      LANGUAGE_UNDEFINED, FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_UNDEFINED
    { "ar-EG", "Arabic (experimental)", "Arabic (experimental)", LANGUAGE_UNDEFINED, FAMILY(&TTFFamilySansSerif),           true  }, // LANGUAGE_ARABIC
    { "ca-ES", "Catalan",               u8"Català",              LANGUAGE_UNDEFINED, FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_CATALAN
    { "zh-CN", "Chinese (Simplified)",  "Chinese (Simplified)",  LANGUAGE_UNDEFINED, FAMILY(&TTFFamilyChineseSimplified),   false }, // LANGUAGE_CHINESE_SIMPLIFIED
    { "zh-TW", "Chinese (Traditional)", "Chinese (Traditional)", LANGUAGE_UNDEFINED, FAMILY(&TTFFamilyChineseTraditional),  false }, // LANGUAGE_CHINESE_TRADITIONAL
    { "cs-CZ", "Czech",                 u8"Čeština",             LANGUAGE_UNDEFINED, FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_CZECH
    { "da-DK", "Danish",                "Dansk",                 LANGUAGE_UNDEFINED, FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_DANISH
    { "de-DE", "German",                "Deutsch",               LANGUAGE_UNDEFINED, FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_GERMAN
    { "en-GB", "English (UK)",          "English (UK)",          LANGUAGE_UNDEFINED, FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_ENGLISH_UK
    { "en-US", "English (US)",          "English (US)",          LANGUAGE_UNDEFINED, FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_ENGLISH_US
    { "eo-ZZ", "Esperanto",             "Esperanto",             LANGUAGE_UNDEFINED, FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_ESPERANTO
    { "es-ES", "Spanish",               u8"Español",             LANGUAGE_UNDEFINED, FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_SPANISH
    { "fr-FR", "French",                u8"Français",            LANGUAGE_UNDEFINED, FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_FRENCH
    { "fr-CA", "French (CA)",           u8"Français (CA)",       LANGUAGE_FRENCH,    FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_FRENCH_CA
    { "it-IT", "Italian",               "Italiano",              LANGUAGE_UNDEFINED, FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_ITALIAN
    { "ja-JP", "Japanese",              "Japanese",              LANGUAGE_UNDEFINED, FAMILY(&TTFFamilyJapanese),            false }, // LANGUAGE_JAPANESE
    { "ko-KR", "Korean",                "Korean",                LANGUAGE_UNDEFINED, FAMILY(&TTFFamilyKorean),              false }, // LANGUAGE_KOREAN
    { "hu-HU", "Hungarian",             "Magyar",                LANGUAGE_UNDEFINED, FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_HUNGARIAN
    { "nl-NL", "Dutch",                 "Nederlands",            LANGUAGE_UNDEFINED, FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_DUTCH
    { "nb-NO", "Norwegian",             "Norsk",                 LANGUAGE_UNDEFINED, FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_NORWEGIAN
    { "pl-PL", "Polish",                "Polski",                LANGUAGE_UNDEFINED, FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_POLISH
    { "pt-BR", "Portuguese (BR)",       u8"Português (BR)",      LANGUAGE_UNDEFINED, FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_PORTUGUESE_BR
    { "ru-RU", "Russian",               u8"Русский",             LANGUAGE_UNDEFINED, FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_RUSSIAN
    { "fi-FI", "Finnish",               "Suomi",                 LANGUAGE_UNDEFINED, FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_FINNISH
    { "sv-SE", "Swedish",               "Svenska",               LANGUAGE_UNDEFINED, FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_SWEDISH
    { "tr-TR", "Turkish",               "Türkçe",                LANGUAGE_UNDEFINED, FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_TURKISH
    { "vi-VN", "Vietnamese",            "Vietnamese",            LANGUAGE_UNDEFINED, FAMILY(&TTFFamilySansSerif),           false }, // LANGUAGE_VIETNAMESE
};
    // clang-format on

    uint8_t LanguageGetIDFromLocale(const char* locale)
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

    const char* LanguageGetString(StringId id)
    {
        const auto& localisationService = OpenRCT2::GetContext()->GetLocalisationService();
        return localisationService.GetString(id);
    }

    bool LanguageOpen(int32_t id)
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

    bool LanguageGetLocalisedScenarioStrings(const utf8* scenarioFilename, StringId* outStringIds)
    {
        const auto& localisationService = OpenRCT2::GetContext()->GetLocalisationService();
        auto result = localisationService.GetLocalisedScenarioStrings(scenarioFilename);
        outStringIds[0] = std::get<0>(result);
        outStringIds[1] = std::get<1>(result);
        outStringIds[2] = std::get<2>(result);
        return outStringIds[0] != STR_NONE || outStringIds[1] != STR_NONE || outStringIds[2] != STR_NONE;
    }

    void LanguageFreeObjectString(StringId stringId)
    {
        auto& localisationService = OpenRCT2::GetContext()->GetLocalisationService();
        localisationService.FreeObjectString(stringId);
    }

    StringId LanguageAllocateObjectString(const std::string& target)
    {
        auto& localisationService = OpenRCT2::GetContext()->GetLocalisationService();
        return localisationService.AllocateObjectString(target);
    }
} // namespace OpenRCT2
