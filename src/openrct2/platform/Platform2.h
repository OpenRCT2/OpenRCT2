/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

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
    uint32_t GetTicks();
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

#ifdef _WIN32
    bool IsOSVersionAtLeast(uint32_t major, uint32_t minor, uint32_t build);
#endif

    bool IsColourTerminalSupported();
} // namespace Platform
