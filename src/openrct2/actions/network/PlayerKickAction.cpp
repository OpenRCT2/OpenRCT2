/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "PlayerKickAction.h"

#include "../network/Network.h"

namespace OpenRCT2::GameActions
{
    PlayerKickAction::PlayerKickAction(Network::PlayerId_t playerId)
        : _playerId(playerId)
    {
    }

    void PlayerKickAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit("playerId", _playerId);
    }

    uint16_t PlayerKickAction::GetActionFlags() const
    {
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
    }

    void PlayerKickAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_playerId);
    }
    Result PlayerKickAction::Query(GameState_t& gameState) const
    {
        return Network::KickPlayer(_playerId, false);
    }

    Result PlayerKickAction::Execute(GameState_t& gameState) const
    {
        return Network::KickPlayer(_playerId, true);
    }
} // namespace OpenRCT2::GameActions
