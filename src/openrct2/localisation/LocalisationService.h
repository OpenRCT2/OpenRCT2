/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

#include <memory>
#include <stack>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

struct ILanguagePack;
struct IObjectManager;

namespace OpenRCT2
{
    struct IPlatformEnvironment;
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
        std::stack<StringId> _availableObjectStringIds;
        std::vector<std::string> _objectStrings;

    public:
        int32_t GetCurrentLanguage() const
        {
            return _currentLanguage;
        }
        bool UseTrueTypeFont() const
        {
            return _useTrueTypeFont;
        }
        void UseTrueTypeFont(bool value)
        {
            _useTrueTypeFont = value;
        }

        LocalisationService(const std::shared_ptr<IPlatformEnvironment>& env);
        ~LocalisationService();

        const char* GetString(StringId id) const;
        std::tuple<StringId, StringId, StringId> GetLocalisedScenarioStrings(const std::string& scenarioFilename) const;
        StringId GetObjectOverrideStringId(std::string_view legacyIdentifier, uint8_t index) const;
        std::string GetLanguagePath(uint32_t languageId) const;

        void OpenLanguage(int32_t id);
        void CloseLanguages();
        StringId AllocateObjectString(const std::string& target);
        void FreeObjectString(StringId stringId);
    };
} // namespace OpenRCT2::Localisation

// Legacy getters
// TODO Remove usages of these and instead call via shared reference
int32_t LocalisationService_GetCurrentLanguage();
bool LocalisationService_UseTrueTypeFont();
