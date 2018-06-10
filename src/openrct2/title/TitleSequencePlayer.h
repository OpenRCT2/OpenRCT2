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

#pragma once

#include "../common.h"

namespace OpenRCT2
{
    class GameState;
}

interface IScenarioRepository;

interface ITitleSequencePlayer
{
    virtual ~ITitleSequencePlayer() = default;

    virtual sint32 GetCurrentPosition() const abstract;

    virtual bool Begin(size_t titleSequenceId) abstract;
    virtual void Reset() abstract;
    virtual bool Update() abstract;
    virtual void Seek(sint32 position) abstract;
    virtual void Eject() abstract;
};

ITitleSequencePlayer * CreateTitleSequencePlayer(IScenarioRepository& scenarioRepository, OpenRCT2::GameState& gameState);

// When testing title sequences within a normal game
extern bool gPreviewingTitleSequenceInGame;

sint32 title_sequence_player_get_current_position(ITitleSequencePlayer * player);
bool title_sequence_player_begin(ITitleSequencePlayer * player, uint32 titleSequenceId);
void title_sequence_player_reset(ITitleSequencePlayer * player);
bool title_sequence_player_update(ITitleSequencePlayer * player);
void title_sequence_player_seek(ITitleSequencePlayer * player, uint32 position);
