/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/DataSerialiser.h"
#include "NetworkTypes.h"

enum NetworkPacketId : int32_t
{
    NETWORK_PACKET_REQUESTTOKEN,
    NETWORK_PACKET_REQUESTAUTH,
    NETWORK_PACKET_AUTHSTATUS,
    NETWORK_PACKET_GAMEACTION,
    NETWORK_PACKET_GAMECOMMAND,
    NETWORK_PACKET_LAST,
    NETWORK_PACKET_INVALID = -1,
};

struct NetworkPacketHead
{
    int32_t type;
    bool tickBound;

    NetworkPacketHead()
    {
    }
    NetworkPacketHead(int32_t a, bool b)
        : type(a)
        , tickBound(b)
    {
    }

    void SerialiseHead(DataSerialiser& stream)
    {
        stream << type << tickBound;
    }
};

// If TTickBound is true the packet will only be processed at a given tick.
// the network layer will handle this for us.
template<int32_t TType, bool TTickBound = false>
struct NetworkPacketBase : NetworkPacketHead
{
    NetworkPacketBase()
        : NetworkPacketHead(TType, TTickBound)
    {
    }

    static NetworkPacketId Type()
    {
        return static_cast<NetworkPacketId>(TType);
    }

    static bool TickBound()
    {
        return TTickBound;
    }
};

struct NetworkPacketRequestToken : NetworkPacketBase<NETWORK_PACKET_REQUESTTOKEN>
{
    std::string gameVersion;

    void Serialise(DataSerialiser& stream)
    {
        stream << gameVersion;
    }
};

struct NetworkPacketAuthRequest : NetworkPacketBase<NETWORK_PACKET_REQUESTAUTH>
{
    std::string gameVersion;
    std::string playerName;
    std::string password;
    std::string publicKey;
    std::string signature;

    void Serialise(DataSerialiser& stream)
    {
        stream << gameVersion << playerName << password << publicKey << signature;
    }
};

struct NetworkPacketAuthStatus : NetworkPacketBase<NETWORK_PACKET_AUTHSTATUS>
{
    uint32_t status;
    uint32_t playerId;

    void Serialise(DataSerialiser& stream)
    {
        stream << status << playerId;
    }
};

struct NetworkPacketGameCommand : NetworkPacketBase<NETWORK_PACKET_GAMECOMMAND, true>
{
    uint32_t args[7];
    uint8_t playerId;
    uint8_t callback;

    void Serialise(DataSerialiser& stream)
    {
        stream << args << playerId << callback;
    }
};