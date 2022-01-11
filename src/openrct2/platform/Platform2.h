/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "platform.h"

#include <ctime>
#include <string>

enum class SPECIAL_FOLDER
{
    USER_CACHE,
    USER_CONFIG,
    USER_DATA,
    USER_HOME,

    RCT2_DISCORD,
};

namespace Platform
{
    uint32_t GetTicks();
    std::string GetEnvironmentVariable(std::string_view name);
    std::string GetFolderPath(SPECIAL_FOLDER folder);
    std::string GetInstallPath();
    std::string GetDocsPath();
    std::string GetCurrentExecutablePath();
    std::string GetCurrentExecutableDirectory();
    bool ShouldIgnoreCase();
    bool IsPathSeparator(char c);
    utf8* GetAbsolutePath(utf8* buffer, size_t bufferSize, const utf8* relativePath);
    uint64_t GetLastModified(std::string_view path);
    uint64_t GetFileSize(std::string_view path);
    std::string ResolveCasing(std::string_view path, bool fileExists);
    std::string SanitiseFilename(std::string_view originalName);

    uint16_t GetLocaleLanguage();
    CurrencyType GetLocaleCurrency();
    CurrencyType GetCurrencyValue(const char* currCode);
    MeasurementFormat GetLocaleMeasurementFormat();
    uint8_t GetLocaleDateFormat();
    TemperatureUnit GetLocaleTemperatureFormat();
    rct2_time GetTimeLocal();
    rct2_date GetDateLocal();

    bool FindApp(std::string_view app, std::string* output);
    int32_t Execute(std::string_view command, std::string* output = nullptr);
    bool ProcessIsElevated();
    float GetDefaultScale();

    bool OriginalGameDataExists(std::string_view path);

    std::string GetUsername();

    std::string GetSteamPath();
#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__)) || defined(__FreeBSD__)
    std::string GetEnvironmentPath(const char* name);
    std::string GetHomePath();
#endif
#ifndef NO_TTF
    std::string GetFontPath(const TTFFontDescriptor& font);
#endif // NO_TTF

    std::string FormatShortDate(std::time_t timestamp);
    std::string FormatTime(std::time_t timestamp);

#ifdef _WIN32
    bool IsOSVersionAtLeast(uint32_t major, uint32_t minor, uint32_t build);
    void SetUpFileAssociations();
    bool SetUpFileAssociation(
        std::string_view extension, std::string_view fileTypeText, std::string_view commandText, std::string_view commandArgs,
        const uint32_t iconIndex);
    void RemoveFileAssociations();
#endif

    bool IsRunningInWine();
    bool IsColourTerminalSupported();
    bool HandleSpecialCommandLineArgument(const char* argument);
    utf8* StrDecompToPrecomp(utf8* input);
    bool RequireNewWindow(bool openGL);
} // namespace Platform
