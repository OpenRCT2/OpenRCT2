#pragma once

#include "../actions/GameAction.h"
#include "NetworkConnection.h"
#include "NetworkGroup.h"
#include "NetworkPlayer.h"
#include "NetworkServerAdvertiser.h"
#include "NetworkTypes.h"
#include "NetworkUser.h"

#include <fstream>

class NetworkBase
{
public:
    NetworkBase();

    void SetEnvironment(const std::shared_ptr<OpenRCT2::IPlatformEnvironment>& env);
    bool Init();
    void Reconnect();
    void Close();
    bool BeginClient(const std::string& host, uint16_t port);
    bool BeginServer(uint16_t port, const std::string& address);
    int32_t GetMode();
    int32_t GetStatus();
    int32_t GetAuthStatus();
    uint32_t GetServerTick();
    uint8_t GetPlayerID();
    void Update();
    void Flush();
    void ProcessPending();
    void ProcessPlayerList();
    void ProcessPlayerInfo();
    void ProcessDisconnectedClients();
    std::vector<std::unique_ptr<NetworkPlayer>>::iterator GetPlayerIteratorByID(uint8_t id);
    NetworkPlayer* GetPlayerByID(uint8_t id);
    NetworkConnection* GetPlayerConnection(uint8_t id);
    std::vector<std::unique_ptr<NetworkGroup>>::iterator GetGroupIteratorByID(uint8_t id);
    NetworkGroup* GetGroupByID(uint8_t id);
    static const char* FormatChat(NetworkPlayer* fromplayer, const char* text);
    void SendPacketToClients(NetworkPacket& packet, bool front = false, bool gameCmd = false);
    bool CheckSRAND(uint32_t tick, uint32_t srand0);
    bool IsDesynchronised();
    bool CheckDesynchronizaton();
    void RequestStateSnapshot();
    NetworkServerState_t GetServerState() const;
    void KickPlayer(int32_t playerId);
    void SetPassword(const char* password);
    void ServerClientDisconnected();
    NetworkGroup* AddGroup();
    void RemoveGroup(uint8_t id);
    uint8_t GetDefaultGroup();
    uint8_t GetGroupIDByHash(const std::string& keyhash);
    void SetDefaultGroup(uint8_t id);
    void SaveGroups();
    void LoadGroups();

    std::string BeginLog(const std::string& directory, const std::string& midName, const std::string& filenameFormat);
    void AppendLog(std::ostream& fs, const std::string& s);

    void BeginChatLog();
    void AppendChatLog(const std::string& s);
    void CloseChatLog();

    void BeginServerLog();
    void AppendServerLog(const std::string& s);
    void CloseServerLog();

    void Client_Send_RequestGameState(uint32_t tick);

    void Client_Send_TOKEN();
    void Client_Send_AUTH(
        const std::string& name, const std::string& password, const std::string& pubkey, const std::vector<uint8_t>& signature);
    void Server_Send_AUTH(NetworkConnection& connection);
    void Server_Send_TOKEN(NetworkConnection& connection);
    void Server_Send_MAP(NetworkConnection* connection = nullptr);
    void Client_Send_CHAT(const char* text);
    void Server_Send_CHAT(const char* text, const std::vector<uint8_t>& playerIds = {});
    void Client_Send_GAME_ACTION(const GameAction* action);
    void Server_Send_GAME_ACTION(const GameAction* action);
    void Server_Send_TICK();
    void Server_Send_PLAYERINFO(int32_t playerId);
    void Server_Send_PLAYERLIST();
    void Client_Send_PING();
    void Server_Send_PING();
    void Server_Send_PINGLIST();
    void Server_Send_SETDISCONNECTMSG(NetworkConnection& connection, const char* msg);
    void Server_Send_GAMEINFO(NetworkConnection& connection);
    void Server_Send_SHOWERROR(NetworkConnection& connection, rct_string_id title, rct_string_id message);
    void Server_Send_GROUPLIST(NetworkConnection& connection);
    void Server_Send_EVENT_PLAYER_JOINED(const char* playerName);
    void Server_Send_EVENT_PLAYER_DISCONNECTED(const char* playerName, const char* reason);
    void Client_Send_GAMEINFO();
    void Client_Send_MAPREQUEST(const std::vector<std::string>& objects);
    void Server_Send_OBJECTS_LIST(NetworkConnection& connection, const std::vector<const ObjectRepositoryItem*>& objects) const;
    void Server_Send_SCRIPTS(NetworkConnection& connection) const;
    void Client_Send_HEARTBEAT(NetworkConnection& connection) const;

    NetworkStats_t GetStats() const;
    json_t* GetServerInfoAsJson() const;

    std::vector<std::unique_ptr<NetworkPlayer>> player_list;
    std::vector<std::unique_ptr<NetworkGroup>> group_list;
    NetworkKey _key;
    std::vector<uint8_t> _challenge;
    std::map<uint32_t, GameAction::Callback_t> _gameActionCallbacks;
    NetworkUserManager _userManager;
    std::string ServerName;
    std::string ServerDescription;
    std::string ServerGreeting;
    std::string ServerProviderName;
    std::string ServerProviderEmail;
    std::string ServerProviderWebsite;

private:
    void DecayCooldown(NetworkPlayer* player);
    void CloseConnection();

    bool ProcessConnection(NetworkConnection& connection);
    void ProcessPacket(NetworkConnection& connection, NetworkPacket& packet);
    void AddClient(std::unique_ptr<ITcpSocket>&& socket);
    void ServerClientDisconnected(std::unique_ptr<NetworkConnection>& connection);

    void RemovePlayer(std::unique_ptr<NetworkConnection>& connection);
    NetworkPlayer* AddPlayer(const std::string& name, const std::string& keyhash);
    std::string MakePlayerNameUnique(const std::string& name);

    std::string GetMasterServerUrl();
    std::string GenerateAdvertiseKey();
    void SetupDefaultGroups();

    bool LoadMap(IStream* stream);
    bool SaveMap(IStream* stream, const std::vector<const ObjectRepositoryItem*>& objects) const;

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

    std::map<uint32_t, ServerTickData_t> _serverTickData;
    std::map<uint32_t, PlayerListUpdate> _pendingPlayerLists;
    std::multimap<uint32_t, NetworkPlayer> _pendingPlayerInfo;
    bool _playerListInvalidated = false;
    int32_t mode = NETWORK_MODE_NONE;
    int32_t status = NETWORK_STATUS_NONE;
    bool _closeLock = false;
    bool _requireClose = false;
    bool _requireReconnect = false;
    bool wsa_initialized = false;
    bool _clientMapLoaded = false;
    std::unique_ptr<ITcpSocket> _listenSocket;
    std::unique_ptr<NetworkConnection> _serverConnection;
    std::unique_ptr<INetworkServerAdvertiser> _advertiser;
    uint16_t listening_port = 0;
    SOCKET_STATUS _lastConnectStatus = SOCKET_STATUS_CLOSED;
    uint32_t last_ping_sent_time = 0;
    uint8_t player_id = 0;
    std::list<std::unique_ptr<NetworkConnection>> client_connection_list;
    std::vector<uint8_t> chunk_buffer;
    std::string _host;
    uint16_t _port = 0;
    std::string _password;
    NetworkServerState_t _serverState;
    MemoryStream _serverGameState;
    uint32_t server_connect_time = 0;
    uint8_t default_group = 0;
    uint32_t _actionId;
    uint32_t _lastUpdateTime = 0;
    uint32_t _currentDeltaTime = 0;
    uint32_t _lastSentHeartbeat = 0;
    std::string _chatLogPath;
    std::string _chatLogFilenameFormat = "%Y%m%d-%H%M%S.txt";
    std::string _serverLogPath;
    std::string _serverLogFilenameFormat = "%Y%m%d-%H%M%S.txt";
    std::shared_ptr<OpenRCT2::IPlatformEnvironment> _env;
    std::vector<std::string> _missingObjects;

    void UpdateServer();
    void UpdateClient();

private:
    using CommandHandler = void (NetworkBase::*)(NetworkConnection& connection, NetworkPacket& packet);

    std::unordered_map<NetworkCommand, CommandHandler> client_command_handlers;
    std::unordered_map<NetworkCommand, CommandHandler> server_command_handlers;

    void Server_Handle_REQUEST_GAMESTATE(NetworkConnection& connection, NetworkPacket& packet);
    void Server_Handle_HEARTBEAT(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_AUTH(NetworkConnection& connection, NetworkPacket& packet);
    void Server_Handle_AUTH(NetworkConnection& connection, NetworkPacket& packet);
    void Server_Client_Joined(const char* name, const std::string& keyhash, NetworkConnection& connection);
    void Client_Handle_MAP(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_CHAT(NetworkConnection& connection, NetworkPacket& packet);
    void Server_Handle_CHAT(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_GAME_ACTION(NetworkConnection& connection, NetworkPacket& packet);
    void Server_Handle_GAME_ACTION(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_TICK(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_PLAYERINFO(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_PLAYERLIST(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_PING(NetworkConnection& connection, NetworkPacket& packet);
    void Server_Handle_PING(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_PINGLIST(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_SETDISCONNECTMSG(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_GAMEINFO(NetworkConnection& connection, NetworkPacket& packet);
    void Server_Handle_GAMEINFO(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_SHOWERROR(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_GROUPLIST(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_EVENT(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_TOKEN(NetworkConnection& connection, NetworkPacket& packet);
    void Server_Handle_TOKEN(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_OBJECTS_LIST(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_SCRIPTS(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_GAMESTATE(NetworkConnection& connection, NetworkPacket& packet);
    void Server_Handle_MAPREQUEST(NetworkConnection& connection, NetworkPacket& packet);

    uint8_t* save_for_network(size_t& out_size, const std::vector<const ObjectRepositoryItem*>& objects) const;

    std::ofstream _chat_log_fs;
    std::ofstream _server_log_fs;
};
