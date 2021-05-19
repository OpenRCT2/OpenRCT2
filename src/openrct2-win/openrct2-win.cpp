/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
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
#include <shellapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

static std::vector<std::string> GetCommandLineArgs(const wchar_t* cmdLine);
static void CreateConsole();

/**
 * Windows entry point to OpenRCT2 for a GUI application, calling to the traditional C main function.
 */
int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nShowCmd*/)
{
    auto argvStrings = GetCommandLineArgs(GetCommandLineW());

    // Handle a special Windows-only commandline argument "--console". If present, create our own console and point stdin/stdout
    // at that.
#ifdef DEBUG
    constexpr bool alwaysCreateConsole = true;
#else
    constexpr bool alwaysCreateConsole = false;
#endif

    auto consoleArg = std::find(argvStrings.begin(), argvStrings.end(), "--console");
    if (consoleArg != argvStrings.end())
    {
        CreateConsole();

        // Remove this argument so the game doesn't throw an unknown argument error.
        argvStrings.erase(consoleArg);
    }
    else if constexpr (alwaysCreateConsole)
    {
        CreateConsole();
    }

    std::vector<const char*> argv;
    std::transform(
        argvStrings.begin(), argvStrings.end(), std::back_inserter(argv), [](const auto& string) { return string.c_str(); });
    argv.push_back(nullptr);
    auto exitCode = main(static_cast<int>(argvStrings.size()), argv.data());
    return exitCode;
}

static std::vector<std::string> GetCommandLineArgs(const wchar_t* cmdLine)
{
    // Allocate UTF-8 strings
    std::vector<std::string> argv;
    int numArgs;
    LPWSTR* args = CommandLineToArgvW(cmdLine, &numArgs);
    if (args != nullptr)
    {
        for (int i = 0; i < numArgs; i++)
        {
            argv.push_back(String::ToUtf8(args[i]));
        }
    }
    LocalFree(args);
    return argv;
}

static void CreateConsole()
{
    if (AllocConsole())
    {
        FILE* dummyFile;
        freopen_s(&dummyFile, "CONIN$", "r", stdin);
        freopen_s(&dummyFile, "CONOUT$", "w", stderr);
        freopen_s(&dummyFile, "CONOUT$", "w", stdout);

        SetConsoleCP(CODE_PAGE::CP_UTF8);
        SetConsoleOutputCP(CODE_PAGE::CP_UTF8);
    }
}
