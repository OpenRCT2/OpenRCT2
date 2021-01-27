#include "NetworkServer.h"

#ifndef DISABLE_NETWORK

#    include "../Context.h"
#    include "../GameStateSnapshots.h"
#    include "../PlatformEnvironment.h"
#    include "../actions/GameAction.h"
#    include "../actions/PeepPickupAction.h"
#    include "../core/Console.hpp"
#    include "../core/FileStream.h"
#    include "../core/Json.hpp"
#    include "../core/String.hpp"
#    include "../interface/Chat.h"
#    include "../object/ObjectManager.h"
#    include "../object/ObjectRepository.h"
#    include "../platform/Platform2.h"
#    include "../rct2/S6Exporter.h"
#    include "../scenario/Scenario.h"
#    include "../scripting/ScriptEngine.h"
#    include "../ui/UiContext.h"
#    include "../ui/WindowManager.h"
#    include "../util/SawyerCoding.h"
#    include "../windows/Intent.h"
#    include "../world/Park.h"

namespace OpenRCT2
{
    NetworkServer::NetworkServer(const std::shared_ptr<OpenRCT2::IPlatformEnvironment>& env)
        : NetworkBase(env)
    {
        SetupCommandHandler(NetworkCommand::Auth, &NetworkServer::OnPacketAuthRequest);
        SetupCommandHandler(NetworkCommand::Chat, &NetworkServer::OnPacketChat);
        SetupCommandHandler(NetworkCommand::GameAction, &NetworkServer::OnPacketGameAction);
        SetupCommandHandler(NetworkCommand::Ping, &NetworkServer::OnPacketPing);
        SetupCommandHandler(NetworkCommand::GameInfo, &NetworkServer::OnPacketGameInfo);
        SetupCommandHandler(NetworkCommand::Token, &NetworkServer::OnPacketToken);
        SetupCommandHandler(NetworkCommand::MapRequest, &NetworkServer::OnPacketMapRequest);
        SetupCommandHandler(NetworkCommand::RequestGameState, &NetworkServer::OnPacketRequestGameState);
        SetupCommandHandler(NetworkCommand::Heartbeat, &NetworkServer::OnPacketHeartbeat);

        _server_log_fs << std::unitbuf;
    }

    void NetworkServer::BeginServerLog()
    {
        auto directory = _env->GetDirectoryPath(DIRBASE::USER, DIRID::LOG_SERVER);
        _serverLogPath = BeginLog(directory, ServerName, _serverLogFilenameFormat);

#    if defined(_WIN32) && !defined(__MINGW32__)
        auto pathW = String::ToWideChar(_serverLogPath.c_str());
        _server_log_fs.open(pathW.c_str(), std::ios::out | std::ios::app | std::ios::binary);
#    else
        _server_log_fs.open(_serverLogPath, std::ios::out | std::ios::app | std::ios::binary);
#    endif

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

    void NetworkServer::AppendServerLog(const std::string& s)
    {
        if (gConfigNetwork.log_server_actions && _server_log_fs.is_open())
        {
            AppendLog(_server_log_fs, s);
        }
    }

    void NetworkServer::CloseServerLog()
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

    bool NetworkServer::BeginServer(uint16_t port, const std::string& address)
    {
        Close();

        if (!Init())
            return false;

        BeginChatLog();
        BeginServerLog();

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

        NetworkPlayer* player = AddPlayer(gConfigNetwork.player_name, "");
        player->Flags |= NETWORK_PLAYER_FLAG_ISSERVER;
        player->Group = 0;
        _playerId = player->Id;

        if (OpenRCT2::GetContext()->GetNetwork()->GetMode() == NETWORK_MODE_SERVER)
        {
            // Add SERVER to users.json and save.
            NetworkUser* networkUser = _userManager.GetOrAddUser(player->KeyHash);
            networkUser->GroupId = player->Group;
            networkUser->Name = player->Name;
            _userManager.Save();
        }

        printf("Ready for clients...\n");

        ChatShowConnectedMessage();
        ChatShowServerGreeting();

        status = NETWORK_STATUS_CONNECTED;
        listening_port = port;
        _advertiser = CreateServerAdvertiser(listening_port);

        game_load_scripts();

        return true;
    }

    void NetworkServer::Close()
    {
        NetworkBase::Close();

        _clients.clear();

        CloseServerLog();
    }

    void NetworkServer::Update()
    {
        NetworkBase::Update();

        for (auto& connection : _clients)
        {
            // This can be called multiple times before the connection is removed.
            if (connection->IsDisconnected)
                continue;

            if (!ProcessConnection(*connection))
            {
                connection->IsDisconnected = true;
            }
            else
            {
                DecayCooldown(connection->Player);
            }
        }

        uint32_t ticks = Platform::GetTicks();
        if (ticks > _lastPingTime + 3000)
        {
            SendPing();
            SendPingList();
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

    void NetworkServer::DecayCooldown(NetworkPlayer* player)
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

    void NetworkServer::PostUpdate()
    {
        NetworkBase::PostUpdate();

        ProcessPlayerList();
        ProcessDisconnectedClients();
    }

    void NetworkServer::Flush()
    {
        NetworkBase::Flush();

        for (auto& it : _clients)
        {
            it->SendQueuedPackets();
        }
    }

    int32_t NetworkServer::GetMode() const
    {
        return NETWORK_MODE_SERVER;
    }

    NetworkStats_t NetworkServer::GetStats() const
    {
        NetworkStats_t stats = {};
        for (auto& connection : _clients)
        {
            for (size_t n = 0; n < EnumValue(NetworkStatisticsGroup::Max); n++)
            {
                stats.bytesReceived[n] += connection->Stats.bytesReceived[n];
                stats.bytesSent[n] += connection->Stats.bytesSent[n];
            }
        }
        return stats;
    }

    NetworkAuth NetworkServer::GetAuthStatus()
    {
        return NetworkAuth::Ok;
    }

    NetworkConnection* NetworkServer::GetPlayerConnection(NetworkPlayerId_t id)
    {
        auto player = GetPlayerByID(id);
        if (player != nullptr)
        {
            auto it = std::find_if(
                _clients.begin(), _clients.end(), [player](const auto& conn) -> bool { return conn->Player == player; });
            if (it == _clients.end())
                return nullptr;
            return it->get();
        }
        return nullptr;
    }

    void NetworkServer::KickPlayer(int32_t playerId)
    {
        for (auto& client_connection : _clients)
        {
            if (client_connection->Player->Id == playerId)
            {
                // Disconnect the client gracefully
                client_connection->SetLastDisconnectReason(STR_MULTIPLAYER_KICKED);
                char str_disconnect_msg[256];
                format_string(str_disconnect_msg, 256, STR_MULTIPLAYER_KICKED_REASON, nullptr);
                SendDisconnectMsg(*client_connection, str_disconnect_msg);
                client_connection->Socket->Disconnect();
                break;
            }
        }
    }

    void NetworkServer::AddClient(std::unique_ptr<ITcpSocket>&& socket)
    {
        // Log connection info.
        char addr[128];
        snprintf(addr, sizeof(addr), "Client joined from %s", socket->GetHostName());

        AppendServerLog(addr);

        // Store connection
        auto connection = std::make_unique<NetworkConnection>();
        connection->Socket = std::move(socket);

        _clients.push_back(std::move(connection));
    }

    void NetworkServer::SendPacketToClients(const NetworkPacket& packet, bool front, bool gameCmd)
    {
        for (auto& client : _clients)
        {
            if (client->IsDisconnected)
            {
                // Client will be removed at the end of the tick, don't bother.
                continue;
            }

            if (gameCmd)
            {
                // If marked as game command we can not send the packet to connections that are not fully connected.
                // Sending the packet would cause the client to store a command that is behind the tick where he starts,
                // which would be essentially never executed. The clients do not require commands before the server has not sent
                // the map data.
                if (client->Player == nullptr)
                {
                    continue;
                }
            }
            auto packetCopy = packet;
            client->QueuePacket(std::move(packetCopy), front);
        }
    }

    void NetworkServer::SendGameAction(const GameAction* action)
    {
        NetworkPacket packet(NetworkCommand::GameAction);

        DataSerialiser stream(true);
        action->Serialise(stream);

        packet << gCurrentTicks << action->GetType() << stream;

        SendPacketToClients(packet, false, true);
    }

    void NetworkServer::SendTick()
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
            rct_sprite_checksum checksum = sprite_checksum();
            packet.WriteString(checksum.ToString().c_str());
        }

        SendPacketToClients(packet, false, false);
    }

    void NetworkServer::Server_Send_GAMEINFO(NetworkConnection& connection)
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

        packet.WriteString(jsonObj.dump().c_str());
        packet << gConfigNetwork.desync_debugging;

#    endif
        connection.QueuePacket(std::move(packet));
    }

    void NetworkServer::Server_Send_SHOWERROR(NetworkConnection& connection, rct_string_id title, rct_string_id message)
    {
        NetworkPacket packet(NetworkCommand::ShowError);
        packet << title << message;
        connection.QueuePacket(std::move(packet));
    }

    void NetworkServer::Server_Send_GROUPLIST(NetworkConnection& connection)
    {
        NetworkPacket packet(NetworkCommand::GroupList);
        packet << static_cast<uint8_t>(group_list.size()) << default_group;
        for (auto& group : group_list)
        {
            group->Write(packet);
        }
        connection.QueuePacket(std::move(packet));
    }

    void NetworkServer::Server_Send_EVENT_PLAYER_JOINED(const char* playerName)
    {
        NetworkPacket packet(NetworkCommand::Event);
        packet << static_cast<uint16_t>(SERVER_EVENT_PLAYER_JOINED);
        packet.WriteString(playerName);
        SendPacketToClients(packet, false, false);
    }

    void NetworkServer::SendEventPlayerDisconnected(const char* playerName, const char* reason)
    {
        NetworkPacket packet(NetworkCommand::Event);
        packet << static_cast<uint16_t>(SERVER_EVENT_PLAYER_DISCONNECTED);
        packet.WriteString(playerName);
        packet.WriteString(reason);
        SendPacketToClients(packet, false, false);
    }

    void NetworkServer::SendChat(const char* text, const std::vector<uint8_t>& playerIds)
    {
        NetworkPacket packet(NetworkCommand::Chat);
        packet.WriteString(text);

        if (playerIds.empty())
        {
            // Empty players / default value means send to all players
            SendPacketToClients(packet, false, false);
        }
        else
        {
            for (auto playerId : playerIds)
            {
                auto conn = GetPlayerConnection(playerId);
                if (conn != nullptr && !conn->IsDisconnected)
                {
                    conn->QueuePacket(packet);
                }
            }
        }
    }

    void NetworkServer::SendPlayerInfo(int32_t playerId)
    {
        NetworkPacket packet(NetworkCommand::PlayerInfo);
        packet << gCurrentTicks;

        auto* player = GetPlayerByID(playerId);
        if (player == nullptr)
            return;

        player->Write(packet);
        SendPacketToClients(packet, false, false);
    }

    void NetworkServer::SendPlayerList()
    {
        NetworkPacket packet(NetworkCommand::PlayerList);
        packet << gCurrentTicks << static_cast<uint8_t>(_playerList.size());
        for (auto& player : _playerList)
        {
            player->Write(packet);
        }
        SendPacketToClients(packet, false, false);
    }

    void NetworkServer::SendPing()
    {
        _lastPingTime = platform_get_ticks();
        NetworkPacket packet(NetworkCommand::Ping);
        for (auto& client_connection : _clients)
        {
            client_connection->PingTime = platform_get_ticks();
        }
        SendPacketToClients(packet, false, false);
    }

    void NetworkServer::SendPingList()
    {
        NetworkPacket packet(NetworkCommand::PingList);
        packet << static_cast<uint8_t>(_playerList.size());
        for (auto& player : _playerList)
        {
            packet << player->Id << player->Ping;
        }
        SendPacketToClients(packet, false, false);
    }

    void NetworkServer::SendDisconnectMsg(NetworkConnection& connection, const char* msg)
    {
        NetworkPacket packet(NetworkCommand::DisconnectMessage);
        packet.WriteString(msg);
        connection.QueuePacket(std::move(packet));
    }

    void NetworkServer::SendToken(NetworkConnection& connection)
    {
        NetworkPacket packet(NetworkCommand::Token);
        packet << static_cast<uint32_t>(connection.Challenge.size());
        packet.Write(connection.Challenge.data(), connection.Challenge.size());
        connection.QueuePacket(std::move(packet));
    }

    void NetworkServer::SendObjectsList(
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

                log_verbose("Object %.8s (checksum %x)", object->ObjectEntry.name, object->ObjectEntry.checksum);
                packet.Write(reinterpret_cast<const uint8_t*>(object->ObjectEntry.name), 8);
                packet << object->ObjectEntry.checksum << object->ObjectEntry.flags;

                connection.QueuePacket(std::move(packet));
            }
        }
    }

    void NetworkServer::SendScripts(NetworkConnection& connection) const
    {
        NetworkPacket packet(NetworkCommand::Scripts);

#    ifdef ENABLE_SCRIPTING
        using namespace OpenRCT2::Scripting;

        auto& scriptEngine = GetContext()->GetScriptEngine();
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

    static bool SaveMap(IStream* stream, const std::vector<const ObjectRepositoryItem*>& objects)
    {
        bool result = false;
        map_reorganise_elements();
        viewport_set_saved_view();
        try
        {
            auto s6exporter = std::make_unique<S6Exporter>();
            s6exporter->ExportObjectsList = objects;
            s6exporter->Export();
            s6exporter->SaveGame(stream);

            // Write other data not in normal save files
            stream->WriteValue<uint32_t>(gGamePaused);
            stream->WriteValue<uint32_t>(_guestGenerationProbability);
            stream->WriteValue<uint32_t>(_suggestedGuestMaximum);
            stream->WriteValue<uint8_t>(gCheatsAllowTrackPlaceInvalidHeights);
            stream->WriteValue<uint8_t>(gCheatsEnableAllDrawableTrackPieces);
            stream->WriteValue<uint8_t>(gCheatsSandboxMode);
            stream->WriteValue<uint8_t>(gCheatsDisableClearanceChecks);
            stream->WriteValue<uint8_t>(gCheatsDisableSupportLimits);
            stream->WriteValue<uint8_t>(gCheatsDisableTrainLengthLimit);
            stream->WriteValue<uint8_t>(gCheatsEnableChainLiftOnAllTrack);
            stream->WriteValue<uint8_t>(gCheatsShowAllOperatingModes);
            stream->WriteValue<uint8_t>(gCheatsShowVehiclesFromOtherTrackTypes);
            stream->WriteValue<uint8_t>(gCheatsFastLiftHill);
            stream->WriteValue<uint8_t>(gCheatsDisableBrakesFailure);
            stream->WriteValue<uint8_t>(gCheatsDisableAllBreakdowns);
            stream->WriteValue<uint8_t>(gCheatsBuildInPauseMode);
            stream->WriteValue<uint8_t>(gCheatsIgnoreRideIntensity);
            stream->WriteValue<uint8_t>(gCheatsDisableVandalism);
            stream->WriteValue<uint8_t>(gCheatsDisableLittering);
            stream->WriteValue<uint8_t>(gCheatsNeverendingMarketing);
            stream->WriteValue<uint8_t>(gCheatsFreezeWeather);
            stream->WriteValue<uint8_t>(gCheatsDisablePlantAging);
            stream->WriteValue<uint8_t>(gCheatsAllowArbitraryRideTypeChanges);
            stream->WriteValue<uint8_t>(gCheatsDisableRideValueAging);
            stream->WriteValue<uint8_t>(gConfigGeneral.show_real_names_of_guests);
            stream->WriteValue<uint8_t>(gCheatsIgnoreResearchStatus);

            result = true;
        }
        catch (const std::exception&)
        {
        }
        return result;
    }

    static std::vector<uint8_t> SaveMapForNetwork(const std::vector<const ObjectRepositoryItem*>& objects)
    {
        std::vector<uint8_t> header;
        bool RLEState = gUseRLE;
        gUseRLE = false;

        auto ms = OpenRCT2::MemoryStream();
        if (!SaveMap(&ms, objects))
        {
            log_warning("Failed to export map.");
            return header;
        }
        gUseRLE = RLEState;

        const void* data = ms.GetData();
        int32_t size = ms.GetLength();

        auto compressed = util_zlib_deflate(static_cast<const uint8_t*>(data), size);
        if (compressed != std::nullopt)
        {
            std::string headerString = "open2_sv6_zlib";
            header.resize(headerString.size() + 1 + compressed->size());
            std::memcpy(&header[0], headerString.c_str(), headerString.size() + 1);
            std::memcpy(&header[headerString.size() + 1], compressed->data(), compressed->size());
            log_verbose(
                "Sending map of size %u bytes, compressed to %u bytes", size, headerString.size() + 1 + compressed->size());
        }
        else
        {
            log_warning("Failed to compress the data, falling back to non-compressed sv6.");
            header.resize(size);
            std::memcpy(header.data(), data, size);
        }
        return header;
    }

    void NetworkServer::SendMap(NetworkConnection* connection)
    {
        std::vector<const ObjectRepositoryItem*> objects;
        if (connection)
        {
            objects = connection->RequestedObjects;
        }
        else
        {
            // This will send all custom objects to connected clients
            // TODO: fix it so custom objects negotiation is performed even in this case.
            auto context = GetContext();
            auto& objManager = context->GetObjectManager();
            objects = objManager.GetPackableObjects();
        }

        auto header = SaveMapForNetwork(objects);
        if (header.empty())
        {
            if (connection)
            {
                connection->SetLastDisconnectReason(STR_MULTIPLAYER_CONNECTION_CLOSED);
                connection->Socket->Disconnect();
            }
            return;
        }
        size_t chunksize = PacketChunkSize;
        for (size_t i = 0; i < header.size(); i += chunksize)
        {
            size_t datasize = std::min(chunksize, header.size() - i);
            NetworkPacket packet(NetworkCommand::Map);
            packet << static_cast<uint32_t>(header.size()) << static_cast<uint32_t>(i);
            packet.Write(&header[i], datasize);
            if (connection)
            {
                connection->QueuePacket(std::move(packet));
            }
            else
            {
                SendPacketToClients(packet, false, false);
            }
        }
    }

    void NetworkServer::SendAuthStatus(NetworkConnection& connection)
    {
        uint8_t new_playerid = 0;
        if (connection.Player)
        {
            new_playerid = connection.Player->Id;
        }
        NetworkPacket packet(NetworkCommand::Auth);
        packet << static_cast<uint32_t>(connection.AuthStatus) << new_playerid;
        if (connection.AuthStatus == NetworkAuth::BadVersion)
        {
            packet.WriteString(OpenRCT2::GetContext()->GetNetwork()->GetVersion().c_str());
        }
        connection.QueuePacket(std::move(packet));
        if (connection.AuthStatus != NetworkAuth::Ok && connection.AuthStatus != NetworkAuth::RequirePassword)
        {
            connection.Socket->Disconnect();
        }
    }

    static bool ProcessPlayerAuthenticatePluginHooks(
        const NetworkConnection& connection, const std::string_view& name, const std::string_view& publicKeyHash)
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

    void NetworkServer::RemovePlayer(std::unique_ptr<NetworkConnection>& connection)
    {
        NetworkPlayer* connection_player = connection->Player;
        if (connection_player == nullptr)
            return;

        _playerList.erase(
            std::remove_if(
                _playerList.begin(), _playerList.end(),
                [connection_player](std::unique_ptr<NetworkPlayer>& player) { return player.get() == connection_player; }),
            _playerList.end());

        // Send new player list.
        _playerListInvalidated = true;
    }

    void NetworkServer::ProcessDisconnectedClients()
    {
        for (auto it = _clients.begin(); it != _clients.end();)
        {
            auto& connection = *it;
            if (connection->IsDisconnected)
            {
                OnClientDisconnected(connection);
                RemovePlayer(connection);

                it = _clients.erase(it);
            }
            else
            {
                it++;
            }
        }
    }

    void NetworkServer::OnClientDisconnected(std::unique_ptr<NetworkConnection>& connection)
    {
        NetworkPlayer* connection_player = connection->Player;
        if (connection_player == nullptr)
            return;

        char text[256];
        const char* has_disconnected_args[2] = {
            connection_player->Name.c_str(),
            connection->GetLastDisconnectReason(),
        };
        if (has_disconnected_args[1])
        {
            format_string(text, 256, STR_MULTIPLAYER_PLAYER_HAS_DISCONNECTED_WITH_REASON, has_disconnected_args);
        }
        else
        {
            format_string(text, 256, STR_MULTIPLAYER_PLAYER_HAS_DISCONNECTED_NO_REASON, &(has_disconnected_args[0]));
        }

        chat_history_add(text);

        Peep* pickupPeep = GetPickupPeep(connection_player->Id);
        if (pickupPeep)
        {
            PeepPickupAction pickupAction{ PeepPickupType::Cancel,
                                           pickupPeep->sprite_index,
                                           { GetPickupPeepOldX(connection_player->Id), 0, 0 },
                                           GetPlayerID() };
            auto res = GameActions::Execute(&pickupAction);
        }

        SendEventPlayerDisconnected(const_cast<char*>(connection_player->Name.c_str()), connection->GetLastDisconnectReason());

        // Log player disconnected event
        AppendServerLog(text);

        ProcessPlayerLeftPluginHooks(connection_player->Id);
    }

    void NetworkServer::ProcessPlayerList()
    {
        // Avoid sending multiple times the player list, we mark the list invalidated on modifications
        // and then send at the end of the tick the final player list.
        if (!_playerListInvalidated)
            return;

        _playerListInvalidated = false;
        SendPlayerList();
    }

    void NetworkServer::OnPacketAuthRequest(NetworkConnection& connection, NetworkPacket& packet)
    {
        if (connection.AuthStatus != NetworkAuth::Ok)
        {
            const char* gameversion = packet.ReadString();
            const char* name = packet.ReadString();
            const char* password = packet.ReadString();
            const char* pubkey = packet.ReadString();
            uint32_t sigsize;
            packet >> sigsize;
            if (pubkey == nullptr)
            {
                connection.AuthStatus = NetworkAuth::VerificationFailure;
            }
            else
            {
                try
                {
                    std::vector<uint8_t> signature;
                    signature.resize(sigsize);

                    const uint8_t* signatureData = packet.Read(sigsize);
                    if (signatureData == nullptr)
                    {
                        throw std::runtime_error("Failed to read packet.");
                    }

                    std::memcpy(signature.data(), signatureData, sigsize);

                    auto ms = MemoryStream(pubkey, strlen(pubkey));
                    if (!connection.Key.LoadPublic(&ms))
                    {
                        throw std::runtime_error("Failed to load public key.");
                    }

                    bool verified = connection.Key.Verify(connection.Challenge.data(), connection.Challenge.size(), signature);
                    const std::string hash = connection.Key.PublicKeyHash();
                    if (verified)
                    {
                        log_verbose("Signature verification ok. Hash %s", hash.c_str());
                        if (gConfigNetwork.known_keys_only && _userManager.GetUserByHash(hash) == nullptr)
                        {
                            log_verbose("Hash %s, not known", hash.c_str());
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
                        log_verbose("Signature verification failed!");
                    }
                }
                catch (const std::exception&)
                {
                    connection.AuthStatus = NetworkAuth::VerificationFailure;
                    log_verbose("Signature verification failed, invalid data!");
                }
            }

            bool passwordless = false;
            if (connection.AuthStatus == NetworkAuth::Verified)
            {
                const NetworkGroup* group = GetGroupByID(GetGroupIDByHash(connection.Key.PublicKeyHash()));
                passwordless = group->CanPerformCommand(GameCommand::PasswordlessLogin);
            }
            if (!gameversion || OpenRCT2::GetContext()->GetNetwork()->GetVersion() != gameversion)
            {
                connection.AuthStatus = NetworkAuth::BadVersion;
            }
            else if (!name)
            {
                connection.AuthStatus = NetworkAuth::BadName;
            }
            else if (!passwordless)
            {
                if ((!password || strlen(password) == 0) && !_password.empty())
                {
                    connection.AuthStatus = NetworkAuth::RequirePassword;
                }
                else if (password && _password != password)
                {
                    connection.AuthStatus = NetworkAuth::BadPassword;
                }
            }

            if (static_cast<size_t>(gConfigNetwork.maxplayers) <= _playerList.size())
            {
                connection.AuthStatus = NetworkAuth::Full;
            }
            else if (connection.AuthStatus == NetworkAuth::Verified)
            {
                const std::string hash = connection.Key.PublicKeyHash();
                if (ProcessPlayerAuthenticatePluginHooks(connection, name, hash))
                {
                    connection.AuthStatus = NetworkAuth::Ok;
                    OnClientAuthenticated(connection, name);
                }
                else
                {
                    connection.AuthStatus = NetworkAuth::VerificationFailure;
                }
            }
            else if (connection.AuthStatus != NetworkAuth::RequirePassword)
            {
                log_error("Unknown failure (%d) while authenticating client", connection.AuthStatus);
            }
            SendAuthStatus(connection);
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

    void NetworkServer::OnPacketChat(NetworkConnection& connection, NetworkPacket& packet)
    {
        auto szText = packet.ReadString();
        if (szText == nullptr || szText[0] == '\0')
            return;

        if (connection.Player)
        {
            NetworkGroup* group = GetGroupByID(connection.Player->Group);
            if (!group || !group->CanPerformCommand(GameCommand::Chat))
            {
                return;
            }
        }

        std::string text = szText;
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
        SendChat(formatted);
    }

    void NetworkServer::OnPacketGameAction(NetworkConnection& connection, NetworkPacket& packet)
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

    void NetworkServer::OnPacketPing(NetworkConnection& connection, [[maybe_unused]] NetworkPacket& packet)
    {
        int32_t ping = platform_get_ticks() - connection.PingTime;
        if (ping < 0)
        {
            ping = 0;
        }
        if (connection.Player)
        {
            connection.Player->Ping = ping;
            window_invalidate_by_number(WC_PLAYER, connection.Player->Id);
        }
    }

    void NetworkServer::OnPacketGameInfo(NetworkConnection& connection, [[maybe_unused]] NetworkPacket& packet)
    {
        Server_Send_GAMEINFO(connection);
    }

    void NetworkServer::OnPacketToken(NetworkConnection& connection, [[maybe_unused]] NetworkPacket& packet)
    {
        uint8_t token_size = 10 + (rand() & 0x7f);
        connection.Challenge.resize(token_size);
        for (int32_t i = 0; i < token_size; i++)
        {
            connection.Challenge[i] = static_cast<uint8_t>(rand() & 0xff);
        }
        SendToken(connection);
    }

    void NetworkServer::OnPacketMapRequest(NetworkConnection& connection, NetworkPacket& packet)
    {
        uint32_t size;
        packet >> size;
        if (size > OBJECT_ENTRY_COUNT)
        {
            connection.SetLastDisconnectReason(STR_MULTIPLAYER_CLIENT_INVALID_REQUEST);
            connection.Socket->Disconnect();
            std::string playerName = "(unknown)";
            if (connection.Player)
            {
                playerName = connection.Player->Name;
            }
            std::string text = std::string("Player ") + playerName + std::string(" requested invalid amount of objects");
            AppendServerLog(text);
            log_warning(text.c_str());
            return;
        }
        log_verbose("Client requested %u objects", size);
        auto& repo = GetContext()->GetObjectRepository();
        for (uint32_t i = 0; i < size; i++)
        {
            const char* name = reinterpret_cast<const char*>(packet.Read(8));
            // This is required, as packet does not have null terminator
            std::string s(name, name + 8);
            log_verbose("Client requested object %s", s.c_str());
            const ObjectRepositoryItem* item = repo.FindObjectLegacy(s.c_str());
            if (item == nullptr)
            {
                log_warning("Client tried getting non-existent object %s from us.", s.c_str());
            }
            else
            {
                connection.RequestedObjects.push_back(item);
            }
        }

        const char* player_name = static_cast<const char*>(connection.Player->Name.c_str());
        SendMap(&connection);
        Server_Send_EVENT_PLAYER_JOINED(player_name);
        Server_Send_GROUPLIST(connection);
    }

    void NetworkServer::OnPacketRequestGameState(NetworkConnection& connection, NetworkPacket& packet)
    {
        uint32_t tick;
        packet >> tick;

        if (gConfigNetwork.desync_debugging == false)
        {
            // Ignore this if this is off.
            return;
        }

        IGameStateSnapshots* snapshots = GetContext()->GetGameStateSnapshots();

        const GameStateSnapshot_t* snapshot = snapshots->GetLinkedSnapshot(tick);
        if (snapshot)
        {
            MemoryStream snapshotMemory;
            DataSerialiser ds(true, snapshotMemory);

            snapshots->SerialiseSnapshot(const_cast<GameStateSnapshot_t&>(*snapshot), ds);

            uint32_t bytesSent = 0;
            uint32_t length = static_cast<uint32_t>(snapshotMemory.GetLength());
            while (bytesSent < length)
            {
                uint32_t dataSize = PacketChunkSize;
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

    void NetworkServer::OnPacketHeartbeat(NetworkConnection& connection, NetworkPacket& packet)
    {
        log_verbose("Client %s heartbeat", connection.Socket->GetHostName());
        connection.ResetLastPacketTime();
    }

    void NetworkServer::OnClientAuthenticated(NetworkConnection& connection, const char* name)
    {
        auto keyHash = connection.Key.PublicKeyHash();

        auto* player = AddPlayer(name, keyHash);
        if (!player)
        {
            log_error("Unable to add new player");
            return;
        }

        connection.Player = player;

        char text[256];
        const char* player_name = static_cast<const char*>(player->Name.c_str());
        format_string(text, 256, STR_MULTIPLAYER_PLAYER_HAS_JOINED_THE_GAME, &player_name);
        chat_history_add(text);

        auto context = GetContext();
        auto& objManager = context->GetObjectManager();
        auto objects = objManager.GetPackableObjects();
        SendObjectsList(connection, objects);
        SendScripts(connection);

        // Log player joining event
        std::string playerNameHash = player->Name + " (" + keyHash + ")";
        player_name = static_cast<const char*>(playerNameHash.c_str());
        format_string(text, 256, STR_MULTIPLAYER_PLAYER_HAS_JOINED_THE_GAME, &player_name);
        AppendServerLog(text);

        ProcessPlayerJoinedPluginHooks(player->Id);
    }

} // namespace OpenRCT2

#endif // DISABLE_NETWORK
