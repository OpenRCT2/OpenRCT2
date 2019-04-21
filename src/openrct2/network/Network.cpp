/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "network.h"

#include "../Context.h"
#include "../Game.h"
#include "../OpenRCT2.h"
#include "../PlatformEnvironment.h"
#include "../actions/LoadOrQuitAction.hpp"
#include "../core/Guard.hpp"
#include "../platform/platform.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../util/SawyerCoding.h"
#include "../world/Location.hpp"

#include <algorithm>
#include <iterator>
#include <stdexcept>

#define ACTION_COOLDOWN_TIME_PLACE_SCENERY 20
#define ACTION_COOLDOWN_TIME_DEMOLISH_RIDE 1000

// This string specifies which version of network stream current build uses.
// It is used for making sure only compatible builds get connected, even within
// single OpenRCT2 version.
#define NETWORK_STREAM_VERSION "19"
#define NETWORK_STREAM_ID OPENRCT2_VERSION "-" NETWORK_STREAM_VERSION

static Peep* _pickup_peep = nullptr;
static int32_t _pickup_peep_old_x = LOCATION_NULL;

#ifndef DISABLE_NETWORK

#    include "../Cheats.h"
#    include "../ParkImporter.h"
#    include "../Version.h"
#    include "../actions/GameAction.h"
#    include "../actions/PauseToggleAction.hpp"
#    include "../config/Config.h"
#    include "../core/Console.hpp"
#    include "../core/FileStream.hpp"
#    include "../core/Json.hpp"
#    include "../core/MemoryStream.h"
#    include "../core/Nullable.hpp"
#    include "../core/Path.hpp"
#    include "../core/String.hpp"
#    include "../interface/Chat.h"
#    include "../interface/Window.h"
#    include "../localisation/Date.h"
#    include "../localisation/Localisation.h"
#    include "../object/ObjectManager.h"
#    include "../object/ObjectRepository.h"
#    include "../rct2/S6Exporter.h"
#    include "../scenario/Scenario.h"
#    include "../util/Util.h"
#    include "../world/Park.h"
#    include "NetworkAction.h"
#    include "NetworkConnection.h"
#    include "NetworkGroup.h"
#    include "NetworkKey.h"
#    include "NetworkPacket.h"
#    include "NetworkPlayer.h"
#    include "NetworkServerAdvertiser.h"
#    include "NetworkUser.h"
#    include "TcpSocket.h"

#    include <algorithm>
#    include <array>
#    include <cerrno>
#    include <cmath>
#    include <fstream>
#    include <functional>
#    include <list>
#    include <map>
#    include <memory>
#    include <set>
#    include <string>
#    include <vector>

#    pragma comment(lib, "Ws2_32.lib")

using namespace OpenRCT2;

enum
{
    SERVER_EVENT_PLAYER_JOINED,
    SERVER_EVENT_PLAYER_DISCONNECTED,
};

enum
{
    NETWORK_TICK_FLAG_CHECKSUMS = 1 << 0,
};

static void network_chat_show_connected_message();
static void network_chat_show_server_greeting();
static void network_get_keys_directory(utf8* buffer, size_t bufferSize);
static void network_get_private_key_path(utf8* buffer, size_t bufferSize, const std::string& playerName);
static void network_get_public_key_path(utf8* buffer, size_t bufferSize, const std::string& playerName, const utf8* hash);

class Network
{
public:
    Network();

    void SetEnvironment(const std::shared_ptr<OpenRCT2::IPlatformEnvironment>& env);
    bool Init();
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
    void ProcessGameCommands();
    void EnqueueGameAction(const GameAction* action);
    std::vector<std::unique_ptr<NetworkPlayer>>::iterator GetPlayerIteratorByID(uint8_t id);
    NetworkPlayer* GetPlayerByID(uint8_t id);
    std::vector<std::unique_ptr<NetworkGroup>>::iterator GetGroupIteratorByID(uint8_t id);
    NetworkGroup* GetGroupByID(uint8_t id);
    static const char* FormatChat(NetworkPlayer* fromplayer, const char* text);
    void SendPacketToClients(NetworkPacket& packet, bool front = false, bool gameCmd = false);
    bool CheckSRAND(uint32_t tick, uint32_t srand0);
    void CheckDesynchronizaton();
    void KickPlayer(int32_t playerId);
    void SetPassword(const char* password);
    void ShutdownClient();
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

    void Client_Send_TOKEN();
    void Client_Send_AUTH(
        const std::string& name, const std::string& password, const std::string& pubkey, const std::vector<uint8_t>& signature);
    void Server_Send_AUTH(NetworkConnection& connection);
    void Server_Send_TOKEN(NetworkConnection& connection);
    void Server_Send_MAP(NetworkConnection* connection = nullptr);
    void Client_Send_CHAT(const char* text);
    void Server_Send_CHAT(const char* text);
    void Client_Send_GAMECMD(
        uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi, uint32_t ebp, uint8_t callback);
    void Server_Send_GAMECMD(
        uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi, uint32_t ebp, uint8_t playerid,
        uint8_t callback);
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
    void Client_Send_OBJECTS(const std::vector<std::string>& objects);
    void Server_Send_OBJECTS(NetworkConnection& connection, const std::vector<const ObjectRepositoryItem*>& objects) const;

    NetworkStats_t GetStats() const;

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
    void RemoveClient(std::unique_ptr<NetworkConnection>& connection);

    NetworkPlayer* AddPlayer(const std::string& name, const std::string& keyhash);
    std::string MakePlayerNameUnique(const std::string& name);

    std::string GetMasterServerUrl();
    std::string GenerateAdvertiseKey();
    void SetupDefaultGroups();

    bool LoadMap(IStream* stream);
    bool SaveMap(IStream* stream, const std::vector<const ObjectRepositoryItem*>& objects) const;

    struct GameCommand
    {
        GameCommand(uint32_t t, uint32_t* args, uint8_t p, uint8_t cb, uint32_t id)
        {
            tick = t;
            eax = args[0];
            ebx = args[1];
            ecx = args[2];
            edx = args[3];
            esi = args[4];
            edi = args[5];
            ebp = args[6];
            playerid = p;
            callback = cb;
            action = nullptr;
            commandIndex = id;
        }

        GameCommand(uint32_t t, std::unique_ptr<GameAction>&& ga, uint32_t id)
        {
            tick = t;
            action = std::move(ga);
            commandIndex = id;
        }

        ~GameCommand()
        {
        }

        uint32_t tick = 0;
        uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0, esi = 0, edi = 0, ebp = 0;
        GameAction::Ptr action;
        uint8_t playerid = 0;
        uint8_t callback = 0;
        uint32_t commandIndex = 0;
        bool operator<(const GameCommand& comp) const
        {
            // First sort by tick
            if (tick < comp.tick)
                return true;
            if (tick > comp.tick)
                return false;

            // If the ticks are equal sort by commandIndex
            return commandIndex < comp.commandIndex;
        }
    };

    struct PlayerListUpdate
    {
        uint32_t tick = 0;
        std::vector<NetworkPlayer> players;

        void reset()
        {
            tick = 0;
            players.clear();
        }
    };

    PlayerListUpdate _pendingPlayerList;

    struct ServerTickData_t
    {
        uint32_t srand0;
        uint32_t tick;
        std::string spriteHash;
    };

    std::map<uint32_t, ServerTickData_t> _serverTickData;
    std::multimap<uint32_t, NetworkPlayer> _pendingPlayerInfo;
    int32_t mode = NETWORK_MODE_NONE;
    int32_t status = NETWORK_STATUS_NONE;
    bool _closeLock = false;
    bool _requireClose = false;
    bool wsa_initialized = false;
    bool _clientMapLoaded = false;
    std::unique_ptr<ITcpSocket> _listenSocket;
    std::unique_ptr<NetworkConnection> _serverConnection;
    std::unique_ptr<INetworkServerAdvertiser> _advertiser;
    uint16_t listening_port = 0;
    SOCKET_STATUS _lastConnectStatus = SOCKET_STATUS_CLOSED;
    uint32_t last_ping_sent_time = 0;
    uint32_t server_tick = 0;
    uint8_t player_id = 0;
    std::list<std::unique_ptr<NetworkConnection>> client_connection_list;
    std::multiset<GameCommand> game_command_queue;
    std::vector<uint8_t> chunk_buffer;
    std::string _password;
    bool _desynchronised = false;
    uint32_t server_connect_time = 0;
    uint8_t default_group = 0;
    uint32_t _commandId;
    uint32_t _actionId;
    uint32_t _lastUpdateTime = 0;
    uint32_t _currentDeltaTime = 0;
    std::string _chatLogPath;
    std::string _chatLogFilenameFormat = "%Y%m%d-%H%M%S.txt";
    std::string _serverLogPath;
    std::string _serverLogFilenameFormat = "%Y%m%d-%H%M%S.txt";
    std::shared_ptr<OpenRCT2::IPlatformEnvironment> _env;

    void UpdateServer();
    void UpdateClient();

private:
    std::vector<void (Network::*)(NetworkConnection& connection, NetworkPacket& packet)> client_command_handlers;
    std::vector<void (Network::*)(NetworkConnection& connection, NetworkPacket& packet)> server_command_handlers;
    void Client_Handle_AUTH(NetworkConnection& connection, NetworkPacket& packet);
    void Server_Handle_AUTH(NetworkConnection& connection, NetworkPacket& packet);
    void Server_Client_Joined(const char* name, const std::string& keyhash, NetworkConnection& connection);
    void Client_Handle_MAP(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_CHAT(NetworkConnection& connection, NetworkPacket& packet);
    void Server_Handle_CHAT(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_GAMECMD(NetworkConnection& connection, NetworkPacket& packet);
    void Server_Handle_GAMECMD(NetworkConnection& connection, NetworkPacket& packet);
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
    void Client_Handle_OBJECTS(NetworkConnection& connection, NetworkPacket& packet);
    void Server_Handle_OBJECTS(NetworkConnection& connection, NetworkPacket& packet);

    uint8_t* save_for_network(size_t& out_size, const std::vector<const ObjectRepositoryItem*>& objects) const;

    std::ofstream _chat_log_fs;
    std::ofstream _server_log_fs;
};

static Network gNetwork;

Network::Network()
{
    wsa_initialized = false;
    mode = NETWORK_MODE_NONE;
    status = NETWORK_STATUS_NONE;
    last_ping_sent_time = 0;
    _commandId = 0;
    _actionId = 0;
    client_command_handlers.resize(NETWORK_COMMAND_MAX, nullptr);
    client_command_handlers[NETWORK_COMMAND_AUTH] = &Network::Client_Handle_AUTH;
    client_command_handlers[NETWORK_COMMAND_MAP] = &Network::Client_Handle_MAP;
    client_command_handlers[NETWORK_COMMAND_CHAT] = &Network::Client_Handle_CHAT;
    client_command_handlers[NETWORK_COMMAND_GAMECMD] = &Network::Client_Handle_GAMECMD;
    client_command_handlers[NETWORK_COMMAND_GAME_ACTION] = &Network::Client_Handle_GAME_ACTION;
    client_command_handlers[NETWORK_COMMAND_TICK] = &Network::Client_Handle_TICK;
    client_command_handlers[NETWORK_COMMAND_PLAYERLIST] = &Network::Client_Handle_PLAYERLIST;
    client_command_handlers[NETWORK_COMMAND_PLAYERINFO] = &Network::Client_Handle_PLAYERINFO;
    client_command_handlers[NETWORK_COMMAND_PING] = &Network::Client_Handle_PING;
    client_command_handlers[NETWORK_COMMAND_PINGLIST] = &Network::Client_Handle_PINGLIST;
    client_command_handlers[NETWORK_COMMAND_SETDISCONNECTMSG] = &Network::Client_Handle_SETDISCONNECTMSG;
    client_command_handlers[NETWORK_COMMAND_SHOWERROR] = &Network::Client_Handle_SHOWERROR;
    client_command_handlers[NETWORK_COMMAND_GROUPLIST] = &Network::Client_Handle_GROUPLIST;
    client_command_handlers[NETWORK_COMMAND_EVENT] = &Network::Client_Handle_EVENT;
    client_command_handlers[NETWORK_COMMAND_GAMEINFO] = &Network::Client_Handle_GAMEINFO;
    client_command_handlers[NETWORK_COMMAND_TOKEN] = &Network::Client_Handle_TOKEN;
    client_command_handlers[NETWORK_COMMAND_OBJECTS] = &Network::Client_Handle_OBJECTS;
    server_command_handlers.resize(NETWORK_COMMAND_MAX, nullptr);
    server_command_handlers[NETWORK_COMMAND_AUTH] = &Network::Server_Handle_AUTH;
    server_command_handlers[NETWORK_COMMAND_CHAT] = &Network::Server_Handle_CHAT;
    server_command_handlers[NETWORK_COMMAND_GAMECMD] = &Network::Server_Handle_GAMECMD;
    server_command_handlers[NETWORK_COMMAND_GAME_ACTION] = &Network::Server_Handle_GAME_ACTION;
    server_command_handlers[NETWORK_COMMAND_PING] = &Network::Server_Handle_PING;
    server_command_handlers[NETWORK_COMMAND_GAMEINFO] = &Network::Server_Handle_GAMEINFO;
    server_command_handlers[NETWORK_COMMAND_TOKEN] = &Network::Server_Handle_TOKEN;
    server_command_handlers[NETWORK_COMMAND_OBJECTS] = &Network::Server_Handle_OBJECTS;

    _chat_log_fs << std::unitbuf;
    _server_log_fs << std::unitbuf;
}

void Network::SetEnvironment(const std::shared_ptr<IPlatformEnvironment>& env)
{
    _env = env;
}

bool Network::Init()
{
    if (!InitialiseWSA())
    {
        return false;
    }

    status = NETWORK_STATUS_READY;

    ServerName = std::string();
    ServerDescription = std::string();
    ServerGreeting = std::string();
    ServerProviderName = std::string();
    ServerProviderEmail = std::string();
    ServerProviderWebsite = std::string();
    return true;
}

void Network::Close()
{
    if (status != NETWORK_STATUS_NONE)
    {
        // HACK Because Close() is closed all over the place, it sometimes gets called inside an Update
        //      call. This then causes disposed data to be accessed. Therefore, save closing until the
        //      end of the update loop.
        if (_closeLock)
        {
            _requireClose = true;
            return;
        }

        CloseChatLog();
        CloseServerLog();
        CloseConnection();

        client_connection_list.clear();
        game_command_queue.clear();
        player_list.clear();
        group_list.clear();
        _pendingPlayerList.reset();
        _pendingPlayerInfo.clear();

        gfx_invalidate_screen();

        _requireClose = false;
    }
}

void Network::DecayCooldown(NetworkPlayer* player)
{
    if (player == nullptr)
        return; // No valid connection yet.

    for (auto it = std::begin(player->CooldownTime); it != std::end(player->CooldownTime);)
    {
        it->second -= _currentDeltaTime;
        if (it->second <= 0)
            it = player->CooldownTime.erase(it);
        else
            it++;
    }
}

void Network::CloseConnection()
{
    if (mode == NETWORK_MODE_CLIENT)
    {
        _serverConnection.reset();
    }
    else if (mode == NETWORK_MODE_SERVER)
    {
        _listenSocket.reset();
        _advertiser.reset();
    }

    mode = NETWORK_MODE_NONE;
    status = NETWORK_STATUS_NONE;
    _lastConnectStatus = SOCKET_STATUS_CLOSED;

    DisposeWSA();
}

bool Network::BeginClient(const std::string& host, uint16_t port)
{
    if (GetMode() != NETWORK_MODE_NONE)
    {
        return false;
    }

    Close();
    if (!Init())
        return false;

    mode = NETWORK_MODE_CLIENT;

    log_info("Connecting to %s:%u\n", host.c_str(), port);

    _serverConnection = std::make_unique<NetworkConnection>();
    _serverConnection->Socket = CreateTcpSocket();
    _serverConnection->Socket->ConnectAsync(host, port);
    status = NETWORK_STATUS_CONNECTING;
    _lastConnectStatus = SOCKET_STATUS_CLOSED;
    _clientMapLoaded = false;
    _serverTickData.clear();

    BeginChatLog();
    BeginServerLog();

    utf8 keyPath[MAX_PATH];
    network_get_private_key_path(keyPath, sizeof(keyPath), gConfigNetwork.player_name);
    if (!platform_file_exists(keyPath))
    {
        Console::WriteLine("Generating key... This may take a while");
        Console::WriteLine("Need to collect enough entropy from the system");
        _key.Generate();
        Console::WriteLine("Key generated, saving private bits as %s", keyPath);

        utf8 keysDirectory[MAX_PATH];
        network_get_keys_directory(keysDirectory, sizeof(keysDirectory));
        if (!platform_ensure_directory_exists(keysDirectory))
        {
            log_error("Unable to create directory %s.", keysDirectory);
            return false;
        }

        try
        {
            auto fs = FileStream(keyPath, FILE_MODE_WRITE);
            _key.SavePrivate(&fs);
        }
        catch (const std::exception&)
        {
            log_error("Unable to save private key at %s.", keyPath);
            return false;
        }

        const std::string hash = _key.PublicKeyHash();
        const utf8* publicKeyHash = hash.c_str();
        network_get_public_key_path(keyPath, sizeof(keyPath), gConfigNetwork.player_name, publicKeyHash);
        Console::WriteLine("Key generated, saving public bits as %s", keyPath);

        try
        {
            auto fs = FileStream(keyPath, FILE_MODE_WRITE);
            _key.SavePublic(&fs);
        }
        catch (const std::exception&)
        {
            log_error("Unable to save public key at %s.", keyPath);
            return false;
        }
    }
    else
    {
        // LoadPrivate returns validity of loaded key
        bool ok = false;
        try
        {
            log_verbose("Loading key from %s", keyPath);
            auto fs = FileStream(keyPath, FILE_MODE_OPEN);
            ok = _key.LoadPrivate(&fs);
        }
        catch (const std::exception&)
        {
            log_error("Unable to read private key from %s.", keyPath);
            return false;
        }

        // Don't store private key in memory when it's not in use.
        _key.Unload();
        return ok;
    }

    return true;
}

bool Network::BeginServer(uint16_t port, const std::string& address)
{
    Close();
    if (!Init())
        return false;

    mode = NETWORK_MODE_SERVER;

    _userManager.Load();

    log_verbose("Begin listening for clients");

    _listenSocket = CreateTcpSocket();
    try
    {
        _listenSocket->Listen(address, port);
    }
    catch (const std::exception& ex)
    {
        Console::Error::WriteLine(ex.what());
        Close();
        return false;
    }

    ServerName = gConfigNetwork.server_name;
    ServerDescription = gConfigNetwork.server_description;
    ServerGreeting = gConfigNetwork.server_greeting;
    ServerProviderName = gConfigNetwork.provider_name;
    ServerProviderEmail = gConfigNetwork.provider_email;
    ServerProviderWebsite = gConfigNetwork.provider_website;

    cheats_reset();
    LoadGroups();
    BeginChatLog();
    BeginServerLog();

    NetworkPlayer* player = AddPlayer(gConfigNetwork.player_name, "");
    player->Flags |= NETWORK_PLAYER_FLAG_ISSERVER;
    player->Group = 0;
    player_id = player->Id;

    if (network_get_mode() == NETWORK_MODE_SERVER)
    {
        // Add SERVER to users.json and save.
        NetworkUser* networkUser = _userManager.GetOrAddUser(player->KeyHash);
        networkUser->GroupId = player->Group;
        networkUser->Name = player->Name;
        _userManager.Save();
    }

    printf("Ready for clients...\n");
    network_chat_show_connected_message();
    network_chat_show_server_greeting();

    status = NETWORK_STATUS_CONNECTED;
    listening_port = port;
    if (gConfigNetwork.advertise)
    {
        _advertiser = CreateServerAdvertiser(listening_port);
    }

    if (gConfigNetwork.pause_server_if_no_clients)
    {
        auto pauseToggleAction = PauseToggleAction();
        GameActions::Execute(&pauseToggleAction);
    }

    return true;
}

int32_t Network::GetMode()
{
    return mode;
}

int32_t Network::GetStatus()
{
    return status;
}

int32_t Network::GetAuthStatus()
{
    if (GetMode() == NETWORK_MODE_CLIENT)
    {
        return _serverConnection->AuthStatus;
    }
    else if (GetMode() == NETWORK_MODE_SERVER)
    {
        return NETWORK_AUTH_OK;
    }
    return NETWORK_AUTH_NONE;
}

uint32_t Network::GetServerTick()
{
    return server_tick;
}

uint8_t Network::GetPlayerID()
{
    return player_id;
}

void Network::Update()
{
    _closeLock = true;

    // Update is not necessarily called per game tick, maintain our own delta time
    uint32_t ticks = platform_get_ticks();
    _currentDeltaTime = std::max<uint32_t>(ticks - _lastUpdateTime, 1);
    _lastUpdateTime = ticks;

    switch (GetMode())
    {
        case NETWORK_MODE_SERVER:
            UpdateServer();
            break;
        case NETWORK_MODE_CLIENT:
            UpdateClient();
            break;
    }

    // If the Close() was called during the update, close it for real
    _closeLock = false;
    if (_requireClose)
    {
        Close();
    }
}

void Network::Flush()
{
    if (GetMode() == NETWORK_MODE_CLIENT)
    {
        _serverConnection->SendQueuedPackets();
    }
    else
    {
        for (auto& it : client_connection_list)
        {
            it->SendQueuedPackets();
        }
    }
}

void Network::UpdateServer()
{
    auto it = client_connection_list.begin();
    while (it != client_connection_list.end())
    {
        if (!ProcessConnection(*(*it)))
        {
            RemoveClient((*it));
            it = client_connection_list.begin();
        }
        else
        {
            DecayCooldown((*it)->Player);
            it++;
        }
    }

    uint32_t ticks = platform_get_ticks();
    if (ticks > last_ping_sent_time + 3000)
    {
        Server_Send_PING();
        Server_Send_PINGLIST();
    }

    if (_advertiser != nullptr)
    {
        _advertiser->Update();
    }

    std::unique_ptr<ITcpSocket> tcpSocket = _listenSocket->Accept();
    if (tcpSocket != nullptr)
    {
        AddClient(std::move(tcpSocket));
    }
}

void Network::UpdateClient()
{
    assert(_serverConnection != nullptr);

    switch (status)
    {
        case NETWORK_STATUS_CONNECTING:
        {
            switch (_serverConnection->Socket->GetStatus())
            {
                case SOCKET_STATUS_RESOLVING:
                {
                    if (_lastConnectStatus != SOCKET_STATUS_RESOLVING)
                    {
                        _lastConnectStatus = SOCKET_STATUS_RESOLVING;
                        char str_resolving[256];
                        format_string(str_resolving, 256, STR_MULTIPLAYER_RESOLVING, nullptr);

                        auto intent = Intent(WC_NETWORK_STATUS);
                        intent.putExtra(INTENT_EXTRA_MESSAGE, std::string{ str_resolving });
                        intent.putExtra(INTENT_EXTRA_CALLBACK, []() -> void { gNetwork.Close(); });
                        context_open_intent(&intent);
                    }
                    break;
                }
                case SOCKET_STATUS_CONNECTING:
                {
                    if (_lastConnectStatus != SOCKET_STATUS_CONNECTING)
                    {
                        _lastConnectStatus = SOCKET_STATUS_CONNECTING;
                        char str_connecting[256];
                        format_string(str_connecting, 256, STR_MULTIPLAYER_CONNECTING, nullptr);

                        auto intent = Intent(WC_NETWORK_STATUS);
                        intent.putExtra(INTENT_EXTRA_MESSAGE, std::string{ str_connecting });
                        intent.putExtra(INTENT_EXTRA_CALLBACK, []() -> void { gNetwork.Close(); });
                        context_open_intent(&intent);

                        server_connect_time = platform_get_ticks();
                    }
                    break;
                }
                case SOCKET_STATUS_CONNECTED:
                {
                    status = NETWORK_STATUS_CONNECTED;
                    _serverConnection->ResetLastPacketTime();
                    Client_Send_TOKEN();
                    char str_authenticating[256];
                    format_string(str_authenticating, 256, STR_MULTIPLAYER_AUTHENTICATING, nullptr);

                    auto intent = Intent(WC_NETWORK_STATUS);
                    intent.putExtra(INTENT_EXTRA_MESSAGE, std::string{ str_authenticating });
                    intent.putExtra(INTENT_EXTRA_CALLBACK, []() -> void { gNetwork.Close(); });
                    context_open_intent(&intent);
                    break;
                }
                default:
                {
                    const char* error = _serverConnection->Socket->GetError();
                    if (error != nullptr)
                    {
                        Console::Error::WriteLine(error);
                    }

                    Close();
                    context_force_close_window_by_class(WC_NETWORK_STATUS);
                    context_show_error(STR_UNABLE_TO_CONNECT_TO_SERVER, STR_NONE);
                    break;
                }
            }
            break;
        }
        case NETWORK_STATUS_CONNECTED:
        {
            if (!ProcessConnection(*_serverConnection))
            {
                // Do not show disconnect message window when password window closed/canceled
                if (_serverConnection->AuthStatus == NETWORK_AUTH_REQUIREPASSWORD)
                {
                    context_force_close_window_by_class(WC_NETWORK_STATUS);
                }
                else
                {
                    char str_disconnected[256];

                    if (_serverConnection->GetLastDisconnectReason())
                    {
                        const char* disconnect_reason = _serverConnection->GetLastDisconnectReason();
                        format_string(str_disconnected, 256, STR_MULTIPLAYER_DISCONNECTED_WITH_REASON, &disconnect_reason);
                    }
                    else
                    {
                        format_string(str_disconnected, 256, STR_MULTIPLAYER_DISCONNECTED_NO_REASON, nullptr);
                    }

                    auto intent = Intent(WC_NETWORK_STATUS);
                    intent.putExtra(INTENT_EXTRA_MESSAGE, std::string{ str_disconnected });
                    context_open_intent(&intent);
                }
                window_close_by_class(WC_MULTIPLAYER);
                Close();
            }
            break;
        }
    }
}

std::vector<std::unique_ptr<NetworkPlayer>>::iterator Network::GetPlayerIteratorByID(uint8_t id)
{
    auto it = std::find_if(player_list.begin(), player_list.end(), [&id](std::unique_ptr<NetworkPlayer> const& player) {
        return player->Id == id;
    });
    if (it != player_list.end())
    {
        return it;
    }
    return player_list.end();
}

NetworkPlayer* Network::GetPlayerByID(uint8_t id)
{
    auto it = GetPlayerIteratorByID(id);
    if (it != player_list.end())
    {
        return it->get();
    }
    return nullptr;
}

std::vector<std::unique_ptr<NetworkGroup>>::iterator Network::GetGroupIteratorByID(uint8_t id)
{
    auto it = std::find_if(
        group_list.begin(), group_list.end(), [&id](std::unique_ptr<NetworkGroup> const& group) { return group->Id == id; });
    if (it != group_list.end())
    {
        return it;
    }
    return group_list.end();
}

NetworkGroup* Network::GetGroupByID(uint8_t id)
{
    auto it = GetGroupIteratorByID(id);
    if (it != group_list.end())
    {
        return it->get();
    }
    return nullptr;
}

const char* Network::FormatChat(NetworkPlayer* fromplayer, const char* text)
{
    static char formatted[1024];
    char* lineCh = formatted;
    formatted[0] = 0;
    if (fromplayer)
    {
        lineCh = utf8_write_codepoint(lineCh, FORMAT_OUTLINE);
        lineCh = utf8_write_codepoint(lineCh, FORMAT_BABYBLUE);
        safe_strcpy(lineCh, (const char*)fromplayer->Name.c_str(), sizeof(formatted) - (lineCh - formatted));
        safe_strcat(lineCh, ": ", sizeof(formatted) - (lineCh - formatted));
        lineCh = strchr(lineCh, '\0');
    }
    lineCh = utf8_write_codepoint(lineCh, FORMAT_OUTLINE);
    lineCh = utf8_write_codepoint(lineCh, FORMAT_WHITE);
    char* ptrtext = lineCh;
    safe_strcpy(lineCh, text, 800);
    utf8_remove_format_codes(ptrtext, true);
    return formatted;
}

void Network::SendPacketToClients(NetworkPacket& packet, bool front, bool gameCmd)
{
    for (auto& client_connection : client_connection_list)
    {
        if (gameCmd)
        {
            // If marked as game command we can not send the packet to connections that are not fully connected.
            // Sending the packet would cause the client to store a command that is behind the tick where he starts,
            // which would be essentially never executed. The clients do not require commands before the server has not sent the
            // map data.
            if (client_connection->Player == nullptr)
            {
                continue;
            }
        }
        client_connection->QueuePacket(NetworkPacket::Duplicate(packet), front);
    }
}

bool Network::CheckSRAND(uint32_t tick, uint32_t srand0)
{
    // We have to wait for the map to be loaded first, ticks may match current loaded map.
    if (_clientMapLoaded == false)
        return true;

    auto itTickData = _serverTickData.find(tick);
    if (itTickData == std::end(_serverTickData))
        return true;

    const ServerTickData_t storedTick = itTickData->second;
    _serverTickData.erase(itTickData);

    if (storedTick.srand0 != srand0)
        return false;

    if (storedTick.spriteHash.empty() == false)
    {
        rct_sprite_checksum checksum = sprite_checksum();
        std::string clientSpriteHash = checksum.ToString();
        if (clientSpriteHash != storedTick.spriteHash)
        {
            return false;
        }
    }

    return true;
}

void Network::CheckDesynchronizaton()
{
    // Check synchronisation
    if (GetMode() == NETWORK_MODE_CLIENT && !_desynchronised && !CheckSRAND(gCurrentTicks, scenario_rand_state().s0))
    {
        _desynchronised = true;

        char str_desync[256];
        format_string(str_desync, 256, STR_MULTIPLAYER_DESYNC, nullptr);

        auto intent = Intent(WC_NETWORK_STATUS);
        intent.putExtra(INTENT_EXTRA_MESSAGE, std::string{ str_desync });
        context_open_intent(&intent);

        if (!gConfigNetwork.stay_connected)
        {
            Close();
        }
    }
}

void Network::KickPlayer(int32_t playerId)
{
    for (auto& client_connection : client_connection_list)
    {
        if (client_connection->Player->Id == playerId)
        {
            // Disconnect the client gracefully
            client_connection->SetLastDisconnectReason(STR_MULTIPLAYER_KICKED);
            char str_disconnect_msg[256];
            format_string(str_disconnect_msg, 256, STR_MULTIPLAYER_KICKED_REASON, nullptr);
            Server_Send_SETDISCONNECTMSG(*client_connection, str_disconnect_msg);
            client_connection->Socket->Disconnect();
            break;
        }
    }
}

void Network::SetPassword(const char* password)
{
    _password = password == nullptr ? "" : password;
}

void Network::ShutdownClient()
{
    if (GetMode() == NETWORK_MODE_CLIENT)
    {
        _serverConnection->Socket->Disconnect();
    }
}

std::string Network::GenerateAdvertiseKey()
{
    // Generate a string of 16 random hex characters (64-integer key as a hex formatted string)
    static char hexChars[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
    char key[17];
    for (int32_t i = 0; i < 16; i++)
    {
        int32_t hexCharIndex = util_rand() % std::size(hexChars);
        key[i] = hexChars[hexCharIndex];
    }
    key[std::size(key) - 1] = 0;

    return key;
}

std::string Network::GetMasterServerUrl()
{
    if (gConfigNetwork.master_server_url.empty())
    {
        return OPENRCT2_MASTER_SERVER_URL;
    }
    else
    {
        return gConfigNetwork.master_server_url;
    }
}

NetworkGroup* Network::AddGroup()
{
    NetworkGroup* addedgroup = nullptr;
    int32_t newid = -1;
    // Find first unused group id
    for (int32_t id = 0; id < 255; id++)
    {
        if (std::find_if(
                group_list.begin(), group_list.end(),
                [&id](std::unique_ptr<NetworkGroup> const& group) { return group->Id == id; })
            == group_list.end())
        {
            newid = id;
            break;
        }
    }
    if (newid != -1)
    {
        auto group = std::make_unique<NetworkGroup>();
        group->Id = newid;
        group->SetName("Group #" + std::to_string(newid));
        addedgroup = group.get();
        group_list.push_back(std::move(group));
    }
    return addedgroup;
}

void Network::RemoveGroup(uint8_t id)
{
    auto group = GetGroupIteratorByID(id);
    if (group != group_list.end())
    {
        group_list.erase(group);
    }

    if (GetMode() == NETWORK_MODE_SERVER)
    {
        _userManager.UnsetUsersOfGroup(id);
        _userManager.Save();
    }
}

uint8_t Network::GetGroupIDByHash(const std::string& keyhash)
{
    const NetworkUser* networkUser = _userManager.GetUserByHash(keyhash);

    uint8_t groupId = GetDefaultGroup();
    if (networkUser != nullptr && networkUser->GroupId.HasValue())
    {
        const uint8_t assignedGroup = networkUser->GroupId.GetValue();
        if (GetGroupByID(assignedGroup) != nullptr)
        {
            groupId = assignedGroup;
        }
        else
        {
            log_warning(
                "User %s is assigned to non-existent group %u. Assigning to default group (%u)", keyhash.c_str(), assignedGroup,
                groupId);
        }
    }
    return groupId;
}

uint8_t Network::GetDefaultGroup()
{
    return default_group;
}

void Network::SetDefaultGroup(uint8_t id)
{
    if (GetGroupByID(id))
    {
        default_group = id;
    }
}

void Network::SaveGroups()
{
    if (GetMode() == NETWORK_MODE_SERVER)
    {
        utf8 path[MAX_PATH];

        platform_get_user_directory(path, nullptr, sizeof(path));
        safe_strcat_path(path, "groups.json", sizeof(path));

        json_t* jsonGroupsCfg = json_object();
        json_t* jsonGroups = json_array();
        for (auto& group : group_list)
        {
            json_array_append_new(jsonGroups, group->ToJson());
        }
        json_object_set_new(jsonGroupsCfg, "default_group", json_integer(default_group));
        json_object_set_new(jsonGroupsCfg, "groups", jsonGroups);
        try
        {
            Json::WriteToFile(path, jsonGroupsCfg, JSON_INDENT(4) | JSON_PRESERVE_ORDER);
        }
        catch (const std::exception& ex)
        {
            log_error("Unable to save %s: %s", path, ex.what());
        }

        json_decref(jsonGroupsCfg);
    }
}

void Network::SetupDefaultGroups()
{
    // Admin group
    auto admin = std::make_unique<NetworkGroup>();
    admin->SetName("Admin");
    admin->ActionsAllowed.fill(0xFF);
    admin->Id = 0;
    group_list.push_back(std::move(admin));

    // Spectator group
    auto spectator = std::make_unique<NetworkGroup>();
    spectator->SetName("Spectator");
    spectator->ToggleActionPermission(NETWORK_PERMISSION_CHAT);
    spectator->Id = 1;
    group_list.push_back(std::move(spectator));

    // User group
    auto user = std::make_unique<NetworkGroup>();
    user->SetName("User");
    user->ActionsAllowed.fill(0xFF);
    user->ToggleActionPermission(NETWORK_PERMISSION_KICK_PLAYER);
    user->ToggleActionPermission(NETWORK_PERMISSION_MODIFY_GROUPS);
    user->ToggleActionPermission(NETWORK_PERMISSION_SET_PLAYER_GROUP);
    user->ToggleActionPermission(NETWORK_PERMISSION_CHEAT);
    user->ToggleActionPermission(NETWORK_PERMISSION_PASSWORDLESS_LOGIN);
    user->ToggleActionPermission(NETWORK_PERMISSION_MODIFY_TILE);
    user->ToggleActionPermission(NETWORK_PERMISSION_EDIT_SCENARIO_OPTIONS);
    user->Id = 2;
    group_list.push_back(std::move(user));

    SetDefaultGroup(1);
}

void Network::LoadGroups()
{
    group_list.clear();

    utf8 path[MAX_PATH];

    platform_get_user_directory(path, nullptr, sizeof(path));
    safe_strcat_path(path, "groups.json", sizeof(path));

    json_t* json = nullptr;
    if (platform_file_exists(path))
    {
        try
        {
            json = Json::ReadFromFile(path);
        }
        catch (const std::exception& e)
        {
            log_error("Failed to read %s as JSON. Setting default groups. %s", path, e.what());
        }
    }

    if (json == nullptr)
    {
        SetupDefaultGroups();
    }
    else
    {
        json_t* json_groups = json_object_get(json, "groups");
        size_t groupCount = (size_t)json_array_size(json_groups);
        for (size_t i = 0; i < groupCount; i++)
        {
            json_t* jsonGroup = json_array_get(json_groups, i);

            auto newgroup = std::make_unique<NetworkGroup>(NetworkGroup::FromJson(jsonGroup));
            group_list.push_back(std::move(newgroup));
        }
        json_t* jsonDefaultGroup = json_object_get(json, "default_group");
        default_group = (uint8_t)json_integer_value(jsonDefaultGroup);
        if (GetGroupByID(default_group) == nullptr)
        {
            default_group = 0;
        }
        json_decref(json);
    }

    // Host group should always contain all permissions.
    group_list.at(0)->ActionsAllowed.fill(0xFF);
}

std::string Network::BeginLog(const std::string& directory, const std::string& midName, const std::string& filenameFormat)
{
    utf8 filename[256];
    time_t timer;
    time(&timer);
    auto tmInfo = localtime(&timer);
    if (strftime(filename, sizeof(filename), filenameFormat.c_str(), tmInfo) == 0)
    {
        throw std::runtime_error("strftime failed");
    }

    platform_ensure_directory_exists(Path::Combine(directory, midName).c_str());
    return Path::Combine(directory, midName, filename);
}

void Network::AppendLog(std::ostream& fs, const std::string& s)
{
    if (fs.fail())
    {
        log_error("bad ostream failed to append log");
        return;
    }
    try
    {
        utf8 buffer[256];
        time_t timer;
        time(&timer);
        auto tmInfo = localtime(&timer);
        if (strftime(buffer, sizeof(buffer), "[%Y/%m/%d %H:%M:%S] ", tmInfo) != 0)
        {
            String::Append(buffer, sizeof(buffer), s.c_str());
            utf8_remove_formatting(buffer, false);
            String::Append(buffer, sizeof(buffer), PLATFORM_NEWLINE);

            fs.write(buffer, strlen(buffer));
        }
    }
    catch (const std::exception& ex)
    {
        log_error("%s", ex.what());
    }
}

void Network::BeginChatLog()
{
    auto directory = _env->GetDirectoryPath(DIRBASE::USER, DIRID::LOG_CHAT);
    _chatLogPath = BeginLog(directory, "", _chatLogFilenameFormat);

#    if defined(_WIN32) && !defined(__MINGW32__)
    auto pathW = std::unique_ptr<wchar_t>(utf8_to_widechar(_chatLogPath.c_str()));
    _chat_log_fs.open(pathW.get(), std::ios::out | std::ios::app);
#    else
    _chat_log_fs.open(_chatLogPath, std::ios::out | std::ios::app);
#    endif
}

void Network::AppendChatLog(const std::string& s)
{
    if (gConfigNetwork.log_chat && _chat_log_fs.is_open())
    {
        AppendLog(_chat_log_fs, s);
    }
}

void Network::CloseChatLog()
{
    _chat_log_fs.close();
}

void Network::BeginServerLog()
{
    auto directory = _env->GetDirectoryPath(DIRBASE::USER, DIRID::LOG_SERVER);
    _serverLogPath = BeginLog(directory, ServerName, _serverLogFilenameFormat);

#    if defined(_WIN32) && !defined(__MINGW32__)
    auto pathW = std::unique_ptr<wchar_t>(utf8_to_widechar(_serverLogPath.c_str()));
    _server_log_fs.open(pathW.get(), std::ios::out | std::ios::app);
#    else
    _server_log_fs.open(_serverLogPath, std::ios::out | std::ios::app);
#    endif

    // Log server start event
    utf8 logMessage[256];
    if (GetMode() == NETWORK_MODE_CLIENT)
    {
        format_string(logMessage, sizeof(logMessage), STR_LOG_CLIENT_STARTED, nullptr);
    }
    else if (GetMode() == NETWORK_MODE_SERVER)
    {
        format_string(logMessage, sizeof(logMessage), STR_LOG_SERVER_STARTED, nullptr);
    }
    AppendServerLog(logMessage);
}

void Network::AppendServerLog(const std::string& s)
{
    if (gConfigNetwork.log_server_actions && _server_log_fs.is_open())
    {
        AppendLog(_server_log_fs, s);
    }
}

void Network::CloseServerLog()
{
    // Log server stopped event
    char logMessage[256];
    if (GetMode() == NETWORK_MODE_CLIENT)
    {
        format_string(logMessage, sizeof(logMessage), STR_LOG_CLIENT_STOPPED, nullptr);
    }
    else if (GetMode() == NETWORK_MODE_SERVER)
    {
        format_string(logMessage, sizeof(logMessage), STR_LOG_SERVER_STOPPED, nullptr);
    }
    AppendServerLog(logMessage);
    _server_log_fs.close();
}

void Network::Client_Send_TOKEN()
{
    log_verbose("requesting token");
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32_t)NETWORK_COMMAND_TOKEN;
    _serverConnection->AuthStatus = NETWORK_AUTH_REQUESTED;
    _serverConnection->QueuePacket(std::move(packet));
}

void Network::Client_Send_AUTH(
    const std::string& name, const std::string& password, const std::string& pubkey, const std::vector<uint8_t>& signature)
{
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32_t)NETWORK_COMMAND_AUTH;
    packet->WriteString(network_get_version().c_str());
    packet->WriteString(name.c_str());
    packet->WriteString(password.c_str());
    packet->WriteString(pubkey.c_str());
    assert(signature.size() <= (size_t)UINT32_MAX);
    *packet << (uint32_t)signature.size();
    packet->Write(signature.data(), signature.size());
    _serverConnection->AuthStatus = NETWORK_AUTH_REQUESTED;
    _serverConnection->QueuePacket(std::move(packet));
}

void Network::Client_Send_OBJECTS(const std::vector<std::string>& objects)
{
    log_verbose("client requests %u objects", uint32_t(objects.size()));
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32_t)NETWORK_COMMAND_OBJECTS << (uint32_t)objects.size();
    for (const auto& object : objects)
    {
        log_verbose("client requests object %s", object.c_str());
        packet->Write((const uint8_t*)object.c_str(), 8);
    }
    _serverConnection->QueuePacket(std::move(packet));
}

void Network::Server_Send_TOKEN(NetworkConnection& connection)
{
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32_t)NETWORK_COMMAND_TOKEN << (uint32_t)connection.Challenge.size();
    packet->Write(connection.Challenge.data(), connection.Challenge.size());
    connection.QueuePacket(std::move(packet));
}

void Network::Server_Send_OBJECTS(NetworkConnection& connection, const std::vector<const ObjectRepositoryItem*>& objects) const
{
    log_verbose("Server sends objects list with %u items", objects.size());
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32_t)NETWORK_COMMAND_OBJECTS << (uint32_t)objects.size();
    for (auto object : objects)
    {
        log_verbose("Object %.8s (checksum %x)", object->ObjectEntry.name, object->ObjectEntry.checksum);
        packet->Write((const uint8_t*)object->ObjectEntry.name, 8);
        *packet << object->ObjectEntry.checksum << object->ObjectEntry.flags;
    }
    connection.QueuePacket(std::move(packet));
}

NetworkStats_t Network::GetStats() const
{
    NetworkStats_t stats = {};
    if (mode == NETWORK_MODE_CLIENT)
    {
        stats = _serverConnection->Stats;
    }
    else
    {
        for (auto& connection : client_connection_list)
        {
            for (size_t n = 0; n < NETWORK_STATISTICS_GROUP_MAX; n++)
            {
                stats.bytesReceived[n] += connection->Stats.bytesReceived[n];
                stats.bytesSent[n] += connection->Stats.bytesSent[n];
            }
        }
    }
    return stats;
}

void Network::Server_Send_AUTH(NetworkConnection& connection)
{
    uint8_t new_playerid = 0;
    if (connection.Player)
    {
        new_playerid = connection.Player->Id;
    }
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32_t)NETWORK_COMMAND_AUTH << (uint32_t)connection.AuthStatus << new_playerid;
    if (connection.AuthStatus == NETWORK_AUTH_BADVERSION)
    {
        packet->WriteString(network_get_version().c_str());
    }
    connection.QueuePacket(std::move(packet));
    if (connection.AuthStatus != NETWORK_AUTH_OK && connection.AuthStatus != NETWORK_AUTH_REQUIREPASSWORD)
    {
        connection.Socket->Disconnect();
    }
}

void Network::Server_Send_MAP(NetworkConnection* connection)
{
    std::vector<const ObjectRepositoryItem*> objects;
    if (connection)
    {
        objects = connection->RequestedObjects;
    }
    else
    {
        // This will send all custom objects to connected clients
        // TODO: fix it so custom objects negotiation is performed even in this case.
        auto context = GetContext();
        auto& objManager = context->GetObjectManager();
        objects = objManager.GetPackableObjects();
    }

    size_t out_size;
    uint8_t* header = save_for_network(out_size, objects);
    if (header == nullptr)
    {
        if (connection)
        {
            connection->SetLastDisconnectReason(STR_MULTIPLAYER_CONNECTION_CLOSED);
            connection->Socket->Disconnect();
        }
        return;
    }
    size_t chunksize = 65000;
    for (size_t i = 0; i < out_size; i += chunksize)
    {
        size_t datasize = std::min(chunksize, out_size - i);
        std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
        *packet << (uint32_t)NETWORK_COMMAND_MAP << (uint32_t)out_size << (uint32_t)i;
        packet->Write(&header[i], datasize);
        if (connection)
        {
            connection->QueuePacket(std::move(packet));
        }
        else
        {
            SendPacketToClients(*packet);
        }
    }
    free(header);
}

uint8_t* Network::save_for_network(size_t& out_size, const std::vector<const ObjectRepositoryItem*>& objects) const
{
    uint8_t* header = nullptr;
    out_size = 0;
    bool RLEState = gUseRLE;
    gUseRLE = false;

    auto ms = MemoryStream();
    if (!SaveMap(&ms, objects))
    {
        log_warning("Failed to export map.");
        return nullptr;
    }
    gUseRLE = RLEState;

    const void* data = ms.GetData();
    int32_t size = ms.GetLength();

    uint8_t* compressed = util_zlib_deflate((const uint8_t*)data, size, &out_size);
    if (compressed != nullptr)
    {
        header = (uint8_t*)_strdup("open2_sv6_zlib");
        size_t header_len = strlen((char*)header) + 1; // account for null terminator
        header = (uint8_t*)realloc(header, header_len + out_size);
        if (header == nullptr)
        {
            log_error("Failed to allocate %u bytes.", header_len + out_size);
        }
        else
        {
            std::memcpy(&header[header_len], compressed, out_size);
            out_size += header_len;
            log_verbose("Sending map of size %u bytes, compressed to %u bytes", size, out_size);
        }
        free(compressed);
    }
    else
    {
        log_warning("Failed to compress the data, falling back to non-compressed sv6.");
        header = (uint8_t*)malloc(size);
        if (header == nullptr)
        {
            log_error("Failed to allocate %u bytes.", size);
        }
        else
        {
            out_size = size;
            std::memcpy(header, data, size);
        }
    }
    return header;
}

void Network::Client_Send_CHAT(const char* text)
{
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32_t)NETWORK_COMMAND_CHAT;
    packet->WriteString(text);
    _serverConnection->QueuePacket(std::move(packet));
}

void Network::Server_Send_CHAT(const char* text)
{
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32_t)NETWORK_COMMAND_CHAT;
    packet->WriteString(text);
    SendPacketToClients(*packet);
}

void Network::Client_Send_GAMECMD(
    uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi, uint32_t ebp, uint8_t callback)
{
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32_t)NETWORK_COMMAND_GAMECMD << gCurrentTicks << eax << (ebx | GAME_COMMAND_FLAG_NETWORKED) << ecx << edx
            << esi << edi << ebp << callback;

    _serverConnection->QueuePacket(std::move(packet));
}

void Network::Server_Send_GAMECMD(
    uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi, uint32_t ebp, uint8_t playerid,
    uint8_t callback)
{
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32_t)NETWORK_COMMAND_GAMECMD << gCurrentTicks << eax << (ebx | GAME_COMMAND_FLAG_NETWORKED) << ecx << edx
            << esi << edi << ebp << playerid << callback;
    SendPacketToClients(*packet, false, true);
}

void Network::Client_Send_GAME_ACTION(const GameAction* action)
{
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());

    uint32_t networkId = 0;
    networkId = ++_actionId;

    // I know its ugly, want basic functionality for now.
    const_cast<GameAction*>(action)->SetNetworkId(networkId);
    if (action->GetCallback())
    {
        _gameActionCallbacks.insert(std::make_pair(networkId, action->GetCallback()));
    }

    DataSerialiser stream(true);
    action->Serialise(stream);

    *packet << (uint32_t)NETWORK_COMMAND_GAME_ACTION << gCurrentTicks << action->GetType() << stream;
    _serverConnection->QueuePacket(std::move(packet));
}

void Network::Server_Send_GAME_ACTION(const GameAction* action)
{
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());

    DataSerialiser stream(true);
    action->Serialise(stream);

    *packet << (uint32_t)NETWORK_COMMAND_GAME_ACTION << gCurrentTicks << action->GetType() << stream;

    SendPacketToClients(*packet);
}

void Network::Server_Send_TICK()
{
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32_t)NETWORK_COMMAND_TICK << gCurrentTicks << scenario_rand_state().s0;
    uint32_t flags = 0;
    // Simple counter which limits how often a sprite checksum gets sent.
    // This can get somewhat expensive, so we don't want to push it every tick in release,
    // but debug version can check more often.
    static int32_t checksum_counter = 0;
    checksum_counter++;
    if (checksum_counter >= 100)
    {
        checksum_counter = 0;
        flags |= NETWORK_TICK_FLAG_CHECKSUMS;
    }
    // Send flags always, so we can understand packet structure on the other end,
    // and allow for some expansion.
    *packet << flags;
    if (flags & NETWORK_TICK_FLAG_CHECKSUMS)
    {
        rct_sprite_checksum checksum = sprite_checksum();
        packet->WriteString(checksum.ToString().c_str());
    }

    SendPacketToClients(*packet);
}

void Network::Server_Send_PLAYERINFO(int32_t playerId)
{
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32_t)NETWORK_COMMAND_PLAYERINFO << gCurrentTicks;

    auto* player = GetPlayerByID(playerId);
    if (player == nullptr)
        return;

    player->Write(*packet);
    SendPacketToClients(*packet);
}

void Network::Server_Send_PLAYERLIST()
{
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32_t)NETWORK_COMMAND_PLAYERLIST << gCurrentTicks << (uint8_t)player_list.size();
    for (auto& player : player_list)
    {
        player->Write(*packet);
    }
    SendPacketToClients(*packet);
}

void Network::Client_Send_PING()
{
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32_t)NETWORK_COMMAND_PING;
    _serverConnection->QueuePacket(std::move(packet));
}

void Network::Server_Send_PING()
{
    last_ping_sent_time = platform_get_ticks();
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32_t)NETWORK_COMMAND_PING;
    for (auto& client_connection : client_connection_list)
    {
        client_connection->PingTime = platform_get_ticks();
    }
    SendPacketToClients(*packet, true);
}

void Network::Server_Send_PINGLIST()
{
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32_t)NETWORK_COMMAND_PINGLIST << (uint8_t)player_list.size();
    for (auto& player : player_list)
    {
        *packet << player->Id << player->Ping;
    }
    SendPacketToClients(*packet);
}

void Network::Server_Send_SETDISCONNECTMSG(NetworkConnection& connection, const char* msg)
{
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32_t)NETWORK_COMMAND_SETDISCONNECTMSG;
    packet->WriteString(msg);
    connection.QueuePacket(std::move(packet));
}

void Network::Server_Send_GAMEINFO(NetworkConnection& connection)
{
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32_t)NETWORK_COMMAND_GAMEINFO;
#    ifndef DISABLE_HTTP
    json_t* obj = json_object();
    json_object_set_new(obj, "name", json_string(gConfigNetwork.server_name.c_str()));
    json_object_set_new(obj, "requiresPassword", json_boolean(_password.size() > 0));
    json_object_set_new(obj, "version", json_string(network_get_version().c_str()));
    json_object_set_new(obj, "players", json_integer(player_list.size()));
    json_object_set_new(obj, "maxPlayers", json_integer(gConfigNetwork.maxplayers));
    json_object_set_new(obj, "description", json_string(gConfigNetwork.server_description.c_str()));
    json_object_set_new(obj, "greeting", json_string(gConfigNetwork.server_greeting.c_str()));
    json_object_set_new(obj, "dedicated", json_boolean(gOpenRCT2Headless));

    // Provider details
    json_t* jsonProvider = json_object();
    json_object_set_new(jsonProvider, "name", json_string(gConfigNetwork.provider_name.c_str()));
    json_object_set_new(jsonProvider, "email", json_string(gConfigNetwork.provider_email.c_str()));
    json_object_set_new(jsonProvider, "website", json_string(gConfigNetwork.provider_website.c_str()));
    json_object_set_new(obj, "provider", jsonProvider);

    packet->WriteString(json_dumps(obj, 0));
    json_decref(obj);
#    endif
    connection.QueuePacket(std::move(packet));
}

void Network::Server_Send_SHOWERROR(NetworkConnection& connection, rct_string_id title, rct_string_id message)
{
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32_t)NETWORK_COMMAND_SHOWERROR << title << message;
    connection.QueuePacket(std::move(packet));
}

void Network::Server_Send_GROUPLIST(NetworkConnection& connection)
{
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32_t)NETWORK_COMMAND_GROUPLIST << (uint8_t)group_list.size() << default_group;
    for (auto& group : group_list)
    {
        group->Write(*packet);
    }
    connection.QueuePacket(std::move(packet));
}

void Network::Server_Send_EVENT_PLAYER_JOINED(const char* playerName)
{
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32_t)NETWORK_COMMAND_EVENT;
    *packet << (uint16_t)SERVER_EVENT_PLAYER_JOINED;
    packet->WriteString(playerName);
    SendPacketToClients(*packet);
}

void Network::Server_Send_EVENT_PLAYER_DISCONNECTED(const char* playerName, const char* reason)
{
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32_t)NETWORK_COMMAND_EVENT;
    *packet << (uint16_t)SERVER_EVENT_PLAYER_DISCONNECTED;
    packet->WriteString(playerName);
    packet->WriteString(reason);
    SendPacketToClients(*packet);
}

bool Network::ProcessConnection(NetworkConnection& connection)
{
    int32_t packetStatus;
    do
    {
        packetStatus = connection.ReadPacket();
        switch (packetStatus)
        {
            case NETWORK_READPACKET_DISCONNECTED:
                // closed connection or network error
                if (!connection.GetLastDisconnectReason())
                {
                    connection.SetLastDisconnectReason(STR_MULTIPLAYER_CONNECTION_CLOSED);
                }
                return false;
            case NETWORK_READPACKET_SUCCESS:
                // done reading in packet
                ProcessPacket(connection, connection.InboundPacket);
                if (connection.Socket == nullptr)
                {
                    return false;
                }
                break;
            case NETWORK_READPACKET_MORE_DATA:
                // more data required to be read
                break;
            case NETWORK_READPACKET_NO_DATA:
                // could not read anything from socket
                break;
        }
    } while (packetStatus == NETWORK_READPACKET_MORE_DATA || packetStatus == NETWORK_READPACKET_SUCCESS);
    connection.SendQueuedPackets();
    if (!connection.ReceivedPacketRecently())
    {
        if (!connection.GetLastDisconnectReason())
        {
            connection.SetLastDisconnectReason(STR_MULTIPLAYER_NO_DATA);
        }
        return false;
    }
    return true;
}

void Network::ProcessPacket(NetworkConnection& connection, NetworkPacket& packet)
{
    uint32_t command;
    packet >> command;
    if (command < NETWORK_COMMAND_MAX)
    {
        switch (gNetwork.GetMode())
        {
            case NETWORK_MODE_SERVER:
                if (server_command_handlers[command])
                {
                    if (connection.AuthStatus == NETWORK_AUTH_OK || !packet.CommandRequiresAuth())
                    {
                        (this->*server_command_handlers[command])(connection, packet);
                    }
                }
                break;
            case NETWORK_MODE_CLIENT:
                if (client_command_handlers[command])
                {
                    (this->*client_command_handlers[command])(connection, packet);
                }
                break;
        }
    }
    packet.Clear();
}

void Network::ProcessPending()
{
    ProcessGameCommands();
    ProcessPlayerList();
    ProcessPlayerInfo();
}

void Network::ProcessPlayerList()
{
    if (_pendingPlayerList.tick == 0 || _pendingPlayerList.tick < gCurrentTicks)
    {
        return;
    }

    // List of active players found in the list.
    std::vector<uint8_t> activePlayerIds;

    for (auto&& pendingPlayer : _pendingPlayerList.players)
    {
        activePlayerIds.push_back(pendingPlayer.Id);

        auto* player = GetPlayerByID(pendingPlayer.Id);
        if (player == nullptr)
        {
            // Add new player.
            player = AddPlayer("", "");
            if (player)
            {
                *player = pendingPlayer;
                if (player->Flags & NETWORK_PLAYER_FLAG_ISSERVER)
                {
                    _serverConnection->Player = player;
                }
            }
        }
        else
        {
            // Update.
            *player = pendingPlayer;
        }
    }

    // Remove any players that are not in newly received list
    auto it = player_list.begin();
    while (it != player_list.end())
    {
        if (std::find(activePlayerIds.begin(), activePlayerIds.end(), (*it)->Id) == activePlayerIds.end())
        {
            it = player_list.erase(it);
        }
        else
        {
            it++;
        }
    }

    _pendingPlayerList.reset();
}

void Network::ProcessPlayerInfo()
{
    auto range = _pendingPlayerInfo.equal_range(gCurrentTicks);
    for (auto it = range.first; it != range.second; it++)
    {
        auto* player = GetPlayerByID(it->second.Id);
        if (player != nullptr)
        {
            const NetworkPlayer& networkedInfo = it->second;
            player->Flags = networkedInfo.Flags;
            player->Group = networkedInfo.Group;
            player->LastAction = networkedInfo.LastAction;
            player->LastActionCoord = networkedInfo.LastActionCoord;
            player->MoneySpent = networkedInfo.MoneySpent;
            player->CommandsRan = networkedInfo.CommandsRan;
        }
    }
    _pendingPlayerInfo.erase(gCurrentTicks);
}

void Network::ProcessGameCommands()
{
    while (game_command_queue.begin() != game_command_queue.end())
    {
        // run all the game commands at the current tick
        const GameCommand& gc = (*game_command_queue.begin());

        if (mode == NETWORK_MODE_CLIENT)
        {
            if (game_command_queue.begin()->tick < gCurrentTicks)
            {
                // Having old command from a tick where we have not been active yet or malicious server,
                // the command is useless so lets not keep it.
                log_warning(
                    "Discarding game command from tick behind current tick, CMD: %08X, CMD Tick: %08X, Current Tick: %08X\n",
                    gc.esi, gc.tick, gCurrentTicks);

                game_command_queue.erase(game_command_queue.begin());

                // At this point we should not return, would add the possibility to skip commands this tick.
                continue;
            }

            // exit the game command processing loop to still have a chance at finding desync.
            if (game_command_queue.begin()->tick != gCurrentTicks)
                break;
        }

        if (gc.action != nullptr)
        {
            // Remove ghost scenery so it doesn't interfere with incoming network command
            switch (gc.action->GetType())
            {
                case GAME_COMMAND_PLACE_WALL:
                case GAME_COMMAND_PLACE_LARGE_SCENERY:
                case GAME_COMMAND_PLACE_BANNER:
                case GAME_COMMAND_PLACE_SCENERY:
                    scenery_remove_ghost_tool_placement();
                    break;
            }

            GameAction* action = gc.action.get();
            action->SetFlags(action->GetFlags() | GAME_COMMAND_FLAG_NETWORKED);

            Guard::Assert(action != nullptr);

            GameActionResult::Ptr result = GameActions::Execute(action);
            if (result->Error == GA_ERROR::OK)
            {
                Server_Send_GAME_ACTION(action);
                Server_Send_PLAYERINFO(action->GetPlayer());
            }
        }
        else
        {
            if (GetPlayerID() == gc.playerid)
            {
                game_command_callback = game_command_callback_get_callback(gc.callback);
            }

            game_command_playerid = gc.playerid;

            int32_t command = gc.esi;
            int32_t flags = gc.ebx;
            if (mode == NETWORK_MODE_SERVER)
                flags |= GAME_COMMAND_FLAG_NETWORKED;

            money32 cost = game_do_command(gc.eax, flags, gc.ecx, gc.edx, gc.esi, gc.edi, gc.ebp);

            if (cost != MONEY32_UNDEFINED)
            {
                NetworkPlayer* player = GetPlayerByID(gc.playerid);
                if (!player)
                    return;

                player->LastAction = NetworkActions::FindCommand(command);
                player->LastActionTime = platform_get_ticks();
                player->AddMoneySpent(cost);

                if (mode == NETWORK_MODE_SERVER)
                {
                    // Note these are currently not reached as both commands are ported to GameActions
                    if (command == GAME_COMMAND_PLACE_SCENERY)
                    {
                        player->LastPlaceSceneryTime = player->LastActionTime;
                    }
                    else if (command == GAME_COMMAND_DEMOLISH_RIDE)
                    {
                        player->LastDemolishRideTime = player->LastActionTime;
                    }

                    Server_Send_GAMECMD(gc.eax, gc.ebx, gc.ecx, gc.edx, gc.esi, gc.edi, gc.ebp, gc.playerid, gc.callback);
                    Server_Send_PLAYERINFO(gc.playerid);
                }
            }
        }
        game_command_queue.erase(game_command_queue.begin());
    }
}

void Network::EnqueueGameAction(const GameAction* action)
{
    std::unique_ptr<GameAction> ga = GameActions::Clone(action);
    if (ga->GetPlayer() == -1 && GetMode() != NETWORK_MODE_NONE)
    {
        // Server can directly invoke actions and will have no player id assigned
        // as that normally happens when receiving them over network.
        ga->SetPlayer(network_get_current_player_id());
    }

    game_command_queue.emplace(gCurrentTicks, std::move(ga), _commandId++);
}

void Network::AddClient(std::unique_ptr<ITcpSocket>&& socket)
{
    if (gConfigNetwork.pause_server_if_no_clients && game_is_paused())
    {
        auto pauseToggleAction = PauseToggleAction();
        GameActions::Execute(&pauseToggleAction);
    }

    // Log connection info.
    char addr[128];
    snprintf(addr, sizeof(addr), "Client joined from %s", socket->GetHostName());
    AppendServerLog(addr);

    // Store connection
    auto connection = std::make_unique<NetworkConnection>();
    connection->Socket = std::move(socket);

    client_connection_list.push_back(std::move(connection));
}

void Network::RemoveClient(std::unique_ptr<NetworkConnection>& connection)
{
    NetworkPlayer* connection_player = connection->Player;
    if (connection_player)
    {
        char text[256];
        const char* has_disconnected_args[2] = {
            connection_player->Name.c_str(),
            connection->GetLastDisconnectReason(),
        };
        if (has_disconnected_args[1])
        {
            format_string(text, 256, STR_MULTIPLAYER_PLAYER_HAS_DISCONNECTED_WITH_REASON, has_disconnected_args);
        }
        else
        {
            format_string(text, 256, STR_MULTIPLAYER_PLAYER_HAS_DISCONNECTED_NO_REASON, &(has_disconnected_args[0]));
        }

        chat_history_add(text);
        Peep* pickup_peep = network_get_pickup_peep(connection_player->Id);
        if (pickup_peep)
        {
            game_command_playerid = connection_player->Id;
            game_do_command(
                pickup_peep->sprite_index, GAME_COMMAND_FLAG_APPLY, 1, 0,
                pickup_peep->type == PEEP_TYPE_GUEST ? GAME_COMMAND_PICKUP_GUEST : GAME_COMMAND_PICKUP_STAFF,
                network_get_pickup_peep_old_x(connection_player->Id), 0);
        }
        gNetwork.Server_Send_EVENT_PLAYER_DISCONNECTED(
            (char*)connection_player->Name.c_str(), connection->GetLastDisconnectReason());

        // Log player disconnected event
        AppendServerLog(text);
    }
    player_list.erase(
        std::remove_if(
            player_list.begin(), player_list.end(),
            [connection_player](std::unique_ptr<NetworkPlayer>& player) { return player.get() == connection_player; }),
        player_list.end());
    client_connection_list.remove(connection);
    if (gConfigNetwork.pause_server_if_no_clients && game_is_not_paused() && client_connection_list.empty())
    {
        auto pauseToggleAction = PauseToggleAction();
        GameActions::Execute(&pauseToggleAction);
    }
    Server_Send_PLAYERLIST();
}

NetworkPlayer* Network::AddPlayer(const std::string& name, const std::string& keyhash)
{
    NetworkPlayer* addedplayer = nullptr;
    int32_t newid = -1;
    if (GetMode() == NETWORK_MODE_SERVER)
    {
        // Find first unused player id
        for (int32_t id = 0; id < 255; id++)
        {
            if (std::find_if(
                    player_list.begin(), player_list.end(),
                    [&id](std::unique_ptr<NetworkPlayer> const& player) { return player->Id == id; })
                == player_list.end())
            {
                newid = id;
                break;
            }
        }
    }
    else
    {
        newid = 0;
    }
    if (newid != -1)
    {
        std::unique_ptr<NetworkPlayer> player;
        if (GetMode() == NETWORK_MODE_SERVER)
        {
            // Load keys host may have added manually
            _userManager.Load();

            // Check if the key is registered
            const NetworkUser* networkUser = _userManager.GetUserByHash(keyhash);

            player = std::make_unique<NetworkPlayer>();
            player->Id = newid;
            player->KeyHash = keyhash;
            if (networkUser == nullptr)
            {
                player->Group = GetDefaultGroup();
                if (name.empty() == false)
                {
                    player->SetName(MakePlayerNameUnique(String::Trim(name)));
                }
            }
            else
            {
                player->Group = networkUser->GroupId.GetValueOrDefault(GetDefaultGroup());
                player->SetName(networkUser->Name);
            }
        }
        else
        {
            player = std::make_unique<NetworkPlayer>();
            player->Id = newid;
            player->Group = GetDefaultGroup();
            player->SetName(String::Trim(std::string(name)));
        }

        addedplayer = player.get();
        player_list.push_back(std::move(player));
    }
    return addedplayer;
}

std::string Network::MakePlayerNameUnique(const std::string& name)
{
    // Note: Player names are case-insensitive

    std::string new_name = name.substr(0, 31);
    int32_t counter = 1;
    bool unique;
    do
    {
        unique = true;

        // Check if there is already a player with this name in the server
        for (const auto& player : player_list)
        {
            if (String::Equals(player->Name.c_str(), new_name.c_str(), true))
            {
                unique = false;
                break;
            }
        }

        if (unique)
        {
            // Check if there is already a registered player with this name
            if (_userManager.GetUserByName(new_name) != nullptr)
            {
                unique = false;
            }
        }

        if (!unique)
        {
            // Increment name counter
            counter++;
            new_name = name.substr(0, 31) + " #" + std::to_string(counter);
        }
    } while (!unique);
    return new_name;
}

void Network::Client_Handle_TOKEN(NetworkConnection& connection, NetworkPacket& packet)
{
    utf8 keyPath[MAX_PATH];
    network_get_private_key_path(keyPath, sizeof(keyPath), gConfigNetwork.player_name);
    if (!platform_file_exists(keyPath))
    {
        log_error("Key file (%s) was not found. Restart client to re-generate it.", keyPath);
        return;
    }

    try
    {
        auto fs = FileStream(keyPath, FILE_MODE_OPEN);
        if (!_key.LoadPrivate(&fs))
        {
            throw std::runtime_error("Failed to load private key.");
        }
    }
    catch (const std::exception&)
    {
        log_error("Failed to load key %s", keyPath);
        connection.SetLastDisconnectReason(STR_MULTIPLAYER_VERIFICATION_FAILURE);
        connection.Socket->Disconnect();
        return;
    }

    uint32_t challenge_size;
    packet >> challenge_size;
    const char* challenge = (const char*)packet.Read(challenge_size);

    std::vector<uint8_t> signature;
    const std::string pubkey = _key.PublicKeyString();
    _challenge.resize(challenge_size);
    std::memcpy(_challenge.data(), challenge, challenge_size);
    bool ok = _key.Sign(_challenge.data(), _challenge.size(), signature);
    if (!ok)
    {
        log_error("Failed to sign server's challenge.");
        connection.SetLastDisconnectReason(STR_MULTIPLAYER_VERIFICATION_FAILURE);
        connection.Socket->Disconnect();
        return;
    }
    // Don't keep private key in memory. There's no need and it may get leaked
    // when process dump gets collected at some point in future.
    _key.Unload();

    const char* password = String::IsNullOrEmpty(gCustomPassword) ? "" : gCustomPassword;
    Client_Send_AUTH(gConfigNetwork.player_name.c_str(), password, pubkey.c_str(), signature);
}

void Network::Client_Handle_AUTH(NetworkConnection& connection, NetworkPacket& packet)
{
    uint32_t auth_status;
    packet >> auth_status >> (uint8_t&)player_id;
    connection.AuthStatus = (NETWORK_AUTH)auth_status;
    switch (connection.AuthStatus)
    {
        case NETWORK_AUTH_OK:
            Client_Send_GAMEINFO();
            break;
        case NETWORK_AUTH_BADNAME:
            connection.SetLastDisconnectReason(STR_MULTIPLAYER_BAD_PLAYER_NAME);
            connection.Socket->Disconnect();
            break;
        case NETWORK_AUTH_BADVERSION:
        {
            const char* version = packet.ReadString();
            connection.SetLastDisconnectReason(STR_MULTIPLAYER_INCORRECT_SOFTWARE_VERSION, &version);
            connection.Socket->Disconnect();
            break;
        }
        case NETWORK_AUTH_BADPASSWORD:
            connection.SetLastDisconnectReason(STR_MULTIPLAYER_BAD_PASSWORD);
            connection.Socket->Disconnect();
            break;
        case NETWORK_AUTH_VERIFICATIONFAILURE:
            connection.SetLastDisconnectReason(STR_MULTIPLAYER_VERIFICATION_FAILURE);
            connection.Socket->Disconnect();
            break;
        case NETWORK_AUTH_FULL:
            connection.SetLastDisconnectReason(STR_MULTIPLAYER_SERVER_FULL);
            connection.Socket->Disconnect();
            break;
        case NETWORK_AUTH_REQUIREPASSWORD:
            context_open_window_view(WV_NETWORK_PASSWORD);
            break;
        case NETWORK_AUTH_UNKNOWN_KEY_DISALLOWED:
            connection.SetLastDisconnectReason(STR_MULTIPLAYER_UNKNOWN_KEY_DISALLOWED);
            connection.Socket->Disconnect();
            break;
        default:
            connection.SetLastDisconnectReason(STR_MULTIPLAYER_INCORRECT_SOFTWARE_VERSION);
            connection.Socket->Disconnect();
            break;
    }
}

void Network::Server_Client_Joined(const char* name, const std::string& keyhash, NetworkConnection& connection)
{
    NetworkPlayer* player = AddPlayer(name, keyhash);
    connection.Player = player;
    if (player)
    {
        char text[256];
        const char* player_name = (const char*)player->Name.c_str();
        format_string(text, 256, STR_MULTIPLAYER_PLAYER_HAS_JOINED_THE_GAME, &player_name);
        chat_history_add(text);

        auto context = GetContext();
        auto& objManager = context->GetObjectManager();
        auto objects = objManager.GetPackableObjects();
        Server_Send_OBJECTS(connection, objects);

        // Log player joining event
        std::string playerNameHash = player->Name + " (" + keyhash + ")";
        player_name = (const char*)playerNameHash.c_str();
        format_string(text, 256, STR_MULTIPLAYER_PLAYER_HAS_JOINED_THE_GAME, &player_name);
        AppendServerLog(text);

        Server_Send_PLAYERLIST();
    }
}

void Network::Server_Handle_TOKEN(NetworkConnection& connection, [[maybe_unused]] NetworkPacket& packet)
{
    uint8_t token_size = 10 + (rand() & 0x7f);
    connection.Challenge.resize(token_size);
    for (int32_t i = 0; i < token_size; i++)
    {
        connection.Challenge[i] = (uint8_t)(rand() & 0xff);
    }
    Server_Send_TOKEN(connection);
}

void Network::Client_Handle_OBJECTS(NetworkConnection& connection, NetworkPacket& packet)
{
    auto& repo = GetContext()->GetObjectRepository();
    uint32_t size;
    packet >> size;
    log_verbose("client received object list, it has %u entries", size);
    if (size > OBJECT_ENTRY_COUNT)
    {
        connection.SetLastDisconnectReason(STR_MULTIPLAYER_SERVER_INVALID_REQUEST);
        connection.Socket->Disconnect();
        log_warning("Server sent invalid amount of objects");
        return;
    }
    std::vector<std::string> requested_objects;
    for (uint32_t i = 0; i < size; i++)
    {
        const char* name = (const char*)packet.Read(8);
        // Required, as packet has no null terminators.
        std::string s(name, name + 8);
        uint32_t checksum, flags;
        packet >> checksum >> flags;
        const ObjectRepositoryItem* ori = repo.FindObject(s.c_str());
        // This could potentially request the object if checksums don't match, but since client
        // won't replace its version with server-provided one, we don't do that.
        if (ori == nullptr)
        {
            log_verbose("Requesting object %s with checksum %x from server", s.c_str(), checksum);
            requested_objects.push_back(s);
        }
        else if (ori->ObjectEntry.checksum != checksum || ori->ObjectEntry.flags != flags)
        {
            log_warning(
                "Object %s has different checksum/flags (%x/%x) than server (%x/%x).", s.c_str(), ori->ObjectEntry.checksum,
                ori->ObjectEntry.flags, checksum, flags);
        }
    }
    Client_Send_OBJECTS(requested_objects);
}

void Network::Server_Handle_OBJECTS(NetworkConnection& connection, NetworkPacket& packet)
{
    uint32_t size;
    packet >> size;
    if (size > OBJECT_ENTRY_COUNT)
    {
        connection.SetLastDisconnectReason(STR_MULTIPLAYER_CLIENT_INVALID_REQUEST);
        connection.Socket->Disconnect();
        std::string playerName = "(unknown)";
        if (connection.Player)
        {
            playerName = connection.Player->Name;
        }
        std::string text = std::string("Player ") + playerName + std::string(" requested invalid amount of objects");
        AppendServerLog(text);
        log_warning(text.c_str());
        return;
    }
    log_verbose("Client requested %u objects", size);
    auto& repo = GetContext()->GetObjectRepository();
    for (uint32_t i = 0; i < size; i++)
    {
        const char* name = (const char*)packet.Read(8);
        // This is required, as packet does not have null terminator
        std::string s(name, name + 8);
        log_verbose("Client requested object %s", s.c_str());
        const ObjectRepositoryItem* item = repo.FindObject(s.c_str());
        if (item == nullptr)
        {
            log_warning("Client tried getting non-existent object %s from us.", s.c_str());
        }
        else
        {
            connection.RequestedObjects.push_back(item);
        }
    }

    const char* player_name = (const char*)connection.Player->Name.c_str();
    Server_Send_MAP(&connection);
    Server_Send_EVENT_PLAYER_JOINED(player_name);
    Server_Send_GROUPLIST(connection);
    Server_Send_PLAYERLIST();
}

void Network::Server_Handle_AUTH(NetworkConnection& connection, NetworkPacket& packet)
{
    if (connection.AuthStatus != NETWORK_AUTH_OK)
    {
        const char* gameversion = packet.ReadString();
        const char* name = packet.ReadString();
        const char* password = packet.ReadString();
        const char* pubkey = packet.ReadString();
        uint32_t sigsize;
        packet >> sigsize;
        if (pubkey == nullptr)
        {
            connection.AuthStatus = NETWORK_AUTH_VERIFICATIONFAILURE;
        }
        else
        {
            try
            {
                std::vector<uint8_t> signature;
                signature.resize(sigsize);

                const uint8_t* signatureData = (const uint8_t*)packet.Read(sigsize);
                if (signatureData == nullptr)
                {
                    throw std::runtime_error("Failed to read packet.");
                }

                std::memcpy(signature.data(), signatureData, sigsize);

                auto ms = MemoryStream(pubkey, strlen(pubkey));
                if (!connection.Key.LoadPublic(&ms))
                {
                    throw std::runtime_error("Failed to load public key.");
                }

                bool verified = connection.Key.Verify(connection.Challenge.data(), connection.Challenge.size(), signature);
                const std::string hash = connection.Key.PublicKeyHash();
                if (verified)
                {
                    log_verbose("Signature verification ok. Hash %s", hash.c_str());
                    if (gConfigNetwork.known_keys_only && _userManager.GetUserByHash(hash) == nullptr)
                    {
                        log_verbose("Hash %s, not known", hash.c_str());
                        connection.AuthStatus = NETWORK_AUTH_UNKNOWN_KEY_DISALLOWED;
                    }
                    else
                    {
                        connection.AuthStatus = NETWORK_AUTH_VERIFIED;
                    }
                }
                else
                {
                    connection.AuthStatus = NETWORK_AUTH_VERIFICATIONFAILURE;
                    log_verbose("Signature verification failed!");
                }
            }
            catch (const std::exception&)
            {
                connection.AuthStatus = NETWORK_AUTH_VERIFICATIONFAILURE;
                log_verbose("Signature verification failed, invalid data!");
            }
        }

        bool passwordless = false;
        if (connection.AuthStatus == NETWORK_AUTH_VERIFIED)
        {
            const NetworkGroup* group = GetGroupByID(GetGroupIDByHash(connection.Key.PublicKeyHash()));
            passwordless = group->CanPerformCommand(MISC_COMMAND_PASSWORDLESS_LOGIN);
        }
        if (!gameversion || network_get_version() != gameversion)
        {
            connection.AuthStatus = NETWORK_AUTH_BADVERSION;
        }
        else if (!name)
        {
            connection.AuthStatus = NETWORK_AUTH_BADNAME;
        }
        else if (!passwordless)
        {
            if ((!password || strlen(password) == 0) && !_password.empty())
            {
                connection.AuthStatus = NETWORK_AUTH_REQUIREPASSWORD;
            }
            else if (password && _password != password)
            {
                connection.AuthStatus = NETWORK_AUTH_BADPASSWORD;
            }
        }

        if ((size_t)gConfigNetwork.maxplayers <= player_list.size())
        {
            connection.AuthStatus = NETWORK_AUTH_FULL;
        }
        else if (connection.AuthStatus == NETWORK_AUTH_VERIFIED)
        {
            connection.AuthStatus = NETWORK_AUTH_OK;
            const std::string hash = connection.Key.PublicKeyHash();
            Server_Client_Joined(name, hash, connection);
        }
        else if (connection.AuthStatus != NETWORK_AUTH_REQUIREPASSWORD)
        {
            log_error("Unknown failure (%d) while authenticating client", connection.AuthStatus);
        }
        Server_Send_AUTH(connection);
    }
}

void Network::Client_Handle_MAP([[maybe_unused]] NetworkConnection& connection, NetworkPacket& packet)
{
    uint32_t size, offset;
    packet >> size >> offset;
    int32_t chunksize = (int32_t)(packet.Size - packet.BytesRead);
    if (chunksize <= 0)
    {
        return;
    }
    if (size > chunk_buffer.size())
    {
        chunk_buffer.resize(size);
    }
    char str_downloading_map[256];
    uint32_t downloading_map_args[2] = {
        (offset + chunksize) / 1024,
        size / 1024,
    };
    format_string(str_downloading_map, 256, STR_MULTIPLAYER_DOWNLOADING_MAP, downloading_map_args);

    auto intent = Intent(WC_NETWORK_STATUS);
    intent.putExtra(INTENT_EXTRA_MESSAGE, std::string{ str_downloading_map });
    intent.putExtra(INTENT_EXTRA_CALLBACK, []() -> void { gNetwork.Close(); });
    context_open_intent(&intent);

    std::memcpy(&chunk_buffer[offset], (void*)packet.Read(chunksize), chunksize);
    if (offset + chunksize == size)
    {
        context_force_close_window_by_class(WC_NETWORK_STATUS);
        bool has_to_free = false;
        uint8_t* data = &chunk_buffer[0];
        size_t data_size = size;
        // zlib-compressed
        if (strcmp("open2_sv6_zlib", (char*)&chunk_buffer[0]) == 0)
        {
            log_verbose("Received zlib-compressed sv6 map");
            has_to_free = true;
            size_t header_len = strlen("open2_sv6_zlib") + 1;
            data = util_zlib_inflate(&chunk_buffer[header_len], size - header_len, &data_size);
            if (data == nullptr)
            {
                log_warning("Failed to decompress data sent from server.");
                Close();
                return;
            }
        }
        else
        {
            log_verbose("Assuming received map is in plain sv6 format");
        }

        auto ms = MemoryStream(data, data_size);
        if (LoadMap(&ms))
        {
            game_load_init();
            game_command_queue.clear();
            server_tick = gCurrentTicks;
            // window_network_status_open("Loaded new map from network");
            _desynchronised = false;
            _clientMapLoaded = true;
            gFirstTimeSaving = true;

            // Notify user he is now online and which shortcut key enables chat
            network_chat_show_connected_message();

            // Fix invalid vehicle sprite sizes, thus preventing visual corruption of sprites
            fix_invalid_vehicle_sprite_sizes();
        }
        else
        {
            // Something went wrong, game is not loaded. Return to main screen.
            auto loadOrQuitAction = LoadOrQuitAction(LoadOrQuitModes::OpenSavePrompt, PM_SAVE_BEFORE_QUIT);
            GameActions::Execute(&loadOrQuitAction);
        }
        if (has_to_free)
        {
            free(data);
        }
    }
}

bool Network::LoadMap(IStream* stream)
{
    bool result = false;
    try
    {
        auto context = GetContext();
        auto& objManager = context->GetObjectManager();
        auto importer = ParkImporter::CreateS6(context->GetObjectRepository());
        auto loadResult = importer->LoadFromStream(stream, false);
        objManager.LoadObjects(loadResult.RequiredObjects.data(), loadResult.RequiredObjects.size());
        importer->Import();

        sprite_position_tween_reset();

        // Read checksum
        [[maybe_unused]] uint32_t checksum = stream->ReadValue<uint32_t>();

        // Read other data not in normal save files
        stream->Read(gSpriteSpatialIndex, 0x10001 * sizeof(uint16_t));
        gGamePaused = stream->ReadValue<uint32_t>();
        _guestGenerationProbability = stream->ReadValue<uint32_t>();
        _suggestedGuestMaximum = stream->ReadValue<uint32_t>();
        gCheatsSandboxMode = stream->ReadValue<uint8_t>() != 0;
        gCheatsDisableClearanceChecks = stream->ReadValue<uint8_t>() != 0;
        gCheatsDisableSupportLimits = stream->ReadValue<uint8_t>() != 0;
        gCheatsDisableTrainLengthLimit = stream->ReadValue<uint8_t>() != 0;
        gCheatsEnableChainLiftOnAllTrack = stream->ReadValue<uint8_t>() != 0;
        gCheatsShowAllOperatingModes = stream->ReadValue<uint8_t>() != 0;
        gCheatsShowVehiclesFromOtherTrackTypes = stream->ReadValue<uint8_t>() != 0;
        gCheatsFastLiftHill = stream->ReadValue<uint8_t>() != 0;
        gCheatsDisableBrakesFailure = stream->ReadValue<uint8_t>() != 0;
        gCheatsDisableAllBreakdowns = stream->ReadValue<uint8_t>() != 0;
        gCheatsBuildInPauseMode = stream->ReadValue<uint8_t>() != 0;
        gCheatsIgnoreRideIntensity = stream->ReadValue<uint8_t>() != 0;
        gCheatsDisableVandalism = stream->ReadValue<uint8_t>() != 0;
        gCheatsDisableLittering = stream->ReadValue<uint8_t>() != 0;
        gCheatsNeverendingMarketing = stream->ReadValue<uint8_t>() != 0;
        gCheatsFreezeWeather = stream->ReadValue<uint8_t>() != 0;
        gCheatsDisablePlantAging = stream->ReadValue<uint8_t>() != 0;
        gCheatsAllowArbitraryRideTypeChanges = stream->ReadValue<uint8_t>() != 0;
        gCheatsDisableRideValueAging = stream->ReadValue<uint8_t>() != 0;
        gConfigGeneral.show_real_names_of_guests = stream->ReadValue<uint8_t>() != 0;
        gCheatsIgnoreResearchStatus = stream->ReadValue<uint8_t>() != 0;

        gLastAutoSaveUpdate = AUTOSAVE_PAUSE;
        result = true;
    }
    catch (const std::exception&)
    {
    }
    return result;
}

bool Network::SaveMap(IStream* stream, const std::vector<const ObjectRepositoryItem*>& objects) const
{
    bool result = false;
    viewport_set_saved_view();
    try
    {
        auto s6exporter = std::make_unique<S6Exporter>();
        s6exporter->ExportObjectsList = objects;
        s6exporter->Export();
        s6exporter->SaveGame(stream);

        // Write other data not in normal save files
        stream->Write(gSpriteSpatialIndex, 0x10001 * sizeof(uint16_t));
        stream->WriteValue<uint32_t>(gGamePaused);
        stream->WriteValue<uint32_t>(_guestGenerationProbability);
        stream->WriteValue<uint32_t>(_suggestedGuestMaximum);
        stream->WriteValue<uint8_t>(gCheatsSandboxMode);
        stream->WriteValue<uint8_t>(gCheatsDisableClearanceChecks);
        stream->WriteValue<uint8_t>(gCheatsDisableSupportLimits);
        stream->WriteValue<uint8_t>(gCheatsDisableTrainLengthLimit);
        stream->WriteValue<uint8_t>(gCheatsEnableChainLiftOnAllTrack);
        stream->WriteValue<uint8_t>(gCheatsShowAllOperatingModes);
        stream->WriteValue<uint8_t>(gCheatsShowVehiclesFromOtherTrackTypes);
        stream->WriteValue<uint8_t>(gCheatsFastLiftHill);
        stream->WriteValue<uint8_t>(gCheatsDisableBrakesFailure);
        stream->WriteValue<uint8_t>(gCheatsDisableAllBreakdowns);
        stream->WriteValue<uint8_t>(gCheatsBuildInPauseMode);
        stream->WriteValue<uint8_t>(gCheatsIgnoreRideIntensity);
        stream->WriteValue<uint8_t>(gCheatsDisableVandalism);
        stream->WriteValue<uint8_t>(gCheatsDisableLittering);
        stream->WriteValue<uint8_t>(gCheatsNeverendingMarketing);
        stream->WriteValue<uint8_t>(gCheatsFreezeWeather);
        stream->WriteValue<uint8_t>(gCheatsDisablePlantAging);
        stream->WriteValue<uint8_t>(gCheatsAllowArbitraryRideTypeChanges);
        stream->WriteValue<uint8_t>(gCheatsDisableRideValueAging);
        stream->WriteValue<uint8_t>(gConfigGeneral.show_real_names_of_guests);
        stream->WriteValue<uint8_t>(gCheatsIgnoreResearchStatus);

        result = true;
    }
    catch (const std::exception&)
    {
    }
    return result;
}

void Network::Client_Handle_CHAT([[maybe_unused]] NetworkConnection& connection, NetworkPacket& packet)
{
    const char* text = packet.ReadString();
    if (text)
    {
        chat_history_add(text);
    }
}

void Network::Server_Handle_CHAT(NetworkConnection& connection, NetworkPacket& packet)
{
    if (connection.Player)
    {
        NetworkGroup* group = GetGroupByID(connection.Player->Group);
        if (!group || !group->CanPerformCommand(MISC_COMMAND_CHAT))
        {
            return;
        }
    }
    const char* text = packet.ReadString();
    if (text)
    {
        const char* formatted = FormatChat(connection.Player, text);
        chat_history_add(formatted);
        Server_Send_CHAT(formatted);
    }
}

void Network::Client_Handle_GAMECMD([[maybe_unused]] NetworkConnection& connection, NetworkPacket& packet)
{
    uint32_t tick;
    uint32_t args[7];
    uint8_t playerid;
    uint8_t callback;
    packet >> tick >> args[0] >> args[1] >> args[2] >> args[3] >> args[4] >> args[5] >> args[6] >> playerid >> callback;

    game_command_queue.emplace(tick, args, playerid, callback, _commandId++);
}

void Network::Client_Handle_GAME_ACTION([[maybe_unused]] NetworkConnection& connection, NetworkPacket& packet)
{
    uint32_t tick;
    uint32_t actionType;
    packet >> tick >> actionType;

    MemoryStream stream;
    size_t size = packet.Size - packet.BytesRead;
    stream.WriteArray(packet.Read(size), size);
    stream.SetPosition(0);

    DataSerialiser ds(false, stream);

    GameAction::Ptr action = GameActions::Create(actionType);
    if (action == nullptr)
    {
        log_error("Received unregistered game action type: 0x%08X", actionType);
        return;
    }
    action->Serialise(ds);

    if (player_id == action->GetPlayer().id)
    {
        // Only execute callbacks that belong to us,
        // clients can have identical network ids assigned.
        auto itr = _gameActionCallbacks.find(action->GetNetworkId());
        if (itr != _gameActionCallbacks.end())
        {
            action->SetCallback(itr->second);

            _gameActionCallbacks.erase(itr);
        }
    }

    game_command_queue.emplace(tick, std::move(action), _commandId++);
}

void Network::Server_Handle_GAME_ACTION(NetworkConnection& connection, NetworkPacket& packet)
{
    uint32_t tick;
    uint32_t actionType;

    NetworkPlayer* player = connection.Player;
    if (player == nullptr)
    {
        return;
    }

    packet >> tick >> actionType;

    // Don't let clients send pause or quit
    if (actionType == GAME_COMMAND_TOGGLE_PAUSE || actionType == GAME_COMMAND_LOAD_OR_QUIT)
    {
        return;
    }

    // Check if player's group permission allows command to run
    NetworkGroup* group = GetGroupByID(connection.Player->Group);
    if (group == nullptr || group->CanPerformCommand(actionType) == false)
    {
        Server_Send_SHOWERROR(connection, STR_CANT_DO_THIS, STR_PERMISSION_DENIED);
        return;
    }

    // Create and enqueue the action.
    GameAction::Ptr ga = GameActions::Create(actionType);
    if (ga == nullptr)
    {
        log_error(
            "Received unregistered game action type: 0x%08X from player: (%d) %s", actionType, connection.Player->Id,
            connection.Player->Name.c_str());
        return;
    }

    // Player who is hosting is not affected by cooldowns.
    if ((player->Flags & NETWORK_PLAYER_FLAG_ISSERVER) == 0)
    {
        auto cooldownIt = player->CooldownTime.find(actionType);
        if (cooldownIt != std::end(player->CooldownTime))
        {
            if (cooldownIt->second > 0)
            {
                Server_Send_SHOWERROR(connection, STR_CANT_DO_THIS, STR_NETWORK_ACTION_RATE_LIMIT_MESSAGE);
                return;
            }
        }

        uint32_t cooldownTime = ga->GetCooldownTime();
        if (cooldownTime > 0)
        {
            player->CooldownTime[actionType] = cooldownTime;
        }
    }

    DataSerialiser stream(false);
    size_t size = packet.Size - packet.BytesRead;
    stream.GetStream().WriteArray(packet.Read(size), size);
    stream.GetStream().SetPosition(0);

    ga->Serialise(stream);
    // Set player to sender, should be 0 if sent from client.
    ga->SetPlayer(NetworkPlayerId_t{ connection.Player->Id });

    game_command_queue.emplace(tick, std::move(ga), _commandId++);
}

void Network::Server_Handle_GAMECMD(NetworkConnection& connection, NetworkPacket& packet)
{
    uint32_t tick;
    uint32_t args[7];
    uint8_t playerid;
    uint8_t callback;

    if (!connection.Player)
    {
        return;
    }

    playerid = connection.Player->Id;

    packet >> tick >> args[0] >> args[1] >> args[2] >> args[3] >> args[4] >> args[5] >> args[6] >> callback;

    int32_t commandCommand = args[4];

    uint32_t ticks = platform_get_ticks(); // tick count is different by time last_action_time is set, keep same value.

    // Check if player's group permission allows command to run
    NetworkGroup* group = GetGroupByID(connection.Player->Group);
    if (!group || !group->CanPerformCommand(commandCommand))
    {
        Server_Send_SHOWERROR(connection, STR_CANT_DO_THIS, STR_PERMISSION_DENIED);
        return;
    }

    // In case someone modifies the code / memory to enable cluster build,
    // require a small delay in between placing scenery to provide some security, as
    // cluster mode is a for loop that runs the place_scenery code multiple times.
    if (commandCommand == GAME_COMMAND_PLACE_SCENERY)
    {
        if (ticks - connection.Player->LastPlaceSceneryTime < ACTION_COOLDOWN_TIME_PLACE_SCENERY &&
            // In case platform_get_ticks() wraps after ~49 days, ignore larger logged times.
            ticks > connection.Player->LastPlaceSceneryTime)
        {
            if (!(group->CanPerformCommand(MISC_COMMAND_TOGGLE_SCENERY_CLUSTER)))
            {
                Server_Send_SHOWERROR(connection, STR_CANT_DO_THIS, STR_NETWORK_ACTION_RATE_LIMIT_MESSAGE);
                return;
            }
        }
    }
    // This is to prevent abuse of demolishing rides. Anyone that is not the server
    // host will have to wait a small amount of time in between deleting rides.
    else if (commandCommand == GAME_COMMAND_DEMOLISH_RIDE)
    {
        if (ticks - connection.Player->LastDemolishRideTime < ACTION_COOLDOWN_TIME_DEMOLISH_RIDE &&
            // In case platform_get_ticks() wraps after ~49 days, ignore larger logged times.
            ticks > connection.Player->LastDemolishRideTime)
        {
            Server_Send_SHOWERROR(connection, STR_CANT_DO_THIS, STR_NETWORK_ACTION_RATE_LIMIT_MESSAGE);
            return;
        }
    }

    game_command_queue.emplace(tick, args, playerid, callback, _commandId++);
}

void Network::Client_Handle_TICK([[maybe_unused]] NetworkConnection& connection, NetworkPacket& packet)
{
    uint32_t srand0;
    uint32_t flags;
    packet >> server_tick >> srand0 >> flags;

    ServerTickData_t tickData;
    tickData.srand0 = srand0;
    tickData.tick = server_tick;

    if (flags & NETWORK_TICK_FLAG_CHECKSUMS)
    {
        const char* text = packet.ReadString();
        if (text != nullptr)
        {
            tickData.spriteHash = text;
        }
    }

    // Don't let the history grow too much.
    while (_serverTickData.size() >= 100)
    {
        _serverTickData.erase(_serverTickData.begin());
    }

    _serverTickData.emplace(server_tick, tickData);
}

void Network::Client_Handle_PLAYERINFO([[maybe_unused]] NetworkConnection& connection, NetworkPacket& packet)
{
    uint32_t tick;
    packet >> tick;

    NetworkPlayer playerInfo;
    playerInfo.Read(packet);

    _pendingPlayerInfo.emplace(tick, playerInfo);
}

void Network::Client_Handle_PLAYERLIST([[maybe_unused]] NetworkConnection& connection, NetworkPacket& packet)
{
    uint32_t tick;
    uint8_t size;
    packet >> tick >> size;

    _pendingPlayerList.reset();
    _pendingPlayerList.tick = tick;

    for (uint32_t i = 0; i < size; i++)
    {
        NetworkPlayer tempplayer;
        tempplayer.Read(packet);

        _pendingPlayerList.players.push_back(tempplayer);
    }
}

void Network::Client_Handle_PING([[maybe_unused]] NetworkConnection& connection, [[maybe_unused]] NetworkPacket& packet)
{
    Client_Send_PING();
}

void Network::Server_Handle_PING(NetworkConnection& connection, [[maybe_unused]] NetworkPacket& packet)
{
    int32_t ping = platform_get_ticks() - connection.PingTime;
    if (ping < 0)
    {
        ping = 0;
    }
    if (connection.Player)
    {
        connection.Player->Ping = ping;
        window_invalidate_by_number(WC_PLAYER, connection.Player->Id);
    }
}

void Network::Client_Handle_PINGLIST([[maybe_unused]] NetworkConnection& connection, NetworkPacket& packet)
{
    uint8_t size;
    packet >> size;
    for (uint32_t i = 0; i < size; i++)
    {
        uint8_t id;
        uint16_t ping;
        packet >> id >> ping;
        NetworkPlayer* player = GetPlayerByID(id);
        if (player)
        {
            player->Ping = ping;
        }
    }
    window_invalidate_by_class(WC_PLAYER);
}

void Network::Client_Handle_SETDISCONNECTMSG(NetworkConnection& connection, NetworkPacket& packet)
{
    static std::string msg;
    const char* disconnectmsg = packet.ReadString();
    if (disconnectmsg)
    {
        msg = disconnectmsg;
        connection.SetLastDisconnectReason(msg.c_str());
    }
}

void Network::Server_Handle_GAMEINFO(NetworkConnection& connection, [[maybe_unused]] NetworkPacket& packet)
{
    Server_Send_GAMEINFO(connection);
}

void Network::Client_Handle_SHOWERROR([[maybe_unused]] NetworkConnection& connection, NetworkPacket& packet)
{
    rct_string_id title, message;
    packet >> title >> message;
    context_show_error(title, message);
}

void Network::Client_Handle_GROUPLIST([[maybe_unused]] NetworkConnection& connection, NetworkPacket& packet)
{
    group_list.clear();
    uint8_t size;
    packet >> size >> default_group;
    for (uint32_t i = 0; i < size; i++)
    {
        NetworkGroup group;
        group.Read(packet);
        auto newgroup = std::make_unique<NetworkGroup>(group);
        group_list.push_back(std::move(newgroup));
    }
}

void Network::Client_Handle_EVENT([[maybe_unused]] NetworkConnection& connection, NetworkPacket& packet)
{
    char text[256];
    uint16_t eventType;
    packet >> eventType;
    switch (eventType)
    {
        case SERVER_EVENT_PLAYER_JOINED:
        {
            const char* playerName = packet.ReadString();
            format_string(text, 256, STR_MULTIPLAYER_PLAYER_HAS_JOINED_THE_GAME, &playerName);
            chat_history_add(text);
            break;
        }
        case SERVER_EVENT_PLAYER_DISCONNECTED:
        {
            const char* playerName = packet.ReadString();
            const char* reason = packet.ReadString();
            const char* args[] = { playerName, reason };
            if (str_is_null_or_empty(reason))
            {
                format_string(text, 256, STR_MULTIPLAYER_PLAYER_HAS_DISCONNECTED_NO_REASON, args);
            }
            else
            {
                format_string(text, 256, STR_MULTIPLAYER_PLAYER_HAS_DISCONNECTED_WITH_REASON, args);
            }
            chat_history_add(text);
            break;
        }
    }
}

void Network::Client_Send_GAMEINFO()
{
    log_verbose("requesting gameinfo");
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32_t)NETWORK_COMMAND_GAMEINFO;
    _serverConnection->QueuePacket(std::move(packet));
}

static std::string json_stdstring_value(const json_t* string)
{
    const char* cstr = json_string_value(string);
    return cstr == nullptr ? std::string() : std::string(cstr);
}

void Network::Client_Handle_GAMEINFO([[maybe_unused]] NetworkConnection& connection, NetworkPacket& packet)
{
    const char* jsonString = packet.ReadString();

    json_error_t error;
    json_t* root = json_loads(jsonString, 0, &error);

    ServerName = json_stdstring_value(json_object_get(root, "name"));
    ServerDescription = json_stdstring_value(json_object_get(root, "description"));
    ServerGreeting = json_stdstring_value(json_object_get(root, "greeting"));

    json_t* jsonProvider = json_object_get(root, "provider");
    if (jsonProvider != nullptr)
    {
        ServerProviderName = json_stdstring_value(json_object_get(jsonProvider, "name"));
        ServerProviderEmail = json_stdstring_value(json_object_get(jsonProvider, "email"));
        ServerProviderWebsite = json_stdstring_value(json_object_get(jsonProvider, "website"));
    }
    json_decref(root);

    network_chat_show_server_greeting();
}

void network_set_env(const std::shared_ptr<IPlatformEnvironment>& env)
{
    gNetwork.SetEnvironment(env);
}

void network_close()
{
    gNetwork.Close();
}

void network_shutdown_client()
{
    gNetwork.ShutdownClient();
}

int32_t network_begin_client(const std::string& host, int32_t port)
{
    return gNetwork.BeginClient(host, port);
}

int32_t network_begin_server(int32_t port, const std::string& address)
{
    return gNetwork.BeginServer(port, address);
}

void network_update()
{
    gNetwork.Update();
}

void network_process_pending()
{
    gNetwork.ProcessPending();
}

void network_flush()
{
    gNetwork.Flush();
}

int32_t network_get_mode()
{
    return gNetwork.GetMode();
}

int32_t network_get_status()
{
    return gNetwork.GetStatus();
}

void network_check_desynchronization()
{
    return gNetwork.CheckDesynchronizaton();
}

void network_send_tick()
{
    gNetwork.Server_Send_TICK();
}

int32_t network_get_authstatus()
{
    return gNetwork.GetAuthStatus();
}

uint32_t network_get_server_tick()
{
    return gNetwork.GetServerTick();
}

uint8_t network_get_current_player_id()
{
    return gNetwork.GetPlayerID();
}

int32_t network_get_num_players()
{
    return (int32_t)gNetwork.player_list.size();
}

const char* network_get_player_name(uint32_t index)
{
    return (const char*)gNetwork.player_list[index]->Name.c_str();
}

uint32_t network_get_player_flags(uint32_t index)
{
    return gNetwork.player_list[index]->Flags;
}

int32_t network_get_player_ping(uint32_t index)
{
    return gNetwork.player_list[index]->Ping;
}

int32_t network_get_player_id(uint32_t index)
{
    return gNetwork.player_list[index]->Id;
}

money32 network_get_player_money_spent(uint32_t index)
{
    return gNetwork.player_list[index]->MoneySpent;
}

void network_add_player_money_spent(uint32_t index, money32 cost)
{
    gNetwork.player_list[index]->AddMoneySpent(cost);
}

int32_t network_get_player_last_action(uint32_t index, int32_t time)
{
    if (time && platform_get_ticks() > gNetwork.player_list[index]->LastActionTime + time)
    {
        return -999;
    }
    return gNetwork.player_list[index]->LastAction;
}

void network_set_player_last_action(uint32_t index, int32_t command)
{
    gNetwork.player_list[index]->LastAction = NetworkActions::FindCommand(command);
    gNetwork.player_list[index]->LastActionTime = platform_get_ticks();
}

LocationXYZ16 network_get_player_last_action_coord(uint32_t index)
{
    return gNetwork.player_list[index]->LastActionCoord;
}

void network_set_player_last_action_coord(uint32_t index, LocationXYZ16 coord)
{
    if (index < gNetwork.player_list.size())
    {
        gNetwork.player_list[index]->LastActionCoord = coord;
    }
}

uint32_t network_get_player_commands_ran(uint32_t index)
{
    return gNetwork.player_list[index]->CommandsRan;
}

int32_t network_get_player_index(uint32_t id)
{
    auto it = gNetwork.GetPlayerIteratorByID(id);
    if (it == gNetwork.player_list.end())
    {
        return -1;
    }
    return (int32_t)(gNetwork.GetPlayerIteratorByID(id) - gNetwork.player_list.begin());
}

uint8_t network_get_player_group(uint32_t index)
{
    return gNetwork.player_list[index]->Group;
}

void network_set_player_group(uint32_t index, uint32_t groupindex)
{
    gNetwork.player_list[index]->Group = gNetwork.group_list[groupindex]->Id;
}

int32_t network_get_group_index(uint8_t id)
{
    auto it = gNetwork.GetGroupIteratorByID(id);
    if (it == gNetwork.group_list.end())
    {
        return -1;
    }
    return (int32_t)(gNetwork.GetGroupIteratorByID(id) - gNetwork.group_list.begin());
}

uint8_t network_get_group_id(uint32_t index)
{
    return gNetwork.group_list[index]->Id;
}

int32_t network_get_num_groups()
{
    return (int32_t)gNetwork.group_list.size();
}

const char* network_get_group_name(uint32_t index)
{
    return gNetwork.group_list[index]->GetName().c_str();
}

void network_chat_show_connected_message()
{
    auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
    std::string s = windowManager->GetKeyboardShortcutString(41 /* SHORTCUT_OPEN_CHAT_WINDOW */);
    const char* sptr = s.c_str();

    utf8 buffer[256];
    format_string(buffer, sizeof(buffer), STR_MULTIPLAYER_CONNECTED_CHAT_HINT, &sptr);

    NetworkPlayer server;
    server.Name = "Server";
    const char* formatted = Network::FormatChat(&server, buffer);
    chat_history_add(formatted);
}

// Display server greeting if one exists
void network_chat_show_server_greeting()
{
    const char* greeting = network_get_server_greeting();
    if (!str_is_null_or_empty(greeting))
    {
        static char greeting_formatted[CHAT_INPUT_SIZE];
        char* lineCh = greeting_formatted;
        greeting_formatted[0] = 0;
        lineCh = utf8_write_codepoint(lineCh, FORMAT_OUTLINE);
        lineCh = utf8_write_codepoint(lineCh, FORMAT_GREEN);
        char* ptrtext = lineCh;
        safe_strcpy(lineCh, greeting, CHAT_INPUT_SIZE - 24); // Limit to 1000 characters so we don't overflow the buffer
        utf8_remove_format_codes(ptrtext, true);
        chat_history_add(greeting_formatted);
    }
}

void game_command_set_player_group(
    [[maybe_unused]] int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, [[maybe_unused]] int32_t* esi,
    [[maybe_unused]] int32_t* edi, [[maybe_unused]] int32_t* ebp)
{
    uint8_t playerid = (uint8_t)*ecx;
    uint8_t groupid = (uint8_t)*edx;
    NetworkPlayer* player = gNetwork.GetPlayerByID(playerid);
    NetworkGroup* fromgroup = gNetwork.GetGroupByID(game_command_playerid);
    if (!player)
    {
        gGameCommandErrorTitle = STR_CANT_DO_THIS;
        gGameCommandErrorText = STR_NONE;
        *ebx = MONEY32_UNDEFINED;
        return;
    }
    if (!gNetwork.GetGroupByID(groupid))
    {
        gGameCommandErrorTitle = STR_CANT_DO_THIS;
        gGameCommandErrorText = STR_NONE;
        *ebx = MONEY32_UNDEFINED;
        return;
    }
    if (player->Flags & NETWORK_PLAYER_FLAG_ISSERVER)
    {
        gGameCommandErrorTitle = STR_CANT_CHANGE_GROUP_THAT_THE_HOST_BELONGS_TO;
        gGameCommandErrorText = STR_NONE;
        *ebx = MONEY32_UNDEFINED;
        return;
    }
    if (groupid == 0 && fromgroup && fromgroup->Id != 0)
    {
        gGameCommandErrorTitle = STR_CANT_SET_TO_THIS_GROUP;
        gGameCommandErrorText = STR_NONE;
        *ebx = MONEY32_UNDEFINED;
        return;
    }
    if (*ebx & GAME_COMMAND_FLAG_APPLY)
    {
        player->Group = groupid;

        if (network_get_mode() == NETWORK_MODE_SERVER)
        {
            // Add or update saved user
            NetworkUserManager* userManager = &gNetwork._userManager;
            NetworkUser* networkUser = userManager->GetOrAddUser(player->KeyHash);
            networkUser->GroupId = groupid;
            networkUser->Name = player->Name;
            userManager->Save();
        }

        window_invalidate_by_number(WC_PLAYER, playerid);

        // Log set player group event
        NetworkPlayer* game_command_player = gNetwork.GetPlayerByID(game_command_playerid);
        NetworkGroup* new_player_group = gNetwork.GetGroupByID(groupid);
        char log_msg[256];
        const char* args[3] = {
            player->Name.c_str(),
            new_player_group->GetName().c_str(),
            game_command_player->Name.c_str(),
        };
        format_string(log_msg, 256, STR_LOG_SET_PLAYER_GROUP, args);
        network_append_server_log(log_msg);
    }
    *ebx = 0;
}

void game_command_modify_groups(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, [[maybe_unused]] int32_t* esi, int32_t* edi, int32_t* ebp)
{
    uint8_t action = (uint8_t)*eax;
    uint8_t groupid = (uint8_t)(*eax >> 8);
    uint8_t nameChunkIndex = (uint8_t)(*eax >> 16);

    switch (action)
    {
        case 0:
        { // add group
            if (*ebx & GAME_COMMAND_FLAG_APPLY)
            {
                NetworkGroup* newgroup = gNetwork.AddGroup();
                if (!newgroup)
                {
                    gGameCommandErrorTitle = STR_CANT_DO_THIS;
                    gGameCommandErrorText = STR_NONE;
                    *ebx = MONEY32_UNDEFINED;
                    return;
                }

                // Log add player group event
                NetworkPlayer* game_command_player = gNetwork.GetPlayerByID(game_command_playerid);
                if (game_command_player)
                {
                    char log_msg[256];
                    const char* args[2] = {
                        game_command_player->Name.c_str(),
                        newgroup->GetName().c_str(),
                    };
                    format_string(log_msg, 256, STR_LOG_ADD_PLAYER_GROUP, args);
                    network_append_server_log(log_msg);
                }
            }
        }
        break;
        case 1:
        { // remove group
            if (groupid == 0)
            {
                gGameCommandErrorTitle = STR_THIS_GROUP_CANNOT_BE_MODIFIED;
                gGameCommandErrorText = STR_NONE;
                *ebx = MONEY32_UNDEFINED;
                return;
            }
            for (auto it = gNetwork.player_list.begin(); it != gNetwork.player_list.end(); it++)
            {
                if ((*it)->Group == groupid)
                {
                    gGameCommandErrorTitle = STR_CANT_REMOVE_GROUP_THAT_PLAYERS_BELONG_TO;
                    gGameCommandErrorText = STR_NONE;
                    *ebx = MONEY32_UNDEFINED;
                    return;
                }
            }
            if (*ebx & GAME_COMMAND_FLAG_APPLY)
            {
                // Log remove player group event
                NetworkPlayer* game_command_player = gNetwork.GetPlayerByID(game_command_playerid);
                NetworkGroup* group = gNetwork.GetGroupByID(groupid);
                if (game_command_player && group)
                {
                    char log_msg[256];
                    const char* args[2] = {
                        game_command_player->Name.c_str(),
                        group->GetName().c_str(),
                    };
                    format_string(log_msg, 256, STR_LOG_REMOVE_PLAYER_GROUP, args);
                    network_append_server_log(log_msg);
                }

                gNetwork.RemoveGroup(groupid);
            }
        }
        break;
        case 2:
        { // set permissions
            int32_t index = *ecx;
            bool all = *edx & 1;
            bool allvalue = (*edx >> 1) & 1;
            if (groupid == 0)
            { // cant change admin group permissions
                gGameCommandErrorTitle = STR_THIS_GROUP_CANNOT_BE_MODIFIED;
                gGameCommandErrorText = STR_NONE;
                *ebx = MONEY32_UNDEFINED;
                return;
            }
            NetworkGroup* mygroup = nullptr;
            NetworkPlayer* player = gNetwork.GetPlayerByID(game_command_playerid);
            if (player && !all)
            {
                mygroup = gNetwork.GetGroupByID(player->Group);
                if (!mygroup || !mygroup->CanPerformAction(index))
                {
                    gGameCommandErrorTitle = STR_CANT_MODIFY_PERMISSION_THAT_YOU_DO_NOT_HAVE_YOURSELF;
                    gGameCommandErrorText = STR_NONE;
                    *ebx = MONEY32_UNDEFINED;
                    return;
                }
            }
            if (*ebx & GAME_COMMAND_FLAG_APPLY)
            {
                NetworkGroup* group = gNetwork.GetGroupByID(groupid);
                if (group)
                {
                    if (all)
                    {
                        if (mygroup)
                        {
                            if (allvalue)
                            {
                                group->ActionsAllowed = mygroup->ActionsAllowed;
                            }
                            else
                            {
                                group->ActionsAllowed.fill(0x00);
                            }
                        }
                    }
                    else
                    {
                        group->ToggleActionPermission(index);
                    }
                }

                // Log edit player group permissions event
                char log_msg[256];
                const char* args[2] = {
                    player->Name.c_str(),
                    group->GetName().c_str(),
                };
                format_string(log_msg, 256, STR_LOG_EDIT_PLAYER_GROUP_PERMISSIONS, args);
                network_append_server_log(log_msg);
            }
        }
        break;
        case 3:
        { // set group name
            NetworkGroup* group = gNetwork.GetGroupByID(groupid);
            const char* oldName = group->GetName().c_str();
            static char newName[128];

            size_t nameChunkOffset = nameChunkIndex - 1;
            if (nameChunkIndex == 0)
                nameChunkOffset = 2;
            nameChunkOffset *= 12;
            nameChunkOffset = (std::min)(nameChunkOffset, std::size(newName) - 12);
            std::memcpy((void*)((uintptr_t)newName + (uintptr_t)nameChunkOffset + 0), edx, sizeof(uint32_t));
            std::memcpy((void*)((uintptr_t)newName + (uintptr_t)nameChunkOffset + 4), ebp, sizeof(uint32_t));
            std::memcpy((void*)((uintptr_t)newName + (uintptr_t)nameChunkOffset + 8), edi, sizeof(uint32_t));

            if (nameChunkIndex != 0)
            {
                *ebx = 0;
                return;
            }

            if (strcmp(oldName, newName) == 0)
            {
                *ebx = 0;
                return;
            }

            if (newName[0] == 0)
            {
                gGameCommandErrorTitle = STR_CANT_RENAME_GROUP;
                gGameCommandErrorText = STR_INVALID_GROUP_NAME;
                *ebx = MONEY32_UNDEFINED;
                return;
            }

            if (*ebx & GAME_COMMAND_FLAG_APPLY)
            {
                if (group)
                {
                    // Log edit player group name event
                    NetworkPlayer* player = gNetwork.GetPlayerByID(game_command_playerid);
                    char log_msg[256];
                    const char* args[3] = {
                        player->Name.c_str(),
                        oldName,
                        newName,
                    };
                    format_string(log_msg, 256, STR_LOG_EDIT_PLAYER_GROUP_NAME, args);
                    network_append_server_log(log_msg);

                    group->SetName(newName);
                }
            }
        }
        break;
        case 4:
        { // set default group
            if (groupid == 0)
            {
                gGameCommandErrorTitle = STR_CANT_SET_TO_THIS_GROUP;
                gGameCommandErrorText = STR_NONE;
                *ebx = MONEY32_UNDEFINED;
                return;
            }
            if (*ebx & GAME_COMMAND_FLAG_APPLY)
            {
                gNetwork.SetDefaultGroup(groupid);

                // Log edit default player group event
                NetworkPlayer* player = gNetwork.GetPlayerByID(game_command_playerid);
                NetworkGroup* group = gNetwork.GetGroupByID(groupid);
                char log_msg[256];
                const char* args[2] = {
                    player->Name.c_str(),
                    group->GetName().c_str(),
                };
                format_string(log_msg, 256, STR_LOG_EDIT_DEFAULT_PLAYER_GROUP, args);
                network_append_server_log(log_msg);
            }
        }
        break;
    }

    gNetwork.SaveGroups();

    *ebx = 0;
}

void game_command_kick_player(
    int32_t* eax, int32_t* ebx, [[maybe_unused]] int32_t* ecx, [[maybe_unused]] int32_t* edx, [[maybe_unused]] int32_t* esi,
    [[maybe_unused]] int32_t* edi, [[maybe_unused]] int32_t* ebp)
{
    uint8_t playerid = (uint8_t)*eax;
    NetworkPlayer* player = gNetwork.GetPlayerByID(playerid);
    NetworkPlayer* kicker = gNetwork.GetPlayerByID(game_command_playerid);
    if (player == nullptr)
    {
        // Player might be already removed by the PLAYERLIST command, need to refactor non-game commands executing too early.
        return;
    }

    if (player && player->Flags & NETWORK_PLAYER_FLAG_ISSERVER)
    {
        gGameCommandErrorTitle = STR_CANT_KICK_THE_HOST;
        gGameCommandErrorText = STR_NONE;
        *ebx = MONEY32_UNDEFINED;
        return;
    }

    if (*ebx & GAME_COMMAND_FLAG_APPLY)
    {
        if (gNetwork.GetMode() == NETWORK_MODE_SERVER)
        {
            gNetwork.KickPlayer(playerid);

            NetworkUserManager* networkUserManager = &gNetwork._userManager;
            networkUserManager->Load();
            networkUserManager->RemoveUser(player->KeyHash);
            networkUserManager->Save();
        }

        if (kicker != nullptr)
        {
            // Log kick player event
            char log_msg[256];
            const char* args[2] = {
                player->Name.c_str(),
                kicker->Name.c_str(),
            };
            format_string(log_msg, 256, STR_LOG_PLAYER_KICKED, args);
            network_append_server_log(log_msg);
        }
    }
    *ebx = 0;
}

uint8_t network_get_default_group()
{
    return gNetwork.GetDefaultGroup();
}

int32_t network_get_num_actions()
{
    return (int32_t)NetworkActions::Actions.size();
}

rct_string_id network_get_action_name_string_id(uint32_t index)
{
    if (index < NetworkActions::Actions.size())
    {
        return NetworkActions::Actions[index].Name;
    }
    else
    {
        return STR_NONE;
    }
}

int32_t network_can_perform_action(uint32_t groupindex, uint32_t index)
{
    return gNetwork.group_list[groupindex]->CanPerformAction(index);
}

int32_t network_can_perform_command(uint32_t groupindex, int32_t index)
{
    return gNetwork.group_list[groupindex]->CanPerformCommand(index);
}

void network_set_pickup_peep(uint8_t playerid, Peep* peep)
{
    if (gNetwork.GetMode() == NETWORK_MODE_NONE)
    {
        _pickup_peep = peep;
    }
    else
    {
        NetworkPlayer* player = gNetwork.GetPlayerByID(playerid);
        if (player)
        {
            player->PickupPeep = peep;
        }
    }
}

Peep* network_get_pickup_peep(uint8_t playerid)
{
    if (gNetwork.GetMode() == NETWORK_MODE_NONE)
    {
        return _pickup_peep;
    }
    else
    {
        NetworkPlayer* player = gNetwork.GetPlayerByID(playerid);
        if (player)
        {
            return player->PickupPeep;
        }
        return nullptr;
    }
}

void network_set_pickup_peep_old_x(uint8_t playerid, int32_t x)
{
    if (gNetwork.GetMode() == NETWORK_MODE_NONE)
    {
        _pickup_peep_old_x = x;
    }
    else
    {
        NetworkPlayer* player = gNetwork.GetPlayerByID(playerid);
        if (player)
        {
            player->PickupPeepOldX = x;
        }
    }
}

int32_t network_get_pickup_peep_old_x(uint8_t playerid)
{
    if (gNetwork.GetMode() == NETWORK_MODE_NONE)
    {
        return _pickup_peep_old_x;
    }
    else
    {
        NetworkPlayer* player = gNetwork.GetPlayerByID(playerid);
        if (player)
        {
            return player->PickupPeepOldX;
        }
        return -1;
    }
}

int32_t network_get_current_player_group_index()
{
    NetworkPlayer* player = gNetwork.GetPlayerByID(gNetwork.GetPlayerID());
    if (player)
    {
        return network_get_group_index(player->Group);
    }
    return -1;
}

void network_send_map()
{
    gNetwork.Server_Send_MAP();
}

void network_send_chat(const char* text)
{
    if (gNetwork.GetMode() == NETWORK_MODE_CLIENT)
    {
        gNetwork.Client_Send_CHAT(text);
    }
    else if (gNetwork.GetMode() == NETWORK_MODE_SERVER)
    {
        NetworkPlayer* player = gNetwork.GetPlayerByID(gNetwork.GetPlayerID());
        const char* formatted = gNetwork.FormatChat(player, text);
        chat_history_add(formatted);
        gNetwork.Server_Send_CHAT(formatted);
    }
}

void network_send_game_action(const GameAction* action)
{
    switch (gNetwork.GetMode())
    {
        case NETWORK_MODE_SERVER:
            gNetwork.Server_Send_GAME_ACTION(action);
            break;
        case NETWORK_MODE_CLIENT:
            gNetwork.Client_Send_GAME_ACTION(action);
            break;
    }
}

void network_enqueue_game_action(const GameAction* action)
{
    gNetwork.EnqueueGameAction(action);
}

void network_send_gamecmd(
    uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi, uint32_t ebp, uint8_t callback)
{
    switch (gNetwork.GetMode())
    {
        case NETWORK_MODE_SERVER:
            gNetwork.Server_Send_GAMECMD(eax, ebx, ecx, edx, esi, edi, ebp, gNetwork.GetPlayerID(), callback);
            break;
        case NETWORK_MODE_CLIENT:
            gNetwork.Client_Send_GAMECMD(eax, ebx, ecx, edx, esi, edi, ebp, callback);
            break;
    }
}

void network_send_password(const std::string& password)
{
    utf8 keyPath[MAX_PATH];
    network_get_private_key_path(keyPath, sizeof(keyPath), gConfigNetwork.player_name);
    if (!platform_file_exists(keyPath))
    {
        log_error("Private key %s missing! Restart the game to generate it.", keyPath);
        return;
    }
    try
    {
        auto fs = FileStream(keyPath, FILE_MODE_OPEN);
        gNetwork._key.LoadPrivate(&fs);
    }
    catch (const std::exception&)
    {
        log_error("Error reading private key from %s.", keyPath);
        return;
    }
    const std::string pubkey = gNetwork._key.PublicKeyString();

    std::vector<uint8_t> signature;
    gNetwork._key.Sign(gNetwork._challenge.data(), gNetwork._challenge.size(), signature);
    // Don't keep private key in memory. There's no need and it may get leaked
    // when process dump gets collected at some point in future.
    gNetwork._key.Unload();
    gNetwork.Client_Send_AUTH(gConfigNetwork.player_name.c_str(), password, pubkey.c_str(), signature);
}

void network_set_password(const char* password)
{
    gNetwork.SetPassword(password);
}

void network_append_chat_log(const utf8* text)
{
    gNetwork.AppendChatLog(text);
}

void network_append_server_log(const utf8* text)
{
    gNetwork.AppendServerLog(text);
}

static void network_get_keys_directory(utf8* buffer, size_t bufferSize)
{
    platform_get_user_directory(buffer, "keys", bufferSize);
}

static void network_get_private_key_path(utf8* buffer, size_t bufferSize, const std::string& playerName)
{
    network_get_keys_directory(buffer, bufferSize);
    Path::Append(buffer, bufferSize, playerName.c_str());
    String::Append(buffer, bufferSize, ".privkey");
}

static void network_get_public_key_path(utf8* buffer, size_t bufferSize, const std::string& playerName, const utf8* hash)
{
    network_get_keys_directory(buffer, bufferSize);
    Path::Append(buffer, bufferSize, playerName.c_str());
    String::Append(buffer, bufferSize, "-");
    String::Append(buffer, bufferSize, hash);
    String::Append(buffer, bufferSize, ".pubkey");
}

const utf8* network_get_server_name()
{
    return gNetwork.ServerName.c_str();
}
const utf8* network_get_server_description()
{
    return gNetwork.ServerDescription.c_str();
}
const utf8* network_get_server_greeting()
{
    return gNetwork.ServerGreeting.c_str();
}
const utf8* network_get_server_provider_name()
{
    return gNetwork.ServerProviderName.c_str();
}
const utf8* network_get_server_provider_email()
{
    return gNetwork.ServerProviderEmail.c_str();
}
const utf8* network_get_server_provider_website()
{
    return gNetwork.ServerProviderWebsite.c_str();
}

std::string network_get_version()
{
    return NETWORK_STREAM_ID;
}

NetworkStats_t network_get_stats()
{
    return gNetwork.GetStats();
}

#else
int32_t network_get_mode()
{
    return NETWORK_MODE_NONE;
}
int32_t network_get_status()
{
    return NETWORK_STATUS_NONE;
}
int32_t network_get_authstatus()
{
    return NETWORK_AUTH_NONE;
}
uint32_t network_get_server_tick()
{
    return gCurrentTicks;
}
void network_flush()
{
}
void network_send_tick()
{
}
void network_check_desynchronization()
{
}
void network_enqueue_game_action(const GameAction* action)
{
}
void network_send_gamecmd(
    uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi, uint32_t ebp, uint8_t callback)
{
}
void network_send_game_action(const GameAction* action)
{
}
void network_send_map()
{
}
void network_update()
{
}
void network_process_pending()
{
}
int32_t network_begin_client(const std::string& host, int32_t port)
{
    return 1;
}
int32_t network_begin_server(int32_t port, const std::string& address)
{
    return 1;
}
int32_t network_get_num_players()
{
    return 1;
}
const char* network_get_player_name(uint32_t index)
{
    return "local (OpenRCT2 compiled without MP)";
}
uint32_t network_get_player_flags(uint32_t index)
{
    return 0;
}
int32_t network_get_player_ping(uint32_t index)
{
    return 0;
}
int32_t network_get_player_id(uint32_t index)
{
    return 0;
}
money32 network_get_player_money_spent(uint32_t index)
{
    return MONEY(0, 0);
}
void network_add_player_money_spent(uint32_t index, money32 cost)
{
}
int32_t network_get_player_last_action(uint32_t index, int32_t time)
{
    return -999;
}
void network_set_player_last_action(uint32_t index, int32_t command)
{
}
LocationXYZ16 network_get_player_last_action_coord(uint32_t index)
{
    return { 0, 0, 0 };
}
void network_set_player_last_action_coord(uint32_t index, LocationXYZ16 coord)
{
}
uint32_t network_get_player_commands_ran(uint32_t index)
{
    return 0;
}
int32_t network_get_player_index(uint32_t id)
{
    return -1;
}
uint8_t network_get_player_group(uint32_t index)
{
    return 0;
}
void network_set_player_group(uint32_t index, uint32_t groupindex)
{
}
int32_t network_get_group_index(uint8_t id)
{
    return -1;
}
uint8_t network_get_group_id(uint32_t index)
{
    return 0;
}
int32_t network_get_num_groups()
{
    return 0;
}
const char* network_get_group_name(uint32_t index)
{
    return "";
};
void game_command_set_player_group(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp)
{
}
void game_command_modify_groups(
    int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp)
{
}
void game_command_kick_player(int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp)
{
}
uint8_t network_get_default_group()
{
    return 0;
}
int32_t network_get_num_actions()
{
    return 0;
}
rct_string_id network_get_action_name_string_id(uint32_t index)
{
    return -1;
}
int32_t network_can_perform_action(uint32_t groupindex, uint32_t index)
{
    return 0;
}
int32_t network_can_perform_command(uint32_t groupindex, int32_t index)
{
    return 0;
}
void network_set_pickup_peep(uint8_t playerid, Peep* peep)
{
    _pickup_peep = peep;
}
Peep* network_get_pickup_peep(uint8_t playerid)
{
    return _pickup_peep;
}
void network_set_pickup_peep_old_x(uint8_t playerid, int32_t x)
{
    _pickup_peep_old_x = x;
}
int32_t network_get_pickup_peep_old_x(uint8_t playerid)
{
    return _pickup_peep_old_x;
}
void network_send_chat(const char* text)
{
}
void network_send_password(const std::string& password)
{
}
void network_close()
{
}
void network_set_env(const std::shared_ptr<OpenRCT2::IPlatformEnvironment>&)
{
}
void network_shutdown_client()
{
}
void network_set_password(const char* password)
{
}
uint8_t network_get_current_player_id()
{
    return 0;
}
int32_t network_get_current_player_group_index()
{
    return 0;
}
void network_append_chat_log(const utf8* text)
{
}
void network_append_server_log(const utf8* text)
{
}
const utf8* network_get_server_name()
{
    return nullptr;
}
const utf8* network_get_server_description()
{
    return nullptr;
}
const utf8* network_get_server_greeting()
{
    return nullptr;
}
const utf8* network_get_server_provider_name()
{
    return nullptr;
}
const utf8* network_get_server_provider_email()
{
    return nullptr;
}
const utf8* network_get_server_provider_website()
{
    return nullptr;
}
std::string network_get_version()
{
    return "Multiplayer disabled";
}
NetworkStats_t network_get_stats()
{
    return NetworkStats_t{};
}
#endif /* DISABLE_NETWORK */
