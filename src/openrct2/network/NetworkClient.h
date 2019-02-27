/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
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
    virtual NETWORK_MODE GetMode() const override
    {
        return NETWORK_MODE::CLIENT;
    }

    virtual bool Startup() override;
    virtual bool Shutdown() override;

    virtual void Close() override;
    virtual void Update() override;
    virtual void Flush() override;

    bool Connect(const std::string& host, int32_t port);

private:
    bool UpdateConnecting();
    bool UpdateConnection();

private:
    // Connection Handlers.
    void HandleSocketResolving();
    void HandleSocketConnecting();
    void HandleSocketConnected();
    void HandleSocketError();

private:
    void RequestToken();

private:
    std::unique_ptr<NetworkConnection> _connection;
    SOCKET_STATUS _lastConnectStatus;
};
