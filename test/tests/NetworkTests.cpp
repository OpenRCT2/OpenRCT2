/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <gtest/gtest.h>
#include <openrct2/Context.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/network/NetworkBase.h>
#include <openrct2/network/NetworkConnection.h>
#include <openrct2/network/NetworkPacket.h>
#include <openrct2/network/Socket.h>

#ifndef _WIN32
    #include <arpa/inet.h>
    #include <chrono>
    #include <netinet/in.h>
    #include <signal.h>
    #include <sys/socket.h>
    #include <thread>
    #include <unistd.h>
    #include <vector>
#endif

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

TEST(SocketTests, ConnectToClosedPortThrows)
{
    // Nothing listens on port 1 on loopback, so the connection is refused.
    auto socket = CreateTcpSocket();
    EXPECT_ANY_THROW(socket->Connect("127.0.0.1", 1));
    EXPECT_NE(socket->GetStatus(), SocketStatus::connected);
}

#ifndef _WIN32
TEST(SocketTests, SendDataToClosedPeerReturnsErrorInsteadOfSigpipe)
{
    // Regression test for #27386: on platforms without MSG_NOSIGNAL (e.g. macOS)
    // sending to a peer that closed the connection used to raise SIGPIPE,
    // killing the process with exit code 141 and no diagnostics. The send path
    // must instead fail gracefully and let the caller handle the disconnect.
    // Restore the default disposition first: linked libraries (e.g. libcurl)
    // may have installed a handler, while the shipped game runs with the
    // default and dies.
    ::signal(SIGPIPE, SIG_DFL);
    bool exercised = false;
    for (uint16_t port = 41100; port < 41120 && !exercised; port++)
    {
        auto listener = CreateTcpSocket();
        try
        {
            listener->Listen("127.0.0.1", port);
        }
        catch (const std::exception&)
        {
            continue; // Port already in use, try the next one.
        }

        // Connect a raw client socket and close it straight away so the
        // accepted socket ends up writing to a half-closed connection.
        auto client = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        ASSERT_NE(client, -1);
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        ASSERT_EQ(::connect(client, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)), 0);
        ::close(client);

        std::unique_ptr<ITcpSocket> serverSide;
        for (int i = 0; i < 200 && serverSide == nullptr; i++)
        {
            serverSide = listener->Accept();
            if (serverSide == nullptr)
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        ASSERT_NE(serverSide, nullptr);

        // Give the FIN time to arrive, then keep sending until the write end
        // notices the closed peer. Unfixed this dies from SIGPIPE; fixed it
        // just returns a short count.
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        std::vector<uint8_t> payload(64 * 1024, 0);
        for (int attempt = 0; attempt < 25; attempt++)
        {
            serverSide->SendData(payload.data(), payload.size());
            // Leave the kernel time to process the peer's close and reply RST;
            // sending in a tight loop keeps everything in the local buffer and
            // would not reproduce the fault.
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        exercised = true;
    }
    ASSERT_TRUE(exercised) << "Could not bind any loopback port for the test";
    // Reaching this point means no SIGPIPE was raised.
}
#endif
