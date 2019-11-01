/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "GameScreen.h"

#include "../../Context.h"
#include "../../Game.h"
#include "../../GameState.h"
#include "../../Input.h"
#include "../../OpenRCT2.h"
#include "../../Version.h"
#include "../../audio/audio.h"
#include "../../config/Config.h"
#include "../../core/Console.hpp"
#include "../../drawing/Drawing.h"
#include "../../interface/Screenshot.h"
#include "../../interface/Viewport.h"
#include "../../interface/Window.h"
#include "../../localisation/Localisation.h"
#include "../../network/network.h"
#include "../../scenario/Scenario.h"
#include "../../scenario/ScenarioRepository.h"
#include "../../ui/UiContext.h"

using namespace OpenRCT2;

void GameScreen::Load()
{
    log_verbose("GameScreen::Load()");

    gScreenFlags = SCREEN_FLAGS_PLAYING;

    log_verbose("GameScreen::Load() finished");
}

void GameScreen::Update()
{
    _gameState.Update();
}

void GameScreen::Stop()
{
    audio_stop_all_music_and_sounds();
}
