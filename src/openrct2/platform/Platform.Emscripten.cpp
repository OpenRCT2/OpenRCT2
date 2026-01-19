/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef __EMSCRIPTEN__

    #include "Platform.h"

    #include "../Diagnostic.h"
    #include "../GameState.h"
    #include "../core/Compression.h"
    #include "../core/File.h"
    #include "../core/Guard.hpp"
    #include "../core/MemoryStream.h"
    #include "../localisation/Language.h"
    #include "../park/ParkFile.h"

    #include <emscripten.h>

extern "C" {
extern char* GetLocaleCurrencyCode();
}

namespace OpenRCT2::Platform
{
    std::string GetFolderPath(SpecialFolder folder)
    {
        switch (folder)
        {
            case SpecialFolder::userCache:
            case SpecialFolder::userConfig:
            case SpecialFolder::userData:
            case SpecialFolder::userHome:
                return "/persistent";
            default:
                return std::string();
        }
    }

    std::string GetDocsPath()
    {
        return std::string();
    }

    std::string GetInstallPath()
    {
        return "/OpenRCT2";
    }

    std::string GetCurrentExecutablePath()
    {
        return std::string();
    }

    u8string StrDecompToPrecomp(u8string_view input)
    {
        return u8string(input);
    }

    bool HandleSpecialCommandLineArgument(const char* argument)
    {
        return false;
    }

    uint16_t GetLocaleLanguage()
    {
        auto locale = reinterpret_cast<char*>(EM_ASM_PTR({
            const locale = Intl.DateTimeFormat().resolvedOptions().locale;
            return stringToNewUTF8(locale);
        }));
        auto languageId = LanguageGetIDFromLocale(locale);
        free(locale);
        return languageId;
    }

    CurrencyType GetLocaleCurrency()
    {
        auto localeCurrencyCode = GetLocaleCurrencyCode();
        auto currency = Platform::GetCurrencyValue(localeCurrencyCode);
        free(localeCurrencyCode);
        return currency;
    }

    MeasurementFormat GetLocaleMeasurementFormat()
    {
        int isImperial = EM_ASM_INT({
            // There is no way to get this directly from javascript! Assume based off of user country
            const country = Intl.DateTimeFormat().resolvedOptions().locale.split("-").pop();
            return ([ "LR", "MM", "US" ]).includes(country) ? 1 : 0;
        });

        return isImperial == 1 ? MeasurementFormat::Imperial : MeasurementFormat::Metric;
    }

    SteamPaths GetSteamPaths()
    {
        return {};
    }

    #ifndef DISABLE_TTF
    std::string GetFontPath(const TTFFontDescriptor& font)
    {
        return {};
    }
    #endif

    std::vector<std::string_view> GetSearchablePathsRCT1()
    {
        return { "/RCT" };
    }

    std::vector<std::string_view> GetSearchablePathsRCT2()
    {
        return { "/RCT" };
    }
} // namespace OpenRCT2::Platform

#endif
