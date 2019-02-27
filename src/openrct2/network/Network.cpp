/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Network.h"

#include "NetworkStub.h"

Network::Network()
    : _networkImpl(std::make_unique<NetworkStub>())
{
}

void Network::Startup()
{
}

void Network::Shutdown()
{
}

bool Network::BeginServer(const std::string& host, uint16_t port)
{
    return true;
}

bool Network::BeginClient(const std::string& host, uint16_t port)
{
    return true;
}

void Network::Close()
{
}

NETWORK_MODE Network::GetMode() const
{
    return _networkImpl->GetMode();
}
