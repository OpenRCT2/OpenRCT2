/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_NETWORK

#include <cmath>
#include <chrono>
#include <cstring>
#include <future>
#include <string>
#include <thread>

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

#include "TcpSocket.h"

constexpr auto CONNECT_TIMEOUT = std::chrono::milliseconds(3000);

#ifdef _WIN32
    static bool _wsaInitialised = false;
#endif

class TcpSocket;

class SocketException : public std::runtime_error
{
public:
    explicit SocketException(const std::string &message) : std::runtime_error(message) { }
};

class TcpSocket final : public ITcpSocket
{
private:
    SOCKET_STATUS   _status         = SOCKET_STATUS_CLOSED;
    uint16_t          _listeningPort  = 0;
    SOCKET          _socket         = INVALID_SOCKET;

    std::string         _hostName;
    std::future<void>   _connectFuture;
    std::string         _error;

public:
    TcpSocket() = default;

    ~TcpSocket() override
    {
        if (_connectFuture.valid())
        {
            _connectFuture.wait();
        }
        CloseSocket();
    }

    SOCKET_STATUS GetStatus() override
    {
        return _status;
    }

    const char * GetError() override
    {
        return _error.empty() ? nullptr : _error.c_str();
    }

    void Listen(uint16_t port) override
    {
        Listen(nullptr, port);
    }

    void Listen(const char * address, uint16_t port) override
    {
        if (_status != SOCKET_STATUS_CLOSED)
        {
            throw std::runtime_error("Socket not closed.");
        }

        sockaddr_storage ss { };
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
            if (bind(_socket, (sockaddr *)&ss, ss_len) != 0)
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
        catch (const std::exception &)
        {
            CloseSocket();
            throw;
        }

        _listeningPort = port;
        _status = SOCKET_STATUS_LISTENING;
    }

    ITcpSocket * Accept() override
    {
        if (_status != SOCKET_STATUS_LISTENING)
        {
            throw std::runtime_error("Socket not listening.");
        }
        struct sockaddr_storage client_addr { };
        socklen_t client_len = sizeof(struct sockaddr_storage);

        ITcpSocket * tcpSocket = nullptr;
        SOCKET socket = accept(_socket, (struct sockaddr *)&client_addr, &client_len);
        if (socket == INVALID_SOCKET)
        {
            if (LAST_SOCKET_ERROR() != EWOULDBLOCK)
            {
                log_error("Failed to accept client.");
            }
        }
        else
        {
            if (!SetNonBlocking(socket, true))
            {
                closesocket(socket);
                log_error("Failed to set non-blocking mode.");
            }
            else
            {
                char hostName[NI_MAXHOST];
                int32_t rc = getnameinfo(
                    (struct sockaddr *)&client_addr,
                    client_len,
                    hostName,
                    sizeof(hostName),
                    nullptr,
                    0,
                    NI_NUMERICHOST | NI_NUMERICSERV);
                SetTCPNoDelay(socket, true);
                tcpSocket = new TcpSocket(socket);
                if (rc == 0)
                {
                    _hostName = std::string(hostName);
                }
            }
        }
        return tcpSocket;
    }

    void Connect(const char * address, uint16_t port) override
    {
        if (_status != SOCKET_STATUS_CLOSED)
        {
            throw std::runtime_error("Socket not closed.");
        }

        try
        {
            // Resolve address
            _status = SOCKET_STATUS_RESOLVING;

            sockaddr_storage ss { };
            int32_t ss_len;
            if (!ResolveAddress(address, port, &ss, &ss_len))
            {
                throw SocketException("Unable to resolve address.");
            }

            _status = SOCKET_STATUS_CONNECTING;
            _socket = socket(ss.ss_family, SOCK_STREAM, IPPROTO_TCP);
            if (_socket == INVALID_SOCKET)
            {
                throw SocketException("Unable to create socket.");
            }

            SetTCPNoDelay(_socket, true);
            if (!SetNonBlocking(_socket, true))
            {
                throw SocketException("Failed to set non-blocking mode.");
            }

            // Connect
            int32_t connectResult = connect(_socket, (sockaddr *)&ss, ss_len);
            if (connectResult != SOCKET_ERROR || (LAST_SOCKET_ERROR() != EINPROGRESS &&
                                                  LAST_SOCKET_ERROR() != EWOULDBLOCK))
            {
                throw SocketException("Failed to connect.");
            }

            auto connectStartTime = std::chrono::system_clock::now();

            int32_t error = 0;
            socklen_t len = sizeof(error);
            if (getsockopt(_socket, SOL_SOCKET, SO_ERROR, (char *)&error, &len) != 0)
            {
                throw SocketException("getsockopt failed with error: " + std::to_string(LAST_SOCKET_ERROR()));
            }
            if (error != 0)
            {
                throw SocketException("Connection failed: " + std::to_string(error));
            }

            do
            {
                // Sleep for a bit
                std::this_thread::sleep_for(std::chrono::milliseconds(100));

                fd_set writeFD;
                FD_ZERO(&writeFD);
#pragma warning(push)
#pragma warning(disable : 4548) // expression before comma has no effect; expected expression with side-effect
                FD_SET(_socket, &writeFD);
#pragma warning(pop)
                timeval timeout { };
                timeout.tv_sec = 0;
                timeout.tv_usec = 0;
                if (select((int32_t)(_socket + 1), nullptr, &writeFD, nullptr, &timeout) > 0)
                {
                    error = 0;
                    len = sizeof(error);
                    if (getsockopt(_socket, SOL_SOCKET, SO_ERROR, (char*)&error, &len) != 0)
                    {
                        throw SocketException("getsockopt failed with error: " + std::to_string(LAST_SOCKET_ERROR()));
                    }
                    if (error == 0)
                    {
                        _status = SOCKET_STATUS_CONNECTED;
                        return;
                    }
                }
            } while ((std::chrono::system_clock::now() - connectStartTime) < CONNECT_TIMEOUT);

            // Connection request timed out
            throw SocketException("Connection timed out.");
        }
        catch (const std::exception &)
        {
            CloseSocket();
            throw;
        }
    }

    void ConnectAsync(const char * address, uint16_t port) override
    {
        if (_status != SOCKET_STATUS_CLOSED)
        {
            throw std::runtime_error("Socket not closed.");
        }

        auto saddress = std::string(address);
        std::promise<void> barrier;
        _connectFuture = barrier.get_future();
        auto thread = std::thread([this, saddress, port](std::promise<void> barrier2) -> void
        {
            try
            {
                Connect(saddress.c_str(), port);
            }
            catch (const std::exception &ex)
            {
                _error = std::string(ex.what());
            }
            barrier2.set_value();
        }, std::move(barrier));
        thread.detach();
    }

    void Disconnect() override
    {
        if (_status == SOCKET_STATUS_CONNECTED)
        {
            shutdown(_socket, SHUT_RDWR);
        }
    }

    size_t SendData(const void * buffer, size_t size) override
    {
        if (_status != SOCKET_STATUS_CONNECTED)
        {
            throw std::runtime_error("Socket not connected.");
        }

        size_t totalSent = 0;
        do
        {
            const char * bufferStart = (const char *)buffer + totalSent;
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

    NETWORK_READPACKET ReceiveData(void * buffer, size_t size, size_t * sizeReceived) override
    {
        if (_status != SOCKET_STATUS_CONNECTED)
        {
            throw std::runtime_error("Socket not connected.");
        }

        int32_t readBytes = recv(_socket, (char *)buffer, (int32_t)size, 0);
        if (readBytes == 0)
        {
            *sizeReceived = 0;
            return NETWORK_READPACKET_DISCONNECTED;
        }
        else if (readBytes == SOCKET_ERROR)
        {
            *sizeReceived = 0;
#ifndef _WIN32
            // Removing the check for EAGAIN and instead relying on the values being the same allows turning on of
            // -Wlogical-op warning.
            // This is not true on Windows, see:
            // * https://msdn.microsoft.com/en-us/library/windows/desktop/ms737828(v=vs.85).aspx
            // * https://msdn.microsoft.com/en-us/library/windows/desktop/ms741580(v=vs.85).aspx
            // * https://msdn.microsoft.com/en-us/library/windows/desktop/ms740668(v=vs.85).aspx
            static_assert(EWOULDBLOCK == EAGAIN, "Portability note: your system has different values for EWOULDBLOCK "
                    "and EAGAIN, please extend the condition below");
#endif // _WIN32
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
        if (_connectFuture.valid())
        {
            _connectFuture.wait();
        }
        CloseSocket();
    }

    const char * GetHostName() const override
    {
        return _hostName.empty() ? nullptr : _hostName.c_str();
    }

private:
    explicit TcpSocket(SOCKET socket)
    {
        _socket = socket;
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

    bool ResolveAddress(const char * address, uint16_t port, sockaddr_storage * ss, int32_t * ss_len)
    {
        std::string serviceName = std::to_string(port);

        addrinfo hints = {};
        hints.ai_family = AF_UNSPEC;
        if (address == nullptr)
        {
            hints.ai_flags = AI_PASSIVE;
        }

        addrinfo * result = nullptr;
        int errorcode = getaddrinfo(address, serviceName.c_str(), &hints, &result);
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
            memcpy(ss, result->ai_addr, result->ai_addrlen);
            *ss_len = (int32_t)result->ai_addrlen;
            freeaddrinfo(result);
            return true;
        }
    }

    static bool SetNonBlocking(SOCKET socket, bool on)
    {
#ifdef _WIN32
        u_long nonBlocking = on;
        return ioctlsocket(socket, FIONBIO, &nonBlocking) == 0;
#else
        int32_t flags = fcntl(socket, F_GETFL, 0);
        return fcntl(socket, F_SETFL, on ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK)) == 0;
#endif
    }

    static bool SetTCPNoDelay(SOCKET socket, bool enabled)
    {
        return setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&enabled, sizeof(enabled)) == 0;
    }
};

ITcpSocket * CreateTcpSocket()
{
    return new TcpSocket();
}

bool InitialiseWSA()
{
#ifdef _WIN32
    if (!_wsaInitialised)
    {
        log_verbose("Initialising WSA");
        WSADATA wsa_data;
        if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
        {
            log_error("Unable to initialise winsock.");
            return false;
        }
        _wsaInitialised = true;
    }
    return _wsaInitialised;
#else
    return true;
#endif
}

void DisposeWSA()
{
#ifdef _WIN32
    if (_wsaInitialised)
    {
        WSACleanup();
        _wsaInitialised = false;
    }
#endif
}

namespace Convert
{
    uint16_t HostToNetwork(uint16_t value)
    {
        return htons(value);
    }

    uint16_t NetworkToHost(uint16_t value)
    {
        return ntohs(value);
    }
} // namespace Convert

#endif
