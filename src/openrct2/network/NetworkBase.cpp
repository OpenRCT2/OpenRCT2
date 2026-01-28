/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "NetworkBase.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../Game.h"
#include "../GameState.h"
#include "../GameStateSnapshots.h"
#include "../OpenRCT2.h"
#include "../PlatformEnvironment.h"
#include "../Version.h"
#include "../actions/LoadOrQuitAction.h"
#include "../actions/NetworkModifyGroupAction.h"
#include "../actions/PeepPickupAction.h"
#include "../core/File.h"
#include "../core/Guard.hpp"
#include "../core/Json.hpp"
#include "../drawing/Drawing.h"
#include "../entity/EntityRegistry.h"
#include "../entity/EntityTweener.h"
#include "../localisation/Formatter.h"
#include "../localisation/Formatting.h"
#include "../localisation/LocalisationService.h"
#include "../park/ParkFile.h"
#include "../platform/Platform.h"
#include "../sawyer_coding/SawyerCoding.h"
#include "../scripting/ScriptEngine.h"
#include "../ui/WindowManager.h"
#include "../util/Util.h"
#include "../world/Location.hpp"
#include "../world/MapAnimation.h"
#include "Network.h"

#include <cassert>
#include <iterator>
#include <stdexcept>

// This string specifies which version of network stream current build uses.
// It is used for making sure only compatible builds get connected, even within
// single OpenRCT2 version.

constexpr uint8_t kStreamVersion = 2;

const std::string kStreamID = std::string(kOpenRCT2Version) + "-" + std::to_string(kStreamVersion);

static Peep* _pickup_peep = nullptr;
static int32_t _pickup_peep_old_x = kLocationNull;

#ifndef DISABLE_NETWORK

// General chunk size is 63 KiB, this can not be any larger because the packet size is encoded
// with uint16_t and needs some spare room for other data in the packet.
static constexpr uint32_t kChunkSize = 1024 * 63;

// If data is sent fast enough it would halt the entire server, process only a maximum amount.
// This limit is per connection, the current value was determined by tests with fuzzing.
static constexpr uint32_t kMaxPacketsPerTick = 100;

    #include "../Cheats.h"
    #include "../ParkImporter.h"
    #include "../Version.h"
    #include "../actions/GameAction.h"
    #include "../config/Config.h"
    #include "../core/Console.hpp"
    #include "../core/EnumUtils.hpp"
    #include "../core/FileStream.h"
    #include "../core/MemoryStream.h"
    #include "../core/Path.hpp"
    #include "../core/String.hpp"
    #include "../interface/Chat.h"
    #include "../localisation/Localisation.Date.h"
    #include "../object/ObjectManager.h"
    #include "../object/ObjectRepository.h"
    #include "../world/Park.h"
    #include "NetworkAction.h"
    #include "NetworkConnection.h"
    #include "NetworkGroup.h"
    #include "NetworkKey.h"
    #include "NetworkPacket.h"
    #include "NetworkPlayer.h"
    #include "NetworkServerAdvertiser.h"
    #include "NetworkUser.h"
    #include "Socket.h"

    #include <array>
    #include <cerrno>
    #include <cmath>
    #include <fstream>
    #include <functional>
    #include <list>
    #include <map>
    #include <memory>
    #include <set>
    #include <string>
    #include <vector>

namespace OpenRCT2::Network
{
    static void ChatShowConnectedMessage();
    static void ChatShowServerGreeting();
    static u8string GetKeysDirectory();
    static u8string GetPrivateKeyPath(u8string_view playerName);
    static u8string GetPublicKeyPath(u8string_view playerName, u8string_view hash);

    NetworkBase::NetworkBase(IContext& context)
        : System(context)
    {
        mode = Mode::none;
        status = Status::none;
        last_ping_sent_time = 0;
        _actionId = 0;

        client_command_handlers[Command::auth] = &NetworkBase::Client_Handle_AUTH;
        client_command_handlers[Command::beginMap] = &NetworkBase::Client_Handle_BEGINMAP;
        client_command_handlers[Command::map] = &NetworkBase::Client_Handle_MAP;
        client_command_handlers[Command::chat] = &NetworkBase::Client_Handle_CHAT;
        client_command_handlers[Command::gameAction] = &NetworkBase::Client_Handle_GAME_ACTION;
        client_command_handlers[Command::tick] = &NetworkBase::Client_Handle_TICK;
        client_command_handlers[Command::playerList] = &NetworkBase::Client_Handle_PLAYERLIST;
        client_command_handlers[Command::playerInfo] = &NetworkBase::Client_Handle_PLAYERINFO;
        client_command_handlers[Command::ping] = &NetworkBase::Client_Handle_PING;
        client_command_handlers[Command::pingList] = &NetworkBase::Client_Handle_PINGLIST;
        client_command_handlers[Command::disconnectMessage] = &NetworkBase::Client_Handle_SETDISCONNECTMSG;
        client_command_handlers[Command::showError] = &NetworkBase::Client_Handle_SHOWERROR;
        client_command_handlers[Command::groupList] = &NetworkBase::Client_Handle_GROUPLIST;
        client_command_handlers[Command::event] = &NetworkBase::Client_Handle_EVENT;
        client_command_handlers[Command::gameInfo] = &NetworkBase::Client_Handle_GAMEINFO;
        client_command_handlers[Command::token] = &NetworkBase::Client_Handle_TOKEN;
        client_command_handlers[Command::objectsList] = &NetworkBase::Client_Handle_OBJECTS_LIST;
        client_command_handlers[Command::scriptsData] = &NetworkBase::Client_Handle_SCRIPTS_DATA;
        client_command_handlers[Command::gameState] = &NetworkBase::Client_Handle_GAMESTATE;

        server_command_handlers[Command::auth] = &NetworkBase::ServerHandleAuth;
        server_command_handlers[Command::chat] = &NetworkBase::ServerHandleChat;
        server_command_handlers[Command::gameAction] = &NetworkBase::ServerHandleGameAction;
        server_command_handlers[Command::ping] = &NetworkBase::ServerHandlePing;
        server_command_handlers[Command::gameInfo] = &NetworkBase::ServerHandleGameInfo;
        server_command_handlers[Command::token] = &NetworkBase::ServerHandleToken;
        server_command_handlers[Command::mapRequest] = &NetworkBase::ServerHandleMapRequest;
        server_command_handlers[Command::requestGameState] = &NetworkBase::ServerHandleRequestGamestate;
        server_command_handlers[Command::heartbeat] = &NetworkBase::ServerHandleHeartbeat;

        _chat_log_fs << std::unitbuf;
        _server_log_fs << std::unitbuf;
    }

    bool NetworkBase::Init()
    {
        status = Status::ready;

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
        if (status != Status::none)
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
        if (status != Status::none)
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

    #ifdef ENABLE_SCRIPTING
            auto& scriptEngine = GetContext().GetScriptEngine();
            scriptEngine.RemoveNetworkPlugins();
    #endif

            GfxInvalidateScreen();

            _requireClose = false;
        }
    }

    void NetworkBase::DecayCooldown(Player* player)
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
        if (mode == Mode::client)
        {
            _serverConnection.reset();
        }
        else if (mode == Mode::server)
        {
            _listenSocket.reset();
            _advertiser.reset();
        }

        mode = Mode::none;
        status = Status::none;
        _lastConnectStatus = SocketStatus::closed;
    }

    bool NetworkBase::BeginClient(const std::string& host, uint16_t port)
    {
        if (GetMode() != Mode::none)
        {
            return false;
        }

        Close();
        if (!Init())
            return false;

        mode = Mode::client;

        LOG_INFO("Connecting to %s:%u", host.c_str(), port);
        _host = host;
        _port = port;

        _serverConnection = std::make_unique<Connection>();
        _serverConnection->Socket = CreateTcpSocket();
        _serverConnection->Socket->ConnectAsync(host, port);
        _serverState.gamestateSnapshotsEnabled = false;

        status = Status::connecting;
        _lastConnectStatus = SocketStatus::closed;
        _clientMapLoaded = false;
        _serverTickData.clear();

        BeginChatLog();
        BeginServerLog();

        // We need to wait for the map load before we execute any actions.
        // If the client has the title screen running then there's a potential
        // risk of tick collision with the server map and title screen map.
        GameActions::SuspendQueue();

        auto keyPath = GetPrivateKeyPath(Config::Get().network.playerName);
        if (!File::Exists(keyPath))
        {
            Console::WriteLine("Generating key... This may take a while");
            Console::WriteLine("Need to collect enough entropy from the system");
            _key.Generate();
            Console::WriteLine("Key generated, saving private bits as %s", keyPath.c_str());

            const auto keysDirectory = GetKeysDirectory();
            if (!Path::CreateDirectory(keysDirectory))
            {
                LOG_ERROR("Unable to create directory %s.", keysDirectory.c_str());
                return false;
            }

            try
            {
                auto fs = FileStream(keyPath, FileMode::write);
                _key.SavePrivate(&fs);
            }
            catch (const std::exception&)
            {
                LOG_ERROR("Unable to save private key at %s.", keyPath.c_str());
                return false;
            }

            const std::string hash = _key.PublicKeyHash();
            const utf8* publicKeyHash = hash.c_str();
            keyPath = GetPublicKeyPath(Config::Get().network.playerName, publicKeyHash);
            Console::WriteLine("Key generated, saving public bits as %s", keyPath.c_str());

            try
            {
                auto fs = FileStream(keyPath, FileMode::write);
                _key.SavePublic(&fs);
            }
            catch (const std::exception&)
            {
                LOG_ERROR("Unable to save public key at %s.", keyPath.c_str());
                return false;
            }
        }
        else
        {
            // LoadPrivate returns validity of loaded key
            bool ok = false;
            try
            {
                LOG_VERBOSE("Loading key from %s", keyPath.c_str());
                auto fs = FileStream(keyPath, FileMode::open);
                ok = _key.LoadPrivate(&fs);
            }
            catch (const std::exception&)
            {
                LOG_ERROR("Unable to read private key from %s.", keyPath.c_str());
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

        mode = Mode::server;

        _userManager.Load();

        LOG_VERBOSE("Begin listening for clients");

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

        ServerName = Config::Get().network.serverName;
        ServerDescription = Config::Get().network.serverDescription;
        ServerGreeting = Config::Get().network.serverGreeting;
        ServerProviderName = Config::Get().network.providerName;
        ServerProviderEmail = Config::Get().network.providerEmail;
        ServerProviderWebsite = Config::Get().network.providerWebsite;

        IsServerPlayerInvisible = gOpenRCT2Headless;

        LoadGroups();
        BeginChatLog();
        BeginServerLog();

        Player* player = AddPlayer(Config::Get().network.playerName, "");
        player->Flags |= PlayerFlags::kIsServer;
        player->Group = 0;
        player_id = player->Id;

        if (GetMode() == Mode::server)
        {
            // Add SERVER to users.json and save.
            User* networkUser = _userManager.GetOrAddUser(player->KeyHash);
            networkUser->GroupId = player->Group;
            networkUser->Name = player->Name;
            _userManager.Save();
        }

        auto* szAddress = address.empty() ? "*" : address.c_str();
        Console::WriteLine("Listening for clients on %s:%hu", szAddress, port);
        ChatShowConnectedMessage();
        ChatShowServerGreeting();

        status = Status::connected;
        listening_port = port;
        _serverState.gamestateSnapshotsEnabled = Config::Get().network.desyncDebugging;
        _advertiser = CreateServerAdvertiser(listening_port);

        GameLoadScripts();
        GameNotifyMapChanged();

        return true;
    }

    Mode NetworkBase::GetMode() const noexcept
    {
        return mode;
    }

    Status NetworkBase::GetStatus() const noexcept
    {
        return status;
    }

    Auth NetworkBase::GetAuthStatus()
    {
        if (GetMode() == Mode::client)
        {
            return _serverConnection->AuthStatus;
        }
        if (GetMode() == Mode::server)
        {
            return Auth::ok;
        }
        return Auth::none;
    }

    uint32_t NetworkBase::GetServerTick() const noexcept
    {
        return _serverState.tick;
    }

    PlayerId_t NetworkBase::GetPlayerID() const noexcept
    {
        return player_id;
    }

    Connection* NetworkBase::GetPlayerConnection(uint8_t id) const
    {
        auto player = GetPlayerByID(id);
        if (player != nullptr)
        {
            auto clientIt = std::find_if(
                client_connection_list.begin(), client_connection_list.end(),
                [player](const auto& conn) -> bool { return conn->player == player; });
            return clientIt != client_connection_list.end() ? clientIt->get() : nullptr;
        }
        return nullptr;
    }

    void NetworkBase::Update()
    {
        switch (GetMode())
        {
            case Mode::server:
                UpdateServer();
                break;
            case Mode::client:
                UpdateClient();
                break;
            default:
                break;
        }
    }

    void NetworkBase::Tick()
    {
        _closeLock = true;

        // Update is not necessarily called per game tick, maintain our own delta time
        uint32_t ticks = Platform::GetTicks();
        _currentDeltaTime = std::max<uint32_t>(ticks - _lastUpdateTime, 1);
        _lastUpdateTime = ticks;

        switch (GetMode())
        {
            case Mode::server:
                TickServer();
                break;
            case Mode::client:
                TickClient();
                break;
            default:
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
        if (GetMode() == Mode::client)
        {
            _serverConnection->SendQueuedData();
        }
        else
        {
            for (auto& it : client_connection_list)
            {
                it->SendQueuedData();
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

            connection->update();
        }
    }

    void NetworkBase::TickServer()
    {
        for (auto& connection : client_connection_list)
        {
            // This can be called multiple times before the connection is removed.
            if (!connection->IsValid())
                continue;

            if (!ProcessConnection(*connection))
            {
                if (connection->player != nullptr)
                    LOG_INFO("Disconnecting player %s", connection->player->Name.c_str());
                else
                    LOG_INFO("Disconnecting unknown player");
                connection->Disconnect();
            }
            else
            {
                DecayCooldown(connection->player);
            }
        }

        uint32_t ticks = Platform::GetTicks();
        if (ticks > last_ping_sent_time + 3000)
        {
            ServerSendPing();
            ServerSendPingList();
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
        _serverConnection->update();
    }

    void NetworkBase::TickClient()
    {
        assert(_serverConnection != nullptr);

        switch (status)
        {
            case Status::connecting:
            {
                switch (_serverConnection->Socket->GetStatus())
                {
                    case SocketStatus::resolving:
                    {
                        if (_lastConnectStatus != SocketStatus::resolving)
                        {
                            _lastConnectStatus = SocketStatus::resolving;
                            char str_resolving[256];
                            FormatStringLegacy(str_resolving, 256, STR_MULTIPLAYER_RESOLVING, nullptr);

                            auto intent = Intent(WindowClass::networkStatus);
                            intent.PutExtra(INTENT_EXTRA_MESSAGE, std::string{ str_resolving });
                            intent.PutExtra(
                                INTENT_EXTRA_CALLBACK, []() -> void { OpenRCT2::GetContext()->GetNetwork().Close(); });
                            ContextOpenIntent(&intent);
                        }
                        break;
                    }
                    case SocketStatus::connecting:
                    {
                        if (_lastConnectStatus != SocketStatus::connecting)
                        {
                            _lastConnectStatus = SocketStatus::connecting;
                            char str_connecting[256];
                            FormatStringLegacy(str_connecting, 256, STR_MULTIPLAYER_CONNECTING, nullptr);

                            auto intent = Intent(WindowClass::networkStatus);
                            intent.PutExtra(INTENT_EXTRA_MESSAGE, std::string{ str_connecting });
                            intent.PutExtra(
                                INTENT_EXTRA_CALLBACK, []() -> void { OpenRCT2::GetContext()->GetNetwork().Close(); });
                            ContextOpenIntent(&intent);

                            server_connect_time = Platform::GetTicks();
                        }
                        break;
                    }
                    case SocketStatus::connected:
                    {
                        status = Status::connected;
                        Client_Send_TOKEN();
                        char str_authenticating[256];
                        FormatStringLegacy(str_authenticating, 256, STR_MULTIPLAYER_AUTHENTICATING, nullptr);

                        auto intent = Intent(WindowClass::networkStatus);
                        intent.PutExtra(INTENT_EXTRA_MESSAGE, std::string{ str_authenticating });
                        intent.PutExtra(INTENT_EXTRA_CALLBACK, []() -> void { OpenRCT2::GetContext()->GetNetwork().Close(); });
                        ContextOpenIntent(&intent);
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
                        ContextForceCloseWindowByClass(WindowClass::networkStatus);
                        ContextShowError(STR_UNABLE_TO_CONNECT_TO_SERVER, kStringIdNone, {});
                        break;
                    }
                }
                break;
            }
            case Status::connected:
            {
                if (!ProcessConnection(*_serverConnection))
                {
                    // Do not show disconnect message window when password window closed/canceled
                    if (_serverConnection->AuthStatus == Auth::requirePassword)
                    {
                        ContextForceCloseWindowByClass(WindowClass::networkStatus);
                    }
                    else
                    {
                        char str_disconnected[256];

                        if (_serverConnection->GetLastDisconnectReason())
                        {
                            const char* disconnect_reason = _serverConnection->GetLastDisconnectReason();
                            FormatStringLegacy(
                                str_disconnected, 256, STR_MULTIPLAYER_DISCONNECTED_WITH_REASON, &disconnect_reason);
                        }
                        else
                        {
                            FormatStringLegacy(str_disconnected, 256, STR_MULTIPLAYER_DISCONNECTED_NO_REASON, nullptr);
                        }

                        auto intent = Intent(WindowClass::networkStatus);
                        intent.PutExtra(INTENT_EXTRA_MESSAGE, std::string{ str_disconnected });
                        ContextOpenIntent(&intent);
                    }

                    auto* windowMgr = Ui::GetWindowManager();
                    windowMgr->CloseByClass(WindowClass::multiplayer);
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

            default:
                break;
        }
    }

    auto NetworkBase::GetPlayerIteratorByID(uint8_t id) const
    {
        return std::find_if(
            player_list.begin(), player_list.end(), [id](std::unique_ptr<Player> const& player) { return player->Id == id; });
    }

    Player* NetworkBase::GetPlayerByID(uint8_t id) const
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

    int32_t NetworkBase::GetTotalNumPlayers() const noexcept
    {
        return static_cast<int32_t>(player_list.size());
    }

    int32_t NetworkBase::GetNumVisiblePlayers() const noexcept
    {
        if (IsServerPlayerInvisible)
            return static_cast<int32_t>(player_list.size() - 1);
        return static_cast<int32_t>(player_list.size());
    }

    const char* NetworkBase::FormatChat(Player* fromPlayer, const char* text)
    {
        static std::string formatted;
        formatted.clear();

        if (fromPlayer != nullptr)
        {
            auto& network = OpenRCT2::GetContext()->GetNetwork();
            auto it = network.GetGroupByID(fromPlayer->Id);
            std::string groupName = "";
            std::vector<std::string> colours;
            if (it != nullptr)
            {
                groupName = it->GetName();
                if (groupName[0] != '{')
                {
                    colours.push_back("{WHITE}");
                }
            }

            for (size_t i = 0; i < groupName.size(); ++i)
            {
                if (groupName[i] == '{')
                {
                    std::string colour = "{";
                    ++i;
                    while (i < groupName.size() && groupName[i] != '}' && groupName[i] != '{')
                    {
                        colour += groupName[i];
                        ++i;
                    }
                    colour += '}';
                    if (groupName[i] == '}' && i < groupName.size())
                    {
                        colours.push_back(colour);
                    }
                }
            }

            if (colours.size() == 0 || (colours.size() == 1 && colours[0] == "{WHITE}"))
            {
                formatted += "{BABYBLUE}";
                formatted += fromPlayer->Name;
            }
            else
            {
                size_t j = 0;
                size_t proportionalSize = fromPlayer->Name.size() / colours.size();
                for (size_t i = 0; i < colours.size(); ++i)
                {
                    formatted += colours[i];
                    size_t numCharacters = proportionalSize + j;
                    for (; j < numCharacters && j < fromPlayer->Name.size(); ++j)
                    {
                        formatted += fromPlayer->Name[j];
                    }
                }
                while (j < fromPlayer->Name.size())
                {
                    formatted += fromPlayer->Name[j];
                    j++;
                }
            }

            formatted += ": ";
        }
        formatted += "{WHITE}";
        formatted += text;
        return formatted.c_str();
    }

    void NetworkBase::SendPacketToClients(const Packet& packet, bool front, bool gameCmd) const
    {
        for (auto& client_connection : client_connection_list)
        {
            if (gameCmd)
            {
                // If marked as game command we can not send the packet to connections that are not fully connected.
                // Sending the packet would cause the client to store a command that is behind the tick where he starts,
                // which would be essentially never executed. The clients do not require commands before the server has not sent
                // the map data.
                if (client_connection->player == nullptr)
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

        const ServerTickData storedTick = itTickData->second;
        _serverTickData.erase(itTickData);

        if (storedTick.srand0 != srand0)
        {
            LOG_INFO("Srand0 mismatch, client = %08X, server = %08X", srand0, storedTick.srand0);
            return false;
        }

        if (!storedTick.spriteHash.empty())
        {
            EntitiesChecksum checksum = getGameState().entities.GetAllEntitiesChecksum();
            std::string clientSpriteHash = checksum.ToString();
            if (clientSpriteHash != storedTick.spriteHash)
            {
                LOG_INFO(
                    "Sprite hash mismatch, client = %s, server = %s", clientSpriteHash.c_str(), storedTick.spriteHash.c_str());
                return false;
            }
        }

        return true;
    }

    bool NetworkBase::IsDesynchronised() const noexcept
    {
        return _serverState.state == ServerStatus::desynced;
    }

    bool NetworkBase::CheckDesynchronizaton()
    {
        const auto currentTicks = getGameState().currentTicks;

        // Check synchronisation
        if (GetMode() == Mode::client && _serverState.state != ServerStatus::desynced
            && !CheckSRAND(currentTicks, ScenarioRandState().s0))
        {
            _serverState.state = ServerStatus::desynced;
            _serverState.desyncTick = currentTicks;

            char str_desync[256];
            FormatStringLegacy(str_desync, 256, STR_MULTIPLAYER_DESYNC, nullptr);

            auto intent = Intent(WindowClass::networkStatus);
            intent.PutExtra(INTENT_EXTRA_MESSAGE, std::string{ str_desync });
            ContextOpenIntent(&intent);

            if (!Config::Get().network.stayConnected)
            {
                Close();
            }

            return true;
        }

        return false;
    }

    void NetworkBase::RequestStateSnapshot()
    {
        LOG_INFO("Requesting game state for tick %u", _serverState.desyncTick);

        Client_Send_RequestGameState(_serverState.desyncTick);
    }

    ServerState NetworkBase::GetServerState() const noexcept
    {
        return _serverState;
    }

    void NetworkBase::KickPlayer(int32_t playerId)
    {
        for (auto& client_connection : client_connection_list)
        {
            if (client_connection->player->Id == playerId)
            {
                // Disconnect the client gracefully
                client_connection->SetLastDisconnectReason(STR_MULTIPLAYER_KICKED);
                char str_disconnect_msg[256];
                FormatStringLegacy(str_disconnect_msg, 256, STR_MULTIPLAYER_KICKED_REASON, nullptr);
                ServerSendSetDisconnectMsg(*client_connection, str_disconnect_msg);
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
        if (GetMode() == Mode::client)
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
            int32_t hexCharIndex = UtilRand() % std::size(hexChars);
            key[i] = hexChars[hexCharIndex];
        }
        key[std::size(key) - 1] = 0;

        return key;
    }

    std::string NetworkBase::GetMasterServerUrl()
    {
        if (Config::Get().network.masterServerUrl.empty())
        {
            return kMasterServerURL;
        }

        return Config::Get().network.masterServerUrl;
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

        if (GetMode() == Mode::server)
        {
            _userManager.UnsetUsersOfGroup(id);
            _userManager.Save();
        }
    }

    uint8_t NetworkBase::GetGroupIDByHash(const std::string& keyhash)
    {
        const User* networkUser = _userManager.GetUserByHash(keyhash);

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
                LOG_WARNING(
                    "User %s is assigned to non-existent group %u. Assigning to default group (%u)", keyhash.c_str(),
                    assignedGroup, groupId);
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
        if (GetMode() == Mode::server)
        {
            auto& env = GetContext().GetPlatformEnvironment();
            auto path = Path::Combine(env.GetDirectoryPath(DirBase::user), u8"groups.json");

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
                LOG_ERROR("Unable to save %s: %s", path.c_str(), ex.what());
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
        spectator->ToggleActionPermission(Permission::chat);
        spectator->Id = 1;
        group_list.push_back(std::move(spectator));

        // User group
        auto user = std::make_unique<NetworkGroup>();
        user->SetName("User");
        user->ActionsAllowed.fill(0xFF);
        user->ToggleActionPermission(Permission::kickPlayer);
        user->ToggleActionPermission(Permission::modifyGroups);
        user->ToggleActionPermission(Permission::setPlayerGroup);
        user->ToggleActionPermission(Permission::cheat);
        user->ToggleActionPermission(Permission::passwordlessLogin);
        user->ToggleActionPermission(Permission::modifyTile);
        user->ToggleActionPermission(Permission::editScenarioOptions);
        user->Id = 2;
        group_list.push_back(std::move(user));

        SetDefaultGroup(1);
    }

    void NetworkBase::LoadGroups()
    {
        group_list.clear();

        auto& env = GetContext().GetPlatformEnvironment();
        auto path = Path::Combine(env.GetDirectoryPath(DirBase::user), u8"groups.json");

        json_t jsonGroupConfig;
        if (File::Exists(path))
        {
            try
            {
                jsonGroupConfig = Json::ReadFromFile(path);
            }
            catch (const std::exception& e)
            {
                LOG_ERROR("Failed to read %s as JSON. Setting default groups. %s", path.c_str(), e.what());
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

    std::string NetworkBase::BeginLog(
        const std::string& directory, const std::string& midName, const std::string& filenameFormat)
    {
        utf8 filename[256];
        time_t timer;
        time(&timer);
        auto tmInfo = localtime(&timer);
        if (strftime(filename, sizeof(filename), filenameFormat.c_str(), tmInfo) == 0)
        {
            throw std::runtime_error("strftime failed");
        }

        auto directoryMidName = Path::Combine(directory, midName);
        Path::CreateDirectory(directoryMidName);
        return Path::Combine(directoryMidName, filename);
    }

    void NetworkBase::AppendLog(std::ostream& fs, std::string_view s)
    {
        if (fs.fail())
        {
            LOG_ERROR("bad ostream failed to append log");
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
                String::append(buffer, sizeof(buffer), std::string(s).c_str());
                String::append(buffer, sizeof(buffer), PLATFORM_NEWLINE);

                fs.write(buffer, strlen(buffer));
            }
        }
        catch (const std::exception& ex)
        {
            LOG_ERROR("%s", ex.what());
        }
    }

    void NetworkBase::BeginChatLog()
    {
        auto& env = GetContext().GetPlatformEnvironment();
        auto directory = env.GetDirectoryPath(DirBase::user, DirId::chatLogs);
        _chatLogPath = BeginLog(directory, "", _chatLogFilenameFormat);
        _chat_log_fs.open(fs::u8path(_chatLogPath), std::ios::out | std::ios::app);
    }

    void NetworkBase::AppendChatLog(std::string_view s)
    {
        if (Config::Get().network.logChat && _chat_log_fs.is_open())
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
        auto& env = GetContext().GetPlatformEnvironment();
        auto directory = env.GetDirectoryPath(DirBase::user, DirId::serverLogs);
        _serverLogPath = BeginLog(directory, ServerName, _serverLogFilenameFormat);
        _server_log_fs.open(fs::u8path(_serverLogPath), std::ios::out | std::ios::app | std::ios::binary);

        // Log server start event
        utf8 logMessage[256];
        if (GetMode() == Mode::client)
        {
            FormatStringLegacy(logMessage, sizeof(logMessage), STR_LOG_CLIENT_STARTED, nullptr);
        }
        else if (GetMode() == Mode::server)
        {
            FormatStringLegacy(logMessage, sizeof(logMessage), STR_LOG_SERVER_STARTED, nullptr);
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
        if (Config::Get().network.logServerActions && _server_log_fs.is_open())
        {
            AppendLog(_server_log_fs, s);
        }
    }

    void NetworkBase::CloseServerLog()
    {
        // Log server stopped event
        char logMessage[256];
        if (GetMode() == Mode::client)
        {
            FormatStringLegacy(logMessage, sizeof(logMessage), STR_LOG_CLIENT_STOPPED, nullptr);
        }
        else if (GetMode() == Mode::server)
        {
            FormatStringLegacy(logMessage, sizeof(logMessage), STR_LOG_SERVER_STOPPED, nullptr);
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
            LOG_VERBOSE("Server does not store a gamestate history");
            return;
        }

        LOG_VERBOSE("Requesting gamestate from server for tick %u", tick);

        Packet packet(Command::requestGameState);
        packet << tick;
        _serverConnection->QueuePacket(std::move(packet));
    }

    void NetworkBase::Client_Send_TOKEN()
    {
        LOG_VERBOSE("requesting token");
        Packet packet(Command::token);
        _serverConnection->AuthStatus = Auth::requested;
        _serverConnection->QueuePacket(std::move(packet));
    }

    void NetworkBase::Client_Send_AUTH(
        const std::string& name, const std::string& password, const std::string& pubkey, const std::vector<uint8_t>& signature)
    {
        Packet packet(Command::auth);
        packet.WriteString(GetVersion());
        packet.WriteString(name);
        packet.WriteString(password);
        packet.WriteString(pubkey);
        assert(signature.size() <= static_cast<size_t>(UINT32_MAX));
        packet << static_cast<uint32_t>(signature.size());
        packet.Write(signature.data(), signature.size());
        _serverConnection->AuthStatus = Auth::requested;
        _serverConnection->QueuePacket(std::move(packet));
    }

    void NetworkBase::Client_Send_MAPREQUEST(const std::vector<ObjectEntryDescriptor>& objects)
    {
        LOG_VERBOSE("client requests %u objects", uint32_t(objects.size()));
        Packet packet(Command::mapRequest);
        packet << static_cast<uint32_t>(objects.size());
        for (const auto& object : objects)
        {
            std::string name(object.GetName());
            LOG_VERBOSE("client requests object %s", name.c_str());
            if (object.Generation == ObjectGeneration::DAT)
            {
                packet << static_cast<uint8_t>(0);
                packet.Write(&object.Entry, sizeof(RCTObjectEntry));
            }
            else
            {
                packet << static_cast<uint8_t>(1);
                packet.WriteString(name);
            }
        }
        _serverConnection->QueuePacket(std::move(packet));
    }

    void NetworkBase::ServerSendToken(Connection& connection)
    {
        Packet packet(Command::token);
        packet << static_cast<uint32_t>(connection.Challenge.size());
        packet.Write(connection.Challenge.data(), connection.Challenge.size());
        connection.QueuePacket(std::move(packet));
    }

    void NetworkBase::ServerSendObjectsList(
        Connection& connection, const std::vector<const ObjectRepositoryItem*>& objects) const
    {
        LOG_VERBOSE("Server sends objects list with %u items", objects.size());

        Packet packet(Command::objectsList);

        // Count.
        packet << static_cast<uint32_t>(objects.size());

        // List
        for (size_t i = 0; i < objects.size(); ++i)
        {
            const auto* object = objects[i];

            if (object->Identifier.empty())
            {
                // DAT
                LOG_VERBOSE("Object %.8s (checksum %x)", object->ObjectEntry.name, object->ObjectEntry.checksum);
                packet << static_cast<uint8_t>(0);
                packet.Write(&object->ObjectEntry, sizeof(RCTObjectEntry));
            }
            else
            {
                // JSON
                LOG_VERBOSE("Object %s", object->Identifier.c_str());
                packet << static_cast<uint8_t>(1);
                packet.WriteString(object->Identifier);
            }
        }

        connection.QueuePacket(std::move(packet));
    }

    void NetworkBase::ServerSendScripts(Connection& connection)
    {
        Packet packet(Command::scriptsData);

    #ifdef ENABLE_SCRIPTING
        using namespace OpenRCT2::Scripting;

        auto& scriptEngine = GetContext().GetScriptEngine();

        // Get remote plugin list.
        const auto remotePlugins = scriptEngine.GetRemotePlugins();
        LOG_VERBOSE("Server sends %zu scripts", remotePlugins.size());

        packet << static_cast<uint32_t>(remotePlugins.size());

        for (auto& plugin : remotePlugins)
        {
            const auto& code = plugin->GetCode();
            const auto codeSize = static_cast<uint32_t>(code.size());

            packet << codeSize;
            packet.Write(code.c_str(), code.size());
        }
    #else
        packet << static_cast<uint32_t>(0);
    #endif

        connection.QueuePacket(std::move(packet));
    }

    void NetworkBase::Client_Send_HEARTBEAT(Connection& connection) const
    {
        LOG_VERBOSE("Sending heartbeat");

        Packet packet(Command::heartbeat);
        connection.QueuePacket(std::move(packet));
    }

    Stats NetworkBase::GetStats() const
    {
        Stats stats = {};
        if (mode == Mode::client)
        {
            stats = _serverConnection->stats;
        }
        else
        {
            for (auto& connection : client_connection_list)
            {
                for (size_t n = 0; n < EnumValue(StatisticsGroup::Max); n++)
                {
                    stats.bytesReceived[n] += connection->stats.bytesReceived[n];
                    stats.bytesSent[n] += connection->stats.bytesSent[n];
                }
            }
        }
        return stats;
    }

    void NetworkBase::ServerSendAuth(Connection& connection)
    {
        uint8_t new_playerid = 0;
        if (connection.player != nullptr)
        {
            new_playerid = connection.player->Id;
        }
        Packet packet(Command::auth);
        packet << static_cast<uint32_t>(connection.AuthStatus) << new_playerid;
        if (connection.AuthStatus == Auth::badVersion)
        {
            packet.WriteString(GetVersion());
        }
        connection.QueuePacket(std::move(packet));
        if (connection.AuthStatus != Auth::ok && connection.AuthStatus != Auth::requirePassword)
        {
            connection.Disconnect();
        }
    }

    void NetworkBase::ServerSendMap(Connection* connection)
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

        auto mapContent = SaveForNetwork(objects);
        if (mapContent.empty())
        {
            if (connection != nullptr)
            {
                connection->SetLastDisconnectReason(STR_MULTIPLAYER_CONNECTION_CLOSED);
                connection->Disconnect();
            }
            return;
        }

        Packet packetBeginMap(Command::beginMap);

        Packet packetMap(Command::map);
        packetMap.Write(mapContent.data(), mapContent.size());

        if (connection != nullptr)
        {
            connection->QueuePacket(std::move(packetBeginMap));
            connection->QueuePacket(std::move(packetMap));
        }
        else
        {
            SendPacketToClients(packetBeginMap);
            SendPacketToClients(packetMap);
        }
    }

    std::vector<uint8_t> NetworkBase::SaveForNetwork(const std::vector<const ObjectRepositoryItem*>& objects) const
    {
        std::vector<uint8_t> result;
        auto ms = MemoryStream();
        if (SaveMap(&ms, objects))
        {
            result.resize(ms.GetLength());
            std::memcpy(result.data(), ms.GetData(), result.size());
        }
        else
        {
            LOG_WARNING("Failed to export map.");
        }
        return result;
    }

    void NetworkBase::Client_Send_CHAT(const char* text)
    {
        Packet packet(Command::chat);
        packet.WriteString(text);
        _serverConnection->QueuePacket(std::move(packet));
    }

    void NetworkBase::ServerSendChat(const char* text, const std::vector<uint8_t>& playerIds)
    {
        Packet packet(Command::chat);
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

    void NetworkBase::Client_Send_GAME_ACTION(const GameActions::GameAction* action)
    {
        Packet packet(Command::gameAction);

        uint32_t networkId = 0;
        networkId = ++_actionId;

        // I know its ugly, want basic functionality for now.
        const_cast<GameActions::GameAction*>(action)->SetNetworkId(networkId);
        if (action->GetCallback())
        {
            _gameActionCallbacks.insert(std::make_pair(networkId, action->GetCallback()));
        }

        DataSerialiser stream(true);
        action->Serialise(stream);

        packet << getGameState().currentTicks << action->GetType() << stream;
        _serverConnection->QueuePacket(std::move(packet));
    }

    void NetworkBase::ServerSendGameAction(const GameActions::GameAction* action)
    {
        Packet packet(Command::gameAction);

        DataSerialiser stream(true);
        action->Serialise(stream);

        packet << getGameState().currentTicks << action->GetType() << stream;

        SendPacketToClients(packet);
    }

    void NetworkBase::ServerSendTick()
    {
        Packet packet(Command::tick);
        packet << getGameState().currentTicks << ScenarioRandState().s0;

        uint32_t flags = 0;

        // Simple counter which limits how often a sprite checksum gets sent.
        // This can get somewhat expensive, so we don't want to push it every tick in release,
        // but debug version can check more often.
        static int32_t checksum_counter = 0;
        checksum_counter++;
        if (checksum_counter >= 100)
        {
            checksum_counter = 0;
            flags |= TickFlags::kChecksums;
        }
        // Send flags always, so we can understand packet structure on the other end,
        // and allow for some expansion.
        packet << flags;
        if (flags & TickFlags::kChecksums)
        {
            EntitiesChecksum checksum = getGameState().entities.GetAllEntitiesChecksum();
            packet.WriteString(checksum.ToString());
        }

        SendPacketToClients(packet);
    }

    void NetworkBase::ServerSendPlayerInfo(int32_t playerId)
    {
        Packet packet(Command::playerInfo);
        packet << getGameState().currentTicks;

        auto* player = GetPlayerByID(playerId);
        if (player == nullptr)
            return;

        player->Write(packet);
        SendPacketToClients(packet);
    }

    void NetworkBase::ServerSendPlayerList()
    {
        Packet packet(Command::playerList);
        packet << getGameState().currentTicks << static_cast<uint8_t>(player_list.size());
        for (auto& player : player_list)
        {
            player->Write(packet);
        }
        SendPacketToClients(packet);
    }

    void NetworkBase::Client_Send_PING()
    {
        Packet packet(Command::ping);
        _serverConnection->QueuePacket(std::move(packet));
    }

    void NetworkBase::ServerSendPing()
    {
        last_ping_sent_time = Platform::GetTicks();
        Packet packet(Command::ping);
        for (auto& client_connection : client_connection_list)
        {
            client_connection->PingTime = Platform::GetTicks();
        }
        SendPacketToClients(packet, true);
    }

    void NetworkBase::ServerSendPingList()
    {
        Packet packet(Command::pingList);
        packet << static_cast<uint8_t>(player_list.size());
        for (auto& player : player_list)
        {
            packet << player->Id << player->Ping;
        }
        SendPacketToClients(packet);
    }

    void NetworkBase::ServerSendSetDisconnectMsg(Connection& connection, const char* msg)
    {
        Packet packet(Command::disconnectMessage);
        packet.WriteString(msg);
        connection.QueuePacket(std::move(packet));
    }

    json_t NetworkBase::GetServerInfoAsJson() const
    {
        json_t jsonObj = {
            { "name", Config::Get().network.serverName },
            { "requiresPassword", _password.size() > 0 },
            { "version", GetVersion() },
            { "players", GetNumVisiblePlayers() },
            { "maxPlayers", Config::Get().network.maxplayers },
            { "description", Config::Get().network.serverDescription },
            { "greeting", Config::Get().network.serverGreeting },
            { "dedicated", gOpenRCT2Headless },
        };
        return jsonObj;
    }

    void NetworkBase::ServerSendGameInfo(Connection& connection)
    {
        Packet packet(Command::gameInfo);
    #ifndef DISABLE_HTTP
        json_t jsonObj = GetServerInfoAsJson();

        // Provider details
        json_t jsonProvider = {
            { "name", Config::Get().network.providerName },
            { "email", Config::Get().network.providerEmail },
            { "website", Config::Get().network.providerWebsite },
        };

        jsonObj["provider"] = jsonProvider;

        packet.WriteString(jsonObj.dump());
        packet << _serverState.gamestateSnapshotsEnabled;
        packet << IsServerPlayerInvisible;

    #endif
        connection.QueuePacket(std::move(packet));
    }

    void NetworkBase::ServerSendShowError(Connection& connection, StringId title, StringId message)
    {
        Packet packet(Command::showError);
        packet << title << message;
        connection.QueuePacket(std::move(packet));
    }

    void NetworkBase::ServerSendGroupList(Connection& connection)
    {
        Packet packet(Command::groupList);
        packet << static_cast<uint8_t>(group_list.size()) << default_group;
        for (auto& group : group_list)
        {
            group->Write(packet);
        }
        connection.QueuePacket(std::move(packet));
    }

    void NetworkBase::ServerSendEventPlayerJoined(const char* playerName)
    {
        Packet packet(Command::event);
        packet << static_cast<ServerEvent>(ServerEvent::playerJoined);
        packet.WriteString(playerName);
        SendPacketToClients(packet);
    }

    void NetworkBase::ServerSendEventPlayerDisconnected(const char* playerName, const char* reason)
    {
        Packet packet(Command::event);
        packet << static_cast<ServerEvent>(ServerEvent::playerDisconnected);
        packet.WriteString(playerName);
        packet.WriteString(reason);
        SendPacketToClients(packet);
    }

    bool NetworkBase::UpdateConnection(Connection& connection)
    {
        connection.update();

        return connection.IsValid();
    }

    static void displayNetworkProgress(StringId captionStringId)
    {
        auto captionString = GetContext()->GetLocalisationService().GetString(captionStringId);
        auto intent = Intent(INTENT_ACTION_PROGRESS_OPEN);
        intent.PutExtra(INTENT_EXTRA_MESSAGE, captionString);
        intent.PutExtra(INTENT_EXTRA_CALLBACK, []() -> void {
            LOG_INFO("User aborted network operation");
            GetContext()->GetNetwork().Close();
        });
        ContextOpenIntent(&intent);
    }

    static void reportPacketProgress(NetworkBase& network, Connection& connection)
    {
        if (network.GetMode() != Mode::client)
        {
            return;
        }

        const auto nextPacketCommand = connection.getPendingPacketCommand();
        const auto bytesReceived = connection.getPendingPacketAvailable();
        const auto bytesTotal = connection.getPendingPacketSize();

        switch (nextPacketCommand)
        {
            case Command::objectsList:
                displayNetworkProgress(STR_MULTIPLAYER_RECEIVING_OBJECTS_LIST);
                break;
            case Command::map:
                displayNetworkProgress(STR_MULTIPLAYER_DOWNLOADING_MAP);
                break;
            case Command::scriptsData:
                displayNetworkProgress(STR_MULTIPLAYER_RECEIVING_SCRIPTS);
                break;
            default:
                // Nothing to report.
                return;
        }

        network.GetContext().SetProgress(
            static_cast<uint32_t>(bytesReceived / 1024), static_cast<uint32_t>(bytesTotal / 1024), STR_STRING_M_OF_N_KIB);
    }

    bool NetworkBase::ProcessConnection(Connection& connection)
    {
        ReadPacket packetStatus;

        uint32_t countProcessed = 0;
        do
        {
            countProcessed++;
            packetStatus = connection.readPacket();
            switch (packetStatus)
            {
                case ReadPacket::disconnected:
                    // closed connection or network error
                    if (!connection.GetLastDisconnectReason())
                    {
                        connection.SetLastDisconnectReason(STR_MULTIPLAYER_CONNECTION_CLOSED);
                    }
                    return false;
                case ReadPacket::success:
                    // done reading in packet
                    reportPacketProgress(*this, connection);
                    ProcessPacket(connection, connection.InboundPacket);
                    if (!connection.IsValid())
                    {
                        return false;
                    }
                    break;
                case ReadPacket::moreData:
                    // more data required to be read
                    reportPacketProgress(*this, connection);
                    break;
                case ReadPacket::noData:
                    // could not read anything from socket
                    break;
            }
        } while (packetStatus == ReadPacket::success && countProcessed < kMaxPacketsPerTick);

        if (!connection.ReceivedDataRecently())
        {
            LOG_INFO(
                "No data received recently from connection %s, disconnecting connection.",
                connection.Socket->GetIpAddress().c_str());

            if (!connection.GetLastDisconnectReason())
            {
                connection.SetLastDisconnectReason(STR_MULTIPLAYER_NO_DATA);
            }
            return false;
        }

        return true;
    }

    void NetworkBase::ProcessPacket(Connection& connection, Packet& packet)
    {
        const auto& handlerList = GetMode() == Mode::server ? server_command_handlers : client_command_handlers;

        auto it = handlerList.find(packet.GetCommand());
        if (it != handlerList.end())
        {
            auto commandHandler = it->second;
            if (connection.AuthStatus == Auth::ok || !packet.CommandRequiresAuth())
            {
                try
                {
                    (this->*commandHandler)(connection, packet);
                }
                catch (const std::exception& ex)
                {
                    LOG_VERBOSE("Exception during packet processing: %s", ex.what());
                }
            }
        }

        packet.Clear();
    }

    // This is called at the end of each game tick, this where things should be processed that affects the game state.
    void NetworkBase::PostTick()
    {
        if (GetMode() == Mode::server)
        {
            ProcessDisconnectedClients();
        }
        else if (GetMode() == Mode::client)
        {
            ProcessPlayerInfo();
        }
        ProcessPlayerList();
    }

    static bool ProcessPlayerAuthenticatePluginHooks(
        const Connection& connection, std::string_view name, std::string_view publicKeyHash)
    {
    #ifdef ENABLE_SCRIPTING
        using namespace OpenRCT2::Scripting;

        auto& hookEngine = GetContext()->GetScriptEngine().GetHookEngine();
        if (hookEngine.HasSubscriptions(HookType::networkAuthenticate))
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
            hookEngine.Call(HookType::networkAuthenticate, e, false);

            // Check if any hook has cancelled the join
            if (AsOrDefault(e["cancel"], false))
            {
                return false;
            }
        }
    #endif
        return true;
    }

    static void ProcessPlayerJoinedPluginHooks(uint8_t playerId)
    {
    #ifdef ENABLE_SCRIPTING
        using namespace OpenRCT2::Scripting;

        auto& hookEngine = GetContext()->GetScriptEngine().GetHookEngine();
        if (hookEngine.HasSubscriptions(HookType::networkJoin))
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();

            // Create event args object
            DukObject eObj(ctx);
            eObj.Set("player", playerId);
            auto e = eObj.Take();

            // Call the subscriptions
            hookEngine.Call(HookType::networkJoin, e, false);
        }
    #endif
    }

    static void ProcessPlayerLeftPluginHooks(uint8_t playerId)
    {
    #ifdef ENABLE_SCRIPTING
        using namespace OpenRCT2::Scripting;

        auto& hookEngine = GetContext()->GetScriptEngine().GetHookEngine();
        if (hookEngine.HasSubscriptions(HookType::networkLeave))
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();

            // Create event args object
            DukObject eObj(ctx);
            eObj.Set("player", playerId);
            auto e = eObj.Take();

            // Call the subscriptions
            hookEngine.Call(HookType::networkLeave, e, false);
        }
    #endif
    }

    void NetworkBase::ProcessPlayerList()
    {
        if (GetMode() == Mode::server)
        {
            // Avoid sending multiple times the player list, we mark the list invalidated on modifications
            // and then send at the end of the tick the final player list.
            if (_playerListInvalidated)
            {
                _playerListInvalidated = false;
                ServerSendPlayerList();
            }
        }
        else
        {
            // As client we have to keep things in order so the update is tick bound.
            // Commands/Actions reference players and so this list needs to be in sync with those.
            auto itPending = _pendingPlayerLists.begin();
            while (itPending != _pendingPlayerLists.end())
            {
                if (itPending->first > getGameState().currentTicks)
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
                            if (player->Flags & PlayerFlags::kIsServer)
                            {
                                _serverConnection->player = player;
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
                        [&removedPlayers](const std::unique_ptr<Player>& player) {
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
        const auto currentTicks = getGameState().currentTicks;

        auto range = _pendingPlayerInfo.equal_range(currentTicks);
        for (auto it = range.first; it != range.second; it++)
        {
            auto* player = GetPlayerByID(it->second.Id);
            if (player != nullptr)
            {
                const Player& networkedInfo = it->second;
                player->Flags = networkedInfo.Flags;
                player->Group = networkedInfo.Group;
                player->LastAction = networkedInfo.LastAction;
                player->LastActionCoord = networkedInfo.LastActionCoord;
                player->MoneySpent = networkedInfo.MoneySpent;
                player->CommandsRan = networkedInfo.CommandsRan;
            }
        }
        _pendingPlayerInfo.erase(currentTicks);
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
            connection->SendQueuedData();
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
        auto connection = std::make_unique<Connection>();
        connection->Socket = std::move(socket);

        client_connection_list.push_back(std::move(connection));
    }

    void NetworkBase::ServerClientDisconnected(std::unique_ptr<Connection>& connection)
    {
        Player* connection_player = connection->player;
        if (connection_player == nullptr)
            return;

        char text[256];
        const char* has_disconnected_args[2] = {
            connection_player->Name.c_str(),
            connection->GetLastDisconnectReason(),
        };
        if (has_disconnected_args[1] != nullptr)
        {
            FormatStringLegacy(text, 256, STR_MULTIPLAYER_PLAYER_HAS_DISCONNECTED_WITH_REASON, has_disconnected_args);
        }
        else
        {
            FormatStringLegacy(text, 256, STR_MULTIPLAYER_PLAYER_HAS_DISCONNECTED_NO_REASON, &(has_disconnected_args[0]));
        }

        ChatAddHistory(text);
        Peep* pickup_peep = GetPickupPeep(connection_player->Id);
        if (pickup_peep != nullptr)
        {
            GameActions::PeepPickupAction pickupAction{ GameActions::PeepPickupType::Cancel,
                                                        pickup_peep->Id,
                                                        { GetPickupPeepOldX(connection_player->Id), 0, 0 },
                                                        GetCurrentPlayerId() };
            auto res = GameActions::Execute(&pickupAction, getGameState());
        }
        ServerSendEventPlayerDisconnected(
            const_cast<char*>(connection_player->Name.c_str()), connection->GetLastDisconnectReason());

        // Log player disconnected event
        AppendServerLog(text);

        ProcessPlayerLeftPluginHooks(connection_player->Id);
    }

    void NetworkBase::RemovePlayer(std::unique_ptr<Connection>& connection)
    {
        Player* connection_player = connection->player;
        if (connection_player == nullptr)
            return;

        player_list.erase(
            std::remove_if(
                player_list.begin(), player_list.end(),
                [connection_player](std::unique_ptr<Player>& player) { return player.get() == connection_player; }),
            player_list.end());

        // Send new player list.
        _playerListInvalidated = true;
    }

    Player* NetworkBase::AddPlayer(const std::string& name, const std::string& keyhash)
    {
        Player* addedplayer = nullptr;
        int32_t newid = -1;
        if (GetMode() == Mode::server)
        {
            // Find first unused player id
            for (int32_t id = 0; id < 255; id++)
            {
                if (std::find_if(
                        player_list.begin(), player_list.end(),
                        [&id](std::unique_ptr<Player> const& player) { return player->Id == id; })
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
            std::unique_ptr<Player> player;
            if (GetMode() == Mode::server)
            {
                // Load keys host may have added manually
                _userManager.Load();

                // Check if the key is registered
                const User* networkUser = _userManager.GetUserByHash(keyhash);

                player = std::make_unique<Player>();
                player->Id = newid;
                player->KeyHash = keyhash;
                if (networkUser == nullptr)
                {
                    player->Group = GetDefaultGroup();
                    if (!name.empty())
                    {
                        player->SetName(MakePlayerNameUnique(String::trim(name)));
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
                player = std::make_unique<Player>();
                player->Id = newid;
                player->Group = GetDefaultGroup();
                player->SetName(String::trim(std::string(name)));
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
                if (String::iequals(player->Name, new_name))
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

    void NetworkBase::Client_Handle_TOKEN(Connection& connection, Packet& packet)
    {
        auto keyPath = GetPrivateKeyPath(Config::Get().network.playerName);
        if (!File::Exists(keyPath))
        {
            LOG_ERROR("Key file (%s) was not found. Restart client to re-generate it.", keyPath.c_str());
            return;
        }

        try
        {
            auto fs = FileStream(keyPath, FileMode::open);
            if (!_key.LoadPrivate(&fs))
            {
                throw std::runtime_error("Failed to load private key.");
            }
        }
        catch (const std::exception&)
        {
            LOG_ERROR("Failed to load key %s", keyPath.c_str());
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
            LOG_ERROR("Failed to sign server's challenge.");
            connection.SetLastDisconnectReason(STR_MULTIPLAYER_VERIFICATION_FAILURE);
            connection.Disconnect();
            return;
        }
        // Don't keep private key in memory. There's no need and it may get leaked
        // when process dump gets collected at some point in future.
        _key.Unload();

        Client_Send_AUTH(Config::Get().network.playerName, gCustomPassword, pubkey, signature);
    }

    void NetworkBase::ServerHandleRequestGamestate(Connection& connection, Packet& packet)
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
                uint32_t dataSize = kChunkSize;
                if (bytesSent + dataSize > snapshotMemory.GetLength())
                {
                    dataSize = snapshotMemory.GetLength() - bytesSent;
                }

                Packet packetGameStateChunk(Command::gameState);
                packetGameStateChunk << tick << length << bytesSent << dataSize;
                packetGameStateChunk.Write(static_cast<const uint8_t*>(snapshotMemory.GetData()) + bytesSent, dataSize);

                connection.QueuePacket(std::move(packetGameStateChunk));

                bytesSent += dataSize;
            }
        }
    }

    void NetworkBase::ServerHandleHeartbeat(Connection& connection, Packet& packet)
    {
        LOG_VERBOSE("Client %s heartbeat", connection.Socket->GetIpAddress().c_str());
    }

    void NetworkBase::Client_Handle_AUTH(Connection& connection, Packet& packet)
    {
        uint32_t auth_status;
        packet >> auth_status >> const_cast<uint8_t&>(player_id);
        connection.AuthStatus = static_cast<Auth>(auth_status);
        switch (connection.AuthStatus)
        {
            case Auth::ok:
                Client_Send_GAMEINFO();
                break;
            case Auth::badName:
                connection.SetLastDisconnectReason(STR_MULTIPLAYER_BAD_PLAYER_NAME);
                connection.Disconnect();
                break;
            case Auth::badVersion:
            {
                auto version = std::string(packet.ReadString());
                auto versionp = version.c_str();
                connection.SetLastDisconnectReason(STR_MULTIPLAYER_INCORRECT_SOFTWARE_VERSION, &versionp);
                connection.Disconnect();
                break;
            }
            case Auth::badPassword:
                connection.SetLastDisconnectReason(STR_MULTIPLAYER_BAD_PASSWORD);
                connection.Disconnect();
                break;
            case Auth::verificationFailure:
                connection.SetLastDisconnectReason(STR_MULTIPLAYER_VERIFICATION_FAILURE);
                connection.Disconnect();
                break;
            case Auth::full:
                connection.SetLastDisconnectReason(STR_MULTIPLAYER_SERVER_FULL);
                connection.Disconnect();
                break;
            case Auth::requirePassword:
                ContextOpenWindowView(WindowView::networkPassword);
                break;
            case Auth::unknownKeyDisallowed:
                connection.SetLastDisconnectReason(STR_MULTIPLAYER_UNKNOWN_KEY_DISALLOWED);
                connection.Disconnect();
                break;
            default:
                connection.SetLastDisconnectReason(STR_MULTIPLAYER_RECEIVED_INVALID_DATA);
                connection.Disconnect();
                break;
        }
    }

    void NetworkBase::ServerClientJoined(std::string_view name, const std::string& keyhash, Connection& connection)
    {
        auto player = AddPlayer(std::string(name), keyhash);
        connection.player = player;
        if (player != nullptr)
        {
            char text[256];
            const char* player_name = static_cast<const char*>(player->Name.c_str());
            FormatStringLegacy(text, 256, STR_MULTIPLAYER_PLAYER_HAS_JOINED_THE_GAME, &player_name);
            ChatAddHistory(text);

            auto& context = GetContext();
            auto& objManager = context.GetObjectManager();
            auto objects = objManager.GetPackableObjects();
            ServerSendObjectsList(connection, objects);
            ServerSendScripts(connection);

            // Log player joining event
            std::string playerNameHash = player->Name + " (" + keyhash + ")";
            player_name = static_cast<const char*>(playerNameHash.c_str());
            FormatStringLegacy(text, 256, STR_MULTIPLAYER_PLAYER_HAS_JOINED_THE_GAME, &player_name);
            AppendServerLog(text);

            ProcessPlayerJoinedPluginHooks(player->Id);
        }
    }

    void NetworkBase::ServerHandleToken(Connection& connection, [[maybe_unused]] Packet& packet)
    {
        uint8_t token_size = 10 + (rand() & 0x7f);
        connection.Challenge.resize(token_size);
        for (int32_t i = 0; i < token_size; i++)
        {
            connection.Challenge[i] = static_cast<uint8_t>(rand() & 0xff);
        }
        ServerSendToken(connection);
    }

    void NetworkBase::Client_Handle_OBJECTS_LIST(Connection& connection, Packet& packet)
    {
        auto& repo = GetContext().GetObjectRepository();

        uint32_t objectCount{};
        packet >> objectCount;

        std::vector<ObjectEntryDescriptor> missingObjects;

        for (uint32_t i = 0; i < objectCount; ++i)
        {
            uint8_t objectType{};
            packet >> objectType;

            if (objectType == 0)
            {
                // DAT
                auto entry = reinterpret_cast<const RCTObjectEntry*>(packet.Read(sizeof(RCTObjectEntry)));
                if (entry != nullptr)
                {
                    const auto* object = repo.FindObject(entry);
                    if (object == nullptr)
                    {
                        auto objectName = std::string(entry->GetName());
                        LOG_VERBOSE("Requesting object %s with checksum %x from server", objectName.c_str(), entry->checksum);
                        missingObjects.push_back(ObjectEntryDescriptor(*entry));
                    }
                    else if (object->ObjectEntry.checksum != entry->checksum || object->ObjectEntry.flags != entry->flags)
                    {
                        auto objectName = std::string(entry->GetName());
                        LOG_WARNING(
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
                        LOG_VERBOSE("Requesting object %s from server", objectName.c_str());
                        missingObjects.push_back(ObjectEntryDescriptor(objectName));
                    }
                }
            }
        }

        LOG_VERBOSE("client received object list, it has %u entries, %zu missing", objectCount, missingObjects.size());
        Client_Send_MAPREQUEST(missingObjects);
    }

    void NetworkBase::Client_Handle_SCRIPTS_DATA(Connection& connection, Packet& packet)
    {
    #ifdef ENABLE_SCRIPTING
        auto& scriptEngine = GetContext().GetScriptEngine();

        uint32_t count{};
        packet >> count;

        for (uint32_t i = 0; i < count; ++i)
        {
            uint32_t codeSize{};
            packet >> codeSize;

            const uint8_t* scriptData = packet.Read(codeSize);

            auto code = std::string_view(reinterpret_cast<const char*>(scriptData), codeSize);
            scriptEngine.AddNetworkPlugin(code);

            LOG_VERBOSE("Received and loaded network script plugin %u/%u", i + 1, count);
        }
    #else
        connection.SetLastDisconnectReason("The client requires plugin support.");
        Close();
    #endif
    }

    void NetworkBase::Client_Handle_GAMESTATE(Connection& connection, Packet& packet)
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

        LOG_VERBOSE(
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
                GameStateCompareData cmpData = snapshots->Compare(serverSnapshot, *desyncSnapshot);

                std::string outputPath = GetContext().GetPlatformEnvironment().GetDirectoryPath(
                    DirBase::user, DirId::desyncLogs);

                Path::CreateDirectory(outputPath);

                char uniqueFileName[128] = {};
                snprintf(
                    uniqueFileName, sizeof(uniqueFileName), "desync_%llu_%u.txt",
                    static_cast<long long unsigned>(Platform::GetDatetimeNowUTC()), tick);

                std::string outputFile = Path::Combine(outputPath, uniqueFileName);

                if (snapshots->LogCompareDataToFile(outputFile, cmpData))
                {
                    LOG_INFO("Wrote desync report to '%s'", outputFile.c_str());

                    auto ft = Formatter();
                    ft.Add<char*>(uniqueFileName);

                    char str_desync[1024];
                    FormatStringLegacy(str_desync, sizeof(str_desync), STR_DESYNC_REPORT, ft.Data());

                    auto intent = Intent(WindowClass::networkStatus);
                    intent.PutExtra(INTENT_EXTRA_MESSAGE, std::string{ str_desync });
                    ContextOpenIntent(&intent);
                }
            }
        }
    }

    void NetworkBase::ServerHandleMapRequest(Connection& connection, Packet& packet)
    {
        uint32_t size;
        packet >> size;
        LOG_VERBOSE("Client requested %u objects", size);
        auto& repo = GetContext().GetObjectRepository();
        for (uint32_t i = 0; i < size; i++)
        {
            uint8_t generation{};
            packet >> generation;

            std::string objectName;
            const ObjectRepositoryItem* item{};
            if (generation == static_cast<uint8_t>(ObjectGeneration::DAT))
            {
                const auto* entry = reinterpret_cast<const RCTObjectEntry*>(packet.Read(sizeof(RCTObjectEntry)));
                objectName = std::string(entry->GetName());
                LOG_VERBOSE("Client requested object %s", objectName.c_str());
                item = repo.FindObject(entry);
            }
            else
            {
                objectName = std::string(packet.ReadString());
                LOG_VERBOSE("Client requested object %s", objectName.c_str());
                item = repo.FindObject(objectName);
            }

            if (item == nullptr)
            {
                LOG_WARNING("Client tried getting non-existent object %s from us.", objectName.c_str());
            }
            else
            {
                connection.RequestedObjects.push_back(item);
            }
        }

        auto player_name = connection.player->Name.c_str();
        ServerSendMap(&connection);
        ServerSendEventPlayerJoined(player_name);
        ServerSendGroupList(connection);
    }

    void NetworkBase::ServerHandleAuth(Connection& connection, Packet& packet)
    {
        if (connection.AuthStatus != Auth::ok)
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
                connection.AuthStatus = Auth::verificationFailure;
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
                    if (!connection.key.LoadPublic(&ms))
                    {
                        throw std::runtime_error("Failed to load public key.");
                    }

                    bool verified = connection.key.Verify(connection.Challenge.data(), connection.Challenge.size(), signature);
                    const std::string hash = connection.key.PublicKeyHash();
                    if (verified)
                    {
                        LOG_VERBOSE("Connection %s: Signature verification ok. Hash %s", hostName, hash.c_str());
                        if (Config::Get().network.knownKeysOnly && _userManager.GetUserByHash(hash) == nullptr)
                        {
                            LOG_VERBOSE("Connection %s: Hash %s, not known", hostName, hash.c_str());
                            connection.AuthStatus = Auth::unknownKeyDisallowed;
                        }
                        else
                        {
                            connection.AuthStatus = Auth::verified;
                        }
                    }
                    else
                    {
                        connection.AuthStatus = Auth::verificationFailure;
                        LOG_VERBOSE("Connection %s: Signature verification failed!", hostName);
                    }
                }
                catch (const std::exception&)
                {
                    connection.AuthStatus = Auth::verificationFailure;
                    LOG_VERBOSE("Connection %s: Signature verification failed, invalid data!", hostName);
                }
            }

            bool passwordless = false;
            if (connection.AuthStatus == Auth::verified)
            {
                const NetworkGroup* group = GetGroupByID(GetGroupIDByHash(connection.key.PublicKeyHash()));
                if (group != nullptr)
                {
                    passwordless = group->CanPerformAction(Permission::passwordlessLogin);
                }
            }
            if (gameversion != GetVersion())
            {
                connection.AuthStatus = Auth::badVersion;
                LOG_INFO("Connection %s: Bad version.", hostName);
            }
            else if (name.empty())
            {
                connection.AuthStatus = Auth::badName;
                LOG_INFO("Connection %s: Bad name.", connection.Socket->GetHostName());
            }
            else if (!passwordless)
            {
                if (password.empty() && !_password.empty())
                {
                    connection.AuthStatus = Auth::requirePassword;
                    LOG_INFO("Connection %s: Requires password.", hostName);
                }
                else if (!password.empty() && _password != password)
                {
                    connection.AuthStatus = Auth::badPassword;
                    LOG_INFO("Connection %s: Bad password.", hostName);
                }
            }

            if (GetNumVisiblePlayers() >= Config::Get().network.maxplayers)
            {
                connection.AuthStatus = Auth::full;
                LOG_INFO("Connection %s: Server is full.", hostName);
            }
            else if (connection.AuthStatus == Auth::verified)
            {
                const std::string hash = connection.key.PublicKeyHash();
                if (ProcessPlayerAuthenticatePluginHooks(connection, name, hash))
                {
                    connection.AuthStatus = Auth::ok;
                    ServerClientJoined(name, hash, connection);
                }
                else
                {
                    connection.AuthStatus = Auth::verificationFailure;
                    LOG_INFO("Connection %s: Denied by plugin.", hostName);
                }
            }

            ServerSendAuth(connection);
        }
    }

    void NetworkBase::Client_Handle_BEGINMAP([[maybe_unused]] Connection& connection, Packet& packet)
    {
        // Start of a new map load, clear the queue now as we have to buffer them
        // until the map is fully loaded.
        GameActions::ClearQueue();
        GameActions::SuspendQueue();

        displayNetworkProgress(STR_LOADING_SAVED_GAME);
    }

    void NetworkBase::Client_Handle_MAP([[maybe_unused]] Connection& connection, Packet& packet)
    {
        // Allow queue processing of game actions again.
        GameActions::ResumeQueue();

        // This prevents invoking the callback for when the window closes which would close the connection.
        GetContext().CloseProgress();

        GameUnloadScripts();
        GameNotifyMapChange();

        auto ms = MemoryStream(packet.Data.data(), packet.Data.size());
        if (LoadMap(&ms))
        {
            GameLoadInit();
            GameLoadScripts();
            GameNotifyMapChanged();

            // This seems wrong, we want to catch up to that tick so we shouldn't mess with this.
            //_serverState.tick = getGameState().currentTicks;

            _serverState.state = ServerStatus::ok;
            _clientMapLoaded = true;
            gFirstTimeSaving = true;

            // Notify user he is now online and which shortcut key enables chat
            ChatShowConnectedMessage();

            // Fix invalid vehicle sprite sizes, thus preventing visual corruption of sprites
            FixInvalidVehicleSpriteSizes();

            // NOTE: Game actions are normally processed before processing the player list.
            // Given that during map load game actions are buffered we have to process the
            // player list first to have valid players for the queued game actions.
            ProcessPlayerList();
        }
        else
        {
            // Something went wrong, game is not loaded. Return to main screen.
            auto loadOrQuitAction = GameActions::LoadOrQuitAction(
                GameActions::LoadOrQuitModes::OpenSavePrompt, PromptMode::saveBeforeQuit);

            loadOrQuitAction.Execute(getGameState());
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

            MapAnimations::ClearAll();
            // TODO: Have a separate GameState and exchange once loaded.
            auto& gameState = getGameState();
            importer->Import(gameState);

            EntityTweener::Get().Reset();
            MapAnimations::MarkAllTiles();

            gLastAutoSaveUpdate = kAutosavePause;
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

            auto& gameState = getGameState();
            exporter->Export(gameState, *stream, kParkFileNetCompressionLevel);
            result = true;
        }
        catch (const std::exception& e)
        {
            Console::Error::WriteLine("Unable to serialise map: %s", e.what());
        }
        return result;
    }

    void NetworkBase::Client_Handle_CHAT([[maybe_unused]] Connection& connection, Packet& packet)
    {
        auto text = packet.ReadString();
        if (!text.empty())
        {
            ChatAddHistory(std::string(text));
        }
    }

    static bool ProcessChatMessagePluginHooks(uint8_t playerId, std::string& text)
    {
    #ifdef ENABLE_SCRIPTING
        auto& hookEngine = GetContext()->GetScriptEngine().GetHookEngine();
        if (hookEngine.HasSubscriptions(Scripting::HookType::networkChat))
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
            hookEngine.Call(Scripting::HookType::networkChat, e, false);

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
    #endif
        return true;
    }

    void NetworkBase::ServerHandleChat(Connection& connection, Packet& packet)
    {
        auto szText = packet.ReadString();
        if (szText.empty())
            return;

        if (connection.player != nullptr)
        {
            NetworkGroup* group = GetGroupByID(connection.player->Group);
            if (group == nullptr || !group->CanPerformAction(Permission::chat))
            {
                return;
            }
        }

        std::string text(szText);
        if (connection.player != nullptr)
        {
            if (!ProcessChatMessagePluginHooks(connection.player->Id, text))
            {
                // Message not to be relayed
                return;
            }
        }

        const char* formatted = FormatChat(connection.player, text.c_str());
        ChatAddHistory(formatted);
        ServerSendChat(formatted);
    }

    void NetworkBase::Client_Handle_GAME_ACTION([[maybe_unused]] Connection& connection, Packet& packet)
    {
        uint32_t tick;
        GameCommand actionType;
        packet >> tick >> actionType;

        MemoryStream stream;
        const size_t size = packet.Header.size - packet.BytesRead;
        stream.WriteArray(packet.Read(size), size);
        stream.SetPosition(0);

        DataSerialiser ds(false, stream);

        GameActions::GameAction::Ptr action = GameActions::Create(actionType);
        if (action == nullptr)
        {
            LOG_ERROR("Received unregistered game action type: 0x%08X", actionType);
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

    void NetworkBase::ServerHandleGameAction(Connection& connection, Packet& packet)
    {
        uint32_t tick;
        GameCommand actionType;

        Player* player = connection.player;
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
            NetworkGroup* group = GetGroupByID(connection.player->Group);
            if (group == nullptr || group->CanPerformCommand(actionType) == false)
            {
                ServerSendShowError(connection, STR_CANT_DO_THIS, STR_PERMISSION_DENIED);
                return;
            }
        }

        // Create and enqueue the action.
        GameActions::GameAction::Ptr ga = GameActions::Create(actionType);
        if (ga == nullptr)
        {
            LOG_ERROR(
                "Received unregistered game action type: 0x%08X from player: (%d) %s", actionType, connection.player->Id,
                connection.player->Name.c_str());
            return;
        }

        // Player who is hosting is not affected by cooldowns.
        if ((player->Flags & PlayerFlags::kIsServer) == 0)
        {
            auto cooldownIt = player->CooldownTime.find(actionType);
            if (cooldownIt != std::end(player->CooldownTime))
            {
                if (cooldownIt->second > 0)
                {
                    ServerSendShowError(connection, STR_CANT_DO_THIS, STR_NETWORK_ACTION_RATE_LIMIT_MESSAGE);
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
        const size_t size = packet.Header.size - packet.BytesRead;
        stream.GetStream().WriteArray(packet.Read(size), size);
        stream.GetStream().SetPosition(0);

        ga->Serialise(stream);
        // Set player to sender, should be 0 if sent from client.
        ga->SetPlayer(PlayerId_t{ connection.player->Id });

        GameActions::Enqueue(std::move(ga), tick);
    }

    void NetworkBase::Client_Handle_TICK([[maybe_unused]] Connection& connection, Packet& packet)
    {
        uint32_t srand0;
        uint32_t flags;
        uint32_t serverTick;

        packet >> serverTick >> srand0 >> flags;

        ServerTickData tickData;
        tickData.srand0 = srand0;
        tickData.tick = serverTick;

        if (flags & TickFlags::kChecksums)
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

    void NetworkBase::Client_Handle_PLAYERINFO([[maybe_unused]] Connection& connection, Packet& packet)
    {
        uint32_t tick;
        packet >> tick;

        Player playerInfo;
        playerInfo.Read(packet);

        _pendingPlayerInfo.emplace(tick, playerInfo);
    }

    void NetworkBase::Client_Handle_PLAYERLIST([[maybe_unused]] Connection& connection, Packet& packet)
    {
        uint32_t tick;
        uint8_t size;
        packet >> tick >> size;

        auto& pending = _pendingPlayerLists[tick];
        pending.players.clear();

        for (uint32_t i = 0; i < size; i++)
        {
            Player tempplayer;
            tempplayer.Read(packet);

            pending.players.push_back(std::move(tempplayer));
        }
    }

    void NetworkBase::Client_Handle_PING([[maybe_unused]] Connection& connection, [[maybe_unused]] Packet& packet)
    {
        Client_Send_PING();
    }

    void NetworkBase::ServerHandlePing(Connection& connection, [[maybe_unused]] Packet& packet)
    {
        int32_t ping = Platform::GetTicks() - connection.PingTime;
        if (ping < 0)
        {
            ping = 0;
        }
        if (connection.player != nullptr)
        {
            connection.player->Ping = ping;
            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->InvalidateByNumber(WindowClass::player, connection.player->Id);
        }
    }

    void NetworkBase::Client_Handle_PINGLIST([[maybe_unused]] Connection& connection, Packet& packet)
    {
        uint8_t size;
        packet >> size;
        for (uint32_t i = 0; i < size; i++)
        {
            uint8_t id;
            uint16_t ping;
            packet >> id >> ping;
            Player* player = GetPlayerByID(id);
            if (player != nullptr)
            {
                player->Ping = ping;
            }
        }

        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->InvalidateByClass(WindowClass::player);
    }

    void NetworkBase::Client_Handle_SETDISCONNECTMSG(Connection& connection, Packet& packet)
    {
        auto disconnectmsg = packet.ReadString();
        if (!disconnectmsg.empty())
        {
            connection.SetLastDisconnectReason(disconnectmsg);
        }
    }

    void NetworkBase::ServerHandleGameInfo(Connection& connection, [[maybe_unused]] Packet& packet)
    {
        ServerSendGameInfo(connection);
    }

    void NetworkBase::Client_Handle_SHOWERROR([[maybe_unused]] Connection& connection, Packet& packet)
    {
        StringId title, message;
        packet >> title >> message;
        ContextShowError(title, message, {});
    }

    void NetworkBase::Client_Handle_GROUPLIST([[maybe_unused]] Connection& connection, Packet& packet)
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

    void NetworkBase::Client_Handle_EVENT([[maybe_unused]] Connection& connection, Packet& packet)
    {
        ServerEvent eventType;
        packet >> eventType;
        switch (eventType)
        {
            case ServerEvent::playerJoined:
            {
                auto playerName = packet.ReadString();
                auto message = FormatStringID(STR_MULTIPLAYER_PLAYER_HAS_JOINED_THE_GAME, playerName);
                ChatAddHistory(message);
                break;
            }
            case ServerEvent::playerDisconnected:
            {
                auto playerName = packet.ReadString();
                auto reason = packet.ReadString();
                std::string message;
                if (reason.empty())
                {
                    message = FormatStringID(STR_MULTIPLAYER_PLAYER_HAS_DISCONNECTED_NO_REASON, playerName);
                }
                else
                {
                    message = FormatStringID(STR_MULTIPLAYER_PLAYER_HAS_DISCONNECTED_WITH_REASON, playerName, reason);
                }
                ChatAddHistory(message);
                break;
            }
        }
    }

    void NetworkBase::Client_Send_GAMEINFO()
    {
        LOG_VERBOSE("requesting gameinfo");
        Packet packet(Command::gameInfo);
        _serverConnection->QueuePacket(std::move(packet));
    }

    void NetworkBase::Client_Handle_GAMEINFO([[maybe_unused]] Connection& connection, Packet& packet)
    {
        auto jsonString = packet.ReadString();
        packet >> _serverState.gamestateSnapshotsEnabled;
        packet >> IsServerPlayerInvisible;

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

    void Reconnect()
    {
        GetContext()->GetNetwork().Reconnect();
    }

    void ShutdownClient()
    {
        GetContext()->GetNetwork().ServerClientDisconnected();
    }

    int32_t BeginClient(const std::string& host, int32_t port)
    {
        return GetContext()->GetNetwork().BeginClient(host, port);
    }

    int32_t BeginServer(int32_t port, const std::string& address)
    {
        return GetContext()->GetNetwork().BeginServer(port, address);
    }

    void Update()
    {
        GetContext()->GetNetwork().Update();
    }

    void Tick()
    {
        GetContext()->GetNetwork().Tick();
    }

    void PostTick()
    {
        GetContext()->GetNetwork().PostTick();
    }

    void Flush()
    {
        GetContext()->GetNetwork().Flush();
    }

    Mode GetMode()
    {
        return GetContext()->GetNetwork().GetMode();
    }

    Status GetStatus()
    {
        return GetContext()->GetNetwork().GetStatus();
    }

    bool IsDesynchronised()
    {
        return GetContext()->GetNetwork().IsDesynchronised();
    }

    bool CheckDesynchronisation()
    {
        return GetContext()->GetNetwork().CheckDesynchronizaton();
    }

    void RequestGamestateSnapshot()
    {
        return GetContext()->GetNetwork().RequestStateSnapshot();
    }

    void SendTick()
    {
        GetContext()->GetNetwork().ServerSendTick();
    }

    Auth GetAuthstatus()
    {
        return GetContext()->GetNetwork().GetAuthStatus();
    }

    uint32_t GetServerTick()
    {
        return GetContext()->GetNetwork().GetServerTick();
    }

    uint8_t GetCurrentPlayerId()
    {
        return GetContext()->GetNetwork().GetPlayerID();
    }

    int32_t GetNumPlayers()
    {
        return GetContext()->GetNetwork().GetTotalNumPlayers();
    }

    int32_t GetNumVisiblePlayers()
    {
        return GetContext()->GetNetwork().GetNumVisiblePlayers();
    }

    const char* GetPlayerName(uint32_t index)
    {
        auto& network = GetContext()->GetNetwork();
        Guard::IndexInRange(index, network.player_list);

        return static_cast<const char*>(network.player_list[index]->Name.c_str());
    }

    uint32_t GetPlayerFlags(uint32_t index)
    {
        auto& network = GetContext()->GetNetwork();
        Guard::IndexInRange(index, network.player_list);

        return network.player_list[index]->Flags;
    }

    int32_t GetPlayerPing(uint32_t index)
    {
        auto& network = GetContext()->GetNetwork();
        Guard::IndexInRange(index, network.player_list);

        return network.player_list[index]->Ping;
    }

    int32_t GetPlayerID(uint32_t index)
    {
        auto& network = GetContext()->GetNetwork();
        Guard::IndexInRange(index, network.player_list);

        return network.player_list[index]->Id;
    }

    money64 GetPlayerMoneySpent(uint32_t index)
    {
        auto& network = GetContext()->GetNetwork();
        Guard::IndexInRange(index, network.player_list);

        return network.player_list[index]->MoneySpent;
    }

    std::string GetPlayerIPAddress(uint32_t id)
    {
        auto& network = GetContext()->GetNetwork();
        auto conn = network.GetPlayerConnection(id);
        if (conn != nullptr && conn->Socket != nullptr)
        {
            return conn->Socket->GetIpAddress();
        }
        return {};
    }

    std::string GetPlayerPublicKeyHash(uint32_t id)
    {
        auto& network = GetContext()->GetNetwork();
        auto player = network.GetPlayerByID(id);
        if (player != nullptr)
        {
            return player->KeyHash;
        }
        return {};
    }

    void IncrementPlayerNumCommands(uint32_t playerIndex)
    {
        auto& network = GetContext()->GetNetwork();
        Guard::IndexInRange(playerIndex, network.player_list);

        network.player_list[playerIndex]->IncrementNumCommands();
    }

    void AddPlayerMoneySpent(uint32_t index, money64 cost)
    {
        auto& network = GetContext()->GetNetwork();
        Guard::IndexInRange(index, network.player_list);

        network.player_list[index]->AddMoneySpent(cost);
    }

    int32_t GetPlayerLastAction(uint32_t index, int32_t time)
    {
        auto& network = GetContext()->GetNetwork();
        Guard::IndexInRange(index, network.player_list);

        if (time && Platform::GetTicks() > network.player_list[index]->LastActionTime + time)
        {
            return -999;
        }
        return network.player_list[index]->LastAction;
    }

    void SetPlayerLastAction(uint32_t index, GameCommand command)
    {
        auto& network = GetContext()->GetNetwork();
        Guard::IndexInRange(index, network.player_list);

        network.player_list[index]->LastAction = static_cast<int32_t>(NetworkActions::FindCommand(command));
        network.player_list[index]->LastActionTime = Platform::GetTicks();
    }

    CoordsXYZ GetPlayerLastActionCoord(uint32_t index)
    {
        auto& network = GetContext()->GetNetwork();
        Guard::IndexInRange(index, GetContext()->GetNetwork().player_list);

        return network.player_list[index]->LastActionCoord;
    }

    void SetPlayerLastActionCoord(uint32_t index, const CoordsXYZ& coord)
    {
        auto& network = GetContext()->GetNetwork();
        Guard::IndexInRange(index, network.player_list);

        if (index < network.player_list.size())
        {
            network.player_list[index]->LastActionCoord = coord;
        }
    }

    uint32_t GetPlayerCommandsRan(uint32_t index)
    {
        auto& network = GetContext()->GetNetwork();
        Guard::IndexInRange(index, GetContext()->GetNetwork().player_list);

        return network.player_list[index]->CommandsRan;
    }

    int32_t GetPlayerIndex(uint32_t id)
    {
        auto& network = GetContext()->GetNetwork();
        auto it = network.GetPlayerIteratorByID(id);
        if (it == network.player_list.end())
        {
            return -1;
        }
        return static_cast<int32_t>(network.GetPlayerIteratorByID(id) - network.player_list.begin());
    }

    uint8_t GetPlayerGroup(uint32_t index)
    {
        auto& network = GetContext()->GetNetwork();
        Guard::IndexInRange(index, network.player_list);

        return network.player_list[index]->Group;
    }

    void SetPlayerGroup(uint32_t index, uint32_t groupindex)
    {
        auto& network = GetContext()->GetNetwork();
        Guard::IndexInRange(index, network.player_list);
        Guard::IndexInRange(groupindex, network.group_list);

        network.player_list[index]->Group = network.group_list[groupindex]->Id;
    }

    int32_t GetGroupIndex(uint8_t id)
    {
        auto& network = GetContext()->GetNetwork();
        auto it = network.GetGroupIteratorByID(id);
        if (it == network.group_list.end())
        {
            return -1;
        }
        return static_cast<int32_t>(network.GetGroupIteratorByID(id) - network.group_list.begin());
    }

    uint8_t GetGroupID(uint32_t index)
    {
        auto& network = GetContext()->GetNetwork();
        Guard::IndexInRange(index, network.group_list);

        return network.group_list[index]->Id;
    }

    int32_t GetNumGroups()
    {
        auto& network = GetContext()->GetNetwork();
        return static_cast<int32_t>(network.group_list.size());
    }

    const char* GetGroupName(uint32_t index)
    {
        auto& network = GetContext()->GetNetwork();
        return network.group_list[index]->GetName().c_str();
    }

    void ChatShowConnectedMessage()
    {
        auto windowManager = Ui::GetWindowManager();
        std::string s = windowManager->GetKeyboardShortcutString("interface.misc.multiplayer_chat");
        const char* sptr = s.c_str();

        utf8 buffer[256];
        FormatStringLegacy(buffer, sizeof(buffer), STR_MULTIPLAYER_CONNECTED_CHAT_HINT, &sptr);

        Player server;
        server.Name = "Server";
        const char* formatted = NetworkBase::FormatChat(&server, buffer);
        ChatAddHistory(formatted);
    }

    // Display server greeting if one exists
    void ChatShowServerGreeting()
    {
        const auto& greeting = GetServerGreeting();
        if (!greeting.empty())
        {
            thread_local std::string greeting_formatted;
            greeting_formatted.assign("{OUTLINE}{GREEN}");
            greeting_formatted += greeting;
            ChatAddHistory(greeting_formatted);
        }
    }

    GameActions::Result SetPlayerGroup(PlayerId_t actionPlayerId, PlayerId_t playerId, uint8_t groupId, bool isExecuting)
    {
        auto& network = GetContext()->GetNetwork();
        Player* player = network.GetPlayerByID(playerId);

        NetworkGroup* fromgroup = network.GetGroupByID(actionPlayerId);
        if (player == nullptr)
        {
            return GameActions::Result(GameActions::Status::invalidParameters, STR_CANT_DO_THIS, kStringIdNone);
        }

        if (network.GetGroupByID(groupId) == nullptr)
        {
            return GameActions::Result(GameActions::Status::invalidParameters, STR_CANT_DO_THIS, kStringIdNone);
        }

        if (player->Flags & PlayerFlags::kIsServer)
        {
            return GameActions::Result(
                GameActions::Status::invalidParameters, STR_CANT_CHANGE_GROUP_THAT_THE_HOST_BELONGS_TO, kStringIdNone);
        }

        if (groupId == 0 && fromgroup != nullptr && fromgroup->Id != 0)
        {
            return GameActions::Result(GameActions::Status::invalidParameters, STR_CANT_SET_TO_THIS_GROUP, kStringIdNone);
        }

        if (isExecuting)
        {
            player->Group = groupId;

            if (GetMode() == Mode::server)
            {
                // Add or update saved user
                UserManager& userManager = network._userManager;
                User* networkUser = userManager.GetOrAddUser(player->KeyHash);
                networkUser->GroupId = groupId;
                networkUser->Name = player->Name;
                userManager.Save();
            }

            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->InvalidateByNumber(WindowClass::player, playerId);

            // Log set player group event
            Player* game_command_player = network.GetPlayerByID(actionPlayerId);
            NetworkGroup* new_player_group = network.GetGroupByID(groupId);
            char log_msg[256];
            const char* args[3] = {
                player->Name.c_str(),
                new_player_group->GetName().c_str(),
                game_command_player->Name.c_str(),
            };
            FormatStringLegacy(log_msg, 256, STR_LOG_SET_PLAYER_GROUP, args);
            AppendServerLog(log_msg);
        }
        return GameActions::Result();
    }

    GameActions::Result ModifyGroups(
        PlayerId_t actionPlayerId, GameActions::ModifyGroupType type, uint8_t groupId, const std::string& name,
        uint32_t permissionIndex, GameActions::PermissionState permissionState, bool isExecuting)
    {
        auto& network = GetContext()->GetNetwork();
        switch (type)
        {
            case GameActions::ModifyGroupType::AddGroup:
            {
                if (isExecuting)
                {
                    NetworkGroup* newgroup = network.AddGroup();
                    if (newgroup == nullptr)
                    {
                        return GameActions::Result(GameActions::Status::unknown, STR_CANT_DO_THIS, kStringIdNone);
                    }
                }
            }
            break;
            case GameActions::ModifyGroupType::RemoveGroup:
            {
                if (groupId == 0)
                {
                    return GameActions::Result(
                        GameActions::Status::disallowed, STR_THIS_GROUP_CANNOT_BE_MODIFIED, kStringIdNone);
                }
                for (const auto& it : network.player_list)
                {
                    if ((it.get())->Group == groupId)
                    {
                        return GameActions::Result(
                            GameActions::Status::disallowed, STR_CANT_REMOVE_GROUP_THAT_PLAYERS_BELONG_TO, kStringIdNone);
                    }
                }
                if (isExecuting)
                {
                    network.RemoveGroup(groupId);
                }
            }
            break;
            case GameActions::ModifyGroupType::SetPermissions:
            {
                if (groupId == 0)
                { // can't change admin group permissions
                    return GameActions::Result(
                        GameActions::Status::disallowed, STR_THIS_GROUP_CANNOT_BE_MODIFIED, kStringIdNone);
                }
                NetworkGroup* mygroup = nullptr;
                Player* player = network.GetPlayerByID(actionPlayerId);
                auto networkPermission = static_cast<Permission>(permissionIndex);
                if (player != nullptr && permissionState == GameActions::PermissionState::Toggle)
                {
                    mygroup = network.GetGroupByID(player->Group);
                    if (mygroup == nullptr || !mygroup->CanPerformAction(networkPermission))
                    {
                        return GameActions::Result(
                            GameActions::Status::disallowed, STR_CANT_MODIFY_PERMISSION_THAT_YOU_DO_NOT_HAVE_YOURSELF,
                            kStringIdNone);
                    }
                }
                if (isExecuting)
                {
                    NetworkGroup* group = network.GetGroupByID(groupId);
                    if (group != nullptr)
                    {
                        if (permissionState != GameActions::PermissionState::Toggle)
                        {
                            if (mygroup != nullptr)
                            {
                                if (permissionState == GameActions::PermissionState::SetAll)
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
            case GameActions::ModifyGroupType::SetName:
            {
                NetworkGroup* group = network.GetGroupByID(groupId);
                if (group == nullptr)
                {
                    return GameActions::Result(GameActions::Status::invalidParameters, STR_CANT_RENAME_GROUP, kStringIdNone);
                }

                const char* oldName = group->GetName().c_str();

                if (strcmp(oldName, name.c_str()) == 0)
                {
                    return GameActions::Result();
                }

                if (name.empty())
                {
                    return GameActions::Result(
                        GameActions::Status::invalidParameters, STR_CANT_RENAME_GROUP, STR_INVALID_GROUP_NAME);
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
            case GameActions::ModifyGroupType::SetDefault:
            {
                if (groupId == 0)
                {
                    return GameActions::Result(GameActions::Status::disallowed, STR_CANT_SET_TO_THIS_GROUP, kStringIdNone);
                }
                if (isExecuting)
                {
                    network.SetDefaultGroup(groupId);
                }
            }
            break;
            default:
                LOG_ERROR("Invalid Modify Group Type: %u", static_cast<uint8_t>(type));
                return GameActions::Result(
                    GameActions::Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
        }

        network.SaveGroups();

        return GameActions::Result();
    }

    GameActions::Result KickPlayer(PlayerId_t playerId, bool isExecuting)
    {
        auto& network = GetContext()->GetNetwork();
        Player* player = network.GetPlayerByID(playerId);
        if (player == nullptr)
        {
            // Player might be already removed by the PLAYERLIST command, need to refactor non-game commands executing too
            // early.
            return GameActions::Result(
                GameActions::Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_PLAYER_NOT_FOUND);
        }

        if (player->Flags & PlayerFlags::kIsServer)
        {
            return GameActions::Result(GameActions::Status::disallowed, STR_CANT_KICK_THE_HOST, kStringIdNone);
        }

        if (isExecuting)
        {
            if (network.GetMode() == Mode::server)
            {
                network.KickPlayer(playerId);

                UserManager& networkUserManager = network._userManager;
                networkUserManager.Load();
                networkUserManager.RemoveUser(player->KeyHash);
                networkUserManager.Save();
            }
        }
        return GameActions::Result();
    }

    uint8_t GetDefaultGroup()
    {
        auto& network = GetContext()->GetNetwork();
        return network.GetDefaultGroup();
    }

    int32_t GetNumActions()
    {
        return static_cast<int32_t>(NetworkActions::Actions.size());
    }

    StringId GetActionNameStringID(uint32_t index)
    {
        if (index < NetworkActions::Actions.size())
        {
            return NetworkActions::Actions[index].Name;
        }

        return kStringIdNone;
    }

    int32_t CanPerformAction(uint32_t groupindex, Permission index)
    {
        auto& network = GetContext()->GetNetwork();
        Guard::IndexInRange(groupindex, network.group_list);

        return network.group_list[groupindex]->CanPerformAction(index);
    }

    int32_t CanPerformCommand(uint32_t groupindex, int32_t index)
    {
        auto& network = GetContext()->GetNetwork();
        Guard::IndexInRange(groupindex, network.group_list);

        return network.group_list[groupindex]->CanPerformCommand(static_cast<GameCommand>(index)); // TODO
    }

    void SetPickupPeep(uint8_t playerid, Peep* peep)
    {
        auto& network = GetContext()->GetNetwork();
        if (network.GetMode() == Mode::none)
        {
            _pickup_peep = peep;
        }
        else
        {
            Player* player = network.GetPlayerByID(playerid);
            if (player != nullptr)
            {
                player->PickupPeep = peep;
            }
        }
    }

    Peep* GetPickupPeep(uint8_t playerid)
    {
        auto& network = GetContext()->GetNetwork();
        if (network.GetMode() == Mode::none)
        {
            return _pickup_peep;
        }

        Player* player = network.GetPlayerByID(playerid);
        if (player != nullptr)
        {
            return player->PickupPeep;
        }
        return nullptr;
    }

    void SetPickupPeepOldX(uint8_t playerid, int32_t x)
    {
        auto& network = GetContext()->GetNetwork();
        if (network.GetMode() == Mode::none)
        {
            _pickup_peep_old_x = x;
        }
        else
        {
            Player* player = network.GetPlayerByID(playerid);
            if (player != nullptr)
            {
                player->PickupPeepOldX = x;
            }
        }
    }

    int32_t GetPickupPeepOldX(uint8_t playerid)
    {
        auto& network = GetContext()->GetNetwork();
        if (network.GetMode() == Mode::none)
        {
            return _pickup_peep_old_x;
        }

        Player* player = network.GetPlayerByID(playerid);
        if (player != nullptr)
        {
            return player->PickupPeepOldX;
        }
        return -1;
    }

    bool IsServerPlayerInvisible()
    {
        return GetContext()->GetNetwork().IsServerPlayerInvisible;
    }

    int32_t GetCurrentPlayerGroupIndex()
    {
        auto& network = GetContext()->GetNetwork();
        Player* player = network.GetPlayerByID(network.GetPlayerID());
        if (player != nullptr)
        {
            return GetGroupIndex(player->Group);
        }
        return -1;
    }

    void SendChat(const char* text, const std::vector<uint8_t>& playerIds)
    {
        auto& network = GetContext()->GetNetwork();
        if (network.GetMode() == Mode::client)
        {
            network.Client_Send_CHAT(text);
        }
        else if (network.GetMode() == Mode::server)
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
                        ChatAddHistory(formatted);
                    }
                    network.ServerSendChat(formatted, playerIds);
                }
            }
        }
    }

    void SendGameAction(const GameActions::GameAction* action)
    {
        auto& network = GetContext()->GetNetwork();
        switch (network.GetMode())
        {
            case Mode::server:
                network.ServerSendGameAction(action);
                break;
            case Mode::client:
                network.Client_Send_GAME_ACTION(action);
                break;
            default:
                break;
        }
    }

    void SendPassword(const std::string& password)
    {
        auto& network = GetContext()->GetNetwork();
        const auto keyPath = GetPrivateKeyPath(Config::Get().network.playerName);
        if (!File::Exists(keyPath))
        {
            LOG_ERROR("Private key %s missing! Restart the game to generate it.", keyPath.c_str());
            return;
        }
        try
        {
            auto fs = FileStream(keyPath, FileMode::open);
            network._key.LoadPrivate(&fs);
        }
        catch (const std::exception&)
        {
            LOG_ERROR("Error reading private key from %s.", keyPath.c_str());
            return;
        }
        const std::string pubkey = network._key.PublicKeyString();

        std::vector<uint8_t> signature;
        network._key.Sign(network._challenge.data(), network._challenge.size(), signature);
        // Don't keep private key in memory. There's no need and it may get leaked
        // when process dump gets collected at some point in future.
        network._key.Unload();
        network.Client_Send_AUTH(Config::Get().network.playerName, password, pubkey, signature);
    }

    void SetPassword(const char* password)
    {
        auto& network = GetContext()->GetNetwork();
        network.SetPassword(password);
    }

    void AppendChatLog(std::string_view text)
    {
        auto& network = GetContext()->GetNetwork();
        network.AppendChatLog(text);
    }

    void AppendServerLog(const utf8* text)
    {
        auto& network = GetContext()->GetNetwork();
        network.AppendServerLog(text);
    }

    static u8string GetKeysDirectory()
    {
        auto& env = GetContext()->GetPlatformEnvironment();
        return Path::Combine(env.GetDirectoryPath(DirBase::user), u8"keys");
    }

    static u8string GetPrivateKeyPath(u8string_view playerName)
    {
        return Path::Combine(GetKeysDirectory(), u8string(playerName) + u8".privkey");
    }

    static u8string GetPublicKeyPath(u8string_view playerName, u8string_view hash)
    {
        const auto filename = u8string(playerName) + u8"-" + u8string(hash) + u8".pubkey";
        return Path::Combine(GetKeysDirectory(), filename);
    }

    u8string GetServerName()
    {
        auto& network = GetContext()->GetNetwork();
        return network.ServerName;
    }
    u8string GetServerDescription()
    {
        auto& network = GetContext()->GetNetwork();
        return network.ServerDescription;
    }
    u8string GetServerGreeting()
    {
        auto& network = GetContext()->GetNetwork();
        return network.ServerGreeting;
    }
    u8string GetServerProviderName()
    {
        auto& network = GetContext()->GetNetwork();
        return network.ServerProviderName;
    }
    u8string GetServerProviderEmail()
    {
        auto& network = GetContext()->GetNetwork();
        return network.ServerProviderEmail;
    }
    u8string GetServerProviderWebsite()
    {
        auto& network = GetContext()->GetNetwork();
        return network.ServerProviderWebsite;
    }

    std::string GetVersion()
    {
        return kStreamID;
    }

    Stats GetStats()
    {
        auto& network = GetContext()->GetNetwork();
        return network.GetStats();
    }

    ServerState GetServerState()
    {
        auto& network = GetContext()->GetNetwork();
        return network.GetServerState();
    }

    bool GamestateSnapshotsEnabled()
    {
        return GetServerState().gamestateSnapshotsEnabled;
    }

    json_t GetServerInfoAsJson()
    {
        auto& network = GetContext()->GetNetwork();
        return network.GetServerInfoAsJson();
    }

} // namespace OpenRCT2::Network

#else // DISABLE_NETWORK

namespace OpenRCT2::Network
{
    Mode GetMode()
    {
        return Mode::none;
    }
    Status GetStatus()
    {
        return Status::none;
    }
    Auth GetAuthstatus()
    {
        return Auth::none;
    }
    uint32_t GetServerTick()
    {
        return getGameState().currentTicks;
    }
    void Flush()
    {
    }
    void SendTick()
    {
    }
    bool IsDesynchronised()
    {
        return false;
    }
    bool GamestateSnapshotsEnabled()
    {
        return false;
    }
    bool CheckDesynchronisation()
    {
        return false;
    }
    void RequestGamestateSnapshot()
    {
    }
    void SendGameAction(const GameActions::GameAction* action)
    {
    }
    void Tick()
    {
    }
    void Update()
    {
    }
    void PostTick()
    {
    }
    int32_t BeginClient(const std::string& host, int32_t port)
    {
        return 1;
    }
    int32_t BeginServer(int32_t port, const std::string& address)
    {
        return 1;
    }
    int32_t GetNumPlayers()
    {
        return 1;
    }
    int32_t GetNumVisiblePlayers()
    {
        return 1;
    }
    const char* GetPlayerName(uint32_t index)
    {
        return "local (OpenRCT2 compiled without MP)";
    }
    uint32_t GetPlayerFlags(uint32_t index)
    {
        return 0;
    }
    int32_t GetPlayerPing(uint32_t index)
    {
        return 0;
    }
    int32_t GetPlayerID(uint32_t index)
    {
        return 0;
    }
    money64 GetPlayerMoneySpent(uint32_t index)
    {
        return 0.00_GBP;
    }
    std::string GetPlayerIPAddress(uint32_t id)
    {
        return {};
    }
    std::string GetPlayerPublicKeyHash(uint32_t id)
    {
        return {};
    }
    void IncrementPlayerNumCommands(uint32_t playerIndex)
    {
    }
    void AddPlayerMoneySpent(uint32_t index, money64 cost)
    {
    }
    int32_t GetPlayerLastAction(uint32_t index, int32_t time)
    {
        return -999;
    }
    void SetPlayerLastAction(uint32_t index, GameCommand command)
    {
    }
    CoordsXYZ GetPlayerLastActionCoord(uint32_t index)
    {
        return { 0, 0, 0 };
    }
    void SetPlayerLastActionCoord(uint32_t index, const CoordsXYZ& coord)
    {
    }
    uint32_t GetPlayerCommandsRan(uint32_t index)
    {
        return 0;
    }
    int32_t GetPlayerIndex(uint32_t id)
    {
        return -1;
    }
    uint8_t GetPlayerGroup(uint32_t index)
    {
        return 0;
    }
    void SetPlayerGroup(uint32_t index, uint32_t groupindex)
    {
    }
    int32_t GetGroupIndex(uint8_t id)
    {
        return -1;
    }
    uint8_t GetGroupID(uint32_t index)
    {
        return 0;
    }
    int32_t GetNumGroups()
    {
        return 0;
    }
    const char* GetGroupName(uint32_t index)
    {
        return "";
    };

    GameActions::Result SetPlayerGroup(PlayerId_t actionPlayerId, PlayerId_t playerId, uint8_t groupId, bool isExecuting)
    {
        return GameActions::Result();
    }
    GameActions::Result ModifyGroups(
        PlayerId_t actionPlayerId, GameActions::ModifyGroupType type, uint8_t groupId, const std::string& name,
        uint32_t permissionIndex, GameActions::PermissionState permissionState, bool isExecuting)
    {
        return GameActions::Result();
    }
    GameActions::Result KickPlayer(PlayerId_t playerId, bool isExecuting)
    {
        return GameActions::Result();
    }
    uint8_t GetDefaultGroup()
    {
        return 0;
    }
    int32_t GetNumActions()
    {
        return 0;
    }
    StringId GetActionNameStringID(uint32_t index)
    {
        return -1;
    }
    int32_t CanPerformAction(uint32_t groupindex, Permission index)
    {
        return 0;
    }
    int32_t CanPerformCommand(uint32_t groupindex, int32_t index)
    {
        return 0;
    }
    void SetPickupPeep(uint8_t playerid, Peep* peep)
    {
        _pickup_peep = peep;
    }
    Peep* GetPickupPeep(uint8_t playerid)
    {
        return _pickup_peep;
    }
    void SetPickupPeepOldX(uint8_t playerid, int32_t x)
    {
        _pickup_peep_old_x = x;
    }
    int32_t GetPickupPeepOldX(uint8_t playerid)
    {
        return _pickup_peep_old_x;
    }
    void SendChat(const char* text, const std::vector<uint8_t>& playerIds)
    {
    }
    void SendPassword(const std::string& password)
    {
    }
    void Reconnect()
    {
    }
    void ShutdownClient()
    {
    }
    void SetPassword(const char* password)
    {
    }
    uint8_t GetCurrentPlayerId()
    {
        return 0;
    }
    int32_t GetCurrentPlayerGroupIndex()
    {
        return 0;
    }
    bool IsServerPlayerInvisible()
    {
        return false;
    }
    void AppendChatLog(std::string_view)
    {
    }
    void AppendServerLog(const utf8* text)
    {
    }
    u8string GetServerName()
    {
        return u8string();
    }
    u8string GetServerDescription()
    {
        return u8string();
    }
    u8string GetServerGreeting()
    {
        return u8string();
    }
    u8string GetServerProviderName()
    {
        return u8string();
    }
    u8string GetServerProviderEmail()
    {
        return u8string();
    }
    u8string GetServerProviderWebsite()
    {
        return u8string();
    }
    std::string GetVersion()
    {
        return "Multiplayer disabled";
    }
    Stats GetStats()
    {
        return Stats{};
    }
    ServerState GetServerState()
    {
        return ServerState{};
    }
    json_t GetServerInfoAsJson()
    {
        return {};
    }
} // namespace OpenRCT2::Network

#endif /* DISABLE_NETWORK */
