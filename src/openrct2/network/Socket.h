/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <memory>
#include <string>
#include <vector>

enum class SocketStatus
{
    Closed,
    Waiting,
    Resolving,
    Connecting,
    Connected,
    Listening,
};

enum class NetworkReadPacket : int32_t
{
    Success,
    NoData,
    MoreData,
    Disconnected
};

/**
 * Represents an address and port.
 */
struct INetworkEndpoint
{
    virtual ~INetworkEndpoint()
    {
    }

    virtual std::string GetHostname() const = 0;
};

/**
 * Represents a TCP socket / connection or listener.
 */
struct ITcpSocket
{
public:
    virtual ~ITcpSocket() = default;

    virtual SocketStatus GetStatus() const = 0;
    virtual const char* GetError() const = 0;
    virtual const char* GetHostName() const = 0;
    virtual std::string GetIpAddress() const = 0;

    virtual void Listen(uint16_t port) = 0;
    virtual void Listen(const std::string& address, uint16_t port) = 0;
    [[nodiscard]] virtual std::unique_ptr<ITcpSocket> Accept() = 0;

    virtual void Connect(const std::string& address, uint16_t port) = 0;
    virtual void ConnectAsync(const std::string& address, uint16_t port) = 0;

    virtual size_t SendData(const void* buffer, size_t size) = 0;
    virtual NetworkReadPacket ReceiveData(void* buffer, size_t size, size_t* sizeReceived) = 0;

    virtual void SetNoDelay(bool noDelay) = 0;

    virtual void Finish() = 0;
    virtual void Disconnect() = 0;
    virtual void Close() = 0;
};

/**
 * Represents a UDP socket / listener.
 */
struct IUdpSocket
{
public:
    virtual ~IUdpSocket() = default;

    virtual SocketStatus GetStatus() const = 0;
    virtual const char* GetError() const = 0;
    virtual const char* GetHostName() const = 0;

    virtual void Listen(uint16_t port) = 0;
    virtual void Listen(const std::string& address, uint16_t port) = 0;

    virtual size_t SendData(const std::string& address, uint16_t port, const void* buffer, size_t size) = 0;
    virtual size_t SendData(const INetworkEndpoint& destination, const void* buffer, size_t size) = 0;
    virtual NetworkReadPacket ReceiveData(
        void* buffer, size_t size, size_t* sizeReceived, std::unique_ptr<INetworkEndpoint>* sender)
        = 0;

    virtual void Close() = 0;
};

[[nodiscard]] std::unique_ptr<ITcpSocket> CreateTcpSocket();
[[nodiscard]] std::unique_ptr<IUdpSocket> CreateUdpSocket();
[[nodiscard]] std::vector<std::unique_ptr<INetworkEndpoint>> GetBroadcastAddresses();

namespace OpenRCT2::Convert
{
    uint16_t HostToNetwork(uint16_t value);
    uint16_t NetworkToHost(uint16_t value);
} // namespace OpenRCT2::Convert
