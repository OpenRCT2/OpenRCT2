/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "NetworkBase.h"
#include "TcpSocket.h"

class NetworkClient : public NetworkBase
{
public:
    virtual bool Startup() override;
    virtual bool Shutdown() override;

    virtual void Close() override;
    virtual void Update() override;
    virtual void Flush() override;

    bool Connect(const char* host, int32_t port);

private:
    bool UpdateConnecting();
    bool UpdateConnection();

private:
    void HandleSocketResolving();
    void HandleSocketConnecting();
    void HandleSocketConnected();
	void HandleSocketError();

private:
    void RequestToken();

private:
    std::unique_ptr<NetworkConnection> _connection;

    int32_t _lastConnectStatus = SOCKET_STATUS_CLOSED;
};