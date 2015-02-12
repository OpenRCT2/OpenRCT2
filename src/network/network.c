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

#include "../addresses.h"
#include "../game.h"
#include "../interface/window.h"
#include "../localisation/date.h"
#include "../localisation/localisation.h"
#include "../management/news_item.h"
#include "network.h"

#pragma comment(lib, "Ws2_32.lib")

int gNetworkStart = NETWORK_NONE;
char gNetworkStartHost[128];
int gNetworkStartPort = NETWORK_DEFAULT_PORT;
int gNetworkStatus = NETWORK_NONE;

static int _wsaInitialised = 0;
static WSADATA _wsaData;
static SOCKET _serverSocket;
static SOCKET _clientSocket;

static int network_get_next_packet(network_packet *outPacket);
static void network_process_packet(network_packet *packet);

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
	if (!_wsaInitialised)
		return;

	if (gNetworkStatus == NETWORK_CLIENT)
		network_end_client();
	else if (gNetworkStatus == NETWORK_SERVER)
		network_end_server();

	log_verbose("Closing WSA");
	WSACleanup();
	_wsaInitialised = 0;
}

int network_begin_client(const char *host, int port)
{
	SOCKADDR_IN serverAddress;
	u_long iMode;

	if (!network_init())
		return 0;

	_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_serverSocket == INVALID_SOCKET) {
		log_error("Unable to create socket.");
		return 0;
	}

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.S_un.S_addr = inet_addr(host);
	serverAddress.sin_port = htons(port);

	if (connect(_serverSocket, (SOCKADDR*)&serverAddress, sizeof(SOCKADDR_IN)) != 0) {
		log_error("Unable to connect to host.");
		return 0;
	}

	iMode = 1;
	if (ioctlsocket(_serverSocket, FIONBIO, &iMode) != NO_ERROR) {
		closesocket(_serverSocket);
		log_error("Failed to set non-blocking mode.");
	}

	gNetworkStatus = NETWORK_CLIENT;
	return 1;
}

void network_end_client()
{
	gNetworkStatus = NETWORK_NONE;
	closesocket(_serverSocket);
}

int network_begin_server(int port)
{
	SOCKET listeningSocket;
	SOCKADDR_IN localAddress;
	u_long iMode;

	if (!network_init())
		return 0;

	log_verbose("Begin listening for clients");
	listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listeningSocket == INVALID_SOCKET) {
		log_error("Unable to create socket.");
		return 0;
	}

	localAddress.sin_family = AF_INET;
	localAddress.sin_addr.S_un.S_addr = INADDR_ANY;
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

	iMode = 1;
	if (ioctlsocket(_clientSocket, FIONBIO, &iMode) != NO_ERROR) {
		closesocket(_clientSocket);
		log_error("Failed to set non-blocking mode.");
	}
	
	printf("Connected to client!\n");
	gNetworkStatus = NETWORK_SERVER;
	return 1;
}

void network_end_server()
{
	gNetworkStatus = NETWORK_NONE;
	closesocket(_clientSocket);
}

void network_update()
{
	SOCKET socket;
	int packetStatus;
	network_packet packet;

	if (gNetworkStatus == NETWORK_NONE)
		return;

	socket = gNetworkStatus == NETWORK_CLIENT ? _serverSocket : _clientSocket;

	do {
		packetStatus = network_get_next_packet(&packet);
		if (packetStatus == NETWORK_DISCONNECTED) {
			network_print_error();
			if (gNetworkStatus == NETWORK_CLIENT) {
				network_end_client();
				printf("Server disconnected...\n");
				return;
			} else if (gNetworkStatus == NETWORK_SERVER) {
				network_end_server();
				printf("client disconnected...\n");
				return;
			}
		} else if (packetStatus == NETWORK_SUCCESS) {
			network_process_packet(&packet);
		}
	} while (packetStatus == NETWORK_SUCCESS);
}

static int network_get_next_packet(network_packet *outPacket)
{
	SOCKET socket;
	int readBytes;

	socket = gNetworkStatus == NETWORK_CLIENT ? _serverSocket : _clientSocket;

	readBytes = recv(socket, (char*)&outPacket->size, sizeof(outPacket->size), 0);
	if (readBytes == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
		return NETWORK_DISCONNECTED;
	
	if (readBytes != sizeof(outPacket->size))
		return NETWORK_NO_DATA;

	readBytes = recv(socket, (char*)&outPacket->data, outPacket->size, 0);
	if (readBytes != outPacket->size)
		return NETWORK_NO_DATA;

	return NETWORK_SUCCESS;
}

static void network_process_packet(network_packet *packet)
{
	uint32 *args;
	int command;
	rct_news_item newsItem;

	args = (uint32*)&packet->data;
	command = args[0];

	switch (command) {
	case 700:
		newsItem.type = NEWS_ITEM_BLANK;
		newsItem.flags = 1;
		newsItem.assoc = 0;
		newsItem.ticks = 0;
		newsItem.month_year = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, uint16);
		newsItem.day = ((days_in_month[(newsItem.month_year & 7)] * RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_TICKS, uint16)) >> 16) + 1;
		newsItem.colour = FORMAT_TOPAZ;
		strcpy(newsItem.text, packet->data + 4);
		news_item_add_to_queue_custom(&newsItem);
		break;
	default:
		if (gNetworkStatus == NETWORK_CLIENT)
			command |= (1 << 31);

		game_do_command_p(command, &args[1], &args[2], &args[3], &args[4], &args[5], &args[6], &args[7]);
		break;
	}
}

void network_send_packet(network_packet *packet)
{
	SOCKET socket;

	if (gNetworkStatus == NETWORK_NONE)
		return;

	socket = gNetworkStatus == NETWORK_CLIENT ? _serverSocket : _clientSocket;
	send(socket, (char*)packet, 2 + packet->size, 0);
}

void network_print_error()
{
	wchar_t *s = NULL;
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&s, 0, NULL);
	fprintf(stderr, "%S\n", s);
	LocalFree(s);
}

static void window_chat_host_emptysub() { }

static void window_chat_host_textinput()
{
	rct_window *w;
	short widgetIndex;
	uint8 result;
	char *text;

	window_textinput_get_registers(w, widgetIndex, result, text);

	if (!result)
		return;

	network_packet packet;
	packet.size = 4 + strlen(text) + 1;
	*((uint32*)packet.data) = 700;
	strcpy(packet.data + 4, text);

	network_send_packet(&packet);

	window_close(w);
}

static void* window_chat_host_events[] = {
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_textinput,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub,
	window_chat_host_emptysub
};

void network_open_chat_box()
{
	rct_window *w;
	w = window_create(0, 0, 0, 0, (uint32*)window_chat_host_events, WC_CHAT_HOST, 0);
	w->colours[0] = 1;
	w->colours[1] = 1;
	w->colours[2] = 0;
	w->number = 0;
	window_text_input_open(w, 0, 6000, 6001, STR_NONE, 0, 64);
}

#endif /* DISABLE_NETWORK */