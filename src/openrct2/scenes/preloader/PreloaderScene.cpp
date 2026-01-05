/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "PreloaderScene.h"

#include "../../Context.h"
#include "../../Diagnostic.h"
#include "../../Game.h"
#include "../../GameState.h"
#include "../../OpenRCT2.h"
#include "../../audio/Audio.h"
#include "../../interface/Viewport.h"
#include "../../localisation/StringIds.h"
#include "../../ui/WindowManager.h"
#include "../../windows/Intent.h"
#include "../../world/Map.h"

#include <sstream>

using namespace OpenRCT2;

PreloaderScene::PreloaderScene(IContext& context)
    : Scene(context)
    , _jobs(1)
{
}

void PreloaderScene::Load()
{
    LOG_VERBOSE("PreloaderScene::Load()");

    gLegacyScene = LegacyScene::playing;
    gameStateInitAll(getGameState(), kDefaultMapSize);
    ContextResetSubsystems();
    ContextOpenWindow(WindowClass::mainWindow);
    WindowSetFlagForAllViewports(VIEWPORT_FLAG_RENDERING_INHIBITED, true);
    WindowResizeGui(ContextGetWidth(), ContextGetHeight());

    LOG_VERBOSE("PreloaderScene::Load() finished");
}

void PreloaderScene::Tick()
{
    gInUpdateCode = true;

    ContextHandleInput();

    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateAll();

    gInUpdateCode = false;

    if (!_jobs.IsBusy())
    {
        // Make sure the job is fully completed.
        _jobs.Join();

        FinishScene();
    }
}

void PreloaderScene::Stop()
{
    Audio::StopAll();
}
