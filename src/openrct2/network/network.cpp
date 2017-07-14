#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../Context.h"
#include "../core/Guard.hpp"
#include "../OpenRCT2.h"
#include "../PlatformEnvironment.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"

extern "C" {
    #include "../platform/platform.h"
    #include "../util/sawyercoding.h"
}

#include "network.h"

#define ACTION_COOLDOWN_TIME_PLACE_SCENERY  20
#define ACTION_COOLDOWN_TIME_DEMOLISH_RIDE  1000

rct_peep* _pickup_peep = 0;
sint32 _pickup_peep_old_x = SPRITE_LOCATION_NULL;

#ifndef DISABLE_NETWORK

#include <cmath>
#include <cerrno>
#include <algorithm>
#include <set>
#include <string>

#include "../actions/GameAction.h"
#include "../core/Console.hpp"
#include "../core/FileStream.hpp"
#include "../core/Json.hpp"
#include "../core/Math.hpp"
#include "../core/MemoryStream.h"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../core/Util.hpp"
#include "../object/ObjectManager.h"
#include "../object/ObjectRepository.h"
#include "../ParkImporter.h"
#include "../rct2/S6Exporter.h"

extern "C" {
#include "../config/Config.h"
#include "../game.h"
#include "../interface/chat.h"
#include "../interface/window.h"
#include "../localisation/date.h"
#include "../localisation/localisation.h"
#include "../management/finance.h"
#include "../network/http.h"
#include "../scenario/scenario.h"
#include "../windows/error.h"
#include "../util/util.h"
#include "../cheats.h"

#include "NetworkAction.h"

#include <openssl/evp.h> // just for OpenSSL_add_all_algorithms()
}

#pragma comment(lib, "Ws2_32.lib")

using namespace OpenRCT2;

Network gNetwork;

enum {
    SERVER_EVENT_PLAYER_JOINED,
    SERVER_EVENT_PLAYER_DISCONNECTED,
};

void network_chat_show_connected_message();
void network_chat_show_server_greeting();
static void network_get_keys_directory(utf8 *buffer, size_t bufferSize);
static void network_get_private_key_path(utf8 *buffer, size_t bufferSize, const utf8 * playerName);
static void network_get_public_key_path(utf8 *buffer, size_t bufferSize, const utf8 * playerName, const utf8 * hash);

Network::Network()
{
    wsa_initialized = false;
    mode = NETWORK_MODE_NONE;
    status = NETWORK_STATUS_NONE;
    last_tick_sent_time = 0;
    last_ping_sent_time = 0;
    client_command_handlers.resize(NETWORK_COMMAND_MAX, 0);
    client_command_handlers[NETWORK_COMMAND_AUTH] = &Network::Client_Handle_AUTH;
    client_command_handlers[NETWORK_COMMAND_MAP] = &Network::Client_Handle_MAP;
    client_command_handlers[NETWORK_COMMAND_CHAT] = &Network::Client_Handle_CHAT;
    client_command_handlers[NETWORK_COMMAND_GAMECMD] = &Network::Client_Handle_GAMECMD;
    client_command_handlers[NETWORK_COMMAND_GAME_ACTION] = &Network::Client_Handle_GAME_ACTION;
    client_command_handlers[NETWORK_COMMAND_TICK] = &Network::Client_Handle_TICK;
    client_command_handlers[NETWORK_COMMAND_PLAYERLIST] = &Network::Client_Handle_PLAYERLIST;
    client_command_handlers[NETWORK_COMMAND_PING] = &Network::Client_Handle_PING;
    client_command_handlers[NETWORK_COMMAND_PINGLIST] = &Network::Client_Handle_PINGLIST;
    client_command_handlers[NETWORK_COMMAND_SETDISCONNECTMSG] = &Network::Client_Handle_SETDISCONNECTMSG;
    client_command_handlers[NETWORK_COMMAND_SHOWERROR] = &Network::Client_Handle_SHOWERROR;
    client_command_handlers[NETWORK_COMMAND_GROUPLIST] = &Network::Client_Handle_GROUPLIST;
    client_command_handlers[NETWORK_COMMAND_EVENT] = &Network::Client_Handle_EVENT;
    client_command_handlers[NETWORK_COMMAND_GAMEINFO] = &Network::Client_Handle_GAMEINFO;
    client_command_handlers[NETWORK_COMMAND_TOKEN] = &Network::Client_Handle_TOKEN;
    client_command_handlers[NETWORK_COMMAND_OBJECTS] = &Network::Client_Handle_OBJECTS;
    server_command_handlers.resize(NETWORK_COMMAND_MAX, 0);
    server_command_handlers[NETWORK_COMMAND_AUTH] = &Network::Server_Handle_AUTH;
    server_command_handlers[NETWORK_COMMAND_CHAT] = &Network::Server_Handle_CHAT;
    server_command_handlers[NETWORK_COMMAND_GAMECMD] = &Network::Server_Handle_GAMECMD;
    server_command_handlers[NETWORK_COMMAND_GAME_ACTION] = &Network::Server_Handle_GAME_ACTION;
    server_command_handlers[NETWORK_COMMAND_PING] = &Network::Server_Handle_PING;
    server_command_handlers[NETWORK_COMMAND_GAMEINFO] = &Network::Server_Handle_GAMEINFO;
    server_command_handlers[NETWORK_COMMAND_TOKEN] = &Network::Server_Handle_TOKEN;
    server_command_handlers[NETWORK_COMMAND_OBJECTS] = &Network::Server_Handle_OBJECTS;
    OpenSSL_add_all_algorithms();
}

Network::~Network()
{
    Close();
}

void Network::SetEnvironment(IPlatformEnvironment * env)
{
    _env = env;
}

bool Network::Init()
{
    if (!InitialiseWSA()) {
        return false;
    }

    status = NETWORK_STATUS_READY;

    server_connection = new NetworkConnection();
    ServerName = std::string();
    ServerDescription = std::string();
    ServerGreeting = std::string();
    ServerProviderName = std::string();
    ServerProviderEmail = std::string();
    ServerProviderWebsite = std::string();
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

    // HACK Because Close() is closed all over the place, it sometimes gets called inside an Update
    //      call. This then causes disposed data to be accessed. Therefore, save closing until the
    //      end of the update loop.
    if (_closeLock) {
        _requireClose = true;
        return;
    }

    if (mode == NETWORK_MODE_CLIENT) {
        delete server_connection->Socket;
        server_connection->Socket = nullptr;
    } else if (mode == NETWORK_MODE_SERVER) {
        delete listening_socket;
        listening_socket = nullptr;
        delete _advertiser;
        _advertiser = nullptr;
    }

    CloseChatLog();
    CloseServerLog();

    mode = NETWORK_MODE_NONE;
    status = NETWORK_STATUS_NONE;
    _lastConnectStatus = SOCKET_STATUS_CLOSED;
    server_connection->AuthStatus = NETWORK_AUTH_NONE;
    server_connection->InboundPacket.Clear();
    server_connection->SetLastDisconnectReason(nullptr);
    SafeDelete(server_connection);

    client_connection_list.clear();
    game_command_queue.clear();
    player_list.clear();
    group_list.clear();

    DisposeWSA();

    gfx_invalidate_screen();

    _requireClose = false;
}

bool Network::BeginClient(const char* host, uint16 port)
{
    if (GetMode() != NETWORK_MODE_NONE) {
        return false;
    }

    Close();
    if (!Init())
        return false;

    mode = NETWORK_MODE_CLIENT;

    assert(server_connection->Socket == nullptr);
    server_connection->Socket = CreateTcpSocket();
    server_connection->Socket->ConnectAsync(host, port);
    status = NETWORK_STATUS_CONNECTING;
    _lastConnectStatus = SOCKET_STATUS_CLOSED;

    BeginChatLog();
    BeginServerLog();

    utf8 keyPath[MAX_PATH];
    network_get_private_key_path(keyPath, sizeof(keyPath), gConfigNetwork.player_name);
    if (!platform_file_exists(keyPath)) {
        Console::WriteLine("Generating key... This may take a while");
        Console::WriteLine("Need to collect enough entropy from the system");
        _key.Generate();
        Console::WriteLine("Key generated, saving private bits as %s", keyPath);

        utf8 keysDirectory[MAX_PATH];
        network_get_keys_directory(keysDirectory, sizeof(keysDirectory));
        if (!platform_ensure_directory_exists(keysDirectory)) {
            log_error("Unable to create directory %s.", keysDirectory);
            return false;
        }

        try
        {
            auto fs = FileStream(keyPath, FILE_MODE_WRITE);
            _key.SavePrivate(&fs);
        }
        catch (Exception)
        {
            log_error("Unable to save private key at %s.", keyPath);
            return false;
        }

        const std::string hash = _key.PublicKeyHash();
        const utf8 *publicKeyHash = hash.c_str();
        network_get_public_key_path(keyPath, sizeof(keyPath), gConfigNetwork.player_name, publicKeyHash);
        Console::WriteLine("Key generated, saving public bits as %s", keyPath);

        try
        {
            auto fs = FileStream(keyPath, FILE_MODE_WRITE);
            _key.SavePublic(&fs);
        }
        catch (Exception)
        {
            log_error("Unable to save public key at %s.", keyPath);
            return false;
        }
    } else {
        // LoadPrivate returns validity of loaded key
        bool ok = false;
        try
        {
            log_verbose("Loading key from %s", keyPath);
            auto fs = FileStream(keyPath, FILE_MODE_OPEN);
            ok = _key.LoadPrivate(&fs);
        }
        catch (Exception)
        {
            log_error("Unable to read private key from %s.", keyPath);
            return false;
        }

        // Don't store private key in memory when it's not in use.
        _key.Unload();
        return ok;
    }

    return true;
}

bool Network::BeginServer(uint16 port, const char* address)
{
    Close();
    if (!Init())
        return false;

    mode = NETWORK_MODE_SERVER;

    _userManager.Load();

    if (strlen(address) == 0)
        address = nullptr;

    log_verbose("Begin listening for clients");

    assert(listening_socket == nullptr);
    listening_socket = CreateTcpSocket();
    try
    {
        listening_socket->Listen(address, port);
    }
    catch (const Exception &ex)
    {
        Console::Error::WriteLine(ex.GetMessage());
        Close();
        return false;
    }

    ServerName = String::ToStd(gConfigNetwork.server_name);
    ServerDescription = String::ToStd(gConfigNetwork.server_description);
    ServerGreeting = String::ToStd(gConfigNetwork.server_greeting);
    ServerProviderName = String::ToStd(gConfigNetwork.provider_name);
    ServerProviderEmail = String::ToStd(gConfigNetwork.provider_email);
    ServerProviderWebsite = String::ToStd(gConfigNetwork.provider_website);

    cheats_reset();
    LoadGroups();
    BeginChatLog();
    BeginServerLog();

    NetworkPlayer *player = AddPlayer(gConfigNetwork.player_name, "");
    player->Flags |= NETWORK_PLAYER_FLAG_ISSERVER;
    player->Group = 0;
    player_id = player->Id;

    if (network_get_mode() == NETWORK_MODE_SERVER) {
        // Add SERVER to users.json and save.
        NetworkUser *networkUser = _userManager.GetOrAddUser(player->KeyHash);
        networkUser->GroupId = player->Group;
        networkUser->Name = player->Name;
        _userManager.Save();
    }

    printf("Ready for clients...\n");
    network_chat_show_connected_message();
    network_chat_show_server_greeting();

    status = NETWORK_STATUS_CONNECTED;
    listening_port = port;
    if (gConfigNetwork.advertise) {
        _advertiser = CreateServerAdvertiser(listening_port);
    }

    return true;
}

sint32 Network::GetMode()
{
    return mode;
}

sint32 Network::GetStatus()
{
    return status;
}

sint32 Network::GetAuthStatus()
{
    if (GetMode() == NETWORK_MODE_CLIENT) {
        return server_connection->AuthStatus;
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
    _closeLock = true;

    switch (GetMode()) {
    case NETWORK_MODE_SERVER:
        UpdateServer();
        break;
    case NETWORK_MODE_CLIENT:
        UpdateClient();
        break;
    }

    // If the Close() was called during the update, close it for real
    _closeLock = false;
    if (_requireClose) {
        Close();
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

    uint32 ticks = platform_get_ticks();
    if (ticks > last_tick_sent_time + 25) {
        Server_Send_TICK();
    }
    if (ticks > last_ping_sent_time + 3000) {
        Server_Send_PING();
        Server_Send_PINGLIST();
    }

    if (_advertiser != nullptr) {
        _advertiser->Update();
    }

    ITcpSocket * tcpSocket = listening_socket->Accept();
    if (tcpSocket != nullptr) {
        AddClient(tcpSocket);
    }
}

void Network::UpdateClient()
{
    assert(server_connection != nullptr);

    switch(status){
    case NETWORK_STATUS_CONNECTING:
    {
        switch (server_connection->Socket->GetStatus()) {
        case SOCKET_STATUS_RESOLVING:
        {
            if (_lastConnectStatus != SOCKET_STATUS_RESOLVING)
            {
                _lastConnectStatus = SOCKET_STATUS_RESOLVING;
                char str_resolving[256];
                format_string(str_resolving, 256, STR_MULTIPLAYER_RESOLVING, NULL);
                window_network_status_open(str_resolving, []() -> void {
                    gNetwork.Close();
                });
            }
            break;
        }
        case SOCKET_STATUS_CONNECTING:
        {
            if (_lastConnectStatus != SOCKET_STATUS_CONNECTING)
            {
                _lastConnectStatus = SOCKET_STATUS_CONNECTING;
                char str_connecting[256];
                format_string(str_connecting, 256, STR_MULTIPLAYER_CONNECTING, NULL);
                window_network_status_open(str_connecting, []() -> void {
                    gNetwork.Close();
                });
                server_connect_time = platform_get_ticks();
            }
            break;
        }
        case SOCKET_STATUS_CONNECTED:
        {
            status = NETWORK_STATUS_CONNECTED;
            server_connection->ResetLastPacketTime();
            Client_Send_TOKEN();
            char str_authenticating[256];
            format_string(str_authenticating, 256, STR_MULTIPLAYER_AUTHENTICATING, NULL);
            window_network_status_open(str_authenticating, []() -> void {
                gNetwork.Close();
            });
            break;
        }
        default:
        {
            const char * error = server_connection->Socket->GetError();
            if (error != nullptr) {
                Console::Error::WriteLine(error);
            }

            Close();
            window_network_status_close();
            window_error_open(STR_UNABLE_TO_CONNECT_TO_SERVER, STR_NONE);
            break;
        }
        }
        break;
    }
    case NETWORK_STATUS_CONNECTED:
    {
        if (!ProcessConnection(*server_connection)) {
            // Do not show disconnect message window when password window closed/canceled
            if (server_connection->AuthStatus == NETWORK_AUTH_REQUIREPASSWORD) {
                window_network_status_close();
            } else {
                char str_disconnected[256];

                if (server_connection->GetLastDisconnectReason()) {
                    const char * disconnect_reason = server_connection->GetLastDisconnectReason();
                    format_string(str_disconnected, 256, STR_MULTIPLAYER_DISCONNECTED_WITH_REASON, &disconnect_reason);
                } else {
                    format_string(str_disconnected, 256, STR_MULTIPLAYER_DISCONNECTED_NO_REASON, NULL);
                }

                window_network_status_open(str_disconnected, NULL);
            }
            Close();
        }
        break;
    }
    }
}

std::vector<std::unique_ptr<NetworkPlayer>>::iterator Network::GetPlayerIteratorByID(uint8 id)
{
    auto it = std::find_if(player_list.begin(), player_list.end(), [&id](std::unique_ptr<NetworkPlayer> const& player) { return player->Id == id; });
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
    auto it = std::find_if(group_list.begin(), group_list.end(), [&id](std::unique_ptr<NetworkGroup> const& group) { return group->Id == id; });
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
        safe_strcpy(lineCh, (const char *) fromplayer->Name.c_str(), sizeof(formatted) - (lineCh - formatted));
        safe_strcat(lineCh, ": ", sizeof(formatted) - (lineCh - formatted));
        lineCh = strchr(lineCh, '\0');
    }
    lineCh = utf8_write_codepoint(lineCh, FORMAT_OUTLINE);
    lineCh = utf8_write_codepoint(lineCh, FORMAT_WHITE);
    char* ptrtext = lineCh;
    safe_strcpy(lineCh, text, 800);
    utf8_remove_format_codes((utf8*)ptrtext, true);
    return formatted;
}

void Network::SendPacketToClients(NetworkPacket& packet, bool front, bool gameCmd)
{
    for (auto it = client_connection_list.begin(); it != client_connection_list.end(); it++) {

        if (gameCmd) {
            // If marked as game command we can not send the packet to connections that are not fully connected.
            // Sending the packet would cause the client to store a command that is behind the tick where he starts,
            // which would be essentially never executed. The clients do not require commands before the server has not sent the map data.
            if ((*it)->Player == nullptr) {
                continue;
            }
        }
        (*it)->QueuePacket(NetworkPacket::Duplicate(packet), front);
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

    if (game_commands_processed_this_tick != 0) {
        // SRAND/sprite hash is only updated once at beginning of tick so it is invalid otherwise
        return true;
    }

    if (tick == server_srand0_tick)
    {
        server_srand0_tick = 0;
        // Check that the server and client sprite hashes match
        const char *client_sprite_hash = sprite_checksum();
        const bool sprites_mismatch = server_sprite_hash[0] != '\0' && strcmp(client_sprite_hash, server_sprite_hash);
        // Check PRNG values and sprite hashes, if exist
        if ((srand0 != server_srand0) || sprites_mismatch) {
#ifdef DEBUG_DESYNC
            dbg_report_desync(tick, srand0, server_srand0, client_sprite_hash, server_sprite_hash);
#endif
            return false;
        }
    }
    return true;
}

void Network::KickPlayer(sint32 playerId)
{
    for(auto it = client_connection_list.begin(); it != client_connection_list.end(); it++) {
        if ((*it)->Player->Id == playerId) {
            // Disconnect the client gracefully
            (*it)->SetLastDisconnectReason(STR_MULTIPLAYER_KICKED);
            char str_disconnect_msg[256];
            format_string(str_disconnect_msg, 256, STR_MULTIPLAYER_KICKED_REASON, NULL);
            Server_Send_SETDISCONNECTMSG(*(*it), str_disconnect_msg);
            (*it)->Socket->Disconnect();
            break;
        }
    }
}

void Network::SetPassword(const char* password)
{
    _password = password == nullptr ? "" : password;
}

void Network::ShutdownClient()
{
    if (GetMode() == NETWORK_MODE_CLIENT) {
        server_connection->Socket->Disconnect();
    }
}

std::string Network::GenerateAdvertiseKey()
{
    // Generate a string of 16 random hex characters (64-integer key as a hex formatted string)
    static char hexChars[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
    char key[17];
    for (sint32 i = 0; i < 16; i++) {
        sint32 hexCharIndex = util_rand() % Util::CountOf(hexChars);
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

NetworkGroup* Network::AddGroup()
{
    NetworkGroup* addedgroup = nullptr;
    sint32 newid = -1;
    // Find first unused group id
    for (sint32 id = 0; id < 255; id++) {
        if (std::find_if(group_list.begin(), group_list.end(), [&id](std::unique_ptr<NetworkGroup> const& group) {
                         return group->Id == id;
            }) == group_list.end()) {
            newid = id;
            break;
        }
    }
    if (newid != -1) {
        std::unique_ptr<NetworkGroup> group(new NetworkGroup); // change to make_unique in c++14
        group->Id = newid;
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

    if (GetMode() == NETWORK_MODE_SERVER) {
        _userManager.UnsetUsersOfGroup(id);
        _userManager.Save();
    }
}

uint8 Network::GetGroupIDByHash(const std::string &keyhash)
{
    const NetworkUser * networkUser = _userManager.GetUserByHash(keyhash);

    uint8 groupId = GetDefaultGroup();
    if (networkUser != nullptr && networkUser->GroupId.HasValue()) {
        const uint8 assignedGroup = networkUser->GroupId.GetValue();
        if (GetGroupByID(assignedGroup) != nullptr) {
            groupId = assignedGroup;
        } else {
            log_warning("User %s is assigned to non-existent group %u. Assigning to default group (%u)",
                        keyhash.c_str(), assignedGroup, groupId);
        }
    }
    return groupId;
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

        platform_get_user_directory(path, NULL, sizeof(path));
        safe_strcat_path(path, "groups.json", sizeof(path));

        json_t * jsonGroupsCfg = json_object();
        json_t * jsonGroups = json_array();
        for (auto it = group_list.begin(); it != group_list.end(); it++) {
            json_array_append_new(jsonGroups, (*it)->ToJson());
        }
        json_object_set_new(jsonGroupsCfg, "default_group", json_integer(default_group));
        json_object_set_new(jsonGroupsCfg, "groups", jsonGroups);
        try
        {
            Json::WriteToFile(path, jsonGroupsCfg, JSON_INDENT(4) | JSON_PRESERVE_ORDER);
        }
        catch (const Exception &ex)
        {
            log_error("Unable to save %s: %s", path, ex.GetMessage());
        }

        json_decref(jsonGroupsCfg);
    }
}

void Network::SetupDefaultGroups()
{
    std::unique_ptr<NetworkGroup> admin(new NetworkGroup()); // change to make_unique in c++14
    admin->SetName("Admin");
    admin->ActionsAllowed.fill(0xFF);
    admin->Id = 0;
    group_list.push_back(std::move(admin));
    std::unique_ptr<NetworkGroup> spectator(new NetworkGroup()); // change to make_unique in c++14
    spectator->SetName("Spectator");
    spectator->ToggleActionPermission(0); // Chat
    spectator->Id = 1;
    group_list.push_back(std::move(spectator));
    std::unique_ptr<NetworkGroup> user(new NetworkGroup()); // change to make_unique in c++14
    user->SetName("User");
    user->ActionsAllowed.fill(0xFF);
    user->ToggleActionPermission(15); // Kick Player
    user->ToggleActionPermission(16); // Modify Groups
    user->ToggleActionPermission(17); // Set Player Group
    user->ToggleActionPermission(18); // Cheat
    user->ToggleActionPermission(20); // Passwordless login
    user->ToggleActionPermission(21); // Modify Tile
    user->ToggleActionPermission(22); // Edit Scenario Options
    user->Id = 2;
    group_list.push_back(std::move(user));
    SetDefaultGroup(1);
}

void Network::LoadGroups()
{
    group_list.clear();

    utf8 path[MAX_PATH];

    platform_get_user_directory(path, NULL, sizeof(path));
    safe_strcat_path(path, "groups.json", sizeof(path));

    json_t * json = nullptr;
    if (platform_file_exists(path)) {
        try {
            json = Json::ReadFromFile(path);
        } catch (const Exception &e) {
            log_error("Failed to read %s as JSON. Setting default groups. %s", path, e.GetMessage());
        }
    }

    if (json == nullptr) {
        SetupDefaultGroups();
    } else {
        json_t * json_groups = json_object_get(json, "groups");
        size_t groupCount = (size_t)json_array_size(json_groups);
        for (size_t i = 0; i < groupCount; i++) {
            json_t * jsonGroup = json_array_get(json_groups, i);

            std::unique_ptr<NetworkGroup> newgroup(new NetworkGroup(NetworkGroup::FromJson(jsonGroup))); // change to make_unique in c++14
            group_list.push_back(std::move(newgroup));
        }
        json_t * jsonDefaultGroup = json_object_get(json, "default_group");
        default_group = (uint8)json_integer_value(jsonDefaultGroup);
        if (GetGroupByID(default_group) == nullptr) {
            default_group = 0;
        }
        json_decref(json);
    }

    // Host group should always contain all permissions.
    group_list.at(0)->ActionsAllowed.fill(0xFF);
}

std::string Network::BeginLog(const std::string &directory, const std::string &filenameFormat)
{
    utf8 filename[256];
    time_t timer;
    time(&timer);
    auto tmInfo = localtime(&timer);
    if (strftime(filename, sizeof(filename), filenameFormat.c_str(), tmInfo) == 0) {
        throw std::runtime_error("strftime failed");
    }

    return Path::Combine(directory, filename);
}

void Network::AppendLog(const std::string &logPath, const std::string &s)
{
    std::string directory = Path::GetDirectory(logPath);
    if (platform_ensure_directory_exists(directory.c_str())) {
        try
        {
            auto fs = FileStream(logPath, FILE_MODE_APPEND);

            utf8 buffer[256];
            time_t timer;
            time(&timer);
            auto tmInfo = localtime(&timer);
            if (strftime(buffer, sizeof(buffer), "[%Y/%m/%d %H:%M:%S] ", tmInfo) != 0) {
                String::Append(buffer, sizeof(buffer), s.c_str());
                utf8_remove_formatting(buffer, false);
                String::Append(buffer, sizeof(buffer), PLATFORM_NEWLINE);

                fs.Write(buffer, strlen(buffer));
            }
        }
        catch (const Exception &ex)
        {
            log_error("%s", ex.GetMessage());
        }
    }
}

void Network::BeginChatLog()
{
    auto directory = _env->GetDirectoryPath(DIRBASE::USER, DIRID::LOG_CHAT);
    _chatLogPath = BeginLog(directory, _chatLogFilenameFormat);
}

void Network::AppendChatLog(const std::string &s)
{
    if (gConfigNetwork.log_chat) {
        AppendLog(_chatLogPath, s);
    }
}

void Network::CloseChatLog()
{
}

void Network::BeginServerLog()
{
    auto directory = _env->GetDirectoryPath(DIRBASE::USER, DIRID::LOG_SERVER);
    _serverLogPath = BeginLog(directory, (ServerName + _serverLogFilenameFormat));

    // Log server start event
    utf8 logMessage[256];
    if (GetMode() == NETWORK_MODE_CLIENT) {
        format_string(logMessage, sizeof(logMessage), STR_LOG_CLIENT_STARTED, NULL);
    } else if (GetMode() == NETWORK_MODE_SERVER) {
        format_string(logMessage, sizeof(logMessage), STR_LOG_SERVER_STARTED, NULL);
    }
    AppendServerLog(logMessage);
}

void Network::AppendServerLog(const std::string &s)
{
    if (gConfigNetwork.log_server_actions) {
        AppendLog(_serverLogPath.c_str(), s);
    }
}

void Network::CloseServerLog()
{
    // Log server stopped event
    char logMessage[256];
    if (GetMode() == NETWORK_MODE_CLIENT) {
        format_string(logMessage, sizeof(logMessage), STR_LOG_CLIENT_STOPPED, NULL);
    } else if (GetMode() == NETWORK_MODE_SERVER) {
        format_string(logMessage, sizeof(logMessage), STR_LOG_SERVER_STOPPED, NULL);
    }
    AppendServerLog(logMessage);
}

void Network::Client_Send_TOKEN()
{
    log_verbose("requesting token");
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32)NETWORK_COMMAND_TOKEN;
    server_connection->AuthStatus = NETWORK_AUTH_REQUESTED;
    server_connection->QueuePacket(std::move(packet));
}

void Network::Client_Send_AUTH(const char* name, const char* password, const char* pubkey, const char *sig, size_t sigsize)
{
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32)NETWORK_COMMAND_AUTH;
    packet->WriteString(NETWORK_STREAM_ID);
    packet->WriteString(name);
    packet->WriteString(password);
    packet->WriteString(pubkey);
    assert(sigsize <= (size_t)UINT32_MAX);
    *packet << (uint32)sigsize;
    packet->Write((const uint8 *)sig, sigsize);
    server_connection->AuthStatus = NETWORK_AUTH_REQUESTED;
    server_connection->QueuePacket(std::move(packet));
}

void Network::Client_Send_OBJECTS(const std::vector<std::string> &objects)
{
    log_verbose("client requests %u objects", uint32(objects.size()));
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32)NETWORK_COMMAND_OBJECTS << (uint32)objects.size();
    for (uint32 i = 0; i < objects.size(); i++)
    {
        log_verbose("client requests object %s", objects[i].c_str());
        packet->Write((const uint8 *)objects[i].c_str(), 8);
    }
    server_connection->QueuePacket(std::move(packet));
}

void Network::Server_Send_TOKEN(NetworkConnection& connection)
{
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32)NETWORK_COMMAND_TOKEN << (uint32)connection.Challenge.size();
    packet->Write(connection.Challenge.data(), connection.Challenge.size());
    connection.QueuePacket(std::move(packet));
}

void Network::Server_Send_OBJECTS(NetworkConnection& connection, const std::vector<const ObjectRepositoryItem *> &objects) const
{
    log_verbose("Server sends objects list with %u items", objects.size());
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32)NETWORK_COMMAND_OBJECTS << (uint32)objects.size();
    for (size_t i = 0; i < objects.size(); i++)
    {
        log_verbose("Object %.8s (checksum %x)", objects[i]->ObjectEntry.name, objects[i]->ObjectEntry.checksum);
        packet->Write((const uint8 *)objects[i]->ObjectEntry.name, 8);
        *packet << objects[i]->ObjectEntry.checksum << objects[i]->ObjectEntry.flags;
    }
    connection.QueuePacket(std::move(packet));
}

void Network::Server_Send_AUTH(NetworkConnection& connection)
{
    uint8 new_playerid = 0;
    if (connection.Player) {
        new_playerid = connection.Player->Id;
    }
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32)NETWORK_COMMAND_AUTH << (uint32)connection.AuthStatus << (uint8)new_playerid;
    if (connection.AuthStatus == NETWORK_AUTH_BADVERSION) {
        packet->WriteString(NETWORK_STREAM_ID);
    }
    connection.QueuePacket(std::move(packet));
    if (connection.AuthStatus != NETWORK_AUTH_OK && connection.AuthStatus != NETWORK_AUTH_REQUIREPASSWORD) {
        connection.SendQueuedPackets();
        connection.Socket->Disconnect();
    }
}

void Network::Server_Send_MAP(NetworkConnection* connection)
{
    std::vector<const ObjectRepositoryItem *> objects;
    if (connection) {
        objects = connection->RequestedObjects;
    } else {
        // This will send all custom objects to connected clients
        // TODO: fix it so custom objects negotiation is performed even in this case.
        IObjectManager * objManager = GetObjectManager();
        objects = objManager->GetPackableObjects();
    }

    size_t out_size;
    uint8 * header = save_for_network(out_size, objects);
    if (header == nullptr) {
        if (connection) {
            connection->SetLastDisconnectReason(STR_MULTIPLAYER_CONNECTION_CLOSED);
            connection->Socket->Disconnect();
        }
        return;
    }
    size_t chunksize = 65000;
    for (size_t i = 0; i < out_size; i += chunksize) {
        size_t datasize = Math::Min(chunksize, out_size - i);
        std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
        *packet << (uint32)NETWORK_COMMAND_MAP << (uint32)out_size << (uint32)i;
        packet->Write(&header[i], datasize);
        if (connection) {
            connection->QueuePacket(std::move(packet));
        } else {
            SendPacketToClients(*packet);
        }
    }
    free(header);
}

uint8 * Network::save_for_network(size_t &out_size, const std::vector<const ObjectRepositoryItem *> &objects) const
{
    uint8 * header = nullptr;
    out_size = 0;
    bool RLEState = gUseRLE;
    gUseRLE = false;

    auto ms = MemoryStream();
    if (!SaveMap(&ms, objects)) {
        log_warning("Failed to export map.");
        return nullptr;
    }
    gUseRLE = RLEState;

    const void * data = ms.GetData();
    sint32 size = ms.GetLength();

    uint8 *compressed = util_zlib_deflate((const uint8 *)data, size, &out_size);
    if (compressed != NULL)
    {
        header = (uint8 *)_strdup("open2_sv6_zlib");
        size_t header_len = strlen((char *)header) + 1; // account for null terminator
        header = (uint8 *)realloc(header, header_len + out_size);
        if (header == nullptr) {
            log_error("Failed to allocate %u bytes.", header_len + out_size);
        } else {
            memcpy(&header[header_len], compressed, out_size);
            out_size += header_len;
            log_verbose("Sending map of size %u bytes, compressed to %u bytes", size, out_size);
        }
        free(compressed);
    } else {
        log_warning("Failed to compress the data, falling back to non-compressed sv6.");
        header = (uint8 *)malloc(size);
        if (header == nullptr) {
            log_error("Failed to allocate %u bytes.", size);
        } else {
            out_size = size;
            memcpy(header, data, size);
        }
    }
    return header;
}

void Network::Client_Send_CHAT(const char* text)
{
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32)NETWORK_COMMAND_CHAT;
    packet->WriteString(text);
    server_connection->QueuePacket(std::move(packet));
}

void Network::Server_Send_CHAT(const char* text)
{
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32)NETWORK_COMMAND_CHAT;
    packet->WriteString(text);
    SendPacketToClients(*packet);
}

void Network::Client_Send_GAMECMD(uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp, uint8 callback)
{
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32)NETWORK_COMMAND_GAMECMD << (uint32)gCurrentTicks << eax << (ebx | GAME_COMMAND_FLAG_NETWORKED)
            << ecx << edx << esi << edi << ebp << callback;
    server_connection->QueuePacket(std::move(packet));
}

void Network::Server_Send_GAMECMD(uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp, uint8 playerid, uint8 callback)
{
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32)NETWORK_COMMAND_GAMECMD << (uint32)gCurrentTicks << eax << (ebx | GAME_COMMAND_FLAG_NETWORKED)
            << ecx << edx << esi << edi << ebp << playerid << callback;
    SendPacketToClients(*packet, false, true);
}

void Network::Client_Send_GAME_ACTION(const IGameAction *action, uint32 flags = 0)
{
	std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
	*packet << (uint32)NETWORK_COMMAND_GAME_ACTION << (uint32)gCurrentTicks << action->GetType() << flags;
	MemoryStream stream;
	action->Serialise(&stream);
	packet->Write((uint8*)stream.GetData(), stream.GetLength());
	server_connection->QueuePacket(std::move(packet));
}

void Network::Server_Send_GAME_ACTION(const IGameAction *action, uint32 flags = 0)
{
	std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
	*packet << (uint32)NETWORK_COMMAND_GAME_ACTION << (uint32)gCurrentTicks << action->GetType() << gNetwork.GetPlayerID() << flags;
	MemoryStream stream;
	action->Serialise(&stream);
	packet->Write((uint8*)stream.GetData(), stream.GetLength());
	SendPacketToClients(*packet);
}

void Network::Server_Send_TICK()
{
    last_tick_sent_time = platform_get_ticks();
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32)NETWORK_COMMAND_TICK << (uint32)gCurrentTicks << (uint32)gScenarioSrand0;
    uint32 flags = 0;
    // Simple counter which limits how often a sprite checksum gets sent.
    // This can get somewhat expensive, so we don't want to push it every tick in release,
    // but debug version can check more often.
    static sint32 checksum_counter = 0;
    checksum_counter++;
    if (checksum_counter >= 100) {
        checksum_counter = 0;
        flags |= NETWORK_TICK_FLAG_CHECKSUMS;
    }
    // Send flags always, so we can understand packet structure on the other end,
    // and allow for some expansion.
    *packet << flags;
    if (flags & NETWORK_TICK_FLAG_CHECKSUMS) {
        packet->WriteString(sprite_checksum());
    }
    SendPacketToClients(*packet);
}

void Network::Server_Send_PLAYERLIST()
{
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32)NETWORK_COMMAND_PLAYERLIST << (uint8)player_list.size();
    for (uint32 i = 0; i < player_list.size(); i++) {
        player_list[i]->Write(*packet);
    }
    SendPacketToClients(*packet);
}

void Network::Client_Send_PING()
{
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32)NETWORK_COMMAND_PING;
    server_connection->QueuePacket(std::move(packet));
}

void Network::Server_Send_PING()
{
    last_ping_sent_time = platform_get_ticks();
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32)NETWORK_COMMAND_PING;
    for (auto it = client_connection_list.begin(); it != client_connection_list.end(); it++) {
        (*it)->PingTime = platform_get_ticks();
    }
    SendPacketToClients(*packet, true);
}

void Network::Server_Send_PINGLIST()
{
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32)NETWORK_COMMAND_PINGLIST << (uint8)player_list.size();
    for (size_t i = 0; i < player_list.size(); i++) {
        *packet << player_list[i]->Id << player_list[i]->Ping;
    }
    SendPacketToClients(*packet);
}

void Network::Server_Send_SETDISCONNECTMSG(NetworkConnection& connection, const char* msg)
{
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32)NETWORK_COMMAND_SETDISCONNECTMSG;
    packet->WriteString(msg);
    connection.QueuePacket(std::move(packet));
    connection.SendQueuedPackets();
}

void Network::Server_Send_GAMEINFO(NetworkConnection& connection)
{
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32)NETWORK_COMMAND_GAMEINFO;
#ifndef DISABLE_HTTP
    json_t* obj = json_object();
    json_object_set_new(obj, "name", json_string(gConfigNetwork.server_name));
    json_object_set_new(obj, "requiresPassword", json_boolean(_password.size() > 0));
    json_object_set_new(obj, "version", json_string(NETWORK_STREAM_ID));
    json_object_set_new(obj, "players", json_integer(player_list.size()));
    json_object_set_new(obj, "maxPlayers", json_integer(gConfigNetwork.maxplayers));
    json_object_set_new(obj, "description", json_string(gConfigNetwork.server_description));
    json_object_set_new(obj, "greeting", json_string(gConfigNetwork.server_greeting));
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
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32)NETWORK_COMMAND_SHOWERROR << title << message;
    connection.QueuePacket(std::move(packet));
}

void Network::Server_Send_GROUPLIST(NetworkConnection& connection)
{
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32)NETWORK_COMMAND_GROUPLIST << (uint8)group_list.size() << default_group;
    for (uint32 i = 0; i < group_list.size(); i++) {
        group_list[i]->Write(*packet);
    }
    connection.QueuePacket(std::move(packet));
}

void Network::Server_Send_EVENT_PLAYER_JOINED(const char *playerName)
{
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32)NETWORK_COMMAND_EVENT;
    *packet << (uint16)SERVER_EVENT_PLAYER_JOINED;
    packet->WriteString(playerName);
    SendPacketToClients(*packet);
}

void Network::Server_Send_EVENT_PLAYER_DISCONNECTED(const char *playerName, const char *reason)
{
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32)NETWORK_COMMAND_EVENT;
    *packet << (uint16)SERVER_EVENT_PLAYER_DISCONNECTED;
    packet->WriteString(playerName);
    packet->WriteString(reason);
    SendPacketToClients(*packet);
}

bool Network::ProcessConnection(NetworkConnection& connection)
{
    sint32 packetStatus;
    do {
        packetStatus = connection.ReadPacket();
        switch(packetStatus) {
        case NETWORK_READPACKET_DISCONNECTED:
            // closed connection or network error
            if (!connection.GetLastDisconnectReason()) {
                connection.SetLastDisconnectReason(STR_MULTIPLAYER_CONNECTION_CLOSED);
            }
            return false;
            break;
        case NETWORK_READPACKET_SUCCESS:
            // done reading in packet
            ProcessPacket(connection, connection.InboundPacket);
            if (connection.Socket == nullptr) {
                return false;
            }
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
        if (!connection.GetLastDisconnectReason()) {
            connection.SetLastDisconnectReason(STR_MULTIPLAYER_NO_DATA);
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
                if (connection.AuthStatus == NETWORK_AUTH_OK || !packet.CommandRequiresAuth()) {
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
    while (game_command_queue.begin() != game_command_queue.end()) {

        // run all the game commands at the current tick
        const GameCommand& gc = (*game_command_queue.begin());

        if (mode == NETWORK_MODE_CLIENT) {

            if (game_command_queue.begin()->tick < gCurrentTicks) {
                // Having old command from a tick where we have not been active yet or malicious server,
                // the command is useless so lets not keep it.
                game_command_queue.erase(game_command_queue.begin());

                log_warning("Discarding game command from tick behind current tick, CMD: %08X, CMD Tick: %08X, Current Tick: %08X\n",
                            gc.esi,
                            gc.tick,
                            gCurrentTicks);

                // At this point we should not return, would add the possibility to skip commands this tick.
                continue;
            }

            // exit the game command processing loop to still have a chance at finding desync.
            if (game_command_queue.begin()->tick != gCurrentTicks)
                break;
        }
        if (gc.actionType != UINT32_MAX) {
            IGameAction * action = GameActions::Create(gc.actionType);
            uint32 flags = gc.parameters->ReadValue<uint32>();
            action->Deserialise(gc.parameters);
            GameActionResult result = GameActions::Execute(action, flags | GAME_COMMAND_FLAG_NETWORKED);
            if (result.Error != GA_ERROR::OK)
            {
                game_commands_processed_this_tick++;
                NetworkPlayer* player = GetPlayerByID(gc.playerid);
                if (player) {
                    player->LastAction = NetworkActions::FindCommand(gc.actionType);
                    player->LastActionTime = platform_get_ticks();
                    player->AddMoneySpent(result.Cost);
                }
            }
        }
        else {
            if (GetPlayerID() == gc.playerid) {
                game_command_callback = game_command_callback_get_callback(gc.callback);
            }

            game_command_playerid = gc.playerid;

            sint32 command = gc.esi;
            sint32 flags = gc.ebx;
            if (mode == NETWORK_MODE_SERVER)
                flags |= GAME_COMMAND_FLAG_NETWORKED;

            money32 cost = game_do_command(gc.eax, flags, gc.ecx, gc.edx, gc.esi, gc.edi, gc.ebp);

            if (cost != MONEY32_UNDEFINED)
            {
                game_commands_processed_this_tick++;
                NetworkPlayer* player = GetPlayerByID(gc.playerid);
                if (!player)
                    return;

                player->LastAction = NetworkActions::FindCommand(command);
                player->LastActionTime = platform_get_ticks();
                player->AddMoneySpent(cost);

                if (mode == NETWORK_MODE_SERVER) {

                    if (command == GAME_COMMAND_PLACE_SCENERY) {
                        player->LastPlaceSceneryTime = player->LastActionTime;
                    }
                    else if (command == GAME_COMMAND_DEMOLISH_RIDE) {
                        player->LastDemolishRideTime = player->LastActionTime;
                    }

                    Server_Send_GAMECMD(gc.eax, gc.ebx, gc.ecx, gc.edx, gc.esi, gc.edi, gc.ebp, gc.playerid, gc.callback);
                }
            }
        }
        game_command_queue.erase(game_command_queue.begin());
    }

    // Check synchronisation
    if (mode == NETWORK_MODE_CLIENT && !_desynchronised && !CheckSRAND(gCurrentTicks, gScenarioSrand0)) {
        _desynchronised = true;

        char str_desync[256];
        format_string(str_desync, 256, STR_MULTIPLAYER_DESYNC, NULL);
        window_network_status_open(str_desync, NULL);
        if (!gConfigNetwork.stay_connected) {
            Close();
        }
    }

    if (mode == NETWORK_MODE_SERVER)
    {
        for (const auto& it : client_connection_list)
        {
            it->SendQueuedPackets();
        }
    }
}

void Network::AddClient(ITcpSocket * socket)
{
    auto connection = std::unique_ptr<NetworkConnection>(new NetworkConnection);  // change to make_unique in c++14
    connection->Socket = socket;
    char addr[128];
    snprintf(addr, sizeof(addr), "Client joined from %s", socket->GetHostName());
    AppendServerLog(addr);
    client_connection_list.push_back(std::move(connection));
}

void Network::RemoveClient(std::unique_ptr<NetworkConnection>& connection)
{
    NetworkPlayer* connection_player = connection->Player;
    if (connection_player) {
        char text[256];
        const char * has_disconnected_args[2] = {
                (char *) connection_player->Name.c_str(),
                connection->GetLastDisconnectReason()
        };
        if (has_disconnected_args[1]) {
            format_string(text, 256, STR_MULTIPLAYER_PLAYER_HAS_DISCONNECTED_WITH_REASON, has_disconnected_args);
        } else {
            format_string(text, 256, STR_MULTIPLAYER_PLAYER_HAS_DISCONNECTED_NO_REASON, &(has_disconnected_args[0]));
        }

        chat_history_add(text);
        rct_peep* pickup_peep = network_get_pickup_peep(connection_player->Id);
        if(pickup_peep) {
            game_command_playerid = connection_player->Id;
            game_do_command(pickup_peep->sprite_index, GAME_COMMAND_FLAG_APPLY, 1, 0, pickup_peep->type == PEEP_TYPE_GUEST ? GAME_COMMAND_PICKUP_GUEST : GAME_COMMAND_PICKUP_STAFF, network_get_pickup_peep_old_x(connection_player->Id), 0);
        }
        gNetwork.Server_Send_EVENT_PLAYER_DISCONNECTED((char*)connection_player->Name.c_str(), connection->GetLastDisconnectReason());

        // Log player disconnected event
        AppendServerLog(text);
    }
    player_list.erase(std::remove_if(player_list.begin(), player_list.end(), [connection_player](std::unique_ptr<NetworkPlayer>& player){
                          return player.get() == connection_player;
                      }), player_list.end());
    client_connection_list.remove(connection);
    Server_Send_PLAYERLIST();
}

NetworkPlayer* Network::AddPlayer(const utf8 *name, const std::string &keyhash)
{
    NetworkPlayer* addedplayer = nullptr;
    sint32 newid = -1;
    if (GetMode() == NETWORK_MODE_SERVER) {
        // Find first unused player id
        for (sint32 id = 0; id < 255; id++) {
            if (std::find_if(player_list.begin(), player_list.end(), [&id](std::unique_ptr<NetworkPlayer> const& player) {
                             return player->Id == id;
                }) == player_list.end()) {
                newid = id;
                break;
            }
        }
    } else {
        newid = 0;
    }
    if (newid != -1) {
        std::unique_ptr<NetworkPlayer> player;
        if (GetMode() == NETWORK_MODE_SERVER) {
            // Load keys host may have added manually
            _userManager.Load();

            // Check if the key is registered
            const NetworkUser * networkUser = _userManager.GetUserByHash(keyhash);

            player = std::unique_ptr<NetworkPlayer>(new NetworkPlayer); // change to make_unique in c++14
            player->Id = newid;
            player->KeyHash = keyhash;
            if (networkUser == nullptr) {
                player->Group = GetDefaultGroup();
                if (!String::IsNullOrEmpty(name)) {
                    player->SetName(MakePlayerNameUnique(std::string(name)));
                }
            } else {
                player->Group = networkUser->GroupId.GetValueOrDefault(GetDefaultGroup());
                player->SetName(networkUser->Name);
            }
        } else {
            player = std::unique_ptr<NetworkPlayer>(new NetworkPlayer); // change to make_unique in c++14
            player->Id = newid;
            player->Group = GetDefaultGroup();
            player->SetName(name);
        }

        addedplayer = player.get();
        player_list.push_back(std::move(player));
    }
    return addedplayer;

}

std::string Network::MakePlayerNameUnique(const std::string &name)
{
    // Note: Player names are case-insensitive

    std::string new_name = name.substr(0, 31);
    sint32 counter = 1;
    bool unique;
    do {
        unique = true;

        // Check if there is already a player with this name in the server
        for (const auto &player : player_list) {
            if (String::Equals(player->Name.c_str(), new_name.c_str(), true)) {
                unique = false;
                break;
            }
        }

        if (unique) {
            // Check if there is already a registered player with this name
            if (_userManager.GetUserByName(new_name) != nullptr) {
                unique = false;
            }
        }

        if (!unique) {
            // Increment name counter
            counter++;
            new_name = name.substr(0, 31) + " #" + std::to_string(counter);
        }
    } while (!unique);
    return new_name;
}

void Network::Client_Handle_TOKEN(NetworkConnection& connection, NetworkPacket& packet)
{
    utf8 keyPath[MAX_PATH];
    network_get_private_key_path(keyPath, sizeof(keyPath), gConfigNetwork.player_name);
    if (!platform_file_exists(keyPath)) {
        log_error("Key file (%s) was not found. Restart client to re-generate it.", keyPath);
        return;
    }

    try
    {
        auto fs = FileStream(keyPath, FILE_MODE_OPEN);
        if (!_key.LoadPrivate(&fs))
        {
            throw Exception();
        }
    }
    catch (Exception)
    {
        log_error("Failed to load key %s", keyPath);
        connection.SetLastDisconnectReason(STR_MULTIPLAYER_VERIFICATION_FAILURE);
        connection.Socket->Disconnect();
        return;
    }

    uint32 challenge_size;
    packet >> challenge_size;
    const char *challenge = (const char *)packet.Read(challenge_size);
    size_t sigsize;
    char *signature;
    const std::string pubkey = _key.PublicKeyString();
    _challenge.resize(challenge_size);
    memcpy(_challenge.data(), challenge, challenge_size);
    bool ok = _key.Sign(_challenge.data(), _challenge.size(), &signature, &sigsize);
    if (!ok) {
        log_error("Failed to sign server's challenge.");
        connection.SetLastDisconnectReason(STR_MULTIPLAYER_VERIFICATION_FAILURE);
        connection.Socket->Disconnect();
        return;
    }
    // Don't keep private key in memory. There's no need and it may get leaked
    // when process dump gets collected at some point in future.
    _key.Unload();
    Client_Send_AUTH(gConfigNetwork.player_name, "", pubkey.c_str(), signature, sigsize);
    delete [] signature;
}

void Network::Client_Handle_AUTH(NetworkConnection& connection, NetworkPacket& packet)
{
    uint32 auth_status;
    packet >> auth_status >> (uint8&)player_id;
    connection.AuthStatus = (NETWORK_AUTH)auth_status;
    switch(connection.AuthStatus) {
    case NETWORK_AUTH_OK:
        Client_Send_GAMEINFO();
        break;
    case NETWORK_AUTH_BADNAME:
        connection.SetLastDisconnectReason(STR_MULTIPLAYER_BAD_PLAYER_NAME);
        connection.Socket->Disconnect();
        break;
    case NETWORK_AUTH_BADVERSION:
    {
        const char *version = packet.ReadString();
        connection.SetLastDisconnectReason(STR_MULTIPLAYER_INCORRECT_SOFTWARE_VERSION, &version);
        connection.Socket->Disconnect();
        break;
    }
    case NETWORK_AUTH_BADPASSWORD:
        connection.SetLastDisconnectReason(STR_MULTIPLAYER_BAD_PASSWORD);
        connection.Socket->Disconnect();
        break;
    case NETWORK_AUTH_VERIFICATIONFAILURE:
        connection.SetLastDisconnectReason(STR_MULTIPLAYER_VERIFICATION_FAILURE);
        connection.Socket->Disconnect();
        break;
    case NETWORK_AUTH_FULL:
        connection.SetLastDisconnectReason(STR_MULTIPLAYER_SERVER_FULL);
        connection.Socket->Disconnect();
        break;
    case NETWORK_AUTH_REQUIREPASSWORD:
        window_network_status_open_password();
        break;
    case NETWORK_AUTH_UNKNOWN_KEY_DISALLOWED:
        connection.SetLastDisconnectReason(STR_MULTIPLAYER_UNKNOWN_KEY_DISALLOWED);
        connection.Socket->Disconnect();
        break;
    default:
        connection.SetLastDisconnectReason(STR_MULTIPLAYER_INCORRECT_SOFTWARE_VERSION);
        connection.Socket->Disconnect();
        break;
    }
}

void Network::Server_Client_Joined(const char* name, const std::string &keyhash, NetworkConnection& connection)
{
    NetworkPlayer* player = AddPlayer(name, keyhash);
    connection.Player = player;
    if (player) {
        char text[256];
        const char * player_name = (const char *) player->Name.c_str();
        format_string(text, 256, STR_MULTIPLAYER_PLAYER_HAS_JOINED_THE_GAME, &player_name);
        chat_history_add(text);

        IObjectManager * objManager = GetObjectManager();
        auto objects = objManager->GetPackableObjects();
        Server_Send_OBJECTS(connection, objects);

        // Log player joining event
        AppendServerLog(text);
    }
}

void Network::Server_Handle_TOKEN(NetworkConnection& connection, NetworkPacket& packet)
{
    uint8 token_size = 10 + (rand() & 0x7f);
    connection.Challenge.resize(token_size);
    for (sint32 i = 0; i < token_size; i++) {
        connection.Challenge[i] = (uint8)(rand() & 0xff);
    }
    Server_Send_TOKEN(connection);
}

void Network::Client_Handle_OBJECTS(NetworkConnection& connection, NetworkPacket& packet)
{
    IObjectRepository * repo = GetObjectRepository();
    uint32 size;
    packet >> size;
    log_verbose("client received object list, it has %u entries", size);
    std::vector<std::string> requested_objects;
    for (uint32 i = 0; i < size; i++)
    {
        const char * name = (const char *)packet.Read(8);
        // Required, as packet has no null terminators.
        std::string s(name, name + 8);
        uint32 checksum, flags;
        packet >> checksum >> flags;
        const ObjectRepositoryItem * ori = repo->FindObject(s.c_str());
        // This could potentially request the object if checksums don't match, but since client
        // won't replace its version with server-provided one, we don't do that.
        if (ori == nullptr) {
            log_verbose("Requesting object %s with checksum %x from server",
                        s.c_str(), checksum);
            requested_objects.push_back(s);
        } else if (ori->ObjectEntry.checksum != checksum || ori->ObjectEntry.flags != flags) {
            log_warning("Object %s has different checksum/flags (%x/%x) than server (%x/%x).",
                        s.c_str(), ori->ObjectEntry.checksum, ori->ObjectEntry.flags, checksum, flags);
        }
    }
    Client_Send_OBJECTS(requested_objects);
}

void Network::Server_Handle_OBJECTS(NetworkConnection& connection, NetworkPacket& packet)
{
    uint32 size;
    packet >> size;
    log_verbose("Client requested %u objects", size);
    IObjectRepository * repo = GetObjectRepository();
    for (uint32 i = 0; i < size; i++)
    {
        const char * name = (const char *)packet.Read(8);
        // This is required, as packet does not have null terminator
        std::string s(name, name + 8);
        log_verbose("Client requested object %s", s.c_str());
        const ObjectRepositoryItem * item = repo->FindObject(s.c_str());
        if (item == nullptr) {
            log_warning("Client tried getting non-existent object %s from us.", s.c_str());
        } else {
            connection.RequestedObjects.push_back(item);
        }
    }

    const char * player_name = (const char *) connection.Player->Name.c_str();
    Server_Send_MAP(&connection);
    gNetwork.Server_Send_EVENT_PLAYER_JOINED(player_name);
    Server_Send_GROUPLIST(connection);
    Server_Send_PLAYERLIST();
}

void Network::Server_Handle_AUTH(NetworkConnection& connection, NetworkPacket& packet)
{
    if (connection.AuthStatus != NETWORK_AUTH_OK) {
        const char* gameversion = packet.ReadString();
        const char* name = packet.ReadString();
        const char* password = packet.ReadString();
        const char *pubkey = (const char *)packet.ReadString();
        uint32 sigsize;
        packet >> sigsize;
        if (pubkey == nullptr) {
            connection.AuthStatus = NETWORK_AUTH_VERIFICATIONFAILURE;
        } else {
            try
            {
                const char *signature = (const char *)packet.Read(sigsize);
                if (signature == nullptr)
                {
                    throw Exception();
                }

                auto ms = MemoryStream(pubkey, strlen(pubkey));
                if (!connection.Key.LoadPublic(&ms))
                {
                    throw Exception();
                }

                bool verified = connection.Key.Verify(connection.Challenge.data(), connection.Challenge.size(), signature, sigsize);
                const std::string hash = connection.Key.PublicKeyHash();
                if (verified)
                {
                    log_verbose("Signature verification ok. Hash %s", hash.c_str());
                    if (gConfigNetwork.known_keys_only && _userManager.GetUserByHash(hash) == nullptr)
                    {
                        log_verbose("Hash %s, not known", hash.c_str());
                        connection.AuthStatus = NETWORK_AUTH_UNKNOWN_KEY_DISALLOWED;
                    }
                    else
                    {
                        connection.AuthStatus = NETWORK_AUTH_VERIFIED;
                    }
                }
                else
                {
                    connection.AuthStatus = NETWORK_AUTH_VERIFICATIONFAILURE;
                    log_verbose("Signature verification failed!");
                }
            }
            catch (Exception)
            {
                connection.AuthStatus = NETWORK_AUTH_VERIFICATIONFAILURE;
                log_verbose("Signature verification failed, invalid data!");
            }
        }

        bool passwordless = false;
        if (connection.AuthStatus == NETWORK_AUTH_VERIFIED) {
            const NetworkGroup * group = GetGroupByID(GetGroupIDByHash(connection.Key.PublicKeyHash()));
            passwordless = group->CanPerformCommand(MISC_COMMAND_PASSWORDLESS_LOGIN);
        }
        if (!gameversion || strcmp(gameversion, NETWORK_STREAM_ID) != 0) {
            connection.AuthStatus = NETWORK_AUTH_BADVERSION;
        } else
        if (!name) {
            connection.AuthStatus = NETWORK_AUTH_BADNAME;
        } else
        if (!passwordless) {
            if ((!password || strlen(password) == 0) && _password.size() > 0) {
                connection.AuthStatus = NETWORK_AUTH_REQUIREPASSWORD;
            } else
            if (password && _password != password) {
                connection.AuthStatus = NETWORK_AUTH_BADPASSWORD;
            }
        }

        if ((size_t)gConfigNetwork.maxplayers <= player_list.size()) {
            connection.AuthStatus = NETWORK_AUTH_FULL;
        } else
        if (connection.AuthStatus == NETWORK_AUTH_VERIFIED) {
            connection.AuthStatus = NETWORK_AUTH_OK;
            const std::string hash = connection.Key.PublicKeyHash();
            Server_Client_Joined(name, hash, connection);
        } else
        if (connection.AuthStatus != NETWORK_AUTH_REQUIREPASSWORD) {
            log_error("Unknown failure (%d) while authenticating client", connection.AuthStatus);
        }
        Server_Send_AUTH(connection);
    }
}

void Network::Client_Handle_MAP(NetworkConnection& connection, NetworkPacket& packet)
{
    uint32 size, offset;
    packet >> size >> offset;
    sint32 chunksize = (sint32)(packet.Size - packet.BytesRead);
    if (chunksize <= 0) {
        return;
    }
    if (size > chunk_buffer.size()) {
        chunk_buffer.resize(size);
    }
    char str_downloading_map[256];
    uint32 downloading_map_args[2] = {(offset + chunksize) / 1024, size / 1024};
    format_string(str_downloading_map, 256, STR_MULTIPLAYER_DOWNLOADING_MAP, downloading_map_args);
    window_network_status_open(str_downloading_map, []() -> void {
        gNetwork.Close();
    });
    memcpy(&chunk_buffer[offset], (void*)packet.Read(chunksize), chunksize);
    if (offset + chunksize == size) {
        window_network_status_close();
        bool has_to_free = false;
        uint8 *data = &chunk_buffer[0];
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
                Close();
                return;
            }
        } else {
            log_verbose("Assuming received map is in plain sv6 format");
        }

        auto ms = MemoryStream(data, data_size);
        if (LoadMap(&ms))
        {
            game_load_init();
            game_command_queue.clear();
            server_tick = gCurrentTicks;
            server_srand0_tick = 0;
            // window_network_status_open("Loaded new map from network");
            _desynchronised = false;
            gFirstTimeSaving = true;

            // Notify user he is now online and which shortcut key enables chat
            network_chat_show_connected_message();

            // Fix invalid vehicle sprite sizes, thus preventing visual corruption of sprites
            fix_invalid_vehicle_sprite_sizes();
        }
        else
        {
            //Something went wrong, game is not loaded. Return to main screen.
            game_do_command(0, GAME_COMMAND_FLAG_APPLY, 0, 0, GAME_COMMAND_LOAD_OR_QUIT, 1, 0);
        }
        if (has_to_free)
        {
            free(data);
        }
    }
}

bool Network::LoadMap(IStream * stream)
{
    bool result = false;
    try
    {
        auto importer = std::unique_ptr<IParkImporter>(
            ParkImporter::CreateS6(GetObjectRepository(), GetObjectManager()));
        importer->LoadFromStream(stream, false);
        importer->Import();

        sprite_position_tween_reset();

        // Read checksum
        uint32 checksum = stream->ReadValue<uint32>();
        UNUSED(checksum);

        // Read other data not in normal save files
        stream->Read(gSpriteSpatialIndex, 0x10001 * sizeof(uint16));
        gGamePaused = stream->ReadValue<uint32>();
        _guestGenerationProbability = stream->ReadValue<uint32>();
        _suggestedGuestMaximum = stream->ReadValue<uint32>();
        gCheatsSandboxMode = stream->ReadValue<uint8>() != 0;
        gCheatsDisableClearanceChecks = stream->ReadValue<uint8>() != 0;
        gCheatsDisableSupportLimits = stream->ReadValue<uint8>() != 0;
        gCheatsDisableTrainLengthLimit = stream->ReadValue<uint8>() != 0;
        gCheatsEnableChainLiftOnAllTrack = stream->ReadValue<uint8>() != 0;
        gCheatsShowAllOperatingModes = stream->ReadValue<uint8>() != 0;
        gCheatsShowVehiclesFromOtherTrackTypes = stream->ReadValue<uint8>() != 0;
        gCheatsFastLiftHill = stream->ReadValue<uint8>() != 0;
        gCheatsDisableBrakesFailure = stream->ReadValue<uint8>() != 0;
        gCheatsDisableAllBreakdowns = stream->ReadValue<uint8>() != 0;
        gCheatsUnlockAllPrices = stream->ReadValue<uint8>() != 0;
        gCheatsBuildInPauseMode = stream->ReadValue<uint8>() != 0;
        gCheatsIgnoreRideIntensity = stream->ReadValue<uint8>() != 0;
        gCheatsDisableVandalism = stream->ReadValue<uint8>() != 0;
        gCheatsDisableLittering = stream->ReadValue<uint8>() != 0;
        gCheatsNeverendingMarketing = stream->ReadValue<uint8>() != 0;
        gCheatsFreezeClimate = stream->ReadValue<uint8>() != 0;
        gCheatsDisablePlantAging = stream->ReadValue<uint8>() != 0;
        gCheatsAllowArbitraryRideTypeChanges = stream->ReadValue<uint8>() != 0;
        gCheatsDisableRideValueAging = stream->ReadValue<uint8>() != 0;
        gConfigGeneral.show_real_names_of_guests = stream->ReadValue<uint8>() != 0;

        gLastAutoSaveUpdate = AUTOSAVE_PAUSE;
        result = true;
    }
    catch (const Exception &)
    {
    }
    return result;
}

bool Network::SaveMap(IStream * stream, const std::vector<const ObjectRepositoryItem *> &objects) const
{
    bool result = false;
    viewport_set_saved_view();
    try
    {
        auto s6exporter = std::make_unique<S6Exporter>();
        s6exporter->ExportObjectsList = objects;
        s6exporter->Export();
        s6exporter->SaveGame(stream);

        // Write other data not in normal save files
        stream->Write(gSpriteSpatialIndex, 0x10001 * sizeof(uint16));
        stream->WriteValue<uint32>(gGamePaused);
        stream->WriteValue<uint32>(_guestGenerationProbability);
        stream->WriteValue<uint32>(_suggestedGuestMaximum);
        stream->WriteValue<uint8>(gCheatsSandboxMode);
        stream->WriteValue<uint8>(gCheatsDisableClearanceChecks);
        stream->WriteValue<uint8>(gCheatsDisableSupportLimits);
        stream->WriteValue<uint8>(gCheatsDisableTrainLengthLimit);
        stream->WriteValue<uint8>(gCheatsEnableChainLiftOnAllTrack);
        stream->WriteValue<uint8>(gCheatsShowAllOperatingModes);
        stream->WriteValue<uint8>(gCheatsShowVehiclesFromOtherTrackTypes);
        stream->WriteValue<uint8>(gCheatsFastLiftHill);
        stream->WriteValue<uint8>(gCheatsDisableBrakesFailure);
        stream->WriteValue<uint8>(gCheatsDisableAllBreakdowns);
        stream->WriteValue<uint8>(gCheatsUnlockAllPrices);
        stream->WriteValue<uint8>(gCheatsBuildInPauseMode);
        stream->WriteValue<uint8>(gCheatsIgnoreRideIntensity);
        stream->WriteValue<uint8>(gCheatsDisableVandalism);
        stream->WriteValue<uint8>(gCheatsDisableLittering);
        stream->WriteValue<uint8>(gCheatsNeverendingMarketing);
        stream->WriteValue<uint8>(gCheatsFreezeClimate);
        stream->WriteValue<uint8>(gCheatsDisablePlantAging);
        stream->WriteValue<uint8>(gCheatsAllowArbitraryRideTypeChanges);
        stream->WriteValue<uint8>(gCheatsDisableRideValueAging);
        stream->WriteValue<uint8>(gConfigGeneral.show_real_names_of_guests);

        result = true;
    }
    catch (const Exception &)
    {
    }
    return result;
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
    if (connection.Player) {
        NetworkGroup* group = GetGroupByID(connection.Player->Group);
        if (!group || (group && !group->CanPerformCommand(MISC_COMMAND_CHAT))) {
            return;
        }
    }
    const char* text = packet.ReadString();
    if (text) {
        const char* formatted = FormatChat(connection.Player, text);
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

    GameCommand gc(tick, args, playerid, callback);
    game_command_queue.insert(gc);
}
void Network::Client_Handle_GAME_ACTION(NetworkConnection& connection, NetworkPacket& packet)
{
	uint32 tick;
	uint32 type;
	uint8 playerid;
	packet >> tick >> type >> playerid;
	MemoryStream stream;
	size_t size = packet.Size - packet.BytesRead;
	stream.WriteArray(packet.Read(size), size);
	stream.SetPosition(0);
	GameCommand gc(tick, type, stream, playerid);
	game_command_queue.insert(gc);
}

void Network::Server_Handle_GAME_ACTION(NetworkConnection& connection, NetworkPacket& packet)
{
	uint32 tick;
	uint32 commandType;

	if (!connection.Player) {
		return;
	}

	packet >> tick >> commandType;

	//tick count is different by time last_action_time is set, keep same value
	// Check if player's group permission allows command to run
	uint32 ticks = platform_get_ticks();
	NetworkGroup* group = GetGroupByID(connection.Player->Group);
	if (!group || (group && !group->CanPerformCommand(commandType))) {
		Server_Send_SHOWERROR(connection, STR_CANT_DO_THIS, STR_PERMISSION_DENIED);
		return;
	}

	// In case someone modifies the code / memory to enable cluster build,
	// require a small delay in between placing scenery to provide some security, as
	// cluster mode is a for loop that runs the place_scenery code multiple times.
	if (commandType == GAME_COMMAND_PLACE_SCENERY) {
		if (
			ticks - connection.Player->LastPlaceSceneryTime < ACTION_COOLDOWN_TIME_PLACE_SCENERY &&
			// Incase platform_get_ticks() wraps after ~49 days, ignore larger logged times.
			ticks > connection.Player->LastPlaceSceneryTime
			) {
			if (!(group->CanPerformCommand(MISC_COMMAND_TOGGLE_SCENERY_CLUSTER))) {
				Server_Send_SHOWERROR(connection, STR_CANT_DO_THIS, STR_NETWORK_ACTION_RATE_LIMIT_MESSAGE);
				return;
			}
		}
	}
	// This is to prevent abuse of demolishing rides. Anyone that is not the server
	// host will have to wait a small amount of time in between deleting rides.
	else if (commandType == GAME_COMMAND_DEMOLISH_RIDE) {
		if (
			ticks - connection.Player->LastDemolishRideTime < ACTION_COOLDOWN_TIME_DEMOLISH_RIDE &&
			// Incase platform_get_ticks()() wraps after ~49 days, ignore larger logged times.
			ticks > connection.Player->LastDemolishRideTime
			) {
			Server_Send_SHOWERROR(connection, STR_CANT_DO_THIS, STR_NETWORK_ACTION_RATE_LIMIT_MESSAGE);
			return;
		}
	}
	// Don't let clients send pause or quit
	else if (commandType == GAME_COMMAND_TOGGLE_PAUSE ||
		commandType == GAME_COMMAND_LOAD_OR_QUIT
		) {
		return;
	}

	// Set this to reference inside of game command functions
	game_command_playerid = connection.Player->Id;
	// Run game command, and if it is successful send to clients
	auto ga = GameActions::Create(commandType);
	MemoryStream stream;
	size_t size = packet.Size - packet.BytesRead;
	stream.WriteArray(packet.Read(size), size);
	stream.SetPosition(0);
	uint32 flags = stream.ReadValue<uint32>();
	ga->Deserialise(&stream);
	auto result = GameActions::Execute(ga, GAME_COMMAND_FLAG_NETWORKED | flags);
	if (result.Error != GA_ERROR::OK) {
		return;
	}

	connection.Player->LastAction = NetworkActions::FindCommand(commandType);
	connection.Player->LastActionTime = platform_get_ticks();
	connection.Player->AddMoneySpent(result.Cost);

	if (commandType == GAME_COMMAND_PLACE_SCENERY) {
		connection.Player->LastPlaceSceneryTime = connection.Player->LastActionTime;
	}
	else if (commandType == GAME_COMMAND_DEMOLISH_RIDE) {
		connection.Player->LastDemolishRideTime = connection.Player->LastActionTime;
	}

	Server_Send_GAME_ACTION(ga, flags);
}

void Network::Server_Handle_GAMECMD(NetworkConnection& connection, NetworkPacket& packet)
{
    uint32 tick;
    uint32 args[7];
    uint8 playerid;
    uint8 callback;

    if (!connection.Player) {
        return;
    }

    playerid = connection.Player->Id;

    packet >> tick >> args[0] >> args[1] >> args[2] >> args[3] >> args[4] >> args[5] >> args[6] >> callback;

    sint32 commandCommand = args[4];

    uint32 ticks = platform_get_ticks(); //tick count is different by time last_action_time is set, keep same value.

    // Check if player's group permission allows command to run
    NetworkGroup* group = GetGroupByID(connection.Player->Group);
    if (!group || (group && !group->CanPerformCommand(commandCommand))) {
        Server_Send_SHOWERROR(connection, STR_CANT_DO_THIS, STR_PERMISSION_DENIED);
        return;
    }

    // In case someone modifies the code / memory to enable cluster build,
    // require a small delay in between placing scenery to provide some security, as
    // cluster mode is a for loop that runs the place_scenery code multiple times.
    if (commandCommand == GAME_COMMAND_PLACE_SCENERY) {
        if (
            ticks - connection.Player->LastPlaceSceneryTime < ACTION_COOLDOWN_TIME_PLACE_SCENERY &&
            // In case platform_get_ticks() wraps after ~49 days, ignore larger logged times.
            ticks > connection.Player->LastPlaceSceneryTime
        ) {
            if (!(group->CanPerformCommand(MISC_COMMAND_TOGGLE_SCENERY_CLUSTER))) {
                Server_Send_SHOWERROR(connection, STR_CANT_DO_THIS, STR_NETWORK_ACTION_RATE_LIMIT_MESSAGE);
                return;
            }
        }
    }
    // This is to prevent abuse of demolishing rides. Anyone that is not the server
    // host will have to wait a small amount of time in between deleting rides.
    else if (commandCommand == GAME_COMMAND_DEMOLISH_RIDE) {
        if (
            ticks - connection.Player->LastDemolishRideTime < ACTION_COOLDOWN_TIME_DEMOLISH_RIDE &&
            // In case platform_get_ticks() wraps after ~49 days, ignore larger logged times.
            ticks > connection.Player->LastDemolishRideTime
        ) {
            Server_Send_SHOWERROR(connection, STR_CANT_DO_THIS, STR_NETWORK_ACTION_RATE_LIMIT_MESSAGE);
            return;
        }
    }
    // Don't let clients send pause or quit
    else if (commandCommand == GAME_COMMAND_TOGGLE_PAUSE ||
             commandCommand == GAME_COMMAND_LOAD_OR_QUIT
    ) {
        return;
    }

    GameCommand gc = GameCommand(tick, args, playerid, callback);
    game_command_queue.insert(gc);
}

void Network::Client_Handle_TICK(NetworkConnection& connection, NetworkPacket& packet)
{
    uint32 srand0;
    uint32 flags;
    // Note: older server version may not advertise flags at all.
    // NetworkPacket will return 0, if trying to read past end of buffer,
    // so flags == 0 is expected in such cases.
    packet >> server_tick >> srand0 >> flags;
    if (server_srand0_tick == 0) {
        server_srand0 = srand0;
        server_srand0_tick = server_tick;
        server_sprite_hash[0] = '\0';
        if (flags & NETWORK_TICK_FLAG_CHECKSUMS)
        {
            const char* text = packet.ReadString();
            if (text != nullptr)
            {
                safe_strcpy(server_sprite_hash, text, sizeof(server_sprite_hash));
            }
        }
    }
    game_commands_processed_this_tick = 0;
}

void Network::Client_Handle_PLAYERLIST(NetworkConnection& connection, NetworkPacket& packet)
{
    uint8 size;
    packet >> size;
    std::vector<uint8> ids;
    for (uint32 i = 0; i < size; i++) {
        NetworkPlayer tempplayer;
        tempplayer.Read(packet);
        ids.push_back(tempplayer.Id);
        if (!GetPlayerByID(tempplayer.Id)) {
            NetworkPlayer* player = AddPlayer("", "");
            if (player) {
                *player = tempplayer;
                if (player->Flags & NETWORK_PLAYER_FLAG_ISSERVER) {
                    server_connection->Player = player;
                }
            }
        }
    }
    // Remove any players that are not in newly received list
    auto it = player_list.begin();
    while (it != player_list.end()) {
        if (std::find(ids.begin(), ids.end(), (*it)->Id) == ids.end()) {
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
    sint32 ping = platform_get_ticks() - connection.PingTime;
    if (ping < 0) {
        ping = 0;
    }
    if (connection.Player) {
        connection.Player->Ping = ping;
        window_invalidate_by_number(WC_PLAYER, connection.Player->Id);
    }
}

void Network::Client_Handle_PINGLIST(NetworkConnection& connection, NetworkPacket& packet)
{
    uint8 size;
    packet >> size;
    for (uint32 i = 0; i < size; i++) {
        uint8 id;
        uint16 ping;
        packet >> id >> ping;
        NetworkPlayer* player = GetPlayerByID(id);
        if (player) {
            player->Ping = ping;
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
        connection.SetLastDisconnectReason(msg.c_str());
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
    for (uint32 i = 0; i < size; i++) {
        NetworkGroup group;
        group.Read(packet);
        std::unique_ptr<NetworkGroup> newgroup(new NetworkGroup(group)); // change to make_unique in c++14
        group_list.push_back(std::move(newgroup));
    }
}

void Network::Client_Handle_EVENT(NetworkConnection& connection, NetworkPacket& packet)
{
    char text[256];
    uint16 eventType;
    packet >> eventType;
    switch (eventType) {
    case SERVER_EVENT_PLAYER_JOINED:
    {
        const char *playerName = packet.ReadString();
        format_string(text, 256, STR_MULTIPLAYER_PLAYER_HAS_JOINED_THE_GAME, &playerName);
        chat_history_add(text);
        break;
    }
    case SERVER_EVENT_PLAYER_DISCONNECTED:
    {
        const char *playerName = packet.ReadString();
        const char *reason = packet.ReadString();
        const char *args[] = { playerName, reason };
        if (str_is_null_or_empty(reason)) {
            format_string(text, 256, STR_MULTIPLAYER_PLAYER_HAS_DISCONNECTED_NO_REASON, args);
        } else {
            format_string(text, 256, STR_MULTIPLAYER_PLAYER_HAS_DISCONNECTED_WITH_REASON, args);
        }
        chat_history_add(text);
        break;
    }
    }
}

void Network::Client_Send_GAMEINFO()
{
    log_verbose("requesting gameinfo");
    std::unique_ptr<NetworkPacket> packet(NetworkPacket::Allocate());
    *packet << (uint32)NETWORK_COMMAND_GAMEINFO;
    server_connection->QueuePacket(std::move(packet));
}

static std::string json_stdstring_value(const json_t * string)
{
    const char * cstr = json_string_value(string);
    return cstr == nullptr ? std::string() : std::string(cstr);
}

void Network::Client_Handle_GAMEINFO(NetworkConnection& connection, NetworkPacket& packet)
{
    const char * jsonString = packet.ReadString();

    json_error_t error;
    json_t *root = json_loads(jsonString, 0, &error);

    ServerName = json_stdstring_value(json_object_get(root, "name"));
    ServerDescription = json_stdstring_value(json_object_get(root, "description"));
    ServerGreeting = json_stdstring_value(json_object_get(root, "greeting"));

    json_t *jsonProvider = json_object_get(root, "provider");
    if (jsonProvider != nullptr) {
        ServerProviderName = json_stdstring_value(json_object_get(jsonProvider, "name"));
        ServerProviderEmail = json_stdstring_value(json_object_get(jsonProvider, "email"));
        ServerProviderWebsite = json_stdstring_value(json_object_get(jsonProvider, "website"));
    }
    json_decref(root);

    network_chat_show_server_greeting();
}

void network_set_env(void * env)
{
    gNetwork.SetEnvironment((IPlatformEnvironment *)env);
}

void network_close()
{
    gNetwork.Close();
}

void network_shutdown_client()
{
    gNetwork.ShutdownClient();
}

sint32 network_begin_client(const char *host, sint32 port)
{
    return gNetwork.BeginClient(host, port);
}

sint32 network_begin_server(sint32 port, const char* address)
{
    return gNetwork.BeginServer(port, address);
}

void network_update()
{
    gNetwork.Update();
}

void network_process_game_commands()
{
    gNetwork.ProcessGameCommandQueue();
}

sint32 network_get_mode()
{
    return gNetwork.GetMode();
}

sint32 network_get_status()
{
    return gNetwork.GetStatus();
}

sint32 network_get_authstatus()
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

sint32 network_get_num_players()
{
    return (sint32)gNetwork.player_list.size();
}

const char* network_get_player_name(uint32 index)
{
    return (const char*)gNetwork.player_list[index]->Name.c_str();
}

uint32 network_get_player_flags(uint32 index)
{
    return gNetwork.player_list[index]->Flags;
}

sint32 network_get_player_ping(uint32 index)
{
    return gNetwork.player_list[index]->Ping;
}

sint32 network_get_player_id(uint32 index)
{
    return gNetwork.player_list[index]->Id;
}

money32 network_get_player_money_spent(uint32 index)
{
    return gNetwork.player_list[index]->MoneySpent;
}

void network_add_player_money_spent(uint32 index, money32 cost)
{
    gNetwork.player_list[index]->AddMoneySpent(cost);
}

sint32 network_get_player_last_action(uint32 index, sint32 time)
{
    if (time && platform_get_ticks() > gNetwork.player_list[index]->LastActionTime + time) {
        return -999;
    }
    return gNetwork.player_list[index]->LastAction;
}

void network_set_player_last_action(uint32 index, sint32 command)
{
    gNetwork.player_list[index]->LastAction = NetworkActions::FindCommand(command);
    gNetwork.player_list[index]->LastActionTime = platform_get_ticks();
}

rct_xyz16 network_get_player_last_action_coord(uint32 index)
{
    return gNetwork.player_list[index]->LastActionCoord;
}

void network_set_player_last_action_coord(uint32 index, rct_xyz16 coord)
{
    if (index < gNetwork.player_list.size()) {
        gNetwork.player_list[index]->LastActionCoord = coord;
    }
}

uint32 network_get_player_commands_ran(uint32 index)
{
    return gNetwork.player_list[index]->CommandsRan;
}

sint32 network_get_player_index(uint8 id)
{
    auto it = gNetwork.GetPlayerIteratorByID(id);
    if(it == gNetwork.player_list.end()){
        return -1;
    }
    return (sint32)(gNetwork.GetPlayerIteratorByID(id) - gNetwork.player_list.begin());
}

uint8 network_get_player_group(uint32 index)
{
    return gNetwork.player_list[index]->Group;
}

void network_set_player_group(uint32 index, uint32 groupindex)
{
    gNetwork.player_list[index]->Group = gNetwork.group_list[groupindex]->Id;
}

sint32 network_get_group_index(uint8 id)
{
    auto it = gNetwork.GetGroupIteratorByID(id);
    if(it == gNetwork.group_list.end()){
        return -1;
    }
    return (sint32)(gNetwork.GetGroupIteratorByID(id) - gNetwork.group_list.begin());
}

uint8 network_get_group_id(uint32 index)
{
    return gNetwork.group_list[index]->Id;
}

sint32 network_get_num_groups()
{
    return (sint32)gNetwork.group_list.size();
}

const char* network_get_group_name(uint32 index)
{
    return gNetwork.group_list[index]->GetName().c_str();
}

void network_chat_show_connected_message()
{
    auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
    std::string s = windowManager->GetKeyboardShortcutString(41 /* SHORTCUT_OPEN_CHAT_WINDOW */);
    const char * sptr = s.c_str();

    utf8 buffer[256];
    format_string(buffer, sizeof(buffer), STR_MULTIPLAYER_CONNECTED_CHAT_HINT, &sptr);

    NetworkPlayer server;
    server.Name = "Server";
    const char * formatted = Network::FormatChat(&server, buffer);
    chat_history_add(formatted);
}

// Display server greeting if one exists
void network_chat_show_server_greeting()
{
    const char* greeting = network_get_server_greeting();
    if (!str_is_null_or_empty(greeting)) {
        static char greeting_formatted[CHAT_INPUT_SIZE];
        char* lineCh = greeting_formatted;
        greeting_formatted[0] = 0;
        lineCh = utf8_write_codepoint(lineCh, FORMAT_OUTLINE);
        lineCh = utf8_write_codepoint(lineCh, FORMAT_GREEN);
        char* ptrtext = lineCh;
        safe_strcpy(lineCh, greeting, CHAT_INPUT_SIZE - 24); // Limit to 1000 characters so we don't overflow the buffer
        utf8_remove_format_codes((utf8*)ptrtext, true);
        chat_history_add(greeting_formatted);
    }
}

void game_command_set_player_group(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp)
{
    uint8 playerid = (uint8)*ecx;
    uint8 groupid = (uint8)*edx;
    NetworkPlayer* player = gNetwork.GetPlayerByID(playerid);
    NetworkGroup* fromgroup = gNetwork.GetGroupByID(game_command_playerid);
    if (!player) {
        gGameCommandErrorTitle = STR_CANT_DO_THIS;
        gGameCommandErrorText = STR_NONE;
        *ebx = MONEY32_UNDEFINED;
        return;
    }
    if (!gNetwork.GetGroupByID(groupid)) {
        gGameCommandErrorTitle = STR_CANT_DO_THIS;
        gGameCommandErrorText = STR_NONE;
        *ebx = MONEY32_UNDEFINED;
        return;
    }
    if (player->Flags & NETWORK_PLAYER_FLAG_ISSERVER) {
        gGameCommandErrorTitle = STR_CANT_CHANGE_GROUP_THAT_THE_HOST_BELONGS_TO;
        gGameCommandErrorText = STR_NONE;
        *ebx = MONEY32_UNDEFINED;
        return;
    }
    if (groupid == 0 && fromgroup && fromgroup->Id != 0) {
        gGameCommandErrorTitle = STR_CANT_SET_TO_THIS_GROUP;
        gGameCommandErrorText = STR_NONE;
        *ebx = MONEY32_UNDEFINED;
        return;
    }
    if (*ebx & GAME_COMMAND_FLAG_APPLY) {
        player->Group = groupid;

        if (network_get_mode() == NETWORK_MODE_SERVER) {
            // Add or update saved user
            NetworkUserManager *userManager = &gNetwork._userManager;
            NetworkUser *networkUser = userManager->GetOrAddUser(player->KeyHash);
            networkUser->GroupId = groupid;
            networkUser->Name = player->Name;
            userManager->Save();
        }

        window_invalidate_by_number(WC_PLAYER, playerid);

        // Log set player group event
        NetworkPlayer* game_command_player = gNetwork.GetPlayerByID(game_command_playerid);
        NetworkGroup* new_player_group = gNetwork.GetGroupByID(groupid);
        char log_msg[256];
        const char * args[3] = {
            (char *) player->Name.c_str(),
            (char *) new_player_group->GetName().c_str(),
            (char *) game_command_player->Name.c_str()
        };
        format_string(log_msg, 256, STR_LOG_SET_PLAYER_GROUP, args);
        network_append_server_log(log_msg);
    }
    *ebx = 0;
}

void game_command_modify_groups(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp)
{
    uint8 action = (uint8)*eax;
    uint8 groupid = (uint8)(*eax >> 8);
    uint8 nameChunkIndex = (uint8)(*eax >> 16);

    switch (action)
    {
    case 0:{ // add group
        if (*ebx & GAME_COMMAND_FLAG_APPLY) {
            NetworkGroup* newgroup = gNetwork.AddGroup();
            if (!newgroup) {
                gGameCommandErrorTitle = STR_CANT_DO_THIS;
                gGameCommandErrorText = STR_NONE;
                *ebx = MONEY32_UNDEFINED;
                return;
            }

            // Log add player group event
            NetworkPlayer* game_command_player = gNetwork.GetPlayerByID(game_command_playerid);
            char log_msg[256];
            const char * args[2] = {
                (char *) game_command_player->Name.c_str(),
                (char *) newgroup->GetName().c_str()
            };
            format_string(log_msg, 256, STR_LOG_ADD_PLAYER_GROUP, args);
            network_append_server_log(log_msg);
        }
    }break;
    case 1:{ // remove group
        if (groupid == 0) {
            gGameCommandErrorTitle = STR_THIS_GROUP_CANNOT_BE_MODIFIED;
            gGameCommandErrorText = STR_NONE;
            *ebx = MONEY32_UNDEFINED;
            return;
        }
        for (auto it = gNetwork.player_list.begin(); it != gNetwork.player_list.end(); it++) {
            if((*it)->Group == groupid) {
                gGameCommandErrorTitle = STR_CANT_REMOVE_GROUP_THAT_PLAYERS_BELONG_TO;
                gGameCommandErrorText = STR_NONE;
                *ebx = MONEY32_UNDEFINED;
                return;
            }
        }
        if (*ebx & GAME_COMMAND_FLAG_APPLY) {
            // Log remove player group event
            NetworkPlayer* game_command_player = gNetwork.GetPlayerByID(game_command_playerid);
            NetworkGroup* group = gNetwork.GetGroupByID(groupid);
            char* groupName = (char *)group->GetName().c_str();
            char log_msg[256];
            const char * args[2] = {
                (char *) game_command_player->Name.c_str(),
                groupName
            };
            format_string(log_msg, 256, STR_LOG_REMOVE_PLAYER_GROUP, args);
            network_append_server_log(log_msg);

            gNetwork.RemoveGroup(groupid);
        }
    }break;
    case 2:{ // set permissions
        sint32 index = *ecx;
        bool all = *edx & 1;
        bool allvalue = (*edx >> 1) & 1;
        if (groupid == 0) { // cant change admin group permissions
            gGameCommandErrorTitle = STR_THIS_GROUP_CANNOT_BE_MODIFIED;
            gGameCommandErrorText = STR_NONE;
            *ebx = MONEY32_UNDEFINED;
            return;
        }
        NetworkGroup* mygroup = nullptr;
        NetworkPlayer* player = gNetwork.GetPlayerByID(game_command_playerid);
        if (player && !all) {
            mygroup = gNetwork.GetGroupByID(player->Group);
            if (!mygroup || (mygroup && !mygroup->CanPerformAction(index))) {
                gGameCommandErrorTitle = STR_CANT_MODIFY_PERMISSION_THAT_YOU_DO_NOT_HAVE_YOURSELF;
                gGameCommandErrorText = STR_NONE;
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
                            group->ActionsAllowed = mygroup->ActionsAllowed;
                        } else {
                            group->ActionsAllowed.fill(0x00);
                        }
                    }
                } else {
                    group->ToggleActionPermission(index);
                }
            }

            // Log edit player group permissions event
            char log_msg[256];
            const char * args[2] = {
                (char *) player->Name.c_str(),
                (char *) group->GetName().c_str()
            };
            format_string(log_msg, 256, STR_LOG_EDIT_PLAYER_GROUP_PERMISSIONS, args);
            network_append_server_log(log_msg);
        }
    }break;
    case 3:{ // set group name
        NetworkGroup* group = gNetwork.GetGroupByID(groupid);
        const char * oldName = group->GetName().c_str();
        static char newName[128];

        size_t nameChunkOffset = nameChunkIndex - 1;
        if (nameChunkIndex == 0)
            nameChunkOffset = 2;
        nameChunkOffset *= 12;
        nameChunkOffset = (std::min)(nameChunkOffset, Util::CountOf(newName) - 12);
        memcpy((void*)((uintptr_t)newName + (uintptr_t)nameChunkOffset + 0), edx, sizeof(uint32));
        memcpy((void*)((uintptr_t)newName + (uintptr_t)nameChunkOffset + 4), ebp, sizeof(uint32));
        memcpy((void*)((uintptr_t)newName + (uintptr_t)nameChunkOffset + 8), edi, sizeof(uint32));

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
            if (group) {
                // Log edit player group name event
                NetworkPlayer* player = gNetwork.GetPlayerByID(game_command_playerid);
                char log_msg[256];
                const char * args[3] = {
                    (char *) player->Name.c_str(),
                    oldName,
                    newName
                };
                format_string(log_msg, 256, STR_LOG_EDIT_PLAYER_GROUP_NAME, args);
                network_append_server_log(log_msg);

                group->SetName(newName);
            }
        }
    }break;
    case 4:{ // set default group
        if (groupid == 0) {
            gGameCommandErrorTitle = STR_CANT_SET_TO_THIS_GROUP;
            gGameCommandErrorText = STR_NONE;
            *ebx = MONEY32_UNDEFINED;
            return;
        }
        if (*ebx & GAME_COMMAND_FLAG_APPLY) {
            gNetwork.SetDefaultGroup(groupid);

            // Log edit default player group event
            NetworkPlayer* player = gNetwork.GetPlayerByID(game_command_playerid);
            NetworkGroup* group = gNetwork.GetGroupByID(groupid);
            char log_msg[256];
            const char * args[2] = {
                (char *) player->Name.c_str(),
                (char *) group->GetName().c_str()
            };
            format_string(log_msg, 256, STR_LOG_EDIT_DEFAULT_PLAYER_GROUP, args);
            network_append_server_log(log_msg);
        }
    }break;
    }

    gNetwork.SaveGroups();

    *ebx = 0;
}

void game_command_kick_player(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp)
{
    uint8 playerid = (uint8)*eax;
    NetworkPlayer* player = gNetwork.GetPlayerByID(playerid);
    if (player == nullptr) {
        // Player might be already removed by the PLAYERLIST command, need to refactor non-game commands executing too early.
        return;
    }

    if (player && player->Flags & NETWORK_PLAYER_FLAG_ISSERVER) {
        gGameCommandErrorTitle = STR_CANT_KICK_THE_HOST;
        gGameCommandErrorText = STR_NONE;
        *ebx = MONEY32_UNDEFINED;
        return;
    }
    if (*ebx & GAME_COMMAND_FLAG_APPLY) {
        if (gNetwork.GetMode() == NETWORK_MODE_SERVER) {
            gNetwork.KickPlayer(playerid);

            NetworkUserManager * networkUserManager = &gNetwork._userManager;
            networkUserManager->Load();
            networkUserManager->RemoveUser(player->KeyHash);
            networkUserManager->Save();
        }

        // Log kick player event
        NetworkPlayer* kicker = gNetwork.GetPlayerByID(game_command_playerid);
        char log_msg[256];
        const char * args[2] = {
            (char *) player->Name.c_str(),
            (char *) kicker->Name.c_str(),
        };
        format_string(log_msg, 256, STR_LOG_PLAYER_KICKED, args);
        network_append_server_log(log_msg);
    }
    *ebx = 0;
}

uint8 network_get_default_group()
{
    return gNetwork.GetDefaultGroup();
}

sint32 network_get_num_actions()
{
    return (sint32)NetworkActions::Actions.size();
}

rct_string_id network_get_action_name_string_id(uint32 index)
{
    if (index < NetworkActions::Actions.size())
    {
        return NetworkActions::Actions[index].Name;
    } else {
        return STR_NONE;
    }
}

sint32 network_can_perform_action(uint32 groupindex, uint32 index)
{
    return gNetwork.group_list[groupindex]->CanPerformAction(index);
}

sint32 network_can_perform_command(uint32 groupindex, uint32 index)
{
    return gNetwork.group_list[groupindex]->CanPerformCommand(index);
}

void network_set_pickup_peep(uint8 playerid, rct_peep* peep)
{
    gNetwork.GetMode() == NETWORK_MODE_NONE ? _pickup_peep = peep : gNetwork.GetPlayerByID(playerid)->PickupPeep = peep;
}

rct_peep* network_get_pickup_peep(uint8 playerid)
{
    return gNetwork.GetMode() == NETWORK_MODE_NONE ? _pickup_peep : gNetwork.GetPlayerByID(playerid)->PickupPeep;
}

void network_set_pickup_peep_old_x(uint8 playerid, sint32 x)
{
    gNetwork.GetMode() == NETWORK_MODE_NONE ? _pickup_peep_old_x = x : gNetwork.GetPlayerByID(playerid)->PickupPeepOldX = x;
}

sint32 network_get_pickup_peep_old_x(uint8 playerid)
{
    return gNetwork.GetMode() == NETWORK_MODE_NONE ? _pickup_peep_old_x : gNetwork.GetPlayerByID(playerid)->PickupPeepOldX;
}

sint32 network_get_current_player_group_index()
{
    return network_get_group_index(gNetwork.GetPlayerByID(gNetwork.GetPlayerID())->Group);
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

void network_send_game_action(const IGameAction *action, uint32 flags = 0)
{
	switch (gNetwork.GetMode()) {
	case NETWORK_MODE_SERVER:
		gNetwork.Server_Send_GAME_ACTION(action, flags);
		break;
	case NETWORK_MODE_CLIENT:
		gNetwork.Client_Send_GAME_ACTION(action, flags);
		break;
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
    utf8 keyPath[MAX_PATH];
    network_get_private_key_path(keyPath, sizeof(keyPath), gConfigNetwork.player_name);
    if (!platform_file_exists(keyPath)) {
        log_error("Private key %s missing! Restart the game to generate it.", keyPath);
        return;
    }
    try
    {
        auto fs = FileStream(keyPath, FILE_MODE_OPEN);
        gNetwork._key.LoadPrivate(&fs);
    }
    catch (Exception)
    {
        log_error("Error reading private key from %s.", keyPath);
        return;
    }
    const std::string pubkey = gNetwork._key.PublicKeyString();
    size_t sigsize;
    char *signature;
    gNetwork._key.Sign(gNetwork._challenge.data(), gNetwork._challenge.size(), &signature, &sigsize);
    // Don't keep private key in memory. There's no need and it may get leaked
    // when process dump gets collected at some point in future.
    gNetwork._key.Unload();
    gNetwork.Client_Send_AUTH(gConfigNetwork.player_name, password, pubkey.c_str(), signature, sigsize);
    delete [] signature;
}

void network_set_password(const char* password)
{
    gNetwork.SetPassword(password);
}

void network_append_chat_log(const utf8 *text)
{
    gNetwork.AppendChatLog(text);
}

void network_append_server_log(const utf8 *text)
{
    gNetwork.AppendServerLog(text);
}

static void network_get_keys_directory(utf8 *buffer, size_t bufferSize)
{
    platform_get_user_directory(buffer, "keys", bufferSize);
}

static void network_get_private_key_path(utf8 *buffer, size_t bufferSize, const utf8 * playerName)
{
    network_get_keys_directory(buffer, bufferSize);
    Path::Append(buffer, bufferSize, playerName);
    String::Append(buffer, bufferSize, ".privkey");
}

static void network_get_public_key_path(utf8 *buffer, size_t bufferSize, const utf8 * playerName, const utf8 * hash)
{
    network_get_keys_directory(buffer, bufferSize);
    Path::Append(buffer, bufferSize, playerName);
    String::Append(buffer, bufferSize, "-");
    String::Append(buffer, bufferSize, hash);
    String::Append(buffer, bufferSize, ".pubkey");
}

const utf8 * network_get_server_name() { return gNetwork.ServerName.c_str(); }
const utf8 * network_get_server_description() { return gNetwork.ServerDescription.c_str(); }
const utf8 * network_get_server_greeting() { return gNetwork.ServerGreeting.c_str(); }
const utf8 * network_get_server_provider_name() { return gNetwork.ServerProviderName.c_str(); }
const utf8 * network_get_server_provider_email() { return gNetwork.ServerProviderEmail.c_str(); }
const utf8 * network_get_server_provider_website() { return gNetwork.ServerProviderWebsite.c_str(); }

#else
sint32 network_get_mode() { return NETWORK_MODE_NONE; }
sint32 network_get_status() { return NETWORK_STATUS_NONE; }
sint32 network_get_authstatus() { return NETWORK_AUTH_NONE; }
uint32 network_get_server_tick() { return gCurrentTicks; }
void network_send_gamecmd(uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp, uint8 callback) {}
void network_send_game_action(const IGameAction *action, uint32 flags = 0) {}
void network_send_map() {}
void network_update() {}
void network_process_game_commands() {}
sint32 network_begin_client(const char *host, sint32 port) { return 1; }
sint32 network_begin_server(sint32 port, const char * address) { return 1; }
sint32 network_get_num_players() { return 1; }
const char* network_get_player_name(uint32 index) { return "local (OpenRCT2 compiled without MP)"; }
uint32 network_get_player_flags(uint32 index) { return 0; }
sint32 network_get_player_ping(uint32 index) { return 0; }
sint32 network_get_player_id(uint32 index) { return 0; }
money32 network_get_player_money_spent(uint32 index) { return MONEY(0, 0); }
void network_add_player_money_spent(uint32 index, money32 cost) { }
sint32 network_get_player_last_action(uint32 index, sint32 time) { return -999; }
void network_set_player_last_action(uint32 index, sint32 command) { }
rct_xyz16 network_get_player_last_action_coord(uint32 index) { return {0, 0, 0}; }
void network_set_player_last_action_coord(uint32 index, rct_xyz16 coord) { }
uint32 network_get_player_commands_ran(uint32 index) { return 0; }
sint32 network_get_player_index(uint8 id) { return -1; }
uint8 network_get_player_group(uint32 index) { return 0; }
void network_set_player_group(uint32 index, uint32 groupindex) { }
sint32 network_get_group_index(uint8 id) { return -1; }
uint8 network_get_group_id(uint32 index) { return 0; }
sint32 network_get_num_groups() { return 0; }
const char* network_get_group_name(uint32 index) { return ""; };
void game_command_set_player_group(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp) { }
void game_command_modify_groups(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp) { }
void game_command_kick_player(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp) { }
uint8 network_get_default_group() { return 0; }
sint32 network_get_num_actions() { return 0; }
rct_string_id network_get_action_name_string_id(uint32 index) { return -1; }
sint32 network_can_perform_action(uint32 groupindex, uint32 index) { return 0; }
sint32 network_can_perform_command(uint32 groupindex, uint32 index) { return 0; }
void network_set_pickup_peep(uint8 playerid, rct_peep* peep) { _pickup_peep = peep; }
rct_peep* network_get_pickup_peep(uint8 playerid) { return _pickup_peep; }
void network_set_pickup_peep_old_x(uint8 playerid, sint32 x) { _pickup_peep_old_x = x; }
sint32 network_get_pickup_peep_old_x(uint8 playerid) { return _pickup_peep_old_x; }
void network_send_chat(const char* text) {}
void network_send_password(const char* password) {}
void network_close() {}
void network_set_env(void * env) {}
void network_shutdown_client() {}
void network_set_password(const char* password) {}
uint8 network_get_current_player_id() { return 0; }
sint32 network_get_current_player_group_index() { return 0; }
void network_append_chat_log(const utf8 *text) { }
void network_append_server_log(const utf8 *text) { }
const utf8 * network_get_server_name() { return nullptr; }
const utf8 * network_get_server_description() { return nullptr; }
const utf8 * network_get_server_greeting() { return nullptr; }
const utf8 * network_get_server_provider_name() { return nullptr; }
const utf8 * network_get_server_provider_email() { return nullptr; }
const utf8 * network_get_server_provider_website() { return nullptr; }
#endif /* DISABLE_NETWORK */
