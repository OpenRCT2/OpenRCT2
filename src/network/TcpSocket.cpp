#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#ifndef DISABLE_NETWORK

// MSVC: include <math.h> here otherwise PI gets defined twice
#include <math.h>

#include <SDL_platform.h>
#include <SDL_thread.h>
#include <SDL_timer.h>

#ifdef __WINDOWS__
    // winsock2 must be included before windows.h
    #include <winsock2.h>
    #include <ws2tcpip.h>

    #undef GetMessage
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
    #include <errno.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <netinet/tcp.h>
    #include <sys/socket.h>
    #include <fcntl.h>
    typedef int SOCKET;
    #define SOCKET_ERROR -1
    #define INVALID_SOCKET -1
    #define LAST_SOCKET_ERROR() errno
    #define closesocket close
    #define ioctlsocket ioctl
    #if defined(__LINUX__)
        #define FLAG_NO_PIPE MSG_NOSIGNAL
    #else
        #define FLAG_NO_PIPE 0
    #endif // defined(__LINUX__)
#endif // __WINDOWS__

#include "../core/Exception.hpp"
#include "TcpSocket.h"

constexpr uint32 CONNECT_TIMEOUT_MS = 3000;

class TcpSocket;

class SocketException : public Exception
{
public:
    SocketException(const char * message) : Exception(message) { }
    SocketException(const std::string &message) : Exception(message) { }
};

struct ConnectRequest
{
    TcpSocket * Socket;
    std::string Address;
    uint16      Port;
};

class TcpSocket : public ITcpSocket
{
private:
    SOCKET_STATUS   _status         = SOCKET_STATUS_CLOSED;
    uint16          _listeningPort  = 0;
    SOCKET          _socket         = INVALID_SOCKET;

    SDL_mutex *     _connectMutex   = nullptr;
    std::string     _error;

public:
    TcpSocket()
    {
        _connectMutex = SDL_CreateMutex();
    }

    ~TcpSocket() override
    {
        SDL_LockMutex(_connectMutex);
        {
            CloseSocket();
        }
        SDL_UnlockMutex(_connectMutex);
        SDL_DestroyMutex(_connectMutex);
    }

    SOCKET_STATUS GetStatus() override
    {
        return _status;
    }

    const char * GetError() override
    {
        return _error.empty() ? nullptr : _error.c_str();
    }

    void Listen(uint16 port) override
    {
        Listen(nullptr, port);
    }

    void Listen(const char * address, uint16 port) override
    {
        if (_status != SOCKET_STATUS_CLOSED)
        {
            throw Exception("Socket not closed.");
        }

        sockaddr_storage ss;
        int ss_len;
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
        int value = 0;
        if (setsockopt(_socket, IPPROTO_IPV6, IPV6_V6ONLY, (const char*)&value, sizeof(value)) != 0)
        {
            log_error("IPV6_V6ONLY failed. %d", LAST_SOCKET_ERROR());
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
        catch (Exception ex)
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
            throw Exception("Socket not listening.");
        }

        ITcpSocket * tcpSocket = nullptr;
        SOCKET socket = accept(_socket, nullptr, nullptr);
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
                SetTCPNoDelay(socket, true);
                tcpSocket = new TcpSocket(socket);
            }
        }
        return tcpSocket;
    }

    void Connect(const char * address, uint16 port) override
    {
        if (_status != SOCKET_STATUS_CLOSED)
        {
            throw Exception("Socket not closed.");
        }
        
        try
        {
            // Resolve address
            _status = SOCKET_STATUS_RESOLVING;

            sockaddr_storage ss;
            int ss_len;
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
            uint32 connectStartTick;
            int connectResult = connect(_socket, (sockaddr *)&ss, ss_len);
            if (connectResult != SOCKET_ERROR || (LAST_SOCKET_ERROR() != EINPROGRESS &&
                                                  LAST_SOCKET_ERROR() != EWOULDBLOCK))
            {
                throw SocketException("Failed to connect.");
            }

            connectStartTick = SDL_GetTicks();

            int error = 0;
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
                SDL_Delay(100);

                fd_set writeFD;
                FD_ZERO(&writeFD);
                FD_SET(_socket, &writeFD);
                timeval timeout;
                timeout.tv_sec = 0;
                timeout.tv_usec = 0;
                if (select((int)(_socket + 1), nullptr, &writeFD, nullptr, &timeout) > 0)
                {
                    error = 0;
                    socklen_t len = sizeof(error);
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
            } while (!SDL_TICKS_PASSED(SDL_GetTicks(), connectStartTick + CONNECT_TIMEOUT_MS));

            // Connection request timed out
            throw SocketException("Connection timed out.");
        }
        catch (Exception ex)
        {
            CloseSocket();
            throw;
        }
    }

    void ConnectAsync(const char * address, uint16 port) override
    {
        if (_status != SOCKET_STATUS_CLOSED)
        {
            throw Exception("Socket not closed.");
        }

        if (SDL_TryLockMutex(_connectMutex) == 0)
        {
            // Spin off a worker thread for resolving the address
            auto req = new ConnectRequest();
            req->Socket = this;
            req->Address = std::string(address);
            req->Port = port;
            SDL_CreateThread([](void * pointer) -> int
            {
                auto req = (ConnectRequest *)pointer;
                try
                {
                    req->Socket->Connect(req->Address.c_str(), req->Port);
                }
                catch (Exception ex)
                {
                    req->Socket->_error = std::string(ex.GetMessage());
                }

                SDL_UnlockMutex(req->Socket->_connectMutex);
                delete req;
                return 0;
            }, 0, req);
        }
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
            throw Exception("Socket not connected.");
        }

        size_t totalSent = 0;
        do
        {
            const char * bufferStart = (const char *)buffer + totalSent;
            size_t remainingSize = size - totalSent;
            int sentBytes = send(_socket, (const char *)bufferStart, (int)remainingSize, FLAG_NO_PIPE);
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
            throw Exception("Socket not connected.");
        }

        int readBytes = recv(_socket, (char *)buffer, (int)size, 0);
        if (readBytes == 0)
        {
            *sizeReceived = 0;
            return NETWORK_READPACKET_DISCONNECTED;
        }
        else if (readBytes == SOCKET_ERROR)
        {
            *sizeReceived = 0;
            if (LAST_SOCKET_ERROR() != EWOULDBLOCK && LAST_SOCKET_ERROR() != EAGAIN)
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
        SDL_LockMutex(_connectMutex);
        {
            CloseSocket();
        }
        SDL_UnlockMutex(_connectMutex);
    }

private:
    TcpSocket(SOCKET socket)
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

    bool ResolveAddress(const char * address, uint16 port, sockaddr_storage * ss, int * ss_len)
    {
        std::string serviceName = std::to_string(port);

        addrinfo hints = { 0 };
        hints.ai_family = AF_UNSPEC;
        if (address == nullptr)
        {
            hints.ai_flags = AI_PASSIVE;
        }

        addrinfo * result;
        getaddrinfo(address, serviceName.c_str(), &hints, &result);
        if (result == nullptr)
        {
            return false;
        }
        else
        {
            memcpy(ss, result->ai_addr, result->ai_addrlen);
            *ss_len = (int)result->ai_addrlen;
            freeaddrinfo(result);
            return true;
        }
    }

    static bool SetNonBlocking(SOCKET socket, bool on)
    {
#ifdef __WINDOWS__
        u_long nonBlocking = on;
        return ioctlsocket(socket, FIONBIO, &nonBlocking) == 0;
#else
        int flags = fcntl(socket, F_GETFL, 0);
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

#endif
