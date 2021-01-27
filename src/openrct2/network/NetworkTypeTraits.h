#pragma once

#include "../core/DataSerialiserTraits.h"
#include "NetworkTypes.h"
#include "network.h"

template<> struct DataSerializerTraits_t<NetworkPlayerId_t>
{
    static void encode(OpenRCT2::IStream* stream, const NetworkPlayerId_t& val)
    {
        uint32_t temp = static_cast<uint32_t>(val.id);
        temp = ByteSwapBE(temp);
        stream->Write(&temp);
    }
    static void decode(OpenRCT2::IStream* stream, NetworkPlayerId_t& val)
    {
        uint32_t temp;
        stream->Read(&temp);
        val.id = static_cast<decltype(val.id)>(ByteSwapBE(temp));
    }
    static void log(OpenRCT2::IStream* stream, const NetworkPlayerId_t& val)
    {
        char playerId[28] = {};
        snprintf(playerId, sizeof(playerId), "%u", val.id);

        stream->Write(playerId, strlen(playerId));

        int32_t playerIndex = OpenRCT2::GetContext()->GetNetwork()->GetPlayerIndex(val.id);
        if (playerIndex != -1)
        {
            const char* playerName = OpenRCT2::GetContext()->GetNetwork()->GetPlayerName(playerIndex);
            if (playerName != nullptr)
            {
                stream->Write(" \"", 2);
                stream->Write(playerName, strlen(playerName));
                stream->Write("\"", 1);
            }
        }
    }
};

template<> struct DataSerializerTraits_t<NetworkRideId_t>
{
    static void encode(OpenRCT2::IStream* stream, const NetworkRideId_t& val)
    {
        uint32_t temp = static_cast<uint32_t>(val.id);
        temp = ByteSwapBE(temp);
        stream->Write(&temp);
    }
    static void decode(OpenRCT2::IStream* stream, NetworkRideId_t& val)
    {
        uint32_t temp;
        stream->Read(&temp);
        val.id = static_cast<decltype(val.id)>(ByteSwapBE(temp));
    }
    static void log(OpenRCT2::IStream* stream, const NetworkRideId_t& val)
    {
        char rideId[28] = {};
        snprintf(rideId, sizeof(rideId), "%u", val.id);

        stream->Write(rideId, strlen(rideId));

        auto ride = get_ride(val.id);
        if (ride != nullptr)
        {
            auto rideName = ride->GetName();

            stream->Write(" \"", 2);
            stream->Write(rideName.c_str(), rideName.size());
            stream->Write("\"", 1);
        }
    }
};

template<> struct DataSerializerTraits_t<NetworkCheatType_t>
{
    static void encode(OpenRCT2::IStream* stream, const NetworkCheatType_t& val)
    {
        uint32_t temp = ByteSwapBE(val.id);
        stream->Write(&temp);
    }
    static void decode(OpenRCT2::IStream* stream, NetworkCheatType_t& val)
    {
        uint32_t temp;
        stream->Read(&temp);
        val.id = ByteSwapBE(temp);
    }
    static void log(OpenRCT2::IStream* stream, const NetworkCheatType_t& val)
    {
        const char* cheatName = CheatsGetName(static_cast<CheatType>(val.id));
        stream->Write(cheatName, strlen(cheatName));
    }
};
