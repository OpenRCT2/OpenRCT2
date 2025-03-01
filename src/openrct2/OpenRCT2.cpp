/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "OpenRCT2.h"

#include "core/EnumUtils.hpp"

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
ScreenMode gScreenMode;
uint32_t gScreenAge;
PromptMode gSavePromptMode;

bool gSilentReplays = false;

bool isInEditorMode()
{
    return EnumValue(gScreenMode) >= EnumValue(ScreenMode::scenarioEditor);
}

bool isInTrackDesignerOrManager()
{
    return gScreenMode == ScreenMode::trackDesigner || gScreenMode == ScreenMode::trackDesignsManager;
}
