/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
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

std::unique_ptr<NetworkPacket> NetworkPacket::Allocate()
{
    return std::make_unique<NetworkPacket>();
}

std::unique_ptr<NetworkPacket> NetworkPacket::Duplicate(NetworkPacket& packet)
{
    return std::make_unique<NetworkPacket>(packet);
}

uint8_t* NetworkPacket::GetData()
{
    return &(*Data)[0];
}

int32_t NetworkPacket::GetCommand() const
{
    if (Data->size() >= sizeof(uint32_t))
    {
        return ByteSwapBE(*(uint32_t*)(&(*Data)[0]));
    }
    else
    {
        return NETWORK_COMMAND_INVALID;
    }
}

void NetworkPacket::Clear()
{
    BytesTransferred = 0;
    BytesRead = 0;
    Data->clear();
}

bool NetworkPacket::CommandRequiresAuth()
{
    switch (GetCommand())
    {
        case NETWORK_COMMAND_PING:
        case NETWORK_COMMAND_AUTH:
        case NETWORK_COMMAND_TOKEN:
        case NETWORK_COMMAND_GAMEINFO:
        case NETWORK_COMMAND_OBJECTS:
            return false;
        default:
            return true;
    }
}

void NetworkPacket::Write(const uint8_t* bytes, size_t size)
{
    Data->insert(Data->end(), bytes, bytes + size);
}

void NetworkPacket::WriteString(const utf8* string)
{
    Write((uint8_t*)string, strlen(string) + 1);
}

const uint8_t* NetworkPacket::Read(size_t size)
{
    if (BytesRead + size > NetworkPacket::Size)
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
    char* str = (char*)&GetData()[BytesRead];
    char* strend = str;
    while (BytesRead < Size && *strend != 0)
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
