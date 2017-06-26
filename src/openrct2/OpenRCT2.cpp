#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#include "core/Console.hpp"
#include "core/File.h"
#include "core/FileStream.hpp"
#include "OpenRCT2.h"

extern "C"
{
    #include "audio/audio.h"
    #include "platform/platform.h"
    #include "rct2.h"
}

extern "C"
{
    sint32 gExitCode;
    sint32 gOpenRCT2StartupAction = STARTUP_ACTION_TITLE;
    utf8 gOpenRCT2StartupActionPath[512] = { 0 };
    utf8 gExePath[MAX_PATH];
    utf8 gCustomUserDataPath[MAX_PATH] = { 0 };
    utf8 gCustomOpenrctDataPath[MAX_PATH] = { 0 };
    utf8 gCustomRCT2DataPath[MAX_PATH] = { 0 };
    utf8 gCustomPassword[MAX_PATH] = { 0 };

    bool gOpenRCT2Headless = false;
    bool gOpenRCT2NoGraphics = false;

    bool gOpenRCT2ShowChangelog;
    bool gOpenRCT2SilentBreakpad;

#ifndef DISABLE_NETWORK
    // OpenSSL's message digest context used for calculating sprite checksums
    EVP_MD_CTX * gHashCTX = nullptr;
#endif // DISABLE_NETWORK

    bool check_file_path(sint32 pathId)
    {
        const utf8 * path = get_file_path(pathId);
        switch (pathId) {
        case PATH_ID_G1:
            if (!File::Exists(path))
            {
                Console::Error::WriteLine("Unable to find '%s'", path);
                return false;
            }
            break;
        case PATH_ID_CUSTOM1:
        case PATH_ID_CUSTOM2:
            if (File::Exists(path))
            {
                try
                {
                    auto fs = FileStream(path, FILE_MODE_OPEN);
                    sint32 index = 36 + (pathId - PATH_ID_CUSTOM1);
                    gRideMusicInfoList[index]->length = fs.GetLength();
                }
                catch (const Exception &)
                {
                }
            }
            break;
        }
        return true;
    }
}
