/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/Endianness.h"
#include "../core/EnumUtils.hpp"
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

enum class NetworkAuth : int32_t
{
    None,
    Requested,
    Ok,
    BadVersion,
    BadName,
    BadPassword,
    VerificationFailure,
    Full,
    RequirePassword,
    Verified,
    UnknownKeyDisallowed
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
    ScriptsHeader,
    ScriptsData,
    Heartbeat,
    Max,
    Invalid = static_cast<uint32_t>(-1),
};

static_assert(NetworkCommand::GameInfo == static_cast<NetworkCommand>(9), "Master server expects this to be 9");

enum class NetworkServerStatus
{
    Ok,
    Desynced
};

struct NetworkServerState
{
    NetworkServerStatus state = NetworkServerStatus::Ok;
    uint32_t desyncTick = 0;
    uint32_t tick = 0;
    uint32_t srand0 = 0;
    bool gamestateSnapshotsEnabled = false;
};

// Structure is used for networking specific fields with meaning,
// this structure can be used in combination with DataSerialiser
// to provide extra details with template specialization.
#pragma pack(push, 1)
template<typename T, size_t _TypeID>
struct NetworkObjectId
{
    NetworkObjectId(T v)
        : id(v)
    {
    }
    NetworkObjectId()
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
using NetworkPlayerId_t = NetworkObjectId<int32_t, 0>;
using NetworkCheatType_t = NetworkObjectId<int32_t, 2>;

enum class NetworkStatisticsGroup : uint32_t
{
    Total = 0, // Entire network traffic.
    Base,      // Messages such as Tick, Ping
    Commands,  // Command / Game actions
    MapData,
    Max,
};

struct NetworkStats
{
    uint64_t bytesReceived[EnumValue(NetworkStatisticsGroup::Max)];
    uint64_t bytesSent[EnumValue(NetworkStatisticsGroup::Max)];
};
