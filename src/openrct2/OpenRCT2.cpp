/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "OpenRCT2.h"

StartupAction gOpenRCT2StartupAction = StartupAction::Title;
utf8 gOpenRCT2StartupActionPath[512] = { 0 };
u8string gCustomUserDataPath = {};
u8string gCustomOpenRCT2DataPath = {};
u8string gCustomRCT1DataPath = {};
u8string gCustomRCT2DataPath = {};
u8string gCustomPassword = {};
u8string gSilentRecordingName = {};

bool gOpenRCT2Headless = false;
bool gOpenRCT2NoGraphics = false;

bool gOpenRCT2ShowChangelog;
bool gOpenRCT2SilentBreakpad;

uint32_t gCurrentDrawCount = 0;
uint8_t gScreenFlags;
uint32_t gScreenAge;
PromptMode gSavePromptMode;

bool gSilentReplays = false;
