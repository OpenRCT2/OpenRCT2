/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "NetworkBase.h"

#include "../Context.h"
#include "../Game.h"
#include "../GameStateSnapshots.h"
#include "../OpenRCT2.h"
#include "../PlatformEnvironment.h"
#include "../actions/LoadOrQuitAction.h"
#include "../actions/NetworkModifyGroupAction.h"
#include "../actions/PeepPickupAction.h"
#include "../core/File.h"
#include "../core/Guard.hpp"
#include "../core/Json.hpp"
#include "../entity/EntityList.h"
#include "../entity/EntityRegistry.h"
#include "../entity/EntityTweener.h"
#include "../localisation/Formatter.h"
#include "../localisation/Formatting.h"
#include "../park/ParkFile.h"
#include "../platform/Platform.h"
#include "../scenario/Scenario.h"
#include "../scripting/ScriptEngine.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../util/SawyerCoding.h"
#include "../world/Location.hpp"
#include "network.h"

#include <algorithm>
#include <iterator>
#include <stdexcept>

// This string specifies which version of network stream current build uses.
// It is used for making sure only compatible builds get connected, even within
// single OpenRCT2 version.
#define NETWORK_STREAM_VERSION "4"
#define NETWORK_STREAM_ID OPENRCT2_VERSION "-" NETWORK_STREAM_VERSION

static Peep* _pickup_peep = nullptr;
static int32_t _pickup_peep_old_x = LOCATION_NULL;

#ifndef DISABLE_NETWORK

// General chunk size is 63 KiB, this can not be any larger because the packet size is encoded
// with uint16_t and needs some spare room for other data in the packet.
static constexpr uint32_t CHUNK_SIZE = 1024 * 63;

// If data is sent fast enough it would halt the entire server, process only a maximum amount.
// This limit is per connection, the current value was determined by tests with fuzzing.
static constexpr uint32_t MaxPacketsPerUpdate = 100;

#    include "../Cheats.h"
#    include "../ParkImporter.h"
#    include "../Version.h"
#    include "../actions/GameAction.h"
#    include "../config/Config.h"
#    include "../core/Console.hpp"
#    include "../core/FileStream.h"
#    include "../core/MemoryStream.h"
#    include "../core/Path.hpp"
#    include "../core/String.hpp"
#    include "../interface/Chat.h"
#    include "../interface/Window.h"
#    include "../localisation/Date.h"
#    include "../localisation/Localisation.h"
#    include "../object/ObjectManager.h"
#    include "../object/ObjectRepository.h"
#    include "../scenario/Scenario.h"
#    include "../util/Util.h"
#    include "../world/Park.h"
#    include "NetworkAction.h"
#    include "NetworkConnection.h"
#    include "NetworkGroup.h"
#    include "NetworkKey.h"
#    include "NetworkPacket.h"
#    include "NetworkPlayer.h"
#    include "NetworkServerAdvertiser.h"
#    include "NetworkUser.h"
#    include "Socket.h"

#    include <algorithm>
#    include <array>
#    include <cerrno>
#    include <cmath>
#    include <fstream>
#    include <functional>
#    include <list>
#    include <map>
#    include <memory>
#    include <set>
#    include <string>
#    include <vector>

using namespace OpenRCT2;

static void network_chat_show_connected_message();
static void network_chat_show_server_greeting();
static u8string network_get_keys_directory();
static u8string network_get_private_key_path(u8string_view playerName);
static u8string network_get_public_key_path(u8string_view playerName, u8string_view hash);

NetworkBase::NetworkBase(OpenRCT2::IContext& context)
    : OpenRCT2::System(context)
{
    wsa_initialized = false;
    mode = NETWORK_MODE_NONE;
    status = NETWORK_STATUS_NONE;
    last_ping_sent_time = 0;
    _actionId = 0;

    client_command_handlers[NetworkCommand::Auth] = &NetworkBase::Client_Handle_AUTH;
    client_command_handlers[NetworkCommand::Map] = &NetworkBase::Client_Handle_MAP;
    client_command_handlers[NetworkCommand::Chat] = &NetworkBase::Client_Handle_CHAT;
    client_command_handlers[NetworkCommand::GameAction] = &NetworkBase::Client_Handle_GAME_ACTION;
    client_command_handlers[NetworkCommand::Tick] = &NetworkBase::Client_Handle_TICK;
    client_command_handlers[NetworkCommand::PlayerList] = &NetworkBase::Client_Handle_PLAYERLIST;
    client_command_handlers[NetworkCommand::PlayerInfo] = &NetworkBase::Client_Handle_PLAYERINFO;
    client_command_handlers[NetworkCommand::Ping] = &NetworkBase::Client_Handle_PING;
    client_command_handlers[NetworkCommand::PingList] = &NetworkBase::Client_Handle_PINGLIST;
    client_command_handlers[NetworkCommand::DisconnectMessage] = &NetworkBase::Client_Handle_SETDISCONNECTMSG;
    client_command_handlers[NetworkCommand::ShowError] = &NetworkBase::Client_Handle_SHOWERROR;
    client_command_handlers[NetworkCommand::GroupList] = &NetworkBase::Client_Handle_GROUPLIST;
    client_command_handlers[NetworkCommand::Event] = &NetworkBase::Client_Handle_EVENT;
    client_command_handlers[NetworkCommand::GameInfo] = &NetworkBase::Client_Handle_GAMEINFO;
    client_command_handlers[NetworkCommand::Token] = &NetworkBase::Client_Handle_TOKEN;
    client_command_handlers[NetworkCommand::ObjectsList] = &NetworkBase::Client_Handle_OBJECTS_LIST;
    client_command_handlers[NetworkCommand::Scripts] = &NetworkBase::Client_Handle_SCRIPTS;
    client_command_handlers[NetworkCommand::GameState] = &NetworkBase::Client_Handle_GAMESTATE;

    server_command_handlers[NetworkCommand::Auth] = &NetworkBase::Server_Handle_AUTH;
    server_command_handlers[NetworkCommand::Chat] = &NetworkBase::Server_Handle_CHAT;
    server_command_handlers[NetworkCommand::GameAction] = &NetworkBase::Server_Handle_GAME_ACTION;
    server_command_handlers[NetworkCommand::Ping] = &NetworkBase::Server_Handle_PING;
    server_command_handlers[NetworkCommand::GameInfo] = &NetworkBase::Server_Handle_GAMEINFO;
    server_command_handlers[NetworkCommand::Token] = &NetworkBase::Server_Handle_TOKEN;
    server_command_handlers[NetworkCommand::MapRequest] = &NetworkBase::Server_Handle_MAPREQUEST;
    server_command_handlers[NetworkCommand::RequestGameState] = &NetworkBase::Server_Handle_REQUEST_GAMESTATE;
    server_command_handlers[NetworkCommand::Heartbeat] = &NetworkBase::Server_Handle_HEARTBEAT;

    _chat_log_fs << std::unitbuf;
    _server_log_fs << std::unitbuf;
}

bool NetworkBase::Init()
{
    status = NETWORK_STATUS_READY;

    ServerName.clear();
    ServerDescription.clear();
    ServerGreeting.clear();
    ServerProviderName.clear();
    ServerProviderEmail.clear();
    ServerProviderWebsite.clear();
    return true;
}

void NetworkBase::Reconnect()
{
    if (status != NETWORK_STATUS_NONE)
    {
        Close();
    }
    if (_requireClose)
    {
        _requireReconnect = true;
        return;
    }
    BeginClient(_host, _port);
}

void NetworkBase::Close()
{
    if (status != NETWORK_STATUS_NONE)
    {
        // HACK Because Close() is closed all over the place, it sometimes gets called inside an Update
        //      call. This then causes disposed data to be accessed. Therefore, save closing until the
        //      end of the update loop.
        if (_closeLock)
        {
            _requireClose = true;
            return;
        }

        CloseChatLog();
        CloseServerLog();
        CloseConnection();

        client_connection_list.clear();
        GameActions::ClearQueue();
        GameActions::ResumeQueue();
        player_list.clear();
        group_list.clear();
        _serverTickData.clear();
        _pendingPlayerLists.clear();
        _pendingPlayerInfo.clear();

#    ifdef ENABLE_SCRIPTING
        auto& scriptEngine = GetContext().GetScriptEngine();
        scriptEngine.RemoveNetworkPlugins();
#    endif

        gfx_invalidate_screen();

        _requireClose = false;
    }
}

void NetworkBase::DecayCooldown(NetworkPlayer* player)
{
    if (player == nullptr)
        return; // No valid connection yet.

    for (auto it = std::begin(player->CooldownTime); it != std::end(player->CooldownTime);)
    {
        it->second -= _currentDeltaTime;
        if (it->second <= 0)
            it = player->CooldownTime.erase(it);
        else
            it++;
    }
}

void NetworkBase::CloseConnection()
{
    if (mode == NETWORK_MODE_CLIENT)
    {
        _serverConnection.reset();
    }
    else if (mode == NETWORK_MODE_SERVER)
    {
        _listenSocket.reset();
        _advertiser.reset();
    }

    mode = NETWORK_MODE_NONE;
    status = NETWORK_STATUS_NONE;
    _lastConnectStatus = SocketStatus::Closed;
}

bool NetworkBase::BeginClient(const std::string& host, uint16_t port)
{
    if (GetMode() != NETWORK_MODE_NONE)
    {
        return false;
    }

    Close();
    if (!Init())
        return false;

    mode = NETWORK_MODE_CLIENT;

    log_info("Connecting to %s:%u", host.c_str(), port);
    _host = host;
    _port = port;

    _serverConnection = std::make_unique<NetworkConnection>();
    _serverConnection->Socket = CreateTcpSocket();
    _serverConnection->Socket->ConnectAsync(host, port);
    _serverState.gamestateSnapshotsEnabled = false;

    status = NETWORK_STATUS_CONNECTING;
    _lastConnectStatus = SocketStatus::Closed;
    _clientMapLoaded = false;
    _serverTickData.clear();

    BeginChatLog();
    BeginServerLog();

    // We need to wait for the map load before we execute any actions.
    // If the client has the title screen running then there's a potential
    // risk of tick collision with the server map and title screen map.
    GameActions::SuspendQueue();

    auto keyPath = network_get_private_key_path(gConfigNetwork.player_name);
    if (!File::Exists(keyPath))
    {
        Console::WriteLine("Generating key... This may take a while");
        Console::WriteLine("Need to collect enough entropy from the system");
        _key.Generate();
        Console::WriteLine("Key generated, saving private bits as %s", keyPath.c_str());

        const auto keysDirectory = network_get_keys_directory();
        if (!Platform::EnsureDirectoryExists(keysDirectory.c_str()))
        {
            log_error("Unable to create directory %s.", keysDirectory.c_str());
            return false;
        }

        try
        {
            auto fs = FileStream(keyPath, FILE_MODE_WRITE);
            _key.SavePrivate(&fs);
        }
        catch (const std::exception&)
        {
            log_error("Unable to save private key at %s.", keyPath.c_str());
            return false;
        }

        const std::string hash = _key.PublicKeyHash();
        const utf8* publicKeyHash = hash.c_str();
        keyPath = network_get_public_key_path(gConfigNetwork.player_name, publicKeyHash);
        Console::WriteLine("Key generated, saving public bits as %s", keyPath.c_str());

        try
        {
            auto fs = FileStream(keyPath, FILE_MODE_WRITE);
            _key.SavePublic(&fs);
        }
        catch (const std::exception&)
        {
            log_error("Unable to save public key at %s.", keyPath.c_str());
            return false;
        }
    }
    else
    {
        // LoadPrivate returns validity of loaded key
        bool ok = false;
        try
        {
            log_verbose("Loading key from %s", keyPath.c_str());
            auto fs = FileStream(keyPath, FILE_MODE_OPEN);
            ok = _key.LoadPrivate(&fs);
        }
        catch (const std::exception&)
        {
            log_error("Unable to read private key from %s.", keyPath.c_str());
            return false;
        }

        // Don't store private key in memory when it's not in use.
        _key.Unload();
        return ok;
    }

    return true;
}

bool NetworkBase::BeginServer(uint16_t port, const std::string& address)
{
    Close();
    if (!Init())
        return false;

    mode = NETWORK_MODE_SERVER;

    _userManager.Load();

    log_verbose("Begin listening for clients");

    _listenSocket = CreateTcpSocket();
    try
    {
        _listenSocket->Listen(address, port);
    }
    catch (const std::exception& ex)
    {
        Console::Error::WriteLine(ex.what());
        Close();
        return false;
    }

    ServerName = gConfigNetwork.server_name;
    ServerDescription = gConfigNetwork.server_description;
    ServerGreeting = gConfigNetwork.server_greeting;
    ServerProviderName = gConfigNetwork.provider_name;
    ServerProviderEmail = gConfigNetwork.provider_email;
    ServerProviderWebsite = gConfigNetwork.provider_website;

    CheatsReset();
    LoadGroups();
    BeginChatLog();
    BeginServerLog();

    NetworkPlayer* player = AddPlayer(gConfigNetwork.player_name, "");
    player->Flags |= NETWORK_PLAYER_FLAG_ISSERVER;
    player->Group = 0;
    player_id = player->Id;

    if (network_get_mode() == NETWORK_MODE_SERVER)
    {
        // Add SERVER to users.json and save.
        NetworkUser* networkUser = _userManager.GetOrAddUser(player->KeyHash);
        networkUser->GroupId = player->Group;
        networkUser->Name = player->Name;
        _userManager.Save();
    }

    auto* szAddress = address.empty() ? "*" : address.c_str();
    Console::WriteLine("Listening for clients on %s:%hu", szAddress, port);
    network_chat_show_connected_message();
    network_chat_show_server_greeting();

    status = NETWORK_STATUS_CONNECTED;
    listening_port = port;
    _serverState.gamestateSnapshotsEnabled = gConfigNetwork.desync_debugging;
    _advertiser = CreateServerAdvertiser(listening_port);

    game_load_scripts();
    game_notify_map_changed();

    return true;
}

int32_t NetworkBase::GetMode() const noexcept
{
    return mode;
}

int32_t NetworkBase::GetStatus() const noexcept
{
    return status;
}

NetworkAuth NetworkBase::GetAuthStatus()
{
    if (GetMode() == NETWORK_MODE_CLIENT)
    {
        return _serverConnection->AuthStatus;
    }
    if (GetMode() == NETWORK_MODE_SERVER)
    {
        return NetworkAuth::Ok;
    }
    return NetworkAuth::None;
}

uint32_t NetworkBase::GetServerTick() const noexcept
{
    return _serverState.tick;
}

uint8_t NetworkBase::GetPlayerID() const noexcept
{
    return player_id;
}

NetworkConnection* NetworkBase::GetPlayerConnection(uint8_t id) const
{
    auto player = GetPlayerByID(id);
    if (player != nullptr)
    {
        auto clientIt = std::find_if(
            client_connection_list.begin(), client_connection_list.end(),
            [player](const auto& conn) -> bool { return conn->Player == player; });
        return clientIt != client_connection_list.end() ? clientIt->get() : nullptr;
    }
    return nullptr;
}

void NetworkBase::Update()
{
    _closeLock = true;

    // Update is not necessarily called per game tick, maintain our own delta time
    uint32_t ticks = Platform::GetTicks();
    _currentDeltaTime = std::max<uint32_t>(ticks - _lastUpdateTime, 1);
    _lastUpdateTime = ticks;

    switch (GetMode())
    {
        case NETWORK_MODE_SERVER:
            UpdateServer();
            break;
        case NETWORK_MODE_CLIENT:
            UpdateClient();
            break;
    }

    // If the Close() was called during the update, close it for real
    _closeLock = false;
    if (_requireClose)
    {
        Close();
        if (_requireReconnect)
        {
            Reconnect();
        }
    }
}

void NetworkBase::Flush()
{
    if (GetMode() == NETWORK_MODE_CLIENT)
    {
        _serverConnection->SendQueuedPackets();
    }
    else
    {
        for (auto& it : client_connection_list)
        {
            it->SendQueuedPackets();
        }
    }
}

void NetworkBase::UpdateServer()
{
    for (auto& connection : client_connection_list)
    {
        // This can be called multiple times before the connection is removed.
        if (!connection->IsValid())
            continue;

        if (!ProcessConnection(*connection))
        {
            connection->Disconnect();
        }
        else
        {
            DecayCooldown(connection->Player);
        }
    }

    uint32_t ticks = Platform::GetTicks();
    if (ticks > last_ping_sent_time + 3000)
    {
        Server_Send_PING();
        Server_Send_PINGLIST();
    }

    if (_advertiser != nullptr)
    {
        _advertiser->Update();
    }

    std::unique_ptr<ITcpSocket> tcpSocket = _listenSocket->Accept();
    if (tcpSocket != nullptr)
    {
        AddClient(std::move(tcpSocket));
    }
}

void NetworkBase::UpdateClient()
{
    assert(_serverConnection != nullptr);

    switch (status)
    {
        case NETWORK_STATUS_CONNECTING:
        {
            switch (_serverConnection->Socket->GetStatus())
            {
                case SocketStatus::Resolving:
                {
                    if (_lastConnectStatus != SocketStatus::Resolving)
                    {
                        _lastConnectStatus = SocketStatus::Resolving;
                        char str_resolving[256];
                        format_string(str_resolving, 256, STR_MULTIPLAYER_RESOLVING, nullptr);

                        auto intent = Intent(WC_NETWORK_STATUS);
                        intent.putExtra(INTENT_EXTRA_MESSAGE, std::string{ str_resolving });
                        intent.putExtra(INTENT_EXTRA_CALLBACK, []() -> void { ::GetContext()->GetNetwork().Close(); });
                        context_open_intent(&intent);
                    }
                    break;
                }
                case SocketStatus::Connecting:
                {
                    if (_lastConnectStatus != SocketStatus::Connecting)
                    {
                        _lastConnectStatus = SocketStatus::Connecting;
                        char str_connecting[256];
                        format_string(str_connecting, 256, STR_MULTIPLAYER_CONNECTING, nullptr);

                        auto intent = Intent(WC_NETWORK_STATUS);
                        intent.putExtra(INTENT_EXTRA_MESSAGE, std::string{ str_connecting });
                        intent.putExtra(INTENT_EXTRA_CALLBACK, []() -> void { ::GetContext()->GetNetwork().Close(); });
                        context_open_intent(&intent);

                        server_connect_time = Platform::GetTicks();
                    }
                    break;
                }
                case SocketStatus::Connected:
                {
                    status = NETWORK_STATUS_CONNECTED;
                    _serverConnection->ResetLastPacketTime();
                    Client_Send_TOKEN();
                    char str_authenticating[256];
                    format_string(str_authenticating, 256, STR_MULTIPLAYER_AUTHENTICATING, nullptr);

                    auto intent = Intent(WC_NETWORK_STATUS);
                    intent.putExtra(INTENT_EXTRA_MESSAGE, std::string{ str_authenticating });
                    intent.putExtra(INTENT_EXTRA_CALLBACK, []() -> void { ::GetContext()->GetNetwork().Close(); });
                    context_open_intent(&intent);
                    break;
                }
                default:
                {
                    const char* error = _serverConnection->Socket->GetError();
                    if (error != nullptr)
                    {
                        Console::Error::WriteLine(error);
                    }

                    Close();
                    context_force_close_window_by_class(WC_NETWORK_STATUS);
                    context_show_error(STR_UNABLE_TO_CONNECT_TO_SERVER, STR_NONE, {});
                    break;
                }
            }
            break;
        }
        case NETWORK_STATUS_CONNECTED:
        {
            if (!ProcessConnection(*_serverConnection))
            {
                // Do not show disconnect message window when password window closed/canceled
                if (_serverConnection->AuthStatus == NetworkAuth::RequirePassword)
                {
                    context_force_close_window_by_class(WC_NETWORK_STATUS);
                }
                else
                {
                    char str_disconnected[256];

                    if (_serverConnection->GetLastDisconnectReason())
                    {
                        const char* disconnect_reason = _serverConnection->GetLastDisconnectReason();
                        format_string(str_disconnected, 256, STR_MULTIPLAYER_DISCONNECTED_WITH_REASON, &disconnect_reason);
                    }
                    else
                    {
                        format_string(str_disconnected, 256, STR_MULTIPLAYER_DISCONNECTED_NO_REASON, nullptr);
                    }

                    auto intent = Intent(WC_NETWORK_STATUS);
                    intent.putExtra(INTENT_EXTRA_MESSAGE, std::string{ str_disconnected });
                    context_open_intent(&intent);
                }
                window_close_by_class(WC_MULTIPLAYER);
                Close();
            }
            else
            {
                uint32_t ticks = Platform::GetTicks();
                if (ticks - _lastSentHeartbeat >= 3000)
                {
                    Client_Send_HEARTBEAT(*_serverConnection);
                    _lastSentHeartbeat = ticks;
                }
            }

            break;
        }
    }
}

auto NetworkBase::GetPlayerIteratorByID(uint8_t id) const
{
    return std::find_if(player_list.begin(), player_list.end(), [id](std::unique_ptr<NetworkPlayer> const& player) {
        return player->Id == id;
    });
}

NetworkPlayer* NetworkBase::GetPlayerByID(uint8_t id) const
{
    auto it = GetPlayerIteratorByID(id);
    if (it != player_list.end())
    {
        return it->get();
    }
    return nullptr;
}

auto NetworkBase::GetGroupIteratorByID(uint8_t id) const
{
    return std::find_if(
        group_list.begin(), group_list.end(), [id](std::unique_ptr<NetworkGroup> const& group) { return group->Id == id; });
}

NetworkGroup* NetworkBase::GetGroupByID(uint8_t id) const
{
    auto it = GetGroupIteratorByID(id);
    if (it != group_list.end())
    {
        return it->get();
    }
    return nullptr;
}

const char* NetworkBase::FormatChat(NetworkPlayer* fromplayer, const char* text)
{
    static std::string formatted;
    formatted.clear();
    formatted += "{OUTLINE}";
    if (fromplayer != nullptr)
    {
        formatted += "{BABYBLUE}";
        formatted += fromplayer->Name;
        formatted += ": ";
    }
    formatted += "{WHITE}";
    formatted += text;
    return formatted.c_str();
}

void NetworkBase::SendPacketToClients(const NetworkPacket& packet, bool front, bool gameCmd) const
{
    for (auto& client_connection : client_connection_list)
    {
        if (gameCmd)
        {
            // If marked as game command we can not send the packet to connections that are not fully connected.
            // Sending the packet would cause the client to store a command that is behind the tick where he starts,
            // which would be essentially never executed. The clients do not require commands before the server has not sent the
            // map data.
            if (client_connection->Player == nullptr)
            {
                continue;
            }
        }
        client_connection->QueuePacket(packet, front);
    }
}

bool NetworkBase::CheckSRAND(uint32_t tick, uint32_t srand0)
{
    // We have to wait for the map to be loaded first, ticks may match current loaded map.
    if (!_clientMapLoaded)
        return true;

    auto itTickData = _serverTickData.find(tick);
    if (itTickData == std::end(_serverTickData))
        return true;

    const ServerTickData_t storedTick = itTickData->second;
    _serverTickData.erase(itTickData);

    if (storedTick.srand0 != srand0)
    {
        log_info("Srand0 mismatch, client = %08X, server = %08X", srand0, storedTick.srand0);
        return false;
    }

    if (!storedTick.spriteHash.empty())
    {
        EntitiesChecksum checksum = GetAllEntitiesChecksum();
        std::string clientSpriteHash = checksum.ToString();
        if (clientSpriteHash != storedTick.spriteHash)
        {
            log_info("Sprite hash mismatch, client = %s, server = %s", clientSpriteHash.c_str(), storedTick.spriteHash.c_str());
            return false;
        }
    }

    return true;
}

bool NetworkBase::IsDesynchronised() const noexcept
{
    return _serverState.state == NetworkServerState::Desynced;
}

bool NetworkBase::CheckDesynchronizaton()
{
    // Check synchronisation
    if (GetMode() == NETWORK_MODE_CLIENT && _serverState.state != NetworkServerState::Desynced
        && !CheckSRAND(gCurrentTicks, scenario_rand_state().s0))
    {
        _serverState.state = NetworkServerState::Desynced;
        _serverState.desyncTick = gCurrentTicks;

        char str_desync[256];
        format_string(str_desync, 256, STR_MULTIPLAYER_DESYNC, nullptr);

        auto intent = Intent(WC_NETWORK_STATUS);
        intent.putExtra(INTENT_EXTRA_MESSAGE, std::string{ str_desync });
        context_open_intent(&intent);

        if (!gConfigNetwork.stay_connected)
        {
            Close();
        }

        return true;
    }

    return false;
}

void NetworkBase::RequestStateSnapshot()
{
    log_info("Requesting game state for tick %u", _serverState.desyncTick);

    Client_Send_RequestGameState(_serverState.desyncTick);
}

NetworkServerState_t NetworkBase::GetServerState() const noexcept
{
    return _serverState;
}

void NetworkBase::KickPlayer(int32_t playerId)
{
    for (auto& client_connection : client_connection_list)
    {
        if (client_connection->Player->Id == playerId)
        {
            // Disconnect the client gracefully
            client_connection->SetLastDisconnectReason(STR_MULTIPLAYER_KICKED);
            char str_disconnect_msg[256];
            format_string(str_disconnect_msg, 256, STR_MULTIPLAYER_KICKED_REASON, nullptr);
            Server_Send_SETDISCONNECTMSG(*client_connection, str_disconnect_msg);
            client_connection->Disconnect();
            break;
        }
    }
}

void NetworkBase::SetPassword(u8string_view password)
{
    _password = password;
}

void NetworkBase::ServerClientDisconnected()
{
    if (GetMode() == NETWORK_MODE_CLIENT)
    {
        _serverConnection->Disconnect();
    }
}

std::string NetworkBase::GenerateAdvertiseKey()
{
    // Generate a string of 16 random hex characters (64-integer key as a hex formatted string)
    static char hexChars[] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
    };
    char key[17];
    for (int32_t i = 0; i < 16; i++)
    {
        int32_t hexCharIndex = util_rand() % std::size(hexChars);
        key[i] = hexChars[hexCharIndex];
    }
    key[std::size(key) - 1] = 0;

    return key;
}

std::string NetworkBase::GetMasterServerUrl()
{
    if (gConfigNetwork.master_server_url.empty())
    {
        return OPENRCT2_MASTER_SERVER_URL;
    }

    return gConfigNetwork.master_server_url;
}

NetworkGroup* NetworkBase::AddGroup()
{
    NetworkGroup* addedgroup = nullptr;
    int32_t newid = -1;
    // Find first unused group id
    for (int32_t id = 0; id < 255; id++)
    {
        if (std::find_if(
                group_list.begin(), group_list.end(),
                [&id](std::unique_ptr<NetworkGroup> const& group) { return group->Id == id; })
            == group_list.end())
        {
            newid = id;
            break;
        }
    }
    if (newid != -1)
    {
        auto group = std::make_unique<NetworkGroup>();
        group->Id = newid;
        group->SetName("Group #" + std::to_string(newid));
        addedgroup = group.get();
        group_list.push_back(std::move(group));
    }
    return addedgroup;
}

void NetworkBase::RemoveGroup(uint8_t id)
{
    auto group = GetGroupIteratorByID(id);
    if (group != group_list.end())
    {
        group_list.erase(group);
    }

    if (GetMode() == NETWORK_MODE_SERVER)
    {
        _userManager.UnsetUsersOfGroup(id);
        _userManager.Save();
    }
}

uint8_t NetworkBase::GetGroupIDByHash(const std::string& keyhash)
{
    const NetworkUser* networkUser = _userManager.GetUserByHash(keyhash);

    uint8_t groupId = GetDefaultGroup();
    if (networkUser != nullptr && networkUser->GroupId.has_value())
    {
        const uint8_t assignedGroup = *networkUser->GroupId;
        if (GetGroupByID(assignedGroup) != nullptr)
        {
            groupId = assignedGroup;
        }
        else
        {
            log_warning(
                "User %s is assigned to non-existent group %u. Assigning to default group (%u)", keyhash.c_str(), assignedGroup,
                groupId);
        }
    }
    return groupId;
}

uint8_t NetworkBase::GetDefaultGroup() const noexcept
{
    return default_group;
}

void NetworkBase::SetDefaultGroup(uint8_t id)
{
    if (GetGroupByID(id) != nullptr)
    {
        default_group = id;
    }
}

void NetworkBase::SaveGroups()
{
    if (GetMode() == NETWORK_MODE_SERVER)
    {
        auto env = GetContext().GetPlatformEnvironment();
        auto path = Path::Combine(env->GetDirectoryPath(DIRBASE::USER), u8"groups.json");

        json_t jsonGroups = json_t::array();
        for (auto& group : group_list)
        {
            jsonGroups.push_back(group->ToJson());
        }
        json_t jsonGroupsCfg = {
            { "default_group", default_group },
            { "groups", jsonGroups },
        };
        try
        {
            Json::WriteToFile(path, jsonGroupsCfg);
        }
        catch (const std::exception& ex)
        {
            log_error("Unable to save %s: %s", path.c_str(), ex.what());
        }
    }
}

void NetworkBase::SetupDefaultGroups()
{
    // Admin group
    auto admin = std::make_unique<NetworkGroup>();
    admin->SetName("Admin");
    admin->ActionsAllowed.fill(0xFF);
    admin->Id = 0;
    group_list.push_back(std::move(admin));

    // Spectator group
    auto spectator = std::make_unique<NetworkGroup>();
    spectator->SetName("Spectator");
    spectator->ToggleActionPermission(NetworkPermission::Chat);
    spectator->Id = 1;
    group_list.push_back(std::move(spectator));

    // User group
    auto user = std::make_unique<NetworkGroup>();
    user->SetName("User");
    user->ActionsAllowed.fill(0xFF);
    user->ToggleActionPermission(NetworkPermission::KickPlayer);
    user->ToggleActionPermission(NetworkPermission::ModifyGroups);
    user->ToggleActionPermission(NetworkPermission::SetPlayerGroup);
    user->ToggleActionPermission(NetworkPermission::Cheat);
    user->ToggleActionPermission(NetworkPermission::PasswordlessLogin);
    user->ToggleActionPermission(NetworkPermission::ModifyTile);
    user->ToggleActionPermission(NetworkPermission::EditScenarioOptions);
    user->Id = 2;
    group_list.push_back(std::move(user));

    SetDefaultGroup(1);
}

void NetworkBase::LoadGroups()
{
    group_list.clear();

    auto env = GetContext().GetPlatformEnvironment();
    auto path = Path::Combine(env->GetDirectoryPath(DIRBASE::USER), u8"groups.json");

    json_t jsonGroupConfig;
    if (File::Exists(path))
    {
        try
        {
            jsonGroupConfig = Json::ReadFromFile(path);
        }
        catch (const std::exception& e)
        {
            log_error("Failed to read %s as JSON. Setting default groups. %s", path.c_str(), e.what());
        }
    }

    if (!jsonGroupConfig.is_object())
    {
        SetupDefaultGroups();
    }
    else
    {
        json_t jsonGroups = jsonGroupConfig["groups"];
        if (jsonGroups.is_array())
        {
            for (auto& jsonGroup : jsonGroups)
            {
                group_list.emplace_back(std::make_unique<NetworkGroup>(NetworkGroup::FromJson(jsonGroup)));
            }
        }

        default_group = Json::GetNumber<uint8_t>(jsonGroupConfig["default_group"]);
        if (GetGroupByID(default_group) == nullptr)
        {
            default_group = 0;
        }
    }

    // Host group should always contain all permissions.
    group_list.at(0)->ActionsAllowed.fill(0xFF);
}

std::string NetworkBase::BeginLog(const std::string& directory, const std::string& midName, const std::string& filenameFormat)
{
    utf8 filename[256];
    time_t timer;
    time(&timer);
    auto tmInfo = localtime(&timer);
    if (strftime(filename, sizeof(filename), filenameFormat.c_str(), tmInfo) == 0)
    {
        throw std::runtime_error("strftime failed");
    }

    Platform::EnsureDirectoryExists(Path::Combine(directory, midName).c_str());
    return Path::Combine(directory, midName, filename);
}

void NetworkBase::AppendLog(std::ostream& fs, std::string_view s)
{
    if (fs.fail())
    {
        log_error("bad ostream failed to append log");
        return;
    }
    try
    {
        utf8 buffer[1024];
        time_t timer;
        time(&timer);
        auto tmInfo = localtime(&timer);
        if (strftime(buffer, sizeof(buffer), "[%Y/%m/%d %H:%M:%S] ", tmInfo) != 0)
        {
            String::Append(buffer, sizeof(buffer), std::string(s).c_str());
            String::Append(buffer, sizeof(buffer), PLATFORM_NEWLINE);

            fs.write(buffer, strlen(buffer));
        }
    }
    catch (const std::exception& ex)
    {
        log_error("%s", ex.what());
    }
}

void NetworkBase::BeginChatLog()
{
    auto env = GetContext().GetPlatformEnvironment();
    auto directory = env->GetDirectoryPath(DIRBASE::USER, DIRID::LOG_CHAT);
    _chatLogPath = BeginLog(directory, "", _chatLogFilenameFormat);
    _chat_log_fs.open(fs::u8path(_chatLogPath), std::ios::out | std::ios::app);
}

void NetworkBase::AppendChatLog(std::string_view s)
{
    if (gConfigNetwork.log_chat && _chat_log_fs.is_open())
    {
        AppendLog(_chat_log_fs, s);
    }
}

void NetworkBase::CloseChatLog()
{
    _chat_log_fs.close();
}

void NetworkBase::BeginServerLog()
{
    auto env = GetContext().GetPlatformEnvironment();
    auto directory = env->GetDirectoryPath(DIRBASE::USER, DIRID::LOG_SERVER);
    _serverLogPath = BeginLog(directory, ServerName, _serverLogFilenameFormat);
    _server_log_fs.open(fs::u8path(_serverLogPath), std::ios::out | std::ios::app | std::ios::binary);

    // Log server start event
    utf8 logMessage[256];
    if (GetMode() == NETWORK_MODE_CLIENT)
    {
        format_string(logMessage, sizeof(logMessage), STR_LOG_CLIENT_STARTED, nullptr);
    }
    else if (GetMode() == NETWORK_MODE_SERVER)
    {
        format_string(logMessage, sizeof(logMessage), STR_LOG_SERVER_STARTED, nullptr);
    }
    else
    {
        logMessage[0] = '\0';
        Guard::Assert(false, "Unknown network mode!");
    }
    AppendServerLog(logMessage);
}

void NetworkBase::AppendServerLog(const std::string& s)
{
    if (gConfigNetwork.log_server_actions && _server_log_fs.is_open())
    {
        AppendLog(_server_log_fs, s);
    }
}

void NetworkBase::CloseServerLog()
{
    // Log server stopped event
    char logMessage[256];
    if (GetMode() == NETWORK_MODE_CLIENT)
    {
        format_string(logMessage, sizeof(logMessage), STR_LOG_CLIENT_STOPPED, nullptr);
    }
    else if (GetMode() == NETWORK_MODE_SERVER)
    {
        format_string(logMessage, sizeof(logMessage), STR_LOG_SERVER_STOPPED, nullptr);
    }
    else
    {
        logMessage[0] = '\0';
        Guard::Assert(false, "Unknown network mode!");
    }
    AppendServerLog(logMessage);
    _server_log_fs.close();
}

void NetworkBase::Client_Send_RequestGameState(uint32_t tick)
{
    if (_serverState.gamestateSnapshotsEnabled == false)
    {
        log_verbose("Server does not store a gamestate history");
        return;
    }

    log_verbose("Requesting gamestate from server for tick %u", tick);

    NetworkPacket packet(NetworkCommand::RequestGameState);
    packet << tick;
    _serverConnection->QueuePacket(std::move(packet));
}

void NetworkBase::Client_Send_TOKEN()
{
    log_verbose("requesting token");
    NetworkPacket packet(NetworkCommand::Token);
    _serverConnection->AuthStatus = NetworkAuth::Requested;
    _serverConnection->QueuePacket(std::move(packet));
}

void NetworkBase::Client_Send_AUTH(
    const std::string& name, const std::string& password, const std::string& pubkey, const std::vector<uint8_t>& signature)
{
    NetworkPacket packet(NetworkCommand::Auth);
    packet.WriteString(network_get_version());
    packet.WriteString(name);
    packet.WriteString(password);
    packet.WriteString(pubkey);
    assert(signature.size() <= static_cast<size_t>(UINT32_MAX));
    packet << static_cast<uint32_t>(signature.size());
    packet.Write(signature.data(), signature.size());
    _serverConnection->AuthStatus = NetworkAuth::Requested;
    _serverConnection->QueuePacket(std::move(packet));
}

void NetworkBase::Client_Send_MAPREQUEST(const std::vector<ObjectEntryDescriptor>& objects)
{
    log_verbose("client requests %u objects", uint32_t(objects.size()));
    NetworkPacket packet(NetworkCommand::MapRequest);
    packet << static_cast<uint32_t>(objects.size());
    for (const auto& object : objects)
    {
        std::string name(object.GetName());
        log_verbose("client requests object %s", name.c_str());
        if (object.Generation == ObjectGeneration::DAT)
        {
            packet << static_cast<uint8_t>(0);
            packet.Write(&object.Entry, sizeof(rct_object_entry));
        }
        else
        {
            packet << static_cast<uint8_t>(1);
            packet.WriteString(name);
        }
    }
    _serverConnection->QueuePacket(std::move(packet));
}

void NetworkBase::Server_Send_TOKEN(NetworkConnection& connection)
{
    NetworkPacket packet(NetworkCommand::Token);
    packet << static_cast<uint32_t>(connection.Challenge.size());
    packet.Write(connection.Challenge.data(), connection.Challenge.size());
    connection.QueuePacket(std::move(packet));
}

void NetworkBase::Server_Send_OBJECTS_LIST(
    NetworkConnection& connection, const std::vector<const ObjectRepositoryItem*>& objects) const
{
    log_verbose("Server sends objects list with %u items", objects.size());

    if (objects.empty())
    {
        NetworkPacket packet(NetworkCommand::ObjectsList);
        packet << static_cast<uint32_t>(0) << static_cast<uint32_t>(objects.size());

        connection.QueuePacket(std::move(packet));
    }
    else
    {
        for (size_t i = 0; i < objects.size(); ++i)
        {
            const auto* object = objects[i];

            NetworkPacket packet(NetworkCommand::ObjectsList);
            packet << static_cast<uint32_t>(i) << static_cast<uint32_t>(objects.size());

            if (object->Identifier.empty())
            {
                // DAT
                log_verbose("Object %.8s (checksum %x)", object->ObjectEntry.name, object->ObjectEntry.checksum);
                packet << static_cast<uint8_t>(0);
                packet.Write(&object->ObjectEntry, sizeof(rct_object_entry));
            }
            else
            {
                // JSON
                log_verbose("Object %s", object->Identifier.c_str());
                packet << static_cast<uint8_t>(1);
                packet.WriteString(object->Identifier);
            }

            connection.QueuePacket(std::move(packet));
        }
    }
}

void NetworkBase::Server_Send_SCRIPTS(NetworkConnection& connection)
{
    NetworkPacket packet(NetworkCommand::Scripts);

#    ifdef ENABLE_SCRIPTING
    using namespace OpenRCT2::Scripting;

    auto& scriptEngine = GetContext().GetScriptEngine();
    const auto& plugins = scriptEngine.GetPlugins();
    std::vector<std::shared_ptr<Plugin>> pluginsToSend;
    for (const auto& plugin : plugins)
    {
        const auto& metadata = plugin->GetMetadata();
        if (metadata.Type == OpenRCT2::Scripting::PluginType::Remote)
        {
            pluginsToSend.push_back(plugin);
        }
    }

    log_verbose("Server sends %u scripts", pluginsToSend.size());
    packet << static_cast<uint32_t>(pluginsToSend.size());
    for (const auto& plugin : pluginsToSend)
    {
        const auto& metadata = plugin->GetMetadata();
        log_verbose("Script %s", metadata.Name.c_str());

        const auto& code = plugin->GetCode();
        packet << static_cast<uint32_t>(code.size());
        packet.Write(reinterpret_cast<const uint8_t*>(code.c_str()), code.size());
    }
#    else
    packet << static_cast<uint32_t>(0);
#    endif
    connection.QueuePacket(std::move(packet));
}

void NetworkBase::Client_Send_HEARTBEAT(NetworkConnection& connection) const
{
    log_verbose("Sending heartbeat");

    NetworkPacket packet(NetworkCommand::Heartbeat);
    connection.QueuePacket(std::move(packet));
}

NetworkStats_t NetworkBase::GetStats() const
{
    NetworkStats_t stats = {};
    if (mode == NETWORK_MODE_CLIENT)
    {
        stats = _serverConnection->Stats;
    }
    else
    {
        for (auto& connection : client_connection_list)
        {
            for (size_t n = 0; n < EnumValue(NetworkStatisticsGroup::Max); n++)
            {
                stats.bytesReceived[n] += connection->Stats.bytesReceived[n];
                stats.bytesSent[n] += connection->Stats.bytesSent[n];
            }
        }
    }
    return stats;
}

void NetworkBase::Server_Send_AUTH(NetworkConnection& connection)
{
    uint8_t new_playerid = 0;
    if (connection.Player != nullptr)
    {
        new_playerid = connection.Player->Id;
    }
    NetworkPacket packet(NetworkCommand::Auth);
    packet << static_cast<uint32_t>(connection.AuthStatus) << new_playerid;
    if (connection.AuthStatus == NetworkAuth::BadVersion)
    {
        packet.WriteString(network_get_version());
    }
    connection.QueuePacket(std::move(packet));
    if (connection.AuthStatus != NetworkAuth::Ok && connection.AuthStatus != NetworkAuth::RequirePassword)
    {
        connection.Disconnect();
    }
}

void NetworkBase::Server_Send_MAP(NetworkConnection* connection)
{
    std::vector<const ObjectRepositoryItem*> objects;
    if (connection != nullptr)
    {
        objects = connection->RequestedObjects;
    }
    else
    {
        // This will send all custom objects to connected clients
        // TODO: fix it so custom objects negotiation is performed even in this case.
        auto& context = GetContext();
        auto& objManager = context.GetObjectManager();
        objects = objManager.GetPackableObjects();
    }

    auto header = save_for_network(objects);
    if (header.empty())
    {
        if (connection != nullptr)
        {
            connection->SetLastDisconnectReason(STR_MULTIPLAYER_CONNECTION_CLOSED);
            connection->Disconnect();
        }
        return;
    }
    size_t chunksize = CHUNK_SIZE;
    for (size_t i = 0; i < header.size(); i += chunksize)
    {
        size_t datasize = std::min(chunksize, header.size() - i);
        NetworkPacket packet(NetworkCommand::Map);
        packet << static_cast<uint32_t>(header.size()) << static_cast<uint32_t>(i);
        packet.Write(&header[i], datasize);
        if (connection != nullptr)
        {
            connection->QueuePacket(std::move(packet));
        }
        else
        {
            SendPacketToClients(packet);
        }
    }
}

std::vector<uint8_t> NetworkBase::save_for_network(const std::vector<const ObjectRepositoryItem*>& objects) const
{
    std::vector<uint8_t> result;
    auto ms = OpenRCT2::MemoryStream();
    if (SaveMap(&ms, objects))
    {
        result.resize(ms.GetLength());
        std::memcpy(result.data(), ms.GetData(), result.size());
    }
    else
    {
        log_warning("Failed to export map.");
    }
    return result;
}

void NetworkBase::Client_Send_CHAT(const char* text)
{
    NetworkPacket packet(NetworkCommand::Chat);
    packet.WriteString(text);
    _serverConnection->QueuePacket(std::move(packet));
}

void NetworkBase::Server_Send_CHAT(const char* text, const std::vector<uint8_t>& playerIds)
{
    NetworkPacket packet(NetworkCommand::Chat);
    packet.WriteString(text);

    if (playerIds.empty())
    {
        // Empty players / default value means send to all players
        SendPacketToClients(packet);
    }
    else
    {
        for (auto playerId : playerIds)
        {
            auto conn = GetPlayerConnection(playerId);
            if (conn != nullptr)
            {
                conn->QueuePacket(packet);
            }
        }
    }
}

void NetworkBase::Client_Send_GAME_ACTION(const GameAction* action)
{
    NetworkPacket packet(NetworkCommand::GameAction);

    uint32_t networkId = 0;
    networkId = ++_actionId;

    // I know its ugly, want basic functionality for now.
    const_cast<GameAction*>(action)->SetNetworkId(networkId);
    if (action->GetCallback())
    {
        _gameActionCallbacks.insert(std::make_pair(networkId, action->GetCallback()));
    }

    DataSerialiser stream(true);
    action->Serialise(stream);

    packet << gCurrentTicks << action->GetType() << stream;
    _serverConnection->QueuePacket(std::move(packet));
}

void NetworkBase::Server_Send_GAME_ACTION(const GameAction* action)
{
    NetworkPacket packet(NetworkCommand::GameAction);

    DataSerialiser stream(true);
    action->Serialise(stream);

    packet << gCurrentTicks << action->GetType() << stream;

    SendPacketToClients(packet);
}

void NetworkBase::Server_Send_TICK()
{
    NetworkPacket packet(NetworkCommand::Tick);
    packet << gCurrentTicks << scenario_rand_state().s0;
    uint32_t flags = 0;
    // Simple counter which limits how often a sprite checksum gets sent.
    // This can get somewhat expensive, so we don't want to push it every tick in release,
    // but debug version can check more often.
    static int32_t checksum_counter = 0;
    checksum_counter++;
    if (checksum_counter >= 100)
    {
        checksum_counter = 0;
        flags |= NETWORK_TICK_FLAG_CHECKSUMS;
    }
    // Send flags always, so we can understand packet structure on the other end,
    // and allow for some expansion.
    packet << flags;
    if (flags & NETWORK_TICK_FLAG_CHECKSUMS)
    {
        EntitiesChecksum checksum = GetAllEntitiesChecksum();
        packet.WriteString(checksum.ToString());
    }

    SendPacketToClients(packet);
}

void NetworkBase::Server_Send_PLAYERINFO(int32_t playerId)
{
    NetworkPacket packet(NetworkCommand::PlayerInfo);
    packet << gCurrentTicks;

    auto* player = GetPlayerByID(playerId);
    if (player == nullptr)
        return;

    player->Write(packet);
    SendPacketToClients(packet);
}

void NetworkBase::Server_Send_PLAYERLIST()
{
    NetworkPacket packet(NetworkCommand::PlayerList);
    packet << gCurrentTicks << static_cast<uint8_t>(player_list.size());
    for (auto& player : player_list)
    {
        player->Write(packet);
    }
    SendPacketToClients(packet);
}

void NetworkBase::Client_Send_PING()
{
    NetworkPacket packet(NetworkCommand::Ping);
    _serverConnection->QueuePacket(std::move(packet));
}

void NetworkBase::Server_Send_PING()
{
    last_ping_sent_time = Platform::GetTicks();
    NetworkPacket packet(NetworkCommand::Ping);
    for (auto& client_connection : client_connection_list)
    {
        client_connection->PingTime = Platform::GetTicks();
    }
    SendPacketToClients(packet, true);
}

void NetworkBase::Server_Send_PINGLIST()
{
    NetworkPacket packet(NetworkCommand::PingList);
    packet << static_cast<uint8_t>(player_list.size());
    for (auto& player : player_list)
    {
        packet << player->Id << player->Ping;
    }
    SendPacketToClients(packet);
}

void NetworkBase::Server_Send_SETDISCONNECTMSG(NetworkConnection& connection, const char* msg)
{
    NetworkPacket packet(NetworkCommand::DisconnectMessage);
    packet.WriteString(msg);
    connection.QueuePacket(std::move(packet));
}

json_t NetworkBase::GetServerInfoAsJson() const
{
    json_t jsonObj = {
        { "name", gConfigNetwork.server_name },         { "requiresPassword", _password.size() > 0 },
        { "version", network_get_version() },           { "players", player_list.size() },
        { "maxPlayers", gConfigNetwork.maxplayers },    { "description", gConfigNetwork.server_description },
        { "greeting", gConfigNetwork.server_greeting }, { "dedicated", gOpenRCT2Headless },
    };
    return jsonObj;
}

void NetworkBase::Server_Send_GAMEINFO(NetworkConnection& connection)
{
    NetworkPacket packet(NetworkCommand::GameInfo);
#    ifndef DISABLE_HTTP
    json_t jsonObj = GetServerInfoAsJson();

    // Provider details
    json_t jsonProvider = {
        { "name", gConfigNetwork.provider_name },
        { "email", gConfigNetwork.provider_email },
        { "website", gConfigNetwork.provider_website },
    };

    jsonObj["provider"] = jsonProvider;

    packet.WriteString(jsonObj.dump());
    packet << _serverState.gamestateSnapshotsEnabled;

#    endif
    connection.QueuePacket(std::move(packet));
}

void NetworkBase::Server_Send_SHOWERROR(NetworkConnection& connection, StringId title, StringId message)
{
    NetworkPacket packet(NetworkCommand::ShowError);
    packet << title << message;
    connection.QueuePacket(std::move(packet));
}

void NetworkBase::Server_Send_GROUPLIST(NetworkConnection& connection)
{
    NetworkPacket packet(NetworkCommand::GroupList);
    packet << static_cast<uint8_t>(group_list.size()) << default_group;
    for (auto& group : group_list)
    {
        group->Write(packet);
    }
    connection.QueuePacket(std::move(packet));
}

void NetworkBase::Server_Send_EVENT_PLAYER_JOINED(const char* playerName)
{
    NetworkPacket packet(NetworkCommand::Event);
    packet << static_cast<uint16_t>(SERVER_EVENT_PLAYER_JOINED);
    packet.WriteString(playerName);
    SendPacketToClients(packet);
}

void NetworkBase::Server_Send_EVENT_PLAYER_DISCONNECTED(const char* playerName, const char* reason)
{
    NetworkPacket packet(NetworkCommand::Event);
    packet << static_cast<uint16_t>(SERVER_EVENT_PLAYER_DISCONNECTED);
    packet.WriteString(playerName);
    packet.WriteString(reason);
    SendPacketToClients(packet);
}

bool NetworkBase::ProcessConnection(NetworkConnection& connection)
{
    NetworkReadPacket packetStatus;

    uint32_t countProcessed = 0;
    do
    {
        countProcessed++;
        packetStatus = connection.ReadPacket();
        switch (packetStatus)
        {
            case NetworkReadPacket::Disconnected:
                // closed connection or network error
                if (!connection.GetLastDisconnectReason())
                {
                    connection.SetLastDisconnectReason(STR_MULTIPLAYER_CONNECTION_CLOSED);
                }
                return false;
            case NetworkReadPacket::Success:
                // done reading in packet
                ProcessPacket(connection, connection.InboundPacket);
                if (!connection.IsValid())
                {
                    return false;
                }
                break;
            case NetworkReadPacket::MoreData:
                // more data required to be read
                break;
            case NetworkReadPacket::NoData:
                // could not read anything from socket
                break;
        }
    } while (packetStatus == NetworkReadPacket::Success && countProcessed < MaxPacketsPerUpdate);

    if (!connection.ReceivedPacketRecently())
    {
        if (!connection.GetLastDisconnectReason())
        {
            connection.SetLastDisconnectReason(STR_MULTIPLAYER_NO_DATA);
        }
        return false;
    }

    return true;
}

void NetworkBase::ProcessPacket(NetworkConnection& connection, NetworkPacket& packet)
{
    const auto& handlerList = GetMode() == NETWORK_MODE_SERVER ? server_command_handlers : client_command_handlers;

    auto it = handlerList.find(packet.GetCommand());
    if (it != handlerList.end())
    {
        auto commandHandler = it->second;
        if (connection.AuthStatus == NetworkAuth::Ok || !packet.CommandRequiresAuth())
        {
            try
            {
                (this->*commandHandler)(connection, packet);
            }
            catch (const std::exception& ex)
            {
                log_verbose("Exception during packet processing: %s", ex.what());
            }
        }
    }

    packet.Clear();
}

// This is called at the end of each game tick, this where things should be processed that affects the game state.
void NetworkBase::ProcessPending()
{
    if (GetMode() == NETWORK_MODE_SERVER)
    {
        ProcessDisconnectedClients();
    }
    else if (GetMode() == NETWORK_MODE_CLIENT)
    {
        ProcessPlayerInfo();
    }
    ProcessPlayerList();
}

static bool ProcessPlayerAuthenticatePluginHooks(
    const NetworkConnection& connection, std::string_view name, std::string_view publicKeyHash)
{
#    ifdef ENABLE_SCRIPTING
    using namespace OpenRCT2::Scripting;

    auto& hookEngine = GetContext()->GetScriptEngine().GetHookEngine();
    if (hookEngine.HasSubscriptions(OpenRCT2::Scripting::HOOK_TYPE::NETWORK_AUTHENTICATE))
    {
        auto ctx = GetContext()->GetScriptEngine().GetContext();

        // Create event args object
        DukObject eObj(ctx);
        eObj.Set("name", name);
        eObj.Set("publicKeyHash", publicKeyHash);
        eObj.Set("ipAddress", connection.Socket->GetIpAddress());
        eObj.Set("cancel", false);
        auto e = eObj.Take();

        // Call the subscriptions
        hookEngine.Call(OpenRCT2::Scripting::HOOK_TYPE::NETWORK_AUTHENTICATE, e, false);

        // Check if any hook has cancelled the join
        if (AsOrDefault(e["cancel"], false))
        {
            return false;
        }
    }
#    endif
    return true;
}

static void ProcessPlayerJoinedPluginHooks(uint8_t playerId)
{
#    ifdef ENABLE_SCRIPTING
    using namespace OpenRCT2::Scripting;

    auto& hookEngine = GetContext()->GetScriptEngine().GetHookEngine();
    if (hookEngine.HasSubscriptions(OpenRCT2::Scripting::HOOK_TYPE::NETWORK_JOIN))
    {
        auto ctx = GetContext()->GetScriptEngine().GetContext();

        // Create event args object
        DukObject eObj(ctx);
        eObj.Set("player", playerId);
        auto e = eObj.Take();

        // Call the subscriptions
        hookEngine.Call(OpenRCT2::Scripting::HOOK_TYPE::NETWORK_JOIN, e, false);
    }
#    endif
}

static void ProcessPlayerLeftPluginHooks(uint8_t playerId)
{
#    ifdef ENABLE_SCRIPTING
    using namespace OpenRCT2::Scripting;

    auto& hookEngine = GetContext()->GetScriptEngine().GetHookEngine();
    if (hookEngine.HasSubscriptions(OpenRCT2::Scripting::HOOK_TYPE::NETWORK_LEAVE))
    {
        auto ctx = GetContext()->GetScriptEngine().GetContext();

        // Create event args object
        DukObject eObj(ctx);
        eObj.Set("player", playerId);
        auto e = eObj.Take();

        // Call the subscriptions
        hookEngine.Call(OpenRCT2::Scripting::HOOK_TYPE::NETWORK_LEAVE, e, false);
    }
#    endif
}

void NetworkBase::ProcessPlayerList()
{
    if (GetMode() == NETWORK_MODE_SERVER)
    {
        // Avoid sending multiple times the player list, we mark the list invalidated on modifications
        // and then send at the end of the tick the final player list.
        if (_playerListInvalidated)
        {
            _playerListInvalidated = false;
            Server_Send_PLAYERLIST();
        }
    }
    else
    {
        // As client we have to keep things in order so the update is tick bound.
        // Commands/Actions reference players and so this list needs to be in sync with those.
        auto itPending = _pendingPlayerLists.begin();
        while (itPending != _pendingPlayerLists.end())
        {
            if (itPending->first > gCurrentTicks)
                break;

            // List of active players found in the list.
            std::vector<uint8_t> activePlayerIds;
            std::vector<uint8_t> newPlayers;
            std::vector<uint8_t> removedPlayers;

            for (const auto& pendingPlayer : itPending->second.players)
            {
                activePlayerIds.push_back(pendingPlayer.Id);

                auto* player = GetPlayerByID(pendingPlayer.Id);
                if (player == nullptr)
                {
                    // Add new player.
                    player = AddPlayer("", "");
                    if (player != nullptr)
                    {
                        *player = pendingPlayer;
                        if (player->Flags & NETWORK_PLAYER_FLAG_ISSERVER)
                        {
                            _serverConnection->Player = player;
                        }
                        newPlayers.push_back(player->Id);
                    }
                }
                else
                {
                    // Update.
                    *player = pendingPlayer;
                }
            }

            // Remove any players that are not in newly received list
            for (const auto& player : player_list)
            {
                if (std::find(activePlayerIds.begin(), activePlayerIds.end(), player->Id) == activePlayerIds.end())
                {
                    removedPlayers.push_back(player->Id);
                }
            }

            // Run player removed hooks (must be before players removed from list)
            for (auto playerId : removedPlayers)
            {
                ProcessPlayerLeftPluginHooks(playerId);
            }

            // Run player joined hooks (must be after players added to list)
            for (auto playerId : newPlayers)
            {
                ProcessPlayerJoinedPluginHooks(playerId);
            }

            // Now actually remove removed players from player list
            player_list.erase(
                std::remove_if(
                    player_list.begin(), player_list.end(),
                    [&removedPlayers](const std::unique_ptr<NetworkPlayer>& player) {
                        return std::find(removedPlayers.begin(), removedPlayers.end(), player->Id) != removedPlayers.end();
                    }),
                player_list.end());

            _pendingPlayerLists.erase(itPending);
            itPending = _pendingPlayerLists.begin();
        }
    }
}

void NetworkBase::ProcessPlayerInfo()
{
    auto range = _pendingPlayerInfo.equal_range(gCurrentTicks);
    for (auto it = range.first; it != range.second; it++)
    {
        auto* player = GetPlayerByID(it->second.Id);
        if (player != nullptr)
        {
            const NetworkPlayer& networkedInfo = it->second;
            player->Flags = networkedInfo.Flags;
            player->Group = networkedInfo.Group;
            player->LastAction = networkedInfo.LastAction;
            player->LastActionCoord = networkedInfo.LastActionCoord;
            player->MoneySpent = networkedInfo.MoneySpent;
            player->CommandsRan = networkedInfo.CommandsRan;
        }
    }
    _pendingPlayerInfo.erase(gCurrentTicks);
}

void NetworkBase::ProcessDisconnectedClients()
{
    for (auto it = client_connection_list.begin(); it != client_connection_list.end();)
    {
        auto& connection = *it;

        if (!connection->ShouldDisconnect)
        {
            it++;
            continue;
        }

        // Make sure to send all remaining packets out before disconnecting.
        connection->SendQueuedPackets();
        connection->Socket->Disconnect();

        ServerClientDisconnected(connection);
        RemovePlayer(connection);

        it = client_connection_list.erase(it);
    }
}

void NetworkBase::AddClient(std::unique_ptr<ITcpSocket>&& socket)
{
    // Log connection info.
    char addr[128];
    snprintf(addr, sizeof(addr), "Client joined from %s", socket->GetHostName());
    AppendServerLog(addr);

    // Store connection
    auto connection = std::make_unique<NetworkConnection>();
    connection->Socket = std::move(socket);

    client_connection_list.push_back(std::move(connection));
}

void NetworkBase::ServerClientDisconnected(std::unique_ptr<NetworkConnection>& connection)
{
    NetworkPlayer* connection_player = connection->Player;
    if (connection_player == nullptr)
        return;

    char text[256];
    const char* has_disconnected_args[2] = {
        connection_player->Name.c_str(),
        connection->GetLastDisconnectReason(),
    };
    if (has_disconnected_args[1] != nullptr)
    {
        format_string(text, 256, STR_MULTIPLAYER_PLAYER_HAS_DISCONNECTED_WITH_REASON, has_disconnected_args);
    }
    else
    {
        format_string(text, 256, STR_MULTIPLAYER_PLAYER_HAS_DISCONNECTED_NO_REASON, &(has_disconnected_args[0]));
    }

    chat_history_add(text);
    Peep* pickup_peep = network_get_pickup_peep(connection_player->Id);
    if (pickup_peep != nullptr)
    {
        PeepPickupAction pickupAction{ PeepPickupType::Cancel,
                                       pickup_peep->sprite_index,
                                       { network_get_pickup_peep_old_x(connection_player->Id), 0, 0 },
                                       network_get_current_player_id() };
        auto res = GameActions::Execute(&pickupAction);
    }
    Server_Send_EVENT_PLAYER_DISCONNECTED(
        const_cast<char*>(connection_player->Name.c_str()), connection->GetLastDisconnectReason());

    // Log player disconnected event
    AppendServerLog(text);

    ProcessPlayerLeftPluginHooks(connection_player->Id);
}

void NetworkBase::RemovePlayer(std::unique_ptr<NetworkConnection>& connection)
{
    NetworkPlayer* connection_player = connection->Player;
    if (connection_player == nullptr)
        return;

    player_list.erase(
        std::remove_if(
            player_list.begin(), player_list.end(),
            [connection_player](std::unique_ptr<NetworkPlayer>& player) { return player.get() == connection_player; }),
        player_list.end());

    // Send new player list.
    _playerListInvalidated = true;
}

NetworkPlayer* NetworkBase::AddPlayer(const std::string& name, const std::string& keyhash)
{
    NetworkPlayer* addedplayer = nullptr;
    int32_t newid = -1;
    if (GetMode() == NETWORK_MODE_SERVER)
    {
        // Find first unused player id
        for (int32_t id = 0; id < 255; id++)
        {
            if (std::find_if(
                    player_list.begin(), player_list.end(),
                    [&id](std::unique_ptr<NetworkPlayer> const& player) { return player->Id == id; })
                == player_list.end())
            {
                newid = id;
                break;
            }
        }
    }
    else
    {
        newid = 0;
    }
    if (newid != -1)
    {
        std::unique_ptr<NetworkPlayer> player;
        if (GetMode() == NETWORK_MODE_SERVER)
        {
            // Load keys host may have added manually
            _userManager.Load();

            // Check if the key is registered
            const NetworkUser* networkUser = _userManager.GetUserByHash(keyhash);

            player = std::make_unique<NetworkPlayer>();
            player->Id = newid;
            player->KeyHash = keyhash;
            if (networkUser == nullptr)
            {
                player->Group = GetDefaultGroup();
                if (!name.empty())
                {
                    player->SetName(MakePlayerNameUnique(String::Trim(name)));
                }
            }
            else
            {
                player->Group = networkUser->GroupId.has_value() ? *networkUser->GroupId : GetDefaultGroup();
                player->SetName(networkUser->Name);
            }

            // Send new player list.
            _playerListInvalidated = true;
        }
        else
        {
            player = std::make_unique<NetworkPlayer>();
            player->Id = newid;
            player->Group = GetDefaultGroup();
            player->SetName(String::Trim(std::string(name)));
        }

        addedplayer = player.get();
        player_list.push_back(std::move(player));
    }
    return addedplayer;
}

std::string NetworkBase::MakePlayerNameUnique(const std::string& name)
{
    // Note: Player names are case-insensitive

    std::string new_name = name.substr(0, 31);
    int32_t counter = 1;
    bool unique;
    do
    {
        unique = true;

        // Check if there is already a player with this name in the server
        for (const auto& player : player_list)
        {
            if (String::Equals(player->Name.c_str(), new_name.c_str(), true))
            {
                unique = false;
                break;
            }
        }

        if (unique)
        {
            // Check if there is already a registered player with this name
            if (_userManager.GetUserByName(new_name) != nullptr)
            {
                unique = false;
            }
        }

        if (!unique)
        {
            // Increment name counter
            counter++;
            new_name = name.substr(0, 31) + " #" + std::to_string(counter);
        }
    } while (!unique);
    return new_name;
}

void NetworkBase::Client_Handle_TOKEN(NetworkConnection& connection, NetworkPacket& packet)
{
    auto keyPath = network_get_private_key_path(gConfigNetwork.player_name);
    if (!File::Exists(keyPath))
    {
        log_error("Key file (%s) was not found. Restart client to re-generate it.", keyPath.c_str());
        return;
    }

    try
    {
        auto fs = FileStream(keyPath, FILE_MODE_OPEN);
        if (!_key.LoadPrivate(&fs))
        {
            throw std::runtime_error("Failed to load private key.");
        }
    }
    catch (const std::exception&)
    {
        log_error("Failed to load key %s", keyPath.c_str());
        connection.SetLastDisconnectReason(STR_MULTIPLAYER_VERIFICATION_FAILURE);
        connection.Disconnect();
        return;
    }

    uint32_t challenge_size;
    packet >> challenge_size;
    const char* challenge = reinterpret_cast<const char*>(packet.Read(challenge_size));

    std::vector<uint8_t> signature;
    const std::string pubkey = _key.PublicKeyString();
    _challenge.resize(challenge_size);
    std::memcpy(_challenge.data(), challenge, challenge_size);
    bool ok = _key.Sign(_challenge.data(), _challenge.size(), signature);
    if (!ok)
    {
        log_error("Failed to sign server's challenge.");
        connection.SetLastDisconnectReason(STR_MULTIPLAYER_VERIFICATION_FAILURE);
        connection.Disconnect();
        return;
    }
    // Don't keep private key in memory. There's no need and it may get leaked
    // when process dump gets collected at some point in future.
    _key.Unload();

    Client_Send_AUTH(gConfigNetwork.player_name, gCustomPassword, pubkey, signature);
}

void NetworkBase::Server_Handle_REQUEST_GAMESTATE(NetworkConnection& connection, NetworkPacket& packet)
{
    uint32_t tick;
    packet >> tick;

    if (_serverState.gamestateSnapshotsEnabled == false)
    {
        // Ignore this if this is off.
        return;
    }

    IGameStateSnapshots* snapshots = GetContext().GetGameStateSnapshots();

    const GameStateSnapshot_t* snapshot = snapshots->GetLinkedSnapshot(tick);
    if (snapshot != nullptr)
    {
        MemoryStream snapshotMemory;
        DataSerialiser ds(true, snapshotMemory);

        snapshots->SerialiseSnapshot(const_cast<GameStateSnapshot_t&>(*snapshot), ds);

        uint32_t bytesSent = 0;
        uint32_t length = static_cast<uint32_t>(snapshotMemory.GetLength());
        while (bytesSent < length)
        {
            uint32_t dataSize = CHUNK_SIZE;
            if (bytesSent + dataSize > snapshotMemory.GetLength())
            {
                dataSize = snapshotMemory.GetLength() - bytesSent;
            }

            NetworkPacket packetGameStateChunk(NetworkCommand::GameState);
            packetGameStateChunk << tick << length << bytesSent << dataSize;
            packetGameStateChunk.Write(static_cast<const uint8_t*>(snapshotMemory.GetData()) + bytesSent, dataSize);

            connection.QueuePacket(std::move(packetGameStateChunk));

            bytesSent += dataSize;
        }
    }
}

void NetworkBase::Server_Handle_HEARTBEAT(NetworkConnection& connection, NetworkPacket& packet)
{
    log_verbose("Client %s heartbeat", connection.Socket->GetHostName());
    connection.ResetLastPacketTime();
}

void NetworkBase::Client_Handle_AUTH(NetworkConnection& connection, NetworkPacket& packet)
{
    uint32_t auth_status;
    packet >> auth_status >> const_cast<uint8_t&>(player_id);
    connection.AuthStatus = static_cast<NetworkAuth>(auth_status);
    switch (connection.AuthStatus)
    {
        case NetworkAuth::Ok:
            Client_Send_GAMEINFO();
            break;
        case NetworkAuth::BadName:
            connection.SetLastDisconnectReason(STR_MULTIPLAYER_BAD_PLAYER_NAME);
            connection.Disconnect();
            break;
        case NetworkAuth::BadVersion:
        {
            auto version = std::string(packet.ReadString());
            auto versionp = version.c_str();
            connection.SetLastDisconnectReason(STR_MULTIPLAYER_INCORRECT_SOFTWARE_VERSION, &versionp);
            connection.Disconnect();
            break;
        }
        case NetworkAuth::BadPassword:
            connection.SetLastDisconnectReason(STR_MULTIPLAYER_BAD_PASSWORD);
            connection.Disconnect();
            break;
        case NetworkAuth::VerificationFailure:
            connection.SetLastDisconnectReason(STR_MULTIPLAYER_VERIFICATION_FAILURE);
            connection.Disconnect();
            break;
        case NetworkAuth::Full:
            connection.SetLastDisconnectReason(STR_MULTIPLAYER_SERVER_FULL);
            connection.Disconnect();
            break;
        case NetworkAuth::RequirePassword:
            context_open_window_view(WV_NETWORK_PASSWORD);
            break;
        case NetworkAuth::UnknownKeyDisallowed:
            connection.SetLastDisconnectReason(STR_MULTIPLAYER_UNKNOWN_KEY_DISALLOWED);
            connection.Disconnect();
            break;
        default:
            connection.SetLastDisconnectReason(STR_MULTIPLAYER_RECEIVED_INVALID_DATA);
            connection.Disconnect();
            break;
    }
}

void NetworkBase::Server_Client_Joined(std::string_view name, const std::string& keyhash, NetworkConnection& connection)
{
    auto player = AddPlayer(std::string(name), keyhash);
    connection.Player = player;
    if (player != nullptr)
    {
        char text[256];
        const char* player_name = static_cast<const char*>(player->Name.c_str());
        format_string(text, 256, STR_MULTIPLAYER_PLAYER_HAS_JOINED_THE_GAME, &player_name);
        chat_history_add(text);

        auto& context = GetContext();
        auto& objManager = context.GetObjectManager();
        auto objects = objManager.GetPackableObjects();
        Server_Send_OBJECTS_LIST(connection, objects);
        Server_Send_SCRIPTS(connection);

        // Log player joining event
        std::string playerNameHash = player->Name + " (" + keyhash + ")";
        player_name = static_cast<const char*>(playerNameHash.c_str());
        format_string(text, 256, STR_MULTIPLAYER_PLAYER_HAS_JOINED_THE_GAME, &player_name);
        AppendServerLog(text);

        ProcessPlayerJoinedPluginHooks(player->Id);
    }
}

void NetworkBase::Server_Handle_TOKEN(NetworkConnection& connection, [[maybe_unused]] NetworkPacket& packet)
{
    uint8_t token_size = 10 + (rand() & 0x7f);
    connection.Challenge.resize(token_size);
    for (int32_t i = 0; i < token_size; i++)
    {
        connection.Challenge[i] = static_cast<uint8_t>(rand() & 0xff);
    }
    Server_Send_TOKEN(connection);
}

void NetworkBase::Client_Handle_OBJECTS_LIST(NetworkConnection& connection, NetworkPacket& packet)
{
    auto& repo = GetContext().GetObjectRepository();

    uint32_t index = 0;
    uint32_t totalObjects = 0;
    packet >> index >> totalObjects;

    static constexpr uint32_t OBJECT_START_INDEX = 0;
    if (index == OBJECT_START_INDEX)
    {
        _missingObjects.clear();
    }

    if (totalObjects > 0)
    {
        char objectListMsg[256];
        const uint32_t args[] = {
            index + 1,
            totalObjects,
        };
        format_string(objectListMsg, 256, STR_MULTIPLAYER_RECEIVING_OBJECTS_LIST, &args);

        auto intent = Intent(WC_NETWORK_STATUS);
        intent.putExtra(INTENT_EXTRA_MESSAGE, std::string{ objectListMsg });
        intent.putExtra(INTENT_EXTRA_CALLBACK, []() -> void { ::GetContext()->GetNetwork().Close(); });
        context_open_intent(&intent);

        uint8_t objectType{};
        packet >> objectType;

        if (objectType == 0)
        {
            // DAT
            auto entry = reinterpret_cast<const rct_object_entry*>(packet.Read(sizeof(rct_object_entry)));
            if (entry != nullptr)
            {
                const auto* object = repo.FindObject(entry);
                if (object == nullptr)
                {
                    auto objectName = std::string(entry->GetName());
                    log_verbose("Requesting object %s with checksum %x from server", objectName.c_str(), entry->checksum);
                    _missingObjects.push_back(ObjectEntryDescriptor(*entry));
                }
                else if (object->ObjectEntry.checksum != entry->checksum || object->ObjectEntry.flags != entry->flags)
                {
                    auto objectName = std::string(entry->GetName());
                    log_warning(
                        "Object %s has different checksum/flags (%x/%x) than server (%x/%x).", objectName.c_str(),
                        object->ObjectEntry.checksum, object->ObjectEntry.flags, entry->checksum, entry->flags);
                }
            }
        }
        else
        {
            // JSON
            auto identifier = packet.ReadString();
            if (!identifier.empty())
            {
                const auto* object = repo.FindObject(identifier);
                if (object == nullptr)
                {
                    auto objectName = std::string(identifier);
                    log_verbose("Requesting object %s from server", objectName.c_str());
                    _missingObjects.push_back(ObjectEntryDescriptor(objectName));
                }
            }
        }
    }

    if (index + 1 >= totalObjects)
    {
        log_verbose("client received object list, it has %u entries", totalObjects);
        Client_Send_MAPREQUEST(_missingObjects);
        _missingObjects.clear();
    }
}

void NetworkBase::Client_Handle_SCRIPTS(NetworkConnection& connection, NetworkPacket& packet)
{
    uint32_t numScripts{};
    packet >> numScripts;

#    ifdef ENABLE_SCRIPTING
    auto& scriptEngine = GetContext().GetScriptEngine();
    for (uint32_t i = 0; i < numScripts; i++)
    {
        uint32_t codeLength{};
        packet >> codeLength;
        auto code = std::string_view(reinterpret_cast<const char*>(packet.Read(codeLength)), codeLength);
        scriptEngine.AddNetworkPlugin(code);
    }
#    else
    if (numScripts > 0)
    {
        connection.SetLastDisconnectReason("The server requires plugin support.");
        Close();
    }
#    endif
}

void NetworkBase::Client_Handle_GAMESTATE(NetworkConnection& connection, NetworkPacket& packet)
{
    uint32_t tick;
    uint32_t totalSize;
    uint32_t offset;
    uint32_t dataSize;

    packet >> tick >> totalSize >> offset >> dataSize;

    if (offset == 0)
    {
        // Reset
        _serverGameState = MemoryStream();
    }

    _serverGameState.SetPosition(offset);

    const uint8_t* data = packet.Read(dataSize);
    _serverGameState.Write(data, dataSize);

    log_verbose(
        "Received Game State %.02f%%",
        (static_cast<float>(_serverGameState.GetLength()) / static_cast<float>(totalSize)) * 100.0f);

    if (_serverGameState.GetLength() == totalSize)
    {
        _serverGameState.SetPosition(0);
        DataSerialiser ds(false, _serverGameState);

        IGameStateSnapshots* snapshots = GetContext().GetGameStateSnapshots();

        GameStateSnapshot_t& serverSnapshot = snapshots->CreateSnapshot();
        snapshots->SerialiseSnapshot(serverSnapshot, ds);

        const GameStateSnapshot_t* desyncSnapshot = snapshots->GetLinkedSnapshot(tick);
        if (desyncSnapshot != nullptr)
        {
            GameStateCompareData_t cmpData = snapshots->Compare(serverSnapshot, *desyncSnapshot);

            std::string outputPath = GetContext().GetPlatformEnvironment()->GetDirectoryPath(DIRBASE::USER, DIRID::LOG_DESYNCS);

            Platform::EnsureDirectoryExists(outputPath.c_str());

            char uniqueFileName[128] = {};
            snprintf(
                uniqueFileName, sizeof(uniqueFileName), "desync_%llu_%u.txt",
                static_cast<long long unsigned>(Platform::GetDatetimeNowUTC()), tick);

            std::string outputFile = Path::Combine(outputPath, uniqueFileName);

            if (snapshots->LogCompareDataToFile(outputFile, cmpData))
            {
                log_info("Wrote desync report to '%s'", outputFile.c_str());

                auto ft = Formatter();
                ft.Add<char*>(uniqueFileName);

                char str_desync[1024];
                format_string(str_desync, sizeof(str_desync), STR_DESYNC_REPORT, ft.Data());

                auto intent = Intent(WC_NETWORK_STATUS);
                intent.putExtra(INTENT_EXTRA_MESSAGE, std::string{ str_desync });
                context_open_intent(&intent);
            }
        }
    }
}

void NetworkBase::Server_Handle_MAPREQUEST(NetworkConnection& connection, NetworkPacket& packet)
{
    uint32_t size;
    packet >> size;
    log_verbose("Client requested %u objects", size);
    auto& repo = GetContext().GetObjectRepository();
    for (uint32_t i = 0; i < size; i++)
    {
        uint8_t generation{};
        packet >> generation;

        std::string objectName;
        const ObjectRepositoryItem* item{};
        if (generation == static_cast<uint8_t>(ObjectGeneration::DAT))
        {
            const auto* entry = reinterpret_cast<const rct_object_entry*>(packet.Read(sizeof(rct_object_entry)));
            objectName = std::string(entry->GetName());
            log_verbose("Client requested object %s", objectName.c_str());
            item = repo.FindObject(entry);
        }
        else
        {
            objectName = std::string(packet.ReadString());
            log_verbose("Client requested object %s", objectName.c_str());
            item = repo.FindObject(objectName);
        }

        if (item == nullptr)
        {
            log_warning("Client tried getting non-existent object %s from us.", objectName.c_str());
        }
        else
        {
            connection.RequestedObjects.push_back(item);
        }
    }

    auto player_name = connection.Player->Name.c_str();
    Server_Send_MAP(&connection);
    Server_Send_EVENT_PLAYER_JOINED(player_name);
    Server_Send_GROUPLIST(connection);
}

void NetworkBase::Server_Handle_AUTH(NetworkConnection& connection, NetworkPacket& packet)
{
    if (connection.AuthStatus != NetworkAuth::Ok)
    {
        auto* hostName = connection.Socket->GetHostName();
        auto gameversion = packet.ReadString();
        auto name = packet.ReadString();
        auto password = packet.ReadString();
        auto pubkey = packet.ReadString();
        uint32_t sigsize;
        packet >> sigsize;
        if (pubkey.empty())
        {
            connection.AuthStatus = NetworkAuth::VerificationFailure;
        }
        else
        {
            try
            {
                // RSA technically supports keys up to 65536 bits, so this is the
                // maximum signature size for now.
                constexpr auto MaxRSASignatureSizeInBytes = 8192;

                if (sigsize == 0 || sigsize > MaxRSASignatureSizeInBytes)
                {
                    throw std::runtime_error("Invalid signature size");
                }

                std::vector<uint8_t> signature;
                signature.resize(sigsize);

                const uint8_t* signatureData = packet.Read(sigsize);
                if (signatureData == nullptr)
                {
                    throw std::runtime_error("Failed to read packet.");
                }

                std::memcpy(signature.data(), signatureData, sigsize);

                auto ms = MemoryStream(pubkey.data(), pubkey.size());
                if (!connection.Key.LoadPublic(&ms))
                {
                    throw std::runtime_error("Failed to load public key.");
                }

                bool verified = connection.Key.Verify(connection.Challenge.data(), connection.Challenge.size(), signature);
                const std::string hash = connection.Key.PublicKeyHash();
                if (verified)
                {
                    log_verbose("Connection %s: Signature verification ok. Hash %s", hostName, hash.c_str());
                    if (gConfigNetwork.known_keys_only && _userManager.GetUserByHash(hash) == nullptr)
                    {
                        log_verbose("Connection %s: Hash %s, not known", hostName, hash.c_str());
                        connection.AuthStatus = NetworkAuth::UnknownKeyDisallowed;
                    }
                    else
                    {
                        connection.AuthStatus = NetworkAuth::Verified;
                    }
                }
                else
                {
                    connection.AuthStatus = NetworkAuth::VerificationFailure;
                    log_verbose("Connection %s: Signature verification failed!", hostName);
                }
            }
            catch (const std::exception&)
            {
                connection.AuthStatus = NetworkAuth::VerificationFailure;
                log_verbose("Connection %s: Signature verification failed, invalid data!", hostName);
            }
        }

        bool passwordless = false;
        if (connection.AuthStatus == NetworkAuth::Verified)
        {
            const NetworkGroup* group = GetGroupByID(GetGroupIDByHash(connection.Key.PublicKeyHash()));
            passwordless = group->CanPerformAction(NetworkPermission::PasswordlessLogin);
        }
        if (gameversion != network_get_version())
        {
            connection.AuthStatus = NetworkAuth::BadVersion;
            log_info("Connection %s: Bad version.", hostName);
        }
        else if (name.empty())
        {
            connection.AuthStatus = NetworkAuth::BadName;
            log_info("Connection %s: Bad name.", connection.Socket->GetHostName());
        }
        else if (!passwordless)
        {
            if (password.empty() && !_password.empty())
            {
                connection.AuthStatus = NetworkAuth::RequirePassword;
                log_info("Connection %s: Requires password.", hostName);
            }
            else if (!password.empty() && _password != password)
            {
                connection.AuthStatus = NetworkAuth::BadPassword;
                log_info("Connection %s: Bad password.", hostName);
            }
        }

        if (static_cast<size_t>(gConfigNetwork.maxplayers) <= player_list.size())
        {
            connection.AuthStatus = NetworkAuth::Full;
            log_info("Connection %s: Server is full.", hostName);
        }
        else if (connection.AuthStatus == NetworkAuth::Verified)
        {
            const std::string hash = connection.Key.PublicKeyHash();
            if (ProcessPlayerAuthenticatePluginHooks(connection, name, hash))
            {
                connection.AuthStatus = NetworkAuth::Ok;
                Server_Client_Joined(name, hash, connection);
            }
            else
            {
                connection.AuthStatus = NetworkAuth::VerificationFailure;
                log_info("Connection %s: Denied by plugin.", hostName);
            }
        }

        Server_Send_AUTH(connection);
    }
}

void NetworkBase::Client_Handle_MAP([[maybe_unused]] NetworkConnection& connection, NetworkPacket& packet)
{
    uint32_t size, offset;
    packet >> size >> offset;
    int32_t chunksize = static_cast<int32_t>(packet.Header.Size - packet.BytesRead);
    if (chunksize <= 0)
    {
        return;
    }
    if (offset == 0)
    {
        // Start of a new map load, clear the queue now as we have to buffer them
        // until the map is fully loaded.
        GameActions::ClearQueue();
        GameActions::SuspendQueue();

        _serverTickData.clear();
        _clientMapLoaded = false;
    }
    if (size > chunk_buffer.size())
    {
        chunk_buffer.resize(size);
    }
    char str_downloading_map[256];
    uint32_t downloading_map_args[2] = {
        (offset + chunksize) / 1024,
        size / 1024,
    };
    format_string(str_downloading_map, 256, STR_MULTIPLAYER_DOWNLOADING_MAP, downloading_map_args);

    auto intent = Intent(WC_NETWORK_STATUS);
    intent.putExtra(INTENT_EXTRA_MESSAGE, std::string{ str_downloading_map });
    intent.putExtra(INTENT_EXTRA_CALLBACK, []() -> void { ::GetContext()->GetNetwork().Close(); });
    context_open_intent(&intent);

    std::memcpy(&chunk_buffer[offset], const_cast<void*>(static_cast<const void*>(packet.Read(chunksize))), chunksize);
    if (offset + chunksize == size)
    {
        // Allow queue processing of game actions again.
        GameActions::ResumeQueue();

        context_force_close_window_by_class(WC_NETWORK_STATUS);
        game_unload_scripts();
        game_notify_map_change();

        bool has_to_free = false;
        uint8_t* data = &chunk_buffer[0];
        size_t data_size = size;
        auto ms = MemoryStream(data, data_size);
        if (LoadMap(&ms))
        {
            game_load_init();
            game_load_scripts();
            game_notify_map_changed();
            _serverState.tick = gCurrentTicks;
            // window_network_status_open("Loaded new map from network");
            _serverState.state = NetworkServerState::Ok;
            _clientMapLoaded = true;
            gFirstTimeSaving = true;

            // Notify user he is now online and which shortcut key enables chat
            network_chat_show_connected_message();

            // Fix invalid vehicle sprite sizes, thus preventing visual corruption of sprites
            fix_invalid_vehicle_sprite_sizes();

            // NOTE: Game actions are normally processed before processing the player list.
            // Given that during map load game actions are buffered we have to process the
            // player list first to have valid players for the queued game actions.
            ProcessPlayerList();
        }
        else
        {
            // Something went wrong, game is not loaded. Return to main screen.
            auto loadOrQuitAction = LoadOrQuitAction(LoadOrQuitModes::OpenSavePrompt, PromptMode::SaveBeforeQuit);
            GameActions::Execute(&loadOrQuitAction);
        }
        if (has_to_free)
        {
            free(data);
        }
    }
}

bool NetworkBase::LoadMap(IStream* stream)
{
    bool result = false;
    try
    {
        auto& context = GetContext();
        auto& objManager = context.GetObjectManager();
        auto importer = ParkImporter::CreateParkFile(context.GetObjectRepository());
        auto loadResult = importer->LoadFromStream(stream, false);
        objManager.LoadObjects(loadResult.RequiredObjects);
        importer->Import();

        EntityTweener::Get().Reset();
        AutoCreateMapAnimations();

        gLastAutoSaveUpdate = AUTOSAVE_PAUSE;
        result = true;
    }
    catch (const std::exception& e)
    {
        Console::Error::WriteLine("Unable to read map from server: %s", e.what());
    }
    return result;
}

bool NetworkBase::SaveMap(IStream* stream, const std::vector<const ObjectRepositoryItem*>& objects) const
{
    bool result = false;
    PrepareMapForSave();
    try
    {
        auto exporter = std::make_unique<ParkFileExporter>();
        exporter->ExportObjectsList = objects;
        exporter->Export(*stream);
        result = true;
    }
    catch (const std::exception& e)
    {
        Console::Error::WriteLine("Unable to serialise map: %s", e.what());
    }
    return result;
}

void NetworkBase::Client_Handle_CHAT([[maybe_unused]] NetworkConnection& connection, NetworkPacket& packet)
{
    auto text = packet.ReadString();
    if (!text.empty())
    {
        chat_history_add(std::string(text));
    }
}

static bool ProcessChatMessagePluginHooks(uint8_t playerId, std::string& text)
{
#    ifdef ENABLE_SCRIPTING
    auto& hookEngine = GetContext()->GetScriptEngine().GetHookEngine();
    if (hookEngine.HasSubscriptions(OpenRCT2::Scripting::HOOK_TYPE::NETWORK_CHAT))
    {
        auto ctx = GetContext()->GetScriptEngine().GetContext();

        // Create event args object
        auto objIdx = duk_push_object(ctx);
        duk_push_number(ctx, playerId);
        duk_put_prop_string(ctx, objIdx, "player");
        duk_push_string(ctx, text.c_str());
        duk_put_prop_string(ctx, objIdx, "message");
        auto e = DukValue::take_from_stack(ctx);

        // Call the subscriptions
        hookEngine.Call(OpenRCT2::Scripting::HOOK_TYPE::NETWORK_CHAT, e, false);

        // Update text from object if subscriptions changed it
        if (e["message"].type() != DukValue::Type::STRING)
        {
            // Subscription set text to non-string, do not relay message
            return false;
        }
        text = e["message"].as_string();
        if (text.empty())
        {
            // Subscription set text to empty string, do not relay message
            return false;
        }
    }
#    endif
    return true;
}

void NetworkBase::Server_Handle_CHAT(NetworkConnection& connection, NetworkPacket& packet)
{
    auto szText = packet.ReadString();
    if (szText.empty())
        return;

    if (connection.Player != nullptr)
    {
        NetworkGroup* group = GetGroupByID(connection.Player->Group);
        if (group == nullptr || !group->CanPerformAction(NetworkPermission::Chat))
        {
            return;
        }
    }

    std::string text(szText);
    if (connection.Player != nullptr)
    {
        if (!ProcessChatMessagePluginHooks(connection.Player->Id, text))
        {
            // Message not to be relayed
            return;
        }
    }

    const char* formatted = FormatChat(connection.Player, text.c_str());
    chat_history_add(formatted);
    Server_Send_CHAT(formatted);
}

void NetworkBase::Client_Handle_GAME_ACTION([[maybe_unused]] NetworkConnection& connection, NetworkPacket& packet)
{
    uint32_t tick;
    GameCommand actionType;
    packet >> tick >> actionType;

    MemoryStream stream;
    const size_t size = packet.Header.Size - packet.BytesRead;
    stream.WriteArray(packet.Read(size), size);
    stream.SetPosition(0);

    DataSerialiser ds(false, stream);

    GameAction::Ptr action = GameActions::Create(actionType);
    if (action == nullptr)
    {
        log_error("Received unregistered game action type: 0x%08X", actionType);
        return;
    }
    action->Serialise(ds);

    if (player_id == action->GetPlayer().id)
    {
        // Only execute callbacks that belong to us,
        // clients can have identical network ids assigned.
        auto itr = _gameActionCallbacks.find(action->GetNetworkId());
        if (itr != _gameActionCallbacks.end())
        {
            action->SetCallback(itr->second);
            _gameActionCallbacks.erase(itr);
        }
    }

    GameActions::Enqueue(std::move(action), tick);
}

void NetworkBase::Server_Handle_GAME_ACTION(NetworkConnection& connection, NetworkPacket& packet)
{
    uint32_t tick;
    GameCommand actionType;

    NetworkPlayer* player = connection.Player;
    if (player == nullptr)
    {
        return;
    }

    packet >> tick >> actionType;

    // Don't let clients send pause or quit
    if (actionType == GameCommand::TogglePause || actionType == GameCommand::LoadOrQuit)
    {
        return;
    }

    if (actionType != GameCommand::Custom)
    {
        // Check if player's group permission allows command to run
        NetworkGroup* group = GetGroupByID(connection.Player->Group);
        if (group == nullptr || group->CanPerformCommand(actionType) == false)
        {
            Server_Send_SHOWERROR(connection, STR_CANT_DO_THIS, STR_PERMISSION_DENIED);
            return;
        }
    }

    // Create and enqueue the action.
    GameAction::Ptr ga = GameActions::Create(actionType);
    if (ga == nullptr)
    {
        log_error(
            "Received unregistered game action type: 0x%08X from player: (%d) %s", actionType, connection.Player->Id,
            connection.Player->Name.c_str());
        return;
    }

    // Player who is hosting is not affected by cooldowns.
    if ((player->Flags & NETWORK_PLAYER_FLAG_ISSERVER) == 0)
    {
        auto cooldownIt = player->CooldownTime.find(actionType);
        if (cooldownIt != std::end(player->CooldownTime))
        {
            if (cooldownIt->second > 0)
            {
                Server_Send_SHOWERROR(connection, STR_CANT_DO_THIS, STR_NETWORK_ACTION_RATE_LIMIT_MESSAGE);
                return;
            }
        }

        uint32_t cooldownTime = ga->GetCooldownTime();
        if (cooldownTime > 0)
        {
            player->CooldownTime[actionType] = cooldownTime;
        }
    }

    DataSerialiser stream(false);
    const size_t size = packet.Header.Size - packet.BytesRead;
    stream.GetStream().WriteArray(packet.Read(size), size);
    stream.GetStream().SetPosition(0);

    ga->Serialise(stream);
    // Set player to sender, should be 0 if sent from client.
    ga->SetPlayer(NetworkPlayerId_t{ connection.Player->Id });

    GameActions::Enqueue(std::move(ga), tick);
}

void NetworkBase::Client_Handle_TICK([[maybe_unused]] NetworkConnection& connection, NetworkPacket& packet)
{
    uint32_t srand0;
    uint32_t flags;
    uint32_t serverTick;

    packet >> serverTick >> srand0 >> flags;

    ServerTickData_t tickData;
    tickData.srand0 = srand0;
    tickData.tick = serverTick;

    if (flags & NETWORK_TICK_FLAG_CHECKSUMS)
    {
        auto text = packet.ReadString();
        if (!text.empty())
        {
            tickData.spriteHash = text;
        }
    }

    // Don't let the history grow too much.
    while (_serverTickData.size() >= 100)
    {
        _serverTickData.erase(_serverTickData.begin());
    }

    _serverState.tick = serverTick;
    _serverTickData.emplace(serverTick, tickData);
}

void NetworkBase::Client_Handle_PLAYERINFO([[maybe_unused]] NetworkConnection& connection, NetworkPacket& packet)
{
    uint32_t tick;
    packet >> tick;

    NetworkPlayer playerInfo;
    playerInfo.Read(packet);

    _pendingPlayerInfo.emplace(tick, playerInfo);
}

void NetworkBase::Client_Handle_PLAYERLIST([[maybe_unused]] NetworkConnection& connection, NetworkPacket& packet)
{
    uint32_t tick;
    uint8_t size;
    packet >> tick >> size;

    auto& pending = _pendingPlayerLists[tick];
    pending.players.clear();

    for (uint32_t i = 0; i < size; i++)
    {
        NetworkPlayer tempplayer;
        tempplayer.Read(packet);

        pending.players.push_back(std::move(tempplayer));
    }
}

void NetworkBase::Client_Handle_PING([[maybe_unused]] NetworkConnection& connection, [[maybe_unused]] NetworkPacket& packet)
{
    Client_Send_PING();
}

void NetworkBase::Server_Handle_PING(NetworkConnection& connection, [[maybe_unused]] NetworkPacket& packet)
{
    int32_t ping = Platform::GetTicks() - connection.PingTime;
    if (ping < 0)
    {
        ping = 0;
    }
    if (connection.Player != nullptr)
    {
        connection.Player->Ping = ping;
        window_invalidate_by_number(WC_PLAYER, connection.Player->Id);
    }
}

void NetworkBase::Client_Handle_PINGLIST([[maybe_unused]] NetworkConnection& connection, NetworkPacket& packet)
{
    uint8_t size;
    packet >> size;
    for (uint32_t i = 0; i < size; i++)
    {
        uint8_t id;
        uint16_t ping;
        packet >> id >> ping;
        NetworkPlayer* player = GetPlayerByID(id);
        if (player != nullptr)
        {
            player->Ping = ping;
        }
    }
    window_invalidate_by_class(WC_PLAYER);
}

void NetworkBase::Client_Handle_SETDISCONNECTMSG(NetworkConnection& connection, NetworkPacket& packet)
{
    auto disconnectmsg = packet.ReadString();
    if (!disconnectmsg.empty())
    {
        connection.SetLastDisconnectReason(disconnectmsg);
    }
}

void NetworkBase::Server_Handle_GAMEINFO(NetworkConnection& connection, [[maybe_unused]] NetworkPacket& packet)
{
    Server_Send_GAMEINFO(connection);
}

void NetworkBase::Client_Handle_SHOWERROR([[maybe_unused]] NetworkConnection& connection, NetworkPacket& packet)
{
    StringId title, message;
    packet >> title >> message;
    context_show_error(title, message, {});
}

void NetworkBase::Client_Handle_GROUPLIST([[maybe_unused]] NetworkConnection& connection, NetworkPacket& packet)
{
    group_list.clear();
    uint8_t size;
    packet >> size >> default_group;
    for (uint32_t i = 0; i < size; i++)
    {
        NetworkGroup group;
        group.Read(packet);
        auto newgroup = std::make_unique<NetworkGroup>(group);
        group_list.push_back(std::move(newgroup));
    }
}

void NetworkBase::Client_Handle_EVENT([[maybe_unused]] NetworkConnection& connection, NetworkPacket& packet)
{
    uint16_t eventType;
    packet >> eventType;
    switch (eventType)
    {
        case SERVER_EVENT_PLAYER_JOINED:
        {
            auto playerName = packet.ReadString();
            auto message = FormatStringId(STR_MULTIPLAYER_PLAYER_HAS_JOINED_THE_GAME, playerName);
            chat_history_add(message);
            break;
        }
        case SERVER_EVENT_PLAYER_DISCONNECTED:
        {
            auto playerName = packet.ReadString();
            auto reason = packet.ReadString();
            std::string message;
            if (reason.empty())
            {
                message = FormatStringId(STR_MULTIPLAYER_PLAYER_HAS_DISCONNECTED_NO_REASON, playerName);
            }
            else
            {
                message = FormatStringId(STR_MULTIPLAYER_PLAYER_HAS_DISCONNECTED_WITH_REASON, playerName, reason);
            }
            chat_history_add(message);
            break;
        }
    }
}

void NetworkBase::Client_Send_GAMEINFO()
{
    log_verbose("requesting gameinfo");
    NetworkPacket packet(NetworkCommand::GameInfo);
    _serverConnection->QueuePacket(std::move(packet));
}

void NetworkBase::Client_Handle_GAMEINFO([[maybe_unused]] NetworkConnection& connection, NetworkPacket& packet)
{
    auto jsonString = packet.ReadString();
    packet >> _serverState.gamestateSnapshotsEnabled;

    json_t jsonData = Json::FromString(jsonString);

    if (jsonData.is_object())
    {
        ServerName = Json::GetString(jsonData["name"]);
        ServerDescription = Json::GetString(jsonData["description"]);
        ServerGreeting = Json::GetString(jsonData["greeting"]);

        json_t jsonProvider = jsonData["provider"];
        if (jsonProvider.is_object())
        {
            ServerProviderName = Json::GetString(jsonProvider["name"]);
            ServerProviderEmail = Json::GetString(jsonProvider["email"]);
            ServerProviderWebsite = Json::GetString(jsonProvider["website"]);
        }
    }

    network_chat_show_server_greeting();
}

void network_reconnect()
{
    OpenRCT2::GetContext()->GetNetwork().Reconnect();
}

void network_shutdown_client()
{
    OpenRCT2::GetContext()->GetNetwork().ServerClientDisconnected();
}

int32_t network_begin_client(const std::string& host, int32_t port)
{
    return OpenRCT2::GetContext()->GetNetwork().BeginClient(host, port);
}

int32_t network_begin_server(int32_t port, const std::string& address)
{
    return OpenRCT2::GetContext()->GetNetwork().BeginServer(port, address);
}

void network_update()
{
    OpenRCT2::GetContext()->GetNetwork().Update();
}

void network_process_pending()
{
    OpenRCT2::GetContext()->GetNetwork().ProcessPending();
}

void network_flush()
{
    OpenRCT2::GetContext()->GetNetwork().Flush();
}

int32_t network_get_mode()
{
    return OpenRCT2::GetContext()->GetNetwork().GetMode();
}

int32_t network_get_status()
{
    return OpenRCT2::GetContext()->GetNetwork().GetStatus();
}

bool network_is_desynchronised()
{
    return OpenRCT2::GetContext()->GetNetwork().IsDesynchronised();
}

bool network_check_desynchronisation()
{
    return OpenRCT2::GetContext()->GetNetwork().CheckDesynchronizaton();
}

void network_request_gamestate_snapshot()
{
    return OpenRCT2::GetContext()->GetNetwork().RequestStateSnapshot();
}

void network_send_tick()
{
    OpenRCT2::GetContext()->GetNetwork().Server_Send_TICK();
}

NetworkAuth network_get_authstatus()
{
    return OpenRCT2::GetContext()->GetNetwork().GetAuthStatus();
}

uint32_t network_get_server_tick()
{
    return OpenRCT2::GetContext()->GetNetwork().GetServerTick();
}

uint8_t network_get_current_player_id()
{
    return OpenRCT2::GetContext()->GetNetwork().GetPlayerID();
}

int32_t network_get_num_players()
{
    return static_cast<int32_t>(OpenRCT2::GetContext()->GetNetwork().player_list.size());
}

const char* network_get_player_name(uint32_t index)
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    Guard::IndexInRange(index, network.player_list);

    return static_cast<const char*>(network.player_list[index]->Name.c_str());
}

uint32_t network_get_player_flags(uint32_t index)
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    Guard::IndexInRange(index, network.player_list);

    return network.player_list[index]->Flags;
}

int32_t network_get_player_ping(uint32_t index)
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    Guard::IndexInRange(index, network.player_list);

    return network.player_list[index]->Ping;
}

int32_t network_get_player_id(uint32_t index)
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    Guard::IndexInRange(index, network.player_list);

    return network.player_list[index]->Id;
}

money32 network_get_player_money_spent(uint32_t index)
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    Guard::IndexInRange(index, network.player_list);

    return network.player_list[index]->MoneySpent;
}

std::string network_get_player_ip_address(uint32_t id)
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    auto conn = network.GetPlayerConnection(id);
    if (conn != nullptr && conn->Socket != nullptr)
    {
        return conn->Socket->GetIpAddress();
    }
    return {};
}

std::string network_get_player_public_key_hash(uint32_t id)
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    auto player = network.GetPlayerByID(id);
    if (player != nullptr)
    {
        return player->KeyHash;
    }
    return {};
}

void network_add_player_money_spent(uint32_t index, money32 cost)
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    Guard::IndexInRange(index, network.player_list);

    network.player_list[index]->AddMoneySpent(cost);
}

int32_t network_get_player_last_action(uint32_t index, int32_t time)
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    Guard::IndexInRange(index, network.player_list);

    if (time && Platform::GetTicks() > network.player_list[index]->LastActionTime + time)
    {
        return -999;
    }
    return network.player_list[index]->LastAction;
}

void network_set_player_last_action(uint32_t index, GameCommand command)
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    Guard::IndexInRange(index, network.player_list);

    network.player_list[index]->LastAction = static_cast<int32_t>(NetworkActions::FindCommand(command));
    network.player_list[index]->LastActionTime = Platform::GetTicks();
}

CoordsXYZ network_get_player_last_action_coord(uint32_t index)
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    Guard::IndexInRange(index, OpenRCT2::GetContext()->GetNetwork().player_list);

    return network.player_list[index]->LastActionCoord;
}

void network_set_player_last_action_coord(uint32_t index, const CoordsXYZ& coord)
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    Guard::IndexInRange(index, network.player_list);

    if (index < network.player_list.size())
    {
        network.player_list[index]->LastActionCoord = coord;
    }
}

uint32_t network_get_player_commands_ran(uint32_t index)
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    Guard::IndexInRange(index, OpenRCT2::GetContext()->GetNetwork().player_list);

    return network.player_list[index]->CommandsRan;
}

int32_t network_get_player_index(uint32_t id)
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    auto it = network.GetPlayerIteratorByID(id);
    if (it == network.player_list.end())
    {
        return -1;
    }
    return static_cast<int32_t>(network.GetPlayerIteratorByID(id) - network.player_list.begin());
}

uint8_t network_get_player_group(uint32_t index)
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    Guard::IndexInRange(index, network.player_list);

    return network.player_list[index]->Group;
}

void network_set_player_group(uint32_t index, uint32_t groupindex)
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    Guard::IndexInRange(index, network.player_list);
    Guard::IndexInRange(groupindex, network.group_list);

    network.player_list[index]->Group = network.group_list[groupindex]->Id;
}

int32_t network_get_group_index(uint8_t id)
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    auto it = network.GetGroupIteratorByID(id);
    if (it == network.group_list.end())
    {
        return -1;
    }
    return static_cast<int32_t>(network.GetGroupIteratorByID(id) - network.group_list.begin());
}

uint8_t network_get_group_id(uint32_t index)
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    Guard::IndexInRange(index, network.group_list);

    return network.group_list[index]->Id;
}

int32_t network_get_num_groups()
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    return static_cast<int32_t>(network.group_list.size());
}

const char* network_get_group_name(uint32_t index)
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    return network.group_list[index]->GetName().c_str();
}

void network_chat_show_connected_message()
{
    auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
    std::string s = windowManager->GetKeyboardShortcutString("interface.misc.multiplayer_chat");
    const char* sptr = s.c_str();

    utf8 buffer[256];
    format_string(buffer, sizeof(buffer), STR_MULTIPLAYER_CONNECTED_CHAT_HINT, &sptr);

    NetworkPlayer server;
    server.Name = "Server";
    const char* formatted = NetworkBase::FormatChat(&server, buffer);
    chat_history_add(formatted);
}

// Display server greeting if one exists
void network_chat_show_server_greeting()
{
    auto greeting = network_get_server_greeting();
    if (!str_is_null_or_empty(greeting))
    {
        thread_local std::string greeting_formatted;
        greeting_formatted.assign("{OUTLINE}{GREEN}");
        greeting_formatted += greeting;
        chat_history_add(greeting_formatted);
    }
}

GameActions::Result network_set_player_group(
    NetworkPlayerId_t actionPlayerId, NetworkPlayerId_t playerId, uint8_t groupId, bool isExecuting)
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    NetworkPlayer* player = network.GetPlayerByID(playerId);

    NetworkGroup* fromgroup = network.GetGroupByID(actionPlayerId);
    if (player == nullptr)
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_DO_THIS, STR_NONE);
    }

    if (network.GetGroupByID(groupId) == nullptr)
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_DO_THIS, STR_NONE);
    }

    if (player->Flags & NETWORK_PLAYER_FLAG_ISSERVER)
    {
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_CHANGE_GROUP_THAT_THE_HOST_BELONGS_TO, STR_NONE);
    }

    if (groupId == 0 && fromgroup != nullptr && fromgroup->Id != 0)
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_SET_TO_THIS_GROUP, STR_NONE);
    }

    if (isExecuting)
    {
        player->Group = groupId;

        if (network_get_mode() == NETWORK_MODE_SERVER)
        {
            // Add or update saved user
            NetworkUserManager& userManager = network._userManager;
            NetworkUser* networkUser = userManager.GetOrAddUser(player->KeyHash);
            networkUser->GroupId = groupId;
            networkUser->Name = player->Name;
            userManager.Save();
        }

        window_invalidate_by_number(WC_PLAYER, playerId);

        // Log set player group event
        NetworkPlayer* game_command_player = network.GetPlayerByID(actionPlayerId);
        NetworkGroup* new_player_group = network.GetGroupByID(groupId);
        char log_msg[256];
        const char* args[3] = {
            player->Name.c_str(),
            new_player_group->GetName().c_str(),
            game_command_player->Name.c_str(),
        };
        format_string(log_msg, 256, STR_LOG_SET_PLAYER_GROUP, args);
        network_append_server_log(log_msg);
    }
    return GameActions::Result();
}

GameActions::Result network_modify_groups(
    NetworkPlayerId_t actionPlayerId, ModifyGroupType type, uint8_t groupId, const std::string& name, uint32_t permissionIndex,
    PermissionState permissionState, bool isExecuting)
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    switch (type)
    {
        case ModifyGroupType::AddGroup:
        {
            if (isExecuting)
            {
                NetworkGroup* newgroup = network.AddGroup();
                if (newgroup == nullptr)
                {
                    return GameActions::Result(GameActions::Status::Unknown, STR_CANT_DO_THIS, STR_NONE);
                }
            }
        }
        break;
        case ModifyGroupType::RemoveGroup:
        {
            if (groupId == 0)
            {
                return GameActions::Result(GameActions::Status::Disallowed, STR_THIS_GROUP_CANNOT_BE_MODIFIED, STR_NONE);
            }
            for (const auto& it : network.player_list)
            {
                if ((it.get())->Group == groupId)
                {
                    return GameActions::Result(
                        GameActions::Status::Disallowed, STR_CANT_REMOVE_GROUP_THAT_PLAYERS_BELONG_TO, STR_NONE);
                }
            }
            if (isExecuting)
            {
                network.RemoveGroup(groupId);
            }
        }
        break;
        case ModifyGroupType::SetPermissions:
        {
            if (groupId == 0)
            { // can't change admin group permissions
                return GameActions::Result(GameActions::Status::Disallowed, STR_THIS_GROUP_CANNOT_BE_MODIFIED, STR_NONE);
            }
            NetworkGroup* mygroup = nullptr;
            NetworkPlayer* player = network.GetPlayerByID(actionPlayerId);
            auto networkPermission = static_cast<NetworkPermission>(permissionIndex);
            if (player != nullptr && permissionState == PermissionState::Toggle)
            {
                mygroup = network.GetGroupByID(player->Group);
                if (mygroup == nullptr || !mygroup->CanPerformAction(networkPermission))
                {
                    return GameActions::Result(
                        GameActions::Status::Disallowed, STR_CANT_MODIFY_PERMISSION_THAT_YOU_DO_NOT_HAVE_YOURSELF, STR_NONE);
                }
            }
            if (isExecuting)
            {
                NetworkGroup* group = network.GetGroupByID(groupId);
                if (group != nullptr)
                {
                    if (permissionState != PermissionState::Toggle)
                    {
                        if (mygroup != nullptr)
                        {
                            if (permissionState == PermissionState::SetAll)
                            {
                                group->ActionsAllowed = mygroup->ActionsAllowed;
                            }
                            else
                            {
                                group->ActionsAllowed.fill(0x00);
                            }
                        }
                    }
                    else
                    {
                        group->ToggleActionPermission(networkPermission);
                    }
                }
            }
        }
        break;
        case ModifyGroupType::SetName:
        {
            NetworkGroup* group = network.GetGroupByID(groupId);
            const char* oldName = group->GetName().c_str();

            if (strcmp(oldName, name.c_str()) == 0)
            {
                return GameActions::Result();
            }

            if (name.empty())
            {
                return GameActions::Result(
                    GameActions::Status::InvalidParameters, STR_CANT_RENAME_GROUP, STR_INVALID_GROUP_NAME);
            }

            if (isExecuting)
            {
                if (group != nullptr)
                {
                    group->SetName(name);
                }
            }
        }
        break;
        case ModifyGroupType::SetDefault:
        {
            if (groupId == 0)
            {
                return GameActions::Result(GameActions::Status::Disallowed, STR_CANT_SET_TO_THIS_GROUP, STR_NONE);
            }
            if (isExecuting)
            {
                network.SetDefaultGroup(groupId);
            }
        }
        break;
        default:
            log_error("Invalid Modify Group Type: %u", static_cast<uint8_t>(type));
            return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }

    network.SaveGroups();

    return GameActions::Result();
}

GameActions::Result network_kick_player(NetworkPlayerId_t playerId, bool isExecuting)
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    NetworkPlayer* player = network.GetPlayerByID(playerId);
    if (player == nullptr)
    {
        // Player might be already removed by the PLAYERLIST command, need to refactor non-game commands executing too
        // early.
        return GameActions::Result(GameActions::Status::Unknown, STR_NONE, STR_NONE);
    }

    if (player->Flags & NETWORK_PLAYER_FLAG_ISSERVER)
    {
        return GameActions::Result(GameActions::Status::Disallowed, STR_CANT_KICK_THE_HOST, STR_NONE);
    }

    if (isExecuting)
    {
        if (network.GetMode() == NETWORK_MODE_SERVER)
        {
            network.KickPlayer(playerId);

            NetworkUserManager& networkUserManager = network._userManager;
            networkUserManager.Load();
            networkUserManager.RemoveUser(player->KeyHash);
            networkUserManager.Save();
        }
    }
    return GameActions::Result();
}

uint8_t network_get_default_group()
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    return network.GetDefaultGroup();
}

int32_t network_get_num_actions()
{
    return static_cast<int32_t>(NetworkActions::Actions.size());
}

StringId network_get_action_name_string_id(uint32_t index)
{
    if (index < NetworkActions::Actions.size())
    {
        return NetworkActions::Actions[index].Name;
    }

    return STR_NONE;
}

int32_t network_can_perform_action(uint32_t groupindex, NetworkPermission index)
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    Guard::IndexInRange(groupindex, network.group_list);

    return network.group_list[groupindex]->CanPerformAction(index);
}

int32_t network_can_perform_command(uint32_t groupindex, int32_t index)
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    Guard::IndexInRange(groupindex, network.group_list);

    return network.group_list[groupindex]->CanPerformCommand(static_cast<GameCommand>(index)); // TODO
}

void network_set_pickup_peep(uint8_t playerid, Peep* peep)
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    if (network.GetMode() == NETWORK_MODE_NONE)
    {
        _pickup_peep = peep;
    }
    else
    {
        NetworkPlayer* player = network.GetPlayerByID(playerid);
        if (player != nullptr)
        {
            player->PickupPeep = peep;
        }
    }
}

Peep* network_get_pickup_peep(uint8_t playerid)
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    if (network.GetMode() == NETWORK_MODE_NONE)
    {
        return _pickup_peep;
    }

    NetworkPlayer* player = network.GetPlayerByID(playerid);
    if (player != nullptr)
    {
        return player->PickupPeep;
    }
    return nullptr;
}

void network_set_pickup_peep_old_x(uint8_t playerid, int32_t x)
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    if (network.GetMode() == NETWORK_MODE_NONE)
    {
        _pickup_peep_old_x = x;
    }
    else
    {
        NetworkPlayer* player = network.GetPlayerByID(playerid);
        if (player != nullptr)
        {
            player->PickupPeepOldX = x;
        }
    }
}

int32_t network_get_pickup_peep_old_x(uint8_t playerid)
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    if (network.GetMode() == NETWORK_MODE_NONE)
    {
        return _pickup_peep_old_x;
    }

    NetworkPlayer* player = network.GetPlayerByID(playerid);
    if (player != nullptr)
    {
        return player->PickupPeepOldX;
    }
    return -1;
}

int32_t network_get_current_player_group_index()
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    NetworkPlayer* player = network.GetPlayerByID(network.GetPlayerID());
    if (player != nullptr)
    {
        return network_get_group_index(player->Group);
    }
    return -1;
}

void network_send_chat(const char* text, const std::vector<uint8_t>& playerIds)
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    if (network.GetMode() == NETWORK_MODE_CLIENT)
    {
        network.Client_Send_CHAT(text);
    }
    else if (network.GetMode() == NETWORK_MODE_SERVER)
    {
        std::string message = text;
        if (ProcessChatMessagePluginHooks(network.GetPlayerID(), message))
        {
            auto player = network.GetPlayerByID(network.GetPlayerID());
            if (player != nullptr)
            {
                auto formatted = network.FormatChat(player, message.c_str());
                if (playerIds.empty()
                    || std::find(playerIds.begin(), playerIds.end(), network.GetPlayerID()) != playerIds.end())
                {
                    // Server is one of the recipients
                    chat_history_add(formatted);
                }
                network.Server_Send_CHAT(formatted, playerIds);
            }
        }
    }
}

void network_send_game_action(const GameAction* action)
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    switch (network.GetMode())
    {
        case NETWORK_MODE_SERVER:
            network.Server_Send_GAME_ACTION(action);
            break;
        case NETWORK_MODE_CLIENT:
            network.Client_Send_GAME_ACTION(action);
            break;
    }
}

void network_send_password(const std::string& password)
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    const auto keyPath = network_get_private_key_path(gConfigNetwork.player_name);
    if (!File::Exists(keyPath))
    {
        log_error("Private key %s missing! Restart the game to generate it.", keyPath.c_str());
        return;
    }
    try
    {
        auto fs = FileStream(keyPath, FILE_MODE_OPEN);
        network._key.LoadPrivate(&fs);
    }
    catch (const std::exception&)
    {
        log_error("Error reading private key from %s.", keyPath.c_str());
        return;
    }
    const std::string pubkey = network._key.PublicKeyString();

    std::vector<uint8_t> signature;
    network._key.Sign(network._challenge.data(), network._challenge.size(), signature);
    // Don't keep private key in memory. There's no need and it may get leaked
    // when process dump gets collected at some point in future.
    network._key.Unload();
    network.Client_Send_AUTH(gConfigNetwork.player_name, password, pubkey, signature);
}

void network_set_password(const char* password)
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    network.SetPassword(password);
}

void network_append_chat_log(std::string_view text)
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    network.AppendChatLog(text);
}

void network_append_server_log(const utf8* text)
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    network.AppendServerLog(text);
}

static u8string network_get_keys_directory()
{
    auto env = GetContext()->GetPlatformEnvironment();
    return Path::Combine(env->GetDirectoryPath(DIRBASE::USER), u8"keys");
}

static u8string network_get_private_key_path(u8string_view playerName)
{
    return Path::Combine(network_get_keys_directory(), u8string(playerName) + u8".privkey");
}

static u8string network_get_public_key_path(u8string_view playerName, u8string_view hash)
{
    const auto filename = u8string(playerName) + u8"-" + u8string(hash) + u8".pubkey";
    return Path::Combine(network_get_keys_directory(), filename);
}

const utf8* network_get_server_name()
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    return network.ServerName.c_str();
}
const utf8* network_get_server_description()
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    return network.ServerDescription.c_str();
}
const utf8* network_get_server_greeting()
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    return network.ServerGreeting.c_str();
}
const utf8* network_get_server_provider_name()
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    return network.ServerProviderName.c_str();
}
const utf8* network_get_server_provider_email()
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    return network.ServerProviderEmail.c_str();
}
const utf8* network_get_server_provider_website()
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    return network.ServerProviderWebsite.c_str();
}

std::string network_get_version()
{
    return NETWORK_STREAM_ID;
}

NetworkStats_t network_get_stats()
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    return network.GetStats();
}

NetworkServerState_t network_get_server_state()
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    return network.GetServerState();
}

bool network_gamestate_snapshots_enabled()
{
    return network_get_server_state().gamestateSnapshotsEnabled;
}

json_t network_get_server_info_as_json()
{
    auto& network = OpenRCT2::GetContext()->GetNetwork();
    return network.GetServerInfoAsJson();
}
#else
int32_t network_get_mode()
{
    return NETWORK_MODE_NONE;
}
int32_t network_get_status()
{
    return NETWORK_STATUS_NONE;
}
NetworkAuth network_get_authstatus()
{
    return NetworkAuth::None;
}
uint32_t network_get_server_tick()
{
    return gCurrentTicks;
}
void network_flush()
{
}
void network_send_tick()
{
}
bool network_is_desynchronised()
{
    return false;
}
bool network_gamestate_snapshots_enabled()
{
    return false;
}
bool network_check_desynchronisation()
{
    return false;
}
void network_request_gamestate_snapshot()
{
}
void network_send_game_action(const GameAction* action)
{
}
void network_update()
{
}
void network_process_pending()
{
}
int32_t network_begin_client(const std::string& host, int32_t port)
{
    return 1;
}
int32_t network_begin_server(int32_t port, const std::string& address)
{
    return 1;
}
int32_t network_get_num_players()
{
    return 1;
}
const char* network_get_player_name(uint32_t index)
{
    return "local (OpenRCT2 compiled without MP)";
}
uint32_t network_get_player_flags(uint32_t index)
{
    return 0;
}
int32_t network_get_player_ping(uint32_t index)
{
    return 0;
}
int32_t network_get_player_id(uint32_t index)
{
    return 0;
}
money32 network_get_player_money_spent(uint32_t index)
{
    return 0.00_GBP;
}
std::string network_get_player_ip_address(uint32_t id)
{
    return {};
}
std::string network_get_player_public_key_hash(uint32_t id)
{
    return {};
}
void network_add_player_money_spent(uint32_t index, money32 cost)
{
}
int32_t network_get_player_last_action(uint32_t index, int32_t time)
{
    return -999;
}
void network_set_player_last_action(uint32_t index, GameCommand command)
{
}
CoordsXYZ network_get_player_last_action_coord(uint32_t index)
{
    return { 0, 0, 0 };
}
void network_set_player_last_action_coord(uint32_t index, const CoordsXYZ& coord)
{
}
uint32_t network_get_player_commands_ran(uint32_t index)
{
    return 0;
}
int32_t network_get_player_index(uint32_t id)
{
    return -1;
}
uint8_t network_get_player_group(uint32_t index)
{
    return 0;
}
void network_set_player_group(uint32_t index, uint32_t groupindex)
{
}
int32_t network_get_group_index(uint8_t id)
{
    return -1;
}
uint8_t network_get_group_id(uint32_t index)
{
    return 0;
}
int32_t network_get_num_groups()
{
    return 0;
}
const char* network_get_group_name(uint32_t index)
{
    return "";
};

GameActions::Result network_set_player_group(
    NetworkPlayerId_t actionPlayerId, NetworkPlayerId_t playerId, uint8_t groupId, bool isExecuting)
{
    return GameActions::Result();
}
GameActions::Result network_modify_groups(
    NetworkPlayerId_t actionPlayerId, ModifyGroupType type, uint8_t groupId, const std::string& name, uint32_t permissionIndex,
    PermissionState permissionState, bool isExecuting)
{
    return GameActions::Result();
}
GameActions::Result network_kick_player(NetworkPlayerId_t playerId, bool isExecuting)
{
    return GameActions::Result();
}
uint8_t network_get_default_group()
{
    return 0;
}
int32_t network_get_num_actions()
{
    return 0;
}
StringId network_get_action_name_string_id(uint32_t index)
{
    return -1;
}
int32_t network_can_perform_action(uint32_t groupindex, NetworkPermission index)
{
    return 0;
}
int32_t network_can_perform_command(uint32_t groupindex, int32_t index)
{
    return 0;
}
void network_set_pickup_peep(uint8_t playerid, Peep* peep)
{
    _pickup_peep = peep;
}
Peep* network_get_pickup_peep(uint8_t playerid)
{
    return _pickup_peep;
}
void network_set_pickup_peep_old_x(uint8_t playerid, int32_t x)
{
    _pickup_peep_old_x = x;
}
int32_t network_get_pickup_peep_old_x(uint8_t playerid)
{
    return _pickup_peep_old_x;
}
void network_send_chat(const char* text, const std::vector<uint8_t>& playerIds)
{
}
void network_send_password(const std::string& password)
{
}
void network_reconnect()
{
}
void network_shutdown_client()
{
}
void network_set_password(const char* password)
{
}
uint8_t network_get_current_player_id()
{
    return 0;
}
int32_t network_get_current_player_group_index()
{
    return 0;
}
void network_append_chat_log(std::string_view)
{
}
void network_append_server_log(const utf8* text)
{
}
const utf8* network_get_server_name()
{
    return nullptr;
}
const utf8* network_get_server_description()
{
    return nullptr;
}
const utf8* network_get_server_greeting()
{
    return nullptr;
}
const utf8* network_get_server_provider_name()
{
    return nullptr;
}
const utf8* network_get_server_provider_email()
{
    return nullptr;
}
const utf8* network_get_server_provider_website()
{
    return nullptr;
}
std::string network_get_version()
{
    return "Multiplayer disabled";
}
NetworkStats_t network_get_stats()
{
    return NetworkStats_t{};
}
NetworkServerState_t network_get_server_state()
{
    return NetworkServerState_t{};
}
json_t network_get_server_info_as_json()
{
    return {};
}
#endif /* DISABLE_NETWORK */
