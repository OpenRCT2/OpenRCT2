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

#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <shellapi.h>

// Enable visual styles
#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

static bool GetCommandLineArgs(int * outargc, char * * * outargv);
static void FreeCommandLineArgs(int argc, char * * argv);
static char * ConvertUTF16toUTF8(const wchar_t * src);

/**
 * Windows entry point to OpenRCT2 with a console window using a traditional C main function.
 */
int main(int argc, char * * argv)
{
    if (!GetCommandLineArgs(&argc, &argv))
    {
        puts("Unable to fetch command line arguments.");
        return -1;
    }

    int exitCode = 0; // RunOpenRCT2

    FreeCommandLineArgs(argc, argv);
    return exitCode;
}

/**
 * Windows entry point to OpenRCT2 without a console window.
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    int argc;
    char * * argv;
    if (!GetCommandLineArgs(&argc, &argv))
    {
        puts("Unable to fetch command line arguments.");
        return -1;
    }

    int exitCode = 0; // RunOpenRCT2

    FreeCommandLineArgs(argc, argv);
    return exitCode;
}

static bool GetCommandLineArgs(int * outargc, char * * * outargv)
{
    // Get UTF-16 command line arguments
    int argc;
    LPWSTR cmdLine = GetCommandLineW();
    LPWSTR * argvW = CommandLineToArgvW(cmdLine, &argc);

    // Allocate UTF-8 strings
    char * * argv = (char * *)malloc(argc * sizeof(char *));
    if (argv == nullptr)
    {
        return false;
    }

    // Convert to UTF-8
    for (int i = 0; i < argc; i++)
    {
        argv[i] = ConvertUTF16toUTF8(argvW[i]);
    }

    // Free UTF-16 strings
    LocalFree(argvW);

    *outargc = argc;
    *outargv = argv;
    return true;
}

static void FreeCommandLineArgs(int argc, char * * argv)
{
    // Free argv
    for (int i = 0; i < argc; i++)
    {
        free(argv[i]);
    }
    free(argv);
}

static char * ConvertUTF16toUTF8(const wchar_t * src)
{
    int srcLen = lstrlenW(src);
    int sizeReq = WideCharToMultiByte(CP_UTF8, 0, src, srcLen, nullptr, 0, nullptr, nullptr);
    char * result = (char *)calloc(1, sizeReq + 1);
    WideCharToMultiByte(CP_UTF8, 0, src, srcLen, result, sizeReq, nullptr, nullptr);
    return result;
}
