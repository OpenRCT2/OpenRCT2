#pragma region Copyright (c) 2018 OpenRCT2 Developers
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

#include <stdexcept>
#include "../Context.h"
#include "../core/Path.hpp"
#include "../interface/Fonts.h"
#include "../object/ObjectManager.h"
#include "../PlatformEnvironment.h"
#include "Language.h"
#include "LanguagePack.h"
#include "LocalisationService.h"
#include "StringIds.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Localisation;

static constexpr rct_string_id NONSTEX_BASE_STRING_ID = 3463;
static constexpr uint16 MAX_OBJECT_CACHED_STRINGS = 2048;

LocalisationService::LocalisationService(const std::shared_ptr<IPlatformEnvironment>& env)
    : _env(env)
{
    for (rct_string_id stringId = NONSTEX_BASE_STRING_ID + MAX_OBJECT_CACHED_STRINGS; stringId >= NONSTEX_BASE_STRING_ID; stringId--)
    {
        _availableObjectStringIds.push(stringId);
    }
}

// Define implementation here to avoid including LanguagePack.h in header
LocalisationService::~LocalisationService()
{
}

const char * LocalisationService::GetString(rct_string_id id) const
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

std::string LocalisationService::GetLanguagePath(uint32 languageId) const
{
    auto locale = std::string(LanguagesDescriptors[languageId].locale);
    auto languageDirectory = _env->GetDirectoryPath(DIRBASE::OPENRCT2, DIRID::LANGUAGE);
    auto languagePath = Path::Combine(languageDirectory, locale + ".txt");
    return languagePath;
}

void LocalisationService::OpenLanguage(sint32 id, IObjectManager& objectManager)
{
    CloseLanguages();
    if (id == LANGUAGE_UNDEFINED)
    {
        throw std::invalid_argument("id was undefined");
    }

    std::string filename;
    if (id != LANGUAGE_ENGLISH_UK)
    {
        filename = GetLanguagePath(LANGUAGE_ENGLISH_UK);
        _languageFallback = std::unique_ptr<ILanguagePack>(LanguagePackFactory::FromFile(LANGUAGE_ENGLISH_UK, filename.c_str()));
    }

    filename = GetLanguagePath(id);
    _languageCurrent = std::unique_ptr<ILanguagePack>(LanguagePackFactory::FromFile(id, filename.c_str()));
    if (_languageCurrent != nullptr)
    {
        _currentLanguage = id;
        TryLoadFonts(*this);

        // Objects and their localised strings need to be refreshed
        objectManager.ResetObjects();
    }
    else
    {
        throw std::runtime_error("Unable to open language " + std::to_string(id));
    }
}

void LocalisationService::CloseLanguages()
{
    _languageFallback = nullptr;
    _languageCurrent = nullptr;
    _currentLanguage = LANGUAGE_UNDEFINED;
}

std::tuple<rct_string_id, rct_string_id, rct_string_id> LocalisationService::GetLocalisedScenarioStrings(const std::string& scenarioFilename) const
{
    auto result0 = _languageCurrent->GetScenarioOverrideStringId(scenarioFilename.c_str(), 0);
    auto result1 = _languageCurrent->GetScenarioOverrideStringId(scenarioFilename.c_str(), 1);
    auto result2 = _languageCurrent->GetScenarioOverrideStringId(scenarioFilename.c_str(), 2);
    return std::make_tuple(result0, result1, result2);
}

rct_string_id LocalisationService::GetObjectOverrideStringId(const char * identifier, uint8 index) const
{
    if (_languageCurrent == nullptr)
    {
        return STR_NONE;
    }
    return _languageCurrent->GetObjectOverrideStringId(identifier, index);
}

rct_string_id LocalisationService::AllocateObjectString(const std::string& target)
{
    auto stringId = _availableObjectStringIds.top();
    _availableObjectStringIds.pop();
    _languageCurrent->SetString(stringId, target);
    return stringId;
}

void LocalisationService::FreeObjectString(rct_string_id stringId)
{
    if (stringId != STR_EMPTY)
    {
        if (_languageCurrent != nullptr)
        {
            _languageCurrent->RemoveString(stringId);
        }
        _availableObjectStringIds.push(stringId);
    }
}

sint32 LocalisationService_GetCurrentLanguage()
{
    const auto& localisationService = GetContext()->GetLocalisationService();
    return localisationService.GetCurrentLanguage();
}

bool LocalisationService_UseTrueTypeFont()
{
    const auto& localisationService = GetContext()->GetLocalisationService();
    return localisationService.UseTrueTypeFont();
}
