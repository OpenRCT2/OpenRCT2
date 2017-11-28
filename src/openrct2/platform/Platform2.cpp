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

#include <cstring>
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#else
    #include <pwd.h>
#endif


#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "Platform2.h"
#include "platform.h"

namespace Platform
{
    uint32 GetTicks()
    {
        return platform_get_ticks();
    }

    std::string GetEnvironmentVariable(const std::string &name)
    {
#ifdef _WIN32
        auto wname = String::ToUtf16(name);
        wchar_t wvalue[MAX_PATH];
        GetEnvironmentVariableW(wname.c_str(), wvalue, sizeof(wvalue));
        return String::ToUtf8(wvalue);
#else
        return String::ToStd(getenv(name.c_str()));
#endif
    }

    static std::string GetEnvironmentPath(const char * name)
    {
        auto value = getenv(name);
        if (value == nullptr)
        {
            return std::string();
        }
        else
        {
            auto colon = std::strchr(value, ':');
            if (colon == nullptr)
            {
                return std::string(value);
            }
            else
            {
                return std::string(value, colon);
            }
        }
    }

    std::string GetFolderPath(SPECIAL_FOLDER folder)
    {
        switch (folder)
        {
#if defined(_WIN32)
        // We currently store everything under Documents/OpenRCT2
        case SPECIAL_FOLDER::USER_CACHE:
        case SPECIAL_FOLDER::USER_CONFIG:
        case SPECIAL_FOLDER::USER_DATA:
            {
                wchar_t wpath[MAX_PATH];
                if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_PERSONAL | CSIDL_FLAG_CREATE, nullptr, 0, wpath)))
                {
                    return ToUtf8(std::wstring(wpath));
                }
                else
                {
                    return GetFolderPath(SPECIAL_FOLDER::USER_HOME);
                }
            }
        case SPECIAL_FOLDER::USER_HOME:
            {
                wchar_t wpath[MAX_PATH];
                if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_PROFILE | CSIDL_FLAG_CREATE, nullptr, 0, wpath)))
                {
                    return ToUtf8(std::wstring(wpath));
                }
                else
                {
                    // Try default environment variables
                    auto homedrive = GetEnvironmentVariable("HOMEDRIVE");
                    auto homepath = GetEnvironmentVariable("HOMEPATH");
                    if (!homedrive.empty() && !homepath.empty())
                    {
                        return Path::Combine(homedrive, homepath);
                    }
                    else
                    {
                        return "C:\\";
                    }
                }
            }
#elif defined (__ANDROID__)
        // Andorid builds currently only read from /sdcard/openrct2*
        case SPECIAL_FOLDER::USER_CACHE:
        case SPECIAL_FOLDER::USER_CONFIG:
        case SPECIAL_FOLDER::USER_DATA:
        case SPECIAL_FOLDER::USER_HOME:
            return "/sdcard";
#elif defined (__MACOS__)
        // macOS stores everything in ~/Library/Application Support/OpenRCT2
        case SPECIAL_FOLDER::USER_CACHE:
        case SPECIAL_FOLDER::USER_CONFIG:
        case SPECIAL_FOLDER::USER_DATA:
        case SPECIAL_FOLDER::USER_HOME:
            {
                auto home = GetFolderPath(SPECIAL_FOLDER::USER_HOME);
                return Path::Combine(home, "/Library/Application Support");
            }
#else
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
            return getpwuid(getuid())->pw_dir;
#endif
        }
    }

    std::string GetInstallPath()
    {
        utf8 path[260];
        platform_resolve_openrct_data_path();
        platform_get_openrct_data_path(path, sizeof(path));
        return path;
    }
}
