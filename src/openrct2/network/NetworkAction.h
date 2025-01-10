/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Game.h"
#include "../localisation/StringIdType.h"

#include <array>
#include <string>
#include <vector>

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
    StringId Name;
    std::string PermissionName;
    std::vector<GameCommand> Commands;
};

class NetworkActions final
{
public:
    static const std::array<NetworkAction, static_cast<size_t>(NetworkPermission::Count)> Actions;

    static NetworkPermission FindCommand(GameCommand command);
    static NetworkPermission FindCommandByPermissionName(const std::string& permission_name);
};
