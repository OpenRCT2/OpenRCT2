/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef _WIN32

// Windows.h needs to be included first
#    include <windows.h>
// Then the rest
#    include "../Version.h"

#    include <datetimeapi.h>
#    include <lmcons.h>
#    include <memory>
#    include <shlobj.h>
#    include <wbemcli.h>
#    undef GetEnvironmentVariable

#    include "../OpenRCT2.h"
#    include "../common.h"
#    include "../core/Path.hpp"
#    include "../core/String.hpp"
#    include "../localisation/Date.h"
#    include "../localisation/Language.h"
#    include "Platform.h"

#    include <cstring>
#    include <iterator>
#    include <locale>

// Native resource IDs
#    include "../../../resources/resource.h"

// Enable visual styles
#    pragma comment(                                                                                                           \
        linker,                                                                                                                \
        "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

static uint32_t _frequency = 0;
static LARGE_INTEGER _entryTimestamp;

// The name of the mutex used to prevent multiple instances of the game from running
static constexpr char SINGLE_INSTANCE_MUTEX_NAME[] = "RollerCoaster Tycoon 2_GSKMUTEX";

#    define SOFTWARE_CLASSES L"Software\\Classes"
#    define MUI_CACHE L"Local Settings\\Software\\Microsoft\\Windows\\Shell\\MuiCache"

char* strndup(const char* src, size_t size)
{
    size_t len = strnlen(src, size);
    char* dst = reinterpret_cast<char*>(malloc(len + 1));

    if (dst == nullptr)
    {
        return nullptr;
    }

    dst = reinterpret_cast<char*>(std::memcpy(dst, src, len));
    dst[len] = '\0';
    return dst;
}

namespace Platform
{
    static std::string WIN32_GetKnownFolderPath(REFKNOWNFOLDERID rfid);
    static std::string WIN32_GetModuleFileNameW(HMODULE hModule);

    std::string GetEnvironmentVariable(std::string_view name)
    {
        std::wstring result;
        auto wname = String::ToWideChar(name);
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
        return String::ToUtf8(result);
    }

    typedef LONG NTSTATUS, *PNTSTATUS;
#    define STATUS_SUCCESS (0x00000000)

    RTL_OSVERSIONINFOW GetRealOSVersion()
    {
        using RtlGetVersionPtr = long(WINAPI*)(PRTL_OSVERSIONINFOW);
        HMODULE hMod = ::GetModuleHandleW(L"ntdll.dll");
        if (hMod)
        {
            RtlGetVersionPtr fxPtr = reinterpret_cast<RtlGetVersionPtr>(GetProcAddress(hMod, "RtlGetVersion"));
            if (fxPtr != nullptr)
            {
                RTL_OSVERSIONINFOW rovi = { 0 };
                rovi.dwOSVersionInfoSize = sizeof(rovi);
                if (STATUS_SUCCESS == fxPtr(&rovi))
                {
                    return rovi;
                }
            }
        }
        RTL_OSVERSIONINFOW rovi = { 0 };
        return rovi;
    }

    std::string GetOsName()
    {
        std::string output = "";
        SYSTEM_INFO si;
        GetNativeSystemInfo(&si);

        auto VN = GetRealOSVersion();

        if (VN.dwMajorVersion == 10 && VN.dwBuildNumber >= 22000)
        {
            output += "Windows 11";
        }
        else if (VN.dwMajorVersion == 10 && VN.dwBuildNumber < 22000)
        {
            output += "Windows 10";
        }
        else if (VN.dwMajorVersion == 6 && VN.dwMinorVersion == 3)
        {
            output += "Windows 8.1";
        }
        else if (VN.dwMajorVersion == 6 && VN.dwMinorVersion == 2)
        {
            output += "Windows 8";
        }
        else if (VN.dwMajorVersion == 6 && VN.dwMinorVersion == 1)
        {
            output += "Windows 7";
        }
        else if (VN.dwMajorVersion == 6 && VN.dwMinorVersion == 0)
        {
            output += "Windows Vista";
        }
        else if (VN.dwMajorVersion == 5 && VN.dwMinorVersion >= 1)
        {
            output += "Windows XP";
        }
        else if (VN.dwMajorVersion == 5 && VN.dwMinorVersion == 0)
        {
            output += "Windows 2000";
        }
        else
        {
            output += "Windows";
        }

        if (si.wProcessorArchitecture == 6 || si.wProcessorArchitecture == 9)
        {
            output += ", 64-bit";
        }
        else if (si.wProcessorArchitecture == 0)
        {
            output += ", 32-bit";
        }
        else
        {
            output += ", Unknown";
        }

        return output;
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
            result = String::ToUtf8(std::wstring_view(date, charsWritten - 1));
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
            result = String::ToUtf8(std::wstring_view(time, charsWritten - 1));
        }
        return result;
    }

    bool IsOSVersionAtLeast(uint32_t major, uint32_t minor, uint32_t build)
    {
        bool result = false;
        auto hModule = GetModuleHandleA("ntdll.dll");
        if (hModule != nullptr)
        {
            using RtlGetVersionPtr = long(WINAPI*)(PRTL_OSVERSIONINFOW);
#    if defined(__GNUC__) && __GNUC__ >= 8
#        pragma GCC diagnostic push
#        pragma GCC diagnostic ignored "-Wcast-function-type"
#    endif
            auto fn = reinterpret_cast<RtlGetVersionPtr>(GetProcAddress(hModule, "RtlGetVersion"));
#    if defined(__GNUC__) && __GNUC__ >= 8
#        pragma GCC diagnostic pop
#    endif
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
            path = String::ToUtf8(wpath);
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
        return String::ToUtf8(wExePath.get());
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

    static std::wstring get_progIdName(std::string_view extension)
    {
        auto progIdName = std::string(OPENRCT2_NAME) + std::string(extension);
        auto progIdNameW = String::ToWideChar(progIdName);
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

        auto extensionW = String::ToWideChar(extension);
        auto fileTypeTextW = String::ToWideChar(fileTypeText);
        auto commandTextW = String::ToWideChar(commandText);
        auto commandArgsW = String::ToWideChar(commandArgs);
        auto progIdNameW = get_progIdName(extension);

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
            RegDeleteTreeA(hRootKey, extension);

            // [hRootKey\OpenRCT2.ext]
            auto progIdName = get_progIdName(extension);
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
        log_warning("FindApp() not implemented for Windows!");
        return false;
    }

    int32_t Execute(std::string_view command, std::string* output)
    {
        log_warning("Execute() not implemented for Windows!");
        return -1;
    }

    uint64_t GetLastModified(std::string_view path)
    {
        uint64_t lastModified = 0;
        auto pathW = String::ToWideChar(path);
        auto hFile = CreateFileW(pathW.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
        if (hFile != INVALID_HANDLE_VALUE)
        {
            FILETIME ftCreate, ftAccess, ftWrite;
            if (GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
            {
                lastModified = (static_cast<uint64_t>(ftWrite.dwHighDateTime) << 32ULL)
                    | static_cast<uint64_t>(ftWrite.dwLowDateTime);
            }
            CloseHandle(hFile);
        }
        return lastModified;
    }

    uint64_t GetFileSize(std::string_view path)
    {
        uint64_t size = 0;
        auto pathW = String::ToWideChar(path);
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
            result = String::ToUtf8(usernameW);
        }
        return result;
    }

    uint16_t GetLocaleLanguage()
    {
        CHAR langCode[4];

        if (GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SABBREVLANGNAME, reinterpret_cast<LPSTR>(&langCode), sizeof(langCode))
            == 0)
        {
            return LANGUAGE_UNDEFINED;
        }

        if (strcmp(langCode, "ENG") == 0)
        {
            return LANGUAGE_ENGLISH_UK;
        }
        if (strcmp(langCode, "ENU") == 0)
        {
            return LANGUAGE_ENGLISH_US;
        }
        if (strcmp(langCode, "DEU") == 0)
        {
            return LANGUAGE_GERMAN;
        }
        if (strcmp(langCode, "NLD") == 0)
        {
            return LANGUAGE_DUTCH;
        }
        if (strcmp(langCode, "FRA") == 0)
        {
            return LANGUAGE_FRENCH;
        }
        if (strcmp(langCode, "HUN") == 0)
        {
            return LANGUAGE_HUNGARIAN;
        }
        if (strcmp(langCode, "PLK") == 0)
        {
            return LANGUAGE_POLISH;
        }
        if (strcmp(langCode, "ESP") == 0)
        {
            return LANGUAGE_SPANISH;
        }
        if (strcmp(langCode, "SVE") == 0)
        {
            return LANGUAGE_SWEDISH;
        }
        if (strcmp(langCode, "ITA") == 0)
        {
            return LANGUAGE_ITALIAN;
        }
        if (strcmp(langCode, "POR") == 0)
        {
            return LANGUAGE_PORTUGUESE_BR;
        }
        if (strcmp(langCode, "FIN") == 0)
        {
            return LANGUAGE_FINNISH;
        }
        if (strcmp(langCode, "NOR") == 0)
        {
            return LANGUAGE_NORWEGIAN;
        }
        if (strcmp(langCode, "DAN") == 0)
        {
            return LANGUAGE_DANISH;
        }
        return LANGUAGE_UNDEFINED;
    }

    CurrencyType GetLocaleCurrency()
    {
        CHAR currCode[4];
        if (GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SINTLSYMBOL, reinterpret_cast<LPSTR>(&currCode), sizeof(currCode)) == 0)
        {
            return Platform::GetCurrencyValue(nullptr);
        }

        return Platform::GetCurrencyValue(currCode);
    }

    MeasurementFormat GetLocaleMeasurementFormat()
    {
        UINT measurement_system;
        if (GetLocaleInfo(
                LOCALE_USER_DEFAULT, LOCALE_IMEASURE | LOCALE_RETURN_NUMBER, reinterpret_cast<LPSTR>(&measurement_system),
                sizeof(measurement_system))
            == 0)
        {
            return MeasurementFormat::Metric;
        }

        switch (measurement_system)
        {
            case 1:
                return MeasurementFormat::Imperial;
            case 0:
            default:
                return MeasurementFormat::Metric;
        }
    }

    uint8_t GetLocaleDateFormat()
    {
        // Retrieve short date format, eg "MM/dd/yyyy"
        wchar_t dateFormat[20];
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
        wchar_t first[sizeof(dateFormat)];
        wchar_t second[sizeof(dateFormat)];
        if (swscanf_s(
                dateFormat, L"%l[dyM]%*l[^dyM]%l[dyM]%*l[^dyM]%*l[dyM]", first, static_cast<uint32_t>(std::size(first)), second,
                static_cast<uint32_t>(std::size(second)))
            != 2)
        {
            return DATE_FORMAT_DAY_MONTH_YEAR;
        }

        if (wcsncmp(L"d", first, 1) == 0)
        {
            return DATE_FORMAT_DAY_MONTH_YEAR;
        }
        if (wcsncmp(L"M", first, 1) == 0)
        {
            return DATE_FORMAT_MONTH_DAY_YEAR;
        }
        if (wcsncmp(L"y", first, 1) == 0)
        {
            if (wcsncmp(L"d", second, 1) == 0)
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

        // GetLocaleInfo will set fahrenheit to 1 if the locale on this computer
        // uses the United States measurement system or 0 otherwise.
        if (GetLocaleInfo(
                LOCALE_USER_DEFAULT, LOCALE_IMEASURE | LOCALE_RETURN_NUMBER, reinterpret_cast<LPSTR>(&fahrenheit),
                sizeof(fahrenheit))
            == 0)
        {
            // Assume celsius by default if function call fails
            return TemperatureUnit::Celsius;
        }

        if (fahrenheit)
            return TemperatureUnit::Fahrenheit;

        return TemperatureUnit::Celsius;
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
            auto utf8SteamPath = String::ToUtf8(wSteamPath);
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

    bool EnsureDirectoryExists(u8string_view path)
    {
        if (Path::DirectoryExists(path))
            return 1;

        auto wPath = String::ToWideChar(path);
        auto success = CreateDirectoryW(wPath.c_str(), nullptr);
        return success != FALSE;
    }

    bool LockSingleInstance()
    {
        // Check if operating system mutex exists
        HANDLE mutex = CreateMutex(nullptr, FALSE, SINGLE_INSTANCE_MUTEX_NAME);
        if (mutex == nullptr)
        {
            log_error("unable to create mutex");
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
        auto wPath = String::ToWideChar(path);
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
                return ull.QuadPart / 10000000ULL - 11644473600ULL;
            }
        }
        return 0;
    }

    datetime64 GetDatetimeNowUTC()
    {
        // Get file time
        FILETIME fileTime;
        GetSystemTimeAsFileTime(&fileTime);
        uint64_t fileTime64 = (static_cast<uint64_t>(fileTime.dwHighDateTime) << 32ULL)
            | (static_cast<uint64_t>(fileTime.dwLowDateTime));

        // File time starts from: 1601-01-01T00:00:00Z
        // Convert to start from: 0001-01-01T00:00:00Z
        datetime64 utcNow = fileTime64 - 504911232000000000ULL;
        return utcNow;
    }

    bool SetupUriProtocol()
    {
        log_verbose("Setting up URI protocol...");

        // [HKEY_CURRENT_USER\Software\Classes]
        HKEY hRootKey;
        if (RegOpenKeyW(HKEY_CURRENT_USER, SOFTWARE_CLASSES, &hRootKey) == ERROR_SUCCESS)
        {
            // [hRootKey\openrct2]
            HKEY hClassKey;
            if (RegCreateKeyA(hRootKey, "openrct2", &hClassKey) == ERROR_SUCCESS)
            {
                if (RegSetValueA(hClassKey, nullptr, REG_SZ, "URL:openrct2", 0) == ERROR_SUCCESS)
                {
                    if (RegSetKeyValueA(hClassKey, nullptr, "URL Protocol", REG_SZ, "", 0) == ERROR_SUCCESS)
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

                            log_verbose("URI protocol setup successful");
                            return true;
                        }
                    }
                }
            }
        }

        log_verbose("URI protocol setup failed");
        return false;
    }

    uint32_t GetTicks()
    {
        LARGE_INTEGER pfc;
        QueryPerformanceCounter(&pfc);

        LARGE_INTEGER runningDelta;
        runningDelta.QuadPart = pfc.QuadPart - _entryTimestamp.QuadPart;

        return static_cast<uint32_t>(runningDelta.QuadPart / _frequency);
    }

    void Sleep(uint32_t ms)
    {
        ::Sleep(ms);
    }

    void InitTicks()
    {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        _frequency = static_cast<uint32_t>(freq.QuadPart / 1000);
        QueryPerformanceCounter(&_entryTimestamp);
    }
} // namespace Platform

#endif
