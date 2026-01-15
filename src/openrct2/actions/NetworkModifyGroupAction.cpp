/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "NetworkModifyGroupAction.h"

#include "../network/Network.h"

namespace OpenRCT2::GameActions
{
    NetworkModifyGroupAction::NetworkModifyGroupAction(
        ModifyGroupType type, uint8_t groupId, const std::string name, uint32_t permissionIndex,
        PermissionState permissionState)
        : _type(type)
        , _groupId(groupId)
        , _name(name)
        , _permissionIndex(permissionIndex)
        , _permissionState(permissionState)
    {
    }

    void NetworkModifyGroupAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit("type", _type);
        visitor.Visit("groupId", _groupId);
        visitor.Visit("name", _name);
        visitor.Visit("permissionIndex", _permissionIndex);
        visitor.Visit("permissionState", _permissionState);
    }

    uint16_t NetworkModifyGroupAction::GetActionFlags() const
    {
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
    }

    void NetworkModifyGroupAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_type) << DS_TAG(_groupId) << DS_TAG(_name) << DS_TAG(_permissionIndex) << DS_TAG(_permissionState);
    }

    Result NetworkModifyGroupAction::Query(GameState_t& gameState) const
    {
        return Network::ModifyGroups(GetPlayer(), _type, _groupId, _name, _permissionIndex, _permissionState, false);
    }

    Result NetworkModifyGroupAction::Execute(GameState_t& gameState) const
    {
        return Network::ModifyGroups(GetPlayer(), _type, _groupId, _name, _permissionIndex, _permissionState, true);
    }
} // namespace OpenRCT2::GameActions
