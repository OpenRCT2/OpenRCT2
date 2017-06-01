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
    std::vector<sint32> Commands;
};

class NetworkActions final
{
public:
    static const std::vector<NetworkAction> Actions;

    static sint32 FindCommand(sint32 command);
    static sint32 FindCommandByPermissionName(const std::string &permission_name);
};
