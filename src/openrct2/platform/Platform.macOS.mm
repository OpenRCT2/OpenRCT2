#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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
        uint32 size = MAX_PATH;
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
