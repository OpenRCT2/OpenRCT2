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

#include <cmath>
#include <cerrno>
#include <algorithm>
#include <set>
#include <string>
#include "../core/Util.hpp"
extern "C" {
#include "../config.h"
#include "../game.h"
#include "../interface/chat.h"
#include "../interface/window.h"
#include "../interface/keyboard_shortcut.h"
#include "../localisation/date.h"
#include "../localisation/localisation.h"
#include "../network/http.h"
#include "../scenario.h"
#include "../windows/error.h"
#include "../util/util.h"
#include "../cheats.h"
}

#pragma comment(lib, "Ws2_32.lib")

Network gNetwork;
NetworkActions gNetworkActions;

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
	NETWORK_COMMAND_SHOWERROR,
	NETWORK_COMMAND_GROUPLIST,
	NETWORK_COMMAND_EVENT,
	NETWORK_COMMAND_MAX,
	NETWORK_COMMAND_INVALID = -1
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

enum {
	SERVER_EVENT_PLAYER_JOINED,
	SERVER_EVENT_PLAYER_DISCONNECTED,
};

constexpr int MASTER_SERVER_REGISTER_TIME = 120 * 1000;	// 2 minutes
constexpr int MASTER_SERVER_HEARTBEAT_TIME = 60 * 1000;	// 1 minute

void network_chat_show_connected_message();

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
		return nullptr;
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

void NetworkPlayer::Read(NetworkPacket& packet)
{
	const char* name = packet.ReadString();
	SetName(name);
	packet >> id >> flags >> group;
}

void NetworkPlayer::Write(NetworkPacket& packet)
{
	packet.WriteString((const char*)name);
	packet << id << flags << group;
}

void NetworkPlayer::SetName(const char* name)
{
	safe_strcpy((char*)NetworkPlayer::name, name, sizeof(NetworkPlayer::name));
	NetworkPlayer::name[sizeof(NetworkPlayer::name) - 1] = 0;
	utf8_remove_format_codes((utf8*)NetworkPlayer::name, false);
}

void NetworkPlayer::AddMoneySpent(money32 cost)
{
	money_spent += cost;
	commands_ran++;
	window_invalidate_by_number(WC_PLAYER, id);
}

int NetworkActions::FindCommand(int command)
{
	auto it = std::find_if(actions.begin(), actions.end(), [&command](NetworkAction const& action) {
		for (auto it = action.commands.begin(); it != action.commands.end(); it++) {
			if ((*it) == command) {
				return true;
			}
		}
		return false;
	});
	if (it != actions.end()) {
		return it - actions.begin();
	}
	return -1;
}

NetworkGroup::NetworkGroup()
{
	actions_allowed = {0};
}

NetworkGroup::~NetworkGroup()
{

}

void NetworkGroup::Read(NetworkPacket& packet)
{
	packet >> id;
	SetName(packet.ReadString());
	for (size_t i = 0; i < actions_allowed.size(); i++) {
		packet >> actions_allowed[i];
	}
}

void NetworkGroup::Write(NetworkPacket& packet) 
{
	packet << id;
	packet.WriteString(GetName().c_str());
	for (size_t i = 0; i < actions_allowed.size(); i++) {
		packet << actions_allowed[i];
	}
}

void NetworkGroup::ToggleActionPermission(size_t index)
{
	size_t byte = index / 8;
	size_t bit = index % 8;
	if (byte >= actions_allowed.size()) {
		return;
	}
	actions_allowed[byte] ^= (1 << bit);
}

bool NetworkGroup::CanPerformAction(size_t index)
{
	size_t byte = index / 8;
	size_t bit = index % 8;
	if (byte >= actions_allowed.size()) {
		return false;
	}
	return (actions_allowed[byte] & (1 << bit)) ? true : false;
}

bool NetworkGroup::CanPerformCommand(int command)
{
	int action = gNetworkActions.FindCommand(command);
	if (action != -1) {
		return CanPerformAction(action);
	}
	return false;
}

std::string& NetworkGroup::GetName()
{
	return name;
}

void NetworkGroup::SetName(std::string name)
{
	NetworkGroup::name = name;
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

	if (last_disconnect_reason) {
		delete[] last_disconnect_reason;
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
#ifdef __WINDOWS__
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
#ifndef DEBUG
	if (SDL_TICKS_PASSED(SDL_GetTicks(), last_packet_time + 7000)) {
		return false;
	}
#endif
	return true;
}

const char* NetworkConnection::getLastDisconnectReason() const
{
	return this->last_disconnect_reason;
}

void NetworkConnection::setLastDisconnectReason(const char *src)
{
	if (src == nullptr) {
		if (last_disconnect_reason) {
			delete[] last_disconnect_reason;
			last_disconnect_reason = NULL;
		}
		return;
	}

	if (!last_disconnect_reason) {
		last_disconnect_reason = new char[NETWORK_DISCONNECT_REASON_BUFFER_SIZE];
	}

	strncpy(last_disconnect_reason, src, NETWORK_DISCONNECT_REASON_BUFFER_SIZE - 1);
}

void NetworkConnection::setLastDisconnectReason(const rct_string_id string_id, void *args)
{
	char buffer[NETWORK_DISCONNECT_REASON_BUFFER_SIZE];
	format_string(buffer, string_id, args);
	setLastDisconnectReason(buffer);
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
	client_command_handlers[NETWORK_COMMAND_SHOWERROR] = &Network::Client_Handle_SHOWERROR;
	client_command_handlers[NETWORK_COMMAND_GROUPLIST] = &Network::Client_Handle_GROUPLIST;
	client_command_handlers[NETWORK_COMMAND_EVENT] = &Network::Client_Handle_EVENT;
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
#ifdef __WINDOWS__
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

	server_connection.setLastDisconnectReason(nullptr);

	client_connection_list.clear();
	game_command_queue.clear();
	player_list.clear();
	group_list.clear();

#ifdef __WINDOWS__
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

	char str_resolving[256];
	format_string(str_resolving, STR_MULTIPLAYER_RESOLVING, NULL);
	window_network_status_open(str_resolving, []() -> void {
		gNetwork.Close();
	});

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

	cheats_reset();
	LoadGroups();

	NetworkPlayer* player = AddPlayer();
	player->SetName(gConfigNetwork.player_name);
	player->flags |= NETWORK_PLAYER_FLAG_ISSERVER;
	player->group = 0;
	player_id = player->id;

	printf("Ready for clients...\n");
	network_chat_show_connected_message();

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
				char str_connecting[256];
				format_string(str_connecting, STR_MULTIPLAYER_CONNECTING, NULL);
				window_network_status_open(str_connecting, []() -> void {
					gNetwork.Close();
				});
				server_connect_time = SDL_GetTicks();
				status = NETWORK_STATUS_CONNECTING;
			} else {
				log_error("connect() failed %d", LAST_SOCKET_ERROR());
				connectfailed = true;
				break;
			}
		} else if (server_address.GetResolveStatus() == NetworkAddress::RESOLVE_INPROGRESS) {
			break;
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
				char str_authenticating[256];
				format_string(str_authenticating, STR_MULTIPLAYER_AUTHENTICATING, NULL);
				window_network_status_open(str_authenticating, []() -> void {
					gNetwork.Close();
				});
			}
		}
	}break;
	case NETWORK_STATUS_CONNECTED:
		if (!ProcessConnection(server_connection)) {
			if (server_connection.authstatus == NETWORK_AUTH_REQUIREPASSWORD) { // Do not show disconnect message window when password window closed/canceled
				window_network_status_close();
			} else {
				char str_disconnected[256];

				if (server_connection.getLastDisconnectReason()) {
					const char * disconnect_reason = server_connection.getLastDisconnectReason();
					format_string(str_disconnected, STR_MULTIPLAYER_DISCONNECTED_WITH_REASON, &disconnect_reason);
				} else {
					format_string(str_disconnected, STR_MULTIPLAYER_DISCONNECTED_NO_REASON, NULL);
				}

				window_network_status_open(str_disconnected, NULL);
			}
			Close();
		}
		ProcessGameCommandQueue();

		// Check synchronisation
		if (!_desynchronised && !CheckSRAND(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32), RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_SRAND_0, uint32))) {
			_desynchronised = true;
			char str_desync[256];
			format_string(str_desync, STR_MULTIPLAYER_DESYNC, NULL);
			window_network_status_open(str_desync, NULL);
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

std::vector<std::unique_ptr<NetworkPlayer>>::iterator Network::GetPlayerIteratorByID(uint8 id)
{
	auto it = std::find_if(player_list.begin(), player_list.end(), [&id](std::unique_ptr<NetworkPlayer> const& player) { return player->id == id; });
	if (it != player_list.end()) {
		return it;
	}
	return player_list.end();
}

NetworkPlayer* Network::GetPlayerByID(uint8 id)
{
	auto it = GetPlayerIteratorByID(id);
	if (it != player_list.end()) {
		return it->get();
	}
	return nullptr;
}

std::vector<std::unique_ptr<NetworkGroup>>::iterator Network::GetGroupIteratorByID(uint8 id)
{
	auto it = std::find_if(group_list.begin(), group_list.end(), [&id](std::unique_ptr<NetworkGroup> const& group) { return group->id == id; });
	if (it != group_list.end()) {
		return it;
	}
	return group_list.end();
}

NetworkGroup* Network::GetGroupByID(uint8 id)
{
	auto it = GetGroupIteratorByID(id);
	if (it != group_list.end()) {
		return it->get();
	}
	return nullptr;
}

const char* Network::FormatChat(NetworkPlayer* fromplayer, const char* text)
{
	static char formatted[1024];
	char* lineCh = formatted;
	formatted[0] = 0;
	if (fromplayer) {
		lineCh = utf8_write_codepoint(lineCh, FORMAT_OUTLINE);
		lineCh = utf8_write_codepoint(lineCh, FORMAT_BABYBLUE);
		safe_strcpy(lineCh, (const char*)fromplayer->name, sizeof(fromplayer->name));
		strcat(lineCh, ": ");
		lineCh = strchr(lineCh, '\0');
	}
	lineCh = utf8_write_codepoint(lineCh, FORMAT_OUTLINE);
	lineCh = utf8_write_codepoint(lineCh, FORMAT_WHITE);
	char* ptrtext = lineCh;
	safe_strcpy(lineCh, text, 800);
	utf8_remove_format_codes((utf8*)ptrtext, true);
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
			(*it)->setLastDisconnectReason(STR_MULTIPLAYER_KICKED);
			char str_disconnect_msg[256];
			format_string(str_disconnect_msg, STR_MULTIPLAYER_KICKED_REASON, NULL);
			Server_Send_SETDISCONNECTMSG(*(*it), str_disconnect_msg);
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
		int hexCharIndex = util_rand() % Util::CountOf(hexChars);
		key[i] = hexChars[hexCharIndex];
	}
	key[Util::CountOf(key) - 1] = 0;

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
	json_object_set_new(body, "key", json_string(advertise_key.c_str()));
	json_object_set_new(body, "port", json_integer(listening_port));
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
	json_object_set_new(body, "token", json_string(advertise_token.c_str()));
	json_object_set_new(body, "players", json_integer(network_get_num_players()));

	json_t *gameInfo = json_object();
	json_object_set_new(gameInfo, "mapSize", json_integer(RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE_MINUS_2, sint16)));
	json_object_set_new(gameInfo, "day", json_integer(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_TICKS, uint16)));
	json_object_set_new(gameInfo, "month", json_integer(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, uint16)));
	json_object_set_new(gameInfo, "guests", json_integer(RCT2_GLOBAL(RCT2_ADDRESS_GUESTS_IN_PARK, uint16)));
	json_object_set_new(gameInfo, "parkValue", json_integer(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PARK_VALUE, money32)));
	if (!(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY)) {
		money32 cash = DECRYPT_MONEY(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONEY_ENCRYPTED, money32));
		json_object_set_new(gameInfo, "cash", json_integer(cash));
	}

	json_object_set_new(body, "gameInfo", gameInfo);
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

NetworkGroup* Network::AddGroup()
{
	NetworkGroup* addedgroup = nullptr;
	int newid = -1;
	// Find first unused group id
	for (int id = 0; id < 255; id++) {
		if (std::find_if(group_list.begin(), group_list.end(), [&id](std::unique_ptr<NetworkGroup> const& group) { return group->id == id; }) == group_list.end()) {
			newid = id;
			break;
		}
	}
	if (newid != -1) {
		std::unique_ptr<NetworkGroup> group(new NetworkGroup); // change to make_unique in c++14
		group->id = newid;
		group->SetName("Group #" + std::to_string(newid));
		addedgroup = group.get();
		group_list.push_back(std::move(group));
	}
	return addedgroup;
}

void Network::RemoveGroup(uint8 id)
{
	auto group = GetGroupIteratorByID(id);
	if (group != group_list.end()) {
		group_list.erase(group);
	}
}

uint8 Network::GetDefaultGroup()
{
	return default_group;
}

void Network::SetDefaultGroup(uint8 id)
{
	if (GetGroupByID(id)) {
		default_group = id;
	}
}

void Network::SaveGroups()
{
	if (GetMode() == NETWORK_MODE_SERVER) {
		utf8 path[MAX_PATH];
		SDL_RWops *file;

		platform_get_user_directory(path, NULL);
		strcat(path, "groups.cfg");

		file = SDL_RWFromFile(path, "wb");
		if (file == NULL) {
			return;
		}

		std::unique_ptr<NetworkPacket> stream = std::move(NetworkPacket::Allocate());
		*stream << (uint8)group_list.size();
		*stream << default_group;
		for (auto it = group_list.begin(); it != group_list.end(); it++) {
			(*it)->Write(*stream);
		}

		SDL_RWwrite(file, stream->GetData(), stream->data->size(), 1);

		SDL_RWclose(file);
	}
}

void Network::LoadGroups()
{
	group_list.clear();

	utf8 path[MAX_PATH];
	SDL_RWops *file;

	platform_get_user_directory(path, NULL);
	strcat(path, "groups.cfg");

	file = SDL_RWFromFile(path, "rb");
	if (file == NULL) {
		// Hardcoded permission groups
		std::unique_ptr<NetworkGroup> admin(new NetworkGroup()); // change to make_unique in c++14
		admin->SetName("Admin");
		admin->actions_allowed.fill(0xFF);
		admin->id = 0;
		group_list.push_back(std::move(admin));
		std::unique_ptr<NetworkGroup> spectator(new NetworkGroup()); // change to make_unique in c++14
		spectator->SetName("Spectator");
		spectator->ToggleActionPermission(0); // Chat
		spectator->id = 1;
		group_list.push_back(std::move(spectator));
		std::unique_ptr<NetworkGroup> user(new NetworkGroup()); // change to make_unique in c++14
		user->SetName("User");
		user->actions_allowed.fill(0xFF);
		user->ToggleActionPermission(15); // Kick Player
		user->ToggleActionPermission(16); // Modify Groups
		user->ToggleActionPermission(17); // Set Player Group
		user->ToggleActionPermission(18); // Cheat
		user->id = 2;
		group_list.push_back(std::move(user));
		SetDefaultGroup(1);
		return;
	}

	std::unique_ptr<NetworkPacket> stream = std::move(NetworkPacket::Allocate());
	uint8 byte;
	while(SDL_RWread(file, &byte, sizeof(byte), 1)){
		*stream << byte;
	}
	stream->size = (uint16)stream->data->size();

	uint8 num;
	*stream >> num >> default_group;
	for (unsigned int i = 0; i < num; i++) {
		NetworkGroup group;
		group.Read(*stream);
		std::unique_ptr<NetworkGroup> newgroup(new NetworkGroup(group)); // change to make_unique in c++14
		group_list.push_back(std::move(newgroup));
	}

	SDL_RWclose(file);
}

void Network::Client_Send_AUTH(const char* name, const char* password)
{
	std::unique_ptr<NetworkPacket> packet = std::move(NetworkPacket::Allocate());
	*packet << (uint32)NETWORK_COMMAND_AUTH;
	packet->WriteString(NETWORK_STREAM_ID);
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
	if (connection.authstatus == NETWORK_AUTH_BADVERSION) {
		packet->WriteString(NETWORK_STREAM_ID);
	}
	connection.QueuePacket(std::move(packet));
	if (connection.authstatus != NETWORK_AUTH_OK && connection.authstatus != NETWORK_AUTH_REQUIREPASSWORD) {
		shutdown(connection.socket, SHUT_RD);
		connection.SendQueuedPackets();
	}
}

void Network::Server_Send_MAP(NetworkConnection* connection)
{
	bool RLEState = gUseRLE;
	gUseRLE = false;
	FILE* temp = tmpfile();
	if (!temp) {
		log_warning("Failed to create temporary file to save map.");
		return;
	}
	SDL_RWops* rw = SDL_RWFromFP(temp, SDL_TRUE);
	scenario_save_network(rw);
	gUseRLE = RLEState;
	int size = (int)SDL_RWtell(rw);
	std::vector<uint8> buffer(size);
	SDL_RWseek(rw, 0, RW_SEEK_SET);
	if (SDL_RWread(rw, &buffer[0], size, 1) == 0) {
		log_warning("Failed to read temporary map file into memory.");
		SDL_RWclose(rw);
		return;
	}
	size_t chunksize = 1024;
	size_t out_size = size;
	unsigned char *compressed = util_zlib_deflate(&buffer[0], size, &out_size);
	unsigned char *header;
	if (compressed != NULL)
	{
		header = (unsigned char *)_strdup("open2_sv6_zlib");
		size_t header_len = strlen((char *)header) + 1; // account for null terminator
		header = (unsigned char *)realloc(header, header_len + out_size);
		memcpy(&header[header_len], compressed, out_size);
		out_size += header_len;
		free(compressed);
		log_verbose("Sending map of size %u bytes, compressed to %u bytes", size, out_size);
	} else {
		log_warning("Failed to compress the data, falling back to non-compressed sv6.");
		header = (unsigned char *)malloc(size);
		out_size = size;
		memcpy(header, &buffer[0], size);
	}
	for (size_t i = 0; i < out_size; i += chunksize) {
		int datasize = (std::min)(chunksize, out_size - i);
		std::unique_ptr<NetworkPacket> packet = std::move(NetworkPacket::Allocate());
		*packet << (uint32)NETWORK_COMMAND_MAP << (uint32)out_size << (uint32)i;
		packet->Write(&header[i], datasize);
		if (connection) {
			connection->QueuePacket(std::move(packet));
		} else {
			SendPacketToClients(*packet);
		}
	}
	free(header);
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
	*packet << (uint32)NETWORK_COMMAND_PLAYERLIST << (uint8)player_list.size();
	for (unsigned int i = 0; i < player_list.size(); i++) {
		player_list[i]->Write(*packet);
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
	*packet << (uint32)NETWORK_COMMAND_PINGLIST << (uint8)player_list.size();
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
	json_object_set_new(obj, "name", json_string(gConfigNetwork.server_name));
	json_object_set_new(obj, "requiresPassword", json_boolean(password.size() > 0));
	json_object_set_new(obj, "version", json_string(NETWORK_STREAM_ID));
	json_object_set_new(obj, "players", json_integer(player_list.size()));
	json_object_set_new(obj, "maxPlayers", json_integer(gConfigNetwork.maxplayers));
	json_object_set_new(obj, "description", json_string(gConfigNetwork.server_description));
	json_object_set_new(obj, "dedicated", json_boolean(gOpenRCT2Headless));

	// Provider details
	json_t* jsonProvider = json_object();
	json_object_set_new(jsonProvider, "name", json_string(gConfigNetwork.provider_name));
	json_object_set_new(jsonProvider, "email", json_string(gConfigNetwork.provider_email));
	json_object_set_new(jsonProvider, "website", json_string(gConfigNetwork.provider_website));
	json_object_set_new(obj, "provider", jsonProvider);

	packet->WriteString(json_dumps(obj, 0));
	json_decref(obj);
#endif
	connection.QueuePacket(std::move(packet));
}

void Network::Server_Send_SHOWERROR(NetworkConnection& connection, rct_string_id title, rct_string_id message)
{
	std::unique_ptr<NetworkPacket> packet = std::move(NetworkPacket::Allocate());
	*packet << (uint32)NETWORK_COMMAND_SHOWERROR << title << message;
	connection.QueuePacket(std::move(packet));
}

void Network::Server_Send_GROUPLIST(NetworkConnection& connection)
{
	std::unique_ptr<NetworkPacket> packet = std::move(NetworkPacket::Allocate());
	*packet << (uint32)NETWORK_COMMAND_GROUPLIST << (uint8)group_list.size() << default_group;
	for (unsigned int i = 0; i < group_list.size(); i++) {
		group_list[i]->Write(*packet);
	}
	connection.QueuePacket(std::move(packet));
}

void Network::Server_Send_EVENT_PLAYER_JOINED(const char *playerName)
{
	std::unique_ptr<NetworkPacket> packet = std::move(NetworkPacket::Allocate());
	*packet << (uint32)NETWORK_COMMAND_EVENT;
	*packet << (uint16)SERVER_EVENT_PLAYER_JOINED;
	packet->WriteString(playerName);
	SendPacketToClients(*packet);
}

void Network::Server_Send_EVENT_PLAYER_DISCONNECTED(const char *playerName, const char *reason)
{
	std::unique_ptr<NetworkPacket> packet = std::move(NetworkPacket::Allocate());
	*packet << (uint32)NETWORK_COMMAND_EVENT;
	*packet << (uint16)SERVER_EVENT_PLAYER_DISCONNECTED;
	packet->WriteString(playerName);
	packet->WriteString(reason);
	SendPacketToClients(*packet);
}

bool Network::ProcessConnection(NetworkConnection& connection)
{
	int packetStatus;
	do {
		packetStatus = connection.ReadPacket();
		switch(packetStatus) {
		case NETWORK_READPACKET_DISCONNECTED:
			// closed connection or network error
			if (!connection.getLastDisconnectReason()) {
				connection.setLastDisconnectReason(STR_MULTIPLAYER_CONNECTION_CLOSED);
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
		if (!connection.getLastDisconnectReason()) {
			connection.setLastDisconnectReason(STR_MULTIPLAYER_NO_DATA);
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
		game_command_playerid = gc.playerid;
		int command = gc.esi;
		money32 cost = game_do_command_p(command, (int*)&gc.eax, (int*)&gc.ebx, (int*)&gc.ecx, (int*)&gc.edx, (int*)&gc.esi, (int*)&gc.edi, (int*)&gc.ebp);
		if (cost != MONEY32_UNDEFINED) {
			NetworkPlayer* player = GetPlayerByID(gc.playerid);
			if (player) {
				player->last_action = gNetworkActions.FindCommand(command);
				player->last_action_time = SDL_GetTicks();
				player->AddMoneySpent(cost);
			}
		}
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
		const char * has_disconnected_args[2] = {
				(char *) connection_player->name,
				connection->getLastDisconnectReason()
		};
		if (has_disconnected_args[1]) {
			format_string(text, STR_MULTIPLAYER_PLAYER_HAS_DISCONNECTED_WITH_REASON, has_disconnected_args);
		} else {
			format_string(text, STR_MULTIPLAYER_PLAYER_HAS_DISCONNECTED_NO_REASON, &(has_disconnected_args[0]));
		}

		chat_history_add(text);
		gNetwork.Server_Send_EVENT_PLAYER_DISCONNECTED((char*)connection_player->name, connection->getLastDisconnectReason());
	}
	player_list.erase(std::remove_if(player_list.begin(), player_list.end(), [connection_player](std::unique_ptr<NetworkPlayer>& player){ return player.get() == connection_player; }), player_list.end());
	client_connection_list.remove(connection);
	Server_Send_PLAYERLIST();
}

NetworkPlayer* Network::AddPlayer()
{
	NetworkPlayer* addedplayer = nullptr;
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
		std::unique_ptr<NetworkPlayer> player(new NetworkPlayer); // change to make_unique in c++14
		player->id = newid;
		player->group = GetDefaultGroup();
		addedplayer = player.get();
		player_list.push_back(std::move(player));
	}
	return addedplayer;
}

void Network::PrintError()
{
#ifdef __WINDOWS__
	wchar_t *s = NULL;
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, LAST_SOCKET_ERROR(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&s, 0, NULL);
	fprintf(stderr, "%S\n", s);
	LocalFree(s);
#else
	char *s = strerror(LAST_SOCKET_ERROR());
	fprintf(stderr, "%s\n", s);
#endif

}

void Network::Client_Handle_AUTH(NetworkConnection& connection, NetworkPacket& packet)
{
	packet >> (uint32&)connection.authstatus >> (uint8&)player_id;
	switch(connection.authstatus) {
	case NETWORK_AUTH_BADNAME:
		connection.setLastDisconnectReason(STR_MULTIPLAYER_BAD_PLAYER_NAME);
		shutdown(connection.socket, SHUT_RDWR);
		break;
	case NETWORK_AUTH_BADVERSION:
	{
		const char *version = packet.ReadString();
		connection.setLastDisconnectReason(STR_MULTIPLAYER_INCORRECT_SOFTWARE_VERSION, &version);
		shutdown(connection.socket, SHUT_RDWR);
		break;
	}
	case NETWORK_AUTH_BADPASSWORD:
		connection.setLastDisconnectReason(STR_MULTIPLAYER_BAD_PASSWORD);
		shutdown(connection.socket, SHUT_RDWR);
		break;
	case NETWORK_AUTH_FULL:
		connection.setLastDisconnectReason(STR_MULTIPLAYER_SERVER_FULL);
		shutdown(connection.socket, SHUT_RDWR);
		break;
	case NETWORK_AUTH_REQUIREPASSWORD:
		window_network_status_open_password();
		break;
	}
}

void Network::Server_Handle_AUTH(NetworkConnection& connection, NetworkPacket& packet)
{
	if (connection.authstatus != NETWORK_AUTH_OK) {
		const char* gameversion = packet.ReadString();
		const char* name = packet.ReadString();
		const char* password = packet.ReadString();
		if (!gameversion || strcmp(gameversion, NETWORK_STREAM_ID) != 0) {
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
			NetworkPlayer* player = AddPlayer();
			connection.player = player;
			if (player) {
				player->SetName(name);
				char text[256];
				const char * player_name = (const char *) player->name;
				format_string(text, STR_MULTIPLAYER_PLAYER_HAS_JOINED_THE_GAME, &player_name);
				chat_history_add(text);
				Server_Send_MAP(&connection);
				gNetwork.Server_Send_EVENT_PLAYER_JOINED(player_name);
				Server_Send_GROUPLIST(connection);
				Server_Send_PLAYERLIST();
			}
		}
		Server_Send_AUTH(connection);
	}
}

void Network::Client_Handle_MAP(NetworkConnection& connection, NetworkPacket& packet)
{
	uint32 size, offset;
	packet >> size >> offset;
	int chunksize = packet.size - packet.read;
	if (chunksize <= 0) {
		return;
	}
	if (size > chunk_buffer.size()) {
		chunk_buffer.resize(size);
	}
	char str_downloading_map[256];
	unsigned int downloading_map_args[2] = {(offset + chunksize) / 1024, size / 1024};
	format_string(str_downloading_map, STR_MULTIPLAYER_DOWNLOADING_MAP, downloading_map_args);
	window_network_status_open(str_downloading_map, []() -> void {
		gNetwork.Close();
	});
	memcpy(&chunk_buffer[offset], (void*)packet.Read(chunksize), chunksize);
	if (offset + chunksize == size) {
		window_network_status_close();
		bool has_to_free = false;
		unsigned char *data = &chunk_buffer[0];
		size_t data_size = size;
		// zlib-compressed
		if (strcmp("open2_sv6_zlib", (char *)&chunk_buffer[0]) == 0)
		{
			log_verbose("Received zlib-compressed sv6 map");
			has_to_free = true;
			size_t header_len = strlen("open2_sv6_zlib") + 1;
			data = util_zlib_inflate(&chunk_buffer[header_len], size - header_len, &data_size);
			if (data == NULL)
			{
				log_warning("Failed to decompress data sent from server.");
				return;
			}
		} else {
			log_verbose("Assuming received map is in plain sv6 format");
		}
		SDL_RWops* rw = SDL_RWFromMem(data, data_size);
		if (game_load_network(rw)) {
			game_load_init();
			game_command_queue.clear();
			server_tick = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32);
			server_srand0_tick = 0;
			// window_network_status_open("Loaded new map from network");
			_desynchronised = false;

			// Notify user he is now online and which shortcut key enables chat
			network_chat_show_connected_message();
		}
		else
		{
			//Something went wrong, game is not loaded. Return to main screen.
			game_do_command(0, GAME_COMMAND_FLAG_APPLY, 0, 0, GAME_COMMAND_LOAD_OR_QUIT, 1, 0);
		}
		SDL_RWclose(rw);
		if (has_to_free)
		{
			free(data);
		}
	}
}

void Network::Client_Handle_CHAT(NetworkConnection& connection, NetworkPacket& packet)
{
	const char* text = packet.ReadString();
	if (text) {
		chat_history_add(text);
	}
}

void Network::Server_Handle_CHAT(NetworkConnection& connection, NetworkPacket& packet)
{
	if (connection.player) {
		NetworkGroup* group = GetGroupByID(connection.player->group);
		if (!group || (group && !group->CanPerformCommand(-1))) {
			return;
		}
	}
	const char* text = packet.ReadString();
	if (text) {
		const char* formatted = FormatChat(connection.player, text);
		chat_history_add(formatted);
		Server_Send_CHAT(formatted);
	}
}

void Network::Client_Handle_GAMECMD(NetworkConnection& connection, NetworkPacket& packet)
{
	uint32 tick;
	uint32 args[7];
	uint8 playerid;
	uint8 callback;
	packet >> tick >> args[0] >> args[1] >> args[2] >> args[3] >> args[4] >> args[5] >> args[6] >> playerid >> callback;

	GameCommand gc = GameCommand(tick, args, playerid, callback);
	game_command_queue.insert(gc);
}

void Network::Server_Handle_GAMECMD(NetworkConnection& connection, NetworkPacket& packet)
{
	uint32 tick;
	uint32 args[7];
	uint8 playerid;
	uint8 callback;

	if (!connection.player) {
		return;
	}

	playerid = connection.player->id;

	packet >> tick >> args[0] >> args[1] >> args[2] >> args[3] >> args[4] >> args[5] >> args[6] >> callback;

	int commandCommand = args[4];

	int ticks = SDL_GetTicks(); //tick count is different by time last_action_time is set, keep same value.
	
	// Check if player's group permission allows command to run
	NetworkGroup* group = GetGroupByID(connection.player->group);
	if (!group || (group && !group->CanPerformCommand(commandCommand))) {
		Server_Send_SHOWERROR(connection, STR_CANT_DO_THIS, STR_PERMISSION_DENIED);
		return;
	}
	// Incase someone modifies the code / memory to enable cluster build,
	// require a small delay in between placing scenery to provide some security, as 
	// cluster mode is a for loop that runs the place_scenery code multiple times.
	if (commandCommand == GAME_COMMAND_PLACE_SCENERY) {
		if ((ticks - connection.player->last_action_time) < 20) {
			if (!(group->CanPerformCommand(-2))) {
				Server_Send_SHOWERROR(connection, STR_CANT_DO_THIS, STR_CANT_DO_THIS);
				return;
			}
		}
	}
	// Don't let clients send pause or quit
	if (commandCommand == GAME_COMMAND_TOGGLE_PAUSE ||
		commandCommand == GAME_COMMAND_LOAD_OR_QUIT
	) {
		return;
	}

	// Set this to reference inside of game command functions
	game_command_playerid = playerid;
	// Run game command, and if it is successful send to clients
	money32 cost = game_do_command(args[0], args[1] | GAME_COMMAND_FLAG_NETWORKED, args[2], args[3], args[4], args[5], args[6]);
	if (cost == MONEY32_UNDEFINED) {
		return;
	}

	connection.player->last_action = gNetworkActions.FindCommand(commandCommand);
	connection.player->last_action_time = SDL_GetTicks();
	connection.player->AddMoneySpent(cost);
	Server_Send_GAMECMD(args[0], args[1], args[2], args[3], args[4], args[5], args[6], playerid, callback);
}

void Network::Client_Handle_TICK(NetworkConnection& connection, NetworkPacket& packet)
{
	uint32 srand0;
	packet >> server_tick >> srand0;
	if (server_srand0_tick == 0) {
		server_srand0 = srand0;
		server_srand0_tick = server_tick;
	}
}

void Network::Client_Handle_PLAYERLIST(NetworkConnection& connection, NetworkPacket& packet)
{
	uint8 size;
	packet >> size;
	std::vector<uint8> ids;
	for (unsigned int i = 0; i < size; i++) {
		NetworkPlayer tempplayer;
		tempplayer.Read(packet);
		ids.push_back(tempplayer.id);
		if (!GetPlayerByID(tempplayer.id)) {
			NetworkPlayer* player = AddPlayer();
			if (player) {
				*player = tempplayer;
				if (player->flags & NETWORK_PLAYER_FLAG_ISSERVER) {
					server_connection.player = player;
				}
			}
		}
	}
	// Remove any players that are not in newly received list
	auto it = player_list.begin();
	while (it != player_list.end()) {
		if (std::find(ids.begin(), ids.end(), (*it)->id) == ids.end()) {
			it = player_list.erase(it);
		} else {
			it++;
		}
	}
}

void Network::Client_Handle_PING(NetworkConnection& connection, NetworkPacket& packet)
{
	Client_Send_PING();
}

void Network::Server_Handle_PING(NetworkConnection& connection, NetworkPacket& packet)
{
	int ping = SDL_GetTicks() - connection.ping_time;
	if (ping < 0) {
		ping = 0;
	}
	if (connection.player) {
		connection.player->ping = ping;
		window_invalidate_by_number(WC_PLAYER, connection.player->id);
	}
}

void Network::Client_Handle_PINGLIST(NetworkConnection& connection, NetworkPacket& packet)
{
	uint8 size;
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
	window_invalidate_by_class(WC_PLAYER);
}

void Network::Client_Handle_SETDISCONNECTMSG(NetworkConnection& connection, NetworkPacket& packet)
{
	static std::string msg;
	const char* disconnectmsg = packet.ReadString();
	if (disconnectmsg) {
		msg = disconnectmsg;
		connection.setLastDisconnectReason(msg.c_str());
	}
}

void Network::Server_Handle_GAMEINFO(NetworkConnection& connection, NetworkPacket& packet)
{
	Server_Send_GAMEINFO(connection);
}

void Network::Client_Handle_SHOWERROR(NetworkConnection& connection, NetworkPacket& packet)
{
	rct_string_id title, message;
	packet >> title >> message;
	window_error_open(title, message);
}

void Network::Client_Handle_GROUPLIST(NetworkConnection& connection, NetworkPacket& packet)
{
	group_list.clear();
	uint8 size;
	packet >> size >> default_group;
	for (unsigned int i = 0; i < size; i++) {
		NetworkGroup group;
		group.Read(packet);
		std::unique_ptr<NetworkGroup> newgroup(new NetworkGroup(group)); // change to make_unique in c++14
		group_list.push_back(std::move(newgroup));
	}
}

void Network::Client_Handle_EVENT(NetworkConnection& connection, NetworkPacket& packet)
{
	uint16 eventType;
	packet >> eventType;
	switch (eventType) {
	case SERVER_EVENT_PLAYER_JOINED:
	{
		char text[256];
		const char *playerName = packet.ReadString();
		format_string(text, STR_MULTIPLAYER_PLAYER_HAS_JOINED_THE_GAME, &playerName);
		chat_history_add(text);
		break;
	}
	case SERVER_EVENT_PLAYER_DISCONNECTED:
	{
		char text[256];
		const char *playerName = packet.ReadString();
		const char *reason = packet.ReadString();
		const char *args[] = { playerName, reason };
		if (str_is_null_or_empty(reason)) {
			format_string(text, STR_MULTIPLAYER_PLAYER_HAS_DISCONNECTED_NO_REASON, args);
		} else {
			format_string(text, STR_MULTIPLAYER_PLAYER_HAS_DISCONNECTED_WITH_REASON, args);
		}
		chat_history_add(text);
		break;
	}
	}
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

money32 network_get_player_money_spent(unsigned int index)
{
	return gNetwork.player_list[index]->money_spent;
}

void network_add_player_money_spent(unsigned int index, money32 cost)
{
	gNetwork.player_list[index]->AddMoneySpent(cost);
}

int network_get_player_last_action(unsigned int index, int time)
{
	if (time && SDL_TICKS_PASSED(SDL_GetTicks(), gNetwork.player_list[index]->last_action_time + time)) {
		return -999;
	}
	return gNetwork.player_list[index]->last_action;
}

void network_set_player_last_action(unsigned int index, int command)
{
	gNetwork.player_list[index]->last_action = gNetworkActions.FindCommand(command);
	gNetwork.player_list[index]->last_action_time = SDL_GetTicks();
}

rct_xyz16 network_get_player_last_action_coord(unsigned int index)
{
	return gNetwork.player_list[index]->last_action_coord;
}

void network_set_player_last_action_coord(unsigned int index, rct_xyz16 coord)
{
	if (index < gNetwork.player_list.size()) {
		gNetwork.player_list[index]->last_action_coord = coord;
	}
}

unsigned int network_get_player_commands_ran(unsigned int index)
{
	return gNetwork.player_list[index]->commands_ran;
}

int network_get_player_index(uint8 id)
{
	auto it = gNetwork.GetPlayerIteratorByID(id);
	if(it == gNetwork.player_list.end()){
		return -1;
	}
	return gNetwork.GetPlayerIteratorByID(id) - gNetwork.player_list.begin();
}

uint8 network_get_player_group(unsigned int index)
{
	return gNetwork.player_list[index]->group;
}

void network_set_player_group(unsigned int index, unsigned int groupindex)
{
	gNetwork.player_list[index]->group = gNetwork.group_list[groupindex]->id;
}

int network_get_group_index(uint8 id)
{
	auto it = gNetwork.GetGroupIteratorByID(id);
	if(it == gNetwork.group_list.end()){
		return -1;
	}
	return gNetwork.GetGroupIteratorByID(id) - gNetwork.group_list.begin();
}

uint8 network_get_group_id(unsigned int index)
{
	return gNetwork.group_list[index]->id;
}

int network_get_num_groups()
{
	return gNetwork.group_list.size();
}

const char* network_get_group_name(unsigned int index)
{
	return gNetwork.group_list[index]->GetName().c_str();
}

void network_chat_show_connected_message()
{
	char *templateString = (char*)language_get_string(STR_INDIVIDUAL_KEYS_BASE);
	keyboard_shortcut_format_string(templateString, gShortcutKeys[SHORTCUT_OPEN_CHAT_WINDOW]);
	utf8 buffer[256];
	NetworkPlayer server;
	safe_strcpy((char*)&server.name, "Server", sizeof(server.name));
	format_string(buffer, STR_MULTIPLAYER_CONNECTED_CHAT_HINT, &templateString);
	const char *formatted = Network::FormatChat(&server, buffer);
	chat_history_add(formatted);
}

void game_command_set_player_group(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp)
{
	uint8 playerid = (uint8)*ecx;
	uint8 groupid = (uint8)*edx;
	NetworkPlayer* player = gNetwork.GetPlayerByID(playerid);
	NetworkGroup* fromgroup = gNetwork.GetGroupByID(game_command_playerid);
	if (!player) {
		*ebx = MONEY32_UNDEFINED;
		return;
	}
	if (!gNetwork.GetGroupByID(groupid)) {
		*ebx = MONEY32_UNDEFINED;
		return;
	}
	if (player->flags & NETWORK_PLAYER_FLAG_ISSERVER) {
		gGameCommandErrorTitle = STR_CANT_CHANGE_GROUP_THAT_THE_HOST_BELONGS_TO;
		*ebx = MONEY32_UNDEFINED;
		return;
	}
	if (groupid == 0 && fromgroup && fromgroup->id != 0) {
		gGameCommandErrorTitle = STR_CANT_SET_TO_THIS_GROUP;
		*ebx = MONEY32_UNDEFINED;
		return;
	}
	if (*ebx & GAME_COMMAND_FLAG_APPLY) {
		player->group = groupid;
		window_invalidate_by_number(WC_PLAYER, playerid);
	}
	*ebx = 0;
}

void game_command_modify_groups(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp)
{
	uint8 action = (uint8)*eax;
	uint8 groupid = (uint8)(*eax >> 8);
	uint8 nameChunkIndex = (uint8)(*eax >> 16);

	char oldName[128];
	static char newName[128];

	switch (action)
	{
	case 0:{ // add group
		if (*ebx & GAME_COMMAND_FLAG_APPLY) {
			NetworkGroup* newgroup = gNetwork.AddGroup();
			if (!newgroup) {
				*ebx = MONEY32_UNDEFINED;
				return;
			}
		}
	}break;
	case 1:{ // remove group
		if (groupid == 0) {
			gGameCommandErrorTitle = STR_THIS_GROUP_CANNOT_BE_MODIFIED;
			*ebx = MONEY32_UNDEFINED;
			return;
		}
		for (auto it = gNetwork.player_list.begin(); it != gNetwork.player_list.end(); it++) {
			if((*it)->group == groupid) {
				gGameCommandErrorTitle = STR_CANT_REMOVE_GROUP_THAT_PLAYERS_BELONG_TO;
				*ebx = MONEY32_UNDEFINED;
				return;
			}
		}
		if (*ebx & GAME_COMMAND_FLAG_APPLY) {
			gNetwork.RemoveGroup(groupid);
		}
	}break;
	case 2:{ // set permissions
		int index = *ecx;
		bool all = *edx & 1;
		bool allvalue = (*edx >> 1) & 1;
		if (groupid == 0) { // cant change admin group permissions
			gGameCommandErrorTitle = STR_THIS_GROUP_CANNOT_BE_MODIFIED;
			*ebx = MONEY32_UNDEFINED;
			return;
		}
		NetworkGroup* mygroup = nullptr;
		NetworkPlayer* player = gNetwork.GetPlayerByID(game_command_playerid);
		if (player && !all) {
			mygroup = gNetwork.GetGroupByID(player->group);
			if (!mygroup || (mygroup && !mygroup->CanPerformAction(index))) {
				gGameCommandErrorTitle = STR_CANT_MODIFY_PERMISSION_THAT_YOU_DO_NOT_HAVE_YOURSELF;
				*ebx = MONEY32_UNDEFINED;
				return;
			}
		}
		if (*ebx & GAME_COMMAND_FLAG_APPLY) {
			NetworkGroup* group = gNetwork.GetGroupByID(groupid);
			if (group) {
				if (all) {
					if (mygroup) {
						if (allvalue) {
							group->actions_allowed = mygroup->actions_allowed;
						} else {
							group->actions_allowed.fill(0x00);
						}
					}
				} else {
					group->ToggleActionPermission(index);
				}
			}
		}
	}break;
	case 3:{ // set group name
		size_t nameChunkOffset = nameChunkIndex - 1;
		if (nameChunkIndex == 0)
			nameChunkOffset = 2;
		nameChunkOffset *= 12;
		nameChunkOffset = (std::min)(nameChunkOffset, Util::CountOf(newName) - 12);
		RCT2_GLOBAL(newName + nameChunkOffset + 0, uint32) = *edx;
		RCT2_GLOBAL(newName + nameChunkOffset + 4, uint32) = *ebp;
		RCT2_GLOBAL(newName + nameChunkOffset + 8, uint32) = *edi;

		if (nameChunkIndex != 0) {
			*ebx = 0;
			return;
		}

		if (strcmp(oldName, newName) == 0) {
			*ebx = 0;
			return;
		}

		if (newName[0] == 0) {
			gGameCommandErrorTitle = STR_CANT_RENAME_GROUP;
			gGameCommandErrorText = STR_INVALID_GROUP_NAME;
			*ebx = MONEY32_UNDEFINED;
			return;
		}

		if (*ebx & GAME_COMMAND_FLAG_APPLY) {
			NetworkGroup* group = gNetwork.GetGroupByID(groupid);
			if (group) {
				group->SetName(newName);
			}
		}
	}break;
	case 4:{ // set default group
		if (groupid == 0) {
			gGameCommandErrorTitle = STR_CANT_SET_TO_THIS_GROUP;
			*ebx = MONEY32_UNDEFINED;
			return;
		}
		if (*ebx & GAME_COMMAND_FLAG_APPLY) {
			gNetwork.SetDefaultGroup(groupid);
		}
	}break;
	}

	gNetwork.SaveGroups();

	*ebx = 0;
}

void game_command_kick_player(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp)
{
	uint8 playerid = (uint8)*eax;
	NetworkPlayer* player = gNetwork.GetPlayerByID(playerid);
	if (player && player->flags & NETWORK_PLAYER_FLAG_ISSERVER) {
		gGameCommandErrorTitle = STR_CANT_KICK_THE_HOST;
		*ebx = MONEY32_UNDEFINED;
		return;
	}
	if (*ebx & GAME_COMMAND_FLAG_APPLY) {
		if (gNetwork.GetMode() == NETWORK_MODE_SERVER) {
			gNetwork.KickPlayer(playerid);
		}
	}
	*ebx = 0;
}

uint8 network_get_default_group()
{
	return gNetwork.GetDefaultGroup();
}

int network_get_num_actions()
{
	return gNetworkActions.actions.size();
}

rct_string_id network_get_action_name_string_id(unsigned int index)
{
	return gNetworkActions.actions[index].name;
}

int network_can_perform_action(unsigned int groupindex, unsigned int index)
{
	return gNetwork.group_list[groupindex]->CanPerformAction(index);
}

int network_can_perform_command(unsigned int groupindex, unsigned int index) 
{
	return gNetwork.group_list[groupindex]->CanPerformCommand(index);
}

int network_get_current_player_group_index()
{
	return network_get_group_index(gNetwork.GetPlayerByID(gNetwork.GetPlayerID())->group);
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
money32 network_get_player_money_spent(unsigned int index) { return MONEY(0, 0); }
void network_add_player_money_spent(unsigned int index, money32 cost) { }
int network_get_player_last_action(unsigned int index, int time) { return -999; }
void network_set_player_last_action(unsigned int index, int command) { }
rct_xyz16 network_get_player_last_action_coord(unsigned int index) { return {0, 0, 0}; }
void network_set_player_last_action_coord(unsigned int index, rct_xyz16 coord) { }
unsigned int network_get_player_commands_ran(unsigned int index) { return 0; }
int network_get_player_index(uint8 id) { return -1; }
uint8 network_get_player_group(unsigned int index) { return 0; }
void network_set_player_group(unsigned int index, unsigned int groupindex) { }
int network_get_group_index(uint8 id) { return -1; }
uint8 network_get_group_id(unsigned int index) { return 0; }
int network_get_num_groups() { return 0; }
const char* network_get_group_name(unsigned int index) { return ""; };
rct_string_id network_get_group_name_string_id(unsigned int index) { return -1; }
void game_command_set_player_group(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp) { }
void game_command_modify_groups(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp) { }
void game_command_kick_player(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp) { }
uint8 network_get_default_group() { return 0; }
int network_get_num_actions() { return 0; }
rct_string_id network_get_action_name_string_id(unsigned int index) { return -1; }
int network_can_perform_action(unsigned int groupindex, unsigned int index) { return 0; }
int network_can_perform_command(unsigned int groupindex, unsigned int index) { return 0; }
void network_free_string_ids() {}
void network_send_chat(const char* text) {}
void network_send_password(const char* password) {}
void network_close() {}
void network_shutdown_client() {}
void network_set_password(const char* password) {}
uint8 network_get_current_player_id() { return 0; }
int network_get_current_player_group_index() { return 0; }
#endif /* DISABLE_NETWORK */
