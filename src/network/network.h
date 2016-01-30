/*****************************************************************************
 * Copyright (c) 2014 Ted John
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * This file is part of OpenRCT2.
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

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
	NETWORK_AUTH_NONE,
	NETWORK_AUTH_REQUESTED,
	NETWORK_AUTH_OK,
	NETWORK_AUTH_BADVERSION,
	NETWORK_AUTH_BADNAME,
	NETWORK_AUTH_BADPASSWORD,
	NETWORK_AUTH_FULL,
	NETWORK_AUTH_REQUIREPASSWORD
};

enum {
	NETWORK_STATUS_NONE,
	NETWORK_STATUS_READY,
	NETWORK_STATUS_RESOLVING,
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
#ifdef __cplusplus
}
#endif // __cplusplus

#ifndef DISABLE_NETWORK

// This define specifies which version of network stream current build uses.
// It is used for making sure only compatible builds get connected, even within
// single OpenRCT2 version.
#define NETWORK_STREAM_VERSION "0"
#define NETWORK_STREAM_ID OPENRCT2_VERSION "-" NETWORK_STREAM_VERSION

#ifdef __WINDOWS__
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#define LAST_SOCKET_ERROR() WSAGetLastError()
	#undef EWOULDBLOCK
	#define EWOULDBLOCK WSAEWOULDBLOCK
	#ifndef SHUT_RD
		#define SHUT_RD SD_RECEIVE
	#endif
	#ifndef SHUT_RDWR
		#define SHUT_RDWR SD_BOTH
	#endif
#else
	#include <errno.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <netinet/tcp.h>
	#include <sys/socket.h>
	#include <fcntl.h>
	typedef int SOCKET;
	#define SOCKET_ERROR -1
	#define INVALID_SOCKET -1
	#define LAST_SOCKET_ERROR() errno
	#define closesocket close
	#define ioctlsocket ioctl
#endif // __WINDOWS__

// Fixes issues on OS X
#if defined(_RCT2_H_) && !defined(_MSC_VER)
// use similar struct packing as MSVC for our structs
#pragma pack(1)
#endif

#ifdef __cplusplus

#include <array>
#include <list>
#include <set>
#include <memory>
#include <string>
#include <vector>
#include <SDL.h>

template <std::size_t size>
struct ByteSwapT { };
template <>
struct ByteSwapT<1> { static uint8 SwapBE(uint8 value) { return value; } };
template <>
struct ByteSwapT<2> { static uint16 SwapBE(uint16 value) { return SDL_SwapBE16(value); } };
template <>
struct ByteSwapT<4> { static uint32 SwapBE(uint32 value) { return SDL_SwapBE32(value); } };
template <typename T>
T ByteSwapBE(const T& value) { return ByteSwapT<sizeof(T)>::SwapBE(value); }

class NetworkPacket
{
public:
	NetworkPacket();
	static std::unique_ptr<NetworkPacket> Allocate();
	static std::unique_ptr<NetworkPacket> Duplicate(NetworkPacket& packet);
	uint8* GetData();
	uint32 GetCommand();
	template <typename T>
	NetworkPacket& operator<<(T value) { T swapped = ByteSwapBE(value); uint8* bytes = (uint8*)&swapped; data->insert(data->end(), bytes, bytes + sizeof(value)); return *this; }
	void Write(uint8* bytes, unsigned int size);
	void WriteString(const char* string);
	template <typename T>
	NetworkPacket& operator>>(T& value) { if (read + sizeof(value) > size) { value = 0; } else { value = ByteSwapBE(*((T*)&GetData()[read])); read += sizeof(value); } return *this; }
	const uint8* Read(unsigned int size);
	const char* ReadString();
	void Clear();
	bool CommandRequiresAuth();

	uint16 size;
	std::shared_ptr<std::vector<uint8>> data;
	int transferred;
	int read;
};

class NetworkPlayer
{
public:
	NetworkPlayer();
	void Read(NetworkPacket& packet);
	void Write(NetworkPacket& packet);
	void SetName(const char* name);
	void AddMoneySpent(money32 cost);
	uint8 id;
	uint8 name[32 + 1];
	uint16 ping;
	uint8 flags;
	uint8 group;
	money32 money_spent;
	unsigned int commands_ran;
	int last_action;
	uint32 last_action_time;
	rct_xyz16 last_action_coord;
};

class NetworkAction
{
public:
	rct_string_id name;
	std::vector<int> commands;
};

class NetworkActions
{
public:
	int FindCommand(int command);
	const std::vector<NetworkAction> actions = {
		{STR_ACTION_CHAT, {-1}},
		{STR_ACTION_TERRAFORM, {GAME_COMMAND_SET_LAND_HEIGHT, GAME_COMMAND_RAISE_LAND, GAME_COMMAND_LOWER_LAND, GAME_COMMAND_EDIT_LAND_SMOOTH, GAME_COMMAND_CHANGE_SURFACE_STYLE}},
		{STR_ACTION_SET_WATER_LEVEL, {GAME_COMMAND_SET_WATER_HEIGHT, GAME_COMMAND_RAISE_WATER, GAME_COMMAND_LOWER_WATER}},
		{STR_ACTION_TOGGLE_PAUSE, {GAME_COMMAND_TOGGLE_PAUSE}},
		{STR_ACTION_CREATE_RIDE, {GAME_COMMAND_CREATE_RIDE}},
		{STR_ACTION_REMOVE_RIDE, {GAME_COMMAND_DEMOLISH_RIDE}},
		{STR_ACTION_BUILD_RIDE, {GAME_COMMAND_PLACE_TRACK, GAME_COMMAND_REMOVE_TRACK, GAME_COMMAND_SET_MAZE_TRACK, GAME_COMMAND_PLACE_TRACK_DESIGN, GAME_COMMAND_PLACE_MAZE_DESIGN, GAME_COMMAND_PLACE_RIDE_ENTRANCE_OR_EXIT, GAME_COMMAND_REMOVE_RIDE_ENTRANCE_OR_EXIT}},
		{STR_ACTION_RIDE_PROPERTIES, {GAME_COMMAND_SET_RIDE_NAME, GAME_COMMAND_SET_RIDE_APPEARANCE, GAME_COMMAND_SET_RIDE_STATUS, GAME_COMMAND_SET_RIDE_VEHICLES, GAME_COMMAND_SET_RIDE_SETTING, GAME_COMMAND_SET_RIDE_PRICE, GAME_COMMAND_SET_BRAKES_SPEED}},
		{STR_ACTION_SCENERY, {GAME_COMMAND_REMOVE_SCENERY, GAME_COMMAND_PLACE_SCENERY, GAME_COMMAND_SET_BRAKES_SPEED, GAME_COMMAND_REMOVE_FENCE, GAME_COMMAND_PLACE_FENCE, GAME_COMMAND_REMOVE_LARGE_SCENERY, GAME_COMMAND_PLACE_LARGE_SCENERY, GAME_COMMAND_PLACE_BANNER, GAME_COMMAND_REMOVE_BANNER, GAME_COMMAND_SET_SCENERY_COLOUR, GAME_COMMAND_SET_FENCE_COLOUR, GAME_COMMAND_SET_LARGE_SCENERY_COLOUR, GAME_COMMAND_SET_BANNER_COLOUR, GAME_COMMAND_SET_BANNER_NAME, GAME_COMMAND_SET_SIGN_NAME, GAME_COMMAND_SET_BANNER_STYLE, GAME_COMMAND_SET_SIGN_STYLE}},
		{STR_ACTION_PATH, {GAME_COMMAND_PLACE_PATH, GAME_COMMAND_PLACE_PATH_FROM_TRACK, GAME_COMMAND_REMOVE_PATH}},
		{STR_ACTION_CLEAR_LANDSCAPE, {GAME_COMMAND_CLEAR_SCENERY}},
		{STR_ACTION_GUEST, {GAME_COMMAND_SET_PEEP_NAME}},
		{STR_ACTION_STAFF, {GAME_COMMAND_HIRE_NEW_STAFF_MEMBER, GAME_COMMAND_SET_STAFF_PATROL, GAME_COMMAND_FIRE_STAFF_MEMBER, GAME_COMMAND_SET_STAFF_ORDER, GAME_COMMAND_SET_STAFF_COLOUR}},
		{STR_ACTION_PARK_PROPERTIES, {GAME_COMMAND_SET_PARK_NAME, GAME_COMMAND_SET_PARK_OPEN, GAME_COMMAND_SET_PARK_ENTRANCE_FEE, GAME_COMMAND_SET_LAND_OWNERSHIP, GAME_COMMAND_BUY_LAND_RIGHTS, GAME_COMMAND_PLACE_PARK_ENTRANCE, GAME_COMMAND_REMOVE_PARK_ENTRANCE}},
		{STR_ACTION_PARK_FUNDING, {GAME_COMMAND_SET_CURRENT_LOAN, GAME_COMMAND_SET_RESEARCH_FUNDING, GAME_COMMAND_START_MARKETING_CAMPAIGN}},
		{STR_ACTION_KICK_PLAYER, {GAME_COMMAND_KICK_PLAYER}},
		{STR_ACTION_MODIFY_GROUPS, {GAME_COMMAND_MODIFY_GROUPS}},
		{STR_ACTION_SET_PLAYER_GROUP, {GAME_COMMAND_SET_PLAYER_GROUP}}
	};
};

class NetworkGroup
{
public:
	NetworkGroup();
	~NetworkGroup();
	void Read(NetworkPacket& packet);
	void Write(NetworkPacket& packet);
	void FreeNameStringId();
	void ToggleActionPermission(size_t index);
	bool CanPerformAction(size_t index);
	bool CanPerformCommand(int command);
	std::string& GetName();
	void SetName(std::string name);
	rct_string_id GetNameStringId();
	std::array<uint8, 8> actions_allowed;
	uint8 id;

private:
	std::string name;
	rct_string_id name_string_id;
};

class NetworkConnection
{
public:
	NetworkConnection();
	~NetworkConnection();
	int ReadPacket();
	void QueuePacket(std::unique_ptr<NetworkPacket> packet, bool front = false);
	void SendQueuedPackets();
	bool SetTCPNoDelay(bool on);
	bool SetNonBlocking(bool on);
	static bool SetNonBlocking(SOCKET socket, bool on);
	void ResetLastPacketTime();
	bool ReceivedPacketRecently();

	SOCKET socket;
	NetworkPacket inboundpacket;
	int authstatus;
	NetworkPlayer* player;
	uint32 ping_time;
	const char* last_disconnect_reason;

private:
	bool SendPacket(NetworkPacket& packet);
	std::list<std::unique_ptr<NetworkPacket>> outboundpackets;
	uint32 last_packet_time;
};

class NetworkAddress
{
public:
	NetworkAddress();
	void Resolve(const char* host, unsigned short port, bool nonblocking = true);
	int GetResolveStatus(void);

	std::shared_ptr<sockaddr_storage> ss;
	std::shared_ptr<int> ss_len;

	enum {
		RESOLVE_NONE,
		RESOLVE_INPROGRESS,
		RESOLVE_OK,
		RESOLVE_FAILED
	};

private:
	static int ResolveFunc(void* pointer);

	const char* host;
	unsigned short port;
	SDL_mutex* mutex;
	SDL_cond* cond;
	std::shared_ptr<int> status;
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
	const char* FormatChat(NetworkPlayer* fromplayer, const char* text);
	void SendPacketToClients(NetworkPacket& packet, bool front = false);
	bool CheckSRAND(uint32 tick, uint32 srand0);
	void KickPlayer(int playerId);
	void SetPassword(const char* password);
	void ShutdownClient();
	void AdvertiseRegister();
	void AdvertiseHeartbeat();
	NetworkGroup* AddGroup();
	void RemoveGroup(uint8 id);
	uint8 GetDefaultGroup();
	void SetDefaultGroup(uint8 id);
	void SaveGroups();
	void LoadGroups();
	void FreeStringIds();

	void Client_Send_AUTH(const char* name, const char* password);
	void Server_Send_AUTH(NetworkConnection& connection);
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

	std::vector<std::unique_ptr<NetworkPlayer>> player_list;
	std::vector<std::unique_ptr<NetworkGroup>> group_list;

private:
	bool ProcessConnection(NetworkConnection& connection);
	void ProcessPacket(NetworkConnection& connection, NetworkPacket& packet);
	void ProcessGameCommandQueue();
	void AddClient(SOCKET socket);
	void RemoveClient(std::unique_ptr<NetworkConnection>& connection);
	NetworkPlayer* AddPlayer();
	void PrintError();
	const char* GetMasterServerUrl();
	std::string GenerateAdvertiseKey();

	struct GameCommand
	{
		GameCommand(uint32 t, uint32* args, uint8 p, uint8 cb) { tick = t, eax = args[0], ebx = args[1], ecx = args[2], edx = args[3], esi = args[4], edi = args[5], ebp = args[6]; playerid = p; callback = cb; };
		uint32 tick;
		uint32 eax, ebx, ecx, edx, esi, edi, ebp;
		uint8 playerid;
		uint8 callback;
		bool operator<(const GameCommand& comp) const {
			return tick < comp.tick;
		}
	};

	int mode;
	int status;
	NetworkAddress server_address;
	bool wsa_initialized;
	SOCKET listening_socket;
	unsigned short listening_port;
	NetworkConnection server_connection;
	uint32 last_tick_sent_time;
	uint32 last_ping_sent_time;
	uint32 server_tick;
	uint32 server_srand0;
	uint32 server_srand0_tick;
	uint8 player_id;
	std::list<std::unique_ptr<NetworkConnection>> client_connection_list;
	std::multiset<GameCommand> game_command_queue;
	std::vector<uint8> chunk_buffer;
	std::string password;
	bool _desynchronised;
	uint32 server_connect_time;
	uint32 last_advertise_time;
	std::string advertise_token;
	std::string advertise_key;
	int advertise_status;
	uint32 last_heartbeat_time;
	uint8 default_group;

	void UpdateServer();
	void UpdateClient();

private:
	std::vector<void (Network::*)(NetworkConnection& connection, NetworkPacket& packet)> client_command_handlers;
	std::vector<void (Network::*)(NetworkConnection& connection, NetworkPacket& packet)> server_command_handlers;
	void Client_Handle_AUTH(NetworkConnection& connection, NetworkPacket& packet);
	void Server_Handle_AUTH(NetworkConnection& connection, NetworkPacket& packet);
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
	void Server_Handle_GAMEINFO(NetworkConnection& connection, NetworkPacket& packet);
	void Client_Handle_SHOWERROR(NetworkConnection& connection, NetworkPacket& packet);
	void Client_Handle_GROUPLIST(NetworkConnection& connection, NetworkPacket& packet);
};

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
void network_set_player_last_action_coord(int index, rct_xyz16 coord);
unsigned int network_get_player_commands_ran(unsigned int index);
int network_get_player_index(uint8 id);
uint8 network_get_player_group(unsigned int index);
void network_set_player_group(unsigned int index, unsigned int groupindex);
int network_get_group_index(uint8 id);
uint8 network_get_group_id(unsigned int index);
int network_get_num_groups();
const char* network_get_group_name(unsigned int index);
rct_string_id network_get_group_name_string_id(unsigned int index);
void game_command_set_player_group(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp);
void game_command_modify_groups(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);
void game_command_kick_player(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);
uint8 network_get_default_group();
int network_get_num_actions();
rct_string_id network_get_action_name_string_id(unsigned int index);
int network_can_perform_action(unsigned int groupindex, unsigned int index);
void network_free_string_ids();

void network_send_map();
void network_send_chat(const char* text);
void network_send_gamecmd(uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp, uint8 callback);
void network_send_password(const char* password);

void network_set_password(const char* password);

void network_print_error();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif
