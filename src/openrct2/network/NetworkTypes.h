/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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

namespace OpenRCT2::Network
{
    enum class ServerEvent : uint16_t
    {
        playerJoined,
        playerDisconnected,
    };

    namespace TickFlags
    {
        constexpr uint16_t kChecksums = 1 << 0;
    }

    enum class Mode : int32_t
    {
        none,
        client,
        server
    };

    namespace PlayerFlags
    {
        constexpr uint16_t kIsServer = 1 << 0;
    }

    enum class Status : int32_t
    {
        none,
        ready,
        connecting,
        connected
    };

    enum class Auth : int32_t
    {
        none,
        requested,
        ok,
        badVersion,
        badName,
        badPassword,
        verificationFailure,
        full,
        requirePassword,
        verified,
        unknownKeyDisallowed
    };

    enum class Command : uint32_t
    {
        auth,
        map,
        chat,
        tick = 4,
        playerList,
        ping,
        pingList,
        disconnectMessage,
        gameInfo,
        showError,
        groupList,
        event,
        token,
        objectsList,
        mapRequest,
        gameAction,
        playerInfo,
        requestGameState,
        gameState,
        scriptsHeader, // Deprecated.
        scriptsData,
        heartbeat,
        beginMap,
        max,
        invalid = static_cast<uint32_t>(-1),
    };

    static_assert(Command::gameInfo == static_cast<Command>(9), "Master server expects this to be 9");

    enum class ServerStatus
    {
        ok,
        desynced
    };

    struct ServerState
    {
        ServerStatus state = ServerStatus::ok;
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
    using PlayerId_t = NetworkObjectId<int32_t, 0>;
    using CheatType_t = NetworkObjectId<int32_t, 2>;

    enum class StatisticsGroup : uint32_t
    {
        Total = 0, // Entire network traffic.
        Base,      // Messages such as Tick, Ping
        Commands,  // Command / Game actions
        MapData,
        Max,
    };

    struct Stats
    {
        uint64_t bytesReceived[EnumValue(StatisticsGroup::Max)];
        uint64_t bytesSent[EnumValue(StatisticsGroup::Max)];
    };
} // namespace OpenRCT2::Network
