/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
#include "../../audio/audio.h"
#include "../../interface/Viewport.h"
#include "../../localisation/LocalisationService.h"
#include "../../localisation/StringIds.h"
#include "../../ui/WindowManager.h"
#include "../../windows/Intent.h"

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

    gScreenFlags = SCREEN_FLAGS_PLAYING;
    gameStateInitAll(GetGameState(), kDefaultMapSize);
    ViewportInitAll();
    ContextOpenWindow(WindowClass::MainWindow);
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
