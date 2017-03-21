#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#ifndef _NETWORK_H_
#define _NETWORK_H_

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
#include "../game.h"
#include "../localisation/string_ids.h"

#include "../Version.h"
#include "NetworkTypes.h"

#ifndef DISABLE_NETWORK

// This define specifies which version of network stream current build uses.
// It is used for making sure only compatible builds get connected, even within
// single OpenRCT2 version.
#define NETWORK_STREAM_VERSION "5"
#define NETWORK_STREAM_ID OPENRCT2_VERSION "-" NETWORK_STREAM_VERSION

#ifdef __cplusplus

#include <array>
#include <list>
#include <set>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <openssl/evp.h>
#include "../core/Json.hpp"
#include "../core/Nullable.hpp"
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

namespace OpenRCT2
{
    interface IPlatformEnvironment;
}

class Network
{
public:
    Network();
    ~Network();
    void SetEnvironment(OpenRCT2::IPlatformEnvironment * env);
    bool Init();
    void Close();
    bool BeginClient(const char* host, uint16 port);
    bool BeginServer(uint16 port, const char* address);
    sint32 GetMode();
    sint32 GetStatus();
    sint32 GetAuthStatus();
    uint32 GetServerTick();
    uint8 GetPlayerID();
    void Update();
    void ProcessGameCommandQueue();
    std::vector<std::unique_ptr<NetworkPlayer>>::iterator GetPlayerIteratorByID(uint8 id);
    NetworkPlayer* GetPlayerByID(uint8 id);
    std::vector<std::unique_ptr<NetworkGroup>>::iterator GetGroupIteratorByID(uint8 id);
    NetworkGroup* GetGroupByID(uint8 id);
    static const char* FormatChat(NetworkPlayer* fromplayer, const char* text);
    void SendPacketToClients(NetworkPacket& packet, bool front = false, bool gameCmd = false);
    bool CheckSRAND(uint32 tick, uint32 srand0);
    void KickPlayer(sint32 playerId);
    void SetPassword(const char* password);
    void ShutdownClient();
    NetworkGroup* AddGroup();
    void RemoveGroup(uint8 id);
    uint8 GetDefaultGroup();
    uint8 GetGroupIDByHash(const std::string &keyhash);
    void SetDefaultGroup(uint8 id);
    void SaveGroups();
    void LoadGroups();

    std::string BeginLog(const std::string &directory, const std::string &filenameFormat);
    void AppendLog(const std::string &logPath, const std::string &s);

    void BeginChatLog();
    void AppendChatLog(const std::string &s);
    void CloseChatLog();

    void BeginServerLog();
    void AppendServerLog(const std::string &s);
    void CloseServerLog();

    void Client_Send_TOKEN();
    void Client_Send_AUTH(const char* name, const char* password, const char *pubkey, const char *sig, size_t sigsize);
    void Client_Send_AUTH(const char* name, const char* password, const char *pubkey);
    void Server_Send_AUTH(NetworkConnection& connection);
    void Server_Send_TOKEN(NetworkConnection& connection);
    void Server_Send_MAP(NetworkConnection* connection = nullptr);
    void Client_Send_CHAT(const char* text);
    void Server_Send_CHAT(const char* text);
    void Client_Send_GAMECMD(uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp, uint8 callback);
    void Server_Send_GAMECMD(uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp, uint8 playerid, uint8 callback);
    void Client_Send_GAME_ACTION(const IGameAction * action);
    void Server_Send_GAME_ACTION(const IGameAction * action);
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
    std::vector<uint8> _challenge;
    NetworkUserManager _userManager;

    std::string ServerName;
    std::string ServerDescription;
    std::string ServerGreeting;
    std::string ServerProviderName;
    std::string ServerProviderEmail;
    std::string ServerProviderWebsite;

private:
    bool ProcessConnection(NetworkConnection& connection);
    void ProcessPacket(NetworkConnection& connection, NetworkPacket& packet);
    void AddClient(ITcpSocket * socket);
    void RemoveClient(std::unique_ptr<NetworkConnection>& connection);
    NetworkPlayer* AddPlayer(const utf8 *name, const std::string &keyhash);
    std::string MakePlayerNameUnique(const std::string &name);
    void PrintError();
    const char* GetMasterServerUrl();
    std::string GenerateAdvertiseKey();
    void SetupDefaultGroups();

    bool LoadMap(IStream * stream);
    bool SaveMap(IStream * stream, const std::vector<const ObjectRepositoryItem *> &objects) const;

    struct GameCommand
    {
        GameCommand(uint32 t, uint32* args, uint8 p, uint8 cb) {
            tick = t; eax = args[0]; ebx = args[1]; ecx = args[2]; edx = args[3];
            esi = args[4]; edi = args[5]; ebp = args[6]; playerid = p; callback = cb;
        }
        uint32 tick;
        uint32 eax, ebx, ecx, edx, esi, edi, ebp;
        uint8 playerid;
        uint8 callback;
        bool operator<(const GameCommand& comp) const {
            return tick < comp.tick;
        }
    };

    sint32 mode = NETWORK_MODE_NONE;
    sint32 status = NETWORK_STATUS_NONE;
    bool _closeLock = false;
    bool _requireClose = false;
    bool wsa_initialized = false;
    ITcpSocket * listening_socket = nullptr;
    uint16 listening_port = 0;
    NetworkConnection * server_connection = nullptr;
    SOCKET_STATUS _lastConnectStatus = SOCKET_STATUS_CLOSED;
    uint32 last_tick_sent_time = 0;
    uint32 last_ping_sent_time = 0;
    uint32 server_tick = 0;
    uint32 server_srand0 = 0;
    uint32 server_srand0_tick = 0;
    char server_sprite_hash[EVP_MAX_MD_SIZE + 1];
    uint8 player_id = 0;
    std::list<std::unique_ptr<NetworkConnection>> client_connection_list;
    std::multiset<GameCommand> game_command_queue;
    std::vector<uint8> chunk_buffer;
    std::string _password;
    bool _desynchronised = false;
    INetworkServerAdvertiser * _advertiser = nullptr;
    uint32 server_connect_time = 0;
    uint8 default_group = 0;
    uint32 game_commands_processed_this_tick = 0;
    std::string _chatLogPath;
    std::string _chatLogFilenameFormat = "%Y%m%d-%H%M%S.txt";
    std::string _serverLogPath;
    std::string _serverLogFilenameFormat = "-%Y%m%d-%H%M%S.txt";
    OpenRCT2::IPlatformEnvironment * _env = nullptr;

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

    uint8 * save_for_network(size_t &out_size, const std::vector<const ObjectRepositoryItem *> &objects) const;
};

#endif // __cplusplus
#else /* DISABLE_NETWORK */
#define NETWORK_STREAM_ID "Multiplayer disabled"
#endif /* DISABLE_NETWORK */

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
void network_set_env(void * env);
void network_close();
void network_shutdown_client();
sint32 network_begin_client(const char *host, sint32 port);
sint32 network_begin_server(sint32 port, const char* address);

sint32 network_get_mode();
sint32 network_get_status();
void network_update();
void network_process_game_commands();
sint32 network_get_authstatus();
uint32 network_get_server_tick();
uint8 network_get_current_player_id();
sint32 network_get_num_players();
const char* network_get_player_name(uint32 index);
uint32 network_get_player_flags(uint32 index);
sint32 network_get_player_ping(uint32 index);
sint32 network_get_player_id(uint32 index);
money32 network_get_player_money_spent(uint32 index);
void network_add_player_money_spent(uint32 index, money32 cost);
sint32 network_get_player_last_action(uint32 index, sint32 time);
void network_set_player_last_action(uint32 index, sint32 command);
rct_xyz16 network_get_player_last_action_coord(uint32 index);
void network_set_player_last_action_coord(uint32 index, rct_xyz16 coord);
uint32 network_get_player_commands_ran(uint32 index);
sint32 network_get_player_index(uint8 id);
uint8 network_get_player_group(uint32 index);
void network_set_player_group(uint32 index, uint32 groupindex);
sint32 network_get_group_index(uint8 id);
sint32 network_get_current_player_group_index();
uint8 network_get_group_id(uint32 index);
sint32 network_get_num_groups();
const char* network_get_group_name(uint32 index);
void game_command_set_player_group(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);
void game_command_modify_groups(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp);
void game_command_kick_player(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp);
uint8 network_get_default_group();
sint32 network_get_num_actions();
rct_string_id network_get_action_name_string_id(uint32 index);
sint32 network_can_perform_action(uint32 groupindex, uint32 index);
sint32 network_can_perform_command(uint32 groupindex, uint32 index);
void network_set_pickup_peep(uint8 playerid, rct_peep* peep);
rct_peep* network_get_pickup_peep(uint8 playerid);
void network_set_pickup_peep_old_x(uint8 playerid, sint32 x);
sint32 network_get_pickup_peep_old_x(uint8 playerid);

void network_send_map();
void network_send_chat(const char* text);
void network_send_gamecmd(uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp, uint8 callback);
#ifdef __cplusplus
void network_send_game_action(const IGameAction * action);
#endif
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

#ifdef __cplusplus
}
#endif // __cplusplus

#endif
