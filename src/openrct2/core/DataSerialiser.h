/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "DataSerialiserTraits.h"

#include <type_traits>

class DataSerialiser
{
private:
    MemoryStream _stream;
    IStream* _activeStream = nullptr;
    bool _isSaving = false;
    bool _isLogging = false;

public:
    DataSerialiser(bool isSaving)
        : _isSaving(isSaving)
        , _isLogging(false)
    {
        _activeStream = &_stream;
    }

    DataSerialiser(bool isSaving, IStream& stream, bool isLogging = false)
        : _isSaving(isSaving)
        , _isLogging(isLogging)
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

    template<typename T> DataSerialiser& operator<<(T& data)
    {
        if (!_isLogging)
        {
            if (_isSaving)
                DataSerializerTraits<T>::encode(_activeStream, data);
            else
                DataSerializerTraits<T>::decode(_activeStream, data);
        }
        else
        {
            DataSerializerTraits<T>::log(_activeStream, data);
        }

        return *this;
    }

    template<typename T> DataSerialiser& operator<<(DataSerialiserTag<T> data)
    {
        if (!_isLogging)
        {
            if (_isSaving)
                DataSerializerTraits<DataSerialiserTag<T>>::encode(_activeStream, data);
            else
                DataSerializerTraits<DataSerialiserTag<T>>::decode(_activeStream, data);
        }
        else
        {
            DataSerializerTraits<DataSerialiserTag<T>>::log(_activeStream, data);
        }

        return *this;
    }
};
