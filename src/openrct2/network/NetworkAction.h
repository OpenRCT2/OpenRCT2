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

enum NETWORK_PERMISSION
{
    NETWORK_PERMISSION_CHAT,
    NETWORK_PERMISSION_TERRAFORM,
    NETWORK_PERMISSION_SET_WATER_LEVEL,
    NETWORK_PERMISSION_TOGGLE_PAUSE,
    NETWORK_PERMISSION_CREATE_RIDE,
    NETWORK_PERMISSION_REMOVE_RIDE,
    NETWORK_PERMISSION_BUILD_RIDE,
    NETWORK_PERMISSION_RIDE_PROPERTIES,
    NETWORK_PERMISSION_SCENERY,
    NETWORK_PERMISSION_PATH,
    NETWORK_PERMISSION_CLEAR_LANDSCAPE,
    NETWORK_PERMISSION_GUEST,
    NETWORK_PERMISSION_STAFF,
    NETWORK_PERMISSION_PARK_PROPERTIES,
    NETWORK_PERMISSION_PARK_FUNDING,
    NETWORK_PERMISSION_KICK_PLAYER,
    NETWORK_PERMISSION_MODIFY_GROUPS,
    NETWORK_PERMISSION_SET_PLAYER_GROUP,
    NETWORK_PERMISSION_CHEAT,
    NETWORK_PERMISSION_TOGGLE_SCENERY_CLUSTER,
    NETWORK_PERMISSION_PASSWORDLESS_LOGIN,
    NETWORK_PERMISSION_MODIFY_TILE,
    NETWORK_PERMISSION_EDIT_SCENARIO_OPTIONS,

    NETWORK_PERMISSION_COUNT,
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
    static const std::array<NetworkAction, NETWORK_PERMISSION_COUNT> Actions;

    static int32_t FindCommand(int32_t command);
    static int32_t FindCommandByPermissionName(const std::string& permission_name);
};
