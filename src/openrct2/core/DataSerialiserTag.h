/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

template<typename T>
class DataSerialiserTag
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

template<typename T>
inline DataSerialiserTag<T> CreateDataSerialiserTag(const char* name, T& data)
{
    DataSerialiserTag<T> r(name, data);
    return r;
}

#define DS_TAG(var) CreateDataSerialiserTag(#var, var)
