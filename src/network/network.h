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

#ifndef DISABLE_NETWORK

#define NETWORK_DEFAULT_PORT 11753

enum {
	NETWORK_MODE_NONE,
	NETWORK_MODE_CLIENT,
	NETWORK_MODE_SERVER
};

enum {
	NETWORK_AUTH_NONE,
	NETWORK_AUTH_REQUESTED,
	NETWORK_AUTH_OK,
	NETWORK_AUTH_BADVERSION,
	NETWORK_AUTH_BADNAME,
	NETWORK_AUTH_BADPASSWORD
};

enum {
	NETWORK_PLAYER_FLAG_ISSERVER = 1 << 0,
};

#ifdef __cplusplus

#include <list>
#include <memory>
#include <vector>
#include <SDL.h>
extern "C" {
#include "../common.h"
#include "../platform/platform.h"
}

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
	static std::unique_ptr<NetworkPacket> AllocatePacket();
	static std::unique_ptr<NetworkPacket> DuplicatePacket(NetworkPacket& packet);
	uint8* GetData();
	template <typename T>
	NetworkPacket& operator<<(T value) { T swapped = ByteSwapBE(value); uint8* bytes = (uint8*)&swapped; data->insert(data->end(), bytes, bytes + sizeof(value)); return *this; }
	void Write(uint8* bytes, unsigned int size);
	void WriteString(const char* string);
	template <typename T>
	NetworkPacket& operator>>(T& value) { if (read + sizeof(value) > size) { value = 0; } else { value = ByteSwapBE(*((T*)&GetData()[read])); read += sizeof(value); } return *this; };
	const uint8* Read(unsigned int size);
	const char* ReadString();
	void Clear();

	uint16 size;
	std::shared_ptr<std::vector<uint8>> data;
	int transferred;
	int read;
};

class NetworkPlayer
{
public:
	NetworkPlayer(const char* name);
	uint8 id;
	uint8 name[32 + 1];
	uint16 ping;
	uint32 flags;
};

class NetworkConnection
{
public:
	NetworkConnection();
	int ReadPacket();
	void QueuePacket(std::unique_ptr<NetworkPacket> packet);
	void SendQueuedPackets();

	SOCKET socket;
	NetworkPacket inboundpacket;
	int authstatus;
	NetworkPlayer* player;
	uint32 ping_time;

private:
	int SendPacket(NetworkPacket& packet);
	std::list<std::unique_ptr<NetworkPacket>> outboundpackets;
};

class Network
{
public:
	Network();
	~Network();
	bool Init();
	void Close();
	bool BeginClient(const char* host, unsigned short port);
	bool BeginServer(unsigned short port);
	int GetMode();
	int GetAuthStatus();
	uint32 GetServerTick();
	void Update();
	NetworkPlayer* GetPlayerByID(int id);

	void Client_Send_AUTH(const char* gameversion, const char* name, const char* password);
	void Server_Send_MAP();
	void Client_Send_CHAT(const char* text);
	void Server_Send_CHAT(const char* text);
	void Client_Send_GAMECMD(uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp);
	void Server_Send_GAMECMD(uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp);
	void Server_Send_TICK();
	void Server_Send_PLAYERLIST();
	void Client_Send_PING();
	void Server_Send_PING();
	void Server_Send_PINGLIST();

	std::vector<std::unique_ptr<NetworkPlayer>> player_list;

private:
	bool ProcessConnection(NetworkConnection& connection);
	void ProcessPacket(NetworkConnection& connection, NetworkPacket& packet);
	void ProcessGameCommandQueue();
	void AddClient(SOCKET socket);
	void RemoveClient(std::unique_ptr<NetworkConnection>& connection);
	NetworkPlayer* AddPlayer(const char* name);
	void PrintError();

	struct GameCommand
	{
		GameCommand(uint32 t, uint32* args) { tick = t, eax = args[0], ebx = args[1], ecx = args[2], edx = args[3], esi = args[4], edi = args[5], ebp = args[6]; };
		uint32 tick;
		uint32 eax, ebx, ecx, edx, esi, edi, ebp;
		bool operator<(const GameCommand& comp) const {
			return tick < comp.tick;
		}
	};

	int mode;
	bool wsa_initialized;
	SOCKET server_socket;
	SOCKET listening_socket;
	NetworkConnection server_connection;
	uint32 last_tick_sent_time;
	uint32 last_ping_sent_time;
	uint32 server_tick;
	std::list<std::unique_ptr<NetworkConnection>> client_connection_list;
	std::multiset<GameCommand> game_command_queue;
	std::vector<uint8> chunk_buffer;
	char password[33];

private:
	std::vector<int (Network::*)(NetworkConnection& connection, NetworkPacket& packet)> client_command_handlers;
	std::vector<int (Network::*)(NetworkConnection& connection, NetworkPacket& packet)> server_command_handlers;
	int Client_Handle_AUTH(NetworkConnection& connection, NetworkPacket& packet);
	int Server_Handle_AUTH(NetworkConnection& connection, NetworkPacket& packet);
	int Client_Handle_MAP(NetworkConnection& connection, NetworkPacket& packet);
	int Client_Handle_CHAT(NetworkConnection& connection, NetworkPacket& packet);
	int Server_Handle_CHAT(NetworkConnection& connection, NetworkPacket& packet);
	int Client_Handle_GAMECMD(NetworkConnection& connection, NetworkPacket& packet);
	int Server_Handle_GAMECMD(NetworkConnection& connection, NetworkPacket& packet);
	int Client_Handle_TICK(NetworkConnection& connection, NetworkPacket& packet);
	int Client_Handle_PLAYERLIST(NetworkConnection& connection, NetworkPacket& packet);
	int Client_Handle_PING(NetworkConnection& connection, NetworkPacket& packet);
	int Server_Handle_PING(NetworkConnection& connection, NetworkPacket& packet);
	int Client_Handle_PINGLIST(NetworkConnection& connection, NetworkPacket& packet);
};

extern "C" {
#endif

int network_init();
void network_close();
int network_begin_client(const char *host, int port);
int network_begin_server(int port);

void network_update();
int network_get_mode();
int network_get_authstatus();
uint32 network_get_server_tick();
int network_get_num_players();
const char* network_get_player_name(unsigned int index);
uint32 network_get_player_flags(unsigned int index);
int network_get_player_ping(unsigned int index);

void network_send_map();
void network_send_chat(const char* text);
void network_send_gamecmd(uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp);

void network_print_error();

#ifdef __cplusplus
}
#endif

#endif /* DISABLE_NETWORK */

#endif