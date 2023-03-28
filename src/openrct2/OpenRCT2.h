/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "common.h"
#include "core/String.hpp"

#include <string>

enum class PromptMode : uint8_t;

enum class StartupAction
{
    None,
    Intro,
    Title,
    Open,
    Edit
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

extern StartupAction gOpenRCT2StartupAction;
extern utf8 gOpenRCT2StartupActionPath[512];
extern u8string gCustomUserDataPath;
extern u8string gCustomOpenRCT2DataPath;
extern u8string gCustomRCT1DataPath;
extern u8string gCustomRCT2DataPath;
extern u8string gCustomPassword;
extern bool gOpenRCT2Headless;
extern bool gOpenRCT2NoGraphics;
extern bool gOpenRCT2ShowChangelog;
extern bool gOpenRCT2SilentBreakpad;
extern u8string gSilentRecordingName;

#ifndef DISABLE_NETWORK
extern int32_t gNetworkStart;
extern std::string gNetworkStartHost;
extern int32_t gNetworkStartPort;
extern std::string gNetworkStartAddress;
#endif

extern uint32_t gCurrentDrawCount;
extern uint8_t gScreenFlags;
extern uint32_t gScreenAge;
extern PromptMode gSavePromptMode;

void OpenRCT2WriteFullVersionInfo(utf8* buffer, size_t bufferSize);
void OpenRCT2Finish();

int32_t CommandLineRun(const char** argv, int32_t argc);
