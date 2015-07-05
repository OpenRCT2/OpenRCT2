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

#include <winsock.h>

#include "../common.h"

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
network_packet* network_alloc_packet(int size);
void network_queue_packet(network_packet *packet);

void network_send_tick();
void network_send_map();

void network_open_chat_box();

void network_print_error();

#endif /* DISABLE_NETWORK */

#endif