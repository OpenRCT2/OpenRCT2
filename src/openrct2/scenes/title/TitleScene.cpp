/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TitleScene.h"

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
#include "TitleSequence.h"
#include "TitleSequenceManager.h"
#include "TitleSequencePlayer.h"

using namespace OpenRCT2;

// TODO Remove when no longer required.
bool gPreviewingTitleSequenceInGame;

ITitleSequencePlayer* TitleScene::GetSequencePlayer()
{
    return _sequencePlayer;
}

size_t TitleScene::GetCurrentSequence()
{
    return _currentSequence;
}

bool TitleScene::PreviewSequence(size_t value)
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

void TitleScene::StopPreviewingSequence()
{
    if (_previewingSequence)
    {
        rct_window* mainWindow = window_get_main();
        if (mainWindow != nullptr)
        {
            window_unfollow_sprite(mainWindow);
        }
        _previewingSequence = false;
        _currentSequence = title_get_config_sequence();
        gPreviewingTitleSequenceInGame = false;
    }
}

bool TitleScene::IsPreviewingSequence()
{
    return _previewingSequence;
}

bool TitleScene::ShouldHideVersionInfo()
{
    return _hideVersionInfo;
}

void TitleScene::SetHideVersionInfo(bool value)
{
    _hideVersionInfo = value;
}

void TitleScene::Load()
{
    log_verbose("TitleScreen::Load()");

    if (game_is_paused())
    {
        pause_toggle();
    }

    gScreenFlags = SCREEN_FLAGS_TITLE_DEMO;
    gScreenAge = 0;
    gCurrentLoadedPath = "";

    network_close();
    GetGameState()->InitAll(150);
    viewport_init_all();
    context_open_window(WC_MAIN_WINDOW);
    CreateWindows();
    TitleInitialise();
    audio_start_title_music();

    if (gOpenRCT2ShowChangelog)
    {
        gOpenRCT2ShowChangelog = false;
        context_open_window(WC_CHANGELOG);
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

void TitleScene::Update()
{
    gInUpdateCode = true;

    screenshot_check();
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
            GetGameState()->UpdateLogic();
        }
        update_palette_effects();
        // update_rain_animation();
    }

    input_set_flag(INPUT_FLAG_VIEWPORT_SCROLLING, false);

    context_update_map_tooltip();
    window_dispatch_update_all();

    gSavedAge++;

    context_handle_input();

    gInUpdateCode = false;
}

void TitleScene::Stop()
{
    audio_stop_all_music_and_sounds();
}

void TitleScene::ChangePresetSequence(size_t preset)
{
    size_t count = TitleSequenceManager::GetCount();
    if (preset >= count)
    {
        return;
    }

    const utf8* configId = title_sequence_manager_get_config_id(preset);
    SafeFree(gConfigInterface.current_title_sequence_preset);
    gConfigInterface.current_title_sequence_preset = _strdup(configId);

    if (!_previewingSequence)
        _currentSequence = preset;
    window_invalidate_all();
}

/**
 * Creates the windows shown on the title screen; New game, load game,
 * tutorial, toolbox and exit.
 */
void TitleScene::CreateWindows()
{
    context_open_window(WC_TITLE_MENU);
    context_open_window(WC_TITLE_EXIT);
    context_open_window(WC_TITLE_OPTIONS);
    context_open_window(WC_TITLE_LOGO);
    window_resize_gui(context_get_width(), context_get_height());
    _hideVersionInfo = false;
}

void TitleScene::TitleInitialise()
{
    if (_sequencePlayer == nullptr)
    {
        _sequencePlayer = GetContext().GetUiContext()->GetTitleSequencePlayer();
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
    ChangePresetSequence((int32_t)seqId);
}

bool TitleScene::TryLoadSequence(bool loadPreview)
{
    if (_loadedTitleSequenceId != _currentSequence || loadPreview)
    {
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
                        SafeFree(gConfigInterface.current_title_sequence_preset);
                        gConfigInterface.current_title_sequence_preset = _strdup(configId);
                    }
                    _currentSequence = targetSequence;
                    gfx_invalidate_screen();
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
            GetGameState()->InitAll(150);
        }
        return false;
    }
    return true;
}

void title_create_windows()
{
    auto* ctx = OpenRCT2::GetContext();
    auto* titleScreen = static_cast<TitleScene*>(ctx->GetTitleScene());
    if (titleScreen != nullptr)
    {
        titleScreen->CreateWindows();
    }
}

void* title_get_sequence_player()
{
    auto* ctx = OpenRCT2::GetContext();
    auto* titleScreen = static_cast<TitleScene*>(ctx->GetTitleScene());
    if (titleScreen != nullptr)
    {
        return titleScreen->GetSequencePlayer();
    }
    return nullptr;
}

void title_sequence_change_preset(size_t preset)
{
    auto* ctx = OpenRCT2::GetContext();
    auto* titleScreen = static_cast<TitleScene*>(ctx->GetTitleScene());
    if (titleScreen != nullptr)
    {
        titleScreen->ChangePresetSequence(preset);
    }
}

bool title_should_hide_version_info()
{
    auto* ctx = OpenRCT2::GetContext();
    auto* titleScreen = static_cast<TitleScene*>(ctx->GetTitleScene());
    if (titleScreen != nullptr)
    {
        return titleScreen->ShouldHideVersionInfo();
    }
    return false;
}

void title_set_hide_version_info(bool value)
{
    auto* ctx = OpenRCT2::GetContext();
    auto* titleScreen = static_cast<TitleScene*>(ctx->GetTitleScene());
    if (titleScreen != nullptr)
    {
        titleScreen->SetHideVersionInfo(value);
    }
}

size_t title_get_config_sequence()
{
    return title_sequence_manager_get_index_for_config_id(gConfigInterface.current_title_sequence_preset);
}

size_t title_get_current_sequence()
{
    auto* ctx = OpenRCT2::GetContext();
    auto* titleScreen = static_cast<TitleScene*>(ctx->GetTitleScene());
    if (titleScreen != nullptr)
    {
        return titleScreen->GetCurrentSequence();
    }
    return 0;
}

bool title_preview_sequence(size_t value)
{
    auto* ctx = OpenRCT2::GetContext();
    auto* titleScreen = static_cast<TitleScene*>(ctx->GetTitleScene());
    if (titleScreen != nullptr)
    {
        return titleScreen->PreviewSequence(value);
    }
    return false;
}

void title_stop_previewing_sequence()
{
    auto* ctx = OpenRCT2::GetContext();
    auto* titleScreen = static_cast<TitleScene*>(ctx->GetTitleScene());
    if (titleScreen != nullptr)
    {
        titleScreen->StopPreviewingSequence();
    }
}

bool title_is_previewing_sequence()
{
    auto* ctx = OpenRCT2::GetContext();
    auto* titleScreen = static_cast<TitleScene*>(ctx->GetTitleScene());
    if (titleScreen != nullptr)
    {
        return titleScreen->IsPreviewingSequence();
    }
    return false;
}

void DrawOpenRCT2(rct_drawpixelinfo* dpi, int32_t x, int32_t y)
{
    utf8 buffer[256];

    // Write format codes
    utf8* ch = buffer;
    ch = utf8_write_codepoint(ch, FORMAT_MEDIUMFONT);
    ch = utf8_write_codepoint(ch, FORMAT_OUTLINE);
    ch = utf8_write_codepoint(ch, FORMAT_WHITE);

    // Write name and version information
    openrct2_write_full_version_info(ch, sizeof(buffer) - (ch - buffer));
    gfx_draw_string(dpi, buffer, COLOUR_BLACK, x + 5, y + 5 - 13);

    // Invalidate screen area
    int16_t width = (int16_t)gfx_get_string_width(buffer);
    gfx_set_dirty_blocks(x, y, x + width, y + 30); // 30 is an arbitrary height to catch both strings

    // Write platform information
    snprintf(ch, 256 - (ch - buffer), "%s (%s)", OPENRCT2_PLATFORM, OPENRCT2_ARCHITECTURE);
    gfx_draw_string(dpi, buffer, COLOUR_BLACK, x + 5, y + 5);
}
