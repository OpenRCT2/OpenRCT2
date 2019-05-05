/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

#include <memory>
#include <string>

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
 * Represents an address and port.
 */
interface INetworkEndpoint
{
    virtual ~INetworkEndpoint()
    {
    }

    virtual std::string GetHostname() abstract;
};

/**
 * Represents a UDP socket / listener.
 */
interface IUdpSocket
{
public:
    virtual ~IUdpSocket()
    {
    }

    virtual SOCKET_STATUS GetStatus() abstract;
    virtual const char* GetError() abstract;
    virtual const char* GetHostName() const abstract;

    virtual void Listen(uint16_t port) abstract;
    virtual void Listen(const std::string& address, uint16_t port) abstract;

    virtual size_t SendData(const std::string& address, uint16_t port, const void* buffer, size_t size) abstract;
    virtual size_t SendData(const INetworkEndpoint& destination, const void* buffer, size_t size) abstract;
    virtual NETWORK_READPACKET ReceiveData(
        void* buffer, size_t size, size_t* sizeReceived, std::unique_ptr<INetworkEndpoint>* sender) abstract;

    virtual void Close() abstract;
};

std::unique_ptr<IUdpSocket> CreateUdpSocket();
