/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2/common.h>
#include "OpenGLAPI.h"

#pragma pack(push, 1)

struct ivec2
{
    union { GLint x; GLint s; GLint r; };
    union { GLint y; GLint t; GLint g; };
};

struct vec2
{
    union { GLfloat x; GLfloat s; GLfloat r; };
    union { GLfloat y; GLfloat t; GLfloat g; };
};

struct ivec3
{
    union { GLint x; GLint s; GLint r; };
    union { GLint y; GLint t; GLint g; };
    union { GLint z; GLint p; GLint b; };
};

struct vec3f
{
    union { GLfloat x; GLfloat s; GLfloat r; };
    union { GLfloat y; GLfloat t; GLfloat g; };
    union { GLfloat z; GLfloat p; GLfloat b; };
};

struct ivec4
{
    union { GLint x; GLint s; GLint r; };
    union { GLint y; GLint t; GLint g; };
    union { GLint z; GLint p; GLint b; };
    union { GLint w; GLint q; GLint a; };
};

struct vec4
{
    union { GLfloat x; GLfloat s; GLfloat r; };
    union { GLfloat y; GLfloat t; GLfloat g; };
    union { GLfloat z; GLfloat p; GLfloat b; };
    union { GLfloat w; GLfloat q; GLfloat a; };
};

#pragma pack(pop)
