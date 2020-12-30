/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__)) || defined(__FreeBSD__)

#    include "../core/String.hpp"
#    include "Platform2.h"
#    include "platform.h"

#    include <clocale>
#    include <cstdlib>
#    include <cstring>
#    include <ctime>
#    include <pwd.h>

namespace Platform
{
    uint32_t GetTicks()
    {
        return platform_get_ticks();
    }

    std::string GetEnvironmentVariable(const std::string& name)
    {
        return String::ToStd(getenv(name.c_str()));
    }

    std::string GetEnvironmentPath(const char* name)
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
        setlocale(LC_TIME, "");
        char date[20];
        std::strftime(date, sizeof(date), "%x", std::localtime(&timestamp));
        return std::string(date);
    }

    std::string FormatTime(std::time_t timestamp)
    {
        setlocale(LC_TIME, "");
        char time[20];
        std::strftime(time, sizeof(time), "%X", std::localtime(&timestamp));
        return std::string(time);
    }

    bool IsColourTerminalSupported()
    {
        static bool hasChecked = false;
        static bool isSupported = false;
        if (!hasChecked)
        {
            auto term = GetEnvironmentVariable("TERM");
            isSupported = term != "cons25" && term != "dumb" && term != "emacs";
            hasChecked = true;
        }
        return isSupported;
    }

    bool IsRunningInWine()
    {
        return false;
    }

    bool FindApp(const std::string& app, std::string* output)
    {
        return Execute(String::StdFormat("which %s 2> /dev/null", app.c_str()), output) == 0;
    }

    int32_t Execute(const std::string& command, std::string* output)
    {
#    ifndef __EMSCRIPTEN__
        log_verbose("executing \"%s\"...", command.c_str());
        FILE* fpipe = popen(command.c_str(), "r");
        if (fpipe == nullptr)
        {
            return -1;
        }
        if (output != nullptr)
        {
            // Read output into buffer
            std::vector<char> outputBuffer;
            char buffer[1024];
            size_t readBytes;
            while ((readBytes = fread(buffer, 1, sizeof(buffer), fpipe)) > 0)
            {
                outputBuffer.insert(outputBuffer.begin(), buffer, buffer + readBytes);
            }

            // Trim line breaks
            size_t outputLength = outputBuffer.size();
            for (size_t i = outputLength - 1; i != SIZE_MAX; i--)
            {
                if (outputBuffer[i] == '\n')
                {
                    outputLength = i;
                }
                else
                {
                    break;
                }
            }

            // Convert to string
            *output = std::string(outputBuffer.data(), outputLength);
        }
        else
        {
            fflush(fpipe);
        }

        // Return exit code
        return pclose(fpipe);
#    else
        log_warning("Emscripten cannot execute processes. The commandline was '%s'.", command.c_str());
        return -1;
#    endif // __EMSCRIPTEN__
    }
} // namespace Platform

#endif
