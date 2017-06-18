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

#include "../core/Console.hpp"
#include "../Context.h"
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
    #include "../config/Config.h"
    #include "../drawing/drawing.h"
    #include "../game.h"
    #include "../input.h"
    #include "../interface/viewport.h"
    #include "../interface/window.h"
    #include "../localisation/localisation.h"
    #include "../management/news_item.h"
    #include "../peep/staff.h"
    #include "../world/Climate.h"
    #include "../world/scenery.h"
}

extern "C"
{
    bool gTitleHideVersionInfo = false;
    uint16 gTitleCurrentSequence;
}

static uint16                   _loadedTitleSequenceId = UINT16_MAX;
static ITitleSequencePlayer *   _sequencePlayer = nullptr;

static void TitleInitialise();
static void TryLoadSequence();

/**
 *
 *  rct2: 0x00678680
 */
static void TitleInitialise()
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

static void TryLoadSequence()
{
    if (_loadedTitleSequenceId != gTitleCurrentSequence)
    {
        uint16 numSequences = (uint16)TitleSequenceManager::GetCount();
        if (numSequences > 0)
        {
            uint16 targetSequence = gTitleCurrentSequence;
            do
            {
                if (_sequencePlayer->Begin(targetSequence) && _sequencePlayer->Update())
                {
                    _loadedTitleSequenceId = targetSequence;
                    gTitleCurrentSequence = targetSequence;
                    gfx_invalidate_screen();
                    return;
                }
                targetSequence = (targetSequence + 1) % numSequences;
            }
            while (targetSequence != gTitleCurrentSequence);
        }
        Console::Error::WriteLine("Unable to play any title sequences.");
        _sequencePlayer->Eject();
        gTitleCurrentSequence = UINT16_MAX;
        _loadedTitleSequenceId = UINT16_MAX;
    }
}

extern "C"
{
    /**
     *
     *  rct2: 0x0068E8DA
     */
    void title_load()
    {
        log_verbose("loading title");

        if (gGamePaused & GAME_PAUSED_NORMAL)
            pause_toggle();

        gScreenFlags = SCREEN_FLAGS_TITLE_DEMO;

#ifndef DISABLE_NETWORK
        network_close();
#endif
        audio_stop_all_music_and_sounds();
        game_init_all(150);
        viewport_init_all();
        window_main_open();
        title_create_windows();
        TitleInitialise();
        gfx_invalidate_screen();
        audio_start_title_music();
        gScreenAge = 0;

        if (gOpenRCT2ShowChangelog) {
            gOpenRCT2ShowChangelog = false;
            window_changelog_open();
        }

        if (_sequencePlayer != nullptr)
        {
            _sequencePlayer->Reset();

            // Force the title sequence to load / update so we
            // don't see a blank screen for a split second.
            TryLoadSequence();
            _sequencePlayer->Update();
        }

        log_verbose("loading title finished");
    }

    /**
     * Creates the windows shown on the title screen; New game, load game,
     * tutorial, toolbox and exit.
     *  rct2: 0x0066B5C0 (part of 0x0066B3E8)
     */
    void title_create_windows()
    {
        window_title_menu_open();
        window_title_exit_open();
        window_title_options_open();
        window_title_logo_open();
        window_resize_gui(context_get_width(), context_get_height());
        gTitleHideVersionInfo = false;
    }

    void title_update()
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

    void * title_get_sequence_player()
    {
        return _sequencePlayer;
    }

    void title_sequence_change_preset(sint32 preset)
    {
        sint32 count = (sint32)title_sequence_manager_get_count();
        if (preset < 0 || preset >= count) {
            return;
        }

        const utf8 * configId = title_sequence_manager_get_config_id(preset);
        SafeFree(gConfigInterface.current_title_sequence_preset);
        gConfigInterface.current_title_sequence_preset = _strdup(configId);

        gTitleCurrentSequence = preset;
        window_invalidate_all();
    }
}
