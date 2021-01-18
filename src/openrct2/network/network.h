/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#define NETWORK_DEFAULT_PORT 11753
#define NETWORK_LAN_BROADCAST_PORT 11754
#define NETWORK_LAN_BROADCAST_MSG "openrct2.server.query"
#define MAX_SERVER_DESCRIPTION_LENGTH 256

#include "../Game.h"
#include "../common.h"
#include "../core/JsonFwd.hpp"
#include "../localisation/StringIds.h"
#include "NetworkTypes.h"

#include <memory>
#include <string>
#include <vector>

struct GameAction;
struct Peep;
struct CoordsXYZ;
namespace GameActions
{
    class Result;
}
enum class ModifyGroupType : uint8_t;
enum class PermissionState : uint8_t;
enum class NetworkPermission : uint32_t;

namespace OpenRCT2
{
    class NetworkBase;

    struct IPlatformEnvironment;

    struct INetwork
    {
        virtual ~INetwork() = default;

        virtual NetworkBase* GetBase() = 0;

        template<typename T> T* As()
        {
            if (T::NetworkMode == GetMode())
                return static_cast<T*>(GetBase());
            return nullptr;
        }

        virtual bool BeginClient(const std::string& host, int32_t port) = 0;
        virtual bool BeginServer(int32_t port, const std::string& address) = 0;
        virtual void Close() = 0;

        virtual int32_t GetMode() = 0;
        virtual int32_t GetStatus() = 0;
        virtual bool IsDesynchronised() = 0;
        virtual bool CheckDesynchronisation() = 0;
        virtual bool GamestateSnapshotsEnabled() = 0;
        virtual void Update() = 0;
        virtual void ProcessPending() = 0;
        virtual void Flush() = 0;

        virtual NetworkAuth GetAuthStatus() = 0;
        virtual uint32_t GetServerTick() = 0;
        virtual uint8_t GetCurrentPlayerId() = 0;
        virtual int32_t GetNumPlayers() = 0;
        virtual const char* GetPlayerName(uint32_t index) = 0;
        virtual uint32_t GetPlayerFlags(uint32_t index) = 0;
        virtual int32_t GetPlayerPing(uint32_t index) = 0;
        virtual int32_t GetPlayerId(uint32_t index) = 0;
        virtual money32 GetPlayerMoneySpent(uint32_t index) = 0;
        virtual std::string GetPlayerIPAddress(uint32_t id) = 0;
        virtual std::string GetPlayerPublicKeyHash(uint32_t id) = 0;
        virtual void AddPlayerMoneySpent(uint32_t index, money32 cost) = 0;
        virtual int32_t GetPlayerLastAction(uint32_t index, int32_t time) = 0;
        virtual void SetPlayerLastAction(uint32_t index, GameCommand command) = 0;
        virtual CoordsXYZ GetPlayerLastActionCoord(uint32_t index) = 0;
        virtual void SetPlayerLastActionCoord(uint32_t index, const CoordsXYZ& coord) = 0;
        virtual uint32_t GetPlayerCommandsRan(uint32_t index) = 0;
        virtual int32_t GetPlayerIndex(uint32_t id) = 0;
        virtual uint8_t GetPlayerGroup(uint32_t index) = 0;
        virtual void SetPlayerGroup(uint32_t index, uint32_t groupindex) = 0;
        virtual int32_t GetGroupIndex(uint8_t id) = 0;
        virtual int32_t GetCurrentPlayerGroupIndex() = 0;
        virtual uint8_t GetGroupId(uint32_t index) = 0;
        virtual int32_t GetNumGroups() = 0;
        virtual const char* GetGroupName(uint32_t index) = 0;
        virtual std::unique_ptr<GameActions::Result>
            SetPlayerGroup(NetworkPlayerId_t actionPlayerId, NetworkPlayerId_t playerId, uint8_t groupId, bool isExecuting) = 0;
        virtual std::unique_ptr<GameActions::Result> ModifyGroups(
            NetworkPlayerId_t actionPlayerId, ModifyGroupType type, uint8_t groupId, const std::string& name,
            uint32_t permissionIndex, PermissionState permissionState, bool isExecuting)
            = 0;
        virtual std::unique_ptr<GameActions::Result> KickPlayer(NetworkPlayerId_t playerId, bool isExecuting) = 0;
        virtual uint8_t GetDefaultGroup() = 0;
        virtual int32_t GetNumActions() = 0;
        virtual rct_string_id GetActionNameStringId(uint32_t index) = 0;
        virtual int32_t CanPerformAction(uint32_t groupindex, NetworkPermission index) = 0;
        virtual int32_t CanPerformCommand(uint32_t groupindex, int32_t index) = 0;
        virtual void SetPickupPeep(uint8_t playerid, Peep* peep) = 0;
        virtual Peep* GetPickupPeep(uint8_t playerid) = 0;
        virtual void SetPickupPeepOldX(uint8_t playerid, int32_t x) = 0;
        virtual int32_t GetPickupPeepOldX(uint8_t playerid) = 0;

        virtual void SendChat(const char* text, const std::vector<uint8_t>& playerIds = {}) = 0;
        virtual void SendGameAction(const GameAction* action) = 0;

        virtual void SetPassword(const char* password) = 0;

        virtual void AppendChatLog(const utf8* text) = 0;
        virtual void AppendServerLog(const utf8* text) = 0;
        virtual const utf8* GetServerName() = 0;
        virtual const utf8* GetServerDescription() = 0;
        virtual const utf8* GetServerGreeting() = 0;
        virtual const utf8* GetServerProviderName() = 0;
        virtual const utf8* GetServerProviderEmail() = 0;
        virtual const utf8* GetServerProvideWebsite() = 0;

        virtual std::string GetVersion() = 0;

        virtual NetworkStats_t GetStats() = 0;
        virtual NetworkServerState_t GetServerState() = 0;
        virtual json_t GetServerInfoAsJson() = 0;
    };

    std::unique_ptr<INetwork> CreateNetwork(const std::shared_ptr<IPlatformEnvironment>& env);

    // Small helper function until everything lives in the Context.
    bool IsMultiplayerGame();

} // namespace OpenRCT2
