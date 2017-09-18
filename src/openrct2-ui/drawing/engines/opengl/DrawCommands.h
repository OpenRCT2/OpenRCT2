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
    void reset()
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
    size_t size() const
    {
        return _numInstances;
    }
    const T* data() const
    {
        return &_instances.at(0);
    }
    const T& operator[](size_t idx) const
    {
        return _instances.at(idx);
    }
};

struct DrawRectCommand {
    uint32 flags;
    GLuint sourceFramebuffer;
    sint32 clip[4];
    sint32 bounds[4];
    GLint paletteRemap[256];
};

struct DrawLineCommand {
    vec4f colour;
    sint32 clip[4];
    sint32 pos[4];
};

// Per-instance data for images
struct DrawImageCommand {
    vec4i clip;
    sint32 texColourAtlas;
    vec4f texColourBounds;
    sint32 texMaskAtlas;
    vec4f texMaskBounds;
    sint32 texPaletteAtlas;
    vec4f texPaletteBounds;
    sint32 flags;
    vec4f colour;
    vec4i bounds;
    sint32 mask;

    enum
    {
        FLAG_COLOUR = (1 << 0),
        FLAG_REMAP = (1 << 1),
        FLAG_TRANSPARENT = (1 << 2),
        FLAG_TRANSPARENT_SPECIAL = (1 << 3),
    };
};

typedef CommandBatch<DrawImageCommand> ImageCommandBatch;
typedef CommandBatch<DrawLineCommand> LineCommandBatch;
typedef CommandBatch<DrawRectCommand> RectCommandBatch;
