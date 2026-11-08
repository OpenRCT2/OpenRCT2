/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "LoopbackServers.hpp"

#include <openrct2/network/Socket.h>
#include <random>
#include <stdexcept>
#include <utility>

using namespace OpenRCT2::Network;

static constexpr const char* kAddress = "127.0.0.1";
static constexpr uint16_t kMinPort = 20000;
static constexpr uint16_t kMaxPort = 60000;
static constexpr int32_t kMaxAttempts = 20;
static constexpr auto kAcceptTimeout = std::chrono::seconds(10);
static constexpr auto kPollInterval = std::chrono::milliseconds(10);

LoopbackTcpListener::LoopbackTcpListener()
{
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<uint16_t> ports(kMinPort, kMaxPort);
    std::string lastError;
    for (int32_t attempt = 0; attempt < kMaxAttempts; attempt++)
    {
        auto port = ports(rng);
        auto socket = CreateTcpSocket();
        try
        {
            socket->Listen(kAddress, port);
        }
        catch (const std::runtime_error& e)
        {
            lastError = e.what();
            continue;
        }
        _listener = std::move(socket);
        _port = port;
        return;
    }
    throw std::runtime_error("Unable to start the loopback server: " + lastError);
}

LoopbackTcpListener::~LoopbackTcpListener() = default;

std::string LoopbackTcpListener::getUrl() const
{
    return std::string("http://") + kAddress + ":" + std::to_string(_port);
}

TrickleHttpServer::TrickleHttpServer(std::string body, std::chrono::milliseconds interval)
    : _body(std::move(body))
    , _interval(interval)
    , _thread([this]() { serve(); })
{
}

TrickleHttpServer::~TrickleHttpServer()
{
    _stop = true;
    _thread.join();
}

static void drain(ITcpSocket& client)
{
    char buffer[256];
    size_t received{};
    while (client.ReceiveData(buffer, sizeof(buffer), &received) == ReadPacket::success)
    {
    }
}

void TrickleHttpServer::serve()
{
    auto client = acceptClient();
    if (client == nullptr)
    {
        return;
    }

    auto header = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: " + std::to_string(_body.size())
        + "\r\nConnection: close\r\n\r\n";
    client->SendData(header.data(), header.size());
    for (char byte : _body)
    {
        if (_stop)
        {
            break;
        }
        std::this_thread::sleep_for(_interval);
        client->SendData(&byte, sizeof(byte));
    }
    client->Finish();
    // Consume the request so that closing does not reset the connection before the client has read the reply.
    drain(*client);
    client->Close();
}

std::unique_ptr<ITcpSocket> TrickleHttpServer::acceptClient()
{
    auto deadline = std::chrono::steady_clock::now() + kAcceptTimeout;
    while (!_stop && std::chrono::steady_clock::now() < deadline)
    {
        auto client = _listener->Accept();
        if (client != nullptr)
        {
            return client;
        }
        std::this_thread::sleep_for(kPollInterval);
    }
    return nullptr;
}
