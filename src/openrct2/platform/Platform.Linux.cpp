/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#if (defined(__unix__) || defined(__HAIKU__)) && !defined(__ANDROID__) && !defined(__APPLE__) && !defined(__EMSCRIPTEN__)

    #include "../Diagnostic.h"

    #include <cstring>
    #include <fnmatch.h>
    #include <limits.h>
    #include <locale.h>
    #include <pwd.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <vector>
    #if defined(__FreeBSD__) || defined(__NetBSD__)
        #include <stddef.h>
        #include <sys/param.h>
        #include <sys/sysctl.h>
    #endif // __FreeBSD__ || __NetBSD__
    #if defined(__linux__)
        // for PATH_MAX
        #include <linux/limits.h>
    #endif // __linux__
    #ifndef DISABLE_TTF
        #include <fontconfig/fontconfig.h>
    #endif // DISABLE_TTF

    #include "../Date.h"
    #include "../OpenRCT2.h"
    #include "../core/Path.hpp"
    #include "../localisation/Language.h"
    #include "Platform.h"

    #ifdef __HAIKU__
        #include <image.h>
    #endif

namespace OpenRCT2::Platform
{
    // EnvLangGuard allows us to temporarily set the user's locale
    // to the generic C locale, in order to trick fontconfig into
    // returning an English font face name, while using RAII to avoid
    // changing locale settings in other parts of the program
    class EnvLangGuard
    {
    public:
        EnvLangGuard();
        ~EnvLangGuard();

    private:
        // GNU recommends scripts/programs set LC_ALL to override
        // locales for uniform testing, clearing it after should let
        // LANG and other locale settings operate normally
        static constexpr const char* _kOverrideVarName{ "LC_ALL" };
        static constexpr const char* _kTargetLocale{ "C.UTF-8" };
    };

    EnvLangGuard::EnvLangGuard()
    {
        int overwrite = 1;
        int result = setenv(_kOverrideVarName, _kTargetLocale, overwrite);
        if (result != 0)
        {
            LOG_VERBOSE("Could not update locale for font selection, some fonts may display incorrectly");
        }
    }

    EnvLangGuard::~EnvLangGuard()
    {
        int result = unsetenv(_kOverrideVarName);
        if (result != 0)
        {
            LOG_VERBOSE("Could not restore user locale");
        }
    }

    std::string GetFolderPath(SpecialFolder folder)
    {
        switch (folder)
        {
            case SpecialFolder::userCache:
            case SpecialFolder::userConfig:
            case SpecialFolder::userData:
            {
                auto path = GetEnvironmentPath("XDG_CONFIG_HOME");
                if (path.empty())
                {
                    auto home = GetFolderPath(SpecialFolder::userHome);
                    path = Path::Combine(home, u8".config");
                }
                return path;
            }
            case SpecialFolder::userHome:
                return GetHomePath();
            default:
                return std::string();
        }
    }

    std::string GetDocsPath()
    {
        std::string relative = Path::GetDirectory(Platform::GetCurrentExecutablePath());
        relative = Path::Combine(relative, u8"../share/doc/openrct2");
        const utf8* searchLocations[] = {
            // First try the directory relative to the executable
            relative.c_str(),
            "./doc",
            "/usr/share/doc/openrct2",
            DOCDIR,
        };
        for (auto searchLocation : searchLocations)
        {
            LOG_VERBOSE("Looking for OpenRCT2 doc path at %s", searchLocation);
            if (Path::DirectoryExists(searchLocation))
            {
                LOG_VERBOSE("Found OpenRCT2 doc path at %s", searchLocation);
                return searchLocation;
            }
        }
        return std::string();
    }

    static std::string GetCurrentWorkingDirectory()
    {
        char cwdPath[PATH_MAX];
        if (getcwd(cwdPath, sizeof(cwdPath)) != nullptr)
        {
            return cwdPath;
        }
        return std::string();
    }

    std::string GetInstallPath()
    {
        // 1. Try command line argument
        if (!gCustomOpenRCT2DataPath.empty())
        {
            return Path::GetAbsolute(gCustomOpenRCT2DataPath);
        }
        // 2. Try {${exeDir},${cwd},/}/{data,standard system app directories}
        // exeDir should come first to allow installing into build dir
        // clang-format off
        const std::string prefixes[]{
            Path::GetDirectory(Platform::GetCurrentExecutablePath()),
            GetCurrentWorkingDirectory(),
            "/"
        };
        static constexpr u8string_view SearchLocations[] = {
            "/data",
            "../share/openrct2",
            "/usr/local/share/openrct2",
            "/var/lib/openrct2",
            "/usr/share/openrct2",
    #ifdef __HAIKU__
            "/boot/system/data/openrct2",
            "/boot/home/config/data/openrct2",
            "/boot/system/non-packaged/data/openrct2",
            "/boot/home/config/non-packaged/data/openrct2",
    #endif
        };
        // clang-format on
        for (const auto& prefix : prefixes)
        {
            for (const auto searchLocation : SearchLocations)
            {
                auto prefixedPath = Path::Combine(prefix, searchLocation);
                LOG_VERBOSE("Looking for OpenRCT2 data in %s", prefixedPath.c_str());
                if (Path::DirectoryExists(prefixedPath))
                {
                    return prefixedPath;
                }
            }
        }
        return "/";
    }

    std::string GetCurrentExecutablePath()
    {
        char exePath[PATH_MAX] = { 0 };
    #ifdef __linux__
        auto bytesRead = readlink("/proc/self/exe", exePath, sizeof(exePath));
        if (bytesRead == -1)
        {
            LOG_FATAL("failed to read /proc/self/exe");
        }
    #elif defined(__HAIKU__)
        image_info info;
        int32 cookie = 0;

        while (get_next_image_info(B_CURRENT_TEAM, &cookie, &info) >= B_OK)
        {
            if (info.type == B_APP_IMAGE)
            {
                strlcpy(exePath, info.name, sizeof(exePath));
                break;
            }
        }
    #elif defined(__FreeBSD__) || defined(__NetBSD__)
        #if defined(__FreeBSD__)
        const int32_t mib[] = {
            CTL_KERN,
            KERN_PROC,
            KERN_PROC_PATHNAME,
            -1,
        };
        #else
        const int32_t mib[] = {
            CTL_KERN,
            KERN_PROC_ARGS,
            -1,
            KERN_PROC_PATHNAME,
        };
        #endif
        auto exeLen = sizeof(exePath);
        if (sysctl(mib, 4, exePath, &exeLen, nullptr, 0) == -1)
        {
            LOG_FATAL("failed to get process path");
        }
    #elif defined(__OpenBSD__)
        // There is no way to get the path name of a running executable.
        // If you are not using the port or package, you may have to change this line!
        strlcpy(exePath, "/usr/local/bin/", sizeof(exePath));
    #else
        #error "Platform does not support full path exe retrieval"
    #endif
        return exePath;
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
        const char* langString = setlocale(LC_MESSAGES, "");
        if (langString != nullptr)
        {
            // The locale has the following form:
            // language[_territory[.codeset]][@modifier]
            // (see https://www.gnu.org/software/libc/manual/html_node/Locale-Names.html)
            // longest on my system is 29 with codeset and modifier, so 32 for the pattern should be more than enough
            char pattern[32];
            // strip the codeset and modifier part
            int32_t length = strlen(langString);
            {
                for (int32_t i = 0; i < length; ++i)
                {
                    if (langString[i] == '.' || langString[i] == '@')
                    {
                        length = i;
                        break;
                    }
                }
            } // end strip
            std::memcpy(pattern, langString, length); // copy all until first '.' or '@'
            pattern[length] = '\0';
            // find _ if present
            const char* strip = strchr(pattern, '_');
            if (strip != nullptr)
            {
                // could also use '-', but '?' is more flexible. Maybe LanguagesDescriptors will change.
                // pattern is now "language?territory"
                pattern[strip - pattern] = '?';
            }

            // Iterate through all available languages
            for (int32_t i = 1; i < LANGUAGE_COUNT; ++i)
            {
                if (!fnmatch(pattern, LanguagesDescriptors[i].locale, 0))
                {
                    return i;
                }
            }

            // special case
            if (fnmatch(pattern, "en_CA", 0) == 0)
            {
                return LANGUAGE_ENGLISH_US;
            }

            // no exact match found trying only language part
            if (strip != nullptr)
            {
                pattern[strip - pattern] = '*';
                pattern[strip - pattern + 1] = '\0'; // pattern is now "language*"
                for (int32_t i = 1; i < LANGUAGE_COUNT; ++i)
                {
                    if (!fnmatch(pattern, LanguagesDescriptors[i].locale, 0))
                    {
                        return i;
                    }
                }
            }
        }
        return LANGUAGE_ENGLISH_UK;
    }

    CurrencyType GetLocaleCurrency()
    {
        char* langstring = setlocale(LC_MONETARY, "");

        if (langstring == nullptr)
        {
            return Platform::GetCurrencyValue(NULL);
        }

        struct lconv* lc = localeconv();

        return Platform::GetCurrencyValue(lc->int_curr_symbol);
    }

    MeasurementFormat GetLocaleMeasurementFormat()
    {
    // LC_MEASUREMENT is GNU specific.
    #ifdef LC_MEASUREMENT
        const char* langstring = setlocale(LC_MEASUREMENT, "");
    #else
        const char* langstring = setlocale(LC_ALL, "");
    #endif

        if (langstring != nullptr)
        {
            // using https://en.wikipedia.org/wiki/Metrication#Chronology_and_status_of_conversion_by_country as reference
            if (!fnmatch("*_US*", langstring, 0) || !fnmatch("*_MM*", langstring, 0) || !fnmatch("*_LR*", langstring, 0))
            {
                return MeasurementFormat::Imperial;
            }
        }
        return MeasurementFormat::Metric;
    }

    SteamPaths GetSteamPaths()
    {
        SteamPaths ret = {};
        ret.nativeFolder = "steamapps/common";
        ret.downloadDepotFolder = "ubuntu12_32/steamapps/content";
        ret.manifests = "steamapps";

        const char* steamRoot = getenv("STEAMROOT");
        if (steamRoot != nullptr)
        {
            ret.roots.emplace_back(steamRoot);
        }

        const char* localSharePath = getenv("XDG_DATA_HOME");
        if (localSharePath != nullptr)
        {
            auto xdgDataHomeSteamPath = Path::Combine(localSharePath, u8"Steam");
            if (Path::DirectoryExists(xdgDataHomeSteamPath))
            {
                ret.roots.emplace_back(xdgDataHomeSteamPath);
            }
        }

        const char* homeDir = getpwuid(getuid())->pw_dir;
        if (homeDir != nullptr)
        {
            auto localShareSteamPath = Path::Combine(homeDir, u8".local/share/Steam");
            if (Path::DirectoryExists(localShareSteamPath))
            {
                ret.roots.emplace_back(localShareSteamPath);
            }

            auto oldSteamPath = Path::Combine(homeDir, u8".steam/steam");
            if (Path::DirectoryExists(oldSteamPath))
            {
                ret.roots.emplace_back(oldSteamPath);
            }

            auto snapLocalShareSteamPath = Path::Combine(homeDir, u8"snap/steam/common/.local/share/Steam");
            if (Path::DirectoryExists(snapLocalShareSteamPath))
            {
                ret.roots.emplace_back(snapLocalShareSteamPath);
            }
        }

        return ret;
    }

    std::vector<std::string_view> GetSearchablePathsRCT1()
    {
        return {
            // game-data-packager uses this path when installing game files
            "/usr/share/games/roller-coaster-tycoon",
        };
    }

    std::vector<std::string_view> GetSearchablePathsRCT2()
    {
        return {
            // game-data-packager uses this path when installing game files
            "/usr/share/games/roller-coaster-tycoon2",
        };
    }

    #ifndef DISABLE_TTF
    std::string GetFontPath(const TTFFontDescriptor& font)
    {
        // set LANG to portable C.UTF-8 so font face names from fontconfig
        // are reported in English
        EnvLangGuard elg;

        LOG_VERBOSE("Looking for font %s with FontConfig.", font.font_name);
        FcConfig* config = FcInitLoadConfigAndFonts();
        if (!config)
        {
            LOG_ERROR("Failed to initialize FontConfig library");
            FcFini();
            return {};
        }

        FcPattern* pat = FcNameParse(reinterpret_cast<const FcChar8*>(font.font_name));

        FcConfigSubstitute(config, pat, FcMatchPattern);
        FcDefaultSubstitute(pat);

        std::string path = "";
        FcResult result = FcResultNoMatch;
        FcPattern* match = FcFontMatch(config, pat, &result);

        if (match)
        {
            bool is_substitute = false;

            // FontConfig implicitly falls back to any default font it is configured to handle.
            // In our implementation, this cannot account for supported character sets, leading
            // to unrendered characters (tofu) when trying to render e.g. CJK characters using a
            // Western (sans-)serif font. We therefore ignore substitutions FontConfig provides,
            // and instead rely on exact matches on the fonts predefined for each font family.
            FcChar8* matched_font_face = nullptr;
            if (FcPatternGetString(match, FC_FULLNAME, 0, &matched_font_face) == FcResultMatch
                && strcmp(font.font_name, reinterpret_cast<const char*>(matched_font_face)) != 0)
            {
                LOG_VERBOSE("FontConfig provided substitute font %s -- disregarding.", matched_font_face);
                is_substitute = true;
            }

            FcChar8* filename = nullptr;
            if (!is_substitute && FcPatternGetString(match, FC_FILE, 0, &filename) == FcResultMatch)
            {
                path = reinterpret_cast<utf8*>(filename);
                LOG_VERBOSE("FontConfig provided font %s", filename);
            }

            FcPatternDestroy(match);
        }
        else
        {
            LOG_WARNING("Failed to find required font.");
        }

        FcPatternDestroy(pat);
        FcConfigDestroy(config);
        FcFini();
        return path;
    }
    #endif // DISABLE_TTF
} // namespace OpenRCT2::Platform

#endif
