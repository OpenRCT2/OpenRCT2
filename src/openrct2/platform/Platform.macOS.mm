/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#if defined(__APPLE__) && defined(__MACH__)

#include <Foundation/Foundation.h>
#include <mach-o/dyld.h>

#include "../core/Path.hpp"
#include "../OpenRCT2.h"
#include "Platform2.h"

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

    std::string GetDocsPath()
    {
        return std::string();
    }

    static std::string GetBundlePath()
    {
        @autoreleasepool
        {
            NSBundle * bundle = [NSBundle mainBundle];
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

    std::string GetInstallPath()
    {
        auto path = std::string(gCustomOpenrctDataPath);
        if (!path.empty())
        {
            path = Path::GetAbsolute(path);
        }
        else
        {
            auto exePath = GetCurrentExecutablePath();
            auto exeDirectory = Path::GetDirectory(exePath);
            path = Path::Combine(exeDirectory, "data");
            NSString * nsPath = [NSString stringWithUTF8String:path.c_str()];
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
}

#endif
