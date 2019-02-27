/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#define NETWORK_DEFAULT_PORT 11753
#define MAX_SERVER_DESCRIPTION_LENGTH 256

#ifdef _WIN32
#    pragma comment(lib, "Ws2_32.lib")
#endif

#include "../common.h"
#include "../localisation/StringIds.h"
#include "NetworkClient.h"
#include "NetworkLegacy.h"
#include "NetworkServer.h"
#include "NetworkTypes.h"

#include <memory>
#include <string>

namespace OpenRCT2
{
    interface IPlatformEnvironment;
}

interface INetwork
{
public:
    static std::unique_ptr<INetwork> Create();

    virtual ~INetwork()
    {
    }

    virtual void Startup() = 0;
    virtual void Shutdown() = 0;
    virtual void Update() = 0;

    virtual bool BeginServer(const std::string& host, uint16_t port) = 0;
    virtual bool BeginClient(const std::string& host, uint16_t port) = 0;
    virtual void Close() = 0;

    virtual NETWORK_MODE GetMode() const = 0;
};
