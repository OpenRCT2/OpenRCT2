/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/MemoryStream.h"
#include "../localisation/Localisation.h"
#include "../network/NetworkTypes.h"
#include "../network/network.h"
#include "../ride/Ride.h"
#include "DataSerialiserTag.h"
#include "Endianness.h"
#include "MemoryStream.h"

#include <cstdio>
#include <stdexcept>

template<typename T> struct DataSerializerTraits
{
    static void encode(IStream* stream, const T& v) = delete;
    static void decode(IStream* stream, T& val) = delete;
    static void log(IStream* stream, const T& val) = delete;
};

template<typename T> struct DataSerializerTraitsIntegral
{
    static void encode(IStream* stream, const T& val)
    {
        T temp = ByteSwapBE(val);
        stream->Write(&temp);
    }
    static void decode(IStream* stream, T& val)
    {
        T temp;
        stream->Read(&temp);
        val = ByteSwapBE(temp);
    }
    static void log(IStream* stream, const T& val)
    {
        char temp[32] = {};
        if constexpr (sizeof(T) == 1)
            snprintf(temp, sizeof(temp), "%02X", val);
        else if constexpr (sizeof(T) == 2)
            snprintf(temp, sizeof(temp), "%04X", val);
        else if constexpr (sizeof(T) == 4)
            snprintf(temp, sizeof(temp), "%08X", val);
        else if constexpr (sizeof(T) == 8)
            snprintf(temp, sizeof(temp), "%016llX", val);
        else
            static_assert("Invalid size");

        stream->Write(temp, strlen(temp));
    }
};

template<> struct DataSerializerTraits<bool>
{
    static void encode(IStream* stream, const bool& val)
    {
        stream->Write(&val);
    }
    static void decode(IStream* stream, bool& val)
    {
        stream->Read(&val);
    }
    static void log(IStream* stream, const bool& val)
    {
        if (val)
            stream->Write("true", 4);
        else
            stream->Write("false", 5);
    }
};

template<> struct DataSerializerTraits<uint8_t> : public DataSerializerTraitsIntegral<uint8_t>
{
};

template<> struct DataSerializerTraits<int8_t> : public DataSerializerTraitsIntegral<int8_t>
{
};

template<> struct DataSerializerTraits<uint16_t> : public DataSerializerTraitsIntegral<uint16_t>
{
};

template<> struct DataSerializerTraits<int16_t> : public DataSerializerTraitsIntegral<int16_t>
{
};

template<> struct DataSerializerTraits<uint32_t> : public DataSerializerTraitsIntegral<uint32_t>
{
};

template<> struct DataSerializerTraits<int32_t> : public DataSerializerTraitsIntegral<int32_t>
{
};

template<> struct DataSerializerTraits<uint64_t> : public DataSerializerTraitsIntegral<uint64_t>
{
};

template<> struct DataSerializerTraits<int64_t> : public DataSerializerTraitsIntegral<int64_t>
{
};

template<> struct DataSerializerTraits<std::string>
{
    static void encode(IStream* stream, const std::string& str)
    {
        uint16_t len = (uint16_t)str.size();
        uint16_t swapped = ByteSwapBE(len);
        stream->Write(&swapped);
        stream->WriteArray(str.c_str(), len);
    }
    static void decode(IStream* stream, std::string& res)
    {
        uint16_t len;
        stream->Read(&len);
        len = ByteSwapBE(len);

        const char* str = stream->ReadArray<char>(len);
        res.assign(str, len);

        Memory::FreeArray(str, len);
    }
    static void log(IStream* stream, const std::string& str)
    {
        stream->Write("\"");
        stream->Write(str.data(), str.size());
        stream->Write("\"");
    }
};

template<> struct DataSerializerTraits<NetworkPlayerId_t>
{
    static void encode(IStream* stream, const NetworkPlayerId_t& val)
    {
        uint32_t temp = ByteSwapBE(val.id);
        stream->Write(&temp);
    }
    static void decode(IStream* stream, NetworkPlayerId_t& val)
    {
        uint32_t temp;
        stream->Read(&temp);
        val.id = ByteSwapBE(temp);
    }
    static void log(IStream* stream, const NetworkPlayerId_t& val)
    {
        char playerId[28] = {};
        snprintf(playerId, sizeof(playerId), "%u", val.id);

        stream->Write(playerId, strlen(playerId));

        int32_t playerIndex = network_get_player_index(val.id);
        if (playerIndex != -1)
        {
            const char* playerName = network_get_player_name(playerIndex);
            if (playerName != nullptr)
            {
                stream->Write(" \"", 2);
                stream->Write(playerName, strlen(playerName));
                stream->Write("\"", 1);
            }
        }
    }
};

template<> struct DataSerializerTraits<NetworkRideId_t>
{
    static void encode(IStream* stream, const NetworkRideId_t& val)
    {
        uint32_t temp = ByteSwapBE(val.id);
        stream->Write(&temp);
    }
    static void decode(IStream* stream, NetworkRideId_t& val)
    {
        uint32_t temp;
        stream->Read(&temp);
        val.id = ByteSwapBE(temp);
    }
    static void log(IStream* stream, const NetworkRideId_t& val)
    {
        char rideId[28] = {};
        snprintf(rideId, sizeof(rideId), "%u", val.id);

        stream->Write(rideId, strlen(rideId));

        Ride* ride = get_ride(val.id);
        if (ride)
        {
            char rideName[256] = {};
            format_string(rideName, 256, ride->name, &ride->name_arguments);

            stream->Write(" \"", 2);
            stream->Write(rideName, strlen(rideName));
            stream->Write("\"", 1);
        }
    }
};

template<typename T> struct DataSerializerTraits<DataSerialiserTag<T>>
{
    static void encode(IStream* stream, const DataSerialiserTag<T>& tag)
    {
        DataSerializerTraits<T> s;
        s.encode(stream, tag.Data());
    }
    static void decode(IStream* stream, DataSerialiserTag<T>& tag)
    {
        DataSerializerTraits<T> s;
        s.decode(stream, tag.Data());
    }
    static void log(IStream* stream, const DataSerialiserTag<T>& tag)
    {
        const char* name = tag.Name();
        stream->Write(name, strlen(name));
        stream->Write(" = ", 3);

        DataSerializerTraits<T> s;
        s.log(stream, tag.Data());

        stream->Write("; ", 2);
    }
};

template<> struct DataSerializerTraits<MemoryStream>
{
    static void encode(IStream* stream, const MemoryStream& val)
    {
        DataSerializerTraits<uint32_t> s;
        s.encode(stream, val.GetLength());

        stream->Write(val.GetData(), val.GetLength());
    }
    static void decode(IStream* stream, MemoryStream& val)
    {
        DataSerializerTraits<uint32_t> s;

        uint32_t length = 0;
        s.decode(stream, length);

        std::unique_ptr<uint8_t[]> buf(new uint8_t[length]);
        stream->Read(buf.get(), length);

        val.Write(buf.get(), length);
    }
    static void log(IStream* stream, const MemoryStream& tag)
    {
    }
};

template<typename _Ty, size_t _Size> struct DataSerializerTraits<std::array<_Ty, _Size>>
{
    static void encode(IStream* stream, const std::array<_Ty, _Size>& val)
    {
        uint16_t len = (uint16_t)_Size;
        uint16_t swapped = ByteSwapBE(len);
        stream->Write(&swapped);

        DataSerializerTraits<_Ty> s;
        for (auto&& sub : val)
        {
            s.encode(stream, sub);
        }
    }
    static void decode(IStream* stream, std::array<_Ty, _Size>& val)
    {
        uint16_t len;
        stream->Read(&len);
        len = ByteSwapBE(len);

        if (len != _Size)
            throw std::runtime_error("Invalid size, can't decode");

        DataSerializerTraits<_Ty> s;
        for (auto&& sub : val)
        {
            s.decode(stream, sub);
        }
    }
    static void log(IStream* stream, const std::array<_Ty, _Size>& val)
    {
        stream->Write("{");
        DataSerializerTraits<_Ty> s;
        for (auto&& sub : val)
        {
            s.log(stream, sub);
            stream->Write("; ", 2);
        }
        stream->Write("}");
    }
};
