#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#include <openrct2/common.h>
#include <vector>
#include "OpenGLAPI.h"
#include "GLSLTypes.h"
#include "TextureCache.h"

template<typename T>
class CommandBatch
{
private:
    std::vector<T> _instances;
    size_t _numInstances;

public:
    CommandBatch()
        : _numInstances(0)
    {
    }
    bool empty() const
    {
        return _numInstances == 0;
    }
    void clear()
    {
        _numInstances = 0;
    }
    T& allocate()
    {
        if (_numInstances + 1 > _instances.size())
        {
            _instances.resize((_numInstances + 1) << 1);
        }
        return _instances[_numInstances++];
    }
    T& insert(const T &value)
    {
        if (_numInstances + 1 > _instances.size())
        {
            _instances.resize((_numInstances + 1) << 1);
        }
        return _instances[_numInstances++] = value;
    }
    size_t size() const
    {
        return _numInstances;
    }
    const T* data() const
    {
        return _instances.data();
    }
    const T& operator[](size_t idx) const
    {
        return _instances.at(idx);
    }

    typename std::vector<T>::iterator begin()
    {
        return _instances.begin();
    }
    typename std::vector<T>::const_iterator begin() const
    {
        return _instances.cbegin();
    }
    typename std::vector<T>::const_iterator cbegin() const
    {
        return _instances.cbegin();
    }
    typename std::vector<T>::iterator end()
    {
        return _instances.begin() + _numInstances;
    }
    typename std::vector<T>::const_iterator end() const
    {
        return _instances.cbegin() + _numInstances;
    }
    typename std::vector<T>::const_iterator cend() const
    {
        return _instances.cbegin() + _numInstances;
    }
};

struct DrawLineCommand
{
    ivec4 clip;
    ivec4 bounds;
    GLuint colour;
    GLint depth;
};

// Per-instance data for images
struct DrawRectCommand
{
    ivec4 clip;
    GLint texColourAtlas;
    vec4 texColourBounds;
    GLint texMaskAtlas;
    vec4 texMaskBounds;
    ivec3 palettes;
    GLint flags;
    GLuint colour;
    ivec4 bounds;
    GLint depth;

    enum
    {
        FLAG_NO_TEXTURE = (1 << 2),
        FLAG_MASK = (1 << 3),
        FLAG_CROSS_HATCH = (1 << 4),
    };
};

using LineCommandBatch = CommandBatch<DrawLineCommand>;
using RectCommandBatch = CommandBatch<DrawRectCommand>;
