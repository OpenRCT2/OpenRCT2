/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../network/network.h"
#include "GameAction.h"

enum class ModifyGroupType : uint8_t
{
    AddGroup,
    RemoveGroup,
    SetPermissions,
    SetName,
    SetDefault,
    Count
};

enum class PermissionState : uint8_t
{
    Toggle,
    SetAll,
    ClearAll,
    Count
};

DEFINE_GAME_ACTION(NetworkModifyGroupAction, GAME_COMMAND_MODIFY_GROUPS, GameActionResult)
{
private:
    uint8_t _type{ static_cast<uint8_t>(ModifyGroupType::Count) };
    uint8_t _groupId{ std::numeric_limits<uint8_t>::max() };
    std::string _name;
    uint32_t _permissionIndex{ std::numeric_limits<uint32_t>::max() };
    uint8_t _permissionState{ static_cast<uint8_t>(PermissionState::Count) };

public:
    NetworkModifyGroupAction() = default;

    NetworkModifyGroupAction(
        ModifyGroupType type, uint8_t groupId = std::numeric_limits<uint8_t>::max(), const std::string name = "",
        uint32_t permissionIndex = 0, PermissionState permissionState = PermissionState::Count)
        : _type(static_cast<uint8_t>(type))
        , _groupId(groupId)
        , _name(name)
        , _permissionIndex(permissionIndex)
        , _permissionState(static_cast<uint8_t>(permissionState))
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_type) << DS_TAG(_groupId) << DS_TAG(_name) << DS_TAG(_permissionIndex) << DS_TAG(_permissionState);
    }

    GameActionResult::Ptr Query() const override
    {
        return network_modify_groups(
            GetPlayer(), static_cast<ModifyGroupType>(_type), _groupId, _name, _permissionIndex,
            static_cast<PermissionState>(_permissionState), false);
    }

    GameActionResult::Ptr Execute() const override
    {
        return network_modify_groups(
            GetPlayer(), static_cast<ModifyGroupType>(_type), _groupId, _name, _permissionIndex,
            static_cast<PermissionState>(_permissionState), true);
    }
};
