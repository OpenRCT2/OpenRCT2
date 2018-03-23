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

#pragma once

#include <ctime>
#include <string>
#include "../common.h"

enum class SPECIAL_FOLDER
{
    USER_CACHE,
    USER_CONFIG,
    USER_DATA,
    USER_HOME,
};

namespace Platform
{
    uint32 GetTicks();
    std::string GetEnvironmentVariable(const std::string &name);
    std::string GetFolderPath(SPECIAL_FOLDER folder);
    std::string GetInstallPath();
    std::string GetDocsPath();
    std::string GetCurrentExecutablePath();

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__)) || defined(__FreeBSD__)
    std::string GetEnvironmentPath(const char * name);
    std::string GetHomePath();
#endif

    std::string FormatShortDate(std::time_t timestamp);
    std::string FormatTime(std::time_t timestamp);

    bool IsColourTerminalSupported();
}
