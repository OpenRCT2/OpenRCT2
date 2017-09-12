#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../config/Config.h"
#include "../Context.h"
#include "../core/Console.hpp"
#include "../interface/Screenshot.h"
#include "../network/network.h"
#include "../OpenRCT2.h"
#include "../scenario/ScenarioRepository.h"
#include "TitleScreen.h"
#include "TitleSequence.h"
#include "TitleSequenceManager.h"
#include "TitleSequencePlayer.h"

extern "C"
{
    #include "../audio/audio.h"
    #include "../drawing/drawing.h"
    #include "../game.h"
    #include "../input.h"
    #include "../interface/viewport.h"
    #include "../interface/window.h"
    #include "../localisation/localisation.h"
}

// TODO Remove when no longer required.
static TitleScreen * _singleton = nullptr;

TitleScreen::TitleScreen()
{
    _singleton = this;
}

TitleScreen::~TitleScreen()
{
    delete _sequencePlayer;
    _singleton = nullptr;
}

ITitleSequencePlayer * TitleScreen::GetSequencePlayer()
{
    return _sequencePlayer;
}

uint16 TitleScreen::GetCurrentSequence()
{
    return _currentSequence;
}

void TitleScreen::SetCurrentSequence(uint16 value)
{
    _currentSequence = value;
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
    gCurrentLoadedPath[0] = '\0';

    network_close();
    audio_stop_all_music_and_sounds();
    game_init_all(150);
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
        _sequencePlayer->Reset();

        // Force the title sequence to load / update so we
        // don't see a blank screen for a split second.
        TryLoadSequence();
        _sequencePlayer->Update();
    }

    log_verbose("TitleScreen::Load() finished");
}

void TitleScreen::Update()
{
    gInUpdateCode = true;

    screenshot_check();
    title_handle_keyboard_input();

    if (game_is_not_paused())
    {
        TryLoadSequence();
        _sequencePlayer->Update();

        sint32 numUpdates = 1;
        if (gGameSpeed > 1) {
            numUpdates = 1 << (gGameSpeed - 1);
        }
        for (sint32 i = 0; i < numUpdates; i++)
        {
            game_logic_update();
        }
        update_palette_effects();
        // update_rain_animation();
    }

    input_set_flag(INPUT_FLAG_VIEWPORT_SCROLLING, false);

    window_map_tooltip_update_visibility();
    window_dispatch_update_all();

    gSavedAge++;

    game_handle_input();

    gInUpdateCode = false;
}

void TitleScreen::ChangeSequence(sint32 preset)
{
    sint32 count = (sint32)title_sequence_manager_get_count();
    if (preset < 0 || preset >= count)
    {
        return;
    }

    const utf8 * configId = title_sequence_manager_get_config_id(preset);
    SafeFree(gConfigInterface.current_title_sequence_preset);
    gConfigInterface.current_title_sequence_preset = _strdup(configId);

    _currentSequence = preset;
    window_invalidate_all();
}

/**
 * Creates the windows shown on the title screen; New game, load game,
 * tutorial, toolbox and exit.
 */
void TitleScreen::CreateWindows()
{
    context_open_window(WC_TITLE_MENU);
    context_open_window(WC_TITLE_EXIT);
    context_open_window(WC_TITLE_OPTIONS);
    context_open_window(WC_TITLE_LOGO);
    window_resize_gui(context_get_width(), context_get_height());
    _hideVersionInfo = false;
}

void TitleScreen::TitleInitialise()
{
    if (_sequencePlayer == nullptr)
    {
        IScenarioRepository * scenarioRepository = GetScenarioRepository();
        _sequencePlayer = CreateTitleSequencePlayer(scenarioRepository);
    }
    size_t seqId = title_sequence_manager_get_index_for_config_id(gConfigInterface.current_title_sequence_preset);
    if (seqId == SIZE_MAX)
    {
        seqId = title_sequence_manager_get_index_for_config_id("*OPENRCT2");
        if (seqId == SIZE_MAX)
        {
            seqId = 0;
        }
    }
    title_sequence_change_preset((sint32)seqId);
    TryLoadSequence();
}

void TitleScreen::TryLoadSequence()
{
    if (_loadedTitleSequenceId != _currentSequence)
    {
        uint16 numSequences = (uint16)TitleSequenceManager::GetCount();
        if (numSequences > 0)
        {
            uint16 targetSequence = _currentSequence;
            do
            {
                if (_sequencePlayer->Begin(targetSequence) && _sequencePlayer->Update())
                {
                    _loadedTitleSequenceId = targetSequence;
                    _currentSequence = targetSequence;
                    gfx_invalidate_screen();
                    return;
                }
                targetSequence = (targetSequence + 1) % numSequences;
            }
            while (targetSequence != _currentSequence);
        }
        Console::Error::WriteLine("Unable to play any title sequences.");
        _sequencePlayer->Eject();
        _currentSequence = UINT16_MAX;
        _loadedTitleSequenceId = UINT16_MAX;
        game_init_all(150);
    }
}

extern "C"
{
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

    void * title_get_sequence_player()
    {
        void * result = nullptr;
        if (_singleton != nullptr)
        {
            result = _singleton->GetSequencePlayer();
        }
        return result;
    }

    void title_sequence_change_preset(sint32 preset)
    {
        if (_singleton != nullptr)
        {
            _singleton->ChangeSequence(preset);
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

    uint16 title_get_current_sequence()
    {
        uint16 result = 0;
        if (_singleton != nullptr)
        {
            result = _singleton->GetCurrentSequence();
        }
        return result;
    }

    void title_set_current_sequence(uint16 value)
    {
        if (_singleton != nullptr)
        {
            _singleton->SetCurrentSequence(value);
        }
    }

    void DrawOpenRCT2(rct_drawpixelinfo * dpi, sint32 x, sint32 y)
    {
        utf8 buffer[256];

        // Write format codes
        utf8 *ch = buffer;
        ch = utf8_write_codepoint(ch, FORMAT_MEDIUMFONT);
        ch = utf8_write_codepoint(ch, FORMAT_OUTLINE);
        ch = utf8_write_codepoint(ch, FORMAT_WHITE);

        // Write name and version information
        openrct2_write_full_version_info(ch, sizeof(buffer) - (ch - buffer));
        gfx_draw_string(dpi, buffer, COLOUR_BLACK, x + 5, y + 5 - 13);

        // Write platform information
        snprintf(ch, 256 - (ch - buffer), "%s (%s)", OPENRCT2_PLATFORM, OPENRCT2_ARCHITECTURE);
        gfx_draw_string(dpi, buffer, COLOUR_BLACK, x + 5, y + 5);
    }
}
