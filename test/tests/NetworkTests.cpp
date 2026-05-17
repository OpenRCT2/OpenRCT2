/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_NETWORK

    #include "TestData.h"

    #include <chrono>
    #include <gtest/gtest.h>
    #include <memory>
    #include <openrct2/Context.h>
    #include <openrct2/Game.h>
    #include <openrct2/OpenRCT2.h>
    #include <openrct2/config/Config.h>
    #include <openrct2/core/Crypt.h>
    #include <openrct2/network/Network.h>
    #include <openrct2/network/NetworkBase.h>
    #include <openrct2/network/NetworkConnection.h>
    #include <openrct2/network/NetworkPacket.h>
    #include <openrct2/network/NetworkTypes.h>
    #include <openrct2/network/Socket.h>
    #include <thread>

using namespace OpenRCT2;
using namespace OpenRCT2::Network;

// Drives the server's accept/read/process loop for a short period.
static void PumpServer(const int iterations = 20)
{
    for (int i = 0; i < iterations; ++i)
    {
        Network::Update();
        Network::Tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

// Build a Connection acting as the test client
static std::unique_ptr<Connection> ConnectTestClient(uint16_t port)
{
    auto client = std::make_unique<Connection>();
    client->Socket = CreateTcpSocket();
    client->Socket->Connect("127.0.0.1", port);
    client->AuthStatus = Auth::ok;
    return client;
}

// Pump the server while letting the client send/receive until a packet of
// the expected command id has been parsed into client.InboundPacket.
static bool WaitForPacket(Connection& client, const Command expectedId, int maxIterations = 50)
{
    for (int i = 0; i < maxIterations; ++i)
    {
        PumpServer(2);
        client.update();
        while (client.readPacket() == ReadPacket::success)
        {
            if (client.InboundPacket.GetCommand() == expectedId)
                return true;
            client.InboundPacket.Clear();
        }
    }
    return false;
}

// Drive the full token + auth handshake. Returns true on Auth::ok.
static bool AuthenticateClient(Connection& client, const std::string_view playerName)
{
    // Send token request, receive challenge
    client.QueuePacket(Packet(Command::token));
    if (!WaitForPacket(client, Command::token))
        return false;
    uint32_t challengeSize = 0;
    client.InboundPacket >> challengeSize;
    if (challengeSize == 0)
        return false;
    const auto* challengeBytes = client.InboundPacket.Read(challengeSize);
    if (challengeBytes == nullptr)
        return false;
    const std::vector<uint8_t> challenge(challengeBytes, challengeBytes + challengeSize);

    // Sign the challenge
    const auto key = Crypt::CreateRSAKey();
    key->Generate();
    const auto rsa = Crypt::CreateRSA();
    const auto signature = rsa->SignData(*key, challenge.data(), challenge.size());
    const std::string pubkey = key->GetPublic();

    // Send the auth packet
    Packet authPacket(Command::auth);
    authPacket.WriteString(Network::GetVersion());
    authPacket.WriteString(playerName);
    authPacket.WriteString("");
    authPacket.WriteString(pubkey);
    authPacket << static_cast<uint32_t>(signature.size());
    authPacket.Write(signature.data(), signature.size());
    client.QueuePacket(authPacket);

    if (!WaitForPacket(client, Command::auth))
        return false;
    uint32_t authStatus = 0;
    client.InboundPacket >> authStatus;
    return static_cast<Auth>(authStatus) == Auth::ok;
}

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
    static constexpr uint16_t kTestPort = 11760;

    void SetUp() override
    {
        gOpenRCT2Headless = true;
        gOpenRCT2NoGraphics = true;
        _context = CreateContext();

        ASSERT_TRUE(_context->Initialise());

        const auto parkPath = TestData::GetParkPath("EverythingPark.park");
        GetContext()->LoadParkFromFile(parkPath);
        GameLoadInit();

        // Don't broadcast to the master server during tests.
        Config::Get().network.advertise = false;

        ASSERT_NE(Network::BeginServer(kTestPort, "127.0.0.1"), 0);
        PumpServer(5);
    }

    std::unique_ptr<IContext> _context;
};

// A misbehaving client sends a `mapRequest` packet without authenticating.
TEST_F(NetworkTests, UnauthenticatedMapRequest_DoesNotCrashServer)
{
    const auto client = ConnectTestClient(kTestPort);
    ASSERT_EQ(client->Socket->GetStatus(), SocketStatus::connected);

    PumpServer(5);

    // Empty mapRequest payload (claims 0 objects)
    client->QueuePacket(Packet(Command::mapRequest));
    client->update();

    PumpServer(20);

    SUCCEED();
}

// An authenticated client sends a malformed `mapRequest`: the declared object
// count is larger than the actual data.
TEST_F(NetworkTests, AuthenticatedMalformedMapRequest_DoesNotCrashServer)
{
    const auto client = ConnectTestClient(kTestPort);
    ASSERT_EQ(client->Socket->GetStatus(), SocketStatus::connected);

    ASSERT_TRUE(AuthenticateClient(*client, "tester"));

    // Claim 1000 DAT objects, supply only the generation byte for one.
    Packet malformed(Command::mapRequest);
    malformed << static_cast<uint32_t>(1000);
    malformed << static_cast<uint8_t>(0); // generation = DAT, no RCTObjectEntry data follows
    client->QueuePacket(malformed);
    client->update();
    PumpServer(20);

    SUCCEED();
}

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
#endif // !DISABLE_NETWORK
