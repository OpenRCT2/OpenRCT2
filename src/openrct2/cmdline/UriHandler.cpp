#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../core/Console.hpp"
#include "../core/Memory.hpp"
#include "../core/String.hpp"
#include "../network/network.h"
#include "../OpenRCT2.h"
#include "CommandLine.hpp"

static exitcode_t HandleUri(const std::string &uri);

#ifndef DISABLE_NETWORK
static exitcode_t HandleUriJoin(const std::vector<std::string> &args);
static bool TryParseHostnamePort(const std::string &hostnamePort, std::string * outHostname, sint32 * outPort, sint32 defaultPort);
#endif

exitcode_t CommandLine::HandleCommandUri(CommandLineArgEnumerator * enumerator)
{
    const utf8 * uri;
    if (enumerator->TryPopString(&uri))
    {
        if (String::StartsWith(uri, "openrct2://"))
        {
            const utf8 * uriCommand = uri + 11;
            return HandleUri(uriCommand);
        }
    }

    Console::Error::WriteLine("Invalid URI");
    return EXITCODE_FAIL;
}

static exitcode_t HandleUri(const std::string &uri)
{
    exitcode_t result = EXITCODE_CONTINUE;
    auto args = String::Split(uri, "/");
    if (args.size() > 0)
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

static exitcode_t HandleUriJoin(const std::vector<std::string> &args)
{
    std::string hostname;
    sint32 port;
    if (args.size() > 1 && TryParseHostnamePort(args[1], &hostname, &port, NETWORK_DEFAULT_PORT))
    {
        // Set the network start configuration
        gNetworkStart = NETWORK_MODE_CLIENT;
        String::Set(gNetworkStartHost, sizeof(gNetworkStartHost), hostname.c_str());
        gNetworkStartPort = port;
        return EXITCODE_CONTINUE;
    }
    else
    {
        Console::Error::WriteLine("Expected hostname:port after join");
        return EXITCODE_FAIL;
    }
}

static bool TryParseHostnamePort(const std::string &hostnamePort, std::string * outHostname, sint32 * outPort, sint32 defaultPort)
{
    try
    {
        // Argument is in hostname:port format, so we need to split
        std::string hostname = hostnamePort;
        sint32 port = defaultPort;
        size_t colonIndex = hostnamePort.find_first_of(':');
        if (colonIndex != std::string::npos)
        {
            hostname = hostnamePort.substr(0, colonIndex);
            port = std::stoi(hostnamePort.substr(colonIndex + 1));
        }
        *outPort = port;
        *outHostname = hostname;
        return true;
    }
    catch (const std::exception &)
    {
        return false;
    }
}

#endif // DISABLE_NETWORK
