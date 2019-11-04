/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "LoadingScene.h"

#include "../../Context.h"
#include "../../Game.h"
#include "../../GameState.h"
#include "../../OpenRCT2.h"
#include "../../audio/audio.h"
#include "../../interface/Viewport.h"
#include "../../interface/Window.h"
#include "../../windows/Intent.h"

using namespace OpenRCT2;

LoadingScene::LoadingScene(IContext& context)
    : Scene(context)
    , _jobs(1)
{
}

void LoadingScene::Load()
{
    log_verbose("LoadingScene::Load()");

    gScreenFlags = SCREEN_FLAGS_PLAYING;
    GetGameState()->InitAll(150);
    viewport_init_all();
    context_open_window(WC_MAIN_WINDOW);
    window_resize_gui(context_get_width(), context_get_height());

    auto intent = Intent(WC_NETWORK_STATUS);
    intent.putExtra(INTENT_EXTRA_MESSAGE, std::string{ "Loading..." });
    context_open_intent(&intent);

    log_verbose("LoadingScene::Load() finished");
}

void LoadingScene::Update()
{
    gInUpdateCode = true;

    context_handle_input();
    window_invalidate_all();

    gInUpdateCode = false;

    if (_jobs.CountPending() == 0 && _jobs.CountProcessing() == 0)
    {
        // Make sure the job is fully completed.
        _jobs.Join();

        FinishScene();
    }
}

void LoadingScene::Stop()
{
    audio_stop_all_music_and_sounds();
}
