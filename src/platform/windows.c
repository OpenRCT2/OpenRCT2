#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../common.h"

#ifdef __WINDOWS__

#include <windows.h>
#include <lmcons.h>
#include <psapi.h>
#include <shlobj.h>
#include <SDL_syswm.h>
#include <sys/stat.h>
#include "../openrct2.h"
#include "../localisation/language.h"
#include "../util/util.h"
#include "../config.h"
#include "platform.h"

// Native resource IDs
#include "../../resources/resource.h"

// The name of the mutex used to prevent multiple instances of the game from running
#define SINGLE_INSTANCE_MUTEX_NAME "RollerCoaster Tycoon 2_GSKMUTEX"

utf8 _userDataDirectoryPath[MAX_PATH] = { 0 };
utf8 _openrctDataDirectoryPath[MAX_PATH] = { 0 };

utf8 **windows_get_command_line_args(int *outNumArgs);

#define OPENRCT2_DLL_MODULE_NAME "openrct2.dll"

static HMODULE _dllModule = NULL;

#ifdef NO_RCT2

/**
 * Windows entry point to OpenRCT2 without a console window.
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	_dllModule = hInstance;

	int argc;
	char ** argv = (char**)windows_get_command_line_args(&argc);
	int runGame = cmdline_run(argv, argc);

	// Free argv
	for (int i = 0; i < argc; i++) {
		free(argv[i]);
	}
	free(argv);

	if (runGame == 1) {
		openrct2_launch();
	}

	return gExitCode;
}

/**
 * Windows entry point to OpenRCT2 with a console window using a traditional C main function.
 */
int main(int argc, char *argv[])
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	_dllModule = hInstance;

	int runGame = cmdline_run((const char **)argv, argc);
	if (runGame == 1) {
		openrct2_launch();
	}

	return gExitCode;
}

#else

/* DllMain is already defined in one of static libraries we implicitly depend
 * on (libcrypto), which is their bug really, but since we don't do anything in
 * here, just comment it out.
 */
#ifndef __MINGW32__
/**
 * Entry point for when the DLL is loaded. This will be removed when OpenRCT2 can be built as a stand alone application.
 */
BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
	_dllModule = hModule;
	return TRUE;
}
#endif // __MINGW32__

/**
 * The function that is called directly from the host application (rct2.exe)'s WinMain. This will be removed when OpenRCT2 can
 * be built as a stand alone application.
 */
__declspec(dllexport) int StartOpenRCT(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	int argc, runGame;
	char **argv;

	if (_dllModule == NULL) {
		_dllModule = GetModuleHandleA(OPENRCT2_DLL_MODULE_NAME);
	}

	// argv = CommandLineToArgvA(lpCmdLine, &argc);
	argv = (char**)windows_get_command_line_args(&argc);
	runGame = cmdline_run((const char **)argv, argc);

	// Free argv
	for (int i = 0; i < argc; i++) {
		free(argv[i]);
	}
	free(argv);

	if (runGame == 1) {
		openrct2_launch();
	}

	exit(gExitCode);
	return gExitCode;
}

#endif // NO_RCT2

utf8 **windows_get_command_line_args(int *outNumArgs)
{
	int argc;

	// Get command line arguments as widechar
	LPWSTR commandLine = GetCommandLineW();
	LPWSTR *argvW = CommandLineToArgvW(commandLine, &argc);

	// Convert to UTF-8
	utf8 **argvUtf8 = (utf8**)malloc(argc * sizeof(utf8*));
	for (int i = 0; i < argc; i++) {
		argvUtf8[i] = widechar_to_utf8(argvW[i]);
	}
	LocalFree(argvW);

	*outNumArgs = argc;
	return argvUtf8;
}

void platform_get_date_local(rct2_date *out_date)
{
	assert(out_date != NULL);
	SYSTEMTIME systime;

	GetSystemTime(&systime);
	out_date->day = systime.wDay;
	out_date->month = systime.wMonth;
	out_date->year = systime.wYear;
	out_date->day_of_week = systime.wDayOfWeek;
}

void platform_get_time_local(rct2_time *out_time)
{
	assert(out_time != NULL);
	SYSTEMTIME systime;
	GetLocalTime(&systime);
	out_time->hour = systime.wHour;
	out_time->minute = systime.wMinute;
	out_time->second = systime.wSecond;
}

char platform_get_path_separator()
{
	return '\\';
}

const char *platform_get_new_line()
{
	return "\r\n";
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
	sprintf(checkPath, "%s%c%s%c%s", path, platform_get_path_separator(), "Data", platform_get_path_separator(), "g1.dat");
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

/**
 * Due to FindFirstFile / FindNextFile searching for DOS names as well, *.doc also matches *.docx which isn't what the pattern
 * specified. This will verify if a filename does indeed match the pattern we asked for.
 * @remarks Based on algorithm (http://xoomer.virgilio.it/acantato/dev/wildcard/wildmatch.html)
 */
static bool match_wildcard(const wchar_t *fileName, const wchar_t *pattern)
{
	while (*fileName != '\0') {
		switch (*pattern) {
		case '?':
			if (*fileName == '.') {
				return false;
			}
			break;
		case '*':
			do {
				pattern++;
			} while (*pattern == '*');
			if (*pattern == '\0') {
				return false;
			}
			while (*fileName != '\0') {
				if (match_wildcard(fileName++, pattern)) {
					return true;
				}
			}
			return false;
		default:
			if (toupper(*fileName) != toupper(*pattern)) {
				return false;
			}
			break;
		}
		pattern++;
		fileName++;
	}
	while (*pattern == '*') {
		++fileName;
	}
	return *pattern == '\0';
}

bool platform_enumerate_files_next(int handle, file_info *outFileInfo)
{
	bool result;
	enumerate_file_info *enumFileInfo;
	HANDLE findFileHandle;

	enumFileInfo = &_enumerateFileInfoList[handle];

	// Get pattern (just filename part)
	const wchar_t *patternWithoutDirectory = wcsrchr(enumFileInfo->pattern, '\\');
	if (patternWithoutDirectory == NULL) {
		patternWithoutDirectory = enumFileInfo->pattern;
	} else {
		patternWithoutDirectory++;
	}

	do {
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
	} while (result && !match_wildcard(enumFileInfo->data.cFileName, patternWithoutDirectory));

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

	if (wcslen(wDirectory) + 3 >= MAX_PATH) {
		free(wDirectory);
		return INVALID_HANDLE;
	}

	for (i = 0; i < countof(_enumerateFileInfoList); i++) {
		enumFileInfo = &_enumerateFileInfoList[i];
		if (!enumFileInfo->active) {
			wcsncpy(enumFileInfo->pattern, wDirectory, MAX_PATH);

			// Ensure pattern ends with a slash
			int patternLength = lstrlenW(enumFileInfo->pattern);
			wchar_t lastChar = enumFileInfo->pattern[patternLength - 1];
			if (lastChar != '\\' && lastChar != '/') {
				wcsncat(enumFileInfo->pattern, L"\\", MAX_PATH);
			}

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
		if (fileHandle != INVALID_HANDLE_VALUE) {
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

void platform_resolve_openrct_data_path()
{
	wchar_t wOutPath[MAX_PATH];
	const char separator[2] = { platform_get_path_separator(), 0 };

	if (gCustomOpenrctDataPath[0] != 0) {
		wchar_t *customUserDataPathW = utf8_to_widechar(gCustomOpenrctDataPath);
		if (GetFullPathNameW(customUserDataPathW, countof(wOutPath), wOutPath, NULL) == 0) {
			log_fatal("Unable to resolve path '%s'.", gCustomOpenrctDataPath);
			exit(-1);
		}
		utf8 *outPathTemp = widechar_to_utf8(wOutPath);
		safe_strcpy(_userDataDirectoryPath, outPathTemp, sizeof(_userDataDirectoryPath));
		free(outPathTemp);
		free(customUserDataPathW);

		// Ensure path ends with separator
		size_t len = strlen(_userDataDirectoryPath);
		if (_userDataDirectoryPath[len - 1] != separator[0]) {
			strcat(_userDataDirectoryPath, separator);
		}
		return;
	}
	char buffer[MAX_PATH] = { 0 };
	platform_get_exe_path(buffer);

	strncat(buffer, separator, MAX_PATH - strnlen(buffer, MAX_PATH) - 1);
	strncat(buffer, "data", MAX_PATH - strnlen(buffer, MAX_PATH) - 1);

	if (platform_directory_exists(buffer))
	{
		_openrctDataDirectoryPath[0] = '\0';
		safe_strcpy(_openrctDataDirectoryPath, buffer, sizeof(_openrctDataDirectoryPath));
		return;
	} else {
		log_fatal("Unable to resolve openrct data path.");
		exit(-1);
	}
}

void platform_get_openrct_data_path(utf8 *outPath)
{
	safe_strcpy(outPath, _openrctDataDirectoryPath, sizeof(_openrctDataDirectoryPath));
}

/**
 * Default directory fallback is:
 *   - (command line argument)
 *   - C:\Users\%USERNAME%\OpenRCT2 (as from SHGetFolderPathW)
 */
void platform_resolve_user_data_path()
{
	wchar_t wOutPath[MAX_PATH];
	const char separator[2] = { platform_get_path_separator(), 0 };

	if (gCustomUserDataPath[0] != 0) {
		wchar_t *customUserDataPathW = utf8_to_widechar(gCustomUserDataPath);
		if (GetFullPathNameW(customUserDataPathW, countof(wOutPath), wOutPath, NULL) == 0) {
			log_fatal("Unable to resolve path '%s'.", gCustomUserDataPath);
			exit(-1);
		}
		utf8 *outPathTemp = widechar_to_utf8(wOutPath);
		safe_strcpy(_userDataDirectoryPath, outPathTemp, sizeof(_userDataDirectoryPath));
		free(outPathTemp);
		free(customUserDataPathW);

		// Ensure path ends with separator
		size_t len = strlen(_userDataDirectoryPath);
		if (_userDataDirectoryPath[len - 1] != separator[0]) {
			strcat(_userDataDirectoryPath, separator);
		}
		return;
	}

	if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_PERSONAL | CSIDL_FLAG_CREATE, NULL, 0, wOutPath))) {
		utf8 *outPathTemp = widechar_to_utf8(wOutPath);
		safe_strcpy(_userDataDirectoryPath, outPathTemp, sizeof(_userDataDirectoryPath));
		free(outPathTemp);

		strcat(_userDataDirectoryPath, separator);
		strcat(_userDataDirectoryPath, "OpenRCT2");
		strcat(_userDataDirectoryPath, separator);
	} else {
		log_fatal("Unable to resolve user data path.");
		exit(-1);
	}
}

void platform_get_user_directory(utf8 *outPath, const utf8 *subDirectory)
{
	const char separator[2] = { platform_get_path_separator(), 0 };

	strcpy(outPath, _userDataDirectoryPath);
	if (subDirectory != NULL && subDirectory[0] != 0) {
		strcat(outPath, subDirectory);
		strcat(outPath, separator);
	}
}

void platform_show_messagebox(utf8 *message)
{
	MessageBoxA(windows_get_window_handle(), message, "OpenRCT2", MB_OK);
}

/**
 *
 *  rct2: 0x004080EA
 */
bool platform_open_common_file_dialog(utf8 *outFilename, file_dialog_desc *desc)
{
	OPENFILENAMEW openFileName;
	wchar_t wcFilename[MAX_PATH];

	// Copy default filename to result filename buffer
	if (desc->default_filename == NULL) {
		wcFilename[0] = 0;
	} else {
		wchar_t *wcDefaultFilename = utf8_to_widechar(desc->default_filename);
		lstrcpyW(wcFilename, wcDefaultFilename);
		free(wcDefaultFilename);
	}

	// Set open file name options
	memset(&openFileName, 0, sizeof(OPENFILENAMEW));
	openFileName.lStructSize = sizeof(OPENFILENAMEW);
	openFileName.hwndOwner = windows_get_window_handle();
	openFileName.nMaxFile = MAX_PATH;
	openFileName.lpstrTitle = utf8_to_widechar(desc->title);
	openFileName.lpstrInitialDir = utf8_to_widechar(desc->initial_directory);
	openFileName.lpstrFile = wcFilename;

	utf8 filters[256];
	utf8 *ch = filters;
	for (int i = 0; i < countof(desc->filters); i++) {
		if (desc->filters[i].name != NULL) {
			strcpy(ch, desc->filters[i].name);
			ch = strchr(ch, 0) + 1;
			strcpy(ch, desc->filters[i].pattern);
			ch = strchr(ch, 0) + 1;
		}
	}
	assert(ch != filters);
	*ch = 0;

	// HACK: Replace all null terminators with 0x01 so that we convert the entire string
	size_t fullLength = (size_t)(ch - filters);
	for (size_t i = 0; i < fullLength; i++) {
		if (filters[i] == '\0') {
			filters[i] = 1;
		}
	}
	wchar_t *wcFilter = utf8_to_widechar(filters);
	fullLength = lstrlenW(wcFilter);
	for (size_t i = 0; i < fullLength; i++) {
		if (wcFilter[i] == 1) {
			wcFilter[i] = '\0';
		}
	}

	openFileName.lpstrFilter = wcFilter;

	// Open dialog
	BOOL result = false;
	DWORD commonFlags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
	if (desc->type == FD_OPEN) {
		openFileName.Flags = commonFlags | OFN_NONETWORKBUTTON | OFN_FILEMUSTEXIST;
		result = GetOpenFileNameW(&openFileName);
	} else if (desc->type == FD_SAVE) {
		openFileName.Flags = commonFlags | OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT;
		result = GetSaveFileNameW(&openFileName);
	}

	// Clean up
	free((void*)openFileName.lpstrTitle);
	free((void*)openFileName.lpstrInitialDir);
	free((void*)openFileName.lpstrFilter);

	if (result) {
		utf8 *resultFilename = widechar_to_utf8(openFileName.lpstrFile);
		strcpy(outFilename, resultFilename);
		free(resultFilename);

		// If there is no extension, append the pattern
		const utf8 *outFilenameExtension = path_get_extension(outFilename);
		if (str_is_null_or_empty(outFilenameExtension)) {
			int filterIndex = openFileName.nFilterIndex - 1;

			assert(filterIndex >= 0);
			assert(filterIndex < countof(desc->filters));

			const utf8 *pattern = desc->filters[filterIndex].pattern;
			const utf8 *patternExtension = path_get_extension(pattern);
			if (!str_is_null_or_empty(patternExtension)) {
				strcat(outFilename, patternExtension);
			}
		}
	}

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
	HKEY hKey;
	DWORD type, size;

	strcpy(subkeyInfogrames, "Software\\Infogrames\\");
	strcat(subkeyInfogrames, source);
	strcpy(subkeyFishTechGroup, "Software\\Fish Technology Group\\");
	strcat(subkeyFishTechGroup, source);

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
	HWND handle = wmInfo.info.win.window;
	#ifdef __MINGW32__
	assert(sizeof(HWND) == sizeof(uint32));
	uint8 A = (uint32)handle & 0xff000000 >> 24;
	uint8 B = (uint32)handle & 0xff0000 >> 16;
	uint8 C = (uint32)handle & 0xff00 >> 8;
	uint8 D = (uint32)handle & 0xff;
	HWND result = (HWND)(D << 24 | A << 16 | B << 8 | C);
	log_warning("twisting bits of handle, a workaround for mingw/sdl bug");
	#else
	HWND result = handle;
	#endif // __MINGW32__
	return result;
}

void platform_init_window_icon()
{
	if (_dllModule != NULL) {
		HICON icon = LoadIcon(_dllModule, MAKEINTRESOURCE(IDI_ICON));
		if (icon != NULL) {
			HWND hwnd = windows_get_window_handle();
			SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)icon);
		}
	}
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

uint8 platform_get_locale_currency()
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

uint8 platform_get_locale_measurement_format()
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

bool platform_check_steam_overlay_attached()
{
	return GetModuleHandle("GameOverlayRenderer.dll") != NULL;
}

char *strndup(const char *src, size_t size)
{
	size_t len = strnlen(src, size);
	char *dst = (char *)malloc(len + 1);

	if (dst == NULL)
	{
		return NULL;
	}

	dst = memcpy(dst, src, len);
	dst[len] = '\0';
	return (char *)dst;
}

void platform_get_exe_path(utf8 *outPath)
{
	wchar_t exePath[MAX_PATH];
	wchar_t tempPath[MAX_PATH];
	wchar_t *exeDelimiter;
	int exeDelimiterIndex;

	GetModuleFileNameW(NULL, exePath, MAX_PATH);
	exeDelimiter = wcsrchr(exePath, platform_get_path_separator());
	exeDelimiterIndex = (int)(exeDelimiter - exePath);
	lstrcpynW(tempPath, exePath, exeDelimiterIndex + 1);
	tempPath[exeDelimiterIndex] = L'\0';
	_wfullpath(exePath, tempPath, MAX_PATH);
	WideCharToMultiByte(CP_UTF8, 0, exePath, countof(exePath), outPath, MAX_PATH, NULL, NULL);
}

bool platform_get_font_path(TTFFontDescriptor *font, utf8 *buffer)
{
#if !defined(__MINGW32__) && ((NTDDI_VERSION >= NTDDI_VISTA) && !defined(_USING_V110_SDK71_) && !defined(_ATL_XP_TARGETING))
	wchar_t *fontFolder;
	if (SUCCEEDED(SHGetKnownFolderPath(&FOLDERID_Fonts, 0, NULL, &fontFolder)))
	{
		// Convert wchar to utf8, then copy the font folder path to the buffer.
		utf8 *outPathTemp = widechar_to_utf8(fontFolder);
		strcpy(buffer, outPathTemp);
		free(outPathTemp);

		CoTaskMemFree(fontFolder);

		// Append the requested font's file name.
		const char separator[2] = { platform_get_path_separator(), 0 };
		strcat(buffer, separator);
		strcat(buffer, font->filename);
		return true;
	}
	else
	{
		return false;
	}
#else
	log_warning("Compatibility hack: falling back to C:\\Windows\\Fonts");
	strcpy(buffer, "C:\\Windows\\Fonts\\");
	strcat(buffer, font->filename);
	return true;
#endif
}

datetime64 platform_get_datetime_now_utc()
{
	// Get file time
	FILETIME fileTime;
	GetSystemTimeAsFileTime(&fileTime);
	uint64 fileTime64 = ((uint64)fileTime.dwHighDateTime << 32ULL) | ((uint64)fileTime.dwLowDateTime);

	// File time starts from: 1601-01-01T00:00:00Z
	// Convert to start from: 0001-01-01T00:00:00Z
	datetime64 utcNow = fileTime64 - 504911232000000000ULL;
	return utcNow;
}

utf8* platform_get_username() {
	static char username[UNLEN + 1];

	DWORD usernameLength = UNLEN + 1;
	if (!GetUserName(username, &usernameLength)) {
		return NULL;
	}

	return username;
}

#ifndef __MINGW32__
///////////////////////////////////////////////////////////////////////////////
// File association setup
///////////////////////////////////////////////////////////////////////////////

#define SOFTWARE_CLASSES L"Software\\Classes"

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
	const uint32 iconIndex
) {
	wchar_t exePathW[MAX_PATH];
	wchar_t dllPathW[MAX_PATH];

	GetModuleFileNameW(NULL, exePathW, sizeof(exePathW));
	GetModuleFileNameW(_dllModule, dllPathW, sizeof(dllPathW));

	wchar_t *extensionW = utf8_to_widechar(extension);
	wchar_t *fileTypeTextW = utf8_to_widechar(fileTypeText);
	wchar_t *commandTextW = utf8_to_widechar(commandText);
	wchar_t *commandArgsW = utf8_to_widechar(commandArgs);

	wchar_t progIdNameW[128];
	get_progIdName(progIdNameW, extension);

	bool result = false;

	// [HKEY_CURRENT_USER\Software\Classes]
	HKEY hRootKey = NULL;
	if (RegOpenKeyW(HKEY_CURRENT_USER, SOFTWARE_CLASSES, &hRootKey) != ERROR_SUCCESS) {
		goto fail;
	}

	// [hRootKey\.ext]
	if (RegSetValueW(hRootKey, extensionW, REG_SZ, progIdNameW, 0) != ERROR_SUCCESS) {
		goto fail;
	}

	HKEY hKey = NULL;
	if (RegCreateKeyW(hRootKey, progIdNameW, &hKey) != ERROR_SUCCESS) {
		goto fail;
	}

	// [hRootKey\OpenRCT2.ext]
	if (RegSetValueW(hKey, NULL, REG_SZ, fileTypeTextW, 0) != ERROR_SUCCESS) {
		goto fail;
	}
	// [hRootKey\OpenRCT2.ext\DefaultIcon]
	wchar_t szIconW[MAX_PATH];
	wsprintfW(szIconW, L"\"%s\",%d", dllPathW, iconIndex);
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
	wsprintfW(szCommandW, L"\"%s\" %s", exePathW, commandArgsW);
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
#endif
#endif
