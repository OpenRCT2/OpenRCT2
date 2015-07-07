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

#include "../common.h"

#ifdef __cplusplus

#include <list>
#include <memory>
#include <vector>

class NetworkPacket
{
public:
	NetworkPacket();
	uint8* GetData();
	template <class T>
	void Write(T value) { uint8* bytes = (uint8*)&value; data->insert(data->end(), bytes, bytes + sizeof(value)); }
	void Write(uint8* bytes, unsigned int size) { data->insert(data->end(), bytes, bytes + size); }

	uint16 size;
	std::shared_ptr<std::vector<uint8>> data;
	int read;
};

class NetworkConnection
{
public:
	int ReadPacket();
	void QueuePacket(std::unique_ptr<NetworkPacket> packet);
	std::unique_ptr<NetworkPacket> AllocatePacket();
	void SendQueuedPackets();

	SOCKET socket;
	NetworkPacket inboundpacket;

private:
	int SendPacket(NetworkPacket& packet);
	std::list<std::unique_ptr<NetworkPacket>> outboundpackets;
};

extern "C" {
#endif

typedef struct network_packet network_packet;

typedef struct network_packet {
	uint16 size;
	uint8* data;
	int read;
	network_packet* next;
} network_packet;

#define NETWORK_DEFAULT_PORT 11753

enum {
	NETWORK_NONE,
	NETWORK_CLIENT,
	NETWORK_SERVER
};

enum {
	NETWORK_SUCCESS,
	NETWORK_NO_DATA,
	NETWORK_MORE_DATA,
	NETWORK_DISCONNECTED
};

enum {
	NETWORK_COMMAND_AUTH,
	NETWORK_COMMAND_MAP,
	NETWORK_COMMAND_CHAT,
	NETWORK_COMMAND_GAMECMD,
	NETWORK_COMMAND_TICK
};

extern int gNetworkStart;
extern char gNetworkStartHost[128];
extern int gNetworkStartPort;
extern int gNetworkStatus;
extern uint32 gNetworkServerTick;

int network_init();
void network_close();
int network_begin_client(const char *host, int port);
void network_end_client();
int network_begin_server(int port);
void network_end_server();

void network_update();

void network_send_tick();
void network_send_map();
void network_send_chat(const char* text);
void network_send_gamecmd(uint32 command, uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp);

void network_print_error();

#ifdef __cplusplus
}
#endif

#endif /* DISABLE_NETWORK */

#endif