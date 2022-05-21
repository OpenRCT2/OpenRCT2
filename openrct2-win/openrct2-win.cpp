/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

// Windows.h needs to be included first
#include <windows.h>

// Enable visual styles
#pragma comment(                                                                                                               \
    linker,                                                                                                                    \
    "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// Then the rest
#include <algorithm>
#include <iterator>
#include <openrct2-ui/Ui.h>
#include <openrct2/core/String.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

static std::vector<std::string> GetCommandLineArgs(int argc, wchar_t** argvW);

/**
 * Windows entry point to OpenRCT2 with a console window using a traditional C main function.
 */
int wmain(int argc, wchar_t** argvW, [[maybe_unused]] wchar_t* envp)
{
    auto argvStrings = GetCommandLineArgs(argc, argvW);

    SetConsoleCP(OpenRCT2::CodePage::UTF8);
    SetConsoleOutputCP(OpenRCT2::CodePage::UTF8);

    std::vector<const char*> argv;
    std::transform(
        argvStrings.begin(), argvStrings.end(), std::back_inserter(argv), [](const auto& string) { return string.c_str(); });
    auto exitCode = NormalisedMain(argc, argv.data());
    return exitCode;
}

static std::vector<std::string> GetCommandLineArgs(int argc, wchar_t** argvW)
{
    // Allocate UTF-8 strings
    std::vector<std::string> argv;
    for (int i = 0; i < argc; i++)
    {
        argv.push_back(String::ToUtf8(argvW[i]));
    }
    return argv;
}
