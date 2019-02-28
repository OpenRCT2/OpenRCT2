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
#include "../world/Location.hpp"
#include "DataSerialiserTag.h"
#include "Endianness.h"
#include "MemoryStream.h"

#include <cstdio>
#include <iomanip>
#include <iostream>
#include <sstream>
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
        std::stringstream ss;
        ss << std::hex << std::setw(sizeof(T) * 2) << std::setfill('0') << +val;

        std::string str = ss.str();
        stream->Write(str.c_str(), str.size());
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
        stream->Write("\"", 1);
        stream->Write(str.data(), str.size());
        stream->Write("\"", 1);
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
        stream->Write("{", 1);
        DataSerializerTraits<_Ty> s;
        for (auto&& sub : val)
        {
            s.log(stream, sub);
            stream->Write("; ", 2);
        }
        stream->Write("}", 1);
    }
};

template<> struct DataSerializerTraits<MapRange>
{
    static void encode(IStream* stream, const MapRange& v)
    {
        stream->WriteValue(ByteSwapBE(v.LeftTop.x));
        stream->WriteValue(ByteSwapBE(v.LeftTop.y));
        stream->WriteValue(ByteSwapBE(v.RightBottom.x));
        stream->WriteValue(ByteSwapBE(v.RightBottom.y));
    }
    static void decode(IStream* stream, MapRange& v)
    {
        auto l = ByteSwapBE(stream->ReadValue<int32_t>());
        auto t = ByteSwapBE(stream->ReadValue<int32_t>());
        auto r = ByteSwapBE(stream->ReadValue<int32_t>());
        auto b = ByteSwapBE(stream->ReadValue<int32_t>());
        v = MapRange(l, t, r, b);
    }
    static void log(IStream* stream, const MapRange& v)
    {
        char coords[128] = {};
        snprintf(
            coords, sizeof(coords), "MapRange(l = %d, t = %d, r = %d, b = %d)", v.LeftTop.x, v.LeftTop.y, v.RightBottom.x,
            v.RightBottom.y);

        stream->Write(coords, strlen(coords));
    }
};

template<> struct DataSerializerTraits<CoordsXY>
{
    static void encode(IStream* stream, const CoordsXY& coords)
    {
        stream->WriteValue(ByteSwapBE(coords.x));
        stream->WriteValue(ByteSwapBE(coords.y));
    }
    static void decode(IStream* stream, CoordsXY& coords)
    {
        auto x = ByteSwapBE(stream->ReadValue<int32_t>());
        auto y = ByteSwapBE(stream->ReadValue<int32_t>());
        coords = CoordsXY{ x, y };
    }
    static void log(IStream* stream, const CoordsXY& coords)
    {
        char msg[128] = {};
        snprintf(msg, sizeof(msg), "CoordsXY(x = %d, y = %d)", coords.x, coords.y);
        stream->Write(msg, strlen(msg));
    }
};

template<> struct DataSerializerTraits<CoordsXYZD>
{
    static void encode(IStream* stream, const CoordsXYZD& coord)
    {
        stream->WriteValue(ByteSwapBE(coord.x));
        stream->WriteValue(ByteSwapBE(coord.y));
        stream->WriteValue(ByteSwapBE(coord.z));
        stream->WriteValue(ByteSwapBE(coord.direction));
    }

    static void decode(IStream* stream, CoordsXYZD& coord)
    {
        auto x = ByteSwapBE(stream->ReadValue<int32_t>());
        auto y = ByteSwapBE(stream->ReadValue<int32_t>());
        auto z = ByteSwapBE(stream->ReadValue<int32_t>());
        auto d = ByteSwapBE(stream->ReadValue<uint8_t>());
        coord = CoordsXYZD{ x, y, z, d };
    }

    static void log(IStream* stream, const CoordsXYZD& coord)
    {
        char msg[128] = {};
        snprintf(
            msg, sizeof(msg), "CoordsXYZD(x = %d, y = %d, z = %d, direction = %d)", coord.x, coord.y, coord.z, coord.direction);
        stream->Write(msg, strlen(msg));
    }
};
