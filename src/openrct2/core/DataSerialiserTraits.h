/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Cheats.h"
#include "../entity/Guest.h"
#include "../network/Network.h"
#include "../network/NetworkTypes.h"
#include "../object/Object.h"
#include "../ride/RideColour.h"
#include "../ride/TrackDesign.h"
#include "../world/Banner.h"
#include "../world/Footpath.h"
#include "../world/Location.hpp"
#include "../world/tile_element/TileElement.h"
#include "DataSerialiserTag.h"
#include "Endianness.h"
#include "MemoryStream.h"
#include "StringTypes.h"

#include <cstdio>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

template<typename T>
struct DataSerializerTraitsT
{
    static void encode(OpenRCT2::IStream* stream, const T& v) = delete;
    static void decode(OpenRCT2::IStream* stream, T& val) = delete;
    static void log(OpenRCT2::IStream* stream, const T& val) = delete;
};

template<typename T>
struct DataSerializerTraitsEnum
{
    using TUnderlying = std::underlying_type_t<T>;

    static void encode(OpenRCT2::IStream* stream, const T& val)
    {
        stream->WriteValue(ByteSwapBE(static_cast<TUnderlying>(val)));
    }
    static void decode(OpenRCT2::IStream* stream, T& val)
    {
        val = static_cast<T>(ByteSwapBE(stream->ReadValue<TUnderlying>()));
    }
    static void log(OpenRCT2::IStream* stream, const T& val)
    {
        std::stringstream ss;
        ss << std::hex << std::setw(sizeof(TUnderlying) * 2) << std::setfill('0') << static_cast<TUnderlying>(val);

        std::string str = ss.str();
        stream->Write(str.c_str(), str.size());
    }
};

template<typename T>
using DataSerializerTraits = std::conditional_t<std::is_enum_v<T>, DataSerializerTraitsEnum<T>, DataSerializerTraitsT<T>>;

template<typename T>
struct DataSerializerTraitsIntegral
{
    static void encode(OpenRCT2::IStream* stream, const T& val)
    {
        stream->WriteValue(ByteSwapBE(val));
    }
    static void decode(OpenRCT2::IStream* stream, T& val)
    {
        val = ByteSwapBE(stream->ReadValue<T>());
    }
    static void log(OpenRCT2::IStream* stream, const T& val)
    {
        std::stringstream ss;
        ss << std::hex << std::setw(sizeof(T) * 2) << std::setfill('0') << +val;

        std::string str = ss.str();
        stream->Write(str.c_str(), str.size());
    }
};

template<>
struct DataSerializerTraitsT<bool>
{
    static void encode(OpenRCT2::IStream* stream, const bool& val)
    {
        stream->WriteValue(val);
    }
    static void decode(OpenRCT2::IStream* stream, bool& val)
    {
        stream->ReadValue(val);
    }
    static void log(OpenRCT2::IStream* stream, const bool& val)
    {
        if (val)
            stream->Write("true", 4);
        else
            stream->Write("false", 5);
    }
};

template<>
struct DataSerializerTraitsT<uint8_t> : public DataSerializerTraitsIntegral<uint8_t>
{
};

template<>
struct DataSerializerTraitsT<int8_t> : public DataSerializerTraitsIntegral<int8_t>
{
};

template<>
struct DataSerializerTraitsT<utf8> : public DataSerializerTraitsIntegral<utf8>
{
};

template<>
struct DataSerializerTraitsT<uint16_t> : public DataSerializerTraitsIntegral<uint16_t>
{
};

template<>
struct DataSerializerTraitsT<int16_t> : public DataSerializerTraitsIntegral<int16_t>
{
};

template<>
struct DataSerializerTraitsT<uint32_t> : public DataSerializerTraitsIntegral<uint32_t>
{
};

template<>
struct DataSerializerTraitsT<int32_t> : public DataSerializerTraitsIntegral<int32_t>
{
};

template<>
struct DataSerializerTraitsT<uint64_t> : public DataSerializerTraitsIntegral<uint64_t>
{
};

template<>
struct DataSerializerTraitsT<int64_t> : public DataSerializerTraitsIntegral<int64_t>
{
};

template<>
struct DataSerializerTraitsT<std::string>
{
    static void encode(OpenRCT2::IStream* stream, const std::string& str)
    {
        uint16_t len = static_cast<uint16_t>(str.size());
        stream->WriteValue(ByteSwapBE(len));
        if (len == 0)
        {
            return;
        }
        stream->Write(str.c_str(), len);
    }
    static void decode(OpenRCT2::IStream* stream, std::string& res)
    {
        uint16_t len = ByteSwapBE(stream->ReadValue<uint16_t>());
        if (len == 0)
        {
            res.clear();
            return;
        }
        auto str = stream->ReadArray<char>(len);
        res.assign(str.get(), len);
    }
    static void log(OpenRCT2::IStream* stream, const std::string& str)
    {
        stream->Write("\"", 1);
        if (str.size() != 0)
        {
            stream->Write(str.data(), str.size());
        }
        stream->Write("\"", 1);
    }
};

template<>
struct DataSerializerTraitsT<OpenRCT2::Network::PlayerId_t>
{
    static void encode(OpenRCT2::IStream* stream, const OpenRCT2::Network::PlayerId_t& val)
    {
        stream->WriteValue(ByteSwapBE(val.id));
    }
    static void decode(OpenRCT2::IStream* stream, OpenRCT2::Network::PlayerId_t& val)
    {
        val.id = ByteSwapBE(stream->ReadValue<int32_t>());
    }
    static void log(OpenRCT2::IStream* stream, const OpenRCT2::Network::PlayerId_t& val)
    {
        char playerId[28] = {};
        snprintf(playerId, sizeof(playerId), "%u", val.id);

        stream->Write(playerId, strlen(playerId));

        int32_t playerIndex = OpenRCT2::Network::GetPlayerIndex(val.id);
        if (playerIndex != -1)
        {
            const char* playerName = OpenRCT2::Network::GetPlayerName(playerIndex);
            if (playerName != nullptr)
            {
                stream->Write(" \"", 2);
                stream->Write(playerName, strlen(playerName));
                stream->Write("\"", 1);
            }
        }
    }
};

template<typename T>
struct DataSerializerTraitsT<DataSerialiserTag<T>>
{
    static void encode(OpenRCT2::IStream* stream, const DataSerialiserTag<T>& tag)
    {
        DataSerializerTraits<T> s;
        s.encode(stream, tag.Data());
    }
    static void decode(OpenRCT2::IStream* stream, DataSerialiserTag<T>& tag)
    {
        DataSerializerTraits<T> s;
        s.decode(stream, tag.Data());
    }
    static void log(OpenRCT2::IStream* stream, const DataSerialiserTag<T>& tag)
    {
        const char* name = tag.Name();
        stream->Write(name, strlen(name));
        stream->Write(" = ", 3);

        DataSerializerTraits<T> s;
        s.log(stream, tag.Data());

        stream->Write("; ", 2);
    }
};

template<>
struct DataSerializerTraitsT<OpenRCT2::MemoryStream>
{
    static void encode(OpenRCT2::IStream* stream, const OpenRCT2::MemoryStream& val)
    {
        DataSerializerTraits<uint32_t> s;
        s.encode(stream, val.GetLength());

        stream->Write(val.GetData(), val.GetLength());
    }
    static void decode(OpenRCT2::IStream* stream, OpenRCT2::MemoryStream& val)
    {
        DataSerializerTraits<uint32_t> s;

        uint32_t length = 0;
        s.decode(stream, length);

        auto buf = std::make_unique<uint8_t[]>(length);
        stream->Read(buf.get(), length);

        val.Write(buf.get(), length);
    }
    static void log(OpenRCT2::IStream* stream, const OpenRCT2::MemoryStream& tag)
    {
    }
};

template<typename _Ty, size_t _Size>
struct DataSerializerTraitsPODArray
{
    static void encode(OpenRCT2::IStream* stream, const _Ty (&val)[_Size])
    {
        stream->WriteValue(ByteSwapBE(static_cast<uint16_t>(_Size)));

        DataSerializerTraits<_Ty> s;
        for (auto&& sub : val)
        {
            s.encode(stream, sub);
        }
    }
    static void decode(OpenRCT2::IStream* stream, _Ty (&val)[_Size])
    {
        uint16_t len = ByteSwapBE(stream->ReadValue<uint16_t>());

        if (len != _Size)
            throw std::runtime_error("Invalid size, can't decode");

        DataSerializerTraits<_Ty> s;
        for (auto&& sub : val)
        {
            s.decode(stream, sub);
        }
    }
    static void log(OpenRCT2::IStream* stream, const _Ty (&val)[_Size])
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

template<size_t _Size>
struct DataSerializerTraitsT<uint8_t[_Size]> : public DataSerializerTraitsPODArray<uint8_t, _Size>
{
};

template<size_t _Size>
struct DataSerializerTraitsT<utf8[_Size]> : public DataSerializerTraitsPODArray<utf8, _Size>
{
};

template<size_t _Size>
struct DataSerializerTraitsT<uint16_t[_Size]> : public DataSerializerTraitsPODArray<uint16_t, _Size>
{
};

template<size_t _Size>
struct DataSerializerTraitsT<uint32_t[_Size]> : public DataSerializerTraitsPODArray<uint32_t, _Size>
{
};

template<size_t _Size>
struct DataSerializerTraitsT<uint64_t[_Size]> : public DataSerializerTraitsPODArray<uint64_t, _Size>
{
};

template<size_t _Size>
struct DataSerializerTraitsT<OpenRCT2::Drawing::Colour[_Size]>
    : public DataSerializerTraitsPODArray<OpenRCT2::Drawing::Colour, _Size>
{
};

template<typename T, T TNullValue, typename TTag, size_t _Size>
struct DataSerializerTraitsT<TIdentifier<T, TNullValue, TTag>[_Size]>
    : public DataSerializerTraitsPODArray<TIdentifier<T, TNullValue, TTag>, _Size>
{
};

template<typename _Ty, size_t _Size>
struct DataSerializerTraitsT<std::array<_Ty, _Size>>
{
    static void encode(OpenRCT2::IStream* stream, const std::array<_Ty, _Size>& val)
    {
        stream->WriteValue(ByteSwapBE(static_cast<uint16_t>(_Size)));

        DataSerializerTraits<_Ty> s;
        for (auto&& sub : val)
        {
            s.encode(stream, sub);
        }
    }
    static void decode(OpenRCT2::IStream* stream, std::array<_Ty, _Size>& val)
    {
        uint16_t len = ByteSwapBE(stream->ReadValue<uint16_t>());

        if (len != _Size)
            throw std::runtime_error("Invalid size, can't decode");

        DataSerializerTraits<_Ty> s;
        for (auto&& sub : val)
        {
            s.decode(stream, sub);
        }
    }
    static void log(OpenRCT2::IStream* stream, const std::array<_Ty, _Size>& val)
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

template<typename _Ty>
struct DataSerializerTraitsT<std::vector<_Ty>>
{
    static void encode(OpenRCT2::IStream* stream, const std::vector<_Ty>& val)
    {
        stream->WriteValue(ByteSwapBE(static_cast<uint16_t>(val.size())));

        DataSerializerTraits<_Ty> s;
        for (auto&& sub : val)
        {
            s.encode(stream, sub);
        }
    }
    static void decode(OpenRCT2::IStream* stream, std::vector<_Ty>& val)
    {
        uint16_t len = ByteSwapBE(stream->ReadValue<uint16_t>());

        DataSerializerTraits<_Ty> s;
        for (auto i = 0; i < len; ++i)
        {
            _Ty sub{};
            s.decode(stream, sub);
            val.push_back(std::move(sub));
        }
    }
    static void log(OpenRCT2::IStream* stream, const std::vector<_Ty>& val)
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

template<>
struct DataSerializerTraitsT<MapRange>
{
    static void encode(OpenRCT2::IStream* stream, const MapRange& v)
    {
        stream->WriteValue(ByteSwapBE(v.GetX1()));
        stream->WriteValue(ByteSwapBE(v.GetY1()));
        stream->WriteValue(ByteSwapBE(v.GetX2()));
        stream->WriteValue(ByteSwapBE(v.GetY2()));
    }
    static void decode(OpenRCT2::IStream* stream, MapRange& v)
    {
        auto l = ByteSwapBE(stream->ReadValue<int32_t>());
        auto t = ByteSwapBE(stream->ReadValue<int32_t>());
        auto r = ByteSwapBE(stream->ReadValue<int32_t>());
        auto b = ByteSwapBE(stream->ReadValue<int32_t>());
        v = MapRange(l, t, r, b);
    }
    static void log(OpenRCT2::IStream* stream, const MapRange& v)
    {
        char coords[128] = {};
        snprintf(
            coords, sizeof(coords), "MapRange(x1 = %d, y1 = %d, x2 = %d, y2 = %d)", v.GetX1(), v.GetY1(), v.GetX2(), v.GetY2());

        stream->Write(coords, strlen(coords));
    }
};

template<>
struct DataSerializerTraitsT<OpenRCT2::TileElement>
{
    static void encode(OpenRCT2::IStream* stream, const OpenRCT2::TileElement& tileElement)
    {
        stream->WriteValue(tileElement.Type);
        stream->WriteValue(tileElement.Flags);
        stream->WriteValue(tileElement.BaseHeight);
        stream->WriteValue(tileElement.ClearanceHeight);
        stream->WriteValue(tileElement.Owner);
        for (auto v : tileElement.Pad05)
        {
            stream->WriteValue(v);
        }
        for (auto v : tileElement.Pad08)
        {
            stream->WriteValue(v);
        }
    }
    static void decode(OpenRCT2::IStream* stream, OpenRCT2::TileElement& tileElement)
    {
        tileElement.Type = stream->ReadValue<uint8_t>();
        tileElement.Flags = stream->ReadValue<uint8_t>();
        tileElement.BaseHeight = stream->ReadValue<uint8_t>();
        tileElement.ClearanceHeight = stream->ReadValue<uint8_t>();
        tileElement.Owner = stream->ReadValue<uint8_t>();
        for (auto& v : tileElement.Pad05)
        {
            v = stream->ReadValue<uint8_t>();
        }
        for (auto& v : tileElement.Pad08)
        {
            v = stream->ReadValue<uint8_t>();
        }
    }
    static void log(OpenRCT2::IStream* stream, const OpenRCT2::TileElement& tileElement)
    {
        char msg[128] = {};
        snprintf(
            msg, sizeof(msg), "TileElement(type = %u, flags = %u, BaseHeight = %u)", tileElement.Type, tileElement.Flags,
            tileElement.BaseHeight);
        stream->Write(msg, strlen(msg));
    }
};

template<>
struct DataSerializerTraitsT<TileCoordsXY>
{
    static void encode(OpenRCT2::IStream* stream, const TileCoordsXY& coords)
    {
        stream->WriteValue(ByteSwapBE(coords.x));
        stream->WriteValue(ByteSwapBE(coords.y));
    }
    static void decode(OpenRCT2::IStream* stream, TileCoordsXY& coords)
    {
        auto x = ByteSwapBE(stream->ReadValue<int32_t>());
        auto y = ByteSwapBE(stream->ReadValue<int32_t>());
        coords = TileCoordsXY{ x, y };
    }
    static void log(OpenRCT2::IStream* stream, const TileCoordsXY& coords)
    {
        char msg[128] = {};
        snprintf(msg, sizeof(msg), "TileCoordsXY(x = %d, y = %d)", coords.x, coords.y);
        stream->Write(msg, strlen(msg));
    }
};

template<>
struct DataSerializerTraitsT<CoordsXY>
{
    static void encode(OpenRCT2::IStream* stream, const CoordsXY& coords)
    {
        stream->WriteValue(ByteSwapBE(coords.x));
        stream->WriteValue(ByteSwapBE(coords.y));
    }
    static void decode(OpenRCT2::IStream* stream, CoordsXY& coords)
    {
        auto x = ByteSwapBE(stream->ReadValue<int32_t>());
        auto y = ByteSwapBE(stream->ReadValue<int32_t>());
        coords = CoordsXY{ x, y };
    }
    static void log(OpenRCT2::IStream* stream, const CoordsXY& coords)
    {
        char msg[128] = {};
        snprintf(msg, sizeof(msg), "CoordsXY(x = %d, y = %d)", coords.x, coords.y);
        stream->Write(msg, strlen(msg));
    }
};

template<>
struct DataSerializerTraitsT<CoordsXYZ>
{
    static void encode(OpenRCT2::IStream* stream, const CoordsXYZ& coord)
    {
        stream->WriteValue(ByteSwapBE(coord.x));
        stream->WriteValue(ByteSwapBE(coord.y));
        stream->WriteValue(ByteSwapBE(coord.z));
    }

    static void decode(OpenRCT2::IStream* stream, CoordsXYZ& coord)
    {
        auto x = ByteSwapBE(stream->ReadValue<int32_t>());
        auto y = ByteSwapBE(stream->ReadValue<int32_t>());
        auto z = ByteSwapBE(stream->ReadValue<int32_t>());
        coord = CoordsXYZ{ x, y, z };
    }

    static void log(OpenRCT2::IStream* stream, const CoordsXYZ& coord)
    {
        char msg[128] = {};
        snprintf(msg, sizeof(msg), "CoordsXYZ(x = %d, y = %d, z = %d)", coord.x, coord.y, coord.z);
        stream->Write(msg, strlen(msg));
    }
};

template<>
struct DataSerializerTraitsT<CoordsXYZD>
{
    static void encode(OpenRCT2::IStream* stream, const CoordsXYZD& coord)
    {
        stream->WriteValue(ByteSwapBE(coord.x));
        stream->WriteValue(ByteSwapBE(coord.y));
        stream->WriteValue(ByteSwapBE(coord.z));
        stream->WriteValue(ByteSwapBE(coord.direction));
    }

    static void decode(OpenRCT2::IStream* stream, CoordsXYZD& coord)
    {
        auto x = ByteSwapBE(stream->ReadValue<int32_t>());
        auto y = ByteSwapBE(stream->ReadValue<int32_t>());
        auto z = ByteSwapBE(stream->ReadValue<int32_t>());
        auto d = ByteSwapBE(stream->ReadValue<uint8_t>());
        coord = CoordsXYZD{ x, y, z, d };
    }

    static void log(OpenRCT2::IStream* stream, const CoordsXYZD& coord)
    {
        char msg[128] = {};
        snprintf(
            msg, sizeof(msg), "CoordsXYZD(x = %d, y = %d, z = %d, direction = %d)", coord.x, coord.y, coord.z, coord.direction);
        stream->Write(msg, strlen(msg));
    }
};

template<>
struct DataSerializerTraitsT<OpenRCT2::Network::CheatType_t>
{
    static void encode(OpenRCT2::IStream* stream, const OpenRCT2::Network::CheatType_t& val)
    {
        stream->WriteValue(ByteSwapBE(val.id));
    }
    static void decode(OpenRCT2::IStream* stream, OpenRCT2::Network::CheatType_t& val)
    {
        val.id = ByteSwapBE(stream->ReadValue<int32_t>());
    }
    static void log(OpenRCT2::IStream* stream, const OpenRCT2::Network::CheatType_t& val)
    {
        const char* cheatName = CheatsGetName(static_cast<CheatType>(val.id));
        stream->Write(cheatName, strlen(cheatName));
    }
};

template<>
struct DataSerializerTraitsT<OpenRCT2::RCTObjectEntry>
{
    static void encode(OpenRCT2::IStream* stream, const OpenRCT2::RCTObjectEntry& val)
    {
        stream->WriteValue(ByteSwapBE(val.flags));
        stream->WriteArray(val.nameWOC, 12);
    }
    static void decode(OpenRCT2::IStream* stream, OpenRCT2::RCTObjectEntry& val)
    {
        val.flags = ByteSwapBE(stream->ReadValue<uint32_t>());
        auto str = stream->ReadArray<char>(12);
        memcpy(val.nameWOC, str.get(), 12);
    }
    static void log(OpenRCT2::IStream* stream, const OpenRCT2::RCTObjectEntry& val)
    {
        stream->WriteArray(val.name, 8);
    }
};

template<>
struct DataSerializerTraitsT<OpenRCT2::ObjectEntryDescriptor>
{
    static void encode(OpenRCT2::IStream* stream, const OpenRCT2::ObjectEntryDescriptor& val)
    {
        stream->WriteValue<uint8_t>(static_cast<uint8_t>(val.Generation));
        if (val.Generation == OpenRCT2::ObjectGeneration::DAT)
        {
            DataSerializerTraits<OpenRCT2::RCTObjectEntry> s;
            s.encode(stream, val.Entry);
        }
        else
        {
            stream->WriteValue<uint8_t>(static_cast<uint8_t>(val.GetType()));
            stream->WriteString(val.Identifier);
        }
    }

    static void decode(OpenRCT2::IStream* stream, OpenRCT2::ObjectEntryDescriptor& val)
    {
        auto generation = static_cast<OpenRCT2::ObjectGeneration>(stream->ReadValue<uint8_t>());
        if (generation == OpenRCT2::ObjectGeneration::DAT)
        {
            DataSerializerTraits<OpenRCT2::RCTObjectEntry> s;
            OpenRCT2::RCTObjectEntry entry;
            s.decode(stream, entry);
            val = OpenRCT2::ObjectEntryDescriptor(entry);
        }
        else
        {
            auto type = static_cast<OpenRCT2::ObjectType>(stream->ReadValue<uint8_t>());
            auto identifier = stream->ReadString();
            val = OpenRCT2::ObjectEntryDescriptor(type, identifier);
        }
    }

    static void log(OpenRCT2::IStream* stream, const OpenRCT2::ObjectEntryDescriptor& val)
    {
        auto identifier = std::string(val.GetName());
        char msg[128] = {};
        snprintf(msg, sizeof(msg), "ObjectEntryDescriptor[%s]", identifier.c_str());
        stream->Write(msg, strlen(msg));
    }
};

template<>
struct DataSerializerTraitsT<TrackDesignTrackElement>
{
    static void encode(OpenRCT2::IStream* stream, const TrackDesignTrackElement& val)
    {
        stream->WriteValue(val.type);
        stream->WriteValue(val.flags);
        stream->WriteValue(val.colourScheme);
        stream->WriteValue(val.stationIndex);
        stream->WriteValue(val.brakeBoosterSpeed);
        stream->WriteValue(val.seatRotation);
    }
    static void decode(OpenRCT2::IStream* stream, TrackDesignTrackElement& val)
    {
        stream->ReadValue(val.type);
        stream->ReadValue(val.flags);
        stream->ReadValue(val.colourScheme);
        stream->ReadValue(val.stationIndex);
        stream->ReadValue(val.brakeBoosterSpeed);
        stream->ReadValue(val.seatRotation);
    }
    static void log(OpenRCT2::IStream* stream, const TrackDesignTrackElement& val)
    {
        char msg[128] = {};
        snprintf(msg, sizeof(msg), "TrackDesignTrackElement(type = %d, flags = %d)", EnumValue(val.type), val.flags);
        stream->Write(msg, strlen(msg));
    }
};

template<>
struct DataSerializerTraitsT<TrackDesignMazeElement>
{
    static void encode(OpenRCT2::IStream* stream, const TrackDesignMazeElement& val)
    {
        stream->WriteValue(val.location);
        stream->WriteValue(val.mazeEntry);
    }
    static void decode(OpenRCT2::IStream* stream, TrackDesignMazeElement& val)
    {
        stream->ReadValue(val.location);
        stream->ReadValue(val.mazeEntry);
    }
    static void log(OpenRCT2::IStream* stream, const TrackDesignMazeElement& val)
    {
        char msg[128] = {};
        snprintf(
            msg, sizeof(msg), "TrackDesignMazeElement(x = %d, y = %d, entry = %d)", val.location.x, val.location.y,
            val.mazeEntry);
        stream->Write(msg, strlen(msg));
    }
};

template<>
struct DataSerializerTraitsT<TrackDesignEntranceElement>
{
    static void encode(OpenRCT2::IStream* stream, const TrackDesignEntranceElement& val)
    {
        stream->WriteValue(val.location);
        stream->WriteValue(val.isExit);
    }
    static void decode(OpenRCT2::IStream* stream, TrackDesignEntranceElement& val)
    {
        stream->ReadValue(val.location);
        stream->ReadValue(val.isExit);
    }
    static void log(OpenRCT2::IStream* stream, const TrackDesignEntranceElement& val)
    {
        char msg[128] = {};
        snprintf(
            msg, sizeof(msg), "TrackDesignEntranceElement(x = %d, y = %d, z = %d, dir = %d, isExit = %d)", val.location.x,
            val.location.y, val.location.z, val.location.direction, val.isExit);
        stream->Write(msg, strlen(msg));
    }
};

template<>
struct DataSerializerTraitsT<TrackDesignSceneryElement>
{
    static void encode(OpenRCT2::IStream* stream, const TrackDesignSceneryElement& val)
    {
        stream->WriteValue(val.loc);
        stream->WriteValue(val.flags);
        stream->WriteValue(val.primaryColour);
        stream->WriteValue(val.secondaryColour);
        stream->WriteValue(val.tertiaryColour);
        DataSerializerTraits<OpenRCT2::ObjectEntryDescriptor> s;
        s.encode(stream, val.sceneryObject);
    }
    static void decode(OpenRCT2::IStream* stream, TrackDesignSceneryElement& val)
    {
        stream->ReadValue(val.loc);
        stream->ReadValue(val.flags);
        stream->ReadValue(val.primaryColour);
        stream->ReadValue(val.secondaryColour);
        stream->ReadValue(val.tertiaryColour);
        DataSerializerTraits<OpenRCT2::ObjectEntryDescriptor> s;
        s.decode(stream, val.sceneryObject);
    }
    static void log(OpenRCT2::IStream* stream, const TrackDesignSceneryElement& val)
    {
        char msg[128] = {};
        snprintf(
            msg, sizeof(msg),
            "TrackDesignSceneryElement(x = %d, y = %d, z = %d, flags = %d, colour1 = %d, colour2 = %d, colour3 = %d)",
            val.loc.x, val.loc.y, val.loc.z, val.flags, EnumValue(val.primaryColour), EnumValue(val.secondaryColour),
            EnumValue(val.tertiaryColour));
        stream->Write(msg, strlen(msg));

        auto identifier = val.sceneryObject.GetName();
        stream->WriteArray(identifier.data(), identifier.size());
    }
};

template<>
struct DataSerializerTraitsT<TrackColour>
{
    static void encode(OpenRCT2::IStream* stream, const TrackColour& val)
    {
        stream->WriteValue(val.main);
        stream->WriteValue(val.additional);
        stream->WriteValue(val.supports);
    }
    static void decode(OpenRCT2::IStream* stream, TrackColour& val)
    {
        stream->ReadValue(val.main);
        stream->ReadValue(val.additional);
        stream->ReadValue(val.supports);
    }
    static void log(OpenRCT2::IStream* stream, const TrackColour& val)
    {
        char msg[128] = {};
        snprintf(
            msg, sizeof(msg), "TrackColour(main = %d, additional = %d, supports = %d)", EnumValue(val.main),
            EnumValue(val.additional), EnumValue(val.supports));
        stream->Write(msg, strlen(msg));
    }
};

template<>
struct DataSerializerTraitsT<VehicleColour>
{
    static void encode(OpenRCT2::IStream* stream, const VehicleColour& val)
    {
        stream->WriteValue(val.Body);
        stream->WriteValue(val.Trim);
        stream->WriteValue(val.Tertiary);
    }
    static void decode(OpenRCT2::IStream* stream, VehicleColour& val)
    {
        stream->ReadValue(val.Body);
        stream->ReadValue(val.Trim);
        stream->ReadValue(val.Tertiary);
    }
    static void log(OpenRCT2::IStream* stream, const VehicleColour& val)
    {
        char msg[128] = {};
        snprintf(
            msg, sizeof(msg), "VehicleColour(Body = %d, Trim = %d, Tertiary = %d)", EnumValue(val.Body), EnumValue(val.Trim),
            EnumValue(val.Tertiary));
        stream->Write(msg, strlen(msg));
    }
};

template<>
struct DataSerializerTraitsT<OpenRCT2::RideRating::Tuple>
{
    static void encode(OpenRCT2::IStream* stream, const OpenRCT2::RideRating::Tuple& val)
    {
        stream->WriteValue(val.excitement);
        stream->WriteValue(val.intensity);
        stream->WriteValue(val.nausea);
    }
    static void decode(OpenRCT2::IStream* stream, OpenRCT2::RideRating::Tuple& val)
    {
        stream->ReadValue(val.excitement);
        stream->ReadValue(val.intensity);
        stream->ReadValue(val.nausea);
    }
    static void log(OpenRCT2::IStream* stream, const OpenRCT2::RideRating::Tuple& val)
    {
        char msg[128] = {};
        snprintf(
            msg, sizeof(msg), "OpenRCT2::RideRating::Tuple(excitement = %d, intensity = %d, nausea = %d)", val.excitement,
            val.intensity, val.nausea);
        stream->Write(msg, strlen(msg));
    }
};

template<>
struct DataSerializerTraitsT<IntensityRange>
{
    static void encode(OpenRCT2::IStream* stream, const IntensityRange& val)
    {
        stream->WriteValue(static_cast<uint8_t>(val));
    }
    static void decode(OpenRCT2::IStream* stream, IntensityRange& val)
    {
        val = static_cast<IntensityRange>(stream->ReadValue<uint8_t>());
    }
    static void log(OpenRCT2::IStream* stream, const IntensityRange& val)
    {
        char msg[128] = {};
        snprintf(msg, sizeof(msg), "IntensityRange(min = %d, max = %d)", val.GetMinimum(), val.GetMaximum());
        stream->Write(msg, strlen(msg));
    }
};

template<>
struct DataSerializerTraitsT<PeepThought>
{
    static void encode(OpenRCT2::IStream* stream, const PeepThought& val)
    {
        stream->WriteValue(val.type);
        stream->WriteValue(val.item);
        stream->WriteValue(val.freshness);
        stream->WriteValue(val.fresh_timeout);
    }
    static void decode(OpenRCT2::IStream* stream, PeepThought& val)
    {
        stream->ReadValue(val.type);
        stream->ReadValue(val.item);
        stream->ReadValue(val.freshness);
        stream->ReadValue(val.fresh_timeout);
    }
    static void log(OpenRCT2::IStream* stream, const PeepThought& val)
    {
        char msg[128] = {};
        snprintf(
            msg, sizeof(msg), "PeepThought(type = %d, item = %u, freshness = %d, freshtimeout = %d)",
            static_cast<int32_t>(val.type), val.item, val.freshness, val.fresh_timeout);
        stream->Write(msg, strlen(msg));
    }
};

template<>
struct DataSerializerTraitsT<TileCoordsXYZD>
{
    static void encode(OpenRCT2::IStream* stream, const TileCoordsXYZD& coord)
    {
        stream->WriteValue(ByteSwapBE(coord.x));
        stream->WriteValue(ByteSwapBE(coord.y));
        stream->WriteValue(ByteSwapBE(coord.z));
        stream->WriteValue(ByteSwapBE(coord.direction));
    }

    static void decode(OpenRCT2::IStream* stream, TileCoordsXYZD& coord)
    {
        auto x = ByteSwapBE(stream->ReadValue<int32_t>());
        auto y = ByteSwapBE(stream->ReadValue<int32_t>());
        auto z = ByteSwapBE(stream->ReadValue<int32_t>());
        auto d = ByteSwapBE(stream->ReadValue<Direction>());
        coord = TileCoordsXYZD{ x, y, z, d };
    }

    static void log(OpenRCT2::IStream* stream, const TileCoordsXYZD& coord)
    {
        char msg[128] = {};
        snprintf(
            msg, sizeof(msg), "TileCoordsXYZD(x = %d, y = %d, z = %d, direction = %d)", coord.x, coord.y, coord.z,
            coord.direction);
        stream->Write(msg, strlen(msg));
    }
};

template<typename T, T TNull, typename TTag>
struct DataSerializerTraitsT<TIdentifier<T, TNull, TTag>>
{
    static void encode(OpenRCT2::IStream* stream, const TIdentifier<T, TNull, TTag>& id)
    {
        stream->WriteValue(ByteSwapBE(id.ToUnderlying()));
    }

    static void decode(OpenRCT2::IStream* stream, TIdentifier<T, TNull, TTag>& id)
    {
        auto temp = ByteSwapBE(stream->ReadValue<T>());
        id = TIdentifier<T, TNull, TTag>::FromUnderlying(temp);
    }

    static void log(OpenRCT2::IStream* stream, const TIdentifier<T, TNull, TTag>& id)
    {
        char msg[128] = {};
        snprintf(msg, sizeof(msg), "Id(%u)", static_cast<uint32_t>(id.ToUnderlying()));
        stream->Write(msg, strlen(msg));
    }
};

template<>
struct DataSerializerTraitsT<Banner>
{
    static void encode(OpenRCT2::IStream* stream, const Banner& banner)
    {
        DataSerializerTraits<BannerIndex>().encode(stream, banner.id);
        DataSerializerTraits<OpenRCT2::ObjectEntryIndex>().encode(stream, banner.type);
        stream->WriteValue(banner.flags);
        stream->WriteString(banner.text);
        stream->WriteValue(banner.colour);
        DataSerializerTraits<RideId>().encode(stream, banner.rideIndex);
        stream->WriteValue(banner.textColour);
        DataSerializerTraits<TileCoordsXY>().encode(stream, banner.position);
    }

    static void decode(OpenRCT2::IStream* stream, Banner& banner)
    {
        DataSerializerTraits<BannerIndex>().decode(stream, banner.id);
        DataSerializerTraits<OpenRCT2::ObjectEntryIndex>().decode(stream, banner.type);
        stream->ReadValue(banner.flags);
        banner.text = stream->ReadString();
        stream->ReadValue(banner.colour);
        DataSerializerTraits<RideId>().decode(stream, banner.rideIndex);
        stream->ReadValue(banner.textColour);
        DataSerializerTraits<TileCoordsXY>().decode(stream, banner.position);
    }

    static void log(OpenRCT2::IStream* stream, const Banner& banner)
    {
        char msg[128] = {};
        snprintf(
            msg, sizeof(msg), "Banner(x = %d, y = %d, text = %s)", banner.position.x, banner.position.y, banner.text.c_str());
        stream->Write(msg, strlen(msg));
    }
};

template<>
struct DataSerializerTraitsT<FootpathSlope>
{
    static void encode(OpenRCT2::IStream* stream, const FootpathSlope& slope)
    {
        uint8_t combined = (EnumValue(slope.type) << 2) | (slope.direction % kNumOrthogonalDirections);
        stream->WriteValue(combined);
    }

    static void decode(OpenRCT2::IStream* stream, FootpathSlope& slope)
    {
        auto combined = stream->ReadValue<uint8_t>();
        auto type = static_cast<FootpathSlopeType>(combined >> 2);
        Direction direction = combined & 0b00000011;
        slope = FootpathSlope{ type, direction };
    }

    static void log(OpenRCT2::IStream* stream, const FootpathSlope& slope)
    {
        char msg[128] = {};
        snprintf(msg, sizeof(msg), "FootpathSlope(type = %d, direction = %d)", EnumValue(slope.type), slope.direction);
        stream->Write(msg, strlen(msg));
    }
};
