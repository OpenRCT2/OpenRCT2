/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
    struct IContext;
} // namespace OpenRCT2

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

        const char* GetString(rct_string_id id) const;
        std::tuple<rct_string_id, rct_string_id, rct_string_id> GetLocalisedScenarioStrings(
            const std::string& scenarioFilename) const;
        rct_string_id GetObjectOverrideStringId(std::string_view legacyIdentifier, uint8_t index) const;
        std::string GetLanguagePath(uint32_t languageId) const;

        void OpenLanguage(OpenRCT2::IContext* context, int32_t id);
        void CloseLanguages();
        rct_string_id AllocateObjectString(const std::string& target);
        void FreeObjectString(rct_string_id stringId);
    };
} // namespace OpenRCT2::Localisation

// Legacy getters
// TODO Remove usages of these and instead call via shared reference
int32_t LocalisationService_GetCurrentLanguage(OpenRCT2::IContext* context);
bool LocalisationService_UseTrueTypeFont(OpenRCT2::IContext* context);
