/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Cheats.h"
#include "../core/MemoryStream.h"
#include "../localisation/Localisation.h"
#include "../network/NetworkTypes.h"
#include "../network/network.h"
#include "../ride/Ride.h"
#include "../ride/TrackDesign.h"
#include "../world/Location.hpp"
#include "../world/TileElement.h"
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
        uint32_t temp = static_cast<uint32_t>(val.id);
        temp = ByteSwapBE(temp);
        stream->Write(&temp);
    }
    static void decode(IStream* stream, NetworkPlayerId_t& val)
    {
        uint32_t temp;
        stream->Read(&temp);
        val.id = static_cast<decltype(val.id)>(ByteSwapBE(temp));
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
        uint32_t temp = static_cast<uint32_t>(val.id);
        temp = ByteSwapBE(temp);
        stream->Write(&temp);
    }
    static void decode(IStream* stream, NetworkRideId_t& val)
    {
        uint32_t temp;
        stream->Read(&temp);
        val.id = static_cast<decltype(val.id)>(ByteSwapBE(temp));
    }
    static void log(IStream* stream, const NetworkRideId_t& val)
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

template<typename _Ty, size_t _Size> struct DataSerializerTraitsPODArray
{
    static void encode(IStream* stream, const _Ty (&val)[_Size])
    {
        uint16_t len = (uint16_t)_Size;
        uint16_t swapped = ByteSwapBE(len);
        stream->Write(&swapped);

        DataSerializerTraits<uint8_t> s;
        for (auto&& sub : val)
        {
            s.encode(stream, sub);
        }
    }
    static void decode(IStream* stream, _Ty (&val)[_Size])
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
    static void log(IStream* stream, const _Ty (&val)[_Size])
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

template<size_t _Size> struct DataSerializerTraits<uint8_t[_Size]> : public DataSerializerTraitsPODArray<uint8_t, _Size>
{
};

template<size_t _Size> struct DataSerializerTraits<uint16_t[_Size]> : public DataSerializerTraitsPODArray<uint16_t, _Size>
{
};

template<size_t _Size> struct DataSerializerTraits<uint32_t[_Size]> : public DataSerializerTraitsPODArray<uint32_t, _Size>
{
};

template<size_t _Size> struct DataSerializerTraits<uint64_t[_Size]> : public DataSerializerTraitsPODArray<uint64_t, _Size>
{
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

template<typename _Ty> struct DataSerializerTraits<std::vector<_Ty>>
{
    static void encode(IStream* stream, const std::vector<_Ty>& val)
    {
        uint16_t len = (uint16_t)val.size();
        uint16_t swapped = ByteSwapBE(len);
        stream->Write(&swapped);

        DataSerializerTraits<_Ty> s;
        for (auto&& sub : val)
        {
            s.encode(stream, sub);
        }
    }
    static void decode(IStream* stream, std::vector<_Ty>& val)
    {
        uint16_t len;
        stream->Read(&len);
        len = ByteSwapBE(len);

        DataSerializerTraits<_Ty> s;
        for (auto i = 0; i < len; ++i)
        {
            _Ty sub;
            s.decode(stream, sub);
            val.push_back(sub);
        }
    }
    static void log(IStream* stream, const std::vector<_Ty>& val)
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

template<> struct DataSerializerTraits<TileElement>
{
    static void encode(IStream* stream, const TileElement& tileElement)
    {
        stream->WriteValue(tileElement.type);
        stream->WriteValue(tileElement.flags);
        stream->WriteValue(tileElement.base_height);
        stream->WriteValue(tileElement.clearance_height);
        for (int i = 0; i < 4; ++i)
        {
            stream->WriteValue(tileElement.pad_04[i]);
        }
        for (int i = 0; i < 8; ++i)
        {
            stream->WriteValue(tileElement.pad_08[i]);
        }
    }
    static void decode(IStream* stream, TileElement& tileElement)
    {
        tileElement.type = stream->ReadValue<uint8_t>();
        tileElement.flags = stream->ReadValue<uint8_t>();
        tileElement.base_height = stream->ReadValue<uint8_t>();
        tileElement.clearance_height = stream->ReadValue<uint8_t>();
        for (int i = 0; i < 4; ++i)
        {
            tileElement.pad_04[i] = stream->ReadValue<uint8_t>();
        }
        for (int i = 0; i < 8; ++i)
        {
            tileElement.pad_08[i] = stream->ReadValue<uint8_t>();
        }
    }
    static void log(IStream* stream, const TileElement& tileElement)
    {
        char msg[128] = {};
        snprintf(
            msg, sizeof(msg), "TileElement(type = %u, flags = %u, base_height = %u)", tileElement.type, tileElement.flags,
            tileElement.base_height);
        stream->Write(msg, strlen(msg));
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

template<> struct DataSerializerTraits<CoordsXYZ>
{
    static void encode(IStream* stream, const CoordsXYZ& coord)
    {
        stream->WriteValue(ByteSwapBE(coord.x));
        stream->WriteValue(ByteSwapBE(coord.y));
        stream->WriteValue(ByteSwapBE(coord.z));
    }

    static void decode(IStream* stream, CoordsXYZ& coord)
    {
        auto x = ByteSwapBE(stream->ReadValue<int32_t>());
        auto y = ByteSwapBE(stream->ReadValue<int32_t>());
        auto z = ByteSwapBE(stream->ReadValue<int32_t>());
        coord = CoordsXYZ{ x, y, z };
    }

    static void log(IStream* stream, const CoordsXYZ& coord)
    {
        char msg[128] = {};
        snprintf(msg, sizeof(msg), "CoordsXYZ(x = %d, y = %d, z = %d)", coord.x, coord.y, coord.z);
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

template<> struct DataSerializerTraits<NetworkCheatType_t>
{
    static void encode(IStream* stream, const NetworkCheatType_t& val)
    {
        uint32_t temp = ByteSwapBE(val.id);
        stream->Write(&temp);
    }
    static void decode(IStream* stream, NetworkCheatType_t& val)
    {
        uint32_t temp;
        stream->Read(&temp);
        val.id = ByteSwapBE(temp);
    }
    static void log(IStream* stream, const NetworkCheatType_t& val)
    {
        const char* cheatName = CheatsGetName(static_cast<CheatType>(val.id));
        stream->Write(cheatName, strlen(cheatName));
    }
};

template<> struct DataSerializerTraits<rct_object_entry>
{
    static void encode(IStream* stream, const rct_object_entry& val)
    {
        uint32_t temp = ByteSwapBE(val.flags);
        stream->Write(&temp);
        stream->WriteArray(val.nameWOC, 12);
    }
    static void decode(IStream* stream, rct_object_entry& val)
    {
        uint32_t temp;
        stream->Read(&temp);
        val.flags = ByteSwapBE(temp);
        const char* str = stream->ReadArray<char>(12);
        memcpy(val.nameWOC, str, 12);
    }
    static void log(IStream* stream, const rct_object_entry& val)
    {
        stream->WriteArray(val.name, 8);
    }
};

template<> struct DataSerializerTraits<TrackDesignTrackElement>
{
    static void encode(IStream* stream, const TrackDesignTrackElement& val)
    {
        stream->Write(&val.flags);
        stream->Write(&val.type);
    }
    static void decode(IStream* stream, TrackDesignTrackElement& val)
    {
        stream->Read(&val.flags);
        stream->Read(&val.type);
    }
    static void log(IStream* stream, const TrackDesignTrackElement& val)
    {
        char msg[128] = {};
        snprintf(msg, sizeof(msg), "TrackDesignTrackElement(type = %d, flags = %d)", val.type, val.flags);
        stream->Write(msg, strlen(msg));
    }
};

template<> struct DataSerializerTraits<TrackDesignMazeElement>
{
    static void encode(IStream* stream, const TrackDesignMazeElement& val)
    {
        uint32_t temp = ByteSwapBE(val.all);
        stream->Write(&temp);
    }
    static void decode(IStream* stream, TrackDesignMazeElement& val)
    {
        uint32_t temp;
        stream->Read(&temp);
        val.all = ByteSwapBE(temp);
    }
    static void log(IStream* stream, const TrackDesignMazeElement& val)
    {
        char msg[128] = {};
        snprintf(msg, sizeof(msg), "TrackDesignMazeElement(all = %d)", val.all);
        stream->Write(msg, strlen(msg));
    }
};

template<> struct DataSerializerTraits<TrackDesignEntranceElement>
{
    static void encode(IStream* stream, const TrackDesignEntranceElement& val)
    {
        stream->Write(&val.x);
        stream->Write(&val.y);
        stream->Write(&val.z);
        stream->Write(&val.direction);
        stream->Write(&val.isExit);
    }
    static void decode(IStream* stream, TrackDesignEntranceElement& val)
    {
        stream->Read(&val.x);
        stream->Read(&val.y);
        stream->Read(&val.z);
        stream->Read(&val.direction);
        stream->Read(&val.isExit);
    }
    static void log(IStream* stream, const TrackDesignEntranceElement& val)
    {
        char msg[128] = {};
        snprintf(
            msg, sizeof(msg), "TrackDesignEntranceElement(x = %d, y = %d, z = %d, dir = %d, isExit = %d)", val.x, val.y, val.z,
            val.direction, val.isExit);
        stream->Write(msg, strlen(msg));
    }
};

template<> struct DataSerializerTraits<TrackDesignSceneryElement>
{
    static void encode(IStream* stream, const TrackDesignSceneryElement& val)
    {
        stream->Write(&val.x);
        stream->Write(&val.y);
        stream->Write(&val.z);
        stream->Write(&val.flags);
        stream->Write(&val.primary_colour);
        stream->Write(&val.secondary_colour);
        DataSerializerTraits<rct_object_entry> s;
        s.encode(stream, val.scenery_object);
    }
    static void decode(IStream* stream, TrackDesignSceneryElement& val)
    {
        stream->Read(&val.x);
        stream->Read(&val.y);
        stream->Read(&val.z);
        stream->Read(&val.flags);
        stream->Read(&val.primary_colour);
        stream->Read(&val.secondary_colour);
        DataSerializerTraits<rct_object_entry> s;
        s.decode(stream, val.scenery_object);
    }
    static void log(IStream* stream, const TrackDesignSceneryElement& val)
    {
        char msg[128] = {};
        snprintf(
            msg, sizeof(msg), "TrackDesignSceneryElement(x = %d, y = %d, z = %d, flags = %d, colour1 = %d, colour2 = %d)",
            val.x, val.y, val.z, val.flags, val.primary_colour, val.secondary_colour);
        stream->Write(msg, strlen(msg));
        stream->WriteArray(val.scenery_object.name, 8);
    }
};

template<> struct DataSerializerTraits<rct_vehicle_colour>
{
    static void encode(IStream* stream, const rct_vehicle_colour& val)
    {
        stream->Write(&val.body_colour);
        stream->Write(&val.trim_colour);
    }
    static void decode(IStream* stream, rct_vehicle_colour& val)
    {
        stream->Read(&val.body_colour);
        stream->Read(&val.trim_colour);
    }
    static void log(IStream* stream, const rct_vehicle_colour& val)
    {
        char msg[128] = {};
        snprintf(msg, sizeof(msg), "rct_vehicle_colour(body_colour = %d, trim_colour = %d)", val.body_colour, val.trim_colour);
        stream->Write(msg, strlen(msg));
    }
};
