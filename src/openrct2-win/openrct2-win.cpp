/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

// Enable visual styles
#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define DLLIMPORT extern "C"
DLLIMPORT int LaunchOpenRCT2(int argc, wchar_t * * argv);

/**
 * Windows entry point to OpenRCT2 with a console window using a traditional C main function.
 */
int wmain(int argc, wchar_t * * argvW, [[maybe_unused]] wchar_t * envp)
{
    return LaunchOpenRCT2(argc, argvW);
}
