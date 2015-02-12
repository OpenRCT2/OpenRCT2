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

typedef struct {
	uint16 size;
	uint8 data[128];
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
	NETWORK_DISCONNECTED
};

extern int gNetworkStart;
extern char gNetworkStartHost[128];
extern int gNetworkStartPort;
extern int gNetworkStatus;

int network_init();
void network_close();
int network_begin_client(const char *host, int port);
void network_end_client();
int network_begin_server(int port);
void network_end_server();

void network_update();
void network_send_packet(network_packet *packet);

void network_print_error();

#endif /* DISABLE_NETWORK */

#endif