/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../drawing/Font.h"
#include "../interface/FontFamilies.h"

#include <string>
#include <string_view>

enum
{
    LANGUAGE_UNDEFINED,
    LANGUAGE_ARABIC,
    LANGUAGE_CATALAN,
    LANGUAGE_CHINESE_SIMPLIFIED,
    LANGUAGE_CHINESE_TRADITIONAL,
    LANGUAGE_CZECH,
    LANGUAGE_DANISH,
    LANGUAGE_GERMAN,
    LANGUAGE_ENGLISH_UK,
    LANGUAGE_ENGLISH_US,
    LANGUAGE_ESPERANTO,
    LANGUAGE_SPANISH,
    LANGUAGE_FRENCH,
    LANGUAGE_FRENCH_CA,
    LANGUAGE_ITALIAN,
    LANGUAGE_JAPANESE,
    LANGUAGE_KOREAN,
    LANGUAGE_HUNGARIAN,
    LANGUAGE_DUTCH,
    LANGUAGE_NORWEGIAN,
    LANGUAGE_POLISH,
    LANGUAGE_PORTUGUESE_BR,
    LANGUAGE_RUSSIAN,
    LANGUAGE_FINNISH,
    LANGUAGE_SWEDISH,
    LANGUAGE_TURKISH,
    LANGUAGE_UKRAINIAN,
    LANGUAGE_VIETNAMESE,
    LANGUAGE_COUNT
};

enum class RCT2LanguageId
{
    EnglishUK,
    EnglishUS,
    French,
    German,
    Spanish,
    Italian,
    Dutch,
    Swedish,
    Japanese,
    Korean,
    ChineseSimplified,
    ChineseTraditional,
    Undefined,
    Portuguese,
    Blank = 254,
    End = 255
};

struct LanguageDescriptor
{
    const char* locale;
    const utf8* english_name;
    const utf8* native_name;
    uint8_t fallback;
#if !defined(NO_TTF)
    TTFontFamily const* font_family;
#else
    void* font_family;
#endif
    bool isRtl;
};

extern const LanguageDescriptor LanguagesDescriptors[LANGUAGE_COUNT];

constexpr const char* BlackUpArrowString = u8"{BLACK}▲";
constexpr const char* BlackDownArrowString = u8"{BLACK}▼";
constexpr const char* BlackLeftArrowString = u8"{BLACK}◀";
constexpr const char* BlackRightArrowString = u8"{BLACK}▶";
constexpr const char* CheckBoxMarkString = u8"✓";
constexpr const char* EyeString = u8"👁";

uint8_t LanguageGetIDFromLocale(const char* locale);
const char* LanguageGetString(StringId id);
bool LanguageOpen(int32_t id);

std::string RCT2StringToUTF8(std::string_view src, RCT2LanguageId languageId);
bool LanguageGetLocalisedScenarioStrings(const utf8* scenarioFilename, StringId* outStringIds);
void LanguageFreeObjectString(StringId stringId);
StringId LanguageAllocateObjectString(const std::string& target);
