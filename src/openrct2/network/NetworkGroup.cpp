/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_NETWORK

    #include "NetworkGroup.h"

    #include "../openrct2/core/Json.hpp"
    #include "NetworkAction.h"
    #include "NetworkTypes.h"

using namespace OpenRCT2;

NetworkGroup NetworkGroup::FromJson(const json_t& jsonData)
{
    Guard::Assert(jsonData.is_object(), "NetworkGroup::FromJson expects parameter jsonData to be object");

    NetworkGroup group;
    json_t jsonId = jsonData["id"];
    json_t jsonName = jsonData["name"];
    json_t jsonPermissions = jsonData["permissions"];

    if (jsonId.is_null() || jsonName.is_null() || jsonPermissions.is_null())
    {
        throw std::runtime_error("Missing group data");
    }

    group.Id = Json::GetNumber<uint8_t>(jsonId);
    group._name = Json::GetString(jsonName);
    std::fill(group.ActionsAllowed.begin(), group.ActionsAllowed.end(), 0);

    for (const auto& jsonValue : jsonPermissions)
    {
        const std::string permission = Json::GetString(jsonValue);

        NetworkPermission action_id = NetworkActions::FindCommandByPermissionName(permission);
        if (action_id != NetworkPermission::Count)
        {
            group.ToggleActionPermission(action_id);
        }
    }
    return group;
}

json_t NetworkGroup::ToJson() const
{
    json_t jsonGroup = {
        { "id", Id },
        { "name", GetName() },
    };
    json_t actionsArray = json_t::array();
    for (size_t i = 0; i < NetworkActions::Actions.size(); i++)
    {
        if (CanPerformAction(static_cast<NetworkPermission>(i)))
        {
            actionsArray.emplace_back(NetworkActions::Actions[i].PermissionName);
        }
    }
    jsonGroup["permissions"] = actionsArray;
    return jsonGroup;
}

const std::string& NetworkGroup::GetName() const noexcept
{
    return _name;
}

void NetworkGroup::SetName(std::string_view name)
{
    _name = name;
}

void NetworkGroup::Read(NetworkPacket& packet)
{
    packet >> Id;
    SetName(packet.ReadString());
    for (auto& action : ActionsAllowed)
    {
        packet >> action;
    }
}

void NetworkGroup::Write(NetworkPacket& packet) const
{
    packet << Id;
    packet.WriteString(GetName().c_str());
    for (const auto& action : ActionsAllowed)
    {
        packet << action;
    }
}

void NetworkGroup::ToggleActionPermission(NetworkPermission index)
{
    size_t index_st = static_cast<size_t>(index);
    size_t byte = index_st / 8;
    size_t bit = index_st % 8;
    if (byte >= ActionsAllowed.size())
    {
        return;
    }
    ActionsAllowed[byte] ^= (1 << bit);
}

bool NetworkGroup::CanPerformAction(NetworkPermission index) const noexcept
{
    size_t index_st = static_cast<size_t>(index);
    size_t byte = index_st / 8;
    size_t bit = index_st % 8;
    if (byte >= ActionsAllowed.size())
    {
        return false;
    }
    return (ActionsAllowed[byte] & (1 << bit)) != 0;
}

bool NetworkGroup::CanPerformCommand(GameCommand command) const
{
    NetworkPermission action = NetworkActions::FindCommand(command);
    if (action != NetworkPermission::Count)
    {
        return CanPerformAction(action);
    }
    return false;
}

#endif
