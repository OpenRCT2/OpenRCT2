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
#include "../addresses.h"
#include "../cmdline.h"
#include "../openrct2.h"
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
	//LocalFree(argv);

	if (runGame)
		openrct2_launch();

	return gExitCode;
}

char platform_get_path_separator()
{
	return '\\';
}

int platform_file_exists(const char *path)
{
	return !(GetFileAttributes(path) == INVALID_FILE_ATTRIBUTES && GetLastError() == ERROR_FILE_NOT_FOUND);
}

int platform_directory_exists(const char *path)
{
	DWORD dwAttrib = GetFileAttributes(path);
	return dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

int platform_ensure_directory_exists(const char *path)
{
	if (platform_directory_exists(path))
		return 1;

	return CreateDirectory(path, NULL);
}

int platform_lock_single_instance()
{
	HANDLE mutex, status;

	// Check if operating system mutex exists
	mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, SINGLE_INSTANCE_MUTEX_NAME);
	if (mutex == NULL) {
		// Create new mutex
		status = CreateMutex(NULL, FALSE, SINGLE_INSTANCE_MUTEX_NAME);
		if (status == NULL)
			fprintf(stderr, "unable to create mutex\n");

		return 1;
	} else {
		// Already running
		CloseHandle(mutex);
		return 0;
	}
}

typedef struct {
	char active;
	char pattern[MAX_PATH];
	HANDLE handle;
	WIN32_FIND_DATAA data;
} enumerate_file_info;
static enumerate_file_info _enumerateFileInfoList[8] = { 0 };

int platform_enumerate_files_begin(const char *pattern)
{
	int i;
	enumerate_file_info *enumFileInfo;

	for (i = 0; i < countof(_enumerateFileInfoList); i++) {
		enumFileInfo = &_enumerateFileInfoList[i];
		if (!enumFileInfo->active) {
			strncpy(enumFileInfo->pattern, pattern, MAX_PATH);
			enumFileInfo->handle = NULL;
			enumFileInfo->active = 1;
			return i;
		}
	}

	return INVALID_HANDLE;
}

int platform_enumerate_files_next(int handle, file_info *outFileInfo)
{
	int result;
	enumerate_file_info *enumFileInfo;
	HANDLE findFileHandle;

	enumFileInfo = &_enumerateFileInfoList[handle];

	if (enumFileInfo->handle == NULL) {
		findFileHandle = FindFirstFile(enumFileInfo->pattern, &enumFileInfo->data);
		if (findFileHandle != INVALID_HANDLE_VALUE) {
			enumFileInfo->handle = findFileHandle;
			result = 1;
		} else {
			result = 0;
		}
	} else {
		result = FindNextFile(enumFileInfo->handle, &enumFileInfo->data);
	}

	if (result) {
		outFileInfo->path = enumFileInfo->data.cFileName;
		outFileInfo->size = ((uint64)enumFileInfo->data.nFileSizeHigh << 32ULL) | (uint64)enumFileInfo->data.nFileSizeLow;
		outFileInfo->last_modified = ((uint64)enumFileInfo->data.ftLastWriteTime.dwHighDateTime << 32ULL) | (uint64)enumFileInfo->data.ftLastWriteTime.dwLowDateTime;
		return 1;
	} else {
		return 0;
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
	enumFileInfo->active = 0;
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
		i + (len + 2)*sizeof(CHAR));

	_argv = (PCHAR)(((PUCHAR)argv) + i);

	argc = 0;
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

#endif