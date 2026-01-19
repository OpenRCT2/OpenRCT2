#pragma once

#include "../System.hpp"
#include "../actions/GameAction.h"
#include "../scenario/Scenario.h"
#include "NetworkConnection.h"
#include "NetworkGroup.h"
#include "NetworkPlayer.h"
#include "NetworkServerAdvertiser.h"
#include "NetworkTypes.h"
#include "NetworkUser.h"

#include <chrono>
#include <fstream>
#include <list>
#include <memory>

#ifndef DISABLE_NETWORK

namespace OpenRCT2
{
    struct IContext;
    struct ObjectRepositoryItem;
} // namespace OpenRCT2

namespace OpenRCT2::Network
{
    class NetworkBase : public System
    {
    public:
        NetworkBase(IContext& context);

    public: // Uncategorized
        bool BeginServer(uint16_t port, const std::string& address);
        bool BeginClient(const std::string& host, uint16_t port);

    public: // Common
        bool Init();
        void Close();
        uint32_t GetServerTick() const noexcept;
        void Update() final;
        void Tick() final;
        void Flush();
        void PostTick() final;
        void ProcessPlayerList();
        auto GetPlayerIteratorByID(uint8_t id) const;
        auto GetGroupIteratorByID(uint8_t id) const;
        Player* GetPlayerByID(uint8_t id) const;
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
        Stats GetStats() const;
        json_t GetServerInfoAsJson() const;
        bool ProcessConnection(Connection& connection);
        void CloseConnection();
        Player* AddPlayer(const std::string& name, const std::string& keyhash);
        void ProcessPacket(Connection& connection, Packet& packet);
        bool UpdateConnection(Connection& connection);

    public: // Server
        Connection* GetPlayerConnection(uint8_t id) const;
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
        void DecayCooldown(Player* player);
        void AddClient(std::unique_ptr<ITcpSocket>&& socket);
        std::string GetMasterServerUrl();
        std::string GenerateAdvertiseKey();
        void SetupDefaultGroups();
        void RemovePlayer(std::unique_ptr<Connection>& connection);
        void UpdateServer();
        void TickServer();
        void ServerClientDisconnected(std::unique_ptr<Connection>& connection);
        bool SaveMap(IStream* stream, const std::vector<const ObjectRepositoryItem*>& objects) const;
        std::vector<uint8_t> SaveForNetwork(const std::vector<const ObjectRepositoryItem*>& objects) const;
        std::string MakePlayerNameUnique(const std::string& name);

        // Packet dispatchers.
        void ServerSendAuth(Connection& connection);
        void ServerSendToken(Connection& connection);
        void ServerSendMap(Connection* connection = nullptr);
        void ServerSendChat(const char* text, const std::vector<uint8_t>& playerIds = {});
        void ServerSendGameAction(const GameActions::GameAction* action);
        void ServerSendTick();
        void ServerSendPlayerInfo(int32_t playerId);
        void ServerSendPlayerList();
        void ServerSendPing();
        void ServerSendPingList();
        void ServerSendSetDisconnectMsg(Connection& connection, const char* msg);
        void ServerSendGameInfo(Connection& connection);
        void ServerSendShowError(Connection& connection, StringId title, StringId message);
        void ServerSendGroupList(Connection& connection);
        void ServerSendEventPlayerJoined(const char* playerName);
        void ServerSendEventPlayerDisconnected(const char* playerName, const char* reason);
        void ServerSendObjectsList(Connection& connection, const std::vector<const ObjectRepositoryItem*>& objects) const;
        void ServerSendScripts(Connection& connection);

        // Handlers
        void ServerHandleRequestGamestate(Connection& connection, Packet& packet);
        void ServerHandleHeartbeat(Connection& connection, Packet& packet);
        void ServerHandleAuth(Connection& connection, Packet& packet);
        void ServerClientJoined(std::string_view name, const std::string& keyhash, Connection& connection);
        void ServerHandleChat(Connection& connection, Packet& packet);
        void ServerHandleGameAction(Connection& connection, Packet& packet);
        void ServerHandlePing(Connection& connection, Packet& packet);
        void ServerHandleGameInfo(Connection& connection, Packet& packet);
        void ServerHandleToken(Connection& connection, Packet& packet);
        void ServerHandleMapRequest(Connection& connection, Packet& packet);

    public: // Client
        void Reconnect();
        Mode GetMode() const noexcept;
        Auth GetAuthStatus();
        Status GetStatus() const noexcept;
        PlayerId_t GetPlayerID() const noexcept;
        void ProcessPlayerInfo();
        void ProcessDisconnectedClients();
        static const char* FormatChat(Player* fromplayer, const char* text);
        void SendPacketToClients(const Packet& packet, bool front = false, bool gameCmd = false) const;
        bool CheckSRAND(uint32_t tick, uint32_t srand0);
        bool CheckDesynchronizaton();
        void RequestStateSnapshot();
        bool IsDesynchronised() const noexcept;
        ServerState GetServerState() const noexcept;
        void ServerClientDisconnected();
        bool LoadMap(IStream* stream);
        void UpdateClient();
        void TickClient();

        // Packet dispatchers.
        void Client_Send_RequestGameState(uint32_t tick);
        void Client_Send_TOKEN();
        void Client_Send_AUTH(
            const std::string& name, const std::string& password, const std::string& pubkey,
            const std::vector<uint8_t>& signature);
        void Client_Send_CHAT(const char* text);
        void Client_Send_GAME_ACTION(const GameActions::GameAction* action);
        void Client_Send_PING();
        void Client_Send_GAMEINFO();
        void Client_Send_MAPREQUEST(const std::vector<ObjectEntryDescriptor>& objects);
        void Client_Send_HEARTBEAT(Connection& connection) const;

        // Handlers.
        void Client_Handle_AUTH(Connection& connection, Packet& packet);
        void Client_Handle_BEGINMAP([[maybe_unused]] Connection& connection, Packet& packet);
        void Client_Handle_MAP(Connection& connection, Packet& packet);
        void Client_Handle_CHAT(Connection& connection, Packet& packet);
        void Client_Handle_GAME_ACTION(Connection& connection, Packet& packet);
        void Client_Handle_TICK(Connection& connection, Packet& packet);
        void Client_Handle_PLAYERINFO(Connection& connection, Packet& packet);
        void Client_Handle_PLAYERLIST(Connection& connection, Packet& packet);
        void Client_Handle_PING(Connection& connection, Packet& packet);
        void Client_Handle_PINGLIST(Connection& connection, Packet& packet);
        void Client_Handle_SETDISCONNECTMSG(Connection& connection, Packet& packet);
        void Client_Handle_GAMEINFO(Connection& connection, Packet& packet);
        void Client_Handle_SHOWERROR(Connection& connection, Packet& packet);
        void Client_Handle_GROUPLIST(Connection& connection, Packet& packet);
        void Client_Handle_EVENT(Connection& connection, Packet& packet);
        void Client_Handle_TOKEN(Connection& connection, Packet& packet);
        void Client_Handle_OBJECTS_LIST(Connection& connection, Packet& packet);
        void Client_Handle_SCRIPTS_DATA(Connection& connection, Packet& packet);
        void Client_Handle_GAMESTATE(Connection& connection, Packet& packet);
        std::vector<uint8_t> _challenge;
        std::map<uint32_t, GameActions::GameAction::Callback_t> _gameActionCallbacks;
        Key _key;
        UserManager _userManager;

    public: // Public common
        std::string ServerName;
        std::string ServerDescription;
        std::string ServerGreeting;
        std::string ServerProviderName;
        std::string ServerProviderEmail;
        std::string ServerProviderWebsite;
        std::vector<std::unique_ptr<Player>> player_list;
        std::vector<std::unique_ptr<NetworkGroup>> group_list;
        bool IsServerPlayerInvisible = false;

    private: // Common Data
        using CommandHandler = void (NetworkBase::*)(Connection& connection, Packet& packet);

        std::ofstream _chat_log_fs;
        uint32_t _lastUpdateTime = 0;
        uint32_t _currentDeltaTime = 0;
        Mode mode = Mode::none;
        uint8_t default_group = 0;
        bool _closeLock = false;
        bool _requireClose = false;

    private: // Server Data
        std::unordered_map<Command, CommandHandler> server_command_handlers;
        std::unique_ptr<ITcpSocket> _listenSocket;
        std::unique_ptr<INetworkServerAdvertiser> _advertiser;
        std::list<std::unique_ptr<Connection>> client_connection_list;
        std::string _serverLogPath;
        std::string _serverLogFilenameFormat = "%Y%m%d-%H%M%S.txt";
        std::ofstream _server_log_fs;
        uint16_t listening_port = 0;
        bool _playerListInvalidated = false;

    private: // Client Data
        struct PlayerListUpdate
        {
            std::vector<Player> players;
        };

        struct ServerTickData
        {
            uint32_t srand0;
            uint32_t tick;
            std::string spriteHash;
        };

        std::unordered_map<Command, CommandHandler> client_command_handlers;
        std::unique_ptr<Connection> _serverConnection;
        std::map<uint32_t, PlayerListUpdate> _pendingPlayerLists;
        std::multimap<uint32_t, Player> _pendingPlayerInfo;
        std::map<uint32_t, ServerTickData> _serverTickData;
        std::string _host;
        std::string _chatLogPath;
        std::string _chatLogFilenameFormat = "%Y%m%d-%H%M%S.txt";
        std::string _password;
        MemoryStream _serverGameState;
        ServerState _serverState;
        uint32_t _lastSentHeartbeat = 0;
        uint32_t last_ping_sent_time = 0;
        uint32_t server_connect_time = 0;
        uint32_t _actionId;
        Status status = Status::none;
        uint8_t player_id = 0;
        uint16_t _port = 0;
        SocketStatus _lastConnectStatus = SocketStatus::closed;
        bool _requireReconnect = false;
        bool _clientMapLoaded = false;
    };
} // namespace OpenRCT2::Network

#endif // DISABLE_NETWORK
