/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "PlayerSetGroupAction.h"

#include "../network/Network.h"

namespace OpenRCT2::GameActions
{
    PlayerSetGroupAction::PlayerSetGroupAction(Network::PlayerId_t playerId, uint8_t groupId)
        : _playerId(playerId)
        , _groupId(groupId)
    {
    }

    void PlayerSetGroupAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit("playerId", _playerId);
        visitor.Visit("groupId", _groupId);
    }

    uint16_t PlayerSetGroupAction::GetActionFlags() const
    {
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
    }

    void PlayerSetGroupAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_playerId) << DS_TAG(_groupId);
    }
    Result PlayerSetGroupAction::Query(GameState_t& gameState) const
    {
        return Network::SetPlayerGroup(GetPlayer(), _playerId, _groupId, false);
    }

    Result PlayerSetGroupAction::Execute(GameState_t& gameState) const
    {
        return Network::SetPlayerGroup(GetPlayer(), _playerId, _groupId, true);
    }
} // namespace OpenRCT2::GameActions
