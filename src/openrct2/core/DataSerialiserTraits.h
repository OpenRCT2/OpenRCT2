/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Endianness.h"
#include "MemoryStream.h"

template<typename T>
struct DataSerializerTraits {
    static void encode(IStream *stream, const T& v) = delete;
    static void decode(IStream *stream, T& val) = delete;
};

template<typename T>
struct DataSerializerTraitsIntegral
{
    static void encode(IStream *stream, const T& val)
    {
        T temp = ByteSwapBE(val);
        stream->Write(&temp);
    }
    static void decode(IStream *stream, T& val)
    {
        T temp;
        stream->Read(&temp);
        val = ByteSwapBE(temp);
    }
};

template<>
struct DataSerializerTraits<bool> : public DataSerializerTraitsIntegral<bool> {};

template<>
struct DataSerializerTraits<uint8> : public DataSerializerTraitsIntegral<uint8> {};

template<>
struct DataSerializerTraits<sint8> : public DataSerializerTraitsIntegral<sint8> {};

template<>
struct DataSerializerTraits<uint16> : public DataSerializerTraitsIntegral<uint16> {};

template<>
struct DataSerializerTraits<sint16> : public DataSerializerTraitsIntegral<sint16> {};

template<>
struct DataSerializerTraits<uint32> : public DataSerializerTraitsIntegral<uint32> {};

template<>
struct DataSerializerTraits<sint32> : public DataSerializerTraitsIntegral<sint32> {};

template<>
struct DataSerializerTraits<std::string>
{
    static void encode(IStream *stream, const std::string& str)
    {
        uint16 len = (uint16)str.size();
        uint16 swapped = ByteSwapBE(len);
        stream->Write(&swapped);
        stream->WriteArray(str.c_str(), len);
    }
    static void decode(IStream *stream, std::string& res)
    {
        uint16 len;
        stream->Read(&len);
        len = ByteSwapBE(len);

        const char* str = stream->ReadArray<char>(len);
        res.assign(str, len);

        Memory::FreeArray(str, len);
    }
};
