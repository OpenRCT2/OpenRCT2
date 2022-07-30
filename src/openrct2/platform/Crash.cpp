/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Crash.h"

#ifdef USE_BREAKPAD
#    include <iterator>
#    include <map>
#    include <memory>
#    include <stdio.h>

#    if defined(_WIN32)
#        include <ShlObj.h>
#        include <client/windows/handler/exception_handler.h>
#        include <common/windows/http_upload.h>
#        include <string>
#    else
#        error Breakpad support not implemented yet for this platform
#    endif

#    include "../Context.h"
#    include "../Game.h"
#    include "../OpenRCT2.h"
#    include "../PlatformEnvironment.h"
#    include "../Version.h"
#    include "../config/Config.h"
#    include "../core/Console.hpp"
#    include "../core/Guard.hpp"
#    include "../core/Path.hpp"
#    include "../core/String.hpp"
#    include "../interface/Screenshot.h"
#    include "../localisation/Language.h"
#    include "../object/ObjectManager.h"
#    include "../park/ParkFile.h"
#    include "../scenario/Scenario.h"
#    include "../util/SawyerCoding.h"
#    include "../util/Util.h"
#    include "Platform.h"

#    define WSZ(x) L"" x

#    ifdef OPENRCT2_COMMIT_SHA1_SHORT
const wchar_t* _wszCommitSha1Short = WSZ(OPENRCT2_COMMIT_SHA1_SHORT);
#    else
const wchar_t* _wszCommitSha1Short = WSZ("");
#    endif

// OPENRCT2_ARCHITECTURE is required to be defined in version.h
const wchar_t* _wszArchitecture = WSZ(OPENRCT2_ARCHITECTURE);

#    define BACKTRACE_TOKEN L"0ca992e20aca116b5e090fd2eaff6e7b5c8225f778fd8f4e77cb077d70329324"

using namespace OpenRCT2;

// Note: uploading gzipped crash dumps manually requires specifying
// 'Content-Encoding: gzip' header in HTTP request, but we cannot do that,
// so just hope the file name with '.gz' suffix is enough.
// For docs on uploading to backtrace.io check
// https://documentation.backtrace.io/product_integration_minidump_breakpad/
static bool UploadMinidump(const std::map<std::wstring, std::wstring>& files, int& error, std::wstring& response)
{
    for (const auto& file : files)
    {
        wprintf(L"files[%s] = %s\n", file.first.c_str(), file.second.c_str());
    }
    std::wstring url(L"https://openrct2.sp.backtrace.io:6098/"
                     L"post?format=minidump&token=" BACKTRACE_TOKEN);
    std::map<std::wstring, std::wstring> parameters;
    parameters[L"product_name"] = L"openrct2";
    parameters[L"version"] = String::ToWideChar(gVersionInfoFull);
    // In case of releases this can be empty
    if (wcslen(_wszCommitSha1Short) > 0)
    {
        parameters[L"commit"] = _wszCommitSha1Short;
    }
    else
    {
        parameters[L"commit"] = String::ToWideChar(gVersionInfoFull);
    }

    auto assertMsg = Guard::GetLastAssertMessage();
    if (assertMsg.has_value())
    {
        parameters[L"assert_failure"] = String::ToWideChar(assertMsg.value());
    }

    int timeout = 10000;
    bool success = google_breakpad::HTTPUpload::SendMultipartPostRequest(url, parameters, files, &timeout, &response, &error);
    wprintf(L"Success = %d, error = %d, response = %s\n", success, error, response.c_str());
    return success;
}

static bool OnCrash(
    const wchar_t* dumpPath, const wchar_t* miniDumpId, void* context, EXCEPTION_POINTERS* exinfo,
    MDRawAssertionInfo* assertion, bool succeeded)
{
    if (!succeeded)
    {
        constexpr const char* DumpFailedMessage = "Failed to create the dump. Please file an issue with OpenRCT2 on GitHub and "
                                                  "provide latest save, and provide "
                                                  "information about what you did before the crash occurred.";
        printf("%s\n", DumpFailedMessage);
        if (!gOpenRCT2SilentBreakpad)
        {
            MessageBoxA(nullptr, DumpFailedMessage, OPENRCT2_NAME, MB_OK | MB_ICONERROR);
        }
        return succeeded;
    }

    std::map<std::wstring, std::wstring> uploadFiles;

    // Get filenames
    wchar_t dumpFilePath[MAX_PATH];
    wchar_t saveFilePath[MAX_PATH];
    wchar_t configFilePath[MAX_PATH];
    wchar_t recordFilePathNew[MAX_PATH];
    swprintf_s(dumpFilePath, std::size(dumpFilePath), L"%s\\%s.dmp", dumpPath, miniDumpId);
    swprintf_s(saveFilePath, std::size(saveFilePath), L"%s\\%s.park", dumpPath, miniDumpId);
    swprintf_s(configFilePath, std::size(configFilePath), L"%s\\%s.ini", dumpPath, miniDumpId);
    swprintf_s(recordFilePathNew, std::size(recordFilePathNew), L"%s\\%s.parkrep", dumpPath, miniDumpId);

    wchar_t dumpFilePathNew[MAX_PATH];
    swprintf_s(
        dumpFilePathNew, std::size(dumpFilePathNew), L"%s\\%s(%s_%s).dmp", dumpPath, miniDumpId, _wszCommitSha1Short,
        _wszArchitecture);

    wchar_t dumpFilePathGZIP[MAX_PATH];
    swprintf_s(dumpFilePathGZIP, std::size(dumpFilePathGZIP), L"%s.gz", dumpFilePathNew);

    // Compress the dump
    {
        FILE* input = _wfopen(dumpFilePath, L"rb");
        FILE* dest = _wfopen(dumpFilePathGZIP, L"wb");

        if (util_gzip_compress(input, dest))
        {
            // TODO: enable upload of gzip-compressed dumps once supported on
            // backtrace.io (uncomment the line below). For now leave compression
            // on, as GitHub will accept .gz files, even though it does not
            // advertise it officially.

            /*
            uploadFiles[L"upload_file_minidump"] = dumpFilePathGZIP;
            */
        }
        fclose(input);
        fclose(dest);
    }

    bool with_record = stop_silent_record();

    // Try to rename the files
    if (_wrename(dumpFilePath, dumpFilePathNew) == 0)
    {
        std::wcscpy(dumpFilePath, dumpFilePathNew);
    }
    uploadFiles[L"upload_file_minidump"] = dumpFilePath;

    // Compress to gzip-compatible stream

    // Log information to output
    wprintf(L"Dump Path: %s\n", dumpPath);
    wprintf(L"Dump File Path: %s\n", dumpFilePath);
    wprintf(L"Dump Id: %s\n", miniDumpId);
    wprintf(L"Version: %s\n", WSZ(OPENRCT2_VERSION));
    wprintf(L"Commit: %s\n", _wszCommitSha1Short);

    bool savedGameDumped = false;
    auto saveFilePathUTF8 = String::ToUtf8(saveFilePath);
    try
    {
        PrepareMapForSave();

        // Export all loaded objects to avoid having custom objects missing in the reports.
        auto exporter = std::make_unique<ParkFileExporter>();
        auto ctx = OpenRCT2::GetContext();
        auto& objManager = ctx->GetObjectManager();
        exporter->ExportObjectsList = objManager.GetPackableObjects();

        exporter->Export(saveFilePathUTF8.c_str());
        savedGameDumped = true;
    }
    catch (const std::exception& e)
    {
        printf("Failed to export save. Error: %s\n", e.what());
    }

    // Compress the save
    if (savedGameDumped)
    {
        uploadFiles[L"attachment_park.park"] = saveFilePath;
    }

    auto configFilePathUTF8 = String::ToUtf8(configFilePath);
    if (config_save(configFilePathUTF8))
    {
        uploadFiles[L"attachment_config.ini"] = configFilePath;
    }

    std::string screenshotPath = screenshot_dump();
    if (!screenshotPath.empty())
    {
        auto screenshotPathW = String::ToWideChar(screenshotPath.c_str());
        uploadFiles[L"attachment_screenshot.png"] = screenshotPathW;
    }

    if (with_record)
    {
        auto parkReplayPathW = String::ToWideChar(gSilentRecordingName);
        bool record_copied = CopyFileW(parkReplayPathW.c_str(), recordFilePathNew, true);
        if (record_copied)
        {
            uploadFiles[L"attachment_replay.parkrep"] = recordFilePathNew;
        }
        else
        {
            with_record = false;
        }
    }

    if (gOpenRCT2SilentBreakpad)
    {
        printf("Uploading minidump in silent mode...\n");
        int error;
        std::wstring response;
        UploadMinidump(uploadFiles, error, response);
        return succeeded;
    }

    constexpr const wchar_t* MessageFormat = L"A crash has occurred and a dump was created at\n%s.\n\nPlease file an issue "
                                             L"with OpenRCT2 on GitHub, and provide "
                                             L"the dump and saved game there.\n\nVersion: %s\nCommit: %s\n\n"
                                             L"We would like to upload the crash dump for automated analysis, do you agree?\n"
                                             L"The automated analysis is done by courtesy of https://backtrace.io/";
    wchar_t message[MAX_PATH * 2];
    swprintf_s(message, MessageFormat, dumpFilePath, WSZ(OPENRCT2_VERSION), _wszCommitSha1Short);

    // Cannot use platform_show_messagebox here, it tries to set parent window already dead.
    int answer = MessageBoxW(nullptr, message, WSZ(OPENRCT2_NAME), MB_YESNO | MB_ICONERROR);
    if (answer == IDYES)
    {
        int error;
        std::wstring response;
        bool ok = UploadMinidump(uploadFiles, error, response);
        if (!ok)
        {
            const wchar_t* MessageFormat2 = L"There was a problem while uploading the dump. Please upload it manually to "
                                            L"GitHub. It should be highlighted for you once you close this message.\n"
                                            L"It might be because you are using outdated build and we have disabled its "
                                            L"access token. Make sure you are running recent version.\n"
                                            L"Dump file = %s\n"
                                            L"Please provide following information as well:\n"
                                            L"Error code = %d\n"
                                            L"Response = %s";
            swprintf_s(message, MessageFormat2, dumpFilePath, error, response.c_str());
            MessageBoxW(nullptr, message, WSZ(OPENRCT2_NAME), MB_OK | MB_ICONERROR);
        }
        else
        {
            MessageBoxW(nullptr, L"Dump uploaded successfully.", WSZ(OPENRCT2_NAME), MB_OK | MB_ICONINFORMATION);
        }
    }
    HRESULT coInitializeResult = CoInitialize(nullptr);
    if (SUCCEEDED(coInitializeResult))
    {
        LPITEMIDLIST pidl = ILCreateFromPathW(dumpPath);
        LPITEMIDLIST files[6];
        uint32_t numFiles = 0;

        files[numFiles++] = ILCreateFromPathW(dumpFilePath);
        // There should be no need to check if this file exists, if it doesn't
        // it simply shouldn't get selected.
        files[numFiles++] = ILCreateFromPathW(dumpFilePathGZIP);
        files[numFiles++] = ILCreateFromPathW(configFilePath);
        if (savedGameDumped)
        {
            files[numFiles++] = ILCreateFromPathW(saveFilePath);
        }
        if (with_record)
        {
            files[numFiles++] = ILCreateFromPathW(recordFilePathNew);
        }
        if (pidl != nullptr)
        {
            SHOpenFolderAndSelectItems(pidl, numFiles, (LPCITEMIDLIST*)files, 0);
            ILFree(pidl);
            for (uint32_t i = 0; i < numFiles; i++)
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
    auto env = GetContext()->GetPlatformEnvironment();
    auto crashPath = env->GetDirectoryPath(DIRBASE::USER, DIRID::CRASH);

    auto result = String::ToWideChar(crashPath);
    return result;
}

// Using non-null pipe name here lets breakpad try setting OOP crash handling
constexpr const wchar_t* PipeName = L"openrct2-bpad";

#endif // USE_BREAKPAD

CExceptionHandler crash_init()
{
#ifdef USE_BREAKPAD
    // Path must exist and be RW!
    auto exHandler = new google_breakpad::ExceptionHandler(
        GetDumpDirectory(), 0, OnCrash, 0, google_breakpad::ExceptionHandler::HANDLER_ALL, MiniDumpWithDataSegs, PipeName, 0);
    return reinterpret_cast<CExceptionHandler>(exHandler);
#else  // USE_BREAKPAD
    return nullptr;
#endif // USE_BREAKPAD
}
