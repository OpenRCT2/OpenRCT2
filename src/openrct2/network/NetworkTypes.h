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

enum
{
    NETWORK_MODE_NONE,
    NETWORK_MODE_CLIENT,
    NETWORK_MODE_SERVER
};

enum
{
    NETWORK_PLAYER_FLAG_ISSERVER = 1 << 0,
};

enum
{
    NETWORK_STATUS_NONE,
    NETWORK_STATUS_READY,
    NETWORK_STATUS_CONNECTING,
    NETWORK_STATUS_CONNECTED
};

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

// Structure is used for networking specific fields with meaning,
// this structure can be used in combination with DataSerialiser
// to provide extra details with template specialization.
#pragma pack(push, 1)
template<typename T, size_t _TypeID> struct NetworkObjectId_t
{
    NetworkObjectId_t(T v)
        : id(v)
    {
    }
    NetworkObjectId_t()
        : id(T(-1))
    {
    }
    operator T() const
    {
        return id;
    }
    T id;
};
#pragma pack(pop)

// NOTE: When adding new types make sure to have no duplicate _TypeID's otherwise
// there is no way to specialize templates if they have the exact symbol.
using NetworkPlayerId_t = NetworkObjectId_t<int32_t, 0>;
using NetworkRideId_t = NetworkObjectId_t<int32_t, 1>;

enum NetworkStatisticsGroup
{
    NETWORK_STATISTICS_GROUP_TOTAL = 0, // Entire network traffic.
    NETWORK_STATISTICS_GROUP_BASE,      // Messages such as Tick, Ping
    NETWORK_STATISTICS_GROUP_COMMANDS,  // Command / Game actions
    NETWORK_STATISTICS_GROUP_MAPDATA,
    NETWORK_STATISTICS_GROUP_MAX,
};

struct NetworkStats_t
{
    uint64_t bytesReceived[NETWORK_STATISTICS_GROUP_MAX];
    uint64_t bytesSent[NETWORK_STATISTICS_GROUP_MAX];
};
