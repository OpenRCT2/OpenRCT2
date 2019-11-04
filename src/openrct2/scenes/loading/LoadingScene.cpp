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

using namespace OpenRCT2;

void LoadingScene::Load()
{
    log_verbose("LoadingScene::Load()");

    gScreenFlags = SCREEN_FLAGS_PLAYING;

    log_verbose("LoadingScene::Load() finished");
}

void LoadingScene::Update()
{
    gInUpdateCode = true;

    context_handle_input();

    gInUpdateCode = false;

    FinishScene();
}

void LoadingScene::Stop()
{
    audio_stop_all_music_and_sounds();
}
