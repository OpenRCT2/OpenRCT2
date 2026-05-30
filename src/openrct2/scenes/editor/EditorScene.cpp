/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "EditorScene.h"

#include "../../Context.h"
#include "../../Diagnostic.h"
#include "../../Editor.h"
#include "../../Game.h"
#include "../../GameState.h"
#include "../../OpenRCT2.h"
#include "../../audio/Audio.h"
#include "../../drawing/Drawing.h"
#include "../../interface/WindowBase.h"
#include "../../localisation/Language.h"
#include "../../world/Map.h"

using namespace OpenRCT2;

void EditorScene::Load()
{
    LOG_VERBOSE("EditorScene::Load()");

    auto& gameState = getGameState();
    Audio::StopAll();
    gameStateInitAll(gameState, kDefaultMapSize);

    gLegacyScene = LegacyScene::scenarioEditor;
    gameState.editorStep = Editor::Step::objectSelection;
    gameState.park.flags |= PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
    gameState.scenarioOptions.category = Scenario::Category::other;

    Editor::ObjectListLoad();
    ContextResetSubsystems();

    WindowBase* mainWindow = Editor::OpenEditorWindows();
    mainWindow->setViewportLocation(TileCoordsXYZ{ 75, 75, 14 }.ToCoordsXYZ());

    LoadPalette();
    gScreenAge = 0;
    gameState.scenarioOptions.name = LanguageGetString(STR_MY_NEW_SCENARIO);

    GameLoadScripts();
    GameNotifyMapChanged();

    LOG_VERBOSE("EditorScene::Load() finished");
}

void EditorScene::Tick()
{
    gameStateTick();
}

void EditorScene::Stop()
{
    Audio::StopAll();

    // Force closure of any object selection windows, regardless of valid state.
    // NB: this is relevant for both in-game scenes and editors, as the window
    // may be opened in-game using cheats.
    ContextForceCloseWindowByClass(WindowClass::editorObjectSelection);
}
