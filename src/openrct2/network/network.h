/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
#include <string_view>
#include <vector>

class GameAction;
struct Peep;
struct CoordsXYZ;
namespace GameActions
{
    class Result;
}
enum class ModifyGroupType : uint8_t;
enum class PermissionState : uint8_t;
enum class NetworkPermission : uint32_t;

void NetworkReconnect();
void NetworkShutdownClient();
int32_t NetworkBeginClient(const std::string& host, int32_t port);
int32_t NetworkBeginServer(int32_t port, const std::string& address);

[[nodiscard]] int32_t NetworkGetMode();
[[nodiscard]] int32_t NetworkGetStatus();
bool NetworkIsDesynchronised();
bool NetworkCheckDesynchronisation();
void NetworkRequestGamestateSnapshot();
void NetworkSendTick();
bool NetworkGamestateSnapshotsEnabled();
void NetworkUpdate();
void NetworkProcessPending();
void NetworkFlush();

[[nodiscard]] NetworkAuth NetworkGetAuthstatus();
[[nodiscard]] uint32_t NetworkGetServerTick();
[[nodiscard]] uint8_t NetworkGetCurrentPlayerId();
[[nodiscard]] int32_t NetworkGetNumPlayers();
[[nodiscard]] int32_t NetworkGetNumVisiblePlayers();
[[nodiscard]] const char* NetworkGetPlayerName(uint32_t index);
[[nodiscard]] uint32_t NetworkGetPlayerFlags(uint32_t index);
[[nodiscard]] int32_t NetworkGetPlayerPing(uint32_t index);
[[nodiscard]] int32_t NetworkGetPlayerID(uint32_t index);
[[nodiscard]] money64 NetworkGetPlayerMoneySpent(uint32_t index);
[[nodiscard]] std::string NetworkGetPlayerIPAddress(uint32_t id);
[[nodiscard]] std::string NetworkGetPlayerPublicKeyHash(uint32_t id);
void NetworkAddPlayerMoneySpent(uint32_t index, money64 cost);
[[nodiscard]] int32_t NetworkGetPlayerLastAction(uint32_t index, int32_t time);
void NetworkSetPlayerLastAction(uint32_t index, GameCommand command);
[[nodiscard]] CoordsXYZ NetworkGetPlayerLastActionCoord(uint32_t index);
void NetworkSetPlayerLastActionCoord(uint32_t index, const CoordsXYZ& coord);
[[nodiscard]] uint32_t NetworkGetPlayerCommandsRan(uint32_t index);
[[nodiscard]] int32_t NetworkGetPlayerIndex(uint32_t id);
[[nodiscard]] uint8_t NetworkGetPlayerGroup(uint32_t index);
void NetworkSetPlayerGroup(uint32_t index, uint32_t groupindex);
[[nodiscard]] int32_t NetworkGetGroupIndex(uint8_t id);
[[nodiscard]] int32_t NetworkGetCurrentPlayerGroupIndex();
[[nodiscard]] uint8_t NetworkGetGroupID(uint32_t index);
[[nodiscard]] int32_t NetworkGetNumGroups();
[[nodiscard]] const char* NetworkGetGroupName(uint32_t index);
[[nodiscard]] GameActions::Result NetworkSetPlayerGroup(
    NetworkPlayerId_t actionPlayerId, NetworkPlayerId_t playerId, uint8_t groupId, bool isExecuting);
[[nodiscard]] GameActions::Result NetworkModifyGroups(
    NetworkPlayerId_t actionPlayerId, ModifyGroupType type, uint8_t groupId, const std::string& name, uint32_t permissionIndex,
    PermissionState permissionState, bool isExecuting);
[[nodiscard]] GameActions::Result NetworkKickPlayer(NetworkPlayerId_t playerId, bool isExecuting);
[[nodiscard]] uint8_t NetworkGetDefaultGroup();
[[nodiscard]] int32_t NetworkGetNumActions();
[[nodiscard]] StringId NetworkGetActionNameStringID(uint32_t index);
[[nodiscard]] int32_t NetworkCanPerformAction(uint32_t groupindex, NetworkPermission index);
[[nodiscard]] int32_t NetworkCanPerformCommand(uint32_t groupindex, int32_t index);
void NetworkSetPickupPeep(uint8_t playerid, Peep* peep);
[[nodiscard]] Peep* NetworkGetPickupPeep(uint8_t playerid);
void NetworkSetPickupPeepOldX(uint8_t playerid, int32_t x);
[[nodiscard]] int32_t NetworkGetPickupPeepOldX(uint8_t playerid);
[[nodiscard]] bool NetworkIsServerPlayerInvisible();

void NetworkSendChat(const char* text, const std::vector<uint8_t>& playerIds = {});
void NetworkSendGameAction(const GameAction* action);
void NetworkSendPassword(const std::string& password);

void NetworkSetPassword(const char* password);

void NetworkAppendChatLog(std::string_view text);
void NetworkAppendServerLog(const utf8* text);
[[nodiscard]] u8string NetworkGetServerName();
[[nodiscard]] u8string NetworkGetServerDescription();
[[nodiscard]] u8string NetworkGetServerGreeting();
[[nodiscard]] u8string NetworkGetServerProviderName();
[[nodiscard]] u8string NetworkGetServerProviderEmail();
[[nodiscard]] u8string NetworkGetServerProviderWebsite();

[[nodiscard]] std::string NetworkGetVersion();

[[nodiscard]] NetworkStats NetworkGetStats();
[[nodiscard]] NetworkServerState NetworkGetServerState();
[[nodiscard]] json_t NetworkGetServerInfoAsJson();
