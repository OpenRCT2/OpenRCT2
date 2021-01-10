#include "NetworkClient.h"

#ifndef DISABLE_NETWORK

#    include "../Context.h"
#    include "../GameStateSnapshots.h"
#    include "../OpenRCT2.h"
#    include "../PlatformEnvironment.h"
#    include "../actions/GameAction.h"
#    include "../actions/LoadOrQuitAction.h"
#    include "../core/Console.hpp"
#    include "../core/FileStream.h"
#    include "../core/Json.hpp"
#    include "../core/Path.hpp"
#    include "../core/String.hpp"
#    include "../interface/Chat.h"
#    include "../object/ObjectManager.h"
#    include "../object/ObjectRepository.h"
#    include "../platform/Platform2.h"
#    include "../scenario/Scenario.h"
#    include "../scripting/ScriptEngine.h"
#    include "../windows/Intent.h"

using namespace OpenRCT2;

NetworkClient::NetworkClient(const std::shared_ptr<OpenRCT2::IPlatformEnvironment>& env)
    : NetworkBase(env)
{
    SetupCommandHandler(NetworkCommand::Auth, &NetworkClient::OnPacketAuth);
    SetupCommandHandler(NetworkCommand::Map, &NetworkClient::OnPacketMap);
    SetupCommandHandler(NetworkCommand::Chat, &NetworkClient::OnPacketChat);
    SetupCommandHandler(NetworkCommand::Tick, &NetworkClient::OnPacketTick);
    SetupCommandHandler(NetworkCommand::PlayerList, &NetworkClient::OnPacketPlayerList);
    SetupCommandHandler(NetworkCommand::Ping, &NetworkClient::OnPacketPing);
    SetupCommandHandler(NetworkCommand::PingList, &NetworkClient::OnPacketPingList);
    SetupCommandHandler(NetworkCommand::DisconnectMessage, &NetworkClient::OnPacketDisconnectMsg);
    SetupCommandHandler(NetworkCommand::GameInfo, &NetworkClient::OnPacketGameInfo);
    SetupCommandHandler(NetworkCommand::ShowError, &NetworkClient::OnPacketShowError);
    SetupCommandHandler(NetworkCommand::GroupList, &NetworkClient::OnPacketGroupList);
    SetupCommandHandler(NetworkCommand::Event, &NetworkClient::OnPacketEvent);
    SetupCommandHandler(NetworkCommand::Token, &NetworkClient::OnPacketToken);
    SetupCommandHandler(NetworkCommand::ObjectsList, &NetworkClient::OnPacketObjectsList);
    SetupCommandHandler(NetworkCommand::GameAction, &NetworkClient::OnPacketGameAction);
    SetupCommandHandler(NetworkCommand::PlayerInfo, &NetworkClient::OnPacketPlayerInfo);
    SetupCommandHandler(NetworkCommand::GameState, &NetworkClient::OnPacketGameState);
    SetupCommandHandler(NetworkCommand::Scripts, &NetworkClient::OnPacketScripts);
}

bool NetworkClient::BeginClient(const std::string& host, uint16_t port)
{
    Close();

    if (!Init())
        return false;

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
    // If the client has the title screen running then theres a potential
    // risk of tick collision with the server map and title screen map.
    GameActions::SuspendQueue();

    utf8 keyPath[MAX_PATH];
    GetPrivateKeyPath(keyPath, sizeof(keyPath), gConfigNetwork.player_name);
    if (!Platform::FileExists(keyPath))
    {
        Console::WriteLine("Generating key... This may take a while");
        Console::WriteLine("Need to collect enough entropy from the system");
        _key.Generate();
        Console::WriteLine("Key generated, saving private bits as %s", keyPath);

        utf8 keysDirectory[MAX_PATH];
        GetKeysDirectory(keysDirectory, sizeof(keysDirectory));
        if (!platform_ensure_directory_exists(keysDirectory))
        {
            log_error("Unable to create directory %s.", keysDirectory);
            return false;
        }

        try
        {
            auto fs = FileStream(keyPath, FILE_MODE_WRITE);
            _key.SavePrivate(&fs);
        }
        catch (const std::exception&)
        {
            log_error("Unable to save private key at %s.", keyPath);
            return false;
        }

        const std::string hash = _key.PublicKeyHash();
        const utf8* publicKeyHash = hash.c_str();
        GetPublicKeyPath(keyPath, sizeof(keyPath), gConfigNetwork.player_name, publicKeyHash);
        Console::WriteLine("Key generated, saving public bits as %s", keyPath);

        try
        {
            auto fs = FileStream(keyPath, FILE_MODE_WRITE);
            _key.SavePublic(&fs);
        }
        catch (const std::exception&)
        {
            log_error("Unable to save public key at %s.", keyPath);
            return false;
        }
    }
    else
    {
        // LoadPrivate returns validity of loaded key
        bool ok = false;
        try
        {
            log_verbose("Loading key from %s", keyPath);
            auto fs = FileStream(keyPath, FILE_MODE_OPEN);
            ok = _key.LoadPrivate(&fs);
        }
        catch (const std::exception&)
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

void NetworkClient::Reconnect()
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

void NetworkClient::SendPassword(const std::string& password)
{
    utf8 keyPath[MAX_PATH];
    GetPrivateKeyPath(keyPath, sizeof(keyPath), gConfigNetwork.player_name);
    if (!Platform::FileExists(keyPath))
    {
        log_error("Private key %s missing! Restart the game to generate it.", keyPath);
        return;
    }
    try
    {
        auto fs = FileStream(keyPath, FILE_MODE_OPEN);
        _key.LoadPrivate(&fs);
    }
    catch (const std::exception&)
    {
        log_error("Error reading private key from %s.", keyPath);
        return;
    }
    const std::string pubkey = _key.PublicKeyString();

    std::vector<uint8_t> signature;
    _key.Sign(_challenge.data(), _challenge.size(), signature);
    // Don't keep private key in memory. There's no need and it may get leaked
    // when process dump gets collected at some point in future.
    _key.Unload();
    SendAuthRequest(gConfigNetwork.player_name.c_str(), password, pubkey.c_str(), signature);
}

void NetworkClient::Update()
{
    NetworkBase::Update();

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
                        intent.putExtra(INTENT_EXTRA_CALLBACK, []() -> void { GetContext()->GetNetwork()->Close(); });
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
                        intent.putExtra(INTENT_EXTRA_CALLBACK, []() -> void { GetContext()->GetNetwork()->Close(); });
                        context_open_intent(&intent);

                        server_connect_time = platform_get_ticks();
                    }
                    break;
                }
                case SocketStatus::Connected:
                {
                    status = NETWORK_STATUS_CONNECTED;
                    _serverConnection->ResetLastPacketTime();
                    SendToken();
                    char str_authenticating[256];
                    format_string(str_authenticating, 256, STR_MULTIPLAYER_AUTHENTICATING, nullptr);

                    auto intent = Intent(WC_NETWORK_STATUS);
                    intent.putExtra(INTENT_EXTRA_MESSAGE, std::string{ str_authenticating });
                    intent.putExtra(INTENT_EXTRA_CALLBACK, []() -> void { GetContext()->GetNetwork()->Close(); });
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
                uint32_t ticks = platform_get_ticks();
                if (ticks - _lastSentHeartbeat >= 3000)
                {
                    SendHeartbeat(*_serverConnection);
                    _lastSentHeartbeat = ticks;
                }
            }

            break;
        }
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

void NetworkClient::PostUpdate()
{
    NetworkBase::PostUpdate();

    ProcessPlayerInfo();
    ProcessPlayerList();
}

void NetworkClient::ProcessPlayerInfo()
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

void NetworkClient::ProcessPlayerList()
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

        for (auto&& pendingPlayer : itPending->second.players)
        {
            activePlayerIds.push_back(pendingPlayer.Id);

            auto* player = GetPlayerByID(pendingPlayer.Id);
            if (player == nullptr)
            {
                // Add new player.
                player = AddPlayer("", "");
                if (player)
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

void NetworkClient::Flush()
{
    NetworkBase::Flush();

    if (!_serverConnection)
        return;

    _serverConnection->SendQueuedPackets();
}

int32_t NetworkClient::GetMode() const
{
    return NETWORK_MODE_CLIENT;
}

NetworkStats_t NetworkClient::GetStats() const
{
    if (!_serverConnection)
        return {};

    return _serverConnection->Stats;
}

NetworkAuth NetworkClient::GetAuthStatus()
{
    if (!_serverConnection)
        return NetworkAuth::None;

    return _serverConnection->AuthStatus;
}

#    pragma region Packet Dispatchers

void NetworkClient::SendGameStateRequest(uint32_t tick)
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

void NetworkClient::SendToken()
{
    log_verbose("requesting token");
    NetworkPacket packet(NetworkCommand::Token);
    _serverConnection->AuthStatus = NetworkAuth::Requested;
    _serverConnection->QueuePacket(std::move(packet));
}

void NetworkClient::SendAuthRequest(
    const std::string& name, const std::string& password, const std::string& pubkey, const std::vector<uint8_t>& signature)
{
    NetworkPacket packet(NetworkCommand::Auth);
    packet.WriteString(OpenRCT2::GetContext()->GetNetwork()->GetVersion().c_str());
    packet.WriteString(name.c_str());
    packet.WriteString(password.c_str());
    packet.WriteString(pubkey.c_str());
    assert(signature.size() <= static_cast<size_t>(UINT32_MAX));
    packet << static_cast<uint32_t>(signature.size());
    packet.Write(signature.data(), signature.size());
    _serverConnection->AuthStatus = NetworkAuth::Requested;
    _serverConnection->QueuePacket(std::move(packet));
}

void NetworkClient::SendMapRequest(const std::vector<std::string>& objects)
{
    log_verbose("client requests %u objects", uint32_t(objects.size()));
    NetworkPacket packet(NetworkCommand::MapRequest);
    packet << static_cast<uint32_t>(objects.size());
    for (const auto& object : objects)
    {
        log_verbose("client requests object %s", object.c_str());
        packet.Write(reinterpret_cast<const uint8_t*>(object.c_str()), 8);
    }
    _serverConnection->QueuePacket(std::move(packet));
}

void NetworkClient::SendHeartbeat(NetworkConnection& connection) const
{
    log_verbose("Sending heartbeat");

    NetworkPacket packet(NetworkCommand::Heartbeat);
    connection.QueuePacket(std::move(packet));
}

void NetworkClient::SendChat(const char* text)
{
    NetworkPacket packet(NetworkCommand::Chat);
    packet.WriteString(text);
    _serverConnection->QueuePacket(std::move(packet));
}

void NetworkClient::SendGameAction(const GameAction* action)
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

void NetworkClient::SendPing()
{
    NetworkPacket packet(NetworkCommand::Ping);
    _serverConnection->QueuePacket(std::move(packet));
}

void NetworkClient::SendGameInfoRequest()
{
    log_verbose("requesting gameinfo");
    NetworkPacket packet(NetworkCommand::GameInfo);
    _serverConnection->QueuePacket(std::move(packet));
}

#    pragma endregion

#    pragma region Packet Handlers

void NetworkClient::OnPacketAuth(NetworkConnection& connection, NetworkPacket& packet)
{
    uint32_t auth_status;
    packet >> auth_status >> const_cast<uint8_t&>(player_id);
    connection.AuthStatus = static_cast<NetworkAuth>(auth_status);
    switch (connection.AuthStatus)
    {
        case NetworkAuth::Ok:
            SendGameInfoRequest();
            break;
        case NetworkAuth::BadName:
            connection.SetLastDisconnectReason(STR_MULTIPLAYER_BAD_PLAYER_NAME);
            connection.Socket->Disconnect();
            break;
        case NetworkAuth::BadVersion:
        {
            const char* version = packet.ReadString();
            connection.SetLastDisconnectReason(STR_MULTIPLAYER_INCORRECT_SOFTWARE_VERSION, &version);
            connection.Socket->Disconnect();
            break;
        }
        case NetworkAuth::BadPassword:
            connection.SetLastDisconnectReason(STR_MULTIPLAYER_BAD_PASSWORD);
            connection.Socket->Disconnect();
            break;
        case NetworkAuth::VerificationFailure:
            connection.SetLastDisconnectReason(STR_MULTIPLAYER_VERIFICATION_FAILURE);
            connection.Socket->Disconnect();
            break;
        case NetworkAuth::Full:
            connection.SetLastDisconnectReason(STR_MULTIPLAYER_SERVER_FULL);
            connection.Socket->Disconnect();
            break;
        case NetworkAuth::RequirePassword:
            context_open_window_view(WV_NETWORK_PASSWORD);
            break;
        case NetworkAuth::UnknownKeyDisallowed:
            connection.SetLastDisconnectReason(STR_MULTIPLAYER_UNKNOWN_KEY_DISALLOWED);
            connection.Socket->Disconnect();
            break;
        default:
            connection.SetLastDisconnectReason(STR_MULTIPLAYER_RECEIVED_INVALID_DATA);
            connection.Socket->Disconnect();
            break;
    }
}

void NetworkClient::OnPacketMap([[maybe_unused]] NetworkConnection& connection, NetworkPacket& packet)
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
    intent.putExtra(INTENT_EXTRA_CALLBACK, []() -> void { GetContext()->GetNetwork()->Close(); });
    context_open_intent(&intent);

    std::memcpy(&chunk_buffer[offset], const_cast<void*>(static_cast<const void*>(packet.Read(chunksize))), chunksize);
    if (offset + chunksize == size)
    {
        // Allow queue processing of game actions again.
        GameActions::ResumeQueue();

        context_force_close_window_by_class(WC_NETWORK_STATUS);
        bool has_to_free = false;
        uint8_t* data = &chunk_buffer[0];
        size_t data_size = size;
        // zlib-compressed
        if (strcmp("open2_sv6_zlib", reinterpret_cast<char*>(&chunk_buffer[0])) == 0)
        {
            log_verbose("Received zlib-compressed sv6 map");
            has_to_free = true;
            size_t header_len = strlen("open2_sv6_zlib") + 1;
            data = util_zlib_inflate(&chunk_buffer[header_len], size - header_len, &data_size);
            if (data == nullptr)
            {
                log_warning("Failed to decompress data sent from server.");
                Close();
                return;
            }
        }
        else
        {
            log_verbose("Assuming received map is in plain sv6 format");
        }

        auto ms = MemoryStream(data, data_size);
        if (LoadMap(&ms))
        {
            game_load_init();
            game_load_scripts();
            _serverState.tick = gCurrentTicks;
            // window_network_status_open("Loaded new map from network");
            _serverState.state = NetworkServerState::Ok;
            _clientMapLoaded = true;
            gFirstTimeSaving = true;

            // Notify user he is now online and which shortcut key enables chat
            ChatShowConnectedMessage();

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

void NetworkClient::OnPacketChat([[maybe_unused]] NetworkConnection& connection, NetworkPacket& packet)
{
    const char* text = packet.ReadString();
    if (text)
    {
        chat_history_add(text);
    }
}

void NetworkClient::OnPacketTick([[maybe_unused]] NetworkConnection& connection, NetworkPacket& packet)
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
        const char* text = packet.ReadString();
        if (text != nullptr)
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

void NetworkClient::OnPacketPlayerList([[maybe_unused]] NetworkConnection& connection, NetworkPacket& packet)
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

void NetworkClient::OnPacketPing([[maybe_unused]] NetworkConnection& connection, [[maybe_unused]] NetworkPacket& packet)
{
    SendPing();
}

void NetworkClient::OnPacketPingList([[maybe_unused]] NetworkConnection& connection, NetworkPacket& packet)
{
    uint8_t size;
    packet >> size;
    for (uint32_t i = 0; i < size; i++)
    {
        uint8_t id;
        uint16_t ping;
        packet >> id >> ping;
        NetworkPlayer* player = GetPlayerByID(id);
        if (player)
        {
            player->Ping = ping;
        }
    }
    window_invalidate_by_class(WC_PLAYER);
}

void NetworkClient::OnPacketDisconnectMsg(NetworkConnection& connection, NetworkPacket& packet)
{
    static std::string msg;
    const char* disconnectmsg = packet.ReadString();
    if (disconnectmsg)
    {
        msg = disconnectmsg;
        connection.SetLastDisconnectReason(msg.c_str());
    }
}

void NetworkClient::OnPacketGameInfo([[maybe_unused]] NetworkConnection& connection, NetworkPacket& packet)
{
    const char* jsonString = packet.ReadString();
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

    ChatShowServerGreeting();
}

void NetworkClient::OnPacketShowError([[maybe_unused]] NetworkConnection& connection, NetworkPacket& packet)
{
    rct_string_id title, message;
    packet >> title >> message;
    context_show_error(title, message, {});
}

void NetworkClient::OnPacketGroupList([[maybe_unused]] NetworkConnection& connection, NetworkPacket& packet)
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

void NetworkClient::OnPacketEvent([[maybe_unused]] NetworkConnection& connection, NetworkPacket& packet)
{
    char text[256];
    uint16_t eventType;
    packet >> eventType;
    switch (eventType)
    {
        case SERVER_EVENT_PLAYER_JOINED:
        {
            const char* playerName = packet.ReadString();
            format_string(text, 256, STR_MULTIPLAYER_PLAYER_HAS_JOINED_THE_GAME, &playerName);
            chat_history_add(text);
            break;
        }
        case SERVER_EVENT_PLAYER_DISCONNECTED:
        {
            const char* playerName = packet.ReadString();
            const char* reason = packet.ReadString();
            const char* args[] = { playerName, reason };
            if (str_is_null_or_empty(reason))
            {
                format_string(text, 256, STR_MULTIPLAYER_PLAYER_HAS_DISCONNECTED_NO_REASON, args);
            }
            else
            {
                format_string(text, 256, STR_MULTIPLAYER_PLAYER_HAS_DISCONNECTED_WITH_REASON, args);
            }
            chat_history_add(text);
            break;
        }
    }
}

void NetworkClient::OnPacketToken(NetworkConnection& connection, NetworkPacket& packet)
{
    utf8 keyPath[MAX_PATH];
    GetPrivateKeyPath(keyPath, sizeof(keyPath), gConfigNetwork.player_name);
    if (!Platform::FileExists(keyPath))
    {
        log_error("Key file (%s) was not found. Restart client to re-generate it.", keyPath);
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
        log_error("Failed to load key %s", keyPath);
        connection.SetLastDisconnectReason(STR_MULTIPLAYER_VERIFICATION_FAILURE);
        connection.Socket->Disconnect();
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
        connection.Socket->Disconnect();
        return;
    }
    // Don't keep private key in memory. There's no need and it may get leaked
    // when process dump gets collected at some point in future.
    _key.Unload();

    const char* password = String::IsNullOrEmpty(gCustomPassword) ? "" : gCustomPassword;
    SendAuthRequest(gConfigNetwork.player_name.c_str(), password, pubkey.c_str(), signature);
}

void NetworkClient::OnPacketObjectsList(NetworkConnection& connection, NetworkPacket& packet)
{
    auto& repo = GetContext()->GetObjectRepository();

    uint32_t index = 0;
    uint32_t totalObjects = 0;
    packet >> index >> totalObjects;

    static constexpr uint32_t OBJECT_START_INDEX = 0;
    if (index == OBJECT_START_INDEX)
    {
        _missingObjects.clear();
    }

    if (totalObjects > OBJECT_ENTRY_COUNT)
    {
        connection.SetLastDisconnectReason(STR_MULTIPLAYER_SERVER_INVALID_REQUEST);
        connection.Socket->Disconnect();
        log_warning("Server sent invalid amount of objects");
        return;
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
        intent.putExtra(INTENT_EXTRA_CALLBACK, []() -> void { GetContext()->GetNetwork()->Close(); });
        context_open_intent(&intent);

        char objectName[12]{};
        std::memcpy(objectName, packet.Read(8), 8);

        uint32_t checksum = 0;
        uint32_t flags = 0;
        packet >> checksum >> flags;

        const auto* object = repo.FindObjectLegacy(objectName);
        // This could potentially request the object if checksums don't match, but since client
        // won't replace its version with server-provided one, we don't do that.
        if (object == nullptr)
        {
            log_verbose("Requesting object %s with checksum %x from server", objectName, checksum);
            _missingObjects.emplace_back(objectName);
        }
        else if (object->ObjectEntry.checksum != checksum || object->ObjectEntry.flags != flags)
        {
            log_warning(
                "Object %s has different checksum/flags (%x/%x) than server (%x/%x).", objectName, object->ObjectEntry.checksum,
                object->ObjectEntry.flags, checksum, flags);
        }
    }

    if (index + 1 >= totalObjects)
    {
        log_verbose("client received object list, it has %u entries", totalObjects);
        SendMapRequest(_missingObjects);
        _missingObjects.clear();
    }
}

void NetworkClient::OnPacketGameAction([[maybe_unused]] NetworkConnection& connection, NetworkPacket& packet)
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

void NetworkClient::OnPacketPlayerInfo([[maybe_unused]] NetworkConnection& connection, NetworkPacket& packet)
{
    uint32_t tick;
    packet >> tick;

    NetworkPlayer playerInfo;
    playerInfo.Read(packet);

    _pendingPlayerInfo.emplace(tick, playerInfo);
}

void NetworkClient::OnPacketGameState(NetworkConnection& connection, NetworkPacket& packet)
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

        IGameStateSnapshots* snapshots = GetContext()->GetGameStateSnapshots();

        GameStateSnapshot_t& serverSnapshot = snapshots->CreateSnapshot();
        snapshots->SerialiseSnapshot(serverSnapshot, ds);

        const GameStateSnapshot_t* desyncSnapshot = snapshots->GetLinkedSnapshot(tick);
        if (desyncSnapshot)
        {
            GameStateCompareData_t cmpData = snapshots->Compare(serverSnapshot, *desyncSnapshot);

            std::string outputPath = GetContext()->GetPlatformEnvironment()->GetDirectoryPath(
                DIRBASE::USER, DIRID::LOG_DESYNCS);

            platform_ensure_directory_exists(outputPath.c_str());

            char uniqueFileName[128] = {};
            snprintf(
                uniqueFileName, sizeof(uniqueFileName), "desync_%llu_%u.txt",
                static_cast<long long unsigned>(platform_get_datetime_now_utc()), tick);

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

void NetworkClient::OnPacketScripts(NetworkConnection& connection, NetworkPacket& packet)
{
    uint32_t numScripts{};
    packet >> numScripts;

#    ifdef ENABLE_SCRIPTING
    auto& scriptEngine = GetContext()->GetScriptEngine();
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

#    pragma endregion

#endif // DISABLE_NETWORK
