/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "NetworkModifyGroupAction.h"

#include "../network/network.h"
#include "../util/Util.h"

NetworkModifyGroupAction::NetworkModifyGroupAction(
    ModifyGroupType type, uint8_t groupId, const std::string name, uint32_t permissionIndex, PermissionState permissionState)
    : _type(type)
    , _groupId(groupId)
    , _name(name)
    , _permissionIndex(permissionIndex)
    , _permissionState(permissionState)
{
}

uint16_t NetworkModifyGroupAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void NetworkModifyGroupAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_type) << DS_TAG(_groupId) << DS_TAG(_name) << DS_TAG(_permissionIndex) << DS_TAG(_permissionState);
}

GameActions::Result NetworkModifyGroupAction::Query() const
{
    return network_modify_groups(GetPlayer(), _type, _groupId, _name, _permissionIndex, _permissionState, false);
}

GameActions::Result NetworkModifyGroupAction::Execute() const
{
    return network_modify_groups(GetPlayer(), _type, _groupId, _name, _permissionIndex, _permissionState, true);
}
