/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <memory>
#include <string>

enum {
    NETWORK_MODE_NONE,
    NETWORK_MODE_CLIENT,
    NETWORK_MODE_SERVER
};

enum {
    NETWORK_PLAYER_FLAG_ISSERVER = 1 << 0,
};

enum {
    NETWORK_STATUS_NONE,
    NETWORK_STATUS_READY,
    NETWORK_STATUS_CONNECTING,
    NETWORK_STATUS_CONNECTED
};

#define NETWORK_DEFAULT_PORT 11753
#define MAX_SERVER_DESCRIPTION_LENGTH 256

#include "../common.h"
#include "../Game.h"
#include "../localisation/StringIds.h"

#include "../Version.h"
#include "NetworkTypes.h"

struct GameAction;
struct rct_peep;
struct LocationXYZ16;

namespace OpenRCT2
{
    interface IPlatformEnvironment;
}

#ifndef DISABLE_NETWORK

#include <array>
#include <list>
#include <set>
#include <vector>
#include <functional>
#include <fstream>
#include <map>
#include "../actions/GameAction.h"
#include "../core/Json.hpp"
#include "../core/Nullable.hpp"
#include "../core/MemoryStream.h"
#include "NetworkConnection.h"
#include "NetworkGroup.h"
#include "NetworkKey.h"
#include "NetworkPacket.h"
#include "NetworkPlayer.h"
#include "NetworkServerAdvertiser.h"
#include "NetworkUser.h"
#include "TcpSocket.h"

enum {
    NETWORK_TICK_FLAG_CHECKSUMS = 1 << 0,
};

struct ObjectRepositoryItem;

class Network
{
public:
    Network();
    ~Network();
    void SetEnvironment(const std::shared_ptr<OpenRCT2::IPlatformEnvironment>& env);
    bool Init();
    void Close();
    bool BeginClient(const char* host, uint16_t port);
    bool BeginServer(uint16_t port, const char* address);
    int32_t GetMode();
    int32_t GetStatus();
    int32_t GetAuthStatus();
    uint32_t GetServerTick();
    uint8_t GetPlayerID();
    void Update();
    void Flush();
    void ProcessGameCommandQueue();
    void EnqueueGameAction(const GameAction *action);
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
    uint8_t GetGroupIDByHash(const std::string &keyhash);
    void SetDefaultGroup(uint8_t id);
    void SaveGroups();
    void LoadGroups();

    std::string BeginLog(const std::string &directory, const std::string &midName, const std::string &filenameFormat);
    void AppendLog(std::ostream &fs, const std::string &s);

    void BeginChatLog();
    void AppendChatLog(const std::string &s);
    void CloseChatLog();

    void BeginServerLog();
    void AppendServerLog(const std::string &s);
    void CloseServerLog();

    void Client_Send_TOKEN();
    void Client_Send_AUTH(const char* name, const char* password, const char *pubkey, const char *sig, size_t sigsize);
    void Server_Send_AUTH(NetworkConnection& connection);
    void Server_Send_TOKEN(NetworkConnection& connection);
    void Server_Send_MAP(NetworkConnection* connection = nullptr);
    void Client_Send_CHAT(const char* text);
    void Server_Send_CHAT(const char* text);
    void Client_Send_GAMECMD(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi, uint32_t ebp, uint8_t callback);
    void Server_Send_GAMECMD(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi, uint32_t ebp, uint8_t playerid, uint8_t callback);
    void Client_Send_GAME_ACTION(const GameAction *action);
    void Server_Send_GAME_ACTION(const GameAction *action);
    void Server_Send_TICK();
    void Server_Send_PLAYERLIST();
    void Client_Send_PING();
    void Server_Send_PING();
    void Server_Send_PINGLIST();
    void Server_Send_SETDISCONNECTMSG(NetworkConnection& connection, const char* msg);
    void Server_Send_GAMEINFO(NetworkConnection& connection);
    void Server_Send_SHOWERROR(NetworkConnection& connection, rct_string_id title, rct_string_id message);
    void Server_Send_GROUPLIST(NetworkConnection& connection);
    void Server_Send_EVENT_PLAYER_JOINED(const char *playerName);
    void Server_Send_EVENT_PLAYER_DISCONNECTED(const char *playerName, const char *reason);
    void Client_Send_GAMEINFO();
    void Client_Send_OBJECTS(const std::vector<std::string> &objects);
    void Server_Send_OBJECTS(NetworkConnection& connection, const std::vector<const ObjectRepositoryItem *> &objects) const;

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
    void CloseConnection();

    bool ProcessConnection(NetworkConnection& connection);
    void ProcessPacket(NetworkConnection& connection, NetworkPacket& packet);
    void AddClient(ITcpSocket * socket);
    void RemoveClient(std::unique_ptr<NetworkConnection>& connection);
    NetworkPlayer* AddPlayer(const utf8 *name, const std::string &keyhash);
    std::string MakePlayerNameUnique(const std::string &name);

    const char* GetMasterServerUrl();
    std::string GenerateAdvertiseKey();
    void SetupDefaultGroups();

    bool LoadMap(IStream * stream);
    bool SaveMap(IStream * stream, const std::vector<const ObjectRepositoryItem *> &objects) const;

    struct GameCommand
    {
        GameCommand(uint32_t t, uint32_t* args, uint8_t p, uint8_t cb, uint32_t id) {
            tick = t; eax = args[0]; ebx = args[1]; ecx = args[2]; edx = args[3];
            esi = args[4]; edi = args[5]; ebp = args[6]; playerid = p; callback = cb;
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
        bool operator<(const GameCommand& comp) const {
            // First sort by tick
            if (tick < comp.tick)
                return true;
            if (tick > comp.tick)
                return false;

            // If the ticks are equal sort by commandIndex
            return commandIndex < comp.commandIndex;
        }
    };

    int32_t mode = NETWORK_MODE_NONE;
    int32_t status = NETWORK_STATUS_NONE;
    bool _closeLock = false;
    bool _requireClose = false;
    bool wsa_initialized = false;
    ITcpSocket * listening_socket = nullptr;
    uint16_t listening_port = 0;
    NetworkConnection * server_connection = nullptr;
    SOCKET_STATUS _lastConnectStatus = SOCKET_STATUS_CLOSED;
    uint32_t last_tick_sent_time = 0;
    uint32_t last_ping_sent_time = 0;
    uint32_t server_tick = 0;
    uint32_t server_srand0 = 0;
    uint32_t server_srand0_tick = 0;
    std::string server_sprite_hash;
    uint8_t player_id = 0;
    std::list<std::unique_ptr<NetworkConnection>> client_connection_list;
    std::multiset<GameCommand> game_command_queue;
    std::vector<uint8_t> chunk_buffer;
    std::string _password;
    bool _desynchronised = false;
    INetworkServerAdvertiser * _advertiser = nullptr;
    uint32_t server_connect_time = 0;
    uint8_t default_group = 0;
    uint32_t game_commands_processed_this_tick = 0;
    uint32_t _commandId;
    uint32_t _actionId;
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
    void Server_Client_Joined(const char* name, const std::string &keyhash, NetworkConnection& connection);
    void Client_Handle_MAP(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_CHAT(NetworkConnection& connection, NetworkPacket& packet);
    void Server_Handle_CHAT(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_GAMECMD(NetworkConnection& connection, NetworkPacket& packet);
    void Server_Handle_GAMECMD(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_GAME_ACTION(NetworkConnection& connection, NetworkPacket& packet);
    void Server_Handle_GAME_ACTION(NetworkConnection& connection, NetworkPacket& packet);
    void Client_Handle_TICK(NetworkConnection& connection, NetworkPacket& packet);
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

    uint8_t * save_for_network(size_t &out_size, const std::vector<const ObjectRepositoryItem *> &objects) const;

    std::ofstream _chat_log_fs;
    std::ofstream _server_log_fs;
};

#endif /* DISABLE_NETWORK */

void network_set_env(const std::shared_ptr<OpenRCT2::IPlatformEnvironment>& env);
void network_close();
void network_shutdown_client();
int32_t network_begin_client(const char *host, int32_t port);
int32_t network_begin_server(int32_t port, const char* address);

int32_t network_get_mode();
int32_t network_get_status();
void network_check_desynchronization();
void network_send_tick();
void network_update();
void network_process_game_commands();
void network_flush();

int32_t network_get_authstatus();
uint32_t network_get_server_tick();
uint8_t network_get_current_player_id();
int32_t network_get_num_players();
const char* network_get_player_name(uint32_t index);
uint32_t network_get_player_flags(uint32_t index);
int32_t network_get_player_ping(uint32_t index);
int32_t network_get_player_id(uint32_t index);
money32 network_get_player_money_spent(uint32_t index);
void network_add_player_money_spent(uint32_t index, money32 cost);
int32_t network_get_player_last_action(uint32_t index, int32_t time);
void network_set_player_last_action(uint32_t index, int32_t command);
LocationXYZ16 network_get_player_last_action_coord(uint32_t index);
void network_set_player_last_action_coord(uint32_t index, LocationXYZ16 coord);
uint32_t network_get_player_commands_ran(uint32_t index);
int32_t network_get_player_index(uint8_t id);
uint8_t network_get_player_group(uint32_t index);
void network_set_player_group(uint32_t index, uint32_t groupindex);
int32_t network_get_group_index(uint8_t id);
int32_t network_get_current_player_group_index();
uint8_t network_get_group_id(uint32_t index);
int32_t network_get_num_groups();
const char* network_get_group_name(uint32_t index);
void game_command_set_player_group(int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);
void game_command_modify_groups(int32_t *eax, int32_t *ebx, int32_t *ecx, int32_t *edx, int32_t *esi, int32_t *edi, int32_t *ebp);
void game_command_kick_player(int32_t *eax, int32_t *ebx, int32_t *ecx, int32_t *edx, int32_t *esi, int32_t *edi, int32_t *ebp);
uint8_t network_get_default_group();
int32_t network_get_num_actions();
rct_string_id network_get_action_name_string_id(uint32_t index);
int32_t network_can_perform_action(uint32_t groupindex, uint32_t index);
int32_t network_can_perform_command(uint32_t groupindex, int32_t index);
void network_set_pickup_peep(uint8_t playerid, rct_peep* peep);
rct_peep* network_get_pickup_peep(uint8_t playerid);
void network_set_pickup_peep_old_x(uint8_t playerid, int32_t x);
int32_t network_get_pickup_peep_old_x(uint8_t playerid);

void network_send_map();
void network_send_chat(const char* text);
void network_send_gamecmd(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi, uint32_t ebp, uint8_t callback);
void network_send_game_action(const GameAction *action);
void network_enqueue_game_action(const GameAction *action);
void network_send_password(const char* password);

void network_set_password(const char* password);

void network_print_error();
void network_append_chat_log(const utf8 *text);
void network_append_server_log(const utf8 *text);
const utf8 * network_get_server_name();
const utf8 * network_get_server_description();
const utf8 * network_get_server_greeting();
const utf8 * network_get_server_provider_name();
const utf8 * network_get_server_provider_email();
const utf8 * network_get_server_provider_website();

std::string network_get_version();
