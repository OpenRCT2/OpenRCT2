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

#ifndef DISABLE_NETWORK

#include "network.h"

#pragma comment(lib, "Ws2_32.lib")

static int _wsaInitialised = 0;
static WSADATA _wsaData;
static SOCKET _clientSocket;

int network_init()
{
	if (!_wsaInitialised) {
		log_verbose("Initialising WSA");
		if (WSAStartup(MAKEWORD(2, 2), &_wsaData) != 0) {
			log_error("Unable to initialise winsock.");
			return 0;
		}
		_wsaInitialised = 1;
	}

	return 1;
}

void network_close()
{
	log_verbose("Closing WSA");
	WSACleanup();
	_wsaInitialised = 0;
}

int network_begin_server(int port)
{
	SOCKET listeningSocket;
	SOCKADDR_IN localAddress;

	if (!network_init())
		return 0;

	log_verbose("Begin listening for clients");
	listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listeningSocket == INVALID_SOCKET) {
		log_error("Unable to create socket.");
		return 0;
	}

	localAddress.sin_family = AF_INET;
	localAddress.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	localAddress.sin_port = htons(port);

	if (bind(listeningSocket, (SOCKADDR*)&localAddress, sizeof(SOCKADDR_IN)) != 0) {
		closesocket(listeningSocket);
		log_error("Unable to bind to socket.");
		return 0;
	}

	if (listen(listeningSocket, SOMAXCONN) != 0) {
		closesocket(listeningSocket);
		log_error("Unable to listen on socket.");
		return 0;
	}

	printf("Waiting for client...\n");
	_clientSocket = accept(listeningSocket, NULL, NULL);
	if (_clientSocket == INVALID_SOCKET) {
		closesocket(listeningSocket);
		log_error("Failed to accept client.");
		return 0;
	}

	closesocket(listeningSocket);
	printf("Connected to client!");
	return 1;
}

void network_end_server()
{
	closesocket(_clientSocket);
}

int network_send_data(uint8 *data, int length)
{
	return send(_clientSocket, data, length, 0);
}

int network_receive_data(uint8 *data, int maxLength)
{
	return recv(_clientSocket, data, maxLength, 0);
}

#endif /* DISABLE_NETWORK */