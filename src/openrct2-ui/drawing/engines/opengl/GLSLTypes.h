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
