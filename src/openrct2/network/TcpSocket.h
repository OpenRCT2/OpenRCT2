#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

    virtual void         Listen(uint16 port)                       abstract;
    virtual void         Listen(const char * address, uint16 port) abstract;
    virtual ITcpSocket * Accept()                                  abstract;

    virtual void Connect(const char * address, uint16 port)      abstract;
    virtual void ConnectAsync(const char * address, uint16 port) abstract;

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
    uint16 HostToNetwork(uint16 value);
    uint16 NetworkToHost(uint16 value);
}
