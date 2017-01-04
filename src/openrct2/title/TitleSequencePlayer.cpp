#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#include "../common.h"
#include <SDL.h>
#include "../core/Console.hpp"
#include "../core/Exception.hpp"
#include "../core/Math.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../scenario/ScenarioRepository.h"
#include "../scenario/ScenarioSources.h"
#include "TitleSequence.h"
#include "TitleSequenceManager.h"
#include "TitleSequencePlayer.h"

extern "C"
{
    #include "../game.h"
    #include "../interface/viewport.h"
    #include "../interface/window.h"
    #include "../world/scenery.h"
}

class TitleSequencePlayer : public ITitleSequencePlayer
{
private:
    uint32          _sequenceId = 0;
    TitleSequence * _sequence = nullptr;
    sint32          _position = 0;
    sint32          _waitCounter = 0;

    sint32          _lastScreenWidth = 0;
    sint32          _lastScreenHeight = 0;
    rct_xy32        _viewCentreLocation = { 0 };

public:
    ~TitleSequencePlayer() override
    {
        Eject();
    }

    sint32 GetCurrentPosition() const override
    {
        return _position;
    }

    void Eject() override
    {
        FreeTitleSequence(_sequence);
        _sequence = nullptr;
    }

    bool Begin(uint32 titleSequenceId) override
    {
        size_t numSequences = TitleSequenceManager::GetCount();
        if (titleSequenceId >= numSequences)
        {
            return false;
        }

        auto seqItem = TitleSequenceManager::GetItem(titleSequenceId);
        auto sequence = LoadTitleSequence(seqItem->Path.c_str());
        if (sequence == nullptr)
        {
            return false;
        }

        Eject();
        _sequence = sequence;
        _sequenceId = titleSequenceId;

        Reset();
        return true;
    }

    bool Update() override
    {
        sint32 entryPosition = _position;
        FixViewLocation();

        if (_sequence == nullptr)
        {
            SetViewLocation(75 * 32, 75 * 32);
            return false;
        }

        // Check that position is valid
        if (_position >= (sint32)_sequence->NumCommands)
        {
            _position = 0;
            return false;
        }

        // Don't execute next command until we are done with the current wait command
        if (_waitCounter != 0)
        {
            _waitCounter--;
            if (_waitCounter == 0)
            {
                const TitleCommand * command = &_sequence->Commands[_position];
                if (command->Type == TITLE_SCRIPT_WAIT)
                {
                    IncrementPosition();
                }
            }
        }
        else
        {
            while (true)
            {
                const TitleCommand * command = &_sequence->Commands[_position];
                if (ExecuteCommand(command))
                {
                    if (command->Type == TITLE_SCRIPT_WAIT)
                    {
                        break;
                    }
                    if (command->Type != TITLE_SCRIPT_RESTART)
                    {
                        IncrementPosition();
                    }
                    if (_position == entryPosition)
                    {
                        Console::Error::WriteLine("Infinite loop detected in title sequence.");
                        Console::Error::WriteLine("  A wait command may be missing.");
                        return false;
                    }
                }
                else
                {
                    SkipToNextLoadCommand();
                    if (_position == entryPosition)
                    {
                        Console::Error::WriteLine("Unable to load any parks within title sequence.");
                        return false;
                    }
                }
            }
        }
        return true;
    }

    void Reset() override
    {
        _position = 0;
        _waitCounter = 0;
    }

    void Seek(sint32 targetPosition) override
    {
        if (targetPosition < 0 || targetPosition >= (sint32)_sequence->NumCommands)
        {
            throw Exception("Invalid position.");
        }
        if (_position >= targetPosition)
        {
            Reset();
        }

        // Set position to the last LOAD command before target position
        for (sint32 i = targetPosition; i >= 0; i--)
        {
            const TitleCommand * command = &_sequence->Commands[i];
            if (TitleSequenceIsLoadCommand(command))
            {
                _position = i;
                break;
            }
        }

        // Keep updating until we reach target position
        while (_position < targetPosition)
        {
            if (Update())
            {
                game_logic_update();
            }
            else
            {
                break;
            }
        }

        _waitCounter = 0;
    }

private:
    void IncrementPosition()
    {
        _position++;
        if (_position >= (sint32)_sequence->NumCommands)
        {
            _position = 0;
        }
    }

    void SkipToNextLoadCommand()
    {
        const TitleCommand * command;
        do
        {
            IncrementPosition();
            command = &_sequence->Commands[_position];
        }
        while (!TitleSequenceIsLoadCommand(command));
    }

    bool ExecuteCommand(const TitleCommand * command)
    {
        switch (command->Type) {
        case TITLE_SCRIPT_END:
            _waitCounter = 1;
            break;
        case TITLE_SCRIPT_WAIT:
            _waitCounter = command->Seconds * 32;
            break;
        case TITLE_SCRIPT_LOADMM:
        {
            const utf8 * path = get_file_path(PATH_ID_SIXFLAGS_MAGICMOUNTAIN);
            if (!LoadParkFromFile(path))
            {
                Console::Error::WriteLine("Failed to load: \"%s\" for the title sequence.", path);
                return false;
            }
            break;
        }
        case TITLE_SCRIPT_LOCATION:
        {
            sint32 x = command->X * 32 + 16;
            sint32 y = command->Y * 32 + 16;
            SetViewLocation(x, y);
            break;
        }
        case TITLE_SCRIPT_ROTATE:
            RotateView(command->Rotations);
            break;
        case TITLE_SCRIPT_ZOOM:
            SetViewZoom(command->Zoom);
            break;
        case TITLE_SCRIPT_SPEED:
            gGameSpeed = Math::Clamp<uint8>(1, command->Speed, 4);
            break;
        case TITLE_SCRIPT_RESTART:
            Reset();
            break;
        case TITLE_SCRIPT_LOAD:
        {
            bool loadSuccess = false;
            uint8 saveIndex = command->SaveIndex;
            TitleSequenceParkHandle * parkHandle = TitleSequenceGetParkHandle(_sequence, saveIndex);
            if (parkHandle != nullptr)
            {
                loadSuccess = LoadParkFromRW(parkHandle->RWOps, parkHandle->IsScenario);
                TitleSequenceCloseParkHandle(parkHandle);
            }
            if (!loadSuccess)
            {
                if (_sequence->NumSaves > saveIndex)
                {
                    const utf8 * path = _sequence->Saves[saveIndex];
                    Console::Error::WriteLine("Failed to load: \"%s\" for the title sequence.", path);
                }
                return false;
            }
            break;
        }
        case TITLE_SCRIPT_LOADRCT1:
        {
            source_desc sourceDesc;
            if (!ScenarioSources::TryGetById(command->SaveIndex, &sourceDesc) || sourceDesc.index == -1)
            {
                Console::Error::WriteLine("Invalid scenario id.");
                return false;
            }

            const utf8 * path = nullptr;
            IScenarioRepository * scenarioRepo = GetScenarioRepository();
            size_t numScenarios =  scenarioRepo->GetCount();
            for (size_t i = 0; i < numScenarios; i++)
            {
                const scenario_index_entry * scenario = scenarioRepo->GetByIndex(i);
                if (scenario->source_index == sourceDesc.index)
                {
                    path = scenario->path;
                    break;
                }
            }
            if (path == nullptr || !LoadParkFromFile(path))
            {
                return false;
            }
            break;
        }
        }
        return true;
    }

    void SetViewZoom(const uint32 &zoom)
    {
        rct_window * w = window_get_main();
        if (w != nullptr && w->viewport != nullptr)
        {
            window_zoom_set(w, zoom);
        }
    }

    void RotateView(uint32 count)
    {
        rct_window * w = window_get_main();
        if (w != nullptr)
        {
            for (uint32 i = 0; i < count; i++)
            {
                window_rotate_camera(w, 1);
            }
        }
    }

    bool LoadParkFromFile(const utf8 * path)
    {
        bool success = false;
        bool isScenario = String::Equals(Path::GetExtension(path), ".sc6", true);
        SDL_RWops * rw = SDL_RWFromFile(path, "rb");
        if (rw != nullptr)
        {
            success = LoadParkFromRW(rw, isScenario);
            SDL_RWclose(rw);
        }
        return success;
    }

    bool LoadParkFromRW(SDL_RWops * rw, bool isScenario)
    {
        bool successfulLoad = isScenario ? scenario_load_rw(rw) :
                                           game_load_sv6(rw);
        if (!successfulLoad)
        {
            return false;
        }

        rct_window * w = window_get_main();
        w->viewport_target_sprite = -1;
        w->saved_view_x = gSavedViewX;
        w->saved_view_y = gSavedViewY;

        char zoomDifference = gSavedViewZoom - w->viewport->zoom;
        w->viewport->zoom = gSavedViewZoom;
        gCurrentRotation = gSavedViewRotation;
        if (zoomDifference != 0)
        {
            if (zoomDifference < 0)
            {
                zoomDifference = -zoomDifference;
                w->viewport->view_width >>= zoomDifference;
                w->viewport->view_height >>= zoomDifference;
            }
            else
            {
                w->viewport->view_width <<= zoomDifference;
                w->viewport->view_height <<= zoomDifference;
            }
        }
        w->saved_view_x -= w->viewport->view_width >> 1;
        w->saved_view_y -= w->viewport->view_height >> 1;

        window_invalidate(w);
        reset_sprite_spatial_index();
        reset_all_sprite_quadrant_placements();
        window_new_ride_init_vars();
        if (!isScenario)
        {
            sub_684AC3();
        }
        scenery_set_default_placement_configuration();
        news_item_init_queue();
        load_palette();
        gfx_invalidate_screen();
        gScreenAge = 0;
        gGameSpeed = 1;
        return true;
    }

    /**
     * Sets the map location to the given tile coordinates. Z is automatic.
     * @param x X position in map tiles.
     * @param y Y position in map tiles.
     */
    void SetViewLocation(sint32 x, sint32 y)
    {
        // Update viewport
        rct_window * w = window_get_main();
        if (w != nullptr)
        {
            sint32 z = map_element_height(x, y);
            window_scroll_to_location(w, x, y, z);
            w->flags &= ~WF_SCROLLING_TO_LOCATION;
            viewport_update_position(w);
        }

        // Save known tile position in case of window resize
        _lastScreenWidth = gScreenWidth;
        _lastScreenHeight = gScreenHeight;
        _viewCentreLocation.x = x;
        _viewCentreLocation.y = y;
    }

    /**
     * Fixes the view location for when the game window has changed size.
     */
    void FixViewLocation()
    {
        if (gScreenWidth != _lastScreenWidth ||
            gScreenHeight != _lastScreenHeight)
        {
            SetViewLocation(_viewCentreLocation.x, _viewCentreLocation.y);
        }
    }
};

ITitleSequencePlayer * CreateTitleSequencePlayer()
{
    return new TitleSequencePlayer();
}

extern "C"
{
    sint32 title_sequence_player_get_current_position(ITitleSequencePlayer * player)
    {
        return player->GetCurrentPosition();
    }

    bool title_sequence_player_begin(ITitleSequencePlayer * player, uint32 titleSequenceId)
    {
        return player->Begin(titleSequenceId);
    }

    void title_sequence_player_reset(ITitleSequencePlayer * player)
    {
        player->Reset();
    }

    bool title_sequence_player_update(ITitleSequencePlayer * player)
    {
        return player->Update();
    }

    void title_sequence_player_seek(ITitleSequencePlayer * player, uint32 position)
    {
        player->Seek(position);
    }
}
