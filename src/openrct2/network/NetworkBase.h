#pragma once

#include "../System.hpp"
#include "../actions/GameAction.h"
#include "../object/Object.h"
#include "NetworkConnection.h"
#include "NetworkGroup.h"
#include "NetworkPlayer.h"
#include "NetworkServerAdvertiser.h"
#include "NetworkTypes.h"
#include "NetworkUser.h"

#include <fstream>
#include <memory>

#ifndef DISABLE_NETWORK

namespace OpenRCT2
{
    struct IContext;
}

class NetworkBase : public OpenRCT2::System
{
public:
    NetworkBase(OpenRCT2::IContext& context);

public: // Uncategorized
    bool BeginServer(uint16_t port, const std::string& address);
    bool BeginClient(const std::string& host, uint16_t port);

public: // Common
    bool Init();
    void Close();
    uint32_t GetServerTick() const noexcept;
    // FIXME: This is currently the wrong function to override in System, will be refactored later.
    void Update() override final;
    void Flush();
    void ProcessPending();
    void ProcessPlayerList();
    auto GetPlayerIteratorByID(uint8_t id) const;
    auto GetGroupIteratorByID(uint8_t id) const;
    NetworkPlayer* GetPlayerByID(uint8_t id) const;
    NetworkGroup* GetGroupByID(uint8_t id) const;
    int32_t GetTotalNumPlayers() const noexcept;
    int32_t GetNumVisiblePlayers() const noexcept;
    void SetPassword(u8string_view password);
    uint8_t GetDefaultGroup() const noexcept;
    std::string BeginLog(const std::string& directory, const std::string& midName, const std::string& filenameFormat);
    void AppendLog(std::ostream& fs, std::string_view s);
    void BeginChatLog();
    void AppendChatLog(std::string_view s);
    void CloseChatLog();
    NetworkStats_t GetStats() const;
    json_t GetServerInfoAsJson() const;
    bool ProcessConnection(NetworkConnection& connection);
    void CloseConnection();
    NetworkPlayer* AddPlayer(const std::string& name, const std::string& keyhash);
    void ProcessPacket(NetworkConnection& connection, NetworkPacket& packet);

public: // Server
    NetworkConnection* GetPlayerConnection(uint8_t id) const;
    void KickPlayer(int32_t playerId);
    NetworkGroup* AddGroup();
    void LoadGroups();
    void SetDefaultGroup(uint8_t id);
    void SaveGroups();
    void RemoveGroup(uint8_t id);
    uint8_t GetGroupIDByHash(const std::string& keyhash);
    void BeginServerLog();
    void AppendServerLog(const std::string& s);
    void CloseServerLog();
    void DecayCooldown(NetworkPlayer* player);
    void AddClient(std::unique_ptr<ITcpSocket>&& socket);
    std::string GetMasterServerUrl();
    std::string GenerateAdvertiseKey();
    void SetupDefaultGroups();
    void RemovePlayer(std::unique_ptr<NetworkConnection>& connection);
    void UpdateServer();
    void ServerClientDisconnected(std::unique_ptr<NetworkConnection>& connection);
    bool SaveMap(OpenRCT2::IStream* stream, const std::vector<const ObjectRepositoryItem*>& objects) const;
    std::vector<uint8_t> SaveForNetwork(const std::vector<const ObjectRepositoryItem*>& objects) const;
    std::string MakePlayerNameUnique(const std::string& name);

    // Packet dispatchers.
    void ServerSendAuth(NetworkConnection& connection);
    void ServerSendToken(NetworkConnection& connection);
    void ServerSendMap(NetworkConnection* connection = nullptr);
    void ServerSendChat(const char* text, const std::vector<uint8_t>& playerIds = {});
    void ServerSendGameAction(const GameAction* action);
    void ServerSendTick();
    void ServerSendPlayerInfo(int32_t playerId);
    void ServerSendPlayerList();
    void ServerSendPing();
    void ServerSendPingList();
    void ServerSendSetDisconnectMsg(NetworkConnection& connection, const char* msg);
    void ServerSendGameInfo(NetworkConnection& connection);
    void ServerSendShowError(NetworkConnection& connection, StringId title, StringId message);
    void ServerSendGroupList(NetworkConnection& connection);
    void ServerSendEventPlayerJoined(const char* playerName);
    void ServerSendEventPlayerDisconnected(const char* playerName, const char* reason);
    void ServerSendObjectsList(NetworkConnection& connection, const std::vector<const ObjectRepositoryItem*>& objects) const;
    void ServerSendScripts(NetworkConnection& connection);

    // Handlers
    void ServerHandleRequestGamestate(NetworkConnection& connection, NetworkPacket& packet);
    void ServerHandleHeartbeat(NetworkConnection& connection, NetworkPacket& packet);
    void ServerHandleAuth(NetworkConnection& connection, NetworkPacket& packet);
    void ServerClientJoined(std::string_view name, const std::string& keyhash, NetworkConnection& connection);
    void ServerHandleChat(NetworkConnection& connection, NetworkPacket& packet);
    void ServerHandleGameAction(NetworkConnection& connection, NetworkPacket& packet);
    void ServerHandlePing(NetworkConnection& connection, NetworkPacket& packet);
    void ServerHandleGameInfo(NetworkConnection& connection, NetworkPacket& packet);
    void ServerHandleToken(NetworkConnection& connection, NetworkPacket& packet);
    void ServerHandleMapRequest(NetworkConnection& connection, NetworkPacket& packet);

public: // Client
    void Reconnect();
    int32_t GetMode() const noexcept;
    NetworkAuth GetAuthStatus();
    int32_t GetStatus() const noexcept;
    uint8_t GetPlayerID() const noexcept;
    void ProcessPlayerInfo();
    void ProcessDisconnectedClients();
    static const char* FormatChat(NetworkPlayer* fromplayer, const char* text);
    void SendPacketToClients(const NetworkPacket& packet, bool front = false, bool gameCmd = false) const;
    bool CheckSRAND(uint32_t tick, uint32_t srand0);
    bool CheckDesynchronizaton();
    void RequestStateSnapshot();
    bool IsDesynchronised() const noexcept;
    NetworkServerState_t GetServerState() const noexcept;
    void ServerClientDisconnected();
    bool LoadMap(OpenRCT2::IStream* stream);
    void UpdateClient();

    // Packet dispatchers.
    void Client_Send_RequestGameState(uint32_t tick);
    void Client_Send_TOKEN();
    void Client_Send_AUTH(
        const std::string& name, const std::string& password, const std::string& pubkey, const std::vector<uint8_t>& signature);
    void Client_Send_CHAT(const char* text);
    void Client_Send_GAME_ACTION(const GameAction* action);
    void Client_Send_PING();
    void Client_Send_GAMEINFO();
    void Client_Send_MAPREQUEST(const std::vector<ObjectEntryDescriptor>& objects);
    void Client_Send_HEARTBEAT(NetworkConnection& connection) const;

    // Handlers.
    void Client_Handle_AUTH(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_MAP(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_CHAT(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_GAME_ACTION(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_TICK(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_PLAYERINFO(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_PLAYERLIST(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_PING(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_PINGLIST(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_SETDISCONNECTMSG(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_GAMEINFO(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_SHOWERROR(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_GROUPLIST(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_EVENT(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_TOKEN(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_OBJECTS_LIST(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_SCRIPTS(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_GAMESTATE(NetworkConnection& connection, NetworkPacket& packet);

    std::vector<uint8_t> _challenge;
    std::map<uint32_t, GameAction::Callback_t> _gameActionCallbacks;
    NetworkKey _key;
    NetworkUserManager _userManager;

public: // Public common
    std::string ServerName;
    std::string ServerDescription;
    std::string ServerGreeting;
    std::string ServerProviderName;
    std::string ServerProviderEmail;
    std::string ServerProviderWebsite;
    std::vector<std::unique_ptr<NetworkPlayer>> player_list;
    std::vector<std::unique_ptr<NetworkGroup>> group_list;
    bool IsServerPlayerInvisible = false;

private: // Common Data
    using CommandHandler = void (NetworkBase::*)(NetworkConnection& connection, NetworkPacket& packet);

    std::vector<uint8_t> chunk_buffer;
    std::ofstream _chat_log_fs;
    uint32_t _lastUpdateTime = 0;
    uint32_t _currentDeltaTime = 0;
    int32_t mode = NETWORK_MODE_NONE;
    uint8_t default_group = 0;
    bool _closeLock = false;
    bool _requireClose = false;

private: // Server Data
    std::unordered_map<NetworkCommand, CommandHandler> server_command_handlers;
    std::unique_ptr<ITcpSocket> _listenSocket;
    std::unique_ptr<INetworkServerAdvertiser> _advertiser;
    std::list<std::unique_ptr<NetworkConnection>> client_connection_list;
    std::string _serverLogPath;
    std::string _serverLogFilenameFormat = "%Y%m%d-%H%M%S.txt";
    std::ofstream _server_log_fs;
    uint16_t listening_port = 0;
    bool _playerListInvalidated = false;

private: // Client Data
    struct PlayerListUpdate
    {
        std::vector<NetworkPlayer> players;
    };

    struct ServerTickData_t
    {
        uint32_t srand0;
        uint32_t tick;
        std::string spriteHash;
    };

    std::unordered_map<NetworkCommand, CommandHandler> client_command_handlers;
    std::unique_ptr<NetworkConnection> _serverConnection;
    std::map<uint32_t, PlayerListUpdate> _pendingPlayerLists;
    std::multimap<uint32_t, NetworkPlayer> _pendingPlayerInfo;
    std::map<uint32_t, ServerTickData_t> _serverTickData;
    std::vector<ObjectEntryDescriptor> _missingObjects;
    std::string _host;
    std::string _chatLogPath;
    std::string _chatLogFilenameFormat = "%Y%m%d-%H%M%S.txt";
    std::string _password;
    OpenRCT2::MemoryStream _serverGameState;
    NetworkServerState_t _serverState;
    uint32_t _lastSentHeartbeat = 0;
    uint32_t last_ping_sent_time = 0;
    uint32_t server_connect_time = 0;
    uint32_t _actionId;
    int32_t status = NETWORK_STATUS_NONE;
    uint8_t player_id = 0;
    uint16_t _port = 0;
    SocketStatus _lastConnectStatus = SocketStatus::Closed;
    bool _requireReconnect = false;
    bool _clientMapLoaded = false;
};

#endif // DISABLE_NETWORK
