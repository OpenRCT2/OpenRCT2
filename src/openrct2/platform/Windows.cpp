/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#if defined(__MINGW32__) && !defined(WINVER) && !defined(_WIN32_WINNT)
// 0x0600 == vista
#    define WINVER 0x0600
#    define _WIN32_WINNT 0x0600
#endif // __MINGW32__

#ifdef _WIN32

// Windows.h needs to be included first
// clang-format off
#    include <windows.h>
#    include <shellapi.h>
// clang-format on

// Then the rest
#    include "../OpenRCT2.h"
#    include "../Version.h"
#    include "../config/Config.h"
#    include "../core/Path.hpp"
#    include "../core/String.hpp"
#    include "../localisation/Date.h"
#    include "../localisation/Language.h"
#    include "../rct2/RCT2.h"
#    include "../util/Util.h"
#    include "Platform2.h"
#    include "platform.h"

#    include <algorithm>
#    include <array>
#    include <iterator>
#    include <lmcons.h>
#    include <memory>
#    include <psapi.h>
#    include <shlobj.h>
#    include <sys/stat.h>

// Native resource IDs
#    include "../../../resources/resource.h"

// Enable visual styles
#    pragma comment(                                                                                                           \
        linker,                                                                                                                \
        "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// The name of the mutex used to prevent multiple instances of the game from running
#    define SINGLE_INSTANCE_MUTEX_NAME "RollerCoaster Tycoon 2_GSKMUTEX"

#    define OPENRCT2_DLL_MODULE_NAME "openrct2.dll"

#    if _WIN32_WINNT < 0x600
#        define swprintf_s(a, b, c, d, ...) swprintf(a, b, c, ##__VA_ARGS__)
#    endif

bool platform_ensure_directory_exists(const utf8* path)
{
    if (Path::DirectoryExists(path))
        return 1;

    auto wPath = String::ToWideChar(path);
    auto success = CreateDirectoryW(wPath.c_str(), nullptr);
    return success != FALSE;
}

bool platform_lock_single_instance()
{
    HANDLE mutex, status;

    // Check if operating system mutex exists
    mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, SINGLE_INSTANCE_MUTEX_NAME);
    if (mutex == nullptr)
    {
        // Create new mutex
        status = CreateMutex(nullptr, FALSE, SINGLE_INSTANCE_MUTEX_NAME);
        if (status == nullptr)
            log_error("unable to create mutex");

        return true;
    }

    // Already running
    CloseHandle(mutex);
    return false;
}

int32_t platform_get_drives()
{
    return GetLogicalDrives();
}

bool platform_get_steam_path(utf8* outPath, size_t outSize)
{
    wchar_t* wSteamPath;
    HKEY hKey;
    DWORD type, size;
    LRESULT result;

    if (RegOpenKeyW(HKEY_CURRENT_USER, L"Software\\Valve\\Steam", &hKey) != ERROR_SUCCESS)
        return false;

    // Get the size of the path first
    if (RegQueryValueExW(hKey, L"SteamPath", nullptr, &type, nullptr, &size) != ERROR_SUCCESS)
    {
        RegCloseKey(hKey);
        return false;
    }

    wSteamPath = reinterpret_cast<wchar_t*>(malloc(size));
    result = RegQueryValueExW(hKey, L"SteamPath", nullptr, &type, reinterpret_cast<LPBYTE>(wSteamPath), &size);
    if (result == ERROR_SUCCESS)
    {
        auto utf8SteamPath = String::ToUtf8(wSteamPath);
        safe_strcpy(outPath, utf8SteamPath.c_str(), outSize);
        safe_strcat_path(outPath, "steamapps\\common", outSize);
    }
    free(wSteamPath);
    RegCloseKey(hKey);
    return result == ERROR_SUCCESS;
}

std::string platform_get_rct1_steam_dir()
{
    return "Rollercoaster Tycoon Deluxe";
}

std::string platform_get_rct2_steam_dir()
{
    return "Rollercoaster Tycoon 2";
}

time_t platform_file_get_modified_time(const utf8* path)
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

#    ifndef NO_TTF
bool platform_get_font_path(TTFFontDescriptor* font, utf8* buffer, size_t size)
{
#        if !defined(__MINGW32__)                                                                                              \
            && ((NTDDI_VERSION >= NTDDI_VISTA) && !defined(_USING_V110_SDK71_) && !defined(_ATL_XP_TARGETING))
    wchar_t* fontFolder;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Fonts, 0, nullptr, &fontFolder)))
    {
        // Convert wchar to utf8, then copy the font folder path to the buffer.
        auto outPathTemp = String::ToUtf8(fontFolder);
        safe_strcpy(buffer, outPathTemp.c_str(), size);

        CoTaskMemFree(fontFolder);

        // Append the requested font's file name.
        safe_strcat_path(buffer, font->filename, size);
        return true;
    }

    return false;
#        else
    log_warning("Compatibility hack: falling back to C:\\Windows\\Fonts");
    safe_strcpy(buffer, "C:\\Windows\\Fonts\\", size);
    safe_strcat_path(buffer, font->filename, size);
    return true;
#        endif
}
#    endif // NO_TTF

std::string platform_get_absolute_path(const utf8* relativePath, const utf8* basePath)
{
    std::string result;
    if (relativePath != nullptr)
    {
        std::string pathToResolve;
        if (basePath == nullptr)
        {
            pathToResolve = std::string(relativePath);
        }
        else
        {
            pathToResolve = std::string(basePath) + std::string("\\") + relativePath;
        }

        auto pathToResolveW = String::ToWideChar(pathToResolve);
        wchar_t fullPathW[MAX_PATH]{};
        auto fullPathLen = GetFullPathNameW(
            pathToResolveW.c_str(), static_cast<DWORD>(std::size(fullPathW)), fullPathW, nullptr);
        if (fullPathLen != 0)
        {
            result = String::ToUtf8(fullPathW);
        }
    }
    return result;
}

datetime64 platform_get_datetime_now_utc()
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

///////////////////////////////////////////////////////////////////////////////
// URI protocol association setup
///////////////////////////////////////////////////////////////////////////////

#    define SOFTWARE_CLASSES L"Software\\Classes"
#    define MUI_CACHE L"Local Settings\\Software\\Microsoft\\Windows\\Shell\\MuiCache"

bool platform_setup_uri_protocol()
{
#    if _WIN32_WINNT >= 0x0600
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
#    endif

    log_verbose("URI protocol setup failed");
    return false;
}

///////////////////////////////////////////////////////////////////////////////
#endif
