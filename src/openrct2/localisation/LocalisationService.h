/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <memory>
#include <stack>
#include <string>
#include <tuple>
#include "../common.h"

interface ILanguagePack;
interface IObjectManager;

namespace OpenRCT2
{
    interface IPlatformEnvironment;
}

namespace OpenRCT2::Localisation
{
    class LocalisationService
    {
    private:
        const std::shared_ptr<IPlatformEnvironment> _env;
        int32_t _currentLanguage{};
        bool _useTrueTypeFont{};
        std::unique_ptr<ILanguagePack> _languageFallback;
        std::unique_ptr<ILanguagePack> _languageCurrent;
        std::stack<rct_string_id> _availableObjectStringIds;

    public:
        int32_t GetCurrentLanguage() const { return _currentLanguage; }
        bool UseTrueTypeFont() const { return _useTrueTypeFont; }
        void UseTrueTypeFont(bool value) { _useTrueTypeFont = value; }

        LocalisationService(const std::shared_ptr<IPlatformEnvironment>& env);
        ~LocalisationService();

        const char * GetString(rct_string_id id) const;
        std::tuple<rct_string_id, rct_string_id, rct_string_id> GetLocalisedScenarioStrings(const std::string& scenarioFilename) const;
        rct_string_id GetObjectOverrideStringId(const char * identifier, uint8_t index) const;
        std::string GetLanguagePath(uint32_t languageId) const;

        void OpenLanguage(int32_t id, IObjectManager& objectManager);
        void CloseLanguages();
        rct_string_id AllocateObjectString(const std::string& target);
        void FreeObjectString(rct_string_id stringId);
    };
}

// Legacy getters
// TODO Remove usages of these and instead call via shared reference
int32_t LocalisationService_GetCurrentLanguage();
bool LocalisationService_UseTrueTypeFont();
