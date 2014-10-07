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
// __declspec(dllexport) int StartOpenRCT(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
// {
// }

char platform_get_path_separator()
{
	return '\\';
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

#endif