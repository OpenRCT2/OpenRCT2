#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#ifndef DISABLE_NETWORK

#include "NetworkTypes.h"
#include "NetworkAction.h"
#include "NetworkGroup.h"
#include "../core/Exception.hpp"

NetworkGroup::NetworkGroup()
{
	ActionsAllowed = { 0 };
}

NetworkGroup::~NetworkGroup()
{
}

NetworkGroup NetworkGroup::FromJson(const json_t * json)
{
    NetworkGroup group;
    json_t * jsonId = json_object_get(json, "id");
    json_t * jsonName = json_object_get(json, "name");
    json_t * jsonPermissions = json_object_get(json, "permissions");
    if (jsonId == nullptr || jsonName == nullptr || jsonPermissions == nullptr)
    {
        throw Exception("Missing group data");
    }
    group.Id = (uint8)json_integer_value(jsonId);
    group._name = std::string(json_string_value(jsonName));
    for (size_t i = 0; i < group.ActionsAllowed.size(); i++) {
        group.ActionsAllowed[i] = 0;
    }
    for (size_t i = 0; i < json_array_size(jsonPermissions); i++) {
        json_t * jsonPermissionValue = json_array_get(jsonPermissions, i);
        const char * perm_name = json_string_value(jsonPermissionValue);
        if (perm_name == nullptr) {
            continue;
        }
        sint32 action_id = NetworkActions::FindCommandByPermissionName(perm_name);
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
    for (size_t i = 0; i < ActionsAllowed.size(); i++)
    {
        packet >> ActionsAllowed[i];
    }
}

void NetworkGroup::Write(NetworkPacket &packet)
{
    packet << Id;
    packet.WriteString(GetName().c_str());
    for (size_t i = 0; i < ActionsAllowed.size(); i++)
    {
        packet << ActionsAllowed[i];
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
    return (ActionsAllowed[byte] & (1 << bit)) ? true : false;
}

bool NetworkGroup::CanPerformCommand(sint32 command) const
{
    sint32 action = NetworkActions::FindCommand(command);
    if (action != -1)
    {
        return CanPerformAction(action);
    }
    return false;
}

#endif
