/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef __MINGW32__
// 0x0600 == vista
#define WINVER 0x0600
#define _WIN32_WINNT 0x0600
#endif // __MINGW32__

#include "../common.h"

#ifdef _WIN32

#undef interface
#include <windows.h>
#include <lmcons.h>
#include <psapi.h>
#include <shlobj.h>
#include <sys/stat.h>

#include "../config/Config.h"
#include "../core/Util.hpp"
#include "../localisation/Date.h"
#include "../localisation/Language.h"
#include "../OpenRCT2.h"
#include "../rct2/RCT2.h"
#include "../util/Util.h"
#include "../Version.h"
#include "platform.h"

// Native resource IDs
#include "../../../resources/resource.h"

// Enable visual styles
#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// The name of the mutex used to prevent multiple instances of the game from running
#define SINGLE_INSTANCE_MUTEX_NAME "RollerCoaster Tycoon 2_GSKMUTEX"

#define OPENRCT2_DLL_MODULE_NAME "openrct2.dll"

static HMODULE _dllModule = nullptr;

static HMODULE plaform_get_dll_module()
{
    if (_dllModule == nullptr) {
        _dllModule = GetModuleHandle(NULL);
    }
    return _dllModule;
}

void platform_get_date_local(rct2_date *out_date)
{
    assert(out_date != nullptr);
    SYSTEMTIME systime;

    GetLocalTime(&systime);
    out_date->day = systime.wDay;
    out_date->month = systime.wMonth;
    out_date->year = systime.wYear;
    out_date->day_of_week = systime.wDayOfWeek;
}

void platform_get_time_local(rct2_time *out_time)
{
    assert(out_time != nullptr);
    SYSTEMTIME systime;
    GetLocalTime(&systime);
    out_time->hour = systime.wHour;
    out_time->minute = systime.wMinute;
    out_time->second = systime.wSecond;
}

bool platform_file_exists(const utf8 *path)
{
    wchar_t *wPath = utf8_to_widechar(path);
    DWORD result = GetFileAttributesW(wPath);
    DWORD error = GetLastError();
    free(wPath);
    return !(result == INVALID_FILE_ATTRIBUTES && (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND));
}

bool platform_directory_exists(const utf8 *path)
{
    wchar_t *wPath = utf8_to_widechar(path);
    DWORD dwAttrib = GetFileAttributesW(wPath);
    free(wPath);
    return dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

bool platform_original_game_data_exists(const utf8 *path)
{
    utf8 checkPath[MAX_PATH];
    safe_strcpy(checkPath, path, MAX_PATH);
    safe_strcat_path(checkPath, "Data", MAX_PATH);
    safe_strcat_path(checkPath, "g1.dat", MAX_PATH);
    return platform_file_exists(checkPath);
}

bool platform_ensure_directory_exists(const utf8 *path)
{
    if (platform_directory_exists(path))
        return 1;

    wchar_t *wPath = utf8_to_widechar(path);
    BOOL success = CreateDirectoryW(wPath, NULL);
    free(wPath);
    return success == TRUE;
}

bool platform_directory_delete(const utf8 *path)
{
    wchar_t pszFrom[MAX_PATH];

    wchar_t *wPath = utf8_to_widechar(path);
    wcsncpy(pszFrom, wPath, MAX_PATH);

    // Needs to be double-null terminated for some weird reason
    pszFrom[wcslen(wPath) + 1] = 0;
    free(wPath);

    SHFILEOPSTRUCTW fileop;
    fileop.hwnd   = nullptr;    // no status display
    fileop.wFunc  = FO_DELETE;  // delete operation
    fileop.pFrom  = pszFrom;  // source file name as double null terminated string
    fileop.pTo    = nullptr;    // no destination needed
    fileop.fFlags = FOF_NOCONFIRMATION | FOF_SILENT;  // do not prompt the user

    fileop.fAnyOperationsAborted = FALSE;
    fileop.lpszProgressTitle     = nullptr;
    fileop.hNameMappings         = nullptr;

    int32_t ret = SHFileOperationW(&fileop);
    return (ret == 0);
}

bool platform_lock_single_instance()
{
    HANDLE mutex, status;

    // Check if operating system mutex exists
    mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, SINGLE_INSTANCE_MUTEX_NAME);
    if (mutex == nullptr) {
        // Create new mutex
        status = CreateMutex(NULL, FALSE, SINGLE_INSTANCE_MUTEX_NAME);
        if (status == nullptr)
            log_error("unable to create mutex\n");

        return true;
    } else {
        // Already running
        CloseHandle(mutex);
        return false;
    }
}

int32_t platform_get_drives()
{
    return GetLogicalDrives();
}

bool platform_file_copy(const utf8 *srcPath, const utf8 *dstPath, bool overwrite)
{
    wchar_t *wSrcPath = utf8_to_widechar(srcPath);
    wchar_t *wDstPath = utf8_to_widechar(dstPath);
    BOOL success = CopyFileW(wSrcPath, wDstPath, overwrite ? FALSE : TRUE);
    free(wSrcPath);
    free(wDstPath);
    return success == TRUE;
}

bool platform_file_move(const utf8 *srcPath, const utf8 *dstPath)
{
    wchar_t *wSrcPath = utf8_to_widechar(srcPath);
    wchar_t *wDstPath = utf8_to_widechar(dstPath);
    BOOL success = MoveFileW(wSrcPath, wDstPath);
    free(wSrcPath);
    free(wDstPath);
    return success == TRUE;
}

bool platform_file_delete(const utf8 *path)
{
    wchar_t *wPath = utf8_to_widechar(path);
    BOOL success = DeleteFileW(wPath);
    free(wPath);
    return success == TRUE;
}

bool platform_get_steam_path(utf8 * outPath, size_t outSize)
{
    wchar_t * wSteamPath;
    HKEY hKey;
    DWORD type, size;
    LRESULT result;

    if (RegOpenKeyW(HKEY_CURRENT_USER, L"Software\\Valve\\Steam", &hKey) != ERROR_SUCCESS)
        return false;

    // Get the size of the path first
    if (RegQueryValueExW(hKey, L"SteamPath", 0, &type, NULL, &size) != ERROR_SUCCESS)
    {
        RegCloseKey(hKey);
        return false;
    }

    wSteamPath = (wchar_t*)malloc(size);
    result = RegQueryValueExW(hKey, L"SteamPath", 0, &type, (LPBYTE)wSteamPath, &size);
    if (result == ERROR_SUCCESS)
    {
        utf8 * utf8SteamPath = widechar_to_utf8(wSteamPath);
        safe_strcpy(outPath, utf8SteamPath, outSize);
        safe_strcat_path(outPath, "steamapps\\common", outSize);
        free(utf8SteamPath);
    }
    free(wSteamPath);
    RegCloseKey(hKey);
    return result == ERROR_SUCCESS;
}

uint16_t platform_get_locale_language()
{
    CHAR langCode[4];

    if (GetLocaleInfo(LOCALE_USER_DEFAULT,
        LOCALE_SABBREVLANGNAME,
        (LPSTR)&langCode,
        sizeof(langCode)) == 0){
        return LANGUAGE_UNDEFINED;
    }

    if (strcmp(langCode, "ENG") == 0){
        return LANGUAGE_ENGLISH_UK;
    }
    else if (strcmp(langCode, "ENU") == 0){
        return LANGUAGE_ENGLISH_US;
    }
    else if (strcmp(langCode, "DEU") == 0){
        return LANGUAGE_GERMAN;
    }
    else if (strcmp(langCode, "NLD") == 0){
        return LANGUAGE_DUTCH;
    }
    else if (strcmp(langCode, "FRA") == 0){
        return LANGUAGE_FRENCH;
    }
    else if (strcmp(langCode, "HUN") == 0){
        return LANGUAGE_HUNGARIAN;
    }
    else if (strcmp(langCode, "PLK") == 0){
        return LANGUAGE_POLISH;
    }
    else if (strcmp(langCode, "ESP") == 0){
        return LANGUAGE_SPANISH;
    }
    else if (strcmp(langCode, "SVE") == 0){
        return LANGUAGE_SWEDISH;
    }
    else if (strcmp(langCode, "ITA") == 0){
        return LANGUAGE_ITALIAN;
    }
    else if (strcmp(langCode, "POR") == 0){
        return LANGUAGE_PORTUGUESE_BR;
    }
    else if (strcmp(langCode, "FIN") == 0){
        return LANGUAGE_FINNISH;
    }
    else if (strcmp(langCode, "NOR") == 0){
        return LANGUAGE_NORWEGIAN;
    }
    return LANGUAGE_UNDEFINED;
}

time_t platform_file_get_modified_time(const utf8* path)
{
    WIN32_FILE_ATTRIBUTE_DATA data;

    wchar_t *wPath = utf8_to_widechar(path);
    BOOL result = GetFileAttributesExW(wPath, GetFileExInfoStandard, &data);
    free(wPath);

    if (result) {
        ULARGE_INTEGER ull;
        ull.LowPart = data.ftLastWriteTime.dwLowDateTime;
        ull.HighPart = data.ftLastWriteTime.dwHighDateTime;
        return ull.QuadPart / 10000000ULL - 11644473600ULL;
    } else {
        return 0;
    }
}

uint8_t platform_get_locale_currency()
{
    CHAR currCode[4];
    if (GetLocaleInfo(LOCALE_USER_DEFAULT,
        LOCALE_SINTLSYMBOL,
        (LPSTR)&currCode,
        sizeof(currCode)) == 0
    ) {
        return platform_get_currency_value(NULL);
    }

    return platform_get_currency_value(currCode);
}

uint8_t platform_get_locale_measurement_format()
{
    UINT measurement_system;
    if (GetLocaleInfo(LOCALE_USER_DEFAULT,
        LOCALE_IMEASURE | LOCALE_RETURN_NUMBER,
        (LPSTR)&measurement_system,
        sizeof(measurement_system)) == 0
    ) {
        return MEASUREMENT_FORMAT_METRIC;
    }

    switch (measurement_system) {
    case 1:
        return MEASUREMENT_FORMAT_IMPERIAL;
    case 0:
    default:
        return MEASUREMENT_FORMAT_METRIC;
    }
}

uint8_t platform_get_locale_temperature_format()
{
    UINT fahrenheit;

    // GetLocaleInfo will set fahrenheit to 1 if the locale on this computer
    // uses the United States measurement system or 0 otherwise.
    if (GetLocaleInfo(LOCALE_USER_DEFAULT,
        LOCALE_IMEASURE | LOCALE_RETURN_NUMBER,
        (LPSTR)&fahrenheit,
        sizeof(fahrenheit)) == 0
    ) {
        // Assume celsius by default if function call fails
        return TEMPERATURE_FORMAT_C;
    }

    if(fahrenheit)
        return TEMPERATURE_FORMAT_F;
    else
        return TEMPERATURE_FORMAT_C;
}

uint8_t platform_get_locale_date_format()
{
    // Retrieve short date format, eg "MM/dd/yyyy"
    wchar_t dateFormat[20];
    if (GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SSHORTDATE, dateFormat, sizeof(dateFormat)) == 0)
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
    if (swscanf(dateFormat, L"%l[dyM]%*l[^dyM]%l[dyM]%*l[^dyM]%*l[dyM]", first, second) != 2) {
        return DATE_FORMAT_DAY_MONTH_YEAR;
    }

    if (wcsncmp(L"d", first, 1) == 0)
    {
        return DATE_FORMAT_DAY_MONTH_YEAR;
    }
    else if (wcsncmp(L"M", first, 1) == 0)
    {
        return DATE_FORMAT_MONTH_DAY_YEAR;
    }
    else if (wcsncmp(L"y", first, 1) == 0)
    {
        if (wcsncmp(L"d", second, 1) == 0) {
            return DATE_FORMAT_YEAR_DAY_MONTH;
        }
        else {
            // Closest possible option
            return DATE_FORMAT_YEAR_MONTH_DAY;
        }
    }

    // Default fallback
    return DATE_FORMAT_DAY_MONTH_YEAR;
}

#ifndef NO_TTF
bool platform_get_font_path(TTFFontDescriptor *font, utf8 *buffer, size_t size)
{
#if !defined(__MINGW32__) && ((NTDDI_VERSION >= NTDDI_VISTA) && !defined(_USING_V110_SDK71_) && !defined(_ATL_XP_TARGETING))
    wchar_t *fontFolder;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Fonts, 0, NULL, &fontFolder)))
    {
        // Convert wchar to utf8, then copy the font folder path to the buffer.
        utf8 *outPathTemp = widechar_to_utf8(fontFolder);
        safe_strcpy(buffer, outPathTemp, size);
        free(outPathTemp);

        CoTaskMemFree(fontFolder);

        // Append the requested font's file name.
        safe_strcat_path(buffer, font->filename, size);
        return true;
    }
    else
    {
        return false;
    }
#else
    log_warning("Compatibility hack: falling back to C:\\Windows\\Fonts");
    safe_strcpy(buffer, "C:\\Windows\\Fonts\\", size);
    safe_strcat_path(buffer, font->filename, size);
    return true;
#endif
}
#endif // NO_TTF

utf8 * platform_get_absolute_path(const utf8 * relativePath, const utf8 * basePath)
{
    utf8 path[MAX_PATH];
    safe_strcpy(path, basePath, sizeof(path));
    safe_strcat_path(path, relativePath, sizeof(path));

    wchar_t * pathW = utf8_to_widechar(path);
    wchar_t fullPathW[MAX_PATH];
    DWORD fullPathLen = GetFullPathNameW(pathW, (DWORD)Util::CountOf(fullPathW), fullPathW, NULL);

    free(pathW);

    if (fullPathLen == 0)
    {
        return nullptr;
    }

    return widechar_to_utf8(fullPathW);
}

datetime64 platform_get_datetime_now_utc()
{
    // Get file time
    FILETIME fileTime;
    GetSystemTimeAsFileTime(&fileTime);
    uint64_t fileTime64 = ((uint64_t)fileTime.dwHighDateTime << 32ULL) | ((uint64_t)fileTime.dwLowDateTime);

    // File time starts from: 1601-01-01T00:00:00Z
    // Convert to start from: 0001-01-01T00:00:00Z
    datetime64 utcNow = fileTime64 - 504911232000000000ULL;
    return utcNow;
}

utf8* platform_get_username()
{
    static char username[UNLEN + 1];

    DWORD usernameLength = UNLEN + 1;
    if (!GetUserName(username, &usernameLength)) {
        return nullptr;
    }

    return username;
}

bool platform_process_is_elevated()
{
    BOOL isElevated = FALSE;
    HANDLE hToken = nullptr;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION Elevation;
        DWORD tokenSize = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &tokenSize)) {
            isElevated = Elevation.TokenIsElevated;
        }
    }
    if (hToken) {
        CloseHandle(hToken);
    }
    return isElevated;
}

///////////////////////////////////////////////////////////////////////////////
// File association setup
///////////////////////////////////////////////////////////////////////////////

#define SOFTWARE_CLASSES    L"Software\\Classes"
#define MUI_CACHE           L"Local Settings\\Software\\Microsoft\\Windows\\Shell\\MuiCache"

static void get_progIdName(wchar_t *dst, const utf8 *extension)
{
    utf8 progIdName[128];
    safe_strcpy(progIdName, OPENRCT2_NAME, sizeof(progIdName));
    safe_strcat(progIdName, extension, sizeof(progIdName));

    wchar_t *progIdNameW = utf8_to_widechar(progIdName);
    lstrcpyW(dst, progIdNameW);
    free(progIdNameW);
}

static bool windows_setup_file_association(
    const utf8 * extension,
    const utf8 * fileTypeText,
    const utf8 * commandText,
    const utf8 * commandArgs,
    const uint32_t iconIndex
) {
    wchar_t exePathW[MAX_PATH];
    wchar_t dllPathW[MAX_PATH];

    [[maybe_unused]] int32_t printResult;

    GetModuleFileNameW(NULL, exePathW, sizeof(exePathW));
    GetModuleFileNameW(plaform_get_dll_module(), dllPathW, sizeof(dllPathW));

    wchar_t *extensionW = utf8_to_widechar(extension);
    wchar_t *fileTypeTextW = utf8_to_widechar(fileTypeText);
    wchar_t *commandTextW = utf8_to_widechar(commandText);
    wchar_t *commandArgsW = utf8_to_widechar(commandArgs);

    wchar_t progIdNameW[128];
    get_progIdName(progIdNameW, extension);

    bool result = false;
    HKEY hKey = nullptr;
    HKEY hRootKey = nullptr;

    // [HKEY_CURRENT_USER\Software\Classes]
    if (RegOpenKeyW(HKEY_CURRENT_USER, SOFTWARE_CLASSES, &hRootKey) != ERROR_SUCCESS) {
        goto fail;
    }

    // [hRootKey\.ext]
    if (RegSetValueW(hRootKey, extensionW, REG_SZ, progIdNameW, 0) != ERROR_SUCCESS) {
        goto fail;
    }

    if (RegCreateKeyW(hRootKey, progIdNameW, &hKey) != ERROR_SUCCESS) {
        goto fail;
    }

    // [hRootKey\OpenRCT2.ext]
    if (RegSetValueW(hKey, NULL, REG_SZ, fileTypeTextW, 0) != ERROR_SUCCESS) {
        goto fail;
    }
    // [hRootKey\OpenRCT2.ext\DefaultIcon]
    wchar_t szIconW[MAX_PATH];
    printResult = swprintf_s(szIconW, MAX_PATH, L"\"%s\",%d", dllPathW, iconIndex);
    assert(printResult >= 0);
    if (RegSetValueW(hKey, L"DefaultIcon", REG_SZ, szIconW, 0) != ERROR_SUCCESS) {
        goto fail;
    }

    // [hRootKey\OpenRCT2.sv6\shell]
    if (RegSetValueW(hKey, L"shell", REG_SZ, L"open", 0) != ERROR_SUCCESS) {
        goto fail;
    }

    // [hRootKey\OpenRCT2.sv6\shell\open]
    if (RegSetValueW(hKey, L"shell\\open", REG_SZ, commandTextW, 0) != ERROR_SUCCESS) {
        goto fail;
    }

    // [hRootKey\OpenRCT2.sv6\shell\open\command]
    wchar_t szCommandW[MAX_PATH];
    printResult = swprintf_s(szCommandW, MAX_PATH, L"\"%s\" %s", exePathW, commandArgsW);
    assert(printResult >= 0);
    if (RegSetValueW(hKey, L"shell\\open\\command", REG_SZ, szCommandW, 0) != ERROR_SUCCESS) {
        goto fail;
    }

    result = true;
fail:
    free(extensionW);
    free(fileTypeTextW);
    free(commandTextW);
    free(commandArgsW);
    RegCloseKey(hKey);
    RegCloseKey(hRootKey);
    return result;
}

static void windows_remove_file_association(const utf8 * extension)
{
    // [HKEY_CURRENT_USER\Software\Classes]
    HKEY hRootKey;
    if (RegOpenKeyW(HKEY_CURRENT_USER, SOFTWARE_CLASSES, &hRootKey) == ERROR_SUCCESS) {
        // [hRootKey\.ext]
        RegDeleteTreeA(hRootKey, extension);

        // [hRootKey\OpenRCT2.ext]
        wchar_t progIdName[128];
        get_progIdName(progIdName, extension);
        RegDeleteTreeW(hRootKey, progIdName);

        RegCloseKey(hRootKey);
    }
}

void platform_setup_file_associations()
{
    // Setup file extensions
    windows_setup_file_association(".sc4", "RCT1 Scenario (.sc4)",     "Play",    "\"%1\"", 0);
    windows_setup_file_association(".sc6", "RCT2 Scenario (.sc6)",     "Play",    "\"%1\"", 0);
    windows_setup_file_association(".sv4", "RCT1 Saved Game (.sc4)",   "Play",    "\"%1\"", 0);
    windows_setup_file_association(".sv6", "RCT2 Saved Game (.sv6)",   "Play",    "\"%1\"", 0);
    windows_setup_file_association(".td4", "RCT1 Track Design (.td4)", "Install", "\"%1\"", 0);
    windows_setup_file_association(".td6", "RCT2 Track Design (.td6)", "Install", "\"%1\"", 0);

    // Refresh explorer
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
}

void platform_remove_file_associations()
{
    // Remove file extensions
    windows_remove_file_association(".sc4");
    windows_remove_file_association(".sc6");
    windows_remove_file_association(".sv4");
    windows_remove_file_association(".sv6");
    windows_remove_file_association(".td4");
    windows_remove_file_association(".td6");

    // Refresh explorer
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
}

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// URI protocol association setup
///////////////////////////////////////////////////////////////////////////////

bool platform_setup_uri_protocol()
{
    log_verbose("Setting up URI protocol...");

    // [HKEY_CURRENT_USER\Software\Classes]
    HKEY hRootKey;
    if (RegOpenKeyW(HKEY_CURRENT_USER, SOFTWARE_CLASSES, &hRootKey) == ERROR_SUCCESS) {
        // [hRootKey\openrct2]
        HKEY hClassKey;
        if (RegCreateKeyA(hRootKey, "openrct2", &hClassKey) == ERROR_SUCCESS) {
            if (RegSetValueA(hClassKey, NULL, REG_SZ, "URL:openrct2", 0) == ERROR_SUCCESS) {
                if (RegSetKeyValueA(hClassKey, NULL, "URL Protocol", REG_SZ, "", 0) == ERROR_SUCCESS) {
                    // [hRootKey\openrct2\shell\open\command]
                    wchar_t exePath[MAX_PATH];
                    GetModuleFileNameW(NULL, exePath, MAX_PATH);

                    wchar_t buffer[512];
                    swprintf_s(buffer, sizeof(buffer), L"\"%s\" handle-uri \"%%1\"", exePath);
                    if (RegSetValueW(hClassKey, L"shell\\open\\command", REG_SZ, buffer, 0) == ERROR_SUCCESS) {
                        // Not compulsory, but gives the application a nicer name
                        // [HKEY_CURRENT_USER\SOFTWARE\Classes\Local Settings\Software\Microsoft\Windows\Shell\MuiCache]
                        HKEY hMuiCacheKey;
                        if (RegCreateKeyW(hRootKey, MUI_CACHE, &hMuiCacheKey) == ERROR_SUCCESS) {
                            swprintf_s(buffer, sizeof(buffer), L"%s.FriendlyAppName", exePath);
                            // mingw-w64 used to define RegSetKeyValueW's signature incorrectly
                            // You need at least mingw-w64 5.0 including this commit:
                            //   https://sourceforge.net/p/mingw-w64/mingw-w64/ci/da9341980a4b70be3563ac09b5927539e7da21f7/
                            RegSetKeyValueW(hMuiCacheKey, NULL, buffer, REG_SZ, L"OpenRCT2", sizeof(L"OpenRCT2") + 1);
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

///////////////////////////////////////////////////////////////////////////////
#endif
