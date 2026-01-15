/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "core/StringTypes.h"

enum class PromptMode : uint8_t;

enum class StartupAction
{
    None,
    Intro,
    Title,
    Open,
    Edit
};

enum class LegacyScene : uint8_t
{
    // Although this is labeled a flag it actually means when
    // zero the screen is in playing mode.
    playing,
    titleSequence,
    scenarioEditor,
    trackDesigner,
    trackDesignsManager,
};

bool isInEditorMode();
bool isInTrackDesignerOrManager();

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
extern bool gSilentReplays;

#ifndef DISABLE_NETWORK
namespace OpenRCT2::Network
{
    enum class Mode : int32_t;
}

namespace OpenRCT2
{
    extern Network::Mode gNetworkStart;
    extern std::string gNetworkStartHost;
    extern int32_t gNetworkStartPort;
    extern std::string gNetworkStartAddress;
} // namespace OpenRCT2
#endif

namespace OpenRCT2
{
    int32_t CommandLineRun(const char** argv, int32_t argc);
} // namespace OpenRCT2

extern uint32_t gCurrentDrawCount;
extern LegacyScene gLegacyScene;
extern uint32_t gScreenAge;
extern PromptMode gSavePromptMode;
