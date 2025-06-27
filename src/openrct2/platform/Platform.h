/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../config/ConfigTypes.h"
#include "../core/DateTime.h"
#include "../core/StringTypes.h"

#include <bit>
#include <cmath>
#include <ctime>
#include <optional>
#include <vector>

#ifdef _WIN32
    #define PATH_SEPARATOR u8"\\"
    #define PLATFORM_NEWLINE u8"\r\n"
#else
    #define PATH_SEPARATOR u8"/"
    #define PLATFORM_NEWLINE u8"\n"
#endif
#ifdef __ANDROID__
    #include <jni.h>
#endif // __ANDROID__

#ifndef MAX_PATH
    #define MAX_PATH 260
#endif

static_assert(
    std::endian::native == std::endian::little, "OpenRCT2 is known to be broken on big endian. Proceed with caution!");

enum class SpecialFolder
{
    userCache,
    userConfig,
    userData,
    userHome,

    rct2Discord,
};

enum class RCT2Variant : uint8_t
{
    rct2Original,
    rctClassicWindows,
    rctClassicMac,
    rctClassicPlusMac,
};

struct RealWorldDate;
struct RealWorldTime;
struct TTFFontDescriptor;

namespace OpenRCT2::Platform
{
    constexpr u8string_view kRCTClassicWindowsDataFolder = u8"Assets";
    // clang-format off
    constexpr u8string_view kRCTClassicMacOSDataFolder =
        u8"RCT Classic.app" PATH_SEPARATOR "Contents" PATH_SEPARATOR "Resources";
    constexpr u8string_view kRCTClassicPlusMacOSDataFolder =
        u8"RCT Classic+.app" PATH_SEPARATOR "Contents" PATH_SEPARATOR "Resources";
    // clang-format on

    std::string GetEnvironmentVariable(std::string_view name);
    std::string GetFolderPath(SpecialFolder folder);
    std::string GetInstallPath();
    std::string GetDocsPath();
    std::string GetCurrentExecutablePath();
    std::string GetCurrentExecutableDirectory();
    bool ShouldIgnoreCase();
    bool IsPathSeparator(char c);
    uint64_t GetLastModified(std::string_view path);
    uint64_t GetFileSize(std::string_view path);
    std::string ResolveCasing(std::string_view path, bool fileExists);
    std::string SanitiseFilename(std::string_view originalName);
    bool IsFilenameValid(u8string_view fileName);

    uint16_t GetLocaleLanguage();
    CurrencyType GetLocaleCurrency();
    CurrencyType GetCurrencyValue(const char* currCode);
    MeasurementFormat GetLocaleMeasurementFormat();
    uint8_t GetLocaleDateFormat();
    TemperatureUnit GetLocaleTemperatureFormat();
    RealWorldTime GetTimeLocal();
    RealWorldDate GetDateLocal();

    bool FindApp(std::string_view app, std::string* output);
    int32_t Execute(std::string_view command, std::string* output = nullptr);
    bool ProcessIsElevated();
    float GetDefaultScale();

    std::optional<RCT2Variant> classifyGamePath(std::string_view path);
    bool OriginalGameDataExists(std::string_view path);

    void AdvanceTicks();

    std::string GetUsername();

    std::string GetSteamPath();
#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__)) || defined(__FreeBSD__) || defined(__NetBSD__)
    std::string GetEnvironmentPath(const char* name);
    std::string GetHomePath();
#endif
#ifndef DISABLE_TTF
    std::string GetFontPath(const TTFFontDescriptor& font);
#endif // DISABLE_TTF

    std::string FormatShortDate(std::time_t timestamp);
    std::string FormatTime(std::time_t timestamp);

#ifdef _WIN32
    bool IsOSVersionAtLeast(uint32_t major, uint32_t minor, uint32_t build);
    void SetUpFileAssociations();
    bool SetUpFileAssociation(
        std::string_view extension, std::string_view fileTypeText, std::string_view commandText, std::string_view commandArgs,
        const uint32_t iconIndex);
    void RemoveFileAssociations();
    bool SetupUriProtocol();
#endif
#ifdef __ANDROID__
    jclass AndroidFindClass(JNIEnv* env, std::string_view name);
#endif

    bool IsRunningInWine();
    bool IsColourTerminalSupported();
    bool HandleSpecialCommandLineArgument(const char* argument);
    u8string StrDecompToPrecomp(u8string_view input);
    bool RequireNewWindow(bool openGL);

    // Returns the bitmask of the GetLogicalDrives function for windows, 0 for other systems
    int32_t GetDrives();
    time_t FileGetModifiedTime(u8string_view path);

    bool LockSingleInstance();

    u8string GetRCT1SteamDir();
    u8string GetRCT2SteamDir();
    u8string GetRCTClassicSteamDir();
    datetime64 GetDatetimeNowUTC();
    uint32_t GetTicks();

    void Sleep(uint32_t ms);

    bool SSE41Available();
    bool AVX2Available();

    std::vector<std::string_view> GetSearchablePathsRCT1();
    std::vector<std::string_view> GetSearchablePathsRCT2();
} // namespace OpenRCT2::Platform

#ifdef __ANDROID__
class AndroidClassLoader
{
public:
    AndroidClassLoader();
    ~AndroidClassLoader();
    static jobject _classLoader;
    static jmethodID _findClassMethod;
};

#endif // __ANDROID__
