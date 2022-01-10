/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#if defined(__APPLE__) && defined(__MACH__)

#    include "../config/Config.h"
#    include "../core/Path.hpp"
#    include "../localisation/Language.h"
#    include "../util/Util.h"
#    include "platform.h"

// undefine `interface` and `abstract`, because it's causing conflicts with Objective-C's keywords
#    undef interface
#    undef abstract

#    import <AppKit/AppKit.h>
#    import <Foundation/Foundation.h>
#    include <mach-o/dyld.h>
#    include <pwd.h>

#    ifndef NO_TTF
bool platform_get_font_path(TTFFontDescriptor* font, utf8* buffer, size_t size)
{
    @autoreleasepool
    {
        CTFontDescriptorRef fontRef = CTFontDescriptorCreateWithNameAndSize(
            static_cast<CFStringRef>([NSString stringWithUTF8String:font->font_name]), 0.0);
        CFURLRef url = static_cast<CFURLRef>(CTFontDescriptorCopyAttribute(fontRef, kCTFontURLAttribute));
        if (url)
        {
            NSString* fontPath = [NSString stringWithString:[static_cast<NSURL*>(CFBridgingRelease(url)) path]];
            safe_strcpy(buffer, fontPath.UTF8String, size);
            return true;
        }
        else
        {
            return false;
        }
    }
}
#    endif // NO_TTF

bool platform_get_steam_path(utf8* outPath, size_t outSize)
{
    char steamPath[1024] = { 0 };
    const char* homeDir = getpwuid(getuid())->pw_dir;
    if (homeDir != NULL)
    {
        safe_strcpy(steamPath, homeDir, sizeof(steamPath));
        safe_strcat_path(
            steamPath, "Library/Application Support/Steam/Steam.AppBundle/Steam/Contents/MacOS/steamapps", sizeof(steamPath));
        if (Path::DirectoryExists(steamPath))
        {
            safe_strcpy(outPath, steamPath, outSize);
            return true;
        }
    }
    return false;
}

#endif
