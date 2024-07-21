/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "GameScene.h"

#include "../../Context.h"
#include "../../Diagnostic.h"
#include "../../Game.h"
#include "../../GameState.h"
#include "../../OpenRCT2.h"
#include "../../audio/audio.h"

using namespace OpenRCT2;

void GameScene::Load()
{
    LOG_VERBOSE("GameScene::Load()");

    gScreenFlags = SCREEN_FLAGS_PLAYING;

    LOG_VERBOSE("GameScene::Load() finished");
}

void GameScene::Tick()
{
    gameStateTick();
}

void GameScene::Stop()
{
    Audio::StopAll();
}
