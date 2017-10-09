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

#pragma once

#include "common.h"
#include "core/Guard.hpp"

// Defining MAX_PATH temporarily until the paths below can be removed
// Saves us from importing system headers in a header file.
#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#ifndef DISABLE_NETWORK
    #include <openssl/evp.h>
#endif // DISABLE_NETWORK

enum STARTUP_ACTION
{
    STARTUP_ACTION_INTRO,
    STARTUP_ACTION_TITLE,
    STARTUP_ACTION_OPEN,
    STARTUP_ACTION_EDIT
};

enum
{
    // Although this is labeled a flag it actually means when
    // zero the screen is in playing mode.
    SCREEN_FLAGS_PLAYING = 0,
    SCREEN_FLAGS_TITLE_DEMO = 1,
    SCREEN_FLAGS_SCENARIO_EDITOR = 2,
    SCREEN_FLAGS_TRACK_DESIGNER = 4,
    SCREEN_FLAGS_TRACK_MANAGER = 8,
    SCREEN_FLAGS_EDITOR = (SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER),
};

#ifdef __cplusplus
extern "C"
{
#endif

    /** The exit code for OpenRCT2 when it exits. */
    extern sint32 gExitCode;

    extern sint32 gOpenRCT2StartupAction;
    extern utf8 gOpenRCT2StartupActionPath[512];
    extern utf8 gExePath[MAX_PATH];
    extern utf8 gCustomUserDataPath[MAX_PATH];
    extern utf8 gCustomOpenrctDataPath[MAX_PATH];
    extern utf8 gCustomRCT2DataPath[MAX_PATH];
    extern utf8 gCustomPassword[MAX_PATH];
    extern bool gOpenRCT2Headless;
    extern bool gOpenRCT2NoGraphics;
    extern bool gOpenRCT2ShowChangelog;
    extern bool gOpenRCT2SilentBreakpad;

#ifndef DISABLE_NETWORK
    extern EVP_MD_CTX * gHashCTX;
#endif // DISABLE_NETWORK

#ifndef DISABLE_NETWORK
    extern sint32 gNetworkStart;
    extern char gNetworkStartHost[128];
    extern sint32 gNetworkStartPort;
    extern char* gNetworkStartAddress;
#endif

    extern uint32 gCurrentDrawCount;
    extern uint8 gScreenFlags;
    extern uint32 gScreenAge;
    extern uint8 gSavePromptMode;

    void openrct2_write_full_version_info(utf8 * buffer, size_t bufferSize);
    void openrct2_finish();

    sint32 cmdline_run(const char * * argv, sint32 argc);

#ifdef __cplusplus
}
#endif
