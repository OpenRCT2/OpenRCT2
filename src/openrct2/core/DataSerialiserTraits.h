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
