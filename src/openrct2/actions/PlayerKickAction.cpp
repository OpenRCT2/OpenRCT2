/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "PlayerKickAction.h"

#include "../network/network.h"

using namespace OpenRCT2;

PlayerKickAction::PlayerKickAction(NetworkPlayerId_t playerId)
    : _playerId(playerId)
{
}

void PlayerKickAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit("playerId", _playerId);
}

uint16_t PlayerKickAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void PlayerKickAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_playerId);
}
GameActions::Result PlayerKickAction::Query() const
{
    return NetworkKickPlayer(_playerId, false);
}

GameActions::Result PlayerKickAction::Execute() const
{
    return NetworkKickPlayer(_playerId, true);
}
