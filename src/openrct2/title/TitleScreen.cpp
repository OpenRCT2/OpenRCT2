/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TitleScreen.h"

#include "../Context.h"
#include "../Game.h"
#include "../GameState.h"
#include "../Input.h"
#include "../OpenRCT2.h"
#include "../Version.h"
#include "../audio/audio.h"
#include "../config/Config.h"
#include "../core/Console.hpp"
#include "../drawing/Drawing.h"
#include "../interface/Screenshot.h"
#include "../interface/Viewport.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../network/NetworkBase.h"
#include "../network/network.h"
#include "../scenario/Scenario.h"
#include "../scenario/ScenarioRepository.h"
#include "../ui/UiContext.h"
#include "../util/Util.h"
#include "TitleSequence.h"
#include "TitleSequenceManager.h"
#include "TitleSequencePlayer.h"

using namespace OpenRCT2;

// TODO Remove when no longer required.
bool gPreviewingTitleSequenceInGame;
static TitleScreen* _singleton = nullptr;

TitleScreen::TitleScreen(GameState& gameState)
    : _gameState(gameState)
{
    _singleton = this;
}

TitleScreen::~TitleScreen()
{
    _singleton = nullptr;
}

ITitleSequencePlayer* TitleScreen::GetSequencePlayer()
{
    return _sequencePlayer;
}

size_t TitleScreen::GetCurrentSequence()
{
    return _currentSequence;
}

bool TitleScreen::PreviewSequence(size_t value)
{
    _currentSequence = value;
    _previewingSequence = TryLoadSequence(true);
    if (_previewingSequence)
    {
        if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
        {
            gPreviewingTitleSequenceInGame = true;
        }
    }
    else
    {
        _currentSequence = title_get_config_sequence();
        if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        {
            TryLoadSequence();
        }
    }
    return _previewingSequence;
}

void TitleScreen::StopPreviewingSequence()
{
    if (_previewingSequence)
    {
        rct_window* mainWindow = WindowGetMain();
        if (mainWindow != nullptr)
        {
            WindowUnfollowSprite(*mainWindow);
        }
        _previewingSequence = false;
        _currentSequence = title_get_config_sequence();
        gPreviewingTitleSequenceInGame = false;
    }
}

bool TitleScreen::IsPreviewingSequence()
{
    return _previewingSequence;
}

bool TitleScreen::ShouldHideVersionInfo()
{
    return _hideVersionInfo;
}

void TitleScreen::SetHideVersionInfo(bool value)
{
    _hideVersionInfo = value;
}

void TitleScreen::Load()
{
    log_verbose("TitleScreen::Load()");

    if (game_is_paused())
    {
        pause_toggle();
    }

    gScreenFlags = SCREEN_FLAGS_TITLE_DEMO;
    gScreenAge = 0;
    gCurrentLoadedPath.clear();

#ifndef DISABLE_NETWORK
    GetContext()->GetNetwork().Close();
#endif
    OpenRCT2::Audio::StopAll();
    GetContext()->GetGameState()->InitAll(DEFAULT_MAP_SIZE);
    ViewportInitAll();
    ContextOpenWindow(WindowClass::MainWindow);
    CreateWindows();
    TitleInitialise();
    OpenRCT2::Audio::PlayTitleMusic();

    if (gOpenRCT2ShowChangelog)
    {
        gOpenRCT2ShowChangelog = false;
        ContextOpenWindow(WindowClass::Changelog);
    }

    if (_sequencePlayer != nullptr)
    {
        _sequencePlayer->Begin(_currentSequence);

        // Force the title sequence to load / update so we
        // don't see a blank screen for a split second.
        TryLoadSequence();
        _sequencePlayer->Update();
    }

    log_verbose("TitleScreen::Load() finished");
}

void TitleScreen::Tick()
{
    gInUpdateCode = true;

    ScreenshotCheck();
    title_handle_keyboard_input();

    if (game_is_not_paused())
    {
        TryLoadSequence();
        _sequencePlayer->Update();

        int32_t numUpdates = 1;
        if (gGameSpeed > 1)
        {
            numUpdates = 1 << (gGameSpeed - 1);
        }
        for (int32_t i = 0; i < numUpdates; i++)
        {
            _gameState.UpdateLogic();
        }
        update_palette_effects();
        // update_weather_animation();
    }

    input_set_flag(INPUT_FLAG_VIEWPORT_SCROLLING, false);

    ContextUpdateMapTooltip();
    WindowDispatchUpdateAll();

    gSavedAge++;

    ContextHandleInput();

    gInUpdateCode = false;
}

void TitleScreen::ChangePresetSequence(size_t preset)
{
    size_t count = TitleSequenceManager::GetCount();
    if (preset >= count)
    {
        return;
    }

    const utf8* configId = title_sequence_manager_get_config_id(preset);
    gConfigInterface.CurrentTitleSequencePreset = configId;

    if (!_previewingSequence)
        _currentSequence = preset;
    WindowInvalidateAll();
}

/**
 * Creates the windows shown on the title screen; New game, load game,
 * tutorial, toolbox and exit.
 */
void TitleScreen::CreateWindows()
{
    ContextOpenWindow(WindowClass::TitleMenu);
    ContextOpenWindow(WindowClass::TitleExit);
    ContextOpenWindow(WindowClass::TitleOptions);
    ContextOpenWindow(WindowClass::TitleLogo);
    WindowResizeGui(ContextGetWidth(), ContextGetHeight());
    _hideVersionInfo = false;
}

void TitleScreen::TitleInitialise()
{
    if (_sequencePlayer == nullptr)
    {
        _sequencePlayer = GetContext()->GetUiContext()->GetTitleSequencePlayer();
    }
    if (gConfigInterface.RandomTitleSequence)
    {
        bool RCT1Installed = false, RCT1AAInstalled = false, RCT1LLInstalled = false;
        int RCT1Count = 0;
        size_t scenarioCount = scenario_repository_get_count();

        for (size_t s = 0; s < scenarioCount; s++)
        {
            if (scenario_repository_get_by_index(s)->source_game == ScenarioSource::RCT1)
            {
                RCT1Count++;
            }
            if (scenario_repository_get_by_index(s)->source_game == ScenarioSource::RCT1_AA)
            {
                RCT1AAInstalled = true;
            }
            if (scenario_repository_get_by_index(s)->source_game == ScenarioSource::RCT1_LL)
            {
                RCT1LLInstalled = true;
            }
        }

        // Mega Park can show up in the scenario list even if RCT1 has been uninstalled, so it must be greater than 1
        if (RCT1Count > 1)
        {
            RCT1Installed = true;
        }

        int32_t random = 0;
        bool safeSequence = false;
        std::string RCT1String = format_string(STR_TITLE_SEQUENCE_RCT1, nullptr);
        std::string RCT1AAString = format_string(STR_TITLE_SEQUENCE_RCT1_AA, nullptr);
        std::string RCT1LLString = format_string(STR_TITLE_SEQUENCE_RCT1_AA_LL, nullptr);

        // Ensure the random sequence chosen isn't from RCT1 or expansion if the player doesn't have it installed
        while (!safeSequence)
        {
            size_t total = TitleSequenceManager::GetCount();
            random = util_rand() % static_cast<int32_t>(total);
            const utf8* scName = title_sequence_manager_get_name(random);
            safeSequence = true;
            if (scName == RCT1String)
            {
                safeSequence = RCT1Installed;
            }
            if (scName == RCT1AAString)
            {
                safeSequence = RCT1AAInstalled;
            }
            if (scName == RCT1LLString)
            {
                safeSequence = RCT1LLInstalled;
            }
        }
        ChangePresetSequence(random);
    }
    size_t seqId = title_get_config_sequence();
    if (seqId == SIZE_MAX)
    {
        seqId = title_sequence_manager_get_index_for_config_id("*OPENRCT2");
        if (seqId == SIZE_MAX)
        {
            seqId = 0;
        }
    }
    ChangePresetSequence(static_cast<int32_t>(seqId));
}

bool TitleScreen::TryLoadSequence(bool loadPreview)
{
    if (_loadedTitleSequenceId != _currentSequence || loadPreview)
    {
        if (_sequencePlayer == nullptr)
        {
            _sequencePlayer = GetContext()->GetUiContext()->GetTitleSequencePlayer();
        }

        size_t numSequences = TitleSequenceManager::GetCount();
        if (numSequences > 0)
        {
            size_t targetSequence = _currentSequence;
            do
            {
                if (_sequencePlayer->Begin(targetSequence) && _sequencePlayer->Update())
                {
                    _loadedTitleSequenceId = targetSequence;
                    if (targetSequence != _currentSequence && !loadPreview)
                    {
                        // Forcefully change the preset to a preset that works.
                        const utf8* configId = title_sequence_manager_get_config_id(targetSequence);
                        gConfigInterface.CurrentTitleSequencePreset = configId;
                    }
                    _currentSequence = targetSequence;
                    GfxInvalidateScreen();
                    return true;
                }
                targetSequence = (targetSequence + 1) % numSequences;
            } while (targetSequence != _currentSequence && !loadPreview);
        }
        Console::Error::WriteLine("Unable to play any title sequences.");
        _sequencePlayer->Eject();
        _currentSequence = SIZE_MAX;
        _loadedTitleSequenceId = SIZE_MAX;
        if (!loadPreview)
        {
            GetContext()->GetGameState()->InitAll(DEFAULT_MAP_SIZE);
            game_notify_map_changed();
        }
        return false;
    }
    return true;
}

void title_load()
{
    if (_singleton != nullptr)
    {
        _singleton->Load();
    }
}

void title_create_windows()
{
    if (_singleton != nullptr)
    {
        _singleton->CreateWindows();
    }
}

void* title_get_sequence_player()
{
    void* result = nullptr;
    if (_singleton != nullptr)
    {
        result = _singleton->GetSequencePlayer();
    }
    return result;
}

void title_sequence_change_preset(size_t preset)
{
    if (_singleton != nullptr)
    {
        _singleton->ChangePresetSequence(preset);
    }
}

bool title_should_hide_version_info()
{
    bool result = false;
    if (_singleton != nullptr)
    {
        result = _singleton->ShouldHideVersionInfo();
    }
    return result;
}

void title_set_hide_version_info(bool value)
{
    if (_singleton != nullptr)
    {
        _singleton->SetHideVersionInfo(value);
    }
}

size_t title_get_config_sequence()
{
    return title_sequence_manager_get_index_for_config_id(gConfigInterface.CurrentTitleSequencePreset.c_str());
}

size_t title_get_current_sequence()
{
    size_t result = 0;
    if (_singleton != nullptr)
    {
        result = _singleton->GetCurrentSequence();
    }
    return result;
}

bool title_preview_sequence(size_t value)
{
    if (_singleton != nullptr)
    {
        return _singleton->PreviewSequence(value);
    }
    return false;
}

void title_stop_previewing_sequence()
{
    if (_singleton != nullptr)
    {
        _singleton->StopPreviewingSequence();
    }
}

bool title_is_previewing_sequence()
{
    if (_singleton != nullptr)
    {
        return _singleton->IsPreviewingSequence();
    }
    return false;
}

void DrawOpenRCT2(rct_drawpixelinfo* dpi, const ScreenCoordsXY& screenCoords)
{
    thread_local std::string buffer;
    buffer.clear();
    buffer.assign("{OUTLINE}{WHITE}");

    // Write name and version information
    buffer += gVersionInfoFull;
    GfxDrawString(dpi, screenCoords + ScreenCoordsXY(5, 5 - 13), buffer.c_str(), { COLOUR_BLACK });

    // Invalidate screen area
    int16_t width = static_cast<int16_t>(GfxGetStringWidth(buffer, FontStyle::Medium));
    GfxSetDirtyBlocks(
        { screenCoords, screenCoords + ScreenCoordsXY{ width, 30 } }); // 30 is an arbitrary height to catch both strings

    // Write platform information
    buffer.assign("{OUTLINE}{WHITE}");
    buffer.append(OPENRCT2_PLATFORM);
    buffer.append(" (");
    buffer.append(OPENRCT2_ARCHITECTURE);
    buffer.append(")");
    GfxDrawString(dpi, screenCoords + ScreenCoordsXY(5, 5), buffer.c_str(), { COLOUR_BLACK });
}
