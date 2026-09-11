/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

// AmigaOS 3.x platform layer: the union of what Platform.Posix.cpp and
// Platform.Linux.cpp provide, on top of libnix. Anything that touches the
// OS directly goes through amiga_os.c so the NDK headers stay out of C++.

#ifdef __amigaos__

    #include "../Date.h"
    #include "../Diagnostic.h"
    #include "../OpenRCT2.h"
    #include "../core/Path.hpp"
    #include "../core/String.hpp"
    #include "../drawing/Font.h"
    #include "../localisation/Language.h"
    #include "Platform.h"

    #include <cerrno>
    #include <clocale>
    #include <cstdlib>
    #include <cstring>
    #include <ctime>
    #include <sys/stat.h>
    #include <sys/time.h>
    #include <unistd.h>

extern "C" {
void amiga_sleep_ms(unsigned ms);
unsigned amiga_ticks_ms(void);
int amiga_program_path(char* buf, unsigned size);
}

namespace OpenRCT2::Platform
{
    std::string GetEnvironmentVariable(std::string_view name)
    {
        return String::toStd(getenv(std::string(name).c_str()));
    }

    std::string GetEnvironmentPath(const char* name)
    {
        auto value = getenv(name);
        return value == nullptr ? std::string() : std::string(value);
    }

    std::string GetHomePath()
    {
        return "PROGDIR:";
    }

    std::string GetFolderPath(SpecialFolder folder)
    {
        switch (folder)
        {
            case SpecialFolder::userCache:
            case SpecialFolder::userConfig:
            case SpecialFolder::userData:
                return "PROGDIR:user";
            case SpecialFolder::userHome:
                return GetHomePath();
            default:
                return std::string();
        }
    }

    std::string GetCurrentExecutablePath()
    {
        char exePath[512];
        if (!amiga_program_path(exePath, sizeof(exePath)))
        {
            return "PROGDIR:openrct2";
        }
        return exePath;
    }

    static std::string GetCurrentWorkingDirectory()
    {
        char cwdPath[512];
        if (getcwd(cwdPath, sizeof(cwdPath)) != nullptr)
        {
            return cwdPath;
        }
        return std::string();
    }

    std::string GetInstallPath()
    {
        if (!gCustomOpenRCT2DataPath.empty())
        {
            return Path::GetAbsolute(gCustomOpenRCT2DataPath);
        }
        const std::string candidates[] = {
            "PROGDIR:data",
            Path::Combine(GetCurrentWorkingDirectory(), "data"),
        };
        for (const auto& path : candidates)
        {
            LOG_VERBOSE("Looking for OpenRCT2 data in %s", path.c_str());
            if (Path::DirectoryExists(path))
            {
                return path;
            }
        }
        return "PROGDIR:data";
    }

    std::string GetDocsPath()
    {
        return "PROGDIR:doc";
    }

    std::string FormatShortDate(std::time_t timestamp)
    {
        char date[20];
        std::strftime(date, sizeof(date), "%d/%m/%Y", std::localtime(&timestamp));
        return std::string(date);
    }

    std::string FormatTime(std::time_t timestamp)
    {
        char time[20];
        std::strftime(time, sizeof(time), "%H:%M:%S", std::localtime(&timestamp));
        return std::string(time);
    }

    bool IsColourTerminalSupported()
    {
        return false;
    }

    bool IsRunningInWine()
    {
        return false;
    }

    bool FindApp(std::string_view, std::string*)
    {
        return false;
    }

    int32_t Execute(const char* args[], std::string*)
    {
        LOG_WARNING("AmigaOS build cannot execute processes (%s).", args != nullptr && args[0] != nullptr ? args[0] : "");
        return -1;
    }

    uint64_t GetLastModified(std::string_view path)
    {
        struct stat statInfo{};
        if (stat(std::string(path).c_str(), &statInfo) == 0)
        {
            return static_cast<uint64_t>(statInfo.st_mtime);
        }
        return 0;
    }

    uint64_t GetFileSize(std::string_view path)
    {
        struct stat statInfo{};
        if (stat(std::string(path).c_str(), &statInfo) == 0)
        {
            return static_cast<uint64_t>(statInfo.st_size);
        }
        return 0;
    }

    bool ShouldIgnoreCase()
    {
        // Amiga filesystems are case-insensitive.
        return true;
    }

    bool IsPathSeparator(char c)
    {
        // "Volume:" is a complete directory reference on AmigaOS, so ':' must
        // count as a separator or Path::Combine would produce "Volume:/dir".
        return c == '/' || c == ':';
    }

    std::string ResolveCasing(std::string_view path, bool)
    {
        return std::string(path);
    }

    bool RequireNewWindow(bool)
    {
        return true;
    }

    std::string GetUsername()
    {
        auto user = GetEnvironmentVariable("USER");
        return user.empty() ? std::string("amiga") : user;
    }

    uint16_t GetLocaleLanguage()
    {
        return LANGUAGE_ENGLISH_UK;
    }

    CurrencyType GetLocaleCurrency()
    {
        return Platform::GetCurrencyValue(nullptr);
    }

    MeasurementFormat GetLocaleMeasurementFormat()
    {
        return MeasurementFormat::metric;
    }

    uint8_t GetLocaleDateFormat()
    {
        return DATE_FORMAT_DAY_MONTH_YEAR;
    }

    TemperatureUnit GetLocaleTemperatureFormat()
    {
        return TemperatureUnit::celsius;
    }

    bool ProcessIsElevated()
    {
        return false;
    }

    bool LockSingleInstance()
    {
        return true;
    }

    int32_t GetDrives()
    {
        return 0;
    }

    time_t FileGetModifiedTime(u8string_view path)
    {
        struct stat buf{};
        if (stat(u8string(path).c_str(), &buf) == 0)
        {
            return buf.st_mtime;
        }
        return 100;
    }

    datetime64 GetDatetimeNowUTC()
    {
        const datetime64 epochAsTicks = 621355968000000000;
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        uint64_t utcEpochTicks = static_cast<uint64_t>(tv.tv_sec) * 10000000uLL + static_cast<uint64_t>(tv.tv_usec) * 10;
        return epochAsTicks + utcEpochTicks;
    }

    u8string StrDecompToPrecomp(u8string_view input)
    {
        return u8string(input);
    }

    bool HandleSpecialCommandLineArgument(const char*)
    {
        return false;
    }

    SteamPaths GetSteamPaths()
    {
        return {};
    }

    std::string GetFontPath(const TTFFontDescriptor&)
    {
        return {};
    }

    void Sleep(uint32_t ms)
    {
        amiga_sleep_ms(ms);
    }

    static const uint32_t _processStartTicks = amiga_ticks_ms();

    uint32_t GetTicks()
    {
        return amiga_ticks_ms() - _processStartTicks;
    }
} // namespace OpenRCT2::Platform

#endif
