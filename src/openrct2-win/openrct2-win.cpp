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
#include <openrct2-ui/Ui.h>
#include <openrct2-ui/UiContext.h>
#include <openrct2-ui/audio/AudioContext.h>
#include <openrct2/Context.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/audio/AudioContext.h>
#include <openrct2/ui/UiContext.h>
#include <shellapi.h>
#include <stdio.h>
#include <stdlib.h>

using namespace OpenRCT2;
using namespace OpenRCT2::Audio;
using namespace OpenRCT2::Ui;

static char** GetCommandLineArgs(int argc, wchar_t** argvW);
static void FreeCommandLineArgs(int argc, char** argv);
static char* ConvertWideChartoUTF8(const wchar_t* src);

/**
 * Windows entry point to OpenRCT2 with a console window using a traditional C main function.
 */
int wmain(int argc, wchar_t** argvW, [[maybe_unused]] wchar_t* envp)
{
    char** argv = GetCommandLineArgs(argc, argvW);
    if (argv == nullptr)
    {
        puts("Unable to fetch command line arguments.");
        return -1;
    }

    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    int exitCode = NormalisedMain(argc, const_cast<const char**>(argv));

    FreeCommandLineArgs(argc, argv);
    return exitCode;
}

static char** GetCommandLineArgs(int argc, wchar_t** argvW)
{
    // Allocate UTF-8 strings
    char** argv = (char**)malloc(argc * sizeof(char*));
    if (argv == nullptr)
    {
        return nullptr;
    }

    // Convert to UTF-8
    for (int i = 0; i < argc; i++)
    {
        argv[i] = ConvertWideChartoUTF8(argvW[i]);
    }

    return argv;
}

static void FreeCommandLineArgs(int argc, char** argv)
{
    // Free argv
    for (int i = 0; i < argc; i++)
    {
        free(argv[i]);
    }
    free(argv);
}

static char* ConvertWideChartoUTF8(const wchar_t* src)
{
    int srcLen = lstrlenW(src);
    int sizeReq = WideCharToMultiByte(CP_UTF8, 0, src, srcLen, nullptr, 0, nullptr, nullptr);
    char* result = (char*)calloc(1, sizeReq + 1);
    WideCharToMultiByte(CP_UTF8, 0, src, srcLen, result, sizeReq, nullptr, nullptr);
    return result;
}
