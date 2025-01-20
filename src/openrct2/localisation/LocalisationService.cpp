/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "LocalisationService.h"

#include "../Context.h"
#include "../PlatformEnvironment.h"
#include "../core/Guard.hpp"
#include "../core/Path.hpp"
#include "../interface/Fonts.h"
#include "../object/ObjectManager.h"
#include "Language.h"
#include "LanguagePack.h"
#include "StringIds.h"

#include <stdexcept>

using namespace OpenRCT2;
using namespace OpenRCT2::Localisation;

static constexpr uint16_t kBaseObjectStringID = 0x2000;
static constexpr uint16_t kMaxObjectCachedStrings = 0x5000 - kBaseObjectStringID;

LocalisationService::LocalisationService(const std::shared_ptr<IPlatformEnvironment>& env)
    : _env(env)
{
    for (StringId stringId = kBaseObjectStringID + kMaxObjectCachedStrings; stringId >= kBaseObjectStringID; stringId--)
    {
        _availableObjectStringIds.push(stringId);
    }
}

// Define implementation here to avoid including LanguagePack.h in header
LocalisationService::~LocalisationService() = default;

const char* LocalisationService::GetString(StringId id) const
{
    if (id == kStringIdEmpty)
    {
        return "";
    }
    else if (id >= kBaseObjectStringID && id < kBaseObjectStringID + kMaxObjectCachedStrings)
    {
        size_t index = id - kBaseObjectStringID;
        if (index < _objectStrings.size())
        {
            return _objectStrings[index].c_str();
        }

        return "(unallocated string)";
    }
    else if (id != kStringIdNone)
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

    auto preferredLanguage = LanguagePackFactory::FromLanguageId(id);
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
    while (checkLanguage != LANGUAGE_UNDEFINED)
    {
        _languageOrder.emplace_back(checkLanguage);
        auto fallbackLanguagePack = LanguagePackFactory::FromLanguageId(checkLanguage);
        if (fallbackLanguagePack != nullptr)
        {
            _loadedLanguages.emplace_back(std::move(fallbackLanguagePack));
        }

        checkLanguage = LanguagesDescriptors[checkLanguage].fallback;
    }

    if (id != LANGUAGE_ENGLISH_UK)
    {
        _languageOrder.emplace_back(LANGUAGE_ENGLISH_UK);
        auto englishLanguagePack = LanguagePackFactory::FromLanguageId(LANGUAGE_ENGLISH_UK);
        if (englishLanguagePack != nullptr)
        {
            _loadedLanguages.emplace_back(std::move(englishLanguagePack));
        }
        else
        {
            throw std::runtime_error("Unable to open the English language file!");
        }
    }
}

void LocalisationService::CloseLanguages()
{
    _languageOrder.clear();
    _loadedLanguages.clear();
    _currentLanguage = LANGUAGE_UNDEFINED;
}

StringId LocalisationService::AllocateObjectString(const std::string& target)
{
    if (_availableObjectStringIds.empty())
    {
        return kStringIdEmpty;
    }

    auto stringId = _availableObjectStringIds.top();
    _availableObjectStringIds.pop();

    size_t index = stringId - kBaseObjectStringID;
    if (index >= _objectStrings.size())
    {
        _objectStrings.resize(index + 1);
    }
    _objectStrings[index] = target;

    return stringId;
}

void LocalisationService::FreeObjectString(StringId stringId)
{
    if (stringId != kStringIdEmpty)
    {
        size_t index = stringId - kBaseObjectStringID;
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
