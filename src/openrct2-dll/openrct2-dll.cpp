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
#include <openrct2/audio/AudioContext.h>
#include <openrct2/Context.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/ui/UiContext.h>
#include <openrct2-ui/UiContext.h>

using namespace OpenRCT2;
using namespace OpenRCT2::Audio;
using namespace OpenRCT2::Ui;

#define DLLEXPORT extern "C" __declspec(dllexport)

static char * * GetCommandLineArgs(int argc, wchar_t * * argvW);
static void FreeCommandLineArgs(int argc, char * * argv);
static char * ConvertUTF16toUTF8(const wchar_t * src);

DLLEXPORT int LaunchOpenRCT2(int argc, wchar_t * * argvW)
{
    char * * argv = GetCommandLineArgs(argc, argvW);
    if (argv == nullptr)
    {
        puts("Unable to fetch command line arguments.");
        return -1;
    }

    IAudioContext * audioContext = CreateAudioContext();
    IUiContext * uiContext = CreateUiContext();
    IContext * context = OpenRCT2::CreateContext(audioContext, uiContext);

    int exitCode = context->RunOpenRCT2(argc, argv);

    delete context;
    delete uiContext;
    delete audioContext;

    FreeCommandLineArgs(argc, argv);
    return exitCode;
}

static char * * GetCommandLineArgs(int argc, wchar_t * * argvW)
{
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

    return argv;
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
