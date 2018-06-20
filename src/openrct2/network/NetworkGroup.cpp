/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_NETWORK

#include "NetworkTypes.h"
#include "NetworkAction.h"
#include "NetworkGroup.h"

NetworkGroup NetworkGroup::FromJson(const json_t * json)
{
    NetworkGroup group;
    json_t * jsonId          = json_object_get(json, "id");
    json_t * jsonName        = json_object_get(json, "name");
    json_t * jsonPermissions = json_object_get(json, "permissions");

    if (jsonId == nullptr || jsonName == nullptr || jsonPermissions == nullptr)
    {
        throw std::runtime_error("Missing group data");
    }

    group.Id    = (uint8_t)json_integer_value(jsonId);
    group._name = std::string(json_string_value(jsonName));
    std::fill(group.ActionsAllowed.begin(), group.ActionsAllowed.end(), 0);

    for (size_t i = 0; i < json_array_size(jsonPermissions); i++)
    {
        json_t * jsonPermissionValue = json_array_get(jsonPermissions, i);
        const char * perm_name = json_string_value(jsonPermissionValue);
        if (perm_name == nullptr) {
            continue;
        }
        int32_t action_id = NetworkActions::FindCommandByPermissionName(perm_name);
        if (action_id != -1) {
            group.ToggleActionPermission(action_id);
        }
    }
    return group;
}

json_t * NetworkGroup::ToJson() const
{
    json_t * jsonGroup = json_object();
    json_object_set_new(jsonGroup, "id", json_integer(Id));
    json_object_set_new(jsonGroup, "name", json_string(GetName().c_str()));
    json_t * actionsArray = json_array();
    for (size_t i = 0; i < NetworkActions::Actions.size(); i++)
    {
        if (CanPerformAction(i))
        {
            const char * perm_name = NetworkActions::Actions[i].PermissionName.c_str();
            json_array_append_new(actionsArray, json_string(perm_name));
        }
    }
    json_object_set_new(jsonGroup, "permissions", actionsArray);
    return jsonGroup;
}

const std::string & NetworkGroup::GetName() const
{
    return _name;
}

void NetworkGroup::SetName(std::string name)
{
    _name = name;
}

void NetworkGroup::Read(NetworkPacket &packet)
{
    packet >> Id;
    SetName(packet.ReadString());
    for (auto &action : ActionsAllowed)
    {
        packet >> action;
    }
}

void NetworkGroup::Write(NetworkPacket &packet)
{
    packet << Id;
    packet.WriteString(GetName().c_str());
    for (const auto &action : ActionsAllowed)
    {
        packet << action;
    }
}

void NetworkGroup::ToggleActionPermission(size_t index)
{
    size_t byte = index / 8;
    size_t bit = index % 8;
    if (byte >= ActionsAllowed.size())
    {
        return;
    }
    ActionsAllowed[byte] ^= (1 << bit);
}

bool NetworkGroup::CanPerformAction(size_t index) const
{
    size_t byte = index / 8;
    size_t bit = index % 8;
    if (byte >= ActionsAllowed.size())
    {
        return false;
    }
    return (ActionsAllowed[byte] & (1 << bit)) != 0;
}

bool NetworkGroup::CanPerformCommand(int32_t command) const
{
    int32_t action = NetworkActions::FindCommand(command);
    if (action != -1)
    {
        return CanPerformAction(action);
    }
    return false;
}

#endif
