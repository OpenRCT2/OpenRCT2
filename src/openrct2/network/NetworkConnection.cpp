/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "NetworkConnection.h"

#include "../core/String.hpp"
#include "../localisation/Localisation.h"
#include "../platform/platform.h"
#include "TcpSocket.h"
#include "network.h"

NetworkConnection::NetworkConnection()
{
    _sock = ITcpSocket::Create();
}

NetworkConnection::NetworkConnection(std::unique_ptr<ITcpSocket>&& sock)
    : _sock(std::move(sock))
{
}

NetworkConnection::~NetworkConnection()
{
}

void NetworkConnection::Close()
{
    _sock->Close();
}

void NetworkConnection::Flush()
{
}

SOCKET_STATUS NetworkConnection::GetStatus() const
{
    return _sock->GetStatus();
}

void NetworkConnection::ConnectAsync(const std::string& host, uint16_t port)
{
    _sock->ConnectAsync(host, port);
}

const char* NetworkConnection::GetError() const
{
    return _sock->GetError();
}
