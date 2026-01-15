/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Game.h"
#include "../core/JsonFwd.hpp"
#include "../core/Money.hpp"
#include "../localisation/StringIds.h"
#include "NetworkTypes.h"

#include <string>
#include <string_view>
#include <vector>

struct Peep;
struct CoordsXYZ;

namespace OpenRCT2::GameActions
{
    class GameAction;
    enum class ModifyGroupType : uint8_t;
    enum class PermissionState : uint8_t;
    class Result;
} // namespace OpenRCT2::GameActions

namespace OpenRCT2::Network
{
    constexpr uint16_t kDefaultPort = 11753;
    constexpr uint16_t kLanBroadcastPort = 11754;
    constexpr const char* kLanBroadcastMsg = "openrct2.server.query";
    constexpr const char* kMasterServerURL = "https://servers.openrct2.io";
    constexpr uint16_t kMaxServerDescriptionLength = 256;

    enum class Permission : uint32_t;

    void Reconnect();
    void ShutdownClient();
    int32_t BeginClient(const std::string& host, int32_t port);
    int32_t BeginServer(int32_t port, const std::string& address);

    [[nodiscard]] Mode GetMode();
    [[nodiscard]] Status GetStatus();
    bool IsDesynchronised();
    bool CheckDesynchronisation();
    void RequestGamestateSnapshot();
    void SendTick();
    bool GamestateSnapshotsEnabled();
    void Update();
    void Tick();
    void PostTick();
    void Flush();

    [[nodiscard]] Auth GetAuthstatus();
    [[nodiscard]] uint32_t GetServerTick();
    [[nodiscard]] uint8_t GetCurrentPlayerId();
    [[nodiscard]] int32_t GetNumPlayers();
    [[nodiscard]] int32_t GetNumVisiblePlayers();
    [[nodiscard]] const char* GetPlayerName(uint32_t index);
    [[nodiscard]] uint32_t GetPlayerFlags(uint32_t index);
    [[nodiscard]] int32_t GetPlayerPing(uint32_t index);
    [[nodiscard]] int32_t GetPlayerID(uint32_t index);
    [[nodiscard]] money64 GetPlayerMoneySpent(uint32_t index);
    [[nodiscard]] std::string GetPlayerIPAddress(uint32_t id);
    [[nodiscard]] std::string GetPlayerPublicKeyHash(uint32_t id);
    void IncrementPlayerNumCommands(uint32_t playerIndex);
    void AddPlayerMoneySpent(uint32_t index, money64 cost);
    [[nodiscard]] int32_t GetPlayerLastAction(uint32_t index, int32_t time);
    void SetPlayerLastAction(uint32_t index, GameCommand command);
    [[nodiscard]] CoordsXYZ GetPlayerLastActionCoord(uint32_t index);
    void SetPlayerLastActionCoord(uint32_t index, const CoordsXYZ& coord);
    [[nodiscard]] uint32_t GetPlayerCommandsRan(uint32_t index);
    [[nodiscard]] int32_t GetPlayerIndex(uint32_t id);
    [[nodiscard]] uint8_t GetPlayerGroup(uint32_t index);
    void SetPlayerGroup(uint32_t index, uint32_t groupindex);
    [[nodiscard]] int32_t GetGroupIndex(uint8_t id);
    [[nodiscard]] int32_t GetCurrentPlayerGroupIndex();
    [[nodiscard]] uint8_t GetGroupID(uint32_t index);
    [[nodiscard]] int32_t GetNumGroups();
    [[nodiscard]] const char* GetGroupName(uint32_t index);
    [[nodiscard]] GameActions::Result SetPlayerGroup(
        PlayerId_t actionPlayerId, PlayerId_t playerId, uint8_t groupId, bool isExecuting);
    [[nodiscard]] GameActions::Result ModifyGroups(
        PlayerId_t actionPlayerId, GameActions::ModifyGroupType type, uint8_t groupId, const std::string& name,
        uint32_t permissionIndex, GameActions::PermissionState permissionState, bool isExecuting);
    [[nodiscard]] GameActions::Result KickPlayer(PlayerId_t playerId, bool isExecuting);
    [[nodiscard]] uint8_t GetDefaultGroup();
    [[nodiscard]] int32_t GetNumActions();
    [[nodiscard]] StringId GetActionNameStringID(uint32_t index);
    [[nodiscard]] int32_t CanPerformAction(uint32_t groupindex, Permission index);
    [[nodiscard]] int32_t CanPerformCommand(uint32_t groupindex, int32_t index);
    void SetPickupPeep(uint8_t playerid, Peep* peep);
    [[nodiscard]] Peep* GetPickupPeep(uint8_t playerid);
    void SetPickupPeepOldX(uint8_t playerid, int32_t x);
    [[nodiscard]] int32_t GetPickupPeepOldX(uint8_t playerid);
    [[nodiscard]] bool IsServerPlayerInvisible();

    void SendChat(const char* text, const std::vector<uint8_t>& playerIds = {});
    void SendGameAction(const GameActions::GameAction* action);
    void SendPassword(const std::string& password);

    void SetPassword(const char* password);

    void AppendChatLog(std::string_view text);
    void AppendServerLog(const utf8* text);
    [[nodiscard]] u8string GetServerName();
    [[nodiscard]] u8string GetServerDescription();
    [[nodiscard]] u8string GetServerGreeting();
    [[nodiscard]] u8string GetServerProviderName();
    [[nodiscard]] u8string GetServerProviderEmail();
    [[nodiscard]] u8string GetServerProviderWebsite();

    [[nodiscard]] std::string GetVersion();

    [[nodiscard]] Stats GetStats();
    [[nodiscard]] ServerState GetServerState();
    [[nodiscard]] json_t GetServerInfoAsJson();
} // namespace OpenRCT2::Network
