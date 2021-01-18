/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "DataSerialiserTraits.h"

template<typename T> class DataSerialiserTag
{
public:
    DataSerialiserTag(const char* name, T& data)
        : _name(name)
        , _data(data)
    {
    }

    const char* Name() const
    {
        return _name;
    }

    T& Data() const
    {
        return _data;
    }

private:
    const char* _name = nullptr;
    T& _data;
};

template<typename T> inline DataSerialiserTag<T> CreateDataSerialiserTag(const char* name, T& data)
{
    DataSerialiserTag<T> r(name, data);
    return r;
}

#define DS_TAG(var) CreateDataSerialiserTag(#var, var)

template<typename T> struct DataSerializerTraits_t<DataSerialiserTag<T>>
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
