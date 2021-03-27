/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
    std::string GetEnvironmentVariable(const std::string& name);
    std::string GetFolderPath(SPECIAL_FOLDER folder);
    std::string GetInstallPath();
    std::string GetDocsPath();
    std::string GetCurrentExecutablePath();
    std::string GetCurrentExecutableDirectory();
    bool ShouldIgnoreCase();
    bool FileExists(const std::string path);
    bool IsPathSeparator(char c);
    utf8* GetAbsolutePath(utf8* buffer, size_t bufferSize, const utf8* relativePath);
    uint64_t GetLastModified(const std::string& path);
    std::string ResolveCasing(const std::string& path, bool fileExists);
    rct2_time GetTimeLocal();
    rct2_date GetDateLocal();
    bool FindApp(const std::string& app, std::string* output);
    int32_t Execute(const std::string& command, std::string* output = nullptr);

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__)) || defined(__FreeBSD__)
    std::string GetEnvironmentPath(const char* name);
    std::string GetHomePath();
#endif

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
