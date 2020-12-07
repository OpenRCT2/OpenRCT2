/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "PlayerKickAction.h"

#include "../network/network.h"

void PlayerKickAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_playerId);
}
GameActions::Result::Ptr PlayerKickAction::Query() const
{
    return network_kick_player(_playerId, false);
}

GameActions::Result::Ptr PlayerKickAction::Execute() const
{
    return network_kick_player(_playerId, true);
}
