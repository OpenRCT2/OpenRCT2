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
        sint32 _currentLanguage{};
        bool _useTrueTypeFont{};
        std::unique_ptr<ILanguagePack> _languageFallback;
        std::unique_ptr<ILanguagePack> _languageCurrent;
        std::stack<rct_string_id> _availableObjectStringIds;

    public:
        sint32 GetCurrentLanguage() const { return _currentLanguage; }
        bool UseTrueTypeFont() const { return _useTrueTypeFont; }
        void UseTrueTypeFont(bool value) { _useTrueTypeFont = value; }

        LocalisationService(const std::shared_ptr<IPlatformEnvironment>& env);
        ~LocalisationService();

        const char * GetString(rct_string_id id) const;
        std::tuple<rct_string_id, rct_string_id, rct_string_id> GetLocalisedScenarioStrings(const std::string& scenarioFilename) const;
        rct_string_id GetObjectOverrideStringId(const char * identifier, uint8 index) const;
        std::string GetLanguagePath(uint32 languageId) const;

        void OpenLanguage(sint32 id, IObjectManager& objectManager);
        void CloseLanguages();
        rct_string_id AllocateObjectString(const std::string& target);
        void FreeObjectString(rct_string_id stringId);
    };
}

// Legacy getters
// TODO Remove usages of these and instead call via shared reference
sint32 LocalisationService_GetCurrentLanguage();
bool LocalisationService_UseTrueTypeFont();
