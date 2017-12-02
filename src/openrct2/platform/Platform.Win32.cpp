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

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlobj.h>
#undef GetEnvironmentVariable

#if !defined(__MINGW32__) && ((NTDDI_VERSION >= NTDDI_VISTA) && !defined(_USING_V110_SDK71_) && !defined(_ATL_XP_TARGETING))
    #define __USE_SHGETKNOWNFOLDERPATH__
#endif

#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../core/Util.hpp"
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
        std::wstring result;
        auto wname = String::ToUtf16(name);
        wchar_t wvalue[256];
        auto valueSize = GetEnvironmentVariableW(wname.c_str(), wvalue, (DWORD)Util::CountOf(wvalue));
        if (valueSize < Util::CountOf(wvalue))
        {
            result = wvalue;
        }
        else
        {
            auto wlvalue = new wchar_t[valueSize];
            GetEnvironmentVariableW(wname.c_str(), wlvalue, valueSize);
            result = wlvalue;
            delete[] wlvalue;
        }
        return String::ToUtf8(result);
    }

    static std::string GetHomePathViaEnvironment()
    {
        std::string result;
        auto homedrive = GetEnvironmentVariable("HOMEDRIVE");
        auto homepath = GetEnvironmentVariable("HOMEPATH");
        if (!homedrive.empty() && !homepath.empty())
        {
            result = Path::Combine(homedrive, homepath);
        }
        return result;
    }

#ifdef __USE_SHGETKNOWNFOLDERPATH__
    static std::string WIN32_GetKnownFolderPath(REFKNOWNFOLDERID rfid)
    {
        std::string path;
        wchar_t * wpath = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(rfid, KF_FLAG_CREATE, nullptr, &wpath)))
        {
            path = String::ToUtf8(std::wstring(wpath));
        }
        CoTaskMemFree(wpath);
        return path;
    }
#else
    static std::string WIN32_GetFolderPath(int nFolder)
    {
        std::string path;
        wchar_t wpath[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathW(nullptr, nFolder | CSIDL_FLAG_CREATE, nullptr, 0, wpath)))
        {
            path = String::ToUtf8(std::wstring(wpath));
        }
        return path;
    }
#endif

    std::string GetFolderPath(SPECIAL_FOLDER folder)
    {
        switch (folder)
        {
        // We currently store everything under Documents/OpenRCT2
        case SPECIAL_FOLDER::USER_CACHE:
        case SPECIAL_FOLDER::USER_CONFIG:
        case SPECIAL_FOLDER::USER_DATA:
            {
#ifdef __USE_SHGETKNOWNFOLDERPATH__
                auto path = WIN32_GetKnownFolderPath(FOLDERID_Documents);
#else
                auto path = WIN32_GetFolderPath(CSIDL_PERSONAL);
#endif
                if (path.empty())
                {
                    path = GetFolderPath(SPECIAL_FOLDER::USER_HOME);
                }
                return path;
            }
        case SPECIAL_FOLDER::USER_HOME:
            {
#ifdef __USE_SHGETKNOWNFOLDERPATH__
                auto path = WIN32_GetKnownFolderPath(FOLDERID_Profile);
#else
                auto path = WIN32_GetFolderPath(CSIDL_PROFILE);
#endif
                if (path.empty())
                {
                    path = GetHomePathViaEnvironment();
                    if (path.empty())
                    {
                        path = "C:\\";
                    }
                }
                return path;
            }
        default:
            return std::string();
        }
    }

    std::string GetInstallPath()
    {
        utf8 path[MAX_PATH];
        platform_resolve_openrct_data_path();
        platform_get_openrct_data_path(path, sizeof(path));
        return path;
    }
}

#endif
