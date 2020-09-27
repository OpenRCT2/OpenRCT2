/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

#include <array>
#include <string>
#include <vector>

enum MISC_COMMAND
{
    MISC_COMMAND_CHAT = -1,
    MISC_COMMAND_TOGGLE_SCENERY_CLUSTER = -2,
    MISC_COMMAND_PASSWORDLESS_LOGIN = -3,
};

enum class NetworkPermission : uint32_t
{
    Chat,
    Terraform,
    SetWaterLevel,
    TogglePause,
    CreateRide,
    RemoveRide,
    BuildRide,
    RideProperties,
    Scenery,
    Path,
    ClearLandscape,
    Guest,
    Staff,
    ParkProperties,
    ParkFunding,
    KickPlayer,
    ModifyGroups,
    SetPlayerGroup,
    Cheat,
    ToggleSceneryCluster,
    PasswordlessLogin,
    ModifyTile,
    EditScenarioOptions,

    Count
};

class NetworkAction final
{
public:
    rct_string_id Name;
    std::string PermissionName;
    std::vector<int32_t> Commands;
};

class NetworkActions final
{
public:
    static const std::array<NetworkAction, static_cast<size_t>(NetworkPermission::Count)> Actions;

    static NetworkPermission FindCommand(int32_t command);
    static NetworkPermission FindCommandByPermissionName(const std::string& permission_name);
};
