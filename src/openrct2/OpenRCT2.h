/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "common.h"

enum STARTUP_ACTION
{
    STARTUP_ACTION_NONE,
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

/** The exit code for OpenRCT2 when it exits. */
extern int32_t gExitCode;

extern int32_t gOpenRCT2StartupAction;
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
extern int32_t gNetworkStart;
extern char gNetworkStartHost[128];
extern int32_t gNetworkStartPort;
extern char* gNetworkStartAddress;
#endif

extern uint32_t gCurrentDrawCount;
extern uint8_t gScreenFlags;
extern uint32_t gScreenAge;
extern uint8_t gSavePromptMode;

void openrct2_write_full_version_info(utf8 * buffer, size_t bufferSize);
void openrct2_finish();

int32_t cmdline_run(const char * * argv, int32_t argc);

