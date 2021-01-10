/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "PlayerSetGroupAction.h"

#include "../network/network.h"

PlayerSetGroupAction::PlayerSetGroupAction(NetworkPlayerId_t playerId, uint8_t groupId)
    : _playerId(playerId)
    , _groupId(groupId)
{
}

uint16_t PlayerSetGroupAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void PlayerSetGroupAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_playerId) << DS_TAG(_groupId);
}
GameActions::Result::Ptr PlayerSetGroupAction::Query() const
{
    return OpenRCT2::GetContext()->GetNetwork()->SetPlayerGroup(GetPlayer(), _playerId, _groupId, false);
}

GameActions::Result::Ptr PlayerSetGroupAction::Execute() const
{
    return OpenRCT2::GetContext()->GetNetwork()->SetPlayerGroup(GetPlayer(), _playerId, _groupId, true);
}
