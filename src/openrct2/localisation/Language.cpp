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
    { "",       "",                     "",                      FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_UNDEFINED
    { "ar-EG", "Arabic (experimental)", "Arabic (experimental)", FAMILY(&TTFFamilySansSerif),           true  }, // LANGUAGE_ARABIC
    { "ca-ES", "Catalan",               u8"Català",              FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_CATALAN
    { "zh-CN", "Chinese (Simplified)",  "Chinese (Simplified)",  FAMILY(&TTFFamilyChineseSimplified),   false }, // LANGUAGE_CHINESE_SIMPLIFIED
    { "zh-TW", "Chinese (Traditional)", "Chinese (Traditional)", FAMILY(&TTFFamilyChineseTraditional),  false }, // LANGUAGE_CHINESE_TRADITIONAL
    { "cs-CZ", "Czech",                 u8"Čeština",             FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_CZECH
    { "da-DK", "Danish",                "Dansk",                 FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_DANISH
    { "de-DE", "German",                "Deutsch",               FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_GERMAN
    { "en-GB", "English (UK)",          "English (UK)",          FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_ENGLISH_UK
    { "en-US", "English (US)",          "English (US)",          FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_ENGLISH_US
    { "es-ES", "Spanish",               u8"Español",             FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_SPANISH
    { "fr-FR", "French",                u8"Français",            FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_FRENCH
    { "it-IT", "Italian",               "Italiano",              FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_ITALIAN
    { "ja-JP", "Japanese",              "Japanese",              FAMILY(&TTFFamilyJapanese),            false }, // LANGUAGE_JAPANESE
    { "ko-KR", "Korean",                "Korean",                FAMILY(&TTFFamilyKorean),              false }, // LANGUAGE_KOREAN
    { "hu-HU", "Hungarian",             "Magyar",                FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_HUNGARIAN
    { "nl-NL", "Dutch",                 "Nederlands",            FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_DUTCH
    { "nb-NO", "Norwegian",             "Norsk",                 FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_NORWEGIAN
    { "pl-PL", "Polish",                "Polski",                FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_POLISH
    { "pt-BR", "Portuguese (BR)",       u8"Português (BR)",      FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_PORTUGUESE_BR
    { "ru-RU", "Russian",               u8"Русский",             FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_RUSSIAN
    { "fi-FI", "Finnish",               "Suomi",                 FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_FINNISH
    { "sv-SE", "Swedish",               "Svenska",               FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_SWEDISH
    { "tr-TR", "Turkish",               "Türkçe",                FAMILY_OPENRCT2_SPRITE,                false }, // LANGUAGE_TURKISH
};
// clang-format on

// clang-format off
const utf8 BlackUpArrowString[] =       { static_cast<utf8>(static_cast<uint8_t>(0xC2)), static_cast<utf8>(static_cast<uint8_t>(0x8E)), static_cast<utf8>(static_cast<uint8_t>(0xE2)), static_cast<utf8>(static_cast<uint8_t>(0x96)), static_cast<utf8>(static_cast<uint8_t>(0xB2)), static_cast<utf8>(static_cast<uint8_t>(0x00)) };
const utf8 BlackDownArrowString[] =     { static_cast<utf8>(static_cast<uint8_t>(0xC2)), static_cast<utf8>(static_cast<uint8_t>(0x8E)), static_cast<utf8>(static_cast<uint8_t>(0xE2)), static_cast<utf8>(static_cast<uint8_t>(0x96)), static_cast<utf8>(static_cast<uint8_t>(0xBC)), static_cast<utf8>(static_cast<uint8_t>(0x00)) };
const utf8 BlackLeftArrowString[] =     { static_cast<utf8>(static_cast<uint8_t>(0xC2)), static_cast<utf8>(static_cast<uint8_t>(0x8E)), static_cast<utf8>(static_cast<uint8_t>(0xE2)), static_cast<utf8>(static_cast<uint8_t>(0x97)), static_cast<utf8>(static_cast<uint8_t>(0x80)), static_cast<utf8>(static_cast<uint8_t>(0x00)) };
const utf8 BlackRightArrowString[] =    { static_cast<utf8>(static_cast<uint8_t>(0xC2)), static_cast<utf8>(static_cast<uint8_t>(0x8E)), static_cast<utf8>(static_cast<uint8_t>(0xE2)), static_cast<utf8>(static_cast<uint8_t>(0x96)), static_cast<utf8>(static_cast<uint8_t>(0xB6)), static_cast<utf8>(static_cast<uint8_t>(0x00)) };
const utf8 CheckBoxMarkString[] =       { static_cast<utf8>(static_cast<uint8_t>(0xE2)), static_cast<utf8>(static_cast<uint8_t>(0x9C)), static_cast<utf8>(static_cast<uint8_t>(0x93)), static_cast<utf8>(static_cast<uint8_t>(0x00)) };
// clang-format on

void utf8_remove_format_codes(utf8* text, bool allowcolours)
{
    const utf8* ch = text;
    utf8* dstCh = text;
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
        localisationService.OpenLanguage(id, objectManager);
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

std::string language_convert_string_to_tokens(const std::string_view& s)
{
    std::string result;
    result.reserve(s.size() * 4);
    std::string input = std::string(s);
    auto readPtr = input.c_str();
    while (true)
    {
        char32_t code = utf8_get_next(readPtr, const_cast<const utf8**>(&readPtr));
        if (code == 0)
        {
            break;
        }
        else if (code == '\n')
        {
            result.push_back('\n');
        }
        else if (utf8_is_format_code(code))
        {
            auto token = format_get_token(code);
            result.push_back('{');
            result.append(token);
            result.push_back('}');
        }
        else
        {
            char buffer[8]{};
            utf8_write_codepoint(buffer, code);
            result.append(buffer);
        }
    }
    result.shrink_to_fit();
    return result;
}

std::string language_convert_string(const std::string_view& s)
{
    enum class PARSE_STATE
    {
        DEFAULT,
        CR,
        TOKEN,
    };

    std::string result;
    std::string token;
    PARSE_STATE state{};
    token.reserve(64);
    result.reserve(s.size() * 2);
    for (char c : s)
    {
        switch (state)
        {
            case PARSE_STATE::CR:
                result.push_back(FORMAT_NEWLINE);
                state = PARSE_STATE::DEFAULT;
                [[fallthrough]];
            case PARSE_STATE::DEFAULT:
                switch (c)
                {
                    case '\r':
                        state = PARSE_STATE::CR;
                        break;
                    case '\n':
                        result.push_back(FORMAT_NEWLINE);
                        break;
                    case '{':
                        token.clear();
                        state = PARSE_STATE::TOKEN;
                        break;
                    default:
                        if (static_cast<uint8_t>(c) >= 32)
                        {
                            result.push_back(c);
                        }
                        break;
                }
                break;
            case PARSE_STATE::TOKEN:
                if (c == '}')
                {
                    auto code = format_get_code(token.c_str());
                    if (code == 0)
                    {
                        int32_t number{};
                        if (sscanf(token.c_str(), "%d", &number) == 1)
                        {
                            auto b = static_cast<uint8_t>(std::clamp(number, 0, 255));
                            token.push_back(b);
                        }
                    }
                    else
                    {
                        char buffer[8]{};
                        utf8_write_codepoint(buffer, code);
                        result.append(buffer);
                    }
                    state = PARSE_STATE::DEFAULT;
                }
                else
                {
                    token.push_back(c);
                }
                break;
        }
    }
    result.shrink_to_fit();
    return result;
}
