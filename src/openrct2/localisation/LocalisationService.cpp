/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "LocalisationService.h"

#include "../Context.h"
#include "../PlatformEnvironment.h"
#include "../core/Path.hpp"
#include "../interface/Fonts.h"
#include "../object/ObjectManager.h"
#include "Language.h"
#include "LanguagePack.h"
#include "StringIds.h"

#include <stdexcept>

using namespace OpenRCT2;
using namespace OpenRCT2::Localisation;

static constexpr uint16_t BASE_OBJECT_STRING_ID = 0x2000;
static constexpr uint16_t MAX_OBJECT_CACHED_STRINGS = 0x5000 - BASE_OBJECT_STRING_ID;

LocalisationService::LocalisationService(const std::shared_ptr<IPlatformEnvironment>& env)
    : _env(env)
{
    for (rct_string_id stringId = BASE_OBJECT_STRING_ID + MAX_OBJECT_CACHED_STRINGS; stringId >= BASE_OBJECT_STRING_ID;
         stringId--)
    {
        _availableObjectStringIds.push(stringId);
    }
}

// Define implementation here to avoid including LanguagePack.h in header
LocalisationService::~LocalisationService() = default;

const char* LocalisationService::GetString(rct_string_id id) const
{
    const char* result = nullptr;
    if (id == STR_EMPTY)
    {
        result = "";
    }
    else if (id >= BASE_OBJECT_STRING_ID && id < BASE_OBJECT_STRING_ID + MAX_OBJECT_CACHED_STRINGS)
    {
        size_t index = id - BASE_OBJECT_STRING_ID;
        if (index < _objectStrings.size())
        {
            return _objectStrings[index].c_str();
        }

        result = "(unallocated string)";
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

std::string LocalisationService::GetLanguagePath(uint32_t languageId) const
{
    auto locale = std::string(LanguagesDescriptors[languageId].locale);
    auto languageDirectory = _env->GetDirectoryPath(DIRBASE::OPENRCT2, DIRID::LANGUAGE);
    auto languagePath = Path::Combine(languageDirectory, locale + u8".txt");
    return languagePath;
}

void LocalisationService::OpenLanguage(OpenRCT2::IContext* context, int32_t id)
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
        _languageFallback = LanguagePackFactory::FromFile(LANGUAGE_ENGLISH_UK, filename.c_str());
    }

    filename = GetLanguagePath(id);
    _languageCurrent = LanguagePackFactory::FromFile(id, filename.c_str());
    if (_languageCurrent != nullptr)
    {
        _currentLanguage = id;
        TryLoadFonts(context, *this);
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

std::tuple<rct_string_id, rct_string_id, rct_string_id> LocalisationService::GetLocalisedScenarioStrings(
    const std::string& scenarioFilename) const
{
    auto result0 = _languageCurrent->GetScenarioOverrideStringId(scenarioFilename.c_str(), 0);
    auto result1 = _languageCurrent->GetScenarioOverrideStringId(scenarioFilename.c_str(), 1);
    auto result2 = _languageCurrent->GetScenarioOverrideStringId(scenarioFilename.c_str(), 2);
    return std::make_tuple(result0, result1, result2);
}

rct_string_id LocalisationService::GetObjectOverrideStringId(std::string_view legacyIdentifier, uint8_t index) const
{
    if (_languageCurrent == nullptr)
    {
        return STR_NONE;
    }
    return _languageCurrent->GetObjectOverrideStringId(legacyIdentifier, index);
}

rct_string_id LocalisationService::AllocateObjectString(const std::string& target)
{
    if (_availableObjectStringIds.empty())
    {
        return STR_EMPTY;
    }

    auto stringId = _availableObjectStringIds.top();
    _availableObjectStringIds.pop();

    size_t index = stringId - BASE_OBJECT_STRING_ID;
    if (index >= _objectStrings.size())
    {
        _objectStrings.resize(index + 1);
    }
    _objectStrings[index] = target;

    return stringId;
}

void LocalisationService::FreeObjectString(rct_string_id stringId)
{
    if (stringId != STR_EMPTY)
    {
        size_t index = stringId - BASE_OBJECT_STRING_ID;
        if (index < _objectStrings.size())
        {
            _objectStrings[index] = {};
        }
        _availableObjectStringIds.push(stringId);
    }
}

int32_t LocalisationService_GetCurrentLanguage(OpenRCT2::IContext* context)
{
    const auto* localisationService = context->GetLocalisationService();
    return localisationService->GetCurrentLanguage();
}

bool LocalisationService_UseTrueTypeFont(OpenRCT2::IContext* context)
{
    const auto* localisationService = context->GetLocalisationService();
    return localisationService->UseTrueTypeFont();
}
