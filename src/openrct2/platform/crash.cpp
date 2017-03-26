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

#include "crash.h"

#ifdef USE_BREAKPAD
#include <memory>
#include <stdio.h>

#if defined(_WIN32)
    #include <breakpad/client/windows/handler/exception_handler.h>
    #include <string>
    #include <ShlObj.h>
#else
    #error Breakpad support not implemented yet for this platform
#endif

extern "C"
{
    #include "../localisation/language.h"
    #include "../scenario/scenario.h"
    #include "platform.h"
}

#include "../core/Console.hpp"
#include "../core/Exception.hpp"
#include "../rct2/S6Exporter.h"
#include "../Version.h"

#define WSZ(x) L"" x

#ifdef OPENRCT2_COMMIT_SHA1_SHORT
    const wchar_t *_wszCommitSha1Short = WSZ(OPENRCT2_COMMIT_SHA1_SHORT);
#else
    const wchar_t *_wszCommitSha1Short = WSZ("");
#endif

// OPENRCT2_ARCHITECTURE is required to be defined in version.h
const wchar_t *_wszArchitecture = WSZ(OPENRCT2_ARCHITECTURE);

static bool OnCrash(const wchar_t * dumpPath,
                    const wchar_t * miniDumpId,
                    void * context,
                    EXCEPTION_POINTERS * exinfo,
                    MDRawAssertionInfo * assertion,
                    bool succeeded)
{
    if (!succeeded)
    {
        constexpr const char * DumpFailedMessage = "Failed to create the dump. Please file an issue with OpenRCT2 on GitHub and provide latest save, and provide information about what you did before the crash occurred.";
        printf("%s\n", DumpFailedMessage);
        if (!gOpenRCT2SilentBreakpad)
        {
            MessageBoxA(NULL, DumpFailedMessage, OPENRCT2_NAME, MB_OK | MB_ICONERROR);
        }
        return succeeded;
    }

    // Get filenames
    wchar_t dumpFilePath[MAX_PATH];
    wchar_t saveFilePath[MAX_PATH];
    swprintf_s(dumpFilePath, sizeof(dumpFilePath), L"%s%s.dmp", dumpPath, miniDumpId);
    swprintf_s(saveFilePath, sizeof(saveFilePath), L"%s%s.sv6", dumpPath, miniDumpId);

    // Try to rename the files
    wchar_t dumpFilePathNew[MAX_PATH];
    swprintf_s(dumpFilePathNew, sizeof(dumpFilePathNew), L"%s%s(%s_%s).dmp", dumpPath, miniDumpId, _wszCommitSha1Short, _wszArchitecture);
    if (_wrename(dumpFilePath, dumpFilePathNew) == 0)
    {
        std::wcscpy(dumpFilePath, dumpFilePathNew);
    }

    // Log information to output
    wprintf(L"Dump Path: %s\n", dumpPath);
    wprintf(L"Dump File Path: %s\n", dumpFilePath);
    wprintf(L"Dump Id: %s\n", miniDumpId);
    wprintf(L"Version: %s\n", WSZ(OPENRCT2_VERSION));
    wprintf(L"Commit: %s\n", _wszCommitSha1Short);

    bool savedGameDumped = false;
    utf8 * saveFilePathUTF8 = widechar_to_utf8(saveFilePath);
    try
    {
        auto exporter = std::make_unique<S6Exporter>();
        exporter->Export();
        exporter->SaveGame(saveFilePathUTF8);
        savedGameDumped = true;
    }
    catch (const Exception &)
    {
    }
    free(saveFilePathUTF8);

    if (gOpenRCT2SilentBreakpad)
    {
        return succeeded;
    }

    constexpr const wchar_t * MessageFormat = L"A crash has occurred and a dump was created at\n%s.\n\nPlease file an issue with OpenRCT2 on GitHub, and provide the dump and saved game there.\n\nVersion: %s\nCommit: %s";
    wchar_t message[MAX_PATH * 2];
    swprintf_s(message,
               MessageFormat,
               dumpFilePath,
               WSZ(OPENRCT2_VERSION),
               _wszCommitSha1Short);

    // Cannot use platform_show_messagebox here, it tries to set parent window already dead.
    MessageBoxW(NULL, message, WSZ(OPENRCT2_NAME), MB_OK | MB_ICONERROR);
    HRESULT coInitializeResult = CoInitialize(NULL);
    if (SUCCEEDED(coInitializeResult))
    {
        LPITEMIDLIST pidl = ILCreateFromPathW(dumpPath);
        LPITEMIDLIST files[2];
        uint32 numFiles = 0;

        files[numFiles++] = ILCreateFromPathW(dumpFilePath);
        if (savedGameDumped)
        {
            files[numFiles++] = ILCreateFromPathW(saveFilePath);
        }
        if (pidl != nullptr) {
            SHOpenFolderAndSelectItems(pidl, numFiles, (LPCITEMIDLIST *)files, 0);
            ILFree(pidl);
            for (uint32 i = 0; i < numFiles; i++)
            {
                ILFree(files[i]);
            }
        }
        CoUninitialize();
    }

    // Return whether the dump was successful
    return succeeded;
}

static std::wstring GetDumpDirectory()
{
    char userDirectory[MAX_PATH];
    platform_get_user_directory(userDirectory, NULL, sizeof(userDirectory));

    wchar_t * userDirectoryW = utf8_to_widechar(userDirectory);
    auto result = std::wstring(userDirectoryW);
    free(userDirectoryW);

    return result;
}

// Using non-null pipe name here lets breakpad try setting OOP crash handling
constexpr const wchar_t * PipeName = L"openrct2-bpad";

#endif // USE_BREAKPAD

extern "C" CExceptionHandler crash_init()
{
#ifdef USE_BREAKPAD
    // Path must exist and be RW!
    auto exHandler = new google_breakpad::ExceptionHandler(
        GetDumpDirectory(),
        0,
        OnCrash,
        0,
        google_breakpad::ExceptionHandler::HANDLER_ALL,
        MiniDumpWithDataSegs,
        PipeName,
        0);
    return reinterpret_cast<CExceptionHandler>(exHandler);
#else // USE_BREAKPAD
    return nullptr;
#endif // USE_BREAKPAD
}
