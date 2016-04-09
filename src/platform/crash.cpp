#include <SDL_platform.h>
#include "crash.h"

#ifdef USE_BREAKPAD
#include <stdio.h>

#if defined(__WINDOWS__)
    #include <breakpad/client/windows/handler/exception_handler.h>
    #include <string>
    #include <ShlObj.h>
#elif defined(__LINUX__)
    #include <breakpad/client/linux/handler/exception_handler.h>
    #define BREAKPAD_PATH "/tmp"
#else
    #error Breakpad support not implemented yet for this platform
#endif

extern "C" {
    #include "../localisation/language.h"
    #include "../scenario.h"
    #include "platform.h"
}

#include "../core/Console.hpp"

#define WSZ(x) L"" x

static bool OnCrash(const wchar_t * dumpPath,
                    const wchar_t * miniDumpId,
                    void * context,
                    EXCEPTION_POINTERS * exinfo,
                    MDRawAssertionInfo * assertion,
                    bool succeeded)
{
    if (!succeeded)
    {
        constexpr char * DumpFailedMessage = "Failed to create the dump. Nothing left to do. Please file an issue with OpenRCT2 on Github and provide latest save.";
        printf("%s\n", DumpFailedMessage);
        MessageBoxA(NULL, DumpFailedMessage, OPENRCT2_NAME, MB_OK | MB_ICONERROR);
        return succeeded;
    }

    wchar_t dumpFilePath[MAX_PATH];
    wchar_t saveFilePath[MAX_PATH];
    wsprintfW(dumpFilePath, L"%s%s.dmp", dumpPath, miniDumpId);
    wsprintfW(saveFilePath, L"%s%s.sv6", dumpPath, miniDumpId);

    wprintf(L"Dump Path: %s\n", dumpFilePath);
    wprintf(L"Dump Id: %s\n", miniDumpId);
    wprintf(L"Version: %s\n", WSZ(OPENRCT2_VERSION));
    wprintf(L"Commit: %s\n", WSZ(OPENRCT2_COMMIT_SHA1_SHORT));

    utf8 * saveFilePathUTF8 = widechar_to_utf8(saveFilePath);
    SDL_RWops * rw = SDL_RWFromFile(saveFilePathUTF8, "wb+");
    free(saveFilePathUTF8);

    bool savedGameDumped = false;
    if (rw != NULL) {
        scenario_save(rw, 0x80000000);
        savedGameDumped = true;
        SDL_RWclose(rw);
    }

    constexpr wchar_t * MessageFormat = L"A crash has occurred and dump was created at\n%s.\n\nPlease create an issue with OpenRCT2 on Github and provide the dump and save.\n\nVersion: %s\nCommit: %s";
    wchar_t message[MAX_PATH * 2];
    swprintf_s(message,
               MessageFormat,
               dumpFilePath,
               WSZ(OPENRCT2_VERSION),
               WSZ(OPENRCT2_COMMIT_SHA1_SHORT));

    // Cannot use platform_show_messagebox here, it tries to set parent window already dead.
    MessageBoxW(NULL, message, WSZ(OPENRCT2_NAME), MB_OK | MB_ICONERROR);
    HRESULT coInitializeResult = CoInitialize(NULL);
    if (SUCCEEDED(coInitializeResult))
    {
        ITEMIDLIST * pidl = ILCreateFromPathW(dumpPath);
        ITEMIDLIST * files[2];
        uint32 numFiles = 0;

        files[numFiles++] = ILCreateFromPathW(dumpFilePath);
        if (savedGameDumped)
        {
            files[numFiles++] = ILCreateFromPathW(saveFilePath);
        }
        if (pidl != nullptr) {
            HRESULT result = SHOpenFolderAndSelectItems(pidl, numFiles, (LPCITEMIDLIST *)files, 0);
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
    platform_get_user_directory(userDirectory, NULL);

    wchar_t * userDirectoryW = utf8_to_widechar(userDirectory);
    auto result = std::wstring(userDirectoryW);
    free(userDirectoryW);

    return result;
}

#endif // USE_BREAKPAD

// Using non-null pipe name here lets breakpad try setting OOP crash handling
constexpr wchar_t * PipeName = L"openrct2-bpad";

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
