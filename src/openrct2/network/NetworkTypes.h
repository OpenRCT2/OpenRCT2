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
#include "../core/Endianness.h"
#include "../ride/RideTypes.h"

enum
{
    SERVER_EVENT_PLAYER_JOINED,
    SERVER_EVENT_PLAYER_DISCONNECTED,
};

enum
{
    NETWORK_TICK_FLAG_CHECKSUMS = 1 << 0,
};

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

enum class NetworkCommand : uint32_t
{
    Auth,
    Map,
    Chat,
    Tick = 4,
    PlayerList,
    Ping,
    PingList,
    DisconnectMessage,
    GameInfo,
    ShowError,
    GroupList,
    Event,
    Token,
    ObjectsList,
    MapRequest,
    GameAction,
    PlayerInfo,
    RequestGameState,
    GameState,
    Scripts,
    Heartbeat,
    Max,
    Invalid = static_cast<uint32_t>(-1),
};

static_assert(NetworkCommand::GameInfo == static_cast<NetworkCommand>(9), "Master server expects this to be 9");

enum class NetworkServerState
{
    Ok,
    Desynced
};

struct NetworkServerState_t
{
    NetworkServerState state = NetworkServerState::Ok;
    uint32_t desyncTick = 0;
    uint32_t tick = 0;
    uint32_t srand0 = 0;
    bool gamestateSnapshotsEnabled = false;
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
using NetworkRideId_t = NetworkObjectId_t<ride_id_t, 1>;
using NetworkCheatType_t = NetworkObjectId_t<int32_t, 2>;

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
