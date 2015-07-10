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

#ifdef __cplusplus

#include <list>
#include <memory>
#include <vector>
#include <SDL.h>
extern "C" {
#include "../common.h"
#include "../platform/platform.h"
}

class NetworkPacket
{
public:
	NetworkPacket();
	static std::unique_ptr<NetworkPacket> AllocatePacket();
	static std::unique_ptr<NetworkPacket> DuplicatePacket(NetworkPacket& packet);
	uint8* GetData();
	template <class T>
	void Write(T value) { uint8* bytes = (uint8*)&value; data->insert(data->end(), bytes, bytes + sizeof(value)); }
	void Write(uint8* bytes, unsigned int size) { data->insert(data->end(), bytes, bytes + size); }
	void Clear();

	uint16 size;
	std::shared_ptr<std::vector<uint8>> data;
	int read;
};

class NetworkConnection
{
public:
	int ReadPacket();
	void QueuePacket(std::unique_ptr<NetworkPacket> packet);
	void SendQueuedPackets();

	SOCKET socket;
	NetworkPacket inboundpacket;

private:
	int SendPacket(NetworkPacket& packet);
	std::list<std::unique_ptr<NetworkPacket>> outboundpackets;
};

class NetworkPlayer
{
public:
	uint8 name[16 + 1];
	uint16 ping;
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
	uint32 GetServerTick();
	void Update();

	void Send_TICK();
	void Send_MAP();
	void Send_CHAT(const char* text);
	void Send_GAMECMD(uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp);

private:
	bool ProcessConnection(NetworkConnection& connection);
	void ProcessPacket(NetworkPacket& packet);
	void ProcessGameCommandQueue();
	void AddClient(SOCKET socket);
	void RemoveClient(std::unique_ptr<NetworkConnection>& connection);
	void PrintError();

	struct GameCommand
	{
		GameCommand(uint32 args[8]) { tick = args[0], eax = args[1], ebx = args[2], ecx = args[3], edx = args[4], esi = args[5], edi = args[6], ebp = args[7]; };
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
	uint32 server_tick;
	std::list<std::unique_ptr<NetworkConnection>> client_connection_list;
	std::multiset<GameCommand> game_command_queue;
	std::vector<uint8> chunk_buffer;
};

extern "C" {
#endif

int network_init();
void network_close();
int network_begin_client(const char *host, int port);
int network_begin_server(int port);

void network_update();
int network_get_mode();
uint32 network_get_server_tick();

void network_send_map();
void network_send_chat(const char* text);
void network_send_gamecmd(uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp);

void network_print_error();

#ifdef __cplusplus
}
#endif

#endif /* DISABLE_NETWORK */

#endif