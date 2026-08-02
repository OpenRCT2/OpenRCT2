/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TrackManagerScene.h"

#include "../../Context.h"
#include "../../Diagnostic.h"
#include "../../Game.h"
#include "../../GameState.h"
#include "../../OpenRCT2.h"
#include "../../audio/Audio.h"
#include "../../drawing/Drawing.h"
#include "../../interface/WindowBase.h"
#include "../../localisation/Language.h"
#include "../../world/Map.h"
#include "EditorController.h"

using namespace OpenRCT2;

void TrackManagerScene::Load()
{
    LOG_VERBOSE("TrackManagerScene::Load()");

    Audio::StopAll();
    gLegacyScene = LegacyScene::trackDesignsManager;
    gScreenAge = 0;

    auto& gameState = getGameState();
    gameStateInitAll(gameState, kDefaultMapSize);
    SetAllLandOwned();
    gameState.editorStep = Editor::Step::objectSelection;
    Editor::ObjectListLoad();
    ContextResetSubsystems();

    OpenEditorWindows();
    resetMainViewport();

    LoadPalette();

    GameLoadScripts();
    GameNotifyMapChanged();

    LOG_VERBOSE("TrackManagerScene::Load() finished");
}

void TrackManagerScene::Tick()
{
    gameStateTick();
}

void TrackManagerScene::Stop()
{
    Audio::StopAll();

    // Force closure of any object selection windows, regardless of valid state.
    // NB: this is relevant for both in-game scenes and editors, as the window
    // may be opened in-game using cheats.
    ContextForceCloseWindowByClass(WindowClass::editorObjectSelection);
}
