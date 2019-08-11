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

enum class GuestPlayerCommandType : uint8_t
{
    MoveTo = 0,
    QueueRide,
};

DEFINE_GAME_ACTION(GuestPlayerCommandAction, GAME_COMMAND_GUEST_COMMAND, GameActionResult)
{
private:
    uint8_t _mode{ 0 };
    CoordsXYZ _coords;
    NetworkRideId_t _rideId = -1;

public:
    GuestPlayerCommandAction()
    {
    }

    GuestPlayerCommandAction(GuestPlayerCommandType mode, const CoordsXYZ& coords)
        : _mode(static_cast<uint8_t>(mode))
        , _coords(coords)
    {
    }

    GuestPlayerCommandAction(GuestPlayerCommandType mode, NetworkRideId_t rideId)
        : _mode(static_cast<uint8_t>(mode))
        , _rideId(rideId)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_mode) << DS_TAG(_coords) << DS_TAG(_rideId);
    }

    GameActionResult::Ptr Query() const override
    {
        Peep* peep = GetPeep();
        if (peep == nullptr)
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        return MakeResult(GA_ERROR::OK, STR_NONE);
    }

    GameActionResult::Ptr Execute() const override
    {
        Peep* peep = GetPeep();
        if (peep == nullptr)
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        auto mode = static_cast<GuestPlayerCommandType>(_mode);
        switch (mode)
        {
            case GuestPlayerCommandType::MoveTo:
                return MoveTo(peep);
            case GuestPlayerCommandType::QueueRide:
                return QueueRide(peep);
            default:
                break;
        }
        return MakeResult(GA_ERROR::OK, STR_NONE);
    }

private:
    GameActionResult::Ptr MoveTo(Peep * peep) const
    {
        peep->state = PEEP_STATE_WALKING;
        peep->destination_x = _coords.x;
        peep->destination_y = _coords.y;

        return MakeResult(GA_ERROR::OK, STR_NONE);
    }

    GameActionResult::Ptr QueueRide(Peep * peep) const
    {
        return MakeResult(GA_ERROR::OK, STR_NONE);
    }

    Peep* GetPeep() const
    {
        NetworkPlayerId_t playerId = GetPlayer();

        // Before we are networked we have no player id assigned or are host.
        if (playerId == -1)
            playerId = network_get_current_player_id();

        return network_get_player_controlled_peep(playerId);
    }
};
