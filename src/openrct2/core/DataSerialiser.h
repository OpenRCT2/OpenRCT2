/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

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
            DataSerializerTraits<T>::encode(_activeStream, data);
        else 
            DataSerializerTraits<T>::decode(_activeStream, data);
        return *this;
    }
};
