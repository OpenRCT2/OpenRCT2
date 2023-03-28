/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../OpenRCT2.h"
#include "../core/Console.hpp"
#include "../core/String.hpp"
#include "../network/network.h"
#include "CommandLine.hpp"

static exitcode_t HandleUri(const std::string& uri);

#ifndef DISABLE_NETWORK
static exitcode_t HandleUriJoin(const std::vector<std::string>& args);
static bool TryParseHostnamePort(
    const std::string& hostnamePort, std::string* outHostname, int32_t* outPort, int32_t defaultPort);
#endif

exitcode_t CommandLine::HandleCommandUri(CommandLineArgEnumerator* enumerator)
{
    const utf8* uri;
    if (enumerator->TryPopString(&uri))
    {
        if (String::StartsWith(uri, "openrct2://"))
        {
            const utf8* uriCommand = uri + 11;
            return HandleUri(uriCommand);
        }
    }

    Console::Error::WriteLine("Invalid URI");
    return EXITCODE_FAIL;
}

static exitcode_t HandleUri(const std::string& uri)
{
    exitcode_t result = EXITCODE_CONTINUE;
    auto args = String::Split(uri, "/");
    if (!args.empty())
    {
#ifndef DISABLE_NETWORK
        std::string arg = args[0];
        if (arg == "join")
        {
            result = HandleUriJoin(args);
        }
#endif
    }
    return result;
}

#ifndef DISABLE_NETWORK

static exitcode_t HandleUriJoin(const std::vector<std::string>& args)
{
    std::string hostname;
    int32_t port;
    if (args.size() > 1 && TryParseHostnamePort(args[1], &hostname, &port, NETWORK_DEFAULT_PORT))
    {
        // Set the network start configuration
        gNetworkStart = NETWORK_MODE_CLIENT;
        gNetworkStartHost = std::move(hostname);
        gNetworkStartPort = port;
        return EXITCODE_CONTINUE;
    }

    Console::Error::WriteLine("Expected hostname:port after join");
    return EXITCODE_FAIL;
}

static bool TryParseHostnamePort(
    const std::string& hostnamePort, std::string* outHostname, int32_t* outPort, int32_t defaultPort)
{
    try
    {
        // Argument is in hostname:port format, so we need to split
        std::string hostname = hostnamePort;
        int32_t port = defaultPort;
        size_t colonIndex = hostnamePort.find_first_of(':');
        if (colonIndex != std::string::npos)
        {
            hostname = hostnamePort.substr(0, colonIndex);
            port = std::stoi(hostnamePort.substr(colonIndex + 1));
        }
        *outPort = port;
        *outHostname = std::move(hostname);
        return true;
    }
    catch (const std::exception&)
    {
        return false;
    }
}

#endif // DISABLE_NETWORK
