/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_NETWORK

#    include <chrono>
#    include <cmath>
#    include <cstring>
#    include <future>
#    include <string>
#    include <thread>

// clang-format off
// MSVC: include <math.h> here otherwise PI gets defined twice
#include <cmath>

#ifdef _WIN32
    // winsock2 must be included before windows.h
    #include <winsock2.h>
    #include <ws2tcpip.h>

    #define LAST_SOCKET_ERROR() WSAGetLastError()
    #undef EWOULDBLOCK
    #define EWOULDBLOCK WSAEWOULDBLOCK
    #ifndef SHUT_RD
        #define SHUT_RD SD_RECEIVE
    #endif
    #ifndef SHUT_RDWR
        #define SHUT_RDWR SD_BOTH
    #endif
    #define FLAG_NO_PIPE 0
#else
    #include <cerrno>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <netinet/tcp.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <fcntl.h>
    #include "../common.h"
    using SOCKET = int32_t;
    #define SOCKET_ERROR -1
    #define INVALID_SOCKET -1
    #define LAST_SOCKET_ERROR() errno
    #define closesocket close
    #define ioctlsocket ioctl
    #if defined(__linux__)
        #define FLAG_NO_PIPE MSG_NOSIGNAL
    #else
        #define FLAG_NO_PIPE 0
    #endif // defined(__linux__)
#endif // _WIN32
// clang-format on

#    include "UdpSocket.h"

constexpr auto CONNECT_TIMEOUT = std::chrono::milliseconds(3000);

#    ifdef _WIN32
static bool _wsaInitialised = false;
#    endif

class UdpSocket;

class SocketException : public std::runtime_error
{
public:
    explicit SocketException(const std::string& message)
        : std::runtime_error(message)
    {
    }
};

class NetworkEndpoint final : public INetworkEndpoint
{
private:
    sockaddr _address{};
    socklen_t _addressLen{};

public:
    NetworkEndpoint()
    {
    }

    NetworkEndpoint(const sockaddr* address, socklen_t addressLen)
    {
        std::memcpy(&_address, address, addressLen);
        _addressLen = addressLen;
    }

    const sockaddr& GetAddress() const
    {
        return _address;
    }

    socklen_t GetAddressLen() const
    {
        return _addressLen;
    }

    int32_t GetPort() const
    {
        if (_address.sa_family == AF_INET)
        {
            return ((sockaddr_in*)&_address)->sin_port;
        }
        else
        {
            return ((sockaddr_in6*)&_address)->sin6_port;
        }
    }

    std::string GetHostname() override
    {
        char hostname[256];
        int res = getnameinfo(&_address, _addressLen, hostname, sizeof(hostname), nullptr, 0, NI_NUMERICHOST);
        if (res == 0)
        {
            return hostname;
        }
        return {};
    }
};

class UdpSocket final : public IUdpSocket
{
private:
    SOCKET_STATUS _status = SOCKET_STATUS_CLOSED;
    uint16_t _listeningPort = 0;
    SOCKET _socket = INVALID_SOCKET;
    NetworkEndpoint _endpoint;

    std::string _hostName;
    std::string _error;

public:
    UdpSocket() = default;

    ~UdpSocket() override
    {
        CloseSocket();
    }

    SOCKET_STATUS GetStatus() override
    {
        return _status;
    }

    const char* GetError() override
    {
        return _error.empty() ? nullptr : _error.c_str();
    }

    void Listen(uint16_t port) override
    {
        Listen("", port);
    }

    void Listen(const std::string& address, uint16_t port) override
    {
        if (_status != SOCKET_STATUS_CLOSED)
        {
            throw std::runtime_error("Socket not closed.");
        }

        sockaddr_storage ss{};
        socklen_t ss_len;
        if (!ResolveAddress(address, port, &ss, &ss_len))
        {
            throw SocketException("Unable to resolve address.");
        }

        // Create the listening socket
        _socket = socket(ss.ss_family, SOCK_DGRAM, IPPROTO_UDP);
        if (_socket == INVALID_SOCKET)
        {
            throw SocketException("Unable to create socket.");
        }

        // Turn off IPV6_V6ONLY so we can accept both v4 and v6 connections
        if (!SetOption(_socket, IPPROTO_IPV6, IPV6_V6ONLY, false))
        {
            log_error("IPV6_V6ONLY failed. %d", LAST_SOCKET_ERROR());
        }

        if (!SetOption(_socket, SOL_SOCKET, SO_REUSEADDR, true))
        {
            log_error("SO_REUSEADDR failed. %d", LAST_SOCKET_ERROR());
        }

        // Enable send and receiving of broadcast messages
        if (!SetOption(_socket, SOL_SOCKET, SO_BROADCAST, true))
        {
            log_error("SO_BROADCAST failed. %d", LAST_SOCKET_ERROR());
        }

        try
        {
            // Bind to address:port and listen
            if (bind(_socket, (sockaddr*)&ss, ss_len) != 0)
            {
                throw SocketException("Unable to bind to socket.");
            }

            if (!SetNonBlocking(_socket, true))
            {
                throw SocketException("Failed to set non-blocking mode.");
            }
        }
        catch (const std::exception&)
        {
            CloseSocket();
            throw;
        }

        _listeningPort = port;
        _status = SOCKET_STATUS_LISTENING;
    }

    size_t SendData(const std::string& address, uint16_t port, const void* buffer, size_t size) override
    {
        sockaddr_storage ss{};
        socklen_t ss_len;
        if (!ResolveAddress(address, port, &ss, &ss_len))
        {
            throw SocketException("Unable to resolve address.");
        }
        NetworkEndpoint endpoint((const sockaddr*)&ss, ss_len);
        return SendData(endpoint, buffer, size);
    }

    size_t SendData(const INetworkEndpoint& destination, const void* buffer, size_t size) override
    {
        if (_socket == INVALID_SOCKET)
        {
            _socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            if (_socket == INVALID_SOCKET)
            {
                throw SocketException("Unable to create socket.");
            }

            // Enable send and receiving of broadcast messages
            if (!SetOption(_socket, SOL_SOCKET, SO_BROADCAST, true))
            {
                log_error("SO_BROADCAST failed. %d", LAST_SOCKET_ERROR());
            }

            if (!SetNonBlocking(_socket, true))
            {
                throw SocketException("Failed to set non-blocking mode.");
            }
        }

        const auto& dest = dynamic_cast<const NetworkEndpoint*>(&destination);
        if (dest == nullptr)
        {
            throw std::invalid_argument("destination is not compatible.");
        }
        auto ss = &dest->GetAddress();
        auto ss_len = dest->GetAddressLen();

        if (_status != SOCKET_STATUS_LISTENING)
        {
            _endpoint = *dest;
        }

        size_t totalSent = 0;
        do
        {
            const char* bufferStart = (const char*)buffer + totalSent;
            size_t remainingSize = size - totalSent;
            int32_t sentBytes = sendto(_socket, bufferStart, (int32_t)remainingSize, FLAG_NO_PIPE, (const sockaddr*)ss, ss_len);
            if (sentBytes == SOCKET_ERROR)
            {
                return totalSent;
            }
            totalSent += sentBytes;
        } while (totalSent < size);
        return totalSent;
    }

    NETWORK_READPACKET ReceiveData(
        void* buffer, size_t size, size_t* sizeReceived, std::unique_ptr<INetworkEndpoint>* sender) override
    {
        sockaddr_in senderAddr{};
        socklen_t senderAddrLen = sizeof(sockaddr_in);
        if (_status != SOCKET_STATUS_LISTENING)
        {
            senderAddrLen = _endpoint.GetAddressLen();
            std::memcpy(&senderAddr, &_endpoint.GetAddress(), senderAddrLen);
        }
        auto readBytes = recvfrom(_socket, (char*)buffer, (int32_t)size, 0, (sockaddr*)&senderAddr, &senderAddrLen);
        if (readBytes <= 0)
        {
            *sizeReceived = 0;
            return NETWORK_READPACKET_NO_DATA;
        }
        else
        {
            *sizeReceived = readBytes;
            if (sender != nullptr)
            {
                *sender = std::make_unique<NetworkEndpoint>((sockaddr*)&senderAddr, senderAddrLen);
            }
            return NETWORK_READPACKET_SUCCESS;
        }
    }

    void Close() override
    {
        CloseSocket();
    }

    const char* GetHostName() const override
    {
        return _hostName.empty() ? nullptr : _hostName.c_str();
    }

private:
    explicit UdpSocket(SOCKET socket, const std::string& hostName)
    {
        _socket = socket;
        _hostName = hostName;
        _status = SOCKET_STATUS_CONNECTED;
    }

    void CloseSocket()
    {
        if (_socket != INVALID_SOCKET)
        {
            closesocket(_socket);
            _socket = INVALID_SOCKET;
        }
        _status = SOCKET_STATUS_CLOSED;
    }

    bool ResolveAddress(const std::string& address, uint16_t port, sockaddr_storage* ss, socklen_t* ss_len)
    {
        std::string serviceName = std::to_string(port);

        addrinfo hints = {};
        hints.ai_family = AF_INET;
        if (address.empty())
        {
            hints.ai_flags = AI_PASSIVE;
        }

        addrinfo* result = nullptr;
        int errorcode = getaddrinfo(address.empty() ? nullptr : address.c_str(), serviceName.c_str(), &hints, &result);
        if (errorcode != 0)
        {
            log_error("Resolving address failed: Code %d.", errorcode);
            log_error("Resolution error message: %s.", gai_strerror(errorcode));
            return false;
        }
        if (result == nullptr)
        {
            return false;
        }
        else
        {
            std::memcpy(ss, result->ai_addr, result->ai_addrlen);
            *ss_len = (socklen_t)result->ai_addrlen;
            freeaddrinfo(result);
            return true;
        }
    }

    static bool SetNonBlocking(SOCKET socket, bool on)
    {
#    ifdef _WIN32
        u_long nonBlocking = on;
        return ioctlsocket(socket, FIONBIO, &nonBlocking) == 0;
#    else
        int32_t flags = fcntl(socket, F_GETFL, 0);
        return fcntl(socket, F_SETFL, on ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK)) == 0;
#    endif
    }

    static bool SetOption(SOCKET socket, int32_t a, int32_t b, bool value)
    {
        int32_t ivalue = value ? 1 : 0;
        return setsockopt(socket, a, b, (const char*)&ivalue, sizeof(ivalue)) == 0;
    }
};

std::unique_ptr<IUdpSocket> CreateUdpSocket()
{
    return std::make_unique<UdpSocket>();
}

#endif
