/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "helpers/LoopbackServers.hpp"

#include <chrono>
#include <gtest/gtest.h>
#include <openrct2/Context.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/Http.h>
#include <openrct2/network/Network.h>
#include <openrct2/network/NetworkBase.h>
#include <openrct2/network/NetworkConnection.h>
#include <openrct2/network/NetworkPacket.h>
#include <openrct2/network/NetworkServerAdvertiser.h>

using namespace OpenRCT2;
using namespace OpenRCT2::Network;

class MockTcpSocket final : public ITcpSocket
{
public:
    SocketStatus GetStatus() const override
    {
        return _status;
    }
    const char* GetError() const override
    {
        return nullptr;
    }
    const char* GetHostName() const override
    {
        return "127.0.0.1";
    }
    std::string GetIpAddress() const override
    {
        return "127.0.0.1";
    }

    void Listen(uint16_t) override
    {
        _status = SocketStatus::listening;
    }
    void Listen(const std::string&, uint16_t) override
    {
        _status = SocketStatus::listening;
    }
    std::unique_ptr<ITcpSocket> Accept() override
    {
        return nullptr;
    }

    void Connect(const std::string&, uint16_t) override
    {
        _status = SocketStatus::connected;
    }
    void ConnectAsync(const std::string&, uint16_t) override
    {
        _status = SocketStatus::connecting;
    }

    void SetInboundData(const std::vector<uint8_t>& data)
    {
        _inboundData = data;
        _inboundOffset = 0;
    }

    size_t SendData(const void*, size_t size) override
    {
        return size;
    }
    ReadPacket ReceiveData(void* buffer, size_t size, size_t* sizeReceived) override
    {
        if (_inboundOffset >= _inboundData.size())
        {
            *sizeReceived = 0;
            return ReadPacket::noData;
        }
        size_t available = _inboundData.size() - _inboundOffset;
        size_t toCopy = std::min(size, available);
        std::memcpy(buffer, _inboundData.data() + _inboundOffset, toCopy);
        _inboundOffset += toCopy;
        *sizeReceived = toCopy;
        return ReadPacket::success;
    }

    void SetNoDelay(bool) override
    {
    }

    void Finish() override
    {
    }
    void Disconnect() override
    {
        _status = SocketStatus::closed;
    }
    void Close() override
    {
        _status = SocketStatus::closed;
    }

private:
    SocketStatus _status = SocketStatus::connected;
    std::vector<uint8_t> _inboundData;
    size_t _inboundOffset = 0;
};

class NetworkTests : public testing::Test
{
protected:
    void SetUp() override
    {
        gOpenRCT2Headless = true;
        gOpenRCT2NoGraphics = true;
        _context = CreateContext();
        try
        {
            _context->Initialise();
        }
        catch (...)
        {
        }
    }

    std::unique_ptr<IContext> _context;
};

TEST_F(NetworkTests, MapRequestWithoutPlayerDisconnectsAndDoesNotCrash)
{
    NetworkBase network(*_context);

    Connection connection;
    auto mockSocket = std::make_unique<MockTcpSocket>();
    connection.socket = std::move(mockSocket);
    connection.authStatus = Auth::none;
    connection.player = nullptr;

    Packet packet(Command::mapRequest);
    packet << static_cast<uint32_t>(0); // 0 objects

    // Before the fix, this would crash because it accesses connection.player->Name
    // After the fix, it should detect connection.player == nullptr and disconnect.
    network.ServerHandleMapRequest(connection, packet);

    EXPECT_TRUE(connection.shouldDisconnect);
}

#ifndef DISABLE_HTTP

class NetworkServerAdvertiserTests : public testing::Test
{
protected:
    void SetUp() override
    {
        auto& networkConfig = Config::Get().network;
        _previousMasterServerUrl = networkConfig.masterServerUrl;
        _previousAdvertise = networkConfig.advertise;
        networkConfig.masterServerUrl = _masterServer.getUrl();
        networkConfig.advertise = true;
    }

    void TearDown() override
    {
        auto& networkConfig = Config::Get().network;
        networkConfig.masterServerUrl = _previousMasterServerUrl;
        networkConfig.advertise = _previousAdvertise;
    }

private:
    UnresponsiveTcpServer _masterServer;
    std::string _previousMasterServerUrl;
    bool _previousAdvertise{};
};

TEST_F(NetworkServerAdvertiserTests, DestructionIsBoundedWhenMasterServerNeverResponds)
{
    // Generous upper bound so the assertion holds on slow CI machines.
    constexpr auto kMaxShutdownTime = Http::kDefaultTimeout + std::chrono::seconds(5);

    auto start = std::chrono::steady_clock::now();
    {
        auto advertiser = CreateServerAdvertiser(kDefaultPort);
        // The first update sends the registration request, which the master server never answers.
        advertiser->update();
        EXPECT_EQ(advertiser->getStatus(), AdvertiseStatus::registering);
    }
    auto elapsed = std::chrono::steady_clock::now() - start;

    // Destruction must actually have waited for the pending request rather than found none.
    EXPECT_GE(elapsed, Http::kDefaultTimeout);
    EXPECT_LT(elapsed, kMaxShutdownTime);
}

#endif // DISABLE_HTTP
