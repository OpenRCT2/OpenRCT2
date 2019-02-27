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

interface Network : public INetwork
{
public:
    Network();
    virtual ~Network() override;

    void Startup();
    void Shutdown();
    void Update();

    bool BeginServer(const std::string& host, uint16_t port);
    bool BeginClient(const std::string& host, uint16_t port);
    void Close();

    NETWORK_MODE GetMode() const;

private:
    std::unique_ptr<NetworkBase> _networkImpl;
};

std::unique_ptr<INetwork> INetwork::Create()
{
    return std::make_unique<Network>();
}

Network::Network()
    : _networkImpl(std::make_unique<NetworkStub>())
{
}

Network::~Network()
{
}

void Network::Startup()
{
}

void Network::Shutdown()
{
}

void Network::Update()
{
    _networkImpl->Update();
}

bool Network::BeginServer(const std::string& host, uint16_t port)
{
    auto serverImpl = std::make_unique<NetworkServer>();

    if (serverImpl->Startup() == false)
    {
        log_error("Unable to initialize server.");
        return false;
    }

    if (serverImpl->Listen(host, port) == false)
    {
        log_error("Unable to start server");
        return false;
    }

    _networkImpl = std::move(serverImpl);

    return true;
}

bool Network::BeginClient(const std::string& host, uint16_t port)
{
    auto clientImpl = std::make_unique<NetworkClient>();

    if (clientImpl->Startup() == false)
    {
        log_error("Unable to startup network client.");
        return false;
    }

    if (clientImpl->Connect(host, port) == false)
    {
        log_error("Failed to initiate connection to remote host.");
        return false;
    }

    _networkImpl = std::move(clientImpl);

    return true;
}

void Network::Close()
{
    if (_networkImpl)
    {
        _networkImpl->Close();
    }
    _networkImpl = std::make_unique<NetworkStub>();
}

NETWORK_MODE Network::GetMode() const
{
    return _networkImpl->GetMode();
}
