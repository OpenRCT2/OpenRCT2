#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#include "../common.h"
#include "../game.h"
#include "../platform/platform.h"
#include "../localisation/string_ids.h"
#include <openssl/evp.h>
#ifdef __cplusplus
}
#endif // __cplusplus

#include "NetworkTypes.h"

#ifndef DISABLE_NETWORK

// This define specifies which version of network stream current build uses.
// It is used for making sure only compatible builds get connected, even within
// single OpenRCT2 version.
#define NETWORK_STREAM_VERSION "13"
#define NETWORK_STREAM_ID OPENRCT2_VERSION "-" NETWORK_STREAM_VERSION

#ifdef __cplusplus

#include <array>
#include <list>
#include <set>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <SDL.h>
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

class Network
{
public:
	Network();
	~Network();
	bool Init();
	void Close();
	bool BeginClient(const char* host, unsigned short port);
	bool BeginServer(unsigned short port, const char* address = NULL);
	int GetMode();
	int GetStatus();
	int GetAuthStatus();
	uint32 GetServerTick();
	uint8 GetPlayerID();
	void Update();
	std::vector<std::unique_ptr<NetworkPlayer>>::iterator GetPlayerIteratorByID(uint8 id);
	NetworkPlayer* GetPlayerByID(uint8 id);
	std::vector<std::unique_ptr<NetworkGroup>>::iterator GetGroupIteratorByID(uint8 id);
	NetworkGroup* GetGroupByID(uint8 id);
	static const char* FormatChat(NetworkPlayer* fromplayer, const char* text);
	void SendPacketToClients(NetworkPacket& packet, bool front = false);
	bool CheckSRAND(uint32 tick, uint32 srand0);
	void KickPlayer(int playerId);
	void SetPassword(const char* password);
	void ShutdownClient();
	NetworkGroup* AddGroup();
	void RemoveGroup(uint8 id);
	uint8 GetDefaultGroup();
	uint8 GetGroupIDByHash(const std::string &keyhash);
	void SetDefaultGroup(uint8 id);
	void SaveGroups();
	void LoadGroups();

	void BeginChatLog();
	void AppendChatLog(const utf8 *text);
	void CloseChatLog();

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

	std::vector<std::unique_ptr<NetworkPlayer>> player_list;
	std::vector<std::unique_ptr<NetworkGroup>> group_list;
	NetworkKey key;
	std::vector<uint8> challenge;
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
	void ProcessGameCommandQueue();
	void AddClient(ITcpSocket * socket);
	void RemoveClient(std::unique_ptr<NetworkConnection>& connection);
	NetworkPlayer* AddPlayer(const utf8 *name, const std::string &keyhash);
	std::string MakePlayerNameUnique(const std::string &name);
	void PrintError();
	const char* GetMasterServerUrl();
	std::string GenerateAdvertiseKey();
	void SetupDefaultGroups();

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

	int mode = NETWORK_MODE_NONE;
	int status = NETWORK_STATUS_NONE;
	bool wsa_initialized = false;
	ITcpSocket * listening_socket = nullptr;
	unsigned short listening_port = 0;
	NetworkConnection server_connection;
	SOCKET_STATUS _lastConnectStatus;
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
	std::string password;
	bool _desynchronised = false;
	INetworkServerAdvertiser * _advertiser = nullptr;
	uint32 server_connect_time = 0;
	uint8 default_group = 0;
	SDL_RWops *_chatLogStream;
	std::string _chatLogPath;

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
};

namespace Convert
{
	uint16 HostToNetwork(uint16 value);
	uint16 NetworkToHost(uint16 value);
}

#endif // __cplusplus
#else /* DISABLE_NETWORK */
#define NETWORK_STREAM_ID "Multiplayer disabled"
#endif /* DISABLE_NETWORK */

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
int network_init();
void network_close();
void network_shutdown_client();
int network_begin_client(const char *host, int port);
int network_begin_server(int port);

int network_get_mode();
int network_get_status();
void network_update();
int network_get_authstatus();
uint32 network_get_server_tick();
uint8 network_get_current_player_id();
int network_get_num_players();
const char* network_get_player_name(unsigned int index);
uint32 network_get_player_flags(unsigned int index);
int network_get_player_ping(unsigned int index);
int network_get_player_id(unsigned int index);
money32 network_get_player_money_spent(unsigned int index);
void network_add_player_money_spent(unsigned int index, money32 cost);
int network_get_player_last_action(unsigned int index, int time);
void network_set_player_last_action(unsigned int index, int command);
rct_xyz16 network_get_player_last_action_coord(unsigned int index);
void network_set_player_last_action_coord(unsigned int index, rct_xyz16 coord);
unsigned int network_get_player_commands_ran(unsigned int index);
int network_get_player_index(uint8 id);
uint8 network_get_player_group(unsigned int index);
void network_set_player_group(unsigned int index, unsigned int groupindex);
int network_get_group_index(uint8 id);
int network_get_current_player_group_index();
uint8 network_get_group_id(unsigned int index);
int network_get_num_groups();
const char* network_get_group_name(unsigned int index);
void game_command_set_player_group(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp);
void game_command_modify_groups(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);
void game_command_kick_player(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);
uint8 network_get_default_group();
int network_get_num_actions();
rct_string_id network_get_action_name_string_id(unsigned int index);
int network_can_perform_action(unsigned int groupindex, unsigned int index);
int network_can_perform_command(unsigned int groupindex, unsigned int index);

void network_send_map();
void network_send_chat(const char* text);
void network_send_gamecmd(uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp, uint8 callback);
void network_send_password(const char* password);

void network_set_password(const char* password);

void network_print_error();
void network_append_chat_log(const utf8 *text);
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
