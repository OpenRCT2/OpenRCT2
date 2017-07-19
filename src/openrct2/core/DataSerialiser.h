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

#include <type_traits>
#include "DataSerialiserTraits.h"

class DataSerialiser 
{
private:
    MemoryStream _stream;
    MemoryStream *_activeStream;
    bool _isSaving;

public:
    DataSerialiser(bool isSaving) : _isSaving(isSaving)
    {
        _activeStream = &_stream;
    }

    DataSerialiser(bool isSaving, MemoryStream& stream) : _isSaving(isSaving)
    {
        _activeStream = &stream;
    }

    bool IsSaving() const 
    {
        return _isSaving;
    }

    bool IsLoading() const 
    {
        return !_isSaving;
    }

    MemoryStream& GetStream() 
    {
        return _stream;
    }

    template<typename T>
    DataSerialiser& operator<<(T& data)
    {
        if (_isSaving)
            DataSerializerTraits<std::remove_const<T>::type>::encode(_activeStream, data);
        else 
            DataSerializerTraits<std::remove_const<T>::type>::decode(_activeStream, data);
        return *this;
    }
};