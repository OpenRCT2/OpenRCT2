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
#include "NetworkUser.h"
#include "TcpSocket.h"

#include <list>
#include <memory>

class NetworkServer : public NetworkBase
{
public:
    NetworkServer();

    virtual ~NetworkServer();

    virtual NETWORK_MODE GetMode() const override
    {
        return NETWORK_MODE::HOST;
    }

    virtual bool Startup() override;
    virtual bool Shutdown() override;

    virtual void Close() override;
    virtual void Update() override;
    virtual void Flush() override;

    bool Listen(uint16_t port, const char* address);

private:
    void UpdateServer();
    void UpdateClients();
    bool UpdateClient(std::unique_ptr<NetworkConnection>& client);

    void LoadConfiguration();
    void AcceptClients();
    void AcceptClient(std::unique_ptr<ITcpSocket>&& sock);
    void RemoveClient(std::unique_ptr<NetworkConnection>& connection);

private:
    // Packet Handlers.
    void HandleRequestToken(NetworkConnection& connection, const NetworkPacketRequestToken& packet);

private:
    NetworkUserManager _userManager;

    std::unique_ptr<ITcpSocket> _listenSocket;
    std::list<std::unique_ptr<NetworkConnection>> _clients;
};
