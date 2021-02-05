/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_NETWORK

#    include "NetworkPacket.h"

#    include "NetworkTypes.h"

#    include <memory>

NetworkPacket::NetworkPacket(NetworkCommand id)
    : Header{ 0, id }
{
}

uint8_t* NetworkPacket::GetData()
{
    return Data.data();
}

const uint8_t* NetworkPacket::GetData() const
{
    return Data.data();
}

NetworkCommand NetworkPacket::GetCommand() const
{
    return Header.Id;
}

void NetworkPacket::Clear()
{
    BytesTransferred = 0;
    BytesRead = 0;
    Data.clear();
}

bool NetworkPacket::CommandRequiresAuth()
{
    switch (GetCommand())
    {
        case NetworkCommand::Ping:
        case NetworkCommand::Auth:
        case NetworkCommand::Token:
        case NetworkCommand::GameInfo:
        case NetworkCommand::ObjectsList:
        case NetworkCommand::Scripts:
        case NetworkCommand::MapRequest:
        case NetworkCommand::Heartbeat:
            return false;
        default:
            return true;
    }
}

void NetworkPacket::Write(const void* bytes, size_t size)
{
    const uint8_t* src = reinterpret_cast<const uint8_t*>(bytes);
    Data.insert(Data.end(), src, src + size);
}

void NetworkPacket::WriteString(const utf8* string)
{
    Write(reinterpret_cast<const uint8_t*>(string), strlen(string) + 1);
}

const uint8_t* NetworkPacket::Read(size_t size)
{
    if (BytesRead + size > Header.Size)
    {
        return nullptr;
    }
    else
    {
        uint8_t* data = &GetData()[BytesRead];
        BytesRead += size;
        return data;
    }
}

const utf8* NetworkPacket::ReadString()
{
    char* str = reinterpret_cast<char*>(&GetData()[BytesRead]);
    char* strend = str;
    while (BytesRead < Header.Size && *strend != 0)
    {
        BytesRead++;
        strend++;
    }
    if (*strend != 0)
    {
        return nullptr;
    }
    BytesRead++;
    return str;
}

#endif
