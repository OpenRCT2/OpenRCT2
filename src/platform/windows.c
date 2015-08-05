/*****************************************************************************
 * Copyright (c) 2014 Ted John
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 * 
 * This file is part of OpenRCT2.
 * 
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#ifdef _WIN32

#include <windows.h>
#include <shlobj.h>
#include <SDL_syswm.h>
#include <sys/stat.h>
#include "../addresses.h"
#include "../cmdline.h"
#include "../openrct2.h"
#include "../localisation/language.h"
#include "../localisation/currency.h"
#include "../config.h"
#include "platform.h"

// The name of the mutex used to prevent multiple instances of the game from running
#define SINGLE_INSTANCE_MUTEX_NAME "RollerCoaster Tycoon 2_GSKMUTEX"

LPSTR *CommandLineToArgvA(LPSTR lpCmdLine, int *argc);

/**
 * Windows entry point to OpenRCT2 without a console window.
 */
// int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
// {
//     return 0;
// }

/**
 * Windows entry point to OpenRCT2 with a console window using a traditional C main function.
 */
// int main(char *argv[], int argc)
// {
//     return 0;
// }

/**
 * Entry point for when the DLL is loaded. This will be removed when OpenRCT2 can be built as a stand alone application.
 */
BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
	return TRUE;
}

/**
 * The function that is called directly from the host application (rct2.exe)'s WinMain. This will be removed when OpenRCT2 can
 * be built as a stand alone application.
 */
__declspec(dllexport) int StartOpenRCT(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	int argc, runGame;
	char **argv;

	RCT2_GLOBAL(RCT2_ADDRESS_HINSTANCE, HINSTANCE) = hInstance;
	RCT2_GLOBAL(RCT2_ADDRESS_CMDLINE, LPSTR) = lpCmdLine;

	// Get command line arguments in standard form
	argv = CommandLineToArgvA(lpCmdLine, &argc);
	runGame = cmdline_run(argv, argc);
	GlobalFree(argv);

	if (runGame)
		openrct2_launch();

	exit(gExitCode);
	return gExitCode;
}

char platform_get_path_separator()
{
	return '\\';
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
	sprintf(checkPath, "%s%c%s%c%s", path, platform_get_path_separator(), "data", platform_get_path_separator(), "g1.dat");
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
	free(wPath);

	// Needs to be double-null terminated for some weird reason
	pszFrom[wcslen(wPath) + 1] = 0;

	SHFILEOPSTRUCTW fileop;
	fileop.hwnd   = NULL;    // no status display
	fileop.wFunc  = FO_DELETE;  // delete operation
	fileop.pFrom  = pszFrom;  // source file name as double null terminated string
	fileop.pTo    = NULL;    // no destination needed
	fileop.fFlags = FOF_NOCONFIRMATION | FOF_SILENT;  // do not prompt the user

	fileop.fAnyOperationsAborted = FALSE;
	fileop.lpszProgressTitle     = NULL;
	fileop.hNameMappings         = NULL;

	int ret = SHFileOperationW(&fileop);
	return (ret == 0);
}

bool platform_lock_single_instance()
{
	HANDLE mutex, status;

	// Check if operating system mutex exists
	mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, SINGLE_INSTANCE_MUTEX_NAME);
	if (mutex == NULL) {
		// Create new mutex
		status = CreateMutex(NULL, FALSE, SINGLE_INSTANCE_MUTEX_NAME);
		if (status == NULL)
			log_error("unable to create mutex\n");

		return true;
	} else {
		// Already running
		CloseHandle(mutex);
		return false;
	}
}

typedef struct {
	bool active;
	wchar_t pattern[MAX_PATH];
	HANDLE handle;
	WIN32_FIND_DATAW data;
	utf8 *outFilename;
} enumerate_file_info;
static enumerate_file_info _enumerateFileInfoList[8] = { 0 };

int platform_enumerate_files_begin(const utf8 *pattern)
{
	int i;
	enumerate_file_info *enumFileInfo;

	wchar_t *wPattern = utf8_to_widechar(pattern);

	for (i = 0; i < countof(_enumerateFileInfoList); i++) {
		enumFileInfo = &_enumerateFileInfoList[i];
		if (!enumFileInfo->active) {
			wcsncpy(enumFileInfo->pattern, wPattern, MAX_PATH);
			enumFileInfo->handle = NULL;
			enumFileInfo->active = true;
			enumFileInfo->outFilename = NULL;

			free(wPattern);
			return i;
		}
	}

	free(wPattern);
	return INVALID_HANDLE;
}

bool platform_enumerate_files_next(int handle, file_info *outFileInfo)
{
	bool result;
	enumerate_file_info *enumFileInfo;
	HANDLE findFileHandle;

	enumFileInfo = &_enumerateFileInfoList[handle];

	if (enumFileInfo->handle == NULL) {
		findFileHandle = FindFirstFileW(enumFileInfo->pattern, &enumFileInfo->data);
		if (findFileHandle != INVALID_HANDLE_VALUE) {
			enumFileInfo->handle = findFileHandle;
			result = true;
		} else {
			result = false;
		}
	} else {
		result = FindNextFileW(enumFileInfo->handle, &enumFileInfo->data);
	}

	if (result) {
		outFileInfo->path = enumFileInfo->outFilename = widechar_to_utf8(enumFileInfo->data.cFileName);
		outFileInfo->size = ((uint64)enumFileInfo->data.nFileSizeHigh << 32ULL) | (uint64)enumFileInfo->data.nFileSizeLow;
		outFileInfo->last_modified = ((uint64)enumFileInfo->data.ftLastWriteTime.dwHighDateTime << 32ULL) | (uint64)enumFileInfo->data.ftLastWriteTime.dwLowDateTime;
		return true;
	} else {
		return false;
	}
}

void platform_enumerate_files_end(int handle)
{
	enumerate_file_info *enumFileInfo;

	enumFileInfo = &_enumerateFileInfoList[handle];
	if (enumFileInfo->handle != NULL) {
		FindClose(enumFileInfo->handle);
		enumFileInfo->handle = NULL;
	}
	enumFileInfo->active = false;
	SafeFree(enumFileInfo->outFilename);
}

int platform_enumerate_directories_begin(const utf8 *directory)
{
	int i;
	enumerate_file_info *enumFileInfo;

	wchar_t *wDirectory = utf8_to_widechar(directory);

	if (wcslen(wDirectory) + 3 >= MAX_PATH)
		return INVALID_HANDLE;

	for (i = 0; i < countof(_enumerateFileInfoList); i++) {
		enumFileInfo = &_enumerateFileInfoList[i];
		if (!enumFileInfo->active) {
			wcsncpy(enumFileInfo->pattern, wDirectory, MAX_PATH);
			wcsncat(enumFileInfo->pattern, L"*", MAX_PATH);
			enumFileInfo->handle = NULL;
			enumFileInfo->active = true;
			enumFileInfo->outFilename = NULL;

			free(wDirectory);
			return i;
		}
	}

	free(wDirectory);
	return INVALID_HANDLE;
}

bool platform_enumerate_directories_next(int handle, utf8 *path)
{
	enumerate_file_info *enumFileInfo;
	HANDLE fileHandle;

	enumFileInfo = &_enumerateFileInfoList[handle];

	if (enumFileInfo->handle == NULL) {
		fileHandle = FindFirstFileW(enumFileInfo->pattern, &enumFileInfo->data);
		if (fileHandle != 0) {
			enumFileInfo->handle = fileHandle;
		} else {
			return false;
		}
	} else {
		if (!FindNextFileW(enumFileInfo->handle, &enumFileInfo->data)) {
			return false;
		}
	}

	while (
		(enumFileInfo->data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 ||
		wcschr(enumFileInfo->data.cFileName, '.') != NULL
	) {
		if (!FindNextFileW(enumFileInfo->handle, &enumFileInfo->data)) {
			return false;
		}
	}

	utf8 *filename = widechar_to_utf8(enumFileInfo->data.cFileName);
	strncpy(path, filename, MAX_PATH);
	strncat(path, "\\", MAX_PATH);
	free(filename);
	return true;
}

void platform_enumerate_directories_end(int handle)
{
	enumerate_file_info *enumFileInfo;

	enumFileInfo = &_enumerateFileInfoList[handle];
	if (enumFileInfo->handle != NULL) {
		FindClose(enumFileInfo->handle);
		enumFileInfo->handle = NULL;
	}
	enumFileInfo->active = false;
}

int platform_get_drives()
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

void platform_hide_cursor()
{
	ShowCursor(FALSE);
}

void platform_show_cursor()
{
	ShowCursor(TRUE);
}

void platform_get_cursor_position(int *x, int *y)
{
	POINT point;

	if (GetCursorPos(&point)) {
		*x = point.x;
		*y = point.y;
	} else {
		*x = INT32_MIN;
		*y = INT32_MIN;
	}
}

void platform_set_cursor_position(int x, int y)
{
	SetCursorPos(x, y);
}

unsigned int platform_get_ticks()
{
	return GetTickCount();
}

void platform_get_user_directory(utf8 *outPath, const utf8 *subDirectory)
{
	wchar_t wOutPath[MAX_PATH];
	char separator[2] = { platform_get_path_separator(), 0 };

	if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_PERSONAL | CSIDL_FLAG_CREATE, NULL, 0, wOutPath))) {
		utf8 *outPathTemp = widechar_to_utf8(wOutPath);
		strcpy(outPath, outPathTemp);
		free(outPathTemp);

		strcat(outPath, separator);
		strcat(outPath, "OpenRCT2");
		strcat(outPath, separator);
		if (subDirectory != NULL && subDirectory[0] != 0) {
			strcat(outPath, subDirectory);
			strcat(outPath, separator);
		}
	} else {
		outPath[0] = 0;
	}
}

void platform_show_messagebox(char *message)
{
	MessageBoxA(windows_get_window_handle(), message, "OpenRCT2", MB_OK);
}

/**
 * 
 *  rct2: 0x004080EA
 */
int platform_open_common_file_dialog(int type, utf8 *title, utf8 *filename, utf8 *filterPattern, utf8 *filterName)
{
	wchar_t wctitle[256], wcfilename[MAX_PATH], wcfilterPattern[256], wcfilterName[256];
	wchar_t initialDirectory[MAX_PATH], *dotAddress, *slashAddress;
	OPENFILENAMEW openFileName;
	BOOL result;
	int tmp;
	DWORD commonFlags;

	MultiByteToWideChar(CP_UTF8, 0, title, -1, wctitle, countof(wctitle));
	MultiByteToWideChar(CP_UTF8, 0, filename, -1, wcfilename, countof(wcfilename));
	MultiByteToWideChar(CP_UTF8, 0, filterPattern, -1, wcfilterPattern, countof(wcfilterPattern));
	MultiByteToWideChar(CP_UTF8, 0, filterName, -1, wcfilterName, countof(wcfilterName));

	// Get directory path from given filename
	lstrcpyW(initialDirectory, wcfilename);
	dotAddress = wcsrchr(initialDirectory, '.');
	if (dotAddress != NULL) {
		slashAddress = wcsrchr(initialDirectory, '\\');
		if (slashAddress < dotAddress)
			*(slashAddress + 1) = 0;
	}

	// Clear filename
	if (type != 0)
	wcfilename[0] = 0;

	// Set open file name options
	memset(&openFileName, 0, sizeof(OPENFILENAMEW));
	openFileName.lStructSize = sizeof(OPENFILENAMEW);
	openFileName.hwndOwner = windows_get_window_handle();
	openFileName.lpstrFile = wcfilename;
	openFileName.nMaxFile = MAX_PATH;
	openFileName.lpstrInitialDir = initialDirectory;
	openFileName.lpstrTitle = wctitle;

	// Copy filter name
	lstrcpyW((wchar_t*)0x01423800, wcfilterName);

	// Copy filter pattern
	int wcfilterNameLength = lstrlenW(wcfilterName);
	int wcfilterPatternLength = lstrlenW(wcfilterPattern);

	lstrcpyW((wchar_t*)0x01423800 + wcfilterNameLength + 1, wcfilterPattern);
	*((wchar_t*)((wchar_t*)0x01423800 + wcfilterNameLength + 1 + wcfilterPatternLength + 1)) = 0;
	openFileName.lpstrFilter = (wchar_t*)0x01423800;

	// 
	tmp = RCT2_GLOBAL(0x009E2C74, uint32);
	if (RCT2_GLOBAL(0x009E2BB8, uint32) == 2 && RCT2_GLOBAL(0x009E1AF8, uint32) == 1)
		RCT2_GLOBAL(0x009E2C74, uint32) = 1;

	// Open dialog
	commonFlags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
	if (type == 0) {
		openFileName.Flags = commonFlags | OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT;
		result = GetSaveFileNameW(&openFileName);
	} else if (type == 1) {
		openFileName.Flags = commonFlags | OFN_NONETWORKBUTTON | OFN_FILEMUSTEXIST;
		result = GetOpenFileNameW(&openFileName);
	}

	// 
	RCT2_GLOBAL(0x009E2C74, uint32) = tmp;

	WideCharToMultiByte(CP_UTF8, 0, wcfilename, countof(wcfilename), filename, MAX_PATH, NULL, NULL);

	return result;
}

utf8 *platform_open_directory_browser(utf8 *title)
{
	BROWSEINFOW bi;
	wchar_t pszBuffer[MAX_PATH], wctitle[256];
	LPITEMIDLIST pidl;
	LPMALLOC lpMalloc;

	MultiByteToWideChar(CP_UTF8, 0, title, -1, wctitle, countof(wctitle));

	// Initialize COM
	if (FAILED(CoInitializeEx(0, COINIT_APARTMENTTHREADED))) {
		CoUninitialize();

		log_error("Error opening directory browse window");
		return 0;
	}

	// Get a pointer to the shell memory allocator
	if (FAILED(SHGetMalloc(&lpMalloc))) {
		CoUninitialize();

		log_error("Error opening directory browse window");
		return 0;
	}

	bi.hwndOwner = NULL;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = pszBuffer;
	bi.lpszTitle = wctitle;
	bi.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.lParam = 0;

	utf8 *outPath = NULL;

	if (pidl = SHBrowseForFolderW(&bi)) {
		// Copy the path directory to the buffer
		if (SHGetPathFromIDListW(pidl, pszBuffer)) {
			// Store pszBuffer (and the path) in the outPath
			int outPathCapacity = lstrlenW(pszBuffer) * 4 + 1;
			outPath = (utf8*)malloc(outPathCapacity);
			WideCharToMultiByte(CP_UTF8, 0, pszBuffer, countof(pszBuffer), outPath, outPathCapacity, NULL, NULL);
		}
	}
	CoUninitialize();
	return outPath;
}

/**
 * 
 *  rct2: 0x00407978
 */
int windows_get_registry_install_info(rct2_install_info *installInfo, char *source, char *font, uint8 charset)
{
	char subkeyInfogrames[MAX_PATH], subkeyFishTechGroup[MAX_PATH], keyName[100];
	LOGFONTA lf;
	HKEY hKey;
	DWORD type, size;

	strcpy(subkeyInfogrames, "Software\\Infogrames\\");
	strcat(subkeyInfogrames, source);
	strcpy(subkeyFishTechGroup, "Software\\Fish Technology Group\\");
	strcat(subkeyFishTechGroup, source);
	
	memset(&lf, 0, sizeof(lf));
	lf.lfCharSet = charset;
	lf.lfHeight = 12;
	lf.lfWeight = 400;
	strcpy(lf.lfFaceName, font);

	RCT2_GLOBAL(RCT2_ADDRESS_HFONT, HFONT) = CreateFontIndirectA(&lf);
	
	if (RegOpenKeyA(HKEY_LOCAL_MACHINE, subkeyInfogrames, &hKey) != ERROR_SUCCESS)
		return 0;

	if (RegOpenKeyA(HKEY_LOCAL_MACHINE, subkeyFishTechGroup, &hKey) != ERROR_SUCCESS)
		return 0;

	
	size = 260;
	RegQueryValueExA(hKey, "Title", 0, &type, installInfo->title, &size);

	size = 260;
	RegQueryValueExA(hKey, "Path", 0, &type, installInfo->path, &size);
	
	installInfo->var_20C = 235960;
	
	size = 4;
	RegQueryValueExA(hKey, "InstallLevel", 0, &type, (LPBYTE)&installInfo->installLevel, &size);
	for (int i = 0; i <= 15; i++) {
		sprintf(keyName, "AddonPack%d", i);
		size = sizeof(installInfo->expansionPackNames[i]);
		if (RegQueryValueExA(hKey, keyName, 0, &type, installInfo->expansionPackNames[i], &size) == ERROR_SUCCESS)
			installInfo->activeExpansionPacks |= (1 << i);
	}

	RegCloseKey(hKey);
	return 1;
}

HWND windows_get_window_handle()
{
	SDL_SysWMinfo wmInfo;

	if (gWindow == NULL)
		return NULL;

	SDL_VERSION(&wmInfo.version);
	if (SDL_GetWindowWMInfo(gWindow, &wmInfo) != SDL_TRUE) {
		log_error("SDL_GetWindowWMInfo failed %s", SDL_GetError());
		exit(-1);
	}
	return wmInfo.info.win.window;
}

/**
 * http://alter.org.ua/en/docs/win/args/
 */
PCHAR *CommandLineToArgvA(PCHAR CmdLine, int *_argc)
{
	PCHAR* argv;
	PCHAR  _argv;
	ULONG   len;
	ULONG   argc;
	CHAR   a;
	ULONG   i, j;

	BOOLEAN  in_QM;
	BOOLEAN  in_TEXT;
	BOOLEAN  in_SPACE;

	len = strlen(CmdLine);
	i = ((len + 2) / 2)*sizeof(PVOID) + sizeof(PVOID);

	argv = (PCHAR*)GlobalAlloc(GMEM_FIXED,
		i + (len + 2)*sizeof(CHAR) + 1);

	_argv = (PCHAR)(((PUCHAR)argv) + i);

	// Add in virtual 1st command line argument, process path, for arg_parse's sake.
	argv[0] = 0;
	argc = 1;
	argv[argc] = _argv;
	in_QM = FALSE;
	in_TEXT = FALSE;
	in_SPACE = TRUE;
	i = 0;
	j = 0;

	while (a = CmdLine[i]) {
		if (in_QM) {
			if (a == '\"') {
				in_QM = FALSE;
			} else {
				_argv[j] = a;
				j++;
			}
		} else {
			switch (a) {
			case '\"':
				in_QM = TRUE;
				in_TEXT = TRUE;
				if (in_SPACE) {
					argv[argc] = _argv + j;
					argc++;
				}
				in_SPACE = FALSE;
				break;
			case ' ':
			case '\t':
			case '\n':
			case '\r':
				if (in_TEXT) {
					_argv[j] = '\0';
					j++;
				}
				in_TEXT = FALSE;
				in_SPACE = TRUE;
				break;
			default:
				in_TEXT = TRUE;
				if (in_SPACE) {
					argv[argc] = _argv + j;
					argc++;
				}
				_argv[j] = a;
				j++;
				in_SPACE = FALSE;
				break;
			}
		}
		i++;
	}
	_argv[j] = '\0';
	argv[argc] = NULL;

	(*_argc) = argc;
	return argv;
}

uint16 platform_get_locale_language()
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

uint8 platform_get_locale_currency()
{
	CHAR currCode[4];

	if (GetLocaleInfo(LOCALE_USER_DEFAULT,
		LOCALE_SINTLSYMBOL,
		(LPSTR)&currCode,
		sizeof(currCode)) == 0
	) {
		return CURRENCY_POUNDS;
	}
	if (strcmp(currCode, "GBP") == 0){
		return CURRENCY_POUNDS;
	}
	else if (strcmp(currCode, "USD") == 0){
		return CURRENCY_DOLLARS;
	}
	else if (strcmp(currCode, "EUR") == 0){
		return CURRENCY_EUROS;
	}
	else if (strcmp(currCode, "SEK") == 0){
		return CURRENCY_KRONA;
	}
	else if (strcmp(currCode, "DEM") == 0){
		return CURRENCY_DEUTSCHMARK;
	}
	else if (strcmp(currCode, "ITL") == 0){
		return CURRENCY_LIRA;
	}
	else if (strcmp(currCode, "JPY") == 0){
		return CURRENCY_YEN;
	}
	else if (strcmp(currCode, "ESP") == 0){
		return CURRENCY_PESETA;
	}
	else if (strcmp(currCode, "FRF") == 0){
		return CURRENCY_FRANC;
	}
	else if (strcmp(currCode, "NLG") == 0){
		return CURRENCY_GUILDERS;
	}
	return CURRENCY_POUNDS;
}

uint8 platform_get_locale_measurement_format()
{
	UINT measurement_system;
	if (GetLocaleInfo(LOCALE_USER_DEFAULT,
		LOCALE_IMEASURE | LOCALE_RETURN_NUMBER,
		(LPSTR)&measurement_system,
		sizeof(measurement_system)) == 0
	) {
		return MEASUREMENT_FORMAT_IMPERIAL;
	}

	switch (measurement_system) {
	case 0:
		return MEASUREMENT_FORMAT_METRIC;
	case 1:
	default:
		return MEASUREMENT_FORMAT_IMPERIAL;
	}
}

uint8 platform_get_locale_temperature_format()
{
	// There does not seem to be a function to obtain this, just check the countries
	UINT country;
	if (GetLocaleInfo(LOCALE_USER_DEFAULT,
		LOCALE_IMEASURE | LOCALE_RETURN_NUMBER,
		(LPSTR)&country,
		sizeof(country)) == 0
	) {
		return TEMPERATURE_FORMAT_C;
	}

	switch (country) {
	case CTRY_UNITED_STATES:
	case CTRY_BELIZE:
		return TEMPERATURE_FORMAT_F;
	default:
		return TEMPERATURE_FORMAT_C;
	}
}
#endif
