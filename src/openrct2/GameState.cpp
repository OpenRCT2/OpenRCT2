/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "GameState.h"

#include "Context.h"
#include "Editor.h"
#include "Game.h"
#include "Input.h"
#include "OpenRCT2.h"
#include "ReplayManager.h"
#include "interface/Screenshot.h"
#include "localisation/Date.h"
#include "localisation/Localisation.h"
#include "management/NewsItem.h"
#include "network/network.h"
#include "platform/Platform2.h"
#include "scenario/Scenario.h"
#include "title/TitleScreen.h"
#include "title/TitleSequencePlayer.h"
#include "ui/UiContext.h"
#include "windows/Intent.h"
#include "world/Climate.h"
#include "world/MapAnimation.h"
#include "world/Park.h"
#include "world/Scenery.h"

#include <algorithm>

using namespace OpenRCT2;

GameState::GameState()
{
    _park = std::make_unique<Park>();
}

/**
 * Initialises the map, park etc. basically all S6 data.
 */
void GameState::InitAll(int32_t mapSize)
{
    gInMapInitCode = true;

    scenario_init();
    map_init(mapSize);
    _park->Initialise();
    finance_init();
    reset_park_entry();
    banner_init();
    ride_init_all();
    reset_sprite_list();
    staff_reset_modes();
    date_reset();
    climate_reset(CLIMATE_COOL_AND_WET);
    news_item_init_queue();
    user_string_clear_all();

    gInMapInitCode = false;

    gNextGuestNumber = 1;

    context_init();
    scenery_set_default_placement_configuration();

    auto intent = Intent(INTENT_ACTION_CLEAR_TILE_INSPECTOR_CLIPBOARD);
    context_broadcast_intent(&intent);

    load_palette();
}

/**
 * Function will be called every GAME_UPDATE_TIME_MS.
 * It has its own loop which might run multiple updates per call such as
 * when operating as a client it may run multiple updates to catch up with the server tick,
 * another influence can be the game speed setting.
 */
void GameState::Update()
{
    gInUpdateCode = true;

    // Normal game play will update only once every GAME_UPDATE_TIME_MS
    uint32_t numUpdates = 1;

    // 0x006E3AEC // screen_game_process_mouse_input();
    screenshot_check();
    game_handle_keyboard_input();

    if (game_is_not_paused() && gPreviewingTitleSequenceInGame)
    {
        auto player = GetContext()->GetUiContext()->GetTitleSequencePlayer();
        if (player != nullptr)
        {
            player->Update();
        }
    }

    uint32_t realtimeTicksElapsed = gCurrentDeltaTime / GAME_UPDATE_TIME_MS;
    realtimeTicksElapsed = std::clamp<uint32_t>(realtimeTicksElapsed, 1, GAME_MAX_UPDATES);

    // We use this variable to always advance ticks in normal speed.
    gCurrentRealTimeTicks += realtimeTicksElapsed;

    // Determine how many times we need to update the game
    if (gGameSpeed > 1)
    {
        // Update more often if game speed is above normal.
        numUpdates = 1 << (gGameSpeed - 1);
    }

    if (network_get_mode() == NETWORK_MODE_CLIENT && network_get_status() == NETWORK_STATUS_CONNECTED
        && network_get_authstatus() == NETWORK_AUTH_OK)
    {
        if (network_get_server_tick() - gCurrentTicks >= 10)
        {
            // Make sure client doesn't fall behind the server too much
            numUpdates += 10;
        }
    }

    bool didRunSingleFrame = false;
    if (game_is_paused())
    {
        if (gDoSingleUpdate && network_get_mode() == NETWORK_MODE_NONE)
        {
            didRunSingleFrame = true;
            pause_toggle();
            numUpdates = 1;
        }
        else
        {
            numUpdates = 0;
            // Update the animation list. Note this does not
            // increment the map animation.
            map_animation_invalidate_all();

            // Special case because we set numUpdates to 0, otherwise in game_logic_update.
            network_update();

            network_process_pending();
        }
    }

    // Update the game one or more times
    for (uint32_t i = 0; i < numUpdates; i++)
    {
        UpdateLogic();
        if (gGameSpeed == 1)
        {
            if (input_get_state() == INPUT_STATE_RESET || input_get_state() == INPUT_STATE_NORMAL)
            {
                if (input_test_flag(INPUT_FLAG_VIEWPORT_SCROLLING))
                {
                    input_set_flag(INPUT_FLAG_VIEWPORT_SCROLLING, false);
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }

    if (!gOpenRCT2Headless)
    {
        input_set_flag(INPUT_FLAG_VIEWPORT_SCROLLING, false);

        // the flickering frequency is reduced by 4, compared to the original
        // it was done due to inability to reproduce original frequency
        // and decision that the original one looks too fast
        if (gCurrentRealTimeTicks % 4 == 0)
            gWindowMapFlashingFlags ^= (1 << 15);

        // Handle guest map flashing
        gWindowMapFlashingFlags &= ~(1 << 1);
        if (gWindowMapFlashingFlags & (1 << 0))
            gWindowMapFlashingFlags |= (1 << 1);
        gWindowMapFlashingFlags &= ~(1 << 0);

        // Handle staff map flashing
        gWindowMapFlashingFlags &= ~(1 << 3);
        if (gWindowMapFlashingFlags & (1 << 2))
            gWindowMapFlashingFlags |= (1 << 3);
        gWindowMapFlashingFlags &= ~(1 << 2);

        context_update_map_tooltip();

        // Input
        gUnk141F568 = gUnk13CA740;

        context_handle_input();
    }

    // Always perform autosave check, even when paused
    if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) && !(gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
        && !(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER))
    {
        scenario_autosave_check();
    }

    window_dispatch_update_all();

    if (didRunSingleFrame && game_is_not_paused() && !(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
    {
        pause_toggle();
    }

    gGameCommandNestLevel = 0;
    gDoSingleUpdate = false;
    gInUpdateCode = false;
}

void GameState::UpdateLogic()
{
    gScreenAge++;
    if (gScreenAge == 0)
        gScreenAge--;

    network_update();

    GetContext()->GetReplayManager()->Update();

    if (network_get_mode() == NETWORK_MODE_CLIENT && network_get_status() == NETWORK_STATUS_CONNECTED
        && network_get_authstatus() == NETWORK_AUTH_OK)
    {
        // Can't be in sync with server, round trips won't work if we are at same level.
        if (gCurrentTicks >= network_get_server_tick())
        {
            // Don't run past the server
            return;
        }
    }

    if (network_get_mode() == NETWORK_MODE_SERVER)
    {
        // Send current tick out.
        network_send_tick();
    }
    else if (network_get_mode() == NETWORK_MODE_CLIENT)
    {
        // Check desync.
        network_check_desynchronization();
    }

    date_update();
    _date = Date(gDateMonthTicks, gDateMonthTicks);

    scenario_update();
    climate_update();
    map_update_tiles();
    // Temporarily remove provisional paths to prevent peep from interacting with them
    map_remove_provisional_elements();
    map_update_path_wide_flags();
    peep_update_all();
    map_restore_provisional_elements();
    vehicle_update_all();
    sprite_misc_update_all();
    ride_update_all();

    if (!(gScreenFlags & (SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER)))
    {
        _park->Update(_date);
    }

    research_update();
    ride_ratings_update_all();
    ride_measurements_update();
    news_item_update_current();

    map_animation_invalidate_all();
    vehicle_sounds_update();
    peep_update_crowd_noise();
    climate_update_sound();
    editor_open_windows_for_current_step();

    // Update windows
    // window_dispatch_update_all();

    if (gErrorType != ERROR_TYPE_NONE)
    {
        rct_string_id title_text = STR_UNABLE_TO_LOAD_FILE;
        rct_string_id body_text = gErrorStringId;
        if (gErrorType == ERROR_TYPE_GENERIC)
        {
            title_text = gErrorStringId;
            body_text = 0xFFFF;
        }
        gErrorType = ERROR_TYPE_NONE;

        context_show_error(title_text, body_text);
    }

    // Start autosave timer after update
    if (gLastAutoSaveUpdate == AUTOSAVE_PAUSE)
    {
        gLastAutoSaveUpdate = Platform::GetTicks();
    }

    // Separated out processing commands in network_update which could call scenario_rand where gInUpdateCode is false.
    // All commands that are received are first queued and then executed where gInUpdateCode is set to true.
    network_process_pending();

    network_flush();

    gCurrentTicks++;
    gScenarioTicks++;
    gSavedAge++;
}
