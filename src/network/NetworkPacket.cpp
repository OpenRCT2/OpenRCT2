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

#ifndef DISABLE_NETWORK

#include "NetworkTypes.h"
#include "NetworkPacket.h"

NetworkPacket::NetworkPacket()
{
    transferred = 0;
    read = 0;
    size = 0;
    data = std::make_shared<std::vector<uint8>>();
}

std::unique_ptr<NetworkPacket> NetworkPacket::Allocate()
{
    return std::unique_ptr<NetworkPacket>(new NetworkPacket); // change to make_unique in c++14
}

std::unique_ptr<NetworkPacket> NetworkPacket::Duplicate(NetworkPacket &packet)
{
    return std::unique_ptr<NetworkPacket>(new NetworkPacket(packet)); // change to make_unique in c++14
}

uint8 * NetworkPacket::GetData()
{
    return &(*data)[0];
}

uint32 NetworkPacket::GetCommand()
{
    if (data->size() >= sizeof(uint32))
    {
        return ByteSwapBE(*(uint32 *)(&(*data)[0]));
    }
    else
    {
        return NETWORK_COMMAND_INVALID;
    }
}

void NetworkPacket::Clear()
{
    transferred = 0;
    read = 0;
    data->clear();
}

bool NetworkPacket::CommandRequiresAuth()
{
    switch (GetCommand()) {
    case NETWORK_COMMAND_PING:
    case NETWORK_COMMAND_AUTH:
    case NETWORK_COMMAND_TOKEN:
    case NETWORK_COMMAND_GAMEINFO:
        return false;
    default:
        return true;
    }
}

void NetworkPacket::Write(const uint8 * bytes, size_t size)
{
    data->insert(data->end(), bytes, bytes + size);
}

void NetworkPacket::WriteString(const utf8 * string)
{
    Write((uint8 *)string, strlen(string) + 1);
}

const uint8 * NetworkPacket::Read(size_t size)
{
    if (read + size > NetworkPacket::size)
    {
        return nullptr;
    }
    else
    {
        uint8 * data = &GetData()[read];
        read += size;
        return data;
    }
}

const utf8 * NetworkPacket::ReadString()
{
    char * str = (char *)&GetData()[read];
    char * strend = str;
    while (read < size && *strend != 0)
    {
        read++;
        strend++;
    }
    if (*strend != 0)
    {
        return nullptr;
    }
    read++;
    return str;
}

#endif
