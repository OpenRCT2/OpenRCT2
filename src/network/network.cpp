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

extern "C" {
#include "../openrct2.h"
#include "../platform/platform.h"
}

#include "network.h"

extern "C" {
#include "../addresses.h"
}

#ifndef DISABLE_NETWORK

#include <math.h>
#include <algorithm>
#include <set>
#include <string>
extern "C" {
#include "../config.h"
#include "../game.h"
#include "../interface/chat.h"
#include "../interface/window.h"
#include "../localisation/date.h"
#include "../localisation/localisation.h"
#include "../network/http.h"
#include "../scenario.h"
#include "../windows/error.h"
#include "../util/util.h"
}

#pragma comment(lib, "Ws2_32.lib")

Network gNetwork;

enum {
	NETWORK_READPACKET_SUCCESS,
	NETWORK_READPACKET_NO_DATA,
	NETWORK_READPACKET_MORE_DATA,
	NETWORK_READPACKET_DISCONNECTED
};

enum {
	NETWORK_COMMAND_AUTH,
	NETWORK_COMMAND_MAP,
	NETWORK_COMMAND_CHAT,
	NETWORK_COMMAND_GAMECMD,
	NETWORK_COMMAND_TICK,
	NETWORK_COMMAND_PLAYERLIST,
	NETWORK_COMMAND_PING,
	NETWORK_COMMAND_PINGLIST,
	NETWORK_COMMAND_SETDISCONNECTMSG,
	NETWORK_COMMAND_GAMEINFO,
	NETWORK_COMMAND_MAX,
	NETWORK_COMMAND_INVALID = -1
};

const char *NetworkCommandNames[] = {
	"NETWORK_COMMAND_AUTH",
	"NETWORK_COMMAND_MAP",
	"NETWORK_COMMAND_CHAT",
	"NETWORK_COMMAND_GAMECMD",
	"NETWORK_COMMAND_TICK",
	"NETWORK_COMMAND_PLAYERLIST",
	"NETWORK_COMMAND_PING",
	"NETWORK_COMMAND_PINGLIST",
};

enum {
	ADVERTISE_STATUS_DISABLED,
	ADVERTISE_STATUS_UNREGISTERED,
	ADVERTISE_STATUS_REGISTERED
};

enum {
	MASTER_SERVER_STATUS_OK = 200,
	MASTER_SERVER_STATUS_INVALID_TOKEN = 401,
	MASTER_SERVER_STATUS_SERVER_NOT_FOUND = 404,
	MASTER_SERVER_STATUS_INTERNAL_ERROR = 500
};

constexpr int MASTER_SERVER_REGISTER_TIME = 120 * 1000;	// 2 minutes
constexpr int MASTER_SERVER_HEARTBEAT_TIME = 60 * 1000;	// 1 minute

NetworkPacket::NetworkPacket()
{
	transferred = 0;
	read = 0;
	size = 0;
	data = std::make_shared<std::vector<uint8>>();
}

std::unique_ptr<NetworkPacket> NetworkPacket::Allocate()
{
	return std::unique_ptr<NetworkPacket>(new NetworkPacket); // change to make_unique in c++14
}

std::unique_ptr<NetworkPacket> NetworkPacket::Duplicate(NetworkPacket& packet)
{
	return std::unique_ptr<NetworkPacket>(new NetworkPacket(packet)); // change to make_unique in c++14
}

uint8* NetworkPacket::GetData()
{
	return &(*data)[0];
}

uint32 NetworkPacket::GetCommand()
{
	if (data->size() >= sizeof(uint32)) {
		return ByteSwapBE(*(uint32*)(&(*data)[0]));
	} else {
		return NETWORK_COMMAND_INVALID;
	}
}

void NetworkPacket::Write(uint8* bytes, unsigned int size)
{
	data->insert(data->end(), bytes, bytes + size);
}

void NetworkPacket::WriteString(const char* string)
{
	Write((uint8*)string, strlen(string) + 1);
}

const uint8* NetworkPacket::Read(unsigned int size)
{
	if (read + size > NetworkPacket::size) {
		return 0;
	} else {
		uint8* data = &GetData()[read];
		read += size;
		return data;
	}
}

const char* NetworkPacket::ReadString()
{
	char* str = (char*)&GetData()[read];
	char* strend = str;
	while (read < size && *strend != 0) {
		read++;
		strend++;
	}
	if (*strend != 0) {
		return 0;
	}
	read++;
	return str;
}

void NetworkPacket::Clear()
{
	transferred = 0;
	read = 0;
	data->clear();
}

bool NetworkPacket::CommandRequiresAuth()
{
	switch (GetCommand()) {
	case NETWORK_COMMAND_PING:
	case NETWORK_COMMAND_AUTH:
	case NETWORK_COMMAND_GAMEINFO:
		return false;
	default:
		return true;
	}
}

NetworkPlayer::NetworkPlayer(const char* name)
{
	safe_strncpy((char*)NetworkPlayer::name, name, sizeof(NetworkPlayer::name));
	NetworkPlayer::name[sizeof(NetworkPlayer::name) - 1] = 0;
	ping = 0;
	flags = 0;
}

NetworkConnection::NetworkConnection()
{
	authstatus = NETWORK_AUTH_NONE;
	player = 0;
	socket = INVALID_SOCKET;
	ResetLastPacketTime();
	last_disconnect_reason = NULL;
}

NetworkConnection::~NetworkConnection()
{
	if (socket != INVALID_SOCKET) {
		closesocket(socket);
	}
}

int NetworkConnection::ReadPacket()
{
 	if (inboundpacket.transferred < sizeof(inboundpacket.size)) {
		// read packet size
		int readBytes = recv(socket, &((char*)&inboundpacket.size)[inboundpacket.transferred], sizeof(inboundpacket.size) - inboundpacket.transferred, 0);
		if (readBytes == SOCKET_ERROR || readBytes == 0) {
			if (LAST_SOCKET_ERROR() != EWOULDBLOCK && LAST_SOCKET_ERROR() != EAGAIN) {
				return NETWORK_READPACKET_DISCONNECTED;
			} else {
				return NETWORK_READPACKET_NO_DATA;
			}
		}
		inboundpacket.transferred += readBytes;
		if (inboundpacket.transferred == sizeof(inboundpacket.size)) {
			inboundpacket.size = ntohs(inboundpacket.size);
			if(inboundpacket.size == 0){ // Can't have a size 0 packet
				return NETWORK_READPACKET_DISCONNECTED;
			}
			inboundpacket.data->resize(inboundpacket.size);
		}
	} else {
		// read packet data
		if (inboundpacket.data->capacity() > 0) {
			int readBytes = recv(socket, (char*)&inboundpacket.GetData()[inboundpacket.transferred - sizeof(inboundpacket.size)], sizeof(inboundpacket.size) + inboundpacket.size - inboundpacket.transferred, 0);
			if (readBytes == SOCKET_ERROR || readBytes == 0) {
				if (LAST_SOCKET_ERROR() != EWOULDBLOCK && LAST_SOCKET_ERROR() != EAGAIN) {
					return NETWORK_READPACKET_DISCONNECTED;
				} else {
					return NETWORK_READPACKET_NO_DATA;
				}
			}
			inboundpacket.transferred += readBytes;
		}
		if (inboundpacket.transferred == sizeof(inboundpacket.size) + inboundpacket.size) {
			last_packet_time = SDL_GetTicks();
			return NETWORK_READPACKET_SUCCESS;
		}
	}
	return NETWORK_READPACKET_MORE_DATA;
}

bool NetworkConnection::SendPacket(NetworkPacket& packet)
{
	uint16 sizen = htons(packet.size);
	std::vector<uint8> tosend;
	tosend.reserve(sizeof(sizen) + packet.size);
	tosend.insert(tosend.end(), (uint8*)&sizen, (uint8*)&sizen + sizeof(sizen));
	tosend.insert(tosend.end(), packet.data->begin(), packet.data->end());
	while (1) {
		int sentBytes = send(socket, (const char*)&tosend[packet.transferred], tosend.size() - packet.transferred, 0);
		if (sentBytes == SOCKET_ERROR) {
			return false;
		}
		packet.transferred += sentBytes;
		if (packet.transferred == tosend.size()) {
			return true;
		}
	}
	return false;
}

void NetworkConnection::QueuePacket(std::unique_ptr<NetworkPacket> packet, bool front)
{
	if (authstatus == NETWORK_AUTH_OK || !packet->CommandRequiresAuth()) {
		packet->size = (uint16)packet->data->size();
		if (front) {
			outboundpackets.push_front(std::move(packet));
		} else {
			outboundpackets.push_back(std::move(packet));
		}
	}
}

void NetworkConnection::SendQueuedPackets()
{
	while (outboundpackets.size() > 0 && SendPacket(*(outboundpackets.front()).get())) {
		outboundpackets.remove(outboundpackets.front());
	}
}

bool NetworkConnection::SetTCPNoDelay(bool on)
{
	return setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&on, sizeof(on)) == 0;
}

bool NetworkConnection::SetNonBlocking(bool on)
{
	return SetNonBlocking(socket, on);
}

bool NetworkConnection::SetNonBlocking(SOCKET socket, bool on)
{
#ifdef _WIN32
	u_long nonblocking = on;
	return ioctlsocket(socket, FIONBIO, &nonblocking) == 0;
#else
	int flags = fcntl(socket, F_GETFL, 0);
	return fcntl(socket, F_SETFL, on ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK)) == 0;
#endif
}

void NetworkConnection::ResetLastPacketTime()
{
	last_packet_time = SDL_GetTicks();
}

bool NetworkConnection::ReceivedPacketRecently()
{
	if (SDL_TICKS_PASSED(SDL_GetTicks(), last_packet_time + 7000)) {
		return false;
	}
	return true;
}

NetworkAddress::NetworkAddress()
{
	ss = std::make_shared<sockaddr_storage>();
	ss_len = std::make_shared<int>();
	status = std::make_shared<int>();
	*status = RESOLVE_NONE;
}

void NetworkAddress::Resolve(const char* host, unsigned short port, bool nonblocking)
{
	// A non-blocking hostname resolver
	*status = RESOLVE_INPROGRESS;
	mutex = SDL_CreateMutex();
	cond = SDL_CreateCond();
	NetworkAddress::host = host;
	NetworkAddress::port = port;
	SDL_LockMutex(mutex);
	SDL_Thread* thread = SDL_CreateThread(ResolveFunc, 0, this);
	// The mutex/cond is to make sure ResolveFunc doesn't ever get a dangling pointer
	SDL_CondWait(cond, mutex);
	SDL_UnlockMutex(mutex);
	SDL_DestroyCond(cond);
	SDL_DestroyMutex(mutex);
	if (!nonblocking) {
		int status;
		SDL_WaitThread(thread, &status);
	}
}

int NetworkAddress::GetResolveStatus(void)
{
	return *status;
}

int NetworkAddress::ResolveFunc(void* pointer)
{
	// Copy data for thread safety
	NetworkAddress * networkaddress = (NetworkAddress*)pointer;
	SDL_LockMutex(networkaddress->mutex);
	std::string host;
	if (networkaddress->host) host = networkaddress->host;
	std::string port = std::to_string(networkaddress->port);
	std::shared_ptr<sockaddr_storage> ss = networkaddress->ss;
	std::shared_ptr<int> ss_len = networkaddress->ss_len;
	std::shared_ptr<int> status = networkaddress->status;
	SDL_CondSignal(networkaddress->cond);
	SDL_UnlockMutex(networkaddress->mutex);

	// Perform the resolve
	addrinfo hints;
	addrinfo* res;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	if (host.length() == 0) {
		hints.ai_flags = AI_PASSIVE;
	}
	getaddrinfo(host.length() == 0 ? NULL : host.c_str(), port.c_str(), &hints, &res);
	if (res) {
		memcpy(&(*ss), res->ai_addr, res->ai_addrlen);
		*ss_len = res->ai_addrlen;
		*status = RESOLVE_OK;
		freeaddrinfo(res);
	} else {
		*status = RESOLVE_FAILED;
	}
	return 0;
}

Network::Network()
{
	wsa_initialized = false;
	mode = NETWORK_MODE_NONE;
	status = NETWORK_STATUS_NONE;
	last_tick_sent_time = 0;
	last_ping_sent_time = 0;
	last_advertise_time = 0;
	client_command_handlers.resize(NETWORK_COMMAND_MAX, 0);
	client_command_handlers[NETWORK_COMMAND_AUTH] = &Network::Client_Handle_AUTH;
	client_command_handlers[NETWORK_COMMAND_MAP] = &Network::Client_Handle_MAP;
	client_command_handlers[NETWORK_COMMAND_CHAT] = &Network::Client_Handle_CHAT;
	client_command_handlers[NETWORK_COMMAND_GAMECMD] = &Network::Client_Handle_GAMECMD;
	client_command_handlers[NETWORK_COMMAND_TICK] = &Network::Client_Handle_TICK;
	client_command_handlers[NETWORK_COMMAND_PLAYERLIST] = &Network::Client_Handle_PLAYERLIST;
	client_command_handlers[NETWORK_COMMAND_PING] = &Network::Client_Handle_PING;
	client_command_handlers[NETWORK_COMMAND_PINGLIST] = &Network::Client_Handle_PINGLIST;
	client_command_handlers[NETWORK_COMMAND_SETDISCONNECTMSG] = &Network::Client_Handle_SETDISCONNECTMSG;
	server_command_handlers.resize(NETWORK_COMMAND_MAX, 0);
	server_command_handlers[NETWORK_COMMAND_AUTH] = &Network::Server_Handle_AUTH;
	server_command_handlers[NETWORK_COMMAND_CHAT] = &Network::Server_Handle_CHAT;
	server_command_handlers[NETWORK_COMMAND_GAMECMD] = &Network::Server_Handle_GAMECMD;
	server_command_handlers[NETWORK_COMMAND_PING] = &Network::Server_Handle_PING;
	server_command_handlers[NETWORK_COMMAND_GAMEINFO] = &Network::Server_Handle_GAMEINFO;
}

Network::~Network()
{
	Close();
}

bool Network::Init()
{
#ifdef _WIN32
	if (!wsa_initialized) {
		log_verbose("Initialising WSA");
		WSADATA wsa_data;
		if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
			log_error("Unable to initialise winsock.");
			return false;
		}
		wsa_initialized = true;
	}
#endif
	status = NETWORK_STATUS_READY;
	return true;
}

void Network::Close()
{
	if (status == NETWORK_STATUS_NONE)
	{
		// Already closed. This prevents a call in ~Network() to gfx_invalidate_screen()
		// which may no longer be valid on Linux and would cause a segfault.
		return;
	}
	if (mode == NETWORK_MODE_CLIENT) {
		closesocket(server_connection.socket);
	} else
	if (mode == NETWORK_MODE_SERVER) {
		closesocket(listening_socket);
	}

	mode = NETWORK_MODE_NONE;
	status = NETWORK_STATUS_NONE;
	server_connection.authstatus = NETWORK_AUTH_NONE;
	server_connection.last_disconnect_reason = NULL;

	client_connection_list.clear();
	game_command_queue.clear();
	player_list.clear();

#ifdef _WIN32
	if (wsa_initialized) {
		WSACleanup();
		wsa_initialized = false;
	}
#endif

	gfx_invalidate_screen();
}

bool Network::BeginClient(const char* host, unsigned short port)
{
	if (GetMode() != NETWORK_MODE_NONE) {
		return false;
	}

	Close();
	if (!Init())
		return false;

	server_address.Resolve(host, port);
	status = NETWORK_STATUS_RESOLVING;

	window_network_status_open("Resolving...");

	mode = NETWORK_MODE_CLIENT;

	return true;
}

bool Network::BeginServer(unsigned short port, const char* address)
{
	Close();
	if (!Init())
		return false;

	NetworkAddress networkaddress;
	networkaddress.Resolve(address, port, false);

	log_verbose("Begin listening for clients");
	listening_socket = socket(networkaddress.ss->ss_family, SOCK_STREAM, IPPROTO_TCP);
	if (listening_socket == INVALID_SOCKET) {
		log_error("Unable to create socket.");
		return false;
	}

	// Turn off IPV6_V6ONLY so we can accept both v4 and v6 connections
	int value = 0;
	if (setsockopt(listening_socket, IPPROTO_IPV6, IPV6_V6ONLY, (const char*)&value, sizeof(value)) != 0) {
		log_error("IPV6_V6ONLY failed. %d", LAST_SOCKET_ERROR());
	}

	if (bind(listening_socket, (sockaddr*)&(*networkaddress.ss), (*networkaddress.ss_len)) != 0) {
		closesocket(listening_socket);
		log_error("Unable to bind to socket.");
		return false;
	}

	if (listen(listening_socket, SOMAXCONN) != 0) {
		closesocket(listening_socket);
		log_error("Unable to listen on socket.");
		return false;
	}

	if (!NetworkConnection::SetNonBlocking(listening_socket, true)) {
		closesocket(listening_socket);
		log_error("Failed to set non-blocking mode.");
		return false;
	}

	NetworkPlayer* player = AddPlayer(gConfigNetwork.player_name);
	player->flags |= NETWORK_PLAYER_FLAG_ISSERVER;
	player_id = player->id;

	printf("Ready for clients...\n");

	mode = NETWORK_MODE_SERVER;
	status = NETWORK_STATUS_CONNECTED;
	listening_port = port;
	advertise_status = ADVERTISE_STATUS_DISABLED;
	last_advertise_time = 0;
	last_heartbeat_time = 0;
	advertise_token = "";
	advertise_key = GenerateAdvertiseKey();

#ifndef DISABLE_HTTP
	if (gConfigNetwork.advertise) {
		advertise_status = ADVERTISE_STATUS_UNREGISTERED;
	}
#endif
	return true;
}

int Network::GetMode()
{
	return mode;
}

int Network::GetStatus()
{
	return status;
}

int Network::GetAuthStatus()
{
	if (GetMode() == NETWORK_MODE_CLIENT) {
		return server_connection.authstatus;
	} else
	if (GetMode() == NETWORK_MODE_SERVER) {
		return NETWORK_AUTH_OK;
	}
	return NETWORK_AUTH_NONE;
}

uint32 Network::GetServerTick()
{
	return server_tick;
}

uint8 Network::GetPlayerID()
{
	return player_id;
}

void Network::Update()
{
	switch (GetMode()) {
	case NETWORK_MODE_SERVER:
		UpdateServer();
		break;
	case NETWORK_MODE_CLIENT:
		UpdateClient();
		break;
	}
}

void Network::UpdateServer()
{
	auto it = client_connection_list.begin();
	while (it != client_connection_list.end()) {
		if (!ProcessConnection(*(*it))) {
			RemoveClient((*it));
			it = client_connection_list.begin();
		} else {
			it++;
		}
	}
	if (SDL_TICKS_PASSED(SDL_GetTicks(), last_tick_sent_time + 25)) {
		Server_Send_TICK();
	}
	if (SDL_TICKS_PASSED(SDL_GetTicks(), last_ping_sent_time + 3000)) {
		Server_Send_PING();
		Server_Send_PINGLIST();
	}

	switch (advertise_status) {
	case ADVERTISE_STATUS_UNREGISTERED:
		if (last_advertise_time == 0 || SDL_TICKS_PASSED(SDL_GetTicks(), last_advertise_time + MASTER_SERVER_REGISTER_TIME)) {
			AdvertiseRegister();
		}
		break;
	case ADVERTISE_STATUS_REGISTERED:
		if (SDL_TICKS_PASSED(SDL_GetTicks(), last_heartbeat_time + MASTER_SERVER_HEARTBEAT_TIME)) {
			AdvertiseHeartbeat();
		}
		break;
	}

	SOCKET socket = accept(listening_socket, NULL, NULL);
	if (socket == INVALID_SOCKET) {
		if (LAST_SOCKET_ERROR() != EWOULDBLOCK) {
			PrintError();
			log_error("Failed to accept client.");
		}
	} else {
		if (!NetworkConnection::SetNonBlocking(socket, true)) {
			closesocket(socket);
			log_error("Failed to set non-blocking mode.");
		} else {
			AddClient(socket);
		}
	}
}

void Network::UpdateClient()
{
	bool connectfailed = false;
	switch(status){
	case NETWORK_STATUS_RESOLVING:{
		if (server_address.GetResolveStatus() == NetworkAddress::RESOLVE_OK) {
			server_connection.socket = socket(server_address.ss->ss_family, SOCK_STREAM, IPPROTO_TCP);
			if (server_connection.socket == INVALID_SOCKET) {
				log_error("Unable to create socket.");
				connectfailed = true;
				break;
			}

			server_connection.SetTCPNoDelay(true);
			if (!server_connection.SetNonBlocking(true)) {
				log_error("Failed to set non-blocking mode.");
				connectfailed = true;
				break;
			}

			if (connect(server_connection.socket, (sockaddr *)&(*server_address.ss), (*server_address.ss_len)) == SOCKET_ERROR && (LAST_SOCKET_ERROR() == EINPROGRESS || LAST_SOCKET_ERROR() == EWOULDBLOCK)){
				window_network_status_open("Connecting...");
				server_connect_time = SDL_GetTicks();
				status = NETWORK_STATUS_CONNECTING;
			} else {
				log_error("connect() failed %d", LAST_SOCKET_ERROR());
				connectfailed = true;
				break;
			}
		} else {
			log_error("Could not resolve address.");
			connectfailed = true;
		}
	}break;
	case NETWORK_STATUS_CONNECTING:{
		int error = 0;
		socklen_t len = sizeof(error);
		int result = getsockopt(server_connection.socket, SOL_SOCKET, SO_ERROR, (char*)&error, &len);
		if (result != 0) {
			log_error("getsockopt failed with error %d", LAST_SOCKET_ERROR());
			break;
		}
		if (error != 0) {
			log_error("Connection failed %d", error);
			connectfailed = true;
			break;
		}
		if (SDL_TICKS_PASSED(SDL_GetTicks(), server_connect_time + 3000)) {
			log_error("Connection timed out.");
			connectfailed = true;
			break;
		}
		fd_set writeFD;
		FD_ZERO(&writeFD);
		FD_SET(server_connection.socket, &writeFD);
		timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 0;
		if (select(server_connection.socket + 1, NULL, &writeFD, NULL, &timeout) > 0) {
			error = 0;
			socklen_t len = sizeof(error);
			result = getsockopt(server_connection.socket, SOL_SOCKET, SO_ERROR, (char*)&error, &len);
			if (result != 0) {
				log_error("getsockopt failed with error %d", LAST_SOCKET_ERROR());
				break;
			}
			if (error == 0) {
				status = NETWORK_STATUS_CONNECTED;
				server_connection.ResetLastPacketTime();
				Client_Send_AUTH(gConfigNetwork.player_name, "");
				window_network_status_open("Authenticating...");
			}
		}
	}break;
	case NETWORK_STATUS_CONNECTED:
		if (!ProcessConnection(server_connection)) {
			std::string errormsg = "Disconnected";
			if (server_connection.last_disconnect_reason) {
				errormsg += ": ";
				errormsg += server_connection.last_disconnect_reason;
			}
			window_network_status_open(errormsg.c_str());
			Close();
		}
		ProcessGameCommandQueue();

		// Check synchronisation
		if (!_desynchronised && !CheckSRAND(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32), RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_SRAND_0, uint32))) {
			_desynchronised = true;
			window_network_status_open("Network desync detected");
			if (!gConfigNetwork.stay_connected) {
				Close();
			}
		}
		break;
	}

	if (connectfailed) {
		Close();
		window_network_status_close();
		window_error_open(STR_UNABLE_TO_CONNECT_TO_SERVER, STR_NONE);
	}
}

NetworkPlayer* Network::GetPlayerByID(int id) {
	auto it = std::find_if(player_list.begin(), player_list.end(), [&id](std::unique_ptr<NetworkPlayer> const& player) { return player->id == id; });
	if (it != player_list.end()) {
		return (*it).get();
	}
	return 0;
}

const char* Network::FormatChat(NetworkPlayer* fromplayer, const char* text)
{
	static char formatted[1024];
	char* lineCh = formatted;
	formatted[0] = 0;
	if (fromplayer) {
		lineCh = utf8_write_codepoint(lineCh, FORMAT_OUTLINE);
		lineCh = utf8_write_codepoint(lineCh, FORMAT_BABYBLUE);
		safe_strncpy(lineCh, (const char*)fromplayer->name, sizeof(fromplayer->name));
		strcat(lineCh, ": ");
		lineCh = strchr(lineCh, '\0');
	}
	lineCh = utf8_write_codepoint(lineCh, FORMAT_OUTLINE);
	lineCh = utf8_write_codepoint(lineCh, FORMAT_WHITE);
	safe_strncpy(lineCh, text, 800);
	return formatted;
}

void Network::SendPacketToClients(NetworkPacket& packet, bool front)
{
	for (auto it = client_connection_list.begin(); it != client_connection_list.end(); it++) {
		(*it)->QueuePacket(std::move(NetworkPacket::Duplicate(packet)), front);
	}
}

bool Network::CheckSRAND(uint32 tick, uint32 srand0)
{
	if (server_srand0_tick == 0)
		return true;

	if (tick > server_srand0_tick) {
		server_srand0_tick = 0;
		return true;
	}

	if (tick == server_srand0_tick) {
		server_srand0_tick = 0;
		if (srand0 != server_srand0) {
			return false;
		}
	}
	return true;
}

void Network::KickPlayer(int playerId)
{
	NetworkPlayer *player = GetPlayerByID(playerId);
	for(auto it = client_connection_list.begin(); it != client_connection_list.end(); it++) {
		if ((*it)->player->id == playerId) {
			// Disconnect the client gracefully
			(*it)->last_disconnect_reason = "Kicked";
			Server_Send_SETDISCONNECTMSG(*(*it), "Get out of the server!");
			shutdown((*it)->socket, SHUT_RD);
			(*it)->SendQueuedPackets();
			break;
		}
	}
}

void Network::SetPassword(const char* password)
{
	Network::password = password;
}

void Network::ShutdownClient()
{
	if (GetMode() == NETWORK_MODE_CLIENT) {
		shutdown(server_connection.socket, SHUT_RDWR);
	}
}

std::string Network::GenerateAdvertiseKey()
{
	// Generate a string of 16 random hex characters (64-integer key as a hex formatted string)
	static char hexChars[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
	char key[17];
	for (int i = 0; i < 16; i++) {
		int hexCharIndex = rand() % countof(hexChars);
		key[i] = hexChars[hexCharIndex];
	}
	key[countof(key) - 1] = 0;

	return key;
}

const char *Network::GetMasterServerUrl()
{
	if (str_is_null_or_empty(gConfigNetwork.master_server_url)) {
		return OPENRCT2_MASTER_SERVER_URL;
	} else {
		return gConfigNetwork.master_server_url;
	}
}

void Network::AdvertiseRegister()
{
#ifndef DISABLE_HTTP
	last_advertise_time = SDL_GetTicks();

	// Send the registration request
	http_json_request request;
	request.url = GetMasterServerUrl();
	request.method = HTTP_METHOD_POST;

	json_t *body = json_object();
	json_object_set(body, "key", json_string(advertise_key.c_str()));
	json_object_set(body, "port", json_integer(listening_port));
	request.body = body;

	http_request_json_async(&request, [](http_json_response *response) -> void {
		if (response == NULL) {
			log_warning("Unable to connect to master server");
			return;
		}

		json_t *jsonStatus = json_object_get(response->root, "status");
		if (json_is_integer(jsonStatus)) {
			int status = (int)json_integer_value(jsonStatus);
			if (status == MASTER_SERVER_STATUS_OK) {
				json_t *jsonToken = json_object_get(response->root, "token");
				if (json_is_string(jsonToken)) {
					gNetwork.advertise_token = json_string_value(jsonToken);
					gNetwork.advertise_status = ADVERTISE_STATUS_REGISTERED;
				}
			} else {
				const char *message = "Invalid response from server";
				json_t *jsonMessage = json_object_get(response->root, "message");
				if (json_is_string(jsonMessage)) {
					message = json_string_value(jsonMessage);
				}
				log_warning("Unable to advertise: %s", message);
			}
		}
		http_request_json_dispose(response);
	});

	json_decref(body);
#endif
}

void Network::AdvertiseHeartbeat()
{
#ifndef DISABLE_HTTP
	// Send the heartbeat request
	http_json_request request;
	request.url = GetMasterServerUrl();
	request.method = HTTP_METHOD_PUT;

	json_t *body = json_object();
	json_object_set(body, "token", json_string(advertise_token.c_str()));
	json_object_set(body, "players", json_integer(network_get_num_players()));

	json_t *gameInfo = json_object();
	json_object_set(gameInfo, "mapSize", json_integer(RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE, uint8) - 2));
	json_object_set(gameInfo, "day", json_integer(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_TICKS, uint16)));
	json_object_set(gameInfo, "month", json_integer(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, uint16)));
	json_object_set(gameInfo, "guests", json_integer(RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_IN_PARK, uint16)));
	json_object_set(gameInfo, "parkValue", json_integer(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PARK_VALUE, money32)));
	if (!(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY)) {
		money32 cash = DECRYPT_MONEY(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONEY_ENCRYPTED, money32));
		json_object_set(gameInfo, "cash", json_integer(cash));
	}

	json_object_set(body, "gameInfo", gameInfo);
	request.body = body;

	gNetwork.last_heartbeat_time = SDL_GetTicks();
	http_request_json_async(&request, [](http_json_response *response) -> void {
		if (response == NULL) {
			log_warning("Unable to connect to master server");
			return;
		}

		json_t *jsonStatus = json_object_get(response->root, "status");
		if (json_is_integer(jsonStatus)) {
			int status = (int)json_integer_value(jsonStatus);
			if (status == MASTER_SERVER_STATUS_OK) {
				// Master server has successfully updated our server status
			} else if (status == MASTER_SERVER_STATUS_INVALID_TOKEN) {
				gNetwork.advertise_status = ADVERTISE_STATUS_UNREGISTERED;
				log_warning("Master server heartbeat failed: Invalid Token");
			}
		}
		http_request_json_dispose(response);
	});

	json_decref(body);
#endif
}

void Network::Client_Send_AUTH(const char* name, const char* password)
{
	std::unique_ptr<NetworkPacket> packet = std::move(NetworkPacket::Allocate());
	*packet << (uint32)NETWORK_COMMAND_AUTH;
	packet->WriteString(OPENRCT2_VERSION);
	packet->WriteString(name);
	packet->WriteString(password);
	server_connection.authstatus = NETWORK_AUTH_REQUESTED;
	server_connection.QueuePacket(std::move(packet));
}

void Network::Server_Send_AUTH(NetworkConnection& connection)
{
	uint8 new_playerid = 0;
	if (connection.player) {
		new_playerid = connection.player->id;
	}
	std::unique_ptr<NetworkPacket> packet = std::move(NetworkPacket::Allocate());
	*packet << (uint32)NETWORK_COMMAND_AUTH << (uint32)connection.authstatus << (uint8)new_playerid;
	connection.QueuePacket(std::move(packet));
	if (connection.authstatus != NETWORK_AUTH_OK && connection.authstatus != NETWORK_AUTH_REQUIREPASSWORD) {
		shutdown(connection.socket, SHUT_RD);
		connection.SendQueuedPackets();
	}
}

void Network::Server_Send_MAP(NetworkConnection* connection)
{
	int buffersize = 0x600000;
	std::vector<uint8> buffer(buffersize);
	SDL_RWops* rw = SDL_RWFromMem(&buffer[0], buffersize);
	scenario_save_network(rw);
	int size = (int)SDL_RWtell(rw);
	int chunksize = 1000;
	for (int i = 0; i < size; i += chunksize) {
		int datasize = (std::min)(chunksize, size - i);
		std::unique_ptr<NetworkPacket> packet = std::move(NetworkPacket::Allocate());
		*packet << (uint32)NETWORK_COMMAND_MAP << (uint32)size << (uint32)i;
		packet->Write(&buffer[i], datasize);
		if (connection) {
			connection->QueuePacket(std::move(packet));
		} else {
			SendPacketToClients(*packet);
		}
	}
	SDL_RWclose(rw);
}

void Network::Client_Send_CHAT(const char* text)
{
	std::unique_ptr<NetworkPacket> packet = std::move(NetworkPacket::Allocate());
	*packet << (uint32)NETWORK_COMMAND_CHAT;
	packet->WriteString(text);
	server_connection.QueuePacket(std::move(packet));
}

void Network::Server_Send_CHAT(const char* text)
{
	std::unique_ptr<NetworkPacket> packet = std::move(NetworkPacket::Allocate());
	*packet << (uint32)NETWORK_COMMAND_CHAT;
	packet->WriteString(text);
	SendPacketToClients(*packet);
}

void Network::Client_Send_GAMECMD(uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp, uint8 callback)
{
	std::unique_ptr<NetworkPacket> packet = std::move(NetworkPacket::Allocate());
	*packet << (uint32)NETWORK_COMMAND_GAMECMD << (uint32)RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) << eax << (ebx | GAME_COMMAND_FLAG_NETWORKED) << ecx << edx << esi << edi << ebp << callback;
	server_connection.QueuePacket(std::move(packet));
}

void Network::Server_Send_GAMECMD(uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp, uint8 playerid, uint8 callback)
{
	std::unique_ptr<NetworkPacket> packet = std::move(NetworkPacket::Allocate());
	*packet << (uint32)NETWORK_COMMAND_GAMECMD << (uint32)RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) << eax << (ebx | GAME_COMMAND_FLAG_NETWORKED) << ecx << edx << esi << edi << ebp << playerid << callback;
	SendPacketToClients(*packet);
}

void Network::Server_Send_TICK()
{
	last_tick_sent_time = SDL_GetTicks();
	std::unique_ptr<NetworkPacket> packet = std::move(NetworkPacket::Allocate());
	*packet << (uint32)NETWORK_COMMAND_TICK << (uint32)RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) << (uint32)RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_SRAND_0, uint32);
	SendPacketToClients(*packet);
}

void Network::Server_Send_PLAYERLIST()
{
	std::unique_ptr<NetworkPacket> packet = std::move(NetworkPacket::Allocate());
	*packet << (uint32)NETWORK_COMMAND_PLAYERLIST << (uint32)player_list.size();
	for (unsigned int i = 0; i < player_list.size(); i++) {
		packet->WriteString((const char*)player_list[i]->name);
		*packet << player_list[i]->id << player_list[i]->flags;
	}
	SendPacketToClients(*packet);
}

void Network::Client_Send_PING()
{
	std::unique_ptr<NetworkPacket> packet = std::move(NetworkPacket::Allocate());
	*packet << (uint32)NETWORK_COMMAND_PING;
	server_connection.QueuePacket(std::move(packet));
}

void Network::Server_Send_PING()
{
	last_ping_sent_time = SDL_GetTicks();
	std::unique_ptr<NetworkPacket> packet = std::move(NetworkPacket::Allocate());
	*packet << (uint32)NETWORK_COMMAND_PING;
	for (auto it = client_connection_list.begin(); it != client_connection_list.end(); it++) {
		(*it)->ping_time = SDL_GetTicks();
	}
	SendPacketToClients(*packet, true);
}

void Network::Server_Send_PINGLIST()
{
	std::unique_ptr<NetworkPacket> packet = std::move(NetworkPacket::Allocate());
	*packet << (uint32)NETWORK_COMMAND_PINGLIST << (uint32)player_list.size();
	for (unsigned int i = 0; i < player_list.size(); i++) {
		*packet << player_list[i]->id << player_list[i]->ping;
	}
	SendPacketToClients(*packet);
}

void Network::Server_Send_SETDISCONNECTMSG(NetworkConnection& connection, const char* msg)
{
	std::unique_ptr<NetworkPacket> packet = std::move(NetworkPacket::Allocate());
	*packet << (uint32)NETWORK_COMMAND_SETDISCONNECTMSG;
	packet->WriteString(msg);
	connection.QueuePacket(std::move(packet));
}

void Network::Server_Send_GAMEINFO(NetworkConnection& connection)
{
	std::unique_ptr<NetworkPacket> packet = std::move(NetworkPacket::Allocate());
	*packet << (uint32)NETWORK_COMMAND_GAMEINFO;
#ifndef DISABLE_HTTP
	json_t* obj = json_object();
	json_object_set(obj, "name", json_string(gConfigNetwork.server_name));
	json_object_set(obj, "requiresPassword", json_boolean(password.size() > 0));
	json_object_set(obj, "version", json_string(OPENRCT2_VERSION));
	json_object_set(obj, "players", json_integer(player_list.size()));
	json_object_set(obj, "maxPlayers", json_integer(gConfigNetwork.maxplayers));
	json_object_set(obj, "description", json_string(gConfigNetwork.server_description));
	json_object_set(obj, "dedicated", json_boolean(gOpenRCT2Headless));

	// Provider details
	json_t* jsonProvider = json_object();
	json_object_set(jsonProvider, "name", json_string(gConfigNetwork.provider_name));
	json_object_set(jsonProvider, "email", json_string(gConfigNetwork.provider_email));
	json_object_set(jsonProvider, "website", json_string(gConfigNetwork.provider_website));
	json_object_set(obj, "provider", jsonProvider);

	packet->WriteString(json_dumps(obj, 0));
	json_decref(obj);
#endif
	connection.QueuePacket(std::move(packet));
}

bool Network::ProcessConnection(NetworkConnection& connection)
{
	int packetStatus;
	do {
		packetStatus = connection.ReadPacket();
		switch(packetStatus) {
		case NETWORK_READPACKET_DISCONNECTED:
			// closed connection or network error
			PrintError();
			if (!connection.last_disconnect_reason) {
				connection.last_disconnect_reason = "Connection Closed";
			}
			return false;
			break;
		case NETWORK_READPACKET_SUCCESS:
			// done reading in packet
			ProcessPacket(connection, connection.inboundpacket);
			break;
		case NETWORK_READPACKET_MORE_DATA:
			// more data required to be read
			break;
		case NETWORK_READPACKET_NO_DATA:
			// could not read anything from socket
			break;
		}
	} while (packetStatus == NETWORK_READPACKET_MORE_DATA || packetStatus == NETWORK_READPACKET_SUCCESS);
	connection.SendQueuedPackets();
	if (!connection.ReceivedPacketRecently()) {
		if (!connection.last_disconnect_reason) {
			connection.last_disconnect_reason = "No Data";
		}
		return false;
	}
	return true;
}

void Network::ProcessPacket(NetworkConnection& connection, NetworkPacket& packet)
{
	uint32 command;
	packet >> command;
	if (command < NETWORK_COMMAND_MAX) {
		// printf("RECV %s\n", NetworkCommandNames[command]);
		switch (gNetwork.GetMode()) {
		case NETWORK_MODE_SERVER:
			if (server_command_handlers[command]) {
				if (connection.authstatus == NETWORK_AUTH_OK || !packet.CommandRequiresAuth()) {
					(this->*server_command_handlers[command])(connection, packet);
				}
			}
			break;
		case NETWORK_MODE_CLIENT:
			if (client_command_handlers[command]) {
				(this->*client_command_handlers[command])(connection, packet);
			}
			break;
		}
	}
	packet.Clear();
}

void Network::ProcessGameCommandQueue()
{
	while (game_command_queue.begin() != game_command_queue.end() && game_command_queue.begin()->tick == RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32)) {
		// run all the game commands at the current tick
		const GameCommand& gc = (*game_command_queue.begin());
		if (GetPlayerID() == gc.playerid) {
			game_command_callback = game_command_callback_get_callback(gc.callback);
		}
		game_do_command_p(gc.esi, (int*)&gc.eax, (int*)&gc.ebx, (int*)&gc.ecx, (int*)&gc.edx, (int*)&gc.esi, (int*)&gc.edi, (int*)&gc.ebp);
		game_command_queue.erase(game_command_queue.begin());
	}
}

void Network::AddClient(SOCKET socket)
{
	auto connection = std::unique_ptr<NetworkConnection>(new NetworkConnection);  // change to make_unique in c++14
	connection->socket = socket;
	connection->SetTCPNoDelay(true);
	client_connection_list.push_back(std::move(connection));
}

void Network::RemoveClient(std::unique_ptr<NetworkConnection>& connection)
{
	NetworkPlayer* connection_player = connection->player;
	if (connection_player) {
		char text[256];
		char* lineCh = text;
		lineCh = utf8_write_codepoint(lineCh, FORMAT_OUTLINE);
		lineCh = utf8_write_codepoint(lineCh, FORMAT_RED);
		char reasonstr[100];
		reasonstr[0] = 0;
		if (connection->last_disconnect_reason && strlen(connection->last_disconnect_reason) < sizeof(reasonstr)) {
			sprintf(reasonstr, " (%s)", connection->last_disconnect_reason);
		}
		sprintf(lineCh, "%s has disconnected%s", connection_player->name, reasonstr);
		chat_history_add(text);
		gNetwork.Server_Send_CHAT(text);
	}
	player_list.erase(std::remove_if(player_list.begin(), player_list.end(), [connection_player](std::unique_ptr<NetworkPlayer>& player){ return player.get() == connection_player; }), player_list.end());
	client_connection_list.remove(connection);
	Server_Send_PLAYERLIST();
}

NetworkPlayer* Network::AddPlayer(const char* name)
{
	NetworkPlayer* addedplayer = 0;
	int newid = -1;
	if (GetMode() == NETWORK_MODE_SERVER) {
		// Find first unused player id
		for (int id = 0; id < 255; id++) {
			if (std::find_if(player_list.begin(), player_list.end(), [&id](std::unique_ptr<NetworkPlayer> const& player) { return player->id == id; }) == player_list.end()) {
				newid = id;
				break;
			}
		}
	} else {
		newid = 0;
	}
	if (newid != -1) {
		std::unique_ptr<NetworkPlayer> player(new NetworkPlayer(name)); // change to make_unique in c++14
		player->id = newid;
		addedplayer = player.get();
		player_list.push_back(std::move(player));
		if (GetMode() == NETWORK_MODE_SERVER) {
			Server_Send_PLAYERLIST();
		}
	}
	return addedplayer;
}

void Network::PrintError()
{
#ifdef _WIN32
	wchar_t *s = NULL;
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, LAST_SOCKET_ERROR(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&s, 0, NULL);
	fprintf(stderr, "%S\n", s);
	LocalFree(s);
#else
	char *s = strerror(LAST_SOCKET_ERROR());
	fprintf(stderr, "%s\n", s);
#endif

}

int Network::Client_Handle_AUTH(NetworkConnection& connection, NetworkPacket& packet)
{
	packet >> (uint32&)connection.authstatus >> (uint8&)player_id;
	switch(connection.authstatus) {
	case NETWORK_AUTH_BADNAME:
		connection.last_disconnect_reason = "Bad Player Name";
		shutdown(connection.socket, SHUT_RDWR);
		break;
	case NETWORK_AUTH_BADVERSION:
		connection.last_disconnect_reason = "Incorrect Software Version";
		shutdown(connection.socket, SHUT_RDWR);
		break;
	case NETWORK_AUTH_BADPASSWORD:
		connection.last_disconnect_reason = "Bad Password";
		shutdown(connection.socket, SHUT_RDWR);
		break;
	case NETWORK_AUTH_FULL:
		connection.last_disconnect_reason = "Server Full";
		shutdown(connection.socket, SHUT_RDWR);
		break;
	case NETWORK_AUTH_REQUIREPASSWORD:
		window_network_status_open_password();
		break;
	}
	return 1;
}

int Network::Server_Handle_AUTH(NetworkConnection& connection, NetworkPacket& packet)
{
	if (connection.authstatus != NETWORK_AUTH_OK) {
		const char* gameversion = packet.ReadString();
		const char* name = packet.ReadString();
		const char* password = packet.ReadString();
		if (!gameversion || strcmp(gameversion, OPENRCT2_VERSION) != 0) {
			connection.authstatus = NETWORK_AUTH_BADVERSION;
		} else
		if (!name) {
			connection.authstatus = NETWORK_AUTH_BADNAME;
		} else
		if ((!password || strlen(password) == 0) && Network::password.size() > 0) {
			connection.authstatus = NETWORK_AUTH_REQUIREPASSWORD;
		} else
		if (password && Network::password != password) {
			connection.authstatus = NETWORK_AUTH_BADPASSWORD;
		} else
		if (gConfigNetwork.maxplayers <= player_list.size()) {
			connection.authstatus = NETWORK_AUTH_FULL;
		} else {
			connection.authstatus = NETWORK_AUTH_OK;
			NetworkPlayer* player = AddPlayer(name);
			connection.player = player;
			if (player) {
				char text[256];
				char* lineCh = text;
				lineCh = utf8_write_codepoint(lineCh, FORMAT_OUTLINE);
				lineCh = utf8_write_codepoint(lineCh, FORMAT_GREEN);
				sprintf(lineCh, "%s has joined the game", player->name);
				chat_history_add(text);
				gNetwork.Server_Send_CHAT(text);
				Server_Send_MAP(&connection);
			}
		}
		Server_Send_AUTH(connection);
	}
	return 1;
}

int Network::Client_Handle_MAP(NetworkConnection& connection, NetworkPacket& packet)
{
	uint32 size, offset;
	packet >> size >> offset;
	if (offset > 0x600000) {
		// too big
		return 0;
	} else {
		int chunksize = packet.size - packet.read;
		if (chunksize <= 0) {
			return 0;
		}
		if (offset + chunksize > chunk_buffer.size()) {
			chunk_buffer.resize(offset + chunksize);
		}
		char status[256];
		sprintf(status, "Downloading map ... (%lu / %lu)", (offset + chunksize) / 1000, size / 1000);
		window_network_status_open(status);
		memcpy(&chunk_buffer[offset], (void*)packet.Read(chunksize), chunksize);
		if (offset + chunksize == size) {
			window_network_status_close();

			SDL_RWops* rw = SDL_RWFromMem(&chunk_buffer[0], size);
			if (game_load_network(rw)) {
				game_load_init();
				game_command_queue.clear();
				server_tick = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32);
				server_srand0_tick = 0;
				// window_network_status_open("Loaded new map from network");
				_desynchronised = false;
			}
			SDL_RWclose(rw);
		}
	}
	return 1;
}

int Network::Client_Handle_CHAT(NetworkConnection& connection, NetworkPacket& packet)
{
	const char* text = packet.ReadString();
	if (text) {
		chat_history_add(text);
	}
	return 1;
}

int Network::Server_Handle_CHAT(NetworkConnection& connection, NetworkPacket& packet)
{
	const char* text = packet.ReadString();
	if (text) {
		const char* formatted = FormatChat(connection.player, text);
		chat_history_add(formatted);
		Server_Send_CHAT(formatted);
	}
	return 1;
}

int Network::Client_Handle_GAMECMD(NetworkConnection& connection, NetworkPacket& packet)
{
	uint32 tick;
	uint32 args[7];
	uint8 playerid;
	uint8 callback;
	packet >> tick >> args[0] >> args[1] >> args[2] >> args[3] >> args[4] >> args[5] >> args[6] >> playerid >> callback;

	GameCommand gc = GameCommand(tick, args, playerid, callback);
	game_command_queue.insert(gc);
	return 1;
}

int Network::Server_Handle_GAMECMD(NetworkConnection& connection, NetworkPacket& packet)
{
	uint32 tick;
	uint32 args[7];
	uint8 playerid;
	uint8 callback;
	if (connection.player) {
		playerid = connection.player->id;
	}
	packet >> tick >> args[0] >> args[1] >> args[2] >> args[3] >> args[4] >> args[5] >> args[6] >> callback;

	int commandCommand = args[4];

	// Don't let clients send pause or quit
	if (commandCommand != GAME_COMMAND_TOGGLE_PAUSE &&
		commandCommand != GAME_COMMAND_LOAD_OR_QUIT
	) {
		Server_Send_GAMECMD(args[0], args[1], args[2], args[3], args[4], args[5], args[6], playerid, callback);
		game_do_command(args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
	}

	return 1;
}

int Network::Client_Handle_TICK(NetworkConnection& connection, NetworkPacket& packet)
{
	uint32 srand0;
	packet >> server_tick >> srand0;
	if (server_srand0_tick == 0) {
		server_srand0 = srand0;
		server_srand0_tick = server_tick;
	}
	return 1;
}

int Network::Client_Handle_PLAYERLIST(NetworkConnection& connection, NetworkPacket& packet)
{
	player_list.clear();
	uint32 size;
	packet >> size;
	for (unsigned int i = 0; i < size; i++) {
		const char* name = packet.ReadString();
		NetworkPlayer* player = AddPlayer(name);
		if (player) {
			packet >> player->id >> player->flags;
			if (player->flags & NETWORK_PLAYER_FLAG_ISSERVER) {
				server_connection.player = player;
			}
		}
	}
	return 1;
}

int Network::Client_Handle_PING(NetworkConnection& connection, NetworkPacket& packet)
{
	Client_Send_PING();
	return 1;
}

int Network::Server_Handle_PING(NetworkConnection& connection, NetworkPacket& packet)
{
	int ping = SDL_GetTicks() - connection.ping_time;
	if (ping < 0) {
		ping = 0;
	}
	if (connection.player) {
		connection.player->ping = ping;
	}
	return 1;
}

int Network::Client_Handle_PINGLIST(NetworkConnection& connection, NetworkPacket& packet)
{
	uint32 size;
	packet >> size;
	for (unsigned int i = 0; i < size; i++) {
		uint8 id;
		uint16 ping;
		packet >> id >> ping;
		NetworkPlayer* player = GetPlayerByID(id);
		if (player) {
			player->ping = ping;
		}
	}
	return 1;
}

int Network::Client_Handle_SETDISCONNECTMSG(NetworkConnection& connection, NetworkPacket& packet)
{
	static std::string msg;
	const char* disconnectmsg = packet.ReadString();
	if (disconnectmsg) {
		msg = disconnectmsg;
		connection.last_disconnect_reason = msg.c_str();
	}
	return 1;
}

int Network::Server_Handle_GAMEINFO(NetworkConnection& connection, NetworkPacket& packet)
{
	Server_Send_GAMEINFO(connection);
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

void network_shutdown_client()
{
	gNetwork.ShutdownClient();
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

int network_get_status()
{
	return gNetwork.GetStatus();
}

int network_get_authstatus()
{
	return gNetwork.GetAuthStatus();
}

uint32 network_get_server_tick()
{
	return gNetwork.GetServerTick();
}

uint8 network_get_current_player_id()
{
	return gNetwork.GetPlayerID();
}

int network_get_num_players()
{
	return gNetwork.player_list.size();
}

const char* network_get_player_name(unsigned int index)
{
	return (const char*)gNetwork.player_list[index]->name;
}

uint32 network_get_player_flags(unsigned int index)
{
	return gNetwork.player_list[index]->flags;
}

int network_get_player_ping(unsigned int index)
{
	return gNetwork.player_list[index]->ping;
}

int network_get_player_id(unsigned int index)
{
	return gNetwork.player_list[index]->id;
}

void network_send_map()
{
	gNetwork.Server_Send_MAP();
}

void network_send_chat(const char* text)
{
	if (gNetwork.GetMode() == NETWORK_MODE_CLIENT) {
		gNetwork.Client_Send_CHAT(text);
	} else
	if (gNetwork.GetMode() == NETWORK_MODE_SERVER) {
		NetworkPlayer* player = gNetwork.GetPlayerByID(gNetwork.GetPlayerID());
		const char* formatted = gNetwork.FormatChat(player, text);
		chat_history_add(formatted);
		gNetwork.Server_Send_CHAT(formatted);
	}
}

void network_send_gamecmd(uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp, uint8 callback)
{
	switch (gNetwork.GetMode()) {
	case NETWORK_MODE_SERVER:
		gNetwork.Server_Send_GAMECMD(eax, ebx, ecx, edx, esi, edi, ebp, gNetwork.GetPlayerID(), callback);
		break;
	case NETWORK_MODE_CLIENT:
		gNetwork.Client_Send_GAMECMD(eax, ebx, ecx, edx, esi, edi, ebp, callback);
		break;
	}
}

void network_send_password(const char* password)
{
	gNetwork.Client_Send_AUTH(gConfigNetwork.player_name, password);
}

void network_kick_player(int playerId)
{
	gNetwork.KickPlayer(playerId);
}

void network_set_password(const char* password)
{
	gNetwork.SetPassword(password);
}

#else
int network_get_mode() { return NETWORK_MODE_NONE; }
int network_get_status() { return NETWORK_STATUS_NONE; }
int network_get_authstatus() { return NETWORK_AUTH_NONE; }
uint32 network_get_server_tick() { return RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32); }
void network_send_gamecmd(uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp, uint8 callback) {}
void network_send_map() {}
void network_update() {}
int network_begin_client(const char *host, int port) { return 1; }
int network_begin_server(int port) { return 1; }
int network_get_num_players() { return 1; }
const char* network_get_player_name(unsigned int index) { return "local (OpenRCT2 compiled without MP)"; }
uint32 network_get_player_flags(unsigned int index) { return 0; }
int network_get_player_ping(unsigned int index) { return 0; }
int network_get_player_id(unsigned int index) { return 0; }
void network_send_chat(const char* text) {}
void network_send_password(const char* password) {}
void network_close() {}
void network_shutdown_client() {}
void network_kick_player(int playerId) {}
void network_set_password(const char* password) {}
uint8 network_get_current_player_id() { return 0; }
#endif /* DISABLE_NETWORK */
