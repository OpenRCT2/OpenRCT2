/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

// Windows.h needs to be included first
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef CreateWindow

// Enable visual styles
#pragma comment(                                                                                                               \
    linker,                                                                                                                    \
    "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// Then the rest
#include <cstdio>
#include <cstdlib>
#include <openrct2-ui/Ui.h>
#include <openrct2/core/String.hpp>

static void ReattachConsole()
{
    AttachConsole(MAXDWORD);
    std::freopen("CONOUT$", "w", stdout);
    std::freopen("CONOUT$", "w", stderr);
    std::freopen("CONIN$", "r", stdin);
}

/**
 * Windows entry point to OpenRCT2 with a console window using a traditional C main function.
 */
int wmain(int argc, wchar_t** argvW, [[maybe_unused]] wchar_t* envp)
{
    auto stdoutWanted = false;
    std::vector<std::string> args;
    try
    {
        for (int i = 0; i < argc; i++)
        {
            std::wstring_view s(argvW[i]);
            if (s == L"--console")
            {
                stdoutWanted = true;
            }
            else
            {
                args.push_back(String::ToUtf8(s));
            }
        }
    }
    catch (...)
    {
        std::puts("Unable to fetch command line arguments.");
        return -1;
    }

    if (stdoutWanted)
    {
        // Windows subsytem will detach stdin/stdout, so reattach
        ReattachConsole();
    }

    SetConsoleCP(CODE_PAGE::CP_UTF8);
    SetConsoleOutputCP(CODE_PAGE::CP_UTF8);

    std::vector<const char*> argv(args.size());
    for (int i = 0; i < args.size(); i++)
    {
        argv[i] = args[i].c_str();
    }
    int exitCode = NormalisedMain(static_cast<int>(argv.size()), argv.data());
    return exitCode;
}
