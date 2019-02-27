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

#include <memory>
#include <string>

enum class SOCKET_STATUS : uint8_t
{
    CLOSED,
    RESOLVING,
    CONNECTING,
    CONNECTED,
    LISTENING,
};

enum class NETWORK_READPACKET : uint8_t
{
    SUCCESS,
    FAILURE,
    MORE_DATA,
    DISCONNECTED
};

/**
 * Represents a TCP socket / connection or listener.
 */
interface ITcpSocket
{
public:
    static std::unique_ptr<ITcpSocket> Create();

    virtual ~ITcpSocket()
    {
    }

    virtual SOCKET_STATUS GetStatus() abstract;
    virtual const char* GetError() abstract;
    virtual const char* GetHostName() const abstract;

    virtual void Listen(uint16_t port) abstract;
    virtual void Listen(const std::string& address, uint16_t port) abstract;
    virtual std::unique_ptr<ITcpSocket> Accept() abstract;

    virtual void Connect(const std::string& address, uint16_t port) abstract;
    virtual void ConnectAsync(const std::string& address, uint16_t port) abstract;

    virtual size_t SendData(const void* buffer, size_t size) abstract;
    virtual NETWORK_READPACKET ReceiveData(void* buffer, size_t size, size_t* sizeReceived) abstract;

    virtual void Disconnect() abstract;
    virtual void Close() abstract;
};

bool InitialiseWSA();
void DisposeWSA();

namespace Convert
{
    uint16_t HostToNetwork(uint16_t value);
    uint16_t NetworkToHost(uint16_t value);
} // namespace Convert
