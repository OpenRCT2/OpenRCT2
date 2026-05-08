/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_NETWORK

    #include "NetworkGroup.h"

    #include "../core/Guard.hpp"
    #include "../core/Json.hpp"
    #include "NetworkAction.h"
    #include "NetworkTypes.h"

namespace OpenRCT2::Network
{
    NetworkGroup NetworkGroup::fromJson(const json_t& jsonData)
    {
        Guard::Assert(jsonData.is_object(), "NetworkGroup::fromJson expects parameter jsonData to be object");

        NetworkGroup group;
        json_t jsonId = jsonData["id"];
        json_t jsonName = jsonData["name"];
        json_t jsonPermissions = jsonData["permissions"];

        if (jsonId.is_null() || jsonName.is_null() || jsonPermissions.is_null())
        {
            throw std::runtime_error("Missing group data");
        }

        group.id = Json::GetNumber<uint8_t>(jsonId);
        group._name = Json::GetString(jsonName);
        std::fill(group.actionsAllowed.begin(), group.actionsAllowed.end(), 0);

        for (const auto& jsonValue : jsonPermissions)
        {
            const std::string permission = Json::GetString(jsonValue);

            Permission action_id = NetworkActions::findCommandByPermissionName(permission);
            if (action_id != Permission::count)
            {
                group.toggleActionPermission(action_id);
            }
        }
        return group;
    }

    json_t NetworkGroup::toJson() const
    {
        json_t jsonGroup = {
            { "id", id },
            { "name", getName() },
        };
        json_t actionsArray = json_t::array();
        for (size_t i = 0; i < NetworkActions::kActions.size(); i++)
        {
            if (canPerformAction(static_cast<Permission>(i)))
            {
                actionsArray.emplace_back(NetworkActions::kActions[i].permissionName);
            }
        }
        jsonGroup["permissions"] = actionsArray;
        return jsonGroup;
    }

    const std::string& NetworkGroup::getName() const noexcept
    {
        return _name;
    }

    void NetworkGroup::setName(std::string_view name)
    {
        _name = name;
    }

    void NetworkGroup::read(Packet& packet)
    {
        packet >> id;
        setName(packet.ReadString());
        for (auto& action : actionsAllowed)
        {
            packet >> action;
        }
    }

    void NetworkGroup::write(Packet& packet) const
    {
        packet << id;
        packet.WriteString(getName().c_str());
        for (const auto& action : actionsAllowed)
        {
            packet << action;
        }
    }

    void NetworkGroup::toggleActionPermission(Permission index)
    {
        size_t index_st = static_cast<size_t>(index);
        size_t byte = index_st / 8;
        size_t bit = index_st % 8;
        if (byte >= actionsAllowed.size())
        {
            return;
        }
        actionsAllowed[byte] ^= (1 << bit);
    }

    bool NetworkGroup::canPerformAction(Permission index) const noexcept
    {
        size_t index_st = static_cast<size_t>(index);
        size_t byte = index_st / 8;
        size_t bit = index_st % 8;
        if (byte >= actionsAllowed.size())
        {
            return false;
        }
        return (actionsAllowed[byte] & (1 << bit)) != 0;
    }

    bool NetworkGroup::canPerformCommand(GameCommand command) const
    {
        Permission action = NetworkActions::findCommand(command);
        if (action != Permission::count)
        {
            return canPerformAction(action);
        }
        return false;
    }
} // namespace OpenRCT2::Network

#endif
