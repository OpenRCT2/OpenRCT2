/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#if defined(__APPLE__) && defined(__MACH__)

#    include "../OpenRCT2.h"
#    include "../core/Path.hpp"
#    include "../core/String.hpp"
#    include "Platform2.h"

// undefine `interface` and `abstract`, because it's causing conflicts with Objective-C's keywords
#    undef interface
#    undef abstract

#    include <Foundation/Foundation.h>
#    include <mach-o/dyld.h>

namespace Platform
{
    std::string GetFolderPath(SPECIAL_FOLDER folder)
    {
        // macOS stores everything in ~/Library/Application Support/OpenRCT2
        switch (folder)
        {
            case SPECIAL_FOLDER::USER_CACHE:
            case SPECIAL_FOLDER::USER_CONFIG:
            case SPECIAL_FOLDER::USER_DATA:
            {
                auto home = GetFolderPath(SPECIAL_FOLDER::USER_HOME);
                return Path::Combine(home, "Library/Application Support");
            }
            case SPECIAL_FOLDER::USER_HOME:
                return GetHomePath();
            default:
                return std::string();
        }
    }

    static std::string GetBundlePath()
    {
        @autoreleasepool
        {
            NSBundle* bundle = [NSBundle mainBundle];
            if (bundle)
            {
                auto resources = bundle.resourcePath.UTF8String;
                if (Path::DirectoryExists(resources))
                {
                    return resources;
                }
            }
            return std::string();
        }
    }

    std::string GetDocsPath()
    {
        return GetBundlePath();
    }

    std::string GetInstallPath()
    {
        auto path = std::string(gCustomOpenRCT2DataPath);
        if (!path.empty())
        {
            path = Path::GetAbsolute(path);
        }
        else
        {
            auto exePath = GetCurrentExecutablePath();
            auto exeDirectory = Path::GetDirectory(exePath);
            path = Path::Combine(exeDirectory, "data");
            NSString* nsPath = [NSString stringWithUTF8String:path.c_str()];
            if (![[NSFileManager defaultManager] fileExistsAtPath:nsPath])
            {
                path = GetBundlePath();
                if (path.empty())
                {
                    path = "/";
                }
            }
        }
        return path;
    }

    std::string GetCurrentExecutablePath()
    {
        char exePath[MAX_PATH];
        uint32_t size = MAX_PATH;
        int result = _NSGetExecutablePath(exePath, &size);
        if (result == 0)
        {
            return exePath;
        }
        else
        {
            return std::string();
        }
    }

    utf8* StrDecompToPrecomp(utf8* input)
    {
        @autoreleasepool
        {
            if (input == NULL)
            {
                return 0;
            }

            NSString* inputDecomp = [NSString stringWithUTF8String:input];
            return strdup([inputDecomp.precomposedStringWithCanonicalMapping cStringUsingEncoding:NSUTF8StringEncoding]);
        }
    }

    bool HandleSpecialCommandLineArgument(const char* argument)
    {
        if (String::Equals(argument, "-NSDocumentRevisionsDebugMode"))
        {
            return true;
        }
        if (String::StartsWith(argument, "-psn_"))
        {
            return true;
        }
        return false;
    }

}

#endif
