/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

enum SOCKET_STATUS
{
    SOCKET_STATUS_CLOSED,
    SOCKET_STATUS_RESOLVING,
    SOCKET_STATUS_CONNECTING,
    SOCKET_STATUS_CONNECTED,
    SOCKET_STATUS_LISTENING,
};

enum NETWORK_READPACKET
{
    NETWORK_READPACKET_SUCCESS,
    NETWORK_READPACKET_NO_DATA,
    NETWORK_READPACKET_MORE_DATA,
    NETWORK_READPACKET_DISCONNECTED
};

/**
 * Represents a TCP socket / connection or listener.
 */
interface ITcpSocket
{
public:
    virtual ~ITcpSocket() { }

    virtual SOCKET_STATUS   GetStatus() abstract;
    virtual const char *    GetError() abstract;
    virtual const char *    GetHostName() const abstract;

    virtual void         Listen(uint16_t port)                       abstract;
    virtual void         Listen(const char * address, uint16_t port) abstract;
    virtual ITcpSocket * Accept()                                  abstract;

    virtual void Connect(const char * address, uint16_t port)      abstract;
    virtual void ConnectAsync(const char * address, uint16_t port) abstract;

    virtual size_t             SendData(const void * buffer, size_t size)                     abstract;
    virtual NETWORK_READPACKET ReceiveData(void * buffer, size_t size, size_t * sizeReceived) abstract;

    virtual void Disconnect() abstract;
    virtual void Close() abstract;
};

ITcpSocket * CreateTcpSocket();

bool InitialiseWSA();
void DisposeWSA();

namespace Convert
{
    uint16_t HostToNetwork(uint16_t value);
    uint16_t NetworkToHost(uint16_t value);
}
