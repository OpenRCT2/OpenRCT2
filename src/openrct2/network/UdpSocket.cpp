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

class UdpSocket final : public IUdpSocket
{
private:
    SOCKET_STATUS _status = SOCKET_STATUS_CLOSED;
    uint16_t _listeningPort = 0;
    SOCKET _socket = INVALID_SOCKET;

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
        int32_t ss_len;
        if (!ResolveAddress(address, port, &ss, &ss_len))
        {
            throw SocketException("Unable to resolve address.");
        }

        // Create the listening socket
        _socket = socket(ss.ss_family, SOCK_STREAM, IPPROTO_TCP);
        if (_socket == INVALID_SOCKET)
        {
            throw SocketException("Unable to create socket.");
        }

        // Turn off IPV6_V6ONLY so we can accept both v4 and v6 connections
        int32_t value = 0;
        if (setsockopt(_socket, IPPROTO_IPV6, IPV6_V6ONLY, (const char*)&value, sizeof(value)) != 0)
        {
            log_error("IPV6_V6ONLY failed. %d", LAST_SOCKET_ERROR());
        }

        value = 1;
        if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&value, sizeof(value)) != 0)
        {
            log_error("SO_REUSEADDR failed. %d", LAST_SOCKET_ERROR());
        }

        try
        {
            // Bind to address:port and listen
            if (bind(_socket, (sockaddr*)&ss, ss_len) != 0)
            {
                throw SocketException("Unable to bind to socket.");
            }
            if (listen(_socket, SOMAXCONN) != 0)
            {
                throw SocketException("Unable to listen on socket.");
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
        if (_status != SOCKET_STATUS_CONNECTED)
        {
            throw std::runtime_error("Socket not connected.");
        }

        size_t totalSent = 0;
        do
        {
            const char* bufferStart = (const char*)buffer + totalSent;
            size_t remainingSize = size - totalSent;
            int32_t sentBytes = send(_socket, bufferStart, (int32_t)remainingSize, FLAG_NO_PIPE);
            if (sentBytes == SOCKET_ERROR)
            {
                return totalSent;
            }
            totalSent += sentBytes;
        } while (totalSent < size);
        return totalSent;
    }

    NETWORK_READPACKET ReceiveData(void* buffer, size_t size, size_t* sizeReceived) override
    {
        if (_status != SOCKET_STATUS_CONNECTED)
        {
            throw std::runtime_error("Socket not connected.");
        }

        int32_t readBytes = recv(_socket, (char*)buffer, (int32_t)size, 0);
        if (readBytes == 0)
        {
            *sizeReceived = 0;
            return NETWORK_READPACKET_DISCONNECTED;
        }
        else if (readBytes == SOCKET_ERROR)
        {
            *sizeReceived = 0;
#    ifndef _WIN32
            // Removing the check for EAGAIN and instead relying on the values being the same allows turning on of
            // -Wlogical-op warning.
            // This is not true on Windows, see:
            // * https://msdn.microsoft.com/en-us/library/windows/desktop/ms737828(v=vs.85).aspx
            // * https://msdn.microsoft.com/en-us/library/windows/desktop/ms741580(v=vs.85).aspx
            // * https://msdn.microsoft.com/en-us/library/windows/desktop/ms740668(v=vs.85).aspx
            static_assert(
                EWOULDBLOCK == EAGAIN,
                "Portability note: your system has different values for EWOULDBLOCK "
                "and EAGAIN, please extend the condition below");
#    endif // _WIN32
            if (LAST_SOCKET_ERROR() != EWOULDBLOCK)
            {
                return NETWORK_READPACKET_DISCONNECTED;
            }
            else
            {
                return NETWORK_READPACKET_NO_DATA;
            }
        }
        else
        {
            *sizeReceived = readBytes;
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

    bool ResolveAddress(const std::string& address, uint16_t port, sockaddr_storage* ss, int32_t* ss_len)
    {
        std::string serviceName = std::to_string(port);

        addrinfo hints = {};
        hints.ai_family = AF_UNSPEC;
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
            *ss_len = (int32_t)result->ai_addrlen;
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

    static bool SetSOBroadcast(SOCKET socket, bool enabled)
    {
        return setsockopt(socket, SOL_SOCKET, SO_BROADCAST, (const char*)&enabled, sizeof(enabled)) == 0;
    }
};

std::unique_ptr<IUdpSocket> CreateUdpSocket()
{
    return std::make_unique<UdpSocket>();
}

#endif
