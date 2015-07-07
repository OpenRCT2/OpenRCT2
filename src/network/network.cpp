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

#include <math.h>
extern "C" {
#include "../addresses.h"
#include "../game.h"
#include "../interface/window.h"
#include "../localisation/date.h"
#include "../localisation/localisation.h"
#include "../management/news_item.h"
#include "../scenario.h"
}
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
static std::vector<uint8> _chunkBuffer;
static NetworkConnection _serverConnection;
static NetworkConnection _clientConnection;

static void network_process_packet(NetworkPacket& packet);
static int network_send_packet(network_packet *packet);
static void network_send_queued_packets();

NetworkPacket::NetworkPacket()
{
	read = 0;
	size = 0;
	data = std::make_shared<std::vector<uint8>>();
}

uint8* NetworkPacket::GetData()
{
	return &(*data)[0];
}

int NetworkConnection::ReadPacket()
{
 	if (inboundpacket.read < sizeof(inboundpacket.size)) {
		// read packet size
		int readBytes = recv(socket, &((char*)&inboundpacket.size)[inboundpacket.read], sizeof(inboundpacket.size) - inboundpacket.read, 0);
		if (readBytes == SOCKET_ERROR || readBytes == 0) {
			if (WSAGetLastError() != WSAEWOULDBLOCK) {
				return NETWORK_DISCONNECTED;
			} else {
				return NETWORK_NO_DATA; 
			}
		}
		inboundpacket.read += readBytes;
		if (inboundpacket.read == sizeof(inboundpacket.size)) {
			inboundpacket.size = ntohs(inboundpacket.size);
			inboundpacket.data->resize(inboundpacket.size);
		}
	} else {
		// read packet data
		if (inboundpacket.data->capacity() > 0) {
			int readBytes = recv(socket, (char*)&inboundpacket.GetData()[inboundpacket.read - sizeof(inboundpacket.size)], sizeof(inboundpacket.size) + inboundpacket.size - inboundpacket.read, 0);
			if (readBytes == SOCKET_ERROR || readBytes == 0) {
				if (WSAGetLastError() != WSAEWOULDBLOCK) {
					return NETWORK_DISCONNECTED;
				} else {
					return NETWORK_NO_DATA;
				}
			}
			inboundpacket.read += readBytes;
		}
		if (inboundpacket.read == sizeof(inboundpacket.size) + inboundpacket.size) {
			return NETWORK_SUCCESS;
		}
	}
	return NETWORK_MORE_DATA;
}

int NetworkConnection::SendPacket(NetworkPacket& packet)
{
	if (gNetworkStatus == NETWORK_NONE) {
		return 0;
	}

	while (1) {
		if (packet.read < sizeof(packet.size)) {
			// send packet size
			uint16 size = htons(packet.size);
			int sentBytes = send(socket, &((char*)&size)[packet.read], sizeof(size) - packet.read, 0);
			if (sentBytes == SOCKET_ERROR) {
				return 0;
			}
			packet.read += sentBytes;
		} else {
			// send packet data
			int sentBytes = send(socket, (const char*)&packet.GetData()[packet.read - sizeof(packet.size)], packet.data->size(), 0);
			if (sentBytes == SOCKET_ERROR) {
				return 0;
			}
			packet.read += sentBytes;
			if (packet.read == sizeof(packet.size) + packet.data->size()) {
				return 1;
			}
		}
	}
	return 0;
}

void NetworkConnection::QueuePacket(std::unique_ptr<NetworkPacket> packet)
{
	packet->size = packet->data->size();
	outboundpackets.push_back(std::move(packet));
}

std::unique_ptr<NetworkPacket> NetworkConnection::AllocatePacket()
{
	return std::move(std::unique_ptr<NetworkPacket>(new NetworkPacket)); // can be converted to make_unique in C++14
}

void NetworkConnection::SendQueuedPackets()
{
	while (outboundpackets.size() > 0 && SendPacket(*(outboundpackets.front()).get())) {
		outboundpackets.remove(outboundpackets.front());
	}
}

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
	} else {
		printf("Connected to server!\n");
	}

	iMode = 1;
	if (ioctlsocket(_serverSocket, FIONBIO, &iMode) != NO_ERROR) {
		closesocket(_serverSocket);
		log_error("Failed to set non-blocking mode.");
	}

	_serverConnection.socket = _serverSocket;

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

	_clientConnection.socket = _clientSocket;

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
	int packetStatus;
	static uint32 lastTickUpdate = 0;

	if (gNetworkStatus == NETWORK_NONE)
		return;

	NetworkConnection& networkconnection = gNetworkStatus == NETWORK_CLIENT ? _serverConnection : _clientConnection;

	do {
		packetStatus = networkconnection.ReadPacket();
		switch(packetStatus) {
		case NETWORK_DISCONNECTED:
			network_print_error();
			if (gNetworkStatus == NETWORK_CLIENT) {
				network_end_client();
				printf("Server disconnected...\n");
				return;
			} else if (gNetworkStatus == NETWORK_SERVER) {
				network_end_server();
				printf("Client disconnected...\n");
				return;
			}
			break;
		case NETWORK_SUCCESS:
			// done reading in packet
			network_process_packet(networkconnection.inboundpacket);
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

	networkconnection.SendQueuedPackets();
}

static void network_process_packet(NetworkPacket& packet)
{
	uint32 *args;
	int command;
	rct_news_item newsItem;

	args = (uint32*)packet.GetData();
	command = args[0];
	switch (command) {
		case NETWORK_COMMAND_AUTH:{

		}break;
		case NETWORK_COMMAND_MAP:{
			uint32 size = args[1];
			uint32 offset = args[2];
			if (offset > 0x600000) {
				// too big
			} else {
				int chunksize = packet.size - 4 - 4 - 4;
				if (offset + chunksize > _chunkBuffer.size()) {
					_chunkBuffer.resize(offset + chunksize);
				}
				memcpy(&_chunkBuffer[offset], (void*)&packet.GetData()[4 + 4 + 4], chunksize);
				if (offset + chunksize == size) {
					printf("Loading new map from network...\n");
					SDL_RWops* rw = SDL_RWFromMem(&_chunkBuffer[0], size);
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
			strcpy(newsItem.text, (char*)&packet.GetData()[4]);
			news_item_add_to_queue_custom(&newsItem);
		}break;
		case NETWORK_COMMAND_GAMECMD:{
			if (gNetworkStatus == NETWORK_CLIENT)
				args[1] |= (1 << 31);

			game_do_command_p(args[1], (int*)&args[2], (int*)&args[3], (int*)&args[4], (int*)&args[5], (int*)&args[6], (int*)&args[7], (int*)&args[8]);
		}break;
		case NETWORK_COMMAND_TICK:{
			gNetworkServerTick = args[1];
		}break;
	}
	packet.read = 0;
	packet.data->clear();
}

void network_send_tick()
{
	std::unique_ptr<NetworkPacket> packet = _clientConnection.AllocatePacket();
	packet->Write((uint32)NETWORK_COMMAND_TICK);
	packet->Write((uint32)RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32));
	_clientConnection.QueuePacket(std::move(packet));
}

void network_send_map()
{
	int buffersize = 0x600000;
	std::vector<uint8> buffer(buffersize);
	SDL_RWops* rw = SDL_RWFromMem(&buffer[0], buffersize);
	scenario_save(rw, 0);
	int size = (int)SDL_RWtell(rw);
	int chunksize = 1000;
	for (int i = 0; i < size; i += chunksize) {
		int datasize = min(chunksize, size - i);
		std::unique_ptr<NetworkPacket> packet = _clientConnection.AllocatePacket();
		packet->Write((uint32)NETWORK_COMMAND_MAP);
		packet->Write((uint32)size);
		packet->Write((uint32)i);
		packet->Write(&buffer[i], datasize);
		_clientConnection.QueuePacket(std::move(packet));
	}
	SDL_RWclose(rw);
}

void network_send_gamecmd(uint32 command, uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp)
{
	NetworkConnection& networkconnection = gNetworkStatus == NETWORK_CLIENT ? _serverConnection : _clientConnection;
	std::unique_ptr<NetworkPacket> packet = networkconnection.AllocatePacket();
	packet->Write((uint32)NETWORK_COMMAND_GAMECMD);
	packet->Write((uint32)command);
	packet->Write((uint32)eax);
	packet->Write((uint32)ebx);
	packet->Write((uint32)ecx);
	packet->Write((uint32)edx);
	packet->Write((uint32)esi);
	packet->Write((uint32)edi);
	packet->Write((uint32)ebp);
	networkconnection.QueuePacket(std::move(packet));
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

	std::unique_ptr<NetworkPacket> packet = _clientConnection.AllocatePacket();
	packet->Write((uint32)NETWORK_COMMAND_CHAT);
	packet->Write((uint8*)text, strlen(text) + 1);
	_clientConnection.QueuePacket(std::move(packet));

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