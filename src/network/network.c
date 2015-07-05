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
#include "../scenario.h"
#include "network.h"

#pragma comment(lib, "Ws2_32.lib")

int gNetworkStart = NETWORK_NONE;
char gNetworkStartHost[128];
int gNetworkStartPort = NETWORK_DEFAULT_PORT;
int gNetworkStatus = NETWORK_NONE;
uint32 gNetworkServerTick = 0;

static network_packet* _packetQueue = NULL;
static int _wsaInitialised = 0;
static WSADATA _wsaData;
static SOCKET _serverSocket;
static SOCKET _clientSocket;
static network_packet _inboundPacket;
static char* _chunkBuffer = NULL;

static int network_read_next_packet(network_packet *outPacket);
static void network_process_packet(network_packet *packet);
static void network_init_packet(network_packet* packet, int size);
static void network_close_packet(network_packet *packet);
static int network_send_packet(network_packet *packet);
static void network_send_queued_packets();

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

	network_init_packet(&_inboundPacket, 0);

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
	} else {
		printf("Connected to server!\n");
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
	static uint32 lastTickUpdate = 0;

	if (gNetworkStatus == NETWORK_NONE)
		return;

	socket = gNetworkStatus == NETWORK_CLIENT ? _serverSocket : _clientSocket;

	do {
		packetStatus = network_read_next_packet(&_inboundPacket);
		switch(packetStatus) {
		case NETWORK_DISCONNECTED:
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
			break;
		case NETWORK_SUCCESS:
			// done reading in packet
			network_process_packet(&_inboundPacket);
			network_close_packet(&_inboundPacket);
			network_init_packet(&_inboundPacket, 0); // initialize the packet struct to be used again
			break;
		case NETWORK_MORE_DATA:
			// more data required to be read
			break;
		case NETWORK_NO_DATA:
			break;
		}
	} while (packetStatus == NETWORK_MORE_DATA || packetStatus == NETWORK_SUCCESS);

	if (gNetworkStatus == NETWORK_SERVER) {
		if (SDL_GetTicks() - lastTickUpdate >= 100) {
			lastTickUpdate = SDL_GetTicks();
			network_send_tick();
		}
	}

	network_send_queued_packets();
}

static int network_read_next_packet(network_packet *packet)
{
	SOCKET socket;

	socket = gNetworkStatus == NETWORK_CLIENT ? _serverSocket : _clientSocket;

 	if (packet->read < sizeof(packet->size)) {
		// read packet size
		int readBytes = recv(socket, &((char*)&packet->size)[packet->read], sizeof(packet->size) - packet->read, 0);
		if (readBytes == SOCKET_ERROR || readBytes == 0) {
			if (WSAGetLastError() != WSAEWOULDBLOCK) {
				return NETWORK_DISCONNECTED;
			} else {
				return NETWORK_NO_DATA; 
			}
		}
		packet->read += readBytes;
	} else {
		// read packet data
		if (!packet->data) {
			packet->size = ntohs(packet->size);
			packet->data = malloc(packet->size);
		}
		int dataread = packet->read - sizeof(packet->size);
		int readBytes = recv(socket, &packet->data[dataread], packet->size - dataread, 0);
		if (readBytes == SOCKET_ERROR || readBytes == 0) {
			if (WSAGetLastError() != WSAEWOULDBLOCK) {
				return NETWORK_DISCONNECTED;
			} else {
				return NETWORK_NO_DATA;
			}
		}
		packet->read += readBytes;
		if (packet->read == packet->size + sizeof(packet->size)) {
			return NETWORK_SUCCESS;
		}
	}
	return NETWORK_MORE_DATA;
}

static void network_process_packet(network_packet *packet)
{
	uint32 *args;
	int command;
	rct_news_item newsItem;

	args = (uint32*)packet->data;
	command = args[0];
	switch (command) {
		case NETWORK_COMMAND_AUTH:{

		}break;
		case NETWORK_COMMAND_MAP:{
			uint32 size = args[1];
			uint32 offset = args[2];
			if (!_chunkBuffer) {
				_chunkBuffer = malloc(0x600000);
			}
			if (_chunkBuffer) {
				int chunksize = packet->size - 4 - 4 - 4;
				if (offset + chunksize <= 0x600000) {
					memcpy(&_chunkBuffer[offset], packet->data + 4 + 4 + 4, chunksize);
				}
				if (offset + chunksize == size) {
					SDL_RWops* rw = SDL_RWFromMem(_chunkBuffer, size);
					if (game_load_sv6(rw)) {
						game_load_init();
					}
					SDL_RWclose(rw);
				}
			}
		}break;
		case NETWORK_COMMAND_CHAT:{
			newsItem.type = NEWS_ITEM_BLANK;
			newsItem.flags = 1;
			newsItem.assoc = 0;
			newsItem.ticks = 0;
			newsItem.month_year = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, uint16);
			newsItem.day = ((days_in_month[(newsItem.month_year & 7)] * RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_TICKS, uint16)) >> 16) + 1;
			newsItem.colour = FORMAT_TOPAZ;
			strcpy(newsItem.text, packet->data + 4);
			news_item_add_to_queue_custom(&newsItem);
		}break;
		case NETWORK_COMMAND_GAMECMD:{
			if (gNetworkStatus == NETWORK_CLIENT)
				args[1] |= (1 << 31);

			game_do_command_p(args[1], &args[2], &args[3], &args[4], &args[5], &args[6], &args[7], &args[8]);
		}break;
		case NETWORK_COMMAND_TICK:{
			gNetworkServerTick = args[1];
		}break;
	}
}

network_packet* network_alloc_packet(int size)
{
	network_packet* packet = malloc(sizeof(network_packet));
	network_init_packet(packet, size);
	return packet;
}

void network_init_packet(network_packet* packet, int size)
{
	packet->read = 0;
	packet->size = size;
	packet->data = NULL;
	packet->next = NULL;
	if (size) {
		packet->data = malloc(size);
	}
}

void network_queue_packet(network_packet *packet)
{
	if (!_packetQueue) {
		_packetQueue = packet;
		return;
	}
	network_packet* end_packet = _packetQueue;
	while (end_packet->next) {
		end_packet = end_packet->next;
	}
	end_packet->next = packet;
}

void network_send_queued_packets()
{
	while (network_send_packet(_packetQueue)) {
		network_packet* sentPacket = _packetQueue;
		_packetQueue = _packetQueue->next;
		network_close_packet(sentPacket);
		free(sentPacket);
	}
}

void network_close_packet(network_packet *packet)
{
	free(packet->data);
}

int network_send_packet(network_packet *packet)
{
	if (!packet) {
		return 0;
	}

	SOCKET socket;

	if (gNetworkStatus == NETWORK_NONE)
		return 0;

	socket = gNetworkStatus == NETWORK_CLIENT ? _serverSocket : _clientSocket;

	while (1) {
		if (packet->read < sizeof(packet->size)) {
			// send packet size
			uint16 size = htons(packet->size);
			int sentBytes = send(socket, &((char*)&size)[packet->read], sizeof(size) - packet->read, 0);
			if (sentBytes == SOCKET_ERROR) {
				return 0;
			}
			packet->read += sentBytes;
		} else {
			// send packet data
			int datasent = packet->read - sizeof(packet->size);
			int sentBytes = send(socket, &packet->data[datasent], packet->size - datasent, 0);
			if (sentBytes == SOCKET_ERROR) {
				return 0;
			}
			packet->read += sentBytes;
			if (packet->read == packet->size + sizeof(packet->size)) {
				return 1;
			}
		}
	}
	return 0;
}

void network_send_tick()
{
	network_packet* packet = network_alloc_packet(4 + sizeof(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32)));
	*((uint32*)packet->data) = NETWORK_COMMAND_TICK;
	memcpy(packet->data + 4, (char*)&RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32), sizeof(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32)));
	network_queue_packet(packet);
}

void network_send_map()
{
	int buffersize = 0x600000;
	char* buffer = malloc(buffersize);
	if (!buffer) {
		return;
	}
	SDL_RWops* rw = SDL_RWFromMem(buffer, buffersize);
	scenario_save(rw, 0);
	int size = (int)SDL_RWtell(rw);
	int chunksize = 1000;
	for (int i = 0; i < size; i += chunksize) {
		int datasize = min(chunksize, size - i);
		network_packet* packet = network_alloc_packet(4 + 4 + 4 + datasize);
		*((uint32*)packet->data) = NETWORK_COMMAND_MAP;
		*((uint32*)packet->data + 1) = size;
		*((uint32*)packet->data + 1 + 1) = i;
		memcpy(packet->data + 4 + 4 + 4, &buffer[i], datasize);
		network_queue_packet(packet);
	}
	SDL_RWclose(rw);
	free(buffer);
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

	network_packet* packet = network_alloc_packet(4 + strlen(text) + 1);
	*((uint32*)packet->data) = NETWORK_COMMAND_CHAT;
	strcpy(packet->data + 4, text);
	network_queue_packet(packet);

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