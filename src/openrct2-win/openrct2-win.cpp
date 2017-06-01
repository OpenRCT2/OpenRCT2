#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

// Enable visual styles
#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define DLLIMPORT extern "C"
DLLIMPORT int LaunchOpenRCT2(int argc, wchar_t * * argv);

/**
 * Windows entry point to OpenRCT2 with a console window using a traditional C main function.
 */
int wmain(int argc, wchar_t * * argvW, wchar_t * envp)
{
    return LaunchOpenRCT2(argc, argvW);
}
