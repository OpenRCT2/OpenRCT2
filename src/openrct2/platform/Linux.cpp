/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../common.h"

// Despite the name, this file contains support for more OSs besides Linux, provided the necessary ifdefs remain small.
// Otherwise, they should be spun off into their own files.
#if defined(__unix__) && !defined(__ANDROID__) && !defined(__APPLE__)

#    ifdef __FreeBSD__
#        include <sys/sysctl.h>
#    endif

#    include <cstring>
#    include <ctype.h>
#    include <dlfcn.h>
#    include <errno.h>
#    ifndef NO_TTF
#        include <fontconfig/fontconfig.h>
#    endif // NO_TTF
#    include "../config/Config.h"
#    include "../core/File.h"
#    include "../core/Path.hpp"
#    include "../localisation/Language.h"
#    include "../localisation/StringIds.h"
#    include "../util/Util.h"
#    include "platform.h"

#    include <pwd.h>

bool platform_get_steam_path(utf8* outPath, size_t outSize)
{
    const char* steamRoot = getenv("STEAMROOT");
    if (steamRoot != nullptr)
    {
        safe_strcpy(outPath, steamRoot, outSize);
        safe_strcat_path(outPath, "ubuntu12_32/steamapps/content", outSize);
        return true;
    }

    char steamPath[1024] = { 0 };
    const char* localSharePath = getenv("XDG_DATA_HOME");
    if (localSharePath != nullptr)
    {
        safe_strcpy(steamPath, localSharePath, sizeof(steamPath));
        safe_strcat_path(steamPath, "Steam/ubuntu12_32/steamapps/content", sizeof(steamPath));
        if (Path::DirectoryExists(steamPath))
        {
            safe_strcpy(outPath, steamPath, outSize);
            return true;
        }
    }

    const char* homeDir = getpwuid(getuid())->pw_dir;
    if (homeDir != nullptr)
    {
        safe_strcpy(steamPath, homeDir, sizeof(steamPath));
        safe_strcat_path(steamPath, ".local/share/Steam/ubuntu12_32/steamapps/content", sizeof(steamPath));
        if (Path::DirectoryExists(steamPath))
        {
            safe_strcpy(outPath, steamPath, outSize);
            return true;
        }

        std::fill_n(steamPath, sizeof(steamPath), 0x00);
        safe_strcpy(steamPath, homeDir, sizeof(steamPath));
        safe_strcat_path(steamPath, ".steam/steam/ubuntu12_32/steamapps/content", sizeof(steamPath));
        if (Path::DirectoryExists(steamPath))
        {
            safe_strcpy(outPath, steamPath, outSize);
            return true;
        }
    }
    return false;
}

#    ifndef NO_TTF
bool platform_get_font_path(TTFFontDescriptor* font, utf8* buffer, size_t size)
{
    assert(buffer != nullptr);
    assert(font != nullptr);

    log_verbose("Looking for font %s with FontConfig.", font->font_name);
    FcConfig* config = FcInitLoadConfigAndFonts();
    if (!config)
    {
        log_error("Failed to initialize FontConfig library");
        FcFini();
        return false;
    }

    FcPattern* pat = FcNameParse(reinterpret_cast<const FcChar8*>(font->font_name));

    FcConfigSubstitute(config, pat, FcMatchPattern);
    FcDefaultSubstitute(pat);

    bool found = false;
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
            && strcmp(font->font_name, reinterpret_cast<const char*>(matched_font_face)) != 0)
        {
            log_verbose("FontConfig provided substitute font %s -- disregarding.", matched_font_face);
            is_substitute = true;
        }

        FcChar8* filename = nullptr;
        if (!is_substitute && FcPatternGetString(match, FC_FILE, 0, &filename) == FcResultMatch)
        {
            found = true;
            safe_strcpy(buffer, reinterpret_cast<utf8*>(filename), size);
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
    return found;
}
#    endif // NO_TTF

#endif
