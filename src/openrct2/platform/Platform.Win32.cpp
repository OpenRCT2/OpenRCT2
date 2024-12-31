/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef _WIN32

    // Windows.h needs to be included first
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include "../Diagnostic.h"

    #include <cassert>
    #include <windows.h>

    // Then the rest
    #include "../Version.h"

    #include <datetimeapi.h>
    #include <lmcons.h>
    #include <memory>
    #include <shlobj.h>
    #undef GetEnvironmentVariable

    #include "../Date.h"
    #include "../OpenRCT2.h"
    #include "../core/Path.hpp"
    #include "../core/String.hpp"
    #include "../localisation/Language.h"
    #include "../localisation/Localisation.Date.h"
    #include "Platform.h"

    #include <cstring>
    #include <iterator>
    #include <locale>

    // Native resource IDs
    #include "../../../resources/resource.h"

    // Enable visual styles
    #pragma comment(                                                                                                           \
        linker,                                                                                                                \
        "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
// The name of the mutex used to prevent multiple instances of the game from running
static constexpr wchar_t SINGLE_INSTANCE_MUTEX_NAME[] = L"RollerCoaster Tycoon 2_GSKMUTEX";

    #define SOFTWARE_CLASSES L"Software\\Classes"
    #define MUI_CACHE L"Local Settings\\Software\\Microsoft\\Windows\\Shell\\MuiCache"

namespace OpenRCT2::Platform
{
    static std::string WIN32_GetKnownFolderPath(REFKNOWNFOLDERID rfid);
    static std::string WIN32_GetModuleFileNameW(HMODULE hModule);

    std::string GetEnvironmentVariable(std::string_view name)
    {
        std::wstring result;
        auto wname = String::toWideChar(name);
        wchar_t wvalue[256];
        auto valueSize = GetEnvironmentVariableW(wname.c_str(), wvalue, static_cast<DWORD>(std::size(wvalue)));
        if (valueSize < std::size(wvalue))
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
        return String::toUtf8(result);
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

    std::string GetFolderPath(SPECIAL_FOLDER folder)
    {
        switch (folder)
        {
            // We currently store everything under Documents/OpenRCT2
            case SPECIAL_FOLDER::USER_CACHE:
            case SPECIAL_FOLDER::USER_CONFIG:
            case SPECIAL_FOLDER::USER_DATA:
            {
                auto path = WIN32_GetKnownFolderPath(FOLDERID_Documents);
                if (path.empty())
                {
                    path = GetFolderPath(SPECIAL_FOLDER::USER_HOME);
                }
                return path;
            }
            case SPECIAL_FOLDER::USER_HOME:
            {
                auto path = WIN32_GetKnownFolderPath(FOLDERID_Profile);
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
            case SPECIAL_FOLDER::RCT2_DISCORD:
            {
                auto path = WIN32_GetKnownFolderPath(FOLDERID_LocalAppData);
                if (!path.empty())
                {
                    path = Path::Combine(path, u8"DiscordGames\\RollerCoaster Tycoon 2 Triple Thrill Pack\\content\\Game");
                }
                return path;
            }
            default:
                return std::string();
        }
    }

    std::string GetCurrentExecutableDirectory()
    {
        auto exePath = GetCurrentExecutablePath();
        auto exeDirectory = Path::GetDirectory(exePath);
        return exeDirectory;
    }

    std::string GetInstallPath()
    {
        auto path = std::string(gCustomOpenRCT2DataPath);
        if (!path.empty())
        {
            path = Path::GetAbsolute(path);
        }
        else
        {
            auto exeDirectory = GetCurrentExecutableDirectory();
            path = Path::Combine(exeDirectory, u8"data");
        }
        return path;
    }

    std::string GetCurrentExecutablePath()
    {
        return WIN32_GetModuleFileNameW(nullptr);
    }

    std::string GetDocsPath()
    {
        return GetCurrentExecutableDirectory();
    }

    static SYSTEMTIME TimeToSystemTime(std::time_t timestamp)
    {
        ULARGE_INTEGER time_value;
        time_value.QuadPart = (timestamp * 10000000LL) + 116444736000000000LL;

        FILETIME ft;
        ft.dwLowDateTime = time_value.LowPart;
        ft.dwHighDateTime = time_value.HighPart;

        SYSTEMTIME st;
        FileTimeToSystemTime(&ft, &st);
        return st;
    }

    std::string FormatShortDate(std::time_t timestamp)
    {
        SYSTEMTIME st = TimeToSystemTime(timestamp);
        std::string result;

        wchar_t date[20];
        ptrdiff_t charsWritten = GetDateFormatEx(
            LOCALE_NAME_USER_DEFAULT, DATE_SHORTDATE, &st, nullptr, date, static_cast<int>(std::size(date)), nullptr);
        if (charsWritten != 0)
        {
            result = String::toUtf8(std::wstring_view(date, charsWritten - 1));
        }
        return result;
    }

    std::string FormatTime(std::time_t timestamp)
    {
        SYSTEMTIME st = TimeToSystemTime(timestamp);
        std::string result;

        wchar_t time[20];
        ptrdiff_t charsWritten = GetTimeFormatEx(
            LOCALE_NAME_USER_DEFAULT, 0, &st, nullptr, time, static_cast<int>(std::size(time)));
        if (charsWritten != 0)
        {
            result = String::toUtf8(std::wstring_view(time, charsWritten - 1));
        }
        return result;
    }

    bool IsOSVersionAtLeast(uint32_t major, uint32_t minor, uint32_t build)
    {
        bool result = false;
        auto hModule = GetModuleHandleW(L"ntdll.dll");
        if (hModule != nullptr)
        {
            using RtlGetVersionPtr = long(WINAPI*)(PRTL_OSVERSIONINFOW);
    #if defined(__GNUC__) && __GNUC__ >= 8
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wcast-function-type"
    #endif
            auto fn = reinterpret_cast<RtlGetVersionPtr>(GetProcAddress(hModule, "RtlGetVersion"));
    #if defined(__GNUC__) && __GNUC__ >= 8
        #pragma GCC diagnostic pop
    #endif
            if (fn != nullptr)
            {
                RTL_OSVERSIONINFOW rovi{};
                rovi.dwOSVersionInfoSize = sizeof(rovi);
                if (fn(&rovi) == 0)
                {
                    if (rovi.dwMajorVersion > major
                        || (rovi.dwMajorVersion == major
                            && (rovi.dwMinorVersion > minor || (rovi.dwMinorVersion == minor && rovi.dwBuildNumber >= build))))
                    {
                        result = true;
                    }
                }
            }
        }
        return result;
    }

    bool IsRunningInWine()
    {
        HMODULE ntdllMod = GetModuleHandleW(L"ntdll.dll");

        if (ntdllMod && GetProcAddress(ntdllMod, "wine_get_version"))
        {
            return true;
        }
        return false;
    }

    /**
     * Checks if the current version of Windows supports ANSI colour codes.
     * From Windows 10, build 10586 ANSI escape colour codes can be used on stdout.
     */
    static bool HasANSIColourSupport()
    {
        return IsOSVersionAtLeast(10, 0, 10586);
    }

    static void EnableANSIConsole()
    {
        if (HasANSIColourSupport())
        {
            auto handle = GetStdHandle(STD_OUTPUT_HANDLE);
            DWORD mode;
            GetConsoleMode(handle, &mode);
            if (!(mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING))
            {
                mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                SetConsoleMode(handle, mode);
            }
        }
    }

    bool IsColourTerminalSupported()
    {
        static bool hasChecked = false;
        static bool isSupported = false;
        if (!hasChecked)
        {
            if (HasANSIColourSupport())
            {
                EnableANSIConsole();
                isSupported = true;
            }
            else
            {
                isSupported = false;
            }
            hasChecked = true;
        }
        return isSupported;
    }

    static std::string WIN32_GetKnownFolderPath(REFKNOWNFOLDERID rfid)
    {
        std::string path;
        wchar_t* wpath = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(rfid, KF_FLAG_CREATE, nullptr, &wpath)))
        {
            path = String::toUtf8(wpath);
        }
        CoTaskMemFree(wpath);
        return path;
    }

    static std::string WIN32_GetModuleFileNameW(HMODULE hModule)
    {
        uint32_t wExePathCapacity = MAX_PATH;
        std::unique_ptr<wchar_t[]> wExePath;
        uint32_t size;
        do
        {
            wExePathCapacity *= 2;
            wExePath = std::make_unique<wchar_t[]>(wExePathCapacity);
            size = GetModuleFileNameW(hModule, wExePath.get(), wExePathCapacity);
        } while (size >= wExePathCapacity);
        return String::toUtf8(wExePath.get());
    }

    u8string StrDecompToPrecomp(u8string_view input)
    {
        return u8string(input);
    }

    void SetUpFileAssociations()
    {
        // Setup file extensions
        SetUpFileAssociation(".park", "OpenRCT2 park (.park)", "Play", "\"%1\"", 0);
        SetUpFileAssociation(".sc4", "RCT1 Scenario (.sc4)", "Play", "\"%1\"", 0);
        SetUpFileAssociation(".sc6", "RCT2 Scenario (.sc6)", "Play", "\"%1\"", 0);
        SetUpFileAssociation(".sv4", "RCT1 Saved Game (.sc4)", "Play", "\"%1\"", 0);
        SetUpFileAssociation(".sv6", "RCT2 Saved Game (.sv6)", "Play", "\"%1\"", 0);
        SetUpFileAssociation(".sv7", "RCT Modified Saved Game (.sv7)", "Play", "\"%1\"", 0);
        SetUpFileAssociation(".sea", "RCTC Saved Game (.sea)", "Play", "\"%1\"", 0);
        SetUpFileAssociation(".td4", "RCT1 Track Design (.td4)", "Install", "\"%1\"", 0);
        SetUpFileAssociation(".td6", "RCT2 Track Design (.td6)", "Install", "\"%1\"", 0);

        // Refresh explorer
        SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    }

    static HMODULE _dllModule = nullptr;
    static HMODULE GetDLLModule()
    {
        if (_dllModule == nullptr)
        {
            _dllModule = GetModuleHandle(nullptr);
        }
        return _dllModule;
    }

    static std::wstring GetProdIDName(std::string_view extension)
    {
        auto progIdName = std::string(OPENRCT2_NAME) + std::string(extension);
        auto progIdNameW = String::toWideChar(progIdName);
        return progIdNameW;
    }

    bool SetUpFileAssociation(
        std::string_view extension, std::string_view fileTypeText, std::string_view commandText, std::string_view commandArgs,
        const uint32_t iconIndex)
    {
        wchar_t exePathW[MAX_PATH];
        wchar_t dllPathW[MAX_PATH];

        [[maybe_unused]] int32_t printResult;

        GetModuleFileNameW(nullptr, exePathW, static_cast<DWORD>(std::size(exePathW)));
        GetModuleFileNameW(GetDLLModule(), dllPathW, static_cast<DWORD>(std::size(dllPathW)));

        auto extensionW = String::toWideChar(extension);
        auto fileTypeTextW = String::toWideChar(fileTypeText);
        auto commandTextW = String::toWideChar(commandText);
        auto commandArgsW = String::toWideChar(commandArgs);
        auto progIdNameW = GetProdIDName(extension);

        HKEY hKey = nullptr;
        HKEY hRootKey = nullptr;

        // [HKEY_CURRENT_USER\Software\Classes]
        if (RegOpenKeyW(HKEY_CURRENT_USER, SOFTWARE_CLASSES, &hRootKey) != ERROR_SUCCESS)
        {
            RegCloseKey(hRootKey);
            return false;
        }

        // [hRootKey\.ext]
        if (RegSetValueW(hRootKey, extensionW.c_str(), REG_SZ, progIdNameW.c_str(), 0) != ERROR_SUCCESS)
        {
            RegCloseKey(hRootKey);
            return false;
        }

        if (RegCreateKeyW(hRootKey, progIdNameW.c_str(), &hKey) != ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            RegCloseKey(hRootKey);
            return false;
        }

        // [hRootKey\OpenRCT2.ext]
        if (RegSetValueW(hKey, nullptr, REG_SZ, fileTypeTextW.c_str(), 0) != ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            RegCloseKey(hRootKey);
            return false;
        }
        // [hRootKey\OpenRCT2.ext\DefaultIcon]
        wchar_t szIconW[MAX_PATH];
        printResult = swprintf_s(szIconW, MAX_PATH, L"\"%s\",%d", dllPathW, iconIndex);
        assert(printResult >= 0);
        if (RegSetValueW(hKey, L"DefaultIcon", REG_SZ, szIconW, 0) != ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            RegCloseKey(hRootKey);
            return false;
        }

        // [hRootKey\OpenRCT2.sv6\shell]
        if (RegSetValueW(hKey, L"shell", REG_SZ, L"open", 0) != ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            RegCloseKey(hRootKey);
            return false;
        }

        // [hRootKey\OpenRCT2.sv6\shell\open]
        if (RegSetValueW(hKey, L"shell\\open", REG_SZ, commandTextW.c_str(), 0) != ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            RegCloseKey(hRootKey);
            return false;
        }

        // [hRootKey\OpenRCT2.sv6\shell\open\command]
        wchar_t szCommandW[MAX_PATH];
        printResult = swprintf_s(szCommandW, MAX_PATH, L"\"%s\" %s", exePathW, commandArgsW.c_str());
        assert(printResult >= 0);
        if (RegSetValueW(hKey, L"shell\\open\\command", REG_SZ, szCommandW, 0) != ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            RegCloseKey(hRootKey);
            return false;
        }
        return true;
    }

    static void RemoveFileAssociation(const utf8* extension)
    {
        // [HKEY_CURRENT_USER\Software\Classes]
        HKEY hRootKey;
        if (RegOpenKeyW(HKEY_CURRENT_USER, SOFTWARE_CLASSES, &hRootKey) == ERROR_SUCCESS)
        {
            // [hRootKey\.ext]
            RegDeleteTreeW(hRootKey, String::toWideChar(extension).c_str());

            // [hRootKey\OpenRCT2.ext]
            auto progIdName = GetProdIDName(extension);
            RegDeleteTreeW(hRootKey, progIdName.c_str());

            RegCloseKey(hRootKey);
        }
    }

    void RemoveFileAssociations()
    {
        // Remove file extensions
        RemoveFileAssociation(".park");
        RemoveFileAssociation(".sc4");
        RemoveFileAssociation(".sc6");
        RemoveFileAssociation(".sv4");
        RemoveFileAssociation(".sv6");
        RemoveFileAssociation(".sv7");
        RemoveFileAssociation(".sea");
        RemoveFileAssociation(".td4");
        RemoveFileAssociation(".td6");

        // Refresh explorer
        SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    }

    bool HandleSpecialCommandLineArgument(const char* argument)
    {
        return false;
    }

    bool FindApp(std::string_view app, std::string* output)
    {
        LOG_WARNING("FindApp() not implemented for Windows!");
        return false;
    }

    int32_t Execute(std::string_view command, std::string* output)
    {
        LOG_WARNING("Execute() not implemented for Windows!");
        return -1;
    }

    uint64_t GetLastModified(std::string_view path)
    {
        uint64_t lastModified = 0;
        auto pathW = String::toWideChar(path);
        auto hFile = CreateFileW(pathW.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
        if (hFile != INVALID_HANDLE_VALUE)
        {
            FILETIME ftCreate, ftAccess, ftWrite;
            if (GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
            {
                lastModified = (static_cast<uint64_t>(ftWrite.dwHighDateTime) << 32uLL)
                    | static_cast<uint64_t>(ftWrite.dwLowDateTime);
            }
            CloseHandle(hFile);
        }
        return lastModified;
    }

    uint64_t GetFileSize(std::string_view path)
    {
        uint64_t size = 0;
        auto pathW = String::toWideChar(path);
        WIN32_FILE_ATTRIBUTE_DATA attributes;
        if (GetFileAttributesExW(pathW.c_str(), GetFileExInfoStandard, &attributes) != FALSE)
        {
            ULARGE_INTEGER fileSize;
            fileSize.LowPart = attributes.nFileSizeLow;
            fileSize.HighPart = attributes.nFileSizeHigh;
            size = fileSize.QuadPart;
        }
        return size;
    }

    bool ShouldIgnoreCase()
    {
        return true;
    }

    bool IsPathSeparator(char c)
    {
        return c == '\\' || c == '/';
    }

    std::string ResolveCasing(std::string_view path, bool fileExists)
    {
        std::string result;
        if (fileExists)
        {
            // Windows is case insensitive so it will exist and that is all that matters
            // for now. We can properly resolve the casing if we ever need to.
            result = std::string(path);
        }
        return result;
    }

    bool RequireNewWindow(bool openGL)
    {
        // Windows is apparently able to switch to hardware rendering on the fly although
        // using the same window in an unaccelerated and accelerated context is unsupported by SDL2
        return openGL;
    }

    std::string GetUsername()
    {
        std::string result;
        wchar_t usernameW[UNLEN + 1]{};
        DWORD usernameLength = UNLEN + 1;
        if (GetUserNameW(usernameW, &usernameLength))
        {
            result = String::toUtf8(usernameW);
        }
        return result;
    }

    uint16_t GetLocaleLanguage()
    {
        wchar_t langCode[LOCALE_NAME_MAX_LENGTH];
        if (GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SNAME, langCode, static_cast<int>(std::size(langCode))) == 0)
        {
            return LANGUAGE_UNDEFINED;
        }

        const std::pair<std::wstring_view, int16_t> supportedLocales[] = {
            { L"ar", /*LANGUAGE_ARABIC*/ LANGUAGE_UNDEFINED }, // Experimental, don't risk offering it by default yet
            { L"ca", LANGUAGE_CATALAN },
            { L"zh-Hans", LANGUAGE_CHINESE_SIMPLIFIED },  // May not be accurate enough
            { L"zh-Hant", LANGUAGE_CHINESE_TRADITIONAL }, // May not be accurate enough
            { L"cs", LANGUAGE_CZECH },
            { L"da", LANGUAGE_DANISH },
            { L"de", LANGUAGE_GERMAN },
            { L"en-GB", LANGUAGE_ENGLISH_UK },
            { L"en-US", LANGUAGE_ENGLISH_US },
            { L"eo", LANGUAGE_ESPERANTO },
            { L"es", LANGUAGE_SPANISH },
            { L"fr", LANGUAGE_FRENCH },
            { L"fr-CA", LANGUAGE_FRENCH_CA },
            { L"gl", LANGUAGE_GALICIAN },
            { L"it", LANGUAGE_ITALIAN },
            { L"ja", LANGUAGE_JAPANESE },
            { L"ko", LANGUAGE_KOREAN },
            { L"hu", LANGUAGE_HUNGARIAN },
            { L"nl", LANGUAGE_DUTCH },
            { L"no", LANGUAGE_NORWEGIAN },
            { L"pl", LANGUAGE_POLISH },
            { L"pt-BR", LANGUAGE_PORTUGUESE_BR },
            { L"ru", LANGUAGE_RUSSIAN },
            { L"fi", LANGUAGE_FINNISH },
            { L"sv", LANGUAGE_SWEDISH },
            { L"tr", LANGUAGE_TURKISH },
            { L"uk", LANGUAGE_UKRAINIAN },
            { L"vi", LANGUAGE_VIETNAMESE },
        };
        static_assert(
            std::size(supportedLocales) == LANGUAGE_COUNT - 1, "GetLocaleLanguage: List of languages does not match the enum!");

        for (const auto& locale : supportedLocales)
        {
            if (wcsncmp(langCode, locale.first.data(), locale.first.length()) == 0)
            {
                return locale.second;
            }
        }
        return LANGUAGE_UNDEFINED;
    }

    CurrencyType GetLocaleCurrency()
    {
        wchar_t currCode[9];
        if (GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SINTLSYMBOL, currCode, static_cast<int>(std::size(currCode))) == 0)
        {
            return Platform::GetCurrencyValue(nullptr);
        }

        return Platform::GetCurrencyValue(String::toUtf8(currCode).c_str());
    }

    MeasurementFormat GetLocaleMeasurementFormat()
    {
        UINT measurement_system;
        if (GetLocaleInfoEx(
                LOCALE_NAME_USER_DEFAULT, LOCALE_IMEASURE | LOCALE_RETURN_NUMBER, reinterpret_cast<LPWSTR>(&measurement_system),
                sizeof(measurement_system) / sizeof(wchar_t))
            == 0)
        {
            return MeasurementFormat::Metric;
        }

        return measurement_system == 1 ? MeasurementFormat::Imperial : MeasurementFormat::Metric;
    }

    uint8_t GetLocaleDateFormat()
    {
        // Retrieve short date format, eg "MM/dd/yyyy"
        wchar_t dateFormat[80];
        if (GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SSHORTDATE, dateFormat, static_cast<int>(std::size(dateFormat)))
            == 0)
        {
            return DATE_FORMAT_DAY_MONTH_YEAR;
        }

        // The only valid characters for format types are: dgyM
        // We try to find 3 strings of format types, ignore any characters in between.
        // We also ignore 'g', as it represents 'era' and we don't have that concept
        // in our date formats.
        // https://msdn.microsoft.com/en-us/library/windows/desktop/dd317787(v=vs.85).aspx
        //
        wchar_t first[std::size(dateFormat)];
        wchar_t second[std::size(dateFormat)];
        if (swscanf_s(
                dateFormat, L"%l[dyM]%*l[^dyM]%l[dyM]%*l[^dyM]%*l[dyM]", first, static_cast<uint32_t>(std::size(first)), second,
                static_cast<uint32_t>(std::size(second)))
            != 2)
        {
            return DATE_FORMAT_DAY_MONTH_YEAR;
        }

        if (first[0] == L'd')
        {
            return DATE_FORMAT_DAY_MONTH_YEAR;
        }
        if (first[0] == L'M')
        {
            return DATE_FORMAT_MONTH_DAY_YEAR;
        }
        if (first[0] == L'y')
        {
            if (second[0] == 'd')
            {
                return DATE_FORMAT_YEAR_DAY_MONTH;
            }

            // Closest possible option
            return DATE_FORMAT_YEAR_MONTH_DAY;
        }

        // Default fallback
        return DATE_FORMAT_DAY_MONTH_YEAR;
    }

    TemperatureUnit GetLocaleTemperatureFormat()
    {
        UINT fahrenheit;

        // GetLocaleInfoEx will set fahrenheit to 1 if the locale on this computer
        // uses the United States measurement system or 0 otherwise.
        if (GetLocaleInfoEx(
                LOCALE_NAME_USER_DEFAULT, LOCALE_IMEASURE | LOCALE_RETURN_NUMBER, reinterpret_cast<LPWSTR>(&fahrenheit),
                sizeof(fahrenheit) / sizeof(wchar_t))
            == 0)
        {
            // Assume celsius by default if function call fails
            return TemperatureUnit::Celsius;
        }

        return fahrenheit == 1 ? TemperatureUnit::Fahrenheit : TemperatureUnit::Celsius;
    }

    bool ProcessIsElevated()
    {
        BOOL isElevated = FALSE;
        HANDLE hToken = nullptr;
        if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
        {
            TOKEN_ELEVATION Elevation;
            DWORD tokenSize = sizeof(TOKEN_ELEVATION);
            if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &tokenSize))
            {
                isElevated = Elevation.TokenIsElevated;
            }
        }
        if (hToken)
        {
            CloseHandle(hToken);
        }
        return isElevated;
    }

    std::string GetSteamPath()
    {
        wchar_t* wSteamPath;
        HKEY hKey;
        DWORD type, size;
        LRESULT result;

        if (RegOpenKeyW(HKEY_CURRENT_USER, L"Software\\Valve\\Steam", &hKey) != ERROR_SUCCESS)
            return {};

        // Get the size of the path first
        if (RegQueryValueExW(hKey, L"SteamPath", nullptr, &type, nullptr, &size) != ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            return {};
        }

        std::string outPath = "";
        wSteamPath = reinterpret_cast<wchar_t*>(malloc(size));
        result = RegQueryValueExW(hKey, L"SteamPath", nullptr, &type, reinterpret_cast<LPBYTE>(wSteamPath), &size);
        if (result == ERROR_SUCCESS)
        {
            auto utf8SteamPath = String::toUtf8(wSteamPath);
            outPath = Path::Combine(utf8SteamPath, u8"steamapps", u8"common");
        }
        free(wSteamPath);
        RegCloseKey(hKey);
        return outPath;
    }

    std::string GetFontPath(const TTFFontDescriptor& font)
    {
        auto path = WIN32_GetKnownFolderPath(FOLDERID_Fonts);
        return !path.empty() ? Path::Combine(path, font.filename) : std::string();
    }

    bool LockSingleInstance()
    {
        // Check if operating system mutex exists
        HANDLE mutex = CreateMutexW(nullptr, FALSE, SINGLE_INSTANCE_MUTEX_NAME);
        if (mutex == nullptr)
        {
            LOG_ERROR("unable to create mutex");
            return true;
        }
        else if (GetLastError() == ERROR_ALREADY_EXISTS)
        {
            // Already running
            CloseHandle(mutex);
            return false;
        }
        return true;
    }

    int32_t GetDrives()
    {
        return GetLogicalDrives();
    }

    u8string GetRCT1SteamDir()
    {
        return u8"Rollercoaster Tycoon Deluxe";
    }

    u8string GetRCT2SteamDir()
    {
        return u8"Rollercoaster Tycoon 2";
    }

    time_t FileGetModifiedTime(u8string_view path)
    {
        WIN32_FILE_ATTRIBUTE_DATA data{};
        auto wPath = String::toWideChar(path);
        auto result = GetFileAttributesExW(wPath.c_str(), GetFileExInfoStandard, &data);
        if (result != FALSE)
        {
            FILETIME localFileTime{};
            result = FileTimeToLocalFileTime(&data.ftLastWriteTime, &localFileTime);
            if (result != FALSE)
            {
                ULARGE_INTEGER ull{};
                ull.LowPart = localFileTime.dwLowDateTime;
                ull.HighPart = localFileTime.dwHighDateTime;
                return ull.QuadPart / 10000000uLL - 11644473600uLL;
            }
        }
        return 0;
    }

    datetime64 GetDatetimeNowUTC()
    {
        // Get file time
        FILETIME fileTime;
        GetSystemTimeAsFileTime(&fileTime);
        uint64_t fileTime64 = (static_cast<uint64_t>(fileTime.dwHighDateTime) << 32uLL)
            | (static_cast<uint64_t>(fileTime.dwLowDateTime));

        // File time starts from: 1601-01-01T00:00:00Z
        // Convert to start from: 0001-01-01T00:00:00Z
        datetime64 utcNow = fileTime64 - 504911232000000000uLL;
        return utcNow;
    }

    bool SetupUriProtocol()
    {
        LOG_VERBOSE("Setting up URI protocol...");

        // [HKEY_CURRENT_USER\Software\Classes]
        HKEY hRootKey;
        if (RegOpenKeyW(HKEY_CURRENT_USER, SOFTWARE_CLASSES, &hRootKey) == ERROR_SUCCESS)
        {
            // [hRootKey\openrct2]
            HKEY hClassKey;
            if (RegCreateKeyW(hRootKey, L"openrct2", &hClassKey) == ERROR_SUCCESS)
            {
                if (RegSetValueW(hClassKey, nullptr, REG_SZ, L"URL:openrct2", 0) == ERROR_SUCCESS)
                {
                    if (RegSetKeyValueW(hClassKey, nullptr, L"URL Protocol", REG_SZ, "", 0) == ERROR_SUCCESS)
                    {
                        // [hRootKey\openrct2\shell\open\command]
                        wchar_t exePath[MAX_PATH];
                        GetModuleFileNameW(nullptr, exePath, MAX_PATH);

                        wchar_t buffer[512];
                        swprintf_s(buffer, std::size(buffer), L"\"%s\" handle-uri \"%%1\"", exePath);
                        if (RegSetValueW(hClassKey, L"shell\\open\\command", REG_SZ, buffer, 0) == ERROR_SUCCESS)
                        {
                            // Not compulsory, but gives the application a nicer name
                            // [HKEY_CURRENT_USER\SOFTWARE\Classes\Local Settings\Software\Microsoft\Windows\Shell\MuiCache]
                            HKEY hMuiCacheKey;
                            if (RegCreateKeyW(hRootKey, MUI_CACHE, &hMuiCacheKey) == ERROR_SUCCESS)
                            {
                                swprintf_s(buffer, std::size(buffer), L"%s.FriendlyAppName", exePath);
                                // mingw-w64 used to define RegSetKeyValueW's signature incorrectly
                                // You need at least mingw-w64 5.0 including this commit:
                                //   https://sourceforge.net/p/mingw-w64/mingw-w64/ci/da9341980a4b70be3563ac09b5927539e7da21f7/
                                RegSetKeyValueW(hMuiCacheKey, nullptr, buffer, REG_SZ, L"OpenRCT2", sizeof(L"OpenRCT2"));
                            }

                            LOG_VERBOSE("URI protocol setup successful");
                            return true;
                        }
                    }
                }
            }
        }

        LOG_VERBOSE("URI protocol setup failed");
        return false;
    }

    std::vector<std::string_view> GetSearchablePathsRCT1()
    {
        return {
            R"(C:\Program Files\Steam\steamapps\common\Rollercoaster Tycoon Deluxe)",
            R"(C:\Program Files (x86)\Steam\steamapps\common\Rollercoaster Tycoon Deluxe)",
            R"(C:\GOG Games\RollerCoaster Tycoon Deluxe)",
            R"(C:\Program Files\GalaxyClient\Games\RollerCoaster Tycoon Deluxe)",
            R"(C:\Program Files (x86)\GalaxyClient\Games\RollerCoaster Tycoon Deluxe)",
            R"(C:\Program Files\Hasbro Interactive\RollerCoaster Tycoon)",
            R"(C:\Program Files (x86)\Hasbro Interactive\RollerCoaster Tycoon)",
        };
    }

    std::vector<std::string_view> GetSearchablePathsRCT2()
    {
        return {
            R"(C:\Program Files\Steam\steamapps\common\Rollercoaster Tycoon 2)",
            R"(C:\Program Files (x86)\Steam\steamapps\common\Rollercoaster Tycoon 2)",
            R"(C:\GOG Games\RollerCoaster Tycoon 2 Triple Thrill Pack)",
            R"(C:\Program Files\GalaxyClient\Games\RollerCoaster Tycoon 2 Triple Thrill Pack)",
            R"(C:\Program Files (x86)\GalaxyClient\Games\RollerCoaster Tycoon 2 Triple Thrill Pack)",
            R"(C:\Program Files\Atari\RollerCoaster Tycoon 2)",
            R"(C:\Program Files (x86)\Atari\RollerCoaster Tycoon 2)",
            R"(C:\Program Files\Infogrames\RollerCoaster Tycoon 2)",
            R"(C:\Program Files (x86)\Infogrames\RollerCoaster Tycoon 2)",
            R"(C:\Program Files\Infogrames Interactive\RollerCoaster Tycoon 2)",
            R"(C:\Program Files (x86)\Infogrames Interactive\RollerCoaster Tycoon 2)",
        };
    }

} // namespace OpenRCT2::Platform

#endif
