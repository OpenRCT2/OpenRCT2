/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__)) || defined(__FreeBSD__)

#include <cstring>
#include <pwd.h>
#include <cstdlib>
#include <ctime>
#include "../core/String.hpp"
#include "Platform2.h"
#include "platform.h"

namespace Platform
{
    uint32_t GetTicks()
    {
        return platform_get_ticks();
    }

    std::string GetEnvironmentVariable(const std::string &name)
    {
        return String::ToStd(getenv(name.c_str()));
    }

    std::string GetEnvironmentPath(const char * name)
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

    std::string GetHomePath()
    {
        std::string path;
        auto pw = getpwuid(getuid());
        if (pw != nullptr)
        {
            path = pw->pw_dir;
        }
        else
        {
            path = GetEnvironmentVariable("HOME");
        }
        if (path.empty())
        {
            path = "/";
        }
        return path;
    }

    std::string FormatShortDate(std::time_t timestamp)
    {
        char date[20];
        std::strftime(date, sizeof(date), "%x", std::gmtime(&timestamp));
        return std::string(date);
    }

    std::string FormatTime(std::time_t timestamp)
    {
        char time[20];
        std::strftime(time, sizeof(time), "%X", std::gmtime(&timestamp));
        return std::string(time);
    }

    bool IsColourTerminalSupported()
    {
        static bool hasChecked = false;
        static bool isSupported = false;
        if (!hasChecked)
        {
            auto term = GetEnvironmentVariable("TERM");
            isSupported =
                term != "cons25" &&
                term != "dumb" &&
                term != "emacs";
            hasChecked = true;
        }
        return isSupported;
    }
}

#endif
