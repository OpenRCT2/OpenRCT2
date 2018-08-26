/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../core/Endianness.h"

enum NETWORK_AUTH
{
    NETWORK_AUTH_NONE,
    NETWORK_AUTH_REQUESTED,
    NETWORK_AUTH_OK,
    NETWORK_AUTH_BADVERSION,
    NETWORK_AUTH_BADNAME,
    NETWORK_AUTH_BADPASSWORD,
    NETWORK_AUTH_VERIFICATIONFAILURE,
    NETWORK_AUTH_FULL,
    NETWORK_AUTH_REQUIREPASSWORD,
    NETWORK_AUTH_VERIFIED,
    NETWORK_AUTH_UNKNOWN_KEY_DISALLOWED,
};

enum NETWORK_COMMAND
{
    NETWORK_COMMAND_AUTH,
    NETWORK_COMMAND_MAP,
    NETWORK_COMMAND_CHAT,
    NETWORK_COMMAND_GAMECMD,
    NETWORK_COMMAND_TICK,
    NETWORK_COMMAND_PLAYERLIST,
    NETWORK_COMMAND_PING,
    NETWORK_COMMAND_PINGLIST,
    NETWORK_COMMAND_SETDISCONNECTMSG,
    NETWORK_COMMAND_GAMEINFO,
    NETWORK_COMMAND_SHOWERROR,
    NETWORK_COMMAND_GROUPLIST,
    NETWORK_COMMAND_EVENT,
    NETWORK_COMMAND_TOKEN,
    NETWORK_COMMAND_OBJECTS,
    NETWORK_COMMAND_GAME_ACTION,
    NETWORK_COMMAND_MAX,
    NETWORK_COMMAND_INVALID = -1
};
