/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <vector>
#include <string>
#include "../common.h"

enum MISC_COMMAND
{
    MISC_COMMAND_CHAT = -1,
    MISC_COMMAND_TOGGLE_SCENERY_CLUSTER = -2,
    MISC_COMMAND_PASSWORDLESS_LOGIN = -3,
};

class NetworkAction final
{
public:
    rct_string_id       Name;
    std::string         PermissionName;
    std::vector<int32_t> Commands;
};

class NetworkActions final
{
public:
    static const std::vector<NetworkAction> Actions;

    static int32_t FindCommand(int32_t command);
    static int32_t FindCommandByPermissionName(const std::string &permission_name);
};
