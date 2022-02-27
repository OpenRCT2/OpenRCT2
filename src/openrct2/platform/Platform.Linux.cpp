/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#if defined(__unix__) && !defined(__ANDROID__) && !defined(__APPLE__)

#    include <cstring>
#    include <fnmatch.h>
#    include <limits.h>
#    include <locale.h>
#    include <pwd.h>
#    include <vector>
#    if defined(__FreeBSD__) || defined(__NetBSD__)
#        include <stddef.h>
#        include <sys/param.h>
#        include <sys/sysctl.h>
#    endif // __FreeBSD__ || __NetBSD__
#    if defined(__linux__)
// for PATH_MAX
#        include <linux/limits.h>
#    endif // __linux__
#    ifndef NO_TTF
#        include <fontconfig/fontconfig.h>
#    endif // NO_TTF

#    include "../OpenRCT2.h"
#    include "../core/Path.hpp"
#    include "../localisation/Language.h"
#    include "Platform.h"

namespace Platform
{
    std::string GetFolderPath(SPECIAL_FOLDER folder)
    {
        switch (folder)
        {
            case SPECIAL_FOLDER::USER_CACHE:
            case SPECIAL_FOLDER::USER_CONFIG:
            case SPECIAL_FOLDER::USER_DATA:
            {
                auto path = GetEnvironmentPath("XDG_CONFIG_HOME");
                if (path.empty())
                {
                    auto home = GetFolderPath(SPECIAL_FOLDER::USER_HOME);
                    path = Path::Combine(home, u8".config");
                }
                return path;
            }
            case SPECIAL_FOLDER::USER_HOME:
                return GetHomePath();
            default:
                return std::string();
        }
    }

    std::string GetDocsPath()
    {
        static const utf8* searchLocations[] = {
            "./doc",
            "/usr/share/doc/openrct2",
        };
        for (auto searchLocation : searchLocations)
        {
            log_verbose("Looking for OpenRCT2 doc path at %s", searchLocation);
            if (Path::DirectoryExists(searchLocation))
            {
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
        std::vector<std::string> prefixes;
        auto exePath = Platform::GetCurrentExecutablePath();
        prefixes.push_back(Path::GetDirectory(exePath));
        prefixes.push_back(GetCurrentWorkingDirectory());
        prefixes.push_back("/");
        static const char* SearchLocations[] = {
            "/data",
            "../share/openrct2",
#    ifdef ORCT2_RESOURCE_DIR
            // defined in CMakeLists.txt
            ORCT2_RESOURCE_DIR,
#    endif // ORCT2_RESOURCE_DIR
            "/usr/local/share/openrct2",
            "/var/lib/openrct2",
            "/usr/share/openrct2",
        };
        for (const auto& prefix : prefixes)
        {
            for (auto searchLocation : SearchLocations)
            {
                auto prefixedPath = Path::Combine(prefix, searchLocation);
                log_verbose("Looking for OpenRCT2 data in %s", prefixedPath.c_str());
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
#    ifdef __linux__
        auto bytesRead = readlink("/proc/self/exe", exePath, sizeof(exePath));
        if (bytesRead == -1)
        {
            log_fatal("failed to read /proc/self/exe");
        }
#    elif defined(__FreeBSD__) || defined(__NetBSD__)
#        if defined(__FreeBSD__)
        const int32_t mib[] = {
            CTL_KERN,
            KERN_PROC,
            KERN_PROC_PATHNAME,
            -1,
        };
#        else
        const int32_t mib[] = {
            CTL_KERN,
            KERN_PROC_ARGS,
            -1,
            KERN_PROC_PATHNAME,
        };
#        endif
        auto exeLen = sizeof(exePath);
        if (sysctl(mib, 4, exePath, &exeLen, nullptr, 0) == -1)
        {
            log_fatal("failed to get process path");
        }
#    elif defined(__OpenBSD__)
        // There is no way to get the path name of a running executable.
        // If you are not using the port or package, you may have to change this line!
        strlcpy(exePath, "/usr/local/bin/", sizeof(exePath));
#    else
#        error "Platform does not support full path exe retrieval"
#    endif
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
            }                                         // end strip
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
#    ifdef LC_MEASUREMENT
        const char* langstring = setlocale(LC_MEASUREMENT, "");
#    else
        const char* langstring = setlocale(LC_ALL, "");
#    endif

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

    std::string GetSteamPath()
    {
        const char* steamRoot = getenv("STEAMROOT");
        if (steamRoot != nullptr)
        {
            return Path::Combine(steamRoot, u8"ubuntu12_32/steamapps/content");
        }

        const char* localSharePath = getenv("XDG_DATA_HOME");
        if (localSharePath != nullptr)
        {
            auto steamPath = Path::Combine(localSharePath, u8"Steam/ubuntu12_32/steamapps/content");
            if (Path::DirectoryExists(steamPath))
            {
                return steamPath;
            }
        }

        const char* homeDir = getpwuid(getuid())->pw_dir;
        if (homeDir == nullptr)
        {
            return {};
        }

        auto steamPath = Path::Combine(homeDir, u8".local/share/Steam/ubuntu12_32/steamapps/content");
        if (Path::DirectoryExists(steamPath))
        {
            return steamPath;
        }

        steamPath = Path::Combine(homeDir, u8".steam/steam/ubuntu12_32/steamapps/content");
        if (Path::DirectoryExists(steamPath))
        {
            return steamPath;
        }

        return {};
    }

#    ifndef NO_TTF
    std::string GetFontPath(const TTFFontDescriptor& font)
    {
        log_verbose("Looking for font %s with FontConfig.", font.font_name);
        FcConfig* config = FcInitLoadConfigAndFonts();
        if (!config)
        {
            log_error("Failed to initialize FontConfig library");
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
                log_verbose("FontConfig provided substitute font %s -- disregarding.", matched_font_face);
                is_substitute = true;
            }

            FcChar8* filename = nullptr;
            if (!is_substitute && FcPatternGetString(match, FC_FILE, 0, &filename) == FcResultMatch)
            {
                path = reinterpret_cast<utf8*>(filename);
                log_verbose("FontConfig provided font %s", filename);
            }

            FcPatternDestroy(match);
        }
        else
        {
            log_warning("Failed to find required font.");
        }

        FcPatternDestroy(pat);
        FcConfigDestroy(config);
        FcFini();
        return path;
    }
#    endif // NO_TTF
} // namespace Platform

#endif
