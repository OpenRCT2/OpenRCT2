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

// Enable visual styles
#pragma comment(                                                                                                               \
    linker,                                                                                                                    \
    "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// Then the rest
#include <memory>
#include <openrct2-ui/Ui.h>
#include <stdio.h>
#include <stdlib.h>

static std::unique_ptr<char*[]> GetCommandLineArgs(int argc, wchar_t** argvW);
static char* ConvertWideChartoUTF8(const wchar_t* src);

/**
 * Windows entry point to OpenRCT2 with a console window using a traditional C main function.
 */
int wmain(int argc, wchar_t** argvW, [[maybe_unused]] wchar_t* envp)
{
    auto argv = GetCommandLineArgs(argc, argvW);
    if (argv == nullptr)
    {
        puts("Unable to fetch command line arguments.");
        return -1;
    }

    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    auto exitCode = NormalisedMain(argc, const_cast<const char**>(argv.get()));
    return exitCode;
}

static std::unique_ptr<char* []> GetCommandLineArgs(int argc, wchar_t** argvW) {
    // Allocate UTF-8 strings
    auto argv = std::make_unique<char*[]>(argc);
    if (argv != nullptr)
    {
        // Convert to UTF-8
        for (int i = 0; i < argc; i++)
        {
            argv[i] = ConvertWideChartoUTF8(argvW[i]);
        }
    }
    return argv;
}

static char* ConvertWideChartoUTF8(const wchar_t* src)
{
    auto srcLen = lstrlenW(src);
    auto sizeReq = WideCharToMultiByte(CP_UTF8, 0, src, srcLen, nullptr, 0, nullptr, nullptr);
    auto result = static_cast<char*>(calloc(1, static_cast<size_t>(sizeReq) + 1));
    WideCharToMultiByte(CP_UTF8, 0, src, srcLen, result, sizeReq, nullptr, nullptr);
    return result;
}
