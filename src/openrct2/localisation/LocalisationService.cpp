/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
    for (StringId stringId = BASE_OBJECT_STRING_ID + MAX_OBJECT_CACHED_STRINGS; stringId >= BASE_OBJECT_STRING_ID; stringId--)
    {
        _availableObjectStringIds.push(stringId);
    }
}

// Define implementation here to avoid including LanguagePack.h in header
LocalisationService::~LocalisationService() = default;

const char* LocalisationService::GetString(StringId id) const
{
    if (id == STR_EMPTY)
    {
        return "";
    }
    else if (id >= BASE_OBJECT_STRING_ID && id < BASE_OBJECT_STRING_ID + MAX_OBJECT_CACHED_STRINGS)
    {
        size_t index = id - BASE_OBJECT_STRING_ID;
        if (index < _objectStrings.size())
        {
            return _objectStrings[index].c_str();
        }

        return "(unallocated string)";
    }
    else if (id != STR_NONE)
    {
        for (const auto& language : _loadedLanguages)
        {
            const auto result = language->GetString(id);
            if (result != nullptr)
                return result;
        }

        return "(undefined string)";
    }

    return nullptr;
}

std::string LocalisationService::GetLanguagePath(uint32_t languageId) const
{
    auto locale = std::string(LanguagesDescriptors[languageId].locale);
    auto languageDirectory = _env->GetDirectoryPath(DIRBASE::OPENRCT2, DIRID::LANGUAGE);
    auto languagePath = Path::Combine(languageDirectory, locale + u8".txt");
    return languagePath;
}

std::string_view LocalisationService::GetCurrentLanguageLocale() const
{
    if (_currentLanguage >= 0 && static_cast<size_t>(_currentLanguage) < std::size(LanguagesDescriptors))
    {
        return LanguagesDescriptors[_currentLanguage].locale;
    }
    return {};
}

void LocalisationService::OpenLanguage(int32_t id)
{
    CloseLanguages();
    if (id == LANGUAGE_UNDEFINED)
    {
        throw std::invalid_argument("id was undefined");
    }

    std::string filename;
    filename = GetLanguagePath(id);
    auto preferredLanguage = LanguagePackFactory::FromFile(id, filename.c_str());
    if (preferredLanguage != nullptr)
    {
        _currentLanguage = id;
        _languageOrder.emplace_back(id);
        _loadedLanguages.emplace_back(std::move(preferredLanguage));
        TryLoadFonts(*this);
    }
    else
    {
        throw std::runtime_error("Unable to open language " + std::to_string(id));
    }

    auto checkLanguage = LanguagesDescriptors[id].fallback;
    while (true)
    {
        if (checkLanguage == LANGUAGE_UNDEFINED)
            break;

        _languageOrder.emplace_back(checkLanguage);
        filename = GetLanguagePath(checkLanguage);
        _loadedLanguages.emplace_back(LanguagePackFactory::FromFile(checkLanguage, filename.c_str()));
        checkLanguage = LanguagesDescriptors[checkLanguage].fallback;
    }

    if (id != LANGUAGE_ENGLISH_UK)
    {
        _languageOrder.emplace_back(LANGUAGE_ENGLISH_UK);
        filename = GetLanguagePath(LANGUAGE_ENGLISH_UK);
        _loadedLanguages.emplace_back(LanguagePackFactory::FromFile(LANGUAGE_ENGLISH_UK, filename.c_str()));
    }
}

void LocalisationService::CloseLanguages()
{
    for (auto& language : _loadedLanguages)
    {
        language = nullptr;
    }
    _languageOrder.clear();
    _loadedLanguages.clear();
    _currentLanguage = LANGUAGE_UNDEFINED;
}

std::tuple<StringId, StringId, StringId> LocalisationService::GetLocalisedScenarioStrings(
    const std::string& scenarioFilename) const
{
    auto result0 = _loadedLanguages[0]->GetScenarioOverrideStringId(scenarioFilename.c_str(), 0);
    auto result1 = _loadedLanguages[0]->GetScenarioOverrideStringId(scenarioFilename.c_str(), 1);
    auto result2 = _loadedLanguages[0]->GetScenarioOverrideStringId(scenarioFilename.c_str(), 2);
    return std::make_tuple(result0, result1, result2);
}

StringId LocalisationService::GetObjectOverrideStringId(std::string_view legacyIdentifier, uint8_t index) const
{
    if (_loadedLanguages.empty())
    {
        return STR_NONE;
    }
    return _loadedLanguages[0]->GetObjectOverrideStringId(legacyIdentifier, index);
}

StringId LocalisationService::AllocateObjectString(const std::string& target)
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

void LocalisationService::FreeObjectString(StringId stringId)
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

const std::vector<int32_t>& LocalisationService::GetLanguageOrder() const
{
    return _languageOrder;
}

int32_t LocalisationService_GetCurrentLanguage()
{
    const auto& localisationService = GetContext()->GetLocalisationService();
    return localisationService.GetCurrentLanguage();
}

bool LocalisationService_UseTrueTypeFont()
{
    const auto& localisationService = GetContext()->GetLocalisationService();
    return localisationService.UseTrueTypeFont();
}
