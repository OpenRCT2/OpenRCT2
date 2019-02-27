#include "NetworkServer.h"

#include "../Cheats.h"
#include "../Game.h"
#include "../ParkImporter.h"
#include "../actions/GameAction.h"
#include "../config/Config.h"
#include "../core/Console.hpp"
#include "../core/FileStream.hpp"
#include "../core/Json.hpp"
#include "../core/MemoryStream.h"
#include "../core/String.hpp"
#include "../interface/Chat.h"
#include "../interface/Window.h"
#include "../localisation/Date.h"
#include "../localisation/Localisation.h"
#include "../object/ObjectManager.h"
#include "../object/ObjectRepository.h"

NetworkServer::NetworkServer()
{
    _dispatcher.Register<NetworkPacketRequestToken>(
        std::bind(&NetworkServer::HandleRequestToken, this, std::placeholders::_1, std::placeholders::_2));
}

NetworkServer::~NetworkServer()
{
}

void NetworkServer::LoadConfiguration()
{
    _hostInfo.name = gConfigNetwork.server_name;
    _hostInfo.description = gConfigNetwork.server_description;
    _hostInfo.greeting = gConfigNetwork.server_greeting;
    _hostInfo.providerName = gConfigNetwork.provider_name;
    _hostInfo.providerEmail = gConfigNetwork.provider_email;
    _hostInfo.providerWebsite = gConfigNetwork.provider_website;
}

bool NetworkServer::Startup()
{
    log_verbose("%s\n", __FUNCTION__);

    _userManager.Load();

    _listenSocket = ITcpSocket::Create();

    LoadConfiguration();

    _state = NetworkState::READY;

    return true;
}

bool NetworkServer::Shutdown()
{
    log_verbose("%s\n", __FUNCTION__);

    if (_state == NetworkState::NONE)
    {
        return false;
    }

    Close();
    _state = NetworkState::NONE;

    return true;
}

void NetworkServer::Close()
{
    log_verbose("%s\n", __FUNCTION__);

    // TODO: Close all clients and listen socket.
}

bool NetworkServer::Listen(uint16_t port, const char* address)
{
    log_verbose("%s\n", __FUNCTION__);

    if (_state != NetworkState::READY)
    {
        return false;
    }

    assert(_listenSocket != nullptr);

    if (strlen(address) == 0)
    {
        address = nullptr;
    }

    log_verbose("Begin listening for clients.");

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

    log_verbose("Listening for clients.");

    // TODO: Setup advertiser.
    _state = NetworkState::CONNECTED;

    return true;
}

void NetworkServer::Update()
{
    if (_state != NetworkState::CONNECTED)
    {
        return;
    }

    UpdateServer();
    UpdateClients();
    AcceptClients();
}

void NetworkServer::Flush()
{
    for (auto it = _clients.begin(); it != _clients.end(); it++)
    {
        (*it)->Flush();
    }
}

void NetworkServer::UpdateServer()
{
    /*
    uint32_t ticks = platform_get_ticks();
    if (ticks > last_ping_sent_time + 3000)
    {
        Server_Send_PING();
        Server_Send_PINGLIST();
    }

    if (_advertiser != nullptr)
    {
        _advertiser->Update();
    }
    */
}

void NetworkServer::UpdateClients()
{
    for (auto it = _clients.begin(); it != _clients.end();)
    {
        auto& client = *it;
        if (!UpdateClient(client))
        {
            RemoveClient(client);

            it = _clients.erase(it);
        }
        else
        {
            it++;
        }
    }
}

bool NetworkServer::UpdateClient(std::unique_ptr<NetworkConnection>& client)
{
    /*
    int32_t status = client->ReadPacket(_packetDispatcher);
    if (status == NETWORK_READPACKET_DISCONNECTED)
    {
        log_verbose("Unable to read packet");
        return false;
    }
    */
    return true;
}

void NetworkServer::AcceptClients()
{
    while (true)
    {
        std::unique_ptr<ITcpSocket> tcpSocket = _listenSocket->Accept();
        if (!tcpSocket)
            break;

        AcceptClient(std::move(tcpSocket));
    }
}

void NetworkServer::AcceptClient(std::unique_ptr<ITcpSocket>&& sock)
{
    // TODO: Resume game if this is our first client.
    log_verbose("New client connected: %s", sock->GetHostName());

    std::unique_ptr<NetworkConnection> connection = std::make_unique<NetworkConnection>(std::move(sock));
    _clients.emplace_back(std::move(connection));
}

void NetworkServer::RemoveClient(std::unique_ptr<NetworkConnection>& connection)
{
    log_verbose("%s", __FUNCTION__);
    // All data is still valid at this point, will be erased once this function returns.
}

void NetworkServer::HandleRequestToken(NetworkConnection& connection, const NetworkPacketRequestToken& packet)
{
    log_verbose("%s", __FUNCTION__);
}
