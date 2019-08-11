/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Context.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../windows/Intent.h"
#include "../world/Park.h"
#include "GameAction.h"

enum class ManagePlayerGuestMode : uint8_t
{
    Create = 0,
    Remove,
};

DEFINE_GAME_ACTION(ManagePlayerGuestAction, GAME_COMMAND_MANAGE_PLAYER_GUEST, GameActionResult)
{
private:
    uint8_t _mode{ 0 };
    NetworkPlayerId_t _player = -1;

public:
    ManagePlayerGuestAction()
    {
    }

    ManagePlayerGuestAction(ManagePlayerGuestMode mode, NetworkPlayerId_t playerId)
        : _mode(static_cast<uint8_t>(mode))
        , _player(playerId)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_mode) << DS_TAG(_player);
    }

    GameActionResult::Ptr Query() const override
    {
        int32_t playerIndex = network_get_player_index(_player);
        if (playerIndex == -1)
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        return std::make_unique<GameActionResult>();
    }

    GameActionResult::Ptr Execute() const override
    {
        auto mode = static_cast<ManagePlayerGuestMode>(_mode);
        switch (mode)
        {
            case ManagePlayerGuestMode::Create:
                return CreateGuest();
            case ManagePlayerGuestMode::Remove:
                return RemoveGuest();
            default:
                break;
        }
        return std::make_unique<GameActionResult>();
    }

private:
    GameActionResult::Ptr CreateGuest() const
    {
        int32_t playerIndex = network_get_player_index(_player);
        if (playerIndex == -1)
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        const char* name = network_get_player_name(playerIndex);

        Peep* peep = OpenRCT2::GetContext()->GetGameState()->GetPark().GenerateGuest();
        peep->SetName(name);
        peep->peep_flags |= PEEP_FLAGS_CONTROLLED;
        peep->happiness = 255;
        peep->energy = 255;
        peep->hunger = 255;
        peep->thirst = 255;
        peep->nausea = 0;

        network_set_player_controlled_peep(_player, peep);

        if (network_get_current_player_id() == _player)
        {
            auto intent = Intent(WC_PEEP);
            intent.putExtra(INTENT_EXTRA_PEEP, peep);
            context_open_intent(&intent);
        }

        return std::make_unique<GameActionResult>();
    }

    GameActionResult::Ptr RemoveGuest() const
    {
        return std::make_unique<GameActionResult>();
    }
};
