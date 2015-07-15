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
#include <set>
#include "network.h"
extern "C" {
#include "../addresses.h"
#include "../game.h"
#include "../localisation/date.h"
#include "../localisation/localisation.h"
#include "../management/news_item.h"
#include "../scenario.h"
}

#pragma comment(lib, "Ws2_32.lib")

Network gNetwork;

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
	NETWORK_COMMAND_TICK,
	NETWORK_COMMAND_PLAYER,
	NETWORK_COMMAND_MAX
};

NetworkPacket::NetworkPacket()
{
	transferred = 0;
	read = 0;
	size = 0;
	data = std::make_shared<std::vector<uint8>>();
}

std::unique_ptr<NetworkPacket> NetworkPacket::AllocatePacket()
{
	return std::move(std::unique_ptr<NetworkPacket>(new NetworkPacket)); // change to make_unique in c++14
}

std::unique_ptr<NetworkPacket> NetworkPacket::DuplicatePacket(NetworkPacket& packet)
{
	return std::move(std::unique_ptr<NetworkPacket>(new NetworkPacket(packet))); // change to make_unique in c++14
}

uint8* NetworkPacket::GetData()
{
	return &(*data)[0];
}

void NetworkPacket::Clear()
{
	transferred = 0;
	read = 0;
	data->clear();
}

int NetworkConnection::ReadPacket()
{
 	if (inboundpacket.transferred < sizeof(inboundpacket.size)) {
		// read packet size
		int readBytes = recv(socket, &((char*)&inboundpacket.size)[inboundpacket.transferred], sizeof(inboundpacket.size) - inboundpacket.transferred, 0);
		if (readBytes == SOCKET_ERROR || readBytes == 0) {
			if (WSAGetLastError() != WSAEWOULDBLOCK) {
				return NETWORK_DISCONNECTED;
			} else {
				return NETWORK_NO_DATA; 
			}
		}
		inboundpacket.transferred += readBytes;
		if (inboundpacket.transferred == sizeof(inboundpacket.size)) {
			inboundpacket.size = ntohs(inboundpacket.size);
			inboundpacket.data->resize(inboundpacket.size);
		}
	} else {
		// read packet data
		if (inboundpacket.data->capacity() > 0) {
			int readBytes = recv(socket, (char*)&inboundpacket.GetData()[inboundpacket.transferred - sizeof(inboundpacket.size)], sizeof(inboundpacket.size) + inboundpacket.size - inboundpacket.transferred, 0);
			if (readBytes == SOCKET_ERROR || readBytes == 0) {
				if (WSAGetLastError() != WSAEWOULDBLOCK) {
					return NETWORK_DISCONNECTED;
				} else {
					return NETWORK_NO_DATA;
				}
			}
			inboundpacket.transferred += readBytes;
		}
		if (inboundpacket.transferred == sizeof(inboundpacket.size) + inboundpacket.size) {
			return NETWORK_SUCCESS;
		}
	}
	return NETWORK_MORE_DATA;
}

int NetworkConnection::SendPacket(NetworkPacket& packet)
{
	while (1) {
		if (packet.transferred < sizeof(packet.size)) {
			// send packet size
			uint16 size = htons(packet.size);
			int sentBytes = send(socket, &((char*)&size)[packet.transferred], sizeof(size) - packet.transferred, 0);
			if (sentBytes == SOCKET_ERROR) {
				return 0;
			}
			packet.transferred += sentBytes;
		} else {
			// send packet data
			int sentBytes = send(socket, (const char*)&packet.GetData()[packet.transferred - sizeof(packet.size)], packet.data->size(), 0);
			if (sentBytes == SOCKET_ERROR) {
				return 0;
			}
			packet.transferred += sentBytes;
			if (packet.transferred == sizeof(packet.size) + packet.data->size()) {
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

void NetworkConnection::SendQueuedPackets()
{
	while (outboundpackets.size() > 0 && SendPacket(*(outboundpackets.front()).get())) {
		outboundpackets.remove(outboundpackets.front());
	}
}

Network::Network()
{
	wsa_initialized = false;
	mode = NETWORK_MODE_NONE;
	last_tick_sent_time = 0;
	command_handlers.resize(NETWORK_COMMAND_MAX, 0);
	command_handlers[NETWORK_COMMAND_AUTH] = &Network::CommandHandler_AUTH;
	command_handlers[NETWORK_COMMAND_MAP] = &Network::CommandHandler_MAP;
	command_handlers[NETWORK_COMMAND_CHAT] = &Network::CommandHandler_CHAT;
	command_handlers[NETWORK_COMMAND_GAMECMD] = &Network::CommandHandler_GAMECMD;
	command_handlers[NETWORK_COMMAND_TICK] = &Network::CommandHandler_TICK;
	command_handlers[NETWORK_COMMAND_PLAYER] = &Network::CommandHandler_PLAYER;
}

Network::~Network()
{
	Close();
}

bool Network::Init()
{
	if (!wsa_initialized) {
		log_verbose("Initialising WSA");
		WSADATA wsa_data;
		if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
			log_error("Unable to initialise winsock.");
			return false;
		}
		wsa_initialized = true;
	}
	return true;
}

void Network::Close()
{
	if (mode == NETWORK_MODE_CLIENT) {
		mode = NETWORK_MODE_NONE;
		closesocket(server_socket);
	} else
	if (mode == NETWORK_MODE_SERVER) {
		mode = NETWORK_MODE_NONE;
		closesocket(listening_socket);
		for(auto it = client_connection_list.begin(); it != client_connection_list.end(); it++) {
			closesocket((*it)->socket);
		}
		client_connection_list.clear();
	}

	if (wsa_initialized) {
		WSACleanup();
		wsa_initialized = false;
	}
}

bool Network::BeginClient(const char* host, unsigned short port)
{
	if (!Init())
		return false;

	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_socket == INVALID_SOCKET) {
		log_error("Unable to create socket.");
		return 0;
	}

	SOCKADDR_IN server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_addr.S_un.S_addr = inet_addr(host);
	server_address.sin_port = htons(port);

	if (connect(server_socket, (SOCKADDR*)&server_address, sizeof(SOCKADDR_IN)) != 0) {
		log_error("Unable to connect to host.");
		return false;
	} else {
		printf("Connected to server!\n");
	}

	u_long nonblocking = 1;
	if (ioctlsocket(server_socket, FIONBIO, &nonblocking) != NO_ERROR) {
		closesocket(server_socket);
		log_error("Failed to set non-blocking mode.");
	}

	server_connection.socket = server_socket;

	mode = NETWORK_MODE_CLIENT;
	return true;
}

bool Network::BeginServer(unsigned short port)
{
	if (!Init())
		return false;

	log_verbose("Begin listening for clients");
	listening_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listening_socket == INVALID_SOCKET) {
		log_error("Unable to create socket.");
		return false;
	}

	SOCKADDR_IN local_address;
	local_address.sin_family = AF_INET;
	local_address.sin_addr.S_un.S_addr = INADDR_ANY;
	local_address.sin_port = htons(port);

	if (bind(listening_socket, (SOCKADDR*)&local_address, sizeof(SOCKADDR_IN)) != 0) {
		closesocket(listening_socket);
		log_error("Unable to bind to socket.");
		return false;
	}

	if (listen(listening_socket, SOMAXCONN) != 0) {
		closesocket(listening_socket);
		log_error("Unable to listen on socket.");
		return false;
	}

	u_long nonblocking = 1;
	if (ioctlsocket(listening_socket, FIONBIO, &nonblocking) != NO_ERROR) {
		closesocket(listening_socket);
		log_error("Failed to set non-blocking mode.");
		return false;
	}

	printf("Ready for clients...\n");

	mode = NETWORK_MODE_SERVER;
	return true;
}

int Network::GetMode()
{
	return mode;
}

uint32 Network::GetServerTick()
{
	return server_tick;
}

void Network::Update()
{
	if (GetMode() == NETWORK_MODE_NONE)
		return;

	if (GetMode() == NETWORK_MODE_CLIENT) {
		if (!ProcessConnection(server_connection)) {
			Close();
		} else {
			ProcessGameCommandQueue();
		}
	} else 
	if (GetMode() == NETWORK_MODE_SERVER) {
		auto it = client_connection_list.begin();
		while(it != client_connection_list.end()) {
			if (!ProcessConnection(*(*it))) {
				RemoveClient((*it));
				it = client_connection_list.begin();
			} else {
				it++;
			}
		}
		if (SDL_GetTicks() - last_tick_sent_time >= 25) {
			last_tick_sent_time = SDL_GetTicks();
			Send_TICK();
		}
		SOCKET socket = accept(listening_socket, NULL, NULL);
		if (socket == INVALID_SOCKET) {
			if (WSAGetLastError() != WSAEWOULDBLOCK) {
				PrintError();
				log_error("Failed to accept client.");
			}
		} else {
			u_long nonblocking = 1;
			if (ioctlsocket(socket, FIONBIO, &nonblocking) != NO_ERROR) {
				closesocket(socket);
				log_error("Failed to set non-blocking mode.");
			} else {
				AddClient(socket);
			}
		}
	}
}

void Network::Send_MAP()
{
	if (GetMode() == NETWORK_MODE_SERVER) {
		int buffersize = 0x600000;
		std::vector<uint8> buffer(buffersize);
		SDL_RWops* rw = SDL_RWFromMem(&buffer[0], buffersize);
		scenario_save(rw, 0);
		int size = (int)SDL_RWtell(rw);
		int chunksize = 1000;
		for (int i = 0; i < size; i += chunksize) {
			int datasize = min(chunksize, size - i);
			std::unique_ptr<NetworkPacket> packet = NetworkPacket::AllocatePacket();
			*packet << (uint32)NETWORK_COMMAND_MAP << (uint32)size << (uint32)i;
			packet->Write(&buffer[i], datasize);
			for(auto it = client_connection_list.begin(); it != client_connection_list.end(); it++) {
				(*it)->QueuePacket(NetworkPacket::DuplicatePacket(*packet));
			}
		}
		SDL_RWclose(rw);
	}
}

void Network::Send_CHAT(const char* text)
{
	std::unique_ptr<NetworkPacket> packet = NetworkPacket::AllocatePacket();
	*packet << (uint32)NETWORK_COMMAND_CHAT;
	packet->Write((uint8*)text, strlen(text) + 1);
	if (GetMode() == NETWORK_MODE_SERVER) {
		for(auto it = client_connection_list.begin(); it != client_connection_list.end(); it++) {
			(*it)->QueuePacket(NetworkPacket::DuplicatePacket(*packet));
		}
	} else {
		server_connection.QueuePacket(std::move(packet));
	}
}

void Network::Send_GAMECMD(uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp)
{
	std::unique_ptr<NetworkPacket> packet = NetworkPacket::AllocatePacket();
	*packet << (uint32)NETWORK_COMMAND_GAMECMD << (uint32)RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) << eax << (ebx | (1 << 31)) << ecx << edx << esi << edi << ebp; 
	if (GetMode() == NETWORK_MODE_SERVER) {
		for(auto it = client_connection_list.begin(); it != client_connection_list.end(); it++) {
			(*it)->QueuePacket(NetworkPacket::DuplicatePacket(*packet));
		}
	} else {
		server_connection.QueuePacket(std::move(packet));
	}
}

void Network::Send_TICK()
{
	std::unique_ptr<NetworkPacket> packet = NetworkPacket::AllocatePacket();
	*packet << (uint32)NETWORK_COMMAND_TICK << (uint32)RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32);
	for(auto it = client_connection_list.begin(); it != client_connection_list.end(); it++) {
		(*it)->QueuePacket(NetworkPacket::DuplicatePacket(*packet));
	}
	
}

bool Network::ProcessConnection(NetworkConnection& connection)
{
	int packetStatus;
	do {
		packetStatus = connection.ReadPacket();
		switch(packetStatus) {
		case NETWORK_DISCONNECTED:
			// closed connection or network error
			PrintError();
			if (GetMode() == NETWORK_MODE_CLIENT) {
				printf("Server disconnected...\n");
				return false;
			} else
			if (GetMode() == NETWORK_MODE_SERVER) {
				printf("Client disconnected...\n");
				return false;
			}
			break;
		case NETWORK_SUCCESS:
			// done reading in packet
			ProcessPacket(connection.inboundpacket);
			break;
		case NETWORK_MORE_DATA:
			// more data required to be read
			break;
		case NETWORK_NO_DATA:
			// could not read anything from socket
			break;
		}
	} while (packetStatus == NETWORK_MORE_DATA || packetStatus == NETWORK_SUCCESS);
	connection.SendQueuedPackets();
	return true;
}

void Network::ProcessPacket(NetworkPacket& packet)
{
	uint32 command;
	packet >> command;
	if (command < NETWORK_COMMAND_MAX && command_handlers[command]) {
		(this->*command_handlers[command])(packet);
	}
	packet.Clear();
}

void Network::ProcessGameCommandQueue()
{
	while (game_command_queue.begin() != game_command_queue.end() && game_command_queue.begin()->tick == RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32)) {
		// run all the game commands at the current tick
		const GameCommand& gc = (*game_command_queue.begin());
		game_do_command(gc.eax, gc.ebx, gc.ecx, gc.edx, gc.esi, gc.edi, gc.ebp);
		game_command_queue.erase(game_command_queue.begin());
	}
}

void Network::AddClient(SOCKET socket)
{
	printf("New client connection\n");
	auto networkconnection = std::unique_ptr<NetworkConnection>(new NetworkConnection);  // change to make_unique in c++14
	networkconnection->socket = socket;
	client_connection_list.push_back(std::move(networkconnection));
}

void Network::RemoveClient(std::unique_ptr<NetworkConnection>& networkconnection)
{
	printf("Client removed\n");
	client_connection_list.remove(networkconnection);
}

void Network::PrintError()
{
	wchar_t *s = NULL;
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&s, 0, NULL);
	fprintf(stderr, "%S\n", s);
	LocalFree(s);
}

int Network::CommandHandler_AUTH(NetworkPacket& packet)
{
	return 1;
}

int Network::CommandHandler_MAP(NetworkPacket& packet)
{
	uint32 size, offset;
	packet >> size >> offset;
	if (offset > 0x600000) {
		// too big
		return 0;
	} else {
		int chunksize = packet.size - packet.read;
		if (offset + chunksize > chunk_buffer.size()) {
			chunk_buffer.resize(offset + chunksize);
		}
		memcpy(&chunk_buffer[offset], (void*)packet.Read(chunksize), chunksize);
		if (offset + chunksize == size) {
			printf("Loading new map from network...\n");
			SDL_RWops* rw = SDL_RWFromMem(&chunk_buffer[0], size);
			if (game_load_sv6(rw)) {
				game_load_init();
			}
			SDL_RWclose(rw);
		}
	}
	return 1;
}

int Network::CommandHandler_CHAT(NetworkPacket& packet)
{
	rct_news_item newsItem;
	newsItem.type = NEWS_ITEM_BLANK;
	newsItem.flags = 1;
	newsItem.assoc = 0;
	newsItem.ticks = 0;
	newsItem.month_year = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, uint16);
	newsItem.day = ((days_in_month[(newsItem.month_year & 7)] * RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_TICKS, uint16)) >> 16) + 1;
	newsItem.colour = FORMAT_TOPAZ;
	strcpy(newsItem.text, (char*)packet.Read(packet.size - packet.read));
	news_item_add_to_queue_custom(&newsItem);
	return 1;
}

int Network::CommandHandler_GAMECMD(NetworkPacket& packet)
{
	uint32 tick;
	uint32 args[7];
	packet >> tick >> args[0] >> args[1] >> args[2] >> args[3] >> args[4] >> args[5] >> args[6];
	if (GetMode() == NETWORK_MODE_SERVER) {
		Send_GAMECMD(args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
		game_do_command(args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
	} else {
		GameCommand gc = GameCommand(tick, args);
		game_command_queue.insert(gc);
	}
	return 1;
}

int Network::CommandHandler_TICK(NetworkPacket& packet)
{
	packet >> server_tick;
	return 1;
}

int Network::CommandHandler_PLAYER(NetworkPacket& packet)
{
	return 1;
}

int network_init()
{
	return gNetwork.Init();
}

void network_close()
{
	gNetwork.Close();
}

int network_begin_client(const char *host, int port)
{
	return gNetwork.BeginClient(host, port);
}

int network_begin_server(int port)
{
	return gNetwork.BeginServer(port);
}

void network_update()
{
	gNetwork.Update();
}

int network_get_mode()
{
	return gNetwork.GetMode();
}

uint32 network_get_server_tick()
{
	return gNetwork.GetServerTick();
}

void network_send_map()
{
	gNetwork.Send_MAP();
}

void network_send_chat(const char* text)
{
	gNetwork.Send_CHAT(text);
}

void network_send_gamecmd(uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp)
{
	gNetwork.Send_GAMECMD(eax, ebx, ecx, edx, esi, edi, ebp);
}

#endif /* DISABLE_NETWORK */