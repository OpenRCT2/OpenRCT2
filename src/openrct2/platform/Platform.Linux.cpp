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

#ifdef __linux__

#include <pwd.h>
#include "../core/Path.hpp"
#include "Platform2.h"

namespace Platform
{
    std::string GetFolderPath(SPECIAL_FOLDER folder)
    {
        switch (folder)
        {
        case SPECIAL_FOLDER::USER_CACHE:
            {
                auto path = GetEnvironmentPath("XDG_CACHE_HOME");
                if (path.empty())
                {
                    auto home = GetFolderPath(SPECIAL_FOLDER::USER_HOME);
                    path = Path::Combine(home, ".cache");
                }
                return path;
            }
        case SPECIAL_FOLDER::USER_CONFIG:
            {
                auto path = GetEnvironmentPath("XDG_CONFIG_HOME");
                if (path.empty())
                {
                    auto home = GetFolderPath(SPECIAL_FOLDER::USER_HOME);
                    path = Path::Combine(home, ".config");
                }
                return path;
            }
        case SPECIAL_FOLDER::USER_DATA:
            {
                auto path = GetEnvironmentPath("XDG_DATA_HOME");
                if (path.empty())
                {
                    auto home = GetFolderPath(SPECIAL_FOLDER::USER_HOME);
                    path = Path::Combine(home, ".local/share");
                }
                return path;
            }
        case SPECIAL_FOLDER::USER_HOME:
            return GetHomePath();
        default:
            return std::string();
        }
    }
}

#endif
