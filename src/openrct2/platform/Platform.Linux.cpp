/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#if defined(__FreeBSD__) || defined(__OpenBSD__) || (defined(__linux__) && !defined(__ANDROID__))

#include <limits.h>
#include <pwd.h>
#if defined(__FreeBSD__)
#include <stddef.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#endif // __FreeBSD__
#if defined(__linux__)
// for PATH_MAX
#include <linux/limits.h>
#endif // __linux__
#include "../core/Path.hpp"
#include "../core/Util.hpp"
#include "../OpenRCT2.h"
#include "platform.h"
#include "Platform2.h"

namespace Platform
{
    std::string GetFolderPath(SPECIAL_FOLDER folder)
    {
        switch (folder)
        {
        case SPECIAL_FOLDER::USER_CACHE:
        case SPECIAL_FOLDER::USER_CONFIG:
        case SPECIAL_FOLDER::USER_DATA:
            {
                auto path = GetEnvironmentPath("XDG_CONFIG_HOME");
                if (path.empty())
                {
                    auto home = GetFolderPath(SPECIAL_FOLDER::USER_HOME);
                    path = Path::Combine(home, ".config");
                }
                return path;
            }
        case SPECIAL_FOLDER::USER_HOME:
            return GetHomePath();
        default:
            return std::string();
        }
    }

    std::string GetDocsPath()
    {
        static const utf8 * searchLocations[] =
        {
            "./doc",
            "/usr/share/doc/openrct2",
        };
        for (auto searchLocation : searchLocations)
        {
            log_verbose("Looking for OpenRCT2 doc path at %s", searchLocation);
            if (platform_directory_exists(searchLocation))
            {
                return searchLocation;
            }
        }
        return std::string();
    }

    static std::string FindInstallPath()
    {
        static const char * SearchLocations[] =
        {
            "../share/openrct2",
#ifdef ORCT2_RESOURCE_DIR
            // defined in CMakeLists.txt
            ORCT2_RESOURCE_DIR,
#endif // ORCT2_RESOURCE_DIR
            "/usr/local/share/openrct2",
            "/var/lib/openrct2",
            "/usr/share/openrct2",
        };
        for (auto searchLocation : SearchLocations)
        {
            log_verbose("Looking for OpenRCT2 data in %s", searchLocation);
            if (Path::DirectoryExists(searchLocation))
            {
                return searchLocation;
            }
        }
        return std::string();
    }

    static std::string GetCurrentWorkingDirectory()
    {
        char cwdPath[PATH_MAX];
        if (getcwd(cwdPath, sizeof(cwdPath)) != nullptr)
        {
            return cwdPath;
        }
        return std::string();
    }

    std::string GetInstallPath()
    {
        // 1. Try command line argument
        auto path = std::string(gCustomOpenrctDataPath);
        if (!path.empty())
        {
            path = Path::GetAbsolute(path);
        }
        else
        {
            // 2. Try ${exeDir}/data
            auto exePath = Platform::GetCurrentExecutablePath();
            auto exeDirectory = Path::GetDirectory(exePath);
            path = Path::Combine(exeDirectory, "data");
            if (!Path::DirectoryExists(path))
            {
                // 3. Try standard system app directories
                path = FindInstallPath();
                if (path.empty())
                {
                    // 4. Fallback to ${cwd}/data
                    path = GetCurrentWorkingDirectory();
                    if (!path.empty())
                    {
                        path = Path::Combine(path, "data");
                    }
                    else
                    {
                        return "/";
                    }
                }
            }
        }
        return path;
    }

    std::string GetCurrentExecutablePath()
    {
        char exePath[PATH_MAX] = { 0 };
#ifdef __linux__
        auto bytesRead = readlink("/proc/self/exe", exePath, sizeof(exePath));
        if (bytesRead == -1)
        {
            log_fatal("failed to read /proc/self/exe");
        }
#elif defined(__FreeBSD__)
        const int32_t mib[] = { CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1 };
        auto exeLen = sizeof(exePath);
        if (sysctl(mib, 4, exePath, &exeLen, nullptr, 0) == -1)
        {
            log_fatal("failed to get process path");
        }
#elif defined(__OpenBSD__)
        // There is no way to get the path name of a running executable.
        // If you are not using the port or package, you may have to change this line!
        strlcpy(exePath, "/usr/local/bin/", sizeof(exePath));
#else
    #error "Platform does not support full path exe retrieval"
#endif
        return exePath;
    }
}

#endif
