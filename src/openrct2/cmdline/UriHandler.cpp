#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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
#include "CommandLine.hpp"

extern "C"
{
    #include "../openrct2.h"
}

static exitcode_t HandleUri(const utf8 * uri);

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

static exitcode_t HandleUri(const utf8 * uri)
{
    utf8 * * args;
    size_t numArgs = String::Split(&args, uri, '/');
    if (numArgs > 0)
    {
        utf8 * arg = args[0];
        if (String::Equals(arg, "join"))
        {
            if (numArgs > 1)
            {
                utf8 * hostnamePort = args[1];

                // Argument is in hostname:port format, so we need to split
                utf8 * hostname = String::Duplicate(hostnamePort);
                sint32 port = NETWORK_DEFAULT_PORT;
                size_t colonIndex = String::IndexOf(hostnamePort, ':');
                if (colonIndex != SIZE_MAX)
                {
                    Memory::Free(hostname);
                    hostname = String::Substring(hostnamePort, 0, colonIndex);
                    port = atoi(hostnamePort + colonIndex + 1);
                }

                // Set the network start configuration
                gNetworkStart = NETWORK_MODE_CLIENT;
                String::Set(gNetworkStartHost, sizeof(gNetworkStartHost), hostname);
                gNetworkStartPort = port;

                Memory::Free(hostname);
            }
            else
            {
                Console::Error::WriteLine("Expected hostname:port after join");
                return EXITCODE_FAIL;
            }
        }
    }

    // Clean up
    for (size_t i = 0; i < numArgs; i++)
    {
        Memory::Free(args[i]);
    }
    Memory::FreeArray(args, numArgs);

    return EXITCODE_CONTINUE;
}
